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

#ifndef __ASICREG_H__
#define __ASICREG_H__

#include "reg_def.h"
//#include "chipopt.h"

// wyeo@0527, patch 4.2, whole file

// 8051 Macro
#if 1//(K_HAVE_HAL)
#define M_ENABLE_8051_INT
#define M_DISABLE_8051_INT
#else	
#define M_ENABLE_8051_INT		ET0 = 1
#define M_DISABLE_8051_INT		ET0 = 0
#endif

// register index and name mapping
#define K_REG_DRAM_Control		(REG_Dram_IndirectPrefetch)

//#define K_REG_DRAM_Status		(REG_Dram_IndirectPrefetch)
#define K_REG_DRAM_LsbByte		(REG_Dram_IndirectRData_Low) 
#define K_REG_DRAM_MsbByte		(REG_Dram_IndirectRData_High) 
#define K_REG_DRAM_StRowLow		(REG_Dram_IndirectAddr_B0)
#define K_REG_DRAM_StRowHigh	(REG_Dram_IndirectAddr_B1)
#define K_REG_DRAM_StColLow		(REG_Dram_IndirectAddr_B2)
#define K_REG_DRAM_StColHigh	(REG_Dram_IndirectAddr_B3)

#define K_REG_QTABLE			(REG_Jpeg_Intr_Event)

#define K_REG_DRAM_Control_Prefetch	(0x01)

// register name
#define M_REG_DRAM_Status			XBYTE[K_REG_DRAM_Control]

#define M_REG_DRAM_Control			XBYTE[K_REG_DRAM_Control]

#define M_REG_DRAM_LsbByte			XBYTE[K_REG_DRAM_LsbByte]
#define M_REG_DRAM_MsbByte			XBYTE[K_REG_DRAM_MsbByte]

#define M_REG_DRAM_StRowLow			XBYTE[K_REG_DRAM_StRowLow]
#define M_REG_DRAM_StRowHigh		XBYTE[K_REG_DRAM_StRowHigh]
#define M_REG_DRAM_StColLow			XBYTE[K_REG_DRAM_StColLow]
#define M_REG_DRAM_StColHigh		XBYTE[K_REG_DRAM_StColHigh]

#define M_REG_QTABLE				XBYTE[K_REG_QTABLE]

// Macro
#define M_DRAM_WaitReady			{while (M_REG_DRAM_Status);}
#define M_DRAM_PrefetchOn			{M_REG_DRAM_Control = K_REG_DRAM_Control_Prefetch;}

//wyeo@0527, patch 4.2
#if 1
#define M_DRAM_CardSetStartAddress(a,b) \
{ \
	M_REG_DRAM_StRowLow  = READ8(a,3);	 \
	M_REG_DRAM_StRowHigh = READ8(a,2);	 \
	M_REG_DRAM_StColLow  = READ8(a,1);	 \
	M_REG_DRAM_StColHigh = READ8(a,0);	 \
	G_DOS_SDRAM_NextIntRetAddr = (a);\
	if ((b)) {M_DRAM_PrefetchOn; M_DRAM_WaitReady; G_File_SDRAM_Read = 1;} \
		else G_File_SDRAM_Read = 0;\
}


#define M_DRAM_SetStartAddress(a,b)   \
{\
	M_REG_DRAM_StRowLow = (a);    \
	M_REG_DRAM_StRowHigh = (a)>>8;    \
	M_REG_DRAM_StColLow = (a)>>16;    \
	M_REG_DRAM_StColHigh = (a)>>24;   \
	if ((b)) {M_DRAM_PrefetchOn; M_DRAM_WaitReady;} \
}

                                              
#define M_DRAM_CardSetStartAddressV(a,b) 	\
{ \
	M_REG_DRAM_StRowLow  = READ8(a,3);	 \
	M_REG_DRAM_StRowHigh = READ8(a,2);	 \
	M_REG_DRAM_StColLow  = READ8(a,1);	 \
	M_REG_DRAM_StColHigh = READ8(a,0);	 \
	G_DOS_SDRAM_NextIntRetAddr = (a);\
	if ((b)) {M_DRAM_PrefetchOn; M_DRAM_WaitReady; G_File_SDRAM_Read = 1;} \
		else G_File_SDRAM_Read = 0; \
}


#define M_DRAM_SetStartAddressV(a,b)     \
{ \
	M_REG_DRAM_StRowLow= READ8(a,3);   \
	M_REG_DRAM_StRowHigh= READ8(a,2);	\
	M_REG_DRAM_StColLow = READ8(a,1);	\
	M_REG_DRAM_StColHigh= READ8(a,0);	\
	if ((b)) {M_DRAM_PrefetchOn; M_DRAM_WaitReady;} \
}

                                              
#endif
                                              
#define M_DRAM_WriteWord(a, b)	\
{ \
	M_DRAM_WaitReady; \
	M_REG_DRAM_LsbByte = (a); \
	M_REG_DRAM_MsbByte = (b); \
}
									
					
#define M_DRAM_ReadWord(a, b) \
{\
	M_DRAM_WaitReady; \
	(a) = M_REG_DRAM_LsbByte; \
	(b) = M_REG_DRAM_MsbByte;\
}

#if 1//(K_HAVE_HAL)
#define M_AUDIO_DISABLE_8051_INT
#define M_AUDIO_ENABLE_8051_INT	
#else
#if (AUDIO_OPTION == 1)
#define M_AUDIO_DISABLE_8051_INT	if (G_File_AudioEnable)		\
        					M_DISABLE_8051_INT;
#else
#define M_AUDIO_DISABLE_8051_INT
#endif

#if (AUDIO_OPTION == 1)
#define M_AUDIO_ENABLE_8051_INT		if (G_File_AudioEnable)		\
        					M_ENABLE_8051_INT;
#else
#define M_AUDIO_ENABLE_8051_INT
#endif
#endif

// HIS101c-0002 end

#endif /*__ASICREG_H__*/
