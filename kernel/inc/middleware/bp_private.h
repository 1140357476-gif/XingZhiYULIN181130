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
/**
 * @file		bp_private.h
 * @brief		badpixel function private definition.
 * @author	Phil Lin
 * @since		2008-11-18
 * @date		2008-11-18
 */
#ifndef __BP_PRIVATE_H__
#define __BP_PRIVATE_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "asicreg.h"
#include "reg_dram.h"
#include "reg_def.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
 
/* capture bad pixel compesation by F/W */
#define BP_CMP_BY_FW	1

/* capture bad pixel compesation assert enable */
#define BP_ASSERT_EN	1

#define BP_USE_MOE 1

/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/
		
#define BP_DRAM_SetStartAddress(a,b)   \
{\
	M_REG_DRAM_StRowLow  = READ8(a,3);	 \
	M_REG_DRAM_StRowHigh = READ8(a,2);	 \
	M_REG_DRAM_StColLow  = READ8(a,1);	 \
	M_REG_DRAM_StColHigh = READ8(a,0);	 \
	if ((b)) {M_DRAM_PrefetchOn; } \
}


#define BP_DRAM_WriteWord(a, b)	\
{ \
	M_DRAM_WaitReady; \
	M_REG_DRAM_LsbByte = (a); \
	M_REG_DRAM_MsbByte = (b); \
}


#define BP_DRAM_Readword(a, b) \
{\
	M_DRAM_WaitReady; \
	(a) = M_REG_DRAM_LsbByte; \
	(b) = M_REG_DRAM_MsbByte;\
}

#define BP_DRAM_ReadPword(a, b) \
{\
	M_DRAM_WaitReady; \
	*(a) = M_REG_DRAM_LsbByte; \
	*(b) = M_REG_DRAM_MsbByte;\
}

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/
typedef struct {
	UINT32 srcCurAddr;
	UINT32 dstCurAddr;
	UINT16 xdata* pwDmem;
	UINT16 lineEnd;
	UINT16 curY;
	UINT8 curBadCnt;	// <32
	UINT32 badCnt;
} bpFMT_t;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern UINT8 xdata g_planeWidth;
extern UINT8 xdata g_planeHeight;
extern UINT16 xdata g_stripWidth;
extern UINT8 xdata g_stripHeight;
extern UINT8 xdata g_threshold;
extern UINT32 xdata g_badPixelCnt;
extern UINT32 xdata g_bpNumMaxPrv_HD;
extern UINT32 xdata g_bpNumMaxPrv;
extern UINT32 xdata g_bpNumMaxCap;
extern UINT32 xdata* xdata gp_bpNumMax;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT8 bpDramBpFix(UINT32 addr, UINT16 Hsize, UINT16 xoff, UINT16 yoff);
UINT32 bpFmt_Cvt(UINT32 srcAddr, UINT32 dstAddr);

#endif  /* __BP_PRIVATE_H__ */


