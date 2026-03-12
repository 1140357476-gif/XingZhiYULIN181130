/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20111024 wangjg
 *		a. add sd bg resume function.
 *	v03, 20111024 wangjg
 *		a. Disable sd bg resume function.
 *	v04, 20111024 wangjg
 *		a. sd bg add dummy cmd support.
 *	v05, 20111115 wangjg
 *		a. add bg mode 1
 *	v06, 20111116 wangjg
 *           a. add bg mode 2
 *	v07, 20111117 wangjg
 *           a. add bg mode 3
 *	v08, 20111124 wangjg
 *           a. add bg mode 4
 *	v09, 20111214 wangjg
 *           a. disable time printf
 *	v0a, 20111214 wangjg
 *           a. for bg mode 6,correct openflie freesize error.
 *	v0b, 20120106 wangjg
 *           a. clear sd disable interrupt
 *	v0c, 20120119 wangjg
 *           a. add bgFileJoint function
 *	v0d, 20120206 wangjg
 *           a. for bgFileFat, clusterLast is no value,FAT_FirstFreeCluster not update,load to free space is not release.
 *	v0e, 20120211 wangjg
 *           a. correct bgFile buf overflow bug.
 *	v0f, 20131030 xianxin
 *           a. fix bgWrite error when list overflow
 *	v10, 20140704 xianxin
 *           a. some card error when cmd12 follows the cmd25 without dma
 *
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                  E X T E R N A L    R E F E R E N C E                  *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 code verStr[] = "$sd10";

/**************************************************************************
 *                                                                        *
 *  Function Name: sdLibVerGet                                            *
 *                                                                        *
 *  Purposes: Get current libaray's version.                              *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 *
libStorageSdVerGet(
	void
)
{
	return (verStr + 3);
}

