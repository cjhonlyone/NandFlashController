/*
 * Copyright (c) 2008 Xilinx, Inc.  All rights reserved.
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
#include <string.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwipopts.h"
#if __MICROBLAZE__ || __PPC__
#include "sys/timer.h"
#endif

#ifdef __arm__
#include "xil_printf.h"
#endif

void udpsend_thread(void *p)
{
	int sd;
	struct sockaddr_in server;
	struct sockaddr_in to;
	int BUFSIZE = 8192;
	char buf[BUFSIZE];
	struct ip_addr to_ipaddr;
	int n, i;

	/* create a new socket to send responses to this client */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		xil_printf("%s: error creating socket, return value = %d\r\n", __FUNCTION__, sd);
		return ;
	}

	memset(&server, 0, sizeof server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 9130;
	if (lwip_bind(sd, (struct sockaddr *)&server, sizeof server) < 0)  {
		printf("error binding");
		return ;
	}

	IP4_ADDR(&to_ipaddr,  192, 168,   1, 100);
	memset(&to, 0, sizeof to);
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = to_ipaddr.addr;
	to.sin_port = 9123;

	memset(buf, 0, sizeof buf);
	buf[0] = '0'; buf[1] = '0';
	buf[1400] = '1'; buf[1401] = '1';
	buf[1500] = '2'; buf[1501] = '2';
	buf[2000] = '3'; buf[2001] = '3';

	/* send one packet to create ARP entry */
	lwip_sendto(sd, buf, 1024, 0, (struct sockaddr *)&to, sizeof to);

	/* wait until receive'd arp entry updates ARP cache */
#if __MICROBLAZE__ || __PPC__
	sleep(20);
#endif
#ifdef __arm__
	vTaskDelay(20);
#endif

	/* now send real packets */
	for (i = 0; i < 10; i++) {
		n = lwip_sendto(sd, buf, sizeof buf, 0, (struct sockaddr *)&to, sizeof to);
		xil_printf("sent bytes = %d\r\n", n);
	}
}
