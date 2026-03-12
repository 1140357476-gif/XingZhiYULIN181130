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
 * @file      hal_gprm.h
 * @brief     hal gprm header file
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_GPRM_H__
#define __HAL_GPRM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/	
#define GPRM_DMA_TIME_OUT   10000 //10000ms
#define GPRM_ENG_TIME_OUT   10000 //10000ms
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
typedef enum {
	HAL_GPRM_BUSY_RET  = 0,
	HAL_GPRM_NO_WAIT   = 1,
	HAL_GPRM_WAIT_BUSY = 2,
} HAL_GprmWaitMode_e;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 HAL_GprmDramDma(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode);
UINT8 HAL_GprmDramDmaAdv(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 wait);
UINT8 HAL_GprmDramDmaExt(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode, UINT8 dmaChn);
UINT8 HAL_GprmDramDmaExt_Intr(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 waitMode, UINT8 dmaChn);
UINT8 HAL_GprmDramFill(UINT32 dstAddrWord, UINT32 sizeByte, UINT8 fillData);
UINT8 HAL_GprmDramFillExt(UINT32 dstAddrByte, UINT32 sizeByte, UINT8 fillData, UINT8 waitMode, UINT8 dmaChn);
UINT8 HAL_GprmCopy(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT16 srcHOft, UINT16 srcVOft, 
UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize, UINT16 dstHOft, UINT16 dstVOft, 
UINT16 cpHSize, UINT16 cpVSize, UINT16 pstThd, UINT8 waitMode);
UINT8 HAL_GprmCopy_Intr(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT16 srcHOft, UINT16 srcVOft, 
UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize, UINT16 dstHOft, UINT16 dstVOft, 
UINT16 cpHSize, UINT16 cpVSize, UINT16 pstThd, UINT8 waitMode);

void HAL_GprmScale(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT32 dstAddrWord, 
UINT16 dstHSize, UINT16 dtnVSize, UINT32 tmpAddrWord, UINT8 waitMode);
void HAL_GprmScaleVFactor(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT32 dstAddrWord, 
UINT16 dtnHSize, UINT16 dtnVSize, UINT32 tmpAddrWord, UINT16 vSF, UINT16 vISF, UINT8 waitMode);
UINT8 HAL_GprmRot(UINT8 mode, UINT32 srcAddrWord, UINT32 dtnAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT8 waitMode);
UINT8 HAL_GprmRot_Intr(UINT8 mode, UINT32 srcAddrWord, UINT32 dtnAddrWord, UINT16 srcHSize, UINT16 srcVSize, UINT8 waitMode);

void HAL_DRAMbpcfg(UINT32 bufAddrWord, UINT16 hSize, UINT16 vSize);
void HAL_DRAMbpfix(UINT16 badpixthd, UINT16 bphoft, UINT16 bpvoft, UINT8 wait);

void HAL_GprmParaSrcBuffSet(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize);
void HAL_GprmParaSrcDatOftSet(UINT16 srcHOft, UINT16 srcVOft);
void HAL_GprmParaDstBuffSet(UINT32 dstAddrWord, UINT16 dstHSize, UINT16 dstVSize);
void HAL_GprmParaDstOpOftSet(UINT16 opHOft, UINT16 opVOft);
void HAL_GprmParaDstOpSizeSet(UINT16 opHSize, UINT16 opVSize);
void HAL_GprmParaSclSet(UINT8 sclMode, UINT16 sclFct, UINT16 sclFctIni);

void HAL_GprmPenDrawCfg(UINT8 datFmt, UINT8 drawDat0, UINT8 drawDat1, UINT8 drawDat2, UINT32 buffAddrWord, UINT16 buffW, UINT16 buffH);
UINT8 HAL_GprmPenRectDraw(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT8 wait);
void HAL_GprmOsdIdxSet(UINT8 idx0, UINT8 idx1, UINT8 idx2, UINT8 idx3);
void HAL_GprmOsdIdxGet(UINT8* idx0, UINT8* idx1, UINT8* idx2, UINT8 *idx3);

void HAL_GprmOsdCpy(UINT32 srcaddr, UINT16 srchsize, UINT16 srcvsize, UINT16 srchoft, UINT16 srcvoft,
    UINT32 dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT16 dtnhoft, UINT16 dtnvoft,
    UINT16 cphsize, UINT16 cpvsize, UINT8 bTrsfm, UINT8 bBld, UINT8 bldColor, UINT8 wait);

UINT8 HAL_GprmRotExt(UINT8 mode, UINT32 srcAddrWord, UINT32 dstAddrWord,UINT16 srcHSize, UINT16 srcVSize, UINT16 srcHoff,UINT16 srcVoff,UINT16 dstHSize,UINT16 dstVSize,UINT8 waitMode);

#endif  /* __HAL_GPRM_H__ */

