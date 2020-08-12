/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "xil_cache.h"
#include "xil_io.h"

#include "xparameters.h"		/* XPAR parameters */
#include "xusbps.h"			/* USB controller driver */
#include "xscugic.h"
#include "xtime_l.h"

#include "xpseudo_asm.h"
#include "xreg_cortexa9.h"

#include "xusbps_cdc.h"
#include "xusbps_cdc_irq.h"		/* USB interrupt processing code */
#include "xusbps_cdc_ch9.h"		/* Generic Chapter 9 handling code */
#include "xusbps_cdc_ch9_cdc.h"	/* Storage class handling code */

#include "sleep.h"

#include "NFC.h"

#define INT_CFG0_OFFSET 0x00000C00
#define INT_TYPE_RISING_EDGE 0x03
#define INT_TYPE_HIGHLEVEL 0x01
#define INT_TYPE_MASK 0x03

#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID

static int IntcInitFunction(u16 DeviceId, XUsbPs *usb);
static int setup_interrupts(XScuGic *intc);
static void reset_usb(void);

static XScuGic INTCInst;

uint64_t warray[2048] = {0};
uint64_t rarray[2048] = {0};
uint16_t *wptr = (uint16_t *)warray;
uint16_t *rptr = (uint16_t *)rarray;

#define PAGE_SIZE 8192
#define Block 128

//1 page = (4K + 224) bytes
//1 block = 128 pages = (512K + 28K) bytes
//1 plane = 2048 block = 8640 Mb $\approx 1GB $
//1 LUN = 2 planes = 17280 Mb

void test_WRconsistent()
{
    uint16_t j = 0;
    uint32_t m ,k ,l;
    m = 0;
    k=0;
	u32 way = 1;

	//erase first block
    eraseblock_60h_d0h(way,0);
    usleep(5000);


    for (m = 0;m<128;m++)
    {
    	wptr = (uint16_t *)warray;
    	for (uint32_t i = 0;i<PAGE_SIZE;i++)
    	{
    		*(uint16_t *)wptr = (uint16_t)j;
    		wptr ++;j++;
    	}
    	Xil_DCacheFlushRange((u32)(warray)-32,PAGE_SIZE+64);
    	progpage_80h_10h(way, 0, 0+m, PAGE_SIZE, (uint32_t)(warray));

    	usleep(3000);

    	rptr = (uint16_t *)rarray;
    	memset(rarray, 0x00, sizeof(u8)*PAGE_SIZE);
        readpage_00h_30h(way, 0, 0+m, PAGE_SIZE, (uint32_t)rarray);
        Xil_DCacheInvalidateRange((u32)rarray-128,PAGE_SIZE+256);
        l=0;
        for(uint32_t n = 0;n<(PAGE_SIZE >> 3);n++)
        {
        	if (warray[n]!=rarray[n])
        	{
        		k++;
        		l=n;

        	}
        }
        if (l!=0)
        	xil_printf("check Wrong, %d,%d\n",k,m);
    }
    if (k == 0)
    	xil_printf("check successfully!\n");
    else
    	xil_printf("check wrong!\n");

}

void test_writespeed()
{
    uint32_t m =0;
	u32 way = 1;
	XTime tEnd, tCur;
	u32 tUsed;
    eraseblock_60h_d0h(way,0);
    usleep(5000);
    eraseblock_60h_d0h(way,Block);
    usleep(5000);
    //
    uint32_t page = 0;
    XTime_GetTime(&tCur);

    // 2MB
    for (m = 0;m<64;m++)
    {
    	// plane0 page0
    	while((readstatus_70h(way) & 0x40) == 0x00);
    	progpage_80h_11h_multplane(way, 0, page, PAGE_SIZE, (uint32_t)(warray));

    	// plane1 page0 cache
    	while((readstatus_70h(way) & 0x40) == 0x00);
    	progpage_80h_15h_cache(way, 0, page+Block, PAGE_SIZE, (uint32_t)(warray));

    	page ++;
		// plane0 page1 cache
    	while((readstatus_70h(way) & 0x40) == 0x00);
    	progpage_80h_11h_multplane(way, 0, page, PAGE_SIZE, (uint32_t)(warray));

		// plane1 page1 cache
		while((readstatus_70h(way) & 0x40) == 0x00);

		if (m == 63)
			progpage_80h_10h(way, 0, page+Block, PAGE_SIZE, (uint32_t)(warray));
		else
			progpage_80h_15h_cache(way, 0, page+Block, PAGE_SIZE, (uint32_t)(warray));

		page ++;
    }

    while((readstatus_70h(way) & 0x20) == 0x00);
    XTime_GetTime(&tEnd);
	tUsed = ((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
//	xil_printf("write time elapsed is %d us\n",tUsed);
	float wspeed;
	wspeed = 2*1000000/(float)(tUsed);
	printf("write speed is %f MB/s\n",wspeed);
}
void test_readspeed()
{
    uint16_t j = 0;
    uint32_t m ,k ,l;
    m = 0;
    k=0;
	u32 way = 1;
	XTime tEnd, tCur, tTmp;
	u32 tUsed;
	XTime_GetTime(&tCur);
    for (m = 0;m<128;m++)
    {
        readpage_00h_30h(way, 0, 0+m, PAGE_SIZE, (uint32_t)rarray);
        Xil_DCacheInvalidateRange((u32)rarray-128,PAGE_SIZE+256);
    }
    XTime_GetTime(&tEnd);
    tUsed = ((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
	float wspeed;
	wspeed = 1*1000000/(float)(tUsed);
	printf("read speed is %f MB/s\n",wspeed);

}
uint32_t timestamp[1024] = {0};
uint32_t *time_ptr = timestamp;

static u8 usb_rx_buffer[1024*4];
static u8 usb_tx_buffer[1024*16];
int main()
{
    init_platform();



	int status;
	XUsbPs usb;
	XScuGic intc;

	reset_usb();

	IntcInitFunction(INTC_DEVICE_ID, &usb);

	/* Initialize the USB controller */
	status = xusb_cdc_init(&usb, XPAR_PS7_USB_0_DEVICE_ID, XPAR_PS7_USB_0_INTR, 64 * 1024);
	if (status != XST_SUCCESS) {
		xil_printf("ERROR: Unable to set up USB controller: %d\n", status);
		exit(1);
	}

	u32 way = 1;
	u32 bytes;
    uint16_t j = 0;
    uint32_t m ,k ,l;
    m = 0;
    reset_ffh(way);
    setfeature_efh(way);
    uint32_t flash_page;
    uint32_t flash_block;
	while(1)
	{

		bytes = xusb_cdc_rx_bytes_available();
		if (bytes != 0) {
			bytes = xusb_cdc_receive_data(usb_rx_buffer, bytes);
			if ((usb_rx_buffer[0] == 0x7e) && (usb_rx_buffer[1] == 0x7f))
			{
				if (usb_rx_buffer[2] == 0x00)
				{
					flash_page = *(u32*)(usb_rx_buffer+4);
					flash_block = *(u32*)(usb_rx_buffer+8);
				}
				else if (usb_rx_buffer[2] == 0x01)
				{
			        readpage_00h_30h(way, 0, flash_page+flash_block*128, 8640, (uint32_t)usb_tx_buffer);
			        Xil_DCacheInvalidateRange((u32)usb_tx_buffer-128,8640+256);
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_buffer, 8640);usleep(120);
				}
				else if (usb_rx_buffer[2] == 0x02)
				{
					eraseblock_60h_d0h(way,flash_block*128);
				}
				else if (usb_rx_buffer[2] == 0x03)
				{
					test_WRconsistent();
				}
				else if (usb_rx_buffer[2] == 0x04)
				{
					test_writespeed();
				}
				else if (usb_rx_buffer[2] == 0x05)
				{
					test_readspeed();
				}
				else if (usb_rx_buffer[2] == 0x06)
				{
					readparameterpage(way, (uint32_t)usb_tx_buffer);
			        Xil_DCacheInvalidateRange((u32)usb_tx_buffer-128,8192+256);
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_buffer, 8640);usleep(120);
				}
			}

		}

	}

    cleanup_platform();
    return 0;
}

void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType)
{
	int mask;
	intType &= INT_TYPE_MASK;
	mask = XScuGic_DistReadReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4);
	mask &= ~(INT_TYPE_MASK << (intId%16)*2);
	mask |= intType << ((intId%16)*2);
	XScuGic_DistWriteReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4, mask);
}

int IntcInitFunction(u16 DeviceId, XUsbPs *usb)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig,IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&INTCInst);
	//Xil_ExceptionEnable();


	/*----------------usb interrupt--------------------------*/
	status = xusbps_cdc_register_interrupt(&INTCInst, usb, XPAR_PS7_USB_0_INTR);
//	if (status != XST_SUCCESS) {
//		xil_printf("ERROR: Unable to register USB interrupts: %d\n", status);
//		exit(1);
//	}
	/*-------------------------------------------------------*/

	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

static int setup_interrupts(XScuGic *intc) {
	int status;
	XScuGic_Config *intc_config;

	intc_config = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == intc_config) {
		return XST_FAILURE;
	}

	status = XScuGic_CfgInitialize(intc, intc_config, intc_config->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return status;
	}

	Xil_ExceptionInit();

	/* Connect the GIC interrupt handler to the exception vector in the processor */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
								 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
								 intc);

	return XST_SUCCESS;
}
static void reset_usb(void) {
	// Ensure that the PHY is out of reset
	volatile u32 *gpio_base;
	volatile u32 *gpio_oen;
	volatile u32 *gpio_dir;

	/* Ensure that the USB PHY is not in reset */
	gpio_base = (u32 *)0xE000A000;
	gpio_oen = (u32 *)0xE000A208;
	gpio_dir = (u32 *)0xE000A204;

	*(gpio_oen) |= 0x00000080;
	*(gpio_dir) |= 0x00000080;
	*gpio_base = 0xff7f0080;
}


