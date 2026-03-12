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
 * @file		front_ov_ov9665_raw.c
 * @brief		front sensor omni vision ov9655 driver with yuv format data.
 * @author		CS Lin
 * @since		2008-11-24
 * @date		2008-11-27
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
#include "sp1k_front_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		FRONT_SNAP_BREAK_DOWN	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */

#define SNAP_KEEP_FPS		0	/* add dummy line : 0(reduce fps), keep fps : 1(exposure line equal to frame total-6) */
#define PV_AUTO_OB_BY_GAIN	0	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
#define RES_LSC_LOAD		0	/* after power on load lsc resource file */

/**************************************************************************
 *                            M I R R O R   &   F L I P	C T R L 
 *			RDK : mirror off, flip on
 *			DDX : mirror on, flip on
 **************************************************************************/
/* mirror, flip ctrl */
#define MIRROR_CTRL		0	/* mirror off : 0, mirror on : 1 */
#define FLIP_CTRL		1	/* flip off : 0, flip on : 1 */

/* mode. */
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_24FPS_PREV      640
#define HEIGHT_24FPS_PREV     480
#define XOFF_24FPS_PREV       70
#define YOFF_24FPS_PREV       (25+46)
#define LINE_TOT_24FPS_PREV   760
#define FRAME_TOT_24FPS_PREV  572//526
#define PREV_TO_SNAP_BINNING_RATE_24FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_24FPS_PREV         0

/* dimensiion. */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       139
#define YOFF_30FPS_PREV      (49+96) 
#define LINE_TOT_30FPS_PREV   1520
#define FRAME_TOT_30FPS_PREV  1148
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0

/* 480p, 60fps */
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       3
#define YOFF_60FPS_PREV       1
#define LINE_TOT_60FPS_PREV   1132
#define FRAME_TOT_60FPS_PREV  738
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_60FPS_PREV         0

#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     1024
#define XOFF_SNAP       139
#define YOFF_SNAP       (17+96)
#define LINE_TOT_SNAP   1520
#define FRAME_TOT_SNAP  1148

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_24FPS_PREV    10
#define MCLK_PHASE_24FPS_PREV  0
#define PCLK_DIV_24FPS_PREV    10
#define PCLK_PHASE_24FPS_PREV  0

#define MCLK_DIV_30FPS_PREV    10
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    10
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP          		10
#define MCLK_DIV_SNAP_BURST     	18
#define MCLK_DIV_SNAP_BURST_UP 		24
#define MCLK_PHASE_SNAP         	0
#define PCLK_DIV_SNAP           	10
#define PCLK_PHASE_SNAP         	0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_24FPS_PREV     13050000L
#define PCLK_FREQ_24FPS_PREV_TD  13050000L
#define PCLK_FREQ_30FPS_PREV     26100000L
#define PCLK_FREQ_30FPS_PREV_TD  26100000L
#define PCLK_FREQ_60FPS_PREV     13050000L
#define PCLK_FREQ_60FPS_PREV_TD  13050000L
#define PCLK_FREQ_SNAP           26100000L
#define PCLK_FREQ_SNAP_BURST  	 26100000L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP  26100000L	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_24FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_24FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP           522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	 522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP  522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
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
#define RESHAPE_HRISE_24FPS_PREV  3
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  3
#define RESHAPE_HFALL_60FPS_PREV  2
#define RESHAPE_HRISE_60FPS_PREV  3

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
#define BAYER_PTN_PREV  2//FRONT_BAYER_PTN_GBBRGR
#define BAYER_PTN_SNAP 2 //FRONT_BAYER_PTN_GBBRGR

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_24FPS_24_MAX_IDX  130
#define AE_24FPS_24_MIN_IDX  10
#define AE_24FPS_20_MAX_IDX  130
#define AE_24FPS_20_MIN_IDX  10
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  130
#define AE_30FPS_30_MIN_IDX  10
#define AE_30FPS_25_MAX_IDX  130
#define AE_30FPS_25_MIN_IDX  10
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  130
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  130
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_24FPS_MAX_IDX  65
#define GAIN_24FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  65
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  65
#define GAIN_60FPS_MIN_IDX  0

/* iso gain. */
#define ISO_100_GAIN_IDX	0
#define ISO_200_GAIN_IDX	16
#define ISO_400_GAIN_IDX	32
#define ISO_800_GAIN_IDX	48

/* manual iso capture limit fps */
#define CAPTURE_LIMIT_FPS	15

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

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV9665 RAW";

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
	//_front60fpsPrevCmdTbl	= 0x00D4,
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
static code frontAe_t ae24fps50hzTbl[131] = {
	{	0,	332,	63,	0,	0 },
	{	1,	332,	61,	0,	0 },
	{	2,	332,	59,	0,	0 },
	{	3,	332,	58,	0,	0 },
	{	4,	332,	56,	0,	0 },
	{	5,	332,	54,	0,	0 },
	{	6,	332,	53,	0,	0 },
	{	7,	332,	51,	0,	0 },
	{	8,	332,	50,	0,	0 },
	{	9,	332,	49,	0,	0 },
	{	10,	332,	47,	0,	0 },
	{	11,	332,	45,	0,	0 },
	{	12,	332,	43,	0,	0 },
	{	13,	332,	42,	0,	0 },
	{	14,	332,	40,	0,	0 },
	{	15,	332,	38,	0,	0 },
	{	16,	332,	37,	0,	0 },
	{	17,	332,	35,	0,	0 },
	{	18,	332,	34,	0,	0 },
	{	19,	332,	33,	0,	0 },
	{	20,	332,	31,	0,	0 },
	{	21,	332,	29,	0,	0 },
	{	22,	332,	27,	0,	0 },
	{	23,	332,	26,	0,	0 },
	{	24,	332,	24,	0,	0 },
	{	25,	332,	22,	0,	0 },
	{	26,	332,	21,	0,	0 },
	{	27,	332,	19,	0,	0 },
	{	28,	332,	18,	0,	0 },
	{	29,	332,	17,	0,	0 },
	{	30,	332,	15,	0,	0 },
	{	31,	332,	13,	0,	0 },
	{	32,	332,	11,	0,	0 },
	{	33,	332,	10,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	6,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	3,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	497,	6,	0,	0 },
	{	42,	497,	5,	0,	0 },
	{	43,	497,	3,	0,	0 },
	{	44,	497,	2,	0,	0 },
	{	45,	497,	1,	0,	0 },
	{	46,	497,	0,	0,	0 },
	{	47,	992,	13,	0,	0 },
	{	48,	992,	11,	0,	0 },
	{	49,	992,	9,	0,	0 },
	{	50,	992,	7,	0,	0 },
	{	51,	992,	6,	0,	0 },
	{	52,	992,	4,	0,	0 },
	{	53,	992,	3,	0,	0 },
	{	54,	992,	2,	0,	0 },
	{	55,	992,	1,	0,	0 },
	{	56,	992,	0,	0,	0 },
	{	57,	1059,	0,	0,	0 },
	{	58,	1137,	0,	0,	0 },
	{	59,	1217,	0,	0,	0 },
	{	60,	1310,	0,	0,	0 },
	{	61,	1396,	0,	0,	0 },
	{	62,	1506,	0,	0,	0 },
	{	63,	1604,	0,	0,	0 },
	{	64,	1734,	0,	0,	0 },
	{	65,	1846,	0,	0,	0 },
	{	66,	1996,	0,	0,	0 },
	{	67,	2119,	0,	0,	0 },
	{	68,	2289,	0,	0,	0 },
	{	69,	2453,	0,	0,	0 },
	{	70,	2641,	0,	0,	0 },
	{	71,	2814,	0,	0,	0 },
	{	72,	3012,	0,	0,	0 },
	{	73,	3239,	0,	0,	0 },
	{	74,	3504,	0,	0,	0 },
	{	75,	3732,	0,	0,	0 },
	{	76,	3993,	0,	0,	0 },
	{	77,	4292,	0,	0,	0 },
	{	78,	4640,	0,	0,	0 },
	{	79,	4906,	0,	0,	0 },
	{	80,	5365,	0,	0,	0 },
	{	81,	5723,	0,	0,	0 },
	{	82,	6132,	0,	0,	0 },
	{	83,	6604,	0,	0,	0 },
	{	84,	7154,	0,	0,	0 },
	{	85,	7465,	0,	0,	0 },
	{	86,	8176,	0,	0,	0 },
	{	87,	8585,	0,	0,	0 },
	{	88,	9539,	0,	0,	0 },
	{	89,	10100,	0,	0,	0 },
	{	90,	10731,	0,	0,	0 },
	{	91,	11447,	0,	0,	0 },
	{	92,	12264,	0,	0,	0 },
	{	93,	13208,	0,	0,	0 },
	{	94,	14309,	0,	0,	0 },
	{	95,	15609,	0,	0,	0 },
	{	96,	17170,	1,	0,	0 },
	{	97,	17170,	0,	0,	0 },
	{	98,	19078,	0,	0,	0 },
	{	99,	21463,	1,	0,	0 },
	{	100,	21463,	0,	0,	0 },
	{	101,	24529,	1,	0,	0 },
	{	102,	24529,	0,	0,	0 },
	{	103,	28617,	1,	0,	0 },
	{	104,	28617,	0,	0,	0 },
	{	105,	34341,	2,	0,	0 },
	{	106,	34341,	1,	0,	0 },
	{	107,	34341,	0,	0,	0 },
	{	108,	42926,	2,	0,	0 },
	{	109,	42926,	1,	0,	0 },
	{	110,	42926,	0,	0,	0 },
	{	111,	57234,	4,	0,	0 },
	{	112,	57234,	3,	0,	0 },
	{	113,	57234,	1,	0,	0 },
	{	114,	57234,	0,	0,	0 },
	{	115,	85850,	6,	0,	0 },
	{	116,	85850,	5,	0,	0 },
	{	117,	85850,	4,	0,	0 },
	{	118,	85850,	2,	0,	0 },
	{	119,	85850,	1,	0,	0 },
	{	120,	85850,	0,	0,	0 },
	{	121,	171693,	13,	0,	0 },
	{	122,	171693,	12,	0,	0 },
	{	123,	171693,	10,	0,	0 },
	{	124,	171693,	8,	0,	0 },
	{	125,	171693,	6,	0,	0 },
	{	126,	171693,	5,	0,	0 },
	{	127,	171693,	4,	0,	0 },
	{	128,	171693,	2,	0,	0 },
	{	129,	171693,	1,	0,	0 },
	{	130,	171693,	0,	0,	0 },
	};

static code frontAe_t ae24fps60hzTbl[133] = {
	{	0,	299,	64,	0,	0 },
	{	1,	299,	62,	0,	0 },
	{	2,	299,	60,	0,	0 },
	{	3,	299,	58,	0,	0 },
	{	4,	299,	57,	0,	0 },
	{	5,	299,	55,	0,	0 },
	{	6,	299,	54,	0,	0 },
	{	7,	299,	52,	0,	0 },
	{	8,	299,	51,	0,	0 },
	{	9,	299,	50,	0,	0 },
	{	10,	299,	48,	0,	0 },
	{	11,	299,	46,	0,	0 },
	{	12,	299,	44,	0,	0 },
	{	13,	299,	42,	0,	0 },
	{	14,	299,	41,	0,	0 },
	{	15,	299,	39,	0,	0 },
	{	16,	299,	38,	0,	0 },
	{	17,	299,	36,	0,	0 },
	{	18,	299,	35,	0,	0 },
	{	19,	299,	34,	0,	0 },
	{	20,	299,	32,	0,	0 },
	{	21,	299,	30,	0,	0 },
	{	22,	299,	28,	0,	0 },
	{	23,	299,	26,	0,	0 },
	{	24,	299,	25,	0,	0 },
	{	25,	299,	23,	0,	0 },
	{	26,	299,	22,	0,	0 },
	{	27,	299,	20,	0,	0 },
	{	28,	299,	19,	0,	0 },
	{	29,	299,	18,	0,	0 },
	{	30,	299,	16,	0,	0 },
	{	31,	299,	14,	0,	0 },
	{	32,	299,	12,	0,	0 },
	{	33,	299,	10,	0,	0 },
	{	34,	299,	9,	0,	0 },
	{	35,	299,	7,	0,	0 },
	{	36,	299,	6,	0,	0 },
	{	37,	299,	4,	0,	0 },
	{	38,	299,	3,	0,	0 },
	{	39,	299,	2,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	399,	4,	0,	0 },
	{	42,	399,	3,	0,	0 },
	{	43,	399,	2,	0,	0 },
	{	44,	399,	1,	0,	0 },
	{	45,	401,	0,	0,	0 },
	{	46,	598,	5,	0,	0 },
	{	47,	598,	4,	0,	0 },
	{	48,	598,	3,	0,	0 },
	{	49,	598,	2,	0,	0 },
	{	50,	598,	0,	0,	0 },
	{	51,	1192,	14,	0,	0 },
	{	52,	1192,	12,	0,	0 },
	{	53,	1192,	10,	0,	0 },
	{	54,	1192,	9,	0,	0 },
	{	55,	1192,	7,	0,	0 },
	{	56,	1192,	5,	0,	0 },
	{	57,	1192,	4,	0,	0 },
	{	58,	1192,	3,	0,	0 },
	{	59,	1192,	1,	0,	0 },
	{	60,	1192,	0,	0,	0 },
	{	61,	1217,	0,	0,	0 },
	{	62,	1310,	0,	0,	0 },
	{	63,	1396,	0,	0,	0 },
	{	64,	1506,	0,	0,	0 },
	{	65,	1604,	0,	0,	0 },
	{	66,	1734,	0,	0,	0 },
	{	67,	1846,	0,	0,	0 },
	{	68,	1996,	0,	0,	0 },
	{	69,	2119,	0,	0,	0 },
	{	70,	2289,	0,	0,	0 },
	{	71,	2453,	0,	0,	0 },
	{	72,	2641,	0,	0,	0 },
	{	73,	2814,	0,	0,	0 },
	{	74,	3012,	0,	0,	0 },
	{	75,	3239,	0,	0,	0 },
	{	76,	3504,	0,	0,	0 },
	{	77,	3732,	0,	0,	0 },
	{	78,	3993,	0,	0,	0 },
	{	79,	4292,	0,	0,	0 },
	{	80,	4640,	0,	0,	0 },
	{	81,	4906,	0,	0,	0 },
	{	82,	5365,	0,	0,	0 },
	{	83,	5723,	0,	0,	0 },
	{	84,	6132,	0,	0,	0 },
	{	85,	6604,	0,	0,	0 },
	{	86,	7154,	0,	0,	0 },
	{	87,	7465,	0,	0,	0 },
	{	88,	8176,	0,	0,	0 },
	{	89,	8585,	0,	0,	0 },
	{	90,	9539,	0,	0,	0 },
	{	91,	10100,	0,	0,	0 },
	{	92,	10731,	0,	0,	0 },
	{	93,	11447,	0,	0,	0 },
	{	94,	12264,	0,	0,	0 },
	{	95,	13208,	0,	0,	0 },
	{	96,	14309,	0,	0,	0 },
	{	97,	15609,	0,	0,	0 },
	{	98,	17170,	1,	0,	0 },
	{	99,	17170,	0,	0,	0 },
	{	100,	19078,	0,	0,	0 },
	{	101,	21463,	1,	0,	0 },
	{	102,	21463,	0,	0,	0 },
	{	103,	24529,	1,	0,	0 },
	{	104,	24529,	0,	0,	0 },
	{	105,	28617,	1,	0,	0 },
	{	106,	28617,	0,	0,	0 },
	{	107,	34341,	2,	0,	0 },
	{	108,	34341,	1,	0,	0 },
	{	109,	34341,	0,	0,	0 },
	{	110,	42926,	2,	0,	0 },
	{	111,	42926,	1,	0,	0 },
	{	112,	42926,	0,	0,	0 },
	{	113,	57234,	4,	0,	0 },
	{	114,	57234,	3,	0,	0 },
	{	115,	57234,	1,	0,	0 },
	{	116,	57234,	0,	0,	0 },
	{	117,	85850,	6,	0,	0 },
	{	118,	85850,	5,	0,	0 },
	{	119,	85850,	4,	0,	0 },
	{	120,	85850,	2,	0,	0 },
	{	121,	85850,	1,	0,	0 },
	{	122,	85850,	0,	0,	0 },
	{	123,	171693,	13,	0,	0 },
	{	124,	171693,	12,	0,	0 },
	{	125,	171693,	10,	0,	0 },
	{	126,	171693,	8,	0,	0 },
	{	127,	171693,	6,	0,	0 },
	{	128,	171693,	5,	0,	0 },
	{	129,	171693,	4,	0,	0 },
	{	130,	171693,	2,	0,	0 },
	{	131,	171693,	1,	0,	0 },
	{	132,	171693,	0,	0,	0 },
	};

static code frontAe_t ae30fps50hzTbl[141] = {
	{	0,	166,	63,	0,	0 },
	{	1,	166,	61,	0,	0 },
	{	2,	166,	59,	0,	0 },
	{	3,	166,	58,	0,	0 },
	{	4,	166,	56,	0,	0 },
	{	5,	166,	54,	0,	0 },
	{	6,	166,	53,	0,	0 },
	{	7,	166,	51,	0,	0 },
	{	8,	166,	50,	0,	0 },
	{	9,	166,	49,	0,	0 },
	{	10,	166,	47,	0,	0 },
	{	11,	166,	45,	0,	0 },
	{	12,	166,	43,	0,	0 },
	{	13,	166,	42,	0,	0 },
	{	14,	166,	40,	0,	0 },
	{	15,	166,	38,	0,	0 },
	{	16,	166,	37,	0,	0 },
	{	17,	166,	35,	0,	0 },
	{	18,	166,	34,	0,	0 },
	{	19,	166,	33,	0,	0 },
	{	20,	166,	31,	0,	0 },
	{	21,	166,	29,	0,	0 },
	{	22,	166,	27,	0,	0 },
	{	23,	166,	26,	0,	0 },
	{	24,	166,	24,	0,	0 },
	{	25,	166,	22,	0,	0 },
	{	26,	166,	21,	0,	0 },
	{	27,	166,	19,	0,	0 },
	{	28,	166,	18,	0,	0 },
	{	29,	166,	17,	0,	0 },
	{	30,	166,	15,	0,	0 },
	{	31,	166,	13,	0,	0 },
	{	32,	166,	11,	0,	0 },
	{	33,	166,	10,	0,	0 },
	{	34,	166,	8,	0,	0 },
	{	35,	166,	6,	0,	0 },
	{	36,	166,	5,	0,	0 },
	{	37,	166,	3,	0,	0 },
	{	38,	166,	2,	0,	0 },
	{	39,	166,	1,	0,	0 },
	{	40,	166,	0,	0,	0 },
	{	41,	199,	2,	0,	0 },
	{	42,	199,	1,	0,	0 },
	{	43,	249,	3,	0,	0 },
	{	44,	249,	2,	0,	0 },
	{	45,	249,	1,	0,	0 },
	{	46,	249,	0,	0,	0 },
	{	47,	332,	3,	0,	0 },
	{	48,	332,	2,	0,	0 },
	{	49,	332,	1,	0,	0 },
	{	50,	332,	0,	0,	0 },
	{	51,	497,	6,	0,	0 },
	{	52,	497,	5,	0,	0 },
	{	53,	497,	3,	0,	0 },
	{	54,	497,	2,	0,	0 },
	{	55,	497,	1,	0,	0 },
	{	56,	497,	0,	0,	0 },
	{	57,	992,	13,	0,	0 },
	{	58,	992,	11,	0,	0 },
	{	59,	992,	9,	0,	0 },
	{	60,	992,	7,	0,	0 },
	{	61,	992,	6,	0,	0 },
	{	62,	992,	4,	0,	0 },
	{	63,	992,	3,	0,	0 },
	{	64,	992,	2,	0,	0 },
	{	65,	992,	1,	0,	0 },
	{	66,	992,	0,	0,	0 },
	{	67,	1059,	0,	0,	0 },
	{	68,	1137,	0,	0,	0 },
	{	69,	1217,	0,	0,	0 },
	{	70,	1310,	0,	0,	0 },
	{	71,	1396,	0,	0,	0 },
	{	72,	1506,	0,	0,	0 },
	{	73,	1604,	0,	0,	0 },
	{	74,	1734,	0,	0,	0 },
	{	75,	1846,	0,	0,	0 },
	{	76,	1996,	0,	0,	0 },
	{	77,	2119,	0,	0,	0 },
	{	78,	2289,	0,	0,	0 },
	{	79,	2453,	0,	0,	0 },
	{	80,	2641,	0,	0,	0 },
	{	81,	2814,	0,	0,	0 },
	{	82,	3012,	0,	0,	0 },
	{	83,	3239,	0,	0,	0 },
	{	84,	3504,	0,	0,	0 },
	{	85,	3732,	0,	0,	0 },
	{	86,	3993,	0,	0,	0 },
	{	87,	4292,	0,	0,	0 },
	{	88,	4640,	0,	0,	0 },
	{	89,	4906,	0,	0,	0 },
	{	90,	5365,	0,	0,	0 },
	{	91,	5723,	0,	0,	0 },
	{	92,	6132,	0,	0,	0 },
	{	93,	6604,	0,	0,	0 },
	{	94,	7154,	0,	0,	0 },
	{	95,	7465,	0,	0,	0 },
	{	96,	8176,	0,	0,	0 },
	{	97,	8585,	0,	0,	0 },
	{	98,	9539,	0,	0,	0 },
	{	99,	10100,	0,	0,	0 },
	{	100,	10731,	0,	0,	0 },
	{	101,	11447,	0,	0,	0 },
	{	102,	12264,	0,	0,	0 },
	{	103,	13208,	0,	0,	0 },
	{	104,	14309,	0,	0,	0 },
	{	105,	15609,	0,	0,	0 },
	{	106,	17170,	1,	0,	0 },
	{	107,	17170,	0,	0,	0 },
	{	108,	19078,	0,	0,	0 },
	{	109,	21463,	1,	0,	0 },
	{	110,	21463,	0,	0,	0 },
	{	111,	24529,	1,	0,	0 },
	{	112,	24529,	0,	0,	0 },
	{	113,	28617,	1,	0,	0 },
	{	114,	28617,	0,	0,	0 },
	{	115,	34341,	2,	0,	0 },
	{	116,	34341,	1,	0,	0 },
	{	117,	34341,	0,	0,	0 },
	{	118,	42926,	2,	0,	0 },
	{	119,	42926,	1,	0,	0 },
	{	120,	42926,	0,	0,	0 },
	{	121,	57234,	4,	0,	0 },
	{	122,	57234,	3,	0,	0 },
	{	123,	57234,	1,	0,	0 },
	{	124,	57234,	0,	0,	0 },
	{	125,	85850,	6,	0,	0 },
	{	126,	85850,	5,	0,	0 },
	{	127,	85850,	4,	0,	0 },
	{	128,	85850,	2,	0,	0 },
	{	129,	85850,	1,	0,	0 },
	{	130,	85850,	0,	0,	0 },
	{	131,	171693,	13,	0,	0 },
	{	132,	171693,	12,	0,	0 },
	{	133,	171693,	10,	0,	0 },
	{	134,	171693,	8,	0,	0 },
	{	135,	171693,	6,	0,	0 },
	{	136,	171693,	5,	0,	0 },
	{	137,	171693,	4,	0,	0 },
	{	138,	171693,	2,	0,	0 },
	{	139,	171693,	1,	0,	0 },
	{	140,	171693,	0,	0,	0 },
	};
static code frontAe_t ae30fps60hzTbl[143] = {
	{	0,	149,	64,	0,	0 },
	{	1,	149,	62,	0,	0 },
	{	2,	149,	60,	0,	0 },
	{	3,	149,	58,	0,	0 },
	{	4,	149,	57,	0,	0 },
	{	5,	149,	55,	0,	0 },
	{	6,	149,	54,	0,	0 },
	{	7,	149,	52,	0,	0 },
	{	8,	149,	51,	0,	0 },
	{	9,	149,	50,	0,	0 },
	{	10,	149,	48,	0,	0 },
	{	11,	149,	46,	0,	0 },
	{	12,	149,	44,	0,	0 },
	{	13,	149,	42,	0,	0 },
	{	14,	149,	41,	0,	0 },
	{	15,	149,	39,	0,	0 },
	{	16,	149,	38,	0,	0 },
	{	17,	149,	36,	0,	0 },
	{	18,	149,	35,	0,	0 },
	{	19,	149,	34,	0,	0 },
	{	20,	149,	32,	0,	0 },
	{	21,	149,	30,	0,	0 },
	{	22,	149,	28,	0,	0 },
	{	23,	149,	26,	0,	0 },
	{	24,	149,	25,	0,	0 },
	{	25,	149,	23,	0,	0 },
	{	26,	149,	22,	0,	0 },
	{	27,	149,	20,	0,	0 },
	{	28,	149,	19,	0,	0 },
	{	29,	149,	18,	0,	0 },
	{	30,	149,	16,	0,	0 },
	{	31,	149,	14,	0,	0 },
	{	32,	149,	12,	0,	0 },
	{	33,	149,	10,	0,	0 },
	{	34,	149,	9,	0,	0 },
	{	35,	149,	7,	0,	0 },
	{	36,	149,	6,	0,	0 },
	{	37,	149,	4,	0,	0 },
	{	38,	149,	3,	0,	0 },
	{	39,	149,	2,	0,	0 },
	{	40,	149,	0,	0,	0 },
	{	41,	171,	1,	0,	0 },
	{	42,	171,	0,	0,	0 },
	{	43,	199,	2,	0,	0 },
	{	44,	199,	1,	0,	0 },
	{	45,	239,	3,	0,	0 },
	{	46,	239,	1,	0,	0 },
	{	47,	239,	0,	0,	0 },
	{	48,	299,	3,	0,	0 },
	{	49,	299,	2,	0,	0 },
	{	50,	299,	0,	0,	0 },
	{	51,	399,	4,	0,	0 },
	{	52,	399,	3,	0,	0 },
	{	53,	399,	2,	0,	0 },
	{	54,	399,	1,	0,	0 },
	{	55,	401,	0,	0,	0 },
	{	56,	598,	5,	0,	0 },
	{	57,	598,	4,	0,	0 },
	{	58,	598,	3,	0,	0 },
	{	59,	598,	2,	0,	0 },
	{	60,	598,	0,	0,	0 },
	{	61,	1192,	14,	0,	0 },
	{	62,	1192,	12,	0,	0 },
	{	63,	1192,	10,	0,	0 },
	{	64,	1192,	9,	0,	0 },
	{	65,	1192,	7,	0,	0 },
	{	66,	1192,	5,	0,	0 },
	{	67,	1192,	4,	0,	0 },
	{	68,	1192,	3,	0,	0 },
	{	69,	1192,	1,	0,	0 },
	{	70,	1192,	0,	0,	0 },
	{	71,	1217,	0,	0,	0 },
	{	72,	1310,	0,	0,	0 },
	{	73,	1396,	0,	0,	0 },
	{	74,	1506,	0,	0,	0 },
	{	75,	1604,	0,	0,	0 },
	{	76,	1734,	0,	0,	0 },
	{	77,	1846,	0,	0,	0 },
	{	78,	1996,	0,	0,	0 },
	{	79,	2119,	0,	0,	0 },
	{	80,	2289,	0,	0,	0 },
	{	81,	2453,	0,	0,	0 },
	{	82,	2641,	0,	0,	0 },
	{	83,	2814,	0,	0,	0 },
	{	84,	3012,	0,	0,	0 },
	{	85,	3239,	0,	0,	0 },
	{	86,	3504,	0,	0,	0 },
	{	87,	3732,	0,	0,	0 },
	{	88,	3993,	0,	0,	0 },
	{	89,	4292,	0,	0,	0 },
	{	90,	4640,	0,	0,	0 },
	{	91,	4906,	0,	0,	0 },
	{	92,	5365,	0,	0,	0 },
	{	93,	5723,	0,	0,	0 },
	{	94,	6132,	0,	0,	0 },
	{	95,	6604,	0,	0,	0 },
	{	96,	7154,	0,	0,	0 },
	{	97,	7465,	0,	0,	0 },
	{	98,	8176,	0,	0,	0 },
	{	99,	8585,	0,	0,	0 },
	{	100,	9539,	0,	0,	0 },
	{	101,	10100,	0,	0,	0 },
	{	102,	10731,	0,	0,	0 },
	{	103,	11447,	0,	0,	0 },
	{	104,	12264,	0,	0,	0 },
	{	105,	13208,	0,	0,	0 },
	{	106,	14309,	0,	0,	0 },
	{	107,	15609,	0,	0,	0 },
	{	108,	17170,	1,	0,	0 },
	{	109,	17170,	0,	0,	0 },
	{	110,	19078,	0,	0,	0 },
	{	111,	21463,	1,	0,	0 },
	{	112,	21463,	0,	0,	0 },
	{	113,	24529,	1,	0,	0 },
	{	114,	24529,	0,	0,	0 },
	{	115,	28617,	1,	0,	0 },
	{	116,	28617,	0,	0,	0 },
	{	117,	34341,	2,	0,	0 },
	{	118,	34341,	1,	0,	0 },
	{	119,	34341,	0,	0,	0 },
	{	120,	42926,	2,	0,	0 },
	{	121,	42926,	1,	0,	0 },
	{	122,	42926,	0,	0,	0 },
	{	123,	57234,	4,	0,	0 },
	{	124,	57234,	3,	0,	0 },
	{	125,	57234,	1,	0,	0 },
	{	126,	57234,	0,	0,	0 },
	{	127,	85850,	6,	0,	0 },
	{	128,	85850,	5,	0,	0 },
	{	129,	85850,	4,	0,	0 },
	{	130,	85850,	2,	0,	0 },
	{	131,	85850,	1,	0,	0 },
	{	132,	85850,	0,	0,	0 },
	{	133,	171693,	13,	0,	0 },
	{	134,	171693,	12,	0,	0 },
	{	135,	171693,	10,	0,	0 },
	{	136,	171693,	8,	0,	0 },
	{	137,	171693,	6,	0,	0 },
	{	138,	171693,	5,	0,	0 },
	{	139,	171693,	4,	0,	0 },
	{	140,	171693,	2,	0,	0 },
	{	141,	171693,	1,	0,	0 },
	{	142,	171693,	0,	0,	0 },
	};

static code frontAe_t ae60fps50hzTbl[1] = {
	{	0,	600,	0,	0,	0 },
};
static code frontAe_t ae60fps60hzTbl[1] = {
	{	0,	600,	0,	0,	0 },
};

/* gain table. */
static code UINT16 gainTbl[65] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9, 
   10,    11,    12,    13,    14,    15,    16,    17,    18,    19, 
   20,    21,    22,    23,    24,    25,    26,    27,    28,    29, 
   30,    31,    48,    49,    50,    51,    52,    53,    54,    55, 
   56,    57,    58,    59,    60,    61,    62,    63,   112,   113, 
  114,   115,   116,   117,   118,   119,   120,   121,   122,   123, 
  124,   125,   126,   127,   240, 	};

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
		MCLK_DIV_60FPS_PREV + 5,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV + 5,
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
	{  /* 60 fps. */
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

static code UINT8 frontInit0CmdTbl[] = {
	0x12, 0x80,
};
static code UINT8 frontInit1CmdTbl[] = {
//IO output
	0xd5, 0xff,
	0xd6, 0x3f,
//Clock 24MHz 
	0x3d, 0x3c,//0x38,
	0x11, 0x80,
	0x2a, 0x00,
	0x2b, 0x00,
//Power control	
	0x3a, 0xf1,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//Sync signal	
	0x15, 0x00,

//Data Format Raw
	0xd7, 0x07, 
	0xd8, 0xE4,
	0x6a, 0x23,
	0x85, 0xe0,//0xe7,disable AWB
//Sample Option
	0x63, 0x20,//0x00,dsp function do not work on raw output data
//Windowing	
	0x12, 0x40, 
	0x14, 0x50, 
	0x0c, 0xb8, 
	0x4d, 0x09, 
	0x5c, 0x80, 
	0x17, 0x0c, 
	0x18, 0x5c, 
	0x19, 0x02, 
	0x1a, 0x3e,//0x7a,
	0x03, 0x0f, 
	0x32, 0xad, 
	0x5a, 0x23, 
	0x2b, 0x00, 
	0x4c, 0x3c, 
//BLC
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9f,
	0x64, 0xa4,
#if 0
//AEC; 	Average;9 zone;	
	0x43, 0x00, 
	0x5D, 0x55,
	0x5E, 0x57,
	0x5F, 0x21,
//Brightness	
	0x24, 0x3e, 
	0x25, 0x38, 
	0x26, 0x72, 
//BF 60Hz  
	0x14, 0x68,//0x6c,
	0x0C, 0x38,
	0x4F, 0x4f,
	0x50, 0x42,
	0x5A, 0x67,
//LC enable Largon9306; 
	0x7d, 0x30, 			 
	0x7e, 0x00, 			
	0x82, 0x03, 			 
	0x7f, 0x00, 			 
	0x83, 0x07, 			 
	0x80, 0x03, 			 
	0x81, 0x04, 	
#endif
//ouput mode
	//0xd8, 0xE4,
	//0xd7, 0x07,
	//0x12, 0x40,
//disable AE
	0x13, 0xc0,
	0x00, 0xff,	/*gain*/
	0x01, 0x40,	/*AWB R gain*/
	0x02, 0x40,	/*AWB B gain*/
	0x10, 0x3d,
	0x45, 0x00,
	0x04, 0x28,
//disable LSC
	0x83, 0x00,
//BLC  0x85
	0x0d, 0x92,
	0x0d, 0x90,

	//0x46, 0x2e,  /*dummy Lines +46*/ 
};

static code UINT8 front24fpsPrevCmdTbl[] = {
//IO output
//	0xd5, 0xff,
//	0xd6, 0x3f,
//Clock 24MHz 
//	0x3d, 0x3c,//0x38,
//	0x11, 0x80,
//	0x2a, 0x00,
//	0x2b, 0x00,
//Power control	
	0x3a, 0xf1,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//Sync signal	
	0x15, 0x00,

	0x1b, 0x00,

//Data Format Raw
	0xd7, 0x07, 
	0xd8, 0xE4,
	0x6a, 0x23,
	0x85, 0xe0,//0xe7,disable AWB
//Sample Option
	0x63, 0x20,//0x00,dsp function do not work on raw output data
//Windowing	
	0x12, 0x40, 
	0x14, 0x50, 
	0x0c, 0xb8, 
	0x4d, 0x09, 
	0x5c, 0x80, 
	0x17, 0x0c, 
	0x18, 0x5c, 
	0x19, 0x02, 
	0x1a, 0x3e,//0x7a,
	0x03, 0x0f, 
	0x32, 0xad, 
	0x5a, 0x23, 
	0x2b, 0x00, 
	0x4c, 0x3c, 
//BLC
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9f,
	0x64, 0xa4,
#if 0
//AEC; 	Average;9 zone;	
	0x43, 0x00, 
	0x5D, 0x55,
	0x5E, 0x57,
	0x5F, 0x21,
//Brightness	
	0x24, 0x3e, 
	0x25, 0x38, 
	0x26, 0x72, 
//BF 60Hz  
	0x14, 0x68,//0x6c,
	0x0C, 0x38,
	0x4F, 0x4f,
	0x50, 0x42,
	0x5A, 0x67,
//LC enable Largon9306; 
	0x7d, 0x30, 			 
	0x7e, 0x00, 			
	0x82, 0x03, 			 
	0x7f, 0x00, 			 
	0x83, 0x07, 			 
	0x80, 0x03, 			 
	0x81, 0x04, 	
#endif

//LC enable Largon9306; 
#if 0
	0x7d, 0x30,
	0x7e, 0x00,
	0x82, 0x03,
	0x7f, 0x00,
	0x83, 0x07,
	0x80, 0x03,
	0x81, 0x04,

#endif

//ouput mode
	//0xd8, 0xE4,
	//0xd7, 0x07,
	//0x12, 0x40,
//disable AE
//	0x13, 0xc0,
//	0x00, 0xff,	/*gain*/
//	0x01, 0x40,	/*AWB R gain*/
//	0x02, 0x40,	/*AWB B gain*/
//	0x10, 0x3d,	/*exposure lines*/
//	0x45, 0x00,
//	0x04, 0x28,
//disable LSC
//	0x83, 0x00,
//BLC  0x85
	0x0d, 0x92,
	0x0d, 0x90,
	
	0x46, 0x2e,  /*dummy Lines +46*/ 
	0x2d, 0x00,
	0x2e, 0x00,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
//;IO output
//	0xd5, 0xff,
//	0xd6, 0x3f,
//;Clock 12MHz 6.25FPS
//	0x3d, 0x3c,
//	0x11, 0x80,
//	0x2a, 0x00,
//	0x2b, 0x00,
//Power control
	0x3a, 0xd9,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//;Sync sinal  
	0x15, 0x00,

	0x1b, 0x01,

//Data Format RAW
	0xd7, 0x07,
	0xd8, 0xE4,
	0x6a, 0x23,
	0x85, 0xe7,
//;Sample Option	   	
	0x63, 0x21,//0x01,dsp function do not work on raw output data
//;Windowing	
	0x12, 0x00,
	0x17, 0x0c,
	0x18, 0x5c,
	0x19, 0x01,
	0x1a, 0x82,
	0x03, 0x0f,
	0x2b, 0x00,
	0x32, 0x34,
//blc
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9c,
	0x64, 0x24,
#if 0
//;AEC;	Average;9 zone;	
	0x43, 0x00,
	0x5D, 0x55,
	0x5E, 0x57,  
	0x5F, 0x21,
//; Brightness	
	0x24, 0x3e,
	0x25, 0x38,
	0x26, 0x21,
//;Banding
	0x14, 0x68,
	0x0C, 0x38, 
	0x4F, 0x9e,
	0x50, 0x84,
	0x5A, 0x67,
//;LC enable;		
	0x7d, 0x30,
	0x7e, 0x00,
	0x82, 0x03,
	0x7f, 0x00,
	0x83, 0x07,
	0x80, 0x03,
	0x81, 0x04, 
#endif
//ouput mode
//	0xd8, 0xE4,
//	0xd7, 0x07,
//	0x12, 0x00,
//disable AE
//	0x13, 0xc0,
//	0x00, 0xff,	/*gain*/
//	0x01, 0x40,	/*AWB R gain*/
//	0x02, 0x40,	/*AWB B gain*/
//	0x10, 0x3d,
//	0x45, 0x00,
//	0x04, 0x28,
//;BLC
	0x0d, 0x82, 
	0x0d, 0x80, 
	0x46, 0x5c,  /*dummy Lines +92*/ 
	0x2d, 0x00,
	0x2e, 0x00,
};

static code UINT8 frontSnapCmdTbl[] = {
//;IO output
//	0xd5, 0xff,
//	0xd6, 0x3f,
//;Clock 12MHz 6.25FPS
//	0x3d, 0x3c,
//	0x11, 0x80,
//	0x2a, 0x00,
//	0x2b, 0x00,
//Power control
	0x3a, 0xd9,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//;Sync sinal  
	0x15, 0x00,

	0x1b, 0x01,

//Data Format RAW
	0xd7, 0x07,
	0xd8, 0xE4,
	0x6a, 0x23,
	0x85, 0xe7,
//;Sample Option	   	
	0x63, 0x21,//0x01,dsp function do not work on raw output data
//;Windowing	
	0x12, 0x00,
	0x17, 0x0c,
	0x18, 0x5c,
	0x19, 0x01,
	0x1a, 0x82,
	0x03, 0x0f,
	0x2b, 0x00,
	0x32, 0x34,
//blc
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9c,
	0x64, 0x24,
#if 0
//;AEC;	Average;9 zone;	
	0x43, 0x00,
	0x5D, 0x55,
	0x5E, 0x57,  
	0x5F, 0x21,
//; Brightness	
	0x24, 0x3e,
	0x25, 0x38,
	0x26, 0x21,
//;Banding
	0x14, 0x68,
	0x0C, 0x38, 
	0x4F, 0x9e,
	0x50, 0x84,
	0x5A, 0x67,
//;LC enable;		
	0x7d, 0x30,
	0x7e, 0x00,
	0x82, 0x03,
	0x7f, 0x00,
	0x83, 0x07,
	0x80, 0x03,
	0x81, 0x04, 
#endif
//ouput mode
//	0xd8, 0xE4,
//	0xd7, 0x07,
//	0x12, 0x00,
//disable AE
//	0x13, 0xc0,
//	0x00, 0xff,	/*gain*/
//	0x01, 0x40,	/*AWB R gain*/
//	0x02, 0x40,	/*AWB B gain*/
//	0x10, 0x3d,
//	0x45, 0x00,
//	0x04, 0x28,
//;BLC
	0x0d, 0x82, 
	0x0d, 0x80, 
	0x46, 0x5c,  /*dummy Lines +92*/ 
	0x2d, 0x00,
	0x2e, 0x00,
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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
			frontPrevMode = 0;
			
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 50) {
				printf("50Hz\n");
				//frontResTblBinRead(_ae60fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				//frontResTblBinRead(_ae60fps60hzTbl, __aeTbl);
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	//XBYTE[0x2019] = 0x04;

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* sensor reset complete */
	globalTimerWait(1);

	/* send i2c command. */
	frontResCmdBinSend(_frontInit0CmdTbl);
	globalTimerWait(10);
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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

	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		frontResCmdBinSend(_front24fpsPrevCmdTbl);
	}
	else if ( frontPrevMode == 1 ) {  /* 15 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
	}
	else{ 
		frontResCmdBinSend(_front24fpsPrevCmdTbl);
	}

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

	cdspBayerPatternSet(BAYER_PTN_PREV);

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
	#if 1
	factor = factor;
	#else
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
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
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
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
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
frontOpIntrPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	#if 1
	factor = factor;
	#else
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
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_20FPS_PREV) {//2x
				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front20fpsPrevDz2xCmdTbl, 2, sizeof(front20fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

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

				aeIntrCurGainValueGet(&pvGainIdxVal);

				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				
				i2cCmdTableSend_Intr(front30fpsPrevDz1xCmdTbl, 2, sizeof(front30fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
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
			#endif
		}
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_20FPS_PREV) {//1x

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
			#endif
		}
	}
	#endif
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
	
	printf("...........warit stop\n" );
}

/**
 * @brief	operating function to snap shot.
 * @param	first = [in] 0: not first, 1: first shot for burst shot.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
	if ( frontPrevSensorZoom2xGainEn ) {
		frontPrevSensorZoomSnapStatus = 1;
	}
	
	if ( first != 0 ) {
		#if (DBG_SNAP_TIMING >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		
		#if (VSYNC_INV_PREV >= 1)
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
		
		#if (VSYNC_INV_SNAP >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1);

		#if (VSYNC_INV_SNAP >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		#if (VSYNC_INV_SNAP >= 1)
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
 * @since	2008-11-17
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

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];

	cmdTbl[0] = 0x00;
	cmdTbl[1] = frontLastGain;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);

}

/**
 * @brief	operating function to set exposure time value.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
	UINT8 cmdTbl[2*3];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 2) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = 0x28 | (espline & 0x03);
	cmdTbl[4] = 0x45;
	cmdTbl[5] = (espline>>10)&0x3f;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
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

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];

	cmdTbl[0] = 0x00;
	cmdTbl[1] = frontLastGain;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);

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
	UINT32 pixclk;
	UINT8 cmdTbl[2*3];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;
	
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//espline = ltotal;

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 2) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = 0x28 | (espline & 0x03);
	cmdTbl[4] = 0x45;
	cmdTbl[5] = (espline>>10)&0x3f;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
}


/**
 * @brief	operating function to get optical black status.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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
	#if 1
	en = en;
	#else
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
	#endif
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
	UINT32 tmp0, pixclk;
	UINT8 cmdTbl[2*6];
	UINT16 s_gain, lastt_gain;

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
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 20) / (ltotal * frontSnapLastShutter);
	s_gain = frontSnapLastGain;

	lastt_gain = s_gain;

	cdspColorMatrixRestore();

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
		espline = ftotal - 1;
	}
	else {
		tmp0 = 0;
	}

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 2) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = 0x28 | (espline & 0x03);
	cmdTbl[4] = 0x45;
	cmdTbl[5] = (espline >> 10) & 0x3F;
	cmdTbl[6] = 0x2D;
	cmdTbl[7] = tmp0;
	cmdTbl[8] = 0x2E;
	cmdTbl[9] = tmp0 >> 8;
	cmdTbl[10] = 0x00;
	cmdTbl[11] = lastt_gain;
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
	#if 1
	frontPrevCapabDesc_t *pcap;
	UINT8 pvFreq, gIdval;
	SINT16 pv_gId = 0;
	UINT8 minIdx, maxIdx;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* power on status */
	if (frontPowerOnStatus == 1) {
	
		/* sensor different resolution or fps switch check */
		if (frontPrevModeSave != frontPrevMode) {
			sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
			sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
			pv_gId = gIdval;
			//printf("\033[32mpvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);
			
			if (frontPrevModeSave == 0 && frontPrevMode == 1) {
				pv_gId -= 0;
				//printf("60->30 +20\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
				pv_gId += 0;
				//printf("60->dyn30 +26\n");
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
			//printf("pv_gId = %d \033[0m\n",pv_gId);

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
				
				printf("\033[32mBack PV 1\033[0m\n");
			}
			else {
				sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
				sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
				if (pvFreq == SP1K_FLICKER_60) {
					frontExposureTimeSet(0,pcap->ae60hzTbl[gIdval].shutter,1,0);
					frontGainSet(0,pcap->ae60hzTbl[gIdval].gain,1,0);
				}
				else {//(pvFreq == SP1K_FLICKER_50)
					frontExposureTimeSet(0,pcap->ae50hzTbl[gIdval].shutter,1,0);
					frontGainSet(0,pcap->ae50hzTbl[gIdval].gain,1,0);
				}				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				printf("\033[32mBack PV 2\033[0m\n");
				//printf("frontLastShutter=%lu, frontLastGainIdx=%bu\n",frontLastShutter,frontLastGainIdx);
			}
		}
	}
	else {
		frontPrevModeSave = frontPrevMode;
		frontPowerOnStatus = 1;
	}
	#endif
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

