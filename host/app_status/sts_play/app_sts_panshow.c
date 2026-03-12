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
#include "key_scan.h"

#include "sp1k_disp_api.h"
#include "sp1k_pb_api.h"
#include "Sp1k_osd_api.h"
#include "app_still_play.h"
#include "app_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_pwr_mgr.h"

#include "gui_evt.h"//jj


#include "app_battery.h"
#include "smcfunc.h"
//#include "dos32.h"
#include "app_menu_tab.h"
#include "app_dev_disp.h"
#include "dbg_def.h"

/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/
#define SP1K_PAN_STEP	16
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 JpgCropFlag;
UINT8 edgeSts;
static UINT16 zoomfactor=100;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern UINT16 pbZoomCurLog ;
//-----------------------------------------------------------------------------
//appPanShow
//-----------------------------------------------------------------------------

void appPanShowOsdUpdate(void)
{
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	appDirectionIconShow(edgeSts);
	// uiUpdateOSDPvZoom(zoomfactor, 2);
	// appOsdDigitalZoomWinDraw(zoomfactor);
	sp1kOsdDispBuffShow();
}

void appPanShow(UINT16 msg)
{
	UINT16 newFact, pbZoomFact;
	
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			pbZoomFact = 10000/pbZoomCurLog;    //xian ++ 20081208, #30690
			sp1kPbDZoomFactorSet(pbZoomFact);
			if(pbZoomFact>400)
			{
				//uiUpdateOSDPvZoom(400, 2); 
			}
			else
			{
				//uiUpdateOSDPvZoom(pbZoomFact, 2); 
			}
			edgeSts=sp1kPbImgPanSet(0, 0);
			appDirectionIconShow(edgeSts);
			// appOsdDigitalZoomWinDraw(zoomfactor);	 //after croping while an image is locked , display Zoom Window
			break;
			
	   	case SP1K_MSG_KEY_PRESS_S1:
			break;
			
	    case SP1K_MSG_KEY_PRESS_S2:
			break;

		#if 1
		case SP1K_MSG_KEY_RELEASE_SNAP:
		//case SP1K_MSG_KEY_RELEASE_POWER:
			sp1kPbDZoomFactorSet(100);
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
		#endif
			
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_GUI_PEN_SLIDE:
		{
			guiPix_t lenth;
			guiMoveLengthGet(&lenth);
			if((abs(lenth.x)>20)||(abs(lenth.y)>20))
			{
				break;
			}
			edgeSts=sp1kPbImgPanSet(0-lenth.x, 0-lenth.y);
			appPanShowOsdUpdate();
			break;
		}
		
		case SP1K_MSG_GUI_PEN_UP:
			break;
		#endif

	    case SP1K_MSG_KEY_PRESS_DOWN://zuo
		case SP1K_MSG_KEY_REPEATE_DOWN:
			edgeSts=sp1kPbImgPanSet(0, 0 - SP1K_PAN_STEP);
			appPanShowOsdUpdate();
			break;
			
	    //case SP1K_MSG_KEY_PRESS_UP://you
		//case SP1K_MSG_KEY_REPEATE_UP:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_REPEATE_BURST:
			edgeSts=sp1kPbImgPanSet(0, SP1K_PAN_STEP);
			appPanShowOsdUpdate();
			break;
			
	    //case SP1K_MSG_KEY_PRESS_MENU://shang
		//case SP1K_MSG_KEY_REPEATE_MENU:
		 case SP1K_MSG_KEY_PRESS_UP:
		case SP1K_MSG_KEY_REPEATE_UP:
			edgeSts=sp1kPbImgPanSet(0 - SP1K_PAN_STEP, 0);
			appPanShowOsdUpdate();
			break;
		
	    case SP1K_MSG_KEY_PRESS_PB://xia
		case SP1K_MSG_KEY_REPEATE_PB:
			edgeSts=sp1kPbImgPanSet(SP1K_PAN_STEP, 0);
			appPanShowOsdUpdate();
			break;
			
    	case SP1K_MSG_KEY_PRESS_SNAP:
	    	//JpgCropFlag=1;
	    	//osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;
			
		case SP1K_MSG_KEY_PRESS_DISP:
			break;

		
		case SP1K_MSG_KEY_PRESS_ZOOM_IN:
		case SP1K_MSG_KEY_REPEATE_ZOOM_IN:
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_TELE:
		case SP1K_MSG_KEY_REPEATE_TELE: 
			if (!sp1kPbFileCntGet()) 
			{
				break;
			}
			
			if(pbFile.wType==SP1K_PB_FILE_TYPE_JPG)
			{
				// add for debug zoom error for avi@2008-03-07 end
				pbZoomCurLog -= PLAY_ZOOM_logStep;
			}
						
			if (pbZoomCurLog > PLAY_MAX_ZOOM_LOG)
			{
				pbZoomCurLog = PLAY_MAX_ZOOM_LOG;
			}
			
			newFact = 10000/pbZoomCurLog;
			if(newFact <= PLAY_MAX_ZOOM_FACT) 
			{
				pbZoomFact = newFact;
			}
			else 
			{
				pbZoomCurLog = PLAY_MIN_ZOOM_LOG;
				pbZoomFact = PLAY_MAX_ZOOM_FACT;
				zoomfactor=pbZoomFact;
				appPanShowOsdUpdate();
				return;
			}
			zoomfactor=pbZoomFact;
			sp1kPbDZoomFactorSet(pbZoomFact);
			edgeSts=sp1kPbImgPanSet(0, 0);		//obtain edgeSts value while zooming up with no steps (1024)
			appPanShowOsdUpdate();
			break;

		
		case SP1K_MSG_KEY_PRESS_ZOOM_OUT:		
		case SP1K_MSG_KEY_REPEATE_ZOOM_OUT:
		//case SP1K_MSG_KEY_RELEASE_POWER:
		case SP1K_MSG_KEY_PRESS_WIDE:
		case SP1K_MSG_KEY_REPEATE_WIDE:
			if (!sp1kPbFileCntGet())
			{
				break;
			}
			
			if(pbFile.wType==SP1K_PB_FILE_TYPE_JPG)
			{
				pbZoomCurLog += PLAY_ZOOM_logStep;

			}
						
			if (pbZoomCurLog < PLAY_MIN_ZOOM_LOG)
			{
				pbZoomCurLog = PLAY_MIN_ZOOM_LOG;
			}
			
			newFact = 10000/pbZoomCurLog;
			
			if(newFact >= PLAY_MIN_ZOOM_FACT ) 
			{
				pbZoomFact = newFact;
			}
			else
			{
				pbZoomCurLog = PLAY_MAX_ZOOM_LOG; 
				pbZoomFact = PLAY_MIN_ZOOM_FACT;
				zoomfactor=pbZoomFact;
				appPanShowOsdUpdate();
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				edgeSts=0;
				break;
			}
			
			sp1kPbDZoomFactorSet(pbZoomFact);
			zoomfactor=pbZoomFact;
			edgeSts=sp1kPbImgPanSet(0, 0);
			appPanShowOsdUpdate();
			
			if(pbZoomCurLog == PLAY_MAX_ZOOM_LOG)
			{//xian ++ 20081208, #30690
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				edgeSts=0;
			}
			break;

    	case SP1K_MSG_USB_PLUG_IN:
    	case SP1K_MSG_USB_PLUG_OUT:
    	case SP1K_MSG_CARD_PLUG_IN:
    	case SP1K_MSG_CARD_PLUG_OUT:
    	case SP1K_MSG_TV_PLUG_IN:
    	case SP1K_MSG_TV_PLUG_OUT:
		case SP1K_MSG_HDMI_PLUG_IN:
			edgeSts=0;
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(msg);
			break;
			
		case SP1K_MSG_HDMI_PLUG_OUT:
			appDispSet();			
			break;
			
		case SP1K_MSG_BAT_LV_UPDATE:
			uiUpdateBatteryLevel();
			appBattStsHandle();
			break;
			
		case SP1K_MSG_BAT_EMPTY:
			uiUpdateOSDBatEmpty();
			osMsgPost(SP1K_MSG_POWER_OFF);
			sp1kBtnDisableAll();//ready to power off, disable all button!
			break;

		case SP1K_MSG_POWER_OFF:
		case SP1K_MSG_KEY_REPEATE_POWER:
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



