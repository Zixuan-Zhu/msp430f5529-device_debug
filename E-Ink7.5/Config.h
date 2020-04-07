/*
 * Config.h
 *
 *  Created on: 2020Äê1ÔÂ13ÈÕ
 *      Author: ZZX
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <msp430.h>
void initClock(void);
void IO_init(void);
void ini_UART(void);
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);
void DELAY_M(unsigned int delaytime);


#endif /* CONFIG_H_ */
