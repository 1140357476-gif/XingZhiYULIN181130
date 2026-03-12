/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      func_cmd_def.c
 * @brief     function command definition for internal use
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#define FUNC_EXT_INTERNAL
#include "func_ext_def.h"

#include "cmd.h"
#include "cmd_ext.h"
#include "cmd_test.h"

#include "dbg_mem.h"
#include "knl_func_opt.h"

#if FUNC_KNL_CMD_EXT
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

FUNC_EXT_DCL cmd_t code extCmdArray[] = {

#if DBG_MEM_RPT	
	{"memrpt", dbgMemRpt},
#endif

	{"msgpost", cmdMsgPost},

#if (DOS_CMD_OPTION == 1)
	{"dir", dosDirCmd},
	{"cd", dosCdCmd},
	{"write", dosWriteCmd},
	{"read", dosReadCmd},
	{"del", dosDelCmd},
	{"fmt", dosFmtCmd},
	{"info", dosInfoCmd},
#endif

#if (DOS_USB_OPTION == 1)
	{"usb", usbCmd},
#endif	

	NULL,
};
#endif /* FUNC_KNL_CMD_EXT */
