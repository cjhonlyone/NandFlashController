/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "platform.h"
//#include "platform_fs.h"
#include "platform_config.h"
#if __MICROBLAZE__ || __PPC__
#include "xenv_standalone.h"
#include "xparameters.h"
#include "xintc.h"
#endif
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif

#ifdef PLATFORM_STDOUT_IS_16550
#include "xuartns550_l.h"
#endif
#ifdef __arm__
#include "xil_types.h"
#include "xil_io.h"
#include "xil_assert.h"
#include "xparameters.h"
#include "stdio.h"
#include "sleep.h"
#include "xparameters.h"
#include "xparameters_ps.h"	/* defines XPAR values */
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xuartps.h"
#include "xscugic.h"
#include "xscutimer.h"
#include "xemacps.h"		/* defines XEmacPs API */
#endif

void
enable_caches()
{
#ifdef __PPC__
    XCache_EnableICache(CACHEABLE_REGION_MASK);
    XCache_EnableDCache(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE 
    microblaze_invalidate_icache();
    microblaze_enable_icache();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE 
    microblaze_invalidate_dcache();
    microblaze_enable_dcache();
#endif
#endif
}

void
disable_caches()
{
#ifdef __PPC__
    XCache_DisableDCache();
    XCache_DisableICache();
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE 
#if !XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK
    microblaze_invalidate_dcache();
#endif
    microblaze_disable_dcache();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE 
    microblaze_invalidate_icache();
    microblaze_disable_icache();
#endif
#endif
}

int 
init_platform()
{
#if __MICROBLAZE__ || __PPC__
	enable_caches();

#ifdef PLATFORM_STDOUT_IS_16550
    /* if we have a uart 16550, then that needs to be initialized */
    XUartNs550_SetBaud(PLATFORM_STDOUT_BASEADDR, XPAR_XUARTNS550_CLOCK_HZ, PLATFORM_BAUDRATE);
    XUartNs550_mSetLineControlReg(PLATFORM_STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
#endif

    /* initialize file system layer */
    if (platform_init_fs() < 0)
        return -1;
#endif
#ifdef __arm__
	/* initialize file system layer */
//	if (platform_init_fs() < 0)
//            return -1;
#endif
    return 0;
}

void cleanup_platform()
{
#if __MICROBLAZE__ || __PPC__
	disable_caches();
#endif
#ifdef __arm__
        Xil_ICacheDisable();
        Xil_DCacheDisable();
#endif
}


