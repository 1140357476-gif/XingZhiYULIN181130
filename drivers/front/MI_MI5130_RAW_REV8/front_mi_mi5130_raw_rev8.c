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
#include "sp1k_ae_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "sp1k_front_api.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__

#define LOAD_RES_OPTION				1	/* load resource option, 1 : resource mode 0 : code mode */
#define REGISTER_EXTERN_OP_FUNC		0	/* used register extern operating function */

#define DBG_SNAP_TIMING		0	/* debug for snapshot timing(RDK), used GPIO12, GPIO13; (EVB), used TP_YN & TP_XN GPIO */
 
#define RES_LSC_LOAD		0//after power on load lsc resource file
#define PV_AUTO_OB_BY_GAIN	0//pv sensor auto ob by frame : 0, pv sensor auto ob by gain : 1
 
/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  1

/* dimensiion. */

/* 960p, 30fps */
#define WIDTH_30FPS_PREV      1280
#define HEIGHT_30FPS_PREV     960
#define XOFF_30FPS_PREV       2
#define YOFF_30FPS_PREV       2
#define LINE_TOT_30FPS_PREV   1680//1668//1680//1669//1885//1922//1908//2460//3791
#define FRAME_TOT_30FPS_PREV  1147//1043//1148//1133//1153//1148//1125//1043//1055
#define PREV_TO_SNAP_BINNING_RATE_30FPS_PREV  2

/* 480p, 60fps */
#define WIDTH_60FPS_PREV      640//636
#define HEIGHT_60FPS_PREV     480//476 
#define XOFF_60FPS_PREV       2
#define YOFF_60FPS_PREV       2
#define LINE_TOT_60FPS_PREV   1658//1461//1462//471//1456//1458
#define FRAME_TOT_60FPS_PREV  571//576//571//580//570//612//612//567//686	
#define PREV_TO_SNAP_BINNING_RATE_60FPS_PREV  1

/* 5M */
#define WIDTH_SNAP      2592 
#define HEIGHT_SNAP     1944 
#define XOFF_SNAP       2
#define YOFF_SNAP       2
#define LINE_TOT_SNAP   2693//2690//2692//2673//5214//2352//2100
#define FRAME_TOT_SNAP  2094//2146//1552

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_INT
#define PCLK_SRC  FRONT_PCLK_SRC_EXT

#define MCLK_DIV_30FPS_PREV    26//11 
#define MCLK_PHASE_30FPS_PREV  0
#define MCLK_DRV_30FPS_PREV    1
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  7

#define MCLK_DIV_60FPS_PREV    26//10 
#define MCLK_PHASE_60FPS_PREV  0
#define MCLK_DRV_60FPS_PREV    1
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  7

#define MCLK_DIV_SNAP           26//11 
#define MCLK_DIV_SNAP_BURST     18
#define MCLK_DIV_SNAP_BURST_UP  18
#define MCLK_PHASE_SNAP         0
#define MCLK_DRV_SNAP           1
#define PCLK_DIV_SNAP           1
#define PCLK_PHASE_SNAP         0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV     55220000L//63700000L//64062000L//70700000L//24000000L////30000000L
#define PCLK_FREQ_30FPS_PREV_TD     55220000L//63700000L//64062000L//70700000L//24000000L////30000000L
#define PCLK_FREQ_60FPS_PREV     55220000L////30000000L
#define PCLK_FREQ_60FPS_PREV_TD     55220000L////30000000L
#define PCLK_FREQ_SNAP           55220000L//24000000L//30000000L
#define PCLK_FREQ_SNAP_BURST     41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST+1) */
#define PCLK_FREQ_SNAP_BURST_UP  41052631L   /* PCLK_FREQ_30FPS_PREV * (MCLK_DIV_SNAP+1)/(MCLK_DIV_SNAP_BURST_UP+1) */
#else
#define PCLK_FREQ_30FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_30FPS_PREV_TD     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_60FPS_PREV_TD     522000000L / 2L / PREV_MCLKDIV / PREV_PCLKDIV
#define PCLK_FREQ_SNAP           522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST     522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#define PCLK_FREQ_SNAP_BURST_UP  522000000L / 2L / SNAP_MCLKDIV / SNAP_PCLKDIV
#endif

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HEN_60FPS_PREV    0
#else
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV  1
#define RESHAPE_HRISE_30FPS_PREV  2
#define RESHAPE_HFALL_60FPS_PREV  1
#define RESHAPE_HRISE_60FPS_PREV  2
#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif
#define RESHAPE_VFALL_30FPS_PREV  1
#define RESHAPE_VRISE_30FPS_PREV  2
#define RESHAPE_VFALL_60FPS_PREV  1
#define RESHAPE_VRISE_60FPS_PREV  2

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  1
#define RESHAPE_HRISE_SNAP  2
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  1
#define RESHAPE_VRISE_SNAP  2

/* preview h,v sync inv. */
#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  0

/* snapshot h,v sync inv. */
#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  FRONT_BAYER_PTN_BGBGRR//0x02
#define BAYER_PTN_SNAP  FRONT_BAYER_PTN_BGBGRR//0x02

/* i2c interface. */
#define I2C_DEV_ADDR  0x20
#define I2C_CLK_DIV   4//0x01

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  121
#define AE_30FPS_30_MIN_IDX  0//6
#define AE_30FPS_25_MAX_IDX  121
#define AE_30FPS_25_MIN_IDX  0//6
#define EV10_30FPS_EXP_IDX   90


#define AE_60FPS_60_MAX_IDX  121
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  121
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   90


/* gain. */
#define GAIN_30FPS_MAX_IDX  223
#define GAIN_30FPS_MIN_IDX  0

#define GAIN_60FPS_MAX_IDX  223
#define GAIN_60FPS_MIN_IDX  1

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
static code UINT8 frontDevName[] = "SENSOR - MI MI5130 RAW REV8";
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

/**************************************************************************
 *                            AE TABLE
 **************************************************************************/
/**************************************************************************
 *				banding calculate
 *			50Hz : 1/(shutter/10) < 1/(2*50)
 *			60Hz : 1/(shutter/10) < 1/(2*60)
 **************************************************************************/
/* ae table. */
/*static code frontAe_t ae20fps50hzTbl[1] = {
	{      0,    0,     0,      0,0 }
};
static code frontAe_t ae20fps60hzTbl[1] = {
	{	   0,	 0, 	0,		0,0 }
};*/
static code frontAe_t ae30fps50hzTbl[147] = { 
	{    0,     332,  254, 0,  0 },
	{    1,     332,  248, 0,  0 },
	{    2,     332,  242, 0,  0 },
	{    3,     332,  236, 0,  0 },
	{    4,     332,  231, 0,  0 },
	{    5,     332,  227, 0,  0 },
	{    6,     332,  220, 0,  0 },
	{    7,     332,  212, 0,  0 },
	{    8,     332,  204, 0,  0 },
	{    9,     332,  197, 0,  0 },
	{   10,     332,  190, 0,  0 },
	{   11,     332,  184, 0,  0 },
	{   12,     332,  178, 0,  0 },
	{   13,     332,  172, 0,  0 },
	{   14,     332,  167, 0,  0 },
	{   15,     332,  163, 0,  0 },
	{   16,     332,  156, 0,  0 },
	{   17,     332,  148, 0,  0 },
	{   18,     332,  140, 0,  0 },
	{   19,     332,  133, 0,  0 },
	{   20,     332,  126, 0,  0 },
	{   21,     332,  120, 0,  0 },
	{   22,     332,  114, 0,  0 },
	{   23,     332,  108, 0,  0 },
	{   24,     332,  103, 0,  0 },
	{   25,     332,  99, 0,  0 },
	{   26,     332,  92, 0,  0 },
	{   27,     332,  84, 0,  0 },
	{   28,     332,  76, 0,  0 },
	{   29,     332,  69, 0,  0 },
	{   30,     332,  62, 0,  0 },
	{   31,     332,  56, 0,  0 },
	{   32,     332,  50, 0,  0 },
	{   33,     332,  44, 0,  0 },
	{   34,     332,  39, 0,  0 },
	{   35,     332,  35, 0,  0 },
	{   36,     332,  30, 0,  0 },
	{   37,     332,  26, 0,  0 },
	{   38,     332,  22, 0,  0 },
	{   39,     332,  19, 0,  0 },
	{   40,     332,  15, 0,  0 },
	{   41,     332,  12, 0,  0 },
	{   42,     332,   9, 0,  0 },
	{   43,     332,   6, 0,  0 },
	{   44,     332,   4, 0,  0 },
	{   45,     332,   2, 0,  0 },
	{   46,     332,   0, 0,  0 },
	{   47,     498,  12, 0,  0 },
	{   48,     498,   9, 0,  0 },
	{   49,     498,   6, 0,  0 },
	{   50,     498,   4, 0,  0 },
	{   51,     498,   1, 0,  0 },
	{   52,     498,   0, 0,  0 },
	{   53,     990,  25, 0,  0 },
	{   54,     990,  21, 0,  0 },
	{   55,     990,  18, 0,  0 },
	{   56,     990,  14, 0,  0 },
	{   57,     990,  11, 0,  0 },
	{   58,     990,   8, 0,  0 },
	{   59,     990,   6, 0,  0 },
	{   60,     990,   3, 0,  0 },
	{   61,     990,   1, 0,  0 },
	{   62,     990,   0, 0,  0 },
	{   63,    1058,   0, 0,  0 },
	{   64,    1135,   0, 0,  0 },
	{   65,    1215,   0, 0,  0 },
	{   66,    1303,   0, 0,  0 },
	{   67,    1393,   0, 0,  0 },
	{   68,    1496,   0, 0,  0 },
	{   69,    1601,   0, 0,  0 },
	{   70,    1722,   0, 0,  0 },
	{   71,    1842,   0, 0,  0 },
	{   72,    1981,   0, 0,  0 },
	{   73,    2115,   0, 0,  0 },
	{   74,    2269,   0, 0,  0 },
	{   75,    2430,   0, 0,  0 },
	{   76,    2616,   0, 0,  0 },
	{   77,    2786,   0, 0,  0 },
	{   78,    3006,   0, 0,  0 },
	{   79,    3202,   0, 0,  0 },
	{   80,    3461,   0, 0,  0 },
	{   81,    3684,   0, 0,  0 },
	{   82,    3984,   0, 0,  0 },
	{   83,    4230,   0, 0,  0 },
	{   84,    4568,   0, 0,  0 },
	{   85,    4895,   0, 0,  0 },
	{   86,    5271,   0, 0,  0 },
	{   87,    5617,   0, 0,  0 },
	{   88,    6011,   0, 0,  0 },
	{   89,    6465,   0, 0,  0 },
	{   90,    6993,   0, 0,  0 },
	{   91,    7449,   0, 0,  0 },
	{   92,    7968,   0, 0,  0 },
	{   93,    8566,   0, 0,  0 },
	{   94,    9260,   0, 0,  0 },
	{   95,    9790,   0, 0,  0 },
	{   96,   10707,   0, 0,  0 },
	{   97,   11421,   0, 0,  0 },
	{   98,   12237,   0, 0,  0 },
	{   99,   13178,   0, 0,  0 },
	{  100,   14276,   0, 0,  0 },
	{  101,   14897,   0, 0,  0 },
	{  102,   16316,   0, 0,  0 },
	{  103,   17132,   0, 0,  1 },
	{  104,   19035,   0, 0,  1 },
	{  105,   20155,   0, 0,  0 },
	{  106,   21415,   0, 0,  1 },
	{  107,   22842,   0, 0,  0 },
	{  108,   24474,   0, 0,  1 },
	{  109,   26356,   0, 0,  0 },
	{  110,   28553,   0, 0,  1 },
	{  111,   31149,   0, 0,  1 },
	{  112,   34263,   1, 0,  1 },
	{  113,   34263,   0, 0,  1 },
	{  114,   38070,   0, 0,  1 },
	{  115,   42829,   2, 0,  2 },
	{  116,   42829,   0, 0,  2 },
	{  117,   48947,   2, 0,  1 },
	{  118,   48947,   0, 0,  1 },
	{  119,   57105,   2, 0,  2 },
	{  120,   57105,   0, 0,  2 },
	{  121,   68526,   4, 0,  1 },
	{  122,   68526,   1, 0,  1 },
	{  123,   68526,   0, 0,  1 },
	{  124,   85657,   4, 0,  3 },
	{  125,   85657,   2, 0,  3 },
	{  126,   85657,   0, 0,  3 },
	{  127,  114208,   7, 0,  2 },
	{  128,  114208,   5, 0,  2 },
	{  129,  114208,   2, 0,  2 },
	{  130,  114208,   0, 0,  2 },
	{  131,  171310,  12, 0,  8 },
	{  132,  171310,   9, 0,  8 },
	{  133,  171310,   7, 0,  8 },
	{  134,  171310,   4, 0,  8 },
	{  135,  171310,   2, 0,  8 },
	{  136,  171310,   0, 0,  8 },
	{  137,  342603,  26, 0,  8 },
	{  138,  342603,  23, 0,  8 },
	{  139,  342603,  19, 0,  8 },
	{  140,  342603,  16, 0,  8 },
	{  141,  342603,  12, 0,  8 },
	{  142,  342603,   9, 0,  8 },
	{  143,  342603,   7, 0,  8 },
	{  144,  342603,   4, 0,  8 },
	{  145,  342603,   2, 0,  8 },
	{  146,  342603,   0, 0,  8 },
};

static code frontAe_t ae30fps60hzTbl[148] = {
	{    0,     300,  251, 0,  0 },
	{    1,     300,  245, 0,  0 },
	{    2,     300,  239, 0,  0 },
	{    3,     300,  234, 0,  0 },
	{    4,     300,  229, 0,  0 },
	{    5,     300,  224, 0,  0 },
	{    6,     300,  216, 0,  0 },
	{    7,     300,  208, 0,  0 },
	{    8,     300,  200, 0,  0 },
	{    9,     300,  193, 0,  0 },
	{   10,     300,  187, 0,  0 },
	{   11,     300,  181, 0,  0 },
	{   12,     300,  175, 0,  0 },
	{   13,     300,  170, 0,  0 },
	{   14,     300,  165, 0,  0 },
	{   15,     300,  160, 0,  0 },
	{   16,     300,  152, 0,  0 },
	{   17,     300,  144, 0,  0 },
	{   18,     300,  136, 0,  0 },
	{   19,     300,  129, 0,  0 },
	{   20,     300,  123, 0,  0 },
	{   21,     300,  117, 0,  0 },
	{   22,     300,  111, 0,  0 },
	{   23,     300,  106, 0,  0 },
	{   24,     300,  101, 0,  0 },
	{   25,     300,  96, 0,  0 },
	{   26,     300,  88, 0,  0 },
	{   27,     300,  80, 0,  0 },
	{   28,     300,  72, 0,  0 },
	{   29,     300,  65, 0,  0 },
	{   30,     300,  59, 0,  0 },
	{   31,     300,  53, 0,  0 },
	{   32,     300,  47, 0,  0 },
	{   33,     300,  42, 0,  0 },
	{   34,     300,  37, 0,  0 },
	{   35,     300,  32, 0,  0 },
	{   36,     300,  28, 0,  0 },
	{   37,     300,  24, 0,  0 },
	{   38,     300,  20, 0,  0 },
	{   39,     300,  17, 0,  0 },
	{   40,     300,  14, 0,  0 },
	{   41,     300,  11, 0,  0 },
	{   42,     300,   8, 0,  0 },
	{   43,     300,   5, 0,  0 },
	{   44,     300,   3, 0,  0 },
	{   45,     300,   0, 0,  0 },
	{   46,     399,   8, 0,  0 },
	{   47,     399,   6, 0,  0 },
	{   48,     399,   3, 0,  0 },
	{   49,     399,   1, 0,  0 },
	{   50,     400,   0, 0,  0 },
	{   51,     598,  11, 0,  0 },
	{   52,     598,   8, 0,  0 },
	{   53,     598,   5, 0,  0 },
	{   54,     598,   3, 0,  0 },
	{   55,     598,   0, 0,  0 },
	{   56,    1190,  28, 0,  0 },
	{   57,    1190,  24, 0,  0 },
	{   58,    1190,  20, 0,  0 },
	{   59,    1190,  17, 0,  0 },
	{   60,    1190,  13, 0,  0 },
	{   61,    1190,  10, 0,  0 },
	{   62,    1190,   8, 0,  0 },
	{   63,    1190,   5, 0,  0 },
	{   64,    1190,   2, 0,  0 },
	{   65,    1190,   0, 0,  0 },
	{   66,    1215,   0, 0,  0 },
	{   67,    1303,   0, 0,  0 },
	{   68,    1393,   0, 0,  0 },
	{   69,    1496,   0, 0,  0 },
	{   70,    1601,   0, 0,  0 },
	{   71,    1722,   0, 0,  0 },
	{   72,    1842,   0, 0,  0 },
	{   73,    1981,   0, 0,  0 },
	{   74,    2115,   0, 0,  0 },
	{   75,    2269,   0, 0,  0 },
	{   76,    2430,   0, 0,  0 },
	{   77,    2616,   0, 0,  0 },
	{   78,    2786,   0, 0,  0 },
	{   79,    3006,   0, 0,  0 },
	{   80,    3202,   0, 0,  0 },
	{   81,    3461,   0, 0,  0 },
	{   82,    3684,   0, 0,  0 },
	{   83,    3984,   0, 0,  0 },
	{   84,    4230,   0, 0,  0 },
	{   85,    4568,   0, 0,  0 },
	{   86,    4895,   0, 0,  0 },
	{   87,    5271,   0, 0,  0 },
	{   88,    5617,   0, 0,  0 },
	{   89,    6011,   0, 0,  0 },
	{   90,    6465,   0, 0,  0 },
	{   91,    6993,   0, 0,  0 },
	{   92,    7449,   0, 0,  0 },
	{   93,    7968,   0, 0,  0 },
	{   94,    8566,   0, 0,  0 },
	{   95,    9260,   0, 0,  0 },
	{   96,    9790,   0, 0,  0 },
	{   97,   10707,   0, 0,  0 },
	{   98,   11421,   0, 0,  0 },
	{   99,   12237,   0, 0,  0 },
	{  100,   13178,   0, 0,  0 },
	{  101,   14276,   0, 0,  0 },
	{  102,   14897,   0, 0,  0 },
	{  103,   16316,   0, 0,  0 },
	{  104,   17132,   0, 0,  1 },
	{  105,   19035,   0, 0,  1 },
	{  106,   20155,   0, 0,  0 },
	{  107,   21415,   0, 0,  1 },
	{  108,   22842,   0, 0,  0 },
	{  109,   24474,   0, 0,  1 },
	{  110,   26356,   0, 0,  0 },
	{  111,   28553,   0, 0,  1 },
	{  112,   31149,   0, 0,  1 },
	{  113,   34263,   1, 0,  1 },
	{  114,   34263,   0, 0,  1 },
	{  115,   38070,   0, 0,  1 },
	{  116,   42829,   2, 0,  2 },
	{  117,   42829,   0, 0,  2 },
	{  118,   48947,   2, 0,  1 },
	{  119,   48947,   0, 0,  1 },
	{  120,   57105,   2, 0,  2 },
	{  121,   57105,   0, 0,  2 },
	{  122,   68526,   4, 0,  1 },
	{  123,   68526,   1, 0,  1 },
	{  124,   68526,   0, 0,  1 },
	{  125,   85657,   4, 0,  3 },
	{  126,   85657,   2, 0,  3 },
	{  127,   85657,   0, 0,  3 },
	{  128,  114208,   7, 0,  2 },
	{  129,  114208,   5, 0,  2 },
	{  130,  114208,   2, 0,  2 },
	{  131,  114208,   0, 0,  2 },
	{  132,  171310,  12, 0,  8 },
	{  133,  171310,   9, 0,  8 },
	{  134,  171310,   7, 0,  8 },
	{  135,  171310,   4, 0,  8 },
	{  136,  171310,   2, 0,  8 },
	{  137,  171310,   0, 0,  8 },
	{  138,  342603,  26, 0,  8 },
	{  139,  342603,  23, 0,  8 },
	{  140,  342603,  19, 0,  8 },
	{  141,  342603,  16, 0,  8 },
	{  142,  342603,  12, 0,  8 },
	{  143,  342603,   9, 0,  8 },
	{  144,  342603,   7, 0,  8 },
	{  145,  342603,   4, 0,  8 },
	{  146,  342603,   2, 0,  8 },
	{  147,  342603,   0, 0,  8 },
};

static code frontAe_t ae60fps50hzTbl[131] = {
	{    0,     992,  251, 0,  0 },
	{    1,     992,  245, 0,  0 },
	{    2,     992,  240, 0,  0 },
	{    3,     992,  234, 0,  0 },
	{    4,     992,  229, 0,  0 },
	{    5,     992,  225, 0,  0 },
	{    6,     992,  216, 0,  0 },
	{    7,     992,  208, 0,  0 },
	{    8,     992,  201, 0,  0 },
	{    9,     992,  194, 0,  0 },
	{   10,     992,  187, 0,  0 },
	{   11,     992,  181, 0,  0 },
	{   12,     992,  176, 0,  0 },
	{   13,     992,  170, 0,  0 },
	{   14,     992,  165, 0,  0 },
	{   15,     992,  161, 0,  0 },
	{   16,     992,  152, 0,  0 },
	{   17,     992,  144, 0,  0 },
	{   18,     992,  137, 0,  0 },
	{   19,     992,  130, 0,  0 },
	{   20,     992,  123, 0,  0 },
	{   21,     992,  117, 0,  0 },
	{   22,     992,  112, 0,  0 },
	{   23,     992,  106, 0,  0 },
	{   24,     992,  101, 0,  0 },
	{   25,     992,  97, 0,  0 },
	{   26,     992,  88, 0,  0 },
	{   27,     992,  80, 0,  0 },
	{   28,     992,  73, 0,  0 },
	{   29,     992,  66, 0,  0 },
	{   30,     992,  59, 0,  0 },
	{   31,     992,  53, 0,  0 },
	{   32,     992,  48, 0,  0 },
	{   33,     992,  42, 0,  0 },
	{   34,     992,  37, 0,  0 },
	{   35,     992,  33, 0,  0 },
	{   36,     992,  28, 0,  0 },
	{   37,     992,  24, 0,  0 },
	{   38,     992,  21, 0,  0 },
	{   39,     992,  17, 0,  0 },
	{   40,     992,  14, 0,  0 },
	{   41,     992,  11, 0,  0 },
	{   42,     992,   8, 0,  0 },
	{   43,     992,   5, 0,  0 },
	{   44,     992,   3, 0,  0 },
	{   45,     992,   1, 0,  0 },
	{   46,    1001,   0, 0,  0 },
	{   47,    1072,   0, 0,  0 },
	{   48,    1150,   0, 0,  0 },
	{   49,    1231,   0, 0,  0 },
	{   50,    1320,   0, 0,  0 },
	{   51,    1411,   0, 0,  0 },
	{   52,    1516,   0, 0,  0 },
	{   53,    1622,   0, 0,  0 },
	{   54,    1745,   0, 0,  0 },
	{   55,    1867,   0, 0,  0 },
	{   56,    2007,   0, 0,  0 },
	{   57,    2143,   0, 0,  0 },
	{   58,    2299,   0, 0,  0 },
	{   59,    2462,   0, 0,  0 },
	{   60,    2650,   0, 0,  0 },
	{   61,    2823,   0, 0,  0 },
	{   62,    3045,   0, 0,  0 },
	{   63,    3245,   0, 0,  0 },
	{   64,    3507,   0, 0,  0 },
	{   65,    3733,   0, 0,  0 },
	{   66,    4037,   0, 0,  0 },
	{   67,    4286,   0, 0,  0 },
	{   68,    4629,   0, 0,  0 },
	{   69,    4960,   0, 0,  0 },
	{   70,    5341,   0, 0,  0 },
	{   71,    5692,   0, 0,  0 },
	{   72,    6091,   0, 0,  0 },
	{   73,    6551,   0, 0,  0 },
	{   74,    7085,   0, 0,  0 },
	{   75,    7547,   0, 0,  0 },
	{   76,    8074,   0, 0,  0 },
	{   77,    8680,   0, 0,  0 },
	{   78,    9383,   0, 0,  0 },
	{   79,    9919,   0, 0,  0 },
	{   80,   10849,   0, 0,  0 },
	{   81,   11573,   0, 0,  0 },
	{   82,   12399,   0, 0,  0 },
	{   83,   13353,   0, 0,  0 },
	{   84,   14466,   0, 0,  0 },
	{   85,   15095,   0, 0,  0 },
	{   86,   16532,   0, 0,  0 },
	{   87,   17359,   0, 0,  0 },
	{   88,   19288,   0, 0,  0 },
	{   89,   20422,   0, 0,  1 },
	{   90,   21699,   0, 0,  0 },
	{   91,   23145,   0, 0,  1 },
	{   92,   24799,   0, 0,  0 },
	{   93,   26706,   0, 0,  0 },
	{   94,   28932,   0, 0,  1 },
	{   95,   31562,   0, 0,  0 },
	{   96,   34718,   1, 0,  1 },
	{   97,   34718,   0, 0,  1 },
	{   98,   38575,   0, 0,  0 },
	{   99,   43397,   2, 0,  1 },
	{  100,   43397,   0, 0,  1 },
	{  101,   49597,   2, 0,  0 },
	{  102,   49597,   0, 0,  0 },
	{  103,   57863,   2, 0,  2 },
	{  104,   57863,   0, 0,  2 },
	{  105,   69435,   4, 0,  1 },
	{  106,   69435,   1, 0,  1 },
	{  107,   69435,   0, 0,  1 },
	{  108,   86794,   4, 0,  3 },
	{  109,   86794,   2, 0,  3 },
	{  110,   86794,   0, 0,  3 },
	{  111,  115724,   7, 0,  2 },
	{  112,  115724,   5, 0,  2 },
	{  113,  115724,   2, 0,  2 },
	{  114,  115724,   0, 0,  2 },
	{  115,  173583,  12, 0,  7 },
	{  116,  173583,   9, 0,  7 },
	{  117,  173583,   7, 0,  7 },
	{  118,  173583,   4, 0,  7 },
	{  119,  173583,   2, 0,  7 },
	{  120,  173583,   0, 0,  7 },
	{  121,  347149,  26, 0,  7 },
	{  122,  347149,  23, 0,  7 },
	{  123,  347149,  19, 0,  7 },
	{  124,  347149,  16, 0,  7 },
	{  125,  347149,  12, 0,  7 },
	{  126,  347149,   9, 0,  7 },
	{  127,  347149,   7, 0,  7 },
	{  128,  347149,   4, 0,  7 },
	{  129,  347149,   2, 0,  7 },
	{  130,  347149,   0, 0,  7 },
};

static code frontAe_t ae60fps60hzTbl[138] = {
	{    0,     598,  249, 0,  0 },
	{    1,     598,  243, 0,  0 },
	{    2,     598,  238, 0,  0 },
	{    3,     598,  233, 0,  0 },
	{    4,     598,  228, 0,  0 },
	{    5,     598,  222, 0,  0 },
	{    6,     598,  214, 0,  0 },
	{    7,     598,  206, 0,  0 },
	{    8,     598,  199, 0,  0 },
	{    9,     598,  192, 0,  0 },
	{   10,     598,  185, 0,  0 },
	{   11,     598,  179, 0,  0 },
	{   12,     598,  174, 0,  0 },
	{   13,     598,  169, 0,  0 },
	{   14,     598,  164, 0,  0 },
	{   15,     598,  158, 0,  0 },
	{   16,     598,  150, 0,  0 },
	{   17,     598,  142, 0,  0 },
	{   18,     598,  135, 0,  0 },
	{   19,     598,  128, 0,  0 },
	{   20,     598,  121, 0,  0 },
	{   21,     598,  115, 0,  0 },
	{   22,     598,  110, 0,  0 },
	{   23,     598,  105, 0,  0 },
	{   24,     598,  100, 0,  0 },
	{   25,     598,  94, 0,  0 },
	{   26,     598,  86, 0,  0 },
	{   27,     598,  78, 0,  0 },
	{   28,     598,  71, 0,  0 },
	{   29,     598,  64, 0,  0 },
	{   30,     598,  57, 0,  0 },
	{   31,     598,  51, 0,  0 },
	{   32,     598,  46, 0,  0 },
	{   33,     598,  41, 0,  0 },
	{   34,     598,  36, 0,  0 },
	{   35,     598,  31, 0,  0 },
	{   36,     598,  27, 0,  0 },
	{   37,     598,  23, 0,  0 },
	{   38,     598,  20, 0,  0 },
	{   39,     598,  16, 0,  0 },
	{   40,     598,  13, 0,  0 },
	{   41,     598,  10, 0,  0 },
	{   42,     598,   7, 0,  0 },
	{   43,     598,   5, 0,  0 },
	{   44,     598,   2, 0,  0 },
	{   45,     598,   0, 0,  0 },
	{   46,    1189,  27, 0,  0 },
	{   47,    1189,  23, 0,  0 },
	{   48,    1189,  19, 0,  0 },
	{   49,    1189,  16, 0,  0 },
	{   50,    1189,  13, 0,  0 },
	{   51,    1189,  10, 0,  0 },
	{   52,    1189,   7, 0,  0 },
	{   53,    1189,   4, 0,  0 },
	{   54,    1189,   2, 0,  0 },
	{   55,    1189,   0, 0,  0 },
	{   56,    1231,   0, 0,  0 },
	{   57,    1320,   0, 0,  0 },
	{   58,    1411,   0, 0,  0 },
	{   59,    1516,   0, 0,  0 },
	{   60,    1622,   0, 0,  0 },
	{   61,    1745,   0, 0,  0 },
	{   62,    1867,   0, 0,  0 },
	{   63,    2007,   0, 0,  0 },
	{   64,    2143,   0, 0,  0 },
	{   65,    2299,   0, 0,  0 },
	{   66,    2462,   0, 0,  0 },
	{   67,    2650,   0, 0,  0 },
	{   68,    2823,   0, 0,  0 },
	{   69,    3045,   0, 0,  0 },
	{   70,    3245,   0, 0,  0 },
	{   71,    3507,   0, 0,  0 },
	{   72,    3733,   0, 0,  0 },
	{   73,    4037,   0, 0,  0 },
	{   74,    4286,   0, 0,  0 },
	{   75,    4629,   0, 0,  0 },
	{   76,    4960,   0, 0,  0 },
	{   77,    5341,   0, 0,  0 },
	{   78,    5692,   0, 0,  0 },
	{   79,    6091,   0, 0,  0 },
	{   80,    6551,   0, 0,  0 },
	{   81,    7085,   0, 0,  0 },
	{   82,    7547,   0, 0,  0 },
	{   83,    8074,   0, 0,  0 },
	{   84,    8680,   0, 0,  0 },
	{   85,    9383,   0, 0,  0 },
	{   86,    9919,   0, 0,  0 },
	{   87,   10849,   0, 0,  0 },
	{   88,   11573,   0, 0,  0 },
	{   89,   12399,   0, 0,  0 },
	{   90,   13353,   0, 0,  0 },
	{   91,   14466,   0, 0,  0 },
	{   92,   15095,   0, 0,  0 },
	{   93,   16532,   0, 0,  0 },
	{   94,   17359,   0, 0,  0 },
	{   95,   19288,   0, 0,  0 },
	{   96,   20422,   0, 0,  1 },
	{   97,   21699,   0, 0,  0 },
	{   98,   23145,   0, 0,  1 },
	{   99,   24799,   0, 0,  0 },
	{  100,   26706,   0, 0,  0 },
	{  101,   28932,   0, 0,  1 },
	{  102,   31562,   0, 0,  0 },
	{  103,   34718,   1, 0,  1 },
	{  104,   34718,   0, 0,  1 },
	{  105,   38575,   0, 0,  0 },
	{  106,   43397,   2, 0,  1 },
	{  107,   43397,   0, 0,  1 },
	{  108,   49597,   2, 0,  0 },
	{  109,   49597,   0, 0,  0 },
	{  110,   57863,   2, 0,  2 },
	{  111,   57863,   0, 0,  2 },
	{  112,   69435,   4, 0,  1 },
	{  113,   69435,   1, 0,  1 },
	{  114,   69435,   0, 0,  1 },
	{  115,   86794,   4, 0,  3 },
	{  116,   86794,   2, 0,  3 },
	{  117,   86794,   0, 0,  3 },
	{  118,  115724,   7, 0,  2 },
	{  119,  115724,   5, 0,  2 },
	{  120,  115724,   2, 0,  2 },
	{  121,  115724,   0, 0,  2 },
	{  122,  173583,  12, 0,  7 },
	{  123,  173583,   9, 0,  7 },
	{  124,  173583,   7, 0,  7 },
	{  125,  173583,   4, 0,  7 },
	{  126,  173583,   2, 0,  7 },
	{  127,  173583,   0, 0,  7 },
	{  128,  347149,  26, 0,  7 },
	{  129,  347149,  23, 0,  7 },
	{  130,  347149,  19, 0,  7 },
	{  131,  347149,  16, 0,  7 },
	{  132,  347149,  12, 0,  7 },
	{  133,  347149,   9, 0,  7 },
	{  134,  347149,   7, 0,  7 },
	{  135,  347149,   4, 0,  7 },
	{  136,  347149,   2, 0,  7 },
	{  137,  347149,   0, 0,  7 },
};

/* gain table. */
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
	8666, 	8667, 	8668, 	8669, 	8670, 	8671, 
};

/*
static code UINT16 gain60fpsTbl[256] = {
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
	8666, 	8667, 	8668, 	8669, 	8670, 	8671,
};
*/ 	

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
	AE_60FPS_60_MAX_IDX,
	AE_60FPS_50_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_60_MIN_IDX,
	AE_60FPS_50_MIN_IDX,
};

/* preview to snapshot binning rate table. */
static code UINT8 frontPrevToSnapBinningRateTbl[PREV_MODE_TOT] = {
	PREV_TO_SNAP_BINNING_RATE_30FPS_PREV,
	PREV_TO_SNAP_BINNING_RATE_60FPS_PREV,
};

/* preview tempoal denoise table. */
static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl[PREV_MODE_TOT] = {
	{  /* 30 fps. */
		MCLK_DIV_30FPS_PREV + 5,
		MCLK_PHASE_30FPS_PREV,
		PCLK_DIV_30FPS_PREV + 5,
		PCLK_PHASE_30FPS_PREV,
		PCLK_FREQ_30FPS_PREV,
		31,
		6,
	},
	{  /* 60 fps. */
		MCLK_DIV_60FPS_PREV,
		MCLK_PHASE_60FPS_PREV,
		PCLK_DIV_60FPS_PREV,
		PCLK_PHASE_60FPS_PREV,
		PCLK_FREQ_60FPS_PREV,
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
		&frontPrevTempDenoiseTbl[1],
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
	0x01,0x03,0x00,0x01, 
	//0x30,0x1A,0x00,0x59,
};

static code UINT8 frontInit1CmdTbl[] = {
	0x30,0x1A,0x10,0xCC,	// reset_register
	//0x03,0x40,0x02,0x3a,  
#if 1
	0x30,0x64,0x08,0x05,
	//0x30,0x24,0x00,0x03,
	0x01,0x04,0x00,0x01,	// grouped_parameter_hold

	/* power setting */
	0x30,0x8e,0xd6,0x6a,
	0x30,0x90,0x56,0x6a,
	0x30,0x88,0x6e,0x61,
	0x30,0x92,0x0a,0x52,	// 0x53,// power comsumption---Brian
	0x30,0x94,0x46,0x56,
	0x30,0x96,0x56,0x52,
	0x30,0xca,0x80,0x06,
	0x31,0x2a,0xdd,0x02,
	0x31,0x2c,0x00,0xe4,
	0x31,0x70,0x29,0x9a,	

	0x01,0x04,0x00,0x00,	// grouped_parameter_de_hold

//	0x30,0x88,0x6e,0x61,
	/*0x30,0x88,0x6f,0xf6,
	0x31,0x54,0x02,0x82,
	0x31,0x56,0x03,0x81,
	0x31,0x62,0x04,0xce,*/
#endif

#if 0//pclk=64m,mclk=23.7m
	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x02,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x36,// pll_multiplier, Pixel clock 96Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x02,	//op_sys_clk_div
#endif
#if 1//input clock = 10.04 pclk = 55.22//82.83
	0x03,0x00,0x00,0x06,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x01,//0x02,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x42,//0x48,//0x36,// pll_multiplier, Pixel clock 96Mhz 
	0x03,0x08,0x00,0x0c,//0x08,	// op_pix_clk_div
	0x03,0x0A,0x00,0x01,//0x02,	//op_sys_clk_div
#endif
  0x37,0x80, 0x00, 0x00,
#if 0//lsc  A
0x36, 0x0A, 0x04, 0x50,
0x36, 0x0C, 0x8E, 0x4E,
0x36, 0x0E, 0x41, 0x90,
0x36, 0x10, 0x5B, 0x8E,
0x36, 0x12, 0xF8, 0xD0,
0x36, 0x4A, 0xEA, 0x2B,
0x36, 0x4C, 0x18, 0x0C,
0x36, 0x4E, 0x08, 0x2F,
0x36, 0x50, 0x0B, 0x2E,
0x36, 0x52, 0xF7, 0xCF,
0x36, 0x8A, 0x30, 0x11,
0x36, 0x8C, 0xA4, 0x6E,
0x36, 0x8E, 0xED, 0x8F,
0x36, 0x90, 0x5D, 0x89,
0x36, 0x92, 0x93, 0x51,
0x36, 0xCA, 0x1F, 0xB1,
0x36, 0xCC, 0x0E, 0x50,
0x36, 0xCE, 0xBB, 0x52,
0x36, 0xD0, 0xD0, 0x72,
0x36, 0xD2, 0x06, 0xD2,
0x37, 0x0A, 0xA1, 0x52,
0x37, 0x0C, 0x95, 0x50,
0x37, 0x0E, 0x74, 0xF0,
0x37, 0x10, 0x1C, 0x73,
0x37, 0x12, 0x39, 0x52,
0x36, 0x00, 0x02, 0x70,
0x36, 0x02, 0x1F, 0x6E,
0x36, 0x04, 0x1A, 0x91,
0x36, 0x06, 0x01, 0xEB,
0x36, 0x08, 0x84, 0x91,
0x36, 0x40, 0x68, 0xAB,
0x36, 0x42, 0xEF, 0xCD,
0x36, 0x44, 0x27, 0x2E,
0x36, 0x46, 0x27, 0xAE,
0x36, 0x48, 0xCE, 0xCF,
0x36, 0x80, 0x5E, 0x11,
0x36, 0x82, 0x7B, 0x6F,
0x36, 0x84, 0x81, 0xCD,
0x36, 0x86, 0x80, 0xD1,
0x36, 0x88, 0xE3, 0xF2,
0x36, 0xC0, 0x2F, 0x91,
0x36, 0xC2, 0x06, 0x51,
0x36, 0xC4, 0xA7, 0x70,
0x36, 0xC6, 0xD5, 0x92,
0x36, 0xC8, 0xC8, 0x52,
0x37, 0x00, 0xB8, 0x12,
0x37, 0x02, 0x94, 0x72,
0x37, 0x04, 0x8D, 0x73,
0x37, 0x06, 0x54, 0x93,
0x37, 0x08, 0x57, 0xB4,
0x36, 0x14, 0x03, 0x70,
0x36, 0x16, 0x5B, 0x6E,
0x36, 0x18, 0x15, 0x11,
0x36, 0x1A, 0xCB, 0x2D,
0x36, 0x1C, 0xA6, 0x71,
0x36, 0x54, 0x6C, 0x4C,
0x36, 0x56, 0x84, 0x4C,
0x36, 0x58, 0x0D, 0xF0,
0x36, 0x5A, 0x58, 0xAF,
0x36, 0x5C, 0xB8, 0xB0,
0x36, 0x94, 0x5F, 0x51,
0x36, 0x96, 0x04, 0xB0,
0x36, 0x98, 0x29, 0xCF,
0x36, 0x9A, 0xF6, 0x30,
0x36, 0x9C, 0xF0, 0xB2,
0x36, 0xD4, 0x59, 0x51,
0x36, 0xD6, 0x43, 0x91,
0x36, 0xD8, 0x65, 0xCF,
0x36, 0xDA, 0xC9, 0x33,
0x36, 0xDC, 0x85, 0x74,
0x37, 0x14, 0xC7, 0xD2,
0x37, 0x16, 0xCC, 0x12,
0x37, 0x18, 0xFC, 0x33,
0x37, 0x1A, 0x25, 0x94,
0x37, 0x1C, 0x34, 0x55,
0x36, 0x1E, 0x04, 0xB0,
0x36, 0x20, 0xD4, 0x8E,
0x36, 0x22, 0x23, 0x31,
0x36, 0x24, 0x39, 0x2D,
0x36, 0x26, 0xA2, 0xD1,
0x36, 0x5E, 0x55, 0x6D,
0x36, 0x60, 0xC5, 0x2D,
0x36, 0x62, 0x3E, 0x30,
0x36, 0x64, 0x1A, 0xAE,
0x36, 0x66, 0x94, 0xD1,
0x36, 0x9E, 0x6C, 0x91,
0x36, 0xA0, 0x9A, 0xCE,
0x36, 0xA2, 0x16, 0xED,
0x36, 0xA4, 0xFA, 0xEE,
0x36, 0xA6, 0xE9, 0x52,
0x36, 0xDE, 0x02, 0x72,
0x36, 0xE0, 0xD9, 0xAC,
0x36, 0xE2, 0xD5, 0xF1,
0x36, 0xE4, 0xCA, 0xF1,
0x36, 0xE6, 0xD0, 0xD2,
0x37, 0x1E, 0xBE, 0x52,
0x37, 0x20, 0x9A, 0x2F,
0x37, 0x22, 0xDB, 0x13,
0x37, 0x24, 0x27, 0xB3,
0x37, 0x26, 0x06, 0xD5,
0x37, 0x82, 0x05, 0x08,
0x37, 0x84, 0x02, 0x88,

  0x37,0x80, 0x80, 0x00,
#endif
#if 0//lsc 宝琪用
0x36, 0x0A, 0x04, 0xF0,
0x36, 0x0C, 0xD7, 0x2D,
0x36, 0x0E, 0x79, 0x4F,
0x36, 0x10, 0x11, 0xEE,
0x36, 0x12, 0xA8, 0x8F,
0x36, 0x4A, 0xD6, 0x49,
0x36, 0x4C, 0x72, 0xE6,
0x36, 0x4E, 0x8A, 0xCD,
0x36, 0x50, 0x54, 0xCD,
0x36, 0x52, 0x77, 0xAD,
0x36, 0x8A, 0x1E, 0xD1,
0x36, 0x8C, 0x05, 0x4E,
0x36, 0x8E, 0xB9, 0x51,
0x36, 0x90, 0xBA, 0x70,
0x36, 0x92, 0x5F, 0x92,
0x36, 0xCA, 0x14, 0xEF,
0x36, 0xCC, 0x3F, 0x0B,
0x36, 0xCE, 0x90, 0x31,
0x36, 0xD0, 0xC5, 0xEE,
0x36, 0xD2, 0x1B, 0xD0,
0x37, 0x0A, 0xE7, 0x31,
0x37, 0x0C, 0xA8, 0x90,
0x37, 0x0E, 0x72, 0x93,
0x37, 0x10, 0x0E, 0x73,
0x37, 0x12, 0xA8, 0xD4,
0x36, 0x00, 0x03, 0x10,
0x36, 0x02, 0x2C, 0xCE,
0x36, 0x04, 0x4E, 0xD0,
0x36, 0x06, 0x6F, 0x8C,
0x36, 0x08, 0x87, 0xCF,
0x36, 0x40, 0x1F, 0x0C,
0x36, 0x42, 0xB9, 0x0D,
0x36, 0x44, 0xF1, 0x4D,
0x36, 0x46, 0x77, 0xAB,
0x36, 0x48, 0x7A, 0xAB,
0x36, 0x80, 0x3C, 0x51,
0x36, 0x82, 0x03, 0xD0,
0x36, 0x84, 0xBB, 0x91,
0x36, 0x86, 0x9F, 0x31,
0x36, 0x88, 0x6E, 0xD1,
0x36, 0xC0, 0x17, 0xAF,
0x36, 0xC2, 0x1E, 0x8F,
0x36, 0xC4, 0xA9, 0x2F,
0x36, 0xC6, 0xBD, 0xB1,
0x36, 0xC8, 0xC5, 0x91,
0x37, 0x00, 0x83, 0xF2,
0x37, 0x02, 0xE2, 0xB1,
0x37, 0x04, 0x39, 0x93,
0x37, 0x06, 0x1F, 0x34,
0x37, 0x08, 0x1D, 0x32,
0x36, 0x14, 0x04, 0x90,
0x36, 0x16, 0x48, 0x6E,
0x36, 0x18, 0x37, 0x50,
0x36, 0x1A, 0x89, 0xED,
0x36, 0x1C, 0xF1, 0x6F,
0x36, 0x54, 0x15, 0x0D,
0x36, 0x56, 0xE8, 0x08,
0x36, 0x58, 0x49, 0xAE,
0x36, 0x5A, 0x37, 0xAE,
0x36, 0x5C, 0xF9, 0x0D,
0x36, 0x94, 0x2F, 0x71,
0x36, 0x96, 0x3E, 0x90,
0x36, 0x98, 0xDF, 0x50,
0x36, 0x9A, 0xD8, 0x91,
0x36, 0x9C, 0x1E, 0xF1,
0x36, 0xD4, 0x62, 0xCF,
0x36, 0xD6, 0x33, 0x2F,
0x36, 0xD8, 0x97, 0x70,
0x36, 0xDA, 0xFC, 0x11,
0x36, 0xDC, 0xCF, 0x51,
0x37, 0x14, 0xEF, 0xF1,
0x37, 0x16, 0xAD, 0x52,
0x37, 0x18, 0x04, 0xF3,
0x37, 0x1A, 0x31, 0xD4,
0x37, 0x1C, 0x52, 0x52,
0x36, 0x1E, 0x04, 0xB0,
0x36, 0x20, 0x8E, 0xAE,
0x36, 0x22, 0x59, 0xF0,
0x36, 0x24, 0x06, 0x6C,
0x36, 0x26, 0x82, 0xF0,
0x36, 0x5E, 0x12, 0xCE,
0x36, 0x60, 0xD4, 0x2D,
0x36, 0x62, 0x52, 0x2E,
0x36, 0x64, 0x40, 0xCD,
0x36, 0x66, 0x98, 0x2F,
0x36, 0x9E, 0x41, 0xB1,
0x36, 0xA0, 0x73, 0xEE,
0x36, 0xA2, 0xFC, 0x31,
0x36, 0xA4, 0xA6, 0xD1,
0x36, 0xA6, 0x69, 0x32,
0x36, 0xDE, 0x06, 0x70,
0x36, 0xE0, 0x0C, 0x0C,
0x36, 0xE2, 0x91, 0xB1,
0x36, 0xE4, 0xB1, 0xEF,
0x36, 0xE6, 0x59, 0x2F,
0x37, 0x1E, 0xF0, 0xF1,
0x37, 0x20, 0xAD, 0x11,
0x37, 0x22, 0x6F, 0x33,
0x37, 0x24, 0x02, 0xD4,
0x37, 0x26, 0xD8, 0x33,
0x37, 0x82, 0x05, 0x94,
0x37, 0x84, 0x03, 0x50,
  0x37,0x80, 0x80, 0x00,
#endif
};

/*
static code UINT8 frontInit2CmdTbl[] = {
	0x30,0x1A,0x10,0xD8,//0xcc,
};
*/

static code UINT8 front30fpsPrevCmdTbl[] = {  // 1280x960
#if 0//pclk=64m,mclk=23.7m
	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x01,//0x02,	// vt_sys_clk_div
	0x03,0x04,0x00,0x03,	// pre_pll_clk_div 
	0x03,0x06,0x00,0xac,//0x48,//0x36,// pll_multiplier, Pixel clock 96Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x01,//0x02,	//op_sys_clk_div
#endif
#if 1
	0x01,0x04,0x00,0x01,	// hold 
	 
	0x03,0x4C,0x05,0x10,	// HSize	1296	0x0x510	// HSize	1280 0x500
	0x03,0x4E,0x03,0xCC,	// VSize	972	0x3CC	// VSize	960	0x3C0
	0x03,0x44,0x00,0x08,	// x addr start	8  
	0x03,0x46,0x00,0x08,	// y addr start	8
	0x03,0x48,0x0A,0x25,	// x addr end	2597   
	0x03,0x4A,0x07,0x9D,	// y addr end	1949	 
	0x30,0x40,0x14,0xC3,	// (xy-skip&2x_xybin, summing)
	//0x30,0x40,0x00,0x41,	// (xy-skip&2_xybin, summing)	
	0x04,0x00,0x00,0x00,	// Scaling_Mode
	0x04,0x04,0x00,0x10,     //scale_m 	 	
	0x30,0x12,0x03,0x1B,	// coarse_integration_time	1052    
	0x30,0x10,0x01,0x5C,	// Fine Correction	348	   
	0x30,0x14,0x07,0x02,//0x09,0xAF, 	// Fine Integration Time	2479 	  
	0x03,0x40,0x03,0x23,//0x13,//0xac,//0x1F,	// Frame Lines	1055
	0x03,0x42,0x11,0xE8,//0x0E,0x1e,	// Line Lentth	3791

	0x30,0x6e,0x48,0x00,//from aptina joyce for binning
	
	0x01,0x04,0x00,0x00,	// de-hold  
	//from aptina joyce start
	0x30, 0x1a, 0x10, 0xd8,//streaming off
	
	0x30, 0xE0, 0x77, 0x02,
	0x30, 0xE2, 0x5C, 0x3E,
	0x30, 0xE4, 0x75, 0x5C,
	0x30, 0xE6, 0x75, 0x03,
	0x30, 0xEA, 0x1F, 0x06,
	0x30, 0xEC, 0x3E, 0x1F,
	0x30, 0xF2, 0x75, 0x03,
	0x30, 0xF6, 0x09, 0x00,
	0x30, 0xF8, 0x09, 0x00,
	0x30, 0xFA, 0x09, 0x00,
	0x30, 0xFC, 0x09, 0x00,
	0x30, 0xFE, 0x43, 0x39,
	0x31, 0x00, 0x43, 0x34,
	0x31, 0x02, 0x5B, 0x45,
	0x31, 0x04, 0x72, 0x5D,
	0x31, 0x06, 0x1E, 0x0A,
	0x31, 0x08, 0x35, 0x1F,
	0x31, 0x0A, 0x75, 0x02,
	0x31, 0x0C, 0x75, 0x02,
	0x31, 0x0E, 0x75, 0x0A,
	0x31, 0x10, 0x72, 0x01,
	0x31, 0x12, 0x75, 0x01,
	0x31, 0x14, 0x08, 0x03,
	0x31, 0x16, 0x72, 0x02,
	0x31, 0x1A, 0x44, 0x37,
	0x31, 0x1E, 0x09, 0x00,
	0x31, 0x22, 0x09, 0x00,
	0x31, 0x28, 0x75, 0x09,
	0x31, 0x2A, 0x71, 0x02,
	0x31, 0x2C, 0x00, 0x78,
	0x31, 0x2E, 0x24, 0x78,
	0x31, 0x40, 0x20, 0x01,
	0x31, 0x42, 0x20, 0x02,
	0x31, 0x44, 0x1E, 0x05,
	0x31, 0x46, 0x1B, 0x01,
	0x31, 0x48, 0x1A, 0x01,
	0x31, 0x4A, 0x1B, 0x04,
	0x31, 0x4E, 0x1F, 0x02,
	0x31, 0x70, 0x29, 0x8E,
	0x30, 0x86, 0x30, 0x65,
	0x30, 0X88, 0XAF, 0XF0,
	0x30, 0x94, 0x46, 0x56,
	0x30, 0x96, 0x56, 0x7F,

	0x30, 0x1a, 0x10, 0xdc,//streaming on
	//from aptina joyce end
#endif

};
static code UINT8 front60fpsPrevCmdTbl[] = {    //640 x 480

                    
	///////////////////////////////
	// 640x480_skip4x-Bin2x	     //
	///////////////////////////////

#if 0//pclk=2mclk
	/* clocking */
	0x03,0x00,0x00,0x05,	// vt_pix_clk_div  
	0x03,0x02,0x00,0x02,	// vt_sys_clk_div
	0x03,0x04,0x00,0x01,	// pre_pll_clk_div 
	0x03,0x06,0x00,0x28,	// pll_multiplier, Pixel clock 47Mhz 
	0x03,0x08,0x00,0x0a,	// op_pix_clk_div
	0x03,0x0A,0x00,0x02,	//op_sys_clk_div
#endif
	0x01,0x04,0x00,0x01,

	0x03,0x44,0x00,0x08,
	0x03,0x48,0x0A,0x21,
	0x03,0x46,0x00,0x08,
	0x03,0x4a,0x07,0x91,
	0x30,0x40,0x15,0xc7, //2x_xyskip&2x_xybinning
	0x04,0x00,0x00,0x00,   
	0x04,0x04,0x00,0x10,  
	0x03,0x4C,0x02,0x88,     // H Size   648
	0x03,0x4E,0x01,0xe4,	// V Size	484 	
	0x03,0x42,0x0c,0xc5,//0x0d,0x20,//0x0b,0x60,      // Length Total  1462
	0x03,0x40,0x02,0x33,//0x92,//0x3b,//0x03,0xde, // Frame Lines 571
	0x02,0x02,0x02,0x33,//0x00,0x10,//0x03,0xdb,
	0x30,0x14,0x07,0x02,
	0x30,0x10,0x01,0x5C,
	0x30,0x6e,0x48,0x00,//from aptina joyce for binning
	0x01,0x04,0x00,0x00,
	
	//from aptina joyce start
	0x30, 0x1a, 0x10, 0xd8,//streaming off
	0x30, 0xE0, 0x77, 0x02,
	0x30, 0xE2, 0x5C, 0x3E,
	0x30, 0xE4, 0x75, 0x5C,
	0x30, 0xE6, 0x75, 0x03,
	0x30, 0xEA, 0x1F, 0x06,
	0x30, 0xEC, 0x3E, 0x1F,
	0x30, 0xF2, 0x75, 0x03,
	0x30, 0xF6, 0x09, 0x00,
	0x30, 0xF8, 0x09, 0x00,
	0x30, 0xFA, 0x09, 0x00,
	0x30, 0xFC, 0x09, 0x00,
	0x30, 0xFE, 0x43, 0x39,
	0x31, 0x00, 0x43, 0x34,
	0x31, 0x02, 0x5B, 0x45,
	0x31, 0x04, 0x72, 0x5D,
	0x31, 0x06, 0x1E, 0x0A,
	0x31, 0x08, 0x35, 0x1F,
	0x31, 0x0A, 0x75, 0x02,
	0x31, 0x0C, 0x75, 0x02,
	0x31, 0x0E, 0x75, 0x0A,
	0x31, 0x10, 0x72, 0x01,
	0x31, 0x12, 0x75, 0x01,
	0x31, 0x14, 0x08, 0x03,
	0x31, 0x16, 0x72, 0x02,
	0x31, 0x1A, 0x44, 0x37,
	0x31, 0x1E, 0x09, 0x00,
	0x31, 0x22, 0x09, 0x00,
	0x31, 0x28, 0x75, 0x09,
	0x31, 0x2A, 0x71, 0x02,
	0x31, 0x2C, 0x00, 0x78,
	0x31, 0x2E, 0x24, 0x78,
	0x31, 0x40, 0x20, 0x01,
	0x31, 0x42, 0x20, 0x02,
	0x31, 0x44, 0x1E, 0x05,
	0x31, 0x46, 0x1B, 0x01,
	0x31, 0x48, 0x1A, 0x01,
	0x31, 0x4A, 0x1B, 0x04,
	0x31, 0x4E, 0x1F, 0x02,
	0x31, 0x70, 0x29, 0x8E,
	0x30, 0x86, 0x30, 0x65,
	0x30, 0X88, 0XAF, 0XF0,
	0x30, 0x94, 0x46, 0x56,
	0x30, 0x96, 0x56, 0x7F,
	0x30, 0x1a, 0x10, 0xdc,//streaming on
	//from aptina joyce end
};

static code UINT8 frontSnapCmdTbl[] = {

	//0x30,0x64,0x08,0x05,
	//0x01,0x00,0x00,0x01,                       
	0x01,0x04,0x00,0x01,

#if 1
	0x03,0x4C,0x0A,0x22,
	0x03,0x4E,0x07,0x9a,//0x98,

	0x03,0x44,0x00,0x08,
	0x03,0x46,0x00,0x08,//0x08,

	0x03,0x48,0x0A,0x29,//0x2f,
	0x03,0x4A,0x07,0xa1,//0x9F,

#else
	0x03,0x44,0x00,0x06,
	0x03,0x48,0x0A,0x29,//0x2f,
	0x03,0x46,0x00,0x06,//0x08,
	0x03,0x4A,0x07,0xa1,//0x9F,
	0x03,0x4C,0x0A,0x24,
	0x03,0x4E,0x07,0x9c,//0x98,

#endif
	//0x02,0x02,0x07,0xEC,

	0x30,0x40,0x00,0x41,    // will: x_y_binning and summing  

	0x30,0x14,0x08,0xdc,//0x06,0xbf,/*修改易导致拍照回来死机*/
	0x30,0x10,0x00,0x9C,

	0x03,0x40,0x07,0xed,    // Frame total 2146
	0x03,0x42,0x15,0x43,//0x00,    // Line total 5214/*修改易导致拍照回来死机*/


	//0x03,0x42,0x0f,0x66,    // Line total 3942
	//0x03,0x40,0x07,0xed,    // Frame total 2029

	0x30,0x6e,0x90,0x00,//from aptina joyce for binning
	0x01,0x04,0x00,0x00,
	//from aptina joyce start
	0x30, 0x1a, 0x10, 0xd8,//streaming off
	0x30, 0xE0, 0xE3, 0x02,
	0x30, 0xE2, 0xB6, 0x7C,
	0x30, 0xE4, 0xE1, 0xB6,
	0x30, 0xE6, 0xE1, 0x03,
	0x30, 0xEA, 0x3F, 0x06,
	0x30, 0xEC, 0x7C, 0x3F,
	0x30, 0xF2, 0xE1, 0x03,
	0x30, 0xFE, 0x8C, 0x6F,
	0x31, 0x00, 0x8C, 0x6A,
	0x31, 0x02, 0xB5, 0x8D,
	0x31, 0x04, 0xDC, 0xB7,
	0x31, 0x06, 0x3E, 0x0A,
	0x31, 0x08, 0x68, 0x3F,
	0x31, 0x0A, 0xE1, 0x02,
	0x31, 0x0C, 0xE1, 0x02,
	0x31, 0x0E, 0xE1, 0x0A,
	0x31, 0x10, 0xE2, 0x01,
	0x31, 0x12, 0xE1, 0x01,
	0x31, 0x16, 0xE2, 0x02,
	0x31, 0x1A, 0x8D, 0x6C,
	0x31, 0x28, 0xE1, 0x09,
	0x31, 0x2A, 0xDD, 0x02,
	0x31, 0x2C, 0x00, 0xE4,
	0x31, 0x2E, 0x42, 0xE4,
	0x31, 0x40, 0x41, 0x01,
	0x31, 0x42, 0x40, 0x02,
	0x31, 0x44, 0x3F, 0x03,
	0x31, 0x46, 0x38, 0x04,
	0x31, 0x48, 0x36, 0x01,
	0x31, 0x4A, 0x38, 0x04,
	0x31, 0x4E, 0x40, 0x02,
	0x31, 0x70, 0x29, 0x9A,
	0x30, 0x86, 0x24, 0x65,
	0x30, 0x88, 0x6F, 0xF0,
	0x30, 0x94, 0x46, 0x56,
	0x30, 0x96, 0x56, 0x52,
	0x30, 0x1a, 0x10, 0xdc,//streaming on
	//from aptina joyce end
	//0x01,0x00,0x00,0x01,   
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
	
  
	opFunc.expGainSetPos                           = EXP_GAIN_SET_POS;
	opFunc.gainAfterExp                            = GAIN_AFTER_EXP;
	opFunc.pfOpDevNameGet                          = frontOpDevNameGet;
	opFunc.pfOpFrameRateSet                        = frontOpFrameRateSet;
	opFunc.pfOpCapabilityGet                       = frontOpCapabilityGet;
	opFunc.pfOpIntrCapabilityGet                   = frontOpIntrCapabilityGet;
	opFunc.pfOpPowerOn                             = frontOpPowerOn;
	opFunc.pfOpPowerOff                            = frontOpPowerOff;
	opFunc.pfOpPreviewModeSet                      = frontOpPreviewModeSet;
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
		case 25:
			printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 0;
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
			frontPrevMode = 1;
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

	/* used (RDK) GPIO12, GPIO13, (EVB) TP_YN & TP_XN GPIO */
	#if (DBG_SNAP_TIMING == 1)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	

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

	globalTimerWait(300);
	frontResCmdBinSend(_frontInit1CmdTbl);

	globalTimerWait(600);

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
	if (frontPrevMode == 0) {
		frontResCmdBinSend(_front30fpsPrevCmdTbl);
		//printf("frontOpFrameRateSet = 30\n");
	}
	else {
		frontResCmdBinSend(_front60fpsPrevCmdTbl);
		//printf("frontOpFrameRateSet = 60\n");
	}

	/* Preview Mode Switch Post Proc */
	frontPreviewModeSwitchPostProc();

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
	//frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
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

	//printf("first=%bd,mode=%bd\n",first,mode);

	if ( first != 0 ) {   // will:  expcept for second time of burst capture
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
		
		#if (DBG_SNAP_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
	
		frontResCmdBinSend(_frontSnapCmdTbl);
			
		frontSnapExposureTimeSet(mode,scaleUp);
	}
	//while(1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
	#endif

	cdspSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	cdspSkipWrSyncSet(1);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
	#endif

	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
	#endif

	HAL_CdspEofWait(1);

	#if (DBG_SNAP_TIMING == 1)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
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

#if 1	
	frontPrevCapabDesc_t *pcap;
	UINT8 cmdTbl[4*1];

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	frontLastGain = pcap->gainTbl[val];
	frontLastGainIdx = val;
	//printf("In %s,%ld,frontLastGain=0x%02x\n",__FILE__,(UINT32)__LINE__,frontLastGain);

    cmdTbl[0] =0x30;//( 0x305e>>8); 
	cmdTbl[1] =0x5e;//(0x305e & 0xff);
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

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;
	pixclk = pcap->pclkFreq;
    
	espline = (UINT16)((UINT32)(pixclk *10) /((UINT32)ltotal * frontLastShutter)); 
//ftotal = 570;
	
	if(espline>(ftotal-3))
	{
		ftotal= espline+3;
		//espline = ftotal-3;
	}
	else
	{
		if (frontPrevMode==0)
		ftotal = 1147;//1146;//1130;//1140;//1148;//1133;//1153;//1148;//1043;//1055;//570;//612;
		else
		ftotal=571;//576;//571;//580;//570;		
	}
	

    
	/**/cmdTbl[0] =0x30;//( 0x300a>>8); 
	cmdTbl[1] =0x0a;//(0x300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0Xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

	
	//cmdTbl[0] =( 0x0202>>8); 
	//cmdTbl[1] =(0x0202 & 0xff);

    
	cmdTbl[0] =0x30;//( 0x3012>>8); 
	cmdTbl[1] =0x12;//(0x3012 & 0xff);
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
	frontLastGainIdx = val;
	//printf("In %s,%ld,frontLastGain=0x%02x\n",__FILE__,(UINT32)__LINE__,frontLastGain);

    cmdTbl[0] =0x30;//( 0x305e>>8); 
	cmdTbl[1] =0x5e;//(0x305e & 0xff);
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

	ltotal = pcap->lineTot;
	ftotal = pcap->frameTot;

	if (frontIntrPreviewDynamicTempoalDenoiseEnGet() == FRONT_PV_DYNAMIC_TD_ENABLE) {
		pixclk = pcap->prevDynamicTDTbl->pclkFreq;
	}
	else {
		pixclk = pcap->pclkFreq;
	}
    
	espline = (UINT16)((UINT32)(pixclk *10) /((UINT32)ltotal * frontLastShutter)); 
      //ftotal = 570;
	
	if(espline>(ftotal-3))
	{
		ftotal= espline+3;
		//espline = ftotal-3;
	}
	else
	{
		if (frontPrevMode==0)
		ftotal = 1147;//1146;//1130;//;//1148;//1133;//1153;//1148;//1043;//1055;//570;//612;
		else
		ftotal=571;//576;//580;//570;
	}
	

    
	cmdTbl[0] =0x30;//( 0x300a>>8); 
	cmdTbl[1] =0x0a;//(0x300a & 0xff);
	cmdTbl[2] = ((ftotal>>8)&0xff);
	cmdTbl[3] = (ftotal&0Xff);
	i2cCmdTableSend_Intr(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

    	cmdTbl[0] =0x30;//( 0x3012>>8); 
	cmdTbl[1] =0x12;//(0x3012 & 0xff);
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
		
	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	if (en == FRONT_PV_DYNAMIC_TD_ENABLE) {
		//printf("clk enable\n");
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
		//printf("clk disable\n");
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
	UINT16 s_gain;

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

	espline = (pixclk * 10 ) / (ltotal * frontSnapLastShutter);	 
	s_gain = frontSnapLastGain;
	//lastt_gain = 	frontLastGain;
	//printf("$$$ will test ltotal=%d,ftotal=%d,pixclk=%ld,espline=%d,shutter=%d\n",ltotal,ftotal,pixclk,espline,frontLastGain);

	espline = espline * frontPrevToSnapBinningRateTbl[frontPrevMode];  // compensation of binning mode

#if 0
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


    
	cmdTbl[0] =0x30;//( 0x3012>>8); 
	cmdTbl[1] =0x12;//(0x3012 & 0xff);
	cmdTbl[2] = ((espline>>8)&0xff);
	cmdTbl[3] = (espline&0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);
    
	
	cmdTbl[0] =0x30;//( 0x305e>>8); 
	cmdTbl[1] =0x5e;//(0x305e & 0xff);
	cmdTbl[2] = (s_gain>>8);
	cmdTbl[3] = (s_gain& 0xff);
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);

	
/*	cmdTbl[0] =( 0x301a>>8); 
	cmdTbl[1] =(0x301a & 0xff);
	cmdTbl[2] = 0x18;	
	cmdTbl[3] = 0xde;                                                                                                                                        
	i2cCmdTableSend(cmdTbl, 3, sizeof(cmdTbl)/4, I2C_TRANS_MODE_NORMAL, 0, 0);	
*/

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
	UINT8 isoCfg, isoGainIdx, autoGainIdx, pvBanding, capBanding, pwrFreq;
	UINT16 maxGainTblIdx;
	UINT16 isoNumerator = 0, autoNumerator = 0;
	UINT16 isoDenominator = 0, autoDenominator = 0;
	UINT32 isoShutter, limitShutter, bandingShutter;
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
		/* auto iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[autoGainIdx],&autoNumerator,&autoDenominator);
		//printf("autoGain=0x%x,%d/%d\n",pcap->gainTbl[autoGainIdx],autoNumerator,autoDenominator);
		
		/* user define iso mode, transfer gain idx to gain value (numerator/denominator) type */
		frontGainTblValueTransfer(pcap->gainTbl[isoGainIdx],&isoNumerator,&isoDenominator);
		//printf("isoGain=0x%x,%d/%d\n",pcap->gainTbl[isoGainIdx],isoNumerator,isoDenominator);

		isoShutter = isoNumerator * frontLastShutter * autoDenominator / (isoDenominator * autoNumerator);
		//printf("isoShutter=%lu\n",isoShutter);

		#if (CAPTURE_LIMIT_FPS != 0)
		/* to limit exposure time small than CAPTURE_LIMIT_FPS */
		limitShutter = (UINT32)(CAPTURE_LIMIT_FPS * 10 * frontPrevToSnapBinningRateTbl[frontPrevMode]);
		if (isoShutter < limitShutter) {
			isoShutter = limitShutter;
			//printf("limitShutter=%lu\n",limitShutter);
		}
		#endif

		#if (ISO_CAPTURE_AUTO_DEBANDING == 1)
		/* get banding shutter number */
		bandingShutter = (UINT32)10*pwrFreq*2;

		/* get banding type */
		pvBanding = ((frontLastShutter > bandingShutter) ? 1 : 0);
		//capBanding = ((isoShutter/frontPrevToSnapBinningRateTbl[frontPrevMode] > bandingShutter) ? 1 : 0);
		capBanding = ((isoShutter > bandingShutter*frontPrevToSnapBinningRateTbl[frontPrevMode]) ? 1 : 0);
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
		frontSnapLastGain = pcap->gainTbl[lastGainIdx];
		frontSnapLastShutter = frontLastShutter;
	}
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

	/* power on status */
	if (frontPowerOnStatus == 1) {
	
		/* sensor different resolution or fps switch check */
		if (frontPrevModeSave != frontPrevMode) {//(frameRateSetFlag == 1) {
			sp1kAeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
			sp1kAeStatusGet(SP1K_AE_gId, &gIdval);
			pv_gId = gIdval;
			//printf("pvFreq=%bu, gIdval=%bu\n",pvFreq, gIdval);
			
			if (frontPrevModeSave == 1) {
				pv_gId += 14;
				//printf("60->30 +\n");
			}
			else if (frontPrevModeSave == 0) {
				pv_gId -= 14;
				//printf("30->60 -\n");
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

	i2cCmdTableSend(G_FPGAMemPool, 3, size/4, I2C_TRANS_MODE_NORMAL, 0, 0);
	#else
	UINT32 size;
	UINT8 *pData;
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

	
	i2cCmdTableSend(pData, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);
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

