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
 * @file		disp_tp_td020theg1_ups052.c
 * @brief		display lcd toppoly td020theg1 driver
                with ups052 (serial rgb dummy mode) timing.
 * @author		Matt Wang
 * @since		2008-08-11
 * @date		2008-08-19
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

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_UPS052
#define WIDTH     320
#define HEIGHT    240
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    390
#define VLINE     264
#define PNLH0     56
#define PNLV0     19
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      4
#define CLK_DIV1X      4
#define UPS_CLK_PHASE  8

/* 3-wire interface. */
#define SCL_PIN_BYTE  GPIO_BYTE_TG0
#define SCL_PIN_BIT   (1 << (4 & 0x07))
#define SDA_PIN_BYTE  GPIO_BYTE_TG0
#define SDA_PIN_BIT   (1 << (5 & 0x07))
#define CS_PIN_BYTE   GPIO_BYTE_GEN1
#define CS_PIN_BIT    (1 << (9 & 0x07))

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
#define CS_LO   gpioByteOutSet(CS_PIN_BYTE, CS_PIN_BIT, 0)
#define CS_HI   gpioByteOutSet(CS_PIN_BYTE, CS_PIN_BIT, CS_PIN_BIT)

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
static code UINT8 dispDevName[] = "LCD - TP TD020THEG1 UPS052";

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
	(0x02<<10)+0x00,
	(0x03<<10)+0x01,
	(0x04<<10)+0x0F,
	(0x05<<10)+0x2B,
	(0x07<<10)+0x08,
	(0x09<<10)+0x20+0x01,  /* gain of contrast. */
	(0x0A<<10)+0x20+0x01,  /* r gain of sub-contrast. */
	(0x0B<<10)+0x20+0x01,  /* b gain of sub-contrast. */
	(0x0C<<10)+0x10-0x06,  /* offset of brightness. */
	(0x10<<10)+0x35-0x01,  /* vcom high level. */
	(0x11<<10)+0x38+0x01,  /* vcom low level. */
	(0x14<<10)+0x98-0x45,  /* gama28, gama0. */
	(0x15<<10)+0x9A-0x22,  /* gama93, gama60. */
	(0x16<<10)+0xA9-0x63,  /* gama157, gama125. */
	(0x17<<10)+0x99-0x86,  /* gama222, gama190. */
	(0x18<<10)+0x08-0x08,  /* gama255. */
};

static code UINT16 dispPwrOffCmdTbl[] = {
	(0x04<<10)+0x0A,
};

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-08-11
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
 * @since	2008-08-11
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
 * @since	2008-08-11
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
 * @since	2008-08-11
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
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

	/* set ups interface. */
	dispParaSet(DISP_PARA_UPS_COLOR_FILTER_CFG, DISP_UPS052_EVEN_RGB, DISP_UPS052_ODD_RGB, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_DELAY_CFG, 1, 0, 0, 0, 0);

	/* send 3-wire command. */
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
 * @since	2008-08-11
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
 * @since	2008-08-11
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


	/* save original used pins' status. */
	sclF = gpioByteFuncGet(SCL_PIN_BYTE, SCL_PIN_BIT);
	sclD = gpioByteDirGet(SCL_PIN_BYTE, SCL_PIN_BIT);
	sdaF = gpioByteFuncGet(SDA_PIN_BYTE, SDA_PIN_BIT);
	sdaD = gpioByteDirGet(SDA_PIN_BYTE, SDA_PIN_BIT);

	/* config 3-wire pin. */
	gpioByteFuncSet(SCL_PIN_BYTE, SCL_PIN_BIT, SCL_PIN_BIT);
	gpioByteDirSet(SCL_PIN_BYTE, SCL_PIN_BIT, SCL_PIN_BIT);
	gpioByteFuncSet(SDA_PIN_BYTE, SDA_PIN_BIT, SDA_PIN_BIT);
	gpioByteDirSet(SDA_PIN_BYTE, SDA_PIN_BIT, SDA_PIN_BIT);
	gpioByteFuncSet(CS_PIN_BYTE, CS_PIN_BIT, CS_PIN_BIT);
    gpioByteDirSet(CS_PIN_BYTE, CS_PIN_BIT, CS_PIN_BIT);

	/* send commands via 3-wire. */
	for ( cmdCnt = 0; cmdCnt < tot; cmdCnt++ ) {
		cmd = cmdTbl[cmdCnt];

		CS_LO;

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
		CS_HI;
		delay(3);
	}

	/* restore original used pins' status. */
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

