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
#include "general.h"
#include "host_func_opt.h"
#include "initio.h"
#include "solution.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "func_ptr.h"
#include "app_audio_rec.h"
#include "app_init.h"
#include "app_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"

#include "app_ui_osd.h"
#include "app_menu_tab.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_audio_play.h"
#include "app_still_play.h"
#include "sp1k_disp_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_cdsp_api.h"
#include "key_scan.h"
#include "sp1k_disk_api.h"
#include "app_menu_core.h"//6-11
#include "app_menu_event.h"
#include "cardlink.h"//6-11
#include "sp1k_osd_api.h"//jintao.liu 6-13 mantis #25594
#include "App_osd_icon.h"
#include "smcfunc.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "app_dev_disp.h"
#include "Sp1k_ae_api.h"
#include "sp1k_fd_api.h"
#include "sp1k_dist_api.h"
#include "sp1k_awb_api.h"
#include "video_mem.h"
#if !TAG_TODO
#define sp1kHalCamModeSet(a)
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern SIMAGE xdata pbFile;
 
// extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge	 0:USB contect PC
 extern UINT8 xdata  storageReadyflag;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appAudioRec (UINT16 msg) USING_0
{
//	powerBatLv_t batlv;

#if (FUNC_HOST_DPS == 1)
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
#endif

	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			//printf("aud rec init\n");
			appAudioRecInit();
			appAutoOffTimeSet();//jintao.liu 2008-3-28 add for mantis 21604
			break;
		case SP1K_MSG_KEY_PRESS_S1:
			break;
		case SP1K_MSG_KEY_PRESS_S2:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {				
				sp1kAudParamSet(SP1K_AUD_PARAM_ALC_OPT,1);	//ALC enable for mantis#41529 
				appAudioSnap();//For mantis:#28362				
				if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				}
				break;
			}
		case SP1K_MSG_MEDIA_REC_START:
			//printf("snap\n");
			#if(TV_OPTION == TV_PLAY_OPTION)
			if(appPanelTypeGet() == 0) {
				break;
			}
			#endif

			#if (!NAND_SUPPORT)//2008-7-14 mantis #28129
			if(0 == appSDPlugStatusGet())
			{
				uiOsdDialogDisp(ID_STR_NO_CARD_, 1000);
				appAudioOsdInit();
				break;
			}
			#endif

			if(1 == appSDProtectStatusGet())//jintao.liu 2008-5-6 for sd lock
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				appAudioOsdInit();
				break;
			}

			appAudioSnap();
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				appAutoOffEn(0);
			}
			break;
		case SP1K_MSG_MEDIA_VOICE_MEMO_END:
			//printf("\nVoice memo stop\n\n");			
			sp1kAudParamSet(SP1K_AUD_PARAM_ALC_OPT,1);	//ALC enable for mantis#41529
			sp1kTimer1Stop();
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				appAudioSnap();
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			}
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				appAudioSnap();
				if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {//jz.liu, 20081205, #34267,#30488
    				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
                }
				break;
			}

			//if(sp1kAudioRecStatusGet()  == AUD_REC_IDLE) {
				//osMsgPost(SP1K_MSG_STATE_TO_MENU);
			//}
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			if(sp1kAudioRecTypeGet() == AUDIO_TYPE_NORMAL) {
				appAduioRecPause();
			}
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:

			break;
		case SP1K_MSG_KEY_PRESS_LEFT:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				break;
			}
			if(sp1kAudioRecStatusGet()  != AUD_REC_IDLE) {
				//appAudioSnap();
			}
			if(sp1kAudioRecStatusGet()  == AUD_REC_IDLE) {
				osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			}
			if(osMsgSeek(SP1K_MSG_KEY_RELEASE_TELE) == TRUE)
			{
				osMsgFlush(MSG_CTRL_FLUSH_BTN);
			}
			break;
		case SP1K_MSG_KEY_PRESS_RIGHT:
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			
			break;
		case SP1K_MSG_KEY_PRESS_TELE:
			break;
		case SP1K_MSG_KEY_PRESS_WIDE:
			break;
		case SP1K_MSG_KEY_PRESS_DISP:
			break;
		case SP1K_MSG_KEY_PRESS_PB:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				break;
			}
			if(sp1kAudioRecStatusGet()  != AUD_REC_IDLE) {
				appAudioSnap();
			}
			if(sp1kAudioRecStatusGet()  == AUD_REC_IDLE) {
			//sp1kPbDispInit(K_SDRAM_AviPvwBufAddrA, (UINT16)320,(UINT16) 240);
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			}
			break;
		case SP1K_MSG_CARD_PLUG_IN:
			appCardErrFmtErrSet(0);
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kAudioRecStop();
			}
			appStorageMount(K_MEDIA_SDF);
			appAudioOsdInit();
			uiRemainPixInit();
			break;
		case SP1K_MSG_CARD_PLUG_OUT:
			appCardErrFmtErrSet(0);
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kAudioRecStop();
			}
			sp1kBtnEnableAll();
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);//jintao.liu 2008-6-13 mantis #25594
			appStorageMount(K_MEDIA_NANDF);
			appAudioOsdInit();
			uiRemainPixInit();

			//add for mantis #36294@start
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
			{
				osMsgFlush(MSG_CTRL_FLUSH_ALL);
				uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
			}
			//add for mantis #36294@end
			
			break;
		case SP1K_MSG_USB_PLUG_IN:
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kAudioRecStop();
				uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());
				if (storageReadyflag==SUCCESS)
				{
					osMsgPost(SP1K_MSG_USB_PLUG_IN);
					break; //mantis #29148
				}
			}
			
		//	if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
		//	{
		//		break;
		//	}
			if (storageReadyflag==SUCCESS) {
				if (USB_DBG) {
					sp1kHalUSBSwPlugIn();
				} 
#if (FUNC_HOST_DPS == 1)
				else if ( puiPara->USBMode == USB_SIDC){
					osMsgPost(SP1K_MSG_STATE_TO_DPS);
				}
#endif					
				else {
					osMsgPost(SP1K_MSG_STATE_TO_USB);
				}
			}
			break;
		case SP1K_MSG_USB_PLUG_OUT:
			break;
		case SP1K_MSG_TV_PLUG_IN:
#if(TV_OPTION == TV_PLAY_OPTION)
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
#elif(TV_OPTION == TV_FULL_OPTION)
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {//mantis 26708
					sp1kTimer1Stop();
					//appAudioSnap();
					sp1kAudioRecStop();
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
					osMsgPost(SP1K_MSG_TV_PLUG_OUT);
					break;
				}else {
					//appAudioSnap();
					sp1kAudioRecStop();
					uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());
				}
			}
			if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {
				appDispSet();
			}
#endif
			break;
		case SP1K_MSG_TV_PLUG_OUT:
#if(TV_OPTION == TV_PLAY_OPTION)
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_OUT);
#elif(TV_OPTION == TV_FULL_OPTION)
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {//mantis 26708
					sp1kTimer1Stop();
					//appAudioSnap();
					sp1kAudioRecStop();
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
					osMsgPost(SP1K_MSG_TV_PLUG_OUT);
					break;
				}else {
					//appAudioSnap();
					sp1kAudioRecStop();
					uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());
				}
			}
			if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {
				appDispSet();
			}
#endif
			break;
		case SP1K_MSG_HDMI_PLUG_IN:
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kAudioRecStop();
			}
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
			//appDispHdmiOpen();
			break;
		case SP1K_MSG_HDMI_PLUG_OUT:
			appDispSet();			
			break;
		case SP1K_MSG_POWER_OFF:
		case SP1K_MSG_KEY_PRESS_POWER:
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kAudioRecStop();
				sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			}

			if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {
				appPowerOffProc();
			}
			break;
		case SP1K_MSG_MEDIA_REC_DISK_FULL:
			#if 0
			//mantis #23283
			uiUpdateOSDMemFull();
			appAudioRecInit();
			#else
			//show memory full
			if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
				sp1kOsdClear(0);
				if(sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE){
					sp1kTimer1Stop();
					osMsgPost(SP1K_MSG_MEDIA_VOICE_MEMO_END);
					return;
				} else{
					sp1kAudioRecStop();
					uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());					
				}
			}
			uiUpdateOSDMemFull();
			osMsgPost(SP1K_MSG_STATE_INIT);
			#endif
			break;
		case SP1K_MSG_MEDIA_REC_OK:
			//update osd
			appAutoOffEn(1);
			appAudioOsdInit();
			uiRemainPixInit();
			break;
#if 1
		case SP1K_MSG_BAT_LV_UPDATE://jintao.liu 2008-3-13 add for battery action
			uiUpdateBatteryLevel();
			appBattStsHandle();
			break;
		case SP1K_MSG_BAT_EMPTY:
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			uiUpdateOSDBatEmpty();
			osMsgPost(SP1K_MSG_POWER_OFF);
			sp1kBtnDisableAll();//ready to power off, disable all button!
			break;
#else
		case SP1K_MSG_BAT_LV_UPDATE:
			uiUpdateBatteryLevel();
			appBatLvGet(&batlv);
			if (batlv == POWER_BAT_BELOWEMPTY)//battery empty flow
			{
				if(sp1kAudioRecStatusGet() != AUD_REC_IDLE) {
					sp1kAudioRecStop();
				}//to avoid mantis #25758 stop recording before show "low batt"

				uiUpdateOSDBatEmpty();
				//sp1kHalGPIOBitSet(PWR_ON_EN, 0);//power off
				osMsgPost(SP1K_MSG_POWER_OFF);
			}
#endif
			break;
		case SP1K_MSG_NAND_ERROR:
			uiOsdDialogDisp(ID_STR_CARD_ERROR, 0);
			break;

		case SP1K_MSG_CARD_ERROR:
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			if(sp1kDiskErrorCodeGet() == DISK_ERR_DETECT)
			{
				uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD,1000);
			}
			else
			{
				uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
				appCardErrFmtSta(1);
			}
			break;

		case SP1K_MSG_CARD_NOT_SUPPORT:
			uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
			sp1kBtnDisableAll();
			break;

		#if SUPPORT_PANEL_PROTECT
		case SP1K_MSG_PANEL_PROTECT:
			appPanelAutoProt();
			break;

		case SP1K_MSG_SYS_CLOSE_LCD_BACKLIGHT:
		  	app_set_lcd_backlight_status(0);
		  	break;
	  
		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(1);
		  	break;
	  	#endif

	    default:
			//printf("Unknown message:%d\n", (UINT16)msg);
			break;
	}
}

void appAudioRecInit(void) USING_0
{
	/* power control -- audio rec */
	//appModePowerCtrl(MODE_AUDIOREC);
	/*fix mantis 42596 S*/
	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbDisable();
	sp1kFdDisable();
	sp1kDistDisable();
	/*fix mantis 42596 E*/
	sp1kAudioRecFreqSet(RECORD_FREQUENCE_1KHZ);
	sp1kAudioRecFmtSet(AUD_FMT);
	if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
		sp1kAudParamSet(SP1K_AUD_PARAM_ALC_OPT,0);	//ALC disable for mantis#41529
		osMsgPost(SP1K_MSG_MEDIA_REC_START);
		sp1kTimer1Config(30000, pFunc_VoiceMemoStop, NULL);
		sp1kTimer1Start();
	}
	else {
		sp1kDispImgWinEn(0); 
		appAudioBgShow(K_SDRAM_AviCapVLCBufAddrA);
		sp1kDispImgWinEn(1); 
	}
	appAudioOsdInit();	 //lryy
}

void appAduioRecPause(void) USING_0
{
	UINT8 sts;
	UINT32 time;

	sts = sp1kAudioRecStatusGet();

	if(sts == AUD_REC_RUN) {
		time = sp1kAudioRecTimeGet();
		if(time < 1000) {
			return;
		}
		sp1kAudioRecPause();
	} else if(sts == AUD_REC_PAUSE) {
		sp1kAudioRecResume();
	}
	uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());

	return;
}

extern void appVoiceMemoUI(void);
void appAudioSnap(void)USING_0
{
	UINT32 time;
	uiPara_t* puiPara;

	puiPara = appUiParaGet();
	if(sp1kAudioRecStatusGet() == AUD_REC_IDLE) {
		if ( appCardErrFmtErrChk() ) {
			uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
			appAudioRecInit();
			return;
		}
		else {
			uiUpdateAudRecRemainTime(&time);
			if(time ==0) {
				osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
				return;
			}
		}
		if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
			sp1kImageVoiRecSet(sp1kPbFreeBuffGet(), sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
            		sp1kImageVoiRecImgNameSet(pbFile.FileName);//xian ++ 20091010, #39169
		}
#if 0//def MULTI_FOLDER_AS_ONE  //xian ++ 20080806
        else if(USR_LastFolder(0) == SUCCESS) //go to the last folder
            DOS_FileOrderTblInit();
#endif        
		sp1kAudioVolumeSet(0x01);//volume gain up 1x
		sp1kAudioRecFmtSet(AUD_FMT);
		sp1kAudioRecFreqSet(RECORD_FREQUENCE_1KHZ);
		sp1kAudioRecStart();
		/* power control -- audio recording */
		//appModePowerCtrl(MODE_AUDIOREC);
	}
	else {
		//sp1kHalReadGTimer(&time);
		time = sp1kAudioRecTimeGet();
		if(time < 1000) {
			return;
		}
		sp1kAudioRecStop();		
		/*recover the Volume pre-seting */
		sp1kAudioVolumeSet(puiPara->Volume);
		//uiUpdateAudRecTime(0);
	}
	if(sp1kAudioRecTypeGet() == AUDIO_TYPE_NORMAL) {
		uiUpdateAudioStatus(0, sp1kAudioRecStatusGet());
	}
	else {
		appVoiceMemoUI();
	}
}

void appAudioBgShow(UINT32 addr) USING_0
{
	UINT8 sts = 0;
	UINT32 srcAddr = K_SDRAM_TotalSize - 0x100000;
#if 1
	sp1kHalCdspImgSrcSel(1);
	sp1kHalFrontInputGate(0x11);
	sp1kHalCamModeSet(CAM_MODE_IDLE);
	sp1kHalCdspImgSrcSel(1);
	appBGImgDec(0x15, NULL, srcAddr, addr, 1);
	sp1kDispImgShow(addr, (UINT16)320, (UINT16)240, 100);

#else
	sp1kHalCdspImgSrcSel(1);
	appBGImgDec(0x15, NULL, srcAddr, addr, 1);
	sp1kHalDispScaleSet(320, 240, 100);
	sp1kPbBufHsizeSet(320);
	sp1kDispVvalidWait(SP1K_DISP_SIGNAL_LO);
	sp1kDispVvalidWait(SP1K_DISP_SIGNAL_HI);
	sp1kHalDramDispSrcSel(0);
	sp1kPbBufAddrSet(addr);
	sp1kHalGprmCpy(addr,320,240, 0, 0,
	K_SDRAM_AviPvwBufAddrD, 320, 240, 0, 0, 320, 240, 0,1);
	sp1kDispVvalidWait(SP1K_DISP_SIGNAL_LO);
	sp1kDispVvalidWait(SP1K_DISP_SIGNAL_HI);
	sp1kPbBufAddrSet(K_SDRAM_AviPvwBufAddrD);
#endif
}

