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
 * @file		front_wait.c
 * @brief		wait for front signal event.
 * @author		Matt Wang
 * @since		2008-09-08
 * @date		2008-10-13
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_front.h"
#include "front_logi.h"

#if !FRONT_USE_MACRO
/**
 * @brief	wait for signal event.
 * @param	src = [in] one of frontWaitSource_e.
 * @param	mode = [in] one of frontWaitMode_e.
 * @param	num = [in] number of event to wait (only for rising or falling).
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-09-08
 * @todo	N/A
 * @bug		N/A
*/
UINT8
frontSignalWait(
	UINT8 src,
	UINT8 mode,
	UINT8 num
)
{
	return HAL_FrontSignalWait(src, mode, num);
}
#endif

