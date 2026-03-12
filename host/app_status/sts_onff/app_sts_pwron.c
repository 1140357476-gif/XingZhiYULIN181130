/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
/**
 * @file		app_power_on.c
 * @brief		App for power on state
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#include "general.h"
#include "initio.h"
#include "os_msg.h"
#include "app_msg.h"
#include "key_scan.h"
#include "os_custom.h"
#include "gpio.h"

#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_pv_api.h"
#include "sp1k_util_api.h"

#include "app_main.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_pwr_mgr.h"
#include "app_ui_osd.h"
#include "app_still_view.h"
#include "app_ui_para.h"//jintao.liu 2008-5-7 add for EV
#include "app_power_on.h"

#include "app_osd_icon.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "func_ptr.h"
#include "os_api.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "smcfunc.h"
#include "sp1k_gpio_api.h"
#include "sp1k_cdsp_api.h"
#include "front_logi.h"
#include "hal_front.h"
#include "hal_dram.h"
#include "app_menu_micro.h"
#include "sp1k_fd_api.h"
#include "sp1k_calibration_api.h"
#include "dbg_def.h"
#include "lsc_cal.h"
#include "sp1k_snap_api.h"
#include "sp1k_util_api.h"
#include "app_init.h"
#include "app_video_view.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define G_HSize_P K_Preview_Width
#define G_VSize_P K_Preview_Height
#define SD_MOUNT_SPEEDUP 1
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
static UINT8 startFlag = 0;
UINT8 speedSdOpen = 0;
UINT8 power_up_usb_check_flag = 0;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern appPvZoom_t xdata PvZoomCtrl;
extern UINT8 xdata bUsbPwr;
extern xdata UINT8 G_calibrationMode;
extern xdata UINT8 calibrationAEOn;
extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
#if SUPPORT_AUTO_TEST
extern UINT8 autoTestOpenFlag;
#endif

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//-----------------------------------------------------------------------------
static void appPowerOnInit(void);

extern UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr);
void appCardUpdateFW(void);
static UINT8 appPowerOnHdmiInChk(UINT8 cardType);
extern UINT8 sd_IdentificationCmd(UINT8 mmcMode, UINT8 cardType, UINT16 BlockSize);
extern void sd_PwrOnMountSpeedupEn(UINT8 en);
//-----------------------------------------------------------------------------
//appPowerOn
//-----------------------------------------------------------------------------
/**
 * @brief		appPowerOn
 * @param	msg
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appPowerOn(UINT16 msg)
{
	UINT8 rdy = 0, awbrdy = 0; //Fast ae poweron
	//UINT8 aeCnt;  //cleaer warning
	uiPara_t* puiPara = appUiParaGet();
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			appCardUpdateFW();
			appPowerOnInit();
			appSceneSet(puiPara->SceneMode);
			power_up_usb_check_flag = 1;
			break;
		case SP1K_MSG_KEY_PRESS_S1:
			break;
		case SP1K_MSG_KEY_PRESS_S2:
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			break;
		case SP1K_MSG_KEY_PRESS_LEFT:
			break;
		case SP1K_MSG_KEY_PRESS_RIGHT:
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			break;
		case SP1K_MSG_KEY_PRESS_TELE:
			break;
		case SP1K_MSG_KEY_REPEATE_TELE:
			break;
		case SP1K_MSG_KEY_PRESS_WIDE:
			break;
		case SP1K_MSG_KEY_REPEATE_WIDE:
			break;
		case SP1K_MSG_KEY_RELEASE_TELE:
			break;
		case SP1K_MSG_KEY_RELEASE_WIDE:
			break;
		case SP1K_MSG_KEY_PRESS_DISP:
			break;
		case SP1K_MSG_KEY_PRESS_F1:
			break;
		case SP1K_MSG_KEY_PRESS_F2:
			break;
		case SP1K_MSG_KEY_PRESS_PB:
			break;
		case SP1K_MSG_KEY_PRESS_POWER:
			break;
		case SP1K_MSG_TV_PLUG_IN:
			osMsgPost(SP1K_MSG_TV_PLUG_IN);  // keep resending itself when in power on state.
			break;
		case SP1K_MSG_TV_PLUG_OUT:
			break;
		case SP1K_MSG_USB_PLUG_IN:
			osMsgPost(SP1K_MSG_USB_PLUG_IN);  // keep resending itself when in power on state.
			break;
		case SP1K_MSG_USB_PLUG_OUT:
			break;
		case SP1K_MSG_POWER_OFF:
			sp1kBtnDisableAll();//ready to power off, disable all button!
			appPowerOffProc();
			break;
		case SP1K_MSG_KEY_PRESS_SELFSNAP:
			break;
		case SP1K_MSG_BAT_LV_UPDATE:
			break;
		case SP1K_MSG_CARD_PLUG_IN:
			/*
			if( !( sp1kAeReadyGet(startFlag ) == SUCCESS && startFlag == 1 ) ) {
				osMsgPost(SP1K_MSG_CARD_PLUG_IN);
				//sp1kAwbScaleSet(SP1K_AWB_Jump_Scale, 255);
				startFlag = 1;
				break;
			}
			*/
			if(sp1kAeReadyGet(7)){
				osMsgPost(SP1K_MSG_CARD_PLUG_IN);
				break;
			}
			#if SD_MOUNT_SPEEDUP
			appSdPowerRecycle();
			sd_IdentificationCmd(0,K_MEDIA_SDF,0x200);
			sd_PwrOnMountSpeedupEn(0);
			speedSdOpen = 1;

			#else
			speedSdOpen = 0;
			sd_PwrOnMountSpeedupEn(0);
			#endif

			startFlag = 1;

			//printf("ae_rdy = %bu, aecnt = %bu, awb_rdy = %bu \n", rdy, aeCnt, awbrdy);
			//if SD eject msg in msg buffer, flush it!
			osMsgFlush(MSG_CTRL_FLUSH_ALL);//mantis #41350
			if(appPowerOnHdmiInChk(K_MEDIA_SDF))return;
			printf("Line:%d\n",(UINT16)__LINE__);
			#if(TV_OPTION == TV_PLAY_OPTION)
				if(appPanelTypeGet() == 1){
					sp1kPrevewUnfreeze(1);
					appStillOsdInit(1);
					osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
				}else {
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				}
			#else
				/*2008-6-20 add for power on usb   mantis #26563*/
				sp1kPrevewUnfreeze(1);
				dbgTimerStop();// dbg from pwr on to preview
				//appStillOsdInit(1);
				// appStorageMount(K_MEDIA_SDF);
				//uiUpdateRemainPix();
				//uiCardIcon();//2008-6-24 mantis #26179
				#if 0
				if(!sp1kUSB_PlugGet() || bUsbPwr)
				{
					sp1kPrevewUnfreeze(1);
					appStillOsdInit();
					appStorageMount(K_MEDIA_SDF);
					uiUpdateRemainPix();
					uiCardIcon();//2008-6-24 mantis #26179
				}
				else{
					appStorageMount(K_MEDIA_SDF);
				}
				#endif
				/*2008-6-20 add for power on usb end   mantis #26563*/

				#if SUPPORT_AUTO_TEST
				if (autoTestOpenFlag) {
					osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
				} else {
					osMsgPost(STATE_DEFAULT);
				}
				#else
				osMsgPost(STATE_DEFAULT);
				#endif
				//sp1kBtnEnableAll();//enable the button when status destroyed
			#endif

			#ifdef MI_LSC_CAL_OPTION
			if(appGetMILSCCalSts()){
				appMILSCCalSet();
			}
			#endif
			
			// sp1kCardResourceUpdate();
			//sp1kLoadDateStampData();//2008-5-22 mantis #23702

			if(appSDPlugStatusGet())
			{
				osMsgPost(SP1K_MSG_CARD_PLUG_IN);
			}
			break;

		case SP1K_MSG_CARD_PLUG_OUT:
			/*if( !( sp1kAeReadyGet( 0, startFlag ) == SUCCESS && startFlag == 1 ) ) {
				osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
				//sp1kAwbScaleSet(SP1K_AWB_Jump_Scale, 255);

				startFlag = 1;

				break;
			}*/
			if(sp1kAeReadyGet(7)){
				osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
				break;
			}
			startFlag = 1;
			osMsgFlush(MSG_CTRL_FLUSH_ALL);//mantis #41350
			if(appPowerOnHdmiInChk(K_MEDIA_NANDF)) {
				sp1kAeSetModeEn(AE_DISABLE);
				sp1kAwbDisable();
				printf("Line:%d\n",(UINT16)__LINE__);
				return;
			}
			#if(TV_OPTION == TV_PLAY_OPTION)
				if(appPanelTypeGet() == 1){
					sp1kPrevewUnfreeze(1);
					appStillOsdInit(1);
					osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
				}
				else {
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				}
			#else
			/*
				If the unit power on...

				||. without USB plug in
				||. with USB power mode

				then unfree preview and do storage initialization process
			*/

			sp1kPrevewUnfreeze(1);
			dbgTimerStop();// dbg from pwr on to preview
			//appStillOsdInit(1);
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==TRUE))
			{
				appStorageMount(K_MEDIA_NANDF);
			}
			//uiRemainPixInit();
			//uiUpdateRemainPix();

			#if 0
			if(!sp1kUSB_PlugGet() || bUsbPwr)
			{
				sp1kPrevewUnfreeze(1);
				appStillOsdInit(1);
				appStorageMount(K_MEDIA_NANDF);
				uiRemainPixInit();
				uiUpdateRemainPix();
			}
			else{
				appStorageMount(K_MEDIA_NANDF);
			}
			#endif

#endif
			osMsgPost(STATE_DEFAULT);
		#ifdef MI_LSC_CAL_OPTION
			if(appGetMILSCCalSts()){
				appMILSCCalSet();
			}
		#endif
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
			{
				osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
			}

			//sp1kBtnEnableAll();//enable the button when status destroyed
			//sp1kLoadDateStampData();//jintao.liu 2008-5-22 mantis #23702
			break;

		case SP1K_MSG_NAND_ERROR:
			//Note : 这个msg只可能在SP1K_MSG_CARD_PLUG_IN中产生
			//在appStorageMount()中产生.
			//接下来的一句只是为了将消息传递到still view模式中去
			osMsgPost(SP1K_MSG_NAND_ERROR);
			break;
		case SP1K_MSG_CARD_ERROR:
			osMsgPost(SP1K_MSG_CARD_ERROR);
			break;
		default:
			//printf("Unknown message:0x%bx\n", msg);
			break;
	}
}

static void appPowerOnInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT16 frontHSize,frontVSize;
	UINT16 cropVSize;
	UINT16 cropHsize;

	appPreviewCdspDispResSet();

	sp1kBtnDisableAll();//mantis #28354

	//sp1kAeSetModeEn(AE_DISABLE);
	//sp1kAwbDisable();

	appElecFreqSet(); /*For badpixel fix !DON'T MODIFY!*/
	#if 1//TAG_TODO
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
		if(frontHSize*3 > frontVSize*4)//16:9
		{
			cropHsize=frontVSize*4/3;
			cropVSize=frontVSize;
		}
		else
		{
			cropHsize=frontHSize;
			cropVSize=frontVSize;
		}
		sp1kPreviewCropSizeSet(cropHsize,cropVSize);

		sp1kPreviewSet(0, G_HSize_P, G_VSize_P, sp1kPvZoomFctGet(), 1);

		//sp1kPvZoomSet(1, PvZoomCtrl.factor, 0);

		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_CDSP, 300);	/*will: avoid 2nd path to write error data into dram*/

		sp1k2ndPathCfg(1);/* turn off 2nd frame buffer until all the zoom factor has been updated successfully*/


		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 1);
	#endif

	sp1kAeSetModeEn(AE_ENABLE);
	//sp1kAwbReset();
	sp1kAwbEnable();
	sp1kAwbParamSet(AWB_PARA_RunIdx, 20);
	appEVSet(puiPara->AEMode);

	if(puiPara->FDMode == FDMODE_ON)
	{
		if(!sp1kFdGetStatus())
		{
			sp1kFdReset();
			sp1kFdEnable();
		}

		if(puiPara->SmileMode == SMILEMODE_ON)
		{
			sp1kFdSetSmile(1);
			sp1kFdParamSet(FD_PARAM_ShowMode, 1);
		}
		else
		{
			sp1kFdSetSmile(0);
			sp1kFdParamSet(FD_PARAM_ShowMode, 0);
		}
	}
	else
	{
		sp1kFdSetSmile(0);
		sp1kFdDisable();
	}

	if(puiPara->BlinkMode == BLINKMODE_ON)
	{
		sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,1);
	}
	else
	{
		sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,0);
	}
/*	if(puiPara->WDRMode == WDRMODE_ON)
	{
		sp1kCalibrationWDRSet(1);
	}
	else
	{
		sp1kCalibrationWDRSet(0);
	}
*/


	//appElecFreqSet();

	appSnapParaInit();

	appAutoOffTimeSet();
	appIconPsTabLoad(ICON_MODE_PV);
}

static UINT8 appPowerOnHdmiInChk(UINT8 cardType)
{
	UINT8 hdmiOut= 0;

	if(sp1kHDMI_PlugGet() == hdmiOut)return 0;


	appStorageMount(cardType);
	sp1kHalCtrlScreen(0);
	osMsgFlush(MSG_CTRL_FLUSH_ALL);
	appDispHdmiOpen();
	osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
	if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE)){
		osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
	}
	//sp1kLoadDateStampData();

	return 1;
}

