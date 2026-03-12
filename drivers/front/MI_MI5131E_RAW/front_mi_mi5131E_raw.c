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
 * @file		front_mi_mi5130_raw_rev4.c
 * @brief		front sensor micron mi5130 driver.
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
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "dbg_def.h"
#include "hal_dram.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "ae_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "sp1k_front_api.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE     __FILE_ID_SENSOR_DRIVER__

#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define RES_LSC_LOAD		0//after power on load lsc resource file
/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  1

/* dimensiion. */

#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       2
#define YOFF_30FPS_PREV       2
#define LINE_TOT_30FPS_PREV 1856//1908//1908// 1856//1868//1864//1862//1516//1908//1698//1908//2460//3791
#define FRAME_TOT_30FPS_PREV  1065//1199//1148//1125//1043//1055

#if 1
#define WIDTH_60FPS_PREV      640//636
#define HEIGHT_60FPS_PREV     480//476 
#define XOFF_60FPS_PREV       2
#define YOFF_60FPS_PREV       2
#define LINE_TOT_60FPS_PREV   1456//1461//1471//1456//1458
#define FRAME_TOT_60FPS_PREV  611//580//570//612//612//567//686
#endif
	

#define WIDTH_SNAP      2592 
#define HEIGHT_SNAP     1944 
#define XOFF_SNAP       2
#define YOFF_SNAP       2
#define LINE_TOT_SNAP    2688//2692//2673//5214//2352//2100
#define FRAME_TOT_SNAP  2146//1552

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    22//
#define MCLK_PHASE_30FPS_PREV  0
#define MCLK_DRV_30FPS_PREV    1
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  7

#if 1
#define MCLK_DIV_60FPS_PREV    22//
#define MCLK_PHASE_60FPS_PREV  0
#define MCLK_DRV_60FPS_PREV    1
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  7
#endif

#define MCLK_DIV_SNAP           22//11//4
#define MCLK_DIV_SNAP_BURST     22
#define MCLK_DIV_SNAP_BURST_UP  22
#define MCLK_PHASE_SNAP         0
#define MCLK_DRV_SNAP           1
#define PCLK_DIV_SNAP           1
#define PCLK_PHASE_SNAP         7

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV 59315000 //64060000L//24000000L////30000000L
#if 1
#define PCLK_FREQ_60FPS_PREV  53390000L
#endif
#define PCLK_FREQ_SNAP        	59315000L//24000000L//30000000L
#define PCLK_FREQ_SNAP_BURST  	41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_30FPS_PREV  522000000L /2/ PREV_MCLKDIV / PREV_PCLKDIV
#if 1
#define PCLK_FREQ_60FPS_PREV  522000000L/2 / PREV_MCLKDIV / PREV_PCLKDIV
#endif
#define PCLK_FREQ_SNAP        522000000L/2 / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    0
#else
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV  1
#define RESHAPE_HRISE_30FPS_PREV  2
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_30FPS_PREV  1
#define RESHAPE_VRISE_30FPS_PREV  2

#if 1
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_60FPS_PREV    0
#else
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_60FPS_PREV  1
#define RESHAPE_HRISE_60FPS_PREV  2
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_60FPS_PREV  1
#define RESHAPE_VRISE_60FPS_PREV  2
#endif

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  1
#define RESHAPE_HRISE_SNAP  2
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x02
#define BAYER_PTN_SNAP  0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0x6c//0x20
#define I2C_CLK_DIV   0x01//0x03

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  140
#define AE_30FPS_30_MIN_IDX  10//14
#define AE_30FPS_25_MAX_IDX  140
#define AE_30FPS_25_MIN_IDX  10//14
#define EV10_30FPS_EXP_IDX   90

#if 1
#define AE_60FPS_30_MAX_IDX  128
#define AE_60FPS_30_MIN_IDX  0//14
#define AE_60FPS_25_MAX_IDX  136
#define AE_60FPS_25_MIN_IDX  0//14
#define EV10_60FPS_EXP_IDX   90
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  239//223
#define GAIN_30FPS_MIN_IDX  0
#if 1
#define GAIN_60FPS_MAX_IDX  239
#define GAIN_60FPS_MIN_IDX  0
#endif

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor


#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON

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
static void frontResTblBinRead(UINT16 fileId,	UINT16 address);
static void frontResCmdBinSend(UINT16 fileId);
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - MI MI5131E RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table
 *					CvtTypeLnkAddr: convert to 
 **************************************************************************/

#define CVT_DEF  extern

/* Tag for convert start */
CVT_DEF UINT8 TAG_CVT_S;

/* ae table resource file id  */
typedef enum CvtTypeToX_u_0 {
	//_ae15fps50hzTbl = 0x00C0,
	//_ae15fps60hzTbl = 0x00C1,
	//_ae20fps50hzTbl = 0x00C2,
	//_ae20fps60hzTbl = 0x00C3,
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
	//_front20fpsPrevCmdTbl	= 0x00D1,
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
/*static code frontAe_t ae20fps50hzTbl[1] = {
	{      0,    0,     0,      0,0 }
};
static code frontAe_t ae20fps60hzTbl[1] = {
	{	   0,	 0, 	0,		0,0 }
};*/


static code frontAe_t ae30fps50hzTbl[143] = {
	{	0,	333,	236,	0,	0 },
	{	1,	333,	231,	0,	0 },
	{	2,	333,	227,	0,	0 },
	{	3,	333,	220,	0,	0 },
	{	4,	333,	212,	0,	0 },
	{	5,	333,	204,	0,	0 },
	{	6,	333,	197,	0,	0 },
	{	7,	333,	190,	0,	0 },
	{	8,	333,	184,	0,	0 },
	{	9,	333,	178,	0,	0 },
	{	10,	333,	172,	0,	0 },
	{	11,	333,	167,	0,	0 },
	{	12,	333,	163,	0,	0 },
	{	13,	333,	156,	0,	0 },
	{	14,	333,	148,	0,	0 },
	{	15,	333,	140,	0,	0 },
	{	16,	333,	133,	0,	0 },
	{	17,	333,	126,	0,	0 },
	{	18,	333,	120,	0,	0 },
	{	19,	333,	114,	0,	0 },
	{	20,	333,	108,	0,	0 },
	{	21,	333,	103,	0,	0 },
	{	22,	333,	99,	0,	0 },
	{	23,	333,	92,	0,	0 },
	{	24,	333,	84,	0,	0 },
	{	25,	333,	76,	0,	0 },
	{	26,	333,	69,	0,	0 },
	{	27,	333,	62,	0,	0 },
	{	28,	333,	56,	0,	0 },
	{	29,	333,	50,	0,	0 },
	{	30,	333,	44,	0,	0 },
	{	31,	333,	39,	0,	0 },
	{	32,	333,	35,	0,	0 },
	{	33,	333,	30,	0,	0 },
	{	34,	333,	26,	0,	0 },
	{	35,	333,	22,	0,	0 },
	{	36,	333,	19,	0,	0 },
	{	37,	333,	15,	0,	0 },
	{	38,	333,	12,	0,	0 },
	{	39,	333,	9,	0,	0 },
	{	40,	333,	6,	0,	0 },
	{	41,	333,	4,	0,	0 },
	{	42,	333,	2,	0,	0 },
	{	43,	333,	0,	0,	0 },
	{	44,	498,	12,	0,	0 },
	{	45,	498,	9,	0,	0 },
	{	46,	498,	6,	0,	0 },
	{	47,	498,	4,	0,	0 },
	{	48,	498,	1,	0,	0 },
	{	49,	498,	0,	0,	0 },
	{	50,	993,	25,	0,	0 },
	{	51,	993,	21,	0,	0 },
	{	52,	993,	18,	0,	0 },
	{	53,	993,	15,	0,	0 },
	{	54,	993,	11,	0,	0 },
	{	55,	993,	9,	0,	0 },
	{	56,	993,	6,	0,	0 },
	{	57,	993,	3,	0,	0 },
	{	58,	993,	1,	0,	0 },
	{	59,	993,	0,	0,	0 },
	{	60,	1058,	0,	0,	0 },
	{	61,	1133,	0,	0,	0 },
	{	62,	1215,	0,	0,	0 },
	{	63,	1299,	0,	0,	0 },
	{	64,	1396,	0,	0,	0 },
	{	65,	1493,	0,	0,	0 },
	{	66,	1606,	0,	0,	0 },
	{	67,	1718,	0,	0,	0 },
	{	68,	1847,	0,	0,	0 },
	{	69,	1973,	0,	0,	0 },
	{	70,	2116,	0,	0,	0 },
	{	71,	2267,	0,	0,	0 },
	{	72,	2440,	0,	0,	0 },
	{	73,	2598,	0,	0,	0 },
	{	74,	2803,	0,	0,	0 },
	{	75,	2987,	0,	0,	0 },
	{	76,	3228,	0,	0,	0 },
	{	77,	3436,	0,	0,	0 },
	{	78,	3716,	0,	0,	0 },
	{	79,	3945,	0,	0,	0 },
	{	80,	4261,	0,	0,	0 },
	{	81,	4565,	0,	0,	0 },
	{	82,	4917,	0,	0,	0 },
	{	83,	5239,	0,	0,	0 },
	{	84,	5607,	0,	0,	0 },
	{	85,	6030,	0,	0,	0 },
	{	86,	6522,	0,	0,	0 },
	{	87,	6947,	0,	0,	0 },
	{	88,	7432,	0,	0,	0 },
	{	89,	7990,	0,	0,	0 },
	{	90,	8637,	0,	0,	0 },
	{	91,	9131,	0,	0,	0 },
	{	92,	9987,	0,	0,	0 },
	{	93,	10653,	0,	0,	0 },
	{	94,	11414,	0,	0,	0 },
	{	95,	12292,	0,	0,	0 },
	{	96,	13316,	0,	0,	0 },
	{	97,	13895,	0,	0,	0 },
	{	98,	15218,	0,	0,	0 },
	{	99,	15979,	0,	0,	0 },
	{	100,	17755,	0,	0,	0 },
	{	101,	18799,	0,	0,	1 },
	{	102,	19974,	0,	0,	0 },
	{	103,	21306,	0,	0,	0 },
	{	104,	22827,	0,	0,	1 },
	{	105,	24583,	0,	0,	1 },
	{	106,	26632,	0,	0,	0 },
	{	107,	29053,	0,	0,	0 },
	{	108,	31958,	1,	0,	1 },
	{	109,	31958,	0,	0,	1 },
	{	110,	35509,	0,	0,	1 },
	{	111,	39948,	2,	0,	0 },
	{	112,	39948,	0,	0,	0 },
	{	113,	45654,	2,	0,	1 },
	{	114,	45654,	0,	0,	1 },
	{	115,	53263,	2,	0,	1 },
	{	116,	53263,	0,	0,	1 },
	{	117,	63916,	4,	0,	2 },
	{	118,	63916,	1,	0,	2 },
	{	119,	63916,	0,	0,	2 },
	{	120,	79894,	4,	0,	0 },
	{	121,	79894,	2,	0,	0 },
	{	122,	79894,	0,	0,	0 },
	{	123,	106525,	7,	0,	1 },
	{	124,	106525,	5,	0,	1 },
	{	125,	106525,	2,	0,	1 },
	{	126,	106525,	0,	0,	1 },
	{	127,	159785,	12,	0,	3 },
	{	128,	159785,	9,	0,	3 },
	{	129,	159785,	7,	0,	3 },
	{	130,	159785,	4,	0,	3 },
	{	131,	159785,	2,	0,	3 },
	{	132,	159785,	0,	0,	3 },
	{	133,	319553,	26,	0,	12 },
	{	134,	319553,	23,	0,	12 },
	{	135,	319553,	19,	0,	12 },
	{	136,	319553,	16,	0,	12 },
	{	137,	319553,	12,	0,	12 },
	{	138,	319553,	9,	0,	12 },
	{	139,	319553,	7,	0,	12 },
	{	140,	319553,	4,	0,	12 },
	{	141,	319553,	2,	0,	12 },
	{	142,	319553,	0,	0,	12 },
};

static code frontAe_t ae30fps60hzTbl[145] = {
	{	0,	300,	239,	0,	0 },
	{	1,	300,	234,	0,	0 },
	{	2,	300,	229,	0,	0 },
	{	3,	300,	224,	0,	0 },
	{	4,	300,	216,	0,	0 },
	{	5,	300,	208,	0,	0 },
	{	6,	300,	200,	0,	0 },
	{	7,	300,	193,	0,	0 },
	{	8,	300,	187,	0,	0 },
	{	9,	300,	181,	0,	0 },
	{	10,	300,	175,	0,	0 },
	{	11,	300,	170,	0,	0 },
	{	12,	300,	165,	0,	0 },
	{	13,	300,	160,	0,	0 },
	{	14,	300,	152,	0,	0 },
	{	15,	300,	144,	0,	0 },
	{	16,	300,	136,	0,	0 },
	{	17,	300,	129,	0,	0 },
	{	18,	300,	123,	0,	0 },
	{	19,	300,	117,	0,	0 },
	{	20,	300,	111,	0,	0 },
	{	21,	300,	106,	0,	0 },
	{	22,	300,	101,	0,	0 },
	{	23,	300,	96,	0,	0 },
	{	24,	300,	88,	0,	0 },
	{	25,	300,	80,	0,	0 },
	{	26,	300,	72,	0,	0 },
	{	27,	300,	65,	0,	0 },
	{	28,	300,	59,	0,	0 },
	{	29,	300,	53,	0,	0 },
	{	30,	300,	47,	0,	0 },
	{	31,	300,	42,	0,	0 },
	{	32,	300,	37,	0,	0 },
	{	33,	300,	32,	0,	0 },
	{	34,	300,	28,	0,	0 },
	{	35,	300,	24,	0,	0 },
	{	36,	300,	20,	0,	0 },
	{	37,	300,	17,	0,	0 },
	{	38,	300,	14,	0,	0 },
	{	39,	300,	11,	0,	0 },
	{	40,	300,	8,	0,	0 },
	{	41,	300,	5,	0,	0 },
	{	42,	300,	3,	0,	0 },
	{	43,	300,	0,	0,	0 },
	{	44,	399,	8,	0,	0 },
	{	45,	399,	6,	0,	0 },
	{	46,	399,	3,	0,	0 },
	{	47,	399,	1,	0,	0 },
	{	48,	400,	0,	0,	0 },
	{	49,	597,	11,	0,	0 },
	{	50,	597,	8,	0,	0 },
	{	51,	597,	5,	0,	0 },
	{	52,	597,	3,	0,	0 },
	{	53,	597,	0,	0,	0 },
	{	54,	1192,	28,	0,	0 },
	{	55,	1192,	24,	0,	0 },
	{	56,	1192,	20,	0,	0 },
	{	57,	1192,	17,	0,	0 },
	{	58,	1192,	13,	0,	0 },
	{	59,	1192,	10,	0,	0 },
	{	60,	1192,	8,	0,	0 },
	{	61,	1192,	5,	0,	0 },
	{	62,	1192,	3,	0,	0 },
	{	63,	1192,	0,	0,	0 },
	{	64,	1215,	0,	0,	0 },
	{	65,	1299,	0,	0,	0 },
	{	66,	1396,	0,	0,	0 },
	{	67,	1493,	0,	0,	0 },
	{	68,	1606,	0,	0,	0 },
	{	69,	1718,	0,	0,	0 },
	{	70,	1847,	0,	0,	0 },
	{	71,	1973,	0,	0,	0 },
	{	72,	2116,	0,	0,	0 },
	{	73,	2267,	0,	0,	0 },
	{	74,	2440,	0,	0,	0 },
	{	75,	2598,	0,	0,	0 },
	{	76,	2803,	0,	0,	0 },
	{	77,	2987,	0,	0,	0 },
	{	78,	3228,	0,	0,	0 },
	{	79,	3436,	0,	0,	0 },
	{	80,	3716,	0,	0,	0 },
	{	81,	3945,	0,	0,	0 },
	{	82,	4261,	0,	0,	0 },
	{	83,	4565,	0,	0,	0 },
	{	84,	4917,	0,	0,	0 },
	{	85,	5239,	0,	0,	0 },
	{	86,	5607,	0,	0,	0 },
	{	87,	6030,	0,	0,	0 },
	{	88,	6522,	0,	0,	0 },
	{	89,	6947,	0,	0,	0 },
	{	90,	7432,	0,	0,	0 },
	{	91,	7990,	0,	0,	0 },
	{	92,	8637,	0,	0,	0 },
	{	93,	9131,	0,	0,	0 },
	{	94,	9987,	0,	0,	0 },
	{	95,	10653,	0,	0,	0 },
	{	96,	11414,	0,	0,	0 },
	{	97,	12292,	0,	0,	0 },
	{	98,	13316,	0,	0,	0 },
	{	99,	13895,	0,	0,	0 },
	{	100,	15218,	0,	0,	0 },
	{	101,	15979,	0,	0,	0 },
	{	102,	17755,	0,	0,	0 },
	{	103,	18799,	0,	0,	1 },
	{	104,	19974,	0,	0,	0 },
	{	105,	21306,	0,	0,	0 },
	{	106,	22827,	0,	0,	1 },
	{	107,	24583,	0,	0,	1 },
	{	108,	26632,	0,	0,	0 },
	{	109,	29053,	0,	0,	0 },
	{	110,	31958,	1,	0,	1 },
	{	111,	31958,	0,	0,	1 },
	{	112,	35509,	0,	0,	1 },
	{	113,	39948,	2,	0,	0 },
	{	114,	39948,	0,	0,	0 },
	{	115,	45654,	2,	0,	1 },
	{	116,	45654,	0,	0,	1 },
	{	117,	53263,	2,	0,	1 },
	{	118,	53263,	0,	0,	1 },
	{	119,	63916,	4,	0,	2 },
	{	120,	63916,	1,	0,	2 },
	{	121,	63916,	0,	0,	2 },
	{	122,	79894,	4,	0,	0 },
	{	123,	79894,	2,	0,	0 },
	{	124,	79894,	0,	0,	0 },
	{	125,	106525,	7,	0,	1 },
	{	126,	106525,	5,	0,	1 },
	{	127,	106525,	2,	0,	1 },
	{	128,	106525,	0,	0,	1 },
	{	129,	159785,	12,	0,	3 },
	{	130,	159785,	9,	0,	3 },
	{	131,	159785,	7,	0,	3 },
	{	132,	159785,	4,	0,	3 },
	{	133,	159785,	2,	0,	3 },
	{	134,	159785,	0,	0,	3 },
	{	135,	319553,	26,	0,	12 },
	{	136,	319553,	23,	0,	12 },
	{	137,	319553,	19,	0,	12 },
	{	138,	319553,	16,	0,	12 },
	{	139,	319553,	12,	0,	12 },
	{	140,	319553,	9,	0,	12 },
	{	141,	319553,	7,	0,	12 },
	{	142,	319553,	4,	0,	12 },
	{	143,	319553,	2,	0,	12 },
	{	144,	319553,	0,	0,	12 },
};


#if 1
static code frontAe_t ae60fps50hzTbl[129] = {
{	0,	991,	235,	0,	0 },
	{	1,	991,	230,	0,	0 },
	{	2,	991,	226,	0,	0 },
	{	3,	991,	218,	0,	0 },
	{	4,	991,	210,	0,	0 },
	{	5,	991,	202,	0,	0 },
	{	6,	991,	195,	0,	0 },
	{	7,	991,	189,	0,	0 },
	{	8,	991,	182,	0,	0 },
	{	9,	991,	177,	0,	0 },
	{	10,	991,	171,	0,	0 },
	{	11,	991,	166,	0,	0 },
	{	12,	991,	162,	0,	0 },
	{	13,	991,	154,	0,	0 },
	{	14,	991,	146,	0,	0 },
	{	15,	991,	138,	0,	0 },
	{	16,	991,	131,	0,	0 },
	{	17,	991,	125,	0,	0 },
	{	18,	991,	118,	0,	0 },
	{	19,	991,	113,	0,	0 },
	{	20,	991,	107,	0,	0 },
	{	21,	991,	102,	0,	0 },
	{	22,	991,	98,	0,	0 },
	{	23,	991,	90,	0,	0 },
	{	24,	991,	82,	0,	0 },
	{	25,	991,	74,	0,	0 },
	{	26,	991,	67,	0,	0 },
	{	27,	991,	61,	0,	0 },
	{	28,	991,	54,	0,	0 },
	{	29,	991,	49,	0,	0 },
	{	30,	991,	43,	0,	0 },
	{	31,	991,	38,	0,	0 },
	{	32,	991,	34,	0,	0 },
	{	33,	991,	29,	0,	0 },
	{	34,	991,	25,	0,	0 },
	{	35,	991,	21,	0,	0 },
	{	36,	991,	18,	0,	0 },
	{	37,	991,	15,	0,	0 },
	{	38,	991,	11,	0,	0 },
	{	39,	991,	9,	0,	0 },
	{	40,	991,	6,	0,	0 },
	{	41,	991,	3,	0,	0 },
	{	42,	991,	1,	0,	0 },
	{	43,	991,	0,	0,	0 },
	{	44,	1057,	0,	0,	0 },
	{	45,	1132,	0,	0,	0 },
	{	46,	1214,	0,	0,	0 },
	{	47,	1300,	0,	0,	0 },
	{	48,	1394,	0,	0,	0 },
	{	49,	1491,	0,	0,	0 },
	{	50,	1601,	0,	0,	0 },
	{	51,	1714,	0,	0,	0 },
	{	52,	1843,	0,	0,	0 },
	{	53,	1971,	0,	0,	0 },
	{	54,	2120,	0,	0,	0 },
	{	55,	2264,	0,	0,	0 },
	{	56,	2428,	0,	0,	0 },
	{	57,	2601,	0,	0,	0 },
	{	58,	2799,	0,	0,	0 },
	{	59,	2981,	0,	0,	0 },
	{	60,	3217,	0,	0,	0 },
	{	61,	3427,	0,	0,	0 },
	{	62,	3704,	0,	0,	0 },
	{	63,	3943,	0,	0,	0 },
	{	64,	4264,	0,	0,	0 },
	{	65,	4527,	0,	0,	0 },
	{	66,	4889,	0,	0,	0 },
	{	67,	5238,	0,	0,	0 },
	{	68,	5641,	0,	0,	0 },
	{	69,	6011,	0,	0,	0 },
	{	70,	6433,	0,	0,	0 },
	{	71,	6919,	0,	0,	0 },
	{	72,	7483,	0,	0,	0 },
	{	73,	7971,	0,	0,	0 },
	{	74,	8528,	0,	0,	0 },
	{	75,	9167,	0,	0,	0 },
	{	76,	9911,	0,	0,	0 },
	{	77,	10477,	0,	0,	0 },
	{	78,	11459,	0,	0,	0 },
	{	79,	12223,	0,	0,	0 },
	{	80,	13096,	0,	0,	0 },
	{	81,	14103,	0,	0,	0 },
	{	82,	15279,	0,	0,	0 },
	{	83,	15943,	0,	0,	0 },
	{	84,	17461,	0,	0,	0 },
	{	85,	18334,	0,	0,	0 },
	{	86,	20372,	0,	0,	0 },
	{	87,	21570,	0,	0,	1 },
	{	88,	22918,	0,	0,	0 },
	{	89,	24446,	0,	0,	0 },
	{	90,	26192,	0,	0,	1 },
	{	91,	28207,	0,	0,	0 },
	{	92,	30557,	0,	0,	1 },
	{	93,	33335,	0,	0,	1 },
	{	94,	36669,	1,	0,	0 },
	{	95,	36669,	0,	0,	0 },
	{	96,	40743,	0,	0,	1 },
	{	97,	45836,	2,	0,	0 },
	{	98,	45836,	0,	0,	0 },
	{	99,	52383,	2,	0,	2 },
	{	100,	52383,	0,	0,	2 },
	{	101,	61114,	2,	0,	1 },
	{	102,	61114,	0,	0,	1 },
	{	103,	73336,	4,	0,	0 },
	{	104,	73336,	1,	0,	0 },
	{	105,	73336,	0,	0,	0 },
	{	106,	91670,	4,	0,	2 },
	{	107,	91670,	2,	0,	2 },
	{	108,	91670,	0,	0,	2 },
	{	109,	122226,	7,	0,	1 },
	{	110,	122226,	5,	0,	1 },
	{	111,	122226,	2,	0,	1 },
	{	112,	122226,	0,	0,	1 },
	{	113,	183336,	12,	0,	7 },
	{	114,	183336,	9,	0,	7 },
	{	115,	183336,	7,	0,	7 },
	{	116,	183336,	4,	0,	7 },
	{	117,	183336,	2,	0,	7 },
	{	118,	183336,	0,	0,	7 },
	{	119,	366653,	26,	0,	7 },
	{	120,	366653,	23,	0,	7 },
	{	121,	366653,	19,	0,	7 },
	{	122,	366653,	16,	0,	7 },
	{	123,	366653,	12,	0,	7 },
	{	124,	366653,	9,	0,	7 },
	{	125,	366653,	7,	0,	7 },
	{	126,	366653,	4,	0,	7 },
	{	127,	366653,	2,	0,	7 },
	{	128,	366653,	0,	0,	7 },
};


static code frontAe_t ae60fps60hzTbl[137] = {
	{	0,	597,	239,	0,	0 },
	{	1,	597,	234,	0,	0 },
	{	2,	597,	229,	0,	0 },
	{	3,	597,	224,	0,	0 },
	{	4,	597,	215,	0,	0 },
	{	5,	597,	207,	0,	0 },
	{	6,	597,	200,	0,	0 },
	{	7,	597,	193,	0,	0 },
	{	8,	597,	187,	0,	0 },
	{	9,	597,	181,	0,	0 },
	{	10,	597,	175,	0,	0 },
	{	11,	597,	170,	0,	0 },
	{	12,	597,	165,	0,	0 },
	{	13,	597,	160,	0,	0 },
	{	14,	597,	151,	0,	0 },
	{	15,	597,	143,	0,	0 },
	{	16,	597,	136,	0,	0 },
	{	17,	597,	129,	0,	0 },
	{	18,	597,	123,	0,	0 },
	{	19,	597,	117,	0,	0 },
	{	20,	597,	111,	0,	0 },
	{	21,	597,	106,	0,	0 },
	{	22,	597,	101,	0,	0 },
	{	23,	597,	96,	0,	0 },
	{	24,	597,	87,	0,	0 },
	{	25,	597,	79,	0,	0 },
	{	26,	597,	72,	0,	0 },
	{	27,	597,	65,	0,	0 },
	{	28,	597,	59,	0,	0 },
	{	29,	597,	53,	0,	0 },
	{	30,	597,	47,	0,	0 },
	{	31,	597,	42,	0,	0 },
	{	32,	597,	37,	0,	0 },
	{	33,	597,	32,	0,	0 },
	{	34,	597,	28,	0,	0 },
	{	35,	597,	24,	0,	0 },
	{	36,	597,	20,	0,	0 },
	{	37,	597,	17,	0,	0 },
	{	38,	597,	14,	0,	0 },
	{	39,	597,	11,	0,	0 },
	{	40,	597,	8,	0,	0 },
	{	41,	597,	5,	0,	0 },
	{	42,	597,	3,	0,	0 },
	{	43,	597,	0,	0,	0 },
	{	44,	1191,	28,	0,	0 },
	{	45,	1191,	24,	0,	0 },
	{	46,	1191,	20,	0,	0 },
	{	47,	1191,	17,	0,	0 },
	{	48,	1191,	13,	0,	0 },
	{	49,	1191,	10,	0,	0 },
	{	50,	1191,	8,	0,	0 },
	{	51,	1191,	5,	0,	0 },
	{	52,	1191,	3,	0,	0 },
	{	53,	1191,	0,	0,	0 },
	{	54,	1214,	0,	0,	0 },
	{	55,	1300,	0,	0,	0 },
	{	56,	1394,	0,	0,	0 },
	{	57,	1491,	0,	0,	0 },
	{	58,	1601,	0,	0,	0 },
	{	59,	1714,	0,	0,	0 },
	{	60,	1843,	0,	0,	0 },
	{	61,	1971,	0,	0,	0 },
	{	62,	2120,	0,	0,	0 },
	{	63,	2264,	0,	0,	0 },
	{	64,	2428,	0,	0,	0 },
	{	65,	2601,	0,	0,	0 },
	{	66,	2799,	0,	0,	0 },
	{	67,	2981,	0,	0,	0 },
	{	68,	3217,	0,	0,	0 },
	{	69,	3427,	0,	0,	0 },
	{	70,	3704,	0,	0,	0 },
	{	71,	3943,	0,	0,	0 },
	{	72,	4264,	0,	0,	0 },
	{	73,	4527,	0,	0,	0 },
	{	74,	4889,	0,	0,	0 },
	{	75,	5238,	0,	0,	0 },
	{	76,	5641,	0,	0,	0 },
	{	77,	6011,	0,	0,	0 },
	{	78,	6433,	0,	0,	0 },
	{	79,	6919,	0,	0,	0 },
	{	80,	7483,	0,	0,	0 },
	{	81,	7971,	0,	0,	0 },
	{	82,	8528,	0,	0,	0 },
	{	83,	9167,	0,	0,	0 },
	{	84,	9911,	0,	0,	0 },
	{	85,	10477,	0,	0,	0 },
	{	86,	11459,	0,	0,	0 },
	{	87,	12223,	0,	0,	0 },
	{	88,	13096,	0,	0,	0 },
	{	89,	14103,	0,	0,	0 },
	{	90,	15279,	0,	0,	0 },
	{	91,	15943,	0,	0,	0 },
	{	92,	17461,	0,	0,	0 },
	{	93,	18334,	0,	0,	0 },
	{	94,	20372,	0,	0,	0 },
	{	95,	21570,	0,	0,	1 },
	{	96,	22918,	0,	0,	0 },
	{	97,	24446,	0,	0,	0 },
	{	98,	26192,	0,	0,	1 },
	{	99,	28207,	0,	0,	0 },
	{	100,	30557,	0,	0,	1 },
	{	101,	33335,	0,	0,	1 },
	{	102,	36669,	1,	0,	0 },
	{	103,	36669,	0,	0,	0 },
	{	104,	40743,	0,	0,	1 },
	{	105,	45836,	2,	0,	0 },
	{	106,	45836,	0,	0,	0 },
	{	107,	52383,	2,	0,	2 },
	{	108,	52383,	0,	0,	2 },
	{	109,	61114,	2,	0,	1 },
	{	110,	61114,	0,	0,	1 },
	{	111,	73336,	4,	0,	0 },
	{	112,	73336,	1,	0,	0 },
	{	113,	73336,	0,	0,	0 },
	{	114,	91670,	4,	0,	2 },
	{	115,	91670,	2,	0,	2 },
	{	116,	91670,	0,	0,	2 },
	{	117,	122226,	7,	0,	1 },
	{	118,	122226,	5,	0,	1 },
	{	119,	122226,	2,	0,	1 },
	{	120,	122226,	0,	0,	1 },
	{	121,	183336,	12,	0,	7 },
	{	122,	183336,	9,	0,	7 },
	{	123,	183336,	7,	0,	7 },
	{	124,	183336,	4,	0,	7 },
	{	125,	183336,	2,	0,	7 },
	{	126,	183336,	0,	0,	7 },
	{	127,	366653,	26,	0,	7 },
	{	128,	366653,	23,	0,	7 },
	{	129,	366653,	19,	0,	7 },
	{	130,	366653,	16,	0,	7 },
	{	131,	366653,	12,	0,	7 },
	{	132,	366653,	9,	0,	7 },
	{	133,	366653,	7,	0,	7 },
	{	134,	366653,	4,	0,	7 },
	{	135,	366653,	2,	0,	7 },
	{	136,	366653,	0,	0,	7 },
};

#endif

#if 0
static code UINT16 gainTbl[256] = {
	4128, 	4129, 	4130, 	4131, 	4132, 	4133, 	4134, 	4135, 	4136, 	4137, //10
	4138, 	4139, 	4140, 	4141, 	4142, 	4143, 	4144, 	4145, 	4146, 	4147, //20
	4148, 	4149, 	4150, 	4151, 	4152, 	4153, 	4154, 	4155, 	4156, 	4157, //30
	4158, 	4159, 	4160, 	4161, 	4162, 	4163, 	4164, 	4165, 	4166, 	4167, //40
	4168, 	4169, 	4170, 	4171, 	4172, 	4173, 	4174, 	4175, 	4176, 	4177, //50
	4178, 	4179, 	4180, 	4181, 	4182, 	4183, 	4184, 	4185, 	4186, 	4187, //60
	4188, 	4189, 	4190, 	4191, 	4192, 	4193, 	4194, 	4195, 	4196, 	4197, //70
	4198, 	4199, 	4200, 	4201, 	4202, 	4203, 	4204, 	4205, 	4206, 	4207, //80
	4208, 	4209, 	4210, 	4211, 	4212, 	4213, 	4214, 	4215, 	4216, 	4217, //90
	4218, 	4219, 	4220, 	4221, 	4222, 	4223, 	4288, 	4289, 	4290, 	4291, //100
	4292, 	4293, 	4294, 	4295, 	4296, 	4297, 	4298, 	4299, 	4300, 	4301, //110
	4302, 	4303, 	4304, 	4305, 	4306, 	4307, 	4308, 	4309, 	4310, 	4311, //120
	4312, 	4313, 	4314, 	4315, 	4316, 	4317, 	4318, 	4319, 	4320, 	4321, //130
	4322, 	4323, 	4324, 	4325, 	4326, 	4327, 	4328, 	4329, 	4330, 	4331, //140
	4332, 	4333, 	4334, 	4335, 	4336, 	4337, 	4338, 	4339, 	4340, 	4341, //150
	4342, 	4343, 	4344, 	4345, 	4346, 	4347, 	4348, 	4349, 	4350, 	4351, //160
	4544, 	4545, 	4546, 	4547, 	4548, 	4549, 	4550, 	4551, 	4552, 	4553, //170
	4554, 	4555, 	4556, 	4557, 	4558, 	4559, 	4560, 	4561, 	4562, 	4563, //180
	4564, 	4565, 	4566, 	4567, 	4568, 	4569, 	4570, 	4571, 	4572, 	4573, //190
	4574, 	4575, 	4576, 	4577, 	4578, 	4579, 	4580, 	4581, 	4582, 	4583, //200
	4584, 	4585, 	4586, 	4587, 	4588, 	4589, 	4590, 	4591, 	4592, 	4593, //210
	4594, 	4595, 	4596, 	4597, 	4598, 	4599, 	4600, 	4601, 	4602, 	4603, //220
	4604, 	4605, 	4606, 	4607, 	8640, 	8641, 	8642, 	8643, 	8644, 	8645, //230
	8646, 	8647, 	8648, 	8649, 	8650, 	8651, 	8652, 	8653, 	8654, 	8655, //240
	8656, 	8657, 	8658, 	8659, 	8660, 	8661, 	8662, 	8663, 	8664, 	8665, //250
	8666, 	8667, 	8668, 	8669, 	8670, 	8671, 	};

#endif


#if 1
static code UINT16 gainTbl[256] = {
	4128, 	4129, 	4130, 	4131, 	4132, 	4133, 	4134, 	4135, 	4136, 	4137, 
	4138, 	4139, 	4140, 	4141, 	4142, 	4143, 	4144, 	4145, 	4146, 	4147, 
	4148, 	4149, 	4150, 	4151, 	4152, 	4153, 	4154, 	4155, 	4156, 	4157, 
	4158, 	4159, 	4160, 	4161, 	4162, 	4163, 	4164, 	4165, 	4166, 	4167, 
	4168, 	4169, 	4170, 	4171, 	4172, 	4173, 	4174, 	4175, 	4176, 	4177, 
	4178, 	4179, 	4180, 	4181, 	4182, 	4183, 	4184, 	4185, 	4186, 	4187, 
	4188, 	4189, 	4190, 	4191, 	4192, 	4193, 	4194, 	4195, 	4196, 	4197, 
	4198, 	4199, 	4200, 	4201, 	4202, 	4203, 	4204, 	4205, 	4206, 	4207, 
	4208, 	4209, 	4210, 	4211, 	4212, 	4213, 	4214, 	4215, 	4216, 	4217, 
	4218, 	4219, 	4220, 	4221, 	4222, 	4223, 	4288, 	4289, 	4290, 	4291, 
	4292, 	4293, 	4294, 	4295, 	4296, 	4297, 	4298, 	4299, 	4300, 	4301, 
	4302, 	4303, 	4304, 	4305, 	4306, 	4307, 	4308, 	4309, 	4310, 	4311, 
	4312, 	4313, 	4314, 	4315, 	4316, 	4317, 	4318, 	4319, 	4320, 	4321, 
	4322, 	4323, 	4324, 	4325, 	4326, 	4327, 	4328, 	4329, 	4330, 	4331, 
	4332, 	4333, 	4334, 	4335, 	4336, 	4337, 	4338, 	4339, 	4340, 	4341, 
	4342, 	4343, 	4344, 	4345, 	4346, 	4347, 	4348, 	4349, 	4350, 	4351, 
	4544, 	4545, 	4546, 	4547, 	4548, 	4549, 	4550, 	4551, 	4552, 	4553, 
	4554, 	4555, 	4556, 	4557, 	4558, 	4559, 	4560, 	4561, 	4562, 	4563, 
	4564, 	4565, 	4566, 	4567, 	4568, 	4569, 	4570, 	4571, 	4572, 	4573, 
	4574, 	4575, 	4576, 	4577, 	4578, 	4579, 	4580, 	4581, 	4582, 	4583, 
	4584, 	4585, 	4586, 	4587, 	4588, 	4589, 	4590, 	4591, 	4592, 	4593, 
	4594, 	4595, 	4596, 	4597, 	4598, 	4599, 	4600, 	4601, 	4602, 	4603, 
	4604, 	4605, 	4606, 	4607, 	8640, 	8641, 	8642, 	8643, 	8644, 	8645, 
	8646, 	8647, 	8648, 	8649, 	8650, 	8651, 	8652, 	8653, 	8654, 	8655, 
	8656, 	8657, 	8658, 	8659, 	8660, 	8661, 	8662, 	8663, 	8664, 	8665, 
	8666, 	8667, 	8668, 	8669, 	8670, 	8671, 	};
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

#if 1
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
	#if 1
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
	#endif
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
	0x01,0x03,0x00,0x01, 	
};

static code UINT8 frontInit1CmdTbl[] = {
	0x30,0x1A,0x50,0xCc,	// reset_register
	
	0x30,0x64,0x08,0x05,	
	
	0x01,0x04,0x00,0x01,		
	0x31,0x6C, 0x24,0xf0,
	0x30,0x9a, 0xa5,0x00,
	0x30,0x88,0x6f,0xf0,
	0x30,0x44,0x85,0x90,	
	0x31,0xae,0x02,0x01,
	0x01,0x04,0x00,0x00,	

	//0x30,0x88,0x6e,0x61,
	//0x31,0x6C, 0xA4,0x10,
};

static code UINT8 frontInit2CmdTbl[] = {
	0x30,0x1A,0x50,0xcc,//0xcc,
};


static code UINT8 front30fpsPrevCmdTbl[] = {  // 1280x960
	0x01,0x03,0x00,0x01, 
		
	0x30,0x1A,0x50,0xC8,		

	0x30,0x64,0x08,0x05,	
	
	0x01,0x04,0x00,0x01,	
	0x31,0x6C, 0x24,0xf0,
	0x30,0x9a, 0xa5,0x00,
	0x30,0x88,0x6f,0xf0,
	0x30,0x44,0x85,0x90,	
	0x31,0xae,0x02,0x01,
	0x01,0x04,0x00,0x00,		

	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x01,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x32,	// pll_multiplier, Pixel clock 70Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x01,	//op_sys_clk_div///*不修改*/	

	0x01,0x04,0x00,0x01,	// hold 
	 
	0x03,0x4C,0x05,0x10,	// HSize	1296	0x0x510	// HSize	1280 0x500
	0x03,0x4E,0x03,0xCC,	// VSize	972	0x3CC	// VSize	960	0x3C0
	0x03,0x44,0x00,0x08,	// x addr start	8  
	0x03,0x46,0x00,0x08,	// y addr start	8
	0x03,0x48,0x0A,0x25,	// x addr end	2597   
	0x03,0x4A,0x07,0x9d,	// y addr end	1949	 
	0x30,0x40,0x14,0xC3,	// (xy-skip&2x_xybin, summing)	
	0x04,0x00,0x00,0x00,	// Scaling_Mode
	0x04,0x04,0x00,0x10,     //scale_m 	 	
	0x30,0x12,0x04,0x1C,	// coarse_integration_time	1052    
	0x30,0x10,0x01,0x5C,	// Fine Correction	348	   
	0x30,0x14,0x09,0xAF, 	// Fine Integration Time	2479 	  	
	0x03,0x40,0x04,0x1F,	// Frame Lines	1055
	//0x03,0x42,0x0E,0xCF,	// Line Lentth	3791
	0x03,0x42,0x0e,0x80,	// Line Lentth	3791
	//0x03,0x42,0x0E,0x00,
	0x01,0x04,0x00,0x00,	// de-hold  

#if 1
	0x30,0xE0, 0x77,0x02,
	0x30,0xE2, 0x5C,0x3E,
	0x30,0xE4, 0x75,0x5C,
	0x30,0xE6, 0x75,0x03,
	0x30,0xEA, 0x1F,0x06,
	0x30,0xEC, 0x3E,0x1F,
	0x30,0xF2, 0x75,0x03,
	0x30,0xF6, 0x09,0x00,
	0x30,0xF8, 0x09,0x00,
	0x30,0xFA, 0x09,0x00,
	0x30,0xFC, 0x09,0x00,
	0x30,0xFE, 0x43,0x39,
	0x31,0x00, 0x43,0x34,
	0x31,0x02, 0x5B,0x45,
	0x31,0x04, 0x72,0x5D,
	0x31,0x06, 0x1E,0x0A,
	0x31,0x08, 0x35,0x1F,
	0x31,0x0A, 0x75,0x02,
	0x31,0x0C, 0x75,0x02,
	0x31,0x0E, 0x75,0x0A,
	0x31,0x10, 0x72,0x01,
	0x31,0x12, 0x75,0x01,
	0x31,0x14, 0x08,0x03,
	0x31,0x16, 0x72,0x02,
	0x31,0x1A, 0x44,0x37,
	0x31,0x1E, 0x09,0x00,
	0x31,0x22, 0x09,0x00,
	0x31,0x28, 0x75,0x09,
	0x31,0x2A, 0x71,0x02,
	0x31,0x2C, 0x00,0x78,
	0x31,0x2E, 0x24,0x78,
	0x31,0x40, 0x20,0x01,
	0x31,0x42, 0x20,0x02,
	0x31,0x44, 0x1E,0x05,
	0x31,0x46, 0x1B,0x01,
	0x31,0x48, 0x1A,0x01,
	0x31,0x4A, 0x1B,0x04,
	0x31,0x4E, 0x1F,0x02,
	0x31,0x70, 0x29,0x8E,
	0x30,0x86, 0x30,0x65,
	0x30,0x88, 0XAF,0xF0,
	0x30,0x94, 0x46,0x56,
	0x30,0x96, 0x56,0x7F,
#endif

	0x30,0x9a, 0xad,0x01 ,
	0x30,0x1A, 0x50,0xCc , 
	
	0x30,0xc0, 0x10,0x20 ,
	0x04,0x00, 0x00,0x02 ,
	0x30,0x6e, 0x90,0xB0 , 
};


static code UINT8 front60fpsPrevCmdTbl[] = {    //640 x 480
       0x01,0x03,0x00,0x01, 
	0x30,0x1A,0x50,0xC8,		

	0x30,0x64,0x08,0x05,	
	
	0x01,0x04,0x00,0x01,	
	0x31,0x6C, 0x24,0xf0,
	0x30,0x9a, 0xa5,0x00,
	0x30,0x88,0x6f,0xf0,
	0x30,0x44,0x85,0x90,
	
	0x31,0xae,0x02,0x01,
	0x01,0x04,0x00,0x00,	             
	

	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x01,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x2d,	// pll_multiplier, Pixel clock 47Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x01,	//op_sys_clk_div

	0x01,0x04,0x00,0x01,

	0x03,0x44,0x00,0x08,
	0x03,0x48,0x0A,0x21,
	0x03,0x46,0x00,0x08,
	0x03,0x4a,0x07,0x91,
	0x30,0x40,0x15,0xc7, 
	0x04,0x00,0x00,0x00,   
	0x04,0x04,0x00,0x10,  
	0x03,0x4C,0x02,0x88,     // H Size   648
	0x03,0x4E,0x01,0xe4,	// V Size	484 	
	0x03,0x42,0x0b,0x60,      // Length Total  1470 
	0x03,0x40,0x03,0xde, 
	0x02,0x02,0x03,0xdb,
	0x30,0x14,0x07,0x02,
	0x30,0x10,0x01,0x5C,

	0x01,0x04,0x00,0x00,
	
	0x30,0xE0, 0xE3,0x02 ,
	0x30,0xE2, 0xB6,0x7C ,
	0x30,0xE4, 0xE1,0xB6 ,
	0x30,0xE6, 0xE1,0x03 ,
	0x30,0xEA, 0x3F,0x06 ,
	0x30,0xEC, 0x7C,0x3F ,
	0x30,0xF2, 0xE1,0x03 ,
	0x30,0xFE, 0x8C,0x6F ,
	0x31,0x00, 0x8C,0x6A ,
	0x31,0x02, 0xB5,0x8D ,
	0x31,0x04, 0xDC,0xB7 ,
	0x31,0x06, 0x3E,0x0A ,
	0x31,0x08, 0x68,0x3F ,
	0x31,0x0A, 0xE1,0x02 ,
	0x31,0x0C, 0xE1,0x02 ,
	0x31,0x0E, 0xE1,0x0A ,
	0x31,0x10, 0xE2,0x01 ,
	0x31,0x12, 0xE1,0x01 ,
	0x31,0x16, 0xE2,0x02 ,
	0x31,0x1A, 0x8D,0x6C ,
	0x31,0x28, 0xE1,0x09 ,
	0x31,0x2A, 0xDD,0x02 ,
	0x31,0x2C, 0x00,0xE4 ,
	0x31,0x2E, 0x42,0xE4 ,
	0x31,0x40, 0x41,0x01 ,
	0x31,0x42, 0x40,0x02 ,
	0x31,0x44, 0x3F,0x03 ,
	0x31,0x46, 0x38,0x04 ,
	0x31,0x48, 0x36,0x01 ,
	0x31,0x4A, 0x38,0x04 ,
	0x31,0x4E, 0x40,0x02 ,
	0x31,0x70, 0x29,0x9A ,
	0x30,0x86, 0x24,0x65 ,
	0x30,0x88, 0x6F,0xF0 ,
	0x30,0x94, 0x46,0x56 ,
	0x30,0x96, 0x56,0x52 ,
	0x30,0x9a, 0xad,0x01 ,
	0x30,0x1A, 0x50,0xCc , 
	
	0x30,0xc0, 0x10,0x20 ,
	0x04,0x00, 0x00,0x02 ,
	0x30,0x6e, 0x90,0xb0 , 

};

static code UINT8 frontSnapCmdTbl[] = {
	0x01,0x03,0x00,0x01, 
	0x30,0x1A,0x50,0xC8,		

	0x30,0x64,0x08,0x05,	
	
	0x01,0x04,0x00,0x01,	
	0x31,0x6C, 0x24,0xf0,
	0x30,0x9a, 0xa5,0x00,
	0x30,0x88,0x6f,0xf0,
	0x30,0x44,0x85,0x90,
	
	0x31,0xae,0x02,0x01,
	0x01,0x04,0x00,0x00,	

	
	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x01,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x32,	// pll_multiplier, Pixel clock 70Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x01,	//op_sys_clk_div///*不修改*/	

	           
	0x01,0x04,0x00,0x01,

	0x03,0x4C,0x0A,0x26,
	0x03,0x4E,0x07,0x9e,//0x98,

	0x03,0x44,0x00,0x08,
	0x03,0x46,0x00,0x08,//0x08,

	0x03,0x48,0x0A,0x2d,//0x2f,
	0x03,0x4A,0x07,0xa5,//0x9F,

	
	0x30,0x40,0x00,0x41,    // will: x_y_binning and summing  
	
	0x04,0x00,0x00,0x00,   
	0x04,0x04,0x00,0x10,  
	
	0x30,0x14,0x06,0xbf,
	0x30,0x10,0x00,0x9C,

	0x03,0x40,0x07,0xed,    // Frame total 2146
	0x03,0x42,0x15,0x00,    // Line total 5214	


	0x01,0x04,0x00,0x00,
	
	0x30,0xE0, 0xE3,0x02 ,
	0x30,0xE2, 0xB6,0x7C ,
	0x30,0xE4, 0xE1,0xB6 ,
	0x30,0xE6, 0xE1,0x03 ,
	0x30,0xEA, 0x3F,0x06 ,
	0x30,0xEC, 0x7C,0x3F ,
	0x30,0xF2, 0xE1,0x03 ,
	0x30,0xFE, 0x8C,0x6F ,
	0x31,0x00, 0x8C,0x6A ,
	0x31,0x02, 0xB5,0x8D ,
	0x31,0x04, 0xDC,0xB7 ,
	0x31,0x06, 0x3E,0x0A ,
	0x31,0x08, 0x68,0x3F ,
	0x31,0x0A, 0xE1,0x02 ,
	0x31,0x0C, 0xE1,0x02 ,
	0x31,0x0E, 0xE1,0x0A ,
	0x31,0x10, 0xE2,0x01 ,
	0x31,0x12, 0xE1,0x01 ,
	0x31,0x16, 0xE2,0x02 ,
	0x31,0x1A, 0x8D,0x6C ,
	0x31,0x28, 0xE1,0x09 ,
	0x31,0x2A, 0xDD,0x02 ,
	0x31,0x2C, 0x00,0xE4 ,
	0x31,0x2E, 0x42,0xE4 ,
	0x31,0x40, 0x41,0x01 ,
	0x31,0x42, 0x40,0x02 ,
	0x31,0x44, 0x3F,0x03 ,
	0x31,0x46, 0x38,0x04 ,
	0x31,0x48, 0x36,0x01 ,
	0x31,0x4A, 0x38,0x04 ,
	0x31,0x4E, 0x40,0x02 ,
	0x31,0x70, 0x29,0x9A ,
	0x30,0x86, 0x24,0x65 ,
	0x30,0x88, 0x6F,0xF0 ,
	0x30,0x94, 0x46,0x56 ,
	0x30,0x96, 0x56,0x52 ,	
	
	0x30,0x9a, 0xad,0x01 ,
	0x30,0x1A, 0x50,0xCc , 
	
	0x30,0xc0, 0x10,0x20 ,
	0x04,0x00, 0x00,0x02 ,
	0x30,0x6e, 0x90,0xb0 , 
	
};

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
		/*case 24:
		case 20:
			printf("frontOpFrameRateSet = dyn 30->15\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
	
			/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
		/*	if (fps == 20) {
				printf("50Hz\n");
				frontResTblBinRead(_ae20fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae20fps60hzTbl, __aeTbl);
			}
			break;*/
		case 30:
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
	
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
			frontPrevMode = 1;
			//frontSnapMode = 1;
	
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
	//gpioByteFuncSet(GPIO_BYTE_TG0, 0x0F, GPIO_FUNC);
	//gpioByteDirSet(GPIO_BYTE_TG0, 0x0F, GPIO_DIR);
	//gpioByteInGateSet(GPIO_BYTE_TG0, 0x0F, GPIO_GATE);
	//gpioByteOutSet(GPIO_BYTE_TG0, 0x0F, GPIO_OUT);

	/* set gpio. */
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
	globalTimerWait(300);
	gpioByteOutSet(GPIO_BYTE_P1, 0x20, 0x20);	
	#endif 

	globalTimerWait(300);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* send i2c command. */
	globalTimerWait(20);
	frontResCmdBinSend(_frontInit0CmdTbl);
	//i2cCmdTableSend(frontInit0CmdTbl, 3, sizeof(frontInit0CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(300);
	//i2cCmdTableSend(frontInit2CmdTbl, 3, sizeof(frontInit2CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	//globalTimerWait(300);
	frontResCmdBinSend(_frontInit1CmdTbl);
	//i2cCmdTableSend(frontInit1CmdTbl, 3, sizeof(frontInit1CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	//i2cCmdTableSend(front30fpsPrevCmdTbl, 3, sizeof(front30fpsPrevCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	//globalTimerWait(600);
	//i2cCmdTableSend(frontInit2CmdTbl, 3, sizeof(frontInit2CmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	//globalTimerWait(20);

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
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, MCLK_DRV_30FPS_PREV, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);
	
    /* send i2c command. */
	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		//i2cCmdTableSend(front30fpsPrevCmdTbl, 3, sizeof(front30fpsPrevCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
		printf("frontOpPreviewModeSet <<30FPS>>\n");
	}
	else {// if ( frontPrevMode == 0 ) {  /* 60 fps. */
		//i2cCmdTableSend(front60fpsPrevCmdTbl, 3, sizeof(front60fpsPrevCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
		printf("frontOpPreviewModeSet <<60FPS>>\n");
	}
	

	

	cdspBayerPatternSet(BAYER_PTN_PREV);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	
	
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
   
	if ( first) {   /*  will: It needs to change mode while taking 1st picture */
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		//i2cCmdTableSend(frontSnapCmdTbl, 3, sizeof(frontSnapCmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
		frontResCmdBinSend(_frontSnapCmdTbl);
		frontSnapExposureTimeSet(mode,scaleUp);
	}

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);
//while(1);
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

    cmdTbl[0] =( 0x305e>>8); 
	cmdTbl[1] =(0x305e & 0xff);
	cmdTbl[2] = (frontLastGain>>8);
	cmdTbl[3] = (frontLastGain& 0xff);
	
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
//frontLastShutter =333;
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
    
	espline = (UINT16)((UINT32)(pixclk *10) /((UINT32)ltotal * frontLastShutter)); 
//ftotal = 570;
	//espline=1008;
	if(espline>=(ftotal))
	{
		ftotal= espline;
		espline = ftotal-3;
	}
	else
	{		
		ftotal = pcap->frameTot;//1148;//1043;//1055;//570;//612;
	}
	

    
	/**/cmdTbl[0] =( 0x300a>>8); 
	cmdTbl[1] =(0x300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0Xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	
	//cmdTbl[0] =( 0x0202>>8); 
	//cmdTbl[1] =(0x0202 & 0xff);

    
	cmdTbl[0] =( 0x3012>>8); 
	cmdTbl[1] =(0x3012 & 0xff);
	cmdTbl[2] = ((espline>>8)&0xff);
	cmdTbl[3] = (espline&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
	
}
/**
 * @brief	operating function to set gain value for intr.
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
frontOpIntrGainSet(
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

    cmdTbl[0] =( 0x305e>>8); 
	cmdTbl[1] =(0x305e & 0xff);
	cmdTbl[2] = (frontLastGain>>8);
	cmdTbl[3] = (frontLastGain& 0xff);
	
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, isr, upd);
	
#endif
}

/**
 * @brief	operating function to set exposure time value for intr.
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
frontOpIntrExposureTimeSet(
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
//frontLastShutter = 333;
	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
    
	espline = (UINT16)((UINT32)(pixclk *10) /((UINT32)ltotal * frontLastShutter)); 
      //ftotal = 570;
	//espline=1008;
	if(espline>=(ftotal))
	{
		ftotal= espline;
		espline = ftotal-3;
	}
	else
	{		
		ftotal = pcap->frameTot;//1148;//1043;//1055;//570;//612;
	}
	

    
	cmdTbl[0] =( 0x300a>>8); 
	cmdTbl[1] =(0x300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0Xff);
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

    	cmdTbl[0] =( 0x3012>>8); 
	cmdTbl[1] =(0x3012 & 0xff);
	cmdTbl[2] = ((espline>>8)&0xff);
	cmdTbl[3] = (espline&0xff);
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

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
	#if (_HW_SOLUTION_ == _HW_RDK_A_) 
	/* sensor reset (fmgpio 10) */
	gpioByteFuncSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteDirSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0xFF);
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
	#if (RES_LSC_LOAD == 1)
	/* resource file lsc cmd table send */
	frontResCmdBinSend(FRONT_RES_PARAM_LSC_ID);
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
	UINT32 pixclk;
	UINT8 cmdTbl[4*1];
	UINT16 s_gain, lastt_gain;
	
	//frontBeforeSnapExposureTimeSet();

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

	
	espline = (pixclk * 10 ) / (ltotal * frontLastShutter);	 
	s_gain = frontLastGain;
	//lastt_gain = 	frontLastGain;
	//printf("$$$ will test ltotal=%d,ftotal=%d,pixclk=%ld,espline=%d,shutter=%d\n",ltotal,ftotal,pixclk,espline,frontLastGain);
	//if(PREVIEW_VSIZE==480)
	espline = espline * 2;  // compensation of binning mode
	//else
	//espline = espline; 	
#if 1
	if ( s_gain >= 0x11c0 ) {         // >8x
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

/*	cmdTbl[0] =( 0x301a>>8); 
	cmdTbl[1] =(0x301a & 0xff);
	cmdTbl[2] = 0x98;	
	cmdTbl[3] = 0xde;                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
*/

	
	/*cmdTbl[0] =( 0x0300a>>8); 
	cmdTbl[1] =(0x0300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);*/	


    
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

	
/*	cmdTbl[0] =( 0x301a>>8); 
	cmdTbl[1] =(0x301a & 0xff);
	cmdTbl[2] = 0x18;	
	cmdTbl[3] = 0xde;                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);	
*/

}

#if 0
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

	frontPrevCapabDesc_t *pcap;
	UINT8 compensationGainIdx = 0, lastGainIdx, subGainIdx = 0, pvGainIdxVal;
	#if (SNAP_GAIN_TO_EXP == 1)
	UINT8 pvgId;
	#endif

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	#if (SNAP_GAIN_TO_EXP == 1)
	/* get pv gId */
	sp1kAeStatusGet(SP1K_AE_gId, &pvgId);
	//printf("pvgId=%bu\n", pvgId);
	#endif

	/* get pv gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
	//printf("pvGainIdxVal=%bu\n", pvGainIdxVal);

	/* get compensation gain idx value */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
	//printf("compensationGainIdx=%bu\n", compensationGainIdx);

	#if (SNAP_GAIN_TO_EXP == 1)
	/* gain transfor exposure time */
	if (pvgId<10) {
		frontSnapLastShutter = frontLastShutter/2;
		subGainIdx = 16;
	}
	else {
		frontSnapLastShutter = frontLastShutter;
	}	
	#else
	frontSnapLastShutter = frontLastShutter;
	#endif
	//printf("frontLastShutter=%lu, frontSnapLastShutter=%lu\n",frontLastShutter,frontSnapLastShutter);

	/* calculate last gain */
	lastGainIdx = pvGainIdxVal + compensationGainIdx - subGainIdx;
	//printf("lastGainIdx=%bu\n", lastGainIdx);
	frontSnapLastGain = pcap->gainTbl[lastGainIdx];	
	
	/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
	LoadBackfrontLastSetting = 1;

	#if (PV_AUTO_OB_BY_GAIN == 1)
	if (pvGainIdxVal == lastGainIdx) {		
		/* resolve ov9712 auto ob by gain issue used */
		if (pvGainIdxVal == pcap->gainMinIdx) {
			LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
		}
		else {//(pvGainIdxVal == pcap->gainMaxIdx)  & other case
			LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
		}
	}
	else {
		LoadBackfrontLastGainIdx = pvGainIdxVal;
	}
	//printf("pvGainIdxVal=%bu, LoadBackfrontLastGainIdx=%bu\n", pvGainIdxVal, LoadBackfrontLastGainIdx);
	#else
	LoadBackfrontLastGainIdx = pvGainIdxVal;
	#endif

}
#endif

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
FrontSnapGlbGainSet(
	void
)
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
	sp1kNandRsvSizeGet(fileId, NULL, &size);
	if (size > FRONT_RES_CMD_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_CMD_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize+(((UINT16)G_FPGAMemPool - 0x4000)>>1))<<1));
	}while(!sts);

	//for (i = 0; i < size; i++) {
	//	printf("0x%02bx,",G_FPGAMemPool[i]);
	//}

	i2cCmdTableSend(G_FPGAMemPool, 3, size/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	
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
	UINT16 byteAddress
)
{
	UINT32 size;
	UINT8 sts;

	if (byteAddress & 0x0001) {
		ASSERT(!(byteAddress & 0x0001), 0); 
		return;
	}
	
	//printf("fileId = 0x%04x, address = 0x%04x\n",fileId,address);
	sp1kNandRsvSizeGet(fileId, NULL, &size);

	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_TBL_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((byteAddress - 0x4000) >> 1)) << 1));
	}while(!sts);

	
	
}



