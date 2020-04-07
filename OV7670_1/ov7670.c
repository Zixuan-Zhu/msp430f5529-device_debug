/*
 * ov7670.c
 *
 *  Created on: 2019年8月6日
 *      Author: ZZX
 */
#include "ov7670.h"
#include<msp430f5529.h>
#include "SCCB.h"
#include "include.h"
unsigned char ov7670_init(void)
{

  unsigned int i=0;
  unsigned char temp;
  //VSYNC-P2.0
  //上拉输入，外部中断
  P2DIR &= ~BIT0;
  P2REN |= BIT0;
  P2OUT |= BIT0;//上拉输入

  //FIFO数据输入引脚
  //D0-D3--P4.0-P4.3,D4-D7--P6.0--P6.3
  //上拉输入
  P4DIR &= 0xf0;
  P4REN |= 0x0f;
  P4OUT |= 0x0f;
  P6DIR &= 0xf0;
  P6REN |= 0x0f;
  P6OUT |= 0x0f;

  //OE-P1.2,RCLK-P1.3,WEN-P1.4,WRST-P1.5,RRST-P1.6
  //输出
  P1DIR |= 0xec;
  P1OUT |= 0xec;
  SCCB_init();

     //读写寄存器函数出现错误
    if(wr_Sensor_Reg(0x12,0x80)!= 0 ) //Reset SCCB
    {
          return 1;//错误返回
    }

        delay_ms(50);

    if(rd_Sensor_Reg(0x0b, &temp) != 0)//读ID
    {
        return 2 ;//错误返回
    }

     if(temp==0x73)//OV7670
     {
       for(i=0;i<OV7670_REG_NUM;i++)
       {
        if(wr_Sensor_Reg(OV7670_reg[i][0],OV7670_reg[i][1]) != 0)
        {
                    return 3;//错误返回
        }
       }

    }
        return 0; //ok
}

//功能：写OV7660寄存器
//返回：0-成功   其他失败
unsigned char wr_Sensor_Reg(unsigned char regID, unsigned char regDat)
{
     startSCCB();//发送SCCB 总线开始传输命令
    if(0==SCCBwriteByte(0x42))//写地址
    {
        stopSCCB();//发送SCCB 总线停止传输命令
        return 1;//错误返回
    }
    delay_us(20);
    if(0==SCCBwriteByte(regID))//积存器ID
    {
        stopSCCB();//发送SCCB 总线停止传输命令
        return 2;//错误返回
    }
    delay_us(20);
    if(0==SCCBwriteByte(regDat))//写数据到积存器
    {
        stopSCCB();//发送SCCB 总线停止传输命令
        return 3;//错误返回
    }
    stopSCCB();//发送SCCB 总线停止传输命令

    return 0;//成功返回
}

//功能：读OV7660寄存器
//返回：0-成功   其他失败
unsigned char rd_Sensor_Reg(unsigned char regID,unsigned char *regDat)
{
    //通过写操作设置寄存器地址
    startSCCB();
    if(SCCBwriteByte(0x42)==0)//写地址
    {

        return 1;//错误返回
    }
    delay_us(100);
    if(SCCBwriteByte(regID)==0)//积存器ID
    {

        return 2;//错误返回
    }
        delay_us(100);
    stopSCCB();//发送SCCB 总线停止传输命令

    delay_us(100);

    //设置寄存器地址后，才是读
    startSCCB();
    if(SCCBwriteByte(0x43)==0)//读地址
    {

        return 3;//错误返回
    }
    delay_us(100);
    *regDat=SCCBreadByte();//返回读到的值
    noAck();//发送NACK命令
    stopSCCB();//发送SCCB 总线停止传输命令
    return 0;//成功返回
}

void OV7670_Window_Set(unsigned int sx,unsigned int sy,unsigned int width,unsigned int height)
{
    unsigned int endx;
    unsigned int endy;
    unsigned char temp;
    endx=(sx+width*2)%784;  //   sx:HSTART endx:HSTOP
    endy=sy+height*2;       //   sy:VSTRT endy:VSTOP

        rd_Sensor_Reg(0x32,&temp);
    temp&=0Xc0;
    temp|=((endx&0X07)<<3)|(sx&0X07);
    wr_Sensor_Reg(0X032,temp);
    wr_Sensor_Reg(0X17,sx>>3);
    wr_Sensor_Reg(0X18,endx>>3);

        rd_Sensor_Reg(0x03,&temp);
    temp&=0Xf0;
    temp|=((endy&0X03)<<2)|(sy&0X03);
    wr_Sensor_Reg(0X03,temp);
    wr_Sensor_Reg(0X19,sy>>2);
    wr_Sensor_Reg(0X1A,endy>>2);
}

