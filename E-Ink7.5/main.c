#include <msp430.h> 
#include "EPD.h"
#include "Picture.h"
volatile unsigned char buffer[6000];
//volatile unsigned char buffer_R[48000];
unsigned char recv_data,recv_byte[2];
unsigned int number = 0,index = 0,flag = 0,num=0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    P8DIR |= BIT1;
	//时钟初始化
	initClock();
	//IO初始化
	IO_init();
	ini_UART();
	//EPD_Clean
	EPD_init(); //EPD init
	PIC_display_Clean();//EPD_Clean
	EPD_refresh();//EPD_refresh
//	EPD_sleep();//EPD_sleep
//	DELAY_S(8);
//
//	EPD_init(); //EPD init
//	PIC_display (nandao,45000,nandao,45000);//EPD_picture1
//	EPD_refresh();//EPD_refresh
//	EPD_sleep();//EPD_sleep
//	DELAY_S(8);
//	EPD_W21_WriteCMD(0x13);
//	buffer_write (nandao,6000);
//	buffer_write (nandao+6000,6000);
//	buffer_write (nandao+12000,6000);
//	buffer_write (nandao+18000,6000);
//	buffer_write (nandao+24000,6000);
//	buffer_write (nandao+30000,6000);
//	buffer_write (nandao+36000,6000);
//	EPD_refresh();
	__bis_SR_register(GIE);
}
//void clear_buff(void)
//{
//    int i;
//    for(i=0;i<48000;i++)
//    {
//        buffer[i]=0;
//    }
//
//}
unsigned int H_to_D(unsigned char c){
    switch(c){
      case 'A':case 'a': return 10;break;
      case 'B':case 'b': return 11;break;
      case 'C':case 'c': return 12;break;
      case 'D':case 'd': return 13;break;
      case 'E':case 'e': return 14;break;
      case 'F':case 'f': return 15;break;
      default: return c-0x30;
    }
}

#pragma vector=USCI_A1_VECTOR                                       //USCI中断服务函数
__interrupt void USCI_A1_ISR(void)
{
switch(__even_in_range(UCA1IV,4))
{
case 0:break;                                                   //无中断
case 2:                                                         //接收中断处理
    {
        if(UCRXIFG)                                             //等待完成接收
        {   UCA1TXBUF = 0x30+ (num & 0xff);
            if(flag){
                recv_byte[index] = UCA1RXBUF;     //数据读出
                index ++;
                if(index == 2){
                    index = 0;
                    buffer[number] = H_to_D(recv_byte[0])*16+H_to_D(recv_byte[1]);
                    //buffer[number] = recv_data;
                    //EPD_W21_WriteDATA(recv_data);
                    number++;
                }
                if(number == 6000){
                    //PIC_display (buffer,6000,buffer,6000);
                    number = 0;
                    num ++;
                    buffer_write (buffer,6000);
                    //EPD_refresh();
                   // flag = 0;
                    if(num == 9){
                        EPD_refresh();
                        flag = 0;
                    }
                }
            }
            else if(UCA1RXBUF == '~'){//黑白
                flag = 1;
                number = 0;
                num = 0;
                P8OUT ^= BIT1;
                EPD_W21_WriteCMD(0x13);
                //EPD_W21_WriteCMD(0x10);
            }//else if(UCA1RXBUF == '-'){//红色
               // flag = 1;
               // number = 0;
                //EPD_W21_WriteCMD(0x13);
           // }
        }
    }
    break;
case 4:break;                                                   //发送中断不处理
default:break;                                                  //其他情况无操作

}
}
