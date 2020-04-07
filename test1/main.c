#include <MSP430g2553.h>

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR |= BIT6+BIT0;         //P1。6,P1.0为输出
  P1OUT &=~ BIT3;
  P1OUT |=BIT6+BIT0;          //P1.6,P1.0为高电位每个循环都将输出置高

unsigned char Key=P1IN&BIT3;//存起上一次的键值
  while(1)
  {


    if(P1IN & BIT3)//上次与本次键值不同，且本次键值为按下的时候（即按键按下的时候）

    {
      P1OUT &=~( BIT6+BIT0);//改变输入/输出电平
    }

     Key=P1IN&BIT3;//将本次按键键值存起来
  }
}
