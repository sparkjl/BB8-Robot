#include "ANO_Tech.h"
#include "USCI.h"
#include "string.h"

void Send_data_to_ANO(unsigned char fun, unsigned char *data, unsigned char len)
{
  unsigned char send_buf[32];
  unsigned char i;

  if (len > 28) return;	   //最多28字节数据 
  send_buf[len+3] = 0;	  //校验数置零
  send_buf[0] = 0X88;	 //帧头
  send_buf[1] = fun;	//功能字
  send_buf[2] = len;	//数据长度
  for(i=0;i<len;i++) send_buf[3+i] = data[i];			//复制数据
  for(i=0;i<len+3;i++) send_buf[len+3] += send_buf[i];	        //计算校验和	
  for(i=0;i<len+4;i++) UART_SendChar(send_buf[i]);	        //发送数据到串口 
}

//通过串口传送计算后的姿态数据给ANO_Tech
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void ANO_report_1(short aacx,short aacy,short aacz,short gyrox,short gyroy, \
  short gyroz,short roll,short pitch,short yaw)
{
  unsigned char tbuf[28];
  
  memset(tbuf, 0, sizeof(tbuf));
  tbuf[0] = (aacx>>8) & 0XFF;
  tbuf[1] = aacx & 0XFF;
  tbuf[2] = (aacy>>8) & 0XFF;
  tbuf[3] = aacy & 0XFF;
  tbuf[4] = (aacz>>8) & 0XFF;
  tbuf[5] = aacz & 0XFF; 
  tbuf[6] = (gyrox>>8) & 0XFF;
  tbuf[7] = gyrox & 0XFF;
  tbuf[8] = (gyroy>>8) & 0XFF;
  tbuf[9] = gyroy & 0XFF;
  tbuf[10] = (gyroz>>8) & 0XFF;
  tbuf[11] = gyroz & 0XFF;	
  tbuf[18] = (roll>>8) & 0XFF;
  tbuf[19] = roll & 0XFF;
  tbuf[20] = (pitch>>8) & 0XFF;
  tbuf[21] = pitch & 0XFF;
  tbuf[22] = (yaw>>8) & 0XFF;
  tbuf[23] = yaw & 0XFF;
  
  Send_data_to_ANO(0XAF, tbuf, 28);    //飞控显示帧,0XAF  
}

//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
void ANO_report_2(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
  unsigned char tbuf[12];
  
  tbuf[0] = (aacx>>8) & 0XFF;
  tbuf[1] = aacx & 0XFF;
  tbuf[2] = (aacy>>8) & 0XFF;
  tbuf[3] = aacy & 0XFF;
  tbuf[4] = (aacz>>8) & 0XFF;
  tbuf[5] = aacz & 0XFF; 
  tbuf[6] = (gyrox>>8) & 0XFF;
  tbuf[7] = gyrox & 0XFF;
  tbuf[8] = (gyroy>>8) & 0XFF;
  tbuf[9] = gyroy & 0XFF;
  tbuf[10] = (gyroz>>8) & 0XFF;
  tbuf[11] = gyroz & 0XFF;
  
  Send_data_to_ANO(0XA1, tbuf, 12);    //自定义帧,0XA1  
}

