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
#ifndef __APP_VIDEO_VIEW_H__
#define __APP_VIDEO_VIEW_H__

#include "common.h"

/**************************************************************************
 *note:
 *		(1)VIDEO_NEW_ZOOM == 0 , zoom factor used lookup table and other parameter used cpu calculate or
 *								zoom all parameter used cpu calculate.
 *								ctrl define in app_sts_stillview.c (STILL_ZOOM_FACTOR_LOOKUP_TABLE)
 *                                                and video_clip.h(VIDEO_ZOOM_FACTOR_LOOKUP_TABLE)
 *		(2)VIDEO_NEW_ZOOM == 1 , zoom all parameter used lookup table and setting flow based on intr
 *
 **************************************************************************/
#define VIDEO_NEW_ZOOM		0
#define STILL_NEW_ZOOM		0


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SP1K_VIDEO_VIEW_INIT_FULL    0
#define SP1K_VIDEO_VIEW_INIT_SMART   1
enum{
	K_FRAME_RATE_30 = 300,
	K_FRAME_RATE_25 = 301,
	K_FRAME_RATE_15 = 150,
	K_FRAME_RATE_12P5 = 151
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appVideoView(UINT16 msg);
void appVideoViewInit(UINT8 initMode);
void appVideoResSet(UINT8 res);
void appVideoPreSizeSet(UINT8 res) ;
void appVideoPauseProcess(void) ;
void appVideoSnap(void) ;
UINT8 appVideoQualitySet(void);
UINT8 appVideoFrmRateSet(void);

void appVideoViewTele(void);
void appVideoViewWide(void);

#if (VIDEO_NEW_ZOOM == 0)
void appVideoViewTeleReply(void);
void appVideoViewWideReply(void);
#endif

#if (VIDEO_NEW_ZOOM == 1)

#endif
void appVideoBrcSet(void);
UINT8 appVideoRecMemFullChk(void);
void appMotionDetectTimeSet(void);
UINT8 appVideoSmoothZoomMsgProcFlow (UINT16 sysMsg);

void appVideoSeamlessTimeSet(void);

#endif /*__APP_VIDEO_VIEW_H__*/

