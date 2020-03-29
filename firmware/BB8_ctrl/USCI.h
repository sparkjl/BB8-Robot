#ifndef __USCI_H
#define __USCI_H
#include "io430.h"

extern unsigned char USCIA0_Get_Flag;
extern unsigned char USCIA0_Get_Data;

void UART_init(void);
void UART_SendChar(unsigned char Ch);
unsigned char UART_ReceiveChar(void);
void UART_SendString(unsigned char *str);
//__interrupt void USCIA0_RX_ISR(void);

#endif