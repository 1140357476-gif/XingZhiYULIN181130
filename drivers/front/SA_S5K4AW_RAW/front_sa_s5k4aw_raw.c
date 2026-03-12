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
#include "hal_dram.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */
#define DBG_SNAP_TIMING		0//debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO

#define SNAP_KEEP_FPS		0//add dummy line : 0(reduce fps), keep fps : 1(exposure line equal to frame total-6)
#define PV_AUTO_OB_BY_GAIN	0//pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1
#define SNAP_GAIN_TO_EXP	1//dark noise reduce, sensor gain transfor to exposure time : 1
#define RES_LSC_LOAD		0//after power on load lsc resource file

/* mode. */
#define PREV_MODE_TOT	3
#define SNAP_MODE_TOT	1
#define PREV_RESP		720

/* dimensiion. */
#if (PREV_RESP==720) //720p
//720p, dyn 30->15 fps
#define WIDTH_24FPS_PREV     1280//1280//1024
#define HEIGHT_24FPS_PREV    720//720//960//720
#define XOFF_24FPS_PREV       141
#define YOFF_24FPS_PREV    116//120//1///2
#define LINE_TOT_24FPS_PREV  1420//1200
#define FRAME_TOT_24FPS_PREV  1121//900

//720p, real 30fps
#define WIDTH_30FPS_PREV     640
#define HEIGHT_30FPS_PREV   360
#define XOFF_30FPS_PREV     141//325///4
#define YOFF_30FPS_PREV     56//4///4
#define LINE_TOT_30FPS_PREV 970//1420//960//923//800   
#define FRAME_TOT_30FPS_PREV  825//1121//833//820//600

//480p, real 60fps
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       4
#define YOFF_60FPS_PREV       4
#define LINE_TOT_60FPS_PREV   960//800
#define FRAME_TOT_60FPS_PREV  833//600

#endif

#define WIDTH_SNAP     1280
#define HEIGHT_SNAP    720
#define XOFF_SNAP      141///2
#define YOFF_SNAP     116//120
#define LINE_TOT_SNAP   1420//1200
#define FRAME_TOT_SNAP  1121//900

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

#define MCLK_DIV_SNAP    11
#define MCLK_DIV_SNAP_BURST     11
#define MCLK_DIV_SNAP_BURST_UP  11
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    11
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_24FPS_PREV  24000000L//26692300L
#define PCLK_FREQ_30FPS_PREV  48000000L
#define PCLK_FREQ_60FPS_PREV  48000000L
#define PCLK_FREQ_SNAP        48000000L
#define PCLK_FREQ_SNAP_BURST  	41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */
#else
#define PCLK_FREQ_24FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP        522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			1
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_24FPS_PREV    1
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HEN_60FPS_PREV    0
#else
#define RESHAPE_HEN_24FPS_PREV    1	//bypass h don't enable
#define RESHAPE_HEN_30FPS_PREV    1//0	//bypass h don't enable
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_24FPS_PREV  1
#define RESHAPE_HRISE_24FPS_PREV  2
#define RESHAPE_HFALL_30FPS_PREV  1///2
#define RESHAPE_HRISE_30FPS_PREV  2//16
#define RESHAPE_HFALL_60FPS_PREV  2
#define RESHAPE_HRISE_60FPS_PREV  16
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_24FPS_PREV   0
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_24FPS_PREV    1//1//bypass v don't enable
#define RESHAPE_VEN_30FPS_PREV   1// 0	//bypass v don't enable
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_24FPS_PREV  1
#define RESHAPE_VRISE_24FPS_PREV  2
#define RESHAPE_VFALL_30FPS_PREV  1///2
#define RESHAPE_VRISE_30FPS_PREV  2///3
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP   1
#define RESHAPE_HFALL_SNAP  1
#define RESHAPE_HRISE_SNAP  2
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP 1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP 0

/* bayer pattern. */
#define BAYER_PTN_PREV  1//FRONT_BAYER_PTN_RGRGBB//FRONT_BAYER_PTN_GBBRGR
#define BAYER_PTN_SNAP  1//FRONT_BAYER_PTN_RGRGBB//FRONT_BAYER_PTN_GBBRGR

/* i2c interface. */
#define I2C_DEV_ADDR  0x5A
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_24FPS_24_MAX_IDX  135
#define AE_24FPS_24_MIN_IDX  12
#define AE_24FPS_20_MAX_IDX  136
#define AE_24FPS_20_MIN_IDX  12
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  135
#define AE_30FPS_30_MIN_IDX  12
#define AE_30FPS_25_MAX_IDX  137
#define AE_30FPS_25_MIN_IDX  12
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  116
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  116
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_24FPS_MAX_IDX  241
#define GAIN_24FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  241
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  241
#define GAIN_60FPS_MIN_IDX  0

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor

#define EXP_GAIN_SET_POS AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_OFF				//AE_GAIN_AFTER_EXP_OFF,
																//AE_GAIN_AFTER_EXP_ON

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
/* function type definition. */
#define __STATIC  //static

/* operating function. */
#define frontOpen                     frontSensorOpen
#define frontOpDevNameGet             frontSensorOpDevNameGet
#define frontOpFrameRateSet           frontSensorOpFrameRateSet
#define frontOpCapabilityGet          frontSensorOpCapabilityGet
#define frontOpIntrCapabilityGet      frontSensorOpIntrCapabilityGet
#define frontOpPowerOn                frontSensorOpPowerOn
#define frontOpPowerOff               frontSensorOpPowerOff
#define frontOpPreviewModeSet         frontSensorOpPreviewModeSet
#define frontOpSnapModeSet            frontSensorOpSnapModeSet
#define frontOpSnapShot               frontSensorOpSnapShot
#define frontOpGainSet                frontSensorOpGainSet
#define frontOpExposureTimeSet        frontSensorOpExposureTimeSet
#define frontOpIntrGainSet            frontSensorOpIntrGainSet
#define frontOpIntrExposureTimeSet    frontSensorOpIntrExposureTimeSet
#define frontOpOpticalBlackStatusGet  frontSensorOpOpticalBlackStatusGet
#define frontOpOpticalBlackCalibrate  frontSensorOpOpticalBlackCalibrate
#define frontOpShutterOpen            frontSensorOpShutterOpen
#define frontOpShutterClose           frontSensorOpShutterClose

/* extern operating function. */
#define frontOpAeTargetLuminanceSet       frontSensorOpAeTargetLuminanceSet
#define frontOpAeExposureCompensationSet  frontSensorOpAeExposureCompensationSet
#define frontOpAeFrameRateSet             frontSensorOpAeFrameRateSet
#define frontOpAwbModeSet                 frontSensorOpAwbModeSet
#define frontOpAfterSnapShot              frontSensorOpAfterSnapShot

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
static void frontSnapExposureTimeSet(UINT8 mode);
static void frontBeforeSnapExposureTimeSet(void);
static void frontResTblBinRead(UINT16 fileId,	UINT16 address);
static void frontResCmdBinSend(UINT16 fileId);


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - SA S5K4AW RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table
 *					CvtTypeLnkAddr: convert to 
 **************************************************************************/

#define CVT_DEF  extern

/* Tag for convert start */
CVT_DEF UINT8 TAG_CVT_S;

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
	_frontInit2CmdTbl		= 0x00CD,
	_frontInit3CmdTbl		= 0x00CE,
	_frontInit4CmdTbl		= 0x00CF,
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


/* ae table. */
/********************************/
/*             banding calculate                  */
/*  50Hz : 1/(shutter/10) < 1/(2*50)       */ 
/*  60Hz : 1/(shutter/10) < 1/(2*60)       */ 
/*                                                      */
/********************************/

#if 1
static code frontAe_t ae20fps50hzTbl[141] = {
	{	0,	332,	235,	0,	0 },
	{	1,	332,	218,	0,	0 },
	{	2,	332,	202,	0,	0 },
	{	3,	332,	188,	0,	0 },
	{	4,	332,	174,	0,	0 },
	{	5,	332,	161,	0,	0 },
	{	6,	332,	149,	0,	0 },
	{	7,	332,	138,	0,	0 },
	{	8,	332,	128,	0,	0 },
	{	9,	332,	118,	0,	0 },
	{	10,	332,	110,	0,	0 },
	{	11,	332,	101,	0,	0 },
	{	12,	332,	93,	0,	0 },
	{	13,	332,	86,	0,	0 },
	{	14,	332,	79,	0,	0 },
	{	15,	332,	73,	0,	0 },
	{	16,	332,	67,	0,	0 },
	{	17,	332,	61,	0,	0 },
	{	18,	332,	56,	0,	0 },
	{	19,	332,	51,	0,	0 },
	{	20,	332,	47,	0,	0 },
	{	21,	332,	43,	0,	0 },
	{	22,	332,	39,	0,	0 },
	{	23,	332,	35,	0,	0 },
	{	24,	332,	32,	0,	0 },
	{	25,	332,	29,	0,	0 },
	{	26,	332,	26,	0,	0 },
	{	27,	332,	23,	0,	0 },
	{	28,	332,	20,	0,	0 },
	{	29,	332,	18,	0,	0 },
	{	30,	332,	16,	0,	0 },
	{	31,	332,	14,	0,	0 },
	{	32,	332,	12,	0,	0 },
	{	33,	332,	10,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	7,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	4,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	498,	6,	0,	0 },
	{	42,	498,	5,	0,	0 },
	{	43,	498,	4,	0,	0 },
	{	44,	498,	2,	0,	0 },
	{	45,	498,	1,	0,	0 },
	{	46,	498,	0,	0,	0 },
	{	47,	991,	13,	0,	0 },
	{	48,	991,	11,	0,	0 },
	{	49,	991,	9,	0,	0 },
	{	50,	991,	8,	0,	0 },
	{	51,	991,	6,	0,	0 },
	{	52,	991,	5,	0,	0 },
	{	53,	991,	3,	0,	0 },
	{	54,	991,	2,	0,	0 },
	{	55,	991,	1,	0,	0 },
	{	56,	991,	0,	0,	0 },
	{	57,	1043,	0,	0,	0 },
	{	58,	1119,	0,	0,	0 },
	{	59,	1199,	0,	0,	0 },
	{	60,	1285,	0,	0,	0 },
	{	61,	1374,	0,	0,	0 },
	{	62,	1476,	0,	0,	0 },
	{	63,	1580,	0,	0,	0 },
	{	64,	1699,	0,	0,	0 },
	{	65,	1817,	0,	0,	0 },
	{	66,	1954,	0,	0,	0 },
	{	67,	2087,	0,	0,	0 },
	{	68,	2239,	0,	0,	0 },
	{	69,	2397,	0,	0,	0 },
	{	70,	2580,	0,	0,	0 },
	{	71,	2748,	0,	0,	0 },
	{	72,	2965,	0,	0,	0 },
	{	73,	3159,	0,	0,	0 },
	{	74,	3414,	0,	0,	0 },
	{	75,	3635,	0,	0,	0 },
	{	76,	3931,	0,	0,	0 },
	{	77,	4173,	0,	0,	0 },
	{	78,	4507,	0,	0,	0 },
	{	79,	4829,	0,	0,	0 },
	{	80,	5200,	0,	0,	0 },
	{	81,	5541,	0,	0,	0 },
	{	82,	5930,	0,	0,	0 },
	{	83,	6378,	0,	0,	0 },
	{	84,	6899,	0,	0,	0 },
	{	85,	7348,	0,	0,	0 },
	{	86,	7861,	0,	0,	0 },
	{	87,	8451,	0,	0,	0 },
	{	88,	9136,	0,	0,	0 },
	{	89,	9658,	0,	0,	0 },
	{	90,	10563,	0,	0,	0 },
	{	91,	11268,	0,	0,	1 },
	{	92,	12072,	0,	0,	0 },
	{	93,	13001,	0,	0,	0 },
	{	94,	14084,	0,	0,	0 },
	{	95,	14697,	0,	0,	1 },
	{	96,	16097,	0,	0,	1 },
	{	97,	16901,	0,	0,	0 },
	{	98,	18779,	0,	0,	0 },
	{	99,	19884,	0,	0,	0 },
	{	100,	21127,	0,	0,	1 },
	{	101,	22535,	0,	0,	0 },
	{	102,	24145,	0,	0,	1 },
	{	103,	26002,	0,	0,	0 },
	{	104,	28169,	0,	0,	0 },
	{	105,	30730,	0,	0,	2 },
	{	106,	33802,	1,	0,	0 },
	{	107,	33802,	0,	0,	0 },
	{	108,	37558,	0,	0,	0 },
	{	109,	42253,	1,	0,	0 },
	{	110,	42253,	0,	0,	0 },
	{	111,	48289,	1,	0,	0 },
	{	112,	48289,	0,	0,	0 },
	{	113,	56337,	1,	0,	0 },
	{	114,	56337,	0,	0,	0 },
	{	115,	67604,	2,	0,	0 },
	{	116,	67604,	1,	0,	0 },
	{	117,	67604,	0,	0,	0 },
	{	118,	84505,	2,	0,	0 },
	{	119,	84505,	1,	0,	0 },
	{	120,	84505,	0,	0,	0 },
	{	121,	112672,	4,	0,	0 },
	{	122,	112672,	3,	0,	0 },
	{	123,	112672,	1,	0,	0 },
	{	124,	112672,	0,	0,	0 },
	{	125,	169006,	6,	0,	0 },
	{	126,	169006,	5,	0,	0 },
	{	127,	169006,	4,	0,	0 },
	{	128,	169006,	2,	0,	0 },
	{	129,	169006,	1,	0,	0 },
	{	130,	169006,	0,	0,	0 },
	{	131,	337994,	13,	0,	0 },
	{	132,	337994,	12,	0,	0 },
	{	133,	337994,	10,	0,	0 },
	{	134,	337994,	8,	0,	0 },
	{	135,	337994,	6,	0,	0 },
	{	136,	337994,	5,	0,	0 },
	{	137,	337994,	4,	0,	0 },
	{	138,	337994,	2,	0,	0 },
	{	139,	337994,	1,	0,	0 },
	{	140,	337994,	0,	0,	0 },
	};

static code frontAe_t ae20fps60hzTbl[142] = {
	{	0,	299,	226,	0,	0 },
	{	1,	299,	210,	0,	0 },
	{	2,	299,	195,	0,	0 },
	{	3,	299,	181,	0,	0 },
	{	4,	299,	167,	0,	0 },
	{	5,	299,	155,	0,	0 },
	{	6,	299,	144,	0,	0 },
	{	7,	299,	133,	0,	0 },
	{	8,	299,	123,	0,	0 },
	{	9,	299,	114,	0,	0 },
	{	10,	299,	105,	0,	0 },
	{	11,	299,	97,	0,	0 },
	{	12,	299,	90,	0,	0 },
	{	13,	299,	83,	0,	0 },
	{	14,	299,	76,	0,	0 },
	{	15,	299,	70,	0,	0 },
	{	16,	299,	64,	0,	0 },
	{	17,	299,	59,	0,	0 },
	{	18,	299,	54,	0,	0 },
	{	19,	299,	49,	0,	0 },
	{	20,	299,	45,	0,	0 },
	{	21,	299,	41,	0,	0 },
	{	22,	299,	37,	0,	0 },
	{	23,	299,	34,	0,	0 },
	{	24,	299,	30,	0,	0 },
	{	25,	299,	27,	0,	0 },
	{	26,	299,	24,	0,	0 },
	{	27,	299,	22,	0,	0 },
	{	28,	299,	19,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	15,	0,	0 },
	{	31,	299,	13,	0,	0 },
	{	32,	299,	11,	0,	0 },
	{	33,	299,	9,	0,	0 },
	{	34,	299,	7,	0,	0 },
	{	35,	299,	6,	0,	0 },
	{	36,	299,	4,	0,	0 },
	{	37,	299,	3,	0,	0 },
	{	38,	299,	2,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	399,	3,	0,	0 },
	{	42,	399,	2,	0,	0 },
	{	43,	399,	1,	0,	0 },
	{	44,	399,	0,	0,	0 },
	{	45,	597,	6,	0,	0 },
	{	46,	597,	4,	0,	0 },
	{	47,	597,	3,	0,	0 },
	{	48,	597,	2,	0,	0 },
	{	49,	597,	1,	0,	0 },
	{	50,	598,	0,	0,	0 },
	{	51,	1190,	12,	0,	0 },
	{	52,	1190,	11,	0,	0 },
	{	53,	1190,	9,	0,	0 },
	{	54,	1190,	7,	0,	0 },
	{	55,	1190,	6,	0,	0 },
	{	56,	1190,	4,	0,	0 },
	{	57,	1190,	3,	0,	0 },
	{	58,	1190,	2,	0,	0 },
	{	59,	1190,	1,	0,	0 },
	{	60,	1199,	0,	0,	0 },
	{	61,	1285,	0,	0,	0 },
	{	62,	1374,	0,	0,	0 },
	{	63,	1476,	0,	0,	0 },
	{	64,	1580,	0,	0,	0 },
	{	65,	1699,	0,	0,	0 },
	{	66,	1817,	0,	0,	0 },
	{	67,	1954,	0,	0,	0 },
	{	68,	2087,	0,	0,	0 },
	{	69,	2239,	0,	0,	0 },
	{	70,	2397,	0,	0,	0 },
	{	71,	2580,	0,	0,	0 },
	{	72,	2748,	0,	0,	0 },
	{	73,	2965,	0,	0,	0 },
	{	74,	3159,	0,	0,	0 },
	{	75,	3414,	0,	0,	0 },
	{	76,	3635,	0,	0,	0 },
	{	77,	3931,	0,	0,	0 },
	{	78,	4173,	0,	0,	0 },
	{	79,	4507,	0,	0,	0 },
	{	80,	4829,	0,	0,	0 },
	{	81,	5200,	0,	0,	0 },
	{	82,	5541,	0,	0,	0 },
	{	83,	5930,	0,	0,	0 },
	{	84,	6378,	0,	0,	0 },
	{	85,	6899,	0,	0,	0 },
	{	86,	7348,	0,	0,	0 },
	{	87,	7861,	0,	0,	0 },
	{	88,	8451,	0,	0,	0 },
	{	89,	9136,	0,	0,	0 },
	{	90,	9658,	0,	0,	0 },
	{	91,	10563,	0,	0,	0 },
	{	92,	11268,	0,	0,	1 },
	{	93,	12072,	0,	0,	0 },
	{	94,	13001,	0,	0,	0 },
	{	95,	14084,	0,	0,	0 },
	{	96,	14697,	0,	0,	1 },
	{	97,	16097,	0,	0,	1 },
	{	98,	16901,	0,	0,	0 },
	{	99,	18779,	0,	0,	0 },
	{	100,	19884,	0,	0,	0 },
	{	101,	21127,	0,	0,	1 },
	{	102,	22535,	0,	0,	0 },
	{	103,	24145,	0,	0,	1 },
	{	104,	26002,	0,	0,	0 },
	{	105,	28169,	0,	0,	0 },
	{	106,	30730,	0,	0,	2 },
	{	107,	33802,	1,	0,	0 },
	{	108,	33802,	0,	0,	0 },
	{	109,	37558,	0,	0,	0 },
	{	110,	42253,	1,	0,	0 },
	{	111,	42253,	0,	0,	0 },
	{	112,	48289,	1,	0,	0 },
	{	113,	48289,	0,	0,	0 },
	{	114,	56337,	1,	0,	0 },
	{	115,	56337,	0,	0,	0 },
	{	116,	67604,	2,	0,	0 },
	{	117,	67604,	1,	0,	0 },
	{	118,	67604,	0,	0,	0 },
	{	119,	84505,	2,	0,	0 },
	{	120,	84505,	1,	0,	0 },
	{	121,	84505,	0,	0,	0 },
	{	122,	112672,	4,	0,	0 },
	{	123,	112672,	3,	0,	0 },
	{	124,	112672,	1,	0,	0 },
	{	125,	112672,	0,	0,	0 },
	{	126,	169006,	6,	0,	0 },
	{	127,	169006,	5,	0,	0 },
	{	128,	169006,	4,	0,	0 },
	{	129,	169006,	2,	0,	0 },
	{	130,	169006,	1,	0,	0 },
	{	131,	169006,	0,	0,	0 },
	{	132,	337994,	13,	0,	0 },
	{	133,	337994,	12,	0,	0 },
	{	134,	337994,	10,	0,	0 },
	{	135,	337994,	8,	0,	0 },
	{	136,	337994,	6,	0,	0 },
	{	137,	337994,	5,	0,	0 },
	{	138,	337994,	4,	0,	0 },
	{	139,	337994,	2,	0,	0 },
	{	140,	337994,	1,	0,	0 },
	{	141,	337994,	0,	0,	0 },
	};

#endif


#if 1
static code frontAe_t ae30fps50hzTbl[141] = {
	{	0,	332,	235,	0,	0 },
	{	1,	332,	218,	0,	0 },
	{	2,	332,	202,	0,	0 },
	{	3,	332,	188,	0,	0 },
	{	4,	332,	174,	0,	0 },
	{	5,	332,	161,	0,	0 },
	{	6,	332,	149,	0,	0 },
	{	7,	332,	138,	0,	0 },
	{	8,	332,	128,	0,	0 },
	{	9,	332,	118,	0,	0 },
	{	10,	332,	110,	0,	0 },
	{	11,	332,	101,	0,	0 },
	{	12,	332,	93,	0,	0 },
	{	13,	332,	86,	0,	0 },
	{	14,	332,	79,	0,	0 },
	{	15,	332,	73,	0,	0 },
	{	16,	332,	67,	0,	0 },
	{	17,	332,	61,	0,	0 },
	{	18,	332,	56,	0,	0 },
	{	19,	332,	51,	0,	0 },
	{	20,	332,	47,	0,	0 },
	{	21,	332,	43,	0,	0 },
	{	22,	332,	39,	0,	0 },
	{	23,	332,	35,	0,	0 },
	{	24,	332,	32,	0,	0 },
	{	25,	332,	29,	0,	0 },
	{	26,	332,	26,	0,	0 },
	{	27,	332,	23,	0,	0 },
	{	28,	332,	20,	0,	0 },
	{	29,	332,	18,	0,	0 },
	{	30,	332,	16,	0,	0 },
	{	31,	332,	14,	0,	0 },
	{	32,	332,	12,	0,	0 },
	{	33,	332,	10,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	7,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	4,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	498,	6,	0,	0 },
	{	42,	498,	5,	0,	0 },
	{	43,	498,	4,	0,	0 },
	{	44,	498,	2,	0,	0 },
	{	45,	498,	1,	0,	0 },
	{	46,	498,	0,	0,	0 },
	{	47,	991,	13,	0,	0 },
	{	48,	991,	11,	0,	0 },
	{	49,	991,	9,	0,	0 },
	{	50,	991,	8,	0,	0 },
	{	51,	991,	6,	0,	0 },
	{	52,	991,	5,	0,	0 },
	{	53,	991,	3,	0,	0 },
	{	54,	991,	2,	0,	0 },
	{	55,	991,	1,	0,	0 },
	{	56,	991,	0,	0,	0 },
	{	57,	1043,	0,	0,	0 },
	{	58,	1119,	0,	0,	0 },
	{	59,	1199,	0,	0,	0 },
	{	60,	1285,	0,	0,	0 },
	{	61,	1374,	0,	0,	0 },
	{	62,	1476,	0,	0,	0 },
	{	63,	1580,	0,	0,	0 },
	{	64,	1699,	0,	0,	0 },
	{	65,	1817,	0,	0,	0 },
	{	66,	1954,	0,	0,	0 },
	{	67,	2087,	0,	0,	0 },
	{	68,	2239,	0,	0,	0 },
	{	69,	2397,	0,	0,	0 },
	{	70,	2580,	0,	0,	0 },
	{	71,	2748,	0,	0,	0 },
	{	72,	2965,	0,	0,	0 },
	{	73,	3159,	0,	0,	0 },
	{	74,	3414,	0,	0,	0 },
	{	75,	3635,	0,	0,	0 },
	{	76,	3931,	0,	0,	0 },
	{	77,	4173,	0,	0,	0 },
	{	78,	4507,	0,	0,	0 },
	{	79,	4829,	0,	0,	0 },
	{	80,	5200,	0,	0,	0 },
	{	81,	5541,	0,	0,	0 },
	{	82,	5930,	0,	0,	0 },
	{	83,	6378,	0,	0,	0 },
	{	84,	6899,	0,	0,	0 },
	{	85,	7348,	0,	0,	0 },
	{	86,	7861,	0,	0,	0 },
	{	87,	8451,	0,	0,	0 },
	{	88,	9136,	0,	0,	0 },
	{	89,	9658,	0,	0,	0 },
	{	90,	10563,	0,	0,	0 },
	{	91,	11268,	0,	0,	1 },
	{	92,	12072,	0,	0,	0 },
	{	93,	13001,	0,	0,	0 },
	{	94,	14084,	0,	0,	0 },
	{	95,	14697,	0,	0,	1 },
	{	96,	16097,	0,	0,	1 },
	{	97,	16901,	0,	0,	0 },
	{	98,	18779,	0,	0,	0 },
	{	99,	19884,	0,	0,	0 },
	{	100,	21127,	0,	0,	1 },
	{	101,	22535,	0,	0,	0 },
	{	102,	24145,	0,	0,	1 },
	{	103,	26002,	0,	0,	0 },
	{	104,	28169,	0,	0,	0 },
	{	105,	30730,	0,	0,	2 },
	{	106,	33802,	1,	0,	0 },
	{	107,	33802,	0,	0,	0 },
	{	108,	37558,	0,	0,	0 },
	{	109,	42253,	1,	0,	0 },
	{	110,	42253,	0,	0,	0 },
	{	111,	48289,	1,	0,	0 },
	{	112,	48289,	0,	0,	0 },
	{	113,	56337,	1,	0,	0 },
	{	114,	56337,	0,	0,	0 },
	{	115,	67604,	2,	0,	0 },
	{	116,	67604,	1,	0,	0 },
	{	117,	67604,	0,	0,	0 },
	{	118,	84505,	2,	0,	0 },
	{	119,	84505,	1,	0,	0 },
	{	120,	84505,	0,	0,	0 },
	{	121,	112672,	4,	0,	0 },
	{	122,	112672,	3,	0,	0 },
	{	123,	112672,	1,	0,	0 },
	{	124,	112672,	0,	0,	0 },
	{	125,	169006,	6,	0,	0 },
	{	126,	169006,	5,	0,	0 },
	{	127,	169006,	4,	0,	0 },
	{	128,	169006,	2,	0,	0 },
	{	129,	169006,	1,	0,	0 },
	{	130,	169006,	0,	0,	0 },
	{	131,	337994,	13,	0,	0 },
	{	132,	337994,	12,	0,	0 },
	{	133,	337994,	10,	0,	0 },
	{	134,	337994,	8,	0,	0 },
	{	135,	337994,	6,	0,	0 },
	{	136,	337994,	5,	0,	0 },
	{	137,	337994,	4,	0,	0 },
	{	138,	337994,	2,	0,	0 },
	{	139,	337994,	1,	0,	0 },
	{	140,	337994,	0,	0,	0 },
	};

static code frontAe_t ae30fps60hzTbl[142] = {
	{	0,	299,	226,	0,	0 },
	{	1,	299,	210,	0,	0 },
	{	2,	299,	195,	0,	0 },
	{	3,	299,	181,	0,	0 },
	{	4,	299,	167,	0,	0 },
	{	5,	299,	155,	0,	0 },
	{	6,	299,	144,	0,	0 },
	{	7,	299,	133,	0,	0 },
	{	8,	299,	123,	0,	0 },
	{	9,	299,	114,	0,	0 },
	{	10,	299,	105,	0,	0 },
	{	11,	299,	97,	0,	0 },
	{	12,	299,	90,	0,	0 },
	{	13,	299,	83,	0,	0 },
	{	14,	299,	76,	0,	0 },
	{	15,	299,	70,	0,	0 },
	{	16,	299,	64,	0,	0 },
	{	17,	299,	59,	0,	0 },
	{	18,	299,	54,	0,	0 },
	{	19,	299,	49,	0,	0 },
	{	20,	299,	45,	0,	0 },
	{	21,	299,	41,	0,	0 },
	{	22,	299,	37,	0,	0 },
	{	23,	299,	34,	0,	0 },
	{	24,	299,	30,	0,	0 },
	{	25,	299,	27,	0,	0 },
	{	26,	299,	24,	0,	0 },
	{	27,	299,	22,	0,	0 },
	{	28,	299,	19,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	15,	0,	0 },
	{	31,	299,	13,	0,	0 },
	{	32,	299,	11,	0,	0 },
	{	33,	299,	9,	0,	0 },
	{	34,	299,	7,	0,	0 },
	{	35,	299,	6,	0,	0 },
	{	36,	299,	4,	0,	0 },
	{	37,	299,	3,	0,	0 },
	{	38,	299,	2,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	399,	3,	0,	0 },
	{	42,	399,	2,	0,	0 },
	{	43,	399,	1,	0,	0 },
	{	44,	399,	0,	0,	0 },
	{	45,	597,	6,	0,	0 },
	{	46,	597,	4,	0,	0 },
	{	47,	597,	3,	0,	0 },
	{	48,	597,	2,	0,	0 },
	{	49,	597,	1,	0,	0 },
	{	50,	598,	0,	0,	0 },
	{	51,	1190,	12,	0,	0 },
	{	52,	1190,	11,	0,	0 },
	{	53,	1190,	9,	0,	0 },
	{	54,	1190,	7,	0,	0 },
	{	55,	1190,	6,	0,	0 },
	{	56,	1190,	4,	0,	0 },
	{	57,	1190,	3,	0,	0 },
	{	58,	1190,	2,	0,	0 },
	{	59,	1190,	1,	0,	0 },
	{	60,	1199,	0,	0,	0 },
	{	61,	1285,	0,	0,	0 },
	{	62,	1374,	0,	0,	0 },
	{	63,	1476,	0,	0,	0 },
	{	64,	1580,	0,	0,	0 },
	{	65,	1699,	0,	0,	0 },
	{	66,	1817,	0,	0,	0 },
	{	67,	1954,	0,	0,	0 },
	{	68,	2087,	0,	0,	0 },
	{	69,	2239,	0,	0,	0 },
	{	70,	2397,	0,	0,	0 },
	{	71,	2580,	0,	0,	0 },
	{	72,	2748,	0,	0,	0 },
	{	73,	2965,	0,	0,	0 },
	{	74,	3159,	0,	0,	0 },
	{	75,	3414,	0,	0,	0 },
	{	76,	3635,	0,	0,	0 },
	{	77,	3931,	0,	0,	0 },
	{	78,	4173,	0,	0,	0 },
	{	79,	4507,	0,	0,	0 },
	{	80,	4829,	0,	0,	0 },
	{	81,	5200,	0,	0,	0 },
	{	82,	5541,	0,	0,	0 },
	{	83,	5930,	0,	0,	0 },
	{	84,	6378,	0,	0,	0 },
	{	85,	6899,	0,	0,	0 },
	{	86,	7348,	0,	0,	0 },
	{	87,	7861,	0,	0,	0 },
	{	88,	8451,	0,	0,	0 },
	{	89,	9136,	0,	0,	0 },
	{	90,	9658,	0,	0,	0 },
	{	91,	10563,	0,	0,	0 },
	{	92,	11268,	0,	0,	1 },
	{	93,	12072,	0,	0,	0 },
	{	94,	13001,	0,	0,	0 },
	{	95,	14084,	0,	0,	0 },
	{	96,	14697,	0,	0,	1 },
	{	97,	16097,	0,	0,	1 },
	{	98,	16901,	0,	0,	0 },
	{	99,	18779,	0,	0,	0 },
	{	100,	19884,	0,	0,	0 },
	{	101,	21127,	0,	0,	1 },
	{	102,	22535,	0,	0,	0 },
	{	103,	24145,	0,	0,	1 },
	{	104,	26002,	0,	0,	0 },
	{	105,	28169,	0,	0,	0 },
	{	106,	30730,	0,	0,	2 },
	{	107,	33802,	1,	0,	0 },
	{	108,	33802,	0,	0,	0 },
	{	109,	37558,	0,	0,	0 },
	{	110,	42253,	1,	0,	0 },
	{	111,	42253,	0,	0,	0 },
	{	112,	48289,	1,	0,	0 },
	{	113,	48289,	0,	0,	0 },
	{	114,	56337,	1,	0,	0 },
	{	115,	56337,	0,	0,	0 },
	{	116,	67604,	2,	0,	0 },
	{	117,	67604,	1,	0,	0 },
	{	118,	67604,	0,	0,	0 },
	{	119,	84505,	2,	0,	0 },
	{	120,	84505,	1,	0,	0 },
	{	121,	84505,	0,	0,	0 },
	{	122,	112672,	4,	0,	0 },
	{	123,	112672,	3,	0,	0 },
	{	124,	112672,	1,	0,	0 },
	{	125,	112672,	0,	0,	0 },
	{	126,	169006,	6,	0,	0 },
	{	127,	169006,	5,	0,	0 },
	{	128,	169006,	4,	0,	0 },
	{	129,	169006,	2,	0,	0 },
	{	130,	169006,	1,	0,	0 },
	{	131,	169006,	0,	0,	0 },
	{	132,	337994,	13,	0,	0 },
	{	133,	337994,	12,	0,	0 },
	{	134,	337994,	10,	0,	0 },
	{	135,	337994,	8,	0,	0 },
	{	136,	337994,	6,	0,	0 },
	{	137,	337994,	5,	0,	0 },
	{	138,	337994,	4,	0,	0 },
	{	139,	337994,	2,	0,	0 },
	{	140,	337994,	1,	0,	0 },
	{	141,	337994,	0,	0,	0 },
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

code UINT16 gainTbl[241] = {
  256,   272,   288,   304,   320,   336,   352,   368,   384,   400, 
  416,   432,   448,   464,   480,   496,   512,   528,   544,   560, 
  576,   592,   608,   624,   640,   656,   672,   688,   704,   720, 
  736,   752,   768,   784,   800,   816,   832,   848,   864,   880, 
  896,   912,   928,   944,   960,   976,   992,   1008,   1024,   1040, 
  1056,   1072,   1088,   1104,   1120,   1136,   1152,   1168,   1184,   1200, 
  1216,   1232,   1248,   1264,   1280,   1296,   1312,   1328,   1344,   1360, 
  1376,   1392,   1408,   1424,   1440,   1456,   1472,   1488,   1504,   1520, 
  1536,   1552,   1568,   1584,   1600,   1616,   1632,   1648,   1664,   1680, 
  1696,   1712,   1728,   1744,   1760,   1776,   1792,   1808,   1824,   1840, 
  1856,   1872,   1888,   1904,   1920,   1936,   1952,   1968,   1984,   2000, 
  2016,   2032,   2048,   2064,   2080,   2096,   2112,   2128,   2144,   2160, 
  2176,   2192,   2208,   2224,   2240,   2256,   2272,   2288,   2304,   2320, 
  2336,   2352,   2368,   2384,   2400,   2416,   2432,   2448,   2464,   2480, 
  2496,   2512,   2528,   2544,   2560,   2576,   2592,   2608,   2624,   2640, 
  2656,   2672,   2688,   2704,   2720,   2736,   2752,   2768,   2784,   2800, 
  2816,   2832,   2848,   2864,   2880,   2896,   2912,   2928,   2944,   2960, 
  2976,   2992,   3008,   3024,   3040,   3056,   3072,   3088,   3104,   3120, 
  3136,   3152,   3168,   3184,   3200,   3216,   3232,   3248,   3264,   3280, 
  3296,   3312,   3328,   3344,   3360,   3376,   3392,   3408,   3424,   3440, 
  3456,   3472,   3488,   3504,   3520,   3536,   3552,   3568,   3584,   3600, 
  3616,   3632,   3648,   3664,   3680,   3696,   3712,   3728,   3744,   3760, 
  3776,   3792,   3808,   3824,   3840,   3856,   3872,   3888,   3904,   3920, 
  3936,   3952,   3968,   3984,   4000,   4016,   4032,   4048,   4064,   4080, 
  4096, 	};

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
0xFC,0xFC,0xD0,0x00,
0x00,0x10,0x00,0x01,	// Reset
0x10,0x30,0x00,0x00,	// Clear host interrupt so main will wait
0x00,0x14,0x00,0x01,	// ARM go                                       
};

static code UINT8 frontInit1CmdTbl[] = {
0xF4,0x18,0x00,0x07, 	//ATOP sram internal delay
0x00,0x28,0x70,0x00,
0x00,0x2A,0x0F,0xE0,	//#senHal_Again1_Vms
0x0F,0x12,0x00,0x1E,
0x00,0x2A,0x0F,0xC4,	//#senHal_usAigAnaBoost
0x0F,0x12,0x64,0x47,
0x00,0x2A,0x0F,0xFC,	//#gisp_rnc_nActivePedestal
0x0F,0x12,0x00,0x02,
0x00,0x2A,0x0F,0xC8,	//#senHal_usAigRef
0x0F,0x12,0x01,0x2E,
0x00,0x2A,0x0F,0xD0,	//#senHal_usAigRampStart
0x0F,0x12,0x00,0x30,
0x00,0x2A,0x0f,0xee,	//#senHal_OptimalResetAvAg1
0x0F,0x12,0x02,0x8A,
0x00,0x2A,0x10,0x04,	//#gisp_msm_OffsetNoBin[0]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x06,	//#gisp_msm_OffsetNoBin[1]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x08,	//#gisp_msm_OffsetNoBin[2]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x0a,	//#gisp_msm_OffsetNoBin[3]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x00,	//#gisp_msm_OffsetNoBin[4]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x0E,	//#gisp_msm_OffsetNoBin[5]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x10,	//#gisp_msm_OffsetNoBin[6]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x12,	//#gisp_msm_OffsetNoBin[7]
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x10,0x1C,	//#gisp_msm_uGainNoBin[0]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x1E,	//#gisp_msm_uGainNoBin[1]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x20,	//#gisp_msm_uGainNoBin[2]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x22,	//#gisp_msm_uGainNoBin[3]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x24,	//#gisp_msm_uGainNoBin[4]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x26,	//#gisp_msm_uGainNoBin[5]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x28,	//#gisp_msm_uGainNoBin[6]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x2A,	//#gisp_msm_uGainNoBin[7]
0x0F,0x12,0x10,0x00,	
0x00,0x2A,0x10,0x76,	//#gisp_gos_Pedestal
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x0A,0x3A,	//#setot_bForceMinAgain
0x0F,0x12,0x00,0x00,
//= TUNE Gains ===========
0x00,0x2A,0x04,0x44,	//#lt_uMaxAnGain1 
0x0F,0x12,0x02,0x00,  
0x00,0x2A,0x04,0x46,	//#lt_uMaxAnGain2
0x0F,0x12,0x02,0x00,	
0x00,0x2A,0x04,0x48,	//#lt_uMaxDigGain
0x0F,0x12,0x10,0x00,
0x00,0x2A,0x04,0x34,	//#lt_uMaxExp1 
0x0F,0x12,0x68,0xB0,	// 68B0 67ms; 3390 33ms
0x00,0x2A,0x04,0x38,	//#lt_uMaxExp2 
0x0F,0x12,0x68,0xB0,	// 68B0 67ms; 3390 33ms
0x00,0x2A,0x04,0x4A,	//#lt_uMaxTotGain
0x0F,0x12,0x10,0x00,
0x10,0x00,0x00,0x01,	// Set host interrupt so main start run
};

static code UINT8 frontInit2CmdTbl[] = {
//====PLL CLK SETTING ==========================
0x00,0x28,0x70,0x00,
0x00,0x2A,0x01,0xba,	//#REG_TC_IPRM_InClockLSBs	//input=24MHz
0x0F,0x12,0x5D,0xC0,
0x0F,0x12,0x00,0x00,
0x00,0x2A,0x01,0xc2,	//#REG_TC_IPRM_UseNPviClocks	
0x0F,0x12,0x00,0x02,
0x00,0x2A,0x01,0xc8,	//#REG_TC_IPRM_OpClk4KHz_0	
0x0F,0x12,0x2E,0xE0,
0x0F,0x12,0x2E,0x00,	//#REG_TC_IPRM_MinOutRate4KHz_0
0x0F,0x12,0x2E,0xE0,	//#REG_TC_IPRM_MaxOutRate4KHz_0
0x0F,0x12,0x2E,0xE0,
0x0F,0x12,0x17,0x00,	//#REG_TC_IPRM_MinOutRate4KHz_0
0x0F,0x12,0x17,0x70,	//#REG_TC_IPRM_MaxOutRate4KHz_0
0x00,0x2A,0x01,0xdc,	//#REG_TC_IPRM_InitParamsUpdated
0x0F,0x12,0x00,0x01,
};
static code UINT8 frontInit3CmdTbl[] = {
//====PREVIEW CONFIGURATION 0 ==========================
0x00,0x2A,0x02,0x3c,	
0x0F,0x12,0x02,0x80,	//#REG_0TC_PCFG_usWidth	
0x0F,0x12,0x01,0xe0,	//#REG_0TC_PCFG_usHeight	
0x0F,0x12,0x00,0x07,	//#REG_0TC_PCFG_Format	
0x0F,0x12,0x17,0x70,	//#REG_0TC_PCFG_usMaxOut4KHzRate
0x0F,0x12,0x17,0x00,	//#REG_0TC_PCFG_usMinOut4KHzRate
0x0F,0x12,0x00,0x42,	//#REG_0TC_PCFG_PVIMask
0x0F,0x12,0x00,0x01,	//#REG_0TC_PCFG_uClockInd 
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_usFrTimeType
0x0F,0x12,0x00,0x01,	//#REG_0TC_PCFG_FrRateQualityType
0x0F,0x12,0x09,0xC4,	//#REG_0TC_PCFG_usMaxFrTimeMsecMult10
0x0F,0x12,0x01,0x4d,	//#REG_0TC_PCFG_usMinFrTimeMsecMult10
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_bSmearoutput
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_Satuation
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_SharpBlur
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_Glamour
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_ColorTemp
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_Cammaindex
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_PreMirror
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_CaptureMirror
0x0F,0x12,0x00,0x00,	//#REG_0TC_PCFG_Rotation
//====PREVIEW CONFIGURATION 1 ==========================
0x00,0x2A,0x02,0x64,	
0x0F,0x12,0x05,0x00,	//#REG_1TC_PCFG_usWidth	
0x0F,0x12,0x03,0xc0,	//#REG_1TC_PCFG_usHeight	
0x0F,0x12,0x00,0x07,	//#REG_1TC_PCFG_Format	
0x0F,0x12,0x2E,0xE0,	//#REG_1TC_PCFG_usMaxOut4KHzRate
0x0F,0x12,0x2E,0x00,	//#REG_1TC_PCFG_usMinOut4KHzRate
0x0F,0x12,0x00,0x42,	//#REG_1TC_PCFG_PVIMask
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_uClockInd 
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_usFrTimeType
0x0F,0x12,0x00,0x02,	//#REG_1TC_PCFG_FrRateQualityType
0x0F,0x12,0x09,0xC4,	//#REG_1TC_PCFG_usMaxFrTimeMsecMult10
0x0F,0x12,0x01,0x4D,	//#REG_1TC_PCFG_usMinFrTimeMsecMult10
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_bSmearoutput
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_Satuation
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_SharpBlur
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_Glamour
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_ColorTemp
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_Cammaindex
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_PreMirror
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_CaptureMirror
0x0F,0x12,0x00,0x00,	//#REG_1TC_PCFG_Rotation
0x00,0x2A,0x02,0x1e,	//#REG_TC_GP_ActivePrevConfig		
0x0F,0x12,0x00,0x01,	
0x00,0x2A,0x02,0x22,	//#REG_TC_GP_PrevOpenAfterChange
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xfa,	//#REG_TC_GP_NewConfigSync
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x02,0x20,	//#REG_TC_GP_PrevConfigChanged
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xf0,	//#REG_TC_GP_EnablePreview
0x0F,0x12,0x00,0x01,
0x0F,0x12,0x00,0x01,	//#REG_TC_GP_EnablePreviewChanged
/////////////////////////////////////////////////////////////////////
//ISP Bypass mode  & AE control                                                
//////////////////////////////////////////////////////////////////////
0x00,0x2A,0x51,0x00,
0x0F,0x12,0x08,0x07,
0x00,0x2A,0x54,0x00,
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x66,0x00,
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x68,0x00,
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x04,0x0E,
0x0F,0x12,0x07,0x01,	//#REG_TC_DBG_AutoAlgEnBits		
0x00,0x2A,0x03,0xFC,
0x0F,0x12,0x00,0x00,	//#REG_SF_USER_FlickerQuant		
0x0F,0x12,0x00,0x01,	//#REG_SF_USER_FlickerQuantChanged	
0x00,0x2A,0x03,0xCA,	
0x0F,0x12,0x0b,0xb8,	//REG_SF_USER_Exposure        1000/exposuetime *100       1000/10*100=10000     
0x0F,0x12,0x00,0x00,	//REG_SF_USER_ExposureHigh                  	
0x0F,0x12,0x00,0x01,	//REG_SF_USER_ExposureChanged               	
0x00,0x2A,0x03,0xD0,	
0x0F,0x12,0x04,0x00,	//REG_SF_USER_TotalGain                  
0x0F,0x12,0x00,0x01,	//REG_SF_USER_TotalGainChanged                                                   	
};
static code UINT8 frontInit4CmdTbl[] = {
0x00,0x28,0xD0,0x00,
0x00,0x2A,0xF4,0x02,
0x0F,0x12,0x00,0x1D,    //aig_ana_misc_aps
0x00,0x28,0x70,0x00,		
};

static code UINT8 front24fpsPrevCmdTbl[] = {
0x00,0x28,0x70,0x00, 
0x00,0x2A,0x02,0x1e,	//#REG_TC_GP_ActivePrevConfig		
0x0F,0x12,0x00,0x01,	
0x00,0x2A,0x02,0x22,	//#REG_TC_GP_PrevOpenAfterChange
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xfa,	//#REG_TC_GP_NewConfigSync
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x02,0x20,	//#REG_TC_GP_PrevConfigChanged
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xf0,	//#REG_TC_GP_EnablePreview
0x0F,0x12,0x00,0x01,
0x0F,0x12,0x00,0x01,	//#REG_TC_GP_EnablePreviewChanged
0x00,0x28,0xD0,0x00,	
};

static code UINT8 front30fpsPrevCmdTbl[] = {
0x00,0x28,0x70,0x00, 
0x00,0x2A,0x02,0x1e,	//#REG_TC_GP_ActivePrevConfig		
0x0F,0x12,0x00,0x00,	
0x00,0x2A,0x02,0x22,	//#REG_TC_GP_PrevOpenAfterChange
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xfa,	//#REG_TC_GP_NewConfigSync
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x02,0x20,	//#REG_TC_GP_PrevConfigChanged
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xf0,	//#REG_TC_GP_EnablePreview
0x0F,0x12,0x00,0x01,
0x0F,0x12,0x00,0x01,	//#REG_TC_GP_EnablePreviewChanged
0x00,0x28,0xD0,0x00,	
};

static code UINT8 front60fpsPrevCmdTbl[] = {	
0x00,0x28,0x70,0x00, 
};

static code UINT8 frontSnapCmdTbl[] = {
0x00,0x28,0x70,0x00, 
0x00,0x2A,0x02,0x1e,	//#REG_TC_GP_ActivePrevConfig		
0x0F,0x12,0x00,0x01,	
0x00,0x2A,0x02,0x22,	//#REG_TC_GP_PrevOpenAfterChange
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xfa,	//#REG_TC_GP_NewConfigSync
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x02,0x20,	//#REG_TC_GP_PrevConfigChanged
0x0F,0x12,0x00,0x01,
0x00,0x2A,0x01,0xf0,	//#REG_TC_GP_EnablePreview
0x0F,0x12,0x00,0x01,
0x0F,0x12,0x00,0x01,	//#REG_TC_GP_EnablePreviewChanged
0x00,0x28,0xD0,0x00,	
};
  
static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT16 frontLastGainIdx;
static UINT32 frontLastShutter;
static UINT16 frontSnapLastGain;
static UINT32 frontSnapLastShutter;

static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;

static UINT8 frontPrevModeSave = 0;	/* resolve ov9712 sensor different resolution & fps switch issue used */
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
  
  
	opFunc.expGainSetPos             = EXP_GAIN_SET_POS;
	opFunc.gainAfterExp              = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet            = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet          = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet         = frontOpCapabilityGet;
	opFunc.pfOpIntrCapabilityGet     = frontOpIntrCapabilityGet;
	opFunc.pfOpPowerOn               = frontOpPowerOn;
	opFunc.pfOpPowerOff              = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet        = frontOpPreviewModeSet;
	opFunc.pfOpSnapModeSet           = frontOpSnapModeSet;
	opFunc.pfOpSnapShot              = frontOpSnapShot;
	opFunc.pfOpGainSet               = frontOpGainSet;
	opFunc.pfOpExposureTimeSet       = frontOpExposureTimeSet;
	opFunc.pfOpIntrGainSet           = frontOpIntrGainSet;
	opFunc.pfOpIntrExposureTimeSet   = frontOpIntrExposureTimeSet;
	opFunc.pfOpOpticalBlackStatusGet = frontOpOpticalBlackStatusGet;
	opFunc.pfOpOpticalBlackCalibrate = frontOpOpticalBlackCalibrate;
	opFunc.pfOpShutterOpen           = frontOpShutterOpen;
	opFunc.pfOpShutterClose          = frontOpShutterClose;
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

	/* GPIO 23: power down (low active) */
	gpioByteFuncSet(GPIO_BYTE_GEN2, 0x80, 0xff);  
	gpioByteDirSet(GPIO_BYTE_GEN2, 0x80, 0xff);
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x80, 0xff);
	globalTimerWait(50);
	/* GPIO 10: sensor reset */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x04, 0xff);  
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x04, 0xff);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x04, 0x00);
	globalTimerWait(50);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x04, 0xff);
	globalTimerWait(50);

	/* send i2c command. */
	frontResCmdBinSend(_frontInit0CmdTbl);
	globalTimerWait(100);
	frontResCmdBinSend(_frontInit1CmdTbl);
	globalTimerWait(100);
	frontResCmdBinSend(_frontInit2CmdTbl);
	globalTimerWait(100);
	frontResCmdBinSend(_frontInit3CmdTbl);
	globalTimerWait(100);
	frontResCmdBinSend(_frontInit4CmdTbl);
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
	UINT8 pvFreq, gIdval;
	SINT16 pv_gId = 0;
	UINT8 minIdx, maxIdx;

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

	if(LoadBackfrontLastSetting==1)
	{
		frontExposureTimeSet(0,frontLastShutter,1,0);
		frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
		LoadBackfrontLastSetting = 0;
	}


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
	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
	#endif

	scaleUp=scaleUp;
	
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		frontResCmdBinSend(_frontSnapCmdTbl);
		//i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontSnapExposureTimeSet(mode);
	}
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	
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
	UINT8 cmdTbl[16];
	//UINT8 ADDR8=0x00;
/*	
0x00,0x28,     0x70,0x00,
0x00,0x2A,	0x03,0xD0,	                                
0x0F,0x12,	0x0C,0x00,	//REG_SF_USER_TotalGain         
0x0F,0x12,	0x00,0x01,	//REG_SF_USER_TotalGainChanged  
*/
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
	//frontLastGain = 0x0240;
	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x28;
	cmdTbl[2] = 0x70;
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x00;
	cmdTbl[5] = 0x2A;
	cmdTbl[6] = 0x03;
	cmdTbl[7] = 0xD0;
	cmdTbl[8] = 0x0F;
	cmdTbl[9] = 0x12;
	cmdTbl[10] = (UINT8)(frontLastGain>>8);
	cmdTbl[11] = (UINT8)(frontLastGain & 0x00FF);
	cmdTbl[12] = 0x0F;
	cmdTbl[13] = 0x12;
	cmdTbl[14] = 0x01;
	cmdTbl[15] = 0x01;

	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
	UINT16 ltotal, ftotal;
	UINT32 pixclk, espline;
	UINT8 cmdTbl[20];
	//UINT8 REG_ADDR8[2]={0x16, 0x10};
	//UINT8 REG_ADDR[2];
/*	
0x00,0x28,     0x70,0x00, 
0x00,0x2A,	0x03,0xCA,
0x0F,0x12,	0x0b,0xb8,
0x0F,0x12,	0x00,0x00,
0x0F,0x12,	0x00,0x01,*/


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
	//espline = (pixclk * 10) / (ltotal * frontLastShutter);


	printf("frontLastShutter is %ld\n", frontLastShutter);

	espline =(UINT32)(((UINT32)(1000000))/frontLastShutter);
	//printf("espline0 = %ld\n",espline);
	//printf("espline1 = %#bx%bx%bx%bx\n",(UINT8)((espline & 0xFF000000) >>24),(UINT8)(espline>>16),(UINT8)(espline>>8),(UINT8)(espline & 0xFF));
/*
 0x00,0x2A,0x03,0xCA,	
//0x0F,0x12,0x0b,0xb8,	//REG_SF_USER_Exposure 
0x0F,0x12,0x01,0xF4,	//REG_SF_USER_Exposure
0x0F,0x12,0x00,0x00,	//REG_SF_USER_ExposureHigh                  	
0x0F,0x12,0x00,0x01,	//REG_SF_USER_ExposureChanged     
*/
	
	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x28;
	cmdTbl[2] = 0x70;
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x00;
	cmdTbl[5] = 0x2A;
	cmdTbl[6] = 0x03;
	cmdTbl[7] = 0xCA;
	cmdTbl[8] = 0x0F;
	cmdTbl[9] = 0x12;
	cmdTbl[10] = (UINT8)(espline>>8);
	cmdTbl[11] = (UINT8)(espline & 0xFF);
	cmdTbl[12] = 0x0F;
	cmdTbl[13] = 0x12;
	cmdTbl[14] = (UINT8)((espline & 0xFF000000) >>24);
	cmdTbl[15] = (UINT8)(espline>>16);
	cmdTbl[16] = 0x0F;
	cmdTbl[17] = 0x12;
	cmdTbl[18] = 0x00;
	cmdTbl[19] = 0x01;
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

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
	UINT8 cmdTbl[16];
	//UINT8 REG_ADDR8[4]={0x00, 0x18,0x00,0x1d};
	//UINT8 REG_ADDR[2];
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
//frontLastGain = 0x0240;
	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x28;
	cmdTbl[2] = 0x70;
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x00;
	cmdTbl[5] = 0x2A;
	cmdTbl[6] = 0x03;
	cmdTbl[7] = 0xD0;
	cmdTbl[8] = 0x0F;
	cmdTbl[9] = 0x12;
	cmdTbl[10] = (UINT8)(frontLastGain>>8);
	cmdTbl[11] = (UINT8)(frontLastGain & 0x00FF);
	cmdTbl[12] = 0x0F;
	cmdTbl[13] = 0x12;
	cmdTbl[14] = 0x01;
	cmdTbl[15] = 0x01;

	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
	UINT16 ltotal, ftotal;
	UINT32 pixclk, espline;
	UINT8 cmdTbl[20];
	//UINT8 REG_ADDR8[2]={0x00, 0x16};
	//UINT8 REG_ADDR[2];

	
	//printf("frontPrevMode = %bu\n",frontPrevMode);
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;
//frontLastShutter = 333;
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;


printf("frontLastShutter is %ld\n", frontLastShutter);

	espline =(UINT32)(((UINT32)(1000000))/frontLastShutter);
	//printf("espline0 = %ld\n",espline);
	//printf("espline1 = %#bx%bx%bx%bx\n",(UINT8)((espline & 0xFF000000) >>24),(UINT8)(espline>>16),(UINT8)(espline>>8),(UINT8)(espline & 0xFF));

	
	cmdTbl[0] = 0x00;
	cmdTbl[1] = 0x28;
	cmdTbl[2] = 0x70;
	cmdTbl[3] = 0x00;
	cmdTbl[4] = 0x00;
	cmdTbl[5] = 0x2A;
	cmdTbl[6] = 0x03;
	cmdTbl[7] = 0xCA;
	cmdTbl[8] = 0x0F;
	cmdTbl[9] = 0x12;
	cmdTbl[10] = (UINT8)(espline>>8);
	cmdTbl[11] = (UINT8)(espline & 0xFF);
	cmdTbl[12] = 0x0F;
	cmdTbl[13] = 0x12;
	cmdTbl[14] = (UINT8)((espline & 0xFF000000) >>24);
	cmdTbl[15] = (UINT8)(espline>>16);
	cmdTbl[16] = 0x0F;
	cmdTbl[17] = 0x12;
	cmdTbl[18] = 0x00;
	cmdTbl[19] = 0x01;
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
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
	UINT8 mode
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
#if 0
	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	if(mode == SP1K_FRONT_DOFRONT){
		pixclk = pcap->pclkFreqBurst;
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
//	printf("pvGainIdxVal=%bu\n", pvGainIdxVal);

	/* get compensation gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
//	printf("compensationGainIdx=%bu\n", compensationGainIdx);

	/* calculate last gain */
	lastGainIdx = pvGainIdxVal + compensationGainIdx;
//	printf("lastGainIdx=%bu\n", lastGainIdx);

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

	i2cCmdTableSend(G_FPGAMemPool, 3, size/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	
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
	UINT32 size;
	UINT8 sts;

	if (byteAddress & 0x0001) {
		ASSERT(!(byteAddress & 0x0001), 0); 
		return;
	}
	
	//printf("fileId = 0x%04x, address = 0x%04x\n",fileId,address);
	sp1kNandRsvSizeGet(fileId, NULL, &size);

	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_TBL_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((byteAddress - 0x4000) >> 1)) << 1));
	}while(!sts);

	
	
}



