/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus mMedia *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno-*
 *  logy Co., Ltd. This software may only be used in accordance with the  *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.         *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd.                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#include "general.h"
#include "ver_svn.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
	The svn version logged in "ver_svn.h"
	Refers to the macro _svn_ver_
*/
#ifndef __VER_SVN_H__
#error Can NOT open file 'ver_svn.h'
#error Please execute the batch file ".\script\ver_gen.bat"
#endif
/**************************************************************************
 *                  E X T E R N A L    R E F E R E N C E                  *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT8 code verStr[] = "i@V2.2.0";

static UINT8 code verDate[] = __DATE2__;
static UINT8 code verTime[] = __TIME__;


/*
	version string for customer:
	a.User can get the string in image exif header
	b.The string lenth can not be large than 11
	c.If the first char in "verCustom" is 'X', the tag will be hidden.
*/

UINT8 code verCustom[] =
{
	'V', '0', '0', '0', '-', '0', '0', '-', '0', '1', 0x00
};

/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kFWVerGet                                           *
 *                                                                        *
 *  Purposes:                                                             *
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
UINT8*
sp1kFWVerGet(
	void
)
{
	return (verStr + 2);
}



UINT32
sp1kSvnVerGet(
	void
)
{
	return _svn_ver_;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kFWDateGet                                           *
 *                                                                        *
 *  Purposes:                                                             *
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
UINT8*
sp1kFWDateGet(
	void
)
{
	return verDate;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kFWTimeGet                                           *
 *                                                                        *
 *  Purposes:                                                             *
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
UINT8*
sp1kFWTimeGet(
	void
)
{
	return verTime;
}

