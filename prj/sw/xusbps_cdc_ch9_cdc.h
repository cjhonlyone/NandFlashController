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

#ifndef XUSBPS_CDC_CH9_CDC_H_
#define XUSBPS_CDC_CH9_CDC_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xusbps_hw.h"
#include "xil_types.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

#define XUSBPS_CDC_CLASS					0x02
#define XUSBPS_CDC_DATA_CLASS				0x0a
#define XUSBPS_CDC_SUBCLASS_ACM				0x02
#define XUSBPS_CDC_PROTOCOL_NONE			0x00

#define XUSBPS_CDC_CS_INTERFACE				0x24
#define XUSBPS_CDC_CS_ENDPOINT				0x25

#define XUSBPS_CDC_SUBTYPE_HEADER			0x00
#define XUSBPS_CDC_SUBTYPE_CALL_MANAGEMENT	0x01
#define XUSBPS_CDC_SUBTYPE_ACM				0x02
#define XUSBPS_CDC_SUBTYPE_UNION			0x06

#define XUSBPS_CALL_MGMT_CAP_SELF			0x01

#define XUSBPS_CDC_ACM_CAP_COMM_FEATURE		0x01
#define XUSBPS_CDC_ACM_CAP_LINE_CTL			0x02
#define XUSBPS_CDC_ACM_CAP_SEND_BREAK		0x04
#define XUSBPS_CDC_ACM_CAP_NET_CONN			0x08

#define XUSBPS_CDC_ACM_CAP_ALL				0x0f
#define XUSBPS_CDC_ACM_CAP_PHONE			0x07

#define XUSBPS_BCD_CDC						0x0110

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/* Check where these defines need to go  */
#define be2le(val)	(u32)(val)
#define be2les(x)	(u16) (x)
#define htonl(val)	((((u32)(val) & 0x000000FF)<<24) |	\
			 (((u32)(val) & 0x0000FF00)<<8)  |	\
			 (((u32)(val) & 0x00FF0000)>>8)  |	\
			 (((u32)(val) & 0xFF000000)>>24))

#define htons(x)	(u16) ((((u16)(x))<<8) | (((u16)(x))>>8))

/************************** Function Prototypes ******************************/

u32 XUsbPs_Ch9SetupDevDescReply(u8 *BufPtr, u32 BufLen);
u32 XUsbPs_Ch9SetupCfgDescReply(u8 *BufPtr, u32 BufLen);
u32 XUsbPs_Ch9SetupStrDescReply(u8 *BufPtr, u32 BufLen, u8 Index);
void XUsbPs_SetConfiguration(XUsbPs *InstancePtr, int ConfigIdx);

#ifdef __cplusplus
}
#endif

#endif /* XUSBPS_CDC_CH9_CDC_H_ */
