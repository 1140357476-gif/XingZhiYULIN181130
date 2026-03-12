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
 * @file		front_ov_ov9712_raw.c
 * @brief		front sensor omni vision ov9712 driver.
 * @author		Matt Wang
 * @since		2008-09-24
 * @date		2008-12-09
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
#include "front_logi.h"
#include "gpio_api.h"
#include "i2c.h"
#include "sp1k_util_api.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "ae_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_hal_api.h"
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

#define USED_INTERNAL_LDO	1	/* old HW : 0(external ldo), new HW : 1(internal ldo) */
#define SNAP_KEEP_FPS		0	/* add dummy line : 0(reduce fps), keep fps : 1(exposure line equal to frame total-6) */
#define PV_AUTO_OB_BY_GAIN	1	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
#define RES_LSC_LOAD		0	/* after power on load lsc resource file */

/* mode. */
#define PREV_MODE_TOT	3
#define SNAP_MODE_TOT	1
#define PREV_RESP		720


/* dimensiion. */
#if (PREV_RESP==720)
/* 720p, dyn 30->15 fps */
#define WIDTH_24FPS_PREV      1280
#define HEIGHT_24FPS_PREV     720
#define XOFF_24FPS_PREV       440
#define YOFF_24FPS_PREV       1
#define LINE_TOT_24FPS_PREV   1720
#define FRAME_TOT_24FPS_PREV  844
#define PREV_TO_SNAP_BINNING_RATE_24FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_24FPS_PREV         0

/* 720p, real 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       440
#define YOFF_30FPS_PREV       1
#define LINE_TOT_30FPS_PREV   1720
#define FRAME_TOT_30FPS_PREV  844
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0

/* 480p, real 60fps */
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     400
#define XOFF_60FPS_PREV       204
#define YOFF_60FPS_PREV       1
#define LINE_TOT_60FPS_PREV   1688
#define FRAME_TOT_60FPS_PREV  414
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_60FPS_PREV         0
#endif

/* 1.2M */
#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     720
#define XOFF_SNAP       440
#define YOFF_SNAP       1
#define LINE_TOT_SNAP   1720
#define FRAME_TOT_SNAP  844

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_24FPS_PREV    12
#define MCLK_PHASE_24FPS_PREV  0
#define PCLK_DIV_24FPS_PREV    12
#define PCLK_PHASE_24FPS_PREV  0

#define MCLK_DIV_30FPS_PREV    12
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    12
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP           12
#define MCLK_DIV_SNAP_BURST     12
#define MCLK_DIV_SNAP_BURST_UP  12
#define MCLK_PHASE_SNAP         0
#define PCLK_DIV_SNAP           12
#define PCLK_PHASE_SNAP         0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_24FPS_PREV     43500000L
#define PCLK_FREQ_24FPS_PREV_TD  43500000L
#define PCLK_FREQ_30FPS_PREV     43500000L
#define PCLK_FREQ_30FPS_PREV_TD  43500000L
#define PCLK_FREQ_60FPS_PREV     21000000L
#define PCLK_FREQ_60FPS_PREV_TD  21000000L
#define PCLK_FREQ_SNAP           43500000L
#define PCLK_FREQ_SNAP_BURST  	 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP  41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */
#else
#define PCLK_FREQ_24FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_24FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP        522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP 522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_24FPS_PREV    0
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HEN_60FPS_PREV    0
#else
#define RESHAPE_HEN_24FPS_PREV    0	/* bypass h don't enable */
#define RESHAPE_HEN_30FPS_PREV    0	/* bypass h don't enable */
#define RESHAPE_HEN_60FPS_PREV    0	/* bypass h don't enable */
#endif
#define RESHAPE_HFALL_24FPS_PREV  2
#define RESHAPE_HRISE_24FPS_PREV  16
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  16
#define RESHAPE_HFALL_60FPS_PREV  2
#define RESHAPE_HRISE_60FPS_PREV  16
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_24FPS_PREV    0
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_24FPS_PREV    0	/* bypass v don't enable */
#define RESHAPE_VEN_30FPS_PREV    0	/* bypass v don't enable */
#define RESHAPE_VEN_60FPS_PREV    0	/* bypass v don't enable */
#endif
#define RESHAPE_VFALL_24FPS_PREV  2
#define RESHAPE_VRISE_24FPS_PREV  3
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  3
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

/* preview h,v sync inv. */
#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

/* snapshot h,v sync inv. */
#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GRRBGB
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_GRRBGB

/* i2c interface. */
#define I2C_DEV_ADDR  0x60//0x61
#define I2C_CLK_DIV   2

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_24FPS_24_MAX_IDX  195
#define AE_24FPS_24_MIN_IDX  0
#define AE_24FPS_20_MAX_IDX  192
#define AE_24FPS_20_MIN_IDX  0
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  180
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  178
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  148
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  141
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
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor
																
/* exposure, gain setting position */
#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
																
/* gain setting position */															
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					//AE_GAIN_AFTER_EXP_OFF,
																//AE_GAIN_AFTER_EXP_ON

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

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV9712 RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table
 *					CvtTypeLnkAddr: convert to 
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
	_ae20fps50hzTbl = 0x00C2,
	_ae20fps60hzTbl = 0x00C3,
	//_ae24fps50hzTbl = 0x00C4,
	//_ae24fps60hzTbl = 0x00C5,
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
	//_front24fpsPrevCmdTbl	= 0x00D2,
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

/* dynamic fps 30->15, 50Hz */
static code frontAe_t ae20fps50hzTbl[193] = {	
	{	0,	166,	47,	0,	0 },
	{	1,	166,	46,	0,	0 },
	{	2,	166,	45,	0,	0 },
	{	3,	166,	44,	0,	0 },
	{	4,	166,	42,	0,	0 },
	{	5,	166,	41,	0,	0 },
	{	6,	166,	40,	0,	0 },
	{	7,	166,	39,	0,	0 },
	{	8,	166,	38,	0,	0 },
	{	9,	166,	37,	0,	0 },
	{	10,	166,	36,	0,	0 },
	{	11,	166,	35,	0,	0 },
	{	12,	166,	34,	0,	0 },
	{	13,	166,	33,	0,	0 },
	{	14,	166,	31,	0,	0 },
	{	15,	166,	30,	0,	0 },
	{	16,	166,	29,	0,	0 },
	{	17,	166,	28,	0,	0 },
	{	18,	166,	26,	0,	0 },
	{	19,	166,	25,	0,	0 },
	{	20,	166,	24,	0,	0 },
	{	21,	166,	23,	0,	0 },
	{	22,	166,	22,	0,	0 },
	{	23,	166,	21,	0,	0 },
	{	24,	166,	20,	0,	0 },
	{	25,	166,	19,	0,	0 },
	{	26,	166,	18,	0,	0 },
	{	27,	332,	33,	0,	0 },
	{	28,	332,	31,	0,	0 },
	{	29,	332,	30,	0,	0 },
	{	30,	332,	29,	0,	0 },
	{	31,	332,	28,	0,	0 },
	{	32,	332,	26,	0,	0 },
	{	33,	332,	25,	0,	0 },
	{	34,	332,	24,	0,	0 },
	{	35,	332,	23,	0,	0 },
	{	36,	332,	22,	0,	0 },
	{	37,	332,	21,	0,	0 },
	{	38,	332,	20,	0,	0 },
	{	39,	332,	19,	0,	0 },
	{	40,	332,	18,	0,	0 },
	{	41,	332,	17,	0,	0 },
	{	42,	332,	15,	0,	0 },
	{	43,	332,	14,	0,	0 },
	{	44,	332,	13,	0,	0 },
	{	45,	332,	12,	0,	0 },
	{	46,	332,	10,	0,	0 },
	{	47,	332,	9,	0,	0 },
	{	48,	332,	8,	0,	0 },
	{	49,	332,	7,	0,	0 },
	{	50,	332,	6,	0,	0 },
	{	51,	332,	5,	0,	0 },
	{	52,	332,	4,	0,	0 },
	{	53,	332,	3,	0,	0 },
	{	54,	332,	2,	0,	0 },
	{	55,	332,	1,	0,	0 },
	{	56,	332,	0,	0,	0 },
	{	57,	498,	7,	0,	0 },
	{	58,	498,	6,	0,	0 },
	{	59,	498,	5,	0,	0 },
	{	60,	498,	4,	0,	0 },
	{	61,	498,	3,	0,	0 },
	{	62,	498,	2,	0,	0 },
	{	63,	498,	1,	0,	0 },
	{	64,	498,	0,	0,	0 },
	{	65,	501,	0,	0,	0 },
	{	66,	992,	13,	0,	0 },
	{	67,	992,	12,	0,	0 },
	{	68,	992,	11,	0,	0 },
	{	69,	992,	10,	0,	0 },
	{	70,	992,	9,	0,	0 },
	{	71,	992,	8,	0,	0 },
	{	72,	992,	7,	0,	0 },
	{	73,	992,	6,	0,	0 },
	{	74,	992,	5,	0,	0 },
	{	75,	992,	4,	0,	0 },
	{	76,	992,	3,	0,	0 },
	{	77,	992,	2,	0,	0 },
	{	78,	992,	1,	0,	0 },
	{	79,	992,	0,	0,	0 },
	{	80,	1004,	0,	0,	0 },
	{	81,	1049,	0,	0,	0 },
	{	82,	1095,	0,	0,	0 },
	{	83,	1144,	0,	0,	0 },
	{	84,	1193,	0,	0,	0 },
	{	85,	1246,	0,	0,	0 },
	{	86,	1304,	0,	0,	0 },
	{	87,	1360,	0,	0,	0 },
	{	88,	1421,	0,	0,	0 },
	{	89,	1488,	0,	0,	0 },
	{	90,	1552,	0,	0,	0 },
	{	91,	1621,	0,	0,	0 },
	{	92,	1686,	0,	0,	0 },
	{	93,	1769,	0,	0,	0 },
	{	94,	1846,	0,	0,	0 },
	{	95,	1931,	0,	0,	0 },
	{	96,	2007,	0,	0,	0 },
	{	97,	2108,	0,	0,	0 },
	{	98,	2199,	0,	0,	0 },
	{	99,	2299,	0,	0,	0 },
	{	100,	2386,	0,	0,	0 },
	{	101,	2504,	0,	0,	0 },
	{	102,	2607,	0,	0,	0 },
	{	103,	2719,	0,	0,	0 },
	{	104,	2842,	0,	0,	0 },
	{	105,	2975,	0,	0,	0 },
	{	106,	3122,	0,	0,	0 },
	{	107,	3242,	0,	0,	0 },
	{	108,	3372,	0,	0,	0 },
	{	109,	3562,	0,	0,	0 },
	{	110,	3719,	0,	0,	0 },
	{	111,	3891,	0,	0,	0 },
	{	112,	4014,	0,	0,	0 },
	{	113,	4215,	0,	0,	0 },
	{	114,	4437,	0,	0,	0 },
	{	115,	4598,	0,	0,	0 },
	{	116,	4772,	0,	0,	0 },
	{	117,	5058,	0,	0,	0 },
	{	118,	5269,	0,	0,	0 },
	{	119,	5498,	0,	0,	0 },
	{	120,	5748,	0,	0,	0 },
	{	121,	6022,	0,	0,	0 },
	{	122,	6323,	0,	0,	0 },
	{	123,	6485,	0,	0,	0 },
	{	124,	6835,	0,	0,	0 },
	{	125,	7226,	0,	0,	0 },
	{	126,	7438,	0,	0,	0 },
	{	127,	7903,	0,	0,	0 },
	{	128,	8158,	0,	0,	0 },
	{	129,	8430,	0,	0,	0 },
	{	130,	9032,	0,	0,	0 },
	{	131,	9367,	0,	0,	1 },
	{	132,	9727,	0,	0,	0 },
	{	133,	10116,	0,	0,	0 },
	{	134,	10538,	0,	0,	1 },
	{	135,	10996,	0,	0,	1 },
	{	136,	11496,	0,	0,	1 },
	{	137,	12043,	0,	0,	0 },
	{	138,	12645,	0,	0,	0 },
	{	139,	13311,	0,	0,	1 },
	{	140,	14050,	0,	0,	0 },
	{	141,	14877,	0,	0,	1 },
	{	142,	15807,	0,	0,	1 },
	{	143,	16860,	0,	0,	0 },
	{	144,	18065,	0,	0,	1 },
	{	145,	19454,	1,	0,	0 },
	{	146,	19454,	0,	0,	0 },
	{	147,	21075,	0,	0,	0 },
	{	148,	22991,	0,	0,	0 },
	{	149,	25290,	1,	0,	0 },
	{	150,	25290,	0,	0,	0 },
	{	151,	28100,	1,	0,	0 },
	{	152,	28100,	0,	0,	0 },
	{	153,	31613,	1,	0,	0 },
	{	154,	31613,	0,	0,	0 },
	{	155,	36129,	1,	0,	0 },
	{	156,	36129,	0,	0,	0 },
	{	157,	42150,	2,	0,	0 },
	{	158,	42150,	1,	0,	0 },
	{	159,	42150,	0,	0,	0 },
	{	160,	50580,	2,	0,	0 },
	{	161,	50580,	1,	0,	0 },
	{	162,	50580,	0,	0,	0 },
	{	163,	63225,	3,	0,	0 },
	{	164,	63225,	2,	0,	0 },
	{	165,	63225,	1,	0,	0 },
	{	166,	63225,	0,	0,	0 },
	{	167,	84300,	4,	0,	0 },
	{	168,	84300,	3,	0,	0 },
	{	169,	84300,	2,	0,	0 },
	{	170,	84300,	1,	0,	0 },
	{	171,	84300,	0,	0,	0 },
	{	172,	126447,	6,	0,	0 },
	{	173,	126447,	5,	0,	0 },
	{	174,	126447,	4,	0,	0 },
	{	175,	126447,	3,	0,	0 },
	{	176,	126447,	2,	0,	0 },
	{	177,	126447,	1,	0,	0 },
	{	178,	126447,	0,	0,	0 },
	{	179,	252882,	14,	0,	0 },
	{	180,	252882,	13,	0,	0 },
	{	181,	252882,	12,	0,	0 },
	{	182,	252882,	11,	0,	0 },
	{	183,	252882,	9,	0,	0 },
	{	184,	252882,	8,	0,	0 },
	{	185,	252882,	7,	0,	0 },
	{	186,	252882,	6,	0,	0 },
	{	187,	252882,	5,	0,	0 },
	{	188,	252882,	4,	0,	0 },
	{	189,	252882,	3,	0,	0 },
	{	190,	252882,	2,	0,	0 },
	{	191,	252882,	1,	0,	0 },
	{	192,	252882,	0,	0,	0 },
};

/* dynamic fps 30->15, 60Hz */
static code frontAe_t ae20fps60hzTbl[196] = {	
	{	0,	150,	48,	0,	0 },
	{	1,	150,	47,	0,	0 },
	{	2,	150,	46,	0,	0 },
	{	3,	150,	44,	0,	0 },
	{	4,	150,	43,	0,	0 },
	{	5,	150,	42,	0,	0 },
	{	6,	150,	41,	0,	0 },
	{	7,	150,	40,	0,	0 },
	{	8,	150,	39,	0,	0 },
	{	9,	150,	38,	0,	0 },
	{	10,	150,	37,	0,	0 },
	{	11,	150,	36,	0,	0 },
	{	12,	150,	35,	0,	0 },
	{	13,	150,	34,	0,	0 },
	{	14,	150,	33,	0,	0 },
	{	15,	150,	32,	0,	0 },
	{	16,	150,	31,	0,	0 },
	{	17,	150,	30,	0,	0 },
	{	18,	150,	28,	0,	0 },
	{	19,	150,	27,	0,	0 },
	{	20,	150,	26,	0,	0 },
	{	21,	150,	25,	0,	0 },
	{	22,	150,	24,	0,	0 },
	{	23,	150,	23,	0,	0 },
	{	24,	150,	22,	0,	0 },
	{	25,	150,	21,	0,	0 },
	{	26,	150,	20,	0,	0 },
	{	27,	150,	19,	0,	0 },
	{	28,	150,	18,	0,	0 },
	{	29,	299,	33,	0,	0 },
	{	30,	299,	32,	0,	0 },
	{	31,	299,	31,	0,	0 },
	{	32,	299,	30,	0,	0 },
	{	33,	299,	28,	0,	0 },
	{	34,	299,	27,	0,	0 },
	{	35,	299,	26,	0,	0 },
	{	36,	299,	25,	0,	0 },
	{	37,	299,	24,	0,	0 },
	{	38,	299,	23,	0,	0 },
	{	39,	299,	22,	0,	0 },
	{	40,	299,	21,	0,	0 },
	{	41,	299,	20,	0,	0 },
	{	42,	299,	19,	0,	0 },
	{	43,	299,	18,	0,	0 },
	{	44,	299,	17,	0,	0 },
	{	45,	299,	16,	0,	0 },
	{	46,	299,	15,	0,	0 },
	{	47,	299,	14,	0,	0 },
	{	48,	299,	12,	0,	0 },
	{	49,	299,	11,	0,	0 },
	{	50,	299,	10,	0,	0 },
	{	51,	299,	9,	0,	0 },
	{	52,	299,	8,	0,	0 },
	{	53,	299,	7,	0,	0 },
	{	54,	299,	6,	0,	0 },
	{	55,	299,	5,	0,	0 },
	{	56,	299,	4,	0,	0 },
	{	57,	299,	3,	0,	0 },
	{	58,	299,	2,	0,	0 },
	{	59,	299,	1,	0,	0 },
	{	60,	299,	0,	0,	0 },
	{	61,	399,	4,	0,	0 },
	{	62,	399,	3,	0,	0 },
	{	63,	399,	2,	0,	0 },
	{	64,	399,	1,	0,	0 },
	{	65,	399,	0,	0,	0 },
	{	66,	598,	7,	0,	0 },
	{	67,	598,	6,	0,	0 },
	{	68,	598,	5,	0,	0 },
	{	69,	598,	4,	0,	0 },
	{	70,	598,	3,	0,	0 },
	{	71,	598,	2,	0,	0 },
	{	72,	598,	1,	0,	0 },
	{	73,	598,	0,	0,	0 },
	{	74,	1193,	13,	0,	0 },
	{	75,	1193,	12,	0,	0 },
	{	76,	1193,	11,	0,	0 },
	{	77,	1193,	10,	0,	0 },
	{	78,	1193,	9,	0,	0 },
	{	79,	1193,	8,	0,	0 },
	{	80,	1193,	7,	0,	0 },
	{	81,	1193,	6,	0,	0 },
	{	82,	1193,	5,	0,	0 },
	{	83,	1193,	4,	0,	0 },
	{	84,	1193,	3,	0,	0 },
	{	85,	1193,	2,	0,	0 },
	{	86,	1193,	1,	0,	0 },
	{	87,	1193,	0,	0,	0 },
	{	88,	1246,	0,	0,	0 },
	{	89,	1304,	0,	0,	0 },
	{	90,	1360,	0,	0,	0 },
	{	91,	1421,	0,	0,	0 },
	{	92,	1488,	0,	0,	0 },
	{	93,	1552,	0,	0,	0 },
	{	94,	1621,	0,	0,	0 },
	{	95,	1686,	0,	0,	0 },
	{	96,	1769,	0,	0,	0 },
	{	97,	1846,	0,	0,	0 },
	{	98,	1931,	0,	0,	0 },
	{	99,	2007,	0,	0,	0 },
	{	100,	2108,	0,	0,	0 },
	{	101,	2199,	0,	0,	0 },
	{	102,	2299,	0,	0,	0 },
	{	103,	2386,	0,	0,	0 },
	{	104,	2504,	0,	0,	0 },
	{	105,	2607,	0,	0,	0 },
	{	106,	2719,	0,	0,	0 },
	{	107,	2842,	0,	0,	0 },
	{	108,	2975,	0,	0,	0 },
	{	109,	3122,	0,	0,	0 },
	{	110,	3242,	0,	0,	0 },
	{	111,	3372,	0,	0,	0 },
	{	112,	3562,	0,	0,	0 },
	{	113,	3719,	0,	0,	0 },
	{	114,	3891,	0,	0,	0 },
	{	115,	4014,	0,	0,	0 },
	{	116,	4215,	0,	0,	0 },
	{	117,	4437,	0,	0,	0 },
	{	118,	4598,	0,	0,	0 },
	{	119,	4772,	0,	0,	0 },
	{	120,	5058,	0,	0,	0 },
	{	121,	5269,	0,	0,	0 },
	{	122,	5498,	0,	0,	0 },
	{	123,	5748,	0,	0,	0 },
	{	124,	6022,	0,	0,	0 },
	{	125,	6323,	0,	0,	0 },
	{	126,	6485,	0,	0,	0 },
	{	127,	6835,	0,	0,	0 },
	{	128,	7226,	0,	0,	0 },
	{	129,	7438,	0,	0,	0 },
	{	130,	7903,	0,	0,	0 },
	{	131,	8158,	0,	0,	0 },
	{	132,	8430,	0,	0,	0 },
	{	133,	9032,	0,	0,	0 },
	{	134,	9367,	0,	0,	1 },
	{	135,	9727,	0,	0,	0 },
	{	136,	10116,	0,	0,	0 },
	{	137,	10538,	0,	0,	1 },
	{	138,	10996,	0,	0,	1 },
	{	139,	11496,	0,	0,	1 },
	{	140,	12043,	0,	0,	0 },
	{	141,	12645,	0,	0,	0 },
	{	142,	13311,	0,	0,	1 },
	{	143,	14050,	0,	0,	0 },
	{	144,	14877,	0,	0,	1 },
	{	145,	15807,	0,	0,	1 },
	{	146,	16860,	0,	0,	0 },
	{	147,	18065,	0,	0,	1 },
	{	148,	19454,	1,	0,	0 },
	{	149,	19454,	0,	0,	0 },
	{	150,	21075,	0,	0,	0 },
	{	151,	22991,	0,	0,	0 },
	{	152,	25290,	1,	0,	0 },
	{	153,	25290,	0,	0,	0 },
	{	154,	28100,	1,	0,	0 },
	{	155,	28100,	0,	0,	0 },
	{	156,	31613,	1,	0,	0 },
	{	157,	31613,	0,	0,	0 },
	{	158,	36129,	1,	0,	0 },
	{	159,	36129,	0,	0,	0 },
	{	160,	42150,	2,	0,	0 },
	{	161,	42150,	1,	0,	0 },
	{	162,	42150,	0,	0,	0 },
	{	163,	50580,	2,	0,	0 },
	{	164,	50580,	1,	0,	0 },
	{	165,	50580,	0,	0,	0 },
	{	166,	63225,	3,	0,	0 },
	{	167,	63225,	2,	0,	0 },
	{	168,	63225,	1,	0,	0 },
	{	169,	63225,	0,	0,	0 },
	{	170,	84300,	4,	0,	0 },
	{	171,	84300,	3,	0,	0 },
	{	172,	84300,	2,	0,	0 },
	{	173,	84300,	1,	0,	0 },
	{	174,	84300,	0,	0,	0 },
	{	175,	126447,	6,	0,	0 },
	{	176,	126447,	5,	0,	0 },
	{	177,	126447,	4,	0,	0 },
	{	178,	126447,	3,	0,	0 },
	{	179,	126447,	2,	0,	0 },
	{	180,	126447,	1,	0,	0 },
	{	181,	126447,	0,	0,	0 },
	{	182,	252882,	14,	0,	0 },
	{	183,	252882,	13,	0,	0 },
	{	184,	252882,	12,	0,	0 },
	{	185,	252882,	11,	0,	0 },
	{	186,	252882,	9,	0,	0 },
	{	187,	252882,	8,	0,	0 },
	{	188,	252882,	7,	0,	0 },
	{	189,	252882,	6,	0,	0 },
	{	190,	252882,	5,	0,	0 },
	{	191,	252882,	4,	0,	0 },
	{	192,	252882,	3,	0,	0 },
	{	193,	252882,	2,	0,	0 },
	{	194,	252882,	1,	0,	0 },
	{	195,	252882,	0,	0,	0 },
};

/* real fps 30, 50Hz */
static code frontAe_t ae30fps50hzTbl[179] = {
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
	{	66,	1004,	0,	0,	0 },
	{	67,	1049,	0,	0,	0 },
	{	68,	1095,	0,	0,	0 },
	{	69,	1144,	0,	0,	0 },
	{	70,	1193,	0,	0,	0 },
	{	71,	1246,	0,	0,	0 },
	{	72,	1304,	0,	0,	0 },
	{	73,	1360,	0,	0,	0 },
	{	74,	1421,	0,	0,	0 },
	{	75,	1488,	0,	0,	0 },
	{	76,	1552,	0,	0,	0 },
	{	77,	1621,	0,	0,	0 },
	{	78,	1686,	0,	0,	0 },
	{	79,	1769,	0,	0,	0 },
	{	80,	1846,	0,	0,	0 },
	{	81,	1931,	0,	0,	0 },
	{	82,	2007,	0,	0,	0 },
	{	83,	2108,	0,	0,	0 },
	{	84,	2199,	0,	0,	0 },
	{	85,	2299,	0,	0,	0 },
	{	86,	2386,	0,	0,	0 },
	{	87,	2504,	0,	0,	0 },
	{	88,	2607,	0,	0,	0 },
	{	89,	2719,	0,	0,	0 },
	{	90,	2842,	0,	0,	0 },
	{	91,	2975,	0,	0,	0 },
	{	92,	3122,	0,	0,	0 },
	{	93,	3242,	0,	0,	0 },
	{	94,	3372,	0,	0,	0 },
	{	95,	3562,	0,	0,	0 },
	{	96,	3719,	0,	0,	0 },
	{	97,	3891,	0,	0,	0 },
	{	98,	4014,	0,	0,	0 },
	{	99,	4215,	0,	0,	0 },
	{	100,	4437,	0,	0,	0 },
	{	101,	4598,	0,	0,	0 },
	{	102,	4772,	0,	0,	0 },
	{	103,	5058,	0,	0,	0 },
	{	104,	5269,	0,	0,	0 },
	{	105,	5498,	0,	0,	0 },
	{	106,	5748,	0,	0,	0 },
	{	107,	6022,	0,	0,	0 },
	{	108,	6323,	0,	0,	0 },
	{	109,	6485,	0,	0,	0 },
	{	110,	6835,	0,	0,	0 },
	{	111,	7226,	0,	0,	0 },
	{	112,	7438,	0,	0,	0 },
	{	113,	7903,	0,	0,	0 },
	{	114,	8158,	0,	0,	0 },
	{	115,	8430,	0,	0,	0 },
	{	116,	9032,	0,	0,	0 },
	{	117,	9367,	0,	0,	1 },
	{	118,	9727,	0,	0,	0 },
	{	119,	10116,	0,	0,	0 },
	{	120,	10538,	0,	0,	1 },
	{	121,	10996,	0,	0,	1 },
	{	122,	11496,	0,	0,	1 },
	{	123,	12043,	0,	0,	0 },
	{	124,	12645,	0,	0,	0 },
	{	125,	13311,	0,	0,	1 },
	{	126,	14050,	0,	0,	0 },
	{	127,	14877,	0,	0,	1 },
	{	128,	15807,	0,	0,	1 },
	{	129,	16860,	0,	0,	0 },
	{	130,	18065,	0,	0,	1 },
	{	131,	19454,	1,	0,	0 },
	{	132,	19454,	0,	0,	0 },
	{	133,	21075,	0,	0,	0 },
	{	134,	22991,	0,	0,	0 },
	{	135,	25290,	1,	0,	0 },
	{	136,	25290,	0,	0,	0 },
	{	137,	28100,	1,	0,	0 },
	{	138,	28100,	0,	0,	0 },
	{	139,	31613,	1,	0,	0 },
	{	140,	31613,	0,	0,	0 },
	{	141,	36129,	1,	0,	0 },
	{	142,	36129,	0,	0,	0 },
	{	143,	42150,	2,	0,	0 },
	{	144,	42150,	1,	0,	0 },
	{	145,	42150,	0,	0,	0 },
	{	146,	50580,	2,	0,	0 },
	{	147,	50580,	1,	0,	0 },
	{	148,	50580,	0,	0,	0 },
	{	149,	63225,	3,	0,	0 },
	{	150,	63225,	2,	0,	0 },
	{	151,	63225,	1,	0,	0 },
	{	152,	63225,	0,	0,	0 },
	{	153,	84300,	4,	0,	0 },
	{	154,	84300,	3,	0,	0 },
	{	155,	84300,	2,	0,	0 },
	{	156,	84300,	1,	0,	0 },
	{	157,	84300,	0,	0,	0 },
	{	158,	126447,	6,	0,	0 },
	{	159,	126447,	5,	0,	0 },
	{	160,	126447,	4,	0,	0 },
	{	161,	126447,	3,	0,	0 },
	{	162,	126447,	2,	0,	0 },
	{	163,	126447,	1,	0,	0 },
	{	164,	126447,	0,	0,	0 },
	{	165,	252882,	14,	0,	0 },
	{	166,	252882,	13,	0,	0 },
	{	167,	252882,	12,	0,	0 },
	{	168,	252882,	11,	0,	0 },
	{	169,	252882,	9,	0,	0 },
	{	170,	252882,	8,	0,	0 },
	{	171,	252882,	7,	0,	0 },
	{	172,	252882,	6,	0,	0 },
	{	173,	252882,	5,	0,	0 },
	{	174,	252882,	4,	0,	0 },
	{	175,	252882,	3,	0,	0 },
	{	176,	252882,	2,	0,	0 },
	{	177,	252882,	1,	0,	0 },
	{	178,	252882,	0,	0,	0 },
};

/* real fps 30, 60Hz */
static code frontAe_t ae30fps60hzTbl[181] = {
	{	0,	300,	48,	0,	0 },
	{	1,	300,	47,	0,	0 },
	{	2,	300,	46,	0,	0 },
	{	3,	300,	44,	0,	0 },
	{	4,	300,	43,	0,	0 },
	{	5,	300,	42,	0,	0 },
	{	6,	300,	41,	0,	0 },
	{	7,	300,	40,	0,	0 },
	{	8,	300,	39,	0,	0 },
	{	9,	300,	38,	0,	0 },
	{	10,	300,	37,	0,	0 },
	{	11,	300,	36,	0,	0 },
	{	12,	300,	35,	0,	0 },
	{	13,	300,	34,	0,	0 },
	{	14,	300,	33,	0,	0 },
	{	15,	300,	32,	0,	0 },
	{	16,	300,	31,	0,	0 },
	{	17,	300,	30,	0,	0 },
	{	18,	300,	28,	0,	0 },
	{	19,	300,	27,	0,	0 },
	{	20,	300,	26,	0,	0 },
	{	21,	300,	25,	0,	0 },
	{	22,	300,	24,	0,	0 },
	{	23,	300,	23,	0,	0 },
	{	24,	300,	22,	0,	0 },
	{	25,	300,	21,	0,	0 },
	{	26,	300,	20,	0,	0 },
	{	27,	300,	19,	0,	0 },
	{	28,	300,	18,	0,	0 },
	{	29,	300,	17,	0,	0 },
	{	30,	300,	16,	0,	0 },
	{	31,	300,	15,	0,	0 },
	{	32,	300,	14,	0,	0 },
	{	33,	300,	12,	0,	0 },
	{	34,	300,	11,	0,	0 },
	{	35,	300,	10,	0,	0 },
	{	36,	300,	9,	0,	0 },
	{	37,	300,	8,	0,	0 },
	{	38,	300,	7,	0,	0 },
	{	39,	300,	6,	0,	0 },
	{	40,	300,	5,	0,	0 },
	{	41,	300,	4,	0,	0 },
	{	42,	300,	3,	0,	0 },
	{	43,	300,	2,	0,	0 },
	{	44,	300,	1,	0,	0 },
	{	45,	300,	0,	0,	0 },
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
	{	73,	1246,	0,	0,	0 },
	{	74,	1304,	0,	0,	0 },
	{	75,	1360,	0,	0,	0 },
	{	76,	1421,	0,	0,	0 },
	{	77,	1488,	0,	0,	0 },
	{	78,	1552,	0,	0,	0 },
	{	79,	1621,	0,	0,	0 },
	{	80,	1686,	0,	0,	0 },
	{	81,	1769,	0,	0,	0 },
	{	82,	1846,	0,	0,	0 },
	{	83,	1931,	0,	0,	0 },
	{	84,	2007,	0,	0,	0 },
	{	85,	2108,	0,	0,	0 },
	{	86,	2199,	0,	0,	0 },
	{	87,	2299,	0,	0,	0 },
	{	88,	2386,	0,	0,	0 },
	{	89,	2504,	0,	0,	0 },
	{	90,	2607,	0,	0,	0 },
	{	91,	2719,	0,	0,	0 },
	{	92,	2842,	0,	0,	0 },
	{	93,	2975,	0,	0,	0 },
	{	94,	3122,	0,	0,	0 },
	{	95,	3242,	0,	0,	0 },
	{	96,	3372,	0,	0,	0 },
	{	97,	3562,	0,	0,	0 },
	{	98,	3719,	0,	0,	0 },
	{	99,	3891,	0,	0,	0 },
	{	100,	4014,	0,	0,	0 },
	{	101,	4215,	0,	0,	0 },
	{	102,	4437,	0,	0,	0 },
	{	103,	4598,	0,	0,	0 },
	{	104,	4772,	0,	0,	0 },
	{	105,	5058,	0,	0,	0 },
	{	106,	5269,	0,	0,	0 },
	{	107,	5498,	0,	0,	0 },
	{	108,	5748,	0,	0,	0 },
	{	109,	6022,	0,	0,	0 },
	{	110,	6323,	0,	0,	0 },
	{	111,	6485,	0,	0,	0 },
	{	112,	6835,	0,	0,	0 },
	{	113,	7226,	0,	0,	0 },
	{	114,	7438,	0,	0,	0 },
	{	115,	7903,	0,	0,	0 },
	{	116,	8158,	0,	0,	0 },
	{	117,	8430,	0,	0,	0 },
	{	118,	9032,	0,	0,	0 },
	{	119,	9367,	0,	0,	1 },
	{	120,	9727,	0,	0,	0 },
	{	121,	10116,	0,	0,	0 },
	{	122,	10538,	0,	0,	1 },
	{	123,	10996,	0,	0,	1 },
	{	124,	11496,	0,	0,	1 },
	{	125,	12043,	0,	0,	0 },
	{	126,	12645,	0,	0,	0 },
	{	127,	13311,	0,	0,	1 },
	{	128,	14050,	0,	0,	0 },
	{	129,	14877,	0,	0,	1 },
	{	130,	15807,	0,	0,	1 },
	{	131,	16860,	0,	0,	0 },
	{	132,	18065,	0,	0,	1 },
	{	133,	19454,	1,	0,	0 },
	{	134,	19454,	0,	0,	0 },
	{	135,	21075,	0,	0,	0 },
	{	136,	22991,	0,	0,	0 },
	{	137,	25290,	1,	0,	0 },
	{	138,	25290,	0,	0,	0 },
	{	139,	28100,	1,	0,	0 },
	{	140,	28100,	0,	0,	0 },
	{	141,	31613,	1,	0,	0 },
	{	142,	31613,	0,	0,	0 },
	{	143,	36129,	1,	0,	0 },
	{	144,	36129,	0,	0,	0 },
	{	145,	42150,	2,	0,	0 },
	{	146,	42150,	1,	0,	0 },
	{	147,	42150,	0,	0,	0 },
	{	148,	50580,	2,	0,	0 },
	{	149,	50580,	1,	0,	0 },
	{	150,	50580,	0,	0,	0 },
	{	151,	63225,	3,	0,	0 },
	{	152,	63225,	2,	0,	0 },
	{	153,	63225,	1,	0,	0 },
	{	154,	63225,	0,	0,	0 },
	{	155,	84300,	4,	0,	0 },
	{	156,	84300,	3,	0,	0 },
	{	157,	84300,	2,	0,	0 },
	{	158,	84300,	1,	0,	0 },
	{	159,	84300,	0,	0,	0 },
	{	160,	126447,	6,	0,	0 },
	{	161,	126447,	5,	0,	0 },
	{	162,	126447,	4,	0,	0 },
	{	163,	126447,	3,	0,	0 },
	{	164,	126447,	2,	0,	0 },
	{	165,	126447,	1,	0,	0 },
	{	166,	126447,	0,	0,	0 },
	{	167,	252882,	14,	0,	0 },
	{	168,	252882,	13,	0,	0 },
	{	169,	252882,	12,	0,	0 },
	{	170,	252882,	11,	0,	0 },
	{	171,	252882,	9,	0,	0 },
	{	172,	252882,	8,	0,	0 },
	{	173,	252882,	7,	0,	0 },
	{	174,	252882,	6,	0,	0 },
	{	175,	252882,	5,	0,	0 },
	{	176,	252882,	4,	0,	0 },
	{	177,	252882,	3,	0,	0 },
	{	178,	252882,	2,	0,	0 },
	{	179,	252882,	1,	0,	0 },
	{	180,	252882,	0,	0,	0 },
};

/* real fps 60, 50Hz */
static code frontAe_t ae60fps50hzTbl[142] = {
	{	0,	995,	48,	0,	0 },
	{	1,	995,	47,	0,	0 },
	{	2,	995,	46,	0,	0 },
	{	3,	995,	44,	0,	0 },
	{	4,	995,	43,	0,	0 },
	{	5,	995,	42,	0,	0 },
	{	6,	995,	41,	0,	0 },
	{	7,	995,	40,	0,	0 },
	{	8,	995,	39,	0,	0 },
	{	9,	995,	38,	0,	0 },
	{	10,	995,	37,	0,	0 },
	{	11,	995,	36,	0,	0 },
	{	12,	995,	35,	0,	0 },
	{	13,	995,	34,	0,	0 },
	{	14,	995,	33,	0,	0 },
	{	15,	995,	32,	0,	0 },
	{	16,	995,	31,	0,	0 },
	{	17,	995,	30,	0,	0 },
	{	18,	995,	28,	0,	0 },
	{	19,	995,	27,	0,	0 },
	{	20,	995,	26,	0,	0 },
	{	21,	995,	25,	0,	0 },
	{	22,	995,	24,	0,	0 },
	{	23,	995,	23,	0,	0 },
	{	24,	995,	22,	0,	0 },
	{	25,	995,	21,	0,	0 },
	{	26,	995,	20,	0,	0 },
	{	27,	995,	19,	0,	0 },
	{	28,	995,	18,	0,	0 },
	{	29,	995,	17,	0,	0 },
	{	30,	995,	16,	0,	0 },
	{	31,	995,	15,	0,	0 },
	{	32,	995,	14,	0,	0 },
	{	33,	995,	12,	0,	0 },
	{	34,	995,	11,	0,	0 },
	{	35,	995,	10,	0,	0 },
	{	36,	995,	9,	0,	0 },
	{	37,	995,	8,	0,	0 },
	{	38,	995,	7,	0,	0 },
	{	39,	995,	6,	0,	0 },
	{	40,	995,	5,	0,	0 },
	{	41,	995,	4,	0,	0 },
	{	42,	995,	3,	0,	0 },
	{	43,	995,	2,	0,	0 },
	{	44,	995,	1,	0,	0 },
	{	45,	995,	0,	0,	0 },
	{	46,	1037,	0,	0,	0 },
	{	47,	1082,	0,	0,	0 },
	{	48,	1131,	0,	0,	0 },
	{	49,	1174,	0,	0,	0 },
	{	50,	1232,	0,	0,	0 },
	{	51,	1283,	0,	0,	0 },
	{	52,	1338,	0,	0,	0 },
	{	53,	1398,	0,	0,	0 },
	{	54,	1464,	0,	0,	0 },
	{	55,	1536,	0,	0,	0 },
	{	56,	1595,	0,	0,	0 },
	{	57,	1659,	0,	0,	0 },
	{	58,	1752,	0,	0,	0 },
	{	59,	1830,	0,	0,	0 },
	{	60,	1914,	0,	0,	0 },
	{	61,	1975,	0,	0,	0 },
	{	62,	2073,	0,	0,	0 },
	{	63,	2183,	0,	0,	0 },
	{	64,	2262,	0,	0,	0 },
	{	65,	2347,	0,	0,	0 },
	{	66,	2488,	0,	0,	0 },
	{	67,	2592,	0,	0,	0 },
	{	68,	2705,	0,	0,	0 },
	{	69,	2827,	0,	0,	0 },
	{	70,	2962,	0,	0,	0 },
	{	71,	3110,	0,	0,	0 },
	{	72,	3190,	0,	0,	0 },
	{	73,	3362,	0,	0,	0 },
	{	74,	3554,	0,	0,	0 },
	{	75,	3659,	0,	0,	0 },
	{	76,	3888,	0,	0,	0 },
	{	77,	4013,	0,	0,	0 },
	{	78,	4147,	0,	0,	0 },
	{	79,	4443,	0,	0,	0 },
	{	80,	4608,	0,	0,	0 },
	{	81,	4785,	0,	0,	0 },
	{	82,	4976,	0,	0,	0 },
	{	83,	5184,	0,	0,	0 },
	{	84,	5409,	0,	0,	0 },
	{	85,	5655,	0,	0,	0 },
	{	86,	5924,	0,	0,	0 },
	{	87,	6220,	0,	0,	0 },
	{	88,	6548,	0,	0,	0 },
	{	89,	6911,	0,	0,	0 },
	{	90,	7318,	0,	0,	0 },
	{	91,	7775,	0,	0,	0 },
	{	92,	8294,	0,	0,	0 },
	{	93,	8886,	0,	0,	0 },
	{	94,	9570,	1,	0,	0 },
	{	95,	9570,	0,	0,	0 },
	{	96,	10367,	0,	0,	0 },
	{	97,	11310,	0,	0,	0 },
	{	98,	12441,	1,	0,	0 },
	{	99,	12441,	0,	0,	0 },
	{	100,	13823,	1,	0,	0 },
	{	101,	13823,	0,	0,	0 },
	{	102,	15551,	1,	0,	0 },
	{	103,	15551,	0,	0,	0 },
	{	104,	17772,	1,	0,	0 },
	{	105,	17772,	0,	0,	0 },
	{	106,	20734,	2,	0,	0 },
	{	107,	20734,	1,	0,	0 },
	{	108,	20734,	0,	0,	0 },
	{	109,	24881,	2,	0,	0 },
	{	110,	24881,	1,	0,	0 },
	{	111,	24881,	0,	0,	0 },
	{	112,	31101,	3,	0,	0 },
	{	113,	31101,	2,	0,	0 },
	{	114,	31101,	1,	0,	0 },
	{	115,	31101,	0,	0,	0 },
	{	116,	41468,	4,	0,	0 },
	{	117,	41468,	3,	0,	0 },
	{	118,	41468,	2,	0,	0 },
	{	119,	41468,	1,	0,	0 },
	{	120,	41468,	0,	0,	0 },
	{	121,	62201,	6,	0,	0 },
	{	122,	62201,	5,	0,	0 },
	{	123,	62201,	4,	0,	0 },
	{	124,	62201,	3,	0,	0 },
	{	125,	62201,	2,	0,	0 },
	{	126,	62201,	1,	0,	0 },
	{	127,	62201,	0,	0,	0 },
	{	128,	124395,	14,	0,	0 },
	{	129,	124395,	13,	0,	0 },
	{	130,	124395,	12,	0,	0 },
	{	131,	124395,	11,	0,	0 },
	{	132,	124395,	9,	0,	0 },
	{	133,	124395,	8,	0,	0 },
	{	134,	124395,	7,	0,	0 },
	{	135,	124395,	6,	0,	0 },
	{	136,	124395,	5,	0,	0 },
	{	137,	124395,	4,	0,	0 },
	{	138,	124395,	3,	0,	0 },
	{	139,	124395,	2,	0,	0 },
	{	140,	124395,	1,	0,	0 },
	{	141,	124395,	0,	0,	0 },
};

/* real fps 60, 60Hz */
static code frontAe_t ae60fps60hzTbl[149] = {
	{	0,	598,	47,	0,	0 },
	{	1,	598,	46,	0,	0 },
	{	2,	598,	45,	0,	0 },
	{	3,	598,	44,	0,	0 },
	{	4,	598,	42,	0,	0 },
	{	5,	598,	41,	0,	0 },
	{	6,	598,	40,	0,	0 },
	{	7,	598,	39,	0,	0 },
	{	8,	598,	38,	0,	0 },
	{	9,	598,	37,	0,	0 },
	{	10,	598,	36,	0,	0 },
	{	11,	598,	35,	0,	0 },
	{	12,	598,	34,	0,	0 },
	{	13,	598,	33,	0,	0 },
	{	14,	598,	31,	0,	0 },
	{	15,	598,	30,	0,	0 },
	{	16,	598,	29,	0,	0 },
	{	17,	598,	28,	0,	0 },
	{	18,	598,	26,	0,	0 },
	{	19,	598,	25,	0,	0 },
	{	20,	598,	24,	0,	0 },
	{	21,	598,	23,	0,	0 },
	{	22,	598,	22,	0,	0 },
	{	23,	598,	21,	0,	0 },
	{	24,	598,	20,	0,	0 },
	{	25,	598,	19,	0,	0 },
	{	26,	598,	18,	0,	0 },
	{	27,	598,	17,	0,	0 },
	{	28,	598,	15,	0,	0 },
	{	29,	598,	14,	0,	0 },
	{	30,	598,	13,	0,	0 },
	{	31,	598,	12,	0,	0 },
	{	32,	598,	10,	0,	0 },
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
	{	43,	1196,	14,	0,	0 },
	{	44,	1196,	13,	0,	0 },
	{	45,	1196,	12,	0,	0 },
	{	46,	1196,	10,	0,	0 },
	{	47,	1196,	9,	0,	0 },
	{	48,	1196,	8,	0,	0 },
	{	49,	1196,	7,	0,	0 },
	{	50,	1196,	6,	0,	0 },
	{	51,	1196,	5,	0,	0 },
	{	52,	1196,	4,	0,	0 },
	{	53,	1196,	3,	0,	0 },
	{	54,	1196,	2,	0,	0 },
	{	55,	1196,	1,	0,	0 },
	{	56,	1196,	0,	0,	0 },
	{	57,	1232,	0,	0,	0 },
	{	58,	1283,	0,	0,	0 },
	{	59,	1338,	0,	0,	0 },
	{	60,	1398,	0,	0,	0 },
	{	61,	1464,	0,	0,	0 },
	{	62,	1536,	0,	0,	0 },
	{	63,	1595,	0,	0,	0 },
	{	64,	1659,	0,	0,	0 },
	{	65,	1752,	0,	0,	0 },
	{	66,	1830,	0,	0,	0 },
	{	67,	1914,	0,	0,	0 },
	{	68,	1975,	0,	0,	0 },
	{	69,	2073,	0,	0,	0 },
	{	70,	2183,	0,	0,	0 },
	{	71,	2262,	0,	0,	0 },
	{	72,	2347,	0,	0,	0 },
	{	73,	2488,	0,	0,	0 },
	{	74,	2592,	0,	0,	0 },
	{	75,	2705,	0,	0,	0 },
	{	76,	2827,	0,	0,	0 },
	{	77,	2962,	0,	0,	0 },
	{	78,	3110,	0,	0,	0 },
	{	79,	3190,	0,	0,	0 },
	{	80,	3362,	0,	0,	0 },
	{	81,	3554,	0,	0,	0 },
	{	82,	3659,	0,	0,	0 },
	{	83,	3888,	0,	0,	0 },
	{	84,	4013,	0,	0,	0 },
	{	85,	4147,	0,	0,	0 },
	{	86,	4443,	0,	0,	0 },
	{	87,	4608,	0,	0,	0 },
	{	88,	4785,	0,	0,	0 },
	{	89,	4976,	0,	0,	0 },
	{	90,	5184,	0,	0,	0 },
	{	91,	5409,	0,	0,	0 },
	{	92,	5655,	0,	0,	0 },
	{	93,	5924,	0,	0,	0 },
	{	94,	6220,	0,	0,	0 },
	{	95,	6548,	0,	0,	0 },
	{	96,	6911,	0,	0,	0 },
	{	97,	7318,	0,	0,	0 },
	{	98,	7775,	0,	0,	0 },
	{	99,	8294,	0,	0,	0 },
	{	100,	8886,	0,	0,	0 },
	{	101,	9570,	1,	0,	0 },
	{	102,	9570,	0,	0,	0 },
	{	103,	10367,	0,	0,	0 },
	{	104,	11310,	0,	0,	0 },
	{	105,	12441,	1,	0,	0 },
	{	106,	12441,	0,	0,	0 },
	{	107,	13823,	1,	0,	0 },
	{	108,	13823,	0,	0,	0 },
	{	109,	15551,	1,	0,	0 },
	{	110,	15551,	0,	0,	0 },
	{	111,	17772,	1,	0,	0 },
	{	112,	17772,	0,	0,	0 },
	{	113,	20734,	2,	0,	0 },
	{	114,	20734,	1,	0,	0 },
	{	115,	20734,	0,	0,	0 },
	{	116,	24881,	2,	0,	0 },
	{	117,	24881,	1,	0,	0 },
	{	118,	24881,	0,	0,	0 },
	{	119,	31101,	3,	0,	0 },
	{	120,	31101,	2,	0,	0 },
	{	121,	31101,	1,	0,	0 },
	{	122,	31101,	0,	0,	0 },
	{	123,	41468,	4,	0,	0 },
	{	124,	41468,	3,	0,	0 },
	{	125,	41468,	2,	0,	0 },
	{	126,	41468,	1,	0,	0 },
	{	127,	41468,	0,	0,	0 },
	{	128,	62201,	6,	0,	0 },
	{	129,	62201,	5,	0,	0 },
	{	130,	62201,	4,	0,	0 },
	{	131,	62201,	3,	0,	0 },
	{	132,	62201,	2,	0,	0 },
	{	133,	62201,	1,	0,	0 },
	{	134,	62201,	0,	0,	0 },
	{	135,	124395,	14,	0,	0 },
	{	136,	124395,	13,	0,	0 },
	{	137,	124395,	12,	0,	0 },
	{	138,	124395,	11,	0,	0 },
	{	139,	124395,	9,	0,	0 },
	{	140,	124395,	8,	0,	0 },
	{	141,	124395,	7,	0,	0 },
	{	142,	124395,	6,	0,	0 },
	{	143,	124395,	5,	0,	0 },
	{	144,	124395,	4,	0,	0 },
	{	145,	124395,	3,	0,	0 },
	{	146,	124395,	2,	0,	0 },
	{	147,	124395,	1,	0,	0 },
	{	148,	124395,	0,	0,	0 },
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
		MCLK_DIV_24FPS_PREV,
		MCLK_PHASE_24FPS_PREV,
		PCLK_DIV_24FPS_PREV,
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
		MCLK_DIV_60FPS_PREV,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV_TD,
		31,
		6,
	},
};

/* device capability. */
static code frontPrevCapabDesc_t frontPrevCapabDesc[PREV_MODE_TOT] = {
	{  /* dynamic fps 30->15. */
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
		ae20fps60hzTbl,
		ae20fps50hzTbl,
		ae24fpsMaxIdxTbl,
		ae24fpsMinIdxTbl,
		EV10_24FPS_EXP_IDX,
		gainTbl,
		GAIN_24FPS_MAX_IDX,
		GAIN_24FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[0],
		SENSOR_ZOOM_FACTOR_24FPS_PREV,
	},
	{  /* real 30 fps. */
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
	{  /* real 60 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
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
	/*
	;========	 OV9712 registers setting	=============
	;
	; OVT code base	:  53 platform
	; Customer 	:	iCatch
	; Back end Chip	:	
	; Rev. 		: 	v0.11
	; Date		:	2010/07/27
	;=========================================
	*/
	0x12, 0x80,   /*  Reset */
	
	//to frontInit1CmdTbl
};

static code UINT8 frontInit1CmdTbl[] = {
	/*Slave_ID=0x60;	*/			
	//0x12, 0x80, /*  Reset */  
	                 
	/*   Core Settings  */    
	/*	{0x12,0x00},*/
	0x1e, 0x07,            
	0x5f, 0x18,            
	0x69, 0x04,            
	0x65, 0x2a,            
	0x68, 0x0a,            
	0x39, 0x28,            
	0x4d, 0x90,            
	0xc1, 0x80,
	0x0c, 0x30,
	0x6d, 0x02,   /* high luma black issue */

	/*   DSP   */                               
	0x96, 0x01,   /* ISP disable, WB disable */
	0xbc, 0x68,                              

	/*   YAVG  */                         
	0x4e, 0x55,   /* AVERAGE */          
	0x4f, 0x55,                       
	0x50, 0x55,                       
	0x51, 0x55,                       
	0x24, 0x55,   /* Exposure windows  */
	0x25, 0x40,                       
	0x26, 0xa1,                       

	/*   General */                             
	0x13, 0x88,   /* disable AEC/AGC, enable LAEC(fine AE) */                            
	0x14, 0x40,   /*  Gain Ceiling 8 */ 

	#if (USED_INTERNAL_LDO == 1)
	0x63, 0x00,		/* internal LDO */
	#else
	0x63, 0x04,		/* external LDO */
	#endif
};

#if 0
static code UINT8 front24fpsPrevCmdTbl[] = {
	#if ( PREV_RESP == 720 )
	//;--------------------------------
	//;	1280x720 30fps key setting	
	//;	input	:	24MHZ
	//;	output	:	42MHZ
	//;	fps		:	30fps
	//; 	HTS		: 	1688
	//; 	VTS		:	828
	//;--------------------------------
	//;Line / Frame = {0x3E[7:0], 0x3D[7:0]} 
	//;Tp of Hsync = {0x2B[7:0], 0x2A[7:0]}
	//;--------------------------------
	/*  Resolution and Format  */
	0x12, 0x00,              
	0x3b, 0x00,              
	0x97, 0x80,              
	0x17, 0x25,   /*HStart MSB  = 37*8+7=303*/           
	0x18, 0xA2,   /*HSize MSB = A2*8+ 0=1296*/           
	0x19, 0x01,   /*VStart MSB = 1*8+ 2 = 10 */           
	0x1a, 0xCA,   /*VSize MSB = CA*4+2 =810 */           
	0x03, 0x0A,   /*VStart [1:0]LSB, VSize [3:2]LSB */           
	0x32, 0x07,   /*HStart [2:0]LSB, HSize [5:3]LSB */           
	0x98, 0x00,              
	0x99, 0x00,              
	0x9a, 0x00,              
	0x57, 0x00,   /*DSP output HSize[4:2] VSize[1:0] LSB */           
	0x58, 0xC8,   /*DSP output VSize MSB = C8*4+0 = 800 */         
	0x59, 0xA0,   /*DSP output HSize MSB = A0*8+0 = 1280 */           
	0x4c, 0x13,              
	0x4b, 0x36,   
	
	0x3e, 0x03,   /* Line / Frame hi 828*/   
	0x3d, 0x3C,   /* Line / Frame low */  		
	
	0xbd, 0xA0,      	
	0xbe, 0xC8,     
	
	#if 1
	/*  Clock */
	0x5c, 0x59,   
	0x5d, 0x00,   
	0x11, 0x00,    

	/* add dummy pixel for 24fps add 392 dummy pixel */
	0x2b, 0x08,   /* Tp of Hsync hi 1688*/
	0x2a, 0x20,   /* Tp of Hsync low */   
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */    	
        
	0x00, 0x20,   /*AGC default */
	#endif

	0x37, 0x02,   /*BLC by gain */
	
	#endif
};
#endif

static code UINT8 front30fpsPrevCmdTbl[] = {
	#if ( PREV_RESP == 720 )
	//;--------------------------------
	//;	1280x720 30fps key setting	
	//;	input	:	24MHZ
	//;	output	:	42MHZ
	//;	fps		:	30fps
	//; 	HTS		: 	1688
	//; 	VTS		:	828
	//;--------------------------------
	//;Line / Frame = {0x3E[7:0], 0x3D[7:0]} 
	//;Tp of Hsync = {0x2B[7:0], 0x2A[7:0]}
	//;--------------------------------
	/*  Resolution and Format  */
	0x12, 0x00,              
	0x3b, 0x00,              
	0x97, 0x80,              
	0x17, 0x25,   /*HStart MSB  = 37*8+7=303*/           
	0x18, 0xA2,   /*HSize MSB = A2*8+ 0=1296*/           
	0x19, 0x01,   /*VStart MSB = 1*8+ 2 = 10 */           
	0x1a, 0xCA,   /*VSize MSB = CA*4+2 =810 */           
	0x03, 0x0A,   /*VStart [1:0]LSB, VSize [3:2]LSB */           
	0x32, 0x07,   /*HStart [2:0]LSB, HSize [5:3]LSB */           
	0x98, 0x00,              
	0x99, 0x00,              
	0x9a, 0x00,              
	0x57, 0x00,   /*DSP output HSize[4:2] VSize[1:0] LSB */           
	0x58, 0xC8,   /*DSP output VSize MSB = C8*4+0 = 800 */         
	0x59, 0xA0,   /*DSP output HSize MSB = A0*8+0 = 1280 */           
	0x4c, 0x13,              
	0x4b, 0x36,   		
	
	0xbd, 0xA0,      	
	0xbe, 0xC8,     

	#if 1
	/*  Clock  */
	0x5c, 0x58,   /* mclk  real 21.75m, pclk real 43.5m */   	
	0x5d, 0x00,   
	0x11, 0x00,

	0x3e, 0x03,   /* Line / Frame hi 844*/   
	0x3d, 0x4C,   /* Line / Frame low */  		

	0x2b, 0x06,   /* Tp of Hsync hi 1720 */    
	0x2a, 0xb8,   /* Tp of Hsync low */
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */    	
	#endif
	
	#if 0
	/*  Clock  */
	0x5c, 0x59,   /* mclk  real 23.727272m, pclk real 41.522726m */ 	
	0x5d, 0x00,   
	0x11, 0x00,  

	0x3e, 0x03,   /* Line / Frame hi 828*/   
	0x3d, 0x3C,   /* Line / Frame low */ 
	
	0x2b, 0x06,   /* Tp of Hsync hi 1688*/	
	0x2a, 0x98,   /* Tp of Hsync low */
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */    	
	#endif

	0x37, 0x02,   /*BLC by gain */
	
	#endif
};

static code UINT8 front60fpsPrevCmdTbl[] = {	
	//;--------------------------------
	//;	640X400 60fps key setting	
	//;	input	:	24MHZ
	//;	output	: 	21MHZ
	//;	fps 		:	60fps
	//;  HTS		: 	1688
	//;  VTS		:	414
	//;--------------------------------
	//;Line / Frame = {0x3E[7:0], 0x3D[7:0]} 
	//;Tp of Hsync = {0x2B[7:0], 0x2A[7:0]}
	//;--------------------------------
	/*  Resolution and Format  */
	0x12, 0x40, 
	0x3b, 0x01, 
	0x97, 0x80, 
	0x17, 0x25,   /*HStart MSB  = 37*8+7=303*/   
	0x18, 0xA2,   /*HSize MSB = A2*8+ 0=1296*/
	0x19, 0x01,   /*VStart MSB = 1*8+ 2 = 10 */ 
	//0x1a, 0x64,   /*VSize MSB = 100*4+0 =400 */  
	//0x03, 0x02,   /*VStart [1:0]LSB, VSize [3:2]LSB */ 
	0x1a, 0x64,   /*VSize MSB = 100*4+2 =402 */  
	0x03, 0x0a,   /*VStart [1:0]LSB, VSize [3:2]LSB */ 
	0x32, 0x07,   /*HStart [2:0]LSB, HSize [5:3]LSB */ 
	0x98, 0x00, 
	0x99, 0x00, 
	0x9a, 0x00, 
	//0x57, 0x00,   /*DSP output HSize[4:2] VSize[1:0] LSB */  
	//0x58, 0x64,   /*DSP output VSize MSB = 100*4+0 = 400 */  
	0x57, 0x02,   /*DSP output HSize[4:2] VSize[1:0] LSB */  
	0x58, 0x64,   /*DSP output VSize MSB = 100*4+2 = 402 */  
	0x59, 0x50,   /*DSP output HSize MSB = 80*8+0 = 640 */   
	0x4b, 0x9a, 
	0x4c, 0x09, 
	
	0x3e, 0x01,   /* Line / Frame hi 414*/
	0x3d, 0x9e,   /* Line / Frame low */  
	
	0xbd, 0x50,   
	0xbe, 0x64, 
	0x2c, 0x60, 
	0x23, 0x10, 

	/*  Clock  */
	0x5c, 0x59,   
	0x5d, 0x00,   
	0x11, 0x00,   /* 01=30fps, 00=60fps */
	
	0x2b, 0x06,   /* Tp of Hsync hi 1688*/
	0x2a, 0x98,   /* Tp of Hsync low */
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */

	0x00, 0x20,   /*AGC default */

	0x37, 0x02,   /*BLC by gain */
};

static code UINT8 frontSnapCmdTbl[] = {
	//;--------------------------------
	//;	1280X720 30fps key setting	
	//;	input	:	24MHZ
	//;	output	:	42MHZ
	//;	fps		:	30 fps
	//; HTS		: 	1688
	//; VTS		:	828
	//;--------------------------------
	//;Line / Frame = {0x3E[7:0], 0x3D[7:0]} 
	//;Tp of Hsync = {0x2B[7:0], 0x2A[7:0]}
	//;--------------------------------
	/*	Resolution and Format  */
	0x12, 0x00,			  
	0x3b, 0x00,			  
	0x97, 0x80,			  
	0x17, 0x25,   /*HStart MSB  = 37*8+7=303*/ 		  
	0x18, 0xA2,   /*HSize MSB = A2*8+ 0=1296*/ 		  
	0x19, 0x01,   /*VStart MSB = 1*8+ 2 = 10 */		   
	0x1a, 0xCA,   /*VSize MSB = CA*4+2 =810 */ 		  
	0x03, 0x0A,   /*VStart [1:0]LSB, VSize [3:2]LSB */ 		  
	0x32, 0x07,   /*HStart [2:0]LSB, HSize [5:3]LSB */ 		  
	0x98, 0x00,			  
	0x99, 0x00,			  
	0x9a, 0x00,			  
	0x57, 0x00,   /*DSP output HSize[4:2] VSize[1:0] LSB */		   
	0x58, 0xC8,   /*DSP output VSize MSB = C8*4+0 = 800 */ 		
	0x59, 0xA0,   /*DSP output HSize MSB = A0*8+0 = 1280 */		   
	0x4c, 0x13,			  
	0x4b, 0x36,	
			  
	0xbd, 0xA0,			  
	0xbe, 0xC8,	

	#if 1
	/*  Clock  */
	0x5c, 0x58,   /* mclk  real 21.75m, pclk real 43.5m */   
	0x5d, 0x00,   
	0x11, 0x00,

	0x3e, 0x03,   /* Line / Frame hi 844*/   
	0x3d, 0x4C,   /* Line / Frame low */  

	0x2b, 0x06,   /* Tp of Hsync hi 1720 */    
	0x2a, 0xb8,   /* Tp of Hsync low */
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */    	
	#endif

	#if 0
	/*	Clock  */
	0x5c, 0x59,   /* mclk  real 24m, pclk real 42m */   
	0x5d, 0x00,   
	0x11, 0x00,   

	0x3e, 0x03,   /* Line / Frame hi 828*/   
	0x3d, 0x3C,   /* Line / Frame low */  
	
	0x2b, 0x06,   /* Tp of Hsync hi 1688*/
	0x2a, 0x98,   /* Tp of Hsync low */   
	
	0x2d, 0x00,   /*Dummy line LSB */
	0x2e, 0x00,   /*Dummy line MSB */
	#endif

	0x37, 0x06,   /*BLC by frame */
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

static UINT8 front20fpsPrevDz1xCmdTbl[] = {
	0,
};
static UINT8 front20fpsPrevDz2xCmdTbl[] = {
	0,
};
static UINT8 frontPrevMode = 0;
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
  
/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
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
 * @since	2008-09-24
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
			printf("frontOpFrameRateSet = dyn 30->15\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
	
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 20) {
				printf("50Hz\n");
				frontResTblBinRead(_ae20fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae20fps60hzTbl, __aeTbl);
			}
			break;
		case 30:
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 1;
			//frontSnapMode = 0;
	
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
			//frontSnapMode = 1;
	
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
 * @since	2008-09-24
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
 * @since	2008-09-24
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
	globalTimerWait(300);

	frontResCmdBinSend(_frontInit0CmdTbl);
	
	globalTimerWait(300);
	
	frontResCmdBinSend(_frontInit1CmdTbl);
	
	/* after power on. */
	frontAfterPowerOn();

}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
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
 * @since	2008-09-24
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

	if ( frontPrevMode == 0 ) {  /* dyn fps 30->15. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
		//printf("frontOpPreviewModeSet <<dyn 30->15FPS>>\n");
	}
	else if ( frontPrevMode == 1 ) {  /* 30 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
		//printf("frontOpPreviewModeSet <<30FPS>>\n");
	}
	else {// if ( frontPrevMode == 2 ) {  /* 60 fps. */
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
		//printf("frontOpPreviewModeSet <<60FPS>>\n");
	}

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

	cdspBayerPatternSet(BAYER_PTN_PREV);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

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
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
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
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
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
 * @since	2008-09-24
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
 * @author	Matt Wang
 * @since	2008-09-24
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

	if ( first ) {
		#if (DBG_SNAP_TIMING >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		
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

		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1);
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	
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

	}	
	else{  	/* snap do flow */
		frontSnapCapabDesc_t *pcap;
		pcap = &(frontSnapCapabDesc[frontSnapMode]);
		
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		cdspSkipWrSyncSet(1);	
		cdspSkipWrSet(CDSP_SKIP_ALL,0xff);
		
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		if (scaleUp==1){
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDivBurstUp, pcap->mclkPhase, 0, 0);
		}
		else{
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
 * @since	2008-09-24
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
	UINT8 cmdTbl[2*1];
	//UINT8 ADDR8=0x00;

	//AGC- 0X13=85(AUTO); 0X13=81(MANUAL); 1x~16x Analog Gain, 16x~32x Digital gain
	//Gain Rule
	//0x00= 00 ~0F- 1x ~2x-- gain
	//0x00= 10 ~1F- 2x ~4x-- gain
	//0x00= 30 ~3F- 4x ~8x-- gain
	//0x00= 70 ~7F- 8x ~16x- gain
	//0x00= f0 ~fF- 16x ~32x gain

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
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

	cmdTbl[0] = 0x00;
	cmdTbl[1] = (UINT8)(frontLastGain & 0x00FF);
	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	//i2cRegRead(&ADDR8,&cmdTbl[1],1,1,1,1,0); 
	//printf("cmdTbl[1]=%bd\n",cmdTbl[1]);	
}

/**
 * @brief	operating function to set exposure time value.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[2*4];
	UINT16 compensation_line=0;
	//UINT8 REG_ADDR8[2]={0x16, 0x10};
	//UINT8 REG_ADDR[2];

	//Manual Exposure Ctrl First Set REG bit 0x13[0] to Disable
	//Exposure Value = 0x16[15:8], 0x10[7:0], units of 1 line

	//printf("frontPrevMode = %bu\n",frontPrevMode);
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	if (espline != 0) {
		if (frontPrevMode == 0) {
			/* dynamic 30 -> 15 fps */
			if(espline > (ftotal - 6)) {		
				compensation_line = espline - (ftotal - 6);	
				espline = (ftotal - 6);
				//printf("compensation_line=%d, re-define espline=%d\n",compensation_line,espline);
			}
		}
		else {
			/* real 30 fps, real 60 fps */
			if(espline > (ftotal - 6)) {
				espline = (ftotal - 6);
				//printf("re-define espline=%d\n",espline);
			}
		}
	}
	else {
		espline = 1;
	}

	espline_16 = espline;

	/* dummy line set */
	cmdTbl[0] = 0x2e;
	cmdTbl[1] = (UINT8)((compensation_line & 0xFF00)>>8);
	cmdTbl[2] = 0x2d;
	cmdTbl[3] = (UINT8)(compensation_line & 0x00FF);

	//exposure time set
	cmdTbl[4] = 0x16;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	cmdTbl[6] = 0x10;	
	cmdTbl[7] = (UINT8)(espline_16 & 0x000000FF);
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	//i2cRegRead(&REG_ADDR8[0],&REG_ADDR[0],1,2,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1]);

}

/**
 * @brief	operating function to set gain value for intr.
 * @param	parg->val = [in] gain value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
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
	UINT8 cmdTbl[2*1];
	//UINT8 ADDR8=0x00;

	//AGC- 0X13=85(AUTO); 0X13=81(MANUAL); 1x~16x Analog Gain, 16x~32x Digital gain
	//Gain Rule
	//0x00= 00 ~0F- 1x ~2x-- gain
	//0x00= 10 ~1F- 2x ~4x-- gain
	//0x00= 30 ~3F- 4x ~8x-- gain
	//0x00= 70 ~7F- 8x ~16x- gain
	//0x00= f0 ~fF- 16x ~32x gain

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

	cmdTbl[0] = 0x00;
	cmdTbl[1] = (UINT8)(frontLastGain & 0x00FF);
	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	//i2cRegRead(&ADDR8,&cmdTbl[1],1,1,1,1,0); 
	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);	
}

/**
 * @brief	operating function to set exposure time value for intr.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[2*4];
	UINT16 compensation_line=0;
	//UINT8 REG_ADDR8[2]={0x16, 0x10};
	//UINT8 REG_ADDR[2];

	//Manual Exposure Ctrl First Set REG bit 0x13[0] to Disable
	//Exposure Value = 0x16[15:8], 0x10[7:0], units of 1 line

	//printf("frontPrevMode = %bu\n",frontPrevMode);
	
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

	if (espline != 0) {
		if (frontPrevMode == 0) {
			/* dynamic 30 -> 15 fps */
			if(espline > (ftotal - 6)) {		
				compensation_line = espline - (ftotal -6);	
				espline = (ftotal - 6);
				//printf("compensation_line=%d, re-define espline=%d\n",compensation_line,espline);
			}
		}
		else {
			/* real 30 fps, real 60 fps */
			if(espline > (ftotal - 6)) {
				espline = (ftotal - 6);
				//printf("re-define espline=%d\n",espline);
			}
		}
	}
	else {
		espline = 1;
	}

	espline_16 = espline;

	/* dummy line set */
	cmdTbl[0] = 0x2e;
	cmdTbl[1] = (UINT8)((compensation_line & 0xFF00)>>8);
	cmdTbl[2] = 0x2d;
	cmdTbl[3] = (UINT8)(compensation_line & 0x00FF);

	//exposure time set
	cmdTbl[4] = 0x16;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	cmdTbl[6] = 0x10;	
	cmdTbl[7] = (UINT8)(espline_16 & 0x000000FF);
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	//i2cRegRead(&REG_ADDR8[0],&REG_ADDR[0],1,2,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1]);

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
#if (_HW_SOLUTION_ == _HW_RDK_A_) 
	/* sensor reset (fmgpio 10) */
	gpioByteFuncSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteDirSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0xFF);
#elif(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_|| _HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	/***********HD1Z sensor rst****************/
	gpioByteFuncSet(GPIO_BYTE_GEN2, 0x02, 0x02);
	gpioByteDirSet(GPIO_BYTE_GEN2, 0x02, 0x02);
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x02, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x02, 0x02);
#endif	
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
 * @since	2008-09-24
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[2*5];
	UINT16 s_gain;
	#if (SNAP_KEEP_FPS == 0)
	UINT16 compensation_line = 0;
	#endif
	
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

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 1 ) {  /* 30 fps. */
		}
		else if ( frontPrevMode == 0 ) {	/* 24fps */
		}
	}

	s_gain = frontSnapLastGain;

	if (espline != 0) {
		if(espline > (ftotal - 6)) {
			#if (SNAP_KEEP_FPS == 0)
			/* add dummy line to reduce fps */
			compensation_line = espline - (ftotal - 6);
			espline = (ftotal - 6);
			//printf("compensation_line=%d, re-define snap espline=%d\n",compensation_line,espline);
			#else
			/* keep fps */
			espline = (ftotal - 6);
			//printf("re-define snap espline=%d\n",espline);
			#endif	
		}
	}
	else {
		espline = 1;
	}

	espline_16 = espline;

	
	/* dummy line set */
	cmdTbl[0] = 0x2e;
	cmdTbl[1] = (UINT8)((compensation_line & 0xFF00)>>8);
	cmdTbl[2] = 0x2d;
	cmdTbl[3] = (UINT8)(compensation_line & 0x00FF);

	//exposure time set
	cmdTbl[4] = 0x16;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	cmdTbl[6] = 0x10;	
	cmdTbl[7] = (UINT8)(espline_16 & 0x000000FF);
	
	//printf("pre-s_gain=%d\n",s_gain);
	
	//gain set
	cmdTbl[8] = 0x00;
	cmdTbl[9] = (UINT8)(s_gain & 0x00FF);

	//printf("re-s_gain=0x%bx\n",cmdTbl[5]);
	
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

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
				//printf("60->dyn30 +26\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
				pv_gId += 6;
				//printf("30->dyn30 +6\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 2) {
				pv_gId -= 20;
				//printf("30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 1) {
				pv_gId -= 6;
				//printf("dyn30->30 -6\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 2) {
				pv_gId -= 26;
				//printf("dyn30->60 -26\n");
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

	sts = i2cCmdTableSend(G_FPGAMemPool, 1, size/2, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		//case _front20fpsPrevCmdTbl:
		//	pData = front20fpsPrevCmdTbl;
		//	size = sizeof(front20fpsPrevCmdTbl);
		//	break;
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


	sts = i2cCmdTableSend(pData, 1, size/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	
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

