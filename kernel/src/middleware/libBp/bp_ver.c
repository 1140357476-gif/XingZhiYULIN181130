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
 *  Author: Phil.lin
 *
 *  Change Log:
 *
 *  1001, 20080812 Phil lin
 *    a. initial version
 *  1002, 20080918 Phil lin
 *    a. remove image.c from the lib
 *  1003, 20080923 Phil lin
 *    a. move spe_distort.c from libMisc to libSpe
 *  1004, 20080928 Phil lin
 *    a. bad pixel compensation function for capture
 *  1005, 20080930 eric
 *    a. Modify for SPI compile error
 *  1006, 20081001 eric
 *    a. Modify still capture bad pixel compensation
 *  1007, 20081007 eric
 *    a. Modify preview bad pixel compensation
 *  1008, 20081110 Phil lin
 *    a. badpixel function integration
 *  1009, 20081112 Phil lin
 *    a. mask byte switch for bp function
 *  1010, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1011, 20081125 Phil lin
 *    a. Badpixel function 2nd integration.
 *  1012, 20081125 Phil lin
 *    a. Fixed resource file size error.
 *  1013, 20090415 yong sun
 *    a. perfect bad pixel compensation.
 *  1014, 20090902 tq zhang
 *    a. perfect bad pixel compensation.
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$bp02";

/**************************************************************************
 *
 *  Function Name: miscLibVerGet
 *
 *  Purposes:
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
libBpVerGet(
	void
)
{
	return (verStr + 3);
}

