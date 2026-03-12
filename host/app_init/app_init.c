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
#define __FILE		__FILE_ID_APP_INIT__
#include "general.h"
#include "initio.h"
#include "os_api.h"
#include "os_custom.h"
#include "app_main.h"
#include "app_init.h"
#include "os_msg.h"
#include "app_msg.h"
#include "main.h"
#include "audio.h"
#include "gpio.h"
#include "sp1k_bp_api.h"
#include "app_ui_para.h"
#include "app_menu.h"
#include "app_menu_micro.h"
#include "func_ptr.h"
#include "key_scan.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "sp1k_pv_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_fd_api.h"//wangjg++
#include "sp1k_rsvblk_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_front_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_cmd_api.h"
#include "sp1k_dcf_api.h"
#include "sp1k_pb_api.h"
#include "app_audio_play.h"
#include "app_battery.h"
#include "sp1k_vfs_api.h"
#include "dramfunc.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "sp1k_calibration_api.h"
#include "gpio_api.h"
#include "device.h"
#include "cdsp_init.h"
#include "sp1k_gpio_api.h"
#include "snap_task.h"
#include "app_sts_pwroff.h"
#include "dbg_def.h"
#include "app_init.h"
#include "hal_cdsp_iq.h"
#include "snap_core.h"
#include "sp1k_video_api.h"
#include "app_video_view.h"
#include "rsc_id.h"
#include "osd_mem.h"
#include "res_macro.h"
#include "host_func_opt.h"
#include "lsc_cal.h"
#include "app_still_view.h"
#include "Sp1k_dos_api.h "
#include "Sp1k_disk_api.h"
#include "sp1k_stamp_api.h"
#include "gui_api.h"
#include "gui_dbg.h"
#include "host_func_opt.h"
#include "initio.h"
#include "app_tp_scan.h"
#include "sp1k_video_capture.h"
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
UINT8 TV_Open=0;
extern  void appOsdColorSet(UINT8 attr);
extern xdata UINT8 G_ucMSDC_MLUN;

/**
 * @fn        void appCdspInit(void)
 * @brief     appCdspInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appCdspInit(void)
{
	sp1kCdspParaInit( SP1K_RSC_ID_CDSPPAR );

#ifndef COMPARE_ANSWER
	sp1kCdspGammaInit( SP1K_RSC_ID_GAMMA );
	sp1kCdspLscInit( SP1K_RSC_ID_LSC );
	sp1kCdspWdrInit( SP1K_RSC_ID_WDR );
	sp1kCdspPvParaInit( SP1K_RSC_ID_PVPAR );
	sp1kCdspPvDynamicParaInit(SP1K_RSC_ID_PVPAR);
	#if (EDGE_9_LEVEL_FUN == 0)
	sp1kCdspStillParaInit( SP1K_RSC_ID_STILLPAR );
	sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR);
	#else
	sp1kCdspStillParaInit( SP1K_RSC_ID_STILLPAR_MIDDLE );
	sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR_MIDDLE);
	#endif
	sp1kCDSPInit();
#endif

#ifdef MI_LSC_CAL_OPTION
	if(sp1kUSB_PlugGet()){
		if (sp1kBtnChk(SP1K_BTN_S2) == KEY_STATUS_PRESSED) {
			appSetMILSCCalSts(1);
			//printf("MI Cal EN!\n\n\n");
		}
	}
#endif

}

/**
 * @fn        void appVdSvcInit(void)
 * @brief     appVdSvcInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appVdSvcInit(void)
{
	/* awb initial */
	sp1kAwbInit();
	sp1kAwbGainAdjust(245, 256);//256

	/* AE initial */
	sp1kAeInit();
	sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 115);//100//120

	/* AE user meter call back initial */
	sp1kAeCallbackReg(SP1K_AE_CALLBACK_USER_DEFINE_METER,pFunc_AeUserDefineMeter);

	/* Set user metering  enable/disable */
	appUserMeterModeSet(0);

	/* check AE user meter enable */
	if (appUserMeterModeGet() == 1) {

		/* Set user metering */
		sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_USER_DEFINE);

		/* if need Dynamic user metering flow (host ctrl enable or disable) */
		osTaskCreate(pFunc_AeDynamicUserDefineMeter, OS_PRIO_DYMETER);
	}

	/* Face detection initial */
	sp1kFdInit();

	/* VD service initial */
	sp1kVdInit();

	appElecFreqSet();
}

/**
 * @fn        void appBpInit(void)
 * @brief     appBpInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appBpInit(void)
{
 	//UINT16 snapW, snapH;
	UINT32 addr;

	//sp1kFrontSnapSizeGet(0, &snapW, &snapH);

	/*preview*/
	addr = K_SDRAM_BadPixelCoordBufAddr << 1;
	if ( sp1kNandRsvRead(0x05, addr) == TRUE )
	{
		//HAL_CdspPvBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr);
		//HAL_CdspPvBpEnSet(1);
	}

	/*BP preview HD mode*/
	addr = (K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize_Upper + K_SDRAM_BadPixel_CapBufSize_Lower) << 1;
	if ( sp1kNandRsvRead(0xd6,addr) == TRUE )
	{
		//printf("load success!");
	}

	/*capture*/
	#if 1	/*work around MOE real time bad pixel compensation */
		addr =  (K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize)<<1;
		if ( sp1kNandRsvRead(0x19,addr) == TRUE)
		{
			//HAL_CdspStillBpCfg(0,0,0,snapW);
			//HAL_CdspStillBpEnSet(1);
			sp1kSnapParamSet(SP1K_SNAP_PARAM_BAD_PIXEL_OPT, SNAP_BP_OFFLINE);
			//printf("suc up bp rsc add=0x%lx\n",addr);
		}

		addr += (K_SDRAM_BadPixel_CapBufSize_Upper << 1);
		if ( sp1kNandRsvRead(0x1a,addr) == TRUE)
		{
			//printf("suc down bp rsc add=0x%lx\n",addr);
			//printf("success bp rsc add=0x%lx\n",( K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize) << 1);
		}

	#else
		if ( sp1kNandRsvRead(0x06,( K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize) << 1) == TRUE)
		{
			//HAL_CdspStillBpCfg(0,0,0,snapW);
			//HAL_CdspStillBpEnSet(1);
			sp1kSnapParamSet(SP1K_SNAP_PARAM_BAD_PIXEL_OPT, SNAP_BP_OFFLINE);
			//printf("success bp rsc add=0x%lx\n",( K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize) << 1);
		}
	#endif

}

/**
 * @fn        void appSnapInit(void)
 * @brief     appSnapInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appSnapInit(void)
{
	/* badpixel initial */
	appBpInit();

	/* Snap related function initial */
	sp1kSnapInit(K_SDRAM_CommonEndAddr, K_SDRAM_TotalSize);

	/* snap host burst check call back function reg. */
	sp1kSnapCallbackReg(SP1K_SNAP_CALLBACK_BURST, pFunc_SnapBurst);

	/* snap host load bg call back function reg. */
	sp1kSnapCallbackReg(SP1K_SNAP_CALLBACK_BACKGROUND_RES_LOAD, pFunc_SnapBackgroundResLoad);

	/* snap host stamp call back function reg. */
	sp1kSnapCallbackReg(SP1K_SNAP_CALLBACK_HOST_STAMP, pFunc_SnapHostStamp);

	/* date stamp loading */
	sp1kStampFontInit();
}

void appPbInit()
{
	sp1kPbCallbackReg( SP1K_PB_CALLBACK_ABORT, pFunc_pbAbort);
}


void appSpeedyCaptureInit()
{
	sp1kSpeedyCaptureCallbackReg( SP1K_SPEEDCAP_STILL, pFunc_VideoCap);
}

void appSpeedyCaptureRecInit()
{
	sp1kSpeedyCaptureCallbackReg( SP1K_SPEEDCAP_CLIP,pFunc_VideoCapRec);
}
/**
 * @fn        void appOsdInit(void)
 * @brief     appOsdInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
extern void menuBankGet(UIToolInfo*info);
void appUIToolInfoCfg(void)
{
	UIToolInfo info;
	menuBankGet(&info);
	//info.adfAddr = 0;
	//info.adfSize = sizeof(uiPara_t);
	info.strAddr = (info.currentBank+1)<<15;//32kbytes per bank, include common bank
	info.strIdxAddr = info.strAddr + info.strIdxAddr;
	info.paraAddr = ((info.paraAddr & 0x0000ffff) -0x8000) +info.strAddr;
	//info.adfAddr = 0;//(K_SDRAM_UsbAddr+512*4) << 1;
	//sp1kNandRsvSizeGet(0x0,NULL,&info.adfSize);
	//printf("\n\n\ninfo.adfSize=%ld\n",info.adfSize);
	//printf("straddr=0x%lx,0x%lx\nstridxaddr=0x%lx,0x%lx\n paraaddr=0x%lx,size=0x%lx\n\n\n",info.strAddr,
	//info.strSize,info.strIdxAddr,info.strIdxSize,info.paraAddr,info.paraSize);
	//dramFileSave("PARA", info.paraAddr>>1, info.paraSize);
	sp1kUIToolSet(0xff,0x00,0 , 			0x0,				0x0		,0);//init UITool
	sp1kUIToolSet(0,0x01, 0		, 		sizeof(uiPara_t),	0x0000	,0);//SP1K_MSG_DO_RESET);//adf.bin
	sp1kUIToolSet(1,0x02, 0			,	0,				0x0100	,0);//menu.bin
	sp1kUIToolSet(2,0x03, 0 			,	sizeof(uiPara_t),	0x0801	,SP1K_MSG_DO_RESET);//udf.bin
	sp1kUIToolSet(3,0x04, info.paraAddr, 	info.paraSize,		0xffff	,0);
	sp1kUIToolSet(4,0x05, info.strAddr, 		info.strSize,		0xffff	,0);
	sp1kUIToolSet(5,0x06, info.strIdxAddr, 	info.strIdxSize,	0xffff	,0);
	sp1kUIToolSet(6,0x10, 0			,	0,				0x0048	,0);//exif.bin
	sp1kUIToolSet(7,0x20, 0			,	0,				0x000c	,0);//CUSTOMIZATION\UsbStr.txt
	sp1kUIToolSet(8,0x30, 0			,	0,				0x000d	,0);//JPG\Open.jpg,open image
	sp1kUIToolSet(9,0x31, 0			,	0,				0x000f	,0);//JPG\Close.jpg,close image
	//sp1kUIToolSet(6,0x30, 0			,	0,				0x0801	,0);//JPG\Open.jpg,open image
	//sp1kUIToolSet(7,0x31, 0			,	0,				0x0801	,0);//JPG\Close.jpg,close image
	sp1kUIToolSet(10,0x32, 0			,	0,				0x000e	,0);//WAV\Open_1.wav(Res_0x0e.res)
	sp1kUIToolSet(11,0x33, 0			,	0,				0x0018	,0);//WAV\Close_1.wav(Res_0x18.res)

}
void appOsdInit(void)
{
	uiPara_t *puiPara = appUiParaGet();
	/* Osd related function initial */
	appUIToolInfoCfg();
	sp1kOsdInit(appPanelTypeGet());
	sp1kOsdLoad();
	sp1kOsdFontSizeSet(16, 32); /* Should be (16, 32), (12, 24), (8, 16) */
	sp1kOsdClear(0);
	#if FUNC_HOST_TOUCH_PANEL
	guiObjInit();
	guiDbgEnSet(0);
	guiObjOpModeSet(GUI_OP_MODE_PENDN_ONLY, 0);
	guiObjOpModeSet(GUI_OP_MODE_PENUP, MODE_PENUP_BY_PENDN);
	guiObjOpModeSet(GUI_OP_MODE_SLIDE_EN, 1);
	guiObjOpParaSet(GUI_OP_PARA_SLIDE_THD_X, 0);
	guiObjOpParaSet(GUI_OP_PARA_SLIDE_THD_Y, 48);
	appGuiEnableSet(1);
	tpCalDatLoad(0xfb, K_SDRAM_CommonEndAddr);
	#endif
	appOsdColorSet(MENU_STR_LOST_FOCUS);//MENU_STR_LOST_FOCUS
	menuDispParaInit();
	//appVersionShow();
	if(puiPara->OpenPic == PB_OPENPIC_OFF){
		sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);
	}
	sp1kHalCtrlScreen(3);
}
void appUsbScriptInit(){
	//cx++ for read usb Descriptor
	//printf("\n**@@@\n");
	sp1kNandRsvRead(0x1c, (K_SDRAM_UsbAddr<< 1)); //for usb1.1 configure descriptor
	//sp1kNandRsvRead(0x1b, K_SDRAM_CodeSize*2+(0xc000-0x4000)+10240); //for usb2.0 used  G_FPGAMemPool[0x3000] from 10k addr becase size>1024
	sp1kNandRsvRead(0x1b,( K_SDRAM_UsbAddr<< 1)+1024);
	   //cx++ end
}
/**
 * @fn        void appUsbInit(void)
 * @brief     appUsbInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-7
 * @todo      N/A
 * @bug       N/A
*/
void appUsbInit(void)
{
    /* Load usb customize strings */
	sp1kUsbStrSet(0x0c);
	 appUsbScriptInit();
}

/**
 * @fn        void appVersionShow(void)
 * @brief     appVersionShow
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appVersionShow(void)
{
	UINT8 strVer[128] = "VER.";
	UINT16 higth,width;
	//UINT16 strWidth;

	sp1kDispPnlDimGet(&higth,&width);
	strcpy(strVer+4 , sp1kFWVerGet());
	//strWidth=strlen(strVer);
	sp1kOsdClear(0);
	sp1kOsdStrDrawExt(width/2-40,higth-32,strVer,0);
}

/**
 * @fn        void appMemDosRamInit(void)
 * @brief     appMemDosRamInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
// #define DISK_DRV_DRAM
void appMemDosRamInit(void)
{
	UINT32 totSize = sysMemTotSizeGet();
	UINT32 size;
	UINT8 enable; /* the switch for dos ram function */

	enable = 1;

	switch (totSize) {
		case DEV_MEM_SIZE_16M:	size = 0;
		case DEV_MEM_SIZE_64M: 	size = 0;
			break;
		case DEV_MEM_SIZE_128M: size = 0x18000; // 0x9c000; // 0x400000; //4M Word
			break;
		case DEV_MEM_SIZE_256M: size = 0xC00000; //12M Word
			break;
		case DEV_MEM_SIZE_512M: size = 0x1400000; //20M Word
			break;
		default:                size = 0;
			break;
	}
	if (totSize <= DEV_MEM_SIZE_64M) {
		enable = 0;
	}

	#ifndef DISK_DRV_DRAM
	size = 0;
	enable = 0;
	#endif

	sp1kDosRamCfg(enable);
	sysMemSizeSet(MEM_IDX_DOS_RAM, size);
}

/**
 * @fn        void memInit(void)
 * @brief     mem Init
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
void memInit(void)
{
	/*
		NOTE: this function is called by function "main" firstly.
		Pls DO NOT add anything not important to this function!!!
	*/

	/* system memory block size initialization (Absolute) */
	osdSrcSize_t srcSize;
	srcSize.iconSize =(UINT32)ICON_FILE_SIZE;
	srcSize.FontSize = (UINT32)FONT_FILE_SIZE;
	srcSize.EngSize = (UINT32)ENGLISH_FILE_SIZE;
	srcSize.StrSize = (UINT32)STRING_FILE_SIZE;

	//printf("%lx,%lx,%lx,%lx\n",pSrcSize.iconSize,pSrcSize.FontSize,pSrcSize.EngSize,pSrcSize.StrSize);
	sp1kOsdSrcSizeSet(&srcSize);
	sysMemSizeInit();

	/* customize memory block size here */
	//sysMemSizeSet(MEM_IDX_DOS_RAM, 0);
	appMemDosRamInit();

	/* update the whole system memory address */
	sysMemAddrUpdate();
}

/**
 * @fn        void appInit(void)
 * @brief     appInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void appInit(void)
{
	/* OS task create */
	#if FUNC_HOST_TOUCH_PANEL
	osTaskCreate(pFunc_GuiProc, OS_PRIO_GUI);
	#endif
	osTaskCreate(pFunc_CardDetect, OS_PRIO_CARD);
	osTaskCreate(pFunc_TVDetect, OS_PRIO_TV);
	osTaskCreate(pFunc_BatteryChgDetect, OS_PRIO_BCHG);
	osTaskCreate(pFunc_CheckCountforTestUnitReady, OS_PRIO_CHECK_USB);

#ifdef MI_LSC_CAL_OPTION
	if(!appGetMILSCCalSts()) {
		osTaskCreate(pFunc_USBDetect, OS_PRIO_USB);
	}
	#else
	osTaskCreate(pFunc_USBDetect, OS_PRIO_USB);
	#endif

	if (sp1kDosRamStsGet() == TRUE)
	{
		sp1kDosRamBufSet(K_SDRAM_DosRamBufSize);
	}

	printf("IC Version: %c\n", XBYTE[0x20FF]? 'B':'A');

	/* cdsp initial */
 	appCdspInit();

	/* 3A/FD initial */
	appVdSvcInit();

	#if FUNC_HOST_IGBT_FLASH
	sp1kAwbModeSet(AWB_MODE_Flash);//4 //fan.xiao modified 2011-09-21 , 13:52:40  Add for flash
	#endif

	/* pb initial */
	appPbInit();

	/* snap initial */
	appSnapInit();

	/*speedyCapture initial*/
	appSpeedyCaptureInit();

	/*speedyCapture Record initial*/
	appSpeedyCaptureRecInit();

	sp1kdcfQuickInitSet(0);

	/* osd initial */
	appOsdInit();

	/* usb intial */
	appUsbInit();

	/* ready to switch to first host state */
	if ( appChargeModeGet() ) {
		osMsgPost(SP1K_MSG_STATE_TO_IDLE);
	}
	else {
		osMsgPost(SP1K_MSG_STATE_TO_POWER_ON);
	}
}
