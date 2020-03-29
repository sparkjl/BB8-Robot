#ifndef __IIC_H
#define __IIC_H
#include "io430.h"

#define IIC_SCL_1  P1OUT |= BIT4 
#define IIC_SCL_0  P1OUT &= ~BIT4
#define IIC_SDA_1  P1OUT |= BIT5
#define IIC_SDA_0  P1OUT &= ~BIT5
#define IIC_SDA_IN  P1IN & BIT5

void IIC_init(void);
void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(unsigned char sdata);
unsigned char IIC_Read_Byte(unsigned char ack);
void Set_IIC_SDA_out(void);
void Set_IIC_SDA_in(void);
void IIC_delay(void);

#endif