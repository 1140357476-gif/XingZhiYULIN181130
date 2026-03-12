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
 *                                                                        *
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02, 20127026 juanjuan.yuan
 *		a. modify USR_1FileBaseCpy ,correct  copy to card error
 *
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                  E X T E R N A L    R E F E R E N C E                  *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 code verStr[] = "$oC01";

/**************************************************************************
 *                                                                        *
 *  Function Name: snapLibVerGet                                          *
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
UINT8*
libDosCVerGet(
	void
)
{
	return (verStr + 3);
}

