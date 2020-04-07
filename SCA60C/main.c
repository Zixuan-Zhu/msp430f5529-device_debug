#include <msp430f5529.h>
#include "ADC.h"
#include "lcd1602.h"
/**
 * main.c
 */
unsigned char buf[16];
unsigned char buf1[16];
void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_5 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ


}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    initClock();
    AD_Init();
    IO_init();
    init_lcd1602();
    buf[0]='a';
    buf[1]='n';
    buf[2]='g';
    buf[3]='l';
    buf[4]='e';
    buf[5]=':';
    buf1[0]='D';
    buf1[1]='I';
    buf1[2]='N';
    buf1[3]=':';
    unsigned int value=0;
    unsigned int angle=0;
    int j=0;
    while(1)
        {
            ADC12CTL0 |= ADC12SC;
            value = ADC12MEM0;                                                          //把结果赋给变量
            angle = (value-340)/18;//3269/180
           // delay_ms(1);
           // float Vin = value*5/4095;
           // unsigned int angle = (Vin-0.5)*45;//(Vin-0.5)*180/4
            lcd_printf(buf+6,angle);
            lcd_printf1(buf1+4,value);
            write_cmd(0x01);//清屏
            write_cmd(0x80);
            int i=0;
            while(i<10){
              write_data(buf[i]);
              delay_ms(1);
              i++;
            }
            i=0;
            write_cmd(0xc0);
            while(i<9){
              write_data(buf1[i]);
              delay_ms(1);
              i++;
            }
            delay_ms(1000);
        }
	return 0;
}
