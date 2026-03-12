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
 * @file		front_ov_ov5620_raw.c
 * @brief		front sensor omni vision ov5620 driver.
 * @author		Matt Wang
 * @since		2008-09-08
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
#include "front_logi.h"
#include "gpio_api.h"
#include "i2c.h"
#include "gpio.h"
#include "hal_front_common.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       136
#define YOFF_30FPS_PREV       16
#define LINE_TOT_30FPS_PREV   805
#define FRAME_TOT_30FPS_PREV  498

#define WIDTH_60FPS_PREV      320
#define HEIGHT_60FPS_PREV     240
#define XOFF_60FPS_PREV       69
#define YOFF_60FPS_PREV       16
#define LINE_TOT_60FPS_PREV   390
#define FRAME_TOT_60FPS_PREV  257

#define WIDTH_SNAP      2592
#define HEIGHT_SNAP     1944
#define XOFF_SNAP       273
#define YOFF_SNAP       16
#define LINE_TOT_SNAP   3252
#define FRAME_TOT_SNAP  1992

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

#define MCLK_DIV_SNAP    4
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  12000000L
#define PCLK_FREQ_60FPS_PREV  6000000L
#define PCLK_FREQ_SNAP        24000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / MCLK_DIV_30FPS_PREV / PCLK_DIV_30FPS_PREV
#define PCLK_FREQ_60FPS_PREV  96000000L / MCLK_DIV_60FPS_PREV / PCLK_DIV_60FPS_PREV
#define PCLK_FREQ_SNAP        96000000L / MCLK_DIV_SNAP / PCLK_DIV_SNAP
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  3
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3

#define RESHAPE_HEN_60FPS_PREV    1
#define RESHAPE_HFALL_60FPS_PREV  52
#define RESHAPE_HRISE_60FPS_PREV  54
#define RESHAPE_VEN_60FPS_PREV    1
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  103
#define AE_30FPS_30_MIN_IDX  1
#define AE_30FPS_25_MAX_IDX  103
#define AE_30FPS_25_MIN_IDX  1
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_30_MAX_IDX  106
#define AE_60FPS_30_MIN_IDX  3
#define AE_60FPS_25_MAX_IDX  101
#define AE_60FPS_25_MIN_IDX  3
#define EV10_60FPS_EXP_IDX   100

/* gain. */
#define GAIN_30FPS_MAX_IDX  39
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  41
#define GAIN_60FPS_MIN_IDX  0

#define	EXP_GAIN_SET_POS  0x40
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

#define ST1155A_ENA1(x)	{sp1kSHT_ASet(x); sp1kSHT_ACfg();}
#define ST1155A_IN1(x)	{sp1kSHT_BSet(x); sp1kSHT_BCfg();}

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

static void SHTClose(void);
static void SHTOpen(void);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5620 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[104] = {
	{      0,    300,     38,      0 },
	{      1,    300,     36,      0 },
	{      2,    300,     34,      0 },
	{      3,    301,     33,      0 },
	{      4,    300,     31,      0 },
	{      5,    401,     37,      0 },
	{      6,    300,     28,      0 },
	{      7,    400,     33,      0 },
	{      8,    399,     31,      0 },
	{      9,    401,     30,      0 },
	{     10,    399,     28,      0 },
	{     11,    401,     27,      0 },
	{     12,    300,     18,      0 },
	{     13,    301,     17,      0 },
	{     14,    300,     15,      0 },
	{     15,    401,     21,      0 },
	{     16,    300,     12,      0 },
	{     17,    400,     17,      0 },
	{     18,    399,     15,      0 },
	{     19,    401,     14,      0 },
	{     20,   1223,     39,      0 },
	{     21,    401,     11,      0 },
	{     22,   1223,     35,      0 },
	{     23,   1213,     33,      0 },
	{     24,    599,     15,      0 },
	{     25,   1223,     30,      0 },
	{     26,   1193,     28,      0 },
	{     27,    400,      1,      0 },
	{     28,   1223,     26,      0 },
	{     29,   1193,     24,      0 },
	{     30,   1223,     23,      0 },
	{     31,   1223,     21,      0 },
	{     32,   1223,     19,      0 },
	{     33,   1213,     17,      0 },
	{     34,   1193,     15,      0 },
	{     35,   1223,     14,      0 },
	{     36,   1193,     12,      0 },
	{     37,   1213,     11,      0 },
	{     38,   1223,     10,      0 },
	{     39,   1193,      8,      0 },
	{     40,   1223,      7,      0 },
	{     41,   1223,      5,      0 },
	{     42,   1223,      3,      0 },
	{     43,   1213,      1,      0 },
	{     44,   2814,     19,      0 },
	{     45,   1394,      1,      0 },
	{     46,   1434,      0,      0 },
	{     47,   1538,      0,      0 },
	{     48,   4262,     23,      0 },
	{     49,   1912,      2,      0 },
	{     50,   2445,      7,      0 },
	{     51,   2445,      5,      0 },
	{     52,   2445,      3,      0 },
	{     53,   2331,      0,      0 },
	{     54,   2814,      3,      0 },
	{     55,   6485,     21,      0 },
	{     56,   2868,      0,      0 },
	{     57,   3469,      3,      0 },
	{     58,   4262,      7,      0 },
	{     59,   3825,      2,      0 },
	{     60,   4262,      3,      0 },
	{     61,   4520,      2,      0 },
	{     62,   4520,      1,      0 },
	{     63,   4661,      0,      0 },
	{     64,   5737,      4,      0 },
	{     65,   6485,      5,      0 },
	{     66,   5737,      0,      0 },
	{     67,   6485,      1,      0 },
	{     68,   8774,      9,      0 },
	{     69,   8774,      6,      0 },
	{     70,   8774,      4,      0 },
	{     71,   8774,      2,      0 },
	{     72,   9944,      4,      0 },
	{     73,   9944,      2,      0 },
	{     74,   9944,      0,      0 },
	{     75,  10654,      0,      0 },
	{     76,  11474,      2,      0 },
	{     77,  11474,      0,      0 },
	{     78,  16391,      6,      0 },
	{     79,  16391,      4,      0 },
	{     80,  16391,      2,      0 },
	{     81,  18645,      4,      0 },
	{     82,  18645,      2,      0 },
	{     83,  18645,      0,      0 },
	{     84,  24860,      6,      0 },
	{     85,  21308,      0,      0 },
	{     86,  24860,      2,      0 },
	{     87,  29832,      5,      0 },
	{     88,  29832,      3,      0 },
	{     89,  29832,      1,      0 },
	{     90,  36380,      6,      0 },
	{     91,  36380,      4,      0 },
	{     92,  36380,      2,      0 },
	{     93,  36380,      0,      0 },
	{     94,  49719,      7,      0 },
	{     95,  49719,      5,      0 },
	{     96,  49719,      3,      0 },
	{     97,  49719,      1,      0 },
	{     98,  74577,     15,      0 },
	{     99,  74577,     13,      0 },
	{    100,  74577,     11,      0 },
	{    101,  74577,      9,      0 },
	{    102,  74577,      7,      0 },
	{    103,  74577,      5,      0 },
};

static code frontAe_t ae30fps50hzTbl[104] = {
	{      0,    333,     39,      0 },
	{      1,    333,     39,      0 },
	{      2,    333,     38,      0 },
	{      3,    333,     36,      0 },
	{      4,    334,     35,      0 },
	{      5,    334,     33,      0 },
	{      6,    333,     32,      0 },
	{      7,    333,     30,      0 },
	{      8,    333,     29,      0 },
	{      9,    333,     28,      0 },
	{     10,    333,     27,      0 },
	{     11,    333,     25,      0 },
	{     12,    333,     23,      0 },
	{     13,    333,     21,      0 },
	{     14,    333,     20,      0 },
	{     15,    333,     19,      0 },
	{     16,    333,     17,      0 },
	{     17,    333,     16,      0 },
	{     18,    333,     14,      0 },
	{     19,    333,     12,      0 },
	{     20,    333,     11,      0 },
	{     21,    333,      9,      0 },
	{     22,    333,      8,      0 },
	{     23,    500,     14,      0 },
	{     24,    500,     12,      0 },
	{     25,    502,     10,      0 },
	{     26,    500,      9,      0 },
	{     27,    500,      7,      0 },
	{     28,    500,      6,      0 },
	{     29,    500,      4,      0 },
	{     30,    500,      2,      0 },
	{     31,    500,      0,      0 },
	{     32,    994,     14,      0 },
	{     33,   1015,     13,      0 },
	{     34,    994,     11,      0 },
	{     35,   1001,     10,      0 },
	{     36,   1015,      9,      0 },
	{     37,    994,      6,      0 },
	{     38,    994,      5,      0 },
	{     39,    994,      3,      0 },
	{     40,    994,      1,      0 },
	{     41,   1015,      0,      0 },
	{     42,   1223,      3,      0 },
	{     43,   1165,      0,      0 },
	{     44,   1394,      3,      0 },
	{     45,   1394,      1,      0 },
	{     46,   1434,      0,      0 },
	{     47,   1538,      0,      0 },
	{     48,   1912,      4,      0 },
	{     49,   1912,      2,      0 },
	{     50,   2445,      7,      0 },
	{     51,   2445,      5,      0 },
	{     52,   2445,      3,      0 },
	{     53,   2331,      0,      0 },
	{     54,   2814,      3,      0 },
	{     55,   2814,      1,      0 },
	{     56,   2868,      0,      0 },
	{     57,   3469,      3,      0 },
	{     58,   4262,      7,      0 },
	{     59,   3825,      2,      0 },
	{     60,   4262,      3,      0 },
	{     61,   4520,      2,      0 },
	{     62,   4520,      1,      0 },
	{     63,   4661,      0,      0 },
	{     64,   5737,      4,      0 },
	{     65,   6485,      5,      0 },
	{     66,   5737,      0,      0 },
	{     67,   6485,      1,      0 },
	{     68,   8774,      9,      0 },
	{     69,   8774,      6,      0 },
	{     70,   8774,      4,      0 },
	{     71,   8774,      2,      0 },
	{     72,   9944,      4,      0 },
	{     73,   9944,      2,      0 },
	{     74,   9944,      0,      0 },
	{     75,  10654,      0,      0 },
	{     76,  11474,      2,      0 },
	{     77,  11474,      0,      0 },
	{     78,  16391,      6,      0 },
	{     79,  16391,      4,      0 },
	{     80,  16391,      2,      0 },
	{     81,  18645,      4,      0 },
	{     82,  18645,      2,      0 },
	{     83,  18645,      0,      0 },
	{     84,  24860,      6,      0 },
	{     85,  21308,      0,      0 },
	{     86,  24860,      2,      0 },
	{     87,  29832,      5,      0 },
	{     88,  29832,      3,      0 },
	{     89,  29832,      1,      0 },
	{     90,  36380,      6,      0 },
	{     91,  36380,      4,      0 },
	{     92,  36380,      2,      0 },
	{     93,  36380,      0,      0 },
	{     94,  49719,      7,      0 },
	{     95,  49719,      5,      0 },
	{     96,  49719,      3,      0 },
	{     97,  49719,      1,      0 },
	{     98,  74577,     15,      0 },
	{     99,  74577,     13,      0 },
	{    100,  74577,     11,      0 },
	{    101,  74577,      9,      0 },
	{    102,  74577,      7,      0 },
	{    103,  74577,      5,      0 },
};

static code frontAe_t ae60fps60hzTbl[107] = {
	{      0,    601,     41,      0 },
	{      1,    601,     39,      0 },
	{      2,    599,     37,      0 },
	{      3,    601,     36,      0 },
	{      4,    599,     34,      0 },
	{      5,    601,     33,      0 },
	{      6,    601,     32,      0 },
	{      7,    599,     30,      0 },
	{      8,    601,     29,      0 },
	{      9,    601,     27,      0 },
	{     10,   1221,     41,      0 },
	{     11,   1211,     39,      0 },
	{     12,   1193,     37,      0 },
	{     13,   1221,     36,      0 },
	{     14,   1193,     34,      0 },
	{     15,   1211,     33,      0 },
	{     16,   1221,     32,      0 },
	{     17,   1193,     30,      0 },
	{     18,   1211,     29,      0 },
	{     19,   1221,     27,      0 },
	{     20,   1221,     25,      0 },
	{     21,   1211,     23,      0 },
	{     22,   1193,     21,      0 },
	{     23,   1221,     20,      0 },
	{     24,   1193,     18,      0 },
	{     25,   1211,     17,      0 },
	{     26,   1221,     16,      0 },
	{     27,   1193,     14,      0 },
	{     28,   1211,     13,      0 },
	{     29,   1221,     11,      0 },
	{     30,   1221,      9,      0 },
	{     31,   1211,      7,      0 },
	{     32,   1193,      5,      0 },
	{     33,   1221,      4,      0 },
	{     34,   1193,      2,      0 },
	{     35,   1211,      1,      0 },
	{     36,   1221,      0,      0 },
	{     37,   7326,     39,      0 },
	{     38,   1411,      0,      0 },
	{     39,   2522,     12,      0 },
	{     40,   2522,     10,      0 },
	{     41,   2608,      9,      0 },
	{     42,   2797,      9,      0 },
	{     43,   2107,      1,      0 },
	{     44,   9050,     33,      0 },
	{     45,   2296,      0,      0 },
	{     46,   3140,      5,      0 },
	{     47,   7326,     23,      0 },
	{     48,   4396,     10,      0 },
	{     49,  12820,     33,      0 },
	{     50,   3419,      1,      0 },
	{     51,   3663,      1,      0 },
	{     52,   4963,      6,      0 },
	{     53,   4662,      3,      0 },
	{     54,   9050,     17,      0 },
	{     55,  25641,     39,      0 },
	{     56,  21978,     34,      0 },
	{     57,   7326,      7,      0 },
	{     58,   9050,     11,      0 },
	{     59,  25641,     33,      0 },
	{     60,   9050,      7,      0 },
	{     61,   7326,      1,      0 },
	{     62,  12820,     13,      0 },
	{     63,  12820,     11,      0 },
	{     64,   9050,      1,      0 },
	{     65,  51280,     39,      0 },
	{     66,  30769,     26,      0 },
	{     67,  12820,      4,      0 },
	{     68,  11834,      1,      0 },
	{     69,  51280,     33,      0 },
	{     70,  12820,      0,      0 },
	{     71,  15384,      2,      0 },
	{     72,  25641,     13,      0 },
	{     73,  25641,     11,      0 },
	{     74,  17094,      0,      0 },
	{     75,  51280,     23,      0 },
	{     76,  30769,     10,      0 },
	{     77,  25641,      4,      0 },
	{     78,  38461,     13,      0 },
	{     79,  51280,     17,      0 },
	{     80,  25641,      0,      0 },
	{     81,  30769,      2,      0 },
	{     82,  51280,     13,      0 },
	{     83,  51280,     11,      0 },
	{     84,  51280,      9,      0 },
	{     85,  51280,      7,      0 },
	{     86,  51280,      5,      0 },
	{     87,  51280,      4,      0 },
	{     88,  76919,     13,      0 },
	{     89,  51280,      1,      0 },
	{     90,  51280,      0,      0 },
	{     91,  76919,      7,      0 },
	{     92, 153831,     21,      0 },
	{     93,  76919,      4,      0 },
	{     94, 153831,     18,      0 },
	{     95,  76919,      1,      0 },
	{     96,  76919,      0,      0 },
	{     97, 153831,     14,      0 },
	{     98, 153831,     13,      0 },
	{     99, 153831,     11,      0 },
	{    100, 153831,      9,      0 },
	{    101, 153831,      7,      0 },
	{    102, 153831,      5,      0 },
	{    103, 153831,      4,      0 },
	{    104, 153831,      2,      0 },
	{    105, 153831,      1,      0 },
	{    106, 153831,      0,      0 },
};

static code frontAe_t ae60fps50hzTbl[102] = {
	{      0,    993,     45,      0 },
	{      1,    993,     43,      0 },
	{      2,    993,     41,      0 },
	{      3,   1019,     40,      0 },
	{      4,   1012,     38,      0 },
	{      5,    993,     36,      0 },
	{      6,   1012,     35,      0 },
	{      7,    993,     33,      0 },
	{      8,    999,     32,      0 },
	{      9,   1012,     31,      0 },
	{     10,   1019,     30,      0 },
	{     11,    993,     27,      0 },
	{     12,    993,     25,      0 },
	{     13,   1019,     24,      0 },
	{     14,   1012,     22,      0 },
	{     15,    993,     20,      0 },
	{     16,   1012,     19,      0 },
	{     17,    993,     17,      0 },
	{     18,    999,     16,      0 },
	{     19,   1012,     15,      0 },
	{     20,   1019,     14,      0 },
	{     21,    993,     11,      0 },
	{     22,    993,      9,      0 },
	{     23,   1019,      8,      0 },
	{     24,   1012,      6,      0 },
	{     25,    993,      4,      0 },
	{     26,   1012,      3,      0 },
	{     27,    993,      1,      0 },
	{     28,    999,      0,      0 },
	{     29,   4525,     33,      0 },
	{     30,   2167,     15,      0 },
	{     31,   1570,      5,      0 },
	{     32,   7326,     39,      0 },
	{     33,   1411,      0,      0 },
	{     34,   2522,     12,      0 },
	{     35,   2522,     10,      0 },
	{     36,   2608,      9,      0 },
	{     37,   2797,      9,      0 },
	{     38,   2107,      1,      0 },
	{     39,   9050,     33,      0 },
	{     40,   2296,      0,      0 },
	{     41,   3140,      5,      0 },
	{     42,   7326,     23,      0 },
	{     43,   4396,     10,      0 },
	{     44,  12820,     33,      0 },
	{     45,   3419,      1,      0 },
	{     46,   3663,      1,      0 },
	{     47,   4963,      6,      0 },
	{     48,   4662,      3,      0 },
	{     49,   9050,     17,      0 },
	{     50,  30769,     44,      0 },
	{     51,  30769,     42,      0 },
	{     52,   7326,      7,      0 },
	{     53,   9050,     11,      0 },
	{     54,  25641,     33,      0 },
	{     55,   9050,      7,      0 },
	{     56,   7326,      1,      0 },
	{     57,  12820,     13,      0 },
	{     58,  12820,     11,      0 },
	{     59,   9050,      1,      0 },
	{     60,  51280,     39,      0 },
	{     61,  30769,     26,      0 },
	{     62,  12820,      4,      0 },
	{     63,  11834,      1,      0 },
	{     64,  51280,     33,      0 },
	{     65,  12820,      0,      0 },
	{     66,  15384,      2,      0 },
	{     67,  25641,     13,      0 },
	{     68,  25641,     11,      0 },
	{     69,  17094,      0,      0 },
	{     70,  51280,     23,      0 },
	{     71,  30769,     10,      0 },
	{     72,  25641,      4,      0 },
	{     73,  38461,     13,      0 },
	{     74,  51280,     17,      0 },
	{     75,  25641,      0,      0 },
	{     76,  30769,      2,      0 },
	{     77,  51280,     13,      0 },
	{     78,  51280,     11,      0 },
	{     79,  51280,      9,      0 },
	{     80,  51280,      7,      0 },
	{     81,  51280,      5,      0 },
	{     82,  51280,      4,      0 },
	{     83,  76919,     13,      0 },
	{     84,  51280,      1,      0 },
	{     85,  51280,      0,      0 },
	{     86,  76919,      7,      0 },
	{     87, 153831,     21,      0 },
	{     88,  76919,      4,      0 },
	{     89, 153831,     18,      0 },
	{     90,  76919,      1,      0 },
	{     91,  76919,      0,      0 },
	{     92, 153831,     14,      0 },
	{     93, 153831,     13,      0 },
	{     94, 153831,     11,      0 },
	{     95, 153831,      9,      0 },
	{     96, 153831,      7,      0 },
	{     97, 153831,      5,      0 },
	{     98, 153831,      4,      0 },
	{     99, 153831,      2,      0 },
	{    100, 153831,      1,      0 },
	{    101, 153831,      0,      0 },
};

/* gain table. */
static code UINT16 gain30fpsTbl[40] = {
	  8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
	 18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
	 28,  29,  30,  31,  48,  49,  50,  51,  52,  53,
	 54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
};

static code UINT16 gain60fpsTbl[42] = {
	  2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
	 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
	 22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,
	 58,  59,
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
	0x12, 0x10,
	0x11, 0x40,
 	0x13, 0x00,
	0x10, 0x1E,
	0x3B, 0x07,
	0x5B, 0x40,
	0x39, 0x07,
	0x53, 0x02,
	0x54, 0x60,
	0x27, 0x04,
	0x3d, 0x40,
	0x36, 0x00,
	0xC5, 0x04,
	0x4E, 0x00,
	0x4F, 0x93,
	0x50, 0x7B,
	0xCA, 0x0C,
	0xCB, 0x0F,
	0x39, 0x07,
	0x4a, 0x10,
	0x3e, 0x0A,
	0x3d, 0x00,
	0x89, 0x20,
	0x8b, 0x00,
	0x83, 0x80,
	0xb7, 0x9d,
	0xb6, 0x11,
	0xb5, 0x55,
	0xb4, 0x00,
	0xa9, 0xf0,
	0xa8, 0x0A,
	0xb8, 0xf0,
	0xb9, 0xf0,
	0xba, 0xf0,
	0x14, 0x60,
	0x33, 0x75,
	0x2c, 0x00,
	0x09, 0x00,
	0x35, 0x30,
	0x27, 0x04,
	0x3c, 0x07,
	0x3a, 0x0a,
	0x3b, 0x07,
	0x01, 0x40,
	0x02, 0x40,
	0x16, 0x40,
	0x52, 0xB0,
	0x51, 0x83,
	0x21, 0xBB,
	0x22, 0x10,
	0x23, 0x03,
	0x35, 0x38,
	0x20, 0x90,
	0x28, 0x30,
	0x73, 0xE1,
	0x6c, 0x00,
	0x6d, 0x80,
	0x6e, 0x00,
	0x70, 0x04,
	0x71, 0x00,
	0x8d, 0x04,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x67, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x71, 0x97,
	0x74, 0x20,
	0x8d, 0x44,
	0x80, 0x09,
	0x85, 0xC0,
	0xd2, 0x00,
	0xd3, 0x00,
	0xd4, 0x00,
	0xd5, 0x00,
	0xd6, 0x00,
	0xd7, 0x00,
	0x04, 0x20,
	0x81, 0x07,

	0x15, 0x40,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x12, 0x10,
	0x2A, 0x10,
	0x2B, 0x4C,
	0x0C, 0x0A,
	0x4E, 0x00,
	0x4F, 0x94,
	0x50, 0x7C,
	0xCA, 0x02,
	0xCB, 0x03,
	0x0d, 0x96,
	0x17, 0x12,
	0x19, 0x01,
	0x03, 0x42,
	0x32, 0x80,
	0x53, 0x0E,
	0x54, 0x61,
	0x46, 0x11,
	0x04, 0x30,
	0x8D, 0x64,
	0x38, 0x10,
	0x8e, 0x00,
	0xad, 0x24,
	0xfc, 0x24,
	0xfd, 0x24,
	0x63, 0x44,
	0xab, 0xff,
	0x44, 0xc0,
	0x37, 0x00,

	0x3F, 0x17,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x67, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6a, 0x00,
	0x6b, 0x00,
	0x15, 0x40,
};

static code UINT8 front60fpsPrevCmdTbl[] = {
	0x12, 0x08,
	0x44, 0x48,
	0x0C, 0x08,
	0x0D, 0x8E,
	0x17, 0x12,
	0x19, 0x01,
	0x03, 0x40,
	0x32, 0xC0,
	0x53, 0x0E,
	0x46, 0x11,
	0x37, 0x00,
	0x38, 0x10,
	0x44, 0xC0,
	0x04, 0x30,
	0x8D, 0x64,

	0x15, 0x42,
	0x11, 0x40,

	0x3E, 0x0A,
	0x3D, 0x00,
	0x8D, 0x44,
	0x10, 0x38,
	0x14, 0x66,
};

static code UINT8 frontSnapCmdTbl[] = {
	0x0d, 0x06,
	0x2a, 0x00,
	0x2b, 0x00,
	0x32, 0x00,
	0x12, 0x00,

	#if 0
	0x0C, 0x38,
	#else
	0x0C, 0x0D,
	#endif

	0x38, 0x90,
	0x46, 0x30,

	#if 0
	0x17, 0x12,
	0x18, 0xB4,
	0x19, 0x01,//0x01,
	0x1A, 0xF4,//0xF4,
	0x03, 0x4A,//0x4A,
	#else
	0x17, 0x12,
	0x18, 0xFF,
	0x19, 0x00,
	0x1A, 0xFF,
	0x03, 0x07,
	#endif

	#if 0
	#else
	0x5F, 0x02,
	0x60, 0xA2,
	0x61, 0xF5,
	#endif

	0x4F, 0x93,
	0x50, 0x7B,
	0xCA, 0x0C,
	0xCB, 0x0F,
	0x8D, 0x44,
	0x04, 0x20,
	0x53, 0x02,
	0x54, 0x60,
	0x44, 0x40,//0xc0,
	0x37, 0x02,

	#if 0 //DENOISE
	0x63, 0x40,
	0x8e, 0x80,
	0x8d, 0x44,
	0xab, 0x00,
	0xad, 0x12,
	0xfc, 0x0c,
	0xfd, 0xff,
	#endif
	0x15, 0x40,
	//0x15, 0x00,
	//0x15, 0x00,
	//0x15, 0x00,
	//0x15, 0x00,
	//0x15, 0x00,
	//0x15, 0x00,
	//0x15, 0x00,

};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;

static UINT32 frontEspLine=0;
static UINT8 frame_mode = 1;
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
 * @since	2008-09-08
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
 * @since	2008-09-08
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
		frontPrevMode = 0;
		//frontSnapMode = 0;
		break;

	case 60:
		frontPrevMode = 1;
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
 * @since	2008-09-08
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
 * @since	2008-09-08
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPowerOn(
	void
)
{
	/* before power on. */
	frontBeforePowerOn();

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	globalTimerWait(10);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	globalTimerWait(10);

	/* open mechanical shutter */
	SHTOpen();

	/* after power on. */
	frontAfterPowerOn();
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-08
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
 * @since	2008-09-08
 * @todo	N/A
 * @bug		N/A
*/
//void HalFrontI2CRegRead(UINT8);

__STATIC void
frontOpPreviewModeSet(
	void
)
{
	frontPrevCapabDesc_t *pcap;


	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	}
	else {  /* 60 fps. */
		i2cCmdTableSend(front60fpsPrevCmdTbl, 1, sizeof(front60fpsPrevCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	}

	cdspBayerPatternSet(BAYER_PTN_PREV);

	//HalFrontI2CRegRead(1);
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-08
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
 * @since	2008-09-17
 * @todo	N/A
 * @bug		N/A
*/

__STATIC void
frontOpSnapShot(
	UINT8 first
)
{
	UINT8 sts;
	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
		frontSnapExposureTimeSet();
	}

	ENTER_CRITICAL(sts);
	sp1kFrexCfg();
	sp1kExpstbCfg();
	sp1kExpstbCfg();
	//XBYTE[0x2030] &= 0xFD;
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 2); // 20081024 mantis 32544 wenguo
	cdsp1stPathWriteSkip(6, 1);
	//XBYTE[0x2030] |= 0x02;
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	//while(1);
	
	#if 1
		//printf("Jeff: a first=%bu, frontEspLine=%d\n",first, frontEspLine);
		HAL_Front10UsWait(20L);
		XBYTE[0x2031] |= 0xC0; //sp1kFrexSet(1);			  //FREX ON
		//sp1kFrexCfg();
		//sp1kExpstbSet(1);		  //EXPSTB ON
		//sp1kExpstbCfg();
		HAL_Front10UsWait(18L);
		sp1kExpstbSet(0);		  //EXPSTB OFF
		//sp1kExpstbCfg();
		HAL_Front10UsWait(frontEspLine);
		//HAL_Front10UsWait(50000L);
		SHTClose();
		globalTimerWait(10);
		sp1kFrexSet(0);			   //FREX OFF
		//sp1kFrexCfg();
	#endif

	frontSignalWait(HAL_FRONT_WAIT_VVALID, HAL_FRONT_WAIT_HIGH, 1);
	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);
	//XBYTE[0x2030] &= 0xFD;
	SHTOpen();
	EXIT_CRITICAL(sts);

	frontOpPreviewModeSet();
}

/**
 * @brief	operating function to set gain value.
 * @param	parg->val = [in] gain value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-16
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
 * @since	2008-09-16
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
	UINT8 cmdTbl[2*5];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 3) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0x07;
	cmdTbl[4] = 0x45;
	cmdTbl[5] = 0x00;
	cmdTbl[6] = 0x2D;
	cmdTbl[7] = 0;
	cmdTbl[8] = 0x2E;
	cmdTbl[9] = 0;
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, parg->isr, parg->upd);
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
 * @since	2008-09-17
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

	frontEspLine = 2000000L / frontLastShutter;

	if ( s_gain >= 0x30 ) {
		lastt_gain = s_gain - 48;
		espline = espline * 4;
		frontEspLine = frontEspLine * 4;
	}
	else if ( s_gain >= 0x10 ) {
		lastt_gain = s_gain - 16;
		espline = espline * 2;
		frontEspLine = frontEspLine * 2;
	}
	else {
		lastt_gain = s_gain;
		espline = espline;
		frontEspLine = frontEspLine;
	}

	cdspColorMatrixRestore();
	XBYTE[0x21D3] = 0x02;
	XBYTE[0x21D0] = 0x02;

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
		espline = ftotal - 1;
	}
	else {
		tmp0 = 0;
	};

	if(frame_mode == 1) espline = 4096L;

	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 3) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0x07;
	cmdTbl[4] = 0x45;
	cmdTbl[5] = (espline >> 11) & 0x3F;
	cmdTbl[6] = 0x2D;
	cmdTbl[7] = 0;
	cmdTbl[8] = 0x2E;
	cmdTbl[9] = 0;
	cmdTbl[10] = 0x00;
	cmdTbl[11] = lastt_gain;
	i2cCmdTableSend(cmdTbl, 1, 6/* sizeof(cmdTbl)/2*/, I2C_TRANS_MODE_BURST, 0, 0);
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
	gain = gain;
}

static void SHTClose(void)		//Jim for OB cal
{
	ST1155A_IN1(1);		//SHT Close
	ST1155A_ENA1(1);	//enable
	globalTimerWait(10);
	ST1155A_ENA1(0);	//disable
}
static void SHTOpen(void)		//Jim for OB cal
{
	ST1155A_IN1(0);		//SHT Close
	ST1155A_ENA1(1);	//enable
	globalTimerWait(10);
	ST1155A_ENA1(0);	//disable
}

