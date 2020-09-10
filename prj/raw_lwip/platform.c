/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
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
#include "arch/cc.h"
#include "lwipopts.h"
#include "platform.h"
#include "platform_config.h"
#if __MICROBLAZE__ || __PPC__
#include "xenv_standalone.h"
#include "xparameters.h"
#include "xintc.h"
#include "xil_exception.h"
#endif
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#include "xtmrctr_l.h"
#elif __PPC__
#include "xexception_l.h"
#include "xtime_l.h"
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

int platform_init_fs();
#if LWIP_DHCP==1
volatile int dhcp_timoutcntr = 24;
#endif

#ifdef PLATFORM_STDOUT_IS_16550
#include "xuartns550_l.h"
#endif

#include "lwip/tcp.h"

#if LWIP_DHCP==1
void dhcp_fine_tmr();
void dhcp_coarse_tmr();
#endif

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

#ifdef __MICROBLAZE__
#if XPAR_INTC_0_HAS_FAST == 1
static void xadapter_fasttimer_handler(void) __attribute__ ((fast_interrupt));
#endif
#endif

#if __MICROBLAZE__ || __PPC__
volatile int TxPerfConnMonCntr = 0;
void
timer_callback()
{
    static int odd = 1;
#if LWIP_DHCP==1
    static int dhcp_timer = 0;
#endif
    TcpFastTmrFlag = 1;

	odd = !odd;
	if (odd) {
		TxPerfConnMonCntr++;
#if LWIP_DHCP==1
		dhcp_timer++;
		dhcp_timoutcntr--;
#endif
		TcpSlowTmrFlag = 1;
#if LWIP_DHCP==1
		dhcp_fine_tmr();
		if (dhcp_timer >= 120) {
			dhcp_coarse_tmr();
			dhcp_timer = 0;
		}
#endif
	}
}

#ifdef __MICROBLAZE__

#if XPAR_INTC_0_HAS_FAST == 1
void xadapter_fasttimer_handler(void)
{
	timer_callback();
	/* Load timer, clear interrupt bit */
		XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK);
		XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
					XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
					| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

}
#else
void xadapter_timer_handler(void *p)
{
	timer_callback();
	/* Load timer, clear interrupt bit */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK);
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
				XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
				| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);
    /* Clear interrupt bit */
	XIntc_AckIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}
#endif

#define TIMER_TLR (XPAR_TMRCTR_0_CLOCK_FREQ_HZ / 4)
void
platform_setup_timer()
{
	/* set the number of cycles the timer counts before interrupting */
	/* 100 Mhz clock => .01us for 1 clk tick. For 100ms, 10000000 clk ticks need to elapse  */
	XTmrCtr_SetLoadReg(PLATFORM_TIMER_BASEADDR, 0, TIMER_TLR);

	/* reset the timers, and clear interrupts */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0, XTC_CSR_INT_OCCURED_MASK | XTC_CSR_LOAD_MASK );

	/* start the timers */
	XTmrCtr_SetControlStatusReg(PLATFORM_TIMER_BASEADDR, 0,
			XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK
			| XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

#if XPAR_INTC_0_HAS_FAST == 1
	XIntc_RegisterFastHandler(XPAR_INTC_0_BASEADDR,
			PLATFORM_TIMER_INTERRUPT_INTR,
				(XFastInterruptHandler)xadapter_fasttimer_handler);
#else
	/* Register Timer handler */
	XIntc_RegisterHandler(XPAR_INTC_0_BASEADDR,
			PLATFORM_TIMER_INTERRUPT_INTR,
			(XInterruptHandler)xadapter_timer_handler,
			0);
#endif

	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR, PLATFORM_TIMER_INTERRUPT_MASK);
}
#else
#define MHZ 400
#define PIT_INTERVAL (250*MHZ*1000)
void
xadapter_timer_handler(void *p)
{
	timer_callback();

	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
}

void
platform_setup_timer()
{
#ifdef XPAR_CPU_PPC440_CORE_CLOCK_FREQ_HZ
        XExc_RegisterHandler(XEXC_ID_DEC_INT, (XExceptionHandler)xadapter_timer_handler, NULL);

        /* Set DEC to interrupt every 250 mseconds */
        XTime_DECSetInterval(PIT_INTERVAL);
        XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
        XTime_DECEnableAutoReload();
#else
	XExc_RegisterHandler(XEXC_ID_PIT_INT, (XExceptionHandler)xadapter_timer_handler, NULL);

	/* Set PIT to interrupt every 250 mseconds */
	XTime_PITSetInterval(PIT_INTERVAL);
	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);
	XTime_PITEnableAutoReload();
	XTime_PITEnableInterrupt();
#endif
}
#endif

void platform_enable_interrupts()
{
	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnable();
}

static XIntc intc;

void platform_setup_interrupts()
{
	XIntc *intcp;
	intcp = &intc;

	XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);
	XIntc_Start(intcp, XIN_REAL_MODE);


	platform_setup_timer();

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				intcp);


#ifdef XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK
	/* Enable timer and EMAC interrupts in the interrupt controller */
	XIntc_EnableIntr(XPAR_XPS_INTC_0_BASEADDR,
#ifdef __MICROBLAZE__
			PLATFORM_TIMER_INTERRUPT_MASK |
#endif
		        XPAR_ETHERNET_MAC_IP2INTC_IRPT_MASK);
#endif



#ifdef XPAR_INTC_0_LLTEMAC_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_LLTEMAC_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_AXIETHERNET_0_VEC_ID
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
	XIntc_Enable(intcp, XPAR_INTC_0_AXIETHERNET_0_VEC_ID);
#endif


#ifdef XPAR_INTC_0_EMACLITE_0_VEC_ID
#ifdef __MICROBLAZE__
	XIntc_Enable(intcp, PLATFORM_TIMER_INTERRUPT_INTR);
#endif
	XIntc_Enable(intcp, XPAR_INTC_0_EMACLITE_0_VEC_ID);
#endif
}

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
#endif

#ifdef __arm__
#define EMACPS_DEVICE_ID   0
#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_DEVICE_ID      XPAR_XUARTPS_0_DEVICE_ID
#define TIMER_DEVICE_ID		XPAR_SCUTIMER_DEVICE_ID
#define EMACPS_IRPT_INTR   XPS_GEM0_INT_ID
#define INTC_BASE_ADDR		XPAR_SCUGIC_CPU_BASEADDR
#define INTC_DIST_BASE_ADDR	XPAR_SCUGIC_DIST_BASEADDR
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR

static XScuTimer TimerInstance;
static XUartPs Uart_Pss_0;	/* The instance of the UART 0 Driver */
volatile int TxPerfConnMonCntr = 0;


void
timer_callback(XScuTimer * TimerInstance)
{
#if LWIP_DHCP==1
    static int dhcp_timer = 0;
#endif
	/* we need to call tcp_fasttmr & tcp_slowtmr at intervals specified by lwIP.
	 * It is not important that the timing is absoluetly accurate.
	 */
	static int odd = 1;
	TcpFastTmrFlag = 1;
	odd = !odd;
	if (odd) {
#if LWIP_DHCP==1
		dhcp_timer++;
		dhcp_timoutcntr--;
#endif
		TxPerfConnMonCntr++;
		TcpSlowTmrFlag = 1;
#if LWIP_DHCP==1
		dhcp_fine_tmr();
		if (dhcp_timer >= 120) {
			dhcp_coarse_tmr();
			dhcp_timer = 0;
		}
#endif
	}
	XScuTimer_ClearInterruptStatus(TimerInstance);
}
int Init_ScuTimer(void)
{
	int Status = XST_SUCCESS;
	XScuTimer_Config *ConfigPtr;
	int TimerLoadValue = 0;

	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
	Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr,
			ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {

		xil_printf("In %s: Scutimer Cfg initialization failed...\r\n", __func__);

		return XST_FAILURE;
	}

	Status = XScuTimer_SelfTest(&TimerInstance);
	if (Status != XST_SUCCESS) {

		xil_printf("In %s: Scutimer Self test failed...\r\n", __func__);

		return XST_FAILURE;
	}

	XScuTimer_EnableAutoReload(&TimerInstance);
	/*
	 * Set for 250 milli seconds timeout.
	 */
	TimerLoadValue = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8;

	XScuTimer_LoadTimer(&TimerInstance, TimerLoadValue);
	return XST_SUCCESS;
}

int Init_UART(void)
{
	int Status = XST_SUCCESS;
	XUartPs_Config *Config_0;

	/*
	 * Initialize the UART 0 and 1 driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config_0 = XUartPs_LookupConfig(UART_DEVICE_ID);
	if (NULL == Config_0) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_Pss_0, Config_0, Config_0->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartPs_SetBaudRate(&Uart_Pss_0, 9600);
	return XST_SUCCESS;
}

int SetupIntrSystem( XScuTimer * TimerInstancePtr,
		u16 TimerIntrId)
{
	Xil_ExceptionInit();

	XScuGic_DeviceInitialize(INTC_DEVICE_ID);

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
     * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
						(Xil_ExceptionHandler)XScuGic_DeviceInterruptHandler,
						(void *)INTC_DEVICE_ID);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	XScuGic_RegisterHandler(INTC_BASE_ADDR,
							TimerIntrId,
							(Xil_ExceptionHandler)timer_callback,
							(void *)&TimerInstance);
	/*
	 * Enable the interrupt for scu timer.
	 */
	XScuGic_EnableIntr(INTC_DIST_BASE_ADDR, TimerIntrId);
	return XST_SUCCESS;
}

void platform_enable_interrupts()
{
	/*
	 * Enable non-critical exceptions.
	 */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
	XScuTimer_EnableInterrupt(&TimerInstance);
	XScuTimer_Start(&TimerInstance);
}

#endif

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

	platform_setup_interrupts();

	/* initialize file system layer */
	if (platform_init_fs() < 0)
            return -1;
#endif
#ifdef __arm__

	if (Init_ScuTimer()  != XST_SUCCESS) while(1);

	SetupIntrSystem(&TimerInstance, TIMER_IRPT_INTR);

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
