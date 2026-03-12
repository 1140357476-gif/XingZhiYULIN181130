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
/**
 * @file		app_ui_para.c
 * @brief		App for UI setting parameter
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
 
#include "general.h"
#include "host_func_opt.h"
#include "initio.h"
#include "sp1k_rsvblk_api.h"
#include "app_init.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "sp1k_util_api.h"
#include "app_sts_pwroff.h"
#include "hal_global.h"
#include "sp1k_hal_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/*Don't modify this value!*/
#define UI_PARAM_SIZE		512

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/*static uiPara_t xdata sPara _at_ 0x7000;*/

static UINT8 xdata uiPara[UI_PARAM_SIZE] _at_ 0x7000;
static uiPara_t xdata * xdata psPara;
UINT8  itemModify;
static UINT8 xdata itemSts[] = {0};
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8 appUDFLoad(void);
extern UINT8 xdata preVideoFrameRate;
extern xdata UINT8 frameRateIsL;/* 0:60    1:30 */
extern xdata dateStc_t	DATEINFO;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

//-----------------------------------------------------------------------------
//appUiParaInit
//-----------------------------------------------------------------------------
/**
 * @brief		UI parameter initialization
 * @param	mode:init mode
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

//#define appUiParaInit_Print	printf
#ifndef appUiParaInit_Print
	#include "dbg_def.h"
	#define appUiParaInit_Print	/_printf
#endif
void appUiParaInit(uiParaInitMode_t mode) USING_0
{
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 resID;
	UINT16 num;
	UINT8 *pClean=NULL;
	if (mode == PARA_INIT_FROM_UDF) {
		//Load UDF from nand reserve block
		psPara = (uiPara_t*)&uiPara[0];

		psPara->EmptyPara = 0;
		if(appUDFLoad()==SUCCESS)
		{
			appMENULoad();
			/*
			#if (SUPPORT_AF && SUPPORT_FIXED_AF)
			psPara->AFMode = 0;
			#endif
			psPara->SnapAf = 1;
			*/
			psPara->ImageEffect=PRV_EFFECT_NORNAL;
			psPara->SceneMode = PRV_SCENE_AUTO;
			//psPara->WBMode = PRV_WB_AUTO;
			psPara->AEMode = PRV_COMP_0EV;
			psPara->redEyePb = REDEYEPB_OFF;
			psPara->Iso = PRV_ISO_AUTO;
			psPara->Seamless = 0;
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#if FUNC_HOST_PF_PV
			psPara->PhotoFrame = PRV_PHOTO_FRAME_OFF;
#endif
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
			//psPara->SleepTime = POWER_SAVE_5MIN;	//mark for mantis #36863

			if (prjDbgMode) {
				psPara->USBMode = USB_PCCAM;
			}
			psPara->FlashLed = 0;
			psPara->resCreate = 0;
			psPara->tpCorrect = 0;
			preVideoFrameRate = psPara->FrameRate;
			frameRateIsL = psPara->FrameRate;
			appUiParaInit_Print("UDF load!\n");
			return;
		}
	} 

	if(appADFLoad()==SUCCESS)//UI config tool flow
	{
		appUiParaInit_Print("adf load success\n");
		appMENULoad();
		
	}
	else// FW flow
	{
		psPara = (uiPara_t*)&uiPara[0];
		appUiParaInit_Print("adf load fail\n");
		pClean=(UINT8*)psPara;
		for(num=0;num<sizeof(uiPara_t);num++)//clean para
		{
			*pClean = 0;
			pClean++;
		}
		psPara->EmptyPara = 0;
		psPara->DriverMode = PRV_SNAP_SINGLE;
		psPara->ImageSize = PRV_SIZE_3M;
		psPara->ImageQuality = PRV_QUAL_SUPER;
		psPara->WBMode = PRV_WB_AUTO;
		psPara->AEMetering = PRV_METER_MULTI;
		psPara->FDMetering = FD_AE_OFF;
		psPara->AEMode = PRV_COMP_0EV;
		psPara->Saturation = PRV_SATURATION_HIGH;//PRV_SATURATION_MIDDLE
		psPara->Sharpness = PRV_EDGE_NORMAL;
		psPara->Contrast = PRV_CONTRAST_HIGH;//PRV_CONTRAST_OFF; // PRV_CONTRAST_MIDDLE;
		/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
		#if FUNC_HOST_PF_PV
		psPara->PhotoFrame = PRV_PHOTO_FRAME_OFF;
		#endif	
		/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
		psPara->ImageEffect = PRV_EFFECT_NORNAL;
		psPara->SceneMode = PRV_SCENE_AUTO;
		psPara->VideoSize = MOV_SIZE_1080P;
		psPara->FrameRate = MOV_FRM_LOW;
		psPara->SpeedyCap = SPEEDY_CAPTURE_OFF;
		psPara->SpeedyCapRec = SPEEDY_CAPTURE_REC_OFF;
		psPara->Iso = PRV_ISO_AUTO;
		psPara->AntiShake = PRV_ANTISHAKE_ON;//PRV_ANTISHAKE_OFF;  /*meili for antishake 20110302*/

		psPara->Seamless = 0;
		psPara->StampMode = PRV_STAMP_DATETIME;//PRV_STAMP_DATETIME;//PRV_STAMP_OFF;
		psPara->StampVideoMode = PRV_STAMP_DATETIME;//PRV_STAMP_DATETIME;
		if (prjDbgMode) {
			psPara->USBMode = USB_PCCAM;
		} else {
			psPara->USBMode = USB_MASS;
		}
		psPara->resCreate = 1;
		//psPara->SleepTime = POWER_SAVE_3MIN;
		#if FUNC_HOST_TOUCH_PANEL
		psPara->tpCorrect = 1;
		#else
		psPara->tpCorrect = 0;
		#endif
		psPara->Repeat = 0x01;
		
		#if (FUNC_HOST_DPS == 1)

		psPara->DPSQuality = 0x00;
		psPara->DPSLayout = 0x00;
		psPara->DPSPageSize = 0x00;
		psPara->DPSPageType = 0x00;
		psPara->DPSDate = 0x00;
		psPara->DPSFileName = 0x00;
		psPara->DPSImageMode = 0x00;
		psPara->DPSCopies = 0x00;

		#endif

		psPara->rtcYear= 22;
		psPara->rtcMonth = 1-1;
		psPara->rtcDay = 1-1;
		psPara->rtcMinute = 0;
		psPara->rtcHour = 0;
		
		psPara->DateStyle = 2;
		

		psPara->SpeakSound = 1;
		psPara->FlashLed = 0;
		
		psPara->TVMode = TVMODE_NTSC;
		psPara->BeepSound = BEEP_ON;
		psPara->ShutterSound = SHUTTER_ON;
		psPara->Language = ENGLISH;//Japanese;//ENGLISH;//S_CHINESE;
		psPara->Volume=VOLUME_10;
		psPara->BlinkMode=BLINKMODE_OFF;
		psPara->SmileMode=SMILEMODE_OFF;
		psPara->redEyePb = REDEYEPB_OFF;
		/*psPara->WDRMode=WDRMODE_OFF; */
		psPara->FDMode=FDMODE_OFF;
		psPara->DisMode=DISMODE_OFF;
		psPara->AdvanceDenoise = REC_ADVANCEDENOISE_OFF;
		psPara->StartupSound=STARTSOUND_1;
		psPara->LightFreq=PRV_FREQ_60;//PRV_FREQ_60;
		psPara->OpenPic = PB_OPENPIC_DEFAULT;
		psPara->AutoReviewTime=QUICKVIEW_1S;
		psPara->SleepTime=POWER_SAVE_OFF;//POWER_SAVE_3MIN; // min
		#if SUPPORT_PANEL_PROTECT
		psPara->ProtTime = PANEL_TIME_OFF;
		#endif
		frameRateIsL=psPara->FrameRate;
		psPara->SunplusFlag[0]='S';
		psPara->SunplusFlag[1]='U';
		psPara->SunplusFlag[2]='N';
		psPara->SunplusFlag[3]='P';
		
		psPara->SnapAf = 0; // 1;
		if(!psPara->AdjustMacro){
			psPara->AdjustMacro = 431;//480;//496;
			#if (SUPPORT_AF && SUPPORT_FIXED_AF)
			psPara->AFMode = 0;
			#endif
		}
	}

	DATEINFO.Day=psPara->rtcDay+1;
	DATEINFO.Hour=psPara->rtcHour;
	DATEINFO.Minute=psPara->rtcMinute;
	DATEINFO.Month=psPara->rtcMonth+1;
	DATEINFO.Year=psPara->rtcYear;
	sp1kHalWriteRTC(&DATEINFO);
	
	resID = 0x0d; //.\a\open.jpg
	sp1kNandRsvRead(resID, freeBuffAddr << 1);
	// resID = 0x800; //.\b\open.jpg
	// sp1kNandRsvWrite(resID, freeBuffAddr << 1);
}

//-----------------------------------------------------------------------------
//appUiParaGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get ui parameter pointer
 * @param	NULL
 * @retval	ui parameter pointer
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
uiPara_t* appUiParaGet(void)
{
	return psPara;
}
void appUiItemOrderCfg(void)
{
	UINT8 idx;
	idx=	uiPara[0x100];
	//printf("@@idx--->0x%bx\n",idx);
	if(idx >= ITEM_FIRST && idx != ITEM_NULL)
	{
		itemModify=1;
	}
	else
	{
		itemModify=0;
	}
}
/**
 * @fn        UINT8 appUiItemStsSet(UINT8 idx, idx,UINT8 val)
 * @brief     appUiItemStsSet
 * @param     [in] idx
 * @param     [in] val
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    ZhuXiaolin
 * @since     2012-7-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appUiItemStsSet(UINT8 idx,UINT8 val)
{
	if(!itemModify) val = 0;
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return 0;
	}
	itemSts[idx] = val;
}
/**
 * @fn        UINT8 appUiItemStsGet(UINT8 idx)
 * @brief     appUiItemStsGet
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    ZhuXiaolin
 * @since     2012-7-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8  appUiItemStsGet(UINT8 idx)
{
	if(!itemModify) return 0;
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return 0;
	}
	return itemSts[idx];
}
UINT8 appUiItemOrderGet(UINT8 idx,UINT8 *pTable,UINT8 *length)
{
	UINT8 *pDate=&uiPara[0x100];
	UINT8 num=0;
	
	if(itemModify)
	{
	if(idx < ITEM_EMPTY || idx > ITEM_MAX)
	{
		return FALSE;
	}
	if(*pDate ==ITEM_NULL) {
		return FALSE;
	}
	while(*pDate != idx)//fine index
	{
		pDate++;
		if(*pDate ==ITEM_NULL) {
			return FALSE;
		}
	}

	pDate++;
	while(*pDate<ITEM_EMPTY)
	{
		*pTable = *pDate;
		//printf("pdate[%bu]=%bu  ",num,*pDate);

		pTable++;
		pDate++;
		num++;
	}
	//printf("\nnum=%bu\n",num);
	*length=num;
	return TRUE;
	}
	return FALSE;
}
