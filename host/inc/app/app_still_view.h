/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __APP_STILL_VIEW_H__
#define __APP_STILL_VIEW_H__

#include "quantdef.h"
#include "host_func_opt.h"
#include "app_video_view.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define EDGE_9_LEVEL_FUN	0

#define SP1K_STILL_VIEW_INIT_FULL			0
#define SP1K_STILL_VIEW_INIT_SMART		1

#define ICON_IMGSIZE_SHOW 1
#define ICON_IMGQUALITY_SHOW 1
#define ICON_WBMODE_SHOW 1
#define ICON_AEMET_SHOW 1
#define ICON_FLASH_SHOW 0

#define SNAP_QUALITY_SUPER_FINE	K_Q95
#define SNAP_QUALITY_FINE       K_Q85
#define SNAP_QUALITY_NORMAL     K_Q80
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if FUNC_HOST_PF_PB


#define SYN_DISPBUFF(src, dst) sp1kHalGprmCpy(\
							 	pbDispBuffGet(src), \
							 	PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,\
							 	pbDispBuffGet(dst), \
							 	PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,\
							 	PB_DECODE_WIDTH, PB_DECODE_HEIGHT,\
							 	0, 1) 
#define BACKUP_DISPBUFF() 	SYN_DISPBUFF(1, 0)
#define RESUME_DISPBUFF() 	SYN_DISPBUFF(0, 1)
#endif
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum ResPhotoFrame_e {
	RES_PHOTO_FRAME_A = 0x1f,
	RES_PHOTO_FRAME_B = 0x20,
	RES_PHOTO_FRAME_C = 0x21,
	RES_PHOTO_FRAME_D = 0x22,
} ResPhotoFrame_t;

typedef enum Flow_e {
	CALL_DEFAULT= 0x00,
	IGNORE_DEFAULT = 0x01,
} Flow_t;



/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
**************************************************************************/
void appStillView(UINT16 msg);
void appStillViewInit(UINT8 InitMode);
//void appStillViewZoomFactorSet(UINT16 value);
void appStillViewZoomIn(void) ;
void appStillViewZoomOut(void) ;
void uiShowOSDSelfSnapCntDown(UINT8 countDown,UINT8 enabel);
UINT8 appDriveModeSet(void);
UINT8 appEffectSet(UINT8 effect);
UINT8 appSceneSet(UINT8 scene);

void appSelfSnapMSGSend(void);
UINT8 appStillQualitySet(void);//jintao.liu 2008-3-12
UINT8 appStillResolutionSet(UINT8 resolution);//suny 2008.3.13
void appStillQuickViewSet(void);
void appSnapParaInit(void);
UINT8 appWBSet(UINT8 WBValue);
UINT8 appElecFreqSet(void);
UINT8 appStillViewAEAWBReadyGet(UINT8 start);
UINT8 appSnapBurstCheck( UINT8 burstCnt );
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
UINT8 appPvPhotoFrameSet(UINT8 idx);
UINT8 appPbPhotoFrameSet(UINT8 idx,UINT8 bSave);
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */		
UINT8 appPvForceUpdateGet(void);
void appPvForceUpdateOn(UINT8 on);
UINT8 appISOSet(UINT8 ISOValue);       
void appAntiShakeSet(UINT8 antiShake);           

UINT8* appUserDefineMeter(UINT8 w, UINT8 h);
void appUserMeterModeSet(UINT8 en);
UINT8 appUserMeterModeGet(void);
void appDynamicProgrammingUserMeterFlow(void);


void appCustomizeStampSet(void);

UINT8 appSnapBackgroundResLoad(UINT32 dstAddrWord, UINT32 bufByteSize);
UINT8 appSnapHostStamp(void);

UINT8 appSharpnessSet(UINT8 SharpnessValue);
UINT8 appSaturationSet(UINT8 SaturationValue);
UINT8 appContrastSet(UINT8 ContrastValue);
UINT8 appStillSmoothZoomMsgProcFlow (UINT16 sysMsg);





//void sp1kSetDateStamp(void);

#endif /*__APP_STILL_VIEW_H__*/

