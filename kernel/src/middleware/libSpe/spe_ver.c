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
 *  Author: wangjg
 *
 *  Change Log:
 *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20120222 wangdongdong
 *		a. add distortion
 *	v03, 20120224 wangdongdong
 *		a. modify rotate distortion
 *	v04, 20120228wangdongdong
 *		a. decrease time of distortion
 *	v05, 20120301 dongdong.wang
 *		a. modify for distortion
 *	v06,20120302 dongdong.wang
 *		a.modify for distortion hung up
 *	v07,20120921 xianxin
 *		a.fix g_pb_rec_RotOut as 320x240 because pb decode size is variable
 *	v08,20121009 xianxin
 *		a.fix hourglass error caused by read res file after dram mapping, refine roi calculation
 *	v09,20121031 xianxin
 *		a.fix black border error because valid region is at top left
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$sp09";

/**************************************************************************
 *
 *  Function Name: spLibVerGet
 *
 *  Purposes:
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
libSpeVerGet(
	void
)
{
	return (verStr + 3);
}

