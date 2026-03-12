/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 *  1000, 20080813 Matt Wang
 *    a. add disp_libver.c.
 *  1001, 20080818 Matt Wang
 *    a. disable disp_glob_en must be after power off callback function.
 *  1002, 20080826 Matt Wang
 *    a. add dbg option in disp_logi.c.
 *  1003, 20080826 Matt Wang
 *    a. remove unused files.
 *  1004, 20080904 Matt Wang
 *    a. [0031406] modify img/osd layer control flow when power on/off.
 *    b. [0031406] use function instead of macro for sp1kHalCtrlScreen().
 *  1005, 20080910 Matt Wang
 *    a. [0031687] distinguish pv or pb zfupdate in sp1kHalCtrlScreen().
 *  1006, 20080917 Matt Wang
 *    a. turn off all debug option in disp_logi.c.
 *    b. active set must before powe on in sp1kDispPnlOpen().
 *    c. add macro DISP_DEV_ID_CHECK().
 *  1007, 20081003 Matt Wang
 *    a. remove unused dispImgWinSizeGet().
 *    b. add sp1kDispFitMethod_e and sp1kDispFitMethodSet().
 *  1008, 20081013 Matt Wang
 *    a. integrate new front driver architecture.
 *  1009, 20081027 Matt Wang
 *    a. enable/disable both DACPd & DACTest in dispEn().
 *  1010, 20081031 Matt Wang
 *    a. correct bit fields which are wrong in v1009.
 *  1011, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1012, 20081209 Matt Wang
 *    a. add sp1kDispGammaEnableSet() and sp1kDispGammaTableSet().
 *  1013, 20090220 Matt Wang
 *    a. add dispOsdPaletteSet() and sp1kDispOsdPaletteSet() by using macro.
 *    b remove unused null macro sp1kDispDeviceChg().
 *  1014, 20090727 Phil Lin
 *    a. compiling warning clean up.
 *  1015, 20090915 XianXin
 *    a. add dispPnlAspectRatioGet().
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$dp01";

/**************************************************************************
 *
 *  Function Name: dispLibVerGet
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
libDispVerGet(
	void
)
{
	return (verStr + 3);
}

