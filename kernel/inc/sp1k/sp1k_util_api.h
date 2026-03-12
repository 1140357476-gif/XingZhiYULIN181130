/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
#ifndef __SP1K_UTILITY_H__
#define __SP1K_UTILITY_H__

/** \file sp1k_util_api.h
 * @brief		Spca1528 utility related interfaces (Timer/RT/SuspenResume/Photoframe/datestamp).
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "pv_photoframe.h"
#include "timer.h"
#include "sp1k_def.h"
#include "utility.h"
#include "hal_global.h"
#include "hal_spi.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn void sp1kPwrOffCfg(UINT8 enable, UINT32 delay)
 * @brief	configure power off process when battery low
 * @param none
 * @retval 1: the power off process is running
                0: the power off process is stopped
 */
void sp1kPwrOffCfg(UINT8 enable, UINT32 delay);

/**
 * @fn UINT8 sp1kPwrOffProcChk(void)
 * @brief	query the status of power off process
 * @param[in] enable	 enable/disable power off process
 * @param[in] delay	 the delay time of power off process. The time unit is 10ms,
                                    delay = 0 means power off immediately.
 * @retval none
 */
UINT8 sp1kPwrOffProcChk(void);

/**
 * @fn void sp1kTimeDelay1s(void)
 * @brief	Software timer delay (1S).
 * @param none
 * @retval none
 */
void sp1kTimeDelay1s(void);

/**
 * @fn void sp1kTimeDelay1ms(void)
 * @brief	Software timer delay (1ms).
 * @param none
 * @see sp1kTimeDelay1ms()
 * @retval none
 */
void sp1kTimeDelay1ms(void);

/**
 * @fn void sp1kTimer1Start(void)
 * @brief	Start to count timer1. The count value is defined by sp1kTimer1Config().
 * @param none
 * @see sp1kTimer1Config()
 * @retval none
 */
void sp1kTimer1Start(void);

/**
 * @fn void sp1kTimer1Stop(void)
 * @brief	Stop to count timer1.
 * @param none
 * @see sp1kTimer1Config()
 * @retval none
 */
void sp1kTimer1Stop(void);

/**
 * @fn void sp1kTimer1Config(UINT32 Period, pFuncTmr pFunc, UINT8* pFuncPara)
 * @brief	This service allows to register a call back function by timer1 interrupt.
 * @param[in] Period	 Timer1 period setting. Set to TH1,TL1. The time unit is ms
 * @param[in] pFunc      Specifies the call back function pointer
 * @param[in] pFuncPara	 Specify the parameter pass to the registered function.
*\n                                      That means you can't register a fucntion with more than one parameter, Using a struct pointer may be a good idea.
 * @retval none
 */
void sp1kTimer1Config(UINT32 Period, pFuncTmr pFunc, UINT8* pFuncPara);

void sp1kSfCallbackReg(UINT32 idx, void *pfunct);

#else
#ifndef K_SLASH
#define K_SLASH /
#endif
#define sp1kTimeDelay1s     sp1kHalWait(1000);/K_SLASH
#define sp1kTimeDelay1ms    sp1kHalWait(1);/K_SLASH

#define sp1kTimer1Start     TIMER1_Start
#define sp1kTimer1Stop      TIMER1_Stop
#define sp1kTimer1Config    TIMER1_Config
#define sp1kPwrOffCfg       sp1kTmrSvc_PowerOffCfg
#define sp1kPwrOffProcChk   sp1kTmrSvc_PowerOffChk

#define sp1kTimer0Start     TIMER0_Start
#define sp1kTimer0Stop      TIMER0_Stop

#define sp1kDramFileSave	dramFileSave
#define sp1kDramFileLoad	dramFileLoad

#define sp1kSfCallbackReg	sfCallbackReg
#endif

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
/**
 * @fn void sp1kPvPhotofrmModeSet(UINT8 mode)
 * @brief	Select blending mode.
 *
 * @param[in] mode     lending mode
 *\n                   SP1K_PRV_BLEND_PHOTOFRM : Photoframe mode (Default)
 *\n                   SP1K_PRV_BLEND_ROTATION: Rotation
 * @retval none
 */
void sp1kPvPhotofrmModeSet(UINT8 mode);


/**
 * @fn void sp1kRTCInit(void)
 * @brief	RTC initialization.
 *
 *           This function should be called once in the system initial.
 *\n         This function will get the reliable code and set a default value if the reliable code is not correct.
 *\n         The initial value is 2008/1/1 12:00
 * @param	none
 * @retval 	none
 */

void sp1kRTCInit(void);

/**
 * @fn UINT8 sp1kSuspendResume(void)
 * @brief	Force system enter suspend state and wait for INT0 event to resume system.
 *
 *          System designer usually set some GPIO pins as interrupt function for resuming system.
 * @param	none
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kSuspendResume(void);

/**
 * @fn void sp1kLoadDateStampData( void )
 * @brief	Load datestamp YUV data to specific DRAM buffer address(K_SDRAM_GrafFontBufAddr)
 * @param	none
 * @retval 	none
 */
void sp1kLoadDateStampData( void );

/**
 * @fn void sp1kDateStampCustomStrSet(UINT8 *str)
 * @brief	Set the custom string for date stamp. 
 * @param	str : Specify the custom string.
 * @retval 	none
 */
void sp1kDateStampCustomStrSet(UINT8 *str);

/**
* @fn void sp1kDateStampParamSet(UINT8 paraId,UINT16 value)
* @brief   Set parameters for date stamp
* @param[in] paraId : one of the enum member of "sp1kDsParam_t"
* @param[in] value : the threshold for pasteing
* @retval void
*/
void sp1kDateStampParamSet(UINT8 paraId, UINT16 value);

/**
* @fn UINT16 sp1kDateStampParamGet(UINT8 paraId)
* @brief   Get parameters for date stamp
* @param[in] paraId	: one of the enum member of "sp1kDsParam_t"
* @retval param value : the threshold for pasteing	
*/
UINT16 sp1kDateStampParamGet(UINT8 paraId);

/**
 * @fn UINT8 sp1kUsbPccamStart(void)
 * @brief	Start PCCAM by different resolution. (resolution index is sent by vendor command 0x25)
 * @param	none
 * @retval      none
 */
void sp1kUsbPccamStart(void);

/**
 * @fn UINT8 sp1kUsbIntrDataTx(UINT8 Event,UINT8 Value)
 * @EP7 Intr send data to pc
 * @param	Event:  0x00 =button press; 0x01-0xff=stream error
 			Value:  0x00=button released;0x01=button pressed
 * @retval      none
 */
void sp1kUsbIntrDataTx(UINT8 Event,UINT8 Value);


/**
 * @fn UINT8 sp1kUsbPccamStop(void)
 * @brief	Stop PCCAM and enable display module.
 * @param	none
 * @retval      none
 */
void sp1kUsbPccamStop( void );

/**
 * @fn		  UINT8 sp1kUsbStrSet(UINT32 resIdx)
 * @brief	  Set USB string from resource file
 * @param	  [in] resIdx
 * @retval	  return = SUCCESS / FAIL
*/
UINT8 sp1kUsbStrSet(UINT32 resIdx );

/**
 * @fn        void sp1kSuspendUiIntrSet(UINT8 enable)
 * @brief     Set interrupt for suspend mode
 * @param     [in] enable
                   1: enabled
                   0: disabled
 * @retval    NONE
*/
void sp1kSuspendUiIntrSet(UINT8 enable);

/**
 * @fn        void sp1kCodeSentrySet(UINT8 enable)
 * @brief     Enable the code sentry (MPU)
 * @param     [in] enable
                   1: enabled
                   0: disabled
 * @retval    NONE
*/
void sp1kCodeSentrySet(UINT8 enable);

/**
 * @fn        void sp1kUsbMsdcMLunSet(UINT8 en)
 * @brief     sp1kUsbMsdcMLunSet
 * @param     [in] en
                   1: enabled
                   0: disabled
 * @retval    return = SUCCESS / FAIL
*/
void sp1kUsbMsdcMLunSet(UINT8 en);

/**
 * @fn        void sp1kFwUpdateAddrSet(UINT32 addr)
 * @brief     Set the dram address for f/w update
 * @param     [in] addr : dram word address
 * @retval    NONE
*/
void sp1kFwUpdateAddrSet(UINT32 addr);

/**
 * @fn        UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr)
 * @brief     card firmware update process
 * @param     [in] name :the target file name to update
 * @param     [in] addr :the dram word address for firmware update
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr);

/**
 * @fn        UINT8 sp1kCardFirmwareBoot(UINT8 *name, UINT32 addr)
 * @brief     boot from card
 * @param     [in] name :the target file name to boot
 * @param     [in] addr :the dram word address for firmware boot
 * @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kCardFirmwareBoot(UINT8 *name,	UINT32 addr);

/**
 * @fn        void sp1kCardResourceUpdate(void)
 * @brief     update the resource files from card
 * @param     NONE
 * @retval    NONE
*/
void sp1kCardResourceUpdate(void);

/**
 * @fn		void sp1kRTCDateInit(pDateStc_t DatePt)
 * @brief	Init RTC Date .
 * @param	pDateStc_t DatePt
 * @retval	NONE
 * @bug		NONE.
*/
void sp1kRTCDateInit(pDateStc_t DatePt);

/**
 * @fn		void sp1kUIToolSet(UINT8 id,UINT32 addr,UINT32 size,UINT32 resid,UINT16 msg);
 * @brief	uitool set addr/size for id
 * @param	UINT8 id: 	usb tranfer id
 * @param	UINT32 addr:	data dram addr(byte)
 * @param	UINT32 size:   data size(byte)
 * @param	UINT16 resid:  data res id
 * @param	UINT16 msg:  send msg after data complete
 * @retval	NONE
 * @bug		NONE.
*/
UINT8 sp1kUIToolSet(UINT8 order,UINT8 id,UINT32 addr,UINT32 size,UINT16 resid,UINT16 msg);
UINT8 sp1kUIToolGet(	UINT8 id,	UINT32 xdata* addr,	UINT32 xdata* size);
#endif //__SP1K_UTILITY_H__

