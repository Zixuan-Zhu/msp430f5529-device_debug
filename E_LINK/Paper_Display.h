#include <msp430.h>

#define nRST_H  P1OUT|=BIT4  //外部复位引脚，低电平复位
#define nRST_L  P1OUT&=~BIT4
#define nCS_H   P1OUT|=BIT7   //SPI片选引脚，低电平有效
#define nCS_L   P1OUT&=~BIT7
#define SDA_H   P2OUT|=BIT2   //
#define SDA_L   P2OUT&=~BIT2
#define SCLK_H   P2OUT|=BIT0   //SPI通信SCK引脚，通信时钟
#define SCLK_L   P2OUT&=~BIT0
#define nDC_H   P1OUT|=BIT5    //数据/命令控制引脚，高电平数据，低电平命令
#define nDC_L   P1OUT&=~BIT5
#define nBUSY   P1IN & BIT0 //高电平表示忙

extern volatile unsigned char DisBuffer[250*16];

void SPI4W_WRITECOM(const unsigned char INIT_COM);
void SPI4W_WRITEDATA(const unsigned char INIT_DATA);
//void SPI4W_WRITE(unsigned char INIT_COM,unsigned char INIT_DATA);
void MYRESET(void);
void WRITE_LUT(void);
void INIT_SSD1673(void);
//void DIS_IMG(const unsigned char num);
void FIRST_PICTURE(void);
//void IMAGE_WRITE(const unsigned char patt_num);
void DELAY_100nS(const unsigned int delaytime);
void DELAY_mS(const unsigned int delaytime);
void DELAY_S(const unsigned int delaytime);
void DELAY_M(const unsigned int delaytime);
//void RING(void);
void READBUSY(void);
//void closebump(void);
//void DisplayString(const char* str, unsigned int sx, unsigned int sy);
void enterdeepsleep(void);
void Init_buff(void);
//void SET_EYEONE(void);
void PaperIO_Int(void);
/*void display(const unsigned char *str, //字符串
             const unsigned int xsize,  //x方向位置
             const unsigned int ysize,  //y方向位置
             const unsigned int font,   //字体 0,1,2
             const unsigned int size,   //字号 0,1
             const unsigned int reverse,//反显 0 正常显示， 1 反显
             const unsigned int fresh   //立即刷新
             );
*/
