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
 * @file		sp1k_video_capture.h
 * @brief		sp1k video capture module header file.
 * @author		hy.ren
 * @since		2012-04-13
 * @date		2012-04-13
 */
#ifndef __SP1K_VID_CAP_API_H__
#define __SP1K_VID_CAP_API_H__

#include "video_capture.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn void sp1kVideoCapStart ()
 * @brief	video capture start
 * @retval 			none
 * @see also 		none
 */
void sp1kVideoCapStart ();
/**
 * @fn void sp1kSpeedyCaptureStop ()
 * @brief	
 * @retval 			
 * @see also 		none
 */
UINT8 sp1kSpeedyCaptureStop();
/**
 * @fn void sp1kVideoCapStampStsSet ()
 * @brief	enable date stamp funciton
 * @retval 			none
 * @see also 		none
 */
void sp1kVideoCapDateStampEn(UINT8 en);

/**
 * @fn void sp1kVideoCapParamGet ()
 * @brief	 get video capture paramter
 * @retval 			none
 * @see also 		none
 */
UINT8 sp1kVideoCapParamGet(UINT8 paraId);

/**
 * @fn void sp1kVideoCapParamSet ()
 * @brief	 set video capture paramter
 * @retval 			none
 * @see also 		none
 */
UINT8 sp1kVideoCapParamSet(UINT8 paraId, UINT16 value);

/**
 * @fn void sp1kSpeedyCaptureCallbackReg ()
 * @brief	
 * @param[in] id
 * @param[in] pfunc
 * @retval 	 none
 * @see also none
 */
void sp1kSpeedyCaptureCallbackReg(SP1KVDCapCallBackId_e id,void *pfunc);

/**
 * @fn void sp1kSpeedyCapStsGet ()
 * @brief	get speedy capture running status
 * @retval 	 speedy capture running status
 * @see also none
 */
UINT8 sp1kSpeedyCapStsGet();

#else

typedef enum {
	SP1K_VD_CAP_COUNT,
	SP1K_VD_CAP_SIZE,
	SP1K_VD_CAP_MOD,
	SP1K_VD_CAP_PARAM_MAX,
} SP1KVDCapParam_t;

typedef enum {
	SP1K_SPEEDCAP_STILL = 1,
	SP1K_SPEEDCAP_CLIP,
	SP1K_SPEEDCAP_LIGHT_OPEN,
	SP1k_SPEEDCAP_LIGHT_CLOSE,
}SP1KVDCapCallBackId_e;

typedef enum{
	SP1K_SPEEDCAP_IDLE,
	SP1K_SPEEDCAP_RUNNING,
}SP1KVDCapSts_e;

#define sp1kVideoCapStart				speedyCaptureStart
#define sp1kSpeedyCaptureStop			speedyCaptureStop
#define sp1kVideoCapDateStampEn 		speedyCaptureDstEn
#define sp1kVideoCapParamGet			speedyCaptureParamGet
#define sp1kVideoCapParamSet 			speedyCaptureParamSet
#define sp1kSpeedyCaptureCallbackReg    speedyCaptureCallbackReg
#define sp1kSpeedyCapStsGet				speedyCaptureStsGet
#endif

#endif  /* __SP1K_VIDEO_CAPTURE_API_H__ */

