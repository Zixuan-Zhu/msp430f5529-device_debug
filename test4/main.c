#include <msp430.h>


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR =BIT0|BIT6;
	P1OUT =BIT0;
	TA0CCR0 = 16384;                        // 定义中断计数周期1s,时钟频率为32.768kHZ,32768 / 32768 = 1s//计数9999次执行一次中断
	TA0CCTL0 = CCIE;                        // TA0CCR0捕获/比较中断寄存器中断使能
	TA0CTL = TASSEL_1 + MC_1 + TACLR ;//+ ID_3;       // TASSEL_1,ACLK时钟源  MC_1,增计数模式,所选时钟源8

	_BIS_SR(LPM3_bits + GIE);
	return 0;
}
#pragma vector = TIMER0_A0_VECTOR;
__interrupt void Timer_A(void)              // 定时器中断触发,P3输出口异或,电平翻转
{
    P1OUT ^= BIT0;
    P1OUT ^= BIT6;
}
