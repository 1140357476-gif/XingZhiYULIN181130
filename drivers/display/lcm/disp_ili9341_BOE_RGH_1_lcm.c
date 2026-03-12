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
#define HPIXEL    350//251//334 //182//214//182
#define VLINE   	0x806//334//251
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
#define CLK_DIVNX   	21
#define CLK_DIV1X     	2
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



//void Init_panel(void(*ptr)());
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "LCD - 2.4 inch LCM";
__STATIC  UINT16  Panel_ID;
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


static code UINT32 dispPwrOnCmdTbl[] = {
	
	DISP_LCM_CMD_INST | 0xCF,  
	DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_PARA | 0xaa, // 0xc1, 
	DISP_LCM_CMD_PARA | 0Xe0, 

	DISP_LCM_CMD_INST | 0xED,  
	DISP_LCM_CMD_PARA | 0x67, 
	DISP_LCM_CMD_PARA | 0x03, 
	DISP_LCM_CMD_PARA | 0X12, 
	DISP_LCM_CMD_PARA | 0X81, 

	DISP_LCM_CMD_INST | 0xE8,  
	DISP_LCM_CMD_PARA | 0x85, 
	DISP_LCM_CMD_PARA | 0x11, // 0x00, 
	DISP_LCM_CMD_PARA | 0x79, // 0x78, 

	DISP_LCM_CMD_INST | 0xCB,  
	DISP_LCM_CMD_PARA | 0x39, 
	DISP_LCM_CMD_PARA | 0x2C, 
	DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_PARA | 0x34, 
	DISP_LCM_CMD_PARA | 0x02, 

	DISP_LCM_CMD_INST | 0xF7,  
	DISP_LCM_CMD_PARA | 0x20, 

	DISP_LCM_CMD_INST | 0xEA,  
	DISP_LCM_CMD_PARA | 0x00, 
	DISP_LCM_CMD_PARA | 0x00, 
	
	DISP_LCM_CMD_INST | 0xB1,   
	DISP_LCM_CMD_PARA | 0x00,   
	DISP_LCM_CMD_PARA | 0x1d, // 0x1B,   // frame rate
	
	DISP_LCM_CMD_INST | 0xB6,    	// Display Function Control 
	DISP_LCM_CMD_PARA | 0x0A, 
	DISP_LCM_CMD_PARA | 0xA2, 
	
	// DISP_LCM_CMD_INST | 0xB4,   
	// DISP_LCM_CMD_PARA | 0x07, 


	DISP_LCM_CMD_INST | 0xC0,   	// Power control 
	DISP_LCM_CMD_PARA | 0x21, // 0x1B,   	// VRH[5:0] 

	DISP_LCM_CMD_INST | 0xC1,   	// Power control 
	DISP_LCM_CMD_PARA | 0x01, // 0x12,   	// SAP[2:0];BT[3:0] 
 
	DISP_LCM_CMD_INST | 0xC5,    	// VCM control 
	DISP_LCM_CMD_PARA | 0x2b, // 0x25,	// 0x35 
	DISP_LCM_CMD_PARA | 0x30, // 0x2E,	// 0x3E 
  
	DISP_LCM_CMD_INST | 0xC7,    	// VCM control2 
	DISP_LCM_CMD_PARA | 0xad, // 0xb7, // 0x88,

	DISP_LCM_CMD_INST | 0x36, 	// Set Gamma
	DISP_LCM_CMD_PARA | 0xa8,	// 0x68 //0xa8   my,mx,mv,ml,BGR,mh,0.0 
                    
	DISP_LCM_CMD_INST | 0x3a,	// Memory data  access control
	DISP_LCM_CMD_PARA | 0x05,    	// 0x05 0x66    
	
	DISP_LCM_CMD_INST | 0xB0,	// RGB polarity 
	DISP_LCM_CMD_PARA | 0x00,
	
	
	         
	DISP_LCM_CMD_INST | 0x2a,     // column set 
	DISP_LCM_CMD_PARA | 0x00,     // if MADCTL BIT5=0, EC[15:0]=0x00ef
	DISP_LCM_CMD_PARA | 0x00,     // if MADCTL BIT5=1, EC[15:0]=0x013f
	DISP_LCM_CMD_PARA | 0x01, 	// 00 //0x01
	DISP_LCM_CMD_PARA | 0x3f, 	// ef //0x3f
                
	DISP_LCM_CMD_INST | 0x2b,     // page address set 
	DISP_LCM_CMD_PARA | 0x00,	// if MADCTL BIT5=0, EP[15:0]=0x013f
	DISP_LCM_CMD_PARA | 0x00,	// if MADCTL BIT5=1, EP[15:0]=0x00ef
	DISP_LCM_CMD_PARA | 0x00, 	// 01 //0x00
	DISP_LCM_CMD_PARA | 0xef,  	// 3f //0xef 
                     
	DISP_LCM_CMD_INST | 0xF6,    
	DISP_LCM_CMD_PARA | 0x01, 
	DISP_LCM_CMD_PARA | 0x30,
                   
	DISP_LCM_CMD_INST | 0xF2,	// 3Gamma Function Disable 
	DISP_LCM_CMD_PARA | 0x00, 	
	           
	DISP_LCM_CMD_INST | 0x26,	// Gamma curve selected 
	DISP_LCM_CMD_PARA | 0x01, 	
	             
	DISP_LCM_CMD_INST | 0xE0,	// Set Gamma 
	DISP_LCM_CMD_PARA | 0x0F, 
	DISP_LCM_CMD_PARA | 0x27, // 0x1D,
	DISP_LCM_CMD_PARA | 0x23, // 0x1A, 
	DISP_LCM_CMD_PARA | 0x0b, 
	DISP_LCM_CMD_PARA | 0x0f, 
	DISP_LCM_CMD_PARA | 0x05, // 0x07, 
	DISP_LCM_CMD_PARA | 0x50, // 0x49, 
	DISP_LCM_CMD_PARA | 0x86, // 0X66, 
	DISP_LCM_CMD_PARA | 0x41, // 0x3B, 
	DISP_LCM_CMD_PARA | 0x0e, // 0x07, 
	DISP_LCM_CMD_PARA | 0x1b, // 0x11, 
	DISP_LCM_CMD_PARA | 0x35, // 0x01, 
	DISP_LCM_CMD_PARA | 0x00, // 0x09, 
	DISP_LCM_CMD_PARA | 0x00, // 0x05, 
	DISP_LCM_CMD_PARA | 0x00, // 0x04, 
                  
	DISP_LCM_CMD_INST | 0XE1,    	// Set Gamma 
	DISP_LCM_CMD_PARA | 0x00,
	DISP_LCM_CMD_PARA | 0x1a, // 0x18, 
	DISP_LCM_CMD_PARA | 0x1e, // 0x1D, 
	DISP_LCM_CMD_PARA | 0x03, // 0x02, 
	DISP_LCM_CMD_PARA | 0x0f, // 0x0F, 
	DISP_LCM_CMD_PARA | 0x05, // 0x04, 
	DISP_LCM_CMD_PARA | 0x2e, // 0x36, 
	DISP_LCM_CMD_PARA | 0x25,
	DISP_LCM_CMD_PARA | 0x3e, // 0x4C, 
	DISP_LCM_CMD_PARA | 0x01, // 0x07, 
	DISP_LCM_CMD_PARA | 0x04, // 0x13, 
	DISP_LCM_CMD_PARA | 0x0a, // 0x0F, 
	DISP_LCM_CMD_PARA | 0x3f, // 0x2E, 
	DISP_LCM_CMD_PARA | 0x3f, // 0x2F, 
	DISP_LCM_CMD_PARA | 0x0F, // 0x05, 
                     
	DISP_LCM_CMD_INST | 0x11,    	// Exit Sleep 
	DISP_LCM_CMD_WAIT | 120,
	
	DISP_LCM_CMD_INST | 0X35,
	DISP_LCM_CMD_PARA | 0x00,
	
	DISP_LCM_CMD_INST | 0Xb5,    	// 
	DISP_LCM_CMD_PARA | 0x5f,		// 00~7f vfp
	DISP_LCM_CMD_PARA | 0x5f,		// 00~7f vbp
	DISP_LCM_CMD_PARA | 0x0a,		// 00~1f hfp
	DISP_LCM_CMD_PARA | 0x14,		// 00~1f hbp
	
	DISP_LCM_CMD_INST | 0x29, 	// Display on                     
	DISP_LCM_CMD_INST | 0x2C,
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
	// UINT8 i,j;

	/* LCM reset */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x01 << (14 & 0x07));
	sp1kHalWait(50);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (14 & 0x07), 0x00 << (14 & 0x07));
	sp1kHalWait(110);
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
}
