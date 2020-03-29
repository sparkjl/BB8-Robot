#include "USCI.h"

unsigned char USCIA0_Get_Flag;
unsigned char USCIA0_Get_Data;

void UART_init(void)
{
  P1DIR &= ~BIT1;  //RXD 输入
  P1DIR |= BIT2;  //TXD 输出
  
  P1SEL |= BIT1 + BIT2;
  P1SEL2 |= BIT1 + BIT2;  //选择第二功能：UART
  
  UCA0CTL1 |= UCSWRST;  //配置前，软件复位
  //UCA0CTL0  //默认参数：不校验 8位数据 1位停止位  
  UCA0CTL1 |= UCSSEL_2;  //SMCLK时钟 12MHZ
  /*
  N = F_brclk / Baud_rate = 12MHZ / 9600kps = 1250.0;
  (UCA0BR1 << 8 + UCA0BR0) = (int)N;
  UCA0BRSx = (round)(( N – INT(N)) × 8);
  */
  UCA0BR0 = 0xE2;
  UCA0BR1 = 0x04;
  UCA0MCTL = 0x00;
  UCA0CTL1 &= ~UCSWRST;  //解除软件复位
  //ME1  //msp430g2553无模块使能
  UC0IE |= UCA0RXIE;  //使能串口UART_A0接收中断
  //_BIS_SR(GIE);  
  __bis_SR_register(GIE);  //响应可屏蔽中断
}

void UART_SendChar(unsigned char Ch)
{
  UCA0TXBUF = Ch;

  while((IFG2 & UCA0TXIFG) == 0);
  //IFG2 &= ~UCA0TXIFG;  
}

unsigned char UART_ReceiveChar(void)
{
  unsigned char Ch = 0;

  while((IFG2 & UCA0RXIFG) == 0);
  IFG2 &= ~UCA0RXIFG;
  Ch = UCA0RXBUF;
  
  return Ch;  
}

void UART_SendString(unsigned char *str)
{
  while(*str != '\0')
  {
    UART_SendChar(*str);
    str++;
  }
}

#pragma vector = USCIAB0RX_VECTOR
//__interrupt void USCIA0_RX_ISR(void)
//{
//  while((IFG2 & UCA0TXIFG) == 0);
//  //IFG2 &= ~UCA0TXIFG; 
//  UCA0TXBUF = UCA0RXBUF; 
//}
__interrupt void USCIA0_RX_ISR(void)
{
  while((IFG2 & UCA0RXIFG) == 0);
  IFG2 &= ~UCA0RXIFG;  
  if(USCIA0_Get_Flag == 0)
  {
    USCIA0_Get_Data = UCA0RXBUF;
    USCIA0_Get_Flag = 1;  //数据接收完成    
  }
}