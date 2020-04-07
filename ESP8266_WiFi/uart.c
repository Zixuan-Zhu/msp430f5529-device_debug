#include  <msp430x14x.h>
#include  "esp8266.h"

/*******************************************************************************
函数名称：Mcu_UartInit
功    能：初始化Uart端口,包括UART0和UART1
参    数：无
返回值  ：无
********************************************************************************/
void Mcu_UartInit(void)
{
    //UART0初始化
    P3SEL |= 0x30;        //0011 0000,选择P3.4和P3.5作为UART0通信端口
    ME1 |= UTXE0 + URXE0; //使能UART0的发送和接收
    U0CTL |= CHAR;        //选择八位字符
    U0TCTL |= SSEL1;      //SMCLK
    U0BR0  = 0x45;
    U0BR1  = 0x00;        //波特率115200
    U0MCTL = 0x00;
    U0CTL &= ~SWRST;      //初始化UART状态机
    IE1 |= URXIE0;        //使能UART的接收中断

    //UART1初始化
    P3SEL |= 0xC0;        //1100 0000,选择P3.6和P3.7作为UART1通信端口
    ME2 |= UTXE1 + URXE1; //使能UART1的发送和接收
    U1CTL |= CHAR;        //选择八位字符
    U1TCTL |= SSEL1;      //SMCLK
    U1BR0  = 0x45;
    U1BR1  = 0x00;        //波特率115200
    U1MCTL = 0x00;
    U1CTL &= ~SWRST;      //初始化UART状态机
    IE2 |= URXIE1;        //使能UART的接收中断

}

/***************************************************************************
函数名称：Mcu_Uart0SendString
功    能： 向PC机发送字符串
参    数： 无
返回值 ：无
****************************************************************************/
void Mcu_Uart0SendString( char *ptr)
{
   while(*ptr != '\0')
   {
      while (!(IFG1 & UTXIFG0));                // TX缓存空闲？
      TXBUF0 = *ptr++;                       // 发送数据
   }
   while (!(IFG1 & UTXIFG0));
}

/***************************************************************************
函数名称：Mcu_Uart1SendString
功    能：向ESP8266模块发送字符串
参    数：无
返回值  ：无
****************************************************************************/
void Mcu_Uart1SendString(char *ptr)
{
   while(*ptr != '\0')
   {
      while (!(IFG2 & UTXIFG1));                // TX缓存空闲？
      TXBUF1 = *ptr++;                       // 发送数据
   }
   while (!(IFG2 & UTXIFG1));
}

/***************************************************************************
函数名称：Delay300msInit
功    能：精确延时300ms
参    数：无
返回值：无
****************************************************************************/
void Mcu_TimerA0Start(void)
{
    TACCTL0 = CCIE;
    //TACCR0 = 9829; //延时300ms
    TACCR0 = 32768;  //延时1s
    TACTL = TASSEL_1 +TACLR+MC_1;

    P2DIR |=BIT3;    //P2.3为输出，测试1s延时用的LED
    P2DIR |=BIT2;    //P2.2为输出，测试1s延时用的LED
    //
    __enable_interrupt();      //打开总中断
}
















