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

/***************************** Include Files *********************************/

#include <string.h>

#include "xparameters.h"	/* XPAR parameters */
#include "xusbps.h"		/* USB controller driver */

#include "xusbps_cdc_ch9.h"
#include "xusbps_cdc_ch9_cdc.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	u16 idVendor;
	u16 idProduct;
	u16 bcdDevice;
	u8  iManufacturer;
	u8  iProduct;
	u8  iSerialNumber;
	u8  bNumConfigurations;
}__attribute__((__packed__))USB_STD_DEV_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 wTotalLength;
	u8  bNumInterfaces;
	u8  bConfigurationValue;
	u8  iConfiguration;
	u8  bmAttributes;
	u8  bMaxPower;
}__attribute__((__packed__))USB_STD_CFG_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bInterfaceNumber;
	u8  bAlternateSetting;
	u8  bNumEndPoints;
	u8  bInterfaceClass;
	u8  bInterfaceSubClass;
	u8  bInterfaceProtocol;
	u8  iInterface;
}__attribute__((__packed__))USB_STD_IF_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bEndpointAddress;
	u8  bmAttributes;
	u16 wMaxPacketSize;
	u8  bInterval;
}__attribute__((__packed__))USB_STD_EP_DESC;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 wLANGID[1];
}__attribute__((__packed__))USB_STD_STRING_DESC;

typedef struct {
	u8	bFunctionLength;
	u8 	bDescriptorType;
	u8	bDescriptorSubtype;
	u16	bcdCDC;
}__attribute__((__packed__))USB_CDC_HEADER_DESC;

typedef struct {
	u8	bFunctionLength;
	u8	bDescriptorType;
	u8	bDescriptorSubtype;
	u8	bmCapabilities;
	u8	bDataInterface;
}__attribute__((__packed__))USB_CDC_CALL_MANAGEMENT_DESC;

typedef struct {
	u8	bFunctionLength;
	u8	bDescriptorType;
	u8	bDescriptorSubtype;
	u8	bmCapabilities;
}__attribute__((__packed__))USB_CDC_ACM_DESC;

typedef struct {
	u8	bFunctionLength;
	u8	bDescriptorType;
	u8	bDescriptorSubtype;
	u8	bControlInterface;
	u8	bSubordinateInterface0;
}__attribute__((__packed__))USB_CDC_UNION_DESC;

typedef struct {
	USB_STD_CFG_DESC stdCfg;
	USB_STD_IF_DESC ifCfg;
	USB_CDC_HEADER_DESC cdcHdr;
	USB_CDC_CALL_MANAGEMENT_DESC cdcCallMgmt;
	USB_CDC_ACM_DESC	cdcAcm;
	USB_CDC_UNION_DESC	cdcUnion;
	USB_STD_EP_DESC epCfg1;
	USB_STD_IF_DESC difCfg;
	USB_STD_EP_DESC epCfg2;
	USB_STD_EP_DESC epCfg3;
}__attribute__((__packed__))USB_CONFIG;

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#define USB_ENDPOINT0_MAXP		0x40

#define USB_IRQ_EP				1
#define USB_BULKIN_EP			2
#define USB_BULKOUT_EP			3

#define USB_DEVICE_DESC			0x01
#define USB_CONFIG_DESC			0x02
#define USB_STRING_DESC			0x03
#define USB_INTERFACE_CFG_DESC		0x04
#define USB_ENDPOINT_CFG_DESC		0x05


/*****************************************************************************/
/**
*
* This function returns the device descriptor for the device.
*
* @param	BufPtr is pointer to the buffer that is to be filled
*		with the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupDevDescReply(u8 *BufPtr, u32 BufLen)
{
	USB_STD_DEV_DESC deviceDesc = {
		sizeof(USB_STD_DEV_DESC),	/* bLength */
		USB_DEVICE_DESC,		/* bDescriptorType */
		be2les(0x0200),			/* bcdUSB 2.0 */
		XUSBPS_CLASS_CDC,				/* bDeviceClass */
		0x00,				/* bDeviceSubClass */
		0x00,				/* bDeviceProtocol */
		USB_ENDPOINT0_MAXP,		/* bMaxPackedSize0 */
		be2les(0x03fd),			/* idVendor */
		be2les(0x0103),			/* idProduct */
		be2les(0x0100),			/* bcdDevice */
		0x01,				/* iManufacturer */
		0x02,				/* iProduct */
		0x03,				/* iSerialNumber */
		0x01				/* bNumConfigurations */
	};

	/* Check buffer pointer is there and buffer is big enough. */
	if (!BufPtr) {
		return 0;
	}

	if (BufLen < sizeof(USB_STD_DEV_DESC)) {
		return 0;
	}

	memcpy(BufPtr, &deviceDesc, sizeof(USB_STD_DEV_DESC));

	return sizeof(USB_STD_DEV_DESC);
}


/*****************************************************************************/
/**
*
* This function returns the configuration descriptor for the device.
*
* @param	BufPtr is the pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupCfgDescReply(u8 *BufPtr, u32 BufLen)
{
	USB_CONFIG config = {
		/* Std Config */
		{sizeof(USB_STD_CFG_DESC),		/* bLength */
		 USB_CONFIG_DESC,				/* bDescriptorType */
		 be2les(sizeof(USB_CONFIG)),	/* wTotalLength */
		 0x02,							/* bNumInterfaces */
		 0x01,							/* bConfigurationValue */
		 0x04,							/* iConfiguration */
		 0xc0,							/* bmAttribute */
		 0x00},							/* bMaxPower  */

		/* Interface Config */
		{sizeof(USB_STD_IF_DESC),		/* bLength */
		 USB_INTERFACE_CFG_DESC,		/* bDescriptorType */
		 0x00,							/* bInterfaceNumber */
		 0x00,							/* bAlternateSetting */
		 0x01,							/* bNumEndPoints */
		 XUSBPS_CDC_CLASS,				/* bInterfaceClass */
		 XUSBPS_CDC_SUBCLASS_ACM,		/* bInterfaceSubClass */
		 XUSBPS_CDC_PROTOCOL_NONE,		/* bInterfaceProtocol */
		 0x05},							/* iInterface */

		 /* CDC Header Config */
		 {sizeof(USB_CDC_HEADER_DESC),	/* bLength */
		  XUSBPS_CDC_CS_INTERFACE,		/* bDescriptorType */
		  XUSBPS_CDC_SUBTYPE_HEADER,	/* bDescriptorSubType */
		  be2les(XUSBPS_BCD_CDC)		/* BCD CDC */
		 },

		/* CDC call management descriptor */
		 {sizeof(USB_CDC_CALL_MANAGEMENT_DESC),
		  XUSBPS_CDC_CS_INTERFACE,
		  XUSBPS_CDC_SUBTYPE_CALL_MANAGEMENT,
		  XUSBPS_CALL_MGMT_CAP_SELF,
		  0x00
		 },

		 /* CDC Abstract Control Management Descriptor */
		 {sizeof(USB_CDC_ACM_DESC),
		  XUSBPS_CDC_CS_INTERFACE,
		  XUSBPS_CDC_SUBTYPE_ACM,
		  XUSBPS_CDC_ACM_CAP_PHONE
		 },

		 /* CDC Union Descriptor */
		 {sizeof(USB_CDC_UNION_DESC),
		  XUSBPS_CDC_CS_INTERFACE,
		  XUSBPS_CDC_SUBTYPE_UNION,
		  0,
		  1
		 },

		 /* Interrupt endpoint config */
		 {sizeof(USB_STD_EP_DESC),	/* bLength */
		  USB_ENDPOINT_CFG_DESC,		/* bDescriptorType */
		  0x80 | USB_IRQ_EP,		/* bEndpointAddress */
		  0x03,				/* bmAttribute  */
		  be2les(0x8),			/* wMaxPacketSize */
		  0xFF},				/* bInterval */

		/* Interface Config */
		{sizeof(USB_STD_IF_DESC),		/* bLength */
		 USB_INTERFACE_CFG_DESC,		/* bDescriptorType */
		 0x01,							/* bInterfaceNumber */
		 0x00,							/* bAlternateSetting */
		 0x02,							/* bNumEndPoints */
		 XUSBPS_CDC_DATA_CLASS,				/* bInterfaceClass */
		 0x00,		/* bInterfaceSubClass */
		 XUSBPS_CDC_PROTOCOL_NONE,		/* bInterfaceProtocol */
		 0x00},							/* iInterface */

		/* Bulk Out Endpoint Config */
		{sizeof(USB_STD_EP_DESC),	/* bLength */
		 USB_ENDPOINT_CFG_DESC,		/* bDescriptorType */
		 0x00 | USB_BULKOUT_EP,		/* bEndpointAddress */
		 0x02,				/* bmAttribute  */
		 be2les(0x200),			/* wMaxPacketSize */
		 0x00},				/* bInterval */

		/* Bulk In Endpoint Config */
		{sizeof(USB_STD_EP_DESC),	/* bLength */
		 USB_ENDPOINT_CFG_DESC,		/* bDescriptorType */
		 0x80 | USB_BULKIN_EP,		/* bEndpointAddress */
		 0x02,				/* bmAttribute  */
		 be2les(0x200),			/* wMaxPacketSize */
		 0x00}				/* bInterval */
	};

	/* Check buffer pointer is OK and buffer is big enough. */
	if (!BufPtr) {
		return 0;
	}

	if (BufLen < sizeof(USB_CONFIG)) {
#ifdef XUSB_CDC_DEBUG
		xil_printf("E: Buffer provided is %d bytes, need %d\n", BufLen, sizeof(USB_CONFIG));
#endif
		return 0;
	}

	memcpy(BufPtr, &config, sizeof(USB_CONFIG));

	return sizeof(USB_CONFIG);
}


/*****************************************************************************/
/**
*
* This function returns a string descriptor for the given index.
*
* @param	BufPtr is a  pointer to the buffer that is to be filled with
*		the descriptor.
* @param	BufLen is the size of the provided buffer.
* @param	Index is the index of the string for which the descriptor
*		is requested.
*
* @return 	Length of the descriptor in the buffer on success.
*		0 on error.
*
******************************************************************************/
u32 XUsbPs_Ch9SetupStrDescReply(u8 *BufPtr, u32 BufLen, u8 Index)
{
	int i;

	static char *StringList[] = {
		"UNUSED",
		"Xilinx",
		"CDC ACM Driver",
		"2A49876D9CC1AA4",
		"Default Configuration",
		"Default Interface",
	};
	char *String;
	u32 StringLen;
	u32 DescLen;
	u8 TmpBuf[128];

	USB_STD_STRING_DESC *StringDesc;

	if (!BufPtr) {
		return 0;
	}

	if (Index >= sizeof(StringList) / sizeof(char *)) {
		return 0;
	}

	String = StringList[Index];
	StringLen = strlen(String);

	StringDesc = (USB_STD_STRING_DESC *) TmpBuf;

	/* Index 0 is special as we can not represent the string required in
	 * the table above. Therefore we handle index 0 as a special case.
	 */
	if (0 == Index) {
		StringDesc->bLength = 4;
		StringDesc->bDescriptorType = USB_STRING_DESC;
		StringDesc->wLANGID[0] = be2les(0x0409);
	}
	/* All other strings can be pulled from the table above. */
	else {
		StringDesc->bLength = StringLen * 2 + 2;
		StringDesc->bDescriptorType = USB_STRING_DESC;

		for (i = 0; i < StringLen; i++) {
			StringDesc->wLANGID[i] = be2les((u16) String[i]);
		}
	}
	DescLen = StringDesc->bLength;

	/* Check if the provided buffer is big enough to hold the descriptor. */
	if (DescLen > BufLen) {
#ifdef XUSB_CDC_DEBUG
		xil_printf("E: Provided buffer is not big enough to hold the descriptor\n");
#endif
		return 0;
	}

	memcpy(BufPtr, StringDesc, DescLen);

	return DescLen;
}


/*****************************************************************************/
/**
* This function handles a "set configuration" request.
*
* @param	InstancePtr is a pointer to XUsbPs instance of the controller.
* @param	ConfigIdx is the Index of the desired configuration.
*
* @return	None
*
******************************************************************************/
void XUsbPs_SetConfiguration(XUsbPs *InstancePtr, int ConfigIdx)
{
	Xil_AssertVoid(InstancePtr != NULL);

	/* We only have one configuration. Its index is 1. Ignore anything
	 * else.
	 */
	if (1 != ConfigIdx) {
		return;
	}

#ifdef XUSB_CDC_DEBUG
	xil_printf("I: Setting configuration %d\n", ConfigIdx);
#endif

	XUsbPs_EpEnable(InstancePtr, 1, XUSBPS_EP_DIRECTION_IN);
	XUsbPs_EpEnable(InstancePtr, 2, XUSBPS_EP_DIRECTION_IN);
	XUsbPs_EpEnable(InstancePtr, 3, XUSBPS_EP_DIRECTION_OUT);

	/* Set BULK mode for both directions.  */
	XUsbPs_SetBits(InstancePtr, XUSBPS_EPCR2_OFFSET,
						XUSBPS_EPCR_TXT_BULK_MASK |
						XUSBPS_EPCR_RXT_BULK_MASK |
						XUSBPS_EPCR_TXR_MASK |
						XUSBPS_EPCR_RXR_MASK);
	/* Set BULK mode for both directions.  */
	XUsbPs_SetBits(InstancePtr, XUSBPS_EPCR3_OFFSET,
							XUSBPS_EPCR_TXT_BULK_MASK |
							XUSBPS_EPCR_RXT_BULK_MASK |
							XUSBPS_EPCR_TXR_MASK |
							XUSBPS_EPCR_RXR_MASK);
	XUsbPs_SetBits(InstancePtr, XUSBPS_EPCR1_OFFSET,
							XUSBPS_EPCR_TXT_INTR_MASK |
							XUSBPS_EPCR_RXT_INTR_MASK |
							XUSBPS_EPCR_TXR_MASK |
							XUSBPS_EPCR_RXR_MASK);


	/* Prime the OUT endpoint. */
	XUsbPs_EpPrime(InstancePtr, 3, XUSBPS_EP_DIRECTION_OUT);

}
