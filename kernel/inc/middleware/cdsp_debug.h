/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __CDSP_DEBUG_H__
#define __CDSP_DEBUG_H__

//=============================================================================
//Header file
//=============================================================================
#include "dbg_def.h"
//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define cdspPrintfLog printf 			/* printf important message */
//#define cdspPrintf printf 			/* printf function name */
//#define cdspPrintfCapSize printf 			/* printf capture size */
//#define cdspPrintfDynamic printf 			/* printf dynamic data */
//#define cdspPrintfLoadRes printf 			/* printf load resource */


#ifndef cdspPrintfLog
#define cdspPrintfLog /_printf
#endif

#ifndef cdspPrintf
#define cdspPrintf /_printf
#endif

#ifndef cdspPrintfCapSize
#define cdspPrintfCapSize /_printf
#endif

#ifndef cdspPrintfDynamic
#define cdspPrintfDynamic /_printf
#endif

#ifndef cdspPrintfLoadRes
#define cdspPrintfLoadRes /_printf
#endif


#endif /*__CDSP_DEBUG_H__*/




