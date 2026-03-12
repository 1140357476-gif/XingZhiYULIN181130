
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
#ifndef __SP1K_AE_API_H__
#define __SP1K_AE_API_H__

/** \file sp1k_ae_api.h
 * @brief		Spca1628 AE related interface which include: a)Initial setting, b) Parameter setting, C) Mode setting, d) Status getting of AE/AWB.
 */
 
#include "ae_api.h"
#include "sp1k_ae_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP1K_AE_PARA_TARGET_LUMA = SP1K_AE_TARGET_LUMA,							/*< AE target Luma value, defult is 120.*/
	SP1K_AE_PARA_EV_COMP = SP1K_AE_EV_COMP,									/*< Target Luma value, defult is 120.*/
	SP1K_AE_PARA_FRAME_RATE = SP1K_AE_FRAME_RATE,							/*< Target Luma value, defult is 120.*/
	SP1K_AE_PARA_METERING = SP1K_AE_METERING,								/*< Target Luma value, defult is 120.*/
	SP1K_AE_PARA_GID = SP1K_AE_gId,											/*< The value of AE which current setting of ae table index .*/
	SP1K_AE_PARA_CAP_ISO_MODE = SP1K_AE_CAPTURE_ISO_MODE,					/*< The value of AE which current setting of capture iso mode >*/
	SP1K_AE_PARA_CAP_ISO_VALUE = SP1K_AE_CAPTURE_ISO_VALUE,					/*< The value of AE which current setting of capture iso value.*/
	SP1K_AE_PARA_CAP_ANTISHAKE_FPS = SP1K_AE_CAPTURE_ANTISHAKE_FPS,			/*< The value of AE which current capture limit fps of shutter time. */
	SP1K_AE_PARA_MAX_COVER_ZONE = SP1K_AE_MAX_COVER_ZONE,					/*< The value of AE which current ae luma of max cover zone. */
	SP1K_AE_PARA_MIN_COVER_ZONE = SP1K_AE_MIN_COVER_ZONE,					/*< The value of AE which current ae luma of min cover zone. */
	SP1K_AE_PARA_SPEED = SP1K_AE_SPEED,
}SP1K_PARAMETER_SET_MODE;

typedef enum {
	SP1K_AE_STS_LOW_LIGHT = SP1K_AE_LowLight,								/*< The status of AE which means light too low.*/
	SP1K_AE_STS_IS_READY = SP1K_AE_IsReady,									/*< The status of AE which means AE is finished or not, 0 is not ready and 1 is ready.*/
	SP1K_AE_STS_EV = SP1K_AE_EV_GET,										/*< Get the current EV step */
	SP1K_AE_STS_COUNT = SP1K_AE_Count,										/*< The index of AE which means how many time does AE have been calculate.*/
	SP1K_AE_STS_G_GAIN = SP1K_AE_G_GAIN,									/*< The value of AE which current setting of G_GAIN table index .*/
	SP1K_AE_STS_GID = SP1K_AE_gId,											/*< The value of AE which current setting of ae table index .*/
	SP1K_AE_STS_FREQ = SP1K_AE_G_AETblSel,									/*< The value of AE which current setting of ae table freq select .*/
	SP1K_AE_STS_CAP_COMPENSATION_GAIN = SP1K_AE_G_CaptureCompensationGAIN,	/*< The value of AE which current setting of G_CaptureCompensationGAIN value .*/
	SP1K_AE_STS_FRAME_RATE = SP1K_AE_FRAMERATE,								/*< The value of AE which current fps information of sensor .*/
	SP1K_AE_STS_CAP_ISO_MODE = SP1K_AE_CAPTURE_ISO_MODE,					/*< The value of AE which current setting of capture iso mode >*/
	SP1K_AE_STS_CURRENT_LUMA = SP1K_AE_CURRENT_LUMA,						/*< The value of AE which current preview luma. */
	SP1K_AE_STS_CAP_ISO_VALUE = SP1K_AE_CAPTURE_ISO_VALUE,					/*< The value of AE which current setting of capture iso value.*/
	SP1K_AE_STS_CAP_ANTISHAKE_FPS = SP1K_AE_CAPTURE_ANTISHAKE_FPS,			/*< The value of AE which current capture limit fps of shutter time. */
}SP1K_STATUS_GET_MODE;

typedef enum {
	SP1K_FRAME_RATE_60      = 0,		/*< The selector of sensor driver 60fps setting and 60hz ae table mode.*/
	SP1K_FRAME_RATE_50      = 1,		/*< The selector of sensor driver 60fps setting and 50hz ae table mode.*/
	SP1K_FRAME_RATE_30      = 2,		/*< The selector of sensor driver 30fps setting and 60hz ae table mode.*/
	SP1K_FRAME_RATE_25      = 3,		/*< The selector of sensor driver 30fps setting and 50hz ae table mode.*/
	SP1K_FRAME_RATE_24      = 4,		/*< The selector of sensor driver 24fps setting and 60hz ae table mode.*/
	SP1K_FRAME_RATE_20      = 5,		/*< The selector of sensor driver 24fps setting and 50hz ae table mode.*/
	SP1K_FRAME_RATE_15      = 6,		/*< The selector of sensor driver 15fps setting and 60hz ae table mode.*/
	SP1K_FRAME_RATE_12P5    = 7,		/*< The selector of sensor driver 15fps setting and 50hz ae table mode.*/
	SP1K_FRAME_RATE_SP_60HZ	= 252,		/*< The selector of sensor driver still preview Xfps setting and 60hz ae table mode.*/
	SP1K_FRAME_RATE_SP_50HZ	= 253,		/*< The selector of sensor driver still preview Xfps setting and 50hz ae table mode.*/
	SP1K_FRAME_RATE_TOT,
}SP1K_FRAME_RATE_MODE;

typedef enum {
	SP1K_FLICKER_60 = 60,		/*< The selector of flicker under 60Hz light source.*/
	SP1K_FLICKER_50 = 50,		/*< The selector of flicker under 50Hz light source.*/
}SP1K_FLICKER_MODE;

typedef enum {
	SP1K_AE_METER_MULTI=0,		/*< AE detect window which have the widest range within the view.*/
	SP1K_AE_METER_CENTER,		/*< AE detect window which have the normal range at the center of view.*/
	SP1K_AE_METER_SPOT,			/*< AE detect window which only have a narrow range at the center of view.*/
	SP1K_AE_METER_USER_DEFINE,	/*< AE detect window which user defined range of view.*/
}SP1K_AE_METER_MODE;

typedef enum {
	SP1K_AE_EXP_COMP_M20 = 0,		/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M19,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M18,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M17,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M16,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M15,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M14,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M13,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M12,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M11,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M10,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M09,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M08,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M07,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M06,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M05,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M04,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M03,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M02,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_M01,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_00,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P01,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P02,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P03,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P04,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P05,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P06,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P07,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P08,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P09,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P10,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P11,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P12,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P13,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P14,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P15,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P16,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P17,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P18,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P19,			/*< Target Luma value, defult is 120.*/
	SP1K_AE_EXP_COMP_P20			/*< Target Luma value, defult is 120.*/
}SP1K_AE_EXP_COMP_MODE;

typedef enum {
	SP1K_AE_ISO_AUTO = 0,		/*< ISO AUTO.*/
	SP1K_AE_ISO_100,			/*< ISO 100.*/
	SP1K_AE_ISO_200,			/*< ISO 200.*/
	SP1K_AE_ISO_400,			/*< ISO 400.*/
	SP1K_AE_ISO_800,			/*< ISO 800.*/
	SP1K_AE_ISO_MAX,
}SP1K_AE_ISO_MODE;

typedef enum {
	SP1K_AE_JUMP_STEP_NORMAL = 0,
	SP1K_AE_JUMP_STEP_DIV2,
	SP1K_AE_JUMP_STEP_DIV3,
	SP1K_AE_JUMP_STEP_DIV4,
}SP1K_AE_JUMP_STEP_SPEED;


typedef enum {
	SP1K_AE_CALLBACK_USER_DEFINE_METER = 0,		/*<user ae metering callback index.*/
	//SP1K_AE_CALLBACK_WINDOW_INFORMATION,
	SP1K_AE_CALLBACK_MAX
} SP1K_AE_CALLBACK_IDX;

typedef enum {
	SP1K_HEADER_INFORMATION_AE_CONFIG = HEADER_INFORMATION_AE_CONFIG,               /* Preview Ae Debug Information (system auto added) */
	SP1K_HEADER_INFORMATION_FLASH_CONFIG = HEADER_INFORMATION_FLASH_CONFIG,         /* Capture FLASH Debug Information (sensor driver need added) */
	SP1K_HEADER_INFORMATION_BSHUTTER_CONFIG = HEADER_INFORMATION_BSHUTTER_CONFIG,   /* Capture B-SHUTTER Debug Information (sensor driver need added) */
	SP1K_HEADER_INFORMATION_MSHUTTER_CONFIG = HEADER_INFORMATION_MSHUTTER_CONFIG,   /* Capture M-SHUTTER Debug Information (sensor driver need added) */
	SP1K_HEADER_INFORMATION_CAPTURE_CONFIG = HEADER_INFORMATION_CAPTURE_CONFIG,     /* Capture AGC,AEC Debug Information (sensor driver need added) */
}SP1K_CAPTURE_HEADER_AE_INFORMATION_CONFIG;

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
 * @fn                UINT8 sp1kAeInit(void)
 * @brief             	Initial AE setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            SUCCESS
 */
UINT8 sp1kAeInit(void);

/**
 * @fn                UINT8 sp1kAeGetModeEn(void);
 * @brief	            To Get AE On/off Status function.
 * @param[in]      none
 * @retval                      The status of AE:
 *\n                              AE_DISABLE: AE turn OFF.
 *\n                              AE_ENABLE: AE turn ON.
 */
UINT8 sp1kAeGetModeEn(void);

/**
 * @fn void sp1kAeSetModeEn(UINT8 mode)
 * @brief	To turn ON/OFF AE function.
 * @param[in] mode	 The status of AE:
 *\n                              AE_DISABLE: AE turn OFF.
 *\n                              AE_ENABLE: AE turn ON.
 * @retval none
 */
void sp1kAeSetModeEn(UINT8 enable);

/**
 * @fn void sp1kAeParamSet(UINT8 selector, UINT16 val)
 * @brief	Entering parameter for AE setting
 * @param[in] selector	 To set the parameter of AE:
 *\n                              SP1K_AE_PARA_TARGET_LUMA
 *\n                              SP1K_AE_PARA_EV_COMP
 *\n                              SP1K_AE_PARA_FRAME_RATE
 *\n                              SP1K_AE_PARA_METERING
 *\n	                             SP1K_AE_PARA_GID
 *\n                              SP1K_AE_PARA_CAP_ISO_MODE
 *\n                              SP1K_AE_PARA_CAP_ISO_VALUE
 *\n                              SP1K_AE_PARA_CAP_ANTISHAKE_FPS
 *\n                              SP1K_AE_PARA_MIN_COVER_ZONE
 *\n                              SP1K_AE_PARA_MAX_COVER_ZONE
 *\n                              SP1K_AE_PARA_SPEED
 * @param[in] val	 Value of each setting above.
 *\n                              0~255 for selector SP1K_AE_TARGET_LUMA.
 *\n                              SP1K_AE_EXP_COMP_M20 ~ SP1K_AE_EXP_COMP_00 ~ SP1K_AE_EXP_COMP_P20 for selector SP1K_AE_EV_COMP.
 *\n                              SP1K_FRAME_RATE_SP_60HZ, SP1K_FRAME_RATE_SP_50HZ, SP1K_FRAME_RATE_60, SP1K_FRAME_RATE_50, SP1K_FRAME_RATE_30, SP1K_FRAME_RATE_25, SP1K_FRAME_RATE_15, SP1K_FRAME_RATE_12P5 for selector SP1K_AE_FRAME_RATE.
 *\n                              SP1K_AE_METER_MULTI, SP1K_AE_METER_CENTER, SP1K_AE_METER_SPOT for selector SP1K_AE_TARGET_LUMA.
 *\n                              Set Current Ae gId for selector SP1K_AE_gId
 *\n                              SP1K_AE_ISO_AUTO, SP1K_AE_ISO_100, SP1K_AE_ISO_200, SP1K_AE_ISO_400, SP1K_AE_ISO_800
 *\n                              SP1K_AE_ISO_100, SP1K_AE_ISO_200, SP1K_AE_ISO_400, SP1K_AE_ISO_800
 *\n                              Set Limit Frame Rate Number (shutter time based on fps)
 *\n                              Set ae luma min cover zone
 *\n                              Set ae luma max cover zone
 *\n                              Set ae speed, SP1K_AE_JUMP_STEP_NORMAL, SP1K_AE_JUMP_STEP_DIV2, SP1K_AE_JUMP_STEP_DIV3, SP1K_AE_JUMP_STEP_DIV4,
 * @retval none
 */
void sp1kAeParamSet(UINT8 selector, UINT16 val);

/**
 * @fn UINT8 sp1kAeStatusGet(UINT8 selector, UINT8* val)
 * @brief	To get the calculate status of AE.
 * @param[in] selector	 AE status:
 *\n                              SP1K_AE_STS_IS_READY - Means AE ready or not.
 *\n                              SP1K_AE_STS_LOW_LIGHT - Means AE lowlight or not.
 *\n                              SP1K_AE_STS_EV - Current EV value
 *\n                              SP1K_AE_STS_COUNT - The time that AE have been calculate.
 *\n                              SP1K_AE_STS_G_GAIN - Get Current GAIN Table Index
 *\n                              SP1K_AE_STS_GID - Get Current AE gId
 *\n                              SP1K_AE_STS_FREQ - Get Current AE Table Freq.
 *\n                              SP1K_AE_STS_CAP_COMPENSATION_GAIN - Get Preview to Snapshot Capture Compensation GAIN Index
 *\n                              SP1K_AE_STS_FRAME_RATE - Get Current fps information of sensor
 *\n                              SP1K_AE_STS_CAP_ISO_MODE - Current Capture ISO Mode
 *\n                              SP1K_AE_STS_CURRENT_LUMA - Current Luma
 *\n                              SP1K_AE_STS_CAP_ISO_VALUE - Current Capture ISO Value
 *\n                              SP1K_AE_STS_CAP_ANTISHAKE_FPS - Current anti-shake limit fps (shutter time based on fps)
 * @param[out] val	 AE status value:
 *\n                              0: Not ready, 1: Ready for selector SP1K_AE_IsReady.
 *\n                              0: Normal light, 1: Low light for selector SP1K_AE_LowLight.
 *\n                              0~255 for current EV value
 *\n                              0~255 for selector SP1K_AE_Count.
 *\n                              Current G_GAIN Index for selector SP1K_AE_G_GAIN.
 *\n                              Current gId for selector SP1K_AE_gId
 *\n                              SP1K_FLICKER_50: 50Hz, SP1K_FLICKER_60: 60Hz for selector SP1K_AE_G_AETblSel
 *\n                              Capture Compensation GAIN Index for selector SP1K_AE_G_CaptureCompensationGAIN
 *\n                              Current front frame rate
 *\n                              Current capture ISO mode
 *\n                              Current luma value
 *\n                              Current Capture ISO Value
 *\n                              Current anti-shake limit fps
 * @retval error code -  SUCCESS: 0, FAIL: 1
 */
UINT8 sp1kAeStatusGet(UINT8 selector, UINT8* val);

/**
 * @fn UINT8 sp1kAeReadyGet(UINT8 mode)
 * @brief	To get the calculate status of AE.
 * @param[in] mode:
 *\n                              Bit0:waitAe
 *\n                              Bit1:waitAwb
 *\n                              Bit2:DarkWait (when low light)
 * @retval error code -  SUCCESS: 0, FAIL: 1
 */
UINT8 sp1kAeReadyGet(UINT8 mode);

/**
 * @fn void sp1kFdAeSetModeEn(UINT8 enable)
 * @brief	To turn ON/OFF FD AE function.
 * @param[in] mode	 The status of AE:
 *\n                              AE_DISABLE: FD AE turn OFF.
 *\n                              AE_ENABLE: FD AE turn ON.
 * @retval none
 */
void sp1kFdAeSetModeEn(	UINT8 enable);

/**
 * @fn void sp1kAeWinConfig(void)
 * @brief	based on current front in size to set AE windows config function.
 * @retval none
 */
void sp1kAeWinConfig(void);

/**
 * @fn		  void sp1kAeShowInfo(void)
 * @brief	        AE show gId, G_GAIN info
 * @param	  NONE
 * @retval none
*/
void sp1kAeShowInfo(void);

/**
 * @fn void sp1kAeNegativeEnSet(UINT8 NegativeEn);
 * @brief	      Set Negative Enable (only for II AE used)
 * @param[in] NegativeEn		Negative mode en
 *\n                                       0:Negative mode disable
 *\n                                       1: Negative mode enable
 * @retval none
 */
void sp1kAeNegativeEnSet(UINT8 NegativeEn);

/**
 * @fn void sp1kAeIsoSet(UINT8 iso)
 * @brief	      Set ISO Mode
 * @param[in] iso				ISO mode selection :
 *\n                                       SP1K_AE_ISO_AUTO:AUTO
 *\n                                       SP1K_AE_ISO_100:ISO100
 *\n                                       SP1K_AE_ISO_200:ISO200
 *\n                                       SP1K_AE_ISO_400:ISO400
 *\n                                       SP1K_AE_ISO_800:ISO800
 * @retval none
 */
void sp1kAeIsoSet(UINT8 iso);

/**
 * @fn		  void aeDynamicMeterChangeSet(void)
 * @brief          Dynamic Meter Change set when ae process flow running (auto sync with ae flow)
 * @param	  NONE
 * @retval	  
*/
void sp1kAeDynamicMeterChangeSet(void);

/**
 * @fn		  void aeWindowArrayGet(void)
 * @brief          get current ae window luma array
 * @param	  NONE
 * @retval	  
*/
void sp1kAeWindowArrayGet(UINT8 *pArray);

/**
 * @fn		  void aeWindowDimensionGet(void)
 * @brief          get ae window array dimension h size, v size
 * @param	  NONE
 * @retval	  
*/
void sp1kAeWindowDimensionGet(UINT8 *h, UINT8 *v);

/**
 * @fn          void sp1kAeCallbackReg(UINT32 id, void *pfunct)
 * @brief       register callback of ae
 * @param    [in] id : index of callback, type enum SP1K_AE_CALLBACK_IDX
 *\n                                     SP1K_AE_CALLBACK_USER_DEFINE_METER
 * @param    [in] pfunct : function point you wanna register
 * @retval    NONE
*/
void sp1kAeCallbackReg(UINT32 id, void *pfunct);

/**
 * @fn void sp1kAeAntiShakeFpsSet(UINT8 fps)
 * @brief	      Set Anti-shake Shutter Time (unit:fps)
 * @param[in] fps				Shutter Time based on frame rate
 * @retval none
 */
void sp1kAeAntiShakeFpsSet(UINT8 fps);

/**
 * @fn void sp1kAeCaptureHeaderInformationSet(UINT8 cfg,	void *point)
 * @brief	      Set Capture Ae Debug Information to Jpeg Header Set
 * @param[in] cfg		typedef enum SP1K_CAPTURE_HEADER_AE_INFORMATION_CONFIG
 * @param[in] *point	struct point AEHeaderInformation,
 *                                                   FlashHeaderInformation,
 *                                                   BShutterHeaderInformation,
 *                                                   MShutterHeaderInformation,
 *                                                   CaptureHeaderInformation
 * @retval none
 */
void sp1kAeCaptureHeaderInformationSet(UINT8 cfg, void *point);


#else

#define sp1kAeInit aeInitial
#define sp1kAeGetModeEn	aeModeEnableGet
#define sp1kAeSetModeEn	aeModeEnableSet
#define sp1kAeParamSet aeParamSet
#define sp1kAeStatusGet aeStatusGet
#define sp1kAeReadyGet aeReadyGet
#define sp1kFdAeSetModeEn aeFdAeModeSet
#define sp1kAeWinConfig	aeWinConfigSet
#define sp1kAeShowInfo aeInfoShow
#define sp1kAeNegativeEnSet aeNegativeEnableSet
#define sp1kAeIsoSet(iso) aeParamSet(SP1K_AE_PARA_CAP_ISO_MODE,iso);
#define sp1kAeDynamicMeterChangeSet aeDynamicMeterChangeSet
#define sp1kAeWindowArrayGet aeWindowArrayGet
#define sp1kAeWindowDimensionGet aeWindowDimensionGet
#define sp1kAeCallbackReg aeCallbackReg
#define sp1kAeAntiShakeFpsSet(fps) aeParamSet(SP1K_AE_PARA_CAP_ANTISHAKE_FPS, fps);
#define sp1kAeCaptureHeaderInformationSet aeCaptureHeaderAeInformationSet

#endif

#endif //__SP1K_AE_API_H__

