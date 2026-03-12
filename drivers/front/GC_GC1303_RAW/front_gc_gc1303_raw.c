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
 * @file		front_gc_gc1303_raw.c
 * @brief		front sensor galaxy core gc1303 driver.
 * @author		Matt Wang
 * @since		2008-10-20
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
#include "timer.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
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
#define XOFF_30FPS_PREV       15
#define YOFF_30FPS_PREV       21
#define LINE_TOT_30FPS_PREV   1739
#define FRAME_TOT_30FPS_PREV  608

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
#define XOFF_SNAP       5
#define YOFF_SNAP       5
#define LINE_TOT_SNAP   1500
#define FRAME_TOT_SNAP  1064

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
#define PCLK_FREQ_30FPS_PREV  24000000L
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
#define RESHAPE_HFALL_30FPS_PREV  3
#define RESHAPE_HRISE_30FPS_PREV  4
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

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* i2c interface. */
#define I2C_DEV_ADDR  0x50
#define I2C_CLK_DIV   3

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  111
#define AE_30FPS_30_MIN_IDX  23
#define AE_30FPS_25_MAX_IDX  111
#define AE_30FPS_25_MIN_IDX  23
#define EV10_30FPS_EXP_IDX   100

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  32
#define GAIN_30FPS_MIN_IDX  5

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0
#endif

#define EXP_GAIN_SET_POS  0x01
#define GAIN_AFTER_EXP    0x00

/* sensor control pin. */
#define PWR_PIN_BYTE  GPIO_BYTE_GEN1
#define PWR_PIN_BIT   (1 << (14 & 0x07))
#define RST_PIN_BYTE  GPIO_BYTE_GEN1
#define RST_PIN_BIT   (1 << (15 & 0x07))

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
static code UINT8 frontDevName[] = "SENSOR - GC GC1303 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[121] = {
	{     50,    300,    127,      0 },
	{     51,    300,    127,      0 },
	{     52,    300,    127,      0 },
	{     53,    300,    124,      0 },
	{     54,    300,    116,      0 },
	{     55,    300,    106,      0 },
	{     56,    300,     99,      0 },
	{     57,    300,     91,      0 },
	{     58,    300,     84,      0 },
	{     59,    300,     76,      0 },
	{     60,    300,     71,      0 },
	{     61,    300,     67,      0 },
	{     62,    300,     61,      0 },
	{     63,    300,     57,      0 },
	{     64,    300,     54,      0 },
	{     65,    300,     50,      0 },
	{     66,    300,     45,      0 },
	{     67,    300,     42,      0 },
	{     68,    300,     40,      0 },
	{     69,    300,     37,      0 },
	{     70,    300,     34,      0 },
	{     71,    300,     30,      0 },
	{     72,    300,     28,      0 },
	{     73,    300,     26,      0 },
	{     74,    300,     25,      0 },
	{     75,    300,     24,      0 },
	{     76,    300,     21,      0 },
	{     77,    300,     20,      0 },
	{     78,    300,     19,      0 },
	{     79,    300,     18,      0 },
	{     80,    300,     17,      0 },
	{     81,    300,     15,      0 },
	{     82,    400,     29,      0 },
	{     83,    400,     27,      0 },
	{     84,    400,     24,      0 },
	{     85,    400,     22,      0 },
	{     86,    600,     21,      0 },
	{     87,    600,     20,      0 },
	{     88,    600,     19,      0 },
	{     89,    600,     18,      0 },
	{     90,    600,     16,      0 },
	{     91,    600,     15,      0 },
	{     92,   1200,     29,      0 },
	{     93,   1200,     27,      0 },
	{     94,   1200,     24,      0 },
	{     95,   1200,     22,      0 },
	{     96,   1200,     21,      0 },
	{     97,   1200,     20,      0 },
	{     98,   1200,     18,      0 },
	{     99,   1200,     17,      0 },
	{    100,   1200,     16,      0 },
	{    101,   1200,     13,      0 },
	{    102,   1252,     15,      0 },
	{    103,   1342,     15,      0 },
	{    104,   1439,     15,      0 },
	{    105,   1543,     15,      0 },
	{    106,   1654,     15,      0 },
	{    107,   1773,     15,      0 },
	{    108,   1901,     15,      0 },
	{    109,   2038,     15,      0 },
	{    110,   2185,     15,      0 },
	{    111,   2342,     15,      0 },
	{    112,   2511,     15,      0 },
	{    113,   2692,     15,      0 },
	{    114,   2886,     15,      0 },
	{    115,   3094,     15,      0 },
	{    116,   3317,     15,      0 },
	{    117,   3556,     15,      0 },
	{    118,   3812,     15,      0 },
	{    119,   4086,     15,      0 },
	{    120,   4380,     15,      0 },
	{    121,   4695,     15,      0 },
	{    122,   5032,     15,      0 },
	{    123,   5394,     15,      0 },
	{    124,   5782,     15,      0 },
	{    125,   6198,     15,      0 },
	{    126,   6643,     16,      0 },
	{    127,   7120,     16,      0 },
	{    128,   7632,     16,      0 },
	{    129,   8180,     16,      0 },
	{    130,   8768,     16,      0 },
	{    131,   9398,     16,      0 },
	{    132,  10073,     16,      0 },
	{    133,  10796,     16,      0 },
	{    134,  11000,     15,      0 },
	{    135,  12402,     16,      0 },
	{    136,  13000,     16,      0 },
	{    137,  15000,     17,      0 },
	{    138,  15000,     16,      0 },
	{    139,  17000,     16,      0 },
	{    140,  17000,     15,      0 },
	{    141,  19000,     17,      0 },
	{    142,  19000,     16,      0 },
	{    143,  22000,     16,      0 },
	{    144,  22000,     15,      0 },
	{    145,  25000,     18,      0 },
	{    146,  25000,     17,      0 },
	{    147,  25000,     16,      0 },
	{    148,  30549,     17,      0 },
	{    149,  30549,     16,      0 },
	{    150,  30549,     15,      0 },
	{    151,  40000,     22,      0 },
	{    152,  40000,     20,      0 },
	{    153,  40000,     19,      0 },
	{    154,  40000,     18,      0 },
	{    155,  40000,     17,      0 },
	{    156,  40000,     16,      0 },
	{    157,  40000,     15,      0 },
	{    158,  65000,     17,      0 },
	{    159,  65000,     16,      0 },
	{    160,  65000,     15,      0 },
	{    161,  65000,     14,      0 },
	{    162, 137997,     46,      0 },
	{    163, 137997,     43,      0 },
	{    164, 137997,     40,      0 },
	{    165, 137997,     36,      0 },
	{    166, 137997,     34,      0 },
	{    167, 137997,     32,      0 },
	{    168, 137997,     30,      0 },
	{    169, 137997,     28,      0 },
	{    170, 137997,     26,      0 },
};

static code frontAe_t ae30fps50hzTbl[121] = {
	{     50,    333,    127,      0 },
	{     51,    333,    127,      0 },
	{     52,    333,    127,      0 },
	{     53,    333,    127,      0 },
	{     54,    333,    127,      0 },
	{     55,    333,    120,      0 },
	{     56,    333,    110,      0 },
	{     57,    333,    103,      0 },
	{     58,    333,     97,      0 },
	{     59,    333,     88,      0 },
	{     60,    333,     83,      0 },
	{     61,    333,     75,      0 },
	{     62,    333,     70,      0 },
	{     63,    333,     66,      0 },
	{     64,    333,     60,      0 },
	{     65,    333,     56,      0 },
	{     66,    333,     52,      0 },
	{     67,    333,     47,      0 },
	{     68,    333,     44,      0 },
	{     69,    333,     41,      0 },
	{     70,    333,     39,      0 },
	{     71,    333,     36,      0 },
	{     72,    333,     33,      0 },
	{     73,    333,     30,      0 },
	{     74,    333,     28,      0 },
	{     75,    333,     26,      0 },
	{     76,    333,     25,      0 },
	{     77,    333,     23,      0 },
	{     78,    333,     21,      0 },
	{     79,    333,     20,      0 },
	{     80,    333,     18,      0 },
	{     81,    333,     17,      0 },
	{     82,    333,     16,      0 },
	{     83,    500,     23,      0 },
	{     84,    500,     21,      0 },
	{     85,    500,     19,      0 },
	{     86,    500,     18,      0 },
	{     87,    500,     17,      0 },
	{     88,    500,     16,      0 },
	{     89,   1000,     30,      0 },
	{     90,   1000,     27,      0 },
	{     91,   1000,     26,      0 },
	{     92,   1000,     24,      0 },
	{     93,   1000,     23,      0 },
	{     94,   1000,     21,      0 },
	{     95,   1000,     20,      0 },
	{     96,   1000,     18,      0 },
	{     97,   1000,     17,      0 },
	{     98,   1000,     16,      0 },
	{     99,   1016,     15,      0 },
	{    100,   1089,     15,      0 },
	{    101,   1168,     15,      0 },
	{    102,   1252,     15,      0 },
	{    103,   1342,     15,      0 },
	{    104,   1439,     15,      0 },
	{    105,   1543,     15,      0 },
	{    106,   1654,     15,      0 },
	{    107,   1773,     15,      0 },
	{    108,   1901,     15,      0 },
	{    109,   2038,     15,      0 },
	{    110,   2185,     15,      0 },
	{    111,   2342,     15,      0 },
	{    112,   2511,     15,      0 },
	{    113,   2692,     15,      0 },
	{    114,   2886,     15,      0 },
	{    115,   3094,     15,      0 },
	{    116,   3317,     15,      0 },
	{    117,   3556,     15,      0 },
	{    118,   3812,     15,      0 },
	{    119,   4086,     15,      0 },
	{    120,   4380,     15,      0 },
	{    121,   4695,     15,      0 },
	{    122,   5032,     15,      0 },
	{    123,   5394,     15,      0 },
	{    124,   5782,     15,      0 },
	{    125,   6198,     15,      0 },
	{    126,   6643,     16,      0 },
	{    127,   7120,     16,      0 },
	{    128,   7632,     16,      0 },
	{    129,   8180,     16,      0 },
	{    130,   8768,     16,      0 },
	{    131,   9398,     16,      0 },
	{    132,  10073,     16,      0 },
	{    133,  10796,     16,      0 },
	{    134,  11000,     15,      0 },
	{    135,  12402,     16,      0 },
	{    136,  13000,     16,      0 },
	{    137,  15000,     17,      0 },
	{    138,  15000,     16,      0 },
	{    139,  17000,     16,      0 },
	{    140,  17000,     15,      0 },
	{    141,  19000,     17,      0 },
	{    142,  19000,     16,      0 },
	{    143,  22000,     16,      0 },
	{    144,  22000,     15,      0 },
	{    145,  25000,     18,      0 },
	{    146,  25000,     17,      0 },
	{    147,  25000,     16,      0 },
	{    148,  30549,     17,      0 },
	{    149,  30549,     16,      0 },
	{    150,  30549,     15,      0 },
	{    151,  40000,     22,      0 },
	{    152,  40000,     20,      0 },
	{    153,  40000,     19,      0 },
	{    154,  40000,     18,      0 },
	{    155,  40000,     17,      0 },
	{    156,  40000,     16,      0 },
	{    157,  40000,     15,      0 },
	{    158,  65000,     17,      0 },
	{    159,  65000,     16,      0 },
	{    160,  65000,     15,      0 },
	{    161,  65000,     14,      0 },
	{    162, 137997,     46,      0 },
	{    163, 137997,     43,      0 },
	{    164, 137997,     40,      0 },
	{    165, 137997,     36,      0 },
	{    166, 137997,     34,      0 },
	{    167, 137997,     32,      0 },
	{    168, 137997,     30,      0 },
	{    169, 137997,     28,      0 },
	{    170, 137997,     26,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[128] = {
	0x1000, 0x1010, 0x1020, 0x1030, 0x1040, 0x1050, 0x1060, 0x1070,
	0x1080, 0x1090, 0x10A0, 0x10B0, 0x10C0, 0x10D0, 0x10E0, 0x10F0,
	0x1100, 0x1110, 0x1120, 0x1130, 0x1140, 0x1150, 0x1160, 0x1170,
	0x1180, 0x1190, 0x11A0, 0x11B0, 0x11C0, 0x11D0, 0x11E0, 0x11F0,
	0x2200, 0x2210, 0x2220, 0x2230, 0x2240, 0x2250, 0x2260, 0x2270,
	0x2280, 0x2290, 0x22A0, 0x22B0, 0x22C0, 0x22D0, 0x22E0, 0x22F0,
	0x3300, 0x3310, 0x3320, 0x3330, 0x3340, 0x3350, 0x3360, 0x3370,
	0x3380, 0x3390, 0x33A0, 0x33B0, 0x33C0, 0x33D0, 0x33E0, 0x33F0,
	0x4400, 0x4410, 0x4420, 0x4430, 0x4440, 0x4450, 0x4460, 0x4470,
	0x4480, 0x4490, 0x44A0, 0x44B0, 0x44C0, 0x44D0, 0x44E0, 0x44F0,
	0x5500, 0x5510, 0x5520, 0x5530, 0x5540, 0x5550, 0x5560, 0x5570,
	0x5580, 0x5590, 0x55A0, 0x55B0, 0x55C0, 0x55D0, 0x55E0, 0x55F0,
	0x6600, 0x6610, 0x6620, 0x6630, 0x6640, 0x6650, 0x6660, 0x6670,
	0x6680, 0x6690, 0x66A0, 0x66B0, 0x66C0, 0x66D0, 0x66E0, 0x66F0,
	0x7700, 0x7710, 0x7720, 0x7730, 0x7740, 0x7750, 0x7760, 0x7770,
	0x7780, 0x7790, 0x77A0, 0x77B0, 0x77C0, 0x77D0, 0x77E0, 0x77F0,
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
	0x25, 0x00,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x25, 0x01,
	0x00, 0x11,
	0x01, 0x7B,
	0x02, 0x01,
	0x03, 0x03,
	0x04, 0x0D,
	0x05, 0x00,
	0x06, 0x00,
	0x07, 0x00,
	0x08, 0x00,
	0x09, 0x00,
	0x0A, 0x00,
	0x0B, 0xA0,
	0x0C, 0x00,
	0x0D, 0xA0,
	0x0E, 0x00,
	0x0F, 0xA0,
	0x10, 0x00,
	0x11, 0xA0,
	0x12, 0x00,
	0x13, 0x04,
	0x14, 0x00,
	0x15, 0x0A,
	0x16, 0x04,
	0x17, 0x08,
	0x18, 0x05,
	0x19, 0x08,
	//0x1A, 0x00,
	0x1B, 0x01,
	0x1C, 0x12,
	0x1D, 0x2A,

	0x1F, 0x08,
	0x20, 0x24,
	0x21, 0x05,
	0x22, 0x8A,
	0x23, 0x00,
	0x24, 0x20,
	0x25, 0x01,
	0x1E, 0x10,
};

static code UINT8 frontInit2CmdTbl[] = {
	0x1E, 0x00,
	0x25, 0x01,
};

static code UINT8 front30fpsPrev0CmdTbl[] = {
	0x01, /*0x74*/0x7B,  //Horizontal Blank Line Register  0x7b
	0x02, /*0x19*/0x01,  //Vertical Blank Line Register
	//0x03, 0x09,
	//0x04, 0xB4,
	0x05, 0x00,  //Global Background Composition
	0x06, 0x00,  //GRED Background Composition
	0x07, 0x00,  //RED Background Composition
	0x08, 0x00,  //BLUE Background Composition
	0x09, 0x00,  //GBLUE Background Composition
	//0x0A, 0x00,  //GRED Gain Control High Register
	//0x0B, 0x66,  //GRED Gain Control Low Register
	//0x0C, 0x00,  //RED Gain Control High Register
	//0x0D, 0x7F,  //RED Gain Control Low Register
	//0x0E, 0x00,  //BLUE Gain Control High Register
	//0x0F, 0x7F,  //BLUE Gain Control Low Register
	//0x10, 0x00,  //GBLUE Gain Control High Register
	//0x11, 0x7E,  //GBLUE Gain Control Low Register
	0x12, 0x00,  //Row Start High Control Register
	0x13, 0x04,  //Row Start Low Control Register
	0x14, 0x00,  //Col Start High Control Register
	0x15, 0x00,  //Col Start Low Control Register++//00
	0x16, 0x04,  //Window Height High Control Register  1032
	0x17, 0x08,  //Window Height Low Control Register
	0x18, 0x05,  //Window Width High Control Register   1288
	0x19, 0x10,  //Window Width Low Control Register
	0x1B, 0x01,  //Black Level Calibration Register
	0x1C, 0x10,  //A/D Bias Control Register
	0x1D, 0x29,  //Col Bias Current Control Register
	0x1F, 0x08,  //Timing Control Register
	0x20, 0x24,  //Sub Sampling Contro ------ preview to capture convert
	0x21, 0x05,  //Sub Sampling Mode Control++  0x05
	0x22, 0x8A,  //Frame Control Register++  0x88
	//0x22, 0x48,
	0x23, 0x00,  //VSYNC Output Control
	0x24, 0x20,  //Output Control
	0x1E, 0x10,  //Test Control
};

static code UINT8 front30fpsPrev1CmdTbl[] = {
	0x1E, 0x00,  //Test Control
	0x25, 0x00,  //SoftReset Control
};

static code UINT8 front30fpsPrev2CmdTbl[] = {
	0x25, 0x01,  //SoftReset Control
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};
#endif

static code UINT8 frontSnapCmdTbl[] = {
	0x01, 0x62,
	0x02, 0x19,
	0x05, 0x00,
	0x06, 0x00,
	0x07, 0x00,
	0x08, 0x00,
	0x09, 0x00,
	0x12, 0x00,
	0x13, 0x04,
	0x14, 0x00,
	0x15, 0x0A,
	0x16, 0x04,
	0x17, 0x08,
	0x18, 0x05,
	0x19, 0x08,
	0x1B, 0x01,
	0x1C, 0x10,
	0x1D, 0x29,
	0x1F, 0x08,
	0x20, 0x00,
	0x21, 0x00,
	0x22, 0x4A,  //0x48
	0x23, 0x00,
	0x24, 0x20,
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
 * @since	2008-10-20
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
 * @since	2008-10-20
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
 * @since	2008-10-20
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
 * @since	2008-10-20
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
 * @since	2008-10-20
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
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(100);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(100);
	i2cCmdTableSend(frontInit2CmdTbl, 1, sizeof(frontInit2CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	/* after power on. */
	frontAfterPowerOn();
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-20
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
 * @since	2008-10-20
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

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrev0CmdTbl, 1, sizeof(front30fpsPrev0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(100);
	i2cCmdTableSend(front30fpsPrev1CmdTbl, 1, sizeof(front30fpsPrev1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(100);
	i2cCmdTableSend(front30fpsPrev2CmdTbl, 1, sizeof(front30fpsPrev2CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);
}

/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-20
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

	/* here must wait vsync rising to avoid command sent fail (cross frame). */
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_SNAP);
}

/**
 * @brief	operating function to snap shot.
 * @param	first = [in] 0: not first, 1: first shot for burst shot.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-20
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
		frontSnapExposureTimeSet();
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
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
 * @since	2008-10-20
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
	UINT8 cmdTbl[2*8];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];

	cmdTbl[0] = 0x0A;
	cmdTbl[1] = (frontLastGain >> 8) & 0x0F;
	cmdTbl[2] = 0x0B;
	cmdTbl[3] = frontLastGain & 0xFF;
	cmdTbl[4] = 0x0C;
	cmdTbl[5] = (frontLastGain >> 8) & 0x0F;
	cmdTbl[6] = 0x0D;
	cmdTbl[7] = frontLastGain & 0xFF;
	cmdTbl[8] = 0x0E;
	cmdTbl[9] = (frontLastGain >> 8) & 0x0F;
	cmdTbl[10] = 0x0F;
	cmdTbl[11] = frontLastGain & 0xFF;
	cmdTbl[12] = 0x10;
	cmdTbl[13] = (frontLastGain >> 8) & 0x0F;
	cmdTbl[14] = 0x11;
	cmdTbl[15] = frontLastGain & 0xFF;
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
 * @since	2008-10-20
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
	espline <<= 1;

	cmdTbl[0] = 0x03;
	cmdTbl[1] = (espline >> 8) & 0xFF;
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
#if 0  /* here is demo code. */
	UINT8 i;


	/* set power control pin. */
	gpioByteFuncSet(PWR_PIN_BYTE, PWR_PIN_BIT, PWR_PIN_BIT);
	gpioByteDirSet(PWR_PIN_BYTE, PWR_PIN_BIT, PWR_PIN_BIT);
	gpioByteOutSet(PWR_PIN_BYTE, PWR_PIN_BIT, PWR_PIN_BIT);

	/* set reset control pin. */
	for ( i = 100; i > 0; i-- ) {
		DELAY_1ms();
	}
	gpioByteFuncSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
	gpioByteDirSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
	gpioByteOutSet(RST_PIN_BYTE, RST_PIN_BIT, 0x00);
	for ( i = 100; i > 0; i-- ) {
		DELAY_1ms();
	}
	gpioByteOutSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
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
 * @since	2008-10-20
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
	UINT8 cmdTbl[2*10];
	UINT16 s_gain, lastt_gain;


	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	s_gain = frontLastGain;

	lastt_gain = s_gain & 0x0FFF;

	cdspColorMatrixRestore();
//	XBYTE[0x21D3] = 0x02;
//	XBYTE[0x21D0] = 0x02;

	if ( lastt_gain > 0x160 ) {
		lastt_gain >>= 1;
		espline <<= 1;
	}

	cmdTbl[0] = 0x03;
	cmdTbl[1] = (espline >> 8) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0xFF;
	cmdTbl[4] = 0x0A;
	cmdTbl[5] = (lastt_gain >> 8) & 0x0F;
	cmdTbl[6] = 0x0B;
	cmdTbl[7] = lastt_gain & 0xFF;
	cmdTbl[8] = 0x0C;
	cmdTbl[9] = (lastt_gain >> 8) & 0x0F;
	cmdTbl[10] = 0x0D;
	cmdTbl[11] = lastt_gain & 0xFF;
	cmdTbl[12] = 0x0E;
	cmdTbl[13] = (lastt_gain >> 8) & 0x0F;
	cmdTbl[14] = 0x0F;
	cmdTbl[15] = lastt_gain & 0xFF;
	cmdTbl[16] = 0x10;
	cmdTbl[17] = (lastt_gain >> 8) & 0x0F;
	cmdTbl[18] = 0x11;
	cmdTbl[19] = lastt_gain & 0xFF;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-20
 * @todo	N/A
 * @bug		N/A
*/
static void
frontColorNoiseReduce(
	UINT8 gain
)
{

}

