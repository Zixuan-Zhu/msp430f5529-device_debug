/*
 * Display_EPD_SPI.h
 *
 *  Created on: 2020年1月13日
 *      Author: ZZX
 */

#ifndef DISPLAY_EPD_SPI_H_
#define DISPLAY_EPD_SPI_H_
#include <msp430.h>
#include "Config.h"

#define EPD_W21_SPI_SPEED 0x02

//SDI 1.2 数据线
#define EPD_W21_MOSI_0  P1OUT &= ~BIT2
#define EPD_W21_MOSI_1  P1OUT |= BIT2
//SCLK 1.3 时钟线
#define EPD_W21_CLK_0   P1OUT &= ~ BIT3
#define EPD_W21_CLK_1   P1OUT |= BIT3
//CS 1.4 片选线
#define EPD_W21_CS_0    P1OUT &= ~ BIT4
#define EPD_W21_CS_1    P1OUT |= BIT4
//DC 1.5 数据/命令线
#define EPD_W21_DC_0    P1OUT &= ~ BIT5
#define EPD_W21_DC_1    P1OUT |= BIT5
//RST 1.6 复位线
#define EPD_W21_RST_0   P1OUT &= ~ BIT6
#define EPD_W21_RST_1   P1OUT |= BIT6
//BUSY 2.0
#define isEPD_W21_BUSY  P2IN & BIT0

void EPD_W21_Init(void);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);


#endif /* DISPLAY_EPD_SPI_H_ */
