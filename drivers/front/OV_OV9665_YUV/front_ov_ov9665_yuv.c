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
 * @file		front_ov_ov9665_yuv.c
 * @brief		front sensor omni vision ov9655 driver with yuv format data.
 * @author		CS Lin
 * @since		2008-11-24
 * @date		2008-11-27
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
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       113
#define YOFF_30FPS_PREV       27
#define LINE_TOT_30FPS_PREV   760
#define FRAME_TOT_30FPS_PREV  526

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      1280
#define HEIGHT_SNAP     1024
#define XOFF_SNAP       180//180
#define YOFF_SNAP       18//70
#define LINE_TOT_SNAP   1520
#define FRAME_TOT_SNAP  1052

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_EXT
#define PCLK_SRC  FRONT_PCLK_SRC_INT

#define MCLK_DIV_30FPS_PREV    4
#define MCLK_PHASE_30FPS_PREV  0
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0x08//0

#if 0
#define MCLK_DIV_60FPS_PREV    4
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    		4
#define MCLK_DIV_SNAP_BURST		4
#define MCLK_DIV_SNAP_BURST_UP	4

#define MCLK_PHASE_SNAP  0
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  0x08//0

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  27000000L
#if 0
#define PCLK_FREQ_60FPS_PREV  0
#endif
#define PCLK_FREQ_SNAP        27000000L
#else
#define PCLK_FREQ_30FPS_PREV  96000000L / MCLK_DIV_30FPS_PREV / PCLK_DIV_30FPS_PREV
#if 0
#define PCLK_FREQ_60FPS_PREV  96000000L / MCLK_DIV_60FPS_PREV / PCLK_DIV_60FPS_PREV
#endif
#define PCLK_FREQ_SNAP        96000000L / MCLK_DIV_SNAP / PCLK_DIV_SNAP
#define PCLK_FREQ_SNAP_BURST  	96000000L / MCLK_DIV_SNAP_BURST / PCLK_DIV_SNAP_BURST   
#define PCLK_FREQ_SNAP_BURST_UP 96000000L / MCLK_DIV_SNAP_BURST_UP / PCLK_DIV_SNAP_BURST_UP   
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    1
#define RESHAPE_HFALL_30FPS_PREV  39
#define RESHAPE_HRISE_30FPS_PREV  40
#define RESHAPE_VEN_30FPS_PREV    1
#define RESHAPE_VFALL_30FPS_PREV  2
#define RESHAPE_VRISE_30FPS_PREV  3

#if 0
#define RESHAPE_HEN_60FPS_PREV    0
#define RESHAPE_HFALL_60FPS_PREV  0
#define RESHAPE_HRISE_60FPS_PREV  0
#define RESHAPE_VEN_60FPS_PREV    0
#define RESHAPE_VFALL_60FPS_PREV  0
#define RESHAPE_VRISE_60FPS_PREV  0
#endif

#define HSYNC_INV_PREV  0
#define VSYNC_INV_PREV  0

#define RESHAPE_HEN_SNAP    1
#define RESHAPE_HFALL_SNAP  39
#define RESHAPE_HRISE_SNAP  40
#define RESHAPE_VEN_SNAP    1
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  0
#define VSYNC_INV_SNAP  0

/* bayer pattern. */
#define BAYER_PTN_PREV  0x00
#define BAYER_PTN_SNAP  0x00

/* yuv interface. */
#define YUV_SEQ    FRONT_YUV_OUTPUT_YUYV
#define CCIR_MODE  FRONT_YUV_CCIR601
#define Y_SUB_128  0
#define U_SUB_128  1
#define V_SUB_128  1

/* i2c interface. */
#define I2C_DEV_ADDR  0x60
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

#if 0
#define AE_60FPS_30_MAX_IDX  0
#define AE_60FPS_30_MIN_IDX  0
#define AE_60FPS_25_MAX_IDX  0
#define AE_60FPS_25_MIN_IDX  0
#define EV10_60FPS_EXP_IDX   0
#endif

/* gain. */
#define GAIN_30FPS_MAX_IDX  46
#define GAIN_30FPS_MIN_IDX  5

#if 0
#define GAIN_60FPS_MAX_IDX  0
#define GAIN_60FPS_MIN_IDX  0

#define GAIN_LARGE   0
#define GAIN_MIDDLE  0
#define GAIN_SMALL   0
#endif

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
__STATIC void frontOpSnapShot(UINT8 first,UINT8 mode,UINT8 scaleUp);
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

static void frontSnapExposureTimeSet(void);
static void frontColorNoiseReduce(UINT8 gain);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 frontDevName[] = "SENSOR - OV OV9665 YUV";

/* ae table. */
static code frontAe_t ae30fps60hzTbl[1] = {
	0,
};

static code frontAe_t ae30fps50hzTbl[1] = {
	0,
};

#if 0
static code frontAe_t ae60fps60hzTbl[0] = {
};

static code frontAe_t ae60fps50hzTbl[0] = {
};
#endif

/* gain table. */
static code UINT16 gain30fpsTbl[1] = {
	0,
};

#if 0
static code UINT16 gain60fpsTbl[0] = {
};
#endif

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

#if 0
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
	#if 0
	{  /* 60 fps. */
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
		gain60fpsTbl,
		GAIN_60FPS_MAX_IDX,
		GAIN_60FPS_MIN_IDX,
	},
	#endif
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
	0x12, 0x80,
};
static code UINT8 frontInit1CmdTbl[] = 
{
//IO output
	0xd5, 0xff,
	0xd6, 0x3f,
//Clock 24MHz 
	0x3d, 0x3c,
	0x11, 0x81,
// 25 fps 
	//0x2a, 0x12,
	//0x2b, 0x20,
// 30 fps 
	0x2a, 0x00,
	0x2b, 0x00,
//Power control	
	0x3a, 0xf1,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//Sync signal	
	0x15, 0x00,
//Data Format YUV	
	0xD7, 0x10,
	0x6a, 0x24,
	0x85, 0xe7,
//Sample Option
	0x63, 0x00,
//Windowing	
	0x12, 0x40,
	0x14, 0x50,
	0x0c, 0xb8,
	0x4d, 0x09,
	0x5c, 0x80,
	0x17, 0x0c,
	0x18, 0x5c,
	0x19, 0x02,
	0x1a, 0x3f,
	0x03, 0x03,
	0x32, 0xad,
	0x5a, 0x23,
//	0x2b, 0x00,
	0x4c, 0x3c,
//BLC
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9f,
	0x64, 0xa4,
//AEC; 	Average;9 zone;	
	0x43, 0x00, 
	0x5D, 0x55,
	0x5E, 0x57,
	0x5F, 0x21,
//Brightness	
	0x24, 0x5a,
	0x25, 0x4a,
	0x26, 0x71,
//LC enable	Largon9306;	
	0x7d, 0x30,	
	0x7e, 0x00,	
	0x7f, 0x00,
	0x83, 0x07,
	0x80, 0x08,
	0x81, 0x08, 
	0x82, 0x08,
/*	0x83, 0x07,
	0x80, 0x03,
	0x81, 0x04,*/
//AWB advance	Largon9306;
	0x96, 0xf0,
	0x97, 0x0a,
	0x92, 0x33,
	0x94, 0x5a,
	0x93, 0x3a,
	0x95, 0x48,
	0x91, 0xfc, 
	0x90, 0xff, 
	0x8e, 0x4e, 
	0x8f, 0x4e, 
	0x8d, 0x13, 
	0x8c, 0x0c, 
	0x8b, 0x0c, 
	0x86, 0x9e, 
	0x87, 0x11, 
	0x88, 0x22, 
	0x89, 0x05,  
	0x8a, 0x03, 
//;; Gamma enable       								
	0x9b, 0x0e,      								
	0x9c, 0x1a,      								
	0x9d, 0x2f,      								
	0x9e, 0x4f,      								
	0x9f, 0x60,
	0xa0, 0x6e,
	0xa1, 0x7d,
	0xa2, 0x8b,
	0xa3, 0x97,
	0xa4, 0xa0,
	0xa5, 0xb0,
	0xa6, 0xbe,
	0xa7, 0xd2,
	0xa8, 0xe2,
	0xa9, 0xee,
	0xaa, 0x18,
//De-noise enable auto
	0xb0, 0xc3,
	0xac, 0x03,// 01
	0x84, 0x40, 
//Sharpness	
	0xad, 0x82, 
	0xd9, 0x11, 
	0xda, 0x00, 
	0xae, 0x10, 
//;Scaling	
	0xab, 0xe7,
	0xb9, 0x50,
	0xba, 0x3c,
	0xbb, 0x50,
	0xbc, 0x3c,
//;CMX     	
	0xbd, 0x08,   	
	0xbe, 0x19,   	
	0xbf, 0x02,   	
	0xc0, 0x08,   	
	0xc1, 0x2a,   
	0xc2, 0x34,  	
	0xc3, 0x2e,  	
	0xc4, 0x2d,  	
	0xc5, 0x00,  	
	0xc6, 0x98,  	
	0xc7, 0x18,  	
	0x69, 0x48,      
//;UVave     	  
	0x74, 0xc0, 
//;UVadj     	    	
	0x7c, 0x28,     
	0x65, 0x11,     
	0x66, 0x00,     
	0x41, 0xc0,     
	0x5b, 0x24,     
	0x60, 0x82,  	  
	0x05, 0x07,  	  
	0x03, 0x0f,  	  
	0xd2, 0x94,  	  
//SAT & Brightness	
	0xc7, 0x90,
	0xc8, 0x06,    
	0xcb, 0x40,    
	0xcc, 0x40,  
	0xcf, 0x00,  
	0xd0, 0x1a,  // 1a
	0xd1, 0x00,  
	0xc7, 0x18,  
	
	0x0d, 0x92,
	0x0d, 0x90,
//mirror
//	0x04, 0xe8,  
//	0x33, 0xc8,  
//mirror
	0x04, 0xa8,  
	0x33, 0xc0,  
//output timing
	0xd8, 0xE4,
	0xd7, 0x04,//0x14,
};

static code UINT8 front30fpsPrevCmdTbl[] = {
//size
	0x12, 0x40,
	0x17, 0x0c,
	0x18, 0x5d,
	0x19, 0x02,
	0x1a, 0x3f,
	0x03, 0x03,
	0x32, 0xad,
	0xab, 0xe7,
	0xb9, 0x50,
	0xba, 0x3c,
	0xbb, 0x50,
	0xbc, 0x3c,
	0x0d, 0x92,
	0x0d, 0x90,
//Banding
	0x14, 0x5e,
	0x0C, 0x3C, 
	0x4F, 0x9d,
	0x50, 0x83,
	0x5A, 0x23,
//;Power control
	0x3a, 0xf1,	
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
/*//@@ Contrast 0.25x  
	0xc7, 0x3c,        
	0xc8, 0x06,        
	0x64, 0x24,        
	0xd0, 0x1c,  // 1a
//@@ Color Saturation
	0xc7, 0x18, 
	0xcb, 0x40,   
	0xcc, 0x40, */
//;SAT & Brightness     	   
	0xc8, 0x06,    
	0xcb, 0x40,    
	0xcc, 0x40,  
	0xcf, 0x00,  
	0xd0, 0x1a,  // 1a
	0xd1, 0x00,  
	0xc7, 0x18,  

//BLC
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9f,
	0x64, 0xa4,
//output mode 
	0xd7, 0x04,//0x14,output timing
	0x11, 0x80, //clock frame
	0x12, 0x40,//VGA mode 
	0x3d, 0x3c,  //
// 25 fps 
	//0x2a, 0x12,
	//0x2b, 0x20,
// 30 fps 
	0x2a, 0x00,
	0x2b, 0x00,
	
	0x2d, 0x00,  
	0x2e, 0x00,  
	0x13, 0xe7,  

};

static code UINT8 frontSnapCmdTbl[] = {
//;IO output
	0xd5, 0xff,
	0xd6, 0x3f,
//;Clock 12MHz 6.25FPS
	0x3d, 0x3c,
/*	0x11, 0x00, //6.25fps
//	0x2a, 0x10,//10
//	0x2b, 0x30, // 3b*/
	
	0x11, 0x81,/*7.5fps*/
	0x2a, 0x00,
	0x2b, 0x00, 
//Power control
	0x3a, 0xd9,
	0x3b, 0x00,
	0x3c, 0x58,
	0x3e, 0x50,
	0x71, 0x00,
//;Sync sinal  
	0x15, 0x00,
//Data Format YUV	
	0xD7, 0x10,
	0x6a, 0x24,
	0x85, 0xe7,
//;Sample Option	   	
	0x63, 0x01,
//;Windowing	
	0x17, 0x0c,
	0x18, 0x5c,
	0x19, 0x01,
	0x1a, 0x82,
	0x03, 0x0f,
//	0x2b, 0x00,
	0x32, 0x34,
//blc
	0x36, 0xb4,
	0x65, 0x10,
	0x70, 0x02,
	0x71, 0x9c,
	0x64, 0x24,
//;AEC;	Average;9 zone;	
	0x13, 0xe2,//e2 manual exposure  
	0x43, 0x00,
	0x5D, 0x55,
	0x5E, 0x57,  
	0x5F, 0x21,
//; Brightness	
	0x24, 0x5a,//62
	0x25, 0x4a,//52
	0x26, 0x71,
//;Banding
	0x14, 0x5E,
	0x0C, 0x3C, 
	0x4F, 0x9d,//9d
	0x50, 0x83,
	0x5A, 0x23,
//;LC enable;		
	0x7d, 0x30,
	0x7e, 0x18,
	0x82, 0x03,
	0x7f, 0x00,
	0x83, 0x07,
	0x80, 0x08,
	0x81, 0x08, 
	0x82, 0x08,
/*	0x83, 0x07,
	0x80, 0x03,
	0x81, 0x04, */
//;AWB advance;    
	0x96, 0xf0,
	0x97, 0x10, 
	0x92, 0x28, 
	0x94, 0x41, 
	0x93, 0x30, 
	0x95, 0x42, 
	0x91, 0xae, 
	0x90, 0x93, 
	0x8e, 0x4d, 
	0x8f, 0x4e, 
	0x8d, 0x09, 
	0x8c, 0x08, 
	0x8b, 0x0b, 								
	0x86, 0x9e, 								
	0x87, 0x11, 								
	0x88, 0x22, 								
	0x89, 0x15, 								
	0x8a, 0x03, 																
//;; Gamma enable       								
	0x9b, 0x0e,      								
	0x9c, 0x1a,      								
	0x9d, 0x2f,      								
	0x9e, 0x4f,      								
	0x9f, 0x60,
	0xa0, 0x6e,
	0xa1, 0x7d,
	0xa2, 0x8b,
	0xa3, 0x97,
	0xa4, 0xa0,
	0xa5, 0xb0,
	0xa6, 0xbe,
	0xa7, 0xd2,
	0xa8, 0xe2,
	0xa9, 0xee,
	0xaa, 0x18,
//;De-noise enable auto
	0xAB, 0xe7,      
	0xb0, 0xc3, //4
	0xac, 0x01, //04    
	0x84, 0x40,      
//;Sharpness
	0xad, 0x84,    
	0xd9, 0x24,    
	0xda, 0x00,    
	0xae, 0x10,    
//;Scaling      
	0xab, 0xe7,      
	0xb9, 0xa0,      
	0xba, 0x80,      
	0xbb, 0xa0,      
	0xbc, 0x80,  
//;CMX     	
	0xbd, 0x08,   	
	0xbe, 0x19,   	
	0xbf, 0x02,   	
	0xc0, 0x08,   	
	0xc1, 0x2a,   
	0xc2, 0x34,  	
	0xc3, 0x2e,  	
	0xc4, 0x2d,  	
	0xc5, 0x00,  	
	0xc6, 0x98,  	
	0xc7, 0x18,  	
	0x69, 0x48,      
//;UVave     	  
	0x74, 0xc0, 
//;UVadj     	    	
	0x7c, 0x28,     
	0x65, 0x11,     
	0x66, 0x00,     
	0x41, 0xc0,     
	0x5b, 0x24,     
	0x60, 0x82,  	  
	0x05, 0x07,  	  
	0x03, 0x0f,  	  
	0xd2, 0x94,  	  
//;SAT & Brightness     	   
	0xc8, 0x06,    
	0xcb, 0x40,    
	0xcc, 0x40,  
	0xcf, 0x00,  
	0xd0, 0x1a,  // 1a
	0xd1, 0x00,  
	0xc7, 0x18,  
//ouput mode
	0xd8, 0xE4,
	0xd7, 0x14,
	0x12, 0x00,//SXGA Mode
//;BLC
	0x0d, 0x82, 
	0x0d, 0x80, 
};

static code UINT8 frontSuspendCmdTbl[] = {
	0x11, 0x80,  

};

static code UINT8 frontResumeCmdTbl[] = {
	0x11, 0x80,  
};

static xdata UINT8 frontPrevMode = 0;
static xdata UINT8 frontSnapMode = 0;

static xdata UINT16 G_ExpLine = 250;
static xdata UINT16 G_ExpGain = 1;

#if 0
static UINT16 frontLastGain;
static UINT32 frontLastShutter;
static UINT8 frontGainClass = 0xFF;
#endif

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

	/* sensor reset start */
	sp1kFrexCfg();					//Jim@0903 - M_SHT
	sp1kFrexSet(0);    // Power Down Active high
	sp1kExpstbCfg();
	sp1kExpstbSet(0);  // RESET active low

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);
	//XBYTE[0x2019] = 0x04;

	/* set i2c. */
	i2cDeviceAddrSet(I2C_DEV_ADDR);
	i2cClkCfgSet(I2C_CLK_DIV);

	/* set yuv interface. */
	frontParaSet(FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG, YUV_SEQ, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_CCIR_MODE, CCIR_MODE, 0, 0, 0, 0);
	frontParaSet(FRONT_PARA_YUV_SUB128_ENABLE, Y_SUB_128, U_SUB_128, V_SUB_128, 0, 0);
	frontParaSet(FRONT_PARA_YUV_INPUT_ENABLE, 1, 0, 0, 0, 0);

	/* sensor reset complete */
	globalTimerWait(1);
	sp1kExpstbSet(1);

	/* send i2c command. */
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	globalTimerWait(10);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
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


	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width*2, pcap->height, pcap->xoff*2, pcap->yoff, 0);

	i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);

	cdspBayerPatternSet(BAYER_PTN_PREV);
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
__STATIC void frontSnapExposureTimeGet(void);

__STATIC void
frontOpSnapModeSet(
	void
)
{
	frontSnapCapabDesc_t *pcap;

	pcap = &(frontSnapCapabDesc[frontSnapMode]);

	frontSnapExposureTimeGet();

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_SNAP, VSYNC_INV_SNAP, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width*2, pcap->height, pcap->xoff*2, pcap->yoff, 0);
	printf("...........write I2C\n" );
//	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
//	globalTimerWait(100);
	i2cCmdTableSend(frontSnapCmdTbl, 1, sizeof(frontSnapCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	frontSnapExposureTimeSet();

	cdspBayerPatternSet(BAYER_PTN_SNAP);
	
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	printf("...........warit stop\n" );
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
	first=first;
	mode=mode;
	scaleUp=scaleUp;
	
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); // 20081024 mantis 32544 wenguo
	cdspSkipAeWr(1);
	cdspSkipAwbWr(1);
	cdspSkipDistEdgeWr(1);		
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
	if((val == SP1K_FRAME_RATE_30) || (val == SP1K_FRAME_RATE_15)) mode = 1;//60hz
	if((val == SP1K_FRAME_RATE_25) || (val == SP1K_FRAME_RATE_12P5)) mode = 0;//50hz
	
	addrTbl[0] = 0x0C; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);
	
	oldmode = (dataTbl[0] & 0x04);

	if ((mode == 0) && (oldmode == 0)){
		dataTbl[0]= 0x3c;//50hz
		cmdTbl[0] = 0x0c; cmdTbl[1] = dataTbl[0];
		i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
	else if ((mode == 1) && (oldmode == 0x04)){
		dataTbl[0] = 0x38;//60hz
		cmdTbl[0] = 0x0c; cmdTbl[1] = dataTbl[0];
		i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	}
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

	mode = (UINT8)parg;

	addrTbl[0] = 0x13; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);
	reg13 = dataTbl[0];

	addrTbl[0] = 0x01; dataTbl[0] = 0;
	addrTbl[1] = 0x02; dataTbl[1] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 2, 1, 0, 0);
	//printf("AWB Enable = %bx, bgain = %bu, rgain = %bu\n", reg13&0x02, dataTbl[0], dataTbl[1]);
	printf("081215 AWB Enable = %bx, bgain = %bu, rgain = %bu\n", reg13, dataTbl[0], dataTbl[1]);
	
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

	// NOTE: use variable "mode" for your purpose.
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
frontSnapExposureTimeGet(
	void
)
{
	UINT8 addrTbl[4], dataTbl[4];

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
}

__STATIC void
frontSnapExposureTimeSet(
	void
)
{
	UINT8 cmdTbl[2*4], addrTbl[4], dataTbl[4];

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
	cmdTbl[5] = (G_ExpLine&0x03)|0xa8;
	cmdTbl[6] = 0x00;
	cmdTbl[7] = G_ExpGain;
	
	i2cCmdTableSend_Intr(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	
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

#if 0
/**
 * @brief	frontMirrorFlipSet.
 * @param	MFmode = [in] Mirror or flip mode, has value as SENSOR_NORMAL, SENSOR_ROTATE_180, SENSOR_MIRROR, SENSOR_FLIP
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontMirrorFlipSet(
	UINT8 MFmode
)
{
	static UINT8 reg1e = 0x34;
	UINT8 cmdTbl[2*1];
	frontPrevCapabDesc_t *pcap;

	switch(MFmode){
		default:
		case SENSOR_NORMAL:
			reg1e &= 0xcf;
			break;

		case SENSOR_ROTATE_180:
			reg1e |= 0x30;
			break;

		case SENSOR_MIRROR:
			reg1e &= 0xcf;
			reg1e |= 0x20;
			break;

		case SENSOR_FLIP:
			reg1e &= 0xcf;
			reg1e |= 0x10;
			break;
	};

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	cmdTbl[0] = 0x1e;
	cmdTbl[1] = reg1e;
	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
}

/**
 * @brief	frontFrameRateSet
 * @param	fps = [in] frame rate setting, for 30, 25, 20, 15 fps only
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontFrameRateSet(
	UINT8 fps
)
{
	UINT16 dummyLine = 0;
	UINT8 cmdTbl[2*2];
	frontPrevCapabDesc_t *pcap;

	if (fps >= 30)			//set 30fps
			dummyLine = 0;
	else if(fps >= 25)		//set 25fps
			dummyLine = 100;
	else if(fps >= 20)		//set 20fps
			dummyLine = 250;
	else					//set 15fps
			dummyLine = 500;

	pcap = &(frontPrevCapabDesc[frontPrevMode]);

	cmdTbl[0] = 0x92;
	cmdTbl[1] = (UINT8)(dummyLine&0x00ff);
	cmdTbl[2] = 0x93;
	cmdTbl[3] = (UINT8)(dummyLine>>8);

	i2cCmdTableSend(cmdTbl, 1, sizeof(cmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
}

/**
 * @brief	frontEffectModeSet
 * @param	mode = [in] SEPIA, BW, ...
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontEffectModeSet(
	UINT8 mode
)
{
}

/**
 * @brief	frontFrameRateSet
 * @param	mode = [in] AUTO_MODE, SUNSHINE, CLOUDY, TUNGSTEN_LAMP, FLUORESCENT_LAMP
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-11-24
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontWhiteBalanceModeSet(
	UINT8 mode
)
{
	UINT16 rgain, bgain, reg13,
	UINT8 modeChEn;
	static UINT8 oldmode = 0;

	cmos_rd(sensorSscSlaveAddr, 0x13, &reg13);

	oldmode = (reg13 & 0x02);
	modeChEn = 1;

	switch(mode){
		default:
		case AUTO_MODE:
			if (oldmode != 0)
				modeChEn = 0;
			reg13 |= 0x02;
			bgain = 0x80;
			rgain = 0x80;
			break;

		case SUNSHINE:
			reg13 &= 0xfd;
			bgain = 0x80;
			rgain = 0x80;
			break;

		case CLOUDY:
			reg13 &= 0xfd;
			bgain = 0x80;
			rgain = 0x90;
			break;

		case TUNGSTEN_LAMP:
			reg13 &= 0xfd;
			bgain = 0xc8;
			rgain = 0x80;
			break;

		case FLUORESCENT_LAMP:
			reg13 &= 0xfd;
			bgain = 0x98;
			rgain = 0xb3;
			break;
	}
	if (modeChEn){
		cmos_wr(sensorSscSlaveAddr, 0x01, bgain);
		cmos_wr(sensorSscSlaveAddr, 0x02, rgain);
		cmos_wr(sensorSscSlaveAddr, 0x13, reg13);
	}
}

/**************************************************************************************
*  Function Name : sensor_NightModeSet
*  Desprictions : Night mode enable/disable
*  Arguments :
*   mode: 0 :disable, 1:enable
*  Returns :
*  See also :
***************************************************************************************/
void sensor_NightModeSet(UINT32 mode)
{
	UINT32 reg3b, oldmode;

	cmos_rd(sensorSscSlaveAddr, 0x3b, &reg3b);
	oldmode = (reg3b & 0x80);

	if ((mode == NIGHT_MODE_ON) && (oldmode == 0)){
		reg3b |= 0xc0;
		cmos_wr(sensorSscSlaveAddr, 0x24, 0x3f);
		cmos_wr(sensorSscSlaveAddr, 0x25, 0x33);
		cmos_wr(sensorSscSlaveAddr, 0x3b, reg3b);
	}
	else if ((mode == NIGHT_MODE_OFF) && (oldmode != 0)){
		reg3b &= 0x1f;
		cmos_wr(sensorSscSlaveAddr, 0x3b, reg3b);
		cmos_wr(sensorSscSlaveAddr, 0x2e, 0);
		cmos_wr(sensorSscSlaveAddr, 0x2d, 0);
		cmos_wr(sensorSscSlaveAddr, 0x24, 0x3c);
		cmos_wr(sensorSscSlaveAddr, 0x25, 0x36);
	}
}


/**************************************************************************************
*  Function Name : sensor_PwrFreqModeSet
*  Desprictions : Set power freq. mode, de-flicker.
*  Arguments :
*   mode: 0 :50Hz, 1:60Hz
*  Returns :
*  See also :
***************************************************************************************/
void sensor_PwrFreqModeSet(UINT32 mode)
{
	UINT32 reg42, oldmode;

	cmos_rd(sensorSscSlaveAddr, 0x42, &reg42);
	oldmode = (reg42 & 0x01);

	if ((mode == POWER_FREQ_50HZ) && (oldmode == 0)){
		reg42 |= 0x01;
		cmos_wr(sensorSscSlaveAddr, 0x42, reg42);
	}
	else if ((mode == POWER_FREQ_60HZ) && (oldmode == 1)){
		reg42 &= 0xfe;
		cmos_wr(sensorSscSlaveAddr, 0x42, reg42);
	}
}


/**************************************************************************************
*  Function Name : sensor_BrightValueSet
*  Desprictions : Adjust sensor output brightness
*  Arguments :
*   value: adiust value
*  Returns :
*  See also :
***************************************************************************************/
void sensor_BrightValueSet(INT8 value)
{
	UINT32 regVal;

	printf("Brightness = %d\n", value);

	if (value >= 0)
		regVal = (UINT32)value;
	else
		regVal = (UINT32)(-value | 0x80);

	printf("0x55 = 0x%x\n", regVal);
	cmos_wr(sensorSscSlaveAddr, 0x55, (UINT32)regVal);



//	printf("Brightness = %d\n", value);

//	for(i=0; i<256; i++){
//		cmos_rd(sensorSscSlaveAddr, i, &regVal);
//		printf("0x%x = 0x%x\n", i, regVal);
//	}

//for get wb gain
/*
	cmos_rd(sensorSscSlaveAddr, 0x01, &regVal);
	printf("AWB B Gain = %d\n", regVal);
	cmos_rd(sensorSscSlaveAddr, 0x02, &regVal);
	printf("AWB R Gain = %d\n", regVal);
*/
}


/**************************************************************************************
*  Function Name : sensor_LedCtlSet
*  Desprictions :
*  Arguments :
*  Returns :
*  See also :
***************************************************************************************/
void sensor_LedCtlSet(UINT32 on_off)
{
}

#endif

