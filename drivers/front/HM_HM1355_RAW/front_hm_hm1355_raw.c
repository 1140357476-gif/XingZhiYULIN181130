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
 * @file		front_hm_hm1355_raw.c
 * @brief		front sensor Himax vision hm1355 driver.
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
#include "hal_dram.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		0	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */


#define SNAP_KEEP_FPS		0	/* add dummy line : 0(reduce fps), keep fps : 1(exposure line equal to frame total-6) */
#define PV_AUTO_OB_BY_GAIN	1	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
#define SNAP_GAIN_TO_EXP	1//dark noise reduce, sensor gain transfor to exposure time : 1
#define RES_LSC_LOAD		0//after power on load lsc resource file

/* mode. */
#define PREV_MODE_TOT	3
#define SNAP_MODE_TOT	1
#define PREV_RESP		720


/* dimensiion. */
#if (PREV_RESP==720) //720p
/* 720p, dyn 30->15 fps */
#define WIDTH_24FPS_PREV      640
#define HEIGHT_24FPS_PREV     480
#define XOFF_24FPS_PREV       240
#define YOFF_24FPS_PREV       1
#define LINE_TOT_24FPS_PREV   879 //1088
#define FRAME_TOT_24FPS_PREV  567
#define PREV_TO_SNAP_BINNING_RATE_24FPS_PREV  1

/* 720p, real 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       300
#define YOFF_30FPS_PREV       1
#define LINE_TOT_30FPS_PREV   1574
#define FRAME_TOT_30FPS_PREV 767
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1

/* 480p, real 60fps */
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       240
#define YOFF_60FPS_PREV       1
#define LINE_TOT_60FPS_PREV   879//1088
#define FRAME_TOT_60FPS_PREV  767
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  1
#endif

/* 1.2M */
#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     960
#define XOFF_SNAP      300
#define YOFF_SNAP      31
#define LINE_TOT_SNAP   1574
#define FRAME_TOT_SNAP  987

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_24FPS_PREV    19
#define MCLK_PHASE_24FPS_PREV  0
#define PCLK_DIV_24FPS_PREV    19
#define PCLK_PHASE_24FPS_PREV  0

#define MCLK_DIV_30FPS_PREV    12
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    12
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP    12
#define MCLK_DIV_SNAP_BURST     18
#define MCLK_DIV_SNAP_BURST_UP  18
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    12
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_24FPS_PREV  13736400L//26692300L
#define PCLK_FREQ_30FPS_PREV  36247800L
#define PCLK_FREQ_60FPS_PREV  26692300L
#define PCLK_FREQ_SNAP        36247800L
#define PCLK_FREQ_SNAP_BURST  	41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */
#else
#define PCLK_FREQ_24FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
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
#define RESHAPE_HEN_24FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
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
#define I2C_DEV_ADDR  0x48
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_24FPS_24_MAX_IDX  122
#define AE_24FPS_24_MIN_IDX  0
#define AE_24FPS_20_MAX_IDX  122
#define AE_24FPS_20_MIN_IDX  0
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  127
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  127
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  116
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  116
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_24FPS_MAX_IDX  255
#define GAIN_24FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  255
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  255
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
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_OFF					/* AE_GAIN_AFTER_EXP_OFF, */
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
static code UINT8 frontDevName[] = "SENSOR - HM HM1355 RAW";

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
#if 1
static code frontAe_t ae20fps50hzTbl[123] = {
	{	0,	332,	252,	0,	0 },
	{	1,	332,	241,	0,	0 },
	{	2,	332,	232,	0,	0 },
	{	3,	332,	221,	0,	0 },
	{	4,	332,	204,	0,	0 },
	{	5,	332,	188,	0,	0 },
	{	6,	332,	174,	0,	0 },
	{	7,	332,	160,	0,	0 },
	{	8,	332,	147,	0,	0 },
	{	9,	332,	135,	0,	0 },
	{	10,	332,	124,	0,	0 },
	{	11,	332,	113,	0,	0 },
	{	12,	332,	104,	0,	0 },
	{	13,	332,	95,	0,	0 },
	{	14,	332,	86,	0,	0 },
	{	15,	332,	78,	0,	0 },
	{	16,	332,	71,	0,	0 },
	{	17,	332,	64,	0,	0 },
	{	18,	332,	58,	0,	0 },
	{	19,	332,	52,	0,	0 },
	{	20,	332,	46,	0,	0 },
	{	21,	332,	41,	0,	0 },
	{	22,	332,	36,	0,	0 },
	{	23,	332,	32,	0,	0 },
	{	24,	332,	27,	0,	0 },
	{	25,	332,	23,	0,	0 },
	{	26,	332,	20,	0,	0 },
	{	27,	332,	16,	0,	0 },
	{	28,	332,	13,	0,	0 },
	{	29,	332,	10,	0,	0 },
	{	30,	332,	7,	0,	0 },
	{	31,	332,	5,	0,	0 },
	{	32,	332,	2,	0,	0 },
	{	33,	332,	0,	0,	0 },
	{	34,	498,	13,	0,	0 },
	{	35,	498,	10,	0,	0 },
	{	36,	498,	7,	0,	0 },
	{	37,	498,	4,	0,	0 },
	{	38,	498,	2,	0,	0 },
	{	39,	498,	0,	0,	0 },
	{	40,	995,	27,	0,	0 },
	{	41,	995,	23,	0,	0 },
	{	42,	995,	19,	0,	0 },
	{	43,	995,	16,	0,	0 },
	{	44,	995,	13,	0,	0 },
	{	45,	995,	10,	0,	0 },
	{	46,	995,	7,	0,	0 },
	{	47,	995,	4,	0,	0 },
	{	48,	995,	2,	0,	0 },
	{	49,	995,	0,	0,	0 },
	{	50,	1035,	0,	0,	0 },
	{	51,	1108,	0,	0,	0 },
	{	52,	1193,	0,	0,	0 },
	{	53,	1271,	0,	0,	0 },
	{	54,	1371,	0,	0,	0 },
	{	55,	1460,	0,	0,	0 },
	{	56,	1579,	0,	0,	0 },
	{	57,	1680,	0,	0,	0 },
	{	58,	1817,	0,	0,	0 },
	{	59,	1929,	0,	0,	0 },
	{	60,	2084,	0,	0,	0 },
	{	61,	2232,	0,	0,	0 },
	{	62,	2404,	0,	0,	0 },
	{	63,	2562,	0,	0,	0 },
	{	64,	2742,	0,	0,	0 },
	{	65,	2949,	0,	0,	0 },
	{	66,	3189,	0,	0,	0 },
	{	67,	3397,	0,	0,	0 },
	{	68,	3634,	0,	0,	0 },
	{	69,	3907,	0,	0,	1 },
	{	70,	4224,	0,	0,	0 },
	{	71,	4465,	0,	0,	0 },
	{	72,	4884,	0,	0,	0 },
	{	73,	5209,	0,	0,	0 },
	{	74,	5581,	0,	0,	0 },
	{	75,	6010,	0,	0,	0 },
	{	76,	6511,	0,	0,	0 },
	{	77,	6794,	0,	0,	1 },
	{	78,	7442,	0,	0,	1 },
	{	79,	7814,	0,	0,	1 },
	{	80,	8682,	0,	0,	1 },
	{	81,	9192,	0,	0,	0 },
	{	82,	9767,	0,	0,	1 },
	{	83,	10418,	0,	0,	0 },
	{	84,	11162,	0,	0,	1 },
	{	85,	12021,	0,	0,	0 },
	{	86,	13023,	0,	0,	1 },
	{	87,	14207,	0,	0,	0 },
	{	88,	15627,	1,	0,	2 },
	{	89,	15627,	0,	0,	2 },
	{	90,	17363,	0,	0,	1 },
	{	91,	19534,	2,	0,	2 },
	{	92,	19534,	0,	0,	2 },
	{	93,	22324,	2,	0,	1 },
	{	94,	22324,	0,	0,	1 },
	{	95,	26045,	2,	0,	4 },
	{	96,	26045,	0,	0,	3 },
	{	97,	31254,	4,	0,	2 },
	{	98,	31254,	1,	0,	2 },
	{	99,	31254,	0,	0,	2 },
	{	100,	39067,	4,	0,	6 },
	{	101,	39067,	2,	0,	6 },
	{	102,	39067,	0,	0,	6 },
	{	103,	52089,	7,	0,	4 },
	{	104,	52089,	5,	0,	4 },
	{	105,	52089,	2,	0,	4 },
	{	106,	52089,	0,	0,	3 },
	{	107,	78133,	12,	0,	21 },
	{	108,	78133,	9,	0,	19 },
	{	109,	78133,	7,	0,	18 },
	{	110,	78133,	4,	0,	17 },
	{	111,	78133,	2,	0,	16 },
	{	112,	78133,	0,	0,	15 },
	{	113,	156257,	26,	0,	27 },
	{	114,	156257,	23,	0,	26 },
	{	115,	156257,	19,	0,	24 },
	{	116,	156257,	16,	0,	23 },
	{	117,	156257,	12,	0,	21 },
	{	118,	156257,	9,	0,	19 },
	{	119,	156257,	7,	0,	18 },
	{	120,	156257,	4,	0,	17 },
	{	121,	156257,	2,	0,	16 },
	{	122,	156257,	0,	0,	15 },
	};

static code frontAe_t ae20fps60hzTbl[124] = {
	{	0,	299,	246,	0,	0 },
	{	1,	299,	236,	0,	0 },
	{	2,	299,	227,	0,	0 },
	{	3,	299,	212,	0,	0 },
	{	4,	299,	196,	0,	0 },
	{	5,	299,	181,	0,	0 },
	{	6,	299,	166,	0,	0 },
	{	7,	299,	153,	0,	0 },
	{	8,	299,	141,	0,	0 },
	{	9,	299,	129,	0,	0 },
	{	10,	299,	118,	0,	0 },
	{	11,	299,	108,	0,	0 },
	{	12,	299,	99,	0,	0 },
	{	13,	299,	90,	0,	0 },
	{	14,	299,	82,	0,	0 },
	{	15,	299,	75,	0,	0 },
	{	16,	299,	67,	0,	0 },
	{	17,	299,	61,	0,	0 },
	{	18,	299,	55,	0,	0 },
	{	19,	299,	49,	0,	0 },
	{	20,	299,	43,	0,	0 },
	{	21,	299,	38,	0,	0 },
	{	22,	299,	34,	0,	0 },
	{	23,	299,	29,	0,	0 },
	{	24,	299,	25,	0,	0 },
	{	25,	299,	22,	0,	0 },
	{	26,	299,	18,	0,	0 },
	{	27,	299,	15,	0,	0 },
	{	28,	299,	12,	0,	0 },
	{	29,	299,	9,	0,	0 },
	{	30,	299,	6,	0,	0 },
	{	31,	299,	3,	0,	0 },
	{	32,	299,	1,	0,	0 },
	{	33,	299,	0,	0,	0 },
	{	34,	400,	6,	0,	0 },
	{	35,	400,	4,	0,	0 },
	{	36,	400,	2,	0,	0 },
	{	37,	400,	0,	0,	0 },
	{	38,	599,	12,	0,	0 },
	{	39,	599,	9,	0,	0 },
	{	40,	599,	6,	0,	0 },
	{	41,	599,	3,	0,	0 },
	{	42,	599,	1,	0,	0 },
	{	43,	599,	0,	0,	0 },
	{	44,	1193,	25,	0,	0 },
	{	45,	1193,	21,	0,	0 },
	{	46,	1193,	18,	0,	0 },
	{	47,	1193,	14,	0,	0 },
	{	48,	1193,	11,	0,	0 },
	{	49,	1193,	9,	0,	0 },
	{	50,	1193,	6,	0,	0 },
	{	51,	1193,	3,	0,	0 },
	{	52,	1193,	1,	0,	0 },
	{	53,	1193,	0,	0,	0 },
	{	54,	1271,	0,	0,	0 },
	{	55,	1371,	0,	0,	0 },
	{	56,	1460,	0,	0,	0 },
	{	57,	1579,	0,	0,	0 },
	{	58,	1680,	0,	0,	0 },
	{	59,	1817,	0,	0,	0 },
	{	60,	1929,	0,	0,	0 },
	{	61,	2084,	0,	0,	0 },
	{	62,	2232,	0,	0,	0 },
	{	63,	2404,	0,	0,	0 },
	{	64,	2562,	0,	0,	0 },
	{	65,	2742,	0,	0,	0 },
	{	66,	2949,	0,	0,	0 },
	{	67,	3189,	0,	0,	0 },
	{	68,	3397,	0,	0,	0 },
	{	69,	3634,	0,	0,	0 },
	{	70,	3907,	0,	0,	1 },
	{	71,	4224,	0,	0,	0 },
	{	72,	4465,	0,	0,	0 },
	{	73,	4884,	0,	0,	0 },
	{	74,	5209,	0,	0,	0 },
	{	75,	5581,	0,	0,	0 },
	{	76,	6010,	0,	0,	0 },
	{	77,	6511,	0,	0,	0 },
	{	78,	6794,	0,	0,	1 },
	{	79,	7442,	0,	0,	1 },
	{	80,	7814,	0,	0,	1 },
	{	81,	8682,	0,	0,	1 },
	{	82,	9192,	0,	0,	0 },
	{	83,	9767,	0,	0,	1 },
	{	84,	10418,	0,	0,	0 },
	{	85,	11162,	0,	0,	1 },
	{	86,	12021,	0,	0,	0 },
	{	87,	13023,	0,	0,	1 },
	{	88,	14207,	0,	0,	0 },
	{	89,	15627,	1,	0,	2 },
	{	90,	15627,	0,	0,	2 },
	{	91,	17363,	0,	0,	1 },
	{	92,	19534,	2,	0,	2 },
	{	93,	19534,	0,	0,	2 },
	{	94,	22324,	2,	0,	1 },
	{	95,	22324,	0,	0,	1 },
	{	96,	26045,	2,	0,	4 },
	{	97,	26045,	0,	0,	3 },
	{	98,	31254,	4,	0,	2 },
	{	99,	31254,	1,	0,	2 },
	{	100,	31254,	0,	0,	2 },
	{	101,	39067,	4,	0,	6 },
	{	102,	39067,	2,	0,	6 },
	{	103,	39067,	0,	0,	6 },
	{	104,	52089,	7,	0,	4 },
	{	105,	52089,	5,	0,	4 },
	{	106,	52089,	2,	0,	4 },
	{	107,	52089,	0,	0,	3 },
	{	108,	78133,	12,	0,	21 },
	{	109,	78133,	9,	0,	19 },
	{	110,	78133,	7,	0,	18 },
	{	111,	78133,	4,	0,	17 },
	{	112,	78133,	2,	0,	16 },
	{	113,	78133,	0,	0,	15 },
	{	114,	156257,	26,	0,	27 },
	{	115,	156257,	23,	0,	26 },
	{	116,	156257,	19,	0,	24 },
	{	117,	156257,	16,	0,	23 },
	{	118,	156257,	12,	0,	21 },
	{	119,	156257,	9,	0,	19 },
	{	120,	156257,	7,	0,	18 },
	{	121,	156257,	4,	0,	17 },
	{	122,	156257,	2,	0,	16 },
	{	123,	156257,	0,	0,	15 },
	};

#endif
#if 0
static code frontAe_t ae20fps50hzTbl[128] = {
	{	0,	333,	412,	0,	1 },
	{	1,	333,	398,	0,	1 },
	{	2,	333,	384,	0,	1 },
	{	3,	333,	371,	0,	1 },
	{	4,	333,	359,	0,	1 },
	{	5,	333,	348,	0,	1 },
	{	6,	333,	337,	0,	1 },
	{	7,	333,	328,	0,	1 },
	{	8,	333,	317,	0,	0 },
	{	9,	333,	300,	0,	0 },
	{	10,	333,	284,	0,	0 },
	{	11,	333,	270,	0,	0 },
	{	12,	333,	256,	0,	0 },
	{	13,	333,	243,	0,	0 },
	{	14,	333,	231,	0,	0 },
	{	15,	333,	220,	0,	0 },
	{	16,	333,	209,	0,	0 },
	{	17,	333,	200,	0,	0 },
	{	18,	333,	189,	0,	0 },
	{	19,	333,	172,	0,	0 },
	{	20,	333,	156,	0,	0 },
	{	21,	333,	142,	0,	0 },
	{	22,	333,	128,	0,	0 },
	{	23,	333,	115,	0,	0 },
	{	24,	333,	103,	0,	0 },
	{	25,	333,	92,	0,	0 },
	{	26,	333,	81,	0,	0 },
	{	27,	333,	72,	0,	0 },
	{	28,	333,	63,	0,	0 },
	{	29,	333,	54,	0,	0 },
	{	30,	333,	46,	0,	0 },
	{	31,	333,	39,	0,	0 },
	{	32,	333,	32,	0,	0 },
	{	33,	333,	26,	0,	0 },
	{	34,	333,	20,	0,	0 },
	{	35,	333,	14,	0,	0 },
	{	36,	333,	9,	0,	0 },
	{	37,	333,	4,	0,	0 },
	{	38,	333,	0,	0,	0 },
	{	39,	500,	25,	0,	0 },
	{	40,	500,	19,	0,	0 },
	{	41,	500,	13,	0,	0 },
	{	42,	500,	8,	0,	0 },
	{	43,	500,	3,	0,	0 },
	{	44,	500,	0,	0,	0 },
	{	45,	995,	53,	0,	0 },
	{	46,	995,	45,	0,	0 },
	{	47,	995,	38,	0,	0 },
	{	48,	995,	31,	0,	0 },
	{	49,	995,	25,	0,	0 },
	{	50,	995,	19,	0,	0 },
	{	51,	995,	13,	0,	0 },
	{	52,	995,	8,	0,	0 },
	{	53,	995,	3,	0,	0 },
	{	54,	995,	0,	0,	0 },
	{	55,	1035,	0,	0,	0 },
	{	56,	1108,	0,	0,	0 },
	{	57,	1193,	0,	0,	0 },
	{	58,	1271,	0,	0,	0 },
	{	59,	1371,	0,	0,	0 },
	{	60,	1460,	0,	0,	0 },
	{	61,	1579,	0,	0,	0 },
	{	62,	1680,	0,	0,	0 },
	{	63,	1817,	0,	0,	0 },
	{	64,	1929,	0,	0,	0 },
	{	65,	2084,	0,	0,	0 },
	{	66,	2232,	0,	0,	0 },
	{	67,	2404,	0,	0,	1 },
	{	68,	2562,	0,	0,	1 },
	{	69,	2742,	0,	0,	1 },
	{	70,	2949,	0,	0,	0 },
	{	71,	3189,	0,	0,	0 },
	{	72,	3397,	0,	0,	1 },
	{	73,	3634,	0,	0,	0 },
	{	74,	3907,	0,	0,	1 },
	{	75,	4224,	0,	0,	1 },
	{	76,	4465,	0,	0,	1 },
	{	77,	4884,	0,	0,	0 },
	{	78,	5209,	0,	0,	0 },
	{	79,	5581,	0,	0,	0 },
	{	80,	6010,	0,	0,	1 },
	{	81,	6511,	0,	0,	1 },
	{	82,	6794,	0,	0,	2 },
	{	83,	7442,	0,	0,	2 },
	{	84,	7814,	0,	0,	1 },
	{	85,	8682,	0,	0,	1 },
	{	86,	9192,	0,	0,	0 },
	{	87,	9767,	0,	0,	2 },
	{	88,	10418,	0,	0,	0 },
	{	89,	11162,	0,	0,	2 },
	{	90,	12021,	0,	0,	1 },
	{	91,	13023,	0,	0,	3 },
	{	92,	14207,	0,	0,	1 },
	{	93,	15627,	2,	0,	3 },
	{	94,	15627,	0,	0,	3 },
	{	95,	17363,	0,	0,	1 },
	{	96,	19534,	3,	0,	5 },
	{	97,	19534,	0,	0,	5 },
	{	98,	22324,	3,	0,	2 },
	{	99,	22324,	0,	0,	2 },
	{	100,	26045,	4,	0,	7 },
	{	101,	26045,	0,	0,	7 },
	{	102,	31254,	7,	0,	4 },
	{	103,	31254,	2,	0,	3 },
	{	104,	31254,	0,	0,	3 },
	{	105,	39067,	8,	0,	13 },
	{	106,	39067,	3,	0,	12 },
	{	107,	39067,	0,	0,	11 },
	{	108,	52089,	14,	0,	8 },
	{	109,	52089,	9,	0,	8 },//ev16
	{	110,	52089,	4,	0,	7 },
	{	111,	52089,	0,	0,	7 },
	{	112,	78133,	24,	0,	42 },
	{	113,	78133,	18,	0,	39 },
	{	114,	78133,	13,	0,	36 },
	{	115,	78133,	8,	0,	34 },
	{	116,	78133,	3,	0,	32 },
	{	117,	78133,	0,	0,	30 },
	{	118,	156257,	52,	0,	55 },
	{	119,	156257,	45,	0,	52 },
	{	120,	156257,	37,	0,	48 },
	{	121,	156257,	31,	0,	45 },
	{	122,	156257,	24,	0,	42 },
	{	123,	156257,	18,	0,	39 },
	{	124,	156257,	13,	0,	36 },
	{	125,	156257,	8,	0,	34 },
	{	126,	156257,	3,	0,	32 },
	{	127,	156257,	0,	0,	30 },
	};



static code frontAe_t ae20fps60hzTbl[129] = {
	{	0,	299,	405,	0,	0 },
	{	1,	299,	390,	0,	0 },
	{	2,	299,	377,	0,	0 },
	{	3,	299,	365,	0,	0 },
	{	4,	299,	353,	0,	0 },
	{	5,	299,	342,	0,	0 },
	{	6,	299,	332,	0,	0 },
	{	7,	299,	323,	0,	0 },
	{	8,	299,	308,	0,	0 },
	{	9,	299,	292,	0,	0 },
	{	10,	299,	277,	0,	0 },
	{	11,	299,	262,	0,	0 },
	{	12,	299,	249,	0,	0 },
	{	13,	299,	237,	0,	0 },
	{	14,	299,	225,	0,	0 },
	{	15,	299,	214,	0,	0 },
	{	16,	299,	204,	0,	0 },
	{	17,	299,	195,	0,	0 },
	{	18,	299,	180,	0,	0 },
	{	19,	299,	164,	0,	0 },
	{	20,	299,	149,	0,	0 },
	{	21,	299,	134,	0,	0 },
	{	22,	299,	121,	0,	0 },
	{	23,	299,	109,	0,	0 },
	{	24,	299,	97,	0,	0 },
	{	25,	299,	86,	0,	0 },
	{	26,	299,	76,	0,	0 },
	{	27,	299,	67,	0,	0 },
	{	28,	299,	58,	0,	0 },
	{	29,	299,	50,	0,	0 },
	{	30,	299,	43,	0,	0 },
	{	31,	299,	35,	0,	0 },
	{	32,	299,	29,	0,	0 },
	{	33,	299,	23,	0,	0 },
	{	34,	299,	17,	0,	0 },
	{	35,	299,	11,	0,	0 },
	{	36,	299,	6,	0,	0 },
	{	37,	299,	2,	0,	0 },
	{	38,	299,	0,	0,	0 },
	{	39,	400,	12,	0,	0 },
	{	40,	400,	7,	0,	0 },
	{	41,	400,	3,	0,	0 },
	{	42,	400,	0,	0,	0 },
	{	43,	599,	23,	0,	0 },
	{	44,	599,	17,	0,	0 },
	{	45,	599,	11,	0,	0 },
	{	46,	599,	6,	0,	0 },
	{	47,	599,	2,	0,	0 },
	{	48,	599,	0,	0,	0 },
	{	49,	1193,	50,	0,	0 },
	{	50,	1193,	42,	0,	0 },
	{	51,	1193,	35,	0,	0 },
	{	52,	1193,	28,	0,	0 },
	{	53,	1193,	22,	0,	0 },
	{	54,	1193,	17,	0,	0 },
	{	55,	1193,	11,	0,	0 },
	{	56,	1193,	6,	0,	0 },
	{	57,	1193,	2,	0,	0 },
	{	58,	1193,	0,	0,	0 },
	{	59,	1271,	0,	0,	0 },
	{	60,	1371,	0,	0,	0 },
	{	61,	1460,	0,	0,	0 },
	{	62,	1579,	0,	0,	0 },
	{	63,	1680,	0,	0,	0 },
	{	64,	1817,	0,	0,	0 },
	{	65,	1929,	0,	0,	0 },
	{	66,	2084,	0,	0,	0 },
	{	67,	2232,	0,	0,	0 },
	{	68,	2404,	0,	0,	1 },
	{	69,	2562,	0,	0,	1 },
	{	70,	2742,	0,	0,	1 },
	{	71,	2949,	0,	0,	0 },
	{	72,	3189,	0,	0,	0 },
	{	73,	3397,	0,	0,	1 },
	{	74,	3634,	0,	0,	0 },
	{	75,	3907,	0,	0,	1 },
	{	76,	4224,	0,	0,	1 },
	{	77,	4465,	0,	0,	1 },
	{	78,	4884,	0,	0,	0 },
	{	79,	5209,	0,	0,	0 },
	{	80,	5581,	0,	0,	0 },
	{	81,	6010,	0,	0,	1 },
	{	82,	6511,	0,	0,	1 },
	{	83,	6794,	0,	0,	2 },
	{	84,	7442,	0,	0,	2 },
	{	85,	7814,	0,	0,	1 },
	{	86,	8682,	0,	0,	1 },
	{	87,	9192,	0,	0,	0 },
	{	88,	9767,	0,	0,	2 },
	{	89,	10418,	0,	0,	0 },
	{	90,	11162,	0,	0,	2 },
	{	91,	12021,	0,	0,	1 },
	{	92,	13023,	0,	0,	3 },
	{	93,	14207,	0,	0,	1 },
	{	94,	15627,	2,	0,	3 },
	{	95,	15627,	0,	0,	3 },
	{	96,	17363,	0,	0,	1 },
	{	97,	19534,	3,	0,	5 },
	{	98,	19534,	0,	0,	5 },
	{	99,	22324,	3,	0,	2 },
	{	100,	22324,	0,	0,	2 },
	{	101,	26045,	4,	0,	7 },
	{	102,	26045,	0,	0,	7 },
	{	103,	31254,	7,	0,	4 },
	{	104,	31254,	2,	0,	3 },
	{	105,	31254,	0,	0,	3 },
	{	106,	39067,	8,	0,	13 },
	{	107,	39067,	3,	0,	12 },
	{	108,	39067,	0,	0,	11 },
	{	109,	52089,	14,	0,	8 },
	{	110,	52089,	9,	0,	8 },
	{	111,	52089,	4,	0,	7 },
	{	112,	52089,	0,	0,	7 },
	{	113,	78133,	24,	0,	42 },
	{	114,	78133,	18,	0,	39 },
	{	115,	78133,	13,	0,	36 },
	{	116,	78133,	8,	0,	34 },
	{	117,	78133,	3,	0,	32 },
	{	118,	78133,	0,	0,	30 },
	{	119,	156257,	52,	0,	55 },
	{	120,	156257,	45,	0,	52 },
	{	121,	156257,	37,	0,	48 },
	{	122,	156257,	31,	0,	45 },
	{	123,	156257,	24,	0,	42 },
	{	124,	156257,	18,	0,	39 },
	{	125,	156257,	13,	0,	36 },
	{	126,	156257,	8,	0,	34 },
	{	127,	156257,	3,	0,	32 },
	{	128,	156257,	0,	0,	30 },
	};



#endif

#if 1
static code frontAe_t ae30fps50hzTbl[128] = {
	{	0,	332,	246,	0,	0 },
	{	1,	332,	236,	0,	0 },
	{	2,	332,	226,	0,	0 },
	{	3,	332,	211,	0,	0 },
	{	4,	332,	194,	0,	0 },
	{	5,	332,	179,	0,	0 },
	{	6,	332,	165,	0,	0 },
	{	7,	332,	152,	0,	0 },
	{	8,	332,	140,	0,	0 },
	{	9,	332,	128,	0,	0 },
	{	10,	332,	118,	0,	0 },
	{	11,	332,	108,	0,	0 },
	{	12,	332,	98,	0,	0 },
	{	13,	332,	90,	0,	0 },
	{	14,	332,	81,	0,	0 },
	{	15,	332,	74,	0,	0 },
	{	16,	332,	67,	0,	0 },
	{	17,	332,	60,	0,	0 },
	{	18,	332,	54,	0,	0 },
	{	19,	332,	48,	0,	0 },
	{	20,	332,	43,	0,	0 },
	{	21,	332,	38,	0,	0 },
	{	22,	332,	33,	0,	0 },
	{	23,	332,	29,	0,	0 },
	{	24,	332,	25,	0,	0 },
	{	25,	332,	21,	0,	0 },
	{	26,	332,	18,	0,	0 },
	{	27,	332,	14,	0,	0 },
	{	28,	332,	11,	0,	0 },
	{	29,	332,	8,	0,	0 },
	{	30,	332,	6,	0,	0 },
	{	31,	332,	3,	0,	0 },
	{	32,	332,	1,	0,	0 },
	{	33,	332,	0,	0,	0 },
	{	34,	498,	11,	0,	0 },
	{	35,	498,	8,	0,	0 },
	{	36,	498,	5,	0,	0 },
	{	37,	498,	3,	0,	0 },
	{	38,	498,	1,	0,	0 },
	{	39,	502,	0,	0,	0 },
	{	40,	993,	24,	0,	0 },
	{	41,	993,	20,	0,	0 },
	{	42,	993,	17,	0,	0 },
	{	43,	993,	14,	0,	0 },
	{	44,	993,	11,	0,	0 },
	{	45,	993,	8,	0,	0 },
	{	46,	993,	5,	0,	0 },
	{	47,	993,	3,	0,	0 },
	{	48,	993,	0,	0,	0 },
	{	49,	1006,	0,	0,	0 },
	{	50,	1076,	0,	0,	0 },
	{	51,	1157,	0,	0,	0 },
	{	52,	1238,	0,	0,	0 },
	{	53,	1331,	0,	0,	0 },
	{	54,	1422,	0,	0,	0 },
	{	55,	1525,	0,	0,	0 },
	{	56,	1633,	0,	0,	0 },
	{	57,	1758,	0,	0,	0 },
	{	58,	1872,	0,	0,	0 },
	{	59,	2020,	0,	0,	0 },
	{	60,	2152,	0,	0,	0 },
	{	61,	2326,	0,	0,	0 },
	{	62,	2476,	0,	0,	0 },
	{	63,	2678,	0,	0,	0 },
	{	64,	2843,	0,	0,	0 },
	{	65,	3071,	0,	0,	0 },
	{	66,	3290,	0,	0,	0 },
	{	67,	3543,	0,	0,	0 },
	{	68,	3775,	0,	0,	0 },
	{	69,	4040,	0,	0,	0 },
	{	70,	4345,	0,	0,	0 },
	{	71,	4700,	0,	0,	1 },
	{	72,	5006,	0,	0,	0 },
	{	73,	5356,	0,	0,	1 },
	{	74,	5757,	0,	0,	0 },
	{	75,	6224,	0,	0,	0 },
	{	76,	6580,	0,	0,	1 },
	{	77,	7197,	0,	0,	1 },
	{	78,	7676,	0,	0,	0 },
	{	79,	8225,	0,	0,	1 },
	{	80,	8857,	0,	0,	0 },
	{	81,	9595,	0,	0,	0 },
	{	82,	10013,	0,	0,	1 },
	{	83,	10966,	0,	0,	0 },
	{	84,	11514,	0,	0,	0 },
	{	85,	12794,	0,	0,	2 },
	{	86,	13546,	0,	0,	0 },
	{	87,	14393,	0,	0,	0 },
	{	88,	15353,	0,	0,	2 },
	{	89,	16449,	0,	0,	0 },
	{	90,	17715,	0,	0,	3 },
	{	91,	19191,	0,	0,	3 },
	{	92,	20935,	0,	0,	0 },
	{	93,	23029,	1,	0,	0 },
	{	94,	23029,	0,	0,	0 },
	{	95,	25588,	0,	0,	4 },
	{	96,	28786,	2,	0,	0 },
	{	97,	28786,	0,	0,	0 },
	{	98,	32898,	2,	0,	0 },
	{	99,	32898,	0,	0,	0 },
	{	100,	38381,	2,	0,	0 },
	{	101,	38381,	0,	0,	0 },
	{	102,	46057,	4,	0,	0 },
	{	103,	46057,	1,	0,	0 },
	{	104,	46057,	0,	0,	0 },
	{	105,	57571,	4,	0,	0 },
	{	106,	57571,	2,	0,	0 },
	{	107,	57571,	0,	0,	0 },
	{	108,	76761,	7,	0,	0 },
	{	109,	76761,	5,	0,	0 },
	{	110,	76761,	2,	0,	0 },
	{	111,	76761,	0,	0,	0 },
	{	112,	115140,	12,	0,	0 },
	{	113,	115140,	9,	0,	0 },
	{	114,	115140,	7,	0,	0 },
	{	115,	115140,	4,	0,	0 },
	{	116,	115140,	2,	0,	0 },
	{	117,	115140,	0,	0,	0 },
	{	118,	230268,	26,	0,	0 },
	{	119,	230268,	23,	0,	0 },
	{	120,	230268,	19,	0,	0 },
	{	121,	230268,	16,	0,	0 },
	{	122,	230268,	12,	0,	0 },
	{	123,	230268,	9,	0,	0 },
	{	124,	230268,	7,	0,	0 },
	{	125,	230268,	4,	0,	0 },
	{	126,	230268,	2,	0,	0 },
	{	127,	230268,	0,	0,	0 },
	};

static code frontAe_t ae30fps60hzTbl[130] = {
	{	0,	299,	251,	0,	0 },
	{	1,	299,	240,	0,	0 },
	{	2,	299,	231,	0,	0 },
	{	3,	299,	219,	0,	0 },
	{	4,	299,	202,	0,	0 },
	{	5,	299,	187,	0,	0 },
	{	6,	299,	172,	0,	0 },
	{	7,	299,	158,	0,	0 },
	{	8,	299,	146,	0,	0 },
	{	9,	299,	134,	0,	0 },
	{	10,	299,	123,	0,	0 },
	{	11,	299,	112,	0,	0 },
	{	12,	299,	103,	0,	0 },
	{	13,	299,	94,	0,	0 },
	{	14,	299,	85,	0,	0 },
	{	15,	299,	78,	0,	0 },
	{	16,	299,	70,	0,	0 },
	{	17,	299,	63,	0,	0 },
	{	18,	299,	57,	0,	0 },
	{	19,	299,	51,	0,	0 },
	{	20,	299,	46,	0,	0 },
	{	21,	299,	40,	0,	0 },
	{	22,	299,	36,	0,	0 },
	{	23,	299,	31,	0,	0 },
	{	24,	299,	27,	0,	0 },
	{	25,	299,	23,	0,	0 },
	{	26,	299,	19,	0,	0 },
	{	27,	299,	16,	0,	0 },
	{	28,	299,	13,	0,	0 },
	{	29,	299,	10,	0,	0 },
	{	30,	299,	7,	0,	0 },
	{	31,	299,	4,	0,	0 },
	{	32,	299,	2,	0,	0 },
	{	33,	299,	0,	0,	0 },
	{	34,	399,	7,	0,	0 },
	{	35,	399,	5,	0,	0 },
	{	36,	399,	2,	0,	0 },
	{	37,	399,	0,	0,	0 },
	{	38,	598,	13,	0,	0 },
	{	39,	598,	10,	0,	0 },
	{	40,	598,	7,	0,	0 },
	{	41,	598,	4,	0,	0 },
	{	42,	598,	2,	0,	0 },
	{	43,	598,	0,	0,	0 },
	{	44,	1193,	27,	0,	0 },
	{	45,	1193,	23,	0,	0 },
	{	46,	1193,	19,	0,	0 },
	{	47,	1193,	16,	0,	0 },
	{	48,	1193,	13,	0,	0 },
	{	49,	1193,	10,	0,	0 },
	{	50,	1193,	7,	0,	0 },
	{	51,	1193,	4,	0,	0 },
	{	52,	1193,	2,	0,	0 },
	{	53,	1193,	0,	0,	0 },
	{	54,	1238,	0,	0,	0 },
	{	55,	1331,	0,	0,	0 },
	{	56,	1422,	0,	0,	0 },
	{	57,	1525,	0,	0,	0 },
	{	58,	1633,	0,	0,	0 },
	{	59,	1758,	0,	0,	0 },
	{	60,	1872,	0,	0,	0 },
	{	61,	2020,	0,	0,	0 },
	{	62,	2152,	0,	0,	0 },
	{	63,	2326,	0,	0,	0 },
	{	64,	2476,	0,	0,	0 },
	{	65,	2678,	0,	0,	0 },
	{	66,	2843,	0,	0,	0 },
	{	67,	3071,	0,	0,	0 },
	{	68,	3290,	0,	0,	0 },
	{	69,	3543,	0,	0,	0 },
	{	70,	3775,	0,	0,	0 },
	{	71,	4040,	0,	0,	0 },
	{	72,	4345,	0,	0,	0 },
	{	73,	4700,	0,	0,	1 },
	{	74,	5006,	0,	0,	0 },
	{	75,	5356,	0,	0,	1 },
	{	76,	5757,	0,	0,	0 },
	{	77,	6224,	0,	0,	0 },
	{	78,	6580,	0,	0,	1 },
	{	79,	7197,	0,	0,	1 },
	{	80,	7676,	0,	0,	0 },
	{	81,	8225,	0,	0,	1 },
	{	82,	8857,	0,	0,	0 },
	{	83,	9595,	0,	0,	0 },
	{	84,	10013,	0,	0,	1 },
	{	85,	10966,	0,	0,	0 },
	{	86,	11514,	0,	0,	0 },
	{	87,	12794,	0,	0,	2 },
	{	88,	13546,	0,	0,	0 },
	{	89,	14393,	0,	0,	0 },
	{	90,	15353,	0,	0,	2 },
	{	91,	16449,	0,	0,	0 },
	{	92,	17715,	0,	0,	3 },
	{	93,	19191,	0,	0,	3 },
	{	94,	20935,	0,	0,	0 },
	{	95,	23029,	1,	0,	0 },
	{	96,	23029,	0,	0,	0 },
	{	97,	25588,	0,	0,	4 },
	{	98,	28786,	2,	0,	0 },
	{	99,	28786,	0,	0,	0 },
	{	100,	32898,	2,	0,	0 },
	{	101,	32898,	0,	0,	0 },
	{	102,	38381,	2,	0,	0 },
	{	103,	38381,	0,	0,	0 },
	{	104,	46057,	4,	0,	0 },
	{	105,	46057,	1,	0,	0 },
	{	106,	46057,	0,	0,	0 },
	{	107,	57571,	4,	0,	0 },
	{	108,	57571,	2,	0,	0 },
	{	109,	57571,	0,	0,	0 },
	{	110,	76761,	7,	0,	0 },
	{	111,	76761,	5,	0,	0 },
	{	112,	76761,	2,	0,	0 },
	{	113,	76761,	0,	0,	0 },
	{	114,	115140,	12,	0,	0 },
	{	115,	115140,	9,	0,	0 },
	{	116,	115140,	7,	0,	0 },
	{	117,	115140,	4,	0,	0 },
	{	118,	115140,	2,	0,	0 },
	{	119,	115140,	0,	0,	0 },
	{	120,	230268,	26,	0,	0 },
	{	121,	230268,	23,	0,	0 },
	{	122,	230268,	19,	0,	0 },
	{	123,	230268,	16,	0,	0 },
	{	124,	230268,	12,	0,	0 },
	{	125,	230268,	9,	0,	0 },
	{	126,	230268,	7,	0,	0 },
	{	127,	230268,	4,	0,	0 },
	{	128,	230268,	2,	0,	0 },
	{	129,	230268,	0,	0,	0 },
	};

#endif
#if 0
static code frontAe_t ae30fps50hzTbl[133] = {
	{	0,	333,	403,	0,	0 },
	{	1,	333,	389,	0,	0 },
	{	2,	333,	376,	0,	0 },
	{	3,	333,	364,	0,	0 },
	{	4,	333,	352,	0,	0 },
	{	5,	333,	342,	0,	0 },
	{	6,	333,	332,	0,	0 },
	{	7,	333,	322,	0,	0 },
	{	8,	333,	307,	0,	0 },
	{	9,	333,	290,	0,	0 },
	{	10,	333,	275,	0,	0 },
	{	11,	333,	261,	0,	0 },
	{	12,	333,	248,	0,	0 },
	{	13,	333,	236,	0,	0 },
	{	14,	333,	224,	0,	0 },
	{	15,	333,	214,	0,	0 },
	{	16,	333,	204,	0,	0 },
	{	17,	333,	194,	0,	0 },
	{	18,	333,	179,	0,	0 },
	{	19,	333,	162,	0,	0 },
	{	20,	333,	147,	0,	0 },
	{	21,	333,	133,	0,	0 },
	{	22,	333,	120,	0,	0 },
	{	23,	333,	108,	0,	0 },
	{	24,	333,	96,	0,	0 },
	{	25,	333,	86,	0,	0 },
	{	26,	333,	76,	0,	0 },
	{	27,	333,	66,	0,	0 },
	{	28,	333,	58,	0,	0 },
	{	29,	333,	49,	0,	0 },
	{	30,	333,	42,	0,	0 },
	{	31,	333,	35,	0,	0 },
	{	32,	333,	28,	0,	0 },
	{	33,	333,	22,	0,	0 },
	{	34,	333,	16,	0,	0 },
	{	35,	333,	11,	0,	0 },
	{	36,	333,	6,	0,	0 },
	{	37,	333,	1,	0,	0 },
	{	38,	333,	0,	0,	0 },
	{	39,	500,	21,	0,	0 },
	{	40,	500,	16,	0,	0 },
	{	41,	500,	10,	0,	0 },
	{	42,	500,	5,	0,	0 },
	{	43,	500,	1,	0,	0 },
	{	44,	500,	0,	0,	0 },
	{	45,	993,	48,	0,	0 },
	{	46,	993,	40,	0,	0 },
	{	47,	993,	33,	0,	0 },
	{	48,	993,	27,	0,	0 },
	{	49,	993,	21,	0,	0 },
	{	50,	993,	15,	0,	0 },
	{	51,	993,	10,	0,	0 },
	{	52,	993,	5,	0,	0 },
	{	53,	993,	0,	0,	0 },
	{	54,	1006,	0,	0,	0 },
	{	55,	1076,	0,	0,	0 },
	{	56,	1157,	0,	0,	0 },
	{	57,	1238,	0,	0,	0 },
	{	58,	1331,	0,	0,	0 },
	{	59,	1422,	0,	0,	0 },
	{	60,	1525,	0,	0,	0 },
	{	61,	1633,	0,	0,	0 },
	{	62,	1758,	0,	0,	0 },
	{	63,	1872,	0,	0,	0 },
	{	64,	2020,	0,	0,	0 },
	{	65,	2152,	0,	0,	0 },
	{	66,	2326,	0,	0,	0 },
	{	67,	2476,	0,	0,	0 },
	{	68,	2678,	0,	0,	1 },
	{	69,	2843,	0,	0,	0 },
	{	70,	3071,	0,	0,	1 },
	{	71,	3290,	0,	0,	1 },
	{	72,	3543,	0,	0,	1 },
	{	73,	3775,	0,	0,	0 },
	{	74,	4040,	0,	0,	0 },
	{	75,	4345,	0,	0,	0 },
	{	76,	4700,	0,	0,	1 },
	{	77,	5006,	0,	0,	0 },
	{	78,	5356,	0,	0,	2 },
	{	79,	5757,	0,	0,	0 },
	{	80,	6224,	0,	0,	0 },
	{	81,	6580,	0,	0,	2 },
	{	82,	7197,	0,	0,	2 },
	{	83,	7676,	0,	0,	0 },
	{	84,	8225,	0,	0,	2 },
	{	85,	8857,	0,	0,	0 },
	{	86,	9595,	0,	0,	0 },
	{	87,	10013,	0,	0,	3 },
	{	88,	10966,	0,	0,	0 },
	{	89,	11514,	0,	0,	0 },
	{	90,	12794,	0,	0,	4 },
	{	91,	13546,	0,	0,	0 },
	{	92,	14393,	0,	0,	0 },
	{	93,	15353,	0,	0,	5 },
	{	94,	16449,	0,	0,	0 },
	{	95,	17715,	0,	0,	5 },
	{	96,	19191,	0,	0,	6 },
	{	97,	20935,	0,	0,	0 },
	{	98,	23029,	2,	0,	0 },
	{	99,	23029,	0,	0,	0 },
	{	100,	25588,	0,	0,	8 },
	{	101,	28786,	3,	0,	0 },
	{	102,	28786,	0,	0,	0 },
	{	103,	32898,	3,	0,	0 },
	{	104,	32898,	0,	0,	0 },
	{	105,	38381,	4,	0,	0 },
	{	106,	38381,	0,	0,	0 },
	{	107,	46057,	7,	0,	0 },
	{	108,	46057,	2,	0,	0 },
	{	109,	46057,	0,	0,	0 },
	{	110,	57571,	8,	0,	0 },
	{	111,	57571,	3,	0,	0 },
	{	112,	57571,	0,	0,	0 },
	{	113,	76761,	14,	0,	0 },
	{	114,	76761,	9,	0,	0 },
	{	115,	76761,	4,	0,	0 },
	{	116,	76761,	0,	0,	0 },
	{	117,	115140,	24,	0,	0 },
	{	118,	115140,	18,	0,	0 },
	{	119,	115140,	13,	0,	0 },
	{	120,	115140,	8,	0,	0 },
	{	121,	115140,	3,	0,	0 },
	{	122,	115140,	0,	0,	0 },
	{	123,	230268,	52,	0,	0 },
	{	124,	230268,	45,	0,	0 },
	{	125,	230268,	37,	0,	0 },
	{	126,	230268,	31,	0,	0 },
	{	127,	230268,	24,	0,	0 },
	{	128,	230268,	18,	0,	0 },
	{	129,	230268,	13,	0,	0 },
	{	130,	230268,	8,	0,	0 },
	{	131,	230268,	3,	0,	0 },
	{	132,	230268,	0,	0,	0 },
	};


static code frontAe_t ae30fps60hzTbl[135] = {
	{	0,	299,	411,	0,	0 },
	{	1,	299,	396,	0,	0 },
	{	2,	299,	382,	0,	0 },
	{	3,	299,	370,	0,	0 },
	{	4,	299,	358,	0,	0 },
	{	5,	299,	347,	0,	0 },
	{	6,	299,	336,	0,	0 },
	{	7,	299,	327,	0,	0 },
	{	8,	299,	315,	0,	0 },
	{	9,	299,	298,	0,	0 },
	{	10,	299,	283,	0,	0 },
	{	11,	299,	268,	0,	0 },
	{	12,	299,	254,	0,	0 },
	{	13,	299,	242,	0,	0 },
	{	14,	299,	230,	0,	0 },
	{	15,	299,	219,	0,	0 },
	{	16,	299,	208,	0,	0 },
	{	17,	299,	199,	0,	0 },
	{	18,	299,	187,	0,	0 },
	{	19,	299,	170,	0,	0 },
	{	20,	299,	155,	0,	0 },
	{	21,	299,	140,	0,	0 },
	{	22,	299,	126,	0,	0 },
	{	23,	299,	114,	0,	0 },
	{	24,	299,	102,	0,	0 },
	{	25,	299,	91,	0,	0 },
	{	26,	299,	80,	0,	0 },
	{	27,	299,	71,	0,	0 },
	{	28,	299,	62,	0,	0 },
	{	29,	299,	53,	0,	0 },
	{	30,	299,	46,	0,	0 },
	{	31,	299,	38,	0,	0 },
	{	32,	299,	31,	0,	0 },
	{	33,	299,	25,	0,	0 },
	{	34,	299,	19,	0,	0 },
	{	35,	299,	14,	0,	0 },
	{	36,	299,	8,	0,	0 },
	{	37,	299,	4,	0,	0 },
	{	38,	299,	0,	0,	0 },
	{	39,	399,	14,	0,	0 },
	{	40,	399,	9,	0,	0 },
	{	41,	399,	4,	0,	0 },
	{	42,	399,	0,	0,	0 },
	{	43,	598,	25,	0,	0 },
	{	44,	598,	19,	0,	0 },
	{	45,	598,	14,	0,	0 },
	{	46,	598,	8,	0,	0 },
	{	47,	598,	4,	0,	0 },
	{	48,	598,	0,	0,	0 },
	{	49,	1193,	53,	0,	0 },
	{	50,	1193,	45,	0,	0 },
	{	51,	1193,	38,	0,	0 },
	{	52,	1193,	31,	0,	0 },
	{	53,	1193,	25,	0,	0 },
	{	54,	1193,	19,	0,	0 },
	{	55,	1193,	13,	0,	0 },
	{	56,	1193,	8,	0,	0 },
	{	57,	1193,	3,	0,	0 },
	{	58,	1193,	0,	0,	0 },
	{	59,	1238,	0,	0,	0 },
	{	60,	1331,	0,	0,	0 },
	{	61,	1422,	0,	0,	0 },
	{	62,	1525,	0,	0,	0 },
	{	63,	1633,	0,	0,	0 },
	{	64,	1758,	0,	0,	0 },
	{	65,	1872,	0,	0,	0 },
	{	66,	2020,	0,	0,	0 },
	{	67,	2152,	0,	0,	0 },
	{	68,	2326,	0,	0,	0 },
	{	69,	2476,	0,	0,	0 },
	{	70,	2678,	0,	0,	1 },
	{	71,	2843,	0,	0,	0 },
	{	72,	3071,	0,	0,	1 },
	{	73,	3290,	0,	0,	1 },
	{	74,	3543,	0,	0,	1 },
	{	75,	3775,	0,	0,	0 },
	{	76,	4040,	0,	0,	0 },
	{	77,	4345,	0,	0,	0 },
	{	78,	4700,	0,	0,	1 },
	{	79,	5006,	0,	0,	0 },
	{	80,	5356,	0,	0,	2 },
	{	81,	5757,	0,	0,	0 },
	{	82,	6224,	0,	0,	0 },
	{	83,	6580,	0,	0,	2 },
	{	84,	7197,	0,	0,	2 },
	{	85,	7676,	0,	0,	0 },
	{	86,	8225,	0,	0,	2 },
	{	87,	8857,	0,	0,	0 },
	{	88,	9595,	0,	0,	0 },
	{	89,	10013,	0,	0,	3 },
	{	90,	10966,	0,	0,	0 },
	{	91,	11514,	0,	0,	0 },
	{	92,	12794,	0,	0,	4 },
	{	93,	13546,	0,	0,	0 },
	{	94,	14393,	0,	0,	0 },
	{	95,	15353,	0,	0,	5 },
	{	96,	16449,	0,	0,	0 },
	{	97,	17715,	0,	0,	5 },
	{	98,	19191,	0,	0,	6 },
	{	99,	20935,	0,	0,	0 },
	{	100,	23029,	2,	0,	0 },
	{	101,	23029,	0,	0,	0 },
	{	102,	25588,	0,	0,	8 },
	{	103,	28786,	3,	0,	0 },
	{	104,	28786,	0,	0,	0 },
	{	105,	32898,	3,	0,	0 },
	{	106,	32898,	0,	0,	0 },
	{	107,	38381,	4,	0,	0 },
	{	108,	38381,	0,	0,	0 },
	{	109,	46057,	7,	0,	0 },
	{	110,	46057,	2,	0,	0 },
	{	111,	46057,	0,	0,	0 },
	{	112,	57571,	8,	0,	0 },
	{	113,	57571,	3,	0,	0 },
	{	114,	57571,	0,	0,	0 },
	{	115,	76761,	14,	0,	0 },
	{	116,	76761,	9,	0,	0 },
	{	117,	76761,	4,	0,	0 },
	{	118,	76761,	0,	0,	0 },
	{	119,	115140,	24,	0,	0 },
	{	120,	115140,	18,	0,	0 },
	{	121,	115140,	13,	0,	0 },
	{	122,	115140,	8,	0,	0 },
	{	123,	115140,	3,	0,	0 },
	{	124,	115140,	0,	0,	0 },
	{	125,	230268,	52,	0,	0 },
	{	126,	230268,	45,	0,	0 },
	{	127,	230268,	37,	0,	0 },
	{	128,	230268,	31,	0,	0 },
	{	129,	230268,	24,	0,	0 },
	{	130,	230268,	18,	0,	0 },
	{	131,	230268,	13,	0,	0 },
	{	132,	230268,	8,	0,	0 },
	{	133,	230268,	3,	0,	0 },
	{	134,	230268,	0,	0,	0 },
	};

#endif

/* real fps 30, 50Hz */
static code frontAe_t ae60fps50hzTbl[117] = {
	{	0,	992,	254,	0,	0 },
	{	1,	992,	243,	0,	0 },
	{	2,	992,	234,	0,	0 },
	{	3,	992,	224,	0,	0 },
	{	4,	992,	207,	0,	0 },
	{	5,	992,	191,	0,	0 },
	{	6,	992,	176,	0,	0 },
	{	7,	992,	162,	0,	0 },
	{	8,	992,	149,	0,	0 },
	{	9,	992,	137,	0,	0 },
	{	10,	992,	126,	0,	0 },
	{	11,	992,	115,	0,	0 },
	{	12,	992,	106,	0,	0 },
	{	13,	992,	96,	0,	0 },
	{	14,	992,	88,	0,	0 },
	{	15,	992,	80,	0,	0 },
	{	16,	992,	72,	0,	0 },
	{	17,	992,	65,	0,	0 },
	{	18,	992,	59,	0,	0 },
	{	19,	992,	53,	0,	0 },
	{	20,	992,	47,	0,	0 },
	{	21,	992,	42,	0,	0 },
	{	22,	992,	37,	0,	0 },
	{	23,	992,	32,	0,	0 },
	{	24,	992,	28,	0,	0 },
	{	25,	992,	24,	0,	0 },
	{	26,	992,	20,	0,	0 },
	{	27,	992,	17,	0,	0 },
	{	28,	992,	14,	0,	0 },
	{	29,	992,	11,	0,	0 },
	{	30,	992,	8,	0,	0 },
	{	31,	992,	5,	0,	0 },
	{	32,	992,	3,	0,	0 },
	{	33,	992,	0,	0,	0 },
	{	34,	1006,	0,	0,	0 },
	{	35,	1077,	0,	0,	0 },
	{	36,	1155,	0,	0,	0 },
	{	37,	1234,	0,	0,	0 },
	{	38,	1326,	0,	0,	0 },
	{	39,	1419,	0,	0,	0 },
	{	40,	1526,	0,	0,	0 },
	{	41,	1633,	0,	0,	0 },
	{	42,	1755,	0,	0,	0 },
	{	43,	1874,	0,	0,	0 },
	{	44,	2011,	0,	0,	0 },
	{	45,	2154,	0,	0,	0 },
	{	46,	2318,	0,	0,	0 },
	{	47,	2469,	0,	0,	0 },
	{	48,	2664,	0,	0,	0 },
	{	49,	2838,	0,	0,	0 },
	{	50,	3067,	0,	0,	0 },
	{	51,	3265,	0,	0,	0 },
	{	52,	3531,	0,	0,	0 },
	{	53,	3749,	0,	0,	0 },
	{	54,	4049,	0,	0,	1 },
	{	55,	4338,	0,	0,	0 },
	{	56,	4672,	0,	0,	0 },
	{	57,	4978,	0,	0,	0 },
	{	58,	5327,	0,	0,	0 },
	{	59,	5730,	0,	0,	0 },
	{	60,	6197,	0,	0,	0 },
	{	61,	6601,	0,	0,	1 },
	{	62,	7062,	0,	0,	1 },
	{	63,	7592,	0,	0,	0 },
	{	64,	8207,	0,	0,	0 },
	{	65,	8676,	0,	0,	1 },
	{	66,	9490,	0,	0,	0 },
	{	67,	10122,	0,	0,	1 },
	{	68,	10845,	0,	0,	0 },
	{	69,	11679,	0,	0,	1 },
	{	70,	12653,	0,	0,	0 },
	{	71,	13203,	0,	0,	1 },
	{	72,	14460,	0,	0,	2 },
	{	73,	15183,	0,	0,	0 },
	{	74,	16870,	0,	0,	2 },
	{	75,	17863,	0,	0,	2 },
	{	76,	18979,	0,	0,	0 },
	{	77,	20244,	0,	0,	1 },
	{	78,	21690,	0,	0,	2 },
	{	79,	23359,	0,	0,	3 },
	{	80,	25305,	0,	0,	0 },
	{	81,	27606,	0,	0,	1 },
	{	82,	30366,	1,	0,	3 },
	{	83,	30366,	0,	0,	3 },
	{	84,	33740,	0,	0,	4 },
	{	85,	37958,	2,	0,	0 },
	{	86,	37958,	0,	0,	0 },
	{	87,	43380,	2,	0,	2 },
	{	88,	43380,	0,	0,	2 },
	{	89,	50610,	2,	0,	5 },
	{	90,	50610,	0,	0,	4 },
	{	91,	60732,	4,	0,	10 },
	{	92,	60732,	1,	0,	9 },
	{	93,	60732,	0,	0,	8 },
	{	94,	75915,	4,	0,	0 },
	{	95,	75915,	2,	0,	0 },
	{	96,	75915,	0,	0,	0 },
	{	97,	101219,	7,	0,	5 },
	{	98,	101219,	5,	0,	5 },
	{	99,	101219,	2,	0,	5 },
	{	100,	101219,	0,	0,	4 },
	{	101,	151826,	12,	0,	23 },
	{	102,	151826,	9,	0,	21 },
	{	103,	151826,	7,	0,	20 },
	{	104,	151826,	4,	0,	19 },
	{	105,	151826,	2,	0,	18 },
	{	106,	151826,	0,	0,	17 },
	{	107,	303636,	26,	0,	0 },
	{	108,	303636,	23,	0,	0 },
	{	109,	303636,	19,	0,	0 },
	{	110,	303636,	16,	0,	0 },
	{	111,	303636,	12,	0,	0 },
	{	112,	303636,	9,	0,	0 },
	{	113,	303636,	7,	0,	0 },
	{	114,	303636,	4,	0,	0 },
	{	115,	303636,	2,	0,	0 },
	{	116,	303636,	0,	0,	0 },
	};
static code frontAe_t ae60fps60hzTbl[124] = {
	{	0,	598,	251,	0,	0 },
	{	1,	598,	240,	0,	0 },
	{	2,	598,	231,	0,	0 },
	{	3,	598,	219,	0,	0 },
	{	4,	598,	202,	0,	0 },
	{	5,	598,	186,	0,	0 },
	{	6,	598,	172,	0,	0 },
	{	7,	598,	158,	0,	0 },
	{	8,	598,	145,	0,	0 },
	{	9,	598,	134,	0,	0 },
	{	10,	598,	123,	0,	0 },
	{	11,	598,	112,	0,	0 },
	{	12,	598,	103,	0,	0 },
	{	13,	598,	94,	0,	0 },
	{	14,	598,	85,	0,	0 },
	{	15,	598,	77,	0,	0 },
	{	16,	598,	70,	0,	0 },
	{	17,	598,	63,	0,	0 },
	{	18,	598,	57,	0,	0 },
	{	19,	598,	51,	0,	0 },
	{	20,	598,	46,	0,	0 },
	{	21,	598,	40,	0,	0 },
	{	22,	598,	36,	0,	0 },
	{	23,	598,	31,	0,	0 },
	{	24,	598,	27,	0,	0 },
	{	25,	598,	23,	0,	0 },
	{	26,	598,	19,	0,	0 },
	{	27,	598,	16,	0,	0 },
	{	28,	598,	13,	0,	0 },
	{	29,	598,	10,	0,	0 },
	{	30,	598,	7,	0,	0 },
	{	31,	598,	4,	0,	0 },
	{	32,	598,	2,	0,	0 },
	{	33,	598,	0,	0,	0 },
	{	34,	1191,	27,	0,	0 },
	{	35,	1191,	23,	0,	0 },
	{	36,	1191,	19,	0,	0 },
	{	37,	1191,	16,	0,	0 },
	{	38,	1191,	13,	0,	0 },
	{	39,	1191,	10,	0,	0 },
	{	40,	1191,	7,	0,	0 },
	{	41,	1191,	4,	0,	0 },
	{	42,	1191,	2,	0,	0 },
	{	43,	1191,	0,	0,	0 },
	{	44,	1234,	0,	0,	0 },
	{	45,	1326,	0,	0,	0 },
	{	46,	1419,	0,	0,	0 },
	{	47,	1526,	0,	0,	0 },
	{	48,	1633,	0,	0,	0 },
	{	49,	1755,	0,	0,	0 },
	{	50,	1874,	0,	0,	0 },
	{	51,	2011,	0,	0,	0 },
	{	52,	2154,	0,	0,	0 },
	{	53,	2318,	0,	0,	0 },
	{	54,	2469,	0,	0,	0 },
	{	55,	2664,	0,	0,	0 },
	{	56,	2838,	0,	0,	0 },
	{	57,	3067,	0,	0,	0 },
	{	58,	3265,	0,	0,	0 },
	{	59,	3531,	0,	0,	0 },
	{	60,	3749,	0,	0,	0 },
	{	61,	4049,	0,	0,	1 },
	{	62,	4338,	0,	0,	0 },
	{	63,	4672,	0,	0,	0 },
	{	64,	4978,	0,	0,	0 },
	{	65,	5327,	0,	0,	0 },
	{	66,	5730,	0,	0,	0 },
	{	67,	6197,	0,	0,	0 },
	{	68,	6601,	0,	0,	1 },
	{	69,	7062,	0,	0,	1 },
	{	70,	7592,	0,	0,	0 },
	{	71,	8207,	0,	0,	0 },
	{	72,	8676,	0,	0,	1 },
	{	73,	9490,	0,	0,	0 },
	{	74,	10122,	0,	0,	1 },
	{	75,	10845,	0,	0,	0 },
	{	76,	11679,	0,	0,	1 },
	{	77,	12653,	0,	0,	0 },
	{	78,	13203,	0,	0,	1 },
	{	79,	14460,	0,	0,	2 },
	{	80,	15183,	0,	0,	0 },
	{	81,	16870,	0,	0,	2 },
	{	82,	17863,	0,	0,	2 },
	{	83,	18979,	0,	0,	0 },
	{	84,	20244,	0,	0,	1 },
	{	85,	21690,	0,	0,	2 },
	{	86,	23359,	0,	0,	3 },
	{	87,	25305,	0,	0,	0 },
	{	88,	27606,	0,	0,	1 },
	{	89,	30366,	1,	0,	3 },
	{	90,	30366,	0,	0,	3 },
	{	91,	33740,	0,	0,	4 },
	{	92,	37958,	2,	0,	0 },
	{	93,	37958,	0,	0,	0 },
	{	94,	43380,	2,	0,	2 },
	{	95,	43380,	0,	0,	2 },
	{	96,	50610,	2,	0,	5 },
	{	97,	50610,	0,	0,	4 },
	{	98,	60732,	4,	0,	10 },
	{	99,	60732,	1,	0,	9 },
	{	100,	60732,	0,	0,	8 },
	{	101,	75915,	4,	0,	0 },
	{	102,	75915,	2,	0,	0 },
	{	103,	75915,	0,	0,	0 },
	{	104,	101219,	7,	0,	5 },
	{	105,	101219,	5,	0,	5 },
	{	106,	101219,	2,	0,	5 },
	{	107,	101219,	0,	0,	4 },
	{	108,	151826,	12,	0,	23 },
	{	109,	151826,	9,	0,	21 },
	{	110,	151826,	7,	0,	20 },
	{	111,	151826,	4,	0,	19 },
	{	112,	151826,	2,	0,	18 },
	{	113,	151826,	0,	0,	17 },
	{	114,	303636,	26,	0,	0 },
	{	115,	303636,	23,	0,	0 },
	{	116,	303636,	19,	0,	0 },
	{	117,	303636,	16,	0,	0 },
	{	118,	303636,	12,	0,	0 },
	{	119,	303636,	9,	0,	0 },
	{	120,	303636,	7,	0,	0 },
	{	121,	303636,	4,	0,	0 },
	{	122,	303636,	2,	0,	0 },
	{	123,	303636,	0,	0,	0 },
	};


/* real fps 60, 60Hz */

/* gain table. */

code UINT16 gainTbl[255] = {
   64,    66,    68,    70,    72,    74,    76,    78,    80,    82, 
   84,    86,    88,    90,    92,    94,    96,    98,   100,   102, 
  104,   106,   108,   110,   112,   114,   116,   118,   120,   122, 
  124,   126,   128,   130,   132,   134,   136,   138,   140,   142, 
  144,   146,   148,   150,   152,   154,   156,   158,   160,   162, 
  164,   166,   168,   170,   172,   174,   176,   178,   180,   182, 
  184,   186,   188,   190,   192,   194,   196,   198,   200,   202, 
  204,   206,   208,   210,   212,   214,   216,   218,   220,   222, 
  224,   226,   228,   230,   232,   234,   236,   238,   240,   242, 
  244,   246,   248,   250,   252,   254,   384,   385,   386,   387, 
  388,   389,   390,   391,   392,   393,   394,   395,   396,   397, 
  398,   399,   400,   401,   402,   403,   404,   405,   406,   407, 
  408,   409,   410,   411,   412,   413,   414,   415,   416,   417, 
  418,   419,   420,   421,   422,   423,   424,   425,   426,   427, 
  428,   429,   430,   431,   432,   433,   434,   435,   436,   437, 
  438,   439,   440,   441,   442,   443,   444,   445,   446,   447, 
  448,   449,   450,   451,   452,   453,   454,   455,   456,   457, 
  458,   459,   460,   461,   462,   463,   464,   465,   466,   467, 
  468,   469,   470,   471,   472,   473,   474,   475,   476,   477, 
  478,   479,   480,   481,   482,   483,   484,   485,   486,   487, 
  488,   489,   490,   491,   492,   493,   494,   495,   496,   497, 
  498,   499,   500,   501,   502,   503,   504,   505,   506,   507, 
  508,   509,   510,   511,   640,   641,   642,   643,   644,   645, 
  646,   647,   648,   649,   650,   651,   652,   653,   654,   655, 
  656,   657,   658,   659,   660,   661,   662,   663,   664,   665, 
  666,   667,   668,   669,   670, 	};

#if 0
static code UINT16 gainTbl[432] = {
   64,    65,    66,    67,    68,    69,    70,    71,    72,    73, 
   74,    75,    76,    77,    78,    79,    80,    81,    82,    83, 
   84,    85,    86,    87,    88,    89,    90,    91,    92,    93, 
   94,    95,    96,    97,    98,    99,   100,   101,   102,   103, 
  104,   105,   106,   107,   108,   109,   110,   111,   112,   113, 
  114,   115,   116,   117,   118,   119,   120,   121,   122,   123, 
  124,   125,   126,   127,   128,   129,   130,   131,   132,   133, 
  134,   135,   136,   137,   138,   139,   140,   141,   142,   143, 
  144,   145,   146,   147,   148,   149,   150,   151,   152,   153, 
  154,   155,   156,   157,   158,   159,   160,   161,   162,   163, 
  164,   165,   166,   167,   168,   169,   170,   171,   172,   173, 
  174,   175,   176,   177,   178,   179,   180,   181,   182,   183, 
  184,   185,   186,   187,   188,   189,   190,   191,   192,   193, 
  194,   195,   196,   197,   198,   199,   200,   201,   202,   203, 
  204,   205,   206,   207,   208,   209,   210,   211,   212,   213, 
  214,   215,   216,   217,   218,   219,   220,   221,   222,   223, 
  224,   225,   226,   227,   228,   229,   230,   231,   232,   233, 
  234,   235,   236,   237,   238,   239,   240,   241,   242,   243, 
  244,   245,   246,   247,   248,   249,   250,   251,   252,   253, 
  254,   255,   384,   385,   386,   387,   388,   389,   390,   391, //200
  392,   393,   394,   395,   396,   397,   398,   399,   400,   401, 
  402,   403,   404,   405,   406,   407,   408,   409,   410,   411, 
  412,   413,   414,   415,   416,   417,   418,   419,   420,   421, 
  422,   423,   424,   425,   426,   427,   428,   429,   430,   431, 
  432,   433,   434,   435,   436,   437,   438,   439,   440,   441, 
  442,   443,   444,   445,   446,   447,   448,   449,   450,   451, 
  452,   453,   454,   455,   456,   457,   458,   459,   460,   461, 
  462,   463,   464,   465,   466,   467,   468,   469,   470,   471, 
  472,   473,   474,   475,   476,   477,   478,   479,   480,   481, 
  482,   483,   484,   485,   486,   487,   488,   489,   490,   491, 
  492,   493,   494,   495,   496,   497,   498,   499,   500,   501, 
  502,   503,   504,   505,   506,   507,   508,   509,   510,   511, //320
  640,   641,   642,   643,   644,   645,   646,   647,   648,   649, 
  650,   651,   652,   653,   654,   655,   656,   657,   658,   659, 
  660,   661,   662,   663,   664,   665,   666,   667,   668,   669, 
  670,   671,   672,   673,   674,   675,   676,   677,   678,   679, 
  680,   681,   682,   683,   684,   685,   686,   687,   688,   689, 
  690,   691,   692,   693,   694,   695,   696,   697,   698,   699, 
  700,   701,   702,   703,   704,   705,   706,   707,   708,   709, 
  710,   711,   712,   713,   714,   715,   716,   717,   718,   719, 
  720,   721,   722,   723,   724,   725,   726,   727,   728,   729, 
  730,   731,   732,   733,   734,   735,   736,   737,   738,   739, 
  740,   741,   742,   743,   744,   745,   746,   747,   748,   749, 
  750,   751, 	};
#endif

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
		MCLK_DIV_24FPS_PREV + 6,
		MCLK_PHASE_24FPS_PREV,
		PCLK_DIV_24FPS_PREV + 6,
		PCLK_PHASE_24FPS_PREV,
		29000000L,
		24,
		6,
	},
	{  /* 30 fps. */
		MCLK_DIV_30FPS_PREV + 6,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV + 6,
		PCLK_PHASE_30FPS_PREV,
		29000000L,
		24,
		6,
	},
	{  /* 60 fps. */
		MCLK_DIV_60FPS_PREV,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV,
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
	0x00,0x22, 0x00,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x00,0x06, 0x08,
  
	0x00,0x0D, 0x00,
	0x00,0x0E, 0x00,
	0x00,0x0F, 0x00,
  
	0x00,0x10, 0x00,
	0x00,0x11, 0x02,
	0x00,0x12, 0x01,
       0x00,0x13, 0x00,
	 
	0x00,0x15, 0x01,
	0x00,0x16, 0x80,
  	0x00,0x18, 0x00,
	0x00,0x1D, 0x40,
	  
	0x00,0x20, 0x08,
	0x00,0x23, 0xFF,
	//0x00,0x25, 0x01,//
	0x00,0x26, 0x44,//
	0x00,0x27, 0x20,//
   
	0x00,0x40, 0x08,
	0x00,0x4A, 0x0A,
	0x00,0x4B, 0x72,
	0x00,0x4D, 0xBF,
	0x00,0x4E, 0x60,
	0x00,0x53, 0x08,
     

	0x00,0x70, 0x44,
	0x00,0x71, 0x9F,
	0x00,0x72, 0x33,
	0x00,0x75, 0x40,
	0x00,0x76, 0x2F,
	0x00,0x82, 0xA2,
	0x00,0x83, 0xB0,
	0x00,0x86, 0x49,
	0x00,0x87, 0x92,
	0x00,0x89, 0x0E,
	0x00,0x8A, 0x2D,
	0x00,0x8F, 0x00,
    
	0x00,0x9D, 0x01,
   
	0x01,0x20, 0x02,//
	0x01,0x21, 0x00,
	0x01,0x22, 0x82,//0xE3,
	0x01,0x23, 0x00,//0x0C,
	0x01,0x24, 0x42,//0x52,
	0x01,0x25, 0x00,//0xDF,
	0x01,0x26, 0x31,
 
	0x03,0x80, 0xFC,//0xFF,   
    
	0x05,0xE4, 0x08,
	0x05,0xE5, 0x00,
	0x05,0xE6, 0x07,
	0x05,0xE7, 0x05,
	0x05,0xE8, 0x08,
	0x05,0xE9, 0x00,
	0x05,0xEA, 0x07,
	0x05,0xEB, 0x04,   


	0x00,0x00, 0x01,
	0x00,0x05, 0x01,

};

#if 1
static code UINT8 front24fpsPrevCmdTbl[] = {
	0x00,0x06, 0x0c,	

	0x00,0x0D, 0x11,
	0x00,0x0E, 0x11,
	0x00,0x0F, 0x00,

	0x00,0x10, 0x00,
	0x00,0x11, 0x02,
	0x00,0x12, 0x01,
	0x00,0x13, 0x00,

	0x00,0x20, 0x08,
	0x00,0x23, 0xFF,
	//0x00,0x25, 0x01,//
	0x00,0x26, 0x62,//
	0x00,0x27, 0x20,//

	0x01,0x22, 0x02,//0xE3,
	0x01,0x23, 0x21,//0x0C,	
	
	0x00,0x00, 0x01,	
};
#endif

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x00,0x06, 0x08,	

	0x00,0x0D, 0x00,
	0x00,0x0E, 0x00,
	0x00,0x0F, 0x00,

	0x00,0x10, 0x00,
	0x00,0x11, 0x02,
	0x00,0x12, 0x01,
	0x00,0x13, 0x00,

	0x00,0x20, 0x08,
	0x00,0x23, 0xFF,
	//0x00,0x25, 0x01,//
	0x00,0x26, 0x44,//
	0x00,0x27, 0x20,//

	0x01,0x22, 0x82,//0xE3,
	0x01,0x23, 0x00,//0x0C,

	0x00,0x00, 0x01,
};

static code UINT8 front60fpsPrevCmdTbl[] = {	
	0x00,0x06, 0x0c,	

	0x00,0x0D, 0x11,
	0x00,0x0E, 0x11,
	0x00,0x0F, 0x00,

	0x00,0x10, 0x00,
	0x00,0x11, 0x02,
	0x00,0x12, 0x01,
	0x00,0x13, 0x00,

	0x00,0x20, 0x08,
	0x00,0x23, 0xFF,
	//0x00,0x25, 0x01,//
	0x00,0x26, 0x63,//
	0x00,0x27, 0x20,//

	0x01,0x22, 0x02,//0xE3,
	0x01,0x23, 0x21,//0x0C,	
	
	0x00,0x00, 0x01,
};

static code UINT8 frontSnapCmdTbl[] = {
	0x00,0x06, 0x00,	

	0x00,0x0D, 0x00,
	0x00,0x0E, 0x00,
	0x00,0x0F, 0x00,

	0x00,0x10, 0x00,
	0x00,0x11, 0x02,
	0x00,0x12, 0x01,
	0x00,0x13, 0x00,

	0x00,0x20, 0x08,
	0x00,0x23, 0xFF,
	//0x00,0x25, 0x01,//
	0x00,0x26, 0x44,//
	0x00,0x27, 0x20,//

	0x01,0x22, 0x82,//0xE3,
	0x01,0x23, 0x00,//0x0C,
	
	0x00,0x00, 0x01,
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
	#if (DBG_SNAP_TIMING == 1)
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
		frontResCmdBinSend(_front24fpsPrevCmdTbl);
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

	scaleUp=scaleUp;
	//printf("first=%bd,mode=%bd\n",first,mode);
	
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#if (DBG_SNAP_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
	
		frontResCmdBinSend(_frontSnapCmdTbl);
			
		frontSnapExposureTimeSet(mode,scaleUp);
	}

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
	#endif

	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	
	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
	#endif

	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
	#endif

	HAL_CdspEofWait(1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

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
	UINT8 cmdTbl[3*3];
	//UINT8 ADDR8=0x00;

	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
	//frontLastGain = 0x0240;
	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x18;
	cmdTbl[2] = (UINT8)((frontLastGain & 0x0700)>>8);
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x1d;
	cmdTbl[5] = (UINT8)(frontLastGain & 0x00FF);
	cmdTbl[6] = 0x00;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = 0x01;
	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
	UINT32 pixclk;
	UINT8 cmdTbl[3*3];
	//UINT8 REG_ADDR8[2]={0x16, 0x10};
	//UINT8 REG_ADDR[2];

	
	//printf("frontPrevMode = %bu\n",frontPrevMode);
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
//frontLastShutter = 333;
	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	
	/* real 30 fps, real 60 fps 
	if(espline > (ftotal - 1)) {
		espline = (ftotal - 1);
		//printf("re-define espline=%d\n",espline);
	}		*/

	/* dummy line set */
	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x16;
	cmdTbl[2] = (UINT8)(espline & 0x00FF);

	//exposure time set
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x15;
	cmdTbl[5] = (UINT8)((espline & 0xFF00) >> 8);
	cmdTbl[6] = 0x00;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = 0x01;
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

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
	UINT8 cmdTbl[3*3];
	//UINT8 REG_ADDR8[4]={0x00, 0x18,0x00,0x1d};
	//UINT8 REG_ADDR[2];
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
//frontLastGain = 0x0240;
	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x18;
	cmdTbl[2] = (UINT8)((frontLastGain & 0x0700)>>8);
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x1d;
	cmdTbl[5] = (UINT8)(frontLastGain & 0x00FF);
	cmdTbl[6] = 0x00;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = 0x01;
	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	/*i2cRegRead(&REG_ADDR8[0],&REG_ADDR[0],1,2,1,1,0); 
	printf("frontLastGain=%d\n",frontLastGain);
	printf("read=0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1]);*/
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
	UINT8 cmdTbl[3*3];
	//UINT8 REG_ADDR8[2]={0x00, 0x16};
	//UINT8 REG_ADDR[2];

	
	//printf("frontPrevMode = %bu\n",frontPrevMode);
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;
//frontLastShutter = 333;
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	if (frontIntrPreviewDynamicTempoalDenoiseEnGet() == FRONT_PV_DYNAMIC_TD_ENABLE) {
		pixclk = pcap->prevDynamicTDTbl->pclkFreq;
	}
	else {
		pixclk = pcap->pclkFreq;
	}
	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	
	/* real 30 fps, real 60 fps */
	/*if(espline > (ftotal - 1)) {
		espline = (ftotal - 1);
		//printf("re-define espline=%d\n",espline);
	}	*/	

	/* dummy line set */
	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x16;
	cmdTbl[2] = (UINT8)(espline & 0x00FF);

	//exposure time set
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x15;
	cmdTbl[5] = (UINT8)((espline & 0xFF00) >> 8);
	cmdTbl[6] = 0x00;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = 0x01;
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
	/*i2cRegRead(&REG_ADDR8[0],&REG_ADDR[0],1,2,1,1,0); 
	printf("espline=%d\n",espline);
	printf("read=0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1]);*/
	
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
		
	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (en == FRONT_PV_DYNAMIC_TD_ENABLE) {
		//printf("clk enable\n");
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
		//printf("clk disable\n");
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
	gpioByteFuncSet(GPIO_BYTE_FM2, 0x01, 0xFF);
	gpioByteDirSet(GPIO_BYTE_FM2, 0x01, 0xFF);
	gpioByteOutSet(GPIO_BYTE_FM2, 0x01, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_FM2, 0x01, 0xFF);
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
	UINT16 ltotal, ftotal, espline,compensation_line = 0;
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[3*5];
	UINT16 s_gain;
	UINT16 last_gain ;
	
	/* Before Snap ExposureTime Set */
	frontBeforeSnapExposureTimeSet();
#if 1//0
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

	espline = (pixclk * 10) / (ltotal * frontSnapLastShutter);
	s_gain = frontSnapLastGain;
	//printf("espline1=%d\n",espline);


	last_gain = s_gain;
	//espline = espline;
	
	
	/* dummy line set */
	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x16;
	cmdTbl[2] = (UINT8)(espline & 0x00FF);

	//exposure time set
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x15;
	cmdTbl[5] = (UINT8)((espline & 0xFF00) >> 8);

	
	
	//printf("pre-s_gain=%d\n",s_gain);
	
	//gain set
	cmdTbl[6] = 0x00;
	cmdTbl[7] = 0x18;
	cmdTbl[8] = (UINT8)((last_gain& 0x0700)>>8);
	cmdTbl[9] = 0x00;
	cmdTbl[10] = 0x1d;
	cmdTbl[11] = (UINT8)(last_gain& 0x00FF);
	/**/cmdTbl[12] = 0x00;
	cmdTbl[13] = 0x00;
	cmdTbl[14] = 0x01;

	//printf("re-s_gain=0x%bx\n",cmdTbl[5]);
	
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
#endif
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
	UINT8 compensationGainIdx = 0, lastGainIdx, subGainIdx = 0, pvGainIdxVal;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* get pv gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
	//printf("pvGainIdxVal=%bu\n", pvGainIdxVal);

	/* get compensation gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
	//printf("compensationGainIdx=%bu\n", compensationGainIdx);

	/* calculate last gain */
	lastGainIdx = pvGainIdxVal + compensationGainIdx;
	//printf("lastGainIdx=%bu\n", lastGainIdx);

	#if (SNAP_GAIN_TO_EXP == 1)
	/* gain transfor exposure time */
	if (lastGainIdx >=320) {
		frontSnapLastShutter = frontLastShutter/4;
		subGainIdx = 256;
		lastGainIdx -= subGainIdx;
		//printf("subGainIdx=%bu\n", subGainIdx);
	}
	else if(lastGainIdx >=192){
		frontSnapLastShutter = frontLastShutter/2;
		subGainIdx = 128;
		lastGainIdx -= subGainIdx;
		//printf("subGainIdx=%bu\n", subGainIdx);
	}
		
	else {
		frontSnapLastShutter = frontLastShutter;
	}	
	#else
	frontSnapLastShutter = frontLastShutter;
	#endif
	//printf("frontLastShutter=%lu, frontSnapLastShutter=%lu\n",frontLastShutter,frontSnapLastShutter);

	frontSnapLastGain = pcap->gainTbl[lastGainIdx];	

	#if 1
	/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
	LoadBackfrontLastSetting = 1;
	#endif

	#if (PV_AUTO_OB_BY_GAIN == 1)
	if (pvGainIdxVal == lastGainIdx) {		
		/* resolve ov9712 auto ob by gain issue used */
		if (pvGainIdxVal == pcap->gainMinIdx) {
			LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
		}
		else {//(pvGainIdxVal == pcap->gainMaxIdx)  & other case
			LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
		}

		/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
		LoadBackfrontLastSetting = 1;
	}
	else {
		LoadBackfrontLastGainIdx = pvGainIdxVal;
	}
	//printf("pvGainIdxVal=%bu, LoadBackfrontLastGainIdx=%bu\n", pvGainIdxVal, LoadBackfrontLastGainIdx);
	#else
	LoadBackfrontLastGainIdx = pvGainIdxVal;
	#endif

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

	#if 1
	if(LoadBackfrontLastSetting==1)
	{
		frontExposureTimeSet(0,frontLastShutter,1,0);
		frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
		LoadBackfrontLastSetting = 0;
	}
	#else
	/* power on status */
	if (frontPowerOnStatus == 1) {
	
		/* sensor different resolution or fps switch check */
		if (frontPrevModeSave != frontPrevMode) {//(frameRateSetFlag == 1) {
			sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
			sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
			pv_gId = gIdval;
			//printf("pvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);
			
			if (frontPrevModeSave == 2 && frontPrevMode == 1) {
				//pv_gId += 20;
				//printf("60->30 +20\n");
			}
			else if (frontPrevModeSave == 2 && frontPrevMode == 0) {
				//pv_gId += 26;
				//printf("60->dyn30 +26\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
				pv_gId += 20;
				//printf("60->dyn30 +20\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 2) {
				//pv_gId -= 20;
				//printf("30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 1) {
				pv_gId -= 20;
				//printf("dyn30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 2) {
				//pv_gId -= 26;
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

	i2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	#else
	UINT32 size;
	UINT8 *pData;
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

	
	i2cCmdTableSend(pData, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
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
	sp1kNandRsvSizeGet(fileId, NULL, &size);

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

