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
 * @file		disp_gp_54124a_lcm.c
 * @brief		display lcm giant plus 54124a driver.
 * @author		Matt Wang
 * @since		2008-06-16
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

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_LCM
#define WIDTH     160
#define HEIGHT    128
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    171
#define VLINE     1023
#define PNLH0     12
#define PNLV0     3
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      3
#define CLK_DIV1X      2
#define UPS_CLK_PHASE  8

/* lcm interface. */
#define BUS_TYPE        DISP_LCM_CMD_BUS_I80
#define BIT_WIDTH       DISP_LCM_CMD_8_9_BIT
#define PULSE_ACT       4
#define PULSE_RCV       5
#define CS0_ACT         0
#define A0_CMD          0
#define DATA_TYPE       DISP_LCM_DATA_TYPE_RGB565
#define DATA_FMT        DISP_LCM_DATA_TYPE_RGB565
#define RGB666_PACK     0
#define RGB666_FMT_10   0
#define PIXEL_ALIGN     DISP_LCM_PIXEL_ALIGN_NONE
#define COLOR_PACKAGE   DISP_LCM_COLOR_PACKAGE_RGB
#define SWITCH_G        1
#define CHANGE_RB       0
#define ONE_HALF_PIXEL  0
#define DITHER          1

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

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* operating function. */
__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - GP 54124A LCM";

/* device capability. */
static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};

/* lcm command table. */
static code UINT32 dispPwrOnCmdTbl[] = {
	DISP_LCM_CMD_INST | 0x01,  /* swreset. */
	DISP_LCM_CMD_WAIT | 30,
	DISP_LCM_CMD_INST | 0x11,  /* slpout. */
	DISP_LCM_CMD_INST | 0x38,  /* idmoff. */
	DISP_LCM_CMD_INST | 0x13,  /* noron. */
	DISP_LCM_CMD_INST | 0x3A,  /* colmod. */
	DISP_LCM_CMD_PARA | 0x05,
	DISP_LCM_CMD_INST | 0x2A,  /* caset. */
	DISP_LCM_CMD_PARA | 0,
	DISP_LCM_CMD_PARA | 0,
	DISP_LCM_CMD_PARA | ((WIDTH - 1) >> 8),
	DISP_LCM_CMD_PARA | ((WIDTH - 1) & 0xFF),
	DISP_LCM_CMD_INST | 0x2B,  /* raset. */
	DISP_LCM_CMD_PARA | 0,
	DISP_LCM_CMD_PARA | 0,
	DISP_LCM_CMD_PARA | ((HEIGHT - 1) >> 8),
	DISP_LCM_CMD_PARA | ((HEIGHT - 1) & 0xFF),
	DISP_LCM_CMD_INST | 0x36,  /* madctr. */
	DISP_LCM_CMD_PARA | 0x60,
	DISP_LCM_CMD_WAIT | 500,
	DISP_LCM_CMD_INST | 0xC0,  /* pwctr1. */
	DISP_LCM_CMD_PARA | 0x08,
	DISP_LCM_CMD_PARA | 0x05,
	DISP_LCM_CMD_INST | 0xC5,  /* vmctr1. */
	DISP_LCM_CMD_PARA | 0xA9,
	DISP_LCM_CMD_INST | 0xC6,  /* vmctr2. */
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_WAIT | 500,
	DISP_LCM_CMD_INST | 0x29,  /* dispon. */
	DISP_LCM_CMD_INST | 0x2C,  /* ramwr. */
	DISP_LCM_CMD_STOP,
};

static code UINT32 dispPwrOffCmdTbl[] = {
	DISP_LCM_CMD_INST | 0x28,  /* dispoff. */
	DISP_LCM_CMD_INST | 0x10,  /* slpin. */
	DISP_LCM_CMD_INST | 0x39,  /* idmon. */
	DISP_LCM_CMD_INST | 0x12,  /* ptlon. */
	DISP_LCM_CMD_STOP,
};

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-06-16
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
 * @since	2008-06-16
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
 * @since	2008-06-16
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
 * @since	2008-06-16
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOn(
	void
)
{
	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

	/* set lcm interface. */
	dispParaSet(DISP_PARA_LCM_CMD_BUS_TYPE, BUS_TYPE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CMD_BIT_WIDTH, BIT_WIDTH, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CMD_PULSE_WIDTH, PULSE_ACT, PULSE_RCV, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CMD_ACTIVE_LEVEL, CS0_ACT, A0_CMD, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_RGB666_PACK_ENABLE, RGB666_PACK, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_RGB666_FORMAT_10_ENABLE, RGB666_FMT_10, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_PIXEL_ALIGN, PIXEL_ALIGN, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_COLOR_PACKAGE, COLOR_PACKAGE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_SWITCH_G_ENABLE, SWITCH_G, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CHANGE_RB_ENABLE, CHANGE_RB, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_ONE_HALF_PIXEL_ENABLE, ONE_HALF_PIXEL, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_DITHER_ENABLE, DITHER, 0, 0, 0, 0);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-16
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOff(
	void
)
{
	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOffCmdTbl);
}

