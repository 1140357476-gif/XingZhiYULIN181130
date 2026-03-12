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
#define WIDTH    320 //320//240//320 
#define HEIGHT   240 // 240//320//240
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    350 // 350//251//334 //182//214//182
#define VLINE     0X7FF // 260//334//251
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
#define CLK_SRC       		DISP_CLK_SRC_SYSPLL522M// DISP_CLK_SRC_SYSPLL522M DISP_CLK_SRC_XTAL27M
#define CLK_DIVNX   		52 	// 3
#define CLK_DIV1X     	2//3// 2
#define UPS_CLK_PHASE  	8
#define CS0_ACT         0
#define A0_CMD          0
 
/* bus type */
#define BUS_TYPE        DISP_LCM_CMD_BUS_I80

/* data format */
//#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T444_1
#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T565_1
//#define DATA_FMT        DISP_LCM_DATA_FMT_B8_T666_1

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


extern UINT8 HAL_GlobalDGPIOModeGet(void);
//extern void sensor_flip(UINT8 i); //lsh@20110630
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



//void Init_panel(void(*ptr)());
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - 2.4 inch LCM";
__STATIC  UINT16  Panel_ID;
void dispSyncInterEnable(void) ;

static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};




static code UINT32 dispPwrOnCmdTbl[] = 
{
	DISP_LCM_CMD_INST | 0x00, DISP_LCM_CMD_INST | 0x10,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01,		//Software Reset
	DISP_LCM_CMD_WAIT | 10,

	DISP_LCM_CMD_INST | 0x01, DISP_LCM_CMD_INST | 0x10,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_WAIT | 10,
	
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x62,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,

	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x61,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xa4, // 0x64, a4  	/*bit7/6/5,MX,MY,MV*/

	//Window Horizontal RAM Address Start
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xA0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x28,		//Low Address start
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xA1,		
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,		//High Address start

	//Window Horizontal RAM Address End
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xA2,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x17,		//Low Address End,0xEF+0X28
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xA3,		
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01,		//High Address End

	//Window Vertical RAM Address Start
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xB0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,		//Low Address start
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xB1,		
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,		//High Address start

	//Window Vertical RAM Address End
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xB2,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x3F,		//Low Address start
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xB3,		
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01,		//High Address start

	//Horizontal RAM Address
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xD0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x28, // 0x17
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xD1,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00, // 0x01

	//Vertical RAM Address
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xD2,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0X3f, // 0x3f,
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xD3,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0X01, // 0x01,


	//Frame Tearing Effect Position and Output cycle
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x50, 	//j te wei zhi
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xff, 
	
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x51,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,

	//High speed RAM access, RAM data transfer format
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x60,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,		//0X0000 565数据格式，0X0010 666数据格式

	//Resizing Control
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x70,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,

	//Number of Gate-lines setting
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x71,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x9F,		//屏闪 EF

	//Normary Black, To lit the non-display area
	DISP_LCM_CMD_INST | 0x03, DISP_LCM_CMD_INST | 0x72,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01,

	//Pixel output format, Individual gamma setting
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x00,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x12,

	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x10,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,

	//FP=2, BP=2
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x20,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x22,

	//Panel Interface Control (RTN:1H)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x30, 	//frame rate
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x6a, // 0x6a, // 0x6a, 	//10 - 7f 212

	//Panel Interface Control (FS)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x40,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x08, // 0x2b, // 0x08,

	//Panel Interface Control (FW)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x41,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x1c, // 0x3f, // 0x1C,

	//Panel Interface Control (STI)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x50,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x0C,

	//Panel Interface Control (SWI)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x51,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xD8,

	//Panel Interface Control (MUXS)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x60,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x12,

	//Panel Interface Control (MUXW)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x70,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x1A,

	//Panel Interface Control (MUXG)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x80,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x06,

	//Panel Interface Control (MCP)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0x90,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x05,

	//Panel Interface Control (RCKT)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xA0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x06,

	//Panel Interface Control (RCKW)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xA1,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xEA,

	//Panel Interface Control (SOE)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xB0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xFF, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xB1,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xB2,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xF7, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xB3,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01, 

	//Panel Interface Control (SOP)
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xC0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xC1,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xC2,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xC3,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00, 

	//Source Output frame switching
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xE0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01, 

	//6SSD switching pattern
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xE1,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x3F, 

	//Operating Current source amplifier
	DISP_LCM_CMD_INST | 0x0B, DISP_LCM_CMD_INST | 0xF0,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x05, 

	//VCOM switching mode
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x10,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x02, 

	//step-up frequency of DDVDH1/AVDD
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x30,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x22,

	//Cycle of DCK/DCKB
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x31,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x20,

	//Gamma reference voltage (GVDD)
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x40,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x55,		//55 

	//Driver Driving voltage (PVDD)
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x41,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x06, 

	//Panel CS voltage (VCOMH)
	DISP_LCM_CMD_INST | 0x0C, DISP_LCM_CMD_INST | 0x50,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0xC8,  	/*-dark,+light*/


	//Set Sleep-Out command
	DISP_LCM_CMD_INST | 0x01, DISP_LCM_CMD_INST | 0x10,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_WAIT | 100,

	//Display-On command 
	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0x90,
	DISP_LCM_CMD_PARA | 0x00, DISP_LCM_CMD_PARA | 0x01,

	DISP_LCM_CMD_INST | 0x02, DISP_LCM_CMD_INST | 0xC0,

	DISP_LCM_CMD_STOP,
};

// static code UINT32 dispPwrOffCmdTbl[] = {
	// DISP_LCM_CMD_INST | 0x00,
	// DISP_LCM_CMD_INST | 0x10,
	// DISP_LCM_CMD_PARA | 0x00,
	// DISP_LCM_CMD_PARA | 0x00,
	
	// DISP_LCM_CMD_WAIT | 120,
	
	// DISP_LCM_CMD_INST | 0x00,
	// DISP_LCM_CMD_INST | 0x07,
	// DISP_LCM_CMD_PARA | 0x10,
	// DISP_LCM_CMD_PARA | 0x17,
	
	// DISP_LCM_CMD_INST | 0x22,
	
	// DISP_LCM_CMD_STOP,
// };

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
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc)
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
//typedef void (*pFun)(void);
 //void (*pFuntr)(void);
__STATIC void
dispOpPowerOn(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	// UINT8 i,j;
	 
	#if 0
	EA=0;

	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()!=0x01))
	{
		XBYTE[REG_Disp_VsVdIntEn] &= ~0x02;
	}
	else if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()==0x01))
	{
		XBYTE[REG_Disp_Rsv0] &= ~0x04;
	}
	#endif

	/* LCM reset */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	sp1kHalWait(50);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x00 << (14 & 0x07));
	sp1kHalWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
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
	XBYTE[REG_Disp_GpioSel1] |= 0x01 << (12 & 0x07); //set DGPIO12 as TE pin in
	XBYTE[REG_Disp_GpioOutEn1] &= ~(0x01 << (12 & 0x07));
	XBYTE[REG_Disp_PhsSel_Polar] |= 0x10;
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] |= 0x01;
	XBYTE[0x2028] = 0x31;
	#endif
	
	/* disp enable */
	dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);
	
	#if 0
	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()!=0x01))
	{
		XBYTE[REG_Disp_VsVdIntEn] |= 0x02;
	}
	else if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()==0x01))
	{
		XBYTE[REG_Disp_Rsv0] |= 0x04;
	}
	//EXIT_CRITICAL(sts);
	EA = 1;
	#endif

	sp1kHalWait(100);
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0x01 << (15 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0x01 << (15 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0x01 << (15 & 0x07));
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
	//dispLcmCmdTableSend(dispPwrOffCmdTbl);
	sp1kHalWait(20);	
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0x01 << (15 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0x01 << (15 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (15 & 0x07), 0 << (15 & 0x07));
}
