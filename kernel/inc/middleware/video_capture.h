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
 * @file	  video_capture.h
 * @brief	  video capture funtion 
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_CAP_H__
#define __Video_CAP_H__

#include "video_capdef.h"

typedef enum VDCapParam_e {
	VD_CAP_COUNT,
	VD_CAP_SIZE,
	VD_CAP_MOD,
	VD_CAP_PARAM_MAX,
} VDCapParam_t;

typedef enum VDCapSize_e {
	VD_CAP_VGA,
	VD_CAP_D1,
	VD_CAP_HD,
	VD_CAP_SIZE_MAX,
} VDCapSize_t;

typedef enum {
	SPEEDCAP_STILL = 1,
	SPEEDCAP_CLIP,
	SPEEDCAP_LIGHT_OPEN,
	SPEEDCAP_LIGHT_CLOSE,
}VDCapCallBackId_e;

typedef enum{
	SPEEDCAP_STS_IDLE,
	SPEEDCAP_STS_RUNNING,
}VDCapSts_e;

typedef struct speedyCaptureCbInterface{
		 UINT8 (*pfnStill)(void);
		 UINT8 (*pfnClip)(void);
		 UINT8 (*pfnLightOpen)(void);
		 UINT8 (*pfnLightClose)(void);
}speedyCaptureCbInterface_t;

typedef struct appVidCapCtrl_s {
	UINT16 width;
	UINT16 height; 
	UINT8 dateStampEn;
	UINT8 capNum;
	UINT8 tmrCount;
	UINT8 interCnt;
	UINT8 sts;
	UINT32 vlcBufSize;
	vlcBufList_t vlcBufQue;
	vlcBufList_t writeBufQue;
	speedyCaptureCbInterface_t cbFun;
}appVidCapCtrl_t; 

void speedyCaptureStart();
UINT8 speedyCaptureStop();
UINT8 speedyCapture (void);
UINT8 speedyCaptureRec (void);
void speedyCaptureIntr();
UINT8 speedyCaptureParamSet(UINT8 paraId, UINT16 value);
UINT8 speedyCaptureParamGet(UINT8 paraId);
UINT8 speedyCaptureStsGet();
void speedyCaptureDstEn(UINT8 en);
void speedyCaptureCallbackReg(VDCapCallBackId_e id,void *pfunc);
#endif

