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
 * @file      gui_core.h
 * @brief     GUI releated function definition
 * @author    Phil Lin
 * @since     2011-08-29
 * @date      2011-08-29
*/

#ifndef __GUI_CORE_H__
#define __GUI_CORE_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct guiObjExt_s {

	/* Object Message */
	UINT16 msg;

	/* Object Attribution */
	
	UINT16 resIdx0; /* the icon index for object get focus */
	UINT16 resIdx1; /* the icon index for object lost focus */
	
	/* Object Event */

	UINT8 (code *pPenDn)(void *ptr);
	UINT8 (code *pFocusGet)(void *ptr);
	UINT8 (code *pFocusLost)(void *ptr);

	/* Event Param */
	
	void *pPenUpPara;
	void *pPenDnPara;
	void *pFocusGetPara;
	void *pFocusLostPara;
	void *penRepeateEn;
} guiObjExt_t;

typedef struct guiObj_s 
{

	/* Object Attribution */
	
	struct 
	{
		UINT8 idx:6;
		UINT8 focus:1;
		UINT8 enable:1;
	} attr;
	
	SINT16 x0;
	SINT16 y0;
	SINT16 x1;
	SINT16 y1;
	
	/* Object Event */
	
	UINT8 (code *pPenUp)(void *ptr);
	
	/* Misc Control */
	
	struct guiObj_s xdata *pNext;
	guiObjExt_t xdata *pObjExt;
	
} guiObj_t;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 guiObjInit(void);
UINT8 guiObjNew(guiObj_t **ppObj);
UINT8 guiObjFree(guiObj_t *pObj);
UINT8 guiObjLocate(SINT16 x, SINT16 y, guiObj_t **ppObj);

#endif  /* __GUI_CORE_H__ */

