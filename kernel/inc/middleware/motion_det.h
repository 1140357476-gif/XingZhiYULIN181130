/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
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
 * @file		motion_det.h
 * @brief		motion detection module header file.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-03-14
 * @date		2009-02-20
 */
#ifndef __MOTION_DET_H__
#define __MOTION_DET_H__
#include "knl_func_opt.h"
#include "sp1k_md_api.h"

#if FUNC_KNL_MD
#define MOTION_DET_EN	1
#define MOTION_DET_YUV_SENSOR_EN	II_AE_MOTION_DET_EN
#else
#define MOTION_DET_EN	0
#endif
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

typedef enum mdFuncStatus_e {
	MOTION_DETECTION = 0,
	MOTION_PANORAMA,	
} mdFuncStatus_t;


typedef enum mdActionStatus_e {
	MD_NOT_ACTION = 0,
	MD_ACTION,
	MD_LEARNING,
} mdActionStatus_t;


/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
#define EDGE_SRC_YUV_H	320
#define EDGE_SRC_YUV_V	240


/* edge window */
#define EDGE_TBL_HBLOCKNUM    8
#define EDGE_TBL_VBLOCKNUM    6

#define EDGE_WIN_HOFFSET    16
#define EDGE_WIN_VOFFSET    16

#define EDGE_WIN_H_THR	32
#define EDGE_WIN_V_THR	32

/* me */
#define ME_H_PIXEL_LEFT		(0)
#define ME_H_PIXEL_NONE		(EDGE_WIN_HOFFSET)
#define ME_H_PIXEL_RIGHT	(ME_H_PIXEL_NONE+EDGE_WIN_HOFFSET)

#define ME_V_PIXEL_LEFT		(0)
#define ME_V_PIXEL_NONE		(EDGE_WIN_VOFFSET)
#define ME_V_PIXEL_RIGHT	(ME_V_PIXEL_NONE+EDGE_WIN_VOFFSET)

#define ME_H_HALF_PIXEL_LEFT	(ME_H_PIXEL_LEFT<<1)
#define ME_H_HALF_PIXEL_NONE	(ME_H_PIXEL_NONE<<1)
#define ME_H_HALF_PIXEL_RIGHT	(ME_H_PIXEL_RIGHT<<1)

#define ME_V_HALF_PIXEL_LEFT	(ME_V_PIXEL_LEFT<<1)
#define ME_V_HALF_PIXEL_NONE	(ME_V_PIXEL_NONE<<1)
#define ME_V_HALF_PIXEL_RIGHT	(ME_V_PIXEL_RIGHT<<1)

/* md */	
#define MD_MAX_MV_NUM	(UINT8)70 
	
#define MD_LEARNING_NUM	(UINT8)8
#define MD_DELAY_CNT    (UINT8)8
	
#define MD_MIN_DETECT_THR (UINT8)16 


/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
	

void mdInit(void);
void mdEnSet(UINT8 en);
UINT8 mdEnGet(void);

void mdSenSet(UINT8 sen);
UINT8  mdSenGet(void);

void mdLearningStart(UINT8 start);


UINT8 mdEdgeTblFill(UINT16 hSize,UINT16 vSize,UINT16 hOffset,UINT16 vOffset,UINT8 hNum,UINT8 vNum); 
void mdSvcProc(void *cnt);
void mdMotionVectorGet(UINT8 num, UINT8 blkNumThr, UINT8 *mvX, UINT8 *mvY,	UINT8 *mvXNum,UINT8 *mvYNum);
void mdFindMaxMv(UINT8 *mvPt0, UINT8 blockNum, UINT8 thr, UINT8* maxMv, UINT8 *maxMvNum);
void mdMeOutAddrGet(UINT8 mode,UINT32 *meAddr);
void mdEdgeTblAddrGet(UINT8 mode,UINT32 *edgeWinAddrA,UINT32 *edgeWinAddrB);
void mdAdjustTempoalDenoiseWeightEnSet(UINT8 en);
void mdAdjustTempoalDenoiseWeightAlgo(UINT8* phistXArray,UINT8* phistYArray,UINT8 mdActionSts,UINT8 mvXMidCntNum,UINT8 mvYMidCntNum);
MD_BAR* mdBarGet (void);


#endif  /* __MOTION_DET_H__ */

