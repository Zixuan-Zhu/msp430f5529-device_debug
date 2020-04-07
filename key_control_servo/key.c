/*
 * key.c
 *
 *  Created on: 2019年8月7日
 *      Author: ZZX
 */
#include "key.h"
void Port_Init(void)
{
   KEYDIR = 0X0F;  //高四位输入，低四位输出
   KEYREN |= 0XF0; //高四位开启拉电阻功能
   KEYPORT = 0XF0;  //低四位输出1，高四位为下拉电阻
}

uchar Key_Scan(uchar xx)
{
    static uchar statevalue = 0;
    uchar key_checkin,key_check;
    uchar key = 0;
    KEYPORT = 0XF0;
    if(xx==0)statevalue = 0;
    key_checkin = KEYIN;  //读取电平
        if(key_checkin != 0xf0 && statevalue == 0)
        {
         delay_ms(20);
         key_checkin = KEYIN;  //读取电平
         if(key_checkin != 0xf0)
         {
             statevalue = 1;
          KEYPORT = 0XFE;  //第一行
          key_check = KEYIN;
          switch(key_check)
          {
              case 0xee: key = '1';break;
              case 0xde: key = '2';break;
              case 0xbe: key = '3';break;
              case 0x7e: key = 'A';break;
          }
          KEYPORT = 0XFD;//第二行
           key_check = KEYIN;
           switch(key_check)
           {
               case 0xed: key = '4';break;
               case 0xdd: key = '5';break;
               case 0xbd: key = '6';break;
               case 0x7d: key = 'B';break;
           }
           KEYPORT = 0XFB;//第三行
            key_check = KEYIN;
            switch(key_check)
            {
                case 0xeb: key = '7';break;
                case 0xdb: key = '8';break;
                case 0xbb: key = '9';break;
                case 0x7b: key = 'C';break;
            }
            KEYPORT = 0XF7;//第四行
             key_check = KEYIN;
             switch(key_check)
             {
                 case 0xe7: key = '*';break;
                 case 0xd7: key = '0';break;
                 case 0xb7: key = '#';break;
                 case 0x77: key = 'D';break;
             }
         }
        }
        else if(key_checkin == 0xf0)
        {
            statevalue =0;
        }
   return key;
}



