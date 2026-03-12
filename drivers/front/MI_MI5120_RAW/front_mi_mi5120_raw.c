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
 * @file		front_mi_mi5100_raw.c
 * @brief		front sensor micron mi5100 driver.
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
#include "sp1k_front_api.h"
#include "front_logi.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       6
#define YOFF_30FPS_PREV       4
#define LINE_TOT_30FPS_PREV   1548
#define FRAME_TOT_30FPS_PREV  516

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      2592
#define HEIGHT_SNAP     1944
#define XOFF_SNAP       2
#define YOFF_SNAP       2
#define LINE_TOT_SNAP   3552
#define FRAME_TOT_SNAP  2000

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    12
#define MCLK_PHASE_30FPS_PREV  0
#define MCLK_DRV_30FPS_PREV    1
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#if 0
#define MCLK_DIV_60FPS_PREV    12
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    0
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    		12
#define MCLK_DIV_SNAP_BURST     12
#define MCLK_DIV_SNAP_BURST_UP  12
#define MCLK_PHASE_SNAP  0
#define MCLK_DRV_SNAP    1
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  24000000L
#if 0
#define PCLK_FREQ_60FPS_PREV  0
#endif
#define PCLK_FREQ_SNAP        	32000000L
#define PCLK_FREQ_SNAP_BURST  	32000000L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 32000000L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#if 0
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#endif
#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  4
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  1
#define RESHAPE_VRISE_30FPS_PREV  2

#if 0
#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HFALL_60FPS_PREV  0
#define RESHAPE_HRISE_60FPS_PREV  0
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VFALL_60FPS_PREV  0
#define RESHAPE_VRISE_60FPS_PREV  0
#endif

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  1

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  1
#define RESHAPE_HRISE_SNAP  2
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x03
#define BAYER_PTN_SNAP  0x03

/* i2c interface. */
#define I2C_DEV_ADDR  0xBA
#define I2C_CLK_DIV   1

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  120
#define AE_30FPS_30_MIN_IDX  26
#define AE_30FPS_25_MAX_IDX  120
#define AE_30FPS_25_MIN_IDX  28
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  47
#define GAIN_30FPS_MIN_IDX  3

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
#define frontOpAfterSnapShot          frontSensorOpAfterSnapShot

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
__STATIC void frontOpSnapShot(UINT8 first,UINT8 mode,UINT8 scaleUp);
__STATIC void frontOpGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC UINT8 frontOpOpticalBlackStatusGet(void);
__STATIC UINT8 frontOpOpticalBlackCalibrate(void);
__STATIC void frontOpShutterOpen(void);
__STATIC void frontOpShutterClose(void);
__STATIC void frontOpAfterSnapShot(UINT8 *parg);

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
static code UINT8 frontDevName[] = "SENSOR - MI MI5120 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    300,     47,      0 },
	{     51,    300,     47,      0 },
	{     52,    300,     47,      0 },
	{     53,    300,     47,      0 },
	{     54,    300,     47,      0 },
	{     55,    300,     47,      0 },
	{     56,    300,     47,      0 },
	{     57,    300,     47,      0 },
	{     58,    300,     47,      0 },
	{     59,    300,     47,      0 },
	{     60,    300,     47,      0 },
	{     61,    300,     47,      0 },
	{     62,    300,     47,      0 },
	{     63,    300,     47,      0 },
	{     64,    300,     47,      0 },
	{     65,    300,     47,      0 },
	{     66,    300,     47,      0 },
	{     67,    300,     47,      0 },
	{     68,    300,     47,      0 },
	{     69,    300,     47,      0 },
	{     70,    300,     47,      0 },
	{     71,    300,     47,      0 },
	{     72,    300,     47,      0 },
	{     73,    300,     47,      0 },
	{     74,    300,     45,      0 },
	{     75,    300,     43,      0 },
	{     76,    300,     40,      0 },
	{     77,    300,     38,      0 },
	{     78,    300,     36,      0 },
	{     79,    300,     34,      0 },
	{     80,    300,     32,      0 },
	{     81,    300,     30,      0 },
	{     82,    300,     29,      0 },
	{     83,    300,     28,      0 },
	{     84,    300,     26,      0 },
	{     85,    300,     25,      0 },
	{     86,    300,     23,      0 },
	{     87,    300,     21,      0 },
	{     88,    300,     19,      0 },
	{     89,    300,     17,      0 },
	{     90,    300,     16,      0 },
	{     91,    300,     14,      0 },
	{     92,    300,     12,      0 },
	{     93,    300,     11,      0 },
	{     94,    300,     10,      0 },
	{     95,    300,      9,      0 },
	{     96,    300,      7,      0 },
	{     97,    400,     11,      0 },
	{     98,    400,     10,      0 },
	{     99,    400,      9,      0 },
	{    100,    400,      8,      0 },
	{    101,    400,      7,      0 },
	{    102,    400,      6,      0 },
	{    103,    400,      5,      0 },
	{    104,    400,      4,      0 },
	{    105,    600,      9,      0 },
	{    106,    600,      8,      0 },
	{    107,   1200,     21,      0 },
	{    108,   1200,     19,      0 },
	{    109,   1200,     17,      0 },
	{    110,   1200,     15,      0 },
	{    111,   1200,     14,      0 },
	{    112,   1200,     12,      0 },
	{    113,   1200,     10,      0 },
	{    114,   1200,      9,      0 },
	{    115,   1200,      8,      0 },
	{    116,   1206,      7,      0 },
	{    117,   1228,      7,      0 },
	{    118,   1228,      6,      0 },
	{    119,   1407,      7,      0 },
	{    120,   1501,      7,      0 },
	{    121,   1689,      8,      0 },
	{    122,   1732,      7,      0 },
	{    123,   1732,      6,      0 },
	{    124,   1826,      6,      0 },
	{    125,   1826,      5,      0 },
	{    126,   2815,     11,      0 },
	{    127,   2815,     10,      0 },
	{    128,   3071,     10,      0 },
	{    129,   3071,      9,      0 },
	{    130,   3378,      9,      0 },
	{    131,   3378,      8,      0 },
	{    132,   3378,      7,      0 },
	{    133,   3753,      7,      0 },
	{    134,   4504,      9,      0 },
	{    135,   4504,      8,      0 },
	{    136,   4826,      8,      0 },
	{    137,   5197,      8,      0 },
	{    138,   5630,      8,      0 },
	{    139,   5630,      7,      0 },
	{    140,   6142,      7,      0 },
	{    141,   6756,      8,      0 },
	{    142,   6142,      5,      0 },
	{    143,   7507,      7,      0 },
	{    144,   7507,      6,      0 },
	{    145,   7507,      5,      0 },
	{    146,  11261,     10,      0 },
	{    147,  11261,      9,      0 },
	{    148,  11261,      8,      0 },
	{    149,  13513,      9,      0 },
	{    150,  11261,      5,      0 },
	{    151,  13513,      7,      0 },
	{    152,  13513,      6,      0 },
	{    153,  13513,      5,      0 },
	{    154,  13513,      4,      0 },
	{    155,  16891,      6,      0 },
	{    156,  16891,      5,      0 },
	{    157,  16891,      4,      0 },
	{    158,  22522,      9,      0 },
	{    159,  22522,      8,      0 },
	{    160,  22522,      7,      0 },
	{    161,  33780,     12,      0 },
	{    162,  33780,     11,      0 },
	{    163,  33780,     10,      0 },
	{    164,  33780,      9,      0 },
	{    165,  33780,      8,      0 },
	{    166,  33780,      7,      0 },
	{    167,  33780,      6,      0 },
	{    168,  33780,      5,      0 },
	{    169,  33780,      4,      0 },
	{    170,  33780,      3,      0 },
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
	{     70,    333,     47,      0 },
	{     71,    333,     47,      0 },
	{     72,    333,     47,      0 },
	{     73,    333,     47,      0 },
	{     74,    333,     47,      0 },
	{     75,    333,     47,      0 },
	{     76,    333,     44,      0 },
	{     77,    333,     42,      0 },
	{     78,    333,     39,      0 },
	{     79,    333,     37,      0 },
	{     80,    333,     35,      0 },
	{     81,    333,     33,      0 },
	{     82,    333,     31,      0 },
	{     83,    333,     30,      0 },
	{     84,    333,     28,      0 },
	{     85,    333,     27,      0 },
	{     86,    333,     25,      0 },
	{     87,    333,     24,      0 },
	{     88,    333,     22,      0 },
	{     89,    333,     20,      0 },
	{     90,    333,     18,      0 },
	{     91,    333,     16,      0 },
	{     92,    333,     15,      0 },
	{     93,    333,     13,      0 },
	{     94,    333,     12,      0 },
	{     95,    333,     10,      0 },
	{     96,    333,      9,      0 },
	{     97,    333,      8,      0 },
	{     98,    333,      7,      0 },
	{     99,    333,      6,      0 },
	{    100,    333,      5,      0 },
	{    101,    333,      4,      0 },
	{    102,    500,      9,      0 },
	{    103,    500,      8,      0 },
	{    104,    500,      7,      0 },
	{    105,    500,      6,      0 },
	{    106,   1000,     19,      0 },
	{    107,   1000,     17,      0 },
	{    108,   1000,     15,      0 },
	{    109,   1000,     14,      0 },
	{    110,   1000,     12,      0 },
	{    111,   1000,     11,      0 },
	{    112,   1023,     10,      0 },
	{    113,   1055,      9,      0 },
	{    114,   1126,      9,      0 },
	{    115,   1206,      9,      0 },
	{    116,   1206,      8,      0 },
	{    117,   1228,      7,      0 },
	{    118,   1228,      6,      0 },
	{    119,   1407,      7,      0 },
	{    120,   1501,      7,      0 },
	{    121,   1689,      8,      0 },
	{    122,   1732,      7,      0 },
	{    123,   1732,      6,      0 },
	{    124,   1826,      6,      0 },
	{    125,   1826,      5,      0 },
	{    126,   2815,     11,      0 },
	{    127,   2815,     10,      0 },
	{    128,   3071,     10,      0 },
	{    129,   3071,      9,      0 },
	{    130,   3378,      9,      0 },
	{    131,   3378,      8,      0 },
	{    132,   3378,      7,      0 },
	{    133,   3753,      7,      0 },
	{    134,   4504,      9,      0 },
	{    135,   4504,      8,      0 },
	{    136,   4826,      8,      0 },
	{    137,   5197,      8,      0 },
	{    138,   5630,      8,      0 },
	{    139,   5630,      7,      0 },
	{    140,   6142,      7,      0 },
	{    141,   6756,      8,      0 },
	{    142,   6142,      5,      0 },
	{    143,   7507,      7,      0 },
	{    144,   7507,      6,      0 },
	{    145,   7507,      5,      0 },
	{    146,  11261,     10,      0 },
	{    147,  11261,      9,      0 },
	{    148,  11261,      8,      0 },
	{    149,  13513,      9,      0 },
	{    150,  11261,      5,      0 },
	{    151,  13513,      7,      0 },
	{    152,  13513,      6,      0 },
	{    153,  13513,      5,      0 },
	{    154,  13513,      4,      0 },
	{    155,  16891,      6,      0 },
	{    156,  16891,      5,      0 },
	{    157,  16891,      4,      0 },
	{    158,  22522,      9,      0 },
	{    159,  22522,      8,      0 },
	{    160,  22522,      7,      0 },
	{    161,  33780,     12,      0 },
	{    162,  33780,     11,      0 },
	{    163,  33780,     10,      0 },
	{    164,  33780,      9,      0 },
	{    165,  33780,      8,      0 },
	{    166,  33780,      7,      0 },
	{    167,  33780,      6,      0 },
	{    168,  33780,      5,      0 },
	{    169,  33780,      4,      0 },
	{    170,  33780,      3,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[48] = {
	  8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
	 18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
	 28,  29,  30,  31,  32,  81,  82,  83,  84,  85,
	 86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	 96,  97,  98,  99, 100, 101, 102, 103,
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
	0x0D, 0x00, 0x00,

	0x1E, 0x40, 0x06,
	0x49, 0x00, 0x00,
	0x03, 0x07, 0x9B,
	0x04, 0x0A, 0x23,
	0x05, 0x00, 0x00,
	0x06, 0x00, 0x1A,
	0x2B, 0x00, 0x08,
	0x2C, 0x00, 0x08,
	0x2D, 0x00, 0x08,
	0x2E, 0x00, 0x08,
	0x30, 0x00, 0x01,
	0x27, 0x00, 0x0B,
	0x35, 0x00, 0x1B,
	0x09, 0x02, 0x6A,
	0x08, 0x00, 0x00,
	0x3E, 0x80, 0xC7,
	0x41, 0x00, 0x00,
	0x42, 0x00, 0x03,
	0x43, 0x00, 0x03,
	0x4F, 0x00, 0x14,
	0x70, 0x00, 0xAC,
	0x71, 0xA7, 0x00,
	0x72, 0xA7, 0x00,
	0x73, 0x0C, 0x00,
	0x74, 0x06, 0x00,
	0x75, 0x56, 0x17,
	0x76, 0x6B, 0x57,
	0x77, 0x6B, 0x57,
	0x78, 0xA5, 0x00,
	0x79, 0xAB, 0x00,
	0x7A, 0xA9, 0x04,
	0x7B, 0xA7, 0x00,
	0x7C, 0xA7, 0x00,
	0x7D, 0xFF, 0x00,
	0x7E, 0xA9, 0x00,
	0x7F, 0xA9, 0x00,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	//0x03, 0x07, 0x9B,
	//0x1E, 0x40, 0x06,
	0x22, 0x00, 0x03,
	0x23, 0x00, 0x03,
	//0x0B, 0x00, 0x01,
	//0x08, 0x00, 0x00,
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};
#endif

static code UINT8 frontSnapCmdTbl[] = {
	0x22, 0x00, 0x00,
	0x23, 0x00, 0x00,
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
static void frontAllRegDump(UINT8 dataLen);

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
	UINT8 id;
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

	id = frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));
	if ( id != NULL ) {
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AFTER_SNAP_SHOT, frontOpAfterSnapShot);
	}

	return id;
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


	/* before power on. */
	frontBeforePowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_30FPS_PREV, 0);
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

	//frontAllRegDump(2);
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
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_30FPS_PREV, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);

	//frontAllRegDump(2);
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

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_SNAP, 0);
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
		frontSnapExposureTimeSet(mode,scaleUp);
	}

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
	cdspSkipWrSyncSet(1);	
	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);

	//i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	//frontAllRegDump(2);
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
	UINT8 burst = (UINT8)(*(UINT8 *)parg);

	if( burst == 0 ) {
		i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	}

	return;
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

	cmdTbl[0] = 0x35;
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
//	XBYTE[0x21D3] = 0x02;
//	XBYTE[0x21D0] = 0x02;

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

static void
frontAllRegDump(
	UINT8 dataLen
)
{
	UINT8 addrTbl[1], dataTbl[1*8];
	UINT16 regIdx;
	UINT8 dataIdx;


	if ( dataLen == 0 || dataLen >= 8 ) {
		printf("data len err\n");
		return;
	}

	for ( regIdx = 0; regIdx < 256; regIdx++ ) {
		addrTbl[0] = regIdx;
		i2cRegRead(addrTbl, dataTbl, dataLen, 1, 1, 0, 0);
		printf("reg[0x%02X] =", regIdx);
		for ( dataIdx = 0 ; dataIdx < dataLen; dataIdx++ ) {
			printf(" 0x%02bX", dataTbl[dataIdx]);
		}
		printf("\n");
	}
}

