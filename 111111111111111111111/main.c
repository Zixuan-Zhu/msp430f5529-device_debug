#include <msp430.h>
unsigned int i=0;

/**
 * main.c
 */
int main(void)
{
    P2DIR |= BIT0+BIT5;                       // P2.0 and P2.1 output
      P2SEL |= BIT0+BIT5;                       // P2.0 and P2.1 options select
      TA1CCR0 = 1000-1;                          // PWM Period
      TA2CCR0 = 1000-1;
    //  TA1CCTL1 = OUTMOD_7;                      // CCR1 reset/set
   //   TA1CCR1 = period;                            // CCR1 PWM duty cycle
      TA2CCTL2 = OUTMOD_7;                      // CCR2 reset/set
      TA2CCR2 = 500;                            // CCR2 PWM duty cycle
      TA1CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, up mode, clear TAR
      TA2CTL = TASSEL_1 + MC_1 + TACLR;
      __bis_SR_register(LPM3_bits);             // Enter LPM3
      __no_operation();                         // For debugger
}
