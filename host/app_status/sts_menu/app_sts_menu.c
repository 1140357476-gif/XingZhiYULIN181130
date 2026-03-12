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
#include "initio.h"

#include "hal.h"
#include "hal_gprm.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "App_ui_para.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"

#include "app_menu_core.h"
#include "app_menu.h"
#include "app_menu_api.h"
#include "app_menu_micro.h"
#include "app_menu_tab.h"
#include "app_menu_event.h"
#include "app_audio_rec.h"
#include "app_cal_api.h"
#include "cardlink.h"
#include "sp1k_disk_api.h"
#include "key_scan.h"
#include "func_ptr.h"
#include "copydisk.h"
#include "main.h"
#include "app_still_view.h"
#include "app_still_play.h"
#include "sp1k_aud_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_cdsp_api.h"

//#include "dos32.h"
#include "app_audio_play.h"
#include "Uiflow.h"
#include "sp1k_dps_api.h"
#include "sp1k_disp_api.h"
#include "app_dev_disp.h"
#include "dbg_def.h"
#if (FUNC_HOST_DPS == 1)
#include "app_dps.h"
#include "app_sts_pwroff.h"
#endif
#if (FUNC_HOST_CAL == 1)
#include "App_cal_api.h"
#endif
#include "gui_evt.h"
#define sp1kHalCamModeSet(a)
#include "sp1k_pv_api.h"


/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 enterCalMode;
UINT8 user_exit_menu_flag=0;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern UINT8 JpgCropFlag;
 extern UINT8 key_enter_AF_menu_flag;

extern UINT8 flag_menu_back_videoview;

extern void menuDynamicFontInit(void);
extern void menuDynamicOsdExit(void);

extern UINT32 menuPosBackup;
extern SIMAGE xdata pbFile;
extern UINT8 PrintStatus;
extern UINT8 xdata preVideoFrameRate;
extern UINT8 xdata preVideoSize;
extern UINT8 xdata  storageReadyflag;
extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
extern UINT8  bUsbPwr;

/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if MSG_AUTO_SEND
extern UINT8 dele;
#endif
extern UINT8 delete_key_to_menu;

/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
//extern UINT8 xdata OsdMaxX, OsdMaxY;
//-----------------------------------------------------------------------------
//appMenu
//-----------------------------------------------------------------------------
void appMenu(UINT16 msg) USING_0
{
	UINT8 layer;
	UINT8 item;
	UINT8 sel = 0;
	UINT8 subItem=0;
	UINT32 menuPos=0;
//	powerBatLv_t batlv;
	static UINT8 hotKeyFlag = 0;
	static UINT16 preMsg = SP1K_MSG_NULL;
	uiPara_t* puiPara = appUiParaGet();

	if(appCardErrFmtErrChk())
	{

		//uiOsdDialogDisp(ID_STR_CARD_ERROR, 10);
		uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD, 10);
		if(msg ==SP1K_MSG_KEY_PRESS_POWER)
		{

		}
		else	if((msg>SP1K_MSG_START_BTN)&&(msg<SP1K_MSG_KEY_MAX))
		{
			return;
		}

	}
	
	if (preMsg == SP1K_MSG_KEY_PRESS_S2 && msg == SP1K_MSG_KEY_PRESS_OK) {
		//Enter the state of version show
		hotKeyFlag = 1;
	} else if (hotKeyFlag && preMsg == SP1K_MSG_KEY_RELEASE_SNAP) {
		//Exit the state of version show 		
		hotKeyFlag = 0;
		menuProcDisp();
		return;
	}

	menuPos = menuProcCurrPosGet();
	
	if (msg != SP1K_MSG_STATE_INIT)
	{
		layer = menuProcLayerGet();
		item = menuProcCurrItemGet(layer);
		if (menuProcExecute(layer, item, msg) == MENU_IGNORE_DEFAULT)
			return;
	}
	
	switch (msg) {
	case SP1K_MSG_STATE_INIT:
		//printf("menu init\n");
		menuInit();
		menuProcDisp();
		#if MSG_AUTO_SEND
			if(dele)
			{
				osMsgPost(SP1K_MSG_KEY_PRESS_OK);
			}

		#endif
		break;

	case SP1K_MSG_KEY_PRESS_S1:
		//printf("S1 on menu\n");
		break;

	case SP1K_MSG_KEY_PRESS_S2:
		break;
	#if 0
	case SP1K_MSG_KEY_PRESS_AF:
		if ((layer == MENU_LAYER_0) || (layer == MENU_LAYER_1))
		{
			menuExit();
		}	
		else if (layer == MENU_LAYER_2)
		{
			menuProcParent ();
			menuProcDisp();
		}
		osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);

		break;
	#endif
		
//1 PressMenu
	case SP1K_MSG_KEY_PRESS_MENU:
	//case SP1K_MSG_KEY_PRESS_MODE:
	//case SP1K_MSG_KEY_PRESS_AF:
		if ((layer == MENU_LAYER_0) || (layer == MENU_LAYER_1))
		{
			menuExit();
		}	
		else if (layer == MENU_LAYER_2)
		{
			menuProcParent ();
			menuProcDisp();
		}
		//osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);

		break;
		
	#if FUNC_HOST_TOUCH_PANEL
	case SP1K_MSG_GUI_PEN_SLIDE:
		{
		guiPix_t p0, p1;
		guiSlidePixGet(&p0, &p1);
		printf("%i,%i->%i,%i\n", p0.x, p0.y, p1.x, p1.y);
		if (p1.y > p0.y) {
			osMsgPost(SP1K_MSG_KEY_PRESS_UP);
		} else {
			osMsgPost(SP1K_MSG_KEY_PRESS_DOWN);
		}
		}
		break;
	
//1 TpPressUP		
	case SP1K_MSG_TP_UP:
		menuProcPageUp();
		menuProcDisp();		
		break;
		
//1 TpPressDown			
	case SP1K_MSG_TP_DOWN:
		menuProcPageDown();
		menuProcDisp();
		break;
	#endif
	
//1 PressUp
	case SP1K_MSG_KEY_PRESS_UP:
		menuProcMoveUp ();
		menuProcDisp();
		break;

//1 PressDown
	case SP1K_MSG_KEY_PRESS_DOWN:
	//case SP1K_MSG_KEY_PRESS_PB:
		menuProcMoveDown ();
		menuProcDisp();
		break;
	

	case SP1K_MSG_KEY_PRESS_LEFT:
//1 PressLeft
		/*if (layer == MENU_LAYER_2)
		{
			menuProcParent ();
			menuProcDisp();
		}*/
		break;
	
//1 PressRight
	case SP1K_MSG_KEY_PRESS_RIGHT:		
		/*subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			sel = menuProcParaGet(layer, item);
			menuProcChild (sel);
			menuProcDisp();
		 }*/
		break;

//1 PressOk
	//case SP1K_MSG_KEY_PRESS_SNAP:
	case SP1K_MSG_KEY_PRESS_OK:
		if (hotKeyFlag) {
			#if 0
			osMsgPost(SP1K_MSG_STATE_TO_CAL_MENU);
			#else
			appMenuEvent_VerShow();
			#endif
			break;
		}

	case SP1K_MSG_KEY_PRESS_F1:
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			sel = menuProcParaGet(layer, item);
			menuProcChild (sel);
			menuProcDisp();
		}
		else if (layer >= MENU_LAYER_2)
		{
			menuProcParaSet(layer, item);
			menuProcParent ();
			menuProcDisp();
		}
		break;

	case SP1K_MSG_KEY_PRESS_TELE:
		break;
		
	case SP1K_MSG_KEY_PRESS_WIDE:
		break;
		
	case SP1K_MSG_KEY_PRESS_DISP:
		break;
		
	case SP1K_MSG_CARD_PLUG_IN:

		#if 1//old method
			appStorageMount(K_MEDIA_SDF);
			//uiRemainPixInit();
			//menuExit();	
			//osMsgPost(SP1K_MSG_STATE_INIT);
			//uiRemainPixInit();
			//appStillOsdInit();
		#else//new method
			menuExit();	
			osMsgPost(SP1K_MSG_CARD_PLUG_IN);
		#endif
		break;
		
	case SP1K_MSG_CARD_PLUG_OUT:
		appCardErrFmtErrSet(0);
		appCardErrFmtSta(0);
		appStorageMount(K_MEDIA_NANDF);
		uiRemainPixInit();
		menuExit();
		osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
		//osMsgPost(SP1K_MSG_STATE_INIT);
		//uiRemainPixInit();
		//appStillOsdInit();
		//osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		//uiOsdDialogDisp(ID_STR_NO_CARD_);
		break;
		
	case SP1K_MSG_CARD_NOT_SUPPORT:
		menuExit();	
		osMsgPost(SP1K_MSG_CARD_NOT_SUPPORT);
		break;
		
	case SP1K_MSG_USB_PLUG_IN:

		if((1 == bUsbPwr && !prjDbgMode) ||appCardErrFmtChk())
		{
			break;
		}
		
		if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
		{
			break;
		}
		
		#if 0
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
		#endif
		osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);//SP1K_MSG_STATE_TO_VIDEO_VIEW
		osMsgPost(SP1K_MSG_USB_PLUG_IN);
		break;
		
    	case SP1K_MSG_USB_PLUG_OUT:
    		//osMsgPost(SP1K_MSG_POWER_OFF);
		break;
		
	case SP1K_MSG_BAT_LV_UPDATE://jintao.liu 2008-3-13 add for battery action
		//uiUpdateBatteryLevel();
		appBattStsHandle();
		break;
		
	case SP1K_MSG_BAT_EMPTY:
		sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
		uiUpdateOSDBatEmpty();
		osMsgPost(SP1K_MSG_POWER_OFF);
		sp1kBtnDisableAll();//ready to power off, disable all button!
		break;

	case SP1K_MSG_POWER_OFF:
		//break;
	case SP1K_MSG_KEY_REPEATE_POWER:
		menuExit();
		appPowerOffProc();
		break;
		
	case SP1K_MSG_TV_PLUG_IN:
	case SP1K_MSG_TV_PLUG_OUT:
		menuExit();
		appDispSet();
		break;
		
	case SP1K_MSG_HDMI_PLUG_IN:
		JpgCropFlag = 0;
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
		//appDispHdmiOpen();
		break;
		
	case SP1K_MSG_HDMI_PLUG_OUT:
		JpgCropFlag = 0;
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		appDispSet();			
		break;
		
	#if(FUNC_HOST_CAL ==1)
	case SP1K_MSG_UPDATE_AF_WINDOW:
		appCalAfOsdShow();
		break;
	#endif

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

		/*********************************************
		* jintao.liu 2008-6-17 add to mask cp to card
		*
		* Note:
		* the below code avoid xdata overflow
		*/
		#if (!FUNC_HOST_CP2CARD)
			{	
				UINT8 i = 0;
				if (i != 0)
					appCopy2Card(SP1K_FILE_PROC_ONE, K_Copy_Nand2Card);
			}
		#endif
		break;
	}

	preMsg = msg;
	menuProcDbgMsg(1);
}

/**
 * @fn        void menuOsdFitScr(UINT8 mode)
 * @brief     menuOsdFitScr
 * @param     [in] mode
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void menuOsdFitScr(UINT8 mode)
{
#define OSD_W 320
#define OSD_H 240
	UINT16 osdH0, osdH1, osdV0, osdV1;
	UINT16 osdFitH, osdFitV;
	UINT8 ratX, ratY;

	if (dispDevActGet() !=1) {
		return; /* Need not to change in TV out & HDMI mode */
	}

	HAL_DispPnlInitHvGet(&osdH0, &osdH1, &osdV0, &osdV1);

	osdFitH = osdH1 - osdH0 + 1;
	osdFitV = osdV1 - osdV0 + 1;
	
	if (mode) {
		dispPnlAspectRatioGet(&ratX, &ratY);
	
		if ((UINT32)OSD_W * ratY >= (UINT32)ratX * OSD_H) {
			/* fit width */
			osdFitV = ((UINT32)osdFitV * OSD_H * ratX) / ((UINT32)OSD_W * ratY);
		} else {
			/* fit height */
			osdFitH = ((UINT32)osdFitH * OSD_W * ratY) / ((UINT32)OSD_H * ratX);
		}
	}

	osdH0 += (osdH1 - osdH0 + 1 - osdFitH) >> 1;

	sp1kDispOsdEnSet(0);
	sp1kDispOsdHvSet(osdH0, osdV0, osdH0 + osdFitH - 1, osdV0 + osdFitV - 1);
	sp1kDispOsdSclUpd();

	sp1kHalCtrlScreen(3);
}

void appGuiMenuMainBgShow(UINT16 resId)
{
	UINT8 sts; 

	UINT32 srcAdd;
	UINT32 dstAdd;
	

	srcAdd=K_SDRAM_PvPanoramaTmpMeAddr; // -2*(320L*240L);
	dstAdd=srcAdd-(320L*240L); // -3*(320L*240L);
	
	ENTER_CRITICAL(sts); 

	sp1kHalCdspImgSrcSel(1);
	sp1kHalFrontInputGate(0x11);
	sp1kHalCamModeSet(CAM_MODE_IDLE);
	sp1kHalCdspImgSrcSel(1);
	
	appBGImgDec(resId, NULL, srcAdd, dstAdd, 1);
	sp1kDispImgShow(dstAdd, (UINT16)320, (UINT16)240, 100);
	
	EXIT_CRITICAL(sts);
}


//-----------------------------------------------------------------------------
//menuInit
//-----------------------------------------------------------------------------
void menuInit(void) USING_0
{
	UINT32 menuPos;
	UINT8 ratX, ratY;

	//appGuiMenuMainBgShow(0xb5);
		
	dispPnlAspectRatioGet(&ratX, &ratY);
	if ((UINT32)320 * ratY < (UINT32)ratX * 240) //mantis #42126
	{
		menuOsdFitScr(1);
	}

	
	switch ( appPreStateGet(0) )
	{
	case APP_STATE_STILL_VIEW:
		//menuPos = key_enter_AF_menu_flag ? MENU_MARK_AF : MENU_MARK_SETUP;
		menuPos = MENU_MARK_STILL;
		//menuPos = MENU_MARK_SETUP;
		key_enter_AF_menu_flag = 0;
		break;
		
	case APP_STATE_VIDEO_VIEW:	
		menuPos = MENU_MARK_VIDEO;
		//menuPos = MENU_MARK_SETUP;
		break;
		
	case APP_STATE_AUDIO_REC:		
		menuPos = MENU_MARK_AUDIO;
		break;
		
	case APP_STATE_VIDEO_PLAY:			
	case APP_STATE_STILL_PLAY:
	case APP_STATE_MUTI_PLAY:
		//menuPos = MENU_MARK_PLAYBACK; // MENU_MARK_DELONE_YES;
		menuPos = MENU_MARK_SETUP; 
		//menuPos = delete_key_to_menu ? MENU_MARK_PLAYBACK : MENU_MARK_STILL;
		break;

	case APP_STATE_USB:
		break;
		
	#if (FUNC_HOST_DPS == 1)
	case APP_STATE_DPS:
		menuPos = MENU_MARK_DPS;
		break;
	#endif
	
	default:
		menuPos = MENU_MARK_STILL;
	}

	if(shtMenuChk()){
		menuPos = shtMenuChk();
		printf("menuPos:%lx\n",menuPos);
	}

#if MSG_AUTO_SEND
	if(dele)
	{
		menuPos=MENU_MARK_DELETE_SINGLE_YES;
	}
#endif

	#if 0
		menuPos=MENU_MARK_CALIBRATION;
	#endif
	
	menuPosBackup = menuPos;
	menuProcCurrPosSet (menuPos);
}

//-----------------------------------------------------------------------------
//menuExit
//-----------------------------------------------------------------------------
void menuExit(void) USING_0
{
	UINT16 msg;

	UINT8 ratX, ratY;
	uiPara_t* puiPara = appUiParaGet();
	dispPnlAspectRatioGet(&ratX, &ratY);

	
	if(appCardErrFmtErrChk())
	{
		return;
	}
	switch ( appPreStateGet(0) )
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
		case APP_STATE_PAN_SHOW:
			msg = SP1K_MSG_STATE_TO_PAN_SHOW;
			break;
		case APP_STATE_STILL_PLAY:
			//delete_key_to_menu = 0;
			//msg = flag_menu_back_videoview ? SP1K_MSG_STATE_TO_VIDEO_VIEW : SP1K_MSG_STATE_TO_STILL_VIEW;
			//flag_menu_back_videoview = 0;
			msg = SP1K_MSG_STATE_TO_STILL_PLAY;//SP1K_MSG_STATE_TO_STILL_VIEW
			break;
		case APP_STATE_MUTI_PLAY:
			msg = SP1K_MSG_STATE_TO_MULTI_PLAY;
			break;

		case APP_STATE_VIDEO_PLAY:			
			msg = SP1K_MSG_STATE_TO_VIDEO_PLAY;
			break;

 		case APP_STATE_USB:
			msg = SP1K_MSG_STATE_TO_USB;
			break;
#if (FUNC_HOST_DPS == 1)
 		case APP_STATE_DPS:
			msg = SP1K_MSG_STATE_TO_DPS;
			break;
#endif			
		default:
			msg = SP1K_MSG_STATE_TO_STILL_VIEW;
			break;
	}
	bUsbPwr=0;
	JpgCropFlag = 0;
	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
	if ((UINT32)320 * ratY < (UINT32)ratX * 240) //16:9
	{
		menuOsdFitScr(0);
	}

	if((sp1kUSB_PlugGet())&&(AdapterCharge==0)&&(user_exit_menu_flag==0))
	{  
		if((puiPara->USBMode==0)||(puiPara->USBMode==1))
	  		osMsgPost(SP1K_MSG_STATE_TO_USB);
		else
		{
			puiPara->USBMode=0;
			//osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);//SP1K_MSG_STATE_TO_VIDEO_VIEW
			switch ( appPreStateGet(0) )
			{
				case APP_STATE_STILL_VIEW:
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;
					break;	
				case APP_STATE_VIDEO_VIEW:
					msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
					break;
				case APP_STATE_STILL_PLAY:
					msg = SP1K_MSG_STATE_TO_STILL_PLAY;//SP1K_MSG_STATE_TO_STILL_VIEW
				break;
				
				default:
				msg = SP1K_MSG_STATE_TO_STILL_VIEW;
				break;
			}
			osMsgPost(msg);
		}
	}
	else
		osMsgPost(msg);
}

UINT32 shtMenuChk(void)
{
	UINT32 pos=0;
	uiPara_t* puiPara = appUiParaGet();

	if((sp1kUSB_PlugGet())&&(AdapterCharge==0))
	{
		if(user_exit_menu_flag==0)
		{
			//printf("jasingou:set to usb menu\n");
			pos=MENU_MARK_USB|0x00010000;
		}
		else
		{
			//#if FUNC_HOST_CAL	
			//enterCalMode=1;
			//appCalInit();
			//pos = MENU_MARK_CALIBRATION;
			//#endif
		}
	}
	else
	if(appCardErrFmtChk())
	{
		pos = MENU_MARK_FORMAT_YES;
	}
	else if(appFileDelHotKeyChk())
	{
		appStillPlayInit(0);
		pos = MENU_MARK_DELONE_YES;
		
	} 
	else if(1 == bUsbPwr && !prjDbgMode) 
	{
		#if FUNC_HOST_CAL	
		enterCalMode=1;
		appCalInit();
		pos = MENU_MARK_CALIBRATION;
		#endif		
	}
	else if(sp1kSnapParamGet(SP1K_SNAP_PARAM_BLINK)==0x00ff)
	{
		pos=MENU_MARK_BLINK_SAVE;
	}
	else if(puiPara->redEyePb == REDEYEPB_SAVE)
	{
		pos=MENU_MARK_REDEYE_WRITE;
	}
	else if(JpgCropFlag)
	{
		pos=MENU_MARK_JPG_CROP_YES;
	}
	
	return pos;
}
