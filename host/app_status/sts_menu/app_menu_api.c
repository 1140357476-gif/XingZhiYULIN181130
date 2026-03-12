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
#include "sp1k_dos_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_gpio_api.h"
#include "app_audio_play.h"
#include "Uiflow.h"
#include "sp1k_dps_api.h"
#include "app_dev_disp.h"
#include "app_multi_play.h"
#include "dps_struct.h"
#include "app_dps.h"
#include "dbg_def.h"
#include "Sp1k_pv_api.h"
#include "App_cal_api.h"
#include "sp1k_video_api.h"
#include "sp1k_pv_api.h"
#include "app_video_view.h"
#include "pv_zoom.h"
#include "sp1k_video_capture.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 code rtc_init[3]={1, 2, 0};
UINT8 code rtc_left[3][6]={{2, 5, 1, 0, 3, 4}, {1, 2, 5, 0, 3, 4}, {5, 0, 1, 2, 3, 4}};
UINT8 code rtc_right[3][6]={{3, 2, 0, 4, 5, 1}, {3, 0, 1, 4, 5, 2}, {1, 2, 3, 4, 5, 0}};
xdata UINT8 frameRateIsL;/* 0:60    1:30 */
UINT8 fdflag = 0;
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
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern SIMAGE xdata pbFile;
extern xdata dateStc_t	DATEINFO;
extern xdata UINT8 voicePlayFlag;
extern UINT8 DpsImageChooseFlag;

extern UINT8 fast_delete_back;

extern UINT8 user_exit_menu_flag;

extern UINT8 JpgCropFlag;
extern UINT8 imgResolutionSet;
extern UINT8 specialMenuOrder[30];
extern UINT8 specialMenuCurrItemIdx;
extern UINT8 specialMenuItemNum;
extern UINT16 stillZFctor;
extern UINT16 stillCurrLog;
extern UINT16 videoZFctor;
extern UINT16 videoCurrLog;
extern appPvZoom_t xdata PvZoomCtrl;
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuWBModeSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
		//case SP1K_MSG_KEY_PRESS_LEFT:
			appWBSet(puiPara->WBMode);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appWBSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcMoveDown();
			appWBSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuSharpnessSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			appSharpnessSet(puiPara->Sharpness);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appSharpnessSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcMoveDown();
			appSharpnessSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

UINT8 menuSaturationSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			appSaturationSet(puiPara->Saturation);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appSaturationSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			appSaturationSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

UINT8 menuContrastSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			appContrastSet(puiPara->Contrast);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appContrastSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			appContrastSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

UINT8 menuImageSizeInit (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT8 num=0;
	UINT8 itemIdx,imageSize;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			imgResolutionSet = 1;
			if(appSDPlugStatusGet()==1) {
				itemIdx = ITEM_EMPTY + (&puiPara->ImageSize- &puiPara->EmptyPara);
				imageSize = puiPara->ImageSize;
			}
			else {
				itemIdx = ITEM_EMPTY + (&puiPara->ImageSizeNoCard- &puiPara->EmptyPara);;
				imageSize = puiPara->ImageSizeNoCard;
			}
			if(FALSE==appUiItemOrderGet(itemIdx,specialMenuOrder,&specialMenuItemNum))
			{
				appUiItemStsSet(itemIdx,0);
				ret = MENU_CALL_DEFAULT;
				break;
			}
			appUiItemStsSet(itemIdx,1);
			while(specialMenuOrder[num] != imageSize)
			{
				num++;
				if(num > specialMenuItemNum-1) {
					specialMenuCurrItemIdx = 0;
					break;
				}
			}
			specialMenuCurrItemIdx=num;

			menuProcChild(specialMenuOrder[specialMenuCurrItemIdx]);
			menuSpecialSequenceDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
			
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}


/*	for(num=0;num<specialMenuItemNum;num++)
	{
		printf("%bu---->%bu\n",num,specialMenuOrder[num]);
	}*/
	return ret;

}
UINT8 menuImagesizeSet (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 itemIdx;
	item=item;
	layer=layer;
	if(appSDPlugStatusGet()==1) {
		itemIdx = ITEM_EMPTY + (&puiPara->ImageSize - &puiPara->EmptyPara);
	}
	else {
		itemIdx = ITEM_EMPTY + (&puiPara->ImageSizeNoCard- &puiPara->EmptyPara);
	}
	if(appUiItemStsGet(itemIdx) == FALSE)
	{
		return ret;
	}
	else
	{
		switch (msg)
		{
			//case SP1K_MSG_KEY_PRESS_SNAP:
			case SP1K_MSG_KEY_PRESS_OK:
				puiPara ->ImageSize = specialMenuOrder[specialMenuCurrItemIdx];
				menuProcParent ();
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			case SP1K_MSG_KEY_PRESS_UP:
				menuSpecialMoveUp();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			//case SP1K_MSG_KEY_PRESS_DOWN:
			case SP1K_MSG_KEY_PRESS_PB:
				menuSpecialMoveDown();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
		}

	}

	return ret;


}

UINT8 menuFDMeteringSet (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer=layer;
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item == FD_AE_ON)
			{
				puiPara->FDMode = FDMODE_ON;//force fd open
			}
			else
			{

			}
			break;
	}
	return ret;


}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuAEModeInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	#if FUNC_HOST_TOUCH_PANEL
	return ret;
	#endif
	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			menuProcChild(menuProcParaGet(layer, item));
			menuEVDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuAEModeSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_AF:
		//	appEVSet(puiPara->AEMode);
		//	break;

		
		//case SP1K_MSG_KEY_PRESS_MENU:
		case SP1K_MSG_KEY_PRESS_DOWN:
		//case SP1K_MSG_KEY_PRESS_LEFT:
			menuProcMoveUp();
			appEVSet(menuProcCurrItemGet(layer));
			menuEVDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		
		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BURST:
		//case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcMoveDown();
			appEVSet(menuProcCurrItemGet(layer));
			menuEVDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			menuProcParaSet(layer, item);
			#if FUNC_HOST_TOUCH_PANEL
			appEVSet(puiPara->AEMode);
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			#endif
			break;

		case SP1K_MSG_KEY_PRESS_UP:
		//case SP1K_MSG_KEY_PRESS_DOWN:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}



UINT8 menuSpeedyCapSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;
	item = item;

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_OK:
			if(item==0)
			{
				puiPara->SpeedyCap = SPEEDY_CAPTURE_ON;
				sp1kVideoCapParamSet(SP1K_VD_CAP_MOD,SP1K_SPEEDCAP_STILL);
			}
			else
			{
				sp1kVideoRecParamSet(VD_CAPTURE,0);
				puiPara->SpeedyCap = SPEEDY_CAPTURE_OFF;
			}
			/*if(item == SPEEDY_CAPTURE_ON)
			{
				#if (STILL_NEW_ZOOM == 1)
				stillZFctor=sp1kPvZoomFctGet();
				stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=PvZoomCtrl.factor;
				stillCurrLog = PvZoomCtrl.curLog;
				#endif
			}
			else
			{
				#if (VIDEO_NEW_ZOOM == 1)
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=video_zoomFactor;
				stillCurrLog = ZoomCurLog;
				#endif
			}*/
			break;
	}
	return ret;



}



UINT8 menuSpeedyCapRecSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puipara = appUiParaGet();
	layer=layer;
	item = item;

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item==0)
			{
				puipara->SpeedyCapRec = SPEEDY_CAPTURE_REC_ON;
				sp1kVideoCapParamSet(SP1K_VD_CAP_MOD,SP1K_SPEEDCAP_CLIP);
			}
			else
			{
				sp1kVideoRecParamSet(VD_CAPTURE,0);
				puipara->SpeedyCapRec = SPEEDY_CAPTURE_REC_OFF;
			}
			/*if(item == SPEEDY_CAPTURE_ON)
			{
				#if (STILL_NEW_ZOOM == 1)
				stillZFctor=sp1kPvZoomFctGet();
				stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=PvZoomCtrl.factor;
				stillCurrLog = PvZoomCtrl.curLog;
				#endif
			}
			else
			{
				#if (VIDEO_NEW_ZOOM == 1)
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=video_zoomFactor;
				stillCurrLog = ZoomCurLog;
				#endif
			}*/
			break;
	}
	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSnapEffectSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			//case SP1K_MSG_KEY_PRESS_LEFT:
			appEffectSet(puiPara->ImageEffect);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appEffectSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			appEffectSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if 1//FUNC_HOST_TOUCH_PANEL
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			appEffectSet(menuProcCurrItemGet(layer));
			menuProcParaSet(layer, item);
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
	}
	return ret;
}


/**
 * @fn        UINT8 menuSceneModeSet(UINT8 layer, UINT8 item, UINT16 msg)
 * @brief     menuSceneModeSet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2011-1-5
 * @todo      N/A
 * @bug       N/A
*/
UINT8 menuSceneModeSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			appSceneSet(puiPara->SceneMode);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			appSceneSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcMoveDown();
			appSceneSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}
#if FUNC_HOST_PF_PV
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 mneuPhotoFrameSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 currentItem;
	sp1kBtnDisableAll();
	switch (msg) 
	{
		case SP1K_MSG_KEY_PRESS_MENU:
			if (puiPara->PhotoFrame != item)
			{
				appPvPhotoFrameSet(puiPara->PhotoFrame);
			}
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			currentItem = menuProcCurrItemGet(layer);
			if (currentItem != PRV_PHOTO_FRAME_OFF)
			{
				sp1kOsdClear(0);
			}
			else
			{
				sp1kPvPhotofrmEnable(0);
				menuProcDisp();
			}
			appPvPhotoFrameSet(currentItem);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			currentItem = menuProcCurrItemGet(layer);
			if (currentItem != PRV_PHOTO_FRAME_OFF)
			{
				sp1kOsdClear(0);
			}
			else
			{
				menuProcDisp();
			}
			appPvPhotoFrameSet(currentItem);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	sp1kBtnEnableAll();
	return ret;
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLockFile (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg)
	{
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item == 0)
			{
				menuProcChild (menuProcParaGet(layer, item));
				 if(pbFile.bImageProtect == TRUE)
				 {
					 menuProcDispYesNo(ID_STR_UNPROTECT_FILE_);
				 }
				 else
				 {
					 menuProcDispYesNo(ID_STR_PROTECT_FILE_);
				 }

			}
			else if(item == 1)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcDispYesNo(ID_STR_PROTECT_ALL_);
			}
			else if(item == 2)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcDispYesNo(ID_STR_UNPROTECT_ALL_);
			}
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}
UINT8 menuLockOne (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 idx;
	PBFile_t pFileInfo;
	layer=layer;//just for clearing waring
	sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
	if((pFileInfo.FileAttrib & 0x03) == 1){
		idx = ID_STR_UNPROTECT_FILE_;
	}
	else{
		idx	= ID_STR_PROTECT_FILE_;
	}
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(idx);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(idx);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:

			if (appPreStateGet(0) == APP_STATE_MUTI_PLAY)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			if(appStillPlayNext(0)== FAIL)
			{
				menuExit();
			}
			else
			{
				if(pbFile.bImageProtect == TRUE)
				{
					menuProcDispYesNo(ID_STR_UNPROTECT_FILE_);
				}
				else
				{
					menuProcDispYesNo(ID_STR_PROTECT_FILE_);
				}
				uiUpdateOSDPbFileNum();
				uiUpdateOSDPbType(0 , 0 , 5);
				ret = MENU_IGNORE_DEFAULT;
			}
			break;

		case SP1K_MSG_KEY_PRESS_RIGHT:

			if (appPreStateGet(0) == APP_STATE_MUTI_PLAY)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			if(appStillPlayNext(1) == FAIL)
			{
				menuExit();
			}
			else
			{
				if(pbFile.bImageProtect == TRUE)
				{
					menuProcDispYesNo(ID_STR_UNPROTECT_FILE_);
				}
				else
				{
					menuProcDispYesNo(ID_STR_PROTECT_FILE_);
				}
				uiUpdateOSDPbFileNum();
				uiUpdateOSDPbType(0 , 0 , 5);
				ret = MENU_IGNORE_DEFAULT;
			}

			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				}
				else {
					sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
					if((pFileInfo.FileAttrib & 0x03) == 1){
						appFileAttrSet(pbFile.dwIndex, 0, SP1K_FILE_PROC_ONE);
					}
					else{
						appFileAttrSet(pbFile.dwIndex, 1, SP1K_FILE_PROC_ONE);
					}
					/*fixed mantis bug 45390*/
					appPbFileInfoUpdate();

					/* fixed mantis 45355*/
					//dcfFileInitialize(); //xian -- 20121116, fix 45355 in dcf lib

				}
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLockAll (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_PROTECT_ALL_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_PROTECT_ALL_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				}
				else {
					appUiBusyWait(250, 1);
					appFileAttrSet(pbFile.dwIndex, 1, SP1K_FILE_PROC_ALL);
					appUiBusyExit();

					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuUnlockAll (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg) {

		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_UNPROTECT_ALL_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_UNPROTECT_ALL_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				}
				else {
					appUiBusyWait(250, 1);
					appFileAttrSet(pbFile.dwIndex, 0, SP1K_FILE_PROC_ALL);
					appUiBusyExit();

					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
			}
			break;
	}
	return ret;
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDeleteInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring
	if ((appPreStateGet(0) == APP_STATE_MUTI_PLAY) ||(pbFile.bFileErr == TRUE)) {
		switch (msg) {
			case SP1K_MSG_KEY_PRESS_UP:
				menuProcParent();
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			case SP1K_MSG_KEY_PRESS_DOWN:
				if((pbFile.wType == SP1K_PB_FILE_TYPE_AVI)||(pbFile.wType == SP1K_PB_FILE_TYPE_WAV)){
					menuProcParent();
					menuProcDisp();
					ret = MENU_IGNORE_DEFAULT;
				}
				break;
			}

	}
	return ret;

}



/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDeleteFile (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg)
	{
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item == 0)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
			}
			else if(item == 1)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcDispYesNo(ID_STR_DELETE_ALL_FILE_);
			}
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}





/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDeleteOne (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;	//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_LEFT:

			if (appPreStateGet(0) == APP_STATE_MUTI_PLAY)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			if(appStillPlayNext(0) == FAIL)
			{
				appFileDelHotKeySta(0);
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				//menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
				appPbFileInfoUpdate();
				uiUpdateOSDPbFileNum();
				uiUpdateOSDPbType(0 , 0 , 5);
				ret = MENU_IGNORE_DEFAULT;
			}
			break;

		case SP1K_MSG_KEY_PRESS_RIGHT:

			if (appPreStateGet(0) == APP_STATE_MUTI_PLAY)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			if(appStillPlayNext(1) == FAIL)
			{
				appFileDelHotKeySta(0);
				menuExit();
			}
			else
			{
				appPbFileInfoUpdate();
				//menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
				uiUpdateOSDPbFileNum();
				uiUpdateOSDPbType(0 , 0 , 5);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{

				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
					appFileDelHotKeySta(0);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
					break;
				}
				else if(pbFile.bImageProtect == TRUE)
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
					appFileDelHotKeySta(0);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
					break;
				}

				if(appFileDelete(SP1K_FILE_PROC_ONE))
				{
					if (appPreStateGet(0) == APP_STATE_MUTI_PLAY)
					{
						appFileDelHotKeySta(0);
						menuExit();
						appMultiPlayInit(SP1K_PB_INIT_FULL);
						ret = MENU_IGNORE_DEFAULT;
					}
					else
					{
						menuProcDispYesNo(ID_STR_DELETE_THIS_FILE_);
						uiUpdateOSDPbFileNum();
						uiUpdateOSDPbType(0 , 0 , 5);
						ret = MENU_IGNORE_DEFAULT;

					}
				}
				else
				{
					appFileDelHotKeySta(0);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
			}else if(appFileDelHotKeyChk()){
				appFileDelHotKeySta(0);
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}
UINT8 menuFDModeSet(UINT8 layer,UINT8 item,UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch(msg)
	{
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if(1 == item)//fd off,force smile off
			{
				puiPara->SmileMode = SMILEMODE_OFF;
				puiPara->FDMetering = FD_AE_OFF;
			}else{
				puiPara->FDMode =FDMODE_ON;
			}
			fdflag = 0;
			uiSmildeUpdate();
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp();
			//printf("SMILEMODE : %d\n", (UINT8)puiPara->SmileMode);
			//printf("FDMODE : %d\n", (UINT8)puiPara->FDMode);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT8 menuSmileModeSet(UINT8 layer,UINT8 item,UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch(msg)
	{
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if(0 == item)//smile open,force  FD open
			{
				puiPara->FDMode =FDMODE_ON;
				fdflag = 1;
			}
			else if(fdflag) //smile off
			{
				fdflag = 0;
				//puiPara->FDMode =FDMODE_OFF;
			}
			break;
	}

	return ret;

}

//#define menuBlinkSave_Print	printf
#ifndef menuBlinkSave_Print
	#define menuBlinkSave_Print	/_printf
#endif

UINT8 menuBlinkSave (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,1);
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			else if(pbFile.bImageProtect == TRUE)
			{
				uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
				sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,1);
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
				break;
			}

			if(item == 0)
			{
				appUiBusyWait(250, 1);
				sp1kSnapStore();
				menuBlinkSave_Print("save img\n");
				appUiBusyExit();
				menuExit();
				ret = MENU_IGNORE_DEFAULT;

			}
			else
			{
				menuBlinkSave_Print("skip img\n");
				sp1kSnapTurnOnPv();
				menuExit();
				ret = MENU_IGNORE_DEFAULT;

			}
			sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,1);
			break;
	}
	return ret;


}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDeleteAll (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT8 LastMode;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;	//just for clearing waring
	switch (msg) {

		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_DELETE_ALL_FILE_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_DELETE_ALL_FILE_);
			uiUpdateOSDPbFileNum();
			uiUpdateOSDPbType(0 , 0 , 5);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{

				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
					break;
				}

				appUiBusyWait(250, 1);
				appFileDelete(SP1K_FILE_PROC_ALL);

				pbFile.dwIndex = sp1kPbFileCntGet();
				if (0 != pbFile.dwIndex)
				{
					//printf("B\n");
					appPbFileInfoUpdate();//to refresh "pbFile".
				}

				appUiBusyExit();

				LastMode = appPreStateGet(0);

				if (APP_STATE_MUTI_PLAY != LastMode)
				{
					//printf("C\n");
					if((pbFile.dwIndex != 0)) //&& (pbFile.bImageProtect == TRUE))
					{
						appStillPlayNext(0xFF); //2008-5-22 for mantis 25200
						if(pbFile.bImageProtect == TRUE)
						{
							uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
						}

					}
					else
					{
						sp1kDispImgWinEn(0);
					}
				}
				else
				{

					//printf("D\n");
					if((0 != pbFile.dwIndex) && (pbFile.bImageProtect == TRUE))
					{
						uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
					}
					appMultiPlayInit(SP1K_PB_INIT_FULL);
				}

				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSlideShowInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring
	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcChild(0);
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			if((appPreStateGet(0) == APP_STATE_MUTI_PLAY)&&(pbFile.wType == SP1K_PB_FILE_TYPE_JPG)) {
				menuProcParent();
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSlideShowSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:
			if(item != 0)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcMoveUp();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);
				menuProcParaSet(layer, item);
				menuProcParent();
				menuProcDisp2Layer();
			}
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_RIGHT:
			if(item != 0)
			{
				menuProcChild (menuProcParaGet(layer, item));
				menuProcMoveDown();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);
				menuProcParaSet(layer, item);
				menuProcParent();
				menuProcDisp2Layer();
			}
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			if (item == 0)
			{
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
				osMsgPost(SP1K_MSG_STATE_TO_SLIDE_SHOW);
				//menuExit();
			}
			else
			{
				#if FUNC_HOST_TOUCH_PANEL
				menuProcTPSet(item,0);
				menuProcDisp2Layer();
				#else
				menuProcParent ();
				menuProcDisp();
				#endif
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuImageEffectSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	PBFile_t fileInfo;
	pbEditType_t imgEffect;
	layer=layer;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:

		switch (item) {
			case PB_EFFECT_BLACK_WHITE:
				imgEffect = PB_IMG_EFFECT_BLACKWHITE;
				break;
			case PB_EFFECT_SEPIA:
				imgEffect = PB_IMG_EFFECT_SEPIA;
				break;
			case PB_EFFECT_NEGA:
				imgEffect = PB_IMG_EFFECT_NEGTIVE;
				break;
			case PB_EFFECT_RED:
				imgEffect = PB_IMG_EFFECT_RED;
				break;
			case PB_EFFECT_GREEN:
				imgEffect = PB_IMG_EFFECT_GREEN;
				break;
			case PB_EFFECT_BLUE:
				imgEffect = PB_IMG_EFFECT_BLUE;
				break;
			case PB_EFFECT_BINARIZE:
				imgEffect = PB_IMG_EFFECT_BINARIZE;
				break;
			case PB_EFFECT_SUNSET:
				imgEffect = PB_IMG_EFFECT_SUNSET;
				break;
			case PB_EFFECT_WARM:
				imgEffect = PB_IMG_EFFECT_WARM;
				break;
			case PB_EFFECT_COLD:
				imgEffect = PB_IMG_EFFECT_COLD;
				break;
			case PB_EFFECT_CONTRAST:
				imgEffect = PB_IMG_EFFECT_CONTRAST;
				break;
			default:
				imgEffect = PB_IMG_EFFECT_NORMAL;
				break;
		}

		if(1 == appSDProtectStatusGet()) //card lock remainder
		{
			uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
			break;
		}
		if (pbFile.bImageProtect == TRUE)
		{
			uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
			//menuExit();
			//ret = MENU_IGNORE_DEFAULT;
			break;
		}
		if (sp1kDosGetDiskSpace() < 1000)
		{
			sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
			uiUpdateOSDMemFull();
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
		}
		else
		{
			appUiBusyWait(250, 0);
			sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
			sp1kPbEffectSet(&fileInfo, imgEffect);

			appUiBusyExit();

			pbFile.dwIndex = sp1kPbFileCntGet();
			//sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
		}
		break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDistortionSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 DistorConTab[12]={1,2 ,7,8,3,4,9,10,13,5,14,15};
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	PBFile_t fileInfo;
	UINT32 addr;
	UINT16 pw,ph;

	layer=layer;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:

			if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				break;
			}
			else if (pbFile.bImageProtect == TRUE)
			{
				uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
				break;
			}
			if (sp1kDosGetDiskSpace() < 1000)
			{
				sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
				uiUpdateOSDMemFull();
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				appUiBusyWait(250, 0);
				sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
				sp1kHALDispPbFrameBufferGet(&addr,&pw,&ph);
				if((pw == 320)&&(ph == 240) || (pw == 240)&&(ph == 320)){
					sp1kPbDistortView(DistorConTab[item]);
				}
				sp1kPbDistortSave(fileInfo.FileIdx, DistorConTab[item]);
				appUiBusyExit();
				pbFile.dwIndex = sp1kPbFileCntGet();
				sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuRotateSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	PBFile_t fileInfo;
	layer=layer;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
		//case SP1K_MSG_KEY_PRESS_LEFT:
			sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
			sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
			pbCropDispBuffer2ABBuf();//mantis 45655
			break;

		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(0 == item) // rot
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
					break;
				}
				else if (pbFile.bImageProtect == TRUE)
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
				//	menuExit();
				//	ret = MENU_IGNORE_DEFAULT;
					break;
				}
				sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
				sp1kPbParamSet(PB_PARAM_ROT, 1);
				sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
				ret = MENU_IGNORE_DEFAULT;
			}
			else if (1 == item)	 // save
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
					sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
					sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
					break;
				}
				else if (pbFile.bImageProtect == TRUE)
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
					sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
					sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
				//	menuExit();
				//	ret = MENU_IGNORE_DEFAULT;
					break;
				}
				if (sp1kDosGetDiskSpace() < 1000)
				{
					sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
					uiUpdateOSDMemFull();
					sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
					sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
				else{
					appUiBusyWait(250, 0);

					sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
					//pbDZoomPause();  //rhy 20100401
					//pbParamSet(PB_PARAM_JPG_ROT, 1); //open real rotation function
					sp1kPbImgRotate(&fileInfo);
					sp1kPbParamSet(PB_PARAM_ROT, 0);	//clear

					pbFile.dwIndex = sp1kPbFileCntGet();
					sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));

					appUiBusyExit();
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
			}
			else // backup
			{
				sp1kPbParamSet(PB_PARAM_ROT, 0);	//clear
				sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
				pbCropDispBuffer2ABBuf();//mantis 45655
			}
			break;

		case SP1K_MSG_CARD_PLUG_IN: //xian ++ 42011
		case SP1K_MSG_CARD_PLUG_OUT:

		case SP1K_MSG_TV_PLUG_IN:
		case SP1K_MSG_TV_PLUG_OUT:
			sp1kPbParamSet(PB_PARAM_ROT, 0);
			sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
			break;
	}
	return ret;
}


UINT8 menuRedEyeSet(UINT8 layer,UINT8 item,UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg)
	{
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item == 0)
			{
				/*
					call function of red eye process
				*/
				puiPara->redEyePb = REDEYEPB_SAVE;
				osMsgPost(SP1K_MSG_STATE_INIT);
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				puiPara->redEyePb = REDEYEPB_OFF;
			}
			break;
		default:
			break;
	}

	return ret;

}

UINT8 menuRedEyeSave(UINT8 layer,UINT8 item,UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg)
	{
		case SP1K_MSG_KEY_PRESS_MENU:
			puiPara->redEyePb = REDEYEPB_OFF;
			/*cancel*/
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item == 0)
			{
				/*overwrite*/
			}
			else if(item == 1)
			{
				/*save*/
			}
			else
			{
				/*cancel*/
			}
			puiPara->redEyePb = REDEYEPB_OFF;
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;
		default:
			break;
	}

	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuResizeSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 size;
	PBFile_t fileInfo;
	layer=layer;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:

			if(0 == item)
			{
				size = PRV_SIZE_44M;
			}
			else if (1 == item)
			{
				size = PRV_SIZE_40M;
			}
			else if (2 == item)
			{
				size = PRV_SIZE_30M;
			}
			else if (3 == item)
			{
				size = PRV_SIZE_24M;
			}
			else if (4 == item)
			{
				size = PRV_SIZE_20M;
			}
			else if (5 == item)
			{
				size = PRV_SIZE_18M;
			}
			else if (6 == item)
			{
				size = PRV_SIZE_16M;
			}
			else if (7 == item)
			{
				size = PRV_SIZE_12M;
			}
			else if (8 == item)
			{
				size = PRV_SIZE_8M;
			}
			else if (9 == item)
			{
				size = PRV_SIZE_5M;
			}
			else if (10 == item)
			{
				size = PRV_SIZE_2M_16_9;
			}
			else if (11 == item)
			{
				size = PRV_SIZE_2M;
			}
			else if (12 == item)
			{
				size = PRV_SIZE_1M;
			}
			else
			{
				size = PRV_SIZE_VGA;
			}
			if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
			}
			else if (pbFile.bImageProtect == TRUE)
			{
				uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
			//	menuExit();
			//	ret = MENU_IGNORE_DEFAULT;
			}
			else if (sp1kDosGetDiskSpace() < 1000)
			{
				sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
				uiUpdateOSDMemFull();
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
	        	appUiBusyWait(250, 0); //xian move here for #38498, 20090801
	        	sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);

				if(appPbImgResize(&fileInfo, size) == SUCCESS)//xian ++ 20080703
				{
					pbFile.dwIndex = sp1kPbFileCntGet();
				}
				appUiBusyExit();
				//sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));

				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuVoiceMemoSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 time;
	UINT8 fName[12];
	UINT16 fHdl;
	layer=layer;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				break;
			}
			if (item == 0) // memo add
			{
				if (pbFile.bImageProtect == TRUE)
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
					//menuExit();
					//ret = MENU_IGNORE_DEFAULT;
				}
				else
				{
					sp1kAudioRecFreqSet(RECORD_FREQUENCE_1KHZ);  	//xian ++ 20090423, #32339
					uiUpdateAudRecRemainTime(&time);

					//30s max in fact, another 30s for reserved
					if (time >= 30) {
						//check if there is any voice memo belong the current image file
						strcpy(fName, pbFile.FileName);
						fName[8] = 'W';
						fName[9] = 'A';
						fName[10] = 'V';
						fHdl = vfsFileOpen(fName, K_DOS_ReadFile, 0);
						if (fHdl) {
							//delete old vioce memo file
							//CPrintf("delete old file");
							sp1kVfsFileDelete(fName);
							vfsFileClose(fHdl, fName, K_DOS_ReadFile, 0);
						}
			            //pbDZoomPause(); //xian ++ 20080624, fix bug #26147
			            pbCropDispBuffer2ABBuf();
						sp1kAudioRecTypeSet(AUDIO_TYPE_VOICE);
						//menuExit();
						osMsgPost(SP1K_MSG_STATE_TO_AUDIO_REC);
						ret = MENU_IGNORE_DEFAULT;
					}
					else
					{
						uiUpdateOSDMemFull();
					}
				}
			}
			else  // memo del
			{
				if (pbFile.bImageProtect != TRUE) {
					strcpy(fName, pbFile.FileName);
					fName[8] = 'W';
					fName[9] = 'A';
					fName[10] = 'V';

					sp1kVfsFileDelete(fName);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
				else
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
				}
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuStartImageSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 addr;
	UINT8 status;
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:

			sp1kPbUserStartImgGet(&addr);
			ENTER_CRITICAL(status);
			sp1kNandRsvWrite(0x800, addr << 1);
			EXIT_CRITICAL(status);
			//puiPara->OpenPic = PB_OPENPIC_USER_DEF;

			menuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}
#if (FUNC_HOST_PF_PB)
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuImgPhotoFrameInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:

			HAL_GprmCopy(
				 pbDispBuffGet(1), // Active buffer    //display show this
				 PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,
				 pbDispBuffGet(0), //inactive buffer   //store the source display data
				 PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,
				 PB_DECODE_WIDTH, PB_DECODE_HEIGHT,
				 0,
				 1
			);
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuImgPhotoFrameSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 currentItem;
	//printf("layer=%bu, item=%bu\n",layer, item);
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
		case SP1K_MSG_KEY_PRESS_LEFT:
			sp1kHalGprmCpy(pbDispBuffGet(0), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0, 	pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,PB_DECODE_WIDTH, PB_DECODE_HEIGHT,0, 1);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			currentItem = menuProcCurrItemGet(layer);
			if (currentItem != PRV_PHOTO_FRAME_OFF)
			{
				sp1kOsdClear(0);
				//sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);

				//appPbPhotoFrameSet(currentItem,0);
				//ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				menuProcDisp();
			}
			appPbPhotoFrameSet(currentItem,0);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			currentItem = menuProcCurrItemGet(layer);
			if (currentItem != PRV_PHOTO_FRAME_OFF)
			{
				sp1kOsdClear(0);
				//sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);

				//appPbPhotoFrameSet(currentItem,0);
				//ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				menuProcDisp();
			}
			appPbPhotoFrameSet(currentItem,0);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
			}
			else if (item != PRV_PHOTO_FRAME_OFF)
			{
				sp1kOsdClear(0);
				sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);

				if (sp1kDosGetDiskSpace() < 1000)
				{
					//sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
					uiUpdateOSDMemFull();
				}
				else
				{
					appPbPhotoFrameSet(item,1);
				}
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	return ret;
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuDateItemConv                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuDayItemCheck(UINT8 item, UINT16 key)
{
	UINT8 dayMax;
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->rtcMonth+1)
	{
		case 2:
		//modify for leep year problem @2008-04-11 wei.sun begin
		//if ((puiPara->rtcYear) && (!(puiPara->rtcYear & 0x03)))
		if (!(puiPara->rtcYear & 0x03))
		//modify for leep year problem @2008-04-11 wei.sun end
			dayMax = 28;
		else
			dayMax = 27;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (key == SP1K_MSG_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;

		case 4:
		case 6:
		case 9:
		case 11:
		dayMax = 29;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (key == SP1K_MSG_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;
	}
	if(puiPara->rtcYear == 7 && key == SP1K_MSG_KEY_PRESS_DOWN)
		puiPara->rtcYear = 30;
	if(puiPara->rtcYear == 0 && key == SP1K_MSG_KEY_PRESS_UP)
		puiPara->rtcYear = 8;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuRTCInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			sp1kHalReadRTC(&DATEINFO);
			puiPara->rtcDay=DATEINFO.Day-1;
			puiPara->rtcHour=DATEINFO.Hour;
			puiPara->rtcMinute=DATEINFO.Minute;
			puiPara->rtcMonth=DATEINFO.Month-1;
			puiPara->rtcYear=DATEINFO.Year;
			menuProcChild (rtc_init[puiPara->DateStyle]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuRTCSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_REPEATE_UP:
			if (item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}

		
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuDayItemCheck(item, SP1K_MSG_KEY_PRESS_UP);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_REPEATE_DOWN:
		case SP1K_MSG_KEY_REPEATE_PB:
			if (item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuDayItemCheck(item, SP1K_MSG_KEY_PRESS_DOWN);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_MENU:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcParent();
			menuProcChild (rtc_left[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcParent();
			menuProcChild (rtc_right[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			DATEINFO.Day=puiPara->rtcDay+1;
			DATEINFO.Hour=puiPara->rtcHour;
			DATEINFO.Minute=puiPara->rtcMinute;
			DATEINFO.Month=puiPara->rtcMonth+1;
			DATEINFO.Year=puiPara->rtcYear;
			sp1kHalWriteRTC(&DATEINFO);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuImagePhotoFrameSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			break;

		case SP1K_MSG_KEY_REPEATE_UP:
			if (item == 5)
				break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			menuProcParaSet(menuProcLayerGet(), menuProcCurrItemGet(layer));
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuDayItemCheck(item, SP1K_MSG_KEY_PRESS_UP);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_REPEATE_DOWN:
			if (item == 5)
				break;

 		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			menuProcParaSet(menuProcLayerGet(), menuProcCurrItemGet(layer));
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuDayItemCheck(item, SP1K_MSG_KEY_PRESS_DOWN);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:
			menuProcParent();
			menuProcChild (rtc_left[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcParent();
			menuProcChild (rtc_right[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			DATEINFO.Day=puiPara->rtcDay+1;
			DATEINFO.Hour=puiPara->rtcHour;
			DATEINFO.Minute=puiPara->rtcMinute;
			DATEINFO.Month=puiPara->rtcMonth+1;
			DATEINFO.Year=puiPara->rtcYear;
			sp1kHalWriteRTC(&DATEINFO);
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 

UINT8 menuSoundInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcChild(0);
			voicePlayFlag=FALSE;
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 UINT8 menuAFSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			switch (item) {
				case 0:
					puiPara->AFMode = 0;
					AF_Set(0);
					menuExit();
					ret = MENU_CALL_DEFAULT;
					break;
					
				case 1:
					puiPara->AFMode = 1;
					AF_Set(puiPara->AdjustMacro);//431);
					menuExit();
					ret = MENU_CALL_DEFAULT;
					break;

				case 2:
					puiPara->SnapAf = 1;
					menuExit();
					ret = MENU_CALL_DEFAULT;
					break;
			}
			break;
	}
	return ret;
}

UINT8 menuSoundSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
			voicePlayFlag=TRUE;
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:
	 		menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			appVoicePreview(item, menuProcCurrItemGet(layer));
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,((~(puiPara->ShutterSound))&0x01));
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_RIGHT:
	 		menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			appVoicePreview(item, menuProcCurrItemGet(layer));
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,((~(puiPara->ShutterSound))&0x01));
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_SNAP:
			voicePlayFlag=TRUE;
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


UINT8 menuLanguageInit (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 num=0;
	UINT8 itemIdx;
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring
	itemIdx = ITEM_EMPTY + (&puiPara->Language - &puiPara->EmptyPara);
	switch (msg)
	{
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if(FALSE==appUiItemOrderGet(itemIdx,specialMenuOrder,&specialMenuItemNum))
			{
				appUiItemStsSet(itemIdx,0);
				ret = MENU_CALL_DEFAULT;
				break;
			}
			appUiItemStsSet(itemIdx,1);
			while(specialMenuOrder[num] != puiPara->Language)
			{
				num++;
				if(num > specialMenuItemNum-1) {
					specialMenuCurrItemIdx = 0;
					break;
				}
			}
			specialMenuCurrItemIdx=num;

			menuProcChild(specialMenuOrder[specialMenuCurrItemIdx]);
			menuSpecialSequenceDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLanguageSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 itemIdx;
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	itemIdx = ITEM_EMPTY + (&puiPara->Language - &puiPara->EmptyPara);
	if(appUiItemStsGet(itemIdx)==FALSE)
	{
		switch(msg)
		{
			//case SP1K_MSG_KEY_PRESS_SNAP:
			case SP1K_MSG_KEY_PRESS_OK:
				menuProcParaSet(layer, item);
				sp1kOsdLoad();
				break;
			default:
				break;
		}
	}
	else
	{
		switch (msg)
		{
			//case SP1K_MSG_KEY_PRESS_SNAP:
			case SP1K_MSG_KEY_PRESS_OK:
				puiPara ->Language= specialMenuOrder[specialMenuCurrItemIdx];
				if(puiPara ->VideoSize != MOV_SIZE_1080P)
				{
					puiPara->FrameRate = MOV_FRM_MIDDLE;
				}
				else
				{
					puiPara->FrameRate = MOV_FRM_LOW;
				}
				sp1kOsdLoad();
				menuProcParent ();
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			case SP1K_MSG_KEY_PRESS_UP:
				menuSpecialMoveUp();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			//case SP1K_MSG_KEY_PRESS_DOWN:
			case SP1K_MSG_KEY_PRESS_PB:
				menuSpecialMoveDown();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
		}
	}

	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuTVModeSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			if(0 == appPanelTypeGet())//TV
			{
				appPanelStatusSet(0xFF);
			}
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuResetSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if((puiPara->ImageSize == PRV_SIZE_7M_16_9) || (puiPara->ImageSize == PRV_SIZE_2M_16_9) || (puiPara->ImageSize == PRV_SIZE_1M_16_9))
				imgResolutionSet = 1; // for mantis 44089
			sp1kBtnDisableAll();
			appCalShowWait();
			appUiParaInit(PARA_INIT_WITH_DEF);
			puiPara = appUiParaGet();
			if((appSDPlugStatusGet()==0) &&\
				(sp1kDiskTypeGet()==DEV_TYPE_DISK_NAND) &&\
				 (puiPara->VideoSize==MOV_SIZE_HD||puiPara->VideoSize==MOV_SIZE_1080P||puiPara==MOV_SIZE_D1))
			{//xiaolin.zhu

				puiPara->VideoSize = MOV_SIZE_VGA;
			}
#if FUNC_HOST_PF_PV
			appPvPhotoFrameSet(PRV_PHOTO_FRAME_OFF); //wangdong@2009.1.15 add  for mantis 0034652
#endif
			sp1kOsdLoad(); // Mantis 0034619
			appWBSet(puiPara->WBMode);
			appEffectSet(puiPara->ImageEffect);
			appEVSet(puiPara->AEMode);
			appISOSet(puiPara->Iso);
			appVolumeSet();
			sp1kBtnEnableAll();
			break;
	}
	return ret;
}
#if FUNC_HOST_DPS
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDPSResetSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appDpsParaRst();
			break;
	}
	return ret;
}

UINT8 menuDpsImageChoose (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	layer = layer;

	switch(msg){
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item != 2){
				DpsImageChooseFlag = 0;
				return ret;
			}
			menuExit();
			DpsImageChooseFlag = 1;
			break;
	}
	return ret;
}

UINT8 menuDpsCopyInit (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring
	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcChild(0);
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

extern UINT16 printTable[24][2];
UINT8 menuDpsCpoySet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 i;

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			ret = MENU_IGNORE_DEFAULT;
			break;
			
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;		

		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParent ();
			menuProcDisp();
			for(i = 0;i < 24;i ++){
				if(printTable[i][0] == pbFile.dwIndex){
					printTable[i][1] = puiPara->DPSCopies + 1;
					break;
				}
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

#endif
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuFrameRateSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	layer=layer;//just for clearing waring
	switch(msg)
	{
		case SP1K_MSG_KEY_PRESS_SNAP:
			if(item==0)
			{
				frameRateIsL=0;
			}
			else
			{
				frameRateIsL=1;
			}
			break;
	}
	return ret;
}
UINT8 menuMovieSizeInit(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT8 num=0;
	UINT8 itemIdx,movieSize;
	uiPara_t* puiPara = appUiParaGet();

	layer=layer;//just for clearing waring
	item=item;//just for clearing waring
	switch (msg)
	{
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if(appSDPlugStatusGet()==1) {
				itemIdx = ITEM_EMPTY + (&puiPara->VideoSize - &puiPara->EmptyPara);
				movieSize = puiPara->VideoSize;
			}
			else {
				itemIdx = ITEM_EMPTY + (&puiPara->VideoSize - &puiPara->EmptyPara);;
				movieSize = puiPara->VideoSizeNoCard;
			}

			if(FALSE==appUiItemOrderGet(itemIdx,specialMenuOrder,&specialMenuItemNum))
			{
				appUiItemStsSet(itemIdx,0);
				ret = MENU_CALL_DEFAULT;
				break;
			}
			appUiItemStsSet(itemIdx,1);
			while(specialMenuOrder[num] != movieSize)
			{

				num++;
				if(num > specialMenuItemNum-1) {
					specialMenuCurrItemIdx = 0;
					break;
				}
			}
			specialMenuCurrItemIdx=num;
			menuProcChild(specialMenuOrder[specialMenuCurrItemIdx]);
			menuSpecialSequenceDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;


}

UINT8 menuMovieSizeSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT8 itemIdx;
	layer=layer;//just for clearing waring
	if(appSDPlugStatusGet()==1) {
		itemIdx = ITEM_EMPTY + (&puiPara->VideoSize - &puiPara->EmptyPara);
	}
	else {
		itemIdx = ITEM_EMPTY + (&puiPara->VideoSizeNoCard- &puiPara->EmptyPara);;
	}
	if(appUiItemStsGet(itemIdx)==FALSE)
	{
		switch(msg)
		{
			//case SP1K_MSG_KEY_PRESS_SNAP:
			case SP1K_MSG_KEY_PRESS_OK:
				if(item != 0)
				{
					puiPara->FrameRate = MOV_FRM_MIDDLE;
				}
				else
				{
					puiPara->FrameRate = MOV_FRM_LOW;
				}
				break;
			default:
				break;
		}
	}
	else
	{
		switch(msg)
		{
			//case SP1K_MSG_KEY_PRESS_SNAP:
			case SP1K_MSG_KEY_PRESS_OK:
				puiPara ->VideoSize= specialMenuOrder[specialMenuCurrItemIdx];
				if(puiPara ->VideoSize != MOV_SIZE_1080P)
				{
					puiPara->FrameRate = MOV_FRM_MIDDLE;
				}
				else
				{
					puiPara->FrameRate = MOV_FRM_LOW;
				}
				menuProcParent ();
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			case SP1K_MSG_KEY_PRESS_UP:
				menuSpecialMoveUp();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
			//case SP1K_MSG_KEY_PRESS_DOWN:
			case SP1K_MSG_KEY_PRESS_PB:
				menuSpecialMoveDown();
				menuSpecialSequenceDisp();
				ret = MENU_IGNORE_DEFAULT;
				break;
		}

	}

	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuFirstItemProcess (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			imgResolutionSet = 1;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuLastItemProcess (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuFormatSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 sts;
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			
			if (item == 0 && appSDPlugStatusGet() == 0) {
				uiOsdDialogDisp(ID_STR_NO_CARD_, 1000);
			} else if(1 == appSDProtectStatusGet()) //card lock remainder
			{
				uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
			}
			else if (item == 0)
			{
				appUiBusyWait(250, 1);
				sp1kHalWait(60);
				#if SUPPORT_SPEAKER_CONTROL
				app_set_speaker(0);
				#endif
				sts = sp1kDiskFormat(0xC0);//format
				if(sts == TRUE){
					appCardErrFmtErrSet(0);
					sts = sp1kDiskInitialize(1);// initialization
				}
				else {
					appCardErrFmtErrSet(1);
				}
			
				
				appUiBusyExit();
				uiRemainPixInit();
				
				if(sts != TRUE)
				{
					appCardErrFmtErrSet(1);
					//uiOsdDialogDisp(ID_STR_CARD_ERROR, 10);
					uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD,10);
				}
				
				menuExit();
				
				ret = MENU_IGNORE_DEFAULT;
			}
			else if(appCardErrFmtChk())
			{
				appCardErrFmtErrSet(1);//if choice "no",define card as bad card
				uiOsdDialogDisp(ID_STR_CARD_ERROR, 10);
				menuExit();
				
				ret = MENU_IGNORE_DEFAULT;
			}
			
			appCardErrFmtSta(0);

			
			#if SUPPORT_SPEAKER_CONTROL
			app_set_speaker(1);
			#endif

			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuModeStill (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;

			break;
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		
		case SP1K_MSG_KEY_PRESS_BACK:
		case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
			
		
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuModeVideo (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		
		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BACK:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuModePlayback (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuModeDPS (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuModeAF (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuInit();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


UINT8 menuModeSetup (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BACK:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_PRESS_LEFT:
		case SP1K_MSG_KEY_PRESS_RIGHT:
			menuInit();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuModeCalibration(UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		#if FUNC_HOST_TOUCH_PANEL
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
		case SP1K_MSG_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

UINT8 menuVersionProcess(UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	switch(msg)
	{
   		//case SP1K_MSG_KEY_PRESS_SNAP :
   		case SP1K_MSG_KEY_PRESS_OK:
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent();
			menuProcDisp();
			break;

		//case SP1K_MSG_KEY_RELEASE_POWER:
		//	menuProcParent();
		//	menuProcDisp();
		//	break;

		case SP1K_MSG_KEY_REPEATE_POWER:
			// menuExit();
			appPowerOffProc();
			break;
			
		default:
			break;
	}

	return MENU_IGNORE_DEFAULT;
}


/**
 * @brief		appMenuEvent_VerShow
 * @param	NULL
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-11-11
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appMenuEvent_VerShow(void)
{
	UINT8 strVer[16] = "VER.";
	UINT8 strDate[16] = {0};
	UINT8 strTime[16] = {0};
	UINT8 OsdMaxX, OsdMaxY;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	strcpy(strVer + 4, sp1kFWVerGet());
	strcpy(strDate, sp1kFWDateGet());
	strcpy(strTime, sp1kFWTimeGet());

	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);

	sp1kOsdStrDraw((OsdMaxX - strlen(strVer)) >> 1, OsdMaxY >> 1, strVer, OSD_ATTR_COLOR_BLUE | OSD_ATTR_MODE_NOMAL | OSD_ATTR_BACK_EN);
	sp1kOsdStrDraw((OsdMaxX - strlen(strDate)), (OsdMaxY) - 2, strDate, OSD_ATTR_COLOR_BLUE| OSD_ATTR_MODE_NOMAL | OSD_ATTR_BACK_EN);
	sp1kOsdStrDraw((OsdMaxX - strlen(strTime)), (OsdMaxY) - 1 , strTime, OSD_ATTR_COLOR_BLUE | OSD_ATTR_MODE_NOMAL | OSD_ATTR_BACK_EN);

	return SUCCESS;
}
/**
 * @brief		menuIsoSet
 * @param	layer item msg
 * @retval	ret
 * @see
 * @author	Li tuhong
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 menuIsoSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	item=item;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_MENU:
		//case SP1K_MSG_KEY_PRESS_LEFT:
			appISOSet(puiPara->Iso);
			break;

		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			//item = menuProcCurrItemGet(layer);
			//printf("item=%bx\n",item);
			appISOSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcMoveDown();
			//item = menuProcCurrItemGet(layer);
			//printf("item=%bx\n",item);
			appISOSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}
/**
 * @brief		menuAntishakeSet
 * @param	layer item msg
 * @retval	ret
 * @see
 * @author	Li tuhong
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 menuAntishakeSet (UINT8 layer, UINT8 item, UINT16 msg) USING_0
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer=layer;//just for clearing waring
	switch(msg)
	{
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			if(0 == item)//fd open
			{
				puiPara->AntiShake = PRV_ANTISHAKE_ON;
			}
			else //fd off,force smile off
			{
				puiPara->AntiShake = PRV_ANTISHAKE_OFF;
			}
			appAntiShakeSet(puiPara->AntiShake);/*meili for antishake 20110302*/
			//printf("item=%bx\n",item);
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			//item = menuProcCurrItemGet(layer);
			//printf("item=%bx\n",item);
			appAntiShakeSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		//case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_PRESS_PB:
			menuProcMoveDown();
			//item = menuProcCurrItemGet(layer);
			//printf("item=%bx\n",item);
			appAntiShakeSet(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

	}
	return ret;
}

UINT8 menuModeUsbSet (UINT8 layer, UINT8 item, UINT16 msg)
{  
	uiPara_t* puiPara = appUiParaGet();
	UINT8 ret = MENU_CALL_DEFAULT;
	
  	switch (msg) 
	{
		case SP1K_MSG_KEY_PRESS_OK :
			if (item == 0) {
				if (appSDPlugStatusGet() == 1) {
					puiPara->USBMode=item;
					menuExit();
					osMsgPost(SP1K_MSG_STATE_TO_USB);
				} else {
					puiPara->USBMode=2;
					menuExit();
					user_exit_menu_flag=1;
				}				
			} else {
				puiPara->USBMode=item;
				menuExit();
				user_exit_menu_flag=1;
			}		
			ret = MENU_IGNORE_DEFAULT;
	   		break;

		
		case SP1K_MSG_USB_PLUG_OUT:
			puiPara->USBMode=2;
			menuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_CARD_PLUG_OUT:
		case SP1K_MSG_KEY_PRESS_MENU:
			// printf("presss ok:%bx\n",item);
			puiPara->USBMode=2;
			menuExit();
			user_exit_menu_flag=1;
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}


UINT8 menuJpgCrop (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	layer=layer;	//just for clearing waring
	switch (msg)
	{
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_CROP_THIS_PHOTO_);
			ret = MENU_IGNORE_DEFAULT;
			break;

		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_CROP_THIS_PHOTO_);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			JpgCropFlag=0;
			if (item == 0)//JPG crop
			{
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
					appFileDelHotKeySta(0);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
					break;
				}
				else if(pbFile.bImageProtect == TRUE)//img protected
				{
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
					appFileDelHotKeySta(0);
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
					break;
				}

				if (sp1kDosGetDiskSpace() < 1000)
				{
					sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
					uiUpdateOSDMemFull();
					menuExit();
					ret = MENU_IGNORE_DEFAULT;
				}
				else
				{
					appUiBusyWait(250, 0);
					appPbJpgCrop();
					appUiBusyExit();

					pbFile.dwIndex = sp1kPbFileCntGet();
					//sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
					ret = MENU_IGNORE_DEFAULT;
				}

			}
			else
			{
				menuExit();
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
	}
	return ret;
}

UINT8 menuSpeakSoundSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			#if SUPPORT_SPEAKER_CONTROL
			app_set_speaker(1);
			#endif
			
			puiPara->Volume = item ? VOLUME_10 : VOLUME_0;
			sp1kAudioVolumeSet(puiPara->Volume);
			break;
	}
	return ret;
}

UINT8 menuDateStampSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		//case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
			switch (item) {
				case 0:
					puiPara->StampMode = PRV_STAMP_OFF;
					puiPara->StampVideoMode = PRV_STAMP_OFF;
					break;
					
				//case 1:
				//	puiPara->StampMode = PRV_STAMP_DATE;
				//	puiPara->StampVideoMode = PRV_STAMP_DATE;
				//	break;

				case 1:
					puiPara->StampMode = PRV_STAMP_DATETIME;
					puiPara->StampVideoMode = PRV_STAMP_DATETIME;
					break;
			}
			break;
	}
	return ret;
}



UINT8 menuFlashLedSet(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;
	switch(msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			switch (item) {
			case 0:
				appSnapFlashLed(0);
				break;

			case 1:
				appSnapFlashLed(0);
				break;

			case 2:
				appSnapFlashLed(1);
				break;
			}
			break;
	}
	return ret;
}

