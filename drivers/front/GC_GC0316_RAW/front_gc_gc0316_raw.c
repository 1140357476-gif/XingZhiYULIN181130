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
 * @file		front_gc_gc0316_raw.c
 * @brief		front sensor galaxy core gc0316 driver.
 * @author		Matt Wang
 * @since		2008-10-22
 * @date		2008-12-19
 */

/*
 * History
 *
 * 1000, 20081022 Matt Wang
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

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       142
#define YOFF_30FPS_PREV       2
#define LINE_TOT_30FPS_PREV   789
#define FRAME_TOT_30FPS_PREV  504

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
#define XOFF_SNAP       144
#define YOFF_SNAP       2
#define LINE_TOT_SNAP   789
#define FRAME_TOT_SNAP  504

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    4
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    2
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
#define PCLK_FREQ_SNAP        12000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#if 0
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#endif
#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  56
#define RESHAPE_HRISE_30FPS_PREV  60
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VFALL_30FPS_PREV  4
#define RESHAPE_VRISE_30FPS_PREV  5

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
#define RESHAPE_HFALL_SNAP  56
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  4
#define RESHAPE_VRISE_SNAP  5

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x03
#define BAYER_PTN_SNAP  0x03

/* i2c interface. */
#define I2C_DEV_ADDR  0x42
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  116
#define AE_30FPS_30_MIN_IDX  20
#define AE_30FPS_25_MAX_IDX  120
#define AE_30FPS_25_MIN_IDX  30
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  62
#define GAIN_30FPS_MIN_IDX  8

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
__STATIC void frontOpSnapShot(UINT8 first,UINT8 mode,UINT8 scaleUp);
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
static code UINT8 frontDevName[] = "SENSOR - GC GC0316 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    333,    114,      0 },
	{     51,    333,    114,      0 },
	{     52,    333,    114,      0 },
	{     53,    333,    114,      0 },
	{     54,    333,    114,      0 },
	{     55,    333,    114,      0 },
	{     56,    333,    114,      0 },
	{     57,    333,    114,      0 },
	{     58,    333,    114,      0 },
	{     59,    333,    114,      0 },
	{     60,    333,    114,      0 },
	{     61,    333,    114,      0 },
	{     62,    333,    114,      0 },
	{     63,    333,    114,      0 },
	{     64,    333,    114,      0 },
	{     65,    333,    114,      0 },
	{     66,    333,    114,      0 },
	{     67,    333,    112,      0 },
	{     68,    333,    108,      0 },
	{     69,    333,    104,      0 },
	{     70,    333,    100,      0 },
	{     71,    333,     97,      0 },
	{     72,    333,     94,      0 },
	{     73,    333,     90,      0 },
	{     74,    333,     87,      0 },
	{     75,    333,     83,      0 },
	{     76,    333,     79,      0 },
	{     77,    333,     75,      0 },
	{     78,    333,     72,      0 },
	{     79,    333,     70,      0 },
	{     80,    333,     66,      0 },
	{     81,    333,     64,      0 },
	{     82,    333,     62,      0 },
	{     83,    333,     59,      0 },
	{     84,    333,     55,      0 },
	{     85,    333,     51,      0 },
	{     86,    333,     47,      0 },
	{     87,    333,     44,      0 },
	{     88,    333,     41,      0 },
	{     89,    333,     38,      0 },
	{     90,    333,     35,      0 },
	{     91,    500,     50,      0 },
	{     92,    500,     46,      0 },
	{     93,    500,     43,      0 },
	{     94,    500,     41,      0 },
	{     95,    500,     38,      0 },
	{     96,    500,     35,      0 },
	{     97,    500,     32,      0 },
	{     98,   1000,     61,      0 },
	{     99,   1000,     58,      0 },
	{    100,   1000,     55,      0 },
	{    101,   1000,     52,      0 },
	{    102,   1000,     47,      0 },
	{    103,   1000,     44,      0 },
	{    104,   1000,     40,      0 },
	{    105,   1000,     37,      0 },
	{    106,   1000,     35,      0 },
	{    107,   1000,     32,      0 },
	{    108,   1000,     30,      0 },
	{    109,   1000,     28,      0 },
	{    110,   1000,     26,      0 },
	{    111,   1266,     31,      0 },
	{    112,   1357,     31,      0 },
	{    113,   1455,     31,      0 },
	{    114,   1560,     31,      0 },
	{    115,   1673,     31,      0 },
	{    116,   1794,     31,      0 },
	{    117,   1923,     31,      0 },
	{    118,   2062,     31,      0 },
	{    119,   2210,     31,      0 },
	{    120,   2369,     31,      0 },
	{    121,   2540,     31,      0 },
	{    122,   2723,     31,      0 },
	{    123,   2919,     31,      0 },
	{    124,   3129,     31,      0 },
	{    125,   3354,     31,      0 },
	{    126,   3595,     31,      0 },
	{    127,   3854,     31,      0 },
	{    128,   4131,     31,      0 },
	{    129,   4428,     31,      0 },
	{    130,   4746,     31,      0 },
	{    131,   5087,     31,      0 },
	{    132,   5453,     31,      0 },
	{    133,   5845,     31,      0 },
	{    134,   6265,     31,      0 },
	{    135,   6715,     31,      0 },
	{    136,   7197,     31,      0 },
	{    137,   7714,     31,      0 },
	{    138,   8268,     31,      0 },
	{    139,   8862,     31,      0 },
	{    140,   9499,     31,      0 },
	{    141,  10181,     31,      0 },
	{    142,  10912,     31,      0 },
	{    143,  11696,     31,      0 },
	{    144,  12536,     31,      0 },
	{    145,  13436,     31,      0 },
	{    146,  14401,     31,      0 },
	{    147,  15435,     31,      0 },
	{    148,  16543,     31,      0 },
	{    149,  17731,     31,      0 },
	{    150,  19004,     30,      0 },
	{    151,  20369,     29,      0 },
	{    152,  21832,     31,      0 },
	{    153,  23400,     30,      0 },
	{    154,  25080,     30,      0 },
	{    155,  26881,     30,      0 },
	{    156,  28811,     30,      0 },
	{    157,  30879,     30,      0 },
	{    158,  33096,     28,      0 },
	{    159,  35472,     26,      0 },
	{    160,  38019,     24,      0 },
	{    161,  40748,     27,      0 },
	{    162,  43673,     26,      0 },
	{    163,  46808,     24,      0 },
	{    164,  50168,     22,      0 },
	{    165,  53769,     27,      0 },
	{    166,  57629,     25,      0 },
	{    167,  61766,     23,      0 },
	{    168,  66200,      9,      0 },
	{    169,  70952,      9,      0 },
	{    170,  76045,      9,      0 },
};

static code frontAe_t ae30fps50hzTbl[121] = {
	{     50,    333,    253,      0 },
	{     51,    333,    253,      0 },
	{     52,    333,    253,      0 },
	{     53,    333,    253,      0 },
	{     54,    333,    253,      0 },
	{     55,    333,    253,      0 },
	{     56,    333,    253,      0 },
	{     57,    333,    253,      0 },
	{     58,    333,    253,      0 },
	{     59,    333,    253,      0 },
	{     60,    333,    253,      0 },
	{     61,    333,    253,      0 },
	{     62,    333,    253,      0 },
	{     63,    333,    253,      0 },
	{     64,    333,    253,      0 },
	{     65,    333,    253,      0 },
	{     66,    333,    253,      0 },
	{     67,    333,    253,      0 },
	{     68,    333,    253,      0 },
	{     69,    333,    253,      0 },
	{     70,    333,    253,      0 },
	{     71,    333,    253,      0 },
	{     72,    333,    253,      0 },
	{     73,    333,    253,      0 },
	{     74,    333,    234,      0 },
	{     75,    333,    217,      0 },
	{     76,    333,    203,      0 },
	{     77,    333,    194,      0 },
	{     78,    333,    180,      0 },
	{     79,    333,    165,      0 },
	{     80,    333,    156,      0 },//66
	{     81,    333,    145,      0 },
	{     82,    333,    137,      0 },
	{     83,    333,    127,      0 },
	{     84,    333,    119,      0 },
	{     85,    333,    110,      0 },
	{     86,    333,    103,      0 },
	{     87,    333,     95,      0 },
	{     88,    333,     89,      0 },
	{     89,    333,     83,      0 },
	{     90,    333,     76,      0 },
	{     91,    500,    108,      0 },
	{     92,    500,    102,      0 },
	{     93,    500,     94,      0 },
	{     94,    500,     87,      0 },
	{     95,    500,     81,      0 },
	{     96,    500,     75,      0 },
	{     97,    500,     71,      0 },
	{     98,   1000,    134,      0 },
	{     99,   1000,    124,      0 },
	{    100,   1000,    117,      0 },
	{    101,   1000,    109,      0 },
	{    102,   1000,    104,      0 },
	{    103,   1000,     96,      0 },
	{    104,   1000,     89,      0 },
	{    105,   1000,     82,      0 },
	{    106,   1000,     76,      0 },
	{    107,   1000,     71,      0 },
	{    108,   1000,     66,      0 },
	{    109,   1000,     61,      0 },
	{    110,   1000,     57,      0 },
	{    111,   1266,     68,      0 },
	{    112,   1357,     68,      0 },
	{    113,   1455,     68,      0 },
	{    114,   1560,     68,      0 },
	{    115,   1673,     68,      0 },
	{    116,   1794,     68,      0 },
	{    117,   1923,     68,      0 },
	{    118,   2062,     68,      0 },
	{    119,   2210,     68,      0 },
	{    120,   2369,     68,      0 },
	{    121,   2540,     68,      0 },
	{    122,   2723,     68,      0 },
	{    123,   2919,     68,      0 },
	{    124,   3129,     68,      0 },
	{    125,   3354,     68,      0 },
	{    126,   3595,     68,      0 },
	{    127,   3854,     68,      0 },
	{    128,   4131,     68,      0 },
	{    129,   4428,     68,      0 },
	{    130,   4746,     68,      0 },
	{    131,   5087,     68,      0 },
	{    132,   5453,     68,      0 },
	{    133,   5845,     68,      0 },
	{    134,   6265,     68,      0 },
	{    135,   6715,     68,      0 },
	{    136,   7197,     68,      0 },
	{    137,   7714,     68,      0 },
	{    138,   8268,     67,      0 },
	{    139,   8862,     66,      0 },
	{    140,   9499,     65,      0 },
	{    141,  10181,     70,      0 },
	{    142,  10912,     70,      0 },
	{    143,  11696,     65,      0 },
	{    144,  12536,     65,      0 },
	{    145,  13436,     65,      0 },
	{    146,  14401,     67,      0 },
	{    147,  15435,     69,      0 },
	{    148,  16543,     64,      0 },
	{    149,  17731,     66,      0 },
	{    150,  19004,     62,      0 },
	{    151,  20369,     65,      0 },
	{    152,  21832,     70,      0 },
	{    153,  23400,     65,      0 },
	{    154,  25080,     61,      0 },
	{    155,  26881,     66,      0 },
	{    156,  28811,     62,      0 },
	{    157,  30879,     69,      0 },
	{    158,  33096,     64,      0 },
	{    159,  35472,     60,      0 },
	{    160,  38019,     55,      0 },
	{    161,  40748,     65,      0 },
	{    162,  43673,     60,      0 },
	{    163,  46808,     55,      0 },
	{    164,  50168,     52,      0 },
	{    165,  53769,     65,      0 },
	{    166,  57629,     60,      0 },
	{    167,  61766,     56,      0 },
	{    168,  66200,     20,      0 },
	{    169,  70952,     20,      0 },
	{    170,  76045,     20,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[254] = {

	  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,
	 17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,
	 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	 49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
	 65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
	 81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,
	 97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
	113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
	129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
	145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
	161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
	177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
	193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
	209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
	225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
	241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
};

#if 0
static code UINT8 gain60fpsTbl[0] = {
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
	0x13, 0x80,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x13, 0x00,
	0x01, 0x14,
	0x02, 0x10,
	0x03, 0x01,
	0x04, 0xF8,
	0x05, 0x00,
	0x06, 0x00,
	0x07, 0x00,
	0x08, 0x00,
	0x09, 0x01,
	0x0A, 0xE8,
	0x0B, 0x02,
	0x0C, 0x90,
	0x0D, 0x01,  //0x03
	0x0F, 0x00,  //flip
	0x10, 0x24,
	0x11, 0x00,
	0x12, 0x10,
	0x14, 0x00,
	0x15, 0x08,
	0x16, 0x06,
	0x17, 0x08,
	0x18, 0x02,
	0x19, 0x44,
	0x1A, 0x44,
	0x1B, 0x00,
	0x1C, 0x02,
	0x1D, 0x02,
	0x1E, 0x00,
	0x1F, 0x00,
	0x35, 0x6D,
	0x36, 0x28,
	0x40, 0x0C,
	0x41, 0x00,
	0x42, 0x30,
	0x43, 0x40,
	0x44, 0xF8,
	0x47, 0x46,
	0x4E, 0x26,
	0x59, 0x30,
	0x61, 0x80,
	0x63, 0x80,
	0x65, 0x80,
	0x67, 0x80,
	0x68, 0x05,
	0x7E, 0x44,
	0x7F, 0x84,
	0x80, 0x10,
	0x81, 0x10,
	0x82, 0x44,
	0x83, 0x14,
	0x84, 0x40,
	0x85, 0x04,
	0x86, 0xFF,
	0x87, 0x93,
	0xD6, 0x98,
	0xD8, 0x00,
	0xDC, 0x40,
	0xDB, 0x40,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x68, 0x04,
	0xDC, 0xC0,
	0x19, 0x00,
	0x1A, 0x00,
	0x03, 0x00,
	0x04, 0xF0,
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};

static code UINT8 frontSnapCmdTbl[] = {
};
#endif

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
	globalTimerWait(100);
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
		frontSnapExposureTimeSet();
	}

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo

	cdspSkipAeWr(1);
	cdspSkipAwbWr(1);
	cdspSkipDistEdgeWr(1);
	
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
 * @since	2008-10-22
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

	cmdTbl[0] = 0xDC;
	cmdTbl[1] = frontLastGain;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, isr, upd);

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
 * @since	2008-10-22
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

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * parg->val);
	frontLastShutter = espline;

	cmdTbl[0] = 0x03;
	cmdTbl[1] = (espline >> 8) & 0x0F;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0xFF;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, parg->isr, parg->upd);
}

/**
 * @brief	operating function to get optical black status.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
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
 * @since	2008-10-22
 * @todo	N/A
 * @bug		N/A
*/
static void
frontSnapExposureTimeSet(
	void
)
{
	UINT16 espline, s_gain;
	UINT8 cmdTbl[2*3];


	espline = frontLastShutter;
	s_gain = frontLastGain;

	if ( s_gain > 100 ) {
		espline <<= 1;
		s_gain >>= 1;
	}

	cmdTbl[0] = 0x03;
	cmdTbl[1] = (espline >> 8) & 0x0F;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0xFF;
	cmdTbl[4] = 0xDC;
	cmdTbl[5] = s_gain;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-22
 * @todo	N/A
 * @bug		N/A
*/
static void
frontColorNoiseReduce(
	UINT8 gain
)
{

}

