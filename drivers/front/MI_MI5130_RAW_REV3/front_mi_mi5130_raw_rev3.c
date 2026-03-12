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
#include "sp1k_util_api.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define PREVIEW_1280

/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#ifndef PREVIEW_1280
  #define WIDTH_30FPS_PREV      636
  #define HEIGHT_30FPS_PREV     476
#else
  #define WIDTH_30FPS_PREV      1280
  #define HEIGHT_30FPS_PREV     960
#endif

#ifndef PREVIEW_1280
  #define XOFF_30FPS_PREV       4
  #define YOFF_30FPS_PREV       2//4
  #define LINE_TOT_30FPS_PREV   1470
  #define FRAME_TOT_30FPS_PREV  686
#else
  #define XOFF_30FPS_PREV 	  4
  #define YOFF_30FPS_PREV 	  2//4
  #define LINE_TOT_30FPS_PREV   3791
  #define FRAME_TOT_30FPS_PREV  1055
#endif

#define WIDTH_SNAP      2592 
#define HEIGHT_SNAP     1944 
#define XOFF_SNAP       4
#define YOFF_SNAP       2
#define LINE_TOT_SNAP   5214//2352//2100
#define FRAME_TOT_SNAP  2146//1552

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    12
#define MCLK_PHASE_30FPS_PREV  2
#define MCLK_DRV_30FPS_PREV    1
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  9

#define MCLK_DIV_SNAP    12
#define MCLK_PHASE_SNAP  0
#define MCLK_DRV_SNAP    1
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  30000000L
#if 0
#define PCLK_FREQ_60FPS_PREV  6000000L
#endif
#define PCLK_FREQ_SNAP        30000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV  96000000L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP        96000000L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  1
#define RESHAPE_HRISE_30FPS_PREV  2
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  1
#define RESHAPE_VRISE_30FPS_PREV  2

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
#define BAYER_PTN_PREV  0x02
#define BAYER_PTN_SNAP  0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0x20
#define I2C_CLK_DIV   0x01//0x03

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  122
#define AE_30FPS_30_MIN_IDX  1
#define AE_30FPS_25_MAX_IDX  122
#define AE_30FPS_25_MIN_IDX  1
#define EV10_30FPS_EXP_IDX   90
/* gain. */
#define GAIN_30FPS_MAX_IDX  223
#define GAIN_30FPS_MIN_IDX  1
#if 0
#define GAIN_60FPS_MAX_IDX  51
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

xdata UINT8   showgidflag;



/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - MI MI5130 RAW";


static code frontAe_t ae30fps50hzTbl[123] = {
		{0,	 334,	 215,	 0	},
		{1,	 334,	 207,	 0	},
		{2,	 334,	 200,	 0	},
		{3,	 334,	 193,	 0	},
		{4,	 333,	 186,	 0	},
		{5,	 333,	 180,	 0	},
		{6,	 334,	 175,	 0	},
		{7,	 333,	 169,	 0	},
		{8,	 334,	 165,	 0	},
		{9,	 334,	 160,	 0	},
		{10,	 334,	 151,	 0	},
		{11,	 334,	 143,	 0	},
		{12,	 334,	 136,	 0	},
		{13,	 334,	 129,	 0	},
		{14,	 333,	 122,	 0	},
		{15,	 333,	 116,	 0	},
		{16,	 334,	 111,	 0	},
		{17,	 333,	 105,	 0	},
		{18,	 334,	 101,	 0	},
		{19,	 334,	 96,	 0	},
		{20,	 334,	 87,	 0	},
		{21,	 334,	 79,	 0	},
		{22,	 334,	 72,	 0	},
		{23,	 334,	 65,	 0	},
		{24,	 333,	 58,	 0	},
		{25,	 333,	 52,	 0	},
		{26,	 334,	 47,	 0	},
		{27,	 333,	 41,	 0	},
		{28,	 334,	 37,	 0	},
		{29,	 334,	 32,	 0	},
		{30,	 334,	 28,	 0	},
		{31,	 334,	 24,	 0	},
		{32,	 334,	 20,	 0	},
		{33,	 334,	 17,	 0	},
		{34,	 333,	 13,	 0	},
		{35,	 333,	 10,	 0	},
		{36,	 334,	 8,	 0	},
		{37,	 334,	 5,	 0	},
		{38,	 334,	 3,	 0	},
		{39,	 334,	 0,	 0	},
		{40,	 502,	 13,	 0	},
		{41,	 502,	 10,	 0	},
		{42,	 502,	 7,	 0	},
		{43,	 502,	 5,	 0	},
		{44,	 502,	 2,	 0	},
		{45,	 502,	 0,	 0	},
		{46,	 1000,	 27,	 0	},
		{47,	 1000,	 23,	 0	},
		{48,	 995,	 19,	 0	},
		{49,	 1004,	 16,	 0	},
		{50,	 1009,	 13,	 0	},
		{51,	 1009,	 10,	 0	},
		{52,	 1004,	 7,	 0	},
		{53,	 995,	 4,	 0	},
		{54,	 1004,	 2,	 0	},
		{55,	 1014,	 0,	 0	},
		{56,	 1084,	 0,	 0	},
		{57,	 1164,	 0,	 0	},
		{58,	 1249,	 0,	 0	},
		{59,	 1339,	 0,	 0	},
		{60,	 1433,	 0,	 0	},
		{61,	 1529,	 0,	 0	},
		{62,	 1639,	 0,	 0	},
		{63,	 1767,	 0,	 0	},
		{64,	 1880,	 0,	 0	},
		{65,	 2029,	 0,	 0	},
		{66,	 2157,	 0,	 0	},
		{67,	 2329,	 0,	 0	},
		{68,	 2499,	 0,	 0	},
		{69,	 2661,	 0,	 0	},
		{70,	 2846,	 0,	 0	},
		{71,	 3058,	 0,	 0	},
		{72,	 3253,	 0,	 0	},
		{73,	 3533,	 0,	 0	},
		{74,	 3726,	 0,	 0	},
		{75,	 4018,	 0,	 0	},
		{76,	 4269,	 0,	 0	},
		{77,	 4657,	 0,	 0	},
		{78,	 4998,	 0,	 0	},
		{79,	 5254,	 0,	 0	},
		{80,	 5692,	 0,	 0	},
		{81,	 6027,	 0,	 0	},
		{82,	 6404,	 0,	 0	},
		{83,	 7066,	 0,	 0	},
		{84,	 7318,	 0,	 0	},
		{85,	 7881,	 0,	 0	},
		{86,	 8538,	 0,	 0	},
		{87,	 9314,	 0,	 0	},
		{88,	 9758,	 0,	 0	},
		{89,	 10246,	 0,	 0	},
		{90,	 11384,	 0,	 0	},
		{91,	 12054,	 0,	 0	},
		{92,	 12807,	 0,	 0	},
		{93,	 13661,	 0,	 0	},
		{94,	 14637,	 0,	 0	},
		{95,	 15763,	 0,	 0	},
		{96,	 17076,	 0,	 0	},
		{97,	 18629,	 0,	 0	},
		{98,	 20492,	 1,	 0	},
		{99,	 20492,	 0,	 0	},
		{100,	 22768,	 0,	 0	},
		{101,	 25614,	 2,	 0	},
		{102,	 25614,	 0,	 0	},
		{103,	 29274,	 2,	 0	},
		{104,	 29274,	 0,	 0	},
		{105,	 34152,	 2,	 0	},
		{106,	 34152,	 0,	 0	},
		{107,	 40983,	 4,	 0	},
		{108,	 40983,	 1,	 0	},
		{109,	 40983,	 0,	 0	},
		{110,	 51228,	 4,	 0	},
		{111,	 51228,	 2,	 0	},
		{112,	 51228,	 0,	 0	},
		{113,	 68304,	 7,	 0	},
		{114,	 68304,	 5,	 0	},
		{115,	 68304,	 2,	 0	},
		{116,	 68304,	 0,	 0	},
		{117,	 102454,	 12,	 0	},
		{118,	 102454,	 9,	 0	},
		{119,	 102454,	 7,	 0	},
		{120,	 102454,	 4,	 0	},
		{121,	 102454,	 2,	 0	},
		{122,	 102454,	 0,	 0	},
	};


static code frontAe_t ae30fps60hzTbl[125] = {
		{0,	 300,	 219,	 0	},
		{1,	 300,	 211,	 0	},
		{2,	 300,	 203,	 0	},
		{3,	 300,	 196,	 0	},
		{4,	 300,	 189,	 0	},
		{5,	 300,	 183,	 0	},
		{6,	 300,	 177,	 0	},
		{7,	 300,	 172,	 0	},
		{8,	 300,	 167,	 0	},
		{9,	 300,	 162,	 0	},
		{10,	 300,	 155,	 0	},
		{11,	 300,	 147,	 0	},
		{12,	 300,	 139,	 0	},
		{13,	 300,	 132,	 0	},
		{14,	 300,	 125,	 0	},
		{15,	 300,	 119,	 0	},
		{16,	 300,	 113,	 0	},
		{17,	 300,	 108,	 0	},
		{18,	 300,	 103,	 0	},
		{19,	 300,	 98,	 0	},
		{20,	 300,	 91,	 0	},
		{21,	 300,	 83,	 0	},
		{22,	 300,	 75,	 0	},
		{23,	 300,	 68,	 0	},
		{24,	 300,	 61,	 0	},
		{25,	 300,	 55,	 0	},
		{26,	 300,	 49,	 0	},
		{27,	 300,	 44,	 0	},
		{28,	 300,	 39,	 0	},
		{29,	 300,	 34,	 0	},
		{30,	 300,	 30,	 0	},
		{31,	 300,	 26,	 0	},
		{32,	 300,	 22,	 0	},
		{33,	 300,	 18,	 0	},
		{34,	 300,	 15,	 0	},
		{35,	 300,	 12,	 0	},
		{36,	 300,	 9,	 0	},
		{37,	 300,	 6,	 0	},
		{38,	 300,	 4,	 0	},
		{39,	 300,	 1,	 0	},
		{40,	 300,	 0,	 0	},
		{41,	 401,	 7,	 0	},
		{42,	 401,	 4,	 0	},
		{43,	 401,	 2,	 0	},
		{44,	 401,	 0,	 0	},
		{45,	 603,	 12,	 0	},
		{46,	 603,	 9,	 0	},
		{47,	 603,	 6,	 0	},
		{48,	 603,	 4,	 0	},
		{49,	 601,	 1,	 0	},
		{50,	 603,	 0,	 0	},
		{51,	 1191,	 25,	 0	},
		{52,	 1205,	 22,	 0	},
		{53,	 1198,	 18,	 0	},
		{54,	 1205,	 15,	 0	},
		{55,	 1213,	 12,	 0	},
		{56,	 1213,	 9,	 0	},
		{57,	 1205,	 6,	 0	},
		{58,	 1220,	 4,	 0	},
		{59,	 1198,	 1,	 0	},
		{60,	 1220,	 0,	 0	},
		{61,	 1339,	 0,	 0	},
		{62,	 1433,	 0,	 0	},
		{63,	 1529,	 0,	 0	},
		{64,	 1639,	 0,	 0	},
		{65,	 1767,	 0,	 0	},
		{66,	 1880,	 0,	 0	},
		{67,	 2029,	 0,	 0	},
		{68,	 2157,	 0,	 0	},
		{69,	 2329,	 0,	 0	},
		{70,	 2499,	 0,	 0	},
		{71,	 2661,	 0,	 0	},
		{72,	 2846,	 0,	 0	},
		{73,	 3058,	 0,	 0	},
		{74,	 3253,	 0,	 0	},
		{75,	 3533,	 0,	 0	},
		{76,	 3726,	 0,	 0	},
		{77,	 4018,	 0,	 0	},
		{78,	 4269,	 0,	 0	},
		{79,	 4657,	 0,	 0	},
		{80,	 4998,	 0,	 0	},
		{81,	 5254,	 0,	 0	},
		{82,	 5692,	 0,	 0	},
		{83,	 6027,	 0,	 0	},
		{84,	 6404,	 0,	 0	},
		{85,	 7066,	 0,	 0	},
		{86,	 7318,	 0,	 0	},
		{87,	 7881,	 0,	 0	},
		{88,	 8538,	 0,	 0	},
		{89,	 9314,	 0,	 0	},
		{90,	 9758,	 0,	 0	},
		{91,	 10246,	 0,	 0	},
		{92,	 11384,	 0,	 0	},
		{93,	 12054,	 0,	 0	},
		{94,	 12807,	 0,	 0	},
		{95,	 13661,	 0,	 0	},
		{96,	 14637,	 0,	 0	},
		{97,	 15763,	 0,	 0	},
		{98,	 17076,	 0,	 0	},
		{99,	 18629,	 0,	 0	},
		{100,	 20492,	 1,	 0	},
		{101,	 20492,	 0,	 0	},
		{102,	 22768,	 0,	 0	},
		{103,	 25614,	 2,	 0	},
		{104,	 25614,	 0,	 0	},
		{105,	 29274,	 2,	 0	},
		{106,	 29274,	 0,	 0	},
		{107,	 34152,	 2,	 0	},
		{108,	 34152,	 0,	 0	},
		{109,	 40983,	 4,	 0	},
		{110,	 40983,	 1,	 0	},
		{111,	 40983,	 0,	 0	},
		{112,	 51228,	 4,	 0	},
		{113,	 51228,	 2,	 0	},
		{114,	 51228,	 0,	 0	},
		{115,	 68304,	 7,	 0	},
		{116,	 68304,	 5,	 0	},
		{117,	 68304,	 2,	 0	},
		{118,	 68304,	 0,	 0	},
		{119,	 102454,	 12,	 0	},
		{120,	 102454,	 9,	 0	},
		{121,	 102454,	 7,	 0	},
		{122,	 102454,	 4,	 0	},
		{123,	 102454,	 2,	 0	},
		{124,	 102454,	 0,	 0	},
	};



static code UINT16 gain30fpsTbl[224] = {
	544, 	545, 	546, 	547, 	548, 	549, 	550, 	551, 	552, 	553, 
	554, 	555, 	556, 	557, 	558, 	559, 	560, 	561, 	562, 	563, 
	564, 	565, 	566, 	567, 	568, 	569, 	570, 	571, 	572, 	573, 
	574, 	575, 	576, 	577, 	578, 	579, 	580, 	581, 	582, 	583, 
	584, 	585, 	586, 	587, 	588, 	589, 	590, 	591, 	592, 	593, 
	594, 	595, 	596, 	597, 	598, 	599, 	600, 	601, 	602, 	603, 
	604, 	605, 	606, 	607, 	608, 	609, 	610, 	611, 	612, 	613, 
	614, 	615, 	616, 	617, 	618, 	619, 	620, 	621, 	622, 	623, 
	624, 	625, 	626, 	627, 	628, 	629, 	630, 	631, 	632, 	633, 
	634, 	635, 	636, 	637, 	638, 	703, 	704, 	705, 	706, 	707, 
	708, 	709, 	710, 	711, 	712, 	713, 	714, 	715, 	716, 	717, 
	718, 	719, 	720, 	721, 	722, 	723, 	724, 	725, 	726, 	727, 
	728, 	729, 	730, 	731, 	732, 	733, 	734, 	735, 	736, 	737, 
	738, 	739, 	740, 	741, 	742, 	743, 	744, 	745, 	746, 	747, 
	748, 	749, 	750, 	751, 	752, 	753, 	754, 	755, 	756, 	757, 
	758, 	759, 	760, 	761, 	762, 	763, 	764, 	765, 	766, 	767, 
	960, 	961, 	962, 	963, 	964, 	965, 	966, 	967, 	968, 	969, 
	970, 	971, 	972, 	973, 	974, 	975, 	976, 	977, 	978, 	979, 
	980, 	981, 	982, 	983, 	984, 	985, 	986, 	987, 	988, 	989, 
	990, 	991, 	992, 	993, 	994, 	995, 	996, 	997, 	998, 	999, 
	1000, 	1001, 	1002, 	1003, 	1004, 	1005, 	1006, 	1007, 	1008, 	1009, 
	1010, 	1011, 	1012, 	1013, 	1014, 	1015, 	1016, 	1017, 	1018, 	1019, 
	1020, 	1021, 	1022, 	1023, 	};



/* exposure time index table. */
static code UINT8 ae30fpsMaxIdxTbl[2] = {
	AE_30FPS_30_MAX_IDX,
	AE_30FPS_25_MAX_IDX,
};

static code UINT8 ae30fpsMinIdxTbl[2] = {
	AE_30FPS_30_MIN_IDX,
	AE_30FPS_25_MIN_IDX,
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
0x01,0x03,0x00,0x01, 
};


static code UINT8 frontInit1CmdTbl[] = {

0x30,0x1A,0x10,0xCC,
0x01,0x12,0x0A,0x0A,
0x30,0x64,0x08,0x05,
0x31,0xAE,0x02,0x01,
//0x01,0x00,0x00,0x00,

0x03,0x00,0x00,0x08,  //PLL Setting
0x03,0x02,0x00,0x01,
0x03,0x04,0x00,0x02,
 
//0x03,0x06,0x00,0x28,    // Pixel clock 30Mhz
 0x03,0x06,0x00,0x20,    // Pixel clock 24Mhz
//0x03,0x06,0x00,0x14,  // Pixel clock 15Mhz 
//0x03,0x06,0x00,0x40,  // Pixel clock 48Mhz 


0x03,0x08,0x00,0x08,
0x03,0x0A,0x00,0x02,



0x30,0x92,0x0a,0x52,//0x53,//power comsumption---Brian
0x30,0x94,0x46,0x56,
0x30,0x96,0x56,0x52,



#if 0

0x30,0x88,0x6e,0x61,//zouchd add micron optimized power setting
0x30,0x8e,0xd6,0x6a,
0x30,0x90,0x56,0x6a,
0x30,0xca,0x80,0x06,
0x31,0x2a,0xdd,0x02,
//0x31,0x2c,0x00,0xe4,
0x31,0x70,0x29,0x9a,

#endif


//0x30,0x5e,0x03,0xf7,    // gloabl gain

                          // Lens Correction parameters
0x36,0x0A,0x01,0x30,
0x36,0x0C,0x7D,0x2D,
0x36,0x0E,0x71,0x31,
0x36,0x10,0x21,0xAD,
0x36,0x12,0xCA,0x51,
0x36,0x4A,0xC2,0x2C,
0x36,0x4C,0xE0,0x08,
0x36,0x4E,0xCD,0x2E,
0x36,0x50,0xB1,0x0E,
0x36,0x52,0x6F,0x0F,
0x36,0x8A,0x1A,0x72,
0x36,0x8C,0x6A,0x0F,
0x36,0x8E,0xDE,0xD3,
0x36,0x90,0xAE,0x50,
0x36,0x92,0x08,0x74,
0x36,0xCA,0x91,0x4F,
0x36,0xCC,0xDB,0xAD,
0x36,0xCE,0x73,0x71,
0x36,0xD0,0x2F,0xB1,
0x36,0xD2,0xEE,0x72,
0x37,0x0A,0xDB,0xF2,
0x37,0x0C,0xD6,0xD1,
0x37,0x0E,0x5C,0x93,
0x37,0x10,0x59,0xD3,
0x37,0x12,0x65,0x34,
0x36,0x00,0x03,0x30,
0x36,0x02,0xC3,0x0D,
0x36,0x04,0x7B,0xF1,
0x36,0x06,0x3A,0xAC,
0x36,0x08,0x8A,0x92,
0x36,0x40,0xD7,0x0C,
0x36,0x42,0x0C,0x6B,
0x36,0x44,0xA8,0x2F,
0x36,0x46,0xAF,0x2E,
0x36,0x48,0x51,0xD0,
0x36,0x80,0x76,0xF1,
0x36,0x82,0x75,0xAE,
0x36,0x84,0xDD,0x53,
0x36,0x86,0x49,0x10,
0x36,0x88,0x0C,0xB4,
0x36,0xC0,0xEF,0x0E,
0x36,0xC2,0x86,0x2E,
0x36,0xC4,0x14,0xD2,
0x36,0xC6,0x48,0x4F,
0x36,0xC8,0xAD,0x33,
0x37,0x00,0xC7,0xD2,
0x37,0x02,0x14,0xCC,
0x37,0x04,0x3D,0xF3,
0x37,0x06,0x43,0xB1,
0x37,0x08,0x54,0x14,
0x36,0x14,0x01,0x70,
0x36,0x16,0xFE,0xEC,
0x36,0x18,0x3A,0x11,
0x36,0x1A,0x8D,0xCD,
0x36,0x1C,0xA1,0xD1,
0x36,0x54,0xF3,0xAA,
0x36,0x56,0x05,0x2B,
0x36,0x58,0x82,0x0E,
0x36,0x5A,0xF2,0x8E,
0x36,0x5C,0x50,0x2F,
0x36,0x94,0x5F,0xB1,
0x36,0x96,0x7D,0xAE,
0x36,0x98,0xCB,0xD3,
0x36,0x9A,0x43,0xD0,
0x36,0x9C,0x0C,0x34,
0x36,0xD4,0x9E,0xAD,
0x36,0xD6,0xEE,0x6E,
0x36,0xD8,0x4A,0xF1,
0x36,0xDA,0x67,0x51,
0x36,0xDC,0xEC,0xB2,
0x37,0x14,0xB7,0x92,
0x37,0x16,0x9E,0x11,
0x37,0x18,0x32,0x94,
0x37,0x1A,0x2B,0xF2,
0x37,0x1C,0x4C,0x13,
0x36,0x1E,0x01,0x50,
0x36,0x20,0x32,0x4D,
0x36,0x22,0x64,0x51,
0x36,0x24,0x07,0xEC,
0x36,0x26,0xDE,0xB1,
0x36,0x5E,0xE0,0xEA,
0x36,0x60,0xA0,0xE6,
0x36,0x62,0x9C,0x4D,
0x36,0x64,0x92,0x0E,
0x36,0x66,0x59,0x6F,
0x36,0x9E,0x0A,0xF2,
0x36,0xA0,0x17,0xAF,
0x36,0xA2,0xE2,0x13,
0x36,0xA4,0x80,0x4F,
0x36,0xA6,0x6F,0xF3,
0x36,0xDE,0xD4,0xEC,
0x36,0xE0,0x86,0xEE,
0x36,0xE2,0x13,0x51,
0x36,0xE4,0x00,0x70,
0x36,0xE6,0x89,0xF3,
0x37,0x1E,0xDD,0x12,
0x37,0x20,0x85,0xB0,
0x37,0x22,0x4C,0x53,
0x37,0x24,0x7F,0xD2,
0x37,0x26,0x01,0xB5,
0x37,0x82,0x05,0x30,
0x37,0x84,0x03,0xC8,

0x37,0x80,0x80,0x00,    // will:turn on Lens Correction

};

#ifndef  PREVIEW_1280
static code UINT8 front30fpsPrevCmdTbl[] = {    //640 x 480

                    
///////////////////////////////
// 640x480_skip4x-Bin2x	     //
///////////////////////////////

0x30,0x64,0x08,0x05,
0x01,0x04,0x00,0x01,
//0x03,0x4C,0x02,0x80,   // H Size   640 
//0x03,0x4E,0x01,0xE0,   // V Size   480 
0x03,0x4C,0x02,0x82,     // H Size   642
0x03,0x4E,0x01,0xE4,     // V Size   484 

0x03,0x44,0x00,0x08,
0x03,0x46,0x00,0x08,
0x03,0x48,0x0A,0x01,
0x03,0x4A,0x07,0x81,
0x30,0x40,0x14,0xFC,     // will: x_y_binning and summing
0x30,0x12,0x00,0x29,
0x30,0x10,0x01,0x5C,
0x30,0x14,0x07,0x02,

0x03,0x40,0x02,0xae,     // Frame Total  686
0x03,0x42,0x05,0xbe,     // Length Total  1470 

0x01,0x04,0x00,0x00,


#if 0
///////////////////////////////
// view Finder Setting       //
///////////////////////////////
0x01,0x04,0x00,0x01,
0x03,0x82,0x00,0x07,
0x03,0x86,0x00,0x07,
0x30,0x40,0x14,0xfc,
/*0x30,0x40,0x14,0xfc,
0x30,0x40,0x14,0xfc,
0x30,0x6e,0x90,0x80,*/
0x03,0x44,0x00,0x08,
0x03,0x46,0x00,0x08,
0x03,0x48,0x0A,0x21,
0x03,0x4A,0x07,0x99,

0x03,0x4C,0x02,0x80,
0x03,0x4E,0x01,0xE0,
0x04,0x00,0x00,0x00,
0x04,0x04,0x00,0x10,
0x01,0x04,0x00,0x00,

///////////////////////////////
// original Setting          //
///////////////////////////////
0x01,0x04,0x00,0x01,
0x03,0x44,0x00,0x18,
0x03,0x48,0x0A,0x11,
0x03,0x46,0x00,0x09,//0x14,
0x03,0x4A,0x07,0x99,//0x8D,
0x30,0x40,0x14,0xFC,  //binning and summing turn on
//0x30,0x40,0x14,0xFC,  //binning is off

0x03,0x4C,0x02,0x80,
0x03,0x4E,0x01,0xe4,//0xE0,
//0x03,0x42,0x05,0xBE,//0x0B,0x60,//
0x03,0x42,0x0b,0x60,

0x03,0x40,0x02,0xAE,//
0x02,0x02,0x02,0xA0,//0x07,0xEC,
0x30,0x14,0x07,0x02,//
0x30,0x10,0x01,0x5C,
0x01,0x04,0x00,0x00,
#endif

};

#else

static code UINT8 front30fpsPrevCmdTbl[] = {  // 1280x960

0x30,0x64,0x08,0x05,   
0x01,0x04,0x00,0x01,   
0x03,0x4C,0x05,0x10,   //HSize  1296  
0x03,0x4E,0x03,0xCC,   //VSize  976
0x03,0x44,0x00,0x08,   
0x03,0x46,0x00,0x08,   
0x03,0x48,0x0A,0x25,   
0x03,0x4A,0x07,0x9D,   
0x30,0x40,0x14,0x6C,   // Read Mode
0x30,0x12,0x00,0x2F,   
0x30,0x10,0x01,0x5C,   
0x30,0x14,0x09,0xAF,   
0x03,0x40,0x04,0x1F,   // Frame Lines 1055
0x03,0x42,0x0E,0xCF,   // Line Lentth 3791
0x01,0x04,0x00,0x00,   
};
#endif

static code UINT8 frontSnapCmdTbl[] = {

//0x01,0x00,0x00,0x01,                       
0x01,0x04,0x00,0x01,

#if 1
0x03,0x44,0x00,0x08,
0x03,0x48,0x0A,0x27,//0x2f,
0x03,0x46,0x00,0x06,//0x08,
0x03,0x4A,0x07,0xa2,//0x9F,
0x03,0x4C,0x0A,0x20,
0x03,0x4E,0x07,0x9c,//0x98,
#else
0x03,0x44,0x00,0x06,
0x03,0x48,0x0A,0x29,//0x2f,
0x03,0x46,0x00,0x06,//0x08,
0x03,0x4A,0x07,0xa1,//0x9F,
0x03,0x4C,0x0A,0x24,
0x03,0x4E,0x07,0x9c,//0x98,

#endif

0x30,0x40,0x00,0x24,    // will: x_y_binning and summing   
0x03,0x42,0x14,0x5E,    // Line total 5214
0x03,0x40,0x08,0x62,    // Frame total 2146
//0x03,0x42,0x0f,0x66,    // Line total 3942
//0x03,0x40,0x07,0xed,    // Frame total 2029

0x02,0x02,0x07,0xEC,
0x30,0x14,0x03,0x72,
0x30,0x10,0x00,0x9C,
0x01,0x04,0x00,0x00,

};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
//extern UINT8 HAL_CdspEofWait(UINT8 Number);
extern void cdspColorMatrixRestore(void);
//void wr_byt_i2c(UINT8 i2caddr,UINT8 regaddr_hi,UINT8 regaddr_low,UINT8 dat_hi,UINT8 dat_low);

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
#if 0
static void
frontAllReg16Dump(
	UINT8 dataLen
)
{
	UINT16 addrTbl[1];
	UINT8 dataTbl[1*8];
	UINT16 regIdx;
	UINT8 dataIdx;


	if ( dataLen == 0 || dataLen >= 8 ) {
		printf("data len err\n");
		return;
	}

	for ( regIdx = 0x0000; regIdx < 0x0001; regIdx++ ) {
		addrTbl[0] = regIdx;
		i2cReg16Read(addrTbl, dataTbl, dataLen, 1, 1, 0, 0);
		printf("reg[0x%04X] =", regIdx);
		for ( dataIdx = 0 ; dataIdx < dataLen; dataIdx++ ) {
			printf(" 0x%02bX", dataTbl[dataIdx]);
		}
		printf("\n");
	}
}
#endif


void mi5130registorset(UINT16 regaddr, UINT16 regval)
{
	UINT8 cmdTbl[4*1];

	cmdTbl[0] =( regaddr>>8); 
	cmdTbl[1] =(regaddr & 0xff);
	cmdTbl[2] = ( regval>>8);
	cmdTbl[3] = (regval & 0xff);                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
}

void mi5130showgidset(UINT8 show )
{
	showgidflag = show;
}


__STATIC void
frontOpPowerOn(
	void
)
{
	frontPrevCapabDesc_t *pcap;

/*sensor power has already stable here*/

	//frontBeforePowerOn();
    //printf("mi5130 initial\n");

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	//globalTimerWait(300);
      
	/* set gpio. */
	//gpioByteFuncSet(GPIO_BYTE_TG0, 0x0F, GPIO_FUNC);
	//gpioByteDirSet(GPIO_BYTE_TG0, 0x0F, GPIO_DIR);
	//gpioByteInGateSet(GPIO_BYTE_TG0, 0x0F, GPIO_GATE);
	//gpioByteOutSet(GPIO_BYTE_TG0, 0x0F, GPIO_OUT);

	
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_30FPS_PREV, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	
	/*Output 24mhz for sensor EXTCLK,it is OK now! */
	
#if 0

	gpioByteFuncSet(GPIO_BYTE_P1, 0x20, 0x20);/*zouchd P15 ----sensor reset*/
	gpioByteDirSet(GPIO_BYTE_P1, 0x20, 0x20);
	gpioByteOutSet(GPIO_BYTE_P1, 0x20, 0x00);
	globalTimerWait(20);
	gpioByteOutSet(GPIO_BYTE_P1, 0x20, 0x20);
	
#endif 

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);
	globalTimerWait(20);

    /* send i2c command. 	*/

	i2cCmdTableSend(frontInit0CmdTbl, 3, sizeof(frontInit0CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(300);
	i2cCmdTableSend(frontInit1CmdTbl, 3, sizeof(frontInit1CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	//HAL_Wait(20);
	//i2cCmdTableSend(front30fpsPrevCmdTbl, 3, sizeof(front30fpsPrevCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);

	//frontcusinit();
	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);

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
__STATIC void
frontOpPreviewModeSet(
	void
)
{
	frontPrevCapabDesc_t *pcap;


	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	//printf("mi5130 rev3 initial 00\n");
	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);
	//printf("mi5130 rev3 initial 01\n");

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_30FPS_PREV, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);


    /* send i2c command. */
	i2cCmdTableSend(front30fpsPrevCmdTbl, 3, sizeof(front30fpsPrevCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	/*frontcusinit();write sensor registor*/

	cdspBayerPatternSet(BAYER_PTN_PREV);
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
 * @since	2008-09-17
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
		i2cCmdTableSend(frontSnapCmdTbl, 3, sizeof(frontSnapCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontSnapExposureTimeSet();
	}
	//while(1);  
	
	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);
	
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);
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

#if 1
	
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[4*1];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	
	//printf("In %s,%ld,frontLastGain=0x%02x\n",__FILE__,(UINT32)__LINE__,frontLastGain);

#if 0

       cmdTbl[0] =( 0x305e>>8); 
	cmdTbl[1] =(0x305e & 0xff);
	cmdTbl[2] = 0xa0;
	cmdTbl[3] = 0xa0;
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);
	
#endif

    cmdTbl[0] =( 0x305e>>8); 
	cmdTbl[1] =(0x305e & 0xff);
	cmdTbl[2] = (frontLastGain>>8);
	cmdTbl[3] = (frontLastGain& 0xff);
	
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);

//	cmdTbl[0] =( 0x301a>>8); 
//	cmdTbl[1] =(0x301a & 0xff);
//	cmdTbl[2] = 0x18;
//	cmdTbl[3] = 0xde;                                                                                                                                        
//	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);


#endif

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
	UINT8 cmdTbl[4*1];

	//printf("In %s,%ld\n",__FILE__,(UINT32)__LINE__);
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
    
	espline = (UINT16)((UINT32)(pixclk *10) /((UINT32)ltotal * frontLastShutter)); 

	
	if(espline>ftotal)
	{
		ftotal= espline;
	}
	

    
	cmdTbl[0] =( 0x300a>>8); 
	cmdTbl[1] =(0x300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0Xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);

	
	//cmdTbl[0] =( 0x0202>>8); 
	//cmdTbl[1] =(0x0202 & 0xff);

    
	cmdTbl[0] =( 0x3012>>8); 
	cmdTbl[1] =(0x3012 & 0xff);
	cmdTbl[2] = ((espline>>8)&0xff);
	cmdTbl[3] = (espline&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
	
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
	UINT32 pixclk;
	UINT8 cmdTbl[4*1];
	UINT16 s_gain, lastt_gain;


	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	
    espline = (pixclk * 10 ) / (ltotal * frontLastShutter);	 
	s_gain = frontLastGain;
    		
	//printf("$$$ will test ltotal=%d,ftotal=%d,pixclk=%ld,espline=%d,shutter=%d\n",ltotal,ftotal,pixclk,espline,frontLastGain);

	espline = espline * 4;  // compensation of binning mode
#if 1
	if ( s_gain >= 0x3c0 ) {         // >8x
		lastt_gain = s_gain - 256;
		espline = espline * 2;
	}
	else {		
		lastt_gain = s_gain;
		espline = espline;
	}
#endif

    //printf("$$$ will test ltotal=%d,ftotal=%d,pixclk=%ld,espline=%d,shutter=%d\n",ltotal,ftotal,pixclk,espline,lastt_gain);

	cdspColorMatrixRestore();

    //	XBYTE[0x21D3] = 0x02;   //Edge Enhancement
    //	XBYTE[0x21D0] = 0x02;

	if ( espline >= ftotal ) {
     	 ftotal= espline;
	}

	cmdTbl[0] =( 0x301a>>8); 
	cmdTbl[1] =(0x301a & 0xff);
	cmdTbl[2] = 0x98;	
	cmdTbl[3] = 0xde;                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);


	
	cmdTbl[0] =( 0x0300a>>8); 
	cmdTbl[1] =(0x0300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);	


    
	cmdTbl[0] =( 0x3012>>8); 
	cmdTbl[1] =(0x3012 & 0xff);
	cmdTbl[2] = ((espline>>8)&0xff);
	cmdTbl[3] = (espline&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
    
	
	cmdTbl[0] =( 0x305e>>8); 
	cmdTbl[1] =(0x305e & 0xff);
	cmdTbl[2] = (lastt_gain>>8);
	cmdTbl[3] = (lastt_gain& 0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);

	
	cmdTbl[0] =( 0x301a>>8); 
	cmdTbl[1] =(0x301a & 0xff);
	cmdTbl[2] = 0x18;	
	cmdTbl[3] = 0xde;                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);	


}


#if 0

/*
 *SDA----P12
 *SCL----P16
 */
//#define	SDA_LOW	setSDA(0x00)
//#define	SDA_HIGH	setSDA(0x20)
//#define	SCL_LOW	setSCL(0x00)
//#define	SCL_HIGH	setSCL(0x10)
#define SCL_HIGH  XBYTE[0x2A64] |= 0x10   /* sck output set to high. */
#define SCL_LOW  XBYTE[0x2A64] &= ~0x10  /* sck output set to low. */
#define SDA_HIGH  XBYTE[0x2A64] |= 0x20   /* sda output set to high. */
#define SDA_LOW  XBYTE[0x2A64] &= ~0x20  /* sda output set to low. */


void setSDA(UINT8 value)
{
	gpioByteFuncSet(GPIO_BYTE_TG0, 0x20, 0x20);/*zouchd  TGGPIO5---- SDA*/
	gpioByteOutSet(GPIO_BYTE_TG0, 0x20, value);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x20);
}
void setSCL(UINT8 value)
{
	gpioByteFuncSet(GPIO_BYTE_TG0, 0x10, 0x10);/*zouchd TGGPIO4 ---- SCL*/
	gpioByteOutSet(GPIO_BYTE_TG0, 0x10, value);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x10, 0x10);
}

void usWait(UINT32 us)
{
	UINT32 i;
	
	for(i= us/10;i>0;i--){
		HAL_10usWait(1);
	}
}

void i2c_delay(void)
{
	/*UINT8 i;
	for(i=0;i<200;i++);*/
	usWait(120);

}

void i2c_delay64us(void)
{
	/*UINT32 i;
	for(i=0;i<1000;i++);*/
	usWait(120);
}


void i2c_start(void)
{
	SCL_HIGH;
	i2c_delay();
	i2c_delay();
	SDA_HIGH;
	i2c_delay();
	i2c_delay();
	SDA_LOW;
	i2c_delay();
	SCL_LOW;
	i2c_delay();
}

void i2c_stop(void)
{
	SDA_LOW;
	i2c_delay();
	SCL_HIGH;
	i2c_delay();
	SDA_HIGH;
	i2c_delay();
}

void i2c_init(void)
{
	gpioByteFuncSet(GPIO_BYTE_TG0, 0x20, 0x20);/*zouchd  TGGPIO5---- SDA hi*/
	gpioByteOutSet(GPIO_BYTE_TG0, 0x20, 0x20);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x20);
	gpioByteFuncSet(GPIO_BYTE_TG0, 0x10, 0x10);/*zouchd TGGPIO4 ---- SCL low*/
	gpioByteOutSet(GPIO_BYTE_TG0, 0x10, 0x00);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x10, 0x10);
	i2c_delay();
	i2c_delay();
	gpioByteOutSet(GPIO_BYTE_TG0, 0x10, 0x10);
}

void i2c_clock(void)
{
	SCL_HIGH;
	i2c_delay();
	i2c_delay();
	SCL_LOW;
	i2c_delay();
}


UINT8 i2c_send(UINT8 i2c_schar)
{
	UINT8 i,j,ack;
	UINT8 value;
	
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x20);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x10, 0x10);

	for (i=0;i<8;i++)
	{		
		j = i2c_schar&0x80;
		SCL_LOW;		
		if(j)
			SDA_HIGH;
		else SDA_LOW;

		i2c_schar <<= 1;
		i2c_delay();
		//i2c_clock();
		SCL_HIGH;
		i2c_delay();
		//i2c_delay();
	}
	//SDA_HIGH;
	SCL_LOW;

	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x00);/*SDA input for ACK*/
	
	i2c_delay();
	SCL_HIGH;
	i2c_delay();
	//i2c_delay();
	//i2c_delay();

	value = gpioByteInGet(GPIO_BYTE_TG0, 0x20, 1);/*Get SDA ACK (high/low)*/
	//printf("\n%bx",value);
	if(value)
		ack = 0;
	else ack = 1;

	i2c_delay();
	SCL_LOW;
	//i2c_delay();
	//i2c_delay();
	//i2c_delay();
	//i2c_delay();

	//hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);  /*hwGenGpioCfgSet(0x0600,0x0600);*/
	//gpioByteDirSet(GPIO_BYTE_P1, 0x04, 0x04);/*SDA OUTput????*/
	//HAL_ConfigGPIOBit(15, 1);
	
	return ack;
}

UINT8 i2c_send16(UINT16 i2c_schar)
{
	UINT8 i,ack;
	UINT8 value;
	
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x20);
	gpioByteDirSet(GPIO_BYTE_TG0, 0x10, 0x10);

	for (i=0;i<16;i++)
	{
		if(i2c_schar&0x8000)
			SDA_HIGH;
		else SDA_LOW;

		i2c_schar <<= 1;
		i2c_delay();
		i2c_clock();
	}
	SDA_HIGH;

	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x00);/*SDA input for ACK*/

	SCL_HIGH;
	i2c_delay();
	i2c_delay();
	
	value = gpioByteInGet(GPIO_BYTE_TG0, 0x20, 1);/*Get SDA ACK (high/low)*/
	
	if(value)
		ack = 0;
	else ack = 1;

	SCL_LOW;
	i2c_delay();

	//hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);  /*hwGenGpioCfgSet(0x0600,0x0600);*/
	//gpioByteDirSet(GPIO_BYTE_P1, 0x04, 0x04);/*SDA OUTput for ACK*/
	//HAL_ConfigGPIOBit(15, 1);
	
	return ack;
}

UINT8 i2c_receive(void)
{
	UINT8 i2c_rchar = 0;
	UINT8 i;
	UINT8 value;
	//hwTgGpioCfgSet(0x00,0x00,0x40,0xc0);   /*hwGenGpioCfgSet(0x0200,0x0600);*/
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x00);/*SDA input for ACK*/
	
	//printf("\n");
	for( i=0;i<8;i++)
	{
		i2c_rchar <<=1;

		SCL_HIGH;
		i2c_delay();

		//if(hwTgGpioMsbRead()&0x80)
		value = gpioByteInGet(GPIO_BYTE_TG0, 0x20, 1);/*get data*/
		if(value){
			i2c_rchar++;
			//printf("1");
		}//else
			//printf("0");

		
		i2c_delay();
		SCL_LOW;
		i2c_delay();
	}
	//printf("\n");

//	hwTgGpioCfgSet(0x00,0x00,0xc0,0xc0);   /*hwGenGpioCfgSet(0x0200,0x0600);*/
	gpioByteDirSet(GPIO_BYTE_TG0, 0x20, 0x20);/*SDA OUTput*/
	return(i2c_rchar);
}


void i2c_nack(void)
{
	SDA_HIGH;
	i2c_delay();
	i2c_clock();
	SDA_HIGH;
}

void i2c_ack(void)
{
	SDA_LOW;
	i2c_delay();
	i2c_clock();
	SDA_HIGH;
}

/*============================================================*
*	read and write functions for devices
*=============================================================*/
void wr_byt_i2c(UINT8 i2caddr,UINT8 regaddr_hi,UINT8 regaddr_low,UINT8 dat_hi,UINT8 dat_low)
{
	UINT8 temp;

	for(temp=0;temp<100;temp++)
	{
		printf("wr_byt_i2c temp = %bu\n",temp);
		i2c_init();
		i2c_start();

		if( !i2c_send(i2caddr) )
		{
			i2c_stop();
			continue;
		}

		if( !i2c_send(regaddr_hi) )
		{
			i2c_stop();
			continue;
		}

		if( !i2c_send(regaddr_low) )
		{
			i2c_stop();
			continue;
		}
		if( !i2c_send(dat_hi) )
		{
			i2c_stop();
			continue;
		}
		if( !i2c_send(dat_low) )
		{
			i2c_stop();
			continue;
		}
		break;
	}
	SCL_LOW;
	i2c_delay64us();		/*Wait tvp5150*/
	i2c_stop();
}

#if 0
UINT16 re_byt_i2c(UINT8 i2caddr,UINT8 regaddr_hi,UINT8 regaddr_low)
{
	UINT8 hi=0x0;
	UINT8 low=0x0;
	UINT16 read ;
	
	i2c_init();
	i2c_start();
	i2c_send(i2caddr);
	i2c_send(regaddr_hi);
	i2c_send(regaddr_low);
	i2c_stop();

	i2c_start();
	i2c_send(i2caddr+1);

	SCL_LOW;
	i2c_delay64us();		/*Wait tvp5150 prepare data*/

	hi = i2c_receive();
	i2c_ack();
	low = i2c_receive();
	i2c_ack();
	i2c_stop();
	read = hi;
	read = read << 8;
	read =read + low;
	
	printf("reg:%02bx%02bx = 0x%02bx%02bx\n",regaddr_hi,regaddr_low,hi,low);
	
	return read;
}


#else

UINT16 re_byt_i2c(UINT8 i2caddr,UINT8 regaddr_hi,UINT8 regaddr_low)
{
	UINT8 hi=0x0;
	UINT8 low=0x0;
	UINT16 read ;
	UINT8 temp;
	
	for(temp=0;temp<100;temp++)
	{
		printf("re_byt_i2c temp = %bu\n",temp);
		i2c_init();
		i2c_start();

		if( !i2c_send(i2caddr) )
		{
			i2c_stop();
			continue;
		}

		if( !i2c_send(regaddr_hi) )
		{
			i2c_stop();
			continue;
		}

		if( !i2c_send(regaddr_low) )
		{
			i2c_stop();
			continue;
		}

		break;
	}
	
	//i2c_stop();
	
	for(temp=0;temp<100;temp++)
	{
		printf("re_byt_i2c temp2 = %bu\n",temp);
		SDA_HIGH;
		i2c_delay();
		i2c_start();
		if( !i2c_send(i2caddr | 0X01) )
		{
			i2c_stop();
			continue;
		}
	}
	SCL_LOW;
	i2c_delay64us();	
	hi = i2c_receive();
	i2c_ack();
	low = i2c_receive();
	i2c_ack();
	i2c_stop();
	read = hi;
	read = read << 8;
	read =read + low;
	
	printf("reg:%02bx%02bx = 0x%02bx%02bx\n",regaddr_hi,regaddr_low,hi,low);
	
	return read;
}

#endif

#if 0


UINT8 digi[10]={0x9f,0x06,0x5b,0x6f,0xc6,0xcd,0xdd,0x07,0xdf,0xcf};

void showdigi(void)
{
	UINT8 i;
	UINT8 m,n;
	for(i=0; i<100; i++)
	{
		n = i % 10;
		m = i /10 %10;
		wr_byt_i2c(0x64,0x01,digi[m]);
		wr_byt_i2c(0x64,0x00,digi[n]);
		//PirmsWait(1000);
		DELAY_1ms();
	}
	
	wr_byt_i2c(0x64,0x01,0);		
	wr_byt_i2c(0x64,0x00,0);
	//PirmsWait(1000);
	DELAY_1ms();
	
	for(i=0; i<100; i++)
	{
		n = i % 10;
		m = i /10 %10;
		wr_byt_i2c(0x64,0x01,digi[m]);
		wr_byt_i2c(0x64,0x00,digi[n]);
		//PirmsWait(1000);
		DELAY_1ms();
	}
	wr_byt_i2c(0x64,0x01,0xff);		
	wr_byt_i2c(0x64,0x00,0xff);
	//PirmsWait(1000);
	DELAY_1ms();

}

void shownum(UINT8 i)
{
	UINT8 m,n;
	
	n = i % 10;
	m = i /10 %10;
	wr_byt_i2c(0x64,0x01,digi[m]);
	wr_byt_i2c(0x64,0x00,digi[n]);
}

void shownull(void)
{
	wr_byt_i2c(0x64,0x01,0);		
	wr_byt_i2c(0x64,0x00,0);
}

void ClrWDT(void)
{
	SCL_HIGH;
	globalTimerWait(1);
	SCL_LOW;
	globalTimerWait(1);
	SCL_HIGH;	
}


static void
frontAllReg16Dump(
	UINT8 dataLen
)
{
	UINT16 addrTbl[1];
	UINT8 dataTbl[1*8];
	UINT16 regIdx;
	UINT8 dataIdx;


	if ( dataLen == 0 || dataLen >= 8 ) {
		printf("data len err\n");
		return;
	}

	for ( regIdx = 0x3000; regIdx < 0x3100; regIdx++ ) {
		addrTbl[0] = regIdx;
		i2cReg16Read(addrTbl, dataTbl, dataLen, 1, 1, 1, 0);
		printf("reg[0x%04X] =", regIdx);
		for ( dataIdx = 0 ; dataIdx < dataLen; dataIdx++ ) {
			printf(" 0x%02bX", dataTbl[dataIdx]);
		}
		printf("\n");
	}
}
#endif

#endif


