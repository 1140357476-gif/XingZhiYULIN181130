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
#include "initio.h"
#include "solution.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
//#include "doslink.h"
//#include "cardlink.h"
#include "hal_gprm.h"
#include "sdramhw.h"
#include "key_scan.h"
#include "copydisk.h"
#include "app_ui_osd.h"	 //lryy
#include "App_ui_para.h" //suny add
#include "app_still_play.h"
#include "app_menu_micro.h" //suny add
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"

#include "app_osd_icon.h"
#include "app_menu_tab.h"
#include "app_audio_rec.h"
#include "sp1k_pv_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_rsvblk_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_vfs_api.h"
#include "app_menu_core.h"//6-11
#include "cardlink.h"//6-11
#include "app_still_view.h"
#include "app_menu_event.h"
#include "smcfunc.h"
#include "Uiflow.h"
#include "os_custom.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "hal_dram.h"
#if (FUNC_HOST_DPS == 1)
#include "app_dps.h"
#endif
#include "app_init.h"
#include "dev_init.h"

#include "sp1k_disk_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_dcf_api.h"
#include "app_dev_disp.h"
#include "sp1k_gpio_api.h"
#include "dbg_def.h"
#include "hal_device.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define _PB_FADEIN_		0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT16 G_PlayBackPageCount;

SIMAGE xdata pbFile;
UINT16 pbZoomCurLog = 100;
UINT8 pbPreState = APP_STATE_STILL_VIEW;
UINT8 cur_idx = 0;
static UINT8 delete_key_hot_delete_sta = 0;
UINT8 delete_key_to_menu = 0;

UINT8 fast_delete_back = 0;
UINT8 pb_delete_flag = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern UINT16 xdata G_wValueLock;

#if MSG_AUTO_SEND
 extern UINT8 dele;
#endif
extern UINT8 xdata  storageReadyflag;
extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge	0:USB contect PC
extern UINT8 xdata cardplug;
extern UINT8 AutoTestMode;
#if SUPPORT_AUTO_TEST
extern UINT8 autoTestOpenFlag;
// 0 : Key And Usb Test
// 1 : Video Record Test
// 2 : Video Playback Test
extern UINT8 autoTestStatus; 
#endif


extern void app_quick_delete_dialog(UINT8 cur_idx);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
void app_delete_key_hot_delete_sta_set(UINT8 sta) {
	delete_key_hot_delete_sta = sta;
}

UINT8 app_delete_key_hot_delete_sta_get() {
	return delete_key_hot_delete_sta;
}


UINT8 appPbFolderUpdate(UINT8 forward);
/**
 * @brief		seek abortKey,then post abortkey to show next
 * @param	NONE
 * @retval	NONE
 * @see		NONE
 * @author	hy.ren
 * @since		2011-03-03
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void pbAbortMsgSeek(){
	btnMsgList_t abortKey;
	if(osMsgSeek(SP1K_MSG_KEY_PRESS_LEFT)){
		abortKey = SP1K_MSG_KEY_PRESS_LEFT;
	}
	else{
		abortKey = SP1K_MSG_KEY_PRESS_RIGHT;
	}
	if(osMsgSeek(SP1K_MSG_KEY_RELEASE_RIGHT) ||osMsgSeek(SP1K_MSG_KEY_RELEASE_LEFT))
	{
		osMsgFlush(MSG_CTRL_FLUSH_BTN);
	}
	osMsgPost(abortKey);
}

//-----------------------------------------------------------------------------
//appStillPlay
//-----------------------------------------------------------------------------
/**
 * @brief		still play mode
 * @param	receive msg to still play mode
 * @retval	NONE
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appStillPlay (UINT16 msg) USING_0
{
	UINT8 InitMode;
	UINT8 LastMode;
//	UINT16 newFact, pbZoomFact;
	UINT8 playsts;
//	powerBatLv_t batlv;
	uiPara_t* puiPara ;

	puiPara = appUiParaGet();
	
	switch (msg) {
	case SP1K_MSG_STATE_INIT:
		if (puiPara->SpeakSound) {
			sp1kAudioVolumeSet(puiPara->Volume);
		}
		
		if ( cardplug ) {
			//CPrintf("still playback init\n");
			cardplug = 0;
			break;
		}
		
		LastMode = appPreStateGet(0);
		
		switch (LastMode) {
		case APP_STATE_MENU:
		case APP_STATE_PAN_SHOW:
		case APP_STATE_SLIDE_SHOW:
		case APP_STATE_AUDIO_PLAY:
		case APP_STATE_MUTI_PLAY:
			InitMode = SP1K_PB_INIT_FULL;//SP1K_PB_INIT_SMART;
			appPbPreStateSet(LastMode);
			break;
			
		case APP_STATE_STILL_VIEW:
		case APP_STATE_VIDEO_VIEW:
			InitMode = SP1K_PB_INIT_FULL;
			appPbPreStateSet(LastMode);
			break;
			
		case APP_STATE_AUDIO_REC:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				InitMode = SP1K_PB_INIT_SMART;
			} else {
				InitMode = SP1K_PB_INIT_FULL;
				appPbPreStateSet(LastMode);
			}
			break;
			
		default:
			InitMode = SP1K_PB_INIT_FULL;
			break;
		}
		
		if(pbFile.dwIndex == 0xffff){    //xian ++, invalid index for #31928
			InitMode = SP1K_PB_INIT_FULL;
			pbFile.dwIndex = sp1kPbFileCntGet();
		}

		if (appStillPlayInit(InitMode) == FAIL)
		{
			//printf("\n1\n");
			pbFile.bFileErr = TRUE;
			sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);//2008-7-28 mantis #28190 step 1
			if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE)
			{
				uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
			}
			else
			{
				uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
			}
		}
		//printf("\n2\n");
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(0);
		}*/

		appAutoOffTimeSet();//jintao.liu 2008-3-28 add for mantis 21604
/***************Just for QA test @start***************************************/
		if(AutoTestMode == 0xAA)
		{
			appFileDelete(SP1K_FILE_PROC_ALL);
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_NAND)) {

					static UINT8 VSize = MOV_SIZE_D1;
					VSize ++;
					if (VSize == MOV_SIZE_MAX) {
						VSize = MOV_SIZE_VGA;
					}
					puiPara->VideoSize=VSize;

			}
			else {
				uiPara_t* puiPara;
				puiPara = appUiParaGet();
				puiPara->VideoSize++;
				if (MOV_SIZE_MAX== puiPara->VideoSize)
				{
					puiPara->VideoSize = MOV_SIZE_HD;
				}
			}
			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			break;
		}
		else if(AutoTestMode == 0x55)
		{
			appFileDelete(SP1K_FILE_PROC_ALL);
			osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			break;
		}
/***************Just for QA test @end***************************************/

		#if MSG_AUTO_SEND
		sp1kHalWait(2000);
		if(dele)
		{
			sp1kHalWait(1000);
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
		}
		else
		{
			osMsgPost(SP1K_MSG_KEY_PRESS_SNAP);
		}
		#endif

		#if SUPPORT_AUTO_TEST
		if (autoTestOpenFlag) {
			if (2 == autoTestStatus) {
				osMsgPost(SP1K_MSG_KEY_PRESS_SNAP);
			} else if (3 == autoTestStatus) {
				appButtonAutoTest();
			}
		}
		#endif
		break;
		
	case SP1K_MSG_KEY_PRESS_S1:
		//printf("S1 on still playback\n");
		break;
		
	case SP1K_MSG_KEY_PRESS_S2:
		//printf("S2\n");
		break;

	
	#if 0
	case SP1K_MSG_KEY_PRESS_MENU:
		if (app_delete_key_hot_delete_sta_get()) {
			// 清除当前选择索引
			cur_idx = 0;

			// 清除删除标志
			app_delete_key_hot_delete_sta_set(0);

			// 退出对话框，发送INIT消息回到STILL_PLAY
			// osMsgPost(SP1K_MSG_STATE_INIT);
			appPbOsdInit();
			//pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
			appStillPlayInit(SP1K_PB_INIT_SMART);
		} else {
			osMsgPost(SP1K_MSG_SYS_QUICK_DELETE);
		}
		break;
	#endif

	//case SP1K_MSG_KEY_PRESS_AF:
	//case SP1K_MSG_KEY_PRESS_MENU:
	//case SP1K_MSG_KEY_PRESS_PB:
	case SP1K_MSG_KEY_PRESS_DOWN:
		if(pb_delete_flag){
			pb_delete_flag = 0;
		}else{
			pb_delete_flag = 1;
		}

	case SP1K_MSG_SYS_QUICK_DELETE:
		if (app_delete_key_hot_delete_sta_get()) {
			// 清除当前选择索引
			cur_idx = 0;

			// 清除删除标志
			app_delete_key_hot_delete_sta_set(0);

			// 退出对话框，发送INIT消息回到STILL_PLAY
			// osMsgPost(SP1K_MSG_STATE_INIT);
			appPbOsdInit();
			//pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
			appStillPlayInit(SP1K_PB_INIT_SMART);
			break;
		}
		
		delete_key_to_menu = 1;
		#if 0
		if(dispDevActGet() != 4) {//HDMI do not support menu
			pbZoomCurLog = PLAY_MIN_ZOOM_FACT;
			pbCropDispBuffer2ABBuf();
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
		}
		#else
		if (!sp1kSD_CdGet()) {
			if (sp1kPbFileCntGet() != 0) {
				app_delete_key_hot_delete_sta_set(1);
				app_quick_delete_dialog(cur_idx);
			}
		}
		#endif
		break;
		
	#if 0
	case SP1K_MSG_KEY_REPEATE_UP:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
		uiUpdateSelFrame(SEL_FRAME_MOVE_UP | SEL_FRAME_STEP);
		break;
		}*/
		if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
			osMsgPost(SP1K_MSG_STATE_TO_PAN_SHOW);
		}
		break;
		
	
	case SP1K_MSG_KEY_REPEATE_DOWN:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_MOVE_DOWN | SEL_FRAME_STEP);
			break;
		}*/
		if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
			osMsgPost(SP1K_MSG_STATE_TO_PAN_SHOW);
		}
		break;
		#endif
		
	case SP1K_MSG_KEY_REPEATE_LEFT:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_MOVE_LEFT | SEL_FRAME_STEP);
			break;
		}*/
		osMsgFlush(MSG_CTRL_FLUSH_BTN);
		playsts = appStillPlayNext(0);
		if(playsts != SUCCESS)
		{
			if(playsts == PBABORT)
			{
				pbAbortMsgSeek();
				break;
			}
		else
		{
			pbFile.bFileErr = TRUE;
			if(sp1kPbFileCntGet())
			{
				if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
					uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
				}
				else{
					uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
				}

			}
			else{
				uiOsdDialogDisp(ID_STR_NO_FILE, 0);
			}
		 }

		}
		if(osMsgSeek(SP1K_MSG_KEY_RELEASE_LEFT) == TRUE)
		{
			if(sp1kpbDZoomKeyGet()==1){
				osMsgFlush(MSG_CTRL_FLUSH_BTN);
			}
		}
		break;

	//case SP1K_MSG_KEY_PRESS_UP:
	//case SP1K_MSG_KEY_PRESS_DOWN:
	//case SP1K_MSG_KEY_PRESS_PB:
	case SP1K_MSG_KEY_PRESS_BACK:
		if (app_delete_key_hot_delete_sta_get()) {
			cur_idx = !cur_idx;
			app_quick_delete_dialog(cur_idx);
			break;
		}
		
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_MOVE_LEFT | SEL_FRAME_STEP);
			break;
		}*/

		if (sp1kPbFileCntGet()) {
			if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
				osMsgPost(SP1K_MSG_STATE_TO_PAN_SHOW);
				break;
			}
		}
		osMsgFlush(MSG_CTRL_FLUSH_BTN);
		playsts = appStillPlayNext(0);
		if(playsts != SUCCESS)
		{
			if(playsts == PBABORT)
			{
				pbAbortMsgSeek();
				break;
			}
			else
			{
				pbFile.bFileErr = TRUE;
			  	if(sp1kPbFileCntGet())
			  	{
					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
						uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
					}
					else{
						uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
					}

			  	}
			  	else{
					uiOsdDialogDisp(ID_STR_NO_FILE, 0);
			  	}
		       }
		}
		if(osMsgSeek(SP1K_MSG_KEY_RELEASE_LEFT) == TRUE)
		{
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
		}
		break;

	case SP1K_MSG_KEY_REPEATE_RIGHT:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_MOVE_RIGHT | SEL_FRAME_STEP);
			break;
		}*/
		osMsgFlush(MSG_CTRL_FLUSH_BTN);
		playsts = appStillPlayNext(1);
		if(playsts != SUCCESS)
		{
			if(PBABORT == playsts)
			{
				pbAbortMsgSeek();
				break;
			}
			else
			{
			pbFile.bFileErr = TRUE;
			if(sp1kPbFileCntGet())
			{
				if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
					uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
				}
				else{
					uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
				}

			}
			else{
				uiOsdDialogDisp(ID_STR_NO_FILE, 0);
			}
			}
		}
		if(osMsgSeek(SP1K_MSG_KEY_RELEASE_RIGHT) == TRUE)
		{
			if(sp1kpbDZoomKeyGet()==1)
			{
				osMsgFlush(MSG_CTRL_FLUSH_BTN);
			}
		}
		break;

	//case SP1K_MSG_KEY_PRESS_DOWN:
	case SP1K_MSG_KEY_PRESS_BURST:
	case SP1K_MSG_KEY_PRESS_PB:
		if (app_delete_key_hot_delete_sta_get()) {
			cur_idx = !cur_idx;
			app_quick_delete_dialog(cur_idx);
			break;
		}
		
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_MOVE_RIGHT | SEL_FRAME_STEP);
			break;
		}*/
		if (sp1kPbFileCntGet()) {
			if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
				osMsgPost(SP1K_MSG_STATE_TO_PAN_SHOW);
				break;
			}
		}
		osMsgFlush(MSG_CTRL_FLUSH_BTN);
		playsts = appStillPlayNext(1);
		if(playsts != SUCCESS)
		{
			if(PBABORT == playsts)
			{
				pbAbortMsgSeek();
				break;
			}
			 else
			 {
				pbFile.bFileErr = TRUE;
				if(sp1kPbFileCntGet())
				{
					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE)
					{
						uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
					}
					else
					{
						uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
					}

				}
				else
				{
					uiOsdDialogDisp(ID_STR_NO_FILE, 0);
				}
		  	}
		}
		
		if(osMsgSeek(SP1K_MSG_KEY_RELEASE_RIGHT) == TRUE)
		{
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
		}
		break;

	case SP1K_MSG_KEY_PRESS_OK:
		if (app_delete_key_hot_delete_sta_get()) {
			if (!cur_idx) {
				// 删除当前文件
				
				if(1 == appSDProtectStatusGet()) //card lock remainder
				{
					// 提示写保护
					uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
				}
				else if(pbFile.bImageProtect == TRUE)
				{
					// 提示文件只读
					uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 1000);
				}

				appFileDelete(SP1K_FILE_PROC_ONE);

				// 清除当前选择索引
				cur_idx = 0;

				// 清除删除标志
				app_delete_key_hot_delete_sta_set(0);

				// 退出对话框，发送INIT消息回到STILL_PLAY
				// osMsgPost(SP1K_MSG_STATE_INIT);
				appPbOsdInit();

				fast_delete_back = 1;

				// 180723:1 linziyang begin
				// pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
				// appStillPlayInit(SP1K_PB_INIT_FULL);

				appStillPlayInit(SP1K_PB_INIT_SMART);
				// end
			}else {
				// 清除当前选择索引
				cur_idx = 0;

				// 清除删除标志
				app_delete_key_hot_delete_sta_set(0);

				// 退出对话框，发送INIT消息回到STILL_PLAY
				// osMsgPost(SP1K_MSG_STATE_INIT);
				appPbOsdInit();
				
				fast_delete_back = 1;

				//pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
				appStillPlayInit(SP1K_PB_INIT_SMART);
			}
			break;
		}
		break;

		case SP1K_MSG_KEY_PRESS_AF:
		case SP1K_MSG_KEY_PRESS_SNAP:
		if (sp1kPbFileCntGet() != 0)
		{
			if ((pbFile.wType == SP1K_PB_FILE_TYPE_AVI) || (pbFile.wType == SP1K_PB_FILE_TYPE_WGI))
			{
				osMsgPost(SP1K_MSG_STATE_TO_VIDEO_PLAY);
				appHdmiOsdSet(0);
			}
			else if (pbFile.wType == SP1K_PB_FILE_TYPE_WAV)
			{
				appHdmiOsdSet(0);
				osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
			}
			else if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG )
			{
				if (pbFile.bVoiceMemo == TRUE)
				{
					//fixed bug when zoom in, the buffer isn't enough
//						if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
						pbZoomCurLog = PLAY_MIN_ZOOM_FACT;
//						}
					//pbDZoomPause();
					pbCropDispBuffer2ABBuf(); //modified by rhy 20110412
					osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
				} else {
					if (!sp1kPbFileCntGet() || pbFile.wType != SP1K_PB_FILE_TYPE_JPG || pbFile.bFileErr == TRUE)
					{
						break;
					}
				}
				
				#if MSG_AUTO_SEND
				osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
				#endif
			}
		}
		break;
	
	case SP1K_MSG_KEY_PRESS_ZOOM_IN:
	case SP1K_MSG_KEY_REPEATE_ZOOM_IN:
	case SP1K_MSG_KEY_PRESS_TELE:
	case SP1K_MSG_KEY_REPEATE_TELE:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_ZOOM_IN | SEL_FRAME_STEP);
			break;
		}*/

		if (!sp1kPbFileCntGet() || pbFile.wType != SP1K_PB_FILE_TYPE_JPG || pbFile.bFileErr == TRUE)
		{
			break;
		}

		if(pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
		{
			//osMsgPost(SP1K_MSG_STATE_TO_PAN_SHOW);
			//osMsgPost(SP1K_MSG_KEY_REPEATE_TELE);//avoid beep sound
		}
		#if 0
		pbZoomCurLog -= PLAY_ZOOM_logStep;

		if (pbZoomCurLog > PLAY_MAX_ZOOM_LOG)
			pbZoomCurLog = PLAY_MAX_ZOOM_LOG;
		//printf("pbZoomCurLog=%u,PLAY_ZOOM_logStep=%bu,newFact=%u\n",pbZoomCurLog,PLAY_ZOOM_logStep,newFact);
		newFact = 10000/pbZoomCurLog;
		if(newFact <= PLAY_MAX_ZOOM_FACT) {
			pbZoomFact = newFact;
		}
		else {
			pbZoomCurLog = PLAY_MIN_ZOOM_LOG;
			pbZoomFact = PLAY_MAX_ZOOM_FACT;
			uiUpdateOSDPvZoom(pbZoomFact, 2); //lryy modify
			return;
		}
		//printf("fac:%d\n",pbZoomFact);
		sp1kPbDZoomFactorSet(pbZoomFact);
		uiUpdateOSDPvZoom(pbZoomFact, 2); //lryy modify
		#endif
		break;

	case SP1K_MSG_KEY_PRESS_ZOOM_OUT:
    case SP1K_MSG_KEY_PRESS_WIDE:
		if(puiPara->redEyePb == REDEYEPB_ON)
		{
			break;
		}

		if(pbZoomCurLog == PLAY_MIN_ZOOM_FACT && sp1kPbFileCntGet())
		{ //xian ++ #42006
			//switch to multi playback
			//osMsgPost(SP1K_MSG_STATE_TO_MULTI_PLAY);
		}
		break;

	
	case SP1K_MSG_KEY_REPEATE_ZOOM_OUT:
	case SP1K_MSG_KEY_RELEASE_ZOOM_OUT:
	case SP1K_MSG_KEY_RELEASE_WIDE:
	case SP1K_MSG_KEY_REPEATE_WIDE:
		/*if(puiPara->redEyePb == REDEYEPB_ON)
		{
			uiUpdateSelFrame(SEL_FRAME_ZOOM_OUT | SEL_FRAME_STEP);
			break;
		}*/

		if (!sp1kPbFileCntGet() || pbFile.wType != SP1K_PB_FILE_TYPE_JPG || pbFile.bFileErr == TRUE)
		{
			break;
		}
		
		#if 0
		pbZoomCurLog += PLAY_ZOOM_logStep;

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
			uiUpdateOSDPvZoom(pbZoomFact, 2);
			//switch to multi playback
			//osMsgPost(SP1K_MSG_STATE_TO_MULTI_PLAY);
			break;
		}
//			printf("fac:%d\n",pbZoomFact);
		sp1kPbDZoomFactorSet(pbZoomFact);
		uiUpdateOSDPvZoom(pbZoomFact, 2);//lryy modify
		#endif
		//printf("dzf = %u\n", dzmFacTbl[pbFile.dzFactor]);
		//printf("tele\n");
		break;
		
    case SP1K_MSG_KEY_PRESS_DISP:
		break;
		
    case SP1K_MSG_KEY_PRESS_F1:
		if (sp1kPbFileCntGet() != 0) {
			if ((pbFile.wType == SP1K_PB_FILE_TYPE_AVI) || (pbFile.wType == SP1K_PB_FILE_TYPE_AVI)) {
				osMsgPost(SP1K_MSG_STATE_TO_VIDEO_PLAY);
			} else if (pbFile.wType == SP1K_PB_FILE_TYPE_WAV) {
				osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
			} else if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG ) {
				if (pbFile.bVoiceMemo == TRUE) {
					//fixed bug when zoom in, the buffer isn't enough
//						if (pbZoomCurLog != PLAY_MIN_ZOOM_FACT) {
						pbZoomCurLog = PLAY_MIN_ZOOM_FACT;
//						}
					pbDZoomPause();  //xian ++ 20080624, fix bug #26515
					osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
				}
			}
		}
		break;
		
    case SP1K_MSG_KEY_PRESS_F2:
		//pbImgResize(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), 2592, 1944);
		break;
	
	case SP1K_MSG_KEY_PRESS_MODE:
		if(fast_delete_back == 1){
			switch (appPreStateGet(0))
				{
					case APP_STATE_STILL_VIEW:
						msg = SP1K_MSG_STATE_TO_STILL_VIEW;
						break;
						
					case APP_STATE_VIDEO_VIEW:
						msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
						break;
					case APP_STATE_STILL_PLAY:
						msg = SP1K_MSG_STATE_TO_STILL_PLAY;
						break;
					
					default:
						msg = SP1K_MSG_STATE_TO_STILL_VIEW;
						break;
				}
				
				osMsgPost(msg);

				fast_delete_back = 0;
		}else{
		if (app_delete_key_hot_delete_sta_get()) {
				if(pb_delete_flag){
					// 清除当前选择索引
					cur_idx = 0;
	
					// 清除删除标志
					app_delete_key_hot_delete_sta_set(0);

					pb_delete_flag = 0;
					
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				}else{
					
					// 清除当前选择索引
						cur_idx = 0;
		
						// 清除删除标志
						app_delete_key_hot_delete_sta_set(0);
					//menuExit();
					switch (appPreStateGet(0))
					{
						case APP_STATE_STILL_VIEW:
							msg = SP1K_MSG_STATE_TO_STILL_VIEW;
							break;
							
						case APP_STATE_VIDEO_VIEW:
							msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
							break;
						//case APP_STATE_STILL_PLAY:
						//	msg = SP1K_MSG_STATE_TO_STILL_PLAY;
						//	break;
						
						default:
							msg = SP1K_MSG_STATE_TO_STILL_VIEW;
							break;
					}
					
					osMsgPost(msg);

				}
				
			}else
				{
					#if(TV_OPTION == TV_PLAY_OPTION)
					if(appPanelTypeGet() == 0) {
						return;
					}
					#endif
					
					if(dispDevActGet() == 4) {//HDMI in do not suport
						break;
					}
					
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;//SP1K_MSG_STATE_TO_MENU
					delete_key_to_menu = 0;
									
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
				}
			}
		break;
	
		
	/*jintao.liu 2008-3-28 add for mantis 22007*/
	case SP1K_MSG_CARD_PLUG_IN:
		appStorageMount(K_MEDIA_SDF);
		uiRemainPixInit();
		//appPbOsdInit();
		pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
		osMsgPost(SP1K_MSG_STATE_INIT);
		break;
		
	case SP1K_MSG_CARD_PLUG_OUT:
		if (app_delete_key_hot_delete_sta_get()) {
			// 清除当前选择索引
			cur_idx = 0;

			// 清除删除标志
			app_delete_key_hot_delete_sta_set(0);
		}
		
		//add for mantis #36936 @start
		if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
		{
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			sp1kDispImgWinEn(0);
			//sp1kHalCtrlScreen(2);
			sp1kOsdClear(0);
			uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
		}
		else
		{
			
			sp1kBtnEnableAll();
			appStorageMount(K_MEDIA_NANDF);
			uiRemainPixInit();
			appPbOsdInit();
			pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
			osMsgPost(SP1K_MSG_STATE_INIT);
		}
		break;
	/*jintao.liu 2008-3-28 add for mantis 22007 end*/
	
	case SP1K_MSG_USB_PLUG_IN:
		if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
			{
				break;
			}
			#if 0
				if (storageReadyflag==SUCCESS) {
					if (USB_DBG) {
						sp1kHalUSBSwPlugIn();
					} else if( puiPara->USBMode == USB_SIDC){
						osMsgPost( SP1K_MSG_STATE_TO_DPS );
					}
					else {
					osMsgPost(SP1K_MSG_STATE_TO_USB);
				       }
				}
			#endif
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;
		
	case SP1K_MSG_USB_PLUG_OUT:
		// osMsgPost(SP1K_MSG_POWER_OFF);
		break;
		
    case SP1K_MSG_TV_PLUG_IN:
	case SP1K_MSG_TV_PLUG_OUT:
		pbZoomCurLog = 100;
		appDispSet();
		break;
		
	case SP1K_MSG_HDMI_PLUG_IN:
		pbZoomCurLog = 100;
		appDispHdmiOpen();
		osMsgPost(SP1K_MSG_STATE_INIT);
		sp1kHalCtrlScreen(3);
		break;
		
	case SP1K_MSG_HDMI_PLUG_OUT:
		pbZoomCurLog = 100;
		sp1kPbDecodeSizeSet(PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		//add for #41757
		appPanelStatusSet(PANEL_STATUS_LCD);
		appDispSet();
		if(sp1kTV_PlugGet() ==0){
			if ( puiPara->TVMode == TVMODE_NTSC ) {
				appPanelStatusSet(PANEL_STATUS_NTSC);
			}else if ( puiPara->TVMode == TVMODE_PAL ) {
				appPanelStatusSet(PANEL_STATUS_PAL);
			}
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
		}
		break;
		
	case SP1K_MSG_BAT_LV_UPDATE:
		//uiUpdateBatteryLevel();
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
		
	case SP1K_MSG_NAND_ERROR:
		uiOsdDialogDisp(ID_STR_CARD_ERROR, 0);
	case SP1K_MSG_CARD_ERROR:
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		if(sp1kDiskErrorCodeGet() == DISK_ERR_DETECT)
		{
			uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD,1000);
		} else if(sp1kSD_CdGet()) {	//mantis 43151
			uiOsdDialogDisp(ID_STR_NO_CARD_,1000);


		}else
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

//-----------------------------------------------------------------------------
//appStillPlayInit
//-----------------------------------------------------------------------------
/**
 * @brief		still play mode initialization
 * @param	initmode: smart / full
 * @retval	NONE
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
extern void gprmBlackFill(UINT32 dstAddr, UINT16 imgW, UINT16 imgH);
UINT8 appStillPlayInit(UINT8 InitMode) USING_0
{
	devAttr_t* devAttr;
	UINT16 cardMsg = 0;
	UINT8 LastState;
	UINT8 ret;
#if FUNC_HOST_DPS
	UINT32 lastJpgIdx;
#endif
	ret = SUCCESS;

	//xian ++ 20100914, #41098
	devAttr = sp1kDevHdlGet();
	if (G_Card_Type == K_MEDIA_SDF) {
		devAttr += DEV_ID_CARD;
	} else {
		devAttr += DEV_ID_DISK;
	}

	if(G_Card_Type == K_MEDIA_SDF && sp1kSD_CdGet() ){//#40925
		cardMsg = SP1K_MSG_CARD_PLUG_OUT;
	}
	else if(devAttr->devSts == DEV_STS_ERR){//#41098
		cardMsg = SP1K_MSG_CARD_ERROR;
	}

	if(cardMsg)
	{
		gprmBlackFill(sp1kPbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);
		sp1kDispImgWinEn(1);
		osMsgPost(cardMsg);
		appHdmiOsdSet(1);
		return ret;
	}
	//xian --


	if (InitMode == SP1K_PB_INIT_SMART)
	{
		//Smart init
		if (sp1kPbFileCntGet() == 0)
		{
			gprmBlackFill(sp1kPbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
			sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);

			sp1kDispImgWinEn(1); // hlc, Mantis #0020853
			uiOsdDialogDisp(ID_STR_NO_FILE, 0);
			appHdmiOsdSet(1);
			return ret;
		}

		if(pbFile.dwIndex>sp1kPbFileCntGet())
		{
			pbFile.dwIndex =sp1kPbFileCntGet();
		}
#if FUNC_HOST_DPS
		if((appCurrStateGet() == APP_STATE_DPS)&&(appPreStateGet(0)!=APP_STATE_MENU))
		{
			appDpsCheckFileNum(&lastJpgIdx);
			if(lastJpgIdx){
				pbFile.dwIndex = lastJpgIdx;
			}else{
				pbFile.dwIndex = 1;
			}

		}
#endif
		appPbFileInfoUpdate();//add by hy.ren 20110301 ,avoid some unkonw bug
#if FUNC_HOST_PF_PB
		BACKUP_DISPBUFF();
#endif
		sp1kOsdClear(0);
		appPbOsdInit();
		if(pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
		{
			ret = sp1kPbSingleImgPlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
		}
		else if((pbFile.wType == SP1K_PB_FILE_TYPE_AVI) || (pbFile.wType == SP1K_PB_FILE_TYPE_WGI))
		{
			ret = sp1kPbVideoFramePlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
		}
		else if(pbFile.wType == SP1K_PB_FILE_TYPE_WAV)
		{
			ret = appAudioPlayBGShow();

		}
		sp1kDispImgWinEn(1); // hlc, Mantis #0020853
		appHdmiOsdSet(1);
		return ret;
	}
	
	if(pbFile.dwIndex>sp1kPbFileCntGet())
	{
		pbFile.dwIndex =sp1kPbFileCntGet();
	}  //add by rhy 20110301 avoid  some unknow bug
	
#if FUNC_HOST_DPS
	if((appCurrStateGet()== APP_STATE_DPS)&&(appPreStateGet(0)!=APP_STATE_MENU))
	{
		appDpsCheckFileNum(&lastJpgIdx);
		if(lastJpgIdx){
			pbFile.dwIndex = lastJpgIdx;
		}else{
			pbFile.dwIndex = 1;
		}

	}
#endif

	appPbFileInfoUpdate();

	#if 1//jintao.liu 2008-5-21 add for mantis 24629
	sp1kDispImgWinEn(0);
	sp1kPvPhotofrmEnable(0);
	#endif
	// hlc, turn off 3A at play mode
	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbDisable();

	/*SP1K_PB_INIT_FULL*/
	sp1kPbInit();
	sp1kPbFileOrdertableInit(); //rhy++ mantis bug #42023
	sp1kPbParamSet(PB_PARAM_AUDIO_RES_ID, 0x15);
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if 1//TAG_TODO
	//sp1kPbDistortParamSet(0,0,PB_DECODE_WIDTH,PB_DECODE_HEIGHT);
	//sp1kSpeParamSetCropping(0,0,PB_DECODE_WIDTH,PB_DECODE_HEIGHT);
	//sp1kSpeParamSetDstSize(0,0);
	//sp1kSpeParamSetMirrorIdx(0);
	sp1kSpeBuffSizeSet(0x9A000);//
	sp1kSpeParamSet(SPE_PARAM_H_SIZE,PB_DECODE_WIDTH);
	sp1kSpeParamSet(SPE_PARAM_V_SIZE,PB_DECODE_HEIGHT);
	sp1kSpeParamSet(SPE_PARAM_H_OFF,0);
	sp1kSpeParamSet(SPE_PARAM_V_OFF,0);
	sp1kSpeParamSet(SPE_PARAM_H_SIZE_DST,0);
	sp1kSpeParamSet(SPE_PARAM_V_SIZE_DST,0);
	sp1kSpeParamSet(SPE_PARAM_MIRROR_INDEX,0);

	sp1kPbParamSet(PB_PARAM_SPLAY_MODE,PB_SPLAY_MODE_MAIN);
#endif //#if TAG_TODO
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
	sp1kPbParamSet(PB_PARAM_SPLAY_MODE,PB_SPLAY_MODE_MAIN|PB_SPLAY_MODE_ZOOM1X);
    //xian ++ 20080703
	sp1kPbParamSet(PB_PARAM_MT_LOCK,  PB_ICON_RIGHT | PB_ICON_TOP | 0x2a);
	sp1kPbParamSet(PB_PARAM_MT_MEMO,  PB_ICON_LEFT | PB_ICON_BOTTOM | 0x2b);
	sp1kPbParamSet(PB_PARAM_MT_VIDEO, PB_ICON_LEFT | PB_ICON_BOTTOM | 0x2c);
    //xian --
	sp1kPbParamSet(PB_PARAM_FILE_ERR_RES, 0x2f);


	if (sp1kPbFileCntGet() == 0) {

		//No image process
		gprmBlackFill(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);

		sp1kDispImgWinEn(1); // hlc, Mantis #0020853z
		uiOsdDialogDisp(ID_STR_NO_FILE, 0);
		appHdmiOsdSet(1);
		return ret;
	}

	//If file exists in this directory
	//2008-4-3 mantis #22397
	LastState = appPreStateGet(0);
	if(LastState != APP_STATE_VIDEO_PLAY && LastState != APP_STATE_AUDIO_PLAY && LastState != APP_STATE_MUTI_PLAY) {
		pbFile.dwIndex = sp1kPbFileCntGet();
#if FUNC_HOST_DPS
		if((appCurrStateGet()== APP_STATE_DPS)&&(appPreStateGet(0)!=APP_STATE_MENU))
		{
			appDpsCheckFileNum(&lastJpgIdx);
			if(lastJpgIdx){
				pbFile.dwIndex = lastJpgIdx;
			}else{
				pbFile.dwIndex = 1;
			}
		}
#endif

	}
	appPbFileInfoUpdate();

#if 1
//	printf("Play->%s\n", pbFile.FileName);
	//OSD display  move up for mantis #41375
	sp1kOsdClear(0);
	appPbOsdInit();
	if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG) {
		//printf("JPG\n");
		ret = sp1kPbSingleImgPlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if FUNC_HOST_PF_PB
		BACKUP_DISPBUFF();
#endif
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
	} else if ((pbFile.wType == SP1K_PB_FILE_TYPE_AVI)||(pbFile.wType == SP1K_PB_FILE_TYPE_WGI)) {
		//play first frame of the video
		//printf("AVI\n");
		ret = sp1kPbVideoFramePlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
	} else if (pbFile.wType == SP1K_PB_FILE_TYPE_WAV) {
		//printf("WAV\n");
		ret = appAudioPlayBGShow();
	}
#else
	ret = appStillPlayNext(0xFF);
#endif
	appPbFileInfoUpdate();

	appHdmiOsdSet(1);
	sp1kDispImgWinEn(1); // hlc, Mantis #0020853

	return ret;
}

//-----------------------------------------------------------------------------
//appStillPlayNext
//-----------------------------------------------------------------------------
/**
 * @brief		show next/ previos file in playback mode
 * @param	dir: foward / backward
 * @retval	NONE
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
//add : when Dir == 0xFF,play current file
#if FUNC_HOST_DPS
extern UINT8 DpsImageChooseFlag;
#endif
UINT8 appStillPlayNext(UINT8 Dir) USING_0
{
	UINT8 ret = SUCCESS;
#if (FUNC_HOST_DPS == 1)
	UINT8 printerStatus = appDpsPrinterStatusGet();
#endif
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
#if FUNC_HOST_DPS
	if(!DpsImageChooseFlag){
		appPbFolderUpdate(Dir);
	}
#else
    appPbFolderUpdate(Dir);
#endif
	//printf("idx is %u\n", pbFile.FileIdx);
	if (sp1kPbFileCntGet() == 0) {
		if(dispDevActGet() == 4)pbDZoomPause(); //Fixed #42020
		sp1kPbDispBuffSwitch();
		gprmBlackFill(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100); //xian ++ #41986
		return FAIL;
	}
	if (1 == Dir) {
		//Forward
		pbFile.dwIndex++;
		if (pbFile.dwIndex > sp1kPbFileCntGet()) {
			pbFile.dwIndex = 1;
		}
	} else if (0 == Dir){
		//Backward
		if (pbFile.dwIndex == 1) {
			pbFile.dwIndex = sp1kPbFileCntGet();
		} else {
			pbFile.dwIndex--;
		}
	} else if (0xFF == Dir){
		//
		;//jintao.liu 2008-5-22 for mantis bug
	}
#if 0
	sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
	pbFile.wType = pFileInfo.FileType;
	pbFile.dzFactor = 0;
	pbFile.bVoiceMemo = FALSE;
	strcpy(pbFile.FileName, pFileInfo.FileName);

	if((pFileInfo.FileAttrib & 0x03) == 1) {
		pbFile.bImageProtect = TRUE;
	} else {
		pbFile.bImageProtect = FALSE;
	}
#else
	appPbFileInfoUpdate();
#if (FUNC_HOST_DPS == 1)
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
	if(printerStatus == DPS_PRINT_CON_OK)
	{
		while(pbFile.wType != SP1K_PB_FILE_TYPE_JPG)
		{
			if(!DpsImageChooseFlag){
				appPbFolderUpdate(Dir);
			}
			if (1 == Dir) {
			//Forward
				pbFile.dwIndex++;
				if (pbFile.dwIndex > sp1kPbFileCntGet()) {
					pbFile.dwIndex = 1;
				}
			} else if (0 == Dir){
			//Backward
				if (pbFile.dwIndex == 1) {
					pbFile.dwIndex = sp1kPbFileCntGet();
				} else {
					pbFile.dwIndex--;
				}
			}
			appPbFileInfoUpdate();
		}
	}
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
	#endif
#endif
	//printf("Play -> %s\n", pbFile.FileName);
	if(appCurrStateGet() != APP_STATE_MENU)
	{
		appPbOsdInit();
	}

	if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG) {
		ret = sp1kPbSingleImgPlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if FUNC_HOST_PF_PB
		sp1kHalGprmCpy(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0, 	pbDispBuffGet(0), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0,PB_DECODE_WIDTH, PB_DECODE_HEIGHT,0, 1);
#endif
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
#if _PB_FADEIN_
		sp1kPbFilePlayFadeIn(20);
#endif
	} else if ((pbFile.wType == SP1K_PB_FILE_TYPE_AVI)||(pbFile.wType == SP1K_PB_FILE_TYPE_WGI)) {
		ret = sp1kPbVideoFramePlay(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
#if _PB_FADEIN_
		sp1kPbFilePlayFadeIn(20);
#endif

	} else if (pbFile.wType == SP1K_PB_FILE_TYPE_WAV) {
		ret = appAudioPlayBGShow();
	} else {
		//printf("Unknown file type!\n");
		ret = FAIL;
	}
	appPbFileInfoUpdate();
	//appPbOsdInit();	 //lryy
	return ret;
}

//-----------------------------------------------------------------------------
//appAudioPlayBGShow
//-----------------------------------------------------------------------------
/**
 * @brief		show audio playback background image
 * @param	NONE
 * @retval	NONE
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appAudioPlayBGShow(void) USING_0
{
	UINT32  resId;
	resId=0x15;
	appImgDecode(resId);
	sp1kDispImgShow(sp1kPbDispBuffGet(1), 320, 240, 100);
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appFileDelete
//-----------------------------------------------------------------------------
/**
 * @brief		delete file
 * @param	mode: 0:delete one / 1:delete all
 * @retval	remain file number
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT16 appFileDelete(UINT8 mode) USING_0
{
	UINT16 FileIdx;
	UINT16 FileNum = 0;
	UINT8 fName[12];
	//UINT8 sts;
	UINT16 TempDirIndex;
	//UINT8 tmp1,tmp2;
       //UINT8 CurrDirName[9];
	if (mode == SP1K_FILE_PROC_ONE) {
		FileIdx = sp1kPbFileIndexByOrderGet(pbFile.dwIndex);
		FileNum = sp1kPbFileCntGet();
		strcpy(fName, pbFile.FileName);

		if( pbFile.bImageProtect == TRUE) {
			//printf("File Read only!\n");
			return FileNum;
		}
		if(sp1kVfsFileDelete(fName) != TRUE) {
			FileNum = 0;
			return FileNum;
		}
		FileNum--;
		if (pbFile.bVoiceMemo) {
			fName[8] = 'W';
			fName[9] = 'A';
			fName[10] = 'V';
			sp1kVfsFileDelete(fName);
		}
		dcfFileDramFdbParameterClear(K_DCF_JPG_TYPE, FileIdx);

		sp1kPbFileOrderReset(pbFile.dwIndex);
		sp1kPbLastIndexClear();

		if (FileNum) {
			if (appPreStateGet(0) == APP_STATE_STILL_PLAY || appFileDelHotKeyChk()) {
				appStillPlayNext(0);
			} else if (pbFile.dwIndex > sp1kPbFileCntGet()) {
				pbFile.dwIndex = sp1kPbFileCntGet();
			}
		}
        else  //Folder null
        {
         	TempDirIndex = dcfDir1IndexGet();
            sp1kDcfDeleteFolder(TempDirIndex);
		    sp1kPbFileOrdertableInit();

			appPbFileInfoUpdate();
			switch(pbFile.wType){
				case SP1K_PB_FILE_TYPE_JPG:
					sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
				break;
				case SP1K_PB_FILE_TYPE_AVI:
				case SP1K_PB_FILE_TYPE_WGI:
					sp1kPbVideoFramePlay(DOS_FileIndexGet(pbFile.dwIndex));
				break;
				case SP1K_PB_FILE_TYPE_WAV:
					appAudioPlayBGShow();
				break;
			}
        }
		return FileNum;
	} else { /*if (mode == SP1K_FILE_PROC_ALL)*/

		File_SendMessage( K_File_UI_Start_DeleteAll, 0 );
		G_Card_UI_Action = K_Act_Go_DeletAll;
            sp1kDcfDeleteAllFile();
		#if 0
        TempDirIndex = G_USR_MaxDir1Count;
        do{
            G_USR_Dir1Count = TempDirIndex;
            sprintf(CurrDirName, "%d",TempDirIndex);
            strcat(CurrDirName, "MEDIA");
            DIR_Change(CurrDirName);

            sts = sp1kDcfInit();

            if (sp1kDcfFileCountGet() > 0)
            {
            sts = sp1kVfsFileDeleteAll(K_DCF_ALL_DCF_FILE, K_DCF_OUR_TYPE);

            sts = sp1kDcfInit();
            }
                 DIR_Change("\\DCIM");
            DIR_Remove(CurrDirName);
            sp1kHalDRAMSetStartAddr((K_SDRAM_DcfDirBufAddr + (((UINT32)TempDirIndex) - 100) * 9 + 7),K_SDRAM_Prefetch);
            sp1kHalDRAMReadWord(&tmp1,&tmp2);

            TempDirIndex = (((UINT16)tmp2) << 8) | ((UINT16)tmp1);
        }while(TempDirIndex != 0);

        DIR_Change("..");
        dcfDirInitialize();
	#endif
		File_SendMessage( K_File_UI_End_DeleteAll, sp1kDcfFileCountGet() );
		G_Card_UI_Action = 0;

		sp1kPbFileOrdertableInit();
	}
}

//-----------------------------------------------------------------------------
//appFileReadOnlySet
//-----------------------------------------------------------------------------
/**
 * @brief		modify the attrib of current file
 * @param	FileIndex, ReadOnly, mode
 * @retval
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appFileAttrSet(UINT16 FileIndex, UINT8 ReadOnly, UINT8 mode)
{
	UINT16 FileIdx;
//BFile_t pFileInfo;
	UINT16 FileNum;
	UINT16 i = FileIndex;
	UINT8 ret = SUCCESS;

	//wangjg ++ 20100930
	if(mode == SP1K_FILE_PROC_ALL)
	{
		sp1kDcfProtectAllFile(ReadOnly);
		return SUCCESS;
	}

	FileNum = sp1kPbFileCntGet();
	//printf("file num is %u\n", FileNum);
	//printf("fileindex is %u\n", FileIndex);

	if (!FileNum || FileIndex > FileNum) {
		return FAIL;
	}

#if 0 //mask by xian
	if (mode == SP1K_FILE_PROC_ONE) {
		FileNum = 1;
	} else { //if (mode = SP1K_FILE_PROC_ALL)
		//FileNum = sp1kPbFileCntGet();
	}
	//printf("All(%u)\n", FileNum);
	for (i = 1; i <= FileNum; i++) {
		//printf("%u", i);
		if (FileNum > 1) {
			FileIdx = sp1kPbFileIndexByOrderGet(i);
		} else {
			FileIdx = sp1kPbFileIndexByOrderGet(FileIndex);
		}

		if (sp1kPbFileInfoGet(FileIdx, &pFileInfo) != SUCCESS) {
			//printf("Idx(%u) error\n", FileIdx);
			ret |= FAIL;
		}
		dcfFileReadOnlyAttrSet(FileIdx, pFileInfo.FileType, ReadOnly);
		//printf(">\n");
	}
#else
#if 0//wangjg ++ 20100930
    if(mode == SP1K_FILE_PROC_ONE){
        i =  FileIndex;
        FileNum = i;
    }
    else{
        i = 1;
    }
    do{
	dbgWdtFeed(10000);
#endif
	FileIdx = sp1kPbFileIndexByOrderGet(i);
	dcfFileReadOnlyAttrSet(FileIdx, 0xff, ReadOnly);
#if 0//wangjg ++ 20100930
        i ++;
    }while(i <= FileNum);
#endif
#endif

	return ret;
}

//-----------------------------------------------------------------------------
//appPbImgResize
//-----------------------------------------------------------------------------
/**
 * @brief		appPbImgResize
 * @param
 * @retval
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appPbImgResize(PBFile_t* fileInfo, UINT8 size)
{
	UINT8 ret = SUCCESS;
	UINT16 dstWidth, dstHeight;

	switch (size) {
		case PRV_SIZE_50M:
			dstWidth = 8160;
			dstHeight = 6120;
			break;
		case PRV_SIZE_44M:
			dstWidth = 7664;
			dstHeight = 5748;
			break;
		case PRV_SIZE_42M:
			dstWidth = 7488;
			dstHeight = 5616;
			break;
		case PRV_SIZE_36M:
			dstWidth = 6944;
			dstHeight = 5208;
			break;
		case PRV_SIZE_24M:
			dstWidth = 5600;
			dstHeight = 4200;
			break;
		case PRV_SIZE_20M:
			dstWidth = 5120;
			dstHeight = 3840;
			break;
		case PRV_SIZE_18M:
			dstWidth = 4896;
			dstHeight = 3672;
			break;
		case PRV_SIZE_16M:
			dstWidth = 4608;
			dstHeight = 3456;
			break;
		case PRV_SIZE_12M:
			dstWidth = 3888;
			dstHeight = 2916;
			break;
		case PRV_SIZE_10M:
			dstWidth = 3664;
			dstHeight = 2748;
			break;
		case PRV_SIZE_8M:
			dstWidth = 3264;
			dstHeight = 2448;
			break;
		case PRV_SIZE_5M:
			dstWidth = 2592;
			dstHeight = 1944;
			break;
		case PRV_SIZE_3M:
			dstWidth = 2048;
			dstHeight = 1536;
			break;
		case PRV_SIZE_2M:
			dstWidth = 1600;
			dstHeight = 1200;
			break;
		case PRV_SIZE_2M_16_9:
			dstWidth = 1920;
			dstHeight = 1080;
			break;
		case PRV_SIZE_1M_16_9:
			dstWidth = 1280;
			dstHeight = 720;
			break;
		case PRV_SIZE_1M:
			dstWidth = 1024;
			dstHeight = 768;
			break;
		case PRV_SIZE_VGA:
			dstWidth = 640;
			dstHeight = 480;
			break;
		default:
		//	printf("unkown\n");
			break;
	}

	ret = sp1kPbImgResize(fileInfo, dstWidth, dstHeight);

	return ret;
}

//-----------------------------------------------------------------------------
//appCopy2Card
//-----------------------------------------------------------------------------
/**
 * @brief		copy
 * @param	mode, direction
 * @retval
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appCopy2Card(UINT8 mode, UINT8 direction)
{
	UINT32 tempBuffer;
	UINT8 sts;

	if(appPreStateGet(0) == APP_STATE_STILL_PLAY || appPreStateGet(0) == APP_STATE_MUTI_PLAY) {
		tempBuffer = pbFreeBuffGet();
	}else if (appPreStateGet(0) == APP_STATE_STILL_VIEW || appPreStateGet(0) == APP_STATE_VIDEO_VIEW) {
		tempBuffer = sp1kPvFreeBuffGet();
	}
/*
	if (mode == SP1K_FILE_PROC_ONE) {
		USR_CopyAllDir(tempBuffer, K_Copy_Nand2Card);
	}
*/
	if (mode == SP1K_FILE_PROC_ALL) {
		sts = USR_Copy1Dir(tempBuffer, G_wValueLock, direction);
	}
	else if (mode == SP1K_FILE_PROC_ONE) {
		sts = USR_Copy1File(tempBuffer, G_wValueLock, direction);
	}

	return sts;
}

//-----------------------------------------------------------------------------
//appPbPreStateSet
//-----------------------------------------------------------------------------
/**
 * @brief		record the PB prestate
 * @param	sts: prestate
 * @retval
 * @see
 * @author	sunyong
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appPbPreStateSet(UINT8 sts)
{
	pbPreState = sts;
	return;
}

//-----------------------------------------------------------------------------
//appPbPreStateGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get the PB prestate
 * @param
 * @retval	prestate
 * @see
 * @author	sunyong
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8  appPbPreStateGet(void)
{
	return pbPreState;
}

//-----------------------------------------------------------------------------
//appPbFileInfoUpdate
//-----------------------------------------------------------------------------
/**
 * @brief		appPbFileInfoUpdate
 * @param
 * @retval	prestate
 * @see
 * @author	sunyong
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appPbFileInfoUpdate(void)
{
	PBFile_t pFileInfo;

	sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
	pbFile.wType = pFileInfo.FileType;
	pbFile.dzFactor = 0;
	pbFile.bFileErr = FALSE;
	pbFile.wHSize = pFileInfo.MediaWidth;
	pbFile.wVSize = pFileInfo.MediaHeight;
	strcpy(pbFile.FileName, pFileInfo.FileName);

	if((pFileInfo.FileAttrib & 0x03) == 1) {
		pbFile.bImageProtect = TRUE;
	} else {
		pbFile.bImageProtect = FALSE;
	}

	if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG) {
		if (sp1kPbVoiceMemoCheck(sp1kPbFileIndexByOrderGet(pbFile.dwIndex)) == TRUE) {
			//printf("Voice memo!\n");
			pbFile.bVoiceMemo = TRUE;
		} else {
			pbFile.bVoiceMemo = FALSE;
		}
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appFolderUpdate
//-----------------------------------------------------------------------------
/**
 * @brief   appFolderUpdate
 * @param
 * @retval  FAIL / SUCCESS
 * @see
 * @author	xianxin
 * @since	2008-07-11
 * @todo
 * @bug
*/
UINT8 appPbFolderUpdate(UINT8 forward) USING_0
{
    UINT8 ret = FAIL;
//printf("appPbFolderUpdate %bu\n", forward);
    if(!sp1kPbFileCntGet() || (pbFile.dwIndex == 1 && forward == 0) || (pbFile.dwIndex == sp1kPbFileCntGet() && forward == 1 )){
        //ret = pbFolderNext(forward);
        ret = dcfFolderNext(forward);
        if(ret == SUCCESS){
            dcfFileInitialize();
            if(dcfFileCountGet()){
                DOS_FileOrderTblInit();

                if(forward)
                {
                    pbFile.dwIndex = dcfFileCountGet();

                }
                else
                {
                    pbFile.dwIndex = 1;

                }
                appPbFileInfoUpdate();
            }
            else{//empt folder
				//printf("empty folder\n");
            }
            ret = TRUE;
        }
    }
    return ret;
}


UINT8 appPbFileAspectRatioGet(void)
{
	UINT32 ratio;
	UINT8 rotOpt;
	if((pbFile.wHSize == 0) || (pbFile.wVSize == 0))
	{
		return PB_IMG_ORIENT_UNKNOWN;
	}
	if(pbFile.wHSize > pbFile.wVSize)
	{
		ratio=((pbFile.wHSize*100L)/pbFile.wVSize);
	}
	else
	{
		ratio=((pbFile.wVSize*100L)/pbFile.wHSize);
	}

	rotOpt=sp1kpbDecRotModeGet();
	//printf("rot=%bu\n",sp1kpbDecRotModeGet());
	if((rotOpt == 0) || (rotOpt == 3))//img rot 0 or 180
	{

		if(pbFile.wHSize > pbFile.wVSize)//exif rotate && real rotate
		{
			if((ratio>=185) || (ratio<=120))
			{
				return PB_IMG_ORIENT_UNKNOWN;

			}

			if(ratio>160)
			{
				/*-- 1.777778 */
				return PB_IMG_ORIENT_16X9;
			}
			else if((ratio <= 160) && (ratio > 140))
			{
				/*-- 1.50 */
				return PB_IMG_ORIENT_3X2;
			}
			else
			{
				/*-- 1.33333 */
				return PB_IMG_ORIENT_4X3;
			}

		}
		else
		{
			if((ratio>=185) || (ratio<=120))
			{
				return PB_IMG_ORIENT_UNKNOWN;
			}

			if(ratio>160)
			{
				/*-- 1.777778 */
				return PB_IMG_ORIENT_9X16;
			}
			else if((ratio <= 160) && (ratio > 140))
			{
				/*-- 1.50 */
				return PB_IMG_ORIENT_2X3;
			}
			else
			{
				/*-- 1.33333 */
				return PB_IMG_ORIENT_3X4;
			}

		}
	}
	else
	{
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_9X16;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_2X3;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_3X4;
		}

	}

}

void appPbJpgCrop(void)
{
	PBFile_t pFileInfo;
	UINT32 ImgX0, ImgX1, ImgY0, ImgY1;
	UINT32 addr;
	UINT16 pw,ph;
	PbCropImage_t crop;
	UINT8 ret;

	sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
	sp1kHALDispImgXyGet(&ImgX0, &ImgY0, &ImgX1, &ImgY1);
	sp1kHALDispPbFrameBufferGet(&addr,&pw,&ph);

	crop.startX = ImgX0;
	crop.startY = ImgY0;
	crop.srcWidth = pw;
	crop.srcHeight = ph;
	crop.recWidth = ImgX1 - ImgX0;
	crop.recHeight = ImgY1 - ImgY0;
	crop.srcAddrW = addr;

	pbCropDispBuffer2ABBuf();  //change display buffer ...
	//printf("pw=%d,ph=%d\n",pw,ph);
	//printf("startX=%d,startY=%d,recW=%d,recH=%d,addr=%lx\n",crop.startX,crop.startY,crop.recWidth,crop.recHeight,crop.srcAddrW);
	ret = sp1kPbEdit_ImgDec(&pFileInfo,PB_CROP, &crop);
	if(FAIL == ret ){
		return;
	}
	sp1kPbEditSave( PB_CROP, crop);
}


UINT8 appPbAbortCheck()
{
	UINT8 abort = FALSE;
	#if 0
       printf("appPbAbort %s,%d\n",__FILE__,__LINE__);
	#endif
	   #if 0
	if(sp1kBtnChk(SP1K_BTN_LEFT) || sp1kBtnChk(SP1K_BTN_RIGHT))
	{
	    printf("appPbAbort IN %s,%d\n",__FILE__,__LINE__);
	    abort = TRUE;
	}
       #endif
	 #if 1
	 if(osMsgSeek(SP1K_MSG_KEY_PRESS_LEFT) ||osMsgSeek(SP1K_MSG_KEY_PRESS_RIGHT)){
	   // printf("%s,%d\n",__FILE__,__LINE__);
	    abort = TRUE;
	 }
	 #endif

	return abort;
}
