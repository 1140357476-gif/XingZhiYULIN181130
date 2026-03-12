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
 * @file		disp_para.c
 * @brief		set display parameters.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-04-29
 * @date		2008-08-05
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

#include "hal.h"
#include "hal_dram.h"
#include "hal_disp.h"
#include "hal_gprm.h"
#include "disp_api.h"

#include "osd_api.h"

#if MODULE_OSD_OPT
/**
 * @brief	set parameters for OSD select window
 * @param	x0       : x-axis coord of start point, if w=-1, ignore this item
 * @param	y0       : x-axis coord of start point, if h=-1, ignore this item
 * @param	w        : width of select window, -1 is means "don't change"
 * @param	h        : high of select window, -1 is means "don't change"
 * @param	depth    : depth of select window, -1 is means "don't change", 0 is means "disable", 1~15
 * @param	color_idx: color_idx of select window, -1 is means "don't change", 0~7
 * @retval	NONE
 * @author	Ted Chiu
 * @since	07/12/27 10:12
 */
void osdSelWinSet(UINT16 x0, UINT16 y0, UINT16 w, UINT16 h, UINT8 depth, UINT8 color_idx)
{

	color_idx = color_idx & 0x3F;
	osdIndexSave();
	osdPenCfg(0xC0 | color_idx, depth); 
	osdFrameDraw(x0, y0, w, h);
	osdIndexRestore();
}

/**
 * @fn        void osdRegUpdate(void)
 * @brief     osdRegUpdate
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
void
osdRegUpdate(
	void
)
{
	//HAL_DispSignalWait(HAL_DISP_WAIT_VSYNC, HAL_DISP_WAIT_RISING);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_OSD, 0);
}
#endif /* MODULE_OSD_OPT */