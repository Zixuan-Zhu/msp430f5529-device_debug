#include <msp430.h>                        //函数库
int sum=0;                                  //全局变量
int main(void)
{
  volatile unsigned int i;                  //局部变量
                //设置timerA相关参数

  WDTCTL = WDTPW+WDTHOLD;                   // 关闭看门狗
  P1DIR |= BIT2;                            // 将P1.2设置为输出端口，验证状态
  P2DIR |= BIT0;                            //P2.0输出PWM波
  TA0CCTL1 = OUTMOD_7;                         //将输出模式选择为模式7
  TA0CTL = TASSEL_1 + MC_1 + TACLR ;         //选择ACLK时钟源，timerA计数模式选择up模式，计数到TA0CCR0，重置timerA计数器
  TA1CCTL1 = OUTMOD_7;
  TA1CTL = TASSEL_1 + MC_1 +TACLR ;

    P1REN |= BIT1;                            // 使能端口1.7
    P1OUT |= BIT1;                            // 选择下拉模式
    P1IES |= BIT1;                            // 1.7端口中断触发沿选择下降沿，高到低触发
    P1IFG &= ~BIT1;                           // 1.7端口中断寄存器清零
    P1IE |= BIT1;                       // 1.7端口中断使能

    __bis_SR_register(GIE);                     //将GIE置1，打开全局中断使能

  while(1)                                       // 采用switch，每一个case值对应一个状态
  {
    switch (sum)
    {
    case 0:
        P1OUT &= ~BIT2;                              //初始状态为低功耗状态
        TA0CCR1 = 0;
        __bis_SR_register(LPM3_bits);            //将LPM3置1，使系统进入低功耗
        break;                                   //跳出switch函数
    case 1:                                      //选系统的默认时钟，频率为1MHZ。
        P1OUT &= ~BIT2;                 //状态1要求：转到—60°，900微秒，设置的为1000微秒，时钟频率为32768Hz
        TA0CCR0 = 20000;          //验证进入状态1
        TA0CCR1 = 10000;
        TA1CCR0 = 100;              //输出PWM波周期
        TA1CCR1 = 34;              //占空比
        P1DIR |=BIT2;
        P1SEL |=BIT2;
        P2DIR |=BIT0;
        P2SEL |=BIT0;
        break;
    case 2:
        P1OUT &= ~BIT2;                 //状态2要求：转到0°。1520微秒
        TA0CCR0 = 20000;
        TA0CCR1 = 5000;
        TA1CCR0 = 100;
        TA1CCR1 = 57;
        P1DIR |=BIT2;
        P1SEL |=BIT2;
        P2DIR |=BIT0;
        P2SEL |=BIT0;
        break;
    case 3:                     //时钟为辅助时钟ACLK，频率为32768HZ
    P1OUT &= ~BIT2;                 //状态3要求：转到60°   2100微秒
        TA0CCR0 = 20000;
        TA0CCR1 = 800;
        TA1CCR0 = 100;
        TA1CCR1 = 85;
        P1DIR |=BIT2;
        P1SEL |=BIT2;  //选择1.2端口的复用功能，使之能够实现亮和灭
        P2DIR |=BIT0;
        P2SEL |=BIT0;
        break;

    }
    if(sum>3)                     //当sum的值超过3时，要让sum减3，重新进入循环
          {
            sum=sum-3;
          }

  }
}

//采用S1作为按键实现
#pragma vector=PORT1_VECTOR                //端口p1的中断函数
__interrupt void Port_1(void)
{
    unsigned int push_key =0;
    push_key == P1IFG & (P1IN);
    __delay_cycles(20000);                  //精确时间延迟，乘以MCLK时钟周期,防止抖动
    if( push_key == (P1IN & BIT1))
    {
        __low_power_mode_off_on_exit();        //SR寄存器置0，退出低功耗
        sum=sum+1;                               // 按下p1.7，sum加1，周期变化一次
    }
      P1IFG &= ~BIT1;                          // p1.7中断重置
}
