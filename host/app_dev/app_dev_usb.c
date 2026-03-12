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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
		  
#include "solution.h"
#include "initio.h"
#include "os_msg.h"
#include "app_msg.h"
#include "cardlink.h"
#include "main.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "usbmsdc2.h"
#include "uiflow.h"
#include "sp1k_gpio_api.h"
	 
#include "sp1k_disk_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_cdsp_api.h"
//#include "sp1k_pv_api.h"

#include "app_ui_osd.h"
#include "app_usb.h"
#include "app_battery.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"

#include "app_dev_disp.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_main.h"
#include "app_battery.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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
 
 extern UINT8 G_UI_USBConnect;
// extern xdata UINT8 G_USBCharge;
 extern xdata UINT8 G_PCCAM;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC


//-----------------------------------------------------------------------------
//appUSBOutDetect
//-----------------------------------------------------------------------------
/**
 * @brief	detect tv plug in plug out
 * @param	none
 * @retval	none
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
 * @note	in EVB,can not connect TV and usb together.
*/
extern UINT8 user_exit_menu_flag;
void appUSBOutDetect(void)
{
	static UINT8 plugIn = 0xFF;
	static UINT8 usbStatus = 0xFF;
	static UINT8 time = 0;
	scanTag_t* pAdcHdl;
	UINT8 sts;
	UINT16 msg;
	//static UINT8 flg=0;

	if ( plugIn == sp1kUSB_PlugGet() ) {
		time++;
	}
	else {
		plugIn = sp1kUSB_PlugGet();
		time = 0;
		return;
	}

	if ( time < 3 ) {
		return;//detect 3 times to avoid dithering
	}

	sts = appCurrStateGet();
	if (sts == APP_STATE_POWER_ON) {
		return;
	}
	
	if ( usbStatus != plugIn ) {
		//if ( (sts != APP_STATE_POWER_ON)&&(sts != APP_STATE_IDLE)  ) {
			usbStatus = plugIn;
		//}
		
		if ( plugIn ) {  /* usb plug in. */
			if(sts == APP_STATE_IDLE)
			{
				return;
			}
			
			if ( (sts != APP_STATE_POWER_ON)&&(sts != APP_STATE_IDLE)  ) {
				//sp1kHalUSBSwPlugIn(); //software plug in
				sp1kTimeDelay1s();//wait about 1 second

				//disable battery detect function, force battery level to full
				sp1kBattEnable(DISABLE);
				sp1kAdcHdlGet(&pAdcHdl);
				pAdcHdl->battAdc.battLvl = POWER_BAT_FULL;
				if(sts != APP_STATE_USB)
				{
					//uiUpdateBatteryLevel();
				}
			}

			msg = SP1K_MSG_USB_PLUG_IN;

			sp1kHalUSBSwPlugIn(); //wait PC ack
			sp1kHalWait(2000);
			sp1kHalUSBSwPlugOut();
			
			//sp1kHalUSBSwPlugIn(); //wait PC ack
			//sp1kHalWait(800);

			//sp1kHalUSBSwPlugOut();
			/*add for usb charge S*/	
			/*sp1kHalUSBSwPlugIn(); 
			sp1kHalWait(300);
			
			if (!G_USBCharge){
				printf("charge=%bx\n",G_USBCharge);
				//appUsbBack2State();	
				sp1kHalUSBSwPlugOut();
				//osMsgFlush();
				
				//sp1kPrevewUnfreeze();
				
				return;
			}
			G_USBCharge=0;
			sp1kHalUSBSwPlugOut();*/
			/*add for usb charge E*/	
		}
		else {  /* usb plug out. */
			if ( (sts != APP_STATE_POWER_ON)&&(sts != APP_STATE_IDLE)  ) {
				G_UI_USBConnect = 0;
				sp1kBattEnable(ENABLE);
				uiUpdateBatteryLevel();
				//HAL_USBSwPlugOut();
				if(G_PCCAM ==1){
					G_PCCAM = 0;
					sp1kUsbPccamStop();
					TIMER0_Start();
				}
			}
			user_exit_menu_flag = 0;
			msg = SP1K_MSG_USB_PLUG_OUT;
		}

		osMsgPost(msg);
	}
}

void UI_UsbModeSet(UINT32 mode) USING_0
{
	switch( mode ){
/*
		case K_UISTATUS_USBMODE:
			G_UIStatus = K_UISTATUS_USBMODE;
			break;
*/			
		case K_UISTATUS_USBMODE_PCCAM:
			G_UIStatus = K_UISTATUS_USBMODE_PCCAM;
			break;
		case K_UISTATUS_USBMODE_MASS:
			G_UIStatus = K_UISTATUS_USBMODE_MASS;
//			if (G_SDCardPlugInOut == 0x01){
				sp1kDiskMemInit();
				sp1kDiskGlobalInit();

				MLUN_Initialize();
//				G_SDCardPlugInOut = 0x00;
//			}
			break;
		default:
			break;
	}
}

