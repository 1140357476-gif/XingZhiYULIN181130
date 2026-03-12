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
 * @file      front_debug.h
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __FRONT_DEBUG_H__
#define __FRONT_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "dbg_file_list.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* front logic system information */
#define frontSysInfo printf
#ifndef frontSysInfo
#define frontSysInfo	/_printf
#endif

/* probe front signal function dgb switch */
#define FRONT_PROBE_MODE_SET_DBG                  0

/* normal function dgb switch */
#define FRONT_DEV_OPEN_DBG                        0
#define FRONT_EXTEND_OP_FUNC_SET_DBG              0
#define FRONT_DEV_ACT_SET_DBG                     0
#define FRONT_DEV_ACT_GET_DBG                     0
#define FRONT_DEV_ID_CHECK_DBG				      0
#define FRONT_INTR_DEV_ID_CHECK_DBG               0
#define FRONT_FRAME_RATE_SET_DBG                  0
#define FRONT_CAPABILITY_GET_DBG                  0
#define FRONT_INTR_CAPABILITY_GET_DBG             0
#define FRONT_DEVICE_INFO_GET_DBG                 0
#define FRONT_POWER_ON_DBG                        1	/* system message, always enable */
#define FRONT_POWER_OFF_DBG                       0
#define FRONT_PREVIEW_MODE_SET_DBG                0
#define FRONT_SNAP_MODE_SET_DBG                   0
#define FRONT_SNAP_SHOT_DBG                       0
#define FRONT_GAIN_SET_DBG                        0
#define FRONT_EXPOSURE_TIME_SET_DBG               0
#define FRONT_INTR_GAIN_SET_DBG                   0
#define FRONT_INTR_EXPOSURE_TIME_SET_DBG          0
#define FRONT_OPTICAL_BLACK_STATUS_GET_DBG        0
#define FRONT_OPTICAL_BLACK_CALIBRATE_DBG         0
#define FRONT_SHUTTER_OPEN_DBG                    0
#define FRONT_SHUTTER_CLOSE_DBG                   0
#define FRONT_LENS_COMPENSATION_SET_DBG           0
#define FRONT_PREVIEW_SIZE_GET_DBG                0
#define FRONT_SNAP_SIZE_GET_DBG                   0
#define FRONT_AE_TARGET_LUMINANCE_SET_DBG         0
#define FRONT_AE_EXPOSURE_COMPENSATION_SET_DBG    0
#define FRONT_AE_FRAME_RATE_SET_DBG               0
#define FRONT_AWB_MODE_SET_DBG                    0
#define FRONT_SNAP_METHOD_SET_DBG                 0
#define FRONT_SNAP_METHOD_GET_DBG                 0
#define FRONT_DARK_SUB_CTRL_DBG                   0












/* frontDevOpen function dbg */
#if (FRONT_DEV_OPEN_DBG)
#define frontDevOpenPrintf printf
#endif
#ifndef frontDevOpenPrintf
#define frontDevOpenPrintf	/_printf
#endif

/* frontExtendOpFuncSet function dbg */
#if (FRONT_EXTEND_OP_FUNC_SET_DBG)
#define frontExtendOpFuncSetPrintf printf
#endif
#ifndef frontExtendOpFuncSetPrintf
#define frontExtendOpFuncSetPrintf	/_printf
#endif

/* frontDevActSet function dbg */
#if (FRONT_DEV_ACT_SET_DBG)
#define frontDevActSetPrintf printf
#endif
#ifndef frontDevActSetPrintf
#define frontDevActSetPrintf	/_printf
#endif

/* frontDevActGet function dbg */
#if (FRONT_DEV_ACT_GET_DBG)
#define frontDevActGetPrintf printf
#endif
#ifndef frontDevActGetPrintf
#define frontDevActGetPrintf	/_printf
#endif

/* frontDevIdCheck function dbg */
#if (FRONT_DEV_ID_CHECK_DBG)
#define frontDevIdCheckPrintf printf
#endif
#ifndef frontDevIdCheckPrintf
#define frontDevIdCheckPrintf	/_printf
#endif

/* frontIntrDevIdCheck function dbg */
#if (FRONT_INTR_DEV_ID_CHECK_DBG)
#define frontIntrDevIdCheckPrintf printf
#endif
#ifndef frontIntrDevIdCheckPrintf
#define frontIntrDevIdCheckPrintf	/_printf
#endif

/* frontFrameRateSet function dbg */
#if (FRONT_FRAME_RATE_SET_DBG)
#define frontFrameRateSetPrintf printf
#endif
#ifndef frontFrameRateSetPrintf
#define frontFrameRateSetPrintf	/_printf
#endif

/* frontCapabilityGet function dbg */
#if (FRONT_CAPABILITY_GET_DBG)
#define frontCapabilityGetPrintf printf
#endif
#ifndef frontCapabilityGetPrintf
#define frontCapabilityGetPrintf	/_printf
#endif

/* frontIntrCapabilityGet function dbg */
#if (FRONT_INTR_CAPABILITY_GET_DBG)
#define frontIntrCapabilityGetPrintf printf
#endif
#ifndef frontIntrCapabilityGetPrintf
#define frontIntrCapabilityGetPrintf	/_printf
#endif

/* frontDeviceInfoGet function dbg */
#if (FRONT_DEVICE_INFO_GET_DBG)
#define frontDeviceInfoGetPrintf printf
#endif
#ifndef frontDeviceInfoGetPrintf
#define frontDeviceInfoGetPrintf	/_printf
#endif

/* frontPowerOn function dbg */
#if (FRONT_POWER_ON_DBG)
#define frontPowerOnPrintf printf
#endif
#ifndef frontPowerOnPrintf
#define frontPowerOnPrintf	/_printf
#endif

/* frontPowerOff function dbg */
#if (FRONT_POWER_OFF_DBG)
#define frontPowerOffPrintf printf
#endif
#ifndef frontPowerOffPrintf
#define frontPowerOffPrintf	/_printf
#endif

/* frontPreviewModeSet function dbg */
#if (FRONT_PREVIEW_MODE_SET_DBG)
#define frontPreviewModeSetPrintf printf
#endif
#ifndef frontPreviewModeSetPrintf
#define frontPreviewModeSetPrintf	/_printf
#endif

/* frontSnapModeSet function dbg */
#if (FRONT_SNAP_MODE_SET_DBG)
#define frontSnapModeSetPrintf printf
#endif
#ifndef frontSnapModeSetPrintf
#define frontSnapModeSetPrintf	/_printf
#endif

/* frontSnapShot function dbg */
#if (FRONT_SNAP_SHOT_DBG)
#define frontSnapShotPrintf printf
#endif
#ifndef frontSnapShotPrintf
#define frontSnapShotPrintf	/_printf
#endif

/* frontGainSet function dbg */
#if (FRONT_GAIN_SET_DBG)
#define frontGainSetPrintf printf
#endif
#ifndef frontGainSetPrintf
#define frontGainSetPrintf	/_printf
#endif

/* frontExposureTimeSet function dbg */
#if (FRONT_EXPOSURE_TIME_SET_DBG)
#define frontExposureTimeSetPrintf printf
#endif
#ifndef frontExposureTimeSetPrintf
#define frontExposureTimeSetPrintf	/_printf
#endif

/* frontIntrGainSet function dbg */
#if (FRONT_INTR_GAIN_SET_DBG)
#define frontIntrGainSetPrintf printf
#endif
#ifndef frontIntrGainSetPrintf
#define frontIntrGainSetPrintf	/_printf
#endif

/* frontIntrExposureTimeSet function dbg */
#if (FRONT_INTR_EXPOSURE_TIME_SET_DBG)
#define frontIntrExposureTimeSetPrintf printf
#endif
#ifndef frontIntrExposureTimeSetPrintf
#define frontIntrExposureTimeSetPrintf	/_printf
#endif

/* frontOpticalBlackStatusGet function dbg */
#if (FRONT_OPTICAL_BLACK_STATUS_GET_DBG)
#define frontOpticalBlackStatusGetPrintf printf
#endif
#ifndef frontOpticalBlackStatusGetPrintf
#define frontOpticalBlackStatusGetPrintf	/_printf
#endif

/* frontOpticalBlackCalibrate function dbg */
#if (FRONT_OPTICAL_BLACK_CALIBRATE_DBG)
#define frontOpticalBlackCalibratePrintf printf
#endif
#ifndef frontOpticalBlackCalibratePrintf
#define frontOpticalBlackCalibratePrintf	/_printf
#endif

/* frontShutterOpen function dbg */
#if (FRONT_SHUTTER_OPEN_DBG)
#define frontShutterOpenPrintf printf
#endif
#ifndef frontShutterOpenPrintf
#define frontShutterOpenPrintf	/_printf
#endif

/* frontShutterClose function dbg */
#if (FRONT_SHUTTER_CLOSE_DBG)
#define frontShutterClosePrintf printf
#endif
#ifndef frontShutterClosePrintf
#define frontShutterClosePrintf	/_printf
#endif

/* frontLensCompensationSet function dbg */
#if (FRONT_LENS_COMPENSATION_SET_DBG)
#define frontLensCompensationSetPrintf printf
#endif
#ifndef frontLensCompensationSetPrintf
#define frontLensCompensationSetPrintf	/_printf
#endif

/* frontPreviewSizeGet function dbg */
#if (FRONT_PREVIEW_SIZE_GET_DBG)
#define frontPreviewSizeGetPrintf printf
#endif
#ifndef frontPreviewSizeGetPrintf
#define frontPreviewSizeGetPrintf	/_printf
#endif

/* frontSnapSizeGet function dbg */
#if (FRONT_SNAP_SIZE_GET_DBG)
#define frontSnapSizeGetPrintf printf
#endif
#ifndef frontSnapSizeGetPrintf
#define frontSnapSizeGetPrintf	/_printf
#endif

/* frontAeTargetLuminanceSet function dbg */
#if (FRONT_AE_TARGET_LUMINANCE_SET_DBG)
#define frontAeTargetLuminanceSetPrintf printf
#endif
#ifndef frontAeTargetLuminanceSetPrintf
#define frontAeTargetLuminanceSetPrintf	/_printf
#endif

/* frontAeExposureCompensationSet function dbg */
#if (FRONT_AE_EXPOSURE_COMPENSATION_SET_DBG)
#define frontAeExposureCompensationSetPrintf printf
#endif
#ifndef frontAeExposureCompensationSetPrintf
#define frontAeExposureCompensationSetPrintf	/_printf
#endif

/* frontAeFrameRateSet function dbg */
#if (FRONT_AE_FRAME_RATE_SET_DBG)
#define frontAeFrameRateSetPrintf printf
#endif
#ifndef frontAeFrameRateSetPrintf
#define frontAeFrameRateSetPrintf	/_printf
#endif

/* frontAwbModeSet function dbg */
#if (FRONT_AWB_MODE_SET_DBG)
#define frontAwbModeSetPrintf printf
#endif
#ifndef frontAwbModeSetPrintf
#define frontAwbModeSetPrintf	/_printf
#endif

/* frontProbeModeSet function dbg */
#if (FRONT_PROBE_MODE_SET_DBG)
#define frontProbeModeSetPrintf printf
#endif
#ifndef frontProbeModeSetPrintf
#define frontProbeModeSetPrintf	/_printf
#endif

/* frontSnapMethodSet function dbg */
#if (FRONT_SNAP_METHOD_SET_DBG)
#define frontSnapMethodSetPrintf printf
#endif
#ifndef frontSnapMethodSetPrintf
#define frontSnapMethodSetPrintf	/_printf
#endif

/* frontSnapMethodGet function dbg */
#if (FRONT_SNAP_METHOD_GET_DBG)
#define frontSnapMethodGetPrintf printf
#endif
#ifndef frontSnapMethodGetPrintf
#define frontSnapMethodGetPrintf	/_printf
#endif

/* frontDarkSubCtrl function dbg */
#if (FRONT_DARK_SUB_CTRL_DBG)
#define frontDarkSubCtrlPrintf printf
#endif
#ifndef frontDarkSubCtrlPrintf
#define frontDarkSubCtrlPrintf	/_printf
#endif

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

#endif  /* __FRONT_DEBUG_H__ */
