#ifndef LCD12864_H_
#define LCD12864_H_
#include <msp430f5529.h>

#define CS_H P3OUT|=BIT3        //片选信号，高有效
#define CS_L P3OUT&=~BIT3

#define SID_H P3OUT|=BIT2       //串行数据线
#define SID_L P3OUT&=~BIT2

#define SCLK_H P3OUT|=BIT1      //串行时钟输入
#define SCLK_L P3OUT&=~BIT1

#define LCD_Pin_OUT P3DIR|=(BIT1+BIT2+BIT3);

#define uchar unsigned char
#define uint unsigned int

extern void write_cmd_data(uchar cmd_data,uchar cmd_or_data);	    //cmd_or_data,0：写命令，1：写数据
extern void set_DDR_addr(uchar x,uchar y);                          //设定DDRAM地址，x:行数，y：列数
extern void DispStr(uchar x,uchar y,char*ptr);                      //显示字符串，x或y为0时不设定新地址
extern void DispInt(uchar x,uchar y,uint number);                   //可显示小于65536的自然数，返回值为自然数位数，x或y为0时不设定新地址
extern void LCD_Delay_us(uint time);                                //延时函数,按16MHz估算
extern void LCD_Init(void);                                         //液晶初始化
extern void LCD_Clear(void);                                        //液晶清屏
extern void disp_int_right(uchar x, uchar y, uint number);          //x,y 为起始地址，，number小于65536															//右对齐
extern void disp_int_left(uchar x, uchar y, uint number);           //x,y 为起始地址，，number小于65536
extern void lcd_printf(char *s,int temp_data);
extern void lcd_printf1(char *s,int temp_data);
extern void LCD_Delay_us(uint time);
#endif /* LCD12864_H_ */
