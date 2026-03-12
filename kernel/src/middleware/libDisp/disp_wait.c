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
 * @file		disp_wait.c
 * @brief		wait for display signal event.
 * @author		Matt Wang
 * @since		2008-06-05
 * @date		2008-06-05
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_disp.h"
#include "disp_api.h"

/**
 * @brief	wait for signal event.
 * @param	src = [in] one of dispWaitSource_e.
 * @param	mode = [in] one of dispWaitMode_e.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-06-05
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispSignalWait(
	UINT8 src,
	UINT8 mode
)
{
	return HAL_DispSignalWait(src, mode);
}

