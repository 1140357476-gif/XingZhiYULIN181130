/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      app_suspend.c
 * @brief     suspend & resume handle
 * @author    Phil Lin
 * @since     2011-03-03
 * @date      2011-03-03
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

#include "initio.h"
#include "dbg_wdt.h"	 
#include "dbg_def.h"
#include "gpio_api.h"

#include "sp1k_hal_api.h"
#include "sp1k_front_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"
#include "sp1k_util_api.h"
#include "sp1k_gpio_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_osd_api.h"

#include "app_msg.h"
#include "app_suspend.h"
#include "app_dev_usb.h"
#include "app_battery.h"
#include "app_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_disp.h"
#include "app_audio_play.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HUNTING_CAM  0

#define RESUME_AS_POWER_ON 0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if !TAG_TODO
#define sp1kHalCamModeSet(a)
#endif
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT8 gpiopullmap[13];
static UINT8 gpiooutmap[4],gpiofunmap[4],gpiodirmap[4];
static UINT8 fmoutmap[4],fmdirmap[4];//fmfunmap;
static UINT8 dispoutmap[4],dispfunmap[4],dispdirmap[4];
static UINT8 tgmap[3];
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 GmountCard;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 appSpdEnvChk(void)
 * @brief     Host check the enviroment for suspend
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
appSpdEnvChk(
	void
)
{
#if HUNTING_CAM
	sp1kHalCtrlScreen(0);
	sp1kHalWait(20);
	return SUCCESS;
#else
	if ((!prjDbgMode) && (sp1kUSB_PlugGet() == USB_PLUG_IN)) {
		sp1kHalCtrlScreen(0);
		sp1kHalWait(20);
		return SUCCESS;
	}	
#endif

	return SUCCESS;
}

/**
 * @fn        void appSpdPreProc(void)
 * @brief     Suspend pre-process
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appSpdPreProc(
	void
)
{
	/*
		Suspend pre-process
		1. Stop system service
		2. Stop host application
		3. Peripheral device power off/enter suspend
	*/
	//printf("#Enter Suspend!##\n");

	dbgWdtSet(0);
}
/**
 * @fn        void appSpdGpioHdl(void)
 * @brief     Suspend GPIO handle
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appSpdGpioHdl(
	void
)
{
	/*
		Suspend GPIO handle
		1. Save all the GPIO status
		2. GPIO custmization before entering suspend(for power save)
	*/

	/*
		//Current Tuning Principle for GPIO
		1.Power Hold setting : Output driving High !
		2.For external pull high setting : Output driving High !
		3.For external pull low setting : Output driving Low !
		4.For Sensor setting : Output driving should be turn off the sensor & reset driving High !
		5.For LCD setting : Output driving should be turn off the LCD !
		6.Other Pins setting : Output driving Low !
		7.Disable All GPIO Pull up/down !
		8.All module power down
	*/

	UINT8 tgSts;
	UINT8 i;	

	/*reg save*/	
	for(i=0;i<13;i++){			
		gpiopullmap[i] = gpioPullEnGet(i,0xff);//XBYTE[0x20a0+i];	
		if(i<4){
			dispoutmap[i] = gpioByteOutGet(GPIO_BYTE_DISP0+i,0xff);
			dispfunmap[i] = gpioByteFuncGet(GPIO_BYTE_DISP0+i,0xff);
			dispdirmap[i] = gpioByteDirGet(GPIO_BYTE_DISP0+i,0xff);
			
			if(i<3) {	
				fmoutmap[i] = gpioByteOutGet(GPIO_BYTE_FM0+i,0xff);
				fmdirmap[i] = gpioByteDirGet(GPIO_BYTE_FM0+i,0xff);
			}

			gpiooutmap[i] = gpioByteOutGet(GPIO_BYTE_GEN0+i,0xff);
			gpiofunmap[i] = gpioByteFuncGet(GPIO_BYTE_GEN0+i,0xff);
			gpiodirmap[i] = gpioByteDirGet(GPIO_BYTE_GEN0+i,0xff);
			//printf("SgpioO:%bx,F:%bx,D:%bx\n",gpiooutmap[i],gpiofunmap[i],gpiodirmap[i]);
		}			
	}
	//save TGGPIO function
	tgmap[0] = gpioByteOutGet(GPIO_BYTE_TG0,0x3f);//XBYTE[0x2a64] ;
	tgmap[1] = gpioByteDirGet(GPIO_BYTE_TG0,0x3f);//XBYTE[0x2a62] ;
	tgmap[2] = gpioByteFuncGet(GPIO_BYTE_TG0,0x3f);//XBYTE[0x2a60] ;
	
	//GPIO_4.5.7.12.23 Output-High 	
	gpioByteFuncSet(GPIO_BYTE_GEN0,0x18,0x18);//GPIO 3,4 as gpio
	gpioByteOutSet(GPIO_BYTE_GEN0,0xff,0xB4); //gpio2, 4.5 7output high
	gpioByteOutSet(GPIO_BYTE_GEN1,0xff,0x0C);//gpio 10.11 output high
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	gpioByteOutSet(GPIO_BYTE_GEN2,0xff,0x80); //gpio 23 sensor powr down
	gpioByteOutSet(GPIO_BYTE_GEN3,0xff,0x00);
	
	//GPIO_OE 	  
	gpioByteDirSet(GPIO_BYTE_GEN0,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_GEN1,0xff,0xfd); //skip gpio9 for input
	gpioByteDirSet(GPIO_BYTE_GEN2,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_GEN3,0xff,0xff);
	
	//FMGPIO			
	gpioByteFuncSet(GPIO_BYTE_FM0,0xff,0xff); //set all fm cotrol pin as gpios
	gpioByteOutSet(GPIO_BYTE_FM0,0xff,0x01);//FM0	
	gpioByteOutSet(GPIO_BYTE_FM1,0xff,0x04);//FM10
	gpioByteOutSet(GPIO_BYTE_FM2,0xff,0x02);//FM17
				  
	gpioByteDirSet(GPIO_BYTE_FM0,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_FM1,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_FM2,0xff,0xff);

	//get sts of tggpio
	gpioByteFuncSet(GPIO_BYTE_TG0,0x3f,0x3f);
	gpioByteDirSet(GPIO_BYTE_TG0,0x3f,0x00);		
	tgSts = gpioByteInGet(GPIO_BYTE_TG0,0x3f,1);
	//TG							
	gpioByteFuncSet(GPIO_BYTE_TG0,0x3f,0x3f);
	gpioByteOutSet(GPIO_BYTE_TG0,0x3f,tgSts);
	gpioByteDirSet(GPIO_BYTE_TG0,0x3f,0x3f);		
	//Display					  
	gpioByteFuncSet(GPIO_BYTE_DISP0,0xff,0xff);
	gpioByteFuncSet(GPIO_BYTE_DISP1,0xff,0xff);
	gpioByteFuncSet(GPIO_BYTE_DISP2,0xff,0xff);
	gpioByteFuncSet(GPIO_BYTE_DISP3,0xff,0xff);

	gpioByteOutSet(GPIO_BYTE_DISP0,0xff,0x00);
	gpioByteOutSet(GPIO_BYTE_DISP1,0xff,0x00);
	gpioByteOutSet(GPIO_BYTE_DISP2,0xff,0x00);
	gpioByteOutSet(GPIO_BYTE_DISP3,0xff,0x00);

	gpioByteDirSet(GPIO_BYTE_DISP0,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_DISP1,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_DISP2,0xff,0xff);
	gpioByteDirSet(GPIO_BYTE_DISP3,0xff,0xff);
	
	
	//CARD_PWR_OFF			
	sp1kHalPwrMosSet(0);
	
	//Audio module OFF	
	sp1kHalAudPwerSave(1);

#if 0
	//Pull Disable			   
	gpioPullEnSet(0,0xff,0x00);
	gpioPullEnSet(1,0xff,0x00);
	gpioPullEnSet(2,0xff,0x00);
	gpioPullEnSet(3,0xff,0x00);
	gpioPullEnSet(4,0xff,0x00);
	gpioPullEnSet(5,0xff,0x04);//gpio 9
	gpioPullEnSet(6,0xff,0x00);
	gpioPullEnSet(7,0xff,0x00);
	gpioPullEnSet(8,0xff,0x00);
	gpioPullEnSet(9,0xff,0x00);
	gpioPullEnSet(10,0xff,0x00);
	gpioPullEnSet(11,0xff,0x00);
	gpioPullEnSet(12,0xff,0x00);
#else
	appSpdGpioPullEnSet();
#endif

}
/**
 * @fn        void appSpdIsrReg(void)
 * @brief     Suspend interrupt service register
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appSpdIsrReg(
	void
)
{
	/*
		Suspend interrupt service register
		1. Register the GPIO/RTC interrupt service for resume
		2. Clear the ISR status and enable them
	*/

	sp1kSuspendUiIntrSet(1);
}

/**
 * @fn        void appRsmIsrHdl(void)
 * @brief     Resume ISR handle
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appRsmIsrHdl(
	void
)
{
	/*
		Resume ISR handle
		1. Save the interrupt status & clear them
		2. Disable the interrupt for suspend
	*/

	sp1kSuspendUiIntrSet(0);
}
/**
 * @fn        UINT8 appRsmEnvChk(void)
 * @brief     Host check enviroment for resume
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
appRsmEnvChk(
	void
)
{
	/*
		Host check enviroment for resume
		1. System backing to suspend when the checking result is fail
	*/

	return SUCCESS;
}
/**
 * @fn        UINT8 appRsmGpioHdl(void)
 * @brief     Resume GPIO handle
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appRsmGpioHdl(
	void
)
{
	/*
		Resume GPIO handle
		1. Recover all the GPIO status
	*/

	UINT8 i;	

	//CARD_PWR_ON			
	sp1kHalPwrMosSet(1);
	
	//Audio module resume	
	sp1kHalAudPwerSave(0);	

	//recover reg 
	for(i=0;i<13;i++){			
		gpioPullEnSet(i,0xff,gpiopullmap[i]);//XBYTE[0x20a0+i];	
		if(i<4){
			gpioByteOutSet(GPIO_BYTE_DISP0+i,0xff,dispoutmap[i]);
			gpioByteFuncSet(GPIO_BYTE_DISP0+i,0xff,dispfunmap[i]);
		 	gpioByteDirSet(GPIO_BYTE_DISP0+i,0xff,dispdirmap[i]);
			
			if (i<3) {	
			 	gpioByteOutSet(GPIO_BYTE_FM0+i,0xff,fmoutmap[i]);
			 	gpioByteDirSet(GPIO_BYTE_FM0+i,0xff,fmdirmap[i]);
			}

		 	gpioByteOutSet(GPIO_BYTE_GEN0+i,0xff,gpiooutmap[i]);
		 	gpioByteFuncSet(GPIO_BYTE_GEN0+i,0xff,gpiofunmap[i]);
		 	gpioByteDirSet(GPIO_BYTE_GEN0+i,0xff,gpiodirmap[i]);
			//printf("RgpioO:%bx,F:%bx,D:%bx\n",gpiooutmap[i],gpiofunmap[i],gpiodirmap[i]);
		}	
	}
	//XBYTE[0x2419] = fmfunmap;	//recover FMGPIO function
	gpioByteOutSet(GPIO_BYTE_TG0,0x3f,tgmap[0]);//XBYTE[0x2a64] ;
	gpioByteDirSet(GPIO_BYTE_TG0,0x3f,tgmap[1]);//XBYTE[0x2a62] ;
	gpioByteFuncSet(GPIO_BYTE_TG0,0x3f,tgmap[2]);//XBYTE[0x2a60] ;	
}
/**
 * @fn        UINT8 appRsmPostProc(void)
 * @brief     Resume post-process
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appRsmPostProc(
	void
)
{
	/*
		Resume post-process
		1. Peripheral device power on/exit suspend
		2. Start system service
		3. Start host application
	*/
	
#if HUNTING_CAM
	sp1kAeSetModeEn(AE_ENABLE);
	sp1kAwbEnable();
	dbgWdtSet(1);
	GmountCard=1;
#else

#if 1
	sp1kDispPowerOn(1);
	dispDevActSet(1);
#else
	sp1kDispPowerOn(sp1kDispDevActGet());
#endif

#if RESUME_AS_POWER_ON
	appRsmAsPwrOn();
	//appStorageMount(K_MEDIA_SDF);	
#else
	appStorageMount(K_MEDIA_SDF);
	sp1kHalCtrlScreen(2);
#endif

	sp1kAeSetModeEn(AE_ENABLE);
	sp1kAwbEnable();
	dbgWdtSet(1);
#endif
}

/**
 * @fn        void appRsmAsPwrOn(void)
 * @brief     appRsmAsPwrOn
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/

//#define appRsmAsPwrOn_Print	printf
#ifndef appRsmAsPwrOn_Print
#define	appRsmAsPwrOn_Print	/_printf
#endif
void appRsmAsPwrOn(void) 
{
	UINT32 srcAdd;
	UINT32 dstAdd;
	powerBatLv_t batlv;
	UINT8 resID = 0x0d; /* the same as start up image */

	sp1kHalWait(100);
	appBatLvGet(&batlv);
	appRsmAsPwrOn_Print("batlv=%bu\n",batlv);
	//battery empty flow
	if (batlv >= POWER_BAT_BELOWEMPTY) 
	{
		appRsmAsPwrOn_Print("battery empty!\n");
		uiUpdateOSDBatEmpty();
		while(1);
	}
	
	srcAdd=K_SDRAM_Pv2BufAddrD-2*(320L*240L);//mantis #26146
	dstAdd=srcAdd-3*(320L*240L);

	sp1kHalCtrlScreen(0);
	sp1kOsdClear(0);
	sp1kHalCdspImgSrcSel(1);
	sp1kHalFrontInputGate(0x11);
	sp1kHalCamModeSet(CAM_MODE_IDLE);

	sp1kHalCdspImgSrcSel(1);

	appBGImgDec(resID, NULL, srcAdd, dstAdd, 1);
	sp1kDispImgShow(dstAdd, (UINT16)320, (UINT16)240, 100);
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_OPEN, VOICE_PLAYLEVLE_HIGH);
	//sp1kHalWait(1000);//mantis:27691
	sp1kHalCtrlScreen(3);
	/*simulate open*/
}
/**
 * @fn        UINT8 appRsmBackOn(void)
 * @brief     host back to the pre-status before entering power off status
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
static void
appRsmBackOn(
	void
)
{
#if HUNTING_CAM
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	osMsgPost(SP1K_MSG_KEY_PRESS_S2);
#else
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);

	if(sp1kHDMI_PlugGet()){
		appDispHdmiOpen();
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
	} else {
		if(sp1kTV_PlugGet() == 0){
			appDispSet();
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
	}
#endif
}

/**
 * @fn        UINT8 appSuspendResume(void)
 * @brief     appSuspendResume
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appSuspendResume(
	void
)
{
	UINT8 ret;
	UINT8 sts;

	/*
		Host check the enviroment for suspend
	*/
	ret = appSpdEnvChk();
	if (ret != SUCCESS) {
		return ret;
	}

	ENTER_CRITICAL(sts);

	/*
		Suspend pre-process
		1. Stop system service
		2. Stop host application
		3. Peripheral device power off/enter suspend
	*/
	appSpdPreProc();

	/*
		Suspend GPIO handle
		1. Save all the GPIO status
		2. GPIO custmization before entering suspend(for power save)
	*/
	appSpdGpioHdl();

	/*
		Suspend interrupt service register
		1. Register the GPIO/RTC interrupt service for resume
		2. Clear the ISR status and enable them
	*/
	appSpdIsrReg();

	while (1) {

		/* System enter suspend here */

		sp1kSuspendResume();

		/* System resume from here */
		
		/*
			Resume ISR handle
			1. Save the interrupt status & clear them
			2. Disable the interrupt for suspend
		*/
		appRsmIsrHdl();

		/*
			Host check enviroment for resume
			1. System backing to suspend when the checking result is fail
		*/
		ret = appRsmEnvChk();
		if (ret == SUCCESS) {
			break;
		}
	}

	/*
		Resume GPIO handle
		1. Recover all the GPIO status
	*/
	appRsmGpioHdl();

	EXIT_CRITICAL(sts);

	/*
		Resume post-process
		1. Peripheral device power on/exit suspend
		2. Start system service
		3. Start host application
	*/
	appRsmPostProc();

	/* host back to the pre-status before entering power off status */
	
	appRsmBackOn();

	return ret;
}

