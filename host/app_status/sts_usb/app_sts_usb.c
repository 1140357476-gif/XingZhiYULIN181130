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
#include "main.h"
#include "initio.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_usb.h"
#include "app_init.h"
#include "app_ui_osd.h"
#include "app_osd_icon.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_audio_rec.h"
#include "app_audio_play.h"
#include "app_still_view.h"
#include "func_ptr.h"
#include "sp1k_aud_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_cdsp_api.h"
#include "sdramhw.h"
#include "asicreg.h"
#include "Uiflow.h"
#include "app_menu_tab.h"
#include "cardlink.h"
#include "usbmsdc2.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_dram.h"
#include "sp1k_gpio_api.h"
#include "sp1k_disk_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"
#include "sp1k_fd_api.h"
#include "sp1k_dist_api.h"
#include "app_battery.h"
#include "app_dev_disp.h"
#include "dbg_def.h"
#include "App_cal_api.h"
#include "reg_def.h"
#include "class.h"
#include "video_clip.h"
#include "sp1k_video_api.h"
#if !TAG_TODO
#define HAL_BulkInFromDRAM(a, b)
#define sp1kHalCamModeSet(a)
#endif
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
//extern UINT8 OsdMaxX;
//extern UINT8 OsdMaxY;
extern xdata UINT8 G_PCCAM;
extern xdata UINT8 G_Card_Type;

extern  xdata   UINT32   G_BulkSize;
extern  xdata   UINT32   G_BulkDRAMAddr;
extern  xdata   UINT32   G_BulkFlashType;
extern  xdata   UINT32   G_BulkSRAMAddr;
extern xdata UINT32 	G_LastBulkSize;
extern xdata UINT8 panelOnOff;// 1:panel on 0: panel off
extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
#if SUPPORT_MCU_WTD
extern UINT8 check_usb_msdc_flag;
#endif

void appUSBBGShow(void)
{
	UINT32 srcAdd;
	UINT32 dstAdd;
	uiPara_t* puienv;
	UINT8 xShow,yShow;
	UINT8 str[32]="PC Cam";
	UINT8 OsdMaxX, OsdMaxY;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	xShow=OsdMaxX/2-3;
	yShow=OsdMaxY-2;

	puienv=appUiParaGet();
    if(puienv->USBMode == USB_PCCAM)//xian 20090106 #35349
    	srcAdd=K_SDRAM_Avi2PvwBufAddrD-(1024L*10L);
    else
    	srcAdd=K_SDRAM_PvBufAddrD-(1024L*10L);
	dstAdd=srcAdd-(320L*240L);

	sp1kHalCdspImgSrcSel(1);
	sp1kHalFrontInputGate(0x11);
	sp1kHalCamModeSet(CAM_MODE_IDLE);

	sp1kHalCdspImgSrcSel(1);

	appBGImgDec(0x16, NULL, srcAdd, dstAdd, 1);

	sp1kDispImgShow(dstAdd, (UINT16)320, (UINT16)240, 100);

	sp1kOsdClear(0);

	//puienv=appUiParaGet(); //xian move up
	switch(puienv->USBMode) {
		case USB_PCCAM:
			str[0]='P';
			str[1]='C';
			str[2]=' ';
			str[3]='C';
			str[4]='A';
			str[5]='M';
			str[6]=0;
			xShow=OsdMaxX/2-3;
			break;
		case USB_MASS:
			str[0]='M';
			str[1]='S';
			str[2]='D';
			str[3]='C';
			str[4]=0;
			xShow=OsdMaxX/2-2;
			//yShow=OsdMaxY-2;
			break;
		case USB_SIDC:
			str[0]='S';
			str[1]='I';
			str[2]='D';
			str[3]='C';
			str[4]=0;
			xShow=OsdMaxX/2-2;
			//yShow=OsdMaxY-2;
			break;
		default:
			str[0]='E';
			str[1]='R';
			str[2]='R';
			str[3]='O';
			str[4]='R';
			str[5]=0;
			xShow=OsdMaxX/2-2;
			return;
			break;
	}
	sp1kOsdStrDraw(xShow, yShow, str, 0);
}

void appUsbSavePwr(void)
{
	sp1kTimer1Stop();
	osMsgPost(SP1K_MSG_CMD_PANEL_OFF);
}

void appUsbSavePwrSet(void)
{
	sp1kTimer1Config(5000, pFunc_USBPwrSave,NULL);
	sp1kTimer1Start();
}

void appUsbBack2State(void)
{
	sp1kDispPowerOn(sp1kDispDevActGet());
	switch(appPreStateGet(0)) {
		case APP_STATE_STILL_VIEW:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			break;
		case APP_STATE_STILL_PLAY:
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
		case APP_STATE_AUDIO_PLAY:
			osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
			break;
		case APP_STATE_AUDIO_REC:
			sp1kAudioRecTypeSet(AUDIO_TYPE_NORMAL);
			osMsgPost(SP1K_MSG_STATE_TO_AUDIO_REC);
			break;
		case APP_STATE_MENU:
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;
		case APP_STATE_MUTI_PLAY:
			osMsgPost(SP1K_MSG_STATE_TO_MULTI_PLAY);
			break;
		case APP_STATE_VIDEO_PLAY:
			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_PLAY);
			break;
		case APP_STATE_VIDEO_VIEW:
			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			break;
		default:
			break;
	}
}

void
appUsbStsInit(
	void
)
{
	uiPara_t* pUiPara;

	pUiPara = appUiParaGet();

	sp1kHalWait(100);

	/* MSDC initial */
	sp1kUsbMsdcMLunSet(MLUN_OPTION ? 1 : 0);

	if(pUiPara->LightFreq == PRV_FREQ_50) {//mantis #41604
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
	}
	else {
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
	}
	sp1kAeSetModeEn(0);
	sp1kAwbDisable();
	sp1kFdDisable();
	sp1kDistDisable();
	if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE) &&(USB_MASS == pUiPara->USBMode))
	{
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
		appUsbBack2State();
		sp1kHalCtrlScreen(3);
		osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
		return;	
	}


#if (_HW_SOLUTION_ == _HW_TK_MODEL_B_ || _HW_SOLUTION_ == _HW_RDK_A_)
	sp1kHalSpeakerSet(1);
#endif
	appWBSet(PRV_WB_AUTO);
	appEffectSet(PRV_EFFECT_NORNAL);
	appEVSet(PRV_COMP_0EV);		
	
	if(4!=sp1kDispDevActGet()){ //mantis #41688
		if(!panelOnOff)
		{
			sp1kDispPowerOn(sp1kDispDevActGet());
			sp1kHalWait(100);
			panelOnOff=1;
		}

	}	
	
#if (FOCUS_MODE == PC_FOCUS)
	pUiPara->USBMode = USB_PCCAM;
	appAfFrameInit();
#endif
	appUIUSBModeSet();
	if(pUiPara->USBMode == USB_PCCAM)
	{
		/* biao.cheng  add for fixed problem of speedy capture rec on when to PCCAM*/
		sp1kVideoRecParamSet(VD_CAPTURE,0);
		appPvForceUpdateOn(1);
		/* the end edit of biao.cheng*/
#if(K_AUDIOINTER)
		XBYTE[REG_Usb_Alt_If1_NumMax] = 0x05;			/* Set alternating max to 5 */
		XBYTE[REG_Usb_UsbMode] = 0x01;	//0x256F
		XBYTE[REG_Usb_LogicEp1_Disable] = 0xe0;  //0x2530
#else
		XBYTE[REG_Usb_Alt_If1_NumMax] = 0x08;			/* Set alternating max to 8 */
		XBYTE[REG_Usb_UsbMode] = 0x03;
		XBYTE[REG_Usb_LogicEp1_Disable] = 0x00;
#endif
	}
	appUSBBGShow();
	sp1kHalUSBSwPlugIn();
	sp1kHalWait(500); /* wait 300ms pc ack */
	if (AdapterCharge) { //conect to charge
		//CPrintf("charge!");
		sp1kHalUSBSwPlugOut();				
		appChargeEnable();
		appUsbBack2State();
		sp1kHalCtrlScreen(3);  // open pv & osd layer
		return;
	}
	
	sp1kTimer1Stop();

	#if 0
	if(4!=sp1kDispDevActGet()) { //mantis #41688
		appUsbSavePwrSet();
	}	
	#endif
	
	sp1kHalCtrlScreen(3);  // open pv & osd layer
	appAutoOffEn(0);//2008-4-29 auto power off	
   }

void appUsb(UINT16 msg) USING_0
{
//	powerBatLv_t batlv;
	uiPara_t* pUiPara;

	pUiPara = appUiParaGet();
	
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
			// appUDFSave();
			appUsbStsInit();
			#if SUPPORT_MCU_WTD
			if (pUiPara->USBMode == USB_MASS) {
				check_usb_msdc_flag = 1;
			}
			#endif
			break;
		case SP1K_MSG_KEY_PRESS_S1:
			break;
		case SP1K_MSG_KEY_PRESS_S2:
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			break;
		case SP1K_MSG_KEY_PRESS_LEFT:
			break;
		case SP1K_MSG_KEY_PRESS_RIGHT:
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			break;
		case SP1K_MSG_KEY_PRESS_TELE:
			break;
		case SP1K_MSG_KEY_PRESS_WIDE:
			break;
		case SP1K_MSG_KEY_PRESS_DISP:
			break;
		case SP1K_MSG_KEY_REPEATE_POWER:
			appPowerOffProc();
			break;
		case SP1K_MSG_KEY_PRESS_PB:
			#if 0
			if (G_PCCAM == 0x01) {
				G_PCCAM = 0;
				sp1kUsbPccamStop();
				TIMER0_Start();
			}
			
			/* mask by phil because we do usb software plug out before change usb mode */
			sp1kHalUSBSwPlugOut(); 

			if (USB_PCCAM == pUiPara->USBMode) {
				pUiPara->USBMode = USB_MASS;
			}
			else if (USB_MASS == pUiPara->USBMode) {
				pUiPara->USBMode = USB_PCCAM;
			}
			/*else if (USB_SIDC == pUiPara->USBMode) {
				pUiPara->USBMode = USB_PCCAM;
			}*/
			else {
				pUiPara->USBMode = USB_PCCAM;
			}

			osMsgPost(SP1K_MSG_STATE_INIT);
			#endif
			break;
		case SP1K_MSG_CARD_PLUG_IN:
			//if (pUiPara->USBMode != USB_PCCAM) {	//mark for mantis #0040832
				sp1kHalUSBSwPlugOut();
				G_Card_Type = K_MEDIA_NANDF;
				sp1kDiskInitialize(1);
				G_Card_Type = K_MEDIA_SDF;
				sp1kDiskInitialize(1);
				MLUN_Initialize();
				sp1kHalUSBSwPlugIn();
				if((appSDPlugStatusGet()==1) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
				{
				
					sp1kDispPowerOn(sp1kDispDevActGet());
					sp1kHalCtrlScreen(3);
					osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
					//sp1kOsdClear(0);
					appUsbSavePwrSet();
				}	
			//}
			break;
		case SP1K_MSG_CARD_PLUG_OUT:
			if (pUiPara->USBMode != USB_PCCAM) {
				sp1kHalUSBSwPlugOut();
				if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
				{	
					sp1kDispPowerOn(sp1kDispDevActGet());
					sp1kHalCtrlScreen(3);
					osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
					//uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
					appUsbSavePwrSet();
					break;
				}
				
				G_Card_Type = K_MEDIA_NANDF;
				sp1kDiskInitialize(1);
				G_Card_Type = K_MEDIA_SDF;
				sp1kDiskInitialize(1);
				MLUN_Initialize();
				sp1kHalUSBSwPlugIn();
			}
			break;
		case SP1K_MSG_USB_PLUG_IN:
			break;
			
		case SP1K_MSG_USB_PLUG_OUT:
			sp1kHalUSBSwPlugOut();
			
			appAutoOffEn(1);//2008-4-29 auto power off
			
			sp1kHalSpeakerSet(1);
			
			// appUSBBGShow();
			
			#ifdef USB_PLUG_POWER_OFF
			
			/*dbg for resume from MSDC ,snap hang up S*/	
			#if 0 //mark for mantis #41827
			sp1kAeSetModeEn(1);
			sp1kAwbReset();	
			sp1kVdInit();
			#endif

			
			/* dbg for resume from PCCAM ,snap hang up S*/	
			if (pUiPara->USBMode == USB_PCCAM) {
				sp1kDispPowerOn(sp1kDispDevActGet());
				sp1kHalCtrlScreen(3);
			}
			
			appPowerOffProc();
			return;
			#endif
			break;
		case SP1K_MSG_TV_PLUG_IN:
		case SP1K_MSG_TV_PLUG_OUT:
			appDispSet();
			break;
		case SP1K_MSG_HDMI_PLUG_IN:
			appDispHdmiOpen();
			sp1kHalCtrlScreen(3);
			break;
		case SP1K_MSG_HDMI_PLUG_OUT:
			appDispSet();				
			osMsgPost(SP1K_MSG_STATE_INIT); //mantis #41688
			break;
		case SP1K_MSG_BAT_LV_UPDATE://jintao.liu 2008-3-13 add for battery action
			break;
		case SP1K_MSG_POWER_OFF:
			//appPowerOffProc();
			break;
		case SP1K_MSG_KEY_PRESS_POWER:
			//appPowerOffProc();
			break;
		case SP1K_MSG_CMD_PANEL_OFF:
#if (FOCUS_MODE == PC_FOCUS)
				sp1kTimer1Config(300, pFunc_updateAfwindow, NULL);
				sp1kTimer1Start();
				XBYTE[0X27f5] &= ~0x04;
#endif
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
			sp1kDispPowerOff(sp1kDispDevActGet());
			panelOnOff=0;
#if (_HW_SOLUTION_ == _HW_TK_MODEL_B_ || _HW_SOLUTION_ == _HW_RDK_A_)
			sp1kHalSpeakerSet(0);//2008-7-10 add for disable audio in usb mode
#endif
			break;
		case SP1K_MSG_CMD_PANEL_ON:
			if(!panelOnOff)
			{
				sp1kDispPowerOn(sp1kDispDevActGet());
				panelOnOff=1;
				sp1kHalCtrlScreen(3);//jintao.liu 2008-6-13 open pv & osd layer
			}
			break;
		case SP1K_MSG_DO_RESET:
			appUiParaInit(PARA_INIT_FROM_UDF);
			//appUiParaInit(PARA_INIT_WITH_DEF);
			//appUDFSave();
			/*destroy udf,force  entering adf flow*/
			//sp1kNandRsvWrite(0x801, K_SDRAM_CommonEndAddr);
			break;
			
		#if SUPPORT_PANEL_PROTECT
		case SP1K_MSG_PANEL_PROTECT:
			appPanelAutoProt();
			break;
			
		#if 0
		case SP1K_MSG_SYS_CLOSE_LCD_BACKLIGHT:
			osMsgPost(SP1K_MSG_CMD_PANEL_OFF);
			break;

		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			osMsgPost(SP1K_MSG_CMD_PANEL_ON);
			break;
			#endif

		case SP1K_MSG_SYS_CLOSE_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(0);
			break;

		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(1);
			break;
			
		#endif
		
		case SP1K_MSG_UPDATE_AF_WINDOW:
			appCalAfOsdShow();
		default:
			//printf("Unknown message:%d\n", (UINT16)msg);
			break;
	}
}

UINT8 appUSBPc(UINT8 G_FuncID, UINT32* ArgBuf)
{
	switch(G_FuncID) {
		case 0xf1:
			appUSBDownloadISP(ArgBuf[0], ArgBuf[1], ArgBuf[2], (UINT16)ArgBuf[3]);
			break;

		case 0xf2:
			appUSBUploadFromDRAM(ArgBuf[0], ArgBuf[1]);
			break;

		case 0xf3:
			appUSBDownloadToDRAM(ArgBuf[0], ArgBuf[1]);
			break;

		case 0xf7:/*USB booter isp.bin bulk out*/
			//skip write DRAM
			sp1kHalSkipWrite2Dram();
			//printf("\nUSB BOOT !, size=0x%lx\n",ArgBuf[1]);
			appUSBDownloadToDRAM(ArgBuf[0], ArgBuf[1]);
			G_ISP = 1;
			break;

		default:
			break;
	}

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//L1_DownloadISP
//-----------------------------------------------------------------------------
/*
routine description:
	BulkOut setting for ISP function
arguments:
return value:
        0x00   - success
        others - error
*/
UINT8 appUSBDownloadISP(UINT32 ISPDRAMAddr, UINT32 ISPROMAddr, UINT32 ISPSecAddr, UINT16 ISPSecCnt) USING_0
{
	UINT8 status;

	//skip write DRAM
	sp1kHalSkipWrite2Dram();
	ISPDRAMAddr = ISPDRAMAddr;
	ISPSecAddr = ISPSecAddr;
	//G_BulkDRAMAddr = ISPROMAddr + 0x4000;
	G_BulkDRAMAddr = ISPDRAMAddr + 0x10000;
	G_BulkSize = ((UINT32)ISPSecCnt) << 8;
//	printf("L1_DownloadISP %lx %lx\n",G_BulkDRAMAddr, G_BulkSize);
	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;
	G_ISP = 1;
	ENTER_CRITICAL( status );
	/* Will be used inside isp.bin ,0x4000 is to have a free space*/
	//printf("L1_DownloadISP %lx G_BulkDRAMAddr=%lx %lx\n",ISPROMAddr, G_BulkDRAMAddr,G_BulkSize);
	SDRAM_CardSetStartAddress(ISPROMAddr, FALSE);
	M_DRAM_WriteWord((UINT8)G_BulkSize,(UINT8)(G_BulkSize >> 8));
 	M_DRAM_WriteWord((UINT8)(G_BulkSize >> 16),(UINT8)(G_BulkSize >> 24));
	M_DRAM_WriteWord((UINT8)G_BulkDRAMAddr,(UINT8)(G_BulkDRAMAddr >> 8));
 	M_DRAM_WriteWord((UINT8)(G_BulkDRAMAddr >> 16),(UINT8)(G_BulkDRAMAddr >> 24));
	sp1kHalUsbBulkEnable(0);

	return HALK_SUCCESS;
}
//-----------------------------------------------------------------------------
//appUSBDownloadToDRAM
//-----------------------------------------------------------------------------
/*
routine description:
	Download File to DRAM
arguments:
return value:
        0x00   - success
        others - error
*/
UINT8 appUSBDownloadToDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
{
	//printf("BulkOut DRAMAddr=%lx,BulkSize=%lx\n",BulkDRAMAddr,(UINT32)BulkSize);
	G_BulkDRAMAddr = BulkDRAMAddr;
	G_ISPDRAMAddr = BulkDRAMAddr;
	G_BulkSize = BulkSize;
	G_LastBulkSize = BulkSize;
	G_BulkFlashType = 0x00;
	//printf("L1_DownloadToDRAM %lx %lx\n",G_ISPDRAMAddr, G_BulkSize);
	//richie@0125
	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;
//	printf("L1_DownloadToDRAM %lx %lx\n",BulkDRAMAddr, BulkSize);
	sp1kHalUsbBulkEnable(0);

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//appUSBUploadFromDRAM
//-----------------------------------------------------------------------------
/*
routine description:
	Upload File From DRAM
arguments:
return value:
        0x00   - success
        others - error
*/
UINT8 appUSBUploadFromDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
{
	UINT16 BulkInSize;

	BulkInSize = 0x40;

	if (G_bUSBSpeed == 0x01) {
		BulkInSize = 0x200;
	}

	G_BulkDRAMAddr = BulkDRAMAddr;
 	G_BulkSize = BulkSize;
	G_BulkFlashType = 0x00;

	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;

	if (G_BulkSize<(BulkInSize + 1)) {
		sp1kHalBulkInFromDRAM(G_BulkDRAMAddr,(UINT16)G_BulkSize);
	}
	else {
		sp1kHalBulkInFromDRAM(G_BulkDRAMAddr,BulkInSize);
	}

	sp1kHalUsbBulkEnable(1);

	return HALK_SUCCESS;
}

