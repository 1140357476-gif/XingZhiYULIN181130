/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__
#include "key_scan.h"
#include "func_timer_def.h"
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
/*
	(2^16-X)*(12/((522/5)*10^6))=5*10^-3	-> X=0x5614 (5ms time tick)
*/
#define K_Timer0IntervalCompLo  0x14
#define K_Timer0IntervalCompHi  0x56

#if UI_TEST
#define K_TIMER_PERIOD          2             //  10ms/timer0 interrupt
#else 
#define K_TIMER_PERIOD          600  
#endif
#define K_Timer1IntervalLo	0x00 //0x60            //  Timer1 interrput interval (HEX)
#define K_Timer1IntervalHi	0x06 // 0xf0            // 2ms counter

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void TIMER0_Routine(void);
void TIMER0_Handler(void);
void TIMER0_Start(void);
void TIMER0_Stop(void);
void TIMER0_PowerOffCfg(UINT8 enable, UINT32 delay);
UINT8 TIMER0_PowerOffProcChk(void);

void DELAY_10us(void);
void DELAY_1ms(void);
void DELAY_1s(void);

void TIMER1_Config(UINT32 Period, pFuncTmr1 pFunc, UINT8* pFuncPara);
void TIMER1_Routine(void);
void TIMER1_Handler(void);
void TIMER1_Start(void);
void TIMER1_Stop(void);

#endif /*__TIMER_H__*/
