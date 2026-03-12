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
 * @file      dbg_wdt.c
 * @brief     WDT
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "reg_def.h" 
#include "dbg_wdt.h" 
#include "hal_global.h" 
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
static UINT32 xdata sWdtReload; 
static UINT8 xdata sWdtEn;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 dbgWdtCfg(UINT8 bWdtEn, wdtRstMode_t mode, UINT32 timeOut)
 * @brief     dbgWdtCfg
 * @param     [in] bWdtEn
 * @param     [in] mode
 * @param     [in] timeOut
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-7
 * @todo      N/A
 * @bug       N/A
*/
UINT32
dbgWdtCfg(
	UINT8 bWdtEn,
	wdtRstMode_t mode,
	UINT32 timeOut
)
{
	sWdtEn = bWdtEn;
	if (!bWdtEn) {
		sWdtReload = 0;
		return SUCCESS;
	}
	
	switch(mode) {
		case WDT_PROC_RELOAD:
			mode = WDT_PROC_RELOAD;
			break;
		case WDT_PROC_DBG_USB_ONLY:
			mode = WDT_PROC_DBG_USB_ONLY;
			break;
		case WDT_PROC_DBG_ALL:
		default:
			mode = WDT_PROC_DBG_ALL;
			break;
	}

	XBYTE[REG_Cpu_LPC0] = 0x01;	/* reset lpc */
	XBYTE[WDT_PROC_REG] = mode;
	sWdtReload = timeOut;
	
	return HAL_GlobalWDTCfg(timeOut, 0x00, bWdtEn);
}

/**
 * @fn        void dbgWdtFeed(UINT32 food)
 * @brief     dbgWdtFeed
 * @param     [in] food
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
void
dbgWdtFeed(
	UINT32 food
)
{
	if (sWdtEn) {
		HAL_GlobalWDTTimeLoad(food == -1 ? sWdtReload : food);
	}
}

/**
 * @fn        void dbgWdtSet(UINT8 en)
 * @brief     dbgWdtSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
void
dbgWdtSet(
	UINT8 en
)
{
	if (sWdtEn) {
		HAL_GlobalWDTSet(en);
	}
}
