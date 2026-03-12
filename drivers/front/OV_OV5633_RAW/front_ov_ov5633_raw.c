/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 * @file		front_ov_ov5633_raw.c
 * @brief		front sensor omni vision ov5633 driver.
 * @author		Matt Wang
 * @since		2008-10-14
 * @date		2008-11-21
 */

/*
 * History
 *
 * 1000, 20081117 Matt Wang
 *    a. Created
 */
 
/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "asicreg.h"
#include "initio.h"
#include "front_logi.h"
#include "gpio.h"
#include "gpio_api.h"
#include "i2c.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "dbg_def.h"
#include "hal_front_common.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "sp1k_ae_api.h"
#include "ae_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "sp1k_front_api.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		FRONT_SNAP_BREAK_DOWN	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */

#define RES_LSC_LOAD		0//after power on load lsc resource file
 
#define PV_AUTO_OB_BY_GAIN	1	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
/* mode. */
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1
#define PREV_RESP  960//960//720

/* dimensiion. */
#if (PREV_RESP==960)
/* 960p, 24fps */
#define WIDTH_24FPS_PREV      1280
#define HEIGHT_24FPS_PREV     960
#define XOFF_24FPS_PREV       496
#define YOFF_24FPS_PREV       12
#define LINE_TOT_24FPS_PREV   1776
#define FRAME_TOT_24FPS_PREV  981
#define PREV_TO_SNAP_BINNING_RATE_24FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_24FPS_PREV         0


/* 960p, 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       496
#define YOFF_30FPS_PREV       12
#define LINE_TOT_30FPS_PREV   1776
#define FRAME_TOT_30FPS_PREV  981
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0


#elif (PREV_RESP==720)//720p
/* 720p, 24fps */
#define WIDTH_24FPS_PREV      1280
#define HEIGHT_24FPS_PREV     720
#define XOFF_24FPS_PREV       836//695
#define YOFF_24FPS_PREV       9 
#define LINE_TOT_24FPS_PREV   2202
#define FRAME_TOT_24FPS_PREV  752
#define PREV_TO_SNAP_BINNING_RATE_24FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_24FPS_PREV         0


/* 720p, 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       836//695
#define YOFF_30FPS_PREV       9 
#define LINE_TOT_30FPS_PREV   2202
#define FRAME_TOT_30FPS_PREV  752
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0
#endif

/* 480p, 60fps */
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       910
#define YOFF_60FPS_PREV       9
#define LINE_TOT_60FPS_PREV   1550
#define FRAME_TOT_60FPS_PREV  516
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_60FPS_PREV         0


/* 5M */
#define WIDTH_SNAP      2592
#define HEIGHT_SNAP     1944
#define XOFF_SNAP       650
#define YOFF_SNAP       2
#define LINE_TOT_SNAP   3240
#define FRAME_TOT_SNAP  1988

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_24FPS_PREV    11
#define MCLK_PHASE_24FPS_PREV  0
#define PCLK_DIV_24FPS_PREV    11
#define PCLK_PHASE_24FPS_PREV  0

#define MCLK_DIV_30FPS_PREV    11
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP    		    11
#define MCLK_DIV_SNAP_BURST     18
#define MCLK_DIV_SNAP_BURST_UP  24
#define MCLK_PHASE_SNAP         0
#define PCLK_DIV_SNAP           11
#define PCLK_PHASE_SNAP         0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_24FPS_PREV     53380000L
#define PCLK_FREQ_24FPS_PREV_TD  36700000L
#define PCLK_FREQ_30FPS_PREV     53380000L
#define PCLK_FREQ_30FPS_PREV_TD  36700000L
#define PCLK_FREQ_60FPS_PREV     47450000L
#define PCLK_FREQ_60FPS_PREV_TD  34800000L
#define PCLK_FREQ_SNAP           53380000L
#define PCLK_FREQ_SNAP_BURST  	 33473684L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */	
#define PCLK_FREQ_SNAP_BURST_UP  25440000L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */	
#else
#define PCLK_FREQ_24FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_24FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP           522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	 522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP  522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_24FPS_PREV    1
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HEN_60FPS_PREV    1
#else
#define RESHAPE_HEN_24FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_24FPS_PREV  298
#define RESHAPE_HRISE_24FPS_PREV  300
#define RESHAPE_HFALL_30FPS_PREV  298
#define RESHAPE_HRISE_30FPS_PREV  300
#define RESHAPE_HFALL_60FPS_PREV  298
#define RESHAPE_HRISE_60FPS_PREV  300
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_24FPS_PREV    1
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VEN_60FPS_PREV    1
#else
#define RESHAPE_VEN_24FPS_PREV    0	//bypass v don't enable
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_24FPS_PREV  2
#define RESHAPE_VRISE_24FPS_PREV  3
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  22
#define RESHAPE_HRISE_SNAP  24
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

/* preview h,v sync inv. */
#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

/* snapshot h,v sync inv. */
#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_BGBGRR
#define BAYER_PTN_PREV_60FPS  FRONT_BAYER_PTN_BGBGRR
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_BGBGRR

/* i2c interface. */
#define I2C_DEV_ADDR  0x6C
#define I2C_CLK_DIV   1

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_24FPS_24_MAX_IDX  189
#define AE_24FPS_24_MIN_IDX  0
#define AE_24FPS_20_MAX_IDX  189
#define AE_24FPS_20_MIN_IDX  0
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  184
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  182
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  169
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  162
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_24FPS_MAX_IDX  48
#define GAIN_24FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  48
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  48
#define GAIN_60FPS_MIN_IDX  0

/* iso gain. */
#define ISO_100_GAIN_IDX	0
#define ISO_200_GAIN_IDX	16
#define ISO_400_GAIN_IDX	32
#define ISO_800_GAIN_IDX	48

/* iso capture auto de-banding */
#define ISO_CAPTURE_AUTO_DEBANDING	1


/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				/* FRONT_SRAM_MODE_ENABLE : image width <= 1280 */
																/* FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor */

/* exposure, gain setting position */
#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		/* AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, */
																/* AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling, */
																/* AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising. */

/* gain setting position */
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					/* AE_GAIN_AFTER_EXP_OFF, */
																/* AE_GAIN_AFTER_EXP_ON */

/* awb. */
#define EV10_index 25//30
#define EV9_index 15//20
#define EV8_index 5//10
#define EV7_index 0

#define AWBMaxGain 355
#define AWBMinGain 64

#define STB_RGain  0x54
#define STB_GrGain 0x3f
#define STB_BGain  0x58
#define STB_GbGain 0x40

/* shutter control pin. */
#define FREX_PIN_BYTE    GPIO_BYTE_GEN1
#define FREX_PIN_BIT     (1 << (14 & 0x07))
#define EXPSTB_PIN_BYTE  GPIO_BYTE_GEN1
#define EXPSTB_PIN_BIT   (1 << (15 & 0x07))
#define SHTA_PIN_BYTE    GPIO_BYTE_GEN2
#define SHTA_PIN_BIT     (1 << (16 & 0x07))
#define SHTB_PIN_BYTE    GPIO_BYTE_GEN2
#define SHTB_PIN_BIT     (1 << (17 & 0x07))

#define FLASHLIGHT_USE    0

#define K_FlashLightOn    0
#define K_FlashLightOff   1
#define K_FlashLightAuto  2


/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
/* function type definition. */
#define __STATIC	//static

/* operating function. */
#define frontOpen                                   frontSensorOpen
#define frontOpDevNameGet                           frontSensorOpDevNameGet
#define frontOpFrameRateSet                         frontSensorOpFrameRateSet
#define frontOpCapabilityGet                        frontSensorOpCapabilityGet
#define frontOpIntrCapabilityGet                    frontSensorOpIntrCapabilityGet
#define frontOpPowerOn                              frontSensorOpPowerOn
#define frontOpPowerOff                             frontSensorOpPowerOff
#define frontOpPreviewModeSet                       frontSensorOpPreviewModeSet
#define frontOpPreviewSensorZoomModeSet             frontSensorOpPreviewSensorZoomModeSet
#define frontOpIntrPreviewSensorZoomModeSet         frontSensorOpIntrPreviewSensorZoomModeSet
#define frontOpSnapModeSet                          frontSensorOpSnapModeSet
#define frontOpSnapShot                             frontSensorOpSnapShot
#define frontOpGainSet                              frontSensorOpGainSet
#define frontOpExposureTimeSet                      frontSensorOpExposureTimeSet
#define frontOpIntrGainSet                          frontSensorOpIntrGainSet     
#define frontOpIntrExposureTimeSet                  frontSensorOpIntrExposureTimeSet
#define frontOpOpticalBlackStatusGet                frontSensorOpOpticalBlackStatusGet
#define frontOpOpticalBlackCalibrate                frontSensorOpOpticalBlackCalibrate
#define frontOpShutterOpen                          frontSensorOpShutterOpen
#define frontOpShutterClose                         frontSensorOpShutterClose
#define frontOpIntrPreviewDynamicTempoalDenoiseSet  frontSensorOpIntrPreviewDynamicTempoalDenoiseSet

/* extern operating function. */
#define frontOpAeTargetLuminanceSet                 frontSensorOpAeTargetLuminanceSet
#define frontOpAeExposureCompensationSet            frontSensorOpAeExposureCompensationSet
#define frontOpAeFrameRateSet                       frontSensorOpAeFrameRateSet
#define frontOpAwbModeSet                           frontSensorOpAwbModeSet
#define frontOpAfterSnapShot                        frontSensorOpAfterSnapShot


/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* operating function. */
__STATIC UINT8 *frontOpDevNameGet(void);
__STATIC void frontOpFrameRateSet(UINT8 fps);
__STATIC void frontOpCapabilityGet(frontCapabilityArg_t *parg);
__STATIC void frontOpIntrCapabilityGet(frontCapabilityArg_t *parg);
__STATIC void frontOpPowerOn(void);
__STATIC void frontOpPowerOff(void);
__STATIC void frontOpPreviewModeSet(void);
__STATIC void frontOpPreviewSensorZoomModeSet(UINT16 factor);
__STATIC void frontOpIntrPreviewSensorZoomModeSet(UINT16 factor);
__STATIC void frontOpSnapModeSet(void);
__STATIC void frontOpSnapShot(UINT8 first, UINT8 mode, UINT8 scaleUp);
__STATIC void frontOpGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC void frontOpIntrGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpIntrExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC UINT8 frontOpOpticalBlackStatusGet(void);
__STATIC UINT8 frontOpOpticalBlackCalibrate(void);
__STATIC void frontOpShutterOpen(void);
__STATIC void frontOpShutterClose(void);
__STATIC void frontOpIntrPreviewDynamicTempoalDenoiseSet(UINT8 en);

/* extern operating function. */
__STATIC void frontOpAeTargetLuminanceSet(void *parg);
__STATIC void frontOpAeExposureCompensationSet(void *parg);
__STATIC void frontOpAeFrameRateSet(void *parg);
__STATIC void frontOpAwbModeSet(void *parg);
__STATIC void frontOpAfterSnapShot(void *parg);

/* local function. */
static void frontBeforePowerOn(void);
static void frontAfterPowerOn(void);
static void frontBeforePowerOff(void);
static void frontAfterPowerOff(void);
static void frontSnapExposureTimeSet(UINT8 mode,UINT8 scaleUp);
static void frontBeforeSnapExposureTimeSet(void);
static void frontGainTblValueTransfer(UINT16 value,UINT16 *numerator,UINT16 *denominator);
static void frontPreviewModeSwitchPostProc(void);
static void frontResTblBinRead(UINT16 fileId,	UINT16 byteAddress);
static void frontResCmdBinSend(UINT16 fileId);
static void OV5633FlashWBSet(void);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5633 RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table	
 **************************************************************************/
#define CVT_DEF  extern

/* Tag for convert start */
#if (LOAD_RES_OPTION == 1)
CVT_DEF UINT8 TAG_CVT_S;
#endif

/* ae table resource file id  */
typedef enum CvtTypeToX_u_0 {
	//_ae15fps50hzTbl = 0x00C0,
	//_ae15fps60hzTbl = 0x00C1,
	//_ae20fps50hzTbl = 0x00C2,
	//_ae20fps60hzTbl = 0x00C3,
	_ae24fps50hzTbl = 0x00C4,
	_ae24fps60hzTbl = 0x00C5,
	_ae30fps50hzTbl = 0x00C6,
	_ae30fps60hzTbl = 0x00C7,
	_ae60fps50hzTbl = 0x00C8,
	_ae60fps60hzTbl = 0x00C9,
} _aeTbl;

/* gain table resource file id */
typedef enum CvtTypeToX_e {
	_gainTbl = 0x00CA,
};

/* i2c initial cmd table resource file id */
typedef enum CvtTypeRmv_e {
	_frontInit0CmdTbl		= 0x00CB,
	_frontInit1CmdTbl		= 0x00CC,
	//_frontInit2CmdTbl		= 0x00CD,
	//_frontInit3CmdTbl		= 0x00CE,
	//_frontInit4CmdTbl		= 0x00CF,
	//_front15fpsPrevCmdTbl	= 0x00D0,
	//_front20fpsPrevCmdTbl	= 0x00D1,
	_front24fpsPrevCmdTbl	= 0x00D2,
	_front30fpsPrevCmdTbl	= 0x00D3,
	_front60fpsPrevCmdTbl	= 0x00D4,
	_frontSnapCmdTbl		= 0x00D5,
};

/* ae table & gain table link address */
typedef enum CvtTypeLnkAddr_e {
	__aeTbl = 0xa000,
	__gainTbl = 0xb000,
};

/* Tag for convert end */
CVT_DEF UINT8 TAG_CVT_E;

/**************************************************************************
 *                            AE TABLE
 **************************************************************************/
/**************************************************************************
 *				banding calculate
 *			50Hz : 1/(shutter/10) < 1/(2*50)
 *			60Hz : 1/(shutter/10) < 1/(2*60)
 **************************************************************************/
/* ae table. */

static code frontAe_t ae24fps50hzTbl[190] = {
	{	0,	249,	48,	0,	0 },
	{	1,	249,	47,	0,	0 },
	{	2,	249,	45,	0,	0 },
	{	3,	249,	44,	0,	0 },
	{	4,	249,	43,	0,	0 },
	{	5,	249,	42,	0,	0 },
	{	6,	249,	41,	0,	0 },
	{	7,	249,	40,	0,	0 },
	{	8,	249,	39,	0,	0 },
	{	9,	249,	38,	0,	0 },
	{	10,	249,	37,	0,	0 },
	{	11,	249,	36,	0,	0 },
	{	12,	249,	35,	0,	0 },
	{	13,	249,	34,	0,	0 },
	{	14,	249,	33,	0,	0 },
	{	15,	249,	32,	0,	0 },
	{	16,	249,	31,	0,	0 },
	{	17,	249,	29,	0,	0 },
	{	18,	249,	28,	0,	0 },
	{	19,	249,	27,	0,	0 },
	{	20,	332,	33,	0,	0 },
	{	21,	332,	31,	0,	0 },
	{	22,	332,	30,	0,	0 },
	{	23,	332,	29,	0,	0 },
	{	24,	332,	28,	0,	0 },
	{	25,	332,	26,	0,	0 },
	{	26,	332,	25,	0,	0 },
	{	27,	332,	24,	0,	0 },
	{	28,	332,	23,	0,	0 },
	{	29,	332,	22,	0,	0 },
	{	30,	332,	21,	0,	0 },
	{	31,	332,	20,	0,	0 },
	{	32,	332,	19,	0,	0 },
	{	33,	332,	18,	0,	0 },
	{	34,	332,	17,	0,	0 },
	{	35,	332,	15,	0,	0 },
	{	36,	332,	14,	0,	0 },
	{	37,	332,	13,	0,	0 },
	{	38,	332,	12,	0,	0 },
	{	39,	332,	10,	0,	0 },
	{	40,	332,	9,	0,	0 },
	{	41,	332,	8,	0,	0 },
	{	42,	332,	7,	0,	0 },
	{	43,	332,	6,	0,	0 },
	{	44,	332,	5,	0,	0 },
	{	45,	332,	4,	0,	0 },
	{	46,	332,	3,	0,	0 },
	{	47,	332,	2,	0,	0 },
	{	48,	332,	1,	0,	0 },
	{	49,	332,	0,	0,	0 },
	{	50,	498,	7,	0,	0 },
	{	51,	498,	6,	0,	0 },
	{	52,	498,	5,	0,	0 },
	{	53,	498,	4,	0,	0 },
	{	54,	498,	3,	0,	0 },
	{	55,	498,	2,	0,	0 },
	{	56,	498,	1,	0,	0 },
	{	57,	498,	0,	0,	0 },
	{	58,	501,	0,	0,	0 },
	{	59,	992,	13,	0,	0 },
	{	60,	992,	12,	0,	0 },
	{	61,	992,	11,	0,	0 },
	{	62,	992,	10,	0,	0 },
	{	63,	992,	9,	0,	0 },
	{	64,	992,	8,	0,	0 },
	{	65,	992,	7,	0,	0 },
	{	66,	992,	6,	0,	0 },
	{	67,	992,	5,	0,	0 },
	{	68,	992,	4,	0,	0 },
	{	69,	992,	3,	0,	0 },
	{	70,	992,	2,	0,	0 },
	{	71,	992,	1,	0,	0 },
	{	72,	992,	0,	0,	0 },
	{	73,	1002,	0,	0,	0 },
	{	74,	1047,	0,	0,	0 },
	{	75,	1093,	0,	0,	0 },
	{	76,	1143,	0,	0,	0 },
	{	77,	1193,	0,	0,	0 },
	{	78,	1247,	0,	0,	0 },
	{	79,	1301,	0,	0,	0 },
	{	80,	1360,	0,	0,	0 },
	{	81,	1418,	0,	0,	0 },
	{	82,	1481,	0,	0,	0 },
	{	83,	1549,	0,	0,	0 },
	{	84,	1616,	0,	0,	0 },
	{	85,	1689,	0,	0,	0 },
	{	86,	1768,	0,	0,	0 },
	{	87,	1844,	0,	0,	0 },
	{	88,	1927,	0,	0,	0 },
	{	89,	2004,	0,	0,	0 },
	{	90,	2102,	0,	0,	0 },
	{	91,	2194,	0,	0,	0 },
	{	92,	2294,	0,	0,	0 },
	{	93,	2385,	0,	0,	0 },
	{	94,	2505,	0,	0,	0 },
	{	95,	2614,	0,	0,	0 },
	{	96,	2732,	0,	0,	0 },
	{	97,	2835,	0,	0,	0 },
	{	98,	2976,	0,	0,	0 },
	{	99,	3099,	0,	0,	0 },
	{	100,	3232,	0,	0,	0 },
	{	101,	3377,	0,	0,	0 },
	{	102,	3536,	0,	0,	0 },
	{	103,	3711,	0,	0,	0 },
	{	104,	3853,	0,	0,	0 },
	{	105,	4008,	0,	0,	0 },
	{	106,	4233,	0,	0,	0 },
	{	107,	4420,	0,	0,	0 },
	{	108,	4624,	0,	0,	0 },
	{	109,	4771,	0,	0,	0 },
	{	110,	5009,	0,	0,	0 },
	{	111,	5273,	0,	0,	0 },
	{	112,	5465,	0,	0,	0 },
	{	113,	5671,	0,	0,	0 },
	{	114,	6011,	0,	0,	0 },
	{	115,	6262,	0,	0,	0 },
	{	116,	6534,	0,	0,	0 },
	{	117,	6831,	0,	0,	0 },
	{	118,	7156,	0,	0,	0 },
	{	119,	7514,	0,	0,	1 },
	{	120,	7707,	0,	0,	0 },
	{	121,	8123,	0,	0,	0 },
	{	122,	8587,	0,	0,	0 },
	{	123,	8840,	0,	0,	1 },
	{	124,	9393,	0,	0,	1 },
	{	125,	9696,	0,	0,	1 },
	{	126,	10019,	0,	0,	0 },
	{	127,	10734,	0,	0,	0 },
	{	128,	11132,	0,	0,	1 },
	{	129,	11560,	0,	0,	0 },
	{	130,	12022,	0,	0,	1 },
	{	131,	12523,	0,	0,	0 },
	{	132,	13068,	0,	0,	1 },
	{	133,	13662,	0,	0,	0 },
	{	134,	14312,	0,	0,	1 },
	{	135,	15028,	0,	0,	2 },
	{	136,	15819,	0,	0,	1 },
	{	137,	16698,	0,	0,	2 },
	{	138,	17680,	0,	0,	1 },
	{	139,	18785,	0,	0,	2 },
	{	140,	20037,	0,	0,	0 },
	{	141,	21469,	0,	0,	2 },
	{	142,	23120,	1,	0,	0 },
	{	143,	23120,	0,	0,	0 },
	{	144,	25047,	0,	0,	2 },
	{	145,	27324,	0,	0,	0 },
	{	146,	30056,	1,	0,	2 },
	{	147,	30056,	0,	0,	2 },
	{	148,	33396,	1,	0,	4 },
	{	149,	33396,	0,	0,	4 },
	{	150,	37570,	1,	0,	2 },
	{	151,	37570,	0,	0,	2 },
	{	152,	42937,	1,	0,	5 },
	{	153,	42937,	0,	0,	4 },
	{	154,	50093,	2,	0,	2 },
	{	155,	50093,	1,	0,	2 },
	{	156,	50093,	0,	0,	2 },
	{	157,	60111,	2,	0,	7 },
	{	158,	60111,	1,	0,	6 },
	{	159,	60111,	0,	0,	6 },
	{	160,	75139,	3,	0,	2 },
	{	161,	75139,	2,	0,	2 },
	{	162,	75139,	1,	0,	2 },
	{	163,	75139,	0,	0,	2 },
	{	164,	100184,	4,	0,	13 },
	{	165,	100184,	3,	0,	12 },
	{	166,	100184,	2,	0,	12 },
	{	167,	100184,	1,	0,	11 },
	{	168,	100184,	0,	0,	10 },
	{	169,	150274,	6,	0,	2 },
	{	170,	150274,	5,	0,	2 },
	{	171,	150274,	4,	0,	2 },
	{	172,	150274,	3,	0,	2 },
	{	173,	150274,	2,	0,	2 },
	{	174,	150274,	1,	0,	2 },
	{	175,	150274,	0,	0,	2 },
	{	176,	300533,	14,	0,	0 },
	{	177,	300533,	13,	0,	0 },
	{	178,	300533,	12,	0,	0 },
	{	179,	300533,	11,	0,	0 },
	{	180,	300533,	9,	0,	0 },
	{	181,	300533,	8,	0,	0 },
	{	182,	300533,	7,	0,	0 },
	{	183,	300533,	6,	0,	0 },
	{	184,	300533,	5,	0,	0 },
	{	185,	300533,	4,	0,	0 },
	{	186,	300533,	3,	0,	0 },
	{	187,	300533,	2,	0,	0 },
	{	188,	300533,	1,	0,	0 },
	{	189,	300533,	0,	0,	0 },
};

static code frontAe_t ae24fps60hzTbl[190] = {
	{	0,	240,	48,	0,	0 },
	{	1,	240,	47,	0,	0 },
	{	2,	240,	45,	0,	0 },
	{	3,	240,	44,	0,	0 },
	{	4,	240,	43,	0,	0 },
	{	5,	240,	42,	0,	0 },
	{	6,	240,	41,	0,	0 },
	{	7,	240,	40,	0,	0 },
	{	8,	240,	39,	0,	0 },
	{	9,	240,	38,	0,	0 },
	{	10,	240,	37,	0,	0 },
	{	11,	240,	36,	0,	0 },
	{	12,	240,	35,	0,	0 },
	{	13,	240,	34,	0,	0 },
	{	14,	240,	33,	0,	0 },
	{	15,	240,	32,	0,	0 },
	{	16,	240,	31,	0,	0 },
	{	17,	240,	29,	0,	0 },
	{	18,	240,	28,	0,	0 },
	{	19,	299,	33,	0,	0 },
	{	20,	299,	32,	0,	0 },
	{	21,	299,	31,	0,	0 },
	{	22,	299,	30,	0,	0 },
	{	23,	299,	28,	0,	0 },
	{	24,	299,	27,	0,	0 },
	{	25,	299,	26,	0,	0 },
	{	26,	299,	25,	0,	0 },
	{	27,	299,	24,	0,	0 },
	{	28,	299,	23,	0,	0 },
	{	29,	299,	22,	0,	0 },
	{	30,	299,	21,	0,	0 },
	{	31,	299,	20,	0,	0 },
	{	32,	299,	19,	0,	0 },
	{	33,	299,	18,	0,	0 },
	{	34,	299,	17,	0,	0 },
	{	35,	299,	16,	0,	0 },
	{	36,	299,	15,	0,	0 },
	{	37,	299,	14,	0,	0 },
	{	38,	299,	12,	0,	0 },
	{	39,	299,	11,	0,	0 },
	{	40,	299,	10,	0,	0 },
	{	41,	299,	9,	0,	0 },
	{	42,	299,	8,	0,	0 },
	{	43,	299,	7,	0,	0 },
	{	44,	299,	6,	0,	0 },
	{	45,	299,	5,	0,	0 },
	{	46,	299,	4,	0,	0 },
	{	47,	299,	3,	0,	0 },
	{	48,	299,	2,	0,	0 },
	{	49,	299,	1,	0,	0 },
	{	50,	299,	0,	0,	0 },
	{	51,	399,	4,	0,	0 },
	{	52,	399,	3,	0,	0 },
	{	53,	399,	2,	0,	0 },
	{	54,	399,	1,	0,	0 },
	{	55,	399,	0,	0,	0 },
	{	56,	598,	7,	0,	0 },
	{	57,	598,	6,	0,	0 },
	{	58,	598,	5,	0,	0 },
	{	59,	598,	4,	0,	0 },
	{	60,	598,	3,	0,	0 },
	{	61,	598,	2,	0,	0 },
	{	62,	598,	1,	0,	0 },
	{	63,	598,	0,	0,	0 },
	{	64,	1193,	13,	0,	0 },
	{	65,	1193,	12,	0,	0 },
	{	66,	1193,	11,	0,	0 },
	{	67,	1193,	10,	0,	0 },
	{	68,	1193,	9,	0,	0 },
	{	69,	1193,	8,	0,	0 },
	{	70,	1193,	7,	0,	0 },
	{	71,	1193,	6,	0,	0 },
	{	72,	1193,	5,	0,	0 },
	{	73,	1193,	4,	0,	0 },
	{	74,	1193,	3,	0,	0 },
	{	75,	1193,	2,	0,	0 },
	{	76,	1193,	1,	0,	0 },
	{	77,	1193,	0,	0,	0 },
	{	78,	1247,	0,	0,	0 },
	{	79,	1301,	0,	0,	0 },
	{	80,	1360,	0,	0,	0 },
	{	81,	1418,	0,	0,	0 },
	{	82,	1481,	0,	0,	0 },
	{	83,	1549,	0,	0,	0 },
	{	84,	1616,	0,	0,	0 },
	{	85,	1689,	0,	0,	0 },
	{	86,	1768,	0,	0,	0 },
	{	87,	1844,	0,	0,	0 },
	{	88,	1927,	0,	0,	0 },
	{	89,	2004,	0,	0,	0 },
	{	90,	2102,	0,	0,	0 },
	{	91,	2194,	0,	0,	0 },
	{	92,	2294,	0,	0,	0 },
	{	93,	2385,	0,	0,	0 },
	{	94,	2505,	0,	0,	0 },
	{	95,	2614,	0,	0,	0 },
	{	96,	2732,	0,	0,	0 },
	{	97,	2835,	0,	0,	0 },
	{	98,	2976,	0,	0,	0 },
	{	99,	3099,	0,	0,	0 },
	{	100,	3232,	0,	0,	0 },
	{	101,	3377,	0,	0,	0 },
	{	102,	3536,	0,	0,	0 },
	{	103,	3711,	0,	0,	0 },
	{	104,	3853,	0,	0,	0 },
	{	105,	4008,	0,	0,	0 },
	{	106,	4233,	0,	0,	0 },
	{	107,	4420,	0,	0,	0 },
	{	108,	4624,	0,	0,	0 },
	{	109,	4771,	0,	0,	0 },
	{	110,	5009,	0,	0,	0 },
	{	111,	5273,	0,	0,	0 },
	{	112,	5465,	0,	0,	0 },
	{	113,	5671,	0,	0,	0 },
	{	114,	6011,	0,	0,	0 },
	{	115,	6262,	0,	0,	0 },
	{	116,	6534,	0,	0,	0 },
	{	117,	6831,	0,	0,	0 },
	{	118,	7156,	0,	0,	0 },
	{	119,	7514,	0,	0,	1 },
	{	120,	7707,	0,	0,	0 },
	{	121,	8123,	0,	0,	0 },
	{	122,	8587,	0,	0,	0 },
	{	123,	8840,	0,	0,	1 },
	{	124,	9393,	0,	0,	1 },
	{	125,	9696,	0,	0,	1 },
	{	126,	10019,	0,	0,	0 },
	{	127,	10734,	0,	0,	0 },
	{	128,	11132,	0,	0,	1 },
	{	129,	11560,	0,	0,	0 },
	{	130,	12022,	0,	0,	1 },
	{	131,	12523,	0,	0,	0 },
	{	132,	13068,	0,	0,	1 },
	{	133,	13662,	0,	0,	0 },
	{	134,	14312,	0,	0,	1 },
	{	135,	15028,	0,	0,	2 },
	{	136,	15819,	0,	0,	1 },
	{	137,	16698,	0,	0,	2 },
	{	138,	17680,	0,	0,	1 },
	{	139,	18785,	0,	0,	2 },
	{	140,	20037,	0,	0,	0 },
	{	141,	21469,	0,	0,	2 },
	{	142,	23120,	1,	0,	0 },
	{	143,	23120,	0,	0,	0 },
	{	144,	25047,	0,	0,	2 },
	{	145,	27324,	0,	0,	0 },
	{	146,	30056,	1,	0,	2 },
	{	147,	30056,	0,	0,	2 },
	{	148,	33396,	1,	0,	4 },
	{	149,	33396,	0,	0,	4 },
	{	150,	37570,	1,	0,	2 },
	{	151,	37570,	0,	0,	2 },
	{	152,	42937,	1,	0,	5 },
	{	153,	42937,	0,	0,	4 },
	{	154,	50093,	2,	0,	2 },
	{	155,	50093,	1,	0,	2 },
	{	156,	50093,	0,	0,	2 },
	{	157,	60111,	2,	0,	7 },
	{	158,	60111,	1,	0,	6 },
	{	159,	60111,	0,	0,	6 },
	{	160,	75139,	3,	0,	2 },
	{	161,	75139,	2,	0,	2 },
	{	162,	75139,	1,	0,	2 },
	{	163,	75139,	0,	0,	2 },
	{	164,	100184,	4,	0,	13 },
	{	165,	100184,	3,	0,	12 },
	{	166,	100184,	2,	0,	12 },
	{	167,	100184,	1,	0,	11 },
	{	168,	100184,	0,	0,	10 },
	{	169,	150274,	6,	0,	2 },
	{	170,	150274,	5,	0,	2 },
	{	171,	150274,	4,	0,	2 },
	{	172,	150274,	3,	0,	2 },
	{	173,	150274,	2,	0,	2 },
	{	174,	150274,	1,	0,	2 },
	{	175,	150274,	0,	0,	2 },
	{	176,	300533,	14,	0,	0 },
	{	177,	300533,	13,	0,	0 },
	{	178,	300533,	12,	0,	0 },
	{	179,	300533,	11,	0,	0 },
	{	180,	300533,	9,	0,	0 },
	{	181,	300533,	8,	0,	0 },
	{	182,	300533,	7,	0,	0 },
	{	183,	300533,	6,	0,	0 },
	{	184,	300533,	5,	0,	0 },
	{	185,	300533,	4,	0,	0 },
	{	186,	300533,	3,	0,	0 },
	{	187,	300533,	2,	0,	0 },
	{	188,	300533,	1,	0,	0 },
	{	189,	300533,	0,	0,	0 },
};

static code frontAe_t ae30fps50hzTbl[183] = {
	{	0,	332,	47,	0,	0 },
	{	1,	332,	46,	0,	0 },
	{	2,	332,	45,	0,	0 },
	{	3,	332,	44,	0,	0 },
	{	4,	332,	42,	0,	0 },
	{	5,	332,	41,	0,	0 },
	{	6,	332,	40,	0,	0 },
	{	7,	332,	39,	0,	0 },
	{	8,	332,	38,	0,	0 },
	{	9,	332,	37,	0,	0 },
	{	10,	332,	36,	0,	0 },
	{	11,	332,	35,	0,	0 },
	{	12,	332,	34,	0,	0 },
	{	13,	332,	33,	0,	0 },
	{	14,	332,	31,	0,	0 },
	{	15,	332,	30,	0,	0 },
	{	16,	332,	29,	0,	0 },
	{	17,	332,	28,	0,	0 },
	{	18,	332,	26,	0,	0 },
	{	19,	332,	25,	0,	0 },
	{	20,	332,	24,	0,	0 },
	{	21,	332,	23,	0,	0 },
	{	22,	332,	22,	0,	0 },
	{	23,	332,	21,	0,	0 },
	{	24,	332,	20,	0,	0 },
	{	25,	332,	19,	0,	0 },
	{	26,	332,	18,	0,	0 },
	{	27,	332,	17,	0,	0 },
	{	28,	332,	15,	0,	0 },
	{	29,	332,	14,	0,	0 },
	{	30,	332,	13,	0,	0 },
	{	31,	332,	12,	0,	0 },
	{	32,	332,	10,	0,	0 },
	{	33,	332,	9,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	7,	0,	0 },
	{	36,	332,	6,	0,	0 },
	{	37,	332,	5,	0,	0 },
	{	38,	332,	4,	0,	0 },
	{	39,	332,	3,	0,	0 },
	{	40,	332,	2,	0,	0 },
	{	41,	332,	1,	0,	0 },
	{	42,	332,	0,	0,	0 },
	{	43,	498,	7,	0,	0 },
	{	44,	498,	6,	0,	0 },
	{	45,	498,	5,	0,	0 },
	{	46,	498,	4,	0,	0 },
	{	47,	498,	3,	0,	0 },
	{	48,	498,	2,	0,	0 },
	{	49,	498,	1,	0,	0 },
	{	50,	498,	0,	0,	0 },
	{	51,	501,	0,	0,	0 },
	{	52,	992,	13,	0,	0 },
	{	53,	992,	12,	0,	0 },
	{	54,	992,	11,	0,	0 },
	{	55,	992,	10,	0,	0 },
	{	56,	992,	9,	0,	0 },
	{	57,	992,	8,	0,	0 },
	{	58,	992,	7,	0,	0 },
	{	59,	992,	6,	0,	0 },
	{	60,	992,	5,	0,	0 },
	{	61,	992,	4,	0,	0 },
	{	62,	992,	3,	0,	0 },
	{	63,	992,	2,	0,	0 },
	{	64,	992,	1,	0,	0 },
	{	65,	992,	0,	0,	0 },
	{	66,	1002,	0,	0,	0 },
	{	67,	1047,	0,	0,	0 },
	{	68,	1093,	0,	0,	0 },
	{	69,	1143,	0,	0,	0 },
	{	70,	1193,	0,	0,	0 },
	{	71,	1247,	0,	0,	0 },
	{	72,	1301,	0,	0,	0 },
	{	73,	1360,	0,	0,	0 },
	{	74,	1418,	0,	0,	0 },
	{	75,	1481,	0,	0,	0 },
	{	76,	1549,	0,	0,	0 },
	{	77,	1616,	0,	0,	0 },
	{	78,	1689,	0,	0,	0 },
	{	79,	1768,	0,	0,	0 },
	{	80,	1844,	0,	0,	0 },
	{	81,	1927,	0,	0,	0 },
	{	82,	2004,	0,	0,	0 },
	{	83,	2102,	0,	0,	0 },
	{	84,	2194,	0,	0,	0 },
	{	85,	2294,	0,	0,	0 },
	{	86,	2385,	0,	0,	0 },
	{	87,	2505,	0,	0,	0 },
	{	88,	2614,	0,	0,	0 },
	{	89,	2732,	0,	0,	0 },
	{	90,	2835,	0,	0,	0 },
	{	91,	2976,	0,	0,	0 },
	{	92,	3099,	0,	0,	0 },
	{	93,	3232,	0,	0,	0 },
	{	94,	3377,	0,	0,	0 },
	{	95,	3536,	0,	0,	0 },
	{	96,	3711,	0,	0,	0 },
	{	97,	3853,	0,	0,	0 },
	{	98,	4008,	0,	0,	0 },
	{	99,	4233,	0,	0,	0 },
	{	100,	4420,	0,	0,	0 },
	{	101,	4624,	0,	0,	0 },
	{	102,	4771,	0,	0,	0 },
	{	103,	5009,	0,	0,	0 },
	{	104,	5273,	0,	0,	0 },
	{	105,	5465,	0,	0,	0 },
	{	106,	5671,	0,	0,	0 },
	{	107,	6011,	0,	0,	0 },
	{	108,	6262,	0,	0,	0 },
	{	109,	6534,	0,	0,	0 },
	{	110,	6831,	0,	0,	0 },
	{	111,	7156,	0,	0,	0 },
	{	112,	7514,	0,	0,	1 },
	{	113,	7707,	0,	0,	0 },
	{	114,	8123,	0,	0,	0 },
	{	115,	8587,	0,	0,	0 },
	{	116,	8840,	0,	0,	1 },
	{	117,	9393,	0,	0,	1 },
	{	118,	9696,	0,	0,	1 },
	{	119,	10019,	0,	0,	0 },
	{	120,	10734,	0,	0,	0 },
	{	121,	11132,	0,	0,	1 },
	{	122,	11560,	0,	0,	0 },
	{	123,	12022,	0,	0,	1 },
	{	124,	12523,	0,	0,	0 },
	{	125,	13068,	0,	0,	1 },
	{	126,	13662,	0,	0,	0 },
	{	127,	14312,	0,	0,	1 },
	{	128,	15028,	0,	0,	2 },
	{	129,	15819,	0,	0,	1 },
	{	130,	16698,	0,	0,	2 },
	{	131,	17680,	0,	0,	1 },
	{	132,	18785,	0,	0,	2 },
	{	133,	20037,	0,	0,	0 },
	{	134,	21469,	0,	0,	2 },
	{	135,	23120,	1,	0,	0 },
	{	136,	23120,	0,	0,	0 },
	{	137,	25047,	0,	0,	2 },
	{	138,	27324,	0,	0,	0 },
	{	139,	30056,	1,	0,	2 },
	{	140,	30056,	0,	0,	2 },
	{	141,	33396,	1,	0,	4 },
	{	142,	33396,	0,	0,	4 },
	{	143,	37570,	1,	0,	2 },
	{	144,	37570,	0,	0,	2 },
	{	145,	42937,	1,	0,	5 },
	{	146,	42937,	0,	0,	4 },
	{	147,	50093,	2,	0,	2 },
	{	148,	50093,	1,	0,	2 },
	{	149,	50093,	0,	0,	2 },
	{	150,	60111,	2,	0,	7 },
	{	151,	60111,	1,	0,	6 },
	{	152,	60111,	0,	0,	6 },
	{	153,	75139,	3,	0,	2 },
	{	154,	75139,	2,	0,	2 },
	{	155,	75139,	1,	0,	2 },
	{	156,	75139,	0,	0,	2 },
	{	157,	100184,	4,	0,	13 },
	{	158,	100184,	3,	0,	12 },
	{	159,	100184,	2,	0,	12 },
	{	160,	100184,	1,	0,	11 },
	{	161,	100184,	0,	0,	10 },
	{	162,	150274,	6,	0,	2 },
	{	163,	150274,	5,	0,	2 },
	{	164,	150274,	4,	0,	2 },
	{	165,	150274,	3,	0,	2 },
	{	166,	150274,	2,	0,	2 },
	{	167,	150274,	1,	0,	2 },
	{	168,	150274,	0,	0,	2 },
	{	169,	300533,	14,	0,	0 },
	{	170,	300533,	13,	0,	0 },
	{	171,	300533,	12,	0,	0 },
	{	172,	300533,	11,	0,	0 },
	{	173,	300533,	9,	0,	0 },
	{	174,	300533,	8,	0,	0 },
	{	175,	300533,	7,	0,	0 },
	{	176,	300533,	6,	0,	0 },
	{	177,	300533,	5,	0,	0 },
	{	178,	300533,	4,	0,	0 },
	{	179,	300533,	3,	0,	0 },
	{	180,	300533,	2,	0,	0 },
	{	181,	300533,	1,	0,	0 },
	{	182,	300533,	0,	0,	0 },
};

static code frontAe_t ae30fps60hzTbl[185] = {
	{	0,	299,	48,	0,	0 },
	{	1,	299,	47,	0,	0 },
	{	2,	299,	46,	0,	0 },
	{	3,	299,	44,	0,	0 },
	{	4,	299,	43,	0,	0 },
	{	5,	299,	42,	0,	0 },
	{	6,	299,	41,	0,	0 },
	{	7,	299,	40,	0,	0 },
	{	8,	299,	39,	0,	0 },
	{	9,	299,	38,	0,	0 },
	{	10,	299,	37,	0,	0 },
	{	11,	299,	36,	0,	0 },
	{	12,	299,	35,	0,	0 },
	{	13,	299,	34,	0,	0 },
	{	14,	299,	33,	0,	0 },
	{	15,	299,	32,	0,	0 },
	{	16,	299,	31,	0,	0 },
	{	17,	299,	30,	0,	0 },
	{	18,	299,	28,	0,	0 },
	{	19,	299,	27,	0,	0 },
	{	20,	299,	26,	0,	0 },
	{	21,	299,	25,	0,	0 },
	{	22,	299,	24,	0,	0 },
	{	23,	299,	23,	0,	0 },
	{	24,	299,	22,	0,	0 },
	{	25,	299,	21,	0,	0 },
	{	26,	299,	20,	0,	0 },
	{	27,	299,	19,	0,	0 },
	{	28,	299,	18,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	16,	0,	0 },
	{	31,	299,	15,	0,	0 },
	{	32,	299,	14,	0,	0 },
	{	33,	299,	12,	0,	0 },
	{	34,	299,	11,	0,	0 },
	{	35,	299,	10,	0,	0 },
	{	36,	299,	9,	0,	0 },
	{	37,	299,	8,	0,	0 },
	{	38,	299,	7,	0,	0 },
	{	39,	299,	6,	0,	0 },
	{	40,	299,	5,	0,	0 },
	{	41,	299,	4,	0,	0 },
	{	42,	299,	3,	0,	0 },
	{	43,	299,	2,	0,	0 },
	{	44,	299,	1,	0,	0 },
	{	45,	299,	0,	0,	0 },
	{	46,	399,	4,	0,	0 },
	{	47,	399,	3,	0,	0 },
	{	48,	399,	2,	0,	0 },
	{	49,	399,	1,	0,	0 },
	{	50,	399,	0,	0,	0 },
	{	51,	598,	7,	0,	0 },
	{	52,	598,	6,	0,	0 },
	{	53,	598,	5,	0,	0 },
	{	54,	598,	4,	0,	0 },
	{	55,	598,	3,	0,	0 },
	{	56,	598,	2,	0,	0 },
	{	57,	598,	1,	0,	0 },
	{	58,	598,	0,	0,	0 },
	{	59,	1193,	13,	0,	0 },
	{	60,	1193,	12,	0,	0 },
	{	61,	1193,	11,	0,	0 },
	{	62,	1193,	10,	0,	0 },
	{	63,	1193,	9,	0,	0 },
	{	64,	1193,	8,	0,	0 },
	{	65,	1193,	7,	0,	0 },
	{	66,	1193,	6,	0,	0 },
	{	67,	1193,	5,	0,	0 },
	{	68,	1193,	4,	0,	0 },
	{	69,	1193,	3,	0,	0 },
	{	70,	1193,	2,	0,	0 },
	{	71,	1193,	1,	0,	0 },
	{	72,	1193,	0,	0,	0 },
	{	73,	1247,	0,	0,	0 },
	{	74,	1301,	0,	0,	0 },
	{	75,	1360,	0,	0,	0 },
	{	76,	1418,	0,	0,	0 },
	{	77,	1481,	0,	0,	0 },
	{	78,	1549,	0,	0,	0 },
	{	79,	1616,	0,	0,	0 },
	{	80,	1689,	0,	0,	0 },
	{	81,	1768,	0,	0,	0 },
	{	82,	1844,	0,	0,	0 },
	{	83,	1927,	0,	0,	0 },
	{	84,	2004,	0,	0,	0 },
	{	85,	2102,	0,	0,	0 },
	{	86,	2194,	0,	0,	0 },
	{	87,	2294,	0,	0,	0 },
	{	88,	2385,	0,	0,	0 },
	{	89,	2505,	0,	0,	0 },
	{	90,	2614,	0,	0,	0 },
	{	91,	2732,	0,	0,	0 },
	{	92,	2835,	0,	0,	0 },
	{	93,	2976,	0,	0,	0 },
	{	94,	3099,	0,	0,	0 },
	{	95,	3232,	0,	0,	0 },
	{	96,	3377,	0,	0,	0 },
	{	97,	3536,	0,	0,	0 },
	{	98,	3711,	0,	0,	0 },
	{	99,	3853,	0,	0,	0 },
	{	100,	4008,	0,	0,	0 },
	{	101,	4233,	0,	0,	0 },
	{	102,	4420,	0,	0,	0 },
	{	103,	4624,	0,	0,	0 },
	{	104,	4771,	0,	0,	0 },
	{	105,	5009,	0,	0,	0 },
	{	106,	5273,	0,	0,	0 },
	{	107,	5465,	0,	0,	0 },
	{	108,	5671,	0,	0,	0 },
	{	109,	6011,	0,	0,	0 },
	{	110,	6262,	0,	0,	0 },
	{	111,	6534,	0,	0,	0 },
	{	112,	6831,	0,	0,	0 },
	{	113,	7156,	0,	0,	0 },
	{	114,	7514,	0,	0,	1 },
	{	115,	7707,	0,	0,	0 },
	{	116,	8123,	0,	0,	0 },
	{	117,	8587,	0,	0,	0 },
	{	118,	8840,	0,	0,	1 },
	{	119,	9393,	0,	0,	1 },
	{	120,	9696,	0,	0,	1 },
	{	121,	10019,	0,	0,	0 },
	{	122,	10734,	0,	0,	0 },
	{	123,	11132,	0,	0,	1 },
	{	124,	11560,	0,	0,	0 },
	{	125,	12022,	0,	0,	1 },
	{	126,	12523,	0,	0,	0 },
	{	127,	13068,	0,	0,	1 },
	{	128,	13662,	0,	0,	0 },
	{	129,	14312,	0,	0,	1 },
	{	130,	15028,	0,	0,	2 },
	{	131,	15819,	0,	0,	1 },
	{	132,	16698,	0,	0,	2 },
	{	133,	17680,	0,	0,	1 },
	{	134,	18785,	0,	0,	2 },
	{	135,	20037,	0,	0,	0 },
	{	136,	21469,	0,	0,	2 },
	{	137,	23120,	1,	0,	0 },
	{	138,	23120,	0,	0,	0 },
	{	139,	25047,	0,	0,	2 },
	{	140,	27324,	0,	0,	0 },
	{	141,	30056,	1,	0,	2 },
	{	142,	30056,	0,	0,	2 },
	{	143,	33396,	1,	0,	4 },
	{	144,	33396,	0,	0,	4 },
	{	145,	37570,	1,	0,	2 },
	{	146,	37570,	0,	0,	2 },
	{	147,	42937,	1,	0,	5 },
	{	148,	42937,	0,	0,	4 },
	{	149,	50093,	2,	0,	2 },
	{	150,	50093,	1,	0,	2 },
	{	151,	50093,	0,	0,	2 },
	{	152,	60111,	2,	0,	7 },
	{	153,	60111,	1,	0,	6 },
	{	154,	60111,	0,	0,	6 },
	{	155,	75139,	3,	0,	2 },
	{	156,	75139,	2,	0,	2 },
	{	157,	75139,	1,	0,	2 },
	{	158,	75139,	0,	0,	2 },
	{	159,	100184,	4,	0,	13 },
	{	160,	100184,	3,	0,	12 },
	{	161,	100184,	2,	0,	12 },
	{	162,	100184,	1,	0,	11 },
	{	163,	100184,	0,	0,	10 },
	{	164,	150274,	6,	0,	2 },
	{	165,	150274,	5,	0,	2 },
	{	166,	150274,	4,	0,	2 },
	{	167,	150274,	3,	0,	2 },
	{	168,	150274,	2,	0,	2 },
	{	169,	150274,	1,	0,	2 },
	{	170,	150274,	0,	0,	2 },
	{	171,	300533,	14,	0,	0 },
	{	172,	300533,	13,	0,	0 },
	{	173,	300533,	12,	0,	0 },
	{	174,	300533,	11,	0,	0 },
	{	175,	300533,	9,	0,	0 },
	{	176,	300533,	8,	0,	0 },
	{	177,	300533,	7,	0,	0 },
	{	178,	300533,	6,	0,	0 },
	{	179,	300533,	5,	0,	0 },
	{	180,	300533,	4,	0,	0 },
	{	181,	300533,	3,	0,	0 },
	{	182,	300533,	2,	0,	0 },
	{	183,	300533,	1,	0,	0 },
	{	184,	300533,	0,	0,	0 },
};                             
  
static code frontAe_t ae60fps50hzTbl[163] = {
	{	0,	991,	48,	0,	0 },
	{	1,	991,	47,	0,	0 },
	{	2,	991,	46,	0,	0 },
	{	3,	991,	45,	0,	0 },
	{	4,	991,	43,	0,	0 },
	{	5,	991,	42,	0,	0 },
	{	6,	991,	41,	0,	0 },
	{	7,	991,	40,	0,	0 },
	{	8,	991,	39,	0,	0 },
	{	9,	991,	38,	0,	0 },
	{	10,	991,	37,	0,	0 },
	{	11,	991,	36,	0,	0 },
	{	12,	991,	35,	0,	0 },
	{	13,	991,	34,	0,	0 },
	{	14,	991,	33,	0,	0 },
	{	15,	991,	32,	0,	0 },
	{	16,	991,	31,	0,	0 },
	{	17,	991,	30,	0,	0 },
	{	18,	991,	29,	0,	0 },
	{	19,	991,	27,	0,	0 },
	{	20,	991,	26,	0,	0 },
	{	21,	991,	25,	0,	0 },
	{	22,	991,	24,	0,	0 },
	{	23,	991,	23,	0,	0 },
	{	24,	991,	22,	0,	0 },
	{	25,	991,	21,	0,	0 },
	{	26,	991,	20,	0,	0 },
	{	27,	991,	19,	0,	0 },
	{	28,	991,	18,	0,	0 },
	{	29,	991,	17,	0,	0 },
	{	30,	991,	16,	0,	0 },
	{	31,	991,	15,	0,	0 },
	{	32,	991,	14,	0,	0 },
	{	33,	991,	13,	0,	0 },
	{	34,	991,	11,	0,	0 },
	{	35,	991,	10,	0,	0 },
	{	36,	991,	9,	0,	0 },
	{	37,	991,	8,	0,	0 },
	{	38,	991,	7,	0,	0 },
	{	39,	991,	6,	0,	0 },
	{	40,	991,	5,	0,	0 },
	{	41,	991,	4,	0,	0 },
	{	42,	991,	3,	0,	0 },
	{	43,	991,	2,	0,	0 },
	{	44,	991,	1,	0,	0 },
	{	45,	991,	0,	0,	0 },
	{	46,	1020,	0,	0,	0 },
	{	47,	1067,	0,	0,	0 },
	{	48,	1113,	0,	0,	0 },
	{	49,	1164,	0,	0,	0 },
	{	50,	1215,	0,	0,	0 },
	{	51,	1270,	0,	0,	0 },
	{	52,	1325,	0,	0,	0 },
	{	53,	1385,	0,	0,	0 },
	{	54,	1444,	0,	0,	0 },
	{	55,	1508,	0,	0,	0 },
	{	56,	1578,	0,	0,	0 },
	{	57,	1646,	0,	0,	0 },
	{	58,	1720,	0,	0,	0 },
	{	59,	1801,	0,	0,	0 },
	{	60,	1878,	0,	0,	0 },
	{	61,	1962,	0,	0,	0 },
	{	62,	2041,	0,	0,	0 },
	{	63,	2141,	0,	0,	0 },
	{	64,	2235,	0,	0,	0 },
	{	65,	2337,	0,	0,	0 },
	{	66,	2430,	0,	0,	0 },
	{	67,	2551,	0,	0,	0 },
	{	68,	2662,	0,	0,	0 },
	{	69,	2783,	0,	0,	0 },
	{	70,	2888,	0,	0,	0 },
	{	71,	3031,	0,	0,	0 },
	{	72,	3156,	0,	0,	0 },
	{	73,	3292,	0,	0,	0 },
	{	74,	3440,	0,	0,	0 },
	{	75,	3602,	0,	0,	0 },
	{	76,	3779,	0,	0,	0 },
	{	77,	3925,	0,	0,	0 },
	{	78,	4082,	0,	0,	0 },
	{	79,	4312,	0,	0,	0 },
	{	80,	4502,	0,	0,	0 },
	{	81,	4710,	0,	0,	0 },
	{	82,	4859,	0,	0,	0 },
	{	83,	5102,	0,	0,	0 },
	{	84,	5371,	0,	0,	0 },
	{	85,	5566,	0,	0,	0 },
	{	86,	5776,	0,	0,	0 },
	{	87,	6123,	0,	0,	1 },
	{	88,	6378,	0,	0,	1 },
	{	89,	6655,	0,	0,	1 },
	{	90,	6957,	0,	0,	0 },
	{	91,	7289,	0,	0,	0 },
	{	92,	7653,	0,	0,	0 },
	{	93,	7849,	0,	0,	1 },
	{	94,	8274,	0,	0,	1 },
	{	95,	8747,	0,	0,	1 },
	{	96,	9004,	0,	0,	0 },
	{	97,	9567,	0,	0,	0 },
	{	98,	9875,	0,	0,	1 },
	{	99,	10204,	0,	0,	0 },
	{	100,	10933,	0,	0,	0 },
	{	101,	11338,	0,	0,	1 },
	{	102,	11774,	0,	0,	1 },
	{	103,	12245,	0,	0,	1 },
	{	104,	12755,	0,	0,	1 },
	{	105,	13310,	0,	0,	1 },
	{	106,	13915,	0,	0,	1 },
	{	107,	14578,	0,	0,	0 },
	{	108,	15306,	0,	0,	1 },
	{	109,	16112,	0,	0,	0 },
	{	110,	17007,	0,	0,	1 },
	{	111,	18007,	0,	0,	0 },
	{	112,	19133,	0,	0,	1 },
	{	113,	20408,	0,	0,	0 },
	{	114,	21866,	0,	0,	1 },
	{	115,	23548,	1,	0,	3 },
	{	116,	23548,	0,	0,	3 },
	{	117,	25511,	0,	0,	1 },
	{	118,	27830,	0,	0,	3 },
	{	119,	30613,	1,	0,	1 },
	{	120,	30613,	0,	0,	1 },
	{	121,	34014,	1,	0,	4 },
	{	122,	34014,	0,	0,	3 },
	{	123,	38266,	1,	0,	1 },
	{	124,	38266,	0,	0,	1 },
	{	125,	43732,	1,	0,	4 },
	{	126,	43732,	0,	0,	4 },
	{	127,	51021,	2,	0,	1 },
	{	128,	51021,	1,	0,	1 },
	{	129,	51021,	0,	0,	1 },
	{	130,	61225,	2,	0,	6 },
	{	131,	61225,	1,	0,	6 },
	{	132,	61225,	0,	0,	6 },
	{	133,	76530,	3,	0,	1 },
	{	134,	76530,	2,	0,	1 },
	{	135,	76530,	1,	0,	1 },
	{	136,	76530,	0,	0,	1 },
	{	137,	102040,	4,	0,	12 },
	{	138,	102040,	3,	0,	12 },
	{	139,	102040,	2,	0,	11 },
	{	140,	102040,	1,	0,	10 },
	{	141,	102040,	0,	0,	10 },
	{	142,	153057,	6,	0,	2 },
	{	143,	153057,	5,	0,	2 },
	{	144,	153057,	4,	0,	2 },
	{	145,	153057,	3,	0,	1 },
	{	146,	153057,	2,	0,	1 },
	{	147,	153057,	1,	0,	1 },
	{	148,	153057,	0,	0,	1 },
	{	149,	306098,	14,	0,	0 },
	{	150,	306098,	13,	0,	0 },
	{	151,	306098,	12,	0,	0 },
	{	152,	306098,	11,	0,	0 },
	{	153,	306098,	9,	0,	0 },
	{	154,	306098,	8,	0,	0 },
	{	155,	306098,	7,	0,	0 },
	{	156,	306098,	6,	0,	0 },
	{	157,	306098,	5,	0,	0 },
	{	158,	306098,	4,	0,	0 },
	{	159,	306098,	3,	0,	0 },
	{	160,	306098,	2,	0,	0 },
	{	161,	306098,	1,	0,	0 },
	{	162,	306098,	0,	0,	0 },
};

static code frontAe_t ae60fps60hzTbl[170] = {
	{	0,	598,	48,	0,	0 },
	{	1,	598,	46,	0,	0 },
	{	2,	598,	45,	0,	0 },
	{	3,	598,	44,	0,	0 },
	{	4,	598,	43,	0,	0 },
	{	5,	598,	41,	0,	0 },
	{	6,	598,	40,	0,	0 },
	{	7,	598,	39,	0,	0 },
	{	8,	598,	38,	0,	0 },
	{	9,	598,	37,	0,	0 },
	{	10,	598,	36,	0,	0 },
	{	11,	598,	35,	0,	0 },
	{	12,	598,	34,	0,	0 },
	{	13,	598,	33,	0,	0 },
	{	14,	598,	32,	0,	0 },
	{	15,	598,	30,	0,	0 },
	{	16,	598,	29,	0,	0 },
	{	17,	598,	28,	0,	0 },
	{	18,	598,	27,	0,	0 },
	{	19,	598,	25,	0,	0 },
	{	20,	598,	24,	0,	0 },
	{	21,	598,	23,	0,	0 },
	{	22,	598,	22,	0,	0 },
	{	23,	598,	21,	0,	0 },
	{	24,	598,	20,	0,	0 },
	{	25,	598,	19,	0,	0 },
	{	26,	598,	18,	0,	0 },
	{	27,	598,	17,	0,	0 },
	{	28,	598,	16,	0,	0 },
	{	29,	598,	14,	0,	0 },
	{	30,	598,	13,	0,	0 },
	{	31,	598,	12,	0,	0 },
	{	32,	598,	11,	0,	0 },
	{	33,	598,	9,	0,	0 },
	{	34,	598,	8,	0,	0 },
	{	35,	598,	7,	0,	0 },
	{	36,	598,	6,	0,	0 },
	{	37,	598,	5,	0,	0 },
	{	38,	598,	4,	0,	0 },
	{	39,	598,	3,	0,	0 },
	{	40,	598,	2,	0,	0 },
	{	41,	598,	1,	0,	0 },
	{	42,	598,	0,	0,	0 },
	{	43,	1191,	14,	0,	0 },
	{	44,	1191,	13,	0,	0 },
	{	45,	1191,	12,	0,	0 },
	{	46,	1191,	10,	0,	0 },
	{	47,	1191,	9,	0,	0 },
	{	48,	1191,	8,	0,	0 },
	{	49,	1191,	7,	0,	0 },
	{	50,	1191,	6,	0,	0 },
	{	51,	1191,	5,	0,	0 },
	{	52,	1191,	4,	0,	0 },
	{	53,	1191,	3,	0,	0 },
	{	54,	1191,	2,	0,	0 },
	{	55,	1191,	1,	0,	0 },
	{	56,	1191,	0,	0,	0 },
	{	57,	1215,	0,	0,	0 },
	{	58,	1270,	0,	0,	0 },
	{	59,	1325,	0,	0,	0 },
	{	60,	1385,	0,	0,	0 },
	{	61,	1444,	0,	0,	0 },
	{	62,	1508,	0,	0,	0 },
	{	63,	1578,	0,	0,	0 },
	{	64,	1646,	0,	0,	0 },
	{	65,	1720,	0,	0,	0 },
	{	66,	1801,	0,	0,	0 },
	{	67,	1878,	0,	0,	0 },
	{	68,	1962,	0,	0,	0 },
	{	69,	2041,	0,	0,	0 },
	{	70,	2141,	0,	0,	0 },
	{	71,	2235,	0,	0,	0 },
	{	72,	2337,	0,	0,	0 },
	{	73,	2430,	0,	0,	0 },
	{	74,	2551,	0,	0,	0 },
	{	75,	2662,	0,	0,	0 },
	{	76,	2783,	0,	0,	0 },
	{	77,	2888,	0,	0,	0 },
	{	78,	3031,	0,	0,	0 },
	{	79,	3156,	0,	0,	0 },
	{	80,	3292,	0,	0,	0 },
	{	81,	3440,	0,	0,	0 },
	{	82,	3602,	0,	0,	0 },
	{	83,	3779,	0,	0,	0 },
	{	84,	3925,	0,	0,	0 },
	{	85,	4082,	0,	0,	0 },
	{	86,	4312,	0,	0,	0 },
	{	87,	4502,	0,	0,	0 },
	{	88,	4710,	0,	0,	0 },
	{	89,	4859,	0,	0,	0 },
	{	90,	5102,	0,	0,	0 },
	{	91,	5371,	0,	0,	0 },
	{	92,	5566,	0,	0,	0 },
	{	93,	5776,	0,	0,	0 },
	{	94,	6123,	0,	0,	1 },
	{	95,	6378,	0,	0,	1 },
	{	96,	6655,	0,	0,	1 },
	{	97,	6957,	0,	0,	0 },
	{	98,	7289,	0,	0,	0 },
	{	99,	7653,	0,	0,	0 },
	{	100,	7849,	0,	0,	1 },
	{	101,	8274,	0,	0,	1 },
	{	102,	8747,	0,	0,	1 },
	{	103,	9004,	0,	0,	0 },
	{	104,	9567,	0,	0,	0 },
	{	105,	9875,	0,	0,	1 },
	{	106,	10204,	0,	0,	0 },
	{	107,	10933,	0,	0,	0 },
	{	108,	11338,	0,	0,	1 },
	{	109,	11774,	0,	0,	1 },
	{	110,	12245,	0,	0,	1 },
	{	111,	12755,	0,	0,	1 },
	{	112,	13310,	0,	0,	1 },
	{	113,	13915,	0,	0,	1 },
	{	114,	14578,	0,	0,	0 },
	{	115,	15306,	0,	0,	1 },
	{	116,	16112,	0,	0,	0 },
	{	117,	17007,	0,	0,	1 },
	{	118,	18007,	0,	0,	0 },
	{	119,	19133,	0,	0,	1 },
	{	120,	20408,	0,	0,	0 },
	{	121,	21866,	0,	0,	1 },
	{	122,	23548,	1,	0,	3 },
	{	123,	23548,	0,	0,	3 },
	{	124,	25511,	0,	0,	1 },
	{	125,	27830,	0,	0,	3 },
	{	126,	30613,	1,	0,	1 },
	{	127,	30613,	0,	0,	1 },
	{	128,	34014,	1,	0,	4 },
	{	129,	34014,	0,	0,	3 },
	{	130,	38266,	1,	0,	1 },
	{	131,	38266,	0,	0,	1 },
	{	132,	43732,	1,	0,	4 },
	{	133,	43732,	0,	0,	4 },
	{	134,	51021,	2,	0,	1 },
	{	135,	51021,	1,	0,	1 },
	{	136,	51021,	0,	0,	1 },
	{	137,	61225,	2,	0,	6 },
	{	138,	61225,	1,	0,	6 },
	{	139,	61225,	0,	0,	6 },
	{	140,	76530,	3,	0,	1 },
	{	141,	76530,	2,	0,	1 },
	{	142,	76530,	1,	0,	1 },
	{	143,	76530,	0,	0,	1 },
	{	144,	102040,	4,	0,	12 },
	{	145,	102040,	3,	0,	12 },
	{	146,	102040,	2,	0,	11 },
	{	147,	102040,	1,	0,	10 },
	{	148,	102040,	0,	0,	10 },
	{	149,	153057,	6,	0,	2 },
	{	150,	153057,	5,	0,	2 },
	{	151,	153057,	4,	0,	2 },
	{	152,	153057,	3,	0,	1 },
	{	153,	153057,	2,	0,	1 },
	{	154,	153057,	1,	0,	1 },
	{	155,	153057,	0,	0,	1 },
	{	156,	306098,	14,	0,	0 },
	{	157,	306098,	13,	0,	0 },
	{	158,	306098,	12,	0,	0 },
	{	159,	306098,	11,	0,	0 },
	{	160,	306098,	9,	0,	0 },
	{	161,	306098,	8,	0,	0 },
	{	162,	306098,	7,	0,	0 },
	{	163,	306098,	6,	0,	0 },
	{	164,	306098,	5,	0,	0 },
	{	165,	306098,	4,	0,	0 },
	{	166,	306098,	3,	0,	0 },
	{	167,	306098,	2,	0,	0 },
	{	168,	306098,	1,	0,	0 },
	{	169,	306098,	0,	0,	0 },
};

/* gain table. */
static code UINT16 gainTbl[49] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9, 
   10,    11,    12,    13,    14,    15,    16,    17,    18,    19, 
   20,    21,    22,    23,    24,    25,    26,    27,    28,    29, 
   30,    31,    48,    49,    50,    51,    52,    53,    54,    55, 
   56,    57,    58,    59,    60,    61,    62,    63,   112, 	
};

/* exposure time index table. */
static code UINT8 ae24fpsMaxIdxTbl[2] = {
	AE_24FPS_24_MAX_IDX,
	AE_24FPS_20_MAX_IDX,
};

static code UINT8 ae24fpsMinIdxTbl[2] = {
	AE_24FPS_24_MIN_IDX,
	AE_24FPS_20_MIN_IDX,
};

static code UINT8 ae30fpsMaxIdxTbl[2] = {
	AE_30FPS_30_MAX_IDX,
	AE_30FPS_25_MAX_IDX,
};

static code UINT8 ae30fpsMinIdxTbl[2] = {
	AE_30FPS_30_MIN_IDX,
	AE_30FPS_25_MIN_IDX,
};

static code UINT8 ae60fpsMaxIdxTbl[2] = {
	AE_60FPS_60_MAX_IDX,
	AE_60FPS_50_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_60_MIN_IDX,
	AE_60FPS_50_MIN_IDX,
};

/* preview to snapshot binning rate table. */
static code UINT8 frontPrevToSnapBinningRateTbl[PREV_MODE_TOT] = {
	PREV_TO_SNAP_BINNING_RATE_24FPS_PREV,
	PREV_TO_SNAP_BINNING_RATE_30FPS_PREV,
	PREV_TO_SNAP_BINNING_RATE_60FPS_PREV,
};

/* preview tempoal denoise table. */
static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl[PREV_MODE_TOT] = {
	{  /* 24 fps. */
		MCLK_DIV_24FPS_PREV + 5,
		MCLK_PHASE_24FPS_PREV,
		PCLK_DIV_24FPS_PREV + 5,
		PCLK_PHASE_24FPS_PREV,
		PCLK_FREQ_24FPS_PREV_TD,
		31,
		6,
	},
	{  /* 30 fps. */
		MCLK_DIV_30FPS_PREV + 5,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV + 5,
		PCLK_PHASE_30FPS_PREV,
		PCLK_FREQ_30FPS_PREV_TD,
		31,
		6,
	},
	{  /* 60 fps. */
		MCLK_DIV_60FPS_PREV + 4,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV + 4,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV_TD,
		31,
		6,
	},
};

/* device capability. */
static code frontPrevCapabDesc_t frontPrevCapabDesc[PREV_MODE_TOT] = {
	{  /* 24 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_24FPS_PREV,
		HEIGHT_24FPS_PREV,
		XOFF_24FPS_PREV,
		YOFF_24FPS_PREV,
		LINE_TOT_24FPS_PREV,
		FRAME_TOT_24FPS_PREV,
		MCLK_DIV_24FPS_PREV,
		MCLK_PHASE_24FPS_PREV,
		PCLK_DIV_24FPS_PREV,
		PCLK_PHASE_24FPS_PREV,
		PCLK_FREQ_24FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_24FPS_PREV,
		RESHAPE_HFALL_24FPS_PREV,
		RESHAPE_HRISE_24FPS_PREV,
		RESHAPE_VEN_24FPS_PREV,
		RESHAPE_VFALL_24FPS_PREV,
		RESHAPE_VRISE_24FPS_PREV,
		ae24fps60hzTbl,
		ae24fps50hzTbl,
		ae24fpsMaxIdxTbl,
		ae24fpsMinIdxTbl,
		EV10_24FPS_EXP_IDX,
		gainTbl,
		GAIN_24FPS_MAX_IDX,
		GAIN_24FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[0],
		SENSOR_ZOOM_FACTOR_24FPS_PREV,
	},
	{  /* 30 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_30FPS_PREV,
		HEIGHT_30FPS_PREV,
		XOFF_30FPS_PREV,
		YOFF_30FPS_PREV,
		LINE_TOT_30FPS_PREV,
		FRAME_TOT_30FPS_PREV,
		MCLK_DIV_30FPS_PREV,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV,
		PCLK_PHASE_30FPS_PREV,
		PCLK_FREQ_30FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_30FPS_PREV,
		RESHAPE_HFALL_30FPS_PREV,
		RESHAPE_HRISE_30FPS_PREV,
		RESHAPE_VEN_30FPS_PREV,
		RESHAPE_VFALL_30FPS_PREV,
		RESHAPE_VRISE_30FPS_PREV,
		ae30fps60hzTbl,
		ae30fps50hzTbl,
		ae30fpsMaxIdxTbl,
		ae30fpsMinIdxTbl,
		EV10_30FPS_EXP_IDX,
		gainTbl,
		GAIN_30FPS_MAX_IDX,
		GAIN_30FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[1],
		SENSOR_ZOOM_FACTOR_30FPS_PREV,
	},
	{  /* 60 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV_60FPS,
		WIDTH_60FPS_PREV,
		HEIGHT_60FPS_PREV,
		XOFF_60FPS_PREV,
		YOFF_60FPS_PREV,
		LINE_TOT_60FPS_PREV,
		FRAME_TOT_60FPS_PREV,
		MCLK_DIV_60FPS_PREV,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_60FPS_PREV,
		RESHAPE_HFALL_60FPS_PREV,
		RESHAPE_HRISE_60FPS_PREV,
		RESHAPE_VEN_60FPS_PREV,
		RESHAPE_VFALL_60FPS_PREV,
		RESHAPE_VRISE_60FPS_PREV,
		ae60fps60hzTbl,
		ae60fps50hzTbl,
		ae60fpsMaxIdxTbl,
		ae60fpsMinIdxTbl,
		EV10_60FPS_EXP_IDX,
		gainTbl,
		GAIN_60FPS_MAX_IDX,
		GAIN_60FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[2],
		SENSOR_ZOOM_FACTOR_60FPS_PREV,
	},
};

static code frontSnapCapabDesc_t frontSnapCapabDesc[SNAP_MODE_TOT] = {
	{  /* 30/60 fps. */
		BAYER_PTN_SNAP,
		WIDTH_SNAP,
		HEIGHT_SNAP,
		XOFF_SNAP,
		YOFF_SNAP,
		LINE_TOT_SNAP,
		FRAME_TOT_SNAP,
		MCLK_DIV_SNAP,
		MCLK_DIV_SNAP_BURST,
		MCLK_DIV_SNAP_BURST_UP,
		MCLK_PHASE_SNAP,
		PCLK_DIV_SNAP,
		PCLK_PHASE_SNAP,
		PCLK_FREQ_SNAP,
		PCLK_FREQ_SNAP_BURST,
		PCLK_FREQ_SNAP_BURST_UP,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_SNAP,
		RESHAPE_HFALL_SNAP,
		RESHAPE_HRISE_SNAP,
		RESHAPE_VEN_SNAP,
		RESHAPE_VFALL_SNAP,
		RESHAPE_VRISE_SNAP,
	},
};

/* i2c command table. */
static code UINT8 frontInit0CmdTbl[] = {
	0x30, 0x12, 0x80,  /* Soft Reset delay 100ms */
};

static code UINT8 frontInit1CmdTbl[] = {
	0x30, 0xfa, 0x00,
	0x30, 0x0f, 0x00,
	0x30, 0xb2, 0x32,
	0x30, 0x84, 0x40,
	0x30, 0x16, 0x01,
	0x30, 0x8a, 0x25,
	//0x30,0x13,0xff,
	0x30, 0x15, 0x03,
	0x30, 0xbf, 0x02,
	0x30, 0x69, 0x00,
	0x30, 0x65, 0x50,
	0x30, 0x68, 0x08,
	0x30, 0xac, 0x05,
	0x30, 0x9d, 0x00,
	0x30, 0x9e, 0x24,
	0x30, 0x98, 0x5c,
	0x30, 0x91, 0x04,
	0x30, 0x75, 0x22,
	0x30, 0x76, 0x23,
	0x30, 0x77, 0x24,
	0x30, 0x78, 0x25,
	0x30, 0xb5, 0x0c,
	0x30, 0x90, 0x6f,
	0x30, 0xf9, 0x11,
	0x33, 0x11, 0x80,
	0x33, 0x12, 0x1f,
	0x31, 0x03, 0x10,
	0x30, 0x5c, 0x01,
	0x30, 0x5d, 0x29,
	0x30, 0x5e, 0x00,
	0x30, 0x5f, 0xf7,
	0x30, 0x8d, 0x0b,
	0x30, 0xad, 0x20,
	0x30, 0x72, 0x0d,
	0x30, 0x8b, 0x82,
	0x33, 0x17, 0x9c,
	0x33, 0x18, 0x22,
	0x30, 0x25, 0x20,
	0x30, 0x27, 0x08,
	0x30, 0x29, 0x3f,
	0x30, 0x2b, 0xa3,
	0x33, 0x19, 0x22,
	0x30, 0xa1, 0xc4,
	0x30, 0x6a, 0x05,
	0x33, 0x15, 0x22,
	0x30, 0xae, 0x25,
	0x33, 0x04, 0x40,
	0x30, 0x99, 0x49,
	0x30, 0xbe, 0x50,
	0x30, 0x92, 0x02,
	0x30, 0x0e, 0xb0,//pll
	0x30, 0x0f, 0x00,//pll
	0x30, 0x10, 0x03,//pll
	0x30, 0x11, 0x40,//pll
	0x30, 0xf8, 0x00,//image transform VSUB full, HSUB full
	0x30, 0xf8, 0x01,//image transform VSUB full, HSUB 1:2
	0x30, 0xf8, 0x09,
	0x30, 0x83, 0x00,
	0x30, 0x86, 0x24,
	0x30, 0x8c, 0x88,
	0x30, 0xf9, 0x13,
	0x30, 0xfb, 0x81,
	0x30, 0x8a, 0x26,
	0x30, 0x16, 0x02,
	
	0x30, 0x69, 0x80,  /* BLC */ 
	0x30, 0x9d, 0x04,
	0x30, 0x9e, 0x24,
	0x30, 0x71, 0x34,
	0x33, 0x1f, 0x22,
	0x30, 0x98, 0x54,
	0x33, 0x1e, 0x07,
	0x33, 0x22, 0x40,
	0x33, 0x23, 0x40,
	0x33, 0x1e, 0x05,
	0x30, 0x48, 0x1f,
	0x30, 0x49, 0x4e,
	0x30, 0x4a, 0x20,
	0x30, 0x4b, 0x02,
	0x30, 0x4c, 0x00,
	0x30, 0x4d, 0x20,
	0x30, 0x4e, 0x42,
	0x30, 0xa9, 0x02,
	0x30, 0xa4, 0x11,
	//0x30,0x13,0xf7,
	0x30, 0x14, 0x44,
	0x30, 0x5c, 0x01,
	0x30, 0x5d, 0x29,
	0x30, 0x5e, 0x00,
	0x30, 0x5f, 0xf7,
	0x30, 0x4b, 0x00,
	0x30, 0x4e, 0x62,
	0x30, 0x4a, 0x30,
	0x30, 0x4d, 0xff,
	0x30, 0x58, 0x07,
	0x30, 0x59, 0x16,
	0x30, 0x5a, 0x1a,
	0x30, 0xe9, 0x04,
	0x30, 0x90, 0x6f,
	0x30, 0x84, 0x44,
	0x30, 0xe9, 0x04,
	0x30, 0xb5, 0x1c,
	0x33, 0x1f, 0x22,
	0x30, 0x20, 0x02,//frame length 516
	0x30, 0x21, 0x04,//frame length
	0x30, 0x22, 0x07,//line length 1920
	0x30, 0x23, 0x80,//line length
	0x30, 0x24, 0x00,//x addr start 44
	0x30, 0x25, 0x2c,//x addr start
	0x30, 0x26, 0x00,//y addr start 20
	0x30, 0x27, 0x14,//y addr start
	0x30, 0x28, 0x0a,//x addr end 2607
	0x30, 0x29, 0x2f,//x addr end
	0x30, 0x2a, 0x07,//y addr end 1955
	0x30, 0x2b, 0xa3,//y addr end
	0x30, 0x2c, 0x02,//x output size 640
	0x30, 0x2d, 0x80,//x output size
	0x30, 0x2e, 0x01,//y output size 480
	0x30, 0x2f, 0xe0,//y output size
	0x30, 0xb3, 0x09,
	0x33, 0x01, 0xc1,
	0x33, 0x13, 0xf1,
	0x33, 0x14, 0x05,
	0x33, 0x15, 0x04,
	0x33, 0x16, 0x01,
	0x33, 0x17, 0xe4,
	0x33, 0x18, 0x20,
	0x30, 0x0f, 0x01,
	0x30, 0x22, 0x08,
	0x30, 0x23, 0x10,
	                  
	//0x33, 0x00, 0xfb,
    0x33, 0x00, 0xd3,  //disable AEC/AGC/AWB    
	0x33, 0x02, 0xe0,
	0x33, 0x08, 0x04,
	0x33, 0x09, 0x00,
	0x33, 0x0a, 0x04,
	0x33, 0x0b, 0x00,
	0x33, 0x0c, 0x04,
	0x33, 0x0d, 0x00,
	0x30, 0xae, 0x15,
	0x33, 0x04, 0x4c,
	0x33, 0x11, 0x81,
	0x33, 0x22, 0x10,
	0x33, 0x23, 0x10,
	0x30, 0xfa, 0x01,
					 
	0x30, 0x13, 0x00,													  
#if 0							   
	 0x30, 0x0e, 0xb0,							  
	 0x30, 0x0f, 0x11,   /* 11=30fps, 01= 60fps */  
	 0x30, 0x10, 0x07,							  
	 0x30, 0x11, 0x40,							  
#endif											
	 0x30, 0xbf, 0x11,												
																	
	 0x30, 0x00, 0x30,												
	 0x30, 0x02, 0x02,												
																	
	 0x35, 0x06, 0x30,	/* 30=Hsync out , 20=HREF out */				
	 0x35, 0x00, 0x00,	/* [2]= PCLK, [1]=Href, [0]=Vsync polarity	*/

	 0x30,0xfa,0x01,
};

static code UINT8 front24fpsPrevCmdTbl[] = {
	//	0x30, 0xfa, 0x00,	 /*sw sleep on		 */
	//	0x30,0x89,0x0a,  /* bayer pattern */
	//	0x30,0xb4,0x00,  
	//	0x35,0x06,0x30,  /* HSYNC Mode */
	//	0x35,0x01,0x01, 
	//	0x35,0x0e,0x40,  /* Vsync from Sensor */

	#if (PREV_RESP==960) //960p

	0x30, 0xfa, 0x00, /*sw sleep on */
#if 0
	0x30, 0x20, 0x03, /* VD width 996 lines*/ 			 
	0x30, 0x21, 0xe4, 			 
	0x30, 0x22, 0x07, /* HD width 1920 pclk*/			 
	0x30, 0x23, 0x80, 			 
#else
	0x30, 0x20, 0x03, /* VD width 981 lines*/				
	0x30, 0x21, 0xd5,				
	0x30, 0x22, 0x06, /* HD width 1776 pclk*/			
	0x30, 0x23, 0xf0,				
#endif

	0x30, 0x24, 0x00, /* H Start */			 
	0x30, 0x25, 0x30,		
	0x30, 0x26, 0x00, /* V Start */			 
	0x30, 0x27, 0x10, 			 
	0x30, 0x28, 0x0a, /* H End */			 
	0x30, 0x29, 0x2f, 			 
	0x30, 0x2a, 0x07, /* V End */			 
	0x30, 0x2b, 0x97, 			 

	0x30, 0x2c, 0x05, /*image size */ 			 
	0x30, 0x2d, 0x00, 			 
	0x30, 0x2e, 0x03, 			 
	0x30, 0x2f, 0xc0, 			 

	0x30, 0xb3, 0x00, 			 
	0x33, 0x01, 0xc0, 			 
	0x33, 0x13, 0xf0, 			 
	0x33, 0x14, 0x05, 			 
	0x33, 0x15, 0x02, 			 
	0x33, 0x16, 0x03, 			 
	0x33, 0x17, 0xc4, 			 
	0x33, 0x18, 0x22, 			 
						 
	0x30, 0xf8, 0x05, 			 
	0x30, 0x83, 0x08, 			 
	0x30, 0x86, 0x00, 			 
	0x30, 0x8c, 0x80, 			 
	0x30, 0xf9, 0x11, 			 
	0x30, 0xfb, 0x80, 			 
	0x30, 0x8a, 0x25, 			 
	0x30, 0x16, 0x01, 			 

	0x30, 0x0e, 0xb7,  /*54 Mhz */
	0x30, 0x0f, 0x00,  /* 00 for 30fps PCLK=54MHz , 10 for 15fps PCLK=27MHz, */
	0x30, 0x10, 0x07, 			 
	0x30, 0x11, 0x40, 			 	   	

	0x30, 0x89, 0x0a,  /* bayer pattern */


	0x30, 0xb4, 0x00,  
	0x35, 0x06, 0x30,  /* HSYNC Mode */
	0x35, 0x01, 0x01, 

	0x30, 0xfa, 0x01,  /* sleep off */
	
#elif (PREV_RESP==720)
						 
	0x30, 0xfa, 0x00, /*sw sleep on */
#if 0
	0x30, 0x20, 0x03, /* VD width 996 lines*/ 			 
	0x30, 0x21, 0xe4, 			 
	0x30, 0x22, 0x07, /* HD width 1920 pclk*/			 
	0x30, 0x23, 0x80, 			 
#else
	0x30, 0x20, 0x03, /* VD width 981 lines*/				
	0x30, 0x21, 0xd5,				
	0x30, 0x22, 0x06, /* HD width 1776 pclk*/			
	0x30, 0x23, 0xf0,				
#endif

	0x30, 0x24, 0x00, /* H Start */			 
	0x30, 0x25, 0x24,		
	0x30, 0x26, 0x00, /* V Start */			 
	0x30, 0x27, 0x10, 			 
	0x30, 0x28, 0x0a, /* H End */			 
	0x30, 0x29, 0x2f, 			 
	0x30, 0x2a, 0x07, /* V End */			 
	0x30, 0x2b, 0x97, 			 

	0x30, 0x2c, 0x05, /*image size */ 			 
	0x30, 0x2d, 0x00, 			 
	0x30, 0x2e, 0x02, 			 
	0x30, 0x2f, 0xD0, 			 

	0x30, 0xb3, 0x00, 			 
	0x33, 0x01, 0xc0, 			 
	0x33, 0x13, 0xf0, 			 
	0x33, 0x14, 0x05, 			 
	0x33, 0x15, 0x02, 			 
	0x33, 0x16, 0x03, 			 
	0x33, 0x17, 0xc4, 			 
	0x33, 0x18, 0x22, 			 
						 
	0x30, 0xf8, 0x05, 			 
	0x30, 0x83, 0x08, 			 
	0x30, 0x86, 0x00, 			 
	0x30, 0x8c, 0x80, 			 
	0x30, 0xf9, 0x11, 			 
	0x30, 0xfb, 0x80, 			 
	0x30, 0x8a, 0x25, 			 
	0x30, 0x16, 0x01, 			 

	0x30, 0x0e, 0xb7,  /*54 Mhz */
	0x30, 0x0f, 0x00,  /* 00 for 30fps PCLK=54MHz , 10 for 15fps PCLK=27MHz, */
	0x30, 0x10, 0x07, 			 
	0x30, 0x11, 0x40, 			 	   	

	0x30, 0x89, 0x0a,  /* bayer pattern */


	0x30, 0xb4, 0x00,  
	0x35, 0x06, 0x30,  /* HSYNC Mode */
	0x35, 0x01, 0x01, 

	0x30, 0xfa, 0x01,  /* sleep off */
	
#endif			 
	
	0x30,0x02,0x40,		//exp
    0x30, 0xfa, 0x01,     /* sw sleep off*/
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	

//	0x30, 0xfa, 0x00,	 /*sw sleep on		 */
//	0x30,0x89,0x0a,  /* bayer pattern */
//	0x30,0xb4,0x00,  
//	0x35,0x06,0x30,  /* HSYNC Mode */
//	0x35,0x01,0x01, 
//	0x35,0x0e,0x40,  /* Vsync from Sensor */

#if (PREV_RESP==960) //960p

	0x30, 0xfa, 0x00, /*sw sleep on */
#if 0
	0x30, 0x20, 0x03, /* VD width 996 lines*/ 			 
	0x30, 0x21, 0xe4, 			 
	0x30, 0x22, 0x07, /* HD width 1920 pclk*/			 
	0x30, 0x23, 0x80, 			 
#else
	0x30, 0x20, 0x03, /* VD width 981 lines*/				
	0x30, 0x21, 0xd5,				
	0x30, 0x22, 0x06, /* HD width 1776 pclk*/			
	0x30, 0x23, 0xf0,				
#endif

	0x30, 0x24, 0x00, /* H Start */			 
	0x30, 0x25, 0x24,		
	0x30, 0x26, 0x00, /* V Start */			 
	0x30, 0x27, 0x10, 			 
	0x30, 0x28, 0x0a, /* H End */			 
	0x30, 0x29, 0x2f, 			 
	0x30, 0x2a, 0x07, /* V End */			 
	0x30, 0x2b, 0x97, 			 

	0x30, 0x2c, 0x05, /*image size */ 			 
	0x30, 0x2d, 0x00, 			 
	0x30, 0x2e, 0x03, 			 
	0x30, 0x2f, 0xc0, 			 

	0x30, 0xb3, 0x00, 			 
	0x33, 0x01, 0xc0, 			 
	0x33, 0x13, 0xf0, 			 
	0x33, 0x14, 0x05, 			 
	0x33, 0x15, 0x02, 			 
	0x33, 0x16, 0x03, 			 
	0x33, 0x17, 0xc4, 			 
	0x33, 0x18, 0x22, 			 
						 
	0x30, 0xf8, 0x05, 			 
	0x30, 0x83, 0x08, 			 
	0x30, 0x86, 0x00, 			 
	0x30, 0x8c, 0x80, 			 
	0x30, 0xf9, 0x11, 			 
	0x30, 0xfb, 0x80, 			 
	0x30, 0x8a, 0x25, 			 
	0x30, 0x16, 0x01, 			 

	0x30, 0x0e, 0xb7,  /*54 Mhz */
	0x30, 0x0f, 0x00,  /* 00 for 30fps PCLK=54MHz , 10 for 15fps PCLK=27MHz, */
	0x30, 0x10, 0x07, 			 
	0x30, 0x11, 0x40, 			 	   	

	0x30, 0x89, 0x0a,  /* bayer pattern */


	0x30, 0xb4, 0x00,  
	0x35, 0x06, 0x30,  /* HSYNC Mode */
	0x35, 0x01, 0x01, 

	0x30, 0xfa, 0x01,  /* sleep off */
	
#elif (PREV_RESP==720)
						 
	0x30, 0xfa, 0x00, /*sw sleep on */
#if 0
	0x30, 0x20, 0x03, /* VD width 996 lines*/ 			 
	0x30, 0x21, 0xe4, 			 
	0x30, 0x22, 0x07, /* HD width 1920 pclk*/			 
	0x30, 0x23, 0x80, 			 
#else
	0x30, 0x20, 0x03, /* VD width 981 lines*/				
	0x30, 0x21, 0xd5,				
	0x30, 0x22, 0x06, /* HD width 1776 pclk*/			
	0x30, 0x23, 0xf0,				
#endif

	0x30, 0x24, 0x00, /* H Start */			 
	0x30, 0x25, 0x30,		
	0x30, 0x26, 0x00, /* V Start */			 
	0x30, 0x27, 0x10, 			 
	0x30, 0x28, 0x0a, /* H End */			 
	0x30, 0x29, 0x2f, 			 
	0x30, 0x2a, 0x07, /* V End */			 
	0x30, 0x2b, 0x97, 			 

	0x30, 0x2c, 0x05, /*image size */ 			 
	0x30, 0x2d, 0x00, 			 
	0x30, 0x2e, 0x02, 			 
	0x30, 0x2f, 0xD0, 			 

	0x30, 0xb3, 0x00, 			 
	0x33, 0x01, 0xc0, 			 
	0x33, 0x13, 0xf0, 			 
	0x33, 0x14, 0x05, 			 
	0x33, 0x15, 0x02, 			 
	0x33, 0x16, 0x03, 			 
	0x33, 0x17, 0xc4, 			 
	0x33, 0x18, 0x22, 			 
						 
	0x30, 0xf8, 0x05, 			 
	0x30, 0x83, 0x08, 			 
	0x30, 0x86, 0x00, 			 
	0x30, 0x8c, 0x80, 			 
	0x30, 0xf9, 0x11, 			 
	0x30, 0xfb, 0x80, 			 
	0x30, 0x8a, 0x25, 			 
	0x30, 0x16, 0x01, 			 

	0x30, 0x0e, 0xb7,  /*54 Mhz */
	0x30, 0x0f, 0x00,  /* 00 for 30fps PCLK=54MHz , 10 for 15fps PCLK=27MHz, */
	0x30, 0x10, 0x07, 			 
	0x30, 0x11, 0x40, 			 	   	

	0x30, 0x89, 0x0a,  /* bayer pattern */


	0x30, 0xb4, 0x00,  
	0x35, 0x06, 0x30,  /* HSYNC Mode */
	0x35, 0x01, 0x01, 

	0x30, 0xfa, 0x01,  /* sleep off */
	
#endif			 
	
	0x30,0x02,0x40,		//exp
    0x30, 0xfa, 0x01,     /* sw sleep off*/
    
};

static code UINT8 front60fpsPrevCmdTbl[] = {
	//	0x30, 0xfa, 0x00,	 /*sw sleep on		 */
	//	0x30,0x89,0x0a,  /* bayer pattern */
	//	0x30,0xb4,0x00,  
	//	0x35,0x06,0x30,  /* HSYNC Mode */
	//	0x35,0x01,0x01, 
	//	0x35,0x0e,0x40,  /* Vsync from Sensor */

	0x30, 0x20, 0x02,//frame length 516
	0x30, 0x21, 0x04,//frame length
	0x30, 0x22, 0x0c,//line length 3100 /* HD width 3100/2=1550 lines*/ 			
	0x30, 0x23, 0x1c,//line length
	0x30, 0x24, 0x00,//x addr start 44				   
	0x30, 0x25, 0x2c,//x addr start				   
	0x30, 0x26, 0x00,//y addr start	20				   
	0x30, 0x27, 0x14,//y addr start					   
	0x30, 0x28, 0x0a,//x addr end	2607				   
	0x30, 0x29, 0x2f,//x addr end					   
	0x30, 0x2a, 0x07,//y addr end	1955				   
	0x30, 0x2b, 0xa3,//y addr end
	0x30, 0x2c, 0x02,//x output size 640					   
	0x30, 0x2d, 0x80,//x output size	       				   
	0x30, 0x2e, 0x01,//y output size 480					   
	0x30, 0x2f, 0xe0,//y output size					   
	0x30, 0xb3, 0x09,//pclk				   
	0x33, 0x01, 0xc1,	
	0x33, 0x13, 0xf1,//vap avg, h=1:1,v=1:2				   
	0x33, 0x14, 0x05,//DSP H size	in 1284				   
	0x33, 0x15, 0x04,//DSP H size in
	0x33, 0x16, 0x01,//DSP V size	in 484						   
	0x33, 0x17, 0xe4,//DSP V size	in						   
	0x33, 0x18, 0x20,					   
	0x30, 0xf8, 0x09,//image transform HSUB 1:2, VSUB 1:4				   
	0x30, 0x83, 0x00,					   
	0x30, 0x86, 0x24,					   
	0x30, 0x8c, 0x88,					   
	0x30, 0xf9, 0x13,					   
	0x30, 0xfb, 0x81,					   
	0x30, 0x8a, 0x26,					   
	0x30, 0x16, 0x02,					   
	/* PLL settings */
	0x30, 0x0e, 0xb0, 			 
	0x30, 0x0f, 0x11,     /* 00 for 30fps PCLK=96MHz , 10 for 15fps PCLK=48MHz, 20 for 10fps PCLK= 32 MHz, 30 for 7.5fps PCLK=24MHz*/
	0x30, 0x10, 0x07, 			 
	0x30, 0x11, 0x00,     /* 0x40 for 30fps PCLK=24MHz, 0x00 for 60fps PCLK=48MHz */ 			 							  
	0x30, 0x89, 0x0a,     /* bayer pattern */
	0x30, 0xb4, 0x00,  
	0x35, 0x06, 0x30,     /* HSYNC Mode */
	0x35, 0x01, 0x01, 		
	0x35, 0x0e, 0x40,     /* Vsync from Sensor */
    0x30, 0x71, 0x34,     /* OB adjust while changing gain */

	0x30,0x02,0x40,		//exp
    0x30, 0xfa, 0x01,     /* sw sleep off*/
};

static code UINT8 frontSnapCmdTbl[] = {
	0x30, 0xfa, 0x00,     /* sw sleep off*/

	0x30, 0xf8, 0x00,//image transform VSUB full, HSUB full
	0x30, 0x83, 0x08, 
	0x30, 0x86, 0x00, 
	0x30, 0x8c, 0x82, 
	0x30, 0xf9, 0x11, 
	0x30, 0xfb, 0x84, 
	0x30, 0x8a, 0x25, 
	0x30, 0x16, 0x01, 
			  
	0x30, 0x20, 0x07,//frame length 1988	   
	0x30, 0x21, 0xc4,//frame length
	//0x30, 0x20, 0x0b,//frame length 1988+900	   
	//0x30, 0x21, 0xac,//frame length
	
	0x30, 0x22, 0x0c,//line length 3240	
	0x30, 0x23, 0xa8,//line length

#if 0	
	0x30, 0x24, 0x00, 
	0x30, 0x25, 0x20, 
	0x30, 0x26, 0x00, 
	0x30, 0x27, 0x08, 
	0x30, 0x28, 0x0a, 
	0x30, 0x29, 0x3f, 
	0x30, 0x2a, 0x07, 
	0x30, 0x2b, 0xa3,

	                  /*image size */
	0x30, 0x2c, 0x0a, /* H=2600 */
	0x30, 0x2d, 0x28, 
	0x30, 0x2e, 0x07, /* v=1952 */
	0x30, 0x2f, 0xa0, 
	   
	0x30, 0xb3, 0x00, 
	0x33, 0x01, 0xc0, 
	0x33, 0x13, 0xf0, 
	0x33, 0x14, 0x0a, 
	0x33, 0x15, 0x22, 
	0x33, 0x16, 0x07, 
	0x33, 0x17, 0x9c, 
	0x33, 0x18, 0x22, 


#else
 
	0x30, 0x24, 0x00,//x addr start 18
	0x30, 0x25, 0x12,//x addr start
	0x30, 0x26, 0x00,//y addr start 2
	0x30, 0x27, 0x02,//y addr start
	0x30, 0x28, 0x0a,//x addr end 2621
	0x30, 0x29, 0x3d,//x addr end
	0x30, 0x2a, 0x07,//y addr end 1957		   
	0x30, 0x2b, 0xa5,//y addr end
	
	0x30, 0x2c, 0x0a,//x output size 2600	   
	0x30, 0x2d, 0x28,//x output size
	0x30, 0x2e, 0x07,//y output size 1952
	0x30, 0x2f, 0xa0,//y output size
		
	0x30, 0xb3, 0x00,		   
	0x33, 0x01, 0xc0,			
	0x33, 0x13, 0xf0,		   
	0x33, 0x14, 0x0a,		   
	0x33, 0x15, 0x2c,		   
	0x33, 0x16, 0x07,		   
	0x33, 0x17, 0xa4,		   
	0x33, 0x18, 0x22, 
#endif







	
	0x30, 0x13, 0x00, 
	//0x30, 0xbf, 0x11, 

    0x30, 0x71, 0x20,    /* OB adjust every frame */

#if 0 
	   /* PLL Settings */
	   0x30, 0x0e, 0xb7,  /*54 Mhz */
	   0x30, 0x0f, 0x00,  /* 00 for 30fps PCLK=54MHz , 10 for 15fps PCLK=27MHz, */
	   0x30, 0x10, 0x07,			
	   0x30, 0x11, 0x40,			
#endif
		
#if 1
	   0x35, 0x06, 0x20, /*HREF Mode*/ 
	   0x35, 0x01, 0x00,
#else	 
	   0x30, 0xb4, 0x00,  
	   0x35, 0x06, 0x30, /*HSYNC Mode*/
	   0x35, 0x01, 0x01,	   
#endif
0x30, 0xfa, 0x01,     /* sw sleep off*/


};

static UINT8 front30fpsPrevDz1xCmdTbl[] = {
	0,
};
static UINT8 front30fpsPrevDz2xCmdTbl[] = {
	0,
};

static UINT8 front60fpsPrevDz1xCmdTbl[] = {
	0,
};
static UINT8 front60fpsPrevDz2xCmdTbl[] = {
	0,
};

static UINT8 front24fpsPrevDz1xCmdTbl[] = {
	0,
};
static UINT8 front24fpsPrevDz2xCmdTbl[] = {
	0,
};


static UINT8 frontPrevMode = 1;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;		/* preview last set gain value */
static UINT16 frontLastGainIdx;		/* preview last set gain table index */
static UINT32 frontLastShutter;		/* preview last set shutter value */
static UINT16 frontSnapLastGain;	/* snapshot last set gain value */
static UINT32 frontSnapLastShutter;	/* snapshot last set shutter value */

static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;

static UINT8 frontPrevModeSave = 0;	/* pre-sensor mode */

static UINT8 frontPowerOnStatus = 0; /* 0:power on no initial */

static UINT8 frontPrevDyTdEn = 0;
static UINT8 frontPrevDyTdSwitchPclk = 0;
static UINT8 frontPrevDyTdSwitchExp = 0;
static UINT8 frontPrevDyTdSwitchGain = 0;
static UINT8 frontPrevDyTdSetFlow = 0;

#define ST1155A_ENA1(x)	//{sp1kSHT_ACfg();sp1kSHT_ASet(x);}
#define ST1155A_IN1(x)	//{sp1kSHT_BCfg();sp1kSHT_BSet(x); }

#if 0
extern UINT8 calibrationObData[];
extern UINT8 G_AETblSel;
extern SINT16 gId;
extern UINT8 G_luma;
extern UINT8 G_FlashStatus;
extern UINT8 G_FlashCharging;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
  
/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
UINT8
frontOpen(
	void
) 
{
	#if (REGISTER_EXTERN_OP_FUNC == 1)
	UINT8 id;
	#endif
	frontOperatingFunc_t opFunc; 
	
  
	opFunc.expGainSetPos                           = EXP_GAIN_SET_POS;
	opFunc.gainAfterExp                            = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet                          = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet                        = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet                       = frontOpCapabilityGet;
	opFunc.pfOpIntrCapabilityGet                   = frontOpIntrCapabilityGet;
	opFunc.pfOpPowerOn                             = frontOpPowerOn;
	opFunc.pfOpPowerOff                            = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet                      = frontOpPreviewModeSet;
	opFunc.pfOpPreviewSensorZoomModeSet            = frontOpPreviewSensorZoomModeSet;
	opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontOpIntrPreviewSensorZoomModeSet;
	opFunc.pfOpSnapModeSet                         = frontOpSnapModeSet;
	opFunc.pfOpSnapShot                            = frontOpSnapShot;
	opFunc.pfOpGainSet                             = frontOpGainSet;
	opFunc.pfOpExposureTimeSet                     = frontOpExposureTimeSet;
	opFunc.pfOpIntrGainSet                         = frontOpIntrGainSet;
	opFunc.pfOpIntrExposureTimeSet                 = frontOpIntrExposureTimeSet;
	opFunc.pfOpOpticalBlackStatusGet               = frontOpOpticalBlackStatusGet;
	opFunc.pfOpOpticalBlackCalibrate               = frontOpOpticalBlackCalibrate;
	opFunc.pfOpShutterOpen                         = frontOpShutterOpen;
	opFunc.pfOpShutterClose                        = frontOpShutterClose;
	opFunc.pfOpIntrPreviewDynamicTempoalDenoiseSet = frontOpIntrPreviewDynamicTempoalDenoiseSet;
	
	/* load gain tbl to static xdata UINT16 gainTbl[48] _at_ 0xb000; */
	frontResTblBinRead(_gainTbl, __gainTbl);

  	#if (REGISTER_EXTERN_OP_FUNC == 1)
	/* register extern operating function */
	id = frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));

	if ( id != NULL ) {
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET, frontOpAeTargetLuminanceSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET, frontOpAeExposureCompensationSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_FRAME_RATE_SET, frontOpAeFrameRateSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AWB_MODE_SET, frontOpAwbModeSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AFTER_SNAP_SHOT, frontOpAfterSnapShot);
	}

	return id;
	#else
	return frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));
	#endif
} 
  
/**
 * @brief	operating function to get device name (ascii string).
 * @param	None.
 * @retval	return = device name string.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8 *
frontOpDevNameGet(
	void
)
{
	return frontDevName;
}

/**
 * @brief	operating function to set frame rate, called before further operating.
 * @param   fps = [in] frame rate.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-17
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpFrameRateSet(
	UINT8 fps
)
{
	switch ( fps ) {
		case 24:
		case 20:
			printf("frontOpFrameRateSet = 24\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
			
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 20) {
				printf("50Hz\n");
				frontResTblBinRead(_ae24fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae24fps60hzTbl, __aeTbl);
			}
			break;
		case 30:
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 1;
			//frontSnapMode = 1;
			
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 25) {
				printf("50Hz\n");
				frontResTblBinRead(_ae30fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae30fps60hzTbl, __aeTbl);
			}   
			break;
		case 60:
		case 50:
			printf("frontOpFrameRateSet = 60\n");
			frontPrevMode = 2;
			//frontSnapMode = 2;
			
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 50) {
				printf("50Hz\n");
				frontResTblBinRead(_ae60fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae60fps60hzTbl, __aeTbl);
			}
			break;
	}
}

/**
 * @brief	operating function to get device capability.
 * @param   *(parg->ppprevCap) = [out] pointer to device preview capability.
 * @param   *(parg->ppsnapCap) = [out] pointer to device snap capability.
 * @param   descSize = [in] describer structure size.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpCapabilityGet(
	frontCapabilityArg_t *parg
)
{
	if ( parg->ppprevCap ) {
		*(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
	}

	if ( parg->ppsnapCap ) {
		*(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
	}
}

/**
 * @brief	operating function to get device capability.
 * @param   *(parg->ppprevCap) = [out] pointer to device preview capability.
 * @param   *(parg->ppsnapCap) = [out] pointer to device snap capability.
 * @param   descSize = [in] describer structure size.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrCapabilityGet(
	frontCapabilityArg_t *parg
)
{
	if ( parg->ppprevCap ) {
		*(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
	}

	if ( parg->ppsnapCap ) {
		*(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
	}
}


/**
 * @brief	operating function to power on device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPowerOn(
	void
)
{
	frontPrevCapabDesc_t *pcap;

	/* before power on. */
	frontBeforePowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* used (RDK) GPIO12, GPIO13, (EVB) TP_YN & TP_XN GPIO */
	#if (DBG_SNAP_TIMING != 0)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	frontResCmdBinSend(_frontInit0CmdTbl);
	
	globalTimerWait(10);
	frontResCmdBinSend(_frontInit1CmdTbl);
	
	globalTimerWait(1);

	/* after power on. */
	frontAfterPowerOn();

}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPowerOff(
	void
)
{
	/* before power off. */
	frontBeforePowerOff();

	/* after power off. */
	frontAfterPowerOff();
}

/**
 * @brief	operating function to set preview mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPreviewModeSet(
	void
)
{
	frontPrevCapabDesc_t *pcap;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 24 fps. */
		frontResCmdBinSend(_front24fpsPrevCmdTbl);
	}
	else if ( frontPrevMode == 1 ) {  /* 30 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
	}
	else if ( frontPrevMode == 2 ) {  /* 60 fps. */
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
	}

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

	cdspBayerPatternSet(pcap->pvBayerPtn);

    frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	
    #if FLASHLIGHT_USE
   	  #if 1 // for Strobe TEST   	  	
	    G_FlashStatus = K_FlashLightOn;
	    G_FlashCharging = 1;
	    //sp1kSTChgEnSet(0);
	    //sp1kSTChgEnSet(1);
	  #endif
	#endif
}

/**
 * @brief	operating function to set preview mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvGainIdxVal;
	SINT16 pv_gId = 0;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (frontPrevSensorZoomSnapStatus == 1) {
		frontPrevSensorZoomSnapStatus = 0;
		return;
	}
	
	if ( frontPrevSensorZoomStatus == 0 ) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_30FPS_PREV) {//2x
			
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front30fpsPrevDz2xCmdTbl, 2, sizeof(front30fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontGainSet(0,pvGainIdxVal,1,0);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_60FPS_PREV) {//2x
			
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front60fpsPrevDz2xCmdTbl, 2, sizeof(front60fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontGainSet(0,pvGainIdxVal,1,0);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
		else if (frontPrevMode == 2) {	/* 24 fps */
			#if (SENSOR_ZOOM_FACTOR_24FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_20FPS_PREV) {//2x
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front20fpsPrevDz2xCmdTbl, 2, sizeof(front20fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontGainSet(0,pvGainIdxVal,1,0);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
	}
	else if ( frontPrevSensorZoomStatus == 1) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_30FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front30fpsPrevDz1xCmdTbl, 2, sizeof(front30fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontGainSet(0,pvGainIdxVal,1,0);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_60FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front60fpsPrevDz1xCmdTbl, 2, sizeof(front60fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontGainSet(0,pvGainIdxVal,1,0);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
		else if (frontPrevMode == 2) {	/* 24 fps */
			#if (SENSOR_ZOOM_FACTOR_24FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_20FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front20fpsPrevDz1xCmdTbl, 2, sizeof(front20fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontGainSet(0,pvGainIdxVal,1,0);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
	}
	
}

/**
 * @brief	operating function to set preview mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvGainIdxVal;
	SINT16 pv_gId = 0, paraAry;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (frontPrevSensorZoomSnapStatus == 1) {
		frontPrevSensorZoomSnapStatus = 0;
		return;
	}
	
	if ( frontPrevSensorZoomStatus == 0 ) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_30FPS_PREV) {//2x
			
				aeIntrCurGainValueGet(&pvGainIdxVal);

				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front30fpsPrevDz2xCmdTbl, 2, sizeof(front30fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_60FPS_PREV) {//2x
			
				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front60fpsPrevDz2xCmdTbl, 2, sizeof(front60fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_24FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_24FPS_PREV) {//2x
				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front20fpsPrevDz2xCmdTbl, 2, sizeof(front20fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
	}
	else if ( frontPrevSensorZoomStatus == 1) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_30FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);

				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				
				i2cCmdTableSend_Intr(front30fpsPrevDz1xCmdTbl, 2, sizeof(front30fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_60FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				
				i2cCmdTableSend_Intr(front60fpsPrevDz1xCmdTbl, 2, sizeof(front60fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_24FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_24FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);

				#if 0//intr used
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				#else
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				#endif
				
				i2cCmdTableSend_Intr(front20fpsPrevDz1xCmdTbl, 2, sizeof(front20fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
	}
	
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpSnapModeSet(
	void
)
{
	frontSnapCapabDesc_t *pcap;

	pcap = &(frontSnapCapabDesc[frontSnapMode]);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);	
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	cdspBayerPatternSet(BAYER_PTN_SNAP);
}

/**
 * @brief	operating function to snap shot.
 * @param	first = [in] 0: not first, 1: first shot for burst shot.
 * @retval	None.
 * @see
 * @author	
 * @since	
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpSnapShot(
	UINT8 first,
	UINT8 mode,
	UINT8 scaleUp
)
{

	//printf("first=%bd,mode=%bd\n",first,mode);

	if ( frontPrevSensorZoom2xGainEn ) {
		frontPrevSensorZoomSnapStatus = 1;
	}
	
	if ( first != 0 ) {

		#if (DBG_SNAP_TIMING >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		#if (VSYNC_INV_PREV == 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
		frontResCmdBinSend(_frontSnapCmdTbl);
		
		frontSnapExposureTimeSet(mode,scaleUp);  
	}


	if (mode != SP1K_FRONT_DOFRONT){  /*will: normal capture*/

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		#if (VSYNC_INV_SNAP == 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif
		
	    #if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
	   	//printf("$$$ will test FrameMode=%bd, MSTime=%ld\n",FrameMode, MSTime);
	   	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1); 
		
		#if (VSYNC_INV_SNAP == 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		
		#if 0   
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

			#if FLASHLIGHT_USE  /* Exposure must be longer than 1988 , frame rate is too slow */
	  			if(sp1kSTChgRdyGet()!=0x00){    
			    sp1kSTChgEnSet(0);	
			    sp1kST_TrigSet(1);
			}	       
			#endif
		#endif   
		//globalTimerWait(1);

		#if (VSYNC_INV_SNAP == 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#endif
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
	    frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
		#endif
		
	    HAL_CdspEofWait(1);	   

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		   
	    #if FLASHLIGHT_USE
			//sp1kST_TrigSet(0);	   
			//sp1kSTChgEnSet(0);	  /* charge enable should be controlled by host */
		#endif   
	}
	else{	/* do front flow */
		frontSnapCapabDesc_t *pcap;
		pcap = &(frontSnapCapabDesc[frontSnapMode]);

		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1); // 20081024 mantis 32544 wenguo

		cdspSkipWrSyncSet(1);	
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		
		
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		//gpioByteDirSet(GPIO_BYTE_GEN1, 0x20, 0x20);
		//gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
		
		//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			
		//frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
		
		if (scaleUp==1){
			printf("up\n");
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDivBurstUp, pcap->mclkPhase, 0, 0);
		}
		else{
			printf("down\n");
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDivBurst, pcap->mclkPhase, 0, 0);
		}
	}
	
}

/**
 * @brief	operating function to set gain value.
 * @param	parg->val = [in] gain value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpGainSet(
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[3*1];
	//UINT8 ADDR16=0x3000;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;


	// printf("gain set val=%bu\n",val);

	// printf("gain set frontLastGain=%d\n",frontLastGain);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			//digitalZoom2xGain = 0x07;
		}
		else if ( frontPrevMode == 1 ) {	/* 60fps */
			//digitalZoom2xGain = 0x05;
		}
		else if ( frontPrevMode == 2 ) {	/* 24fps */
			//digitalZoom2xGain = 0x07;
		}
	}


	cmdTbl[0] = 0x30;
	cmdTbl[1] = 0x00;
	cmdTbl[2] = frontLastGain;
	//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
	//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);	
}

/**
 * @brief	operating function to set exposure time value.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpExposureTimeSet(
	frontExposureTimeArg_t *parg
)
{
	frontPrevCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 pixclk;
	UINT8 cmdTbl[3*4];
	//UINT8 REG_ADDR16[4]={0x301e, 0x301f, 0x3002, 0x3003};
	//UINT8 REG_ADDR[4];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	if (espline == 0) {
		espline = 1;
	}

	cmdTbl[0] = 0x30;
	cmdTbl[1] = 0x1e;

	cmdTbl[3] = 0x30;
	cmdTbl[4] = 0x1f;

	cmdTbl[6] = 0x30;
	cmdTbl[7] = 0x02;

	cmdTbl[9] = 0x30;
	cmdTbl[10] = 0x03;
    
	if (espline>(ftotal-2)){
	    cmdTbl[2] = ((espline-(ftotal-2))&0xff00)>>8;	
		cmdTbl[5] = (espline-(ftotal-2)) & 0xff;
		cmdTbl[8] = (ftotal-2)>>8;
		cmdTbl[11] = (ftotal-2) & 0xff;		
	}
	else{
	    cmdTbl[2] = 0;
		cmdTbl[5] = 0;		
		cmdTbl[8] = (espline&0xff00)>>8;
		cmdTbl[11] = espline & 0xff;	
	}
	
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,4,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2],REG_ADDR[3]);
}

/**
 * @brief	operating function to set gain value for intr.
 * @param	parg->val = [in] gain value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrGainSet(
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[3*1];
	//UINT8 ADDR16=0x3000;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	#if 1
	switch (frontPrevDyTdSwitchGain) {
		case 1:
			//printf("%bu,G -\n",val);
			switch (frontPrevMode) {
				case 0:
					if (val >= 1) {
						val -= 1;
					}
					else {
						val = 0;
					}
					break;
				case 1:
					if (val >= 3) {
						val -= 3;
					}
					else {
						val = 0;
					}
					break;
			}
			
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		case 2:
			//printf("%bu,G +\n",val);
			switch (frontPrevMode) {
				case 0:
					if (val < 48) {
						val += 1;
					}
					else {
						val = val-1;
					}
					break;
				case 1:
					if (val < 46) {
						val += 3;
					}
					else {
						val = val-1;
					}
					break;
			}
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		default:
			break;
	}
	#else
	switch (frontPrevDyTdSwitchGain) {
		case 1:
			//printf("%bu,G -\n",val);
			switch (frontPrevMode) {
				case 0:
					val -= 1;
					break;
				case 1:
					val -= 3;
					break;
			}
			
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		case 2:
			//printf("%bu,G +\n",val);
			switch (frontPrevMode) {
				case 0:
					val += 1;
					break;
				case 1:
					val += 3;
					break;
			}
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		default:
			break;
	}
	#endif

	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			//digitalZoom2xGain = 0x07;
		}
		else if ( frontPrevMode == 1 ) {	/* 60fps */
			//digitalZoom2xGain = 0x05;
		}
		else if ( frontPrevMode == 2 ) {	/* 24fps */
			//digitalZoom2xGain = 0x07;
		}
	}

	cmdTbl[0] = 0x30;
	cmdTbl[1] = 0x00;
	cmdTbl[2] = frontLastGain;
	//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
	//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);	

}

/**
 * @brief	operating function to set exposure time value for intr.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrExposureTimeSet(
	frontExposureTimeArg_t *parg
)
{
	frontPrevCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 pixclk;
	UINT8 cmdTbl[3*4];
	//UINT8 REG_ADDR16[4]={0x301e, 0x301f, 0x3002, 0x3003};
	//UINT8 REG_ADDR[4];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;

	if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {
		pixclk = pcap->prevDynamicTDTbl->pclkFreq;
	}
	else {
		pixclk = pcap->pclkFreq;
	}

	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	if (espline == 0) {
		espline = 1;
	}

	cmdTbl[0] = 0x30;
	cmdTbl[1] = 0x1e;

	cmdTbl[3] = 0x30;
	cmdTbl[4] = 0x1f;

	cmdTbl[6] = 0x30;
	cmdTbl[7] = 0x02;

	cmdTbl[9] = 0x30;
	cmdTbl[10] = 0x03;
    
	if (espline>(ftotal-2)){
	    cmdTbl[2] = ((espline-(ftotal-2))&0xff00)>>8;	
		cmdTbl[5] = (espline-(ftotal-2)) & 0xff;
		cmdTbl[8] = (ftotal-2)>>8;
		cmdTbl[11] = (ftotal-2) & 0xff;		
	}
	else{
	    cmdTbl[2] = 0;
		cmdTbl[5] = 0;		
		cmdTbl[8] = (espline&0xff00)>>8;
		cmdTbl[11] = espline & 0xff;	
	}
	
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,4,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2],REG_ADDR[3]);
}


/**
 * @brief	operating function to get optical black status.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8
frontOpOpticalBlackStatusGet(
	void
)
{

	return SUCCESS;
}

/**
 * @brief	operating function to calibrate optical black.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8
frontOpOpticalBlackCalibrate(
	void
)
{

	return SUCCESS;
}

/**
 * @brief	operating function to open shutter
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpShutterOpen(
	void
)
{
	ST1155A_IN1(0);		//SHT Close
	ST1155A_ENA1(1);	//enable
	globalTimerWait(10);
	ST1155A_ENA1(0);	//disable

}

/**
 * @brief	operating function to close shutter
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpShutterClose(
	void
)
{

}

/**
 * @brief	operating function to set preview mode dynamic temporal denoise intr flow.
 * @param	[in] preview mode dynamic temporal denoise enable/disable.
 *						enable : FRONT_PV_DYNAMIC_TD_ENABLE
 *						disable : FRONT_PV_DYNAMIC_TD_DISABLE
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrPreviewDynamicTempoalDenoiseSet(
	UINT8 en
)
{
	frontPrevCapabDesc_t *pcap;
	SINT16 paraAry[4];
	UINT8 upd;
		
	if (frontPrevDyTdSetFlow == 0) {
		
		//printf("front flag\n");
		
		frontPrevDyTdSetFlow = 1;
		
		frontPrevDyTdEn = en;
		frontPrevDyTdSwitchPclk = 1;
		frontPrevDyTdSwitchExp = 1;
		
		if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_DISABLE) {
			frontPrevDyTdSwitchGain = 1;//-gain idx
		}
		else {
			frontPrevDyTdSwitchGain = 2;//+gain idx
		}				
	}
	else {
		
		//printf("front pclk");
		
		frontPrevDyTdSetFlow = 0;
		
		pcap = &(frontPrevCapabDesc[frontPrevMode]);
		
		if (frontPrevDyTdSwitchPclk == 1) {
			frontPrevDyTdSwitchPclk = 0;
			
			if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {

				//printf("[en]\n");
				
				paraAry[0] = MCLK_SRC;
				paraAry[1] = pcap->prevDynamicTDTbl->mclkDiv;
				paraAry[2] = pcap->prevDynamicTDTbl->mclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
				paraAry[0] = PCLK_SRC;
				paraAry[1] = pcap->prevDynamicTDTbl->pclkDiv;
				paraAry[2] = pcap->prevDynamicTDTbl->pclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
			}
			else {
				
				//printf("[dis]\n");
				
				paraAry[0] = MCLK_SRC;
				paraAry[1] = pcap->mclkDiv;
				paraAry[2] = pcap->mclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
				paraAry[0] = PCLK_SRC;
				paraAry[1] = pcap->pclkDiv;
				paraAry[2] = pcap->pclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
			}
		}
	}
}

/**
 * @brief	operating function to set ae target luminance.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeTargetLuminanceSet(
	void *parg
)
{
	parg = parg;
	//printf("frontSensorOpAeTargetLuminanceSet=%bu\n",(UINT8)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set ae exposure compensation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-26
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeExposureCompensationSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAeExposureCompensationSet=%bu\n", (UINT8)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set ae frame rate.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeFrameRateSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAeFrameRateSet=%d\n", (UINT16)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set awb mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAwbModeSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAwbModeSet=%bu\n", (UINT8)parg);
	// NOTE: use variable "mode" for your purpose.
}

/**
 * @brief	after snap shot.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Wen-Guo Gan
 * @since	2009-01-09
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAfterSnapShot(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAfterSnapShot=%bu\n", (UINT8)parg);
	// NOTE: use variable "mode" for your purpose.
}

/**
 * @brief	before power on.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-11
 * @todo	N/A
 * @bug		N/A
*/
static void
frontBeforePowerOn(
	void
)
{

}

/**
 * @brief	after power on.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-11
 * @todo	N/A
 * @bug		N/A
*/
static void
frontAfterPowerOn(
	void
)
{
	#if (RES_LSC_LOAD == 1)
	/* resource file lsc cmd table send */
	frontResCmdBinSend(FRONT_RES_PARAM_LSC_ID);
	#endif
	
	#if FLASHLIGHT_USE
	    //sp1kSTChgEnCfg();
	    //sp1kST_TrigCfg();
		//sp1kSTChgRdyCfg();
		
		//sp1kSTChgEnSet(1);
		//sp1kST_TrigSet(0);
	#endif
}

/**
 * @brief	before power off.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-11
 * @todo	N/A
 * @bug		N/A
*/
static void
frontBeforePowerOff(
	void
)
{

}

/**
 * @brief	after power off.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-11
 * @todo	N/A
 * @bug		N/A
*/
static void
frontAfterPowerOff(
	void
)
{

}

/**
 * @brief	set snap exposure time.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
static void
frontSnapExposureTimeSet(
	UINT8 mode,
	UINT8 scaleUp
)
{
	frontSnapCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 pixclk;
	UINT8 cmdTbl[3*5];
	UINT8 s_gain;//, lastt_gain;

	/* Before Snap ExposureTime Set */
	frontBeforeSnapExposureTimeSet();
	
	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;

	if(mode == SP1K_FRONT_DOFRONT){
		if (scaleUp==1){
			pixclk = pcap->pclkFreqBurstUp;
		}
		else{
			pixclk = pcap->pclkFreqBurst;
		}
	}
	else{
		pixclk = pcap->pclkFreq;
	}
	
	//printf("ltotal=%d,ftotal=%d,pixclk=%lu\n",ltotal,ftotal,pixclk);
	espline = (UINT32)(pixclk * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]) / (UINT32)(ltotal * frontSnapLastShutter);
	//printf("espline=%d\n",espline);

	if (espline == 0) {
		espline = 1;
	}

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 1 ) {  /* 30 fps. */
		}
		else if ( frontPrevMode == 0 ) {	/* 24fps */
		}
	}

	s_gain = frontSnapLastGain;

    //printf("$$$ will test ltotal=%d,ftotal=%d,pixclk=%ld,espline=%d,shutter=%d\n",ltotal,ftotal,pixclk,frontLastGain);	
	/*
		if ( s_gain >= 0x30 ) {
			lastt_gain = s_gain - 48;
			espline = espline * 4;
		}
		else if ( s_gain >= 0x10 ) {
			lastt_gain = s_gain - 16;
			espline = espline * 2;
		}
		else {
			lastt_gain = s_gain;
			espline = espline;
		}
	*/
	//cdspColorMatrixRestore();
	//XBYTE[0x21D3] = 0x02;   // Edge Enhance
	//XBYTE[0x21D0] = 0x02;

	cmdTbl[0] = 0x30;
	cmdTbl[1] = 0x1e;

	cmdTbl[3] = 0x30;
	cmdTbl[4] = 0x1f;

	cmdTbl[6] = 0x30;
	cmdTbl[7] = 0x02;

	cmdTbl[9] = 0x30;
	cmdTbl[10] = 0x03;
    
	if (espline>(ftotal-2)){
	    cmdTbl[2] = ((espline-(ftotal-2))&0xff00)>>8;	
		cmdTbl[5] = (espline-(ftotal-2)) & 0xff;
		cmdTbl[8] = (ftotal-2)>>8;
		cmdTbl[11] = (ftotal-2) & 0xff;		
	}
	else{
	    cmdTbl[2] = 0;
		cmdTbl[5] = 0;		
		cmdTbl[8] = (espline&0xff00)>>8;
		cmdTbl[11] = espline & 0xff;	
	}


 	cmdTbl[12] = 0x30;
	cmdTbl[13] = 0x00;
	cmdTbl[14] = s_gain;
 
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
}

/**
 * @brief	Before Snap Exposure Time Set.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontBeforeSnapExposureTimeSet(
	void
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 compensationGainIdx = 0, lastGainIdx, pvGainIdxVal;
	UINT8 isoCfg, isoValue, isoGainIdx, autoGainIdx, adjustGainIdx, pvBanding, capBanding, pwrFreq, limitShutterFps;
	UINT16 maxGainTblIdx;
	UINT16 isoNumerator = 0, autoNumerator = 0;
	UINT16 isoDenominator = 0, autoDenominator = 0;
	UINT32 isoShutter, limitShutter, bandingShutter, adjustShutter;
	#if (PV_AUTO_OB_BY_GAIN == 1)
	UINT8 minIdx, maxIdx;
	#endif
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* get max gain table index */
	maxGainTblIdx = (UINT16)((sizeof(gainTbl)/sizeof(UINT16))-1);
	//printf("maxGainTblIdx=0x%x\n",maxGainTblIdx);

	/* get capture iso mode */
	aeStatusGet(SP1K_AE_CAPTURE_ISO_MODE,&isoCfg);
	//printf("iso=%bu\n",isoCfg);

	/* get pv gain idx */
	sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
	//printf("pvGainIdxVal=0x%bx\n", pvGainIdxVal);

	/* get capture compensation gain idx */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
	//printf("compensationGainIdx=0x%bx\n", compensationGainIdx);

	/* get current power freq */
	sp1kAeStatusGet(SP1K_AE_G_AETblSel, &pwrFreq);
	//printf("pwrFreq=%bu\n", pwrFreq);

	/* get anti-shake limit fps */
	sp1kAeStatusGet(SP1K_AE_STS_CAP_ANTISHAKE_FPS, &limitShutterFps);
	
	/* calculate atuo iso gain idx */
	autoGainIdx = pvGainIdxVal + compensationGainIdx;

	/* set iso gain or atuo iso gain idx */
	switch (isoCfg) {
		case SP1K_AE_ISO_AUTO:			
			if (autoGainIdx > maxGainTblIdx) {
				autoGainIdx = maxGainTblIdx;
			}
			break;
		case SP1K_AE_ISO_100:
			/* calculate last gain */
			isoGainIdx = ISO_100_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_200:
			/* calculate last gain */
			isoGainIdx = ISO_200_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_400:
			/* calculate last gain */
			isoGainIdx = ISO_400_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_800:
			/* calculate last gain */
			isoGainIdx = ISO_800_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		default:
			/* not this case */
			ASSERT(0,1);
			break;
	}
	//printf("isoCfg=%bu,autoGainIdx=0x%bx,isoGainIdx=0x%bx,limitShutterFps=%bu\n",isoCfg,autoGainIdx,isoGainIdx,(UINT8)limitShutterFps);

	/* switch iso mode */
	if (isoCfg != SP1K_AE_ISO_AUTO) {

		/* set capture exif iso tag value */
		isoValue = isoCfg;
		
		/* auto iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[autoGainIdx],&autoNumerator,&autoDenominator);
		//printf("autoGain=0x%x,%d/%d\n",pcap->gainTbl[autoGainIdx],autoNumerator,autoDenominator);
		
		/* user define iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[isoGainIdx],&isoNumerator,&isoDenominator);
		//printf("isoGain=0x%x,%d/%d\n",pcap->gainTbl[isoGainIdx],isoNumerator,isoDenominator);

		isoShutter = (UINT32)isoNumerator * frontLastShutter * autoDenominator / (isoDenominator * autoNumerator);
		//printf("isoShutter=%lu\n",isoShutter);

		if (limitShutterFps != 0) {
			/* to limit exposure time small than CAPTURE_LIMIT_FPS */
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]);
			if (isoShutter < limitShutter) {
				isoShutter = limitShutter;
				//printf("limitShutter=%lu\n",limitShutter);
			}
		}


		#if (ISO_CAPTURE_AUTO_DEBANDING == 1)
		/* get banding shutter number */
		bandingShutter = (UINT32)10*pwrFreq*2;

		/* get banding type */
		pvBanding = ((frontLastShutter > bandingShutter) ? 1 : 0);
		//capBanding = ((isoShutter/frontPrevToSnapBinningRateTbl[frontPrevMode] > bandingShutter) ? 1 : 0);
		capBanding = ((isoShutter > bandingShutter*frontPrevToSnapBinningRateTbl[frontPrevMode]) ? 1 : 0);
		//printf("pvBanding=%bu,capBanding=%bu\n",pvBanding,capBanding);

		/* check preview not banding capture banding case */
		if (pvBanding == 0 && capBanding == 1) {
			lastGainIdx = autoGainIdx;
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = frontLastShutter;
		}
		else {
			lastGainIdx = isoGainIdx;
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = isoShutter;
		}
		#else	
		lastGainIdx = isoGainIdx;
		frontSnapLastGain = pcap->gainTbl[lastGainIdx];
		frontSnapLastShutter = isoShutter;
		#endif
	}
	else {

		lastGainIdx = autoGainIdx;
		
		if ( lastGainIdx > ISO_400_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_800;
		}
		else if ( lastGainIdx > ISO_200_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_400;
		}
		else if ( lastGainIdx > ISO_100_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_200;
		}
		else {
			isoValue = SP1K_AE_ISO_100;
		}
		//printf("isoValue=%bu\n",isoValue);		

		if (limitShutterFps != 0) {
			
			/* to limit exposure time small than CAPTURE_LIMIT_FPS */
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]);

			adjustShutter = frontLastShutter;

			//printf("limitShutter=%lu,%lu\n",limitShutter,adjustShutter);
			
			while ( adjustShutter < limitShutter ) {

				/* check max gain idx */
				if (isoValue != (SP1K_AE_ISO_MAX - 1)) {

					if ( isoValue < (SP1K_AE_ISO_MAX - 1) ) {
						isoValue++;
						adjustShutter = (adjustShutter << 1);
						//printf("isoValue=%bu,%lu\n",isoValue,adjustShutter);
					}

					switch (isoValue) {
						case SP1K_AE_ISO_800:
							adjustGainIdx = lastGainIdx + (ISO_800_GAIN_IDX - ISO_400_GAIN_IDX);
							break;
						case SP1K_AE_ISO_400:
							adjustGainIdx = lastGainIdx + (ISO_400_GAIN_IDX - ISO_200_GAIN_IDX);
							break;
						case SP1K_AE_ISO_200:
							adjustGainIdx = lastGainIdx + (ISO_200_GAIN_IDX - ISO_100_GAIN_IDX);
							break;
						case SP1K_AE_ISO_100:
							adjustGainIdx = lastGainIdx;
							break;
					}
					lastGainIdx = adjustGainIdx;
				}
				else {
					adjustShutter = limitShutter;
					break;
				}
			}

			frontSnapLastGain = pcap->gainTbl[lastGainIdx];			
			frontSnapLastShutter = adjustShutter;
			//printf("frontSnapLastShutter=%lu,0x%x\n",frontSnapLastShutter,frontSnapLastGain);

		}
		else {
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = frontLastShutter;
		}
	}

	/* set capture exif iso tag value */
	sp1kAeParamSet(SP1K_AE_CAPTURE_ISO_VALUE, isoValue);
	
	//printf("frontLastGain=0x%x,frontLastShutter=%lu\n",frontLastGain,frontLastShutter);
	//printf("frontSnapLastGain=0x%x,frontSnapLastShutter=%lu\n",frontSnapLastGain,frontSnapLastShutter); 

	#if (PV_AUTO_OB_BY_GAIN == 1)
	if (pvGainIdxVal == lastGainIdx) {
		/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
		LoadBackfrontLastSetting = 1;

		minIdx = pcap->gainMinIdx;
		maxIdx = pcap->gainMaxIdx;
		//printf("minIdx=0x%bx,maxIdx=0x%bx\n",minIdx,maxIdx);
		
		/* auto ob by gain */
		if (pvGainIdxVal == minIdx) {
			LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
		}
		else {//(pvGainIdxVal == maxIdx)  & other case
			LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
		}		
	}
	else {
		LoadBackfrontLastGainIdx = pvGainIdxVal;
	}
	#else
	LoadBackfrontLastGainIdx = pvGainIdxVal;
	#endif
	//printf("pvGainIdxVal=0x%bx, LoadBackfrontLastGainIdx=0x%bx\n", pvGainIdxVal, LoadBackfrontLastGainIdx);
	
}

/**
 * @brief	Sensor Gain Table Value Transfer Func.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontGainTblValueTransfer(
	UINT16 value,
	UINT16 *numerator,
	UINT16 *denominator
)
{
	#if 0
	*numerator = (16+(value&0x000F))*((value&0x0010)?2:1)*((value&0x0020)?2:1)*((value&0x0040)?2:1);
	*denominator = 16;
	#else
	*numerator = (16+(value&0x000F));
	*denominator = (((16/((value&0x0010)?2:1))/((value&0x0020)?2:1))/((value&0x0040)?2:1));	
	#endif
}


/**
 * @brief	Preview Mode Switch Post Proc Set.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontPreviewModeSwitchPostProc(
	void
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvFreq, gIdval;
	SINT16 pv_gId = 0;
	UINT8 minIdx, maxIdx;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontPrevSensorZoomStatus = 0;
	frontPrevSensorZoom2xGainEn = 0;

	/* power on status */
	if (frontPowerOnStatus == 1) {
	
		/* sensor different resolution or fps switch check */
		if (frontPrevModeSave != frontPrevMode) {//(frameRateSetFlag == 1) {
			sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
			sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
			pv_gId = gIdval;
			//printf("pvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);
			
			if (frontPrevModeSave == 2 && frontPrevMode == 1) {
				pv_gId += 20;
				//printf("60->30 +20\n");
			}
			else if (frontPrevModeSave == 2 && frontPrevMode == 0) {
				pv_gId += 26;
				//printf("60->24 +26\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
				pv_gId += 6;
				//printf("30->24 +6\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 2) {
				pv_gId -= 20;
				//printf("30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 1) {
				pv_gId -= 6;
				//printf("24->30 -6\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 2) {
				pv_gId -= 26;
				//printf("24->60 -26\n");
			}
			
			if (pvFreq == SP1K_FLICKER_60) {
				minIdx = pcap->aeMinIdxTbl[0];
				maxIdx = pcap->aeMaxIdxTbl[0];
			}
			else {
				minIdx = pcap->aeMinIdxTbl[1];
				maxIdx = pcap->aeMaxIdxTbl[1];
			}

			//printf("min=%bu,max=%bu\n",minIdx,maxIdx);

			if(pv_gId < minIdx) {
				pv_gId = minIdx;
				//printf("pv_gId = %d < min\n",pv_gId);
			}

			if(pv_gId > maxIdx) {
				pv_gId = maxIdx;
				//printf("pv_gId = %d > max\n",pv_gId);
			}

			sp1kAeParamSet(SP1K_AE_gId,(UINT8)pv_gId);

			if (pvFreq == SP1K_FLICKER_60) {
				frontExposureTimeSet(0,pcap->ae60hzTbl[pv_gId].shutter,1,0);
				frontGainSet(0,pcap->ae60hzTbl[pv_gId].gain,1,0);
			}
			else {//(pvFreq == SP1K_FLICKER_50)
				frontExposureTimeSet(0,pcap->ae50hzTbl[pv_gId].shutter,1,0);
				frontGainSet(0,pcap->ae50hzTbl[pv_gId].gain,1,0);
			}
			
			frontPrevModeSave = frontPrevMode;
		}
		else {
			/* ov sensor auto ob by gain & snap to pv sensor shutter need re-setting */
			if (LoadBackfrontLastSetting == 1) {
				frontExposureTimeSet(0,frontLastShutter,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,frontLastGainIdx,1,0);
				//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				LoadBackfrontLastSetting = 0;
			}
			else {
				/* pv to video mode switch panel dumping */
				frontExposureTimeSet(0,frontLastShutter,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,frontLastGainIdx,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				//printf("frontLastShutter=%lu, frontLastGainIdx=%bu\n",frontLastShutter,frontLastGainIdx);
			}
		}
	}
	else {
		frontPrevModeSave = frontPrevMode;
		frontPowerOnStatus = 1;
	}
}

/**
 * @fn        void frontResCmdBinSend(UINT16 fileId)
 * @brief	front Resource File Table Cmd Send.
 * @param	fileId = [in] resource file id...
 * @retval	None.
 * @see
 * @author	LinJieCheng
 * @since	2010-12-07
 * @todo		N/A
 * @bug		N/A
*/
static void
frontResCmdBinSend(
	UINT16 fileId
)
{
	#if (LOAD_RES_OPTION == 1)
	UINT32 size;
	UINT8 sts;
	//UINT32 i;
	
	//printf("fileId = 0x%bx\n",fileId);
	sp1kNandRsvSizeGet(fileId, NULL, &size);
	if (size > FRONT_RES_CMD_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_CMD_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize+(((UINT16)G_FPGAMemPool - 0x4000)>>1))<<1));
	}while(!sts);

	//for (i = 0; i < size; i++) {
	//	printf("0x%02bx,",G_FPGAMemPool[i]);
	//}

	sts = i2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	if (sts == FAIL) {
		printf("Sensor command table (0x%x)\n",fileId);
		ASSERT(0,1);
	}
	
	#else
	UINT32 size;
	UINT8 *pData;
	UINT8 sts;
	
	switch (fileId) {
		case _frontInit0CmdTbl:
			pData = frontInit0CmdTbl;
			size = sizeof(frontInit0CmdTbl);
			break;
		case _frontInit1CmdTbl:
			pData = frontInit1CmdTbl;
			size = sizeof(frontInit1CmdTbl);
			break;
		//case _frontInit2CmdTbl:
		//	pData = frontInit2CmdTbl;
		//	size = sizeof(frontInit2CmdTbl);
		//	break;
		//case _frontInit3CmdTbl:
		//	pData = frontInit3CmdTbl;
		//	size = sizeof(frontInit3CmdTbl);
		//	break;
		//case _frontInit4CmdTbl:
		//	pData = frontInit4CmdTbl;
		//	size = sizeof(frontInit4CmdTbl);
		//	break;
		//case _front15fpsPrevCmdTbl:
		//	pData = front15fpsPrevCmdTbl;
		//	size = sizeof(front15fpsPrevCmdTbl);
		//	break;
		case _front20fpsPrevCmdTbl:
			pData = front20fpsPrevCmdTbl;
			size = sizeof(front20fpsPrevCmdTbl);
			break;
		//case _front24fpsPrevCmdTbl:
		//	pData = front24fpsPrevCmdTbl;
		//	size = sizeof(front24fpsPrevCmdTbl);
		//	break;
		case _front30fpsPrevCmdTbl:
			pData = front30fpsPrevCmdTbl;
			size = sizeof(front30fpsPrevCmdTbl);
			break;
		case _front60fpsPrevCmdTbl:
			pData = front60fpsPrevCmdTbl;
			size = sizeof(front60fpsPrevCmdTbl);
			break;
		case _frontSnapCmdTbl:
			pData = frontSnapCmdTbl;
			size = sizeof(frontSnapCmdTbl);
			break;
	}

	
	sts = i2cCmdTableSend(pData, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	if (sts == FAIL) {
		printf("Sensor command table (0x%x)\n",fileId);
		ASSERT(0,1);
	}
	#endif
}

/**
 * @fn        void frontResTblBinRead(UINT16 fileId)
 * @brief	front Resource File Table Bin Read.
 * @param	fileId = [in] resource file id...
 * @retval	None.
 * @see
 * @author	LinJieCheng
 * @since	2010-12-07
 * @todo		N/A
 * @bug		N/A
*/
static void
frontResTblBinRead(
	UINT16 fileId,
	UINT16 byteAddress
)
{
	#if (LOAD_RES_OPTION == 1)
	UINT32 size;
	UINT8 sts;

	if (byteAddress & 0x0001) {
		ASSERT(!(byteAddress & 0x0001), 0); 
		return;
	}
	
	//printf("fileId = 0x%04x, byteAddress = 0x%04x\n",fileId,byteAddress);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_TBL_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((byteAddress - 0x4000) >> 1)) << 1));
	}while(!sts);
	#else
	fileId = fileId;
	byteAddress = byteAddress;
	#endif	
}

static void
OV5633FlashWBSet(
	void
)
{
#if 0
	UINT8 tmp_gain0, tmp_gain1;
	UINT16 r_gain, gr_gain, b_gain, gb_gain;
	float ratio=1.0;

	tmp_gain0 = XBYTE[0x2154]; tmp_gain1 = XBYTE[0x2155]; r_gain = ((UINT16) tmp_gain0) | ((UINT16) tmp_gain1 %0x07);
	tmp_gain0 = XBYTE[0x2156]; tmp_gain1 = XBYTE[0x2157]; gr_gain = ((UINT16) tmp_gain0) | ((UINT16) tmp_gain1 %0x07);
	tmp_gain0 = XBYTE[0x2158]; tmp_gain1 = XBYTE[0x2159]; b_gain = ((UINT16) tmp_gain0) | ((UINT16) tmp_gain1 %0x07);
	tmp_gain0 = XBYTE[0x215A]; tmp_gain1 = XBYTE[0x215B]; gb_gain = ((UINT16) tmp_gain0) | ((UINT16) tmp_gain1 %0x07);

	if ((((G_FlashStatus == K_FlashLightOn)&&(gId<10))||((G_FlashStatus == K_FlashLightAuto) && (gId <= 0)))&& (/*G_FlashCharging > */1)/*&&(flashwidth!=0)*/)
		STROBE = 1;
	else
		STROBE = 0;


	if ( STROBE )
   	{
  		if ( gId > EV9_index )
		{
			FR =r_gain;
			FG1=gr_gain;
			FB =b_gain;
			FG2=gb_gain;
		}
	   	else if ( gId > EV8_index )
		{
		 	FR =STB_RGain *0.2+0.8*r_gain;
		 	FG1=STB_GrGain*0.2+0.8*gr_gain;
			FB =STB_BGain *0.2+0.8*b_gain;
		 	FG2=STB_GbGain*0.2+0.8*gb_gain;
		}
		else if ( gId > EV7_index )
		{
		 	FR =(STB_RGain *0.5+0.5*r_gain);
		 	FG1=(STB_GrGain*0.5+0.5*gr_gain);
         	FB =(STB_BGain *0.5+0.5*b_gain);
			FG2=(STB_GbGain*0.5+0.5*gb_gain);
		}
		else if ( (gId <= EV7_index) && (G_luma > 80) )
		{
		 	FR =STB_RGain *0.8+0.2*r_gain;
		 	FG1=STB_GrGain*0.8+0.2*gr_gain;
         	FB =STB_BGain *0.8+0.2*b_gain;
		 	FG2=STB_GbGain*0.8+0.2*gb_gain;
		}
  		else
		{
		 	FR = STB_RGain;
		 	FG1 = STB_GrGain;
		 	FB =STB_BGain;
		 	FG2 =STB_GbGain;
		}

		tmp_gain0 = FR;
		if ( FG1 < tmp_gain0 ) tmp_gain0 = FG1;
		if ( FB  < tmp_gain0 ) tmp_gain0 = FB;
		if ( FG2 < tmp_gain0 ) tmp_gain0 = FG2;

		ratio = 64/(float)tmp_gain0;

		FR *= ratio;
		FG1*= ratio;
		FB *= ratio;
		FG2*= ratio;

		if (FR > AWBMaxGain) 	FR=AWBMaxGain;
	    	if (FR < AWBMinGain) 	FR=AWBMinGain;
		if (FG1> AWBMaxGain) 	FG1=AWBMaxGain;
        	if (FG1< AWBMinGain) 	FG1=AWBMinGain;
		if (FB > AWBMaxGain) 	FB=AWBMaxGain;
        	if (FB < AWBMinGain) 	FB=AWBMinGain;
		if (FG2> AWBMaxGain) 	FG2=AWBMaxGain;
        	if (FG2< AWBMinGain) 	FG2=AWBMinGain;

        HAL_SetWBGainSet(FR, FG1, FB, FG2);
        halCdspWBGainSet( 1 );         
 	}
#endif	
}


#if FUNC_HOST_IGBT_FLASH  /*MI2020*/

#define SENSOR_GAIN_1X_INDEX 0
#define SENSOR_GAIN_2X_INDEX 16
#define SENSOR_GAIN_4X_INDEX 48
#define SENSOR_GAIN_8X_INDEX 80

void frontLastShutterGainSet(UINT32 shutter, UINT8 gain)
{
	frontPrevCapabDesc_t *pcap;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	LoadBackfrontLastGainIdx = gain; /*Add for load preview setting after flash snap*/
	
	frontLastGain = pcap->gainTbl[gain];
	frontLastShutter = shutter;
}

sensor_flash_gain_t sensorFlashGainIdxGet(void)
{
	UINT8 currGain;
	sensor_flash_gain_t flashGainIdx;

	aeStatusGet(SP1K_AE_STS_G_GAIN , &currGain);	
	if(currGain < SENSOR_GAIN_2X_INDEX){
		flashGainIdx = SENSOR_GAIN_1X; 
	}
	else if(currGain < SENSOR_GAIN_4X_INDEX){
		flashGainIdx = SENSOR_GAIN_2X;
	}
	else if(currGain < /*SENSOR_GAIN_8X_INDEX*/79){
		flashGainIdx = SENSOR_GAIN_4X;
	}
	else{
		flashGainIdx = SENSOR_GAIN_8X;
	}
	return flashGainIdx;
}

UINT8 sensorGainIdxGet(sensor_flash_gain_t flashGainIdx)
{
	UINT8 gainIdx;

	switch(flashGainIdx){
		case  SENSOR_GAIN_1X:
			gainIdx = SENSOR_GAIN_1X_INDEX;
			break;
		case  SENSOR_GAIN_2X:
			gainIdx = SENSOR_GAIN_2X_INDEX; 
			break;
		case  SENSOR_GAIN_4X:
			gainIdx = SENSOR_GAIN_4X_INDEX;
			break;
		case  SENSOR_GAIN_8X:
			gainIdx = SENSOR_GAIN_8X_INDEX;
			break;
	}
	
	return gainIdx;

}
UINT16 sensorPvToSnapFrmTimeRatio(void)
{
	UINT16 ratio;

	UINT32 capTime;
	UINT32 preTime;

	capTime = ( (((UINT32)LINE_TOT_SNAP * (UINT32)FRAME_TOT_SNAP) <<8) /(UINT32)PCLK_FREQ_SNAP>>2);

	preTime = ( (((UINT32)LINE_TOT_30FPS_PREV * (UINT32)FRAME_TOT_30FPS_PREV) <<8) /(UINT32)PCLK_FREQ_30FPS_PREV>>2);

	ratio = (UINT16)((capTime<<8)/preTime);

	if(ratio > 300){
		ratio = ratio - 50;
	}

	ratio = 450;
	printf("\033[32mratio=%u\033[0m\n",ratio);
	return ratio;
}
#endif

	
