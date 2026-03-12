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
 * @file      front_para.c
 * @brief     set front parameters
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_front.h"
#include "front_logi.h" 
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

/**
 * @fn        UINT8 frontParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd)
 * @brief     set front misc parameters
 * @param     [in] id : parameter id, one of frontParaId_e
 * @param     [in] para1 : parameter #1
 * @param     [in] para2 : parameter #2
 * @param     [in] para3 : parameter #3
 * @param     [in] para4 : parameter #4
 * @param     [in] upd : 0: immediately update, 1: sync to frontvd falling or eof
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/

#if !FRONT_USE_MACRO
#if 0
UINT8
frontParaSet(
	UINT8 id,
	SINT16 para1,
	SINT16 para2,
	SINT16 para3,
	SINT16 para4,
	UINT8 upd
)
{
	UINT16 ret;



	if ( id < FRONT_PARA_MAX_ID ) {
		ret = HAL_FrontParaSet(id, para1, para2, para3, para4, upd);

		return ret;
	}
	else {
		return FAIL;
	}
}
#endif
#endif


