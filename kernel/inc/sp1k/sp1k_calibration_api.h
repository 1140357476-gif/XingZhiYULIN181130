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
#ifndef __SP1K_CALIBRATION_API_H__
#define __SP1K_CALIBRATION_API_H__

/** \file sp1k_calibration_api.h
 * @brief		Spca1628 calibration mode related interface
 */

#include "sp1k_bp_api.h"
#include "sp1k_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SP1K_CALIBRATION_CRITERIA_FILE_ID 0x30
#define SP1K_CALIBRATION_OB_FILE_ID 0x31
#define SP1K_CALIBRATION_AWB_FILE_ID 0x32
#define SP1K_CALIBRATION_IQ_FILE_ID 0x33

#define USE_RGB_WIN 1    //if USE_RGB_WIN = 1 ,can't save the pv RAW
#define OB_CAL_EN 0

typedef enum {
	SP1K_CALIBRATION_OB_R = 0,
	SP1K_CALIBRATION_OB_Gr,
	SP1K_CALIBRATION_OB_B,
	SP1K_CALIBRATION_OB_Gb,
	SP1K_CALIBRATION_OB_MAX
} calibrationOb_e;

typedef enum {
	SP1K_CALIBRATION_AWB_R = 0,
	SP1K_CALIBRATION_AWB_Gr,
	SP1K_CALIBRATION_AWB_B,
	SP1K_CALIBRATION_AWB_Gb,
	SP1K_CALIBRATION_AWB_MAX
} calibrationAwb_e;

typedef enum {
	SP1K_CALIBRATION_PV_OB_R = 0,
	SP1K_CALIBRATION_PV_OB_Gr,
	SP1K_CALIBRATION_PV_OB_B,
	SP1K_CALIBRATION_PV_OB_Gb,
	SP1K_CALIBRATION_PV_OB_MAX
} calibrationPvOb_e;

typedef enum {
	SP1K_CALIBRATION_CRITERIA_OB_LOW_THD = 0,
	SP1K_CALIBRATION_CRITERIA_OB_HIGH_THD,
	SP1K_CALIBRATION_CRITERIA_AE_GID0,
	SP1K_CALIBRATION_CRITERIA_AE_GID1,
	SP1K_CALIBRATION_CRITERIA_AE_GID2,
	SP1K_CALIBRATION_CRITERIA_AE_GID3,
	SP1K_CALIBRATION_CRITERIA_MAX
} calibrationCriteria_e;

typedef enum {
	SP1K_CALIBRATION_MODE_OFF = 0,
	SP1K_CALIBRATION_MODE_STILL_RAW,
	SP1K_CALIBRATION_MODE_AE_TEST,
	SP1K_CALIBRATION_MODE_PV_RAW,
	SP1K_CALIBRATION_MODE_OB,
	SP1K_CALIBRATION_MODE_AWB,
	SP1K_CALIBRATION_MODE_PV_OB,
	SP1K_CALIBRATION_MODE_MAX
} calibrationMode_e;

typedef enum {
	SP1K_CAL_PV_OB_GAIN_INDEX = 0,
	SP1K_CAL_PV_OB_INIT_EXP,
	SP1K_CAL_PV_OB_LINE_TOL,
	SP1K_CAL_PV_OB_PCLK,
	SP1K_CAL_PV_OB_SAVE_RAW,
	SP1K_CAL_PV_OB_MULTI_CAL,
	SP1K_CAL_PV_OB_ONE_POINT_CAL,
	SP1K_CAL_PV_OB_BAD_PIXEL_THR,
	SP1K_CAL_PV_OB_MAX,
} calPvOBPara_e;


typedef struct calPvOBData_s{
	float rAvg;
	float grAvg;
	float bAvg;
	float gbAvg;
	UINT16 expLine;
}calPvOBData_t;

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
 #ifdef SP1K_DOXY_GEN
 /**
 * @fn void sp1kCalibrationModeSet( UINT8 mode )
 * @brief	 Set calibration mode
 * @param[in] mode:
 	SP1K_CALIBRATION_MODE_OFF = 0,
	SP1K_CALIBRATION_MODE_STILL_RAW,
	SP1K_CALIBRATION_MODE_AE_TEST,
	SP1K_CALIBRATION_MODE_PV_RAW,
	SP1K_CALIBRATION_MODE_OB,
	SP1K_CALIBRATION_MODE_AWB,
	SP1K_CALIBRATION_MODE_PV_OB,
	SP1K_CALIBRATION_MODE_MAX
 * @retval none
 */
void sp1kCalibrationModeSet( UINT8 mode );
/**
 * @fn void sp1kCalibrationObSet( UINT8 enable )
 * @brief	Calibration ob(optical black) enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */
void sp1kCalibrationObSet( UINT8 enable );
/**
 * @fn void sp1kOsdLayoutAttrGet(UINT8* hNum, UINT8* vNum)
 * @brief	calbration gamma enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */

void sp1kCalibrationGammaSet( UINT8 enable );
/**
 * @fn void sp1kCalibrationLscSet( UINT8 enable )
 * @brief	calbration lens shading enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */

void sp1kCalibrationLscSet( UINT8 enable );
/**
 * @fn void sp1kCalibrationWDRSet(UINT8 enable)
 * @brief	calbration WDR enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */

void sp1kCalibrationWDRSet(UINT8 enable);
/**
 * @fn void sp1kCalibrationAESet( UINT8 enable )
 * @brief	calbration AE enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */

void sp1kCalibrationAESet( UINT8 enable );
/**
 * @fn void sp1kCalibrationAWBSet( UINT8 enable )
 * @brief	calbration AWB enable
 * @param[in] enable	1:enable 0:disable
 * @retval none
 */

void sp1kCalibrationAWBSet( UINT8 enable );
/**
 * @fn void sp1kCalibrationAEConfig( UINT16 idx, UINT8 opt )
 * @brief	AE config(shutter,gain)
 * @param[in] idx	ae table index
 * @param[in] opt	50:50hz,60:60hz
 * @retval none
 */
void sp1kCalibrationAEConfig( UINT16 idx, UINT8 opt );
/**
 * @fn UINT8 sp1kCalibrationRawCal( UINT32 addr, UINT16 hSize, UINT16 vSize )
 * @brief	get average of every chanel
 * @param[in] addr	addr of raw data
 * @param[in] hsize	hsize of raw data
 * @param[in] vise		vsize of raw data
 * @retval 0:success 1:fail
 */

UINT8 sp1kCalibrationRawCal( UINT32 addr, UINT16 hSize, UINT16 vSize );
/**
 * @fn void sp1kCalibrationOBDataGet(UINT8 *ob)
 * @brief Get ob data
 * @param[in] *ob :R,Gr,B,Gb
 * @retval none
 */

void sp1kCalibrationOBDataGet(UINT8 *ob);
/**
 * @fn UINT8 sp1kCalibrationPvRaw( void )
 * @brief	Set pv calibration parameter
 * @param none
 * @retval 0:success 1:fail
 */


UINT8 sp1kCalibrationPvRaw( void );
/**
 * @fn void sp1kCalibrationAETest( UINT16 fromIdx, UINT16 toIdx, UINT8 opt )
 * @brief	Set AE vlaue from fronidx to toidx
 * @param[in] fromIdx	start of index
 * @param[in] toIdx	end of index
 * @param[in] opt		50:50Hz ,60: 60Hz
 * @retval none
 */

void sp1kCalibrationAETest( UINT16 fromIdx, UINT16 toIdx, UINT8 opt );
/**
 * @fn UINT8 sp1kCalibrationLoadCriteria( UINT8 id )
 * @brief load criteria resource for AE test
 * @param[out] *hNum numer of fonts in X direction.
 * @retval none
 */

UINT8 sp1kCalibrationLoadCriteria( UINT8 id );
/**
 * @fn UINT8 sp1kCalibrationAeTestGidStepGet( UINT8 *idx0, UINT8 *idx1, UINT8 *idx2, UINT8 *idx3 )
 * @brief	Get step of gid
 * @param[in] *idx0 AE gid0
 * @param[in] *idx1 AE gid1
 * @param[in] *idx2 AE gid2
 * @param[in] *idx3 AE gid3
 * @retval none
 */

UINT8 sp1kCalibrationAeTestGidStepGet( UINT8 *idx0, UINT8 *idx1, UINT8 *idx2, UINT8 *idx3 );
/**
 * @fn void sp1kCalibrationAETestSave( UINT16 fromIdx, UINT16 toIdx )
 * @brief Save value of testing ae
 * @param[in] fronIdx	start index
 * @param[in]	toIdx	end index
 * @retval none
 */

void sp1kCalibrationAETestSave( UINT16 fromIdx, UINT16 toIdx );
/**
 * @fn UINT8 sp1kCalibrationLoadIq( UINT8 id )
 * @brief	Load iq resouce
 * @param[in] id:index of iq
 * @retval 0:success 1:fail
 */

UINT8 sp1kCalibrationLoadIq( UINT8 id );
/**
 * @fn UINT8 sp1kCalibrationSaveIq( UINT8 id )
 * @brief	Save iq data
 * @param[in] id:index of iq
 * @retval 0:success 1:fail
 */

UINT8 sp1kCalibrationSaveIq( UINT8 id );
/**
 * @fn void sp1kCalibrationDefaultSizeSet( void )
 * @brief	Set size of snap
 * @param none
 * @retval none
 */

void sp1kCalibrationDefaultSizeSet( void );

/**
 * @fn UINT8 sp1kCalibrationPvYuv( void )
 * @brief	Save current Pv YUV data
 * @param none
 * @retval 0:success 1:fail
 */

UINT8 sp1kCalibrationPvYuv( void );
/**
 * @fn UINT8 sp1kCalBpProc(UINT8 mode)
 * @brief	Snap raw data && find bad piexl
 * @param[in] mode :
 	BP_MODE_PRV_HD = 0, // preview mode/
	BP_MODE_PRV_VGA,// preview mode/
	BP_MODE_CAP,  /* capture mode /
 * @retval none
 */

UINT8 sp1kCalBpProc(UINT8 mode);
/**
 * @fn UINT8 Sp1kAeCalibration(void)
 * @brief	Set AE parameter of calibration
 * @param none
 * @retval 0:success 1:fail
 */

UINT8 Sp1kAeCalibration(void);
/**
 * @fn UINT8 Sp1kCalibrationEvParamInit(void)
 * @brief	EV parameter init
 * @param none
 * @retval 0:success 1:fail
 */

UINT8 Sp1kCalibrationEvParamInit(void);
/**
 * @fn void sp1kOsdLayoutAttrGet(UINT8* hNum, UINT8* vNum)
 * @brief	Get EV value
 * @param none
 * @retval 0:success 1:fail
 */
UINT8 Sp1kCalibrationEvValueGet(void);
#else
void sp1kCalibrationModeSet( UINT8 mode );
void sp1kCalibrationObSet( UINT8 enable );
void sp1kCalibrationGammaSet( UINT8 enable );
void sp1kCalibrationLscSet( UINT8 enable );
void sp1kCalibrationWDRSet(UINT8 enable);
void sp1kCalibrationAESet( UINT8 enable );
void sp1kCalibrationAWBSet( UINT8 enable );

void sp1kCalibrationAEConfig( UINT16 idx, UINT8 opt );
UINT8 sp1kCalibrationRawCal( UINT32 addr, UINT16 hSize, UINT16 vSize );
void sp1kCalibrationOBDataGet(UINT8 *ob);

UINT8 sp1kCalibrationPvRaw( void );
void sp1kCalibrationAETest( UINT16 fromIdx, UINT16 toIdx, UINT8 opt );
UINT8 sp1kCalibrationLoadCriteria( UINT8 id );
UINT8 sp1kCalibrationAeTestGidStepGet( UINT8 *idx0, UINT8 *idx1, UINT8 *idx2, UINT8 *idx3 );
void sp1kCalibrationAETestSave( UINT16 fromIdx, UINT16 toIdx );
UINT8 sp1kCalibrationLoadIq( UINT8 id );
UINT8 sp1kCalibrationSaveIq( UINT8 id );
void sp1kCalibrationDefaultSizeSet( void );
UINT8 sp1kCalibrationPvYuv( void );
UINT8 sp1kCalBpProc(UINT8 mode);
UINT8 Sp1kAeCalibration(void);
UINT8 Sp1kCalibrationEvParamInit(void);
UINT8 Sp1kCalibrationEvValueGet(void);
/**************************************************************************
 *                   I N T E R N A L   U S E   O N L Y                    *
 **************************************************************************/
UINT8 calibrationRaw( UINT32 addr, UINT16 xSize, UINT16 ySize );
UINT8 calibrationPvRaw( void );
UINT8 calibrationPvYuv( void );
void calibrationObGammeLoad( void );
void sp1kCalibrationPrvRawModeSet(UINT8 mode);
void sp1kCalibrationPrvRawSave(UINT8 mode);
void sp1kCalibrationAEWinConfigSet(UINT8 mode);
#if OB_CAL_EN
void sp1kCalPvOBAEWinConfigSet(UINT8 mode);
void sp1kCalPvOBGainShutterSet(UINT16 gain,UINT32 shutter,UINT8 wait);
calPvOBData_t *sp1kCalPvOBAvgDataGet(void);
UINT8 sp1kCalPvOBResultGet(UINT8* result,UINT8 mode);
UINT8 sp1kCalPvOBLinearRegression(float *rgbData, UINT8 rows, float *a, float *b);
UINT8 sp1kCalPvOBResultWrite(UINT8* result , UINT16 resId , UINT16 resOff);
void sp1kCalPvOBParaGet(UINT8 infoId,UINT16 *pPara1,UINT16 *pPara2);
void sp1kCalPvOBParaSet(UINT8 infoId,UINT16 para);
void sp1kCalPvOBAEWinRGBValueGet(UINT8 mode);
#endif

#endif
#endif  /* __SP1K_CALIBRATION_API_H__ */

