/**********************************************************************************************
     修改日志：偏移量有待调整，指示方向有待调整。
**********************************************************************************************/
#include <msp430f5529.h>
#include <math.h>
#include "lcd12864.h"
#include "INTRINSICS.H"
#include "lcd1602.h"
#define uchar unsigned char
#define uint  unsigned int
#define CPU_F ((double)16000000)
//#define delayus(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))  //宏定义延时函数
#define delayms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define pi 3.141592
#define Gry_offset -0.3         // 陀螺仪X偏移量
#define Gry_offsety -0.6        // 陀螺仪Y偏移量
#define Gry_offsetz 1.4         // 陀螺仪Z偏移量
#define Gyr_Gain 131            // 满量程250dps时灵敏度(dps/digital)
/************管脚定义******************/
#define SCL1 P2OUT |=BIT0
#define SCL0 P2OUT &=~BIT0
#define SCLOUT P2DIR |= BIT0
#define SDA1 P2OUT |=BIT2       //IIC数据引脚
#define SDA0 P2OUT &=~BIT2
#define SDAIN P2DIR &=~BIT2
#define SDAOUT P2DIR |=BIT2
#define SDADATA (P2IN & BIT2)
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
//float angleG=0;
//float Kx_0,Ky_0,Kz_0;
//float omegax,omegay,omegaz;
uint flag_kalman;
/*void delay1(uint z)
    {
    uint t,y;
    for(t=z;t>0;t--)
    for(y=110;y>0;y--);
}*/
void ini_time()
{
    P1SEL|=BIT2+BIT3+BIT4+BIT5;
    P1OUT|=BIT2+BIT3+BIT4+BIT5;
    TA0CCR0 = 512;                              // PWM 周期
    TA0CCTL1 = OUTMOD_7;                        // CCR1复位置位
    TA0CCR1 = 100;                              // 配置75%占空比
    TA0CCTL2 = OUTMOD_7;                        // CCR2 复位置位
    TA0CCR2 =100;                               // 配置25%占空比
    TA0CCTL3 = OUTMOD_7;                        // CCR3 复位置位
    TA0CCR3 =100;                               // 配置25%占空比
    TA0CCTL4 = OUTMOD_7;                        // CCR4 复位置位
    TA0CCR4 =100;                               // 配置25%占空比
    TA0CTL = TASSEL_2 + TACLR;                  // SCLK, 增模式

    TB0CTL|=TBCLR+TBSSEL_2 ;                    //不打开TAIIE、不用清中断变量
    TB0CCR0|=1000;
    TB0CCTL0|=CCIE;
    TB0CTL|=MC_1;

    TA0CTL |= MC_1;
}

void count_x(float a1,float a2,float g)         //卡尔曼滤波,计算角度   x、y、z分别和yz、xz、xy平面的夹角
{
    a1/=16384;
    a2/=16384;
    g/=131;
    float angleA=(atan(a1/a2))*180/pi;          //(atan(a1/(sqrt(a3*a3+a2*a2)))*180/pi);根据加速度分量得到的角度(degree)
    float omega =  Gyr_Gain * (g +  Gry_offset);    //角速度
    //angleG = angleG + omega * dt;                 // 对角速度积分得到的角度(degree)

    kalman1 += (omega - bias) * dt;                 // 先验估计
       P_00 += -(P_10 + P_01) * dt + Q_angle *dt;
       P_01 += -P_11 * dt;
       P_10 += -P_11 * dt;
       P_11 += +Q_omega * dt;                       // 先验估计误差协方差

       float K_0 = P_00 / (P_00 + R_angle);
       float K_1 = P_10 / (P_00 + R_angle);

       bias += K_1 * (angleA - kalman1);
       kalman1 += K_0 * (angleA - kalman1);         // 后验估计
       P_00 -= K_0 * P_00;
       P_01 -= K_0 * P_01;
       P_10 -= K_1 * P_00;
       P_11 -= K_1 * P_01;                          // 后验估计误差协方差

}
void count_y(float a1,float a2,float g)
{
        a1/=16384;
        a2/=16384;
        g/=131;
        float angleA=(atan(a1/a2))*180/pi;          //(atan(a1/(sqrt(a3*a3+a2*a2)))*180/pi);根据加速度分量得到的角度(degree)
        float omega =  Gyr_Gain * (g +  Gry_offsety);       //角速度

        kalman2 += (omega - biasy) * dt;                    // 先验估计
               Py_00 += -(Py_10 + Py_01) * dt + Q_angle *dt;
               Py_01 += -Py_11 * dt;
               Py_10 += -Py_11 * dt;
               Py_11 += +Q_omega * dt;                      // 先验估计误差协方差

               float K_0 = Py_00 / (Py_00 + R_angle);
               float K_1 = Py_10 / (Py_00 + R_angle);

               biasy += K_1 * (angleA - kalman2);
               kalman2 += K_0 * (angleA - kalman2);         // 后验估计
               Py_00 -= K_0 * Py_00;
               Py_01 -= K_0 * Py_01;
               Py_10 -= K_1 * Py_00;
               Py_11 -= K_1 * Py_01;                        // 后验估计误差协方差

}
void count_z(float a1,float a2,float g)
{
        a1/=16384;
        a2/=16384;
        g/=131;
        float angleA=(atan(a1/a2))*180/pi;          //(atan(a1/(sqrt(a3*a3+a2*a2)))*180/pi);根据加速度分量得到的角度(degree)
        float omega =  Gyr_Gain * (g +  Gry_offsetz);       //角速度

        kalman3 += (omega - biasz) * dt;                    // 先验估计
               Pz_00 += -(Pz_10 + Pz_01) * dt + Q_angle *dt;
               Pz_01 += -Pz_11 * dt;
               Pz_10 += -Pz_11 * dt;
               Pz_11 += +Q_omega * dt;                      // 先验估计误差协方差

               float K_0 = Pz_00 / (Pz_00 + R_angle);
               float K_1 = Pz_10 / (Pz_00 + R_angle);

               biasz += K_1 * (angleA - kalman3);
               kalman3 += K_0 * (angleA - kalman3);         // 后验估计
               Pz_00 -= K_0 * Pz_00;
               Pz_01 -= K_0 * Pz_01;
               Pz_10 -= K_1 * Pz_00;
               Pz_11 -= K_1 * Pz_01;                        // 后验估计误差协方差
}

void delayus(uint time)
{
    for(;time>0;time--)
    {
        _NOP(); _NOP(); _NOP();_NOP();
        _NOP(); _NOP(); _NOP();_NOP();
        _NOP(); _NOP(); _NOP();_NOP();
        _NOP(); _NOP();
    }
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
     P5SEL |= BIT2+BIT3;
     UCSCTL6 &= ~XT2OFF;            //打开XT2
    /*********************寄存器配置部分******************************/
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;                    //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_4 + 1;                   //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;       //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;      //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;               //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                                             //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ

     __bic_SR_register(SCG0);                //Enable the FLL control loop
    /**********************************************************************/
     //__delay_cycles(8192);
     do
     {
         UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);    //Clear XT2,XT1,DCO fault flags
         SFRIFG1 &= ~OFIFG;                             //Clear fault flags
     }while (SFRIFG1&OFIFG);

       //  P1DIR |= BIT0;                                 // LED1 输出
        // LCD_Init();
         InitMPU6050();
         ini_time();
         init();
         _BIS_SR(GIE);                                  //开中断
     int j=0;
     while(1)
     {
     //P1OUT ^=BIT0;
     ax=(int)(GetData(ACCEL_XOUT_H));//16384);
     ay=(int)(GetData(ACCEL_YOUT_H));//16384);
     az=(int)(GetData(ACCEL_ZOUT_H));//16384);
     gx=(int)(GetData(GYRO_XOUT_H)/32);
     gy=(int)(GetData(GYRO_YOUT_H)/32);
     gz=(int)(GetData(GYRO_ZOUT_H)/32);
     /*AX=ax/16384;
     AY=ay/16384;
     AZ=az/16384;
     GX=gx/131;
     GY=gy/131;
     GZ=gz/131;*/

     /*
     if(flag_kalman>=1)
     {
     count_x(ay,az,gx);                 //实际对应电子罗盘的x轴，按照该轴指示的旋转方向测试
     count_y(az,ax,gy);                 //实际对应电子罗盘的y轴？
     count_z(ax,ay,gz);                 //实际对应电子罗盘的z轴
     lcd_printf1(dis,kalman1);
     //DispStr(4,1,dis);
     delayus(50);
     lcd_printf1(dis,kalman2);
    // DispStr(4,4,dis);
     delayus(50);
     lcd_printf1(dis,kalman3);
    // DispStr(4,7,dis);
     delayus(50);
     lcd_printf1(dis,angleG);
     //DispInt(3,7,angleG);
     //delayus(50);
     }*/

     lcd_printf(dis,ax);
     //DispStr(1,1,dis);                  //显示X轴加速度
    // delayus(50);
     lcd_printf(dis+7,ay);
    // DispStr(1,4,dis);                  //显示Y轴加速度
     //delayus(50);
     //lcd_printf(dis+10,az);
    // DispStr(2,1,dis);                  //显示Z轴加速度
     //delayus(50);
     lcd_printf(dis1,az);
    // DispStr(2,4,dis);                  //显示X轴角速度
     //delayus(50);
     lcd_printf(dis1+7,gy);
    // DispStr(3,1,dis);                  //显示Y轴角速度
     //delayus(50);
     //lcd_printf(dis1+10,gz);
    // DispStr(3,4,dis);
     //delayms(30);
     write_cmd(0x01);
     write_cmd(0x80);
     dis[14]=0x30+j;
     if(j==9){
         j=0;
     }else
         j++;
     int i=0;
     while(i<15){
         write_data(dis[i]);
         delay_ms(1);
         i++;
     }
     write_cmd(0xC0);
     i=0;
     while(i<14){
         write_data(dis1[i]);
         delay_ms(1);
         i++;
     }
     delay_ms(1000);
     if(flag_kalman>1000)               //调整参数
       {
          bias=0.05;                    //define bias index
          biasy=0.05;
          biasz=0.05;
          P_00=0.001,P_01=0,P_10=0,P_11=0;
          Py_00=0.001,Py_01=0,Py_10=0,Py_11=0;
          Pz_00=0.001,Pz_01=0,Pz_10=0,Pz_11=0;
          flag_kalman=0;
       }
     }
}
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B (void)
{      dt=TB0R/1000;
       flag_kalman++;
       TB0R=0;
       P1OUT ^= BIT0;                   //LED1 闪烁
       TB0CCR0 = 1000;                  //CCR0 重新配置
       TB0CTL&=~TBIFG;
}
