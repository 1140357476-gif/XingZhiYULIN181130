/*******************************************************************************
*
*       Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "general.h"
#include "disp_api.h"
#include "gpio_api.h"
#include "reg_front.h"
#include "hal_global.h"
#include "i2c.h"
#include "sp1k_hal_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_gpio_api.h"
#include "hal_audio.h"
#include "ep952controller.h"
#include "dbg_wdt.h"
#if 1
#include "ep952api.h"
#ifdef printf
#undef printf
#endif
#endif

#include "host_func_opt.h"

#if FUNC_HOST_HDMI

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/

#if DISP_CCIR601_PROGRESSIVE
#define DISP_CCIR601_PROGRESSIVE_720p	1
#define DISP_CCIR601_PROGRESSIVE_720p25	0
#define DISP_CCIR601_PROGRESSIVE_480p	0
#define DISP_CCIR601_PROGRESSIVE_576p	0

#else
#define DISP_CCIR601_PROGRESSIVE_720p	0
#define DISP_CCIR601_PROGRESSIVE_720p25	0
#define DISP_CCIR601_PROGRESSIVE_480p	0
#define DISP_CCIR601_PROGRESSIVE_576p	0

#endif

/* render dimension. */
#if HDMI_12BIT_MODE

#if DISP_CCIR601_PROGRESSIVE_720p
#define DEV_NAME	"HDMI - EP952 1280X720P@30HZ"
#define DEV_FMT		(4)
#define DEV_TYPE	DISP_DEVICE_TYPE_CCIR601_PROGRESSIVE
#define FIELD_FMT	(DISP_PROGRESSIVE | 0x00)

#define WIDTH		(1280)
#define HEIGHT		(720)
#define HSYNCW		(40)
#define VSYNCW		(5)
#define HPIXEL		(1650)
#define VLINE		(750)
#define PNLH0		(220+40-1)
#define PNLV0		(21+5-1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      7
#define CLK_DIV1X      2

#endif
#if DISP_CCIR601_PROGRESSIVE_720p25
#define DEV_NAME	"HDMI - EP952 1280X720P@25HZ"
#define DEV_FMT		(19)
#define DEV_TYPE	DISP_DEVICE_TYPE_CCIR601_PROGRESSIVE
#define FIELD_FMT	(DISP_PROGRESSIVE | 0x00)

#define WIDTH		(1280)
#define HEIGHT		(720)
#define HSYNCW		(40)
#define VSYNCW		(5)
#define HPIXEL		(1980)
#define VLINE		(750)
#define PNLH0		(220+40-1)
#define PNLV0		(21+5-1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      7
#define CLK_DIV1X      2

#endif

#if DISP_CCIR601_PROGRESSIVE_480p
#define DEV_NAME	"HDMI - EP952 720X480P@60HZ"
#define DEV_FMT		(3)
#define DEV_TYPE       DISP_DEVICE_TYPE_CCIR601_PROGRESSIVE
#define FIELD_FMT	(DISP_PROGRESSIVE | 0x00)

#define WIDTH		(720)
#define HEIGHT		(480)
#define HSYNCW		(62)
#define VSYNCW		(6)
#define HPIXEL		(720+138)
#define VLINE		(480+45)
#define PNLH0		(60+62-1)
#define PNLV0		(31+6-1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      9

//#define CLK_SRC        DISP_CLK_SRC_XTAL27M
//#define CLK_DIVNX      1

#define CLK_DIV1X      2

#endif
#if DISP_CCIR601_PROGRESSIVE_576p
#define DEV_NAME	"HDMI - EP952 720X576P@50HZ"
#define DEV_FMT		(18)
#define DEV_TYPE       DISP_DEVICE_TYPE_CCIR601_PROGRESSIVE
#define FIELD_FMT	(DISP_PROGRESSIVE | 0x00)

#define WIDTH		(720)
#define HEIGHT		(576)
#define HSYNCW		(64)
#define VSYNCW		(5)
#define HPIXEL		(720+144)
#define VLINE		(576+49)
#define PNLH0		(68+64-1)
#define PNLV0		(40+5-1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      9

//#define CLK_SRC        DISP_CLK_SRC_XTAL27M
//#define CLK_DIVNX      1

#define CLK_DIV1X      2

#endif

#else
#define DEV_NAME	"HDMI - EP952 1280X720P@50HZ"
#define DEV_FMT		(19)
#define DEV_TYPE       DISP_DEVICE_TYPE_UPS051
#define FIELD_FMT	(DISP_PROGRESSIVE | 0x00)

#define WIDTH		1280
#define HEIGHT		720
#define HSYNCW		40
#define VSYNCW		5
#define HPIXEL		1980
#define VLINE		750
#define PNLH0		260
#define PNLV0		25

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      7
#define CLK_DIV1X      1

#endif

#define PNLH1          (PNLH0 + WIDTH - 1)
#define PNLV1          (PNLV0 + HEIGHT - 1)

/* pv size */
#define PV_SIZE_X 320
#define PV_SIZE_Y 240

/* image region */
#define PVX0      0
#define PVY0      0
#define PVX1      (PV_SIZE_X - 1)
#define PVY1      (PV_SIZE_Y - 1)
#define PVH0      PNLH0
#define PVV0      PNLV0
#define PVH1      PNLH1
#define PVV1      PNLV1

/* osd region */
#define OSDX0     0
#define OSDY0     0
#define OSDX1     (PV_SIZE_X - 1)
#define OSDY1     (PV_SIZE_Y - 1)
#define OSDH0     PNLH0 +28
#define OSDV0     PNLV0 +16
#define OSDH1     PNLH1 -28*2
#define OSDV1     PNLV1 -16*2

#define UPS_CLK_PHASE  8

#define CCIR_SEQ     DISP_CCIR_CBYCRY
#define FILTER_EVEN  DISP_UPS051_EVEN_BRG
#define FILTER_ODD   DISP_UPS051_ODD_BRG

#define HDMI_I2C_CLK_DIV   1

#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
/* function type definition. */
#define __STATIC  //static

/* operating function. */
#define dispOpDevNameGet     hdmiOpDevNameGet
#define dispOpCapabilityGet  hdmiOpCapabilityGet
#define dispOpPowerOn        hdmiOpPowerOn
#define dispOpPowerOff       hdmiOpPowerOff

typedef enum fieldRate_e {
	FIELD_RATE_SUPPORT_50HZ = (UINT8)(0x01 << 0),
	FIELD_RATE_SUPPORT_60HZ = (UINT8)(0x01 << 1),

	FIELD_RATE_SUPPORT_UNKNOWN = (UINT8)(0x00 << 0),
} fieldRate_t;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/

/* operating function. */
__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);
void hdmiDispColorMatrixInit(void);

void EP952_Open(void);

extern unsigned char is_AMute;
extern unsigned char is_VMute;
extern TX_STATE TX_State;
extern unsigned char hdmiVidTiming;
/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* device name. */

static code UINT8 dispDevName[] = DEV_NAME;

/* device capability. */
static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	16,
	9,
};

static UINT8 xdata i2cDevAddr;
static UINT8 xdata i2cClkDiv;

/**
 * @fn        void hdmiReset(void)
 * @brief     hdmiReset
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-1-6
 * @todo      N/A
 * @bug       N/A
*/
static void
hdmiReset(
	void
)
{
	//EP_EP932M_Reset(); //Phil: Already reset in EPM932 driver
}

/**
 * @fn        fieldRate_s hdmiFieldRateGet(void)
 * @brief     hdmiFieldRateGet
 * @param     NONE
 * @retval    return fieldRate_s
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-1-6
 * @todo      N/A
 * @bug       N/A
*/
static fieldRate_t
hdmiFieldRateGet(
	void
)
{
	UINT8 bSupport50Hz = 0;
	UINT8 bSupport60Hz = 0;
	fieldRate_t ret = FIELD_RATE_SUPPORT_UNKNOWN;

	/*
		After read the EDID from TV
		We should set the field rate below
	*/

	/* customized process start */

	bSupport50Hz = (hdmiVidTiming & 0x01) ? 1 : 0;
	bSupport60Hz = (hdmiVidTiming & 0x02) ? 1 : 0;

	/* customized process end */

	/* return value process */
	if (bSupport50Hz) {
		ret |= FIELD_RATE_SUPPORT_50HZ;
		printf("fRate 50Hz\n");
	}

	if (bSupport60Hz) {
		ret |= FIELD_RATE_SUPPORT_60HZ;
		printf("fRate 60Hz\n");
	}

	return ret;
}

/**
 * @fn        void hdmiBusReq(void)
 * @brief     hdmiBusReq
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
static void
hdmiBusI2CReq(
	void
)
{
	sp1kAeSetModeEn(0);

	/* configure I2C GPIO */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	//XBYTE[0x2065] = 0x29;

	i2cClkCfgGet(&i2cClkDiv);
	i2cDeviceAddrGet(&i2cDevAddr);

	i2cClkCfgSet(HDMI_I2C_CLK_DIV);
}

/**
 * @fn        void hdmiBusFree(void)
 * @brief     hdmiBusFree
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
static void
hdmiBusI2CFree(
	void
)
{
	i2cClkCfgSet(i2cClkDiv);
	i2cDeviceAddrSet(i2cDevAddr);

	sp1kAeSetModeEn(1);
}

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
UINT8
hdmiOpen(
	void
)
{
	dispOperatingFunc_t dispOpFunc;


	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;

	return dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));
}

/**
 * @brief	operating function to get device name (ascii string).
 * @param	None.
 * @retval	return = device name string.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8 *
hdmiOpDevNameGet(
	void
)
{
	return dispDevName;
}

/**
 * @brief	operating function to get device capability.
 * @param	*pcapabDesc = [out] capability describer.
 * @param   descSize = [in] describer structure size.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
hdmiOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc
)
{
	*pcapabDesc = dispCapabDesc;
}

void hdmiDispColorMatrixInit(void)
{
#if 1
    XBYTE[REG_Disp_ColA00L] = 0x4d;  /*7:0*/
    XBYTE[REG_Disp_ColA01L] = 0x96;  /*7:0*/
    XBYTE[REG_Disp_ColA02L] = 0x1d;  /*7:0*/
    XBYTE[REG_Disp_ColA10L] = 0xd3;  /*7:0*/
    XBYTE[REG_Disp_ColA11L] = 0xa8;  /*7:0*/
    XBYTE[REG_Disp_ColA12L] = 0x83;  /*7:0*/
    XBYTE[REG_Disp_ColA20L] = 0x83;  /*7:0*/
    XBYTE[REG_Disp_ColA21L] = 0x91;  /*7:0*/
    XBYTE[REG_Disp_ColA22L] = 0xea;  /*7:0*/
    XBYTE[REG_Disp_ColA00H01H] = 0x00;  /*10:8*/
    XBYTE[REG_Disp_ColA02H10H] = 0x70;  /*10:8*/
    XBYTE[REG_Disp_ColA11H12H] = 0x07;  /*10:8*/
    XBYTE[REG_Disp_ColA20H21H] = 0x70;  /*10:8*/
    XBYTE[REG_Disp_ColA22H] = 0x07;  /*10:8*/
    XBYTE[REG_Disp_ColB00H] = 0x00;  /*b00*/
    XBYTE[REG_Disp_ColB01H] = 0x7f;  /*b01*/
    XBYTE[REG_Disp_ColB02H] = 0x7f;  /*b02*/
    XBYTE[REG_Disp_ColB00L01L] = 0x70;  /*b01 b00*/
    XBYTE[REG_Disp_ColB02L] = 0x07;  /*b02*/
    //XBYTE[REG_Disp_ColMtxEnFmt] = 0x11;  /*cmffmt cmten*/
#else
	XBYTE[REG_Disp_ColA00L] = 0xe2;  /*7:0*/
	XBYTE[REG_Disp_ColA01L] = 0xb7;  /*7:0*/
	XBYTE[REG_Disp_ColA02L] = 0x26;  /*7:0*/
	XBYTE[REG_Disp_ColA10L] = 0xec;  /*7:0*/
	XBYTE[REG_Disp_ColA11L] = 0x00;  /*7:0*/
	XBYTE[REG_Disp_ColA12L] = 0xc8;  /*7:0*/
	XBYTE[REG_Disp_ColA20L] = 0xf8;  /*7:0*/
	XBYTE[REG_Disp_ColA21L] = 0xd8;  /*7:0*/
	XBYTE[REG_Disp_ColA22L] = 0xe7;  /*7:0*/

	XBYTE[REG_Disp_ColA00H01H] = 0x70;  /*10:8*/
	XBYTE[REG_Disp_ColA02H10H] = 0x70;  /*10:8*/
	XBYTE[REG_Disp_ColA11H12H] = 0x71;  /*10:8*/
	XBYTE[REG_Disp_ColA20H21H] = 0x77;  /*10:8*/
	XBYTE[REG_Disp_ColA22H] = 0x00;  /*10:8*/

	XBYTE[REG_Disp_ColMtxEnFmt] = 0x01;  /*cmffmt cmten*/
#endif

}


/**
 * @brief	operating function to power on device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
hdmiOpPowerOn(
	void
)
{
	fieldRate_t fRate;

	#if HDMI_12BIT_MODE
	SINT16 mtxA[9] = {0x004d,0x0096,0x001d,0x07d3,0x07a8,0x0083,0x0083,0x0791,0x07ea};
	SINT16 mtxB[3] = {0x0000,0x7f07,0x7f07};

	/*gpioPullSelSet(6,0xFF,0xFF);
	gpioPullEnSet(7,0xF0,0xF0);
	gpioPullEnSet(8,0x0F,0x0F);*/
	XBYTE[0x205d] = 0xFF;
	XBYTE[0x205e] = 0xFF;
	XBYTE[0x205f] = 0xff;
	XBYTE[0x2060] = 0x57;/**/
	//XBYTE[0x2065] &= ~0x3c;//I2C driving

	//-------------------------------------------------------------------------------
	dispParaSet(DISP_PARA_EDGE_GAIN_SET, 0x00, 0, 0, 0, 0); //disable edge enhancement
	dispParaSet(DISP_PARA_GLOB_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0);
	//-------------------------------------------------------------------------------

	//hdmiDispColorMatrixInit();/* for CCIR601 */
	dispParaSet(DISP_PARA_COLOR_MATRIX_TBL_CFG, (SINT16)mtxA, (SINT16)mtxB, 0, 0, 0);
	dispParaSet(DISP_PARA_COLOR_MATRIX_FMT_CFG, DISP_COLOR_MATRIX_FMT_YCBCR, 0, 0, 0, 0);
	#endif

	dispParaSet(DISP_PARA_GLOB_ENABLE, 0, 0, 0, 0, 0);
    	dispParaSet(DISP_PARA_SW_RST, DISP_DEV_RST_ALL, 0, 0, 0, 0);

	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);

	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
   	dispParaSet(DISP_PARA_DEVICE_IMG_REGION_CROP, PVX0, PVX1, PVY0, PVY1, 0);
    	dispParaSet(DISP_PARA_DEVICE_IMG_REGION_RENDER, PVH0, PVH1, PVV0, PVV1, 0);
    	dispParaSet(DISP_PARA_DEVICE_OSD_REGION_CROP, OSDX0, OSDX1, OSDY0, OSDY1, 0);
    	dispParaSet(DISP_PARA_DEVICE_OSD_REGION_RENDER, OSDH0, OSDH1, OSDV0, OSDV1, 0);

	dispParaSet(DISP_PARA_INTERLACE_CFG, FIELD_FMT, 0, 0, 0, 0);
	#if HDMI_12BIT_MODE
	/* set CCIR interface. */
	dispParaSet(DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG, CCIR_SEQ, 0, 0, 0, 0);
	dispParaSet(
		DISP_PARA_CLK_SIGNAL_POLAR_CFG, \
		DISP_SIGNAL_CLK_INVERSE | DISP_SIGNAL_VSYNC_ACT_HIGH | DISP_SIGNAL_HSYNC_ACT_HIGH, \
		0, 0, 0, 0
	);

   	XBYTE[REG_Disp_Rsv0] |= 0x03;  /*rsv fix*/

	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 1, 0, 0, 0, 0); //enable color matrix
	#else
	/* set ups interface. */
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_DELAY_CFG, DISP_UPS051_HSD_RGB, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS051_RGB888_ENABLE, 1, 0, 0, 0, 0); /* set RGB888 single clock rate mode */
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_RB_SWITCH_SET, 1, 0, 0, 0, 0); /* enable rb switch */


	dispParaSet(DISP_PARA_REQ_LENTH_SET, DISP_IMG_REQ_LENTH_32 | DISP_OSD_REQ_LENTH_32, 0, 0, 0, 0);
	dispParaSet(
		DISP_PARA_CLK_SIGNAL_POLAR_CFG, \
		DISP_SIGNAL_CLK_INVERSE | DISP_SIGNAL_VSYNC_ACT_HIGH | DISP_SIGNAL_HSYNC_ACT_HIGH, \
		0, 0, 0, 0
	);
	dispParaSet(DISP_PARA_EDGE_GAIN_SET, 0x00, 0, 0, 0, 0); //disable edge enhancement
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0); //disable color matrix
	#endif
	dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);

	/* configure I2C for HDMI */
	hdmiBusI2CReq();

	/* set I2S for HDMI audio (GPIO19 ~ 22)*/
	//sp1kHalAudI2SSet(1);
	//HAL_AudSoundPlayCfg(44100,16,1); //add for HDMI beep voice
	hdmiReset();

	EP952_Open();

	/* set I2S for HDMI audio (GPIO19 ~ 22)*/
	#if HDMI_12BIT_MODE
	XBYTE[0x201a] &= ~0x60;//gpio3,4 as GPIO for I2S data and clk
	#endif
	sp1kHalAudI2SSet(1);
	HAL_AudSoundPlayCfg(44100,16,1); //add for HDMI beep voice

	dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);

	fRate = hdmiFieldRateGet();
	if (!(fRate & FIELD_RATE_SUPPORT_50HZ)) {
		if ((fRate & FIELD_RATE_SUPPORT_60HZ)) {
			dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_60HZ, VLINE_60HZ, HSYNCW, VSYNCW, 0);
		} else {
			/* Read EDID Fail */
		}
	}

	/* free the I2C bus */
	hdmiBusI2CFree();

}

/**
 * @fn        void hdmiOpPowerOff(void)
 * @brief     operating function to power off device
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
__STATIC void
hdmiOpPowerOff(
	void
)
{
	dispParaSet(DISP_PARA_RB_SWITCH_SET, 0, 0, 0, 0, 0); /* disable rb switch */
	dispParaSet(DISP_PARA_EDGE_GAIN_SET, 0x08, 0, 0, 0, 0); //enable edge enhancement, 0x08-> gain: 1.0
	dispParaSet(DISP_PARA_REQ_LENTH_SET, DISP_IMG_REQ_LENTH_16 | DISP_OSD_REQ_LENTH_16, 0, 0, 0, 0);
	dispParaSet(
		DISP_PARA_CLK_SIGNAL_POLAR_CFG, \
		DISP_SIGNAL_CLK_NORMAL | DISP_SIGNAL_VSYNC_ACT_LOW | DISP_SIGNAL_HSYNC_ACT_LOW, \
		0, 0, 0, 0
	);
	dispParaSet(DISP_PARA_UPS051_RGB888_ENABLE, 0, 0, 0, 0, 0); /* Disable RGB888 single clock rate mode */
	sp1kHalAudI2SSet(0);
	#if HDMI_12BIT_MODE
	XBYTE[0x201a] |= 0x60;//gpio3,4 as UART rx and tx
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0);
	#endif
}

/**
 * @fn        void EP932_Open(void)
 * @brief     EP932_Open
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-8-26
 * @todo      N/A
 * @bug       N/A
*/

void EP952_Open(void)
{
#define HDMI_DEV_TIME_OUT   10000 //10s
	UINT8 c;
	UINT8 ret;
	UINT32 t0, t1;

	EP_HDMI_Init();

	EP_HDMI_Set_Video_Timing( DEV_FMT );	/*need change parameter*/

	//EP_HDMI_Set_Video_Output(0);
	EP_HDMI_Set_Audio_Fmt(0, 2/*AUD_SF_48000Hz*/); /* need change Audio sample rate */

	sp1kHalReadGTimer(&t0);
	while (1) {
		/* check HDMI plug in*/
		if(!sp1kHDMI_PlugGet()){
			ret = FAIL;
			break;
		}
		EP952Controller_Task();
		EP952Controller_Timer();

		/* check if the HDMI connected successfully */
		if (!is_AMute && !is_VMute && TX_State == TXS_Stream) {
			ret = SUCCESS;
			break;
		}

		/* check time out */
		sp1kHalReadGTimer(&t1);
		t1 -= t0;
		if (t1 > HDMI_DEV_TIME_OUT) {
			ret = FAIL;
			break;
		}
		dbgWdtFeed(-1);
	}

	if (ret == SUCCESS) {
		c = 0;
		HDMI2_i2c_write(0x3e, &c, 1, 1);
	}

	EP_HDMI_DumpMessage();
}
#endif //FUNC_HOST_HDMI
