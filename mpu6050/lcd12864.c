#include "lcd12864.h"

void write_cmd_data(uchar cmd_data,uchar cmd_or_data)	//cmd_or_data,0：写命令，1：写数据
{
	uint i,j;
	CS_H;
	SID_H;
	SCLK_L;
	for(i=0;i<5;i++)            //写5个1，对数据进行初始化
	{
	    SCLK_H;
	    _NOP();_NOP();_NOP();_NOP();
	    SCLK_L;
	    _NOP();_NOP();
	}

    SID_L;		                //写RW(0)，低位有效
    _NOP();
    SCLK_H;
    _NOP();_NOP();_NOP();_NOP();
    SCLK_L;

    if(cmd_or_data)SID_H;       //RS(0or1)，此时为RS=1,写数据1或者0
    _NOP();_NOP();_NOP();
    SCLK_H;
    _NOP();_NOP();_NOP();_NOP();
    SCLK_L;
    _NOP();
    SID_L;		                //写0
    _NOP();_NOP();
    SCLK_H;

    for(j=0;j<2;j++)
    {
        for( i=0;i<4;i++)       //写4位数
        {
            SCLK_L;
            if(cmd_data&BIT7)
                SID_H;
            else SID_L;
            _NOP();_NOP();_NOP();
            SCLK_H;
            cmd_data<<=1;       //通过移位的方法来进行数字的存储
            _NOP();
        }
        SCLK_L;
        SID_L;
        for(i=0;i<4;i++)        //写4个0
        {
            _NOP();
            SCLK_H;
            _NOP();_NOP();_NOP();_NOP();
            SCLK_L;
        }
    }
    CS_L;
}

void set_DDR_addr(uchar x,uchar y)          //设定DDRAM地址，x:行数，y：列数
{
    uchar address=0;
    switch(x)
    {
         case 1:address=0x80|(y-1);break;
         case 2:address=0x90|(y-1);break;
         case 3:address=0x88|(y-1);break;
         case 4:address=0x98|(y-1);break;
    }
    write_cmd_data(address,0);
}

void DispStr(uchar x,uchar y,char *ptr)     //显示字符串，x或y为0时不设定新地址
{
    if((x!=0)&&(y!=0))
        set_DDR_addr(x,y);
    while(*ptr!='\0')
    {
        write_cmd_data(*ptr,1);
        ptr++;
    }
}

void DispInt(uchar x,uchar y,uint number)       //可显示小于65536的自然数，返回值为自然数位数，x或y为0时不设定新地址
{												//左对齐显示
  	uint i;
	uint weight=10000;
	set_DDR_addr(x,y);
	for(i=5;(number/weight==0)&&(i>0);i--)
	{
		number%=weight;
		weight/=10;
	}
	if(i)
	{
		while(weight)
		{
			write_cmd_data(number/weight+'0',1);
			number%=weight;
			weight/=10;
		}
	}
	else write_cmd_data('0',1);
	//return i;
}

void disp_int_right(uchar x, uchar y, uint number)      //x,y 为起始地址，number小于65536
														//右对齐显示
{
	uchar i , j;
	uint weight=10000;
	set_DDR_addr(x,y);
	for(i=5;(number/weight==0)&&(i>0);i--)
	{
		number%=weight;
		weight/=10;
	}

	if(i)
	{
		for(j=0;j<6-i;j++)
		{
			write_cmd_data(' ',1);
		}
		while(weight)
		{
			write_cmd_data(number/weight+'0',1);
			number%=weight;
			weight/=10;
		}
	}
	else
	{
		for(j=0;j<5;j++)
		{
			write_cmd_data(' ',1);
		}
		write_cmd_data('0',1);
	}
}

void disp_int_left(uchar x, uchar y, uint number)           //x,y 为起始地址，，number小于65536
															//右对齐
{
	uchar i , j;
	uint weight=10000;
	set_DDR_addr(x,y);
	for(i=5;(number/weight==0)&&(i>0);i--)
	{
		number%=weight;
		weight/=10;
	}

	if(i)
	{
		while(weight)
		{
			write_cmd_data(number/weight+'0',1);
			number%=weight;
			weight/=10;
		}
		for( j=0;j<6-i;j++)
		{
			write_cmd_data(' ',1);
		}
	}
	else
	{
		write_cmd_data('0',1);
		for( j=0;j<5;j++)
		{
			write_cmd_data(' ',1);
		}
	}
}


void LCD_Delay_us(uint time)            //延时函数,按16MHz估算
{
  for(;time>0;time--)
  {
    _NOP(); _NOP(); _NOP();_NOP();
    _NOP(); _NOP(); _NOP();_NOP();
    _NOP(); _NOP(); _NOP();_NOP();
    _NOP(); _NOP();
  }
}

void LCD_Init(void)               //液晶初始化
{
    LCD_Pin_OUT;
    LCD_Delay_us(40000);
    write_cmd_data(0x30,0);         //功能设定，基本指令集
    LCD_Delay_us(100);
    write_cmd_data(0x30,0);         //功能设定，基本指令集
    LCD_Delay_us(37);
    write_cmd_data(0x0c,0);         //整体显示开
    LCD_Delay_us(100);
    write_cmd_data(0x01,0);         //清屏
    LCD_Delay_us(1600);
}

void LCD_Clear(void)               //液晶清屏
{
    write_cmd_data(0x01,0);
    LCD_Delay_us(1600);
}

void lcd_printf(char *s,int temp_data)
{
	//temp_data/=16384;
	if(temp_data<0)
	{
		temp_data=-temp_data;
		*s='-';
	}
	else *s='+';
    *++s =temp_data/10000+0x30;
    temp_data=temp_data%10000;     //取余运算
    *++s =temp_data/1000+0x30;
    temp_data=temp_data%1000;     //取余运算
	*++s =temp_data/100+0x30;
	temp_data=temp_data%100;     //取余运算
	*++s =temp_data/10+0x30;
	temp_data=temp_data%10;      //取余运算
	*++s =temp_data+0x30;
	*++s=' ';
}
void lcd_printf1(char *s,int temp_data)
{
	//temp_data/=64;
	if(temp_data<0)
	{
		temp_data=-temp_data;
		*s='-';
	}
	else *s='+';
	*++s =temp_data/100+0x30;
	temp_data=temp_data%100;     //取余运算
	*++s =temp_data/10+0x30;
	temp_data=temp_data%10;      //取余运算
	*++s =temp_data+0x30;
	*++s=0;
}
