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

#include "lwip/err.h"
#include "lwip/udp.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif

static unsigned utxperf_port = 5001;
static unsigned txperf_client_connected = 0;

static struct udp_pcb *connected_pcb = NULL;
static struct pbuf *pbuf_to_be_sent = NULL;

#define SEND_BUFSIZE (1450)
static char send_buf[SEND_BUFSIZE];

int
transfer_utxperf_data()
{
	err_t err;
	struct udp_pcb *pcb = connected_pcb;
	static signed int id = 0;
	int *payload;

	pbuf_to_be_sent = pbuf_alloc(PBUF_TRANSPORT, 1450, PBUF_POOL);
	if (!pbuf_to_be_sent) {
		xil_printf("error allocating pbuf to send\r\n");
		return 0;
	}
	else {
		memcpy(pbuf_to_be_sent->payload, send_buf, 1300);
	}

	/* always increment the id */
	payload = (int*)(pbuf_to_be_sent->payload);
	if (id == 0x7FFFFFFE )
		id = 0;
	id++;
	payload[0] = htonl(id);

	err = udp_send(pcb, pbuf_to_be_sent);
	if (err != ERR_OK) {
		xil_printf("Error on udp_send: %d\r\n", err);
		pbuf_free(pbuf_to_be_sent);
		return -1;
	}
	pbuf_free(pbuf_to_be_sent);
	return 0;
}

int
start_utxperf_application()
{
	struct udp_pcb *pcb;
	struct ip_addr ipaddr;
	err_t err;
	u16_t port;
	int i;

	/* create a udp socket */
	pcb = udp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind local address */
	if ((err = udp_bind(pcb, IP_ADDR_ANY, 0)) != ERR_OK) {
		xil_printf("error on udp_bind: %x\n\r", err);
	}

	/* connect to iperf server */
	IP4_ADDR(&ipaddr,  192, 168,   1, 100);		/* iperf server address */
	port = utxperf_port;                            /* iperf default port */
	err = udp_connect(pcb, &ipaddr, port);
	if (err != ERR_OK)
		xil_printf("error on udp_connect: %x\n\r", err);

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	connected_pcb = pcb;
	return 0;
}

void
print_utxperf_app_header()
{
        xil_printf("%20s %6s %10s %s\r\n", "utxperf client",
                        "N/A",
                        txperf_client_connected ? "CONNECTED" : "CONNECTING",
                        "$ iperf -u -s -i 5 (on host with IP 192.168.1.100)");
}
