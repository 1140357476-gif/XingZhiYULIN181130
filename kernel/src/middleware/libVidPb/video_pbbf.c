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
 * @file	  vid_pbbf.c
 * @brief	  middle ware  API for video playback backward/forward  module
 * @author	  tq.zhang
 * @since	  2010-05-11
 * @date	  2010-05-11
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_pb.h"
#include "hal_audio.h"
#include "hal_global.h"

#if FUNC_KNL_VDPB_LIB_EN
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

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
extern UINT8 slowMode;
extern UINT32 gPlayStartTime;
extern UINT32 audChunkSize;
extern xdata UINT32 gPlayTime;
extern xdata UINT8 fastFlg;
extern xdata UINT8 vid_flag;
extern UINT8 nonAudioFile;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//videoPbFWSpeedSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbFWSpeedSet(UINT8 speed)
 * @brief		Set FW speed when playback video
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbFWSpeedSet(UINT8 speed)USING_0
{
	UINT8 rate;
	UINT32 audRingCnt;
	UINT32 outIndex;

	if((vidPbCtrl.Idle == VID_PB_IDLE) || (vidPbCtrl.Idle == VID_PB_PAUSE)) {
		return;
	}

	slowMode = 0;//xian ++ 20080919

	rate = speed;
	if(rate ==1) {
		return;
	}
	if(!nonAudioFile){
		HAL_AudPause();
		HAL_AudRingBufCntRead( &audRingCnt);
		if(audRingCnt%audChunkSize) {
			audRingCnt = audRingCnt +audChunkSize;
		}
		audRingCnt = audRingCnt/audChunkSize;

		if(audRingCnt) {
			vidPbCtrl.lastAudIdx  = videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx +1, vidPbCtrl.chunckCnt , audRingCnt,1);
		}
		HAL_AudOutIndexGet(&outIndex);
		HAL_AudInIndexSet(((outIndex >> 9) +1)<<9);
	}
	HAL_GlobalReadGTimer(&gPlayTime);
	gPlayStartTime = gPlayTime - videoPbTimeLengthGet(); //xian ++ 20081211, for no voice
	vidPbCtrl.Idle = VID_PB_FW;
	vidPbCtrl.speed = rate;

	return;
}


//-----------------------------------------------------------------------------
//videoPbFRSpeedSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbRWSpeedSet(UINT8 speed)
 * @brief		Set FR speed when playback video
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbRWSpeedSet(UINT8 speed)USING_0
{
	UINT8 rate;
	UINT32 audRingCnt;
	UINT32 outIndex;

	if((vidPbCtrl.Idle == VID_PB_IDLE) || (vidPbCtrl.Idle == VID_PB_PAUSE)) {
		return;
	}

	slowMode = 0;//xian ++ 20080919

	rate = speed;
	if(rate ==1) {
		return;
	}
	if(!nonAudioFile){
		HAL_AudPause();
		HAL_AudRingBufCntRead(&audRingCnt);
		if(audRingCnt%audChunkSize) {
			audRingCnt = audRingCnt +audChunkSize;
		}
		audRingCnt = audRingCnt/audChunkSize;
		if(audRingCnt) {
			vidPbCtrl.lastAudIdx  = videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx+1 , vidPbCtrl.chunckCnt , audRingCnt,1);
		}
		HAL_AudOutIndexGet(&outIndex);
		HAL_AudInIndexSet(((outIndex >> 9) +1)<<9);
	}
	HAL_GlobalReadGTimer(&gPlayTime);
	gPlayStartTime = gPlayTime - videoPbTimeLengthGet(); //xian ++ 20081211, for no voice
	vidPbCtrl.Idle = VID_PB_RW;
	vidPbCtrl.speed = rate;

	return;
}

//-----------------------------------------------------------------------------
//videoPbFW
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbFW(void)
 * @brief		FW	process
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbFW(void)USING_0
{
	UINT32 idx =0;
	UINT32 curTime =0;
	UINT32 tmp = 0;
	UINT32 FWIdx = 0;
	UINT8 status;
	UINT8 speed;

	ENTER_CRITICAL( status );
	HAL_GlobalReadGTimer(&curTime);
	speed = vidPbCtrl.speed<<1;
	//tmp = curTime - gPlayTime;
	//FWIdx = (UINT32)(((UINT32)tmp <<8) * ((UINT32)speed)/vidPbCtrl.msPerFrame);
	tmp = (curTime - gPlayTime) << 8;
	if(slowMode){
		tmp = tmp / (UINT32)speed;
	}
	else{
		tmp = tmp * (UINT32)speed;
	}
	FWIdx = tmp / vidPbCtrl.msPerFrame;

	if(FWIdx) {
		idx= videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr, vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt, FWIdx , 0);
		vidPbCtrl.lastVidIdx = idx;

		if(idx) {
			HAL_AudPause();
			fastFlg = 1 - fastFlg;
			vid_flag = 1;
			gPlayTime = curTime;
		}else {

		}
	}
	EXIT_CRITICAL( status );

	return;
}

//-----------------------------------------------------------------------------
//videoPbFR
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbFR(void)
 * @brief		FR process
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbFR(void)USING_0
{
	UINT32 idx = 0;
	UINT32 curTime =0;
	UINT32 tmp = 0;
	UINT32 FRIdx = 0;
	UINT8 status;
	UINT8 speed;

	ENTER_CRITICAL( status );
	HAL_GlobalReadGTimer(&curTime);
	speed = vidPbCtrl.speed<<1;
	//tmp = curTime - gPlayTime;
	//FRIdx = (UINT32)(((UINT32)tmp <<8)* (UINT32)speed/vidPbCtrl.msPerFrame);
	tmp = (curTime - gPlayTime) << 8;
	if(slowMode){
		tmp = tmp / (UINT32)speed;
	}
	else{
		tmp = tmp * (UINT32)speed;
	}
	FRIdx = tmp / vidPbCtrl.msPerFrame;


	if(FRIdx>vidPbCtrl.vidChunkCnt)
		FRIdx = vidPbCtrl.vidChunkCnt;

	if(FRIdx) {
		idx = videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr, vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt, FRIdx, 0);
		vidPbCtrl.lastVidIdx = idx;
		if(idx) {
			HAL_AudPause();
			fastFlg = 1 - fastFlg;
			vid_flag = 1;
			gPlayTime = curTime;

		}else {

		}
	}


	EXIT_CRITICAL( status );
	return;
}

//xian --

//xian ++ 20080919, for slow motion
/**
 * @fn        void videoPbSlowFWSpeedSet(UINT8 speed)
 * @brief     set video Pb Slow FW Speed
 * @param     [in] speed
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void videoPbSlowFWSpeedSet(UINT8 speed)USING_0
{
	UINT32 pbTime;

	if((vidPbCtrl.Idle == VID_PB_IDLE) || (vidPbCtrl.Idle == VID_PB_PAUSE)) {
		return;
	}

	if(slowMode){
		slowMode = 0; //maybe from slowMode, clear first
		speed = 2;
	}

	videoPbFWSpeedSet(speed);

	pbTime = videoPbTimeGet();	//ptTime is the current frame time

	gPlayStartTime = gPlayTime - pbTime;	//gPlayTime is the time when change to slow mode
	slowMode = 1;

	return;
}
//xian ++ 20080919, for slow motion
/**
 * @fn        void videoPbSlowFWSpeedSet(UINT8 speed)
 * @brief     set video Pb Slow FW Speed
 * @param     [in] speed
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void videoPbSlowRWSpeedSet(UINT8 speed)USING_0
{
	UINT32 pbTime;

	if((vidPbCtrl.Idle == VID_PB_IDLE) || (vidPbCtrl.Idle == VID_PB_PAUSE)) {
		return;
	}

	if(slowMode){
		slowMode = 0; //maybe from slowMode, clear first
		speed = 2;
	}

	videoPbRWSpeedSet(speed);

	pbTime = videoPbTimeGet();	//ptTime is the current frame time

	gPlayStartTime = gPlayTime - pbTime;	//gPlayTime is the time when change to slow mode
	slowMode = 1;

	return;
}

#endif
