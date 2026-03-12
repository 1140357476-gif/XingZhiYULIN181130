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
 * @file      sp1k_snap_api.h
 * @brief     sp1k snap API for Host
 * @author    will.chiu
 * @since     2010-05-18
 * @date      2010-05-18
*/


#ifndef __SP1K_SNAP_API_H__
#define __SP1K_SNAP_API_H__

/** \file sp1k_snap_api.h
 * @brief		Spca1628 snap related interface
 */
#include "sp1k_def.h" 
#include "snap_task.h"
#include "knl_func_opt.h"
//#include "exifhdr.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP1K_SNAP_PARAM_QUALITY = 0,
	SP1K_SNAP_PARAM_FORMAT,
	SP1K_SNAP_PARAM_BURST,
	SP1K_SNAP_PARAM_QV,
	SP1K_SNAP_PARAM_QV_PERIOD,
	SP1K_SNAP_PARAM_PRE_BRC,
	SP1K_SNAP_PARAM_POST_BRC,
	SP1K_SNAP_PARAM_COMPRESS_RATIO,
	SP1K_SNAP_PARAM_ROT_FLIP_OPT,
	SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,
	SP1K_SNAP_PARAM_STAMP_OPT,
	SP1K_SNAP_PARAM_PHOTO_FRM_OPT,
	SP1K_SNAP_PARAM_QV_ROT_OPT,
	SP1K_SNAP_PARAM_SKIP_PV,
	SP1K_SNAP_PARAM_QV_EFFECT,
	SP1K_SNAP_PARAM_PV_FREEZE_OPT,
	SP1K_SNAP_PARAM_SAVE_RAW_OPT,
	SP1K_SNAP_PARAM_SAVE_PV_YUV_OPT,
	SP1K_SNAP_PARAM_BYPASS_QV_THB_OPT,
	SP1K_SNAP_PARAM_DOFLOW_STORE_FIRST_OPT,
	SP1K_SNAP_PARAM_PANORAMA_OPT,
	SP1K_SNAP_PARAM_SAVE_YUV_OPT,
	SP1K_SNAP_PARAM_SAVE_JPG_OPT,    
	SP1K_SNAP_PARAM_PH_WIDTH,
	SP1K_SNAP_PARAM_PH_HEIGHT,
	//SP1K_SNAP_PARAM_SOUND_LOW_ADDR,
	//SP1K_SNAP_PARAM_SOUND_HIGH_ADDR,
	SP1K_SNAP_PARAM_PH_LOW_ADDR,
	SP1K_SNAP_PARAM_PH_HIGH_ADDR,
	SP1K_SNAP_PARAM_BAD_PIXEL_OPT,
	SP1K_SNAP_PARAM_10_BIT_RAW_OPT,
	SP1K_SNAP_PARAM_BACKGROUND_WIDTH,
	SP1K_SNAP_PARAM_BACKGROUND_HEIGHT,
	SP1K_SNAP_PARAM_STAMP_X_RATIO,
	SP1K_SNAP_PARAM_STAMP_Y_RATIO,
	SP1K_SNAP_PARAM_STAMP_DT_DEFAULT_LOCATION_OPT,
	//SP1K_SNAP_PARAM_QV_WIDTH,
	//SP1K_SNAP_PARAM_QV_HEIGHT,
	SP1K_SNAP_PARAM_BLINK,				/* 1:blinking enable  0:blinking disable  0xFF: get msg of blinking */
	//------INTERNAL OPTION------//
	//SP1K_SNAP_PARAM_AUTO_POST_WB_OPT, /* will : delete it */
    SP1K_SNAP_PARAM_DZOOM_OPT,   	   	/* will: it's just for FPGA verification*/
    SP1K_SNAP_PARAM_DOFLOW_OPT,     
    SP1K_SNAP_PARAM_CHANGERAW, 			/* will: for FPGA verification */
	SP1K_SNAP_PARAM_LSCCAL,
	SP1K_SNAP_PARAM_REDO_PREPROCESS,
	//------DEBUG OPTION------//
	SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,
	//------TIMELAPSE--------//
	SP1K_SNAP_TIMESLAPE_STS,
	SP1K_SNAP_TIMESLAPE_PAUSE,
	//------MMS-------------//
	SP1K_SANP_PARAM_MMS_STS,
	SP1K_SNAP_PARAM_MAX
} snapParam_e;

typedef enum {
	SP1K_SNAP_CALLBACK_BURST = 0,
	SP1K_SNAP_CALLBACK_BACKGROUND_RES_LOAD,
	SP1K_SNAP_CALLBACK_HOST_STAMP,
	SP1K_SNAP_CALLBACK_MAX
} snapCallback_e;

typedef enum {
	SP1K_SNAP_MSG_OK = 0,
	SP1K_SNAP_MSG_FILE_WRITE_ERROR,
	SP1K_SNAP_MSG_MAX
} snapMsg_e;

typedef enum {
	SP1K_SNAP_QV_EFFECT_NORMAL = 0,
	SP1K_SNAP_QV_EFFECT_BLACKWHITE,
	SP1K_SNAP_QV_EFFECT_SEPIA,
	SP1K_SNAP_QV_EFFECT_BINARIZE,
	SP1K_SNAP_QV_EFFECT_MAX
} snapQvEffect_e;

typedef enum {
	SNAP_INFO_RAW_BUF_ADDR = 0,
	SNAP_INFO_MAX
} snapInfo_e;

typedef enum {
	SP1K_SNAP_PRE_REDO_DISABLE = 0,
	SP1K_SNAP_POST_WB,
	SP1K_SNAP_PRE_REDO,
	SP1K_SNAP_PRE_REDO_DARKSUB,
	SP1K_SNAP_GET_WBGAIN,
} snapPreRedo_e;



#define SP1K_SNAP_FORMAT_422	0
#define SP1K_SNAP_FORMAT_420	1
#define SP1K_SNAP_FORMAT_400	2

#define SP1K_SNAP_QUALITY_SUPER_FINE	0
#define SP1K_SNAP_QUALITY_FINE			1
#define SP1K_SNAP_QUALITY_NORMAL		2




/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn        UINT8 sp1kSnapParamSet(UINT8 attrId, UINT16 value)
 * @brief     set parameters of snap
 * @param     [in] attrId : idx of parameters
 * @param     [in] value : parameter value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kSnapParamSet( UINT8 attrId, UINT16 value );

/**
 * @fn        UINT16 sp1kSnapParamGet(UINT8 attrId)
 * @brief     get parameters of snap
 * @param     [in] attrId : index of parameters
 * @retval    return = parameters value
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT16 sp1kSnapParamGet( UINT8 attrId );

 /**
  * @fn 	   UINT8 snapStore(void)
  * @brief	   save jpg file
  * @param	   NONE
  * @retval    return = SUCCESS / FAIL
  * @see	   NULL
  * @author    Will Chiu
  * @since	   2010-5-15
  * @todo	   N/A
  * @bug	   N/A
 */
UINT8 sp1kSnapStore( void );

 /**
  * @fn 	   UINT8 sp1kSnapTurnOnPv(void)
  * @brief	   Go back to preview state
  * @param	   NONE
  * @retval    return = SUCCESS / FAIL
  * @see	   NULL
  * @author    Will Chiu
  * @since	   2010-5-15
  * @todo	   N/A
  * @bug	   N/A
 */
UINT8 sp1kSnapTurnOnPv( void );

#else
#define sp1kSnapParamSet  snapParamSet
#define sp1kSnapParamGet  snapParamGet 
#define sp1kSnapStore()	snapStore(NULL)
#define sp1kSnapTurnOnPv snapPvTurnOn
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


/* snap_task.c*/

/**
 * @fn        UINT8 sp1kSnapInit(UINT32 dramMByteSize, UINT32 memBeginAddrWord, UINT32 memEndAddrWord)
 * @brief     initialize memory for snap 
 * @param     [in] memBeginAddrWord : start address of memory which can be used for snap (word address)
 * @param     [in] memEndAddrWord : end address of meomory which can be used for snap (word address)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kSnapInit( UINT32 memBeginAddrWord, UINT32 memEndAddrWord );
 
 /**
 * @fn        UINT8 sp1kSnapTask(void)
 * @brief     It triggers snap flow to capture image.
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kSnapTask( void );


 /**
 * @fn        UINT8 sp1kJpegQTableSet(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     It set the Q-table which jpeg uses for encode and decode. It can set with Q index or customized table
 * @param     [in] QTableIndex : q table index
 * @param     [in] yQTable : input Y q table
 * @param     [in] uvQTable : input U/V q table
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       jpegQTableSet
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kJpegQTableSet( UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt );


 /**
  * @fn 	   void sp1kSnapCallbackReg(UINT32 id, void *pfunct)
  * @brief	   register callback of snap
  * @param	   [in] id : index of callback
  * @param	   [in] pfunct : function point you wanna register
  * @retval    NONE
  * @see	   NULL
  * @author    Will Chiu
  * @since	   2010-5-17
  * @todo	   N/A
  * @bug	   N/A
 */
void sp1kSnapCallbackReg( UINT32 id, void *pfunct );

 /**
  * @fn 	   void sp1kSnapSizeSet(UINT16 hSize, UINT16 vSize)
  * @brief	   Set horizontal/vertical size of capture image
  * @param	   [in] hSize : horizontal size
  * @param	   [in] vSize : vertical size
  * @retval    NONE
  * @see	   NULL
  * @author    Will Chiu
  * @since	   2010-5-17
  * @todo	   N/A
  * @bug	   N/A
 */
void sp1kSnapSizeSet( UINT16 hSize, UINT16 vSize );

 /**
  * @fn 	   void sp1kSnapSizeGet(UINT16* hSize, UINT16* vSize)
  * @brief	   Get horizontal/vertical size of capture image
  * @param	   [out] hSize : horizontal size
  * @param	   [out] vSize : vertical size
  * @retval    NONE
  * @see	   NULL
  * @author    Will Chiu
  * @since	   2010-5-17
  * @todo	   N/A
  * @bug	   N/A
 */
void sp1kSnapSizeGet( UINT16* hSize, UINT16* vSize );

/**
 * @fn        void sp1kSnapDzoomFactorSet(UINT16 hFactor, UINT16 vFactor)
 * @brief     Set digital zoom factor for snap
 * @param     [in] hFactor : horizontal zoom factor (100 -> 1x , 200 -> 2x)
 * @param     [in] vFactor : vertical zoom factor (100 -> 1x , 200 -> 2x) 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSnapDzoomFactorSet( UINT16 hFactor, UINT16 vFactor );

/**
 * @fn        void sp1kSnapCropSizeSet(UINT16 cropHSize, UINT16 cropVSize)
 * @brief     Set crop size for capture. It can't be bigger than sensor full size.
 * @param     [In] cropHSize : horizontal size	
 * @param     [In] cropVSize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSnapCropSizeSet(UINT16 cropHSize,UINT16 cropVSize);






//#if FUNC_KNL_FRM
/**
 * @fn		  void sp1kSnapPhotoFrameParaSet(UINT8 resId, UINT32 rscAddr)
 * @brief	  Set parameters for photo frame in capture mode.
 * @param	  [in] resId : resource id 
 * @param	  [in] rscAddr : the address of photo frame in DRAM.
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Will Chiu
 * @since	  2011-3-16
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kSnapPhotoFrameParaSet(	UINT16 resId,UINT32 rscAddr);
//#endif



#endif  /* __SP1K_SNAP_API_H__ */

