/*

Copyright (c) 2007 Sunplus Technology Co., Ltd.

Module Name:

		key_scan.h

Abstract:

		Module related to the definition of key scan

Environment:

		Keil C51 Compiler

Revision History:

		07/24/2007		Phil	created

*/

#ifndef __APP_BATTERY_H__
#define __APP_BATTERY_H__
//=============================================================================
//Header file
//=============================================================================
#include "solution.h"
//=============================================================================
//Symbol
//=============================================================================]

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define BATT_LOW_PWR_OFF 		0
#define BATT_LOW_PWR_OFF_DELAY		500 // 500*10ms= 5s
#if (_HW_SOLUTION_ == _HW_EVB_ )	// 		
#define POWER_BAT_LV_FULL		632//235 // 3.9v	
#define POWER_BAT_LV_HALF		614//226 // 3.8v	
#define POWER_BAT_LV_LOW		596//220 // 3.7v	
#define POWER_BAT_LV_EMPTY		580//214//217 // 3.6v	

#define POWER_BAT_GAP			0
#elif(_HW_SOLUTION_ == _HW_TK_MODEL_B_)  // 3.6V ~ 6V: ADC val-> 446 ~ 744
#define POWER_BAT_LV_FULL		567		// 4.7V
#define POWER_BAT_LV_HALF		535		// 4.4V
#define POWER_BAT_LV_LOW		494		// 4.1V
#define POWER_BAT_LV_EMPTY		482		// 4.0V
#define POWER_BAT_GAP			10
#elif(_HW_SOLUTION_ == _HW_RDK_A_)  // 3.6V ~ 6V: ADC val-> 446 ~ 744
#define POWER_BAT_LV_FULL		615//567		// 4V
#define POWER_BAT_LV_HALF		589//535		// 3.8V
#define POWER_BAT_LV_LOW		570//494		// 3.7V
#define POWER_BAT_LV_EMPTY	550//482		// 3.6V
#define POWER_BAT_GAP			10
#elif(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_ || _HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)  // 3.6V ~ 6V: ADC val-> 446 ~ 744
#define POWER_BAT_LV_FULL		590//600//567		// 4V
#define POWER_BAT_LV_HALF		575//535		// 3.8V
#define POWER_BAT_LV_LOW		560//494		// 3.7V
#define POWER_BAT_LV_EMPTY	550//482		// 3.6V
#define POWER_BAT_GAP			10



#endif
//note:add battery gap to avoid battery level change rapidly
/*
|
|
|Full Battery
|
----------------------->Power Bat Level Full
   Gap
----------------------->Power Bat Level Full - Gap
|
|Half Battery
|
----------------------->Power Bat Level Half
   Gap
----------------------->Power Bat Level Half - Gap
|
|Low Battery
|
----------------------->Power Bat Level Low

----------------------->Power Bat Level Empty
|
|Empty Battery
|(Post power off msg)
|
*/
//------------------------------------------------------------------------------
//Data type
//------------------------------------------------------------------------------
typedef enum powerBatLv {
	POWER_BAT_FULL = 0x00,
	POWER_BAT_HALF = 0x01,
	POWER_BAT_LOW = 0x02,
	POWER_BAT_EMPTY = 0x03,
	POWER_BAT_BELOWEMPTY = 0x04,
	POWER_BAT_TOTAL = 0x05,
} powerBatLv_t;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void appBattInit(void);
UINT8 appBattStsHandle(void);
void appBatLvGet(powerBatLv_t* batlv);
void appChargeModeChk(void);
void appChargeModeSet(UINT8 value);
UINT8  appChargeModeGet(void);
void appChargePinCfg(void);
void appChargeEnable(void);
void appChargeDisable(void);
void appBatteryChgDetect(void);
#endif  /* __APP_BATTERY_H__ */

