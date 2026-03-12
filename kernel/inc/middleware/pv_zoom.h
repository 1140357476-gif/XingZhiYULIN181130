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
 * @file      pv_zoom.h
 * @brief     Brief
 * @author    LinJieCheng
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __PV_ZOOM_H__
#define __PV_ZOOM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 #include "app_video_view.h"
 #include "initio.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
// #if (STILL_NEW_ZOOM == 0)
typedef struct appPvZoom_s {
	UINT16 factor;
	UINT16 step;
	UINT16 MaxFct;
	UINT16 MinFct;
	UINT16 maxLog;// hlc 0328
	UINT16 minLog;// hlc 0328
	UINT16 curLog;
	UINT8 logStep;// hlc 0328
}appPvZoom_t;
//#endif
 
typedef struct appStillZoom_s {
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
}appPvZoom_NBlocking_t;

typedef enum {
	PV_ZOOM_STATUS_START = 0,		
	PV_ZOOM_STATUS_FINISH,
	PV_ZOOM_STATUS_MAX,
}PV_ZOOM_STATUS;

typedef enum {
	PV_ZOOM_ACTION_INITIAL = 0,	//initial
	PV_ZOOM_ACTION_TELE,		//in
	PV_ZOOM_ACTION_WIDE,		//out
	PV_ZOOM_ACTION_TELE_ONE,	//in 1
	PV_ZOOM_ACTION_WIDE_ONE,	//out 1
	PV_ZOOM_ACTION_MAX,
}PV_ZOOM_ACTION;

typedef enum {
	PV_ZOOM_CTRL_PARA_CURRENT_FACTOR = 0,
	PV_ZOOM_CTRL_PARA_MAX_FACTOR,
	PV_ZOOM_CTRL_PARA_MIN_FACTOR,
	PV_ZOOM_CTRL_PARA_MAX_LOG,
	PV_ZOOM_CTRL_PARA_MIN_LOG,
	PV_ZOOM_CTRL_PARA_CURRENT_LOG,
	PV_ZOOM_CTRL_PARA_LOG_STEP,
	PV_ZOOM_CTRL_PARA_MAX,
}PV_ZOOM_CTRL_PARA_ITEM;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 pvZoomDevOpen(UINT8 type);
UINT8 pvZoomResLoad_Blocking(UINT32 *pArray) ;
UINT16 pvZoomFctGet_Blocking(void);
void pvSmoothZoomSet_Blocking(UINT8 en);
UINT8 pvSmoothZoomGet_Blocking(void);
UINT8 pvSmoothZoomStatusGet_Blocking(void);
UINT8 pvTeleSet_Blocking(void);
UINT8 pvWideSet_Blocking(void);
UINT8 pvZoom3APreSet_Blocking(void);
UINT8 pvZoom3APostSet_Blocking(void);
void pvZoomCtrlParaSet_Blocking(UINT8 item,	UINT16 value);
UINT16 pvZoomCtrlParaGet_Blocking(UINT8 item);
UINT8 pvZoomSet_Blocking(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze);

UINT8 pvZoomResLoad_NBlocking(UINT32 *pArray) ;
UINT16 pvZoomFctGet_NBlocking(void);
void pvSmoothZoomSet_NBlocking(UINT8 en);
UINT8 pvSmoothZoomGet_NBlocking(void);
UINT8 pvSmoothZoomStatusGet_NBlocking(void);
UINT8 pvTeleSet_NBlocking(void);
UINT8 pvWideSet_NBlocking(void);
UINT8 pvZoom3APreSet_NBlocking(void);
UINT8 pvZoom3APostSet_NBlocking(void);
void pvZoomCtrlParaSet_NBlocking(UINT8 item,	UINT16 value);
UINT16 pvZoomCtrlParaGet_NBlocking(UINT8 item);
UINT8 pvZoomSet_NBlocking(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze);
#endif  /* __PV_ZOOM_H__ */
