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
 * @file		front_gc_gc0307_yuv.c
 * @brief		front sensor gc0307 driver with yuv format data.
 * @author		Matt Wang
 * @since		2008-11-17
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

#include "gpio_api.h"
#include "i2c.h"
#include "hal_cdsp_iq.h"
#include "hal_front.h"
#include "hal_front_para.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* mode. */
#define PREV_MODE_TOT  1
#define SNAP_MODE_TOT  1

/* dimensiion. */
#define WIDTH_30FPS_PREV      640
#define HEIGHT_30FPS_PREV     480
#define XOFF_30FPS_PREV       0x04
#define YOFF_30FPS_PREV       0x02//110
#define LINE_TOT_30FPS_PREV   640
#define FRAME_TOT_30FPS_PREV  480

#if 0
#define WIDTH_60FPS_PREV      0
#define HEIGHT_60FPS_PREV     0
#define XOFF_60FPS_PREV       0
#define YOFF_60FPS_PREV       0
#define LINE_TOT_60FPS_PREV   0
#define FRAME_TOT_60FPS_PREV  0
#endif

#define WIDTH_SNAP      640
#define HEIGHT_SNAP     480
#define XOFF_SNAP       0x04
#define YOFF_SNAP       0x02
#define LINE_TOT_SNAP   640
#define FRAME_TOT_SNAP  480

/* clk. */
#define MCLK_SRC  FRONT_MCLK_SRC_EXT
#define PCLK_SRC  FRONT_PCLK_SRC_INT

#define MCLK_DIV_30FPS_PREV    0x04
#define MCLK_PHASE_30FPS_PREV  0x00
#define PCLK_DIV_30FPS_PREV    1
#define PCLK_PHASE_30FPS_PREV  0x05

#if 0
#define MCLK_DIV_60FPS_PREV    4
#define MCLK_PHASE_60FPS_PREV  0
#define PCLK_DIV_60FPS_PREV    1
#define PCLK_PHASE_60FPS_PREV  0
#endif

#define MCLK_DIV_SNAP    4
#define MCLK_PHASE_SNAP  2
#define PCLK_DIV_SNAP    1
#define PCLK_PHASE_SNAP  5

#if PCLK_SRC == FRONT_PCLK_SRC_EXT
#define PCLK_FREQ_30FPS_PREV  24000000L
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
#endif

/* reshape. */
#define RESHAPE_HEN_30FPS_PREV    0
#define RESHAPE_HFALL_30FPS_PREV  0x02
#define RESHAPE_HRISE_30FPS_PREV  0x04
#define RESHAPE_VEN_30FPS_PREV    0
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

#define HSYNC_INV_PREV  1
#define VSYNC_INV_PREV  1

#define RESHAPE_HEN_SNAP    0
#define RESHAPE_HFALL_SNAP  2
#define RESHAPE_HRISE_SNAP  4
#define RESHAPE_VEN_SNAP    0
#define RESHAPE_VFALL_SNAP  2
#define RESHAPE_VRISE_SNAP  3

#define HSYNC_INV_SNAP  1
#define VSYNC_INV_SNAP  1

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
#define I2C_DEV_ADDR  0x42
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

#define frontOpAeTargetLuminanceSet       frontSensorOpAeTargetLuminanceSet
#define frontOpAeExposureCompensationSet  frontSensorOpAeExposureCompensationSet
#define frontOpAeFrameRateSet             frontSensorOpAeFrameRateSet
#define frontOpAwbModeSet                 frontSensorOpAwbModeSet

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
__STATIC void frontOpSnapShot(UINT8 first, UINT8 mode, UINT8 scaleUp);
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
static code UINT8 frontDevName[] = "SENSOR - GC GC0307 YUV";

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

#if 0

static code UINT8 frontInit0CmdTbl[] = {

0x43  ,0x00,
0x44  ,0xa2,
0x40  ,0x10,
0x41  ,0x00,
0x42  ,0x30,
0x47  ,0x00,
0x48  ,0xc3,
0x49  ,0x00,
0x4a  ,0x00,
0x4b  ,0x00,
0x4E  ,0x23,
0x4F  ,0x01,
0x01  ,0x74,
0x02  ,0x70,
0x1C  ,0x00,
0x1D  ,0x00,
0x10  ,0x00,
0x11  ,0x05,
0x05  ,0x00,
0x06  ,0x00,
0x07  ,0x00,
0x08  ,0x00,
0x09  ,0x01,
0x0A  ,0xE8,
0x0B  ,0x02,
0x0C  ,0x80,
0x0D  ,0x22,
0x0E  ,0x02,
0x0F  ,0x82,
0x12  ,0x70,
0x13  ,0x00,
0x14  ,0x00,
0x15  ,0xba,
0x16  ,0x13,
0x17  ,0x52,
0x18  ,0xc0,
0x1E  ,0x0d,
0x1F  ,0x32,
0x47  ,0x00,
0x19  ,0x06,
0x1a  ,0x06,
0x31  ,0x00,
0x3B  ,0x00,
0x59  ,0x0f,
0x58  ,0x88,
0x57  ,0x08,
0x56  ,0x77,
0x35  ,0xd8,

};

static code UINT8 frontInit1CmdTbl[] = {

0x36  ,0x40,
0x3C  ,0x00,
0x3D  ,0x00,
0x3E  ,0x00,
0x3F  ,0x00,
0xb5  ,0x70,
0xb6  ,0x40,
0xb7  ,0x00,
0xb8  ,0x38,
0xb9  ,0xc3,
0xba  ,0x0f,
0x7e  ,0x35,
0x7f  ,0x86,
0x5c  ,0x68,
0x5d  ,0x78,
0x61  ,0x80,
0x63  ,0x80,
0x65  ,0x98,
0x67  ,0x80,
0x68  ,0x18,
0x69  ,0x58,
0x6A  ,0xf6,
0x6B  ,0xfb,
0x6C  ,0xf4,
0x6D  ,0x5a,
0x6E  ,0xe6,
0x6f  ,0x00,
0x70  ,0x14,
0x71  ,0x1c,
0x72  ,0x20,
0x73  ,0x10,
0x74  ,0x3c,
0x75  ,0x52,
0x7d  ,0x2f,
0x80  ,0x0c,
0x81  ,0x0c,
0x82  ,0x44,
0x83  ,0x18,
0x84  ,0x18,
0x85  ,0x04,
0x87  ,0x34,
0x88  ,0x04,
0x89  ,0x01,
0x8a  ,0x50,
0x8b  ,0x50,
0x8c  ,0x07,
0x50  ,0x0c,
0x5f  ,0x3c,

};

static code UINT8 frontInit2CmdTbl[] = {



0x8e  ,0x02,
0x86  ,0x02,
0x51  ,0x20,
0x52  ,0x08,
0x53  ,0x00,
0x77  ,0x80,
0x78  ,0x00,
0x79  ,0x00,
0x7a  ,0x00,
0x7b  ,0x40,
0x7c  ,0x00,
0xa0  ,0x40,
0xa1  ,0x40,
0xa2  ,0x34,
0xa3  ,0x34,
0xa4  ,0xc8,
0xa5  ,0x02,
0xa6  ,0x28,
0xa7  ,0x02,
0xa8  ,0xee,
0xa9  ,0x12,
0xaa  ,0x01,
0xab  ,0x20,
0xac  ,0xf0,
0xad  ,0x10,
0xae  ,0x18,
0xaf  ,0x74,
0xb0  ,0xe0,
0xb1  ,0x20,
0xb2  ,0x6c,
0xb3  ,0x40,
0xb4  ,0x04,
0xbb  ,0x42,
0xbc  ,0x60,
0xbd  ,0x50,
0xbe  ,0x50,
0xbf  ,0x0c,
0xc0  ,0x06,
0xc1  ,0x70,
0xc2  ,0xf1,
0xc3  ,0x40,
0xc4  ,0x20,
0xc5  ,0x33,
0xc6  ,0x1d,
0xca  ,0x80,
0xcb  ,0x80,
0xcc  ,0x80,
0xcd  ,0x80,
0xce  ,0x80,
0xcf  ,0x80,
0x20  ,0x02,
0x21  ,0xc0,
0x22  ,0x60,
0x23  ,0x88,

};

static code UINT8 frontInit3CmdTbl[] = {

0x24  ,0x96,
0x25  ,0x30,
0x26  ,0xd0,
0x27  ,0x00,
0x28  ,0x02,
0x29  ,0x58,
0x2a  ,0x03,
0x2b  ,0x84,
0x2c  ,0x09,
0x2d  ,0x60,
0x2e  ,0x0a,
0x2f  ,0x8c,
0x30  ,0x20,
0x31  ,0x00,
0x32  ,0x1c,
0x33  ,0x90,
0x34  ,0x10,
0xd0  ,0x34,
0xd1  ,0x50,
0xd2  ,0xf2,
0xd4  ,0x96,
0xd5  ,0x10,
0xd6  ,0x96,
0xd7  ,0x10,
0xd8  ,0x02,
0xdd  ,0x02,
0xe0  ,0x03,
0xe1  ,0x02,
0xe2  ,0x27,
0xe3  ,0x1e,
0xe8  ,0x3b,
0xe9  ,0x6e,
0xea  ,0x2c,
0xeb  ,0x50,
0xec  ,0x73,
0xed  ,0x00,
0xee  ,0x00,
0xef  ,0x00,
0xf0  ,0x01,
0x00  ,0x20,
0x01  ,0x20,
0x02  ,0x20,
0x03  ,0x20,
0x04  ,0x78,
0x05  ,0x78,
0x06  ,0x78,
0x07  ,0x78,


};

static code UINT8 frontInit4CmdTbl[] = {

0x10  ,0x04,
0x11  ,0x04,
0x12  ,0x04,
0x13  ,0x04,
0x14  ,0x01,
0x15  ,0x01,
0x16  ,0x01,
0x17  ,0x01,
0x20  ,0x00,
0x21  ,0x00,
0x22  ,0x00,
0x23  ,0x00,
0x24  ,0x00,
0x25  ,0x00,
0x26  ,0x00,
0x27  ,0x00,
0x40  ,0x11,
0x45  ,0x06,
0x46  ,0x06,
0x47  ,0x05,
0x48  ,0x04,
0x49  ,0x03,
0x4a  ,0x03,
0x62  ,0xd8,
0x63  ,0x24,
0x64  ,0x24,
0x65  ,0x24,
0x66  ,0xd8,
0x67  ,0x24,
0x5a  ,0x00,
0x5b  ,0x00,
0x5c  ,0x00,
0x5d  ,0x00,
0x5e  ,0x00,
0x5f  ,0x00,
0x69  ,0x03,
0x70  ,0x5d,
0x71  ,0xed,
0x72  ,0xff,
0x73  ,0xe5,
0x74  ,0x5f,
0x75  ,0xe6,
0x76  ,0x41,
0x77  ,0xef,
0x78  ,0xff,
0x79  ,0xff,

};

static code UINT8 frontInit5CmdTbl[] = {

0x7a  ,0x5f,
0x7b  ,0xfa,
0x7e  ,0x00,
0x7f  ,0x00,
0x80  ,0xc8,
0x81  ,0x06,
0x82  ,0x08,
0x83  ,0x23,
0x84  ,0x38,
0x85  ,0x4F,
0x86  ,0x61,
0x87  ,0x72,
0x88  ,0x80,
0x89  ,0x8D,
0x8a  ,0xA2,
0x8b  ,0xB2,
0x8c  ,0xC0,
0x8d  ,0xCA,
0x8e  ,0xD3,
0x8f  ,0xDB,
0x90  ,0xE2,
0x91  ,0xED,
0x92  ,0xF6,
0x93  ,0xFD,
0x94  ,0x04,
0x95  ,0x0E,
0x96  ,0x1B,
0x97  ,0x28,
0x98  ,0x35,
0x99  ,0x41,
0x9a  ,0x4E,
0x9b  ,0x67,
0x9c  ,0x7E,
0x9d  ,0x94,
0x9e  ,0xA7,
0x9f  ,0xBA,
0xa0  ,0xC8,
0xa1  ,0xD4,
0xa2  ,0xE7,
0xa3  ,0xF4,
0xa4  ,0xFA,
0xf0  ,0x00,
0x45  ,0x24,
0x40  ,0x7e,
0x41  ,0x2F,
0x47  ,0x20,
0x43  ,0x40,
0x44  ,0xE2,

};


#else 
/*pccam*/

static code UINT8 frontInit0CmdTbl[] = {
0xf0,0x00,
0x00,0x99,
0x01,0x6A,
0x02,0x70,
0x03,0x05,
0x04,0x46,
0x05,0x00,
0x06,0x00,
0x07,0x00,
0x08,0x00,
0x09,0x01,
0x0A,0xEa,//0x0A,0xE8,
0x0B,0x02,
0x0C,0x80,//0x0C,0x80,
0x0D,0x22,
0x0E,0x02,//0x42,zouchd
0x0F,0xB2,
0x10,0x00,
0x11,0x05,
0x12,0x70,
0x13,0x00,//0x13,0x80,
};

static code UINT8 frontInit1CmdTbl[] = {

0x14,0x00,
0x15,0xBA,
0x16,0x13,
0x17,0x52,
0x18,0xC0,
0x19,0x06,
0x1A,0x06,
0x1B,0x00,
0x1C,0x02,
0x1D,0x02,
0x1E,0x0D,
0x1F,0x32,
0x20,0x06,//0x02,
0x21,0xC0,
0x22,0x40,
0x23,0x88,
0x24,0x96,
0x25,0x30,
0x26,0xD0,
0x27,0x00,
0x28,0x02,
0x29,0x58,
0x2A,0x03,
0x2B,0x84,
0x2C,0x07,
0x2D,0x08,
0x2E,0x0d,
0x2F,0x7a,
0x30,0x20,
0x31,0x00,
0x32,0x04,
0x33,0x90,
0x34,0x10,
0x35,0xD8,
0x36,0x40,
0x37,0x02,
0x38,0x02,
0x39,0x02,
0x3A,0x02,
0x3B,0x00,
0x3C,0x00,
0x3D,0x00,
0x3E,0x00,

};

static code UINT8 frontInit2CmdTbl[] = {


0x3F,0x00,
0x40,0x7E,
0x41,0x2F,
0x42,0x10,
0x43,0x40,
0x44,0xE2,
0x45,0x27,
0x46,0x42,
0x47,0x2C,
0x48,0xC3,
0x49,0x00,
0x4A,0x00,
0x4B,0x00,
0x4C,0x00,
0x4D,0x00,
0x4E,0x23,
0x4F,0x01,
0x50,0x0C,
0x51,0x20,
0x52,0x08,
0x53,0x00,
0x54,0x00,
0x55,0x00,
0x56,0x77,
0x57,0x08,
0x58,0x88,
0x59,0x0F,
0x5A,0x40,
0x5B,0x3A,
0x5C,0x68,
0x5D,0x78,
0x5E,0x00,
0x5F,0x00,
0x60,0x00,
0x61,0x80,
0x62,0x00,
0x63,0x80,
0x64,0x00,
0x65,0x98,
0x66,0x00,
0x67,0x80,
0x68,0x14,
0x69,0x46,
0x6A,0xF8,

};

static code UINT8 frontInit3CmdTbl[] = {


0x6B,0xF4,
0x6C,0xFc,
0x6D,0x53,
0x6E,0xE7,
0x6F,0x00,
0x70,0x10,
0x71,0x1C,
0x72,0x20,
0x73,0x10,
0x74,0x3C,
0x75,0x52,
0x76,0x00,
0x77,0x80,
0x78,0x00,
0x79,0x00,
0x7A,0x00,
0x7B,0x40,
0x7C,0x00,
0x7D,0x0F,
0x7E,0x35,
0x7F,0x86,
0x80,0x0C,
0x81,0x0C,
0x82,0x88,
0x83,0x18,
0x84,0x18,
0x85,0x04,
0x86,0x02,
0x87,0x34,
0x88,0x04,
0x89,0x01,
0x8A,0x50,
0x8B,0x50,
0x8C,0x07,
0x8D,0x67,
0x8E,0x02,
0x8F,0x1A,
0x90,0x2C,
0x91,0x41,
0x92,0x51,
0x93,0x61,

};

static code UINT8 frontInit4CmdTbl[] = {


0x94,0x6F,
0x95,0x7C,
0x96,0x92,
0x97,0xA4,
0x98,0xB4,
0x99,0xC0,
0x9A,0xCC,
0x9B,0xD5,
0x9C,0xDE,
0x9D,0xEB,
0x9E,0xF5,
0x9F,0xFC,
0xA0,0x40,
0xA1,0x40,
0xA2,0x34,
0xA3,0x34,
0xA4,0xC8,
0xA5,0x02,
0xA6,0x28,
0xA7,0x02,
0xA8,0xEE,
0xA9,0x12,
0xAA,0x01,
0xAB,0x20,
0xAC,0xF0,
0xAD,0x10,
0xAE,0x18,
0xAF,0x74,
0xB0,0xBF,
0xB1,0x20,
0xB2,0x6C,
0xB3,0x40,
0xB4,0x04,
0xB5,0x70,
0xB6,0x40,
0xB7,0x00,
0xB8,0x38,
0xB9,0xC3,
0xBA,0x0F,

};

static code UINT8 frontInit5CmdTbl[] = {

0xBB,0x42,
0xBC,0x60,
0xBD,0x50,
0xBE,0x50,
0xBF,0x0C,
0xC0,0x06,
0xC1,0x70,
0xC2,0xF1,
0xC3,0x40,
0xC4,0x20,
0xC5,0x33,
0xC6,0x1D,
0xC7,0x4B,
0xC8,0x40,
0xC9,0x44,
0xCA,0x70,
0xCB,0x70,
0xCC,0x78,
0xCD,0x80,
0xCE,0x80,
0xCF,0x80,
0xD0,0x34,
0xD1,0x50,//lumi
0xD2,0x61,//0xF2,
0xD3,0x46,
0xD4,0x96,
0xD5,0x01,//0x10,
0xD6,0x96,//0xD7,0x96,
0xD7,0x96,//0xD8,0x00,
//0xD5,0x01,
//0xD7,0x04,
0xD8,0x00,
0xD9,0x00,
0xDA,0x00,
0xDB,0x4F,
0xDC,0x40,
0xDD,0x22,
0xDE,0x00,
0xDF,0x00,
0xE0,0x03,
0xE1,0x02,
0xE2,0x27,

};

static code UINT8 frontInit6CmdTbl[] = {


0xE3,0x1E,
0xE4,0x00,
0xE5,0x00,
0xE6,0x00,
0xE7,0x00,
0xE8,0x3B,
0xE9,0x6E,
0xEA,0x2C,
0xEB,0x50,
0xEC,0x73,
0xED,0x00,
0xEE,0x00,
0xEF,0x00,
0xF0,0x00,
0xF1,0x03,
0xF2,0x33,
0xF3,0x00,
0xF4,0x00,
0xF5,0x00,
0xF6,0x00,
0xF7,0x00,
0xF8,0x00,
0xF9,0x00,
0xFA,0x00,
0xFB,0x00,
0xFC,0x00,
0xFD,0x00,
0xFE,0x00,

0xf0,0x01,
0x00,0x20,
0x01,0x20,
0x02,0x20,
0x03,0x20,
0x04,0x78,
0x05,0x78,
0x06,0x78,
0x07,0x78,
0x08,0x00,
0x09,0x00,
0x0A,0x00,
0x0B,0x00,
0x0C,0x00,
0x0D,0x00,
0x0E,0x00,
0x0F,0x00,
0x10,0x04,
0x11,0x04,
0x12,0x04,
0x13,0x04,
0x14,0x01,
0x15,0x01,


};

static code UINT8 frontInit7CmdTbl[] = {


0x16,0x01,
0x17,0x01,
0x18,0x00,
0x19,0x00,
0x1A,0x00,
0x1B,0x00,
0x1C,0x00,
0x1D,0x00,
0x1E,0x00,
0x1F,0x00,
0x20,0x00,
0x21,0x00,
0x22,0x00,
0x23,0x00,
0x24,0x00,
0x25,0x00,
0x26,0x00,
0x27,0x00,
0x28,0x00,
0x29,0x00,
0x2A,0x00,
0x2B,0x00,
0x2C,0x00,
0x2D,0x00,
0x2E,0x00,
0x2F,0x00,
0x30,0x00,
0x31,0x00,
0x32,0x00,
0x33,0x00,
0x34,0x00,
0x35,0x00,
0x36,0x00,
0x37,0x00,
0x38,0x00,
0x39,0x00,
0x3A,0x00,
0x3B,0x00,
0x3C,0x00,
0x3D,0x00,
0x3E,0x00,
0x3F,0x00,
0x40,0x11,
0x41,0x00,
0x42,0x00,
0x43,0x00,
0x44,0x00,
0x45,0x06,
0x46,0x06,
0x47,0x05,
0x48,0x04,
0x49,0x03,
0x4A,0x03,
0x4B,0x00,
0x4C,0x00,
0x4D,0x00,
0x4E,0x00,
0x4F,0x00,
0x50,0x00,

};

static code UINT8 frontInit8CmdTbl[] = {


0x51,0x00,
0x52,0x00,
0x53,0x00,
0x54,0xFF,
0x55,0x01,
0x56,0xFF,
0x57,0x01,
0x58,0x00,
0x59,0x00,
0x5A,0x3C,
0x5B,0x3E,
0x5C,0x3E,
0x5D,0x36,
0x5E,0x38,
0x5F,0x39,
0x60,0x00,
0x61,0x00,
0x62,0xD8,
0x63,0x24,
0x64,0x24,
0x65,0x24,
0x66,0xD8,
0x67,0x24,
0x68,0x00,
0x69,0x03,
0x6A,0x00,
0x6B,0x00,
0x6C,0x00,
0x6D,0x00,
0x6E,0x00,
0x6F,0x00,
0x70,0x5D,
0x71,0xED,
0x72,0xFF,
0x73,0xE5,
0x74,0x5F,
0x75,0xE6,
0x76,0x41,
0x77,0xEF,
0x78,0xFF,
0x79,0xFF,
0x7A,0x5F,
0x7B,0xFA,
0x7C,0x00,
0x7D,0x00,
0x7E,0x11,
0x7F,0x00,
0x80,0xC8,


};

static code UINT8 frontInit9CmdTbl[] = {

0x81,0x06,
0x82,0x08,
0x83,0x23,
0x84,0x38,
0x85,0x4F,
0x86,0x61,
0x87,0x72,
0x88,0x80,
0x89,0x8D,
0x8A,0xA2,
0x8B,0xB2,
0x8C,0xC0,
0x8D,0xCA,
0x8E,0xD3,
0x8F,0xDB,
0x90,0xE2,
0x91,0xED,
0x92,0xF6,
0x93,0xFD,
0x94,0x04,
0x95,0x0E,
0x96,0x1B,
0x97,0x28,
0x98,0x35,
0x99,0x41,
0x9A,0x4E,
0x9B,0x67,
0x9C,0x7E,
0x9D,0x94,
0x9E,0xA7,
0x9F,0xBA,
0xA0,0xC8,
0xA1,0xD4,
0xA2,0xE7,
0xA3,0xF4,
0xA4,0xFA,
0xA5,0x00,
0xA6,0x00,
0xA7,0x00,
0xA8,0x00,
0xA9,0x00,
0xAA,0x00,
0xAB,0x00,
0xAC,0x00,
0xAD,0x00,
0xAE,0x00,
0xAF,0x00,
0xB0,0x49,
0xB1,0x4C,
0xB2,0x4D,
0xB3,0x2B,

};

static code UINT8 frontInit10CmdTbl[] = {

0xB4,0x2C,
0xB5,0x2D,
0xB6,0x2A,
0xB7,0x2C,
0xB8,0x2D,
0xB9,0x35,
0xBA,0x37,
0xBB,0x37,
0xBC,0x5E,
0xBD,0x5F,
0xBE,0x61,
0xBF,0x33,
0xC0,0x35,
0xC1,0x36,
0xC2,0x30,
0xC3,0x31,
0xC4,0x30,
0xC5,0x42,
0xC6,0x43,
0xC7,0x44,
0xC8,0x49,
0xC9,0x4C,
0xCA,0x4D,
0xCB,0x00,
0xCC,0x00,
0xCD,0x00,
0xCE,0x00,
0xCF,0x00,
0xD0,0x00,
0xD1,0x00,
0xD2,0x00,
0xD3,0x00,
0xD4,0x00,
0xD5,0x00,
0xD6,0x00,
0xD7,0x00,
0xD8,0x00,
0xD9,0x00,
0xDA,0x00,
0xDB,0x00,
0xDC,0x00,
0xDD,0x00,
0xDE,0x00,
0xDF,0x00,
0xE0,0x00,
0xE1,0x00,
0xE2,0x00,
0xE3,0x00,
0xE4,0x00,
0xE5,0x00,
0xE6,0x00,
0xE7,0x00,
0xE8,0x00,
0xE9,0x00,
0xEA,0x00,


};

static code UINT8 frontInit11CmdTbl[] = {



0xEB,0x00,
0xEC,0x00,
0xED,0x00,
0xEE,0x00,
0xEF,0x00,
0xF0,0x01,
0xF1,0x00,
0xF2,0x00,
0xF3,0x00,
0xF4,0x00,
0xF5,0x00,
0xF6,0x00,
0xF7,0x00,
0xF8,0x00,
0xF9,0x00,
0xFA,0x00,
0xFB,0x00,
0xFC,0x00,
0xFD,0x00,
0xFE,0x00,

0xf0,0x00,

};

#endif





/*
static code UINT8 front30fpsPrevCmdTbl[] = {
	0x68, 0x04,
	0xDC, 0xC0,
	0x19, 0x00,
	0x1A, 0x00,
	0x03, 0x00,
	0x04, 0xF0,
};
*/


static UINT8 frontPrevMode = 0;
static UINT8 frontSnapMode = 0;

#if 0
static UINT16 frontLastGain;
static UINT32 frontLastShutter;
static UINT8 frontGainClass = 0xFF;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

#if 1/*for cmd seting GC0307 register*/

UINT8 gc0307reg[2] = {
0x00,0x00,
};
void writegc0307reg(UINT8 addr,UINT8 value)
{
	gc0307reg[0] = addr;
	gc0307reg[1] = value;
	i2cCmdTableSend(gc0307reg, 1, sizeof(gc0307reg)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
}

#endif


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

	/* set clk. */
	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 1, 0);
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

	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);
	
	/* send i2c command. */
	/* send i2c command. */
	globalTimerWait(20);
	i2cCmdTableSend(frontInit0CmdTbl, 1, sizeof(frontInit0CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit1CmdTbl, 1, sizeof(frontInit1CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit2CmdTbl, 1, sizeof(frontInit2CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit3CmdTbl, 1, sizeof(frontInit3CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit4CmdTbl, 1, sizeof(frontInit4CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit5CmdTbl, 1, sizeof(frontInit5CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
#if 1

	i2cCmdTableSend(frontInit6CmdTbl, 1, sizeof(frontInit6CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit7CmdTbl, 1, sizeof(frontInit7CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit8CmdTbl, 1, sizeof(frontInit8CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit9CmdTbl, 1, sizeof(frontInit9CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit10CmdTbl, 1, sizeof(frontInit10CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	i2cCmdTableSend(frontInit11CmdTbl, 1, sizeof(frontInit11CmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	
#endif

	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);

	
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

	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);

	frontParaSet(FRONT_PARA_MCLK_CFG, MCLK_SRC, pcap->mclkDiv, pcap->mclkPhase, 1, 0);
	frontParaSet(FRONT_PARA_PCLK_CFG, PCLK_SRC, pcap->pclkDiv, pcap->pclkPhase, 0, 0);
	frontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, HSYNC_INV_PREV, VSYNC_INV_PREV, 0, 0, 0);

	frontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
	frontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
	frontParaSet(FRONT_PARA_CROP_REGION, pcap->width*2, pcap->height, pcap->xoff*2, pcap->yoff, 0);

	//i2cCmdTableSend(front30fpsPrevCmdTbl, 1, sizeof(front30fpsPrevCmdTbl)/2, I2C_TRANS_MODE_NORMAL, 0, 0);
	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);

	cdspBayerPatternSet(BAYER_PTN_PREV);

	printf("here is in %s,%ld\n",__FILE__,(UINT32)__LINE__);
	
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
 * @brief	operating function to open shutter.
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
 * @brief	operating function to close shutter.
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
 * @since	2008-11-26
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
	UINT8 high_level;
	UINT8 low_level;
	UINT8 cmdTbl[2*2];
	static code SINT8 ae_comp[41] = {
	#if 0
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

	UINT8 addrTbl[2], dataTbl[2];

	addrTbl[0] = 0x24; dataTbl[0] = 0;
	addrTbl[0] = 0x25; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 2, 1, 0, 0);
	printf("high = %bu, low = %bu \n", dataTbl[0],dataTbl[1]);

	#endif

	val = (UINT8)parg;

	#if 0
	if (val > 40) val = 0;

	high_level = 0x70 + ae_comp[val] + 8;
	low_level = 0x70 + ae_comp[val] - 8;
	cmdTbl[0] = 0x24;
	cmdTbl[1] = high_level;
	cmdTbl[2] = 0x25;
	cmdTbl[3] = low_level;

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
 * @since	2008-11-26
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
frontOpAeFrameRateSet(
	void *parg
)
{
	UINT16 val;


	val = (UINT16)parg;
	//printf("val=%d\n", (UINT16)parg);

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

#if 0

	UINT8 mode;
	UINT8 rgain, bgain, reg13, oldmode, modeChEn;
	UINT8 addrTbl[1], dataTbl[1], cmdTbl[2*3];

	mode = (UINT8)parg;

	addrTbl[0] = 0x13; dataTbl[0] = 0;
	i2cRegRead(addrTbl, dataTbl, 1, 1, 1, 0, 0);
	reg13 = dataTbl[0];

	oldmode = (reg13 & 0x02);
	modeChEn = 1;

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


	//printf("mode=%bd\n", mode);

	// NOTE: use variable "mode" for your purpose.

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
static void
frontSnapExposureTimeSet(
	void
)
{

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

