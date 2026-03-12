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
 * @date		2009-02-03
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
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "dbg_def.h"
#include "hal_dram.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "ae_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "sp1k_ae_api.h"
#include "snap_core.h"
#include "dbg_file_list.h"
#include "dbg_def.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE __FILE_ID_SENSOR_DRIVER__

#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		0	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */
 #define RES_LSC_LOAD		0	/* after power on load lsc resource file */

/* mode. */
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_20FPS_PREV      1280
#define HEIGHT_20FPS_PREV     480
#define XOFF_20FPS_PREV       273
#define YOFF_20FPS_PREV       16
#define LINE_TOT_20FPS_PREV   1609
#define FRAME_TOT_20FPS_PREV  498

#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       273
#define YOFF_30FPS_PREV       16
#define LINE_TOT_30FPS_PREV   1609
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
#define YOFF_SNAP       20
#define LINE_TOT_SNAP   3252
#define FRAME_TOT_SNAP  1987

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_20FPS_PREV    12//4
#define MCLK_PHASE_20FPS_PREV  0
#define PCLK_DIV_20FPS_PREV    1
#define PCLK_PHASE_20FPS_PREV  0

#define MCLK_DIV_30FPS_PREV    11//11
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    12//4
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP           11
#define MCLK_DIV_SNAP_BURST     18
#define MCLK_DIV_SNAP_BURST_UP  18

#define MCLK_PHASE_SNAP         0
#define PCLK_DIV_SNAP           11
#define PCLK_PHASE_SNAP         0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_20FPS_PREV  16000000L
#define PCLK_FREQ_30FPS_PREV  24000000L
#define PCLK_FREQ_60FPS_PREV  6000000L
#define PCLK_FREQ_SNAP        48000000L
#define PCLK_FREQ_SNAP_BURST  	41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */
#else
#define PCLK_FREQ_20FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP        522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_20FPS_PREV    1
#else
#define RESHAPE_HEN_20FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_20FPS_PREV  2
#define RESHAPE_HRISE_20FPS_PREV  3
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_20FPS_PREV    1
#else
#define RESHAPE_VEN_20FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_20FPS_PREV  2
#define RESHAPE_VRISE_20FPS_PREV  3

#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    1
#else
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  3
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    1
#else
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3

#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_60FPS_PREV    1
#else
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_60FPS_PREV  52
#define RESHAPE_HRISE_60FPS_PREV  54
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_60FPS_PREV    1
#else
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
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
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GRRBGB
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_GRRBGB

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
#define I2C_CLK_DIV   1

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_20FPS_30_MAX_IDX  119
#define AE_20FPS_30_MIN_IDX  1
#define AE_20FPS_25_MAX_IDX  119
#define AE_20FPS_25_MIN_IDX  1
#define EV10_20FPS_EXP_IDX   100

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
#define GAIN_20FPS_MAX_IDX  39
#define GAIN_20FPS_MIN_IDX  0

#define GAIN_30FPS_MAX_IDX  39
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  39
#define GAIN_60FPS_MIN_IDX  0

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor
#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					//AE_GAIN_AFTER_EXP_OFF,
																//AE_GAIN_AFTER_EXP_ON
#if 0
/*----Tq add global gain start----*/
//extern UINT8 Ggain_mode_En;    //will mark                                   //Tq@20080331  --add global gain
#define HighByte(x) 	(unsigned char) ((x) >> 8)
#define LowByte(x)  	(unsigned char) (x)
#define OB_Sensor 2
#define OB_CDSP 6
/*----Tq add global gain start----*/
#endif

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
#define frontOpIntrCapabilityGet      frontSensorOpIntrCapabilityGet
#define frontOpPowerOn                frontSensorOpPowerOn
#define frontOpPowerOff               frontSensorOpPowerOff
#define frontOpPreviewModeSet         frontSensorOpPreviewModeSet
#define frontOpSnapModeSet            frontSensorOpSnapModeSet
#define frontOpSnapShot               frontSensorOpSnapShot
#define frontOpGainSet                frontSensorOpGainSet
#define frontOpExposureTimeSet        frontSensorOpExposureTimeSet
#define frontOpIntrGainSet            frontSensorOpIntrGainSet     
#define frontOpIntrExposureTimeSet    frontSensorOpIntrExposureTimeSet
#define frontOpOpticalBlackStatusGet  frontSensorOpOpticalBlackStatusGet
#define frontOpOpticalBlackCalibrate  frontSensorOpOpticalBlackCalibrate
#define frontOpShutterOpen            frontSensorOpShutterOpen
#define frontOpShutterClose           frontSensorOpShutterClose

/* extern operating function. */
#define frontOpAeTargetLuminanceSet       frontSensorOpAeTargetLuminanceSet
#define frontOpAeExposureCompensationSet  frontSensorOpAeExposureCompensationSet
#define frontOpAeFrameRateSet             frontSensorOpAeFrameRateSet
#define frontOpAwbModeSet                 frontSensorOpAwbModeSet
#define frontOpAfterSnapShot              frontSensorOpAfterSnapShot


/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* operating function. */
__STATIC UINT8 *frontOpDevNameGet(void);
__STATIC void frontOpFrameRateSet(UINT8 fps);
__STATIC void frontOpCapabilityGet(frontCapabilityArg_t *parg);
__STATIC void frontOpIntrCapabilityGet(frontCapabilityArg_t *parg);
__STATIC void frontOpPowerOn(void);
__STATIC void frontOpPowerOff(void);
__STATIC void frontOpPreviewModeSet(void);
__STATIC void frontOpSnapModeSet(void);
__STATIC void frontOpSnapShot(UINT8 first, UINT8 mode, UINT8 scaleUp);
__STATIC void frontOpGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC void frontOpIntrGainSet(UINT8 val, UINT8 isr, UINT8 upd);
__STATIC void frontOpIntrExposureTimeSet(frontExposureTimeArg_t *parg);
__STATIC UINT8 frontOpOpticalBlackStatusGet(void);
__STATIC UINT8 frontOpOpticalBlackCalibrate(void);
__STATIC void frontOpShutterOpen(void);
__STATIC void frontOpShutterClose(void);

/* extern operating function. */
__STATIC void frontOpAeTargetLuminanceSet(void *parg);
__STATIC void frontOpAeExposureCompensationSet(void *parg);
__STATIC void frontOpAeFrameRateSet(void *parg);
__STATIC void frontOpAwbModeSet(void *parg);
__STATIC void frontOpAfterSnapShot(void *parg);

/* local function. */
static void frontBeforePowerOn(void);
static void frontAfterPowerOn(void);
static void frontBeforePowerOff(void);
static void frontAfterPowerOff(void);
static void frontSnapExposureTimeSet(UINT8 mode,UINT8 scaleUp);
static void frontBeforeSnapExposureTimeSet(void);
static void frontResTblBinRead(UINT16 fileId,	UINT16 address);
static void frontResCmdBinSend(UINT16 fileId);
static void FrontSnapGlbGainSet();  //Tq@20090403 --add global gain

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5620 RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table	
 **************************************************************************/
#define CVT_DEF  extern

/* Tag for convert start */
CVT_DEF UINT8 TAG_CVT_S;

/* ae table resource file id  */
typedef enum CvtTypeToX_u_0 {
	//_ae15fps50hzTbl = 0x00C0,
	//_ae15fps60hzTbl = 0x00C1,
	_ae20fps50hzTbl = 0x00C2,
	_ae20fps60hzTbl = 0x00C3,
	//_ae24fps50hzTbl = 0x00C4,
	//_ae24fps60hzTbl = 0x00C5,
	_ae30fps50hzTbl = 0x00C6,
	_ae30fps60hzTbl = 0x00C7,
	_ae60fps50hzTbl = 0x00C8,
	_ae60fps60hzTbl = 0x00C9,
} _aeTbl;

/* gain table resource file id */
typedef enum CvtTypeToX_e {
	_gainTbl = 0x00CA,
};

/* i2c initial cmd table resource file id */
typedef enum CvtTypeRmv_e {
	_frontInit0CmdTbl		= 0x00CB,
	_frontInit1CmdTbl		= 0x00CC,
	//_frontInit2CmdTbl		= 0x00CD,
	//_frontInit3CmdTbl		= 0x00CE,
	//_frontInit4CmdTbl		= 0x00CF,
	//_front15fpsPrevCmdTbl	= 0x00D0,
	_front20fpsPrevCmdTbl	= 0x00D1,
	//_front24fpsPrevCmdTbl	= 0x00D2,
	_front30fpsPrevCmdTbl	= 0x00D3,
	_front60fpsPrevCmdTbl	= 0x00D4,
	_frontSnapCmdTbl		= 0x00D5,
};

/* ae table & gain table link address */
typedef enum CvtTypeLnkAddr_e {
	__aeTbl = 0xa000,
	__gainTbl = 0xb000,
};

/* Tag for convert end */
CVT_DEF UINT8 TAG_CVT_E;

/* ae table. */
static code frontAe_t ae20fps50hzTbl[120] = {
	{      0,    200,     38,      0,	0 },
	{      1,    200,     36,      0,	0 },
	{      2,    200,     34,      0,	0 },
	{      3,    200,     33,      0,	0 },
	{      4,    200,     31,      0,	0 },
	{      5,    200,     30,      0,	0 },
	{      6,    200,     28,      0,	0 },
	{      7,    200,     27,      0,	0 },
	{      8,    200,     26,      0,	0 },
	{      9,    200,     24,      0,	0 },
	{     10,    200,     22,      0,	0 },
	{     11,    200,     20,      0,	0 },
	{     12,    200,     18,      0,	0 },
	{     13,    200,     17,      0,	0 },
	{     14,    200,     15,      0,	0 },
	{     15,    200,     14,      0,	0 },
	{     16,    200,     12,      0,	0 },
	{     17,    200,     11,      0,	0 },
	{     18,    200,     10,      0,	0 },
	{     19,    200,      8,      0,	0 },
	{     20,    200,      6,      0,	0 },
	{     21,    200,      4,      0,	0 },
	{     22,    200,      2,      0,	0 },
	{     23,    200,      1,      0,	0 },
	{     24,    200,      0,      0,	0 },
	{     25,    250,      3,      0,	0 },
	{     26,    250,      1,      0,	0 },
	{     27,    250,      0,      0,	0 },
	{     28,    334,      5,      0,	0 },
	{     29,    334,      3,      0,	0 },
	{     30,    334,      1,      0,	0 },
	{     31,    334,      0,      0,	0 },
	{     32,    502,      9,      0,	0 },
	{     33,    502,      7,      0,	0 },
	{     34,    502,      5,      0,	0 },
	{     35,    502,      3,      0,	0 },
	{     36,    502,      1,      0,	0 },
	{     37,    502,      0,      0,	0 },
	{     38,   1015,     14,      0,	0 },
	{     39,   1001,     12,      0,	0 },
	{     40,   1015,     11,      0,	0 },
	{     41,   1015,     10,      0,	0 },
	{     42,   1015,      9,      0,	0 },
	{     43,   1015,      7,      0,	0 },
	{     44,   1015,      5,      0,	0 },
	{     45,   1015,      3,      0,	0 },
	{     46,   1015,      1,      0,	0 },
	{     47,   1015,      0,      0,	0 },
	{     48,   1113,      0,      0,	0 },
	{     49,   1193,      0,      0,	0 },
	{     50,   1286,      0,      0,	0 },
	{     51,   1320,      0,      0,	0 },
	{     52,   1477,      0,      0,	0 },
	{     53,   1570,      0,      0,	0 },
	{     54,   1695,      0,      0,	0 },
	{     55,   1819,      0,      0,	0 },
	{     56,   1937,      0,      0,	0 },
	{     57,   2072,      0,      0,	0 },
	{     58,   2226,      0,      0,	0 },
	{     59,   2368,      0,      0,	0 },
	{     60,   2572,      0,      0,	0 },
	{     61,   2712,      0,      0,	0 },
	{     62,   2925,      0,      0,	0 },
	{     63,   3108,      0,      0,	0 },
	{     64,   3390,      0,      0,	0 },
	{     65,   3638,      0,      0,	0 },
	{     66,   3825,      0,      0,	0 },
	{     67,   4143,      0,      0,	0 },
	{     68,   4387,      0,      0,	0 },
	{     69,   4661,      0,      0,	0 },
	{     70,   5143,      0,      0,	0 },
	{     71,   5327,      0,      0,	0 },
	{     72,   5737,      0,      0,	0 },
	{     73,   6215,      0,      0,	0 },
	{     74,   6780,      0,      0,	0 },
	{     75,   7103,      0,      0,	0 },
	{     76,   7458,      0,      0,	0 },
	{     77,   8287,      0,      0,	0 },
	{     78,   8774,      0,      0,	0 },
	{     79,   9323,      0,      0,	0 },
	{     80,   9944,      0,      0,	0 },
	{     81,  10654,      0,      0,	0 },
	{     82,  11474,      0,      0,	0 },
	{     83,  12430,      0,      0,	0 },
	{     84,  13560,      0,      0,	0 },
	{     85,  14916,      1,      0,	0 },
	{     86,  14916,      0,      0,	0 },
	{     87,  16573,      0,      0,	0 },
	{     88,  18645,      1,      0,	0 },
	{     89,  18645,      0,      0,	0 },
	{     90,  21308,      1,      0,	0 },
	{     91,  21308,      0,      0,	0 },
	{     92,  24860,      2,      0,	0 },
	{     93,  24860,      0,      0,	0 },
	{     94,  29832,      3,      0,	0 },
	{     95,  29832,      1,      0,	0 },
	{     96,  29832,      0,      0,	0 },
	{     97,  37289,      3,      0,	0 },
	{     98,  37289,      1,      0,	0 },
	{     99,  37289,      0,      0,	0 },
	{    100,  49719,      5,      0,	0 },
	{    101,  49719,      4,      0,	0 },
	{    102,  49719,      2,      0,	0 },
	{    103,  49719,      0,      0,	0 },
	{    104,  74577,      9,      0,	0 },
	{    105,  74577,      7,      0,	0 },
	{    106,  74577,      5,      0,	0 },
	{    107,  74577,      3,      0,	0 },
	{    108,  74577,      1,      0,	0 },
	{    109,  74577,      0,      0,	0 },
	{    110, 149146,     14,      0,	0 },
	{    111, 149146,     13,      0,	0 },
	{    112, 149146,     11,      0,	0 },
	{    113, 149146,     10,      0,	0 },
	{    114, 149146,      9,      0,	0 },
	{    115, 149146,      7,      0,	0 },
	{    116, 149146,      5,      0,	0 },
	{    117, 149146,      3,      0,	0 },
	{    118, 149146,      1,      0,	0 },
	{    119, 149146,      0,      0,	0 },
};

static code frontAe_t ae20fps60hzTbl[120] = {
	{      0,    200,     38,      0,	0 },
	{      1,    200,     36,      0,	0 },
	{      2,    200,     34,      0,	0 },
	{      3,    200,     33,      0,	0 },
	{      4,    200,     31,      0,	0 },
	{      5,    200,     30,      0,	0 },
	{      6,    200,     28,      0,	0 },
	{      7,    200,     27,      0,	0 },
	{      8,    200,     26,      0,	0 },
	{      9,    200,     24,      0,	0 },
	{     10,    200,     22,      0,	0 },
	{     11,    200,     20,      0,	0 },
	{     12,    200,     18,      0,	0 },
	{     13,    200,     17,      0,	0 },
	{     14,    200,     15,      0,	0 },
	{     15,    200,     14,      0,	0 },
	{     16,    200,     12,      0,	0 },
	{     17,    200,     11,      0,	0 },
	{     18,    200,     10,      0,	0 },
	{     19,    200,      8,      0,	0 },
	{     20,    200,      6,      0,	0 },
	{     21,    200,      4,      0,	0 },
	{     22,    200,      2,      0,	0 },
	{     23,    200,      1,      0,	0 },
	{     24,    200,      0,      0,	0 },
	{     25,    240,      2,      0,	0 },
	{     26,    240,      0,      0,	0 },
	{     27,    301,      4,      0,	0 },
	{     28,    301,      2,      0,	0 },
	{     29,    300,      0,      0,	0 },
	{     30,    401,      6,      0,	0 },
	{     31,    401,      4,      0,	0 },
	{     32,    400,      2,      0,	0 },
	{     33,    401,      1,      0,	0 },
	{     34,    401,      0,      0,	0 },
	{     35,    601,      8,      0,	0 },
	{     36,    601,      6,      0,	0 },
	{     37,    601,      4,      0,	0 },
	{     38,    601,      2,      0,	0 },
	{     39,    599,      0,      0,	0 },
	{     40,   1223,     15,      0,	0 },
	{     41,   1203,     13,      0,	0 },
	{     42,   1223,     12,      0,	0 },
	{     43,   1223,     11,      0,	0 },
	{     44,   1203,      9,      0,	0 },
	{     45,   1213,      8,      0,	0 },
	{     46,   1213,      6,      0,	0 },
	{     47,   1213,      4,      0,	0 },
	{     48,   1213,      2,      0,	0 },
	{     49,   1193,      0,      0,	0 },
	{     50,   1286,      0,      0,	0 },
	{     51,   1320,      0,      0,	0 },
	{     52,   1477,      0,      0,	0 },
	{     53,   1570,      0,      0,	0 },
	{     54,   1695,      0,      0,	0 },
	{     55,   1819,      0,      0,	0 },
	{     56,   1937,      0,      0,	0 },
	{     57,   2072,      0,      0,	0 },
	{     58,   2226,      0,      0,	0 },
	{     59,   2368,      0,      0,	0 },
	{     60,   2572,      0,      0,	0 },
	{     61,   2712,      0,      0,	0 },
	{     62,   2925,      0,      0,	0 },
	{     63,   3108,      0,      0,	0 },
	{     64,   3390,      0,      0,	0 },
	{     65,   3638,      0,      0,	0 },
	{     66,   3825,      0,      0,	0 },
	{     67,   4143,      0,      0,	0 },
	{     68,   4387,      0,      0,	0 },
	{     69,   4661,      0,      0,	0 },
	{     70,   5143,      0,      0,	0 },
	{     71,   5327,      0,      0,	0 },
	{     72,   5737,      0,      0,	0 },
	{     73,   6215,      0,      0,	0 },
	{     74,   6780,      0,      0,	0 },
	{     75,   7103,      0,      0,	0 },
	{     76,   7458,      0,      0,	0 },
	{     77,   8287,      0,      0,	0 },
	{     78,   8774,      0,      0,	0 },
	{     79,   9323,      0,      0,	0 },
	{     80,   9944,      0,      0,	0 },
	{     81,  10654,      0,      0,	0 },
	{     82,  11474,      0,      0,	0 },
	{     83,  12430,      0,      0,	0 },
	{     84,  13560,      0,      0,	0 },
	{     85,  14916,      1,      0,	0 },
	{     86,  14916,      0,      0,	0 },
	{     87,  16573,      0,      0,	0 },
	{     88,  18645,      1,      0,	0 },
	{     89,  18645,      0,      0,	0 },
	{     90,  21308,      1,      0,	0 },
	{     91,  21308,      0,      0,	0 },
	{     92,  24860,      2,      0,	0 },
	{     93,  24860,      0,      0,	0 },
	{     94,  29832,      3,      0,	0 },
	{     95,  29832,      1,      0,	0 },
	{     96,  29832,      0,      0,	0 },
	{     97,  37289,      3,      0,	0 },
	{     98,  37289,      1,      0,	0 },
	{     99,  37289,      0,      0,	0 },
	{    100,  49719,      5,      0,	0 },
	{    101,  49719,      4,      0,	0 },
	{    102,  49719,      2,      0,	0 },
	{    103,  49719,      0,      0,	0 },
	{    104,  74577,      9,      0,	0 },
	{    105,  74577,      7,      0,	0 },
	{    106,  74577,      5,      0,	0 },
	{    107,  74577,      3,      0,	0 },
	{    108,  74577,      1,      0,	0 },
	{    109,  74577,      0,      0,	0 },
	{    110, 149146,     14,      0,	0 },
	{    111, 149146,     13,      0,	0 },
	{    112, 149146,     11,      0,	0 },
	{    113, 149146,     10,      0,	0 },
	{    114, 149146,      9,      0,	0 },
	{    115, 149146,      7,      0,	0 },
	{    116, 149146,      5,      0,	0 },
	{    117, 149146,      3,      0,	0 },
	{    118, 149146,      1,      0,	0 },
	{    119, 149146,      0,      0,	0 },
};

static code frontAe_t ae30fps50hzTbl[104] = {
	{      0,    333,     39,      0,	0 },
	{      1,    333,     39,      0,	0 },
	{      2,    333,     38,      0,	0 },
	{      3,    333,     36,      0,	0 },
	{      4,    334,     35,      0,	0 },
	{      5,    334,     33,      0,	0 },
	{      6,    333,     32,      0,	0 },
	{      7,    333,     30,      0,	0 },
	{      8,    333,     29,      0,	0 },
	{      9,    333,     28,      0,	0 },
	{     10,    333,     27,      0,	0 },
	{     11,    333,     25,      0,	0 },
	{     12,    333,     23,      0,	0 },
	{     13,    333,     21,      0,	0 },
	{     14,    333,     20,      0,	0 },
	{     15,    333,     19,      0,	0 },
	{     16,    333,     17,      0,	0 },
	{     17,    333,     16,      0,	0 },
	{     18,    333,     14,      0,	0 },
	{     19,    333,     12,      0,	0 },
	{     20,    333,     11,      0,	0 },
	{     21,    333,      9,      0,	0 },
	{     22,    333,      8,      0,	0 },
	{     23,    500,     14,      0,	0 },
	{     24,    500,     12,      0,	0 },
	{     25,    502,     10,      0,	0 },
	{     26,    500,      9,      0,	0 },
	{     27,    500,      7,      0,	0 },
	{     28,    500,      6,      0,	0 },
	{     29,    500,      4,      0,	0 },
	{     30,    500,      2,      0,	0 },
	{     31,    500,      0,      0,	0 },
	{     32,    994,     14,      0,	0 },
	{     33,   1015,     13,      0,	0 },
	{     34,    994,     11,      0,	0 },
	{     35,   1001,     10,      0,	0 },
	{     36,   1015,      9,      0,	0 },
	{     37,    994,      6,      0,	0 },
	{     38,    994,      5,      0,	0 },
	{     39,    994,      3,      0,	0 },
	{     40,    994,      1,      0,	0 },
	{     41,   1015,      0,      0,	0 },
	{     42,   1223,      3,      0,	0 },
	{     43,   1165,      0,      0,	0 },
	{     44,   1394,      3,      0,	0 },
	{     45,   1394,      1,      0,	0 },
	{     46,   1434,      0,      0,	0 },
	{     47,   1538,      0,      0,	0 },
	{     48,   1912,      4,      0,	0 },
	{     49,   1912,      2,      0,	0 },
	{     50,   2445,      7,      0,	0 },
	{     51,   2445,      5,      0,	0 },
	{     52,   2445,      3,      0,	0 },
	{     53,   2331,      0,      0,	0 },
	{     54,   2814,      3,      0,	0 },
	{     55,   2814,      1,      0,	0 },
	{     56,   2868,      0,      0,	0 },
	{     57,   3469,      3,      0,	0 },
	{     58,   4262,      7,      0,	0 },
	{     59,   3825,      2,      0,	0 },
	{     60,   4262,      3,      0,	0 },
	{     61,   4520,      2,      0,	0 },
	{     62,   4520,      1,      0,	0 },
	{     63,   4661,      0,      0,	0 },
	{     64,   5737,      4,      0,	0 },
	{     65,   6485,      5,      0,	0 },
	{     66,   5737,      0,      0,	0 },
	{     67,   6485,      1,      0,	0 },
	{     68,   8774,      9,      0,	0 },
	{     69,   8774,      6,      0,	0 },
	{     70,   8774,      4,      0,	0 },
	{     71,   8774,      2,      0,	0 },
	{     72,   9944,      4,      0,	0 },
	{     73,   9944,      2,      0,	0 },
	{     74,   9944,      0,      0,	0 },
	{     75,  10654,      0,      0,	0 },
	{     76,  11474,      2,      0,	0 },
	{     77,  11474,      0,      0,	0 },
	{     78,  16391,      6,      0,	0 },
	{     79,  16391,      4,      0,	0 },
	{     80,  16391,      2,      0,	0 },
	{     81,  18645,      4,      0,	0 },
	{     82,  18645,      2,      0,	0 },
	{     83,  18645,      0,      0,	0 },
	{     84,  24860,      6,      0,	0 },
	{     85,  21308,      0,      0,	0 },
	{     86,  24860,      2,      0,	0 },
	{     87,  29832,      5,      0,	0 },
	{     88,  29832,      3,      0,	0 },
	{     89,  29832,      1,      0,	0 },
	{     90,  36380,      6,      0,	0 },
	{     91,  36380,      4,      0,	0 },
	{     92,  36380,      2,      0,	0 },
	{     93,  36380,      0,      0,	0 },
	{     94,  49719,      7,      0,	0 },
	{     95,  49719,      5,      0,	0 },
	{     96,  49719,      3,      0,	0 },
	{     97,  49719,      1,      0,	0 },
	{     98,  74577,     15,      0,	0 },
	{     99,  74577,     13,      0,	0 },
	{    100,  74577,     11,      0,	0 },
	{    101,  74577,      9,      0,	0 },
	{    102,  74577,      7,      0,	0 },
	{    103,  74577,      5,      0,	0 },
};

static code frontAe_t ae30fps60hzTbl[104] = {
	{      0,    300,     38,      0,	0 },
	{      1,    300,     36,      0,	0 },
	{      2,    300,     34,      0,	0 },
	{      3,    301,     33,      0,	0 },
	{      4,    300,     31,      0,	0 },
	{      5,    401,     37,      0,	0 },
	{      6,    300,     28,      0,	0 },
	{      7,    400,     33,      0,	0 },
	{      8,    399,     31,      0,	0 },
	{      9,    401,     30,      0,	0 },
	{     10,    399,     28,      0,	0 },
	{     11,    401,     27,      0,	0 },
	{     12,    300,     18,      0,	0 },
	{     13,    301,     17,      0,	0 },
	{     14,    300,     15,      0,	0 },
	{     15,    401,     21,      0,	0 },
	{     16,    300,     12,      0,	0 },
	{     17,    400,     17,      0,	0 },
	{     18,    399,     15,      0,	0 },
	{     19,    401,     14,      0,	0 },
	{     20,   1223,     39,      0,	0 },
	{     21,    401,     11,      0,	0 },
	{     22,   1223,     35,      0,	0 },
	{     23,   1213,     33,      0,	0 },
	{     24,    599,     15,      0,	0 },
	{     25,   1223,     30,      0,	0 },
	{     26,   1193,     28,      0,	0 },
	{     27,    400,      1,      0,	0 },
	{     28,   1223,     26,      0,	0 },
	{     29,   1193,     24,      0,	0 },
	{     30,   1223,     23,      0,	0 },
	{     31,   1223,     21,      0,	0 },
	{     32,   1223,     19,      0,	0 },
	{     33,   1213,     17,      0,	0 },
	{     34,   1193,     15,      0,	0 },
	{     35,   1223,     14,      0,	0 },
	{     36,   1193,     12,      0,	0 },
	{     37,   1213,     11,      0,	0 },
	{     38,   1223,     10,      0,	0 },
	{     39,   1193,      8,      0,	0 },
	{     40,   1223,      7,      0,	0 },
	{     41,   1223,      5,      0,	0 },
	{     42,   1223,      3,      0,	0 },
	{     43,   1213,      1,      0,	0 },
	{     44,   2814,     19,      0,	0 },
	{     45,   1394,      1,      0,	0 },
	{     46,   1434,      0,      0,	0 },
	{     47,   1538,      0,      0,	0 },
	{     48,   4262,     23,      0,	0 },
	{     49,   1912,      2,      0,	0 },
	{     50,   2445,      7,      0,	0 },
	{     51,   2445,      5,      0,	0 },
	{     52,   2445,      3,      0,	0 },
	{     53,   2331,      0,      0,	0 },
	{     54,   2814,      3,      0,	0 },
	{     55,   6485,     21,      0,	0 },
	{     56,   2868,      0,      0,	0 },
	{     57,   3469,      3,      0,	0 },
	{     58,   4262,      7,      0,	0 },
	{     59,   3825,      2,      0,	0 },
	{     60,   4262,      3,      0,	0 },
	{     61,   4520,      2,      0,	0 },
	{     62,   4520,      1,      0,	0 },
	{     63,   4661,      0,      0,	0 },
	{     64,   5737,      4,      0,	0 },
	{     65,   6485,      5,      0,	0 },
	{     66,   5737,      0,      0,	0 },
	{     67,   6485,      1,      0,	0 },
	{     68,   8774,      9,      0,	0 },
	{     69,   8774,      6,      0,	0 },
	{     70,   8774,      4,      0,	0 },
	{     71,   8774,      2,      0,	0 },
	{     72,   9944,      4,      0,	0 },
	{     73,   9944,      2,      0,	0 },
	{     74,   9944,      0,      0,	0 },
	{     75,  10654,      0,      0,	0 },
	{     76,  11474,      2,      0,	0 },
	{     77,  11474,      0,      0,	0 },
	{     78,  16391,      6,      0,	0 },
	{     79,  16391,      4,      0,	0 },
	{     80,  16391,      2,      0,	0 },
	{     81,  18645,      4,      0,	0 },
	{     82,  18645,      2,      0,	0 },
	{     83,  18645,      0,      0,	0 },
	{     84,  24860,      6,      0,	0 },
	{     85,  21308,      0,      0,	0 },
	{     86,  24860,      2,      0,	0 },
	{     87,  29832,      5,      0,	0 },
	{     88,  29832,      3,      0,	0 },
	{     89,  29832,      1,      0,	0 },
	{     90,  36380,      6,      0,	0 },
	{     91,  36380,      4,      0,	0 },
	{     92,  36380,      2,      0,	0 },
	{     93,  36380,      0,      0,	0 },
	{     94,  49719,      7,      0,	0 },
	{     95,  49719,      5,      0,	0 },
	{     96,  49719,      3,      0,	0 },
	{     97,  49719,      1,      0,	0 },
	{     98,  74577,     15,      0,	0 },
	{     99,  74577,     13,      0,	0 },
	{    100,  74577,     11,      0,	0 },
	{    101,  74577,      9,      0,	0 },
	{    102,  74577,      7,      0,	0 },
	{    103,  74577,      5,      0,	0 },
};

static code frontAe_t ae60fps50hzTbl[102] = {
	{      0,    993,     45,      0,	0 },
	{      1,    993,     43,      0,	0 },
	{      2,    993,     41,      0,	0 },
	{      3,   1019,     40,      0,	0 },
	{      4,   1012,     38,      0,	0 },
	{      5,    993,     36,      0,	0 },
	{      6,   1012,     35,      0,	0 },
	{      7,    993,     33,      0,	0 },
	{      8,    999,     32,      0,	0 },
	{      9,   1012,     31,      0,	0 },
	{     10,   1019,     30,      0,	0 },
	{     11,    993,     27,      0,	0 },
	{     12,    993,     25,      0,	0 },
	{     13,   1019,     24,      0,	0 },
	{     14,   1012,     22,      0,	0 },
	{     15,    993,     20,      0,	0 },
	{     16,   1012,     19,      0,	0 },
	{     17,    993,     17,      0,	0 },
	{     18,    999,     16,      0,	0 },
	{     19,   1012,     15,      0,	0 },
	{     20,   1019,     14,      0,	0 },
	{     21,    993,     11,      0,	0 },
	{     22,    993,      9,      0,	0 },
	{     23,   1019,      8,      0,	0 },
	{     24,   1012,      6,      0,	0 },
	{     25,    993,      4,      0,	0 },
	{     26,   1012,      3,      0,	0 },
	{     27,    993,      1,      0,	0 },
	{     28,    999,      0,      0,	0 },
	{     29,   4525,     33,      0,	0 },
	{     30,   2167,     15,      0,	0 },
	{     31,   1570,      5,      0,	0 },
	{     32,   7326,     39,      0,	0 },
	{     33,   1411,      0,      0,	0 },
	{     34,   2522,     12,      0,	0 },
	{     35,   2522,     10,      0,	0 },
	{     36,   2608,      9,      0,	0 },
	{     37,   2797,      9,      0,	0 },
	{     38,   2107,      1,      0,	0 },
	{     39,   9050,     33,      0,	0 },
	{     40,   2296,      0,      0,	0 },
	{     41,   3140,      5,      0,	0 },
	{     42,   7326,     23,      0,	0 },
	{     43,   4396,     10,      0,	0 },
	{     44,  12820,     33,      0,	0 },
	{     45,   3419,      1,      0,	0 },
	{     46,   3663,      1,      0,	0 },
	{     47,   4963,      6,      0,	0 },
	{     48,   4662,      3,      0,	0 },
	{     49,   9050,     17,      0,	0 },
	{     50,  30769,     44,      0,	0 },
	{     51,  30769,     42,      0,	0 },
	{     52,   7326,      7,      0,	0 },
	{     53,   9050,     11,      0,	0 },
	{     54,  25641,     33,      0,	0 },
	{     55,   9050,      7,      0,	0 },
	{     56,   7326,      1,      0,	0 },
	{     57,  12820,     13,      0,	0 },
	{     58,  12820,     11,      0,	0 },
	{     59,   9050,      1,      0,	0 },
	{     60,  51280,     39,      0,	0 },
	{     61,  30769,     26,      0,	0 },
	{     62,  12820,      4,      0,	0 },
	{     63,  11834,      1,      0,	0 },
	{     64,  51280,     33,      0,	0 },
	{     65,  12820,      0,      0,	0 },
	{     66,  15384,      2,      0,	0 },
	{     67,  25641,     13,      0,	0 },
	{     68,  25641,     11,      0,	0 },
	{     69,  17094,      0,      0,	0 },
	{     70,  51280,     23,      0,	0 },
	{     71,  30769,     10,      0,	0 },
	{     72,  25641,      4,      0,	0 },
	{     73,  38461,     13,      0,	0 },
	{     74,  51280,     17,      0,	0 },
	{     75,  25641,      0,      0,	0 },
	{     76,  30769,      2,      0,	0 },
	{     77,  51280,     13,      0,	0 },
	{     78,  51280,     11,      0,	0 },
	{     79,  51280,      9,      0,	0 },
	{     80,  51280,      7,      0,	0 },
	{     81,  51280,      5,      0,	0 },
	{     82,  51280,      4,      0,	0 },
	{     83,  76919,     13,      0,	0 },
	{     84,  51280,      1,      0,	0 },
	{     85,  51280,      0,      0,	0 },
	{     86,  76919,      7,      0,	0 },
	{     87, 153831,     21,      0,	0 },
	{     88,  76919,      4,      0,	0 },
	{     89, 153831,     18,      0,	0 },
	{     90,  76919,      1,      0,	0 },
	{     91,  76919,      0,      0,	0 },
	{     92, 153831,     14,      0,	0 },
	{     93, 153831,     13,      0,	0 },
	{     94, 153831,     11,      0,	0 },
	{     95, 153831,      9,      0,	0 },
	{     96, 153831,      7,      0,	0 },
	{     97, 153831,      5,      0,	0 },
	{     98, 153831,      4,      0,	0 },
	{     99, 153831,      2,      0,	0 },
	{    100, 153831,      1,      0,	0 },
	{    101, 153831,      0,      0,	0 },
};

static code frontAe_t ae60fps60hzTbl[107] = {
	{      0,    601,     41,      0,	0 },
	{      1,    601,     39,      0,	0 },
	{      2,    599,     37,      0,	0 },
	{      3,    601,     36,      0,	0 },
	{      4,    599,     34,      0,	0 },
	{      5,    601,     33,      0,	0 },
	{      6,    601,     32,      0,	0 },
	{      7,    599,     30,      0,	0 },
	{      8,    601,     29,      0,	0 },
	{      9,    601,     27,      0,	0 },
	{     10,   1221,     41,      0,	0 },
	{     11,   1211,     39,      0,	0 },
	{     12,   1193,     37,      0,	0 },
	{     13,   1221,     36,      0,	0 },
	{     14,   1193,     34,      0,	0 },
	{     15,   1211,     33,      0,	0 },
	{     16,   1221,     32,      0,	0 },
	{     17,   1193,     30,      0,	0 },
	{     18,   1211,     29,      0,	0 },
	{     19,   1221,     27,      0,	0 },
	{     20,   1221,     25,      0,	0 },
	{     21,   1211,     23,      0,	0 },
	{     22,   1193,     21,      0,	0 },
	{     23,   1221,     20,      0,	0 },
	{     24,   1193,     18,      0,	0 },
	{     25,   1211,     17,      0,	0 },
	{     26,   1221,     16,      0,	0 },
	{     27,   1193,     14,      0,	0 },
	{     28,   1211,     13,      0,	0 },
	{     29,   1221,     11,      0,	0 },
	{     30,   1221,      9,      0,	0 },
	{     31,   1211,      7,      0,	0 },
	{     32,   1193,      5,      0,	0 },
	{     33,   1221,      4,      0,	0 },
	{     34,   1193,      2,      0,	0 },
	{     35,   1211,      1,      0,	0 },
	{     36,   1221,      0,      0,	0 },
	{     37,   7326,     39,      0,	0 },
	{     38,   1411,      0,      0,	0 },
	{     39,   2522,     12,      0,	0 },
	{     40,   2522,     10,      0,	0 },
	{     41,   2608,      9,      0,	0 },
	{     42,   2797,      9,      0,	0 },
	{     43,   2107,      1,      0,	0 },
	{     44,   9050,     33,      0,	0 },
	{     45,   2296,      0,      0,	0 },
	{     46,   3140,      5,      0,	0 },
	{     47,   7326,     23,      0,	0 },
	{     48,   4396,     10,      0,	0 },
	{     49,  12820,     33,      0,	0 },
	{     50,   3419,      1,      0,	0 },
	{     51,   3663,      1,      0,	0 },
	{     52,   4963,      6,      0,	0 },
	{     53,   4662,      3,      0,	0 },
	{     54,   9050,     17,      0,	0 },
	{     55,  25641,     39,      0,	0 },
	{     56,  21978,     34,      0,	0 },
	{     57,   7326,      7,      0,	0 },
	{     58,   9050,     11,      0,	0 },
	{     59,  25641,     33,      0,	0 },
	{     60,   9050,      7,      0,	0 },
	{     61,   7326,      1,      0,	0 },
	{     62,  12820,     13,      0,	0 },
	{     63,  12820,     11,      0,	0 },
	{     64,   9050,      1,      0,	0 },
	{     65,  51280,     39,      0,	0 },
	{     66,  30769,     26,      0,	0 },
	{     67,  12820,      4,      0,	0 },
	{     68,  11834,      1,      0,	0 },
	{     69,  51280,     33,      0,	0 },
	{     70,  12820,      0,      0,	0 },
	{     71,  15384,      2,      0,	0 },
	{     72,  25641,     13,      0,	0 },
	{     73,  25641,     11,      0,	0 },
	{     74,  17094,      0,      0,	0 },
	{     75,  51280,     23,      0,	0 },
	{     76,  30769,     10,      0,	0 },
	{     77,  25641,      4,      0,	0 },
	{     78,  38461,     13,      0,	0 },
	{     79,  51280,     17,      0,	0 },
	{     80,  25641,      0,      0,	0 },
	{     81,  30769,      2,      0,	0 },
	{     82,  51280,     13,      0,	0 },
	{     83,  51280,     11,      0,	0 },
	{     84,  51280,      9,      0,	0 },
	{     85,  51280,      7,      0,	0 },
	{     86,  51280,      5,      0,	0 },
	{     87,  51280,      4,      0,	0 },
	{     88,  76919,     13,      0,	0 },
	{     89,  51280,      1,      0,	0 },
	{     90,  51280,      0,      0,	0 },
	{     91,  76919,      7,      0,	0 },
	{     92, 153831,     21,      0,	0 },
	{     93,  76919,      4,      0,	0 },
	{     94, 153831,     18,      0,	0 },
	{     95,  76919,      1,      0,	0 },
	{     96,  76919,      0,      0,	0 },
	{     97, 153831,     14,      0,	0 },
	{     98, 153831,     13,      0,	0 },
	{     99, 153831,     11,      0,	0 },
	{    100, 153831,      9,      0,	0 },
	{    101, 153831,      7,      0,	0 },
	{    102, 153831,      5,      0,	0 },
	{    103, 153831,      4,      0,	0 },
	{    104, 153831,      2,      0,	0 },
	{    105, 153831,      1,      0,	0 },
	{    106, 153831,      0,      0,	0 },
};

/* gain table. */
static code UINT16 gainTbl[40] = {
	  8,   9,  10,  11,  12,  13,  14,  15,  16,  17,
	 18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
	 28,  29,  30,  31,  48,  49,  50,  51,  52,  53,
	 54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
};

/* exposure time index table. */
static code UINT8 ae20fpsMaxIdxTbl[2] = {
	AE_20FPS_30_MAX_IDX,
	AE_20FPS_25_MAX_IDX,
};

static code UINT8 ae20fpsMinIdxTbl[2] = {
	AE_20FPS_30_MIN_IDX,
	AE_20FPS_25_MIN_IDX,
};

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
	{  /* 20 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_20FPS_PREV,
		HEIGHT_20FPS_PREV,
		XOFF_20FPS_PREV,
		YOFF_20FPS_PREV,
		LINE_TOT_20FPS_PREV,
		FRAME_TOT_20FPS_PREV,
		MCLK_DIV_20FPS_PREV,
		MCLK_PHASE_20FPS_PREV,
		PCLK_DIV_20FPS_PREV,
		PCLK_PHASE_20FPS_PREV,
		PCLK_FREQ_20FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_20FPS_PREV,
		RESHAPE_HFALL_20FPS_PREV,
		RESHAPE_HRISE_20FPS_PREV,
		RESHAPE_VEN_20FPS_PREV,
		RESHAPE_VFALL_20FPS_PREV,
		RESHAPE_VRISE_20FPS_PREV,
		ae20fps60hzTbl,
		ae20fps50hzTbl,
		ae20fpsMaxIdxTbl,
		ae20fpsMinIdxTbl,
		EV10_20FPS_EXP_IDX,
		gainTbl,
		GAIN_20FPS_MAX_IDX,
		GAIN_20FPS_MIN_IDX,
	},
	{  /* 30 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
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
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
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
		gainTbl,
		GAIN_30FPS_MAX_IDX,
		GAIN_30FPS_MIN_IDX,
	},
	{  /* 60 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
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
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
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
		gainTbl,
		GAIN_60FPS_MAX_IDX,
		GAIN_60FPS_MIN_IDX,
	},
};

static code frontSnapCapabDesc_t frontSnapCapabDesc[SNAP_MODE_TOT] = {
	{  /* 30/60 fps. */
		BAYER_PTN_SNAP,
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
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
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

//	0x12, 0x00,
	0x13, 0x00,
	0x11, 0x00,
//	0x15, 0x42,
 	0x15, 0x40,
	0x10, 0x1E,
	0x3B, 0x07,
	0x5F, 0x02,
	0x60, 0xA2,
	0x61, 0xF5,

	0x5B, 0x40,
	0x39, 0x07,
	0x53, 0x02,
	0x54, 0x60,
	0x04, 0x20,
	0x27, 0x04,
	0x3D, 0x40,

	0x36, 0x00,
	0xC5, 0x04,

	0x4E, 0x00,
	0x4F, 0x93,
	0x50, 0x7B,
	0xCA, 0x0C,
	0xCB, 0x0F,

	0x39, 0x07,
	0x4A, 0x10,
	0x3E, 0x0A,
	0x3D, 0x00,

	0x89, 0x20,
	0x83, 0x80,
	0xB7, 0x9D,
	0xB6, 0x11,
	0xB5, 0x55,
	0xB4, 0x00,
	0xA9, 0xF0,
	0xA8, 0x0A,
	0xB8, 0xF0,
	0xB9, 0xF0,
	0xBA, 0xF0,

	0x81, 0x07,
	0x63, 0x44,

	0x14, 0x60,
	0x33, 0x75,
	0x2C, 0x00,
	0x09, 0x00,

	0x35, 0x30,
	0x27, 0x04,
	0x3C, 0x07,
	0x3A, 0x0A,
	0x3B, 0x07,
	0x01, 0x40,
	0x02, 0x40,

//	0x35, 0x30,
//	0x27, 0x04,
//	0x3C, 0x07,
//	0x35, 0x30,
//	0x27, 0x04,

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
	0x6C, 0x00,
	0x6D, 0x80,
	0x6E, 0x00,
	0x70, 0x04,
	0x71, 0x00,
	0x8D, 0x04,
	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x67, 0x00,
	0x68, 0x00,
	0x69, 0x00,
	0x6A, 0x00,
	0x6B, 0x00,

	0x71, 0x94,
	0x74, 0x20,

	0x80, 0x09,

	0x85, 0xC0,
};

static code UINT8 front20fpsPrevCmdTbl[] = {
	0x12, 0x10,

	0x38, 0x10,
	0x2A, 0x10,
	0x2B, 0x4C,

//	0x0C, 0x0A,
	0x0C, 0x0E,
	0x5F, 0x02,
	0x61, 0x3D,

	0x4E, 0x00,
	0x4F, 0x94,
	0x50, 0x7C,
	0xCA, 0x02,
	0xCB, 0x03,

	0x0D, 0x96,
	0x17, 0x12,
	0x19, 0x01,
	0x03, 0x40,
	0x53, 0x0E,
	0x54, 0x61,
	0x46, 0x11,
	0x37, 0x00,
	0x44, 0xC0,

//	0x04, 0x30,
	0x8D, 0x64,
//	0x04, 0xB0,
//	0x04, 0x60,
	0x04, 0x20,
	0x15, 0x42,
	0x11, 0x40,

	0x3E, 0x0A,
	0x3D, 0x00,
	0x8D, 0x44,
	0x10, 0x38,
	0x14, 0x66,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x12, 0x10,

	0x38, 0x10,
	0x2A, 0x10,
	0x2B, 0x4C,

//	0x0C, 0x0A,
	0x0C, 0x0E,
	0x5F, 0x02,
	0x61, 0x3D,

	0x4E, 0x00,
	0x4F, 0x94,
	0x50, 0x7C,
	0xCA, 0x02,
	0xCB, 0x03,

	0x0D, 0x96,
	0x17, 0x12,
	0x19, 0x01,
	0x03, 0x40,
	0x53, 0x0E,
	0x54, 0x61,
	0x46, 0x11,
	0x37, 0x00,
	0x44, 0xC0,

//	0x04, 0x30,
	0x8D, 0x64,
//	0x04, 0xB0,
//	0x04, 0x60,
	0x04, 0x20,
	0x15, 0x42,
	0x11, 0x40,

	0x3E, 0x0A,
	0x3D, 0x00,
	0x8D, 0x44,
	0x10, 0x38,
	0x14, 0x66,
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
	0x12, 0x00,
	0x17, 0x12,
	0x18, 0xFF,
	0x19, 0x00,
	0x1A, 0xFF,
	0x03, 0x07,
	0x11, 0x40,

	0x3E, 0x0A,
	0x3D, 0x00,

	0x0C, 0x0D,
	0x38, 0x90,

	0x46, 0x30,
	0x5F, 0x02,
	0x60, 0xA2,
	0x61, 0xF5,
	0x0D, 0x06,

	0x05, 0x11,
	0x06, 0x11,
	0x07, 0x11,
	0x08, 0x11,

	0x2A, 0x00,
	0x2B, 0x00,

	0x37, 0x04,
	0x44, 0x00,
	0x45, 0x00,
	0x46, 0x00,

	0x4F, 0x93,
	0x50, 0x7B,

	0x53, 0x02,
	0x54, 0x60,

	0x64, 0x00,
	0x65, 0x00,
	0x66, 0x00,
	0x67, 0x00,

	0x7F, 0x00,
	0x8D, 0x04,

	0xCA, 0x0C,
	0xCB, 0x0F,
};

static UINT8 frontPrevMode = 1;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;
static UINT32 frontLastShutter;
//extern UINT16 GlbGain;  //Tq@20090403  --add global gain
extern UINT8  base_one;

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
	#if (REGISTER_EXTERN_OP_FUNC == 1)
	UINT8 id;
	#endif
	frontOperatingFunc_t opFunc;


	opFunc.expGainSetPos             = EXP_GAIN_SET_POS;
	opFunc.gainAfterExp              = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet            = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet          = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet         = frontOpCapabilityGet;
	opFunc.pfOpIntrCapabilityGet     = frontOpIntrCapabilityGet;
	opFunc.pfOpPowerOn               = frontOpPowerOn;
	opFunc.pfOpPowerOff              = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet        = frontOpPreviewModeSet;
	opFunc.pfOpSnapModeSet           = frontOpSnapModeSet;
	opFunc.pfOpSnapShot              = frontOpSnapShot;
	opFunc.pfOpGainSet               = frontOpGainSet;
	opFunc.pfOpExposureTimeSet       = frontOpExposureTimeSet;
	opFunc.pfOpIntrGainSet           = frontOpIntrGainSet;
	opFunc.pfOpIntrExposureTimeSet   = frontOpIntrExposureTimeSet;
	opFunc.pfOpOpticalBlackStatusGet = frontOpOpticalBlackStatusGet;
	opFunc.pfOpOpticalBlackCalibrate = frontOpOpticalBlackCalibrate;
	opFunc.pfOpShutterOpen           = frontOpShutterOpen;
	opFunc.pfOpShutterClose          = frontOpShutterClose;

	/* load gain tbl to static xdata UINT16 gainTbl[48] _at_ 0xb000; */
	frontResTblBinRead(_gainTbl, __gainTbl);

	#if (REGISTER_EXTERN_OP_FUNC == 1)
	/* register extern operating function */
	id = frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));

	if ( id != NULL ) {
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET, frontOpAeTargetLuminanceSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET, frontOpAeExposureCompensationSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_FRAME_RATE_SET, frontOpAeFrameRateSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AWB_MODE_SET, frontOpAwbModeSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AFTER_SNAP_SHOT, frontOpAfterSnapShot);
	}

	return id;
	#else
	return frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));
	#endif
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
		case 24:
		case 20:
			printf("frontOpFrameRateSet = 24\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
			
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 20) {
				printf("50Hz\n");
				frontResTblBinRead(_ae20fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae20fps60hzTbl, __aeTbl);
			}
			break;
		case 30:
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 1;
			//frontSnapMode = 1;

			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 25) {
				printf("50Hz\n");
				frontResTblBinRead(_ae30fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae30fps60hzTbl, __aeTbl);
			} 
			break;
		case 60:
		case 50:
			printf("frontOpFrameRateSet = 60\n");
			frontPrevMode = 2;
			//frontSnapMode = 2;

			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
			if (fps == 50) {
				printf("50Hz\n");
				frontResTblBinRead(_ae60fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae60fps60hzTbl, __aeTbl);
			}
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
frontOpIntrCapabilityGet(
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
	frontPrevCapabDesc_t *pcap;


	/* before power on. */
	frontBeforePowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	//debug
	#if (DBG_SNAP_TIMING == 1)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif

	//XBYTE[0x2a60] = 0x00;
	//XBYTE[0x2a62] = 0x04;
	//XBYTE[0x2a5c] = 0x00;
	//printf("XBYTE[0x2a60]=0x%bx\n",XBYTE[0x2a60]);
	
		
	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	frontResCmdBinSend(_frontInit0CmdTbl);
	//i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	globalTimerWait(10);
	frontResCmdBinSend(_frontInit1CmdTbl);	
	//i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	globalTimerWait(10);

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

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 20 fps. */
		frontResCmdBinSend(_front20fpsPrevCmdTbl);
		//i2cCmdTableSend(front20fpsPrevCmdTbl, 1, sizeof(front20fpsPrevCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	}
	else if ( frontPrevMode == 1 ) {  /* 30 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);	
		//i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	}
	else {  /* 60 fps. */
		frontResCmdBinSend(_front60fpsPrevCmdTbl);	
		//i2cCmdTableSend(front60fpsPrevCmdTbl, 1, sizeof(front60fpsPrevCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
	}

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

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
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
	UINT8 first,
	UINT8 mode,
	UINT8 scaleUp
)
{
	if ( first ) {   // will:  expcept for second time of burst capture
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		frontResCmdBinSend(_frontSnapCmdTbl);
		//i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
		#if (DBG_SNAP_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x10);
		#endif
		
		frontSnapExposureTimeSet(mode,scaleUp);
		
		#if (DBG_SNAP_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
		#endif
	}

	if (mode != SP1K_FRONT_DOFRONT){  //will: normal/burst capture
		#if 1 // normal capture
			HAL_FrontSnapTrig(1,1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
			cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
			cdspSkipWrSyncSet(1);			
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			#if (DBG_SNAP_TIMING == 1)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x20);
			#endif
			frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
			#if (DBG_SNAP_TIMING == 1)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
			#endif
			HAL_CdspEofWait(1);		

		#else /* Dark Sub */


		XBYTE[REG_Disp_DispEn_Flip] &=	0xfe;  /* turn off display */

			XBYTE[0x22EF] = 1;
			XBYTE[0x22EF] = 0;
			  
			frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);
			cdsp1stPathWriteSkip(0x0e, 0); 
			//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
			HAL_CdspEofWait(1); 	   

			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

			//HAL_CdspEofWait(1); 	   // next is second frame
			
			XBYTE[0x21EE] = 0xFF;
			XBYTE[0x21EF] = 0xFF;	 
			HAL_FrontSnapTrig(2,1);    // snap trigger	
			
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // next is first frame

			
			HAL_CdspEofWait(1); 	   // next is second frame
				
			XBYTE[0x2290] = 0x12;			/* Drak Enable */
			//HAL_DramRawFrmBufAddrSet(0x331c00); 	/* test, save second frame to dram */
			
			//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // next is first frame
			HAL_CdspEofWait(1); 	   // receiving second frame is done

			
				
			cdsp1stPathWriteSkip(0x0f, 0);
			//frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
			//cdsp1stPathWriteSkip(0x0f, 0);
			//cdsp1stPathWriteSkip(0x0e, 0);  
				
			//HAL_CdspEofWait(1); 	  

		

			XBYTE[REG_Disp_DispEn_Flip] |= 0x01;  /* turn on display */
			// test dark sub
			//XBYTE[0x2A3A] = 0x10; //Disable front gating
			//XBYTE[0x2277] = 0x0E; //Disable image skip write
			//eofwait();
			//XBYTE[0x21EE] = 0xFF;
			//XBYTE[0x21EF] = 0xFF;
			//XBYTE[0x2276] = 0x11; 			//Raw data 8 Bits
			//XBYTE[0x2A10] = 0x12; 			//Snap trigger
			//eofwait();
			//XBYTE[0x2290] = 0x12; 			//Drak Disable
			
			//eofwait();
			//XBYTE[0x2277] = 0x0F;
			printf("dark\n");
	  #endif		

    }
	else{   /* snap do flow */
		//printf("do front\n");
		//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		cdspSkipWrSet(CDSP_SKIP_ALL,0xff);
		cdspSkipWrSyncSet(1);	
		frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	}

	#if 0
		else if (mode == SENSOR_CAP_NORMAL_BPREALTIME){
			//printf("front bad pixel mode\n"); 	
		
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, 18, pcap->mclkPhase, 0, 0); /* lower clock to avoid front is too fast */
		
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
			cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
			cdspSkipWrSyncSet(1);		
			HAL_FrontSnapTrig(1,1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			XBYTE[0x2E00] = 0xFF;
			HAL_CdspStillBpTrigger(1);
			frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
			HAL_CdspEofWait(1);
			
			//printf("K Bad Pixel start type 2\n");
			while ( (XBYTE[0x2E00]&0x02) == 0x00){;}		
			//printf("K Bad Pixel finish type 2\n");	
		}
	#endif
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
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, isr, upd);

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
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, parg->isr, parg->upd);
}

/**
 * @brief	operating function to set gain value for intr.
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
frontOpIntrGainSet(
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

}

/**
 * @brief	operating function to set exposure time value for intr.
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
frontOpIntrExposureTimeSet(
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
 * @brief	operating function to set ae target luminance.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeTargetLuminanceSet(
	void *parg
)
{
	parg = parg;
	//printf("frontSensorOpAeTargetLuminanceSet=%bu\n",(UINT8)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set ae exposure compensation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-26
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeExposureCompensationSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAeExposureCompensationSet=%bu\n", (UINT8)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set ae frame rate.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeFrameRateSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAeFrameRateSet=%d\n", (UINT16)parg);
	// NOTE: use variable "val" for your purpose.
}

/**
 * @brief	operating function to set awb mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-27
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAwbModeSet(
	void *parg
)
{
	parg = parg;
	//printf("frontOpAwbModeSet=%bu\n", (UINT8)parg);
	// NOTE: use variable "mode" for your purpose.
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
	parg = parg;
	//printf("frontOpAfterSnapShot=%bu\n", (UINT8)parg);
	// NOTE: use variable "mode" for your purpose.
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
	#if (RES_LSC_LOAD == 1)
	/* resource file lsc cmd table send */
	frontResTblCmdSend(FRONT_RES_PARAM_LSC_ID);
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
 * @since	2008-09-17
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
	UINT16 s_gain, lastt_gain;
	//UINT32 GlbGainTmp;   //will mark

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


	espline = (pixclk * 20) / (ltotal * frontLastShutter);
	s_gain = frontLastGain;

  #if 0 //will mark
	if (Ggain_mode_En==2)
		{
			if (GlbGain>64)
				{
				GlbGainTmp = (((UINT32)s_gain&0x0f)+16)*(UINT32)GlbGain*4
							-((UINT32)GlbGain-64)*(OB_Sensor*(((UINT32)s_gain&0x0f)+16)+OB_CDSP);
				if (GlbGainTmp<((((UINT32)s_gain&0x0f)+16)*64*4))
					GlbGainTmp = (((UINT32)s_gain&0x0f)+16)*64*4;
				}
			else
				GlbGainTmp = (((UINT32)s_gain&0x0f)+16)*(UINT32)GlbGain*4;
			s_gain = (UINT16)(GlbGainTmp >> 6) - 16;
			if (s_gain>=0x03f0)
				s_gain = 0x03f0;
		}
  #endif 	
	if (s_gain >= 0x1f0)
		{
		lastt_gain = s_gain/32 - 15;       // (s_gain+16)/32 - 16  /*0x1f0 / 32 = 15*/
		espline = espline * 32;
		}
	else if (s_gain >= 0xf0)
		{
		lastt_gain = s_gain/16 - 15;       // (s_gain+16)/16 - 16
		espline = espline * 16;
		}
	else if (s_gain >= 0x70)
		{
		lastt_gain = s_gain/8 - 14;       // (s_gain+16)/8 - 16 
		espline = espline * 8;
		}
	
	else if ( s_gain >= 0x30 ) {
	  #if 0	   //will mark
		if (Ggain_mode_En==2)
			lastt_gain = s_gain/4 - 12;       // (s_gain+16)/4 - 16 
		else
	  #endif		
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
      #if 0  //will mark 
		if (Ggain_mode_En==2)
		{
			FrontSnapGlbGainSet();
			halCdspWBGainSet( 1 );
		}
	  #endif	
	//cdspColorMatrixRestore();  //will : Don't change color Matrix
    //XBYTE[0x21D3] = 0x02;
    //XBYTE[0x21D0] = 0x02;

	if ( espline >= ftotal ) {
		tmp0 = espline - ftotal;
		espline = ftotal - 1;
	}
	else {
		tmp0 = 0;
	};

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
	//i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_BURST, 0, 0);
}

/**
 * @brief	Before Snap Exposure Time Set.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontBeforeSnapExposureTimeSet(
	void
)
{

}

/**
 * @fn        void frontResCmdBinSend(UINT16 fileId)
 * @brief	front Resource File Table Cmd Send.
 * @param	fileId = [in] resource file id...
 * @retval	None.
 * @see
 * @author	LinJieCheng
 * @since	2010-12-07
 * @todo		N/A
 * @bug		N/A
*/
static void
frontResCmdBinSend(
	UINT16 fileId
)
{
	UINT32 size;
	UINT8 sts;
	//UINT32 i;
	
	//printf("fileId = 0x%bx\n",fileId);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_CMD_MAX_SIZE) {
		ASSERT(size > FRONT_RES_CMD_MAX_SIZE, 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize+(((UINT16)G_FPGAMemPool - 0x4000)>>1))<<1));
	}while(!sts);

	//for (i = 0; i < size; i++) {
	//	printf("0x%02bx,",G_FPGAMemPool[i]);
	//}
	
	i2cCmdTableSend(G_FPGAMemPool, 1, size/2, I2C_TRANS_MODE_BURST, 0, 0);	
}

/**
 * @fn        void frontResTblBinRead(UINT16 fileId)
 * @brief	front Resource File Table Bin Read.
 * @param	fileId = [in] resource file id...
 * @retval	None.
 * @see
 * @author	LinJieCheng
 * @since	2010-12-07
 * @todo		N/A
 * @bug		N/A
*/
static void
frontResTblBinRead(
	UINT16 fileId,
	UINT16 address
)
{
	UINT32 size;
	UINT8 sts;
	
	//printf("fileId = 0x%04x, address = 0x%04x\n",fileId,address);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(size > FRONT_RES_TBL_MAX_SIZE, 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((address - 0x4000) >> 1)) << 1));
	}while(!sts);	
}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	TQ Zhang
 * @since	2009-04-03
 * @todo 	N/A
 * @bug		N/A
*/
static void
FrontSnapGlbGainSet()
{

#if 0
UINT16 RGain,GrGain,BGain,GbGain;
	#if 1
	RGain  = ((XBYTE[ 0x2155 ])  <<8) + XBYTE[ 0x2154 ];
	GrGain = ((XBYTE[ 0x2157 ])<<8) + XBYTE[ 0x2156 ];
	BGain  = ((XBYTE[ 0x2159 ])  <<8) + XBYTE[ 0x2158 ];
	GbGain= ((XBYTE[ 0x215b ])<<8) + XBYTE[ 0x215a ];
#if 0   //will mark
	RGain = (RGain * 32) / (GlbGain>>1);
	GrGain = (GrGain * 32) / (GlbGain>>1);
	BGain = (BGain * 32) / (GlbGain>>1);
	GbGain = (GbGain * 32) / (GlbGain>>1);
#endif

	RGain = RGain>0x7ff? 0x7ff: RGain;
	GrGain = GrGain>0x7ff? 0x7ff: GrGain;
	BGain = BGain>0x7ff? 0x7ff: BGain;
	GbGain = GbGain>0x7ff? 0x7ff: GbGain;

	XBYTE[ 0x2154 ] = LowByte( RGain );
	XBYTE[ 0x2155 ] = HighByte( RGain );

	XBYTE[ 0x2156 ] = LowByte( GrGain );
	XBYTE[ 0x2157 ] = HighByte( GrGain );

	XBYTE[ 0x2158 ] = LowByte( BGain );
	XBYTE[ 0x2159 ] = HighByte( BGain );

	XBYTE[ 0x215a ] = LowByte( GbGain );
	XBYTE[ 0x215b ] = HighByte( GbGain );
	#else 
	XBYTE[ 0x2154 ] = LowByte( 64 );
	XBYTE[ 0x2155 ] = HighByte( 64 );

	XBYTE[ 0x2156 ] = LowByte( 64 );
	XBYTE[ 0x2157 ] = HighByte( 64 );

	XBYTE[ 0x2158 ] = LowByte( 64 );
	XBYTE[ 0x2159 ] = HighByte( 64 );

	XBYTE[ 0x215a ] = LowByte( 64 );
	XBYTE[ 0x215b ] = HighByte( 64 );
	#endif
#endif	
}


