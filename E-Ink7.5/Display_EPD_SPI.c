/*
 * Display_EPD_SPI.c
 *
 *  Created on: 2020年1月13日
 *      Author: ZZX
 */
#include "Display_EPD_SPI.h"

void EPD_W21_Init(void)
{
    EPD_W21_RST_0;      // Module reset
    driver_delay_xms(1000);
    EPD_W21_RST_1;
    driver_delay_xms(1000);

}

void SPI_Delay(unsigned char xrate)
{
    unsigned char i;
    while(xrate)
    {
        for(i=0;i<EPD_W21_SPI_SPEED;i++);
        xrate--;
    }
}
//写值
void SPI_Write(unsigned char value)
{
    unsigned char i;

    SPI_Delay(1);
    for(i=0; i<8; i++)
    {
        EPD_W21_CLK_0;
        SPI_Delay(1);
        if(value & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;
        value = (value << 1);
        SPI_Delay(1);
        driver_delay_us(1);
        EPD_W21_CLK_1;
        SPI_Delay(1);
    }
}
//写指令
void EPD_W21_WriteCMD(unsigned char command)
{
    SPI_Delay(1);
    EPD_W21_CS_0;
    EPD_W21_DC_0;       // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}
//写数据
void EPD_W21_WriteDATA(unsigned char command)
{
    SPI_Delay(1);
    EPD_W21_CS_0;
    EPD_W21_DC_1;       // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}

