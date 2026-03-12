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

#ifndef __SP1K_ZOOM_API_H__
#define __SP1K_ZOOM_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "zoom_logi.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	SP1K_ZOOM_LOGI_STILL_MODE = ZOOM_LOGI_STILL_MODE,
	SP1K_ZOOM_LOGI_VIDEO_MODE = ZOOM_LOGI_VIDEO_MODE,
	SP1K_ZOOM_LOGI_MODE_MAX = ZOOM_LOGI_MODE_MAX,
}SP1K_ZOOM_LOGI_MODE;

typedef enum {
	SP1K_ZOOM_LOGI_BLOCKING_TYPE = ZOOM_LOGI_BLOCKING_TYPE,
	SP1K_ZOOM_LOGI_NONBLOCKING_TYPE = ZOOM_LOGI_NONBLOCKING_TYPE,
	SP1K_ZOOM_LOGI_TYPE_MAX = ZOOM_LOGI_TYPE_MAX,
}SP1K_ZOOM_LOGI_TYPE;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn        UINT8 sp1kZoomDevInit(void)
 * @brief     open pv and video zoom device (register blocking and nonblocking operating function)
 * @param    N/A
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kZoomDevInit(void);

/**
 * @fn        UINT8 sp1kZoomDevActSet(UINT8 mode,UINT8 type)
 * @brief     set current actived zoom type
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] type : typedef enum ZOOM_LOGI_TYPE
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kZoomDevActSet(UINT8 mode,UINT8 type);

/**
 * @fn        UINT8 sp1kZoomDevActGet(UINT8 mode,UINT8 *type)
 * @brief     get current actived zoom type
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *type : typedef enum ZOOM_LOGI_TYPE
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kZoomDevActGet(UINT8 mode,UINT8 *type);


#else

#define sp1kZoomDevInit					zoomDevInit
#define sp1kZoomDevActSet				zoomDevActSet
#define sp1kZoomDevActGet				zoomDevActGet

#endif


#endif  /* __SP1K_ZOOM_API_H__ */
