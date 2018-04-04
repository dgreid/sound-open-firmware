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
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#ifndef __INCLUDE_INTERRUPT_MAP__
#define __INCLUDE_INTERRUPT_MAP__

#include <config.h>

#define SOF_IRQ_BIT_SHIFT	24
#define SOF_IRQ_LEVEL_SHIFT	16
#define SOF_IRQ_CPU_SHIFT	8
#define SOF_IRQ_NUM_SHIFT	0
#define SOF_IRQ_NUM_MASK	0xff
#define SOF_IRQ_LEVEL_MASK	0xff
#define SOF_IRQ_BIT_MASK	0x1f
#define SOF_IRQ_CPU_MASK	0xff

#define SOF_IRQ(_bit, _level, _cpu, _number) \
	(((_bit) << SOF_IRQ_BIT_SHIFT)	      \
	 | ((_level) << SOF_IRQ_LEVEL_SHIFT) \
	 | ((_cpu) << SOF_IRQ_CPU_SHIFT)     \
	 | ((_number) << SOF_IRQ_NUM_SHIFT))

#ifdef CONFIG_IRQ_MAP
/*
 * IRQs are mapped on 4 levels.
 *
 * 1. Peripheral Register bit offset.
 * 2. CPU interrupt level.
 * 3. CPU number.
 * 4. CPU interrupt number.
 */
#define SOF_IRQ_NUMBER(_irq) \
	(((_irq) >> SOF_IRQ_NUM_SHIFT) & SOF_IRQ_NUM_MASK)
#define SOF_IRQ_LEVEL(_level) \
	(((_level) >> SOF_IRQ_LEVEL_SHIFT) & SOF_IRQ_LEVEL_MASK)
#define SOF_IRQ_BIT(_bit) \
	(((_bit) >> SOF_IRQ_BIT_SHIFT) & SOF_IRQ_BIT_MASK)
#define SOF_IRQ_CPU(_cpu) \
	(((_cpu) >> SOF_IRQ_CPU_SHIFT) & SOF_IRQ_CPU_MASK)
#else
/*
 * IRQs are directly mapped onto a single level, bit and level.
 */
#define SOF_IRQ_NUMBER(_irq)	(_irq)
#define SOF_IRQ_LEVEL(_level)	0
#define SOF_IRQ_BIT(_bit)	0
#define SOF_IRQ_CPU(_cpu)	0
#endif

#endif
