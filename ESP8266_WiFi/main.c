/*****************************************************
程序功能：MSP430F149与ESP8266实现通信
------------------------------------------------------
通信格式：N.8.1, 115200
------------------------------------------------------
测试说明：打开串口调试精灵，正确设置通信格式，观察屏幕
******************************************************/
#include <msp430x14x.h>
#include "uart.h"
#include "clock.h"
#include "esp8266.h"
//
unsigned char  DelayFlag=0; //延时标志位


//main
void main(void)
{

    WDTCTL = WDTPW + WDTHOLD;  // 关狗
    Mcu_ClockInit();           //时钟出初始化，8MHz
    Mcu_UartInit();            //UART初始化（包括UART0和UART1）
    Mcu_TimerA0Start();        //TimerA0初始化-用于进行300ms延时
    ESP8266_AT_Init();

    while(1)
    {
         Mcu_Uart0SendString("AT+CIPSEND=0,20\r\n");      //向PC打印log
         Mcu_Uart1SendString("AT+CIPSEND=0,20\r\n");      //向ESP8266发送指令
         delay_ms(1000);   //1000ms延时

         Mcu_Uart0SendString("Hello World\r\n");              //向PC打印log
         Mcu_Uart1SendString("Hello World\r\n");
         delay_ms(1000);
    }
}

/******************定时300ms中断服务子程序**********/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void){

   DelayFlag=1; //do nothing

}
//
#pragma vector = USART0RX_VECTOR
__interrupt void USART0_RXISR(void)
{
    TXBUF0=RXBUF0;               //将接收到的数据发出去
    while((UTCTL0 & 0X01)==0);   //等待数据发送完毕
}
#pragma vector = USART1RX_VECTOR
__interrupt void USART1_RXISR(void)
{

    //TXBUF1=RXBUF1;

    //while((UTCTL1 & 0X01)==0);   //等待数据发送完毕
}












