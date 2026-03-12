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
 *  Author: hy.ren
 *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20111031 renhy
 *		a. support timelapse video file play
 *	v03, 2011221 xianxin
 *		a. add timeout for 43427
 *	v04, 2011223 xianxin
 *		a. use [0x2002] reset jpg
 *	v05, 20120112 rf.wang
 *		a. modify video decode HD 960*540 to 640*360
 * 	v06, 20130304 xianxin
 *		a. Add slow pb  with sp1kVideoPbSlowFWSpeedSet()
 * 	v07,20130325 kai.wang
 *		a. Add slow pb  with sp1kVideoPbSlowRWSpeedSet()
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$vd07";

/**************************************************************************
 *
 *  Function Name: snapLibVerGet
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
libVideoPbVerGet(
	void
)
{
	return (verStr + 3);
}

