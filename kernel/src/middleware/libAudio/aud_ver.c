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
 *  Author: Wen-Guo Gan
 *
 *  1005, 20080813 feng.lin
 *    a.Notice that the variable type of "code" had been linked to specfied bank
 *  1006, 20080819 xianxin
 *    a.Support multi folder as one(special for 9999 fileidx, update folder info after save)
 *  1007, 20080820 yong.sun
 *    a.Turn on AGC function
 *  1008, 20080820 cs
 *    a. add silence detection function
 *  1009, 20080922 yongsun
 *    a. Fix mantis bug 31938
 *  1010, 20080922 cslin
 *    a. add ALC adjust function
 *  1011, 20081111 yongsun
 *    a. Fixed mantis bug:32258
 *  1012, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1013, 20081208 xianxin
 * 	  a. update folder infor after create new media folder, fix #33748.
 * 	  b. donot care file index when voice memo.
 *  1014, 20081218 Matt Wang
 *    a. [0034671] use hal_gpio api instead of directly access reg in audioPbSet().
 *  1015, 20081222 yong sun
 *    a. support ADPCM format
 *  1016, 20090104 xianxin
 *    a. modify free space calculation
 *  1017, 20090106 Matt Wang
 *    a. [0034893] move audio reg and gpio setting to hal_av in audioPbSet(),
 *       audioRecInit() and sp1kAudioPlay().
 *  1018, 20090113 Phil Lin
 *    a. Modify for memory layout adjustment
 *  1019, 20090116 xianxin
 *    a. modify required by "$oA1043",
 *  1020, 20090213 yongsun
 *    a. fix audio play issue
 *  1021, 20090424 yongsun
 *    a. fix voice size read issue
 *  1022, 20090605 rongqi.yang
 *    a. modify audio_alc for improving audio rec
 *  1023, 20090803 yong.sun
 *    a. modify audio_alc for add voice memo
 *  1024, 20091010 xianixn
 *    a. add imageVoiRecImgNameSet() to fix #39169
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$au01";

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
libAudVerGet(
	void
)
{
	return (verStr + 3);
}

