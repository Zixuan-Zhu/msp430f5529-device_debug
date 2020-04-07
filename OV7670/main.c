#include <msp430f5529.h>
#include "ov7670.h"
#include "SCCB.h"

unsigned int ov_sta = 0;
/**
 * main.c
 */

void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_5 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	initClock();
	while(ov7670_init());
	//unsigned char  data_fifo[2];
	unsigned char FIFO_1,FIFO_2,FIFO_data;
	unsigned char img[piexl_h*piexl_w][2];
	P2IE |= BIT0;
	P2IFG &=~BIT0;//清楚中断标志
	P2IES &=~BIT0;//上升沿中断
	/* OE  AL422 FIFO的输出使能引脚 ，OE为低电平时，允许数据输出 ，
	  高电平时，数据输出高阻态*/
	OE_L;
	if(ov_sta == 2)//读数据
	      {
	        P2IE &= ~BIT0;//关外部中断
	        //设置图像分辨率
	        OV7670_Window_Set(180,10,piexl_w,piexl_h);

	        RRST_L;//开始复位读指针
	        RCLK_L;
	        RCLK_H;
	        RCLK_L;
	        RRST_H;//读指针复位结束
	        RCLK_H;

	        int j=0;
	        while(j<piexl_h*piexl_w){
	            RCLK_L;
	            FIFO_1 = P4IN&0x0f;//读取高字节
	            FIFO_2 = P6IN&0x0f;
	            FIFO_data = (FIFO_2<<4)|FIFO_1;
	            img[j][0] =  FIFO_data;
	           // data_fifo[0] = FIFO_data;
	            RCLK_H;
	            RCLK_L;
	            FIFO_1 = P4IN&0xf0;//读取低字节
	            FIFO_2 = P6IN&0xf0;
	            FIFO_data = (FIFO_2<<4)|FIFO_1;
	            img[j][1] =  FIFO_data;
	           // data_fifo[1] = FIFO_data;
	            RCLK_H;
	            j++;
	          }
	        j=0;
	        ov_sta = 0;
	        P2IE |= BIT0;
	     }
	return 0;
}

//FIFO模块存储摄像头数据
#pragma vector = PORT2_VECTOR
__interrupt void PORT_2(void)
{
      WRST_L;//开始复位写指针
      WRST_H;//写指针复位结束
      if(ov_sta == 0)
      {
        WEN_H;
        ov_sta = 1;
      }
      else if(ov_sta == 1)
      {
        WEN_L;
        ov_sta = 2;
      }
      P2IFG &=~BIT0;     //清除标志位
}
