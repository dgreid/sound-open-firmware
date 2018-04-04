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
 *         Keyon Jie <yang.jie@linux.intel.com>
 */

#ifndef __PLATFORM_DMA_H__
#define __PLATFORM_DMA_H__

#include <stdint.h>
#include <sof/io.h>
#include <arch/cache.h>

/* available DMACs */
#define DMA_GP_LP_DMAC0		0
#define DMA_GP_LP_DMAC1		1
#define DMA_GP_HP_DMAC0		2
#define DMA_GP_HP_DMAC1		3
#define DMA_HOST_IN_DMAC	4
#define DMA_HOST_OUT_DMAC	5
#define DMA_LINK_IN_DMAC	6
#define DMA_LINK_OUT_DMAC	7

/* mappings - TODO improve API to get type */
#define DMA_ID_DMAC0		DMA_HOST_IN_DMAC
#define DMA_ID_DMAC1		DMA_GP_LP_DMAC0
#define DMA_ID_DMAC2		DMA_HOST_OUT_DMAC
#define DMA_ID_DMAC3		DMA_GP_HP_DMAC0
#define DMA_ID_DMAC4		DMA_GP_LP_DMAC1
#define DMA_ID_DMAC5		DMA_GP_HP_DMAC1
#define DMA_ID_DMAC6		DMA_LINK_IN_DMAC
#define DMA_ID_DMAC7		DMA_LINK_OUT_DMAC

/* handshakes */
#define DMA_HANDSHAKE_DMIC_CH0	0
#define DMA_HANDSHAKE_DMIC_CH1	1
#define DMA_HANDSHAKE_SSP0_TX	2
#define DMA_HANDSHAKE_SSP0_RX	3
#define DMA_HANDSHAKE_SSP1_TX	4
#define DMA_HANDSHAKE_SSP1_RX	5
#define DMA_HANDSHAKE_SSP2_TX	6
#define DMA_HANDSHAKE_SSP2_RX	7
#define DMA_HANDSHAKE_SSP3_TX	8
#define DMA_HANDSHAKE_SSP3_RX	9
#define DMA_HANDSHAKE_SSP4_TX	10
#define DMA_HANDSHAKE_SSP4_RX	11
#define DMA_HANDSHAKE_SSP5_TX	12
#define DMA_HANDSHAKE_SSP5_RX	13

#endif
