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
#ifndef __SNAP_DEBUG_H__
#define __SNAP_DEBUG_H__

//=============================================================================
//Header file
//=============================================================================
#include "dbg_def.h"

//=============================================================================
//Symbol
//=============================================================================

#define NORMAL_BREAK_DOWN				0
#define BURST_BREAK_DOWN				0
#define BURST_STORE_FIRST_BREAK_DOWN	0


#define _HAL_GlobalReadGTimer /HAL_GlobalReadGTimer(x)
#if (NORMAL_BREAK_DOWN == 0)
#define snapNormalBreakDown /_HAL_GlobalReadGTimer
#else
#define snapNormalBreakDown HAL_GlobalReadGTimer
#define snapPrintfNormalBreakDown printf 			/* printf Break Down all time cost message */
#endif

#if (BURST_BREAK_DOWN == 0)
#define snapBurstBreakDown /_HAL_GlobalReadGTimer
#else
#define snapBurstBreakDown HAL_GlobalReadGTimer
#define snapPrintfBurstBreakDown printf 			/* printf Break Down all time cost message */
#endif

#if (BURST_STORE_FIRST_BREAK_DOWN == 0)
#define snapBurstStoreFirstBreakDown /_HAL_GlobalReadGTimer
#else
#define snapBurstStoreFirstBreakDown HAL_GlobalReadGTimer
#define snapPrintfBurstStoreFirstBreakDown printf 			/* printf Break Down all time cost message */
#endif

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

#define snapPrintfLog printf 			/* printf important message */
//#define snapPrintf printf 			/* printf function name */
//#define snapPrintfMemBuf printf  	/* printf memory allocation */
//#define snapPrintfMemOneBuf printf  	/* printf one buffer memory allocation */
//#define snapPrintfMemTwoBuf printf  	/* printf two buffer memory allocation */
//#define snapPrintfSize printf  		/* printf snap size */
//#define snapPrintfDate printf  		/* printf date stamp size */
//#define snapPrintfTimeToPV printf  	/* printf the time of coming back to pv */
//#define snapPrintfJobTbl printf  		/* printf job table information */
//#define snapPrintfDoFront printf		/* printf do front */
//#define snapPrintfDoRaw printf			/* printf do front */
//#define snapPrintfJobTblDump printf			/* printf do front */
//#define snapPrintfDoYUV printf		/* printf do front */


#ifndef snapPrintfLog
#define snapPrintfLog /_printf
#endif

#ifndef snapPrintf
#define snapPrintf /_printf
#endif

#ifndef snapPrintfNormalBreakDown
#define snapPrintfNormalBreakDown /_printf
#endif

#ifndef snapPrintfBurstBreakDown
#define snapPrintfBurstBreakDown /_printf
#endif

#ifndef snapPrintfBurstStoreFirstBreakDown
#define snapPrintfBurstStoreFirstBreakDown /_printf
#endif

#ifndef snapPrintfMemBuf
#define snapPrintfMemBuf /_printf
#endif

#ifndef snapPrintfMemOneBuf
#define snapPrintfMemOneBuf /_printf
#endif

#ifndef snapPrintfMemTwoBuf
#define snapPrintfMemTwoBuf /_printf
#endif

#ifndef snapPrintfSize
#define snapPrintfSize /_printf
#endif

#ifndef snapPrintfDate
#define snapPrintfDate /_printf
#endif

#ifndef snapPrintfTimeToPV
#define snapPrintfTimeToPV /_printf
#endif

#ifndef snapPrintfJobTbl
#define snapPrintfJobTbl /_printf
#endif

#ifndef snapPrintfDoFront
#define snapPrintfDoFront /_printf
#endif

#ifndef snapPrintfDoYUV
#define snapPrintfDoYUV /_printf
#endif

#ifndef snapPrintfDoRaw
#define snapPrintfDoRaw /_printf
#endif

#ifndef snapPrintfJobTblDump
#define snapPrintfJobTblDump /_printf
#endif

#endif /*__PB_DEBUG_H__*/




