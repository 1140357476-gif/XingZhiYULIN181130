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
 * @file      osd_palette.c
 * @brief     osd palette related function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

#include <stdio.h>
#include "general.h"

#include "hal.h"
#include "hal_disp.h"

#include "osd_api.h"

#if MODULE_OSD_OPT 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define RGB565(r, g, b) \
(\
	(UINT16)(((UINT16)((r) & 0xF8)) << (11-3)) | \
	(UINT16)(((UINT16)((g) & 0xFC)) << (5-2) ) | \
	(UINT16)(((UINT16)((b) & 0xF8)) >> (3)   )\
)
 
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

/**
 * @fn        void osdLocalBlendSet(UINT8 idx, UINT8 val)
 * @brief     osdLocalBlendSet
 * @param     [in] idx
 * @param     [in] val
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void
osdLocalBlendSet(
	UINT8 idx,
	UINT8 val
)
{
	HAL_DispParaSet(HAL_DISP_PARA_OSD_BLEND_FACTOR, idx, val, 0, 0, 0);
}

/**
 * @fn        void osdPaletteInit(void)
 * @brief     osdPaletteInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
void 
osdPaletteInit(
    void
)
{
#if 0
	UINT16 osdTb[64]= {

	    RGB565(0xff,0xff,0xff), RGB565(0x00,0x00,0x00), RGB565(0xF0,0x73,0x91), RGB565(0xDE,0xEA,0x86),
	    RGB565(0x60,0x60,0x20), RGB565(0x85,0xC7,0xF5), RGB565(0x88,0x88,0x88), RGB565(0x49,0x53,0x67),

	    RGB565(0x7f,0x00,0x00), RGB565(0x00,0x7f,0x00), RGB565(0x00,0x00,0x7f), RGB565(0x7f,0x7f,0x00),
	    RGB565(0x7f,0x00,0x7f), RGB565(0x00,0x7f,0x7f), RGB565(0x7f,0x7f,0x7f), RGB565(0xc0,0xc0,0xc0),

	    RGB565(0x7f,0x7f,0xaf), RGB565(0x6f,0x6f,0xaf), RGB565(0x5f,0x5f,0xaf), RGB565(0x4f,0x4f,0xaf),
	    RGB565(0x3f,0x3f,0xaf), RGB565(0x2f,0x2f,0xaf), RGB565(0x1f,0x1f,0xaf), RGB565(0x0f,0x0f,0xaf),
	    RGB565(0x0f,0x0f,0xaf), RGB565(0x0f,0x0f,0x9f), RGB565(0x0f,0x0f,0x8f), RGB565(0x0f,0x0f,0x7f),
	    RGB565(0x0f,0x0f,0x6f), RGB565(0x0f,0x0f,0x5f), RGB565(0x0f,0x0f,0x4f), RGB565(0x0f,0x0f,0x3f),
	};
#else
	UINT16 osdTb[64];
	#if 0
	UINT16 map_r[4] = {RGB565(0x00,0x00,0x00), RGB565(0x55,0x00,0x00), RGB565(0xaa,0x00,0x00), RGB565(0xff,0x00,0x00)};
	UINT16 map_g[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x55,0x00), RGB565(0x00,0xaa,0x00), RGB565(0x00,0xff,0x00)};
	UINT16 map_b[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x00,0x55), RGB565(0x00,0x00,0xaa), RGB565(0x00,0x00,0xff)};
	#elif 1
	UINT16 map_r[4] = {RGB565(0x00,0x00,0x00), RGB565(0x80,0x00,0x00), RGB565(0xc0,0x00,0x00), RGB565(0xff,0x00,0x00)};
	UINT16 map_g[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x80,0x00), RGB565(0x00,0xc0,0x00), RGB565(0x00,0xff,0x00)};
	UINT16 map_b[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x00,0x80), RGB565(0x00,0x00,0xc0), RGB565(0x00,0x00,0xff)};
	#else
	UINT16 map_r[4] = {RGB565(0x00,0x00,0x00), RGB565(0x40,0x00,0x00), RGB565(0x80,0x00,0x00), RGB565(0xc0,0x00,0x00)};
	UINT16 map_g[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x40,0x00), RGB565(0x00,0x80,0x00), RGB565(0x00,0xc0,0x00)};
	UINT16 map_b[4] = {RGB565(0x00,0x00,0x00), RGB565(0x00,0x00,0x40), RGB565(0x00,0x00,0x80), RGB565(0x00,0x00,0xc0)};
	#endif
	UINT8 r, g, b;
	UINT8 i=0;
	for (r=0; r<4; r++)
		for (g=0; g<4; g++)
			for (b=0; b<4; b++)
				osdTb[i++] = map_r[r] | map_g[g] | map_b[b];
#endif
    HAL_DispOsdPaletteInit((UINT32)osdTb, 0);
}
#endif /* MODULE_OSD_OPT */