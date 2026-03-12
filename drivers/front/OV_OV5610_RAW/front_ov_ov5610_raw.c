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
 * @brief		front sensor omni vision ov5610 driver.
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
#include "hal_cdsp_iq.h"
#include "hal.h"
#include "hal_global.h"

#include "front_logi.h"
#include "gpio.h"
#include "gpio_api.h"
#include "i2c.h"
#include "sp1k_calibration_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_snap_api.h"

#include "hal_cdsp.h"

#include "dbg_def.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "hal_front_common.h"
#include "sp1k_gpio_api.h"
#include "sp1k_front_api.h"

#undef TDC_5080
#undef MSHUTTER_CALIBRATION
#define SEN_TAG_TODO

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
/*
#define WIDTH_30FPS_PREV      (1280 - 128)
#define HEIGHT_30FPS_PREV     (480 - 48)
#define XOFF_30FPS_PREV       (310 + 64)
#define YOFF_30FPS_PREV       (8 + 24)
*/
#define WIDTH_30FPS_PREV      (1280 )
#define HEIGHT_30FPS_PREV     (480 )
#define XOFF_30FPS_PREV       (310 )
#define YOFF_30FPS_PREV       (8 )


#define LINE_TOT_30FPS_PREV   1632
#define FRAME_TOT_30FPS_PREV  490

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      2592//2304//2592
#define HEIGHT_SNAP     1944//1728//1944
#define XOFF_SNAP       252//(252 + (2592 - 2304)/2)
#define YOFF_SNAP       6//(6 + (1944 - 1728)/2) //4
#define LINE_TOT_SNAP   3000
#define FRAME_TOT_SNAP  2000

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    12
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#if 0
#define MCLK_DIV_60FPS_PREV    0
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    0
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    		12
#define MCLK_DIV_SNAP_BURST    	12
#define MCLK_DIV_SNAP_BURST_UP  12

#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
	#define PCLK_FREQ_30FPS_PREV  6000000L
	#if 0
	#define PCLK_FREQ_60FPS_PREV  0
	#endif

	#define PCLK_FREQ_SNAP       	6000000L
	#define PCLK_FREQ_SNAP_BURST    6000000L
	#define PCLK_FREQ_SNAP_BURST_UP 6000000L
#else
	#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV

	#if 0
	#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
	#endif

	#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  298
#define RESHAPE_HRISE_30FPS_PREV  300
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

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  1

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x1//0x01

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
#define I2C_CLK_DIV   1

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  102
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  101
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
#define GAIN_30FPS_MAX_IDX  16
#define GAIN_30FPS_MIN_IDX  0

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0
#endif

#define EXP_GAIN_SET_POS  0x40
#define GAIN_AFTER_EXP    0x01

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

#define K_FlashLightOn    0
#define K_FlashLightOff   1
#define K_FlashLightAuto  2

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

/*
*because some mechanical shutter driver ICs are not the same as our EVB,
*we must redefine the action of shutter...
*/
#define SHUTTER_CLOSE() \
{ \
	XBYTE[0x2032] |= 0x01;	\
	globalTimerWait(20);			\
	XBYTE[0x2032] &= 0xFC;	\
}

#define SHUTTER_OPEN() \
{ \
	XBYTE[0x2032] |= 0x02;	\
	globalTimerWait(20);           \
	XBYTE[0x2032] &= 0xFC;	\
}

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
__STATIC void frontOpSnapShot(UINT8 first,UINT8 mode, UINT8 scaleUp);
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
static UINT8 frontObCalibrate(void);
static void OV5610PrevOBCalDataSet(void);
static UINT8 OV5610OBPreviewCalibrate(void);
static UINT8 OV5610OBCaptureCalibrate(void);
static UINT8 OV5610OBSnapFrontFrame(UINT8 SnapNum, UINT8 R, UINT8 B, UINT8 Gr, UINT8 Gb);
static void OV5610OBFrameSnap(void);
static void OV5610FlashWBSet(void);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5610 RAW";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[103] = {
	{      0,    301,     20,      0 },
	{      1,    301,     19,      0 },
	{      2,    301,     17,      0 },
	{      3,    301,     15,      0 },
	{      4,    301,     14,      0 },
	{      5,    301,     12,      0 },
	{      6,    301,     11,      0 },
	{      7,    301,     10,      0 },
	{      8,    301,      9,      0 },
//	{      9,    300,      6,      0 },
	{      9,    301,      6,      0 },
	{     10,    301,      4,      0 },
//	{     11,    301,      3,      0 },
	{     11,    301,      2,      0 },
	{     12,    301,      1,      0 },
	{     13,    301,      0,      0 },
	{     14,    401,      5,      0 },
	{     15,    401,      3,      0 },
	{     16,    401,      2,      0 },
	{     17,    401,      0,      0 },
//	{     18,    598,      8,      0 },
	{     18,    598,      9,      0 },
	{     19,    600,      7,      0 },
	{     20,    600,      4,      0 },
	{     21,    598,      2,      0 },
	{     22,    603,      1,      0 },
	{     23,    603,      0,      0 },
	{     24,   1215,     14,      0 },
	{     25,   1205,     12,      0 },
	{     26,   1215,     11,      0 },
	{     27,   1215,     10,      0 },
//	{     28,   1196,      8,      0 },
	{     28,   1196,      9,      0 },
	{     29,   1196,      6,      0 },
	{     30,   1196,      4,      0 },
	{     31,   1196,      2,      0 },
	{     32,   1215,      1,      0 },
	{     33,   1268,      0,      0 },
	{     34,   1349,      0,      0 },
	{     35,   1456,      0,      0 },
	{     36,   1548,      0,      0 },
	{     37,   1671,      0,      0 },
	{     38,   1793,      0,      0 },
	{     39,   1910,      0,      0 },
	{     40,   2042,      0,      0 },
//	{     41,   2195,      0,      0 },
	{     41,   2163,      0,      0 },
	{     42,   2334,      0,      0 },
	{     43,   2535,      0,      0 },
	{     44,   2674,      0,      0 },
	{     45,   2884,      0,      0 },
	{     46,   3064,      0,      0 },
	{     47,   3342,      0,      0 },
//	{     48,   3587,      0,      0 },
	{     48,   3501,      0,      0 },
	{     49,   3771,      0,      0 },
	{     50,   4085,      0,      0 },
//	{     51,   4325,      0,      0 },
	{     51,   4456,      0,      0 },
	{     52,   4596,      0,      0 },
//	{     53,   5071,      0,      0 },
	{     53,   5071,      1,      0 },
	{     54,   5252,      0,      0 },
	{     55,   5656,      0,      0 },
	{     56,   6127,      0,      0 },
	{     57,   6684,      0,      0 },
	{     58,   7003,      0,      0 },
	{     59,   7353,      0,      0 },
	{     60,   8170,      0,      0 },
	{     61,   8650,      0,      0 },
	{     62,   9191,      0,      0 },
//	{     63,   9804,      0,      0 },
	{     63,   9804,      1,      0 },
	{     64,  10504,      0,      0 },
	{     65,  11312,      0,      0 },
//	{     66,  12255,      0,      0 },
	{     66,  12255,      2,      0 },
//	{     67,  13369,      0,      0 },
	{     67,  13369,      2,      0 },
//	{     68,  14706,      1,      0 },
	{     68,  14706,      3,      0 },
//	{     69,  14706,      0,      0 },
	{     69,  14706,      1,      0 },
//	{     70,  16340,      0,      0 },
	{     70,  16340,      2,      0 },
	{     71,  18382,      1,      0 },
	{     72,  18382,      0,      0 },
	{     73,  21008,      1,      0 },
	{     74,  21008,      0,      0 },
	{     75,  24509,      2,      0 },
	{     76,  24509,      0,      0 },
	{     77,  29411,      3,      0 },
	{     78,  29411,      1,      0 },
	{     79,  29411,      0,      0 },
	{     80,  36764,      3,      0 },
//	{     81,  36764,      1,      0 },
	{     81,  36764,      2,      0 },
	{     82,  36764,      0,      0 },
	{     83,  49018,      5,      0 },
	{     84,  49018,      4,      0 },
	{     85,  49018,      2,      0 },
	{     86,  49018,      0,      0 },
	{     87,  73526,      9,      0 },
	{     88,  73526,      7,      0 },
	{     89,  73526,      5,      0 },
	{     90,  73526,      3,      0 },
	{     91,  73526,      1,      0 },
	{     92,  73526,      0,      0 },
	{     93, 147044,     14,      0 },
	{     94, 147044,     13,      0 },
	{     95, 147044,     11,      0 },
	{     96, 147044,     10,      0 },
	{     97, 147044,      9,      0 },
	{     98, 147044,      7,      0 },
	{     99, 147044,      5,      0 },
	{    100, 147044,      3,      0 },
	{    101, 147044,      1,      0 },
	{    102, 147044,      0,      0 },
};

static code frontAe_t ae30fps50hzTbl[101] = {
	{      0,    333,     19,      0 },
	{      1,    334,     18,      0 },
	{      2,    334,     16,      0 },
	{      3,    333,     14,      0 },
//	{      4,    334,     13,      0 },
	{      4,    334,     12,      0 },
//	{      5,    334,     12,      0 },
	{      5,    334,     11,      0 },
	{      6,    334,     10,      0 },
	{      7,    334,      9,      0 },
	{      8,    333,      7,      0 },
	{      9,    333,      5,      0 },
	{     10,    333,      3,      0 },
//	{     11,    334,      2,      0 },
	{     11,    334,      1,      0 },
	{     12,    334,      0,      0 },
	{     13,    500,      9,      0 },
	{     14,    502,      7,      0 },
	{     15,    502,      5,      0 },
	{     16,    502,      3,      0 },
	{     17,    500,      1,      0 },
	{     18,    502,      0,      0 },
//	{     19,    994,     18,      0 },
	{     19,    994,     14,      0 },
//	{     20,   1000,     15,      0 },
	{     20,   1000,     13,      0 },
//	{     21,   1000,     13,      0 },
	{     21,   1000,     11,      0 },
	{     22,   1014,     10,      0 },
	{     23,   1014,      9,      0 },
	{     24,   1007,      7,      0 },
	{     25,   1007,      5,      0 },
	{     26,   1007,      3,      0 },
//	{     27,   1000,      1,      0 },
	{     27,   1000,      1,      0 },
	{     28,   1014,      0,      0 },
	{     29,   1097,      0,      0 },
	{     30,   1176,      0,      0 },
	{     31,   1268,      0,      0 },
//	{     32,   1290,      0,      0 },
	{     32,   1362,      0,      0 },
	{     33,   1456,      0,      0 },
	{     34,   1548,      0,      0 },
	{     35,   1671,      0,      0 },
	{     36,   1793,      0,      0 },
	{     37,   1910,      0,      0 },
	{     38,   2042,      0,      0 },
//	{     39,   2195,      0,      0 },
	{     39,   2195,      1,      0 },
//	{     40,   2262,      0,      0 },
	{     40,   2334,      0,      0 },
	{     41,   2535,      0,      0 },
	{     42,   2674,      0,      0 },
	{     43,   2884,      0,      0 },
	{     44,   3064,      0,      0 },
	{     45,   3342,      0,      0 },
//	{     46,   3587,      0,      0 },
	{     46,   3676,      1,      0 },
	{     47,   3771,      0,      0 },
	{     48,   4085,      0,      0 },
//	{     49,   4325,      0,      0 },
	{     49,   4456,      0,      0 },
	{     50,   4596,      0,      0 },
//	{     51,   5071,      0,      0 },
	{     51,   5071,      1,      0 },
	{     52,   5252,      0,      0 },
	{     53,   5656,      0,      0 },
	{     54,   6127,      0,      0 },
	{     55,   6684,      0,      0 },
//	{     56,   7003,      0,      0 },
	{     56,   7003,      1,      0 },
	{     57,   7353,      0,      0 },
//	{     58,   8170,      0,      0 },
	{     58,   8170,      1,      0 },
	{     59,   8650,      0,      0 },
	{     60,   9191,      0,      0 },
//	{     61,   9804,      0,      0 },
	{     61,   9804,      1,      0 },
	{     62,  10504,      0,      0 },
	{     63,  11312,      0,      0 },
//	{     64,  12255,      0,      0 },
	{     64,  12255,      2,      0 },
//	{     65,  13369,      0,      0 },
	{     65,  13369,      2,      0 },
//	{     66,  14706,     10,      0 },
	{     66,  14706,      3,      0 },
//	{     67,  14706,      0,      0 },
	{     67,  14706,      1,      0 },
//	{     68,  16340,      0,      0 },
	{     68,  16340,      2,      0 },
	{     69,  18382,      1,      0 },
	{     70,  18382,      0,      0 },
	{     71,  21008,      1,      0 },
	{     72,  21008,      0,      0 },
	{     73,  24509,      2,      0 },
	{     74,  24509,      0,      0 },
	{     75,  29411,      3,      0 },
	{     76,  29411,      1,      0 },
	{     77,  29411,      0,      0 },
	{     78,  36764,      3,      0 },
	{     79,  36764,      1,      0 },
	{     80,  36764,      0,      0 },
	{     81,  49018,      5,      0 },
	{     82,  49018,      4,      0 },
	{     83,  49018,      2,      0 },
	{     84,  49018,      0,      0 },
	{     85,  73526,      9,      0 },
	{     86,  73526,      7,      0 },
	{     87,  73526,      5,      0 },
	{     88,  73526,      3,      0 },
	{     89,  73526,      1,      0 },
	{     90,  73526,      0,      0 },
	{     91, 147044,     14,      0 },
	{     92, 147044,     13,      0 },
	{     93, 147044,     11,      0 },
	{     94, 147044,     10,      0 },
	{     95, 147044,      9,      0 },
	{     96, 147044,      7,      0 },
	{     97, 147044,      5,      0 },
	{     98, 147044,      3,      0 },
	{     99, 147044,      1,      0 },
	{    100, 147044,      0,      0 },
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[21] = {
	  8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
	 18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
	 28,
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

static code UINT32 snapShutter60hzTbl[103] = {
	129,
	134,
	144,
	157,
	164,
	180,
	190,
	200,
	212,
	240,
	258,
	267,
	289,
	301,
	332,
	356,
	385,
	401,
	448,
	480,
	514,
	552,
	579,
	603,
	663,
	723,
	768,
	810,
	897,
	956,
	1025,
	1104,
	1167,
	1268,
	1349,
	1456,
	1548,
	1671,
	1793,
	1910,
	2042,
	2195,
	2334,
	2535,
	2674,
	2884,
	3064,
	3342,
	3587,
	3771,
	4085,
	4325,
	4596,
	5071,
	5252,
	5656,
	6127,
	6684,
	7003,
	7353,
	8170,
	8650,
	9191,
	9804,
	10504,
	11312,
	12255,
	13369,
	14118,
	14706,
	16340,
	17647,
	18382,
	20168,
	21008,
	22624,
	24509,
	26143,
	28235,
	29411,
	32679,
	35293,
	36764,
	40567,
	42015,
	45247,
	49018,
	51901,
	56923,
	60849,
	65356,
	70585,
	73526,
	80206,
	84025,
	92870,
	98029,
	103796,
	113841,
	121692,
	130706,
	141162,
	147044,
};

code UINT32 snapShutter50hzTbl[101] = {
	148,
	154,
	167,
	182,
	191,
	201,
	223,
	236,
	258,
	276,
	296,
	309,
	334,
	354,
	389,
	415,
	446,
	480,
	502,
	549,
	580,
	632,
	676,
	716,
	780,
	834,
	895,
	960,
	1014,
	1097,
	1176,
	1268,
	1290,
	1456,
	1548,
	1671,
	1793,
	1910,
	2042,
	2195,
	2262,
	2535,
	2674,
	2884,
	3064,
	3342,
	3587,
	3771,
	4085,
	4325,
	4596,
	5071,
	5252,
	5656,
	6127,
	6684,
	7003,
	7353,
	8170,
	8650,
	9191,
	9804,
	10504,
	11312,
	12255,
	13369,
	14118,
	14706,
	16340,
	17647,
	18382,
	20168,
	21008,
	22624,
	24509,
	26143,
	28235,
	29411,
	32679,
	35293,
	36764,
	40567,
	42015,
	45247,
	49018,
	51901,
	56923,
	60849,
	65356,
	70585,
	73526,
	80206,
	84025,
	92870,
	98029,
	103796,
	113841,
	121692,
	130706,
	141162,
	147044,
};


code UINT16 mshutter_early_time[43] = {
	0	,	0	,	0	,	0	,	36	,	143	,	287	,	574	,	717	,	1004,
	1184,	1291,	1435,	1632,	1741,	1865,	1986,	2084,	2179,	2321,
	2382,	2476,	2588,	2630,	2710,	2785,	2894,	2908,	3024,	3091,
	3136,	3178,	3232,	3251,	3254,	3304,	3377,	3367,	3424,	3404,
	3461,	3480,	3495,
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
	0x12, 0x80,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x12, 0X20,  //set DV mode
	0x11, 0X80,  //
	0x03, 0X44,  //
	0x16, 0X00,  //
	0x17, 0X14,  //
	0x18, 0X64,  //
	0x19, 0X00,  //
	0x1a, 0X3d,  //
	0x09, 0X08,  //org 0x00

	0x0c, 0X08,  //org 0x38
	0x0d, 0X31,  //
	0x13, 0XC0,  //
	0x14, 0XC6,  //
	0x15, 0X42,  //

	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//
	0x0e, 0X00,  //
	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//

	0x0f, 0X42,  //

	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//
	0x1b, 0X00,  //
	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//

	0x22, 0X80,  //
	0x23, 0X80,  //
	0x32, 0X39,  //
	0x33, 0X1b,  //
	0x34, 0X5a,  //
	0x35, 0X48,  //
	0x36, 0X20,  //
	0x37, 0X04,  //0x00 ??
	0x38, 0Xd3,  //
	0x39, 0X00,  //
	0x3b, 0X18,  //
	0x3c, 0X1f,  //
};

static code UINT8 frontInit2CmdTbl[] = {
	0x44, 0x00,  //
	0x48, 0x00,  //
	0x49, 0x89,  //0x19
	0x4b, 0x08,  //
	0x4c, 0x30,  //
	0x4d, 0x00,  //
	0x4e, 0x0f,  //

	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//
	0x4f, 0x0a,  //used to solve one narrow horizontal band
	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//

	0x50, 0x4f,  //

	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//
	0x10, 0x3d,  //
	0x04, 0x00,  //
	0x00, 0x10,  //
	//++++++++++++++++++++chenbin@2005.05.18 add +++++++++++++++++//
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x12, 0x20,
	0x03, 0x44,
	0x17, 0x14,
	0x18, 0x64,
	0x19, 0x00,
	0x1A, 0x3D,
	0x1B, 0x00,

	0x11, 0x80,
	0x09, 0x08,
	0x0C, 0x08,
	0x32, 0x39,
	0x33, 0x1B,
	0x34, 0x5A,
	0x36, 0x20,
	0x48, 0x00,
	0x4B, 0x08,
	0x4F, 0x0A,
};

#if 0
static code UINT8 front60fpsPrevCmdTbl[] = {
};
#endif

static code UINT8 frontSnapCmdTbl[] = {

	0x12,0x00, /*this code change SVGA/UXGA immediately*/
	0x1a,0xf4,
	0x17,0x0f,
	0x18,0xb3,
	0x19,0x00,
	0x03,0x44,
	0x1b,0x01,
  #if 0 /*always HSYNC, no HREF*/
	0x15,0x82,/*RD19 0x02??*/
  #else
	0x15,0x42,
  #endif
};

static code UINT8 frontObSnapCmdTbl[] = {
	0x10, 0xFF,
	0x00, 0x08,
	//0x00, 0x0f,

	0x12, 0x00,
	0x1A, 0xF5,
	0x17, 0x0F,
	0x18, 0xB3,
	0x19, 0x00,
	0x03, 0x4D,
	0x1B, 0x01,
	0x15, 0x42,
};

static code UINT8 frontObPrev0CmdTbl[] = {
	0x00, 0x7F,
	0x10, 0x3D,
	0x04, 0x08,
};

static code UINT8 frontObPrev1CmdTbl[] = {
	0x3F, 0x0F, //cs add
	0x40, 0x00, //cs add
	0x41, 0x00, //cs add
	0x42, 0x00, //cs add
	0x43, 0x00, //cs add
};

static code UINT8 frontObSnap0CmdTbl[] = {
	0x11, 0x81,
	0x09, 0x00,
	0x33, 0x69,
	0x34, 0x5f,
	0x36, 0x28,
	0x4b, 0x18,
	0x4f, 0x1a,
//	0x03, 0x4d,
//	0x1b, 0x01,
//	0x12, 0x00,
	0x49, 0x19,
	0x48, 0x80,

	0x17, 0x0f,
	0x18, 0xb3,
	0x19, 0x00,
	0x1a, 0xf5,
	0x03, 0x4d,
	0x1b, 0x00,  //0x01,
};

static code UINT8 frontObSnap1CmdTbl[] = {
	0x00, 0x1f,
	0x04, 0x06,
	0x10, 0xf0,

	0x12, 0x00,
};

static code UINT8 frontObSnap2CmdTbl[] = {
	0x3f, 0x01,
	0x40, 249,
	0x41, 255,
	0x42, 253,
	0x43, 2,
};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;

static UINT8 ob_R_set, ob_B_set, ob_Gr_set, ob_Gb_set;

static UINT8 G_P_EN;		//Jim for OB cal
static UINT8 G_P_B;		//Jim for OB cal
static UINT8 G_P_Gb;		//Jim for OB cal
static UINT8 G_P_Gr;
static UINT8 G_P_R;
static UINT8 G_C_EN;		//Jim for OB cal
static UINT8 G_C_B;		//Jim for OB cal
static UINT8 G_C_Gb;		//Jim for OB cal
static UINT8 G_C_Gr;
static UINT8 G_C_R;

static UINT8 G_mode = 0;

static UINT16 FR, FG1, FB, FG2;
static UINT8 STROBE;

static UINT16 caliMshtterDelay;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
//extern void HAL_Front10UsWait(UINT32);
//extern UINT8 HAL_CdspEofWait(UINT8 Number);
//extern void cdspColorMatrixRestore(void);

#ifndef SEN_TAG_TODO
	extern UINT8 calibrationObData[];

	extern UINT8 G_AETblSel;
	extern SINT16 gId;
	extern UINT8 G_luma;

	extern UINT8 G_FlashStatus;	
	extern UINT8 G_FlashCharging;
#endif
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
	//frontSensorOpen(

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

	/* set i2c pin. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(1);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	OV5610PrevOBCalDataSet();
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

    TAG();
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
    TAG();
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	OV5610PrevOBCalDataSet();

	cdspBayerPatternSet(BAYER_PTN_PREV);

	#if 1 // for Strobe TEST
		#ifndef SEN_TAG_TODO
			G_FlashStatus = K_FlashLightOn;		
			G_FlashCharging = 1;
		#endif
		sp1kSTChgEnSet(0);
		sp1kSTChgEnSet(1);
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

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
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
	UINT8 first,
	UINT8 mode,
	UINT8 scaleUp
)
{
    

	if ( first ) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);		
		i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);     
		frontSnapExposureTimeSet(mode,scaleUp);
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
	UINT8 cmdTbl[2*3];


	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	espline = (pixclk * 10) / (ltotal * frontLastShutter);

    printf("OV5610 espline=%d\n",espline);
	
	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 3) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0x07;
	cmdTbl[4] = 0x45;
	cmdTbl[5] = 0x00;
//	cmdTbl[6] = 0x2D;
//	cmdTbl[7] = 0;
//	cmdTbl[8] = 0x2E;
//	cmdTbl[9] = 0;
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
	UINT8 RLSB, RMSB;


	//sp1kNandRsvRead(0x37, (K_SDRAM_CommonEndAddr << 1));

	M_DRAM_SetStartAddress(K_SDRAM_CommonEndAddr, 1);
	M_DRAM_ReadWord(RLSB, RMSB);
	if ( RLSB != 0x00 ) {
		return FAIL;
	}

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
	UINT8 RLSB,RMSB;
	UINT8 P_EN;
	UINT8 P_B;
	UINT8 P_Gb;
	UINT8 P_Gr;
	UINT8 P_R;
	UINT8 C_EN;
	UINT8 C_B;
	UINT8 C_Gb;
	UINT8 C_Gr;
	UINT8 C_R;
	UINT8 ret;

	printf("Jim : Prevoew OB Cal start!!! \n");
	frontOpPreviewModeSet();
	//frontOpShutterClose();

	frontOpShutterClose();
	ret = OV5610OBPreviewCalibrate();
	frontOpShutterOpen();
	if ( ret != SUCCESS ) {
		return FAIL;
	}

	printf("Jim : Prevoew OB Cal Done!!! \n");
	//return SUCCESS;

	frontOpSnapModeSet();

	frontOpShutterClose();
	ret = OV5610OBCaptureCalibrate();
	frontOpShutterOpen();
	if ( ret != SUCCESS ) {
		return FAIL;
	}

	printf("Jim : Capture OB Cal Done!!! \n");
	P_EN = G_P_EN;
	P_B = G_P_B;
	P_Gb = G_P_Gb;
	P_Gr = G_P_Gr;
	P_R = G_P_R;
	C_EN = G_C_EN;
	C_B = G_C_B;
	C_Gb = G_C_Gb;
	C_Gr = G_C_Gr;
	C_R = G_C_R;

	M_DRAM_SetStartAddress(K_SDRAM_CommonEndAddr,0);
	RLSB = 0x00;
	RMSB = P_EN;
	M_DRAM_WriteWord(RLSB, RMSB);
	RLSB = P_B;
	RMSB = P_Gb;
	M_DRAM_WriteWord(RLSB, RMSB);
	RLSB = P_Gr;
	RMSB = P_R;
	M_DRAM_WriteWord(RLSB, RMSB);

	//sp1kNandRsvWrite(0x37, (K_SDRAM_CommonEndAddr<< 1));

	M_DRAM_SetStartAddress(K_SDRAM_CommonEndAddr+3,0);
	RLSB = 0x00;
	RMSB = C_EN;
	M_DRAM_WriteWord(RLSB, RMSB);
	RLSB = C_B;
	RMSB = C_Gb;
	M_DRAM_WriteWord(RLSB, RMSB);
	RLSB = C_Gr;
	RMSB = C_R;
	M_DRAM_WriteWord(RLSB, RMSB);

	//sp1kNandRsvWrite(0x37, (K_SDRAM_CommonEndAddr<< 1));

	printf("reg[0x3F] = 0x%bx\n", P_EN);
	printf("reg[0x40] = 0x%bx\n", P_B);
	printf("reg[0x41] = 0x%bx\n", P_R);
	printf("reg[0x42] = 0x%bx\n", P_Gb);
	printf("reg[0x43] = 0x%bx\n", P_Gr);
	printf("reg[0x3F] = 0x%bx\n", C_EN);
	printf("reg[0x40] = 0x%bx\n", C_B);
	printf("reg[0x41] = 0x%bx\n", C_R);
	printf("reg[0x42] = 0x%bx\n", C_Gb);
	printf("reg[0x43] = 0x%bx\n", C_Gr);

	frontOpPreviewModeSet();
	//sp1kAeSetModeEn(AE_ENABLE);

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
/*
#ifndef TDC_5080
	sp1kSHT_BSet(0);		  //SHT Open
	sp1kSHT_BCfg();
	sp1kSHT_ASet(1);
	sp1kSHT_ACfg();
	globalTimerWait(10);
	sp1kSHT_ASet(0);
	sp1kSHT_ACfg();
#else
	sp1kSHT_ACfg();
	sp1kSHT_BCfg();
	SHUTTER_OPEN();
#endif
*/
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
/*
#ifndef TDC_5080
	sp1kSHT_BSet(1);		  //SHT Close
	sp1kSHT_BCfg();
	sp1kSHT_ASet(1);
	sp1kSHT_ACfg();
	globalTimerWait(10);
	sp1kSHT_ASet(0);
	sp1kSHT_ACfg();
#else
	sp1kSHT_ACfg();
	sp1kSHT_BCfg();
	SHUTTER_CLOSE();
#endif
*/
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
	UINT8 RLSB, RMSB;


   	#if 1
	//sp1kNandRsvRead(0x37, (K_SDRAM_CommonEndAddr<< 1));	//Jim for OB cal

	M_DRAM_SetStartAddress(K_SDRAM_CommonEndAddr,1);
	M_DRAM_ReadWord(RLSB, RMSB);
	if ( RLSB != 0x00 ) {
		//osMsgPost(SP1K_MSG_STATE_TO_CAL_MENU);
		//OV5610PreviewOBCal();
		//OV5610OBCal();

		return;
	}
	#endif

	sp1kSHT_ACfg();
	sp1kSHT_ASet(0);
	sp1kFrexCfg();
	sp1kFrexSet(0);
	sp1kExpstbCfg();
	sp1kExpstbSet(0);
	sp1kSHT_BCfg();
	sp1kSHT_BSet(0);

	sp1kSTChgEnSet(0);
	sp1kSTChgEnCfg();
	sp1kST_TrigSet(0);
	sp1kST_TrigCfg();
	sp1kSTChgRdyCfg();

	frontOpShutterOpen();

	#if 1 // for Strobe TEST
		#ifndef SEN_TAG_TODO
			G_FlashStatus = K_FlashLightOn;
			G_FlashCharging = 1;
		#endif	
	//sp1kSTChgEnSet(1);
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
	UINT8 mode, 
	UINT8 scaleUp
)
{
	frontSnapCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 tmp0, pixclk;
	UINT8 cmdTbl[2*6];
	UINT8 s_gain, lastt_gain;


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
#endif
	cmdTbl[0] = 0x10;
	cmdTbl[1] = (espline >> 3) & 0xFF;
	cmdTbl[2] = 0x04;
	cmdTbl[3] = espline & 0x07;
	cmdTbl[4] = 0x45;
	cmdTbl[5] = (espline >> 11) & 0x3F;
	cmdTbl[6] = 0x2D;
	cmdTbl[7] = tmp0;
	cmdTbl[8] = 0x2E;
	cmdTbl[9] = tmp0 >> 8;
	cmdTbl[10] = 0x00;
	cmdTbl[11] = lastt_gain;
	//i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
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


void
OV5610PrevOBCalDataSet(
	void
)
{
	UINT8 RLSB, RMSB;
	UINT8 cmdTbl[2*5];


	//sp1kNandRsvRead(0x37, (K_SDRAM_CommonEndAddr << 1));

	M_DRAM_SetStartAddress(K_SDRAM_CommonEndAddr, 1);
	M_DRAM_ReadWord(RLSB, RMSB);
	cmdTbl[0] = 0x3F;
	cmdTbl[1] = RMSB;
	//printf("0x3F = %bu\n", RMSB);
	M_DRAM_ReadWord(RLSB, RMSB);
	cmdTbl[2] = 0x40;
	cmdTbl[3] = RLSB;
	cmdTbl[4] = 0x42;
	cmdTbl[5] = RMSB;
	//printf("0x40 = %bu\n", RLSB);
	//printf("0x42 = %bu\n", RMSB);
	M_DRAM_ReadWord(RLSB, RMSB);
	cmdTbl[6] = 0x43;
	cmdTbl[7] = RLSB;
	cmdTbl[8] = 0x41;
	cmdTbl[9] = RMSB;
	//printf("0x43 = %bu\n", RLSB);
	//printf("0x41 = %bu\n", RMSB);
	i2cCmdTableSend(cmdTbl, 1, 10/2, I2C_TRANS_MODE_NORMAL, 0, 0);
}

static UINT8
OV5610OBPreviewCalibrate(
	void
)		//Jim for OB cal
{
	UINT8 reg_0x05, reg_0x06, reg_0x07, reg_0x08, reg_0x3F;
	UINT8 comp_0x3F, comp_0x40, comp_0x41, comp_0x42, comp_0x43;
	UINT8 counter;
	UINT8 tmp0;
	UINT8 cmdTbl[2*5], addrTbl[5], dataTbl[5];


	printf("\n ob calibration \n");
	//sp1kAeSetModeEn(AE_DISABLE);
	//sp1kAwbModeSet(SP1K_AWB_MODE_DISABLE);

	i2cCmdTableSend(frontObPrev0CmdTbl, 1, sizeof(frontObPrev0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

	i2cCmdTableSend(frontObPrev1CmdTbl, 1, sizeof(frontObPrev1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);

	addrTbl[0] = 0x05;  // read back reg_0x05 = dataTbl[0]
	addrTbl[1] = 0x06;  // read back reg_0x06 = dataTbl[1]
	addrTbl[2] = 0x07;  // read back reg_0x07 = dataTbl[2]
	addrTbl[3] = 0x08;  // read back reg_0x08 = dataTbl[3]
	addrTbl[4] = 0x3F;  // read back reg_0x3F = dataTbl[4]
	i2cRegRead(addrTbl, dataTbl, 1, 5, 1, 0, 0);
	reg_0x05 = dataTbl[0];
	reg_0x06 = dataTbl[1];
	reg_0x07 = dataTbl[2];
	reg_0x08 = dataTbl[3];
	reg_0x3F = dataTbl[4];

	comp_0x3F = 0x0F;
	comp_0x40 = 0;
	comp_0x41 = 0;
	comp_0x42 = 0;
	comp_0x43 = 0;

	cmdTbl[0] = 0x3F;
	if (reg_0x05 > 6) {
		reg_0x3F &= ~0x08;
	}
	else {
		reg_0x3F |= 0x08;
	}
	cmdTbl[1] = reg_0x3F;

	cmdTbl[2] = 0x3F;
	if (reg_0x06 > 4) {
		reg_0x3F &= ~0x02;
	}
	else {
		reg_0x3F |= 0x02;
	}
	cmdTbl[3] = reg_0x3F;

	cmdTbl[4] = 0x3F;
	if (reg_0x07 > 4) {
		reg_0x3F &= ~0x01;
	}
	else {
		reg_0x3F |= 0x01;
	}
	cmdTbl[5] = reg_0x3F;

	cmdTbl[6] = 0x3F;
	if (reg_0x08 > 6) {
		reg_0x3F &= ~0x04;
	}
	else {
		reg_0x3F |= 0x04;
	}
	cmdTbl[7] = reg_0x3F;

	i2cCmdTableSend(cmdTbl, 1, 8/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	comp_0x3F = reg_0x3F;

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);

	if ((comp_0x3F & 0x08)==0) {
		for (counter = 0xff ; counter > 0 ; counter--) {
			cmdTbl[0] = 0x40;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x05;
			i2cRegRead(addrTbl, &reg_0x05, 1, 1, 1, 0, 0);
			printf("reg[0x3F]= %bu, reg[0x40] = %bu, reg[0x05] = %bu\n", reg_0x3F, counter,reg_0x05);
			if (reg_0x05 < 6) {
				if (counter == 0xff) {
					reg_0x3F = comp_0x3F | 0x08;
					cmdTbl[0] = 0x3F;
					cmdTbl[1] = reg_0x3F;
					cmdTbl[2] = 0x40;
					cmdTbl[3] = 0;
					i2cCmdTableSend(cmdTbl, 1, 4/2, I2C_TRANS_MODE_NORMAL, 0, 0);
					frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
					addrTbl[0] = 0x05;
					i2cRegRead(addrTbl, &tmp0, 1, 1, 1, 0, 0);
					printf("reg[0x3F]= %bu, reg[0x40] = 0, reg[0x05] = %bu\n", reg_0x3F, reg_0x05);
					if ((tmp0 >= reg_0x05) && (tmp0 >= 6)&&(tmp0 <= 9)) {
						comp_0x3F = reg_0x3F;
						counter = 0;
					}
					else {
						if ((reg_0x05 > 1)&&(tmp0 > 9)) {
							counter = 0xff;
						}
						else {
							comp_0x3F = reg_0x3F;
							counter = 0;
						}
					}
				}
				else {
					counter = counter+1;
				}

				comp_0x40 = counter;
				cmdTbl[0] = 0x40;
				cmdTbl[1] = comp_0x40;
				i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);
				break;
			}
		}
	}
	else {
		for (counter = 1 ; counter < 0xff ; counter++) {
			cmdTbl[0] = 0x40;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x05;
			i2cRegRead(addrTbl, &reg_0x05, 1, 1, 1, 0, 0);
			if (reg_0x05 > 6) {
				comp_0x40 = counter-1;
				break;
			}
			printf("reg[0x3F]= %bu, reg[0x40] = %bu, reg[0x05] = %bu\n", reg_0x3F, counter,reg_0x05);
		}
	}

	if ((comp_0x3F & 0x02)==0) {
		for (counter = 0xff ; counter > 0 ; counter--) {
			cmdTbl[0] = 0x42;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x06;
			i2cRegRead(addrTbl, &reg_0x06, 1, 1, 1, 0, 0);
			if (reg_0x06 < 4) {
				if (counter == 0xff) {
					reg_0x3F = comp_0x3F | 0x02;
					cmdTbl[0] = 0x3F;
					cmdTbl[1] = reg_0x3F;
					cmdTbl[2] = 0x42;
					cmdTbl[3] = 0;
					i2cCmdTableSend(cmdTbl, 1, 4/2, I2C_TRANS_MODE_NORMAL, 0, 0);
					frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
					addrTbl[0] = 0x06;
					i2cRegRead(addrTbl, &tmp0, 1, 1, 1, 0, 0);
					if ((tmp0 >= reg_0x06) && (tmp0 >= 4)&&(tmp0 <= 6)) {
						comp_0x3F = reg_0x3F;
						counter = 0;
					}
					else {
						if ((reg_0x06 > 1)&&(tmp0 > 8)) {
							counter = 0xff;
						}
						else {
							comp_0x3F = reg_0x3F;
							counter = 0;
						}
					}
				}
				else {
					counter = counter+1;
				}

				comp_0x42 = counter;
				cmdTbl[0] = 0x42;
				cmdTbl[1] = comp_0x42;
				i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);
				break;
			}
		}
	}
	else {
		for (counter = 1 ; counter < 0xff ; counter++) {
			cmdTbl[0] = 0x42;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x06;
			i2cRegRead(addrTbl, &reg_0x06, 1, 1, 1, 0, 0);
			if (reg_0x06 > 4) {
				comp_0x42 = counter-1;
				break;
			}
		}
	}

	if ((comp_0x3F & 0x01)==0) {
		for (counter = 0xff ; counter > 0 ; counter--) {
			cmdTbl[0] = 0x43;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x07;
			i2cRegRead(addrTbl, &reg_0x07, 1, 1, 1, 0, 0);
			if (reg_0x07 < 4) {
				if (counter == 0xff) {
					reg_0x3F = comp_0x3F | 0x01;
					cmdTbl[0] = 0x3F;
					cmdTbl[1] = reg_0x3F;
					cmdTbl[2] = 0x43;
					cmdTbl[3] = 0;
					i2cCmdTableSend(cmdTbl, 1, 4/2, I2C_TRANS_MODE_NORMAL, 0, 0);
					frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
					addrTbl[0] = 0x07;
					i2cRegRead(addrTbl, &tmp0, 1, 1, 1, 0, 0);
					if ((tmp0 >= reg_0x07) && (tmp0 >= 4)&&(tmp0 <= 6)) {
						comp_0x3F = reg_0x3F;
						counter = 0;
					}
					else {
						if ((reg_0x07 > 1)&&(tmp0 > 8)) {
							counter = 0xff;
						}
						else {
							comp_0x3F = reg_0x3F;
							counter = 0;
						}
					}
				}
				else {
					counter = counter+1;
				}

				comp_0x43 = counter;
				cmdTbl[0] = 0x43;
				cmdTbl[1] = comp_0x43;
				i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);
				break;
			}
		}
	}
	else {
		for (counter = 1 ; counter < 0xff ; counter++) {
			cmdTbl[0] = 0x43;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x07;
			i2cRegRead(addrTbl, &reg_0x07, 1, 1, 1, 0, 0);
			if (reg_0x07 > 4) {
				comp_0x43 = counter-1;
				break;
			}
		}
	}

	if ((comp_0x3F & 0x04)==0) {
		for (counter = 0xff ; counter > 0 ; counter--) {
			cmdTbl[0] = 0x41;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x08;
			i2cRegRead(addrTbl, &reg_0x08, 1, 1, 1, 0, 0);
			if (reg_0x08 < 6) {
				if (counter == 0xff) {
					reg_0x3F = comp_0x3F | 0x04;
					cmdTbl[0] = 0x3F;
					cmdTbl[1] = reg_0x3F;
					cmdTbl[2] = 0x41;
					cmdTbl[3] = 0;
					i2cCmdTableSend(cmdTbl, 1, 4/2, I2C_TRANS_MODE_NORMAL, 0, 0);
					frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
					addrTbl[0] = 0x08;
					i2cRegRead(addrTbl, &tmp0, 1, 1, 1, 0, 0);
					if ((tmp0 >= reg_0x08) && (tmp0 >= 6)&&(tmp0 <= 9)) {
						comp_0x3F = reg_0x3F;
						counter = 0;
					}
					else {
						if ((reg_0x08 > 1)&&(tmp0 > 9)) {
							counter = 0xff;
						}
						else {
							comp_0x3F = reg_0x3F;
							counter = 0;
						}
					}
				}
				else {
					counter = counter+1;
				}

				comp_0x41 = counter;
				cmdTbl[0] = 0x41;
				cmdTbl[1] = comp_0x41;
				i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);
				break;
			}
		}
	}
	else {
		for (counter = 1 ; counter < 0xff ; counter++) {
			cmdTbl[0] = 0x41;
			cmdTbl[1] = counter;
			i2cCmdTableSend(cmdTbl, 1, 2/2, I2C_TRANS_MODE_NORMAL, 0, 0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			addrTbl[0] = 0x08;
			i2cRegRead(addrTbl, &reg_0x08, 1, 1, 1, 0, 0);
			if (reg_0x08 > 6) {
				comp_0x41 = counter;
				break;
			}
		}
	}

	printf("reg[0x3F] = 0x%bx\n", comp_0x3F);
	printf("reg[0x40] = 0x%bx\n", comp_0x40);//setting[0]);
	printf("reg[0x41] = 0x%bx\n", comp_0x41);//setting[3]);
	printf("reg[0x42] = 0x%bx\n", comp_0x42);//setting[1]);
	printf("reg[0x43] = 0x%bx\n", comp_0x43);//setting[2]);

	cmdTbl[0] = 0x3F;
	cmdTbl[1] = comp_0x3F;
	cmdTbl[2] = 0x40;
	cmdTbl[3] = comp_0x40;
	cmdTbl[4] = 0x41;
	cmdTbl[5] = comp_0x41;
	cmdTbl[6] = 0x42;
	cmdTbl[7] = comp_0x42;
	cmdTbl[8] = 0x43;
	cmdTbl[9] = comp_0x43;
	i2cCmdTableSend(cmdTbl, 1, 10/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	addrTbl[0] = 0x05;
	addrTbl[1] = 0x06;
	addrTbl[2] = 0x07;
	addrTbl[3] = 0x08;
	i2cRegRead(addrTbl, dataTbl, 1, 4, 1, 0, 0);

	printf("reg[0x05] = 0x%bx\n", dataTbl[0]);//setting[0]);
	printf("reg[0x06] = 0x%bx\n", dataTbl[1]);//setting[3]);
	printf("reg[0x07] = 0x%bx\n", dataTbl[2]);//setting[1]);
	printf("reg[0x08] = 0x%bx\n", dataTbl[3]);//setting[2]);

	G_P_EN = comp_0x3F;
	G_P_B = comp_0x40;
	G_P_Gb = comp_0x42;
	G_P_Gr = comp_0x43;
	G_P_R = comp_0x41;

	return SUCCESS;
}

static UINT8
OV5610OBCaptureCalibrate(
	void
)		//Jim for OB cal
{
	#ifndef SEN_TAG_TODO
	UINT8 R_Value,Gr_Value,B_Value,Gb_Value;
	UINT8 R_Offset,B_Offset,Gr_Offset,Gb_Offset;
	UINT8 ChannelOKFlag = 0x00;
	UINT8 Rmid=0, Rminvalue =0, Rmaxvalue =0xff;
	UINT8 Grmid=0, Grminvalue =0, Grmaxvalue =0xff;
	UINT8 Bmid=0, Bminvalue =0, Bmaxvalue =0xff;
	UINT8 Gbmid=0, Gbminvalue =0, Gbmaxvalue =0xff;
	UINT16  range_high_r ,range_low_r ;
	UINT16  range_high_b ,range_low_b ;
	UINT16  range_high_gr ,range_low_gr ;
	UINT16  range_high_gb ,range_low_gb ;

	UINT8 BetRangeFlag=0;
	UINT8 Tempoffset_R =0,Tempoffset_B=0,Tempoffset_Gr=0,Tempoffset_Gb=0;
	UINT8 ROKFlag=0,BOKFlag=0,GrOKFlag=0,GbOKFlag=0;


	printf("\n ob calibration \n");
	#if 0
	range_high_r= 16;//8;//
	range_low_r= 14;//6;//
	range_high_b= 13;//8;//
	range_low_b= 11;//6;//
	range_high_gr=16;
	range_low_gr=14;
	range_high_gb=13;
	range_low_gb=11;
	#else
	range_high_r= 25;
	range_low_r= 23;
	range_high_b= 20;
	range_low_b= 18;
	range_high_gr=16;
	range_low_gr=14;
	range_high_gb=13;
	range_low_gb=11;
	#endif
	
    Rmid = Rmaxvalue;//(Rmaxvalue + Rminvalue)/2;
    Bmid = Bmaxvalue;//(Bmaxvalue + Bminvalue)/2;
    Grmid = Grmaxvalue;//(Grmaxvalue + Grminvalue)/2;
    Gbmid = Gbmaxvalue;//(Gbmaxvalue + Gbminvalue)/2;

	ob_R_set = R_Offset = Rmaxvalue;//Rmid;//0x20;//
	ob_B_set = B_Offset = Bmaxvalue;//Bmid;//0x00;//
	ob_Gr_set = Gr_Offset = Grmaxvalue;//Grmid;//0xff;//
	ob_Gb_set = Gb_Offset = Gbmaxvalue;//Gbmid; //0x00;//

	//sp1kSnapParamSet( SP1K_SNAP_PARAM_SKIP_PV, 0x01 );
	G_mode = 1;
	//sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );
	//sp1kCalibrationGammaSet(0);
	//sp1kCalibrationObSet(0);
	//sp1kCalibrationLscSet(0);
	//sp1kAeSetModeEn(AE_DISABLE);
	//sp1kAwbModeSet(SP1K_AWB_MODE_UNITY);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	
	while (1) {
		ob_R_set = R_Offset;
		ob_B_set = B_Offset;
		ob_Gr_set = Gr_Offset;
		ob_Gb_set = Gb_Offset;

		//sp1kSnapSizeSet( 2592, 1944 );
		//sp1kSnapParamSet( SP1K_SNAP_PARAM_DEBUG_RAW_ONLY, 1 );
		//sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

		//sp1kSnapTask();
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

		//printf("Jim : Cap OB Done!!! \n");
		printf("Jim :c1= %bu,c2=%bu,c3=%bu,c4=%bu\n", calibrationObData[0],calibrationObData[1],calibrationObData[2],calibrationObData[3]);
		//sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );

		#if 0// org
		R_Value = calibrationObData[0];
		Gr_Value = calibrationObData[1];
		B_Value = calibrationObData[2];
		Gb_Value = calibrationObData[3];
		#else
		R_Value = calibrationObData[2];//1];
		Gr_Value = calibrationObData[1];//0];
		B_Value = calibrationObData[3];
		Gb_Value = calibrationObData[0];//2];
		#endif

		printf(" B_Offset= %d,B_Value = %d \n",(SINT16)B_Offset,(SINT16)B_Value);
		printf(" Gb_Offset= %d,Gb_Value = %d \n",(SINT16)Gb_Offset,(SINT16)Gb_Value);
		printf(" R_Offset= %d,R_Value = %d \n",(SINT16)R_Offset,(SINT16)R_Value);
		printf(" Gr_Offset= %d,Gr_Value = %d \n",(SINT16)Gr_Offset,(SINT16)Gr_Value);

		#if 1
		if (BetRangeFlag == 0) {
			if ((R_Value<=range_high_r)&&(R_Value>=range_low_r)) {//((R_Value<=8)&&(R_Value>=6))//for frame exposure
				ChannelOKFlag |= 0x01;//R
			}
			else {
				ChannelOKFlag &= 0xfe;//R
			}

			if ((Gr_Value<=range_high_gr)&&(Gr_Value>=range_low_gr)) {
				ChannelOKFlag |=0x02;//Gr
			}
			else {
				ChannelOKFlag &=0xfd;//Gr
			}

			if ((B_Value<=range_high_b)&&(B_Value>=range_low_b)) {
				ChannelOKFlag |= 0x04;//B
			}
			else {
				ChannelOKFlag &= 0xfb;//B
			}

			if ((Gb_Value<=range_high_gb)&&(Gb_Value>=range_low_gb)) {
				ChannelOKFlag |=0x08;//Gb
			}
			else {
				ChannelOKFlag &=0xf7;//Gb
			}

			if ((ChannelOKFlag&0x01)&&(ChannelOKFlag&0x02)&&(ChannelOKFlag&0x04)&&(ChannelOKFlag&0x08)) {
				Tempoffset_R = R_Offset;
				Tempoffset_B = B_Offset;
				Tempoffset_Gr = Gr_Offset;
				Tempoffset_Gb = Gb_Offset;
				BetRangeFlag = 1;
			}
			else {
				BetRangeFlag = 0;
			}
		}
		//break;
		if (BetRangeFlag == 0) {
			if (!(ChannelOKFlag&0x01)) {
				if (R_Value>range_high_r) {
					Rmaxvalue=Rmid;
					Rmid = ((UINT16)(Rmaxvalue + Rminvalue))/2;

				}
				else if (R_Value<range_low_r) {
					Rminvalue=Rmid;
					Rmid = ((UINT16)(Rmaxvalue + Rminvalue))/2;

				}
			}
			if (!(ChannelOKFlag&0x02)) {
				if (Gr_Value>range_high_gr) {
					Grmaxvalue=Grmid;
					Grmid = ((UINT16)(Grmaxvalue + Grminvalue))/2;
				}
				else if (Gr_Value<range_low_gr) {
					Grminvalue=Grmid;
					Grmid = ((UINT16)(Grmaxvalue + Grminvalue))/2;
				}
			}
			if (!(ChannelOKFlag&0x04)) {
				if (B_Value>range_high_b)
				{
					Bmaxvalue=Bmid;
					Bmid = ((UINT16)(Bmaxvalue + Bminvalue))/2;

				}
				else if (B_Value<range_low_b) {
					Bminvalue=Bmid;

					Bmid = ((UINT16)(Bmaxvalue + Bminvalue))/2;
				}
			}
			if (!(ChannelOKFlag&0x08)) {
				if (Gb_Value>range_high_gb) {
					Gbmaxvalue=Gbmid;
					Gbmid = ((UINT16)(Gbmaxvalue + Gbminvalue))/2;
				}
				else if (Gb_Value<range_low_gb) {
					Gbminvalue=Gbmid;
					Gbmid = ((UINT16)(Gbmaxvalue + Gbminvalue))/2;
				}
			}
			R_Offset = Rmid;
			B_Offset = Bmid;
			Gr_Offset = Grmid;
			Gb_Offset = Gbmid;
		}
		else {
			if (ROKFlag==0) {
				if (R_Value>=range_low_r) {
					Tempoffset_R = R_Offset;
					R_Offset --;
				}
				else if (R_Value<range_low_r) {
					R_Offset = Tempoffset_R;
					ROKFlag = 1;
				}
			}
			if (BOKFlag==0) {
				if (B_Value>=range_low_b) {
					Tempoffset_B = B_Offset;
					B_Offset --;
				}
				else if (B_Value<range_low_b) {
					B_Offset = Tempoffset_B;
					BOKFlag = 1;
				}
			}
			if (GrOKFlag==0) {
				if (Gr_Value>=range_low_gr) {
					Tempoffset_Gr = Gr_Offset;
					Gr_Offset --;
				}
				else if (Gr_Value<range_low_gr) {
					Gr_Offset = Tempoffset_Gr;
					GrOKFlag = 1;
				}
			}
			if (GbOKFlag==0) {
				if (Gb_Value>=range_low_gb) {
					Tempoffset_Gb = Gb_Offset;
					Gb_Offset --;
				}
				else if (Gb_Value<range_low_gb) {
					Gb_Offset = Tempoffset_Gb;
					GbOKFlag = 1;
				}
			}
			if ((ROKFlag==1)&&(BOKFlag==1)&&(GrOKFlag==1)&&(GbOKFlag==1)) {
				break;
			}
		}
		#endif
	}

	printf("reg[0x3F] = 0x%bu\n", 0x0F);
	printf("reg[0x40] = 0x%bu\n", B_Offset);
	printf("reg[0x41] = 0x%bu\n", R_Offset);
	printf("reg[0x42] = 0x%bu\n", Gb_Offset);
	printf("reg[0x43] = 0x%bu\n", Gr_Offset);

	printf("B_Value = 0x%bu\n", B_Value);
	printf("Gb_Value = 0x%bu\n", Gb_Value);
	printf("R_Value = 0x%bu\n", R_Value);
	printf("Gr_Value = 0x%bu\n", Gr_Value);

	G_C_EN = 0x0F;
	G_C_B =  B_Offset;
	G_C_Gb = Gb_Offset;
	G_C_Gr = Gr_Offset;
	G_C_R =  R_Offset;
	#endif
	return SUCCESS;
}

static UINT8
OV5610OBSnapFrontFrame(
	UINT8 SnapNum,
	UINT8 R,
	UINT8 B,
	UINT8 Gr,
	UINT8 Gb
)
{
	UINT8 cmdTbl[2*5];
    UINT8 sts;

	if (SnapNum > 16) {
		return 1;
	}

	XBYTE[REG_Cdsp_wboffseten] = 0x10;
	#if 0
	#ifndef TDC_5080
	sp1kSHT_BSet(1);		  //SHT Close
	sp1kSHT_BCfg();
	sp1kSHT_ASet(1);
	sp1kSHT_ACfg();
	globalTimerWait(10);
	sp1kSHT_ASet(0);
	sp1kSHT_ACfg();
	#else
	frontOpShutterClose();
	#endif
	#endif

	i2cCmdTableSend(frontObSnap0CmdTbl, 1, sizeof(frontObSnap0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cmdTbl[0] = 0x3F;
	cmdTbl[1] = 0x0F;
	cmdTbl[2] = 0x40;
	cmdTbl[3] = B;
	cmdTbl[4] = 0x42;
	cmdTbl[5] = R;
	cmdTbl[6] = 0x43;
	cmdTbl[7] = Gb;
	cmdTbl[8] = 0x41;
	cmdTbl[9] = Gr;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	i2cCmdTableSend(frontObSnap1CmdTbl, 1, sizeof(frontObSnap1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	ENTER_CRITICAL(sts);
	cdsp1stPathWriteSkip( 6, 1 );

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 2);
/*
	sp1kFrexSet(1);			  //FREX ON
	sp1kFrexCfg();

	sp1kExpstbSet(1);		  //EXPSTB ON
	sp1kExpstbCfg();

	HAL_Front10UsWait(18);

	sp1kExpstbSet(0);		  //EXPSTB OFF
	sp1kExpstbCfg();
*/
	HAL_Front10UsWait(50000L);

	#if 0
	sp1kSHT_BSet(1);		  //SHT Close
	sp1kSHT_BCfg();
	sp1kSHT_ASet(1);
	sp1kSHT_ACfg();
	globalTimerWait(10);
	sp1kSHT_ASet(0);
	sp1kSHT_ACfg();

	globalTimerWait(10);
	#endif

	//sp1kFrexSet(0);			   //FREX OFF
	//sp1kFrexCfg();

	frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 1);
	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);
	ENTER_CRITICAL(sts);

	i2cCmdTableSend(frontObSnap2CmdTbl, 1, sizeof(frontObSnap2CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
}

static void
OV5610OBFrameSnap(
	void
)
{
	OV5610OBSnapFrontFrame(1, ob_R_set, ob_B_set, ob_Gr_set, ob_Gb_set);
}

static void
OV5610FlashWBSet(
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

#ifdef MSHUTTER_CALIBRATION  // used for mechanical shutter calibration
void
OV5610MShutterCalibraion(
	void
)
{
#if 0
	UINT16 cur_Y = 0;
	UINT16 high_msh_delay = 8192;
	UINT16 low_msh_delay = 0;
	UINT8 i;
	static code UINT8 cal_Y_target[2][30] = {
		{244,227,212,198,185,172,161,150,140,131,122,114,106,99,92,86,80,75,70,65,61,57,53,49,46,43,40,37,35,33},
		{234,218,204,190,177,166,154,144,134,125,117,109,102,95,89,83,77,72,67,63,59,55,51,48,44,41,39,36,34,31}
	};
	UINT16 cal_Y_max, cal_Y_min;
	
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SKIP_PV, 0x01 );
	G_mode = 2;
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );
	XBYTE[REG_Cdsp_Lutgammaen] = 0x00;
	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbModeSet(SP1K_AWB_MODE_DISABLE);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	
	for(i= 0; i< 30;i++) {
		high_msh_delay = 8192;
		low_msh_delay = 0;
		cal_Y_max = cal_Y_target[0][i];
		cal_Y_min = cal_Y_target[1][i];
	
		while (1) {
			sp1kSnapSizeSet( 2592, 1944 );
			sp1kSnapParamSet( SP1K_SNAP_PARAM_DEBUG_RAW_ONLY, 1 );
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
			caliMshtterDelay = (low_msh_delay + high_msh_delay)/2;
			sp1kSnapTask();
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	
			//printf("cs : Cap OB Done!!! \n");
			//printf("Jim :c1= %bu,c2=%bu,c3=%bu,c4=%bu\n", calibrationObData[0],calibrationObData[1],calibrationObData[2],calibrationObData[3]);
			//printf("CS : caliMshtterDelay = %d \n", caliMshtterDelay);
			//sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
			
			cur_Y = calibrationObData[1] - 7  + calibrationObData[3] - 6;
			
			//printf("CS : cur_Y = %d", cur_Y);
			if(cur_Y > cal_Y_max) {
				low_msh_delay = caliMshtterDelay;
				if(low_msh_delay == 8191) break;
			}
			else if(cur_Y < cal_Y_min) {
				high_msh_delay = caliMshtterDelay;
				if(high_msh_delay == 0) break;
			}
			else {
				break;
			}
		}
		
		printf("CS : %bdth MSH calibration Y = %d, mshutter_delay = %d, tar_H = %d, tar_L =%d\n",i,cur_Y, caliMshtterDelay,cal_Y_max,cal_Y_min);
	}
#endif	
}
#endif
