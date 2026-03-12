/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		osd_misc.c
 * @brief		set display parameters.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-04-29
 * @date		2008-12-18
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

#include "hal_disp.h"

#include "disp_api.h"
#include "osd_api.h"

#if MODULE_OSD_OPT
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static xdata UINT8 osdMaxX;
static xdata UINT8 osdMaxY;
static xdata UINT8 osdFontW = 16;
static xdata UINT8 osdFontH = 32;

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S
 **************************************************************************/
static void osdLayoutAttrSet(UINT8 hNum, UINT8 vNum);
/**************************************************************************
 *
 *  Function Name: osdLayoutAttrGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
void
osdLayoutAttrGet(
	UINT8* hNum,
	UINT8* vNum
)
{
#if TAG_TODO
	*hNum = osdMaxX;
	*vNum = osdMaxY;
#else	
	*hNum = 20;
	*vNum = 7;
#endif
}

/**************************************************************************
 *
 *  Function Name: osdLayoutAttrSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
static void
osdLayoutAttrSet(
	UINT8 hNum,
	UINT8 vNum
)
{
	osdMaxX = hNum;
	osdMaxY = vNum;
}

/**************************************************************************
 *
 *  Function Name: osdFontSizeSet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
void
osdFontSizeSet(
	UINT8 width,
	UINT8 height
)
{
	osdFontW = width;
	osdFontH = height;
	//HAL_DispParaSet(HAL_DISP_PARA_OSD_FONT_SIZE, width, height, 0, 0, 0);
}

/**
 * @fn        void osdFontSizeGet(UINT16* width, UINT16* height)
 * @brief     osdFontSizeGet
 * @param     [in] width
 * @param     [in] height
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
void
osdFontSizeGet(
	UINT16* width,
	UINT16* height
)
{
	if (width) *width = osdFontW;
	if (height) *height = osdFontH;
}
#endif /* MODULE_OSD_OPT */