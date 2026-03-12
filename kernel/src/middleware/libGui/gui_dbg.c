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
 * @file      gui_dbg.c
 * @brief     GUI debug module
 * @author    Phil Lin
 * @since     2011-09-05
 * @date      2011-09-05
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "gui_dbg.h" 
#include "osd_api.h" 
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
UINT8 xdata guiDbgEn; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if GUI_DBG_LV == 2
extern guiObj_t xdata * xdata pObjAct;
extern guiObj_t xdata * xdata pObjFree;
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void guiDbgEnSet(UINT8 en)
 * @brief     guiDbgEnSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-5
 * @todo      N/A
 * @bug       N/A
*/
void
guiDbgEnSet(
	UINT8 en
)
{
	if (en) {
		guiDbgEn = 1;
	} else {
		guiDbgEn = 0;
	}
}

/**
 * @fn        UINT8 guiObjDraw(guiObj_t *pObj, UINT8 color)
 * @brief     guiObjDraw
 * @param     [in] pObj
 * @param     [in] color
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-5
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjDraw(
	guiObj_t *pObj,
	UINT8 color
)
{
	UINT8 ret = SUCCESS;
	
	if (!pObj || !guiDbgEn) return ret;
	
	osdIndexSave();
	ret |= osdPenCfg(color, 2);
	ret |= osdFrameDraw(pObj->x0, pObj->y0, pObj->x1 - pObj->x0, pObj->y1 - pObj->y0);
	osdIndexRestore();
	
	return ret;
}

/**
 * @fn        UINT8 guiObjClr(guiObj_t *pObj)
 * @brief     guiObjClr
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-1
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjClr(
	guiObj_t *pObj
)
{
	return guiObjDraw(pObj, 0);
}

#if GUI_DBG_LV == 2
/**
 * @fn        void guiObjDump(void)
 * @brief     guiObjDump
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-29
 * @todo      N/A
 * @bug       N/A
*/
void
guiObjDump(
	void
)
{
	UINT8 i;
	guiObj_t xdata *ptr = NULL;

#if 0
	printf("All\n");
	i = 0;
	while(i < GUI_OBJ_MAX) {
		printf("[%bu]:%bu\t", i, guiObj[i++]);
		if (!(i&3)) putchar('\n');
	}
#endif

	ptr = pObjAct;
	printf("\nAct\n");
	i = 0;
	while(ptr) {
		printf("[%bu]:%bu(%u,%u,%u,%u)\t", i++, ptr->attr.idx, ptr->x0, ptr->y0, ptr->x1, ptr->y1);
		ptr = ptr->pNext;
		if (!(i&1)) putchar('\n');
	}

	ptr = pObjFree;
	printf("\nFree\n");
	i = 0;
	while(ptr) {
		printf("[%bu]:%bu\t", i++, ptr->attr.idx);
		ptr = ptr->pNext;
		if (!(i&3)) putchar('\n');
	}
}
#endif /* GUI_DBG_LV == 2 */
