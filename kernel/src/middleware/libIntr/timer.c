/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
 
//=============================================================================
//Header file
//=============================================================================
#define TMR_TASK_INTERNAL
#include "general.h"
#include "timer.h"

#include "os_api.h"
//#include "os_msg.h"
#include "tmr_svc.h"
#include "key_scan.h"
#include "func_timer_def.h"
//=============================================================================
//Symbol
//=============================================================================
extern void adctest(void);
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

static UINT8 xdata Tmr0Stamp;

static UINT32 xdata Tmr1ReloadVal;
static pFuncTmr1 xdata Tmr1PFunc;
static UINT8* xdata Tmr1PFuncPara;
static UINT32 xdata InitVal;
static UINT32 xdata TimeTick;
extern UINT8 TestFlag;

extern TMR_TASK_DCL pFuncTmr0 code timerFuncCallList0[];
extern TMR_TASK_DCL pFuncTmr1 code timerFuncCallList1[];
//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//                               TIMER0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//TIMER0
//-----------------------------------------------------------------------------
void TIMER0_Routine(void)
{
	TIMER0_Handler();     //Timer tick: 5ms
}

//-----------------------------------------------------------------------------
//TIMER0_Handler
//-----------------------------------------------------------------------------
void TIMER0_Handler(void) USING_2
{
	UINT8 i;
	pFuncTmr0 code* ppFunc;

	if (!(--Tmr0Stamp))
	{
		//5ms * 2 
		Tmr0Stamp = K_TIMER_PERIOD;
        
		osTimeTick();

		/* customized timer task */
		i = TMR0_TASK_MAX;
		ppFunc = &timerFuncCallList0[0];
		do {
			if (*ppFunc) {
				(*ppFunc)();
			} else {
				break;
			}
			ppFunc++;
		} while (--i);

        tmrSvc_APO_Intr();
        tmrSvc_PowerOffProc_Intr();
		
#if (UI_TEST)		

#else
		adctest();
#endif		

	}
}

//-----------------------------------------------------------------------------
//TIMER0_Start
//-----------------------------------------------------------------------------
void TIMER0_Start(void) USING_0
{
	Tmr0Stamp = K_TIMER_PERIOD;

	//start timer
	ET0 = 1;                //IE:    TIMER0 interrupt enable
	TR0 = 1;                //TCON:  TIMER0 enable
}

//-----------------------------------------------------------------------------
//TIMER0_Stop
//-----------------------------------------------------------------------------
void TIMER0_Stop(void) USING_0
{
     //stop timer
     TR0 = 0;                //TCON:  TIMER0 disable
     ET0 = 0;                //IE:    TIMER0 interrupt disable
}

//-----------------------------------------------------------------------------
//                               TIMER1
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//TIMER1
//-----------------------------------------------------------------------------
void TIMER1_Routine(void)
{
    //EX0 = 0;
    TIMER1_Handler();     //TIMER1 handler
    //EX0 = 1;
}

//-----------------------------------------------------------------------------
//TIMER1_Handler
//-----------------------------------------------------------------------------
void TIMER1_Handler(void) USING_3
{
	pFuncTmr1* ptr;
	
	TR1 = 0 ;
	
	// add by phil for correcting the call tree for function pointer S	
	if (TR1) {
		ptr = timerFuncCallList1;
		//ptr = intrFuncArray;
	}
	// add by phil for correcting the call tree for function pointer E	
	
	TL1 = READ8(InitVal, 3);
	TH1 = READ8(InitVal, 2);
	if (TimeTick) {
		TimeTick--;
	} else {
		TimeTick = Tmr1ReloadVal;
		if (Tmr1PFunc != NULL) {
			Tmr1PFunc(Tmr1PFuncPara);
		}
	}
		
	TR1 = 1 ;
}


//-----------------------------------------------------------------------------
//TIMER1_Start
//-----------------------------------------------------------------------------
void TIMER1_Start(void) USING_3
{
    //start timer
    ET1 = 1;                //IE:    TIMER1 interrupt enable
    TR1 = 1;                //TCON:  TIMER1 enable
}

//-----------------------------------------------------------------------------
//TIMER1_Stop
//-----------------------------------------------------------------------------
void TIMER1_Stop(void) USING_3
{
    ET1 = 0;                //IE:    TIMER1 interrupt disable
    TR1 = 0;                //TCON:  TIMER1 disable
}

void TIMER1_Config(UINT32 Period, pFuncTmr1 pFunc, UINT8* pFuncPara)
{
	UINT32 CpuClk;
	
	TMOD &= 0x0f;
	TMOD |= 0x10;
	TF1=0;
			
	if ((!Period) || (pFunc == (void (*)())NULL)) {
		return;
	}

	Tmr1ReloadVal = Period;
	Tmr1PFunc = pFunc;
	Tmr1PFuncPara = pFuncPara;

	CpuClk=522000000UL/(XBYTE[0x2024]+1);

	InitVal = 65536UL -  CpuClk / 12000UL;
	TimeTick = Tmr1ReloadVal;
	TL1 = READ8(InitVal, 3);
	TH1 = READ8(InitVal, 2);
	
}

#if 0
//---------------------------------------------------------------------------------------------------------------------------------------
//DELAY_10us
//---------------------------------------------------------------------------------------------------------------------------------------
void DELAY_10us(void) USING_0
{
#pragma asm	
     MOV     R6,#010        
DELAY_10us_1:                        	;10 * 6 * (4 / 24M) s = 10 us
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     DJNZ    R6,DELAY_10us_1		     ;3 Instr. Cycles
#pragma endasm
}	
//---------------------------------------------------------------------------------------------------------------------------------------
//DELAY_1ms
//---------------------------------------------------------------------------------------------------------------------------------------
void DELAY_1ms(void) USING_0
{
#pragma asm	
	MOV	R5,#010				
DELAY_1ms_1:	
     MOV     R6,#100        
DELAY_1ms_2:                        	;100 * 6 * (4 / 24M) s = 100 us
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     DJNZ    R6,DELAY_1ms_2		     ;3 Instr. Cycles
     DJNZ    R5,DELAY_1ms_1		     ;3 Instr. Cycles
#pragma endasm
}	
//#endif

//---------------------------------------------------------------------------------------------------------------------------------------
//DELAY_1s
//---------------------------------------------------------------------------------------------------------------------------------------
void DELAY_1s(void) USING_0
{
#pragma asm	
	MOV	R4,#100				
DELAY_1s_1:	
	MOV	R5,#100				
DELAY_1s_2:	
     MOV     R6,#100        
DELAY_1s_3:                        	;100 * 6 * (4 / 24M) s = 100 us
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     NOP                             	;1 Instr. Cycles
     DJNZ    R6,DELAY_1s_3 	      	;3 Instr. Cycles
     DJNZ    R5,DELAY_1s_2 	      	;3 Instr. Cycles
     DJNZ    R4,DELAY_1s_1 	      	;3 Instr. Cycles
#pragma endasm
}	
#endif

