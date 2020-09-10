/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
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

#include "platform_gpio.h"
#include "xparameters.h"

#if defined(XPAR_LEDS_8BITS_BASEADDR)
#define LED_BASE XPAR_LEDS_8BITS_BASEADDR
#elif defined(XPAR_LEDS_4BITS_BASEADDR)
#define LED_BASE XPAR_LEDS_4BITS_BASEADDR
#elif defined(XPAR_LEDS_6BIT_BASEADDR)
#define LED_BASE XPAR_LEDS_6BIT_BASEADDR
#else
#define NO_GPIOS
#endif

#if defined(XPAR_DIP_SWITCHES_8BITS_BASEADDR)
#define DIP_BASE XPAR_DIP_SWITCHES_8BITS_BASEADDR
#elif defined(XPAR_DIPS_4BIT_BASEADDR)
#define DIP_BASE XPAR_DIPS_4BIT_BASEADDR
#elif defined(XPAR_PUSH_BUTTONS_POSITION_BASEADDR)
#define DIP_BASE XPAR_PUSH_BUTTONS_POSITION_BASEADDR 
#elif defined(XPAR_DIP_SWITCHES_4BITS_BASEADDR)
#define DIP_BASE XPAR_DIP_SWITCHES_4BITS_BASEADDR
#else
#define NO_GPIOS
#endif

void
platform_init_gpios()
{
#ifndef NO_GPIOS
    /* set led gpio data direction to output */
    *(volatile unsigned int*)(LED_BASE + 4) = 0;

    /* set dip switch gpio data direction to in */
    *(volatile unsigned int*)(DIP_BASE + 4) = ~0;

    /* initialize leds to OFF */
    *(volatile int *)(LED_BASE) = 0;
#endif
}

int 
toggle_leds()
{
    static int state = 0;
#ifndef NO_GPIOS
    state = ~state;
    *(volatile int *)(LED_BASE) = state;
#endif
    return state;

}

unsigned int 
get_switch_state()
{
#ifdef NO_GPIOS
    return 0x0;
#else
    return *(volatile unsigned int *)(DIP_BASE);
#endif
}
