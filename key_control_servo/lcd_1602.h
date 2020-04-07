/*
 * lcd1602.h
 *
 *  Created on: 2019Äê8ÔÂ2ÈÕ
 *      Author: ZZX
 */
#include <msp430f5529.h>
#include<string.h>
#include <intrinsics.h>

#ifndef LCD1602_H_
#define LCD1602_H_
#define CPU_F ((double)8000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define RS P1OUT //1.3
#define RW P1OUT //1.4
#define EN P1OUT //1.5
#define DataPort_L P4OUT
#define DataPort_H P2OUT
#define CmdPort_L P4OUT
#define CmdPort_H P2OUT

void initport();
void write_cmd(unsigned char cmd);
void write_data(unsigned char data);
void init_lcd1602();
#endif /* LCD1602_H_ */
