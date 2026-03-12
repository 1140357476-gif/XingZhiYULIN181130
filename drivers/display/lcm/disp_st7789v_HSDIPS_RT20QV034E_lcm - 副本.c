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
 * @file		disp_gp_gpm815a0_lcm.c
 * @brief		display lcm giant plus 54124a driver.
 * @author		Matt Wang
 * @since		2008-07-25
 * @date		2008-09-25
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
#include "sp1k_util_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_hal_api.h"
#include "gpio.h"
#include "app_ui_para.h"
#include "app_dev_disp.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
 
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_LCM
#define WIDTH    320//240//320 
#define HEIGHT   240//320//240
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    352//251//334 //182//214//182
#define VLINE     0x806//301 //260 // 0x806 // 260 
#define PNLH0     5
#define PNLV0     10
#define PNLH1     (PNLH0 + WIDTH-1)
#define PNLV1     (PNLV0 + HEIGHT-1 ) 

/* image region */
#define PVX0      0
#define PVY0      0
#define PVX1      (WIDTH -1)
#define PVY1      (HEIGHT - 1)
#define PVH0      PNLH0
#define PVV0      PNLV0
#define PVH1      (PVH0 + PVX1)
#define PVV1      (PVV0 + PVY1)

/* osd region */
#define OSDX0     0 
#define OSDY0     0 
#define OSDX1     (WIDTH -1) 
#define OSDY1     (HEIGHT - 1) 
#define OSDH0     PNLH0 
#define OSDV0     PNLV0 
#define OSDH1     (OSDH0 + OSDX1) 
#define OSDV1     (OSDV0 + OSDY1) 

/* clock config. */
#define CLK_SRC       	DISP_CLK_SRC_SYSPLL522M 	// DISP_CLK_SRC_XTAL27M DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX   	29 // 29 28
#define CLK_DIV1X     	2 // 2 3
#define UPS_CLK_PHASE  	8
#define CS0_ACT         0
#define A0_CMD          0
 
/* bus type */
#define BUS_TYPE        DISP_LCM_CMD_BUS_I80

/* data format */
//#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T444_1
#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T565_1
//#define DATA_FMT        	DISP_LCM_DATA_FMT_B8_T666_1

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
// extern void  dispLcmCmdTableWrite(UINT32 *cmdTbl);
extern UINT16  Read_Register (UINT32 cmd);
extern UINT8 HAL_GlobalDGPIOModeGet(void);

void dispSyncInterEnable(void) ;


//void Init_panel(void(*ptr)());
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - 2.4 inch LCM";

__STATIC  UINT16  Panel_ID;


UINT8 gLcdCloseFlag = 0;


/* device capability. */
static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};


static code UINT32 dispPwrOnCmdTbl[] = {
	DISP_LCM_CMD_WAIT | 120,						//ms
	
	DISP_LCM_CMD_INST | 0x11,						//Sleep out
	
	DISP_LCM_CMD_WAIT | 120,						//Delay 120ms
	
	DISP_LCM_CMD_INST | 0x36,	  
	DISP_LCM_CMD_PARA | 0x70,

	DISP_LCM_CMD_INST | 0x3a,
	DISP_LCM_CMD_PARA | 0x55, //18 RGB ,55-16BIT RGB
	
	DISP_LCM_CMD_INST | 0x21,	  
	
	DISP_LCM_CMD_INST | 0xB2,	  
	DISP_LCM_CMD_PARA | 0x7f,	
	DISP_LCM_CMD_PARA | 0x7f,	
	DISP_LCM_CMD_PARA | 0x00,	
	DISP_LCM_CMD_PARA | 0xff,	
	DISP_LCM_CMD_PARA | 0xff,	
	
	DISP_LCM_CMD_INST | 0xB7,	  
	DISP_LCM_CMD_PARA | 0x75, // 0x35,	
	
	DISP_LCM_CMD_INST | 0xBB,	  
	DISP_LCM_CMD_PARA | 0x22, // 0x0C,	
	
	DISP_LCM_CMD_INST | 0xC0,	  
	DISP_LCM_CMD_PARA | 0x2C,	
	
	DISP_LCM_CMD_INST | 0xC2,	  
	DISP_LCM_CMD_PARA | 0x01,	
	
	DISP_LCM_CMD_INST | 0xC3,	  
	DISP_LCM_CMD_PARA | 0x18,//0x13, // 0x0B,	
	
	DISP_LCM_CMD_INST | 0xC4,	  
	DISP_LCM_CMD_PARA | 0x20, // 0x30,	
	
	DISP_LCM_CMD_INST | 0xC6,	  
	DISP_LCM_CMD_PARA | 0x0f, // 0x09 0X0F
	
	DISP_LCM_CMD_INST | 0xD0,	  
	DISP_LCM_CMD_PARA | 0xA4,	
	DISP_LCM_CMD_PARA | 0xA1,	
	
	DISP_LCM_CMD_INST | 0xD6,	  
	DISP_LCM_CMD_PARA | 0xA1,	

	DISP_LCM_CMD_INST | (0x35),
	DISP_LCM_CMD_PARA | (0x00),
	
	DISP_LCM_CMD_INST | 0xE0,	  
	DISP_LCM_CMD_PARA | 0xD0,	
	DISP_LCM_CMD_PARA | 0x05,	
	DISP_LCM_CMD_PARA | 0x0A,	
	DISP_LCM_CMD_PARA | 0x09,	
	DISP_LCM_CMD_PARA | 0x08,	
	DISP_LCM_CMD_PARA | 0x05,	
	DISP_LCM_CMD_PARA | 0x2E,	
	DISP_LCM_CMD_PARA | 0x44,	
	DISP_LCM_CMD_PARA | 0x45,	
	DISP_LCM_CMD_PARA | 0x0F,	
	DISP_LCM_CMD_PARA | 0x17,	
	DISP_LCM_CMD_PARA | 0x16,	
	DISP_LCM_CMD_PARA | 0x2B,	
	DISP_LCM_CMD_PARA | 0x33,	
	
	DISP_LCM_CMD_INST | 0xE1,	  
	DISP_LCM_CMD_PARA | 0xD0,	
	DISP_LCM_CMD_PARA | 0x05,	
	DISP_LCM_CMD_PARA | 0x0A,	
	DISP_LCM_CMD_PARA | 0x09,	
	DISP_LCM_CMD_PARA | 0x08,	
	DISP_LCM_CMD_PARA | 0x05,	
	DISP_LCM_CMD_PARA | 0x2E,	
	DISP_LCM_CMD_PARA | 0x43,	
	DISP_LCM_CMD_PARA | 0x45,	
	DISP_LCM_CMD_PARA | 0x0F,	
	DISP_LCM_CMD_PARA | 0x16,	
	DISP_LCM_CMD_PARA | 0x16,	
	DISP_LCM_CMD_PARA | 0x2B,	
	DISP_LCM_CMD_PARA | 0x33,	
	
	

	DISP_LCM_CMD_INST | 0x2a,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x01,
	DISP_LCM_CMD_PARA | 0x3f,

	DISP_LCM_CMD_INST | 0x2b,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0xef,

	DISP_LCM_CMD_INST | 0x29,	  //Display on

	DISP_LCM_CMD_INST | 0x2c,

	DISP_LCM_CMD_STOP,
};

static code UINT32 dispPwrOffCmdTbl[] = {
	DISP_LCM_CMD_INST | 0x11,    	// Exit Sleep 
	DISP_LCM_CMD_WAIT | 120,
	
	DISP_LCM_CMD_INST | 0x28,  /* dispoff. */
	DISP_LCM_CMD_INST | 0x2C,
	DISP_LCM_CMD_STOP, 
};






/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-07-25
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
 * @since	2008-07-25
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
 * @since	2008-07-25
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
 * @since	2008-07-25
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
dispOpPowerOn(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();

	EA=0;

	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()!=0x01))
	{
		XBYTE[REG_Disp_VsVdIntEn] &= ~0x02;
	}
	else if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()==0x01))
	{
		XBYTE[REG_Disp_Rsv0] &= ~0x04;
	}

	/* gpio 9 LCM reset */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	sp1kHalWait(50);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x00 << (9 & 0x07));
	sp1kHalWait(110);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	sp1kHalWait(100);
	
	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	dispParaSet(DISP_PARA_SW_RST, DISP_DEV_RST_ALL, 0, 0, 0, 0);
	// dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
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
	
	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);

	
	#if 1
	XBYTE[REG_Disp_GpioSel1] |= 0x01 << (8 & 0x07); //set DGPIO 8 as TE pin in
	XBYTE[REG_Disp_GpioOutEn1] &= ~(0x01 << (8 & 0x07));
	XBYTE[REG_Disp_PhsSel_Polar] |= 0x10;
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] |= 0x03;
	#endif

	/* disp enable */
	dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);

	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()!=0x01))
	{
		XBYTE[REG_Disp_VsVdIntEn] |= 0x02;
	}
	else if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()==0x01))
	{
		XBYTE[REG_Disp_Rsv0] |= 0x04;
	}
	EA = 1;

	sp1kHalWait(100);

	app_set_lcd_backlight_status(1);
	gLcdCloseFlag = 0;
}

/**
 * @brief  operating function to power off device.
 * @param       None.
 * @retval         None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-25
 * @todo  N/A
 * @bug            N/A
*/
__STATIC void dispOpPowerOff(void)
{
	sp1kHalWait(20);
	
	dispLcmCmdTableSend(dispPwrOffCmdTbl);

	app_set_lcd_backlight_status(0);
	gLcdCloseFlag = 1;

}
