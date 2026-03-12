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
 * @file      osd_init.c
 * @brief     osd initialization related functions
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_OSD_INIT__
#include "general.h" 
#include "dbg_def.h" 

#include "hal.h"
#include "hal_dram.h"
#include "hal_disp.h"

#include "osd_com.h"
#include "osd_api.h"
#include "osd_mem.h"
#include "osd_buff.h"

#if MODULE_OSD_OPT
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
static gOsdFmt_t xdata gOsdFmt;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/**
 * @fn        void osdInit(UINT8 panel)
 * @brief     osdInit
 * @param     [in] panel
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
void
osdInit(
	UINT8 panel
)
{
//	UINT8 i, maxy;
	UINT16 buffW, buffH;

	//osdIsPanel = panel;
	panel = panel;

	/* osd system memory initialization */
	osdMemInit();

	/* display buffer initializaion */
	osdDispBuffInit();
	osdDispBuffSizeGet(&buffW, &buffH);
	/* implement to H/W */
	HAL_DispOsdBuffAddrSet(osdDispBuffAddrGet(1));
	HAL_DispOsdBuffSizeSet(buffW, buffH);

	/* set osd layer position. */
	//HAL_DispParaSet(HAL_DISP_PARA_OSD_WINDOW_OFFSET, 0, 0, 0, 0, 0);
	//HAL_DispParaSet(HAL_DISP_PARA_OSD_WINDOW_SCROLL, 0, 0, 0, 0, 0);
	
	/* init palette and blending. */
	osdPaletteInit();
	
	osdLocalBlendSet(0, 0x00);
	osdLocalBlendSet(1, 0x3f);
	osdLocalBlendSet(2, 0x0c);
	osdLocalBlendSet(3, 0x3f);

	HAL_DispOsdScaleUpdate();
	HAL_DispParaSet(HAL_DISP_PARA_OSD_LAYER_ENABLE, 0, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
}

/**
 * @fn        void osdFmtGet(const gOsdFmt_t** ppFmt)
 * @brief     osdFmtGet
 * @param     [in] ppFmt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
void osdFmtGet(
	const gOsdFmt_t** ppFmt
)
{
	*ppFmt = &gOsdFmt;
}

/**
 * @fn        UINT8 osdFmtCfg(osdFmt_t cFmt, osdFmt_t iFmt)
 * @brief     osdFmtCfg
 * @param     [in] cFmt
 * @param     [in] iFmt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdFmtCfg(
	osdFmt_t cFmt,
	osdFmt_t iFmt
)
{
	if ((cFmt != OSD_FMT_2BIT && cFmt != OSD_FMT_8BIT) || \
		(iFmt != OSD_FMT_2BIT && iFmt != OSD_FMT_8BIT)) {
		ASSERT(0, 1);
		return FAIL;
	}
	
	gOsdFmt.lang = cFmt;
	gOsdFmt.icon = iFmt;
	
	return SUCCESS;
}
#endif /* MODULE_OSD_OPT */