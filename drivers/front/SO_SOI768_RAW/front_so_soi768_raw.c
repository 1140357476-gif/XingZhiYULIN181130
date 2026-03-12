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
 * @file		front_so_soi768_raw.c
 * @brief		front sensor silicon optronics soi768 driver.
 * @author		Matt Wang
 * @since		2008-10-17
 * @date		2008-11-17
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
#include "hal_front.h"
#include "hal_front_para.h"
#include "cdsp_misc.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       50
#define YOFF_30FPS_PREV       3
#define LINE_TOT_30FPS_PREV   784
#define FRAME_TOT_30FPS_PREV  510

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      640
#define HEIGHT_SNAP     480
#define XOFF_SNAP       48
#define YOFF_SNAP       3
#define LINE_TOT_SNAP   784
#define FRAME_TOT_SNAP  510

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    4
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#if 0
#define MCLK_DIV_60FPS_PREV    0
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    0
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    4
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  12000000L
#if 0
#define PCLK_FREQ_60FPS_PREV  0
#endif
#define PCLK_FREQ_SNAP        24000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#if 0
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#endif
#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HFALL_30FPS_PREV  30
#define RESHAPE_HRISE_30FPS_PREV  40
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3

#if 0
#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HFALL_60FPS_PREV  0
#define RESHAPE_HRISE_60FPS_PREV  0
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VFALL_60FPS_PREV  0
#define RESHAPE_VRISE_60FPS_PREV  0
#endif

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x02
#define BAYER_PTN_SNAP  0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0x42
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  120
#define AE_30FPS_30_MIN_IDX  38
#define AE_30FPS_25_MAX_IDX  120
#define AE_30FPS_25_MIN_IDX  38
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  14
#define GAIN_30FPS_MIN_IDX  1

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0
#endif

#define GAIN_LARGE   8
#define GAIN_MIDDLE  30
#define GAIN_SMALL   30

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
__STATIC void frontOpSnapShot(UINT8 first, UINT8 mode,UINT8 scaleUp);
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
static void frontSnapExposureTimeSet(void);
static void frontColorNoiseReduce(UINT8 gain);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - SO SOI768 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    333,     47,      0 },
	{     51,    333,     47,      0 },
	{     52,    333,     47,      0 },
	{     53,    333,     47,      0 },
	{     54,    333,     47,      0 },
	{     55,    333,     47,      0 },
	{     56,    333,     47,      0 },
	{     57,    333,     47,      0 },
	{     58,    333,     47,      0 },
	{     59,    333,     47,      0 },
	{     60,    333,     47,      0 },
	{     61,    333,     47,      0 },
	{     62,    333,     47,      0 },
	{     63,    333,     47,      0 },
	{     64,    333,     47,      0 },
	{     65,    333,     47,      0 },
	{     66,    333,     47,      0 },
	{     67,    333,     47,      0 },
	{     68,    333,     47,      0 },
	{     69,    333,     47,      0 },
	{     70,    333,     45,      0 },
	{     71,    333,     43,      0 },
	{     72,    333,     41,      0 },
	{     73,    333,     38,      0 },
	{     74,    333,     36,      0 },
	{     75,    333,     35,      0 },
	{     76,    333,     34,      0 },
	{     77,    333,     32,      0 },
	{     78,    333,     30,      0 },
	{     79,    333,     29,      0 },
	{     80,    333,     26,      0 },
	{     81,    333,     24,      0 },
	{     82,    333,     22,      0 },
	{     83,    333,     20,      0 },
	{     84,    333,     19,      0 },
	{     85,    333,     17,      0 },
	{     86,    333,     16,      0 },
	{     87,    333,     15,      0 },
	{     88,    333,     14,      0 },
	{     89,    333,     12,      0 },
	{     90,    333,     10,      0 },
	{     91,    333,      8,      0 },
	{     92,    333,      6,      0 },
	{     93,    333,      4,      0 },
	{     94,    333,      3,      0 },
	{     95,    333,      2,      0 },
	{     96,    333,      1,      0 },
	{     97,    333,      0,      0 },
	{     98,    500,      6,      0 },
	{     99,    500,      4,      0 },
	{    100,    500,      3,      0 },
	{    101,    500,      2,      0 },
	{    102,    500,      1,      0 },
	{    103,    500,      0,      0 },
	{    104,   1000,     13,      0 },
	{    105,   1000,     11,      0 },
	{    106,   1000,      9,      0 },
	{    107,   1000,      7,      0 },
	{    108,   1000,      6,      0 },
	{    109,   1000,      4,      0 },
	{    110,   1000,      3,      0 },
	{    111,   1000,      2,      0 },
	{    112,   1000,      1,      0 },
	{    113,   1000,      0,      0 },
	{    114,   1044,      5,      0 },
	{    115,   1119,      5,      0 },
	{    116,   1200,      5,      0 },
	{    117,   1287,      5,      0 },
	{    118,   1380,      5,      0 },
	{    119,   1480,      5,      0 },
	{    120,   1587,      5,      0 },
	{    121,   1701,      5,      0 },
	{    122,   1824,      5,      0 },
	{    123,   1955,      5,      0 },
	{    124,   2096,      5,      0 },
	{    125,   2247,      5,      0 },
	{    126,   2409,      5,      0 },
	{    127,   2582,      5,      0 },
	{    128,   2768,      5,      0 },
	{    129,   2967,      5,      0 },
	{    130,   3181,      5,      0 },
	{    131,   3410,      5,      0 },
	{    132,   3655,      5,      0 },
	{    133,   3918,      5,      0 },
	{    134,   4200,      5,      0 },
	{    135,   4502,      5,      0 },
	{    136,   4826,      5,      0 },
	{    137,   5173,      5,      0 },
	{    138,   5545,      5,      0 },
	{    139,   5943,      5,      0 },
	{    140,   6370,      5,      0 },
	{    141,   6828,      5,      0 },
	{    142,   7319,      5,      0 },
	{    143,   7845,      5,      0 },
	{    144,   8409,      5,      0 },
	{    145,   9013,      5,      0 },
	{    146,   9660,      5,      0 },
	{    147,  10354,      5,      0 },
	{    148,  11098,      5,      0 },
	{    149,  11895,      5,      0 },
	{    150,  12749,      5,      0 },
	{    151,  13665,      5,      0 },
	{    152,  14646,      5,      0 },
	{    153,  15698,      5,      0 },
	{    154,  16825,      5,      0 },
	{    155,  18033,      5,      0 },
	{    156,  19328,      5,      0 },
	{    157,  20716,      5,      0 },
	{    158,  22203,      5,      0 },
	{    159,  23797,      5,      0 },
	{    160,  25506,      5,      0 },
	{    161,  27337,      5,      0 },
	{    162,  29300,      5,      0 },
	{    163,  31404,      5,      0 },
	{    164,  33659,      5,      0 },
	{    165,  36075,      5,      0 },
	{    166,  38665,      5,      0 },
	{    167,  41441,      5,      0 },
	{    168,  44416,      5,      0 },
	{    169,  47604,      5,      0 },
	{    170,  51021,      6,      0 },
};

static code frontAe_t ae30fps50hzTbl[121] = {
	{     50,    333,     47,      0 },
	{     51,    333,     47,      0 },
	{     52,    333,     47,      0 },
	{     53,    333,     47,      0 },
	{     54,    333,     47,      0 },
	{     55,    333,     47,      0 },
	{     56,    333,     47,      0 },
	{     57,    333,     47,      0 },
	{     58,    333,     47,      0 },
	{     59,    333,     47,      0 },
	{     60,    333,     47,      0 },
	{     61,    333,     47,      0 },
	{     62,    333,     47,      0 },
	{     63,    333,     47,      0 },
	{     64,    333,     47,      0 },
	{     65,    333,     47,      0 },
	{     66,    333,     47,      0 },
	{     67,    333,     47,      0 },
	{     68,    333,     47,      0 },
	{     69,    333,     47,      0 },
	{     70,    333,     45,      0 },
	{     71,    333,     43,      0 },
	{     72,    333,     41,      0 },
	{     73,    333,     38,      0 },
	{     74,    333,     36,      0 },
	{     75,    333,     35,      0 },
	{     76,    333,     34,      0 },
	{     77,    333,     32,      0 },
	{     78,    333,     30,      0 },
	{     79,    333,     29,      0 },
	{     80,    333,     26,      0 },
	{     81,    333,     24,      0 },
	{     82,    333,     22,      0 },
	{     83,    333,     20,      0 },
	{     84,    333,     19,      0 },
	{     85,    333,     17,      0 },
	{     86,    333,     16,      0 },
	{     87,    333,     15,      0 },
	{     88,    333,     14,      0 },
	{     89,    333,     12,      0 },
	{     90,    333,     10,      0 },
	{     91,    333,      8,      0 },
	{     92,    333,      6,      0 },
	{     93,    333,      4,      0 },
	{     94,    333,      3,      0 },
	{     95,    333,      2,      0 },
	{     96,    333,      1,      0 },
	{     97,    333,      0,      0 },
	{     98,    500,      6,      0 },
	{     99,    500,      4,      0 },
	{    100,    500,      3,      0 },
	{    101,    500,      2,      0 },
	{    102,    500,      1,      0 },
	{    103,    500,      0,      0 },
	{    104,   1000,     13,      0 },
	{    105,   1000,     11,      0 },
	{    106,   1000,      9,      0 },
	{    107,   1000,      7,      0 },
	{    108,   1000,      6,      0 },
	{    109,   1000,      4,      0 },
	{    110,   1000,      3,      0 },
	{    111,   1000,      2,      0 },
	{    112,   1000,      1,      0 },
	{    113,   1000,      0,      0 },
	{    114,   1044,      5,      0 },
	{    115,   1119,      5,      0 },
	{    116,   1200,      5,      0 },
	{    117,   1287,      5,      0 },
	{    118,   1380,      5,      0 },
	{    119,   1480,      5,      0 },
	{    120,   1587,      5,      0 },
	{    121,   1701,      5,      0 },
	{    122,   1824,      5,      0 },
	{    123,   1955,      5,      0 },
	{    124,   2096,      5,      0 },
	{    125,   2247,      5,      0 },
	{    126,   2409,      5,      0 },
	{    127,   2582,      5,      0 },
	{    128,   2768,      5,      0 },
	{    129,   2967,      5,      0 },
	{    130,   3181,      5,      0 },
	{    131,   3410,      5,      0 },
	{    132,   3655,      5,      0 },
	{    133,   3918,      5,      0 },
	{    134,   4200,      5,      0 },
	{    135,   4502,      5,      0 },
	{    136,   4826,      5,      0 },
	{    137,   5173,      5,      0 },
	{    138,   5545,      5,      0 },
	{    139,   5943,      5,      0 },
	{    140,   6370,      5,      0 },
	{    141,   6828,      5,      0 },
	{    142,   7319,      5,      0 },
	{    143,   7845,      5,      0 },
	{    144,   8409,      5,      0 },
	{    145,   9013,      5,      0 },
	{    146,   9660,      5,      0 },
	{    147,  10354,      5,      0 },
	{    148,  11098,      5,      0 },
	{    149,  11895,      5,      0 },
	{    150,  12749,      5,      0 },
	{    151,  13665,      5,      0 },
	{    152,  14646,      5,      0 },
	{    153,  15698,      5,      0 },
	{    154,  16825,      5,      0 },
	{    155,  18033,      5,      0 },
	{    156,  19328,      5,      0 },
	{    157,  20716,      5,      0 },
	{    158,  22203,      5,      0 },
	{    159,  23797,      5,      0 },
	{    160,  25506,      5,      0 },
	{    161,  27337,      5,      0 },
	{    162,  29300,      5,      0 },
	{    163,  31404,      5,      0 },
	{    164,  33659,      5,      0 },
	{    165,  36075,      5,      0 },
	{    166,  38665,      5,      0 },
	{    167,  41441,      5,      0 },
	{    168,  44416,      5,      0 },
	{    169,  47604,      5,      0 },
	{    170,  51021,      6,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[48] = {
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
};

#if 0
static code UINT16 gain60fpsTbl[0] = {
};
#endif

/* color matrix table. */
static code UINT16 colorMatrixTbl[3][9] = {
	{ 0x0058, 0x03EF, 0x03F9, 0x03C6, 0x0090, 0x03EA, 0x03DB, 0x03D7, 0x008E },
	{ 0x0048, 0x03FA, 0x03FE, 0x03CF, 0x0083, 0x03ED, 0x03E2, 0x03E9, 0x0075 },
	{ 0x0050, 0x03F8, 0x03F8, 0x03F8, 0x0050, 0x03F8, 0x03F8, 0x03F8, 0x0050 },
};

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
		MCLK_PHASE_SNAP,
		PCLK_DIV_SNAP,
		PCLK_PHASE_SNAP,
		PCLK_FREQ_SNAP,
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
	0x12, 0x80,
};

static code UINT8 frontInit1CmdTbl[] = {
    0x12, 0x04,
    0x13, 0x88,

    0x11, 0x00,
    0x0F, 0x03,

    0x14, 0x04,
    0x03, 0x4A,
    0x17, 0x0B,
    0x18, 0x61,
    0x19, 0x00,
    0x1A, 0x3E,
    0x32, 0x2D,
    0x1B, 0x00,
    #if 0
    0x3E, 0xFF,
    0x3F, 0xFF,
    0x42, 0xFF,
    0x43, 0xFF,
    0x44, 0x00,
    #endif

    0x0C, 0x2A,  //0x28

    0x00, 0x1F,  //gain
    0x04, 0x02,  //AEC[2:0]
    0x10, 0x26,  //AEC[10:3]
    0x2D, 0x00,  //dummy LSB
    0x2E, 0x00,  //dummy MSB
};

#if 0
static code UINT8 front30fpsPrevCmdTbl[] = {
};

static code UINT8 front60fpsPrevCmdTbl[] = {
};

static code UINT8 frontSnapCmdTbl[] = {
};
#endif

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;
static UINT8 frontGainClass = 0xFF;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 HAL_CdspEofWait(UINT8 Number);

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-10-17
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
 * @since	2008-10-17
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
 * @since	2008-10-17
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
 * @since	2008-10-17
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

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(10);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	/* after power on. */
	frontAfterPowerOn();
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-17
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
 * @since	2008-10-17
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

	//i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-17
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
 * @since	2008-10-17
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
	mode=mode;
	scaleUp=scaleUp;
	
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		//i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
		//frontSnapExposureTimeSet();
	}

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
	cdsp1stPathWriteSkip(6, 1);
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
 * @since	2008-10-17
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
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);

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
 * @since	2008-10-17
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
	UINT32 pixclk, tmp0;
	UINT8 cmdTbl[2*4];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	if (espline >= ftotal ) {
		tmp0 =espline-ftotal;
		espline = ftotal-1;
	}
	else {
	    tmp0 =0;
	}

	cmdTbl[0] = 0x04;
	cmdTbl[1] = espline & 0x07;
	cmdTbl[2] = 0x10;
	cmdTbl[3] = (espline >> 3) & 0xFF;
	cmdTbl[4] = 0x2D;
	cmdTbl[5] = tmp0 & 0xFF;
	cmdTbl[6] = 0x2E;
	cmdTbl[7] = (tmp0 >> 8) & 0xFF;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
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
 * @since	2008-10-17
 * @todo	N/A
 * @bug		N/A
*/
static void
frontSnapExposureTimeSet(
	void
)
{

}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-17
 * @todo	N/A
 * @bug		N/A
*/
static void
frontColorNoiseReduce(
	UINT8 gain
)
{
	if ( gain > GAIN_LARGE ) {
		if ( frontGainClass != 3 ) {
			HAL_CdspColorMSet_Intr(colorMatrixTbl[2][0], colorMatrixTbl[2][1], colorMatrixTbl[2][2],
			                       colorMatrixTbl[2][3], colorMatrixTbl[2][4], colorMatrixTbl[2][5],
			                       colorMatrixTbl[2][6], colorMatrixTbl[2][7], colorMatrixTbl[2][8]);
			XBYTE[0x21D3] = 0x08;
			XBYTE[0x21D0] = 0x08;

			frontGainClass = 3;
		}
	}
	else if ( gain > GAIN_MIDDLE ) {
		if ( frontGainClass != 2 ) {
			HAL_CdspColorMSet_Intr(colorMatrixTbl[1][0], colorMatrixTbl[1][1], colorMatrixTbl[1][2],
			                       colorMatrixTbl[1][3], colorMatrixTbl[1][4], colorMatrixTbl[1][5],
			                       colorMatrixTbl[1][6], colorMatrixTbl[1][7], colorMatrixTbl[1][8]);
			XBYTE[0x21D3] = 0x06;
			XBYTE[0x21D0] = 0x08;

			frontGainClass = 2;
		}
	}
	else if ( gain > GAIN_SMALL ) {
		if ( frontGainClass != 1 ) {
			HAL_CdspColorMSet_Intr(colorMatrixTbl[0][0], colorMatrixTbl[0][1], colorMatrixTbl[0][2],
			                       colorMatrixTbl[0][3], colorMatrixTbl[0][4], colorMatrixTbl[0][5],
			                       colorMatrixTbl[0][6], colorMatrixTbl[0][7], colorMatrixTbl[0][8]); // for hgnoise
			XBYTE[0x21D3] = 0x04;
			XBYTE[0x21D0] = 0x0A;

			frontGainClass = 1;
		}
	}
	else {
		if ( frontGainClass != 0 ) {
			cdspColorMatrixRestore();
			XBYTE[0x21D3] = 0x08;
			XBYTE[0x21D0] = 0x08;

			frontGainClass = 0;
		}
	};
}

