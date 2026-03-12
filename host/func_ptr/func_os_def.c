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
 * @file      func_os_def.c
 * @brief     OS function definition for Host
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#define SYS_OS_INTERNAL // for libOs internal use
#include "func_os_def.h"
#include "os_api.h"
#include "func_ptr.h"
#include "app_init.h"
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

/* 
  Note: The array size can't be lager than FUNC_SYS_INIT_MAX
*/
SYS_OS_DCL pFuncOS code funcOsArray[] = {
	(pFuncOS)osTaskIdle,
	(pFuncOS)pFunc_AudioRec,
	(pFuncOS)pFunc_AudioPb,

	(pFuncOS)pFunc_CardDetect,
	(pFuncOS)pFunc_USBDetect,
	(pFuncOS)pFunc_TVDetect,
	(pFuncOS)pFunc_CheckCountforTestUnitReady,

	(pFuncOS)pFunc_VideoRec,
	(pFuncOS)pFunc_VideoPb,
	(pFuncOS)pFunc_SoundCtrl,
#if LIFE_SPAN_TEST
	(pFuncOS)pFunc_TestDscLife_span,
#endif

	NULL,
};
