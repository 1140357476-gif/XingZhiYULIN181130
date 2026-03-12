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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __ZOOM_DEBUG_H__
#define __ZOOM_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "dbg_file_list.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ZOOM_DEV_OPEN_DBG						0
#define ZOOM_DEV_ACT_SET_DBG					0
#define ZOOM_DEV_ACT_GET_DBG					0
#define ZOOM_DEV_MODE_CHECK_DBG					0
#define ZOOM_DEV_RES_LOAD_DBG					0
#define ZOOM_DEV_ZOOM_SET_DBG					0
#define ZOOM_DEV_ZOOM_CTRL_PARA_SET_DBG			0
#define ZOOM_DEV_ZOOM_CTRL_PARA_GET_DBG			0
#define ZOOM_DEV_ZOOM_FACTOR_GET_DBG			0
#define ZOOM_DEV_ZOOM_SMOOTH_SET_DBG			0
#define ZOOM_DEV_ZOOM_SMOOTH_GET_DBG			0
#define ZOOM_DEV_ZOOM_SMOOTH_STATUS_GET_DBG		0
#define ZOOM_DEV_ZOOM_SMOOTH_3A_PRE_SET_DBG		0
#define ZOOM_DEV_ZOOM_SMOOTH_3A_POST_SET_DBG	0
#define ZOOM_DEV_ZOOM_SMOOTH_TELE_SET_DBG		0
#define ZOOM_DEV_ZOOM_SMOOTH_WIDE_SET_DBG		0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* zoomDevOpen function dbg */
#if (ZOOM_DEV_OPEN_DBG)
#define zoomDevOpenPrintf printf
#endif
#ifndef zoomDevOpenPrintf
#define zoomDevOpenPrintf	/_printf
#endif 

/* zoomDevActSet function dbg */
#if (ZOOM_DEV_ACT_SET_DBG)
#define zoomDevActSetPrintf printf
#endif
#ifndef zoomDevActSetPrintf
#define zoomDevActSetPrintf	/_printf
#endif

/* zoomDevActGet function dbg */
#if (ZOOM_DEV_ACT_GET_DBG)
#define zoomDevActGetPrintf printf
#endif
#ifndef zoomDevActGetPrintf
#define zoomDevActGetPrintf	/_printf
#endif

/* zoomDevModeCheck function dbg */
#if (ZOOM_DEV_MODE_CHECK_DBG)
#define zoomDevModeCheckPrintf printf
#endif
#ifndef zoomDevModeCheckPrintf
#define zoomDevModeCheckPrintf	/_printf
#endif

/* zoomDevResourceLoad function dbg */
#if (ZOOM_DEV_RES_LOAD_DBG)
#define zoomDevResourceLoadPrintf printf
#endif
#ifndef zoomDevResourceLoadPrintf
#define zoomDevResourceLoadPrintf	/_printf
#endif

/* zoomDevZoomSet function dbg */
#if (ZOOM_DEV_ZOOM_SET_DBG)
#define zoomDevZoomSetPrintf printf
#endif
#ifndef zoomDevZoomSetPrintf
#define zoomDevZoomSetPrintf	/_printf
#endif

/* zoomDevZoomCtrlParaSet function dbg */
#if (ZOOM_DEV_ZOOM_CTRL_PARA_SET_DBG)
#define zoomDevZoomCtrlParaSetPrintf printf
#endif
#ifndef zoomDevZoomCtrlParaSetPrintf
#define zoomDevZoomCtrlParaSetPrintf	/_printf
#endif

/* zoomDevZoomCtrlParaGet function dbg */
#if (ZOOM_DEV_ZOOM_CTRL_PARA_GET_DBG)
#define zoomDevZoomCtrlParaGetPrintf printf
#endif
#ifndef zoomDevZoomCtrlParaGetPrintf
#define zoomDevZoomCtrlParaGetPrintf	/_printf
#endif

/* zoomDevZoomFactorGet function dbg */
#if (ZOOM_DEV_ZOOM_FACTOR_GET_DBG)
#define zoomDevZoomFactorGetPrintf printf
#endif
#ifndef zoomDevZoomFactorGetPrintf
#define zoomDevZoomFactorGetPrintf	/_printf
#endif

/* zoomDevZoomSmoothSet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_SET_DBG)
#define zoomDevZoomSmoothSetPrintf printf
#endif
#ifndef zoomDevZoomSmoothSetPrintf
#define zoomDevZoomSmoothSetPrintf	/_printf
#endif

/* zoomDevZoomSmoothGet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_GET_DBG)
#define zoomDevZoomSmoothGetPrintf printf
#endif
#ifndef zoomDevZoomSmoothGetPrintf
#define zoomDevZoomSmoothGetPrintf	/_printf
#endif

/* zoomDevZoomSmoothStatusGet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_STATUS_GET_DBG)
#define zoomDevZoomSmoothStatusGetPrintf printf
#endif
#ifndef zoomDevZoomSmoothStatusGetPrintf
#define zoomDevZoomSmoothStatusGetPrintf	/_printf
#endif

/* zoomDevZoomSmooth3APreSet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_3A_PRE_SET_DBG)
#define zoomDevZoomSmooth3APreSetPrintf printf
#endif
#ifndef zoomDevZoomSmooth3APreSetPrintf
#define zoomDevZoomSmooth3APreSetPrintf	/_printf
#endif

/* zoomDevZoomSmooth3APostSet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_3A_POST_SET_DBG)
#define zoomDevZoomSmooth3APostSetPrintf printf
#endif
#ifndef zoomDevZoomSmooth3APostSetPrintf
#define zoomDevZoomSmooth3APostSetPrintf	/_printf
#endif

/* zoomDevZoomSmoothTeleSet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_TELE_SET_DBG)
#define zoomDevZoomSmoothTeleSetPrintf printf
#endif
#ifndef zoomDevZoomSmoothTeleSetPrintf
#define zoomDevZoomSmoothTeleSetPrintf	/_printf
#endif

/* zoomDevZoomSmoothWideSet function dbg */
#if (ZOOM_DEV_ZOOM_SMOOTH_WIDE_SET_DBG)
#define zoomDevZoomSmoothWideSetPrintf printf
#endif
#ifndef zoomDevZoomSmoothWideSetPrintf
#define zoomDevZoomSmoothWideSetPrintf	/_printf
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

#endif  /* __ZOOM_DEBUG_H__ */
