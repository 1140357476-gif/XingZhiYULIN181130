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
 *  wangjg
 *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20111031 renhy
 *		a. modify exif.dat 
 *		b. modify exifCreateFile interface 
 *	v03, 20121116 xianxin
 *		a. fix parse app1 error (variable offset) and max app1count(more dht, dqt)
 *		b. support no appn (for speed capture)
 *	v04, 20121128 xianxin
 *		a. fix 46334
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ef04";

/**************************************************************************
 *
 *  Function Name: exifLibVerGet
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
libExifVerGet(
	void
)
{
	return (verStr + 3);
}

