/*************************************************************************
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
 *  Author: Allen Lin
 *
 *  1000, 20080812 Allen Lin
 *    a. Initial libusb_B.a version create
 *  1001, 20080813 Allen Lin
 *    a. Solve G_Card_TotalSizeMB not correct issue
 *  1002, 20080814 Allen Lin
 *    a. Change product string from 533 to 1528
 *  1003, 20080818 Allen Lin
 *    a. Add 0x20 in product string end
 *  1004, 20080818 Phil Lin
 *    a. modify for disk driver
 *  1005, 20080818 Phil Lin
 *    a. check MLUN type when PC read capacity from 1528
 *  1006, 20080915 Allen Lin
 *    a. Solve MSDC r/w 4G limitation
 *  1007, 20080915 Allen Lin
 *    a. Change G_USBMSDC_ulLogicalBlockAddress from byte address to block address
 *  1008, 20080917 Allen Lin
 *    a. optimze debug message
 *  1009, 20081103 eric
 *    a. Modify for Dos Ram
 *  1010, 20081118 Matt Wang
 *    a. shorten verStr[] to reduce code size usage.
 *  1011, 20081120 eric
 *   a. Add use register 0x22e7 as package option.
 *  1012, 20090105 Wangjg
 *   a. modify SIDC Cmd parser 
 *   b. delete pimafile.c and pima.c 
 *  1013, 20090108 Xian Xin
 *   a. check security for mWeb tool if MWEBTOOL_ENABLE = 1 
 *  1014, 20090119 Wangjg
 *    a. add pima.c sidc*.c.
 *    b. clean up code.
 *  1015, 20090119 Wangjg
 *    a. add pima.c sidc*.c.
 *  1016, 20090120 Phil Lin
 *    a. libUsb restructure.
 *  1017, 20090205 Phil Lin
 *    a. Do not to mount dos ram if the dos ram function is disabled
 *  1018, 20090206 Phil Lin
 *    a. modify variable to function for dos ram function
 *  1019, 20090407 eric
 *    a. Modify the RSV variable from UINT8 to UINT16
 *  1020, 20090409 Allen
 *    a. Solve Single Lun can't show nand disk issue. (#36633)
 *  1021, 20090513 zengping
 *    a. change usb description by user.
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$ua01";

/**************************************************************************
 *
 *  Function Name: usbALibVerGet
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
libUsbAVerGet(
	void
)
{
	return (verStr + 3);
}

