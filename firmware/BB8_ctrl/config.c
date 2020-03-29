#include "config.h"

//系统时钟配置
void CLK_config(void)
{ 
  int i = 0;
  BCSCTL1 = CALBC1_12MHZ;  //信息段A内DCO校正数据 RSELx  
  DCOCTL = CALDCO_12MHZ;  //信息段A内DCO校正数据 DCOx MODx  
  BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0;  //MCLK SMCLK: DCO = 12MHZ 不分频
  BCSCTL3 |= LFXT1S_2;  //低频时钟选择为VLOCLK = 12KHZ
   
  while((IFG1&OFIFG) != 0)
  {
    IFG1 &= ~OFIFG;  //清除OSCFault 标志
    for(i=0xff; i>0; i--);  //延时待稳定
  }
}

//看门狗初始化
void WDT_init(void)
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;  
}

//端口初始化
void Port_init(void)
{
  P1DIR |= BIT0 + BIT6;                            // Set P1.0 P1.6 to output direction
  P1OUT &= ~(BIT0 + BIT6);
  P1OUT &= ~BIT0;
  P1OUT |= BIT6;
}


void Delay_nms(unsigned int n)
{
  for(unsigned int i=0; i<n; i++)
  {
    delay_ms(1);
  }
}