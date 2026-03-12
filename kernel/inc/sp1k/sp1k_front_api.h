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
 * @file		sp1k_front_api.h
 * @brief		sp1k front module header file.
 * @author		Matt Wang
 * @since		2008-09-15
 * @date		2008-10-23
 */
#ifndef __SP1K_FRONT_API_H__
#define __SP1K_FRONT_API_H__

#include "front_logi.h"
#include "snap_core.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define	VD_PROC_AE		0x01
#define	VD_PROC_AWB		0x02
#define	VD_PROC_FD		0x04
#define	VD_PROC_DIST	0x08
#define VD_PROC_MD		0x10



#define VD_PROC_NUM 5
#define	VD_PROC_EN	0x80
#define	VD_PROC_GET	0xFF


#define	SP1K_FRONT_NORMAL  CDSP_DO_RAW
#define	SP1K_FRONT_DOFRONT CDSP_DO_FRONT



/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

/**
 * @fn        void sp1kVdAvgTimeSvcProc(void)
 * @brief     Vd Avg Time Svc Proc
 * @param     [in] ptr	:in param
 * @retval    NONE
*/
void sp1kVdAvgTimeSvcProc (void);

/**
 * @fn        void vdSvcProc(void* ptr)
 * @brief     vdSvcProc
 * @param     [in] ptr	:in param
 * @retval    NONE
*/
void vdSvcProc(void* ptr	);

/**
 * @fn        UINT8 sp1kVdProcEnable(UINT8 flag, UINT8 en)
 * @brief     sp1kVdProcEnable
 * @param     [in] flag	VD_PROC_AE		:set/get ae status
 * 					VD_PROC_AWB	:set/get awb status
 *					VD_PROC_FD		:set/get fd status
 *					VD_PROC_DIST	:set/get dist status
 *					VD_PROC_GET	:return all status
 * @param     [in] en	1		:enable
 *					0		:disable
 *					0xff		:return status
 * @retval    return = last status
*/

UINT8 sp1kVdProcEnable(UINT8 flag, UINT8 en);

/**
 * @fn        void sp1kVdInit(void)
 * @brief     sp1k Vd  Init
 * @param     NONE
 * @retval    NONE
*/
void sp1kVdInit(void);

/**
 * @fn        UINT8 sp1kVdCountGet(void)
 * @brief     sp1kVdCountGet
 * @param     NONE
 * @retval    return = vd counter
*/
UINT8 sp1kVdCountGet(void);

/**
 * @fn        UINT8 sp1kFrontDevActSet(UINT8 id)
 * @brief     set current actived device
 * @param     [in] id : device id (1~FRONT_DEVICE_TOT)
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontDevActSet(UINT8 id);

/**
 * @fn        UINT8 sp1kFrontDevActGet(void)
 * @brief     get current actived device
 * @param     NONE
 * @retval    return = current actived device id, 1~FRONT_DEVICE_TOT
*/
UINT8 sp1kFrontDevActGet(void);


/**
 * @fn        UINT8 sp1kFrontPowerOn(UINT8 id)
 * @brief     power on device
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontPowerOn(UINT8 id);


/**
 * @fn        UINT8 sp1kFrontPowerOff(UINT8 id)
 * @brief     power off device
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontPowerOff(UINT8 id);


/**
 * @fn        UINT8 sp1kFrontOpticalBlackStatusGet(UINT8 id)
 * @brief     get optical black status
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontOpticalBlackStatusGet(UINT8 id);


/**
 * @fn        UINT8 sp1kFrontOpticalBlackCalibrate(UINT8 id)
 * @brief     calibrate optical black
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontOpticalBlackCalibrate(UINT8 id);


/**
 * @fn        UINT8 sp1kFrontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize)
 * @brief     get preview size
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] pHSize : preview horizontal size
 * @param     [in] pVSize : preview vertical size
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);


/**
 * @fn        UINT8 sp1kFrontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize)
 * @brief     get snap size
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] pHSize : snap horizontal size
 * @param     [in] pVSize : snap vertical size
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kFrontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);

/**
 * @fn        UINT8 sp1kFrontHighQualityDigitalZoomCapabilityGet(UINT8 id)
 * @brief     set High Quality Digital Zoom Capability Get
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kFrontHighQualityDigitalZoomCapabilityGet(UINT8 id);

/**
 * @fn        UINT8 sp1kFrontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en)
 * @brief     High Quality Digital Zoom mode set
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] en : 0 disable, 1 enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kFrontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en);


#define sp1kFrontDevActSet              frontDevActSet
#define sp1kFrontDevActGet              frontDevActGet
#define sp1kFrontPowerOn                frontPowerOn
#define sp1kFrontPowerOff               frontPowerOff
#define sp1kFrontOpticalBlackStatusGet  frontOpticalBlackStatusGet
#define sp1kFrontOpticalBlackCalibrate  frontOpticalBlackCalibrate
#define sp1kFrontPreviewSizeGet         frontPreviewSizeGet
#define sp1kFrontSnapSizeGet            frontSnapSizeGet
#define sp1kFrontHighQualityDigitalZoomCapabilityGet frontHighQualityDigitalZoomCapabilityGet
#define sp1kFrontHighQualityDigitalZoomModeSet frontHighQualityDigitalZoomModeSet



#endif  /* __SP1K_FRONT_API_H__ */

