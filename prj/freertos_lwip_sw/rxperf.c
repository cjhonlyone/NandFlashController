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
#if __arm__
#include "xil_printf.h"
#endif
#include "lwipopts.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "FreeRTOS.h"
#include "task.h"

static unsigned rxperf_port = 5001;	/* iperf default port */

void print_rxperf_app_header()
{
        xil_printf("%20s %6d %s\r\n", "rxperf server",
                        rxperf_port,
                        "$ iperf -c <board ip> -i 5 -t 50 -w 64k");
}

void rx_application_thread()
{
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(rxperf_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}

	lwip_listen(sock, 0);

	size = sizeof(remote);
	new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t*)&size);

	while (1) {
#if (USE_JUMBO_FRAMES==1)
		char recv_buf[9700];
		/* keep reading data */
		if (lwip_read(new_sd, recv_buf, 8000) <= 0)
			break;
#else
		char recv_buf[1500];
		/* keep reading data */
		if (lwip_read(new_sd, recv_buf, 1460) <= 0)
			break;
#endif
	}

	print("Connection closed. RXPERF exiting.\r\n");

    lwip_close(new_sd);
#ifdef OS_IS_FREERTOS
	xil_printf("Rx IPERF Thread is being DELETED\r\n");
    vTaskDelete(NULL);
#endif
}
