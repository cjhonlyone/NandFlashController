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

#ifndef XUSBPS_CDC_BUFFER_H_
#define XUSBPS_CDC_BUFFER_H_

#include "xil_types.h"

typedef struct {
	u8 *buffer;
	u32 length;
	u32 start;
	u32 end;
	u32 initialized;
} RingBuffer;

#define XUSB_CDC_BUFFER_INITIALIZED	0xaa995566

/* Function Prototypes */
int xusb_cdc_buffer_init(RingBuffer *rbuf, u32 size);
int xusb_cdc_buffer_read(RingBuffer *rbuf, u8 *buffer, u32 length);
int xusb_cdc_buffer_write(RingBuffer *rbuf, u8 *data, u32 length);
int xusb_cdc_buffer_available_bytes(RingBuffer *rbuf);
int xusb_cdc_buffer_available_space(RingBuffer *rbuf);

#endif /* XUSBPS_CDC_BUFFER_H_ */
