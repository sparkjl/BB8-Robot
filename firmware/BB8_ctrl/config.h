#ifndef __CONFIG_H
#define __CONFIG_H
#include "io430.h"
#include "stdio.h"

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define CPU_F  ((double)12000000)
#define delay_us(x)  __delay_cycles((long)(CPU_F * (double)x / 1000000.0))
#define delay_ms(x)  __delay_cycles((long)(CPU_F * (double)x / 1000.0))

#define LED1_ON   P1OUT |= BIT0
#define LED1_OFF  P1OUT &= ~BIT0
#define LED2_ON   P1OUT |= BIT6
#define LED2_OFF  P1OUT &= ~BIT6

void CLK_config(void);
void WDT_init(void);
void Port_init(void);
void Delay_nms(unsigned int n);

#endif