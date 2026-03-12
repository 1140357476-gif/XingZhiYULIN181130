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
 *	v01, 20111024 wangkai
 *		a. initialize version
 *	v02, 20111201 cxhe
 *		a. add vendor command for UI Tool,request=UITOOL_REQUEST,
 *	v03, 20111206 jg.wang
 *		a. correct UI Tool Command.
 *	v04, 20111207 jg.wang
 *		a. correct UI Tool Command.
 *	v05,20111209 dongdong.wang
 *		a.modify upload udf replace adf
 *	v06,20111222 cxhe
 *		a.modify VLC buffer addr for PCCAM,solve  8M Dram buffer insufficiency
 *	v07,20120222 linjie.cheng
 *		a.fixed non-blocking dzoom for still & video flow
 * 	v08,20120318 peng.tan
 *		a.add still image snap flow
 * 	v09,20120321 dongdong.wang
 *		a.modify for ui tool
 *	v10,20120515 peng.tan
 *		change audio record setting to (44100 16 1)used by uac.
 *	v11,20120605 linjie.cheng
 *		a. add signal af window focus flow
 *      v12,20120727 rf.wang
 *              a.add blocking and nonblocking zoom
 *  v12,20120824 rf.wang
 *      a.fix an error when preview 720P(Amcap), bugid:0044117
 *	v13,20120804 peng.tan
 *		a.fix cannot preview after pccam set frame rate
 *	v14,20120905 peng.tan
 *		a.skip first four frame In pccam mode(preview mjpeg)
 *	v15,20120913 peng.tan
 *		a.change pccam com_mode to vlc when snap still image(Preview yuyv)
  *	v16,20120918 peng.tan
 *		a.modify code when pccam use our uvc driver, #0043939.
  *	v17,20140704 xianxin
 *		a.add uvc extension unit command callback , #0050532.
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ug17";

/**************************************************************************
 *
 *  Function Name: usbGlobalLibVerGet
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
libUsbGlobalVerGet(
	void
)
{
	return (verStr + 3);
}

