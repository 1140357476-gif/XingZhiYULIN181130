
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
#ifndef __SP1K_MD_API_H__
#define __SP1K_MD_API_H__

/** \file sp1k_md_api.h
 * @brief		Spca1628 MD related interface 
 */
 
#include "motion_det.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum mdSensitivity_e {
	MD_SEN_LOW = 0,
	MD_SEN_MIDDLE,
	MD_SEN_HIGH,
} mdSensitivity_t;

typedef enum mdEn_e {
	MD_DISABLE = 0,
	MD_ENABLE,
} mdEn_t;

typedef enum mdLearningEn_e {
	MD_LEARNING_DISABLE = 0,
	MD_LEARNING_ENABLE,
} mdLearningEn_t;

typedef enum mdAdjustTdEn_e {
	MD_ADJUST_TD_DISABLE = 0,
	MD_ADJUST_TD_ENABLE,
} mdAdjustTdEn_t;

typedef struct _MD_BAR{
	UINT8 numeratorMdStatus;//¤À¤l
	UINT8 denominatorMdStatus;//¤À¥À
}MD_BAR;

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
 * @fn                void sp1kMDInit(void)
 * @brief             	Initial Motion Detection setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            none
 */
void sp1kMDInit(void);

/**
 * @fn                void sp1kMDEnSet(UINT8 en);
 * @brief	            To Set MD On/off Status function.
 * @param[in]      en : (The status of MD)
 *\n                              MD_DISABLE: MD turn OFF.
 *\n                              MD_ENABLE: MD turn ON.
 * @retval            none          
 */
void sp1kMDEnSet(UINT8 en);

/**
 * @fn                UINT8 sp1kMDEnGet(void);
 * @brief	            To Get MD On/off Status function.
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 sp1kMDEnGet(void);

/**
 * @fn                void sp1kMDSenSet(UINT8 sen);
 * @brief	            To Set MD Sensitivity function.
 * @param[in]      sen : (The status of MD Sensitivity)
 *\n                              MD_SEN_LOW: low Sensitivity.
 *\n                              MD_SEN_MIDDLE: middle Sensitivity.
 *\n                              MD_SEN_HIGH: high Sensitivity.
 * @retval           none
 */
void sp1kMDSenSet(UINT8 sen);

/**
 * @fn                UINT8 sp1kMDSenGet(void);
 * @brief	            To Get MD Sensitivity function.
 * @param[in]      none
 * @retval           The status of MD Sensitivity:
 *\n                              MD_SEN_LOW: low Sensitivity.
 *\n                              MD_SEN_MIDDLE: middle Sensitivity.
 *\n                              MD_SEN_HIGH: high Sensitivity.
 */
UINT8  sp1kMDSenGet(void);

/**
 * @fn                void sp1kMDLearningStart(UINT8 start);
 * @brief	            Set MD Learning function.
 * @param[in]      start : MD Learning function enable/disable
 *\n                              MD_LEARNING_ENABLE: Learning function enable.
 *\n                              MD_LEARNING_DISABLE: Learning function disable.
 * @retval           none
 */
void sp1kMDLearningStart(UINT8 start);

/**
 * @fn                void sp1kMDAdjustTempoalDenoiseWeightEnSet(UINT8 en);
 * @brief	            To Set MD Adjust Tempoal Denoise Weight Switch
 * @param[in]      en :
 *\n                              MD_ADJUST_TD_DISABLE: disable
 *\n                              MD_ADJUST_TD_ENABLE: enable
 * @retval           none
 */
void sp1kMDAdjustTempoalDenoiseWeightEnSet(UINT8 en);

/**
 * @fn                MD_BAR* sp1kMdBarGet (void)
 * @brief	            MD Bar value get
 * @param[in]      NULL
 * @retval           typedef struct _MD_BAR
 */
MD_BAR* sp1kMdBarGet (void);

#else

#define sp1kMDInit mdInit
#define sp1kMDEnSet	mdEnSet
#define sp1kMDEnGet	mdEnGet
#define sp1kMDSenSet mdSenSet
#define sp1kMDSenGet mdSenGet
#define sp1kMDLearningStart mdLearningStart
#define sp1kMDAdjustTempoalDenoiseWeightEnSet mdAdjustTempoalDenoiseWeightEnSet
#define sp1kMdBarGet mdBarGet
#endif

#endif //__SP1K_MD_API_H__

