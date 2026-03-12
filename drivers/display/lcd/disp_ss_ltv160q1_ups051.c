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
 * @file		disp_ss_ltv160q1_ups051.c
 * @brief		display lcd samsung ltv160q1 driver with ups051 timing.
 * @author		Matt Wang
 * @since		2008-08-20
 * @date		2008-08-29
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
#include "timer.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_UPS051
#define WIDTH     492
#define HEIGHT    240
#define HBP       11
#define HFP       24
#define VBP       20
#define VFP       6
#define HSYNCW    15
#define VSYNCW    3
#define HPIXEL    (HBP + WIDTH + HFP)
#define VLINE     (VBP + HEIGHT + VFP)
#define PNLH0     34
#define PNLV0     17
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      9
#define CLK_DIV1X      1
#define UPS_CLK_PHASE  8

/* 3-wire interface. */
#define SCL_PIN_BYTE  GPIO_BYTE_TG0
#define SCL_PIN_BIT   (1 << (4 & 0x07))
#define SDA_PIN_BYTE  GPIO_BYTE_TG0
#define SDA_PIN_BIT   (1 << (5 & 0x07))
#define CS_PIN_BYTE   GPIO_BYTE_GEN1
#define CS_PIN_BIT    (1 << (9 & 0x07))

/* panel control pin. */
#define RST_PIN_USE   1  /* 0: not use, 1: use reset pin. */
#define RST_PIN_BYTE  GPIO_BYTE_GEN1
#define RST_PIN_BIT   (1 << (15 & 0x07))
#define BL_PIN_USE    1  /* 0: not use, 1: use backlight pin. */
#define BL_PIN_BYTE   GPIO_BYTE_GEN1
#define BL_PIN_BIT    (1 << (14 & 0x07))

/* 3-wire command. */
#define SPI_CMD_INST  0x01000000L
#define SPI_CMD_WAIT  0x02000000L
#define SPI_CMD_STOP  0xFF000000L

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

/* debug. */
#if 0
#define DBG_PRINT  printf
#else
#define DBG_PRINT
#endif

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* operating function. */
__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);

/* serial command. */
static void dispSpiCmdSend(UINT32 *cmdTbl);
static void delay(UINT16 cnt);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - SS LTV160Q1 UPS051";

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
static code UINT32 dispPwrOnCmdTbl[] = {
	/* initializing function. */
	SPI_CMD_INST | 0x000000,
	SPI_CMD_INST | 0x010000,
	SPI_CMD_INST | 0x020000,
	SPI_CMD_INST | 0x0600A3,
	SPI_CMD_INST | 0x050000,
	SPI_CMD_WAIT | 1,
	/* power setting function 1. */
	SPI_CMD_INST | 0x020910,
	SPI_CMD_INST | 0x000006,
	SPI_CMD_WAIT | 3,
	/* power setting function 2. */
	SPI_CMD_INST | 0x022910,
	/* display setting function. */
	SPI_CMD_INST | 0x000016,
	SPI_CMD_INST | 0x031900,
	SPI_CMD_INST | 0x040100 | (HBP<<10) | VBP,
	SPI_CMD_INST | 0x053000,
	SPI_CMD_INST | 0x060083,
	SPI_CMD_INST | 0x070300,
	SPI_CMD_INST | 0x080003,
	SPI_CMD_INST | 0x090908,
	SPI_CMD_INST | 0x0A0809,
	SPI_CMD_INST | 0x0B0303,
	SPI_CMD_INST | 0x0C0303,
	SPI_CMD_INST | 0x0D0303,
	SPI_CMD_INST | 0x0E0303,
	SPI_CMD_INST | 0x0F0303,
	SPI_CMD_INST | 0x100303,
	SPI_CMD_WAIT | 4,
	/* display function. */
	SPI_CMD_INST | 0x000816,
	SPI_CMD_INST | 0x053010,
	SPI_CMD_INST | 0x061083,
	SPI_CMD_INST | 0x010200,
	SPI_CMD_STOP,
};

static code UINT32 dispPwrOffCmdTbl[] = {
	SPI_CMD_INST | 0x010000,
	SPI_CMD_WAIT | 0x000001,
	SPI_CMD_INST | 0x0602A3,
	SPI_CMD_INST | 0x050000,
	SPI_CMD_INST | 0x020910,
	SPI_CMD_WAIT | 0x000001,
	SPI_CMD_INST | 0x000000,
	SPI_CMD_STOP,
};

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-08-20
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
 * @since	2008-08-20
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
 * @since	2008-08-20
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
 * @since	2008-08-20
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOn(
	void
)
{
	UINT16 i;


	/* hold reset pin. */
	#if RST_PIN_USE
	gpioByteFuncSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
	gpioByteDirSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
	gpioByteOutSet(RST_PIN_BYTE, RST_PIN_BIT, 0x00);
	#endif

	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

	/* set ups interface. */
	dispParaSet(DISP_PARA_UPS_COLOR_FILTER_CFG, DISP_UPS051_EVEN_GBR, DISP_UPS051_ODD_BRG, 0, 0, 0);

	/* release reset pin. */
	#if RST_PIN_USE
	for ( i = 20; i > 0; i-- ) {
		DELAY_1ms();
	}
	gpioByteOutSet(RST_PIN_BYTE, RST_PIN_BIT, RST_PIN_BIT);
	for ( i = 200; i > 0; i-- ) {
		DELAY_1ms();
	}
	#endif

	/* send 3-wire command. */
	dispSpiCmdSend(dispPwrOnCmdTbl);

	/* set backlight on. */
	#if BL_PIN_USE
	gpioByteFuncSet(BL_PIN_BYTE, BL_PIN_BIT, BL_PIN_BIT);
	gpioByteDirSet(BL_PIN_BYTE, BL_PIN_BIT, BL_PIN_BIT);
	gpioByteOutSet(BL_PIN_BYTE, BL_PIN_BIT, BL_PIN_BIT);
	#endif
}

/**
 * @brief	operating function to power off device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-08-20
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOff(
	void
)
{
	/* set backlight off. */
	#if BL_PIN_USE
	gpioByteFuncSet(BL_PIN_BYTE, BL_PIN_BIT, BL_PIN_BIT);
	gpioByteDirSet(BL_PIN_BYTE, BL_PIN_BIT, BL_PIN_BIT);
	gpioByteOutSet(BL_PIN_BYTE, BL_PIN_BIT, 0x00);
	#endif

	/* send 3-wire command. */
	dispSpiCmdSend(dispPwrOffCmdTbl);
}

/**
 * @brief	send command to 3-wire interface.
 * @param	cmdTbl = [in] command table.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-08-20
 * @todo	N/A
 * @bug		N/A
*/
static void
dispSpiCmdSend(
	UINT32 *cmdTbl
)
{
	UINT8 sclF, sclD, sdaF, sdaD;
	UINT8 run, cmdHdr, bitCnt;
	UINT16 delayCnt;
	UINT32 cmd;


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
	CS_HI;
	SCL_HI;
	SDA_HI;

	run = 1;
	while ( run ) {
		cmdHdr = READ8(*cmdTbl, 0);
		switch ( cmdHdr ) {
		case (UINT8)(SPI_CMD_INST >> 24):
			for ( cmdHdr = 2; cmdHdr > 0; cmdHdr-- ) {
				if ( cmdHdr == 2 ) {
					cmd = 0x740000 | READ8(*cmdTbl, 1);
					DBG_PRINT("idx=0x%06lX ", cmd);
				}
				else {
					cmd = 0x760000 | (UINT16)(*cmdTbl);
					DBG_PRINT("val=0x%06lX ", cmd);
				}

				CS_LO;

				for ( bitCnt = 24; bitCnt > 0; bitCnt-- ) {
					SCL_LO;
					if ( READ8(cmd, 1) & 0x80 ) {
						DBG_PRINT("1");
						SDA_HI;
					}
					else {
						DBG_PRINT("0");
						SDA_LO;
					}
					delay(3);
					SCL_HI;

					cmd <<= 1;
				}
				SDA_HI;
				CS_HI;

				DBG_PRINT("b\n");
			}

			break;

		case (UINT8)(SPI_CMD_WAIT >> 24):
			delayCnt = *cmdTbl;
			DBG_PRINT("delay %d\n",  delayCnt);

			for ( ; delayCnt > 0; delayCnt-- ) {
				dispSignalWait(DISP_WAIT_VSYNC, DISP_WAIT_RISING);
			}

			break;

		case (UINT8)(SPI_CMD_STOP >> 24):
			DBG_PRINT("stop\n");
			run = 0;

			break;
		}

		cmdTbl++;
	}
	CS_HI;

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

