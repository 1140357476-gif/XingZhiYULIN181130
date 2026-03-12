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
 *  Author: Wen-Guo Gan
 *  1000, 20080813 Matt Wang
 *    a. add osd_libver.c.
 *  1001, 20080818 Matt Wang
 *    a. recalculate osd window and scale factor when setting font size.
 *    b. fix osd vertical scale factor calculation (rounding up).
 *  1002, 20080827 Matt Wang
 *    a. fix bottom white horizontal line (osd not cleared completely).
 *  1003, 20080904 Matt Wang
 *    a. [0031406] becasue of hw bug for tv left side green line, use sw
 *       method to enable/disable osd layer.
 *  1004, 20081107 Phil.lin
 *    a. [0032004] speed up osd clear function
 *  1005, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1006, 20081218 Matt Wang
 *    a. [0032036] clear extra 2 osd buffer rows in osdInit() to avoid bottom noise.
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$od01";

/**************************************************************************
 *
 *  Function Name: osdLibVerGet
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
libOsdVerGet(
	void
)
{
	return (verStr + 3);
}

