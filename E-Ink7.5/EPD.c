/*
 * EPD.c
 *
 *  Created on: 2020Äê1ÔÂ13ÈÕ
 *      Author: ZZX
 */
#include "EPD.h"

void EPD_init(void)
{
      HRES_byte1=0x03;          //800
      HRES_byte2=0x20;
      VRES_byte1=0x01;          //480
      VRES_byte2=0xE0;

        EPD_W21_Init();

        EPD_W21_WriteCMD(0x01);         //POWER SETTING
        EPD_W21_WriteDATA (0x07);
        EPD_W21_WriteDATA (0x07);    //VGH=20V,VGL=-20V
        EPD_W21_WriteDATA (0x3f);       //VDH=15V
        EPD_W21_WriteDATA (0x3f);       //VDL=-15V

        EPD_W21_WriteCMD(0x04); //POWER ON
        driver_delay_xms(100);
        lcd_chkstatus();

        EPD_W21_WriteCMD(0X00);         //PANNEL SETTING
        EPD_W21_WriteDATA(0x0F);   //KW-3f   KWR-2F BWROTP 0f   BWOTP 1f

        EPD_W21_WriteCMD(0x61);         //tres
        EPD_W21_WriteDATA (HRES_byte1);     //source 800
        EPD_W21_WriteDATA (HRES_byte2);
        EPD_W21_WriteDATA (VRES_byte1);     //gate 480
        EPD_W21_WriteDATA (VRES_byte2);

        EPD_W21_WriteCMD(0X15);     // Dual SPI Mode (DUSPI)
        EPD_W21_WriteDATA(0x00);

        EPD_W21_WriteCMD(0X50);         //VCOM AND DATA INTERVAL SETTING
        EPD_W21_WriteDATA(0x11);
        EPD_W21_WriteDATA(0x07);

        EPD_W21_WriteCMD(0X60);         //TCON SETTING
        EPD_W21_WriteDATA(0x22);

}
void EPD_refresh(void)
{
        EPD_W21_WriteCMD(0x12);         //DISPLAY REFRESH
        driver_delay_xms(100);          //!!!The delay here is necessary, 200uS at least!!!
        lcd_chkstatus();
}
void EPD_sleep(void)
{
        EPD_W21_WriteCMD(0X02);     //power off
        lcd_chkstatus();
        EPD_W21_WriteCMD(0X07);     //deep sleep
        EPD_W21_WriteDATA(0xA5);
}



/**********************************display picture**********************************/

void buffer_write (unsigned char* buffer,int length)
{
  u32 i;
  for(i=0;i<length;i++)
  {
      EPD_W21_WriteDATA(*buffer);
      buffer++;
  }
}
void PIC_display (const unsigned char* picData_BW,int length_BW,const unsigned char* picData_R,int length_R)
{
  u32 i;
  EPD_W21_WriteCMD(0x10);
  for(i=0;i<length_BW;i++)
  {
      EPD_W21_WriteDATA(~(*picData_BW));
      picData_BW++;
  }
  EPD_W21_WriteCMD(0x13);
  for(i=0;i<length_R;i++)
  {
     EPD_W21_WriteDATA(*picData_R);
     picData_R++;
  }

}
void PIC_display_Clean(void)
{
  u32 i;
    EPD_W21_WriteCMD(0x10);
  for(i=0;i<48000;i++)
    {
        EPD_W21_WriteDATA(0xFF);
    }
    EPD_W21_WriteCMD(0x13);
  for(i=0;i<48000;i++)
    {
        EPD_W21_WriteDATA(0x00);
    }

}

void lcd_chkstatus(void)
{
    unsigned char busy;
    do
    {
        EPD_W21_WriteCMD(0x71);
        busy = isEPD_W21_BUSY;
        busy =!(busy & 0x01);
    }
    while(busy);
    driver_delay_xms(200);
}



