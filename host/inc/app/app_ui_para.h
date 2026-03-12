/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus mMedia
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Technology Co., Ltd.
 *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *
 **************************************************************************/

/**
 * @file		app_ui_para.h
 * @brief		App for UI setting parameter definition
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __APP_UI_PARA_H__
#define __APP_UI_PARA_H__
 //=============================================================================
//Header file
//=============================================================================
#include "app_menu_api.h"
#include "host_func_opt.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------

typedef struct uiPara_s {	/* Can't be larger than 512Bytes */
/* Note: Don't add any struct member at here !!! */
/* ------ Note: Auto create code zone start flag, don't modify or delete this line!!! ------ */
	UINT8 EmptyPara;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 SnapAf;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageQuality;		/* Auto create code zone, don't modify or delete! */
	UINT8 AEMetering;		/* Auto create code zone, don't modify or delete! */
	UINT8 FDMetering;		/* Auto create code zone, don't modify or delete! */
	UINT8 Sharpness;		/* Auto create code zone, don't modify or delete! */
	UINT8 Saturation;		/* Auto create code zone, don't modify or delete! */
	UINT8 Contrast;		/* Auto create code zone, don't modify or delete! */
	UINT8 WBMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 AEMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 DriverMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 SpeedyCap;		/* Auto create code zone, don't modify or delete! */
	UINT8 SpeedyCapRec;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageEffect;		/* Auto create code zone, don't modify or delete! */
	UINT8 SceneMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 PhotoFrame;		/* Auto create code zone, don't modify or delete! */
	UINT8 StampMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 Iso;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 FrameRate;		/* Auto create code zone, don't modify or delete! */
	UINT8 StampVideoMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 AdvanceDenoise;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageSizeNoCard;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoSizeNoCard;		/* Auto create code zone, don't modify or delete! */
	UINT8 AudInAviRec;		/* Auto create code zone, don't modify or delete! */
	UINT8 AudInAviPlay;		/* Auto create code zone, don't modify or delete! */
	UINT8 Interval;		/* Auto create code zone, don't modify or delete! */
	UINT8 Repeat;		/* Auto create code zone, don't modify or delete! */
	UINT8 AntiShake;		/* Auto create code zone, don't modify or delete! */
	UINT8 FDMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 SmileMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcYear;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMonth;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcDay;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcHour;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMinute;		/* Auto create code zone, don't modify or delete! */
	UINT8 DateStyle;		/* Auto create code zone, don't modify or delete! */
	UINT8 Language;		/* Auto create code zone, don't modify or delete! */
	UINT8 SpeakSound;		/* Auto create code zone, don't modify or delete! */
	UINT8 LightFreq;		/* Auto create code zone, don't modify or delete! */
	UINT8 SleepTime;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSImageMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSCopies;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSQuality;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSLayout;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSPageSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSPageType;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSDate;		/* Auto create code zone, don't modify or delete! */
	UINT8 DPSFileName;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_ob;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_lc;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_gma;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_ae;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_awb;		/* Auto create code zone, don't modify or delete! */
	UINT8 cal_wdr;		/* Auto create code zone, don't modify or delete! */
	UINT8 USBMode;		/* Auto create code zone, don't modify or delete! */
/* ------ Note: Auto create code zone end flag, don't modify or delete this line!!! ------ */
/* Note: You can add struct member after this line !!! */
	UINT8 ImageEdge;
	UINT8 VideoWBMode;
	UINT8 VideoQuality;
	UINT8 resCreate;
	UINT8 DisMode;
	UINT8 BlinkMode;	
	UINT8 redEyePb;	
	UINT8 tpCorrect;
	UINT8 ShutterSound;
	UINT8 StartupSound;
	UINT8 BeepSound;
	UINT8 Volume;
	UINT8 AutoReviewTime;
	UINT8 TVMode;
	//UINT8 USBMode;
	UINT8 OpenPic;
	UINT8 ProtTime;
	UINT8 Seamless;
	UINT8 FlashLed;
//	UINT8 SnapAf;

	UINT8 AFMode;
	//UINT8 LightFreq;
	//UINT8 SleepTime;
	//UINT8 FDMode;
	//UINT8 SmileMode;
	//UINT8 AntiShake;
	#if (SUPPORT_AF && SUPPORT_FIXED_AF)
	UINT8 FocalLenght;
	#endif
	UINT16 AdjustMacro;
	//UINT8 SpeakSound;
/* Note: Next is checksum , don't add anything below */
	UINT8 SunplusFlag[4];
	UINT32 Checksum;
} uiPara_t;


typedef enum uiParaInitMode_e {
	PARA_INIT_WITH_DEF = 0x00,
	PARA_INIT_FROM_UDF = 0x01,
} uiParaInitMode_t;

typedef enum uiItemOrder_e {
	ITEM_EMPTY = 0x80,
	ITEM_FIRST = 0x81,
	ITEM_MAX   = 0xfe,
	ITEM_NULL  = 0xff
} uiItemOrder_t;

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------
void appUiParaInit(uiParaInitMode_t mode);
uiPara_t* appUiParaGet(void);
UINT8 appUiItemOrderGet(UINT8 idx,UINT8 *pTable,UINT8 *length);
void appUiItemOrderCfg(void);
UINT8 appUiItemStsSet(UINT8 idx,UINT8 val);
UINT8  appUiItemStsGet(UINT8 idx);

#endif /*__APP_UI_PARA_H__*/
