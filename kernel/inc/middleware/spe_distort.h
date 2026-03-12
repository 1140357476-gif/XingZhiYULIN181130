/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  spe_distort.h
 * @brief	  special effect top function
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/

#ifndef __SPE_DISTORT_H__
#define __SPE_DISTORT_H__

#define SPE_DISTORT_BUBBLE_RADIUS_MAX 240
#define SPE_DISTORT_V_SIZE_MAX 960

#define speParamGet(idx) g_speParams[idx]
#define speParamSet(idx,value) g_speParams[idx]=value

#define speParamSetCropping(hOff,vOff,hSize,vSize)	speParamSet(SPE_PARAM_H_SIZE,hSize);\
													speParamSet(SPE_PARAM_V_SIZE,vSize);\
													speParamSet(SPE_PARAM_H_OFF,hOff);\
													speParamSet(SPE_PARAM_V_OFF,vOff)
													
#define speParamSetDstSize(hSizeDst,vSizeDst)		speParamSet(SPE_PARAM_H_SIZE_DST,hSizeDst);\
													speParamSet(SPE_PARAM_V_SIZE_DST,vSizeDst)
													
#define speParamSetMirrorIdx(idx)					speParamSet(SPE_PARAM_MIRROR_INDEX,idx)

//#define IMG_THUMB_WIDTH 160
//#define IMG_THUMB_HEIGHT 120
typedef enum{
	SPE_DISTORT_H_= 0,
	SPE_DISTORT_H_PINCH,//1
	SPE_DISTORT_H_PUNCH,

	SPE_DISTORT_H_STRETCH,
	SPE_DISTORT_H_SHRINK,

	SPE_DISTORT_H_HOURGLASS,

	SPE_DISTORT_V_,
	SPE_DISTORT_V_PINCH,
	SPE_DISTORT_V_PUNCH,

	SPE_DISTORT_V_STRETCH,
	SPE_DISTORT_V_SHRINK,

	SPE_DISTORT_V_HOURGLASS,

	SPE_DISTORT_M_,

	SPE_DISTORT_WAVY,
	SPE_DISTORT_BUBBLE,
	SPE_DISTORT_BUBBLE_FILL,

	SPE_DISTORT_NO_LOOP,
	SPE_DISTORT_H_MIRROR,
	SPE_DISTORT_V_MIRROR,

	SPE_DISTORT_MAX
}speDistortID_t;

typedef enum{
	SPE_PARAM_H_SIZE=0,
	SPE_PARAM_V_SIZE,
	SPE_PARAM_H_OFF,
	SPE_PARAM_V_OFF,

	SPE_PARAM_H_SIZE_DST,
	SPE_PARAM_V_SIZE_DST,

	SPE_PARAM_MIRROR_INDEX,
	
	SPE_PARAM_MAX,
}speParamID_t;
extern UINT16 g_speParams[SPE_PARAM_MAX];

UINT8 speDistort(
	UINT8 id, 
	UINT32 srcAddr, 
	UINT16 srcHsize, 
	UINT16 srcVsize, 
	UINT16 hOffset, 
	UINT16 vOffset, 
	UINT16 hSize, 
	UINT16 vSize, 
	UINT32 tmpAddr
	);
UINT8 speDistortView(UINT8 speDisIdx );
UINT8 speDistortSave(UINT16 fileIndex, UINT8 frameResIdx );
void speBuffSizeSet(UINT32 size);
UINT32 speBuffSizeGet(void);

#endif


