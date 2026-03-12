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
 * @file		front_ov_ov3630_raw.c
 * @brief		front sensor omni vision ov3630 driver.
 * @author		Matt Wang
 * @since		2008-10-14
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
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "cdsp_misc.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      1024
#define HEIGHT_30FPS_PREV     768
#define XOFF_30FPS_PREV       136
#define YOFF_30FPS_PREV       7
#define LINE_TOT_30FPS_PREV   1152
#define FRAME_TOT_30FPS_PREV  790

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      2048
#define HEIGHT_SNAP     1536
#define XOFF_SNAP       280
#define YOFF_SNAP       15
#define LINE_TOT_SNAP   2320
#define FRAME_TOT_SNAP  1567

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
#define PCLK_FREQ_30FPS_PREV  27600000L
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
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  39
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

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  1

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x02
#define BAYER_PTN_SNAP  0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  120
#define AE_30FPS_30_MIN_IDX  13
#define AE_30FPS_25_MAX_IDX  120
#define AE_30FPS_25_MIN_IDX  13
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  46
#define GAIN_30FPS_MIN_IDX  5

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0
#endif

#define GAIN_LARGE   45
#define GAIN_MIDDLE  30
#define GAIN_SMALL   15

#define EXP_GAIN_SET_POS  0x40
#define GAIN_AFTER_EXP    0x01

#undef _CHANGE_COLOR_MATRIX_EN_ // off change color matrix

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
__STATIC void frontOpSnapShot(UINT8 mode);
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
static code UINT8 frontDevName[] = "SENSOR - OV OV3630 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    300,     46,      0 },
	{     51,    300,     46,      0 },
	{     52,    300,     46,      0 },
	{     53,    300,     46,      0 },
	{     54,    300,     46,      0 },
	{     55,    300,     46,      0 },
	{     56,    300,     46,      0 },
	{     57,    300,     46,      0 },
	{     58,    300,     46,      0 },
	{     59,    300,     46,      0 },
	{     60,    300,     46,      0 },
	{     61,    300,     46,      0 },
	{     62,    300,     46,      0 },
	{     63,    300,     46,      0 },
	{     64,    300,     46,      0 },
	{     65,    300,     45,      0 },
	{     66,    300,     43,      0 },
	{     67,    300,     42,      0 },
	{     68,    300,     40,      0 },
	{     69,    300,     38,      0 },
	{     70,    300,     37,      0 },
	{     71,    300,     35,      0 },
	{     72,    300,     34,      0 },
	{     73,    300,     32,      0 },
	{     74,    300,     31,      0 },
	{     75,    300,     29,      0 },
	{     76,    300,     27,      0 },
	{     77,    300,     25,      0 },
	{     78,    300,     23,      0 },
	{     79,    300,     21,      0 },
	{     80,    300,     20,      0 },
	{     81,    300,     19,      0 },
	{     82,    300,     18,      0 },
	{     83,    300,     16,      0 },
	{     84,    300,     15,      0 },
	{     85,    300,     14,      0 },
	{     86,    300,     12,      0 },
	{     87,    300,     10,      0 },
	{     88,    300,      8,      0 },
	{     89,    400,     14,      0 },
	{     90,    400,     12,      0 },
	{     91,    400,     10,      0 },
	{     92,    400,      9,      0 },
	{     93,    400,      8,      0 },
	{     94,    600,     16,      0 },
	{     95,    600,     14,      0 },
	{     96,    600,     12,      0 },
	{     97,    600,     11,      0 },
	{     98,    600,      9,      0 },
	{     99,    600,      8,      0 },
	{    100,   1200,     21,      0 },
	{    101,   1200,     20,      0 },
	{    102,   1200,     17,      0 },
	{    103,   1200,     16,      0 },
	{    104,   1200,     15,      0 },
	{    105,   1200,     14,      0 },
	{    106,   1200,     12,      0 },
	{    107,   1200,     10,      0 },
	{    108,   1200,      8,      0 },
	{    109,   1757,     16,      0 },
	{    110,   1757,     15,      0 },
	{    111,   1757,     13,      0 },
	{    112,   1757,     11,      0 },
	{    113,   1757,     10,      0 },
	{    114,   1757,      8,      0 },
	{    115,   1884,      8,      0 },
	{    116,   2020,      8,      0 },
	{    117,   2165,      8,      0 },
	{    118,   2321,      8,      0 },
	{    119,   2488,      8,      0 },
	{    120,   2667,      8,      0 },
	{    121,   2859,      8,      0 },
	{    122,   3065,      8,      0 },
	{    123,   3286,      8,      0 },
	{    124,   3522,      8,      0 },
	{    125,   3775,      8,      0 },
	{    126,   4047,      8,      0 },
	{    127,   4338,      8,      0 },
	{    128,   4650,      8,      0 },
	{    129,   4984,      8,      0 },
	{    130,   5342,      8,      0 },
	{    131,   5726,      8,      0 },
	{    132,   6138,      8,      0 },
	{    133,   6579,      8,      0 },
	{    134,   7052,      8,      0 },
	{    135,   7559,      8,      0 },
	{    136,   8102,      8,      0 },
	{    137,   8684,      8,      0 },
	{    138,   9308,      8,      0 },
	{    139,   9977,      8,      0 },
	{    140,  10694,      8,      0 },
	{    141,  11462,      8,      0 },
	{    142,  12285,      8,      0 },
	{    143,  13167,      8,      0 },
	{    144,  14113,      8,      0 },
	{    145,  15127,      8,      0 },
	{    146,  16213,      8,      0 },
	{    147,  17377,      8,      0 },
	{    148,  18625,      8,      0 },
	{    149,  21395,      8,      0 },
	{    150,  21395,      8,      0 },
	{    151,  22931,      8,      0 },
	{    152,  24577,      8,      0 },
	{    153,  26341,      8,      0 },
	{    154,  28232,      8,      0 },
	{    155,  30259,      8,      0 },
	{    156,  32431,      8,      0 },
	{    157,  34759,      8,      0 },
	{    158,  37254,      8,      0 },
	{    159,  39928,      8,      0 },
	{    160,  42794,      9,      0 },
	{    161,  42794,      8,      0 },
	{    162,  45866,      8,      0 },
	{    163,  49158,      9,      0 },
	{    164,  52687,      8,      0 },
	{    165,  56469,      8,      0 },
	{    166,  60523,     10,      0 },
	{    167,  64867,      8,      0 },
	{    168,  69523,      8,      0 },
	{    169,  74513,      8,      0 },
	{    170,  79862,     10,      0 },
};

static code frontAe_t ae30fps50hzTbl[121] = {
	{     50,    333,     46,      0 },
	{     51,    333,     46,      0 },
	{     52,    333,     46,      0 },
	{     53,    333,     46,      0 },
	{     54,    333,     46,      0 },
	{     55,    333,     46,      0 },
	{     56,    333,     46,      0 },
	{     57,    333,     46,      0 },
	{     58,    333,     46,      0 },
	{     59,    333,     46,      0 },
	{     60,    333,     46,      0 },
	{     61,    333,     46,      0 },
	{     62,    333,     46,      0 },
	{     63,    333,     46,      0 },
	{     64,    333,     46,      0 },
	{     65,    333,     44,      0 },
	{     66,    333,     42,      0 },
	{     67,    333,     40,      0 },
	{     68,    333,     39,      0 },
	{     69,    333,     37,      0 },
	{     70,    333,     36,      0 },
	{     71,    333,     34,      0 },
	{     72,    333,     33,      0 },
	{     73,    333,     32,      0 },
	{     74,    333,     30,      0 },
	{     75,    333,     28,      0 },
	{     76,    333,     26,      0 },
	{     77,    333,     24,      0 },
	{     78,    333,     22,      0 },
	{     79,    333,     20,      0 },
	{     80,    333,     19,      0 },
	{     81,    333,     18,      0 },
	{     82,    333,     17,      0 },
	{     83,    333,     16,      0 },
	{     84,    333,     14,      0 },
	{     85,    333,     12,      0 },
	{     86,    333,     11,      0 },
	{     87,    333,      9,      0 },
	{     88,    333,      8,      0 },
	{     89,    500,     16,      0 },
	{     90,    500,     15,      0 },
	{     91,    500,     13,      0 },
	{     92,    500,     11,      0 },
	{     93,    500,     10,      0 },
	{     94,    500,      8,      0 },
	{     95,    500,      8,      0 },
	{     96,   1000,     21,      0 },
	{     97,   1000,     19,      0 },
	{     98,   1000,     18,      0 },
	{     99,   1000,     17,      0 },
	{    100,   1000,     16,      0 },
	{    101,   1000,     14,      0 },
	{    102,   1000,     10,      0 },
	{    103,   1000,      8,      0 },
	{    104,   1000,      8,      0 },
	{    105,   1000,      8,      0 },
	{    106,   1005,      8,      0 },
	{    107,   1078,      8,      0 },
	{    108,   1156,      8,      0 },
	{    109,   1240,      8,      0 },
	{    110,   1330,      8,      0 },
	{    111,   1426,      8,      0 },
	{    112,   1529,      8,      0 },
	{    113,   1639,      8,      0 },
	{    114,   1757,      8,      0 },
	{    115,   1884,      8,      0 },
	{    116,   2020,      8,      0 },
	{    117,   2165,      8,      0 },
	{    118,   2321,      8,      0 },
	{    119,   2488,      8,      0 },
	{    120,   2667,      8,      0 },
	{    121,   2859,      8,      0 },
	{    122,   3065,      8,      0 },
	{    123,   3286,      8,      0 },
	{    124,   3522,      8,      0 },
	{    125,   3775,      8,      0 },
	{    126,   4047,      8,      0 },
	{    127,   4338,      8,      0 },
	{    128,   4650,      8,      0 },
	{    129,   4984,      8,      0 },
	{    130,   5342,      8,      0 },
	{    131,   5726,      8,      0 },
	{    132,   6138,      8,      0 },
	{    133,   6579,      8,      0 },
	{    134,   7052,      8,      0 },
	{    135,   7559,      8,      0 },
	{    136,   8102,      8,      0 },
	{    137,   8684,      8,      0 },
	{    138,   9308,      8,      0 },
	{    139,   9977,      8,      0 },
	{    140,  10694,      8,      0 },
	{    141,  11462,      8,      0 },
	{    142,  12285,      8,      0 },
	{    143,  13167,      8,      0 },
	{    144,  14113,      8,      0 },
	{    145,  15127,      8,      0 },
	{    146,  16213,      8,      0 },
	{    147,  17377,      8,      0 },
	{    148,  18625,      8,      0 },
	{    149,  21395,      8,      0 },
	{    150,  21395,      8,      0 },
	{    151,  22931,      8,      0 },
	{    152,  24577,      8,      0 },
	{    153,  26341,      8,      0 },
	{    154,  28232,      8,      0 },
	{    155,  30259,      8,      0 },
	{    156,  32431,      8,      0 },
	{    157,  34759,      8,      0 },
	{    158,  37254,      8,      0 },
	{    159,  39928,      8,      0 },
	{    160,  42794,      9,      0 },
	{    161,  42794,      8,      0 },
	{    162,  45866,      8,      0 },
	{    163,  49158,      9,      0 },
	{    164,  52687,      8,      0 },
	{    165,  56469,      8,      0 },
	{    166,  60523,     10,      0 },
	{    167,  64867,      8,      0 },
	{    168,  69523,      8,      0 },
	{    169,  74513,      8,      0 },
	{    170,  79862,     10,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[76] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

#if 0
static code UINT16 gain60fpsTbl[0] = {
};
#endif

/* color matrix table. */
static code UINT16 colorMatrixTbl[3][9] = {
	{ 0x0058, 0x03D7, 0x0011, 0x03DF, 0x007A, 0x03E7, 0x03EB, 0x03DA, 0x007B },
	{ 0x0045, 0x03EF, 0x000C, 0x03E8, 0x006B, 0x03EC, 0x03F0, 0x03F4, 0x005C },
	{ 0x0029, 0x0010, 0x0006, 0x03F4, 0x0058, 0x03F4, 0x03F8, 0x0015, 0x0033 },
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
	0x3C, 0xB7,
	0x3C, 0x37,
	0x0E, 0x19,
	0x14, 0x98,
	0x33, 0x38,
	0x34, 0x20,
	0x37, 0x81,
	0x44, 0x00,
	0x11, 0x40,
	0x39, 0x02,
	0x4A, 0x0A,
	0x03, 0x0F,
	0x48, 0x80,
	0x21, 0x90,
	0x42, 0x03,
	0x5C, 0x40,
	0x60, 0x06,
	0x1E, 0xC0,
	0x36, 0x12,
	0x51, 0x00,
	0x52, 0x40,
	0x59, 0x00,
	0x7E, 0x08,
	0x53, 0x10,
	0x59, 0x01,
	0x7E, 0x20,
	0x59, 0x02,
	0x7E, 0x03,
	0x54, 0x08,
	0x55, 0x85,
	0x58, 0xC1,
	0x56, 0x28,
	0x20, 0x80,
	0x28, 0x30,
	0x6D, 0x00,
	0x6E, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6A, 0x00,
	0x6B, 0x00,
	0x70, 0x01,
	0x71, 0x94,
	0x73, 0xC1,
	0x36, 0x40,
	0x3E, 0x60,
	0x41, 0x40,
	0x6C, 0x00,
	0x70, 0x02,
	0x71, 0x94,
	0x1B, 0x02,
	0x13, 0x80,
	0x01, 0x80,
	0x02, 0x80,
	0x16, 0x80,
	0x43, 0x84,
	0x38, 0xA4,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x3C, 0xB7,
	0x3C, 0x37,
	0x11, 0x40,
	0x12, 0x40,
	0x0D, 0x26,
	0x17, 0x10,
	0x18, 0x50,
	0x19, 0x00,
	0x1A, 0x60,
	0x32, 0xA4,
	0x15, 0x42,
	0x43, 0x84,
	0x38, 0xA4,
	0x18, 0x01,
	0x03, 0x0F,
	0x2B, 0x00,
	0x3D, 0x25,
	0x2D, 0x00,
	0x2E, 0x00,
	0x45, 0x00,
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};
#endif

static code UINT8 frontSnapCmdTbl[] = {
	0x12, 0x00,
	0x17, 0x11,
	0x11, 0x00,
	0x18, 0x00,
	0x19, 0x01,
	0x1A, 0xC2,
	0x03, 0x03,
	0x2B, 0x20,
	0x32, 0x34,
	0x38, 0xA0,
	0x3D, 0x31,
};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;
static UINT8 frontGainClass = 0xFF;

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
	globalTimerWait(300);
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

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);
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
 * @since	2008-10-14
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpSnapShot(
	UINT8 first
)
{
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontSnapExposureTimeSet();
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
	UINT8 cmdTbl[2*2];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 2) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = 0x24 | (espline & 0x03);
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
	UINT32 tmp0, pixclk;
	UINT8 cmdTbl[2*6];
	UINT16 s_gain, lastt_gain;


	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 20) / (ltotal * frontLastShutter);
	s_gain = frontLastGain;

	if ( s_gain >= 0x30 ) {
		lastt_gain = s_gain - 32;
		espline = espline * 2;
	}
	else if ( s_gain >= 0x10 ) {
		lastt_gain = s_gain - 16;
		espline = espline * 2;
	}
	else {
		lastt_gain = s_gain;
		espline = espline;
	}

	cdspColorMatrixRestore();
//	XBYTE[0x21D3] = 0x02;
//	XBYTE[0x21D0] = 0x02;
	frontGainClass = 0;

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
		espline = ftotal - 1;
	}
	else {
		tmp0 = 0;
	};

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 2) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = 0x24 | (espline & 0x03);
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
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-14
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
			#ifdef _CHANGE_COLOR_MATRIX_EN_
			HAL_CdspColorMSet_Intr(colorMatrixTbl[2][0], colorMatrixTbl[2][1], colorMatrixTbl[2][2],
			                       colorMatrixTbl[2][3], colorMatrixTbl[2][4], colorMatrixTbl[2][5],
			                       colorMatrixTbl[2][6], colorMatrixTbl[2][7], colorMatrixTbl[2][8]);
			#endif
			XBYTE[REG_Cdsp_Yhdiv] = 0x08;
			XBYTE[REG_Cdsp_EdgeEn] = 0x08;

			frontGainClass = 3;
		}
	}
	else if ( gain > GAIN_MIDDLE ) {
		if ( frontGainClass != 2 ) {
			#ifdef _CHANGE_COLOR_MATRIX_EN_
			HAL_CdspColorMSet_Intr(colorMatrixTbl[1][0], colorMatrixTbl[1][1], colorMatrixTbl[1][2],
			                       colorMatrixTbl[1][3], colorMatrixTbl[1][4], colorMatrixTbl[1][5],
			                       colorMatrixTbl[1][6], colorMatrixTbl[1][7], colorMatrixTbl[1][8]);
			#endif
			XBYTE[REG_Cdsp_Yhdiv] = 0x06;
			XBYTE[REG_Cdsp_EdgeEn] = 0x08;

			frontGainClass = 2;
		}
	}
	else if ( gain > GAIN_SMALL ) {
		if ( frontGainClass != 1 ) {
			#ifdef _CHANGE_COLOR_MATRIX_EN_
			HAL_CdspColorMSet_Intr(colorMatrixTbl[0][0], colorMatrixTbl[0][1], colorMatrixTbl[0][2],
			                       colorMatrixTbl[0][3], colorMatrixTbl[0][4], colorMatrixTbl[0][5],
			                       colorMatrixTbl[0][6], colorMatrixTbl[0][7], colorMatrixTbl[0][8]); // for hgnoise
			#endif
			XBYTE[REG_Cdsp_Yhdiv] = 0x04;
			XBYTE[REG_Cdsp_EdgeEn] = 0x0A;

			frontGainClass = 1;
		}
	}
	else {
		if ( frontGainClass != 0 ) {
			cdspColorMatrixRestore();
			XBYTE[REG_Cdsp_Yhdiv] = 0x02;
			XBYTE[REG_Cdsp_EdgeEn] = 0x02;

			frontGainClass = 0;
		}
	};
}

