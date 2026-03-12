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
 * @file      dev_init.c
 * @brief     Brief
 * @author    Phil Lin
 * @since     2011-02-17
 * @date      2011-02-17
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

#include "hal_device.h"
#include "hal_spi.h"
#include "func_ptr.h"

#include "dev_init.h"
#include "device.h"
#include "main.h"
#include "initio.h"
#include "timer.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "pb_general.h"

#include "sp1k_hal_api.h"
#include "sp1k_front_api.h"
#include "sp1k_gpio_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_util_api.h"
#include "sp1k_zoom_api.h"

#include "app_def.h"
#include "app_init.h"
#include "app_msg.h"
#include "app_battery.h"
#include "app_dev_disp.h"
#include "app_dev_storage.h"
#include "app_menu.h"
#include "app_menu_micro.h"
#include "app_ui_para.h"
#include "app_audio_play.h"
#include "app_gpio.h"
#include "app_sts_pwroff.h"
#include "sp1k_dos_api.h"
#include "app_cal_api.h"

#include "host_func_opt.h"
#include "initio.h"
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

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if SUPPORT_SENSOR_SWITCH
	// 1:大镜头
	// 0:自拍
	UINT8 sensor_switch = 1;
#endif

UINT8 enterMacroadjust = 0;

UINT8 enterVersionshow = 0;

UINT8 enterPCCamflag = 0;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern panelStatus_t G_PanelStatus;
extern UINT8 TV_Open;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT8 appAudioInit(void)
{
	uiPara_t *pPara = appUiParaGet();

	sp1kAudioVolumeSet(VOLUME_10);

	//add for improving audio rec
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_MIN_VOL,15); // 15
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_MAX_VOL,100);
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_DEFAULT_GAIN_IDX,34); // 38
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED,4);
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_GAIN_UP_SPEED,30);
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_OPT,1);	//ALC enable

	return SUCCESS;
}

/**
 * @fn        UINT8 appFrontInit(void)
 * @brief     appFrontInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appFrontInit(void)
{
	UINT8 sensorId, actId;

	sensorId = sp1kFrontSensorInit();
	sp1kFrontDevActSet(sensorId);
	sp1kFrontPowerOn(sensorId);

	actId = sp1kFrontDevActGet();

	if (SUCCESS == sp1kFrontHighQualityDigitalZoomCapabilityGet(actId)) {
		sp1kFrontHighQualityDigitalZoomModeSet(actId, 1);
	}
	else {
		sp1kFrontHighQualityDigitalZoomModeSet(actId, 0);
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 appZoomDevInit(void)
 * @brief     appZoomDevInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appZoomDevInit(void)
{
	sp1kZoomDevInit();


	if(VIDEO_NEW_ZOOM == 0 && STILL_NEW_ZOOM == 0)
	{
		sp1kZoomDevActSet(SP1K_ZOOM_LOGI_STILL_MODE,SP1K_ZOOM_LOGI_BLOCKING_TYPE);

		sp1kZoomDevActSet(SP1K_ZOOM_LOGI_VIDEO_MODE,SP1K_ZOOM_LOGI_BLOCKING_TYPE);
	}
	else
	{
		sp1kZoomDevActSet(SP1K_ZOOM_LOGI_STILL_MODE,SP1K_ZOOM_LOGI_NONBLOCKING_TYPE);

		sp1kZoomDevActSet(SP1K_ZOOM_LOGI_VIDEO_MODE,SP1K_ZOOM_LOGI_NONBLOCKING_TYPE);
	}


	return SUCCESS;
}


/**
 * @fn        UINT8 appDispInit(void)
 * @brief     appDispInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appDispInit(void)
{
	UINT8 pnlType, pnlSts;
	UINT16 decWidth, decHeight;
	uiPara_t *pPara = appUiParaGet();

	/*
		Panel driver logic install process
		The flow is NOT the physic driver, so the panel was still not been opened
	*/
	sp1kDispPanelInit();
	sp1kDispTvNtscInit();
	sp1kDispTvPalInit();
	sp1kDispHdmiInit();

	/* parioty: TV > HDMI > LCD */
	if (IS_GPIO_TV_PLUGIN()) {
		TV_Open = 1;
		if (pPara->TVMode == TVMODE_NTSC) {
			pnlType = DISP_DEV_NTSC;
			pnlSts  = PANEL_STATUS_NTSC;
		} else {
			pnlType = DISP_DEV_PAL;
			pnlSts  = PANEL_STATUS_PAL;
		}
	}
	/*The open image must be showed on the LCD while HDMI in.*/
	/* else if (IS_GPIO_HDMI_PLUGIN()) {
		pnlType = DISP_DEV_HDMI;
		pnlSts  = PANEL_STATUS_LCD;
	}*/
	else {
		pnlType = DISP_DEV_PANEL;
		pnlSts  = PANEL_STATUS_LCD;
	}

	/* panel physic open here */
	sp1kDispDevActSet(pnlType);
	sp1kDispPowerOn(pnlType);
	G_PanelStatus = pnlSts;

	if (pPara->OpenPic) {
		//sp1kHalJpegDecodeWait();
		if (IS_DISP_MODE_TVOUT()) {
			sp1kHalWait(500); //wait TV contect ready
		}
		sp1kPbDecodeSizeGet(&decWidth, &decHeight);
		sp1kDispImgShow(sp1kPbDispBuffGet(1), decWidth, decHeight, 100);
	}

	return SUCCESS;
}

/**
 * @fn        void appGpioInit(void)
 * @brief     appGpioInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
void appGpioInit(void)
{

	
	//only for 1527 debug @090520
	#if TAG_TODO
		#if(_HW_SOLUTION_ == _HW_TK_MODEL_B_ || _HW_SOLUTION_ == _HW_RDK_A_)
			//enable speaker
			sp1kSpeaker_ENCfg();
		#endif
	#endif

	if((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))
	{
		gpioPullSelSet(6,0xFF,0xFF);
		gpioPullEnSet(7,0xF0,0xF0);
		gpioPullEnSet(8,0x0F,0x0F);/*DGPIO0~7 pull high*/
	}

	sp1kSDWPDectCfg();

	sp1kTV_PlugCfg(TV_PLUG, 0);
	sp1kHDMI_PlugCfg();
	sp1kUSB_PlugCfg();
	
	//Add for card detect polarity seting
	//sp1kSD_CD_PolaritySet(0);
	sp1kSD_CdCfg();
	
	#if TAG_TODO
	//if slect LED function
	sp1kLED_SelfCfg();
	sp1kLED_SelfSet(1);
	///
	sp1kLED_BusyCfg();
	sp1kLED_BusySet(1);//turn off status led
	#endif
	
	appChargePinCfg();

	#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
	/*sensor power down*/
	gpioByteFuncSet(GPIO_BYTE_GEN2, 0x80, 0x80);
	gpioByteOutSet(GPIO_BYTE_GEN2, 0x80, 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN2, 0x80, 0x80);

	/*lcd BL ctrl gpio6*/
	gpioByteFuncSet(GPIO_BYTE_GEN0, 0x40, 0x40);
	gpioByteOutSet(GPIO_BYTE_GEN0, 0x40, 0x40);
	gpioByteDirSet(GPIO_BYTE_GEN0, 0x40, 0x40);
	#endif

	#if (_HW_SOLUTION_ == _HW_RDK_A_)
	#if TAG_TODO
		//lcd power
		sp1kGpioByteOutSet(GPIO_BYTE_P1, 0x80, 0x00);
		sp1kGpioByteDirSet(GPIO_BYTE_P1, 0x80, 0xff);

		//sensor power
		sp1kGpioByteOutSet(GPIO_BYTE_GEN2, 0x01, 0x00);
		sp1kGpioByteDirSet(GPIO_BYTE_GEN2, 0x01, 0xff);
	#endif
	#endif

	// GPIO 0 ：AF
	appAFCtrl(0);

	// GPIO 1 闪光灯
	appSnapFlashLed(0);
	
	// GPIO 2 
	#if SUPPORT_SENSOR_SWITCH
	appSensorPWDNSwitch(sensor_switch);
	#endif

	// GPIO 1 ?????
	
	// GPIO 3 RXD
	
	// GPIO 4 RXD/LED:???
	app_set_led(1);
	
	// GPIO 5 屏背光
	
	// GPIO 6 SPI写保护
	#if SUPPORT_SPI_WRITE_CONTROL
	appSetGpio6SpiWP(1);
	#endif
	
	// GPIO 7 POWER_HOLD

	// GPIO 8 USB检测
	
	// GPIO 8 功放使能
	#if SUPPORT_SPEAKER_CONTROL
	app_set_speaker(1);
	#endif	
	
	// GPIO 9 OK键
	
	// GPIO 10 看门狗
	#if SUPPORT_MCU_WTD
	appSetWTDHeartbeat(1);
	#endif

}

/**
 * @fn        void appRtcInit(void)
 * @brief     appRtcInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2011-5-16
 * @todo      N/A
 * @bug       N/A
*/
void appRtcInit(void){
	dateStc_t DateInfo;

	DateInfo.Year = 22;
	DateInfo.Month = 1;
	DateInfo.Day = 1;
	DateInfo.Hour = 0;
	DateInfo.Minute = 0;
	DateInfo.Second = 0;

	sp1kRTCDateInit(&DateInfo);
}

/**
 * @fn        void hostDevInit(void)
 * @brief     hostDevInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
void hostDevInit(void)
{
	uiPara_t *pPara;
	UINT16 adcVal = 0;
	UINT8 enterCardUpdateFlag = 0;

	// devInitPrintf("F/W ver : %s.%lu\n",   sp1kFWVerGet(),  sp1kSvnVerGet());
	// devInitPrintf("compiled@ %s, %s\n\n", sp1kFWDateGet(), sp1kFWTimeGet());

	/* GPIO initial */
	appGpioInit();
	sp1kHalWait(100);

	// 卡升级 电源键+MENU键 // 卡升级 电源键+MODE键
	#if 0
	sp1kADCSarSel(0x02);
	sp1kHalWait(50);
	adcVal = sp1kADCValueSnapIntr();
	//printf("----------adc val : %d\n", adcVal);
	//printf("----------adc val : %d\n", adcVal);
	//printf("----------adc val : %d\n", adcVal);
	sp1kHalWait(50);
	if ((adcVal >= (391 - 30) && adcVal <= (391 + 30))||(adcVal >= (336 - 30) && adcVal <= (336 + 30))){
		enterCardUpdateFlag = 1;
	}
	#endif
	
	//  微距调试模式 电源键 + 上键	//  微距调试模式 电源键 + AF键
	#if 0
	if ((adcVal >= (626 - 30) && adcVal <= (626 + 30))||(adcVal >= (587 - 30) && adcVal <= (587 + 30))){
		enterMacroadjust = 1;
	}
	#endif

	sp1kADCSarSel(0x00);
	sp1kHalWait(50);
	adcVal = sp1kADCValueSnapIntr();
	//printf("----------adc val : %d\n", adcVal);
	//printf("----------adc val : %d\n", adcVal);
	//printf("----------adc val : %d\n", adcVal);
	if ((adcVal >= (600 - 100) && adcVal <= (600 + 100)) && (HAL_GpioByteInGet(HAL_GPIO_BYTE_DISP0, 0x04, 0)) && (!sp1kUSB_PlugGet())){
		enterVersionshow = 1;
		//printf("enterVersionshow: %d\n", (UINT16)enterVersionshow);
	}else if ((adcVal >= (600 - 100) && adcVal <= (600 + 100)) && (HAL_GpioByteInGet(HAL_GPIO_BYTE_DISP0, 0x04, 0)) && (sp1kUSB_PlugGet())){
		enterPCCamflag = 1;
	}

	// 无USB时需要长按开机
	// USB DGPIO 2
	#if 1
	if (!sp1kUSB_PlugGet()) {
		// 电源键 adc 170
		sp1kADCSarSel(0x00);
		sp1kHalWait(50);
		adcVal = sp1kADCValueSnapIntr();
		if (adcVal >= (1023 - 100) && adcVal <= (1023 + 100)) {
			sp1kHalWait(50);
			adcVal = sp1kADCValueSnapIntr();
			if (!(adcVal >= (1023 - 100) && adcVal <= (1023 + 100))) {
				appPowerHoldPullDown();
			}
		} else {
			appPowerHoldPullDown();
		}
	}
	sp1kADCSarSel(0x00);
	#endif

	#ifdef DISK_DRV_SPI
		#if SUPPORT_SPI_WRITE_CONTROL
			appSpiWpSet(1);
		#else
			//spi host set WP call back function reg.
			sp1kSfCallbackReg(SP1K_SPI_CALLBACK_WP_SET, pFunc_SfWPSet);
		#endif
	#endif

	/* Load UDF */
	appUiParaInit(PARA_INIT_FROM_UDF);
	pPara = appUiParaGet();
	
	appUiItemOrderCfg();
	
	/* check the charge mode */
	appChargeModeChk();
	
	if(appChargeModeGet()) {
		/*
			background image in charge mode decode process
			we can't see it currently because the panel have not been opened.
		*/
		appImgDecode(0x38);
	} else { /* NOT in chare mode */
		/*
		   open image image decode process.
		   we can't see it currently because the panel have not been opened.
		*/
		appOpenImgDec(pPara->OpenPic);

		/*
			The first battery detection.
			The system will be power off if the battery low detected.
		*/
		appBattInit();

		/*
			power key check
			The system will be power off if there's NO power key pressed at this time.
		*/
		#if (_HW_SOLUTION_ != _HW_EVB_)
		if (!prjDbgMode) {
			if (sp1kBtnChk(SP1K_BTN_POWER) != KEY_STATUS_PRESSED) {
				appPowerShutDown();
			}
		}
		#endif
	}

	/* panel open */
	appDispInit();

	// 开机卡升级
	if (!sp1kSD_CdGet()) {
		appStorageMount(K_MEDIA_SDF);
		//appCardUpdateFW();
		#if SUPPORT_MCU_WTD
		appMCUWDTEnterCardISPMode();
		#endif
	} 
	
	#if 0
	else {
		pPara->ImageSize = PRV_SIZE_VGA;
		printf(">>>>>>>>>>>no card dos ram\n");
	}
	#endif

	/* audio open */
	appAudioInit();
	
	if(!appChargeModeGet()) {
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_OPEN, VOICE_PLAYLEVLE_HIGH);
		
		#if SUPPORT_MCU_WTD
		appSetWTDHeartbeat(0);
		#endif
		#if SUPPORT_MCU_WTD
		appSetWTDHeartbeat(1);
		#endif
		sp1kHalWait(907);  /*mantis 42008*/
	}
	sp1kAudioVolumeSet(pPara->Volume);

	/* front initial */
	appFrontInit();

	/* ZoomDev initial */
	appZoomDevInit();

	/* RTC DATE initial */
	appRtcInit();
}

/**
 * @fn        void appButtonInit(void)
 * @brief     appButtonInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
void appButtonInit(void) USING_0
{
	devAttr_t* pDevAttr;

	sp1kBtnDisableAll();

 	pDevAttr = sp1kDevHdlGet();
	pDevAttr[DEV_ID_KEYBROAD].devSts = DEV_STS_RDY;
	pDevAttr[DEV_ID_KEYBROAD].devEn = 1;
	pDevAttr[DEV_ID_KEYBROAD].devType = DEV_TYPE_KEYBROAD_COM;
}

/**
 * @fn        UINT8 appOpenImgDec(UINT8 mode)
 * @brief     appOpenImgDec
 * @param     [in] mode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appOpenImgDec(UINT8 mode) USING_0
{
	UINT32 resID;
	uiPara_t *pPara;
	
	pPara = appUiParaGet();

	if (mode == PB_OPENPIC_OFF) {
		resID = 0;
	} else if (mode == PB_OPENPIC_USER_DEF) {
		/* specify user defined startup image */
		resID = 0x800;
	} else {
		/* specify the default startup image */
			resID = 0x0d;
	}

	appImgDecode(resID);

	return SUCCESS;
}

/**
 * @fn        UINT8 appImgDecode(UINT32 resId)
 * @brief     appImgDecode
 * @param     [in] resId
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-11-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appImgDecode(UINT32 resId) USING_0
{
	UINT32 freeBuffAddr = K_SDRAM_TotalSize - 0x100000;// K_SDRAM_ImagePlaybackBufAddr;
	UINT16 decWidth = PB_DECODE_WIDTH;
	UINT16 decHeight = PB_DECODE_HEIGHT;

	sp1kPbDecodeSizeSet(decWidth, decHeight);
	sp1kPbDispBuffSwitch();

	if(resId) {
		appBGImgDec(resId, NULL, freeBuffAddr, sp1kPbDispBuffGet(1), 0);
	} else {
		sp1kPbFillBlack(sp1kPbDispBuffGet(1), decWidth, decWidth);
	}

	return SUCCESS;
}

/**
 * @fn        void devInit(void)
 * @brief     devInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-17
 * @todo      N/A
 * @bug       N/A
*/
void devInit(void)
{
	keyScanInit();

	appButtonInit();

	TIMER0_Start();

	appInit();
}

