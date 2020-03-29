#include "Timer_A.h"
#include "config.h"

void PWM_config(void)
{
  /***控制端口配置 ***/
  //ENA
  //ENB
  //对应IN1、IN2、IN3、IN4
  P2DIR |= BIT0 + BIT1 + BIT3 + BIT5;
  P2OUT |= BIT0 + BIT1 + BIT3 + BIT5;
  
  /***TA1 2路PWM配置***/
  P2DIR |= BIT2 + BIT4;  //输出模式
  P2SEL |= BIT2 + BIT4;  //P2.2 -> TA1.1 P2.4 -> TA1.2 比较输出
  P2SEL2 &= ~(BIT2 + BIT4);
  
  TA1CTL |= TASSEL_2 + ID_2 + MC_0 + TACLR;  //停止模式下设置: MCLK,4分频,=3MHZ清除TAR
  TA1CCR0 = 60000;
  TA1CCTL1 = OUTMOD_7;  //PWM复位/置位
  TA1CCR1 = 0;
  TA1CCTL2 = OUTMOD_7;
  TA1CCR2 = 0;
  
  TA1CTL |= MC_1;  //增计数模式

  /***TA0 1路PWM配置***/  
  P2DIR |= BIT6;  //输出模式
  P2SEL |= BIT6;  //P2.6 -> TA0.1 比较输出
  P2SEL &= ~BIT7;
  P2SEL2 &= ~(BIT6 + BIT7);
  
  TA0CTL |= TASSEL_2 + ID_2 + MC_0 + TACLR;  //停止模式下设置: MCLK,4分频,=3MHZ清除TAR
  TA0CCTL0 |= CCIE;  // 捕获/比较中断开启
  TA0CCR0 = 60000;  //周期: 3MHZ / 60000 = 50HZ
  TA0CCTL1 = OUTMOD_7;  //PWM复位/置位
  TA0CCR1 = 4500;
  
  TA0CTL |= MC_1;  //增计数模式  
}

//修改TA1 输出PWM的占空比
void PWM_Control_1(unsigned int Val)
{
//  TA1CTL |= MC_0 + TACLR;   //暂停并清除计数
  TA1CCR1 = Val; 
  TA1CCR2 = Val;
  
  TA1CTL |= MC_1;  //增计数模式
}

//修改TA0 输出PWM的占空比
void PWM_Control_0(unsigned int Val)
{
  TA0CTL |= MC_0 + TACLR;   //暂停并清除计数
  TA0CCR1 = Val; 
  
  TA0CTL |= MC_1;  //增计数模式
}

unsigned char Capture_count = 0;
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
  if(Capture_count < 50)
  {
    Capture_count++;
  }
  else
  {
    P1OUT ^= BIT0;
    P1OUT ^= BIT6;    
    Capture_count = 0;  
  }
}

//------------------------------
/***数字舵机 & 双马达控制***/
//------------------------------

//数字舵机角度控制: 0度 -> 180度
//PWM: 周期: 50HZ,即20ms   控制范围: 500us -> 2500us
//angle: 控制角度     preangle: 当前角度  1度对应计数 = (7500 - 1500) / 180 = 33.3333333 
void Servo_Angle_Control(unsigned char angle, unsigned char preangle)
{
  unsigned char angle_temp;
  
  angle_temp = preangle;
  if(preangle < angle)
  {
    while(angle_temp < angle)
    {
      angle_temp++;
      PWM_Control_0((unsigned int)(1500 + 33.3333333 * angle_temp));
      delay_ms(20);
    }
  }
  else if(preangle > angle)
  {
    while(angle_temp > angle)
    {
      angle_temp--;
      PWM_Control_0((unsigned int)(1500 + 33.3333333 * angle_temp));
      delay_ms(20);
    }    
  }
  else  //相等时直接控制
  {
    PWM_Control_0((unsigned int)(1500 + 33.3333333 * angle_temp));
    delay_ms(20);
  }
}

//速度范围: 0 -> 255
void Motor_Speed_Control(int vL, int vR)
{
  if(vL >= 0 && vR >= 0)  //前进
  {
    IN1_0; IN2_1;
    IN3_0; IN4_1;
  }
  else if(vL >= 0 && vR < 0)  //右转
  {
    IN1_1; IN2_0;
    IN3_0; IN4_1;    
  }
  else if(vL < 0 && vR >= 0)  //左转
  {
    IN1_0; IN2_1;
    IN3_1; IN4_0;    
  }
  else if(vL < 0 && vR < 0)  //后退
  {
    IN1_1; IN2_0;
    IN3_1; IN4_0;    
  }
  
  if(vL < 0)  //或vR
  {
    vL = -vL;
  }
  
  vL = (int)((float)vL / 255 * 60000);  //设置50HZ PWM的占空比
  PWM_Control_1(vL);
}

