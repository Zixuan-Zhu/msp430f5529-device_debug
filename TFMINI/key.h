/*
 * key.h
 *
 *  Created on: 2019年8月7日
 *      Author: ZZX
 */
#include <msp430f5529.h>

#ifndef KEY_H_
#define KEY_H_

#define CPU_CLOCK 1000000               //默认主频1Mhz
#define delay_ms(x) __delay_cycles((x)*CPU_CLOCK/1000);
#define delay_us(x) __delay_cycles((x)*CPU_CLOCK/1000000);
#define uchar unsigned char
#define KEYPORT P3OUT   //按键输入控制
#define KEYSEL  P3SEL   //控制口功能选择io口功能
#define KEYDIR  P3DIR  // 控制口方向寄存器
#define KEYIN   P3IN   //读取io电平
#define KEYREN  P3REN  //使能上下啦电阻

void Port_Init(void);
uchar Key_Scan(uchar xx);

#endif /* KEY_H_ */
