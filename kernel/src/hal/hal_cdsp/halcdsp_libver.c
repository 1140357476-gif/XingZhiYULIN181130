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
 *  Author: Wen-Guo Gan
 *  1000, 20080813 Wen-Guo Gan
 *    a. Initial libhalimage version create
 *  1001, 20080819 Matt Wang
 *    a. use libHal.Opt and libHal.Uv2 instead of hal_image.Opt and hal_image.Uv2.
 *  1002, 20080821 Matt Wang
 *    a. [0028661] fix 320x240 video zoom right side noise.
 *  1003, 20080825 Matt Wang
 *    a. optimize hal_cdsp, reduce code size and increase performance.
 *  1004, 20080829 Wen-Guo
 *    a. change HAL_CdspEofWait() to wait CDSP EOF
 *  1005, 20080923 Phil Lin
 *    a. uart interrupt clean up
 *  1006, 20080923 Huiling
 *    a. YUV scaling overflow if request buffer width is 720
 *  1007, 20081008 wenguo
 *    a. scaling down with A/B buffer
 *  1008, 20081013 Matt Wang
 *    a. move hal_front.c, hal_i2c.c to hal_front.
 *  1009, 20081014 wenguo
 *    a. change for 10 bit raw and post WB
 *  1010, 20081014 allen
 *    a. Solve ICAT hang up issue
 *  1011, 20081022 Matt Wang
 *    a. rearrange lens compensation api to HAL_LensCompCfgEnable(),
 *       HAL_LensCompSizeSet() and HAL_LensCompCenterSet().
 *    b. add HAL_CdspBayerPatternSet().
 *  1012, 20081023 wenguo
 *    a. fix AB scaling down with two pass
 *  1013, 20081024 wenguo
 *    a. backup cdsp reg to jpeg header
 *  1014, 20081106 wenguo
 *    a. QV use PV edge parameter
 *  1015, 20081114 Matt Wang
 *    a. fix DIV_ADDR define value from 0F810H to 0FFA8H in USER_DIV.A51
 *       to match .lin files.
 *  1016, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1017, 20081124 wenguo
 *    a. YUV snap flow.
 *  1018, 20081127 cs
 *    a. YUV sensor zoom factor setting.
 *  1019, 20081209 wenguo
 *    a. add multi LC
 *  1020, 20081218 wenguo
 *    a. [mantis 34668] fix RGB H scale even/odd register
 *  1021, 20081218 Matt Wang
 *    a. [0034671] remove unused access gen gpio reg in HAL_StillWB().
 *  1022, 20081221 wenguo
 *    a. [mantis 34699] rollback v1020
 *  1023, 20090106 wenguo
 *    a. [mantis 33349] post BRC
 *  1024, 20090204 Phil Lin
 *    a. [mantis 35307] fix awb win data error
 *  1025, 20090407 wenguo
 *    a. [mantis 36447] fix ae/awb window offset in zoom
 *  1026, 20090408 wenguo
 *    a. [mantis 36621] fix awb window offset in zoom
 *  1027, 20090413 wenguo
 *    a. change bad pixel on/off control
 *  1028, 20090605 wenguo
 *    a. fix YUV sensor zoom in/out snap issue.
 *  1029, 20090702 wangdongdong
 *    a. fix YUV sensor zoom in/out issue.
 *  1030, 20090915 XianXin
 *    a. add clampWidth and clampHeight to HAL_Cdsp1stPathZFactorSet() for HD.
 *  hi02, 20120619 linjie.cheng
 *    a. fixed debug printf information
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$hi02";

/**************************************************************************
 *
 *  Function Name: HAL_CdspLibVerGet
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
libHalCdspVerGet(
	void
)
{
	return (verStr + 3);
}

