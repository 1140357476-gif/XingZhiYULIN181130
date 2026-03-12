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
 * @file      gui_api.h
 * @brief     GUI api
 * @author    Phil Lin
 * @since     2011-09-05
 * @date      2011-09-05
*/

#ifndef __GUI_API_H__
#define __GUI_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "gui_core.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum guiOpMode_e {
	GUI_OP_MODE_PENDN_ONLY = (unsigned char)0,
	GUI_OP_MODE_PENUP = (unsigned char)1,
	GUI_OP_MODE_SLIDE_EN = (unsigned char)2,
	GUI_OP_MODE_MAX,
} guiOpMode_t;

typedef enum guiOpModePenUp_e {
	MODE_PENUP_BY_PENDN = (unsigned char)0,
	MODE_PENUP_ALWAYS = (unsigned char)1,
} guiOpModePenUp_t;


typedef enum guiOpPara_e {
	GUI_OP_PARA_SLIDE_THD_X = (unsigned char)0,
	GUI_OP_PARA_SLIDE_THD_Y = (unsigned char)1,
	GUI_OP_PARA_MAX,
} guiOpPara_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 guiObjCreate(guiObj_t *pObjAttr, guiObj_t **ppObjHdl);
UINT8 guiObjDestory(guiObj_t *pObj);

UINT8 guiObjOpModeSet(UINT8 modeIdx, UINT8 modeVal);
UINT8 guiObjOpModeGet(UINT8 modeIdx);
UINT8 guiObjOpParaSet(UINT8 paraIdx, UINT32 paraVal);
UINT32 guiObjOpParaGet(UINT8 paraIdx);

#endif  /* __GUI_API_H__ */

