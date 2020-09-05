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

#ifndef XUSBPS_CDC_H_
#define XUSBPS_CDC_H_

#include "xusbps.h"

//#define XUSB_CDC_DEBUG
#define XUSB_CDC_DRIVER_UNCONNECTED			0
#define XUSB_CDC_DRIVER_CONFIG 				1
#define XUSB_CDC_DRIVER_CONNECTED 			2

/* Function prototypes */
int xusb_cdc_init(XUsbPs *usb, u16 usb_device_id, u16 usb_irq, u32 rx_buf_size);
int xusb_cdc_handle_bulk_request(XUsbPs *usb, u8 endpoint, u8 *buffer, u32 buffer_length);
int xusb_cdc_send_data(XUsbPs *usb, u8 *buffer, u32 length);
int xusb_cdc_rx_bytes_available(void);
int xusb_cdc_receive_data(u8 *buffer, u32 length);

#endif /* XUSBPS_CDC_H_ */
