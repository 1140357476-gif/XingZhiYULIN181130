/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus mMedia *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno-*
 *  logy Co., Ltd. This software may only be used in accordance with the  *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.         *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd.                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "solution.h"
#include "app_main.h"
#include "os_msg.h"
#include "hal_gprm.h"
#include "app_init.h"
#include "app_msg.h"
#include "app_audio_play.h"
#include "app_still_play.h"
#include "app_audio_rec.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"
#include "gpio.h"

#include "app_ui_para.h"
#include "app_ui_osd.h"
#include "app_menu_micro.h"
#include "sp1k_pb_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_util_api.h"
#include "sp1k_video_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_rsvblk_api.h"
#include "video_pb.h"
#include "key_scan.h"
//#include "dos32.h"
#include "app_menu_tab.h"
#include "smcfunc.h"
#include "sp1k_gpio_api.h"
#include "app_dev_disp.h"


#if !TAG_TODO
#define HAL_ConfigPG(a, b, c , d, e, f)
#define HAL_StartPG(a)
#define HAL_StopPG(a)
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
static UINT8 audPbSpeed;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern	SIMAGE xdata pbFile;
extern xdata UINT8 voicePlayFlag;
extern UINT8 xdata  storageReadyflag;
//extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//appAudioPlay
//-----------------------------------------------------------------------------
void appAudioPlay (UINT16 msg) USING_0
{
	static UINT8 mode = 0;
	static UINT8 actEvent= EVENT_NULL;
//	UINT16 decWidth,decHeight;
	switch (msg) {
		case SP1K_MSG_STATE_INIT:
		//	printf("init audpb\n");
			appAudioPlayInit();
			break;
		case SP1K_MSG_KEY_PRESS_S1:

			break;
		case SP1K_MSG_KEY_PRESS_S2:
		//	printf("S2\n");

			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			if(sp1kAudioPbStatusGet() == SP1K_AUD_PB_IDLE) {
				osMsgPost(SP1K_MSG_STATE_TO_MENU);
			} else {
				sp1kAudioPbStop();
				//uiUpdateAudioStatus(1, AUD_PB_IDLE);//mantis #28067
				//appPbOsdInit();
				//osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			}
			break;
		case SP1K_MSG_KEY_PRESS_UP:
		case SP1K_MSG_MEDIA_PLAY_START:
			appAudioPlayOK();
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
			break;
		case SP1K_MSG_KEY_PRESS_LEFT:
		//	printf("left\n");
			appAudioPlayRWSpeedSet(appAudioPlaySpeedGet());
			uiUpdateAudioStatus(1, sp1kAudioPbStatusGet());
			break;
		case SP1K_MSG_KEY_RELEASE_LEFT:
			//audioPbKeyRelease();
			break;
		case SP1K_MSG_KEY_PRESS_RIGHT:
		//	printf("right\n");
			appAudioPlayFWSpeedSet(appAudioPlaySpeedGet());
			uiUpdateAudioStatus(1, sp1kAudioPbStatusGet());
			break;
		case SP1K_MSG_KEY_RELEASE_RIGHT:
		//	printf("rel right\n");
			//audioPbKeyRelease();
			break;
		case SP1K_MSG_KEY_PRESS_F1:
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			break;
		case SP1K_MSG_KEY_PRESS_TELE:
			break;
		case SP1K_MSG_KEY_PRESS_WIDE:
			break;
		case SP1K_MSG_KEY_PRESS_DISP:
			break;
		case SP1K_MSG_KEY_PRESS_PB:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appPbOsdInit();
				appAutoOffEn(1);
				mode = 1;
			}
			//osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			break;
		case SP1K_MSG_CARD_PLUG_IN:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
			}

			#if SUPPORT_PANEL_PROTECT
			app_set_lcd_backlight_status(1);
			#endif

			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_CARD_PLUG_IN);
			actEvent = EVENT_CARD_PLUG_IN;
			break;
		case SP1K_MSG_CARD_PLUG_OUT:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
			sp1kAudioPbStop();
			}
			actEvent = EVENT_CARD_PLUG_OUT;
			break;
		case SP1K_MSG_USB_PLUG_IN:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
			}
			//if(AdapterCharge)//  1: USB contect power adapter charge   0:USB contect PC
			//{
			//	break;
			//}

			if (storageReadyflag == SUCCESS) {
				actEvent = EVENT_USB_PLUG_IN;
			}
			break;
		case SP1K_MSG_USB_PLUG_OUT:
			actEvent = EVENT_USB_PLUG_OUT;
			break;
		case SP1K_MSG_TV_PLUG_IN:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appPbOsdInit();
				appAutoOffEn(1);
			}
			actEvent = EVENT_TV_PLUG_IN;

			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
			break;
		case SP1K_MSG_TV_PLUG_OUT:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appPbOsdInit();
				appAutoOffEn(1);
			}
			actEvent = EVENT_TV_PLUG_OUT;
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);//??????
			osMsgPost(SP1K_MSG_TV_PLUG_OUT);
			break;
		case SP1K_MSG_HDMI_PLUG_IN:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appAutoOffEn(1);
			}
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
			//appDispHdmiOpen();
			break;
		case SP1K_MSG_HDMI_PLUG_OUT:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appAutoOffEn(1);
			}
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_HDMI_PLUG_OUT);
			break;
		case SP1K_MSG_BAT_LV_UPDATE://2008-3-13 add for battery action
			uiUpdateBatteryLevel();
			appBattStsHandle();
			break;
		case SP1K_MSG_BAT_EMPTY:
			uiUpdateOSDBatEmpty();
			osMsgPost(SP1K_MSG_POWER_OFF);
			sp1kBtnDisableAll();//ready to power off, disable all button!
			break;
		case SP1K_MSG_POWER_OFF:
		case SP1K_MSG_KEY_PRESS_POWER:
			if(sp1kAudioPbStatusGet() != SP1K_AUD_PB_IDLE) {
				sp1kAudioPbStop();
				appPbOsdInit();
				uiUpdateAudioStatus(1, sp1kAudioPbStatusGet());//jintao.liu 2008-4-22
			}
			appPowerOffProc();
			break;
		case SP1K_MSG_MEDIA_SOUND_FINISH:
			appAutoOffEn(1);
			if(sp1kHDMI_PlugGet())
			{
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				break;
			}
			if(mode) {
				switch (appPbPreStateGet())
					{
						case APP_STATE_STILL_VIEW:
							msg = SP1K_MSG_STATE_TO_STILL_VIEW;
							break;
						case APP_STATE_VIDEO_VIEW:
							msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
							break;
						case APP_STATE_AUDIO_REC:
							sp1kAudioRecTypeSet(AUDIO_TYPE_NORMAL);
							msg = SP1K_MSG_STATE_TO_AUDIO_REC;
							break;
						default:
							msg = SP1K_MSG_STATE_TO_STILL_VIEW;
							break;
					}
					#if(TV_OPTION == TV_PLAY_OPTION)
						if(appPanelTypeGet() == 0) {
							osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
							osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
						} else {
							osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
							osMsgPost(msg);
						}
					#else
						#if SUPPORT_PANEL_PROTECT
						app_set_lcd_backlight_status(1);
						#endif
						
						osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
						osMsgPost(msg);
					#endif
					mode = 0;
			}else{
				//sp1kPbDispInit(K_SDRAM_AviPbBufAddrA, 320, 240);
				//mantis bug #42193,mark sp1kPbDispInit
				//sp1kPbDecodeSizeGet(&decWidth,&decHeight);
				//sp1kPbDispInit(K_SDRAM_AviPbBufAddrA, decWidth,decHeight); //mantis bug #42165 rhy++ 2011/03/16

				#if SUPPORT_PANEL_PROTECT
				app_set_lcd_backlight_status(1);
				#endif

				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				switch(actEvent) {
					case EVENT_NULL:
						break;
					case EVENT_CARD_PLUG_IN:
						osMsgPost(SP1K_MSG_CARD_PLUG_IN);
						break;
					case EVENT_CARD_PLUG_OUT:
						osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
						break;
					case EVENT_USB_PLUG_IN://2008-7-25 add for mantis #29145
						osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
						osMsgPost(SP1K_MSG_STATE_TO_USB);
						break;
					case EVENT_USB_PLUG_OUT:
						break;
					case EVENT_TV_PLUG_IN:
					case EVENT_TV_PLUG_OUT:
						//osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
						osMsgPost(SP1K_MSG_TV_PLUG_IN);
						break;
				}
				actEvent = EVENT_NULL;
			}
			//uiUpdateAudioStatus(1, sp1kAudioPbStatusGet());
			break;

		#if SUPPORT_PANEL_PROTECT
		case SP1K_MSG_PANEL_PROTECT:
			appPanelAutoProt();
			break;

		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(1);
			break;
		#endif
		
		default:
			//printf("Unknown message:%d\n", (UINT16)msg);
			break;
	}
}

void appAudioPlayInit(void) USING_0
{
	/* power control -- audio playback */
	//appModePowerCtrl(MODE_AUDIOPB);

	//audioPbGlobalInit();
#if 1
	appMediaPlayDispBufSet(sp1kPbDispBuffGet(1));
#else
	//appAudioBgShow(K_SDRAM_AviPbBufAddrA);
	//Mask by Phil@08.03.06
#endif
	osMsgPost(SP1K_MSG_MEDIA_PLAY_START);

	return;
}

void appAudioPlayPause(void) USING_0
{
	UINT8 sts;

	sts = sp1kAudioPbStatusGet();
//	printf("sts:%bx\n",sts);
	if(sts == SP1K_AUD_PB_PAUSE) {
		sp1kAudioPbResume();
	}
	else if(sts == SP1K_AUD_PB_RUN){
		sp1kAudioPbPause();
	}

	return;
}

void appAudioPlayOK(void) USING_0
{
	UINT8 speed;

	speed = sp1kAudioPbSpeedGet();
//	printf("speed  :%bx\n",speed);
	if(speed  == AUD_PB_SPEED_NORMAL) {
		if(sp1kAudioPbStatusGet() == SP1K_AUD_PB_IDLE) {
			sp1kAudioPbStart(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
			if(sp1kAudioPbStatusGet() == SP1K_AUD_PB_RUN) {
				appPbVideoOsdInit();
				uiUpdateAudPlayTime(0);
				appAutoOffEn(0);//jintao.liu 2008-4-29 auto power off
				//printf("sts=%bx\n",sp1kTV_PlugGet());
				if(pkgChipIdGet()== 0x1626)
				{
					if((0x20 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in or HDMI plug in
						sp1kHalSpeakerSet(0);
					}
					else {
						sp1kHalSpeakerSet(1);
					}
				}
				else
				{
					if((0x04 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in
						sp1kHalSpeakerSet(0);
					}
					else {
						sp1kHalSpeakerSet(1);
					}
				}
			}
			else {
				#if SUPPORT_PANEL_PROTECT
				app_set_lcd_backlight_status(1);
				#endif
				
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				return;
			}
		}
		else {
			appAudioPlayPause();
		}
	}
	else {
		sp1kAudioPbResume();
	}
	uiUpdateAudioStatus(1, sp1kAudioPbStatusGet());//jintao.liu 2008-4-22
}

void appMediaPlayDispBufSet(UINT32 addr) USING_0
{
	UINT16 width,height;
	UINT32 tmpAddr;

	return;//suny mark
	sp1kPbDecodeSizeGet(&width, &height);
#if 0
	printf("width:%d\n", width);
	printf("height:%d\n", height);
#endif
	if(width == 320 && height == 240) {
		if(addr > K_SDRAM_AviPbBufAddrA) {
			if((addr - K_SDRAM_AviPbBufAddrA) >= (UINT32)320 *(UINT32)240) {
				sp1kHalGprmCpy(addr, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
			}
			else {
				sp1kHalGprmCpy(addr, width, height, 0, 0, addr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, width, height,0, 1);
				sp1kDispVvalidWait(SP1K_DISP_SIGNAL_FALLING);
				sp1kPbBufAddrSet(addr + (UINT32)width *(UINT32)height *(UINT32)4);
				sp1kHalGprmCpy( addr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
			}
		}
		else {
			if(addr < K_SDRAM_AviPbBufAddrA) {
				if((K_SDRAM_AviPbBufAddrA - addr) >= (UINT32)320 *(UINT32)240) {
					sp1kHalGprmCpy(addr, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
				}
				else {
					sp1kHalGprmCpy(addr, width, height, 0, 0, K_SDRAM_AviPbBufAddrA + (UINT32)320 *(UINT32)240 *2, width, height, 0, 0, width, height,0, 1);
					sp1kPbBufAddrSet( K_SDRAM_AviPbBufAddrA + (UINT32)320 *(UINT32)240 *2);
					sp1kHalGprmCpy( K_SDRAM_AviPbBufAddrA + (UINT32)320 *240 *2, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
				}
			}
		}
	}
	else {
		if(width > 320) {
			tmpAddr = addr+ (UINT32)width * (UINT32)height ;
			sp1kHalGprmScale(addr, width, height,  tmpAddr , (UINT16)320, (UINT16)240, addr + (UINT32)width * (UINT32)height*(UINT32)2, 1);
			if(tmpAddr > K_SDRAM_AviPbBufAddrA) {
				if((tmpAddr - K_SDRAM_AviPbBufAddrA) >= (UINT32)320 *(UINT32)240) {
					sp1kHalGprmCpy(tmpAddr, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
					sp1kPbBufHsizeSet(320);
				}
				else {
					sp1kHalGprmCpy(tmpAddr, width, height, 0, 0, tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, width, height,0, 1);
					sp1kDispVvalidWait(SP1K_DISP_SIGNAL_FALLING);
					sp1kPbBufHsizeSet(320);
					sp1kPbBufAddrSet(tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4);
					sp1kHalGprmCpy( tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
				}
			}
		}
		else {
			tmpAddr = addr+ (UINT32)width * (UINT32)height ;
			sp1kHalGprmScale(addr, width, height,  addr + (UINT32)width * (UINT32)height , (UINT16)320, (UINT16)240, addr + (UINT32)width * (UINT32)height + (UINT32)320 *240, 1);
			if(tmpAddr > K_SDRAM_AviPbBufAddrA) {
				if((tmpAddr - K_SDRAM_AviPbBufAddrA) >= (UINT32)320 *(UINT32)240) {
					sp1kHalGprmCpy(tmpAddr, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
					sp1kPbBufHsizeSet(320);
				}
				else {
					sp1kHalGprmCpy(tmpAddr, width, height, 0, 0, tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, width, height,0, 1);
					sp1kDispVvalidWait(SP1K_DISP_SIGNAL_FALLING);
					sp1kPbBufHsizeSet(320);
					sp1kPbBufAddrSet(tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4);
					sp1kHalGprmCpy( tmpAddr + (UINT32)width *(UINT32)height *(UINT32)4, width, height, 0, 0, K_SDRAM_AviPbBufAddrA, width, height, 0, 0, width, height,0, 1);
				}
			}
		}
	}

	sp1kDispVvalidWait(SP1K_DISP_SIGNAL_FALLING);
	sp1kPbBufAddrSet(K_SDRAM_AviPbBufAddrA);
}

void appVoicePlay(UINT16 msg,UINT8 voiceType , UINT8 playLevel)
{
	uiPara_t* puiparaenv;
	UINT8 statevideoplay,stateaudioplay,statevideoaction,stateaudioaction;
	UINT8 index;
	UINT32 addr;

	puiparaenv=appUiParaGet();
	if(puiparaenv->Volume!=0) {
		#if SUPPORT_SPEAKER_CONTROL
		app_set_speaker(1);
		#endif

		
		
		switch(voiceType) {
			case VOICE_TYPE_BEEP:
				index = 0x13;
				addr = K_SDRAM_KeySoundAddr;
				if(puiparaenv->BeepSound==BEEP_ON) {
					//printf("beepvoice\n");
					if(playLevel) {
						appVoiceLoad(index ,addr);
						sp1kAudioPlay(addr);
					}
					else {
						if(voicePlayFlag) {
							statevideoplay=sp1kVideoPbStatusGet();
							stateaudioplay=sp1kAudioPbStatusGet();
							statevideoaction=sp1kVideoRecStatusGet();
							stateaudioaction=sp1kAudioRecStatusGet();
							if((stateaudioaction==AUD_REC_IDLE)&&(stateaudioplay==AUD_PB_IDLE)&&(statevideoplay==VID_PB_IDLE)&&(statevideoaction==VID_REC_IDLE)) {
								if( (msg>SP1K_MSG_KEY_PRESS_S2 && msg<SP1K_MSG_KEY_PRESS_MAX) && msg != SP1K_MSG_KEY_PRESS_POWER) {
									appVoiceLoad(index ,addr);
									sp1kAudioPlay(addr);
								}
							}
						}
					}
				}
				else {
					#if (_HW_SOLUTION_ == _HW_TK_MODEL_B_ || _HW_SOLUTION_ == _HW_RDK_A_)
					sp1kHalSpeakerSet(1);//mantis:27895
					#endif
				}
				break;
				
			case VOICE_TYPE_OPEN:
				addr = K_SDRAM_CommonEndAddr;
				switch(puiparaenv->StartupSound) {
					case SP1K_AUD_ITEM_OPEN_ONE:
						index=0xe;
						break;
					case SP1K_AUD_ITEM_OPEN_TWO:
						index=0x11;
						break;
					case SP1K_AUD_ITEM_OPEN_THREE:
						index=0x12;
						break;
					default:
						index=0;
						break;
				}
				if(index) {
					appVoiceLoad(index ,addr);
					sp1kAudioPlay(addr);
					//sp1kHalWait(5000);
				}
				break;
			case VOICE_TYPE_CLOSE:
				//printf("\n========================\n");
				//printf("power off sound\n");
				//printf("========================\n");
				
				index = 0x18;
				addr = K_SDRAM_CommonEndAddr;
				appVoiceLoad(index ,addr);
				sp1kAudioPlay(addr);
				//sp1kHalWait(2000);
				break;
			case VOICE_TYPE_SHUTTER:
				if(puiparaenv->ShutterSound == SHUTTER_ON) {
					addr = K_SDRAM_CommonEndAddr;
					index = 0x10;
					appVoiceLoad(index ,addr);
				}
				break;
			default:
				break;
		}
	}
}

//-----------------------------------------------------------------------------
//appVoicePreview
//-----------------------------------------------------------------------------
/**
 * @brief		appVoicePreview
 * @param	item 	: 	uper layer
 * @param	value 	: 	set value
 * @retval	None
 * @todo
 * @author	SunWei
 * @since	2008-03-13
 */
void appVoicePreview(UINT8 item,UINT8 value)
{
	UINT32 bufadd;
	uiPara_t* puipara;
	UINT8 index;

	puipara=appUiParaGet();
	switch(appPreStateGet(0)) {

		case APP_STATE_STILL_VIEW:
			bufadd=sp1kPvFreeBuffGet();
			break;
		case APP_STATE_STILL_PLAY:
			bufadd=sp1kPbFreeBuffGet();
			break;
		case APP_STATE_VIDEO_VIEW:
			//bufadd=sp1kVideoPvFreeAddrGet();
			bufadd=K_SDRAM_CommonEndAddr;	//mantis 43355
			break;
		default:
			bufadd=K_SDRAM_CommonEndAddr;
			break;
	}
	switch(item) {
		case 0:
			if(puipara->Volume!=0) {
				if(value==0) {
					index = 0x10;
					appVoiceLoad(index ,bufadd);
					sp1kAudioPlay(bufadd);
					//sp1kAudioVoicePlay(bufadd,0x10);
				}
			}
			break;
		case 1:
			if(puipara->Volume!=0) {
				switch(value) {
					case 1:
						index = 0xe;
						break;
					case 2:
						index = 0x11;
						break;
					case 3:
						index = 0x12;
						break;
					default:
						break;
				}
				if(value)
				{
					appVoiceLoad(index ,bufadd);
					sp1kAudioPlay(bufadd);
				}
			}
			break;
		case 2:
			if(puipara->Volume!=0) {
				if(value==0) {
					bufadd = K_SDRAM_KeySoundAddr;
					//appVoiceLoad(index ,bufadd);
					sp1kAudioPlay(bufadd);
				}
			}
			break;
		case 3:
			if(appPanelTypeGet() ==1){
				sp1kAudioVolumeSet(value);
			}
			else{
				sp1kAudioVolumeSet(value + 7);
			}
			if(value!=0){
				bufadd = K_SDRAM_KeySoundAddr;
				//appVoiceLoad(index ,bufadd);
				sp1kAudioPlay(bufadd);
			}
			break;
		default:
			//sp1kVoiceSet(2);
			break;
	}
}

void appVoiceLoad(UINT8 index ,UINT32 addr)
{
	sp1kNandRsvRead(index, (addr<< 1)); // read wav file from nandflash
}

void appBuzzer(void)
{
	//pwm0 or gpio7 function
	sp1kHalConfigPG(0,1,0,6000,6000,1);
	sp1kHalStartPG(0) ;
	sp1kHalWait(50);
	sp1kHalStopPG(0) ;
}

UINT8 appAudioPlaySpeedGet(void)
{
	UINT8 sts;

	sts = sp1kAudioPbStatusGet();
	if(sts == SP1K_AUD_PB_IDLE) {
		return APP_AUD_PB_SPD_NORMAL;
	}
	else if(sts == SP1K_AUD_PB_RUN){
		audPbSpeed = APP_AUD_PB_SPD2;
		return audPbSpeed;
	}
	else {
		switch(audPbSpeed) {
			case APP_AUD_PB_SPD_NORMAL:
				audPbSpeed = APP_AUD_PB_SPD2;
				break;
			case APP_AUD_PB_SPD2:
				audPbSpeed = APP_AUD_PB_SPD4;
				break;
			case APP_AUD_PB_SPD4:
				audPbSpeed = APP_AUD_PB_SPD8;
				break;
			case APP_AUD_PB_SPD8:
				audPbSpeed = APP_AUD_PB_SPD8;
				break;
			default :
				audPbSpeed = APP_AUD_PB_SPD_NORMAL;
				break;
		}
	}

	return audPbSpeed;
}

void appAudioPlayFWSpeedSet(UINT8 spd)
{
	UINT8 speed;

	speed = spd;
	if(sp1kAudioPbStatusGet() == SP1K_AUD_PB_RW) {
		speed = APP_AUD_PB_SPD2;
	}
	sp1kAudioPbFWSpeedSet(speed);

	return;
}

void appAudioPlayRWSpeedSet(UINT8 spd)
{
	UINT8 speed;

	speed = spd;
	if(sp1kAudioPbStatusGet() == SP1K_AUD_PB_FW) {
		speed = APP_AUD_PB_SPD2;
	}
	sp1kAudioPbRWSpeedSet(speed);

	return;
}

