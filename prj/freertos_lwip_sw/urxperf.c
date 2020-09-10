/*
 * urxperf.c
 *
 *  Created on: Aug 29, 2012
 *      Author: anirudh
 */

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
#ifdef __arm__
#include "FreeRTOS.h"
#include "task.h"
#include "xil_printf.h"
#endif

static unsigned rxperf_port = 5001;
static unsigned rxperf_server_running = 0;

void print_urxperf_app_header()
{
    xil_printf("%20s %6d %10s %s\r\n", "rxperf server",
                        rxperf_port,
                        rxperf_server_running ? "RUNNING" : "INACTIVE",
                        "$ iperf -c <board ip> -i 5 -t 100 -u -b <bandwidth>");
}

void urxperf_application_thread()
{
	int sock;
	struct sockaddr_in local_addr;

	if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		xil_printf("error creating socket\r\n");
		return;
	}

	memset((void*)&local_addr, 0, sizeof local_addr);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(rxperf_port);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&local_addr, sizeof (local_addr)) < 0) {
#ifdef OS_IS_FREERTOS
			vTaskDelete(NULL);
#endif
			return;
	}

	while (1) {
		char recv_buf[1500];
		/* keep reading data */
		if (lwip_read(sock, recv_buf, 1400) <= 0)
			break;
	}
	xil_printf("Exiting UDP Rx PERF Thread\r\n");
	return;
}
