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
 * @file      video_zoom.h
 * @brief     Brief
 * @author    LinJieCheng
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __VIDEO_ZOOM_H__
#define __VIDEO_ZOOM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ZOOM_BUF_MAX 2
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 typedef UINT16 zoom_t;

typedef struct ZoomSmoothQue_s
{
	struct ZoomSmoothQue_s* pQue;
	zoom_t* QueStart;
	zoom_t* QueEnd;
	zoom_t* QueIn;
	zoom_t* QueOut;
	UINT8 QueEntries;
	UINT8 QueSize;
	zoom_t* MsgBuf;
	UINT8 bLock;
} ZoomSmoothQue_t;

typedef enum ZoomSmoothCtrlType_e {
	ZOOM_CTRL_TYPE_STILL = 0x00,	//for still flow queue used
	ZOOM_CTRL_TYPE_VIDEO  = 0x01,	//for video flow queue used
	ZOOM_CTRL_TYPE_MAX  = 0x02,
} ZoomSmoothCtrlType_t;

typedef struct intrSmoothZoom_s {
	UINT32 zoomRegSize;
	UINT8 zoomRealTimeUpdate;
	UINT8 zoomReleaseKeySts;
	UINT8 zoom3AUpdateDlyFrame;
	UINT16 zoomClampH;
	UINT16 zoomClampV;
	UINT8 zoomFactUpdate;
	UINT8 zoomSmoothEn;
	UINT8 zoomFrameCnt;
	UINT8 zoomFrameUpdateInterval;
	UINT8 frontGateDisable;
}intrSmoothZoom;

typedef enum {
	INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE = 0,
	INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE,
	INTR_SMOOTH_ZOOM_CTRL_STATUS_MAX,
}INTR_SMOOTH_ZOOM_CTRL_STATUS;

typedef enum {
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_STOP = 0,
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_START,
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_NEXT_DELAY_FINISH,
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH,
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH_NEXT_FRAME,
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_MAX,
}INTR_SMOOTH_ZOOM_MACHINE_STATUS;

typedef enum {
	INTR_SMOOTH_SENSOR_ZOOM_FLOW_END = 0,
	INTR_SMOOTH_SENSOR_ZOOM_FLOW_START,
	INTR_SMOOTH_SENSOR_ZOOM_FLOW_GATING_DISABLE,
	INTR_SMOOTH_SENSOR_ZOOM_FLOW_MAX,
}INTR_SMOOTH_SENSOR_ZOOM_FLOW_STATUS;


typedef enum {
	INTR_SMOOTH_ZOOM_NOT_RELEASE_KEY = 0,
	INTR_SMOOTH_ZOOM_RELEASE_KEY,
	INTR_SMOOTH_ZOOM_MAX,
}INTR_SMOOTH_ZOOM_RELEASE_KEY_STATUS;

typedef enum {
	INTR_SMOOTH_ZOOM_FRAME_UPDATE_INTERVAL_2 = 2,
	INTR_SMOOTH_ZOOM_FRAME_UPDATE_INTERVAL_3,
	INTR_SMOOTH_ZOOM_FRAME_UPDATE_INTERVAL_MAX,
}INTR_SMOOTH_ZOOM_FRAME_UPDATE_INTERVAL;

typedef struct appVideoZoom_s {
	UINT16 factor;
	UINT16 MaxFct;
	UINT16 MinFct;
	UINT16 maxLog;
	UINT16 minLog;
	UINT16 curLog;
	UINT8 logStep;
	UINT8 zoomAction;
	UINT8 zoomPreAction;
	UINT8 hQDzoomStatus;
	UINT16 hQDzoomFct;
	UINT16 hQDzoomStep;
	UINT8 zoomStatus;
	UINT16 curLoadLog;
	UINT16 queueOffset;
}appVideoZoom_NBlocking_t;

typedef enum {
	VIDEO_ZOOM_STATUS_START = 0,	
	VIDEO_ZOOM_STATUS_FINISH,
	VIDEO_ZOOM_STATUS_MAX,
}VIDEO_ZOOM_STATUS;


typedef enum {
	VIDEO_ZOOM_ACTION_INITIAL = 0,	//initial
	VIDEO_ZOOM_ACTION_TELE,			//in
	VIDEO_ZOOM_ACTION_WIDE,			//out
	VIDEO_ZOOM_ACTION_TELE_ONE,		//in 1
	VIDEO_ZOOM_ACTION_WIDE_ONE,		//out 1
	VIDEO_ZOOM_ACTION_MAX,
}VIDEO_ZOOM_ACTION;


typedef enum {
	VIDEO_ZOOM_CTRL_PARA_CURRENT_FACTOR = 0,
	VIDEO_ZOOM_CTRL_PARA_MAX_FACTOR,
	VIDEO_ZOOM_CTRL_PARA_MIN_FACTOR,
	VIDEO_ZOOM_CTRL_PARA_MAX_LOG,
	VIDEO_ZOOM_CTRL_PARA_MIN_LOG,
	VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG,
	VIDEO_ZOOM_CTRL_PARA_LOG_STEP,
	VIDEO_ZOOM_CTRL_PARA_MAX,
}VIDEO_ZOOM_CTRL_PARA_ITEM;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT16 video_zoomFactor ;
extern UINT8 video_zoomInFlag ;
extern UINT8 video_zoomOutFlag ;
extern UINT16 ZoomCurLog ; // hlc 0328 improve zoom quality
extern appVideoZoom_NBlocking_t VideoZoomNBCtrl;
extern UINT16 dzoomDispXZoomFactor;
extern UINT16 dzoomDispYZoomFactor;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 videoZoomDevOpen(UINT8 type);
void zoomSmoothQueueInit (UINT16 msgGrp);
UINT8 zoomSmoothQueuePost (UINT16 msgGrp,UINT16 offset);
UINT8 zoomSmoothQueueAccept (UINT16 msgGrp,UINT16* offset);
UINT8 zoomSmoothQueueEntriesGet (UINT16 msgGrp,UINT8* Entries);
UINT8 INTR_zoomSmoothQueueEntriesGet (UINT16 msgGrp, UINT8* Entries);
UINT8 videoZoomResLoad_Blocking(UINT32 *pArray);
UINT8 videoZoom3APreSet_Blocking(void);
UINT8 videoZoom3APostSet_Blocking(void);
UINT8 videoZoomResLoad_Blocking(UINT32 *pParaArray);
void videoSmoothZoomSet_Blocking(UINT8 en);
UINT8 videoSmoothZoomGet_Blocking(void);
UINT8 videoSmoothZoomStatusGet_Blocking(void);
UINT8 videoWideSet_Blocking(void);
UINT8 videoTeleSet_Blocking(void);
void videoZoomCtrlParaSet_Blocking(UINT8 item,UINT16 value);
UINT16 videoZoomCtrlParaGet_Blocking(UINT8 item);
UINT8 videoRecZoomFacSet_Blocking(UINT8 noUsed,UINT16 factor, UINT8 videozoom);
UINT16 videoZoomFctGet_Blocking(void) ;

UINT8 videoZoomResLoad_NBlocking(UINT32 *pArray);
UINT8 videoZoom3APreSet_NBlocking(void);
UINT8 videoZoom3APostSet_NBlocking(void);
UINT8 videoZoomResLoad_NBlocking(UINT32 *pParaArray);
void videoSmoothZoomSet_NBlocking(UINT8 en);
UINT8 videoSmoothZoomGet_NBlocking(void);
UINT8 videoSmoothZoomStatusGet_NBlocking(void);
UINT8 videoWideSet_NBlocking(void);
UINT8 videoTeleSet_NBlocking(void);
void videoZoomCtrlParaSet_NBlocking(UINT8 item,UINT16 value);
UINT16 videoZoomCtrlParaGet_NBlocking(UINT8 item);
UINT8 videoRecZoomFacSet_NBlocking(UINT8 noUsed,UINT16 factor, UINT8 videozoom);
UINT16 videoZoomFctGet_NBlocking(void);
void videoRec2ndFacSet(UINT16 Src1stX,UINT16 Src1stY);

#endif  /* __VIDEO_ZOOM_H__ */

