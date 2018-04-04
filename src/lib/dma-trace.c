/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Intel Corporation nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Yan Wang <yan.wang@linux.intel.com>
 */

#include <sof/trace.h>
#include <sof/dma-trace.h>
#include <sof/ipc.h>
#include <sof/sof.h>
#include <sof/alloc.h>
#include <arch/cache.h>
#include <platform/timer.h>
#include <platform/dma.h>
#include <platform/platform.h>
#include <sof/lock.h>
#include <stdint.h>

static struct dma_trace_data *trace_data = NULL;

static uint64_t trace_work(void *data, uint64_t delay)
{
	struct dma_trace_data *d = (struct dma_trace_data *)data;
	struct dma_trace_buf *buffer = &d->dmatb;
	struct dma_sg_config *config = &d->config;
	unsigned long flags;
	uint32_t avail = buffer->avail;
	int32_t size;
	uint32_t hsize;
	uint32_t lsize;

	if (d->host_offset == d->host_size)
		d->host_offset = 0;

#if defined CONFIG_DMA_GW
	/*
	 * there isn't DMA completion callback in GW DMA copying.
	 * so we send previous position always before the next copying
	 * for guaranteeing previous DMA copying is finished.
	 * This function will be called once every 500ms at least even
	 * if no new trace is filled.
	 */
	if (d->old_host_offset != d->host_offset) {
		ipc_dma_trace_send_position();
		d->old_host_offset = d->host_offset;
	}
#endif

	/* any data to copy ? */
	if (avail == 0)
		return DMA_TRACE_PERIOD;

	/* DMA trace copying is working */
	d->copy_in_progress = 1;

	/* make sure we dont write more than buffer */
	if (avail > DMA_TRACE_LOCAL_SIZE) {
		d->overflow = avail - DMA_TRACE_LOCAL_SIZE;
		avail = DMA_TRACE_LOCAL_SIZE;
	} else {
		d->overflow = 0;
	}

	/* copy to host in sections if we wrap */
	lsize = hsize = avail;

	/* host buffer wrap ? */
	if (d->host_offset + avail > d->host_size)
		hsize = d->host_size - d->host_offset;

	/* local buffer wrap ? */
	if (buffer->r_ptr + avail > buffer->end_addr)
		lsize = buffer->end_addr - buffer->r_ptr;

	/* get smallest size */
	if (hsize < lsize)
		size = hsize;
	else
		size = lsize;

	/* writeback trace data */
	dcache_writeback_region((void*)buffer->r_ptr, size);

	/* copy this section to host */
	size = dma_copy_to_host_nowait(&d->dc, config, d->host_offset,
		buffer->r_ptr, size);
	if (size < 0) {
		trace_buffer_error("ebb");
		goto out;
	}

	/* update host pointer and check for wrap */
	d->host_offset += size;

	/* update local pointer and check for wrap */
	buffer->r_ptr += size;
	if (buffer->r_ptr == buffer->end_addr)
		buffer->r_ptr = buffer->addr;

out:
	spin_lock_irq(&d->lock, flags);

	/* disregard any old messages and dont resend them if we overflow */
	if (size > 0) {
		if (d->overflow)
			buffer->avail = DMA_TRACE_LOCAL_SIZE - size;
		else
			buffer->avail -= size;
	}

	/* DMA trace copying is done, allow reschedule */
	d->copy_in_progress = 0;

	spin_unlock_irq(&d->lock, flags);

	/* reschedule the trace copying work */
	return DMA_TRACE_PERIOD;
}

int dma_trace_init_early(struct sof *sof)
{
	struct dma_trace_buf *buffer;

	trace_data = rzalloc(RZONE_SYS, SOF_MEM_CAPS_RAM, sizeof(*trace_data));
	buffer = &trace_data->dmatb;

	/* allocate new buffer */
	buffer->addr = rballoc(RZONE_RUNTIME,
			       SOF_MEM_CAPS_RAM | SOF_MEM_CAPS_DMA,
			       DMA_TRACE_LOCAL_SIZE);
	if (buffer->addr == NULL) {
		trace_buffer_error("ebm");
		return -ENOMEM;
	}

	bzero(buffer->addr, DMA_TRACE_LOCAL_SIZE);

	/* initialise the DMA buffer */
	buffer->size = DMA_TRACE_LOCAL_SIZE;
	buffer->w_ptr = buffer->r_ptr = buffer->addr;
	buffer->end_addr = buffer->addr + buffer->size;
	buffer->avail = 0;

	list_init(&trace_data->config.elem_list);
	spinlock_init(&trace_data->lock);
	sof->dmat = trace_data;

	return 0;
}

int dma_trace_init_complete(struct dma_trace_data *d)
{
	struct dma_trace_buf *buffer = &d->dmatb;
	int ret;

	trace_buffer("dtn");

	/* init DMA copy context */
	ret = dma_copy_new(&d->dc, PLATFORM_TRACE_DMAC);
	if (ret < 0) {
		trace_buffer_error("edm");
		rfree(buffer->addr);
		return ret;
	}

	work_init(&d->dmat_work, trace_work, d, WORK_ASYNC);

	return 0;
}

int dma_trace_host_buffer(struct dma_trace_data *d, struct dma_sg_elem *elem,
		uint32_t host_size)
{
	struct dma_sg_elem *e;

	/* allocate new host DMA elem and add it to our list */
	e = rzalloc(RZONE_RUNTIME, SOF_MEM_CAPS_RAM, sizeof(*e));
	if (e == NULL)
		return -ENOMEM;

	/* copy fields - excluding possibly non-initialized elem->src */
	e->dest = elem->dest;
	e->size = elem->size;

	d->host_size = host_size;

	list_item_append(&e->list, &d->config.elem_list);
	return 0;
}

#if defined CONFIG_DMA_GW

static int dma_trace_start(struct dma_trace_data *d)
{
	struct dma_sg_config config;
	struct dma_sg_elem *e;
	uint32_t elem_size, elem_addr, elem_num;
	int err = 0;
	int i;

	err = dma_copy_set_stream_tag(&d->dc, d->stream_tag);
	if (err < 0)
		return err;

	/* size of every trace record */
	elem_size = sizeof(uint64_t) * 2;

	/* Initialize address of local elem */
	elem_addr = (uint32_t)d->dmatb.addr;

	/* the number of elem list */
	elem_num = DMA_TRACE_LOCAL_SIZE / elem_size;

	config.direction = DMA_DIR_LMEM_TO_HMEM;
	config.src_width = sizeof(uint32_t);
	config.dest_width = sizeof(uint32_t);
	config.cyclic = 0;
	list_init(&config.elem_list);

	/* generate local elem list for local trace buffer */
	e = rzalloc(RZONE_SYS, SOF_MEM_CAPS_RAM, sizeof(*e) * elem_num);
	if (!e)
		return -ENOMEM;

	for (i = 0; i < elem_num; i++) {
		e[i].dest = 0;
		e[i].src = elem_addr;
		e[i].size = elem_size; /* the minimum size of DMA copy */

		list_item_append(&e[i].list, &config.elem_list);
		elem_addr += elem_size;
	}

	err = dma_set_config(d->dc.dmac, d->dc.chan, &config);
	if (err < 0) {
		rfree(e);
		return err;
	}

	err = dma_start(d->dc.dmac, d->dc.chan);

	rfree(e);
	return err;
}

#endif

int dma_trace_enable(struct dma_trace_data *d)
{
#if defined CONFIG_DMA_GW
	int err;

	/*
	 * GW DMA need finish DMA config and start before
	 * host driver trigger start DMA
	 */
	err = dma_trace_start(d);
	if (err < 0)
		return err;
#endif

	/* validate DMA context */
	if (d->dc.dmac == NULL || d->dc.chan < 0) {
		trace_error_atomic(TRACE_CLASS_BUFFER, "eem");
		return -ENODEV;
	}

	d->enabled = 1;
	work_schedule_default(&d->dmat_work, DMA_TRACE_PERIOD);
	return 0;
}

void dma_trace_flush(void *t)
{
	struct dma_trace_buf *buffer = &trace_data->dmatb;
	uint32_t avail = buffer->avail;
	int32_t size;
	int32_t wrap_count;

	/* number of bytes to flush */
	if (avail > DMA_FLUSH_TRACE_SIZE) {
		size = DMA_FLUSH_TRACE_SIZE;
	} else {
		/* check for buffer wrap */
		if (buffer->w_ptr > buffer->r_ptr)
			size = buffer->w_ptr - buffer->r_ptr;
		else
			size = buffer->end_addr - buffer->r_ptr +
				buffer->w_ptr - buffer->addr;
	}

	/* check for buffer wrap */
	if (buffer->w_ptr - size < buffer->addr) {
		wrap_count = buffer->w_ptr - buffer->addr;
		memcpy(t, buffer->end_addr - (size - wrap_count),
		       size - wrap_count);
		memcpy(t + (size - wrap_count), buffer->addr,
		       wrap_count);
	} else {
		memcpy(t, buffer->w_ptr - size, size);
	}

	/* writeback trace data */
	dcache_writeback_region(t, size);
}

static void dtrace_add_event(const char *e, uint32_t length)
{
	struct dma_trace_buf *buffer = &trace_data->dmatb;
	int margin;

	margin = buffer->end_addr - buffer->w_ptr;

	/* check for buffer wrap */
	if (margin > length) {
		/* no wrap */
		memcpy(buffer->w_ptr, e, length);
		buffer->w_ptr += length;
	} else {

		/* data is bigger than remaining margin so we wrap */
		memcpy(buffer->w_ptr, e, margin);
		buffer->w_ptr = buffer->addr;

		memcpy(buffer->w_ptr, e + margin, length - margin);
		buffer->w_ptr += length - margin;
	}

	buffer->avail += length;
	trace_data->messages++;
}

void dtrace_event(const char *e, uint32_t length)
{
	struct dma_trace_buf *buffer = NULL;
	unsigned long flags;

	if (trace_data == NULL ||
		length > DMA_TRACE_LOCAL_SIZE / 8 || length == 0)
		return;

	buffer = &trace_data->dmatb;

	spin_lock_irq(&trace_data->lock, flags);
	dtrace_add_event(e, length);

	/* if DMA trace copying is working */
	/* don't check if local buffer is half full */
	if (trace_data->copy_in_progress) {
		spin_unlock_irq(&trace_data->lock, flags);
		return;
	}

	spin_unlock_irq(&trace_data->lock, flags);

	/* schedule copy now if buffer > 50% full */
	if (trace_data->enabled &&
	    buffer->avail >= (DMA_TRACE_LOCAL_SIZE / 2)) {
		work_reschedule_default(&trace_data->dmat_work,
		DMA_TRACE_RESCHEDULE_TIME);
		/* reschedule should not be intrrupted */
		/* just like we are in copy progress */
		trace_data->copy_in_progress = 1;
	}

}

void dtrace_event_atomic(const char *e, uint32_t length)
{
	if (trace_data == NULL ||
		length > DMA_TRACE_LOCAL_SIZE / 8 || length == 0)
		return;

	dtrace_add_event(e, length);
}
