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
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_init.h"
#include "initio.h"
#include "app_multi_play.h"
#include "app_still_play.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"

#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "sp1k_pb_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "app_ui_osd.h"
#include "sp1k_osd_api.h"
#include "sp1k_aud_api.h"
#include "app_menu_tab.h"
#include "key_scan.h"
#include "smcfunc.h"
#include "app_pwr_mgr.h"
#include "disp_api.h"
#include "sp1k_disk_api.h"
//#include "dos32.h"
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
UINT8 xdata cardplug;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern SIMAGE xdata pbFile;
extern UINT8 xdata  storageReadyflag;
//extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC

//-----------------------------------------------------------------------------
//appMultiPlay
//-----------------------------------------------------------------------------
extern void uiUpdateOSDMultiPbFileNum(void);

void appMultiPlay (UINT16 msg)
{
	UINT8 InitMode;
//	powerBatLv_t batlv;
#if (FUNC_HOST_DPS == 1)
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
#endif
	
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			if (!sp1kPbFileCntGet()) {
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				break;
			}
			if (appPreStateGet(0) == APP_STATE_MENU) {
				InitMode = SP1K_PB_INIT_SMART;
			} else {
				InitMode = SP1K_PB_INIT_FULL;
			}
			appMultiPlayInit(InitMode);
			appAutoOffTimeSet();//jintao.liu 2008-3-28 add for mantis 21604
			break;
	    	case SP1K_MSG_KEY_PRESS_S1:
			break;
	    	case SP1K_MSG_KEY_PRESS_S2:
			break;
			#if 0
	    	case SP1K_MSG_KEY_PRESS_MENU:
			if(dispDevActGet() != 4) {//HDMI do not support menu 
				osMsgPost(SP1K_MSG_STATE_TO_MENU);
			}
			break;
			#endif
	    	case SP1K_MSG_KEY_PRESS_UP:
			appMultiPlayShift(PB_SHIFT_UP);
			break;
	    	//case SP1K_MSG_KEY_PRESS_DOWN:
	    	case SP1K_MSG_KEY_PRESS_PB:
			appMultiPlayShift(PB_SHIFT_DOWN);
			break;
			
	    	//case SP1K_MSG_KEY_PRESS_MENU:
	    	case SP1K_MSG_KEY_PRESS_DOWN:
	    	case SP1K_MSG_KEY_PRESS_LEFT:
			appMultiPlayShift(PB_SHIFT_LEFT);
			break;
			
	    	//case SP1K_MSG_KEY_PRESS_PB:
	    	case SP1K_MSG_KEY_PRESS_BURST:
	    	case SP1K_MSG_KEY_PRESS_RIGHT:
			appMultiPlayShift(PB_SHIFT_RIGHT);
			break;
		case SP1K_MSG_TP_UP:
			appMultiPlayShift(PB_SHIFT_PAGE_UP);
			break;
		case SP1K_MSG_TP_DOWN:
			appMultiPlayShift(PB_SHIFT_PAGE_DOWN);
			break;
	    	case SP1K_MSG_KEY_PRESS_SNAP:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);			
			break;
	    	case SP1K_MSG_KEY_PRESS_TELE:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);	
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
			break;
	    	case SP1K_MSG_KEY_PRESS_WIDE:
			break;
	    	case SP1K_MSG_KEY_PRESS_DISP:
			break;
	    	case SP1K_MSG_KEY_PRESS_F1:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
			
	    	case SP1K_MSG_KEY_PRESS_MODE:
	    	//case SP1K_MSG_KEY_PRESS_PB:
			//printf("PB\n");
			//suny add for pb mode -> other mode start
#if(TV_OPTION == TV_PLAY_OPTION) 
			if(appPanelTypeGet() == 0) {
				return;
			}
#endif
			if(dispDevActGet() == 4) {//HDMI in
				break;
			}
			switch (appPbPreStateGet())
			{
				case APP_STATE_STILL_VIEW:
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;
					break;	
				case APP_STATE_VIDEO_VIEW:
					msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
					break;
		
				case APP_STATE_AUDIO_REC:
					sp1kAudioRecTypeSet(AUDIO_TYPE_NORMAL);
					msg = SP1K_MSG_STATE_TO_AUDIO_REC;
					break;
				default:
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;
					break;
			}
#if(TV_OPTION == TV_PLAY_OPTION) 
				if(appPanelTypeGet() == 0) {
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);	
				} else {
					osMsgPost(msg);
				}
#else
				osMsgPost(msg);
#endif
			//suny add for pb mode -> other mode end
			break;
			/*jintao.liu 2008-3-28 add for mantis 22007*/
		case SP1K_MSG_CARD_PLUG_IN:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_CARD_PLUG_IN);
			break;
		case SP1K_MSG_CARD_PLUG_OUT:
			cardplug = 1 ;
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
			break;
	    case SP1K_MSG_USB_PLUG_IN:
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(msg);
			break;

			/*
		//	if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
		//	{
		//		break;
			}
			if (storageReadyflag==SUCCESS) {
				if (USB_DBG) {
					sp1kHalUSBSwPlugIn();
				}
			#if (FUNC_HOST_DPS == 1)
				else if ( puiPara->USBMode == USB_SIDC) {
					osMsgPost(SP1K_MSG_STATE_TO_DPS);
				} 
			#endif					
				else {
					osMsgPost(SP1K_MSG_STATE_TO_USB);
				}				
			}
			break;
			*/
	    	case SP1K_MSG_USB_PLUG_OUT:
			break;
	    	case SP1K_MSG_TV_PLUG_IN:
	    	case SP1K_MSG_TV_PLUG_OUT:
			appDispSet();
			break;
		case SP1K_MSG_HDMI_PLUG_IN:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
			//appDispHdmiOpen();
			break;
		case SP1K_MSG_HDMI_PLUG_OUT:
			appDispSet();			
			break;
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

		case SP1K_MSG_POWER_OFF:
		//case SP1K_MSG_KEY_PRESS_POWER:
		case SP1K_MSG_KEY_REPEATE_POWER:
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			appPowerOffProc();
			break;
		case SP1K_MSG_NAND_ERROR:
		case SP1K_MSG_CARD_ERROR:
			if(sp1kDiskErrorCodeGet() == DISK_ERR_DETECT)
			{
				uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD,1000);
			}
			else
			{
				uiOsdDialogDisp(ID_STR_CARD_ERROR, 0);
			}
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
}

UINT8 appMultiPlayInit(UINT8 InitMode)
{
	sp1kOsdClear(0); //jintao.liu 2008-5-22 mantis 25063
	if (InitMode == SP1K_PB_INIT_SMART) 
	{
		sp1kPbMultiPlayObjFrameDraw(pbFile.dwIndex, 1);
		uiUpdateOSDMultiPbFileNum();
		return SUCCESS;		
	}
	//Multi play layout setting
	if(1==dispDevActGet()){//LCD
		sp1kPbParamSet(PB_PARAM_DISP_BUF_W,320);
		sp1kPbParamSet(PB_PARAM_DISP_BUF_H,240);	
		#if FUNC_HOST_TOUCH_PANEL
		sp1kPbParamSet(PB_PARAM_THUMB_START_X_Y,0x100C);
		sp1kPbParamSet(PB_PARAM_THUMB_W_H,0x5844);
		#else
		sp1kPbParamSet(PB_PARAM_THUMB_START_X_Y,0x080c);
		sp1kPbParamSet(PB_PARAM_THUMB_W_H,0x5f44);
		#endif
	}else if(3==dispDevActGet()){//TV PAL
		sp1kPbParamSet(PB_PARAM_DISP_BUF_W,720);
		sp1kPbParamSet(PB_PARAM_DISP_BUF_H,576);	
		sp1kPbParamSet(PB_PARAM_THUMB_START_X_Y,0x0c40);	
		sp1kPbParamSet(PB_PARAM_THUMB_W_H,0xA078);	
	}else if(2==dispDevActGet()){//TV NTSC 
		
		sp1kPbParamSet(PB_PARAM_DISP_BUF_W,720);
		sp1kPbParamSet(PB_PARAM_DISP_BUF_H,480);	
		sp1kPbParamSet(PB_PARAM_THUMB_START_X_Y,0x1214);
		sp1kPbParamSet(PB_PARAM_THUMB_W_H,0xA078);	
	}
	else{// HDMI
		sp1kPbParamSet(PB_PARAM_DISP_BUF_W,960);
		sp1kPbParamSet(PB_PARAM_DISP_BUF_H,720);	
		sp1kPbParamSet(PB_PARAM_THUMB_START_X_Y,0x0c0c);
		sp1kPbParamSet(PB_PARAM_THUMB_W_H,0xf0b4);	
	}

	sp1kPbMultiPlayLayoutInit(PB_MULTIPLAY_LAYOUT_9, pbFile.dwIndex);
	sp1kPbMultiPlay();
	
	uiUpdateOSDMultiPbFileNum();
	appMultiOsdDirectionShow();
	appMultiGuiInit();
	appUiCoverTVOsd();
	
	return SUCCESS;
}


UINT8 appMultiPlayShift(UINT8 shift)
{
	UINT8 sts;
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	sp1kBtnDisableAll();
	sts = sp1kPbMultiPlayShift(&pbFile.dwIndex, shift); 
	appPbFileInfoUpdate();
	sp1kBtnEnableAll();
	uiUpdateOSDMultiPbFileNum();
	appMultiOsdDirectionShow();
	sp1kOsdDispBuffShow();
	if(sts==FAIL)
	{
		sp1kPbMultiPlayLayoutInit(PB_MULTIPLAY_LAYOUT_9, pbFile.dwIndex);
		sp1kPbMultiPlay();
		appMultiGuiInit();
	}
	return SUCCESS;	
}
