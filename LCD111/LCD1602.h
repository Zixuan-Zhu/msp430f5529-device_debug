/*
 * LCD1602.h
 *
 *  Created on: 2018Äê6ÔÂ3ÈÕ
 *      Author: Sunnl
 */

#ifndef LCD1602_H_
#define LCD1602_H_
#include <msp430.h>
void Disp1Char(unsigned char data);
void DispStr(unsigned char *ptr);
void LCD1602_WriteCMD(unsigned char cmd);
void LCD1602_WriteDATA(unsigned char Data);
void LCD1602_Initi();
#endif
/* LCD1602_H_ */
