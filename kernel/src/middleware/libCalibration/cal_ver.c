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
 *  1001, 20080812 Wen-Guo Gan
 *    a. fix raw data calculation
 *  1002, 20080813 Wen-Guo Gan
 *    a. change AE luma calculation
 *  1003, 20080816 Wen-Guo Gan
 *    a. add PV YUV capture and OB calibration flow
 *  1004, 20080903 Wen-Guo Gan
 *    a. fix OB calibration gamma table
 *  1005, 20080911 Wen-Guo Gan
 *    a. add PV OB calibration mode
 *  1006, 20081002 Wen-Guo Gan
 *    a. replace HAL_FrontVvalidWait with HAL_FrontVdWait
 *  1007, 20081008 Huiling
 *    a. add save preview raw image relative functions
 *  1008, 20081009 Huiling
 *    a. sp1k API for preview raw image relative functions
 *  1009, 20081013 Matt Wang
 *    a. integrate new front driver architecture.
 *  1010, 20081016 Huiling
 *    a. Modify preview raw image relative functions
 *  1011, 20081022 Matt Wang
 *    a. use frontPreviewSizeGet() and frontSnapSizeGet() instead of old api.
 *  1012, 20081029 Matt Wang
 *    a. no need to call HAL_FrontIntrEnable().
 *  1013, 20081110 Phil lin
 *    a. badpixel function integration
 *  1014, 20081112 Phil lin
 *    a. fixed some bug for badpixel function
 *  1015, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1016, 20081125 Phil lin
 *    a. Badpixel function 2nd integration.
 *  1017, 20090727 Phil lin
 *    a. compiling warning clean up.
 *  1018, 2009.8.28 rongqi.yang
 *    a. add ae calibration for get ae value
 *  1019, 2009.9.17 Phil Lin
 *    a. modify the linking address of badpixel functions
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ca01";

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
libCalVerGet(
	void
)
{
	return (verStr + 3);
}

