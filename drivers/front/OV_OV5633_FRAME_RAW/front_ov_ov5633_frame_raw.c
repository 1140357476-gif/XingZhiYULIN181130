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
 * @file		front_ov_ov5610_raw.c
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
#include "ae_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "hal_dram.h"
#include "sp1k_gpio_api.h"
#include "sp1k_ae_api.h"
#include "func_ptr.h"
#include "ms_cal.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define RES_LSC_LOAD		0//after power on load lsc resource file
 
/* mode. */
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1
#define PREV_RESP  960//960//720

/* dimensiion. */
#if (PREV_RESP==960) //960p

#define WIDTH_24FPS_PREV      1280
#define HEIGHT_24FPS_PREV     960
#define XOFF_24FPS_PREV       495
#define YOFF_24FPS_PREV       12
#define LINE_TOT_24FPS_PREV   1776
#define FRAME_TOT_24FPS_PREV  981

#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       495
#define YOFF_30FPS_PREV       12
#define LINE_TOT_30FPS_PREV   1776
#define FRAME_TOT_30FPS_PREV  981

#elif (PREV_RESP==720)//720p

#define WIDTH_24FPS_PREV      1280
#define HEIGHT_24FPS_PREV     720
#define XOFF_24FPS_PREV       836//695
#define YOFF_24FPS_PREV       9 
#define LINE_TOT_24FPS_PREV   2202
#define FRAME_TOT_24FPS_PREV  752

#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       836//695
#define YOFF_30FPS_PREV       9 
#define LINE_TOT_30FPS_PREV   2202
#define FRAME_TOT_30FPS_PREV  752
#endif

#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       910
#define YOFF_60FPS_PREV       9
#define LINE_TOT_60FPS_PREV   1550
#define FRAME_TOT_60FPS_PREV  516

#define WIDTH_SNAP      2592
#define HEIGHT_SNAP     1944
#define XOFF_SNAP       640
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

#define MCLK_DIV_SNAP    11
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    11
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
	#define PCLK_FREQ_24FPS_PREV  53000000L
	#define PCLK_FREQ_30FPS_PREV  53000000L
	#define PCLK_FREQ_60FPS_PREV  53000000L
	#define PCLK_FREQ_SNAP        53000000L
#else
	#define PCLK_FREQ_24FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
	#define PCLK_FREQ_30FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
	#define PCLK_FREQ_60FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
	#define PCLK_FREQ_SNAP        522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
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

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  22
#define RESHAPE_HRISE_SNAP  24
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GBBRGR
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
#define AE_24FPS_24_MAX_IDX  0
#define AE_24FPS_24_MIN_IDX  0
#define AE_24FPS_20_MAX_IDX  0
#define AE_24FPS_20_MIN_IDX  0
#define EV10_24FPS_EXP_IDX   100

#define AE_30FPS_30_MAX_IDX  129
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  125
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  0
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  0
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_24FPS_MAX_IDX  47
#define GAIN_24FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  47
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  47
#define GAIN_60FPS_MIN_IDX  0

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor

#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					//AE_GAIN_AFTER_EXP_OFF,
																//AE_GAIN_AFTER_EXP_ON

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

#define MS_DELAY      	(UINT16)280     /* unit 10us   Mechanical Shutter Delay */
#define MS_HOLD_TIME    (UINT16)1000    /* unit 10us   Mechanical Shutter enable Time */
#define MS_MIN_TIME     (UINT16)280     /* unit 10us   Minimum Shutter Time */

#define FLASHLIGHT_USE        0
#define MSHUTTER_CALIBRATION  0

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

static void frontBeforePowerOn(void);
static void frontAfterPowerOn(void);
static void frontBeforePowerOff(void);
static void frontAfterPowerOff(void);
static void frontSnapExposureTimeSet(void);
static void frontBeforeSnapExposureTimeSet(void);

static void OV5633FlashWBSet(void);


static void frontMSCfg();
static void frontFrameModeExpGainSet(UINT8 mode,UINT8 *frameModeEn,UINT16 *mShutterTime,UINT8 *flashEn);
static void frontSnapMSTimeGet(UINT16 *mSTime);
static void frontSnapMSControl(UINT8 sts);
static void frontMSExpControl(UINT16 MShutter_Time,UINT8 FlashEn);
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5633 FRAME RAW";

/* ae table. */
static code frontAe_t ae20fps50hzTbl[1] = {
	{	   0,	 0, 	0,		0,	0 },
};
static code frontAe_t ae20fps60hzTbl[1] = {
	{	   0,	 0, 	0,		0,	0 },
};
static code frontAe_t ae30fps50hzTbl[129] = {
	{	0,	332,	47,	0,	0 },
	{	1,	332,	45,	0,	0 },
	{	2,	332,	43,	0,	0 },
	{	3,	332,	42,	0,	0 },
	{	4,	332,	40,	0,	0 },
	{	5,	332,	38,	0,	0 },
	{	6,	332,	37,	0,	0 },
	{	7,	332,	35,	0,	0 },
	{	8,	332,	34,	0,	0 },
	{	9,	332,	33,	0,	0 },
	{	10,	332,	31,	0,	0 },
	{	11,	332,	29,	0,	0 },
	{	12,	332,	27,	0,	0 },
	{	13,	332,	26,	0,	0 },
	{	14,	332,	24,	0,	0 },
	{	15,	332,	22,	0,	0 },
	{	16,	332,	21,	0,	0 },
	{	17,	332,	19,	0,	0 },
	{	18,	332,	18,	0,	0 },
	{	19,	332,	17,	0,	0 },
	{	20,	332,	15,	0,	0 },
	{	21,	332,	13,	0,	0 },
	{	22,	332,	11,	0,	0 },
	{	23,	332,	10,	0,	0 },
	{	24,	332,	8,	0,	0 },
	{	25,	332,	6,	0,	0 },
	{	26,	332,	5,	0,	0 },
	{	27,	332,	3,	0,	0 },
	{	28,	332,	2,	0,	0 },
	{	29,	332,	1,	0,	0 },
	{	30,	332,	0,	0,	0 },
	{	31,	498,	6,	0,	0 },
	{	32,	498,	5,	0,	0 },
	{	33,	498,	3,	0,	0 },
	{	34,	498,	2,	0,	0 },
	{	35,	498,	1,	0,	0 },
	{	36,	498,	0,	0,	0 },
	{	37,	991,	13,	0,	0 },
	{	38,	991,	11,	0,	0 },
	{	39,	991,	9,	0,	0 },
	{	40,	991,	7,	0,	0 },
	{	41,	991,	6,	0,	0 },
	{	42,	991,	5,	0,	0 },
	{	43,	991,	3,	0,	0 },
	{	44,	991,	2,	0,	0 },
	{	45,	991,	1,	0,	0 },
	{	46,	991,	0,	0,	0 },
	{	47,	1058,	0,	0,	0 },
	{	48,	1135,	0,	0,	0 },
	{	49,	1213,	0,	0,	0 },
	{	50,	1303,	0,	0,	0 },
	{	51,	1395,	0,	0,	0 },
	{	52,	1500,	0,	0,	0 },
	{	53,	1604,	0,	0,	0 },
	{	54,	1725,	0,	0,	0 },
	{	55,	1842,	0,	0,	0 },
	{	56,	1976,	0,	0,	0 },
	{	57,	2116,	0,	0,	0 },
	{	58,	2278,	0,	0,	0 },
	{	59,	2426,	0,	0,	0 },
	{	60,	2618,	0,	0,	0 },
	{	61,	2789,	0,	0,	0 },
	{	62,	3014,	0,	0,	0 },
	{	63,	3209,	0,	0,	0 },
	{	64,	3470,	0,	0,	0 },
	{	65,	3684,	0,	0,	0 },
	{	66,	3979,	0,	0,	0 },
	{	67,	4263,	0,	0,	0 },
	{	68,	4591,	0,	0,	0 },
	{	69,	4892,	0,	0,	0 },
	{	70,	5235,	0,	0,	0 },
	{	71,	5631,	0,	0,	0 },
	{	72,	6090,	0,	0,	1 },
	{	73,	6487,	0,	0,	0 },
	{	74,	6940,	0,	0,	0 },
	{	75,	7461,	0,	0,	1 },
	{	76,	8065,	0,	0,	0 },
	{	77,	8526,	0,	0,	0 },
	{	78,	9326,	0,	0,	1 },
	{	79,	9947,	0,	0,	0 },
	{	80,	10658,	0,	0,	0 },
	{	81,	11478,	0,	0,	0 },
	{	82,	12434,	0,	0,	0 },
	{	83,	12975,	0,	0,	1 },
	{	84,	14211,	0,	0,	1 },
	{	85,	14921,	0,	0,	2 },
	{	86,	16579,	0,	0,	2 },
	{	87,	17554,	0,	0,	1 },
	{	88,	18651,	0,	0,	2 },
	{	89,	19895,	0,	0,	0 },
	{	90,	21316,	0,	0,	2 },
	{	91,	22955,	0,	0,	0 },
	{	92,	24868,	0,	0,	2 },
	{	93,	27129,	0,	0,	0 },
	{	94,	29842,	1,	0,	2 },
	{	95,	29842,	0,	0,	2 },
	{	96,	33158,	0,	0,	4 },
	{	97,	37302,	1,	0,	2 },
	{	98,	37302,	0,	0,	2 },
	{	99,	42631,	1,	0,	4 },
	{	100,	42631,	0,	0,	4 },
	{	101,	49736,	1,	0,	2 },
	{	102,	49736,	0,	0,	2 },
	{	103,	59683,	2,	0,	6 },
	{	104,	59683,	1,	0,	6 },
	{	105,	59683,	0,	0,	6 },
	{	106,	74604,	2,	0,	2 },
	{	107,	74604,	1,	0,	2 },
	{	108,	74604,	0,	0,	2 },
	{	109,	99471,	4,	0,	10 },
	{	110,	99471,	3,	0,	10 },
	{	111,	99471,	1,	0,	10 },
	{	112,	99471,	0,	0,	10 },
	{	113,	149204,	6,	0,	2 },
	{	114,	149204,	5,	0,	2 },
	{	115,	149204,	4,	0,	2 },
	{	116,	149204,	2,	0,	2 },
	{	117,	149204,	1,	0,	2 },
	{	118,	149204,	0,	0,	2 },
	{	119,	298394,	13,	0,	0 },
	{	120,	298394,	12,	0,	0 },
	{	121,	298394,	10,	0,	0 },
	{	122,	298394,	8,	0,	0 },
	{	123,	298394,	6,	0,	0 },
	{	124,	298394,	5,	0,	0 },
	{	125,	298394,	4,	0,	0 },
	{	126,	298394,	2,	0,	0 },
	{	127,	298394,	1,	0,	0 },
	{	128,	298394,	0,	0,	0 },
	};

static code frontAe_t ae30fps60hzTbl[130] = {
	{	0,	299,	46,	0,	0 },
	{	1,	299,	44,	0,	0 },
	{	2,	299,	42,	0,	0 },
	{	3,	299,	41,	0,	0 },
	{	4,	299,	39,	0,	0 },
	{	5,	299,	38,	0,	0 },
	{	6,	299,	36,	0,	0 },
	{	7,	299,	35,	0,	0 },
	{	8,	299,	34,	0,	0 },
	{	9,	299,	32,	0,	0 },
	{	10,	299,	30,	0,	0 },
	{	11,	299,	28,	0,	0 },
	{	12,	299,	26,	0,	0 },
	{	13,	299,	25,	0,	0 },
	{	14,	299,	23,	0,	0 },
	{	15,	299,	22,	0,	0 },
	{	16,	299,	20,	0,	0 },
	{	17,	299,	19,	0,	0 },
	{	18,	299,	18,	0,	0 },
	{	19,	299,	16,	0,	0 },
	{	20,	299,	14,	0,	0 },
	{	21,	299,	12,	0,	0 },
	{	22,	299,	10,	0,	0 },
	{	23,	299,	9,	0,	0 },
	{	24,	299,	7,	0,	0 },
	{	25,	299,	6,	0,	0 },
	{	26,	299,	4,	0,	0 },
	{	27,	299,	3,	0,	0 },
	{	28,	299,	2,	0,	0 },
	{	29,	299,	0,	0,	0 },
	{	30,	399,	4,	0,	0 },
	{	31,	399,	3,	0,	0 },
	{	32,	399,	2,	0,	0 },
	{	33,	399,	1,	0,	0 },
	{	34,	401,	0,	0,	0 },
	{	35,	598,	6,	0,	0 },
	{	36,	598,	4,	0,	0 },
	{	37,	598,	3,	0,	0 },
	{	38,	598,	2,	0,	0 },
	{	39,	598,	0,	0,	0 },
	{	40,	1189,	14,	0,	0 },
	{	41,	1189,	12,	0,	0 },
	{	42,	1189,	10,	0,	0 },
	{	43,	1189,	8,	0,	0 },
	{	44,	1189,	7,	0,	0 },
	{	45,	1189,	5,	0,	0 },
	{	46,	1189,	4,	0,	0 },
	{	47,	1189,	3,	0,	0 },
	{	48,	1189,	1,	0,	0 },
	{	49,	1189,	0,	0,	0 },
	{	50,	1213,	0,	0,	0 },
	{	51,	1303,	0,	0,	0 },
	{	52,	1395,	0,	0,	0 },
	{	53,	1500,	0,	0,	0 },
	{	54,	1604,	0,	0,	0 },
	{	55,	1725,	0,	0,	0 },
	{	56,	1842,	0,	0,	0 },
	{	57,	1976,	0,	0,	0 },
	{	58,	2116,	0,	0,	0 },
	{	59,	2278,	0,	0,	0 },
	{	60,	2426,	0,	0,	0 },
	{	61,	2618,	0,	0,	0 },
	{	62,	2789,	0,	0,	0 },
	{	63,	3014,	0,	0,	0 },
	{	64,	3209,	0,	0,	0 },
	{	65,	3470,	0,	0,	0 },
	{	66,	3684,	0,	0,	0 },
	{	67,	3979,	0,	0,	0 },
	{	68,	4263,	0,	0,	0 },
	{	69,	4591,	0,	0,	0 },
	{	70,	4892,	0,	0,	0 },
	{	71,	5235,	0,	0,	0 },
	{	72,	5631,	0,	0,	0 },
	{	73,	6090,	0,	0,	1 },
	{	74,	6487,	0,	0,	0 },
	{	75,	6940,	0,	0,	0 },
	{	76,	7461,	0,	0,	1 },
	{	77,	8065,	0,	0,	0 },
	{	78,	8526,	0,	0,	0 },
	{	79,	9326,	0,	0,	1 },
	{	80,	9947,	0,	0,	0 },
	{	81,	10658,	0,	0,	0 },
	{	82,	11478,	0,	0,	0 },
	{	83,	12434,	0,	0,	0 },
	{	84,	12975,	0,	0,	1 },
	{	85,	14211,	0,	0,	1 },
	{	86,	14921,	0,	0,	2 },
	{	87,	16579,	0,	0,	2 },
	{	88,	17554,	0,	0,	1 },
	{	89,	18651,	0,	0,	2 },
	{	90,	19895,	0,	0,	0 },
	{	91,	21316,	0,	0,	2 },
	{	92,	22955,	0,	0,	0 },
	{	93,	24868,	0,	0,	2 },
	{	94,	27129,	0,	0,	0 },
	{	95,	29842,	1,	0,	2 },
	{	96,	29842,	0,	0,	2 },
	{	97,	33158,	0,	0,	4 },
	{	98,	37302,	1,	0,	2 },
	{	99,	37302,	0,	0,	2 },
	{	100,	42631,	1,	0,	4 },
	{	101,	42631,	0,	0,	4 },
	{	102,	49736,	1,	0,	2 },
	{	103,	49736,	0,	0,	2 },
	{	104,	59683,	2,	0,	6 },
	{	105,	59683,	1,	0,	6 },
	{	106,	59683,	0,	0,	6 },
	{	107,	74604,	2,	0,	2 },
	{	108,	74604,	1,	0,	2 },
	{	109,	74604,	0,	0,	2 },
	{	110,	99471,	4,	0,	10 },
	{	111,	99471,	3,	0,	10 },
	{	112,	99471,	1,	0,	10 },
	{	113,	99471,	0,	0,	10 },
	{	114,	149204,	6,	0,	2 },
	{	115,	149204,	5,	0,	2 },
	{	116,	149204,	4,	0,	2 },
	{	117,	149204,	2,	0,	2 },
	{	118,	149204,	1,	0,	2 },
	{	119,	149204,	0,	0,	2 },
	{	120,	298394,	13,	0,	0 },
	{	121,	298394,	12,	0,	0 },
	{	122,	298394,	10,	0,	0 },
	{	123,	298394,	8,	0,	0 },
	{	124,	298394,	6,	0,	0 },
	{	125,	298394,	5,	0,	0 },
	{	126,	298394,	4,	0,	0 },
	{	127,	298394,	2,	0,	0 },
	{	128,	298394,	1,	0,	0 },
	{	129,	298394,	0,	0,	0 },
};                            

static code frontAe_t ae60fps60hzTbl[1] = {
	{      0,    0,     0,      0,	0 },
};
static code frontAe_t ae60fps50hzTbl[1] = {
	{      0,    0,     0,      0,	0 },
};

/* gain table. */
#if 0
static code UINT16 gain24fpsTbl[48] = {
     0, 	 1,      2,	     3, 	 4, 	 5, 	 6, 	 7, 	 8, 	 9,
    10, 	11, 	12, 	13, 	14, 	15, 	16, 	17, 	18, 	19,
    20, 	21, 	22, 	23, 	24, 	25, 	26, 	27, 	28, 	29,
    30, 	31, 	48, 	49, 	50, 	51, 	52, 	53, 	54, 	55,
    56, 	57, 	58, 	59, 	60, 	61, 	62, 	63, 			 	
};
#endif

static code UINT16 gain30fpsTbl[48] = {
     0, 	 1,      2,	     3, 	 4, 	 5, 	 6, 	 7, 	 8, 	 9,
    10, 	11, 	12, 	13, 	14, 	15, 	16, 	17, 	18, 	19,
    20, 	21, 	22, 	23, 	24, 	25, 	26, 	27, 	28, 	29,
    30, 	31, 	48, 	49, 	50, 	51, 	52, 	53, 	54, 	55,
    56, 	57, 	58, 	59, 	60, 	61, 	62, 	63, 			 	
};		

#if 0
static code UINT16 gain60fpsTbl[48] = {
     0, 	 1,      2,	     3, 	 4, 	 5, 	 6, 	 7, 	 8, 	 9,
    10, 	11, 	12, 	13, 	14, 	15, 	16, 	17, 	18, 	19,
    20, 	21, 	22, 	23, 	24, 	25, 	26, 	27, 	28, 	29,
    30, 	31, 	48, 	49, 	50, 	51, 	52, 	53, 	54, 	55,
    56, 	57, 	58, 	59, 	60, 	61, 	62, 	63, 			 	
};	
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

static code frontShutter_t snapShutter50hzTbl[106] = {
	{	 332,  47 },
	{	 332,  45 },
	{	 332,  43 },
	{	 332,  42 },
	{	 332,  40 },
	{	 332,  38 },
	{	 332,  37 },
	{	 332,  35 },
	{	 332,  34 },
	{	 332,  33 },
	{	 332,  31 },
	{	 332,  29 },
	{	 332,  27 },
	{	 332,  26 },
	{	 332,  24 },
	{	 332,  22 },
	{	 332,  21 },
	{	 332,  19 },
	{	 332,  18 },
	{	 332,  17 },
	{	 332,  15 },
	{	 332,  13 },
	{	 332,  11 },
	{	 332,  10 },
	{	 332,	8 },
	{	 332,	6 },
	{	 332,	5 },
	{	 332,	3 },
	{	 332,	2 },
	{	 313,	0 },
	{	 332,	0 },
	{	 362,	0 },
	{	 380,	0 },
	{	 420,	0 },
	{	 443,	0 },
	{	 469,	0 },
	{	 498,	0 },
	{	 547,	0 },
	{	 588,	0 },
	{	 635,	0 },
	{	 690,	0 },
	{	 721,	0 },
	{	 755,	0 },
	{	 835,	0 },
	{	 881,	0 },
	{	 933,	0 },
	{	 991,	0 },
	{	1058,	0 },
	{	1135,	0 },
	{	1213,	0 },
	{	1303,	0 },
	{	1395,	0 },
	{	1500,	0 },
	{	1604,	0 },
	{	1725,	0 },
	{	1842,	0 },
	{	1976,	0 },
	{	2116,	0 },
	{	2278,	0 },
	{	2426,	0 },
	{	2618,	0 },
	{	2789,	0 },
	{	3014,	0 },
	{	3209,	0 },
	{	3470,	0 },
	{	3684,	0 },
	{	3979,	0 },
	{	4263,	0 },
	{	4591,	0 },
	{	4892,	0 },
	{	5235,	0 },
	{	5631,	0 },
	{	6090,	0 },
	{	6487,	0 },
	{	6940,	0 },
	{	7461,	0 },
	{	8065,	0 },
	{	8526,	0 },
	{	9326,	0 },
	{	9947,	0 },
	{  10658,	0 },
	{  11478,	0 },
	{  12434,	0 },
	{  12975,	0 },
	{  14211,	0 },
	{  14921,	0 },
	{  16579,	0 },
	{  17554,	0 },
	{  18651,	0 },
	{  19895,	0 },
	{  21316,	0 },
	{  22955,	0 },
	{  24868,	0 },
	{  27129,	0 },
	{  28087,	0 },
	{  29842,	0 },
	{  33158,	0 },
	{  35108,	0 },
	{  37302,	0 },
	{  40124,	0 },
	{  42631,	0 },
	{  46811,	0 },
	{  49736,	0 },
	{  53052,	0 },
	{  56173,	0 },
	{  59683,	0 },
/*	{  66315,	0 },
	{  70216,	0 },
	{  74604,	0 },
	{  79577,	0 },
	{  83765,	0 },
	{  93620,	0 },
	{  99471,	0 },
	{ 108512,	0 },
	{ 113679,	0 },
	{ 119363,	0 },
	{ 132626,	0 },
	{ 140428,	0 },
	{ 149204,	0 },
	{ 164631,	0 },
	{ 170511,	0 },
	{ 183627,	0 },
	{ 198929,	0 },
	{ 217014,	0 },
	{ 227347,	0 },
	{ 238715,	0 },
	{ 265239,	0 },
	{ 280841,	0 },
	{ 298394,	0 },
*/
};

static code frontShutter_t snapShutter60hzTbl[107] = {
 {    299,  46 },
 {    299,  44 },
 {    299,  42 },
 {    299,  41 },
 {    299,  39 },
 {    299,  38 },
 {    299,  36 },
 {    299,  35 },
 {    299,  34 },
 {    299,  32 },
 {    299,  30 },
 {    299,  28 },
 {    299,  26 },
 {    299,  25 },
 {    299,  23 },
 {    299,  22 },
 {    299,  20 },
 {    299,  19 },
 {    299,  18 },
 {    299,  16 },
 {    299,  14 },
 {    299,  12 },
 {    299,  10 },
 {    299,   9 },
 {    299,   7 },
 {    299,   6 },
 {    299,   4 },
 {    299,   3 },
 {    299,   2 },
 {    299,   0 },
 {    319,   0 },
 {    336,   0 },
 {    355,   0 },
 {    375,   0 },
 {    401,   0 },
 {    435,   0 },
 {    478,   0 },
 {    504,   0 },
 {    532,   0 },
 {    598,   0 },
 {    634,   0 },
 {    679,   0 },
 {    732,   0 },
 {    793,   0 },
 {    827,   0 },
 {    906,   0 },
 {    951,   0 },
 {   1001,   0 },
 {   1119,   0 },
 {   1189,   0 },
 {   1213,   0 },
 {   1303,   0 },
 {   1395,   0 },
 {   1500,   0 },
 {   1604,   0 },
 {   1725,   0 },
 {   1842,   0 },
 {   1976,   0 },
 {   2116,   0 },
 {   2278,   0 },
 {   2426,   0 },
 {   2618,   0 },
 {   2789,   0 },
 {   3014,   0 },
 {   3209,   0 },
 {   3470,   0 },
 {   3684,   0 },
 {   3979,   0 },
 {   4263,   0 },
 {   4591,   0 },
 {   4892,   0 },
 {   5235,   0 },
 {   5631,   0 },
 {   6090,   0 },
 {   6487,   0 },
 {   6940,   0 },
 {   7461,   0 },
 {   8065,   0 },
 {   8526,   0 },
 {   9326,   0 },
 {   9947,   0 },
 {  10658,   0 },
 {  11478,   0 },
 {  12434,   0 },
 {  12975,   0 },
 {  14211,   0 },
 {  14921,   0 },
 {  16579,   0 },
 {  17554,   0 },
 {  18651,   0 },
 {  19895,   0 },
 {  21316,   0 },
 {  22955,   0 },
 {  24868,   0 },
 {  27129,   0 },
 {  28087,   0 },
 {  29842,   0 },
 {  33158,   0 },
 {  35108,   0 },
 {  37302,   0 },
 {  40124,   0 },
 {  42631,   0 },
 {  46811,   0 },
 {  49736,   0 },
 {  53052,   0 },
 {  56173,   0 },
 {  59683,   0 },
/*
 {  66315,   0 },
 {  70216,   0 },
 {  74604,   0 },
 {  79577,   0 },
 {  83765,   0 },
 {  93620,   0 },
 {  99471,   0 },
 { 108512,   0 },
 { 113679,   0 },
 { 119363,   0 },
 { 132626,   0 },
 { 140428,   0 },
 { 149204,   0 },
 { 164631,   0 },
 { 170511,   0 },
 { 183627,   0 },
 { 198929,   0 },
 { 217014,   0 },
 { 227347,   0 },
 { 238715,   0 },
 { 265239,   0 },
 { 280841,   0 },
 { 298394,   0 },
*/
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
		ae20fps60hzTbl,
		ae20fps50hzTbl,
		ae24fpsMaxIdxTbl,
		ae24fpsMinIdxTbl,
		EV10_24FPS_EXP_IDX,
		gain30fpsTbl,
		GAIN_24FPS_MAX_IDX,
		GAIN_24FPS_MIN_IDX,
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
		gain30fpsTbl,
		GAIN_30FPS_MAX_IDX,
		GAIN_30FPS_MIN_IDX,
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
		gain30fpsTbl,
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
		MCLK_PHASE_SNAP,
		PCLK_DIV_SNAP,
		PCLK_PHASE_SNAP,
		PCLK_FREQ_SNAP,
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
	0x30, 0x11, 0x40, 			 							  
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

	/******* Frame Mode ******/
	0x30, 0x13, 0x00, 					  
	0x30, 0x02, 0xff, 					  
	0x30, 0x03, 0xff, 					  
	0x30, 0x89, 0x0a, 					  
	0x30, 0xb1, 0x3f, 					  
	0x30, 0xb2, 0x02, 					  
	0x30, 0xb4, 0x20, 					  
	0x30, 0xe4, 0x01, 					  
	0x30, 0xe7, 0x00, 					  

	#if 1
		0x30, 0xec, 0x28, 					  
		0x30, 0xf6, 0xb0, 					  
		0x35, 0x0e, 0x44, 					  	
	#endif

	0x30, 0xe9, 0x05, 					  
	0x33, 0x19, 0x22, 					  
	0x30, 0x1e, 0x00, 					  

	0x30, 0x1f, 0x40, 					  
	 	
	0x30, 0x93, 0x40,  
    0x30, 0x90, 0x62, 
	/***************************/
	
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

static UINT8 frontPrevMode = 1;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT16 frontLastGainIdx;
static UINT32 frontLastShutter;
static UINT16 frontSnapLastGain;
static UINT32 frontSnapLastShutter;

static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;

static UINT8 frontPrevModeSave = 0;	/* resolve ov9712 sensor different resolution & fps switch issue used */


#define ST1155A_ENA1(x)	//{sp1kSHT_ACfg();sp1kSHT_ASet(x);}
#define ST1155A_IN1(x)	//{sp1kSHT_BCfg();sp1kSHT_BSet(x); }

#if 0
extern UINT8 calibrationObData[];
extern UINT8 G_AETblSel;
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
	switch ( fps ) {
	case 24:
		frontPrevMode = 1;
		//frontSnapMode = 0;
		break;
	case 30:
		frontPrevMode = 1;
		//frontSnapMode = 0;
		break;
	case 60:
		frontPrevMode = 1;
		//frontSnapMode = 0;
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
	i2cCmdTableSend(frontInit0CmdTbl, 2, sizeof(frontInit0CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(10);

	i2cCmdTableSend(frontInit1CmdTbl, 2, sizeof(frontInit1CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
    globalTimerWait(1);

	frontMSCfg();
	sp1kFrexSet(0);
	
	/* after power on. */
	frontAfterPowerOn();

	frontSnapMethodSet(SNAP_MS_NORMAL);
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
	//UINT8 pvFreq, gIdval;
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

	if ( frontPrevMode == 0 ) {  /* 24 fps. */
		i2cCmdTableSend(front24fpsPrevCmdTbl, 2, sizeof(front24fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
		//printf("frontOpPreviewModeSet <<24FPS>>\n");
	}
	else if ( frontPrevMode == 1 ) {  /* 30 fps. */
		i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
		//printf("frontOpPreviewModeSet <<30FPS>>\n");
	}
	else {// if ( frontPrevMode == 2 ) {  /* 60 fps. */
		i2cCmdTableSend(front60fpsPrevCmdTbl, 2, sizeof(front60fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
		//printf("frontOpPreviewModeSet <<60FPS>>\n");
	}

	/* resolve ov5633 sensor different resolution & fps switch issue */
	if (frontPrevModeSave != frontPrevMode) {//(frameRateSetFlag == 1) {
		#if 0
		sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
		sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
		//printf("pvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);

		pv_gId = gIdval;
		
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

		//printf("check pv_gId = %d, *pcap->aeMinIdxTbl = %bu, *pcap->aeMaxIdxTbl = %bu\n",pv_gId, *pcap->aeMinIdxTbl, *pcap->aeMaxIdxTbl);

		if(pv_gId < *pcap->aeMinIdxTbl) {
			pv_gId = *pcap->aeMinIdxTbl;
			//printf("pv_gId = %d < min\n",pv_gId);
		}

		if(pv_gId > *pcap->aeMaxIdxTbl) {
			pv_gId = *pcap->aeMaxIdxTbl;
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
		#endif
		frontPrevModeSave = frontPrevMode;
	}
	else {
		/* resolve ov5633 auto ob by gain issue & snap to pv sensor shutter need re-setting */
		if (LoadBackfrontLastSetting == 1) {
			frontExposureTimeSet(0,frontLastShutter,1,0);
			frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontGainSet(0,frontLastGainIdx,1,0);
			LoadBackfrontLastSetting = 0;
		}
		else {
			/* resolve ov5633 pv to video mode switch panel dumping */
			frontExposureTimeSet(0,frontLastShutter,1,0);
			frontGainSet(0,frontLastGainIdx,1,0);
			//printf("frontLastShutter=%lu, frontLastGainIdx=%bu\n",frontLastShutter,frontLastGainIdx);
		}
	}

	/* resolve ov5633 auto ob by gain issue & snap to pv sensor shutter need re-setting */
	if (LoadBackfrontLastSetting == 1) {
		frontExposureTimeSet(0,frontLastShutter,1,0);
		frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
		LoadBackfrontLastSetting = 0;
	}

	cdspBayerPatternSet(BAYER_PTN_PREV);

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
	UINT16 mShutterTime;		
	UINT8 frameModeEn,flashEn;	

	mode=mode;
	scaleUp=scaleUp;
	
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		i2cCmdTableSend(frontSnapCmdTbl, 2, sizeof(frontSnapCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontFrameModeExpGainSet(frontSnapMethodGet(),&frameModeEn,&mShutterTime,&flashEn);
	}
	
	//printf("MSTime=%ld\n",mShutterTime); 
	if (frameModeEn==0) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1);

		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		
		frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
		HAL_CdspEofWait(1); 	 		
	}
	else{		
		
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1); // 20081024 mantis 32544 wenguo
		frontReset();  /* workaround, reset front state machine to avoid valid error */
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1);
	
		frontMSExpControl(mShutterTime,flashEn);		 

		frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
		HAL_CdspEofWait(1); 
		
		if(frontSnapMethodGet()==SNAP_MS_DARK){	
			frontDarkSubCtrl(0,0,0);
		}
		frontOpShutterOpen();				
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

	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

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

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

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
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set val=%bu\n",val);

	//printf("gain set frontLastGain=%d\n",frontLastGain);

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
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

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
	frontSnapMSControl(MS_OPEN);
	HAL_Front10UsWait(MS_HOLD_TIME);
	frontSnapMSControl(MS_IDLE);
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
	//frontSnapMSControl(MS_CLOSE);
	//HAL_Front10UsWait(MS_HOLD_TIME);
	//frontSnapMSControl(MS_IDLE);	
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
	frontResTblCmdSend(FRONT_RES_PARAM_LSC_ID);
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
	void
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
	pixclk = pcap->pclkFreq;


	espline = (pixclk * 10) / (ltotal * frontSnapLastShutter);
	s_gain = frontSnapLastGain;

	//printf("espline=0x%x,gain=0x%bx\n",espline,s_gain);
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
 
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3-1, I2C_TRANS_MODE_NORMAL, 0, 0);
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
	frontSnapLastShutter = frontLastShutter;
	frontSnapLastGain = frontLastGain;
	LoadBackfrontLastGainIdx = frontLastGain;
	/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
	LoadBackfrontLastSetting = 1;
}

#if (RES_LSC_LOAD == 1)
static void
frontResTblCmdSend(
	UINT8 fileId
)
{
	UINT8 cmdTbl[(FRONT_RES_PARAM_MAX_SIZE + 1) & ~0x01] = {0};			
	UINT16 i;
	UINT8 sts, LSB, MSB;
	UINT32 size;

	//printf("fileId = 0x%bx\n",fileId);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_PARAM_MAX_SIZE) {
		return;
	}

	//printf("size = %lu\n",size);
	
	do {
		sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
	}while(!sts);

	HAL_DramDmaRead(&cmdTbl[0], K_SDRAM_CommonEndAddr, size, 1);
	//for (i = 0; i < size; i++) {
	//	printf("cmdTbl[%d] = 0x%bx\n", i, cmdTbl[i]);
	//}
	
	i2cCmdTableSend(cmdTbl, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	
}
#endif

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


static void 
frontMSCfg(
	void
)
{
	//UINT8 val=0;
	sp1kFrexCfg();
	sp1kSHT_ACfg();
	sp1kSHT_BCfg();	
	//HAL_GlobalTimer2Config(0xffff,0,0,sp1kFrexSet_Intr,&val);
}

static void
frontFrameModeExpGainSet(
	UINT8 mode,
	UINT8 *frameModeEn,
	UINT16 *mShutterTime,
	UINT8 *flashEn
)
{
	UINT8 freq,gIdVal;
	
	frontSnapMSTimeGet(mShutterTime);	
	*flashEn=0;
	
	if(mode == SNAP_MSCAL_FRAME) {  // Mechnical Shutter Calibration Frame Mode
		*frameModeEn=1;		  
	
		#if MSHUTTER_CALIBRATION
			*mShutterTime=CalMS_ExpTime;
		#endif
		 
		//frontOpGainSet(0,0,0);  // set gain =1
	 }
	 else if (mode == SNAP_MSCAL_ROLLING) {  // Mechnical Shutter Calibration Rolling Mode
		*frameModeEn=0;
		frontSnapExposureTimeSet();
	 }
	 else if (*mShutterTime>MS_MIN_TIME){  //Frame Mode
		*frameModeEn=1;						
		sp1kAeStatusGet(SP1K_AE_G_AETblSel,&freq);
		sp1kAeStatusGet(SP1K_AE_gId, &gIdVal); 		 
		 
		#if FLASHLIGHT_USE 
			frontFlashExpControl(flashEn,mShutterTime); 
		#endif
		
		if (freq == SP1K_FLICKER_50 ) 
			frontOpGainSet(snapShutter50hzTbl[gIdVal].GainIdx,0,0);   // set gain				
		else
			frontOpGainSet(snapShutter60hzTbl[gIdVal].GainIdx,0,0);   // set gain
														 
	}
	else { 					//Rolling Mode	  
		*frameModeEn=0;
		frontSnapExposureTimeSet();
	}		 

}

static void 
frontSnapMSTimeGet(
	UINT16 *mSTime
)
{
    UINT16 value;
	UINT8 freq;
	UINT8 gIdVal;
	
	sp1kAeStatusGet(SP1K_AE_G_AETblSel,&freq);
	sp1kAeStatusGet(SP1K_AE_gId, &gIdVal);
	
	if ( freq == SP1K_FLICKER_50 ) {
		value = snapShutter50hzTbl[gIdVal].Shutter;
	}
	else {
		value = snapShutter60hzTbl[gIdVal].Shutter;
	}	
	
	*mSTime =(UINT16)(10*100000/value);   /*unit 10us*/
	
    //printf("idx=%bx,value=%x,time=%x \n",gIdVal,value,*msTime);	
}


static void 
frontSnapMSControl(
	UINT8 sts
)
{
	
	/*Motor Driver ST1155A*/
	switch (sts){
	  case MS_CLOSE:	  		
		  sp1kSHT_BSet(1);	  //SHT Close
		  sp1kSHT_ASet(1);	  //enable	  	  	
   	  break;		
	  case MS_OPEN:
		  sp1kSHT_BSet(0);	  //SHT Open
		  sp1kSHT_ASet(1);	  //enable
	  break;
	  case MS_IDLE:	  	  
          sp1kSHT_ASet(0);	  //disable			  	
	  break;  
	}
}


static void 
frontMSExpControl(
	UINT16 time,
	UINT8 flashEn
)
{
#if 1
    //UINT8 sts;
	UINT32 mShutterTime;
	UINT16 mShutterDelay;
	UINT16 timeMSClose;
    float FrameLen;
	
	//ENTER_CRITICAL(sts);
	//sp1kFrexCfg();
	
	//Time=3000;
   	//printf("Sh Time=%d\n",Time);
   	
	if (frontSnapMethodGet()==SNAP_MSCAL_FRAME){
		#ifdef MS_CALIB_ENABLE
			msCalShutterParGet(&mShutterTime,&mShutterDelay);
		#endif
	}
	else{
	    mShutterTime=time;
		mShutterDelay=MS_DELAY;		
	}
	
	//printf("ms time=%ld delay=%d\n",mShutterTime,mShutterDelay);	

	if (mShutterTime< mShutterDelay){		  

		FrameLen=100000/(float)PCLK_FREQ_SNAP;
		FrameLen=FrameLen*LINE_TOT_SNAP*FRAME_TOT_SNAP;

		//FrameLen-=32;	 //  overhead is 380us

		FrameLen-=(mShutterDelay-mShutterTime);
		timeMSClose=(UINT16)(mShutterDelay-mShutterTime);
		/*
		if (timeMSClose>20){ 
			timeMSClose-=20;
		}
		else{         
			timeMSClose=1;
		}
		
		if (mShutterTime>20){ 
			mShutterTime-=20;
		}
		else{               
			mShutterTime=1;                 
		}
		*/		
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		
		HAL_Front10UsWait((UINT16)FrameLen);		  // VD Rising
	    frontSnapMSControl(MS_CLOSE);
		
		HAL_Front10UsWait(timeMSClose);
		
		sp1kFrexSet(1); 			 //FREX High	 
		
		HAL_Front10UsWait(mShutterTime);
		
		sp1kFrexSet(0);
		HAL_GlobalTimerWait10us(MS_HOLD_TIME-mShutterDelay);
		
		frontSnapMSControl(MS_IDLE);
	}
	else{		  
		timeMSClose=(UINT16)(mShutterTime-mShutterDelay);			
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		if(frontSnapMethodGet()==SNAP_MS_DARK){
			HAL_FrontSnapTrig(2,1);    /* snap trigger, 2 raw data */
		}

        if (flashEn){		  
		    #if FLASHLIGHT_USE		  
	  		    if(sp1kSTChgRdyGet()!=0x00){
			       sp1kSTChgEnSet(0);	
			       sp1kST_TrigSet(1);
			    }		  			  
		    #endif  
		}
		
	    sp1kFrexSet(1); 			 //FREX High	 
		
	    HAL_Front10UsWait(timeMSClose);
	    frontSnapMSControl(MS_CLOSE);  

	    HAL_Front10UsWait(mShutterDelay);
        sp1kFrexSet(0); 					//FREX Low				
	
	    HAL_Front10UsWait(MS_HOLD_TIME-mShutterDelay);  // Mechnical Shutter HoldTime > Mechnical Shutter Dealy								
	
	    frontSnapMSControl(MS_IDLE);
 
		if (flashEn){		  
		    #if FLASHLIGHT_USE	
  		      sp1kST_TrigSet(0);
		      sp1kSTChgEnSet(1);  /* charge enable should be controlled by host */		  	  		  	  
		    #endif
		}

		if(frontSnapMethodGet()==SNAP_MS_DARK){
			frontDarkSubCtrl(1,FRONT_WAIT_RISING,mShutterTime);
		}
		
	}
	
	//EXIT_CRITICAL(sts);
#endif	
}
