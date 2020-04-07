#include <msp430.h>
#include "LCD1602.h"

unsigned char s[]="LCD1602Trst";
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    LCD1602_Initi();
    LCD1602_WriteCMD(0x80);
    DispStr(s);
    return 0;
}
