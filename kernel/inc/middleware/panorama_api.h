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
 * @file		panorama_api.h
 * @brief		panorama api module header file.
 * @author		LinJieCheng
 * @since		2013-02-19
 * @date			2013-02-19
 */
#ifndef __PANORAMA_API_H__
#define __PANORAMA_API_H__
#include "knl_func_opt.h"
#include "panorama_def.h"

//#include "sp1k_panorama_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct _PanoramaParamInf{
	UINT8 direction;
	UINT8 scaleRatio;
	UINT16 stitchPixel;
} PanoramaParamInf_t;


/* Panorama Frame Temp Buffer Information */
typedef struct _PanoramaFrameTempBufInf{
	UINT32 startBufAddr;
	UINT16 hSize;
	UINT16 vSize;
	UINT16 totalNum;
	UINT32 cdsp1stBufAddr[4];
	UINT32 cdsp1stOneWSize;
} PanoramaFrameTempBufInf_t;

/* Panorama Me Temp Buffer Information */
typedef struct _PanoramaMeTempBufInf{
	UINT32 startBufAddr;
	UINT8 hBlockNum;
	UINT8 vBlockNum;
} PanoramaMeTempBufInf_t;

/* Panorama Dst Buffer Information */
typedef struct _PanoramaDstBufInf{
	UINT32 bufAddr;
	UINT16 hSize;
	UINT16 vSize;
} PanoramaDstBufInf_t;

/* Panorama Final Information */
typedef struct _PanoramaFinalInf{
	UINT16 hSize;
	UINT16 vSize;
} PanoramaFinalInf_t;

/* Panorama GMV By Frame Information */
typedef struct _PanoramaGMVByFrameInf{
	SINT16 x;
	SINT16 y;
} PanoramaGMVByFrameInf_t;

/* Panorama Sum Of GMV Information */
typedef struct _PanoramaSumOfGMVInf{
	SINT16 x;
	SINT16 y;
} PanoramaSumOfGMVInf_t;

/* Panorama Information */
typedef struct _PanoramaInf{
	PanoramaParamInf_t param;
	PanoramaFrameTempBufInf_t frameTmp;
	PanoramaMeTempBufInf_t meTmp;
	PanoramaDstBufInf_t dst;
	PanoramaFinalInf_t final;
	PanoramaGMVByFrameInf_t gmv;
	PanoramaSumOfGMVInf_t sogmv;
} PanoramaInf_t;








/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT8 INTR_panoramaBufSwitch(void);
UINT8 panoramaMeDoneGet(void);
UINT8 panoramaEnGet(void);
void panoramaEnSet(UINT8 en);
UINT8 panoramaParamSet(UINT8 selector,UINT16 val);
void panoramaInit(void);
UINT8 panoramaGMVByFrameFunc(SINT16 *gmvXByFrame,SINT16 *gmvYByFrame,UINT8 *status);
UINT8 panoramaSumOfGMVFunc(SINT16 *sumOfGmvX,SINT16 *sumOfGmvY);
UINT8 panoramaBufSwitchSet(void);
UINT8 panoramaDstFileSave(UINT8 *name);
UINT8 panoramaGprmStitchFunc (void);
void 
Panorama_LR(
	UINT32 srcaddr,//word addr (G_PanoramaInf.frameTmp.startBufAddr)
	UINT32 dstAddr,//word addr (G_PanoramaInf.dst.bufAddr)
	UINT32 sh_idx,// 3 (G_PanoramaInf.frameTmp.totalNum)
	UINT32 width,//640 (G_PanoramaInf.frameTmp.hSize)
	UINT32 height,//480 (G_PanoramaInf.frameTmp.vSize)
	UINT32 outW,//3344 (panoDstWidthGet)
	UINT32 outH,//640 (panoDstHeightGet)
	UINT32 xrange,//128 (panoRangeGet)
	UINT32 extend,//0 (EXTEND)
	UINT32 xoff,//0 (X_OFFSET)
	UINT32 direction,// 1
	UINT32 widthall,//3008 (panoFinalWidthGet)
	UINT32 shotsummvAddr 
);

void
Panorama_LR_Ori(
		UINT32 srcaddr,//word addr (G_PanoramaInf.frameTmp.startBufAddr)
		UINT32 dstAddr,//word addr (G_PanoramaInf.dst.bufAddr)
		UINT32 sh_idx,// 3 (G_PanoramaInf.frameTmp.totalNum)
		UINT32 width,//640 (G_PanoramaInf.frameTmp.hSize)
		UINT32 height,//480 (G_PanoramaInf.frameTmp.vSize)
		UINT32 outW,//3344 (panoDstWidthGet)
		UINT32 outH,//640 (panoDstHeightGet)
		UINT32 xrange,//128 (panoRangeGet)
		UINT32 extend,//0 (EXTEND)
		UINT32 xoff,//0 (X_OFFSET)
		UINT32 direction,// 1
		UINT32 widthall,//3008 (panoFinalWidthGet)
		UINT32 shotsummvAddr 
);

UINT32 panoDstWidthGet(UINT8 direction);
UINT32 panoDstHeightGet(UINT8 direction);
void panoFinalSizeSet(UINT32 w,	UINT32 h);
UINT32 panoFinalWidthGet(UINT8 direction);
UINT32 panoFinalHeightGet(UINT8 direction);
UINT32 panoRangeGet(UINT8 direction);










#endif  /* __PANORAMA_API_H__ */

