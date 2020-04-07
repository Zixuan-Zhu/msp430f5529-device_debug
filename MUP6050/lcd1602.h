/*
 * lcd1602.h
 *
 *  Created on: 2019Äê8ÔÂ2ÈÕ
 *      Author: ZZX
 */
#include <msp430g2553.h>
#include<string.h>
#include <intrinsics.h>

#ifndef LCD1602_H_
#define LCD1602_H_
#define CPU_F ((double)8000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define RS P2OUT //2.3
#define RW P2OUT //2.4
#define EN P2OUT //2.5
#define DataPort P1OUT
#define CmdPort P1OUT

void initport();
void write_cmd(unsigned char cmd);
void write_data(unsigned char data);
void init();
void lcd_printf(char *s,int temp_data);
#endif /* LCD1602_H_ */
