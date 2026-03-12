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
#include "app_init.h"
#include "cardlink.h"
#include "main.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "usbmsdc2.h"
#include "uiflow.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "asicreg.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "i2c.h"
#include "exif_entry.h"

#include "sp1k_gpio_api.h"
#include "sp1k_disk_api.h"
#include "sp1k_dos_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_video_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_cdsp_api.h"
#include "sp1k_dcf_api.h"
#include "sp1k_ae_api.h"

#include "app_menu_tab.h"
#include "app_ui_osd.h"
#include "app_osd_icon.h"
#include "app_usb.h"
#include "app_still_play.h"
#include "app_multi_play.h"
#include "app_audio_play.h"
#include "app_battery.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_pwr_mgr.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_main.h"
#include "app_battery.h"
#include "dbg_def.h"
#include "app_menu.h"
#include "app_gui_obj.h"
#include "rsc_id.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	  __FILE_ID_APP_DEV_DISP__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
panelStatus_t xdata G_PanelStatus = PANEL_STATUS_LCD;//0:LCD  1:NTSC  2:PAL
#if SUPPORT_PANEL_PROTECT
xdata UINT16 G_AutoOffPanelTime;
#endif
// 1 : open
// 0 : close
static UINT8 lcd_backlight_status = 1;

/* 
	打开闪光灯拍照时需要增加曝光时间，降低GAIN（减少噪点）
	打开条件：
		在暗态下，并且打开闪光灯
 */
UINT8 flash_led_snap_adj_exp = 0;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

extern xdata UINT8 G_PCCAM;
extern UINT8 TV_Open;
extern UINT16 currPos;
extern UINT8 sensor_switch;



/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern void sp1kDispPnlOpen(UINT8);

//-----------------------------------------------------------------------------
//appTVOutDetect
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
*/
extern UINT8 G_UI_USBConnect;
void appTVOutDetect(void)
{
	static UINT8 plugOut = 0xFF;
	static UINT8 tvStatus = 0xFF;
	static UINT8 time = 0;
	static UINT8 tvMode = 0xFF;
	UINT8 mode;
	uiPara_t* uiPara;
	devAttr_t* devAttr;
	UINT8 sts;


	if ( plugOut == sp1kTV_PlugGet() ) {
		time++;
	}
	else {
		plugOut = sp1kTV_PlugGet();
		time = 0;
		return;
	}

	if ( time < 3 ) {
		return;//detect 3 times to avoid dithering
	}

	if ( (appPanelTypeGet() == plugOut) && (0xFF != G_PanelStatus) ) { //suny add for insert TV open
		uiPara = appUiParaGet();
		if ( uiPara->TVMode == TVMODE_NTSC ) {
			G_PanelStatus= PANEL_STATUS_NTSC;
		}
		else if ( uiPara->TVMode == TVMODE_PAL ) {
			G_PanelStatus= PANEL_STATUS_PAL;
		}
		//return;
	}

	sts = appCurrStateGet();
	uiPara = appUiParaGet();
	devAttr = sp1kDevHdlGet();
	if ( tvStatus != plugOut || (!plugOut && tvMode != uiPara->TVMode) ) {
		if ( !plugOut ) {  /* tv plug in. */
			if ( sts != APP_STATE_POWER_ON ) {
				//Panel = TV , so estimate ntsc/pal
				if ( uiPara->TVMode == TVMODE_NTSC ) {
					mode = PANEL_STATUS_NTSC;
					devAttr[DEV_ID_TV].devType = DEV_TYPE_TV_NTSC;
				}
				else {
					mode = PANEL_STATUS_PAL;
					devAttr[DEV_ID_TV].devType = DEV_TYPE_TV_PAL;
				}
				//tvMode = uiPara->TVMode;//jj move out

				devAttr[DEV_ID_TV].devEn = 1;
				devAttr[DEV_ID_TV].devSts = DEV_STS_CONNECTED;
				devAttr[DEV_ID_PANEL].devEn = 0;
				devAttr[DEV_ID_PANEL].devSts = DEV_STS_CONNECTED;
			}
			tvMode = uiPara->TVMode;
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
			//CPrintf("plug in \n");
		}
		else {  /* tv plug out. */
			if ( sts != APP_STATE_POWER_ON ) {
				mode = PANEL_STATUS_LCD;
				devAttr[DEV_ID_TV].devEn = 0;
				devAttr[DEV_ID_TV].devSts = DEV_STS_DISCONNECTED;
				devAttr[DEV_ID_PANEL].devEn = 1;
				devAttr[DEV_ID_PANEL].devSts = DEV_STS_CONNECTED;
			}
		}

		if ( sts != APP_STATE_POWER_ON ) {
			if ( plugOut && tvStatus != 0xFF ) {
				osMsgPost(SP1K_MSG_TV_PLUG_OUT);
				//CPrintf("out 1\n");
			}
			else {
				if(appPreStateGet(0) == APP_STATE_POWER_ON){
					if(plugOut && TV_Open) {
						osMsgPost(SP1K_MSG_TV_PLUG_OUT);
						//CPrintf("out 2\n");
						TV_Open=0;
					}

				}
			}
			//tvStatus = plugOut;//jj move out
		}
		tvStatus = plugOut;

		if ( G_PanelStatus != mode ) {
			if(G_PCCAM ==1){
				G_PCCAM = 0;
				sp1kUsbPccamStop();
				TIMER0_Start();
			}
			if ( sts != APP_STATE_POWER_ON )
			{
				G_PanelStatus = mode;
			}
			appAutoOffTimeReload();//jintao.liu 2008-4-29 auto power off
			printf("Panel Chg!\n");
		}
	}
}

void appHDMIDetect(void)
{
	static UINT8 hdmiSts = -1;
	static UINT8 cnt = -1;
	UINT8 bPlugIn;

	bPlugIn = sp1kHDMI_PlugGet();
	if (hdmiSts == bPlugIn) {
		cnt = 0;
		return ;
	}

	if (cnt++ < 3) {
		return;
	} else {
		cnt = 0;
		if (hdmiSts == -1) {
			hdmiSts = bPlugIn; //abort the first detection
			return;
		} else {
			hdmiSts = bPlugIn;
		}
	}

	if (hdmiSts) {
		//printf("HDMI i\n");
		osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
	} else {
		//printf("HDMI o\n");
		osMsgPost(SP1K_MSG_HDMI_PLUG_OUT);
	}
}

//-----------------------------------------------------------------------------
//appPanelTypeGet
//-----------------------------------------------------------------------------
/**
 * @brief	get panel type
 * @param	none
 * @retval	1: LCD, 0: TV
 * @see
 * @author	sunyong
 * @since	2008-02-23
 * @todo
 * @bug
*/
UINT8 appPanelTypeGet(void)
{
	devAttr_t* devAttr;

	devAttr = sp1kDevHdlGet();
	return devAttr[DEV_ID_PANEL].devEn;
}

//-----------------------------------------------------------------------------
//appPanelStatusGet
//-----------------------------------------------------------------------------
/**
 * @brief	get panel status
 * @param	none
 * @retval	none
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appPanelStatusGet(panelStatus_t* panelMode)
{
	//return &G_BatteryLevel;
	*panelMode = G_PanelStatus;
}

void appPanelStatusSet(panelStatus_t panelMode)
{
	//return &G_BatteryLevel;
	//*panelMode = G_PanelStatus;
	G_PanelStatus = panelMode;
}
void appDispOpen()
{
	devAttr_t* pDevAttr;
	uiPara_t* puiPara;
	puiPara = appUiParaGet();
	pDevAttr = sp1kDevHdlGet();
	if ( pDevAttr[DEV_ID_TV].devEn == 1
	&& pDevAttr[DEV_ID_TV].devSts == DEV_STS_CONNECTED ) {
		TV_Open = 1;                       //SUN 20090514
		sp1kAudioVolumeSet(puiPara->Volume );
		if ( puiPara->TVMode == TVMODE_NTSC ) {
			sp1kDispPnlOpen(DISP_DEV_NTSC);
			pDevAttr[DEV_ID_TV].devType = DEV_TYPE_TV_NTSC;
			G_PanelStatus= PANEL_STATUS_NTSC;
		}
		else {
			sp1kDispPnlOpen(DISP_DEV_PAL);
			pDevAttr[DEV_ID_TV].devType = DEV_TYPE_TV_PAL;
			G_PanelStatus= PANEL_STATUS_PAL;
		}
		pDevAttr[DEV_ID_TV].devSts = DEV_STS_RDY;
	}
	else
	{
 		//sp1kDispDevActSet(DISP_DEV_PANEL);
		//sp1kDispPowerOn(DISP_DEV_PANEL);
	}


}

//-----------------------------------------------------------------------------
//appDispSet
//-----------------------------------------------------------------------------
/**
 * @brief		panel select
 * @param	none
 * @retval	none
 * @see
 * @author	yong.sun
 * @since	2008-02-23
 * @todo
 * @bug
*/
extern xdata UINT16 G_UIStatus;
void appPreviewCdspDispResSet(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 pnl,modeSts,preModeSts,type;

	pnl = dispDevActGet();
	preModeSts = appPreStateGet(0);
	modeSts = appCurrStateGet();


	if (modeSts == APP_STATE_MENU) {
		modeSts = preModeSts;
	}

	if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON ||\
		puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
		modeSts = APP_STATE_VIDEO_VIEW;
	}

	//printf("pnl=%bu,preModeSts=%bu,modeSts=%bu\n",pnl,preModeSts,modeSts);

	switch (modeSts)
	{
		case APP_STATE_VIDEO_VIEW:

			sp1kZoomDevActGet(ZOOM_LOGI_VIDEO_MODE,&type);

			if ( type == ZOOM_LOGI_BLOCKING_TYPE ) {
				return;
			}

			switch(puiPara->VideoSize)
			{
				case MOV_SIZE_VGA:
					switch(puiPara->FrameRate)
					{
						case MOV_FRM_HIGH:
							switch(pnl){
								case 1://STATUS_LCD:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_HIGH);
									break;
								case 2://STATUS_NTSC:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_HIGH_NTSC);
									break;
								case 3://STATUS_PAL:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_HIGH_PAL);
									break;
								case 4://STATUS_HDMI:
									break;
								default:
									break;
							}
							break;
						case MOV_FRM_LOW:
						case MOV_FRM_MIDDLE:
							switch(pnl){
								case 1://STATUS_LCD:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA);
									break;
								case 2://STATUS_NTSC:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_NTSC);
									break;
								case 3://STATUS_PAL:
									sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_PAL);
									break;
								case 4://STATUS_HDMI:
									break;
								default:
									break;
							}
							break;
						default:
							ASSERT(0,0);
							break;
					}
					break;
				case MOV_SIZE_QVGA:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA);
							break;
						case 2://STATUS_NTSC:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
				case MOV_SIZE_QVGA_HFR:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA);
							break;
						case 2://STATUS_NTSC:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_QVGA_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
				case MOV_SIZE_D1:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_D1);
							break;
						case 2://STATUS_NTSC:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_D1_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_D1_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
				case MOV_SIZE_HD:
					if(((G_UIStatus & 0xff00) == 0x2100 )||sp1kHDPvsizeStatusGet()/*K_UISTATUS_USBMODE_PCCAM*/)
					{
						switch(pnl){
							case 1://STATUS_LCD:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_88);
								break;
							case 2://STATUS_NTSC:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_88_NTSC);
								break;
							case 3://STATUS_PAL:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_88_PAL);
								break;
							case 4://STATUS_HDMI:
								break;
							default:
								break;
						}
					}
					else{
						switch(pnl){
							case 1://STATUS_LCD:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68);
								break;
							case 2://STATUS_NTSC:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68_NTSC);
								break;
							case 3://STATUS_PAL:
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68_PAL);
								break;
							case 4://STATUS_HDMI:
								break;
							default:
								break;
						}
					}
					break;
				case MOV_SIZE_1080P:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_FHD);
							break;
						case 2://STATUS_NTSC:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_FHD_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_FHD_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
				default:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA);
							break;
						case 2://STATUS_NTSC:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_VGA_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
			}
			break;
		case APP_STATE_NULL:
		case APP_STATE_POWER_ON:

			sp1kZoomDevActGet(ZOOM_LOGI_STILL_MODE,&type);

			if ( type == ZOOM_LOGI_BLOCKING_TYPE ) {
				return;
			}

			switch(pnl){
				case 1://STATUS_LCD:
					sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43);
					break;
				case 2://STATUS_NTSC:
					sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43_NTSC);
					break;
				case 3://STATUS_PAL:
					sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43_PAL);
					break;
				case 4://STATUS_HDMI:
					break;
				default:
					break;
			}
			break;
		case APP_STATE_STILL_VIEW:

			sp1kZoomDevActGet(ZOOM_LOGI_STILL_MODE,&type);

			if ( type == ZOOM_LOGI_BLOCKING_TYPE ) {
				return;
			}

			switch(puiPara->ImageSize)
			{
				case PRV_SIZE_1M_16_9:
				case PRV_SIZE_2M_16_9:
				case PRV_SIZE_7M_16_9:
					switch(pnl){
						case 1://STATUS_LCD:
							if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON || puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68);
							}else{
								sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_169);
							}
							break;
						case 2://STATUS_NTSC:
							if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON || puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68_NTSC);
							}else{
								sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_169_NTSC);
							}
							break;
						case 3://STATUS_PAL:
							if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON || puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
								sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_VIDEO_HD_68_PAL);
							}else{
								sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_169_PAL);
							}
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
					
				default:
					switch(pnl){
						case 1://STATUS_LCD:
							sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43);
							break;
						case 2://STATUS_NTSC:
							sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43_NTSC);
							break;
						case 3://STATUS_PAL:
							sp1kPvZoomResLoad(SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR,SP1K_RSC_ID_STILL_960P_ZOOM_REG,SP1K_RSC_ID_STILL_VGA_43_PAL);
							break;
						case 4://STATUS_HDMI:
							break;
						default:
							break;
					}
					break;
			}
			break;
	}

}

//-----------------------------------------------------------------------------
//appDispSet
//-----------------------------------------------------------------------------
/**
 * @brief		panel select
 * @param	none
 * @retval	none
 * @see
 * @author	yong.sun
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appDispSet(void)
{
	panelStatus_t pnl;
	UINT16 hSize,vSize;
	UINT8 status;
	devAttr_t* devAttr;
	uiPara_t* puiPara;
//	UINT16 secPathVsize;
	struct mut_dispbuf{
		UINT8 dev;
		UINT16 width;
		UINT16 height;
	}dispwh[] = {
		{PANEL_STATUS_LCD,320,240},
		{PANEL_STATUS_NTSC,720,480},
		{PANEL_STATUS_PAL,720,576}
	};

	puiPara = appUiParaGet();
	appPanelStatusGet(&pnl);

	devAttr = sp1kDevHdlGet();
	switch(pnl){
		case PANEL_STATUS_LCD:
			//printf("LCD\n");
			#if FUNC_HOST_TOUCH_PANEL
			appGuiEnableSet(1);
			#endif
			sp1kAudioVolumeSet(puiPara->Volume);
			sp1kDispPnlOpen(1);
			sp1kOsdFontSizeSet(16, 32); /* Should be (16, 32), (12, 24), (8, 16) */
			//sp1kDispImgWinEn(1);
			devAttr[DEV_ID_PANEL].devSts = DEV_STS_RDY;
			break;
		case PANEL_STATUS_NTSC:
			#if FUNC_HOST_TOUCH_PANEL
			appGuiEnableSet(0);
			#endif
			//printf("NTSC\n");
			sp1kAudioVolumeSet(puiPara->Volume);
			sp1kDispPnlOpen(2);
			sp1kOsdFontSizeSet(16, 32);
			devAttr[DEV_ID_TV].devSts = DEV_STS_RDY;
			break;
		case PANEL_STATUS_PAL:
			#if FUNC_HOST_TOUCH_PANEL
			appGuiEnableSet(0);
			#endif
			//printf("PAL\n");
			sp1kAudioVolumeSet(puiPara->Volume);
			sp1kDispPnlOpen(3);
			sp1kOsdFontSizeSet(16, 32);
			devAttr[DEV_ID_TV].devSts = DEV_STS_RDY;
			break;
		default:
			break;
	}
	sp1kOsdLoad();
	menuDispParaInit();
	sp1kOsdInit(appPanelTypeGet());

	appPreviewCdspDispResSet();

	status = appCurrStateGet();
	switch(status) {
		case APP_STATE_STILL_VIEW:
			sp1kPvSizeGet(&hSize, &vSize);
			if((hSize*3)==(vSize*4)){
				hSize=320;
				vSize=240;
			}else{
			 	hSize=320;
				vSize=hSize/16*9;
			}
			break;
		case APP_STATE_STILL_PLAY:
		case APP_STATE_MUTI_PLAY:
			//sp1kPbDecodeSizeGet(&hSize, &vSize);
			hSize = dispwh[pnl].width;
			vSize = dispwh[pnl].height;
			break;
		case APP_STATE_VIDEO_VIEW:
			sp1kPvSizeGet(&hSize, &vSize);
			if((hSize*3)==(vSize*4)){
				hSize=320;
				vSize=240;
			}else{
			 	hSize=320;
				vSize=hSize/16*9;
			}
			break;
		case APP_STATE_VIDEO_PLAY:
			sp1kVideoPbSizeGet(&hSize, &vSize);
			break;
		case APP_STATE_MENU:
			switch(appPreStateGet(0)) {
				case APP_STATE_STILL_VIEW:
					sp1kPvSizeGet(&hSize, &vSize);
					if((hSize*3)==(vSize*4)){
						hSize=320;
						vSize=240;
					}else{
			 			hSize=320;
						vSize=hSize/16*9;
					}
					break;
				case APP_STATE_STILL_PLAY:
					sp1kPbDecodeSizeGet(&hSize, &vSize);
					break;
				case APP_STATE_MUTI_PLAY:
					sp1kPbDecodeSizeGet(&hSize, &vSize);
					break;
				case APP_STATE_VIDEO_VIEW:
					sp1kPvSizeGet(&hSize, &vSize);
					if((hSize*3)==(vSize*4)){
						hSize=320;
						vSize=240;
					}else{
			 			hSize=320;
						vSize=hSize/16*9;
					}
					break;
				default:
					hSize = 320;
					vSize = 240;
					break;
			}
			break;
		default:
			hSize = 320;
			vSize = 240;
			break;
	}
	//printf("h: %u v:%u\n", hSize, vSize);
	//secPathVsize = (UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
	sp1kHalDispScaleSet(hSize, vSize, 100, 0);
	sp1kDispFitShow(hSize,vSize,0);
	switch(status) {
		case APP_STATE_STILL_VIEW:
			appIconPsTabLoad(ICON_MODE_PV);
			appStillOsdInit(1);
			break;
		case APP_STATE_PAN_SHOW:
		case APP_STATE_STILL_PLAY:
			appIconPsTabLoad(ICON_MODE_PB);
			if(appStillPlayInit(1)!= SUCCESS){
				if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
					uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
				}else{
					uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
				}
			}
			break;
		case APP_STATE_MUTI_PLAY:
			//sp1kPbMultiPlayObjFrameDraw(pbFile.dwIndex, 1);
			appIconPsTabLoad(ICON_MODE_PB);
			appMultiPlayInit(0);
			break;
		case APP_STATE_VIDEO_VIEW:
			appIconPsTabLoad(ICON_MODE_VIDEO);
			appVideoOsdInit();
			break;
		case APP_STATE_AUDIO_REC:
			appIconPsTabLoad(ICON_MODE_AUDIO);
			appAudioOsdInit();
			break;
		case APP_STATE_USB:
			appUSBBGShow();
			break;
		case APP_STATE_MENU:
			switch(appPreStateGet(0)) {
				case APP_STATE_STILL_VIEW:
					appIconPsTabLoad(ICON_MODE_PV);
					appStillOsdInit(1);
					break;
				case APP_STATE_STILL_PLAY:
					appIconPsTabLoad(ICON_MODE_PB);
				if(appStillPlayInit(1)!= SUCCESS){
					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
						uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
					}else{
						uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
					}
				}
					break;
				case APP_STATE_MUTI_PLAY:
					appIconPsTabLoad(ICON_MODE_PB);
					appMultiPlayInit(0);
					break;
				case APP_STATE_VIDEO_VIEW:
					appIconPsTabLoad(ICON_MODE_VIDEO);
					appVideoOsdInit();
					break;
				default:
					appIconPsTabLoad(ICON_MODE_PV);
					appStillOsdInit(1);
					break;
			}
			break;
		default:
			appIconPsTabLoad(ICON_MODE_PV);
			appStillOsdInit(1);
			break;
	}


	//add for mantis #36958 @start
	if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet() == FALSE))
	{
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
	}
	//add for mantis #36958 @end

	/*if ( pnl == PANEL_STATUS_LCD ) {
		sp1kDispPowerOn(sp1kDispDevActGet());
	}*/
	sp1kHalCtrlScreen(3);
}

/**
 * @fn        UINT8 appDispHdmiOpen(void)
 * @brief     appDispHdmiOpen
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appDispHdmiOpen(void)
{
	sp1kHalCtrlScreen(0);
	sp1kDispPnlOpen(4);
	sp1kOsdLoad();
	menuDispParaInit();
	sp1kOsdInit(appPanelTypeGet());
	return SUCCESS;
}
void appHdmiOsdSet(UINT8 on)
{
	if(dispDevActGet() != 4)return;

	if(UI_LCD_ON_WITH_OSD == appGetOsdMode()){
		if(on){
			sp1kHalCtrlScreen(3);
		}else {
			sp1kHalCtrlScreen(1);
		}
	}
}

#ifdef WHITE_LED
//an API to ctrl white LED
//if low light --> led on
//if high light --> led off
void appWhiteLedCtrl(void)
{
	UINT8 value;

	sp1kAeStatusGet(SP1K_AE_LowLight, &value);

	if (value == 1)
	{
		sp1kLED_SelfSet(0);
	}
	else
	{
		sp1kLED_SelfSet(1);
	}
}
#endif /*WHITE_LED*/

void app_set_led(UINT8 falg){
	// gpio 4
	#if !DBG_MODE
	gpioByteFuncSet(GPIO_BYTE_GEN0, (1 << (4 & 0x07)), (1 << (4 & 0x07)));
	gpioByteOutSet(GPIO_BYTE_GEN0, (1 << (4 & 0x07)), falg ? 0x00 : (1 << (4 & 0x07)));
	gpioByteDirSet(GPIO_BYTE_GEN0, (1 << (4 & 0x07)), (1 << (4 & 0x07)));
	#endif
}

#if SUPPORT_PANEL_PROTECT
void appPanelAutoProt() {
	uiPara_t* puiPara;
	puiPara = appUiParaGet();

	if(puiPara->ProtTime == PANEL_TIME_OFF)
	{
		G_AutoOffPanelTime = 0xffff;
	}
  	else
  	{
		if((!G_AutoOffPanelTime)&&(app_get_lcd_backlight_status()))
		{
			osMsgPost(SP1K_MSG_SYS_CLOSE_LCD_BACKLIGHT);
			G_AutoOffPanelTime = 0;
		}
  	}
}
#endif

void app_set_lcd_backlight_status(UINT8 status) {
	// gpio 5
	
	lcd_backlight_status = (status ? 1 : 0);
	if (lcd_backlight_status){
		gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
		gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
		
		#if SUPPORT_PANEL_PROTECT
		app_set_close_lcd_backlight_time();
		#endif

		appAutoOffTimeReload();
	} else {
		gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
		gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0 << (9 & 0x07));
	}
}

UINT8 app_get_lcd_backlight_status() {
	return lcd_backlight_status;
}

#if SUPPORT_PANEL_PROTECT
void app_set_close_lcd_backlight_time() {
	uiPara_t* puiPara = appUiParaGet();
	
	if(puiPara->ProtTime == PANEL_TIME_30S)
	{
		G_AutoOffPanelTime = 30;
	}	
	else if(puiPara->ProtTime ==  PANEL_TIME_1MIN)
	{
		G_AutoOffPanelTime = 60;
	}	
	else  if(puiPara->ProtTime ==  PANEL_TIME_3MIN)
	{
		G_AutoOffPanelTime = 300;//180;
	}
}
#endif

#if SUPPORT_UPDATE_LED
void appUpdateStatsLightStatus() {
	static unsigned char ucDelayTime = 5;	
	static bit bFlag = 1;

	if (appCurrStateGet() == APP_STATE_VIDEO_VIEW) {
		if (--ucDelayTime == 0) {
			ucDelayTime = 5;
			bFlag = ~bFlag;
		}
		
		if (sp1kVideoRecStatusGet() == VID_REC_RUN) {
			if (bFlag) {
				app_set_led(0);
			} else {
				app_set_led(1);
			}
		} else {
			app_set_led(1);
		}
	}
}
#endif

#if SUPPORT_SPI_WRITE_CONTROL
void appSetGpio6SpiWP(UINT8 falg) {
	// gpio6
	gpioByteFuncSet(GPIO_BYTE_GEN0, 0x40, 0x40);
	gpioByteOutSet(GPIO_BYTE_GEN0, 0x40, falg ? 0x40 : 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN0, 0x40, 0x40);
}
#endif

#if SUPPORT_SPEAKER_CONTROL
void app_set_speaker(UINT8 falg) {
	uiPara_t* puiPara = appUiParaGet();
	
	if (!puiPara->SpeakSound) {
		// gpio 8
		gpioByteFuncSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), 1 << (8 & 0x07));
		gpioByteDirSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), 1 << (8 & 0x07));
		gpioByteOutSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), 0 << (8 & 0x07));
	} else {
		// gpio 8
		gpioByteFuncSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), 1 << (8 & 0x07));
		gpioByteDirSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), 1 << (8 & 0x07));
		gpioByteOutSet(GPIO_BYTE_GEN1, 1 << (8 & 0x07), falg ? 1 << (8 & 0x07) : 0 << (8 & 0x07));
	}
}
#endif

#if SUPPORT_MCU_WTD
void appSetWTDHeartbeat(UINT8 falg) {
	// gpio 10
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x04, 0x04);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x04, 0x04);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x04, falg ? 0x04 : 0x00);
}
#endif

//#if SUPPORT_SENSOR_SWITCH
// �ߵ�ƽ: ���ľ�ͷ
// �͵�ƽ: ��ͷ
void appSensorPWDNSwitch(UINT8 falg) {
	// gpio 2
	//gpioByteFuncSet(GPIO_BYTE_GEN0, 1 << (0 & 0x07), 1 << (0 & 0x07));
	//gpioByteDirSet(GPIO_BYTE_GEN0, 1 << (0 & 0x07), 1 << (0 & 0x07));
	//gpioByteOutSet(GPIO_BYTE_GEN0, 1 << (0 & 0x07), falg ? 0 << (0 & 0x07) : 1 << (0 & 0x07));
}
//#endif

void appAFCtrl(UINT8 flag) {
	
	// gpio 0
	sp1kPullEnSet(4, 0x04, 0x04);
	sp1kPullSelSet(2, 0x40, flag ? 0 : 0x40);
	gpioByteFuncSet(GPIO_BYTE_GEN0, 1 << (1 & 0x07), 1 << (1 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN0, 1 << (1 & 0x07), 1 << (1 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN0, 1 << (1 & 0x07), flag ? 0 << (1 & 0x07) : 1 << (1 & 0x07));
	
}


void appFlashLedSet(UINT8 flag) {
	// gpio 5
	sp1kPullEnSet(4,0x40,0x00);
	gpioByteFuncSet(GPIO_BYTE_GEN0, 1 << (5 & 0x07), 1 << (5 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN0, 1 << (5 & 0x07), 1 << (5 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN0, 1 << (5 & 0x07), flag ? 1 << (5 & 0x07) : 0 << (5 & 0x07));
}

/*
	0xff:reset
	0:flash off
	1:flash force on
	2:flash auto 
*/
extern UINT8 appContrastSet(UINT8 ContrastValue);
void appSnapFlashAdjustsAe(UINT8 flag) {
	uiPara_t* puiPara = appUiParaGet();
	static UINT8 val = 0;

	return;

	switch (flag) {
	case 0:
		sp1kAeStatusGet(SP1K_AE_LowLight, &val);
		
		if (val) {
			appEVSet(PRV_COMP_P20EV);
			
			/*
				��Ӧ�����������������ƫ��
				���տ������֮ǰ ��OB����ɫ����
			*/
			XBYTE[0x2118] = 0xf4;	// r
			XBYTE[0x211a] = 0xf4;	// gr
			XBYTE[0x211c] = 0xf4;	// b
			XBYTE[0x211e] = 0xf4;	// gb

			XBYTE[0x2154] = 0x5e;	// r
			XBYTE[0x2155] = 0x00;	// r
			
			XBYTE[0x2156] = 0x40;	// gr
			XBYTE[0x2157] = 0x00;	// gr
			
			XBYTE[0x2158] = 0x50;	// b
			XBYTE[0x2159] = 0x00;	// b
			
			XBYTE[0x215a] = 0x40;	// gb
			XBYTE[0x215b] = 0x00;	// gb
		}
		break;

	case 1:
	case 2:
	case 3:
		sp1kAeStatusGet(SP1K_AE_LowLight, &val);
		
		if (val) {
			appEVSet(PRV_COMP_P20EV);

			appContrastSet(PRV_CONTRAST_MIDDLE);
			
			/*
				��Ӧ�����������������ƫ��
				���տ������֮ǰ ��OB����ɫ����
			*/
			XBYTE[0x2118] = 0xf4;
			XBYTE[0x211a] = 0xf4;
			XBYTE[0x211c] = 0xf3;
			XBYTE[0x211e] = 0xf4;
			
			XBYTE[0x2154] = 0x5e;	// r
			XBYTE[0x2155] = 0x00;	// r
			
			XBYTE[0x2156] = 0x34;	// gr
			XBYTE[0x2157] = 0x00;	// gr
			
			XBYTE[0x2158] = 0x5b;	// b
			XBYTE[0x2159] = 0x00;	// b
			
			XBYTE[0x215a] = 0x34;	// gb
			XBYTE[0x215b] = 0x00;	// gb
		}
		break;

	case 0xff:
	default:
		if (val) {
			val = 0;
			
			/*
				��Ӧ�����������������ƫ��
				���ս���֮�� ��OB����ɫ��ԭ
			*/
			XBYTE[0x2118] = 0xf4;
			XBYTE[0x211a] = 0xfa;
			XBYTE[0x211c] = 0xf4;
			XBYTE[0x211e] = 0xfa;



			XBYTE[0x2154] = 0x5e;	// r
			XBYTE[0x2155] = 0x00;	// r
			
			XBYTE[0x2156] = 0x40;	// gr
			XBYTE[0x2157] = 0x00;	// gr
			
			XBYTE[0x2158] = 0x50;	// b
			XBYTE[0x2159] = 0x00;	// b
			
			XBYTE[0x215a] = 0x40;	// gb
			XBYTE[0x215b] = 0x00;	// gb

			appEVSet(PRV_COMP_0EV);

			appContrastSet(PRV_CONTRAST_OFF);
		}
		break;
	}
}

void appSnapFlashLed(UINT8 flag) {
	uiPara_t* puiPara = appUiParaGet();
	UINT8 val = 0;

	sp1kAeStatusGet(SP1K_AE_LowLight, &val);

	if (0 == puiPara->FlashLed) { // off
		appFlashLedSet(0);
		exifFlashModeSet(0);
		flash_led_snap_adj_exp = 0;
	} else if (2 == puiPara->FlashLed) { // auto
		if (flag) {
			appFlashLedSet(val ? 1 : 0);
			exifFlashModeSet(2);
			flash_led_snap_adj_exp = val ? 1 : 0;
		} else {
			appFlashLedSet(0);
			exifFlashModeSet(0);
			flash_led_snap_adj_exp = 0;
		}
	} else if (3 == puiPara->FlashLed) { // force on
		appFlashLedSet(flag);
		exifFlashModeSet(2);
		flash_led_snap_adj_exp = (flag && val) ? 1 : 0;
	}else { // on
		appFlashLedSet(flag);
		exifFlashModeSet(1);
		flash_led_snap_adj_exp = (flag && val) ? 1 : 0;
	}

}

#if SUPPORT_AF
static UINT16 postion = 0;

#if 0
static UINT8 poscmd[] = {
	0xec, 0xa3, 
	0xa1, 0x04, 
	0xf2, 0x00, //ST[8:3] 0xf8
	0xdc, 0x51,
	0x00, 0x05,
};
#elif 0
static UINT8 poscmd[] = {
	0xec, 0xa3, 
	0xa1, 0x04, // 0x05, 
	0xf2, 0x80, //ST[8:3] 0xf8
	0xdc, 0x51,
	0x00, 0x07, // 0x05,
};
#else
static UINT8 poscmd[] = {
	//0xec, 0xa3, 
	//0xa1, 0x04, // 0x05, 
	//0xf2, 0x80, //ST[8:3] 0xf8
	//0xdc, 0x51,
	0x00, 0x07, // 0x05,
};

#endif

void AF_SetPos(UINT16 postion) {
	UINT8 i2cAddrTemp = 0xff;
	UINT16 pwdnFlag = 0;
	
	UINT8 read[] = {0xff};
	UINT8 datatt[] = {0xff, 0xff};

	//if (!sensor_switch) return;

	//if (!postion)
	//	pwdnFlag = 0x8000;

	postion = postion<<4;
	postion |= 0x0007; // 0x0005;
	postion = postion & 0x3ff7; // 0x3ff5;

	postion |= pwdnFlag;
	
	poscmd[0] = (UINT8)(postion >> 8);
	poscmd[1] = (UINT8)(postion & 0xff);

	i2cDeviceAddrGet(&i2cAddrTemp);

	i2cDeviceAddrSet(0x18);
	i2cRegClkCfgSet(I2C_SOURCE_DIV_9);
	i2cClkCfgSet(I2C_REG_CLK_DIV_2048);

	i2cRegRead(read, datatt, 2, 1, 1, 0, 0);
	
	i2cCmdTableSend(poscmd, 1, 1, I2C_TRANS_MODE_NORMAL, 0, 0);

	i2cRegRead(read, datatt, 2, 1, 1, 0, 0);
	
	i2cDeviceAddrSet(i2cAddrTemp);
	i2cRegClkCfgSet(I2C_SOURCE_DIV_10);
	i2cClkCfgSet(I2C_REG_CLK_DIV_2048);
}

void AF_Set(UINT16 pos) {
	postion = pos;
	
	postion = postion >= 1024 ? 1023 : postion;
	
	AF_SetPos(postion);
}

UINT16 AF_GetPos(void) {
	return postion;
}

#if SUPPORT_FIXED_AF
void AF_Switch(void) {
	uiPara_t* puiPara = appUiParaGet();

	//if (!sensor_switch) return;

	puiPara->AFMode = !puiPara->AFMode;

	appAFCtrl(puiPara->AFMode);
	
	if (puiPara->AFMode) {
		AF_Set(431);//puiPara->AdjustMacro);//431);
	} else {
		AF_Set(0);
	}

	AF_uiAfIconShow();
}
#endif

#endif

