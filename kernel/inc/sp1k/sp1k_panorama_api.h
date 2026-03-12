
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
#ifndef __SP1K_PANORAMA_API_H__
#define __SP1K_PANORAMA_API_H__

/** \file sp1k_panorama_api.h
 * @brief		Spca1628 Panorama related interface 
 */
 
#include "panorama_def.h"
#include "panorama_api.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum panoramaEn_e {
	PANORAMA_DISABLE = 0,
	PANORAMA_ENABLE,
} panoramaEn_t;


typedef enum {
	SP1K_PANORAMA_PARA_FRAME_NUM = 0,								/*< Panorama Stitch Source Frame Number , defult is 15.*/
	SP1K_PANORAMA_PARA_STITCH_PIXEL,                                /*< Panorama Stitch Pixel , defult is 80.*/
}SP1K_PANORAMA_PARAM_SET_MODE;


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
 * @fn                void sp1kPanoramaInit(void)
 * @brief             	Initial Panorama setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            none
 */
void sp1kPanoramaInit(void);

/**
 * @fn                void sp1kPanoramaEnSet(UINT8 en);
 * @brief	            To Set Panorama On/off Status function.
 * @param[in]      en : (The status of Panorama)
 *\n                              PANORAMA_DISABLE: Panorama turn OFF.
 *\n                              PANORAMA_ENABLE: Panorama turn ON.
 * @retval            none          
 */
void sp1kPanoramaEnSet(UINT8 en);

/**
 * @fn                UINT8 sp1kPanoramaEnGet(void);
 * @brief	            To Get Panorama On/off Status function.
 * @param[in]      none
 * @retval          The status of Panorama
 *\n                              PANORAMA_DISABLE: Panorama turn OFF.
 *\n                              PANORAMA_ENABLE: Panorama turn ON.            
 */
UINT8 sp1kPanoramaEnGet(void);

/**
 * @fn                UINT8 sp1kPanoramaGMVByFrameCalculus(SINT16 *gmvXByFrame, SINT16 *gmvYByFrame, UINT8 *status)
 * @brief	            To Get Global Motion Vector By frame
 * @param[in]      *gmvXByFrame : Global Motion Vector x By frame (half-pixel)
 *                       *gmvYByFrame : Global Motion Vector y By frame (half-pixel)
 *                       *status : 0:noraml, 1:too fast, 2:too slow
 * @retval          The status of Panorama
 *\n                              SUCCESS/FAIL
 */
UINT8 sp1kPanoramaGMVByFrameCalculus(SINT16 *gmvXByFrame, SINT16 *gmvYByFrame, UINT8 *status);

/**
 * @fn                
 UINT8 sp1kPanoramaSumOfGMVCalculus (SINT16 *sumOfGmvX, SINT16 *sumOfGmvY) 
 * @brief	            To Get Sum of Global Motion Vector
 * @param[in]      *sumOfGmvX : Sum of Global Motion Vector x (half-pixel)
 *                       *sumOfGmvY : Sum of Global Motion Vector y (half-pixel)
 * @retval          The status of Panorama
 *\n                              SUCCESS/FAIL     
 */
UINT8 sp1kPanoramaSumOfGMVCalculus (SINT16 *sumOfGmvX, SINT16 *sumOfGmvY);

/**
 * @fn                UINT8 sp1kPanoramaParamSet(UINT8 selector,UINT16 val)
 * @brief	            To Get Panorama On/off Status function.
 * @param[in]      selector : type eunm SP1K_PANORAMA_PARAM_SET_MODE
 * @param[in]      val : SP1K_PANORAMA_PARA_FRAME_NUM = Panorama Stitch Source Frame Number , defult is 15.
 *                              SP1K_PANORAMA_PARA_STITCH_PIXEL = Panorama Stitch Pixel , defult is 80.
 * @retval          The status of Panorama
 *\n                              SUCCESS
 *\n                              FAIL
 */
UINT8 sp1kPanoramaParamSet(UINT8 selector,UINT16 val);

/**
 * @fn                UINT8 sp1kPanoramaStitchFunc(void)
 * @brief	            Stitch function.
 * @param[in]      nona
 * @retval          The status of Panorama
 *\n                              SUCCESS
 *\n                              FAIL
 */
UINT8 sp1kPanoramaStitchFunc (void);

/**
 * @fn                void sp1kPanoramaFinalSizeSet(UINT32 w, UINT32 h); 
 * @brief	            To Set Panorama Size function.
 * @param[in]      w : width (16x)
 * @param[in]      h : height (16x)
 * @retval          The status of Panorama
 *\n                              SUCCESS
 *\n                              FAIL
 */
void sp1kPanoramaFinalSizeSet(UINT32 w,	UINT32 h);

#else

#define sp1kPanoramaInit panoramaInit
#define sp1kPanoramaEnSet	panoramaEnSet
#define sp1kPanoramaEnGet	panoramaEnGet
#define sp1kPanoramaGMVByFrameCalculus panoramaGMVByFrameFunc
#define sp1kPanoramaSumOfGMVCalculus panoramaSumOfGMVFunc
#define sp1kPanoramaParamSet panoramaParamSet
#define sp1kPanoramaDstFileSave panoramaDstFileSave
#define sp1kPanoramaStitchFunc panoramaGprmStitchFunc
#define sp1kPanoramaFinalSizeSet panoFinalSizeSet

#endif

#endif //__SP1K_PANORAMA_API_H__

