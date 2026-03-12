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

#ifndef __GENERAL_H__
#define __GENERAL_H__


#define SIMULATE	0

#define DRAM_ASSERT 0

/* tt = 0x12345678
	READ8(tt,0) = 0x12
	...
	READ8(tt,3) = 0x78
*/
#define	READ8(a,b)  (*((unsigned char *)&(a)+b))
#define	READ16(a,b) (*(unsigned short *)((unsigned char *)&(a)+b))
#define	READ32(a,b) (*(unsigned long  *)((unsigned char *)&(a)+b))

#define	WRITE8(a,b,v)  *((unsigned char *)&(a)+b) = v
#define	WRITE16(a,b,v) *(unsigned short *)((unsigned char *)&(a)+b) = v
#define	WRITE32(a,b,v) *(unsigned long  *)((unsigned char *)&(a)+b) = v

#define	READP8(p,b)  (*((unsigned char *)(p)+b))
#define	READP16(p,b) (*(unsigned short *)((unsigned char *)(p)+b))
#define	READP32(p,b) (*(unsigned long  *)((unsigned char *)(p)+b))

#define	WRITEP8(p,b,v)  *((unsigned char *)p+b) = v
#define	WRITEP16(p,b,v) *(unsigned short *)((unsigned char *)(p)+b) = v
#define	WRITEP32(p,b,v) *(unsigned long  *)((unsigned char *)(p)+b) = v


#undef SNAP_IQ_FLOW
#undef CALIBRATION
#define LOAD_CDSP_PARAMETER_BIN
#define LOAD_AWBCTT_BIN
#define AAA_ON
#define _ENABLERLE_ 0

#ifdef CALIBRATION
#undef LOAD_CDSP_PARAMETER_BIN
#undef AAA_ON
#endif

// In the future, always load cdsp bin (#define LOAD_CDSP_PARAMETER_BIN).
// For calibration, cdsp bin is blank file and turn off 3A (#undef AAA_ON)
// For normal mode, cdsp bin is parameter file and turn on 3A (#define AAA_ON)


#undef PV_RAW_DATA
#undef PREVIEW_RAW_SAVE_MODE // add for preview image CDSP simulation

#undef MULTI_FOLDER_AS_ONE
//#define MULTI_FOLDER_AS_ONE

//-----------------------------------------------------------------------------
// Data type
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Macro
//-----------------------------------------------------------------------------
#define M_BCDToHEX(x)           (((x) >> 4) & 0x0f) * 10 + ((x) & 0x0f)
#define M_HEXToBCD(x)           ((((x) / 10) << 4) | ((x) % 10))
#define M_HiByteOfWord(x)       (unsigned char) ((x) >> 8)
#define M_LoByteOfWord(x)       (unsigned char) (x)
#define M_ByteSwapOfWord(x)     (((x) >> 8) | ((x) << 8))
#define M_HiByteOfDword(x)      (unsigned char) ((x) >> 24)
#define M_MidHiByteOfDword(x)   (unsigned char) ((x) >> 16)
#define M_MidLoByteOfDword(x)   (unsigned char) ((x) >> 8)
#define M_LoByteOfDword(x)      (unsigned char) (x)
//richie@0117 modify
#define M_ByteSwapOfDword(x)    (((unsigned long)(x) << 24) | (((unsigned long)(x) & 0x0000ff00) << 8) | (((unsigned long)(x) & 0x00ff0000) >> 8) | ((unsigned long)(x) >> 24))
#define M_UCHARToULONG(x,y,u,v) (unsigned long) (((unsigned long)x)<<24)|(((unsigned long)y)<<16)|(((unsigned long)u)<<8)|((unsigned long)(v))
#define M_UCHARToUSHORT(x,y)	(unsigned short)  ((((unsigned short)x)<<8)|((unsigned short)y))

//-----------------------------------------------------------------------------
//Header file
//-----------------------------------------------------------------------------

#define USING_0			//using 0
#define USING_1			//using 1
#define USING_2			//using 2
#define USING_3			//using 3
#define INTERRUPT_0		//interrupt 0
#define INTERRUPT_1		//interrupt 1
#define INTERRUPT_3		//interrupt 3
#define INTERRUPT_4		//interrupt 4

#define ISR_0   interrupt 0
#define ISR_1   interrupt 1
#define ISR_2   interrupt 2
#define ISR_3   interrupt 3
#define ISR_4   interrupt 4

#define USING_REG_0    using 0
#define USING_REG_1    using 1
#define USING_REG_2    using 2
#define USING_REG_3    using 3

#define DbgPrint                printf
#define DBG_SLASH	/

//2if define _DBG_WITH_ALL_FUNC_ may take warning in compiling
#ifndef _DBG_WITH_ALL_FUNC_
#define _DBG_CLR_WARNING_
#endif

#define DBG_SLASH	/

/*
    BYTE address for FPGA verify used!
    Refers to "UINT8 xdata G_FPGAMemPool[0x4000] _at_ 0xc000".
*/
#define FPGA_MEM_POOL_ADDR  (0x80000UL + 0x8000UL) // mapping to 0xc000~0xefff


//-----------------------------------------------------------------------------
//Header file
//-----------------------------------------------------------------------------

#include <reg320.h>
#include <absacc.h>

#include <stdio.h>
//#include <intrins.h>
#include "string.h"
#include "stdlib.h"

#include "common.h"

/*
DBG_UART = 1: Normal debug mode
DBG_UART = 0: Disable uart information
*/
#define DBG_UART	1

#if (!DBG_UART)
#define printf(a)
#endif

#if 1
#define ENTER_CRITICAL(x)		do {(x) = IE; IE &= 0x7f;} while(0)
#define EXIT_CRITICAL(x)			do {XBYTE[0x2D00] |= 0x08;IE = (x);} while(0)
#else
#include "dbg_def.h"
#include "hal_global.h"

#define ENTER_CRITICAL(x)		{\
									do {\
										(x) = IE;\
										IE &= 0x7f;\
										HAL_GlobalTimerRead10us(&G_TimerStartRef);\
										ENTER_CRITICAL_TIME_MEASURE(__LINE);\
									} while(0);\
								}
#define EXIT_CRITICAL(x)		{\
									do {\
										HAL_GlobalTimerRead10us(&G_TimerEndRef);\
										XBYTE[0x2D00] |= 0x08;\
										IE = (x);\
										EXIT_CRITICAL_TIME_MEASURE(__LINE);\
									} while(0);\
								}

extern UINT32 xdata G_TimerStartRef;
extern UINT32 xdata G_TimerEndRef;
extern void ENTER_CRITICAL_TIME_MEASURE(UINT16 sLine);
extern void EXIT_CRITICAL_TIME_MEASURE(UINT16 eLine);

#endif

#define FPGA_VERIFY   1

#define TAG_TODO	0

//#define strcpy(pDst, pSrc)  strncpy(pDst, pSrc/*, sizeof(pDst)*/), //Please use "strncpy" instead of "strcpy"

extern switchbank (unsigned char bank_number);
extern char INTR_putchar (char c);
//patch5.2.1@ada@Different chip select pin for SMC end
extern xdata UINT8 G_ShadowData[];
extern const UINT8 xdata prjDbgMode;
extern UINT8 xdata gErrLine;

#ifdef FPGA_MEM_POOL_ADDR
extern UINT8 xdata G_FPGAMemPool[0x3000];
#endif

#endif /*__GENERAL_H__*/
