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
#ifndef __SP1K_OSD_API_H__
#define __SP1K_OSD_API_H__

/** \file sp1k_osd_api.h
 * @brief		Spca1528 OSD related interfaces
 */

#include "osd_api.h"
#include "sp1k_def.h"
#include "osd_buff.h"
#include "sp1k_front_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn void sp1kOsdInit(UINT8 panel)
 * @brief   Initialize OSD related setting. This function must be called befor access other OSD funcitons
 *@param[in]panel	current display device type
 *\n		1: LCD
 *\n		0: TV
 * @retval none
 */
void sp1kOsdInit(UINT8 panel);

/**
 * @fn void sp1kOsdCharDraw(UINT8 x, UINT8 y, UINT16 idx, UINT8 attr)
 * @brief	Draws a character at a specific position.
 * @param[in] x    X position of OSD drawing area left side.
 * @param[in] y    Y position of OSD drawing area upper side.
 * @param[in] idx  index of the character in font database entries
 * @param[in]  attr  Please refer to attr description in sp1kOsdStrDraw() for more attribute details.
 * @retval none
 */
void sp1kOsdCharDraw(UINT8 x, UINT8 y, UINT16 idx, UINT8 attr);

/**
 * @fn void sp1kOsdStrDraw(UINT8 x, UINT8 y, UINT8 *str, UINT8 attr)
 * @brief	Draws a string ended by a zero character at a specific position
 * @param[in] x    X position of drawing area left side
 * @param[in] y    Y position of drawing area up side
 * @param[in] str  String ended by a zero character. The range of each entry is 0~255
 * @param[in] attr Set display attribute. -1 means keep the crrent setting
 *\n			bit[2:0]: foreground color index
 *\n 			bit3: background color enable. 1: Enable.
 *\n			bit[5:4]
 *\n			0: Normal mode
 *\n			1: Half-tone mode
 *\n			2: Exchange foreground/background color
 *\n			3: Blinking mode
 * @retval none
 */
void sp1kOsdStrDraw(UINT8 x, UINT8 y, UINT8 *str, UINT8 attr);

/**
 * @fn void sp1kOsdClear(UINT8 attr)
 * @brief   Clears OSD buffer with desired attribute
 * @param[in]  idx   Reserved, not used
 * @param[in]  attr  Please refer to attr description in sp1kOsdStrDraw() for more attribute details.
 * @retval none
 */
void sp1kOsdClear(UINT8 attr);

/**
 * @fn void sp1kOsdSelWinSet(UINT16 x0, UINT16 y0, UINT16 w, UINT16 h, UINT8 depth, UINT8 color_idx)
 * @brief   Set highlight retangle  dimension and its attribute
 * @param[in]  x0         x-axis coord of start point. -1 means keep the crrent setting
 * @param[in]  y0         y-axis coord of start point. -1 means keep the crrent setting
 * @param[in]  w          width of select window. -1 means keep the crrent setting
 * @param[in]  h          height of select window. -1 means keep the crrent setting
 * @param[in]  depeh      depth of select window. -1 means keep the crrent setting, 0 means "disable". Range is 1 to 15
 *\n                      The value should be one of the foreground color index
 * @param[in]  color_idx  color_idx of select window. -1 means keep the crrent setting. Range is 0 to 7
 * @retval none
  * @see sp1kOsdInit()
*/
void sp1kOsdSelWinSet(UINT16 x0, UINT16 y0, UINT16 w, UINT16 h, UINT8 depth, UINT8 color_idx);

/**
 * @fn void sp1kOsdLocalFlashSet(UINT8 frequency)
 * @brief	Specifes OSD blanking frequency
 * @param[in] level blanking frequency.
 *\n                0: every 64 frames
 *\n                1: every 32 frames
 *\n                2: every 16 frames
 *\n                3: every 8 frames
 * @retval none
 */
void sp1kOsdLocalFlashSet(UINT8 frequency);

/**
 * @fn void sp1kOsdPageDrawUpdate(UINT16 idx[], UINT8 attr[], UINT8 update)
 * @brief	Update whole OSD font index and display attribute.
 * @param[in] idx[]  Pointer to series of OSD font index.
 * @param[in] attr[] Pointer to series of display attribute.

 * @retval none
 */
void sp1kOsdPageDrawUpdate(UINT16 idx[], UINT8 attr[]);

/**
 * @fn void sp1kOsdLayoutAttrGet(UINT8* hNum, UINT8* vNum)
 * @brief	Obtain the numer of fonts in X and Y direction
 * @param[out] *hNum numer of fonts in X direction.
 * @param[out] *vNum numer of fonts in Y direction.
 * @retval none
 */
void sp1kOsdLayoutAttrGet(UINT8* hNum, UINT8* vNum);

/**
 * @fn void sp1kOsdFontSizeSet(UINT8 width, UINT8 height)
 * @brief	Set OSD font size, must be 16x32, 12x24 or 8x16.
 * @param[in] font width in unit of pixel.
 * @param[in] font height in unit of pixel.
 * @retval none
 */
void sp1kOsdFontSizeSet(UINT8 width, UINT8 height);

#else
#define sp1kOsdInit			    osdInit
#define sp1kOsdStrDraw          osdStrDraw
#define sp1kOsdStrDrawExt	osdStrDrawExt
#define sp1kOsdClear            osdClear
#define sp1kOsdClearChar		osdClearChar
#define sp1kOsdClearCharExt	osdClearCharExt
#define sp1kOsdSelWinSet        osdSelWinSet
#define sp1kOsdLocalFlashSet(a) //this function is removed in 1628
#define sp1kOsdCharDraw         osdCharDraw
#define sp1kOsdPageDrawUpdate   osdPageDrawUpdate
#define sp1kOsdLayoutAttrGet	osdLayoutAttrGet
#define sp1kOsdFontSizeSet      osdFontSizeSet
#define sp1kOsdFontSizeGet	osdFontSizeGet
#define sp1kOsdDispBuffSw	osdDispBuffSw
#define sp1kOsdDispBuffCpy	osdDispBuffCpy
#define sp1kOsdDispBuffShow		osdDispBuffShow
#define sp1kOsdDispBuffAddrGet	osdDispBuffAddrGet
#define sp1kOsdDispBuffSizeGet	osdDispBuffSizeGet
#define sp1kOsdIconShow				osdIconShow
#define sp1kOsdIconPsGet			osdIconPsGet
#define sp1kOsdIconWidthGet 	osdIconWidthGet
#define sp1kOsdIconHeightGet osdIconHeightGet
#define sp1kOsdSrcSizeSet	osdSrcSizeSet
#define sp1kOsdRectDraw	osdRectDraw
#define sp1kOsdPenCfg osdPenCfg
#define sp1kOsdIndexSave osdIndexSave
#define sp1kOsdIndexRestore osdIndexRestore
#define sp1kOsdLineDraw	osdLineDraw
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern void sp1kOsdLoad (void);

#endif //__SP1K_OSD_API_H__

