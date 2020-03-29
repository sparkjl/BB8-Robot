#include "IIC.h"
#include "config.h"

void IIC_init(void)
{
  P1DIR |= BIT4 + BIT5;
  //P1REN |= BIT4 + BIT5;  //上拉/下拉电阻使能
  P1OUT |= BIT4 + BIT5;  //对应引脚上拉
}

void IIC_Start(void)
{
  Set_IIC_SDA_out();  //SDA 输出
  IIC_SDA_1;
  IIC_SCL_1;
  IIC_delay();
  IIC_SDA_0;  //SCL = 1时，SDA产生下降沿
  IIC_delay();
  IIC_SCL_0;  //钳住IIC总线，准备发送或接收数据  
}

void IIC_Stop(void)
{
  Set_IIC_SDA_out();  //SDA 输出
  IIC_SCL_0;  
  IIC_SDA_0;
  IIC_delay();
  IIC_SCL_1;
  IIC_SDA_1;  //SCL = 1时，SDA产生上升沿
  IIC_delay();  
}

unsigned char IIC_Wait_Ack(void)
{
  unsigned char ucErrTime = 0;
    
  Set_IIC_SDA_in();
  IIC_SDA_1;  //设置上拉输入
  //Set_IIC_SDA_in();
  IIC_delay();
  IIC_SCL_1; 
  IIC_delay();
  
  while(IIC_SDA_IN)
  {
    ucErrTime++;
    if(ucErrTime > 250)
    {
      IIC_Stop();
      return 1;
    }    
  }
  IIC_SCL_0;
  return 0;
}

void IIC_Ack(void)
{
  IIC_SCL_0;
  Set_IIC_SDA_out();
  IIC_SDA_0;
  IIC_delay();
  IIC_SCL_1;
  IIC_delay();
  IIC_SCL_0;  
}

void IIC_NAck(void)
{
  IIC_SCL_0;
  Set_IIC_SDA_out();
  IIC_SDA_1;
  IIC_delay();
  IIC_SCL_1;
  IIC_delay();
  IIC_SCL_0;  
}

void IIC_Send_Byte(unsigned char sdata)
{
  unsigned char t = 0;

  Set_IIC_SDA_out();
  IIC_SCL_0;
  for(t=0; t<8; t++)
  {
    if(sdata & 0x80)
    {
      IIC_SDA_1;
    }
    else
    {
      IIC_SDA_0;
    }
    sdata <<= 1;
    IIC_SCL_1;
    IIC_delay();
    IIC_SCL_0;
    IIC_delay();
  }
}

//读1个字节，ack=1时，发送ACK，ack=0，发送NACK
unsigned char IIC_Read_Byte(unsigned char ack)
{
  unsigned char i, rdata = 0;
  
  //IIC_SDA_1;
  Set_IIC_SDA_in();
  IIC_SDA_1;
  for(i=0; i<8; i++)
  {
    IIC_SCL_0;
    IIC_delay();
    IIC_SCL_1;
    rdata <<= 1;
    if(IIC_SDA_IN)
    {
      rdata++;  
    }
    IIC_delay();
  }
  if(ack)
  {
    IIC_Ack();  
  }
  else
  {
    IIC_NAck();
  }
  return rdata;
}

void Set_IIC_SDA_out(void)
{
  P1DIR |= BIT5;  //SDA 输出
}

void Set_IIC_SDA_in(void)
{
  P1DIR &= ~BIT5;  //SDA 输入  
}

void IIC_delay(void)
{
  delay_us(2);  
}