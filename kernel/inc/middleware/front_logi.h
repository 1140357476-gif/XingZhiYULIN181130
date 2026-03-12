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
 * @file		front_logi.h
 * @brief		front module header file.
 * @author		Will 
 * @since		2010-03-01
 * @date		2010-03-01
 */
#ifndef __FRONT_LOGI_H__
#define __FRONT_LOGI_H__


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
//#include "hal_front.h" 
#include "front_para.h"
#include "hal_global.h"
#include "sys_mem_init.h"
#include "Hal_front.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FRONT_SNAP_BREAK_DOWN	0	/* 1:sensor timing, 2:host+sensor timing, 3:host+sensor+vlc timing */

#define FRONT_USE_MACRO 	1  /* fdk internal usage only. */

#define FRONT_DEVICE_TOT	1	/* support sensor number */

/**************************************************************************
 *                          F R O N T  R E S O U R C E  F I L E  I D                            *
 **************************************************************************/
#define FRONT_RES_PARAM_MAX_SIZE	(512UL)
#define FRONT_RES_TBL_MAX_SIZE		(4UL * 1024UL)	//ae table bin max size
#define FRONT_RES_CMD_MAX_SIZE		(12UL * 1024UL)	//i2c cmd bin max size
#define FRONT_RES_PARAM_LSC_ID		(0x42)
//#define FRONR_RES_CMD_TBL_SNAP_ID	(0x00)

 /**************************************************************************
  * 						 E N U M							 *
  **************************************************************************/
 /* plz keep sync to HAL_FrontParaId_e. */

typedef enum {
	FRONT_YUV_OUTPUT_UYVY = (0 << 0),
	FRONT_YUV_OUTPUT_YVYU = (1 << 0),
	FRONT_YUV_OUTPUT_VYUY = (2 << 0),
	FRONT_YUV_OUTPUT_YUYV = (3 << 0),
} frontYuvOutputSeqCfg_e;  /* plz keep sync to HAL_FrontYuvOutputSeqCfg_e. */

typedef enum {
	FRONT_YUV_CCIR601 = (0 << 4),
	FRONT_YUV_CCIR656 = (1 << 4),
} frontYuvCcirMode_e;  /* plz keep sync to HAL_FrontYuvCcirMode_e. */

typedef enum {
	FRONT_MCLK_SRC_INT = (0 << 2),
	FRONT_MCLK_SRC_EXT = (1 << 2),
} frontMclkSrc_e;  /* plz keep sync to HAL_FrontMclkSrc_e. */

#if 1  /* use define instead of enum to reduce compile warning. */
#define FRONT_PCLK_SRC_INT  (0 << 3)
#define FRONT_PCLK_SRC_EXT  (1 << 3)
#else
typedef enum {
	FRONT_PCLK_SRC_INT = (0 << 3),
	FRONT_PCLK_SRC_EXT = (1 << 3),
} frontpclkSrc_e;  /* plz keep sync to HAL_FrontpclkSrc_e. */
#endif

typedef enum {
	FRONT_BAYER_PTN_GRRBGB = 0,
	FRONT_BAYER_PTN_RGRGBB,
	FRONT_BAYER_PTN_BGBGRR,
	FRONT_BAYER_PTN_GBBRGR,
} frontBayerPattern_e;

typedef enum {
	FRONT_SRAM_MODE_DISABLE = 0,
	FRONT_SRAM_MODE_ENABLE,
} frontSramMode_e;

typedef enum {
	FRONT_WAIT_HSYNC  = (1 << 0),
	FRONT_WAIT_VSYNC  = (1 << 1),
	FRONT_WAIT_HVALID = (1 << 2),
	FRONT_WAIT_VVALID = (1 << 3),
} frontWaitSource_e;  /* plz keep sync to HAL_FrontWaitSource_e. */

typedef enum {
	FRONT_WAIT_FALLING = 0,
	FRONT_WAIT_RISING  = 1,
	FRONT_WAIT_HIGH    = 2,
	FRONT_WAIT_LOW     = 3,
} frontWaitMode_e;  /* plz keep sync to HAL_FrontWaitMode_e. */

typedef enum {
	FRONT_DEV_INFO_DATA_FMT,
	FRONT_DEV_INFO_BAYER_FMT,
	FRONT_DEV_INFO_PREVIEW_LINE_TOL	,	
	FRONT_DEV_INFO_PREVIEW_PCLK,
	FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,
	FRONT_DEV_INFO_TOT,
} frontDeviceInfo_e;

typedef enum {
	FRONT_DATA_FMT_RAW,
	FRONT_DATA_FMT_YUV,
	FRONT_DATA_FMT_TOT,
} frontDataFmt_e;

typedef enum {
	FRONT_PV_DYNAMIC_TD_DISABLE = 0,
	FRONT_PV_DYNAMIC_TD_ENABLE,
} frontDynamicTempDenoiseMode_e;


#if 0
typedef enum {
	FRONT_CAP_INT_NUM_0 = 0,
	FRONT_CAP_INT_NUM_1 = 1,
	FRONT_CAP_INT_NUM_2 = 2,
	FRONT_CAP_INT_NUM_3 = 3,
	FRONT_CAP_INT_NUM_4 = 4,
	FRONT_CAP_INT_NUM_5 = 5,
	FRONT_CAP_INT_NUM_6 = 6,
	FRONT_CAP_INT_NUM_7 = 7,
} frontCaptureIntervalNum_e;

typedef enum {
	FRONT_CAP_INT_DISABLE = 0,
	FRONT_CAP_INT_ENABLE = (1<<4),
} frontCaptureIntervalMode_e;
#endif

typedef enum {
	FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET,
	FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET,
	FRONT_EXT_OP_AE_FRAME_RATE_SET,
	FRONT_EXT_OP_AWB_MODE_SET,
	FRONT_EXT_OP_AFTER_SNAP_SHOT,
	FRONT_EXT_OP_TOT,
} frontExtendOperatingFunc_e;

typedef enum {
	SENSOR_SNAP_FLASH_CFG_MODE = 0,
	SENSOR_SNAP_FLASH_CFG_WIDTH = 1,
	SENSOR_SNAP_FLASH_CFG_H_POS = 2,
	SENSOR_SNAP_FLASH_CFG_DELAY_TYPE = 3,
	SENSOR_SNAP_FLASH_CFG_MAX_GAIN = 4,
} sensor_snap_flash_cfg_t;

typedef enum {
	SENSOR_SNAP_FLASH_MODE_OFF = 0,
	SENSOR_SNAP_FLASH_CURTAIN_FRONT_SYNC = 0x02,
	SENSOR_SNAP_FLASH_CURTAIN_REAR_SYNC = 0x82,
} sensor_snap_flash_mode_t;

typedef enum {
	SENSOR_FLASH_DELAY_TYPE_H_POS = 0,
	SENSOR_FLASH_DELAY_TYPE_IMMEDIATE_TIMER = 2,
	SENSOR_FLASH_DELAY_TYPE_TOTAL,
} sensor_flash_delay_t;

typedef enum {
	SENSOR_PREFLASH_CFG_WIDTH = 1,
	SENSOR_PREFLASH_CFG_H_POS = 2,
	SENSOR_PREFLASH_CFG_DELAY_TYPE = 3,
} sensor_preflash_cfg_t;

typedef enum {
	SENSOR_GAIN_1X,
	SENSOR_GAIN_2X,
	SENSOR_GAIN_4X,
	SENSOR_GAIN_8X,
	SENSOR_GAIN_AUTO = 0xff,
} sensor_flash_gain_t;  

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if FRONT_USE_MACRO
	#define frontParaSet   		HAL_FrontParaSet
	#define frontReset      	HAL_FrontReset
	#define frontSignalWait 	HAL_FrontSignalWait
	#define cdspBayerPatternSet HAL_CdspBayerPatternSet
	#define cdspSkipWrSet		HAL_CdspVDramSkipWrSet
	#define cdspSkipWrSyncSet	HAL_CdspVDramSkipWrSyncSet
	#define globalTimerWait 	HAL_GlobalTimerWait
	#define frontReset  		HAL_FrontReset
	#define frontSnapSignalWait(a,b,c) 	{\
										UINT8 sts;\
										ENTER_CRITICAL(sts);\
										HAL_FrontSignalWait(a,b,c);\
										EXIT_CRITICAL(sts);\
										}
#endif 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if 0
typedef struct {
	UINT8 frontPrevMode;
	UINT8 frontSnapMode;
	UINT16 frontLastGain;			/* preview last set gain value */
	UINT16 frontLastGainIdx;		/* preview last set gain table index */
	UINT32 frontLastShutter;		/* preview last set shutter value */
	UINT16 frontSnapLastGain;		/* snapshot last set gain value */
	UINT32 frontSnapLastShutter;	/* snapshot last set shutter value */
	UINT8 frontLoadBackLastGainIdx;	/* for ov sensor auto ob need change gain */
	UINT8 frontLoadBackLastSetting;	/* load back option */
	UINT8 frontPrevModeSave;		/* pre-sensor mode */
} frontSystem_t;
#endif

typedef struct {
	UINT16 ev;
	UINT32 shutter;
	UINT16 gain;
	UINT16 iris;
	UINT8  captureCompensationGainIdx;
} frontAe_t;

typedef struct {
    UINT16 Shutter;
	UINT8 GainIdx;
} frontShutter_t;

typedef struct {
	UINT16 mclkDiv;
	UINT8  mclkPhase;
	UINT16 pclkDiv;
	UINT8  pclkPhase;
	UINT32 pclkFreq;
	UINT8  enTDGainIdx;
	UINT8  idxCvgZon;
} frontPreviewTemporalDenoise_t;


typedef struct {
	UINT8  sramModeEn;
	UINT8  pvBayerPtn;
	UINT16 width;
	UINT16 height;
	UINT16 xoff;
	UINT16 yoff;
	UINT16 lineTot;
	UINT16 frameTot;
	UINT16 mclkDiv;
	UINT8  mclkPhase;
	UINT16 pclkDiv;
	UINT8  pclkPhase;
	UINT32 pclkFreq;
	UINT8  bypassHref;
	UINT8  bypassVref;
	UINT8  reshapeHen;
	UINT16 reshapeHfall;
	UINT16 reshapeHrise;
	UINT8  reshapeVen;
	UINT16 reshapeVfall;
	UINT16 reshapeVrise;
	frontAe_t *ae60hzTbl;
	frontAe_t *ae50hzTbl;
	UINT8  *aeMaxIdxTbl;
	UINT8  *aeMinIdxTbl;
	UINT8  ev10ExpIdx;
	UINT16 *gainTbl;
	UINT8  gainMaxIdx;
	UINT8  gainMinIdx;
	frontPreviewTemporalDenoise_t *prevDynamicTDTbl;
	UINT16 sensorZoomFactor;
} frontPrevCapabDesc_t;

typedef struct {
	UINT8  snapBayerPtn;
	UINT16 width;
	UINT16 height;
	UINT16 xoff;
	UINT16 yoff;
	UINT16 lineTot;
	UINT16 frameTot;
	UINT16 mclkDiv;
	UINT16 mclkDivBurst;
	UINT16 mclkDivBurstUp;
	UINT8  mclkPhase;
	UINT16 pclkDiv;
	UINT8  pclkPhase;
	UINT32 pclkFreq;
	UINT32 pclkFreqBurst;
	UINT32 pclkFreqBurstUp;
	UINT8  bypassHref;
	UINT8  bypassVref;
	UINT8  reshapeHen;
	UINT16 reshapeHfall;
	UINT16 reshapeHrise;
	UINT8  reshapeVen;
	UINT16 reshapeVfall;
	UINT16 reshapeVrise;	
} frontSnapCapabDesc_t;

/* this data type is because of keil c function pointer argument limitation. */
typedef struct {
	frontPrevCapabDesc_t **ppprevCap;
	frontSnapCapabDesc_t **ppsnapCap;
} frontCapabilityArg_t;

/* this data type is because of keil c function pointer argument limitation. */
typedef struct {
	UINT32 val;
	UINT8 isr;
	UINT8 upd;
} frontExposureTimeArg_t;

/* operating function. */
typedef UINT8 *(code *pfOperatingFrontDevNameGet_t)(void);
typedef void (code *pfOperatingFrontFrameRateSet_t)(UINT8 fps);
typedef void (code *pfOperatingFrontCapabilityGet_t)(frontCapabilityArg_t *parg);
typedef void (code *pfOperatingFrontIntrCapabilityGet_t)(frontCapabilityArg_t *parg);
typedef void (code *pfOperatingFrontPowerOn_t)(void);
typedef void (code *pfOperatingFrontPowerOff_t)(void);
typedef void (code *pfOperatingFrontPreviewModeSet_t)(void);
typedef void (code *pfOperatingFrontPreviewSensorZoomModeSet_t)(UINT16 zoomfactor);
typedef void (code *pfOperatingFrontIntrPreviewSensorZoomModeSet_t)(UINT16 zoomfactor);
typedef void (code *pfOperatingFrontSnapModeSet_t)(void);
typedef void (code *pfOperatingFrontSnapShot_t)(UINT8 first, UINT8 mode, UINT8 scaleUp);
typedef void (code *pfOperatingFrontGainSet_t)(UINT8 val, UINT8 isr, UINT8 upd);
typedef void (code *pfOperatingFrontExposureTimeSet_t)(frontExposureTimeArg_t *parg);
typedef void (code *pfOperatingFrontIntrGainSet_t)(UINT8 val, UINT8 isr, UINT8 upd);
typedef void (code *pfOperatingFrontIntrExposureTimeSet_t)(frontExposureTimeArg_t *parg);
typedef UINT8 (code *pfOperatingFrontOpticalBlackStatusGet_t)(void);
typedef UINT8 (code *pfOperatingFrontOpticalBlackCalibrate_t)(void);
typedef void (code *pfOperatingFrontShutterOpen_t)(void);
typedef void (code *pfOperatingFrontShutterClose_t)(void);
typedef void (code *pfOperatingFrontIntrPreviewDynamicTempoalDenoiseSet_t)(UINT8 en);

typedef struct {
	UINT8 expGainSetPos;
	UINT8 gainAfterExp;
	pfOperatingFrontDevNameGet_t pfOpDevNameGet;
	pfOperatingFrontFrameRateSet_t pfOpFrameRateSet;
	pfOperatingFrontCapabilityGet_t pfOpCapabilityGet;
	pfOperatingFrontIntrCapabilityGet_t pfOpIntrCapabilityGet;
	pfOperatingFrontPowerOn_t pfOpPowerOn;
	pfOperatingFrontPowerOff_t pfOpPowerOff;
	pfOperatingFrontPreviewModeSet_t pfOpPreviewModeSet;
	pfOperatingFrontPreviewSensorZoomModeSet_t pfOpPreviewSensorZoomModeSet;
	pfOperatingFrontIntrPreviewSensorZoomModeSet_t pfOpIntrPreviewSensorZoomModeSet;
	pfOperatingFrontSnapModeSet_t pfOpSnapModeSet;
	pfOperatingFrontSnapShot_t pfOpSnapShot;
	pfOperatingFrontGainSet_t pfOpGainSet;
	pfOperatingFrontExposureTimeSet_t pfOpExposureTimeSet;
	pfOperatingFrontIntrGainSet_t pfOpIntrGainSet;
	pfOperatingFrontIntrExposureTimeSet_t pfOpIntrExposureTimeSet;
	pfOperatingFrontOpticalBlackStatusGet_t pfOpOpticalBlackStatusGet;
	pfOperatingFrontOpticalBlackCalibrate_t pfOpOpticalBlackCalibrate;
	pfOperatingFrontShutterOpen_t pfOpShutterOpen;
	pfOperatingFrontShutterClose_t pfOpShutterClose;
	pfOperatingFrontIntrPreviewDynamicTempoalDenoiseSet_t pfOpIntrPreviewDynamicTempoalDenoiseSet;
} frontOperatingFunc_t;

/* extended operating function. */
typedef void (code *pfFrontExtOperatingFunc_t)(void *parg);

typedef struct frontDev_s {
	frontOperatingFunc_t opFunc;
	pfFrontExtOperatingFunc_t extOpFunc[FRONT_EXT_OP_TOT];
} frontDev_t; 

typedef enum {
	FRONT_PARA_MCLK_CFG,
	FRONT_PARA_PCLK_CFG,
	FRONT_PARA_SYNC_INVERT_ENABLE,
	FRONT_PARA_BYPASS_ENABLE,
	FRONT_PARA_RESHAPE_ENABLE,
	FRONT_PARA_RESHAPE_REGION,
	FRONT_PARA_CROP_REGION,
	FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE,
	FRONT_PARA_TG_TOTAL_REGION,
	FRONT_PARA_TG_SYNC_GENERATOR_ENABLE,
	FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG,
	FRONT_PARA_YUV_CCIR_MODE,
	FRONT_PARA_YUV_SUB128_ENABLE,
	FRONT_PARA_YUV_INPUT_ENABLE,
	FRONT_PARA_MAX_ID,
} frontParaId_e; 

typedef enum {
    MS_CLOSE,
	MS_OPEN,	
	MS_IDLE,
} frontMSSts_e;

typedef enum {
	SNAP_MS_NORMAL,
	SNAP_OBCAL,
	SNAP_MSCAL_FRAME,
	SNAP_MSCAL_ROLLING,
	SNAP_MS_DARK,
} frontSnapMode_e;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8 frontPrevSensorZoomStatus;
extern UINT8 frontPrevSensorZoom2xGainEn;
extern UINT8 frontPrevSensorZoomSnapStatus;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/* operating function. */
UINT8 frontDevOpen(frontOperatingFunc_t *popFunc, UINT16 opFuncSize);
UINT8 frontExtendOpFuncSet(UINT8 id, UINT8 opId, pfFrontExtOperatingFunc_t pfExtOp);
UINT8 frontDevActSet(UINT8 id);
UINT8 frontDevActGet(void);
UINT8 frontIntrDevActGet(void);
UINT8 frontDevIdCheck(UINT8 *pid, frontDev_t **ppdev);
UINT8 frontFrameRateSet(UINT8 id, UINT8 fps);
UINT8 frontCapabilityGet(UINT8 id, frontPrevCapabDesc_t **ppPrevCap, frontSnapCapabDesc_t **ppSnapCap);
UINT8 frontIntrCapabilityGet(UINT8 id, frontPrevCapabDesc_t **ppPrevCap, frontSnapCapabDesc_t **ppSnapCap);
UINT8 frontDeviceInfoGet(UINT8 id, UINT8 infId, UINT16 *ppara1, UINT16 *ppara2);
UINT8 frontPowerOn(UINT8 id);
UINT8 frontPowerOff(UINT8 id);
UINT8 frontPreviewModeSet(UINT8 id);
UINT8 frontPreviewSensorZoomModeSet(UINT8 id, UINT16 factor);
UINT8 frontIntrPreviewSensorZoomModeSet(UINT8 id, UINT16 factor);
UINT8 frontSnapModeSet(UINT8 id);
UINT8 frontSnapShot(UINT8 id, UINT8 burst, UINT8 first, UINT8 mode, UINT8 scaleUp);
UINT8 frontGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd);
UINT8 frontExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd);
UINT8 frontIntrGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd);
UINT8 frontIntrExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd);
UINT8 frontOpticalBlackStatusGet(UINT8 id);
UINT8 frontOpticalBlackCalibrate(UINT8 id);
UINT8 frontShutterOpen(UINT8 id);
UINT8 frontShutterClose(UINT8 id);
UINT8 frontIntrPreviewDynamicTempoalDenoiseSet(UINT8 id, UINT8 en);

UINT8 frontLensCompensationSet(UINT8 mode);
UINT8 frontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);
UINT8 frontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);

/* extern operating function. */
UINT8 frontAeTargetLuminanceSet(UINT8 id, UINT8 val);
UINT8 frontAeExposureCompensationSet(UINT8 id, UINT8 val);
UINT8 frontAeFrameRateSet(UINT8 id, UINT16 val);
UINT8 frontAwbModeSet(UINT8 id, UINT8 mode);

UINT8 frontProbeModeSet(UINT8 probsel, UINT8 probmode,	UINT8 proben);


void frontSnapMethodSet(UINT8 method);
UINT8 frontSnapMethodGet(void);
void frontDarkSubCtrl(UINT8 en,UINT8 mode,UINT16 time);

UINT8 frontHighQualityDigitalZoomCapabilityGet(UINT8 id);
UINT8 frontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en);





#if 1//FUNC_HOST_IGBT_FLASH

void frontPreflashAeCfg(UINT32 shutter,UINT8 gain,UINT8 wait);
void frontSnapFlashAeCfg(UINT32 shutter,UINT8 gainIndex,UINT8 wait);

void frontPreflashCfg(UINT8 cfgsel,UINT16 value);
void frontSnapFlashCfg(UINT32 cfgsel,UINT32 value);

void frontPreflashTrig(UINT8 *befPreLuma,UINT8 *aftPreLuma); 
void frontSnapFlashTrig(UINT32 exptime);
UINT8  frontIgbtFlashCalibTrig(UINT32 trigTime,UINT8 saveYUV);

UINT32 frontSnapFlashTimeGet(UINT8 befPreFlashLuma,UINT8 preFlashLuma,UINT8 tagLuma,UINT8 pv2CapGainRatio);

void frontStrobeInit(void);

void frontLastShutterGainSet(UINT32 shutter, UINT8 gain);

sensor_flash_gain_t sensorFlashGainIdxGet(void);

UINT8 sensorGainIdxGet(sensor_flash_gain_t flashGainIdx);

UINT16 sensorPvToSnapFrmTimeRatio(void);
#endif

#endif  /* __FRONT_API_H__ */

