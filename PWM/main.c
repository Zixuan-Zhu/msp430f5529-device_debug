#include <msp430.h>
#include <msp430f5529.h>
unsigned int i = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    P8DIR |= BIT1+BIT2;
    P8OUT |= BIT1;
    P8OUT &= ~BIT2;
    TA0CTL |= MC_2 + TASSEL_1 + TACLR;      //增计数模式，时钟为SMCLK,比较模式，开始时清零计数器
    TA0CCR0 = 0xffff;
    TA0CCTL1 = CCIE;                        //比较器中断使能
    TA0CCR1  = 32768;                         //比较值设为50000，相当于50ms的时间间隔
    __enable_interrupt();
    /*
	  P6DIR |= BIT0+BIT1;
	  P6OUT |= BIT1;
	  P6OUT &=~BIT0;
	  P2DIR |= BIT0+BIT5;                       // P2.0 and P2.1 output
	  P2SEL |= BIT0+BIT5;                       // P2.0 and P2.1 options select
	  TA1CCR0 = 1000;                          // PWM Period
	  TA2CCR0 = 1000;
	  TA1CCTL1 = OUTMOD_7;                      // CCR1 reset/set
	  TA1CCR1 = 1000;                            // CCR1 PWM duty cycle
	  TA2CCTL2 = OUTMOD_7;                      // CCR2 reset/set
	  TA2CCR2 = 700;                            // CCR2 PWM duty cycle
	  TA1CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, up mode, clear TAR
	  TA2CTL = TASSEL_1 + MC_1 + TACLR;

	  __bis_SR_register(LPM3_bits);             // Enter LPM3
	  __no_operation();*/

	while(1);
	return 0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A (void)
{
    switch(__even_in_range(TA0IV,14))

       {

          case 2:
              if(i == 5)
              {
                  i=0;
                  P8OUT ^= BIT1+BIT2;
              }
              i++;
              break; //TACCR1 CCIFG置位，表明计数值和设定的13107相等了，也就是说计了0.4S了

          case 4:P1OUT=BIT2;break; //TACCR2 CCIFG置位，表明计了0.8S了

          case 6:P1OUT=BIT3;break; //TACCR3 CCIFG置位，表明计了1.2S了

          case 8:P1OUT=BIT4;break; //TACCR4 CCIFG置位，表明计了1.6S了

          case 14:P1OUT=BIT5;break;   //TAIFG置位，表明计了2S了

          default:break;

       }
    //i++;
  //  if(i == 20){
      //  P8OUT ^= BIT1;
      //  i=0;
   // }

}

