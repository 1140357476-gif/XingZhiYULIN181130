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
 *  Author: Phil.lin
 *
 *  Change Log:
 *
 *  1001, 20080812 Phil.lin
 *    a. initio version
 *  1002, 20080819 wenguo
 *    a. change for gprm scaling for photoframe
 *  1003, 20080822 Matt Wang
 *    a. add SP1K_GPIO_BYTE_P1 in sp1kGpioByte_e.
 *    b. add GPIO_BYTE_P1 in gpioByte_e in sp1kGpioByteFuncSet(),
 *       sp1kGpioByteDirSet() and sp1kGpioByteOutSet().
 *  1005, 20080826 Huiling
 *    a. Support Preview buffer rotation - PartII
 *  1006, 20081006 Huiling
 *    a. Fixed disable photo frame bug interrupt in sp1kPvPhotofrmEnable()
 *  1007, 20081008 Huiling
 *    a. add pvPhofrmEnableGet()
 *  1008, 20081014 wenguo
 *    a. add utilRegDump()
 *  1009, 20081017 Matt Wang
 *    a. use GPIO_USE_MACRO=1 in gpio_api.h.
 *  1010, 20081024 Matt Wang
 *    a. add SP1K_GPIO_BYTE_DISP0, SP1K_GPIO_BYTE_DISP1 and SP1K_GPIO_BYTE_DISP2
 *       in sp1kGpioByte_e for sp1kGpioByteXxxSet().
 *  1011, 20081028 Eric
 *    a. Modify for SuspendResume
 *  1012, 20081103 Matt Wang
 *    a. add sp1kPullEnSet().
 *  1013, 20081103 Matt Wang
 *    a. add sp1kPullEnGet().
 *  1014, 20081103 Xianxin
 *    a. for date stamp
 *  1015, 20081105 Xianxin
 *    a. move date stamp code(non-intr) to video module
 *    b. specify stamp position of display
 *  1016, 20081110 Matt Wang
 *    a. add sp1kGpioByteInGet().
 *  1017, 20081111 Huiling
 *    a. remove datstamp from interrupt to main loop
 *  1018, 20081111 Huiling
 *    a. date stamp location fine tuning
 *  1019, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1020, 20081208 Huiling
 *    a. Fix bad video date stamp frames
 *  1021, 20081209 Huiling
 *    a. Fix sp1kPvPhotofrmEnable prototype compiling error
 *  1022, 20081217 Xian Xin
 *    a. move photo frame blending from intr to main loop
 *  1023, 20081223 Xian Xin
 *    a. enable show second in stamp
 *  1024, 20081224 Xian Xin
 *    a. set zero paste threshold of stamp
 *  1025, 20081229 Allen Lin
 *    a. Remove Hal_DramShadowToImem();
 *  1026, 20081229 Allen Lin
 *    a. Resolve snap hang up after suspend/resume
 *  1027, 20090105 wenguo
 *    a. [mantis 34962] fix datestamp incorrect problem
 *  1028, 20090211 wenguo
 *    a. add ASSERT to protect date stamp paste
 *  1029, 20090217 xianxin
 *    a. add custom string mode
 *  1030, 20090522 xianxin
 *    a. add sp1kDateStampParamSet() and sp1kDateStampParamGet()
 *  1031, 20090717 xianxin
 *    a. blend date stamp in interrupt and control cdsp idx manually
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ut01";

/**************************************************************************
 *
 *  Function Name: utilLibVerGet
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
libUtilVerGet(
	void
)
{
	return (verStr + 3);
}

