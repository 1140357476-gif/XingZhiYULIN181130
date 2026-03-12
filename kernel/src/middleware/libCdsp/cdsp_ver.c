/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C. 
 *                                             *
 *	v02, 20111223 gaoxu
 *		solve the issue of snapeffect invalidation  
 *	v03, 20120521 linjie.cheng
 *		a. Still CDSP Edge Enhan. Strong{s,m,w}, Middle{s,m,w}, Weak{s,m,w}
 *	v04, 20120529 linjie.cheng
 *		a. fixed still dynamic edge 9 level not load resource file issue
 *	v05, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
 *************************************************************************/
 
/**
 * @file      Cdsp_ver.c
 * @brief     
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/


/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$cd05";

/**************************************************************************
 *
 *  Function Name: cdspLibVerGet
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
libCdspVerGet(
	void
)
{
	return (verStr + 3);
}

