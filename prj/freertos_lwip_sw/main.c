/*
 * Copyright (c) 2007-2009 Xilinx, Inc.  All rights reserved.
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
#if __MICROBLAZE__ || __PPC__
#include "xmk.h"
#include "sys/timer.h"
#include "xenv_standalone.h"
#endif
#include "xparameters.h"
#include "lwipopts.h"

#include "platform_config.h"
#include "platform.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "netif/xadapter.h"
#include "lwip/dhcp.h"
#include "config_apps.h"
#if __arm__
#include "task.h"
#include "portmacro.h"
#include "xil_printf.h"
int main_thread();
#endif
void print_headers();
void launch_app_threads();



#define LWIP_DHCP 0
static int complete_nw_thread;
static sys_thread_t main_thread_handle;

#include "xil_cache.h"
#include "xtime_l.h"
#include "NFC.h"
void nand_thrd();

void print_ip(char *msg, struct ip_addr *ip)
{
    print(msg);
    xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
            ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{

    print_ip("Board IP: ", ip);
    print_ip("Netmask : ", mask);
    print_ip("Gateway : ", gw);
}

int main()
{
    if (init_platform() < 0) {
        xil_printf("ERROR initializing platform.\r\n");
        return -1;
    }
#ifndef OS_IS_FREERTOS
    /* start the kernel - does not return */
    xilkernel_main();
#else
    main_thread_handle = sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
                THREAD_STACKSIZE,
                DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
    while(1);
#endif
    return 0;
}

struct netif server_netif;

void network_thread(void *p)
{
    struct netif *netif;
    struct ip_addr ipaddr, netmask, gw;
#if LWIP_DHCP==1
    int mscnt = 0;
#endif
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    netif = &server_netif;

#if LWIP_DHCP==0
    /* initliaze IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168,   1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168,   1,  1);
#endif

    /* print out IP settings of the board */
    print("\r\n\r\n");
    print("-----lwIP Socket Mode Demo Application ------\r\n");

#if LWIP_DHCP==0
    print_ip_settings(&ipaddr, &netmask, &gw);
    /* print all application headers */
#endif

#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#endif
    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\r\n");
        return;
    }
    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);
    complete_nw_thread = 1;
//    vTaskResume(main_thread_handle);
#if LWIP_DHCP==1
    dhcp_start(netif);
    while (1) {
#ifdef OS_IS_FREERTOS
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
#else
		sleep(DHCP_FINE_TIMER_MSECS);
#endif
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
#else
    print_headers();
    launch_app_threads();
    sys_thread_new("nand_thrd", (void(*)(void*))nand_thrd, 0,
                    THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
#ifdef OS_IS_FREERTOS
    vTaskDelete(NULL);
#endif
#endif
    return;
}

int main_thread()
{
#if LWIP_DHCP==1
	int mscnt = 0;
#endif
	/* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);
	/* Suspend Task until auto-negotiation is completed */
	if (!complete_nw_thread)
		vTaskSuspend(NULL);

#if LWIP_DHCP==1
    while (1) {
#ifdef OS_IS_FREERTOS
    	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
#else
    	sleep(DHCP_FINE_TIMER_MSECS);
#endif
		if (server_netif.ip_addr.addr) {
			xil_printf("DHCP request success\r\n");
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			/* print all application headers */
			print_headers();
			/* now we can start application threads */
//			launch_app_threads();
			break;
		}
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= 10000) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(server_netif.ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255,  0);
			IP4_ADDR(&(server_netif.gw),      192, 168,   1,  1);
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			/* print all application headers */
			print_headers();
//			launch_app_threads();
			break;
		}

	}
#ifdef OS_IS_FREERTOS
	vTaskDelete(NULL);
#endif
#endif

    return 0;
}
#ifdef __arm__
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue or
	semaphore is created.  It is also called by various parts of the demo
	application.  If heap_1.c or heap_2.c are used, then the size of the heap
	available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	xil_printf("Memory Allocation Error\r\n");
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* vApplicationStackOverflowHook() will only be called if
	configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2.  The handle and name
	of the offending task will be passed into the hook function via its
	parameters.  However, when a stack has overflowed, it is possible that the
	parameters will have been corrupted, in which case the pxCurrentTCB variable
	can be inspected directly. */
	xil_printf("Stack Overflow in %s\r\n", pcTaskName);
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
void vApplicationSetupHardware( void )
{

}

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

#define send_data(a,b) write(sd, a,b)

u32 CmdFlashBlock = 0;
u32 CmdFlashPage = 0;
u32 way = 1;

u32 NandPageSize;

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

/* thread spawned for each connection */
void process_nand_request(void *p)
{
	int sd = (int)p;
	int RECV_BUF_SIZE = 8704;
	char recv_buf[RECV_BUF_SIZE];
	int n, nwrote;

	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read(sd, rx_buffer, RECV_BUF_SIZE)) < 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
#ifndef OS_IS_FREERTOS
			close(sd);
			return;
#else
			break;
#endif
		}
//		printf("rx %d\r\n", n);
		/* break if the recved message = "quit" */
		if (!strncmp(rx_buffer, "quit", 4))
			break;

		/* break if client closed connection */
		if (n <= 0)
			break;

		/* handle request */
//		if ((nwrote = write(sd, recv_buf, n)) < 0) {
//			xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
//					__FUNCTION__, n, nwrote);
//			xil_printf("Closing socket %d\r\n", sd);
//#ifndef OS_IS_FREERTOS
//			close(sd);
//			return;
//#else
//			break;
//#endif
//		}

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

		}



	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}
void nand_thrd()
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


	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(11256);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
		vTaskDelete(NULL);
		return;
	}

	lwip_listen(sock, 0);

	size = sizeof(remote);
//	new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t*)&size);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new("nand", process_nand_request,
				(void*)new_sd,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		}
	}
}
#endif
