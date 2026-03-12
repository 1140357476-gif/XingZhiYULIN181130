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
 * @file      hal_front.h
 * @brief     hal front header file
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_FRONT_H__
#define __HAL_FRONT_H__


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	HAL_FRONT_PARA_MCLK_CFG,
	HAL_FRONT_PARA_PCLK_CFG,
	HAL_FRONT_PARA_SYNC_INVERT_ENABLE,
	HAL_FRONT_PARA_BYPASS_ENABLE,
	HAL_FRONT_PARA_RESHAPE_ENABLE,
	HAL_FRONT_PARA_RESHAPE_REGION,
	HAL_FRONT_PARA_CROP_REGION,
	HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE,
	HAL_FRONT_PARA_TG_TOTAL_REGION,
	HAL_FRONT_PARA_TG_SYNC_GENERATOR_ENABLE,
	HAL_FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG,
	HAL_FRONT_PARA_YUV_CCIR_MODE,
	HAL_FRONT_PARA_YUV_SUB128_ENABLE,
	HAL_FRONT_PARA_YUV_INPUT_ENABLE,
	HAL_FRONT_PARA_MAX_ID,
} HAL_FrontParaId_e;

typedef enum {
	HAL_FRONT_YUV_OUTPUT_UYVY = (0 << 0),
	HAL_FRONT_YUV_OUTPUT_YVYU = (1 << 0),
	HAL_FRONT_YUV_OUTPUT_VYUY = (2 << 0),
	HAL_FRONT_YUV_OUTPUT_YUYV = (3 << 0),
} HAL_FrontYuvOutputSeqCfg_e;

typedef enum {
	HAL_FRONT_YUV_CCIR601 = (0 << 4),
	HAL_FRONT_YUV_CCIR656 = (1 << 4),
} HAL_FrontYuvCcirMode_e;

typedef enum {
	HAL_FRONT_MCLK_SRC_INT = (0 << 2),
	HAL_FRONT_MCLK_SRC_EXT = (1 << 2),
} HAL_FrontMclkSrc_e;

typedef enum {
	HAL_FRONT_PCLK_SRC_INT = (0 << 3),
	HAL_FRONT_PCLK_SRC_EXT = (1 << 3),
} HAL_FrontPclkSrc_e;

typedef enum {
	HAL_FRONT_WAIT_HSYNC  = (1 << 0),
	HAL_FRONT_WAIT_VSYNC  = (1 << 1),
	HAL_FRONT_WAIT_HVALID = (1 << 2),
	HAL_FRONT_WAIT_VVALID = (1 << 3),
} HAL_FrontWaitSource_e;

typedef enum {
	HAL_FRONT_WAIT_FALLING = 0,
	HAL_FRONT_WAIT_RISING  = 1,
	HAL_FRONT_WAIT_HIGH    = 2,
	HAL_FRONT_WAIT_LOW     = 3,
} HAL_FrontWaitMode_e; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void HAL_FrontIntrEnable(UINT8 enable, UINT8 value);
void HAL_FrontReset(void);
UINT8 HAL_FrontSnapTrig(UINT8 snapNum,UINT8 trig);
UINT8 HAL_FrontSignalWait(UINT8 src, UINT8 mode, UINT8 num);


void HAL_FrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd);
void HAL_IntrFrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd);

void HAL_FrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd);
void HAL_IntrFrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd);

void HAL_FrontParaSyncInvertEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaByPassEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaReshapeEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaReshapeRegionSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaCropRegionSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontSizeGet(UINT16 *hSize,UINT16 *vSize);
void HAL_FrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_IntrFrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry,UINT8 upd);
void HAL_FrontParaTgTotalRegionSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaTgSyncGeneratorEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaYuvOutputSequenceCfgSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaYuvCcirModeSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaYuvSub128EnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaYuvInputEnableSet(SINT16 *paraAry, UINT8 upd);
void HAL_FrontParaMClkCfgGet(UINT8 *pSrc, UINT16 *pDiv, UINT8 *pPhase, UINT8 *pDrv, UINT8 *pUpd);
void HAL_FrontParaPClkCfgGet(UINT8 *psrc, UINT16 *pdiv, UINT8 *pphase);



void sp1kHalFrontInputGate( UINT8 tggz );

#endif  /* __HAL_FRONT_H__ */

