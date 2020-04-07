#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |=BIT0+BIT6;
    P1DIR &=~BIT3;//设置为输入
    P1OUT |=BIT0+BIT6;

    P1REN |=BIT3; //启用1.3电阻
    P1OUT |=BIT3;//设置上拉电阻
    P1IES &=~BIT3;//中断边缘选择，0为下降沿，1为上升沿
    P1IFG &=~BIT3;//清空1.3中断标志
    P1IN  |=BIT3;//中断使能
    P1IE  |=BIT3;

    _EINT();//开启全局中断

    //__bis_SR_register(LPM4_bits+GIE);//开启低功耗模式和全局中断
    //__no_operation();
    /*
    while(1){
        if(P1IN & BIT3){
            P1OUT &=~(BIT0+BIT6);
        }
        else
            P1OUT |=BIT0+BIT6;
    }*/
    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    P1OUT ^=BIT0+BIT6;
    P1IFG &=~BIT3;
}
