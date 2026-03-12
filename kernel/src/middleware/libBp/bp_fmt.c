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
 * @file		bp_fmt.c
 * @brief		badpixel format convertion
 * @author	Phil Lin
 * @since		2008-11-18
 * @date		2008-11-18
 */
 
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h" 
#include "bp_private.h" 
#include "bp_api.h"
#include "hal.h"

#if MODULE_BP_OPT
/**************************************************************************
 *  						 C O N S T A N T S  						  *
 **************************************************************************/
#define DBG_BP_EN	0

#define MAX_BAD_PER_LINE	32

/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *  					   G L O B A L    D A T A   					  *
 **************************************************************************/
static bpFMT_t xdata sBpFmt;

/**************************************************************************
 *  			   E X T E R N A L    R E F E R E N C E S   			  *
 **************************************************************************/

/**************************************************************************
 *  			 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/


//-----------------------------------------------------------------------------
//bpFmt_init
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_init
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_init(void)
{
	sBpFmt.pwDmem = (UINT16 xdata *) 0x3800;
	sBpFmt.lineEnd = 0x7FF;

	sBpFmt.curY = 0;
	sBpFmt.curBadCnt = 0;

	sBpFmt.badCnt = 0;
}

//-----------------------------------------------------------------------------
//bpFmt_ReadBadXY
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_ReadBadXY
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_ReadBadXY(UINT16 *pX, UINT16 *pY)
{
	BP_DRAM_SetStartAddress(sBpFmt.srcCurAddr, 1);
	BP_DRAM_ReadPword((UINT8*)pX + 0, (UINT8*)pX + 1);
	BP_DRAM_ReadPword((UINT8*)pY + 0, (UINT8*)pY + 1);
	sBpFmt.srcCurAddr += 2;

#if DBG_BP_EN	
	printf("pX: %lx, ", (UINT32)*pX);
	printf("pY: %lx\n", (UINT32)*pY);
#endif
}

//-----------------------------------------------------------------------------
//bpFmt_AddX
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_AddX
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_AddX(UINT16 x)
{
	sBpFmt.pwDmem[sBpFmt.curBadCnt] = x;
	sBpFmt.curBadCnt ++;
}

//-----------------------------------------------------------------------------
//bpFmt_FillSaveBadIdx
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_FillSaveBadIdx
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_FillSaveBadIdx(UINT16 idx)
{
	UINT8 cnt, tmp;
	UINT16 val, idxVal;

	idxVal = (idx & 0x1f) << 11;

	for (cnt = 0; cnt < sBpFmt.curBadCnt; cnt ++) {
		val = idxVal + sBpFmt.pwDmem[cnt];
#if DBG_BP_EN			
		printf("pwDmem: %lx\n", (UINT32)sBpFmt.pwDmem[cnt]);
#endif		
		/* exchange byte order */
#if 1
		tmp = READ8(val, 0);
		WRITE8(val, 0, READ8(val, 1));
		WRITE8(val, 1, tmp);
#endif		
		sBpFmt.pwDmem[cnt] = val;
	}
	
#if DBG_BP_EN		
	printf("cnt: %lx\n", (UINT32) cnt);
#endif

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(sBpFmt.dstCurAddr);
	HAL_DmaDo(1, 0, cnt << 1, 0, 0);
	sBpFmt.dstCurAddr += cnt;
}

//-----------------------------------------------------------------------------
//bpFmt_FillSaveEndLine
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_FillSaveEndLine
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_FillSaveEndLine(UINT16 idx)
{
	UINT16 val;
	UINT8 buf[2];

	val = (idx & 0x1f) << 11;
	val += sBpFmt.lineEnd;

	buf[0] = (UINT8) val;
	buf[1] = (UINT8) (val >> 8);

	BP_DRAM_SetStartAddress(sBpFmt.dstCurAddr, 0);
	BP_DRAM_WriteWord(buf[0], buf[1]);
	sBpFmt.dstCurAddr++;
}

//-----------------------------------------------------------------------------
//bpFmt_FillSaveEndFrame
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_FillSaveEndFrame
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_FillSaveEndFrame()
{
	UINT8 buf[2];

	buf[0] = 0xfe;
	buf[1] = 0x07;

	BP_DRAM_SetStartAddress(sBpFmt.dstCurAddr, 0);
	BP_DRAM_WriteWord(buf[0], buf[1]);
	sBpFmt.dstCurAddr++;
}

//-----------------------------------------------------------------------------
//bpFmt_EndLastLine
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_EndLastLine
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpFmt_EndLastLine(UINT16 nextY)
{
	UINT16 idxY;
	UINT16 curY;

	curY = sBpFmt.curY;
	idxY = nextY - curY;
	if (sBpFmt.curBadCnt != 0) {
		/* end current line */
		/* fill bad buf */
		if (idxY <= MAX_BAD_PER_LINE) {
			bpFmt_FillSaveBadIdx(idxY);
			idxY = 0;
		} else {
			bpFmt_FillSaveBadIdx(0);
			idxY -= MAX_BAD_PER_LINE;
		}

		/* fill end frame or end line */
		if (curY == nextY) {
			bpFmt_FillSaveEndFrame();
		} else if (sBpFmt.curBadCnt < MAX_BAD_PER_LINE) {
			bpFmt_FillSaveEndLine(0);
		}
	}

	while (idxY) {
		/* nextY-curY > 32 */
		if (idxY >= MAX_BAD_PER_LINE) {
			bpFmt_FillSaveEndLine(MAX_BAD_PER_LINE - 1);
			idxY -= (MAX_BAD_PER_LINE - 1);
		} else {
			/* < 32 */
			bpFmt_FillSaveEndLine(idxY);
			idxY = 0;
		}
	}

	sBpFmt.curY = nextY;
	sBpFmt.curBadCnt = 0;
}

//-----------------------------------------------------------------------------
//bpFmt_AddBad
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_AddBad
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 bpFmt_AddBad(UINT16 x, UINT16 y)
{
	if (x == 0xffff || y == 0xffff) {
		/* frame end */
		bpFmt_EndLastLine(sBpFmt.curY);
		return FAIL;
	} else if (y > sBpFmt.curY) {
		/* new line */
		bpFmt_EndLastLine(y);
		bpFmt_AddX(x);
	} else if (sBpFmt.curBadCnt < MAX_BAD_PER_LINE) {
		bpFmt_AddX(x);
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpFmt_Cvt
//-----------------------------------------------------------------------------
/**
 * @brief		bpFmt_Cvt
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT32 bpFmt_Cvt(UINT32 srcAddr, UINT32 dstAddr)
{
	UINT16 x, y;
	UINT32 addr;

	sBpFmt.srcCurAddr = srcAddr;
	sBpFmt.dstCurAddr = dstAddr;
	addr = dstAddr;

	bpFmt_init();

	while (1) {
		bpFmt_ReadBadXY(&x, &y);
		if (bpFmt_AddBad(x, y) == SUCCESS) {
			sBpFmt.badCnt ++;
		} else {
			break;
		}
	}
	//printf("bad total:%lu\n", sBpFmt.badCnt);   	 

	/* return the data size after convertion */
	return ((sBpFmt.dstCurAddr - addr) << 1);
}
#endif /* MODULE_BP_OPT */