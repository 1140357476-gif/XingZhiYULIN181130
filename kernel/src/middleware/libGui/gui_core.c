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
 * @file      gui_core.c
 * @brief     GUI related function
 * @author    Phil Lin
 * @since     2011-08-25
 * @date      2011-08-25
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
#define GUI_OBJ_CTRL_MAX    32

#if GUI_DBG_LV == 2
#define __STATIC
#else
#define __STATIC static
#endif

#define GUI_CORE_DBG_EN   0

#if GUI_CORE_DBG_EN
#define guiCorePrintf printf
#else
#define guiCorePrintf ;/DBG_SLASH
#endif

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
__STATIC guiObj_t xdata guiObj[GUI_OBJ_CTRL_MAX];
__STATIC guiObj_t xdata * xdata pObjAct;
__STATIC guiObj_t xdata * xdata pObjFree;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 guiObjInit(void)
 * @brief     guiObjInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-5
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjInit(
	void
)
{
	UINT8 i;
	
	memset (&guiObj[0], 0, sizeof(guiObj));

	for(i = 0; i < GUI_OBJ_CTRL_MAX - 1; i++)
	{
		guiObj[i].attr.idx = i;
		guiObj[i].pNext = &guiObj[i + 1];
	}

	guiObj[GUI_OBJ_CTRL_MAX - 1].attr.idx = GUI_OBJ_CTRL_MAX - 1;
	guiObj[GUI_OBJ_CTRL_MAX - 1].pNext = NULL;

	pObjAct = NULL;
	pObjFree = &guiObj[0];
	
	return SUCCESS;
}
/**
 * @fn        UINT8 guiObjNew(guiObj_t **pObj)
 * @brief     guiObjNew
 * @param     [out] ppObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjNew(
	guiObj_t **ppObj
)
{
	guiObj_t xdata *pAct;
	
	if (!pObjFree) {
		*ppObj = NULL;
		return FAIL;
	}

	if (!pObjAct) {
		pObjAct = pObjFree;
		pObjFree = pObjFree->pNext;
		pObjAct->pNext = NULL;
		
		*ppObj = pObjAct;
	} else {
		pAct = pObjAct;
		while(pAct->pNext) {
			pAct = pAct->pNext;
		}

		pAct->pNext = pObjFree;
		pAct = pAct->pNext;
		pObjFree = pObjFree->pNext;
		pAct->pNext = NULL;

		*ppObj = pAct;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 guiObjFree(guiObj_t *pObj)
 * @brief     guiObjFree
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-25
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjFree(
	guiObj_t *pObj
)
{
	guiObj_t xdata *pAct = pObjAct;
	guiObj_t xdata *pActPrev = pObjAct;

	if (!pObj) 
	{
		return SUCCESS;
	}

	if (pAct == pObj)
	{
		pObjAct = pAct->pNext;

		pAct->pNext = pObjFree;
		pObjFree = pAct;
	}
	else
	{
		while(pAct)
		{
			if (pAct == pObj)
			{
				break;
			}

			pActPrev = pAct;
			pAct = pAct->pNext;
		}

		if (!pAct) return FAIL;
			
		pActPrev->pNext = pAct->pNext;
		pAct->pNext = pObjFree;
		pObjFree = pAct;
	}

	/* pObjFree->attr = -1; */
	
	return SUCCESS;
}

/**
 * @fn        UINT8 guiObjLocate(SINT16 x, SINT16 y, guiObj_t **ppObj)
 * @brief     guiObjLocate
 * @param     [in] x
 * @param     [in] y
 * @param     [in] ppObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-1
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjLocate(
	SINT16 x,
	SINT16 y,
	guiObj_t **ppObj
)
{
	guiObj_t xdata *pAct = pObjAct;

	if (pAct) 
	{
		do 
		{
			if (x >= pAct->x0 && x <= pAct->x1 &&\
				y >= pAct->y0 && y <= pAct->y1 \
			) 
			{
				*ppObj = pAct;
				guiCorePrintf("L:%i,%i->%i,%i,%i,%i,%bu,0x%x\n", x, y, pAct->x0, pAct->y0, pAct->x1, pAct->y1,pAct->attr.idx,pAct->pObjExt->msg);
				return SUCCESS;
			}

			pAct = pAct->pNext;
		} while(pAct);
	}

	*ppObj = NULL;
	
	return FAIL;
}

