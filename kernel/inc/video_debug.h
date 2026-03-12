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
 * @file	  video_pb.h
 * @brief	  header for playback video
 * @author	  tq.zhang
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_DEBUG_H__
#define __Video_DEBUG_H__
#include "general.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define VD_DEBUG_STATUTE	0
#define VD_DEBUG_TIMING		0
#define VD_TEST_MODE		0

#define VD_REC_DEBUG		0

#define VD_SMOOTH_ZOOM_DBG		0


#define VD_TIMER_MODE_LENGTH		0	//end-start
#define VD_TIMER_MODE_GAP_AVG		1	//(end-start)/count
#define VD_TIMER_MODE_LENGTH_AVG	2	//sum(end-start)/count
#define VD_TIMER_MODE_LENGTH_MAX	3	//max(end-start)
#define VD_TIMER_MAX 	4
#define VD_COUNT_MAX	4

#define VD_BIG_VLC_SIZE		51200UL	//102400UL
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define VIDEO_FRAME_DEBUG_LEVEL 1

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
typedef struct{
	UINT32 t0;
	UINT32 t1;
	UINT32 cnt;
	UINT32 val;
}vdTimer_t;

typedef enum{
	VD_EVT_JPG_EOF = 0,
	VD_EVT_MOVE_VLC,
	VD_EVT_WR_FILE,
	VD_EVT_DROP_FRAME,
	VD_EVT_BOUNDARY_OVERFLOW
}vdEvt_e;

/* video smooth zoom system information */
//#define videoSmoothZoomSysInfo printf
#ifndef videoSmoothZoomSysInfo
#define videoSmoothZoomSysInfo	/_printf
#endif


/* video smooth zoom system information */
#if (VD_SMOOTH_ZOOM_DBG == 1)
#define videoSmoothZoomDbg printf
#endif
#ifndef videoSmoothZoomDbg
#define videoSmoothZoomDbg	/_printf
#endif

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
extern UINT32 vdTotalLoop;
extern UINT32 vdDropDisp;
extern UINT32 vdVlcWaitBuf;
extern UINT32 vdDispWaitBuf;
extern UINT32 vdMaxExceed;

extern vdTimer_t vdTimeTest[VD_TIMER_MAX]; //0: disp, 1:jpg
extern UINT32 vdTestCnt[VD_COUNT_MAX]; //0: dumy frame count

extern UINT8 testMode;

extern UINT16 vdTimeEvtCnt;
extern UINT32 vdBigVlcSizeSum;
extern UINT32 vdBigVlcCnt;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void vdDebugInit(void);
void vdDebugShow(void);

void vdTimerStart(UINT8 idx);
void vdTimerEnd(UINT8 idx);
void vdTimerGapTrig(UINT8 idx);
void vdTimerAvgLengthEnd(UINT8 idx);
void vdTimerMaxLengthEnd(UINT8 idx);
void vdTimerShow(UINT8 idx,UINT8 mode);

void vdPbTestModeSet(UINT8 mode);

void vdTimeEvt(UINT8 type);
void vdTimeEvt_Intr(UINT8 type);
void vdTimeEvtShow(void);
void vdTimeEvtSaveFile(UINT8 *aviFilename);

#endif /*__Video_DEBUG_H__*/
