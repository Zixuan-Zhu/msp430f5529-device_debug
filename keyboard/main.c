#include <msp430.h>
#define CPU_CLOCK 1000000               //默认主频1Mhz
#define delay_ms(x) __delay_cycles((x)*CPU_CLOCK/1000);
#define delay_us(x) __delay_cycles((x)*CPU_CLOCK/1000000);
#define uchar unsigned char
#define KEYPORT P3OUT   //按键输入控制
#define KEYSEL  P3SEL   //控制口功能选择io口功能
#define KEYDIR  P3DIR  // 控制口方向寄存器
#define KEYIN   P3IN   //读取io电平
#define KEYREN  P3REN  //使能上下啦电阻
uchar key1 = 0;
void Port_Init(void)
{
   KEYDIR = 0X0F;  //高四位输入，低四位输出
   KEYREN |= 0XF0; //高四位开启拉电阻功能
   KEYPORT = 0XF0;  //低四位输出1，高四位为下拉电阻
}
/*
 * xx == 0时为连续触发
 * xx 为其他时单次触发
 *
 *
 * */
uchar Key_Scan(uchar xx)
{
    static uchar statevalue = 0;
    uchar key_checkin,key_check;
    uchar key = 0;
    KEYPORT = 0XF0;
    if(xx==0)statevalue = 0;
    key_checkin = KEYIN;  //读取电平
        if(key_checkin != 0xf0 && statevalue == 0)
        {
         delay_ms(20);
         key_checkin = KEYIN;  //读取电平
         if(key_checkin != 0xf0)
         {
             statevalue = 1;
          KEYPORT = 0XFE;  //第一行
          key_check = KEYIN;
          switch(key_check)
          {
              case 0xee: key = '1';break;
              case 0xde: key = '2';break;
              case 0xbe: key = '3';break;
              case 0x7e: key = 'A';break;
          }
          KEYPORT = 0XFD;//第二行
           key_check = KEYIN;
           switch(key_check)
           {
               case 0xed: key = '4';break;
               case 0xdd: key = '5';break;
               case 0xbd: key = '6';break;
               case 0x7d: key = 'B';break;
           }
           KEYPORT = 0XFB;//第三行
            key_check = KEYIN;
            switch(key_check)
            {
                case 0xeb: key = '7';break;
                case 0xdb: key = '8';break;
                case 0xbb: key = '9';break;
                case 0x7b: key = 'C';break;
            }
            KEYPORT = 0XF7;//第四行
             key_check = KEYIN;
             switch(key_check)
             {
                 case 0xe7: key = '*';break;
                 case 0xd7: key = '0';break;
                 case 0xb7: key = '#';break;
                 case 0x77: key = 'D';break;
             }
         }
        }
        else if(key_checkin == 0xf0)
        {
            statevalue =0;
        }
   return key;
}
void main(void)
{
    uchar key1 = 0;
    uchar k10 = 0,k14 = 0;
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    Port_Init();
    P1DIR |= BIT0;
    P4DIR |= BIT7;
    while(1)
    {
        key1 = Key_Scan(0);
        switch(key1)
        {
        case '1': P1OUT |= BIT0;break;
        case '2': P1OUT &= ~BIT0;k14 = 1;break;
        case '3': P4OUT |= BIT7;break;
        case '4': P4OUT &= ~BIT7;k14 = 0;break;
        case 'A': k10 =1;break;
        case 'B': k14 = 1;break;
        }
        if(k10 == 1)
        {
            P1OUT ^= BIT0;
        }
        if(k14 == 1)
        {
            P4OUT ^= BIT7;
        }
        delay_ms(50);
    }

}
