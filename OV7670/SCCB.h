/*
 * SCCB.h
 *
 *  Created on: 2019Äê8ÔÂ6ÈÕ
 *      Author: ZZX
 */
#include<msp430f5529.h>
#ifndef SCCB_H_
#define SCCB_H_
#define CPU_F ((double)8000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
//SCL-P8.1,SDA-P8.2
#define SCCB_SIC_H()     P8OUT|=BIT1
#define SCCB_SIC_L()     P8OUT&=~BIT1

#define SCCB_SID_H()     P8OUT|=BIT2
#define SCCB_SID_L()     P8OUT&=~BIT2

#define SCCB_SID_IN      P8DIR &= ~BIT2
#define SCCB_SID_OUT     P8DIR |= BIT2
#define SCCB_SID_STATE   P8IN&BIT2


void SCCB_init(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned int SCCBwriteByte(unsigned int m_data);
unsigned char SCCBreadByte(void);


#endif /* SCCB_H_ */
