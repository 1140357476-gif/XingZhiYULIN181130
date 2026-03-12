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
#include "app_dev_disp.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		0	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */

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
#define XOFF_30FPS_PREV       3 // 1
#define YOFF_30FPS_PREV       1 // 2
#define LINE_TOT_30FPS_PREV   1630 // 2132
#define FRAME_TOT_30FPS_PREV  984 // 1018
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  (1.6)// 2 // 4
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
#define WIDTH_SNAP      2560
#define HEIGHT_SNAP     1920
#define XOFF_SNAP       1
#define YOFF_SNAP       1 // 3
#define LINE_TOT_SNAP   2700
#define FRAME_TOT_SNAP  1968




/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT // 522 MHZ
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    11	// 23.72727272727273 MHZ
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_50FPS_PREV    12
#define MCLK_PHASE_50FPS_PREV  0
#define PCLK_DIV_50FPS_PREV    12
#define PCLK_PHASE_50FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    11//9
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    11//9
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
#define PCLK_FREQ_30FPS_PREV     48260000L // 44850000L // 48000000L // 65250000L
#define PCLK_FREQ_30FPS_PREV_TD  44850000L
#define PCLK_FREQ_60FPS_PREV     50420000L
#define PCLK_FREQ_60FPS_PREV_TD  34660000L
#define PCLK_FREQ_50FPS_PREV  87000000L
#define PCLK_FREQ_50FPS_PREV_TD  87000000L
#define PCLK_FREQ_20FPS_PREV     25200000L
#define PCLK_FREQ_20FPS_PREV_TD  25200000L
#define PCLK_FREQ_SNAP           51000000L // 50420000L // 80600000L//44850000L // 65250000L
#define PCLK_FREQ_SNAP_BURST  	41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 31200000L	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

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
#define I2C_DEV_ADDR  			0x6C	//0x6D
#define I2C_REG_CLK_DIV      	I2C_SOURCE_DIV_10
#define I2C_CLK_DIV          	0 // I2C_REG_CLK_DIV_512

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  139
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  138
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

#define AE_50FPS_50_MAX_IDX  129
#define AE_50FPS_50_MIN_IDX  0
#define AE_50FPS_45_MAX_IDX  128
#define AE_50FPS_45_MIN_IDX  0
#define EV10_50FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  125
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  118
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   100

#define AE_20FPS_20_MAX_IDX  125
#define AE_20FPS_20_MIN_IDX  0
#define AE_20FPS_15_MAX_IDX  124
#define AE_20FPS_15_MIN_IDX  0
#define EV10_20FPS_EXP_IDX   100

/* gain. */
#define GAIN_30FPS_MAX_IDX  240 // 48
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_50FPS_MAX_IDX  112 // 48
#define GAIN_50FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  112 // 48
#define GAIN_60FPS_MIN_IDX  0

#define GAIN_20FPS_MAX_IDX  112 // 48
#define GAIN_20FPS_MIN_IDX  0

/* iso gain. */
#define ISO_100_GAIN_IDX	0
#define ISO_200_GAIN_IDX	16
#define ISO_400_GAIN_IDX	32
#define ISO_800_GAIN_IDX	48

/* manual iso capture limit fps */
#define CAPTURE_LIMIT_FPS	0

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

#if SUPPORT_SENSOR_SWITCH
extern UINT8 sensor_switch;
#endif


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV5647 RAW";

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
	_frontInit2CmdTbl		= 0x00CD,
	_frontInit3CmdTbl		= 0x00CE,
	_frontInit4CmdTbl		= 0x00CF,
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
static code frontAe_t ae20fps50hzTbl[1/*34*/] = {
	/* --------30fps-------------- */
	{	0,	333,	48,	0,	0 },
	/*{	1,	333,	46,	0,	0 },
	{	2,	333,	44,	0,	0 },
	{	3,	333,	42,	0,	0 },
	{	4,	333,	40,	0,	0 },
	{	5,	333,	39,	0,	0 },
	{	6,	333,	37,	0,	0 },
	{	7,	333,	36,	0,	0 },
	{	8,	333,	35,	0,	0 },
	{	9,	333,	33,	0,	0 },
	{	10,	333,	32,	0,	0 },
	{	11,	333,	30,	0,	0 },
	{	12,	333,	28,	0,	0 },
	{	13,	333,	26,	0,	0 },
	{	14,	333,	24,	0,	0 },
	{	15,	333,	23,	0,	0 },
	{	16,	333,	21,	0,	0 },
	{	17,	333,	20,	0,	0 },
	{	18,	333,	19,	0,	0 },
	{	19,	333,	17,	0,	0 },
	{	20,	333,	16,	0,	0 },
	{	21,	333,	14,	0,	0 },
	{	22,	333,	12,	0,	0 },
	{	23,	333,	10,	0,	0 },
	{	24,	333,	8,	0,	0 },
	{	25,	333,	7,	0,	0 },
	{	26,	333,	5,	0,	0 },
	{	27,	333,	4,	0,	0 },
	{	28,	333,	3,	0,	0 },
	{	29,	333,	1,	0,	0 },
	{	30,	333,	0,	0,	0 },
	{	31,	499,	7,	0,	0 },
	{	32,	499,	5,	0,	0 },
	{	33,	499,	4,	0,	0 },
	{	34,	499,	2,	0,	0 },
	{	35,	499,	1,	0,	0 },
	{	36,	499,	0,	0,	0 },
	{	37,	990,	13,	0,	0 },
	{	38,	990,	12,	0,	0 },
	{	39,	990,	10,	0,	0 },
	{	40,	990,	8,	0,	0 },
	{	41,	990,	6,	0,	0 },
	{	42,	990,	5,	0,	0 },
	{	43,	990,	4,	0,	0 },
	{	44,	990,	2,	0,	0 },
	{	45,	990,	1,	0,	0 },
	{	46,	990,	0,	0,	0 },
	{	47,	1032,	0,	0,	0 },
	{	48,	1110,	0,	0,	0 },
	{	49,	1187,	0,	0,	0 },
	{	50,	1277,	0,	0,	0 },
	{	51,	1363,	0,	0,	0 },
	{	52,	1463,	0,	0,	0 },
	{	53,	1566,	0,	0,	0 },
	{	54,	1686,	0,	0,	0 },
	{	55,	1796,	0,	0,	0 },
	{	56,	1937,	0,	0,	0 },
	{	57,	2064,	0,	0,	0 },
	{	58,	2231,	0,	0,	0 },
	{	59,	2375,	0,	0,	0 },
	{	60,	2568,	0,	0,	0 },
	{	61,	2727,	0,	0,	0 },
	{	62,	2945,	0,	0,	0 },
	{	63,	3155,	0,	0,	0 },
	{	64,	3398,	0,	0,	0 },
	{	65,	3620,	0,	0,	0 },
	{	66,	3875,	0,	0,	0 },
	{	67,	4167,	0,	0,	0 },
	{	68,	4507,	0,	0,	0 },
	{	69,	4801,	0,	0,	0 },
	{	70,	5136,	0,	0,	0 },
	{	71,	5521,	0,	0,	0 },
	{	72,	5969,	0,	0,	0 },
	{	73,	6310,	0,	0,	0 },
	{	74,	6901,	0,	0,	1 },
	{	75,	7362,	0,	0,	0 },
	{	76,	7887,	0,	0,	0 },
	{	77,	8494,	0,	0,	0 },
	{	78,	9202,	0,	0,	1 },
	{	79,	9602,	0,	0,	1 },
	{	80,	10517,	0,	0,	0 },
	{	81,	11042,	0,	0,	0 },
	{	82,	12269,	0,	0,	0 },
	{	83,	12991,	0,	0,	0 },
	{	84,	13803,	0,	0,	1 },
	{	85,	14723,	0,	0,	1 },
	{	86,	15775,	0,	0,	1 },
	{	87,	16988,	0,	0,	1 },
	{	88,	18404,	0,	0,	1 },
	{	89,	20077,	0,	0,	2 },
	{	90,	22085,	1,	0,	0 },
	{	91,	22085,	0,	0,	0 },
	{	92,	24538,	0,	0,	0 },
	{	93,	27606,	1,	0,	1 },
	{	94,	27606,	0,	0,	1 },
	{	95,	31549,	1,	0,	1 },
	{	96,	31549,	0,	0,	1 },
	{	97,	36807,	1,	0,	1 },
	{	98,	36807,	0,	0,	1 },
	{	99,	44169,	2,	0,	2 },
	{	100,	44169,	1,	0,	2 },
	{	101,	44169,	0,	0,	2 },
	{	102,	55211,	2,	0,	4 },
	{	103,	55211,	1,	0,	4 },
	{	104,	55211,	0,	0,	3 },
	{	105,	73614,	4,	0,	7 },
	{	106,	73614,	3,	0,	7 },
	{	107,	73614,	1,	0,	6 },
	{	108,	73614,	0,	0,	6 },
	{	109,	110419,	6,	0,	18 },
	{	110,	110419,	5,	0,	17 },
	{	111,	110419,	4,	0,	16 },
	{	112,	110419,	2,	0,	15 },
	{	113,	110419,	1,	0,	14 },
	{	114,	110419,	0,	0,	13 },
	{	115,	220826,	13,	0,	0 },
	{	116,	220826,	12,	0,	0 },
	{	117,	220826,	10,	0,	0 },
	{	118,	220826,	8,	0,	0 },
	{	119,	220826,	6,	0,	0 },
	{	120,	220826,	5,	0,	0 },
	{	121,	220826,	4,	0,	0 },
	{	122,	220826,	2,	0,	0 },
	{	123,	220826,	1,	0,	0 },
	{	124,	220826,	0,	0,	0 },*/
};

static code frontAe_t ae20fps60hzTbl[1/*26*/] = {
	/* --------30fps-------------- */
	{	0,	299,	47,	0,	0 },
	/*{	1,	299,	45,	0,	0 },
	{	2,	299,	43,	0,	0 },
	{	3,	299,	41,	0,	0 },
	{	4,	299,	40,	0,	0 },
	{	5,	299,	38,	0,	0 },
	{	6,	299,	37,	0,	0 },
	{	7,	299,	35,	0,	0 },
	{	8,	299,	34,	0,	0 },
	{	9,	299,	33,	0,	0 },
	{	10,	299,	31,	0,	0 },
	{	11,	299,	29,	0,	0 },
	{	12,	299,	27,	0,	0 },
	{	13,	299,	25,	0,	0 },
	{	14,	299,	24,	0,	0 },
	{	15,	299,	22,	0,	0 },
	{	16,	299,	21,	0,	0 },
	{	17,	299,	19,	0,	0 },
	{	18,	299,	18,	0,	0 },
	{	19,	299,	17,	0,	0 },
	{	20,	299,	15,	0,	0 },
	{	21,	299,	13,	0,	0 },
	{	22,	299,	11,	0,	0 },
	{	23,	299,	9,	0,	0 },
	{	24,	299,	8,	0,	0 },
	{	25,	299,	6,	0,	0 },
	{	26,	299,	5,	0,	0 },
	{	27,	299,	3,	0,	0 },
	{	28,	299,	2,	0,	0 },
	{	29,	299,	1,	0,	0 },
	{	30,	299,	0,	0,	0 },
	{	31,	399,	3,	0,	0 },
	{	32,	399,	2,	0,	0 },
	{	33,	399,	1,	0,	0 },
	{	34,	399,	0,	0,	0 },
	{	35,	599,	6,	0,	0 },
	{	36,	599,	5,	0,	0 },
	{	37,	599,	3,	0,	0 },
	{	38,	599,	2,	0,	0 },
	{	39,	599,	1,	0,	0 },
	{	40,	599,	0,	0,	0 },
	{	41,	1194,	13,	0,	0 },
	{	42,	1194,	11,	0,	0 },
	{	43,	1194,	9,	0,	0 },
	{	44,	1194,	8,	0,	0 },
	{	45,	1194,	6,	0,	0 },
	{	46,	1194,	5,	0,	0 },
	{	47,	1194,	3,	0,	0 },
	{	48,	1194,	2,	0,	0 },
	{	49,	1194,	1,	0,	0 },
	{	50,	1194,	0,	0,	0 },
	{	51,	1277,	0,	0,	0 },
	{	52,	1363,	0,	0,	0 },
	{	53,	1463,	0,	0,	0 },
	{	54,	1566,	0,	0,	0 },
	{	55,	1686,	0,	0,	0 },
	{	56,	1796,	0,	0,	0 },
	{	57,	1937,	0,	0,	0 },
	{	58,	2064,	0,	0,	0 },
	{	59,	2231,	0,	0,	0 },
	{	60,	2375,	0,	0,	0 },
	{	61,	2568,	0,	0,	0 },
	{	62,	2727,	0,	0,	0 },
	{	63,	2945,	0,	0,	0 },
	{	64,	3155,	0,	0,	0 },
	{	65,	3398,	0,	0,	0 },
	{	66,	3620,	0,	0,	0 },
	{	67,	3875,	0,	0,	0 },
	{	68,	4167,	0,	0,	0 },
	{	69,	4507,	0,	0,	0 },
	{	70,	4801,	0,	0,	0 },
	{	71,	5136,	0,	0,	0 },
	{	72,	5521,	0,	0,	0 },
	{	73,	5969,	0,	0,	0 },
	{	74,	6310,	0,	0,	0 },
	{	75,	6901,	0,	0,	1 },
	{	76,	7362,	0,	0,	0 },
	{	77,	7887,	0,	0,	0 },
	{	78,	8494,	0,	0,	0 },
	{	79,	9202,	0,	0,	1 },
	{	80,	9602,	0,	0,	1 },
	{	81,	10517,	0,	0,	0 },
	{	82,	11042,	0,	0,	0 },
	{	83,	12269,	0,	0,	0 },
	{	84,	12991,	0,	0,	0 },
	{	85,	13803,	0,	0,	1 },
	{	86,	14723,	0,	0,	1 },
	{	87,	15775,	0,	0,	1 },
	{	88,	16988,	0,	0,	1 },
	{	89,	18404,	0,	0,	1 },
	{	90,	20077,	0,	0,	2 },
	{	91,	22085,	1,	0,	0 },
	{	92,	22085,	0,	0,	0 },
	{	93,	24538,	0,	0,	0 },
	{	94,	27606,	1,	0,	1 },
	{	95,	27606,	0,	0,	1 },
	{	96,	31549,	1,	0,	1 },
	{	97,	31549,	0,	0,	1 },
	{	98,	36807,	1,	0,	1 },
	{	99,	36807,	0,	0,	1 },
	{	100,	44169,	2,	0,	2 },
	{	101,	44169,	1,	0,	2 },
	{	102,	44169,	0,	0,	2 },
	{	103,	55211,	2,	0,	4 },
	{	104,	55211,	1,	0,	4 },
	{	105,	55211,	0,	0,	3 },
	{	106,	73614,	4,	0,	7 },
	{	107,	73614,	3,	0,	7 },
	{	108,	73614,	1,	0,	6 },
	{	109,	73614,	0,	0,	6 },
	{	110,	110419,	6,	0,	18 },
	{	111,	110419,	5,	0,	17 },
	{	112,	110419,	4,	0,	16 },
	{	113,	110419,	2,	0,	15 },
	{	114,	110419,	1,	0,	14 },
	{	115,	110419,	0,	0,	13 },
	{	116,	220826,	13,	0,	0 },
	{	117,	220826,	12,	0,	0 },
	{	118,	220826,	10,	0,	0 },
	{	119,	220826,	8,	0,	0 },
	{	120,	220826,	6,	0,	0 },
	{	121,	220826,	5,	0,	0 },
	{	122,	220826,	4,	0,	0 },
	{	123,	220826,	2,	0,	0 },
	{	124,	220826,	1,	0,	0 },
	{	125,	220826,	0,	0,	0 },*/
};

static code frontAe_t ae30fps50hzTbl[139] = {
	{	0,	332,	233,	0,	0 },
	{	1,	332,	216,	0,	0 },
	{	2,	332,	201,	0,	0 },
	{	3,	332,	186,	0,	0 },
	{	4,	332,	173,	0,	0 },
	{	5,	332,	160,	0,	0 },
	{	6,	332,	148,	0,	0 },
	{	7,	332,	137,	0,	0 },
	{	8,	332,	127,	0,	0 },
	{	9,	332,	118,	0,	0 },
	{	10,	332,	109,	0,	0 },
	{	11,	332,	100,	0,	0 },
	{	12,	332,	93,	0,	0 },
	{	13,	332,	85,	0,	0 },
	{	14,	332,	79,	0,	0 },
	{	15,	332,	72,	0,	0 },
	{	16,	332,	66,	0,	0 },
	{	17,	332,	61,	0,	0 },
	{	18,	332,	56,	0,	0 },
	{	19,	332,	51,	0,	0 },
	{	20,	332,	47,	0,	0 },
	{	21,	332,	42,	0,	0 },
	{	22,	332,	39,	0,	0 },
	{	23,	332,	35,	0,	0 },
	{	24,	332,	32,	0,	0 },
	{	25,	332,	28,	0,	0 },
	{	26,	332,	25,	0,	0 },
	{	27,	332,	23,	0,	0 },
	{	28,	332,	20,	0,	0 },
	{	29,	332,	18,	0,	0 },
	{	30,	332,	16,	0,	0 },
	{	31,	332,	13,	0,	0 },
	{	32,	332,	12,	0,	0 },
	{	33,	332,	10,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	6,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	4,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	497,	6,	0,	0 },
	{	42,	497,	5,	0,	0 },
	{	43,	497,	3,	0,	0 },
	{	44,	497,	2,	0,	0 },
	{	45,	497,	1,	0,	0 },
	{	46,	497,	0,	0,	0 },
	{	47,	990,	13,	0,	0 },
	{	48,	990,	11,	0,	0 },
	{	49,	990,	9,	0,	0 },
	{	50,	990,	8,	0,	0 },
	{	51,	990,	6,	0,	0 },
	{	52,	990,	5,	0,	0 },
	{	53,	990,	3,	0,	0 },
	{	54,	990,	2,	0,	0 },
	{	55,	990,	1,	0,	0 },
	{	56,	990,	0,	0,	0 },
	{	57,	1049,	0,	0,	0 },
	{	58,	1125,	0,	0,	0 },
	{	59,	1203,	0,	0,	0 },
	{	60,	1292,	0,	0,	0 },
	{	61,	1383,	0,	0,	0 },
	{	62,	1487,	0,	0,	0 },
	{	63,	1591,	0,	0,	0 },
	{	64,	1711,	0,	0,	0 },
	{	65,	1827,	0,	0,	0 },
	{	66,	1960,	0,	0,	0 },
	{	67,	2099,	0,	0,	0 },
	{	68,	2260,	0,	0,	0 },
	{	69,	2407,	0,	0,	0 },
	{	70,	2597,	0,	0,	0 },
	{	71,	2767,	0,	0,	0 },
	{	72,	2990,	0,	0,	0 },
	{	73,	3183,	0,	0,	0 },
	{	74,	3442,	0,	0,	0 },
	{	75,	3655,	0,	0,	0 },
	{	76,	3947,	0,	0,	0 },
	{	77,	4229,	0,	0,	0 },
	{	78,	4554,	0,	0,	0 },
	{	79,	4853,	0,	0,	0 },
	{	80,	5194,	0,	0,	0 },
	{	81,	5586,	0,	0,	0 },
	{	82,	6042,	0,	0,	0 },
	{	83,	6436,	0,	0,	0 },
	{	84,	6885,	0,	0,	0 },
	{	85,	7401,	0,	0,	0 },
	{	86,	8001,	0,	0,	0 },
	{	87,	8459,	0,	0,	0 },
	{	88,	9252,	0,	0,	0 },
	{	89,	9869,	0,	0,	0 },
	{	90,	10574,	0,	0,	0 },
	{	91,	11387,	0,	0,	0 },
	{	92,	12336,	0,	0,	0 },
	{	93,	12872,	0,	0,	0 },
	{	94,	14098,	0,	0,	0 },
	{	95,	14803,	0,	0,	0 },
	{	96,	16448,	0,	0,	0 },
	{	97,	17415,	0,	0,	0 },
	{	98,	18504,	0,	0,	0 },
	{	99,	19738,	0,	0,	0 },
	{	100,	21147,	0,	0,	0 },
	{	101,	22774,	0,	0,	0 },
	{	102,	24672,	0,	0,	0 },
	{	103,	26915,	0,	0,	0 },
	{	104,	29607,	1,	0,	0 },
	{	105,	29607,	0,	0,	0 },
	{	106,	32896,	0,	0,	1 },
	{	107,	37008,	1,	0,	0 },
	{	108,	37008,	0,	0,	0 },
	{	109,	42295,	1,	0,	1 },
	{	110,	42295,	0,	0,	1 },
	{	111,	49344,	1,	0,	0 },
	{	112,	49344,	0,	0,	0 },
	{	113,	59213,	2,	0,	1 },
	{	114,	59213,	1,	0,	1 },
	{	115,	59213,	0,	0,	1 },
	{	116,	74016,	2,	0,	0 },
	{	117,	74016,	1,	0,	0 },
	{	118,	74016,	0,	0,	0 },
	{	119,	98687,	4,	0,	2 },
	{	120,	98687,	3,	0,	2 },
	{	121,	98687,	1,	0,	2 },
	{	122,	98687,	0,	0,	1 },
	{	123,	148029,	6,	0,	0 },
	{	124,	148029,	5,	0,	0 },
	{	125,	148029,	4,	0,	0 },
	{	126,	148029,	2,	0,	0 },
	{	127,	148029,	1,	0,	0 },
	{	128,	148029,	0,	0,	0 },
	{	129,	296044,	13,	0,	8 },
	{	130,	296044,	12,	0,	8 },
	{	131,	296044,	10,	0,	7 },
	{	132,	296044,	8,	0,	7 },
	{	133,	296044,	6,	0,	6 },
	{	134,	296044,	5,	0,	6 },
	{	135,	296044,	4,	0,	6 },
	{	136,	296044,	2,	0,	5 },
	{	137,	296044,	1,	0,	5 },
	{	138,	296044,	0,	0,	4 },
	};


static code frontAe_t ae30fps60hzTbl[140] = {
	{	0,	299,	224,	0,	0 },
	{	1,	299,	208,	0,	0 },
	{	2,	299,	193,	0,	0 },
	{	3,	299,	179,	0,	0 },
	{	4,	299,	166,	0,	0 },
	{	5,	299,	154,	0,	0 },
	{	6,	299,	143,	0,	0 },
	{	7,	299,	132,	0,	0 },
	{	8,	299,	122,	0,	0 },
	{	9,	299,	113,	0,	0 },
	{	10,	299,	104,	0,	0 },
	{	11,	299,	96,	0,	0 },
	{	12,	299,	89,	0,	0 },
	{	13,	299,	82,	0,	0 },
	{	14,	299,	75,	0,	0 },
	{	15,	299,	69,	0,	0 },
	{	16,	299,	64,	0,	0 },
	{	17,	299,	58,	0,	0 },
	{	18,	299,	53,	0,	0 },
	{	19,	299,	49,	0,	0 },
	{	20,	299,	44,	0,	0 },
	{	21,	299,	40,	0,	0 },
	{	22,	299,	37,	0,	0 },
	{	23,	299,	33,	0,	0 },
	{	24,	299,	30,	0,	0 },
	{	25,	299,	27,	0,	0 },
	{	26,	299,	24,	0,	0 },
	{	27,	299,	21,	0,	0 },
	{	28,	299,	19,	0,	0 },
	{	29,	299,	17,	0,	0 },
	{	30,	299,	14,	0,	0 },
	{	31,	299,	12,	0,	0 },
	{	32,	299,	11,	0,	0 },
	{	33,	299,	9,	0,	0 },
	{	34,	299,	7,	0,	0 },
	{	35,	299,	6,	0,	0 },
	{	36,	299,	4,	0,	0 },
	{	37,	299,	3,	0,	0 },
	{	38,	299,	2,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	399,	4,	0,	0 },
	{	41,	399,	3,	0,	0 },
	{	42,	399,	2,	0,	0 },
	{	43,	399,	1,	0,	0 },
	{	44,	399,	0,	0,	0 },
	{	45,	598,	6,	0,	0 },
	{	46,	598,	4,	0,	0 },
	{	47,	598,	3,	0,	0 },
	{	48,	598,	2,	0,	0 },
	{	49,	598,	1,	0,	0 },
	{	50,	601,	0,	0,	0 },
	{	51,	1189,	12,	0,	0 },
	{	52,	1189,	10,	0,	0 },
	{	53,	1189,	9,	0,	0 },
	{	54,	1189,	7,	0,	0 },
	{	55,	1189,	6,	0,	0 },
	{	56,	1189,	4,	0,	0 },
	{	57,	1189,	3,	0,	0 },
	{	58,	1189,	2,	0,	0 },
	{	59,	1189,	0,	0,	0 },
	{	60,	1203,	0,	0,	0 },
	{	61,	1292,	0,	0,	0 },
	{	62,	1383,	0,	0,	0 },
	{	63,	1487,	0,	0,	0 },
	{	64,	1591,	0,	0,	0 },
	{	65,	1711,	0,	0,	0 },
	{	66,	1827,	0,	0,	0 },
	{	67,	1960,	0,	0,	0 },
	{	68,	2099,	0,	0,	0 },
	{	69,	2260,	0,	0,	0 },
	{	70,	2407,	0,	0,	0 },
	{	71,	2597,	0,	0,	0 },
	{	72,	2767,	0,	0,	0 },
	{	73,	2990,	0,	0,	0 },
	{	74,	3183,	0,	0,	0 },
	{	75,	3442,	0,	0,	0 },
	{	76,	3655,	0,	0,	0 },
	{	77,	3947,	0,	0,	0 },
	{	78,	4229,	0,	0,	0 },
	{	79,	4554,	0,	0,	0 },
	{	80,	4853,	0,	0,	0 },
	{	81,	5194,	0,	0,	0 },
	{	82,	5586,	0,	0,	0 },
	{	83,	6042,	0,	0,	0 },
	{	84,	6436,	0,	0,	0 },
	{	85,	6885,	0,	0,	0 },
	{	86,	7401,	0,	0,	0 },
	{	87,	8001,	0,	0,	0 },
	{	88,	8459,	0,	0,	0 },
	{	89,	9252,	0,	0,	0 },
	{	90,	9869,	0,	0,	0 },
	{	91,	10574,	0,	0,	0 },
	{	92,	11387,	0,	0,	0 },
	{	93,	12336,	0,	0,	0 },
	{	94,	12872,	0,	0,	0 },
	{	95,	14098,	0,	0,	0 },
	{	96,	14803,	0,	0,	0 },
	{	97,	16448,	0,	0,	0 },
	{	98,	17415,	0,	0,	0 },
	{	99,	18504,	0,	0,	0 },
	{	100,	19738,	0,	0,	0 },
	{	101,	21147,	0,	0,	0 },
	{	102,	22774,	0,	0,	0 },
	{	103,	24672,	0,	0,	0 },
	{	104,	26915,	0,	0,	0 },
	{	105,	29607,	1,	0,	0 },
	{	106,	29607,	0,	0,	0 },
	{	107,	32896,	0,	0,	1 },
	{	108,	37008,	1,	0,	0 },
	{	109,	37008,	0,	0,	0 },
	{	110,	42295,	1,	0,	1 },
	{	111,	42295,	0,	0,	1 },
	{	112,	49344,	1,	0,	0 },
	{	113,	49344,	0,	0,	0 },
	{	114,	59213,	2,	0,	1 },
	{	115,	59213,	1,	0,	1 },
	{	116,	59213,	0,	0,	1 },
	{	117,	74016,	2,	0,	0 },
	{	118,	74016,	1,	0,	0 },
	{	119,	74016,	0,	0,	0 },
	{	120,	98687,	4,	0,	2 },
	{	121,	98687,	3,	0,	2 },
	{	122,	98687,	1,	0,	2 },
	{	123,	98687,	0,	0,	1 },
	{	124,	148029,	6,	0,	0 },
	{	125,	148029,	5,	0,	0 },
	{	126,	148029,	4,	0,	0 },
	{	127,	148029,	2,	0,	0 },
	{	128,	148029,	1,	0,	0 },
	{	129,	148029,	0,	0,	0 },
	{	130,	296044,	13,	0,	8 },
	{	131,	296044,	12,	0,	8 },
	{	132,	296044,	10,	0,	7 },
	{	133,	296044,	8,	0,	7 },
	{	134,	296044,	6,	0,	6 },
	{	135,	296044,	5,	0,	6 },
	{	136,	296044,	4,	0,	6 },
	{	137,	296044,	2,	0,	5 },
	{	138,	296044,	1,	0,	5 },
	{	139,	296044,	0,	0,	4 },
	};


static code frontAe_t ae60fps50hzTbl[119] = {
	/* --------60fps-------------- */
	{	0,	991,	47,	0,	0 },
	{	1,	991,	45,	0,	0 },
	{	2,	991,	43,	0,	0 },
	{	3,	991,	42,	0,	0 },
	{	4,	991,	40,	0,	0 },
	{	5,	991,	38,	0,	0 },
	{	6,	991,	37,	0,	0 },
	{	7,	991,	35,	0,	0 },
	{	8,	991,	34,	0,	0 },
	{	9,	991,	33,	0,	0 },
	{	10,	991,	31,	0,	0 },
	{	11,	991,	29,	0,	0 },
	{	12,	991,	27,	0,	0 },
	{	13,	991,	26,	0,	0 },
	{	14,	991,	24,	0,	0 },
	{	15,	991,	22,	0,	0 },
	{	16,	991,	21,	0,	0 },
	{	17,	991,	19,	0,	0 },
	{	18,	991,	18,	0,	0 },
	{	19,	991,	17,	0,	0 },
	{	20,	991,	15,	0,	0 },
	{	21,	991,	13,	0,	0 },
	{	22,	991,	11,	0,	0 },
	{	23,	991,	10,	0,	0 },
	{	24,	991,	8,	0,	0 },
	{	25,	991,	6,	0,	0 },
	{	26,	991,	5,	0,	0 },
	{	27,	991,	3,	0,	0 },
	{	28,	991,	2,	0,	0 },
	{	29,	991,	1,	0,	0 },
	{	30,	991,	0,	0,	0 },
	{	31,	1040,	0,	0,	0 },
	{	32,	1116,	0,	0,	0 },
	{	33,	1197,	0,	0,	0 },
	{	34,	1283,	0,	0,	0 },
	{	35,	1374,	0,	0,	0 },
	{	36,	1474,	0,	0,	0 },
	{	37,	1579,	0,	0,	0 },
	{	38,	1693,	0,	0,	0 },
	{	39,	1810,	0,	0,	0 },
	{	40,	1945,	0,	0,	0 },
	{	41,	2081,	0,	0,	0 },
	{	42,	2238,	0,	0,	0 },
	{	43,	2394,	0,	0,	0 },
	{	44,	2574,	0,	0,	0 },
	{	45,	2749,	0,	0,	0 },
	{	46,	2949,	0,	0,	0 },
	{	47,	3158,	0,	0,	0 },
	{	48,	3400,	0,	0,	0 },
	{	49,	3621,	0,	0,	0 },
	{	50,	3907,	0,	0,	0 },
	{	51,	4162,	0,	0,	0 },
	{	52,	4499,	0,	0,	0 },
	{	53,	4789,	0,	0,	0 },
	{	54,	5179,	0,	0,	0 },
	{	55,	5498,	0,	0,	0 },
	{	56,	5938,	0,	0,	0 },
	{	57,	6362,	0,	0,	0 },
	{	58,	6852,	0,	0,	0 },
	{	59,	7301,	0,	0,	0 },
	{	60,	7814,	0,	0,	0 },
	{	61,	8403,	0,	0,	0 },
	{	62,	9089,	0,	0,	0 },
	{	63,	9682,	0,	0,	0 },
	{	64,	10358,	0,	0,	0 },
	{	65,	11135,	0,	0,	0 },
	{	66,	12037,	0,	0,	0 },
	{	67,	12725,	0,	0,	0 },
	{	68,	13918,	0,	0,	0 },
	{	69,	14846,	0,	0,	0 },
	{	70,	15907,	0,	0,	0 },
	{	71,	17130,	0,	0,	0 },
	{	72,	18558,	0,	0,	0 },
	{	73,	19365,	0,	0,	0 },
	{	74,	21209,	0,	0,	0 },
	{	75,	22270,	0,	0,	0 },
	{	76,	24744,	0,	0,	0 },
	{	77,	26200,	0,	0,	0 },
	{	78,	27837,	0,	0,	0 },
	{	79,	29693,	0,	0,	0 },
	{	80,	31814,	0,	0,	0 },
	{	81,	34261,	0,	0,	0 },
	{	82,	37116,	0,	0,	0 },
	{	83,	40491,	0,	0,	1 },
	{	84,	44540,	1,	0,	0 },
	{	85,	44540,	0,	0,	0 },
	{	86,	49489,	0,	0,	0 },
	{	87,	55675,	1,	0,	1 },
	{	88,	55675,	0,	0,	0 },
	{	89,	63628,	1,	0,	1 },
	{	90,	63628,	0,	0,	1 },
	{	91,	74233,	1,	0,	1 },
	{	92,	74233,	0,	0,	1 },
	{	93,	89079,	2,	0,	0 },
	{	94,	89079,	1,	0,	0 },
	{	95,	89079,	0,	0,	0 },
	{	96,	111348,	2,	0,	1 },
	{	97,	111348,	1,	0,	1 },
	{	98,	111348,	0,	0,	0 },
	{	99,	148463,	4,	0,	2 },
	{	100,	148463,	3,	0,	2 },
	{	101,	148463,	1,	0,	1 },
	{	102,	148463,	0,	0,	1 },
	{	103,	222692,	6,	0,	4 },
	{	104,	222692,	5,	0,	4 },
	{	105,	222692,	4,	0,	4 },
	{	106,	222692,	2,	0,	4 },
	{	107,	222692,	1,	0,	3 },
	{	108,	222692,	0,	0,	3 },
	{	109,	445361,	13,	0,	23 },
	{	110,	445361,	12,	0,	22 },
	{	111,	445361,	10,	0,	21 },
	{	112,	445361,	8,	0,	19 },
	{	113,	445361,	6,	0,	18 },
	{	114,	445361,	5,	0,	17 },
	{	115,	445361,	4,	0,	16 },
	{	116,	445361,	2,	0,	14 },
	{	117,	445361,	1,	0,	14 },
	{	118,	445361,	0,	0,	13 },
};

static code frontAe_t ae60fps60hzTbl[126] = {
	/* --------60fps-------------- */
	{	0,	597,	47,	0,	0 },
	{	1,	597,	45,	0,	0 },
	{	2,	597,	43,	0,	0 },
	{	3,	597,	41,	0,	0 },
	{	4,	597,	39,	0,	0 },
	{	5,	597,	38,	0,	0 },
	{	6,	597,	36,	0,	0 },
	{	7,	597,	35,	0,	0 },
	{	8,	597,	34,	0,	0 },
	{	9,	597,	33,	0,	0 },
	{	10,	597,	31,	0,	0 },
	{	11,	597,	29,	0,	0 },
	{	12,	597,	27,	0,	0 },
	{	13,	597,	25,	0,	0 },
	{	14,	597,	23,	0,	0 },
	{	15,	597,	22,	0,	0 },
	{	16,	597,	20,	0,	0 },
	{	17,	597,	19,	0,	0 },
	{	18,	597,	18,	0,	0 },
	{	19,	597,	17,	0,	0 },
	{	20,	597,	15,	0,	0 },
	{	21,	597,	13,	0,	0 },
	{	22,	597,	11,	0,	0 },
	{	23,	597,	9,	0,	0 },
	{	24,	597,	7,	0,	0 },
	{	25,	597,	6,	0,	0 },
	{	26,	597,	4,	0,	0 },
	{	27,	597,	3,	0,	0 },
	{	28,	597,	2,	0,	0 },
	{	29,	597,	1,	0,	0 },
	{	30,	597,	0,	0,	0 },
	{	31,	1190,	13,	0,	0 },
	{	32,	1190,	11,	0,	0 },
	{	33,	1190,	9,	0,	0 },
	{	34,	1190,	7,	0,	0 },
	{	35,	1190,	6,	0,	0 },
	{	36,	1190,	4,	0,	0 },
	{	37,	1190,	3,	0,	0 },
	{	38,	1190,	2,	0,	0 },
	{	39,	1190,	1,	0,	0 },
	{	40,	1197,	0,	0,	0 },
	{	41,	1283,	0,	0,	0 },
	{	42,	1374,	0,	0,	0 },
	{	43,	1474,	0,	0,	0 },
	{	44,	1579,	0,	0,	0 },
	{	45,	1693,	0,	0,	0 },
	{	46,	1810,	0,	0,	0 },
	{	47,	1945,	0,	0,	0 },
	{	48,	2081,	0,	0,	0 },
	{	49,	2238,	0,	0,	0 },
	{	50,	2394,	0,	0,	0 },
	{	51,	2574,	0,	0,	0 },
	{	52,	2749,	0,	0,	0 },
	{	53,	2949,	0,	0,	0 },
	{	54,	3158,	0,	0,	0 },
	{	55,	3400,	0,	0,	0 },
	{	56,	3621,	0,	0,	0 },
	{	57,	3907,	0,	0,	0 },
	{	58,	4162,	0,	0,	0 },
	{	59,	4499,	0,	0,	0 },
	{	60,	4789,	0,	0,	0 },
	{	61,	5179,	0,	0,	0 },
	{	62,	5498,	0,	0,	0 },
	{	63,	5938,	0,	0,	0 },
	{	64,	6362,	0,	0,	0 },
	{	65,	6852,	0,	0,	0 },
	{	66,	7301,	0,	0,	0 },
	{	67,	7814,	0,	0,	0 },
	{	68,	8403,	0,	0,	0 },
	{	69,	9089,	0,	0,	0 },
	{	70,	9682,	0,	0,	0 },
	{	71,	10358,	0,	0,	0 },
	{	72,	11135,	0,	0,	0 },
	{	73,	12037,	0,	0,	0 },
	{	74,	12725,	0,	0,	0 },
	{	75,	13918,	0,	0,	0 },
	{	76,	14846,	0,	0,	0 },
	{	77,	15907,	0,	0,	0 },
	{	78,	17130,	0,	0,	0 },
	{	79,	18558,	0,	0,	0 },
	{	80,	19365,	0,	0,	0 },
	{	81,	21209,	0,	0,	0 },
	{	82,	22270,	0,	0,	0 },
	{	83,	24744,	0,	0,	0 },
	{	84,	26200,	0,	0,	0 },
	{	85,	27837,	0,	0,	0 },
	{	86,	29693,	0,	0,	0 },
	{	87,	31814,	0,	0,	0 },
	{	88,	34261,	0,	0,	0 },
	{	89,	37116,	0,	0,	0 },
	{	90,	40491,	0,	0,	1 },
	{	91,	44540,	1,	0,	0 },
	{	92,	44540,	0,	0,	0 },
	{	93,	49489,	0,	0,	0 },
	{	94,	55675,	1,	0,	1 },
	{	95,	55675,	0,	0,	0 },
	{	96,	63628,	1,	0,	1 },
	{	97,	63628,	0,	0,	1 },
	{	98,	74233,	1,	0,	1 },
	{	99,	74233,	0,	0,	1 },
	{	100,	89079,	2,	0,	0 },
	{	101,	89079,	1,	0,	0 },
	{	102,	89079,	0,	0,	0 },
	{	103,	111348,	2,	0,	1 },
	{	104,	111348,	1,	0,	1 },
	{	105,	111348,	0,	0,	0 },
	{	106,	148463,	4,	0,	2 },
	{	107,	148463,	3,	0,	2 },
	{	108,	148463,	1,	0,	1 },
	{	109,	148463,	0,	0,	1 },
	{	110,	222692,	6,	0,	4 },
	{	111,	222692,	5,	0,	4 },
	{	112,	222692,	4,	0,	4 },
	{	113,	222692,	2,	0,	4 },
	{	114,	222692,	1,	0,	3 },
	{	115,	222692,	0,	0,	3 },
	{	116,	445361,	13,	0,	23 },
	{	117,	445361,	12,	0,	22 },
	{	118,	445361,	10,	0,	21 },
	{	119,	445361,	8,	0,	19 },
	{	120,	445361,	6,	0,	18 },
	{	121,	445361,	5,	0,	17 },
	{	122,	445361,	4,	0,	16 },
	{	123,	445361,	2,	0,	14 },
	{	124,	445361,	1,	0,	14 },
	{	125,	445361,	0,	0,	13 },


};

/* gain table. */
code UINT16 gainTbl[241] = {
   16,    17,    18,    19,    20,    21,    22,    23,    24,    25, 
   26,    27,    28,    29,    30,    31,    32,    33,    34,    35, 
   36,    37,    38,    39,    40,    41,    42,    43,    44,    45, 
   46,    47,    48,    49,    50,    51,    52,    53,    54,    55, 
   56,    57,    58,    59,    60,    61,    62,    63,    64,    65, 
   66,    67,    68,    69,    70,    71,    72,    73,    74,    75, 
   76,    77,    78,    79,    80,    81,    82,    83,    84,    85, 
   86,    87,    88,    89,    90,    91,    92,    93,    94,    95, 
   96,    97,    98,    99,   100,   101,   102,   103,   104,   105, 
  106,   107,   108,   109,   110,   111,   112,   113,   114,   115, 
  116,   117,   118,   119,   120,   121,   122,   123,   124,   125, 
  126,   127,   128,   129,   130,   131,   132,   133,   134,   135, 
  136,   137,   138,   139,   140,   141,   142,   143,   144,   145, 
  146,   147,   148,   149,   150,   151,   152,   153,   154,   155, 
  156,   157,   158,   159,   160,   161,   162,   163,   164,   165, 
  166,   167,   168,   169,   170,   171,   172,   173,   174,   175, 
  176,   177,   178,   179,   180,   181,   182,   183,   184,   185, 
  186,   187,   188,   189,   190,   191,   192,   193,   194,   195, 
  196,   197,   198,   199,   200,   201,   202,   203,   204,   205, 
  206,   207,   208,   209,   210,   211,   212,   213,   214,   215, 
  216,   217,   218,   219,   220,   221,   222,   223,   224,   225, 
  226,   227,   228,   229,   230,   231,   232,   233,   234,   235, 
  236,   237,   238,   239,   240,   241,   242,   243,   244,   245, 
  246,   247,   248,   249,   250,   251,   252,   253,   254,   255, 
  256, 	};


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
	;========	 OV5647 registers setting	=============
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
	0x01,0x00,0x00,                   
	0x01,0x03,0x01,                  
};

static code UINT8 frontInit2CmdTbl[] = {                 
 	0x01,0x00,0x00,                   
 	0x01,0x00,0x00,                   
 	0x01,0x00,0x00,                   
 	0x01,0x00,0x00,                   
	
	0x30, 0x11, 0x42,
	0x30, 0x13, 0x08,		//0x04-->0x00-->0x08  Turn off internal LDO
	0x47, 0x08, 0x00,
	0x50, 0x00, 0x06,
	0x50, 0x03, 0x08,
	0x5a, 0x00, 0x08,
	0x30, 0x00, 0xff,
	0x30, 0x01, 0xff,
	0x30, 0x02, 0xff,
	0x3a, 0x18, 0x01,
	0x3a, 0x19, 0xe0,
	0x3c, 0x01, 0x80,
	0x3b, 0x07, 0x0c,
	0x36, 0x30, 0x2e,
	0x36, 0x32, 0xe2,
	0x36, 0x33, 0x23,
	0x36, 0x34, 0x44,
	0x36, 0x20, 0x64,
	0x36, 0x21, 0xe0,
	0x36, 0x00, 0x37,
	0x37, 0x04, 0xa0,
	0x37, 0x03, 0x5a,
	0x37, 0x15, 0x78,
	0x37, 0x17, 0x01,
	0x37, 0x31, 0x02,
	0x37, 0x0b, 0x60,
	0x37, 0x05, 0x1a,
	0x3f, 0x05, 0x02,
	0x3f, 0x06, 0x10,
	0x3f, 0x01, 0x0a,
	0x3a, 0x08, 0x00,
	0x3a, 0x0a, 0x00,
	0x3a, 0x0f, 0x58,
	0x3a, 0x10, 0x50,
	0x3a, 0x1b, 0x58,
	0x3a, 0x1e, 0x50,
	0x3a, 0x11, 0x60,
	0x3a, 0x1f, 0x28,
	0x40, 0x01, 0x02,
	0x40, 0x00, 0x09,
	0x40, 0x03, 0x08,

	// manual AWB,manual AE,close Lenc,open WBC
	0x35, 0x03, 0x07,		//;manual AE					 
	0x35, 0x01, 0x3c,								 
	0x35, 0x02, 0x00,								 
	0x35, 0x0a, 0x00,								 
	0x35, 0x0b, 0x7f,								 
	0x50, 0x01, 0x01,		//;manual AWB					 
	0x51, 0x80, 0x08,								 
	0x51, 0x86, 0x04,								 
	0x51, 0x87, 0x00,								 
	0x51, 0x88, 0x04,								 
	0x51, 0x89, 0x00,								 
	0x51, 0x8a, 0x04,								 
	0x51, 0x8b, 0x00,								 
	0x50, 0x00, 0x06,		//;No lenc,WBC on	
		
	0x30, 0x34, 0x1a,		/* PLL ctrl0 *///0x1a zmlin mod
	//0x30, 0x35, 0x21,		/* Debug mode  */
	0x30, 0x36, 0x4a,		/*	48.1M */
	0x30, 0x37, 0x02,		/* PLL ctrl3 */				
	0x31, 0x06, 0xf9,			
	0x30, 0x3c, 0x11,		/*	PLLS control2  0x12*/
	
#if 0
	0x50, 0x3D, 0xa3,
#endif
	
	0x01, 0x00, 0x01,

};


static code UINT8 front30fpsPrevCmdTbl[] = {	
	0x01, 0x00, 0x00,
	0x40, 0x05, 0x18,		//mtk70677,Upate BLC when gain changed
	0x40, 0x51, 0x8F,	
	0x36, 0x12, 0x09,		/*	Reserved */
	0x36, 0x18, 0x00,		/*	Reserved */	
	0x37, 0x08, 0x22,		/*	Reserved */
	0x37, 0x09, 0x52,		/*	Reserved */
	0x37, 0x0c, 0x03,		/*	Reserved */	
	0x38, 0x00, 0x00,		/* TIMING X ADDR START	*/
	0x38, 0x01, 0x08,		/* TIMING X ADDR START	*/
	0x38, 0x02, 0x00,		/* TIMING Y ADDR START	*/
	0x38, 0x03, 0x02,		/* TIMING Y ADDR START	*/	
	0x38, 0x04, 0x0a,		/* TIMING X ADDR END  */
	0x38, 0x05, 0x37,		/* TIMING X ADDR END  */
	0x38, 0x06, 0x07,		/* TIMING Y ADDR END  */
	0x38, 0x07, 0xa1,		/* TIMING Y ADDR END  */			
	0x38, 0x08, 0x05,		/* TIMING X OUTPUT SIZE 0x0500 */
	0x38, 0x09, 0x10,		/* TIMING X OUTPUT SIZE  1280 */
	0x38, 0x0a, 0x03,		/* TIMING Y OUTPUT SIZE  0x03c0 */
	0x38, 0x0b, 0xcc,		/* TIMING Y OUTPUT SIZE  960 */
	0x38, 0x0c, 0x07,		/*	TIMING HTS 0x065e */
	0x38, 0x0d, 0x68,		/*	TIMING HTS 1630 */
	0x38, 0x0e, 0x03,		/*	TIMING VTS 0x03d8 */
	0x38, 0x0f, 0xd8,		/*	TIMING VTS 984 */			
	0x38, 0x14, 0x31,		/*	TIMING X INC */
	0x38, 0x15, 0x31,		/*	TIMING Y INC */
	0x38, 0x20, 0x47,//0x41,//0x47,		/*	timing tc reg20 */
	0x38, 0x21, 0x01,//0x07,//0x01,		/*	timing tc reg21 */
	0x3a, 0x08, 0x01,		/* B50 STEP  */			
	0x3a, 0x09, 0x27,		/* B50 STEP  */
	0x3a, 0x0a, 0x00,		/* B60 STEP  */
	0x3a, 0x0b, 0xf6,		/* B60 STEP  */
	0x3a, 0x0d, 0x04,		/* B60 MAX	*/
	0x3a, 0x0e, 0x03,		/* B50 MAX	*/
	0x40, 0x04, 0x02,		/* BLC CTRL04  */	
	0x30, 0x34, 0x1a,		/* PLL ctrl0 *///zmlin mod 1a
	0x30, 0x35, 0x11,		/* Debug mode  */
	0x30, 0x36, 0x3D,		/*	52.867M */
	0x30, 0x37, 0x03,		/* PLL ctrl3 */	
	0x30, 0x3c, 0x11,		/*	PLLS control2  0x12*/	
	0x31, 0x06, 0xf9, 	
	0x01, 0x00, 0x01,
};

static code UINT8 frontInit3CmdTbl[] = {	
	0x01, 0x00, 0x00,
	0x40, 0x05, 0x18,		//mtk70677,Upate BLC when gain changed
	0x40, 0x51, 0x8F,	
	0x36, 0x12, 0x09,		/*	Reserved */
	0x36, 0x18, 0x00,		/*	Reserved */	
	0x37, 0x08, 0x22,		/*	Reserved */
	0x37, 0x09, 0x52,		/*	Reserved */
	0x37, 0x0c, 0x03,		/*	Reserved */	
	0x38, 0x00, 0x00,		/* TIMING X ADDR START	*/
	0x38, 0x01, 0x08,		/* TIMING X ADDR START	*/
	0x38, 0x02, 0x00,		/* TIMING Y ADDR START	*/
	0x38, 0x03, 0x02,		/* TIMING Y ADDR START	*/	
	0x38, 0x04, 0x0a,		/* TIMING X ADDR END  */
	0x38, 0x05, 0x37,		/* TIMING X ADDR END  */
	0x38, 0x06, 0x07,		/* TIMING Y ADDR END  */
	0x38, 0x07, 0xa1,		/* TIMING Y ADDR END  */			
	0x38, 0x08, 0x05,		/* TIMING X OUTPUT SIZE 0x0500 */
	0x38, 0x09, 0x10,		/* TIMING X OUTPUT SIZE  1280 */
	0x38, 0x0a, 0x03,		/* TIMING Y OUTPUT SIZE  0x03c0 */
	0x38, 0x0b, 0xcc,		/* TIMING Y OUTPUT SIZE  960 */
	0x38, 0x0c, 0x07,		/*	TIMING HTS 0x065e */
	0x38, 0x0d, 0x68,		/*	TIMING HTS 1630 */
	0x38, 0x0e, 0x03,		/*	TIMING VTS 0x03d8 */
	0x38, 0x0f, 0xd8,		/*	TIMING VTS 984 */			
	0x38, 0x14, 0x31,		/*	TIMING X INC */
	0x38, 0x15, 0x31,		/*	TIMING Y INC */
	0x38, 0x20, 0x47, // 0x41,		/*	timing tc reg20 */
	0x38, 0x21, 0x07,		/*	timing tc reg21 */
	0x3a, 0x08, 0x01,		/* B50 STEP  */			
	0x3a, 0x09, 0x27,		/* B50 STEP  */
	0x3a, 0x0a, 0x00,		/* B60 STEP  */
	0x3a, 0x0b, 0xf6,		/* B60 STEP  */
	0x3a, 0x0d, 0x04,		/* B60 MAX	*/
	0x3a, 0x0e, 0x03,		/* B50 MAX	*/
	0x40, 0x04, 0x02,		/* BLC CTRL04  */	
	0x30, 0x34, 0x1a,		/* PLL ctrl0 *///zmlin mod 1a
	0x30, 0x35, 0x11,		/* Debug mode  */
	0x30, 0x36, 0x3D,		/*	52.867M */
	0x30, 0x37, 0x03,		/* PLL ctrl3 */	
	0x30, 0x3c, 0x11,		/*	PLLS control2  0x12*/	
	0x31, 0x06, 0xf9, 	
	0x01, 0x00, 0x01,
};



static code UINT8 frontSnapCmdTbl[] = {
	0x01,0x00,0x00,//
	0x01,0x00,0x00,//	
	0x01,0x00,0x00,//	
	0x30,0x35,0x11,//
	0x30,0x36,0x40,//
	0x30,0x3c,0x11,//
	0x37,0x08,0x24,
	0x37,0x09,0x12,	
	0x37,0x0c,0x00,//
	0x36,0x12,0x4b,//
	0x36,0x18,0x04,//	
	0x38,0x00,0x00,//		/* TIMING X ADDR START	*/
	0x38,0x01,0x0c,//		/* TIMING X ADDR START	*/	
	0x38,0x02,0x00,//		/* TIMING Y ADDR START	*/
	0x38,0x03,0x04,//		/* TIMING Y ADDR START	*/	
	0x38,0x04,0x0a,//		/* TIMING X ADDR END  */
	0x38,0x05,0x33,//		/* TIMING X ADDR END  *	
	0x38,0x06,0x07,//		/* TIMING Y ADDR END  */
	0x38,0x07,0xa3,//		/* TIMING Y ADDR END  */		
	0x38,0x08,0x0a,//		/* TIMING X OUTPUT SIZE 0x0a20 */
	0x38,0x09,0x20,//		/* TIMING X OUTPUT SIZE 2592 */	
	0x38,0x0a,0x07,//		/* TIMING Y OUTPUT SIZE 0x0798*/
	0x38,0x0b,0x98,//		/* TIMING Y OUTPUT SIZE 1944 */	
	0x38,0x0c,0x0a,//
	0x38,0x0d,0x8c,	//96	//2700	
	0x38,0x0e,0x07,//
	0x38,0x0f,0xb0,	//	//1968 	
	0x38,0x14,0x11,//
	0x38,0x15,0x11,//	
	0x38,0x21,0x00,//0x06,//0x00
	0x38,0x20,0x06,//0x00,//0x06	
	//0x3a,0x09,0x27,
	//0x3a,0x0b,0xf6,
	//0x3a,0x0d,0x08,
	//0x3a,0x0e,0x06,
	//0x40,0x04,0x04,
	//0x30,0x35,0x11,
	//0x30,0x3c,0x11,
	//0x3a,0x08,0x01,
	//0x3a,0x09,0x27,
	//0x3a,0x0a,0x00,
	//0x3a,0x0b,0xf6,
	//0x3a,0x0d,0x08,
	//0x3a,0x0e,0x06,
	0x01,0x00,0x01,
	0x01,0x00,0x01,
	0x01,0x00,0x01,
};

static code UINT8 frontInit4CmdTbl[] = {
	0x01,0x00,0x00,//
	0x01,0x00,0x00,//	
	0x01,0x00,0x00,//	
	0x30,0x35,0x11,//
	0x30,0x36,0x40,//
	0x30,0x3c,0x11,//
	0x37,0x08,0x24,
	0x37,0x09,0x12,	
	0x37,0x0c,0x00,//
	0x36,0x12,0x4b,//
	0x36,0x18,0x04,//	
	0x38,0x00,0x00,//		/* TIMING X ADDR START	*/
	0x38,0x01,0x0c,//		/* TIMING X ADDR START	*/	
	0x38,0x02,0x00,//		/* TIMING Y ADDR START	*/
	0x38,0x03,0x04,//		/* TIMING Y ADDR START	*/	
	0x38,0x04,0x0a,//		/* TIMING X ADDR END  */
	0x38,0x05,0x33,//		/* TIMING X ADDR END  *	
	0x38,0x06,0x07,//		/* TIMING Y ADDR END  */
	0x38,0x07,0xa3,//		/* TIMING Y ADDR END  */		
	0x38,0x08,0x0a,//		/* TIMING X OUTPUT SIZE 0x0a20 */
	0x38,0x09,0x20,//		/* TIMING X OUTPUT SIZE 2592 */	
	0x38,0x0a,0x07,//		/* TIMING Y OUTPUT SIZE 0x0798*/
	0x38,0x0b,0x98,//		/* TIMING Y OUTPUT SIZE 1944 */	
	0x38,0x0c,0x0a,//
	0x38,0x0d,0x8c,	//96	//2700	
	0x38,0x0e,0x07,//
	0x38,0x0f,0xb0,	//	//1968 	
	0x38,0x14,0x11,//
	0x38,0x15,0x11,//	
	0x38,0x21,0x06,//
	0x38,0x20,0x06,//	
	//0x3a,0x09,0x27,
	//0x3a,0x0b,0xf6,
	//0x3a,0x0d,0x08,
	//0x3a,0x0e,0x06,
	//0x40,0x04,0x04,
	//0x30,0x35,0x11,
	//0x30,0x3c,0x11,
	//0x3a,0x08,0x01,
	//0x3a,0x09,0x27,
	//0x3a,0x0a,0x00,
	//0x3a,0x0b,0xf6,
	//0x3a,0x0d,0x08,
	//0x3a,0x0e,0x06,
	0x01,0x00,0x01,
	0x01,0x00,0x01,
	0x01,0x00,0x01,
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

static code UINT8 front30fpsPrevDz1xCmdTbl[1] = {
	0,
};
static UINT8 front30fpsPrevDz2xCmdTbl[1] = {
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
/* 
	打开闪光灯拍照时需要增加曝光时间，降低GAIN（减少噪点）
 */
extern UINT8 flash_led_snap_adj_exp;

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
	UINT16 addrt[] = {0x300a};
	UINT8 datat[] = {0XFF};
	UINT8 i;

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
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 3, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cRegClkCfgSet(I2C_REG_CLK_DIV);
	i2cClkCfgSet(I2C_CLK_DIV);	

	/* send i2c command. */
	globalTimerWait(300);

	//frontResCmdBinSend(_frontInit0CmdTbl);
	
	//globalTimerWait(50);
	
	frontResCmdBinSend(_frontInit1CmdTbl);
	globalTimerWait(5);
	frontResCmdBinSend(_frontInit2CmdTbl);
	globalTimerWait(50);

	
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

	__FUNC_TRACK__;
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */
	__FUNC_TRACK__;

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	if ( frontPrevMode == 0 ) {  /* 30 fps. */
		// frontResCmdBinSend(_front30fpsPrevCmdTbl);
		frontResCmdBinSend(sensor_switch ? _front30fpsPrevCmdTbl : _frontInit3CmdTbl);
	}
	else if ( frontPrevMode == 1 ) {  /* 60 fps. */
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
	}
	else if (frontPrevMode == 2) {	/* 24 fps */
		frontResCmdBinSend(_front20fpsPrevCmdTbl);
	}
	// printf("frontPrevMode %d\n", (UINT16)frontPrevMode);

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

	cdspBayerPatternSet(BAYER_PTN_PREV);
	
	__FUNC_TRACK__;
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */
	__FUNC_TRACK__;

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
	
		// frontResCmdBinSend(_frontSnapCmdTbl);
		frontResCmdBinSend(sensor_switch ? _frontSnapCmdTbl : _frontInit4CmdTbl);
			
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
	//UINT16 ADDR16=0x350b;

	//Manual Gain Ctrl First Set REG bit 0x3503[1] to Enable
	//Gain = high bit in 0x350A[0], low bit in 0x350B[7:0]
	//Gain = (0x350B[6]+1)*(0x350B[5]+1)*(0x350B[4]+1)*(0x350B[3:0]/16+1)

	//printf("gain idx=%bu\n",val);

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;

	//printf("gain set frontLastGain=%d\n",frontLastGain);

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0B;
	cmdTbl[2] = (UINT8)(frontLastGain & 0x00FF);

	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0a;
	cmdTbl[5] = (UINT8)((frontLastGain & 0x0300) >> 8);
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
	cmdTbl[2] = (UINT8)((espline_16 & 0x000F0000) >> 16);
	
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x01;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x02;
	cmdTbl[8] = (UINT8)(espline_16 & 0x000000FF);
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

	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0B;
	cmdTbl[2] = (UINT8)(frontLastGain & 0x00FF);
	//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);

	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0a;
	cmdTbl[5] = (UINT8)((frontLastGain & 0x0300) >> 8);
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
	cmdTbl[2] = (UINT8)((espline_16 & 0x000F0000) >> 16);
	
	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x01;
	cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x02;
	cmdTbl[8] = (UINT8)(espline_16 & 0x000000Ff);
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
	#if (_HW_SOLUTION_ == _HW_RDK_A_) 
	/* sensor reset (fmgpio 10) */
	gpioByteFuncSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteDirSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0x00);
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_FM1, 0x04, 0xFF);
	#endif	
	
	/*Power on*/
	/*
	gpioByteOutSet(GPIO_BYTE_GEN3, 0x01 << (24 & 0x07), 0x01 << (24 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN3, 0x01 << (24 & 0x07), 0x01 << (24 & 0x07));
	gpioByteFuncSet(GPIO_BYTE_GEN3, 0x01 << (24 & 0x07), 0x01 << (24 & 0x07));
	*/
	
	/*Reset*/
	gpioByteFuncSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));  //gpio 1 reset
	gpioByteDirSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
	sp1kHalWait(20);
	
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (0 << ( 0 & 0x07)));
	sp1kHalWait(100);
	
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
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
	UINT32 pixclk, espline_16;
	UINT8 cmdTbl[3*7];
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
	
	espline = (UINT32)(pixclk * 10 * PREV_TO_SNAP_BINNING_RATE_30FPS_PREV/*frontPrevToSnapBinningRateTbl[frontPrevMode]*/) / (UINT32)(ltotal * frontSnapLastShutter);// 2*2 binning mode on need used
	//printf("espline=%d\n",espline);

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
	if (flash_led_snap_adj_exp) {
		espline_16 <<= 1;
	}
	//printf("espline_16=%lu\n",espline_16);

	/* VTS DIFF set */
	cmdTbl[0] = 0x35;
	cmdTbl[1] = 0x0c;
	cmdTbl[2] = 0x00; // (UINT8)((compensation_line & 0xFF00)>>8);

	cmdTbl[3] = 0x35;
	cmdTbl[4] = 0x0d;
	cmdTbl[5] = 0x00; // (UINT8)(compensation_line & 0x00FF);

	/* exposure time set */
	cmdTbl[6] = 0x35;
	cmdTbl[7] = 0x00;
	cmdTbl[8] = (UINT8)((espline_16 & 0x000F0000) >> 16);
	
	cmdTbl[9]  = 0x35;
	cmdTbl[10] = 0x01;
	cmdTbl[11] = (UINT8)((espline_16 & 0x0000FF00) >> 8);
	
	cmdTbl[12] = 0x35;
	cmdTbl[13] = 0x02;
	cmdTbl[14] = (UINT8)(espline_16 & 0x000000FF);

	printf("pre-s_gain=%d\n",s_gain);
	
	/* gain set */
	cmdTbl[15]  = 0x35;
	cmdTbl[16] = 0x0B;
	cmdTbl[17] = (UINT8)(s_gain & 0x00FF);

	cmdTbl[18]  = 0x35;
	cmdTbl[19] = 0x0a;
	cmdTbl[20] = (UINT8)((s_gain & 0x0300) >> 8);

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
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * PREV_TO_SNAP_BINNING_RATE_30FPS_PREV/*frontPrevToSnapBinningRateTbl[frontPrevMode]*/);
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
		capBanding = ((isoShutter > bandingShutter*PREV_TO_SNAP_BINNING_RATE_30FPS_PREV/*frontPrevToSnapBinningRateTbl[frontPrevMode]*/) ? 1 : 0);
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
			limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * PREV_TO_SNAP_BINNING_RATE_30FPS_PREV/*frontPrevToSnapBinningRateTbl[frontPrevMode]*/);

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

		if (pwrFreq == SP1K_FLICKER_60) {
			minIdx = pcap->aeMinIdxTbl[0];
			maxIdx = pcap->aeMaxIdxTbl[0];
		}
		else {
			minIdx = pcap->aeMinIdxTbl[1];
			maxIdx = pcap->aeMaxIdxTbl[1];
		}
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
	sp1kNandRsvSizeGet(fileId, NULL, &size); // Res_0x2.res = calLen.bin
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
		case _frontInit2CmdTbl:
			pData = frontInit2CmdTbl;
			size = sizeof(frontInit2CmdTbl);
			break;
		case _frontInit3CmdTbl:
			pData = frontInit3CmdTbl;
			size = sizeof(frontInit3CmdTbl);
			break;
		case _frontInit4CmdTbl:
			pData = frontInit4CmdTbl;
			size = sizeof(frontInit4CmdTbl);
			break;
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
