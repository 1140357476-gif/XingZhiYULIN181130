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
 * @file      sidc_dbg.h
 * @brief     Brief
 * @author    wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SIDC_DBG_H__
#define __SIDC_DBG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "usbOpt.h"
#if USBSIDC_OPTION
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//#define sidcPrintf printf	//sidc normal printf
//#define sidcPrintfData printf//printf sidc bulk data
//#define sidcPrintfCmd printf //printf sidc command
//#define sidcPrintfFunc printf //printf function name
//#define sidcPrintfErr printf //printf function name
//#define sidcPrintfPima printf //printf pima function name
//#define sidcPrintfInfo printf //printf pima dbg info
//#define sidcPrintfBulk printf //printf bulkIn/Out dbg info
//#define sidcPrintfImport printf //printf pima function name
#if USBSIDC_OPTION ==2
//#define dpsPrintf printf //dps normal printf
//#define dpsPrintfAction printf //printf dps action function name
//#define dpsPrintfInfo printf //printf dps info
//#define dpsPrintfXml printf //printf dps info
//#define dpsPrintfFunc printf //printf dps function name


//#define xmlPrintfInfo printf

#define DPS_CMD_ENABLE 0
#endif
#endif

#ifndef sidcPrintf
#define sidcPrintf /_printf
#endif

#ifndef sidcPrintfData
#define sidcPrintfData /_printf
#endif

#ifndef sidcPrintfCmd
#define sidcPrintfCmd /_printf
#endif

#ifndef sidcPrintfFunc
#define sidcPrintfFunc /_printf
#endif

#ifndef sidcPrintfErr
#define sidcPrintfErr /_printf
#endif

#ifndef sidcPrintfPima
#define sidcPrintfPima /_printf
#endif

#ifndef sidcPrintfInfo
#define sidcPrintfInfo /_printf
#endif

#ifndef sidcPrintfBulk
#define sidcPrintfBulk /_printf
#endif

#ifndef sidcPrintfImport
#define sidcPrintfImport /_printf
#endif

#ifndef dpsPrintf
#define dpsPrintf /_printf
#endif

#ifndef dpsPrintfAction
#define dpsPrintfAction /_printf
#endif

#ifndef dpsPrintfInfo
#define dpsPrintfInfo /_printf
#endif

#ifndef dpsPrintfXml
#define dpsPrintfXml /_printf
#endif

#ifndef dpsPrintfFunc
#define dpsPrintfFunc /_printf
#endif

#ifndef xmlPrintfInfo
#define xmlPrintfInfo /_printf
#endif

#ifndef DPS_CMD_ENABLE
#define DPS_CMD_ENABLE 0
#endif

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

#endif  /* __TEMPLATE_HDR_H__ */
