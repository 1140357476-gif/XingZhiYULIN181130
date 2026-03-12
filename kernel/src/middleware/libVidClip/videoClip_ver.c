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
 *  Author: rf.wang
 *
 *  v01-20101020 wangjg
 *     a. video version initialize
 *     b. bg flow 
 *  v02-20101021 wangjg
 *     a. correct rec write data flow for bad frame;
 *  v03-20101031 wangjg
 *     a. correct video bad frame problem.
 *     b. Clean bg *** WARNING L15: MULTIPLE CALL TO FUNCTION.
 *     c. correct video rec audio cnt calculate error.
 *     d. correct moviOffset calculate error.
 *     e. Clean Video invalid golbal.
 *  v04
 *  v05-20101101 wangjg
 *     a. correct video first audio frame error.
 *     b. correct wb/dc net balance.
 *  v06-20101101 wangjg
 *     a. correct video bg file size calculate error.
 *  v07-20101101 wangjg
 *     a. correct video Frag buf size error.
 *  v08-20101103 wangjg
 *     a. add data flow ratio debug info.
 *  v09-20101103 wangjg
 *     a. disable vlc intr for some variadle.
 *  v0a-20101103 wangjg
 *     a. add video cmd for switch bg;
 *  v0b-20101107 LinJieCheng
 *     a. add HQ preview (sensor zoom Architecture)function
 *  v0c-20101110 LinJieCheng
 *     a. add Dzoom non-blocking flow
 *     b. removed old Dzoom non-blocking flow
 *  v0d-20101112 LinJieCheng
 *     a. add 1080P FHD AE FLOW (8 frame)
 *  v0e-20101115 wangjg
 *     a. add bg mode 2
 *  v0f-20101116 wangjg
 *     a. add bg mode 3
 *  v10-20101117 wangjg
 *     a. add bg mode 4~5
 *	 b. clean G_FPGAMemPool
 *  v11-20101124 wangjg
 *     a. add bg mode 6
 *  v12-20111125 rf.wang
 *     a. add 1st path size and buf check function
 *  v13-20111207 jg.wang
 *     a. correct bg disk free space detect.
 *  v14-20111208 jg.wang
 *     a. correct pause/resume datastamp sync problem.
 *  v15-20111208 LinJieCheng
 *     a.fixed HQ Dzoom flow
 *  v16-20111209 rf.wang
 *     a.modify video rec pause flow   
 *  v17-20111213 jg.wang
 *     a.modify video rec pause flow   
 *  v18-20111214 jg.wang
 *     a.correct commit error for previous several version
 *  v19-20111215 jg.wang
 *     a.correct commit error for previous several version
 *  v19-20111220 jg.wang
 *     a.add condition(jpgEncodeSize==0) for Intr
 *  v20-20111221 rf.wang
 *     a.open date stamp workround to fix the block bad frame
 *  v21-20111221 jg.wang
 *     a.for rec, exchange order of video and audio process,to avoid audio overflow.
 *  v22-20111222 jg.wang
 *     a.correct post message sp1k_MSG_REC_SEAMLESS_DISK_FULL for seamless delete file. 
 *  v23-20111229 jg.wang
 *	a. correct seamless stop accidentally.
 *	b.G_VideoVlcBufNum in D1 is too small ,lead to crash in videoRecBrcAdjust()
 *	c. for pb,1st frame of out avi, have to be video;
 *	d. integrate and improve seamless rec rename function;
 *	e.detach video rec msg process:SP1K_MSG_MEDIA_REC_DISK_FULL and SP1K_MSG_MEDIA_REC_FILE_MAX
 *	f. correct plug out sd crash when video rec;
 *	g.correct autotest rec bad file; 
 *	h.correct bg rec disk full error.
 *  v24-20120105 jg.wang
 * 	a.correct sl rec free mem check;
 *  v25-20120106 jg.wang
 * 	clear G_Video_Bound_Size;
 *  v26-20120111 rf.wang
 *  	a.modify vlc VlcBound size to fix jpeg header error bug;
 *	b.make videoRecBrcAdjust();
 *  v27-20120111 rf.wang
 *  	a.correct video dma wait timeout detect;
 *  v28-20120119 jg.wang
 *  	a.adjust AVI structure for seamless speedup;
 *	b.use bgFileJoint for seamless speedup;
 *	c.correct bg enable condition;
 * 	d.clean video rec abort flow;
 *  v29-20120130 rf.wang
 *	a.fix jpg header Q value err when jpg overflow
 *  v2a-20120202 linjie.cheng
 *	a.fixed dist knl opt enable/disable videorec ini value define warning.
 *  v2b-20120206 jg.wang
 *	a.bg write data to cluster 0;
 *	b.correct stamp copy thr;
 *  v2c-20120207 rf.wang
 *	a.fix cdsp idx error bug;
 *	b.off brc when video rec pause;
 *  v2e-20120210 rf.wang
 *	a.fix bug #44348;
 *  v2f-20120211 jg.wang
 *	a. correct bg mode for QVGA, fixed 4G bad file bug;
 *  v30-20120216 rf.wang
 *	a. fix seamless loop rec audio and video are not sync bug
 *  v31-20120222 linjie.cheng
 *	a. fixed non-blocking dzoom for still & video flow
 *  vd32, 20120223 linjie.cheng
 *      a. add high quality sensor zoom flow
 *  vd33, 20120303 rf.wang
 *      a. fix data error bad frame
 *      b. add yuv buf mode and vlc buf num infor
 *  vd34, 20120303 rf.wang
 *      a. add pv size infor   
 *  vd35, 20120305 rf.wang
 *      a. remove pv size infor   
 *  vd36, 20120308 rf.wang
 *      a. fix setting 0x27b2[4] to be 0 when jpg overflow  bug 
 *  vd37, 20120308 rf.wang
 *      a. limit video rec time to 4 hour
 *  vd38, 20120320 linjie.cheng
 *      a. add rec stop need to stop ae intr smooth zoom flow
 *  vd39, 20120502 linjie.cheng
 *      a. add get and set zoom ctrl parameter api for host use. (video and still)
 *  vd40, 20120601 linjie.cheng
 *      a. fixed non-blocking intr zoom auto set max factor, min factor, zoom step, HQdzoom step
 *  vd41, 20120619 linjie.cheng
 *      a.fixed smooth zoom flow
 *  vd42, 20120702 rf.wang
 *      a.add video rec time check debug infor
 *  vd43, 20120716 linjie.cheng
 *      a.fixed one step zoom ae fail
 *  vd44, 20120718 rf.wang
 *      a.fix seamless loop rec osd not update bug
 *      b.add file idex printf when video rec init  fail
 *  vd45, 20120724 rf.wang
 *      a.fix fram size error when rec video HD and SD cluster size <8K 
 *  vd46, 20120727 rf.wang
 *      a.add blocking zoom and nonblocking zoom 
 *  vd47, 20120801 rf.wang
 *      a.open video debug info 
 *  vd48, 20130306 rf.wang
 *      a.get vlc size from reg(0x2838~0x283a),fix jpeg without jpeg eof flag bug
 *  vd49-20131030 xianxin
 *  a. fix bgWrite error when index list overflow and correct videoAbortWriteIndex()  
/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$vd49";

/**************************************************************************
 *
 *  Function Name: snapLibVerGet
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
libVideoClipVerGet(
	void
)
{
	return (verStr + 3);
}

