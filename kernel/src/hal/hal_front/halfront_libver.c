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
 *  1000, 20081013 Matt Wang
 *    a. separate hal_front from hal_image.
 *  1001, 20081016 Matt Wang
 *    a. add HAL_I2cRegRead().
 *  1002, 20081027 wenguo
 *    a. VD wait use 0x2A66 instead of 0x2AB8
 *  1003, 20081105 Matt Wang
 *    a. set VDFintNum and VDRintNum=1 in HAL_FrontIntrEnable().
 *  1004, 20081110 Matt Wang
 *    a. busy waiting must be before any reg writing in HAL_I2cCmdTableSend()
 *       and HAL_I2cRegRead().
 *  1005, 20081113 Matt Wang
 *    a. set master clk phase in HAL_FrontParaMClkCfgSet().
 *  1006, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1007, 20081126 Matt Wang
 *    a. modify HAL_I2cRegRead() wait busy flow to fix first read back data error.
 *  1008, 20081208 Matt Wang
 *    a. [0033193] fix getting clk division in HAL_FrontParaMClkCfgGet() and
 *       HAL_FrontParaPClkCfgGet().
 *    b. [0033193] add argument *pphase in HAL_FrontParaMClkCfgGet().
 *  1009, 20081209 Matt Wang
 *    a. [0034493] set master clk driving in HAL_FrontParaMClkCfgSet().
 *    b. [0034493] add argument *pdrv in HAL_FrontParaMClkCfgGet().
 *  1010, 20081229 Matt Wang
 *    a. [0034567] add i2c 16-bit read api HAL_I2cReg16Read().
 *  1011, 20090321 Phil Lin
 *    a. add new i2c function for interrupt routine.
 *  1012, 20090727 Phil Lin
 *    a. function call tree correction
 *    b. compiling warning clean up
 *  ht02, 20120223 linjie.cheng
 *    a. add high quality sensor zoom flow
 *  ht03, 20120508 linjie.cheng
 *    a. add i2c source div
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ht03";

/**************************************************************************
 *
 *  Function Name: halFrontLibVerGet
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
libHalFrontVerGet(
	void
)
{
	return (verStr + 3);
}

