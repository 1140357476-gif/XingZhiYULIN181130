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
 * @file      gui_api.c
 * @brief     API of GUI module
 * @author    Phil Lin
 * @since     2011-09-05
 * @date      2011-09-05
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "gui_api.h" 
#include "gui_dbg.h" 
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
static UINT8 sGuiOpMode[GUI_OP_MODE_MAX];
static UINT32 sGuiOpPara[GUI_OP_PARA_MAX];
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 guiObjCreate(guiObj_t *pObjAttr, guiObj_t **ppObjHdl)
 * @brief     guiObjCreate
 * @param     [in] pObjAttr
 * @param     [in] ppObjHdl
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjCreate(
	guiObj_t *pObjAttr,
	guiObj_t **ppObjHdl
)
{
	UINT8 ret = SUCCESS;

	ret |= guiObjNew(ppObjHdl);
	if (ret != SUCCESS) {
		return ret;
	}

	(*ppObjHdl)->x0      = pObjAttr->x0;
	(*ppObjHdl)->y0      = pObjAttr->y0;
	(*ppObjHdl)->x1      = pObjAttr->x1;
	(*ppObjHdl)->y1      = pObjAttr->y1;
	(*ppObjHdl)->pPenUp  = pObjAttr->pPenUp;
	(*ppObjHdl)->pObjExt = pObjAttr->pObjExt;
	
	ret |= guiObjDraw(pObjAttr, 0x40 | 0x3c);
	
	return ret;
}

/**
 * @fn        UINT8 guiObjDestory(guiObj_t *pObj)
 * @brief     guiObjDestory
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjDestory(
	guiObj_t *pObj
)
{
	UINT8 ret = SUCCESS;

	ret |= guiObjFree(pObj);
	if (ret != SUCCESS) 
	{
		return ret;
	}

	ret |= guiObjClr(pObj);

	return ret;
}


/**
 * @fn        UINT8 guiObjOpModeSet(UINT8 modeIdx, UINT8 modeVal)
 * @brief     guiObjOpModeSet
 * @param     [in] modeIdx
 * @param     [in] modeVal
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-7
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjOpModeSet(
	UINT8 modeIdx,
	UINT8 modeVal
)
{
	if (modeIdx < GUI_OP_MODE_MAX) {
		sGuiOpMode[modeIdx] = modeVal;
		return SUCCESS;
	} else {
		return FAIL;;
	}
}
/**
 * @fn        UINT8 guiObjOpModeGet(UINT8 modeIdx)
 * @brief     guiObjOpModeGet
 * @param     [in] modeIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-7
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjOpModeGet(
	UINT8 modeIdx
)
{
	UINT8 modeVal = -1;
	
	if (modeIdx < GUI_OP_MODE_MAX) {
		modeVal = sGuiOpMode[modeIdx];
	}
		
	return modeVal;
}

/**
 * @fn        UINT8 guiObjOpParaSet(UINT8 paraIdx, UINT32 paraVal)
 * @brief     guiObjOpParaSet
 * @param     [in] paraIdx
 * @param     [in] paraVal
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-31
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjOpParaSet(
	UINT8 paraIdx,
	UINT32 paraVal
)
{
	if (paraIdx < GUI_OP_PARA_MAX) {
		sGuiOpPara[paraIdx] = paraVal;
		return SUCCESS;
	} else {
		return FAIL;;
	}
}

/**
 * @fn        UINT32 guiObjOpParaGet(UINT8 paraIdx)
 * @brief     guiObjOpParaGet
 * @param     [in] paraIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-31
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjOpParaGet(
	UINT8 paraIdx
)
{
	UINT32 paraVal = -1;
	
	if (paraIdx < GUI_OP_PARA_MAX) {
		paraVal = sGuiOpPara[paraIdx];
	}

	return paraVal;
}

