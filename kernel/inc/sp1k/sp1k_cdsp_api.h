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
  * @file	   sp1k_cdsp_api.h
  * @brief	   sp1k cdsp API for Host
  * @author    will.chiu
  * @since	   2010-05-18
  * @date	   2010-05-18
 */

#ifndef __SP1K_CDSP_API_H__
#define __SP1K_CDSP_API_H__

/** \file sp1k_cdsp_api.h
 * @brief		Spca1628 cdsp related interface
 */

#include "sp1k_def.h"
#include "cdsp_task.h"
#include "cdsp_misc.h"
#include "cdsp_filter.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#ifdef SP1K_DOXY_GEN
/**
 * @fn        void sp1kCdspEffect_BlackWhite(void)
 * @brief     black&white effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_BlackWhite( void );

/**
 * @fn        void sp1kCdspEffect_Cold(void)
 * @brief     cold effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Cold( void );

/**
 * @fn        void sp1kCdspEffect_Warm(void)
 * @brief     warm effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Warm( void );

/**
 * @fn        void sp1kCdspEffect_Sunset(void)
 * @brief     sunset effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Sunset( void );


/**
 * @fn        void cdspSepiaEffectSet(void)
 * @brief     sepia effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Sepia( void );

/**
 * @fn        void cdspNegativeEffectSet(void)
 * @brief     negative effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/ 
 
void sp1kCdspEffect_Negative( void );

/**
 * @fn        void sp1kCdspEffect_Binarize(void)
 * @brief     binarize effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Binarize( void );


/**
 * @fn        void sp1kCdspEffect_ContrastSet(void)
 * @brief     contrast effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_ContrastSet( void );

/**
 * @fn        void sp1kCdspEffect_Red(void)
 * @brief     red effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Red( void );

/**
 * @fn        void sp1kCdspEffect_Blue(void)
 * @brief     bule effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Blue( void );

/**
 * @fn        void sp1kCdspEffect_Green(void)
 * @brief     green effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Green( void );

/**
 * @fn        void sp1kCdspEffect_Normal(void)
 * @brief     normal effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEffect_Normal( void );


/**
 * @fn        void sp1kCdspEdgeFilterSet(UINT8 level, UINT8 opt)
 * @brief     Several sets of edge fileter parameters. It needs resource, "EdgeCoring.bin".
 * @param     [in] level : 0~10 for edge ( higher -> stronger)
 * @param     [in] opt :  0: capture 
 *\n					  1: preview
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspEdgeFilterSet(UINT8 level,UINT8 opt);

/**
 * @fn        void sp1kCdspCoringFilterSet(UINT8 level)
 * @brief     6 sets of coring parameter. It only works for capture mode.
 * @param     [in] level  :  0~5  (higher -> stronger)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspCoringFilterSet(UINT8 level);

/**
 * @fn        void sp1kCdspStillDynamicEnhanSet(UINT8 fileId,	UINT8 intensity)
 * @brief     load resource for preview edge 
 * @param     [in] fileId : resource id
 * @param     [in] intensity : 0 weak, 1 middle, 2 strong
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspStillDynamicEnhanSet(UINT8 fileId, UINT8 intensity);


/**
 * @fn        void sp1kCdspPvDynamicEnhanSet(UINT8 fileId,	UINT8 intensity)
 * @brief     Set egge level for preview
 * @param     [in] fileId : resource id
 * @param     [in] intensity : 0 weak, 1 middle, 2 strong
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspPvDynamicEnhanSet(UINT8 fileId, UINT8 intensity);

/**
 * @fn        void sp1kCdspPvDynamicParaFullLoad(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspPvDynamicParaFullLoad(UINT8 fileId);

/**
 * @fn        UINT8 sp1kCdspTDEn(UINT8 en, UINT8 level)
 * @brief     enable/disable temporal denoise and set level of de-noise
 * @param     [in] en    1: on   0: off
 * @param     [in] level	0: weak  1: normal  2:strong
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-23
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspTDEn(UINT8 en,UINT8 level); 

/**
 * @fn        UINT8 sp1kCdspPvDynamicTDEn(UINT8 en, UINT8 level)
 * @brief      enable/disable preview dynamic ctrl temporal denoise (based on sensor gain) and set level of de-noise
 * @param     [in] en    1: on (after ae enable)  0: off (before ae disable)
 * @param     [in] level	0: weak  1: normal  2:strong
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-23
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspPvDynamicTDEn(UINT8 en,UINT8 level); 

/**
 * @fn        void sp1kCdspSaturationEnhanSet(	UINT8 en, UINT8 uvsatFactor, UINT8 uOffset, UINT8 vOffset)
 * @brief     sp1kCdspSaturationEnhanSet
 * @param     [in] uvsatFactor : UV saturation factor (3.5 bit)
 * @param     [in] UOffset : U Offset ( -128 ~ 127 )
 * @param     [in] VOffset : V Offset ( -128 ~ 127 )
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspSaturationEnhanSet(UINT8 en, UINT8 uvsatFactor, UINT8 uOffset, UINT8 vOffset);

/**
 * @fn        void sp1kCdspContrastEnhanSet(UINT8 en,	UINT8 yConFactor)
 * @brief     sp1kCdspContrastEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] yConFactor : Y contrast factor (3.5 bit)
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspContrastEnhanSet(UINT8 en, UINT8 yConFactor);

/**
 * @fn        void sp1kCdspSharpnessEnhanSet(UINT8 stillDynamicEn,	UINT8 stillDynamicEnhanMode)
 * @brief     sp1kCdspSharpnessEnhanSet
 * @param     [in] stillDynamicEn : enable/disable
 * @param     [in] stillDynamicEnhanMode : CDSP_DYNAMIC_WEAK = 0,	CDSP_DYNAMIC_MIDDLE, CDSP_DYNAMIC_STRONG,
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspSharpnessEnhanSet(UINT8 stillDynamicEn,	UINT8 stillDynamicEnhanMode);

/**
 * @fn        void sp1kCdspBrightnessEnhanSet(UINT8 en,	SINT8 brightnessOffset)
 * @brief     sp1kCdspBrightnessEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] brightnessOffset : brightness Offset (2's complement, -128~+127)
 *\n                                               +127 : 01111111
 *\n                                               -128 : 10000000
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspBrightnessEnhanSet(UINT8 en,SINT8 brightnessOffset);

/**
 * @fn        void sp1kCdspHueEnhanSet(UINT8 en,	SINT8 brightnessOffset)
 * @brief     sp1kCdspHueEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] channel : 0, 1, 2
 * @param     [in] fileId : resource id
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspHueEnhanSet(UINT8 en,UINT8 channel,UINT8 fileId);

#else
#define sp1kCdspEffect_BlackWhite cdspBlackWhiteEffectSet
#define sp1kCdspEffect_Cold	cdspColdEffectSet
#define sp1kCdspEffect_Warm	cdspWarmEffectSet
#define sp1kCdspEffect_Sunset cdspSunsetEffectSet


#define sp1kCdspEffect_Sepia cdspSepiaEffectSet
#define sp1kCdspEffect_Negative	cdspNegativeEffectSet
#define sp1kCdspEffect_Binarize	cdspBinarizeEffectSet
#define sp1kCdspEffect_ContrastSet cdspContrastEffectSet
#define sp1kCdspEffect_Red cdspRedEffectSet
#define sp1kCdspEffect_Blue	cdspBlueEffectSet
#define sp1kCdspEffect_Green cdspGreenEffectSet
#define sp1kCdspEffect_Normal cdspNormalEffectSet

#define sp1kCdspEdgeFilterSet cdspEdgeFilterSet
#define sp1kCdspCoringFilterSet cdspCoringFilterSet

#define sp1kCdspStillDynamicEnhanSet cdspStillDynamicEnhanSet
#define sp1kCdspPvDynamicEnhanSet cdspPvDynamicEnhanSet
#define sp1kCdspPvDynamicParaFullLoad cdspPvDynamicParaFullLoad

#define sp1kCdspTDEn cdspTempDenoiseEn
#define sp1kCdspPvDynamicTDEn cdspPvDynamicTempDenoiseEn

#define sp1kCdspSaturationEnhanSet cdspSaturationEnhanSet
#define sp1kCdspContrastEnhanSet cdspContrastEnhanSet
#define sp1kCdspSharpnessEnhanSet cdspSharpnessEnhanSet
#define sp1kCdspBrightnessEnhanSet cdspBrightnessEnhanSet
#define sp1kCdspHueEnhanSet cdspHueEnhanSet
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
 * @fn		  void sp1kCdspParaInit(UINT8 fileId)
 * @brief	  cdsp initialize (load tables)
 * @param	  [in] fileId
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Will Chiu
 * @since	  2010-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kCdspParaInit( UINT8 fileId );


/**
 * @fn        void sp1kCdspGammaInit(UINT8 fileId)
 * @brief     load gamma table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspGammaInit( UINT8 fileId );
/**
 * @fn        void sp1kCdspLscInit(UINT8 fileId)
 * @brief     load LSC table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspLscInit( UINT8 fileId );

/**
 * @fn        void sp1kCdspWdrInit(UINT8 fileId)
 * @brief     load WDR table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspWdrInit( UINT8 fileId );

/**
 * @fn        void sp1kCdspPvParaInit(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspPvParaInit( UINT8 fileId );

/**
 * @fn        void sp1kCdspPvParaInit(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspPvDynamicParaInit( UINT8 fileId );

/**
 * @fn        void sp1kCdspStillParaInit(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspStillParaInit( UINT8 fileId );

/**
 * @fn        void sp1kCdspPvDynamicParaInit(UINT8 fileId)
 * @brief     load still cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCdspStillDynamicParaInit( UINT8 fileId );


/**
 * @fn        void sp1kHalCdspImgSrcSel(UINT8 srcSel)
 * @brief     switch cdsp source from front or dram data. 
 * @param     [in] srcSel :  0 -> From Front Interface
 *							 1 -> From SDRAM
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kHalCdspImgSrcSel( UINT8 srcSel );



//UINT8 sp1kCdspManualWbGainSet(UINT8 WbRGain, UINT8 WbBGain);

#endif //__SP1K_OSD_API_H__

