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
 * @file		front_pa_pas6371lt_raw.c
 * @brief		front sensor pixart imaging pas6371lt driver.
 * @author		Matt Wang
 * @since		2008-10-16
 * @date		2008-12-17
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

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  2

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       4
#define YOFF_30FPS_PREV       4
#define LINE_TOT_30FPS_PREV   786
#define FRAME_TOT_30FPS_PREV  508

#define WIDTH_60FPS_PREV      320
#define HEIGHT_60FPS_PREV     240
#define XOFF_60FPS_PREV       2
#define YOFF_60FPS_PREV       2
#define LINE_TOT_60FPS_PREV   460
#define FRAME_TOT_60FPS_PREV  248

#define WIDTH_30FPS_SNAP      640
#define HEIGHT_30FPS_SNAP     480
#define XOFF_30FPS_SNAP       4
#define YOFF_30FPS_SNAP       4
#define LINE_TOT_30FPS_SNAP   786
#define FRAME_TOT_30FPS_SNAP  508

#define WIDTH_60FPS_SNAP      320
#define HEIGHT_60FPS_SNAP     240
#define XOFF_60FPS_SNAP       2
#define YOFF_60FPS_SNAP       2
#define LINE_TOT_60FPS_SNAP   460
#define FRAME_TOT_60FPS_SNAP  248

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    4
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    4
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_30FPS_SNAP    4
#define MCLK_PHASE_30FPS_SNAP  0
#define PCLK_DIV_30FPS_SNAP    1
#define PCLK_PHASE_30FPS_SNAP  0

#define MCLK_DIV_60FPS_SNAP    4
#define MCLK_PHASE_60FPS_SNAP  0
#define PCLK_DIV_60FPS_SNAP    1
#define PCLK_PHASE_60FPS_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  12000000L
#define PCLK_FREQ_60FPS_PREV  12000000L
#define PCLK_FREQ_30FPS_SNAP  12000000L
#define PCLK_FREQ_60FPS_SNAP  12000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_SNAP  96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_60FPS_SNAP  96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HFALL_30FPS_PREV  30
#define RESHAPE_HRISE_30FPS_PREV  40
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VFALL_30FPS_PREV  5
#define RESHAPE_VRISE_30FPS_PREV  4

#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HFALL_60FPS_PREV  30
#define RESHAPE_HRISE_60FPS_PREV  40
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VFALL_60FPS_PREV  5
#define RESHAPE_VRISE_60FPS_PREV  4

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_30FPS_SNAP    0
#define RESHAPE_HFALL_30FPS_SNAP  2
#define RESHAPE_HRISE_30FPS_SNAP  4
#define RESHAPE_VEN_30FPS_SNAP    0
#define RESHAPE_VFALL_30FPS_SNAP  2
#define RESHAPE_VRISE_30FPS_SNAP  3

#define RESHAPE_HEN_60FPS_SNAP    0
#define RESHAPE_HFALL_60FPS_SNAP  2
#define RESHAPE_HRISE_60FPS_SNAP  4
#define RESHAPE_VEN_60FPS_SNAP    0
#define RESHAPE_VFALL_60FPS_SNAP  2
#define RESHAPE_VRISE_60FPS_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* i2c interface. */
#define I2C_DEV_ADDR  0x80
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  120
#define AE_30FPS_30_MIN_IDX  10
#define AE_30FPS_25_MAX_IDX  120
#define AE_30FPS_25_MIN_IDX  10
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_30_MAX_IDX  120
#define AE_60FPS_30_MIN_IDX  25
#define AE_60FPS_25_MAX_IDX  120
#define AE_60FPS_25_MIN_IDX  25
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_30FPS_MAX_IDX  74
#define GAIN_30FPS_MIN_IDX  14

#define GAIN_60FPS_MAX_IDX  74
#define GAIN_60FPS_MIN_IDX  14

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
static void frontAllRegDump(UINT8 dataLen);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - PA PAS6371LT RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    333,     79,      0 },
	{     51,    333,     79,      0 },
	{     52,    333,     79,      0 },
	{     53,    333,     79,      0 },
	{     54,    333,     79,      0 },
	{     55,    333,     79,      0 },
	{     56,    333,     79,      0 },
	{     57,    333,     79,      0 },
	{     58,    333,     78,      0 },
	{     59,    333,     76,      0 },
	{     60,    333,     74,      0 },
	{     61,    333,     72,      0 },
	{     62,    333,     70,      0 },
	{     63,    333,     68,      0 },
	{     64,    333,     67,      0 },
	{     65,    333,     65,      0 },
	{     66,    333,     64,      0 },
	{     67,    333,     63,      0 },
	{     68,    333,     60,      0 },
	{     69,    333,     59,      0 },
	{     70,    333,     57,      0 },
	{     71,    333,     55,      0 },
	{     72,    333,     53,      0 },
	{     73,    333,     52,      0 },
	{     74,    333,     50,      0 },
	{     75,    333,     49,      0 },
	{     76,    333,     48,      0 },
	{     77,    333,     46,      0 },
	{     78,    333,     43,      0 },
	{     79,    333,     41,      0 },
	{     80,    333,     39,      0 },
	{     81,    333,     38,      0 },
	{     82,    333,     36,      0 },
	{     83,    333,     35,      0 },
	{     84,    333,     33,      0 },
	{     85,    333,     32,      0 },
	{     86,    333,     31,      0 },
	{     87,    333,     29,      0 },
	{     88,    333,     27,      0 },
	{     89,    333,     25,      0 },
	{     90,    333,     23,      0 },
	{     91,    333,     21,      0 },
	{     92,    333,     20,      0 },
	{     93,    333,     18,      0 },
	{     94,    333,     17,      0 },
	{     95,    333,     16,      0 },
	{     96,    333,     15,      0 },
	{     97,    333,     14,      0 },
	{     98,    500,     19,      0 },
	{     99,    500,     18,      0 },
	{    100,    500,     17,      0 },
	{    101,    500,     16,      0 },
	{    102,    500,     15,      0 },
	{    103,    500,     14,      0 },
	{    104,   1000,     26,      0 },
	{    105,   1000,     24,      0 },
	{    106,   1000,     23,      0 },
	{    107,   1000,     21,      0 },
	{    108,   1000,     20,      0 },
	{    109,   1000,     18,      0 },
	{    110,   1000,     17,      0 },
	{    111,   1000,     16,      0 },
	{    112,   1000,     15,      0 },
	{    113,   1000,     14,      0 },
	{    114,   1042,     15,      0 },
	{    115,   1117,     15,      0 },
	{    116,   1198,     15,      0 },
	{    117,   1285,     15,      0 },
	{    118,   1378,     15,      0 },
	{    119,   1477,     15,      0 },
	{    120,   1584,     15,      0 },
	{    121,   1698,     15,      0 },
	{    122,   1820,     15,      0 },
	{    123,   1951,     15,      0 },
	{    124,   2092,     15,      0 },
	{    125,   2243,     15,      0 },
	{    126,   2404,     15,      0 },
	{    127,   2577,     15,      0 },
	{    128,   2763,     15,      0 },
	{    129,   2962,     15,      0 },
	{    130,   3175,     15,      0 },
	{    131,   3403,     15,      0 },
	{    132,   3648,     15,      0 },
	{    133,   3910,     15,      0 },
	{    134,   4191,     15,      0 },
	{    135,   4492,     15,      0 },
	{    136,   4815,     15,      0 },
	{    137,   5161,     15,      0 },
	{    138,   5532,     15,      0 },
	{    139,   5930,     15,      0 },
	{    140,   6356,     15,      0 },
	{    141,   6813,     15,      0 },
	{    142,   7302,     15,      0 },
	{    143,   7827,     15,      0 },
	{    144,   8389,     15,      0 },
	{    145,   8992,     15,      0 },
	{    146,   9638,     15,      0 },
	{    147,  10330,     15,      0 },
	{    148,  11072,     15,      0 },
	{    149,  11867,     15,      0 },
	{    150,  12719,     15,      0 },
	{    151,  13632,     15,      0 },
	{    152,  14611,     15,      0 },
	{    153,  15660,     15,      0 },
	{    154,  16785,     15,      0 },
	{    155,  17990,     15,      0 },
	{    156,  19282,     15,      0 },
	{    157,  20667,     15,      0 },
	{    158,  22151,     15,      0 },
	{    159,  23741,     15,      0 },
	{    160,  25445,     15,      0 },
	{    161,  27272,     17,      0 },
	{    162,  29230,     16,      0 },
	{    163,  31329,     19,      0 },
	{    164,  33578,     18,      0 },
	{    165,  35989,     17,      0 },
	{    166,  38573,     21,      0 },
	{    167,  41342,     20,      0 },
	{    168,  44310,     18,      0 },
	{    169,  47491,     17,      0 },
	{    170,  50900,     22,      0 },
};

static code frontAe_t ae30fps50hzTbl[121] = {
	{     50,    333,     79,      0 },
	{     51,    333,     79,      0 },
	{     52,    333,     79,      0 },
	{     53,    333,     79,      0 },
	{     54,    333,     79,      0 },
	{     55,    333,     79,      0 },
	{     56,    333,     79,      0 },
	{     57,    333,     79,      0 },
	{     58,    333,     78,      0 },
	{     59,    333,     76,      0 },
	{     60,    333,     74,      0 },
	{     61,    333,     72,      0 },
	{     62,    333,     70,      0 },
	{     63,    333,     68,      0 },
	{     64,    333,     67,      0 },
	{     65,    333,     65,      0 },
	{     66,    333,     64,      0 },
	{     67,    333,     63,      0 },
	{     68,    333,     60,      0 },
	{     69,    333,     59,      0 },
	{     70,    333,     57,      0 },
	{     71,    333,     55,      0 },
	{     72,    333,     53,      0 },
	{     73,    333,     52,      0 },
	{     74,    333,     50,      0 },
	{     75,    333,     49,      0 },
	{     76,    333,     48,      0 },
	{     77,    333,     46,      0 },
	{     78,    333,     43,      0 },
	{     79,    333,     41,      0 },
	{     80,    333,     39,      0 },
	{     81,    333,     38,      0 },
	{     82,    333,     36,      0 },
	{     83,    333,     35,      0 },
	{     84,    333,     33,      0 },
	{     85,    333,     32,      0 },
	{     86,    333,     31,      0 },
	{     87,    333,     29,      0 },
	{     88,    333,     27,      0 },
	{     89,    333,     25,      0 },
	{     90,    333,     23,      0 },
	{     91,    333,     21,      0 },
	{     92,    333,     20,      0 },
	{     93,    333,     18,      0 },
	{     94,    333,     17,      0 },
	{     95,    333,     16,      0 },
	{     96,    333,     15,      0 },
	{     97,    333,     14,      0 },
	{     98,    500,     19,      0 },
	{     99,    500,     18,      0 },
	{    100,    500,     17,      0 },
	{    101,    500,     16,      0 },
	{    102,    500,     15,      0 },
	{    103,    500,     14,      0 },
	{    104,   1000,     26,      0 },
	{    105,   1000,     24,      0 },
	{    106,   1000,     23,      0 },
	{    107,   1000,     21,      0 },
	{    108,   1000,     20,      0 },
	{    109,   1000,     18,      0 },
	{    110,   1000,     17,      0 },
	{    111,   1000,     16,      0 },
	{    112,   1000,     15,      0 },
	{    113,   1000,     14,      0 },
	{    114,   1042,     15,      0 },
	{    115,   1117,     15,      0 },
	{    116,   1198,     15,      0 },
	{    117,   1285,     15,      0 },
	{    118,   1378,     15,      0 },
	{    119,   1477,     15,      0 },
	{    120,   1584,     15,      0 },
	{    121,   1698,     15,      0 },
	{    122,   1820,     15,      0 },
	{    123,   1951,     15,      0 },
	{    124,   2092,     15,      0 },
	{    125,   2243,     15,      0 },
	{    126,   2404,     15,      0 },
	{    127,   2577,     15,      0 },
	{    128,   2763,     15,      0 },
	{    129,   2962,     15,      0 },
	{    130,   3175,     15,      0 },
	{    131,   3403,     15,      0 },
	{    132,   3648,     15,      0 },
	{    133,   3910,     15,      0 },
	{    134,   4191,     15,      0 },
	{    135,   4492,     15,      0 },
	{    136,   4815,     15,      0 },
	{    137,   5161,     15,      0 },
	{    138,   5532,     15,      0 },
	{    139,   5930,     15,      0 },
	{    140,   6356,     15,      0 },
	{    141,   6813,     15,      0 },
	{    142,   7302,     15,      0 },
	{    143,   7827,     15,      0 },
	{    144,   8389,     15,      0 },
	{    145,   8992,     15,      0 },
	{    146,   9638,     15,      0 },
	{    147,  10330,     15,      0 },
	{    148,  11072,     15,      0 },
	{    149,  11867,     15,      0 },
	{    150,  12719,     15,      0 },
	{    151,  13632,     15,      0 },
	{    152,  14611,     15,      0 },
	{    153,  15660,     15,      0 },
	{    154,  16785,     15,      0 },
	{    155,  17990,     15,      0 },
	{    156,  19282,     15,      0 },
	{    157,  20667,     15,      0 },
	{    158,  22151,     15,      0 },
	{    159,  23741,     15,      0 },
	{    160,  25445,     15,      0 },
	{    161,  27272,     17,      0 },
	{    162,  29230,     16,      0 },
	{    163,  31329,     19,      0 },
	{    164,  33578,     18,      0 },
	{    165,  35989,     17,      0 },
	{    166,  38573,     21,      0 },
	{    167,  41342,     20,      0 },
	{    168,  44310,     18,      0 },
	{    169,  47491,     17,      0 },
	{    170,  50900,     22,      0 },
};

static code frontAe_t ae60fps60hzTbl[121] = {
	{     50,    333,     79,      0 },
	{     51,    333,     79,      0 },
	{     52,    333,     79,      0 },
	{     53,    333,     79,      0 },
	{     54,    333,     79,      0 },
	{     55,    333,     79,      0 },
	{     56,    333,     79,      0 },
	{     57,    333,     79,      0 },
	{     58,    333,     78,      0 },
	{     59,    333,     76,      0 },
	{     60,    333,     74,      0 },
	{     61,    333,     72,      0 },
	{     62,    333,     70,      0 },
	{     63,    333,     68,      0 },
	{     64,    333,     67,      0 },
	{     65,    333,     65,      0 },
	{     66,    333,     64,      0 },
	{     67,    333,     63,      0 },
	{     68,    333,     60,      0 },
	{     69,    333,     59,      0 },
	{     70,    333,     57,      0 },
	{     71,    333,     55,      0 },
	{     72,    333,     53,      0 },
	{     73,   1000,     79,      0 },
	{     74,   1000,     78,      0 },
	{     75,   1000,     76,      0 },
	{     76,   1000,     74,      0 },
	{     77,   1000,     72,      0 },
	{     78,   1000,     70,      0 },
	{     79,   1000,     68,      0 },
	{     80,   1000,     67,      0 },
	{     81,   1000,     65,      0 },
	{     82,   1000,     64,      0 },
	{     83,   1000,     62,      0 },
	{     84,   1000,     60,      0 },
	{     85,   1000,     59,      0 },
	{     86,   1000,     57,      0 },
	{     87,   1000,     55,      0 },
	{     88,   1000,     53,      0 },
	{     89,   1000,     52,      0 },
	{     90,   1000,     50,      0 },
	{     91,   1000,     49,      0 },
	{     92,   1000,     47,      0 },
	{     93,   1000,     45,      0 },
	{     94,   1000,     43,      0 },
	{     95,   1000,     41,      0 },
	{     96,   1000,     39,      0 },
	{     97,   1000,     38,      0 },
	{     98,   1000,     36,      0 },
	{     99,   1000,     35,      0 },
	{    100,   1000,     33,      0 },
	{    101,   1000,     32,      0 },
	{    102,   1000,     30,      0 },
	{    103,   1000,     28,      0 },
	{    104,   1000,     26,      0 },
	{    105,   1000,     24,      0 },
	{    106,   1000,     23,      0 },
	{    107,   1000,     21,      0 },
	{    108,   1000,     20,      0 },
	{    109,   1000,     18,      0 },
	{    110,   1000,     17,      0 },
	{    111,   1000,     16,      0 },
	{    112,   1000,     15,      0 },
	{    113,   1000,     14,      0 },
	{    114,   1042,     15,      0 },
	{    115,   1117,     15,      0 },
	{    116,   1198,     15,      0 },
	{    117,   1285,     15,      0 },
	{    118,   1378,     15,      0 },
	{    119,   1477,     15,      0 },
	{    120,   1584,     15,      0 },
	{    121,   1698,     15,      0 },
	{    122,   1820,     15,      0 },
	{    123,   1951,     15,      0 },
	{    124,   2092,     15,      0 },
	{    125,   2243,     15,      0 },
	{    126,   2404,     15,      0 },
	{    127,   2577,     15,      0 },
	{    128,   2763,     15,      0 },
	{    129,   2962,     15,      0 },
	{    130,   3175,     15,      0 },
	{    131,   3403,     15,      0 },
	{    132,   3648,     15,      0 },
	{    133,   3910,     15,      0 },
	{    134,   4191,     15,      0 },
	{    135,   4492,     15,      0 },
	{    136,   4815,     15,      0 },
	{    137,   5161,     15,      0 },
	{    138,   5532,     15,      0 },
	{    139,   5930,     15,      0 },
	{    140,   6356,     15,      0 },
	{    141,   6813,     15,      0 },
	{    142,   7302,     15,      0 },
	{    143,   7827,     15,      0 },
	{    144,   8389,     15,      0 },
	{    145,   8992,     15,      0 },
	{    146,   9638,     15,      0 },
	{    147,  10330,     15,      0 },
	{    148,  11072,     15,      0 },
	{    149,  11867,     15,      0 },
	{    150,  12719,     15,      0 },
	{    151,  13632,     15,      0 },
	{    152,  14611,     15,      0 },
	{    153,  15660,     15,      0 },
	{    154,  16785,     15,      0 },
	{    155,  17990,     15,      0 },
	{    156,  19282,     15,      0 },
	{    157,  20667,     15,      0 },
	{    158,  22151,     15,      0 },
	{    159,  23741,     15,      0 },
	{    160,  25445,     15,      0 },
	{    161,  27272,     15,      0 },
	{    162,  29230,     15,      0 },
	{    163,  31329,     15,      0 },
	{    164,  33578,     15,      0 },
	{    165,  35989,     15,      0 },
	{    166,  38573,     15,      0 },
	{    167,  41342,     15,      0 },
	{    168,  44310,     15,      0 },
	{    169,  47491,     15,      0 },
	{    170,  50900,     22,      0 },
};

static code frontAe_t ae60fps50hzTbl[121] = {
	{     50,    333,     79,      0 },
	{     51,    333,     79,      0 },
	{     52,    333,     79,      0 },
	{     53,    333,     79,      0 },
	{     54,    333,     79,      0 },
	{     55,    333,     79,      0 },
	{     56,    333,     79,      0 },
	{     57,    333,     79,      0 },
	{     58,    333,     78,      0 },
	{     59,    333,     76,      0 },
	{     60,    333,     74,      0 },
	{     61,    333,     72,      0 },
	{     62,    333,     70,      0 },
	{     63,    333,     68,      0 },
	{     64,    333,     67,      0 },
	{     65,    333,     65,      0 },
	{     66,    333,     64,      0 },
	{     67,    333,     63,      0 },
	{     68,    333,     60,      0 },
	{     69,    333,     59,      0 },
	{     70,    333,     57,      0 },
	{     71,    333,     55,      0 },
	{     72,    333,     53,      0 },
	{     73,   1000,     79,      0 },
	{     74,   1000,     78,      0 },
	{     75,   1000,     76,      0 },
	{     76,   1000,     74,      0 },
	{     77,   1000,     72,      0 },
	{     78,   1000,     70,      0 },
	{     79,   1000,     68,      0 },
	{     80,   1000,     67,      0 },
	{     81,   1000,     65,      0 },
	{     82,   1000,     64,      0 },
	{     83,   1000,     62,      0 },
	{     84,   1000,     60,      0 },
	{     85,   1000,     59,      0 },
	{     86,   1000,     57,      0 },
	{     87,   1000,     55,      0 },
	{     88,   1000,     53,      0 },
	{     89,   1000,     52,      0 },
	{     90,   1000,     50,      0 },
	{     91,   1000,     49,      0 },
	{     92,   1000,     47,      0 },
	{     93,   1000,     45,      0 },
	{     94,   1000,     43,      0 },
	{     95,   1000,     41,      0 },
	{     96,   1000,     39,      0 },
	{     97,   1000,     38,      0 },
	{     98,   1000,     36,      0 },
	{     99,   1000,     35,      0 },
	{    100,   1000,     33,      0 },
	{    101,   1000,     32,      0 },
	{    102,   1000,     30,      0 },
	{    103,   1000,     28,      0 },
	{    104,   1000,     26,      0 },
	{    105,   1000,     24,      0 },
	{    106,   1000,     23,      0 },
	{    107,   1000,     21,      0 },
	{    108,   1000,     20,      0 },
	{    109,   1000,     18,      0 },
	{    110,   1000,     17,      0 },
	{    111,   1000,     16,      0 },
	{    112,   1000,     15,      0 },
	{    113,   1000,     14,      0 },
	{    114,   1042,     15,      0 },
	{    115,   1117,     15,      0 },
	{    116,   1198,     15,      0 },
	{    117,   1285,     15,      0 },
	{    118,   1378,     15,      0 },
	{    119,   1477,     15,      0 },
	{    120,   1584,     15,      0 },
	{    121,   1698,     15,      0 },
	{    122,   1820,     15,      0 },
	{    123,   1951,     15,      0 },
	{    124,   2092,     15,      0 },
	{    125,   2243,     15,      0 },
	{    126,   2404,     15,      0 },
	{    127,   2577,     15,      0 },
	{    128,   2763,     15,      0 },
	{    129,   2962,     15,      0 },
	{    130,   3175,     15,      0 },
	{    131,   3403,     15,      0 },
	{    132,   3648,     15,      0 },
	{    133,   3910,     15,      0 },
	{    134,   4191,     15,      0 },
	{    135,   4492,     15,      0 },
	{    136,   4815,     15,      0 },
	{    137,   5161,     15,      0 },
	{    138,   5532,     15,      0 },
	{    139,   5930,     15,      0 },
	{    140,   6356,     15,      0 },
	{    141,   6813,     15,      0 },
	{    142,   7302,     15,      0 },
	{    143,   7827,     15,      0 },
	{    144,   8389,     15,      0 },
	{    145,   8992,     15,      0 },
	{    146,   9638,     15,      0 },
	{    147,  10330,     15,      0 },
	{    148,  11072,     15,      0 },
	{    149,  11867,     15,      0 },
	{    150,  12719,     15,      0 },
	{    151,  13632,     15,      0 },
	{    152,  14611,     15,      0 },
	{    153,  15660,     15,      0 },
	{    154,  16785,     15,      0 },
	{    155,  17990,     15,      0 },
	{    156,  19282,     15,      0 },
	{    157,  20667,     15,      0 },
	{    158,  22151,     15,      0 },
	{    159,  23741,     15,      0 },
	{    160,  25445,     15,      0 },
	{    161,  27272,     15,      0 },
	{    162,  29230,     15,      0 },
	{    163,  31329,     15,      0 },
	{    164,  33578,     15,      0 },
	{    165,  35989,     15,      0 },
	{    166,  38573,     15,      0 },
	{    167,  41342,     15,      0 },
	{    168,  44310,     15,      0 },
	{    169,  47491,     15,      0 },
	{    170,  50900,     22,      0 },
};

/* gain table. */
static code UINT16 gain30fpsTbl[80] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static code UINT16 gain60fpsTbl[80] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
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
};

static code frontSnapCapabDesc_t frontSnapCapabDesc[SNAP_MODE_TOT] = {
	{  /* 30 fps. */
		WIDTH_30FPS_SNAP,
		HEIGHT_30FPS_SNAP,
		XOFF_30FPS_SNAP,
		YOFF_30FPS_SNAP,
		LINE_TOT_30FPS_SNAP,
		FRAME_TOT_30FPS_SNAP,
		MCLK_DIV_30FPS_SNAP,
		MCLK_PHASE_30FPS_SNAP,
		PCLK_DIV_30FPS_SNAP,
		PCLK_PHASE_30FPS_SNAP,
		PCLK_FREQ_30FPS_SNAP,
		RESHAPE_HEN_30FPS_SNAP,
		RESHAPE_HFALL_30FPS_SNAP,
		RESHAPE_HRISE_30FPS_SNAP,
		RESHAPE_VEN_30FPS_SNAP,
		RESHAPE_VFALL_30FPS_SNAP,
		RESHAPE_VRISE_30FPS_SNAP,
	},
	{  /* 60 fps. */
		WIDTH_60FPS_SNAP,
		HEIGHT_60FPS_SNAP,
		XOFF_60FPS_SNAP,
		YOFF_60FPS_SNAP,
		LINE_TOT_60FPS_SNAP,
		FRAME_TOT_60FPS_SNAP,
		MCLK_DIV_60FPS_SNAP,
		MCLK_PHASE_60FPS_SNAP,
		PCLK_DIV_60FPS_SNAP,
		PCLK_PHASE_60FPS_SNAP,
		PCLK_FREQ_60FPS_SNAP,
		RESHAPE_HEN_60FPS_SNAP,
		RESHAPE_HFALL_60FPS_SNAP,
		RESHAPE_HRISE_60FPS_SNAP,
		RESHAPE_VEN_60FPS_SNAP,
		RESHAPE_VFALL_60FPS_SNAP,
		RESHAPE_VRISE_60FPS_SNAP,
	},
};

/* i2c command table. */
static code UINT8 frontInitBank0CmdTbl[] = {
	0xEF, 0x00,  // bank0
	0x03, 0x0B,
	0x04, 0x0B,
	0x05, 0x11,
	0x06, 0x00,
	0x07, 0x00,
	0x08, 0x00,
	0x09, 0x00,
	0x0A, 0x00,
	0x0B, 0x00,
	0x0C, 0x00,
	0x0D, 0x11,
	0x0E, 0x01,
	0x0F, 0x40,
	0x10, 0x02,
	0x11, 0xB0,
	0x12, 0x01,
	0x13, 0xFC,
	0x14, 0x91,
	0x15, 0x90,
	0x16, 0x01,
	0x17, 0x2C,
	0x18, 0x01,
	0x19, 0x40,
	0x1A, 0x9C,
	0x1B, 0x40,
	0x1C, 0x95,
	0x1D, 0x01,
	0x1E, 0x62,
	0x1F, 0x8E,
	0x20, 0x84,
	0x21, 0x66,
	0x22, 0x58,
	0x23, 0xA4,
	0x24, 0x87,
	0x25, 0x63,
	0x26, 0x01,
	0x27, 0x4C,
	0x28, 0xF4,
	0x29, 0x00,
	0x2A, 0xF2,
	0x2B, 0x66,
	0x2C, 0xE8,
	0x2D, 0x00,
	0x2E, 0xDC,
	0x2F, 0x64,
	0x30, 0x00,
	0x31, 0x00,
	0x32, 0x00,
	0x34, 0x00,
	0x36, 0x03,
	0x37, 0x3C,
	0x38, 0xF5,
	0x39, 0x10,
	0x3A, 0x60,
	0x3B, 0x0A,
	0x3C, 0x14,
	0x3D, 0x0A,
	0x3E, 0x14,
	0x3F, 0x0E,
	0x40, 0x00,
	0x41, 0x0B,
	0x42, 0x13,
	0x43, 0x02,
	0x44, 0x10,
	0x45, 0x00,
	0x46, 0x01,
	0x47, 0x14,
	0x48, 0x03,
	0x49, 0x1D,
	0x4A, 0x1D,
	0x4B, 0x10,
	0x4C, 0x06,
	0x4D, 0x07,
	0x4E, 0x80,
	0x4F, 0x80,
	0x50, 0xA7,
	0x51, 0xC2,
	0x52, 0x1C,
	0x53, 0xD4,
	0x54, 0x0B,
	0x55, 0x38,
	0x56, 0x2F,
	0x57, 0x28,
	0x58, 0x01,
	0x59, 0x08,
	0x5A, 0x14,
	0x5B, 0x29,
	0x5C, 0x3C,
	0x5D, 0x4E,
	0x5E, 0x70,
	0x5F, 0x8F,
	0x60, 0xAD,
	0x61, 0xC9,
	0x62, 0x01,
	0x63, 0x30,
	0x64, 0x03,
	0x65, 0x81,
	0x66, 0x21,
	0x67, 0xC6,
	0x68, 0x2D,
	0x69, 0x04,
	0x6A, 0x10,
	0x6B, 0x06,
	0x6C, 0x12,
	0x6D, 0x2A,
	0x6E, 0xD2,
	0x6F, 0x70,
	0x70, 0x00,
	0x71, 0x01,
	0x72, 0x00,
	0x73, 0x50,
	0x74, 0xFF,
	0x75, 0x56,
	0x76, 0x33,
	0x77, 0x08,
	0x78, 0x00,
	0x79, 0x08,
	0x7A, 0x00,
	0x7B, 0x06,
	0x7C, 0x00,
	0x7D, 0x07,
	0x7E, 0xA0,
	0x7F, 0x2F,
	0x80, 0xC3,
	0x81, 0x03,
	0x82, 0x1C,
	0x83, 0xF0,
	0x84, 0x20,
	0x85, 0x00,
	0x86, 0x10,
	0x89, 0x00,
	0x8B, 0x00,
	0x8D, 0x00,
	0x8E, 0x01,
	0x8F, 0x40,
	0x90, 0x40,
	0x91, 0x10,
	0x92, 0x80,
	0x93, 0x00,
	0x94, 0x00,
	0x95, 0x02,
	0x96, 0x00,
	0x97, 0x30,
	0x98, 0x01,
	0x99, 0x23,
	0x9A, 0x28,
	0x9B, 0x22,
	0x9C, 0x2E,
	0x9D, 0x26,
	0x9E, 0x2B,
	0x9F, 0x28,
	0xA0, 0x32,
	0xA1, 0x26,
	0xA2, 0x2B,
	0xA3, 0x28,
	0xA4, 0x32,
	0xA5, 0x35,
	0xA6, 0x3E,
	0xA7, 0x38,
	0xA8, 0x43,
	0xA9, 0x4E,
	0xAA, 0x01,
	0xAB, 0xFC,
	0xAC, 0x00,
	0xB9, 0x02,
	0xD7, 0x80,
	0xD8, 0x02,
	0xD9, 0xE0,
	0xDA, 0x01,
	0xDB, 0x00,
	0xDC, 0x00,
	0xDD, 0xFF,
	0xDE, 0x00,
	0xDF, 0xAA,
	0xE0, 0xAA,
	0xE1, 0x00,
	0xE2, 0x02,
	0xE3, 0x11,
	0xE4, 0x11,
	0xE5, 0x00,
	0xE6, 0x80,
	0xED, 0x00,
	0xED, 0x01,
};

static code UINT8 frontInit30fpsBank1CmdTbl[] = {
	0xEF, 0x01,  // bank1
	0x00, 0x09,
	0x01, 0x30,
	0x02, 0x02,
	0x03, 0x89,
	0x04, 0xFC,
	0x05, 0x01,
	0x06, 0x0F,
	0x07, 0x0F,
	0x08, 0x0F,
	0x09, 0x0F,
	0x0C, 0x03,
	0x0D, 0xC8,
	0x0E, 0x01,
	0x0F, 0x00,
	0x10, 0x1F,
	0x12, 0x04,
	0x13, 0x10,
	0x14, 0x15,
	0x15, 0x10,
	0x16, 0x15,
	0x17, 0x08,
	0x18, 0x00,
	0x19, 0x08,
	0x1A, 0x00,
	0x1B, 0x87,
	0x1C, 0x02,
	0x1D, 0xE7,
	0x1E, 0x01,
	0x21, 0x03,
	0x22, 0x00,
	0x23, 0x00,
	0x24, 0x01,
	0x27, 0x03,
	0x2B, 0x48,
	0x2C, 0x86,
	0x2D, 0x02,
	0x2E, 0x46,
	0x2F, 0x48,
	0x30, 0x62,
	0x31, 0x46,
	0x32, 0x5A,
	0x33, 0x1E,
	0x34, 0x88,
	0x35, 0x00,
	0x36, 0x00,
	0x37, 0x00,
	0x38, 0x00,
	0x39, 0x64,
	0x3B, 0x00,
	0x3C, 0x00,
	0x3D, 0x00,
	0x3E, 0x00,
	0x40, 0x02,
	0x41, 0x57,
	0x42, 0x0C,
	0x43, 0xF2,
	0x44, 0x1F,
	0x45, 0x04,
	0x46, 0x04,
	0x48, 0x00,
	0x49, 0x04,
	0x4A, 0x00,
	0x4B, 0x80,
	0x4C, 0x40,
	0x4D, 0x00,
	0x50, 0x00,
	0x51, 0x40,
	0x58, 0x00,
	0x59, 0xC0,
	0x5A, 0xFF,
	0x5B, 0x03,
	0x5C, 0x19,
	0x60, 0x01,
	0x6D, 0xC8,
	0x6E, 0xC8,
	0x6F, 0xC8,
	0x70, 0xC8,
	0x71, 0x00,
	0x73, 0x00,
	0x76, 0x00,
	0x77, 0x10,
	0x78, 0x00,
	0x79, 0x00,
	0x7A, 0x01,
	0x7B, 0x00,
	0x7C, 0x13,
	0x7E, 0xFF,
	0x7F, 0x00,
	0x80, 0x00,
	0x82, 0x00,
	0x84, 0x00,
	0x85, 0x00,
	0x86, 0x00,
	0x87, 0x00,
	0x88, 0x00,
	0x89, 0x00,
	0x8A, 0x00,
	0x8B, 0x00,
	0x8C, 0x00,
	0x8D, 0x00,
	0x8E, 0x00,
	0x8F, 0x00,
	0x90, 0x00,
	0x91, 0x00,
	0x96, 0x00,
	0x97, 0x00,
	0x98, 0x80,
	0x99, 0x37,
	0x9A, 0x58,
	0x9B, 0x04,
	0x9C, 0x4E,
	0x9D, 0x18,
	0x9E, 0x00,
	0x9F, 0x10,
	0xA0, 0x02,
	0xA1, 0x01,
	0xA2, 0x05,
	0xA3, 0x03,
	0xA4, 0x03,
	0xA5, 0x00,
	0x11, 0x01,	 // update
};

static code UINT8 frontInit60fpsBank1CmdTbl[] = {
	0xEF, 0x01,  // bank1
	0x00, 0x09,
	0x01, 0x30,
	0x02, 0x02,
	0x03, 0x89,
	0x04, 0xB1,
	0x05, 0x01,
	0x06, 0x0F,
	0x07, 0x0F,
	0x08, 0x0F,
	0x09, 0x0F,
	0x0C, 0x03,
	0x0D, 0xC8,
	0x0E, 0x00,
	0x0F, 0x00,
	0x10, 0x1F,
	0x12, 0x04,
	0x13, 0x10,
	0x14, 0x15,
	0x15, 0x10,
	0x16, 0x15,
	0x17, 0x04,
	0x18, 0x00,
	0x19, 0x04,
	0x1A, 0x00,
	0x1B, 0x87,
	0x1C, 0x02,
	0x1D, 0xEF,
	0x1E, 0x01,
	0x21, 0x13,
	0x22, 0x00,
	0x23, 0x00,
	0x24, 0x01,
	0x27, 0x03,
	0x2B, 0x48,
	0x2C, 0x86,
	0x2D, 0x02,
	0x2E, 0x46,
	0x2F, 0x48,
	0x30, 0x62,
	0x31, 0x46,
	0x32, 0x5A,
	0x33, 0x1E,
	0x34, 0x88,
	0x35, 0x00,
	0x36, 0x00,
	0x37, 0x00,
	0x38, 0x00,
	0x39, 0x64,
	0x3B, 0x00,
	0x3C, 0x00,
	0x3D, 0x00,
	0x3E, 0x00,
	0x40, 0x02,
	0x41, 0x57,
	0x42, 0x0C,
	0x43, 0xF2,
	0x44, 0x1F,
	0x45, 0x04,
	0x46, 0x04,
	0x48, 0x00,
	0x49, 0x04,
	0x4A, 0x00,
	0x4B, 0x80,
	0x4C, 0x40,
	0x4D, 0x00,
	0x50, 0x00,
	0x51, 0x40,
	0x58, 0x00,
	0x59, 0xC0,
	0x5A, 0xFF,
	0x5B, 0x03,
	0x5C, 0x19,
	0x60, 0x01,
	0x6D, 0xC8,
	0x6E, 0xC8,
	0x6F, 0xC8,
	0x70, 0xC8,
	0x71, 0x00,
	0x73, 0x00,
	0x76, 0x00,
	0x77, 0x10,
	0x78, 0x00,
	0x79, 0x00,
	0x7A, 0x01,
	0x7B, 0x00,
	0x7C, 0x13,
	0x7E, 0xFF,
	0x7F, 0x00,
	0x80, 0x00,
	0x82, 0x00,
	0x84, 0x00,
	0x85, 0x00,
	0x86, 0x00,
	0x87, 0x00,
	0x88, 0x00,
	0x89, 0x00,
	0x8A, 0x00,
	0x8B, 0x00,
	0x8C, 0x00,
	0x8D, 0x00,
	0x8E, 0x00,
	0x8F, 0x00,
	0x90, 0x00,
	0x91, 0x00,
	0x96, 0x00,
	0x97, 0x00,
	0x98, 0x80,
	0x99, 0x37,
	0x9A, 0x58,
	0x9B, 0x04,
	0x9C, 0x4E,
	0x9D, 0x18,
	0x9E, 0x00,
	0x9F, 0x10,
	0xA0, 0x02,
	0xA1, 0x01,
	0xA2, 0x05,
	0xA3, 0x03,
	0xA4, 0x03,
	0xA5, 0x00,
	0x11, 0x01,	 // update
};

static code UINT8 frontInitBank2CmdTbl[] = {
	0xEF, 0x02,  // bank2
	0x00, 0x00,
	0x01, 0x00,
	0x02, 0x00,
	0x03, 0x00,
	0x04, 0x00,
	0x05, 0x00,
	0x06, 0x00,
	0x07, 0x00,
	0x08, 0x00,
	0x09, 0x00,
	0x0A, 0x00,
	0x0E, 0x00,
	0x0F, 0x00,
	0x10, 0x00,
	0x13, 0x00,
	0x17, 0x01,
	0x18, 0x24,
	0x19, 0x3C,
	0x1A, 0x51,
	0x1B, 0x63,
	0x1C, 0x74,
	0x1D, 0x84,
	0x1E, 0xA1,
	0x1F, 0xBC,
	0x20, 0xD6,
	0x21, 0xED,
	0x22, 0x0F,
	0x23, 0x85,
	0x24, 0xF0,
	0x25, 0x96,
	0x26, 0x20,
	0x27, 0x14,
	0x28, 0x0A,
	0x29, 0x0F,
	0x2A, 0xF8,
	0x2B, 0x00,
	0x2C, 0x30,
	0x2D, 0x18,
	0x2E, 0x22,
	0x2F, 0x12,
	0x30, 0x11,
	0x33, 0x01,
	0x34, 0xF0,
	0x55, 0x00,
	0x56, 0x01,
	0x57, 0x01,
	0x58, 0x04,
	0x59, 0x01,
	0x5A, 0x03,
	0x5B, 0x2F,
	0x5C, 0x06,
	0x5D, 0x01,
	0x5E, 0x0F,
	0x5F, 0x40,
	0x60, 0x50,
	0x61, 0x00,
	0x62, 0x03,
	0x63, 0x0F,
	0x64, 0x00,
	0x65, 0x03,
	0x66, 0x0F,
	0x67, 0x00,
	0x68, 0x03,
	0x69, 0x0F,
	0x9B, 0x02,
	0x9C, 0x80,
	0x9D, 0x01,
	0x9E, 0xE0,
	0x9F, 0x00,
	0xA0, 0x00,
	0xA1, 0x00,
	0xA2, 0x00,
	0xA3, 0x00,
	0xA4, 0x02,
	0xA5, 0x00,
	0xA6, 0xF0,
	0xA8, 0x01,
	0xA9, 0x20,
	0xB0, 0x00,
	0xB1, 0x00,
	0xB2, 0x00,
	0xB3, 0x00,
	0xB4, 0x02,
	0xB5, 0x80,
	0xB6, 0x01,
	0xB7, 0xE0,
	0xB8, 0x00,
	0xB9, 0x00,
	0xBA, 0x00,
	0xBB, 0x00,
	0xC0, 0x80,
	0xC1, 0x03,
	0xE4, 0x00,
	0xEB, 0x00,
	0xEC, 0x00,
	0xED, 0x00,
	0xEE, 0x00,
	0x00, 0x01,
	0xEF, 0x01,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0xEF, 0x01,  // bank1
	0x04, 0xFC,
	0x05, 0x01,
	0x0E, 0x01,
	0x0F, 0x00,
	0x06, 0x0F,  // gb gain
	0x07, 0x0F,  // b gain
	0x08, 0x0F,  // gr gain
	0x09, 0x0F,  // r gain
	0x10, 0x0F,  // gain
	0x17, 0x08,
	0x19, 0x08,
	0x1D, 0xE7,
	0x21, 0x03,
	0x42, 0x0C,  // 0x0fx2gain
	0x58, 0x00,  // 0x18x2gain
	0x11, 0x01,	 // update
};

static code UINT8 front60fpsPrevCmdTbl[] = {
	0xEF, 0x01,  // bank1
	0x04, 0xB1,
	0x05, 0x01,
	0x0E, 0x00,
	0x0F, 0x00,
	0x06, 0x0F,  // gb gain
	0x07, 0x0F,  // b gain
	0x08, 0x0F,  // gr gain
	0x09, 0x0F,  // r gain
	0x10, 0x0F,  // gain
	0x17, 0x04,
	0x19, 0x04,
	0x1D, 0xEF,
	0x21, 0x13,
	0x42, 0x0C,  // 0x0fx2gain
	0x58, 0x00,  // 0x18x2gain
	0x11, 0x01,	 // update
};

#if 0
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
extern UINT8 HAL_CdspEofWait(UINT8 Number);
extern void cdspColorMatrixRestore(void);

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-10-16
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
 * @since	2008-10-16
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
 * @since	2008-10-16
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
		frontPrevMode = 0;
		frontSnapMode = 0;
		break;

	case 60:
		frontPrevMode = 1;
		frontSnapMode = 1;
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
 * @since	2008-10-16
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
 * @since	2008-10-16
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
	i2cCmdTableSend(frontInitBank0CmdTbl, 1, sizeof(frontInitBank0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		i2cCmdTableSend(frontInit30fpsBank1CmdTbl, 1, sizeof(frontInit30fpsBank1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
	else {  /* 60 fps. */
		i2cCmdTableSend(frontInit60fpsBank1CmdTbl, 1, sizeof(frontInit60fpsBank1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
	i2cCmdTableSend(frontInitBank2CmdTbl, 1, sizeof(frontInitBank2CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	/* after power on. */
	frontAfterPowerOn();
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-16
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
 * @since	2008-10-16
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

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
		XBYTE[0x21D3] = 0x04;
		XBYTE[0x21D5] = 0x80;
		XBYTE[0x21D8] = 0x73;
	}
	else {  /* 60 fps. */
		i2cCmdTableSend(front60fpsPrevCmdTbl, 1, sizeof(front60fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}

	cdspBayerPatternSet(BAYER_PTN_PREV);

	//frontAllRegDump(1);
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-16
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

	if ( frontSnapMode == 0 ) {  /* 30 fps. */
		// for still edge (cdsp_task will update 0x21d0, 0x21d3, 0x21d9~0x21de to cdsppara.bin's data)
		XBYTE[0x21D5] = 0x18;
		XBYTE[0x21D8] = 0x63;
	}

	cdspBayerPatternSet(BAYER_PTN_SNAP);
}

/**
 * @brief	operating function to snap shot.
 * @param	first = [in] 0: not first, 1: first shot for burst shot.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-16
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
 * @since	2008-10-16
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
	UINT8 cmdTbl[2*4];
	UINT16 gain;


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	gain = pcap->gainTbl[val];

	cmdTbl[0] = 0x10;
	cmdTbl[1] = gain & 0x1F;
	cmdTbl[2] = 0x42;
	cmdTbl[3] = ((gain & 0x60) >> 5) | 0x0C;
	cmdTbl[4] = 0x58;
	cmdTbl[5] = ((gain & 0x80) >> 3) | 0x00;
	cmdTbl[6] = 0x11;
	cmdTbl[7] = 0x01;
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
 * @since	2008-10-16
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
	SINT16 espline_ny;
	UINT32 pixclk;
	UINT8 cmdTbl[2*3];
	UINT32 shut;


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	shut = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * shut);

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		espline_ny = 509 - espline;
	}
	else {  /* 60 fps. */
		espline_ny = 434 - espline;
	};

	if ( espline_ny <= 0 ) {
		espline_ny = 0;
	}

	cmdTbl[0] = 0x0e;
	cmdTbl[1] = espline_ny & 0xFF;
	cmdTbl[2] = 0x0f;
	cmdTbl[3] = espline_ny >> 8;
	cmdTbl[4] = 0x11;
	cmdTbl[5] = 0x01;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, parg->isr, parg->upd);
}

/**
 * @brief	operating function to get optical black status.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-10-16
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
 * @since	2008-10-16
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
 * @since	2008-10-16
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
 * @since	2008-10-16
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
 * @since	2008-10-16
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
 * @since	2008-09-17
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

