/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      osd_buff.c
 * @brief     osd buffer related function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_OSD_BUFF__ 
#include "general.h"
#include "dbg_def.h"

#include "hal.h" 
#include "hal_dram.h"
#include "hal_disp.h" 
#include "hal_gprm.h" 

#include "osd_buff.h" 
#include "osd_mem.h" 
#include "osd_api.h"

#if MODULE_OSD_OPT
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define OSD_BUFF_W    320
#define OSD_BUFF_H    240 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static osdDispBuff_t xdata gOsdDispBuff; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 osdDispBuffInit(void)
 * @brief     osdDispBuffInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdDispBuffInit(
	void
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	osdMem_t* pMem;

	pBuf->pAct = &pBuf->addrA;
	pBuf->pInAct = &pBuf->addrB;
	
	osdMemCbGet(&pMem);
	pBuf->addrA = pMem->dispA.addr;
	pBuf->addrB = pMem->dispB.addr;
	
	pBuf->buffW = OSD_BUFF_W;
	pBuf->buffH = OSD_BUFF_H;
		
	return SUCCESS;
}

/**
 * @fn        UINT32 osdDispBuffSw(void)
 * @brief     osdDispBuffSw
 * @param     NONE
 * @retval    return = osd current buffer address (word based)
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT32
osdDispBuffSw(
	void
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	UINT32* pTmp;

	/* address switch */
	pTmp = pBuf->pAct;
	pBuf->pAct = pBuf->pInAct;
	pBuf->pInAct = pTmp;
	
	return (UINT32)*pBuf->pAct;
}

/**
 * @fn        void osdDispBuffRst(void)
 * @brief     osdDispBuffRst
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
void
osdDispBuffRst(
	void
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	UINT32 osdAddr;
	
	HAL_DispOsdBuffAddrGet(&osdAddr);

	if (osdAddr == pBuf->addrA) {
		pBuf->pAct = &pBuf->addrA;
		pBuf->pInAct = &pBuf->addrB;		
	} else if (osdAddr == pBuf->addrB) {
		pBuf->pAct = &pBuf->addrB;
		pBuf->pInAct = &pBuf->addrA;		
	} else {
		/* exception correct */
		ASSERT(0, 1);
		pBuf->pAct = &pBuf->addrA;
		pBuf->pInAct = &pBuf->addrB;
		HAL_DispOsdBuffAddrSet(pBuf->addrA);
	}
}

/**
 * @fn        UINT32 osdDispBuffShow(void)
 * @brief     osdDispBuffShow
 * @param     NONE
 * @retval    return = osd current buffer address (word based)
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT32
osdDispBuffShow(
	void
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	
	HAL_DispOsdBuffAddrSet((UINT32)*pBuf->pAct);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
	
	return (UINT32)*pBuf->pAct;
}

/**
 * @fn        UINT32 osdDispBuffAddrGet(UINT8 bAct)
 * @brief     osdDispBuffAddrGet
 * @param     [in] bAct : 1: acitve buffer, 0: inactive buffer
 * @retval    return = specified osd buffer address (word based)
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT32
osdDispBuffAddrGet(
	UINT8 bAct
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	UINT32 addr;
	
	if (bAct) {
		addr = (UINT32)*pBuf->pAct;
	} else {
		addr = (UINT32)*pBuf->pInAct;	
	}
	
	return addr;
}

/**
 * @fn        void osdDispBuffSizeGet(UINT16* buffW, UINT16* buffH)
 * @brief     osdDispBuffSizeGet
 * @param     [out] buffW
 * @param     [out] buffH
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
void
osdDispBuffSizeGet(
	UINT16* buffW,
	UINT16* buffH
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	
	if(buffW) *buffW = pBuf->buffW;
	if(buffH) *buffH = pBuf->buffH;
}

/**
 * @fn        UINT8 osdDispBuffCpy(UINT8 bActSrc, UINT8 bActDst)
 * @brief     osdDispBuffCpy
 * @param     [in] bActSrc   0: the source buffer is inactive, 1: the source buffer is active
 * @param     [in] bActDst   0: the destination buffer is inactive, 1: the destination buffer is active
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
osdDispBuffCpy(
	UINT8 bActSrc,
	UINT8 bActDst
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	UINT8 ret = FAIL;
	UINT32* pSrcBuf;
	UINT32* pDstBuf;
	
#if 1	
	if (bActSrc == bActDst) return FAIL;
	
	if (bActSrc) {
		pSrcBuf = pBuf->pAct;
		pDstBuf = pBuf->pInAct;
	} else {
		pSrcBuf = pBuf->pInAct;
		pDstBuf = pBuf->pAct;
	}
	
	ret = HAL_GprmDramDmaExt(
		(UINT32)*pSrcBuf << 1,
		(UINT32)*pDstBuf << 1,
		(UINT32)pBuf->buffW * pBuf->buffH,
		0,
		1
	);
#else
	if (bActSrc == bActDst) return FAIL;
	
	if (bActSrc) {
		ret = HAL_GprmCopy(
			(UINT32)*pBuf->pAct, pBuf->buffW >> 1, pBuf->buffH, 0, 0,
			(UINT32)*pBuf->pInAct, pBuf->buffW >> 1, pBuf->buffH, 0, 0,
			pBuf->buffW >> 1, pBuf->buffH,
			0, 1
		);
	} else {
		ret = HAL_GprmCopy(
			(UINT32)*pBuf->pInAct, pBuf->buffW >> 1, pBuf->buffH, 0, 0,
			(UINT32)*pBuf->pAct, pBuf->buffW >> 1, pBuf->buffH, 0, 0,
			pBuf->buffW >> 1, pBuf->buffH,
			0, 1
		);
	}
#endif

	return ret;
}

/**
 * @fn        UINT8 osdDispBuffFill(UINT8 bActDst, UINT8 bColor)
 * @brief     osdDispBuffFill
 * @param     [in] bActDst
 * @param     [in] bColor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osdDispBuffFill(
	UINT8 bActDst,
	UINT8 bColor
)
{
	osdDispBuff_t* pBuf = &gOsdDispBuff;
	osdIndexSave();
	HAL_GprmPenDrawCfg(
		1,
		bColor, 0, 0,
		osdDispBuffAddrGet(bActDst), pBuf->buffW >> 1, pBuf->buffH
	);

	HAL_GprmPenRectDraw(0, 0, pBuf->buffW >> 1, pBuf->buffH, 1);
	osdIndexRestore();
	return SUCCESS;
}
#endif /* MODULE_OSD_OPT */
