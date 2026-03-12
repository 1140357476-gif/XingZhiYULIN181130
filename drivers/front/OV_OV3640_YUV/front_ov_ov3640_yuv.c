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
 * @file		front_ov_ov3640_yuv.c
 * @brief		front sensor omni vision ov9655 driver with yuv format data.
 * @author		CS Lin
 * @since		2008-11-24
 * @date		2008-12-29
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
//#include "front_logi.h"
#include "gpio_api.h"
#include "gpio.h"
#include "i2c.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      1024
#define HEIGHT_30FPS_PREV     760
#define XOFF_30FPS_PREV       4
#define YOFF_30FPS_PREV       4
#define LINE_TOT_30FPS_PREV   1024
#define FRAME_TOT_30FPS_PREV  768

#define WIDTH_SNAP      2016
#define HEIGHT_SNAP     1512//1480
#define XOFF_SNAP       4
#define YOFF_SNAP       4
#define LINE_TOT_SNAP   2016
#define FRAME_TOT_SNAP  1512//1480

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_EXT
#define PCLK_SRC  FRONT_PCLK_SRC_INT

#define MCLK_DIV_30FPS_PREV    4
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0

#define MCLK_DIV_SNAP    4//16
#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  28000000L
#define PCLK_FREQ_SNAP        38000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / MCLK_DIV_30FPS_PREV / PCLK_DIV_30FPS_PREV
#define PCLK_FREQ_SNAP        96000000L / MCLK_DIV_SNAP / PCLK_DIV_SNAP
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HFALL_30FPS_PREV  1
#define RESHAPE_HRISE_30FPS_PREV  2
#define RESHAPE_VEN_30FPS_PREV    0
#define RESHAPE_VFALL_30FPS_PREV  1
#define RESHAPE_VRISE_30FPS_PREV  2
#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  0
#define RESHAPE_HRISE_SNAP  1
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* yuv interface. */
#define YUV_SEQ    FRONT_YUV_OUTPUT_VYUY
#define CCIR_MODE  FRONT_YUV_CCIR601
#define Y_SUB_128  0
#define U_SUB_128  1
#define V_SUB_128  1

/* i2c interface. */
#define I2C_DEV_ADDR  0x78
#define I2C_CLK_DIV   3

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

/* gain. */
#define GAIN_30FPS_MAX_IDX  46
#define GAIN_30FPS_MIN_IDX  5

#define	EXP_GAIN_SET_POS  0x00
#define GAIN_AFTER_EXP    0x00

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
__STATIC void frontOpAeTargetLuminanceSet(void *parg);
__STATIC void frontOpAeExposureCompensationSet(void *parg);
__STATIC void frontOpAeFrameRateSet(void *parg);
__STATIC void frontOpAwbModeSet(void *parg);

static void frontSnapExposureTimeGet(void);
static void frontSnapExposureTimeSet(void);
static void frontColorNoiseReduce(UINT8 gain);
static void frontAllReg16Dump(UINT8 dataLen);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV3640 YUV";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[1] = {
	0,
};

static code frontAe_t ae30fps50hzTbl[1] = {
	0,
};

/* gain table. */
static code UINT16 gain30fpsTbl[1] = {
	0,
};

/* color matrix table. */
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

static code UINT8 frontInit0CmdTbl[] = {
	0x30,0x12,0x80,
};
#if 0
static code UINT8 frontInit1CmdTbl[] = {
	0x30,0x4d,0x45,
	0x30,0x87,0x16,
	0x30,0x9c,0x1a,
	0x30,0xa2,0xe4,
	0x30,0xaa,0x42,
	0x30,0xb0,0xff,
	0x30,0xb1,0xff,
	0x30,0xb2,0x10,
	0x30,0x0e,0x32,
	0x30,0x0f,0x21,
	0x30,0x10,0x20,
	0x30,0x11,0x01,
	0x30,0x4c,0x82,
	0x30,0xd7,0x10,
	0x30,0xd9,0x0d,
	0x30,0xdb,0x08,
	0x30,0x16,0x82,
	0x30,0x18,0x48,
	0x30,0x19,0x40,
	0x30,0x1a,0x82,
	0x30,0x7d,0x00,
	0x30,0x87,0x02,
	0x30,0x82,0x20,
	0x30,0x71,0x75, //
	0x30,0x73,0x62, //
	0x30,0x1c,0x06, //50hz
	//0x30,0x1c,0x05, //60hz
	0x30,0x1d,0x07, //
	0x30,0x15,0x12,
	0x30,0x14,0x84, //50hz
	//0x30,0x14,0x04, 60hz,
//30,0x13,0xf7, //awb
	0x30,0x3c,0x08,
	0x30,0x3d,0x18,
	0x30,0x3e,0x06,
	0x30,0x3F,0x0c,
	0x30,0x30,0x62,
	0x30,0x31,0x26,
	0x30,0x32,0xe6,
	0x30,0x33,0x6e,
	0x30,0x34,0xea,
	0x30,0x35,0xae,
	0x30,0x36,0xa6,
	0x30,0x37,0x6a,
	0x31,0x04,0x02,
	0x31,0x05,0xfd,
	0x31,0x06,0x00,
	0x31,0x07,0xff,
	0x33,0x00,0x12,
	0x33,0x01,0xde,
	0x33,0x02,0xcf,
	0x33,0x12,0x26,
	0x33,0x14,0x42,
	0x33,0x13,0x2b,
	0x33,0x15,0x42,
	0x33,0x10,0xd0,
	0x33,0x11,0xbd,
	0x33,0x0c,0x18,
	0x33,0x0d,0x18,
	0x33,0x0e,0x56,
	0x33,0x0f,0x5c,
	0x33,0x0b,0x1c,
	0x33,0x06,0x5c,
	0x33,0x07,0x11,
	0x33,0x6a,0x52,
	0x33,0x70,0x46,
	0x33,0x76,0x38,
	0x33,0x00,0x13,
	0x30,0xb8,0x20,
	0x30,0xb9,0x17,
	0x30,0xba,0x04,
	0x30,0xbb,0x08,
	0x35,0x07,0x06,
	0x35,0x0a,0x4f,
	0x31,0x00,0x02,
	0x33,0x01,0xde,
	0x33,0x04,0xfc,
	0x34,0x00,0x00,
	0x34,0x04,0x00,
	0x36,0x00,0xc0,
	0x33,0x58,0x38,//
	0x33,0x59,0x38,//
	0x33,0x55,0x02,//
0x30,0x13,0xf7, //awb
};

static code UINT8 front30fpsPrevCmdTbl[] = {
	0x30,0x12,0x10,
	0x30,0x23,0x06,
	0x30,0x26,0x03,
	0x30,0x27,0x04,
	0x30,0x2a,0x03,
	0x30,0x2b,0x39,//50hz
	//0x30,0x2b,0x10,//60hz
0x30,0x75,0x24,
0x30,0x0d,0x01,
0x30,0xd7,0x90,
0x30,0x69,0x04,
0x30,0x3e,0x00,
0x30,0x3f,0xc0,
0x33,0x02,0xef,
0x33,0x5f,0x34,
0x33,0x60,0x0c,
0x33,0x61,0x04,
#if 1
0x33,0x62,0x34,//for xga output size start
0x33,0x63,0x08,
0x33,0x64,0x04,
0x34,0x03,0x42,
0x30,0x88,0x04,
0x30,0x89,0x00,
0x30,0x8a,0x03,
0x30,0x8b,0x00,
0x30,0x4c,0x82,//for xga output size end
#else
0x33,0x62,0x12,//for vga output size start
0x33,0x63,0x88,
0x33,0x64,0xe4,
0x34,0x03,0x42,
0x30,0x88,0x12,
0x30,0x89,0x80,
0x30,0x8a,0x01,
0x30,0x8b,0xe0,
0x30,0x4c,0x85,//for vga output size end
#endif
0x30,0x8d,0x04,
0x30,0x86,0x03,
0x30,0x86,0x00,

//0x30,0x77,0x40,
//for a few more hd appear in blank area

};

#else

static code UINT8 frontInit1CmdTbl[] = {
0x30,0x4d,0x45,
0x30,0x87,0x16,
0x30,0x9c,0x1a,
0x30,0xa2,0xe4,
0x30,0xaa,0x42,
0x30,0xb0,0xff,
0x30,0xb1,0xff,
0x30,0xb2,0x10,
0x30,0x0e,0x32,
0x30,0x0f,0x21,
0x30,0x10,0x20,
0x30,0x11,0x01,
0x30,0x4c,0x82,
0x30,0xd7,0x10,
0x30,0xd9,0x0d,
0x30,0xdb,0x08,
0x30,0x16,0x82,
0x30,0x18,0x48,
0x30,0x19,0x40,
0x30,0x1a,0x82,
0x30,0x7d,0x00,
0x30,0x87,0x02,
0x30,0x82,0x20,
0x30,0x71,0x75,
0x30,0x73,0x62,
0x30,0x1c,0x06,
0x30,0x1d,0x07,
0x30,0x15,0x12,
0x30,0x14,0x84,
0x30,0x13,0xf7,
0x30,0x3c,0x08,
0x30,0x3d,0x18,
0x30,0x3e,0x06,
0x30,0x3F,0x0c,
0x30,0x30,0x62,
0x30,0x31,0x26,
0x30,0x32,0xe6,
0x30,0x33,0x6e,
0x30,0x34,0xea,
0x30,0x35,0xae,
0x30,0x36,0xa6,
0x30,0x37,0x6a,
0x31,0x04,0x02,
0x31,0x05,0xfd,
0x31,0x06,0x00,
0x31,0x07,0xff,
0x33,0x00,0x12,
0x33,0x01,0xde,
0x33,0x02,0xcf,
0x33,0x40,0x05,
0x33,0x41,0x3A,
0x33,0x42,0x01,
0x33,0x43,0x18,
0x33,0x44,0x52,
0x33,0x45,0x6B,
0x33,0x46,0x6D,
0x33,0x47,0x7E,
0x33,0x48,0x10,
0x33,0x49,0x98,
0x33,0x3F,0x0C,
0x33,0x17,0x10,
0x33,0x16,0xf0,
0x33,0x12,0x38,
0x33,0x14,0x52,
0x33,0x13,0x2e,
0x33,0x15,0x3f,
0x33,0x11,0xcd,
0x33,0x10,0xa6,
0x33,0x0c,0x13,
0x33,0x0d,0x18,
0x33,0x0e,0x60,
0x33,0x0f,0x51,
0x33,0x0b,0x11,
0x33,0x06,0x5e,
0x33,0x07,0x11,
0x33,0x08,0x25,
0x33,0x68,0x01,
0x33,0x6a,0x4f,
0x33,0x70,0x46,
0x33,0x76,0x38,
0x33,0x00,0x13,
0x33,0x66,0x15,
0x30,0xb8,0x20,
0x30,0xb9,0x17,
0x30,0xba,0x04,
0x30,0xbb,0x08,
0x35,0x07,0x06,
0x35,0x0a,0x4f,
0x31,0x00,0x02,
0x33,0x01,0xde,
0x33,0x04,0xfc,
0x34,0x00,0x00,
0x34,0x04,0x00,
0x36,0x00,0xc0,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
                
0x30,0x12,0x10, 
0x30,0x23,0x06, 
0x30,0x26,0x03, 
0x30,0x27,0x04, 
0x30,0x2a,0x03, 
0x30,0x2b,0x39, 
0x30,0x75,0x24, 
0x30,0x0d,0x01, 
0x30,0xd7,0x90, 
0x30,0x69,0x04, 
0x30,0x3e,0x00, 
0x30,0x3f,0xc0, 
0x33,0x02,0xef, 
0x33,0x5f,0x34, 
0x33,0x60,0x0c, 
0x33,0x61,0x04, 
0x33,0x62,0x34, 
0x33,0x63,0x08, 
0x33,0x64,0x04, 
0x34,0x03,0x42, 
0x30,0x88,0x04, 
0x30,0x89,0x00, 
0x30,0x8a,0x03, 
0x30,0x8b,0x00, 
0x30,0x8d,0x04, 
0x30,0x86,0x03, 
0x30,0x86,0x00, 

};


#endif
static code UINT8 frontSnapCmdTbl[] = {
#if 1 //QXGA
	0x30,0x12,0x00,
	0x30,0x0e,0x39,
	0x30,0x0f,0x21,
	0x30,0x10,0x20,
	0x30,0x11,0x01,
	0x30,0x4c,0x81,
	0x30,0x20,0x01,
	0x30,0x21,0x1d,
	0x30,0x22,0x00,
	0x30,0x23,0x0a,
#if 1
	0x30,0x24,0x08,
	0x30,0x25,0x18,
	0x30,0x26,0x06,
	0x30,0x27,0x0c,
#else
	0x30,0x24,WIDTH_SNAP>>8,      //0x08,
	0x30,0x25,WIDTH_SNAP&&0xFF,   //0x18,
	0x30,0x26,HEIGHT_SNAP>>8,     //0x06,
	0x30,0x27,HEIGHT_SNAP&&0xFF,  //0x0c,
#endif
	0x30,0x2a,0x06,
	0x30,0x2b,0x20,
	0x30,0x75,0x44,
	0x30,0x0d,0x00,
	0x30,0xd7,0x10,
	0x30,0x69,0x44,
	0x30,0x3e,0x01,
	0x30,0x3f,0x80,
	0x33,0x02,0xef,
	0x33,0x5f,0x68,
	0x33,0x60,0x18,
	0x33,0x61,0x0C,
	0x33,0x62,0x68,
	0x33,0x63,0x08,
	0x33,0x64,0x04,
	0x33,0x66,0x10,
	0x34,0x03,0x42,
	0x30,0x88,0x08,
	0x30,0x89,0x00,
	0x30,0x8a,0x06,
	0x30,0x8b,0x00,
	0x30,0x13,0xf2,
#else//XGA
	0x30,0x12,0x10,
	//0x30,0x77,0x06,
	0x30,0x23,0x06,
	0x30,0x26,0x03,
	0x30,0x27,0x04,
	0x30,0x2a,0x03,
	0x30,0x2b,0x10,
	0x30,0x75,0x24,
	0x30,0x0d,0x01,
	0x30,0xd7,0x90,
	0x30,0x69,0x04,
	0x30,0x3e,0x00,
	0x30,0x3f,0xc0,
	0x33,0x02,0xef,
	0x33,0x5f,0x34,
	0x33,0x60,0x0c,
	0x33,0x61,0x04,
	0x33,0x62,0x34,
	//0x33,0x62,0x12,
	0x33,0x63,0x08,
	//0x33,0x63,0x88,
	0x33,0x64,0x04,
	//0x33,0x64,0xe4,
	0x34,0x03,0x42,
	0x30,0x88,0x04,
	//0x30,0x88,0x12,
	0x30,0x89,0x00,
	//0x30,0x89,0x80,
	0x30,0x8a,0x03,
	//0x30,0x8a,0x01,
	0x30,0x8b,0x00,
	//0x30,0x8b,0xe0,
	0x30,0x4c,0x82,
	//0x30,0x4c,0x85,
	0x30,0x8d,0x04,
	0x30,0x86,0x03,
	0x30,0x86,0x00,
#endif
};

static code UINT8 frontSuspendCmdTbl[] = {
	0x12,0x00,
};

static code UINT8 frontResumeCmdTbl[] = {
	0x12,0x00,
};

static xdata UINT8 frontPrevMode = 0;
static xdata UINT8 frontSnapMode = 0;

static xdata UINT16 G_ExpLine = 250;
static xdata UINT16 G_ExpGain = 1;

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
	UINT8 id;
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

	id = frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));
	if ( id != NULL ) {
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET, frontOpAeTargetLuminanceSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET, frontOpAeExposureCompensationSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AE_FRAME_RATE_SET, frontOpAeFrameRateSet);
		frontExtendOpFuncSet(id, FRONT_EXT_OP_AWB_MODE_SET, frontOpAwbModeSet);
	}

	return id;
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


	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	/* set gpio. */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);
#if 0
	/* sensor reset start */
	sp1kFrexCfg();					//Jim@0903 - M_SHT
	sp1kFrexSet(0);    // Power Down Active high
	sp1kExpstbCfg();
	sp1kExpstbSet(0);  // RESET active low
#endif
	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* set yuv interface. */
	frontParaSet(FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG, YUV_SEQ, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_CCIR_MODE, CCIR_MODE, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, Y_SUB_128, U_SUB_128, V_SUB_128, 0, 0);
	frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 1, 0, 0, 0, 0);
#if 0
	globalTimerWait(1);
	sp1kExpstbSet(1);
#endif
	i2cCmdTableSend(frontInit0CmdTbl, 2, sizeof(frontInit0CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(200);
	i2cCmdTableSend(frontInit1CmdTbl, 2, sizeof(frontInit1CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
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


	printf("return to prev set 1!\n");

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width*2, pcap->height, pcap->xoff*2, pcap->yoff, 0);
	//i2cCmdTableSend(frontInit0CmdTbl, 2, sizeof(frontInit0CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	//globalTimerWait(200);
	//i2cCmdTableSend(frontInit1CmdTbl, 2, sizeof(frontInit1CmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	//i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	printf("return to prev set 2!\n");

	i2cCmdTableSend(front30fpsPrevCmdTbl, 2, sizeof(front30fpsPrevCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);

//	frontAllReg16Dump(1);
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
static void
frontOpSnapModeSet(
	void
)
{
	frontSnapCapabDesc_t *pcap;


	pcap = &(frontSnapCapabDesc[frontSnapMode]);
	printf("\nenter snap mode set!\n");

	//frontSnapExposureTimeGet();

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width*2, pcap->height, pcap->xoff*2, pcap->yoff, 0);

	printf("\nsend sensor cmd for snap!\n");
	i2cCmdTableSend(frontSnapCmdTbl, 2, sizeof(frontSnapCmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);
	printf("dead in while(1) for snap tuning!\n");
//	while(1)
//	;
//	frontSnapExposureTimeSet();
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
	UINT8 first
)
{	
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
	UINT8 val;


	val = (UINT8)parg;
	//printf("val=%bd\n", val);

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
	UINT8 val;


	#if 0
	UINT8 high_level, thr_level, low_level;
	UINT8 cmdTbl[2*3];
	static code SINT8 ae_comp[41] = {
	#if 1
		-40,-38,-36,-34,-32,
		-30,-28,-26,-24,-22,
		-20,-18,-16,-14,-12,
		-10, -8, -6, -4, -2,
		  0,
		  2,  4,  6,  8, 10,
		 12, 14, 16, 18, 20,
		 22, 24, 26, 28, 30,
		 32, 34, 36, 38, 40
	#else
		-90 ,	-88 ,	-84 ,	-81 ,	-78 ,
		-74 ,	-69 ,	-66 ,	-63 ,	-58 ,
		-53 ,	-48 ,	-43 ,	-39 ,	-33 ,
		-27 ,	-21 ,	-15 ,	-9	,	-4	,
		0	,
		3	,	8	,	17	,	30	,	36	,
		41	,	47	,	53	,	59	,	64	,
		70	,	76	,	82	,	87	,	93	,
		99	,	105 ,	110 ,	115 ,	120 ,
	#endif
	};

	UINT8 addrTbl[3], dataTbl[3];

	addrTbl[0] = 0x24; dataTbl[0] = 0;
	addrTbl[1] = 0x25; dataTbl[1] = 0;
	addrTbl[2] = 0x26; dataTbl[2] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 3, 1, 0, 0);
	printf("high = 0x%bx, low =0x%bx, thr = 0x%bx \n", dataTbl[0],dataTbl[1],dataTbl[2]);

	#endif

	val = (UINT8)parg;

	#if 0
	if (val > 40) val = 0;

	high_level = 0x70 + ae_comp[val] + 8;
	low_level = 0x70 + ae_comp[val] - 8;
	thr_level = ((high_level / 10 + 1) & 0x0F) * 16 + ((low_level / 10 - 6) & 0x0F) ;

	cmdTbl[0] = 0x24;
	cmdTbl[1] = high_level;
	cmdTbl[2] = 0x25;
	cmdTbl[3] = low_level;
	cmdTbl[4] = 0x26;
	cmdTbl[5] = thr_level;

	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	#endif
	//printf("val=%bd\n", val);

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
	UINT16 val;
	UINT8 oldmode, mode;
	UINT8 addrTbl[1], dataTbl[1], cmdTbl[1*2];

	val = (UINT16)parg;
#if 0
	if((val == SP1K_FRAME_RATE_30) || (val == SP1K_FRAME_RATE_15)) mode = 1;
	if((val == SP1K_FRAME_RATE_25) || (val == SP1K_FRAME_RATE_12P5)) mode = 0;

	addrTbl[0] = 0x0C; dataTbl[0] = 0;
	//i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);

	oldmode = (dataTbl[0] & 0x04);

	if ((mode == 0) && (oldmode == 0)){
		dataTbl[0]= 0x3c;//50hz
		cmdTbl[0] = 0x0c; cmdTbl[1] = dataTbl[0];
	//	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
	else if ((mode == 1) && (oldmode == 0x04)){
		dataTbl[0] = 0x38;//60hz
		cmdTbl[0] = 0x0c; cmdTbl[1] = dataTbl[0];
	//	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
#endif
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
	UINT8 mode;
	UINT8 rgain, bgain, reg13, oldmode, modeChEn;
	UINT8 addrTbl[2], dataTbl[2], cmdTbl[2*3];
#if 0
	mode = (UINT8)parg;

	addrTbl[0] = 0x13; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);
	reg13 = dataTbl[0];

	addrTbl[0] = 0x01; dataTbl[0] = 0;
	addrTbl[1] = 0x02; dataTbl[1] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 2, 1, 0, 0);

	printf("AWB Enable = %bx, bgain = %bu, rgain = %bu\n", reg13, dataTbl[0], dataTbl[1]);

	oldmode = (reg13 & 0x02);
	modeChEn = 1;

	#if 0 // for AWB calibration
	mode = SP1K_AWB_MODE_AUTO;
	oldmode = 0;
	#endif

	switch(mode){
		default:
		case SP1K_AWB_MODE_AUTO:
			if (oldmode != 0)
				modeChEn = 0;
			reg13 |= 0x02;
			bgain = 0x80;
			rgain = 0x80;
			break;

		case SP1K_AWB_MODE_DAYLIGHT:
			reg13 &= 0xfd;
			bgain = 0x80;
			rgain = 0x80;
			break;

		case SP1K_AWB_MODE_CLOUDY:
			reg13 &= 0xfd;
			bgain = 0x80;
			rgain = 0x90;
			break;

		case SP1K_AWB_MODE_TUNGSTENG:
			reg13 &= 0xfd;
			bgain = 0xc8;
			rgain = 0x80;
			break;

		case SP1K_AWB_MODE_FLUORESCENT:
			reg13 &= 0xfd;
			bgain = 0x98;
			rgain = 0xb3;
			break;
	}

	if (modeChEn){
		if(mode == SP1K_AWB_MODE_AUTO) {
			cmdTbl[0] = 0x13;cmdTbl[1] = reg13;
			i2cCmdTableSend(cmdTbl, 1, 1, I2C_TRANS_MODE_NORMAL, 0, 0);
		}
		else {
			cmdTbl[0] = 0x01;cmdTbl[1] = bgain;
			cmdTbl[2] = 0x02;cmdTbl[3] = rgain;
			cmdTbl[4] = 0x13;cmdTbl[5] = reg13;
			i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
		}
	}
	printf("AWB Enable = %bx, bgain = %bu, rgain = %bu\n", reg13, dataTbl[0], dataTbl[1]);

	//printf("mode=%bd\n", mode);
#endif
}

/**
 * @brief	get snap exposure time.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontSnapExposureTimeGet(
	void
)
{
	UINT8 addrTbl[4], dataTbl[4];


#if 0
	addrTbl[0] = 0x45; dataTbl[0] = 0;
	addrTbl[1] = 0x10; dataTbl[1] = 0;
	addrTbl[2] = 0x04; dataTbl[2] = 0;
	addrTbl[3] = 0x00; dataTbl[3] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 4, 1, 0, 0);
	G_ExpLine = (UINT16) (dataTbl[0] & 0x3F) * 1024 + (UINT16)(dataTbl[1] * 4) + (UINT16)(dataTbl[2] & 0x03);
	//G_ExpLine = G_ExpLine / 2;
	G_ExpGain = dataTbl[3];
	if((G_ExpGain & 0x80) != 0) G_ExpLine *= 2;
	if((G_ExpGain & 0x40) != 0) G_ExpLine *= 2;
	if((G_ExpGain & 0x20) != 0) G_ExpLine *= 2;
	G_ExpGain &= 0x1F;

	if(G_ExpLine == 0) G_ExpLine = 1;
#endif
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
__STATIC void
frontSnapExposureTimeSet(
	void
)
{
	UINT8 cmdTbl[2*4], addrTbl[4], dataTbl[4];


#if 0
	addrTbl[0] = 0x45;
	addrTbl[1] = 0x10;
	addrTbl[2] = 0x04;
	addrTbl[3] = 0x00;
//	i2cRegRead(addrTbl, dataTbl, 1, 4, 1, 0, 0);

	cmdTbl[0] = 0x45;
	cmdTbl[1] = (G_ExpLine>>10)&0x3f;
	cmdTbl[2] = 0x10;
	cmdTbl[3] = (G_ExpLine>>2)&0xff;
	cmdTbl[4] = 0x04;
	cmdTbl[5] = (G_ExpLine&0x03)|0xe8;
	cmdTbl[6] = 0x00;
	cmdTbl[7] = G_ExpGain;

	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
#endif
}

/**
 * @brief	reduce saturation.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-17
 * @todo	N/A
 * @bug		N/A
*/
static void
frontColorNoiseReduce(
	UINT8 gain
)
{

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
		i2cReg16Read(addrTbl, dataTbl, dataLen, 1, 1, 0, 0);
//		printf("reg[0x%04X] =", regIdx);
		for ( dataIdx = 0 ; dataIdx < dataLen; dataIdx++ ) {
//			printf(" 0x%02bX", dataTbl[dataIdx]);
		}
//		printf("\n");
	}
}


