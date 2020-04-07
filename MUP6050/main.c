#include <msp430g2553.h>
#include <math.h>
#include "INTRINSICS.H"
#include "lcd1602.h"
#define uchar unsigned char
#define uint  unsigned int
//#define CPU_F ((double)16000000)
//#define delayus(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))  //宏定义延时函数
//#define delayms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define pi 3.141592
#define Gry_offset -0.3         // 陀螺仪X偏移量
#define Gry_offsety -0.6        // 陀螺仪Y偏移量
#define Gry_offsetz 1.4         // 陀螺仪Z偏移量
#define Gyr_Gain 131            // 满量程250dps时灵敏度(dps/digital)
/************管脚定义******************/
#define SCL1 P2OUT |=BIT0
#define SCL0 P2OUT &=~BIT0
#define SCLOUT P2DIR |= BIT0
#define SDA1 P2OUT |=BIT1       //IIC数据引脚
#define SDA0 P2OUT &=~BIT1
#define SDAIN P2DIR &=~BIT1
#define SDAOUT P2DIR |=BIT1
#define SDADATA (P2IN & BIT1)
/************管脚定义******************/
#define SMPLRT_DIV  0x19        //陀螺仪采样率典型值0x07(125Hz)
#define CONFIG   0x1A           //低通滤波频率典型值0x06(5Hz)
#define GYRO_CONFIG  0x1B       //陀螺仪自检及测量范围典型值0x18(不自检2000deg/s)
#define ACCEL_CONFIG 0x1C       //加速计自检、测量范围及高通滤波频率典型值0x01(不自检2G5Hz)
#define ACCEL_XOUT_H 0x3B       //X轴加速度高位
#define ACCEL_XOUT_L 0x3C       //X轴加速度低位
#define ACCEL_YOUT_H 0x3D       //Y轴加速度高位
#define ACCEL_YOUT_L 0x3E       //Y轴加速度低位
#define ACCEL_ZOUT_H 0x3F       //Z轴加速度高位
#define ACCEL_ZOUT_L 0x40       //Z轴加速度低位
#define TEMP_OUT_H  0x41        //温度相关寄存器高位
#define TEMP_OUT_L  0x42        //温度相关寄存器低位
#define GYRO_XOUT_H  0x43       //X轴角速度高位
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45       //Y轴角速度高位
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47       //Z轴角速度高位
#define GYRO_ZOUT_L  0x48
#define PWR_MGMT_1  0x6B        //电源管理典型值0x00(正常启用)
#define WHO_AM_I   0x75         //IIC地址寄存器(默认数值0x68只读)
#define SlaveAddress 0xD0       //IIC写入时的地址字节数据+1为读取  //显示数字(-511至512)的字符数组
char dis[15];
unsigned char dis1[15];
int dis_data;
int ax,ay,az,gx,gy,gz;
int AX,AY,AZ,GX,GY,GZ;          //调整后的姿态
float kalman1,kalman2,kalman3;
float bias=0.05;                //定义偏移量，可调整精度大小
float biasy=0.05;
float biasz=0.05;
float P_00=0.001,P_01=0,P_10=0,P_11=0;
float Py_00=0.001,Py_01=0,Py_10=0,Py_11=0;
float Pz_00=0.001,Pz_01=0,Pz_10=0,Pz_11=0;
float Q_angle=0.01;             //bilievble for x
float Q_omega=0.0003;           //bilievble foe y
float R_angle=0.01;             //noise variance
long time_now,time_past;
float dt;                       //sampling time
uint flag_kalman;


void delayus(uint z)
{
    uint i,j;
      for(i = 0;i<20;i++)
        for(j=0;j<z;j++)
        {;;};
}

//**************************************
//I2C起始信号
//**************************************
void I2C_Start()
{
    SDAOUT;
    SCLOUT;
    SDA1;                       //拉高数据线
    SCL1;                       //拉高时钟线
    delayus(5);                 //延时
    SDA0;                       //产生下降沿
    delayus(5);                 //延时
    SCL0;                       //拉低时钟线
}

//**************************************
//I2C停止信号
//**************************************
void I2C_Stop()
{
    SDAOUT;
    SCLOUT;
    SDA0;                       //拉低数据线
    delayus(5);                 //延时
    SCL1;                       //拉高时钟线
    delayus(5);                 //延时
    SDA1;                       //产生上升沿
}

//**************************************
//I2C发送应答信号
//入口参数:ack (0:ACK 1:NAK)
//**************************************
void I2C_SendACK(uchar ack)
{
    SDAOUT;
    SCLOUT;
    //SDA = ack;                //写应答信号
    if(ack) SDA1;
       else SDA0;
    SCL1;                       //拉高时钟线
    delayus(5);                 //延时
    SCL0;                       //拉低时钟线
    delayus(5);                 //延时
}

//**************************************
//I2C接收应答信号
//**************************************
uint I2C_RecvACK()
{
    uchar cy;
    SCLOUT;
    SDAOUT;
    SDA1;
    SCL1;                       //拉高时钟线
    delayus(2);                 //延时
    SDAIN;
   // while(P2IN & BIT1)
    //    SCL0;

    if(SDADATA)
    {
        cy=1;//???
    }
    else
    {
        cy=0;
    }
    //cy = SDA;                 //读应答信号
    SCL0;                       //拉低时钟线
    delayus(5);                 //延时
    SDAOUT;
    return cy;
}

//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(uchar dat)
{
    uchar i;
    SDAOUT;
    SCLOUT;
    for (i=0; i<8; i++)         //8位计数器
    {
        if((dat<<i)&0x80)
        {
            SDA1;
        }
        else
        {
            SDA0;
        }
        SCL1;                   //拉高时钟线
        delayus(5);             //延时
        SCL0;                   //拉低时钟线
        delayus(5);             //延时
    }
   // SCL0;
  //  delayus(2);
  //  SDA1;
  //  delayus(2);
    I2C_RecvACK();
}

//**************************************
//从I2C总线接收一个字节数据
//**************************************
uchar I2C_RecvByte()
{
    uchar i;
    uchar dat = 0,cy;
    SDAOUT;
    SCLOUT;
    SDA1;                       //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        SDAIN;
        dat <<= 1;
        SCL1;                   //拉高时钟线???
        if(SDADATA)
        {
            cy=1;
        }
          else
          {
              cy=0;
          }
        delayus(5);
        dat |= cy;              //读数据
        SCL0;                   //拉低时钟线
        delayus(5);
    }

    return dat;
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(uchar REG_Address,uchar REG_data)
{

    SDAOUT;
    SCLOUT;
    SDA1;
    SCL1;
    I2C_Start();                  //起始信号
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
   // I2C_RecvACK();

    I2C_SendByte(REG_Address);    //内部寄存器地址，
    //I2C_RecvACK();

    I2C_SendByte(REG_data);       //内部寄存器数据，
   // I2C_RecvACK();

    I2C_Stop();                   //发送停止信号
   // delayus(1000);
}

//**************************************
//从I2C设备读取一个字节数据
//**************************************
uchar Single_ReadI2C(uchar REG_Address)
{
    uchar REG_data;
   /* SDAOUT;
    SCLOUT;
    SDA1;
    SCL1;*/
    I2C_Start();                   //起始信号

    I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
   // I2C_RecvACK();

    I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始
   // I2C_RecvACK();

    I2C_Start();                   //起始信号
    I2C_SendByte(SlaveAddress|0x01);  //发送设备地址+读信号
   // I2C_RecvACK();

    REG_data=I2C_RecvByte();       //读出寄存器数据
    I2C_SendACK(1);                //接收非应答信号

    I2C_Stop();                    //停止信号
    return REG_data;
}

//**************************************
//初始化MPU6050
//**************************************

void InitMPU6050()
{
    Single_WriteI2C(PWR_MGMT_1, 0x00); //解除休眠状态
    Single_WriteI2C(SMPLRT_DIV, 0x07);//陀螺仪采样率，典型值：0x07(125Hz)
    Single_WriteI2C(CONFIG, 0x06);//低通滤波频率，典型值：0x06(5Hz)
    Single_WriteI2C(GYRO_CONFIG, 0x18); //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
    Single_WriteI2C(ACCEL_CONFIG, 0x01);//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
}


//**************************************
//合成数据
//**************************************

unsigned int GetData(uchar REG_Address)
{
    char H,L;
    H=Single_ReadI2C(REG_Address);
    L=Single_ReadI2C(REG_Address+1);
    unsigned int data = (H<<8)|L;
    return data;                //合成数据
}
void main(void)
{
     WDTCTL = WDTPW+WDTHOLD;        // Stop watchdog timer
     InitMPU6050();
     _BIS_SR(GIE);                                  //开中断
     init();
     int j=0;
     while(1)
     {
     ax=(int)(GetData(ACCEL_XOUT_H)/16384);
     ay=(int)(GetData(ACCEL_YOUT_H)/16384);
     az=(int)(GetData(ACCEL_ZOUT_H)/16384);
     gx=(int)(GetData(GYRO_XOUT_H));//32);
     gy=(int)(GetData(GYRO_YOUT_H));//32);
     gz=(int)(GetData(GYRO_ZOUT_H));//32);

     lcd_printf1(dis,ax);

     lcd_printf1(dis+7,ay);

     dis[14]=0x30+j;
     if(j==9){
         j=0;
     }else
         j++;
    // lcd_printf(dis+10,az);

     lcd_printf1(dis1,az);

     lcd_printf1(dis1+7,gz);

     //lcd_printf(dis1+10,gz);
     write_cmd(0x01);//清屏

     write_cmd(0x80);
     int i=0;
     while(i<15){
         write_data(dis[i]);
         delayus(1);
         i++;
     }
     write_cmd(0xC0);
     i=0;
     while(i<13){
         write_data(dis1[i]);
         delayus(1);
         i++;
     }
   //  delay_ms(10);
     }
}
