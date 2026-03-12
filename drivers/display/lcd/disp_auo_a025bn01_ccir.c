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
 * @file		disp_auo_a025bn01_ccir656.c
 * @brief		display lcd auo a025bn01 driver
                with CCIR656 timing.
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

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define DISP_CCIR601_NTSC    0
#define DISP_CCIR601_PAL     0
#define DISP_CCIR656_NTSC    1
#define DISP_CCIR656_PAL     0


/* render dimension. */
#if DISP_CCIR601_NTSC
#define DEV_TYPE  DISP_DEVICE_TYPE_CCIR601_NTSC_8
#define WIDTH     (1440/2) //(1264/2)
#define HEIGHT    (480)

/* display region */
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1716/2)
#define VLINE     (252*2)
#define PNLH0     (240/2)
#define PNLV0     (10*2)
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#endif

#if DISP_CCIR601_PAL
#define DEV_TYPE  DISP_DEVICE_TYPE_CCIR601_PAL_8
#define WIDTH     (1440/2)
#define HEIGHT    (576/2)
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1716/2)
#define VLINE     252
#define PNLH0     (240/2)
#define PNLV0     10
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#endif

#if DISP_CCIR656_NTSC
#define DEV_TYPE  DISP_DEVICE_TYPE_CCIR656_NTSC
#define WIDTH     (1440/2)
#define HEIGHT    (480/2)
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1716/2)
#define VLINE     252
#define PNLH0     (240/2)
#define PNLV0     10
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#endif

#if DISP_CCIR656_PAL
#define DEV_TYPE  DISP_DEVICE_TYPE_CCIR656_PAL
#define WIDTH     (1440/2)
#define HEIGHT    (576/2)
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    (1716/2)
#define VLINE     252
#define PNLH0     (240/2)
#define PNLV0     10
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)
#endif

/* image region */
#define PVX0      0
#define PVY0      0
#define PVX1      (320 - 1)
#define PVY1      (240 - 1)
#define PVH0      PNLH0
#define PVV0      PNLV0
#define PVH1      (PVH0 + WIDTH)
#define PVV1      (PVV0 + HEIGHT)

/* osd region */
#define OSDX0     0
#define OSDY0     0
#define OSDX1     (WIDTH - 1)
#define OSDY1     (HEIGHT - 1)
#define OSDH0     PNLH0
#define OSDV0     PNLV0
#define OSDH1     (OSDH0 + OSDX1)
#define OSDV1     (OSDV0 + OSDY1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_XTAL27M
#define CLK_DIVNX      1
#define CLK_DIV1X      2
#define UPS_CLK_PHASE  8

/* 3-wire interface. */
#define SCL_PIN_BYTE  GPIO_BYTE_DISP2
#define SCL_PIN_BIT   (1 << (20 & 0x07))
#define SDA_PIN_BYTE  GPIO_BYTE_DISP2
#define SDA_PIN_BIT   (1 << (19 & 0x07))
#define CS_PIN_BYTE   GPIO_BYTE_DISP2
#define CS_PIN_BIT    (1 << (21 & 0x07))

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
#if DISP_CCIR601_NTSC
	0x1074,  /* CCIR601 NTSC 27MHZ */
#endif
#if DISP_CCIR601_PAL
    0x107C,  /* CCIR601 PAL 27MHZ */
#endif
#if DISP_CCIR656_NTSC
    0x1077,  /* CCIR656 NTSC 27MHZ */
#endif
#if DISP_CCIR656_PAL
    0x107F,  /* CCIR656 PAL 27MHZ */
#endif
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

    /* disp engine reset */
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
    //dispParaSet(DISP_PARA_DEVICE_OSD_REGION_CROP, OSDX0, OSDX1, OSDY0, OSDY1, 0);
    //dispParaSet(DISP_PARA_DEVICE_OSD_REGION_RENDER, OSDH0, OSDH1, OSDV0, OSDV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);//?

	/* set ups interface. */
	dispParaSet(DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG, DISP_CCIR_CBYCRY, 0, 0, 0, 0);
	//dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);

    XBYTE[REG_Disp_FieldEnSwap] = 0x05;  /*felden*/
    XBYTE[REG_Disp_Rsv0] |= 0x03;  /*rsv fix*/

	/* send 3-wire command. */
    dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);;
	dispSpiCmdSend(dispPwrOnCmdTbl, sizeof(dispPwrOnCmdTbl)/sizeof(UINT16));

	for ( i = 10; i > 0; i-- ) {
		dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);
	}

    /* disp enable */
    dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);
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

