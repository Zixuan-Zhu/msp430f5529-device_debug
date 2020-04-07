/*
 * EPD.h
 *
 *  Created on: 2020Äê1ÔÂ13ÈÕ
 *      Author: ZZX
 */

#ifndef EPD_H_
#define EPD_H_
#include <msp430.h>
#include "Display_EPD_SPI.h"
#define u32 unsigned int

unsigned int size;
unsigned char HRES_byte1,HRES_byte2,VRES_byte1,VRES_byte2;

void EPD_init(void);
void buffer_write (unsigned char* buffer,int length);
void PIC_display (const unsigned char* picData_BW,int length_BW,const unsigned char* picData_R,int length_R);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);



#endif /* EPD_H_ */
