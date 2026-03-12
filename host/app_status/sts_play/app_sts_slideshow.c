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
#include "os_msg.h"
#include "app_msg.h"
#include "func_ptr.h"
#include "pb_api.h"
#include "key_scan.h"

#include "sp1k_util_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_disp_api.h"

#include "app_main.h"
#include "app_slide_show.h"
#include "app_still_play.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_pwr_mgr.h"
#include "app_battery.h"
#include "app_ui_osd.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_dev_disp.h"
#include "sp1k_hal_api.h"

/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 extern UINT8 SlideStopFlag;//add for #37067
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge	0:USB contect PC

void appSlideShowInit(void);
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//-----------------------------------------------------------------------------
//appSlideShow
//-----------------------------------------------------------------------------

void appSlideShow(UINT16 msg)
{
//	powerBatLv_t batlv;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
		//	printf("Start to slide show\n");
			appSlideShowInit();
			osMsgPost(SP1K_MSG_SLIDE_SHOW_START);
			break;
	    	case SP1K_MSG_SLIDE_SHOW_START:
			pbSlideShowStart();
			uiUpdateSlideShowState(0);//show osd
			uiUpdateOSDPbFileNum();
			break;
	    	case SP1K_MSG_SLIDE_SHOW_NEXT:
			sp1kHalSpeakerSet(0); //mantis bug 41677 	
			pbSlideShowProc(&pbFile.dwIndex);
			//uiUpdateOSDPbFileNum();
			break;
	    	case SP1K_MSG_SLIDE_SHOW_STOP:
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_SLIDE_SHOW_FINISH);
			break;
		case SP1K_MSG_SLIDE_SHOW_FINISH:
		//	printf("Slide show finished\n");
			SlideStopFlag=1;//add for #37067

			#if SUPPORT_PANEL_PROTECT
			app_set_lcd_backlight_status(1);
			#endif
			
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
	    	case SP1K_MSG_KEY_PRESS_S1:
	    	case SP1K_MSG_KEY_PRESS_S2:
			break;	
	    	case SP1K_MSG_KEY_PRESS_MENU:
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
	    	//case SP1K_MSG_KEY_PRESS_UP://will modify to stop function
	    	case SP1K_MSG_KEY_PRESS_DOWN:
	    	case SP1K_MSG_KEY_PRESS_LEFT:
	    	case SP1K_MSG_KEY_PRESS_RIGHT:
	    	case SP1K_MSG_KEY_PRESS_SNAP:
	    	case SP1K_MSG_KEY_PRESS_TELE:
	    	case SP1K_MSG_KEY_PRESS_WIDE:
	    	case SP1K_MSG_KEY_PRESS_DISP:
		case SP1K_MSG_KEY_PRESS_PB:
		//	printf("abort slide show\n");
			//osMsgPost(SP1K_MSG_SLIDE_SHOW_STOP);
			break;
		case SP1K_MSG_KEY_PRESS_UP://jintao.liu 	2008-6-19
			if ( SLIDE_PLAY == pbSlideShowStateGet() )
			{
				pbSlideShowStop();
				uiUpdateSlideShowState(1);
			}
			else
			{				
				pbSlideShowContinue();
				uiUpdateSlideShowState(0);
			}
			break;
	    	case SP1K_MSG_CARD_PLUG_IN:
			pbSlideShowAbort();

			#if SUPPORT_PANEL_PROTECT
			app_set_lcd_backlight_status(1);
			#endif

			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_CARD_PLUG_IN);
			break;
	    	case SP1K_MSG_CARD_PLUG_OUT:
			pbSlideShowAbort();

			#if SUPPORT_PANEL_PROTECT
			app_set_lcd_backlight_status(1);
			#endif
			
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
			break;
	    	case SP1K_MSG_USB_PLUG_IN:
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			
		//	if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
		//	{
		//		break;
		//	}
			osMsgPost(SP1K_MSG_USB_PLUG_IN);
			break;
	    	case SP1K_MSG_USB_PLUG_OUT:
			pbSlideShowAbort();

			#if SUPPORT_PANEL_PROTECT
			app_set_lcd_backlight_status(1);
			#endif
			
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_USB_PLUG_OUT);
			break;
	    	case SP1K_MSG_TV_PLUG_IN:
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
			break;
	    	case SP1K_MSG_TV_PLUG_OUT:
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_OUT);
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
			uiUpdateOSDBatEmpty();
			osMsgPost(SP1K_MSG_POWER_OFF);
			sp1kBtnDisableAll();//ready to power off, disable all button!
			break;

		case SP1K_MSG_POWER_OFF:
		case SP1K_MSG_KEY_PRESS_POWER:
			pbSlideShowAbort();
			appPowerOffProc();
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

void appSlideShowInit(void)
{
	uiPara_t* puiPara;
	UINT16 SlideShowTime;
	UINT16 SlideShowRpt;
/*	
	if(++pbFile.dwIndex > sp1kPbFileCntGet()) {
		pbFile.dwIndex = 1;
	}
*/	
	puiPara = appUiParaGet();
	switch (puiPara->Interval) {
		case PB_INTERVAL_1S:
			SlideShowTime = 1000;
			break;
		case PB_INTERVAL_2S:
			SlideShowTime = 2000;
			break;
		case PB_INTERVAL_3S:
			SlideShowTime = 3000;
			break;
		case PB_INTERVAL_4S:
			SlideShowTime = 4000;
			break;
		case PB_INTERVAL_5S:
			SlideShowTime = 5000;
			break;
		case PB_INTERVAL_6S:
			SlideShowTime = 6000;
			break;
		case PB_INTERVAL_7S:
			SlideShowTime = 7000;
			break;
		case PB_INTERVAL_8S:
			SlideShowTime = 8000;
			break;
		case PB_INTERVAL_9S:
			SlideShowTime = 9000;
			break;
		case PB_INTERVAL_10S:
			SlideShowTime = 10000;
			break;

	}
	
	pbParamSet(PB_PARAM_SLIDE_SHOW_FREQ, SlideShowTime);

	if (puiPara->Repeat == PB_REPEATE_YES) {
		SlideShowRpt = 1;
	} else {
		SlideShowRpt = 0;
	}
	pbParamSet(PB_PARAM_SLIDE_SHOW_RPT, SlideShowRpt);
	
	//printf("interval %u, Rpt %u\n", SlideShowTime, SlideShowRpt);	
	if(appPreStateGet(1) == APP_STATE_MUTI_PLAY)
	{
		sp1kPbSingleImgPlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
	}

	appAutoOffEn(0);
}

