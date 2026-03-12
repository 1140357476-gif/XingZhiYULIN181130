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
 * @file	   hal_gprm.c
 * @brief	   hal gprm module api
 * @author    Phil Lin
 * @since	   2010-01-01
 * @date	   2010-01-01
*/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_HAL_GPRM__ 
#include "general.h"
#include "hal.h"
#include "hal_gprm.h"
#include "dbg_def.h"
#include "dbg_file_list.h"
#include "hal_global.h"
#include "reg_def.h"

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define GPRM_TIME_OUT_REF     0x08  //8 * 0x10000 * 10us = 2.6s ~ 2.6*2s
#define GPRM_TIME_OUT_MASK    0x0C 

#define __ret  sGprmTimeOut
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HAL_GprmTrig()	{XBYTE[REG_Gprm_Trg_IntSts] = 0x11;} 
#define HAL_GprmRst()   {XBYTE[REG_Gprm_SwRst_IntEn] |= 0x01; XBYTE[REG_Gprm_SwRst_IntEn] &= ~0x01;}
#define HAL_GprmOpModeSet(mode) {XBYTE[REG_Gprm_OpMode] = (mode);}

#define __while(_case_, ret) \
\
	(ret) = XBYTE[0x20C3]; \
	(ret) += GPRM_TIME_OUT_REF; \
	(ret) &= GPRM_TIME_OUT_MASK; \
	while ( \
		(_case_) && \
		(\
			((XBYTE[0x20C3] & GPRM_TIME_OUT_MASK) != (ret)) || \
			(((ret) = -1) && !__ASSERT(__FILE, __LINE, 1)) \
		)\
	)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	GPRM_OP_FUNC_IDLE		= 0,
	GPRM_OP_FUNC_SCALE		= 1,
	GPRM_OP_FUNC_ROTATE 	= 2,
	GPRM_OP_FUNC_CPY		= 3,//GPRM_DRAM_WRITE function
	GPRM_OP_FUNC_OSD_TRSF	= 6,
	GPRM_OP_FUNC_MAX,
} gprmOpFunc_e;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 xdata sGprmTimeOut _at_ 0x0100;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT8 HAL_GprmScadnh(UINT32 srcAddrWord, UINT16 srcHSize, UINT32 dstAddrWord, 
	UINT16 dtnHSize, UINT16 vSize, UINT8 waitMode);
static UINT8 HAL_GprmScadnv(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, 
	UINT16 dstVSize, UINT16 hSize, UINT8 waitMode);
static UINT8 HAL_GprmScauph(UINT32 srcAddrWord, UINT16 srcHSize, UINT32 dtnAddrWord, 
	UINT16 dstHSize, UINT16 vSize, UINT8 waitMode);
static UINT8 HAL_GprmScaupv(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dtnAddrWord, 
	UINT16 dtnvsize, UINT16 hSize, UINT8 waitMode);
static UINT8 HAL_GprmScaupVFactor(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, 
	UINT16 dtnVSize, UINT16 hSize, UINT16 vSF, UINT16 vISF, UINT8 waitMode);
static UINT8 HAL_GprmScadnVFactor(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, 
	UINT16 dtnVSize, UINT16 hSize, UINT16 vSF, UINT16 vISF, UINT8 waitMode);

#if 0
#endif
/**
 * @fn        void HAL_GprmParaSrcBuffSet(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize)
 * @brief     HAL_GprmParaSrcBuffSet
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaSrcBuffSet(
	UINT32 srcAddrWord, 
	UINT16 srcHSize,
	UINT16 srcVSize
)
{
	/* set gprmsb_staddr. */
	XBYTE[REG_Dram_Gprmsb_StAddr_B0] = READ8(&srcAddrWord, 3);
	XBYTE[REG_Dram_Gprmsb_StAddr_B1] = READ8(&srcAddrWord, 2);
	XBYTE[REG_Dram_Gprmsb_StAddr_B2] = READ8(&srcAddrWord, 1);
	XBYTE[REG_Dram_Gprmsb_StAddr_B3] = READ8(&srcAddrWord, 0);

	/* set gprmsb_hsize. */
	XBYTE[REG_Dram_Gprmsb_HSize_Low] = READ8(&srcHSize, 1);
	XBYTE[REG_Dram_Gprmsb_HSize_High] = READ8(&srcHSize, 0);

	/* set gprmsb_vsize. */
	XBYTE[REG_Dram_Gprmsb_VSize_Low] = READ8(&srcVSize, 1);
	XBYTE[REG_Dram_Gprmsb_VSize_High] = READ8(&srcVSize, 0);	
}

/**
 * @fn        void HAL_GprmParaSrcDatOftSet(UINT16 srcHOft, UINT16 srcVOft)
 * @brief     HAL_GprmParaSrcDatOftSet
 * @param     [in] srcHOft
 * @param     [in] srcVOft
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaSrcDatOftSet(
	UINT16 srcHOft, 
	UINT16 srcVOft
)
{
	/* set Agbhoff, must align to 2. */
	XBYTE[REG_Gprm_AgbhoffL] = READ8(&srcHOft, 1);
	XBYTE[REG_Gprm_AgbhoffH] = READ8(&srcHOft, 0);

	/* set Agbvoff. */
	XBYTE[REG_Gprm_AgbvoffL] = READ8(&srcVOft, 1);
	XBYTE[REG_Gprm_AgbvoffH] = READ8(&srcVOft, 0);
}

/**
 * @fn        void HAL_GprmParaDstBuffSet(UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize)
 * @brief     HAL_GprmParaDstBuffSet
 * @param     [in] dstAddrWord
 * @param     [in] dstHSize
 * @param     [in] dstVSize
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaDstBuffSet(
	UINT32 dstAddrWord, 
	UINT16 dstHSize,
	UINT16 dstVSize
)
{
	/* set gprmdb_staddr. */
	XBYTE[REG_Dram_Gprmdb_StAddr_B0] = READ8(&dstAddrWord, 3);
	XBYTE[REG_Dram_Gprmdb_StAddr_B1] = READ8(&dstAddrWord, 2);
	XBYTE[REG_Dram_Gprmdb_StAddr_B2] = READ8(&dstAddrWord, 1);
	XBYTE[REG_Dram_Gprmdb_StAddr_B3] = READ8(&dstAddrWord, 0);

	/* set gprmdb_hsize. */
	XBYTE[REG_Dram_Gprmdb_HSize_Low] = READ8(&dstHSize, 1);
	XBYTE[REG_Dram_Gprmdb_HSize_High] = READ8(&dstHSize, 0);

	/* set gprmdb_vsize. */
	XBYTE[REG_Dram_Gprmdb_VSize_Low] = READ8(&dstVSize, 1);
	XBYTE[REG_Dram_Gprmdb_VSize_High] = READ8(&dstVSize, 0);
}

/**
 * @fn        void HAL_GprmParaDstOpOftSet(UINT16 opHOft, UINT16 opVOft)
 * @brief     HAL_GprmParaDstOpOftSet
 * @param     [in] opHOft
 * @param     [in] opVOft
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaDstOpOftSet(
	UINT16 opHOft, 
	UINT16 opVOft
)
{
	/* set Bgbhoff, must align to 2. */
	XBYTE[REG_Gprm_BgbhoffL] = READ8(&opHOft, 1);
	XBYTE[REG_Gprm_BgbhoffH] = READ8(&opHOft, 0);

	/* set Bgbvoff. */
	XBYTE[REG_Gprm_BgbvoffL] = READ8(&opVOft, 1);
	XBYTE[REG_Gprm_BgbvoffH] = READ8(&opVOft, 0);
}

/**
 * @fn        void HAL_GprmParaDstOpSizeSet(UINT16 opHSize, UINT16 opVSize)
 * @brief     HAL_GprmParaDstOpSizeSet
 * @param     [in] opHSize
 * @param     [in] opVSize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaDstOpSizeSet(
	UINT16 opHSize, 
	UINT16 opVSize
)
{
	/* set Bgbwidth. */
	XBYTE[REG_Gprm_BgbwidthL] = READ8(&opHSize, 1);
	XBYTE[REG_Gprm_BgbwidthH] = READ8(&opHSize, 0);
	
	/* set Bgbheight. */
	XBYTE[REG_Gprm_BgbheightL] = READ8(&opVSize, 1);
	XBYTE[REG_Gprm_BgbheightH] = READ8(&opVSize, 0);
}

/**
 * @fn        void HAL_GprmParaSclSet(UINT8 sclMode, UINT16 sclFct, UINT16 sclFctIni)
 * @brief     HAL_GprmParaSclSet
 * @param     [in] sclMode
 * @param     [in] sclFct
 * @param     [in] sclFctIni
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmParaSclSet(
	UINT8 sclMode,
	UINT16 sclFct, 
	UINT16 sclFctIni
)
{
	/* set scamode. */
	XBYTE[REG_Gprm_SclMode] = sclMode;

	/* set scalefactor. */
	XBYTE[REG_Gprm_SclFctL] = READ8(&sclFct, 1);
	XBYTE[REG_Gprm_SclFctH] = READ8(&sclFct, 0);

	/* set iniscalefactor. */
	XBYTE[REG_Gprm_SclFctIniL] = READ8(&sclFctIni, 1);
	XBYTE[REG_Gprm_SclFctIniH] = READ8(&sclFctIni, 0);	
}
#if 0
#endif
/**
 * @fn        UINT8 HAL_GprmDramFill(UINT32 dstAddrWord, UINT32 sizeByte, UINT8 fillData)
 * @brief     HAL_GprmDramFill
 * @param     [in] dstAddrWord
 * @param     [in] sizeByte
 * @param     [in] fillData
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_GprmDramFill(
	UINT32 dstAddrWord,
	UINT32 sizeByte,
	UINT8 fillData
) 
{
	return HAL_GprmDramFillExt(dstAddrWord << 1, sizeByte, fillData, 1, 0);
}

/**
 * @fn        UINT8 HAL_GprmDramFillExt(UINT32 dstAddrByte, UINT32 sizeByte, UINT8 fillData, UINT8 waitMode, UINT8 dmaChn)
 * @brief     HAL_GprmDramFillExt
 * @param     [in] dstAddrByte
 * @param     [in] sizeByte
 * @param     [in] fillData
 * @param     [in] waitMode
 * @param     [in] dmaChn
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_GprmDramFillExt(
	UINT32 dstAddrByte,
	UINT32 sizeByte,
	UINT8 fillData,
	UINT8 waitMode,
	UINT8 dmaChn
)
{
#define DRAM_DMA_SIZE_MAX	0x200000

	SINT32 bytecnt, bytesizemux;
	UINT32 status;
	UINT8 regOffset; //DMA0(2F6x) / DMA1(2F7x)
	
	/* check param */
	if (!sizeByte) {
		return FAIL;
	}

	/* check dma channel & register address */
	if (dmaChn == 1) {
		dmaChn = 0x01 << 1;
		regOffset = 0x10;
	} else {
		dmaChn = 0x01 << 0;
		regOffset = 0;
	}
	
	/* check dram dma 0 ready */
	__while((XBYTE[REG_Gprm_DmaBusy] & dmaChn), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
		return FAIL;
	}

	for(bytecnt = sizeByte; bytecnt > 0; bytecnt -= DRAM_DMA_SIZE_MAX){

		ENTER_CRITICAL(status);

		/* set to dram fill mode */
		XBYTE[REG_Gprm_FillDramEn] |= dmaChn;
		
		XBYTE[REG_Gprm_Dma0DstAddr0 | regOffset] = READ8(dstAddrByte,3); //GPRMDB addr
		XBYTE[REG_Gprm_Dma0DstAddr1 | regOffset] = READ8(dstAddrByte,2);
		XBYTE[REG_Gprm_Dma0DstAddr2 | regOffset] = READ8(dstAddrByte,1);
		XBYTE[REG_Gprm_Dma0DstAddr3 | regOffset] = READ8(dstAddrByte,0);
		
		bytesizemux = (bytecnt > DRAM_DMA_SIZE_MAX) ? DRAM_DMA_SIZE_MAX : bytecnt;
		bytesizemux--;

		XBYTE[REG_Gprm_Dma0ByteCnt0 | regOffset] = READ8(bytesizemux,3);
		XBYTE[REG_Gprm_Dma0ByteCnt1 | regOffset] = READ8(bytesizemux,2);
		XBYTE[REG_Gprm_Dma0ByteCnt2 | regOffset] = READ8(bytesizemux,1) & 0x1f;

		XBYTE[REG_Gprm_FillDat] = fillData;

		/* chear DMA status */
		XBYTE[REG_Gprm_DmaIntSts] = dmaChn; 
		/* start DMA */
		XBYTE[REG_Gprm_Dma0Start | regOffset] = 0x01;

		EXIT_CRITICAL(status);

		/* wait dma finish */
		if(bytecnt > DRAM_DMA_SIZE_MAX || waitMode) {

			__while((XBYTE[REG_Gprm_DmaBusy] & dmaChn), __ret);
			if (__ret == -1) {
				HAL_GprmRst();
				return FAIL;
			}	
		}

		dstAddrByte += DRAM_DMA_SIZE_MAX;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmDramDma(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode)
 * @brief     DMA copy data all in dram (Using dram DMA channel 0, GPRM independent)
 * @param     [in] srcAddrByte
 * @param     [in] dstAddrByte
 * @param     [in] sizeByte
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_GprmDramDma(
	UINT32 srcAddrByte, 
	UINT32 dstAddrByte, 
	UINT32 sizeByte, 
	UINT8 waitMode
)
{
	/* use dram DMA channel 0 */
	UINT8 chr = 0;//(XBYTE[REG_Gprm_DmaBusy]&0x01)?1:0;

	return HAL_GprmDramDmaExt(srcAddrByte, dstAddrByte, sizeByte, waitMode, chr);
}
UINT8
HAL_GprmDramDmaAdv(
	UINT32 srcAddrByte, 
	UINT32 dstAddrByte, 
	UINT32 sizeByte, 
	UINT8 waitMode
)
{
	/* select dram DMA channel idle */
	UINT8 chr ;// = (XBYTE[REG_Gprm_DmaBusy]&0x01)?1:0;
	dbgWdtFeed(-1);
	while((XBYTE[REG_Gprm_DmaBusy]&3)==3);
	//if((XBYTE[REG_Gprm_DmaBusy]&3)==3){
		//__while(((XBYTE[REG_Gprm_DmaBusy]&3)==3), __ret);
		//if (__ret == -1) {
			//__ASSERT_POS(1);
			//HAL_GprmRst();
			//return FAIL;
		//}
	//}
	chr = (XBYTE[REG_Gprm_DmaBusy]&0x01)?1:0;
	return HAL_GprmDramDmaExt(srcAddrByte, dstAddrByte, sizeByte, waitMode, chr);
} 
/**
 * @fn        UINT8 HAL_GprmDramDmaExt(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode, UINT8 dmaChn)
 * @brief     DMA copy data all in dram (Using dram DMA channel 0/1, GPRM independent)
 * @param     [in] srcAddrByte
 * @param     [in] dstAddrByte
 * @param     [in] sizeByte
 * @param     [in] waitMode
 * @param     [in] dmaChn
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_GprmDramDmaExt(
	UINT32 srcAddrByte,
	UINT32 dstAddrByte,
	UINT32 sizeByte,
	UINT8 waitMode,
	UINT8 dmaChn
)
{
#define DRAM_DMA_SIZE_MAX	0x200000

	SINT32 bytecnt, bytesizemux;
	UINT32 status;
	UINT8 regOffset; //DMA0(2F6x) / DMA1(2F7x)
	
	/* check param */
	if (!sizeByte) {
		return FAIL;
	}

	/* check dma channel & register address */
	if (dmaChn == 1) {
		dmaChn = 0x01 << 1;
		regOffset = 0x10;
	} else {
		dmaChn = 0x01 << 0;
		regOffset = 0;
	}
	
	/* check dram dma 0 ready */
	__while((XBYTE[REG_Gprm_DmaBusy] & dmaChn), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
		return FAIL;
	}

	for(bytecnt = sizeByte; bytecnt > 0; bytecnt -= DRAM_DMA_SIZE_MAX){

		ENTER_CRITICAL(status);

		/* set to dram dma mode */
		XBYTE[REG_Gprm_FillDramEn] &= ~dmaChn;//

		XBYTE[REG_Gprm_Dma0SrcAddr0 | regOffset] = READ8(srcAddrByte,3); //GPRMSB addr
		XBYTE[REG_Gprm_Dma0SrcAddr1 | regOffset] = READ8(srcAddrByte,2);
		XBYTE[REG_Gprm_Dma0SrcAddr2 | regOffset] = READ8(srcAddrByte,1);
		XBYTE[REG_Gprm_Dma0SrcAddr3 | regOffset] = READ8(srcAddrByte,0);
		
		XBYTE[REG_Gprm_Dma0DstAddr0 | regOffset] = READ8(dstAddrByte,3); //GPRMDB addr
		XBYTE[REG_Gprm_Dma0DstAddr1 | regOffset] = READ8(dstAddrByte,2);
		XBYTE[REG_Gprm_Dma0DstAddr2 | regOffset] = READ8(dstAddrByte,1);
		XBYTE[REG_Gprm_Dma0DstAddr3 | regOffset] = READ8(dstAddrByte,0);
		
		bytesizemux = (bytecnt > DRAM_DMA_SIZE_MAX) ? DRAM_DMA_SIZE_MAX : bytecnt;
		bytesizemux--;

		XBYTE[REG_Gprm_Dma0ByteCnt0 | regOffset] = READ8(bytesizemux,3);
		XBYTE[REG_Gprm_Dma0ByteCnt1 | regOffset] = READ8(bytesizemux,2);
		XBYTE[REG_Gprm_Dma0ByteCnt2 | regOffset] = READ8(bytesizemux,1) & 0x1f;

		/* chear DMA status */
		XBYTE[REG_Gprm_DmaIntSts] |= dmaChn; 
		/* start DMA */
		XBYTE[REG_Gprm_Dma0Start | regOffset] = 0x01;

		EXIT_CRITICAL(status);

		/* wait dma finish */
		if(waitMode || bytecnt > DRAM_DMA_SIZE_MAX) {
			__while((XBYTE[REG_Gprm_DmaBusy] & dmaChn), __ret);
			if (__ret == -1) {
				HAL_GprmRst();
				return FAIL;
			}
		}
		
		srcAddrByte += DRAM_DMA_SIZE_MAX;
		dstAddrByte += DRAM_DMA_SIZE_MAX;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmDramDmaExt_Intr(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode, UINT8 dmaChn)
 * @brief     HAL_GprmDramDmaExt_Intr
 * @param     [in] srcAddrByte
 * @param     [in] dstAddrByte
 * @param     [in] sizeByte
 * @param     [in] waitMode
 * @param     [in] dmaChn
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_GprmDramDmaExt_Intr(
	UINT32 srcAddrByte,
	UINT32 dstAddrByte,
	UINT32 sizeByte,
	UINT8 waitMode,
	UINT8 dmaChn
)
{
#define DRAM_DMA_SIZE_MAX	0x200000

	SINT32 bytecnt, bytesizemux;
	//UINT32 status;
	UINT8 regOffset; //DMA0(2F6x) / DMA1(2F7x)
	UINT32 timeOut;
	
	/* check param */
	if (!sizeByte) {
		return FAIL;
	}

	/* check dma channel & register address */
	if (dmaChn == 1) {
		dmaChn = 0x01 << 1;
		regOffset = 0x10;
	} else {
		dmaChn = 0x01 << 0;
		regOffset = 0;
	}

	/* check dram dma ready */
	timeOut = GPRM_ENG_TIME_OUT;
	while(XBYTE[REG_Gprm_DmaBusy] & dmaChn) {
		if (!timeOut--) {
			//ASSERT(0, 1);
			HAL_GprmRst();
			return FAIL;
		}
	}			
	
	for(bytecnt = sizeByte; bytecnt > 0; bytecnt -= DRAM_DMA_SIZE_MAX){

		//ENTER_CRITICAL(status);

		/* set to dram dma mode */
		XBYTE[REG_Gprm_FillDramEn] &= ~dmaChn;//

		XBYTE[REG_Gprm_Dma0SrcAddr0 | regOffset] = READ8(srcAddrByte,3); //GPRMSB addr
		XBYTE[REG_Gprm_Dma0SrcAddr1 | regOffset] = READ8(srcAddrByte,2);
		XBYTE[REG_Gprm_Dma0SrcAddr2 | regOffset] = READ8(srcAddrByte,1);
		XBYTE[REG_Gprm_Dma0SrcAddr3 | regOffset] = READ8(srcAddrByte,0);
		
		XBYTE[REG_Gprm_Dma0DstAddr0 | regOffset] = READ8(dstAddrByte,3); //GPRMDB addr
		XBYTE[REG_Gprm_Dma0DstAddr1 | regOffset] = READ8(dstAddrByte,2);
		XBYTE[REG_Gprm_Dma0DstAddr2 | regOffset] = READ8(dstAddrByte,1);
		XBYTE[REG_Gprm_Dma0DstAddr3 | regOffset] = READ8(dstAddrByte,0);
		
		bytesizemux = (bytecnt > DRAM_DMA_SIZE_MAX) ? DRAM_DMA_SIZE_MAX : bytecnt;
		bytesizemux--;

		XBYTE[REG_Gprm_Dma0ByteCnt0 | regOffset] = READ8(bytesizemux,3);
		XBYTE[REG_Gprm_Dma0ByteCnt1 | regOffset] = READ8(bytesizemux,2);
		XBYTE[REG_Gprm_Dma0ByteCnt2 | regOffset] = READ8(bytesizemux,1) & 0x1f;

		/* chear DMA status */
		XBYTE[REG_Gprm_DmaIntSts] |= dmaChn; 
		/* start DMA */
		XBYTE[REG_Gprm_Dma0Start | regOffset] = 0x01;

		//EXIT_CRITICAL(status);

		/* wait dma finish */
		if(waitMode || bytecnt > DRAM_DMA_SIZE_MAX) {
			timeOut = GPRM_ENG_TIME_OUT;
			while(XBYTE[REG_Gprm_DmaBusy] & dmaChn) {
				if (!timeOut--) {
					//ASSERT(0, 1);
					HAL_GprmRst();
					return FAIL;
				}
			}			
		}
		
		srcAddrByte += DRAM_DMA_SIZE_MAX;
		dstAddrByte += DRAM_DMA_SIZE_MAX;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmCopy(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT16 srcHOft, UINT16 srcVOft, UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize, UINT16 dstHOft, UINT16 dstVOft, UINT16 cpHSize, UINT16 cpVSize, UINT16 pstThd, UINT8 waitMode)
 * @brief     HAL_GprmCopy
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] srcHOft
 * @param     [in] srcVOft
 * @param     [in] dstAddrWord
 * @param     [in] dstHSize
 * @param     [in] dstVSize
 * @param     [in] dstHOft
 * @param     [in] dstVOft
 * @param     [in] cpHSize
 * @param     [in] cpVSize
 * @param     [in] pstThd
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 
HAL_GprmCopy(
	UINT32 srcAddrWord, 
	UINT16 srcHSize, 
	UINT16 srcVSize, 
	UINT16 srcHOft, 
	UINT16 srcVOft,
	UINT32 dstAddrWord, 
	UINT16 dstHSize, 
	UINT16 dstVSize, 
	UINT16 dstHOft, 
	UINT16 dstVOft,
	UINT16 cpHSize, 
	UINT16 cpVSize,  
	UINT16 pstThd, 
	UINT8 waitMode
)
{
	UINT32 sts;
	
	/* wait GPRM ready */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
		return FAIL;
	}

	ENTER_CRITICAL(sts);

	HAL_GprmParaSrcBuffSet(srcAddrWord, srcHSize, srcVSize);
	HAL_GprmParaSrcDatOftSet(srcHOft, srcVOft);

	HAL_GprmParaDstBuffSet(dstAddrWord, dstHSize, dstVSize);
	HAL_GprmParaDstOpOftSet(dstHOft, dstVOft);

	HAL_GprmParaDstOpSizeSet(cpHSize, cpVSize);

	/* set pstThd. */
	XBYTE[REG_Gprm_PstThd] = pstThd;

	/* set imgpromode to copy-paste. */
	XBYTE[REG_Gprm_OpMode] = 0x03;
	XBYTE[REG_Gprm_DatFmt_CpEn] = 0x00;

	/* trigger. */
	HAL_GprmTrig();
	
	EXIT_CRITICAL(sts);

	/* wait operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}
	
	return SUCCESS;
}


/**
 * @fn        UINT8 HAL_GprmCopy_Intr(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT16 srcHOft, UINT16 srcVOft, UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize, UINT16 dstHOft, UINT16 dstVOft, UINT16 cpHSize, UINT16 cpVSize, UINT16 pstThd, UINT8 waitMode)
 * @brief     HAL_GprmCopy_Intr
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] srcHOft
 * @param     [in] srcVOft
 * @param     [in] dstAddrWord
 * @param     [in] dstHSize
 * @param     [in] dstVSize
 * @param     [in] dstHOft
 * @param     [in] dstVOft
 * @param     [in] cpHSize
 * @param     [in] cpVSize
 * @param     [in] pstThd
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_GprmCopy_Intr(
	UINT32 srcAddrWord, 
	UINT16 srcHSize, 
	UINT16 srcVSize, 
	UINT16 srcHOft, 
	UINT16 srcVOft,
	UINT32 dstAddrWord, 
	UINT16 dstHSize, 
	UINT16 dstVSize, 
	UINT16 dstHOft, 
	UINT16 dstVOft,
	UINT16 cpHSize, 
	UINT16 cpVSize,  
	UINT16 pstThd, 
	UINT8 waitMode
)
{
	UINT32 sts;
	UINT32 timeOut;
	
	/* wait GPRM ready */
	timeOut = GPRM_ENG_TIME_OUT;
	while(!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)) {
		if (!timeOut--) {
			//ASSERT(0, 1);
			HAL_GprmRst();
			return FAIL;
		}
	}
	ENTER_CRITICAL(sts);
	
	/* set gprmsb_staddr. */
	XBYTE[REG_Dram_Gprmsb_StAddr_B0] = READ8(&srcAddrWord, 3);
	XBYTE[REG_Dram_Gprmsb_StAddr_B1] = READ8(&srcAddrWord, 2);
	XBYTE[REG_Dram_Gprmsb_StAddr_B2] = READ8(&srcAddrWord, 1);
	XBYTE[REG_Dram_Gprmsb_StAddr_B3] = READ8(&srcAddrWord, 0);

	/* set gprmsb_hsize. */
	XBYTE[REG_Dram_Gprmsb_HSize_Low] = READ8(&srcHSize, 1);
	XBYTE[REG_Dram_Gprmsb_HSize_High] = READ8(&srcHSize, 0);

	/* set gprmsb_vsize. */
	XBYTE[REG_Dram_Gprmsb_VSize_Low] = READ8(&srcVSize, 1);
	XBYTE[REG_Dram_Gprmsb_VSize_High] = READ8(&srcVSize, 0);

	/* set Agbhoff, must align to 2. */
	XBYTE[REG_Gprm_AgbhoffL] = READ8(&srcHOft, 1);
	XBYTE[REG_Gprm_AgbhoffH] = READ8(&srcHOft, 0);

	/* set Agbvoff. */
	XBYTE[REG_Gprm_AgbvoffL] = READ8(&srcVOft, 1);
	XBYTE[REG_Gprm_AgbvoffH] = READ8(&srcVOft, 0);

	/* set gprmdb_staddr. */
	XBYTE[REG_Dram_Gprmdb_StAddr_B0] = READ8(&dstAddrWord, 3);
	XBYTE[REG_Dram_Gprmdb_StAddr_B1] = READ8(&dstAddrWord, 2);
	XBYTE[REG_Dram_Gprmdb_StAddr_B2] = READ8(&dstAddrWord, 1);
	XBYTE[REG_Dram_Gprmdb_StAddr_B3] = READ8(&dstAddrWord, 0);

	/* set gprmdb_hsize. */
	XBYTE[REG_Dram_Gprmdb_HSize_Low] = READ8(&dstHSize, 1);
	XBYTE[REG_Dram_Gprmdb_HSize_High] = READ8(&dstHSize, 0);

	/* set gprmdb_vsize. */
	XBYTE[REG_Dram_Gprmdb_VSize_Low] = READ8(&dstVSize, 1);
	XBYTE[REG_Dram_Gprmdb_VSize_High] = READ8(&dstVSize, 0);

	/* set Bgbhoff, must align to 2. */
	XBYTE[REG_Gprm_BgbhoffL] = READ8(&dstHOft, 1);
	XBYTE[REG_Gprm_BgbhoffH] = READ8(&dstHOft, 0);

	/* set Bgbvoff. */
	XBYTE[REG_Gprm_BgbvoffL] = READ8(&dstVOft, 1);
	XBYTE[REG_Gprm_BgbvoffH] = READ8(&dstVOft, 0);

	/* set Bgbwidth. */
	XBYTE[REG_Gprm_BgbwidthL] = READ8(&cpHSize, 1);
	XBYTE[REG_Gprm_BgbwidthH] = READ8(&cpHSize, 0);

	/* set Bgbheight. */
	XBYTE[REG_Gprm_BgbheightL] = READ8(&cpVSize, 1);
	XBYTE[REG_Gprm_BgbheightH] = READ8(&cpVSize, 0);

	/* set pstThd. */
	XBYTE[REG_Gprm_PstThd] = pstThd;

	/* set imgpromode to copy-paste. */
	XBYTE[REG_Gprm_OpMode] = 0x03;
	XBYTE[REG_Gprm_DatFmt_CpEn] = 0x00;

	/* trigger. */
	XBYTE[REG_Gprm_Trg_IntSts] = 0x11;
	EXIT_CRITICAL(sts);

	/* wait operating done. */
	if(waitMode) {
		timeOut = GPRM_ENG_TIME_OUT;
		while(!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)) {
			if (!timeOut--) {
				//ASSERT(0, 1);
				HAL_GprmRst();
				return FAIL;
			}
		}
	}
	
	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmRot(UINT8 mode, UINT32 srcAddrWord, UINT32 dtnAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT8 waitMode)
 * @brief     HAL_GprmRot
 * @param     [in] mode : [0-2] bit0: x flip, bit1: y flip,bit2: xy flip
 * @param     [in] srcAddrWord
 * @param     [in] dtnAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_GprmRot(
	UINT8 mode, 
	UINT32 srcAddrWord, 
	UINT32 dstAddrWord,
	UINT16 srcHSize, 
	UINT16 srcVSize, 
	UINT8 waitMode
)
{
	UINT16 dstHSize = ((mode&0x4)==0x4) ? srcVSize : srcHSize;
	UINT16 dstVSize = ((mode&0x4)==0x4) ? srcHSize : srcVSize;

	XBYTE[REG_Dram_SdckMode] |= 0x20; //Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x0F;

	/* wait GPRM ready */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, srcHSize, srcVSize);
	
	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dstAddrWord, dstHSize, dstVSize);
	
	HAL_GprmParaDstOpOftSet(0, 0);
	HAL_GprmParaDstOpSizeSet(dstHSize, dstVSize);

	/* set imgpromode to rotation. */
	XBYTE[REG_Gprm_OpMode] = 0x02;

	/* set rotmode. */
	XBYTE[REG_Gprm_RotMode] = mode;

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	XBYTE[REG_Dram_SdckMode] &= ~0x20;//Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x09;

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmRot(UINT8 mode, UINT32 srcAddrWord, UINT32 dtnAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT8 waitMode)
 * @brief     HAL_GprmRot
 * @param     [in] mode : [0-2] bit0: x flip, bit1: y flip,bit2: xy flip
 * @param     [in] srcAddrWord
 * @param     [in] dtnAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_GprmRotExt(
	UINT8 mode, 
	UINT32 srcAddrWord, 
	UINT32 dstAddrWord,
	UINT16 srcHSize, 
	UINT16 srcVSize, 
	UINT16 srcHoff,
	UINT16 srcVoff,
	UINT16 destHSize,
	UINT16 destVSize,
	UINT8 waitMode
)
{
	UINT16 dstHSize = ((mode&0x4)==0x4) ? destVSize : destHSize;
	UINT16 dstVSize = ((mode&0x4)==0x4) ? destHSize : destVSize;

	XBYTE[REG_Dram_SdckMode] |= 0x20; //Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x0F;

	/* wait GPRM ready */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, srcHSize, srcVSize);
	HAL_GprmParaSrcDatOftSet(srcHoff, srcVoff);

	HAL_GprmParaDstBuffSet(dstAddrWord, dstHSize, dstVSize);
	
	HAL_GprmParaDstOpOftSet(0, 0);
	HAL_GprmParaDstOpSizeSet(dstHSize, dstVSize);

	/* set imgpromode to rotation. */
	XBYTE[REG_Gprm_OpMode] = 0x02;

	/* set rotmode. */
	XBYTE[REG_Gprm_RotMode] = mode;

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	XBYTE[REG_Dram_SdckMode] &= ~0x20;//Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x09;

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmRot_Intr(UINT8 mode, UINT32 srcAddrWord, UINT32 dtnAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT8 waitMode)
 * @brief     HAL_GprmRot_Intr
 * @param     [in] mode : [0-2] bit0: x flip, bit1: y flip,bit2: xy flip
 * @param     [in] srcAddrWord
 * @param     [in] dtnAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_GprmRot_Intr(
	UINT8 mode, 
	UINT32 srcAddrWord, 
	UINT32 dtnAddrWord,
	UINT16 srcHSize, 
	UINT16 srcVSize, 
	UINT8 waitMode
)
{
	UINT16 dtnhsize = ((mode&0x4)==0x4) ? srcVSize : srcHSize;
	UINT16 dtnvsize = ((mode&0x4)==0x4) ? srcHSize : srcVSize;
	UINT32 timeOut;

	XBYTE[REG_Dram_SdckMode] |= 0x20; //Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x0F;

	/* wait GPRM ready */
	timeOut = GPRM_ENG_TIME_OUT;
	while(!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)) {
		if (!timeOut--) {
			//ASSERT(0, 1);
			HAL_GprmRst();
			return FAIL;
		}
	}

	/* set gprmsb_staddr. */
	XBYTE[REG_Dram_Gprmsb_StAddr_B0] = READ8(&srcAddrWord, 3);
	XBYTE[REG_Dram_Gprmsb_StAddr_B1] = READ8(&srcAddrWord, 2);
	XBYTE[REG_Dram_Gprmsb_StAddr_B2] = READ8(&srcAddrWord, 1);
	XBYTE[REG_Dram_Gprmsb_StAddr_B3] = READ8(&srcAddrWord, 0);

	/* set gprmsb_hsize. */
	XBYTE[REG_Dram_Gprmsb_HSize_Low] = READ8(&srcHSize, 1);
	XBYTE[REG_Dram_Gprmsb_HSize_High] = READ8(&srcHSize, 0);

	/* set gprmsb_vsize. */
	XBYTE[REG_Dram_Gprmsb_VSize_Low] = READ8(&srcVSize, 1);
	XBYTE[REG_Dram_Gprmsb_VSize_High] = READ8(&srcVSize, 0);

	/* set Agbhoff, must align to 2. */
	XBYTE[REG_Gprm_AgbhoffL] = 0;
	XBYTE[REG_Gprm_AgbhoffH] = 0;

	/* set Agbvoff. */
	XBYTE[REG_Gprm_AgbvoffL] = 0;
	XBYTE[REG_Gprm_AgbvoffH] = 0;

	/* set gprmdb_staddr. */
	XBYTE[REG_Dram_Gprmdb_StAddr_B0] = READ8(&dtnAddrWord, 3);
	XBYTE[REG_Dram_Gprmdb_StAddr_B1] = READ8(&dtnAddrWord, 2);
	XBYTE[REG_Dram_Gprmdb_StAddr_B2] = READ8(&dtnAddrWord, 1);
	XBYTE[REG_Dram_Gprmdb_StAddr_B3] = READ8(&dtnAddrWord, 0);

	/* set gprmdb_hsize. */
	XBYTE[REG_Dram_Gprmdb_HSize_Low] = READ8(&dtnhsize, 1);
	XBYTE[REG_Dram_Gprmdb_HSize_High] = READ8(&dtnhsize, 0);

	/* set gprmdb_vsize. */
	XBYTE[REG_Dram_Gprmdb_VSize_Low] = READ8(&dtnvsize, 1);
	XBYTE[REG_Dram_Gprmdb_VSize_High] = READ8(&dtnvsize, 0);

	/* set Bgbhoff, must align to 2. */
	XBYTE[REG_Gprm_BgbhoffL] = 0;
	XBYTE[REG_Gprm_BgbhoffH] = 0;

	/* set Bgbvoff. */
	XBYTE[REG_Gprm_BgbvoffL] = 0;
	XBYTE[REG_Gprm_BgbvoffH] = 0;

	/* set Bgbwidth. */
	XBYTE[REG_Gprm_BgbwidthL] = READ8(&dtnhsize, 1);
	XBYTE[REG_Gprm_BgbwidthH] = READ8(&dtnhsize, 0);

	/* set Bgbheight. */
	XBYTE[REG_Gprm_BgbheightL] = READ8(&dtnvsize, 1);
	XBYTE[REG_Gprm_BgbheightH] = READ8(&dtnvsize, 0);

	/* set imgpromode to rotation. */
	XBYTE[REG_Gprm_OpMode] = 0x02;

	/* set rotmode. */
	XBYTE[REG_Gprm_RotMode] = mode;

	/* trigger. */
	XBYTE[REG_Gprm_Trg_IntSts] = 0x11;

	/* waitMode operating done. */
	if(waitMode) {
		timeOut = GPRM_ENG_TIME_OUT;
		while(!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)) {
			if (!timeOut--) {
				//ASSERT(0, 1);
				HAL_GprmRst();
				return FAIL;
			}
		}
	}

	XBYTE[REG_Dram_SdckMode] &= ~0x20;//Phil add for gprm rotate issue
	//XBYTE[0x2758] = 0x09;

	return SUCCESS;
}


/**
 * @fn        void HAL_GprmScale(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dtnVSize, UINT32 tmpAddrWord, UINT8 waitMode)
 * @brief     HAL_GprmScale
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] dstAddrWord
 * @param     [in] dstHSize
 * @param     [in] dtnVSize
 * @param     [in] tmpAddrWord
 * @param     [in] waitMode
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GprmScale(
	UINT32 srcAddrWord,
	UINT16 srcHSize,
	UINT16 srcVSize,
	UINT32 dstAddrWord,
	UINT16 dstHSize,
	UINT16 dtnVSize,
	UINT32 tmpAddrWord,
	UINT8 waitMode
)
{
	if(dstHSize<srcHSize) {
		if(dtnVSize<srcVSize){
			HAL_GprmScadnh(srcAddrWord, srcHSize, tmpAddrWord, dstHSize, srcVSize, 1);
			HAL_GprmScadnv(tmpAddrWord, srcVSize, dstAddrWord, dtnVSize, dstHSize, waitMode);
		}
		else if(dtnVSize>srcVSize){
			HAL_GprmScadnh(srcAddrWord, srcHSize, tmpAddrWord, dstHSize, srcVSize, 1);
			HAL_GprmScaupv(tmpAddrWord, srcVSize, dstAddrWord, dtnVSize, dstHSize, waitMode);
		}
		else{ //dtnVSize==srcVSize
			HAL_GprmScadnh(srcAddrWord, srcHSize, dstAddrWord, dstHSize, srcVSize, waitMode);
		}
	}
	else if(dstHSize>srcHSize) {
		if(dtnVSize<srcVSize) {
			HAL_GprmScadnv(srcAddrWord, srcVSize, tmpAddrWord, dtnVSize, srcHSize, 1);
			HAL_GprmScauph(tmpAddrWord, srcHSize, dstAddrWord, dstHSize, dtnVSize, waitMode);
		}
		else if(dtnVSize>srcVSize){
			HAL_GprmScaupv(srcAddrWord, srcVSize, tmpAddrWord, dtnVSize, srcHSize, 1);
			HAL_GprmScauph(tmpAddrWord, srcHSize, dstAddrWord, dstHSize, dtnVSize, waitMode);
		}
		else{ //dtnVSize==srcVSize
			HAL_GprmScauph(srcAddrWord, srcHSize, dstAddrWord, dstHSize, srcVSize, waitMode);
		}
	}
	else { //dstHSize==srcHSize
		if(dtnVSize<srcVSize){
			HAL_GprmScadnv(srcAddrWord, srcVSize, dstAddrWord, dtnVSize, srcHSize, waitMode);
		}
		else if(dtnVSize>srcVSize){
			HAL_GprmScaupv(srcAddrWord, srcVSize, dstAddrWord, dtnVSize, srcHSize, waitMode);
		}
		else{ //dtnVSize==srcVSize
			if( srcAddrWord != dstAddrWord ) {
				HAL_GprmDramDma( srcAddrWord << 1, dstAddrWord << 1, (UINT32)srcHSize * srcVSize * 2, waitMode);
			}
		}
	}
}

/**
 * @fn        void HAL_GprmScaleVFactor(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT32 dstAddrWord, UINT16 dtnHSize, UINT16 dtnVSize, UINT32 tmpAddrWord, UINT16 vSF, UINT16 vISF, UINT8 waitMode)
 * @brief     HAL_GprmScaleVFactor
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @param     [in] dstAddrWord
 * @param     [in] dtnHSize
 * @param     [in] dtnVSize
 * @param     [in] tmpAddrWord
 * @param     [in] vSF
 * @param     [in] vISF
 * @param     [in] waitMode
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_GprmScaleVFactor(
	UINT32 srcAddrWord,
	UINT16 srcHSize,
	UINT16 srcVSize,
	UINT32 dstAddrWord,
	UINT16 dtnHSize,
	UINT16 dtnVSize,
	UINT32 tmpAddrWord,
	UINT16 vSF,
	UINT16 vISF,
	UINT8 waitMode
)
{
	HAL_GprmScaupVFactor(srcAddrWord, srcVSize, tmpAddrWord, dtnVSize, srcHSize, vSF, vISF, 1);
	HAL_GprmScauph(tmpAddrWord, srcHSize, dstAddrWord, dtnHSize, dtnVSize, waitMode);
}

/**
 * @fn        UINT8 HAL_GprmScadnh(UINT32 srcAddrWord, UINT16 srcHSize, UINT32 dstAddrWord, UINT16 dtnHSize, UINT16 vSize, UINT8 waitMode)
 * @brief     HAL_GprmScadnh
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] dstAddrWord
 * @param     [in] dtnHSize
 * @param     [in] vSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
HAL_GprmScadnh(
	UINT32 srcAddrWord,
	UINT16 srcHSize,
	UINT32 dstAddrWord,
	UINT16 dtnHSize,
	UINT16 vSize,
	UINT8 waitMode
)
{
	UINT16 scafactor = (UINT16)(((UINT32) ( dtnHSize  ) << 16) / (UINT32) ( srcHSize ));

	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, srcHSize, vSize);

	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dstAddrWord, dtnHSize, vSize);

	HAL_GprmParaDstOpOftSet(0, 0);
	HAL_GprmParaDstOpSizeSet(dtnHSize, vSize);

	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(0, scafactor, scafactor);

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmScadnv(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, UINT16 dstVSize, UINT16 hSize, UINT8 waitMode)
 * @brief     HAL_GprmScadnv
 * @param     [in] srcAddrWord
 * @param     [in] srcVSize
 * @param     [in] dstAddrWord
 * @param     [in] dstVSize
 * @param     [in] hSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
HAL_GprmScadnv(
	UINT32 srcAddrWord,
	UINT16 srcVSize,
	UINT32 dstAddrWord, 
	UINT16 dstVSize, 
	UINT16 hSize, 
	UINT8 waitMode
)
{
	UINT16 scafactor = (UINT16)(((UINT32)( dstVSize )<<16)/(UINT32)( srcVSize ));
	
	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, hSize, srcVSize);
	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dstAddrWord, hSize, dstVSize);

	HAL_GprmParaDstOpOftSet(0, 0);

	HAL_GprmParaDstOpSizeSet(hSize, dstVSize);
	
	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(1, scafactor, scafactor);

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmScauph(UINT32 srcAddrWord, UINT16 srcHSize, UINT32 dtnAddrWord, UINT16 dstHSize, UINT16 vSize, UINT8 waitMode)
 * @brief     HAL_GprmScauph
 * @param     [in] srcAddrWord
 * @param     [in] srcHSize
 * @param     [in] dtnAddrWord
 * @param     [in] dstHSize
 * @param     [in] vSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
HAL_GprmScauph(
	UINT32 srcAddrWord,
	UINT16 srcHSize,
	UINT32 dtnAddrWord, 
	UINT16 dstHSize, 
	UINT16 vSize, 
	UINT8 waitMode
)
{
	UINT16 scafactor = (UINT16)(((UINT32)( srcHSize - 1 )<<16)/(UINT32)( dstHSize - 1 ));
	
	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, srcHSize, vSize);
	
	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dtnAddrWord, dstHSize, vSize);

	HAL_GprmParaDstOpOftSet(0, 0);

	HAL_GprmParaDstOpSizeSet(dstHSize, vSize);

	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(2, scafactor, 0x8000);

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode==1) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_GprmScaupv(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dtnAddrWord, UINT16 dtnvsize, UINT16 hSize, UINT8 waitMode)
 * @brief     HAL_GprmScaupv
 * @param     [in] srcAddrWord
 * @param     [in] srcVSize
 * @param     [in] dtnAddrWord
 * @param     [in] dtnvsize
 * @param     [in] hSize
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 
HAL_GprmScaupv(
	UINT32 srcAddrWord, 
	UINT16 srcVSize,
	UINT32 dtnAddrWord, 
	UINT16 dtnvsize, 
	UINT16 hSize, 
	UINT8 waitMode
)
{
	UINT16 scafactor = (UINT16)(((UINT32)( srcVSize - 1 )<<16)/(UINT32)( dtnvsize - 1 ));
	
	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, hSize, srcVSize);
	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dtnAddrWord, hSize, dtnvsize);

	HAL_GprmParaDstOpOftSet(0, 0);
	HAL_GprmParaDstOpSizeSet(hSize, dtnvsize);

	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(3, scafactor, 0x8000);

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

#if 1
/**
 * @fn        UINT8 HAL_GprmScadnVFactor(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, UINT16 dtnVSize, UINT16 hSize, UINT16 vSF, UINT16 vISF, UINT8 waitMode)
 * @brief     HAL_GprmScadnVFactor
 * @param     [in] srcAddrWord
 * @param     [in] srcVSize
 * @param     [in] dstAddrWord
 * @param     [in] dtnVSize
 * @param     [in] hSize
 * @param     [in] vSF
 * @param     [in] vISF
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 HAL_GprmScadnVFactor(
	UINT32 srcAddrWord,
	UINT16 srcVSize,
	UINT32 dstAddrWord,
	UINT16 dtnVSize,
	UINT16 hSize,
	UINT16 vSF,
	UINT16 vISF,
	UINT8 waitMode
)
{
	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}
	
	HAL_GprmParaSrcBuffSet(srcAddrWord, hSize, srcVSize);

	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dstAddrWord, hSize, dtnVSize);

	HAL_GprmParaDstOpOftSet(0, 0);
	HAL_GprmParaDstOpSizeSet(hSize, dtnVSize);

	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(1, vSF, vISF);

	/* trigger. */
	HAL_GprmTrig();

	/* waitMode operating done. */
	if(waitMode==1) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}
#endif

/**
 * @fn        UINT8 HAL_GprmScaupVFactor(UINT32 srcAddrWord, UINT16 srcVSize, UINT32 dstAddrWord, UINT16 dtnVSize, UINT16 hSize, UINT16 vSF, UINT16 vISF, UINT8 waitMode)
 * @brief     HAL_GprmScaupVFactor
 * @param     [in] srcAddrWord
 * @param     [in] srcVSize
 * @param     [in] dstAddrWord
 * @param     [in] dtnVSize
 * @param     [in] hSize
 * @param     [in] vSF
 * @param     [in] vISF
 * @param     [in] waitMode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-26
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
HAL_GprmScaupVFactor(
	UINT32 srcAddrWord, 
	UINT16 srcVSize,
	UINT32 dstAddrWord, 
	UINT16 dtnVSize, 
	UINT16 hSize, 
	UINT16 vSF, 
	UINT16 vISF, 
	UINT8 waitMode
)
{
	/* waitMode previous operating done. */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		return FAIL;
	}

	HAL_GprmParaSrcBuffSet(srcAddrWord, hSize, srcVSize);
	
	HAL_GprmParaSrcDatOftSet(0, 0);

	HAL_GprmParaDstBuffSet(dstAddrWord, hSize, dtnVSize);

	HAL_GprmParaDstOpOftSet(0, 0);

	HAL_GprmParaDstOpSizeSet(hSize, dtnVSize);

	/* set imgpromode to scaling. */
	XBYTE[REG_Gprm_OpMode] = 0x01;

	HAL_GprmParaSclSet(3, vSF, vISF);

	/* trigger. */
	HAL_GprmTrig();

//	printf( "f %u, 0x2723 [%bx][%bx][%bx][%bx]\n", vSF, XBYTE[REG_Gprm_SclFctL], XBYTE[REG_Gprm_SclFctH], XBYTE[REG_Gprm_SclFctIniL], XBYTE[REG_Gprm_SclFctIniH] );

	/* waitMode operating done. */
	if(waitMode==1) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_DRAMbpcfg
//-----------------------------------------------------------------------------
/**
 * @brief	Bad pixel configuration
 * @param	bufaddr : image address in word
 * @param	hsize 	: the size of image horizontal direction
 * @param	vsize	: the size of image vertical direction
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void 
HAL_DRAMbpcfg(
	UINT32 bufAddrWord, 
	UINT16 hSize, 
	UINT16 vSize
)
{
	XBYTE[REG_Dram_Gprmsb_StAddr_B0] = READ8(bufAddrWord,3); //GPRMSB addr
	XBYTE[REG_Dram_Gprmsb_StAddr_B1] = READ8(bufAddrWord,2);
	XBYTE[REG_Dram_Gprmsb_StAddr_B2] = READ8(bufAddrWord,1);
	XBYTE[REG_Dram_Gprmsb_StAddr_B3] = READ8(bufAddrWord,0);
	XBYTE[REG_Dram_Gprmsb_HSize_Low] = READ8(hSize,1);
	XBYTE[REG_Dram_Gprmsb_HSize_High] = READ8(hSize,0);
	XBYTE[REG_Dram_Gprmsb_VSize_Low] = READ8(vSize,1);
	XBYTE[REG_Dram_Gprmsb_VSize_High] = READ8(vSize,0);
	XBYTE[REG_Gprm_OpMode] = 0x05;       //imgpromode
}

#if TAG_TODO
//-----------------------------------------------------------------------------
//HAL_DRAMbpfix
//-----------------------------------------------------------------------------
/**
 * @brief	Bad pixel fix
 * @param	badpixthd :Bad pixel correction threshold
 * @param	bphoft 	: Bad pixel graphics buffer hoffset
 * @param	bpvoft	: Bad pixel graphics buffer voffset
 * @param	wait	: 1  if GPRM busy,then wait, 0 normal
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void 
HAL_DRAMbpfix(
	UINT16 badpixthd, 
	UINT16 bphoft, 
	UINT16 bpvoft, 
	UINT8 wait
)
{
	badpixthd = badpixthd;
	bphoft = bphoft;
	bpvoft = bpvoft;
	wait = wait;
}
#endif

/**
 * @fn        void HAL_GprmOsdIdxSet(UINT8 idx0, UINT8 idx1, UINT8 idx2, UINT8 idx3)
 * @brief     HAL_GprmOsdIdxSet
 * @param     [in] idx0
 * @param     [in] idx1
 * @param     [in] idx2
 * @param     [in] idx3
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-31
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GprmOsdIdxSet(
	UINT8 idx0,
	UINT8 idx1,
	UINT8 idx2,
	UINT8 idx3
)
{
    XBYTE[REG_Gprm_OsdIdx0] = idx0;
    XBYTE[REG_Gprm_OsdIdx1] = idx1;
    XBYTE[REG_Gprm_OsdIdx2] = idx2;
    XBYTE[REG_Gprm_OsdIdx3] = idx3;	
}



#pragma disable //disable interrupt
void
HAL_GprmOsdIdxGet(
	UINT8* idx0,
	UINT8* idx1,
	UINT8* idx2,
	UINT8* idx3
)
{
    *idx0 = XBYTE[REG_Gprm_OsdIdx0];
    *idx1 = XBYTE[REG_Gprm_OsdIdx1];
    *idx2 = XBYTE[REG_Gprm_OsdIdx2];
    *idx3=  XBYTE[REG_Gprm_OsdIdx3];	
}





/**
 * @fn        void HAL_GprmOsdCpy(UINT32 srcaddr, UINT16 srchsize, UINT16 srcvsize, UINT16 srchoft, UINT16 srcvoft, UINT32 dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT16 dtnhoft, UINT16 dtnvoft, UINT16 cphsize, UINT16 cpvsize, UINT8 bTrsfm, UINT8 bBld, UINT8 bldColor, UINT8 wait)
 * @brief     HAL_GprmOsdCpy
 * @param     [in] srcaddr
 * @param     [in] srchsize
 * @param     [in] srcvsize
 * @param     [in] srchoft
 * @param     [in] srcvoft
 * @param     [in] dtnaddr
 * @param     [in] dtnhsize
 * @param     [in] dtnvsize
 * @param     [in] dtnhoft
 * @param     [in] dtnvoft
 * @param     [in] cphsize
 * @param     [in] cpvsize
 * @param     [in] bTrsfm
 * @param     [in] bBld
 * @param     [in] bldColor
 * @param     [in] wait
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-31
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void HAL_GprmOsdCpy(UINT32 srcaddr, UINT16 srchsize, UINT16 srcvsize, UINT16 srchoft, UINT16 srcvoft,
    UINT32 dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT16 dtnhoft, UINT16 dtnvoft,
    UINT16 cphsize, UINT16 cpvsize, UINT8 bTrsfm, UINT8 bBld, UINT8 bldColor, UINT8 wait)
{
    UINT32 sts;

	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
	}

    ENTER_CRITICAL(sts);

	HAL_GprmParaSrcBuffSet(srcaddr, srchsize, srcvsize);

	HAL_GprmParaSrcDatOftSet(srchoft, srcvoft);

	HAL_GprmParaDstBuffSet(dtnaddr, dtnhsize, dtnvsize);
	
	HAL_GprmParaDstOpOftSet(dtnhoft, dtnvoft);

	HAL_GprmParaDstOpSizeSet(cphsize, cpvsize);
	XBYTE[REG_Gprm_SclMode] = 0;
    if (bTrsfm) {
        /* set imgpromode to 2bits to 8bits transformation. */
        XBYTE[REG_Gprm_OpMode] = 0x06;
    } else {
        /* set imgpromode to copy-paste. */
        XBYTE[REG_Gprm_OpMode] = 0x03;    
        /* set data format to OSD */
        XBYTE[REG_Gprm_DatFmt_CpEn] = 0x01;
    }

    if (bBld) {
        /* enable osd color key copy paste function */
        XBYTE[REG_Gprm_OsdKeyEn] = 0x01;
        /* set osd color key value */
        XBYTE[REG_Gprm_OsdKeyVal] = bldColor;
    }

	/* trigger. */
	HAL_GprmTrig();
    
	EXIT_CRITICAL(sts);

	/* wait operating done. */
	if(wait==1) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
		}
	}    

    /* restore data format to YUV */
    XBYTE[REG_Gprm_DatFmt_CpEn] = 0x00; 
    /* disable osd color key copy paste function */
    XBYTE[REG_Gprm_OsdKeyEn] = 0x00;
}

#if 0
#endif

/**
 * @fn        void HAL_GprmPenDrawCfg(UINT8 datFmt, UINT8 drawDat0, UINT8 drawDat1, UINT8 drawDat2, UINT32 buffAddrWord, UINT16 buffW, UINT16 buffH)
 * @brief     HAL_GprmPenDrawCfg
 * @param     [in] datFmt
 * @param     [in] drawDat0
 * @param     [in] drawDat1
 * @param     [in] drawDat2
 * @param     [in] buffAddrWord
 * @param     [in] buffW
 * @param     [in] buffH
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GprmPenDrawCfg(
	UINT8 datFmt,
	UINT8 drawDat0,
	UINT8 drawDat1,
	UINT8 drawDat2,
	UINT32 buffAddrWord,
	UINT16 buffW,
	UINT16 buffH	
)
{
	UINT8 datType;
	
	if (datFmt) {
		datType = 0x03;
	} else {
		datType = 0x02;
	}
	
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
	}

	HAL_GprmParaDstBuffSet(buffAddrWord, buffW, buffH);

	/* set imgpromode to copy-paste. */
	XBYTE[REG_Gprm_OpMode] = 0x03;	 
	/* set data format to OSD */
	XBYTE[REG_Gprm_DatFmt_CpEn] = datType; //bit1: skip to read source data in dram
	/* pen color */
	XBYTE[REG_Gprm_OsdIdx0] = drawDat0;
	XBYTE[REG_Gprm_OsdIdx1] = drawDat1;
	XBYTE[REG_Gprm_OsdIdx2] = drawDat2;
}

/**
 * @fn        UINT8 HAL_GprmPenRectDraw(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT8 wait)
 * @brief     HAL_GprmPenRectDraw
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @param     [in] wait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 
HAL_GprmPenRectDraw(
	UINT16 x,
	UINT16 y,
	UINT16 w,
	UINT16 h,
	UINT8 wait
)
{
	/* wait previous operation done */
	__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
	if (__ret == -1) {
		HAL_GprmRst();
	}

	/* set rectangle left-top offset */
	HAL_GprmParaDstOpOftSet(x, y);

	/* set rectangle width & height */
	HAL_GprmParaDstOpSizeSet(w, h);

	/* trigger. */
	HAL_GprmTrig();

	/* wait operating done. */
	if(wait) {
		__while((!(XBYTE[REG_Gprm_Trg_IntSts] & 0x1)), __ret);
		if (__ret == -1) {
			HAL_GprmRst();
			return FAIL;
		}
	}

	return SUCCESS;
}

