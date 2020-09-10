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

#include "xusbps_cdc_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "xstatus.h"

int xusb_cdc_buffer_init(RingBuffer *rbuf, u32 size) {
	if (rbuf == NULL) {
		return XST_FAILURE;
	}

	rbuf->length = size + 1;
	rbuf->start = 0;
	rbuf->end = 0;
	rbuf->buffer = malloc(rbuf->length);

	rbuf->initialized = XUSB_CDC_BUFFER_INITIALIZED;

	return XST_SUCCESS;
}

int xusb_cdc_buffer_read(RingBuffer *rbuf, u8 *buffer, u32 length) {
	u32 bytes_read;
	u32 available_bytes;
	u32 remaining;

	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_CDC_BUFFER_INITIALIZED) {
		return 0;
	}

	remaining = length;
	available_bytes = xusb_cdc_buffer_available_bytes(rbuf);
	if (length > available_bytes) {
		remaining = available_bytes;
	}

	if ((rbuf->start + remaining) > rbuf->length) {
		bytes_read = rbuf->length - rbuf->start;
		remaining -= bytes_read;
		memcpy(buffer,
			   rbuf->buffer + rbuf->start,
			   bytes_read);
		memcpy(buffer + bytes_read,
			   rbuf->buffer,
			   remaining);
		bytes_read += remaining;
	}
	else {
		memcpy(buffer,
			   rbuf->buffer + rbuf->start,
			   length);
		bytes_read = length;
	}

	/* Update the pointers in the buffer */
	if (bytes_read == available_bytes) {
		rbuf->start = 0;
		rbuf->end = 0;
	}
	else if((rbuf->start + bytes_read) > rbuf->length) {
		rbuf->start = rbuf->length % (rbuf->start + bytes_read);
	}
	else {
		rbuf->start += bytes_read;
	}

	return bytes_read;
}

int xusb_cdc_buffer_write(RingBuffer *rbuf, u8 *data, u32 length) {
	u32 bytes_written;
	u32 available_bytes;
	u32 remaining;

	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_CDC_BUFFER_INITIALIZED) {
		return 0;
	}

	remaining = length;
	available_bytes = xusb_cdc_buffer_available_space(rbuf);
	if (length > available_bytes) {
		remaining = available_bytes;
	}

	if ((rbuf->end + remaining) > rbuf->length) {
		bytes_written = rbuf->length - rbuf->end;
		remaining -= bytes_written;
		memcpy(rbuf->buffer + rbuf->end,
			   data,
			   bytes_written);
		memcpy(rbuf->buffer,
			   data + bytes_written,
			   remaining);
		bytes_written += remaining;
	}
	else {
		memcpy(rbuf->buffer + rbuf->end,
			   data,
			   length);
		bytes_written = length;
	}

	/* Update the pointers in the buffer */
	if((rbuf->end + bytes_written) > rbuf->length) {
		rbuf->end = rbuf->length % (rbuf->end + bytes_written);
	}
	else {
		rbuf->end += bytes_written;
	}

	return bytes_written;
}

int xusb_cdc_buffer_available_bytes(RingBuffer *rbuf) {
	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_CDC_BUFFER_INITIALIZED) {
		return 0;
	}

	if (rbuf->end < rbuf->start) {
		return rbuf->length - rbuf->start + rbuf->end;
	}
	else {
		return rbuf->end - rbuf->start;
	}

}

int xusb_cdc_buffer_available_space(RingBuffer *rbuf) {
	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_CDC_BUFFER_INITIALIZED) {
		return 0;
	}

	if (rbuf->end < rbuf->start) {
		return rbuf->start - rbuf->end;
	}
	else {
		return rbuf->length - rbuf->end + rbuf->start;
	}

}
