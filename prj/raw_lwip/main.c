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

#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "lwipopts.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif

void print_headers();
int start_applications();
int transfer_data();
void platform_enable_interrupts();
void lwip_init(void);
void tcp_fasttmr(void);
void tcp_slowtmr(void);

int start_nandapp();
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
extern volatile int TxPerfConnMonCntr;
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;


#include "xil_cache.h"
#include "xtime_l.h"
#include "NFC.h"
void print_ip(char *msg, struct ip_addr *ip)
{
    print(msg);
    xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{
    print_ip("Board IP:       ", ip);
    print_ip("Netmask :       ", mask);
    print_ip("Gateway :       ", gw);
}

int main()
{
	struct netif *netif, server_netif;
	struct ip_addr ipaddr, netmask, gw;

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

	if (init_platform() < 0) {
		xil_printf("ERROR initializing platform.\r\n");
		return -1;
	}

	xil_printf("\r\n\r\n");
	xil_printf("-----lwIP RAW Mode Demo Application ------\r\n");
	/* initliaze IP addresses to be used */
#if (LWIP_DHCP==0)
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
    print_ip_settings(&ipaddr, &netmask, &gw);
#endif
	lwip_init();

#if (LWIP_DHCP==1)
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#endif

	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return -1;
	}
	netif_set_default(netif);

	/* specify that the network if is up */
	netif_set_up(netif);

	/* now enable interrupts */
	platform_enable_interrupts();

#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(netif);
	dhcp_timoutcntr = 24;
	TxPerfConnMonCntr = 0;
	while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0)) {
		xemacif_input(netif);
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
	}
	if (dhcp_timoutcntr <= 0) {
		if ((netif->ip_addr.addr) == 0) {
			xil_printf("DHCP Timeout\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(netif->ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(netif->gw),      192, 168,   1,  1);
		}
	}
	/* receive and process packets */
	print_ip_settings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif

	/* start the application (web server, rxtest, txtest, etc..) */
//	start_applications();
	start_nandapp();
	print_headers();

	while (1) {
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(netif);
		transfer_data();
	}

    /* never reached */
    cleanup_platform();

	return 0;
}

#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwipopts.h"

static uint8_t rx_buffer[2048*8] __attribute__ ((__aligned__(32)));
static uint8_t tx_buffer[2048*8] __attribute__ ((__aligned__(32)));
static uint8_t * rx_bufferPtr = (uint8_t *)rx_buffer;
static uint8_t * tx_bufferPtr = (uint8_t *)tx_buffer;

uint64_t warray[2048] = {0};
uint64_t rarray[2048] = {0};
uint16_t *wptr = (uint16_t *)warray;
uint16_t *rptr = (uint16_t *)rarray;


#define TransferPacketSize 8704
#define FLASH(PAGE,BLOCK) ((PAGE & 0x00000007f) | (BLOCK << 7))

#define DelayForTransferUs 500

#define ReadPage 0x00
#define ReadParameter 0x01
#define ProgramPage 0x02
#define EraseBlcok 0x03
#define ReadBlock 0x04
#define TESTWRconsistent 0x0A
#define TESTwritespeed 0x0B
#define TESTreadspeed 0x0C



#define send_data(a,b) p->payload = a;\
					p->len = b;\
					tcp_write(tpcb, p->payload, p->len, 1);


u32 CmdFlashBlock = 0;
u32 CmdFlashPage = 0;
u32 way = 1;
u32 NandPageSize;
static unsigned nand_server_running = 0;


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
#define Block 128
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

int
transfer_nand_data() {
    return 0;
}
u32 rx_size = 0;
static err_t
nand_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    /* close socket if the peer has sent the FIN packet  */
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    /* all we do is say we've received the packet */
    /* we don't actually make use of it */
    tcp_recved(tpcb, p->tot_len);

    if (rx_size < TransferPacketSize)
    {
    	memcpy(rx_bufferPtr, p->payload, p->tot_len);
    	rx_size = rx_size + p->tot_len;
    	rx_bufferPtr = rx_bufferPtr + p->tot_len;
    }
    if (rx_size == TransferPacketSize)
    {
    	rx_bufferPtr = rx_buffer;
    	rx_size = 0;
		if ((rx_bufferPtr[0] == 0x7e) && (rx_bufferPtr[1] == 0x7f))
		{
			if(rx_bufferPtr[2] == ReadPage)
			{
				CmdFlashPage = *(u32*)(rx_bufferPtr+4);
				CmdFlashBlock = *(u32*)(rx_bufferPtr+8);
				way = *(u32*)(rx_bufferPtr+12);
				memset(tx_bufferPtr, 0x00, TransferPacketSize);
				while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
				readpage_00h_30h(way, 0, FLASH(CmdFlashPage, CmdFlashBlock), NandPageSize, (u32)tx_bufferPtr+32);
//				usleep(10);
				Xil_DCacheInvalidateRange((u32)tx_bufferPtr+32,NandPageSize);
				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (ReadPage << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);
//				send_data(rx_bufferPtr, n);

			}
			else if(rx_bufferPtr[2] == ReadParameter)
			{
				usleep(DelayForTransferUs);
				CmdFlashPage = *(u32*)(rx_bufferPtr+4);
				CmdFlashBlock = *(u32*)(rx_bufferPtr+8);
				way = *(u32*)(rx_bufferPtr+12);
				usleep(DelayForTransferUs);
				memset(tx_bufferPtr, 0x00, TransferPacketSize);
				while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
				readparameterpage(way, (u32)tx_bufferPtr+32);
				usleep(10);
				Xil_DCacheInvalidateRange((u32)tx_bufferPtr+32,NandPageSize);
				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (ReadParameter << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);
			}
			else if(rx_bufferPtr[2] == ProgramPage)
			{
				usleep(DelayForTransferUs);
				CmdFlashPage = *(u32*)(rx_bufferPtr+4);
				CmdFlashBlock = *(u32*)(rx_bufferPtr+8);
				way = *(u32*)(rx_bufferPtr+12);
				memset(tx_bufferPtr, 0x00, TransferPacketSize);
				Xil_DCacheFlushRange((u32)rx_bufferPtr+32,NandPageSize);
				while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
				progpage_80h_10h(way, 0, FLASH(CmdFlashPage, CmdFlashBlock), NandPageSize, (u32)rx_bufferPtr+32);

				*(u32*)(tx_bufferPtr+0) =  0x01007e7f | (ProgramPage << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				*(u32*)(tx_bufferPtr+12) = readstatus_70h(way);
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);
			}
			else if (rx_bufferPtr[2] == EraseBlcok) //
			{
				usleep(DelayForTransferUs);
				CmdFlashBlock = *(u32*)(rx_bufferPtr+8);
				way = *(u32*)(rx_bufferPtr+12);
				while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
				eraseblock_60h_d0h(way,FLASH(0, CmdFlashBlock));
				while((readstatus_70h(way) & (ARDY)) == 0x00);

				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (EraseBlcok << 16);
				*(u32*)(tx_bufferPtr+4) = 0;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				*(u32*)(tx_bufferPtr+12) = readstatus_70h(way);
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);
			}
			else if (rx_bufferPtr[2] == ReadBlock) //
			{
				CmdFlashPage = *(u32*)(rx_bufferPtr+4);//block num
				CmdFlashBlock = *(u32*)(rx_bufferPtr+8);
				way = *(u32*)(rx_bufferPtr+12);
					for (u32 j = 0;j<128;j++)
					{
						usleep(DelayForTransferUs-200);
						memset(tx_bufferPtr, 0x00, TransferPacketSize);
						while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
						readpage_00h_30h(way, 0, FLASH(j, (CmdFlashBlock)), NandPageSize, (u32)tx_bufferPtr+32);
						usleep(200);
						Xil_DCacheInvalidateRange((u32)tx_bufferPtr+32,NandPageSize);

						*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (ReadBlock << 16);
						*(u32*)(tx_bufferPtr+4) = j;
						*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
						send_data((u8 *)tx_bufferPtr, TransferPacketSize);

					}

			}
			else if (rx_bufferPtr[2] == TESTWRconsistent) //
			{
				way = *(u32*)(rx_bufferPtr+12);

				u32 tmp;
				tmp = test_WRconsistent(way, NandPageSize);

				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (TESTWRconsistent << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				*(u32*)(tx_bufferPtr+12) = tmp;
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);

			}
			else if (rx_bufferPtr[2] == TESTreadspeed) //
			{
				way = *(u32*)(rx_bufferPtr+12);

				float tmp;
				tmp = test_readspeed(way, NandPageSize);

				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (TESTreadspeed << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				*(float*)(tx_bufferPtr+12) = tmp;
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);

			}
			else if (rx_bufferPtr[2] == TESTwritespeed) //
			{
				way = *(u32*)(rx_bufferPtr+12);

				float tmp;
				tmp = test_writespeed(way, NandPageSize);

				*(u32*)(tx_bufferPtr+0) = 0x01007e7f | (TESTwritespeed << 16);
				*(u32*)(tx_bufferPtr+4) = CmdFlashPage;
				*(u32*)(tx_bufferPtr+8) = CmdFlashBlock;
				*(float*)(tx_bufferPtr+12) = tmp;
				send_data((u8 *)tx_bufferPtr, TransferPacketSize);

			}
			tcp_output(tpcb);

		}
    }
//    if (tcp_sndbuf(tpcb) > p->len) {
//        err = tcp_write(tpcb, p->payload, p->len, 1);
//    }

    pbuf_free(p);
    return ERR_OK;
}

err_t
nand_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    xil_printf("rxperf: Connection Accepted\r\n");
    tcp_recv(newpcb, nand_recv_callback);

    return ERR_OK;
}


int start_nandapp()
{

    reset_ffh(1);
    setfeature_efh(1, 0x14000000);
	memset(tx_bufferPtr, 0x00, TransferPacketSize);
	while((readstatus_70h(way) & (ARDY | RDY)) == 0x00);
	readparameterpage(way, (u32)tx_bufferPtr+32);
	usleep(10);
	Xil_DCacheInvalidateRange((u32)tx_bufferPtr+32,256);
	struct nand_onfi_params *nand_onfi_params_ptr = (struct nand_onfi_params *)(tx_bufferPtr + 32);
	NandPageSize = nand_onfi_params_ptr->byte_per_page;
    xil_printf("-----Nand Flash Reset ------\r\n");

    struct tcp_pcb *pcb;
    err_t err;

    /* create new TCP PCB structure */
    pcb = tcp_new();
    if (!pcb) {
    	xil_printf("rxperf: Error creating PCB. Out of Memory\r\n");
    	return -1;
    }

    /* bind to iperf @port */
    err = tcp_bind(pcb, IP_ADDR_ANY, 11256);
    if (err != ERR_OK) {
    	xil_printf("rxperf: Unable to bind to port %d: err = %d\r\n", 11256, err);
    	return -2;
    }

    /* we do not need any arguments to callback functions :) */
    tcp_arg(pcb, NULL);

    /* listen for connections */
    pcb = tcp_listen(pcb);
    if (!pcb) {
    	xil_printf("rxperf: Out of memory while tcp_listen\r\n");
    	return -3;
    }

    /* specify callback to use for incoming connections */
    tcp_accept(pcb, nand_accept_callback);

    nand_server_running = 1;

    return 0;
}
