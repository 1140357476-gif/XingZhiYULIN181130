
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
#ifndef __SP1K_AF_API_H__
#define __SP1K_AF_API_H__

/** \file sp1k_af_api.h
 * @brief		Spca1628 AF related interface which include: a)Initial setting, b) Parameter setting, C) Mode setting.
 */
 
#include "af_api.h"
#include "sp1k_af_api.h"

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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn void sp1kAfWinEnSet(UINT8 en)
 * @brief	      Set AF enable set (unit:fps)
 * @param[in] en				enable/disable
 * @retval none
 */
void sp1kAfWinEnSet(UINT8 en);

/**
 * @fn UINT8 sp1kAfWinEnGet(void)
 * @brief	      Get Enable/Disable AF status
 * @param	NULL
 * @retval     AF Enable/Disable
 */
UINT8 sp1kAfWinEnGet(void);

/**
 * @fn void sp1kAfWinCfg(UINT16 frontHSize,UINT16 frontVSize,UINT16 cropHSize,UINT16 cropVSize)
 * @brief		  Set AF Window Config
 * @param	[in]frontHSize : sensor output h size
 * @param     [in]frontVSize : sensor output v size
 * @param	[in]cropHSize : roi h size
 * @param     [in]cropVSize : roi v size
 * @retval none
 */
void sp1kAfWinCfg(UINT16 frontHSize,UINT16 frontVSize,UINT16 cropHSize,UINT16 cropVSize);

/**
 * @brief	 Get AF Value
 * @param	NULL
 * @author	LinJieCheng
 * @todo
 * @return	AF Value
 * @since	2012-5-10
 */
/**
 * @fn UINT32 sp1kAfWinValueGet(void)
 * @brief		Get AF Value
 * @param	NULL
 * @retval     AF Value
 */
UINT32 sp1kAfWinValueGet(void);


#else

#define sp1kAfWinEnSet afWinEnSet
#define sp1kAfWinEnGet afWinEnGet
#define sp1kAfWinCfg afWinCfg
#define sp1kAfWinValueGet afWinValueGet

#endif

#endif //__SP1K_AF_API_H__

