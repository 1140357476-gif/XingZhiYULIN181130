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
 * @file      gui_tp.c
 * @brief     touch panel scan
 * @author    Phil Lin
 * @since     2012-02-01
 * @date      2012-02-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gui_tp.h"
#include "os_msg.h"
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
UINT8 G_tpPenState = TP_WAITINTR;

static UINT8 tpScanProc;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 tpScanProcGet(void)
 * @brief     tpScanProcGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 
tpScanProcGet(
	void
)
{
	return tpScanProc;
}

/**
 * @fn        void tpScanProcAdd(UINT8 proc)
 * @brief     tpScanProcAdd
 * @param     [in] proc
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void 
tpScanProcAdd(
	UINT8 proc
)
{
	tpScanProc |= proc;
}

/**
 * @fn        void tpScanProcHdl(UINT8 proc)
 * @brief     tpScanProcHdl
 * @param     [in] proc
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
tpScanProcHdl(
	UINT8 proc
)
{
	if( (tpScanProcGet() & TP_PROC_PU) &&(tpScanProcGet() & TP_PROC_PU))
	{
		osMsgPost(SP1K_MSG_GUI_PEN_UP);
	}
	tpScanProc &= ~proc;
}

/**
 * @fn        UINT8 tpCurrStsGet(void)
 * @brief     tpCurrStsGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
tpCurrStsGet(
	void
)
{
	return G_tpPenState;
}

/**
 * @fn        void tpCurrStsSet(UINT8 sts)
 * @brief     tpCurrStsSet
 * @param     [in] sts
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
tpCurrStsSet(
	UINT8 sts
)
{
	G_tpPenState = sts;
}

