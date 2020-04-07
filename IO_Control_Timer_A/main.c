#include <msp430.h>
//用IO中断控制时钟中断
int flag=0;
/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= BIT0+BIT6;
    P1DIR &=~BIT3;

    P1OUT |= BIT0;
    P1OUT &= ~BIT6;

    P1REN |= BIT3;
    P1IES &=~BIT3;
    P1IFG &=0x00;
    P1IE  |=BIT3; //如果将此赋值为1,表示开启IO中断，当按下开关时，会去执行中断函数，如果没有IO中断函数，程序会卡住

    TA0CTL |=MC_1+TASSEL_1+TACLR;
    TA0CCTL0 =CCIE;
    TA0CCR0 =9999;

//msp430的指令中，DINT和EINT分别指关和开所有中断，也就是包涵P1IE、P2IE、WDTIE、TAIE、TBIE、ADC12IE、串口中断允许的所有中断允许位为“0”和为“1”
    _EINT();//全局中断会将所有中断使能开启，包括IO中断，计数器中断，但是如果具体的某个中断使能没有设置，该中断不会起作用
    while(1){
        if(flag == 1){
            TA0CCTL0 &=~CCIE;//将计数器中断关闭
        }
        else{
            TA0CCTL0 =CCIE;//计数器中断开启
        }
    }

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    unsigned int i;
    for(i=0;i<12000;i++);//消抖，十分重要
    flag=(flag+1)%2;
    P1IFG &=~BIT3;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void){
    P1OUT ^=BIT0+BIT6;
}
