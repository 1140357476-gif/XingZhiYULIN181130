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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

 #include "general.h"
	 
#include "solution.h"

#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "sp1k_tmr_svc.h"
#include "app_pwr_mgr.h"	

#include "video_clip.h"
#include "video_pb.h"

#include "sp1k_video_api.h"

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
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


//-----------------------------------------------------------------------------
//appAutoOffTimeReload
//-----------------------------------------------------------------------------
/**
 * @brief	reload auto off time
 * @param	none
 * @retval	void
 * @see
 * @author
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appAutoOffTimeReload(void)
{
	//here add time reload code
	sp1kTmrSvc_APO_Rst();
	//printf("sdwAPOTimeVal = %ld\n",sdwAPOTimeVal);
}

//-----------------------------------------------------------------------------
//appAutoOffTimeSet
//-----------------------------------------------------------------------------
/**
 * @brief	set auto off time from menu
 * @param	none
 * @retval	void
 * @see
 * @author
 * @since	2008-02-23
 * @todo
 * @bug
*/
UINT8 appAutoOffTimeSet(void)
{
	uiPara_t* uiPara;

	uiPara = appUiParaGet();

	switch(uiPara->SleepTime)
	{
		case POWER_SAVE_1MIN:
			//printf(" : SAVE 1MIN\n");
			sp1kTmrSvc_APO_ReloadSet(6000);
			appAutoOffEn(1);
			break;
		case POWER_SAVE_3MIN:
			//printf(" : SAVE 3MIN\n");
			sp1kTmrSvc_APO_ReloadSet(18000);
			appAutoOffEn(1);
			break;
		case POWER_SAVE_5MIN:
			//printf(" : SAVE 5MIN\n");
			sp1kTmrSvc_APO_ReloadSet(30000);
			appAutoOffEn(1);
			break;
		case POWER_SAVE_OFF:
			//printf(" : SAVE 5MIN\n");
			appAutoOffEn(0);
			break;
			
		default:
			//printf(" : SAVE DEFT\n");
			sp1kTmrSvc_APO_ReloadSet(18000);
			appAutoOffEn(1);
			break;
	}
	appAutoOffTimeReload();//2008-5-7 add for mantis 24152
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appAutoOffEn
//-----------------------------------------------------------------------------
/**
 * @brief	enable/disable auto power off
 * @param	none
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-04-29
 * @todo
 * @bug

input:
 En=AUTO_POWEROFF_ENABLE       Enable Auto Off
 En=AUTO_POWEROFF_DISABLE      Disable Auto Off
*/
void appAutoOffEn(UINT8 En)
{
	//G_AutoOffEn = En;
	
	uiPara_t* uiPara;

	uiPara = appUiParaGet();
	if(uiPara->SleepTime != POWER_SAVE_OFF)
	{		
		if (sp1kVideoPbStatusGet() == VID_PB_IDLE && sp1kVideoRecStatusGet() == VID_REC_IDLE) {
			sp1kTmrSvc_APO_EnSet(En);
		} else {
			sp1kTmrSvc_APO_EnSet(0);
		}
	}
	else
	{
		sp1kTmrSvc_APO_EnSet(0);
	}
	appAutoOffTimeReload();
}



