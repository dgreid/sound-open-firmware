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

#ifndef __PLATFORM_PMC_H__
#define __PLATFORM_PMC_H__

#include <stdint.h>
#include <config.h>

/* messages */
#define PMC_DDR_LINK_UP		0xc0	/* LPE req path to DRAM is up */
#define PMC_DDR_LINK_DOWN	0xc1	/* LPE req path to DRAM is down */
#define PMC_SET_LPECLK		0xc2	/* LPE req clock change to FR_LAT_REQ */

#ifdef CONFIG_BAYTRAIL

#define PMC_SET_SSP_19M2	0xc5	/* LPE req SSP clock to 19.2MHz w/ PLL*/
#define PMC_SET_SSP_25M		0xc6	/* LPE req SSP clock to 25MHz w/ XTAL */

#elif CONFIG_CHERRYTRAIL

#define PMC_SET_SSP_25M		0xc5	/* LPE req SSP clock to 25MHz w/ PLL */
#define PMC_SET_SSP_19M2	0xc6	/* LPE req SSP clock to 19.2MHz w/ XTAL*/
#endif

int platform_ipc_pmc_init(void);
int ipc_pmc_send_msg(uint32_t message);
int pmc_process_msg_queue(void);

#endif
