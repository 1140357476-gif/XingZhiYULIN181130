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
 * @file		front_ti_tvp5150_yuv.c
 * @brief		front sensor omni vision ov9655 driver with yuv format data.
 * @author		CS Lin
 * @since		2008-11-24
 * @date		2008-12-08
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
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "dbg_file_list.h"
#include "dbg_def.h"


#include "gpio.h"


/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_SENSOR_DRIVER__
 
#define REGISTER_EXTERN_OP_FUNC		1//0	/* used register extern operating function */
 
/* mode. */
#define PREV_MODE_TOT  2
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV    240
#define HEIGHT_30FPS_PREV_PAL    280
#define XOFF_30FPS_PREV       42
#define YOFF_30FPS_PREV       1
#define YOFF_30FPS_PREV_PAL       5
#define LINE_TOT_30FPS_PREV   845 
#define FRAME_TOT_30FPS_PREV  525

#define WIDTH_60FPS_PREV      640
#define HEIGHT_60FPS_PREV     240
#define XOFF_60FPS_PREV       42
#define YOFF_60FPS_PREV       22
#define LINE_TOT_60FPS_PREV   845
#define FRAME_TOT_60FPS_PREV  525

#define WIDTH_SNAP      640
#define HEIGHT_SNAP     240
#define XOFF_SNAP       42
#define YOFF_SNAP       1
#define LINE_TOT_SNAP   845
#define FRAME_TOT_SNAP  525

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_EXT
#define PCLK_SRC  FRONT_PCLK_SRC_INT

#define MCLK_DIV_30FPS_PREV    10
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV  2
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_60FPS_PREV    10///4/*if set 13,no banding,but frame rate=50.2fps*/
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV     2
#define PCLK_PHASE_60FPS_PREV  0

#define MCLK_DIV_SNAP    10
#define MCLK_DIV_SNAP_BURST  10
#define MCLK_DIV_SNAP_BURST_UP  10
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    2
#define PCLK_PHASE_SNAP  0

#define PCLK_FREQ_30FPS_PREV 24000000L //522000000L /2/ MCLK_DIV_30FPS_PREV / PCLK_DIV_30FPS_PREV
#define PCLK_FREQ_60FPS_PREV 24000000L //522000000L /2/ MCLK_DIV_60FPS_PREV / PCLK_DIV_60FPS_PREV
#define PCLK_FREQ_SNAP       24000000L //522000000L /2/ MCLK_DIV_SNAP / PCLK_DIV_SNAP
#define PCLK_FREQ_SNAP_BURST  24000000L//	522000000L /2/ MCLK_DIV_SNAP_BURST / PCLK_DIV_SNAP   
#define PCLK_FREQ_SNAP_BURST_UP 24000000L//522000000L /2/ MCLK_DIV_SNAP_BURST_UP / PCLK_DIV_SNAP   

/* bypass */
#define BYPASS_HREF_EN			0
#define BYPASS_VREF_EN			0

/* reshape. */
#if BYPASS_HREF_EN == 0
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HEN_60FPS_PREV   0
#else
#define RESHAPE_HEN_30FPS_PREV    0	//bypass h don't enable
#define RESHAPE_HEN_60FPS_PREV    0	//bypass h don't enable
#endif
#define RESHAPE_HFALL_30FPS_PREV  39
#define RESHAPE_HRISE_30FPS_PREV  40
#define RESHAPE_HFALL_60FPS_PREV  39
#define RESHAPE_HRISE_60FPS_PREV  40

#if BYPASS_VREF_EN == 0
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VEN_60FPS_PREV    0
#else
#define RESHAPE_VEN_30FPS_PREV    0	//bypass v don't enable
#define RESHAPE_VEN_60FPS_PREV    0	//bypass v don't enable
#endif

#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3
#define RESHAPE_VFALL_60FPS_PREV  2
#define RESHAPE_VRISE_60FPS_PREV  3

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  1

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  39
#define RESHAPE_HRISE_SNAP  40
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* yuv interface. */
#define YUV_SEQ    FRONT_YUV_OUTPUT_UYVY
#define CCIR_MODE  FRONT_YUV_CCIR656
#define Y_SUB_128  0
#define U_SUB_128  1
#define V_SUB_128  1

/* i2c interface. */
#define I2C_DEV_ADDR  0xB8
#define I2C_CLK_DIV  5

/* gpio interface. */
#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/* ae. */
#define AE_30FPS_30_MAX_IDX  1
#define AE_30FPS_30_MIN_IDX  1
#define AE_30FPS_25_MAX_IDX  1
#define AE_30FPS_25_MIN_IDX  1
#define EV10_30FPS_EXP_IDX   100

#define AE_60FPS_60_MAX_IDX  0
#define AE_60FPS_60_MIN_IDX  0
#define AE_60FPS_50_MAX_IDX  0
#define AE_60FPS_50_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0

/* gain. */
#define GAIN_30FPS_MAX_IDX  46
#define GAIN_30FPS_MIN_IDX  5

#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0

#define GAIN_LARGE   0
#define GAIN_MIDDLE  0
#define GAIN_SMALL   0

/* Preview SRAM Mode */
#define PREVIEW_SRAM_MODE	FRONT_SRAM_MODE_DISABLE				//FRONT_SRAM_MODE_ENABLE : image width <= 1280
																//FRONT_SRAM_MODE_DISABLE : image width > 1280 or yuv sensor


#define EXP_GAIN_SET_POS  AE_EXP_GAIN_NOT_ACTION_FOR_YUV		// AE_EXP_GAIN_SET_POS_VD_RISING: VD rising, 
																//AE_EXP_GAIN_SET_POS_VVALID_FALLING: VVALID falling,
																//AE_EXP_GAIN_SET_POS_VVALID_RISING: VVALID rising.
#define GAIN_AFTER_EXP    AE_GAIN_AFTER_EXP_OFF					//AE_GAIN_AFTER_EXP_OFF,
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

//static void frontColorNoiseReduce(UINT8 gain);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - TI TP5150 YUV";

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
	{    0,     0,  0, 0,  0 },
};
static code frontAe_t ae20fps60hzTbl[1] = {
	{    0,     0,  0, 0,  0 },
};
*/
static code frontAe_t ae30fps50hzTbl[1] = {
	{    0,     0,  0, 0,  0 },
};

static code frontAe_t ae30fps60hzTbl[1] = {
	{    0,     0,  0, 0,  0 },
};                               

static code frontAe_t ae60fps60hzTbl[1] = {
	{    0,     0,  0, 0,  0 },
};
static code frontAe_t ae60fps50hzTbl[1] = {
	{    0,     0,  0, 0,  0 },
};


/* gain table. */
static code UINT16 gainTbl[1] = {
	0,
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

static code UINT8 ae60fpsMaxIdxTbl[2] = {
	AE_60FPS_60_MAX_IDX,
	AE_60FPS_50_MAX_IDX,
};

static code UINT8 ae60fpsMinIdxTbl[2] = {
	AE_60FPS_60_MIN_IDX,
	AE_60FPS_50_MIN_IDX,
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
	0x05, 0x01,
};
static code UINT8 frontInit1CmdTbl[] = {
	0x05, 0x00,
	0x03, 0x09,
	0x04, 0xc0,
	0x0d, 0x47,
	0xD0, 0xFF,
	0x0f, 0x0a,
	0x03, 0x6d,
	0x11, 0xff,
	0x12, 0x03,//0x07,
	0x03, 0x8f,//0x0d,
	0x18,0xff,
	0x19,0xff,
//	0x28, 0x02,/*Only for NTSC*/
	0x28, 0x00, /*PAL&NTSC auto switch*/
};

static code UINT8 frontInit2CmdTbl[] = {
	0x3f, 0x00,
	0x75, 0x04,
	0x76, 0xe1,
	0x4c, 0x00,
	0x77, 0x01,
	0x3d, 0xc2,
	0x4b, 0x09,
	0xc9, 0x60,
	0x41, 0x38,
	0x56, 0x40,
};

static code UINT8 frontInit3CmdTbl[] = {
	0X34, 0X11,
	0X3b, 0X0a,
	0Xa4, 0X88,
	0X96, 0X00,
	0X97, 0X30,
	0X98, 0X20,
	0X99, 0X30,
	0X9a, 0X84,
	0X9b, 0X29,
};
static code UINT8 frontInit4CmdTbl[] = {
	0X9c, 0X03,
	0X9d, 0X89,
	0X9e, 0X7e,
	0X78, 0X04,
	0X79, 0X01,
	0Xc8, 0Xf0,
	0X79, 0X0f,
	0Xc8, 0X00,
	0X79, 0X10,
};

static code UINT8 frontInit5CmdTbl[] = {
	0xc8, 0x7e,
	0x79, 0x0a,
	0xc8, 0x80,
	0x79, 0x0b,
	0xc8, 0x01,
	0x79, 0x0c,
	0xc8, 0x0f,
	0x79, 0x0d,
	0xc8, 0x20,
	0x79, 0x09,
};


static code UINT8 frontInit6CmdTbl[] = {
	0xc8, 0x80,
	0x79, 0x02,
	0xc8, 0xc0,
	0x79, 0x03,
	0xc8, 0x40,
	0x79, 0x05,
	0xc8, 0x30,
	0x79, 0x26,
	0x2d, 0x00,
	0x2e, 0x00,
};


static code UINT8 front30fpsPrevCmdTbl[] = {
	/*pixclk = 23.7M,mclk = 23.7M,frame rate = 29.7fps*/
	0x03, 0x0d,
};
static code UINT8 front60fpsPrevCmdTbl[] = {
	/*pixclk = 47.5M,mclk = 23.7M,frame rate = 59.3fps*/
	0x03, 0x0d,
};

static code UINT8 frontSnapCmdTbl[] = {
	0x03, 0x0d,
};

static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;

static UINT16 frontLastGain;
static UINT16 frontLastGainIdx;
static UINT32 frontLastShutter;
static UINT16 frontSnapLastGain;
static UINT32 frontSnapLastShutter;
static UINT8 LoadBackfrontLastGainIdx;
static UINT8 LoadBackfrontLastSetting = 0;
static UINT8 frontPrevModeSave = 0;	/* resolve ov5653 sensor different resolution & fps switch issue used */

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
  
/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
//	frontResTblBinRead(_gainTbl, __gainTbl);

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
 * @since	2008-11-17
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
			printf("frontOpFrameRateSet = 24\n");
			frontPrevMode = 0;
			//frontSnapMode = 0;
			
			
			break;*/
		case 30:
		case 25:
			//printf("frontOpFrameRateSet = 30\n");
			frontPrevMode = 0;
			//frontSnapMode = 1;
			
		
			break;
		case 60:
		case 50:
			//printf("frontOpFrameRateSet = 60\n");
			frontPrevMode = 1;
			//frontSnapMode = 2;
			
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
 * @since	2008-11-17
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
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPowerOn(
	void
)
{
	frontPrevCapabDesc_t *pcap;
	//UINT8 addr[1],datatbl[1];
	
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
	#if 0
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x04, 0xff);  
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x04, 0xff);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x04, 0x00);
	globalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x04, 0xff);
	globalTimerWait(100);
	#endif
	/* set yuv interface. */
	frontParaSet(FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG, YUV_SEQ, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_CCIR_MODE, CCIR_MODE, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, Y_SUB_128, U_SUB_128, V_SUB_128, 0, 0);
	frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 1, 0, 0, 0, 0);
	/* sensor reset complete */
	globalTimerWait(10);
	/* send i2c command. */
	frontResCmdBinSend(_frontInit0CmdTbl);
	globalTimerWait(10);
	frontResCmdBinSend(_frontInit1CmdTbl);
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
 * @since	2008-11-17
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
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpPreviewModeSet(
	void
)
{
	frontPrevCapabDesc_t *pcap;
	
	//if(snapFlg)frontOpPowerOn();

	pcap = &(frontPrevCapabDesc[frontPrevMode]);
	printf("frontOpPreviewModeSet --------------1\n");
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	printf("frontOpPreviewModeSet --------------2\n");
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);
	//i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	/* set yuv interface. */
	frontParaSet(FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG, YUV_SEQ, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_CCIR_MODE, CCIR_MODE, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, Y_SUB_128, U_SUB_128, V_SUB_128, 0, 0);
	frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 1, 0, 0, 0, 0);
	cdspBayerPatternSet(BAYER_PTN_PREV);

	{
		
		UINT8 reg,datLow,datHig;
		reg = 0x84;
		i2cRegRead(&reg,&datHig,1,1,1,1,0);
		reg = 0x85;
		i2cRegRead(&reg,&datLow,1,1,1,1,0);

		printf("\033[32mRead:%#bx = %#bx-%#bx\033[0m\n",reg,datHig,datLow);

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
frontOpPreviewSensorZoomModeSet(
	UINT16 factor
)
{
	factor = factor;
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
	factor = factor;
}
/**
 * @brief	operating function to set snap mode.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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

	//frontSnapExposureTimeGet();
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);
	frontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);
	//i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	//frontResCmdBinSend(_frontSnapCmdTbl);
	//frontSnapExposureTimeSet();
	cdspBayerPatternSet(BAYER_PTN_SNAP);
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	//while(1);
}

/**
 * @brief	operating function to snap shot.
 * @param	first = [in] 0: not first, 1: first shot for burst shot.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
	first = first;
	mode = mode;
	scaleUp = scaleUp;
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
 * @since	2008-11-17
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
	val = val;
	isr = isr;
	upd = upd;
}

/**
 * @brief	operating function to set exposure time value.
 * @param	parg->val = [in] exposure time value.
 * @param	parg->isr = [in] called by 0: regular function, 1: isr.
 * @param	parg->upd = [in] 0: immediately update, 1: sync to vsync falling.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpExposureTimeSet(
	frontExposureTimeArg_t *parg
)
{
	parg = parg;
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
	val = val;
	isr = isr;
	upd = upd;

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
	parg = parg;
}

/**
 * @brief	operating function to get optical black status.
 * @param	None.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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
 * @since	2008-11-17
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
	en = en;
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
	UINT8  mode;
	UINT8 addrTbl[1], dataTbl[1];

	addrTbl[0] = 0x88; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);
	mode =dataTbl[0] & 0x01;
	printf("val=%bd\n", mode);
}

static UINT8 TTTVDEVOGet(void)
{
	frontAfterPowerOff();
	return SUCCESS;
}
/**
 * @brief	set snap exposure time.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
static void
frontSnapExposureTimeGet(
	void
)
{
;
}

static void
frontSnapExposureTimeSet(
	UINT8 mode,
	UINT8 scaleUp
)
{
	mode = mode;
	scaleUp = scaleUp;
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
	value = value;
	numerator = numerator;
	denominator = denominator;
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

	i2cCmdTableSend(G_FPGAMemPool, 1, size/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	
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


