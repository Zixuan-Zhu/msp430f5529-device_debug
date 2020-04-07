#include <msp430.h> 
#include<string.h>

#define RS P3OUT //3.0
#define RW P3OUT //3.1
#define EN P3OUT //3.2
//#define data_port P6OUT P7.0
unsigned char buf[] = "WELCOME MSP430";
void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2 = 4MHz
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_4 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n) = 16MHz;DCOCLKDIV=(N+1)*(FLLREFCLK/n) = 1MHz;
     //最终MCLK:1MHZ,SMCLK:8MHZ,ACLK:32KHZ

     __bic_SR_register(SCG0);                   //Enable the FLL control loop

}
void initport(){
    P3SEL&=0Xf8;                  //设置P3口为I/O模式
    P6SEL&=0X00;                  //设置P6口为I/O模式
    P7SEL&=0xfe;
    P3DIR|=0X07;                  //设置P3口为输出模式
    P6DIR|=0XFF;                  //设置P6口为输出模式
    P7DIR|=0X01;
    P3OUT&=0X07;
    P6OUT&=0X00;
    P7OUT&=0x01;
}
void delay_1ms(int z)       //延时1ms
{
    int i,j;
    for(i=0;i<z;i++)
        for(j=0;j<110;j++);
}
void write_com(unsigned char com){//写命令函数
    RS&=~BIT0;
    RW&=~BIT1;
    delay_1ms(5);
    EN&=~BIT2;
    P6OUT = com;
    P7OUT = com>>7;
    EN|=BIT2;
    delay_ms(5);
    EN&=~BIT2;
    RW |= BIT1;

}
void write_data(unsigned char data){//写数据函数
    RS|=BIT0;
    RW&=~BIT1;
    delay_1ms(5);
    EN&=~BIT2;
    P6OUT = data;
    P7OUT = data>>7;
    EN|=BIT2;
    delay_ms(5);
    EN&=~BIT2;
    RW |= BIT1;
}

void init(){
    RW&=~BIT1;
    initClock();
    initport();
    write_com(0x38);//8位数据接口，2行显示，5*7的点阵字符
    write_com(0x0c);//显示开，光标关，闪烁关
    write_com(0x06);//数据读写操作后，AC自动加1  数据读写操作后，画面保持不变
    write_com(0x01);//清屏
}
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    init();
    write_com(0x80);
    int len = strlen(buf);
    int i;
    for(i=0;i<len;i++){
        write_com(buf[i]);
        delay_1ms(1);
    }
    return 0;
}
