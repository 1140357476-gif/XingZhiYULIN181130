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
 * @file      ae_debug.h
 * @brief     Brief
 * @author    LinJieCheng
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __AE_DEBUG_H__
#define __AE_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "dbg_file_list.h"
#include "intr.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              EXTERN COMMAND DEBUG OPTION                               *
 **************************************************************************/
#define AE_EXTERN_COMMAND_DEBUG_OPTION		0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/* ae system information */
//#define aeSysInfo printf
#ifndef aeSysInfo
#define aeSysInfo	/_printf
#endif

/* aeWinConfigSet function debug */
//#define aeWinConfigSetPrintf printf
#ifndef aeWinConfigSetPrintf
#define aeWinConfigSetPrintf	/_printf
#endif

/* aeReadyGet function debug */
//#define aeReadyGetPrintf printf
#ifndef aeReadyGetPrintf
#define aeReadyGetPrintf	/_printf
#endif

/* aeCreateIIQueue function debug */
//#define aeCreateIIQueuePrintf printf
#ifndef aeCreateIIQueuePrintf
#define aeCreateIIQueuePrintf	/_printf
#endif

/* aeCreateMoeQueue function debug */
//#define aeCreateMoeQueuePrintf printf
#ifndef aeCreateMoeQueuePrintf
#define aeCreateMoeQueuePrintf	/_printf
#endif

/* aeMoeFdWp function debug */
//#define aeMoeFdWpPrintf printf
#ifndef aeMoeFdWpPrintf
#define aeMoeFdWpPrintf	/_printf
#endif

/**************************************************************************
 *                             AE INTR PROC FUNCTION DEBUG                               *
 **************************************************************************/
/* aeIntrProc function debug */
#define AE_FLOW_DBG	0
#define II_AE_DBG	0
#define RGB_AE_DBG	0
#define AE_WIN_DBG	0
#define AE_GID_DBG	0
#define AE_MOE_DBG	0
#define AE_CAL_DBG	0
#define AE_MAX_DBG	0
#define AE_INF_DBG	0
#define AE_EGT_DBG	0

#if (AE_EGT_DBG)
#define EGTaeIntrPutChar		intrPutChar
#define EGTaeIntrPutUint8	intrPutUint8
#define EGTaeIntrPutUint16	intrPutUint16
#define EGTaeIntrPutUint32	intrPutUint32
#else
#ifndef EGTaeIntrPutChar
#define EGTaeIntrPutChar		/_intrPutChar
#endif
#ifndef EGTaeIntrPutUint8
#define EGTaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef EGTaeIntrPutUint16
#define EGTaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef EGTaeIntrPutUint32
#define EGTaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (II_AE_DBG)
#define IIaeIntrPutChar		intrPutChar
#define IIaeIntrPutUint8	intrPutUint8
#define IIaeIntrPutUint16	intrPutUint16
#define IIaeIntrPutUint32	intrPutUint32
#else
#ifndef IIaeIntrPutChar
#define IIaeIntrPutChar		/_intrPutChar
#endif
#ifndef IIaeIntrPutUint8
#define IIaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef IIaeIntrPutUint16
#define IIaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef IIaeIntrPutUint32
#define IIaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (AE_WIN_DBG)
#define AEWINaeIntrPutChar		intrPutChar
#define AEWINaeIntrPutUint8		intrPutUint8
#define AEWINaeIntrPutUint16	intrPutUint16
#define AEWINaeIntrPutUint32	intrPutUint32
#else
#ifndef AEWINaeIntrPutChar
#define AEWINaeIntrPutChar		/_intrPutChar
#endif
#ifndef AEWINaeIntrPutUint8
#define AEWINaeIntrPutUint8		/_intrPutUint8
#endif
#ifndef AEWINaeIntrPutUint16
#define AEWINaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef AEWINaeIntrPutUint32
#define AEWINaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (RGB_AE_DBG)
#define RGBaeIntrPutChar	intrPutChar
#define RGBaeIntrPutUint8	intrPutUint8
#define RGBaeIntrPutUint16	intrPutUint16
#define RGBaeIntrPutUint32	intrPutUint32
#else
#ifndef RGBaeIntrPutChar
#define RGBaeIntrPutChar	/_intrPutChar
#endif
#ifndef RGBaeIntrPutUint8
#define RGBaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef RGBaeIntrPutUint16
#define RGBaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef RGBaeIntrPutUint32
#define RGBaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (AE_GID_DBG)
#define GIDaeIntrPutChar	intrPutChar
#define GIDaeIntrPutUint8	intrPutUint8
#define GIDaeIntrPutUint16	intrPutUint16
#define GIDaeIntrPutUint32	intrPutUint32
#else
#ifndef GIDaeIntrPutChar
#define GIDaeIntrPutChar	/_intrPutChar
#endif
#ifndef GIDaeIntrPutUint8
#define GIDaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef GIDaeIntrPutUint16
#define GIDaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef GIDaeIntrPutUint32
#define GIDaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (AE_MOE_DBG)
#define MOEaeIntrPutChar	intrPutChar
#define MOEaeIntrPutUint8	intrPutUint8
#define MOEaeIntrPutUint16	intrPutUint16
#define MOEaeIntrPutUint32	intrPutUint32
#else
#ifndef MOEaeIntrPutChar
#define MOEaeIntrPutChar	/_intrPutChar
#endif
#ifndef MOEaeIntrPutUint8
#define MOEaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef MOEaeIntrPutUint16
#define MOEaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef MOEaeIntrPutUint32
#define MOEaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (AE_CAL_DBG)
#define CALaeIntrPutChar	intrPutChar
#define CALaeIntrPutUint8	intrPutUint8
#define CALaeIntrPutUint16	intrPutUint16
#define CALaeIntrPutUint32	intrPutUint32
#else
#ifndef CALaeIntrPutChar
#define CALaeIntrPutChar	/_intrPutChar
#endif
#ifndef CALaeIntrPutUint8
#define CALaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef CALaeIntrPutUint16
#define CALaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef CALaeIntrPutUint32
#define CALaeIntrPutUint32	/_intrPutUint32
#endif
#endif

#if (AE_MAX_DBG)
#define MAXaeIntrPutChar	intrPutChar
#define MAXaeIntrPutUint8	intrPutUint8
#define MAXaeIntrPutUint16	intrPutUint16
#define MAXaeIntrPutUint32	intrPutUint32
#else
#ifndef MAXaeIntrPutChar
#define MAXaeIntrPutChar	/_intrPutChar
#endif
#ifndef MAXaeIntrPutUint8
#define MAXaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef MAXaeIntrPutUint16
#define MAXaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef MAXaeIntrPutUint32
#define MAXaeIntrPutUint32	/_intrPutUint32
#endif
#endif


#if (AE_INF_DBG)
#define INFaeIntrPutChar	intrPutChar
#define INFaeIntrPutUint8	intrPutUint8
#define INFaeIntrPutUint16	intrPutUint16
#define INFaeIntrPutUint32	intrPutUint32
#else
#ifndef INFaeIntrPutChar
#define INFaeIntrPutChar	/_intrPutChar
#endif
#ifndef INFaeIntrPutUint8
#define INFaeIntrPutUint8	/_intrPutUint8
#endif
#ifndef INFaeIntrPutUint16
#define INFaeIntrPutUint16	/_intrPutUint16
#endif
#ifndef INFaeIntrPutUint32
#define INFaeIntrPutUint32	/_intrPutUint32
#endif
#endif


#if(AE_FLOW_DBG)
#define aeIntrPutChar		intrPutChar
#define aeIntrGetChar		intrGetChar
#define aeIntrPutUint8		intrPutUint8	
#define aeIntrPutUint16		intrPutUint16
#define aeIntrPutUint32		intrPutUint32
#else
#ifndef aeIntrPutChar
#define aeIntrPutChar		/_intrPutChar
#endif
#ifndef aeIntrGetChar
#define aeIntrGetChar		/_intrGetChar
#endif
#ifndef aeIntrPutUint8
#define aeIntrPutUint8		/_intrPutUint8	
#endif
#ifndef aeIntrPutUint16
#define aeIntrPutUint16		/_intrPutUint16
#endif
#ifndef aeIntrPutUint32
#define aeIntrPutUint32		/_intrPutUint32
#endif
#endif

/**************************************************************************
 *                           T I M I N G - D E B U G                            *
 **************************************************************************/
/**************************************/
/*   0 : front intr status,	1 : vd rising status	    */
/*   2 : vvd faling status,	3 : vvd rising status   */
/*   status => 0->1->0->3->0->2			    */
/**************************************/
#define AEC_AGC_STATUS_DBG				0	/* used GPIO12, GPIO13 */
#define AEC_AGC_TIMING_DBG				0	/* used GPIO12, GPIO13 */
#define AE_COST_TIMING_DBG				0	/* used GPIO12,(AE) */
#define INTR_COST_TIME_DBG				0	/* used GPIO12, GPIO13 */

#define _gpioByteOutSet(byteId,msk,val)		/gpioByteOutSet(byteId,msk,val)
#define _gpioByteDirSet(byteId,msk,dir)		/gpioByteDirSet(byteId,msk,dir)

#if (AEC_AGC_STATUS_DBG)
#define aeStatusGpioByteOutSet		gpioByteOutSet
#define aeStatusGpioByteDirSet		gpioByteDirSet
#else
#define aeStatusGpioByteOutSet		/_gpioByteOutSet
#define aeStatusGpioByteDirSet		/_gpioByteDirSet
#endif

#if (AEC_AGC_TIMING_DBG)
#define aeTimingGpioByteOutSet		gpioByteOutSet
#define aeTimingGpioByteDirSet		gpioByteDirSet
#else
#define aeTimingGpioByteOutSet		/_gpioByteOutSet
#define aeTimingGpioByteDirSet		/_gpioByteDirSet
#endif

#if (AE_COST_TIMING_DBG)
#define aeCostGpioByteOutSet		gpioByteOutSet
#define aeCostGpioByteDirSet		gpioByteDirSet
#else
#define aeCostGpioByteOutSet		/_gpioByteOutSet
#define aeCostGpioByteDirSet		/_gpioByteDirSet
#endif

#if (INTR_COST_TIME_DBG)
#define intrCostTimeSet(mask,x)			XBYTE[0x2031] = (XBYTE[0x2031] & ~mask) | (x & mask);
#else
#define intrCostTimeSet(mask,x)
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

#endif  /* __AE_DEBUG_H__ */
