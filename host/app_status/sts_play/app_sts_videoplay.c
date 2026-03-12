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
#include "solution.h"
#include "os_msg.h"
#include "app_msg.h"
#include "initio.h"
#include "key_scan.h"
#include "app_init.h"
#include "gpio.h"

#include "sp1k_pb_api.h"
#include "sp1k_video_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_disp_api.h"

#include "app_main.h"
#include "app_video_play.h"
#include "app_still_play.h"
#include "app_audio_play.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"

#include "app_ui_osd.h"
#include "app_ui_para.h"
#include "app_audio_rec.h"
#include "app_menu_micro.h"
#include "smcfunc.h"
//#include "dos32.h"
#include "app_menu_tab.h"

//#include "disk.h"
#include "sp1k_gpio_api.h"
#include "app_dev_disp.h"
#include "dbg_def.h"
#include "app_osd_icon.h"
#include "host_func_opt.h"
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
static UINT8 vidPbSpeed=1;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern  SIMAGE xdata pbFile;
extern UINT8 xdata storageReadyflag;
extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge	0:USB contect PC
extern UINT16 LCD_WIDTH;// ((UINT16)320)
extern UINT16 LCD_HEIGHT;// ((UINT16)240)
#if SUPPORT_AUTO_TEST
extern UINT8 autoTestOpenFlag;
// 0 : Key And Usb Test
// 1 : Video Record Test
// 2 : Video Playback Test
extern UINT8 autoTestStatus; 
#endif

UINT8 video_play_timer_start = 0;
UINT8 video_play_timer_out = 0;

#if MSG_AUTO_SEND
UINT8 dele;
#endif
//-----------------------------------------------------------------------------
//appVideoPlay
//-----------------------------------------------------------------------------
void appVideoPlay (UINT16 msg) USING_0
{
	static UINT8 mode=0;
//	powerBatLv_t batlv;
#if (FUNC_HOST_DPS == 1)
	uiPara_t* puiPara = appUiParaGet();
#endif
	switch (msg) {
	case SP1K_MSG_STATE_INIT:
		if (puiPara->Volume <= 0x05) {
				sp1kAudioVolumeSet(VOLUME_7 /*puiPara->Volume*/);
			} else {
				sp1kAudioVolumeSet(0x03);
			}
			
		//if (puiPara->SpeakSound) {
		//	sp1kAudioVolumeSet(VOLUME_10);
		//}
			
		//printf("vid_pb init\n");
		appVideoPlayInit();
		break;
		
    case SP1K_MSG_KEY_PRESS_S1:
		break;
		
    case SP1K_MSG_KEY_PRESS_S2:
		break;
	#if 0
    case SP1K_MSG_KEY_PRESS_MENU:
		//printf("menu\n");
		if(sp1kVideoPbStatusGet() == VID_PB_IDLE) {
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
		} else {
			sp1kVideoPbStop();
			//osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		}
		break;
	#endif
	
	
    case SP1K_MSG_KEY_PRESS_SNAP:
		if (video_play_timer_start && !video_play_timer_out) {
			break;
		}	
	case SP1K_MSG_KEY_PRESS_AF:
		video_play_timer_out = 0;
		video_play_timer_start = 1;
	case SP1K_MSG_MEDIA_PLAY_START:
		#if 0 //defined(SUPPORT_ADJUSTMENT_VOLUME) && SUPPORT_ADJUSTMENT_VOLUME
		// linziyang add for ²¥·ÅÆÆÒô begin
		if (puiPara->Volume <= 0x05) {
			sp1kAudioVolumeSet(VOLUME_10);
		} else {
			sp1kAudioVolumeSet(VOLUME_10);//(0x05);
		}
		// linziyang add for ²¥·ÅÆÆÒô end
		#endif
		appVideoplayOK();

		#if SUPPORT_AUTO_TEST
		if (autoTestOpenFlag) {
			if (2 == autoTestStatus) {
				appButtonAutoTest();
			}
		}
		#endif
		break;
		
    //case SP1K_MSG_KEY_PRESS_DOWN:
	//case SP1K_MSG_KEY_PRESS_MENU:
	//case SP1K_MSG_KEY_PRESS_UP:
    case SP1K_MSG_KEY_PRESS_LEFT:
	case SP1K_MSG_KEY_PRESS_BACK:
		//printf("left\n");
		appVideoPlayRWSpeedSet(appVideoPlaySpeedGet());
		//sp1kVideoPbRWSpeedSet(sp1kVideoPbSpeedGet());
		uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		break;

	case SP1K_MSG_KEY_PRESS_PB:
	case SP1K_MSG_KEY_PRESS_BURST:
	//case SP1K_MSG_KEY_PRESS_AF:
    //case SP1K_MSG_KEY_PRESS_PB:
	//case SP1K_MSG_KEY_PRESS_DOWN:
    case SP1K_MSG_KEY_PRESS_RIGHT:
		//printf("right\n");
		appVideoPlayFWSpeedSet(appVideoPlaySpeedGet());
		//sp1kVideoPbFWSpeedSet(sp1kVideoPbSpeedGet());
		uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		break;
		
    case SP1K_MSG_KEY_PRESS_F1:
		break;
		
    case SP1K_MSG_KEY_PRESS_TELE:
		break;
		
    case SP1K_MSG_KEY_PRESS_WIDE:
		break;
		
    case SP1K_MSG_KEY_PRESS_DISP:
		break;

	case SP1K_MSG_KEY_RELEASE_POWER:
    case SP1K_MSG_KEY_PRESS_MODE:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {
			sp1kVideoPbStop();
			appAutoOffEn(1);
			mode = 1;
		}
		//osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
		break;
		
	case SP1K_MSG_CARD_PLUG_IN:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE)
		{
			sp1kVideoPbStop();
			appAutoOffEn(1);
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_CARD_PLUG_IN);
		break;
		
    case SP1K_MSG_CARD_PLUG_OUT:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE)
		{
			sp1kVideoPbStop();
			appAutoOffEn(1);
		}

		#if SUPPORT_PANEL_PROTECT
		app_set_lcd_backlight_status(1);
		#endif
			
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
		break;
		
    case SP1K_MSG_USB_PLUG_IN:
		if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
		{
			break;
		}
		osMsgPost(SP1K_MSG_STATE_TO_MENU);
		break;
		/*
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {
			sp1kVideoPbStop();
			appAutoOffEn(1);
		}
		
		if (storageReadyflag==SUCCESS && appSDPlugStatusGet()) {
			puiPara->USBMode=USB_MASS;
			
		} else {
			puiPara->USBMode=USB_PCCAM;
		}
		
		osMsgPost(SP1K_MSG_STATE_TO_USB);
		break;
		*/
		
    case SP1K_MSG_USB_PLUG_OUT:
		break;
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE)
			sp1kVideoPbStop();
		osMsgPost(SP1K_MSG_POWER_OFF);
		break;

    case SP1K_MSG_TV_PLUG_IN:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {
			sp1kVideoPbStop();
			appAutoOffEn(1);
			osMsgFlush(MSG_CTRL_FLUSH_SYS);
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_TV_PLUG_IN);
		break;
		
    case SP1K_MSG_TV_PLUG_OUT:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {
			sp1kVideoPbStop();
			appAutoOffEn(1);
			osMsgFlush(MSG_CTRL_FLUSH_SYS);
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_TV_PLUG_OUT);
		break;
		
	case SP1K_MSG_HDMI_PLUG_IN:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {//2008-7-10 mantis #27681 add here to pre stop pb before power off
			sp1kVideoPbStop();
			appAutoOffEn(1);
			uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
		//appDispHdmiOpen();
		break;
		
	case SP1K_MSG_HDMI_PLUG_OUT:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {//2008-7-10 mantis #27681 add here to pre stop pb before power off
			sp1kVideoPbStop();
			appAutoOffEn(1);
			uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_HDMI_PLUG_OUT);
		break;
		
	case SP1K_MSG_BAT_LV_UPDATE://jintao.liu 2008-3-13 add for battery action
		uiUpdateBatteryLevel();
		appBattStsHandle();
		break;
		
	case SP1K_MSG_BAT_EMPTY:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {//2008-7-10 mantis #27681 add here to pre stop pb before power off
			sp1kVideoPbStop();
			appAutoOffEn(1);
			uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		}
		uiUpdateOSDBatEmpty();
		osMsgPost(SP1K_MSG_POWER_OFF);
		sp1kBtnDisableAll();//ready to power off, disable all button!
		break;
		
	case SP1K_MSG_POWER_OFF:
	case SP1K_MSG_KEY_REPEATE_POWER:
		if(sp1kVideoPbStatusGet() != VID_PB_IDLE) {
			sp1kVideoPbStop();
			appAutoOffEn(1);
			uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		}
		appPowerOffProc();
		break;
		
	case SP1K_MSG_MEDIA_PLAY_END:
		appAutoOffEn(1);

		#if SUPPORT_PANEL_PROTECT
		app_set_lcd_backlight_status(1);
		#endif
			
		osMsgFlush(MSG_CTRL_FLUSH_ALL);

		#if SUPPORT_AUTO_TEST
		if (autoTestOpenFlag) {
			if (2 == autoTestStatus) {
				autoTestStatus = 3;
			}
		}
		#endif
			
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());
		break;
		
	#if SUPPORT_PANEL_PROTECT
	case SP1K_MSG_PANEL_PROTECT:
		appPanelAutoProt();
		break;

	case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
		app_set_lcd_backlight_status(1);
		break;
	#endif
	
    default:
		//printf("Unknown message:%d\n", (UINT16)msg);
		break;
	}
	return;
}

void appVideoPlayInit(void) USING_0
{
	//appMediaPlayDispBufSet(sp1kPbDispBuffGet(1));   //xian mask 20090915
	osMsgPost(SP1K_MSG_MEDIA_PLAY_START);

	return;
}

void appVideoplayOK(void) USING_0
{
	UINT8 speed;
	UINT16 x,y;

	speed = sp1kVideoPbSpeedGet();
	if(speed == VID_PB_SPEED_NORMAL) {
		if(sp1kVideoPbStatusGet() == VID_PB_IDLE) {
			sp1kVideoPbStart(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
			if(videoPbStatusGet() == VID_PB_RUN) {
				sp1kOsdIconPsGet(ID_ICON_MODE_PLAYBACK,&x,&y);
				uiUpdateOSDTime(LCD_WIDTH, y, 0, 0);
				appAutoOffEn(0);//jintao.liu 2008-4-29 auto power off
				if(pkgChipIdGet()== 0x1626)
				{
					if((0x20 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in or HDMI plug in
						sp1kHalSpeakerSet(0);
					}
					else {
						sp1kHalSpeakerSet(1);
					}
				}
				else
				{
					if((0x04 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in
						sp1kHalSpeakerSet(0);
					}
					else {
						sp1kHalSpeakerSet(1);
					}
				}
			} else {
				#if SUPPORT_PANEL_PROTECT
				app_set_lcd_backlight_status(1);
				#endif
				
				//show file err
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				return;
			}
		} else {
			appVideoPlayPause();
		}

	} else {
		sp1kVideoPbResume();
	}
	uiUpdateVideoStatus(1, sp1kVideoPbStatusGet());

	return;
}

void appVideoPlayPause(void)
{
	UINT8 sts ;

	sts = sp1kVideoPbStatusGet();
	if(sts == VID_PB_PAUSE) {
		sp1kVideoPbResume();
	} else if(sts == VID_PB_RUN) {
		sp1kVideoPbPause();
	}
	return;
}

UINT8 appVideoPlaySpeedGet(void)
{
	UINT8 sts;

	sts = sp1kVideoPbStatusGet();
	if(sts == VID_PB_IDLE) {
		return APP_VID_PB_SPD_NORMAL;
	}else if(sts == VID_PB_RUN){
		vidPbSpeed = APP_VID_PB_SPD2;
		return vidPbSpeed;
	} else {
		switch(vidPbSpeed) {
			case APP_VID_PB_SPD_NORMAL:
				vidPbSpeed = APP_VID_PB_SPD2;
				break;
			case APP_VID_PB_SPD2:
				vidPbSpeed = APP_VID_PB_SPD4;
				break;
			case APP_VID_PB_SPD4:
				vidPbSpeed = APP_VID_PB_SPD8;
				break;
			case APP_VID_PB_SPD8:
				vidPbSpeed = APP_VID_PB_SPD8;
				break;
			default :
				vidPbSpeed = APP_VID_PB_SPD_NORMAL;
				break;
		}
	}
	return vidPbSpeed;

}

void appVideoPlayFWSpeedSet(UINT8 spd)
{
	UINT8 speed;

	speed = spd;
	if(sp1kVideoPbStatusGet() == VID_PB_RW) {
		speed = APP_VID_PB_SPD2;
	}
	sp1kVideoPbFWSpeedSet(speed);

	return;
}

void appVideoPlayRWSpeedSet(UINT8 spd)
{
	UINT8 speed;

	speed = spd;
	if(sp1kVideoPbStatusGet() == VID_PB_FW) {
		speed = APP_VID_PB_SPD2;
	}
	sp1kVideoPbRWSpeedSet(speed);

	return;
}
