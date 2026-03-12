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
#include "general.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "initio.h"
#include "Gpio_api.h"
#include "hal.h"
#include "hal_gprm.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "App_ui_para.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"
#include "app_idle.h"
#include "app_menu_core.h"
#include "app_menu.h"
#include "app_menu_api.h"
#include "app_menu_micro.h"
#include "app_menu_tab.h"
#include "app_menu_event.h"
#include "app_audio_rec.h"
#include "app_cal_api.h"
#include "cardlink.h"
#include "sp1k_disk_api.h"
#include "key_scan.h"
#include "func_ptr.h"
#include "copydisk.h"
#include "main.h"
#include "Macro_expand.h"
#include "Sp1k_front_api.h"
#include "Sp1k_gpio_api.h"
#include "Sp1k_hal_api.h"
#include "Sp1k_disp_api.h"
#include "sp1k_util_api.h"
#include "sp1k_pb_api.h"
#include "dev_init.h"

/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT8 offPanelTime=0;
xdata UINT8 batterFull;// 1:full 0:not full
xdata UINT8 panelOnOff=1;// 1:panel on 0: panel off

 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

//-----------------------------------------------------------------------------
//appIdle
//-----------------------------------------------------------------------------
void appIdle(UINT16 msg) USING_0
{

	if((msg<SP1K_MSG_KEY_PRESS_MAX)&&(msg>SP1K_MSG_START_BTN))
	{
		if(!panelOnOff)// if panel is off now
		{
		appChargeDisable();
		panelOnOff=1;
		sp1kDispPowerOn(sp1kDispDevActGet());
		sp1kDispImgShow(sp1kPbDispBuffGet(1), 320, 240,100);
#if((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
		appLcdLightSet(1);
#endif
		}
		appTimerCountSet();
		offPanelTime=0;
		
	}

	switch (msg) {
	case SP1K_MSG_STATE_INIT:
		// printf("idle init\n");
		appIdleInit();
		break;
    	case SP1K_MSG_KEY_PRESS_S1:
    	case SP1K_MSG_KEY_PRESS_S2:
    	case SP1K_MSG_KEY_PRESS_MENU:
    	case SP1K_MSG_KEY_PRESS_UP:
    	case SP1K_MSG_KEY_PRESS_DOWN:
	case SP1K_MSG_KEY_PRESS_LEFT:
    	case SP1K_MSG_KEY_PRESS_RIGHT:		
    	case SP1K_MSG_KEY_PRESS_SNAP:
	case SP1K_MSG_KEY_PRESS_F1:
    	case SP1K_MSG_KEY_PRESS_TELE:
    	case SP1K_MSG_KEY_PRESS_WIDE:
    	case SP1K_MSG_KEY_PRESS_DISP:
    		osMsgFlush(MSG_CTRL_FLUSH_BTN);
    		break;
	case SP1K_MSG_TIMER_COUNT:
		offPanelTime++;
		if(offPanelTime == 10)//5s
		{
			sp1kTimer1Stop();
			#if((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
			appLcdLightSet(0);
			#endif
			sp1kDispPowerOff(sp1kDispDevActGet());//off panel
			panelOnOff=0;
			if(batterFull)//battery is full
			{
				appChargeDisable();

			}
			else
			{
				appChargeEnable();
			}
		}
		appChargeBGShow();
		break;
	case SP1K_MSG_CARD_PLUG_IN:
		break;
	case SP1K_MSG_CARD_PLUG_OUT:
		break;
	case SP1K_MSG_CARD_NOT_SUPPORT:
		break;
    	case SP1K_MSG_USB_PLUG_IN:
		break;
	case SP1K_MSG_USB_PLUG_OUT:
		#if((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
		appLcdLightSet(0);
		#endif
		sp1kDispPowerOff(sp1kDispDevActGet());//off panel
	    appPowerShutDown();
		break;
	case SP1K_MSG_BAT_LV_UPDATE:
		break;
	case SP1K_MSG_BAT_EMPTY:
		break;
	case SP1K_MSG_POWER_OFF:
		break;
	case SP1K_MSG_KEY_PRESS_POWER:
		appIdleExit();
		break;
	case SP1K_MSG_TV_PLUG_IN:
	case SP1K_MSG_TV_PLUG_OUT:
		break;
	case SP1K_MSG_HDMI_PLUG_IN:
		break;
	case SP1K_MSG_HDMI_PLUG_OUT:
		break;
	case SP1K_MSG_CHG_BATTERY_FULL:
		batterFull=1;
		appChargeDisable();
		break;
	case SP1K_MSG_CHG_BATTERY_SHORT:
		batterFull=0;
		if(!panelOnOff)
		{
			appChargeEnable();
		}
		break;
	default:
		break;
	}
}
//-----------------------------------------------------------------------------
//appIdleInit
//-----------------------------------------------------------------------------
/**
 * @fn        void appIdleInit(void)
 * @brief     appIdleInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-12
 * @todo      N/A
 * @bug       N/A
*/
void appIdleInit(void) USING_0
{
	UINT8 sensorId;
	
	sensorId = sp1kFrontSensorInit();
	sp1kFrontDevActSet(sensorId);
	sp1kFrontPowerOff(sensorId);
	
	#if((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x80, 0x80);/*sensor power down*/
	#endif
	
	appTimerCountSet();
	appAutoOffEn(0);
}


/**
 * @fn        void appIdleExit(void)
 * @brief     appIdleExit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-12
 * @todo      N/A
 * @bug       N/A
*/
void appIdleExit(void)
{
	UINT8 sensorId;
	
	sp1kTimer1Stop();
	#if((_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_) ||(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_))
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x80, 0x00);/*sensor power down*/
	#endif
	sensorId = sp1kFrontSensorInit();
	sp1kFrontDevActSet(sensorId);
	sp1kFrontPowerOn(sensorId);
	osMsgPost(SP1K_MSG_STATE_TO_POWER_ON);
	if(sp1kSD_CdGet() == 0)
	{
		osMsgPost(SP1K_MSG_CARD_PLUG_IN);
	}
	else
	{
		osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
	}
	sp1kHalCtrlScreen(3);
	appChargeModeSet(0);
	appChargeDisable();
}


/**
 * @fn        void appTimerCountSet(void)
 * @brief     appTimerCountSet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-15
 * @todo      N/A
 * @bug       N/A
*/
void appTimerCountSet(void)
{
	sp1kTimer1Stop();
	sp1kTimer1Config(500, pFunc_TimerCount,NULL);
	sp1kTimer1Start();
}


/**
 * @fn        void appTimerCount(void)
 * @brief     appTimerCount
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-15
 * @todo      N/A
 * @bug       N/A
*/
void appTimerCount(void)
{
	osMsgPost(SP1K_MSG_TIMER_COUNT);
}

/**
 * @fn        void appChargeBGShow(void)
 * @brief     appChargeBGShow
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-15
 * @todo      N/A
 * @bug       N/A
*/
void appChargeBGShow(void)
{
	static UINT8 chargeBGId=0;
	UINT32 resId;
	switch(chargeBGId)
	{
		case 0:
			resId=0x38;
			break;
		case 1:
			resId=0x39;
			break;
		case 2:
			resId=0x3A;
			break;
		default:
			resId=0x3A;
		break;

	}
	//printf("##resId=%ld\n",resId);
	printf("battfull%bu\n",batterFull);
	if(batterFull)
	{
		resId=0x3A;//image of battery full
	}
	appImgDecode(resId);
	sp1kDispImgShow(sp1kPbDispBuffGet(1), 320, 240,100);
	chargeBGId++;
	if(chargeBGId>2)
	{
		chargeBGId=0;
	}

}


