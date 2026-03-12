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
 * @file      osd_api.h
 * @brief     osd base api definition
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __OSD_API_H__
#define __OSD_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "osd_com.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define osdIndexSave()       osdIndexSetRestore(1)
#define osdIndexRestore()    osdIndexSetRestore(0)
 
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
#if MODULE_OSD_OPT

void osdInit(UINT8 panel);
void osdPaletteInit(void);
UINT8 osdMapTabSet(UINT8 idxBg, UINT8 idxOutLine, UINT8 idxBld0, UINT8 idxBld1);
void osdIndexSetRestore(UINT8 save);
void osdBldColorSet(UINT8 bldColor);
void osdLocalBlendSet(UINT8 idx, UINT8 val);

void osdRegUpdate(void);
void osdPageDrawUpdate(UINT16 idx[], UINT8 attr[]);

UINT8 osdFmtCfg(osdFmt_t cFmt, osdFmt_t iFmt);
void osdFmtGet(const gOsdFmt_t** ppFmt);

void osdLayoutAttrGet(UINT8* hNum, UINT8* vNum);
void osdLayoutAttrSet(UINT8 hNum, UINT8 vNum);
void osdFontSizeSet(UINT8 width, UINT8 height);
void osdFontSizeGet(UINT16* width,	UINT16* height);

UINT8 osdPenCfg(UINT16 color, UINT16 width);
void osdPenGet(const osdPen_t** pPen);

void osdClear(UINT8 attr);
void osdClearChar(SINT16 x,SINT16 y,UINT16 w,UINT16 h);
void osdClearCharExt(SINT16 x,SINT16 y,UINT16 w,UINT16 h);

UINT8 osdLineDraw(SINT16 x0, SINT16 y0, SINT16 x1, SINT16 y1);
UINT8 osdFrameDraw(SINT16 x, SINT16 y, UINT16 w, UINT16 h);
UINT8 osdRectDraw(SINT16 x, SINT16 y, UINT16 w, UINT16 h);
void osdSelWinSet(UINT16 x0, UINT16 y0, UINT16 w, UINT16 h, UINT8 depth, UINT8 color_idx);

UINT8 osdCharDraw(UINT8 x, UINT8 y, UINT16 idx, UINT8 attr);
UINT16 osdCharDrawExt(UINT16 x, UINT16 y, UINT16 idx,UINT8 bldOpt);
void osdStrDraw(UINT8 x, UINT8 y, UINT8 *str, UINT8 attr);
void osdStrDrawExt(UINT16 x, UINT16 y, UINT8 *str, UINT8 attr);
UINT16 osdIconDrawExt(UINT16 x, UINT16 y, UINT16 idx,UINT8 bldOpt);
void osdIconShow(UINT16 iconId,UINT8 show);

UINT16 osdCharWidthGet(UINT16 idx);
UINT16 osdIconWidthGet(UINT16 idx);
UINT16 osdIconHeightGet(UINT16 idx);
UINT16 osdStrWidthGet(UINT8 *str);
void osdIconPsTabAddrSet(UINT32 addr);
UINT32 osdIconPsTabAddrGet(void);
void osdIconPsGet(UINT16 idx, UINT16 *x,UINT16 *y);
#else

#define osdInit(panel)
#define osdPaletteInit()
#define osdMapTabSet(idxBg, idxOutLine, idxBld0, idxBld1)
#define osdIndexSetRestore(save)
#define osdBldColorSet(bldColor)
#define osdLocalBlendSet(idx, val)

#define osdRegUpdate()
#define osdPageDrawUpdate(idx, attr)

#define osdFmtCfg(cFmt, iFmt)	SUCCESS /* the return value */
#define osdFmtGet(ppFmt)	(*(ppFmt) = NULL)

#define osdLayoutAttrGet(hNum, vNum)	(*(hNum) = *(vNum) = 0)
#define osdLayoutAttrSet(hNum, vNum)
#define osdFontSizeSet(width, height)
#define osdFontSizeGet(width, height)	(*(width) = *(height) = 0)

#define osdPenCfg(color, width)	SUCCESS /* the return value */
#define osdPenGet(pPen) (*(pPen) = NULL)

#define osdClear(attr)
#define osdClearChar(x,y,w,h)
#define osdClearCharExt(x,y,w,h)

#define osdLineDraw(x0, y0, x1, y1)	SUCCESS /* the return value */
#define osdFrameDraw(x, y, w, h)	SUCCESS /* the return value */
#define osdRectDraw(x, y, w, h)	SUCCESS /* the return value */
#define osdSelWinSet(x0, y0, w, h, depth, color_idx)

#define osdCharDraw(x, y, idx, attr)
#define osdCharDrawExt(x, y, idx, bldOpt)
#define osdStrDraw(x, y, str, attr)
#define osdStrDrawExt(x, y, str, attr)
#define osdIconDrawExt(x, y, idx, bldOpt)
#define osdIconShow(iconId, show)

#define osdCharWidthGet(idx)	0 /* the return value */
#define osdIconWidthGet(idx)	0 /* the return value */
#define osdIconHeightGet(idx)	0 /* the return value */
#define osdStrWidthGet(str)	0 /* the return value */
#define osdIconPsTabAddrSet(addr)
#define osdIconPsTabAddrGet()	0 /* the return value */
#define osdIconPsGet(idx, x, y)	(*(x) = *(y) = 0xA5)

#endif /* MODULE_OSD_OPT */

#endif  /* __OSD_COM_H__ */

