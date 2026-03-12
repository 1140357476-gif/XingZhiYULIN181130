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
  * @file	   sp1k_pv_api.h
  * @brief	   sp1k pv API for Host
  * @author    will.chiu
  * @since	   2010-05-18
  * @date	   2010-05-18
 */

#ifndef __SP1K_PV_API_H__
#define __SP1K_PV_API_H__

/** \file sp1k_pv_api.h
 * @brief		Spca1628 preview related interface.
 */

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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

#include "sp1k_def.h"
#include "pv_task.h"
#include "pv_photoframe.h"
#include "app_video_view.h"
#include "sp1k_zoom_api.h"



/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP1K_PRV_BLEND_PHOTOFRM = PRV_BLEND_PHOTOFRM,
	SP1K_PRV_BLEND_ROTATION = PRV_BLEND_ROTATION,
	SP1K_PRV_BLEND_END		= PRV_BLEND_END,
} sp1kPrvBlendMode_e;

typedef enum sp1kDsParam_e {
	SP1K_DS_PARAM_FONT_PASTETHD = 0,
	SP1K_DS_PARAM_STAMP_PASTETHD,

	SP1K_DS_PARAM_MAX
} sp1kDsParam_t;

typedef enum {
	SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG = PV_ZOOM_CTRL_PARA_CURRENT_LOG,
	SP1K_PV_ZOOM_CTRL_PARA_MAX = PV_ZOOM_CTRL_PARA_MAX,
}SP1K_PV_ZOOM_CTRL_PARA;


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

 /**
 * @fn        UINT8 sp1kPvZoomSet(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze)
 * @brief     Set zoom factor in preview mode. Range is from 100(1.0x)  to 400(4.0x)
 * @param     [in] pvStart :  0: wait until preview zoom update ready. In general, 0 should be set for repeat zoom.
 *\n					 	  1: do not wait any zoom update ready signal                                 
 * @param     [in] factor :   zoom factor. range is 100 to 400
 * @param     [in] pbFreeze  : 0: It shows image immedaitely
 *\n						   1: It doesn't show image 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kPvZoomSet(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze);

/**
 * @fn        void sp1kPvSizeGet(UINT16 *phSize, UINT16 *pvSize)
 * @brief     get preview size
 * @param     [out] hSize : preview horizontal size
 * @param     [out] vSize : preview vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPvSizeGet(UINT16 *phSize, UINT16 *pvSize);


/**
 * @fn        UINT32 pvFreeBuffGet(void)
 * @brief     get free dram address 
 * @param     NONE
 * @retval    return = free dram address 
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT32 sp1kPvFreeBuffGet(void);

/**
 * @fn void sp1kPvZoomResLoad(void)
 * @brief	load stillview zoom table parameter resource file
 * @retval the statring of system free physical buffer address in word unit
 */
void sp1kPvZoomResLoad(void);

/**
 * @fn		  UINT16 sp1kPvZoomCtrlParaGet(UINT8 item)
 * @brief	  still view zoom system crtrl parameter get
 * @param	 item : typedef enum SP1K_PV_ZOOM_CTRL_PARA
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16 sp1kPvZoomCtrlParaGet(UINT8 item);

/**
 * @fn		  void sp1kPvZoomCtrlParaSet(UINT8 item,	UINT16 value)
 * @brief	  still view zoom system crtrl parameter set
 * @param	 item : typedef enum SP1K_PV_ZOOM_CTRL_PARA
 * @param     value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void sp1kPvZoomCtrlParaSet(UINT8 item,UINT16 value);

#else
	#define sp1kPvZoomSet(para1,para2,para3)     zoomDevZoomSet(SP1K_ZOOM_LOGI_STILL_MODE,para1,para2,para3)//pvZoomSet
	#define sp1kPvSizeGet      pvSizeGet
	#define sp1kPvFreeBuffGet  pvFreeBuffGet
	
	#define sp1kPvZoomResLoad(para1,para2,para3)   zoomDevResourceLoad(SP1K_ZOOM_LOGI_STILL_MODE,para1,para2,para3)
	#define sp1kPvZoomFctGet()   com_zoomDevZoomFactorGet(SP1K_ZOOM_LOGI_STILL_MODE)//zoomDevZoomFactorGet(UINT8 mode,UINT16 *outpara1)// pvZoomFctGet
	#define sp1kPvSmoothZoomSet(para1) zoomDevZoomSmoothSet(SP1K_ZOOM_LOGI_STILL_MODE,para1)//pvSmoothZoomSet
	#define sp1kPvSmoothZoomGet() com_zoomDevZoomSmoothGet(SP1K_ZOOM_LOGI_STILL_MODE)//pvSmoothZoomGet
	#define sp1kPvSmoothZoomStatusGet() com_zoomDevZoomSmoothStatusGet(SP1K_ZOOM_LOGI_STILL_MODE)//pvSmoothZoomStatusGet
	
	#define sp1kPvZoom3APreSet()  zoomDevZoomSmooth3APreSet(SP1K_ZOOM_LOGI_STILL_MODE)//pvZoom3APreSet
	#define sp1kPvZoom3APostSet() zoomDevZoomSmooth3APostSet(SP1K_ZOOM_LOGI_STILL_MODE)//pvZoom3APostSet
	
	#define	sp1kPvTeleSet()       zoomDevZoomSmoothTeleSet(SP1K_ZOOM_LOGI_STILL_MODE)//pvTeleSet
	#define sp1kPvWideSet()       zoomDevZoomSmoothWideSet(SP1K_ZOOM_LOGI_STILL_MODE)//pvWideSet

	#define sp1kPvZoomCtrlParaSet(para1,para2) zoomDevZoomCtrlParaSet(SP1K_ZOOM_LOGI_STILL_MODE,para1,para2)//pvZoomCtrlParaSet
	#define sp1kPvZoomCtrlParaGet(para1) com_zoomDevZoomCtrlParaGet(SP1K_ZOOM_LOGI_STILL_MODE,para1)//pvZoomCtrlParaGet

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

/**
 * @fn		  void sp1kFrontInit(void)
 * @brief	  Init front setting. It must be called before the system enters peview mode
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Will Chiu
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kFrontInit(void);

/**
 * @fn        UINT8 sp1kPreviewSet(UINT8 vidMode, UINT16 pvHSize, UINT16 pvVSize, UINT16 fact, UINT8 pbFreeze)
 * @brief     enter still/video preview mode
 * @param     [in] vidMode : 
 *							0: Still preview mode
 *							1: Video preview mode
 * @param     [in] pvHSize
 *							still preview buffer : 320x240(default), it depends on the size of display usually 
 *							video preview buffer : 1280x720(maximum), it depends the size of video you wanna record						
 * @param     [in] pvVSize : the same as pvHSize
 * @param     [in] fact : zoom factor 
 * @param     [in] pbFreeze : 
 *							0: show preview buffer immediately
 *							1: preview will not show immediately until sp1kPrevewUnfreeze() is called.                                  
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kPreviewSet(UINT8 vidMode, UINT16 pvHSize,UINT16 pvVSize, UINT16 fact, UINT8 pbfreeze);

/**
 * @fn        void sp1kCDSPInit(void)
 * @brief     CDSP Initialize
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Huiling Chou
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCDSPInit(void);

/**
 * @fn        void sp1kPrevewUnfreeze(UINT8 opt)
 * @brief     Unfreeze preview after power on
 * @param     [in] opt	0: skip writing frist frame buffer
\n 						1: start writing first frame buffer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-13
 * @todo      N/A
 * @bug       N/A
*/
void  sp1kPrevewUnfreeze(UINT8 opt);

/**
 * @fn        void sp1kPreviewCropSizeSet(UINT16 cropHSize, UINT16 cropVSize)
 * @brief     set preview crop size
 * @param     [in] cropHSize : crop horizontal size 
 * @param     [in] cropVSize : crop vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPreviewCropSizeSet(UINT16 cropHSize, UINT16 cropVSize);

/**
 * @fn        void sp1kPreviewCropSizeGet(UINT16 *cropHSize, UINT16 *cropVSize)
 * @brief     get preview crop size
 * @param     [out] cropHSize : crop horizontal size 
 * @param     [out] cropVSize : crop vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPreviewCropSizeGet(UINT16 *cropHSize, UINT16 *cropVSize);

/**
 * @fn        void sp1kPvPhotofrmEnable(UINT8 enable)
 * @brief     sp1kPvPhotofrmEnable
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPvPhotofrmEnable(UINT8 enable);

/**
 * @fn        void sp1k2ndPathCfg(UINT8 en)
 * @brief     2nd Path disp data in dram
 * @param     [in] en : enable 2nd path skip writing image data to dram
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1k2ndPathCfg(UINT8 en);

/**
 * @fn        void sp1kSuperPreviewSet(UINT8 mode,	UINT8 en)
 * @brief     super preview mode enable/disable (not support record)
 * @param     [in] mode : 
 *							0: Still preview mode
 *							1: Video preview mode
 * @param     [in] en : 
 *							0: Super Preview Diable
 *							1: Super Preview Enable
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSuperPreviewSet(UINT8 mode,UINT8 en);

/**
 * @fn        UINT8 sp1kSuperPreviewStatusGet(UINT8 mode)
 * @brief     Get HDMI HD preview mode status
 * @param     [in] mode : 
 *							0: Still preview mode
 *							1: Video preview mode
 * @retval     [out] en : 
 *							0: Super Preview Diable
 *							1: Super Preview Enable
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kSuperPreviewStatusGet(UINT8 mode);
/**
 * @fn        void  sp1kHDPvsizeStatusSet(UINT8 stauts);
 * @brief     Set HD preview size status
 * @param     [in] mode : 
 *							0: 6/8
 *							1: 8/8
 * @see       NULL
 * @author    rf.wang
 * @since     2012-03-19
 * @todo      N/A
 * @bug       N/A
*/
void  sp1kHDPvsizeStatusSet(UINT8 stauts);
/**
 * @fn       UINT8 sp1kHDPvsizeStatusGet();
 * @brief     Get HD pv size status
 * @retval     [out] en : 
 *							0: 6/8
 *							1:8/8
 * @see       NULL
 * @author  rf.wang
 * @since     2012-03-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kHDPvsizeStatusGet();
/**
 * @fn       UINT8 sp1kHDPvsizeReSet();
 * @brief     if SD card class<=class4 and curr pv size=8/8,reset pv size status
 * @retval     [out] en : 
 *							0: no reset HD pv size
 *							1:eset HD pv size
 * @see       NULL
 * @author  rf.wang
 * @since     2012-03-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kHDPvsizeReSet();
void sp1kYuvBufModeSet();
#endif //__SP1K_PV_API_H__

