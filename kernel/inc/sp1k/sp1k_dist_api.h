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
 * @file      sp1k_dist_api.h
 * @brief     sp1k DIST API for Host
 * @author    tq.zhang
 * @since     2010-05-18
 * @date      2010-05-18
*/

#ifndef __SP1K_DIST_API_H__
#define __SP1K_DIST_API_H__

/** \file sp1k_dist_api.h
 * @brief		Spca1628 DIST related interface.
 */

#include "sp1k_def.h"
#include "sp1k_front_api.h"

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN 
/**
 * @fn        UINT8 sp1kDistStatusGet(void)
 * @brief     Get Dist Status
 * @param     NONE
 * @retval    return  1: working  0: off
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kDistStatusGet(void);
#else
#define sp1kDistStatusGet()	sp1kVdProcEnable(VD_PROC_DIST,0xff)?1:0 
#endif
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
 * @fn        void sp1kDistEnable(void)
 * @brief     enable dist function
 * @param     [in] updateDisp
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kDistEnable();

/**
 * @fn        void sp1kDistDisable(void)
 * @brief     disable dist function
 * @param     [in] updateDisp
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kDistDisable();

/**
 * @fn		  void distInit(void)
 * @brief	  Dist initial parameters set
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kDistInit();

//void sp1kDistReset(void);

#endif //__SP1K_DIST_API_H__
