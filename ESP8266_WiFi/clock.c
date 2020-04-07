#include "clock.h"


/***************************************************************************
函数名称：Mcu_ClockInit
功    能： 时钟的初始化，8MHz
参    数： 无
返回值 ：无
****************************************************************************/
void Mcu_ClockInit(void)
{
    unsigned int temp;

    _DINT();
    BCSCTL1 &= ~XT2OFF;                       // XT2on
    do
    {
       IFG1 &= ~OFIFG;                        // Clear OSCFault flag
       for (temp = 0xFF; temp > 0; temp--);   // Time for flag to set
    }
    while ((IFG1 & OFIFG));                   // OSCFault flag still set?
    BCSCTL2 |= SELM1 + SELS;                  // XT2，MCLK,SCLK=8MHz

}
