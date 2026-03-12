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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "hal_front.h"

#include "app_ui_para.h"
#include "app_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_osd_icon.h"
#include "app_audio_play.h"
#include "app_msg.h"

#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_gpio_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_dist_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_util_api.h"
#include "app_init.h"
#include "dbg_wdt.h"	 
//#include "dist_func.h"
#include "dbg_def.h"

#include "sp1k_awb_api.h"
#include "macro_expand.h"
#include "sp1k_front_api.h"
#include "sp1k_fd_api.h"
#include "sp1k_ae_api.h"
#include "gpio_api.h"
#include "app_menu_micro.h"
#include "app_suspend.h"
#include "sp1k_aud_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if SUPPORT_SPI_WRITE_CONTROL
extern UINT8 mastWriteSpiFlag;
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
extern xdata UINT8 frameRateIsL;/* 0:60    1:30 */

/**
 * @fn        UINT8 appUDFCheckSum(void)
 * @brief     appUDFCheckSum
 * @param     NONE
 * @retval	  0:check sum ok ,load ui para from nand
 			  1:check sum err,load default ui para
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appUDFCheckSum(void)
{
	uiPara_t* ptestUiPara;
	UINT32 checksum;
	UINT16 i;

	ptestUiPara = appUiParaGet();

	checksum = 0;
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		checksum += *(((UINT8 *)ptestUiPara + i));
	}

	if(ptestUiPara->SunplusFlag[0]=='S'&&ptestUiPara->SunplusFlag[1]=='U'&&ptestUiPara->SunplusFlag[2]=='N'&&ptestUiPara->SunplusFlag[3]=='P')
	{
		if(checksum != ptestUiPara->Checksum)//checksum err
		{
			return FAIL;
		}
		else//check sum ok
		{
			return SUCCESS;
		}
	}
	else{
		return FAIL;
	}
}
/**
 * @fn        UINT8 appMENULoad(void)
 * @brief     appMENULoad
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    ZhuXiaolin
 * @since     2012-7-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appMENULoad(void)
{
	UINT8 ret;
	ret = sp1kNandRsvRead(0x100, (K_SDRAM_ShadowAddr << 1) + 0x3100UL);
	if(ret) {
		printf("menu.bin load succ\n");
	}
	else {
		printf("menu.bin load fail\n");
	}
	return ret;
}

/**
 * @fn        UINT8 appADFLoad(void)
 * @brief     appADFLoad
 * @param     NONE
 * @retval	  0:check sum ok ,load ui para from nand
 			  1:check sum err,load default ui para
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appADFLoad(void)
{
	sp1kNandRsvRead(0x0, (K_SDRAM_ShadowAddr << 1) + 0x3000UL);
	
	return appUDFCheckSum();
}

/**
 * @fn        UINT8 appUDFLoad(void)
 * @brief     appUDFLoad
 * @param     NONE
 * @retval	  0:check sum ok ,load ui para from nand
 			  1:check sum err,load default ui para
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appUDFLoad(void)
{
	// sp1kNandRsvRead(0x801, (K_SDRAM_ShadowAddr << 1) + 0x3000UL);
	sp1kNandRsvRead(0x804, (K_SDRAM_ShadowAddr << 1) + 0x3000UL);

	return appUDFCheckSum();
}

/**
 * @fn        void appUDFSave(void)
 * @brief     save User Define Parameters to storage
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void appUDFSave(void)
{
	uiPara_t* tempUiPara;
	//uiPara_t testuipara;
	//uiPara_t* test = &testuipara;
	UINT16 i;

	tempUiPara = appUiParaGet();
	tempUiPara->Checksum = 0;
	tempUiPara->FrameRate = frameRateIsL;

	//add checksum to ui parameters
	//simply add every ui para to checksum
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		(tempUiPara->Checksum) += *(((UINT8 *)tempUiPara + i));
	}

	#if SUPPORT_SPI_WRITE_CONTROL
	mastWriteSpiFlag = 1;
	appSpiWpSet(0);
	#endif

	//save ui parameters to nand
	// sp1kNandRsvWrite(0x801, (K_SDRAM_ShadowAddr << 1) + 0x3000UL);
	sp1kNandRsvWrite(0x804, (K_SDRAM_ShadowAddr << 1) + 0x3000UL);
	

	#if SUPPORT_SPI_WRITE_CONTROL
	appSpiWpSet(1);
	mastWriteSpiFlag = 0;
	#endif
}

/**
 * @fn        void appPowerShutDown_Hdl(void)
 * @brief     appPowerShutDown_Hdl
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-5-24
 * @todo      N/A
 * @bug       N/A
*/
#pragma OT(8, SPEED)
void appPowerShutDown_Hdl(void)
{
	UINT8 status;
	
	ENTER_CRITICAL( status );

	#if 0
	XBYTE[0x2034] |= 0x80;	/* 使能GPIO7输出 */
	XBYTE[0x2030] &= ~0x80;	/* 输出低电平 */
	#endif
	
	while(1) {
		#if 1
		appSetWTDHeartbeat(status);
		sp1kHalWait(118);
		status = !status;
		#endif
	}
}

/**
 * @fn        void appPowerShutDown(void)
 * @brief    shut down power supply
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void appPowerShutDown(void)
{
	//UINT8 status;

	#ifdef NOT_POWER_OFF
	return;
	#endif
	
	printf("Off!\n");
#if 0
	sp1kPWR_HoldCfg();
	sp1kPWR_HoldSet(0);
	ENTER_CRITICAL( status );
	while(1);
#else
	/* Phil: MUST refer to the bank number of the file "app_sts_pwroff.c" */
#ifdef LIB_RELEASE
	HAL_CpuImemSdSet(LOG_BANK_17, 0xC000); 
#else
	HAL_CpuImemSdSet(LOG_BANK_17, 0x8000); 
#endif
	appPowerShutDown_Hdl();
#endif
}

/**
 * @fn        void appPowerOffBGShow(void)
 * @brief     power off background show
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void appPowerOffBGShow(void)
{
	UINT32 srcAdd;
	UINT32 dstAdd;
	UINT8 xShow,yShow;
	UINT8 str[] = "Good Bye";
	UINT8 OsdMaxX, OsdMaxY;
	UINT8 resID = 0x0f;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	xShow=OsdMaxX/2-3;
	yShow=OsdMaxY-1;
	if(appPanelTypeGet()==0){
		yShow=OsdMaxY-1;
	}
	srcAdd=K_SDRAM_Pv2BufAddrD-2*(320L*240L);//mantis #26146
	dstAdd=srcAdd-3*(320L*240L);

	sp1kHalCdspImgSrcSel(1);
	sp1kHalFrontInputGate(0x11);
	sp1kHalCamModeSet(CAM_MODE_IDLE);

	sp1kHalCdspImgSrcSel(1);

	appBGImgDec(resID, NULL, srcAdd, dstAdd, 1);
	sp1kDispImgShow(dstAdd, (UINT16)320, (UINT16)240, 100);

	sp1kOsdClear(0);
	xShow=OsdMaxX/2-4;
	//sp1kOsdStrDraw(xShow, yShow, str, 0);
}

/**
 * @fn        void appPowerOffProc(void)
 * @brief     appPowerOffProc
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void appPowerOffProc(void)
{
	osMsgPost(SP1K_MSG_STATE_TO_POWER_OFF);
}

/**
 * @fn        void appPowerOffPreProc(void)
 * @brief     appPowerOffPreProc
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void
appPowerOffPreProc(
	void
)
{
	sp1kDistDisable(); 
	sp1kFdDisable();
	sp1kAwbDisable();
	sp1kAeSetModeEn(0);
	//here add power off flow
	sp1kTimer1Stop();//add for self snap@wei.sun 20080414
	//printf(" : power off\n");
	//printf("wrt udf\n");
	appUDFSave();
	
	#ifdef NOT_POWER_OFF
	return;
	#endif
	
	appPowerOffBGShow();
	sp1kDispFitShow(320, 240, 1);
	
	sp1kAudioVolumeSet(VOLUME_10);
	//printf("wrt udf END\n");
	//20080327 suny add  start
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_CLOSE, VOICE_PLAYLEVLE_LOW);
	sp1kHalWait(2000);//mantis:27691

	appPowerHoldPullDown();
	
	/* 20080611 mattwang modify, because use new disp drv. */
	sp1kDispPowerOff(sp1kDispDevActGet());	

#if TAG_TODO
	if(AdapterCharge && (sp1kUSB_PlugGet() == USB_PLUG_IN))
	{
		sp1kOsdClear(0);
		appRsmAsPwrOn();
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
		return;
	}
#endif	
}

/**
 * @fn        void appPowerOffInit(void)
 * @brief     appPowerOffInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-3
 * @todo      N/A
 * @bug       N/A
*/
void
appPowerOffInit(
	void
)
{
#if !SUSPEND_TEST
	appPowerOffPreProc();
	osMsgPost(SP1K_MSG_SHUT_DOWN);
#else
	osMsgPost(SP1K_MSG_ENTER_SUSPEND);
#endif
}

/**
 * @fn        void appPowerOff(UINT16 msg)
 * @brief     appPowerOff
 * @param     [in] msg
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
void
appPowerOff(
	UINT16 msg
)
{
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			appPowerOffInit();
			break;
		
		case SP1K_MSG_SHUT_DOWN:
			appPowerShutDown();
			break;
			
		case SP1K_MSG_ENTER_SUSPEND:
			appSuspendResume();
			break;
			
		default:
			break;
	}
}

void appPowerHoldPullDown(void) {
	UINT8 status;
	dbgWdtSet(0);
	
	ENTER_CRITICAL( status );
		
	app_set_lcd_backlight_status(0);
	printf("power hold pull down\n");
	status = 0;
	while (1) {
		#if 0
		gpioByteFuncSet(GPIO_BYTE_GEN0, 0x01 << (7 & 0x07), 0x01 << (7 & 0x07));
		gpioByteDirSet(GPIO_BYTE_GEN0, 0x01 << (7 & 0x07), 0x01 << (7 & 0x07));
		gpioByteOutSet(GPIO_BYTE_GEN0, 0x01 << (7 & 0x07), 0x00 << (7 & 0x07));
		gpioPullSelSet(3, 0x10, 0x00); // gpio 7 pull down
		#else
		appSetWTDHeartbeat(status);
		sp1kHalWait(118);
		status = !status;
		#endif
	}
}

