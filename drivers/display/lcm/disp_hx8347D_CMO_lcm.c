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
 * @file		disp_gp_gpm779a0_lcm.c
 * @brief		display lcm giant plus 54124a driver.
 * @author		Matt Wang
 * @since		2008-08-15
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
#include "timer.h"
#include "sp1k_util_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_hal_api.h"
#include "gpio.h"
#include "reg_disp.h"
#include "app_ui_para.h"
#include "app_dev_disp.h"
#include "app_ui_osd.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
 
/* render dimension. */
#define DEV_TYPE  DISP_DEVICE_TYPE_LCM
#define WIDTH     320
#define HEIGHT    240 
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    	360
#define VLINE     	262
#define PNLH0     11 
#define PNLV0     3
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
#define CLK_DIVNX     	52//192
#define CLK_DIV1X      3
#define UPS_CLK_PHASE  8
#define CS0_ACT         0
#define A0_CMD          0
 
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


extern  void dispPanelDIRSet(UINT8 para);
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
 

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - SJ24N_	NV3029C LCM";


/* device capability. */
static code dispCapabDesc_t dispCapabDesc_4_3 = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};

/* lcm command table. */

static code UINT32 dispPwrOnCmdTbl[] = {
	//Driving ability Setting
	DISP_LCM_CMD_INST | 0xEA,	DISP_LCM_CMD_PARA | 0x00, 	//PTBA[15:8]
	DISP_LCM_CMD_INST | 0xEB,	DISP_LCM_CMD_PARA | 0x20, 	//PTBA[7:0]
	DISP_LCM_CMD_INST | 0xEC,	DISP_LCM_CMD_PARA | 0x08,	//STBA[15:8]
	DISP_LCM_CMD_INST | 0xED,	DISP_LCM_CMD_PARA | 0xC4, 	//STBA[7:0]
	DISP_LCM_CMD_INST | 0xE8,	DISP_LCM_CMD_PARA | 0x40,	//OPON[7:0]
	DISP_LCM_CMD_INST | 0xE9,	DISP_LCM_CMD_PARA | 0x38, 	//OPON1[7:0]
	DISP_LCM_CMD_INST | 0xF1,	DISP_LCM_CMD_PARA | 0x01, 	//OTPS1B
	DISP_LCM_CMD_INST | 0xF2,	DISP_LCM_CMD_PARA | 0x10, 	//GEN[7:0]
	DISP_LCM_CMD_INST | 0x27,	DISP_LCM_CMD_PARA | 0xA3, 	//
	DISP_LCM_CMD_INST | 0x2E,	DISP_LCM_CMD_PARA | 0x79,	//GDOFF[7:0]

	//SET PANEL	
	DISP_LCM_CMD_INST | 0x36,	DISP_LCM_CMD_PARA | 0x01, 	//SS_P, GS_P, REV_P, BGR_P

	//Gamma 2.2 Setting
	DISP_LCM_CMD_INST | 0x40,	DISP_LCM_CMD_PARA | 0x07,
	DISP_LCM_CMD_INST | 0x41,	DISP_LCM_CMD_PARA | 0x07,
	DISP_LCM_CMD_INST | 0x42,	DISP_LCM_CMD_PARA | 0x07,
	DISP_LCM_CMD_INST | 0x43,	DISP_LCM_CMD_PARA | 0x13,
	DISP_LCM_CMD_INST | 0x44,	DISP_LCM_CMD_PARA | 0x11,
	DISP_LCM_CMD_INST | 0x45,	DISP_LCM_CMD_PARA | 0x24,
	DISP_LCM_CMD_INST | 0x46,	DISP_LCM_CMD_PARA | 0x10,
	DISP_LCM_CMD_INST | 0x47,	DISP_LCM_CMD_PARA | 0x57,
	DISP_LCM_CMD_INST | 0x48,	DISP_LCM_CMD_PARA | 0x09,
	DISP_LCM_CMD_INST | 0x49,	DISP_LCM_CMD_PARA | 0x14,
	DISP_LCM_CMD_INST | 0x4A,	DISP_LCM_CMD_PARA | 0x19,
	DISP_LCM_CMD_INST | 0x4B,	DISP_LCM_CMD_PARA | 0x19,
	DISP_LCM_CMD_INST | 0x4C,	DISP_LCM_CMD_PARA | 0x16,
	DISP_LCM_CMD_INST | 0x50,	DISP_LCM_CMD_PARA | 0x1B,
	DISP_LCM_CMD_INST | 0x51,	DISP_LCM_CMD_PARA | 0x2E,
	DISP_LCM_CMD_INST | 0x52,	DISP_LCM_CMD_PARA | 0x2C,
	DISP_LCM_CMD_INST | 0x53,	DISP_LCM_CMD_PARA | 0x38,
	DISP_LCM_CMD_INST | 0x54,	DISP_LCM_CMD_PARA | 0x38,
	DISP_LCM_CMD_INST | 0x55,	DISP_LCM_CMD_PARA | 0x3E,
	DISP_LCM_CMD_INST | 0x56,	DISP_LCM_CMD_PARA | 0x2A,
	DISP_LCM_CMD_INST | 0x57,	DISP_LCM_CMD_PARA | 0x6F,
	DISP_LCM_CMD_INST | 0x58,	DISP_LCM_CMD_PARA | 0x09,
	DISP_LCM_CMD_INST | 0x59,	DISP_LCM_CMD_PARA | 0x06,
	DISP_LCM_CMD_INST | 0x5A,	DISP_LCM_CMD_PARA | 0x06,
	DISP_LCM_CMD_INST | 0x5B,	DISP_LCM_CMD_PARA | 0x0B,
	DISP_LCM_CMD_INST | 0x5C,	DISP_LCM_CMD_PARA | 0x16,
	DISP_LCM_CMD_INST | 0x5D,	DISP_LCM_CMD_PARA | 0xCC,

	//Power Voltage Setting
	DISP_LCM_CMD_INST | 0x1B,	DISP_LCM_CMD_PARA | 0x1B, 	//VRH=4.65V
	DISP_LCM_CMD_INST | 0x1A,	DISP_LCM_CMD_PARA | 0x01, 	//BT (VGH~15V,VGL~-10V,DDVDH~5V)
	DISP_LCM_CMD_INST | 0x24,	DISP_LCM_CMD_PARA | 0x2F, 	//VMH(VCOM High voltage ~3.2V)
	DISP_LCM_CMD_INST | 0x25,	DISP_LCM_CMD_PARA | 0x57, 	//VML(VCOM Low voltage -1.2V)

	//****VCOM offset**///		
	DISP_LCM_CMD_INST | 0x23,	DISP_LCM_CMD_PARA | 0x8a,	//for Flicker adjust //can reload from OTP

	//Power on Setting	
	DISP_LCM_CMD_INST | 0x18,	DISP_LCM_CMD_PARA | 0x39,	//I/P_RADJ,N/P_RADJ, Normal mode 60Hz
	DISP_LCM_CMD_INST | 0x19,	DISP_LCM_CMD_PARA | 0x01, 	//OSC_EN='1', start Osc
	DISP_LCM_CMD_INST | 0x01,	DISP_LCM_CMD_PARA | 0x00, 	//DP_STB='0', out deep sleep
	DISP_LCM_CMD_INST | 0x1F,	DISP_LCM_CMD_PARA | 0x88,	// GAS=1, VOMG=0, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
	DISP_LCM_CMD_WAIT | 5,	
	DISP_LCM_CMD_INST | 0x1F,	DISP_LCM_CMD_PARA | 0x80,	// GAS=1, VOMG=0, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
	DISP_LCM_CMD_WAIT | 5,	
	DISP_LCM_CMD_INST | 0x1F,	DISP_LCM_CMD_PARA | 0x90,	// GAS=1, VOMG=0, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
	DISP_LCM_CMD_WAIT | 5,	
	DISP_LCM_CMD_INST | 0x1F,	DISP_LCM_CMD_PARA | 0xD0,	// GAS=1, VOMG=1, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
	DISP_LCM_CMD_WAIT | 5,

	//262k/65k color selection
	DISP_LCM_CMD_INST | 0x17,	DISP_LCM_CMD_PARA | 0x06, 	//default 0x06 262k color // 0x05 65k color//0x03 4k color
	DISP_LCM_CMD_INST | 0x16,	DISP_LCM_CMD_PARA | 0x60,
		
	//Display ON Setting	
	DISP_LCM_CMD_INST | 0x28,	DISP_LCM_CMD_PARA | 0x38, 	//GON=1, DTE=1, D=10
	DISP_LCM_CMD_WAIT | 40,	
	DISP_LCM_CMD_INST | 0x28,	DISP_LCM_CMD_PARA | 0x3C, 	//GON=1, DTE=1, D=11

	//Set GRAM Area
	DISP_LCM_CMD_INST | 0x02,	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_INST | 0x03,	DISP_LCM_CMD_PARA | 0x00, 		//Column Start
	DISP_LCM_CMD_INST | 0x04,	DISP_LCM_CMD_PARA | 0x01,		
	DISP_LCM_CMD_INST | 0x05,	DISP_LCM_CMD_PARA | 0x3F, 		//Column End
	DISP_LCM_CMD_INST | 0x06,	DISP_LCM_CMD_PARA | 0x00,		
	DISP_LCM_CMD_INST | 0x07,	DISP_LCM_CMD_PARA | 0x00, 		//Row Start
	DISP_LCM_CMD_INST | 0x08,	DISP_LCM_CMD_PARA | 0x00,		
	DISP_LCM_CMD_INST | 0x09,	DISP_LCM_CMD_PARA | 0xef, 		//Row End
	
	DISP_LCM_CMD_INST | 0x70,	DISP_LCM_CMD_PARA | 0x1e, 		//Row End
		
	DISP_LCM_CMD_INST | 0x22, 						//Start GRAM write
	
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
	*pcapabDesc = dispCapabDesc_4_3;
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

	UINT8 i;
	SINT16 mtxA[9] = {0x0100,0x0799,0x001A,0x07f0,0x00e7,0x07db,0x07f1,0x07f0,0x00a6};
	SINT16 mtxB[3] = {0x00};
	
	/* LCM reset */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	sp1kHalWait(10);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x00 << (14 & 0x07));
	sp1kHalWait(5);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	sp1kHalWait(1);
	
	/* set clk and type. */
	dispParaSet(DISP_PARA_SW_RST, DISP_DEV_RST_ALL, 0, 0, 0, 0);
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
	dispParaSet(DISP_PARA_COLOR_MATRIX_TBL_CFG, (SINT16)mtxA, (SINT16)mtxB, 0, 0, 0);	
	dispParaSet(DISP_PARA_COLOR_MATRIX_FMT_CFG, DISP_COLOR_MATRIX_FMT_RGB, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0);

	/* set lcm interface. */
	dispParaSet(DISP_PARA_LCM_CMD_BUS_TYPE, BUS_TYPE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_CMD_ACTIVE_LEVEL, CS0_ACT, A0_CMD, 0, 0, 0);
	dispParaSet(DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, 0, 0, 0, 0);
	//dispParaSet(DISP_PARA_LCM_DITHER_ENABLE, DITHER, 0, 0, 0, 0);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);

	#if 1
	XBYTE[REG_Disp_GpioSel1] |= 0x01 << (12 & 0x07); //set DGPIO12 as TE pin in
	XBYTE[REG_Disp_GpioOutEn1] &= ~(0x01 << (12 & 0x07));
	XBYTE[REG_Disp_PhsSel_Polar] |= 0x10;
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] |= 0x01;
	//XBYTE[0x2028] = 0x31;
	#endif

	/* disp enable */
	dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);
	
	sp1kHalWait(100);
	app_set_lcd_backlight_status(1);
}
/**
 * @brief  operating function to power off device.
 * @param       None.
 * @retval         None.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

__STATIC void dispOpPowerOff(void)
{
//	appTFT_Reset(0);
	sp1kHalWait(20);
         /* send lcm command. */	
	app_set_lcd_backlight_status(0);
}