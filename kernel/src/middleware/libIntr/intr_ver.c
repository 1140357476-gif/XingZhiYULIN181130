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
 *  Author: Matt Wang
 *	it01, 20111024 Guoqiang.li
 *		a. modify number of version
 *	it02, 20111110 LinJieCheng
 *		a. add Dzoom non-blocking flow
 *              b. removed old Dzoom non-blocking flow
 *	it03, 20111112 LinJieCheng
 *		a. add 1080P FHD AE FLOW (8 frame)
 *	it04, 20111112 LinJieCheng
 *		a. fixed 1080P FHD AE FLOW (8 frame)
 *	it05, 20111117 wangjg
 *		a. clean G_FPGAMemPool
 *	it06, 20111209 rf.wang
 *		a. modify 1<<2 to 0x04
 *	it07, 20120105 rf.wang
 *		a.add RTC intr.
 *	it08, 20120119 jg.wang
 *		a.move video intr proc to lib itself.
 *	it09, 20120207 rf.wang
 *		a.move clear jpg overflow intr to INTR_videoClipBrcVlcOvfSetQtable() function.
 *	it0a, 20120208 Phil.Lin
 *		a.add touch panel function.
 *	it0b, 20120222 linjie.cheng
 *		a.fixed non-blocking dzoom for still & video flow
 *      it0c, 20120223 linjie.cheng
 *              a. add high quality sensor zoom flow
 *      it0d, 20120303 rf.wang
 *              a. debug infor when jpg overflow
 *	it0c, 20120316 peng.tan
 *		a.add still image snap flow
 *	it0d, 20120413 dongdong.wang
 *		a.add speedy capture
 *	it0e, 20120511 linjiecheng
 *		a.add AF_FIXTURE host flow & af lib
 *	it0f, 20120619 linjiecheng
 *		a.fixed smooth zoom flow
 *	it10, 20120727 rf.wang
 *		a.add blcoking zoom and nonblocking zoom
 *	it11, 20120801 rf.wang
 *		a.fix msg queue over flow bug when zoom
 *	it12, 20120804 peng.tan
 *		a.fix cannot preview after pccam set frame rate
 *	it13, 20120905 peng.tan
 *		a.skip first four frame In pccam mode(preview mjpeg)
 *	it14, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
***************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$it14";

/**************************************************************************
 *
 *  Function Name: intrLibVerGet
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
libIntrVerGet(
	void
)
{
	return (verStr + 3);
}

