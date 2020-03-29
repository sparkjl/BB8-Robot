#include "mpu6050.h"
#include "config.h"

//初始化MPU6050
//返回值:0,成功
//    其他,错误代码
unsigned char mpu6050_init(void)
{
  unsigned char res;
  
  IIC_init();
  mpu6050_write_byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
  delay_ms(100);
  mpu6050_write_byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050
  mpu6050_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
  mpu6050_Set_Accel_Fsr(0);					//加速度传感器,±2g
  mpu6050_Set_Rate(50);						//设置采样率50Hz
  mpu6050_write_byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
  mpu6050_write_byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
  mpu6050_write_byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
  mpu6050_write_byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
  res = mpu6050_read_byte(MPU_DEVICE_ID_REG);
  if(res == MPU_ADDR)   //器件ID正确
  {
    mpu6050_write_byte(MPU_PWR_MGMT1_REG,0X01);	   //设置CLKSEL,PLL X轴为参考
    mpu6050_write_byte(MPU_PWR_MGMT2_REG,0X00);	   //加速度与陀螺仪都工作
    mpu6050_Set_Rate(50);     //设置采样率为50Hz
  }else return 1;
  return 0;  
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
unsigned char mpu6050_Set_Gyro_Fsr(unsigned char fsr)
{
  return mpu6050_write_byte(MPU_GYRO_CFG_REG,fsr<<3);   //设置陀螺仪满量程范围  
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
unsigned char mpu6050_Set_Accel_Fsr(unsigned char fsr)
{
  return mpu6050_write_byte(MPU_ACCEL_CFG_REG,fsr<<3);   //设置加速度传感器满量程范围  
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
unsigned char mpu6050_Set_LPF(unsigned int lpf)
{
  unsigned char data=0;
  
  if(lpf >= 188)data = 1;
  else if(lpf >= 98)data = 2;
  else if(lpf >= 42)data = 3;
  else if(lpf >= 20)data = 4;
  else if(lpf >= 10)data = 5;
  else data = 6; 
  return mpu6050_write_byte(MPU_CFG_REG,data);   //设置数字低通滤波器  
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
unsigned char mpu6050_Set_Rate(unsigned int rate)
{
  unsigned char data;
  if(rate > 1000)rate = 1000;
  if(rate < 4)rate = 4;
  data = 1000/rate - 1;
  data = mpu6050_write_byte(MPU_SAMPLE_RATE_REG,data);	     //设置采样率
  return mpu6050_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值
float mpu6050_Get_Temperature(void)
{
  unsigned char buf[2]; 
  short raw;
  float temp;
  mpu6050_read_len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
  raw = ((unsigned char)buf[0] << 8) | buf[1];  
  temp = 36.53 + ((double)raw) / 340;  
  return temp;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
unsigned char mpu6050_Get_Gyroscope(short *gx,short *gy,short *gz)
{
  unsigned char buf[6],res;  
  res = mpu6050_read_len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
  if(res == 0)
  {
    *gx = ((unsigned int)buf[0] << 8) | buf[1];  
    *gy = ((unsigned int)buf[2] << 8) | buf[3];  
    *gz = ((unsigned int)buf[4] << 8) | buf[5];
  } 	
  return res;
}

//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
unsigned char mpu6050_Get_Accelerometer(short *ax,short *ay,short *az)
{
  unsigned char buf[6],res;  
  res = mpu6050_read_len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
  if(res == 0)
  {
    *ax = ((unsigned int)buf[0] << 8) | buf[1];  
    *ay = ((unsigned int)buf[2] << 8) | buf[3];  
    *az = ((unsigned int)buf[4] << 8) | buf[5];
  } 	
  return res;
}

//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
unsigned char mpu6050_write_byte(unsigned char reg, unsigned char data)
{
  IIC_Start();
  IIC_Send_Byte((MPU_ADDR<<1)|0);  //发送器件地址+写命令
  if(IIC_Wait_Ack())  //等待应答
  {
    IIC_Stop();
    return 1;
  }
  IIC_Send_Byte(reg);    //写寄存器数据
  IIC_Wait_Ack();        //等待应答
  IIC_Send_Byte(data);   //发送数据
  if(IIC_Wait_Ack())
  {
    IIC_Stop();
    return 1;    
  }
  IIC_Stop();
  return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
unsigned char mpu6050_read_byte(unsigned char reg)
{
  unsigned char res;
  
  IIC_Start();
  IIC_Send_Byte((MPU_ADDR<<1)|0);  //发送器件地址+写命令
  IIC_Wait_Ack();        //等待应答
  IIC_Send_Byte(reg);    //写寄存器地址
  IIC_Wait_Ack();        //等待应答
  IIC_Start();
  IIC_Send_Byte((MPU_ADDR<<1)|1);  //发送器件地址+读命令
  IIC_Wait_Ack();        //等待应答

  res = IIC_Read_Byte(0);        //读取数据,发送NAck
  IIC_Stop();        //产生一个停止条件

  return res;
}

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
unsigned char mpu6050_write_len(unsigned char addr, unsigned char reg, \
                unsigned char len, unsigned char *buf)
{
  unsigned char i = 0;
  
  IIC_Start();
  IIC_Send_Byte((addr<<1)|0);  //发送器件地址+写命令
  if(IIC_Wait_Ack())  //等待应答
  {
    IIC_Stop();
    return 1;
  }
  IIC_Send_Byte(reg);  //写寄存器地址
  IIC_Wait_Ack();  //等待应答
  for(i=0; i<len; i++)
  {
    IIC_Send_Byte(buf[i]);  //发送数据
    if(IIC_Wait_Ack())  //等待Ack
    {
      IIC_Stop();
      return 1;
    }
  }
  IIC_Stop();
  return 0;
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
unsigned char mpu6050_read_len(unsigned char addr, unsigned char reg, \
                unsigned char len, unsigned char *buf)
{
  IIC_Start();
  IIC_Send_Byte((addr<<1)|0);  //发送器件地址+写命令
  if(IIC_Wait_Ack())  //等待应答
  {
    IIC_Stop();
    return 1;
  }
  IIC_Send_Byte(reg);    //写寄存器地址
  IIC_Wait_Ack();        //等待应答
  IIC_Start();
  IIC_Send_Byte((addr<<1)|1);  //发送器件地址+读命令
  IIC_Wait_Ack();        //等待应答 
  while(len)
  {
    if(len == 1)
    {
      *buf = IIC_Read_Byte(0);  //读数据，发送NAck
    }
    else
    {
      *buf = IIC_Read_Byte(1);  //读数据，发送Ack
    }
    len--;
    buf++;
  }
  IIC_Stop();       //产生一个停止条件
  return 0;
}



