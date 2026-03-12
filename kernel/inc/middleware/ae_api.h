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
#ifndef __AAA_API_H__
#define __AAA_API_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define AE_WAIT_TIME_MS				(UINT32)(1000)
#define AE_WAIT_TIME_10US			(UINT32)(100000)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _AEstr{
unsigned short ev;
unsigned long shutter;
unsigned short gain;
unsigned short iris;
}AEstr;

typedef struct _AECQ{
	unsigned long shutter;
	unsigned short gain;
}AECQ;

/* preview ae */
typedef struct _AEHeaderInformation{
	UINT8 aeProc;
	UINT8 stableCondition;
	UINT8 luma[2];           /* pre /current */
	UINT16 gId[2];           /* pre /current */
	UINT32 shutter[2];       /* pre /current */
	UINT16 gain[2];          /* pre /current */
	UINT8 Reserved[16];
}AEHeaderInformation;

/* capture ae */
typedef struct _CaptureHeaderInformation{
	UINT32 snapPclk;
	UINT16 lineTot;
	UINT16 frameTot;
	UINT16 gain;
	UINT32 espline;
	UINT32 compensationLine;	
	UINT8 Reserved[16];
}CaptureHeaderInformation;

/* Flash */
typedef struct _FlashHeaderInformation{
	UINT32 preFlashShutter;
	UINT16 preFlashGain;
	UINT16 preFlashTime;
	UINT32 mainFlashShutter;
	UINT16 mainFlashGain;
	UINT16 mainFlashTime;
	UINT16 beforePreFlashLuma;
	UINT16 afterPreFlashLuma;
	UINT8 previewToCaptureRate;
	UINT8 Reserved[16];
}FlashHeaderInformation;

/* Bshutter */
typedef struct _BShutterHeaderInformation{
	UINT8 Reserved[20];
}BShutterHeaderInformation;

/* Mshutter */
typedef struct _MShutterHeaderInformation{
	UINT8 Reserved[20];
}MShutterHeaderInformation;

/* CaptureHeaderInformation */
typedef struct _CaptureHeaderAeInformation{
	UINT8 id[4];
	AEHeaderInformation ae;
	FlashHeaderInformation flash;
	BShutterHeaderInformation bShutter;
	MShutterHeaderInformation mShutter;
	CaptureHeaderInformation capture;
	UINT8 Reserved[16];
}CaptureHeaderAeInformation;

typedef enum {
	HEADER_INFORMATION_AE_CONFIG = 0,
	HEADER_INFORMATION_FLASH_CONFIG,
	HEADER_INFORMATION_BSHUTTER_CONFIG,
	HEADER_INFORMATION_MSHUTTER_CONFIG,
	HEADER_INFORMATION_CAPTURE_CONFIG,
}CAPTURE_HEADER_AE_INFORMATION_CONFIG;

typedef enum {
	AE_DISABLE = 0,
	AE_ENABLE
}AE_EN_MODE;


typedef enum {
	AE_WIN_MODE_Y = 0,
	AE_WIN_MODE_RGB
}AE_WIN_MODE;

typedef enum {
	AE_SET_FINISH = 0, 
	AE_SET_START
}AE_SETTING_STATUS;

typedef enum {
	AE_EXP_GAIN_NOT_ACTION_FOR_YUV = 0,
	AE_EXP_GAIN_NOT_ACTION_FOR_RGB,
	AE_EXP_GAIN_SET_POS_VD_RISING,
	AE_EXP_GAIN_SET_POS_VVALID_RISING,
	//AE_EXP_GAIN_SET_POS_VVALID_FALLING
}AE_EXP_GAIN_SET_POS;

typedef enum {
	AE_GAIN_AFTER_EXP_OFF = 0,
	AE_GAIN_AFTER_EXP_ON
}AE_GAIN_AFTER_EXP;

typedef enum {
	AE_NOT_READY = 0,
	AE_READY
}AE_READY_STATUS;

typedef enum {
	LUMA_NORMAL = 0,
	LUMA_CAL_COLOR_CHECKER,
	LUMA_CAL_LSC
}CALCULATE_LUMA_MODE;

typedef enum {
	AE_STATUS_SET_GAIN = 0,
	AE_STATUS_EFFECT_FRAME,
	AE_STATUS_PROC_TRIGGER,
	AE_STATUS_SET_EXPOSURE,
}AE_INTR_FLOW_STATUS;

typedef enum {
	AE_TABLE_AE = 0,
	AE_TABLE_GAIN,
}AE_TABLE_SELECT;

/* only support II Ae */
typedef enum {
	AE_PCCAM_320_240 = 0,
	AE_PCCAM_160_120,
	AE_PCCAM_SIZE_MAX,
}AE_PCCAM_SRC_SIZE_MODE;

typedef enum {
	SP1K_AE_TARGET_LUMA = 0,/*< AE target Luma value, defult is 120.*/
	SP1K_AE_EV_COMP,/*< Target Luma value, defult is 120.*/
	SP1K_AE_FRAME_RATE,/*< Target Luma value, defult is 120.*/
	SP1K_AE_METERING,/*< Target Luma value, defult is 120.*/
	SP1K_AE_USING_AWB_GAIN,/*< using AWB GAIN for increasing dynamic range*/
	SP1K_AE_CONVERAGE_ZONE,/*< Target Luma value, defult is 120.*/
	SP1K_AE_GAINSHUTTERTBL,/*< Target Luma value, defult is 120.*/
	//SP1K_AW_H_WINDOW_SIZE,/*< Target Luma value, defult is 120.*/
	//SP1K_AW_V_WINDOW_SIZE,/*< Target Luma value, defult is 120.*/
	SP1K_AE_LowLight,/*< The status of AE which means light too low.*/
	SP1K_AE_IsReady,/*< The status of AE which means AE is finished or not, 0 is not ready and 1 is ready.*/
	SP1K_AE_EV_GET,/*< Get the current EV step */
	SP1K_AE_Count,/*< The index of AE which means how many time does AE have been calculate.*/
	SP1K_AE_G_GAIN,/*< The value of AE which current setting of G_GAIN table index.*/
	SP1K_AE_gId,/*< The value of AE which current setting of ae table index .*/
	SP1K_AE_G_AETblSel,/*< The value of AE which current setting of ae table freq select.*/
	SP1K_AE_G_CaptureCompensationGAIN,/*< The value of AE which current setting of G_CaptureCompensationGAIN value.*/
	SP1K_AE_FRAMERATE,/*< The value of AE which current fps information of sensor.*/
	SP1K_AE_CAPTURE_ISO_MODE,/*< The value of AE which current setting of capture iso mode.*/
	SP1K_AE_CURRENT_LUMA,/*< The value of AE which current preview luma. */
	SP1K_AE_CAPTURE_ISO_VALUE,/*< The value of AE which current setting of capture iso value.*/
	SP1K_AE_CAPTURE_ANTISHAKE_FPS,/*< The value of AE which current capture limit fps of shutter time. */
	SP1K_AE_MAX_COVER_ZONE,/*< The value of AE which current ae luma of max cover zone. */
	SP1K_AE_MIN_COVER_ZONE,/*< The value of AE which current ae luma of min cover zone. */
	SP1K_AE_SPEED,/*< The value of AE which current ae speed value. */
}SP1K_AE_PARAMETER_IDX;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8	G_AEEN;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 aeInitial(void);
void aeExposureGainSet(UINT8 vdCount);
UINT8 aeGetCount(void);
UINT8 aeMeterChangeSet(UINT8 mode);
UINT8 aeMeteringModeGet(void);
UINT8 aeStdLumiSet(UINT8 lumi);
UINT8 aeExpCompSet(UINT8 light);
SINT8 aeExpCompGet(void);
SINT8 aeExpCompIdxGet(void);
UINT8 aeFrameRateChangeSet(UINT16 framerate);
void aeForceSet(void);
void aeFrmInvalidSet(void);
void aeModeEnableSet(UINT8 enable);
UINT8 aeModeEnableGet(void);
void aeWinModeSet(UINT8 aeMode);
void aeGlobalGainEnableSet(UINT8 GlbGnEn);
void aeWinConfigSet(UINT16 hSize, UINT16 vSize, UINT16 aeDispFactor);
void aeTblSelSet(UINT8 AETblSel);
void aeInfoShow(void);
void aeCalculateLumaModeSet(UINT8);
UINT8 aeCalculateLumaModeGet(void);
void aeNormalizationFactorSet(UINT8 winMode);
void aeNegativeEnableSet(UINT8 NegativeEn);
UINT8 aeCalibrationCurLumaGet(void);
void aeIntrCurGainValueGet(UINT8 *val);
UINT8 aeIntrGIdGet(void);
void aeConfigForIQTunning(UINT16 idx,UINT8 opt);
UINT8 aeReadyGet(UINT8 mode);
void aeProcReset(void);
void aeParamSet(UINT8 selector,UINT16 val);
UINT8 aeStatusGet(UINT8 selector, UINT8 *val);
void aeFdAeModeSet(UINT8 enable);
void aeGidStepSet(UINT8 luma);
UINT8 aeFlowGetCurLuma(void);
UINT16 aeFlowGetCurLumaU16(void);

void aeSysDbgSet(void);
void aeCurrentTableCheck(UINT8 tableSelect);
void aeIntrDebug(void);
void aePccamSrcSizeSet(UINT8 mode);

void aeCallbackReg(UINT32 id, void *pfunct);

void aeDynamicMeterChangeSet(void);
void aeWindowDimensionGet(UINT8 * h,UINT8 * v);
void aeWindowArrayGet(UINT8 *pArray);
void aeFrameIntervalSet(UINT8 en);
void aeCaptureHeaderAeInformationSet(UINT8 cfg,	void *point);


#endif //__AAA_API_H__

