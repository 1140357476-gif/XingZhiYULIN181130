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
#ifndef __PV_DEBUG_H__
#define __PV_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_file_list.h"
#include "dbg_str.h"
#include "dbg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PV_SMOOTH_ZOOM_DBG		0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* pv system information */
#define pvSysInfo printf
#ifndef pvSysInfo
#define pvSysInfo	/_printf
#endif

/* pv system function track */
#define pvSysTrack	dbgFuncTrack(__FILE_ID_LIB_PB__, (unsigned short)__LINE__)
#ifndef pvSysTrack
#define _dbgFuncTrack	/dbgFuncTrack(__FILE_ID_LIB_PB__, (unsigned short)__LINE__)
#define pvSysTrack		/_dbgFuncTrack
#endif

/* pvSizeSet function dbg */
//#define pvSizeSetPrintf printf
#ifndef pvSizeSetPrintf
#define pvSizeSetPrintf	/_printf
#endif

/* pvSizeGet function dbg */
//#define pvSizeGetPrintf printf
#ifndef pvSizeGetPrintf
#define pvSizeGetPrintf	/_printf
#endif

/* pvClampSizeGet function dbg */
//#define pvClampSizeGetPrintf printf
#ifndef pvClampSizeGetPrintf
#define pvClampSizeGetPrintf	/_printf
#endif

/* pvZoomResLoad function dbg */
//#define pvZoomResLoadPrintf printf
#ifndef pvZoomResLoadPrintf
#define pvZoomResLoadPrintf	/_printf
#endif

/* pvZoomSet function dbg */
//#define pvZoomSetPrintf printf
#ifndef pvZoomSetPrintf
#define pvZoomSetPrintf	/_printf
#endif

/* pvFreeBuffGet function dbg */
//#define pvFreeBuffGetPrintf printf
#ifndef pvFreeBuffGetPrintf
#define pvFreeBuffGetPrintf	/_printf
#endif


/* sp1kFrontInit function dbg */
//#define sp1kFrontInitPrintf printf
#ifndef sp1kFrontInitPrintf
#define sp1kFrontInitPrintf	/_printf
#endif

/* sp1kPreviewSet function dbg */
//#define sp1kPreviewSetPrintf printf
#ifndef sp1kPreviewSetPrintf
#define sp1kPreviewSetPrintf	/_printf
#endif

/* sp1kCDSPInit function dbg */
//#define sp1kCDSPInitPrintf printf
#ifndef sp1kCDSPInitPrintf
#define sp1kCDSPInitPrintf	/_printf
#endif

/* sp1kPrevewUnfreeze function dbg */
//#define sp1kPrevewUnfreezePrintf printf
#ifndef sp1kPrevewUnfreezePrintf
#define sp1kPrevewUnfreezePrintf	/_printf
#endif

/* sp1kPreviewCropSizeSet function dbg */
//#define sp1kPreviewCropSizeSetPrintf printf
#ifndef sp1kPreviewCropSizeSetPrintf
#define sp1kPreviewCropSizeSetPrintf	/_printf
#endif

/* sp1kPreviewCropSizeGet function dbg */
//#define sp1kPreviewCropSizeGetPrintf printf
#ifndef sp1kPreviewCropSizeGetPrintf
#define sp1kPreviewCropSizeGetPrintf	/_printf
#endif

/* sp1k2ndPathCfg function dbg */
//#define sp1k2ndPathCfgPrintf printf
#ifndef sp1k2ndPathCfgPrintf
#define sp1k2ndPathCfgPrintf	/_printf
#endif

/* pv smooth zoom system information */
#define pvSmoothZoomSysInfo printf
#ifndef pvSmoothZoomSysInfo
#define pvSmoothZoomSysInfo	/_printf
#endif


/* pv smooth zoom system information */
#if (PV_SMOOTH_ZOOM_DBG == 1)
#define pvSmoothZoomDbg printf
#endif
#ifndef pvSmoothZoomDbg
#define pvSmoothZoomDbg	/_printf
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




#endif /*__PB_DEBUG_H__*/




