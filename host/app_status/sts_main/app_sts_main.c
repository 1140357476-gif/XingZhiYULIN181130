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
#include "timer.h"
#include "os_api.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_still_view.h"
#include "app_video_view.h"
#include "app_audio_rec.h"
#include "app_still_play.h"
#include "app_multi_play.h"
#include "app_video_play.h"
#include "app_audio_play.h"
#include "app_slide_show.h"
#include "app_pan_show.h"
#include "app_menu.h"
#include "app_idle.h"
//#include "app_cal_menu.h"
#include "app_usb.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"

#include "sp1k_aud_api.h"
#include "sp1k_video_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_disp_api.h"
#include "app_power_on.h"
#include "key_scan.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "smcfunc.h" //add for #36934
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "sp1k_gpio_api.h"
#include "dbg_def.h"
#include "app_sts_pwroff.h"
#include "app_gui_obj.h"
#include "sp1k_util_api.h"
#if (FUNC_HOST_DPS == 1)
#include "app_dps.h"
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

const UINT8 xdata prjDbgMode = 0;


/*Define the bank number where the func-pointer related function store in*/
#define SP1K_BANK_POWER_ON    11
#define SP1K_BANK_STILL_VIEW  11
#define SP1K_BANK_STILL_PLAY  11
#define SP1K_BANK_MUTI_PLAY   11
#define SP1K_BANK_VIDEO_VIEW  11
#define SP1K_BANK_VIDEO_PLAY  11
#define SP1K_BANK_AUDIO_REC   11
#define SP1K_BANK_AUDIO_PLAY  11
#define SP1K_BANK_MENU        11
#define SP1K_BANK_USB         11
#define SP1K_BANK_SLIDE_SHOW  11
#define SP1K_BANK_PAN_SHOW    11
//#define SP1K_BANK_MSG_BOX     11

#define SP1K_BANK_NUM_MAX		15

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appState_t code stStateFunc[] = {
	{ appPowerOn,	SP1K_BANK_POWER_ON },
	{ appStillView, 	SP1K_BANK_STILL_VIEW }, 
	{ appVideoView, 	SP1K_BANK_VIDEO_VIEW }, 
	{ appAudioRec,	SP1K_BANK_AUDIO_REC }, 
	{ appStillPlay, 	SP1K_BANK_STILL_PLAY }, 
	{ appVideoPlay, 	SP1K_BANK_VIDEO_PLAY }, 
	{ appAudioPlay, 	SP1K_BANK_AUDIO_PLAY }, 
	{ appMultiPlay,	SP1K_BANK_MUTI_PLAY }, 
	{ appUsb,		SP1K_BANK_USB }, 
	{ appMenu,		SP1K_BANK_MENU }, 
	{ appSlideShow,	SP1K_BANK_SLIDE_SHOW }, 
	{ appPanShow,	SP1K_BANK_PAN_SHOW }, 
	{ appIdle,		SP1K_BANK_MENU }, 
	{ appPowerOff,		SP1K_BANK_POWER_ON }, 
#if (FUNC_HOST_DPS == 1)
	{ appDps,	SP1K_BANK_MENU }, 
#endif

};
appState_t* xdata pActiveState = NULL;
CamState_t xdata stStateCtrl;
//appStateList_t xdata guiMode;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 xdata G_SDCardPlug;
extern UINT8 xdata G_ucStorageType;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT8 appComMsgParsing(UINT16 msg);
static UINT8 appStateDestoryProc(UINT8 state, UINT16 msg);
static UINT8 appStateSwitch(UINT16 msg);

//-----------------------------------------------------------------------------
//appMain
//-----------------------------------------------------------------------------
/**
 * @brief		appMain
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appMain (void)
{
	UINT16 msg;

	osMsgAccept(&msg);
	
	if(msg != 0x2904)	//xian ++ for debug
		printf("Msg(%x)\n", msg);//

	#if !TAG_TODO	
	//return;
	#endif	
	
	//CPrintf("Phil: msg is %bu", msg);

	/*Check if there is a state switch*/
	if (msg > SP1K_MSG_STATE_MAX) {
		sp1kLED_BusySet(0);
		sp1kBtnDisableAll();
		appComMsgParsing(msg);
		sp1kBtnEnableAll();
		sp1kLED_BusySet(1);
		
		return;
	}
	
	sp1kBtnDisableAll();
	if (appStateDestoryProc(stStateCtrl.ActiveState, msg) != SUCCESS) {
		//Task still not destoried
		sp1kBtnEnableAll();
		return;
	}

	/*state switch*/
	sp1kLED_BusySet(0);
	appStateSwitch(msg);
	sp1kLED_BusySet(1);
	sp1kBtnEnableAll();
}

//-----------------------------------------------------------------------------
//appComMsgParsing
//-----------------------------------------------------------------------------
/**
 * @brief		common msg parsing
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

static UINT8 appComMsgParsing(UINT16 msg)
{
	devAttr_t* devAttr;
	
	if (pActiveState == NULL) {
		return FAIL;
	}

	if ((msg < SP1K_MSG_KEY_MAX)&&(msg > SP1K_MSG_START_BTN)) {
		//Button Pressed action
		appVoicePlay(msg,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_LOW /*VOICE_PLAYLEVLE_LOW*/);
	}

	#if FUNC_HOST_TOUCH_PANEL
	if((msg>=SP1K_MSG_TP_MODE_SWITCH)&&(msg<SP1K_MSG_TP_MAX))
	{
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
	}
	#endif
	
	if ((msg < SP1K_MSG_KEY_MAX)&&(msg > SP1K_MSG_START_BTN)) {
		#if SUPPORT_NOCARD_OPERATION
		if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
		{
			if(appCurrStateGet() !=	APP_STATE_MENU)
			{
				if(msg == SP1K_MSG_KEY_PRESS_OK || msg == SP1K_MSG_KEY_PRESS_SNAP /*|| msg == SP1K_MSG_KEY_REPEATE_SNAP*/)
					msg = SP1K_MSG_CARD_PLUG_OUT;
			}
		}
		#else
		//add for mantis #36934@start
		if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
		{

			if(msg != SP1K_MSG_KEY_PRESS_POWER )
			{
				if(appCurrStateGet() !=APP_STATE_IDLE)
				{
					return FAIL;
				}
			}

		}	
		//add for mantis #36934@end
		#endif
		
		//Button action
		
		//jintao.liu 2008-2-23
		//add for auto power off
		appAutoOffTimeReload();
		
		//if (G_SDCardPlug || cardError) {
		devAttr = sp1kDevHdlGet();
		if (G_ucStorageType == K_MEDIA_SDF) {
			devAttr += DEV_ID_CARD;
		} else {
			devAttr += DEV_ID_DISK;
		}

		if (devAttr->devSts == DEV_STS_ERR){
			//card error
			if (msg != SP1K_MSG_KEY_PRESS_POWER && msg != SP1K_MSG_POWER_ON &&\
				msg != SP1K_MSG_KEY_PRESS_UP	&& msg != SP1K_MSG_KEY_PRESS_DOWN &&\
				msg != SP1K_MSG_KEY_PRESS_OK    && msg != SP1K_MSG_KEY_PRESS_SNAP
				) {
				return FAIL;//					
			}
		}
	}
	
	/*Send msg to active state*/
	pActiveState->StateFunc(msg);		
	
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appStateDestoryProc
//-----------------------------------------------------------------------------
/**
 * @brief		State destory process
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 appStateDestoryProc(UINT8 state, UINT16 msg)
{

	switch(state) {
		case APP_STATE_POWER_ON:
			sp1kBtnEnableAll();
			break;
			
		case APP_STATE_STILL_VIEW:
			sp1kOsdClear(0);
			#if FUNC_HOST_TOUCH_PANEL
			appStillGuiExit();
			#endif
#ifdef _ENABLE_LCD_OFF_
			appOsdModeSw(0);
#endif /*_ENABLE_LCD_OFF_*/
			break;
			
		case APP_STATE_VIDEO_VIEW:
			#if FUNC_HOST_TOUCH_PANEL
			appVideoGuiExit();
			#endif
			if (sp1kVideoRecStatusGet() != VID_REC_IDLE) {
				//Retry to stop the video record
				osMsgPost(SP1K_MSG_MEDIA_REC_STOP);
				//CPrintf("Retry Stop");
				return FAIL;
			}
			sp1kOsdClear(0);
			if (msg != SP1K_MSG_STATE_TO_MENU) {
				sp1kDispFitShow(320, 240, 0);
			}
			
#ifdef _ENABLE_LCD_OFF_
			appOsdModeSw(0);
#endif /*_ENABLE_LCD_OFF_*/
			break;
			
		case APP_STATE_AUDIO_REC:
			sp1kOsdClear(0);
			break;
			
		case APP_STATE_STILL_PLAY:
			#if FUNC_HOST_TOUCH_PANEL
			appStillPbGuiExit();
			#endif
			sp1kOsdClear(0);
			break;
			
		case APP_STATE_MUTI_PLAY:
			sp1kOsdClear(0);
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			break;
			
		case APP_STATE_VIDEO_PLAY:
			sp1kOsdClear(0);
			break;
			
		case APP_STATE_AUDIO_PLAY:
			sp1kOsdClear(0);
			break;
			
		case APP_STATE_MENU:
			sp1kOsdClear(0);
			#if FUNC_HOST_TOUCH_PANEL
			appMenuGuiExit();
			#endif
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			break;

		case APP_STATE_USB:
			sp1kOsdClear(0);
			break;

		case APP_STATE_SLIDE_SHOW:
			sp1kOsdClear(0);
			appAutoOffEn(1);
			break;
			
		case APP_STATE_PAN_SHOW:
			sp1kOsdClear(0);
			break;
			
		case APP_STATE_NULL:
		case APP_STATE_UNKNOWN:
		default:
			break;
	}
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appStateSwitch
//-----------------------------------------------------------------------------
/**
 * @brief		State switch process
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 appStateSwitch(UINT16 msg)
{
	UINT8 nextState;
	UINT8 nextStateId;
	
	switch (msg) {
	case SP1K_MSG_STATE_INIT:
		pActiveState->StateFunc(SP1K_MSG_STATE_INIT);	
		return SUCCESS;
		
    	case SP1K_MSG_STATE_TO_POWER_ON:		
		nextState = APP_STATE_POWER_ON;
		nextStateId = STATE_FUNC_ID_POWER_ON;
		break;
		
    	case SP1K_MSG_STATE_TO_STILL_VIEW:		
		nextState = APP_STATE_STILL_VIEW;
		nextStateId = STATE_FUNC_ID_STILL_VIEW;
		break;
		
    	case SP1K_MSG_STATE_TO_VIDEO_VIEW:
		nextState = APP_STATE_VIDEO_VIEW;
		nextStateId = STATE_FUNC_ID_VIDEO_VIEW;
		break;
		
    	case SP1K_MSG_STATE_TO_AUDIO_REC:
		nextState = APP_STATE_AUDIO_REC;
		nextStateId = STATE_FUNC_ID_AUDIO_REC;
		break;
		
    	case SP1K_MSG_STATE_TO_STILL_PLAY:
		nextState = APP_STATE_STILL_PLAY;
		nextStateId = STATE_FUNC_ID_STILL_PLAY;
		break;
		
    	case SP1K_MSG_STATE_TO_MULTI_PLAY:
		nextState = APP_STATE_MUTI_PLAY;
		nextStateId = STATE_FUNC_ID_MUTI_PLAY;
		break;
		
    	case SP1K_MSG_STATE_TO_VIDEO_PLAY:
		nextState = APP_STATE_VIDEO_PLAY;
		nextStateId = STATE_FUNC_ID_VIDEO_PLAY;
		break;
		
    	case SP1K_MSG_STATE_TO_AUDIO_PLAY:
		nextState = APP_STATE_AUDIO_PLAY;
		nextStateId = STATE_FUNC_ID_AUDIO_PLAY;
		break;
		
    	case SP1K_MSG_STATE_TO_MENU:
		nextState = APP_STATE_MENU;
		nextStateId = STATE_FUNC_ID_MENU;
		break;
		
    	case SP1K_MSG_STATE_TO_USB:
		nextState = APP_STATE_USB;
		nextStateId = STATE_FUNC_ID_USB;
		break;
		
    	case SP1K_MSG_STATE_TO_SLIDE_SHOW:
		nextState = APP_STATE_SLIDE_SHOW;
		nextStateId = STATE_FUNC_ID_SLIDE_SHOW;
		break;

    	case SP1K_MSG_STATE_TO_PAN_SHOW:
		nextState = APP_STATE_PAN_SHOW;
		nextStateId = STATE_FUNC_ID_PAN_SHOW;
		break;
/*		
    	case SP1K_MSG_STATE_TO_MSGBOX:
		nextState = APP_STATE_MSG_BOX;
		nextStateId = STATE_FUNC_ID_MSG_BOX;
		break;
*/		
		case SP1K_MSG_STATE_TO_IDLE:
		nextState = APP_STATE_IDLE;
		nextStateId = STATE_FUNC_ID_IDLE;
		break;

		case SP1K_MSG_STATE_TO_POWER_OFF:
		nextState = APP_STATE_POWER_OFF;
		nextStateId = STATE_FUNC_ID_POWER_OFF;
		break;
	
#if (FUNC_HOST_DPS == 1)
	case SP1K_MSG_STATE_TO_DPS:
		nextState = APP_STATE_DPS;
		nextStateId = STATE_FUNC_ID_DPS;
		break;
#endif
		
    	default:
		nextState = stStateCtrl.ActiveState;
		nextStateId = 0;
		//printf("msg is null\n");
		break;
	}

	if (stStateCtrl.ActiveState != nextState) {
		stStateCtrl.PrevState[1] = stStateCtrl.PrevState[0];
		stStateCtrl.PrevState[0] = stStateCtrl.ActiveState;
		stStateCtrl.ActiveState = nextState;	
		
		pActiveState = &stStateFunc[nextStateId];
		ACTIVE_FUNC_CLR();
	}

	if (msg != SP1K_MSG_NULL) {
		pActiveState->StateFunc(SP1K_MSG_STATE_INIT);	
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appPreStateGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get previous state
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
 
*/
UINT8 appPreStateGet(UINT8 PreLev)
{
	if (PreLev >= APP_STATE_BUFF_SIZE) {
		return APP_STATE_UNKNOWN;
	}
	
	return (stStateCtrl.PrevState[PreLev]);
}

//-----------------------------------------------------------------------------
//appCurrStateGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get current state
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appCurrStateGet(void)
{
	return (stStateCtrl.ActiveState);
	//return APP_STATE_STILL_VIEW;
}

