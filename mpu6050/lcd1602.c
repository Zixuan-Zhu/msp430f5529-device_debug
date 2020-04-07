#include "lcd1602.h"

void initport(){
    P1SEL&=0X00;                  //设置P3口为I/O模式
    P3SEL&=0X00;                  //设置P6口为I/O模式
    P1DIR|=0X38;                  //设置P3口为输出模式
    P3DIR|=0XFF;                  //设置P6口为输出模式
    P1OUT&=0X00;
    P3OUT&=0X00;
}
void write_cmd(unsigned char cmd){//写命令函数
    EN&=~BIT5;
    delay_ms(5);
    RS&=~BIT3;
    delay_ms(5);
    RW&=~BIT4;
    CmdPort = cmd;
    delay_ms(5);
    EN|=BIT5;
    delay_ms(5);
    EN&=~BIT5;

   // RW |= BIT4;

}
void write_data(unsigned char data){//写数据函数

    RS|=BIT3;
    delay_ms(5);
    EN&=~BIT5;
    RW&=~BIT4;
    delay_ms(5);
    DataPort = data;
    delay_ms(5);
    EN|=BIT5;
    delay_ms(5);
    EN&=~BIT5;
}

void init(){
    RW&=~BIT4;
    initport();
    write_cmd(0x38);//8位数据接口，2行显示，5*7的点阵字符
    write_cmd(0x0c);//显示开，光标关，闪烁关
    write_cmd(0x06);//数据读写操作后，AC自动加1  数据读写操作后，画面保持不变
    write_cmd(0x01);//清屏
}



