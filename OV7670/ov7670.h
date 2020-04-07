/*
 * ov7670.h
 *
 *  Created on: 2019Äê8ÔÂ6ÈÕ
 *      Author: ZZX
 */
#ifndef OV7670_H_
#define OV7670_H_

#define OE_L    P1OUT &= ~BIT2
#define OE_H    P1OUT |= BIT2
#define RCLK_L  P1OUT &= ~BIT3
#define RCLK_H  P1OUT |= BIT3
#define WEN_L   P1OUT &= ~BIT4
#define WEN_H   P1OUT |= BIT4
#define WRST_L  P1OUT &= ~BIT5
#define WRST_H  P1OUT |= BIT5
#define RRST_L  P1OUT &= ~BIT6
#define RRST_H  P1OUT |= BIT6

//ÏñËØ´æ´¢
#define piexl_w 320
#define piexl_h 240

#define OV7670_REG_NUM  116

unsigned char ov7670_init(void);
unsigned char rd_Sensor_Reg(unsigned char regID,unsigned char *regDat);
unsigned char rd_Sensor_Reg(unsigned char regID,unsigned char *regDat);
void OV7670_Window_Set(unsigned int sx,unsigned int sy,unsigned int width,unsigned int height);

#endif /* OV7670_H_ */
