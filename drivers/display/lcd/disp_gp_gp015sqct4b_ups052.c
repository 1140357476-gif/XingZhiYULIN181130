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
 * @file		disp_gp_gp015sqct4b_ups051.c
 * @brief		display lcm giant plus gp015sqct4b driver
                with ups052 (serial rgb dummy mode) timing.
 * @author		Matt Wang
 * @since		2008-07-31
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
#define DEV_TYPE  DISP_DEVICE_TYPE_UPS052
#define WIDTH     128
#define HEIGHT    128
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    141
#define VLINE     132
#define PNLH0     10
#define PNLV0     3
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1)

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      14
#define CLK_DIV1X      3
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
static void dispSpiCmdSend(UINT32 *cmdTbl);
static void delay(UINT16 cnt);

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - GP GP015SQCT4B UPS052";

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
	DISP_LCM_CMD_INST | 0xF0,  /* undocument? */
	DISP_LCM_CMD_PARA | 0x5A,
	DISP_LCM_CMD_INST | 0xF2,  /* undocument? */
	DISP_LCM_CMD_PARA | 0x52,
	DISP_LCM_CMD_INST | 0x11,  /* slpout. */
	DISP_LCM_CMD_WAIT | 100,
	DISP_LCM_CMD_INST | 0xC5,  /* vmctr1. */
	DISP_LCM_CMD_PARA | 0xAA,
	DISP_LCM_CMD_INST | 0xC6,  /* vmctr2. */
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_INST | 0xC0,  /* pwctr1. */
	DISP_LCM_CMD_PARA | 0x08,
	DISP_LCM_CMD_PARA | 0x01,
	DISP_LCM_CMD_INST | 0xC2,  /* pwctr3. */
	DISP_LCM_CMD_PARA | 0x54,
	DISP_LCM_CMD_PARA | 0x05,
	DISP_LCM_CMD_WAIT | 100,
	DISP_LCM_CMD_INST | 0x21,  /* invon. */
	DISP_LCM_CMD_INST | 0x3A,  /* colmod. */
	DISP_LCM_CMD_PARA | 0xE0,
	DISP_LCM_CMD_INST | 0xE0,  /* gamctrp1. */
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x03,
	DISP_LCM_CMD_PARA | 0x04,
	DISP_LCM_CMD_PARA | 0x34,
	DISP_LCM_CMD_PARA | 0x30,
	DISP_LCM_CMD_PARA | 0x0B,
	DISP_LCM_CMD_PARA | 0x1A,
	DISP_LCM_CMD_PARA | 0x04,
	DISP_LCM_CMD_PARA | 0x01,
	DISP_LCM_CMD_PARA | 0x02,
	DISP_LCM_CMD_PARA | 0x0F,
	DISP_LCM_CMD_PARA | 0x0F,
	DISP_LCM_CMD_PARA | 0x02,
	DISP_LCM_CMD_PARA | 0x05,
	DISP_LCM_CMD_PARA | 0x05,
	DISP_LCM_CMD_PARA | 0x08,
	DISP_LCM_CMD_INST | 0xE1,  /* gamctrn1. */
	DISP_LCM_CMD_PARA | 0x06,
	DISP_LCM_CMD_PARA | 0x26,
	DISP_LCM_CMD_PARA | 0x28,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x03,
	DISP_LCM_CMD_PARA | 0x1E,
	DISP_LCM_CMD_PARA | 0x02,
	DISP_LCM_CMD_PARA | 0x01,
	DISP_LCM_CMD_PARA | 0x03,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x0C,
	DISP_LCM_CMD_PARA | 0x0C,
	DISP_LCM_CMD_PARA | 0x03,
	DISP_LCM_CMD_PARA | 0x03,
	DISP_LCM_CMD_INST | 0x2A,  /* caset. */
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | ((WIDTH - 1) >> 8),
	DISP_LCM_CMD_PARA | ((WIDTH - 1) & 0xFF),
	DISP_LCM_CMD_INST | 0x2B,  /* raset. */
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | ((HEIGHT - 1) >> 8),
	DISP_LCM_CMD_PARA | ((HEIGHT - 1) & 0xFF),
	DISP_LCM_CMD_WAIT | 100,
	DISP_LCM_CMD_INST | 0x29,  /* disp on. */
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
 * @since	2008-07-31
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
 * @since	2008-07-31
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
 * @since	2008-07-31
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
 * @since	2008-07-31
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOn(
	void
)
{
	UINT8 i;


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
	dispParaSet(DISP_PARA_UPS_COLOR_FILTER_CFG, DISP_UPS052_EVEN_BGR, DISP_UPS052_ODD_BGR, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 1, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_DELAY_CFG, DISP_UPS052_HSD_DELAY_3T, 0, 0, 0, 0);

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
 * @since	2008-07-31
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

	/* send lcm command. */
	dispSpiCmdSend(dispPwrOffCmdTbl);
}

/**
 * @brief	send command to 3-wire interface.
 * @param	cmdTbl = [in] command table.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-31
 * @todo	N/A
 * @bug		N/A
*/
static void
dispSpiCmdSend(
	UINT32 *cmdTbl
)
{
	UINT8 sclF, sclD, sdaF, sdaD;
	UINT8 cmd8, run;
	UINT32 cmdHdr;
	UINT16 cmd16, bitCnt;


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
	//delay(3);

	run = 1;
	while ( run ) {
		cmdHdr = *cmdTbl & 0xFF000000L;
		cmd16 = *cmdTbl;

		if ( cmdHdr <= DISP_LCM_CMD_PARA ) {
			CS_LO;
			//delay(3);

			SCL_LO;
			if ( cmdHdr == DISP_LCM_CMD_INST ) {
				printf("cmd");
				SDA_LO;
			}
			else {  /* DISP_LCM_CMD_PARA. */
				printf("dat");
				SDA_HI;
			}
			//delay(3);
			SCL_HI;
			//delay(3);

			cmd8 = cmd16;
			printf("=0x%02X =", (UINT16)cmd8);
			for ( bitCnt = 8; bitCnt > 0; bitCnt-- ) {
				SCL_LO;
				if ( cmd8 & 0x80 ) {
					printf("1");
					SDA_HI;
				}
				else {
					printf("0");
					SDA_LO;
				}
				//delay(3);
				SCL_HI;
				//delay(3);

				cmd8 <<= 1;
			}
			printf("b\n");

			SCL_LO;
			SDA_LO;
			CS_HI;
			//delay(10);
		}
		else if ( cmdHdr == DISP_LCM_CMD_WAIT ) {
			printf("delay %d\n", cmd16);
			delay(cmd16);
		}
		else {  /* DISP_LCM_CMD_STOP. */
			printf("stop\n");
			run = 0;
		}

		cmdTbl++;
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

