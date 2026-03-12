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

#ifndef __ZOOM_LOGI_H__
#define __ZOOM_LOGI_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define STILL_ZOOM_DEVICE_TOT	2	/* support zoom type number */
#define VIDEO_ZOOM_DEVICE_TOT	2	/* support zoom type number */
#define ZOOM_DEVICE_TOT			(STILL_ZOOM_DEVICE_TOT + VIDEO_ZOOM_DEVICE_TOT)

 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DEV_STILL_ZOOM_OFFSET	0
#define DEV_VIDEO_ZOOM_OFFSET	2
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
typedef struct {
	UINT8 zoomType;
} zoomStillCapabDesc_t;

typedef struct {
	UINT8 zoomType;
} zoomVideoCapabDesc_t;

/* operating function. */
typedef UINT8 (code *pfOperatingZoomResourceLoad_t)(UINT32 *pParaArray);//UINT32 para1,UINT32 para2,UINT32 para3);
typedef UINT8 (code *pfOperatingZoomSet_t)(UINT8 para1,UINT16 para2,UINT8 para3);
typedef void (code *pfOperatingZoomCtrlParaSet_t)(UINT8 item,UINT16 value);
typedef UINT16 (code *pfOperatingZoomCtrlParaGet_t)(UINT8 item);
typedef UINT16 (code *pfOperatingZoomFactorGet_t)(void);
typedef void (code *pfOperatingZoomSmoothSet_t)(UINT8 en);
typedef UINT8 (code *pfOperatingZoomSmoothGet_t)(void);
typedef UINT8 (code *pfOperatingZoomSmoothStatusGet_t)(void);
typedef UINT8 (code *pfOperatingZoomSmooth3APreSet_t)(void);
typedef UINT8 (code *pfOperatingZoomSmooth3APostSet_t)(void);
typedef UINT8 (code *pfOperatingZoomSmoothTeleSet_t)(void);
typedef UINT8 (code *pfOperatingZoomSmoothWideSet_t)(void);


typedef struct {
	pfOperatingZoomResourceLoad_t pfOpZoomResourceLoad;
	pfOperatingZoomSet_t pfOpZoomSet;
	pfOperatingZoomCtrlParaSet_t pfOpZoomCtrlParaSet;
	pfOperatingZoomCtrlParaGet_t pfOpZoomCtrlParaGet;
	pfOperatingZoomFactorGet_t pfOpZoomFactorGet;
	pfOperatingZoomSmoothSet_t pfOpZoomSmoothSet;
	pfOperatingZoomSmoothGet_t pfOpZoomSmoothGet;
	pfOperatingZoomSmoothStatusGet_t pfOpZoomSmoothStatusGet;
	pfOperatingZoomSmooth3APreSet_t pfOpZoomSmooth3APreSet;
	pfOperatingZoomSmooth3APostSet_t pfOpZoomSmooth3APostSet;
	pfOperatingZoomSmoothTeleSet_t pfOpZoomSmoothTeleSet;
	pfOperatingZoomSmoothWideSet_t pfOpZoomSmoothWideSet;
} zoomOperatingFunc_t;

typedef struct zoomDev_s {
	zoomOperatingFunc_t opFunc;
} zoomDev_t; 

typedef enum {
	ZOOM_LOGI_STILL_MODE = 0,
	ZOOM_LOGI_VIDEO_MODE,
	ZOOM_LOGI_MODE_MAX,
}ZOOM_LOGI_MODE;

typedef enum {
	ZOOM_LOGI_BLOCKING_TYPE = 0,
	ZOOM_LOGI_NONBLOCKING_TYPE,
	ZOOM_LOGI_TYPE_MAX,
}ZOOM_LOGI_TYPE;



 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern zoomStillCapabDesc_t stillCapabDesc;
extern zoomVideoCapabDesc_t videoCapabDesc; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 zoomDevInit(void);
UINT8 zoomDevOpen(zoomOperatingFunc_t *popFunc,UINT16 opFuncSize);
UINT8 zoomDevActSet(UINT8 mode,UINT8 type);
UINT8 zoomDevActGet(UINT8 mode,UINT8 *type);
UINT8 zoomDevModeCheck(	UINT8 *pmode,UINT8 *type,zoomDev_t **ppdev);
UINT8 zoomDevResourceLoad(UINT8 mode,UINT32 para1,UINT32 para2,UINT32 para3);
UINT8 zoomDevZoomSet(UINT8 mode,UINT8 para1,UINT16 para2,UINT8 para3);
UINT8 zoomDevZoomCtrlParaSet(UINT8 mode,UINT8 para1,UINT16 para2);
UINT8 zoomDevZoomCtrlParaGet(UINT8 mode,UINT8 para1,UINT16 *outpara1);
UINT8 zoomDevZoomFactorGet(UINT8 mode,UINT16 *outpara1);
UINT8 zoomDevZoomSmoothSet(UINT8 mode,UINT8 para1);
UINT8 zoomDevZoomSmoothGet(UINT8 mode,UINT8 *outpara1);
UINT8 zoomDevZoomSmoothStatusGet(UINT8 mode,UINT8 *outpara1);
UINT8 zoomDevZoomSmooth3APreSet(UINT8 mode);
UINT8 zoomDevZoomSmooth3APostSet(UINT8 mode);
UINT8 zoomDevZoomSmoothTeleSet(UINT8 mode);
UINT8 zoomDevZoomSmoothWideSet(UINT8 mode);
UINT16 com_zoomDevZoomFactorGet(UINT8 mode);
UINT8 com_zoomDevZoomSmoothGet(UINT8 mode);
UINT16 com_zoomDevZoomCtrlParaGet(UINT8 mode,UINT8 para1);
UINT8 com_zoomDevZoomSmoothStatusGet(UINT8 mode);
#endif  /* __ZOOM_LOGI_H__ */
