/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20120208 Phil.Lin
 *		a. fixed function return error
 *	v03, 20120615 Phil.Lin
 *		a. add SPI Flash software write protect function
 *	v04, 20120813 rf.wang
 *		a. fix load memu.bin overlay Qtable bug
 *	v05, 20120904 xianxin
 *		a. chang rsvAMap[] idx type from UINT8 to UINT32
 *	v06, 20130703 delei.gu
 *		a. add spi WP func for func codeWriteBack
 *
 **************************************************************************/
#include "general.h"
#include "hal_device.h"

#ifdef DISK_DRV_SPI
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                  E X T E R N A L    R E F E R E N C E                  *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 code verStr[] = "$si06";

/**************************************************************************
 *                                                                        *
 *  Function Name: diskLibVerGet                                          *
 *                                                                        *
 *  Purposes: Get current libaray's version.                              *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 *
libDiskVerGet(
	void
)
{
	return (verStr + 3);
}
#endif //DISK_DRV_SPI

