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
 * @file		disp_auo_a025bn01_ups052.c
 * @brief		display lcd auo a025bn01 driver
                with ups052 (serial rgb dummy mode) timing.
 * @author		Matt Wang
 * @since		2008-06-12
 * @date		2008-11-17
 */

/*
 * History
 *
 * 1000, 20080828 Matt Wang
 *    a. Created
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "disp_api.h"
#include "gpio_api.h"
#include "solution.h"
#include "gpio.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_UPS052
#define WIDTH     320
#define HEIGHT    240
#define HSYNCW    5//1
#define VSYNCW    5//1
#define HPIXEL    411//384
#define VLINE     262//254
#define PNLH0     63
#define PNLV0     13
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)

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
#define OSDH0     PNLH0 
#define OSDV0     PNLV0 
#define OSDH1     PNLH1 
#define OSDV1     PNLV1

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      12
#define CLK_DIV1X      4
#define UPS_CLK_PHASE  8

/* 3-wire interface. */
#define SCL_PIN_BYTE  GPIO_BYTE_TG0
#define SCL_PIN_BIT   (1 << (4 & 0x07))
#define SDA_PIN_BYTE  GPIO_BYTE_TG0
#define SDA_PIN_BIT   (1 << (5 & 0x07))

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
/* function type definition. */
#define __STATIC  //static

/* operating function. */
#define dispOpDevNameGet     dispPanelOpDevNameGet
#define dispOpCapabilityGet  dispPanelOpCapabilityGet
#define dispOpPowerOn        dispPanelOpPowerOn
#define dispOpPowerOff       dispPanelOpPowerOff

/* 3-wire interface. */
#define SCL_LO  gpioByteOutSet(SCL_PIN_BYTE, SCL_PIN_BIT, 0)
#define SCL_HI  gpioByteOutSet(SCL_PIN_BYTE, SCL_PIN_BIT, SCL_PIN_BIT)
#define SDA_LO  gpioByteOutSet(SDA_PIN_BYTE, SDA_PIN_BIT, 0)
#define SDA_HI  gpioByteOutSet(SDA_PIN_BYTE, SDA_PIN_BIT, SDA_PIN_BIT)

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* operating function. */
__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);

/* serial command. */
static void dispSpiCmdSend(UINT16 *cmdTbl, UINT16 tot);
static void delay(UINT16 cnt);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - AUO A025BN01 UPS052";

/* device capability. */
static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};

/* serial command table. */
static code UINT16 dispPwrOnCmdTbl[] = {
	0x0037,  /* reset. */
	0x003F,  /* normal operation, dc-dc on. */
	0x1071,  /* ups052 320rgb 24.54mhz. */
	0x5008,  /* contrast. */
	0x6031,  /* brightness. */
	0x003F,  /* normal operation, dc-dc on. */
};

static code UINT16 dispPwrOffCmdTbl[] = {
	0x0000,
};

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispPanelOpen(
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
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8 *
dispOpDevNameGet(
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
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc
)
{
	*pcapabDesc = dispCapabDesc;
}

/**
 * @brief	operating function to power on device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOn(
	void
)
{
	UINT16 i;

	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
    dispParaSet(DISP_PARA_SW_RST, DISP_DEV_RST_ALL, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
    dispParaSet(DISP_PARA_DEVICE_IMG_REGION_CROP, PVX0, PVX1, PVY0, PVY1, 0);
    dispParaSet(DISP_PARA_DEVICE_IMG_REGION_RENDER, PVH0, PVH1, PVV0, PVV1, 0);
    dispParaSet(DISP_PARA_DEVICE_OSD_REGION_CROP, OSDX0, OSDX1, OSDY0, OSDY1, 0);
    dispParaSet(DISP_PARA_DEVICE_OSD_REGION_RENDER, OSDH0, OSDH1, OSDV0, OSDV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0);

	/* set ups interface. */
	dispParaSet(DISP_PARA_UPS_COLOR_FILTER_CFG, DISP_UPS052_EVEN_RGB, DISP_UPS052_ODD_RGB, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_DELAY_CFG, DISP_UPS052_HSD_DELAY_3T, 0, 0, 0, 0);

    dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);
    
	/* send 3-wire command. */
    //dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);
	dispSpiCmdSend(dispPwrOnCmdTbl, sizeof(dispPwrOnCmdTbl)/sizeof(UINT16));

	for ( i = 10; i > 0; i-- ) {
		dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);
	}
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOff(
	void
)
{
	UINT16 i;


	/* send 3-wire command. */
	dispSpiCmdSend(dispPwrOffCmdTbl, sizeof(dispPwrOffCmdTbl)/sizeof(UINT16));

	for ( i = 10; i > 0; i-- ) {
		dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);
	}
}

/**
 * @brief	send command to 3-wire interface.
 * @param	cmdTbl = [in] command table.
 * @param	tot = [in] command total.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-12
 * @todo	N/A
 * @bug		N/A
*/
static void
dispSpiCmdSend(
	UINT16 *cmdTbl,
	UINT16 tot
)
{
	UINT16 cmd, cmdCnt, bitCnt;
	UINT8 sclF, sclD, sdaF, sdaD;
	UINT8 CsId,CsMsk,Cspo;
	
	pkgDedicateGpioGet(PKG_DEDICATE_PIN_LCD_SEN, &CsId, &CsMsk, &Cspo);
#if (_HW_SOLUTION_ == _HW_EVB_)
	if((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))
	{
		gpioByteDirSet(CsId, CsMsk, 0xff);
		gpioByteFuncSet(CsId, CsMsk, 0xff); 
	}
#endif

	/* save original used pins' status. */
	sdaD = gpioByteDirGet(SDA_PIN_BYTE, SDA_PIN_BIT);
	sdaF = gpioByteFuncGet(SDA_PIN_BYTE, SDA_PIN_BIT);
	sclD = gpioByteDirGet(SCL_PIN_BYTE, SCL_PIN_BIT);
	sclF = gpioByteFuncGet(SCL_PIN_BYTE, SCL_PIN_BIT);
	

	/* config 3-wire pin. simulate an I2C SOF timing */
	gpioByteOutSet(CsId, CsMsk, 0xff);
  	gpioByteDirSet(CsId, CsMsk, 0xff);
	gpioByteFuncSet(CsId, CsMsk, 0xff);	
	SDA_LO;
	gpioByteDirSet(SDA_PIN_BYTE, SDA_PIN_BIT, SDA_PIN_BIT);
	gpioByteFuncSet(SDA_PIN_BYTE, SDA_PIN_BIT, SDA_PIN_BIT);
	SCL_LO;
	gpioByteDirSet(SCL_PIN_BYTE, SCL_PIN_BIT, SCL_PIN_BIT);
	gpioByteFuncSet(SCL_PIN_BYTE, SCL_PIN_BIT, SCL_PIN_BIT);


	/* send commands via 3-wire. */
	for ( cmdCnt = 0; cmdCnt < tot; cmdCnt++ ) {
		cmd = cmdTbl[cmdCnt];

		gpioByteOutSet(CsId, CsMsk, 0x00);

		for ( bitCnt = 16; bitCnt > 0; bitCnt-- ) {
			SCL_LO;
			if ( cmd & 0x8000 ) {
				SDA_HI;
			}
			else {
				SDA_LO;
			}
			delay(3);
			SCL_HI;
			delay(3);

			cmd <<= 1;
		}

		SCL_LO;
		SDA_LO;
		gpioByteOutSet(CsId, CsMsk, 0xff);
		delay(3);
	}

	/* restore original used pins' status. simulate an I2C EOF timing*/
	gpioByteFuncSet(SCL_PIN_BYTE, SCL_PIN_BIT, sclF);
	gpioByteDirSet(SCL_PIN_BYTE, SCL_PIN_BIT, sclD);
	gpioByteFuncSet(SDA_PIN_BYTE, SDA_PIN_BIT, sdaF);
	gpioByteDirSet(SDA_PIN_BYTE, SDA_PIN_BIT, sdaD);
}

/*
*/
static void
delay(
	UINT16 cnt
)
{
	UINT16 i;


	for ( i = cnt; i > 0; i-- ) {
	}
}

