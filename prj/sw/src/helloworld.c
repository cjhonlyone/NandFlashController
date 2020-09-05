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

#define DelayForUsbTransferUs 500

#define ReadPage 0x00
#define ReadParameter 0x01
#define ProgramPage 0x02
#define EraseBlcok 0x03
#define ReadBlock 0x04
#define TESTWRconsistent 0x0A
#define TESTwritespeed 0x0B
#define TESTreadspeed 0x0C

#define INT_CFG0_OFFSET 0x00000C00
#define INT_TYPE_RISING_EDGE 0x03
#define INT_TYPE_HIGHLEVEL 0x01
#define INT_TYPE_MASK 0x03

#define INTC_DEVICE_ID XPAR_PS7_SCUGIC_0_DEVICE_ID

static XScuGic INTCInst;


static int IntcInitFunction(u16 DeviceId, XUsbPs *usb);
static int setup_interrupts(XScuGic *intc);
static void reset_usb(void);

u32 UsbCmdFlashBlock = 0;
u32 UsbCmdFlashPage = 0;


#define PAGE_SIZE 4096
#define Block 128
#define FLASH(PAGE,BLOCK) ((PAGE & 0x00000007f) | (BLOCK << 7))
//1 page = (8K + 448) bytes
//1 block = 128 pages = (1024 + 56K) bytes
//1 plane = 2048 block = 17280 Mb
//1 LUN = 2 planes = 34560 Mb 4GB


uint64_t warray[2048] = {0};
uint64_t rarray[2048] = {0};
uint16_t *wptr = (uint16_t *)warray;
uint16_t *rptr = (uint16_t *)rarray;


static uint8_t usb_rx_buffer[2048*8] __attribute__ ((__aligned__(32)));
static uint8_t usb_tx_buffer[2048*8] __attribute__ ((__aligned__(32)));
static uint8_t * usb_rx_bufferPtr = (uint8_t *)usb_rx_buffer;
static uint8_t * usb_tx_bufferPtr = (uint8_t *)usb_tx_buffer;
#define UsbPacketSize 8704

u32 test_WRconsistent(u32 way, u32 page_size)
{
    uint16_t j = 0;
    uint32_t m ,k ;
    m = 0;
    k=0;

	//erase first block
    eraseblock_60h_d0h(way,0);
    usleep(5000);


    for (m = 0;m<128;m++)
    {
    	wptr = (uint16_t *)warray;
    	for (uint32_t i = 0;i<page_size;i++)
    	{
    		*(uint16_t *)wptr = (uint16_t)j;
    		wptr ++;j++;
    	}
    	Xil_DCacheFlushRange((u32)(warray)-32,page_size+64);
    	progpage_80h_10h(way, 0, 0+m, page_size, (uint32_t)(warray));

    	usleep(3000);

    	rptr = (uint16_t *)rarray;
    	memset(rarray, 0x00, sizeof(u8)*page_size);
        readpage_00h_30h(way, 0, 0+m, page_size, (uint32_t)rarray);
        Xil_DCacheInvalidateRange((u32)rarray-128,page_size+256);
//        l=0;
        for(uint32_t n = 0;n<(page_size >> 3);n++)
        {
        	if (warray[n]!=rarray[n])
        	{
        		k++;
//        		l=n;

        	}
        }
//        if (l!=0)
        	//xil_printf("check Wrong, %d,%d\n",k,m);
    }
    if (k == 0)
    	return 1;//xil_printf("check successfully!\n");
    else
    	return 0;//xil_printf("check wrong!\n");

}

float test_writespeed(u32 way, u32 page_size)
{
    uint32_t m =0;
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
    	while((readstatus_70h(way) & RDY) == 0x00);
    	progpage_80h_11h_multplane(way, 0, page, page_size, (uint32_t)(warray));

    	// plane1 page0 cache
    	while((readstatus_70h(way) & RDY) == 0x00);
    	progpage_80h_15h_cache(way, 0, page+Block, page_size, (uint32_t)(warray));

    	page ++;
		// plane0 page1 cache
    	while((readstatus_70h(way) & RDY) == 0x00);
    	progpage_80h_11h_multplane(way, 0, page, page_size, (uint32_t)(warray));

		// plane1 page1 cache
		while((readstatus_70h(way) & RDY) == 0x00);

		if (m == 63)
			progpage_80h_10h(way, 0, page+Block, page_size, (uint32_t)(warray));
		else
			progpage_80h_15h_cache(way, 0, page+Block, page_size, (uint32_t)(warray));

		page ++;
    }

    while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
    XTime_GetTime(&tEnd);
	tUsed = ((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
	float wspeed;
	wspeed = (page_size >> 11)*1000000/(float)(tUsed)/2;
	return wspeed;
}
float test_readspeed(u32 way, u32 page_size)
{

    uint32_t m;
    m = 0;
	XTime tEnd, tCur;
	u32 tUsed;
	XTime_GetTime(&tCur);
    for (m = 0;m<128;m++)
    {
        readpage_00h_30h(way, 0, 0+m, page_size, (uint32_t)rarray);
        Xil_DCacheInvalidateRange((u32)rarray-128,page_size+256);
    }
    XTime_GetTime(&tEnd);
    tUsed = ((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
	float wspeed;
	wspeed = (page_size >> 12)*1000000/(float)(tUsed)/2;
	return wspeed;
//	printf("read speed is %f MB/s\n",wspeed);

}
uint32_t timestamp[1024] = {0};
uint32_t *time_ptr = timestamp;

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
    reset_ffh(1);
    reset_ffh(2);
    setfeature_efh(1, 0x14000000);
    setfeature_efh(2, 0x14000000);



    // read parameter;
	memset(usb_tx_bufferPtr, 0x00, UsbPacketSize);
	while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
	readparameterpage(way, (u32)usb_tx_bufferPtr+32);
	usleep(10);
	Xil_DCacheInvalidateRange((u32)usb_tx_bufferPtr+32,256);

	struct nand_onfi_params *nand_onfi_params_ptr = (struct nand_onfi_params *)(usb_tx_bufferPtr + 32);
	char string[256];
	printf("rev info and features block\r\n");
	memcpy(string, (char *)nand_onfi_params_ptr->sig, 4); string[4] = 0;
	printf("sig[4]                     : %s\r\n", string);
	printf("revision                   : 0x%04x\r\n", nand_onfi_params_ptr->revision);
	printf("features                   : 0x%04x\r\n", nand_onfi_params_ptr->features);
	printf("opt_cmd                    : 0x%04x\r\n", nand_onfi_params_ptr->opt_cmd);
	//memcpy(string, (char *)nand_onfi_params_ptr->reserved0, 2); string[2] = 0;
	//printf("reserved0[2]               : %d\r\n", nand_onfi_params_ptr->reserved0[2]);
	printf("ext_param_page_length      : %d\r\n", nand_onfi_params_ptr->ext_param_page_length);
	printf("num_of_param_pages         : %d\r\n", nand_onfi_params_ptr->num_of_param_pages);
	//memcpy(string, (char *)nand_onfi_params_ptr->reserved1, 17); string[17] = 0;
	//printf("reserved1[17]              : %d\r\n", nand_onfi_params_ptr->reserved1[17]);

	printf("\r\nmanufacturer information block\r\n");
	memcpy(string, nand_onfi_params_ptr->manufacturer, 12); string[12] = 0;
	printf("manufacturer[12]           : %s\r\n", string);
	memcpy(string, nand_onfi_params_ptr->model, 20); string[20] = 0;
	printf("model[20]                  : %s\r\n", string);
	printf("jedec_id                   : 0x%02x\r\n", nand_onfi_params_ptr->jedec_id);
	printf("date_code                  : 0x%04x\r\n", nand_onfi_params_ptr->date_code);
	//memcpy(string, (char *)nand_onfi_params_ptr->reserved2, 13); string[13] = 0;
	//printf("reserved2[13]              : %d\r\n", nand_onfi_params_ptr->reserved2[13]);

	printf("\r\nmemory organization block\r\n");
	printf("byte_per_page              : %d\r\n", nand_onfi_params_ptr->byte_per_page);
	printf("spare_bytes_per_page       : %d\r\n", nand_onfi_params_ptr->spare_bytes_per_page);
	printf("data_bytes_per_ppage       : %d\r\n", nand_onfi_params_ptr->data_bytes_per_ppage);
	printf("spare_bytes_per_ppage      : %d\r\n", nand_onfi_params_ptr->spare_bytes_per_ppage);
	printf("pages_per_block            : %d\r\n", nand_onfi_params_ptr->pages_per_block);
	printf("blocks_per_lun             : %d\r\n", nand_onfi_params_ptr->blocks_per_lun);
	printf("lun_count                  : %d\r\n", nand_onfi_params_ptr->lun_count);
	printf("addr_cycles                : %d\r\n", nand_onfi_params_ptr->addr_cycles);
	printf("bits_per_cell              : %d\r\n", nand_onfi_params_ptr->bits_per_cell);
	printf("bb_per_lun                 : %d\r\n", nand_onfi_params_ptr->bb_per_lun);
	printf("block_endurance            : %d\r\n", nand_onfi_params_ptr->block_endurance);
	printf("guaranteed_good_blocks     : %d\r\n", nand_onfi_params_ptr->guaranteed_good_blocks);
	printf("guaranteed_block_endurance : %d\r\n", nand_onfi_params_ptr->guaranteed_block_endurance);
	printf("programs_per_page          : %d\r\n", nand_onfi_params_ptr->programs_per_page);
	printf("ppage_attr                 : %d\r\n", nand_onfi_params_ptr->ppage_attr);
	printf("ecc_bits                   : %d\r\n", nand_onfi_params_ptr->ecc_bits);
	printf("interleaved_bits           : %d\r\n", nand_onfi_params_ptr->interleaved_bits);
	printf("interleaved_ops            : %d\r\n", nand_onfi_params_ptr->interleaved_ops);
	//memcpy(string, (char *)nand_onfi_params_ptr->reserved3, 13); string[13] = 0;
	//printf("reserved3[13]              : %d\r\n", nand_onfi_params_ptr->reserved3[13]);

	printf("\r\nelectrical parameter block\r\n");
	printf("io_pin_capacitance_max     : %d\r\n", nand_onfi_params_ptr->io_pin_capacitance_max);
	printf("async_timing_mode          : 0x%04x\r\n", nand_onfi_params_ptr->async_timing_mode);
	printf("program_cache_timing_mode  : 0x%04x\r\n", nand_onfi_params_ptr->program_cache_timing_mode);
	printf("t_prog                     : %d\r\n", nand_onfi_params_ptr->t_prog);
	printf("t_bers                     : %d\r\n", nand_onfi_params_ptr->t_bers);
	printf("t_r                        : %d\r\n", nand_onfi_params_ptr->t_r);
	printf("t_ccs                      : %d\r\n", nand_onfi_params_ptr->t_ccs);
	printf("src_sync_timing_mode       : 0x%04x\r\n", nand_onfi_params_ptr->src_sync_timing_mode);
	printf("src_ssync_features         : 0x%04x\r\n", nand_onfi_params_ptr->src_ssync_features);
	printf("clk_pin_capacitance_typ    : %d\r\n", nand_onfi_params_ptr->clk_pin_capacitance_typ);
	printf("io_pin_capacitance_typ     : %d\r\n", nand_onfi_params_ptr->io_pin_capacitance_typ);
	printf("input_pin_capacitance_typ  : %d\r\n", nand_onfi_params_ptr->input_pin_capacitance_typ);
	printf("input_pin_capacitance_max  : %d\r\n", nand_onfi_params_ptr->input_pin_capacitance_max);
	printf("driver_strength_support    : %d\r\n", nand_onfi_params_ptr->driver_strength_support);
	printf("t_int_r                    : %d\r\n", nand_onfi_params_ptr->t_int_r);
	printf("t_adl                      : %d\r\n", nand_onfi_params_ptr->t_adl);
	//memcpy(string, (char *)nand_onfi_params_ptr->reserved4, 8); string[8] = 0;
	//printf("reserved4[8]               : %d\r\n", nand_onfi_params_ptr->reserved4[8]);

	printf("\r\nvendor\r\n");
	printf("vendor_revision            : %d\r\n", nand_onfi_params_ptr->vendor_revision);

	struct nand_onfi_vendor_micron *nand_onfi_vendor_micron_ptr = (struct nand_onfi_vendor_micron *)nand_onfi_params_ptr->vendor;
	printf("\r\nnand_onfi_vendor_micron\r\n");
	printf("two_plane_read                    : %d\r\n", nand_onfi_vendor_micron_ptr->two_plane_read);
	printf("read_cache                        : %d\r\n", nand_onfi_vendor_micron_ptr->read_cache);
	printf("read_unique_id                    : %d\r\n", nand_onfi_vendor_micron_ptr->read_unique_id);
	printf("dq_imped                          : %d\r\n", nand_onfi_vendor_micron_ptr->dq_imped);
	printf("dq_imped_num_settings             : %d\r\n", nand_onfi_vendor_micron_ptr->dq_imped_num_settings);
	printf("dq_imped_feat_addr                : %d\r\n", nand_onfi_vendor_micron_ptr->dq_imped_feat_addr);
	printf("rb_pulldown_strength              : %d\r\n", nand_onfi_vendor_micron_ptr->rb_pulldown_strength);
	printf("rb_pulldown_strength_feat_addr    : %d\r\n", nand_onfi_vendor_micron_ptr->rb_pulldown_strength_feat_addr);
	printf("rb_pulldown_strength_num_settings : %d\r\n", nand_onfi_vendor_micron_ptr->rb_pulldown_strength_num_settings);
	printf("otp_mode                          : %d\r\n", nand_onfi_vendor_micron_ptr->otp_mode);
	printf("otp_page_start                    : %d\r\n", nand_onfi_vendor_micron_ptr->otp_page_start);
	printf("otp_data_prot_addr                : %d\r\n", nand_onfi_vendor_micron_ptr->otp_data_prot_addr);
	printf("otp_num_pages                     : %d\r\n", nand_onfi_vendor_micron_ptr->otp_num_pages);
	printf("otp_feat_addr                     : %d\r\n", nand_onfi_vendor_micron_ptr->otp_feat_addr);
	printf("read_retry_options                : %d\r\n", nand_onfi_vendor_micron_ptr->read_retry_options);
	//printf("reserved[72]                      : %d\r\n", nand_onfi_vendor_micron_ptr->reserved[72]);
	printf("param_revision                    : %d\r\n", nand_onfi_vendor_micron_ptr->param_revision);

	//memcpy(string, (char *)nand_onfi_params_ptr->vendor, 88); string[88] = 0;
	//printf("vendor[88]                 : %s\r\n", string);
	printf("\r\ncrc                        : 0x%04x\r\n", nand_onfi_params_ptr->crc);

	u32 NandPageSize;
	NandPageSize = nand_onfi_params_ptr->byte_per_page;


	while(1)
	{

		bytes = xusb_cdc_rx_bytes_available();
		if (bytes == UsbPacketSize) {
			bytes = xusb_cdc_receive_data(usb_rx_buffer, bytes);
			if ((usb_rx_bufferPtr[0] == 0x7e) && (usb_rx_bufferPtr[1] == 0x7f))
			{
				if(usb_rx_bufferPtr[2] == ReadPage)
				{
					usleep(DelayForUsbTransferUs);
					UsbCmdFlashPage = *(u32*)(usb_rx_bufferPtr+4);
					UsbCmdFlashBlock = *(u32*)(usb_rx_bufferPtr+8);
					way = *(u32*)(usb_rx_bufferPtr+12);
					usleep(DelayForUsbTransferUs);
					memset(usb_tx_bufferPtr, 0x00, UsbPacketSize);
					while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
					readpage_00h_30h(way, 0, FLASH(UsbCmdFlashPage, UsbCmdFlashBlock), NandPageSize, (u32)usb_tx_bufferPtr+32);
					usleep(10);
					Xil_DCacheInvalidateRange((u32)usb_tx_bufferPtr+32,NandPageSize);
					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (ReadPage << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);

				}
				else if(usb_rx_bufferPtr[2] == ReadParameter)
				{
					usleep(DelayForUsbTransferUs);
					UsbCmdFlashPage = *(u32*)(usb_rx_bufferPtr+4);
					UsbCmdFlashBlock = *(u32*)(usb_rx_bufferPtr+8);
					way = *(u32*)(usb_rx_bufferPtr+12);
					usleep(DelayForUsbTransferUs);
					memset(usb_tx_bufferPtr, 0x00, UsbPacketSize);
					while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
					readparameterpage(way, (u32)usb_tx_bufferPtr+32);
					usleep(10);
					Xil_DCacheInvalidateRange((u32)usb_tx_bufferPtr+32,NandPageSize);
					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (ReadParameter << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);
				}
				else if(usb_rx_bufferPtr[2] == ProgramPage)
				{
					usleep(DelayForUsbTransferUs);
					UsbCmdFlashPage = *(u32*)(usb_rx_bufferPtr+4);
					UsbCmdFlashBlock = *(u32*)(usb_rx_bufferPtr+8);
					way = *(u32*)(usb_rx_bufferPtr+12);
					memset(usb_tx_bufferPtr, 0x00, UsbPacketSize);
					Xil_DCacheFlushRange((u32)usb_rx_bufferPtr+32,NandPageSize);
					while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
					progpage_80h_10h(way, 0, FLASH(UsbCmdFlashPage, UsbCmdFlashBlock), NandPageSize, (u32)usb_rx_bufferPtr+32);

					*(u32*)(usb_tx_bufferPtr+0) =  0x01007e7f | (ProgramPage << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					*(u32*)(usb_tx_bufferPtr+12) = readstatus_70h(way);
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);
				}
				else if (usb_rx_bufferPtr[2] == EraseBlcok) //
				{
					usleep(DelayForUsbTransferUs);
					UsbCmdFlashBlock = *(u32*)(usb_rx_bufferPtr+8);
					way = *(u32*)(usb_rx_bufferPtr+12);
					while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
					eraseblock_60h_d0h(way,FLASH(0, UsbCmdFlashBlock));
					while((readstatus_70h(way) & (ARDY)) == 0x00);

					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (EraseBlcok << 16);
					*(u32*)(usb_tx_bufferPtr+4) = 0;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					*(u32*)(usb_tx_bufferPtr+12) = readstatus_70h(way);
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);
				}
				else if (usb_rx_bufferPtr[2] == ReadBlock) //
				{
					UsbCmdFlashPage = *(u32*)(usb_rx_bufferPtr+4);//block num
					UsbCmdFlashBlock = *(u32*)(usb_rx_bufferPtr+8);
					way = *(u32*)(usb_rx_bufferPtr+12);
						for (j = 0;j<128;j++)
						{
							usleep(DelayForUsbTransferUs-200);
							memset(usb_tx_bufferPtr, 0x00, UsbPacketSize);
							while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
							readpage_00h_30h(way, 0, FLASH(j, (UsbCmdFlashBlock)), NandPageSize, (u32)usb_tx_bufferPtr+32);
							usleep(200);
							Xil_DCacheInvalidateRange((u32)usb_tx_bufferPtr+32,NandPageSize);

							*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (ReadBlock << 16);
							*(u32*)(usb_tx_bufferPtr+4) = j;
							*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
							xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);

						}

				}
				else if (usb_rx_bufferPtr[2] == TESTWRconsistent) //
				{
					way = *(u32*)(usb_rx_bufferPtr+12);

					u32 tmp;
					tmp = test_WRconsistent(way, NandPageSize);

					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (TESTWRconsistent << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					*(u32*)(usb_tx_bufferPtr+12) = tmp;
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);

				}
				else if (usb_rx_bufferPtr[2] == TESTreadspeed) //
				{
					way = *(u32*)(usb_rx_bufferPtr+12);

					float tmp;
					tmp = test_readspeed(way, NandPageSize);

					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (TESTreadspeed << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					*(float*)(usb_tx_bufferPtr+12) = tmp;
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);

				}
				else if (usb_rx_bufferPtr[2] == TESTwritespeed) //
				{
					way = *(u32*)(usb_rx_bufferPtr+12);

					float tmp;
					tmp = test_writespeed(way, NandPageSize);

					*(u32*)(usb_tx_bufferPtr+0) = 0x01007e7f | (TESTwritespeed << 16);
					*(u32*)(usb_tx_bufferPtr+4) = UsbCmdFlashPage;
					*(u32*)(usb_tx_bufferPtr+8) = UsbCmdFlashBlock;
					*(float*)(usb_tx_bufferPtr+12) = tmp;
					xusb_cdc_send_data(&usb, (u8 *)usb_tx_bufferPtr, UsbPacketSize);

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


