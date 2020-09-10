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
#include <string.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwipopts.h"
#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

#if (USE_JUMBO_FRAMES==1)
#define SEND_BUFSIZE (9000)
#else
#define SEND_BUFSIZE (1440)
#endif

static char send_buf[SEND_BUFSIZE];

void
print_txperf_app_header()
{
        xil_printf("%20s %6s %s\r\n", "txperf client",
                        "N/A",
                        "$ iperf -s -i 5 -t 100 (on host with IP 192.168.1.100)");
}

void tx_application_thread()
{
	struct ip_addr ipaddr;
	int i, sock;
	struct sockaddr_in serv_addr;

	IP4_ADDR(&ipaddr,  192, 168,   1, 100);		/* iperf server address */

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("error creating socket\r\n");
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5001);
	serv_addr.sin_addr.s_addr = ipaddr.addr;

	print("Connecting to iperf server...");
	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		xil_printf("error in connect\r\n");
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}
	print("Connected\n\r");

	while (lwip_write(sock, send_buf, SEND_BUFSIZE) >= 0) {
		;
	}

	print("TX perf stopped\r\n");
#ifdef OS_IS_FREERTOS
	vTaskDelete(NULL);
#endif
	return;
}
