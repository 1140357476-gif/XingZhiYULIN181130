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
/**
 * @file		app_battery.c
 * @brief		battery detect releated api
 * @author	Phil Lin
 * @since		2008-07-01
 * @date		2008-07-01
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

#include "os_msg.h"
#include "app_msg.h"
#include "main.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "sp1k_disp_api.h"	 
#include "sp1k_osd_api.h"
#include "sp1k_util_api.h"
#include "sp1k_tmr_svc.h"
#include "sp1k_gpio_api.h"		  
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_main.h"
#include "app_battery.h"
#include "app_sts_pwroff.h"
#include "dbg_def.h"
#include "sp1k_hal_api.h"
#include "initio.h"
#include "app_dev_usb.h"
#include "app_ui_osd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 /*Define the gap value of battery detect*/
#define BATT_DETECT_DEV	0//10
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/*
Note: Customer define battery adc value here ,
When the host receive the msg "SP1K_MSG_BAT_LV_UPDATE", 
you can get the battery level by "appBatLvGet"
*/

static UINT16 xdata battAdcVal[] = {
	POWER_BAT_LV_FULL,
	POWER_BAT_LV_HALF,
	POWER_BAT_LV_LOW,
	POWER_BAT_LV_EMPTY,
};

static powerBatLv_t xdata G_BatteryLevel = POWER_BAT_FULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

xdata UINT8 chargeMode;
//-----------------------------------------------------------------------------
//appBatLvGet
//-----------------------------------------------------------------------------
/**
 * @brief	get battery level
 * @param	none
 * @retval	none
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appBatLvGet(powerBatLv_t* batlv)
{
	scanTag_t* pAdcHdl;

	sp1kAdcHdlGet(&pAdcHdl);

	*batlv = pAdcHdl->battAdc.battLvl;
	//printf("Lv(%bu)\n", *batlv);
}



//-----------------------------------------------------------------------------
//appBattInit
//-----------------------------------------------------------------------------
/**
 * @brief		appBattInit
 * @param	
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since		2008-07-01
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

void appBattInit(void)
{
	UINT8 battLv;

	/*Battery detect configure*/	
	battDetectCfg(sizeof(battAdcVal) / sizeof(UINT16), &battAdcVal[0], BATT_DETECT_DEV);

	/*Battery detect initialize, The value returned is the current battery level*/
	battLv = battDetectInit();

	/*Enable battery detect function*/
	sp1kBattEnable(1);

	/*Disabled battery detect in power on process on EVB hw solution*/
	//#if (_HW_SOLUTION_ != _HW_EVB_)
	if (sp1kUSB_PlugGet() != 0) {
	// if (!gpioByteInGet(GPIO_BYTE_DISP0, 0x02, 1)) {
		/*If power on with the USB power, disable the battery detect function*/
		sp1kBattEnable(DISABLE);
	} else {
		if (battLv >= POWER_BAT_BELOWEMPTY) {
			/*Low battery detected in power on process*/
			printf("Low battery!\n");
			// appPowerShutDown();

			appPowerHoldPullDown();
		}
	}
	//#endif
}

/**
 * @brief	appBattStsHandle
 * @param	none
 * @retval	none
 * @see
 * @author	phil.lin
 * @since	2008-02-23
 * @todo
 * @bug
 * @note
*/
UINT8 appBattStsHandle(void)
{
	powerBatLv_t batlv;

	appBatLvGet(&batlv);

	//battery empty flow
	if (batlv >= POWER_BAT_BELOWEMPTY) {
#if BATT_LOW_PWR_OFF
		/*check if the power off process is running*/
		if (!sp1kPwrOffProcChk()) {
			/*if not, configure and start the process*/
			sp1kPwrOffCfg(1, BATT_LOW_PWR_OFF_DELAY);//5s
		}
#else
		uiUpdateOSDBatEmpty();
		//sp1kHalGPIOBitSet(PWR_ON_EN, 0);//power off
		osMsgPost(SP1K_MSG_POWER_OFF);
		sp1kBtnDisableAll();//ready to power off, disable all button!
#endif
	}

	return batlv;
}

void appChargeModeChk(void)
{
#if (_HW_SOLUTION_ != _HW_EVB_)
	if (prjDbgMode) {
		if (sp1kBtnChk(SP1K_BTN_POWER) == KEY_STATUS_PRESSED) {
			appChargeModeSet(1);
		} else {
			appChargeModeSet(0);
		}
	} else {//normal mode
		if (!bUsbPwr) {
			if (sp1kUSB_PlugGet() == USB_PLUG_IN) {
				appChargeModeSet(1);
			} else {
				appChargeModeSet(0);
			}			
		}
	}
#endif
}

void appChargeModeSet(UINT8 value)
{
	chargeMode=value;

}

UINT8  appChargeModeGet(void)
{
	return chargeMode;
}


void appChargePinCfg(void)
{
#if (_HW_SOLUTION_ == _HW_RDK_A_)
	gpioByteDirSet(GPIO_BYTE_FM2,0x02,0x02);// fmgpio 17 
	gpioByteOutSet(GPIO_BYTE_FM2,0x02,0x02);//disable charge 
	gpioByteDirSet(GPIO_BYTE_FM1,0x40,0x00);//fmgpio 14 charge done detect pin
	gpioPullSelSet(1,0x40,0x40);
	gpioPullEnSet(1,0x40,0x40);
#elif((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
	gpioByteFuncSet(GPIO_BYTE_GEN2,0x04,0x04);
	gpioByteDirSet(GPIO_BYTE_GEN2,0x04,0x04);//gpio 18
	gpioByteOutSet(GPIO_BYTE_GEN2,0x04,0x00);//disable charge 
#endif
}

void appChargeEnable(void)
{
#if (_HW_SOLUTION_ == _HW_RDK_A_)
		gpioByteOutSet(GPIO_BYTE_FM2,0x02,0x00);// fmgpio 17 batter charge en
#elif((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
		gpioByteOutSet(GPIO_BYTE_GEN2,0x04,0x04);//gpio18 charge en
#endif

}
void appChargeDisable(void)
{
#if (_HW_SOLUTION_ == _HW_RDK_A_)
		gpioByteOutSet(GPIO_BYTE_FM2,0x02,0x02);// fmgpio 17 batter charge disable
#elif((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
		if(!appChargeModeGet())
		{
		gpioByteOutSet(GPIO_BYTE_GEN2,0x04,0x00);//gpio18 charge disable
		}
#endif

}
void appBatteryChgDetect(void)
{

#if (_HW_SOLUTION_ == _HW_RDK_A_)
	static UINT8 chgOver=0xFF;
	static UINT8 gpioValue=0xff;
	static UINT8 time;
	
	if(appCurrStateGet() != APP_STATE_IDLE)
	{
		return;
	}
	gpioByteOutSet(GPIO_BYTE_FM2,0x02,0x00);//charge enable 
	if(gpioValue != gpioByteInGet(GPIO_BYTE_FM1,0x40,1))//fmgpio14     0x10:batter charge done	0:charging
	{
		gpioValue= gpioByteInGet(GPIO_BYTE_FM1,0x40,1);
		time=0;
	}
	else
	{
		time++;
	}
	if(time>10)
	{
		time=0;
		if(chgOver != gpioValue)
		{
			chgOver=gpioValue;
			if(chgOver)
			{
				osMsgPost(SP1K_MSG_CHG_BATTERY_FULL);
			}
			else
			{
				osMsgPost(SP1K_MSG_CHG_BATTERY_SHORT);
			}

		}
	}
	
			
#elif((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
	UINT16 adcVal=0;
	UINT8 sts=0;
	static UINT16 preMsg = SP1K_MSG_NULL;
	UINT8 reg2649=0;
	static UINT8 timeH=0;
	static UINT8 timeL=0;
	if(appCurrStateGet() != APP_STATE_IDLE)
	{
		return;
	}
	ENTER_CRITICAL(sts);
	reg2649 = sp1kADCSarGet;
	sp1kAdcValGet(0x00,&adcVal);
	sp1kADCSarSel(reg2649);
	EXIT_CRITICAL(sts);
	if(adcVal > 640)
	{
		timeH++;
		timeL=0;
		if(timeH<5)
		{
			return;
		}
		else
		{
			timeH=0;
		}
		if(preMsg != SP1K_MSG_CHG_BATTERY_FULL)	
		{
			osMsgPost(SP1K_MSG_CHG_BATTERY_FULL);
			preMsg=SP1K_MSG_CHG_BATTERY_FULL;
	}
	}
	if(adcVal < 620)
	{
		timeL++;
		timeH=0;
		if(timeL<5)
		{
			return;
		}
		else
		{
			timeL=0;
		}
		if(preMsg != SP1K_MSG_CHG_BATTERY_SHORT)
		{
			osMsgPost(SP1K_MSG_CHG_BATTERY_SHORT);
			preMsg=SP1K_MSG_CHG_BATTERY_SHORT;
	}
	}
#endif
}

