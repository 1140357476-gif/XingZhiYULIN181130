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
 * @file		disp_nt_nt35702_lcm.c
 * @brief		display lcm NovaTek35702 driver.
 * @author		Phil Lin
 * @since		2011-05-17
 * @date		2011-05-17
 */

/*
 * History
 *
 * 1000, 20110517 Phil Lin
 *    a. Created
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/


#include "general.h"
#include "reg_def.h"
#include "disp_api.h"

#include "gpio_api.h"
#include "timer.h"
#include "sp1k_util_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_hal_api.h"
#include "gpio.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
 
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_LCM
#define WIDTH     320
#define HEIGHT    240 
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    327
#define VLINE     0x7ff
#define PNLH0     3 
#define PNLV0     2
#define PNLH1     (PNLH0 + WIDTH - 1 )
#define PNLV1     (PNLV0 + HEIGHT - 1 ) 

/* image region */
#define PVX0      0
#define PVY0      0
#define PVX1      (WIDTH - 1)
#define PVY1      (HEIGHT - 1)
#define PVH0      PNLH0
#define PVV0      PNLV0
#define PVH1      (PVH0 + PVX1)
#define PVV1      (PVV0 + PVY1)

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
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      22// 24
#define CLK_DIV1X      3 //3//2
#define UPS_CLK_PHASE  8
#define CS0_ACT        0
#define A0_CMD         0
 
/* bus type */
#define BUS_TYPE        DISP_LCM_CMD_BUS_I80

/* data format */
//#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T444_1
//#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T565_1
#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T666_1

/* Dither configuration */
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
static code UINT8 dispDevName[] = "LCD - TXD1.77 LCM";
void dispSyncInterEnable(void) ;

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

	DISP_LCM_CMD_INST | (0x01),//software reset 
	DISP_LCM_CMD_WAIT | (20),  //ms

	DISP_LCM_CMD_INST | (0x11),//sleep out
	DISP_LCM_CMD_WAIT | (120), //ms

    DISP_LCM_CMD_INST | (0xc2),//Power Control 3
    DISP_LCM_CMD_PARA | (0x05),//APA2 APA1 APA0   Large
    DISP_LCM_CMD_PARA | (0x00),//Step-up cycle in Booster circuit 1
                               //Step-up cycle in Booster circuit 2,3
    DISP_LCM_CMD_INST | (0xc3),//Power Control 4
    DISP_LCM_CMD_PARA | (0x05),//APA2 APA1 APA0   Large
    DISP_LCM_CMD_PARA | (0x00),//Step-up cycle in Booster circuit 1
                               //Step-up cycle in Booster circuit 2,3
    DISP_LCM_CMD_INST | (0xc4),//Power Control 5
    DISP_LCM_CMD_PARA | (0x05),//APA2 APA1 APA0   Large
    DISP_LCM_CMD_PARA | (0x00),//Step-up cycle in Booster circuit 1
                               //Step-up cycle in Booster circuit 2,3

	DISP_LCM_CMD_INST | (0x35),
	DISP_LCM_CMD_PARA | (0x00),

	DISP_LCM_CMD_INST | (0x0e),
	DISP_LCM_CMD_PARA | (0x80),// TE on

    DISP_LCM_CMD_INST | (0x3A),//Interface Pixel Format
    DISP_LCM_CMD_PARA | (0x06),//RGB666
    //DISP_LCM_CMD_PARA | (0x05),//RGB565

    DISP_LCM_CMD_INST | (0xD7),
    DISP_LCM_CMD_PARA | (0x40),
    DISP_LCM_CMD_PARA | (0xE0),

    DISP_LCM_CMD_INST | (0xFD),
    DISP_LCM_CMD_PARA | (0x06),
    DISP_LCM_CMD_PARA | (0x11),

    DISP_LCM_CMD_INST | (0xFA),
    DISP_LCM_CMD_PARA | (0x38),
    DISP_LCM_CMD_PARA | (0x20),
    DISP_LCM_CMD_PARA | (0x27),//(0x27),//(0x20),//(0x1f),
    DISP_LCM_CMD_PARA | (0x3f),//(0x37),//(0x36),//(0x08),
    DISP_LCM_CMD_PARA | (0x37),
    DISP_LCM_CMD_PARA | (0x12),
    DISP_LCM_CMD_PARA | (0x22),
    DISP_LCM_CMD_PARA | (0x1E),

    DISP_LCM_CMD_INST | (0xC0),//Set GVDD
    DISP_LCM_CMD_PARA | (0x05),

    DISP_LCM_CMD_INST | (0xC5),//Set Vcom
    DISP_LCM_CMD_PARA | (0x16),
    DISP_LCM_CMD_PARA | (0x3D),

    DISP_LCM_CMD_INST | (0xC7),//Set VCOM-OFFSET
    DISP_LCM_CMD_PARA | (0xB5), //B2

    DISP_LCM_CMD_INST | (0x36),//Memory data  access control
    DISP_LCM_CMD_PARA | (0x68),//MY MX MV ML RGB MH 0 0   DC

    DISP_LCM_CMD_INST | (0xE0),//E0H Set
    DISP_LCM_CMD_PARA | (0x04),
    DISP_LCM_CMD_PARA | (0x04),
    DISP_LCM_CMD_PARA | (0x04),
    DISP_LCM_CMD_PARA | (0x02),
    DISP_LCM_CMD_PARA | (0x09),
    DISP_LCM_CMD_PARA | (0x13),
    DISP_LCM_CMD_PARA | (0x29),
    DISP_LCM_CMD_PARA | (0x8B),
    DISP_LCM_CMD_PARA | (0x3E),
    DISP_LCM_CMD_PARA | (0x0B),
    DISP_LCM_CMD_PARA | (0x1F),
    DISP_LCM_CMD_PARA | (0x25),
    DISP_LCM_CMD_PARA | (0x09),
    DISP_LCM_CMD_PARA | (0x0A),
    DISP_LCM_CMD_PARA | (0x0A),

    DISP_LCM_CMD_INST | (0xE1),//E1H Set
    DISP_LCM_CMD_PARA | (0x33),
    DISP_LCM_CMD_PARA | (0x33),
    DISP_LCM_CMD_PARA | (0x33),
    DISP_LCM_CMD_PARA | (0x08),
    DISP_LCM_CMD_PARA | (0x0F),
    DISP_LCM_CMD_PARA | (0x13),
    DISP_LCM_CMD_PARA | (0x44),
    DISP_LCM_CMD_PARA | (0x47),
    DISP_LCM_CMD_PARA | (0x58),
    DISP_LCM_CMD_PARA | (0x0C),
    DISP_LCM_CMD_PARA | (0x26),
    DISP_LCM_CMD_PARA | (0x2D),
    DISP_LCM_CMD_PARA | (0x3B),
    DISP_LCM_CMD_PARA | (0x3B),
    DISP_LCM_CMD_PARA | (0x3B),


    DISP_LCM_CMD_INST | (0x2A),
    DISP_LCM_CMD_PARA | (0x00),
    DISP_LCM_CMD_PARA | (0x00),
    DISP_LCM_CMD_PARA | (0x01),
    DISP_LCM_CMD_PARA | (0x3F),

    DISP_LCM_CMD_INST | (0x2B),
    DISP_LCM_CMD_PARA | (0x00),
    DISP_LCM_CMD_PARA | (0x00),
    DISP_LCM_CMD_PARA | (0x00),
    DISP_LCM_CMD_PARA | (0xEF),

    DISP_LCM_CMD_INST | (0x29),//display on
    
    DISP_LCM_CMD_INST | (0x2c),//Memory Write

	DISP_LCM_CMD_STOP,      
};

static code UINT32 dispPwrOffCmdTbl[] = {
	DISP_LCM_CMD_INST | 0x28,  /* dispoff. */
	DISP_LCM_CMD_STOP, 
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
	/* RD(DGPIO8) Must be pull high, because we have not cmd read function for LCM */
#if 0	
	gpioByteFuncSet(GPIO_BYTE_DISP1, 0x01 << (8 & 0x07), 0x01 << (8 & 0x07));
	gpioByteOutSet(GPIO_BYTE_DISP1, 0x01 << (8 & 0x07), 0x01 << (8 & 0x07));
	gpioByteDirSet(GPIO_BYTE_DISP1, 0x01 << (8 & 0x07), 0x01 << (8 & 0x07));
#endif
	/* RES(DGPIO13) LCM reset */
	gpioByteFuncSet(GPIO_BYTE_DISP1, 0x01 << (13 & 0x07), 0xff);
	gpioByteOutSet (GPIO_BYTE_DISP1, 0x01 << (13 & 0x07), 0xff);
	gpioByteDirSet (GPIO_BYTE_DISP1, 0x01 << (13 & 0x07), 0xff);
	sp1kHalWait(1);
	gpioByteOutSet (GPIO_BYTE_DISP1, 0x01 << (13 & 0x07), 0x00);
	sp1kHalWait(20);
	gpioByteOutSet (GPIO_BYTE_DISP1, 0x01 << (13 & 0x07), 0xff);
	sp1kHalWait(100);

	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	//dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
	dispParaSet(DISP_PARA_DEVICE_IMG_REGION_CROP, PVX0, PVX1, PVY0, PVY1, 0);
	dispParaSet(DISP_PARA_DEVICE_IMG_REGION_RENDER, PVH0, PVH1, PVV0, PVV1, 0);
	dispParaSet(DISP_PARA_DEVICE_OSD_REGION_CROP, OSDX0, OSDX1, OSDY0, OSDY1, 0);
	dispParaSet(DISP_PARA_DEVICE_OSD_REGION_RENDER, OSDH0, OSDH1, OSDV0, OSDV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

	/* set lcm interface. */
	dispParaSet(DISP_PARA_LCM_CMD_BUS_TYPE, BUS_TYPE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CMD_ACTIVE_LEVEL, CS0_ACT, A0_CMD, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, 0, 0, 0, 0);
	//dispParaSet(DISP_PARA_LCM_DITHER_ENABLE, DITHER, 0, 0, 0, 0);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);

	XBYTE[REG_Disp_GpioSel1] |= 0x01 << (12 & 0x07); //set DGPIO12 as TE pin in
	XBYTE[REG_Disp_GpioOutEn1] &= ~(0x01 << (12 & 0x07));
	XBYTE[REG_Disp_PhsSel_Polar] |= 0x10;
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] |= 0x01;

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
__STATIC void dispOpPowerOff(void)
{
	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOffCmdTbl);
}
 
