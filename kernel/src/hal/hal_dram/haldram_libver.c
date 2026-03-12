/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
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
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20111031 wangjg
 *           a. Clean bg *** WARNING L15: MULTIPLE CALL TO FUNCTION.
 *	v03, 20120222 linjie.cheng
 *           a. add intr call function
 *           b. add front2cdsp disable not need wait zf update done
 *	v04, 20130318 linjie.cheng
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
static UINT8 code verStr[] = "$hr04";

/**************************************************************************
 *
 *  Function Name: halDramLibVerGet
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
libHalDramVerGet(
	void
)
{
	return (verStr + 3);
}

