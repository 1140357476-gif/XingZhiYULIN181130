/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus mMedia
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Technology Co., Ltd.
 *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *
 **************************************************************************/

/**
 * @file		ptr_func.c
 * @brief		function pointer jump list
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "func_ptr.h"
#include "Cmd_test.h"
#include "os_msg.h"
#include "app_init.h"
#include "app_still_view.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "audio_rw.h"
#include "video_pb.h"
#include "video_clip.h"
#include "app_usb.h"
#include "app_ui_osd.h"
#include "timer.h"
#include "sp1k_util_api.h"
#include "app_menu_tab.h"
#include "key_scan.h"
#include "app_idle.h"
#include "app_battery.h"
#include "usbmsdc2.h"
#include "uiflow.h"
#include "sp1k_hal_api.h"
#include "audio_rw.h"
#include "app_msg.h"
#include "app_tp_scan.h"
#include "host_func_opt.h"

#include "gui_evt.h"
#include "gui_tp.h"
#include "app_gui_obj.h"
#include "video_capture.h"
#include "app_still_play.h"
#include "host_func_opt.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
extern xdata UINT16 G_UIStatus;
extern xdata	UINT8	G_testUnitReadyFlag;
extern xdata 	UINT32	G_timerCheckTestUnit;

extern UINT8 G_tpOsdDrawFlag;
extern UINT16 G_tpXaxisCoord;
extern UINT16 G_tpYaxisCoord;
//=============================================================================
//Program       
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_TestSnap
 * @param	ptr:test time
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_TestSnap(UINT16* ptr)
{
	return testTSnapRoutine(ptr);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_TestPb
 * @param	ptr:test time
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_TestPb(UINT16* ptr)
{
	return testTPlaybackRoutine(ptr);
}

void pFunc_KeyScan(void)
{
    
	keyScan();

}
#if FUNC_HOST_TOUCH_PANEL
void pFunc_TpScan(void)
{
	if(appGuiEnableGet()==1)
	{
		tpScan();
	}
}

/**
 * @fn        UINT8 pFunc_GuiProc(void)
 * @brief     pFunc_GuiProc
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8 pFunc_GuiProc(void)
{
	UINT8 ret;
	SINT16 G_tpXaxisCoord ,G_tpYaxisCoord;

	tpCoordFtProc();
	tpPixAxisGet(&G_tpXaxisCoord, &G_tpYaxisCoord);
	if(tpScanProcGet()) {
		ret = guiObjEvtHdl(G_tpXaxisCoord ,G_tpYaxisCoord);
		if (tpScanProcGet() == TP_PROC_PU) {
			tpCoordFtRst();
		}
	}
	
	return ret;
}


#endif
/********************Function pFunc_SelfSnapMSGSendShell ***************/
//Name:				void pFunc_SelfSnapMSGSendShell(void)
//Description:			The shell of function appSelfSnapMSGSend
//					For the function should run in common bank
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-10 11:44
//Last Modify Time:	08-03-06 15:40
/*****************************************************/
void pFunc_SelfSnapMSGSendShell(void)
{
	osMsgPost(SP1K_MSG_SELFSNAP_COUNT);
	//appSelfSnapMSGSend();
}

/********************Function pFunc_SelfSnapMSGSendShell ***************/
//Name:				void pFunc_SelfSnapMSGSendShell(void)
//Description:			The shell of function appSelfSnapMSGSend
//					For the function should run in common bank
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-04-09 11:44
//Last Modify Time:	08-04-10 11:40
/*****************************************************/
void pFunc_USBPwrSave(void)
{
	appUsbSavePwr();
}

#if FUNC_HOST_MD
void pFunc_MDTimeProcess(void)
{
	sp1kTimer1Stop();
	osMsgPost(SP1K_MSG_KEY_PRESS_S2);
}
#endif

/**
 * @fn        void pFunc_TimerCount(void)
 * @brief     pFunc_TimerCount
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-15
 * @todo      N/A
 * @bug       N/A
*/
void pFunc_TimerCount(void)
{
	appTimerCount();
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_CardDetect
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_CardDetect(void)
{
	//printf("+\n");
	appSDPlugAction();
	return SUCCESS;
}

//
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_USBDetect
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_USBDetect(void)
{
	//printf("+\n");
	appUSBOutDetect();
	return SUCCESS;
}

UINT8 pFunc_BatteryChgDetect(void)
{
	//printf("+\n");
	appBatteryChgDetect();
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_TVDetect
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	jintao.liu
 * @since		2008-03-13
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 pFunc_TVDetect(void)
{
	//printf("+\n");
	// appTVOutDetect();
	// appHDMIDetect();

	#if SUPPORT_UPDATE_LED
	appUpdateStatsLightStatus();
	#endif

	#if SUPPORT_USB_ICON
	appChargingIconFlicker();
	#endif
		
	return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_AudioPb
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_AudioPb(void)
{
	audioPb();
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_AudioRec
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_AudioRec(void)
{
	audioRec();
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_VideoPb
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_VideoPb(void)
{
	videoPb();
	return SUCCESS;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_VideoRec
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_VideoRec(void)
{
	
	return videoRec();
}

/**
 * @brief		pFunc_VideoCap
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_VideoCap(void)
{
	
	return speedyCapture();
}

UINT8 pFunc_VideoCapRec(void)
{
	return speedyCaptureRec();
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_osdBusyShow
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_osdBusyShow(void)
{
	//appUiBusyShow(0);			
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//pFunc_VoiceMemoStop
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_VoiceMemoStop
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_VoiceMemoStop(void)
{
	osMsgPost(SP1K_MSG_MEDIA_VOICE_MEMO_END);
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//pFunc_SlideShowProc
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_SlideShowProc
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_SlideShowProc(void)
{
	osMsgPost(SP1K_MSG_SLIDE_SHOW_NEXT);
	return SUCCESS;
}

/**
 * @fn        void pFunc_updateAfwindow(void)
 * @brief     pFunc_updateAfwindow
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void pFunc_updateAfwindow(void)
{
	osMsgPost(SP1K_MSG_UPDATE_AF_WINDOW);
}

#if SUPPORT_AF
void pFunc_AfCal(void)
{
	osMsgPost(SP1K_MSG_SYS_AF_CAL);
}
#endif

//-----------------------------------------------------------------------------
//pFunc_TestDscLife_span
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_TestDscLife_span
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if LIFE_SPAN_TEST
UINT8 pFunc_TestDscLife_span(void)
{
	return testDscLife_span();
}
#endif

#if CMD_BTN_TEST
extern void btnTestTask(void);
//-----------------------------------------------------------------------------
//pFunc_btnTestTask
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_btnTestTask
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 pFunc_btnTestTask(void)
{
	btnTestTask();
	return SUCCESS;
}
#endif /*CMD_BTN_TEST*/



#ifdef WHITE_LED
//-----------------------------------------------------------------------------
//pFunc_whiteLedCtrl
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_SlideShowProc
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-03-10
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 pFunc_whiteLedCtrl(void)
{
	appWhiteLedCtrl();
	return SUCCESS;
}
#endif /*WHITE_LED*/

//-----------------------------------------------------------------------------
//pFunc_SnapBurst
//-----------------------------------------------------------------------------
/**
 * @brief		pFunc_SnapBurst
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	Wen-Guo Gan
 * @since		2008-10-15
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 
pFunc_SnapBurst( 
	UINT8 *ptr 
)
{
	UINT8 burstCnt = *ptr;	
	
	return appSnapBurstCheck( burstCnt );
}
UINT8 
pFunc_pbAbort( void)
{      
#if 0
       printf("pFunc_pbAbort %s,%d\n",__FILE__,__LINE__);
#endif
	return appPbAbortCheck();
}
void
pFunc_CheckCountforTestUnitReady(void)
{	
	if ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)
	{
		UINT32 t0, t1;
		sp1kHalReadGTimer(&t0);
		if( t0>=G_timerCheckTestUnit )
			t1 = t0 - G_timerCheckTestUnit;

		if(G_testUnitReadyFlag&&(t1>10000)&&(t0>G_timerCheckTestUnit))
		{
			osMsgPost(SP1K_MSG_USB_PLUG_OUT);
		}
	}
}

/**
 * @brief		pFunc_SoundCtrl
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	
 * @since		2008-10-15
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8
pFunc_SoundCtrl(
	void
)
{
	soundctl();
	return SUCCESS;
}

/**
 * @brief		pFunc_AeUserDefineMeter
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8* 
pFunc_AeUserDefineMeter( 
	UINT8 w,
	UINT8 h
)
{	
	return appUserDefineMeter(w, h);
}

/**
 * @brief		pFunc_AeDynamicUserDefineMeter
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8
pFunc_AeDynamicUserDefineMeter( 
	void
)
{	
	 appDynamicProgrammingUserMeterFlow();

	 return SUCCESS;
}

/**
 * @brief		pFunc_SnapBackgroundResLoad
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8
pFunc_SnapBackgroundResLoad(
	void *ptr
)
{
	UINT32 dstAddrWord, bufByteSize;
	
	dstAddrWord = *((UINT32*)(ptr));
	bufByteSize = *((UINT32*)(ptr) + 1);

	//printf("addr=%lu,size=%lu\n",dstAddrWord,bufByteSize);
		
	return appSnapBackgroundResLoad(dstAddrWord, bufByteSize);
}

/**
 * @brief		pFunc_SnapHostStamp
 * @param	NONE
 * @retval	SUCCESS
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8
pFunc_SnapHostStamp(
	void *ptr
)
{
	ptr = ptr;
	return appSnapHostStamp();
}

UINT8 
pFunc_SfWPSet( 
	UINT8 *ptr 
)
{
	UINT8 mode = *ptr;
	UINT8 sts = 0;
#ifdef DISK_DRV_SPI	
	sts = appSfWPSet( mode );
#endif
	return sts;
}
