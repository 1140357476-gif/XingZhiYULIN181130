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
 *
 *  1000, 20080428 Wen-Guo Gan
 *    a. Init
 *  1001, 20080813 Matt Wang
 *    a. add dispLibVerGet() and osdLibVerGet().
 *  1002, 20080814 CS
 *    a. add aaaLibVerGet().
 *  1003, 20080819 Matt Wang
 *    a. add halAvLibVerGet(), halDispLibVerGet(), halDramLibVerGet(),
 *       halFlashLibVerGet(), halGlobalLibVerGet() and halCdspLibVerGet().
 *  1004, 20080820 Phil lin
 *    a. add disk lib version get.
 *  1005, 20080825 Phil lin
 *    a. add test function for file attrib modify
 *  1006, 20080825 xianxin
 *    a. add test code for distortion, but mask by default
 *  1007, 20080901 xianxin
 *    a. modify for distortion
 *  1008, 20080901 xianxin
 *    a. fix bug when do wavy distortion
 *  1009, 20080916 eric
 *    a. remove ENTER_CRITICAL when using dos cmd
 *  1010, 20080917 wenguo
 *    a. add cdsp lib version get
 *  1011, 20080918 phil lin
 *    a. remove usused code
 *  1012, 20080918 phil lin
 *    a. uart interrupt clean up
 *  1013, 20080923 phil lin
 *    a. add temp data to link global variables to high 4K sram
 *  1014, 20080923 eric
 *    a. Add rtc test code with #if 0
 *  1015, 20080923 eric
 *    a. Add api sp1kHalAlarmRTCIntrSet.
 *  1016, 20080924 eric
 *    a. Modify api sp1kHalAlarmRTCIntrSet.
 *  1017, 20081006 eric
 *    a. Modify rtctest() 1 mim count and 1 min suspend.
 *  1018, 20081008 jg.wang
 *    a. modify headfile of distort.
 *  1019, 20081013 Matt Wang
 *    a. integrate new front driver architecture.
 *  1020, 20081106 Matt Wang
 *    a. add frontLibVerGet(), halFrontLibVerGet() and intrLibVerGet().
 *  1021, 20081111 Phil.lin
 *    a. badpixel function integration.
 *  1022, 20081111 Phil.lin
 *    a. move a head file from pb lib
 *  1023, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1024, 20081121 Phil Lin
 *    a. fixed sd card compatible issue.
 *  1025, 20081125 Phil Lin
 *    a. Badpixel function 2nd integration.
 *  1026, 20090120 Phil Lin
 *    a. add libDosC.lib and libUsbD.lib to version display command line
 *  1027, 20090122 wangjg
 *    a. add dps cmd
 *  1028, 20090321 Phil Lin
 *    a. add switch for front test function
 *  1029, 20090401 wangkai
 *    a.modify for UART function 
 *  1030, 20090508 wangkai
 *    a.fix 1029 check in 
 *  1031, 20090727 Phil Lin
 *    a.remove dummy variables for data alignment. 
 *  1032, 20090918 Phil Lin
 *    a.add cmd debug buffer size from 16 to 32 characters. 
 *    b.support '<' '>' to repeate the cmd history
 *    c.fixed the issue that mem dump length error (more 0x10 have be dumped)
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$cm01";

/**************************************************************************
 *
 *  Function Name: cmdLibVerGet
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
libCmdVerGet(
	void
)
{
	return (verStr + 3);
}

