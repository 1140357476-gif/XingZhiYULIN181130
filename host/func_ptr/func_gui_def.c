/**************************************************************************
 *         Copyright(c) 2011 by i-Catch Technology Co., Ltd.              *
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
 * @file      func_gui_def.c
 * @brief     GUI callback function definition
 * @author    Phil Lin
 * @since     2011-09-02
 * @date      2011-09-02
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#define SYS_GUI_INTERNAL // for libGui internal use
#include "func_gui_def.h"

#include "app_gui_obj.h"
#include "host_func_opt.h"
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

pFuncGui code funcGuiCb_Still[] =
{
	NULL,
#if FUNC_HOST_TOUCH_PANEL
	appStillGuiHdl_WbCrease,
	appStillGuiHdl_AeMeteringCrease,
	appStillGuiHdl_EvIncrease,
	appStillGuiHdl_ImgSizeCrease,
	appStillGuiHdl_ImgQualityCrease,
#endif
};

pFuncGui code funcGuiCb_Video[] = 
{
	NULL,
#if FUNC_HOST_TOUCH_PANEL
	appVideoGuiHdl_WbCrease,
	appVideoGuiHdl_AeMeteringCrease,
	appVideoGuiHdl_SizeCrease,
#endif
};

pFuncGui code funcGuiCb_StillPb[] = 
{
	NULL,
#if FUNC_HOST_TOUCH_PANEL
	appStillPbGuiHdl_MultiObjSel,
#endif	
};


pFuncGui code funcGuiCb_Menu[] = 
{
	NULL,
#if FUNC_HOST_TOUCH_PANEL
	appMenuGuiHdl_MenuMode,
	appMenuGuiHdl_SelItem,
#endif
};

/**************************************************************************
 *               DO NOT MODIFY THE ARRAY BELOW                            *
 **************************************************************************/

/* MUST Sync with the enum "funcGuiCbArray" */

SYS_GUI_DCL pFuncGui code* code funcGuiCbArray[] = 
{
	funcGuiCb_Still,
	funcGuiCb_StillPb,
	funcGuiCb_Video,
	funcGuiCb_Menu,
};

/* this global used to correction call tree */
static pFuncGui code* code* xdata pfuncGuiCbArray = funcGuiCbArray;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        pFuncGui funcGuiHdlGet(funcGuiHdlGrp_t grp, UINT8 idx)
 * @brief     funcGuiHdlGet
 * @param     [in] grp
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
pFuncGui
funcGuiHdlGet(
	funcGuiHdlGrp_t grp,
	UINT8 idx
)
{
	/*
		DO NOT use "funcGuiCbArray" in the function because of the call tree correction
	*/
	
	return pfuncGuiCbArray[grp][idx];
}
