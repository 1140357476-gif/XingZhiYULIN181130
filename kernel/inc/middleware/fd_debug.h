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
 * @file      fd_debug.h
 * @brief     Brief
 * @author    LinJieCheng
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __FD_DEBUG_H__
#define __FD_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "dbg_file_list.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FD_DBG_CMD					0
#define FD_SAVE_FACE_YUV_SRC_DBG	0
#define FD_SAVE_FACE_II_DBG			0
#define FD_SAVE_SMILE_YUV_SRC_DBG	0
#define FD_SAVE_SMILE_II_DBG		0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* fd system information */
//#define fdSysInfo printf
#ifndef fdSysInfo
#define fdSysInfo	/_printf
#endif

/* hal fd system information */
//#define halfdSysInfo printf
#ifndef halfdSysInfo
#define halfdSysInfo	/_printf
#endif


/* fd get time information */
//#define fdGetGTimer HAL_GlobalTimerRead10us
#ifndef fdGetGTimer
#define _fdGetGTimer	/HAL_GlobalTimerRead10us
#define fdGetGTimer		/_fdGetGTimer
#endif


/* fdGroup function dbg */
//#define fdGroupPrintf printf
#ifndef fdGroupPrintf
#define fdGroupPrintf	/_printf
#endif

/* fdProcFace function dbg */
//#define fdProcFacePrintf printf
#ifndef fdProcFacePrintf
#define fdProcFacePrintf	/_printf
#endif

/* fdSvcProc function dbg */
//#define fdSvcProcPrintf printf
#ifndef fdSvcProcPrintf
#define fdSvcProcPrintf	/_printf
#endif


/* fdProcBlink function dbg */
//#define fdProcBlinkPrintf printf
#ifndef fdProcBlinkPrintf
#define fdProcBlinkPrintf	/_printf
#endif

/* fdGroupSub function dbg */
//#define fdGroupSubPrintf printf
#ifndef fdGroupSubPrintf
#define fdGroupSubPrintf	/_printf
#endif

/* fdProcSmileQueue function dbg */
//#define fdProcSmileQueuePrintf printf
#ifndef fdProcSmileQueuePrintf
#define fdProcSmileQueuePrintf	/_printf
#endif

/* fd save face yuv src data */
#define _fdSaveFaceYuvSrcDbg	/fdSaveFaceYuvSrcDbg
#if (FD_SAVE_FACE_YUV_SRC_DBG == 1)
#define fdSaveFaceYuvSrc fdSaveFaceYuvSrcDbg
#else
#ifndef fdSaveFaceYuvSrc
#define fdSaveFaceYuvSrc	/_fdSaveFaceYuvSrcDbg
#endif
#endif

/* fd save face ii data */
#define _fdSaveFaceIIDbg	/fdSaveFaceIIDbg
#if (FD_SAVE_FACE_II_DBG == 1)
#define fdSaveFaceII fdSaveFaceIIDbg
#else
#ifndef fdSaveFaceII
#define fdSaveFaceII	/_fdSaveFaceIIDbg
#endif
#endif

/* fd save smile yuv src data */
#define _fdSaveSmileYuvSrcDbg	/fdSaveSmileYuvSrcDbg
#if (FD_SAVE_SMILE_YUV_SRC_DBG == 1)
#define fdSaveSmileYuvSrc fdSaveSmileYuvSrcDbg
#else
#ifndef fdSaveSmileYuvSrc
#define fdSaveSmileYuvSrc	/_fdSaveSmileYuvSrcDbg
#endif
#endif

/* fd save smile ii data */
#define _fdSaveSmileIIDbg	/fdSaveSmileIIDbg
#if (FD_SAVE_SMILE_II_DBG == 1)
#define fdSaveSmileII	fdSaveSmileIIDbg
#else
#ifndef fdSaveSmileII
#define fdSaveSmileII	/_fdSaveSmileIIDbg
#endif
#endif


/**************************************************************************
 *                          FD command test Ctrl                          *
 **************************************************************************/
/* fdTest function dbg */
//#define fdTestPrintf printf
#ifndef fdTestPrintf
#define fdTestPrintf	/_printf
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

#endif  /* __FD_DEBUG_H__ */
