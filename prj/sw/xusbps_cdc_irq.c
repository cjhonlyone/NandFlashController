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

#include "xusbps_cdc_irq.h"
#include "xusbps_cdc_ch9.h"
#include "xusbps_cdc.h"
#include "xil_cache.h"

static volatile u32 usb_irq_cnt = 0;
extern volatile u32 xusb_cdc_driver_state;
extern u8 xusb_cdc_line_coding[7];

static void dump_buffer(u8 *buffer, u32 length) {
	int loop;

	for (loop = 0; loop < length; loop++) {
		xil_printf("%02X");
		if (loop < (length - 1)) {
			xil_printf("-");
		}
		else {
			xil_printf("\n");
		}
	}
}

int xusbps_cdc_register_interrupt(XScuGic *intc, XUsbPs *usb, u16 usb_irq) {
	int status;
	if (!intc->IsReady) {
		return XST_FAILURE;
	}

	/* Connect the generic USB controller handler for the device that will be called
	 * for all USB events. This handler will then call other specific routines for
	 * handling particular events.
	 */
	status = XScuGic_Connect(intc, usb_irq, (Xil_ExceptionHandler)XUsbPs_IntrHandler, (void *)usb);
	if (status != XST_SUCCESS) {
		return status;
	}

	/* Enable the interrupt for the device */
	XScuGic_Enable(intc, usb_irq);

	return XST_SUCCESS;
}

void xusbps_cdc_disable_interrupt(XScuGic *intc, XUsbPs *usb) {
	// TODO
}

void xusb_cdc_irq_handler(void *callback_ref, u32 mask) {
	/* Nothing needed in this IRQ for now. We'll increment the IRQ count so it's visible via the debugger */
	usb_irq_cnt++;
}

/* This function is registered to handle callbacks for endpoint 0. It is called from the interrupt
 * context so the amount of processing performed should be minimized.
 */
void xusb_cdc_ep0_irq_handler(void *callback_ref, u8 endpoint, u8 event_type, void *data) {
	XUsbPs *usb;
	int status;
	XUsbPs_SetupData setup_data;
	u8 *buffer;
	u32 buffer_length;
	u32 invalidate_length;
	u32 handle;

	Xil_AssertVoid(NULL != callback_ref);

	usb = (XUsbPs *)callback_ref;

	switch (event_type) {
	/* Handle setup packets received on EP0 */
	case XUSBPS_EP_EVENT_SETUP_DATA_RECEIVED:
		status = XUsbPs_EpGetSetupData(usb, endpoint, &setup_data);
		if (status == XST_SUCCESS) {
			(int)xusb_cdc_handle_ch9_setup_packet(usb, &setup_data);
		}
		else {
#ifdef XUSB_CDC_DEBUG
			xil_printf("E: Unable to get setup data on EP0\n");
#endif
		}
		break;

	/* We will see RX events for zero-length packets on EP0. We'll receive them and immediately
	 * release them again. There's no action to be taken.
	 */
	case XUSBPS_EP_EVENT_DATA_RX:
		/* Get the data buffer */
		status = XUsbPs_EpBufferReceive(usb, endpoint, &buffer, &buffer_length, &handle);
		if (status == XST_SUCCESS) {
			/* Invalidate the buffer pointer */
			invalidate_length = buffer_length;
			/* Ensure alignment for invalidation */
			if (buffer_length % 32) {
				invalidate_length = (buffer_length / 32) * 32 + 32;
			}

			/* Invalidate the cache for the range of the received data buffer */
			Xil_DCacheInvalidateRange((unsigned int)buffer, invalidate_length);
			if (xusb_cdc_driver_state == XUSB_CDC_DRIVER_CONFIG) {
				/* This should be our new configuration data */
				invalidate_length = (buffer_length > 7) ? 7 : buffer_length;
				memcpy(xusb_cdc_line_coding, buffer, invalidate_length);

			}
#ifdef XUSB_CDC_DEBUG
			/* If the buffer length is non-zero, maybe we should have handled it? */
			else if (buffer_length != 0) {
				xil_printf("W: Unknown buffer of length %d received on EP0\n", buffer_length);
				dump_buffer(buffer, buffer_length);
			}
#endif

			/* Return the buffer */
			XUsbPs_EpBufferRelease(handle);
		}
		break;

	default:
#ifdef XUSB_CDC_DEBUG
		xil_printf("W: Received other event on EP0\n");
#endif
		break;
	}
}

void xusb_cdc_ep1_irq_handler(void *callback_ref, u8 endpoint, u8 event_type, void *data) {
	XUsbPs *usb;
	int status;
	u8 *buffer;
	u32 invalidate_length;
	u32 buffer_length;
	u32 handle;

	Xil_AssertVoid(NULL != callback_ref);

	usb = (XUsbPs *)callback_ref;

	switch(event_type) {
	case XUSBPS_EP_EVENT_DATA_RX:
		/* Get the data buffer */
		status = XUsbPs_EpBufferReceive(usb, endpoint, &buffer, &buffer_length, &handle);
		/* Invalidate the buffer pointer */
		invalidate_length = buffer_length;
		/* Ensure alignment for invalidation */
		if (buffer_length % 32) {
			invalidate_length = (buffer_length / 32) * 32 + 32;
		}

		/* Invalidate the cache for the range of the received data buffer */
		Xil_DCacheInvalidateRange((unsigned int)buffer, invalidate_length);

		if (status == XST_SUCCESS) {
			status = xusb_cdc_handle_bulk_request(usb, endpoint, buffer, buffer_length);

#ifdef XUSB_CDC_DEBUG
			if (status != XST_SUCCESS) {
				xil_printf("E: Unable to handle CDC bulk request: %d\n", status);
			}
#endif
			XUsbPs_EpBufferRelease(handle);
		}
		break;
	default:
		/* Unhandled event */
#ifdef XUSB_CDC_DEBUG
		xil_printf("W: Unhandled event type %d received on EP1\n", event_type);
#endif
		break;
	}
}

void xusb_cdc_ep2_irq_handler(void *callback_ref, u8 endpoint, u8 event_type, void *data) {
	XUsbPs *usb;
	int status;
	u8 *buffer;
	u32 invalidate_length;
	u32 buffer_length;
	u32 handle;

	Xil_AssertVoid(NULL != callback_ref);

	usb = (XUsbPs *)callback_ref;

	switch(event_type) {
	case XUSBPS_EP_EVENT_DATA_RX:
		/* Get the data buffer */
		status = XUsbPs_EpBufferReceive(usb, endpoint, &buffer, &buffer_length, &handle);
		/* Invalidate the buffer pointer */
		invalidate_length = buffer_length;
		/* Ensure alignment for invalidation */
		if (buffer_length % 32) {
			invalidate_length = (buffer_length / 32) * 32 + 32;
		}

		/* Invalidate the cache for the range of the received data buffer */
		Xil_DCacheInvalidateRange((unsigned int)buffer, invalidate_length);

		if (status == XST_SUCCESS) {
			status = xusb_cdc_handle_bulk_request(usb, endpoint, buffer, buffer_length);

#ifdef XUSB_CDC_DEBUG
			if (status != XST_SUCCESS) {
				xil_printf("E: Unable to handle CDC bulk request: %d\n", status);
			}
#endif
			XUsbPs_EpBufferRelease(handle);
		}
		break;
	default:
		/* Unhandled event */
#ifdef XUSB_CDC_DEBUG
		xil_printf("W: Unhandled event type %d received on EP2\n", event_type);
#endif
		break;
	}
}

void xusb_cdc_ep3_irq_handler(void *callback_ref, u8 endpoint, u8 event_type, void *data) {
	XUsbPs *usb;
	int status;
	u8 *buffer;
	u32 invalidate_length;
	u32 buffer_length;
	u32 handle;

	Xil_AssertVoid(NULL != callback_ref);

	usb = (XUsbPs *)callback_ref;

	switch(event_type) {
	case XUSBPS_EP_EVENT_DATA_RX:
		/* Get the data buffer */
		status = XUsbPs_EpBufferReceive(usb, endpoint, &buffer, &buffer_length, &handle);
		/* Invalidate the buffer pointer */
		invalidate_length = buffer_length;
		/* Ensure alignment for invalidation */
		if (buffer_length % 32) {
			invalidate_length = (buffer_length / 32) * 32 + 32;
		}

		/* Invalidate the cache for the range of the received data buffer */
		Xil_DCacheInvalidateRange((unsigned int)buffer, invalidate_length);

		if (status == XST_SUCCESS) {
			status = xusb_cdc_handle_bulk_request(usb, endpoint, buffer, buffer_length);

#ifdef XUSB_CDC_DEBUG
			if (status != XST_SUCCESS) {
				xil_printf("E: Unable to handle CDC bulk request: %d\n", status);
			}
#endif
			XUsbPs_EpBufferRelease(handle);
		}
		break;
	default:
		/* Unhandled event */
#ifdef XUSB_CDC_DEBUG
		xil_printf("W: Unhandled event type %d received on EP3\n", event_type);
#endif
		break;
	}
}
