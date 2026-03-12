/**************************************************************************
 *
 *        Copyright (c) 2002-2008 by Sunplus mMedia Inc., Ltd.
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
 *	ae01, 20111021 LinJieCheng
 *		a. modify number of version
 *	ae02, 20111031 LinJieCheng
 *		a. add HQ preview function
 *	ae03, 20111110 LinJieCheng
 *		a. add Dzoom non-blocking flow
 *              b. removed old Dzoom non-blocking flow
 *	ae04, 20111112 LinJieCheng
 *		a. add 1080P FHD AE FLOW (8 frame)
 *	ae05, 20111112 LinJieCheng
 *		a. fixed 1080P FHD AE FLOW (8 frame)
 *	ae06, 20111122 LinJieCheng
 *		a. add host ctrl ae speed value
 *	ae07, 20111207 LinJieCheng
 *		a. add AE Jump step speed ctrl parameter (SP1K_AE_JUMP_STEP_NORMAL = 0, SP1K_AE_JUMP_STEP_DIV2,	SP1K_AE_JUMP_STEP_DIV3,	SP1K_AE_JUMP_STEP_DIV
 *	ae08, 20120727 LinJieCheng
 *		a. add capture ae/flash/bshutter/mshutter header information
 *	ae09, 20120801 LinJieCheng
 *		a. add capture ae/flash/bshutter/mshutter header information
 *	ae10, 20120806 LinJieCheng
 *		a. fixed capture ae/flash/bshutter/mshutter header information
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ae10";

/**************************************************************************
 *
 *  Function Name: audLibVerGet
 *
 *  Purposes: Get current libaray's version.
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
UINT8 *
libAeVerGet(
	void
)
{
	return (verStr + 3);
}

