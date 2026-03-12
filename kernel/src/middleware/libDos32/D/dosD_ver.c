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
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02,20111031 renhy
 *		a.assign folder for timelapse video file
 *	v03,20111118 dongdong.wang
 *		a. add dcf smart init
 *	v04,20111122 hy.ren
 *		a.add function for set avi name :dcfAviNameSet
 *		b.modify get fileName function: dcfMediaNameGet
 *	v05,20111123 dongdong.wang
 *		a.modify for strtoul'base
 *	v06,20111209 dongdong.wang
 *		a.clean waring
 *    v07,20111209 xianxin
 *		a.fix dcfFileDramFdbParameterRead() error(pb file order error), 
 *    v08,20111222 wangjg
 *		a. correct dcfDeleteFile for seamless rec.
 *    v09,20120105 xianxin
 *		a. add dcfFileDramFdbUpdate() to dcfFileAttrSet(),
 *    v0a,20120119 jg.wang
 *		a. correct dcfdel readonly
 *    v0b,20120221 rf.wang
 *		a. Abort bg write when set file attr 
 *    v0c,20120906 xianxin
 *		a. fdb.size error in dcfFileInitialize() 
 *    v0d,20121116 xianxin
 *		a. fix dcfFileReadOnlyAttrSet() error (FDB buffer info update)
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$od0d";

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
libDosDVerGet(
	void
)
{
	return (verStr + 3);
}

