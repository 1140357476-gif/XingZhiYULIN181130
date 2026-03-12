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
 * testttttttttt                                                                        *
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "solution.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_still_view.h"
#include "app_cmd.h"
#include "App_menu_micro.h"
#include "App_ui_osd.h"
#include "App_ui_para.h"
#include "Timer.h"
#include "func_ptr.h"
//#include "exifhdr.h"
#include "exif_def.h"//xl
#include "exiffunc.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_pwr_mgr.h"
#include "app_battery.h"

#include "app_menu_tab.h"
#include "app_menu_core.h"//6-11
#include "key_scan.h"//jintao.liu 2008-4-7 mask burst tele/wide for demo
#include "sp1k_osd_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_af_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_rsvblk_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_vfs_api.h"
#include "app_osd_icon.h"
#include "sp1k_aud_api.h"//jintao.liu 2008.05.05 for self snap beep
#include "sp1k_pb_api.h"
#include "sp1k_calibration_api.h"
#include "sp1k_exif_api.h"
#include "sp1k_cdsp_api.h"
#include "app_audio_play.h"
#include "uiflow.h"
#include "cardlink.h"//6-11
#include "doslink.h"
#include "cardlink.h"
#include "pb_general.h"
#include "app_still_play.h"
#include "pb_photoframe.h"
#include "sp1k_pb_api.h"
#include "app_menu_event.h"
#include "smcfunc.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"
#include "hal_dram.h"
#include "hal_gprm.h"
//#include "motion_det.h"
#include "sp1k_md_api.h"
#include "sp1k_disk_api.h"


#include "sp1k_pv_api.h"
#include "sp1k_front_api.h"
#include "macro_expand.h"
#include "dbg_def.h"
#include "sp1k_calibration_api.h"
#include "hal_front.h"
#include "sp1k_cdsp_api.h"
#include "app_ui_osd.h"
#include "sp1k_fd_api.h"
#include "osd_api.h"
#include "sp1k_dist_api.h"
#include "app_dev_disp.h"
#include "sp1k_dcf_api.h"
#include "app_video_view.h"
#include "app_init.h"
#include "app_menu.h"
#include "app_cal_api.h"
#include "sp1k_stamp_api.h"
#include "gpio_api.h"
#include "front_logi.h"
#include "rsc_id.h"
#include "sp1k_video_api.h"
#include "video_clip.h"
#include "video_capture.h"
#include "sp1k_video_capture.h"
#include "video_capture.h"
#include "sp1k_panorama_api.h"
#include "sp1k_gpio_api.h"


#define __FILE	  __FILE_ID_APP_STS_STILLVIEW__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define G_HSize_P K_Preview_Width
#define G_VSize_P K_Preview_Height

#define PV_3A_WAIT_CNT_MAX	((UINT16)10000) /* for error handle */

#define AWB_SPEED 2		//wangjg - invalid
#define AWB_SPEED_FAST_ONCE 255	//wangjg - invalid

#define STILL_ZOOM_FACTOR_LOOKUP_TABLE		0


//#define SCENE_SET_PRINT printf
#ifndef SCENE_SET_PRINT
	#define SCENE_SET_PRINT /_printf
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#ifdef AAA_ON
#define SNAP_WAIT_AEAWBREADY_OPTION
#else
#undef SNAP_WAIT_AEAWBREADY_OPTION
#endif
UINT16 stillZFctor=100;
#if (STILL_NEW_ZOOM==1)
UINT16 stillCurrLog=0;
#else
UINT16 stillCurrLog=100;
#endif

UINT32 count = 0;
UINT32 speedyCapRecTime = 0;
UINT8 affarflag = 0;
UINT8 versionclearflag = 0;

UINT8 afbackfaruiclearflag = 0;
	
extern UINT16 videoZFctor;
extern UINT16 videoCurrLog;

extern UINT8 flag_menu_back_videoview;

extern UINT8 enterMacroadjust;
extern UINT8 enterVersionshow;
extern UINT8 enterPCCamflag;


extern UINT8 cur_idx;
extern UINT16 snaplianan;

static UINT8 adjust_macro_sta = 0;
UINT8 adjust_macro_to_menu = 0;
UINT8 selfsnapbackfarflag = 0;
UINT8 snappanduan = 0;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//#if (STILL_NEW_ZOOM == 0)
#if (STILL_ZOOM_FACTOR_LOOKUP_TABLE == 1)
appPvZoom_t xdata PvZoomCtrl = {
	100,
	0,
	400,//zoom in 4X
	100,
	100,//zoom in 4X
	40,
	100,//zoom in 4X
	2,//4,//3*2,//zoom in 8X
};

//power (0.03333++, 1.6) curve
code UINT16 StillZoomFactor[31] = {
	100,
	101,
	103,
	107,
	111,
	117,
	122,
	129,
	136,
	143,
	151,
	160,
	169,
	178,
	188,
	198,
	209,
	220,
	232,
	244,
	256,
	269,
	282,
	296,
	309,
	324,
	338,
	353,
	368,
	384,
	400,
};
/* 8x
code UINT16 StillZoomFactor[61] = {

	100,
	101,
	103,
	105,
	109,
	113,
	117,
	122,
	127,
	133,
	139,
	146,
	153,
	160,
	168,
	176,
	184,
	193,
	201,
	211,
	220,
	230,
	240,
	250,
	261,
	272,
	283,
	295,
	306,
	318,
	330,
	343,
	356,
	368,
	382,
	395,
	409,
	422,
	437,
	451,
	465,
	480,
	495,
	510,
	526,
	541,
	557,
	573,
	589,
	606,
	622,
	639,
	656,
	673,
	691,
	709,
	726,
	744,
	763,
	781,
	800
};
*/
#else
appPvZoom_t xdata PvZoomCtrl = {
	100,					// factor
	0,						// step
	300,//zoom in 2X			// MaxFct
	100,					// MinFct
	100,//zoom in 2X			// maxLog
	40,						// minLog
	100,//zoom in 2X			// curLog
	5,//2,//4,//3*2,//zoom in 8X
};

code UINT16 StillZoomFactor[1]={0};
#endif
//#endif

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 userMeterEn;	/* default : 0 -> disable */
UINT8 userDefineMeter[8 * 8] = {
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
};

UINT8 aeWindowInformation[8 * 8] = {
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
};
static UINT8 S_imgsizeflag;
UINT8 S_imgRatioChg;
UINT8 imgResolutionSet;
//static UINT8 poweron = 0;
extern UINT8 xdata storageReadyflag;
//extern xdata UINT8 OsdMaxX;
//extern xdata UINT8 OsdMaxY;
extern UINT32 RemainPictureNO;
//extern xdata 	UINT32 	G_DOS_FileSize;
//extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
extern UINT8 AutoTestMode;
extern xdata UINT16 GlobleGain;
extern UINT8 ForceAEDo;
#if LIFE_SPAN_TEST
extern UINT8 snapRdy;
#endif
UINT8 isUpdateNum=0;
extern xdata UINT8 G_calibrationMode;
extern xdata UINT8 calibrationAEOn;
static UINT8 xdata pvZoomSts;
UINT16 xdata pvWaitCnt;

UINT8 xdata G_SceneMode = 0;
//UINT8 afdoingflag = 0;
//UINT8 afdoingsnapflag = 0;

#if MSG_AUTO_SEND
extern UINT8
frontAeTest(
	UINT8 id,
	UINT8 userIdx
);
#endif
UINT8 appStillViewAEAWBWait(void);
UINT8 appSnapEnvChk(void);
UINT8 appSnapParaLoad(void);
UINT8 appSnapPreProc(void);
UINT8 appSnapPostProc(UINT16 msg);
UINT8 menuDistortionSet (UINT8 layer, UINT8 item, UINT16 msg);

#if (FUNC_HOST_AF_FIXTURE == 1)
UINT8 appAfWinSet(UINT8 En);
#endif

/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
extern xdata UINT8 exifVlcAtOdd;
void appPvPhotoFrameRecover(void);
UINT8 appPvPhotoFrameResLoad(UINT8 idx, UINT16* resW, UINT16* resH);
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */


// still view��ʼ�����֮���ٿ�ʼ��ʾ���ͼ��
// ��������ͷ���滹û�г�����ʱ�򣬳��ͼ�������ϲ���ȷ��λ�ÿ�ʼ��ʾ
UINT8 showChargingIcon = 0;

UINT8 press_snap_check_flag = 0;

UINT8 key_repeate_check_flag = 0;

UINT8 key_enter_AF_menu_flag = 0;

/*
	�밴���ռ�ʱ����AF��ʱ
	300�����Ժ����û��ȫ�����ռ�����жԽ�
	����ִ�жԽ�
*/
UINT8 af_timer_start = 0;
UINT8 af_timer_out = 0;

#if SUPPORT_AWB_ADJ
UINT8 awb_auto_disable_flag = 0;
UINT8 awb_adj_start_flag = 0;
#endif


extern xdata UINT8 AdapterCharge; //  1: USB contect power adapter charge   0:USB contect PC
extern UINT8 power_up_usb_check_flag;










/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/********************Function uiShowOSDSelfSnapCntDown **************/
//Name:				void uiShowOSDSelfSnapCntDown(void)
//Description:			Show the Self Snap count down time
//Input:				UINT8
//					countDown:	count down time
//					enableShow:	show time or not
//Output:			none
//Created By:		Wei.Sun
//Created Time:		08-03-03 14:48
//Last Modify Time:	08-03-06 15:36
/*****************************************************/
extern UINT16 LCD_WIDTH;
extern UINT16 LCD_HEIGHT;
extern UINT16 MENU_CHAR_WIDTH;
extern UINT16 MENU_CHAR_HEIGHT;
extern UINT8 cur_idx;

extern void app_quick_delete_dialog(UINT8 cur_idx);

void app_adjust_macro_sta_set(UINT8 sta) {
	adjust_macro_sta = sta;
}

UINT8 app_adjust_macro_sta_get() {
	return adjust_macro_sta;
}

void uiShowOSDSelfSnapCntDown(UINT8 countDown,UINT8 enableShow)
{
	UINT8 str[3]="00";
	//UINT8 OsdMaxX, OsdMaxY;

	//sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	countDown=countDown&0x7f;
	str[0]=countDown/10+48;
	str[1]=countDown%10+48;
	sp1kOsdClearCharExt(128, 88, 64, 64);//����Խ���
	if(strWidthGet(str)<strWidthGet("44"))
	{
		sp1kOsdClearCharExt(LCD_WIDTH/2-MENU_CHAR_WIDTH+strWidthGet(str),LCD_HEIGHT/2,strWidthGet("44")-strWidthGet(str),MENU_CHAR_HEIGHT);
	}
	if(enableShow)
	{
		//osdStrDraw((OsdMaxX/2-2), OsdMaxY /2, str, (1<<4)|(1<<3));
		//sp1kOsdStrDraw((OsdMaxX/2 - 1), OsdMaxY /2, str, 0);
		sp1kOsdStrDrawExt(LCD_WIDTH/2-MENU_CHAR_WIDTH,LCD_HEIGHT/2,str,0);
	}
	else
	{
		//sp1kOsdClearChar((OsdMaxX/2 - 1), OsdMaxY /2, 2, 1);
		sp1kOsdClearCharExt(LCD_WIDTH/2-MENU_CHAR_WIDTH,LCD_HEIGHT/2,strWidthGet("44"),MENU_CHAR_HEIGHT);
	}
}

/********************Function appSelfSnapMSGSend **************/
//Name:				void appSelfSnapMSGSend(void)
//Description:			count down the time and when times up send
//					the self snap msg to snap
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-03 14:48
//Last Modify Time:	08-03-06 15:36
/*****************************************************/
static UINT16 selfSnapCountDown = 0;

void appSelfSnapMSGSend(void)
{
	uiPara_t* uiEnviroment;
	UINT16 selfTime;
	
//	static UINT16 cnt=1000;
//	static UINT16 tmp0;

#if 1 // sunyong mark to solve selfsnap issue


	if((0 == appSDPlugStatusGet()) && (sp1kDiskTypeGet() == DEV_TYPE_DISK_SPI))
	{
		sp1kTimer1Stop();
		selfSnapCountDown = 0;
		sp1kBtnEnableAll();
		return;
	}

	if (!selfSnapCountDown) {
		// selfsnap configure
		uiEnviroment = appUiParaGet();


		switch (uiEnviroment->DriverMode) {
		case PRV_SNAP_SINGLE:
		case PRV_SNAP_BURST:
			selfTime = 0;
			break;
			
		case PRV_SNAP_SELF_2S:
		case PRV_SNAP_CYC_2S:
			selfTime = 2;
			break;

		case PRV_SNAP_SELF_5S:
		case PRV_SNAP_CYC_5S:
			selfTime = 5;
			break;
			
		case PRV_SNAP_SELF_10S:
		case PRV_SNAP_CYC_10S:
			selfTime = 10;
			break;
		
		}
		
		selfSnapCountDown = selfTime * 5;
		uiShowOSDSelfSnapCntDown(selfSnapCountDown / 5,1);
		osMsgPost(SP1K_MSG_SELFSNAP_BEEP);
		return;
	}

	if(!( --selfSnapCountDown)) {
		//ready to snap
		selfSnapCountDown=0;
		uiShowOSDSelfSnapCntDown(selfSnapCountDown / 5, 0);
		sp1kTimer1Stop();
		osMsgPost(SP1K_MSG_KEY_PRESS_SELFSNAP);
	} else {
		if (!(selfSnapCountDown % 5)) {
			uiShowOSDSelfSnapCntDown(selfSnapCountDown / 5, 1);
			osMsgPost(SP1K_MSG_SELFSNAP_BEEP);
			//appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
		} else if (selfSnapCountDown <= 2 * 5) {
			osMsgPost(SP1K_MSG_SELFSNAP_BEEP);
			//appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
		}
	}
	//printf("s:%02x\n",selfSnapCountDown);
#else//sunyong @2008.6.23 add for mantis:25742 begin
	if(!selfSnapCountDown){//
		sp1kTimer1Stop();
		uiEnviroment = appUiParaGet();
		if(uiEnviroment->DriverMode==PRV_SNAP_SELF_10S) {
			selfTime = 10*(UINT16)1000;
			tmp0 = 2*(UINT16)1000;
			sp1kTimer1Config((UINT32)cnt, pFunc_SelfSnapMSGSendShell,NULL);
		}else if(uiEnviroment->DriverMode==PRV_SNAP_SELF_2S){
			selfTime = 2*(UINT16)1000;
			tmp0 = 1*(UINT16)1000;
			cnt=500;
			sp1kTimer1Config((UINT32)cnt, pFunc_SelfSnapMSGSendShell,NULL);
		}else{
			selfTime = 0;
		}
		selfSnapCountDown = selfTime;
		sp1kTimer1Start();
		return;
	}

	uiShowOSDSelfSnapCntDown(selfSnapCountDown/1000,1);
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
	sp1kLED_SelfSet(0);
	selfSnapCountDown -= cnt;
	if(selfSnapCountDown == tmp0){//change  frequency
		if(selfTime == 10*(UINT16)1000){
			cnt = 500;
		}else{
			cnt = 200;
		}
		sp1kTimer1Stop();
		sp1kTimer1Config((UINT32)cnt, pFunc_SelfSnapMSGSendShell,NULL);
		sp1kTimer1Start();
	} else if(selfSnapCountDown<=300) {//post capture mssage
		sp1kTimer1Stop();
		selfSnapCountDown=0;
		uiShowOSDSelfSnapCntDown(selfSnapCountDown, 0);
		osMsgPost(SP1K_MSG_KEY_PRESS_SELFSNAP);
		cnt=1000;
	}
	sp1kHalWait(50);//The time must less than "cnt". specify LED turn on time
	sp1kLED_SelfSet(1);

#endif //sunyong @2008.6.23 add for mantis:25742 end
}


void appSelfSnapHandle(void)
{
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST,VOICE_TYPE_BEEP, VOICE_PLAYLEVLE_HIGH);
#ifndef WHITE_LED
	sp1kLED_SelfSet(0);
#endif
	sp1kHalWait(100);
#ifndef WHITE_LED
	sp1kLED_SelfSet(1);
#endif
}
/********************Function appDriveModeSet **************/
//Name:				UINT8 appDriveModeSet(void)
//Description:			Set the still snap parament based on driver mode
//Input:				None
//Output:			UINT8
//					Success:Set ok!
//					Fail:	In selfSnap it means wait for times up
//						In Others it means some error happened
//Created By:		Wei.Sun
//Created Time:		08-03-03 11:33
//Last Modify Time:	08-03-06 15:40
/*****************************************************/
void appStillPreviewReset(void) {
	UINT16 frontHSize,frontVSize;
	UINT16 cropVSize;
	UINT16 cropHsize;
	uiPara_t* puiPara ;
	UINT16 pvHSize,pvVSize;
	UINT16 snapXSize,snapYSize;
	UINT8 ret;

	//sp1kDispImgWinEn(0);

	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);

	sp1kSnapSizeGet(&snapXSize,&snapYSize);

	pvHSize = K_Preview_Width;
	pvVSize = K_Preview_Height;

	#if SUPPORT_QVGA_SENSOR
		if(frontHSize*3 > frontVSize*4){/*sensor size 16:9*/
			cropHsize = frontHSize;
			cropVSize = frontVSize;
		}else{
			cropHsize = frontHSize;
			cropVSize = frontVSize;
		}

		if ( snapXSize*3 > snapYSize*4 ) {
			/* snap size 16:9*/
			pvVSize = pvVSize*3/4;

			//if(frontHSize*3 > frontVSize*4){/*sensor is 16:9*/
			//	cropHsize = frontHSize;
			//	cropVSize = frontVSize;
			//}
			//else{
			cropVSize = cropVSize*3/4;
			//}
		}
	#else
		/* sensor ratio */
		if ( frontHSize*3 > frontVSize*4 ) {
			/*sensor size 16:9*/
			cropHsize = frontVSize*4/3;
			cropVSize = frontVSize;
		}
		else {
			/*sensor size 4:3*/
			cropHsize = frontHSize;
			cropVSize = frontVSize;
		}
		if ( snapXSize*3 > snapYSize*4 ) {
			/* snap size 16:9*/
			pvVSize = pvVSize*3/4;

			if(frontHSize*3 > frontVSize*4){/*sensor is 16:9*/
				cropHsize = frontHSize;
				cropVSize = frontVSize;
			}
			else{
				cropVSize = cropVSize*3/4;
			}
		}
	#endif

//	printf("H(%u),V(%u),CH(%u),CV(%u),frontHSize(%u),frontVSize(%u)\n",snapXSize,snapYSize,cropHsize,cropVSize,frontHSize,frontVSize);
	sp1kPreviewCropSizeSet(cropHsize,cropVSize);
	#if (STILL_NEW_ZOOM == 1)
	sp1kPvZoomCtrlParaSet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG,stillCurrLog);
	#else
	PvZoomCtrl.factor=stillZFctor;
	PvZoomCtrl.curLog = stillCurrLog;
	sp1kPvZoomSet(1, PvZoomCtrl.factor, 0);
	#endif
	#if 0
	#if (STILL_NEW_ZOOM == 0)
	sp1kPreviewSet(0, pvHSize, pvVSize, PvZoomCtrl.factor, 0);	/* will: still 320x240*/
	#else
	sp1kPreviewSet(0, pvHSize, pvVSize, stillZFctor, 0);	/* will: still 320x240*/
	#endif
	#endif
	sp1kPreviewSet(0, pvHSize, pvVSize, sp1kPvZoomFctGet(), 0);	/* will: still 320x240*/
	//sp1kCalibrationLscSet(1);
	sp1kHalCdspImgSrcSel(0);	// 0: image source from front

	// HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 4);

	// sp1kDispImgWinEn(1); // hlc, Mantis #0021510
}


UINT8 appDriveModeSet(void)
{
	uiPara_t* puiEnviroment;

	puiEnviroment=appUiParaGet();

	switch (puiEnviroment->DriverMode) {
	case PRV_SNAP_SINGLE:
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
		break;

	case PRV_SNAP_BURST:
		if(RemainPictureNO<3)
		{
			uiUpdateOSDMemFull();
			isUpdateNum=1;
			appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
			return FAIL;
		}
		else
		{	
			sp1kSuperPreviewSet(0,0);
			appStillPreviewReset();
			sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 1 );
			//sp1kSnapParamSet( SP1K_SNAP_PARAM_DOFLOW_OPT, 1 );//burest mode
			return SUCCESS;
		}
		break;

	case PRV_SNAP_SELF_2S:
	case PRV_SNAP_SELF_5S:
	case PRV_SNAP_SELF_10S:
		if(RemainPictureNO==0)
		{
			uiUpdateOSDMemFull();
			isUpdateNum=1;
			appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		}
		else
		{
			sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
			
			sp1kBtnDisableAll();

			
			appOsdSelfSnapEnableOsd(1);
			pFunc_SelfSnapMSGSendShell();
			sp1kTimer1Config(200, pFunc_SelfSnapMSGSendShell,NULL);
			sp1kTimer1Start();
		}
		return FAIL;
		break;

	case PRV_SNAP_CYC_2S:
	case PRV_SNAP_CYC_5S:
	case PRV_SNAP_CYC_10S:
		if(RemainPictureNO==0)
		{
			uiUpdateOSDMemFull();
			isUpdateNum=1;
			appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		}
		else
		{
			sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
			
			sp1kBtnDisableAll();

			
			appOsdSelfSnapEnableOsd(1);
			pFunc_SelfSnapMSGSendShell();
			sp1kTimer1Config(200, pFunc_SelfSnapMSGSendShell,NULL);
			sp1kTimer1Start();
		}
		return FAIL;
		break;
	}

	return SUCCESS;
}


/********************Function appEffectSet ***************/
//Name:				UINT8 appEffectSet(void)
//Description:			Set Effect parament based on Effect
//Input:				UINT8
//					effect
//Output:			boolean
//					Success:Set ok!
//					Faile:Some Error must be happened!
//Created By:		Wei.Sun
//Created Time:		08-03-03 13:41
//Last Modify Time:	08-03-06 15:40
/*****************************************************/
UINT8 appEffectSet(UINT8 effect)
{
	switch(effect)
	{
		case PRV_EFFECT_NORNAL:
			sp1kCdspEffect_Normal();
			sp1kAeNegativeEnSet(0);
			break;
		case PRV_EFFECT_BLACKWHITE:
			sp1kCdspEffect_BlackWhite();
			sp1kAeNegativeEnSet(0);
			break;
		case PRV_EFFECT_SEPIA:
			sp1kCdspEffect_Sepia();
			sp1kAeNegativeEnSet(0);
			break;
		case PRV_EFFECT_NEGATIVE:
			sp1kCdspEffect_Negative() ;
			sp1kAeNegativeEnSet(1);
			break;
		case PRV_EFFECT_RED:
			sp1kCdspEffect_Red() ;
			sp1kAeNegativeEnSet(0);
			break;
		case PRV_EFFECT_GREEN:
			sp1kCdspEffect_Green() ;
			sp1kAeNegativeEnSet(0);
			break;
		case PRV_EFFECT_BLUE:
			sp1kCdspEffect_Blue() ;
			sp1kAeNegativeEnSet(0);
			break;
		default:
			sp1kCdspEffect_Normal();
			sp1kAeNegativeEnSet(0);
			return FAIL;
	}
	//sp1kAwbParamSet(AWB_PARA_DarkSatMode, effect==PRV_EFFECT_NORNAL? AWB_FUNC_Enable : AWB_FUNC_Disable);
	return SUCCESS;
}

UINT8 appContrastSet(UINT8 ContrastValue)
{
	switch(ContrastValue)
	{
		case PRV_CONTRAST_HIGH:
			sp1kCdspContrastEnhanSet(1,0x28);// 1.25x
			break;

		case PRV_CONTRAST_MIDDLE:
			sp1kCdspContrastEnhanSet(1,0x20);// 1x
			break;

		case PRV_CONTRAST_LOW:
			sp1kCdspContrastEnhanSet(1,0x18);// 0.75x
			break;

		case PRV_CONTRAST_OFF:
			sp1kCdspContrastEnhanSet(0,0x20);// 1x
			break;

		default:
			sp1kCdspContrastEnhanSet(1,0x20);// 1x
			break;
	}

	return SUCCESS;
}

UINT8 appSaturationSet(UINT8 SaturationValue)
{
	switch ( SaturationValue ) {
		case PRV_SATURATION_HIGH:
			sp1kCdspSaturationEnhanSet(1,0x2E,0x00,0x00);//0x28// 1.25x
			break;

		case PRV_SATURATION_MIDDLE:
			sp1kCdspSaturationEnhanSet(1,0x2E,0x00,0x00);//0x20// 1x
			break;

		case PRV_SATURATION_LOW:
			sp1kCdspSaturationEnhanSet(1,0x2E,0x00,0x00);//0x18// 0.75x
			break;

		default:
			sp1kCdspSaturationEnhanSet(1,0x2E,0x00,0x00);//0x20// 1x
			break;
	}

	return SUCCESS;
}

UINT8 appSharpnessSet(UINT8 SharpnessValue)
{
	switch ( SharpnessValue ) {
		case PRV_EDGE_SOFT:
			#if (EDGE_9_LEVEL_FUN == 0)
			sp1kCdspSharpnessEnhanSet(CDSP_DYNAMIC_DISABLE,CDSP_DYNAMIC_WEAK);
			#else
			sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR_WEAK);
			#endif
			break;

		case PRV_EDGE_NORMAL:
			#if (EDGE_9_LEVEL_FUN == 0)
			sp1kCdspSharpnessEnhanSet(CDSP_DYNAMIC_DISABLE,CDSP_DYNAMIC_MIDDLE);
			#else
			sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR_MIDDLE);
			#endif
			break;

		case PRV_EDGE_HARD:
			#if (EDGE_9_LEVEL_FUN == 0)
			sp1kCdspSharpnessEnhanSet(CDSP_DYNAMIC_DISABLE,CDSP_DYNAMIC_STRONG);
			#else
			sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR_STRONG);
			#endif
			break;

		default:
			#if (EDGE_9_LEVEL_FUN == 0)
			sp1kCdspSharpnessEnhanSet(CDSP_DYNAMIC_ENABLE,CDSP_DYNAMIC_WEAK);
			#else
			sp1kCdspStillDynamicParaInit( SP1K_RSC_ID_STILLPAR_WEAK);
			#endif
			break;
	}

	return SUCCESS;
}


UINT8 appSceneSet(UINT8 scene)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 hueResId[3]={0xe0,0xe1,0xe2};

	switch ( scene ) {
		default:
		case PRV_SCENE_AUTO:
			SCENE_SET_PRINT("auto\n");
			G_SceneMode = PRV_SCENE_AUTO;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_MIDDLE;
			puiPara->Contrast = PRV_CONTRAST_OFF; // PRV_CONTRAST_MIDDLE;
			break;
		case PRV_SCENE_LANDSCAPE:
			SCENE_SET_PRINT("landscap\n");
			G_SceneMode = PRV_SCENE_LANDSCAPE;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_HARD;
			puiPara->Saturation = PRV_SATURATION_HIGH;
			puiPara->Contrast = PRV_CONTRAST_HIGH;
			hueResId[0] = 0xda;
			hueResId[1] = 0xdb;
			hueResId[2] = 0xdc;
			break;
		case PRV_SCENE_NIGHTSCENERY:
			SCENE_SET_PRINT("night_scene\n");
			G_SceneMode = PRV_SCENE_NIGHTSCENERY;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_100;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_MIDDLE;
			puiPara->Contrast = PRV_CONTRAST_MIDDLE;
			break;
		case PRV_SCENE_PORTRAIT:
			SCENE_SET_PRINT("portrait\n");
			G_SceneMode = PRV_SCENE_PORTRAIT;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_ON;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_HIGH;
			puiPara->Contrast = PRV_CONTRAST_MIDDLE;
			hueResId[0] = 0xdd;
			hueResId[1] = 0xde;
			hueResId[2] = 0xdf;
			break;
		case PRV_SCENE_NIGHTPORTRAIT:
			SCENE_SET_PRINT("night_portrait\n");
			G_SceneMode = PRV_SCENE_NIGHTPORTRAIT;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_ON;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_MIDDLE;
			puiPara->Contrast = PRV_CONTRAST_MIDDLE;
			break;
		case PRV_SCENE_BEACH:
			SCENE_SET_PRINT("beach\n");
			G_SceneMode = PRV_SCENE_BEACH;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_DAYLIGHT;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_HARD;
			puiPara->Saturation = PRV_SATURATION_HIGH;
			puiPara->Contrast = PRV_CONTRAST_HIGH;
			hueResId[0] = 0xd7;
			hueResId[1] = 0xd8;
			hueResId[2] = 0xd9;
			break;
		case PRV_SCENE_SPORT:
			SCENE_SET_PRINT("sport\n");
			G_SceneMode = PRV_SCENE_SPORT;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_800;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_MIDDLE;
			puiPara->Contrast = PRV_CONTRAST_MIDDLE;
			break;
		case PRV_SCENE_DINNER:
			SCENE_SET_PRINT("dinner\n");
			G_SceneMode = PRV_SCENE_DINNER;
			puiPara->AEMode = PRV_COMP_0EV;
			puiPara->Iso = PRV_ISO_AUTO;
			puiPara->WBMode = PRV_WB_AUTO;
			puiPara->ImageEffect = PRV_EFFECT_NORNAL;
			puiPara->FDMode = FDMODE_OFF;
			puiPara->DriverMode = PRV_SNAP_SINGLE;
			puiPara->Sharpness = PRV_EDGE_NORMAL;
			puiPara->Saturation = PRV_SATURATION_HIGH;
			puiPara->Contrast = PRV_CONTRAST_MIDDLE;
			break;
	}

	appSharpnessSet(puiPara->Sharpness);
	appSaturationSet(puiPara->Saturation);
	appContrastSet(puiPara->Contrast);
	appEVSet(puiPara->AEMode);
	appISOSet(puiPara->Iso);
	appWBSet(puiPara->WBMode);
	appEffectSet(puiPara->ImageEffect);

	/* Hue */
	sp1kCdspHueEnhanSet(1,0,hueResId[0]);
	//sp1kCdspHueEnhanSet(1,1,hueResId[1]);
	//sp1kCdspHueEnhanSet(1,2,hueResId[2]);

	return SUCCESS;
}

UINT8 appWBSet(UINT8 WBValue)
{
	switch(WBValue)
	{
		case PRV_WB_AUTO:
			sp1kAwbModeSet(AWB_MODE_Auto);
			break;
		case PRV_WB_CLOUDY:
			sp1kAwbModeSet(AWB_MODE_Cloudy);
			break;
		case PRV_WB_DAYLIGHT:
			sp1kAwbModeSet(AWB_MODE_Sunny);
			break;
		case PRV_WB_FLUORESCENT_H:
			sp1kAwbModeSet(AWB_MODE_Fluorescent);
			break;
		case PRV_WB_TUNGSTEN:
			sp1kAwbModeSet(AWB_MODE_Tungsten);
			break;
		default:
			sp1kAwbDisable();
			break;
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//appStillView
//-----------------------------------------------------------------------------
extern void appVideoStampSet(void);
UINT8 appSpeedyCaptureConfig(void)
{
	UINT8 ret=CALL_DEFAULT;
	UINT8 dataModeBak,videoSizeBak;
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
	sp1kDispImgWinEn(0);
	if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		dataModeBak = puiPara->StampVideoMode;
		videoSizeBak = puiPara->VideoSize;
		puiPara->StampVideoMode = puiPara->StampMode;
		puiPara->VideoSize = MOV_SIZE_VGA;
		__FUNC_TRACK__;
		printf("stillfactor=%d,curlog=%d\n",stillZFctor,stillCurrLog);
		#if (VIDEO_NEW_ZOOM == 1)
		sp1kVideoZoomCtrlParaSet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG,stillCurrLog);
		#else
		video_zoomInFlag=1;
		video_zoomFactor = stillZFctor;
		ZoomCurLog = stillCurrLog;
		//sp1kPvZoomSet(0, video_zoomFactor, 0); // hlc, Mantis: 0021118
		//sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
		#endif
		if((sp1kVideoRecParamGet(VD_CAPTURE)==0)||(appPreStateGet(0)!=APP_STATE_MENU))
		{
			sp1kHalCtrlScreen(0);

			sp1kAeSetModeEn(AE_DISABLE);
			sp1kAwbDisable();
			sp1kFdDisable();
			sp1kDistDisable();

			sp1kVideoRecParamSet(VD_CAPTURE,1);
			sp1kVideoCapParamSet(VD_CAP_COUNT,6);
			sp1kVideoCapParamSet (VD_CAP_SIZE,VD_CAP_VGA);
			//sp1kVideoCapParamSet(SP1K_VD_CAP_COUNT,3);
			//sp1kVideoCapParamSet (SP1K_VD_CAP_SIZE,VD_CAP_HD);
			sp1kVideoCapParamSet(SP1K_VD_CAP_MOD,SP1K_SPEEDCAP_STILL);
			if(puiPara->ImageSize != PRV_SIZE_VGA)
			{
				puiPara->ImageSize = PRV_SIZE_VGA;
			}

			isUpdateNum=0;
			appVideoResSet(puiPara->VideoSize);
			sp1kYuvBufModeSet();//fan.xiao add for speedy capture use 4 1st path frame buffer , Mantis 45534
			appVideoPreSizeSet(puiPara->VideoSize);

			sp1kVideoRecFrontRateSet (0x00);
			sp1kVideoRecParamSet(VD_REC_YUV_FORMAT,VD_REC_YUV_422_420);
			sp1kAeSetModeEn(AE_ENABLE);
			sp1kAwbEnable();

		}

		appIconPsTabLoad(ICON_MODE_PV);
		appStillOsdInit(1);
		appVideoStampSet();//move after osdinit
		puiPara->StampVideoMode = dataModeBak;
		puiPara->VideoSize = videoSizeBak;

		sp1kHalCtrlScreen(3);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 4);
		ret=IGNORE_DEFAULT;
	}
	else
	{
		if(sp1kVideoRecParamGet(VD_CAPTURE))
		{
			sp1kVideoRecParamSet(VD_CAPTURE,0);
			appPvForceUpdateOn(1);
		}
		ret = CALL_DEFAULT;
	}
	sp1kDispImgWinEn(1);
	return ret;

}

UINT8 appSpeedyCaptureRecConfig(void)
{
	UINT8 ret=CALL_DEFAULT;
	UINT8 dataModeBak,videoSizeBak;
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
	sp1kDispImgWinEn(0);
	if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		dataModeBak = puiPara->StampVideoMode;
		videoSizeBak = puiPara->VideoSize;
		puiPara->StampVideoMode = puiPara->StampMode;

		__FUNC_TRACK__;
		printf("stillfactor=%d,curlog=%d\n",stillZFctor,stillCurrLog);
		if((sp1kVideoRecParamGet(VD_CAPTURE)==0)||(appPreStateGet(0)!=APP_STATE_MENU))
		{
			sp1kHalCtrlScreen(0);
			sp1kAeSetModeEn(AE_DISABLE);
			sp1kAwbDisable();
			sp1kFdDisable();
			sp1kDistDisable();

			sp1kVideoRecParamSet(VD_CAPTURE,1);
			sp1kVideoCapParamSet(SP1K_VD_CAP_COUNT,3);

			if(puiPara->ImageSize == PRV_SIZE_VGA){
				sp1kVideoCapParamSet (SP1K_VD_CAP_SIZE,VD_CAP_VGA);
				puiPara->VideoSize = MOV_SIZE_VGA;
			}else{
				sp1kVideoCapParamSet (SP1K_VD_CAP_SIZE,VD_CAP_HD);
				puiPara->VideoSize = MOV_SIZE_HD;
				puiPara->ImageSize = PRV_SIZE_1M_16_9;
			}

			sp1kVideoCapParamSet(SP1K_VD_CAP_MOD,SP1K_SPEEDCAP_CLIP);
			puiPara->FrameRate = MOV_FRM_MIDDLE;

			isUpdateNum=0;
			appVideoFrmRateSet();
			appVideoResSet(puiPara->VideoSize);
			sp1kYuvBufModeSet();//fan.xiao add for speedy capture use 4 1st path frame buffer , Mantis 45534

			appVideoPreSizeSet(puiPara->VideoSize);

			#if (VIDEO_NEW_ZOOM == 1)
			sp1kVideoZoomCtrlParaSet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG,stillCurrLog);
			sp1kVideoRecZoomFacSet(videoZFctor, 0);
			#else
			video_zoomInFlag=1;
			video_zoomFactor = stillZFctor;
			ZoomCurLog = stillCurrLog;
			//sp1kPvZoomSet(0, video_zoomFactor, 0); // hlc, Mantis: 0021118
			//sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
			#endif

			sp1kVideoRecFrontRateSet (0x00);
			sp1kVideoRecParamSet(VD_REC_YUV_FORMAT,VD_REC_YUV_422_420);
			sp1kAeSetModeEn(AE_ENABLE);
			sp1kAwbEnable();

		}

		appIconPsTabLoad(ICON_MODE_PV);
		appStillOsdInit(1);
		appVideoStampSet();//move after osdinit
		puiPara->StampVideoMode = dataModeBak;
		puiPara->VideoSize = videoSizeBak;
		sp1kHalCtrlScreen(3);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 4);
		ret=IGNORE_DEFAULT;
	}
	else
	{
		if(sp1kVideoRecParamGet(VD_CAPTURE))
		{
			sp1kVideoRecParamSet(VD_CAPTURE,0);
			appPvForceUpdateOn(1);
		}
		ret = CALL_DEFAULT;
	}
	sp1kDispImgWinEn(1);
	return ret;

}

void appStillView (UINT16 msg)
{
	UINT8 InitMode;
	UINT8 LastMode;
	uiPara_t* snapPara;
	#if FUNC_HOST_MD
	UINT8 mdEn;
	#endif
	UINT8 sts;
	UINT8 ret;
	UINT8 Xoffset = 0;
	uiPara_t* puiPara ;
	static UINT8 stillZoomRptCnt = 0;
	#if  FUNC_HOST_IGBT_FLASH
	UINT8 curGid;
	#endif
	#if FUNC_HOST_PANORAMA
	static SINT16 panoramaSumOfGMV[2];
	static SINT16 panoramaGMVByFrame[2];
	static UINT8 panoramaStatus;
	#endif
	UINT16 adcVal = 0;
	//UINT16 cnt;
	
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();

	puiPara = appUiParaGet();
	switch (msg) {
		case SP1K_MSG_STATE_INIT:

		
		

/***************Just for QA test @start***************************************/
			if(AutoTestMode == 0x55)
			{
				appStillOsdInit(1);	//lryy osd test
			}

/***************Just for QA test @end***************************************/

			LastMode = appPreStateGet(0);
			switch (LastMode) {
				
				case APP_STATE_MENU:
					InitMode = SP1K_STILL_VIEW_INIT_SMART;
					if((appPreStateGet(1) == APP_STATE_VIDEO_VIEW) || (appPreStateGet(1) == APP_STATE_AUDIO_REC)
						|| appPvForceUpdateGet() || imgResolutionSet == 1) {
						imgResolutionSet = 0;
						appPvForceUpdateOn(0);
						InitMode = SP1K_STILL_VIEW_INIT_FULL;
						__FUNC_TRACK__;
					}
					break;

				#if 0
				case APP_STATE_POWER_ON:
					if (1 == bUsbPwr && !prjDbgMode)//if hotkey power on, switch to calibration menu
					{
						sp1kFdDisable();
						osMsgPost(SP1K_MSG_STATE_TO_MENU);
					}

					if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
					{
						InitMode = SP1K_STILL_VIEW_INIT_FULL;
						break;
					}
					if (sp1kSuperPreviewStatusGet(0) || puiPara->ImageSize==PRV_SIZE_2M_16_9 ||puiPara->ImageSize==PRV_SIZE_7M_16_9 \
						|| puiPara->ImageSize == PRV_SIZE_1M_16_9)
					{
						InitMode = SP1K_STILL_VIEW_INIT_FULL;
						break;
					}
					InitMode = SP1K_STILL_VIEW_INIT_SMART;
					break;
				#endif
					
				default:
					InitMode = SP1K_STILL_VIEW_INIT_FULL;
					break;
			}


			//jintao.liu 2008-2-26
			//add for resolve mantis bug 20099
			appElecFreqSet();//jintao.liu 2008-3-31 add for electronic freq
			
			//appSceneSet(puiPara->SceneMode);
			
			appStillViewInit(InitMode);

			
			
			appAutoOffTimeSet();//jintao.liu 2008-3-28 add for mantis 21604
			
			appStillQualitySet();
			

			

			#if MSG_AUTO_SEND
			{
			UINT8 osdMaxX,osdMaxY;
			static UINT8 ISize=0;
			ISize++;

			if (PRV_SIZE_MAX == ISize)
			{
				ISize=0;

			}
			puiPara->ImageSize = ISize;
			sp1kOsdLayoutAttrGet(&osdMaxX, &osdMaxY);
			uiUpdateImgSize(osdMaxX- 9+ Xoffset, 0, 0);
			uiRemainPixInit();//mantis #26379
			uiUpdateRemainPix();//mantis #26379
			sp1kHalWait(2000);

			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
			#endif

			#if FUNC_HOST_TOUCH_PANEL
			if((puiPara->tpCorrect == 1)&&(0 == bUsbPwr))
			{
				appCalTouchPanel();
			}
			#endif

			showChargingIcon = 1;
			#if (SUPPORT_AF && SUPPORT_SNAP_AF)
			if (!puiPara->SnapAf) {
				#if (SUPPORT_AF && SUPPORT_FIXED_AF)
				if (puiPara->AFMode) {
					AF_Set(431);//puiPara->AdjustMacro);//431);
				} else {
					AF_Set(0);
					appAFCtrl(0);
				}
				#else
				AF_Set(0);
				appAFCtrl(0);
				#endif
			}
			#endif
			
			break;

		case SP1K_MSG_KEY_PRESS_S1:
		 	menuDistortionSet (0, 0, 0);//wdd  @just for saving memory
			break;

		
		#if 0
		case SP1K_MSG_KEY_PRESS_SNAP:
			if (app_adjust_macro_sta_get()) {
				if (!cur_idx) {
					puiPara->SnapAf = 1;
					puiPara->AFMode = 1;
					press_snap_check_flag = 1;
						// �����ǰѡ������
					cur_idx = 0;

					// ���ɾ����־
					// app_adjust_macro_sta_set(0);

					// �˳��Ի��򣬷���INIT��Ϣ�ص�STILL_view
					// osMsgPost(SP1K_MSG_STATE_INIT);
					appStillOsdInit(1);
					enterMacroadjust = 0;
				}else{
					// �����ǰѡ������
					cur_idx = 0;

					// ���ɾ����־
					app_adjust_macro_sta_set(0);

					// �˳��Ի��򣬷���INIT��Ϣ�ص�STILL_view
					// osMsgPost(SP1K_MSG_STATE_INIT);
					appStillOsdInit(1);
					
					//pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
					//appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
					
					enterMacroadjust = 0;
				}
				break;
			}

			press_snap_check_flag = 1;
			key_repeate_check_flag = 0;
			break;

		case SP1K_MSG_KEY_REPEATE_SNAP:
			press_snap_check_flag = 0;
			if (!key_repeate_check_flag) {
				key_repeate_check_flag = 1;
			
			// ��ת��video_view
			//osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);

				// video_view��ʼ����ɺ��Զ���ʼ¼��
			}
			break;

		case SP1K_MSG_KEY_RELEASE_SNAP:
			
			if (key_repeate_check_flag) {
				key_repeate_check_flag = 0;
				break;
			}
			
			if (!press_snap_check_flag) {
				break;
			}
			press_snap_check_flag = 0;

			
			#if 0//(SUPPORT_AF && SUPPORT_SNAP_AF)
			appAFCtrl(1);
			if ((puiPara->SnapAf == 1) && (puiPara->DriverMode <= PRV_SNAP_BURST)) {
				osMsgPost(SP1K_MSG_SYS_AF_START);
				break;
			}
			#endif
		#endif
		//case SP1K_MSG_KEY_RELEASE_SNAP:
		case SP1K_MSG_KEY_PRESS_AF:
			#if 0//(SUPPORT_AF && SUPPORT_FIXED_AF)
			AF_Switch();
			//key_enter_AF_menu_flag = 1;
			//osMsgPost(SP1K_MSG_STATE_TO_MENU);
			#endif

			if (!af_timer_out && !af_timer_start) {
				af_timer_start = 1;
				break;
			}
			else if (af_timer_out) {
				af_timer_out = 0;
				af_timer_start = 0;
			}		
			
			appStillOsdInit(1);
			if(puiPara->SnapAf == 1){
			#if (SUPPORT_AF && SUPPORT_SNAP_AF)
			
					#if (SUPPORT_AF && SUPPORT_FIXED_AF)
					if(puiPara->SnapAf == 0){
						puiPara->SnapAf = 1;
					}
					//puiPara->DriverMode = PRV_SNAP_SINGLE;
					
					#endif
					appAFCtrl(1);
					afbackfaruiclearflag = 0;
					osMsgPost(SP1K_MSG_SYS_AF_START);
					
				#endif
			}
			if(puiPara->SnapAf == 1)
			{
				break;
			}
		case SP1K_MSG_KEY_PRESS_SNAP:
			//if (afdoingflag) {
			//    afdoingsnapflag = 1;
			//    break;
			//}
		if(puiPara->SnapAf == 1)
		{
			
			if(snappanduan == 1){

				snappanduan = 2;
				snaplianan = 0;
				break;
				
			}
			
			if(snappanduan == 0){
				
				snappanduan = 1;
			}
			
			if(snappanduan == 2){

				snaplianan = 0;
				break;
			}
		}

		if(puiPara->SnapAf == 0)
		{
			
			if(snappanduan == 1){

				snappanduan = 2;
				snaplianan = 0;
				break;
				
			}
			
			if(snappanduan == 0){
				
				snappanduan = 1;
			}
			
			if(snappanduan == 2){

				snaplianan = 0;
				break;
			}
		}
			af_timer_start = 0;
		case SP1K_MSG_MD_ACTION:
		case SP1K_MSG_FD_SMILE:
		case SP1K_MSG_KEY_PRESS_S2:
			//printf("11111");
			if(sp1kSD_CdGet()){
				//printf("22222");
				break;
			}
			sp1kBtnDisableAll();

			sp1kSuperPreviewSet(0,0);
			
			if (puiPara->DriverMode == PRV_SNAP_SINGLE) {
				app_set_led(0);

				appSnapFlashAdjustsAe(puiPara->FlashLed);
				appSnapFlashLed(1);
				sp1kHalWait(500);
				
				if(puiPara->FlashLed != 1){
					appSnapFlashLed(0);
					sp1kHalWait(500);
				}
				
			}
			
			#if (STILL_NEW_ZOOM == 1)
			if (appStillSmoothZoomMsgProcFlow(msg) == FAIL){
				//printf("33333");
				return;
			}
			#endif

			//printf("4444");
			
			#if (FRONT_SNAP_BREAK_DOWN >= 2)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
			#endif
			
/***************Just for QA test @start***************************************/
			if(AutoTestMode == 0x55)
			{
				UINT8 osdMaxX,osdMaxY;
				static UINT8 ISize=0;
				ISize++;

				if (PRV_SIZE_MAX == ISize)
				{
					ISize=0;

				}
				puiPara->ImageSize = ISize;
				sp1kOsdLayoutAttrGet(&osdMaxX, &osdMaxY);
				uiUpdateImgSize(osdMaxX- 9+ Xoffset, 0, 0);
				uiRemainPixInit();//mantis #26379
				uiUpdateRemainPix();//mantis #26379
				sp1kHalWait(1000);
				sp1kDistDisable();
			}
/***************Just for QA test @end***************************************/

			#if FUNC_HOST_IGBT_FLASH
			if((FlashLightMode == FLASH_AUTO && curGid <= 36) || FlashLightMode == FLASH_FORCE){
				if(ChargeStatusGet() == 1){
					break;
				}
			}
			#endif
			
			/* check environment */
			if ( appSnapEnvChk() != SUCCESS ) {
				sp1kBtnEnableAll();
				//printf("555");
				break;
			}
			
			/* send msg to wait AE/AWB ready */
			pvWaitCnt = 0;
			osMsgPost(SP1K_MSG_3A_WAIT_PROC);
			sp1kFdDisable();
						
			/* disable all buttons when wait AE/AWB ready */
			if(sp1kVideoCapParamGet(SP1K_VD_CAP_MOD) != SP1K_SPEEDCAP_CLIP){
				sp1kBtnDisableAll();
			}
			
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
				puiPara->StampVideoMode = puiPara->StampMode;
				//puiPara->VideoSize = MOV_SIZE_VGA;
				appVideoStampSet();//move after osdinit
			}
			
			if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON){
				puiPara->StampVideoMode = puiPara->StampMode;
				appVideoStampSet();
			}
			break;
			
		case SP1K_MSG_3A_WAIT_PROC:
			/* check 3A ready */
			#if FUNC_HOST_IGBT_FLASH
			aeStatusGet(SP1K_AE_STS_GID , &curGid);
			//printf("curGid = %bu\n",curGid);

			if(FlashLightMode != FLASH_OFF && curGid <= 36) {  /* flash */
				UINT8 befPreFlashLuma,preFlashLuma;
				UINT32 mainFlashTime;

				//printf("\033[32mF_FORCE\033[0m\n");
				sp1kAeSetModeEn(0);
				sp1kAwbDisable();

				frontPreflashCfg(SENSOR_PREFLASH_CFG_WIDTH , 190);	/*20us*/
				frontPreflashCfg(SENSOR_PREFLASH_CFG_H_POS , 10);
				frontPreflashCfg(SENSOR_PREFLASH_CFG_DELAY_TYPE , SENSOR_FLASH_DELAY_TYPE_H_POS);

				sp1kDispImgWinEn(0);
				sp1kCalibrationAWBSet(0);
				sp1kCalibrationObSet(0);
				sp1kCalibrationGammaSet(0);/*disable Gamma */

				frontPreflashAeCfg(333,SENSOR_GAIN_AUTO,4);

				frontPreflashTrig(&befPreFlashLuma,&preFlashLuma);

				if(preFlashLuma - befPreFlashLuma < 5 && befPreFlashLuma < 25){
					preFlashLuma = befPreFlashLuma + 8;
				}
				else if(preFlashLuma - befPreFlashLuma < 5 && befPreFlashLuma < 30){
					preFlashLuma = befPreFlashLuma + 5;
				}

				mainFlashTime = frontSnapFlashTimeGet(befPreFlashLuma,preFlashLuma,60,1);

				frontSnapFlashAeCfg(0,SENSOR_GAIN_AUTO,1);

				sp1kAwbEnable();
				sp1kCalibrationObSet(1);
				sp1kCalibrationAWBSet(1);
				sp1kCalibrationGammaSet(1);/*enable  Gamma */

				frontSnapFlashCfg(SENSOR_SNAP_FLASH_CFG_DELAY_TYPE,SENSOR_FLASH_DELAY_TYPE_H_POS);
				frontSnapFlashCfg(SENSOR_SNAP_FLASH_CFG_WIDTH , mainFlashTime);

				sp1kSnapParamSet( SP1K_SNAP_PARAM_REDO_PREPROCESS, SP1K_SNAP_POST_WB );
			}
			else if(FlashLightMode == FLASH_FORCE && curGid > 36){  /*dummy flash*/
				if (appStillViewAEAWBWait() != SUCCESS) {
					break;
				}
				//printf("\033[32mF_DUMMY\033[0m\n");
				/*sp1kAeSetModeEn(0);*/

				sp1kDispImgWinEn(0);
				frontPreflashAeCfg(333,SENSOR_GAIN_AUTO,0);
				frontPreflashCfg(SENSOR_PREFLASH_CFG_WIDTH , 190);	/*20us*/
				frontPreflashCfg(SENSOR_PREFLASH_CFG_H_POS , 10);
				frontPreflashCfg(SENSOR_PREFLASH_CFG_DELAY_TYPE , SENSOR_FLASH_DELAY_TYPE_H_POS);

				frontIgbtFlashCalibTrig(190,0);
				frontSnapFlashCfg(SENSOR_SNAP_FLASH_CFG_DELAY_TYPE,SENSOR_FLASH_DELAY_TYPE_H_POS);
				frontSnapFlashCfg(SENSOR_SNAP_FLASH_CFG_WIDTH , 200);
				frontSnapFlashAeCfg(0,SENSOR_GAIN_AUTO,0);
			}
			else{/* NO flash , check 3A ready */
				if (appStillViewAEAWBWait() != SUCCESS) {
					break;
				}
				//printf("\033[32mNO_FLASH\033[0m\n");
			}
			#else
			if (appStillViewAEAWBWait() != SUCCESS) {
				break;
			}
			#endif
			
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON)
			{
				sp1kVideoCapStart();
				uiOsdDialogDisp(ID_STR_WAITING___, 0);
				sp1kBtnDisableAll();
				return;
			}
			
			if(puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				//printf("--speedycapture rec-\n");
				sp1kVideoCapStart();
				HAL_GlobalReadGTimer(&speedyCapRecTime);
				count = speedyCapRecTime;
				return;
			}
			
			#if (FRONT_SNAP_BREAK_DOWN >= 2)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
			#endif

			#if (FRONT_SNAP_BREAK_DOWN >= 2)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
			#endif

			/* enable all buttons when AE/AWB ready */
			#if LIFE_SPAN_TEST
			snapRdy =1;
			#endif

			//if (puiPara->PhotoFrame == 1) {
			//	appPvPhotoFrameSet(PRV_PHOTO_FRAME_OFF);
			//}
			
			//printf("66");
			
			/* load snap parameter */
			if( appSnapParaLoad() != SUCCESS ) {
				sp1kBtnEnableAll();
				break;
			}

			/* set Customizestamp parameter api */
			//appCustomizeStampSet();

			//xian ++ 20081216, move from appSnapParaLoad()
			if(appDriveModeSet() !=SUCCESS) {
				sp1kBtnEnableAll();
				break;
			}
			//xian --
			
			/* snap task pre-process */
			if( appSnapPreProc() != SUCCESS ) {
				sp1kBtnEnableAll();
				break;
			}
			
			sp1kSnapParamSet(SP1K_SNAP_PARAM_REDO_PREPROCESS , SP1K_SNAP_POST_WB);
			
			/* trig snap task */
			sp1kPvPhotofrmEnable(0);    //xian ++ 20090717

			#if FUNC_HOST_MD
			mdEn = sp1kMDEnGet();
			sp1kMDEnSet(MD_DISABLE);
			#endif

			sp1kOsdClearCharExt(128, 88, 64, 64);
			appSnapFlashLed(1);
			
			//for (cnt=0;cnt<1;cnt++){
			ret = sp1kSnapTask();
			//}

			
			if (puiPara->PhotoFrame == 1) {
				sp1kSnapParamSet( SP1K_SNAP_PARAM_PHOTO_FRM_OPT, 1 );
			}

			#if FUNC_HOST_MD
			if (mdEn == MD_ENABLE)
			{
				sp1kMDEnSet(MD_ENABLE);
				sp1kMDLearningStart(MD_LEARNING_ENABLE);
			}
			#endif

			#if (FRONT_SNAP_BREAK_DOWN >= 2)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
			#endif

			/* snap task post-process */
			appSnapPostProc(ret);

			#if (FRONT_SNAP_BREAK_DOWN >= 2)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
			#endif

			#if FUNC_HOST_PF_PV
			sp1kPvDlyPhofrmBlendCntSet(1);//mantis 0042795
            appPvPhotoFrameRecover();	//xian ++ 20081216
			#endif
			
	        if(puiPara->FDMode== FDMODE_ON)
			{
				sp1kFdReset();
				sp1kFdEnable();
			}

			#if MSG_AUTO_SEND
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			#endif
			
/***************Just for QA test @start***************************************/
			if(AutoTestMode == 0x55)
			{
				osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
/***************Just for QA test @end***************************************/

			

			app_set_led(1);
			if(puiPara->FlashLed != 1){
				appSnapFlashLed(0);
			}else{
				appSnapFlashLed(1);
			}
			
 			appSnapFlashAdjustsAe(0xff);
			
			sp1kSuperPreviewSet(0,1);
			if (sp1kSuperPreviewStatusGet(0)) {
				appStillPreviewReset();
			}
			#if 0
			#if (SUPPORT_AF && SUPPORT_SNAP_AF)
				if (puiPara->SnapAf) {
					puiPara->SnapAf = 0;
					
					AF_Set(0);
					appAFCtrl(0);

				}
			#endif
			#endif
			
			sp1kBtnEnableAll();
			break;
			
#if FUNC_HOST_PANORAMA
		case SP1K_MSG_GMV_BY_FRAME_CALCULUS:
			sp1kPanoramaGMVByFrameCalculus(&panoramaGMVByFrame[0],&panoramaGMVByFrame[1],&panoramaStatus);
			sp1kPanoramaSumOfGMVCalculus(&panoramaSumOfGMV[0], &panoramaSumOfGMV[1]);
			break;
		case SP1K_MSG_SUM_OF_GMV_OSD_DRAW:
			/* GMV osd draw */
			//printf("SumOfGMV(%d,%d)\n",panoramaSumOfGMV[0],panoramaSumOfGMV[1]);
			break;
		case SP1K_MSG_GMV_TOO_FAST_OSD_DRAW:
			/* too fast osd draw */
			//printf("too fast\n");
			break;
		case SP1K_MSG_GMV_TOO_SLOW_OSD_DRAW:
			/* too slow osd draw */
			//printf("too slow\n");
			break;
		case SP1K_MSG_PANORAMA_FINISH:
			/* too slow osd draw */
			sp1kPanoramaEnSet(PANORAMA_DISABLE);
			sp1kPanoramaStitchFunc();
			sp1kPanoramaDstFileSave("SPCA0001.YUV");
			sp1kSnapSizeSet( 2880, 400 );
			osMsgPost(SP1K_MSG_3A_WAIT_PROC);
			printf("Panorama finish\n");
			break;
#endif
		case SP1K_MSG_EYE_BLINK:
			sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,0xff);
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;

		#if 0
		case SP1K_MSG_KEY_PRESS_AF:
			key_repeate_check_flag = 0;
			break;

		case SP1K_MSG_KEY_REPEATE_AF:
			if (!key_repeate_check_flag) {
				key_repeate_check_flag = 1;
				
			}
			break;

		case SP1K_MSG_KEY_RELEASE_AF:
			if (key_repeate_check_flag) {
				key_repeate_check_flag = 0;
				break;
			}
		#endif
		
		#if 0
		case SP1K_MSG_KEY_PRESS_AF:
			#if (SUPPORT_AF && SUPPORT_FIXED_AF)
			AF_Switch();
			//key_enter_AF_menu_flag = 1;
			//osMsgPost(SP1K_MSG_STATE_TO_MENU);
			#endif
			break;
		#else
		//case SP1K_MSG_KEY_PRESS_AF:
		case SP1K_MSG_KEY_PRESS_MENU:
			afbackfaruiclearflag = 0;
			versionclearflag = 0;
			uiOsdVerDisp(FW_VERSION, 0);
			if(enterMacroadjust == 1){
				enterMacroadjust = 0;
			}
			//printf("menu\n");
			if(sp1kSpeedyCapStsGet() == SPEEDCAP_STS_RUNNING){
				break;
			}
			sp1kFdDisable();
			sp1kFdSetSmile(0);
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{

				#if (VIDEO_NEW_ZOOM == 1)
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=video_zoomFactor;
				stillCurrLog = ZoomCurLog;
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				stillZFctor=sp1kPvZoomFctGet();
				stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				stillZFctor=PvZoomCtrl.factor;
				stillCurrLog = PvZoomCtrl.curLog;
				#endif
			}

			puiPara->FlashLed= 0;
			appSnapFlashLed(0);
			uiUpdateFlashLed(LCD_WIDTH, 0, 0);
			
			osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;
		#endif
		
		//case SP1K_MSG_KEY_PRESS_DOWN:
		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_BACK:
			if (app_adjust_macro_sta_get() && enterMacroadjust == 1) {
			cur_idx = !cur_idx;
			app_quick_delete_dialog(cur_idx);
			break;
			}
			if(puiPara->SceneMode != PRV_SCENE_AUTO){
				puiPara->SceneMode = PRV_SCENE_AUTO;
				uiSceneModeUpdate();
			} else if(puiPara->DriverMode != PRV_SNAP_BURST){
				pUiPara ->DriverMode++;
				if(puiPara->DriverMode == PRV_SNAP_BURST){
					puiPara->DriverMode = PRV_SNAP_SINGLE;
				}
				//appDriveModeSet();
				uiUpdateSnapMode(LCD_WIDTH, 0, 0);
			} else if(puiPara->DriverMode == PRV_SNAP_BURST){
				puiPara->DriverMode = PRV_SNAP_SELF_2S;
				uiUpdateSnapMode(LCD_WIDTH, 0, 0);
			}
			break;


		#if 0
		case SP1K_MSG_KEY_PRESS_DOWN:
			if(puiPara->ImageEffect != PRV_EFFECT_NORNAL || puiPara->DriverMode != PRV_SNAP_SINGLE){
				if (puiPara->DriverMode != PRV_SNAP_SINGLE) {
					puiPara->DriverMode = PRV_SNAP_SINGLE;
					uiUpdateSnapMode(LCD_WIDTH, 0, 0);
				} else if (puiPara->ImageEffect != PRV_EFFECT_NORNAL) {
					pUiPara ->ImageEffect--;
					appEffectSet(puiPara ->ImageEffect);
					uiUpdateEffect(LCD_WIDTH, 0, 0);
					if (pUiPara ->ImageEffect == PRV_EFFECT_NORNAL){
						puiPara->DriverMode = PRV_SNAP_SELF_5S;
						uiUpdateSnapMode(LCD_WIDTH, 0, 0);
					}
				}
			}
			else
			{
				puiPara->PhotoFrame++;
				if (puiPara->PhotoFrame == PRV_PHOTO_FRAME_MAX){
					puiPara->PhotoFrame = PRV_PHOTO_FRAME_OFF;
				}

				if (puiPara->PhotoFrame != PRV_PHOTO_FRAME_OFF){
					sp1kOsdClear(0);
				} else {
					sp1kPvPhotofrmEnable(0);
					appStillOsdInit(1);
				}
				appPvPhotoFrameSet(puiPara->PhotoFrame);
			}
			break;
			#endif
			
		case SP1K_MSG_KEY_PRESS_UP: 
			if (app_adjust_macro_sta_get() && enterMacroadjust == 1) {
			cur_idx = !cur_idx;
			app_quick_delete_dialog(cur_idx);
			break;
		}
			if (puiPara->FlashLed == 0) {
				puiPara->FlashLed = 1;
			}else if(puiPara->FlashLed == 1){
				puiPara->FlashLed = 0;
			}
			appSnapFlashLed(1);
			uiUpdateFlashLed(LCD_WIDTH, 0, 0);
			break;
		
		
		//case SP1K_MSG_KEY_PRESS_MENU:
		case SP1K_MSG_KEY_PRESS_BURST:
		case SP1K_MSG_KEY_PRESS_PB:
			//if (app_adjust_macro_sta_get() && enterMacroadjust == 1) {
			//cur_idx = !cur_idx;
			//app_quick_delete_dialog(cur_idx);
			//break;
			//}
			if(puiPara->SceneMode != PRV_SCENE_AUTO){
				puiPara->SceneMode = PRV_SCENE_AUTO;
				uiSceneModeUpdate();
			} else {
					//pUiPara ->DriverMode++;
					if(puiPara->DriverMode == PRV_SNAP_BURST){
						puiPara->DriverMode = PRV_SNAP_SINGLE;
					}else {\
						puiPara->DriverMode = PRV_SNAP_BURST;
					}
					// appDriveModeSet();
					uiUpdateSnapMode(LCD_WIDTH, 0, 0);
			}
			if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
				AF_Set(0);
				appAFCtrl(0);
			}
			#if 0
			if (puiPara->PhotoFrame == PRV_PHOTO_FRAME_OFF) {
				if ((puiPara->DriverMode == PRV_SNAP_SINGLE) && (puiPara->ImageEffect == PRV_EFFECT_NORNAL)) {
					puiPara->DriverMode = PRV_SNAP_SELF_5S;

					uiUpdateSnapMode(LCD_WIDTH, 0, 0);
				} else {
					puiPara->DriverMode = PRV_SNAP_SINGLE;
					pUiPara ->ImageEffect++;
					if(puiPara->ImageEffect == PRV_EFFECT_MAX)
					{
						puiPara->ImageEffect = PRV_EFFECT_NORNAL;
						puiPara->DriverMode = PRV_SNAP_SINGLE;
					}
					appEffectSet(puiPara ->ImageEffect);
					uiUpdateEffect(LCD_WIDTH, 0, 0);
				}
				
				
			}
			
			else {
				puiPara->PhotoFrame--;
				
				if (puiPara->PhotoFrame != PRV_PHOTO_FRAME_OFF){
					sp1kOsdClear(0);
				} else {
					sp1kPvPhotofrmEnable(0);
					appStillOsdInit(1);
				}
				
				appPvPhotoFrameSet(puiPara->PhotoFrame);
			}

			
			if (puiPara->PhotoFrame == PRV_PHOTO_FRAME_OFF) {
				if (puiPara->DriverMode == PRV_SNAP_SINGLE) {
					puiPara->DriverMode = PRV_SNAP_SELF_5S;
				} else {
					puiPara->DriverMode = PRV_SNAP_SINGLE;
				}
				
				uiUpdateSnapMode(LCD_WIDTH, 0, 0);
			} else {
				puiPara->PhotoFrame--;
				
				if (puiPara->PhotoFrame != PRV_PHOTO_FRAME_OFF){
					sp1kOsdClear(0);
				} else {
					sp1kPvPhotofrmEnable(0);
					appStillOsdInit(1);
				}
				
				appPvPhotoFrameSet(puiPara->PhotoFrame);
			#endif
			break;
			
		case SP1K_MSG_KEY_PRESS_LEFT:
			if (puiPara->DriverMode == PRV_SNAP_SINGLE) {
				puiPara->DriverMode = PRV_SNAP_SELF_5S;
			} else if (puiPara->DriverMode == PRV_SNAP_SELF_5S){
				puiPara->DriverMode = PRV_SNAP_SINGLE;
				pUiPara ->ImageEffect++;
				appEffectSet(puiPara ->ImageEffect);
				uiUpdateEffect(LCD_WIDTH, 0, 0);
			}
			
			uiUpdateSnapMode(LCD_WIDTH, 0, 0);
			if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
				AF_Set(0);
				appAFCtrl(0);
			}
			break;
			
		case SP1K_MSG_KEY_PRESS_RIGHT:
			if(enterMacroadjust == 1){
				enterMacroadjust = 0;
			}
			if (!sp1kSD_CdGet() && sp1kPbFileCntGet() != 0) {
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				osMsgPost(SP1K_MSG_SYS_QUICK_DELETE);
			}
			break;

		case SP1K_MSG_KEY_PRESS_MODE:
			afbackfaruiclearflag = 0;
			versionclearflag = 0;
			uiOsdVerDisp(FW_VERSION, 0);
			if(enterMacroadjust == 1){
				enterMacroadjust = 0;
			}
			
			sp1kDispImgWinEn(0);
			sp1kHalWait(200);
			sp1kPreviewCropSizeSet(1280,960);
			sp1kPreviewSet(0, 1280, 720, sp1kPvZoomFctGet(), 1);
			sp1kHalWait(100);

			#if SUPPORT_VIDEO_EV
			pUiPara->AEMode = PRV_COMP_N03EV;
			appEVSet(puiPara->AEMode);
			#endif
			
			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			flag_menu_back_videoview = 0;
			//osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			break;
			


		case SP1K_MSG_DZOOM_OSD_UPDATE:
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{

				#if (VIDEO_NEW_ZOOM == 1)
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
				uiUpdateOSDPvZoom(stillZFctor, 0);
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				stillZFctor=sp1kPvZoomFctGet();
				stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
				uiUpdateOSDPvZoom(stillZFctor, 0);
				#endif
			}
			break;
		case SP1K_MSG_DZOOM_3A_UPDATE:
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				sp1kVideoZoom3APostSet();
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				sp1kPvZoom3APostSet();
				#endif
			}
			sp1kAeSetModeEn(AE_ENABLE);
			sp1kAwbEnable();
			break;
			
		case SP1K_MSG_KEY_PRESS_ZOOM_IN:
		case SP1K_MSG_KEY_PRESS_TELE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			stillZoomRptCnt=0;
			appOsdModeSw(0);
			pvZoomSts |= 0xf0;
			if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
			{
				sp1kAeSetModeEn(AE_DISABLE);  //hlc, stops 3A when zoom
				sp1kAwbDisable();
			}
			sp1kFdDisable();
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				sp1kVideoZoom3APreSet();
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					sp1kPvZoom3APreSet();
				}
				#endif
			}

			break;//
		case SP1K_MSG_KEY_REPEATE_TELE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if(VIDEO_NEW_ZOOM == 1)
				sp1kVideoSmoothZoomSet(1);
				appVideoViewTele();
				#else
				UINT8 ZfStatus;
				appVideoViewTele();
				if(sp1kVideoRecStatusGet() == VID_REC_IDLE) {
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
				} else {
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 1);
				}

				if (ZfStatus == FAIL) {
					appVideoViewTeleReply();
				}
				uiUpdateOSDPvZoom(video_zoomFactor, 1);
				#endif
			}
			else
			{
				#if(STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					sp1kPvSmoothZoomSet(1);
					appStillViewZoomIn();
				}
				#else
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					appStillViewZoomIn();
				}
				#endif
			}

			
			if(osMsgSeek(SP1K_MSG_KEY_RELEASE_TELE) == TRUE)
			{
				osMsgFlush(MSG_CTRL_FLUSH_BTN);//mantis #41423
			}
			else
			{
				break;
			}

		case SP1K_MSG_KEY_REPEATE_ZOOM_IN:
		case SP1K_MSG_KEY_RELEASE_ZOOM_IN:
		case SP1K_MSG_KEY_RELEASE_TELE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			stillZoomRptCnt=0;
			pvZoomSts &= ~0xf0;
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				appVideoViewTele();
				sp1kVideoSmoothZoomSet(0);
				#else
				UINT8 ZfStatus;
				appVideoViewTele();
				if(sp1kVideoRecStatusGet() == VID_REC_IDLE)
				{
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
				}
				else
				{
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 1);
				}

				if (ZfStatus == FAIL)
				{
					appVideoViewTeleReply();
				}

				uiUpdateOSDPvZoom(video_zoomFactor, 1);
				sp1kAeSetModeEn(AE_ENABLE); // hlc, Enable 3A when zoom key is release
				sp1kAwbEnable();// hlc, Enable 3A when zoom key is release
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					appStillViewZoomIn();
					sp1kPvSmoothZoomSet(0);
				}
				else
				{
				}
				#else
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					appStillViewZoomIn();
					stillZFctor = PvZoomCtrl.factor;
					stillCurrLog =PvZoomCtrl.curLog;
				}
				sp1kAeSetModeEn(AE_ENABLE); // hlc, Enable 3A when zoom key is release
				sp1kAwbEnable();// hlc, Enable 3A when zoom key is release
				#endif
			}

			if(puiPara->FDMode ==FDMODE_ON)
			{
				sp1kFdEnable();
			}
			break;

		case SP1K_MSG_KEY_PRESS_ZOOM_OUT:
		case SP1K_MSG_KEY_PRESS_WIDE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			stillZoomRptCnt=0;
			appOsdModeSw(0);
			pvZoomSts |= 0x0f;
			if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
			{
				sp1kAeSetModeEn(AE_DISABLE);  //hlc, stops 3A when zoom
				sp1kAwbDisable();
			}
			sp1kFdDisable();
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				sp1kVideoZoom3APreSet();
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					sp1kPvZoom3APreSet();
				}
				#endif
			}
			break;//
		case SP1K_MSG_KEY_REPEATE_WIDE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				sp1kVideoSmoothZoomSet(1);
				appVideoViewWide();
				#else
				UINT8 ZfStatus;
				appVideoViewWide();
				if(sp1kVideoRecStatusGet() == VID_REC_IDLE) {
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
				} else {
					ZfStatus = sp1kVideoRecZoomFacSet(video_zoomFactor, 1);
				}

				if (ZfStatus == FAIL) {
					appVideoViewWideReply();
				}

				uiUpdateOSDPvZoom(video_zoomFactor, 1);
				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					sp1kPvSmoothZoomSet(1);
					appStillViewZoomOut();
				}
				#else
				appStillViewZoomOut();
				#endif
			}

			if(osMsgSeek(SP1K_MSG_KEY_RELEASE_WIDE) == TRUE)
			{
				osMsgFlush(MSG_CTRL_FLUSH_BTN);//mantis #41423
			}
			else
			{
				break;
			}
		case SP1K_MSG_KEY_REPEATE_ZOOM_OUT:
		case SP1K_MSG_KEY_RELEASE_ZOOM_OUT:
		case SP1K_MSG_KEY_RELEASE_WIDE:
			//if(puiPara->DriverMode!=PRV_SNAP_SINGLE) {
			//	break;
			//}
			stillZoomRptCnt=0;
			pvZoomSts &= ~0x0f;	
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				#if (VIDEO_NEW_ZOOM == 1)
				appVideoViewWide();
				sp1kVideoSmoothZoomSet(0);
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
				#else
				appVideoViewWide();
				if(sp1kVideoRecStatusGet() == VID_REC_IDLE) {
					sp1kVideoRecZoomFacSet(video_zoomFactor, 0);
				} else {
					sp1kVideoRecZoomFacSet(video_zoomFactor, 1);
				}
				uiUpdateOSDPvZoom(video_zoomFactor, 1);
				sp1kAeSetModeEn(AE_ENABLE); // hlc, Enable 3A when zoom key is release
				sp1kAwbEnable();// hlc, Enable 3A when zoom key is release

				#endif
			}
			else
			{
				#if (STILL_NEW_ZOOM == 1)
				if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
				{
					appStillViewZoomOut();
					sp1kPvSmoothZoomSet(0);
					stillZFctor=sp1kPvZoomFctGet();
					stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
				}
				#else
				appStillViewZoomOut();
				stillZFctor = PvZoomCtrl.factor;
				stillCurrLog =PvZoomCtrl.curLog;
				sp1kAeSetModeEn(AE_ENABLE); // hlc, Enable 3A when zoom key is release
				sp1kAwbEnable();// hlc, Enable 3A when zoom key is release
				#endif
			}

			if(puiPara->FDMode ==FDMODE_ON)
			{
				sp1kFdEnable();
			}
			break;

		
			
		case SP1K_MSG_KEY_PRESS_DISP:
			break;
		case SP1K_MSG_KEY_PRESS_F1:
#ifdef PREVIEW_RAW_SAVE_MODE
			sp1kCalibrationAWBSet(0);
			sp1kCalibrationPrvRawModeSet( 0 );
#endif
			break;
		case SP1K_MSG_KEY_PRESS_F2:
#ifdef PREVIEW_RAW_SAVE_MODE
			sp1kCalibrationPrvRawSave( 0 );
#endif
			break;

		case SP1K_MSG_Version_Show:
			versionclearflag = 1;
			uiOsdVerDisp(FW_VERSION, 0);
			break;

		case SP1K_MSG_Version_Clear:
			versionclearflag = 0;
			uiOsdVerDisp(FW_VERSION, 0);
			break;
			
		//case SP1K_MSG_KEY_PRESS_PB:
		case SP1K_MSG_KEY_PRESS_DOWN:
		//case SP1K_MSG_KEY_PRESS_BACK:
			afbackfaruiclearflag = 0;
			if(enterMacroadjust == 1){
				enterMacroadjust = 0;
			}
			if (!sp1kSD_CdGet() && sp1kPbFileCntGet() != 0) {
				osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				osMsgPost(SP1K_MSG_SYS_QUICK_DELETE);
			}else if (sp1kPbFileCntGet() == 0) {
				uiOsdDialogDisp(ID_STR_NO_FILE, 0);
				sp1kHalWait(500);
				appStillOsdInit(1);
			}
			#if 0
			if(sp1kSpeedyCapStsGet() == SPEEDCAP_STS_RUNNING){
				printf("---stop capture\n");
				HAL_GlobalReadGTimer(&speedyCapRecTime);
				count = (speedyCapRecTime - count)/1000;
				sp1kSpeedyCaptureStop();
				osMsgPost(SP1K_MSG_MEDIA_VIDO_CAP_FINISH);
			}

			osMsgPost(SP1K_MSG_SYS_QUICK_DELETE);
			sp1kAwbDisable();
			sp1kFdDisable();
			sp1kDistDisable();
			#endif
			break;
			
		case SP1K_MSG_Adjust_Macro:
			if (app_adjust_macro_sta_get()) {
				// �����ǰѡ������
				cur_idx = 0;

				// ���ɾ����־
				app_adjust_macro_sta_set(0);

				// �˳��Ի��򣬷���INIT��Ϣ�ص�STILL_PLAY
				// osMsgPost(SP1K_MSG_STATE_INIT);
				appStillOsdInit(1);
				//pbFile.dwIndex = 0xffff; //xian ++, invalid index for #31928
				appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
				break;
			}
			
			adjust_macro_to_menu = 1;
			#if 0
			if(dispDevActGet() != 4) {//HDMI do not support menu
				pbZoomCurLog = PLAY_MIN_ZOOM_FACT;
				pbCropDispBuffer2ABBuf();
				osMsgPost(SP1K_MSG_STATE_TO_MENU);
			}
			#else
				app_adjust_macro_sta_set(1);
				app_quick_delete_dialog(cur_idx);
			#endif
		break;
		
		case SP1K_MSG_KEY_REPEATE_POWER:
			appPowerOffProc();
			break;
			
		case SP1K_MSG_USB_PLUG_IN:
			power_up_usb_check_flag = 0;
			
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			
			if(AdapterCharge)
				break;
			#if 0
			if (storageReadyflag==SUCCESS) {
				if (USB_DBG) {
					/*
						uiPara_t* uienv;
						uienv = appUiParaGet();
						if(uienv->USBMode==USB_PCCAM)
							sp1kHalUSBSwPlugIn();
						else
					*/
					sp1kHalUSBSwPlugIn();
				}
				#if (FUNC_HOST_DPS == 1)
				else if( puiPara->USBMode == USB_SIDC) {
					//printf("mark\n");
						osMsgPost( SP1K_MSG_STATE_TO_DPS );
				}
				#endif
				else {
					//puiPara->USBMode=USB_MASS;
					osMsgPost(SP1K_MSG_STATE_TO_MENU);	
				}
			}else {
				puiPara->USBMode=USB_PCCAM;
				osMsgPost(SP1K_MSG_STATE_TO_USB);
			}
			#endif

			osMsgPost(SP1K_MSG_STATE_TO_MENU);
			break;
			
		case SP1K_MSG_USB_PLUG_OUT:
			if (power_up_usb_check_flag) {
				power_up_usb_check_flag = 0;
				break;
			}
			break;
			
		case SP1K_MSG_POWER_OFF:
			appPowerOffProc();
			break;
			
		case SP1K_MSG_KEY_PRESS_SELFSNAP:
			app_set_led(0);

			appSnapFlashAdjustsAe(puiPara->FlashLed);
			appSnapFlashLed(1);
			sp1kHalWait(500);
			
			appSnapFlashLed(0);
			sp1kHalWait(500);

			
			appSnapFlashLed(1);
			snapPara = appUiParaGet();
            appSnapParaLoad();//xian ++ 20081216
			appStillQuickViewSet();
            sp1kPvPhotofrmEnable(0);    //xian ++ 20090717
            sp1kSuperPreviewSet(0,0);
			if (sp1kSnapTask() ==  SP1K_SNAP_MSG_FILE_WRITE_ERROR)
			{
				uiUpdateOSDMemFull();
				isUpdateNum=1;
				osMsgPost(SP1K_MSG_STATE_INIT);
			}

			selfsnapbackfarflag = 0;
			
			sp1kSuperPreviewSet(0,1);
			if (sp1kSuperPreviewStatusGet(0)) {
				appStillPreviewReset();
			}
			
            #if FUNC_HOST_PF_PV
            appPvPhotoFrameRecover();	//xian ++ 20081216
            #endif

            appOsdSelfSnapEnableOsd(0);
			uiRemainPixShow();

			sp1kBtnEnableAll();
			if(puiPara->FDMode== FDMODE_ON)
			{
				sp1kFdReset();
				sp1kFdEnable();
			}
			sp1kBtnEnableAll();

			if (((puiPara->DriverMode <= PRV_SNAP_CYC_10S) && (puiPara->DriverMode >= PRV_SNAP_CYC_2S))) {
				osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			} else {
				app_set_led(1);
				appSnapFlashLed(0);
				appSnapFlashAdjustsAe(0xff);
			}
			break;
		//jintao.liu 2008-3-12
		//add for battery detect
		case SP1K_MSG_BAT_LV_UPDATE://jintao.liu 2008-3-13 add for battery action
			uiUpdateBatteryLevel();
			appBattStsHandle();
			break;
		case SP1K_MSG_BAT_EMPTY:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			uiUpdateOSDBatEmpty();
			osMsgPost(SP1K_MSG_POWER_OFF);
			sp1kBtnDisableAll();//ready to power off, disable all button!
			break;
		case SP1K_MSG_TV_PLUG_IN:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
#if(TV_OPTION == TV_PLAY_OPTION)
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_IN);
#elif (TV_OPTION == TV_FULL_OPTION)
			appDispSet();
#endif
			break;
		case SP1K_MSG_TV_PLUG_OUT:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
#if(TV_OPTION == TV_PLAY_OPTION)
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_TV_PLUG_OUT);
#elif (TV_OPTION == TV_FULL_OPTION)
			appDispSet();
#endif
			break;
		case SP1K_MSG_HDMI_PLUG_IN:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			#if 1
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
			//appDispHdmiOpen();
			#else
			//example code for super preview in video video HD mode
			//osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			appDispHdmiOpen();
			osMsgPost(SP1K_MSG_STATE_INIT);
			sp1kHalCtrlScreen(3);

			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			sp1kSuperPreviewSet(1,1);
			//osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
			//appDispHdmiOpen();
			break;

			#endif
			break;
		case SP1K_MSG_HDMI_PLUG_OUT:
			appDispSet();
			break;

		case SP1K_MSG_CARD_PLUG_IN:
			versionclearflag = 0;
			uiOsdVerDisp(FW_VERSION, 0);
			appCardErrFmtErrSet(0);
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			appStorageMount(K_MEDIA_SDF);
			uiRemainPixInit();
			appStillOsdInit(1);
			break;
			
		case SP1K_MSG_CARD_PLUG_OUT:
			#ifdef _ENABLE_LCD_OFF_          //yp for mantis 43034
			appOsdModeSw(0);
			#endif /*_ENABLE_LCD_OFF_*/
			
			appCardErrFmtErrSet(0);
			
			//add for mantis #36958 @start
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
			{
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);

				appStillOsdInit(1);
				
				uiOsdDialogDisp(ID_STR_NO_CARD_, 0);

				
				
				if(enterVersionshow == 1){
					enterVersionshow = 0;
					osMsgPost(SP1K_MSG_Version_Show);
				}

				#if SUPPORT_RTC_OSD_UPDATE
				appCurrentTimeShow(1);
				#endif
				
				if(power_up_usb_check_flag){
					power_up_usb_check_flag = 0;
					if(sp1kUSB_PlugGet()){
						osMsgPost(SP1K_MSG_USB_PLUG_IN);
					}
				}
				
				/*
				if(enterPCCamflag && !AdapterCharge){
					enterPCCamflag = 0;
					puiPara->USBMode=USB_PCCAM;
					osMsgPost(SP1K_MSG_STATE_TO_USB);
					break;
				}
				*/
				
				break;
			}
			//add for mantis #36958 @end
			
			sp1kBtnEnableAll();
			appStorageMount(K_MEDIA_NANDF);
			
			uiRemainPixInit();
			appStillOsdInit(1);
			
			app_set_led(1);
			appSnapFlashLed(0);
			appSnapFlashAdjustsAe(0xff);
			break;
		case SP1K_MSG_NAND_ERROR:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			sp1kBtnDisableAll();
			if (osMsgSeek(SP1K_MSG_KEY_PRESS_POWER) == TRUE) {
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
				osMsgPost(SP1K_MSG_POWER_OFF);
			}
			else {
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			}
			uiOsdDialogDisp(ID_STR_FORMATTING___, 0);

			sp1kVfsFmt(0xC0);//format
			sts = sp1kVfsInit(1);
			if (sts == TRUE) {
#if TAG_TODO //by xian ???no use
				G_USR_FileIndex = 1; //DCF Init
#endif
				appStillOsdInit(1);
				uiRemainPixInit();
				uiUpdateRemainPix();
			}
			else {
				uiOsdDialogDisp(ID_STR_CARD_ERROR, 0);
			}
			//appUiBusyExit();
			sp1kBtnEnableAll();
			break;

			
		case SP1K_MSG_CARD_ERROR:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			if(sp1kDiskErrorCodeGet() == DISK_ERR_DETECT)
			{
				uiOsdDialogDisp(ID_STR_PLEASE_PLUG_OUT_CARD,1000);
			}
			else
			{
				uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
				appCardErrFmtSta(1);
			}
			break;
		case SP1K_MSG_CARD_NOT_SUPPORT:
			#if (STILL_NEW_ZOOM == 1)
			appStillSmoothZoomMsgProcFlow(msg);
			#endif
			uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
			sp1kBtnDisableAll();
			break;
		#if SUPPORT_RTC_OSD_UPDATE
		case SP1K_MSG_UPDATE_RTCTIME:
			appCurrentTimeShow(0);
			break;
		#endif
		case SP1K_MSG_SELFSNAP_BEEP:
			appSelfSnapHandle();
			break;
		case SP1K_MSG_SELFSNAP_COUNT:
			selfsnapbackfarflag = 1;
			sp1kBtnDisableAll();//mantis#42706
			appSelfSnapMSGSend();
			break;
		case SP1K_MSG_TP_CAL_COMPLETE:
			appStillOsdInit(1);
			puiPara->tpCorrect=0;
			break;
		case SP1K_MSG_MEDIA_VIDO_CAP_FINISH:
			if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON){
				RemainPictureNO-=6;
			}
			else if(puiPara->SpeedyCapRec== SPEEDY_CAPTURE_REC_ON){
				//printf("count = %lu\n", count);
				if(count >= (RemainPictureNO / 3)){
					RemainPictureNO = 0;
				}else{
					RemainPictureNO -= 3 * count;
				}
			}
			isUpdateNum=1;
			appStillOsdInit(1);
			sp1kBtnEnableAll();
			break;
		case SP1K_MSG_MEDIA_REC_DISK_FULL:
			HAL_GlobalReadGTimer(&speedyCapRecTime);
			count = (speedyCapRecTime - count)/1000;
			sp1kSpeedyCaptureStop();
			osMsgPost(SP1K_MSG_MEDIA_VIDO_CAP_FINISH);
			uiUpdateOSDMemFull();
			isUpdateNum=1;
			appStillOsdInit(1);
			sp1kBtnEnableAll();
			break;
		case SP1K_MSG_MEDIA_VIDO_CAP_ERROR:
			uiOsdDialogDisp(ID_STR_FILE_ERROR_, 1000);
			appStillOsdInit(1);
			sp1kBtnEnableAll();
			break;

		#if SUPPORT_PANEL_PROTECT
		case SP1K_MSG_PANEL_PROTECT:
			appPanelAutoProt();
			break;

		case SP1K_MSG_SYS_CLOSE_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(0);
			break;

		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(1);
			break;
		#endif

		
		#if SUPPORT_AF
		case SP1K_MSG_SYS_AF_START:
			//afdoingflag = 1;
			affarflag = 0;
			AF_AfCalStatusShow(0, 1);
			
			//AF_Set(512);
			AF_Set(0);
			sp1kHalWait(50);
			AF_startCalAF();
			break;
			
		case SP1K_MSG_SYS_AF_CAL:
			
			AF_CalAF();
			break;

		case SP1K_MSG_SYS_AF_BACK_FAR:
			if(afbackfaruiclearflag == 0){
				sp1kOsdClearCharExt(128, 88, 64, 64);
				}
			//puiPara->SnapAf = 0;
			
			AF_Set(0);
			appAFCtrl(0);

			break;
			
		case SP1K_MSG_SYS_AF_SUCCESS:
			
			AF_AfCalStatusShow(1, 1);
			sp1kHalWait(150);
			AF_AfCalStatusShow(1, 0);
			sp1kHalWait(150);
			AF_AfCalStatusShow(1, 1);
			sp1kHalWait(150);
			//AF_AfCalStatusShow(1, 0);
			
			if (app_adjust_macro_sta_get()) {
				//puiPara->SnapAf = 0;
				app_adjust_macro_sta_set(0);
				appAFCtrl(0);
			} else {
				affarflag = 1;
				//osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
			//afdoingflag = 0;
			//if(afdoingsnapflag){
			//	osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			//	afdoingsnapflag = 0;
			//}
			ENTER_CRITICAL(sts);
			sp1kADCSarSel(0x02);
			sp1kHalWait(50);
			adcVal = sp1kADCValueSnapIntr();
			EXIT_CRITICAL(sts);
			//printf("----------adc val : %d\n", adcVal);
			if (adcVal <= (0 + 10)){
				osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
			break;

		case SP1K_MSG_SYS_AF_FAIL:
			
			AF_AfCalStatusShow(0, 1);
			sp1kHalWait(150);
			AF_AfCalStatusShow(0, 0);
			sp1kHalWait(150);
			AF_AfCalStatusShow(0, 1);
			sp1kHalWait(150);
			//AF_AfCalStatusShow(0, 0);
			
			if (app_adjust_macro_sta_get()) {
				puiPara->SnapAf = 0;
				app_adjust_macro_sta_set(0);
				appAFCtrl(0);
			} else {
				//osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
			//afdoingflag = 0;
			break;
		#endif

		#if SUPPORT_AWB_ADJ
		case SP1K_MSG_AWB_OFF:
			sp1kAwbDisable();
			break;
			
		case SP1K_MSG_AWB_CHECK:
			{
				static UINT8 prev_luma = 0;
				xdata UINT8 curr_luma = 0, diff_luma = 0;

				sp1kAeStatusGet(SP1K_AE_STS_CURRENT_LUMA, &curr_luma);

				if (curr_luma > prev_luma) {
					diff_luma= curr_luma - prev_luma;
				}
				else {
					diff_luma= prev_luma - curr_luma;
				}

				//printf("prev luma : %d\n", (UINT16)prev_luma);
				//printf("curr luma : %d\n", (UINT16)curr_luma);
				//printf("diff luma : %d\n", (UINT16)diff_luma);

				prev_luma = curr_luma;

				if (diff_luma > 50) {
					sp1kAwbEnable();
					awb_auto_disable_flag = 1;
				}
				else {
					sp1kAwbDisable();
				}
			}
			break;
		#endif
		
		default:
			//printf("Unknown message:0x%bx\n", msg);
			break;
	}
}


void appStillViewInit(UINT8 InitMode)
{
	/* power control -- preview */
	//appModePowerCtrl(MODE_PREVIEW);

	UINT16 frontHSize,frontVSize;
	UINT16 cropVSize;
	UINT16 cropHsize;
	uiPara_t* puiPara ;
	UINT16 pvHSize,pvVSize;
	UINT16 snapXSize,snapYSize;
	UINT8 ret;

	sp1kSuperPreviewSet(0,1);
	
	ret = appSpeedyCaptureConfig();
	if(ret == IGNORE_DEFAULT)
	{
		return;
	}

	ret = appSpeedyCaptureRecConfig();
	if(ret == IGNORE_DEFAULT)
	{
		return;
	}


	if(appPvForceUpdateGet())
	{
		appPvForceUpdateOn(0);
		InitMode=SP1K_STILL_VIEW_INIT_FULL;
	}

	puiPara = appUiParaGet();
	appSnapParaInit();

	#if FUNC_HOST_IGBT_FLASH
	frontStrobeInit();//4 //fan.xiao modified 2011-09-21 , 17:25:43
	#endif

	if ( InitMode == SP1K_STILL_VIEW_INIT_FULL ) {
		if (appPreStateGet(0) == APP_STATE_POWER_ON) {
			static UINT8 run_once = 1;

			if (run_once) {
				UINT8 strwidth = 0, strheight = 0;
				UINT8 xpos, ypos, str[128] = {0};

				run_once = 0;
		
				sp1kOsdIndexSave();
				sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLUE, 4);
				sp1kOsdRectDraw(0, 0, 320 , 240);
				sp1kOsdIndexRestore();
				
				strGet(str, sizeof(str), ID_STR_WAITING___);
				xpos = (320 - strWidthGet(str)) >> 1;
				ypos = (240 - 32) >> 1;
				sp1kOsdStrDrawExt(xpos, ypos, str, 1);
			}
		}
		
		appPreviewCdspDispResSet();
		if (appPreStateGet(0) != APP_STATE_POWER_ON) {
			sp1kDispImgWinEn(0); // hlc, Mantis #0021510
		}
		
		sp1kCdspPvDynamicParaFullLoad(0x3c);

		if ( appPreStateGet(0) != APP_STATE_STILL_VIEW ) {
			appIconPsTabLoad(ICON_MODE_PV);
		}

		sp1kAeSetModeEn(AE_DISABLE);
		sp1kAwbDisable();
		sp1kFdDisable();
		sp1kDistDisable();
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);

		sp1kSnapSizeGet(&snapXSize,&snapYSize);

		pvHSize = K_Preview_Width;
		pvVSize = K_Preview_Height;

		#if SUPPORT_QVGA_SENSOR
			if(frontHSize*3 > frontVSize*4){/*sensor size 16:9*/
				cropHsize = frontHSize;
				cropVSize = frontVSize;
			}else{
				cropHsize = frontHSize;
				cropVSize = frontVSize;
			}

			if ( snapXSize*3 > snapYSize*4 ) {
				/* snap size 16:9*/
				pvVSize = pvVSize*3/4;

				//if(frontHSize*3 > frontVSize*4){/*sensor is 16:9*/
				//	cropHsize = frontHSize;
				//	cropVSize = frontVSize;
				//}
				//else{
				cropVSize = cropVSize*3/4;
				//}
			}
		#else
			/* sensor ratio */
			if ( frontHSize*3 > frontVSize*4 ) {
				/*sensor size 16:9*/
				cropHsize = frontVSize*4/3;
				cropVSize = frontVSize;
			}
			else {
				/*sensor size 4:3*/
				cropHsize = frontHSize;
				cropVSize = frontVSize;
			}
			if ( snapXSize*3 > snapYSize*4 ) {
				/* snap size 16:9*/
				pvVSize = pvVSize*3/4;

				if(frontHSize*3 > frontVSize*4){/*sensor is 16:9*/
					cropHsize = frontHSize;
					cropVSize = frontVSize;
				}
				else{
					cropVSize = cropVSize*3/4;
				}
			}
		#endif

	//	printf("H(%u),V(%u),CH(%u),CV(%u),frontHSize(%u),frontVSize(%u)\n",snapXSize,snapYSize,cropHsize,cropVSize,frontHSize,frontVSize);
		sp1kPreviewCropSizeSet(cropHsize,cropVSize);
		#if (STILL_NEW_ZOOM == 1)
		sp1kPvZoomCtrlParaSet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG,stillCurrLog);
		#else
		PvZoomCtrl.factor=stillZFctor;
		PvZoomCtrl.curLog = stillCurrLog;
		sp1kPvZoomSet(1, PvZoomCtrl.factor, 0);
		#endif
		#if 0
		#if (STILL_NEW_ZOOM == 0)
		sp1kPreviewSet(0, pvHSize, pvVSize, PvZoomCtrl.factor, 0);	/* will: still 320x240*/
		#else
		sp1kPreviewSet(0, pvHSize, pvVSize, stillZFctor, 0);	/* will: still 320x240*/
		#endif
		#endif
		sp1kPreviewSet(0, pvHSize, pvVSize, sp1kPvZoomFctGet(), 0);	/* will: still 320x240*/
		//sp1kCalibrationLscSet(1);
		sp1kHalCdspImgSrcSel(0);	// 0: image source from front

		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 10);

		if ( appPreStateGet(0) != APP_STATE_STILL_VIEW ) {
			//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 0); /* will mark*/
			sp1kDispImgWinEn(1); // hlc, Mantis #0021510
		}

		#if (STILL_NEW_ZOOM == 1)  /*fan.xiao Add for mantis:0045941 @2012/9/25 , AE enable is in MSG:  SP1K_MSG_DZOOM_3A_UPDATE*/
		//sp1kAeSetModeEn(AE_ENABLE);
		#else
		sp1kAeSetModeEn(AE_ENABLE);
		#endif
		//sp1kAwbReset();
		sp1kAwbEnable();

	}

	#if (FUNC_HOST_AF_FIXTURE == 1)
	appAfWinSet(1);
	#endif

	#if FUNC_HOST_MD
	sp1kMDInit();
	sp1kMDEnSet(MD_ENABLE);
	#endif

	#if FUNC_HOST_PANORAMA
	if ( 1 ) {//panorama enable need do it
		sp1kSnapParamSet( SP1K_SNAP_PARAM_PANORAMA_OPT, 1 );
		sp1kPanoramaParamSet(SP1K_PANORAMA_PARA_STITCH_PIXEL, 40);//16x
		sp1kPanoramaParamSet(SP1K_PANORAMA_PARA_FRAME_NUM,14);
		//sp1kPanoramaFinalSizeSet(1440,208);//QVGA
		sp1kPanoramaFinalSizeSet(2880,400);//VGA
		sp1kSnapSizeSet( 2880, 400 );
		sp1kPanoramaInit();
	}
	#endif

    #if FUNC_HOST_PF_PV
	if (appPreStateGet(0) != APP_STATE_MENU) {
		appPvPhotoFrameRecover();
	}
    #endif

	if ( puiPara->FDMode == FDMODE_ON ) {
		if ( !sp1kFdGetStatus() ) {
			sp1kFdReset();
			sp1kFdEnable();
		}

		if ( puiPara->FDMetering == FD_AE_ON ) {
			sp1kFdAeSetModeEn(1);
		}
		else {
			sp1kFdAeSetModeEn(0);
		}

		if ( puiPara->SmileMode == SMILEMODE_ON ) {
			sp1kFdSetSmile(1);
			sp1kFdParamSet(FD_PARAM_ShowMode, 1);
		}
		else {
			sp1kFdSetSmile(0);
			sp1kFdParamSet(FD_PARAM_ShowMode, 0);
		}
	}
	else {
		sp1kFdAeSetModeEn(0);
		sp1kFdSetSmile(0);
		sp1kFdDisable();
	}

	if ( puiPara->BlinkMode == BLINKMODE_ON) {
		sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,1);
	}
	else {
		sp1kSnapParamSet(SP1K_SNAP_PARAM_BLINK,0);
	}

	if ( puiPara->BlinkMode == BLINKMODE_ON ) {
	}

	if ( puiPara->AntiShake == PRV_ANTISHAKE_ON) {
		sp1kAeParamSet(SP1K_AE_PARA_CAP_ANTISHAKE_FPS, 15);
	}
	else {
		sp1kAeParamSet(SP1K_AE_PARA_CAP_ANTISHAKE_FPS, 0);
	}

	puiPara->FlashLed= 0;
	appSnapFlashLed(0);
	uiUpdateFlashLed(LCD_WIDTH, 0, 0);
	
	//if ((appPreStateGet(0) != APP_STATE_POWER_ON) || ( S_imgsizeflag == 1)){
		S_imgsizeflag = 0;
		appStillOsdInit(1); //lryy osd test
//	}

	if(enterMacroadjust == 1){
		osMsgPost(SP1K_MSG_Adjust_Macro);
	}

	if((enterVersionshow == 1)&&(!sp1kSD_CdGet())){
		enterVersionshow = 0;
		osMsgPost(SP1K_MSG_Version_Show);
	}
	
}

/**
 * @fn		  void appStillViewZoomIn(void)
 * @brief		app StillView Zoom In flow
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
#if (STILL_NEW_ZOOM == 0)
// hlc 0328, start
static UINT16 prvFact = 0;//jintao.liu 2008-4-4 add for 1x 4x
void appStillViewZoomIn(void) USING_0
{
	UINT16 newFact, PvZoomCtrlcurLogBack;
	UINT8 ZfStatus;

	PvZoomCtrlcurLogBack = PvZoomCtrl.curLog;

	PvZoomCtrl.curLog -= PvZoomCtrl.logStep;

	if (PvZoomCtrl.curLog < PvZoomCtrl.minLog) {
		PvZoomCtrl.curLog = PvZoomCtrl.minLog;
	}

	#if (STILL_ZOOM_FACTOR_LOOKUP_TABLE == 1)
	newFact = StillZoomFactor[(100-PvZoomCtrl.curLog)/2];
	#else
	newFact = (10000)/PvZoomCtrl.curLog;//zoom in 4X
	#endif

 	if(newFact <= PvZoomCtrl.MaxFct) {
		PvZoomCtrl.factor = newFact;
//		printf("fct:%d\n",PvZoomCtrl.factor );
	}
	else {
		PvZoomCtrl.curLog  = PvZoomCtrl.minLog;
		PvZoomCtrl.factor = PvZoomCtrl.MaxFct ;
		uiUpdateOSDPvZoom(400, 0);//mantis #27679 //zoom in 4X
		return;
	}

	ZfStatus = sp1kPvZoomSet(0, PvZoomCtrl.factor, 0);

	if (ZfStatus == FAIL) {
		PvZoomCtrl.curLog = PvZoomCtrlcurLogBack;
		return;
	}

	//jintao.liu 2008-4-4 add for 4x
	if(prvFact != PvZoomCtrl.factor)
	{
		prvFact = PvZoomCtrl.factor;
	}
	else
		return;
	//jintao.liu 2008-4-4 add for 4x end
	uiUpdateOSDPvZoom(PvZoomCtrl.factor, 0);
}
#endif

/**
 * @fn		  void appStillViewZoomIn(void)
 * @brief		app StillView Zoom In flow
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
#if (STILL_NEW_ZOOM == 1)
void appStillViewZoomIn(void) USING_0
{
	if (sp1kPvTeleSet() == FAIL) {
		//note : max zoom factor
	}
}
#endif


/**
 * @fn		  void appStillViewZoomOut(void)
 * @brief		app StillView Zoom Out flow
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
#if (STILL_NEW_ZOOM == 0)
void appStillViewZoomOut(void) USING_0
{
	UINT16 newFact, PvZoomCtrlcurLogBack;
	UINT8 ZfStatus;

	PvZoomCtrlcurLogBack = PvZoomCtrl.curLog;

	#if (STILL_ZOOM_FACTOR_LOOKUP_TABLE == 1)
	if(PvZoomCtrl.factor <= PvZoomCtrl.MinFct){
		return;
	}
	#endif

	PvZoomCtrl.curLog += PvZoomCtrl.logStep;

	if (PvZoomCtrl.curLog  > PvZoomCtrl.maxLog) {
		PvZoomCtrl.curLog  = PvZoomCtrl.maxLog;
	}

	#if (STILL_ZOOM_FACTOR_LOOKUP_TABLE == 1)
	newFact = StillZoomFactor[(100-PvZoomCtrl.curLog)/2];
	#else
	newFact = (10000)/PvZoomCtrl.curLog ;//zoom in 4X
	#endif

	if(newFact >= PvZoomCtrl.MinFct ) {
		PvZoomCtrl.factor = newFact;
	}
	else {
		PvZoomCtrl.curLog  = PvZoomCtrl.maxLog;
		PvZoomCtrl.factor = PvZoomCtrl.MinFct ;
		uiUpdateOSDPvZoom(100, 0);
		return;
	}
//	printf("fct:%d\n",PvZoomCtrl.factor );

	ZfStatus = sp1kPvZoomSet(0, PvZoomCtrl.factor, 0);

	if (ZfStatus == FAIL) {
		PvZoomCtrl.curLog = PvZoomCtrlcurLogBack;
		return;
	}

	//jintao.liu 2008-4-4 add for 1x
	if(prvFact != PvZoomCtrl.factor)
	{
		prvFact = PvZoomCtrl.factor;
	}
	else
		return;
	//jintao.liu 2008-4-4 add for 1x end
	uiUpdateOSDPvZoom(PvZoomCtrl.factor, 0);
}
#endif

/**
 * @fn		  void appStillViewZoomOut(void)
 * @brief		app StillView Zoom Out flow
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
#if (STILL_NEW_ZOOM == 1)
void appStillViewZoomOut(void) USING_0
{
	if (sp1kPvWideSet() == FAIL) {
		//note : min zoom factor
	}
}
#endif

/**
 * @fn		  UINT8 appStillViewAEAWBReadyGet(UINT8 start)
 * @brief		NONE
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author
 * @since
 */
// hlc 0328, end
#if 0
//suny add zoom function end
UINT8 appStillViewAEAWBReadyGet(UINT8 start)
{
#ifdef SNAP_WAIT_AEAWBREADY_OPTION
	return sp1kAeReadyGet(start );
#else
	return SUCCESS;
#endif
}
#endif

//-----------------------------------------------------------------------------
//appStillQualitySet
//-----------------------------------------------------------------------------
/**
 * @brief	select snap quality
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-03-12
 * @todo
 * @bug
*/
UINT8 appStillQualitySet(void)
{
	uiPara_t* uiPara;
	UINT8 QTable;
	UINT8 saturation=0;

	uiPara = appUiParaGet();
	switch(uiPara->ImageSize)
	{
		case PRV_SIZE_50M:
		case PRV_SIZE_44M:
		case PRV_SIZE_42M:
		case PRV_SIZE_36M:
			saturation=0x00;
			break;
			
		case PRV_SIZE_24M:
		case PRV_SIZE_20M:
			saturation=0x01;
			break;

		case PRV_SIZE_16M:
		case PRV_SIZE_12M:
		case PRV_SIZE_10M:
			saturation=0x02;
			break;

		case PRV_SIZE_8M:
		case PRV_SIZE_5M:
			saturation=0x10;
			break;

		case PRV_SIZE_40M:
		case PRV_SIZE_30M:
			saturation=0x42;
			break;
			
		case PRV_SIZE_3M:
			saturation=0x10;
			break;
		case PRV_SIZE_1M:
		case PRV_SIZE_1M_16_9:
			saturation=0x20;
			break;
		case PRV_SIZE_VGA:
			saturation=0x30;
			break ;
		case PRV_SIZE_9M:
		case PRV_SIZE_7M_16_9:
			saturation=0x40;
			break ;
		default:
			saturation=0x00;
			break;
	}

	switch(uiPara->ImageQuality)
	{
		case PRV_QUAL_SUPER:
			saturation|=0x00;
			break;
		case PRV_QUAL_FINE:
			saturation|=0x01;
			break;
		case PRV_QUAL_NORMAL:
			saturation|=0x02;
			break;
		default:
			saturation|=0x00;
			break;
	}

	switch(saturation)
	{
		case 0x00:
			QTable=30;//98
			break;
		case 0x01:
			QTable=40;//94
			break;
		case 0x02:
			QTable=50;//84
			break;
		case 0x10:
			QTable=60;//97
			break;
		case 0x11:
			QTable=80;//92
			break;
		case 0x12:
			QTable=80;
			break;
		case 0x20:
			QTable=80;//97
			break;
		case 0x21:
			QTable=80;//93
			break;
		case 0x22:
			QTable=80;//87
			break;
		case 0x30:
			QTable=80;//97
			break;
		case 0x31:
			QTable=80;//93
			break;
		case 0x32:
			QTable=80;//87
			break;
		case 0x40:
			QTable=80;//85
			break;
		case 0x41:
			QTable=80;//83
			break;
		case 0x42:
			QTable=70;
			break;
		default:
			QTable=80;//95
			break;

	}
	//added for file size @20080423 wei.sun begin
	if(sp1kSnapParamGet( SP1K_SNAP_PARAM_DOFLOW_OPT) &&(uiPara->DriverMode == PRV_SNAP_BURST))
	{
		QTable -= 5;
	}
	sp1kJpegQTableSet( QTable, 0, 0, 1 );

	return SUCCESS;
}

UINT8 appStillResolutionSet(UINT8 resolution)
{
	UINT16 frontHSize,frontVSize;
	UINT16 cropVSize;
	UINT16 cropHsize;

	sp1kFrontSnapSizeGet(0, &frontHSize, &frontVSize);

	//printf("frontHSize=%d, frontVSize=%d\n",frontHSize,frontVSize);

	if(frontHSize*3 > frontVSize*4)//16:9
	{
		cropHsize=frontVSize*4/3;
		cropVSize=frontVSize;
	}
	else
	{
		cropHsize=frontHSize;
		cropVSize=frontVSize;
	}

	//printf("cropHsize=%d, cropVSize=%d\n",cropHsize,cropVSize);

	switch(resolution) {
		case PRV_SIZE_50M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 8160, 6120 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_40M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 7200, 5400 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_30M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 6320, 4740 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_24M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 5600, 4200 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_20M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 5120, 3840 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_18M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 4896, 3672 );
			S_imgRatioChg = 1;
			break;

		case PRV_SIZE_16M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 4608, 3456 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_44M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 7664, 5748 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_12M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 3888, 2916 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_10M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 3888, 2916 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_9M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 3456, 2592 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_8M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 3264, 2448 );
			S_imgRatioChg = 0;
			break;
		case PRV_SIZE_7M_16_9:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 3648, 2048 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_5M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 2592, 1944 );
			S_imgRatioChg = 0;
			break;
		case PRV_SIZE_3M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 2048, 1536 );
			S_imgRatioChg = 0;
			break;
		case PRV_SIZE_2M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 1600, 1200 );
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_2M_16_9:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 1920, 1080 );//1920x1080
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_1M_16_9:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 1280, 720 );//1280x720
			S_imgRatioChg = 1;
			break;
		case PRV_SIZE_1M:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 1024, 768 );
			S_imgRatioChg = 0;
			break;
		case PRV_SIZE_VGA:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 640, 480 );
			S_imgRatioChg = 0;
			break;
		default:
			sp1kSnapCropSizeSet(cropHsize, cropVSize);
			sp1kSnapSizeSet( 2592, 1944 );
			S_imgRatioChg = 0;
			break;
	}
	return SUCCESS;
}

void appStillQuickViewSet(void)
{
	uiPara_t* puiEnviroment;

	puiEnviroment = appUiParaGet();
	switch(puiEnviroment->AutoReviewTime)
	{
		case QUICKVIEW_OFF:
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV_PERIOD, 0);
			break;
		case QUICKVIEW_1S:
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV_PERIOD, 1000);
			break;
		case QUICKVIEW_3S:
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV_PERIOD, 3000);
			break;
		default:
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_QV_PERIOD, 1000);
			break;
	}
}

#define ITEM_NUM			10  //8
#define DATA_LENGTH_MAX	20
#define EXIF_DEF 0
#define EXIF_CHG 1
void appExifParaInit(UINT8 chg)
{
	chg = chg;
#if 0 //hy
    struct IFD ifd[ITEM_NUM] = {
	{IFD0_TAG_IMGDESCRIPTION, 	IFD_DATATYPE_ASCII, 11, 0},
	{IFD0_TAG_MAKE,     IFD_DATATYPE_ASCII, 11, 0},
	{IFD0_TAG_MODEL,    IFD_DATATYPE_ASCII, 11, 0},
	//{IFD0_TAG_DATETIME, IFD_DATATYPE_ASCII, 20, 0},

	{EXIF_TAG_FNUMBER,      IFD_DATATYPE_RATIONAL, 1, 0},
	{EXIF_TAG_MAXAPERTURE,  IFD_DATATYPE_RATIONAL, 1, 0},
	{EXIF_TAG_FLASH, 		IFD_DATATYPE_SHORT,    1, 0x00000007},
	{EXIF_TAG_FOCALLENGTH,	IFD_DATATYPE_RATIONAL, 1, 0},
	{EXIF_TAG_WHITEBALANCE,	IFD_DATATYPE_SHORT,    1, 0x00000001},

	{EXIF_TAG_EXPOSURETIME,      IFD_DATATYPE_RATIONAL, 1, 0},
	{EXIF_TAG_ISOSPEEDRATINGS,      IFD_DATATYPE_SHORT, 1, 200},

    };
    UINT8 dataBuf[ITEM_NUM][DATA_LENGTH_MAX]={
	{"ICATCHTEK"},
	{"ICATCHTEK"},
	{"SPCA1628"},
	//{"2008:10:25 12:30:59"},

	{0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x0a},//big endian, 8/4
	{0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x0a},//big endian, 28/10
	{0},
	{0x00, 0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x0a},
	{0},

	{0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x0a},//big endian, 28/10
	{0},
    };
	UINT8 comment[]= " ";//Sunplus MM Inc.
	UINT8 ifdType;
	struct IFD *pIfd;
	UINT8* pval;
	UINT8 i=0;

	if(chg == EXIF_DEF)return;

	while(i<=ITEM_NUM){
		pIfd = &ifd[i];
		pval = dataBuf[i];
		if(i >= 3)
			ifdType = IFD_TYPE_EXIFIFD;
		else
			ifdType = IFD_TYPE_ZEROIFD;

		if(pIfd->tag == EXIF_TAG_FLASH || pIfd->tag == EXIF_TAG_WHITEBALANCE || pIfd->tag == EXIF_TAG_ISOSPEEDRATINGS)
			pval = NULL;

		if(i == ITEM_NUM){
			sp1kEXIFJPGCommentSet(comment,sizeof(comment));
		}else{
			sp1kEXIFTagValueSet(ifdType, pIfd->tag, pIfd->type, pIfd->count, pIfd->valueoffset,pval, 1); //big endian :1
		}
		i++;
	}
#endif
}

void appSnapParaInit(void)
{
	uiPara_t* snapPara;
	appExifParaInit(EXIF_DEF);
	snapPara = appUiParaGet();

	stampVideoModeSet(0);

	//sp1kSnapSizeSet( 2592, 1944 );//suny mark 20083.13
	appStillResolutionSet(snapPara->ImageSize);

	//sp1kSnapDzoomFactorSet( PvZoomCtrl.factor, PvZoomCtrl.factor );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_QUALITY, SP1K_SNAP_QUALITY_SUPER_FINE );//suny mark 20083.13

	sp1kSnapParamSet( SP1K_SNAP_PARAM_FORMAT, SP1K_SNAP_FORMAT_422 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );//suny mark 20083.13
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1 );//suny mark 20083.13
	if(snapPara->DriverMode == PRV_SNAP_BURST) {
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 1 );//suny mark 20083.13
	} else {
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );//suny mark 20083.13
	}

	appStillQuickViewSet();
	sp1kSnapParamSet( SP1K_SNAP_PARAM_PRE_BRC, 1 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_POST_BRC, 0 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,((~(snapPara->ShutterSound))&0x01));



	//sp1kSnapDzoomFactorSet( 100, 100 );

	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 0 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_YUV_OPT, 0 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 1 );

	sp1kSnapParamSet(SP1K_SNAP_PARAM_DZOOM_OPT, 1 );
	sp1kSnapParamSet(SP1K_SNAP_PARAM_CHANGERAW, 0 );

	sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );//burst mode

	sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );//AutoReview mode

	sp1kSnapParamSet( SP1K_SNAP_PARAM_BYPASS_QV_THB_OPT, 0 );//no AutoReview, Thumbnail, QuickView mode

	sp1kSnapParamSet( SP1K_SNAP_PARAM_DOFLOW_OPT, 0 );//high speed burst mode
	sp1kSnapParamSet( SP1K_SNAP_PARAM_DOFLOW_STORE_FIRST_OPT, 0 );//high speed burst mode store first

	sp1kSnapParamSet( SP1K_SNAP_PARAM_STAMP_X_RATIO, 0 );//Adjust datestamp x offset (right corner)
	sp1kSnapParamSet( SP1K_SNAP_PARAM_STAMP_Y_RATIO, 50);//Adjust datestamp y offset (bottom corner)

	sp1kSnapParamSet ( SP1K_SNAP_PARAM_STAMP_DT_DEFAULT_LOCATION_OPT, 1 );//use default date,time stamp location
}

//////////////////////////////
//ѡ�������Ƶ�ʵĺ���
//-----------------------------------------------------------------------------
//appElecFreqSet
//-----------------------------------------------------------------------------
/**
 * @brief	set electronic frequcy
 * @param	void
 * @retval	success
 * @see
 * @author	jintao.liu
 * @since	2008-03-31
 * @todo
 * @bug
*/
UINT8 appElecFreqSet(void)
{
	uiPara_t* uiPara;

	uiPara = appUiParaGet();
	switch(uiPara->LightFreq)
	{
		case PRV_FREQ_50:
			sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
			/*You should set the PV_MODE according to the size which the sensor output !*/
			appCalBpPvModeSet(PV_MODE_HD);
			break;
		case PRV_FREQ_60:
			sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
			/*You should set the PV_MODE according to the size which the sensor output !*/
			appCalBpPvModeSet(PV_MODE_HD);
			break;
		default:
			sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
			/*You should set the PV_MODE according to the size which the sensor output !*/
			appCalBpPvModeSet(PV_MODE_HD);
			break;
	}
	return SUCCESS;
}

/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */

#if (FUNC_HOST_PF_PV)

UINT8 appPvPhotoFrameSet(UINT8 idx)
{
	UINT16 resW, resH;
	sp1kPvPhotofrmEnable(0);
	if (idx == PRV_PHOTO_FRAME_OFF)
	{
		sp1kSnapParamSet( SP1K_SNAP_PARAM_PHOTO_FRM_OPT, 0 );	//xian ++ 20081216
		return FAIL;
	}

	#if (STILL_NEW_ZOOM == 0)
	//add for mantis #36993@start
	PvZoomCtrl.curLog=100;
	PvZoomCtrl.factor=100;
	sp1kPvZoomSet(1, PvZoomCtrl.factor, 0);	// TQ@20100702
	uiUpdateOSDPvZoom(PvZoomCtrl.factor, 0);
	#else
	sp1kPvZoomCtrlParaSet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG, 0);
	sp1kPvZoomSet(1, sp1kPvZoomFctGet(), 0);	// TQ@20100702
	uiUpdateOSDPvZoom(sp1kPvZoomFctGet(), 0);
	#endif
	if (appPvPhotoFrameResLoad(idx, &resW, &resH) != SUCCESS)
	{
		ASSERT(0,0);
		return FAIL;
	}
    //xian ++ 20081216
	sp1kSnapParamSet(SP1K_SNAP_PARAM_PH_WIDTH, resW);
	sp1kSnapParamSet(SP1K_SNAP_PARAM_PH_HEIGHT, resH);
	sp1kSnapParamSet( SP1K_SNAP_PARAM_PHOTO_FRM_OPT, 1 );
	//xian --
	sp1kPvDlyPhofrmBlendCntSet(1);
	sp1kPvPhotofrmEnable(1);
	// menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
	return SUCCESS;
}

#endif

//-----------------------------------------------------------------------------
//appPbPhotoFrameSet
//-----------------------------------------------------------------------------
/**
 * @brief	photo frame for playback (only for view)
 * @param	void
 * @retval	success
 * @see
 * @author	jintao.liu
 * @since	2008-06-16
 * @todo
 * @bug
*/
#if (FUNC_HOST_PF_PB)
UINT8 appPbPhotoFrameSet(UINT8 idx, UINT8 bSave)
{
	UINT16 ResId;
	UINT32 ResAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT8 OsdMaxX, OsdMaxY;
	UINT8 ret;
	UINT16 width = 0, height = 0;
	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	if (idx == PRV_PHOTO_FRAME_OFF)
	{
		RESUME_DISPBUFF();
		return FAIL;
	}

	switch (idx)
	{
		case PRV_PHOTO_FRAME_A:
			ResId = RES_PHOTO_FRAME_A;
			break;
		case PRV_PHOTO_FRAME_B:
			ResId = RES_PHOTO_FRAME_B;
			break;
		case PRV_PHOTO_FRAME_C:
			ResId = RES_PHOTO_FRAME_C;
			break;
		case PRV_PHOTO_FRAME_D:
			ResId = RES_PHOTO_FRAME_D;
			break;
		default:
			ResId = RES_PHOTO_FRAME_A;
			break;
	}
	if(bSave)
	{
		sp1kOsdClear(0);
		appUiBusyWait(250,0);
		ret = pbPhotoFrameSave( DOS_FileIndexGet( pbFile.dwIndex ),ResId);
		pbFile.dwIndex=sp1kPbFileCntGet();
		appUiBusyExit();
	}
	else
	{
		RESUME_DISPBUFF();//resume image before process
		//printf("resAddr = %lx; endAddr = %lx\n", ResAddr,K_SDRAM_TotalSize-256UL);
		ret = sp1kdecodePhotoFrameImage(ResId, &ResAddr, K_SDRAM_TotalSize-256UL, &width, &height );
		if(SUCCESS == ret)
		{
			sp1kHalGprmCpy(ResAddr, width, height, 0, 0,
			sp1kPbDispBuffGet(1),  PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0, 0, PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0x9F, 1);//9f
		}

		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
	}
	return SUCCESS;
}
#endif

/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */

UINT8 appStillViewAEAWBWait(void)
{
	//static UINT8 AEAWB_ReadyFlg = 0;
	//UINT8 sts = FAIL;

	if ( pvWaitCnt++ > PV_3A_WAIT_CNT_MAX ) {
		/* ae/awb waiting timeout */
		pvWaitCnt = 0;
		return SUCCESS;
	}

	#ifdef SNAP_WAIT_AEAWBREADY_OPTION
	if(sp1kAeReadyGet(4/*3 4 7*/)){
		osMsgPost(SP1K_MSG_3A_WAIT_PROC);
		return FAIL;
	}
	#endif
	
	return SUCCESS;
	
	#if 0
	if (AEAWB_ReadyFlg == 0) {
		sts = appStillViewAEAWBReadyGet(0);
		if(sts == SUCCESS) {
			AEAWB_ReadyFlg = 1;// - AEAWB_ReadyFlg;
		}
		sts = FAIL;
	} else if (AEAWB_ReadyFlg == 1){
		sts = appStillViewAEAWBReadyGet(1);
		if(sts == SUCCESS) {
			AEAWB_ReadyFlg = 1 - AEAWB_ReadyFlg;
		}
	} else {
		AEAWB_ReadyFlg = 0; /* exception handle */
	}

	/* msg resend for waiting AE/AWB ready */
	if ( sts != SUCCESS ) {
		osMsgPost(SP1K_MSG_3A_WAIT_PROC);
	}
	return sts;
	#endif
}

/* environment check */
UINT8 appSnapEnvChk(void)
{
	UINT8 ret = FAIL;

	if(sp1kVideoCapParamGet(SP1K_VD_CAP_MOD) == SP1K_SPEEDCAP_CLIP){
		if(sp1kSpeedyCapStsGet() == SPEEDCAP_STS_RUNNING){
			HAL_GlobalReadGTimer(&speedyCapRecTime);
			count = (speedyCapRecTime - count)/1000;
			sp1kSpeedyCaptureStop();
			osMsgPost(SP1K_MSG_MEDIA_VIDO_CAP_FINISH);
			return FAIL;
		}
	}

	/* add for tv play mode */
#if(TV_OPTION == TV_PLAY_OPTION)
	if(appPanelTypeGet() == 0) {
		return FAIL;
	}
#endif

	/* check card status */
#if (!NAND_SUPPORT)//2008-7-14 mantis #28129
	if(0 == appSDPlugStatusGet())
	{
		uiOsdDialogDisp(ID_STR_NO_CARD_, 1000);
		appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		return FAIL;
	}
#endif
	if(1 == appSDProtectStatusGet())//jintao.liu 2008-5-6 for sd lock
	{
		uiOsdDialogDisp(ID_STR_CARD_LOCKED_, 1000);
		appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		return ret;
	}

/***************Just for QA test @start***************************************/
	if(AutoTestMode == 0x55)
	{
		if(RemainPictureNO<10)
		{
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			return FAIL;
		}

	}

/***************Just for QA test @end***************************************/
	if ( appCardErrFmtErrChk() ) {
		uiOsdDialogDisp(ID_STR_CARD_ERROR, 1000);
		appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		return FAIL;
	}
	else {
		/* check remain snap number */
		if(RemainPictureNO==0) {
			uiUpdateOSDMemFull();//show mem full
			isUpdateNum=1;
			appStillOsdInit(1);//yp add 2011.06.27
			appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
                /* yp add from*/
			#ifdef _ENABLE_LCD_OFF_
			appOsdModeSw(0);
                     #endif /*_ENABLE_LCD_OFF_*/
		/*yp add end */
			return FAIL;
		}
	}
	/* check the unit status */
	if ( pvZoomSts != 0 ) {
		/* the unit is still in zoom status */
		//printf("pvzmsts=%bx\n",pvZoomSts);
		return FAIL;
	}

	ret = SUCCESS;
	return ret;
}

UINT8 appSnapParaLoad(void)
{
	uiPara_t* snapPara;
	UINT8 ret = FAIL;
    UINT32 phAddr, resAddr = K_SDRAM_ImagePlaybackBufAddr;
    UINT16 phW, phH;

	snapPara = appUiParaGet();
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,((~(snapPara->ShutterSound))&0x01));
	if(snapPara->Volume ==0){
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,0);
	}
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);

	appStillQuickViewSet();
	//xian ++ 20081216
	if(sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT)){
        phW = sp1kSnapParamGet(SP1K_SNAP_PARAM_PH_WIDTH);
        phH = sp1kSnapParamGet(SP1K_SNAP_PARAM_PH_HEIGHT);
        phAddr = (UINT32)sp1kSnapParamGet(SP1K_SNAP_PARAM_PH_LOW_ADDR) +
            ((UINT32)sp1kSnapParamGet(SP1K_SNAP_PARAM_PH_HIGH_ADDR)<<16);
        if(phAddr >= resAddr && phAddr <= resAddr + (UINT32)phW*phH*2){
           // printf("ph addr over!");
            return FAIL;
        }

        sp1kHalGprmCpy(resAddr, phW, phH, 0, 0, phAddr, phW, phH, 0, 0, phW, phH, 0, 1);
    }
    
	//xian --

    //xian -- 20081216, move out for self snap
	//if(appDriveModeSet() !=SUCCESS) {
	//	return FAIL;
	//}
    //xian --

	ret = SUCCESS;
	return ret;
}

UINT8 appSnapPreProc(void)
{
	uiPara_t* snapPara = appUiParaGet();

	/*disable other key in burst mode*/
	if(snapPara->DriverMode == PRV_SNAP_BURST){
		sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0);
		sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0);
		sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0x02);
	} else {
		sp1kBtnDisableAll();
	}
	
	return SUCCESS;
}

UINT8 appSnapPostProc(UINT16 msg)
{
	UINT8 ret = SUCCESS;

	if ( msg == SP1K_SNAP_MSG_FILE_WRITE_ERROR ) {
		uiUpdateOSDMemFull();
		isUpdateNum=1;
		appStillViewInit(SP1K_STILL_VIEW_INIT_SMART);
		ret = FAIL;
	}
	else {
		if ( UI_LCD_ON_WITH_OSD == appGetOsdMode() ) {
			// UI update remain pix count
			//uiRemainPixShow();
			//uiUpdateRemainPix();
		}
	}
#ifdef _ENABLE_LCD_OFF_
	appOsdModeSw(2);//reload
#endif /*_ENABLE_LCD_OFF_*/

	/*disable other key in burst mode*/
	sp1kBtnEnableAll();

	/* power control -- preview */
	//appModePowerCtrl(MODE_PREVIEW);

	return ret;
}

UINT8 appSnapBurstCheck( UINT8 burstCnt )
{
	UINT8 burstChk = osMsgSeek( SP1K_MSG_KEY_RELEASE_S2 );

	UINT8 sts;
	UINT8 abort = TRUE;

	sts = sp1kBtnChk(SP1K_BTN_S2);

	#if 0 //suny mark for capture 3 images per press snap key in burst mode
		if( burstChk == TRUE || sts != KEY_STATUS_PRESSED ) {
			abort = FALSE;
		}
	#else
		if( burstCnt >= (UINT8)3 ) {
			abort = FALSE;
		}
	#endif

	return abort;
}

/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
//xian ++ 20081216

#if FUNC_HOST_PF_PV
void appPvPhotoFrameRecover(void)
{
	UINT16 phW, phH;
	uiPara_t* pUiPara;
	pUiPara = appUiParaGet();

	if(sp1kSnapParamGet(SP1K_SNAP_PARAM_DOFLOW_OPT) &&(pUiPara ->DriverMode == PRV_SNAP_BURST) )
	{
		sp1kPvPhotofrmEnable(0);
		return;//burst don't support photoframe
	}

	if(!sp1kSnapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT))
	{
		return;
	}
	if (pUiPara->PhotoFrame != PRV_PHOTO_FRAME_OFF)
	{
		if (appPvPhotoFrameResLoad(pUiPara->PhotoFrame, &phW, &phH) != SUCCESS)
		{
			ASSERT(0,0);
			return;
		}

		//printf("load ph h=%d,v=%d\n",phW,phH);

		//xian ++ 20081216
		sp1kSnapParamSet(SP1K_SNAP_PARAM_PH_WIDTH, phW);
		sp1kSnapParamSet(SP1K_SNAP_PARAM_PH_HEIGHT, phH);
		sp1kSnapParamSet( SP1K_SNAP_PARAM_PHOTO_FRM_OPT, 1 );
		//xian --
		sp1kPvPhotofrmEnable(1);
	}
}

UINT8 appPvPhotoFrameResLoad(UINT8 idx, UINT16* resW, UINT16* resH)
{
	UINT32 ResId;
	UINT32 SrcAddr = K_SDRAM_PvPFJpgAddr;
	UINT32 ResAddr = K_SDRAM_PvPFYUVAddr;
//	struct Exif_Decompress_Parameter MainImgPara = {0};//xl
	ExifImage_t MainImgPara;//xl
	switch (idx)
	{
		case PRV_PHOTO_FRAME_A:
			ResId = RES_PHOTO_FRAME_A;
			break;
		case PRV_PHOTO_FRAME_B:
			ResId = RES_PHOTO_FRAME_B;
			break;
		case PRV_PHOTO_FRAME_C:
			ResId = RES_PHOTO_FRAME_C;
			break;
		case PRV_PHOTO_FRAME_D:
			ResId = RES_PHOTO_FRAME_D;
			break;
		case PRV_PHOTO_FRAME_5:
		case PRV_PHOTO_FRAME_6:
		case PRV_PHOTO_FRAME_7:
		case PRV_PHOTO_FRAME_8:
		case PRV_PHOTO_FRAME_9:
		case PRV_PHOTO_FRAME_10:
		case PRV_PHOTO_FRAME_11:
		case PRV_PHOTO_FRAME_12:
		case PRV_PHOTO_FRAME_13:
		case PRV_PHOTO_FRAME_14:
		case PRV_PHOTO_FRAME_15:
		case PRV_PHOTO_FRAME_16:
		case PRV_PHOTO_FRAME_17:
		case PRV_PHOTO_FRAME_18:
		case PRV_PHOTO_FRAME_19:
		case PRV_PHOTO_FRAME_20:
		case PRV_PHOTO_FRAME_21:
			ResId = 0x101 + (idx-PRV_PHOTO_FRAME_5);
			break;
	}

	/* photo frame big image decoder */
	if (appBGImgDec(ResId, &MainImgPara, SrcAddr, ResAddr, 1)) {
		sp1kDramFileSave("PHFM0001.JPG",K_SDRAM_PvPFJpgAddr,K_SDRAM_PvPFJpgSize);
		sp1kDramFileSave("PHFM0001.YUV",K_SDRAM_PvPFYUVAddr,K_SDRAM_PvPFYUVSize*2);
		ASSERT(0,1);
		return FAIL;
	}

/*	*resW = MainImgPara.Width;
	*resH = MainImgPara.Height;
*/	//xl
	*resW = MainImgPara.width;
	*resH = MainImgPara.height;

	sp1kSnapPhotoFrameParaSet(ResId,SrcAddr);	/* Past parameter to Snap_task.c */

	return SUCCESS;
}

#endif

//xian --
static UINT8 pvUpdate = 0;
void appPvForceUpdateOn(UINT8 on)
{
	pvUpdate= on;
}
UINT8 appPvForceUpdateGet(void)
{
	return pvUpdate;
}

/**
 * @brief		appISOSet
 * @param	ISOValue
 * @retval	SUCCESS
 * @see
 * @author	Li tuhong
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT8 appISOSet(UINT8 ISOValue)
{
	sp1kAeIsoSet(ISOValue);
	return SUCCESS;
}

/**
 * @brief		appAntiShakeSet
 * @param	antiShake
 * @retval	NULL
 * @see
 * @author	Li tuhong
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

void appAntiShakeSet(UINT8 antiShake)
{
	switch(antiShake){
		case PRV_ANTISHAKE_ON:
			//printf("AntiShake on\n");
			sp1kAeAntiShakeFpsSet(20);
			break;
		case PRV_ANTISHAKE_OFF:
		default:
			//printf("AntiShake off\n");
			sp1kAeAntiShakeFpsSet(0);
			break;
	}
}

/**
 * @brief		appCustomizedStampSet
 * @param	NULL
 * @retval	NULL
 * @see
 * @author	LinJieCheng
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appCustomizeStampSet(void)
{
	//return;
	/* set background function enable */
	sp1kStampBackgroundEnable(1);

	/* set customer string function enable */
	//sp1kStampTextEnable(STAMP_TEXT,1);

	/* set customer string */
	//sp1kStampTextModifyByString(STAMP_TEXT, "123");

	/* set background h,v size */
	sp1kSnapParamSet( SP1K_SNAP_PARAM_BACKGROUND_WIDTH, 640/*640*/ );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_BACKGROUND_HEIGHT, 32/*60*/ );
}

/**
 * @brief		appSnapBackgroundResLoad
 * @param	dstAddrWord : load background destination buffer address
 *                bufByteSize : buffer size
 * @retval	FAIL, SUCCESS
 * @see
 * @author	LinJieCheng
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appSnapBackgroundResLoad(UINT32 dstAddrWord, UINT32 bufByteSize)
{
	UINT32 resId, bgSize;

	resId = 0x2d;

	sp1kNandRsvSizeGet(resId, NULL, &bgSize); // Res_0x2.res = calLen.bin

	if (bgSize > bufByteSize) {
		return FAIL;
	}

	if (!sp1kNandRsvRead(resId, (dstAddrWord << 1))) {
		return FAIL;
	}

	return SUCCESS;
}

/**
 * @brief		appSnapHostStamp
 * @param	NULL
 * @retval	FAIL, SUCCESS
 * @see      Fast Burst Capture + Host Draw Background Function -> libSnap need turn on SNAP_DOFLOW_BURST_CAPTURE_BG_STAMP_UPDATE Option
 * @author	LinJieCheng
 * @since		2011-03-09
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 capNum = 0;
UINT8 appSnapHostStamp(void)
{
	#if 0//Fast Burst Capture + Host Draw Background Function Example Code
	UINT32 w,h,add;
	//printf("capNum=%bu\n",capNum);
	sp1kStampBackgroundGet(&add,&w,&h);
	switch ( capNum ) {
		case 0:
			sp1kStampTextModifyByString(STAMP_TEXT, "1/3  116F");
			sp1kStampTextSet(STAMP_TEXT,(w>>2)+48,(h-(h>>1))>>1,0xba);
			capNum++;
			break;
		case 1:
			sp1kStampTextModifyByString(STAMP_TEXT, "2/3  109F");
			sp1kStampTextSet(STAMP_TEXT,(w>>2)+48,(h-(h>>1))>>1,0xba);
			capNum++;
			break;
		case 2:
			sp1kStampTextModifyByString(STAMP_TEXT, "3/3  099F");
			sp1kStampTextSet(STAMP_TEXT,(w>>2)+48,(h-(h>>1))>>1,0xba);
			capNum = 0;
			break;
		default:
			ASSERT(0,1);
	}
	#endif

	return SUCCESS;
}

/**
 * @brief		appUserDefineMeter
 * @param	w : hsize, h : vsize
 * @retval	user meter point
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8* appUserDefineMeter(UINT8 w, UINT8 h)
{
	if (sizeof(userDefineMeter) >= (w*h)) {
		return &userDefineMeter[0];
	}
	else {
		return NULL;
	}
}

/**
 * @brief		appUserMeterModeSet
 * @param	en :
 *\n							  enable : 1
 *\n							   disable : 0
 * @retval	NONE
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appUserMeterModeSet(UINT8 en){
	userMeterEn = en;
}

/**
 * @brief		appUserMeterModeGet
 * @param	NONE
 * @retval	userMeterEn : user meter
 *\n                              enable : 1
 *\n                              disable : 0
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 appUserMeterModeGet(void){
	return userMeterEn;
}

/**
 * @brief		appDynamicProgrammingUserMeterFlow
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	LinJieCheng
 * @since	2011-07-28
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void appDynamicProgrammingUserMeterFlow(void)
{
	UINT8 cnt, h, v;

	sp1kAeWindowDimensionGet(&h, &v);

	if ((h*v == sizeof(aeWindowInformation)) && (h*v == sizeof(userDefineMeter))) {

		sp1kAeWindowArrayGet(&aeWindowInformation[0]);

		for(cnt = 0; cnt < sizeof(aeWindowInformation); cnt++) {
			if (aeWindowInformation[cnt] > 200) {
				if (userDefineMeter[cnt] <= 48) {
					userDefineMeter[cnt]++;
				}
			}
			else {
				if (userDefineMeter[cnt] != 1) {
					userDefineMeter[cnt]--;
				}
			}
		}

		sp1kAeDynamicMeterChangeSet();
	}
}

#if (FUNC_HOST_AF_FIXTURE == 1)
UINT8 appAfWinSet(UINT8 En)
{
	UINT16 frontHSize,frontVSize;
	UINT16 cropHSize,cropVSize;

	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewCropSizeGet(&cropHSize,&cropVSize);

	sp1kAfWinCfg(frontHSize,frontVSize,cropHSize,cropVSize);

	sp1kAfWinEnSet(En);

	return SUCCESS;
}
#endif

/**
 * @brief		appStillSmoothZoomMsgProcFlow
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
#if (STILL_NEW_ZOOM == 1)
UINT8 appStillSmoothZoomMsgProcFlow (UINT16 sysMsg)
{
	uiPara_t* pUiPara;

	switch ( sysMsg ) {
		case 1:
		case SP1K_MSG_CARD_PLUG_IN:
		case SP1K_MSG_CARD_PLUG_OUT:
		case SP1K_MSG_USB_PLUG_IN:
		case SP1K_MSG_TV_PLUG_IN:
		case SP1K_MSG_TV_PLUG_OUT:
		case SP1K_MSG_HDMI_PLUG_IN:
		case SP1K_MSG_CARD_ERROR:
		case SP1K_MSG_CARD_NOT_SUPPORT:
		case SP1K_MSG_NAND_ERROR:
		case SP1K_MSG_BAT_EMPTY:
			if (sp1kPvSmoothZoomGet() == 1) {
				sp1kPvSmoothZoomSet(0);
				sp1kPvZoom3APostSet();
				sp1kAeSetModeEn(AE_ENABLE);
				sp1kAwbEnable();
			}
			while(sp1kPvSmoothZoomStatusGet() == FAIL);
			pUiPara = appUiParaGet();
			if(pUiPara->SpeedyCap == SPEEDY_CAPTURE_ON || pUiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
			{
				stillZFctor=sp1kVideoZoomFctGet();
				stillCurrLog=sp1kVideoZoomCtrlParaGet(SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG);
			}
			else
			{
				stillZFctor=sp1kPvZoomFctGet();
				stillCurrLog=sp1kPvZoomCtrlParaGet(SP1K_PV_ZOOM_CTRL_PARA_CURRENT_LOG);
			}
			break;
		case SP1K_MSG_KEY_PRESS_S2:
			if (sp1kPvSmoothZoomStatusGet() == FAIL){
				return FAIL;
			}
			break;
	}

	return SUCCESS;
}
#endif



