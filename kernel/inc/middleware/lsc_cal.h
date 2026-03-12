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
 * @file      lsc_cal
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __LSC_CAL_H__
#define __LSC_CAL_H__

#include "knl_func_opt.h"

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if FUNC_KNL_LSCCAL
	#define LSC_CALIB_ENABLE 
#endif

//#define MI_LSC_CAL_OPTION

#define SP1K_ERR_SUCCESS                        0
#define SP1K_ERR_CALIBRATION_CFG                1

#define MI_CAL_TAG_LUMA 120

typedef enum{
	RAW_NONE,
	RAW_SAVE,	
	RAW_LOAD,
}	LSC_CAL_RAWMODE_e;

typedef enum{
	SP1K_CALIB_LSC_LUMA_LEVEL,
	//SP1K_CALIB_LSC_CENTER_ENABLE,
	//SP1K_CALIB_LSC_VFLIP_ENABLE,
	//SP1K_CALIB_LSC_HFLIP_ENABLE,
	//SP1K_CALIB_LSC_OUTPUT_NAME,
	//SP1K_CALIB_LSC_LOAD_RAW,
	//SP1K_CALIB_LSC_SAVE_RAW,
	//SP1K_CALIB_LSC_LOG_BUF,
	SP1K_CALIB_LSC_GR_AVG_HIGH,
	SP1K_CALIB_LSC_GR_AVG_LOW,
	SP1K_CALIB_LSC_STEP_X,
	SP1K_CALIB_LSC_STEP_Y,
	SP1K_CALIB_LSC_CHANNEL,
	SP1K_CALIB_LSC_RESOLUTION,
	
} LSC_CAL_CONFIG_e;


typedef enum{
	LSC_3CH,
	LSC_4CH,	
}	LSC_CAL_GAINRES_e;


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



#if 1
void lscTblGen (UINT32 rawAddr,UINT16 hSize,UINT16 vSize,UINT16 posX, UINT16 posY,UINT8 cfa,
				UINT16 *gbGain,UINT16 *bGain,UINT16 *grGain,UINT16 *rGain,UINT8 FalloffPer,
				UINT8 maxGbVal,UINT8 maxBVal,UINT8 maxGrVal,UINT8 maxRVal);
#else
void lscTblGen (UINT32 rawAddr,UINT16 imgWidth,UINT16 imgHeight,UINT16 xx, UINT16 yy,UINT8 sampleMethod,
					UINT16 *Gain,UINT8 FalloffPer,UINT8 Maxval);					
#endif

void lscMaxValGetbyFilter(UINT32 rawAddr,UINT16 hSize,UINT16 vSize,
						UINT8 cfa,UINT8 *maxR,UINT8 *maxGr,UINT8 *maxB,UINT8 *maxGb,UINT8 *warningM);

UINT8 lscCalibration(UINT32 rawAddr,UINT16 hSize,UINT16 vSize,UINT8 cfa);
UINT8 lsEq(float *xin, float *yin, UINT8 len, float C[3]);
void lscCalInit(void);
UINT8 sp1kLscCfgSet(UINT8 selector, UINT8 value);
//void lscRawFilterSumGet(UINT32 rawAddr,	UINT16 hSize,UINT16 posX,UINT16 posY,UINT16 fSize,UINT8 cfa,UINT16 *r,UINT16 *g,UINT16 *b);
void lscRawSumGet(UINT32 rawAddr,UINT16 hSize,UINT16 xStart,UINT16 yStart,UINT16 xEnd,UINT16 yEnd,	UINT8 cfa,UINT16 *r,UINT16 *gr,UINT16 *b,UINT16 *gb);

void lscTableShapeParCal(UINT16 hSize,UINT16 vSize,UINT8 per);
void lscTblFilter(UINT32 addr,UINT16 hSize,UINT16 vSize,UINT8 maskSize);
void lscTblFilterMaskDataGet(UINT32 addr,UINT8 xdata* pTbl,UINT16 hSize,UINT16 yStart,UINT16 yEnd);

UINT8 sp1kLscCal(void);


void appMILSCCalSet(void);
void appSetMILSCCalSts(UINT8 enable);
UINT8 appGetMILSCCalSts(void);


#endif  /* __TEMPLATE_HDR_H__ */
