#include <msp430f5529.h>
#include "key.h"
#include "lcd_1602.h"
/**
 * main.c
 */
void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2 = 4MHz
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_4 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n) = 16MHz;DCOCLKDIV=(N+1)*(FLLREFCLK/n) = 1MHz;
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
     __bic_SR_register(SCG0);                   //Enable the FLL control loop

}
void servo_init(){
     P2DIR |= BIT0;                            //P2.0输出PWM波
     TA0CCTL1 = OUTMOD_7;                         //将输出模式选择为模式7
     TA0CTL = TASSEL_1 + MC_1 + TACLR ;         //选择ACLK时钟源，timerA计数模式选择up模式，计数到TA0CCR0，重置timerA计数器
     TA1CCTL1 = OUTMOD_7;
     TA1CTL = TASSEL_1 + MC_1 +TACLR ;
}
void set_PWM(unsigned int angle,char dir){
              //900微秒，设置的为1000微秒，时钟频率为32768Hz
    TA0CCR0 = 20000;          //验证进入状态1
    TA0CCR1 = 10000;
    TA1CCR0 = 100;              //输出PWM波周期
    if(dir == 'A')
        TA1CCR1 = 45+angle*0.38;              //占空比
    else if(dir == 'B')
        TA1CCR1 = 45-angle*0.38;
    P1DIR |=BIT2;
    P1SEL |=BIT2;
    P2DIR |=BIT0;
    P2SEL |=BIT0;
    //delay_ms(300);
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    uchar key = 0;
    Port_Init();//键盘初始化
    init_lcd1602();//lcd1602初始化
    servo_init();

    unsigned int angle1 = 0;
    char dir;

    while(1){
        write_cmd(0x01);
        write_cmd(0x80);
        angle1=0;
        while(1){
           key = Key_Scan(0);
           if(key == '#')
              break;
           else if(key>=0x30&&key<=0x39){
              angle1 = angle1*10+(key-0x30);
              write_data(key);
           }else if(key == 'A'||key == 'B'){
               dir = key;
               write_data(key);
           }
          delay_ms(1000);
       }
       set_PWM(angle1,dir);
    }

	return 0;
}
