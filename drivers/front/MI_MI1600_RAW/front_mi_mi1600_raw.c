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
 * @file		front_mi_mi1600_raw.c
 * @brief		front sensor mi1600 driver.
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
#include "ae_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "sp1k_ae_api.h"
#include "dbg_file_list.h"
#include "dbg_def.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

#define __FILE __FILE_ID_SENSOR_DRIVER__
 
#define PV_AUTO_OB_BY_GAIN	0//pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1
#define SNAP_GAIN_TO_EXP	1//dark noise reduce, sensor gain transfor to exposure time : 1
 
/* mode. */
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1
#define PREV_RESP  960 

/* dimensiion. */
#if (PREV_RESP==960) //960p

#define WIDTH_20FPS_PREV      720
#define HEIGHT_20FPS_PREV     540
#define XOFF_20FPS_PREV       719
#define YOFF_20FPS_PREV       4
#define LINE_TOT_20FPS_PREV   1453
#define FRAME_TOT_20FPS_PREV  557

#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       719
#define YOFF_30FPS_PREV       4 
#define LINE_TOT_30FPS_PREV   2016
#define FRAME_TOT_30FPS_PREV 978

#define WIDTH_60FPS_PREV      720
#define HEIGHT_60FPS_PREV     540
#define XOFF_60FPS_PREV       719
#define YOFF_60FPS_PREV       4
#define LINE_TOT_60FPS_PREV   1453
#define FRAME_TOT_60FPS_PREV  557

#elif (PREV_RESP==720)
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       719
#define YOFF_30FPS_PREV       4
#define LINE_TOT_30FPS_PREV   1932
#define FRAME_TOT_30FPS_PREV 769

#if 1
#define WIDTH_60FPS_PREV      720
#define HEIGHT_60FPS_PREV     540
#define XOFF_60FPS_PREV       719
#define YOFF_60FPS_PREV       4
#define LINE_TOT_60FPS_PREV   1350
#define FRAME_TOT_60FPS_PREV  567
#endif

#elif (PREV_RESP==480)
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       910
#define YOFF_30FPS_PREV       9
#define LINE_TOT_30FPS_PREV   1550
#define FRAME_TOT_30FPS_PREV  516

#if 0
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#endif

#define WIDTH_SNAP      1440
#define HEIGHT_SNAP     1080
#define XOFF_SNAP       (719+4)
#define YOFF_SNAP       (4+2)
#define LINE_TOT_SNAP   2188
#define FRAME_TOT_SNAP  1105

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_20FPS_PREV    18
#define MCLK_PHASE_20FPS_PREV  0
#define PCLK_DIV_20FPS_PREV    11
#define PCLK_PHASE_20FPS_PREV  0

#define MCLK_DIV_30FPS_PREV   18 
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    18
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP    		18
#define MCLK_DIV_SNAP_BURST     18
#define MCLK_DIV_SNAP_BURST_UP  18
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    11
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
	#define PCLK_FREQ_20FPS_PREV  24570000L
	#define PCLK_FREQ_30FPS_PREV  60415000L
	#define PCLK_FREQ_60FPS_PREV  49130000L
	#define PCLK_FREQ_SNAP       	49130000L 
	#define PCLK_FREQ_SNAP_BURST	49130000L 	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */	
	#define PCLK_FREQ_SNAP_BURST_UP 49130000L 	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */		
#else
	#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
	#if 0
	#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
	#endif
	#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */

/*VGA 30fps*/
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_20FPS_PREV    1
#else
#define RESHAPE_HEN_20FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_20FPS_PREV  140
#define RESHAPE_HRISE_20FPS_PREV  150
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_20FPS_PREV    0
#else
#define RESHAPE_VEN_20FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_20FPS_PREV  8
#define RESHAPE_VRISE_20FPS_PREV  10

/*HD 30fps*/
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    1
#else
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV  140
#define RESHAPE_HRISE_30FPS_PREV  150
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_30FPS_PREV  8
#define RESHAPE_VRISE_30FPS_PREV  10

/*VGA 60fps*/
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_60FPS_PREV    1
#else
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_60FPS_PREV  140
#define RESHAPE_HRISE_60FPS_PREV  150
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_60FPS_PREV  8
#define RESHAPE_VRISE_60FPS_PREV  10

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  140
#define RESHAPE_HRISE_SNAP  150
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#if (PREV_RESP==960) //960p
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GRRBGB

#elif (PREV_RESP==480)
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GBBRGR

#elif (PREV_RESP==720)
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GRRBGB
#endif

#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_GRRBGB


/* i2c interface. */
#define I2C_DEV_ADDR  0xB8
#define I2C_CLK_DIV   1

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_20FPS_30_MAX_IDX  129
#define AE_20FPS_30_MIN_IDX  12
#define AE_20FPS_25_MAX_IDX  130
#define AE_20FPS_25_MIN_IDX  13
#define EV10_20FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  137
#define AE_30FPS_30_MIN_IDX  12
#define AE_30FPS_25_MAX_IDX  138
#define AE_30FPS_25_MIN_IDX  13
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_30_MAX_IDX  123
#define AE_60FPS_30_MIN_IDX  12
#define AE_60FPS_25_MAX_IDX  130
#define AE_60FPS_25_MIN_IDX  13
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_20FPS_MAX_IDX 120
#define GAIN_20FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  120
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX 120
#define GAIN_60FPS_MIN_IDX  0

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor
#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					//AE_GAIN_AFTER_EXP_OFF,
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
__STATIC void frontOpSnapShot(UINT8 first,UINT8 mode,UINT8 scaleUp);
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

static void frontBeforePowerOn(void);
static void frontAfterPowerOn(void);
static void frontBeforePowerOff(void);
static void frontAfterPowerOff(void);
static void frontSnapExposureTimeSet(UINT8 mode, UINT8 scaleUp);
static void frontBeforeSnapExposureTimeSet(void);
static void frontResTblBinRead(UINT16 fileId,	UINT16 address);
static void frontResCmdBinSend(UINT16 fileId);


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - MI MI1600 RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table	
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
	//_frontInit3CmdTbl		= 0x00CE,
	//_frontInit4CmdTbl		= 0x00CF,
	//_front15fpsPrevCmdTbl	= 0x00D0,
	_front20fpsPrevCmdTbl	= 0x00D1,
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

/* ae table. */
static code frontAe_t ae20fps50hzTbl[130] = {
	{	0,	333,	120,	0,	0 },
	{	1,	333,	115,	0,	0 },
	{	2,	333,	110,	0,	0 },
	{	3,	333,	106,	0,	0 },
	{	4,	333,	101,	0,	0 },
	{	5,	333,	97,	0,	0 },
	{	6,	333,	92,	0,	0 },
	{	7,	333,	88,	0,	0 },
	{	8,	333,	83,	0,	0 },
	{	9,	333,	79,	0,	0 },
	{	10,	333,	75,	0,	0 },
	{	11,	333,	71,	0,	0 },
	{	12,	333,	67,	0,	0 },
	{	13,	333,	64,	0,	0 },
	{	14,	333,	61,	0,	0 },
	{	15,	333,	58,	0,	0 },
	{	16,	333,	55,	0,	0 },
	{	17,	333,	52,	0,	0 },
	{	18,	333,	50,	0,	0 },
	{	19,	333,	47,	0,	0 },
	{	20,	333,	43,	0,	0 },
	{	21,	333,	39,	0,	0 },
	{	22,	333,	35,	0,	0 },
	{	23,	333,	32,	0,	0 },
	{	24,	333,	29,	0,	0 },
	{	25,	333,	26,	0,	0 },
	{	26,	333,	23,	0,	0 },
	{	27,	333,	20,	0,	0 },
	{	28,	333,	18,	0,	0 },
	{	29,	333,	16,	0,	0 },
	{	30,	333,	14,	0,	0 },
	{	31,	333,	12,	0,	0 },
	{	32,	333,	10,	0,	0 },
	{	33,	333,	8,	0,	0 },
	{	34,	333,	7,	0,	0 },
	{	35,	333,	5,	0,	0 },
	{	36,	333,	4,	0,	0 },
	{	37,	333,	2,	0,	0 },
	{	38,	333,	1,	0,	0 },
	{	39,	333,	0,	0,	0 },
	{	40,	499,	6,	0,	0 },
	{	41,	499,	5,	0,	0 },
	{	42,	499,	4,	0,	0 },
	{	43,	499,	2,	0,	0 },
	{	44,	499,	1,	0,	0 },
	{	45,	499,	0,	0,	0 },
	{	46,	995,	13,	0,	0 },
	{	47,	995,	11,	0,	0 },
	{	48,	995,	10,	0,	0 },
	{	49,	995,	8,	0,	0 },
	{	50,	995,	6,	0,	0 },
	{	51,	995,	5,	0,	0 },
	{	52,	995,	3,	0,	0 },
	{	53,	995,	2,	0,	0 },
	{	54,	995,	1,	0,	0 },
	{	55,	995,	0,	0,	0 },
	{	56,	1044,	0,	0,	0 },
	{	57,	1120,	0,	0,	0 },
	{	58,	1199,	0,	0,	0 },
	{	59,	1291,	0,	0,	0 },
	{	60,	1375,	0,	0,	0 },
	{	61,	1483,	0,	0,	0 },
	{	62,	1580,	0,	0,	0 },
	{	63,	1708,	0,	0,	0 },
	{	64,	1818,	0,	0,	0 },
	{	65,	1966,	0,	0,	0 },
	{	66,	2088,	0,	0,	0 },
	{	67,	2255,	0,	0,	0 },
	{	68,	2416,	0,	0,	0 },
	{	69,	2602,	0,	0,	0 },
	{	70,	2772,	0,	0,	0 },
	{	71,	2967,	0,	0,	0 },
	{	72,	3191,	0,	0,	0 },
	{	73,	3451,	0,	0,	0 },
	{	74,	3676,	0,	0,	0 },
	{	75,	3933,	0,	0,	0 },
	{	76,	4227,	0,	0,	0 },
	{	77,	4570,	0,	0,	0 },
	{	78,	4831,	0,	0,	0 },
	{	79,	5284,	0,	0,	0 },
	{	80,	5637,	0,	0,	0 },
	{	81,	6039,	0,	0,	0 },
	{	82,	6504,	0,	0,	0 },
	{	83,	7046,	0,	0,	0 },
	{	84,	7352,	0,	0,	0 },
	{	85,	8052,	0,	0,	1 },
	{	86,	8455,	0,	0,	0 },
	{	87,	9394,	0,	0,	1 },
	{	88,	9947,	0,	0,	0 },
	{	89,	10569,	0,	0,	0 },
	{	90,	11273,	0,	0,	1 },
	{	91,	12078,	0,	0,	1 },
	{	92,	13007,	0,	0,	0 },
	{	93,	14091,	0,	0,	1 },
	{	94,	15372,	0,	0,	1 },
	{	95,	16910,	1,	0,	0 },
	{	96,	16910,	0,	0,	0 },
	{	97,	18789,	0,	0,	1 },
	{	98,	21137,	1,	0,	1 },
	{	99,	21137,	0,	0,	1 },
	{	100,	24157,	1,	0,	1 },
	{	101,	24157,	0,	0,	1 },
	{	102,	28183,	1,	0,	2 },
	{	103,	28183,	0,	0,	2 },
	{	104,	33819,	2,	0,	2 },
	{	105,	33819,	1,	0,	2 },
	{	106,	33819,	0,	0,	2 },
	{	107,	42274,	2,	0,	1 },
	{	108,	42274,	1,	0,	1 },
	{	109,	42274,	0,	0,	1 },
	{	110,	56364,	4,	0,	5 },
	{	111,	56364,	3,	0,	5 },
	{	112,	56364,	1,	0,	5 },
	{	113,	56364,	0,	0,	5 },
	{	114,	84545,	6,	0,	5 },
	{	115,	84545,	5,	0,	5 },
	{	116,	84545,	4,	0,	5 },
	{	117,	84545,	2,	0,	5 },
	{	118,	84545,	1,	0,	5 },
	{	119,	84545,	0,	0,	5 },
	{	120,	169082,	13,	0,	5 },
	{	121,	169082,	12,	0,	5 },
	{	122,	169082,	10,	0,	5 },
	{	123,	169082,	8,	0,	5 },
	{	124,	169082,	6,	0,	5 },
	{	125,	169082,	5,	0,	5 },
	{	126,	169082,	4,	0,	5 },
	{	127,	169082,	2,	0,	5 },
	{	128,	169082,	1,	0,	5 },
	{	129,	169082,	0,	0,	5 },
	};
static code  frontAe_t ae20fps60hzTbl[131] = {
	{	0,	299,	118,	0,	0 },
	{	1,	299,	112,	0,	0 },
	{	2,	299,	108,	0,	0 },
	{	3,	299,	103,	0,	0 },
	{	4,	299,	99,	0,	0 },
	{	5,	299,	94,	0,	0 },
	{	6,	299,	90,	0,	0 },
	{	7,	299,	85,	0,	0 },
	{	8,	299,	81,	0,	0 },
	{	9,	299,	77,	0,	0 },
	{	10,	299,	73,	0,	0 },
	{	11,	299,	69,	0,	0 },
	{	12,	299,	65,	0,	0 },
	{	13,	299,	62,	0,	0 },
	{	14,	299,	59,	0,	0 },
	{	15,	299,	56,	0,	0 },
	{	16,	299,	54,	0,	0 },
	{	17,	299,	51,	0,	0 },
	{	18,	299,	49,	0,	0 },
	{	19,	299,	45,	0,	0 },
	{	20,	299,	41,	0,	0 },
	{	21,	299,	37,	0,	0 },
	{	22,	299,	33,	0,	0 },
	{	23,	299,	30,	0,	0 },
	{	24,	299,	27,	0,	0 },
	{	25,	299,	24,	0,	0 },
	{	26,	299,	22,	0,	0 },
	{	27,	299,	19,	0,	0 },
	{	28,	299,	17,	0,	0 },
	{	29,	299,	15,	0,	0 },
	{	30,	299,	13,	0,	0 },
	{	31,	299,	11,	0,	0 },
	{	32,	299,	9,	0,	0 },
	{	33,	299,	7,	0,	0 },
	{	34,	299,	6,	0,	0 },
	{	35,	299,	4,	0,	0 },
	{	36,	299,	3,	0,	0 },
	{	37,	299,	2,	0,	0 },
	{	38,	299,	1,	0,	0 },
	{	39,	299,	0,	0,	0 },
	{	40,	399,	3,	0,	0 },
	{	41,	399,	2,	0,	0 },
	{	42,	399,	1,	0,	0 },
	{	43,	399,	0,	0,	0 },
	{	44,	598,	6,	0,	0 },
	{	45,	598,	4,	0,	0 },
	{	46,	598,	3,	0,	0 },
	{	47,	598,	2,	0,	0 },
	{	48,	598,	1,	0,	0 },
	{	49,	600,	0,	0,	0 },
	{	50,	1191,	12,	0,	0 },
	{	51,	1191,	11,	0,	0 },
	{	52,	1191,	9,	0,	0 },
	{	53,	1191,	7,	0,	0 },
	{	54,	1191,	6,	0,	0 },
	{	55,	1191,	4,	0,	0 },
	{	56,	1191,	3,	0,	0 },
	{	57,	1191,	2,	0,	0 },
	{	58,	1191,	1,	0,	0 },
	{	59,	1199,	0,	0,	0 },
	{	60,	1291,	0,	0,	0 },
	{	61,	1375,	0,	0,	0 },
	{	62,	1483,	0,	0,	0 },
	{	63,	1580,	0,	0,	0 },
	{	64,	1708,	0,	0,	0 },
	{	65,	1818,	0,	0,	0 },
	{	66,	1966,	0,	0,	0 },
	{	67,	2088,	0,	0,	0 },
	{	68,	2255,	0,	0,	0 },
	{	69,	2416,	0,	0,	0 },
	{	70,	2602,	0,	0,	0 },
	{	71,	2772,	0,	0,	0 },
	{	72,	2967,	0,	0,	0 },
	{	73,	3191,	0,	0,	0 },
	{	74,	3451,	0,	0,	0 },
	{	75,	3676,	0,	0,	0 },
	{	76,	3933,	0,	0,	0 },
	{	77,	4227,	0,	0,	0 },
	{	78,	4570,	0,	0,	0 },
	{	79,	4831,	0,	0,	0 },
	{	80,	5284,	0,	0,	0 },
	{	81,	5637,	0,	0,	0 },
	{	82,	6039,	0,	0,	0 },
	{	83,	6504,	0,	0,	0 },
	{	84,	7046,	0,	0,	0 },
	{	85,	7352,	0,	0,	0 },
	{	86,	8052,	0,	0,	1 },
	{	87,	8455,	0,	0,	0 },
	{	88,	9394,	0,	0,	1 },
	{	89,	9947,	0,	0,	0 },
	{	90,	10569,	0,	0,	0 },
	{	91,	11273,	0,	0,	1 },
	{	92,	12078,	0,	0,	1 },
	{	93,	13007,	0,	0,	0 },
	{	94,	14091,	0,	0,	1 },
	{	95,	15372,	0,	0,	1 },
	{	96,	16910,	1,	0,	0 },
	{	97,	16910,	0,	0,	0 },
	{	98,	18789,	0,	0,	1 },
	{	99,	21137,	1,	0,	1 },
	{	100,	21137,	0,	0,	1 },
	{	101,	24157,	1,	0,	1 },
	{	102,	24157,	0,	0,	1 },
	{	103,	28183,	1,	0,	2 },
	{	104,	28183,	0,	0,	2 },
	{	105,	33819,	2,	0,	2 },
	{	106,	33819,	1,	0,	2 },
	{	107,	33819,	0,	0,	2 },
	{	108,	42274,	2,	0,	1 },
	{	109,	42274,	1,	0,	1 },
	{	110,	42274,	0,	0,	1 },
	{	111,	56364,	4,	0,	5 },
	{	112,	56364,	3,	0,	5 },
	{	113,	56364,	1,	0,	5 },
	{	114,	56364,	0,	0,	5 },
	{	115,	84545,	6,	0,	5 },
	{	116,	84545,	5,	0,	5 },
	{	117,	84545,	4,	0,	5 },
	{	118,	84545,	2,	0,	5 },
	{	119,	84545,	1,	0,	5 },
	{	120,	84545,	0,	0,	5 },
	{	121,	169082,	13,	0,	5 },
	{	122,	169082,	12,	0,	5 },
	{	123,	169082,	10,	0,	5 },
	{	124,	169082,	8,	0,	5 },
	{	125,	169082,	6,	0,	5 },
	{	126,	169082,	5,	0,	5 },
	{	127,	169082,	4,	0,	5 },
	{	128,	169082,	2,	0,	5 },
	{	129,	169082,	1,	0,	5 },
	{	130,	169082,	0,	0,	5 },
	};

static code frontAe_t ae30fps50hzTbl[138] = {
	{	0,	332,	119,	0,	0 },
	{	1,	332,	114,	0,	0 },
	{	2,	332,	109,	0,	0 },
	{	3,	332,	104,	0,	0 },
	{	4,	332,	100,	0,	0 },
	{	5,	332,	95,	0,	0 },
	{	6,	332,	91,	0,	0 },
	{	7,	332,	86,	0,	0 },
	{	8,	332,	82,	0,	0 },
	{	9,	332,	78,	0,	0 },
	{	10,	332,	74,	0,	0 },
	{	11,	332,	70,	0,	0 },
	{	12,	332,	66,	0,	0 },
	{	13,	332,	63,	0,	0 },
	{	14,	332,	60,	0,	0 },
	{	15,	332,	57,	0,	0 },
	{	16,	332,	54,	0,	0 },
	{	17,	332,	52,	0,	0 },
	{	18,	332,	49,	0,	0 },
	{	19,	332,	46,	0,	0 },
	{	20,	332,	42,	0,	0 },
	{	21,	332,	38,	0,	0 },
	{	22,	332,	34,	0,	0 },
	{	23,	332,	31,	0,	0 },
	{	24,	332,	28,	0,	0 },
	{	25,	332,	25,	0,	0 },
	{	26,	332,	22,	0,	0 },
	{	27,	332,	20,	0,	0 },
	{	28,	332,	17,	0,	0 },
	{	29,	332,	15,	0,	0 },
	{	30,	332,	13,	0,	0 },
	{	31,	332,	11,	0,	0 },
	{	32,	332,	9,	0,	0 },
	{	33,	332,	8,	0,	0 },
	{	34,	332,	6,	0,	0 },
	{	35,	332,	5,	0,	0 },
	{	36,	332,	3,	0,	0 },
	{	37,	332,	2,	0,	0 },
	{	38,	332,	1,	0,	0 },
	{	39,	332,	0,	0,	0 },
	{	40,	498,	6,	0,	0 },
	{	41,	498,	5,	0,	0 },
	{	42,	498,	3,	0,	0 },
	{	43,	498,	2,	0,	0 },
	{	44,	498,	1,	0,	0 },
	{	45,	498,	0,	0,	0 },
	{	46,	992,	13,	0,	0 },
	{	47,	992,	11,	0,	0 },
	{	48,	992,	9,	0,	0 },
	{	49,	992,	7,	0,	0 },
	{	50,	992,	6,	0,	0 },
	{	51,	992,	4,	0,	0 },
	{	52,	992,	3,	0,	0 },
	{	53,	992,	2,	0,	0 },
	{	54,	992,	1,	0,	0 },
	{	55,	992,	0,	0,	0 },
	{	56,	1063,	0,	0,	0 },
	{	57,	1139,	0,	0,	0 },
	{	58,	1218,	0,	0,	0 },
	{	59,	1309,	0,	0,	0 },
	{	60,	1400,	0,	0,	0 },
	{	61,	1506,	0,	0,	0 },
	{	62,	1611,	0,	0,	0 },
	{	63,	1732,	0,	0,	0 },
	{	64,	1850,	0,	0,	0 },
	{	65,	1985,	0,	0,	0 },
	{	66,	2125,	0,	0,	0 },
	{	67,	2288,	0,	0,	0 },
	{	68,	2436,	0,	0,	0 },
	{	69,	2629,	0,	0,	0 },
	{	70,	2801,	0,	0,	0 },
	{	71,	3027,	0,	0,	0 },
	{	72,	3222,	0,	0,	0 },
	{	73,	3485,	0,	0,	0 },
	{	74,	3700,	0,	0,	0 },
	{	75,	3996,	0,	0,	0 },
	{	76,	4281,	0,	0,	0 },
	{	77,	4610,	0,	0,	0 },
	{	78,	4913,	0,	0,	0 },
	{	79,	5257,	0,	0,	0 },
	{	80,	5654,	0,	0,	0 },
	{	81,	6116,	0,	0,	0 },
	{	82,	6515,	0,	0,	0 },
	{	83,	6969,	0,	0,	0 },
	{	84,	7492,	0,	0,	1 },
	{	85,	8099,	0,	0,	0 },
	{	86,	8562,	0,	0,	0 },
	{	87,	9365,	0,	0,	1 },
	{	88,	9989,	0,	0,	0 },
	{	89,	10703,	0,	0,	1 },
	{	90,	11526,	0,	0,	0 },
	{	91,	12487,	0,	0,	1 },
	{	92,	13029,	0,	0,	0 },
	{	93,	14270,	0,	0,	1 },
	{	94,	14984,	0,	0,	1 },
	{	95,	16649,	0,	0,	1 },
	{	96,	17628,	0,	0,	1 },
	{	97,	18730,	0,	0,	1 },
	{	98,	19978,	0,	0,	0 },
	{	99,	21405,	0,	0,	1 },
	{	100,	23052,	0,	0,	1 },
	{	101,	24973,	0,	0,	2 },
	{	102,	27243,	0,	0,	0 },
	{	103,	29967,	1,	0,	1 },
	{	104,	29967,	0,	0,	1 },
	{	105,	33297,	0,	0,	2 },
	{	106,	37459,	1,	0,	3 },
	{	107,	37459,	0,	0,	3 },
	{	108,	42810,	1,	0,	1 },
	{	109,	42810,	0,	0,	1 },
	{	110,	49945,	1,	0,	2 },
	{	111,	49945,	0,	0,	2 },
	{	112,	59934,	2,	0,	4 },
	{	113,	59934,	1,	0,	4 },
	{	114,	59934,	0,	0,	4 },
	{	115,	74918,	2,	0,	8 },
	{	116,	74918,	1,	0,	8 },
	{	117,	74918,	0,	0,	8 },
	{	118,	99889,	4,	0,	2 },
	{	119,	99889,	3,	0,	2 },
	{	120,	99889,	1,	0,	2 },
	{	121,	99889,	0,	0,	2 },
	{	122,	149831,	6,	0,	8 },
	{	123,	149831,	5,	0,	8 },
	{	124,	149831,	4,	0,	8 },
	{	125,	149831,	2,	0,	8 },
	{	126,	149831,	1,	0,	8 },
	{	127,	149831,	0,	0,	8 },
	{	128,	299648,	13,	0,	0 },
	{	129,	299648,	12,	0,	0 },
	{	130,	299648,	10,	0,	0 },
	{	131,	299648,	8,	0,	0 },
	{	132,	299648,	6,	0,	0 },
	{	133,	299648,	5,	0,	0 },
	{	134,	299648,	4,	0,	0 },
	{	135,	299648,	2,	0,	0 },
	{	136,	299648,	1,	0,	0 },
	{	137,	299648,	0,	0,	0 },
	};

static code frontAe_t ae30fps60hzTbl[139] = {
	{	0,	299,	116,	0,	0 },
	{	1,	299,	111,	0,	0 },
	{	2,	299,	107,	0,	0 },
	{	3,	299,	102,	0,	0 },
	{	4,	299,	97,	0,	0 },
	{	5,	299,	93,	0,	0 },
	{	6,	299,	89,	0,	0 },
	{	7,	299,	84,	0,	0 },
	{	8,	299,	80,	0,	0 },
	{	9,	299,	76,	0,	0 },
	{	10,	299,	72,	0,	0 },
	{	11,	299,	68,	0,	0 },
	{	12,	299,	65,	0,	0 },
	{	13,	299,	61,	0,	0 },
	{	14,	299,	58,	0,	0 },
	{	15,	299,	56,	0,	0 },
	{	16,	299,	53,	0,	0 },
	{	17,	299,	51,	0,	0 },
	{	18,	299,	48,	0,	0 },
	{	19,	299,	44,	0,	0 },
	{	20,	299,	40,	0,	0 },
	{	21,	299,	36,	0,	0 },
	{	22,	299,	33,	0,	0 },
	{	23,	299,	29,	0,	0 },
	{	24,	299,	26,	0,	0 },
	{	25,	299,	24,	0,	0 },
	{	26,	299,	21,	0,	0 },
	{	27,	299,	19,	0,	0 },
	{	28,	299,	16,	0,	0 },
	{	29,	299,	14,	0,	0 },
	{	30,	299,	12,	0,	0 },
	{	31,	299,	10,	0,	0 },
	{	32,	299,	9,	0,	0 },
	{	33,	299,	7,	0,	0 },
	{	34,	299,	5,	0,	0 },
	{	35,	299,	4,	0,	0 },
	{	36,	299,	3,	0,	0 },
	{	37,	299,	2,	0,	0 },
	{	38,	299,	0,	0,	0 },
	{	39,	399,	4,	0,	0 },
	{	40,	399,	3,	0,	0 },
	{	41,	399,	2,	0,	0 },
	{	42,	399,	1,	0,	0 },
	{	43,	598,	7,	0,	0 },
	{	44,	598,	5,	0,	0 },
	{	45,	598,	4,	0,	0 },
	{	46,	598,	3,	0,	0 },
	{	47,	598,	1,	0,	0 },
	{	48,	598,	0,	0,	0 },
	{	49,	1189,	14,	0,	0 },
	{	50,	1189,	12,	0,	0 },
	{	51,	1189,	10,	0,	0 },
	{	52,	1189,	8,	0,	0 },
	{	53,	1189,	7,	0,	0 },
	{	54,	1189,	5,	0,	0 },
	{	55,	1189,	4,	0,	0 },
	{	56,	1189,	3,	0,	0 },
	{	57,	1189,	1,	0,	0 },
	{	58,	1189,	0,	0,	0 },
	{	59,	1218,	0,	0,	0 },
	{	60,	1309,	0,	0,	0 },
	{	61,	1400,	0,	0,	0 },
	{	62,	1506,	0,	0,	0 },
	{	63,	1611,	0,	0,	0 },
	{	64,	1732,	0,	0,	0 },
	{	65,	1850,	0,	0,	0 },
	{	66,	1985,	0,	0,	0 },
	{	67,	2125,	0,	0,	0 },
	{	68,	2288,	0,	0,	0 },
	{	69,	2436,	0,	0,	0 },
	{	70,	2629,	0,	0,	0 },
	{	71,	2801,	0,	0,	0 },
	{	72,	3027,	0,	0,	0 },
	{	73,	3222,	0,	0,	0 },
	{	74,	3485,	0,	0,	0 },
	{	75,	3700,	0,	0,	0 },
	{	76,	3996,	0,	0,	0 },
	{	77,	4281,	0,	0,	0 },
	{	78,	4610,	0,	0,	0 },
	{	79,	4913,	0,	0,	0 },
	{	80,	5257,	0,	0,	0 },
	{	81,	5654,	0,	0,	0 },
	{	82,	6116,	0,	0,	0 },
	{	83,	6515,	0,	0,	0 },
	{	84,	6969,	0,	0,	0 },
	{	85,	7492,	0,	0,	1 },
	{	86,	8099,	0,	0,	0 },
	{	87,	8562,	0,	0,	0 },
	{	88,	9365,	0,	0,	1 },
	{	89,	9989,	0,	0,	0 },
	{	90,	10703,	0,	0,	1 },
	{	91,	11526,	0,	0,	0 },
	{	92,	12487,	0,	0,	1 },
	{	93,	13029,	0,	0,	0 },
	{	94,	14270,	0,	0,	1 },
	{	95,	14984,	0,	0,	1 },
	{	96,	16649,	0,	0,	1 },
	{	97,	17628,	0,	0,	1 },
	{	98,	18730,	0,	0,	1 },
	{	99,	19978,	0,	0,	0 },
	{	100,	21405,	0,	0,	1 },
	{	101,	23052,	0,	0,	1 },
	{	102,	24973,	0,	0,	2 },
	{	103,	27243,	0,	0,	0 },
	{	104,	29967,	1,	0,	1 },
	{	105,	29967,	0,	0,	1 },
	{	106,	33297,	0,	0,	2 },
	{	107,	37459,	1,	0,	3 },
	{	108,	37459,	0,	0,	3 },
	{	109,	42810,	1,	0,	1 },
	{	110,	42810,	0,	0,	1 },
	{	111,	49945,	1,	0,	2 },
	{	112,	49945,	0,	0,	2 },
	{	113,	59934,	2,	0,	4 },
	{	114,	59934,	1,	0,	4 },
	{	115,	59934,	0,	0,	4 },
	{	116,	74918,	2,	0,	8 },
	{	117,	74918,	1,	0,	8 },
	{	118,	74918,	0,	0,	8 },
	{	119,	99889,	4,	0,	2 },
	{	120,	99889,	3,	0,	2 },
	{	121,	99889,	1,	0,	2 },
	{	122,	99889,	0,	0,	2 },
	{	123,	149831,	6,	0,	8 },
	{	124,	149831,	5,	0,	8 },
	{	125,	149831,	4,	0,	8 },
	{	126,	149831,	2,	0,	8 },
	{	127,	149831,	1,	0,	8 },
	{	128,	149831,	0,	0,	8 },
	{	129,	299648,	13,	0,	0 },
	{	130,	299648,	12,	0,	0 },
	{	131,	299648,	10,	0,	0 },
	{	132,	299648,	8,	0,	0 },
	{	133,	299648,	6,	0,	0 },
	{	134,	299648,	5,	0,	0 },
	{	135,	299648,	4,	0,	0 },
	{	136,	299648,	2,	0,	0 },
	{	137,	299648,	1,	0,	0 },
	{	138,	299648,	0,	0,	0 },
	};

static code frontAe_t ae60fps50hzTbl[124] = {
	{	0,	992,	119,	0,	0 },
	{	1,	992,	114,	0,	0 },
	{	2,	992,	109,	0,	0 },
	{	3,	992,	105,	0,	0 },
	{	4,	992,	100,	0,	0 },
	{	5,	992,	96,	0,	0 },
	{	6,	992,	91,	0,	0 },
	{	7,	992,	86,	0,	0 },
	{	8,	992,	82,	0,	0 },
	{	9,	992,	78,	0,	0 },
	{	10,	992,	74,	0,	0 },
	{	11,	992,	70,	0,	0 },
	{	12,	992,	66,	0,	0 },
	{	13,	992,	63,	0,	0 },
	{	14,	992,	60,	0,	0 },
	{	15,	992,	57,	0,	0 },
	{	16,	992,	54,	0,	0 },
	{	17,	992,	52,	0,	0 },
	{	18,	992,	49,	0,	0 },
	{	19,	992,	46,	0,	0 },
	{	20,	992,	42,	0,	0 },
	{	21,	992,	38,	0,	0 },
	{	22,	992,	34,	0,	0 },
	{	23,	992,	31,	0,	0 },
	{	24,	992,	28,	0,	0 },
	{	25,	992,	25,	0,	0 },
	{	26,	992,	22,	0,	0 },
	{	27,	992,	20,	0,	0 },
	{	28,	992,	17,	0,	0 },
	{	29,	992,	15,	0,	0 },
	{	30,	992,	13,	0,	0 },
	{	31,	992,	11,	0,	0 },
	{	32,	992,	9,	0,	0 },
	{	33,	992,	8,	0,	0 },
	{	34,	992,	6,	0,	0 },
	{	35,	992,	5,	0,	0 },
	{	36,	992,	3,	0,	0 },
	{	37,	992,	2,	0,	0 },
	{	38,	992,	1,	0,	0 },
	{	39,	992,	0,	0,	0 },
	{	40,	1044,	0,	0,	0 },
	{	41,	1120,	0,	0,	0 },
	{	42,	1199,	0,	0,	0 },
	{	43,	1286,	0,	0,	0 },
	{	44,	1375,	0,	0,	0 },
	{	45,	1477,	0,	0,	0 },
	{	46,	1580,	0,	0,	0 },
	{	47,	1699,	0,	0,	0 },
	{	48,	1818,	0,	0,	0 },
	{	49,	1954,	0,	0,	0 },
	{	50,	2087,	0,	0,	0 },
	{	51,	2239,	0,	0,	0 },
	{	52,	2398,	0,	0,	0 },
	{	53,	2581,	0,	0,	0 },
	{	54,	2749,	0,	0,	0 },
	{	55,	2966,	0,	0,	0 },
	{	56,	3160,	0,	0,	0 },
	{	57,	3415,	0,	0,	0 },
	{	58,	3636,	0,	0,	0 },
	{	59,	3932,	0,	0,	0 },
	{	60,	4174,	0,	0,	0 },
	{	61,	4508,	0,	0,	0 },
	{	62,	4830,	0,	0,	0 },
	{	63,	5202,	0,	0,	0 },
	{	64,	5543,	0,	0,	0 },
	{	65,	5932,	0,	0,	0 },
	{	66,	6380,	0,	0,	0 },
	{	67,	6901,	0,	0,	0 },
	{	68,	7351,	0,	0,	0 },
	{	69,	7863,	0,	0,	0 },
	{	70,	8453,	0,	0,	0 },
	{	71,	9139,	0,	0,	1 },
	{	72,	9661,	0,	0,	0 },
	{	73,	10566,	0,	0,	0 },
	{	74,	11271,	0,	0,	0 },
	{	75,	12076,	0,	0,	1 },
	{	76,	13005,	0,	0,	0 },
	{	77,	14089,	0,	0,	0 },
	{	78,	14701,	0,	0,	0 },
	{	79,	16101,	0,	0,	0 },
	{	80,	16906,	0,	0,	0 },
	{	81,	18785,	0,	0,	0 },
	{	82,	19890,	0,	0,	1 },
	{	83,	21133,	0,	0,	1 },
	{	84,	22542,	0,	0,	0 },
	{	85,	24152,	0,	0,	1 },
	{	86,	26010,	0,	0,	1 },
	{	87,	28177,	0,	0,	0 },
	{	88,	30739,	0,	0,	1 },
	{	89,	33812,	1,	0,	2 },
	{	90,	33812,	0,	0,	2 },
	{	91,	37569,	0,	0,	0 },
	{	92,	42265,	1,	0,	1 },
	{	93,	42265,	0,	0,	1 },
	{	94,	48303,	1,	0,	3 },
	{	95,	48303,	0,	0,	3 },
	{	96,	56354,	1,	0,	0 },
	{	97,	56354,	0,	0,	0 },
	{	98,	67624,	2,	0,	2 },
	{	99,	67624,	1,	0,	2 },
	{	100,	67624,	0,	0,	2 },
	{	101,	84530,	2,	0,	5 },
	{	102,	84530,	1,	0,	5 },
	{	103,	84530,	0,	0,	5 },
	{	104,	112706,	4,	0,	0 },
	{	105,	112706,	3,	0,	0 },
	{	106,	112706,	1,	0,	0 },
	{	107,	112706,	0,	0,	0 },
	{	108,	169056,	6,	0,	5 },
	{	109,	169056,	5,	0,	5 },
	{	110,	169056,	4,	0,	5 },
	{	111,	169056,	2,	0,	5 },
	{	112,	169056,	1,	0,	5 },
	{	113,	169056,	0,	0,	5 },
	{	114,	338094,	13,	0,	0 },
	{	115,	338094,	12,	0,	0 },
	{	116,	338094,	10,	0,	0 },
	{	117,	338094,	8,	0,	0 },
	{	118,	338094,	6,	0,	0 },
	{	119,	338094,	5,	0,	0 },
	{	120,	338094,	4,	0,	0 },
	{	121,	338094,	2,	0,	0 },
	{	122,	338094,	1,	0,	0 },
	{	123,	338094,	0,	0,	0 },
	};
static code frontAe_t ae60fps60hzTbl[131] = {
	{	0,	597,	117,	0,	0 },
	{	1,	597,	112,	0,	0 },
	{	2,	597,	108,	0,	0 },
	{	3,	597,	103,	0,	0 },
	{	4,	597,	98,	0,	0 },
	{	5,	597,	94,	0,	0 },
	{	6,	597,	90,	0,	0 },
	{	7,	597,	85,	0,	0 },
	{	8,	597,	81,	0,	0 },
	{	9,	597,	77,	0,	0 },
	{	10,	597,	73,	0,	0 },
	{	11,	597,	69,	0,	0 },
	{	12,	597,	65,	0,	0 },
	{	13,	597,	62,	0,	0 },
	{	14,	597,	59,	0,	0 },
	{	15,	597,	56,	0,	0 },
	{	16,	597,	54,	0,	0 },
	{	17,	597,	51,	0,	0 },
	{	18,	597,	49,	0,	0 },
	{	19,	597,	45,	0,	0 },
	{	20,	597,	41,	0,	0 },
	{	21,	597,	37,	0,	0 },
	{	22,	597,	33,	0,	0 },
	{	23,	597,	30,	0,	0 },
	{	24,	597,	27,	0,	0 },
	{	25,	597,	24,	0,	0 },
	{	26,	597,	22,	0,	0 },
	{	27,	597,	19,	0,	0 },
	{	28,	597,	17,	0,	0 },
	{	29,	597,	15,	0,	0 },
	{	30,	597,	13,	0,	0 },
	{	31,	597,	11,	0,	0 },
	{	32,	597,	9,	0,	0 },
	{	33,	597,	7,	0,	0 },
	{	34,	597,	6,	0,	0 },
	{	35,	597,	4,	0,	0 },
	{	36,	597,	3,	0,	0 },
	{	37,	597,	2,	0,	0 },
	{	38,	597,	1,	0,	0 },
	{	39,	598,	0,	0,	0 },
	{	40,	1191,	12,	0,	0 },
	{	41,	1191,	11,	0,	0 },
	{	42,	1191,	9,	0,	0 },
	{	43,	1191,	7,	0,	0 },
	{	44,	1191,	6,	0,	0 },
	{	45,	1191,	4,	0,	0 },
	{	46,	1191,	3,	0,	0 },
	{	47,	1191,	2,	0,	0 },
	{	48,	1191,	1,	0,	0 },
	{	49,	1199,	0,	0,	0 },
	{	50,	1286,	0,	0,	0 },
	{	51,	1375,	0,	0,	0 },
	{	52,	1477,	0,	0,	0 },
	{	53,	1580,	0,	0,	0 },
	{	54,	1699,	0,	0,	0 },
	{	55,	1818,	0,	0,	0 },
	{	56,	1954,	0,	0,	0 },
	{	57,	2087,	0,	0,	0 },
	{	58,	2239,	0,	0,	0 },
	{	59,	2398,	0,	0,	0 },
	{	60,	2581,	0,	0,	0 },
	{	61,	2749,	0,	0,	0 },
	{	62,	2966,	0,	0,	0 },
	{	63,	3160,	0,	0,	0 },
	{	64,	3415,	0,	0,	0 },
	{	65,	3636,	0,	0,	0 },
	{	66,	3932,	0,	0,	0 },
	{	67,	4174,	0,	0,	0 },
	{	68,	4508,	0,	0,	0 },
	{	69,	4830,	0,	0,	0 },
	{	70,	5202,	0,	0,	0 },
	{	71,	5543,	0,	0,	0 },
	{	72,	5932,	0,	0,	0 },
	{	73,	6380,	0,	0,	0 },
	{	74,	6901,	0,	0,	0 },
	{	75,	7351,	0,	0,	0 },
	{	76,	7863,	0,	0,	0 },
	{	77,	8453,	0,	0,	0 },
	{	78,	9139,	0,	0,	1 },
	{	79,	9661,	0,	0,	0 },
	{	80,	10566,	0,	0,	0 },
	{	81,	11271,	0,	0,	0 },
	{	82,	12076,	0,	0,	1 },
	{	83,	13005,	0,	0,	0 },
	{	84,	14089,	0,	0,	0 },
	{	85,	14701,	0,	0,	0 },
	{	86,	16101,	0,	0,	0 },
	{	87,	16906,	0,	0,	0 },
	{	88,	18785,	0,	0,	0 },
	{	89,	19890,	0,	0,	1 },
	{	90,	21133,	0,	0,	1 },
	{	91,	22542,	0,	0,	0 },
	{	92,	24152,	0,	0,	1 },
	{	93,	26010,	0,	0,	1 },
	{	94,	28177,	0,	0,	0 },
	{	95,	30739,	0,	0,	1 },
	{	96,	33812,	1,	0,	2 },
	{	97,	33812,	0,	0,	2 },
	{	98,	37569,	0,	0,	0 },
	{	99,	42265,	1,	0,	1 },
	{	100,	42265,	0,	0,	1 },
	{	101,	48303,	1,	0,	3 },
	{	102,	48303,	0,	0,	3 },
	{	103,	56354,	1,	0,	0 },
	{	104,	56354,	0,	0,	0 },
	{	105,	67624,	2,	0,	2 },
	{	106,	67624,	1,	0,	2 },
	{	107,	67624,	0,	0,	2 },
	{	108,	84530,	2,	0,	5 },
	{	109,	84530,	1,	0,	5 },
	{	110,	84530,	0,	0,	5 },
	{	111,	112706,	4,	0,	0 },
	{	112,	112706,	3,	0,	0 },
	{	113,	112706,	1,	0,	0 },
	{	114,	112706,	0,	0,	0 },
	{	115,	169056,	6,	0,	5 },
	{	116,	169056,	5,	0,	5 },
	{	117,	169056,	4,	0,	5 },
	{	118,	169056,	2,	0,	5 },
	{	119,	169056,	1,	0,	5 },
	{	120,	169056,	0,	0,	5 },
	{	121,	338094,	13,	0,	0 },
	{	122,	338094,	12,	0,	0 },
	{	123,	338094,	10,	0,	0 },
	{	124,	338094,	8,	0,	0 },
	{	125,	338094,	6,	0,	0 },
	{	126,	338094,	5,	0,	0 },
	{	127,	338094,	4,	0,	0 },
	{	128,	338094,	2,	0,	0 },
	{	129,	338094,	1,	0,	0 },
	{	130,	338094,	0,	0,	0 },
	};

/* gain table. */
static code UINT16 gainTbl[121] = {
	16, 	17, 	18, 	19, 	20, 	21, 	22, 	23, 	24, 	25, 
	26, 	27, 	28, 	29, 	30, 	31, 	32, 	33, 	34, 	35, 
	36, 	37, 	38, 	39, 	40, 	41, 	42, 	43, 	44, 	45, 
	46, 	47, 	48, 	49, 	50, 	51, 	52, 	53, 	54, 	55, 
	56, 	57, 	58, 	59, 	60, 	61, 	62, 	63, 	96, 	97, 
	98, 	99, 	100, 	101, 	102, 	103, 	104, 	105, 	106, 	107, 
	108, 	109, 	110, 	111, 	112, 	113, 	114, 	115, 	116, 	117, 
	118, 	119, 	120, 	121, 	122, 	123, 	124, 	125, 	126, 	127, 
	377, 	378, 	379, 	380, 	381, 	382, 	383, 	633, 	634, 	635, 
	636, 	637, 	638, 	639, 	890, 	891, 	892, 	893, 	894, 	895, 
	1146, 	1147, 	1148, 	1149, 	1150, 	1151, 	1403, 	1404, 	1405, 	1406, 
	1407, 	1659, 	1660, 	1661, 	1662, 	1663, 	1915, 	1916, 	1917, 	1918, 
	1919, 	
};			

/* exposure time index table. */
static code UINT8 ae20fpsMaxIdxTbl[2] = {
	AE_20FPS_30_MAX_IDX,
	AE_20FPS_25_MAX_IDX,
};

static code UINT8 ae20fpsMinIdxTbl[2] = {
	AE_20FPS_30_MIN_IDX,
	AE_20FPS_25_MIN_IDX,
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
	AE_60FPS_30_MAX_IDX,
	AE_60FPS_25_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_30_MIN_IDX,
	AE_60FPS_25_MIN_IDX,
};

/* device capability. */
static code frontPrevCapabDesc_t frontPrevCapabDesc[PREV_MODE_TOT] = {
	{  /* VGA 30fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_20FPS_PREV,
		HEIGHT_20FPS_PREV,
		XOFF_20FPS_PREV,
		YOFF_20FPS_PREV,
		LINE_TOT_20FPS_PREV,
		FRAME_TOT_20FPS_PREV,
		MCLK_DIV_20FPS_PREV,
		MCLK_PHASE_20FPS_PREV,
		PCLK_DIV_20FPS_PREV,
		PCLK_PHASE_20FPS_PREV,
		PCLK_FREQ_20FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_20FPS_PREV,
		RESHAPE_HFALL_20FPS_PREV,
		RESHAPE_HRISE_20FPS_PREV,
		RESHAPE_VEN_20FPS_PREV,
		RESHAPE_VFALL_20FPS_PREV,
		RESHAPE_VRISE_20FPS_PREV,
		ae20fps60hzTbl,
		ae20fps50hzTbl,
		ae20fpsMaxIdxTbl,
		ae20fpsMinIdxTbl,
		EV10_20FPS_EXP_IDX,
		gainTbl,
		GAIN_20FPS_MAX_IDX,
		GAIN_20FPS_MIN_IDX,
	},	
	{  /* HD 30fps. */
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
	{  /* VGA 60fps. */
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
	0x0D, 0x00, 0x01,//reset
};

static code UINT8 frontInit1CmdTbl[] = {
	0x0D, 0x00, 0x00,
#if 1		
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x10, 0x00, 0x50,/* PLL_CONTROL */
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3f, 0x03,/* PLL_CONTROL_1 *///pclk=2.1*mclk
	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif
#if 0
	0x10, 0x00, 0x50,//bypass pll
#endif
#if 1//720*540
	0x01, 0x00, 0x26,//Row Start = 38          
	0x02, 0x00, 0x10,//Column Start = 16       
	0x03, 0x04, 0x4f,//Row Size = 1103        
	0x04, 0x05, 0xaf,//Column Size = 1455      
	0x05, 0x01, 0x61,//Horz. Blank = 353     
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 759       
#endif	
#if 0//1280*720
	0x01, 0x00, 0x20,//Row Start = 32      
	0x02, 0x00, 0x10,//Column Start = 16       
	0x03, 0x02, 0xff,//Row Size = 767
	0x04, 0x05, 0x0f,//Column Size = 1295   
	0x05, 0x01, 0x62,//Horz. Blank = 354
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 8    
#endif	

	0x20, 0xc0, 0x47, /* READ_MODE_2 */ //mirror 
	0x41, 0x00, 0x00, /* CLAMP_VOLTAGE */ 
	0x42, 0x00, 0x03, /* ADC_REF_LO_VOLTAGE */ 
	0x43, 0x00, 0x03, /* ADC_REF_HI_VOLTAGE */ 
	0x7F, 0x00, 0x00, /* RD_EN_VLN_SH_TIMING */ 

	0x1E, 0xc0, 0x06, /*Set parallel mode *//*MT9M002-REV2 */
	0x9F, 0x30, 0x70, /*Parallel data and clock out */ 
	0x9E, 0x11, 0x1E, /*FV_LV timing adjustment */ 
	0x0B, 0x00, 0x01,  /*restart */ 
};

static code UINT8 frontInit2CmdTbl[] = {
	0x0B, 0x00, 0x00, /*restart */ 
};

static code UINT8 front20fpsPrevCmdTbl[] = {

#if 1
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
//	0x10, 0x00, 0x50,/* PLL_CONTROL */
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x05,/* PLL_CONTROL_1 */
//	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

#if 1//720*540
	0x01, 0x00, 0x26,//Row Start = 38          
	0x02, 0x00, 0x10,//Column Start = 16       
	0x03, 0x04, 0x4f,//Row Size = 1103        
	0x04, 0x05, 0xaf,//Column Size = 1455      
	0x05, 0x01, 0x61,//Horz. Blank = 353     
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 759       
#endif

#if 0
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x10, 0x00, 0x50,/* PLL_CONTROL */
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x05,/* PLL_CONTROL_1 */
	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

	0x22, 0x00, 0x01,        	//Row Mode = 0x1       skip2x          
	0x23, 0x00, 0x01,        	//Column Mode = 0x1   skip2x      

};

static code UINT8 front30fpsPrevCmdTbl[] = {

#if 1
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
//	0x10, 0x00, 0x50,/* PLL_CONTROL */
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x4b, 0x02,/* PLL_CONTROL_1 */
//	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

#if 1//1280*960
	0x01, 0x00, 0x76,//Row Start = 32      
	0x02, 0x00, 0x4c,//Column Start = 16       
	0x03, 0x03, 0xCA,//Row Size = 767
	0x04, 0x05, 0x0f,//Column Size = 1295   
	0x05, 0x01, 0x62,//Horz. Blank = 354
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 8    
#endif	

#if 0
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x10, 0x00, 0x50,/* PLL_CONTROL */
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x4b, 0x02,/* PLL_CONTROL_1 */
	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

	0x22, 0x00, 0x00,        	//Row Mode = 0x1       skip2x          
	0x23, 0x00, 0x00,       	//Column Mode = 0x1   skip2x      

};

static code UINT8 front60fpsPrevCmdTbl[] = {

#if 1
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
//	0x10, 0x00, 0x50,/* PLL_CONTROL */
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x02,/* PLL_CONTROL_1 */
//	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

#if 1//720*540
	0x01, 0x00, 0x26,//Row Start = 38          
	0x02, 0x00, 0x10,//Column Start = 16       
	0x03, 0x04, 0x4f,//Row Size = 1103        
	0x04, 0x05, 0xaf,//Column Size = 1455      
	0x05, 0x01, 0x61,//Horz. Blank = 353     
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 759       
#endif

#if 0
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x10, 0x00, 0x50,/* PLL_CONTROL */
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x02,/* PLL_CONTROL_1 */
	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

	0x22, 0x00, 0x01,        	//Row Mode = 0x1       skip2x          
	0x23, 0x00, 0x01,        	//Column Mode = 0x1   skip2x      

};

static code UINT8 frontSnapCmdTbl[] = {

#if 1
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
//	0x10, 0x00, 0x50,/* PLL_CONTROL */
//	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x02,/* PLL_CONTROL_1 */
//	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

#if 1//720*540
	0x01, 0x00, 0x26,//Row Start = 38          
	0x02, 0x00, 0x10,//Column Start = 16       
	0x03, 0x04, 0x4f,//Row Size = 1103        
	0x04, 0x05, 0xaf,//Column Size = 1455      
	0x05, 0x01, 0x61,//Horz. Blank = 353     
	0x06, 0x00, 0x08,//0xf7,//Vert. Blank = 759       
#endif

#if 0
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x10, 0x00, 0x50,/* PLL_CONTROL */
	0x10, 0x00, 0x51,/* PLL_CONTROL */
	0x11, 0x3d, 0x02,/* PLL_CONTROL_1 */
	0x10, 0x00, 0x53,/* PLL_CONTROL */
#endif

	0x22, 0x00, 0x00,        	//Row Mode = 0x0            
	0x23, 0x00, 0x00,        	//Column Mode = 0x0         

};

static UINT8 frontPrevMode = 1;
static UINT8 frontSnapMode = 0;

static UINT16 frontLastGain;
static UINT16 frontLastGainIdx;
static UINT32 frontLastShutter;
static UINT16 frontSnapLastGain;
static UINT32 frontSnapLastShutter;

static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;

static UINT8 frontPrevModeSave = 0xff;	/* resolve MI1600 sensor different resolution & fps switch issue used */


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

	return frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));
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
	switch ( fps ) 
		{
		case 24:
		case 20:
			printf("frontOpFrameRateSet = 24\n");
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
//	printf("@@@frontPrevMode=%bd\n",frontPrevMode);

	/* set i2c pin. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

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
	//i2cCmdTableSend(frontInit0CmdTbl, 2, sizeof(frontInit0CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(300);
	frontResCmdBinSend(_frontInit1CmdTbl);	
	//i2cCmdTableSend(frontInit1CmdTbl, 2, sizeof(frontInit1CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(100);
	frontResCmdBinSend(_frontInit2CmdTbl);	
	//i2cCmdTableSend(frontInit2CmdTbl, 2, sizeof(frontInit2CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
	UINT8 pvFreq, gIdval;
	SINT16 pv_gId = 0;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

//	printf("@@frontPrevMode@@=%bd\n",frontPrevMode);
	
	if(frontPrevModeSave != frontPrevMode)
	{
		/*sensor cmd send*/
		if(frontPrevMode == 0)
		{
			
			frontResCmdBinSend(_front20fpsPrevCmdTbl);
			//i2cCmdTableSend(front20fpsPrevCmdTbl, 2, sizeof(front20fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);	
		}
		if(frontPrevMode == 1)
		{
			frontResCmdBinSend(_front30fpsPrevCmdTbl);	
			//i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);	
		}
		if(frontPrevMode == 2)
		{
			frontResCmdBinSend(_front60fpsPrevCmdTbl);		
			//i2cCmdTableSend(front20fpsPrevCmdTbl, 2, sizeof(front20fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);	
		}	

		/*AE adjust for mode switch */
		sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
		sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
		pv_gId = gIdval;
	
		if(frontPrevModeSave == 0 && frontPrevMode == 1)
		{
			pv_gId -= 0;
		}
		else if(frontPrevModeSave == 1 && frontPrevMode == 0)
		{
			pv_gId += 0;
		}	

		if(pvFreq == SP1K_FLICKER_60)  pvFreq = 0;/*60Hz*/
		else	  pvFreq = 1;/*50Hz*/

		if(pv_gId < pcap->aeMinIdxTbl[pvFreq]) {
			pv_gId = pcap->aeMinIdxTbl[pvFreq];
		}
		if(pv_gId > pcap->aeMaxIdxTbl[pvFreq]) {
			pv_gId = pcap->aeMaxIdxTbl[pvFreq];
		}
		
		sp1kAeParamSet(SP1K_AE_gId,(UINT8)pv_gId);

		if (pvFreq == 0) {/*60Hz*/
			frontExposureTimeSet(0,pcap->ae60hzTbl[pv_gId].shutter,1,0);
			frontGainSet(0,pcap->ae60hzTbl[pv_gId].gain,1,0);
		}
		else {/*50Hz*/
			frontExposureTimeSet(0,pcap->ae50hzTbl[pv_gId].shutter,1,0);
			frontGainSet(0,pcap->ae50hzTbl[pv_gId].gain,1,0);
		}	
		frontPrevModeSave = frontPrevMode;

	}

	if(LoadBackfrontLastSetting)
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

	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);		
		frontResCmdBinSend(_frontSnapCmdTbl);
		//i2cCmdTableSend(frontSnapCmdTbl, 2, sizeof(frontSnapCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
  		frontSnapExposureTimeSet(mode,scaleUp);		 
  	 	frontPrevModeSave = 0xff;	
	} 	   
   	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo

	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);	

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	
    frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
    HAL_CdspEofWait(1);	   	
	   
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


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
//	printf("Gain set!\n");
	cmdTbl[0] = 0x35;
	cmdTbl[1] = frontLastGain>>8;
	cmdTbl[2] = frontLastGain&0xff;
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);

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
	UINT8 cmdTbl[3*1];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);


	cmdTbl[0] = 0x09;
	cmdTbl[1] = espline >> 8;
	cmdTbl[2] = espline & 0xFF;
	
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
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
	//printf("Intr Gain set!\n");

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];

    
	cmdTbl[0] = 0x35;
	cmdTbl[1] = frontLastGain>>8;
	cmdTbl[2] = frontLastGain&0xff;
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);

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
	UINT8 cmdTbl[3*1];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

	cmdTbl[0] = 0x09;
	cmdTbl[1] = espline >> 8;
	cmdTbl[2] = espline & 0xFF;		
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
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
	UINT8 cmdTbl[3*2];
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

	espline = (pixclk * 10) / (ltotal * frontSnapLastShutter);
	
	s_gain = frontSnapLastGain;

	#if 0
	if ( (s_gain > 63)&&(s_gain <= 127) ) {
		lastt_gain = s_gain - 64;
		espline = espline * 2;
	}
	else {
	#else
	{
	#endif
		lastt_gain = s_gain;
		espline = espline;
	}

	cdspColorMatrixRestore();

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
	}
	else {
		tmp0 = 0;
	};

	cmdTbl[0] = 0x09;
	cmdTbl[1] = espline >> 8;
	cmdTbl[2] = espline & 0xFF;
	cmdTbl[3] = 0x35;
	cmdTbl[4] = lastt_gain >> 8;
	cmdTbl[5] = lastt_gain & 0xFF;
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);  /*don't set gain , it's for FPGA verification */
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
	#if (SNAP_GAIN_TO_EXP == 1)
	UINT8 pvgId;
	#endif

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	#if (SNAP_GAIN_TO_EXP == 1)
	/* get pv gId */
	sp1kAeStatusGet(SP1K_AE_gId, &pvgId);
	printf("pvgId=%bu\n", pvgId);
	#endif

	/* get pv gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
	printf("pvGainIdxVal=%bu\n", pvGainIdxVal);

	/* get compensation gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
	//printf("compensationGainIdx=%bu\n", compensationGainIdx);

	#if (SNAP_GAIN_TO_EXP == 1)
	/* gain transfor exposure time */
/*	if(pvgId < 8)
	{
		frontSnapLastShutter = frontLastShutter/4;
		subGainIdx = 64;
	}
	else if (pvgId < 19) 
	{
		frontSnapLastShutter = frontLastShutter/2;
		subGainIdx = 32;
	}
	else {
		frontSnapLastShutter = frontLastShutter;
	}*/	
	if(pvGainIdxVal > 79)
	{
		frontSnapLastShutter = frontLastShutter/4;
		subGainIdx = 64;
	}
	else if (pvGainIdxVal > 47) 
	{
		frontSnapLastShutter = frontLastShutter/2;
		subGainIdx = 32;
	}
	else {
		frontSnapLastShutter = frontLastShutter;
	}
	#else
	frontSnapLastShutter = frontLastShutter;
	#endif
	//printf("frontLastShutter=%lu, frontSnapLastShutter=%lu\n",frontLastShutter,frontSnapLastShutter);

	/* calculate last gain */
	lastGainIdx = pvGainIdxVal + compensationGainIdx - subGainIdx;
	//printf("lastGainIdx=%bu\n", lastGainIdx);
	frontSnapLastGain = pcap->gainTbl[lastGainIdx];	


	/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
	LoadBackfrontLastSetting = 1;

#if (PV_AUTO_OB_BY_GAIN == 1)
	if (pvGainIdxVal == lastGainIdx) {		
		/* resolve ov9712 auto ob by gain issue used */
		if (pvGainIdxVal == pcap->gainMinIdx) {
			LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
		}
		else {//(pvGainIdxVal == pcap->gainMaxIdx)  & other case
			LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
		}
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
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_CMD_MAX_SIZE) {
		ASSERT(size > FRONT_RES_CMD_MAX_SIZE, 0); 
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
	UINT16 address
)
{
	UINT32 size;
	UINT8 sts;
	
	//printf("fileId = 0x%04x, address = 0x%04x\n",fileId,address);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(size > FRONT_RES_TBL_MAX_SIZE, 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((address - 0x4000) >> 1)) << 1));
	}while(!sts);	
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

