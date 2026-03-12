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
 * @file		front_mi_mi1320_raw.c
 * @brief		front sensor micron mi5100 driver.
 * @author		Matt Wang
 * @since		2008-09-24
 * @date		2008-11-13
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
//#include "front_logi.h"
#include "gpio_api.h"
#include "i2c.h"
#include "hal_cdsp_iq.h"
#include "hal_cdsp.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "sp1k_front_api.h"
#include "front_logi.h"
#include "reg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       194
#define YOFF_30FPS_PREV       24
#define LINE_TOT_30FPS_PREV   835
#define FRAME_TOT_30FPS_PREV  552

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     1024
#define XOFF_SNAP       288
#define YOFF_SNAP       8
#define LINE_TOT_SNAP   1566
#define FRAME_TOT_SNAP  1048

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    2
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#if 0
#define MCLK_DIV_60FPS_PREV    0
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    0
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    		2
#define MCLK_DIV_SNAP_BURST    	2
#define MCLK_DIV_SNAP_BURST_UP  2

#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  12000000L
#if 0
#define PCLK_FREQ_60FPS_PREV  0
#endif
#define PCLK_FREQ_SNAP        	24000000L
#define PCLK_FREQ_SNAP_BURST    24000000L
#define PCLK_FREQ_SNAP_BURST_UP 24000000L

#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#if 0
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#endif
#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  40
#define RESHAPE_HRISE_30FPS_PREV  50
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  8
#define RESHAPE_VRISE_30FPS_PREV  10

#if 0
#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HFALL_60FPS_PREV  0
#define RESHAPE_HRISE_60FPS_PREV  0
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VFALL_60FPS_PREV  0
#define RESHAPE_VRISE_60FPS_PREV  0
#endif

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  80
#define RESHAPE_HRISE_SNAP  90
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x02
#define BAYER_PTN_SNAP  0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0xBA //0x90
#define I2C_CLK_DIV   4

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  127
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  127
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  175
#define GAIN_30FPS_MIN_IDX  0

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0
#endif

#define EXP_GAIN_SET_POS  0x40
#define GAIN_AFTER_EXP    0x01

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
#define frontOpPowerOn                frontSensorOpPowerOn
#define frontOpPowerOff               frontSensorOpPowerOff
#define frontOpPreviewModeSet         frontSensorOpPreviewModeSet
#define frontOpSnapModeSet            frontSensorOpSnapModeSet
#define frontOpSnapShot               frontSensorOpSnapShot
#define frontOpGainSet                frontSensorOpGainSet
#define frontOpExposureTimeSet        frontSensorOpExposureTimeSet
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
__STATIC void frontOpPowerOn(void);
__STATIC void frontOpPowerOff(void);
__STATIC void frontOpPreviewModeSet(void);
__STATIC void frontOpSnapModeSet(void);
__STATIC void frontOpSnapShot(UINT8 first, UINT8 mode, UINT8 scaleUp);
__STATIC void frontOpGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC UINT8 frontOpOpticalBlackStatusGet(void);
__STATIC UINT8 frontOpOpticalBlackCalibrate(void);
__STATIC void frontOpShutterOpen(void);
__STATIC void frontOpShutterClose(void);

static void frontBeforePowerOn(void);
static void frontAfterPowerOn(void);
static void frontBeforePowerOff(void);
static void frontAfterPowerOff(void);
static void frontSnapExposureTimeSet(UINT8 mode,UINT8 scaleUp);
static void frontColorNoiseReduce(UINT8 gain);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - MI MI1320 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[130] = {
		{0,	 299,	 175,	 0	},
		{1,	 300,	 167,	 0	},
		{2,	 299,	 159,	 0	},
		{3,	 301,	 152,	 0	},
		{4,	 301,	 145,	 0	},
		{5,	 299,	 138,	 0	},
		{6,	 299,	 132,	 0	},
		{7,	 400,	 153,	 0	},
		{8,	 400,	 146,	 0	},
		{9,	 399,	 139,	 0	},
		{10,	 301,	 112,	 0	},
		{11,	 300,	 103,	 0	},
		{12,	 299,	 95,	 0	},
		{13,	 399,	 117,	 0	},
		{14,	 301,	 81,	 0	},
		{15,	 299,	 74,	 0	},
		{16,	 299,	 68,	 0	},
		{17,	 400,	 89,	 0	},
		{18,	 400,	 82,	 0	},
		{19,	 399,	 75,	 0	},
		{20,	 301,	 47,	 0	},
		{21,	 1218,	 170,	 0	},
		{22,	 399,	 58,	 0	},
		{23,	 399,	 53,	 0	},
		{24,	 601,	 81,	 0	},
		{25,	 1198,	 138,	 0	},
		{26,	 1198,	 132,	 0	},
		{27,	 1208,	 126,	 0	},
		{28,	 400,	 33,	 0	},
		{29,	 1208,	 117,	 0	},
		{30,	 301,	 15,	 0	},
		{31,	 399,	 22,	 0	},
		{32,	 399,	 20,	 0	},
		{33,	 301,	 8,	 0	},
		{34,	 1218,	 82,	 0	},
		{35,	 1198,	 74,	 0	},
		{36,	 1198,	 68,	 0	},
		{37,	 1208,	 63,	 0	},
		{38,	 1198,	 57,	 0	},
		{39,	 1208,	 53,	 0	},
		{40,	 1208,	 48,	 0	},
		{41,	 1208,	 44,	 0	},
		{42,	 1208,	 40,	 0	},
		{43,	 1208,	 36,	 0	},
		{44,	 1218,	 33,	 0	},
		{45,	 1198,	 29,	 0	},
		{46,	 1198,	 26,	 0	},
		{47,	 1218,	 24,	 0	},
		{48,	 1208,	 21,	 0	},
		{49,	 1198,	 18,	 0	},
		{50,	 1208,	 16,	 0	},
		{51,	 1208,	 14,	 0	},
		{52,	 1208,	 12,	 0	},
		{53,	 1208,	 10,	 0	},
		{54,	 1198,	 8,	 0	},
		{55,	 1218,	 7,	 0	},
		{56,	 1198,	 5,	 0	},
		{57,	 1218,	 4,	 0	},
		{58,	 1198,	 2,	 0	},
		{59,	 1198,	 1,	 0	},
		{60,	 1208,	 0,	 0	},
		{61,	 1796,	 16,	 0	},
		{62,	 1796,	 14,	 0	},
		{63,	 1796,	 12,	 0	},
		{64,	 2083,	 8,	 0	},
		{65,	 2764,	 12,	 0	},
		{66,	 2764,	 9,	 0	},
		{67,	 2356,	 3,	 0	},
		{68,	 2356,	 2,	 0	},
		{69,	 2246,	 1,	 0	},
		{70,	 2764,	 2,	 0	},
		{71,	 2764,	 1,	 0	},
		{72,	 4956,	 14,	 0	},
		{73,	 4956,	 11,	 0	},
		{74,	 4956,	 9,	 0	},
		{75,	 5527,	 10,	 0	},
		{76,	 5527,	 8,	 0	},
		{77,	 5527,	 6,	 0	},
		{78,	 5527,	 4,	 0	},
		{79,	 5527,	 2,	 0	},
		{80,	 15968,	 37,	 0	},
		{81,	 28742,	 73,	 0	},
		{82,	 28742,	 69,	 0	},
		{83,	 28742,	 63,	 0	},
		{84,	 13065,	 17,	 0	},
		{85,	 11055,	 9,	 0	},
		{86,	 47903,	 89,	 0	},
		{87,	 47903,	 82,	 0	},
		{88,	 13065,	 7,	 0	},
		{89,	 13065,	 6,	 0	},
		{90,	 13065,	 5,	 0	},
		{91,	 11055,	 3,	 0	},
		{92,	 11055,	 0,	 0	},
		{93,	 71853,	 81,	 0	},
		{94,	 20530,	 10,	 0	},
		{95,	 71853,	 65,	 0	},
		{96,	 71853,	 63,	 0	},
		{97,	 47903,	 33,	 0	},
		{98,	 23952,	 7,	 0	},
		{99,	 17964,	 0,	 0	},
		{100,	 143698,	 103,	 0	},
		{101,	 143698,	 95,	 0	},
		{102,	 143698,	 88,	 0	},
		{103,	 143698,	 81,	 0	},
		{104,	 143698,	 75,	 0	},
		{105,	 143698,	 68,	 0	},
		{106,	 143698,	 63,	 0	},
		{107,	 143698,	 59,	 0	},
		{108,	 143698,	 56,	 0	},
		{109,	 143698,	 50,	 0	},
		{110,	 143698,	 45,	 0	},
		{111,	 143698,	 40,	 0	},
		{112,	 143698,	 36,	 0	},
		{113,	 143698,	 32,	 0	},
		{114,	 143698,	 29,	 0	},
		{115,	 143698,	 26,	 0	},
		{116,	 143698,	 23,	 0	},
		{117,	 143698,	 21,	 0	},
		{118,	 143698,	 18,	 0	},
		{119,	 143698,	 16,	 0	},
		{120,	 143698,	 14,	 0	},
		{121,	 143698,	 12,	 0	},
		{122,	 143698,	 10,	 0	},
		{123,	 143698,	 8,	 0	},
		{124,	 143698,	 7,	 0	},
		{125,	 143698,	 5,	 0	},
		{126,	 143698,	 4,	 0	},
		{127,	 143698,	 2,	 0	},
		{128,	 143698,	 1,	 0	},
		{129,	 143698,	 0,	 0	},
};

static code frontAe_t ae30fps50hzTbl[128] = {
		{0,	 334,	 172,	 0	},
		{1,	 333,	 163,	 0	},
		{2,	 334,	 156,	 0	},
		{3,	 333,	 148,	 0	},
		{4,	 334,	 142,	 0	},
		{5,	 333,	 135,	 0	},
		{6,	 499,	 170,	 0	},
		{7,	 333,	 124,	 0	},
		{8,	 333,	 119,	 0	},
		{9,	 333,	 114,	 0	},
		{10,	 333,	 107,	 0	},
		{11,	 333,	 99,	 0	},
		{12,	 502,	 129,	 0	},
		{13,	 333,	 84,	 0	},
		{14,	 499,	 118,	 0	},
		{15,	 333,	 71,	 0	},
		{16,	 499,	 106,	 0	},
		{17,	 991,	 161,	 0	},
		{18,	 333,	 55,	 0	},
		{19,	 998,	 147,	 0	},
		{20,	 1005,	 139,	 0	},
		{21,	 1019,	 137,	 0	},
		{22,	 1005,	 129,	 0	},
		{23,	 998,	 123,	 0	},
		{24,	 1012,	 119,	 0	},
		{25,	 991,	 113,	 0	},
		{26,	 499,	 45,	 0	},
		{27,	 991,	 97,	 0	},
		{28,	 991,	 95,	 0	},
		{29,	 998,	 83,	 0	},
		{30,	 1005,	 72,	 0	},
		{31,	 1019,	 78,	 0	},
		{32,	 1005,	 65,	 0	},
		{33,	 998,	 57,	 0	},
		{34,	 1012,	 55,	 0	},
		{35,	 991,	 49,	 0	},
		{36,	 998,	 45,	 0	},
		{37,	 333,	 3,	 0	},
		{38,	 998,	 37,	 0	},
		{39,	 502,	 9,	 0	},
		{40,	 991,	 25,	 0	},
		{41,	 1019,	 28,	 0	},
		{42,	 991,	 24,	 0	},
		{43,	 1012,	 21,	 0	},
		{44,	 998,	 19,	 0	},
		{45,	 1005,	 17,	 0	},
		{46,	 1012,	 15,	 0	},
		{47,	 1019,	 13,	 0	},
		{48,	 1012,	 11,	 0	},
		{49,	 1005,	 9,	 0	},
		{50,	 991,	 7,	 0	},
		{51,	 1019,	 6,	 0	},
		{52,	 991,	 4,	 0	},
		{53,	 1012,	 3,	 0	},
		{54,	 1019,	 2,	 0	},
		{55,	 991,	 0,	 0	},
		{56,	 1178,	 2,	 0	},
		{57,	 1123,	 0,	 0	},
		{58,	 1579,	 5,	 0	},
		{59,	 1579,	 3,	 0	},
		{60,	 1382,	 0,	 0	},
		{61,	 2083,	 22,	 0	},
		{62,	 2083,	 18,	 0	},
		{63,	 2764,	 13,	 0	},
		{64,	 2764,	 10,	 0	},
		{65,	 2356,	 6,	 0	},
		{66,	 2356,	 2,	 0	},
		{67,	 2764,	 7,	 0	},
		{68,	 2764,	 5,	 0	},
		{69,	 2764,	 3,	 0	},
		{70,	 4956,	 12,	 0	},
		{71,	 4956,	 10,	 0	},
		{72,	 4956,	 9,	 0	},
		{73,	 5527,	 10,	 0	},
		{74,	 5527,	 8,	 0	},
		{75,	 5527,	 6,	 0	},
		{76,	 4491,	 2,	 0	},
		{77,	 4491,	 0,	 0	},
		{78,	 13065,	 28,	 0	},
		{79,	 13065,	 25,	 0	},
		{80,	 13065,	 22,	 0	},
		{81,	 13065,	 20,	 0	},
		{82,	 13065,	 17,	 0	},
		{83,	 11055,	 10,	 0	},
		{84,	 47903,	 86,	 0	},
		{85,	 47903,	 81,	 0	},
		{86,	 13065,	 9,	 0	},
		{87,	 11055,	 3,	 0	},
		{88,	 11055,	 2,	 0	},
		{89,	 11055,	 1,	 0	},
		{90,	 11055,	 0,	 0	},
		{91,	 20530,	 12,	 0	},
		{92,	 20530,	 10,	 0	},
		{93,	 20530,	 8,	 0	},
		{94,	 20530,	 6,	 0	},
		{95,	 23952,	 8,	 0	},
		{96,	 23952,	 7,	 0	},
		{97,	 17964,	 0,	 0	},
		{98,	 35927,	 14,	 0	},
		{99,	 35927,	 12,	 0	},
		{100,	 35927,	 10,	 0	},
		{101,	 143698,	 81,	 0	},
		{102,	 143698,	 75,	 0	},
		{103,	 143698,	 68,	 0	},
		{104,	 143698,	 63,	 0	},
		{105,	 143698,	 57,	 0	},
		{106,	 143698,	 50,	 0	},
		{107,	 143698,	 43,	 0	},
		{108,	 143698,	 28,	 0	},
		{109,	 143698,	 12,	 0	},
		{110,	 143698,	 10,	 0	},
		{111,	 143698,	 32,	 0	},
		{112,	 143698,	 29,	 0	},
		{113,	 143698,	 26,	 0	},
		{114,	 143698,	 23,	 0	},
		{115,	 143698,	 21,	 0	},
		{116,	 143698,	 18,	 0	},
		{117,	 143698,	 16,	 0	},
		{118,	 143698,	 14,	 0	},
		{119,	 143698,	 12,	 0	},
		{120,	 143698,	 10,	 0	},
		{121,	 143698,	 8,	 0	},
		{122,	 143698,	 7,	 0	},
		{123,	 143698,	 5,	 0	},
		{124,	 143698,	 4,	 0	},
		{125,	 143698,	 2,	 0	},
		{126,	 143698,	 1,	 0	},
		{127,	 143698,	 0,	 0	},
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[176] = {
	16, 	17, 	18, 	19, 	20, 	21, 	22, 	23, 	24, 	25,
	26, 	27, 	28, 	29, 	30, 	31, 	32, 	33, 	34, 	35,
	36, 	37, 	38, 	39, 	40, 	41, 	42, 	43, 	44, 	45,
	46, 	47, 	48, 	49, 	50, 	51, 	52, 	53, 	54, 	55,
	56, 	57, 	58, 	59, 	60, 	61, 	62, 	63, 	64, 	65,
	66, 	67, 	68, 	69, 	70, 	71, 	72, 	73, 	74, 	75,
	76, 	77, 	78, 	79, 	80, 	81, 	82, 	83, 	84, 	85,
	86, 	87, 	88, 	89, 	90, 	91, 	92, 	93, 	94, 	95,
	96, 	97, 	98, 	99, 	100, 	101, 	102, 	103, 	104, 	105,
	106, 	107, 	108, 	109, 	110, 	111, 	112, 	113, 	114, 	115,
	116, 	117, 	118, 	119, 	120, 	121, 	122, 	123, 	124, 	125,
	126, 	127, 	192, 	193, 	194, 	195, 	196, 	197, 	198, 	199,
	200, 	201, 	202, 	203, 	204, 	205, 	206, 	207, 	208, 	209,
	210, 	211, 	212, 	213, 	214, 	215, 	216, 	217, 	218, 	219,
	220, 	221, 	222, 	223, 	224, 	225, 	226, 	227, 	228, 	229,
	230, 	231, 	232, 	233, 	234, 	235, 	236, 	237, 	238, 	239,
	240, 	241, 	242, 	243, 	244, 	245, 	246, 	247, 	248, 	249,
	250, 	251, 	252, 	253, 	254, 	255,
};

#if 0
static code UINT16 gain60fpsTbl[0] = {
};
#endif

/* exposure time index table. */
static code UINT8 ae30fpsMaxIdxTbl[2] = {
	AE_30FPS_30_MAX_IDX,
	AE_30FPS_25_MAX_IDX,
};

static code UINT8 ae30fpsMinIdxTbl[2] = {
	AE_30FPS_30_MIN_IDX,
	AE_30FPS_25_MIN_IDX,
};

#if 0
static code UINT8 ae60fpsMaxIdxTbl[2] = {
	AE_60FPS_30_MAX_IDX,
	AE_60FPS_25_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_30_MIN_IDX,
	AE_60FPS_25_MIN_IDX,
};
#endif

/* device capability. */
static code frontPrevCapabDesc_t frontPrevCapabDesc[PREV_MODE_TOT] = {
	{  /* 30 fps. */
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
	#if 0
	{  /* 60 fps. */
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
		gain60fpsTbl,
		GAIN_60FPS_MAX_IDX,
		GAIN_60FPS_MIN_IDX,
	},
	#endif
};

static code frontSnapCapabDesc_t frontSnapCapabDesc[SNAP_MODE_TOT] = {
	{  /* 30/60 fps. */
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
	0x0D, 0x00, 0x01,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x0D,0x00,0x08,
	0xF0,0x00,0x00,
	0xc8,0x00,0x00,
	0x21,0x04,0x0c,
	0x07,0x00,0xbe,
	0x08,0x00,0x1e,
	0xF0,0x00,0x01,
	0x06,0x00,0x00,
	0x08,0x00,0x00,
	0x3A,0x57,0x00,
	0x9B,0x57,0x00,
	0xF0,0x00,0x00,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0xF0,0x00,0x00,
	0xc8,0x00,0x00,
	0x21,0x04,0x0c,
	0x20,0x01,0x03,
	0x07,0x00,0xbe,
	0x08,0x00,0x1e,
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};
#endif

static code UINT8 frontSnapCmdTbl[] = {
	0xf0,0x00,0x00,
	0xc8,0x00,0x0b,
	0x05,0x01,0x18,
	0x06,0x00,0x0d,
	0x20,0x01,0x03,
};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
//extern UINT8 HAL_CdspEofWait(UINT8 Number);
//extern void cdspColorMatrixRestore(void);

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
	frontOperatingFunc_t opFunc;

    opFunc.expGainSetPos             = EXP_GAIN_SET_POS;
    opFunc.gainAfterExp              = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet            = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet          = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet         = frontOpCapabilityGet;
	opFunc.pfOpPowerOn               = frontOpPowerOn;
	opFunc.pfOpPowerOff              = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet        = frontOpPreviewModeSet;
	opFunc.pfOpSnapModeSet           = frontOpSnapModeSet;
	opFunc.pfOpSnapShot              = frontOpSnapShot;
	opFunc.pfOpGainSet               = frontOpGainSet;
	opFunc.pfOpExposureTimeSet       = frontOpExposureTimeSet;
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
	case 30:
		//frontPrevMode = 0;
		//frontSnapMode = 0;
		break;

	case 60:
		//frontPrevMode = 1;
		//frontSnapMode = 1;
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


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	/* before power on. */
	frontBeforePowerOn();

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	i2cCmdTableSend(frontInit0CmdTbl, 2, sizeof(frontInit0CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(300);
	i2cCmdTableSend(frontInit1CmdTbl, 2, sizeof(frontInit1CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);
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
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		i2cCmdTableSend(frontSnapCmdTbl, 2, sizeof(frontSnapCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
		mode=mode;
		scaleUp=scaleUp;
		/*frontSnapExposureTimeSet();*/
	}

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
	
	cdspSkipWrSyncSet(1);	
	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);

	/*i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);*/
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
	UINT8 cmdTbl[3*1];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];

	cmdTbl[0] = 0x2f;
	cmdTbl[1] = 0x00;
	cmdTbl[2] = frontLastGain;
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);

	frontColorNoiseReduce(val);
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
	UINT32 tmp0, pixclk;
	UINT8 cmdTbl[3*2];
	UINT16 s_gain, lastt_gain;


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

	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	s_gain = frontLastGain;

	#if 0
	if ( s_gain >= 0x30 ) {
		lastt_gain = s_gain - 32;
		espline = espline * 2;
	}
	else if ( s_gain >= 0x10 ) {
		lastt_gain = s_gain - 16;
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
	XBYTE[REG_Cdsp_Yhdiv] = 0x02;
	XBYTE[REG_Cdsp_EdgeEn] = 0x02;

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
		espline = ftotal - 1;
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
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontColorNoiseReduce(
	UINT8 gain
)
{

}

