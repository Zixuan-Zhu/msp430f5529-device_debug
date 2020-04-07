/*
 * ADC.c
 *
 *  Created on: 2019年8月4日
 *      Author: ZZX
 */
#include "ADC.h"

void IO_init()
{
    P5SEL |= BIT0;
    P6SEL |= BIT0;
    P6DIR |= BIT5;//模拟信号输入                                                                                   //配置GPIO引脚
    P5DIR &= ~BIT0;//参考高电压输入
}
void AD_Init()
{
    ADC12CTL0 |= ADC12MSC;                                                                              //自动循环采样转换
    ADC12CTL0 |= ADC12ON;                                                                                   //启动ADC12模块
    ADC12CTL1 |= ADC12CONSEQ1 ;                                                                     //选择单通道循环采样转换
    ADC12CTL1 |= ADC12SHP;                                                                              //采样保持模式
    ADC12MCTL0 |= ADC12INCH_0;                                                                      //选择通道5，连接拨码电位器
   // ADC12MCTL0 |= ADC12SREF_1;   //参考电压组合1，VREF+(P5.0)  ~   AVSS(GND)
    ADC12CTL0 |= ADC12ENC;
}



