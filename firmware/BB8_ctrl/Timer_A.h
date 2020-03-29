#ifndef __TIMER_A_H
#define __TIMER_A_H
#include "io430.h"

//------------------------------
/***数字舵机 & 双马达控制***/

#define ENA
#define IN1_0  P2OUT &= ~BIT0
#define IN1_1  P2OUT |= BIT0  
#define IN2_0  P2OUT &= ~BIT1
#define IN2_1  P2OUT |= BIT1

#define ENB
#define IN3_0  P2OUT &= ~BIT3
#define IN3_1  P2OUT |= BIT3
#define IN4_0  P2OUT &= ~BIT5
#define IN4_1  P2OUT |= BIT5
//------------------------------

void PWM_config(void);
void PWM_Control_1(unsigned int Val);
void PWM_Control_0(unsigned int Val);

void Servo_Angle_Control(unsigned char angle, unsigned char preangle);
void Motor_Speed_Control(int vL, int vR);

#endif