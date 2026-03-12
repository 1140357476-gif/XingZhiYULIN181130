/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      app_gui_obj.c
 * @brief     GUI object related function
 * @author    Phil Lin
 * @since     2011-09-02
 * @date      2011-09-02
*/
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "func_gui_def.h" 
#include "gui_api.h" 
#include "sys_mem_init.h"
#include "sp1k_hal_api.h"
#include "app_ui_para.h"
#include "app_ui_osd.h"
#include "app_main.h" 
#include "app_msg.h" 
#include "app_gui_obj.h" 
#include "app_menu_core.h" 
#include "app_menu_event.h" 
#include "app_menu_tab.h"
#include "app_menu_micro.h"
#include "app_menu.h"
#include "app_still_view.h"
#include "app_still_play.h"
#include "key_scan.h"
#include "dbg_def.h"
#include "app_audio_play.h"
#include "host_func_opt.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define OSD_BUFF_W 320 
#define OSD_BUFF_H 240 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define LINK_COLOR		OSD_COLOR_GREEN
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static guiObj_t *pGuiObj[GUI_OBJ_MAX]; 
static UINT8 guiObjIdx;
static guiObjExt_t guiObjExtInfo[GUI_OBJ_MAX];
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern SIMAGE xdata pbFile;
static UINT8 GuiEnable;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if FUNC_HOST_TOUCH_PANEL

/**
 * @fn        UINT8 appGuiObjAdd(guiObj_t *pObj)
 * @brief     appGuiObjAdd
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appGuiObjAdd(
	guiObj_t *pObj
)
{
	UINT8 ret = SUCCESS;
	
	if (guiObjIdx >= GUI_OBJ_MAX) 
	{
		ret |= FAIL;
		return ret;
	}
	
	ret |= guiObjCreate(pObj, &pGuiObj[guiObjIdx]);
	
	guiObjIdx++;
	return ret;
}

/**
 * @fn        UINT8 appGuiObjDraw(guiObj_t *pObj, UINT8 objIdx)
 * @brief     appGuiObjDraw
 * @param     [in] pObj
 * @param     [in] objIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appGuiObjDraw(
	guiObj_t *pObj,
	UINT8 objIdx
)
{
	UINT8 ret = SUCCESS;

	if (objIdx >= GUI_OBJ_MAX || pGuiObj[objIdx] != NULL)
	{
		ret |= FAIL;
		return ret;
	}

	pObj->pObjExt = &guiObjExtInfo[objIdx];
	ret |= guiObjCreate(pObj, &pGuiObj[objIdx]);

	return ret;
}

/**
 * @fn        UINT8 appGuiObjClr(UINT8 idx)
 * @brief     appGuiObjClr
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appGuiObjClr(
	UINT8 idx
)
{
	UINT8 ret = SUCCESS;
	UINT8 i;

	if (idx != -1) 
	{
		/* destory the sepcified GUI object */
		ret |= guiObjDestory(pGuiObj[idx]);
		pGuiObj[idx] = NULL;
		memset(guiObjExtInfo, 0, sizeof(guiObjExt_t));
	} 
	else 
	{
		/* destory all GUI object */
		for (i = 0; i < GUI_OBJ_MAX; i++)
		{
			if (pGuiObj[i]) 
			{
				ret |= guiObjDestory(pGuiObj[i]);
				pGuiObj[i] = NULL;
			}
		}
		memset(guiObjExtInfo, 0, sizeof(guiObjExt_t)*GUI_OBJ_MAX);
		guiObjIdx = 0;
	}

	return ret;
}

/*-------------------------------------------------------------------------

				still view function

------------------------------------------------------------------------*/
UINT8 appStillGuiHdl_EvIncrease(void *ptr)
{	
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	pUiPara->AEMode++;
	if (PRV_COMP_MAX == pUiPara->AEMode) 
	{
		pUiPara->AEMode = PRV_COMP_N20EV;
	}
	
	appEVSet(pUiPara->AEMode);
	appOsdColorSet(LINK_COLOR);
	uiUpdateEV(0, 0, 0);
	appOsdColorSet(OSD_COLOR_WHITE);
	return SUCCESS;

}

UINT8 appStillGuiHdl_ImgSizeCrease(void *ptr)
{
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	pUiPara->ImageSize++;

	if (PRV_SIZE_MAX == pUiPara->ImageSize)
	{
		pUiPara->ImageSize = PRV_SIZE_START;
	}
	appStillQualitySet();
	appOsdColorSet(LINK_COLOR);
	uiUpdateImgSize(0, 0, 0);
	appOsdColorSet(OSD_COLOR_WHITE);
	uiRemainPixInit();
	uiUpdateRemainPix();
	return SUCCESS;
}

UINT8 appStillGuiHdl_WbCrease(void *ptr)
{
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	pUiPara->WBMode++;

	if (PRV_WB_MAX== pUiPara->WBMode) 
	{
		pUiPara->WBMode = PRV_WB_AUTO;
	}
	appOsdColorSet(LINK_COLOR);
	uiUpdateWB(0, 0, 0);
	appOsdColorSet(OSD_COLOR_WHITE);
	return SUCCESS;
}

UINT8 appStillGuiHdl_AeMeteringCrease(void *ptr)
{
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	pUiPara->AEMetering++;

	if (PRV_METER_MAX== pUiPara->AEMetering) 
	{
		pUiPara->AEMetering = PRV_METER_CENTER;
	}
	
	appOsdColorSet(LINK_COLOR);
	uiUpdateAEMetering(0, 0, 0);
	appOsdColorSet(OSD_COLOR_WHITE);

	return SUCCESS;
}

UINT8 appStillGuiHdl_ImgQualityCrease(void *ptr)
{
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	pUiPara->ImageQuality++;

	if (PRV_QUAL_MAX== pUiPara->ImageQuality) 
	{
		pUiPara->ImageQuality = PRV_QUAL_SUPER;
	}
	appStillQualitySet();
	appOsdColorSet(LINK_COLOR);
	uiUpdateImgQuality(0, 0, 0);
	appOsdColorSet(OSD_COLOR_WHITE);
	uiRemainPixInit();
	uiUpdateRemainPix();
	return SUCCESS;

}


/*-------------------------------------------------------------------------

					video view function

------------------------------------------------------------------------*/

UINT8 appVideoGuiHdl_WbCrease(void *ptr)
{
	appStillGuiHdl_WbCrease(ptr);
	return SUCCESS;
}
UINT8 appVideoGuiHdl_AeMeteringCrease(void *ptr)
{
	appStillGuiHdl_AeMeteringCrease(ptr);
	return SUCCESS;
}

UINT8 appVideoGuiHdl_SizeCrease(void *ptr)
{
	uiPara_t* puiPara;
	puiPara = appUiParaGet();
	ptr=ptr;
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	if (MOV_SIZE_QVGA== puiPara->VideoSize)
	{
		puiPara->VideoSize = MOV_SIZE_HD;
	}
	else
	{
		puiPara->VideoSize++;
	}
	sp1kBtnDisableAll();
	sp1kHalWait(100);
	sp1kBtnEnableAll();
	osMsgPost(SP1K_MSG_STATE_INIT);
	return SUCCESS;
}



/*-------------------------------------------------------------------------

					playback  function

------------------------------------------------------------------------*/
UINT8 appStillPbGuiHdl_MultiObjSel(void *ptr)
{
	UINT8 ret = SUCCESS;
	UINT8 para = (UINT8)(((guiObj_t*)ptr)->pObjExt->pPenUpPara);
	UINT16 imgIndex;

	imgIndex = (pbFile.dwIndex-1)/9*9+1+para;
	pbFile.dwIndex = imgIndex;
	osMsgPost(SP1K_MSG_KEY_PRESS_SNAP);
	return ret;
	
}


/*-------------------------------------------------------------------------

						menu function

------------------------------------------------------------------------*/


UINT8
appMenuGuiHdl_MenuMode(
	void *ptr
)
{
	UINT32 menuPos;
	UINT8 layer = menuProcLayerGet();
	UINT8 tp = (UINT8)(((guiObj_t*)ptr)->pObjExt->pPenUpPara);
	menuPos=menuProcCurrPosGet();
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);

	switch(tp)
	{
		case 0x00:
			switch ( appPreStateGet(0) )
			{
				case APP_STATE_STILL_VIEW:
					menuPos = MENU_MARK_STILL;
					break;
				case APP_STATE_VIDEO_VIEW:	
					menuPos = MENU_MARK_VIDEO;
					break;	
				case APP_STATE_AUDIO_REC:		
					menuPos = MENU_MARK_AUDIO;
					break;
				case APP_STATE_VIDEO_PLAY:			
				case APP_STATE_STILL_PLAY:
				case APP_STATE_MUTI_PLAY:
					menuPos = MENU_MARK_PLAYBACK;
					break;
				case APP_STATE_USB:
					break;
				case APP_STATE_DPS:
					menuPos = MENU_MARK_DPS;
					break;
				default:
					menuPos = MENU_MARK_STILL;
			}
			menuProcCurrPosSet(menuPos);
			menuProcDisp();
			break;
		case 0x01:
			menuPos = MENU_MARK_SETUP;
			menuProcCurrPosSet(menuPos);
			menuProcDisp();
			break;
		default:
			break;
	}
	return SUCCESS;
}

UINT8
appMenuGuiHdl_SelItem(
	void *ptr
)
{
	UINT8 tp = (UINT8)(((guiObj_t*)ptr)->pObjExt->pPenUpPara);
	if (menuProcTPSet(tp, !menuProcLayerGet()) == SUCCESS)
	{	
		osMsgPost(SP1K_MSG_KEY_PRESS_SNAP);
	}
	
	return SUCCESS;
}


/*-------------------------------------------------------------------------

					Exit function

------------------------------------------------------------------------*/

/**
 * @fn        UINT8 appStillGuiExit(void)
 * @brief     appStillGuiExit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/

UINT8
appStillGuiExit(
	void
)
{
	UINT8 ret = SUCCESS;

	/* Host MUST destory the GUI obj here */

	ret |= appGuiObjClr(-1);
	
	return ret;
}

/**
 * @fn        UINT8 appVideoGuiExit(void)
 * @brief     appStillGuiExit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appVideoGuiExit(
	void
)
{
	UINT8 ret = SUCCESS;

	/* Host MUST destory the GUI obj here */

	ret |= appGuiObjClr(-1);
	
	return ret;
}


/**
 * @fn        UINT8 appStillPbGuiExit(void)
 * @brief     appStillGuiExit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appStillPbGuiExit(
	void
)
{
	UINT8 ret = SUCCESS;

	/* Host MUST destory the GUI obj here */

	ret |= appGuiObjClr(-1);
	
	return ret;
}

/**
 * @fn        UINT8 appMenuGuiExit(void)
 * @brief     appMenuGuiExit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
appMenuGuiExit(
	void
)
{
	UINT8 ret = SUCCESS;

	/* Host MUST destory the GUI obj here */
	
	ret |= appGuiObjClr(-1);

	return ret;
}

void appGuiEnableSet(UINT8 enable)
{
	GuiEnable = enable;
}

UINT8 appGuiEnableGet(void)
{
	return GuiEnable;
}
#endif
