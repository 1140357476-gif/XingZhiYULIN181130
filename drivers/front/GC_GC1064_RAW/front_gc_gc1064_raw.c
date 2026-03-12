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
 * @file		front_gc_gc1004_raw_inv.c
 * @brief		front sensor omni vision gc1004 driver.
 * @author		ht chen
 * @since		2014-04-03
 * @date		2014-04-09 
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
#include "ae_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_front_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "dbg_file_list.h"
#include "dbg_def.h"
#include "Sp1k_cdsp_api.h"

#include "Os_api.h"
#include "reg_cdsp.h"

#include "App_main.h"
#include "Sp1k_video_api.h"
#include "app_cal_api.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		FRONT_SNAP_BREAK_DOWN	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */

#define USED_INTERNAL_LDO	1	/* old HW : 0(external ldo), new HW : 1(internal ldo) */
#define SNAP_KEEP_FPS		0	/* add dummy line : 0(reduce fps), keep fps : 1(exposure line equal to frame total-6) */
#define PV_AUTO_OB_BY_GAIN	1	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
#define RES_LSC_LOAD		0	/* after power on load lsc resource file */
#define PV_720_50FPS		0	/* preview 1280*720 h,v binning 50fps sensor initial setting */

/**************************************************************************
 *                            M I R R O R   &   F L I P	C T R L 
 *			RDK : mirror off, flip on
 *			DDX : mirror on, flip on
 **************************************************************************/
/* mirror, flip ctrl */
#define MIRROR_CTRL		0	/* mirror off : 0, mirror on : 1 */
#define FLIP_CTRL		1	/* flip off : 0, flip on : 1 */

/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */

/* 720p, 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     720
#define XOFF_30FPS_PREV       2
#define YOFF_30FPS_PREV       2// 1
#define LINE_TOT_30FPS_PREV   2190
#define FRAME_TOT_30FPS_PREV  902
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  1
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0//200



/* 5M */
#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     720
#define XOFF_SNAP       2 
#define YOFF_SNAP       2// 1
#define LINE_TOT_SNAP   2190
#define FRAME_TOT_SNAP  902

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    11// 16
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_SNAP          		11
#define MCLK_DIV_SNAP_BURST     	18
#define MCLK_DIV_SNAP_BURST_UP 		24
#define MCLK_PHASE_SNAP         	0
#define PCLK_DIV_SNAP           	11
#define PCLK_PHASE_SNAP         	0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV     59318181L// 31070000L//
#define PCLK_FREQ_30FPS_PREV_TD  59318181L
#define PCLK_FREQ_SNAP           59318181L
#define PCLK_FREQ_SNAP_BURST  	 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP  31200000L	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_30FPS_PREV     595200000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD	 595200000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_20FPS_PREV  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP        595200000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	595200000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP 595200000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HEN_50FPS_PREV    0
#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HEN_20FPS_PREV    0
#else
#define RESHAPE_HEN_30FPS_PREV    0	/* bypass h don't enable */
#define RESHAPE_HEN_50FPS_PREV    0	/* bypass h don't enable */
#define RESHAPE_HEN_60FPS_PREV    0	/* bypass h don't enable */
#define RESHAPE_HEN_20FPS_PREV    0	/* bypass h don't enable */
#endif
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  3
#define RESHAPE_HFALL_50FPS_PREV  2
#define RESHAPE_HRISE_50FPS_PREV  3
#define RESHAPE_HFALL_60FPS_PREV  2
#define RESHAPE_HRISE_60FPS_PREV  3
#define RESHAPE_HFALL_20FPS_PREV  2
#define RESHAPE_HRISE_20FPS_PREV  3

#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VEN_50FPS_PREV    0
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VEN_20FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	/* bypass v don't enable */
#define RESHAPE_VEN_50FPS_PREV    0	/* bypass v don't enable */
#define RESHAPE_VEN_60FPS_PREV    0	/* bypass v don't enable */
#define RESHAPE_VEN_20FPS_PREV    0	/* bypass v don't enable */
#endif
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3
#define RESHAPE_VFALL_50FPS_PREV  2
#define RESHAPE_VRISE_50FPS_PREV  3
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3
#define RESHAPE_VFALL_20FPS_PREV  2
#define RESHAPE_VRISE_20FPS_PREV  3

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  3
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

/* preview h,v sync inv. */
#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  1

/* snapshot h,v sync inv. */
#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1

/* bayer pattern. */

#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GBBRGR//FRONT_BAYER_PTN_RGRGBB
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_GBBRGR//FRONT_BAYER_PTN_RGRGBB

/* i2c interface. */
#define I2C_DEV_ADDR  0x78	//0x79
#define I2C_CLK_DIV          2//I2C_REG_CLK_DIV_256  // I2C_REG_CLK_DIV_128

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  138
#define AE_30FPS_30_MIN_IDX  14
#define AE_30FPS_25_MAX_IDX  137
#define AE_30FPS_25_MIN_IDX  14
#define EV10_30FPS_EXP_IDX   100


/* gain. */
#define GAIN_30FPS_MAX_IDX  239//448
#define GAIN_30FPS_MIN_IDX  0


/* iso gain. */
#define ISO_100_GAIN_IDX	0
#define ISO_200_GAIN_IDX	16
#define ISO_400_GAIN_IDX	32
#define ISO_800_GAIN_IDX	48

/* manual iso capture limit fps */
#define CAPTURE_LIMIT_FPS	15

/* iso capture auto de-banding */
#define ISO_CAPTURE_AUTO_DEBANDING	1


/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_ENABLE				/* FRONT_SRAM_MODE_ENABLE : image width <= 1280 */
																/* FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor */

/* exposure, gain setting position */
#define EXP_GAIN_SET_POS  AE_EXP_GAIN_SET_POS_VVALID_RISING		/* AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, */
																/* AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling, */
																/* AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising. */

/* gain setting position */
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_OFF					/* AE_GAIN_AFTER_EXP_OFF, */
																/* AE_GAIN_AFTER_EXP_ON */

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
/* function type definition. */
#define __STATIC	//static

/* operating function. */
#define frontOpen                                   frontSensorOpen
#define frontOpDevNameGet                           frontSensorOpDevNameGet
#define frontOpFrameRateSet                         frontSensorOpFrameRateSet
#define frontOpCapabilityGet                        frontSensorOpCapabilityGet
#define frontOpIntrCapabilityGet                    frontSensorOpIntrCapabilityGet
#define frontOpPowerOn                              frontSensorOpPowerOn
#define frontOpPowerOff                             frontSensorOpPowerOff
#define frontOpPreviewModeSet                       frontSensorOpPreviewModeSet
#define frontOpPreviewSensorZoomModeSet             frontSensorOpPreviewSensorZoomModeSet
#define frontOpIntrPreviewSensorZoomModeSet         frontSensorOpIntrPreviewSensorZoomModeSet
#define frontOpSnapModeSet                          frontSensorOpSnapModeSet
#define frontOpSnapShot                             frontSensorOpSnapShot
#define frontOpGainSet                              frontSensorOpGainSet
#define frontOpExposureTimeSet                      frontSensorOpExposureTimeSet
#define frontOpIntrGainSet                          frontSensorOpIntrGainSet     
#define frontOpIntrExposureTimeSet                  frontSensorOpIntrExposureTimeSet
#define frontOpOpticalBlackStatusGet                frontSensorOpOpticalBlackStatusGet
#define frontOpOpticalBlackCalibrate                frontSensorOpOpticalBlackCalibrate
#define frontOpShutterOpen                          frontSensorOpShutterOpen
#define frontOpShutterClose                         frontSensorOpShutterClose
#define frontOpIntrPreviewDynamicTempoalDenoiseSet  frontSensorOpIntrPreviewDynamicTempoalDenoiseSet

/* extern operating function. */
#define frontOpAeTargetLuminanceSet                 frontSensorOpAeTargetLuminanceSet
#define frontOpAeExposureCompensationSet            frontSensorOpAeExposureCompensationSet
#define frontOpAeFrameRateSet                       frontSensorOpAeFrameRateSet
#define frontOpAwbModeSet                           frontSensorOpAwbModeSet
#define frontOpAfterSnapShot                        frontSensorOpAfterSnapShot


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
__STATIC void frontOpPreviewSensorZoomModeSet(UINT16 factor);
__STATIC void frontOpIntrPreviewSensorZoomModeSet(UINT16 factor);
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
__STATIC void frontOpIntrPreviewDynamicTempoalDenoiseSet(UINT8 en);

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
static void frontGainTblValueTransfer(UINT16 value,UINT16 *numerator,UINT16 *denominator);
static void frontPreviewModeSwitchPostProc(void);
static void frontResTblBinRead(UINT16 fileId,	UINT16 byteAddress);
static void frontResCmdBinSend(UINT16 fileId);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - GC GC1064 RAW";

/**************************************************************************
 *		This segment is used to define the variables which should be convert to resource files
 *					CvtTypeToX: convert to "XDATA" table	
 *					CvtTypeToC: convert to "CODE" table		
 *					CvtTypeRmv: remove the specified table	
 **************************************************************************/
#define CVT_DEF  extern

/* Tag for convert start */
#if (LOAD_RES_OPTION == 1)
CVT_DEF UINT8 TAG_CVT_S;
#endif

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
	//_ae60fps50hzTbl = 0x00C8,
	//_ae60fps60hzTbl = 0x00C9,
} _aeTbl;

/* gain table resource file id */
typedef enum CvtTypeToX_e {
	_gainTbl = 0x00CA,
};

/* i2c initial cmd table resource file id */
typedef enum CvtTypeRmv_e {
	_frontInit0CmdTbl		= 0x00CB,
	_frontInit1CmdTbl		= 0x00CC,
	_frontInit2CmdTbl		= 0x00CD,
	//_frontInit3CmdTbl		= 0x00CE,
	//_frontInit4CmdTbl		= 0x00CF,
	//_front15fpsPrevCmdTbl	= 0x00D0,
	//_front20fpsPrevCmdTbl	= 0x00D1,
	//_front24fpsPrevCmdTbl	= 0x00D2,
	//_front30fpsPrevCmdTbl	= 0x00D3,
	//_front60fpsPrevCmdTbl	= 0x00D4,
	//_frontSnapCmdTbl		= 0x00D5,
};

/* ae table & gain table link address */
typedef enum CvtTypeLnkAddr_e {
	__aeTbl = 0xa000,
	__gainTbl = 0xb000,
};

/* Tag for convert end */
CVT_DEF UINT8 TAG_CVT_E;

/**************************************************************************
 *                            AE TABLE
 **************************************************************************/
/**************************************************************************
 *				banding calculate
 *			50Hz : 1/(shutter/10) < 1/(2*50)
 *			60Hz : 1/(shutter/10) < 1/(2*60)
 **************************************************************************/
/* ae table. */
static code frontAe_t ae30fps50hzTbl[138] = {
	{	0,	332,	238,	0,	0 },
	{	1,	332,	221,	0,	0 },
	{	2,	332,	205,	0,	0 },
	{	3,	332,	190,	0,	0 },
	{	4,	332,	177,	0,	0 },
	{	5,	332,	164,	0,	0 },
	{	6,	332,	152,	0,	0 },
	{	7,	332,	140,	0,	0 },
	{	8,	332,	130,	0,	0 },
	{	9,	332,	120,	0,	0 },
	{	10,	332,	111,	0,	0 },
	{	11,	332,	103,	0,	0 },
	{	12,	332,	95,	0,	0 },
	{	13,	332,	87,	0,	0 },
	{	14,	332,	81,	0,	0 },
	{	15,	332,	74,	0,	0 },
	{	16,	332,	68,	0,	0 },
	{	17,	332,	62,	0,	0 },
	{	18,	332,	57,	0,	0 },
	{	19,	332,	52,	0,	0 },
	{	20,	332,	48,	0,	0 },
	{	21,	332,	44,	0,	0 },
	{	22,	332,	40,	0,	0 },
	{	23,	332,	36,	0,	0 },
	{	24,	332,	33,	0,	0 },
	{	25,	332,	29,	0,	0 },
	{	26,	332,	26,	0,	0 },
	{	27,	332,	23,	0,	0 },
	{	28,	332,	21,	0,	0 },
	{	29,	332,	18,	0,	0 },
	{	30,	332,	16,	0,	0 },
	{	31,	332,	14,	0,	0 },
	{	32,	332,	12,	0,	0 },
	{	33,	332,	10,	0,	0 },
	{	34,	332,	9,	0,	0 },
	{	35,	332,	7,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	4,	0,	0 },
	{	38,	332,	3,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	497,	7,	0,	0 },
	{	42,	497,	5,	0,	0 },
	{	43,	497,	4,	0,	0 },
	{	44,	497,	2,	0,	0 },
	{	45,	497,	1,	0,	0 },
	{	46,	497,	0,	0,	0 },
	{	47,	992,	14,	0,	0 },
	{	48,	992,	12,	0,	0 },
	{	49,	992,	10,	0,	0 },
	{	50,	992,	8,	0,	0 },
	{	51,	992,	7,	0,	0 },
	{	52,	992,	5,	0,	0 },
	{	53,	992,	4,	0,	0 },
	{	54,	992,	2,	0,	0 },
	{	55,	992,	1,	0,	0 },
	{	56,	992,	0,	0,	0 },
	{	57,	1029,	0,	0,	0 },
	{	58,	1101,	0,	0,	0 },
	{	59,	1182,	0,	0,	0 },
	{	60,	1265,	0,	0,	0 },
	{	61,	1361,	0,	0,	0 },
	{	62,	1456,	0,	0,	0 },
	{	63,	1565,	0,	0,	0 },
	{	64,	1671,	0,	0,	0 },
	{	65,	1793,	0,	0,	0 },
	{	66,	1920,	0,	0,	0 },
	{	67,	2067,	0,	0,	0 },
	{	68,	2202,	0,	0,	0 },
	{	69,	2375,	0,	0,	0 },
	{	70,	2531,	0,	0,	0 },
	{	71,	2735,	0,	0,	0 },
	{	72,	2912,	0,	0,	0 },
	{	73,	3149,	0,	0,	0 },
	{	74,	3343,	0,	0,	0 },
	{	75,	3611,	0,	0,	0 },
	{	76,	3869,	0,	0,	0 },
	{	77,	4167,	0,	0,	0 },
	{	78,	4440,	0,	0,	0 },
	{	79,	4751,	0,	0,	0 },
	{	80,	5110,	0,	0,	0 },
	{	81,	5527,	0,	0,	0 },
	{	82,	5888,	0,	0,	0 },
	{	83,	6299,	0,	0,	0 },
	{	84,	6771,	0,	0,	0 },
	{	85,	7320,	0,	0,	0 },
	{	86,	7738,	0,	0,	0 },
	{	87,	8464,	0,	0,	0 },
	{	88,	9028,	0,	0,	0 },
	{	89,	9673,	0,	0,	0 },
	{	90,	10417,	0,	0,	0 },
	{	91,	11285,	0,	0,	0 },
	{	92,	11776,	0,	0,	0 },
	{	93,	12897,	0,	0,	0 },
	{	94,	13542,	0,	0,	0 },
	{	95,	15047,	0,	0,	0 },
	{	96,	15932,	0,	0,	0 },
	{	97,	16928,	0,	0,	0 },
	{	98,	18057,	0,	0,	0 },
	{	99,	19346,	0,	0,	0 },
	{	100,	20835,	0,	0,	0 },
	{	101,	22571,	0,	0,	0 },
	{	102,	24623,	0,	0,	0 },
	{	103,	27085,	1,	0,	0 },
	{	104,	27085,	0,	0,	0 },
	{	105,	30095,	0,	0,	0 },
	{	106,	33856,	1,	0,	0 },
	{	107,	33856,	0,	0,	0 },
	{	108,	38693,	1,	0,	0 },
	{	109,	38693,	0,	0,	0 },
	{	110,	45142,	1,	0,	0 },
	{	111,	45142,	0,	0,	0 },
	{	112,	54170,	2,	0,	0 },
	{	113,	54170,	1,	0,	0 },
	{	114,	54170,	0,	0,	0 },
	{	115,	67713,	2,	0,	0 },
	{	116,	67713,	1,	0,	0 },
	{	117,	67713,	0,	0,	0 },
	{	118,	90283,	4,	0,	0 },
	{	119,	90283,	3,	0,	0 },
	{	120,	90283,	1,	0,	0 },
	{	121,	90283,	0,	0,	0 },
	{	122,	135422,	6,	0,	0 },
	{	123,	135422,	5,	0,	0 },
	{	124,	135422,	4,	0,	0 },
	{	125,	135422,	2,	0,	0 },
	{	126,	135422,	1,	0,	0 },
	{	127,	135422,	0,	0,	0 },
	{	128,	270832,	13,	0,	0 },
	{	129,	270832,	12,	0,	0 },
	{	130,	270832,	10,	0,	0 },
	{	131,	270832,	8,	0,	0 },
	{	132,	270832,	6,	0,	0 },
	{	133,	270832,	5,	0,	0 },
	{	134,	270832,	4,	0,	0 },
	{	135,	270832,	2,	0,	0 },
	{	136,	270832,	1,	0,	0 },
	{	137,	270832,	0,	0,	0 },
	};



static code frontAe_t ae30fps60hzTbl[139] = {
	{	0,	299,	229,	0,	0 },
	{	1,	299,	213,	0,	0 },
	{	2,	299,	197,	0,	0 },
	{	3,	299,	183,	0,	0 },
	{	4,	299,	170,	0,	0 },
	{	5,	299,	157,	0,	0 },
	{	6,	299,	146,	0,	0 },
	{	7,	299,	135,	0,	0 },
	{	8,	299,	125,	0,	0 },
	{	9,	299,	116,	0,	0 },
	{	10,	299,	107,	0,	0 },
	{	11,	299,	99,	0,	0 },
	{	12,	299,	91,	0,	0 },
	{	13,	299,	84,	0,	0 },
	{	14,	299,	77,	0,	0 },
	{	15,	299,	71,	0,	0 },
	{	16,	299,	65,	0,	0 },
	{	17,	299,	60,	0,	0 },
	{	18,	299,	55,	0,	0 },
	{	19,	299,	50,	0,	0 },
	{	20,	299,	46,	0,	0 },
	{	21,	299,	42,	0,	0 },
	{	22,	299,	38,	0,	0 },
	{	23,	299,	34,	0,	0 },
	{	24,	299,	31,	0,	0 },
	{	25,	299,	28,	0,	0 },
	{	26,	299,	25,	0,	0 },
	{	27,	299,	22,	0,	0 },
	{	28,	299,	20,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	15,	0,	0 },
	{	31,	299,	13,	0,	0 },
	{	32,	299,	11,	0,	0 },
	{	33,	299,	9,	0,	0 },
	{	34,	299,	8,	0,	0 },
	{	35,	299,	6,	0,	0 },
	{	36,	299,	5,	0,	0 },
	{	37,	299,	3,	0,	0 },
	{	38,	299,	2,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	398,	4,	0,	0 },
	{	42,	398,	2,	0,	0 },
	{	43,	398,	1,	0,	0 },
	{	44,	398,	0,	0,	0 },
	{	45,	597,	6,	0,	0 },
	{	46,	597,	5,	0,	0 },
	{	47,	597,	3,	0,	0 },
	{	48,	597,	2,	0,	0 },
	{	49,	597,	1,	0,	0 },
	{	50,	597,	0,	0,	0 },
	{	51,	1193,	13,	0,	0 },
	{	52,	1193,	11,	0,	0 },
	{	53,	1193,	9,	0,	0 },
	{	54,	1193,	8,	0,	0 },
	{	55,	1193,	6,	0,	0 },
	{	56,	1193,	5,	0,	0 },
	{	57,	1193,	3,	0,	0 },
	{	58,	1193,	2,	0,	0 },
	{	59,	1193,	1,	0,	0 },
	{	60,	1193,	0,	0,	0 },
	{	61,	1265,	0,	0,	0 },
	{	62,	1361,	0,	0,	0 },
	{	63,	1456,	0,	0,	0 },
	{	64,	1565,	0,	0,	0 },
	{	65,	1671,	0,	0,	0 },
	{	66,	1793,	0,	0,	0 },
	{	67,	1920,	0,	0,	0 },
	{	68,	2067,	0,	0,	0 },
	{	69,	2202,	0,	0,	0 },
	{	70,	2375,	0,	0,	0 },
	{	71,	2531,	0,	0,	0 },
	{	72,	2735,	0,	0,	0 },
	{	73,	2912,	0,	0,	0 },
	{	74,	3149,	0,	0,	0 },
	{	75,	3343,	0,	0,	0 },
	{	76,	3611,	0,	0,	0 },
	{	77,	3869,	0,	0,	0 },
	{	78,	4167,	0,	0,	0 },
	{	79,	4440,	0,	0,	0 },
	{	80,	4751,	0,	0,	0 },
	{	81,	5110,	0,	0,	0 },
	{	82,	5527,	0,	0,	0 },
	{	83,	5888,	0,	0,	0 },
	{	84,	6299,	0,	0,	0 },
	{	85,	6771,	0,	0,	0 },
	{	86,	7320,	0,	0,	0 },
	{	87,	7738,	0,	0,	0 },
	{	88,	8464,	0,	0,	0 },
	{	89,	9028,	0,	0,	0 },
	{	90,	9673,	0,	0,	0 },
	{	91,	10417,	0,	0,	0 },
	{	92,	11285,	0,	0,	0 },
	{	93,	11776,	0,	0,	0 },
	{	94,	12897,	0,	0,	0 },
	{	95,	13542,	0,	0,	0 },
	{	96,	15047,	0,	0,	0 },
	{	97,	15932,	0,	0,	0 },
	{	98,	16928,	0,	0,	0 },
	{	99,	18057,	0,	0,	0 },
	{	100,	19346,	0,	0,	0 },
	{	101,	20835,	0,	0,	0 },
	{	102,	22571,	0,	0,	0 },
	{	103,	24623,	0,	0,	0 },
	{	104,	27085,	1,	0,	0 },
	{	105,	27085,	0,	0,	0 },
	{	106,	30095,	0,	0,	0 },
	{	107,	33856,	1,	0,	0 },
	{	108,	33856,	0,	0,	0 },
	{	109,	38693,	1,	0,	0 },
	{	110,	38693,	0,	0,	0 },
	{	111,	45142,	1,	0,	0 },
	{	112,	45142,	0,	0,	0 },
	{	113,	54170,	2,	0,	0 },
	{	114,	54170,	1,	0,	0 },
	{	115,	54170,	0,	0,	0 },
	{	116,	67713,	2,	0,	0 },
	{	117,	67713,	1,	0,	0 },
	{	118,	67713,	0,	0,	0 },
	{	119,	90283,	4,	0,	0 },
	{	120,	90283,	3,	0,	0 },
	{	121,	90283,	1,	0,	0 },
	{	122,	90283,	0,	0,	0 },
	{	123,	135422,	6,	0,	0 },
	{	124,	135422,	5,	0,	0 },
	{	125,	135422,	4,	0,	0 },
	{	126,	135422,	2,	0,	0 },
	{	127,	135422,	1,	0,	0 },
	{	128,	135422,	0,	0,	0 },
	{	129,	270832,	13,	0,	0 },
	{	130,	270832,	12,	0,	0 },
	{	131,	270832,	10,	0,	0 },
	{	132,	270832,	8,	0,	0 },
	{	133,	270832,	6,	0,	0 },
	{	134,	270832,	5,	0,	0 },
	{	135,	270832,	4,	0,	0 },
	{	136,	270832,	2,	0,	0 },
	{	137,	270832,	1,	0,	0 },
	{	138,	270832,	0,	0,	0 },
	};

/* gain table. */
static code UINT16 gainTbl[241] = {
   64,    68,    72,    76,    80,    84,    88,    92,    96,   100, 
  104,   108,   112,   116,   120,   124,   128,   132,   136,   140, 
  144,   148,   152,   156,   160,   164,   168,   172,   176,   180, 
  184,   188,   192,   196,   200,   204,   208,   212,   216,   220, 
  224,   228,   232,   236,   240,   244,   248,   252,   256,   260, 
  264,   268,   272,   276,   280,   284,   288,   292,   296,   300, 
  304,   308,   312,   316,   320,   324,   328,   332,   336,   340, 
  344,   348,   352,   356,   360,   364,   368,   372,   376,   380, 
  384,   388,   392,   396,   400,   404,   408,   412,   416,   420, 
  424,   428,   432,   436,   440,   444,   448,   452,   456,   460, 
  464,   468,   472,   476,   480,   484,   488,   492,   496,   500, 
  504,   508,   512,   516,   520,   524,   528,   532,   536,   540, 
  544,   548,   552,   556,   560,   564,   568,   572,   576,   580, 
  584,   588,   592,   596,   600,   604,   608,   612,   616,   620, 
  624,   628,   632,   636,   640,   644,   648,   652,   656,   660, 
  664,   668,   672,   676,   680,   684,   688,   692,   696,   700, 
  704,   708,   712,   716,   720,   724,   728,   732,   736,   740, 
  744,   748,   752,   756,   760,   764,   768,   772,   776,   780, 
  784,   788,   792,   796,   800,   804,   808,   812,   816,   820, 
  824,   828,   832,   836,   840,   844,   848,   852,   856,   860, 
  864,   868,   872,   876,   880,   884,   888,   892,   896,   900, 
  904,   908,   912,   916,   920,   924,   928,   932,   936,   940, 
  944,   948,   952,   956,   960,   964,   968,   972,   976,   980, 
  984,   988,   992,   996,   1000,   1004,   1008,   1012,   1016,   1020, 
  1024, 	};






/* exposure time index table. */
static code UINT8 ae30fpsMaxIdxTbl[2] = {
	AE_30FPS_30_MAX_IDX,
	AE_30FPS_25_MAX_IDX,
};

static code UINT8 ae30fpsMinIdxTbl[2] = {
	AE_30FPS_30_MIN_IDX,
	AE_30FPS_25_MIN_IDX,
};



/* preview to snapshot binning rate table. */
static code UINT8 frontPrevToSnapBinningRateTbl[PREV_MODE_TOT] = {

	PREV_TO_SNAP_BINNING_RATE_30FPS_PREV,
};

/* preview tempoal denoise table. */
static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl[PREV_MODE_TOT] = {

	{  /* 30 fps. */
		MCLK_DIV_30FPS_PREV + 5,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV + 5,
		PCLK_PHASE_30FPS_PREV,
		PCLK_FREQ_30FPS_PREV_TD,
		31,
		6,
	},
};

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
		&frontPrevTempDenoiseTbl[0],
		SENSOR_ZOOM_FACTOR_30FPS_PREV,
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
	0xfe, 0x00,
};

static code UINT8 frontInit1CmdTbl[] = {
	0xfe,0x80,
	0xfe,0x80,
	0xfe,0x80,
	0xf2,0x0f, //sync_pad_io_ebi
	0xf6,0x00, //up down
	0xfc,0xc6,
	0xf7,0xb9, //pll enable
	0xf8,0x04, //Pll mode 2
	0xf9,0x2e, //[0] pll enable
	0xfa,0x00, //div
	0xfe,0x00,
            
	0x03,0x02,
	0x04,0xd0,
	
	0x05,0x01,
	0x06,0xa3,
	0x07,0x00,
	0x08,0x90,//0xb4, //height

	/*
	0x05,0x00,
	0x06,0x0c,
	0x07,0x00,
	0x08,0x12, //height
	*/
    
	0x0e,0xe6,
	0x0f,0x05, //width
	0x10,0x10,
	0x11,0x00,
	0x12,0x18,
	#if MIRROR_CTRL 
	0x17, 0x17, // [0]mirror [1]flip 
	#else 
	0x17, 0x14, // [0]mirror [1]flip 
	#endif 
	0x19,0x06,
	0x1b,0x4f,
	0x1c,0x11,
	0x1d,0x10,
	0x1e,0xf8,
	0x1f,0x38,
	0x20,0x81,
	0x21,0x1f,//6f//2f
	0x22,0xc0,
	0x23,0x82,
	0x24,0x2f,
	0x25,0xd4,
	0x26,0xa8,
	0x29,0x3f,//54//3f
	0x2a,0x00,
	0x2b,0x00,//00--powernoise	03---强光拖尾
	0x2c,0xe0,//DC--30fps	d0---50fps
	0x2d,0x0f,
	0x2e,0x00,
	0x2f,0x16,//1f--横闪线
	0xcc,0x25,
	0xce,0xf3,
	0x3f,0x18,
	
	0x30,0x00,
	0x31,0x01,
	0x32,0x02,
	0x33,0x03,
	0x34,0x04,
	0x35,0x05,
	0x36,0x06,
	0x37,0x07,
	0x38,0x0f,
	0x39,0x17,
	0x3a,0x1f,
	0xfe,0x00,
	0x8a,0x00,
	0x8c,0x02,
	0x8e,0x02,
	0x8f,0x15,
	0x90,0x01,
	0x94,0x0a,
	0x95,0x02,
	0x96,0xd2,
	0x97,0x05,
	0x98,0x00,
	  
	0xfe,0x03,
	0x01,0x00,
	0x02,0x00,
	0x03,0x00,
	0x06,0x00,
	0x10,0x00,
	0x15,0x00,	  
	0xfe,0x00,
	0x18,0x02,
	0x1a,0x11,
	0x40,0x23,
	0x5e,0x00,
	0x66,0x80,  
	0xfe,0x00,//dark sun
	0xcc,0x25,
	0xce,0xf3,
	0x3f,0x18,
	0xfe,0x00,	  
	0xb0,0x50,
	0xb3,0x40,
	0xb4,0x40,
	0xb5,0x40,
	0xb6,0x00,
	0xf2,0x0f,
	0xfe,0x00,



};


static code UINT8 frontInit2CmdTbl[] = {
	0xfe,0x80,
	0xfe,0x80,
	0xfe,0x80,
	0xf2,0x0f, //sync_pad_io_ebi
	0xf6,0x00, //up down
	0xfc,0xc6,
	0xf7,0xb9, //pll enable
	0xf8,0x04, //Pll mode 2
	0xf9,0x2e, //[0] pll enable
	0xfa,0x00, //div
	0xfe,0x00,
            
	0x03,0x02,
	0x04,0xd0,
	
	0x05,0x01,
	0x06,0xa3,
	0x07,0x00,
	0x08,0x90,//0xb4, //height

	/*
	0x05,0x00,
	0x06,0x0c,
	0x07,0x00,
	0x08,0x12, //height
	*/
    
	0x0e,0xe6,
	0x0f,0x05, //width
	0x10,0x10,
	0x11,0x00,
	0x12,0x18,
	#if MIRROR_CTRL 
	0x17, 0x17, // [0]mirror [1]flip 
	#else 
	0x17, 0x14, // [0]mirror [1]flip 
	#endif 
	0x19,0x06,
	0x1b,0x4f,
	0x1c,0x11,
	0x1d,0x10,
	0x1e,0xf8,
	0x1f,0x38,
	0x20,0x81,
	0x21,0x1f,//6f//2f
	0x22,0xc0,
	0x23,0x82,
	0x24,0x2f,
	0x25,0xd4,
	0x26,0xa8,
	0x29,0x3f,//54//3f
	0x2a,0x00,
	0x2b,0x00,//00--powernoise	03---强光拖尾
	0x2c,0xe0,//DC--30fps	d0---50fps
	0x2d,0x0f,
	0x2e,0x00,
	0x2f,0x16,//1f--横闪线
	0xcc,0x25,
	0xce,0xf3,
	0x3f,0x18,
	
	0x30,0x00,
	0x31,0x01,
	0x32,0x02,
	0x33,0x03,
	0x34,0x04,
	0x35,0x05,
	0x36,0x06,
	0x37,0x07,
	0x38,0x0f,
	0x39,0x17,
	0x3a,0x1f,
	0xfe,0x00,
	0x8a,0x00,
	0x8c,0x02,
	0x8e,0x02,
	0x8f,0x15,
	0x90,0x01,
	0x94,0x0a,
	0x95,0x02,
	0x96,0xd2,
	0x97,0x05,
	0x98,0x00,
	  
	0xfe,0x03,
	0x01,0x00,
	0x02,0x00,
	0x03,0x00,
	0x06,0x00,
	0x10,0x00,
	0x15,0x00,	  
	0xfe,0x00,
	0x18,0x02,
	0x1a,0x11,
	0x40,0x23,
	0x5e,0x00,
	0x66,0x80,  
	0xfe,0x00,//dark sun
	0xcc,0x25,
	0xce,0xf3,
	0x3f,0x18,
	0xfe,0x00,	  
	0xb0,0x50,
	0xb3,0x40,
	0xb4,0x40,
	0xb5,0x40,
	0xb6,0x00,
	0xf2,0x0f,
	0xfe,0x00,
};




static UINT8 front30fpsPrevDz1xCmdTbl[] = {
	0,
};
static UINT8 front30fpsPrevDz2xCmdTbl[] = {
	0,
};

static UINT8 front60fpsPrevDz1xCmdTbl[1] = {
	0,
};
static UINT8 front60fpsPrevDz2xCmdTbl[1] = {
	0,
};

static UINT8 front20fpsPrevDz1xCmdTbl[1] = {
	0,
};
static UINT8 front20fpsPrevDz2xCmdTbl[1] = {
	0,
};



static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;
static UINT16 frontLastGain;		/* preview last set gain value */
static UINT16 frontLastGainIdx;		/* preview last set gain table index */
static UINT32 frontLastShutter;		/* preview last set shutter value */
static UINT16 frontSnapLastGain;	/* snapshot last set gain value */
static UINT32 frontSnapLastShutter;	/* snapshot last set shutter value */

static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;

static UINT8 frontPrevModeSave = 0;	/* pre-sensor mode */

static UINT8 frontPowerOnStatus = 0; /* 0:power on no initial */

static UINT8 frontPrevDyTdEn = 0;
static UINT8 frontPrevDyTdSwitchPclk = 0;
static UINT8 frontPrevDyTdSwitchExp = 0;
static UINT8 frontPrevDyTdSwitchGain = 0;
static UINT8 frontPrevDyTdSetFlow = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
#if SUPPORT_SENSOR_SWITCH
	extern UINT8 sensor_switch;
#endif


  
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
	#if (REGISTER_EXTERN_OP_FUNC == 1)
	UINT8 id;
	#endif
	frontOperatingFunc_t opFunc;  
	opFunc.expGainSetPos                           = EXP_GAIN_SET_POS;
	opFunc.gainAfterExp                            = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet                          = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet                        = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet                       = frontOpCapabilityGet;
	opFunc.pfOpIntrCapabilityGet                   = frontOpIntrCapabilityGet;
	opFunc.pfOpPowerOn                             = frontOpPowerOn;
	opFunc.pfOpPowerOff                            = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet                      = frontOpPreviewModeSet;
	opFunc.pfOpPreviewSensorZoomModeSet            = frontOpPreviewSensorZoomModeSet;
	// linziyang modify begin
	//opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontOpIntrPreviewSensorZoomModeSet;
	opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontOpIntrPreviewSensorZoomModeSet;
	// linziyang modify end
	opFunc.pfOpSnapModeSet                         = frontOpSnapModeSet;
	opFunc.pfOpSnapShot                            = frontOpSnapShot;
	opFunc.pfOpGainSet                             = frontOpGainSet;
	opFunc.pfOpExposureTimeSet                     = frontOpExposureTimeSet;
	opFunc.pfOpIntrGainSet                         = frontOpIntrGainSet;
	opFunc.pfOpIntrExposureTimeSet                 = frontOpIntrExposureTimeSet;
	opFunc.pfOpOpticalBlackStatusGet               = frontOpOpticalBlackStatusGet;
	opFunc.pfOpOpticalBlackCalibrate               = frontOpOpticalBlackCalibrate;
	opFunc.pfOpShutterOpen                         = frontOpShutterOpen;
	opFunc.pfOpShutterClose                        = frontOpShutterClose;
	opFunc.pfOpIntrPreviewDynamicTempoalDenoiseSet = frontOpIntrPreviewDynamicTempoalDenoiseSet;
	
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
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 0;

			if (fps == 25) {
				printf("50Hz\n");
				frontResTblBinRead(_ae30fps50hzTbl, __aeTbl);
			}
			else {
				printf("60Hz\n");
				frontResTblBinRead(_ae30fps60hzTbl, __aeTbl);
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
	UINT8 addrt[] = {0xf0};
	UINT8 datat[] = {0XFF};

	/* before power on. */
	frontBeforePowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* used (RDK) GPIO12, GPIO13, (EVB) TP_YN & TP_XN GPIO */
	#if (DBG_SNAP_TIMING == 1)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 3, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);


	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);


	/* send i2c command. */
	globalTimerWait(20);
	frontResCmdBinSend(_frontInit0CmdTbl);
	
	globalTimerWait(50);
	// frontResCmdBinSend(_frontInit1CmdTbl);
	frontResCmdBinSend(sensor_switch ? _frontInit1CmdTbl : _frontInit2CmdTbl);

	#if 0
	i2cDeviceAddrSet(0x79);
	i2cClkCfgSet(I2C_CLK_DIV);

	addrt[0] = 0xf0;
	i2cRegRead(addrt,datat,1,1,1,0,0);
	printf("addr[0]=%d,datatbl[0]=%d\n", (UINT16)addrt[0], (UINT16)datat[0]);

	addrt[0] = 0xf1;
	i2cRegRead(addrt,datat,1,1,1,0,0);
	printf("addr[0]=%d,datatbl[0]=%d\n", (UINT16)addrt[0], (UINT16)datat[0]);

	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);
	#endif
	
	/* after power on. */
	frontAfterPowerOn();

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
	//UINT8 pvGainIdxVal;
	frontPrevCapabDesc_t *pcap;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 3, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);
	#if 0
	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
	}
	#endif

	/* Preview Mode Switch Post Proc */
	//frontPreviewModeSwitchPostProc();
	#if 0
		if(osSysModeGet()==OS_SYS_MODE_VIDEO_PV)
		{	
			//sp1kCdspTDEn(1,2);
			//XBYTE[REG_Cdsp_3Dtden] &= 0xfe; /*disable TD*/
		}
		else{
			//XBYTE[REG_Cdsp_3Dtden] &= 0xfe; /*disable TD*/
		}
	#endif
	cdspBayerPatternSet(BAYER_PTN_PREV);

	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */
	/*XBYTE[0x21b0] = XBYTE[0x21b0]|0x11;
	XBYTE[0x21b4] =  0x00;
	XBYTE[0x21b5] =  0x20;
	XBYTE[0x21b7] =  0x28;*/

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
frontOpPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvGainIdxVal;
	SINT16 pv_gId = 0;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (frontPrevSensorZoomSnapStatus == 1) {
		frontPrevSensorZoomSnapStatus = 0;
		return;
	}
	
	if ( frontPrevSensorZoomStatus == 0 ) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_30FPS_PREV) {//2x
			
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front30fpsPrevDz2xCmdTbl, 2, sizeof(front30fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontGainSet(0,pvGainIdxVal,1,0);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
	
	}
	else if ( frontPrevSensorZoomStatus == 1) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_30FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front30fpsPrevDz1xCmdTbl, 1, sizeof(front30fpsPrevDz1xCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontGainSet(0,pvGainIdxVal,1,0);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			#endif
		}
		
		
	}
	
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
frontOpIntrPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvGainIdxVal;
	SINT16 pv_gId = 0, paraAry;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (frontPrevSensorZoomSnapStatus == 1) {
		frontPrevSensorZoomSnapStatus = 0;
		return;
	}
	
	if ( frontPrevSensorZoomStatus == 0 ) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_30FPS_PREV) {//2x
			
				aeIntrCurGainValueGet(&pvGainIdxVal);

				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front30fpsPrevDz2xCmdTbl, 1, sizeof(front30fpsPrevDz2xCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
				
				frontPrevSensorZoom2xGainEn = 1;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 1;
				//printf("frontOpPreviewSensorZoomModeSet 2x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
			}
		
	}
	else if ( frontPrevSensorZoomStatus == 1) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			#if (SENSOR_ZOOM_FACTOR_30FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_30FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);

				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				
				i2cCmdTableSend_Intr(front30fpsPrevDz1xCmdTbl, 1, sizeof(front30fpsPrevDz1xCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

				frontPrevSensorZoom2xGainEn = 0;
				frontIntrGainSet(0,pvGainIdxVal,1,0);

				frontPrevSensorZoomStatus = 0;
				//printf("frontOpPreviewSensorZoomModeSet 1x\n");
			}
			#else
			factor = factor;
			pvGainIdxVal = pvGainIdxVal;
			paraAry = paraAry;
			#endif
		}
	}
	
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

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 3, 0);	
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

	//printf("first=%bd,mode=%bd\n",first,mode);
	dbgTimerStop();
	if ( frontPrevSensorZoom2xGainEn ) {
		frontPrevSensorZoomSnapStatus = 1;
	}
	
	if ( first != 0 ) {
		#if (DBG_SNAP_TIMING >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		#if 0
		#if (VSYNC_INV_PREV >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
	
		frontResCmdBinSend(_frontSnapCmdTbl);
			
		frontSnapExposureTimeSet(mode,scaleUp);
		#endif
	}

	if (mode != SP1K_FRONT_DOFRONT){  /*will: normal capture*/

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		
		#if (VSYNC_INV_SNAP >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif
		
		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
		cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
		cdspSkipWrSyncSet(1);

		#if (VSYNC_INV_SNAP >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#endif

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		#if (VSYNC_INV_SNAP >= 1)
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		#else
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		#endif

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		
		frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
		#endif

		HAL_CdspEofWait(1);

		#if (DBG_SNAP_TIMING >= 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

	}	
	else{  	/* snap do flow */
		frontSnapCapabDesc_t *pcap;
		pcap = &(frontSnapCapabDesc[frontSnapMode]);
		dbgTimerStop();
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		cdspSkipWrSyncSet(1);	
		cdspSkipWrSet(CDSP_SKIP_ALL,0xff);
		
		frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

		if (scaleUp==1){
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDivBurstUp, pcap->mclkPhase, 0, 0);
		}
		else{
			frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDivBurst, pcap->mclkPhase, 0, 0);
		}

		//dbgTimerStop();
	}
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
#define ANALOG_GAIN_1 64	// 1.00x
#define ANALOG_GAIN_2 105	// 1.64x	// 90  // 1.4x
#define ANALOG_GAIN_3 119	// 1.86x	// 115  // 1.8x
#define ANALOG_GAIN_4 206	// 3.22x	// 163  // 2.56x
#define ANALOG_GAIN_5 235	// 3.64x	// 218  // 3.40x  3.40625
#define ANALOG_GAIN_6 398	// 6.22x	// 307  // 4.7x   4.75
#define ANALOG_GAIN_7 465	// 7.27x	// 438 
#define ANALOG_GAIN_8 775	// 12.11x	// 602 //546     // 8.5x  9.40625
#define ANALOG_GAIN_9 1149	// 17.95x	// 845 //787     // 12.3x  13.296875

__STATIC void
frontOpGainSet(
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[3*2];
	UINT16 temp;
	
	//UINT8 ADDR8=0x00;

	//Manual Gain Ctrl First Set REG bit 0x3503[1] to Enable
	//Gain = high bit in 0x350A[0], low bit in 0x350B[7:0]
	//Gain = (0x350B[6]+1)*(0x350B[5]+1)*(0x350B[4]+1)*(0x350B[3:0]/16+1)

	//printf("gain idx=%bu\n",val);

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
	//frontLastGain=200;
	if((ANALOG_GAIN_1<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_2))	
		{		//analog gain		
	  cmdTbl[0] = 0xb6;
	  cmdTbl[1] = 0x00;	
	  cmdTbl[2] = 0xb1;
	  cmdTbl[3] = frontLastGain>>6; 	
	  cmdTbl[4] = 0xb2;
	  cmdTbl[5] = (frontLastGain<<2)&0xfc; 		
	    }
	else if((ANALOG_GAIN_2<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_3))	
			{		//analog gain		
		  cmdTbl[0] = 0xb6;
		  cmdTbl[1] = 0x01; 
		  temp = 64*frontLastGain/ANALOG_GAIN_2;
		  cmdTbl[2] = 0xb1;
		  cmdTbl[3] = temp>>6; 	
		  cmdTbl[4] = 0xb2;
		  cmdTbl[5] = (temp<<2)&0xfc;		
			}
	else if((ANALOG_GAIN_3<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_4))	
			{		//analog gain		
		  cmdTbl[0] = 0xb6;
		  cmdTbl[1] = 0x02; 
		  temp = 64*frontLastGain/ANALOG_GAIN_3;
		  cmdTbl[2] = 0xb1;
		  cmdTbl[3] = temp>>6; 	
		  cmdTbl[4] = 0xb2;
		  cmdTbl[5] = (temp<<2)&0xfc;		
			}
	else if((ANALOG_GAIN_4<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_5))	
			{		//analog gain		
		  cmdTbl[0] = 0xb6;
		  cmdTbl[1] = 0x03; 
		  temp = 64*frontLastGain/ANALOG_GAIN_4;
		  cmdTbl[2] = 0xb1;
		  cmdTbl[3] = temp>>6; 	
		  cmdTbl[4] = 0xb2;
		  cmdTbl[5] = (temp<<2)&0xfc;		
			}
		else if((ANALOG_GAIN_5<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_6))	
					{		//analog gain		
		 cmdTbl[0] = 0xb6;
		 cmdTbl[1] = 0x04; 
		 temp = 64*frontLastGain/ANALOG_GAIN_5;
	     cmdTbl[2] = 0xb1;
		 cmdTbl[3] = temp>>6;	
		 cmdTbl[4] = 0xb2;
		 cmdTbl[5] = (temp<<2)&0xfc;		
					}
  		else if((ANALOG_GAIN_6<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_7))	
					{		//analog gain		
		 cmdTbl[0] = 0xb6;
		 cmdTbl[1] = 0x05; 
		 temp = 64*frontLastGain/ANALOG_GAIN_6;
	     cmdTbl[2] = 0xb1;
		 cmdTbl[3] = temp>>6;	
		 cmdTbl[4] = 0xb2;
		 cmdTbl[5] = (temp<<2)&0xfc;		
					}
		else if((ANALOG_GAIN_7<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_8))	
					{		//analog gain		
		 cmdTbl[0] = 0xb6;
		 cmdTbl[1] = 0x06; 
		 temp = 64*frontLastGain/ANALOG_GAIN_7;
	     cmdTbl[2] = 0xb1;
		 cmdTbl[3] = temp>>6;	
		 cmdTbl[4] = 0xb2;
		 cmdTbl[5] = (temp<<2)&0xfc;		
					}
		else if((ANALOG_GAIN_8<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_9))
					{		//analog gain		
		 cmdTbl[0] = 0xb6;
		 cmdTbl[1] = 0x07; 
		 temp = 64*frontLastGain/ANALOG_GAIN_8;
		 cmdTbl[2] = 0xb1;
		 cmdTbl[3] = temp>>6;	
		 cmdTbl[4] = 0xb2;
		 cmdTbl[5] = (temp<<2)&0xfc;		
					}
		else if(ANALOG_GAIN_9<= frontLastGain)	
					{		//analog gain		
		 cmdTbl[0] = 0xb6;
		 cmdTbl[1] = 0x08; 
		 temp = 64*frontLastGain/ANALOG_GAIN_9;
		 cmdTbl[2] = 0xb1;
		 cmdTbl[3] = temp>>6;	
		 cmdTbl[4] = 0xb2;
		 cmdTbl[5] = (temp<<2)&0xfc;		
					}

	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);	
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);
	//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
	//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);
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
	#ifdef SUPORT_FACTORY_CAL_BP
	UINT8 cmdTbl[4*2]={0};
	UINT16 compensation_line = 0;
	#else
	UINT8 cmdTbl[2*2]={0};
	#endif
	//UINT16 REG_ADDR16[3]={0x3500, 0x3501, 0x3502};
	//UINT8 REG_ADDR[3];

	//Manual Exposure Ctrl First Set REG bit 0x3503[0] to Enable
	//Exposure Value = 0x3500[19:16], 0x3501[15:8], 0x3502[7:0], units of 1/16 line

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;

	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk/2 * 10) / (/*1083*/(ltotal/2) * frontLastShutter);
	//printf("espline=%d\n",espline);
	//espline = (24000000 * 10) / (960 * frontLastShutter);
	#if 0
	if (espline != 0) {
		if(espline > (ftotal - 6)) {
			espline = (ftotal - 6);
			//printf("re-define espline=%d\n",espline);
		}
	}
	else {
		espline = 1;
	}
	#else
	#ifdef SUPORT_FACTORY_CAL_BP
	if(bpDoing)
	{
		if (espline != 0) {
		if(espline > (ftotal - 6)) {
			/* add dummy line to reduce fps */
			compensation_line = espline - ftotal + 6;//(espline + 6) - ftotal;
		}
	}
	else {
		espline = 1;
	}

     //espline_16 = (UINT32)espline << 4;
	//printf("espline_16=%lu\n",espline_16);

	/* VTS DIFF set */
	cmdTbl[0] = 0x07;
	cmdTbl[1] = (UINT8)((compensation_line & 0xFF00)>>8);
	cmdTbl[2] = 0x08;
	cmdTbl[3] = (UINT8)(compensation_line & 0x00FF);
	}
	else
	#endif
	{
		if(espline>8191) espline=8191;
		else if(espline<1) 
		espline=1;
	}
	
	#endif
	
   // espline_16 = espline << 4;
	//printf("espline=%lu\n",espline);
	//espline_16=720;
	#ifdef SUPORT_FACTORY_CAL_BP
	cmdTbl[4] = 0x03;
	cmdTbl[5] = (UINT8)((espline & 0x1F00) >> 8);
	cmdTbl[6] = 0x04;
	cmdTbl[7] = (UINT8)(espline & 0x00FF);	
	#else
	cmdTbl[0] = 0x03;
	cmdTbl[1] = (UINT8)((espline & 0x1F00) >> 8);
	cmdTbl[2] = 0x04;
	cmdTbl[3] = (UINT8)(espline & 0x00FF);
	#endif
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);
	

	//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,3,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2]);
}

/**
 * @brief	operating function to set gain value for intr.
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
frontOpIntrGainSet(
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[3*2];
	//UINT8 ADDR8=0x00;
	UINT16 temp;

	//Manual Gain Ctrl First Set REG bit 0x3503[1] to Enable
	//Gain = high bit in 0x350A[0], low bit in 0x350B[7:0]
	//Gain = (0x350B[6]+1)*(0x350B[5]+1)*(0x350B[4]+1)*(0x350B[3:0]/16+1)

	//printf("gain idx=%bu\n",val);

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	if((ANALOG_GAIN_1<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_2))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x00; 
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = frontLastGain>>6; 	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (frontLastGain<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_2<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_3))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x01; 
		temp = 64*frontLastGain/ANALOG_GAIN_2;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_3<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_4))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x02; 
		temp = 64*frontLastGain/ANALOG_GAIN_3;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_4<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_5))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x03; 
		temp = 64*frontLastGain/ANALOG_GAIN_4;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_5<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_6))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x04; 
		temp = 64*frontLastGain/ANALOG_GAIN_5;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;
	}
	else if((ANALOG_GAIN_6<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_7))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x05; 
		temp = 64*frontLastGain/ANALOG_GAIN_6;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_7<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_8))	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x06; 
		temp = 64*frontLastGain/ANALOG_GAIN_7;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if((ANALOG_GAIN_8<= frontLastGain)&&(frontLastGain < ANALOG_GAIN_9))
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x07; 
		temp = 64*frontLastGain/ANALOG_GAIN_8;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}
	else if(ANALOG_GAIN_9<= frontLastGain)	
	{	//analog gain		
		cmdTbl[0] = 0xb6;
		cmdTbl[1] = 0x08; 
		temp = 64*frontLastGain/ANALOG_GAIN_9;
		cmdTbl[2] = 0xb1;
		cmdTbl[3] = temp>>6;	
		cmdTbl[4] = 0xb2;
		cmdTbl[5] = (temp<<2)&0xfc;		
	}

	#if 0
	if(ANALOG_GAIN_6<= frontLastGain)
	{
		XBYTE[0x2121] = 0x01;
		XBYTE[0x21b7] =  0x18;
		//sp1kCdspTDEn(1,1);

		XBYTE[0x2174] = 0xA7;
		XBYTE[0x2176] = 0x23;
		XBYTE[0x2178] = 0x10;
		XBYTE[0x217A] = 0xBE;
		XBYTE[0x217C] = 0x22;
		XBYTE[0x217E] = 0x10;
		XBYTE[0x2192] = 0x39;
		XBYTE[0x2194] = 0x03;
		XBYTE[0x2195] = 0x00;
		XBYTE[0x2198] = 0xE3;
		XBYTE[0x219A] = 0x6E;
		XBYTE[0x219C] = 0xF0;
		XBYTE[0x219E] = 0xF4;
		XBYTE[0x21A0] = 0xE8;
		XBYTE[0x21A2] = 0x64;
		
	}
	else if(frontLastGain < ANALOG_GAIN_4)
	{
		XBYTE[0x2121] = 0x11;
		XBYTE[0x21b7] =  0x28;
		//sp1kCdspTDEn(0,0);
		
		XBYTE[0x2174] = 0xA1;
		XBYTE[0x2176] = 0x21;
		XBYTE[0x2178] = 0x11;
		XBYTE[0x217A] = 0x6B;
		XBYTE[0x217C] = 0x23;
		XBYTE[0x217E] = 0x00;
		XBYTE[0x2192] = 0x59;
		XBYTE[0x2194] = 0xE4;
		XBYTE[0x2195] = 0x03;
		XBYTE[0x2198] = 0xD4;
		XBYTE[0x219A] = 0x82;
		XBYTE[0x219C] = 0xEA;
		XBYTE[0x219E] = 0xF2;
		XBYTE[0x21A0] = 0xB5;
		XBYTE[0x21A2] = 0x99;
	}
	#endif

	//printf("cmdTbl[1]=0x%bx\n",cmdTbl[1]);
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, isr, upd);
	
	//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
	//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);

}

/**
 * @brief	operating function to set exposure time value for intr.
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
__STATIC void frontOpIntrExposureTimeSet(
	frontExposureTimeArg_t *parg
)
{
	frontPrevCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 pixclk;
	UINT8 cmdTbl[2*2];
	//UINT16 REG_ADDR16[3]={0x3500, 0x3501, 0x3502};
	//UINT8 REG_ADDR[3];

	//Manual Exposure Ctrl First Set REG bit 0x3503[0] to Enable
	//Exposure Value = 0x3500[19:16], 0x3501[15:8], 0x3502[7:0], units of 1/16 line

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastShutter = parg->val;

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;

	if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {
		pixclk = pcap->prevDynamicTDTbl->pclkFreq;
	}
	else {
		pixclk = pcap->pclkFreq;
	}

	//printf("frontLastShutter=%lu\nltotal=%d\nftotal=%d\npixclk=%lu\n",frontLastShutter,ltotal,ftotal,pixclk);
	espline = (pixclk/2 * 10) / (/*1083*/(ltotal/2) * frontLastShutter);

	//espline = (24000000 * 10) / (960 * frontLastShutter);
#if 0
	if (espline != 0) {
		if(espline > (ftotal - 6)) {
			espline = (ftotal - 6);
			//printf("re-define espline=%d\n",espline);
		}
	}
	else {
		espline = 1;
	}
#else
        if(espline>8191) espline=8191;
		else if(espline<1)  espline=1;
	
#endif

	
	//espline_16 = espline << 4;
	//printf("espline=%d\n",espline);
	//espline_16=720;

	cmdTbl[0] = 0x03;
	cmdTbl[1] = (UINT8)((espline & 0x1F00) >> 8);
	cmdTbl[2] = 0x04;
	cmdTbl[3] = (UINT8)(espline & 0x00FF);
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);


	//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,3,1,1,0); 
	//printf("espline=%d\n",espline);
	//printf("read=0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2]);
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
 * @brief	operating function to set preview mode dynamic temporal denoise intr flow.
 * @param	[in] preview mode dynamic temporal denoise enable/disable.
 *						enable : FRONT_PV_DYNAMIC_TD_ENABLE
 *						disable : FRONT_PV_DYNAMIC_TD_DISABLE
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-15
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpIntrPreviewDynamicTempoalDenoiseSet(
	UINT8 en
)
{
	frontPrevCapabDesc_t *pcap;
	SINT16 paraAry[4];
	UINT8 upd;
		
	if (frontPrevDyTdSetFlow == 0) {
		
		//printf("front flag\n");
		
		frontPrevDyTdSetFlow = 1;
		
		frontPrevDyTdEn = en;
		frontPrevDyTdSwitchPclk = 1;
		frontPrevDyTdSwitchExp = 1;
		
		if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_DISABLE) {
			frontPrevDyTdSwitchGain = 1;//-gain idx
		}
		else {
			frontPrevDyTdSwitchGain = 2;//+gain idx
		}				
	}
	else {
		
		//printf("front pclk");
		
		frontPrevDyTdSetFlow = 0;
		
		pcap = &(frontPrevCapabDesc[frontPrevMode]);
		
		if (frontPrevDyTdSwitchPclk == 1) {
			frontPrevDyTdSwitchPclk = 0;
			
			if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {

				//printf("[en]\n");
				
				paraAry[0] = MCLK_SRC;
				paraAry[1] = pcap->prevDynamicTDTbl->mclkDiv;
				paraAry[2] = pcap->prevDynamicTDTbl->mclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
				paraAry[0] = PCLK_SRC;
				paraAry[1] = pcap->prevDynamicTDTbl->pclkDiv;
				paraAry[2] = pcap->prevDynamicTDTbl->pclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
			}
			else {
				
				//printf("[dis]\n");
				
				paraAry[0] = MCLK_SRC;
				paraAry[1] = pcap->mclkDiv;
				paraAry[2] = pcap->mclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
				paraAry[0] = PCLK_SRC;
				paraAry[1] = pcap->pclkDiv;
				paraAry[2] = pcap->pclkPhase;
				paraAry[3] = 0;
				upd= 0;
				HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
			}
		}
	}
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
	gpioByteFuncSet(GPIO_BYTE_GEN0, (1 << ( 2 & 0x07)), (1 << ( 2 & 0x07)));  //gpio 2 reset
	gpioByteDirSet(GPIO_BYTE_GEN0, (1 << ( 2 & 0x07)), (1 << ( 2 & 0x07)));
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 2 & 0x07)), (1 << ( 2 & 0x07)));
	sp1kHalWait(20);
	
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 2 & 0x07)), (0 << ( 2 & 0x07)));
	sp1kHalWait(100);
	
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 2 & 0x07)), (1 << ( 2 & 0x07)));
	sp1kHalWait(20);
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
	UINT32 pixclk;
	UINT8 cmdTbl[2*5]={0};
	UINT16 s_gain;
	UINT16 temp;
	#if (SNAP_KEEP_FPS == 0)
	UINT16 compensation_line = 0;
	#endif
	/* Before Snap ExposureTime Set */
	#ifdef SUPORT_FACTORY_CAL_BP
	if(!bpDoing)
	#endif
	{
	frontBeforeSnapExposureTimeSet();
	}
	
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
	
	//printf("ltotal=%d,ftotal=%d,pixclk=%lu\n",ltotal,ftotal,pixclk);
	
	#ifdef SUPORT_FACTORY_CAL_BP
	if(bpDoing)
	{
		espline = (UINT32)(pixclk * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]) / (UINT32)(ltotal * frontLastShutter);// 2*2 binning mode on need used
	}
	else
	#endif
	{
		espline = (UINT32)(pixclk * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]) / (UINT32)(ltotal * frontSnapLastShutter);// 2*2 binning mode on need used
	}
	//printf("espline=%d\n",espline);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 1 ) {  /* 30 fps. */
		}
		else if ( frontPrevMode == 0 ) {	/* 24fps */
		}
	}

	#ifdef SUPORT_FACTORY_CAL_BP
	if(bpDoing)
	{
		s_gain = frontLastGain;
	}
	else
	#endif
		s_gain = frontSnapLastGain;

	if (espline != 0) {
		#if 0
		if(espline > (ftotal - 6)) {
			#if (SNAP_KEEP_FPS == 0)
			/* add dummy line to reduce fps */
			compensation_line = espline - ftotal + 6;//(espline + 6) - ftotal;
			//printf("compensation_line=%d\n",compensation_line);
			#else
			/* keep fps */
			espline = (ftotal - 6);
			//printf("re-define snap espline=%d\n",espline);
			#endif
		}
		#endif
		if(espline>8191) espline=8191;
	}
	else {
		espline = 1;
	}

     //espline_16 = (UINT32)espline << 4;
	//printf("espline_16=%lu\n",espline_16);

	/* VTS DIFF set */
	/*cmdTbl[0] = 0x07;
	cmdTbl[1] = (UINT8)((compensation_line & 0xFF00)>>8);
	cmdTbl[2] = 0x08;
	cmdTbl[3] = (UINT8)(compensation_line & 0x00FF);*/

	/* exposure time set */
	cmdTbl[0] = 0x03;
	cmdTbl[1] = (UINT8)((espline & 0x1F00) >> 8);
	cmdTbl[2] = 0x04;
	cmdTbl[3] = (UINT8)(espline & 0x00FF);

	//printf("pre-s_gain=%d\n",s_gain);
	
	/* gain set */
	if((ANALOG_GAIN_1<= s_gain)&&(s_gain < ANALOG_GAIN_2))	
			{		//analog gain		
		  cmdTbl[4] = 0xb6;
		  cmdTbl[5] = 0x00; 
		  cmdTbl[6] = 0xb1;
		  cmdTbl[7] = s_gain>>6; 	
		  cmdTbl[8] = 0xb2;
		  cmdTbl[9] = (s_gain<<2)&0xfc;		
			}
		else if((ANALOG_GAIN_2<= s_gain)&&(s_gain < ANALOG_GAIN_3))	
				{		//analog gain		
			  cmdTbl[4] = 0xb6;
			  cmdTbl[5] = 0x01; 
			  temp = 64*s_gain/ANALOG_GAIN_2;
			  cmdTbl[6] = 0xb1;
			  cmdTbl[7] = temp>>6;	
			  cmdTbl[8] = 0xb2;
			  cmdTbl[9] = (temp<<2)&0xfc;		
				}
		else if((ANALOG_GAIN_3<= s_gain)&&(s_gain < ANALOG_GAIN_4))	
				{		//analog gain		
			  cmdTbl[4] = 0xb6;
			  cmdTbl[5] = 0x02; 
			  temp = 64*s_gain/ANALOG_GAIN_3;
			  cmdTbl[6] = 0xb1;
			  cmdTbl[7] = temp>>6;	
			  cmdTbl[8] = 0xb2;
			  cmdTbl[9] = (temp<<2)&0xfc;		
				}
		else if((ANALOG_GAIN_4<= s_gain)&&(s_gain < ANALOG_GAIN_5))	
				{		//analog gain		
			  cmdTbl[4] = 0xb6;
			  cmdTbl[5] = 0x03; 
			  temp = 64*s_gain/ANALOG_GAIN_4;
			  cmdTbl[6] = 0xb1;
			  cmdTbl[7] = temp>>6;	
			  cmdTbl[8] = 0xb2;
			  cmdTbl[9] = (temp<<2)&0xfc;		
				}
			else if((ANALOG_GAIN_5<= s_gain)&&(s_gain < ANALOG_GAIN_6))	
						{		//analog gain		
			 cmdTbl[4] = 0xb6;
			 cmdTbl[5] = 0x04; 
			 temp = 64*s_gain/ANALOG_GAIN_5;
			 cmdTbl[6] = 0xb1;
			 cmdTbl[7] = temp>>6;	
			 cmdTbl[8] = 0xb2;
			 cmdTbl[9] = (temp<<2)&0xfc;		
						}
			else if((ANALOG_GAIN_6<= s_gain)&&(s_gain < ANALOG_GAIN_7))	
						{		//analog gain		
			 cmdTbl[4] = 0xb6;
			 cmdTbl[5] = 0x05; 
			 temp = 64*s_gain/ANALOG_GAIN_6;
			 cmdTbl[6] = 0xb1;
			 cmdTbl[7] = temp>>6;	
			 cmdTbl[8] = 0xb2;
			 cmdTbl[9] = (temp<<2)&0xfc;		
						}
			else if((ANALOG_GAIN_7<= s_gain)&&(s_gain < ANALOG_GAIN_8))	
						{		//analog gain		
			 cmdTbl[4] = 0xb6;
			 cmdTbl[5] = 0x06; 
			 temp = 64*s_gain/ANALOG_GAIN_7;
			 cmdTbl[6] = 0xb1;
			 cmdTbl[7] = temp>>6;	
			 cmdTbl[9] = 0xb2;
			 cmdTbl[9] = (temp<<2)&0xfc;		
						}
			else if((ANALOG_GAIN_8<= s_gain)&&(s_gain < ANALOG_GAIN_9))
						{		//analog gain		
			 cmdTbl[4] = 0xb6;
			 cmdTbl[5] = 0x07; 
			 temp = 64*s_gain/ANALOG_GAIN_8;
			 cmdTbl[6] = 0xb1;
			 cmdTbl[7] = temp>>6;	
			 cmdTbl[8] = 0xb2;
			 cmdTbl[9] = (temp<<2)&0xfc;		
						}
			else if(ANALOG_GAIN_9<= s_gain)	
						{		//analog gain		
			 cmdTbl[4] = 0xb6;
			 cmdTbl[5] = 0x08; 
			 temp = 64*s_gain/ANALOG_GAIN_9;
			 cmdTbl[6] = 0xb1;
			 cmdTbl[7] = temp>>6;	
			 cmdTbl[8] = 0xb2;
			 cmdTbl[9] = (temp<<2)&0xfc;		
						}


	//printf("re-s_gain=0x%bx\n",cmdTbl[11]);
	
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	
	
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
	frontPrevCapabDesc_t *pcap;
	UINT8 compensationGainIdx = 0, lastGainIdx, pvGainIdxVal;
	UINT8 isoCfg, isoValue, isoGainIdx, autoGainIdx, adjustGainIdx, pvBanding, capBanding, pwrFreq, limitShutterFps;
	UINT16 maxGainTblIdx;
	UINT16 isoNumerator = 0, autoNumerator = 0;
	UINT16 isoDenominator = 0, autoDenominator = 0;
	UINT32 isoShutter, limitShutter, bandingShutter, adjustShutter;
	#if (PV_AUTO_OB_BY_GAIN == 1)
	UINT8 minIdx, maxIdx;
	#endif
	
	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* get max gain table index */
	maxGainTblIdx = (UINT16)((sizeof(gainTbl)/sizeof(UINT16))-1);
	//printf("maxGainTblIdx=0x%x\n",maxGainTblIdx);

	/* get capture iso mode */
	aeStatusGet(SP1K_AE_CAPTURE_ISO_MODE,&isoCfg);
	//printf("iso=%bu\n",isoCfg);

	/* get pv gain idx */
	sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
	//printf("pvGainIdxVal=0x%bx\n", pvGainIdxVal);

	/* get capture compensation gain idx */
	sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);
	//printf("compensationGainIdx=0x%bx\n", compensationGainIdx);

	/* get current power freq */
	sp1kAeStatusGet(SP1K_AE_G_AETblSel, &pwrFreq);
	//printf("pwrFreq=%bu\n", pwrFreq);

	/* get anti-shake limit fps */
	sp1kAeStatusGet(SP1K_AE_STS_CAP_ANTISHAKE_FPS, &limitShutterFps);
	
	/* calculate atuo iso gain idx */
	autoGainIdx = pvGainIdxVal + compensationGainIdx;

	/* set iso gain or atuo iso gain idx */
	switch (isoCfg) {
		case SP1K_AE_ISO_AUTO:			
			if (autoGainIdx > maxGainTblIdx) {
				autoGainIdx = maxGainTblIdx;
			}
			break;
		case SP1K_AE_ISO_100:
			/* calculate last gain */
			isoGainIdx = ISO_100_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_200:
			/* calculate last gain */
			isoGainIdx = ISO_200_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_400:
			/* calculate last gain */
			isoGainIdx = ISO_400_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		case SP1K_AE_ISO_800:
			/* calculate last gain */
			isoGainIdx = ISO_800_GAIN_IDX;
			ASSERT(!(isoGainIdx > maxGainTblIdx),1);
			break;
		default:
			/* not this case */
			ASSERT(0,1);
			break;
	}
	//printf("isoCfg=%bu,autoGainIdx=0x%bx,isoGainIdx=0x%bx\n",isoCfg,autoGainIdx,isoGainIdx);

	/* switch iso mode */
	if (isoCfg != SP1K_AE_ISO_AUTO) {

		/* set capture exif iso tag value */
		isoValue = isoCfg;
		
		/* auto iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[autoGainIdx],&autoNumerator,&autoDenominator);
		//printf("autoGain=0x%x,%d/%d\n",pcap->gainTbl[autoGainIdx],autoNumerator,autoDenominator);
		
		/* user define iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[isoGainIdx],&isoNumerator,&isoDenominator);
		//printf("isoGain=0x%x,%d/%d\n",pcap->gainTbl[isoGainIdx],isoNumerator,isoDenominator);

		isoShutter = (UINT32)isoNumerator * frontLastShutter * autoDenominator / (isoDenominator * autoNumerator);
		//printf("frontLastShutter=%lu,isoShutter=%lu\n",frontLastShutter,isoShutter);

		if (limitShutterFps != 0) {
			/* to limit exposure time small than CAPTURE_LIMIT_FPS */
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]);
			if (isoShutter < limitShutter) {
				isoShutter = limitShutter;
				//printf("limitShutter=%lu\n",limitShutter);
			}
		}


		#if (ISO_CAPTURE_AUTO_DEBANDING == 1)
		/* get banding shutter number */
		bandingShutter = (UINT32)10*pwrFreq*2;

		/* get banding type */
		pvBanding = ((frontLastShutter > bandingShutter) ? 1 : 0);
		//capBanding = ((isoShutter/frontPrevToSnapBinningRateTbl[frontPrevMode] > bandingShutter) ? 1 : 0);
		capBanding = ((isoShutter > bandingShutter/**frontPrevToSnapBinningRateTbl[frontPrevMode]*/) ? 1 : 0);
		//printf("pvBanding=%bu,capBanding=%bu\n",pvBanding,capBanding);

		/* check preview not banding capture banding case */
		if (pvBanding == 0 && capBanding == 1) {
			lastGainIdx = autoGainIdx;
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = frontLastShutter;
		}
		else {
			lastGainIdx = isoGainIdx;
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = isoShutter;
		}
		#else	
		lastGainIdx = isoGainIdx;
		frontSnapLastGain = pcap->gainTbl[lastGainIdx];
		frontSnapLastShutter = isoShutter;
		#endif
	}
	else {

		lastGainIdx = autoGainIdx;
		
		if ( lastGainIdx > ISO_400_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_800;
		}
		else if ( lastGainIdx > ISO_200_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_400;
		}
		else if ( lastGainIdx > ISO_100_GAIN_IDX ) {
			isoValue = SP1K_AE_ISO_200;
		}
		else {
			isoValue = SP1K_AE_ISO_100;
		}
		//printf("isoValue=%bu\n",isoValue);		

		if (limitShutterFps != 0) {
			
			/* to limit exposure time small than CAPTURE_LIMIT_FPS */
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]);

			adjustShutter = frontLastShutter;

			//printf("limitShutter=%lu,%lu\n",limitShutter,adjustShutter);
			
			while ( adjustShutter < limitShutter ) {

				/* check max gain idx */
				if (isoValue != (SP1K_AE_ISO_MAX - 1)) {

					if ( isoValue < (SP1K_AE_ISO_MAX - 1) ) {
						isoValue++;
						adjustShutter = (adjustShutter << 1);
						//printf("isoValue=%bu,%lu\n",isoValue,adjustShutter);
					}

					switch (isoValue) {
						case SP1K_AE_ISO_800:
							adjustGainIdx = lastGainIdx + (ISO_800_GAIN_IDX - ISO_400_GAIN_IDX);
							break;
						case SP1K_AE_ISO_400:
							adjustGainIdx = lastGainIdx + (ISO_400_GAIN_IDX - ISO_200_GAIN_IDX);
							break;
						case SP1K_AE_ISO_200:
							adjustGainIdx = lastGainIdx + (ISO_200_GAIN_IDX - ISO_100_GAIN_IDX);
							break;
						case SP1K_AE_ISO_100:
							adjustGainIdx = lastGainIdx;
							break;
					}
					lastGainIdx = adjustGainIdx;
				}
				else {
					adjustShutter = limitShutter;
					break;
				}
			}

			frontSnapLastGain = pcap->gainTbl[lastGainIdx];			
			frontSnapLastShutter = adjustShutter;
			//printf("frontSnapLastShutter=%lu,0x%x\n",frontSnapLastShutter,frontSnapLastGain);

		}
		else {
			frontSnapLastGain = pcap->gainTbl[lastGainIdx];
			frontSnapLastShutter = frontLastShutter;
		}
	}

	/* set capture exif iso tag value */
	sp1kAeParamSet(SP1K_AE_CAPTURE_ISO_VALUE, isoValue);
	
	//printf("frontLastGain=0x%x,frontLastShutter=%lu\n",frontLastGain,frontLastShutter);
	//printf("frontSnapLastGain=0x%x,frontSnapLastShutter=%lu\n",frontSnapLastGain,frontSnapLastShutter); 

	#if (PV_AUTO_OB_BY_GAIN == 1)
	if (pvGainIdxVal == lastGainIdx) {
		/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
		LoadBackfrontLastSetting = 1;

		minIdx = pcap->gainMinIdx;
		maxIdx = pcap->gainMaxIdx;
		//printf("minIdx=0x%bx,maxIdx=0x%bx\n",minIdx,maxIdx);
		
		/* auto ob by gain */
		if (pvGainIdxVal == minIdx) {
			LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
		}
		else {//(pvGainIdxVal == maxIdx)  & other case
			LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
		}		
	}
	else {
		LoadBackfrontLastGainIdx = pvGainIdxVal;
	}
	#else
	LoadBackfrontLastGainIdx = pvGainIdxVal;
	#endif
	//printf("pvGainIdxVal=0x%bx, LoadBackfrontLastGainIdx=0x%bx\n", pvGainIdxVal, LoadBackfrontLastGainIdx);
	
}

/**
 * @brief	Sensor Gain Table Value Transfer Func.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontGainTblValueTransfer(
	UINT16 value,
	UINT16 *numerator,
	UINT16 *denominator
)
{
	#if 0
	*numerator = (16+(value&0x000F))*((value&0x0010)?2:1)*((value&0x0020)?2:1)*((value&0x0040)?2:1);
	*denominator = 16;
	#else
	*numerator = (16+(value&0x000F));
	*denominator = (((16/((value&0x0010)?2:1))/((value&0x0020)?2:1))/((value&0x0040)?2:1));	
	#endif
}


/**
 * @brief	Preview Mode Switch Post Proc Set.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-09-24
 * @todo	N/A
 * @bug		N/A
*/
static void
frontPreviewModeSwitchPostProc(
	void
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 pvFreq, gIdval;
	SINT16 pv_gId = 0;
	UINT8 minIdx, maxIdx;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontPrevSensorZoomStatus = 0;
	frontPrevSensorZoom2xGainEn = 0;

	/* power on status */
	if (frontPowerOnStatus == 1) {
	
		/* sensor different resolution or fps switch check */
		if (frontPrevModeSave != frontPrevMode) {//(frameRateSetFlag == 1) {
			sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
			sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
			pv_gId = gIdval;
			//printf("pvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);
			
			if (frontPrevModeSave == 2 && frontPrevMode == 1) {
				//pv_gId += 20;
				//printf("60->30 +20\n");
			}
			else if (frontPrevModeSave == 2 && frontPrevMode == 0) {
				//pv_gId += 26;
				//printf("60->dyn30 +26\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
				pv_gId += 20;
				//printf("60->dyn30 +20\n");
			}
			else if (frontPrevModeSave == 1 && frontPrevMode == 2) {
				//pv_gId -= 20;
				//printf("30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 1) {
				pv_gId -= 20;
				//printf("dyn30->60 -20\n");
			}
			else if (frontPrevModeSave == 0 && frontPrevMode == 2) {
				//pv_gId -= 26;
				//printf("dyn30->60 -26\n");
			}
			
			if (pvFreq == SP1K_FLICKER_60) {
				minIdx = pcap->aeMinIdxTbl[0];
				maxIdx = pcap->aeMaxIdxTbl[0];
			}
			else {
				minIdx = pcap->aeMinIdxTbl[1];
				maxIdx = pcap->aeMaxIdxTbl[1];
			}

			//printf("min=%bu,max=%bu\n",minIdx,maxIdx);

			if(pv_gId < minIdx) {
				pv_gId = minIdx;
				//printf("pv_gId = %d < min\n",pv_gId);
			}

			if(pv_gId > maxIdx) {
				pv_gId = maxIdx;
				//printf("pv_gId = %d > max\n",pv_gId);
			}

			sp1kAeParamSet(SP1K_AE_gId,(UINT8)pv_gId);

			if (pvFreq == SP1K_FLICKER_60) {
				frontExposureTimeSet(0,pcap->ae60hzTbl[pv_gId].shutter,1,0);
				frontGainSet(0,pcap->ae60hzTbl[pv_gId].gain,1,0);
			}
			else {//(pvFreq == SP1K_FLICKER_50)
				frontExposureTimeSet(0,pcap->ae50hzTbl[pv_gId].shutter,1,0);
				frontGainSet(0,pcap->ae50hzTbl[pv_gId].gain,1,0);
			}
			
			frontPrevModeSave = frontPrevMode;
		}
		else {
			/* ov sensor auto ob by gain & snap to pv sensor shutter need re-setting */
			if (LoadBackfrontLastSetting == 1) {
				frontExposureTimeSet(0,frontLastShutter,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,frontLastGainIdx,1,0);
				//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				LoadBackfrontLastSetting = 0;
			}
			else {
				/* pv to video mode switch panel dumping */
				frontExposureTimeSet(0,frontLastShutter,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				frontGainSet(0,frontLastGainIdx,1,0);
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
				//printf("frontLastShutter=%lu, frontLastGainIdx=%bu\n",frontLastShutter,frontLastGainIdx);
			}
		}
	}
	else {
		frontPrevModeSave = frontPrevMode;
		frontPowerOnStatus = 1;
	}
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
	#if (LOAD_RES_OPTION == 1)
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

	sts = i2cCmdTableSend(G_FPGAMemPool, 1, size/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	if (sts == FAIL) {
		printf("Sensor command table (0x%x)\n",fileId);
		ASSERT(0,1);
	}
	
	#else
	UINT32 size;
	UINT8 *pData;
	UINT8 sts;
	
	switch (fileId) {
		case _frontInit0CmdTbl:
			pData = frontInit0CmdTbl;
			size = sizeof(frontInit0CmdTbl);
			break;
		case _frontInit1CmdTbl:
			pData = frontInit1CmdTbl;
			size = sizeof(frontInit1CmdTbl);
			break;

		case _frontInit2CmdTbl:
			pData = frontInit2CmdTbl;
			size = sizeof(frontInit2CmdTbl);
			break;
	}

	
	sts = i2cCmdTableSend(pData, 1, size/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	if (sts == FAIL) {
		printf("Sensor command table (0x%x)\n",fileId);
		ASSERT(0,1);
	}
	#endif
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
	#if (LOAD_RES_OPTION == 1)
	UINT32 size;
	UINT8 sts;

	if (byteAddress & 0x0001) {
		ASSERT(!(byteAddress & 0x0001), 0); 
		return;
	}
	
	//printf("fileId = 0x%04x, byteAddress = 0x%04x\n",fileId,byteAddress);
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
	if (size > FRONT_RES_TBL_MAX_SIZE) {
		ASSERT(!(size > FRONT_RES_TBL_MAX_SIZE), 0); 
		return;
	}

	//printf("size = %lu\n",size);

	do {
		sts = sp1kNandRsvRead(fileId, ((K_SDRAM_CodeSize + ((byteAddress - 0x4000) >> 1)) << 1));
	}while(!sts);
	#else
	fileId = fileId;
	byteAddress = byteAddress;
	#endif	
}


#if FUNC_HOST_IGBT_FLASH  /*MI2020*/
#define SENSOR_GAIN_1X_INDEX 0
#define SENSOR_GAIN_2X_INDEX 16
#define SENSOR_GAIN_4X_INDEX 48
#define SENSOR_GAIN_8X_INDEX 80

void frontLastShutterGainSet(UINT32 shutter, UINT8 gain)
{
	frontPrevCapabDesc_t *pcap;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	LoadBackfrontLastGainIdx = gain; /*Add for load preview setting after flash snap*/
	
	frontLastGain = pcap->gainTbl[gain];
	frontLastShutter = shutter;
}

sensor_flash_gain_t sensorFlashGainIdxGet(void)
{
	UINT8 currGain;
	sensor_flash_gain_t flashGainIdx;

	aeStatusGet(SP1K_AE_STS_G_GAIN , &currGain);	
	if(currGain < SENSOR_GAIN_2X_INDEX){
		flashGainIdx = SENSOR_GAIN_1X; 
	}
	else if(currGain < SENSOR_GAIN_4X_INDEX){
		flashGainIdx = SENSOR_GAIN_2X;
	}
	else if(currGain < /*SENSOR_GAIN_8X_INDEX*/79){
		flashGainIdx = SENSOR_GAIN_4X;
	}
	else{
		flashGainIdx = SENSOR_GAIN_8X;
	}
	return flashGainIdx;
}

UINT8 sensorGainIdxGet(sensor_flash_gain_t flashGainIdx)
{
	UINT8 gainIdx;

	switch(flashGainIdx){
		case  SENSOR_GAIN_1X:
			gainIdx = SENSOR_GAIN_1X_INDEX;
			break;
		case  SENSOR_GAIN_2X:
			gainIdx = SENSOR_GAIN_2X_INDEX; 
			break;
		case  SENSOR_GAIN_4X:
			gainIdx = SENSOR_GAIN_4X_INDEX;
			break;
		case  SENSOR_GAIN_8X:
			gainIdx = SENSOR_GAIN_8X_INDEX;
			break;
	}
	
	return gainIdx;

}
UINT16 sensorPvToSnapFrmTimeRatio(void)
{
	UINT16 ratio;

	UINT32 capTime;
	UINT32 preTime;

	capTime = ( (((UINT32)LINE_TOT_SNAP * (UINT32)FRAME_TOT_SNAP) <<8) /(UINT32)PCLK_FREQ_SNAP>>2);

	preTime = ( (((UINT32)LINE_TOT_30FPS_PREV * (UINT32)FRAME_TOT_30FPS_PREV) <<8) /(UINT32)PCLK_FREQ_30FPS_PREV>>2);

	ratio = (UINT16)((capTime<<8)/preTime);

	if(ratio > 300){
		ratio = ratio - 50;
	}

	ratio = 450;
	printf("\033[32mratio=%u\033[0m\n",ratio);
	return ratio;
}
#endif

