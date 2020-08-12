/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
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
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include "xusbps.h"
#include "xusbps_cdc.h"
#include "xusbps_cdc_ch9.h"
#include "xusbps_cdc_ch9_cdc.h"
#include "xusbps_cdc_irq.h"
#include "xusbps_cdc_buffer.h"
#include "xil_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef XUSBPS_CDC_MEM_SIZE
#define XUSBPS_CDC_MEM_SIZE	(64 * 1024)
#endif

static u8 usb_dma_buffer[XUSBPS_CDC_MEM_SIZE] ALIGNMENT_CACHELINE;
volatile u32 xusb_cdc_driver_state = XUSB_CDC_DRIVER_UNCONNECTED;

RingBuffer xusb_cdc_rx_buffer;

XUsbPs_Local usb_local_data;

int xusb_cdc_init(XUsbPs *usb, u16 usb_device_id, u16 usb_irq, u32 rx_buf_size)
{
	int status;
	XUsbPs_Config *usb_cfg;
	XUsbPs_DeviceConfig device_config;
	u8 *buffer_ptr;

	/* For this example we're configuring four endpoints:
	 *   Endpoint 0 (control)
	 *   Endpoint 1 (Interrupt IN)
	 *   Endpoint 2,3 (BULK data IN/OUT endpoints)
	 */
	const u8 num_endpoints = 4;

	usb_cfg = XUsbPs_LookupConfig(usb_device_id);
	if (NULL == usb_cfg) {
		goto out;
	}

	status = XUsbPs_CfgInitialize(usb,
				       	   	   	  usb_cfg,
				       	   	   	  usb_cfg->BaseAddress);
	if (XST_SUCCESS != status) {
		goto out;
	}

	/* Configuration of the DEVICE side of the controller happens in
	 * multiple stages.
	 *
	 * 1) The user configures the desired endpoint configuration using the
	 * XUsbPs_DeviceConfig data structure. This includes the number of
	 * endpoints, the number of Transfer Descriptors for each endpoint
	 * (each endpoint can have a different number of Transfer Descriptors)
	 * and the buffer size for the OUT (receive) endpoints.  Each endpoint
	 * can have different buffer sizes.
	 *
	 * 2) Request the required size of DMAable memory from the driver using
	 * the XUsbPs_DeviceMemRequired() call.
	 *
	 * 3) Allocate the DMAable memory and set up the DMAMemVirt and
	 * DMAMemPhys members in the XUsbPs_DeviceConfig data structure.
	 *
	 * 4) Configure the DEVICE side of the controller by calling the
	 * XUsbPs_ConfigureDevice() function.
	 */

	/*
	 * For this example we only configure Endpoint 0 and Endpoint 1.
	 *
	 * Bufsize = 0 indicates that there is no buffer allocated for OUT
	 * (receive) endpoint 0. Endpoint 0 is a control endpoint and we only
	 * receive control packets on that endpoint. Control packets are 8
	 * bytes in size and are received into the Queue Head's Setup Buffer.
	 * Therefore, no additional buffer space is needed.
	 */
	device_config.EpCfg[0].Out.Type		= XUSBPS_EP_TYPE_CONTROL;
	device_config.EpCfg[0].Out.NumBufs	= 2;
	device_config.EpCfg[0].Out.BufSize	= 64;
	device_config.EpCfg[0].Out.MaxPacketSize	= 64;
	device_config.EpCfg[0].In.Type		= XUSBPS_EP_TYPE_CONTROL;
	device_config.EpCfg[0].In.NumBufs	= 2;
	device_config.EpCfg[0].In.MaxPacketSize	= 64;

	device_config.EpCfg[1].Out.Type		= XUSBPS_EP_TYPE_INTERRUPT;
	device_config.EpCfg[1].Out.NumBufs	= 2;
	device_config.EpCfg[1].Out.BufSize	= 64;
	device_config.EpCfg[1].Out.MaxPacketSize	= 64;
	device_config.EpCfg[1].In.Type		= XUSBPS_EP_TYPE_INTERRUPT;
	device_config.EpCfg[1].In.NumBufs	= 2;
	device_config.EpCfg[1].In.MaxPacketSize	= 64;

	device_config.EpCfg[2].Out.Type		= XUSBPS_EP_TYPE_BULK;
	device_config.EpCfg[2].Out.NumBufs	= 16;
	device_config.EpCfg[2].Out.BufSize	= 512;
	device_config.EpCfg[2].Out.MaxPacketSize	= 512;
	device_config.EpCfg[2].In.Type		= XUSBPS_EP_TYPE_BULK;
	device_config.EpCfg[2].In.NumBufs	= 16;
	device_config.EpCfg[2].In.MaxPacketSize	= 512;

	device_config.EpCfg[3].Out.Type		= XUSBPS_EP_TYPE_BULK;
	device_config.EpCfg[3].Out.NumBufs	= 16;
	device_config.EpCfg[3].Out.BufSize	= 512;
	device_config.EpCfg[3].Out.MaxPacketSize	= 512;
	device_config.EpCfg[3].In.Type		= XUSBPS_EP_TYPE_BULK;
	device_config.EpCfg[3].In.NumBufs	= 16;
	device_config.EpCfg[3].In.MaxPacketSize	= 512;

	device_config.NumEndpoints = num_endpoints;

	buffer_ptr = (u8 *)&usb_dma_buffer[0];
	memset(buffer_ptr, 0, XUSBPS_CDC_MEM_SIZE);
	Xil_DCacheFlushRange((unsigned int)buffer_ptr, XUSBPS_CDC_MEM_SIZE);

	/* Finish the configuration of the DeviceConfig structure and configure
	 * the DEVICE side of the controller.
	 */
	device_config.DMAMemPhys = (u32)buffer_ptr;

	status = XUsbPs_ConfigureDevice(usb, &device_config);
	if (XST_SUCCESS != status) {
		goto out;
	}

	/* Set the handler for receiving frames. */
	status = XUsbPs_IntrSetHandler(usb,
								   xusb_cdc_irq_handler,
								   NULL,
								   XUSBPS_IXR_UE_MASK);
	if (XST_SUCCESS != status) {
		goto out;
	}

	/* Set the handler for handling endpoint 0 events. This is where we
	 * will receive and handle the Setup packet from the host.
	 */
	status = XUsbPs_EpSetHandler(usb,
								 0,
								 XUSBPS_EP_DIRECTION_OUT,
								 xusb_cdc_ep0_irq_handler,
								 usb);

	/* Set the handler for handling endpoint 1 events.
	 *
	 * Note that for this example we do not need to register a handler for
	 * TX complete events as we only send data using static data buffers
	 * that do not need to be free()d or returned to the OS after they have
	 * been sent.
	 */
	status = XUsbPs_EpSetHandler(usb, 1,
								 XUSBPS_EP_DIRECTION_OUT,
								 xusb_cdc_ep1_irq_handler, usb);

	status = XUsbPs_EpSetHandler(usb, 2,
								 XUSBPS_EP_DIRECTION_OUT,
								 xusb_cdc_ep2_irq_handler, usb);
	status = XUsbPs_EpSetHandler(usb, 3,
								 XUSBPS_EP_DIRECTION_IN,
								 xusb_cdc_ep3_irq_handler, usb);
	status = XUsbPs_EpSetHandler(usb, 3,
									 XUSBPS_EP_DIRECTION_OUT,
									 xusb_cdc_ep3_irq_handler, usb);


	/* Enable the interrupts. */
	XUsbPs_IntrEnable(usb, XUSBPS_IXR_UR_MASK |
					   XUSBPS_IXR_UI_MASK);

	usb->UserDataPtr = (void *)&usb_local_data;

	/* Initialize the RX circular buffer */
	status = xusb_cdc_buffer_init(&xusb_cdc_rx_buffer, rx_buf_size);
	if (status != XST_SUCCESS) {
		goto out;
	}

	/* Start the USB engine */
	XUsbPs_Start(usb);

	/* Set return code to indicate success and fall through to clean-up
	 * code.
	 */
	return XST_SUCCESS;

out:
	/* Clean up. It's always safe to disable interrupts and clear the
	 * handlers, even if they have not been enabled/set. The same is true
	 * for disabling the interrupt subsystem.
	 */
	XUsbPs_Stop(usb);
	XUsbPs_IntrDisable(usb, XUSBPS_IXR_ALL);
	(int) XUsbPs_IntrSetHandler(usb, NULL, NULL, 0);

	/* Free allocated memory.
	 */
	if (NULL != usb->UserDataPtr) {
		free(usb->UserDataPtr);
	}
	return status;
}

void dump_buffer(u8 *buffer, u32 length) {
	int loop;

	for (loop = 0; loop < length; loop++) {
		if (loop != 0)
			if ((loop%16) == 0)
				xil_printf("\n");
		xil_printf("%02X", buffer[loop]);
		if (loop < (length - 1)) {
			xil_printf("-");
		}
		else {
			xil_printf("\n");
		}

	}
}

int xusb_cdc_handle_bulk_request(XUsbPs *usb, u8 endpoint, u8 *buffer, u32 buffer_length) {

//	xil_printf("EP%d received %d bytes: ", endpoint, buffer_length);

//	dump_buffer(buffer, buffer_length);

	if (endpoint == 3) {
		xusb_cdc_buffer_write(&xusb_cdc_rx_buffer, buffer, buffer_length);
	}

	return XST_SUCCESS;
}

int xusb_cdc_send_data(XUsbPs *usb, u8 *buffer, u32 length) {
	int status;

	status = XUsbPs_EpBufferSend(usb, 2, buffer, length);
	if (status == XST_SUCCESS) {
		return length;
	}
	else {
		return 0;
	}
}

int xusb_cdc_rx_bytes_available(void) {
	if (xusb_cdc_driver_state == XUSB_CDC_DRIVER_UNCONNECTED) {
		return 0;
	}

	return xusb_cdc_buffer_available_bytes(&xusb_cdc_rx_buffer);
}

int xusb_cdc_receive_data(u8 *buffer, u32 length) {
	if (xusb_cdc_driver_state == XUSB_CDC_DRIVER_UNCONNECTED) {
		return 0;
	}

	return xusb_cdc_buffer_read(&xusb_cdc_rx_buffer, buffer, length);
}
