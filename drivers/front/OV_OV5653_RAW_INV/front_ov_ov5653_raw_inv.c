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
 * @file		front_ov_ov5653_raw_inv.c
 * @brief		front sensor omni vision ov5653 driver.
 * @author		Matt Wang
 * @since		2008-09-24
 * @date		2008-12-09 
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
#include "ae_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_front_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

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
#define PREV_MODE_TOT  3
#define SNAP_MODE_TOT  1

/* dimensiion. */

/* 960p, 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       1
#define YOFF_30FPS_PREV       2
#define LINE_TOT_30FPS_PREV   2132
#define FRAME_TOT_30FPS_PREV  1018
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  4
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0//200

/* 720p, 50fps */
#define WIDTH_50FPS_PREV      1280
#define HEIGHT_50FPS_PREV     720
#define XOFF_50FPS_PREV       1
#define YOFF_50FPS_PREV       2
#define LINE_TOT_50FPS_PREV   2162
#define FRAME_TOT_50FPS_PREV  782
#define PREV_TO_SNAP_BINNING_RATE_50FPS_PREV  4
#define SENSOR_ZOOM_FACTOR_50FPS_PREV         0


/* 480p, 60fps */
#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     480
#define XOFF_60FPS_PREV       3
#define YOFF_60FPS_PREV       1
#define LINE_TOT_60FPS_PREV   1132
#define FRAME_TOT_60FPS_PREV  738
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  4
#define SENSOR_ZOOM_FACTOR_60FPS_PREV         0


/* 480p, 30fps */
#define WIDTH_20FPS_PREV      640
#define HEIGHT_20FPS_PREV     480
#define XOFF_20FPS_PREV       3
#define YOFF_20FPS_PREV       1
#define LINE_TOT_20FPS_PREV   1132
#define FRAME_TOT_20FPS_PREV  738
#define PREV_TO_SNAP_BINNING_RATE_20FPS_PREV  4
#define SENSOR_ZOOM_FACTOR_20FPS_PREV         0


/* 5M */
#define WIDTH_SNAP      2592
#define HEIGHT_SNAP     1944
#define XOFF_SNAP       1
#define YOFF_SNAP       3
#define LINE_TOT_SNAP   3252
#define FRAME_TOT_SNAP  1968

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    11
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_50FPS_PREV    12
#define MCLK_PHASE_50FPS_PREV  0
#define PCLK_DIV_50FPS_PREV    12
#define PCLK_PHASE_50FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_20FPS_PREV    11
#define MCLK_PHASE_20FPS_PREV  0
#define PCLK_DIV_20FPS_PREV    11
#define PCLK_PHASE_20FPS_PREV  0

#define MCLK_DIV_SNAP          		11
#define MCLK_DIV_SNAP_BURST     	18
#define MCLK_DIV_SNAP_BURST_UP 		24
#define MCLK_PHASE_SNAP         	0
#define PCLK_DIV_SNAP           	11
#define PCLK_PHASE_SNAP         	0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV     65250000L
#define PCLK_FREQ_30FPS_PREV_TD  44850000L
#define PCLK_FREQ_60FPS_PREV     50420000L
#define PCLK_FREQ_60FPS_PREV_TD  34660000L
#define PCLK_FREQ_50FPS_PREV     87000000L
#define PCLK_FREQ_50FPS_PREV_TD  87000000L
#define PCLK_FREQ_20FPS_PREV     25200000L
#define PCLK_FREQ_20FPS_PREV_TD  25200000L
#define PCLK_FREQ_SNAP           65250000L
#define PCLK_FREQ_SNAP_BURST  	 41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP  31200000L	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_50FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_50FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_20FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_20FPS_PREV_TD  522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP           522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	 522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP  522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
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
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_GBBRGR
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_GBBRGR

/* i2c interface. */
#define I2C_DEV_ADDR  0x6C	//0x6D
#define I2C_REG_CLK_DIV      I2C_SOURCE_DIV_10
#define I2C_CLK_DIV          I2C_REG_CLK_DIV_128

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  181
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  183
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_50FPS_50_MAX_IDX  129
#define AE_50FPS_50_MIN_IDX  0
#define AE_50FPS_45_MAX_IDX  128
#define AE_50FPS_45_MIN_IDX  0
#define EV10_50FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  182
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  168
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

#define AE_20FPS_20_MAX_IDX  195
#define AE_20FPS_20_MIN_IDX  0
#define AE_20FPS_15_MAX_IDX  190
#define AE_20FPS_15_MIN_IDX  0
#define EV10_20FPS_EXP_IDX   100

/* gain. */
#define GAIN_30FPS_MAX_IDX  48
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_50FPS_MAX_IDX  48
#define GAIN_50FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  48
#define GAIN_60FPS_MIN_IDX  0

#define GAIN_20FPS_MAX_IDX  48
#define GAIN_20FPS_MIN_IDX  0

/* iso gain. */
#define ISO_100_GAIN_IDX	0
#define ISO_200_GAIN_IDX	16
#define ISO_400_GAIN_IDX	32
#define ISO_800_GAIN_IDX	48

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
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_ON					/* AE_GAIN_AFTER_EXP_OFF, */
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
static code UINT8 frontDevName[] = "SENSOR - OV OV5653 RAW INV";

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

/**************************************************************************
 *                            AE TABLE
 **************************************************************************/
/**************************************************************************
 *				banding calculate
 *			50Hz : 1/(shutter/10) < 1/(2*50)
 *			60Hz : 1/(shutter/10) < 1/(2*60)
 **************************************************************************/
/* ae table. */
static code frontAe_t ae20fps50hzTbl[191] = {
	/* --------30fps-------------- */
	{	0,	332,	47,	0,	0 },
	{	1,	332,	46,	0,	0 },
	{	2,	332,	45,	0,	0 },
	{	3,	332,	44,	0,	0 },
	{	4,	332,	42,	0,	0 },
	{	5,	332,	41,	0,	0 },
	{	6,	332,	40,	0,	0 },
	{	7,	332,	39,	0,	0 },
	{	8,	332,	38,	0,	0 },
	{	9,	332,	37,	0,	0 },
	{	10,	332,	36,	0,	0 },
	{	11,	332,	35,	0,	0 },
	{	12,	332,	34,	0,	0 },
	{	13,	332,	33,	0,	0 },
	{	14,	332,	31,	0,	0 },
	{	15,	332,	30,	0,	0 },
	{	16,	332,	29,	0,	0 },
	{	17,	332,	28,	0,	0 },
	{	18,	332,	26,	0,	0 },
	{	19,	332,	25,	0,	0 },
	{	20,	332,	24,	0,	0 },
	{	21,	332,	23,	0,	0 },
	{	22,	332,	22,	0,	0 },
	{	23,	332,	21,	0,	0 },
	{	24,	332,	20,	0,	0 },
	{	25,	332,	19,	0,	0 },
	{	26,	332,	18,	0,	0 },
	{	27,	332,	17,	0,	0 },
	{	28,	332,	15,	0,	0 },
	{	29,	332,	14,	0,	0 },
	{	30,	332,	13,	0,	0 },
	{	31,	332,	12,	0,	0 },
	{	32,	332,	10,	0,	0 },
	{	33,	332,	9,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	7,	0,	0 },
	{	36,	332,	6,	0,	0 },
	{	37,	332,	5,	0,	0 },
	{	38,	332,	4,	0,	0 },
	{	39,	332,	3,	0,	0 },
	{	40,	332,	2,	0,	0 },
	{	41,	332,	1,	0,	0 },
	{	42,	332,	0,	0,	0 },
	{	43,	498,	7,	0,	0 },
	{	44,	498,	6,	0,	0 },
	{	45,	498,	5,	0,	0 },
	{	46,	498,	4,	0,	0 },
	{	47,	498,	3,	0,	0 },
	{	48,	498,	2,	0,	0 },
	{	49,	498,	1,	0,	0 },
	{	50,	498,	0,	0,	0 },
	{	51,	992,	14,	0,	0 },
	{	52,	992,	13,	0,	0 },
	{	53,	992,	12,	0,	0 },
	{	54,	992,	11,	0,	0 },
	{	55,	992,	10,	0,	0 },
	{	56,	992,	8,	0,	0 },
	{	57,	992,	7,	0,	0 },
	{	58,	992,	6,	0,	0 },
	{	59,	992,	5,	0,	0 },
	{	60,	992,	4,	0,	0 },
	{	61,	992,	3,	0,	0 },
	{	62,	992,	2,	0,	0 },
	{	63,	992,	1,	0,	0 },
	{	64,	992,	0,	0,	0 },
	{	65,	1005,	0,	0,	0 },
	{	66,	1050,	0,	0,	0 },
	{	67,	1097,	0,	0,	0 },
	{	68,	1145,	0,	0,	0 },
	{	69,	1197,	0,	0,	0 },
	{	70,	1248,	0,	0,	0 },
	{	71,	1306,	0,	0,	0 },
	{	72,	1362,	0,	0,	0 },
	{	73,	1423,	0,	0,	0 },
	{	74,	1485,	0,	0,	0 },
	{	75,	1552,	0,	0,	0 },
	{	76,	1620,	0,	0,	0 },
	{	77,	1694,	0,	0,	0 },
	{	78,	1767,	0,	0,	0 },
	{	79,	1848,	0,	0,	0 },
	{	80,	1928,	0,	0,	0 },
	{	81,	2015,	0,	0,	0 },
	{	82,	2101,	0,	0,	0 },
	{	83,	2194,	0,	0,	0 },
	{	84,	2296,	0,	0,	0 },
	{	85,	2395,	0,	0,	0 },
	{	86,	2502,	0,	0,	0 },
	{	87,	2620,	0,	0,	0 },
	{	88,	2733,	0,	0,	0 },
	{	89,	2855,	0,	0,	0 },
	{	90,	2969,	0,	0,	0 },
	{	91,	3115,	0,	0,	0 },
	{	92,	3251,	0,	0,	0 },
	{	93,	3400,	0,	0,	0 },
	{	94,	3535,	0,	0,	0 },
	{	95,	3712,	0,	0,	0 },
	{	96,	3873,	0,	0,	0 },
	{	97,	4049,	0,	0,	0 },
	{	98,	4202,	0,	0,	0 },
	{	99,	4410,	0,	0,	0 },
	{	100,	4592,	0,	0,	0 },
	{	101,	4789,	0,	0,	0 },
	{	102,	5005,	0,	0,	0 },
	{	103,	5240,	0,	0,	0 },
	{	104,	5499,	0,	0,	0 },
	{	105,	5710,	0,	0,	0 },
	{	106,	5939,	0,	0,	0 },
	{	107,	6273,	0,	0,	0 },
	{	108,	6550,	0,	0,	0 },
	{	109,	6852,	0,	0,	0 },
	{	110,	7070,	0,	0,	0 },
	{	111,	7423,	0,	0,	0 },
	{	112,	7814,	0,	0,	0 },
	{	113,	8098,	0,	0,	0 },
	{	114,	8404,	0,	0,	0 },
	{	115,	8908,	0,	0,	0 },
	{	116,	9279,	0,	0,	0 },
	{	117,	9683,	0,	0,	0 },
	{	118,	10123,	0,	0,	0 },
	{	119,	10605,	0,	0,	0 },
	{	120,	11135,	0,	0,	0 },
	{	121,	11421,	0,	0,	0 },
	{	122,	12038,	0,	0,	0 },
	{	123,	12726,	0,	0,	0 },
	{	124,	13100,	0,	0,	0 },
	{	125,	13919,	0,	0,	0 },
	{	126,	14368,	0,	0,	0 },
	{	127,	14847,	0,	0,	0 },
	{	128,	15907,	0,	0,	0 },
	{	129,	16496,	0,	0,	0 },
	{	130,	17131,	0,	0,	0 },
	{	131,	17816,	0,	0,	0 },
	{	132,	18559,	0,	0,	0 },
	{	133,	19365,	0,	0,	0 },
	{	134,	20246,	0,	0,	0 },
	{	135,	21210,	0,	0,	0 },
	{	136,	22270,	0,	0,	0 },
	{	137,	23442,	0,	0,	0 },
	{	138,	24745,	0,	0,	0 },
	{	139,	26200,	0,	0,	0 },
	{	140,	27838,	0,	0,	0 },
	{	141,	29694,	0,	0,	0 },
	{	142,	31815,	0,	0,	0 },
	{	143,	34262,	1,	0,	0 },
	{	144,	34262,	0,	0,	0 },
	{	145,	37117,	0,	0,	0 },
	{	146,	40491,	0,	0,	1 },
	{	147,	44540,	1,	0,	0 },
	{	148,	44540,	0,	0,	0 },
	{	149,	49489,	1,	0,	0 },
	{	150,	49489,	0,	0,	0 },
	{	151,	55675,	1,	0,	1 },
	{	152,	55675,	0,	0,	0 },
	{	153,	63629,	1,	0,	1 },
	{	154,	63629,	0,	0,	1 },
	{	155,	74233,	2,	0,	1 },
	{	156,	74233,	1,	0,	1 },
	{	157,	74233,	0,	0,	1 },
	{	158,	89079,	2,	0,	0 },
	{	159,	89079,	1,	0,	0 },
	{	160,	89079,	0,	0,	0 },
	{	161,	111349,	3,	0,	1 },
	{	162,	111349,	2,	0,	1 },
	{	163,	111349,	1,	0,	1 },
	{	164,	111349,	0,	0,	0 },
	{	165,	148464,	4,	0,	2 },
	{	166,	148464,	3,	0,	2 },
	{	167,	148464,	2,	0,	1 },
	{	168,	148464,	1,	0,	1 },
	{	169,	148464,	0,	0,	1 },
	{	170,	222692,	6,	0,	4 },
	{	171,	222692,	5,	0,	4 },
	{	172,	222692,	4,	0,	4 },
	{	173,	222692,	3,	0,	4 },
	{	174,	222692,	2,	0,	4 },
	{	175,	222692,	1,	0,	3 },
	{	176,	222692,	0,	0,	3 },
	{	177,	445362,	14,	0,	24 },
	{	178,	445362,	13,	0,	23 },
	{	179,	445362,	12,	0,	22 },
	{	180,	445362,	11,	0,	22 },
	{	181,	445362,	9,	0,	20 },
	{	182,	445362,	8,	0,	19 },
	{	183,	445362,	7,	0,	18 },
	{	184,	445362,	6,	0,	18 },
	{	185,	445362,	5,	0,	17 },
	{	186,	445362,	4,	0,	16 },
	{	187,	445362,	3,	0,	15 },
	{	188,	445362,	2,	0,	14 },
	{	189,	445362,	1,	0,	14 },
	{	190,	445362,	0,	0,	13 },
};

static code frontAe_t ae20fps60hzTbl[196] = {
	/* --------30fps-------------- */
	{	0,	299,	48,	0,	0 },
	{	1,	299,	47,	0,	0 },
	{	2,	299,	45,	0,	0 },
	{	3,	299,	44,	0,	0 },
	{	4,	299,	43,	0,	0 },
	{	5,	299,	42,	0,	0 },
	{	6,	299,	41,	0,	0 },
	{	7,	299,	40,	0,	0 },
	{	8,	299,	39,	0,	0 },
	{	9,	299,	38,	0,	0 },
	{	10,	299,	37,	0,	0 },
	{	11,	299,	36,	0,	0 },
	{	12,	299,	35,	0,	0 },
	{	13,	299,	34,	0,	0 },
	{	14,	299,	33,	0,	0 },
	{	15,	299,	32,	0,	0 },
	{	16,	299,	31,	0,	0 },
	{	17,	299,	29,	0,	0 },
	{	18,	299,	28,	0,	0 },
	{	19,	299,	27,	0,	0 },
	{	20,	299,	26,	0,	0 },
	{	21,	299,	25,	0,	0 },
	{	22,	299,	24,	0,	0 },
	{	23,	299,	23,	0,	0 },
	{	24,	299,	22,	0,	0 },
	{	25,	299,	21,	0,	0 },
	{	26,	299,	20,	0,	0 },
	{	27,	299,	19,	0,	0 },
	{	28,	299,	18,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	16,	0,	0 },
	{	31,	299,	15,	0,	0 },
	{	32,	299,	13,	0,	0 },
	{	33,	299,	12,	0,	0 },
	{	34,	299,	11,	0,	0 },
	{	35,	299,	10,	0,	0 },
	{	36,	299,	9,	0,	0 },
	{	37,	299,	8,	0,	0 },
	{	38,	299,	7,	0,	0 },
	{	39,	299,	6,	0,	0 },
	{	40,	299,	5,	0,	0 },
	{	41,	299,	4,	0,	0 },
	{	42,	299,	3,	0,	0 },
	{	43,	299,	2,	0,	0 },
	{	44,	299,	1,	0,	0 },
	{	45,	299,	0,	0,	0 },
	{	46,	399,	4,	0,	0 },
	{	47,	399,	3,	0,	0 },
	{	48,	399,	2,	0,	0 },
	{	49,	399,	1,	0,	0 },
	{	50,	399,	0,	0,	0 },
	{	51,	597,	7,	0,	0 },
	{	52,	597,	6,	0,	0 },
	{	53,	597,	5,	0,	0 },
	{	54,	597,	4,	0,	0 },
	{	55,	597,	3,	0,	0 },
	{	56,	597,	2,	0,	0 },
	{	57,	597,	1,	0,	0 },
	{	58,	597,	0,	0,	0 },
	{	59,	598,	0,	0,	0 },
	{	60,	1191,	13,	0,	0 },
	{	61,	1191,	12,	0,	0 },
	{	62,	1191,	11,	0,	0 },
	{	63,	1191,	10,	0,	0 },
	{	64,	1191,	9,	0,	0 },
	{	65,	1191,	8,	0,	0 },
	{	66,	1191,	7,	0,	0 },
	{	67,	1191,	6,	0,	0 },
	{	68,	1191,	5,	0,	0 },
	{	69,	1191,	4,	0,	0 },
	{	70,	1191,	3,	0,	0 },
	{	71,	1191,	2,	0,	0 },
	{	72,	1191,	1,	0,	0 },
	{	73,	1191,	0,	0,	0 },
	{	74,	1197,	0,	0,	0 },
	{	75,	1248,	0,	0,	0 },
	{	76,	1306,	0,	0,	0 },
	{	77,	1362,	0,	0,	0 },
	{	78,	1423,	0,	0,	0 },
	{	79,	1485,	0,	0,	0 },
	{	80,	1552,	0,	0,	0 },
	{	81,	1620,	0,	0,	0 },
	{	82,	1694,	0,	0,	0 },
	{	83,	1767,	0,	0,	0 },
	{	84,	1848,	0,	0,	0 },
	{	85,	1928,	0,	0,	0 },
	{	86,	2015,	0,	0,	0 },
	{	87,	2101,	0,	0,	0 },
	{	88,	2194,	0,	0,	0 },
	{	89,	2296,	0,	0,	0 },
	{	90,	2395,	0,	0,	0 },
	{	91,	2502,	0,	0,	0 },
	{	92,	2620,	0,	0,	0 },
	{	93,	2733,	0,	0,	0 },
	{	94,	2855,	0,	0,	0 },
	{	95,	2969,	0,	0,	0 },
	{	96,	3115,	0,	0,	0 },
	{	97,	3251,	0,	0,	0 },
	{	98,	3400,	0,	0,	0 },
	{	99,	3535,	0,	0,	0 },
	{	100,	3712,	0,	0,	0 },
	{	101,	3873,	0,	0,	0 },
	{	102,	4049,	0,	0,	0 },
	{	103,	4202,	0,	0,	0 },
	{	104,	4410,	0,	0,	0 },
	{	105,	4592,	0,	0,	0 },
	{	106,	4789,	0,	0,	0 },
	{	107,	5005,	0,	0,	0 },
	{	108,	5240,	0,	0,	0 },
	{	109,	5499,	0,	0,	0 },
	{	110,	5710,	0,	0,	0 },
	{	111,	5939,	0,	0,	0 },
	{	112,	6273,	0,	0,	0 },
	{	113,	6550,	0,	0,	0 },
	{	114,	6852,	0,	0,	0 },
	{	115,	7070,	0,	0,	0 },
	{	116,	7423,	0,	0,	0 },
	{	117,	7814,	0,	0,	0 },
	{	118,	8098,	0,	0,	0 },
	{	119,	8404,	0,	0,	0 },
	{	120,	8908,	0,	0,	0 },
	{	121,	9279,	0,	0,	0 },
	{	122,	9683,	0,	0,	0 },
	{	123,	10123,	0,	0,	0 },
	{	124,	10605,	0,	0,	0 },
	{	125,	11135,	0,	0,	0 },
	{	126,	11421,	0,	0,	0 },
	{	127,	12038,	0,	0,	0 },
	{	128,	12726,	0,	0,	0 },
	{	129,	13100,	0,	0,	0 },
	{	130,	13919,	0,	0,	0 },
	{	131,	14368,	0,	0,	0 },
	{	132,	14847,	0,	0,	0 },
	{	133,	15907,	0,	0,	0 },
	{	134,	16496,	0,	0,	0 },
	{	135,	17131,	0,	0,	0 },
	{	136,	17816,	0,	0,	0 },
	{	137,	18559,	0,	0,	0 },
	{	138,	19365,	0,	0,	0 },
	{	139,	20246,	0,	0,	0 },
	{	140,	21210,	0,	0,	0 },
	{	141,	22270,	0,	0,	0 },
	{	142,	23442,	0,	0,	0 },
	{	143,	24745,	0,	0,	0 },
	{	144,	26200,	0,	0,	0 },
	{	145,	27838,	0,	0,	0 },
	{	146,	29694,	0,	0,	0 },
	{	147,	31815,	0,	0,	0 },
	{	148,	34262,	1,	0,	0 },
	{	149,	34262,	0,	0,	0 },
	{	150,	37117,	0,	0,	0 },
	{	151,	40491,	0,	0,	1 },
	{	152,	44540,	1,	0,	0 },
	{	153,	44540,	0,	0,	0 },
	{	154,	49489,	1,	0,	0 },
	{	155,	49489,	0,	0,	0 },
	{	156,	55675,	1,	0,	1 },
	{	157,	55675,	0,	0,	0 },
	{	158,	63629,	1,	0,	1 },
	{	159,	63629,	0,	0,	1 },
	{	160,	74233,	2,	0,	1 },
	{	161,	74233,	1,	0,	1 },
	{	162,	74233,	0,	0,	1 },
	{	163,	89079,	2,	0,	0 },
	{	164,	89079,	1,	0,	0 },
	{	165,	89079,	0,	0,	0 },
	{	166,	111349,	3,	0,	1 },
	{	167,	111349,	2,	0,	1 },
	{	168,	111349,	1,	0,	1 },
	{	169,	111349,	0,	0,	0 },
	{	170,	148464,	4,	0,	2 },
	{	171,	148464,	3,	0,	2 },
	{	172,	148464,	2,	0,	1 },
	{	173,	148464,	1,	0,	1 },
	{	174,	148464,	0,	0,	1 },
	{	175,	222692,	6,	0,	4 },
	{	176,	222692,	5,	0,	4 },
	{	177,	222692,	4,	0,	4 },
	{	178,	222692,	3,	0,	4 },
	{	179,	222692,	2,	0,	4 },
	{	180,	222692,	1,	0,	3 },
	{	181,	222692,	0,	0,	3 },
	{	182,	445362,	14,	0,	24 },
	{	183,	445362,	13,	0,	23 },
	{	184,	445362,	12,	0,	22 },
	{	185,	445362,	11,	0,	22 },
	{	186,	445362,	9,	0,	20 },
	{	187,	445362,	8,	0,	19 },
	{	188,	445362,	7,	0,	18 },
	{	189,	445362,	6,	0,	18 },
	{	190,	445362,	5,	0,	17 },
	{	191,	445362,	4,	0,	16 },
	{	192,	445362,	3,	0,	15 },
	{	193,	445362,	2,	0,	14 },
	{	194,	445362,	1,	0,	14 },
	{	195,	445362,	0,	0,	13 },
};

static code frontAe_t ae30fps50hzTbl[184] = {
	/* --------30fps-------------- */
	{	0,	332,	48,	0,	0 },
	{	1,	332,	47,	0,	0 },
	{	2,	332,	46,	0,	0 },
	{	3,	332,	44,	0,	0 },
	{	4,	332,	43,	0,	0 },
	{	5,	332,	42,	0,	0 },
	{	6,	332,	41,	0,	0 },
	{	7,	332,	40,	0,	0 },
	{	8,	332,	39,	0,	0 },
	{	9,	332,	38,	0,	0 },
	{	10,	332,	37,	0,	0 },
	{	11,	332,	36,	0,	0 },
	{	12,	332,	35,	0,	0 },
	{	13,	332,	34,	0,	0 },
	{	14,	332,	33,	0,	0 },
	{	15,	332,	32,	0,	0 },
	{	16,	332,	31,	0,	0 },
	{	17,	332,	30,	0,	0 },
	{	18,	332,	28,	0,	0 },
	{	19,	332,	27,	0,	0 },
	{	20,	332,	26,	0,	0 },
	{	21,	332,	25,	0,	0 },
	{	22,	332,	24,	0,	0 },
	{	23,	332,	23,	0,	0 },
	{	24,	332,	22,	0,	0 },
	{	25,	332,	21,	0,	0 },
	{	26,	332,	20,	0,	0 },
	{	27,	332,	19,	0,	0 },
	{	28,	332,	18,	0,	0 },
	{	29,	332,	17,	0,	0 },
	{	30,	332,	16,	0,	0 },
	{	31,	332,	15,	0,	0 },
	{	32,	332,	14,	0,	0 },
	{	33,	332,	12,	0,	0 },
	{	34,	332,	11,	0,	0 },
	{	35,	332,	10,	0,	0 },
	{	36,	332,	9,	0,	0 },
	{	37,	332,	8,	0,	0 },
	{	38,	332,	7,	0,	0 },
	{	39,	332,	6,	0,	0 },
	{	40,	332,	5,	0,	0 },
	{	41,	332,	4,	0,	0 },
	{	42,	332,	3,	0,	0 },
	{	43,	332,	2,	0,	0 },
	{	44,	332,	1,	0,	0 },
	{	45,	332,	0,	0,	0 },
	{	46,	498,	6,	0,	0 },
	{	47,	498,	5,	0,	0 },
	{	48,	498,	4,	0,	0 },
	{	49,	498,	3,	0,	0 },
	{	50,	498,	2,	0,	0 },
	{	51,	498,	1,	0,	0 },
	{	52,	498,	0,	0,	0 },
	{	53,	990,	14,	0,	0 },
	{	54,	990,	13,	0,	0 },
	{	55,	990,	11,	0,	0 },
	{	56,	990,	10,	0,	0 },
	{	57,	990,	9,	0,	0 },
	{	58,	990,	8,	0,	0 },
	{	59,	990,	7,	0,	0 },
	{	60,	990,	6,	0,	0 },
	{	61,	990,	5,	0,	0 },
	{	62,	990,	4,	0,	0 },
	{	63,	990,	3,	0,	0 },
	{	64,	990,	2,	0,	0 },
	{	65,	990,	1,	0,	0 },
	{	66,	990,	0,	0,	0 },
	{	67,	1020,	0,	0,	0 },
	{	68,	1066,	0,	0,	0 },
	{	69,	1113,	0,	0,	0 },
	{	70,	1164,	0,	0,	0 },
	{	71,	1214,	0,	0,	0 },
	{	72,	1270,	0,	0,	0 },
	{	73,	1325,	0,	0,	0 },
	{	74,	1385,	0,	0,	0 },
	{	75,	1444,	0,	0,	0 },
	{	76,	1508,	0,	0,	0 },
	{	77,	1578,	0,	0,	0 },
	{	78,	1645,	0,	0,	0 },
	{	79,	1719,	0,	0,	0 },
	{	80,	1800,	0,	0,	0 },
	{	81,	1878,	0,	0,	0 },
	{	82,	1962,	0,	0,	0 },
	{	83,	2040,	0,	0,	0 },
	{	84,	2140,	0,	0,	0 },
	{	85,	2234,	0,	0,	0 },
	{	86,	2336,	0,	0,	0 },
	{	87,	2429,	0,	0,	0 },
	{	88,	2550,	0,	0,	0 },
	{	89,	2661,	0,	0,	0 },
	{	90,	2782,	0,	0,	0 },
	{	91,	2887,	0,	0,	0 },
	{	92,	3030,	0,	0,	0 },
	{	93,	3155,	0,	0,	0 },
	{	94,	3291,	0,	0,	0 },
	{	95,	3439,	0,	0,	0 },
	{	96,	3601,	0,	0,	0 },
	{	97,	3778,	0,	0,	0 },
	{	98,	3924,	0,	0,	0 },
	{	99,	4081,	0,	0,	0 },
	{	100,	4311,	0,	0,	0 },
	{	101,	4501,	0,	0,	0 },
	{	102,	4708,	0,	0,	0 },
	{	103,	4858,	0,	0,	0 },
	{	104,	5101,	0,	0,	0 },
	{	105,	5369,	0,	0,	0 },
	{	106,	5565,	0,	0,	0 },
	{	107,	5775,	0,	0,	0 },
	{	108,	6121,	0,	0,	0 },
	{	109,	6376,	0,	0,	0 },
	{	110,	6653,	0,	0,	0 },
	{	111,	6956,	0,	0,	0 },
	{	112,	7287,	0,	0,	0 },
	{	113,	7651,	0,	0,	0 },
	{	114,	7847,	0,	0,	0 },
	{	115,	8272,	0,	0,	0 },
	{	116,	8744,	0,	0,	0 },
	{	117,	9001,	0,	0,	0 },
	{	118,	9564,	0,	0,	0 },
	{	119,	9873,	0,	0,	0 },
	{	120,	10202,	0,	0,	0 },
	{	121,	10930,	0,	0,	0 },
	{	122,	11335,	0,	0,	0 },
	{	123,	11771,	0,	0,	0 },
	{	124,	12242,	0,	0,	0 },
	{	125,	12752,	0,	0,	0 },
	{	126,	13306,	0,	0,	0 },
	{	127,	13911,	0,	0,	0 },
	{	128,	14574,	0,	0,	0 },
	{	129,	15302,	0,	0,	0 },
	{	130,	16108,	0,	0,	0 },
	{	131,	17003,	0,	0,	0 },
	{	132,	18003,	0,	0,	0 },
	{	133,	19128,	0,	0,	0 },
	{	134,	20403,	0,	0,	0 },
	{	135,	21861,	0,	0,	0 },
	{	136,	23542,	1,	0,	0 },
	{	137,	23542,	0,	0,	0 },
	{	138,	25504,	0,	0,	0 },
	{	139,	27823,	0,	0,	0 },
	{	140,	30605,	1,	0,	0 },
	{	141,	30605,	0,	0,	0 },
	{	142,	34005,	1,	0,	0 },
	{	143,	34005,	0,	0,	0 },
	{	144,	38256,	1,	0,	1 },
	{	145,	38256,	0,	0,	1 },
	{	146,	43721,	1,	0,	0 },
	{	147,	43721,	0,	0,	0 },
	{	148,	51008,	2,	0,	1 },
	{	149,	51008,	1,	0,	1 },
	{	150,	51008,	0,	0,	1 },
	{	151,	61209,	2,	0,	0 },
	{	152,	61209,	1,	0,	0 },
	{	153,	61209,	0,	0,	0 },
	{	154,	76511,	3,	0,	1 },
	{	155,	76511,	2,	0,	1 },
	{	156,	76511,	1,	0,	1 },
	{	157,	76511,	0,	0,	1 },
	{	158,	102013,	4,	0,	2 },
	{	159,	102013,	3,	0,	2 },
	{	160,	102013,	2,	0,	2 },
	{	161,	102013,	1,	0,	2 },
	{	162,	102013,	0,	0,	2 },
	{	163,	153018,	6,	0,	1 },
	{	164,	153018,	5,	0,	1 },
	{	165,	153018,	4,	0,	1 },
	{	166,	153018,	3,	0,	1 },
	{	167,	153018,	2,	0,	1 },
	{	168,	153018,	1,	0,	1 },
	{	169,	153018,	0,	0,	1 },
	{	170,	306020,	14,	0,	9 },
	{	171,	306020,	13,	0,	9 },
	{	172,	306020,	12,	0,	9 },
	{	173,	306020,	11,	0,	8 },
	{	174,	306020,	9,	0,	8 },
	{	175,	306020,	8,	0,	7 },
	{	176,	306020,	7,	0,	7 },
	{	177,	306020,	6,	0,	7 },
	{	178,	306020,	5,	0,	7 },
	{	179,	306020,	4,	0,	6 },
	{	180,	306020,	3,	0,	6 },
	{	181,	306020,	2,	0,	6 },
	{	182,	306020,	1,	0,	5 },
	{	183,	306020,	0,	0,	5 },
};

static code frontAe_t ae30fps60hzTbl[182] = {
	/* --------30fps-------------- */
	{	0,	299,	48,	0,	0 },
	{	1,	299,	46,	0,	0 },
	{	2,	299,	45,	0,	0 },
	{	3,	299,	44,	0,	0 },
	{	4,	299,	43,	0,	0 },
	{	5,	299,	42,	0,	0 },
	{	6,	299,	40,	0,	0 },
	{	7,	299,	39,	0,	0 },
	{	8,	299,	38,	0,	0 },
	{	9,	299,	37,	0,	0 },
	{	10,	299,	36,	0,	0 },
	{	11,	299,	35,	0,	0 },
	{	12,	299,	34,	0,	0 },
	{	13,	299,	33,	0,	0 },
	{	14,	299,	32,	0,	0 },
	{	15,	299,	30,	0,	0 },
	{	16,	299,	29,	0,	0 },
	{	17,	299,	28,	0,	0 },
	{	18,	299,	27,	0,	0 },
	{	19,	299,	26,	0,	0 },
	{	20,	299,	24,	0,	0 },
	{	21,	299,	23,	0,	0 },
	{	22,	299,	22,	0,	0 },
	{	23,	299,	21,	0,	0 },
	{	24,	299,	20,	0,	0 },
	{	25,	299,	19,	0,	0 },
	{	26,	299,	18,	0,	0 },
	{	27,	299,	17,	0,	0 },
	{	28,	299,	16,	0,	0 },
	{	29,	299,	14,	0,	0 },
	{	30,	299,	13,	0,	0 },
	{	31,	299,	12,	0,	0 },
	{	32,	299,	11,	0,	0 },
	{	33,	299,	10,	0,	0 },
	{	34,	299,	8,	0,	0 },
	{	35,	299,	7,	0,	0 },
	{	36,	299,	6,	0,	0 },
	{	37,	299,	5,	0,	0 },
	{	38,	299,	4,	0,	0 },
	{	39,	299,	3,	0,	0 },
	{	40,	299,	2,	0,	0 },
	{	41,	299,	1,	0,	0 },
	{	42,	299,	0,	0,	0 },
	{	43,	399,	4,	0,	0 },
	{	44,	399,	3,	0,	0 },
	{	45,	399,	2,	0,	0 },
	{	46,	399,	1,	0,	0 },
	{	47,	399,	0,	0,	0 },
	{	48,	598,	6,	0,	0 },
	{	49,	598,	5,	0,	0 },
	{	50,	598,	4,	0,	0 },
	{	51,	598,	3,	0,	0 },
	{	52,	598,	2,	0,	0 },
	{	53,	598,	1,	0,	0 },
	{	54,	598,	0,	0,	0 },
	{	55,	1191,	14,	0,	0 },
	{	56,	1191,	13,	0,	0 },
	{	57,	1191,	12,	0,	0 },
	{	58,	1191,	10,	0,	0 },
	{	59,	1191,	9,	0,	0 },
	{	60,	1191,	8,	0,	0 },
	{	61,	1191,	7,	0,	0 },
	{	62,	1191,	6,	0,	0 },
	{	63,	1191,	5,	0,	0 },
	{	64,	1191,	4,	0,	0 },
	{	65,	1191,	3,	0,	0 },
	{	66,	1191,	2,	0,	0 },
	{	67,	1191,	1,	0,	0 },
	{	68,	1191,	0,	0,	0 },
	{	69,	1214,	0,	0,	0 },
	{	70,	1270,	0,	0,	0 },
	{	71,	1325,	0,	0,	0 },
	{	72,	1385,	0,	0,	0 },
	{	73,	1444,	0,	0,	0 },
	{	74,	1508,	0,	0,	0 },
	{	75,	1578,	0,	0,	0 },
	{	76,	1645,	0,	0,	0 },
	{	77,	1719,	0,	0,	0 },
	{	78,	1800,	0,	0,	0 },
	{	79,	1878,	0,	0,	0 },
	{	80,	1962,	0,	0,	0 },
	{	81,	2040,	0,	0,	0 },
	{	82,	2140,	0,	0,	0 },
	{	83,	2234,	0,	0,	0 },
	{	84,	2336,	0,	0,	0 },
	{	85,	2429,	0,	0,	0 },
	{	86,	2550,	0,	0,	0 },
	{	87,	2661,	0,	0,	0 },
	{	88,	2782,	0,	0,	0 },
	{	89,	2887,	0,	0,	0 },
	{	90,	3030,	0,	0,	0 },
	{	91,	3155,	0,	0,	0 },
	{	92,	3291,	0,	0,	0 },
	{	93,	3439,	0,	0,	0 },
	{	94,	3601,	0,	0,	0 },
	{	95,	3778,	0,	0,	0 },
	{	96,	3924,	0,	0,	0 },
	{	97,	4081,	0,	0,	0 },
	{	98,	4311,	0,	0,	0 },
	{	99,	4501,	0,	0,	0 },
	{	100,	4708,	0,	0,	0 },
	{	101,	4858,	0,	0,	0 },
	{	102,	5101,	0,	0,	0 },
	{	103,	5369,	0,	0,	0 },
	{	104,	5565,	0,	0,	0 },
	{	105,	5775,	0,	0,	0 },
	{	106,	6121,	0,	0,	0 },
	{	107,	6376,	0,	0,	0 },
	{	108,	6653,	0,	0,	0 },
	{	109,	6956,	0,	0,	0 },
	{	110,	7287,	0,	0,	0 },
	{	111,	7651,	0,	0,	0 },
	{	112,	7847,	0,	0,	0 },
	{	113,	8272,	0,	0,	0 },
	{	114,	8744,	0,	0,	0 },
	{	115,	9001,	0,	0,	0 },
	{	116,	9564,	0,	0,	0 },
	{	117,	9873,	0,	0,	0 },
	{	118,	10202,	0,	0,	0 },
	{	119,	10930,	0,	0,	0 },
	{	120,	11335,	0,	0,	0 },
	{	121,	11771,	0,	0,	0 },
	{	122,	12242,	0,	0,	0 },
	{	123,	12752,	0,	0,	0 },
	{	124,	13306,	0,	0,	0 },
	{	125,	13911,	0,	0,	0 },
	{	126,	14574,	0,	0,	0 },
	{	127,	15302,	0,	0,	0 },
	{	128,	16108,	0,	0,	0 },
	{	129,	17003,	0,	0,	0 },
	{	130,	18003,	0,	0,	0 },
	{	131,	19128,	0,	0,	0 },
	{	132,	20403,	0,	0,	0 },
	{	133,	21861,	0,	0,	0 },
	{	134,	23542,	1,	0,	0 },
	{	135,	23542,	0,	0,	0 },
	{	136,	25504,	0,	0,	0 },
	{	137,	27823,	0,	0,	0 },
	{	138,	30605,	1,	0,	0 },
	{	139,	30605,	0,	0,	0 },
	{	140,	34005,	1,	0,	0 },
	{	141,	34005,	0,	0,	0 },
	{	142,	38256,	1,	0,	1 },
	{	143,	38256,	0,	0,	1 },
	{	144,	43721,	1,	0,	0 },
	{	145,	43721,	0,	0,	0 },
	{	146,	51008,	2,	0,	1 },
	{	147,	51008,	1,	0,	1 },
	{	148,	51008,	0,	0,	1 },
	{	149,	61209,	2,	0,	0 },
	{	150,	61209,	1,	0,	0 },
	{	151,	61209,	0,	0,	0 },
	{	152,	76511,	3,	0,	1 },
	{	153,	76511,	2,	0,	1 },
	{	154,	76511,	1,	0,	1 },
	{	155,	76511,	0,	0,	1 },
	{	156,	102013,	4,	0,	2 },
	{	157,	102013,	3,	0,	2 },
	{	158,	102013,	2,	0,	2 },
	{	159,	102013,	1,	0,	2 },
	{	160,	102013,	0,	0,	2 },
	{	161,	153018,	6,	0,	1 },
	{	162,	153018,	5,	0,	1 },
	{	163,	153018,	4,	0,	1 },
	{	164,	153018,	3,	0,	1 },
	{	165,	153018,	2,	0,	1 },
	{	166,	153018,	1,	0,	1 },
	{	167,	153018,	0,	0,	1 },
	{	168,	306020,	14,	0,	9 },
	{	169,	306020,	13,	0,	9 },
	{	170,	306020,	12,	0,	9 },
	{	171,	306020,	11,	0,	8 },
	{	172,	306020,	9,	0,	8 },
	{	173,	306020,	8,	0,	7 },
	{	174,	306020,	7,	0,	7 },
	{	175,	306020,	6,	0,	7 },
	{	176,	306020,	5,	0,	7 },
	{	177,	306020,	4,	0,	6 },
	{	178,	306020,	3,	0,	6 },
	{	179,	306020,	2,	0,	6 },
	{	180,	306020,	1,	0,	5 },
	{	181,	306020,	0,	0,	5 },
};

static code frontAe_t ae60fps50hzTbl[169] = {
	/* --------60fps-------------- */
	{	0,	992,	48,	0,	0 },
	{	1,	992,	46,	0,	0 },
	{	2,	992,	45,	0,	0 },
	{	3,	992,	44,	0,	0 },
	{	4,	992,	43,	0,	0 },
	{	5,	992,	42,	0,	0 },
	{	6,	992,	40,	0,	0 },
	{	7,	992,	39,	0,	0 },
	{	8,	992,	38,	0,	0 },
	{	9,	992,	37,	0,	0 },
	{	10,	992,	36,	0,	0 },
	{	11,	992,	35,	0,	0 },
	{	12,	992,	34,	0,	0 },
	{	13,	992,	33,	0,	0 },
	{	14,	992,	32,	0,	0 },
	{	15,	992,	30,	0,	0 },
	{	16,	992,	29,	0,	0 },
	{	17,	992,	28,	0,	0 },
	{	18,	992,	27,	0,	0 },
	{	19,	992,	26,	0,	0 },
	{	20,	992,	24,	0,	0 },
	{	21,	992,	23,	0,	0 },
	{	22,	992,	22,	0,	0 },
	{	23,	992,	21,	0,	0 },
	{	24,	992,	20,	0,	0 },
	{	25,	992,	19,	0,	0 },
	{	26,	992,	18,	0,	0 },
	{	27,	992,	17,	0,	0 },
	{	28,	992,	16,	0,	0 },
	{	29,	992,	14,	0,	0 },
	{	30,	992,	13,	0,	0 },
	{	31,	992,	12,	0,	0 },
	{	32,	992,	11,	0,	0 },
	{	33,	992,	10,	0,	0 },
	{	34,	992,	8,	0,	0 },
	{	35,	992,	7,	0,	0 },
	{	36,	992,	6,	0,	0 },
	{	37,	992,	5,	0,	0 },
	{	38,	992,	4,	0,	0 },
	{	39,	992,	3,	0,	0 },
	{	40,	992,	2,	0,	0 },
	{	41,	992,	1,	0,	0 },
	{	42,	992,	0,	0,	0 },
	{	43,	1005,	0,	0,	0 },
	{	44,	1050,	0,	0,	0 },
	{	45,	1097,	0,	0,	0 },
	{	46,	1145,	0,	0,	0 },
	{	47,	1197,	0,	0,	0 },
	{	48,	1248,	0,	0,	0 },
	{	49,	1306,	0,	0,	0 },
	{	50,	1362,	0,	0,	0 },
	{	51,	1423,	0,	0,	0 },
	{	52,	1485,	0,	0,	0 },
	{	53,	1552,	0,	0,	0 },
	{	54,	1620,	0,	0,	0 },
	{	55,	1694,	0,	0,	0 },
	{	56,	1767,	0,	0,	0 },
	{	57,	1848,	0,	0,	0 },
	{	58,	1928,	0,	0,	0 },
	{	59,	2015,	0,	0,	0 },
	{	60,	2101,	0,	0,	0 },
	{	61,	2194,	0,	0,	0 },
	{	62,	2296,	0,	0,	0 },
	{	63,	2395,	0,	0,	0 },
	{	64,	2502,	0,	0,	0 },
	{	65,	2620,	0,	0,	0 },
	{	66,	2733,	0,	0,	0 },
	{	67,	2855,	0,	0,	0 },
	{	68,	2969,	0,	0,	0 },
	{	69,	3115,	0,	0,	0 },
	{	70,	3251,	0,	0,	0 },
	{	71,	3400,	0,	0,	0 },
	{	72,	3535,	0,	0,	0 },
	{	73,	3712,	0,	0,	0 },
	{	74,	3873,	0,	0,	0 },
	{	75,	4049,	0,	0,	0 },
	{	76,	4202,	0,	0,	0 },
	{	77,	4410,	0,	0,	0 },
	{	78,	4592,	0,	0,	0 },
	{	79,	4789,	0,	0,	0 },
	{	80,	5005,	0,	0,	0 },
	{	81,	5240,	0,	0,	0 },
	{	82,	5499,	0,	0,	0 },
	{	83,	5710,	0,	0,	0 },
	{	84,	5939,	0,	0,	0 },
	{	85,	6273,	0,	0,	0 },
	{	86,	6550,	0,	0,	0 },
	{	87,	6852,	0,	0,	0 },
	{	88,	7070,	0,	0,	0 },
	{	89,	7423,	0,	0,	0 },
	{	90,	7814,	0,	0,	0 },
	{	91,	8098,	0,	0,	0 },
	{	92,	8404,	0,	0,	0 },
	{	93,	8908,	0,	0,	0 },
	{	94,	9279,	0,	0,	0 },
	{	95,	9683,	0,	0,	0 },
	{	96,	10123,	0,	0,	0 },
	{	97,	10605,	0,	0,	0 },
	{	98,	11135,	0,	0,	0 },
	{	99,	11421,	0,	0,	0 },
	{	100,	12038,	0,	0,	0 },
	{	101,	12726,	0,	0,	0 },
	{	102,	13100,	0,	0,	0 },
	{	103,	13919,	0,	0,	0 },
	{	104,	14368,	0,	0,	0 },
	{	105,	14847,	0,	0,	0 },
	{	106,	15907,	0,	0,	0 },
	{	107,	16496,	0,	0,	0 },
	{	108,	17131,	0,	0,	0 },
	{	109,	17816,	0,	0,	0 },
	{	110,	18559,	0,	0,	0 },
	{	111,	19365,	0,	0,	0 },
	{	112,	20246,	0,	0,	0 },
	{	113,	21210,	0,	0,	0 },
	{	114,	22270,	0,	0,	0 },
	{	115,	23442,	0,	0,	0 },
	{	116,	24745,	0,	0,	0 },
	{	117,	26200,	0,	0,	0 },
	{	118,	27838,	0,	0,	0 },
	{	119,	29694,	0,	0,	0 },
	{	120,	31815,	0,	0,	0 },
	{	121,	34262,	1,	0,	0 },
	{	122,	34262,	0,	0,	0 },
	{	123,	37117,	0,	0,	0 },
	{	124,	40491,	0,	0,	1 },
	{	125,	44540,	1,	0,	0 },
	{	126,	44540,	0,	0,	0 },
	{	127,	49489,	1,	0,	0 },
	{	128,	49489,	0,	0,	0 },
	{	129,	55675,	1,	0,	1 },
	{	130,	55675,	0,	0,	0 },
	{	131,	63629,	1,	0,	1 },
	{	132,	63629,	0,	0,	1 },
	{	133,	74233,	2,	0,	1 },
	{	134,	74233,	1,	0,	1 },
	{	135,	74233,	0,	0,	1 },
	{	136,	89079,	2,	0,	0 },
	{	137,	89079,	1,	0,	0 },
	{	138,	89079,	0,	0,	0 },
	{	139,	111349,	3,	0,	1 },
	{	140,	111349,	2,	0,	1 },
	{	141,	111349,	1,	0,	1 },
	{	142,	111349,	0,	0,	0 },
	{	143,	148464,	4,	0,	2 },
	{	144,	148464,	3,	0,	2 },
	{	145,	148464,	2,	0,	1 },
	{	146,	148464,	1,	0,	1 },
	{	147,	148464,	0,	0,	1 },
	{	148,	222692,	6,	0,	4 },
	{	149,	222692,	5,	0,	4 },
	{	150,	222692,	4,	0,	4 },
	{	151,	222692,	3,	0,	4 },
	{	152,	222692,	2,	0,	4 },
	{	153,	222692,	1,	0,	3 },
	{	154,	222692,	0,	0,	3 },
	{	155,	445362,	14,	0,	24 },
	{	156,	445362,	13,	0,	23 },
	{	157,	445362,	12,	0,	22 },
	{	158,	445362,	11,	0,	22 },
	{	159,	445362,	9,	0,	20 },
	{	160,	445362,	8,	0,	19 },
	{	161,	445362,	7,	0,	18 },
	{	162,	445362,	6,	0,	18 },
	{	163,	445362,	5,	0,	17 },
	{	164,	445362,	4,	0,	16 },
	{	165,	445362,	3,	0,	15 },
	{	166,	445362,	2,	0,	14 },
	{	167,	445362,	1,	0,	14 },
	{	168,	445362,	0,	0,	13 },
};

static code frontAe_t ae60fps60hzTbl[183] = {
	/* --------60fps-------------- */
	{	0,	597,	48,	0,	0 },
	{	1,	597,	47,	0,	0 },
	{	2,	597,	45,	0,	0 },
	{	3,	597,	44,	0,	0 },
	{	4,	597,	43,	0,	0 },
	{	5,	597,	42,	0,	0 },
	{	6,	597,	41,	0,	0 },
	{	7,	597,	40,	0,	0 },
	{	8,	597,	39,	0,	0 },
	{	9,	597,	38,	0,	0 },
	{	10,	597,	37,	0,	0 },
	{	11,	597,	36,	0,	0 },
	{	12,	597,	35,	0,	0 },
	{	13,	597,	34,	0,	0 },
	{	14,	597,	33,	0,	0 },
	{	15,	597,	32,	0,	0 },
	{	16,	597,	31,	0,	0 },
	{	17,	597,	29,	0,	0 },
	{	18,	597,	28,	0,	0 },
	{	19,	597,	27,	0,	0 },
	{	20,	597,	26,	0,	0 },
	{	21,	597,	25,	0,	0 },
	{	22,	597,	24,	0,	0 },
	{	23,	597,	23,	0,	0 },
	{	24,	597,	22,	0,	0 },
	{	25,	597,	21,	0,	0 },
	{	26,	597,	20,	0,	0 },
	{	27,	597,	19,	0,	0 },
	{	28,	597,	18,	0,	0 },
	{	29,	597,	17,	0,	0 },
	{	30,	597,	16,	0,	0 },
	{	31,	597,	15,	0,	0 },
	{	32,	597,	13,	0,	0 },
	{	33,	597,	12,	0,	0 },
	{	34,	597,	11,	0,	0 },
	{	35,	597,	10,	0,	0 },
	{	36,	597,	9,	0,	0 },
	{	37,	597,	8,	0,	0 },
	{	38,	597,	7,	0,	0 },
	{	39,	597,	6,	0,	0 },
	{	40,	597,	5,	0,	0 },
	{	41,	597,	4,	0,	0 },
	{	42,	597,	3,	0,	0 },
	{	43,	597,	2,	0,	0 },
	{	44,	597,	1,	0,	0 },
	{	45,	597,	0,	0,	0 },
	{	46,	598,	0,	0,	0 },
	{	47,	1191,	13,	0,	0 },
	{	48,	1191,	12,	0,	0 },
	{	49,	1191,	11,	0,	0 },
	{	50,	1191,	10,	0,	0 },
	{	51,	1191,	9,	0,	0 },
	{	52,	1191,	8,	0,	0 },
	{	53,	1191,	7,	0,	0 },
	{	54,	1191,	6,	0,	0 },
	{	55,	1191,	5,	0,	0 },
	{	56,	1191,	4,	0,	0 },
	{	57,	1191,	3,	0,	0 },
	{	58,	1191,	2,	0,	0 },
	{	59,	1191,	1,	0,	0 },
	{	60,	1191,	0,	0,	0 },
	{	61,	1197,	0,	0,	0 },
	{	62,	1248,	0,	0,	0 },
	{	63,	1306,	0,	0,	0 },
	{	64,	1362,	0,	0,	0 },
	{	65,	1423,	0,	0,	0 },
	{	66,	1485,	0,	0,	0 },
	{	67,	1552,	0,	0,	0 },
	{	68,	1620,	0,	0,	0 },
	{	69,	1694,	0,	0,	0 },
	{	70,	1767,	0,	0,	0 },
	{	71,	1848,	0,	0,	0 },
	{	72,	1928,	0,	0,	0 },
	{	73,	2015,	0,	0,	0 },
	{	74,	2101,	0,	0,	0 },
	{	75,	2194,	0,	0,	0 },
	{	76,	2296,	0,	0,	0 },
	{	77,	2395,	0,	0,	0 },
	{	78,	2502,	0,	0,	0 },
	{	79,	2620,	0,	0,	0 },
	{	80,	2733,	0,	0,	0 },
	{	81,	2855,	0,	0,	0 },
	{	82,	2969,	0,	0,	0 },
	{	83,	3115,	0,	0,	0 },
	{	84,	3251,	0,	0,	0 },
	{	85,	3400,	0,	0,	0 },
	{	86,	3535,	0,	0,	0 },
	{	87,	3712,	0,	0,	0 },
	{	88,	3873,	0,	0,	0 },
	{	89,	4049,	0,	0,	0 },
	{	90,	4202,	0,	0,	0 },
	{	91,	4410,	0,	0,	0 },
	{	92,	4592,	0,	0,	0 },
	{	93,	4789,	0,	0,	0 },
	{	94,	5005,	0,	0,	0 },
	{	95,	5240,	0,	0,	0 },
	{	96,	5499,	0,	0,	0 },
	{	97,	5710,	0,	0,	0 },
	{	98,	5939,	0,	0,	0 },
	{	99,	6273,	0,	0,	0 },
	{	100,	6550,	0,	0,	0 },
	{	101,	6852,	0,	0,	0 },
	{	102,	7070,	0,	0,	0 },
	{	103,	7423,	0,	0,	0 },
	{	104,	7814,	0,	0,	0 },
	{	105,	8098,	0,	0,	0 },
	{	106,	8404,	0,	0,	0 },
	{	107,	8908,	0,	0,	0 },
	{	108,	9279,	0,	0,	0 },
	{	109,	9683,	0,	0,	0 },
	{	110,	10123,	0,	0,	0 },
	{	111,	10605,	0,	0,	0 },
	{	112,	11135,	0,	0,	0 },
	{	113,	11421,	0,	0,	0 },
	{	114,	12038,	0,	0,	0 },
	{	115,	12726,	0,	0,	0 },
	{	116,	13100,	0,	0,	0 },
	{	117,	13919,	0,	0,	0 },
	{	118,	14368,	0,	0,	0 },
	{	119,	14847,	0,	0,	0 },
	{	120,	15907,	0,	0,	0 },
	{	121,	16496,	0,	0,	0 },
	{	122,	17131,	0,	0,	0 },
	{	123,	17816,	0,	0,	0 },
	{	124,	18559,	0,	0,	0 },
	{	125,	19365,	0,	0,	0 },
	{	126,	20246,	0,	0,	0 },
	{	127,	21210,	0,	0,	0 },
	{	128,	22270,	0,	0,	0 },
	{	129,	23442,	0,	0,	0 },
	{	130,	24745,	0,	0,	0 },
	{	131,	26200,	0,	0,	0 },
	{	132,	27838,	0,	0,	0 },
	{	133,	29694,	0,	0,	0 },
	{	134,	31815,	0,	0,	0 },
	{	135,	34262,	1,	0,	0 },
	{	136,	34262,	0,	0,	0 },
	{	137,	37117,	0,	0,	0 },
	{	138,	40491,	0,	0,	1 },
	{	139,	44540,	1,	0,	0 },
	{	140,	44540,	0,	0,	0 },
	{	141,	49489,	1,	0,	0 },
	{	142,	49489,	0,	0,	0 },
	{	143,	55675,	1,	0,	1 },
	{	144,	55675,	0,	0,	0 },
	{	145,	63629,	1,	0,	1 },
	{	146,	63629,	0,	0,	1 },
	{	147,	74233,	2,	0,	1 },
	{	148,	74233,	1,	0,	1 },
	{	149,	74233,	0,	0,	1 },
	{	150,	89079,	2,	0,	0 },
	{	151,	89079,	1,	0,	0 },
	{	152,	89079,	0,	0,	0 },
	{	153,	111349,	3,	0,	1 },
	{	154,	111349,	2,	0,	1 },
	{	155,	111349,	1,	0,	1 },
	{	156,	111349,	0,	0,	0 },
	{	157,	148464,	4,	0,	2 },
	{	158,	148464,	3,	0,	2 },
	{	159,	148464,	2,	0,	1 },
	{	160,	148464,	1,	0,	1 },
	{	161,	148464,	0,	0,	1 },
	{	162,	222692,	6,	0,	4 },
	{	163,	222692,	5,	0,	4 },
	{	164,	222692,	4,	0,	4 },
	{	165,	222692,	3,	0,	4 },
	{	166,	222692,	2,	0,	4 },
	{	167,	222692,	1,	0,	3 },
	{	168,	222692,	0,	0,	3 },
	{	169,	445362,	14,	0,	24 },
	{	170,	445362,	13,	0,	23 },
	{	171,	445362,	12,	0,	22 },
	{	172,	445362,	11,	0,	22 },
	{	173,	445362,	9,	0,	20 },
	{	174,	445362,	8,	0,	19 },
	{	175,	445362,	7,	0,	18 },
	{	176,	445362,	6,	0,	18 },
	{	177,	445362,	5,	0,	17 },
	{	178,	445362,	4,	0,	16 },
	{	179,	445362,	3,	0,	15 },
	{	180,	445362,	2,	0,	14 },
	{	181,	445362,	1,	0,	14 },
	{	182,	445362,	0,	0,	13 },


};

/* gain table. */
static code UINT16 gainTbl[49] = {
    0,     1,     2,     3,     4,     5,     6,     7,     8,     9, 
   10,    11,    12,    13,    14,    15,    16,    17,    18,    19, 
   20,    21,    22,    23,    24,    25,    26,    27,    28,    29, 
   30,    31,    48,    49,    50,    51,    52,    53,    54,    55, 
   56,    57,    58,    59,    60,    61,    62,    63,   112, 	
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

static code UINT8 ae50fpsMaxIdxTbl[2] = {
	AE_50FPS_50_MAX_IDX,
	AE_50FPS_45_MAX_IDX,
};

static code UINT8 ae50fpsMinIdxTbl[2] = {
	AE_50FPS_50_MIN_IDX,
	AE_50FPS_45_MIN_IDX,
};

static code UINT8 ae60fpsMaxIdxTbl[2] = {
	AE_60FPS_60_MAX_IDX,
	AE_60FPS_50_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_60_MIN_IDX,
	AE_60FPS_50_MIN_IDX,
};

static code UINT8 ae20fpsMaxIdxTbl[2] = {
	AE_20FPS_20_MAX_IDX,
	AE_20FPS_15_MAX_IDX,
};

static code UINT8 ae20fpsMinIdxTbl[2] = {
	AE_20FPS_20_MIN_IDX,
	AE_20FPS_15_MIN_IDX,
};


/* preview to snapshot binning rate table. */
static code UINT8 frontPrevToSnapBinningRateTbl[PREV_MODE_TOT] = {
	#if (PV_720_50FPS == 1)
	PREV_TO_SNAP_BINNING_RATE_50FPS_PREV,
	#endif
	#if (PV_720_50FPS == 0)
	PREV_TO_SNAP_BINNING_RATE_30FPS_PREV,
	#endif
	PREV_TO_SNAP_BINNING_RATE_60FPS_PREV,
	PREV_TO_SNAP_BINNING_RATE_20FPS_PREV,
};

/* preview tempoal denoise table. */
static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl[PREV_MODE_TOT] = {
	#if (PV_720_50FPS == 1)
	{  /* 50 fps. */
		MCLK_DIV_50FPS_PREV,
		MCLK_PHASE_50FPS_PREV,
		PCLK_DIV_50FPS_PREV,
		PCLK_PHASE_50FPS_PREV,
		PCLK_FREQ_50FPS_PREV_TD,
		31,
		6,
	},
	#endif
	#if (PV_720_50FPS == 0)
	{  /* 30 fps. */
		MCLK_DIV_30FPS_PREV + 5,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV + 5,
		PCLK_PHASE_30FPS_PREV,
		PCLK_FREQ_30FPS_PREV_TD,
		31,
		6,
	},
	#endif
	{  /* 60 fps. */
		MCLK_DIV_60FPS_PREV + 5,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV + 5,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV_TD,
		31,
		6,
	},
	{  /* 20 fps. */
		MCLK_DIV_20FPS_PREV,
		MCLK_PHASE_20FPS_PREV,
		PCLK_DIV_20FPS_PREV,
		PCLK_PHASE_20FPS_PREV,
		PCLK_FREQ_20FPS_PREV_TD,
		31,
		6,
	},
};

/* device capability. */
static code frontPrevCapabDesc_t frontPrevCapabDesc[PREV_MODE_TOT] = {
	#if (PV_720_50FPS == 1)
	{  /* 50 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_50FPS_PREV,
		HEIGHT_50FPS_PREV,
		XOFF_50FPS_PREV,
		YOFF_50FPS_PREV,
		LINE_TOT_50FPS_PREV,
		FRAME_TOT_50FPS_PREV,
		MCLK_DIV_50FPS_PREV,
		MCLK_PHASE_50FPS_PREV,
		PCLK_DIV_50FPS_PREV,
		PCLK_PHASE_50FPS_PREV,
		PCLK_FREQ_50FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_50FPS_PREV,
		RESHAPE_HFALL_50FPS_PREV,
		RESHAPE_HRISE_50FPS_PREV,
		RESHAPE_VEN_50FPS_PREV,
		RESHAPE_VFALL_50FPS_PREV,
		RESHAPE_VRISE_50FPS_PREV,
		ae30fps60hzTbl,
		ae30fps50hzTbl,
		ae50fpsMaxIdxTbl,
		ae50fpsMinIdxTbl,
		EV10_50FPS_EXP_IDX,
		gainTbl,
		GAIN_50FPS_MAX_IDX,
		GAIN_50FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[0],
		SENSOR_ZOOM_FACTOR_50FPS_PREV,
	},
	#endif
	#if (PV_720_50FPS == 0)
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
	#endif
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
		&frontPrevTempDenoiseTbl[1],
		SENSOR_ZOOM_FACTOR_60FPS_PREV,
	},
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
		&frontPrevTempDenoiseTbl[2],
		SENSOR_ZOOM_FACTOR_20FPS_PREV,
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
	/*
	;========	 OV5653 registers setting	=============
	;
	; OVT code base	:	OV5650R1B_A09.ovd 
	; Customer 	:	iCatch
	; Back end Chip	:	
	; Rev. 		: 	v0.11
	; Date		:	2010/06/15
	;=========================================
	;;
	;--------------------------------
	;		Sensor Initial Setting
	;	resolution : 640X480 30fps 
	;	input	:	24MHZ
	;	output	:	48MHZ
	;	fps		:	30fps
	; 	HTS		: 	2168
	; 	VTS		:	1476
	---------------------------------
	*/
	//0x31, 0xFF, 0x00,	/* select old SCCB */
	0x30, 0x08, 0x82,
	/* ;delay some time */
	
	/* to frontInit1CmdTbl */
};

static code UINT8 frontInit1CmdTbl[] = {
	#if 1
	0x30, 0x08, 0x42,
	0x31, 0x03, 0x93,
	0x3b, 0x07, 0x0c,
	0x30, 0x17, 0xff,
	0x30, 0x18, 0xfc,
	0x37, 0x06, 0x41,
	0x37, 0x03, 0xe6,
	0x36, 0x13, 0x44,
	0x36, 0x30, 0x22,
	0x36, 0x05, 0x04,
	0x36, 0x06, 0x3f,
	0x37, 0x12, 0x13,
	0x37, 0x0e, 0x00,
	0x37, 0x0b, 0x40,
	0x36, 0x00, 0x54,
	0x36, 0x01, 0x05,
	0x37, 0x13, 0x22,
	0x37, 0x14, 0x27,
	0x36, 0x31, 0x22,
	0x36, 0x12, 0x1a,
	0x36, 0x04, 0x40,
	0x37, 0x05, 0xda,
	0x37, 0x0a, 0x80,
	0x37, 0x0c, 0x00,
	0x37, 0x10, 0x28,
	0x37, 0x02, 0x3a,
	0x37, 0x04, 0x18,
	0x3a, 0x18, 0x00,
	0x3a, 0x19, 0xf8,
	0x3a, 0x00, 0x38,
	0x38, 0x00, 0x02,
	0x38, 0x01, 0x54,
	0x38, 0x03, 0x0c,
	0x38, 0x0c, 0x0c,
	0x38, 0x0d, 0xb4,
	0x38, 0x0e, 0x07,
	0x38, 0x0f, 0xb0,
	0x38, 0x30, 0x50,
	0x3a, 0x08, 0x12,
	0x3a, 0x09, 0x70,
	0x3a, 0x0a, 0x0f,
	0x3a, 0x0b, 0x60,
	0x3a, 0x0d, 0x06,
	0x3a, 0x0e, 0x06,
	0x3a, 0x13, 0x54,
	0x38, 0x15, 0x82,
	0x50, 0x59, 0x80,
	0x36, 0x15, 0x52,
	0x50, 0x5a, 0x0a, 
	0x50, 0x5b, 0x2e,
	0x3a, 0x1a, 0x06,
	0x35, 0x03, 0x00,
	0x36, 0x23, 0x01,
	0x36, 0x33, 0x24,
	0x3c, 0x01, 0x34,
	0x3c, 0x04, 0x28,
	0x3c, 0x05, 0x98,
	0x3c, 0x07, 0x07,
	0x3c, 0x09, 0xc2,
	0x40, 0x00, 0x05,
	0x40, 0x06, 0x00,	/* ablc trage level hi */
	0x40, 0x07, 0x05,	/* ablc trage level low */
	0x40, 0x1d, 0x28,	/* blc by gain */
	0x40, 0x01, 0x02,
	0x40, 0x1c, 0x46,
	0x50, 0x46, 0x01,
	0x38, 0x10, 0x40,
	0x38, 0x36, 0x41,
	0x50, 0x5f, 0x04,
	0x50, 0x00, 0x00,
	0x50, 0x01, 0x00,
	0x50, 0x02, 0x00,
	0x50, 0x3d, 0x00,
	0x59, 0x01, 0x00,
	0x58, 0x5a, 0x01,
	0x58, 0x5b, 0x2c,
	0x58, 0x5c, 0x01,
	0x58, 0x5d, 0x93,
	0x58, 0x5e, 0x01,
	0x58, 0x5f, 0x90,
	0x58, 0x60, 0x01,
	0x58, 0x61, 0x0d,
	0x51, 0x80, 0xc0,
	0x51, 0x84, 0x00,
	0x47, 0x0a, 0x00,
	0x47, 0x0b, 0x00,
	0x47, 0x0c, 0x00,
	0x30, 0x0f, 0x8e,
	0x36, 0x03, 0xa7,
	0x36, 0x32, 0x55,
	0x36, 0x20, 0x56,
	0x36, 0x21, 0x2f,
	0x38, 0x1a, 0x3c,
	0x38, 0x18, 0xc0,
	0x36, 0x31, 0x36,
	0x36, 0x32, 0x5f,
	0x37, 0x11, 0x24,

	0x36, 0x13,	0xc4,
	0x36, 0x21,	0xaf,	/* binning (H binning/sub-sample enable ) */
	0x37, 0x03,	0x9a,
	0x37, 0x05,	0xdc,
	0x37, 0x0a,	0x81,
	0x37, 0x0c,	0xc8,
	0x37, 0x0d,	0x42,
	0x37, 0x13,	0x92,
	0x37, 0x14,	0x17, 

	/* ;vga resolution */
	0x38, 0x00, 0x02,
	0x38, 0x01, 0x54,
	0x38, 0x03, 0x08,
	0x38, 0x04, 0x05,
	0x38, 0x05, 0x00,
	0x38, 0x06, 0x01,
	0x38, 0x07, 0xe0,
	0x38, 0x08, 0x02,
	0x38, 0x09, 0x80,
	0x38, 0x0a, 0x01,
	0x38, 0x0b, 0xe0,
	0x38, 0x0C, 0x08,
	0x38, 0x0D, 0x78,
	0x38, 0x0E, 0x05,
	0x38, 0x0F, 0xc4,
	0x38, 0x18, 0xc2,
	0x38, 0x1a, 0x00,
	0x38, 0x1c, 0x20,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x01,
	0x38, 0x1f, 0x20,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x00,
	0x38, 0x24, 0x23,
	0x38, 0x25, 0x20,
	0x38, 0x26, 0x00,
	0x38, 0x27, 0x08,	

	/* ;isp top */
	0x50, 0x02, 0x02,	/* vap (enable) */
	0x59, 0x01, 0x04,	/* vap (sub-sample 2) */

	/* ;blc */
	0x40, 0x1c, 0x42,	/* rsvd */
	
	0x40, 0x1f, 0x03,
	0x30, 0x08, 0x02,
	/* ;PCLK=96MHZ */
	0x30, 0x0e, 0x18,
	0x30, 0x0f, 0x8e,
	0x30, 0x10, 0x10,
	0x30, 0x11, 0x10,
	0x30, 0x12, 0x02,
	/* ;disable AGC/AEC */
	0x35, 0x03, 0x17,
	0x35, 0x00, 0x00,
	0x35, 0x01, 0x10,
	0x35, 0x02, 0x00,
	0x35, 0x0a, 0x00,
	0x35, 0x0b, 0x10,
	/* ;disable AWB */
	0x34, 0x06, 0x01,
	0x34, 0x00, 0x04,
	0x34, 0x01, 0x00,
	0x34, 0x02, 0x04,
	0x34, 0x03, 0x00,
	0x34, 0x04, 0x04,
	#if (USED_INTERNAL_LDO == 0)
	0x30, 0x30, 0x20,	/* external ldo */
	#else
	0x30, 0x30, 0x00,	/* internal ldo */
	#endif

	/* ;SCCB Ctrl Option */
	0x31, 0x0A, 0x1C,
	/* ;pattern enable */
	//0x50, 0x3d, 0x80,

	/* OB */
	//0x40, 0x06, 0x00,
	//0x40, 0x07, 0x30,
	#endif      
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	#if (PV_720_50FPS == 1)
	//@@ DVP Sensor Raw P720 50fps
	//100 99 1280 720
	//100 98 1 0
	//;--------------------------------
	//;	1280x720 30fps key setting	
	//;	input	:	21.75MHZ
	//;	output	:	87MHZ
	//;	fps		:	50fps
	//; HTS		: 2162
	//; VTS		:	782
	//;--------------------------------
	0x30, 0x10,	0x10,
	0x30, 0x11,	0x10,
	/* ;group start */
	0x32, 0x00,	0x00,
	0x32, 0x01,	0xff,
	0x32, 0x02,	0xff,
	0x32, 0x03,	0xff,
	0x32, 0x12,	0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13,	0xc4,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0xbf,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0xaf,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03,	0x9a,
	0x37, 0x05,	0xdb,
	0x37, 0x0a,	0x81,
	0x37, 0x0c,	0x00,
	0x37, 0x0d,	0x42,	/* for v binning on [6] = 1 */
	0x37, 0x13,	0x92,
	0x37, 0x14,	0x17, 
	/* ; */
	0x38, 0x00,	0x02,	/* HS 596 */
	0x38, 0x01,	0x54,	/* HS */
	0x38, 0x02, 0x00,	/* VS 12 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04,	0x05,	/* HW 1288 */
	0x38, 0x05,	0x08,	/* HW */
	0x38, 0x06,	0x02,	/* VH 728 */
	0x38, 0x07,	0xd8,	/* VH */
	
	0x38, 0x08,	0x05,	/* DVP H output size 1288 */
	0x38, 0x09,	0x08,	/* DVP H output size */
	0x38, 0x0a,	0x02,	/* DVP V output size 728 */
	0x38, 0x0b,	0xd8,	/* DVP V output size */
	
	0x38, 0x0C,	0x08,	/* THS 2162 */
	0x38, 0x0D,	0xb1,	/* THS */
	0x38, 0x0E,	0x03,	/* TVS 782 */
	0x38, 0x0F,	0x0E,	/* TVS */

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp off [5] */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x81,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp off [5] */
	#endif
	
	0x38, 0x1a,	0x00,
	0x38, 0x1c,	0x10,
	0x38, 0x1d,	0xfe,
	0x38, 0x1e,	0x05,
	0x38, 0x1f,	0xac,
	0x38, 0x20,	0x00,
	0x38, 0x21,	0x20,
	0x38, 0x24,	0x23,
	0x38, 0x25,	0x2a,
	0x38, 0x26,	0x00,	
	
	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	/* ; */
	0x50, 0x02,	0x00,
	0x59, 0x01,	0x00,
	/* ; */
	0x40, 0x1c, 0x42,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif
	/* ;group launch */
	0x32, 0x12,	0x10,
	0x32, 0x12,	0xa0,
	/* ;dummy line reg */
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	/* ;; */
	/* ;; */
	#endif
	
	#if (PV_720_50FPS == 0)
	//@@ DVP Sensor Raw P964 30fps Mirror on 
	//100 99 1288 964
	//100 98 1 0
	//;--------------------------------
	//;	1288x964 30fps key setting	(h,v binning)
	//;	input	:	24MHZ
	//;	output	:	66MHZ
	//;	fps		:	30fps
	//; HTS		: 	2162
	//; VTS		:	1018
	//;--------------------------------
	0x30, 0x10, 0x10,
	0x30, 0x11, 0x0b,
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0xc4,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0xbf,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0xaf,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0x9a,
	0x37, 0x05, 0xdb,
	0x37, 0x0a, 0x81,
	0x37, 0x0c, 0x00,
	0x37, 0x0d, 0x42,	/* for v binning on [6] = 1 */
	0x37, 0x13, 0x92,
	0x37, 0x14, 0x17, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS 596 */
	0x38, 0x01, 0x54,	/* HS */
	0x38, 0x02, 0x00,	/* VS 12 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x05,	/* HW 1288 */
	0x38, 0x05, 0x08,	/* HW */
	0x38, 0x06, 0x03,	/* VH 964 */
	0x38, 0x07, 0xc8,	/* VH */
	
	0x38, 0x08, 0x05,	/* DVP H output size 1288 */
	0x38, 0x09, 0x08,	/* DVP H output size */
	0x38, 0x0a, 0x03,	/* DVP V output size 964 */
	0x38, 0x0b, 0xc8,	/* DVP V output size */
	
	0x38, 0x0C, 0x08,	/* THS 2132 */
	0x38, 0x0D, 0x54,//0xA2,	/* THS */
	0x38, 0x0E, 0x03,	/* TVS 1018 */
	0x38, 0x0F, 0xfa,//0xD8,	/* TVS */

	0x38, 0x15, 0x81,

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp off [5] */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x81,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp off [5] */
	#endif
	
	0x38, 0x1a, 0x00,
	0x38, 0x1c, 0x30,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x07,
	0x38, 0x1f, 0xb0,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x20,
	0x38, 0x24, 0x23,
	0x38, 0x25, 0x26,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#endif

	/* ; */
	0x50, 0x02, 0x00,
	0x59, 0x01, 0x00,
	/* ; */
	0x40, 0x1c, 0x42,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
	/* ; */
	/* ;dummy line reg */
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	/* ;; */
	/* ;; */
	#endif
};

static code UINT8 front60fpsPrevCmdTbl[] = {	
	//;@@ DVP Sensor Raw VGA 60fps
	//;100 99 640 480
	//;100 98 1 0
	//;--------------------------------
	//;	640X480 60fps key setting	
	//;	input	:	24MHZ
	//;	output	: 	50MHZ
	//;	fps 		:	60fps
	//;  HTS		: 	2264
	//;  VTS		:	738
	//;--------------------------------
	0x30, 0x10, 0x10,
	//0x30, 0x11, 0x0f,	/* 59.5fps */
	0x30, 0x11, 0x11,	/* 60fps */
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0xc4,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0xbf,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0xaf,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0x9a,
	0x37, 0x05, 0xdc,
	0x37, 0x0a, 0x81,
	0x37, 0x0c, 0xc8,
	0x37, 0x0d, 0x42,	/* for v binning on [6] = 1 */
	0x37, 0x13, 0x92,
	0x37, 0x14, 0x17, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS  596 */
	0x38, 0x01, 0x54,	/* HS */

	0x38, 0x02, 0x00,	/* VS 9 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x05,	/* HW 1300 */
	0x38, 0x05, 0x14,	/* HW */
	
	0x38, 0x06, 0x01,	/* VH 496 */
	0x38, 0x07, 0xf0,	/* VH */
	
	0x38, 0x08, 0x02,	/* DVP H output size 650 */
	0x38, 0x09, 0x8a,	/* DVP H output size */
	
	0x38, 0x0a, 0x01,	/* DVP V output size 496 */
	0x38, 0x0b, 0xf0,	/* DVP V output size */
	
	0x38, 0x0C, 0x08,//0x0A,	/* THS 2264 */
	0x38, 0x0D, 0xd8,	/* THS, 60fps */
	
	0x38, 0x0E, 0x02,	/* TVS 738 */
	0x38, 0x0F, 0xe2,	/* TVS */

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa2,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe2,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc2,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp off [5] */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x82,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp off [5] */
	#endif
	
	0x38, 0x1a, 0x00,
	0x38, 0x1c, 0x20,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x01,
	0x38, 0x1f, 0x20,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x00,
	0x38, 0x24, 0x23,
	0x38, 0x25, 0x20,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#endif

	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif
	/* ; */
	0x50, 0x02, 0x02,
	0x59, 0x01, 0x04,
	/* ; */
	0x40, 0x1c, 0x42,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
};

static code UINT8 frontSnapCmdTbl[] = {
	//@@ DVP Sensor Raw 2600x1948 15fps Mirror on
	//100 99 2600 1948
	//100 98 1 0
	//;--------------------------------
	//;	2600x1948 10fps key setting	
	//;	input	:	24MHZ
	//;	output:	96MHZ
	//;	fps		:	15 fps
	//; HTS		: 	3252
	//; VTS		:	1968
	//;--------------------------------
	//0x30, 0x10, 0x10,	/* 96M */
	//0x30, 0x11, 0x10,
	0x30, 0x10, 0x30,	/* 65M */
	0x30, 0x11, 0x16,
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0x44,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0x3f,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0x2f,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0xe6,
	0x37, 0x05, 0xda,
	0x37, 0x0a, 0x80,
	0x37, 0x0c, 0x00,
	0x37, 0x0d, 0x04,	
	0x37, 0x13, 0x22,
	0x37, 0x14, 0x27, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS  588 */
	0x38, 0x01, 0x4c,	/* HS */
	0x38, 0x02, 0x00,	/* VS 12 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x0c,	//0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x0c,	//0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x0a,	/* HW 2600 */
	0x38, 0x05, 0x28,	/* HW */
	0x38, 0x06, 0x07,	/* VH 1948 */
	0x38, 0x07, 0x9c,	/* VH */
	0x38, 0x08, 0x0a,	/* DVP H output size 2600 */
	0x38, 0x09, 0x28,	/* DVP H output size */
	0x38, 0x0a, 0x07,	/* DVP V output size 1948 */
	0x38, 0x0b, 0x9c,	/* DVP V output size */
	0x38, 0x0C, 0x0c,	/* THS 3252 */
	0x38, 0x0D, 0xb4,	/* THS */
	0x38, 0x0E, 0x07,	/* TVS 1968 */
	0x38, 0x0F, 0xb0,	/* TVS */

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa0,	/* for mirror off & vfilp on */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe0,	/* for mirror on & vfilp on */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc0,	/* for mirror on & vfilp off */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x80,	/* for mirror off & vfilp off */
	#endif
	
	0x38, 0x1a, 0x3c,
	0x38, 0x1c, 0x20,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x01,
	0x38, 0x1f, 0x20,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x00,
	0x38, 0x24, 0x01,
	0x38, 0x25, 0xb4,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x0a,	//0x0c,	/* VS,//mirror & vfilp check */
	#else
	0x38, 0x27, 0x0a,	//0x0c,	/* VS,//mirror & vfilp check */
	#endif
	
	/* ; */
	0x50, 0x02, 0x00,
	0x59, 0x01, 0x00,
	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif	
	/* ; */
	0x40, 0x1c, 0x46,
	0x40, 0x1d, 0x08,	/* blc by frame */
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
	/* ;; */
	/* ;; */
};

static code UINT8 front20fpsPrevCmdTbl[] = {
	//;@@ DVP Sensor Raw VGA 30fps
	//;100 99 640 480
	//;100 98 1 0
	//;--------------------------------
	//;	640X480 60fps key setting	
	//;	input	:	24MHZ
	//;	output	: 	50MHZ
	//;	fps 		:	30fps
	//;  HTS		: 	2264
	//;  VTS		:	738
	//;--------------------------------
	0x30, 0x10, 0x30,	/* 30fps */
	0x30, 0x11, 0x11,
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0xc4,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0xbf,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0xaf,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0x9a,
	0x37, 0x05, 0xdc,
	0x37, 0x0a, 0x81,
	0x37, 0x0c, 0xc8,
	0x37, 0x0d, 0x42,	/* for v binning on [6] = 1 */
	0x37, 0x13, 0x92,
	0x37, 0x14, 0x17, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS  596 */
	0x38, 0x01, 0x54,	/* HS */

	0x38, 0x02, 0x00,	/* VS 8 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x08,	//0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x08,	//0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x05,	/* HW 1300 */
	0x38, 0x05, 0x14,	/* HW */
	
	0x38, 0x06, 0x01,	/* VH 496 */
	0x38, 0x07, 0xf0,	/* VH */
	
	0x38, 0x08, 0x02,	/* DVP H output size 650 */
	0x38, 0x09, 0x8a,	/* DVP H output size */
	
	0x38, 0x0a, 0x01,	/* DVP V output size 496 */
	0x38, 0x0b, 0xf0,	/* DVP V output size */
	
	0x38, 0x0C, 0x08,	/* THS 2264 */
	0x38, 0x0D, 0xd8,	/* THS, 60fps */
	
	0x38, 0x0E, 0x02,	/* TVS 738 */
	0x38, 0x0F, 0xe2,	/* TVS */

	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa2,	/* for mirror off & vfilp on */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe2,	/* for mirror on & vfilp on */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc2,	/* for mirror on & vfilp off */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x82,	/* for mirror off & vfilp off */
	#endif
	
	0x38, 0x1a, 0x00,
	0x38, 0x1c, 0x20,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x01,
	0x38, 0x1f, 0x20,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x00,
	0x38, 0x24, 0x23,
	0x38, 0x25, 0x20,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x09,	//0x0c,	/* VS, mirror & vfilp check */
	#endif

	/* ; */
	0x50, 0x02, 0x02,
	0x59, 0x01, 0x04,
	/* ; */
	0x40, 0x1c, 0x42,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
};

static code UINT8 front30fpsPrevDz1xCmdTbl[] = {
	#if (PV_720_50FPS == 0)
	//@@ DVP Sensor Raw P964 30fps Mirror on 
	//100 99 1288 964
	//100 98 1 0
	//;--------------------------------
	//;	1288x964 30fps key setting	(h,v binning)
	//;	input	:	24MHZ
	//;	output	:	66MHZ
	//;	fps		:	30fps
	//; HTS		: 	2162
	//; VTS		:	1018
	//;--------------------------------
	0x30, 0x10, 0x10,
	0x30, 0x11, 0x0b,
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0xc4,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0xbf,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0xaf,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0x9a,
	0x37, 0x05, 0xdb,
	0x37, 0x0a, 0x81,
	0x37, 0x0c, 0x00,
	0x37, 0x0d, 0x42,	/* for v binning on [6] = 1 */
	0x37, 0x13, 0x92,
	0x37, 0x14, 0x17, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS 596 */
	0x38, 0x01, 0x54,	/* HS */
	0x38, 0x02, 0x00,	/* VS 12 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x05,	/* HW 1288 */
	0x38, 0x05, 0x08,	/* HW */
	0x38, 0x06, 0x03,	/* VH 964 */
	0x38, 0x07, 0xc8,	/* VH */
	
	0x38, 0x08, 0x05,	/* DVP H output size 1288 */
	0x38, 0x09, 0x08,	/* DVP H output size */
	0x38, 0x0a, 0x03,	/* DVP V output size 964 */
	0x38, 0x0b, 0xc8,	/* DVP V output size */
	
	0x38, 0x0C, 0x08,	/* THS 2132 */
	0x38, 0x0D, 0xA2,	/* THS */
	0x38, 0x0E, 0x03,	/* TVS 1018 */
	0x38, 0x0F, 0xD8,	/* TVS */

	0x38, 0x15, 0x81,

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp on [5] */
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc1,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp off [5] */
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x81,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp off [5] */
	#endif
	
	0x38, 0x1a, 0x00,
	0x38, 0x1c, 0x30,
	0x38, 0x1d, 0x0a,
	0x38, 0x1e, 0x07,
	0x38, 0x1f, 0xb0,
	0x38, 0x20, 0x00,
	0x38, 0x21, 0x20,
	0x38, 0x24, 0x23,
	0x38, 0x25, 0x26,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#endif

	/* ; */
	0x50, 0x02, 0x00,
	0x59, 0x01, 0x00,
	/* ; */
	0x40, 0x1c, 0x42,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
	/* ; */
	/* ;dummy line reg */
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	/* ;; */
	/* ;; */
	#endif
};
static UINT8 front30fpsPrevDz2xCmdTbl[] = {
	#if (PV_720_50FPS == 0)
	//@@ DVP Sensor Raw P964 30fps Mirror on 
	//100 99 1288 964
	//100 98 1 0
	//;--------------------------------
	//;	1288x964 30fps key setting	(h,v binning)
	//;	input	:	24MHZ
	//;	output	:	66MHZ
	//;	fps		:	30fps
	//; HTS		: 	2162
	//; VTS		:	1018
	//;--------------------------------
	0x30, 0x10, 0x10,
	0x30, 0x11, 0x0b,
	/* ;group start */
	0x32, 0x00, 0x00,
	0x32, 0x01, 0xff,
	0x32, 0x02, 0xff,
	0x32, 0x03, 0xff,
	0x32, 0x12, 0x00,
	/* ; */
	/* ;resolution */
	0x36, 0x13, 0x44,

	#if (MIRROR_CTRL == 0)
	0x36, 0x21, 0x3f,	/* for mirror off [4] = 1, h binning on [7] = 1, h subsampling [6] = 0 */
	#else
	0x36, 0x21, 0x2f,	/* for mirror on [4] = 0, h binning on [7] = 1, h subsampling [6] = 0 */
	#endif
	
	0x37, 0x03, 0xE6,
	0x37, 0x05, 0xda,
	0x37, 0x0a, 0x80,
	0x37, 0x0c, 0x00,
	0x37, 0x0d, 0x04,	/* for v binning on [6] = 1 */
	0x37, 0x13, 0x22,
	0x37, 0x14, 0x27, 
	/* ; */
	0x38, 0x00, 0x02,	/* HS 596 */
	0x38, 0x01, 0x54,	/* HS */
	0x38, 0x02, 0x00,	/* VS 12 */

	#if (FLIP_CTRL == 0)
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x03, 0x0c,	/* VS, mirror & vfilp check */
	#endif
	
	0x38, 0x04, 0x05,	/* HW 1288 */
	0x38, 0x05, 0x08,	/* HW */
	0x38, 0x06, 0x03,	/* VH 964 */
	0x38, 0x07, 0xc8,	/* VH */
	
	0x38, 0x08, 0x05,	/* DVP H output size 1288 */
	0x38, 0x09, 0x08,	/* DVP H output size */
	0x38, 0x0a, 0x03,	/* DVP V output size 964 */
	0x38, 0x0b, 0xc8,	/* DVP V output size */
	
	0x38, 0x0C, 0x08,	/* THS 2132 */
	0x38, 0x0D, 0xA2,	/* THS */
	0x38, 0x0E, 0x03,	/* TVS 1018 */
	0x38, 0x0F, 0xD8,	/* TVS */

	0x38, 0x15, 0x82,	//++

	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 1)
	0x38, 0x18, 0xa0,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp on [5] */	//++
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 1)
	0x38, 0x18, 0xe0,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp on [5] */	//++
	#endif
	#if (MIRROR_CTRL == 1 && FLIP_CTRL == 0)
	0x38, 0x18, 0xc0,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror on [6], vfilp off [5] */	//++
	#endif
	#if (MIRROR_CTRL == 0 && FLIP_CTRL == 0)
	0x38, 0x18, 0x80,	/* for v 2x sub-sample [0], v 4x sub-sample [1], mirror off [6], vfilp off [5] */	//++
	#endif
	
	0x38, 0x1a, 0x1c,	//++
	0x38, 0x1c, 0x31,	//++
	0x38, 0x1d, 0xf0,	//++
	0x38, 0x1e, 0x03,	//++
	0x38, 0x1f, 0xe8,	//++
	0x38, 0x20, 0x06,	//0x09,
	0x38, 0x21, 0x11,
	0x38, 0x24, 0x22,	//0x22,
	0x38, 0x25, 0xe0,	//0xe0,
	0x38, 0x26, 0x00,

	#if (FLIP_CTRL == 0)
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#else
	0x38, 0x27, 0x0c,	/* VS, mirror & vfilp check */
	#endif

	/* ; */
	0x50, 0x02, 0x00,
	0x59, 0x01, 0x00,
	/* ; */
	0x40, 0x1c, 0x46,
	#if (PV_AUTO_OB_BY_GAIN == 1)
	0x40, 0x1d, 0x28,	/* blc by gain */
	#else
	0x40, 0x1d, 0x08,	/* blc by frame */
	#endif
	/* ; */
	#if (MIRROR_CTRL == 0)
	0x50, 0x5a, 0x00,	/* mirror & vfilp check */
	0x50, 0x5b, 0x12,	/* mirror & vfilp check */
	#else
	0x50, 0x5a, 0x0a,	/* mirror & vfilp check */
	0x50, 0x5b, 0x2e,	/* mirror & vfilp check */
	#endif
	/* ; */
	/* ;group launch */
	0x32, 0x12, 0x10,
	0x32, 0x12, 0xa0,
	/* ; */
	/* ;dummy line reg */
	0x35, 0x0c, 0x00,
	0x35, 0x0d, 0x00,
	/* ;; */
	/* ;; */
	#endif
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


#if 0
frontSystem_t ov5653 = {
	0,	/* current preview mode */
	0,	/* current snapshot mode */
	0,	/* preview last set gain value */
	0,	/* preview last set gain table index */
	0,	/* preview last set shutter value */
	0,	/* snapshot last set gain value */
	0,	/* snapshot last set shutter value */
	0,	/* for ov sensor auto ob need change gain */
	0,	/* load back option */
	0,	/* pre-sensor mode */
};
#endif

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
	opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontOpIntrPreviewSensorZoomModeSet;
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
		case 24:
		case 20:
			printf("frontOpFrameRateSet = 24\n");
			frontPrevMode = 2;
			//frontSnapMode = 1;

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

	/* before power on. */
	frontBeforePowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	/* used (RDK) GPIO12, GPIO13, (EVB) TP_YN & TP_XN GPIO */
	#if (DBG_SNAP_TIMING != 0)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cRegClkCfgSet(I2C_REG_CLK_DIV);
	i2cClkCfgSet(I2C_CLK_DIV);	

	/* send i2c command. */
	globalTimerWait(300);

	frontResCmdBinSend(_frontInit0CmdTbl);
	
	globalTimerWait(50);
	
	frontResCmdBinSend(_frontInit1CmdTbl);
	
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
	frontPrevCapabDesc_t *pcap;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
	}
	else if ( frontPrevMode == 1 ) {  /* 60 fps. */
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
	}
	else if (frontPrevMode == 2) {	/* 24 fps */
		frontResCmdBinSend(_front20fpsPrevCmdTbl);
	}

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

	cdspBayerPatternSet(BAYER_PTN_PREV);

	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */

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
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_60FPS_PREV) {//2x
			
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front60fpsPrevDz2xCmdTbl, 2, sizeof(front60fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
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
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_20FPS_PREV) {//2x
				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);
				
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

				i2cCmdTableSend(front20fpsPrevDz2xCmdTbl, 2, sizeof(front20fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
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
				
				i2cCmdTableSend(front30fpsPrevDz1xCmdTbl, 2, sizeof(front30fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_60FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front60fpsPrevDz1xCmdTbl, 2, sizeof(front60fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_20FPS_PREV) {//1x

				sp1kAeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				
				i2cCmdTableSend(front20fpsPrevDz1xCmdTbl, 2, sizeof(front20fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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

				i2cCmdTableSend_Intr(front30fpsPrevDz2xCmdTbl, 2, sizeof(front30fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
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
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_60FPS_PREV) {//2x
			
				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front60fpsPrevDz2xCmdTbl, 2, sizeof(front60fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
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
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor >= SENSOR_ZOOM_FACTOR_20FPS_PREV) {//2x
				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);

				i2cCmdTableSend_Intr(front20fpsPrevDz2xCmdTbl, 2, sizeof(front20fpsPrevDz2xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
				
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
				
				i2cCmdTableSend_Intr(front30fpsPrevDz1xCmdTbl, 2, sizeof(front30fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		else if (frontPrevMode == 1) {	/* 60 fps */
			#if (SENSOR_ZOOM_FACTOR_60FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_60FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);
				
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				
				i2cCmdTableSend_Intr(front60fpsPrevDz1xCmdTbl, 2, sizeof(front60fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		else if (frontPrevMode == 2) {	/* 20 fps */
			#if (SENSOR_ZOOM_FACTOR_20FPS_PREV != 0)
			if (factor < SENSOR_ZOOM_FACTOR_20FPS_PREV) {//1x

				aeIntrCurGainValueGet(&pvGainIdxVal);

				#if 0//intr used
				frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
				frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
				#else
				paraAry = 1;
				HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
				#endif
				
				i2cCmdTableSend_Intr(front20fpsPrevDz1xCmdTbl, 2, sizeof(front20fpsPrevDz1xCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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

	if ( frontPrevSensorZoom2xGainEn ) {
		frontPrevSensorZoomSnapStatus = 1;
	}
	
	if ( first != 0 ) {
		#if (DBG_SNAP_TIMING >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
		
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
__STATIC void
frontOpGainSet(
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[3*2];
	UINT8 TempGain = 0;
	UINT8 digitalZoom2xGain = 0x00;
	UINT8 digitalZoom2xGain_h = 0x00;
	//UINT16 ADDR16=0x350b;

	//Manual Gain Ctrl First Set REG bit 0x3503[1] to Enable
	//Gain = high bit in 0x350A[0], low bit in 0x350B[7:0]
	//Gain = (0x350B[6]+1)*(0x350B[5]+1)*(0x350B[4]+1)*(0x350B[3:0]/16+1)

	//printf("gain idx=%bu\n",val);

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			digitalZoom2xGain = 0x80;
			digitalZoom2xGain_h = 0x01;
		}
		else if ( frontPrevMode == 1 ) {	/* 60fps */
			//digitalZoom2xGain = 0x05;
		}
		else if ( frontPrevMode == 2 ) {	/* 24fps */
			//digitalZoom2xGain = 0x07;
		}
	}

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0B;
	cmdTbl[2] = (UINT8)((frontLastGain|((UINT16)digitalZoom2xGain)) & 0x00FF);
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0A;
	cmdTbl[5] = digitalZoom2xGain_h;
	
	//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[3*3];
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
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);
	
	if (espline != 0) {
		if(espline > (ftotal - 6)) {
			espline = (ftotal - 6);
			//printf("re-define espline=%d\n",espline);
		}
	}
	else {
		espline = 1;
	}
	
	espline_16 = espline << 4;
	//printf("espline_16=%lu\n",espline_16);

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x00;
	cmdTbl[2] = (UINT8)((espline_16 & 0x00FF0000) >> 16);
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x01;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x02;
	cmdTbl[8] = (UINT8)(espline_16 & 0x000000F0);
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

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
	UINT8 TempGain = 0;
	UINT8 digitalZoom2xGain = 0x00;
	UINT8 digitalZoom2xGain_h = 0x00;
	//UINT16 ADDR16=0x350b;

	//Manual Gain Ctrl First Set REG bit 0x3503[1] to Enable
	//Gain = high bit in 0x350A[0], low bit in 0x350B[7:0]
	//Gain = (0x350B[6]+1)*(0x350B[5]+1)*(0x350B[4]+1)*(0x350B[3:0]/16+1)

	//printf("gain idx=%bu\n",val);

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	#if 1
	switch (frontPrevDyTdSwitchGain) {
		case 1:
			//printf("%bu,G -\n",val);
			switch (frontPrevMode) {
				case 0:
					if (val >= 1) {
						val -= 1;
					}
					else {
						val = 0;
					}
					break;
				case 1:
					if (val >= 3) {
						val -= 3;
					}
					else {
						val = 0;
					}
					break;
			}
			
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		case 2:
			//printf("%bu,G +\n",val);
			switch (frontPrevMode) {
				case 0:
					if (val < 48) {
						val += 1;
					}
					else {
						val = val-1;
					}
					break;
				case 1:
					if (val < 46) {
						val += 3;
					}
					else {
						val = val-1;
					}
					break;
			}
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		default:
			break;
	}
	#else
	switch (frontPrevDyTdSwitchGain) {
		case 1:
			//printf("%bu,G -\n",val);
			switch (frontPrevMode) {
				case 0:
					val -= 1;
					break;
				case 1:
					val -= 3;
					break;
			}
			
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		case 2:
			//printf("%bu,G +\n",val);
			switch (frontPrevMode) {
				case 0:
					val += 1;
					break;
				case 1:
					val += 3;
					break;
			}
			frontPrevDyTdSwitchGain = 0;
			//XBYTE[REG_Front_F2CdspGate]|=0x01;
			break;
		default:
			break;
	}
	#endif

	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			digitalZoom2xGain = 0x80;
			digitalZoom2xGain_h = 0x01;
		}
		else if ( frontPrevMode == 1 ) {	/* 60fps */
			//digitalZoom2xGain = 0x05;
		}
		else if ( frontPrevMode == 2 ) {	/* 24fps */
			//digitalZoom2xGain = 0x07;
		}
	}

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0B;
	cmdTbl[2] = (UINT8)((frontLastGain|((UINT16)digitalZoom2xGain)) & 0x00FF);
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0A;
	cmdTbl[5] = digitalZoom2xGain_h;
	//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
	
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
__STATIC void
frontOpIntrExposureTimeSet(
	frontExposureTimeArg_t *parg
)
{
	frontPrevCapabDesc_t *pcap;
	UINT16 ltotal, ftotal, espline;
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[3*3];
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
	espline = (pixclk * 10) / (ltotal * frontLastShutter);
	//printf("espline=%d\n",espline);

	if (espline != 0) {
		if(espline > (ftotal - 6)) {
			espline = (ftotal - 6);
			//printf("re-define espline=%d\n",espline);
		}
	}
	else {
		espline = 1;
	}
	
	espline_16 = espline << 4;
	//printf("espline_16=%lu\n",espline_16);

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x00;
	cmdTbl[2] = (UINT8)((espline_16 & 0x00FF0000) >> 16);
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x01;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x02;
	cmdTbl[8] = (UINT8)(espline_16 & 0x000000F0);
	i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

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
	/* sensor reset (fmgpio 10) */
	gpioByteFuncSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteDirSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0xFF);
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[3*6];
	UINT16 s_gain;
	#if (SNAP_KEEP_FPS == 0)
	UINT16 compensation_line = 0;
	#endif
	CaptureHeaderInformation cap;
	
	/* Before Snap ExposureTime Set */
	frontBeforeSnapExposureTimeSet();
	
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
	
	espline = (UINT32)(pixclk * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]) / (UINT32)(ltotal * frontSnapLastShutter);// 2*2 binning mode on need used
	//printf("espline=%d\n",espline);

	if (frontPrevSensorZoom2xGainEn) {
		if ( frontPrevMode == 1 ) {  /* 30 fps. */
		}
		else if ( frontPrevMode == 0 ) {	/* 24fps */
		}
	}

	s_gain = frontSnapLastGain;

	if (espline != 0) {
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
	}
	else {
		espline = 1;
	}

	espline_16 = (UINT32)espline << 4;
	//printf("espline_16=%lu\n",espline_16);

	/* VTS DIFF set */
	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0c;
	cmdTbl[2] = (UINT8)((compensation_line & 0xFF00)>>8);

	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0d;
	cmdTbl[5] = (UINT8)(compensation_line & 0x00FF);

	/* exposure time set */
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = (UINT8)((espline_16 & 0x00FF0000) >> 16);
	
	cmdTbl[9]  = 0x35;
	cmdTbl[10] = 0x01;
	cmdTbl[11] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	
	cmdTbl[12] = 0x35;
	cmdTbl[13] = 0x02;
	cmdTbl[14] = (UINT8)(espline_16 & 0x000000F0);

	//printf("pre-s_gain=%d\n",s_gain);
	
	/* gain set */
	cmdTbl[15]  = 0x35;
	cmdTbl[16] = 0x0B;
	cmdTbl[17] = (UINT8)(s_gain & 0x00FF);

	//printf("re-s_gain=0x%bx\n",cmdTbl[11]);
	
	i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	/* add capture debug information to jpeg header */
	cap.snapPclk = pixclk;
	cap.espline = espline;
	cap.compensationLine = compensation_line;
	cap.lineTot = ltotal;
	cap.frameTot = ftotal;
	cap.gain = s_gain;
	aeCaptureHeaderAeInformationSet(HEADER_INFORMATION_CAPTURE_CONFIG,&cap);
	
	
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
	//printf("isoCfg=%bu,autoGainIdx=0x%bx,isoGainIdx=0x%bx,limitShutterFps=%bu\n",isoCfg,autoGainIdx,isoGainIdx,(UINT8)limitShutterFps);

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

	sts = i2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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
		//case _frontInit2CmdTbl:
		//	pData = frontInit2CmdTbl;
		//	size = sizeof(frontInit2CmdTbl);
		//	break;
		//case _frontInit3CmdTbl:
		//	pData = frontInit3CmdTbl;
		//	size = sizeof(frontInit3CmdTbl);
		//	break;
		//case _frontInit4CmdTbl:
		//	pData = frontInit4CmdTbl;
		//	size = sizeof(frontInit4CmdTbl);
		//	break;
		//case _front15fpsPrevCmdTbl:
		//	pData = front15fpsPrevCmdTbl;
		//	size = sizeof(front15fpsPrevCmdTbl);
		//	break;
		case _front20fpsPrevCmdTbl:
			pData = front20fpsPrevCmdTbl;
			size = sizeof(front20fpsPrevCmdTbl);
			break;
		//case _front24fpsPrevCmdTbl:
		//	pData = front24fpsPrevCmdTbl;
		//	size = sizeof(front24fpsPrevCmdTbl);
		//	break;
		case _front30fpsPrevCmdTbl:
			pData = front30fpsPrevCmdTbl;
			size = sizeof(front30fpsPrevCmdTbl);
			break;
		case _front60fpsPrevCmdTbl:
			pData = front60fpsPrevCmdTbl;
			size = sizeof(front60fpsPrevCmdTbl);
			break;
		case _frontSnapCmdTbl:
			pData = frontSnapCmdTbl;
			size = sizeof(frontSnapCmdTbl);
			break;
	}

	
	sts = i2cCmdTableSend(pData, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

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

