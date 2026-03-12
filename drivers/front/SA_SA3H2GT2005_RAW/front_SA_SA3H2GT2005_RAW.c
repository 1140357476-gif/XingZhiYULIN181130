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
#define PV_AUTO_OB_BY_GAIN	0	/* pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1 */
#define RES_LSC_LOAD		0	/* after power on load lsc resource file */

/**************************************************************************
 *                            M I R R O R   &   F L I P	C T R L 
 *			RDK : mirror off, flip on
 *			DDX : mirror on, flip on
 **************************************************************************/
/* mirror, flip ctrl */
#define MIRROR_CTRL		0	/* mirror off : 0, mirror on : 1 */
#define FLIP_CTRL		1	/* flip off : 0, flip on : 1 */

/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  1
/* YUV GT2005 */
#define PREV_MODE_TOT_YUV  1
#define SNAP_MODE_TOT_YUV  1


/* dimensiion. */
/* VGA 30FPS */
#define WIDTH_30FPS_PREV     800
#define HEIGHT_30FPS_PREV    600
#define XOFF_30FPS_PREV      8
#define YOFF_30FPS_PREV      6
#define LINE_TOT_30FPS_PREV   3470
#define FRAME_TOT_30FPS_PREV  630
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  (1) // (1.6)// 2 // 4
#define SENSOR_ZOOM_FACTOR_30FPS_PREV         0//200

/* 2M 15fps */
#define WIDTH_15FPS_PREV       1536//1280//1632//1600//1568//1312//1344//1376//1408//1440//1472//1504//1536
#define HEIGHT_15FPS_PREV      1152//960//1224//1200//1176//984//1008//1032//1056//1080//1104//1128//1152
#define XOFF_15FPS_PREV       48//176//0//16//32//160//144//128//112//96//80//64//48
#define YOFF_15FPS_PREV       36//132//0//12//24//120//108//96//84//72//60//48//36
#define LINE_TOT_15FPS_PREV   3470
#define FRAME_TOT_15FPS_PREV  1248

#define PREV_TO_SNAP_BINNING_RATE_15FPS_PREV  (1) // (1.6)// 2 // 4
#define SENSOR_ZOOM_FACTOR_15FPS_PREV         0//200

// YUV dimensiion
#define WIDTH_30FPS_PREV_YUV     768 	// 800
#define HEIGHT_30FPS_PREV_YUV    576 	// 600
#define XOFF_30FPS_PREV_YUV      16 	// 2
#define YOFF_30FPS_PREV_YUV      12 	// 1
#define LINE_TOT_30FPS_PREV_YUV  780
#define FRAME_TOT_30FPS_PREV_YUV 560	// 525

/* 8M */
#define WIDTH_SNAP     3072//3232//3264
#define HEIGHT_SNAP    2304//2424//2448
#define XOFF_SNAP      96//16
#define YOFF_SNAP     	72//12
#define LINE_TOT_SNAP   3470
#define FRAME_TOT_SNAP  2480
// YUV VGA 
#define WIDTH_SNAP_YUV      1536 // 1600
#define HEIGHT_SNAP_YUV     1152 // 1200
#define XOFF_SNAP_YUV     	32
#define YOFF_SNAP_YUV      	24
#define LINE_TOT_SNAP_YUV   780
#define FRAME_TOT_SNAP_YUV  560// 525




/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT // 522 MHZ
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    11	// 23.72727272727273 MHZ
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    11
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_15FPS_PREV    11	// 23.72727272727273 MHZ
#define MCLK_PHASE_15FPS_PREV  0
#define PCLK_DIV_15FPS_PREV    11
#define PCLK_PHASE_15FPS_PREV  0

#define MCLK_DIV_SNAP          		11
#define MCLK_DIV_SNAP_BURST     	11
#define MCLK_DIV_SNAP_BURST_UP 		11
#define MCLK_PHASE_SNAP         	0
#define PCLK_DIV_SNAP           	11
#define PCLK_PHASE_SNAP         	0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV     64200000L // 44850000L // 48000000L // 65250000L
#define PCLK_FREQ_30FPS_PREV_TD  44850000L
#define PCLK_FREQ_15FPS_PREV     64200000L // 44850000L // 48000000L // 65250000L
#define PCLK_FREQ_15FPS_PREV_TD  44850000L

#define PCLK_FREQ_SNAP           64200000L // 50420000L // 80600000L//44850000L // 65250000L
#define PCLK_FREQ_SNAP_BURST  	64200000L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP 64200000L	/* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */

#else
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD	 522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV

#define PCLK_FREQ_SNAP           522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST  	 522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP  522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif
// YUV clk
#define MCLK_SRC_YUV  FRONT_MCLK_SRC_EXT
#define PCLK_SRC_YUV  FRONT_PCLK_SRC_INT

#define MCLK_DIV_30FPS_PREV_YUV    14//16// 2//change by ml 20110316
#define MCLK_PHASE_30FPS_PREV_YUV  0
#define PCLK_DIV_30FPS_PREV_YUV    2// 1//need to be set 2
#define PCLK_PHASE_30FPS_PREV_YUV  0

#define MCLK_DIV_SNAP_YUV    		14// 4
#define MCLK_DIV_SNAP_BURST_YUV     14
#define MCLK_DIV_SNAP_BURST_UP_YUV  14
#define MCLK_PHASE_SNAP_YUV  0
#define PCLK_DIV_SNAP_YUV    2// 1//need to be set 2
#define PCLK_PHASE_SNAP_YUV  0

#if PCLK_SRC_YUV == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV_YUV  24000000L
#define PCLK_FREQ_SNAP_YUV        24000000L
#else
#define PCLK_FREQ_30FPS_PREV_YUV  851200000L / MCLK_DIV_30FPS_PREV_YUV / PCLK_DIV_30FPS_PREV_YUV
#define PCLK_FREQ_SNAP_YUV        851200000L / MCLK_DIV_SNAP_YUV / PCLK_DIV_SNAP_YUV
#define PCLK_FREQ_SNAP_BURST_YUV  	851200000L / MCLK_DIV_SNAP_BURST_YUV / PCLK_DIV_SNAP_YUV   
#define PCLK_FREQ_SNAP_BURST_UP_YUV 851200000L / MCLK_DIV_SNAP_BURST_UP_YUV / PCLK_DIV_SNAP_YUV   
#endif


/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0
// YUV bypass
#define BYPASS_HREF_EN_YUV			0
#define BYPASS_VREF_EN_YUV			0


/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    0
#else
#define RESHAPE_HEN_30FPS_PREV    0	/* bypass h don't enable */
#endif
#define RESHAPE_HFALL_30FPS_PREV  2
#define RESHAPE_HRISE_30FPS_PREV  3

#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	/* bypass v don't enable */
#endif
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3

#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_15FPS_PREV    0
#else
#define RESHAPE_HEN_15FPS_PREV    0	/* bypass h don't enable */
#endif
#define RESHAPE_HFALL_15FPS_PREV  2
#define RESHAPE_HRISE_15FPS_PREV  3

#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_15FPS_PREV    0
#else
#define RESHAPE_VEN_15FPS_PREV    0	/* bypass v don't enable */
#endif
#define RESHAPE_VFALL_15FPS_PREV  2
#define RESHAPE_VRISE_15FPS_PREV  3

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  3
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3
/* YUV reshape. */
#if BYPASS_HREF_EN_YUV == 0
#define RESHAPE_HEN_30FPS_PREV_YUV    0
#else
#define RESHAPE_HEN_30FPS_PREV_YUV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV_YUV  5
#define RESHAPE_HRISE_30FPS_PREV_YUV  6

#if BYPASS_VREF_EN_YUV == 0
#define RESHAPE_VEN_30FPS_PREV_YUV    0
#else
#define RESHAPE_VEN_30FPS_PREV_YUV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_30FPS_PREV_YUV  2
#define RESHAPE_VRISE_30FPS_PREV_YUV  3

#define RESHAPE_HEN_SNAP_YUV    0
#define RESHAPE_HFALL_SNAP_YUV  5
#define RESHAPE_HRISE_SNAP_YUV  6
#define RESHAPE_VEN_SNAP_YUV    0
#define RESHAPE_VFALL_SNAP_YUV  2
#define RESHAPE_VRISE_SNAP_YUV  3


/* preview h,v sync inv. */
#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  1
/* YUV */
#define HSYNC_INV_PREV_YUV  1
#define VSYNC_INV_PREV_YUV  1


/* snapshot h,v sync inv. */
#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1
/* YUV */
#define HSYNC_INV_SNAP_YUV  1
#define VSYNC_INV_SNAP_YUV  1


/* bayer pattern. */
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_BGBGRR//FRONT_BAYER_PTN_RGRGBB// // FRONT_BAYER_PTN_GRRBGB
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_BGBGRR//FRONT_BAYER_PTN_RGRGBB// // FRONT_BAYER_PTN_GRRBGB
/* YUV */
#define BAYER_PTN_PREV_YUV  0x00
#define BAYER_PTN_SNAP_YUV  0x01

/* yuv interface. */
#define YUV_SEQ    FRONT_YUV_OUTPUT_UYVY
#define CCIR_MODE  FRONT_YUV_CCIR601
#define Y_SUB_128  0
#define U_SUB_128  1
#define V_SUB_128  1


/* i2c interface. */
#define I2C_DEV_ADDR  			0x20
#define I2C_REG_CLK_DIV      	I2C_SOURCE_DIV_10
#define I2C_CLK_DIV          	0 // I2C_REG_CLK_DIV_512
/* YUV */
#define I2C_DEV_ADDR_YUV  0x78
#define I2C_CLK_DIV_YUV   I2C_REG_CLK_DIV_256 // I2C_REG_CLK_DIV_256


/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
/* VGA 30FPS */
#define AE_30FPS_30_MAX_IDX  133
#define AE_30FPS_30_MIN_IDX  0
#define AE_30FPS_25_MAX_IDX  131
#define AE_30FPS_25_MIN_IDX  0
#define EV10_30FPS_EXP_IDX   100

/* 2M 15FPS */
#define AE_15FPS_30_MAX_IDX  120
#define AE_15FPS_30_MIN_IDX  5
#define AE_15FPS_25_MAX_IDX  120
#define AE_15FPS_25_MIN_IDX  5
#define EV10_15FPS_EXP_IDX   100

// yuv
#define AE_30FPS_30_MAX_IDX_YUV  1
#define AE_30FPS_30_MIN_IDX_YUV  1
#define AE_30FPS_25_MAX_IDX_YUV  1
#define AE_30FPS_25_MIN_IDX_YUV  1
#define EV10_30FPS_EXP_IDX_YUV   100


/* gain. */
#define GAIN_30FPS_MAX_IDX  240
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_15FPS_MAX_IDX  112
#define GAIN_15FPS_MIN_IDX  0
// yuv
#define GAIN_30FPS_MAX_IDX_YUV  46
#define GAIN_30FPS_MIN_IDX_YUV  5


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

/* YUV */
#define PREVIEW_SRAM_MODE_YUV	FRONT_SRAM_MODE_DISABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor


#define EXP_GAIN_SET_POS_YUV  AE_EXP_GAIN_NOT_ACTION_FOR_YUV		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP_YUV    AE_GAIN_AFTER_EXP_OFF					//AE_GAIN_AFTER_EXP_OFF,
																//AE_GAIN_AFTER_EXP_ON



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

extern UINT8 frontDevTot;


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "8m";
static code UINT8 frontDevName_YUV[] = "2m";

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
	_ae30fps50hzTbl_YUV = 0x00b5,
	_ae30fps60hzTbl_YUV = 0x00b6,
	_ae15fps50hzTbl = 0x00C0,
	_ae15fps60hzTbl = 0x00C1,
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
	_gainTbl_YUV = 0x00b7,
	_gainTbl = 0x00CA,
};

/* i2c initial cmd table resource file id */
typedef enum CvtTypeRmv_e {
	_frontInit0CmdTbl		= 0x00CB,	/* GT2005 INIT 0 */
	_frontInit1CmdTbl		= 0x00CC,	/* GT2005 INIT 1 */
	_frontInit2CmdTbl		= 0x00CD,	/* GT2005 PREVIEW VGA */
	_frontInit3CmdTbl		= 0x00CE,	/* GT2005 SNAP 2M */
	//_frontInit4CmdTbl		= 0x00CF,
	_front15fpsPrevCmdTbl	= 0x00D0,	/* 3H2 PREVIEW 2M 15FPS */
	//_front20fpsPrevCmdTbl	= 0x00D1,
	//_front24fpsPrevCmdTbl	= 0x00D2,
	_front30fpsPrevCmdTbl	= 0x00D3,	/* 3H2 PREVIEW VGA 30FPS */
	//_front60fpsPrevCmdTbl	= 0x00D4,
	_frontSnapCmdTbl		= 0x00D5,	/* 3H2 SNAP */
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
static code frontAe_t ae30fps50hzTbl[132] = {
	{	0,	332,	230,	0,	0 },
	{	1,	332,	213,	0,	0 },
	{	2,	332,	198,	0,	0 },
	{	3,	332,	184,	0,	0 },
	{	4,	332,	170,	0,	0 },
	{	5,	332,	158,	0,	0 },
	{	6,	332,	146,	0,	0 },
	{	7,	332,	135,	0,	0 },
	{	8,	332,	125,	0,	0 },
	{	9,	332,	116,	0,	0 },
	{	10,	332,	107,	0,	0 },
	{	11,	332,	99,	0,	0 },
	{	12,	332,	91,	0,	0 },
	{	13,	332,	84,	0,	0 },
	{	14,	332,	77,	0,	0 },
	{	15,	332,	71,	0,	0 },
	{	16,	332,	65,	0,	0 },
	{	17,	332,	60,	0,	0 },
	{	18,	332,	55,	0,	0 },
	{	19,	332,	50,	0,	0 },
	{	20,	332,	46,	0,	0 },
	{	21,	332,	42,	0,	0 },
	{	22,	332,	38,	0,	0 },
	{	23,	332,	34,	0,	0 },
	{	24,	332,	31,	0,	0 },
	{	25,	332,	28,	0,	0 },
	{	26,	332,	25,	0,	0 },
	{	27,	332,	22,	0,	0 },
	{	28,	332,	20,	0,	0 },
	{	29,	332,	17,	0,	0 },
	{	30,	332,	15,	0,	0 },
	{	31,	332,	13,	0,	0 },
	{	32,	332,	11,	0,	0 },
	{	33,	332,	9,	0,	0 },
	{	34,	332,	8,	0,	0 },
	{	35,	332,	6,	0,	0 },
	{	36,	332,	5,	0,	0 },
	{	37,	332,	3,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	498,	6,	0,	0 },
	{	42,	498,	4,	0,	0 },
	{	43,	498,	3,	0,	0 },
	{	44,	498,	2,	0,	0 },
	{	45,	498,	1,	0,	0 },
	{	46,	498,	0,	0,	0 },
	{	47,	994,	13,	0,	0 },
	{	48,	994,	11,	0,	0 },
	{	49,	994,	9,	0,	0 },
	{	50,	994,	7,	0,	0 },
	{	51,	994,	6,	0,	0 },
	{	52,	994,	4,	0,	0 },
	{	53,	994,	3,	0,	0 },
	{	54,	994,	2,	0,	0 },
	{	55,	994,	1,	0,	0 },
	{	56,	994,	0,	0,	0 },
	{	57,	1069,	0,	0,	0 },
	{	58,	1142,	0,	0,	0 },
	{	59,	1225,	0,	0,	0 },
	{	60,	1312,	0,	0,	0 },
	{	61,	1412,	0,	0,	0 },
	{	62,	1504,	0,	0,	0 },
	{	63,	1622,	0,	0,	0 },
	{	64,	1729,	0,	0,	0 },
	{	65,	1868,	0,	0,	0 },
	{	66,	1989,	0,	0,	0 },
	{	67,	2151,	0,	0,	0 },
	{	68,	2284,	0,	0,	0 },
	{	69,	2466,	0,	0,	0 },
	{	70,	2643,	0,	0,	0 },
	{	71,	2846,	0,	0,	0 },
	{	72,	3033,	0,	0,	0 },
	{	73,	3245,	0,	0,	0 },
	{	74,	3490,	0,	0,	0 },
	{	75,	3775,	0,	0,	0 },
	{	76,	4022,	0,	0,	0 },
	{	77,	4302,	0,	0,	0 },
	{	78,	4625,	0,	0,	0 },
	{	79,	5000,	0,	0,	0 },
	{	80,	5286,	0,	0,	0 },
	{	81,	5781,	0,	0,	0 },
	{	82,	6167,	0,	0,	0 },
	{	83,	6607,	0,	0,	0 },
	{	84,	7115,	0,	0,	0 },
	{	85,	7708,	0,	0,	0 },
	{	86,	8044,	0,	0,	0 },
	{	87,	8810,	0,	0,	0 },
	{	88,	9250,	0,	0,	0 },
	{	89,	10278,	0,	0,	0 },
	{	90,	10883,	0,	0,	0 },
	{	91,	11563,	0,	0,	0 },
	{	92,	12334,	0,	0,	0 },
	{	93,	13215,	0,	0,	0 },
	{	94,	14231,	0,	0,	1 },
	{	95,	15417,	0,	0,	1 },
	{	96,	16819,	0,	0,	0 },
	{	97,	18501,	1,	0,	0 },
	{	98,	18501,	0,	0,	0 },
	{	99,	20556,	0,	0,	0 },
	{	100,	23126,	1,	0,	1 },
	{	101,	23126,	0,	0,	1 },
	{	102,	26430,	1,	0,	1 },
	{	103,	26430,	0,	0,	1 },
	{	104,	30835,	1,	0,	2 },
	{	105,	30835,	0,	0,	2 },
	{	106,	37002,	2,	0,	0 },
	{	107,	37002,	1,	0,	0 },
	{	108,	37002,	0,	0,	0 },
	{	109,	46252,	2,	0,	1 },
	{	110,	46252,	1,	0,	1 },
	{	111,	46252,	0,	0,	1 },
	{	112,	61669,	4,	0,	2 },
	{	113,	61669,	3,	0,	2 },
	{	114,	61669,	1,	0,	2 },
	{	115,	61669,	0,	0,	2 },
	{	116,	92502,	6,	0,	5 },
	{	117,	92502,	5,	0,	5 },
	{	118,	92502,	4,	0,	4 },
	{	119,	92502,	2,	0,	4 },
	{	120,	92502,	1,	0,	4 },
	{	121,	92502,	0,	0,	3 },
	{	122,	184995,	13,	0,	24 },
	{	123,	184995,	12,	0,	23 },
	{	124,	184995,	10,	0,	22 },
	{	125,	184995,	8,	0,	20 },
	{	126,	184995,	6,	0,	18 },
	{	127,	184995,	5,	0,	17 },
	{	128,	184995,	4,	0,	17 },
	{	129,	184995,	2,	0,	15 },
	{	130,	184995,	1,	0,	14 },
	{	131,	184995,	0,	0,	13 },
	};


static code frontAe_t ae30fps60hzTbl[134] = {
	{	0,	299,	238,	0,	0 },
	{	1,	299,	221,	0,	0 },
	{	2,	299,	205,	0,	0 },
	{	3,	299,	190,	0,	0 },
	{	4,	299,	176,	0,	0 },
	{	5,	299,	164,	0,	0 },
	{	6,	299,	152,	0,	0 },
	{	7,	299,	140,	0,	0 },
	{	8,	299,	130,	0,	0 },
	{	9,	299,	120,	0,	0 },
	{	10,	299,	111,	0,	0 },
	{	11,	299,	103,	0,	0 },
	{	12,	299,	95,	0,	0 },
	{	13,	299,	87,	0,	0 },
	{	14,	299,	80,	0,	0 },
	{	15,	299,	74,	0,	0 },
	{	16,	299,	68,	0,	0 },
	{	17,	299,	62,	0,	0 },
	{	18,	299,	57,	0,	0 },
	{	19,	299,	52,	0,	0 },
	{	20,	299,	48,	0,	0 },
	{	21,	299,	44,	0,	0 },
	{	22,	299,	40,	0,	0 },
	{	23,	299,	36,	0,	0 },
	{	24,	299,	32,	0,	0 },
	{	25,	299,	29,	0,	0 },
	{	26,	299,	26,	0,	0 },
	{	27,	299,	23,	0,	0 },
	{	28,	299,	21,	0,	0 },
	{	29,	299,	18,	0,	0 },
	{	30,	299,	16,	0,	0 },
	{	31,	299,	14,	0,	0 },
	{	32,	299,	12,	0,	0 },
	{	33,	299,	10,	0,	0 },
	{	34,	299,	8,	0,	0 },
	{	35,	299,	7,	0,	0 },
	{	36,	299,	5,	0,	0 },
	{	37,	299,	4,	0,	0 },
	{	38,	299,	3,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	398,	4,	0,	0 },
	{	42,	398,	3,	0,	0 },
	{	43,	398,	2,	0,	0 },
	{	44,	398,	0,	0,	0 },
	{	45,	598,	7,	0,	0 },
	{	46,	598,	5,	0,	0 },
	{	47,	598,	4,	0,	0 },
	{	48,	598,	3,	0,	0 },
	{	49,	598,	1,	0,	0 },
	{	50,	598,	0,	0,	0 },
	{	51,	1193,	14,	0,	0 },
	{	52,	1193,	12,	0,	0 },
	{	53,	1193,	10,	0,	0 },
	{	54,	1193,	8,	0,	0 },
	{	55,	1193,	7,	0,	0 },
	{	56,	1193,	5,	0,	0 },
	{	57,	1193,	4,	0,	0 },
	{	58,	1193,	3,	0,	0 },
	{	59,	1193,	1,	0,	0 },
	{	60,	1193,	0,	0,	0 },
	{	61,	1225,	0,	0,	0 },
	{	62,	1312,	0,	0,	0 },
	{	63,	1412,	0,	0,	0 },
	{	64,	1504,	0,	0,	0 },
	{	65,	1622,	0,	0,	0 },
	{	66,	1729,	0,	0,	0 },
	{	67,	1868,	0,	0,	0 },
	{	68,	1989,	0,	0,	0 },
	{	69,	2151,	0,	0,	0 },
	{	70,	2284,	0,	0,	0 },
	{	71,	2466,	0,	0,	0 },
	{	72,	2643,	0,	0,	0 },
	{	73,	2846,	0,	0,	0 },
	{	74,	3033,	0,	0,	0 },
	{	75,	3245,	0,	0,	0 },
	{	76,	3490,	0,	0,	0 },
	{	77,	3775,	0,	0,	0 },
	{	78,	4022,	0,	0,	0 },
	{	79,	4302,	0,	0,	0 },
	{	80,	4625,	0,	0,	0 },
	{	81,	5000,	0,	0,	0 },
	{	82,	5286,	0,	0,	0 },
	{	83,	5781,	0,	0,	0 },
	{	84,	6167,	0,	0,	0 },
	{	85,	6607,	0,	0,	0 },
	{	86,	7115,	0,	0,	0 },
	{	87,	7708,	0,	0,	0 },
	{	88,	8044,	0,	0,	0 },
	{	89,	8810,	0,	0,	0 },
	{	90,	9250,	0,	0,	0 },
	{	91,	10278,	0,	0,	0 },
	{	92,	10883,	0,	0,	0 },
	{	93,	11563,	0,	0,	0 },
	{	94,	12334,	0,	0,	0 },
	{	95,	13215,	0,	0,	0 },
	{	96,	14231,	0,	0,	1 },
	{	97,	15417,	0,	0,	1 },
	{	98,	16819,	0,	0,	0 },
	{	99,	18501,	1,	0,	0 },
	{	100,	18501,	0,	0,	0 },
	{	101,	20556,	0,	0,	0 },
	{	102,	23126,	1,	0,	1 },
	{	103,	23126,	0,	0,	1 },
	{	104,	26430,	1,	0,	1 },
	{	105,	26430,	0,	0,	1 },
	{	106,	30835,	1,	0,	2 },
	{	107,	30835,	0,	0,	2 },
	{	108,	37002,	2,	0,	0 },
	{	109,	37002,	1,	0,	0 },
	{	110,	37002,	0,	0,	0 },
	{	111,	46252,	2,	0,	1 },
	{	112,	46252,	1,	0,	1 },
	{	113,	46252,	0,	0,	1 },
	{	114,	61669,	4,	0,	2 },
	{	115,	61669,	3,	0,	2 },
	{	116,	61669,	1,	0,	2 },
	{	117,	61669,	0,	0,	2 },
	{	118,	92502,	6,	0,	5 },
	{	119,	92502,	5,	0,	5 },
	{	120,	92502,	4,	0,	4 },
	{	121,	92502,	2,	0,	4 },
	{	122,	92502,	1,	0,	4 },
	{	123,	92502,	0,	0,	3 },
	{	124,	184995,	13,	0,	24 },
	{	125,	184995,	12,	0,	23 },
	{	126,	184995,	10,	0,	22 },
	{	127,	184995,	8,	0,	20 },
	{	128,	184995,	6,	0,	18 },
	{	129,	184995,	5,	0,	17 },
	{	130,	184995,	4,	0,	17 },
	{	131,	184995,	2,	0,	15 },
	{	132,	184995,	1,	0,	14 },
	{	133,	184995,	0,	0,	13 },
	};


static code frontAe_t ae15fps50hzTbl[132] = {
	{	0,	166,	107,	0,	0 },
	{	1,	166,	99,	0,	0 },
	{	2,	166,	91,	0,	0 },
	{	3,	166,	84,	0,	0 },
	{	4,	166,	77,	0,	0 },
	{	5,	166,	71,	0,	0 },
	{	6,	166,	65,	0,	0 },
	{	7,	166,	60,	0,	0 },
	{	8,	166,	55,	0,	0 },
	{	9,	166,	50,	0,	0 },
	{	10,	166,	46,	0,	0 },
	{	11,	166,	42,	0,	0 },
	{	12,	166,	38,	0,	0 },
	{	13,	166,	34,	0,	0 },
	{	14,	166,	31,	0,	0 },
	{	15,	166,	28,	0,	0 },
	{	16,	166,	25,	0,	0 },
	{	17,	166,	22,	0,	0 },
	{	18,	166,	20,	0,	0 },
	{	19,	166,	17,	0,	0 },
	{	20,	166,	15,	0,	0 },
	{	21,	166,	13,	0,	0 },
	{	22,	166,	11,	0,	0 },
	{	23,	166,	9,	0,	0 },
	{	24,	166,	8,	0,	0 },
	{	25,	166,	6,	0,	0 },
	{	26,	166,	5,	0,	0 },
	{	27,	166,	3,	0,	0 },
	{	28,	166,	2,	0,	0 },
	{	29,	166,	1,	0,	0 },
	{	30,	166,	0,	0,	0 },
	{	31,	199,	2,	0,	0 },
	{	32,	199,	1,	0,	0 },
	{	33,	249,	3,	0,	0 },
	{	34,	249,	2,	0,	0 },
	{	35,	249,	1,	0,	0 },
	{	36,	249,	0,	0,	0 },
	{	37,	332,	3,	0,	0 },
	{	38,	332,	2,	0,	0 },
	{	39,	332,	1,	0,	0 },
	{	40,	332,	0,	0,	0 },
	{	41,	498,	6,	0,	0 },
	{	42,	498,	4,	0,	0 },
	{	43,	498,	3,	0,	0 },
	{	44,	498,	2,	0,	0 },
	{	45,	498,	1,	0,	0 },
	{	46,	498,	0,	0,	0 },
	{	47,	994,	13,	0,	0 },
	{	48,	994,	11,	0,	0 },
	{	49,	994,	9,	0,	0 },
	{	50,	994,	7,	0,	0 },
	{	51,	994,	6,	0,	0 },
	{	52,	994,	4,	0,	0 },
	{	53,	994,	3,	0,	0 },
	{	54,	994,	2,	0,	0 },
	{	55,	994,	1,	0,	0 },
	{	56,	994,	0,	0,	0 },
	{	57,	1069,	0,	0,	0 },
	{	58,	1142,	0,	0,	0 },
	{	59,	1225,	0,	0,	0 },
	{	60,	1312,	0,	0,	0 },
	{	61,	1412,	0,	0,	0 },
	{	62,	1504,	0,	0,	0 },
	{	63,	1622,	0,	0,	0 },
	{	64,	1729,	0,	0,	0 },
	{	65,	1868,	0,	0,	0 },
	{	66,	1989,	0,	0,	0 },
	{	67,	2151,	0,	0,	0 },
	{	68,	2284,	0,	0,	0 },
	{	69,	2466,	0,	0,	0 },
	{	70,	2643,	0,	0,	0 },
	{	71,	2846,	0,	0,	0 },
	{	72,	3033,	0,	0,	0 },
	{	73,	3245,	0,	0,	0 },
	{	74,	3490,	0,	0,	0 },
	{	75,	3775,	0,	0,	0 },
	{	76,	4022,	0,	0,	0 },
	{	77,	4302,	0,	0,	0 },
	{	78,	4625,	0,	0,	0 },
	{	79,	5000,	0,	0,	0 },
	{	80,	5286,	0,	0,	0 },
	{	81,	5781,	0,	0,	0 },
	{	82,	6167,	0,	0,	0 },
	{	83,	6607,	0,	0,	0 },
	{	84,	7115,	0,	0,	0 },
	{	85,	7708,	0,	0,	0 },
	{	86,	8044,	0,	0,	0 },
	{	87,	8810,	0,	0,	0 },
	{	88,	9250,	0,	0,	0 },
	{	89,	10278,	0,	0,	0 },
	{	90,	10883,	0,	0,	0 },
	{	91,	11563,	0,	0,	0 },
	{	92,	12334,	0,	0,	0 },
	{	93,	13215,	0,	0,	0 },
	{	94,	14231,	0,	0,	0 },
	{	95,	15417,	0,	0,	0 },
	{	96,	16819,	0,	0,	0 },
	{	97,	18501,	1,	0,	0 },
	{	98,	18501,	0,	0,	0 },
	{	99,	20556,	0,	0,	0 },
	{	100,	23126,	1,	0,	0 },
	{	101,	23126,	0,	0,	0 },
	{	102,	26430,	1,	0,	0 },
	{	103,	26430,	0,	0,	0 },
	{	104,	30835,	1,	0,	0 },
	{	105,	30835,	0,	0,	0 },
	{	106,	37002,	2,	0,	0 },
	{	107,	37002,	1,	0,	0 },
	{	108,	37002,	0,	0,	0 },
	{	109,	46252,	2,	0,	0 },
	{	110,	46252,	1,	0,	0 },
	{	111,	46252,	0,	0,	0 },
	{	112,	61669,	4,	0,	0 },
	{	113,	61669,	3,	0,	0 },
	{	114,	61669,	1,	0,	0 },
	{	115,	61669,	0,	0,	0 },
	{	116,	92502,	6,	0,	0 },
	{	117,	92502,	5,	0,	0 },
	{	118,	92502,	4,	0,	0 },
	{	119,	92502,	2,	0,	0 },
	{	120,	92502,	1,	0,	0 },
	{	121,	92502,	0,	0,	0 },
	{	122,	184995,	13,	0,	0 },
	{	123,	184995,	12,	0,	0 },
	{	124,	184995,	10,	0,	0 },
	{	125,	184995,	8,	0,	0 },
	{	126,	184995,	6,	0,	0 },
	{	127,	184995,	5,	0,	0 },
	{	128,	184995,	4,	0,	0 },
	{	129,	184995,	2,	0,	0 },
	{	130,	184995,	1,	0,	0 },
	{	131,	184995,	0,	0,	0 },
	};

static code frontAe_t ae15fps60hzTbl[134] = {
	{	0,	149,	111,	0,	0 },
	{	1,	149,	103,	0,	0 },
	{	2,	149,	95,	0,	0 },
	{	3,	149,	88,	0,	0 },
	{	4,	149,	81,	0,	0 },
	{	5,	149,	74,	0,	0 },
	{	6,	149,	68,	0,	0 },
	{	7,	149,	63,	0,	0 },
	{	8,	149,	57,	0,	0 },
	{	9,	149,	52,	0,	0 },
	{	10,	149,	48,	0,	0 },
	{	11,	149,	44,	0,	0 },
	{	12,	149,	40,	0,	0 },
	{	13,	149,	36,	0,	0 },
	{	14,	149,	33,	0,	0 },
	{	15,	149,	29,	0,	0 },
	{	16,	149,	26,	0,	0 },
	{	17,	149,	24,	0,	0 },
	{	18,	149,	21,	0,	0 },
	{	19,	149,	18,	0,	0 },
	{	20,	149,	16,	0,	0 },
	{	21,	149,	14,	0,	0 },
	{	22,	149,	12,	0,	0 },
	{	23,	149,	10,	0,	0 },
	{	24,	149,	9,	0,	0 },
	{	25,	149,	7,	0,	0 },
	{	26,	149,	5,	0,	0 },
	{	27,	149,	4,	0,	0 },
	{	28,	149,	3,	0,	0 },
	{	29,	149,	1,	0,	0 },
	{	30,	149,	0,	0,	0 },
	{	31,	171,	1,	0,	0 },
	{	32,	171,	0,	0,	0 },
	{	33,	199,	2,	0,	0 },
	{	34,	199,	1,	0,	0 },
	{	35,	239,	2,	0,	0 },
	{	36,	239,	1,	0,	0 },
	{	37,	239,	0,	0,	0 },
	{	38,	299,	3,	0,	0 },
	{	39,	299,	1,	0,	0 },
	{	40,	299,	0,	0,	0 },
	{	41,	398,	4,	0,	0 },
	{	42,	398,	3,	0,	0 },
	{	43,	398,	2,	0,	0 },
	{	44,	398,	0,	0,	0 },
	{	45,	598,	7,	0,	0 },
	{	46,	598,	5,	0,	0 },
	{	47,	598,	4,	0,	0 },
	{	48,	598,	3,	0,	0 },
	{	49,	598,	1,	0,	0 },
	{	50,	598,	0,	0,	0 },
	{	51,	1193,	14,	0,	0 },
	{	52,	1193,	12,	0,	0 },
	{	53,	1193,	10,	0,	0 },
	{	54,	1193,	8,	0,	0 },
	{	55,	1193,	7,	0,	0 },
	{	56,	1193,	5,	0,	0 },
	{	57,	1193,	4,	0,	0 },
	{	58,	1193,	3,	0,	0 },
	{	59,	1193,	1,	0,	0 },
	{	60,	1193,	0,	0,	0 },
	{	61,	1225,	0,	0,	0 },
	{	62,	1312,	0,	0,	0 },
	{	63,	1412,	0,	0,	0 },
	{	64,	1504,	0,	0,	0 },
	{	65,	1622,	0,	0,	0 },
	{	66,	1729,	0,	0,	0 },
	{	67,	1868,	0,	0,	0 },
	{	68,	1989,	0,	0,	0 },
	{	69,	2151,	0,	0,	0 },
	{	70,	2284,	0,	0,	0 },
	{	71,	2466,	0,	0,	0 },
	{	72,	2643,	0,	0,	0 },
	{	73,	2846,	0,	0,	0 },
	{	74,	3033,	0,	0,	0 },
	{	75,	3245,	0,	0,	0 },
	{	76,	3490,	0,	0,	0 },
	{	77,	3775,	0,	0,	0 },
	{	78,	4022,	0,	0,	0 },
	{	79,	4302,	0,	0,	0 },
	{	80,	4625,	0,	0,	0 },
	{	81,	5000,	0,	0,	0 },
	{	82,	5286,	0,	0,	0 },
	{	83,	5781,	0,	0,	0 },
	{	84,	6167,	0,	0,	0 },
	{	85,	6607,	0,	0,	0 },
	{	86,	7115,	0,	0,	0 },
	{	87,	7708,	0,	0,	0 },
	{	88,	8044,	0,	0,	0 },
	{	89,	8810,	0,	0,	0 },
	{	90,	9250,	0,	0,	0 },
	{	91,	10278,	0,	0,	0 },
	{	92,	10883,	0,	0,	0 },
	{	93,	11563,	0,	0,	0 },
	{	94,	12334,	0,	0,	0 },
	{	95,	13215,	0,	0,	0 },
	{	96,	14231,	0,	0,	0 },
	{	97,	15417,	0,	0,	0 },
	{	98,	16819,	0,	0,	0 },
	{	99,	18501,	1,	0,	0 },
	{	100,	18501,	0,	0,	0 },
	{	101,	20556,	0,	0,	0 },
	{	102,	23126,	1,	0,	0 },
	{	103,	23126,	0,	0,	0 },
	{	104,	26430,	1,	0,	0 },
	{	105,	26430,	0,	0,	0 },
	{	106,	30835,	1,	0,	0 },
	{	107,	30835,	0,	0,	0 },
	{	108,	37002,	2,	0,	0 },
	{	109,	37002,	1,	0,	0 },
	{	110,	37002,	0,	0,	0 },
	{	111,	46252,	2,	0,	0 },
	{	112,	46252,	1,	0,	0 },
	{	113,	46252,	0,	0,	0 },
	{	114,	61669,	4,	0,	0 },
	{	115,	61669,	3,	0,	0 },
	{	116,	61669,	1,	0,	0 },
	{	117,	61669,	0,	0,	0 },
	{	118,	92502,	6,	0,	0 },
	{	119,	92502,	5,	0,	0 },
	{	120,	92502,	4,	0,	0 },
	{	121,	92502,	2,	0,	0 },
	{	122,	92502,	1,	0,	0 },
	{	123,	92502,	0,	0,	0 },
	{	124,	184995,	13,	0,	0 },
	{	125,	184995,	12,	0,	0 },
	{	126,	184995,	10,	0,	0 },
	{	127,	184995,	8,	0,	0 },
	{	128,	184995,	6,	0,	0 },
	{	129,	184995,	5,	0,	0 },
	{	130,	184995,	4,	0,	0 },
	{	131,	184995,	2,	0,	0 },
	{	132,	184995,	1,	0,	0 },
	{	133,	184995,	0,	0,	0 },
	};

// yuv
static code frontAe_t ae30fps50hzTbl_YUV[1] = {
	{    0,     0,  0, 0,  0 },
};

static code frontAe_t ae30fps60hzTbl_YUV[1] = {
	{    0,     0,  0, 0,  0 },
};




/* gain table. */
code UINT16 gainTbl[241] = {
   32,    34,    36,    38,    40,    42,    44,    46,    48,    50, 
   52,    54,    56,    58,    60,    62,    64,    66,    68,    70, 
   72,    74,    76,    78,    80,    82,    84,    86,    88,    90, 
   92,    94,    96,    98,   100,   102,   104,   106,   108,   110, 
  112,   114,   116,   118,   120,   122,   124,   126,   128,   130, 
  132,   134,   136,   138,   140,   142,   144,   146,   148,   150, 
  152,   154,   156,   158,   160,   162,   164,   166,   168,   170, 
  172,   174,   176,   178,   180,   182,   184,   186,   188,   190, 
  192,   194,   196,   198,   200,   202,   204,   206,   208,   210, 
  212,   214,   216,   218,   220,   222,   224,   226,   228,   230, 
  232,   234,   236,   238,   240,   242,   244,   246,   248,   250, 
  252,   254,   256,   258,   260,   262,   264,   266,   268,   270, 
  272,   274,   276,   278,   280,   282,   284,   286,   288,   290, 
  292,   294,   296,   298,   300,   302,   304,   306,   308,   310, 
  312,   314,   316,   318,   320,   322,   324,   326,   328,   330, 
  332,   334,   336,   338,   340,   342,   344,   346,   348,   350, 
  352,   354,   356,   358,   360,   362,   364,   366,   368,   370, 
  372,   374,   376,   378,   380,   382,   384,   386,   388,   390, 
  392,   394,   396,   398,   400,   402,   404,   406,   408,   410, 
  412,   414,   416,   418,   420,   422,   424,   426,   428,   430, 
  432,   434,   436,   438,   440,   442,   444,   446,   448,   450, 
  452,   454,   456,   458,   460,   462,   464,   466,   468,   470, 
  472,   474,   476,   478,   480,   482,   484,   486,   488,   490, 
  492,   494,   496,   498,   500,   502,   504,   506,   508,   510, 
  512, 	};

// yuv
static code UINT16 gainTbl_YUV[1] = {
   0,
};

static code UINT16 colorMatrixTbl[3][9] = {
	{ 0x0058, 0x03D7, 0x0011, 0x03DF, 0x007A, 0x03E7, 0x03EB, 0x03DA, 0x007B },
	{ 0x0045, 0x03EF, 0x000C, 0x03E8, 0x006B, 0x03EC, 0x03F0, 0x03F4, 0x005C },
	{ 0x0029, 0x0010, 0x0006, 0x03F4, 0x0058, 0x03F4, 0x03F8, 0x0015, 0x0033 },
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

static code UINT8 ae15fpsMaxIdxTbl[2] = {
	AE_15FPS_30_MAX_IDX,
	AE_15FPS_25_MAX_IDX,
};

static code UINT8 ae15fpsMinIdxTbl[2] = {
	AE_15FPS_30_MIN_IDX,
	AE_15FPS_25_MIN_IDX,
};

// yuv
static code UINT8 ae30fpsMaxIdxTbl_YUV[2] = {
	AE_30FPS_30_MAX_IDX_YUV,
	AE_30FPS_25_MAX_IDX_YUV,
};

static code UINT8 ae30fpsMinIdxTbl_YUV[2] = {
	AE_30FPS_30_MIN_IDX_YUV,
	AE_30FPS_25_MIN_IDX_YUV,
};



/* preview to snapshot binning rate table. */
static code UINT8 frontPrevToSnapBinningRateTbl[PREV_MODE_TOT] = {
	PREV_TO_SNAP_BINNING_RATE_30FPS_PREV,
	PREV_TO_SNAP_BINNING_RATE_15FPS_PREV,
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
	{  /* 15 fps. */
		MCLK_DIV_15FPS_PREV + 5,
		MCLK_PHASE_15FPS_PREV,
		PCLK_DIV_15FPS_PREV + 5,
		PCLK_PHASE_15FPS_PREV,
		PCLK_FREQ_15FPS_PREV_TD,
		31,
		6,
	},	
};
// yuv
static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl_YUV[PREV_MODE_TOT_YUV] = {
	{  /* 24 fps. */
		MCLK_DIV_30FPS_PREV_YUV + 5,
		MCLK_PHASE_30FPS_PREV_YUV,
		PCLK_DIV_30FPS_PREV_YUV + 5,
		PCLK_PHASE_30FPS_PREV_YUV,
		PCLK_FREQ_30FPS_PREV_YUV,
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
	{  /* 15 fps. */
		PREVIEW_SRAM_MODE,
		BAYER_PTN_PREV,
		WIDTH_15FPS_PREV,
		HEIGHT_15FPS_PREV,
		XOFF_15FPS_PREV,
		YOFF_15FPS_PREV,
		LINE_TOT_15FPS_PREV,
		FRAME_TOT_15FPS_PREV,
		MCLK_DIV_15FPS_PREV,
		MCLK_PHASE_15FPS_PREV,
		PCLK_DIV_15FPS_PREV,
		PCLK_PHASE_15FPS_PREV,
		PCLK_FREQ_15FPS_PREV,
		BYPASS_HREF_EN,
		BYPASS_VREF_EN,
		RESHAPE_HEN_15FPS_PREV,
		RESHAPE_HFALL_15FPS_PREV,
		RESHAPE_HRISE_15FPS_PREV,
		RESHAPE_VEN_15FPS_PREV,
		RESHAPE_VFALL_15FPS_PREV,
		RESHAPE_VRISE_15FPS_PREV,
		ae15fps60hzTbl,
		ae15fps50hzTbl,
		ae15fpsMaxIdxTbl,
		ae15fpsMinIdxTbl,
		EV10_15FPS_EXP_IDX,
		gainTbl,
		GAIN_15FPS_MAX_IDX,
		GAIN_15FPS_MIN_IDX,
		&frontPrevTempDenoiseTbl[1],
		SENSOR_ZOOM_FACTOR_15FPS_PREV,
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
// YUV
static code frontPrevCapabDesc_t frontPrevCapabDesc_YUV[PREV_MODE_TOT_YUV] = {
	{  /* 30 fps. */
		PREVIEW_SRAM_MODE_YUV,
		BAYER_PTN_PREV_YUV,
		WIDTH_30FPS_PREV_YUV,
		HEIGHT_30FPS_PREV_YUV,
		XOFF_30FPS_PREV_YUV,
		YOFF_30FPS_PREV_YUV,
		LINE_TOT_30FPS_PREV_YUV,
		FRAME_TOT_30FPS_PREV_YUV,
		MCLK_DIV_30FPS_PREV_YUV,
		MCLK_PHASE_30FPS_PREV_YUV,
		PCLK_DIV_30FPS_PREV_YUV,
		PCLK_PHASE_30FPS_PREV_YUV,
		PCLK_FREQ_30FPS_PREV_YUV,
		BYPASS_HREF_EN_YUV,
		BYPASS_VREF_EN_YUV,
		RESHAPE_HEN_30FPS_PREV_YUV,
		RESHAPE_HFALL_30FPS_PREV_YUV,
		RESHAPE_HRISE_30FPS_PREV_YUV,
		RESHAPE_VEN_30FPS_PREV_YUV,
		RESHAPE_VFALL_30FPS_PREV_YUV,
		RESHAPE_VRISE_30FPS_PREV_YUV,
		ae30fps60hzTbl_YUV,
		ae30fps50hzTbl_YUV,
		ae30fpsMaxIdxTbl_YUV,
		ae30fpsMinIdxTbl_YUV,
		EV10_30FPS_EXP_IDX_YUV,
		gainTbl_YUV,
		GAIN_30FPS_MAX_IDX_YUV,
		GAIN_30FPS_MIN_IDX_YUV,
		&frontPrevTempDenoiseTbl_YUV[0],
		SENSOR_ZOOM_FACTOR_30FPS_PREV,
	},
};

static code frontSnapCapabDesc_t frontSnapCapabDesc_YUV[SNAP_MODE_TOT_YUV] = {
	{  /* 30/60 fps. */
		BAYER_PTN_SNAP_YUV,
		WIDTH_SNAP_YUV,
		HEIGHT_SNAP_YUV,
		XOFF_SNAP_YUV,
		YOFF_SNAP_YUV,
		LINE_TOT_SNAP_YUV,
		FRAME_TOT_SNAP_YUV,
		MCLK_DIV_SNAP_YUV,
		MCLK_DIV_SNAP_BURST_YUV,
		MCLK_DIV_SNAP_BURST_UP_YUV,
		MCLK_PHASE_SNAP_YUV,
		PCLK_DIV_SNAP_YUV,
		PCLK_PHASE_SNAP_YUV,
		PCLK_FREQ_SNAP_YUV,
		PCLK_FREQ_SNAP_BURST_YUV,
		PCLK_FREQ_SNAP_BURST_UP_YUV,
		BYPASS_HREF_EN_YUV,
		BYPASS_VREF_EN_YUV,
		RESHAPE_HEN_SNAP_YUV,
		RESHAPE_HFALL_SNAP_YUV,
		RESHAPE_HRISE_SNAP_YUV,
		RESHAPE_VEN_SNAP_YUV,
		RESHAPE_VFALL_SNAP_YUV,
		RESHAPE_VRISE_SNAP_YUV,
	},
};




/* i2c command table. */
/* 3H2 PREVIEW VGA 30FPS */
static code UINT8 front30fpsPrevCmdTbl[] = {
	// Global
	0x01, 0x00, 0x00,

	0x30, 0x65, 0x15,
	0x31, 0x0E, 0x08,
	0x30, 0xC7, 0x0A,
	0x30, 0xBC, 0x38,
	0x30, 0xBD, 0x40,
	0x31, 0x10, 0x70,
	0x31, 0x11, 0x80,
	0x31, 0x12, 0x7B,
	0x31, 0x13, 0xC0,
	0x30, 0xC7, 0x1A,
	0x30, 0x00, 0x08,
	0x30, 0x01, 0x05,
	0x30, 0x02, 0x0D,
	0x30, 0x03, 0x21,
	0x30, 0x04, 0x62,
	0x30, 0x05, 0x0B,
	0x30, 0x06, 0x6D,
	0x30, 0x07, 0x02,
	0x30, 0x08, 0x62,
	0x30, 0x09, 0x62,
	0x30, 0x0A, 0x41,
	0x30, 0x0B, 0x10,
	0x30, 0x0C, 0x21,
	0x30, 0x0D, 0x04,
	0x30, 0x7E, 0x03,
	0x30, 0x7F, 0xA5,
	0x30, 0x80, 0x04,
	0x30, 0x81, 0x29,
	0x30, 0x82, 0x03,
	0x30, 0x83, 0x21,
	0x30, 0x11, 0x5F,
	0x31, 0x56, 0xE2,
	0x30, 0x27, 0xBE,
	0x30, 0x0f, 0x02,
	0x30, 0x10, 0x10,
	0x30, 0x17, 0x74,
	0x30, 0x18, 0x00,
	0x30, 0x20, 0x02,
	0x30, 0x21, 0x00,
	0x30, 0x23, 0x80,
	0x30, 0x24, 0x08,
	0x30, 0x25, 0x08,
	0x30, 0x1C, 0xD4,
	0x31, 0x5D, 0x00,
	0x30, 0x54, 0x00,
	0x30, 0x55, 0x35,
	0x30, 0x62, 0x04,
	0x30, 0x63, 0x38,
	0x31, 0xA4, 0x04,
	0x30, 0x16, 0x54,
	0x31, 0x57, 0x02,
	0x31, 0x58, 0x00,
	0x31, 0x5B, 0x02,
	0x31, 0x5C, 0x00,
	0x30, 0x1B, 0x05,
	0x30, 0x28, 0x41,
	0x30, 0x2A, 0x10,
	0x30, 0x60, 0x00,
	0x30, 0x2D, 0x19,
	0x30, 0x2B, 0x05,
	0x30, 0x72, 0x13,
	0x30, 0x73, 0x21,
	0x30, 0x74, 0x82,
	0x30, 0x75, 0x20,
	0x30, 0x76, 0xA2,
	0x30, 0x77, 0x02,
	0x30, 0x78, 0x91,
	0x30, 0x79, 0x91,
	0x30, 0x7A, 0x61,
	0x30, 0x7B, 0x28,
	0x30, 0x7C, 0x31,
	0x30, 0x4E, 0x40,
	0x30, 0x4F, 0x01,
	0x30, 0x50, 0x00,
	0x30, 0x88, 0x01,
	0x30, 0x89, 0x00,
	0x32, 0x10, 0x01,
	0x32, 0x11, 0x00,
	0x30, 0x8E, 0x01,
	0x30, 0x8F, 0x8F,
	0x30, 0x64, 0x03,
	0x31, 0xA7, 0x0F,

	// Mode


	0x03, 0x05, 0x04,
	0x03, 0x06, 0x00,
	0x03, 0x07, 0x6B,
	0x03, 0x03, 0x02,
	0x03, 0x01, 0x05,
	0x03, 0x0B, 0x02,
	0x03, 0x09, 0x05,
	0x30, 0xCC, 0xB0,
	0x31, 0xA1, 0x58,

	0x03, 0x44, 0x00,
	0x03, 0x45, 0x08,	// x addr start 8
	0x03, 0x46, 0x00,
	0x03, 0x47, 0x08,	// y addr start 8
	0x03, 0x48, 0x0C,
	0x03, 0x49, 0xC5,	// x addr  end 3271
	0x03, 0x4A, 0x09,
	0x03, 0x4B, 0x97,	// y addr  end 2455

	0x01, 0x01, 0x00,//0x03,//0x00,   // [1]:flip [0]:mirror


	0x03, 0x81, 0x01,
	0x03, 0x83, 0x07,
	0x03, 0x85, 0x01,
	0x03, 0x87, 0x07,
	0x04, 0x01, 0x00,
	0x04, 0x05, 0x10,
	0x07, 0x00, 0x05,
	0x07, 0x01, 0x30,
	0x03, 0x4C, 0x03,	
	0x03, 0x4D, 0x30,	// x out size 816
	0x03, 0x4E, 0x02,
	0x03, 0x4F, 0x64,	// y out size 612
	0x02, 0x00, 0x02,
	0x02, 0x01, 0x50,
	0x02, 0x02, 0x03,
	0x02, 0x03, 0xDB,
	0x02, 0x04, 0x00,
	0x02, 0x05, 0x20,
	0x03, 0x42, 0x0D,
	0x03, 0x43, 0x8E,	// Line Length 3470
	0x03, 0x40, 0x02,
	0x03, 0x41, 0x76, // 0xE8,	// Frame Length 1000
	0x30, 0x0E, 0x2D,
	0x31, 0xA3, 0x40,
	0x30, 0x1A, 0x77,
	0x30, 0x53, 0xCF,
	0x01, 0x00, 0x01,
};


/* 3H2 PREVIEW 2M 15FPS */
static code UINT8 front15fpsPrevCmdTbl[] = {
	// Global
	0x01, 0x00, 0x00,
	0x30, 0x65, 0x15,
	0x31, 0x0E, 0x08,
	0x30, 0xC7, 0x0A,
	0x30, 0xBC, 0x38,
	0x30, 0xBD, 0x40,
	0x31, 0x10, 0x70,
	0x31, 0x11, 0x80,
	0x31, 0x12, 0x7B,
	0x31, 0x13, 0xC0,
	0x30, 0xC7, 0x1A,
	0x30, 0x00, 0x08,
	0x30, 0x01, 0x05,
	0x30, 0x02, 0x0D,
	0x30, 0x03, 0x21,
	0x30, 0x04, 0x62,
	0x30, 0x05, 0x0B,
	0x30, 0x06, 0x6D,
	0x30, 0x07, 0x02,
	0x30, 0x08, 0x62,
	0x30, 0x09, 0x62,
	0x30, 0x0A, 0x41,
	0x30, 0x0B, 0x10,
	0x30, 0x0C, 0x21,
	0x30, 0x0D, 0x04,
	0x30, 0x7E, 0x03,
	0x30, 0x7F, 0xA5,
	0x30, 0x80, 0x04,
	0x30, 0x81, 0x29,
	0x30, 0x82, 0x03,
	0x30, 0x83, 0x21,
	0x30, 0x11, 0x5F,
	0x31, 0x56, 0xE2,
	0x30, 0x27, 0xBE,
	0x30, 0x0f, 0x02,
	0x30, 0x10, 0x10,
	0x30, 0x17, 0x74,
	0x30, 0x18, 0x00,
	0x30, 0x20, 0x02,
	0x30, 0x21, 0x00,
	0x30, 0x23, 0x80,
	0x30, 0x24, 0x08,
	0x30, 0x25, 0x08,
	0x30, 0x1C, 0xD4,
	0x31, 0x5D, 0x00,
	0x30, 0x54, 0x00,
	0x30, 0x55, 0x35,
	0x30, 0x62, 0x04,
	0x30, 0x63, 0x38,
	0x31, 0xA4, 0x04,
	0x30, 0x16, 0x54,
	0x31, 0x57, 0x02,
	0x31, 0x58, 0x00,
	0x31, 0x5B, 0x02,
	0x31, 0x5C, 0x00,
	0x30, 0x1B, 0x05,
	0x30, 0x28, 0x41,
	0x30, 0x2A, 0x10,
	0x30, 0x60, 0x00,
	0x30, 0x2D, 0x19,
	0x30, 0x2B, 0x05,
	0x30, 0x72, 0x13,
	0x30, 0x73, 0x21,
	0x30, 0x74, 0x82,
	0x30, 0x75, 0x20,
	0x30, 0x76, 0xA2,
	0x30, 0x77, 0x02,
	0x30, 0x78, 0x91,
	0x30, 0x79, 0x91,
	0x30, 0x7A, 0x61,
	0x30, 0x7B, 0x28,
	0x30, 0x7C, 0x31,
	0x30, 0x4E, 0x40,
	0x30, 0x4F, 0x01,
	0x30, 0x50, 0x00,
	0x30, 0x88, 0x01,
	0x30, 0x89, 0x00,
	0x32, 0x10, 0x01,
	0x32, 0x11, 0x00,
	0x30, 0x8E, 0x01,
	0x30, 0x8F, 0x8F,
	0x30, 0x64, 0x03,
	0x31, 0xA7, 0x0F,
	
	// Mode
	0x03, 0x05, 0x04,
	0x03, 0x06, 0x00,
	0x03, 0x07, 0x6B,
	0x03, 0x03, 0x02,
	0x03, 0x01, 0x05,
	0x03, 0x0B, 0x02,
	0x03, 0x09, 0x05,
	0x30, 0xCC, 0xB0,
	0x31, 0xA1, 0x58,
	0x03, 0x44, 0x00,
	0x03, 0x45, 0x08,	// x addr start 8
	0x03, 0x46, 0x00,
	0x03, 0x47, 0x08,	// y addr start 8
	0x03, 0x48, 0x0C,
	0x03, 0x49, 0xC7,	// x addr  end 3271
	0x03, 0x4A, 0x09,
	0x03, 0x4B, 0x97,	// y addr  end 2455
	
	0x01, 0x01, 0x00,	// [1]:flip [0]:mirror
	
	0x03, 0x81, 0x01,
	0x03, 0x83, 0x03,
	0x03, 0x85, 0x01,
	0x03, 0x87, 0x03,
	0x04, 0x01, 0x00,
	0x04, 0x05, 0x10,
	0x07, 0x00, 0x05,
	0x07, 0x01, 0x30,
	0x03, 0x4C, 0x06,
	0x03, 0x4D, 0x60,	// x out size 1632
	0x03, 0x4E, 0x04,
	0x03, 0x4F, 0xC8,	// y out size 1224
	0x02, 0x00, 0x02,
	0x02, 0x01, 0x50,
	0x02, 0x02, 0x03,
	0x02, 0x03, 0xDB,
	0x02, 0x04, 0x00,
	0x02, 0x05, 0x20,
	0x03, 0x42, 0x0D,
	0x03, 0x43, 0x8E,	// Line Length 3470
	0x03, 0x40, 0x04,
	0x03, 0x41, 0xE0,	// Frame Length 1248
	0x30, 0x0E, 0x2D,
	0x31, 0xA3, 0x40,
	0x30, 0x1A, 0x77,
	0x30, 0x53, 0xCF,
	0x01, 0x00, 0x01,
};
/* 3H2 SNAP 8M */
static code UINT8 frontSnapCmdTbl[] = {
	// Global
	0x01, 0x00, 0x00,

	0x30, 0x65, 0x15,
	0x31, 0x0E, 0x08,
	0x30, 0xC7, 0x0A,
	0x30, 0xBC, 0x38,
	0x30, 0xBD, 0x40,
	0x31, 0x10, 0x70,
	0x31, 0x11, 0x80,
	0x31, 0x12, 0x7B,
	0x31, 0x13, 0xC0,
	0x30, 0xC7, 0x1A,
	0x30, 0x00, 0x08,
	0x30, 0x01, 0x05,
	0x30, 0x02, 0x0D,
	0x30, 0x03, 0x21,
	0x30, 0x04, 0x62,
	0x30, 0x05, 0x0B,
	0x30, 0x06, 0x6D,
	0x30, 0x07, 0x02,
	0x30, 0x08, 0x62,
	0x30, 0x09, 0x62,
	0x30, 0x0A, 0x41,
	0x30, 0x0B, 0x10,
	0x30, 0x0C, 0x21,
	0x30, 0x0D, 0x04,
	0x30, 0x7E, 0x03,
	0x30, 0x7F, 0xA5,
	0x30, 0x80, 0x04,
	0x30, 0x81, 0x29,
	0x30, 0x82, 0x03,
	0x30, 0x83, 0x21,
	0x30, 0x11, 0x5F,
	0x31, 0x56, 0xE2,
	0x30, 0x27, 0xBE,
	0x30, 0x0f, 0x02,
	0x30, 0x10, 0x10,
	0x30, 0x17, 0x74,
	0x30, 0x18, 0x00,
	0x30, 0x20, 0x02,
	0x30, 0x21, 0x00,
	0x30, 0x23, 0x80,
	0x30, 0x24, 0x08,
	0x30, 0x25, 0x08,
	0x30, 0x1C, 0xD4,
	0x31, 0x5D, 0x00,
	0x30, 0x54, 0x00,
	0x30, 0x55, 0x35,
	0x30, 0x62, 0x04,
	0x30, 0x63, 0x38,
	0x31, 0xA4, 0x04,
	0x30, 0x16, 0x54,
	0x31, 0x57, 0x02,
	0x31, 0x58, 0x00,
	0x31, 0x5B, 0x02,
	0x31, 0x5C, 0x00,
	0x30, 0x1B, 0x05,
	0x30, 0x28, 0x41,
	0x30, 0x2A, 0x10,
	0x30, 0x60, 0x00,
	0x30, 0x2D, 0x19,
	0x30, 0x2B, 0x05,
	0x30, 0x72, 0x13,
	0x30, 0x73, 0x21,
	0x30, 0x74, 0x82,
	0x30, 0x75, 0x20,
	0x30, 0x76, 0xA2,
	0x30, 0x77, 0x02,
	0x30, 0x78, 0x91,
	0x30, 0x79, 0x91,
	0x30, 0x7A, 0x61,
	0x30, 0x7B, 0x28,
	0x30, 0x7C, 0x31,
	0x30, 0x4E, 0x40,
	0x30, 0x4F, 0x01,
	0x30, 0x50, 0x00,
	0x30, 0x88, 0x01,
	0x30, 0x89, 0x00,
	0x32, 0x10, 0x01,
	0x32, 0x11, 0x00,
	0x30, 0x8E, 0x01,
	0x30, 0x8F, 0x8F,
	0x30, 0x64, 0x03,
	0x31, 0xA7, 0x0F,
	
	// Mode
	0x03, 0x05, 0x04,
	0x03, 0x06, 0x00,
	0x03, 0x07, 0x6B,
	0x03, 0x03, 0x02,
	0x03, 0x01, 0x05,
	0x03, 0x0B, 0x02,
	0x03, 0x09, 0x05,
	0x30, 0xCC, 0xB0,
	0x31, 0xA1, 0x58,
	
	0x03, 0x44, 0x00,
	0x03, 0x45, 0x08,	// x addr start 8
	0x03, 0x46, 0x00,
	0x03, 0x47, 0x08,	// y addr start 8
	0x03, 0x48, 0x0C,
	0x03, 0x49, 0xC5,	// x addr  end 3269
	0x03, 0x4A, 0x09,	
	0x03, 0x4B, 0x97,	// y addr  end 2455

	0x01, 0x01, 0x00,//0x03,//0x00   // [1]:flip [0]:mirror
	
	0x03, 0x81, 0x01,
	0x03, 0x83, 0x01,
	0x03, 0x85, 0x01,
	0x03, 0x87, 0x01,
	0x04, 0x01, 0x00,
	0x04, 0x05, 0x10,
	0x07, 0x00, 0x05,
	0x07, 0x01, 0x30,
	0x03, 0x4C, 0x0C,
	0x03, 0x4D, 0xC0,	// x out size 3264
	0x03, 0x4E, 0x09,
	0x03, 0x4F, 0x90,	// y out size 2448
	0x02, 0x00, 0x02,
	0x02, 0x01, 0x50,
	0x02, 0x02, 0x04,
	0x02, 0x03, 0xE7,
	0x02, 0x04, 0x00,
	0x02, 0x05, 0x20,
	0x03, 0x42, 0x0D,
	0x03, 0x43, 0x8E,	// Line Length 3470
	0x03, 0x40, 0x09,
	0x03, 0x41, 0xB0,	// Frame Length 2480
	0x30, 0x0E, 0x29,
	0x31, 0xA3, 0x00,
	0x30, 0x1A, 0x77,
	0x30, 0x53, 0xCF,
	0x01, 0x00, 0x01,

};


/* GT2005 INIT 0 */
static code UINT8 frontInit0CmdTbl[] = {
	0x01, 0x02 , 0x01,

	0x01, 0x03 , 0x00,
	
	//Hcount&Vcount
	0x01, 0x05 , 0x00,
	0x01, 0x06 , 0xF0,
	0x01, 0x07 , 0x00,
	0x01, 0x08 , 0x1C,
	
	//Binning&Resoultion
	0x01, 0x09 , 0x00,
	0x01, 0x0A , 0x04,
	0x01, 0x0B , 0x0F,
	0x01, 0x0C , 0x00,
	0x01, 0x0D , 0x08,
	0x01, 0x0E , 0x00,
	0x01, 0x0F , 0x08,
	
	0x01, 0x10 , 0x02,
	0x01, 0x11 , 0x80,
	0x01, 0x12 , 0x01,
	0x01, 0x13 , 0xE0,
	
	//YUV Mode
	0x01, 0x14 , 0x04,
	
	//Picture Effect
	0x01, 0x15 , 0x00,

	//PLL&Frame Rate
	0x01, 0x16 , 0x01,
	0x01, 0x17 , 0x00,
	0x01, 0x18 , 0x34,
	0x01, 0x19 , 0x01,
	0x01, 0x1A , 0x04,
	0x01, 0x1B , 0x01,
	
	//DCLK Polarity
	0x01, 0x1C , 0x00,

	//Do not change
	0x01, 0x1D , 0x01,
	0x01, 0x1E , 0x00,
	
	0x01, 0x1F , 0x00,
	0x01, 0x20 , 0x1C,
	0x01, 0x21 , 0x00,
	0x01, 0x22 , 0x04,
	0x01, 0x23 , 0x00,
	0x01, 0x24 , 0x00,
	0x01, 0x25 , 0x00,
	0x01, 0x26 , 0x00,
	0x01, 0x27 , 0x00,
	0x01, 0x28 , 0x00,
	
	//Contrast
	0x02, 0x00 , 0x30,
	
	//Brightness
	0x02, 0x01 , 0x18,
	
	//Saturation
	0x02, 0x02 , 0x50,
	
	//Do not change
	0x02, 0x03 , 0x00,
	0x02, 0x04 , 0x03,
	0x02, 0x05 , 0x1F,
	0x02, 0x06 , 0x0B,
	0x02, 0x07 , 0x20,
	0x02, 0x08 , 0x00,
	0x02, 0x09 , 0x2A,
	0x02, 0x0A , 0x01,
	
	//Sharpness
	0x02, 0x0B , 0x48,
	0x02, 0x0C , 0x64,
	
	//Do not change
	0x02, 0x0D , 0xC8,
	0x02, 0x0E , 0xBC,
	0x02, 0x0F , 0x08,
	0x02, 0x10 , 0xE6,
	0x02, 0x11 , 0x00,
	0x02, 0x12 , 0x20,
	0x02, 0x13 , 0x81,
	0x02, 0x14 , 0x15,
	0x02, 0x15 , 0x00,
	0x02, 0x16 , 0x00,
	0x02, 0x17 , 0x00,
	0x02, 0x18 , 0x46,
	0x02, 0x19 , 0x30,
	0x02, 0x1A , 0x03,
	0x02, 0x1B , 0x28,
	0x02, 0x1C , 0x02,
	0x02, 0x1D , 0x60,
	0x02, 0x1E , 0x00,
	0x02, 0x1F , 0x00,
	
	//Color Manager(Default)
	0x02, 0x20 , 0x08,
	0x02, 0x21 , 0x08,
	0x02, 0x22 , 0x04,
	0x02, 0x23 , 0x00,
	0x02, 0x24 , 0x1F,
	0x02, 0x25 , 0x1E,
	0x02, 0x26 , 0x18,
	0x02, 0x27 , 0x1D,
	0x02, 0x28 , 0x1F,
	0x02, 0x29 , 0x1F,
	0x02, 0x2A , 0x01,
	0x02, 0x2B , 0x04,
	0x02, 0x2C , 0x05,
	0x02, 0x2D , 0x05,
	0x02, 0x2E , 0x04,
	0x02, 0x2F , 0x03,
	0x02, 0x30 , 0x02,
	0x02, 0x31 , 0x1F,
	0x02, 0x32 , 0x1A,
	0x02, 0x33 , 0x19,
	0x02, 0x34 , 0x19,
	0x02, 0x35 , 0x1B,
	0x02, 0x36 , 0x1F,
	0x02, 0x37 , 0x04,
	0x02, 0x38 , 0xEE,
	0x02, 0x39 , 0xFF,
	0x02, 0x3A , 0x00,
	0x02, 0x3B , 0x00,
	0x02, 0x3C , 0x00,
	0x02, 0x3D , 0x00,
	0x02, 0x3E , 0x00,
	0x02, 0x3F , 0x00,
	0x02, 0x40 , 0x00,
	0x02, 0x41 , 0x00,
	0x02, 0x42 , 0x00,
	0x02, 0x43 , 0x21,
	0x02, 0x44 , 0x42,
	0x02, 0x45 , 0x53,
	0x02, 0x46 , 0x54,
	0x02, 0x47 , 0x54,
	0x02, 0x48 , 0x54,
	0x02, 0x49 , 0x33,
	0x02, 0x4A , 0x11,
	0x02, 0x4B , 0x00,
	0x02, 0x4C , 0x00,
	0x02, 0x4D , 0xFF,
	0x02, 0x4E , 0xEE,
	0x02, 0x4F , 0xDD,
	0x02, 0x50 , 0x00,
	0x02, 0x51 , 0x00,
	0x02, 0x52 , 0x00,
	0x02, 0x53 , 0x00,
	0x02, 0x54 , 0x00,
	0x02, 0x55 , 0x00,
	0x02, 0x56 , 0x00,
	0x02, 0x57 , 0x00,
	0x02, 0x58 , 0x00,
	0x02, 0x59 , 0x00,
	0x02, 0x5A , 0x00,
	0x02, 0x5B , 0x00,
	0x02, 0x5C , 0x00,
	0x02, 0x5D , 0x00,
	0x02, 0x5E , 0x00,
	0x02, 0x5F , 0x00,
	0x02, 0x60 , 0x00,
	0x02, 0x61 , 0x00,
	0x02, 0x62 , 0x00,
	0x02, 0x63 , 0x00,
	0x02, 0x64 , 0x00,
	0x02, 0x65 , 0x00,
	0x02, 0x66 , 0x00,
	0x02, 0x67 , 0x00,
		 
	0x02, 0x68 , 0x8F,
	0x02, 0x69 , 0xA3,
	0x02, 0x6A , 0xB4,
	0x02, 0x6B , 0x90,
	0x02, 0x6C , 0x00,
	0x02, 0x6D , 0xD0,
	0x02, 0x6E , 0x60,
	0x02, 0x6F , 0xA0,
	0x02, 0x70 , 0x40,
	0x03, 0x00 , 0x81,
	0x03, 0x01 , 0x80,
	0x03, 0x02 , 0x22,
	0x03, 0x03 , 0x06,
	0x03, 0x04 , 0x03,
	0x03, 0x05 , 0x83,
	0x03, 0x06 , 0x00,
	0x03, 0x07 , 0x22,
	0x03, 0x08 , 0x00,
	0x03, 0x09 , 0x55,
	0x03, 0x0A , 0x55,
	0x03, 0x0B , 0x55,
	0x03, 0x0C , 0x54,
	0x03, 0x0D , 0x13,
	0x03, 0x0E , 0x13,
	0x03, 0x0F , 0x10,
	0x03, 0x10 , 0x04,
	0x03, 0x11 , 0xFF,
	0x03, 0x12 , 0x08,	// 0x98:15fps 0x08:23fps
};

/* GT2005 INIT 1 */
static code UINT8 frontInit1CmdTbl[] = {
	//Banding Setting(50Hz)
	0x03, 0x13 , 0x28,
	0x03, 0x14 , 0x34,
	0x03, 0x15 , 0x3B,
	0x03, 0x16 , 0x16,

	0x03, 0x17 , 0x02,
	0x03, 0x18 , 0x08,
	0x03, 0x19 , 0x0C,

	//A LIGHT CORRECTION
	0x03, 0x1A , 0x81,
	0x03, 0x1B , 0x00,
	0x03, 0x1C , 0x1D,
	0x03, 0x1D , 0x00,
	0x03, 0x1E , 0xFD,
	0x03, 0x1F , 0x00,
	0x03, 0x20 , 0xE1,
	0x03, 0x21 , 0x1A,
	0x03, 0x22 , 0xDE,
	0x03, 0x23 , 0x11,
	0x03, 0x24 , 0x1A,
	0x03, 0x25 , 0xEE,
	0x03, 0x26 , 0x50,
	0x03, 0x27 , 0x18,
	0x03, 0x28 , 0x25,
	0x03, 0x29 , 0x37,
	0x03, 0x2A , 0x24,
	0x03, 0x2B , 0x32,
	0x03, 0x2C , 0xA9,
	0x03, 0x2D , 0x32,
	0x03, 0x2E , 0xFF,
	0x03, 0x2F , 0x7F,
	0x03, 0x30 , 0xBA,
	0x03, 0x31 , 0x7F,
	0x03, 0x32 , 0x7F,
	0x03, 0x33 , 0x14,
	0x03, 0x34 , 0x81,
	0x03, 0x35 , 0x14,
	0x03, 0x36 , 0xFF,
	0x03, 0x37 , 0x20,
	0x03, 0x38 , 0x46,
	0x03, 0x39 , 0x04,
	0x03, 0x3A , 0x04,
	0x03, 0x3B , 0x00,
	0x03, 0x3C , 0x00,
	0x03, 0x3D , 0x00,

	//Do not change
	0x03, 0x3E , 0x03,
	0x03, 0x3F , 0x28,
	0x03, 0x40 , 0x02,
	0x03, 0x41 , 0x60,
	0x03, 0x42 , 0xAC,
	0x03, 0x43 , 0x97,
	0x03, 0x44 , 0x7F,
	0x04, 0x00 , 0xE8,
	0x04, 0x01 , 0x40,
	0x04, 0x02 , 0x00,
	0x04, 0x03 , 0x00,
	0x04, 0x04 , 0xF8,
	0x04, 0x05 , 0x03,
	0x04, 0x06 , 0x03,
	0x04, 0x07 , 0x85,
	0x04, 0x08 , 0x44,
	0x04, 0x09 , 0x1F,
	0x04, 0x0A , 0x40,
	0x04, 0x0B , 0x33,

	//Lens Shading Correction(Default Sunny)
	0x04, 0x0C , 0xA0,
	0x04, 0x0D , 0x00,
	0x04, 0x0E , 0x00,
	0x04, 0x0F , 0x00,
	0x04, 0x10 , 0x02,
	0x04, 0x11 , 0x02,
	0x04, 0x12 , 0x00,
	0x04, 0x13 , 0x00,
	0x04, 0x14 , 0x00,
	0x04, 0x15 , 0x00,
	0x04, 0x16 , 0x00,
	0x04, 0x17 , 0x04,
	0x04, 0x18 , 0x02,
	0x04, 0x19 , 0x02,
	0x04, 0x1a , 0x00,
	0x04, 0x1b , 0x00,
	0x04, 0x1c , 0x04,
	0x04, 0x1d , 0x04,
	0x04, 0x1e , 0x00,
	0x04, 0x1f , 0x00,
	0x04, 0x20 , 0x2A,
	0x04, 0x21 , 0x2A,
	0x04, 0x22 , 0x27,
	0x04, 0x23 , 0x22,
	0x04, 0x24 , 0x2A,
	0x04, 0x25 , 0x2A,
	0x04, 0x26 , 0x27,
	0x04, 0x27 , 0x22,
	0x04, 0x28 , 0x07,
	0x04, 0x29 , 0x07,
	0x04, 0x2a , 0x08,
	0x04, 0x2b , 0x06,
	0x04, 0x2c , 0x11,
	0x04, 0x2d , 0x11,
	0x04, 0x2e , 0x0F,
	0x04, 0x2f , 0x0D,
	0x04, 0x30 , 0x00,
	0x04, 0x31 , 0x00,
	0x04, 0x32 , 0x00,
	0x04, 0x33 , 0x00,
	0x04, 0x34 , 0x00,
	0x04, 0x35 , 0x00,
	0x04, 0x36 , 0x00,
	0x04, 0x37 , 0x00,
	0x04, 0x38 , 0x00,
	0x04, 0x39 , 0x00,
	0x04, 0x3a , 0x00,
	0x04, 0x3b , 0x00,
	0x04, 0x3c , 0x00,
	0x04, 0x3d , 0x00,
	0x04, 0x3e , 0x00,
	0x04, 0x3f , 0x00,

	//PWB Gain
	0x04, 0x40 , 0x00,
	0x04, 0x41 , 0x5F,
	0x04, 0x42 , 0x00,
	0x04, 0x43 , 0x00,
	0x04, 0x44 , 0x17,

	0x04, 0x45 , 0x00,
	0x04, 0x46 , 0x00,
	0x04, 0x47 , 0x00,
	0x04, 0x48 , 0x00,
	0x04, 0x49 , 0x00,
	0x04, 0x4A , 0x00,
	0x04, 0x4D , 0xE0,
	0x04, 0x4E , 0x05,
	0x04, 0x4F , 0x07,
	0x04, 0x50 , 0x00,
	0x04, 0x51 , 0x00,
	0x04, 0x52 , 0x00,
	0x04, 0x53 , 0x00,
	0x04, 0x54 , 0x00,
	0x04, 0x55 , 0x00,
	0x04, 0x56 , 0x00,
	0x04, 0x57 , 0x00,
	0x04, 0x58 , 0x00,
	0x04, 0x59 , 0x00,
	0x04, 0x5A , 0x00,
	0x04, 0x5B , 0x00,
	0x04, 0x5C , 0x00,
	0x04, 0x5D , 0x00,
	0x04, 0x5E , 0x00,
	0x04, 0x5F , 0x00,

	//GAMMA Correction
	0x04, 0x60 , 0x80,

	0x04, 0x61 , 0x10,
	0x04, 0x62 , 0x10,
	0x04, 0x63 , 0x10,
	0x04, 0x64 , 0x08,
	0x04, 0x65 , 0x08,
	0x04, 0x66 , 0x11,
	0x04, 0x67 , 0x09,
	0x04, 0x68 , 0x23,
	0x04, 0x69 , 0x2A,
	0x04, 0x6A , 0x2A,
	0x04, 0x6B , 0x47,
	0x04, 0x6C , 0x52,
	0x04, 0x6D , 0x42,
	0x04, 0x6E , 0x36,
	0x04, 0x6F , 0x46,
	0x04, 0x70 , 0x3A,
	0x04, 0x71 , 0x32,
	0x04, 0x72 , 0x32,
	0x04, 0x73 , 0x38,
	0x04, 0x74 , 0x3D,
	0x04, 0x75 , 0x2F,
	0x04, 0x76 , 0x29,
	0x04, 0x77 , 0x48,

	0x06, 0x86 , 0x6F,

	//Output Enable
	0x01, 0x00 , 0x01,
	0x01, 0x02 , 0x02,
	0x01, 0x04 , 0x03,

	///////////////////////////////////////////////////////////////////
	///////////////////////////GAMMA//////////////////////////////////
	///////////////////////////////////////////////////////////////////

	//-----------GAMMA Select(1)---------------//
	0x04, 0x61 , 0x00,
	0x04, 0x62 , 0x00,
	0x04, 0x63 , 0x00,
	0x04, 0x64 , 0x00,
	0x04, 0x65 , 0x00,
	0x04, 0x66 , 0x12,
	0x04, 0x67 , 0x3B,
	0x04, 0x68 , 0x34,
	0x04, 0x69 , 0x26,
	0x04, 0x6A , 0x1E,
	0x04, 0x6B , 0x33,
	0x04, 0x6C , 0x2E,
	0x04, 0x6D , 0x2C,
	0x04, 0x6E , 0x28,
	0x04, 0x6F , 0x42,
	0x04, 0x70 , 0x42,
	0x04, 0x71 , 0x38,
	0x04, 0x72 , 0x37,
	0x04, 0x73 , 0x4D,
	0x04, 0x74 , 0x48,
	0x04, 0x75 , 0x44,
	0x04, 0x76 , 0x40,
	0x04, 0x77 , 0x56,

	/*
	1:	//smallest gamma curve
		0x04, 0x61 , 0x00,
		0x04, 0x62 , 0x00,
		0x04, 0x63 , 0x00,
		0x04, 0x64 , 0x00,
		0x04, 0x65 , 0x00,
		0x04, 0x66 , 0x12,
		0x04, 0x67 , 0x3B,
		0x04, 0x68 , 0x34,
		0x04, 0x69 , 0x26,
		0x04, 0x6A , 0x1E,
		0x04, 0x6B , 0x33,
		0x04, 0x6C , 0x2E,
		0x04, 0x6D , 0x2C,
		0x04, 0x6E , 0x28,
		0x04, 0x6F , 0x42,
		0x04, 0x70 , 0x42,
		0x04, 0x71 , 0x38,
		0x04, 0x72 , 0x37,
		0x04, 0x73 , 0x4D,
		0x04, 0x74 , 0x48,
		0x04, 0x75 , 0x44,
		0x04, 0x76 , 0x40,
		0x04, 0x77 , 0x56,

	2:
		0x04, 0x61 , 0x00,
		0x04, 0x62 , 0x00,
		0x04, 0x63 , 0x00,
		0x04, 0x64 , 0x00,
		0x04, 0x65 , 0x00,
		0x04, 0x66 , 0x29,
		0x04, 0x67 , 0x37,
		0x04, 0x68 , 0x3A,
		0x04, 0x69 , 0x26,
		0x04, 0x6A , 0x21,
		0x04, 0x6B , 0x34,
		0x04, 0x6C , 0x34,
		0x04, 0x6D , 0x2B,
		0x04, 0x6E , 0x28,
		0x04, 0x6F , 0x41,
		0x04, 0x70 , 0x3F,
		0x04, 0x71 , 0x3A,
		0x04, 0x72 , 0x36,
		0x04, 0x73 , 0x47,
		0x04, 0x74 , 0x44,
		0x04, 0x75 , 0x3B,
		0x04, 0x76 , 0x3B,
		0x04, 0x77 , 0x4D,

	3:
		0x04, 0x61 , 0x00,
		0x04, 0x62 , 0x00,
		0x04, 0x63 , 0x00,
		0x04, 0x64 , 0x00,
		0x04, 0x65 , 0x00,
		0x04, 0x66 , 0x29,
		0x04, 0x67 , 0x4B,
		0x04, 0x68 , 0x41,
		0x04, 0x69 , 0x2A,
		0x04, 0x6A , 0x25,
		0x04, 0x6B , 0x3A,
		0x04, 0x6C , 0x2C,
		0x04, 0x6D , 0x2B,
		0x04, 0x6E , 0x28,
		0x04, 0x6F , 0x40,
		0x04, 0x70 , 0x3D,
		0x04, 0x71 , 0x38,
		0x04, 0x72 , 0x31,
		0x04, 0x73 , 0x44,
		0x04, 0x74 , 0x3E,
		0x04, 0x75 , 0x3E,
		0x04, 0x76 , 0x37,
		0x04, 0x77 , 0x43,

	4:
		0x04, 0x61 , 0x00,
		0x04, 0x62 , 0x00,
		0x04, 0x63 , 0x00,
		0x04, 0x64 , 0x00,
		0x04, 0x65 , 0x00,
		0x04, 0x66 , 0x2F,
		0x04, 0x67 , 0x4E,
		0x04, 0x68 , 0x50,
		0x04, 0x69 , 0x31,
		0x04, 0x6A , 0x27,
		0x04, 0x6B , 0x3C,
		0x04, 0x6C , 0x35,
		0x04, 0x6D , 0x27,
		0x04, 0x6E , 0x23,
		0x04, 0x6F , 0x46,
		0x04, 0x70 , 0x3A,
		0x04, 0x71 , 0x32,
		0x04, 0x72 , 0x32,
		0x04, 0x73 , 0x38,
		0x04, 0x74 , 0x3E,
		0x04, 0x75 , 0x36,
		0x04, 0x76 , 0x33,
		0x04, 0x77 , 0x41,

	5:	//largest gamma curve
		0x04, 0x61 , 0x00,
		0x04, 0x62 , 0x00,
		0x04, 0x63 , 0x00,
		0x04, 0x64 , 0x00,
		0x04, 0x65 , 0x15,
		0x04, 0x66 , 0x33,
		0x04, 0x67 , 0x61,
		0x04, 0x68 , 0x56,
		0x04, 0x69 , 0x30,
		0x04, 0x6A , 0x22,
		0x04, 0x6B , 0x3E,
		0x04, 0x6C , 0x2E,
		0x04, 0x6D , 0x2B,
		0x04, 0x6E , 0x28,
		0x04, 0x6F , 0x3C,
		0x04, 0x70 , 0x38,
		0x04, 0x71 , 0x2F,
		0x04, 0x72 , 0x2A,
		0x04, 0x73 , 0x3C,
		0x04, 0x74 , 0x34,
		0x04, 0x75 , 0x31,
		0x04, 0x76 , 0x31,
		0x04, 0x77 , 0x39,
	*/

	//-------------H_V_Switch(Normal)---------------//
	0x01, 0x01 , 0x02,//0x02,
			
	 /*GC2005_H_V_Switch,

	1:	// normal
		0x01, 0x01 , 0x00,
			
	2:	// IMAGE_H_MIRROR
		0x01, 0x01 , 0x01,
			
	3:	// IMAGE_V_MIRROR
		0x01, 0x01 , 0x02,
			
	4:	// IMAGE_HV_MIRROR
		0x01, 0x01 , 0x03,
	*/			
	//-------------H_V_Select End--------------//
};

/* GT2005 PREVIEW VGA */
static code UINT8 frontInit2CmdTbl[] = {
	#if 1
	0x02, 0x0B, 0x48,
	0x02, 0x0C, 0x64,
	0x04, 0x0B, 0x33,
	0x03, 0x00, 0x81,
	#endif
	
	/* 800 x 600 */
	0x01, 0x09, 0x00,
	0x01, 0x0A, 0x04,
	0x01, 0x0B, 0x0F,
	0x01, 0x10, 0x03,
	0x01, 0x11, 0x20,
	0x01, 0x12, 0x02,
	0x01, 0x13, 0x58,
	
	0x01, 0x1D , 0x01,
	0x01, 0x1E , 0x00,
	
	0x01, 0x19 , 0x01,
};

/* GT2005 SNAP 2M */
static code UINT8 frontInit3CmdTbl[] = {
	/* 1600 x 1200 */
	0x01, 0x09, 0x01,
	0x01, 0x0A, 0x00,
	0x01, 0x0B, 0x00,
	0x01, 0x10, 0x06,
	0x01, 0x11, 0x40,
	0x01, 0x12, 0x04,
	0x01, 0x13, 0xB0,

	0x01, 0x1D , 0x02,
	0x01, 0x1E , 0x00,
	
	0x01, 0x19 , 0x02,
};







static code UINT8 front30fpsPrevDz1xCmdTbl[1] = {
	0,
};
static UINT8 front30fpsPrevDz2xCmdTbl[1] = {
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

/* YUV */
static UINT8 frontPrevMode_YUV = 0;
static UINT8 frontSnapMode_YUV = 0;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

extern UINT8 burstsnaplightset;


  
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
	/*
		RAW、YUV两个不同的镜头在切换之前要重新调用此函数
		并且要将frontDevTot设置为0
	*/
	UINT8 id;
	frontOperatingFunc_t opFunc; 

	

	frontDevTot = 0;

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	} else {
		opFunc.expGainSetPos                           = EXP_GAIN_SET_POS_YUV;
		opFunc.gainAfterExp                            = GAIN_AFTER_EXP_YUV;
		opFunc.pfOpDevNameGet                          = frontOpDevNameGet;
		opFunc.pfOpFrameRateSet                        = frontOpFrameRateSet;
		opFunc.pfOpCapabilityGet                       = frontOpCapabilityGet;
		opFunc.pfOpIntrCapabilityGet                   = frontOpIntrCapabilityGet;
		opFunc.pfOpPowerOn                             = frontOpPowerOn;
		opFunc.pfOpPowerOff                            = frontOpPowerOff;
		opFunc.pfOpPreviewModeSet                      = frontOpPreviewModeSet;
		opFunc.pfOpPreviewSensorZoomModeSet            = frontOpPreviewSensorZoomModeSet;
	//	opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontOpIntrPreviewSensorZoomModeSet;
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
		// frontResTblBinRead(_gainTbl_YUV, __gainTbl);

		/* register extern operating function */
		id = frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));//call  the  function on top ?

		if ( id != NULL ) {
			frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET, frontOpAeTargetLuminanceSet);
			frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET, frontOpAeExposureCompensationSet);
			frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_FRAME_RATE_SET, frontOpAeFrameRateSet);
			frontExtendOpFuncSet(id, FRONT_EXT_OP_AWB_MODE_SET, frontOpAwbModeSet);
			frontExtendOpFuncSet(id, FRONT_EXT_OP_AFTER_SNAP_SHOT, frontOpAfterSnapShot);
		}

		return id;
	}
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
	#if SUPPORT_SENSOR_SWITCH
	return sensor_switch ? frontDevName : frontDevName_YUV;
	#else
	return frontDevName;
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		switch ( fps ) {
			case 30:
			case 25:
				//printf("frontOpFrameRateSet = 30\n");

				/* load ae tbl to static xdata frontAe_t aeTbl[130] _at_ 0xa000; */
				if (fps == 25) {
					//printf("50Hz\n");
					
					if (0 == frontPrevMode) {
						frontResTblBinRead(_ae30fps50hzTbl, __aeTbl);
					}
					else if (1 == frontPrevMode) {
						frontResTblBinRead(_ae15fps50hzTbl, __aeTbl);
					}
				}
				else {
					//printf("60Hz\n");
					
					if (0 == frontPrevMode) {
						frontResTblBinRead(_ae30fps60hzTbl, __aeTbl);
					}
					else if (1 == frontPrevMode) {
						frontResTblBinRead(_ae15fps60hzTbl, __aeTbl);
					}
				}   
				break;
		}
	#if SUPPORT_SENSOR_SWITCH
	} else {
		switch ( fps ) {
		case 30:
			//frontPrevMode = 0;
			//frontSnapMode = 0;
			break;
		}
	}
	#endif
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
		#if SUPPORT_SENSOR_SWITCH
		*(parg->ppprevCap) = sensor_switch ? &frontPrevCapabDesc[frontPrevMode] : &frontPrevCapabDesc_YUV[frontPrevMode_YUV];
		#else
		*(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
		#endif
	}

	if ( parg->ppsnapCap ) {
		#if SUPPORT_SENSOR_SWITCH
		*(parg->ppsnapCap) = sensor_switch ? &frontSnapCapabDesc[frontSnapMode] : &frontSnapCapabDesc_YUV[frontSnapMode_YUV];
		#else
		*(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
		#endif
		
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
		#if SUPPORT_SENSOR_SWITCH
		*(parg->ppprevCap) = sensor_switch ? &frontPrevCapabDesc[frontPrevMode] : &frontPrevCapabDesc_YUV[frontPrevMode_YUV];
		#else
		*(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
		#endif
	}

	if ( parg->ppsnapCap ) {
		#if SUPPORT_SENSOR_SWITCH
		*(parg->ppsnapCap) = sensor_switch ? &frontSnapCapabDesc[frontSnapMode] : &frontSnapCapabDesc_YUV[frontSnapMode_YUV];
		#else
		*(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
		#endif
		
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
	
	UINT16 addrt[] = {0xf0};
	UINT8 datat[] = {0XFF};
	
	UINT16 curSensorId = 0xffff;

	/* before power on. */
	frontBeforePowerOn();

	#if SUPPORT_SENSOR_SWITCH
	pcap = sensor_switch ? &(frontPrevCapabDesc[frontPrevMode]) : &(frontPrevCapabDesc_YUV[frontPrevMode_YUV]);
	#else
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	#endif
	

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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		/* set clk. */
		frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 3, 0);
		frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
		frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

		/* set DISABLE yuv interface. */
		frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, 0, 0, 0, 0, 0);
		frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 0, 0, 0, 0, 0);

		/* set i2c. */
		i2cDeviceAddrSet(0x20);
		i2cRegClkCfgSet(I2C_REG_CLK_DIV);
		i2cClkCfgSet(I2C_CLK_DIV);	

		/* send i2c command. */
		globalTimerWait(300);
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		/* set clk. */
		frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC_YUV, pcap->mclkDiv, pcap->mclkPhase, 0, 0);//use the mclk and pclk
		frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC_YUV, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
		frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV_YUV, VSYNC_INV_PREV_YUV, 0, 0, 0);

		/* set yuv interface. */
		frontParaSet(FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG, YUV_SEQ, 0, 0, 0, 0);
		frontParaSet(FRONT_PARA_YUV_CCIR_MODE, CCIR_MODE, 0, 0, 0, 0);
		frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, Y_SUB_128, U_SUB_128, V_SUB_128, 0, 0);
		frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 1, 0, 0, 0, 0);

		/* set i2c. */
		i2cDeviceAddrSet(I2C_DEV_ADDR_YUV);
		i2cClkCfgSet(I2C_CLK_DIV_YUV);	

		/* send i2c command. */
		frontResCmdBinSend(_frontInit0CmdTbl);
		frontResCmdBinSend(_frontInit1CmdTbl);

		#if 0
		i2cDeviceAddrSet(I2C_DEV_ADDR_YUV | 0x01);
		addrt[0] = 0x0000;
		i2cReg16Read(addrt,datat,1,1,1,0,0);
		curSensorId = datat[0];

		addrt[0] = 0x0001;
		i2cReg16Read(addrt,datat,1,1,1,0,0);
		curSensorId = curSensorId<<8;
		curSensorId |= datat[0];
		
		printf("=============================Cur Sensor ID : 0x%04X\n", curSensorId);
		i2cDeviceAddrSet(I2C_DEV_ADDR_YUV);
		#endif
	}
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
	frontPrevCapabDesc_t *pcap;

	#if SUPPORT_SENSOR_SWITCH
	pcap = sensor_switch ? &(frontPrevCapabDesc[frontPrevMode]) : &(frontPrevCapabDesc_YUV[frontPrevMode_YUV]);
	#else
	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	#endif

	__FUNC_TRACK__;
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */
	__FUNC_TRACK__;

	#if SUPPORT_SENSOR_SWITCH
	frontParaSet(FRONT_PARA_MCLK_CFG, sensor_switch ? MCLK_SRC : MCLK_SRC_YUV, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, sensor_switch ? PCLK_SRC : PCLK_SRC_YUV, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, sensor_switch ? HSYNC_INV_PREV : HSYNC_INV_PREV_YUV, sensor_switch ? VSYNC_INV_PREV : VSYNC_INV_PREV_YUV, 0, 0, 0);
	#else
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE,HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	#endif
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		if ( frontPrevMode == 0 ) {  /* 30 fps. */
			frontResCmdBinSend(_front30fpsPrevCmdTbl);
		}
		else if ( frontPrevMode == 1 ) {  /* 15 fps. */
			frontResCmdBinSend(_front15fpsPrevCmdTbl);
		}
		//printf("frontPrevMode %d\n", (UINT16)frontPrevMode);

		/* Preview Mode Switch Post Proc */
		frontPreviewModeSwitchPostProc();

		cdspBayerPatternSet(BAYER_PTN_PREV);
		
		__FUNC_TRACK__;
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	/* not need to wait timing */
		__FUNC_TRACK__;
	#if SUPPORT_SENSOR_SWITCH
	} else {

		if (frontPrevMode_YUV == 0)
			frontResCmdBinSend(_frontInit2CmdTbl);
		
		
		cdspBayerPatternSet(BAYER_PTN_PREV_YUV);

	}
	#endif
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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
			else if ( frontPrevMode == 1 ) {  /* 15 fps. */
				factor = factor;
				pvGainIdxVal = pvGainIdxVal;
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
			else if ( frontPrevMode == 1 ) {  /* 15 fps. */
				factor = factor;
				pvGainIdxVal = pvGainIdxVal;
			}
		}
	#if SUPPORT_SENSOR_SWITCH
	} else {
		factor = factor;
	}
	#endif
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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
			else if ( frontPrevMode == 1 ) {  /* 15 fps. */
				factor = factor;
				pvGainIdxVal = pvGainIdxVal;
				paraAry = paraAry;
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
			else if ( frontPrevMode == 1 ) {  /* 15 fps. */
				factor = factor;
				pvGainIdxVal = pvGainIdxVal;
				paraAry = paraAry;
			}
		}
	#if SUPPORT_SENSOR_SWITCH
	} else {
		factor = factor;
	}
	#endif
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

	#if SUPPORT_SENSOR_SWITCH
	pcap = sensor_switch ? &(frontSnapCapabDesc[frontSnapMode]) : &(frontSnapCapabDesc_YUV[frontSnapMode_YUV]);

	frontParaSet(FRONT_PARA_MCLK_CFG, sensor_switch ? MCLK_SRC : MCLK_SRC_YUV, pcap->mclkDiv, pcap->mclkPhase, 0, 0);	
	frontParaSet(FRONT_PARA_PCLK_CFG, sensor_switch ? PCLK_SRC : PCLK_SRC_YUV, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, sensor_switch ? HSYNC_INV_SNAP : HSYNC_INV_SNAP_YUV, sensor_switch ? VSYNC_INV_SNAP : VSYNC_INV_SNAP_YUV, 0, 0, 0);
	#else
	pcap = &(frontSnapCapabDesc[frontSnapMode]);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);	
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
	#endif
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

	#if SUPPORT_SENSOR_SWITCH
	if (!sensor_switch) {
		frontResCmdBinSend(_frontInit3CmdTbl);
	}
	#endif
	
	#if SUPPORT_SENSOR_SWITCH
	cdspBayerPatternSet(sensor_switch ? BAYER_PTN_SNAP : BAYER_PTN_SNAP_YUV);
	#else
	cdspBayerPatternSet(BAYER_PTN_SNAP);
	#endif

	#if SUPPORT_SENSOR_SWITCH
	if (!sensor_switch) {
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);///
	}
	#endif
}


void frontOpSnapNegativeEffect(void){
	UINT8 cmdTblYuvEffect[3];

	cmdTblYuvEffect[0] = 0x01;
	cmdTblYuvEffect[1] = 0x15;
	cmdTblYuvEffect[2] = 0x07;

	#if SUPPORT_SENSOR_SWITCH
	if(!sensor_switch)
		i2cCmdTableSend(cmdTblYuvEffect, 2, sizeof(cmdTblYuvEffect)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	#endif
}
void frontOpSnapNormalEffect(void){
	UINT8 cmdTblYuvEffect[3];

	cmdTblYuvEffect[0] = 0x01;
	cmdTblYuvEffect[1] = 0x15;
	cmdTblYuvEffect[2] = 0x00;

	#if SUPPORT_SENSOR_SWITCH
	if(!sensor_switch)
		i2cCmdTableSend(cmdTblYuvEffect, 2, sizeof(cmdTblYuvEffect)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	#endif
}
void GT2005_EV(UINT8 expval) {
	UINT8 ev_reg[9];

	ev_reg[0] = 0x03;
	ev_reg[1] = 0x01;
	ev_reg[2] = 0x81;
	
	ev_reg[3] = 0x03;
	ev_reg[4] = 0x01;
	ev_reg[5] = 0x80;

	ev_reg[6] = 0x02;
	ev_reg[7] = 0x01;
	ev_reg[8] = 0x18;//0x1e;
	
	switch(expval)
	{
		case 0:// PRV_COMP_N20EV:
			ev_reg[5] = 0x20;
			ev_reg[8] = 0x80;
			break;
		case 1:// PRV_COMP_N17EV:
			ev_reg[5] = 0x30;
			ev_reg[8] = 0x90;
			break;
		case 2:// PRV_COMP_N13EV:
			ev_reg[5] = 0x40;
			ev_reg[8] = 0xa0;
			break;
		case 3:// PRV_COMP_N10EV:
			ev_reg[5] = 0x50;
			ev_reg[8] = 0xb0;
			break;
		case 4:// PRV_COMP_N07EV:
			ev_reg[5] = 0x60;
			ev_reg[8] = 0xc0;
			break;
		case 5:// PRV_COMP_N03EV:
			ev_reg[5] = 0x70;
			ev_reg[8] = 0xd0;
			break;
		case 6:// PRV_COMP_0EV:
			ev_reg[5] = 0x80;
			ev_reg[8] = 0x18;//0x1e;
			break;
		case 7:// PRV_COMP_P03EV:
			ev_reg[5] = 0x90;
			ev_reg[8] = 0x20;
			break;
		case 8:// PRV_COMP_P07EV:
			ev_reg[5] = 0xa0;
			ev_reg[8] = 0x30;
			break;
		case 9:// PRV_COMP_P10EV:
			ev_reg[5] = 0xb0;
			ev_reg[8] = 0x40;
			break;
		case 10:// PRV_COMP_P13EV:
			ev_reg[5] = 0xc0;
			ev_reg[8] = 0x50;
			break;
		case 11:// PRV_COMP_P17EV:
			ev_reg[5] = 0xd0;
			ev_reg[8] = 0x60;
			break;
		case 12:// PRV_COMP_P20EV:
			ev_reg[5] = 0xe0;
			ev_reg[8] = 0x70;
			break;
	}

	i2cCmdTableSend(ev_reg, 2, sizeof(ev_reg)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
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
UINT16 front_Get_2005Light(){
	UINT16 addr;
	UINT8 datatbl;
	UINT16 sensorLight;
#if 1
		i2cDeviceAddrSet(I2C_DEV_ADDR|0x01);
		i2cClkCfgSet(I2C_CLK_DIV);
		
		addr = 0x0010;
		i2cReg16Read(&addr, &datatbl, 1, 1, 1, 0, 0);
		sensorLight = datatbl << 8;
		addr = 0x0011;
		i2cReg16Read(&addr, &datatbl, 1, 1, 1, 0, 0);
		sensorLight |= datatbl;
		i2cDeviceAddrSet(I2C_DEV_ADDR);
		i2cClkCfgSet(I2C_CLK_DIV);
#endif

return sensorLight;
}

__STATIC void
frontOpSnapShot(
	UINT8 first,
	UINT8 mode,
	UINT8 scaleUp
)
{

	//printf("first=%bd,mode=%bd\n",first,mode);
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	} else {
		UINT16 shutter, AGain_shutter, DGain_shutter;
		UINT8 cmdTblYuv[3*10], regVal;
		UINT16 regAddr;
		
		first = first;
		mode = mode;
		scaleUp = scaleUp;

		
		if (!burstsnaplightset) {
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
			cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
			cdspSkipWrSyncSet(1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

			HAL_CdspEofWait(1);
		} else {
			frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			

			cmdTblYuv[0] = 0x02;
			cmdTblYuv[1] = 0x0b;
			cmdTblYuv[2] = 0x48;	/* 越大边缘加强越明显 */

			cmdTblYuv[3] = 0x02;
			cmdTblYuv[4] = 0x0c;
			cmdTblYuv[5] = 0x64;	/* 越大边缘加强越明显 */

			cmdTblYuv[6] = 0x04;
			cmdTblYuv[7] = 0x0b;
			cmdTblYuv[8] = 0x33;	/* 越大去噪约明显 */
			
			cmdTblYuv[9] = 0x03;
			cmdTblYuv[10] = 0x00;
			cmdTblYuv[11] = 0xc1;


			i2cDeviceAddrSet(I2C_DEV_ADDR_YUV | 0x01);
			
			regAddr = 0x0012;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			shutter = regVal;
			regAddr = 0x0013;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			shutter = (shutter << 8) | regVal;


			regAddr = 0x0014;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			AGain_shutter = regVal;
			regAddr = 0x0015;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			AGain_shutter = (AGain_shutter << 8) | regVal;


			regAddr = 0x0016;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			DGain_shutter = regVal;
			regAddr = 0x0017;
			i2cReg16Read(&regAddr, &regVal, 1,1,1,0,0);
			DGain_shutter = (DGain_shutter << 8) | regVal;

			i2cDeviceAddrSet(I2C_DEV_ADDR_YUV);


			cmdTblYuv[12] = 0x03;
			cmdTblYuv[13] = 0x00;
			cmdTblYuv[14] = 0x41;

			shutter = shutter / 3; 

			cmdTblYuv[15] = 0x03;
			cmdTblYuv[16] = 0x05;
			cmdTblYuv[17] = (UINT8)(shutter & 0xff);
			cmdTblYuv[18] = 0x03;
			cmdTblYuv[19] = 0x04;
			cmdTblYuv[20] = (UINT8)((shutter >> 8) & 0xff);

			cmdTblYuv[21] = 0x03;
			cmdTblYuv[22] = 0x07;
			cmdTblYuv[23] = (UINT8)(AGain_shutter & 0xff);
			cmdTblYuv[24] = 0x03;
			cmdTblYuv[25] = 0x06;
			cmdTblYuv[26] = (UINT8)((AGain_shutter >> 8) & 0xff);

			cmdTblYuv[27] = 0x03;
			cmdTblYuv[28] = 0x08;
			cmdTblYuv[29] = (UINT8)(DGain_shutter & 0xff);

			i2cCmdTableSend(cmdTblYuv, 2, sizeof(cmdTblYuv)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
			
			frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
			cdspSkipWrSyncSet(1);
			frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			frontSnapSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
			frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
			HAL_CdspEofWait(1);
		}
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		//printf("gain idx=%bu\n",val);

		pcap = &(frontPrevCapabDesc[frontPrevMode]);
		frontLastGain = pcap->gainTbl[val];
		frontLastGainIdx = val;

		//printf("gain set frontLastGain=%d\n",frontLastGain);

		cmdTbl[0] = 0x02;
		cmdTbl[1] = 0x04;
		cmdTbl[2] = (UINT8)((frontLastGain & 0xFF00) >> 8);
		
		cmdTbl[3] = 0x02;
		cmdTbl[4] = 0x05;
		cmdTbl[5] = (UINT8)(frontLastGain & 0x00FF);

		
		//printf("cmdTbl[2]=0x%bx\n",cmdTbl[2]);
		
		i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
		
		//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
		//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		val = val;
		isr = isr;
		upd = upd;
	}
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
	UINT8 cmdTbl[2*3];
	//UINT16 REG_ADDR16[3]={0x3500, 0x3501, 0x3502};
	//UINT8 REG_ADDR[3];

	//Manual Exposure Ctrl First Set REG bit 0x3503[0] to Enable
	//Exposure Value = 0x3500[19:16], 0x3501[15:8], 0x3502[7:0], units of 1/16 line

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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

		cmdTbl[0] = 0x02;
		cmdTbl[1] = 0x02;
		cmdTbl[2] = (UINT8)((espline & 0xff00) >> 8);
		
		cmdTbl[3] = 0x02;
		cmdTbl[4] = 0x03;
		cmdTbl[5] = (UINT8)(espline);
		
		i2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

		//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,3,1,1,0); 
		//printf("espline=%d\n",espline);
		//printf("read=0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2]);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		parg = parg;
	}
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		pcap = &(frontPrevCapabDesc[frontPrevMode]);

		#if 1
		switch (frontPrevDyTdSwitchGain) {
			case 1:
				//printf("%bu,G -\n",val);
				switch (frontPrevMode) {
					case 0:
					case 1:
						if (val >= 1) {
							val -= 1;
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
					case 1:
						if (val < 48) {
							val += 1;
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

		cmdTbl[0] = 0x02;
		cmdTbl[1] = 0x04;
		cmdTbl[2] = (UINT8)((frontLastGain & 0xFF00) >> 8);
		
		cmdTbl[3] = 0x02;
		cmdTbl[4] = 0x05;
		cmdTbl[5] = (UINT8)(frontLastGain & 0x00FF);
		
		i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);
		
		//i2cReg16Read(&ADDR16,&cmdTbl[2],1,1,1,1,0); 
		//printf("cmdTbl[2]=%bd\n",cmdTbl[2]);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		val = val;
		isr = isr;
		upd = upd;
	}
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
	UINT8 cmdTbl[2*3];
	//UINT16 REG_ADDR16[3]={0x3500, 0x3501, 0x3502};
	//UINT8 REG_ADDR[3];

	//Manual Exposure Ctrl First Set REG bit 0x3503[0] to Enable
	//Exposure Value = 0x3500[19:16], 0x3501[15:8], 0x3502[7:0], units of 1/16 line

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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

		cmdTbl[0] = 0x02;
		cmdTbl[1] = 0x02;
		cmdTbl[2] = (UINT8)((espline & 0xff00) >> 8);
		
		cmdTbl[3] = 0x02;
		cmdTbl[4] = 0x03;
		cmdTbl[5] = (UINT8)(espline);
		
		i2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

		//i2cReg16Read(&REG_ADDR16[0],&REG_ADDR[0],1,3,1,1,0); 
		//printf("espline=%d\n",espline);
		//printf("read=0x%bx,0x%bx,0x%bx\n",REG_ADDR[0],REG_ADDR[1],REG_ADDR[2]);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		parg = parg;
	}
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	} else {
		en =en;
	}
	#endif
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
	/*Reset*/
	gpioByteFuncSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));  //gpio 0 reset 小镜头
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		#if (RES_LSC_LOAD == 1)
		/* resource file lsc cmd table send */
		frontResCmdBinSend(FRONT_RES_PARAM_LSC_ID);
		#endif
	#if SUPPORT_SENSOR_SWITCH
	}
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
	UINT8 cmdTbl[3*4];
	UINT16 s_gain;
	#if (SNAP_KEEP_FPS == 0)
	UINT16 compensation_line = 0;
	#endif
	CaptureHeaderInformation cap;

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
		//printf("espline_16=%lu\n",espline_16);



		/* exposure time set */
		cmdTbl[0] = 0x02;
		cmdTbl[1] = 0x02;
		cmdTbl[2] = (UINT8)((espline & 0xff00) >> 8);
		
		cmdTbl[3] = 0x02;
		cmdTbl[4] = 0x03;
		cmdTbl[5] = (UINT8)(espline);

		//printf("pre-s_gain=%d\n",s_gain);
		
		/* gain set */
		cmdTbl[6] = 0x02;
		cmdTbl[7] = 0x04;
		cmdTbl[8] = (UINT8)((s_gain & 0xFF00) >> 8);
		
		cmdTbl[9] = 0x02;
		cmdTbl[10] = 0x05;
		cmdTbl[11] = (UINT8)(s_gain & 0x00FF);

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
	#if SUPPORT_SENSOR_SWITCH
	}
	#endif
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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
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

		#if 1
		/* pv need move gain for sensor auto ob & snap to pv sensor shutter need re-setting */
		LoadBackfrontLastSetting = 1;
		#endif

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
	#if SUPPORT_SENSOR_SWITCH
	}
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		#if 0
		*numerator = (16+(value&0x000F))*((value&0x0010)?2:1)*((value&0x0020)?2:1)*((value&0x0040)?2:1);
		*denominator = 16;
		#else
		*numerator = (16+(value&0x000F));
		*denominator = (((16/((value&0x0010)?2:1))/((value&0x0020)?2:1))/((value&0x0040)?2:1));	
		#endif
	#if SUPPORT_SENSOR_SWITCH
	}
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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
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
	#if SUPPORT_SENSOR_SWITCH
	}
	#endif
}

/*
	0: VGA 30FPS
	1: 2M 15FPS (需要开启FUNC_HOST_1080P_SCALE功能才能使用此MODE)
*/
void appFrontPreviewModeSet(UINT8 mode) {
	if (mode >= PREV_MODE_TOT)
		mode = 0;

	frontPrevMode = mode;
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

	#if SUPPORT_SENSOR_SWITCH
	if (sensor_switch) {
	#endif
		sts = i2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	#if SUPPORT_SENSOR_SWITCH
	} else {
		sts = i2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
	#endif

	if (sts == FAIL) {
		//printf("Sensor command table (0x%x)\n",fileId);
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
		/*
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
		*/
		case _frontSnapCmdTbl:
			pData = frontSnapCmdTbl;
			size = sizeof(frontSnapCmdTbl);
			break;
	}

	
	sts = i2cCmdTableSend(pData, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	if (sts == FAIL) {
		//printf("Sensor command table (0x%x)\n",fileId);
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
#if 0
void frontOpSnapNegativeEffect(void){
	UINT8 cmdTblYuvEffect[3];

	cmdTblYuvEffect[0] = 0x01;
	cmdTblYuvEffect[1] = 0x15;
	cmdTblYuvEffect[2] = 0x07;
	
	i2cCmdTableSend(cmdTblYuvEffect, 2, sizeof(cmdTblYuvEffect)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	
}
void frontOpSnapNormalEffect(void){
	UINT8 cmdTblYuvEffect[3];

	cmdTblYuvEffect[0] = 0x01;
	cmdTblYuvEffect[1] = 0x15;
	cmdTblYuvEffect[2] = 0x00;
	
	i2cCmdTableSend(cmdTblYuvEffect, 2, sizeof(cmdTblYuvEffect)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	
}
#endif
