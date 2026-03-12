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
 *  Author: Phil.lin
 *
 *  Change Log:
 *
 *	sb01, 20111024 Guoqiang.li
 *		a. modify number of version
 *	sb01, 20111031 rf.wang
 *		a. fixed HD PV size=1280*720
 *	sb02, 20111031 Phil.lin
 *		a. add dram size and clock information print out
 *	sb03, 20111031 rf.wang
 *		a. fixed HD pv size = 6/8
 *	sb03, 20111104 jg.wang
 *		a. add video cmd for switch bg
 *	sb04, 20111110 LinJieCheng
 *		a. add Dzoom non-blocking flow
 *              b. removed old Dzoom non-blocking flow
 *	sb05, 20111125 rf.wang
 *		a. add 1st path size and buf check function
 *	sb06, 20120316 peng.tan
 *		a. add 3 global variable to support still image snap
 *	sb07, 20120727 rf.wang
 *		a. add blocking zoom and nonblocking zoom
 *	sb08, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$sb08";

/**************************************************************************
 *
 *  Function Name: sysLibVerGet
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
libSysVerGet(
	void
)
{
	return (verStr + 3);
}

