/*
 * Config.c
 *
 *  Created on: 2020年1月13日
 *      Author: ZZX
 */
#include "Config.h"
void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2 = 4MHz
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_4 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n) = 16MHz;DCOCLKDIV=(N+1)*(FLLREFCLK/n) = 1MHz;
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
     __bic_SR_register(SCG0);                   //Enable the FLL control loop

}
void IO_init(void)
{
    P1DIR |= BIT2+BIT3+BIT4+BIT5+BIT6;
    P2DIR &= ~BIT0;
}
void ini_UART(){
          P4SEL |= BIT5+BIT4;                       // P4.4,5使用外设功能 = UCA1 TXD/RXD
          UCA1CTL1 |= UCSWRST;                      // 复位USCI
          UCA1CTL1 |= UCSSEL__SMCLK;                     // 设置SMCLK时钟，用于发生特定波特率
          UCA1BR0 = 4;                            // 设置波特率， 8MHz 波特率= 115200
          UCA1BR1 = 0;
          UCA1MCTL |= 0x51;//UCBRS_1 + UCBRF_0;
          UCA1CTL1 &= ~UCSWRST;                     // 结束复位
          UCA1IE |= UCRXIE;
}
void driver_delay_us(unsigned int xus)  //1us
{
    for(;xus>1;xus--);
}

void driver_delay_xms(unsigned long xms) //1ms
{
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
    {
        for(i=0; i<256; i++);
    }
}
void DELAY_S(unsigned int delaytime)     //  1s
{
    int i,j,k;
    for(i=0;i<delaytime;i++)
  {
        for(j=0;j<4000;j++)
        {
            for(k=0;k<222;k++);

        }
    }
}
void DELAY_M(unsigned int delaytime)     //  1M
{
    int i;
    for(i=0;i<delaytime;i++)
        DELAY_S(60);
}


