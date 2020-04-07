#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= BIT0+BIT4;
	//P1SEL  |=BIT0+BIT4;
	P1OUT |= BIT0+BIT4;
	return 0;
}
