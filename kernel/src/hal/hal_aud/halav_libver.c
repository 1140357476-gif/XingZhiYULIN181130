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
 *  1000, 20080819 Matt Wang
 *    a. separate hal_av from hal_global.
 *  1001, 20080917 wenguo
 *    a. modify HAL_JpegMjpgAbort()
 *  1002, 20081013 Matt Wang
 *    a. remove unused i2c code.
 *  1003, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1004, 20081208 wenguo
 *    a. remove printf.
 *  1005, 20081212 Matt Wang
 *    a. [0034565] fix jpeg scale factor calculation for hw limitation in
 *       HAL_JpgZFactorSet().
 *  1006, 20081218 Matt Wang
 *    a. [0034671] remove unused HAL_AudReset() which access gen gpio reg.
 *    b. [0034671] use hal_gpio api instead of directly access reg in
 *       HAL_AudStart() and HAL_SoundPlayTrg().
 *  1007, 20081222 yong sun
 *    a. support ADPCM format
 *  1008, 20090106 Matt Wang
 *    a. [0034893] modify HAL_AudStart(), HAL_SoundPlayCfg() and HAL_SoundPlayTrg()
 *       to fix pressing button bo-sound.
 *  1009, 2009010 Matt Wang
 *    a. [0034893] disable/enable audio pll in HAL_SoundPlayCfg() to fix audio noise.
 *  1010, 20090623 XianXin
 *    a. [0030807] add INTR_halJpegQTableWrite() for video brc
 *  1011, 20090727 Phil Lin
 *    a. compiling warning clean up
 *  1012, 20090915 XianXin
 *    a. add hOffset and vOffset to HAL_JpgZFactorCropSet() for HD
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ha01";

/**************************************************************************
 *
 *  Function Name: halAvLibVerGet
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
libHalAudVerGet(
	void
)
{
	return (verStr + 3);
}

