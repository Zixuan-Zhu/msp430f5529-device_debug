/*
 * SCCB.c
 *
 *  Created on: 2019年8月6日
 *      Author: ZZX
 */
#include "SCCB.h"
/*
-----------------------------------------------
   功能: 初始化SCCB端口，SCL-P8.1,输出，SDA-P8.2,输出
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void SCCB_init(void)
{
  //SDA-P8.2,上拉输入
  SCCB_SID_IN;
  P8REN |= BIT2;
  SCCB_SID_H();
  //SCL-P8.1,输出
  P8DIR |= BIT1;
  P8OUT |= BIT1;

  SCCB_SID_OUT;
}
/*
-----------------------------------------------
   功能: start命令,SCCB的起始信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //数据线高电平
    SCCB_SIC_H();     //在时钟线高的时候数据线由高至低
    delay_us(50);
    SCCB_SID_L();
    delay_us(50);
    SCCB_SIC_L();     //时钟恢复低电平，单操作函数必要
}
/*
-----------------------------------------------
   功能: stop命令,SCCB的停止信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID_L();
    delay_us(50);
    SCCB_SIC_H();
    delay_us(50);
    SCCB_SID_H();
    delay_us(50);
}
/*
-----------------------------------------------
   功能: noAck,用于连续读取中的最后一个结束周期
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void noAck(void)
{
    delay_us(50);
    SCCB_SID_H();
    SCCB_SIC_H();
    delay_us(50);
    SCCB_SIC_L();
    delay_us(50);
    SCCB_SID_L();
    delay_us(50);
}
/*
-----------------------------------------------
   功能: 写入一个字节的数据到SCCB
   参数: 写入数据
 返回值: 发送成功返回1，发送失败返回0
-----------------------------------------------
*/
unsigned int SCCBwriteByte(unsigned int m_data)
{
    unsigned char j,tem;

    for(j=0;j<8;j++) //循环8次发送数据
    {
        if(m_data&0x80)
        {
                    SCCB_SID_H();
        }
        else
        {
                    SCCB_SID_L();
        }
                m_data<<=1;
        delay_us(50);
        SCCB_SIC_H();
        delay_us(50);
        SCCB_SIC_L();
    }
    SCCB_SID_IN;/*设置SDA为输入*/
    delay_us(50);
    SCCB_SIC_H();
    delay_us(50);

    if(SCCB_SID_STATE){tem=0;}   //SDA=1发送失败，返回0}
    else {tem=1;}   //SDA=0发送成功，返回1
    SCCB_SIC_L();
        SCCB_SID_OUT;/*设置SDA为输出*/

    return tem;
}

/*
-----------------------------------------------
   功能: 一个字节数据读取并且返回
   参数: 无
 返回值: 读取到的数据
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
    unsigned char read,j;
    read = 0x00;

    SCCB_SID_IN;/*设置SDA为输入*/
    for(j=8;j>0;j--) //循环8次接收数据
    {
        delay_us(50);
        SCCB_SIC_H();
        read=read<<1;
        if(SCCB_SID_STATE)
        {
                    read++;
        }
                delay_us(50);
        SCCB_SIC_L();

    }
        SCCB_SID_OUT;/*设置SDA为输出*/
    return read;
}




