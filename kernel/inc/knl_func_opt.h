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
 * @file      knl_func_opt.h
 * @brief     options for the function on kernel level
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __KNL_FUNC_OPT_H__
#define __KNL_FUNC_OPT_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "host_func_opt.h" 
#include "app_video_view.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
	The Macro "LIB_RELEASE" defined in compiler options.
*/ 

#ifndef LIB_RELEASE

/**************************************************************************/
/*
	Engneering Lib Definition
*/

/* Define function for the calibration */
#define FUNC_KNL_CAL    1
#if (FUNC_HOST_CAL) && (!FUNC_KNL_CAL)
#error "FUNC_KNL_CAL" should be enabled
#endif

/* Define function for the engneering command line */
#define FUNC_KNL_CMD_ENG    1

/**************************************************************************/

/**************************************************************************/
/*
	Extended Lib Definition
*/

#define FUNC_KNL_CMD_EXT    1

#define FUNC_KNL_SIDC       1

#define FUNC_KNL_DPS        1
#if (FUNC_HOST_DPS) && (!FUNC_KNL_DPS)
#error "FUNC_KNL_DPS" should be enabled
#endif

#define FUNC_KNL_AUD        1
#define FUNC_KNL_SNAP       1

#define FUNC_KNL_FD         1
#define FUNC_KNL_DIST       0

#define FUNC_KNL_VDPB       1
#define FUNC_KNL_STLPB      1
#define FUNC_KNL_SPE        1

//#define FUNC_KNL_DISP       1
//#define FUNC_KNL_HDMI       1
#define FUNC_KNL_OSD        1

#define FUNC_KNL_BP         1
#if (FUNC_HOST_BADPIXEL) && (!FUNC_KNL_BP)
#error "FUNC_KNL_BP" should be enabled
#endif

#define FUNC_KNL_DS         1
#define FUNC_KNL_FRM        1

#define FUNC_KNL_MD	        1
#define FUNC_KNL_PANORAMA   0
#define FUNC_KNL_LSCCAL     1
#define FUNC_KNL_MSCAL		0
#if (FUNC_HOST_MD) && (!FUNC_KNL_MD)
#error "FUNC_KNL_MD" should be enabled
#endif

#define FUNC_KNL_AF_FIXTURE		1
/*
#if (VIDEO_NEW_ZOOM) && (!STILL_NEW_ZOOM)		
#error "STILL_NEW_ZOOM" should be enabled
#endif
#if (STILL_NEW_ZOOM) && (!VIDEO_NEW_ZOOM)		
#error "VIDEO_NEW_ZOOM" should be enabled
#endif
*/
#else
/**************************************************************************/
/*
	Engneering Lib Definition
*/

/* Define function for the calibration */
#define FUNC_KNL_CAL    1

/* Define function for the engneering command line */
#define FUNC_KNL_CMD_ENG    1

/**************************************************************************/

/**************************************************************************/
/*
	Extended Lib Definition
*/

#define FUNC_KNL_CMD_EXT    1

#define FUNC_KNL_SIDC       1
#define FUNC_KNL_DPS        1
//#define FUNC_KNL_MSDC     1

#define FUNC_KNL_AUD        1
#define FUNC_KNL_SNAP       1

#define FUNC_KNL_FD         1
#define FUNC_KNL_DIST       0

#define FUNC_KNL_VDPB       1
#define FUNC_KNL_STLPB      1
#define FUNC_KNL_SPE        1

//#define FUNC_KNL_DISP       1
//#define FUNC_KNL_HDMI       1
#define FUNC_KNL_OSD        1

#define FUNC_KNL_BP         1
#define FUNC_KNL_DS         1
#define FUNC_KNL_FRM        1
#define FUNC_KNL_MD			1
#define FUNC_KNL_PANORAMA   0
#define FUNC_KNL_LSCCAL		1
#define FUNC_KNL_MSCAL		0

#define FUNC_KNL_AF_FIXTURE		1

#endif //LIB_RELEASE

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

#endif  /* __KNL_FUNC_OPT_H__ */

