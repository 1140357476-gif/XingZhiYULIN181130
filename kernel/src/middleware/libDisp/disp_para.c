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
 * @author		Matt Wang
 * @since		2008-05-26
 * @date		2008-06-05
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_disp.h"
#include "disp_api.h"

/**
 * @brief	set disp misc parameters.
 * @param	id = [in] parameter id, one of dispParaId_e.
 * @param	para1 = [in] parameter #1.
 * @param	para2 = [in] parameter #2.
 * @param	para3 = [in] parameter #3.
 * @param	para4 = [in] parameter #4.
 * @param	upd = [in] 0: no update, 1: zfifo update, 2: cpu update, 3: sof update.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-26
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispParaSet(
	UINT8 id,
	SINT16 para1,
	SINT16 para2,
	SINT16 para3,
	SINT16 para4,
	UINT8 upd
)
{
	UINT16 ret;


	if ( id < DISP_PARA_MAX_ID ) {
		ret = HAL_DispParaSet(id, para1, para2, para3, para4, upd);

		return ret;
	}
	else {
		return FAIL;
	}
}

