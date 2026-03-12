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
#include "host_func_opt.h"

#if FUNC_HOST_CAL
#include "app_init.h"
#include "os_msg.h"
#include "app_msg.h"
#include "dos32.h"
#include "hal_gprm.h"
#include "hal_dram.h"

#include "sp1k_calibration_api.h"//calibration api's prototype
#include "sp1k_snap_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_bp_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_util_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_cdsp_api.h"

#include "app_menu.h"
#include "app_menu_core.h"
#include "app_menu_api.h"
#include "app_menu_tab.h"
#include "app_menu_event.h"
#include "app_cal_api.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_sts_pwroff.h"
#include "app_audio_play.h"
#include "app_pwr_mgr.h"
#include "app_ui_osd.h"
#include "app_ui_para.h"
#include "button_def.h"
#include "initio.h"
#include "Key_scan.h"
#include "bp_private.h"
#include "app_still_view.h"
#include "Doslink.h "
#include "front_logi.h"
#include "dcf_api.h"
#include "dbg_def.h"
#include "hal_cdsp_iq.h"
#include "hal_cdsp_core.h"
#include "sp1k_disp_api.h"
#include "host_func_opt.h"
#include "Sp1k_hal_api.h"
#include "Func_ptr.h"
#include "reg_def.h"
#include "app_tp_scan.h"
#include "sp1k_af_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if !TAG_TODO //code clean up function list
#define sp1kCalibrationLoadCriteria(a)
#define sp1kCalibrationSaveIq(a)
//#define sp1kCalibrationAEConfig(a, b)
//#define sp1kCalBpProc(a)
#endif

#define ABS(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x))  

#define FILTER	2

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern xdata FramePos_t AFWINDOW[5];
extern xdata UINT32 afBuf[5];
xdata UINT32 olderAfBuf[5];
xdata UINT32 oldestAfbuf[5];
UINT8 id[4];	//storage AE Test Gid Step
static UINT8 AEGid = 50;
UINT16 currPos = 0;

#if SUPPORT_AUTO_TEST
UINT8 autoTestOpenFlag = 0;
#endif
 
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 static bpPara_t code sBpPara = {
	1280, /* preview HD raw image width */
	960,  /* preview HD raw image height */

	640,  /* preview raw image width */
	480,  /* preview raw image width */
	
	2592, /* capture raw image width */
	1944, /* capture raw image height */

	30, /* threshold */
	40,//30, /* threshold */

	BADPIXEL_PV_NUM_MAX,//4096, /* preview badpixel max number */
	BADPIXEL_PV_NUM_MAX,//4096, /* preview badpixel max number */
	BADPIXEL_CAP_NUM_MAX -4,//8000, /* capture badpixel max number */
	0,
	0,

	0x55aa, /* EOF */
};
extern xdata UINT8 calibrationObData[ SP1K_CALIBRATION_OB_MAX ];
extern xdata UINT8 calibrationAwbData[ SP1K_CALIBRATION_AWB_MAX ];
extern xdata UINT8 calibrationPvObData[ SP1K_CALIBRATION_PV_OB_MAX ];

#if SUPPORT_SPI_WRITE_CONTROL
extern UINT8 mastWriteSpiFlag;
#endif




static UINT8 appCalBpSnapRaw(UINT8 mode,UINT8 type, UINT8 gain, UINT32 shutter);
static void appCalCfgDataRst(void);
static UINT8 appCalBpPro(void);
static UINT8 appCalBpCapDataArrange(void);
static void appCalRGBvalueShow (UINT8 * value);
static UINT8 appCalStorageChk(void);
static void appDramReadDWord(UINT32 addr ,UINT16* val_x,UINT16* val_y);
static void appDramWriteDWord(UINT32 addr,UINT16 a, UINT16 b);

extern UINT8 appStillViewAEAWBWait(void);
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalInit(void)
{
	appAutoOffEn(0);//disable auto power off
	
	sp1kCalibrationLoadCriteria( SP1K_CALIBRATION_CRITERIA_FILE_ID );	//read ae test parameters
	sp1kCalibrationAeTestGidStepGet( id, id+1, id+2, id+3 );			//and save
	sp1kCalibrationDefaultSizeSet();									//init size

	appCalCfgDataRst();
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalSnapRaw(void)
{
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 1 ); 
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
	sp1kSnapTask();	
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalSnapPureRaw(void)
{
	uiPara_t* puiPara = appUiParaGet();

	/*OB LSC AWB Gamma turning off*/
	sp1kCalibrationObSet(0);
	sp1kCalibrationLscSet(0);
	sp1kCalibrationAWBSet(0);
	sp1kCalibrationGammaSet(0);
	
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 1 ); 
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
	sp1kSnapTask();

	sp1kCalibrationObSet(puiPara->cal_ob);
	sp1kCalibrationLscSet(puiPara->cal_lc);
	sp1kCalibrationAWBSet(puiPara->cal_awb);
	sp1kCalibrationGammaSet(puiPara->cal_gma);
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalSnapIQRaw(void)
{
	//No related code		
#if 0
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );
	//sp1kCalibrationObSet(0);	//set to manual OB mode

	// snap
	//for (AEGid = 0; AEGid < 101; AEGid++)
	{
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
		sp1kSnapTask();

		appCalRGBvalueShow(calibrationObData);
		
		sp1kCalibrationSaveIq( SP1K_CALIBRATION_OB_FILE_ID );
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}

	//restore ob configure
	//sp1kCalibrationObSet(puiPara->cal_ob);
#else
	//#if (LSC_CAL_MODE_EN == 1 || COLOR_CHECK_CAL_MODE_EN == 1)
	UINT8 curAeIdx, frmAeIdx;
	uiPara_t* puiPara;
	UINT16 frontHSize, frontVSize;
	
	puiPara = appUiParaGet();

	sp1kAeSetModeEn(AE_DISABLE);

	sp1kFrontSnapSizeGet(0, &frontHSize, &frontVSize);
	sp1kSnapCropSizeSet(frontHSize, frontVSize);
	sp1kSnapSizeSet( frontHSize, frontVSize );
	
	/* get AE current idx */
	sp1kAeStatusGet(SP1K_AE_gId, &curAeIdx);
	
	for (frmAeIdx = curAeIdx-1; frmAeIdx <= curAeIdx+1; frmAeIdx++) {
		/*OB LSC AWB Gamma turning off*/
		sp1kCalibrationObSet(0);
		sp1kCalibrationLscSet(0);
		sp1kCalibrationAWBSet(0);
		sp1kCalibrationGammaSet(0);
		
		sp1kCalibrationAEConfig(frmAeIdx, 50);
		
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 1 ); 
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
		sp1kSnapTask();
	}

	/*OB LSC AWB Gamma turning on*/
	sp1kCalibrationObSet(puiPara->cal_ob);
	sp1kCalibrationLscSet(puiPara->cal_lc);
	sp1kCalibrationAWBSet(puiPara->cal_awb);
	sp1kCalibrationGammaSet(puiPara->cal_gma);
	HAL_CdspBPEnSet(1);
	sp1kAeSetModeEn(puiPara->cal_ae);
	sp1kAwbEnable();
	//#endif
#endif	
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalOB(void)
{
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );
	//sp1kCalibrationObSet(0);	//set to manual OB mode

	// snap
	//for (AEGid = 0; AEGid < 101; AEGid++)
	{
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
		sp1kSnapTask();

		appCalRGBvalueShow(calibrationObData);
		
		sp1kCalibrationSaveIq( SP1K_CALIBRATION_OB_FILE_ID );
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}

	//restore ob configure
	//sp1kCalibrationObSet(puiPara->cal_ob);		
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalAutoOB(void)
{
	UINT8 gid_bak;
	uiPara_t* puiPara = appUiParaGet();
	
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 0 ); //don not store
	sp1kCalibrationObSet(0);	//set to manual OB mode

	//backup gid value
	gid_bak = AEGid;
	
	// snap
	for (AEGid = 0; AEGid < 101; AEGid++)
	{
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
		sp1kSnapTask();

		//appCalRGBvalueShow(calibrationObData);
		//printf("AEGid / R / Gr / B / Gb = %d / %d / %d / %d / %d\n",(int)AEGid,(int)calibrationObData[0],(int)calibrationObData[1],(int)calibrationObData[2],(int)calibrationObData[3]);
		
		sp1kCalibrationSaveIq( SP1K_CALIBRATION_OB_FILE_ID );
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
		if(sp1kBtnChk(SP1K_BTN_S2) == KEY_STATUS_PRESSED){
			break;
		}
	}

	//restore configure
	sp1kCalibrationObSet(puiPara->cal_ob);
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 1 ); //re-open store
	AEGid = gid_bak;	
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalPvOB(UINT16 resId , UINT16 resOff)
{
	resId = resId;
	resOff = resOff;
#if OB_CAL_EN
	UINT16 gainIndex;
	UINT32 shutter = 333;
	UINT8 currLuma,i,j,result[8];
	UINT8 tagLuma[9] = {90,80,70,60,50,40,30,20,10};/*Tag Luma*/
	UINT16 expIntv[8];
	float expRatio;
	
	UINT16 exp,lineTol,expStar,pClkL,pClkH,calObOnePoint;
	UINT32 pixClk;
	calPvOBData_t *obData;

	UINT32 strTime,endTime;

	
	
	HAL_GlobalReadGTimer(&strTime);
	obData = sp1kCalPvOBAvgDataGet();
//	sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_GAIN_INDEX , 0);
//	sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_INIT_EXP , 90 /*372/*152/*338/*188*/);
//	sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_SAVE_RAW , 1);
	
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_PV_OB ); 
	sp1kCalPvOBParaGet(SP1K_CAL_PV_OB_GAIN_INDEX , &gainIndex , NULL);
	sp1kCalPvOBParaGet(SP1K_CAL_PV_OB_INIT_EXP , &exp , NULL);
	sp1kCalPvOBParaGet(SP1K_CAL_PV_OB_LINE_TOL , &lineTol , NULL);
	sp1kCalPvOBParaGet(SP1K_CAL_PV_OB_PCLK , &pClkL , &pClkH);
	sp1kCalPvOBParaGet(SP1K_CAL_PV_OB_ONE_POINT_CAL , &calObOnePoint , NULL);
	
	pixClk = (((UINT32)pClkH)<<16) + (UINT32)pClkL; 

/***********GET RAW INFO S***********/
	
	dbgWdtSet(0);

#if (USE_RGB_WIN == 1)
	sp1kCalPvOBAEWinConfigSet(0);
#endif
	if(calObOnePoint == 0){
		for(i = 0 ; i < 9 ; i++){
			
		#if (USE_RGB_WIN == 0)
			sp1kCalPvOBAEWinConfigSet(0);
		#endif
			
			if(i == 0){ 		/**First **/	
				if(exp != 0){
					shutter = ((UINT32)(pixClk *10) /((UINT32)lineTol * exp));	
				}
				else{
					exp = 100;
				}
				sp1kCalPvOBGainShutterSet(gainIndex ,shutter,4);
				currLuma = aeFlowGetCurLuma();		
			//	printf("currLuma00 = %bu\n",currLuma);
				if((ABS(currLuma,tagLuma[i])) > 2){    /*jump to the target exp line*/
					exp = (UINT32)exp*(UINT32)90/currLuma;
				//	printf("exp = %u\n",exp);
				}
				shutter = ((UINT32)(pixClk *10) /((UINT32)lineTol * exp));	
				sp1kCalPvOBGainShutterSet(gainIndex ,shutter,4);
				currLuma = aeFlowGetCurLuma();	
			//	printf("currLuma00 = %bu\n",currLuma);
			#if 0
				while((ABS(currLuma,tagLuma[i])) > 2){
					if(currLuma > tagLuma[i]){
						if(exp != 0){			
							exp = exp -2;
						}
					}
					else{
						exp = exp + 2;
					}
					if(exp != 0){
						shutter = ((UINT32)(pixClk *10) /((UINT32)lineTol * exp));	
					}
					sp1kCalPvOBGainShutterSet(gainIndex ,shutter,2);	
					currLuma = aeFlowGetCurLuma();
					printf(".");
				}
			#endif
			}				
			else{		/*Sec To Ni*/
				exp = expStar - expIntv[i - 1]; 	
				if(exp != 0){
					shutter = ((UINT32)(pixClk *10) /((UINT32)lineTol * exp));	
				}
			#if (USE_RGB_WIN == 0)
				sp1kCalPvOBGainShutterSet(gainIndex ,shutter,0);	
			#else
				sp1kCalPvOBGainShutterSet(gainIndex ,shutter,4);	
			#endif
				currLuma = aeFlowGetCurLuma();		
			//	printf("currLuma%bu = %bu\n",i,currLuma);
			//	printf("*");
			}
		//	printf("\n");
			exp = (UINT16)((UINT32)(pixClk *10) /((UINT32)lineTol * shutter)); 
			obData->expLine = exp;
			obData++;		
			if(i == 0){
				expStar = exp; 
			//	printf("expStar = %u\n",expStar);
				expRatio = (float)(exp -2)/(float)(8*exp);	
				//printf("expRatio = %f\n",expRatio);
				for(j = 0 ; j < 8 ; j ++){
					expIntv[j] = (float)exp *expRatio *(float)(j+1);
					//printf("expIntv[%bu] = %u\n",j,expIntv[j]);
				}		
			}
		#if (USE_RGB_WIN == 0)
				appCalPvRaw();	 /*Read RAW*/
		#else
				sp1kCalPvOBAEWinRGBValueGet(i); /*Read AE RGB window*/		
				//appCalPvRaw();   /*Read RAW*/
		#endif
		}
	}
	else{   /* Cal OB once*/
		#if (USE_RGB_WIN == 0)
			sp1kCalPvOBAEWinConfigSet(0);
		#endif
		if(exp != 0){
			shutter = ((UINT32)(pixClk *10) /((UINT32)lineTol * exp));	
		}
		else{
			exp = 100;
		}
		sp1kCalPvOBGainShutterSet(gainIndex ,shutter,4);
		#if (USE_RGB_WIN == 0)
			appCalPvRaw();	 /*Read RAW*/
		#else
			sp1kCalPvOBAEWinRGBValueGet(0); /*Read AE RGB window*/		
			//appCalPvRaw();   /*Read RAW*/
		#endif
	}
	
	/*Cal OB & save result*/
#if (USE_RGB_WIN == 0)
	if(sp1kCalPvOBResultGet(result , 0) == 1)  /* 0 : for 8 bits mode*/
#else
	if(sp1kCalPvOBResultGet(result , 1) == 1)	/* 1 : for 10 bits mode (RGB Window)*/
#endif
	{
		/*for(j = 0 ; j < 9 ; j++){
			calPvOBData_t *obDataExp = sp1kCalPvOBAvgDataGet();
			printf("expLine%bu = %u\n",j,obDataExp[j].expLine);
		}*/ 
		for(j = 0 ; j < 4 ; j++){
			//printf("\033[32m%#bx%bx\033[0m\n",result[j*2],result[j*2+1]);
		}			
		sp1kCalPvOBResultWrite(result , resId , resOff); /*Save to bin*/
	}	
/**********GET RAW INFO E***********/ 
	sp1kCalPvOBAEWinConfigSet(0);	
	sp1kCalPvOBGainShutterSet(gainIndex ,shutter,0);	
	
	HAL_GlobalReadGTimer(&endTime);
//	printf("Cal PV OB time = %ld\n",(endTime-strTime));

	dbgWdtSet(1);
	sp1kCalPvOBAEWinConfigSet(1);	
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF ); 

#else
	// snap
	//appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
	//sp1kSnapTask();
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_PV_OB ); 
	sp1kCalibrationPvRaw(); 
	
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF ); 
	appCalRGBvalueShow(calibrationPvObData);
#endif
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalAwb(void)
{
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_AWB );
	// snap
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
	sp1kSnapTask();
	appCalRGBvalueShow(calibrationAwbData);
	
	//sp1kCalibrationSaveIq( SP1K_CALIBRATION_AWB_FILE_ID );
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalAwbCalibration(UINT8 item)
{
	UINT16 cnt=0xffff; 
	UINT8 isCtt;
	//dbgWdtSet(0); 
	isCtt=!item;
	sp1kAwbCalibStart(); 
	dbgWdtSet(0);
	if(isCtt){ 
		//please switch A light 
		sp1kOsdClear(0);
		//sp1kOsdStrDraw(0,0,"Please switch A light,",2);
		//sp1kOsdStrDraw(0,1,"and press Snap key",2);
		sp1kHalWait(1000);
		//while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
		sp1kAwbCalibDo(0); 
		//please switch CW light 
		sp1kOsdClear(0);
		//sp1kOsdStrDraw(0,0,"please switch CW light",2);
		//sp1kOsdStrDraw(0,1,"and press Snap key",2);
		sp1kHalWait(1000);
		//while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
		sp1kAwbCalibDo(1); 
		//please switch D Light 
		sp1kOsdClear(0);
		//sp1kOsdStrDraw(0,0,"please switch D light",2);
		//sp1kOsdStrDraw(0,1,"and press Snap key",2);
		sp1kHalWait(1000);
		//while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
		sp1kAwbCalibDo(2); 
		//please switch Standand Light
		sp1kOsdClear(0);
		//sp1kOsdStrDraw(0,0,"Please switch Stand light,",2);
		//sp1kOsdStrDraw(0,1,"and press Snap key",2);
		sp1kHalWait(1000);
		//while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
		sp1kAwbCalibDo(3); 
		sp1kAwbCalibCtt(1); 
		//dbgWdtSet(1); 
	}else{ 
		//please switch A-Lv10 
		sp1kOsdClear(0);
		//sp1kOsdStrDraw(0,0,"please switch A-Lv10",2);
		//sp1kOsdStrDraw(0,1,"and press Snap key",2);
		//while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
		sp1kAwbCalibDo(3); 
		sp1kAwbCalibCtt(1); 
	}
	appPowerOffProc();
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalPvRaw(void)
{
	UINT16 frontHSize,frontVSize;
	
	/* preview capture raw */
	sp1kCalibrationPvRaw();

	/* preview setting */
	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewCropSizeSet(frontHSize,frontVSize);
	
	sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);	
		
	sp1kCalibrationLscSet(0);    //fan.xiao modify for pv ob cal
	sp1kHalCdspImgSrcSel(0);	// 0: image source from front
	
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalPvPureRaw(void)
{
	UINT16 frontHSize,frontVSize;
	
	/*OB LSC AWB Gamma turning off*/
	sp1kCalibrationObSet(0);
	sp1kCalibrationLscSet(0);
	sp1kCalibrationAWBSet(0);
	sp1kCalibrationGammaSet(0);
	HAL_CdspBPEnSet(0);

	/* preview capture raw */
	sp1kCalibrationPvRaw();

	/* preview setting */
	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewCropSizeSet(frontHSize,frontVSize);
	sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);							
	sp1kHalCdspImgSrcSel(0);	// 0: image source from front

	/*OB LSC AWB Gamma turning on*/
	sp1kCalibrationObSet(1);
	sp1kCalibrationLscSet(1);
	sp1kCalibrationAWBSet(1);
	sp1kCalibrationGammaSet(1);
	HAL_CdspBPEnSet(1);
	sp1kAeSetModeEn(1);
	sp1kAwbEnable();
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalPvYuv(void)
{
	sp1kCalibrationPvYuv();
}


/**************************************************************************
 *                                                                        *
 *  Function Name:   appCalBpPvModeSet                                                     *
 *                                                                        *
 *  Purposes:  to set the badpixel table address for CDSP when sensor output different size *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: pv_mode  :   PV_MODE_VGA , low size                                                    *
 *                                       PV_MODE_HD , high size                              *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/ 
 void appCalBpPvModeSet(UINT8 pv_mode)
 {
 #if FUNC_HOST_BADPIXEL
     if(HAL_CdspBPEnGet() == 0){
		 if(pv_mode == PV_MODE_VGA) {
	 
			 HAL_CdspPvBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr);
			 HAL_CdspPvBpEnSet(1);
	 
		 }
		 else if (pv_mode == PV_MODE_HD) {
			 
			 HAL_CdspPvBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize_Upper + K_SDRAM_BadPixel_CapBufSize_Lower);
			 HAL_CdspPvBpEnSet(1);
	 
		 }
     }
#else
	pv_mode = pv_mode;
	return ;
#endif 
 }

//#define CalBpSearchPrint printf
#ifndef CalBpSearchPrint
	#define CalBpSearchPrint /_printf
#endif
 /**************************************************************************
  * 																	   *
  *  Function Name: 													   *
  * 																	   *
  *  Purposes:															   *
  * 																	   *
  *  Descriptions:														   *
  * 																	   *
  *  Arguments: 														   *
  * 																	   *
  *  Returns: None														   *
  * 																	   *
  *  See also:															   *
  * 																	   *
  **************************************************************************/
 UINT8 appCalBpSearch(UINT8 item)
{
#if FUNC_HOST_BADPIXEL

	UINT16 frontHSize,frontVSize;
	if ( appCalStorageChk() == FAIL) {
		return FAIL;
	}
	sp1kAeSetModeEn(0);
	sp1kAwbDisable();
	HAL_GprmDramFill(K_SDRAM_BadPixelCoordBufAddr, K_SDRAM_BadPixelCoordBufSize << 1, 0xff);
	
	if ( sp1kBpResCfg(RES_BP_PRV_VGA, 0x05) == SUCCESS ) 
	{
		//sp1kBpResLoad(RES_BP_PRV_VGA, K_SDRAM_BadPixelCoordBufAddr);
	}

	#if 0	/* work around MOE real time bad pixel compensation */
		if ( sp1kBpResCfg(RES_BP_CAP, 0x19) == SUCCESS ) {
			sp1kBpResLoad(RES_BP_CAP, K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize);
		}	

		if ( sp1kBpResCfg(RES_BP_CAP, 0x1a) == SUCCESS ) {
			sp1kBpResLoad(RES_BP_CAP, K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize_Upper);
		}	
		
	#else   /* debug for work around MOE real time bad pixel compensation */
		if ( sp1kBpResCfg(RES_BP_CAP, 0x19) == SUCCESS ) {
			//sp1kBpResLoad(RES_BP_CAP, K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize);
		}
	#endif
	
	
	if ( sp1kBpResCfg(RES_BP_PRV_HD, 0xd6) == SUCCESS ) 
	{
		//sp1kBpResLoad(RES_BP_PRV_HD, K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize_Upper + K_SDRAM_BadPixel_CapBufSize_Lower);
	}
	
	//sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	//sp1kPreviewCropSizeSet(frontHSize,frontVSize);
	//sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);							
	HAL_CdspPvBpEnSet(0);
	HAL_CdspStillBpEnSet(0);
	dbgWdtSet(0);
	
	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);	
	sp1kOsdStrDraw(0,0,"Pls Capture a white image",2);
	while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
	
	sp1kHalCtrlScreen(2);
	/*Preview HD bad pixel */
	if((item !=1) && (item != 2)) {
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
		sp1kPreviewCropSizeSet(frontHSize,frontVSize);
		sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);
		CalBpSearchPrint("H1=%u,V1=%u\n",frontHSize,frontVSize);
		HAL_Cdsp2ndPathCfg(1,0,0);//off 2nd path
		/*add front init*/
		if((sBpPara.prvHeight_HD == frontVSize)&&(sBpPara.prvWidth_HD== frontHSize)) {			
			appCalBpSnapRaw(BP_MODE_PRV_HD,BP_IMG_WHITE,0x01,0x4A6);
		}
	}	
	HAL_Cdsp2ndPathCfg(0,0,0);//off 2nd path
	/*Preview bad pixel */
	if((item != 0) && (item != 2)) {		
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_20);
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
		sp1kPreviewCropSizeSet(frontHSize,frontVSize);
		sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);
		CalBpSearchPrint("H2=%u,V2=%u\n",frontHSize,frontVSize);		
		HAL_Cdsp2ndPathCfg(1,0,0);//off 2nd path
		/*add front init*/
		if((sBpPara.prvHeight == frontVSize)&&(sBpPara.prvWidth== frontHSize)) {
			appCalBpSnapRaw(BP_MODE_PRV_VGA,BP_IMG_WHITE,0x01,0x4A6);
		}
	}
	/*Whole image bad pixel */
	if((item != 0) && (item != 1)) {
		appCalBpSnapRaw(BP_MODE_CAP,BP_IMG_WHITE,0x01,0x4A6);
	}
	//sp1kDispImgWinEn(1);
	HAL_Cdsp2ndPathCfg(0,0,0);//off 2nd path
	sp1kOsdClear(0);
	sp1kOsdStrDraw(0,0,"Pls Capture a black image",2);
	while (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED);
	
	/*Preview HD bad pixel */
	if((item !=1) && (item != 2)) {
		/*add front init*/
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);			
		sp1kPreviewCropSizeSet(frontHSize,frontVSize);
		sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);
		CalBpSearchPrint("BH1=%u,BV1=%u\n",frontHSize,frontVSize);
		HAL_Cdsp2ndPathCfg(1,0,0);//off 2nd path
		if((sBpPara.prvHeight_HD == frontVSize)&&(sBpPara.prvWidth_HD== frontHSize)) {	
			appCalBpSnapRaw(BP_MODE_PRV_HD,BP_IMG_BLACK,0x2F,0x12B);
		}
	}	
	HAL_Cdsp2ndPathCfg(0,0,0);//off 2nd path
	/*Preview bad pixel */
	if((item != 0) && (item != 2)) {
		/*add front init*/
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_20);
		sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);			
		sp1kPreviewCropSizeSet(frontHSize,frontVSize);
		sp1kPreviewSet(0, 320, 240, 100/*PvZoomCtrl.factor*/, 0);
		CalBpSearchPrint("BH2=%u,BV2=%u\n",frontHSize,frontVSize);
		HAL_Cdsp2ndPathCfg(1,0,0);//off 2nd path

		if((sBpPara.prvHeight == frontVSize)&&(sBpPara.prvWidth== frontHSize)) {
			appCalBpSnapRaw(BP_MODE_PRV_VGA,BP_IMG_BLACK,0x2F,0x12B);
		}
	}
	/*Whole image bad pixel */
	if((item != 0) && (item != 1)) {
		appCalBpSnapRaw(BP_MODE_CAP,BP_IMG_BLACK,0x2F,0x12B);
	}
	if(appCalBpPro() ==0)CalBpSearchPrint("Bp FAIL\n");
	if(appCalBpCapDataArrange() !=0)CalBpSearchPrint("translation fail!\n");
	//sp1kDispImgWinEn(1);
	appPowerOffProc();
#else
	item = item;
	return SUCCESS;
#endif
}


#if FUNC_HOST_BADPIXEL
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 static UINT8 appCalBpSnapRaw(UINT8 mode,UINT8 type, UINT8 gain, UINT32 shutter)
{
	appCalShowWait();
	sp1kBpCfg(&sBpPara);
	sp1kBpParaSet(type,shutter,gain);//gain 0x30
	sp1kCalBpProc(mode);

	return  SUCCESS;

}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
static UINT8 appCalBpPro(void)
{
	UINT8 /*bad_idx0= 0x45,*/bad_idx1= 0x06;
	UINT32 startAddr = K_SDRAM_CommonEndAddr+(UINT32)1024*64;
	UINT32 idxSize=(UINT32)BADPIXEL_CAP_NUM_MAX*4;//32768;
	UINT32 badAddr0= startAddr,badAddr1= startAddr + idxSize/2,dstAddr=  startAddr + idxSize;
	UINT8 sts=0;
	static UINT16 x0,y0,x1,y1;
	UINT32 cnt=0;
	const UINT8 bInitVal = 0xfe;
	UINT32 fileSize0,fileSize1;
	
	/* buffer init */
	HAL_GprmDramFill(badAddr0, idxSize << 1, bInitVal);
	HAL_GprmDramFill(badAddr1, idxSize << 1, bInitVal);
	HAL_GprmDramFill(dstAddr, idxSize << 1, bInitVal);
	
	//sts = sp1kNandRsvRead(bad_idx0,badAddr0<<1);
	sts=sp1kDramFileLoad("BPCAP000BIN", badAddr0, &fileSize0);
	//sp1kDramFileSave("SRC0000BBIN", badAddr0, idxSize);
	if(sts){
		//sts=sp1kNandRsvRead(bad_idx1,badAddr1<<1);
		sts=sp1kDramFileLoad("BPCAP001BIN", badAddr1, &fileSize1);
		//sp1kDramFileSave("SRC0000WBIN", badAddr1, idxSize);
		if(sts ==0){
			return sts;
		}
	}
	else{
		return sts;
	}

	start:
	while(badAddr0< (startAddr+ idxSize/2)){
		appDramReadDWord(badAddr0,&x0,&y0);
		if(x0 == 0xffff && y0==0xffff){
			while(1){
				appDramReadDWord(badAddr1,&x1,&y1);
				appDramWriteDWord(dstAddr, x1, y1);
				badAddr1+=2;
				dstAddr+=2;
				if(badAddr1 >= startAddr + idxSize)goto save;
			}
			
		}
		while(badAddr1 < startAddr + idxSize){
			appDramReadDWord(badAddr1,&x1,&y1);
			if(x1 == 0xffff && y1==0xffff){
				while(1){
					appDramReadDWord(badAddr0,&x0,&y0);				
					appDramWriteDWord(dstAddr, x0, y0);
					badAddr0+=2;
					dstAddr+=2;
					if(badAddr0 == startAddr + idxSize/2)goto save;
				}
			}
			if((y0 & 0x3fff)<(y1 & 0x3fff)){
				appDramWriteDWord(dstAddr, x0, y0);
				badAddr0+=2;
				dstAddr+=2;
				cnt+=4;
				if(cnt == idxSize-4){
					appDramWriteDWord(dstAddr, 0xffff, 0xffff);
					goto save;
				}
				goto start;
			}else if((y0 & 0x3fff)==(y1 & 0x3fff)){
				if(x0<x1){
					appDramWriteDWord(dstAddr, x0, y0);
					badAddr0+=2;
					dstAddr+=2;
					cnt+=4;
					if(cnt == idxSize-4){
						appDramWriteDWord(dstAddr, 0xffff, 0xffff);
						goto save;
					}
					goto start;
				}else if(x0==x1){
					appDramWriteDWord(dstAddr, x0, y0);
					badAddr0+=2;
					badAddr1+=2;
					dstAddr+=2;
					cnt+=4;
					if(cnt == idxSize-4){
						appDramWriteDWord(dstAddr, 0xffff, 0xffff);
						goto save;
					}
					goto start;
				}else /*x0>x1*/{
					appDramWriteDWord(dstAddr, x1, y1);
					badAddr1+=2;
					dstAddr+=2;
					cnt+=4;
					if(cnt == idxSize-4){
						appDramWriteDWord(dstAddr, 0xffff, 0xffff);
						goto save;
					}
				}
				
			}else if((y0 & 0x3fff) > (y1 & 0x3fff)){ 
				appDramWriteDWord(dstAddr, x1, y1);
				badAddr1+=2;
				dstAddr+=2;
				cnt+=4;
				if(cnt == idxSize-4){
					appDramWriteDWord(dstAddr, 0xffff, 0xffff);
					goto save;
				}
			}	
			
		}
	}
save:
	sp1kDramFileSave("SSS00006BIN", (startAddr+ idxSize), idxSize);
//	sp1kNandRsvWrite(0x06,(startAddr+ idxSize)<<1);
return sts;	
	
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
static UINT8 appCalBpCapDataArrange(void)
{
	UINT32 startAddr = K_SDRAM_CommonEndAddr+(UINT32)1024*64;
	UINT32 idxSize=(UINT32)BADPIXEL_CAP_NUM_MAX*4;//32768;//32K
	UINT32 SrcSize;
	UINT32 badSrc= startAddr,badDst= startAddr + idxSize/2;
	UINT16 xVal,yVal,capV_tmp;
	UINT32 startPoint=badSrc;

	#if BP_USE_MOE
	#else
	UINT32 badDstup;
	UINT16 yValtmp;
	UINT16 icount;
	#endif
	
	#if BP_USE_MOE
	UINT32 badDstDown,badDstUp,badDstDownSv,badDstUpSv;
	UINT16 xValTmp,yValTmp,badCnt;
	
	badDstUp =  badSrc +  (idxSize>>1);  /*32K*/
	badDstDown =  badDstUp +  (idxSize>>1);	/*32K*/
	badDstUpSv = badDstUp;
	badDstDownSv = badDstDown;
	HAL_GprmDramFill(badSrc, (idxSize << 2), 0xfe); /*32K x 4*/
	#endif
	
	sp1kDramFileLoad("SSS00006BIN", badSrc, &SrcSize);
	if(SrcSize>idxSize)
	{
		//printf("size too large!\n");
		return FAIL;
	}
	
	#if BP_USE_MOE
	badCnt= 0;
	capV_tmp = (sBpPara.capHeight >> 1) -1;
	
	appDramReadDWord(badSrc,&xVal,&yVal);
	while(xVal != 0xffff && yVal !=0xffff){
		badSrc += 2;
		badCnt ++;
		yValTmp = yVal & 0x0fff;    /*High 4Bits is the direct*/
		if( yValTmp > capV_tmp) {/*down*/
			yValTmp = (yVal << 8) | (yVal >> 8); /*switch low byte  and hight byte*/
			xValTmp = (xVal << 8) | (xVal >> 8);
			appDramWriteDWord(badDstDown,xValTmp,yValTmp);
			badDstDown += 2;
		}
		else{/*Up*/
			yValTmp = (yVal << 8) | (yVal >> 8); /*switch low byte  and hight byte*/
			xValTmp = (xVal << 8) | (xVal >> 8);
			appDramWriteDWord(badDstUp,xValTmp,yValTmp);
			badDstUp += 2;
		}
		appDramReadDWord(badSrc,&xVal,&yVal);
	}

	if(badDstDown >= (badDstDownSv + (idxSize>>2))){
		badDstDown = (badDstDownSv + (idxSize>>2)) - 2;
	}
	if(badDstUp >= (badDstUpSv + (idxSize>>2))){
		badDstUp = (badDstUpSv + (idxSize>>2)) - 2;
	}
	appDramWriteDWord(badDstDown,0xffff,0xffff);
	appDramWriteDWord(badDstUp,0xffff,0xffff);
	sp1kDramFileSave("TRANS6UPBIN", badDstUpSv, SrcSize>>1);
	sp1kNandRsvWrite(0x19,badDstUpSv<<1);
	sp1kDramFileSave("TRANS6DNBIN", badDstDownSv, SrcSize>>1);
	sp1kNandRsvWrite(0x1a,badDstDownSv<<1);
	//printf("badCnt = %u\n",badCnt);
	return SUCCESS;
#else
	for(icount=0;icount < sBpPara.capHeight;icount++)
	{
		//printf("*");
		badSrc = startPoint;
		while(1)
		{
			appDramReadDWord(badSrc,&xVal,&yVal);
			if((yVal & 0x0FFF)>(icount & 0xFFF0)+15)// =>(icount/16)*16+15
			{	
				if(icount == ((icount & 0xFFF0)+15))
				{
					startPoint = badSrc;
				}
				break;
			}
			if((yVal & 0x0FFF) == icount)// yval[15:14]: direct
			{
				xVal = (xVal << 8) | (xVal >> 8);//switch low byte  and hight byte
				yVal = (yVal << 8) | (yVal >> 8); 
				appDramWriteDWord(badDst,xVal,yVal);
				badDst=badDst+2;
			}
			badSrc=badSrc+2;
		}

	}
	appDramWriteDWord(badDst,0xFFFF,0xFFFF);
	badDst= startAddr + idxSize/2;
	sp1kDramFileSave("TRANS006BIN", badDst, SrcSize);
	/*for work around MOE real time bad pixel compensation kai ++*/
	capV_tmp = (sBpPara.capHeight >> 1) -1;
	//printf("capV_tmp=%u,hight=%u\n",capV_tmp,sBpPara.capHeight);
	badDstup = badDst;
	do{
		appDramReadDWord(badDst,&xVal,&yVal);		
		//xVal = (xVal << 8) | (xVal >> 8);//switch low byte  and hight byte
		yValtmp =(yVal << 8) | (yVal >> 8); 
		yValtmp = yValtmp & 0x0fff;
		//printf("yValtmp=%u\n",yValtmp);
		if( yValtmp > capV_tmp) {
			appDramWriteDWord(badDst,0xFFFF,0xFFFF);
			sp1kDramFileSave("TRANS6UPBIN", badDstup, SrcSize>>1);
			sp1kNandRsvWrite(0x19,badDstup<<1);
			break;
		}
			
		badDst += 2;
	}while ((xVal!=0xffff) && (yVal!=0xffff));
	
	appDramWriteDWord(badDst,xVal,yVal);
	sp1kDramFileSave("TRANS6DNBIN", badDst, SrcSize>>1);
	sp1kNandRsvWrite(0x1a,badDst<<1);
	/*for work around MOE real time bad pixel compensation kai --*/
	return SUCCESS;
#endif
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
static void appCalCfgDataRst(void)
{
	uiPara_t* puiPara;

	puiPara = appUiParaGet();
	puiPara->cal_ae = 1;
	puiPara->cal_awb = 1;
	puiPara->cal_gma = 1;
	puiPara->cal_ob = 1;
	puiPara->cal_lc = 1;

	/*OB LSC AWB Gamma turning off*/
	sp1kCalibrationObSet(1);
	sp1kCalibrationLscSet(1);	
	sp1kCalibrationAESet(1);
	sp1kCalibrationAWBSet(1);
	sp1kCalibrationGammaSet(1);
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
static void appCalRGBvalueShow (UINT8 * value)
{

	UINT8 str[10];

	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
	sp1kOsdStrDraw(0,0,"Up/Down : Exit",(1<<4)+8);
	sprintf(str, "R : %d", (int)value[0]);
	sp1kOsdStrDraw(0,2,str,(1<<4)+8);
	sprintf(str, "Gr : %d", (int)value[1]);
	sp1kOsdStrDraw(0,3,str,(1<<4)+8);
	sprintf(str, "B : %d", (int)value[2]);
	sp1kOsdStrDraw(0,4,str,(1<<4)+8);
	sprintf(str, "Gb : %d", (int)value[3]);
	sp1kOsdStrDraw(0,5,str,(1<<4)+8);

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 void appCalShowWait(void)
{
	UINT8 OsdX,OsdY,str[] = "waiting...";
	
	sp1kOsdLayoutAttrGet(&OsdX, &OsdY);
	
	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
	sp1kOsdStrDraw((OsdX - strlen(str)) >> 1, OsdY >> 1, str, (1<<4)+8);
	//sp1kOsdStrDraw(0,0,"waiting...",(1<<4)+8);
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
static UINT8 appCalStorageChk(void)
{
	devAttr_t* pDevAttr;
	UINT8 devSts = DEV_STS_UNKNOWN;

	pDevAttr = sp1kDevHdlGet();
	
	/* check the active storage device */
	if ( appSDPlugStatusGet() == 1 ) {
		pDevAttr += DEV_ID_CARD;
	} else {
		pDevAttr += DEV_ID_DISK;
	}

	/* check device status */
	switch ( pDevAttr->devSts ) {
		case DEV_STS_RDY:
			devSts = DEV_STS_RDY;
			break;
		case DEV_STS_ERR:
			devSts = DEV_STS_ERR;
			break;
		case DEV_STS_UNKNOWN:
		case DEV_STS_BUSY:
		case DEV_STS_CONNECTED:
		case DEV_STS_DISCONNECTED:
		default:
			devSts = DEV_STS_UNKNOWN;
			break;
	}

	/* if the storage is not in ready */
	if ( devSts != DEV_STS_RDY ) {
		/* if there's card insert */
		if ( appSDPlugStatusGet() == 1 ) {
//			printf("Card err\n");
			uiOsdDialogDisp(ID_STR_CARD_ERROR,1000);
		} else {
			/* the internal memory is active */
			if ( devSts == DEV_STS_ERR ) {
//				printf("Disk err\n");
				uiOsdDialogDisp(ID_STR_CARD_ERROR,1000);
			} else {
				/* the internal memory status is unknown */
//				printf("Pls insert card\n");
				uiOsdDialogDisp(ID_STR_NO_CARD_,1000);
			}
		}
	}
	
	if ( pDevAttr->devSts != DEV_STS_RDY ) {
		return FAIL;			
	} else {
		return SUCCESS;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appCalAe(void)
{
	UINT32 cnt=0;
	UINT8 sts;
	
	//appStillViewInit(SP1K_STILL_VIEW_INIT_FULL);
	sp1kOsdClear(0);
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);
	sp1kBtnDisableAll();

	do{
		sts = appStillViewAEAWBWait(); 
		cnt ++;
	}while(sts != SUCCESS && cnt< 5000);

	sp1kBtnEnableAll();
	if(Sp1kAeCalibration()){
		sp1kOsdStrDraw(3,2,"Ae calibration error",2);
		sp1kHalWait(1000);
	}		
	
	return;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if SUPPORT_MCU_WTD
void appMCUWDTEnterCardISPMode(void) {
	UINT8 filestr[20] ="D:\\1628.BIN";	
	UINT8 fd = DOS_FileOpen(filestr, 'r');
	
	if (fd) {
		#if SUPPORT_MCU_WTD
		dbgTimerStop();


		appSetWTDHeartbeat(0);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(1);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(0);
		sp1kHalWait(58);

		appSetWTDHeartbeat(1);
		sp1kHalWait(58);

		appSetWTDHeartbeat(0);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(1);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(0);
		sp1kHalWait(58);

		appSetWTDHeartbeat(1);
		sp1kHalWait(58);

		appSetWTDHeartbeat(0);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(1);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(0);
		sp1kHalWait(58);

		appSetWTDHeartbeat(1);
		sp1kHalWait(58);

		appSetWTDHeartbeat(0);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(1);
		sp1kHalWait(58);
		
		appSetWTDHeartbeat(0);
		sp1kHalWait(58);

		appSetWTDHeartbeat(1);
		sp1kHalWait(58);

		
		#endif
	}
}
#endif
 void appCardUpdateFW(void)
{
	UINT8 strwidth = 0, strheight = 0;
	UINT8 xpos, ypos, str[30] = "ISP";
	
	code UINT8  filestr[20] = "D:\\1628.BIN",file1str[30] = "D:\\DCIM\\100MEDIA\\";
	UINT8 fd = DOS_FileOpen(filestr, 'r');

	dbgWdtSet(0);
		
	if (!fd) {
		#if SUPPORT_AUTO_TEST
		UINT8 en;
		#endif
		
		
		#if SUPPORT_AUTO_TEST
		ENTER_CRITICAL(en);
		fd = DOS_FileOpen(SUPPORT_AUTO_TEST_FILENAME, 'r');
		if (fd) {
			autoTestOpenFlag = 1;
		}
		DOS_FileClose(fd);
		EXIT_CRITICAL(en);
	#endif
		
		
		DIR_Change(file1str);
		
		return;
	}
	DOS_FileClose(fd);

	
	app_set_led(1);
	appFlashLedSet(1);

	app_set_lcd_backlight_status(1);

	#if SUPPORT_SPI_WRITE_CONTROL
	mastWriteSpiFlag = 1;
 	appSpiWpSet(0);
	sp1kHalWait(300);
	#endif

	sp1kOsdIndexSave();
	sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK, 4);
	sp1kOsdRectDraw(0, 0, 320 , 240);
	sp1kOsdIndexRestore();
	
	strGet(str, sizeof(str), ID_STR_SOFTWARE_UPDATE___);
	xpos = (320 - strWidthGet(str)) >> 1;
	ypos = (240 - 32) >> 1;
	sp1kOsdStrDrawExt(xpos, ypos, str, 1);
	strGet(str, sizeof(str), ID_STR_WAITING___);
	sp1kOsdStrDrawExt(xpos, ypos+32, str, 1);

	sp1kHalWait(100);
	if(sp1kCardFirmwareUpdate(filestr , 0)==FAIL)
	{
		sp1kOsdIndexSave();
		sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK, 4);
		sp1kOsdRectDraw(0, 0, 320 , 240);
		sp1kOsdIndexRestore();

		strGet(str, sizeof(str), ID_STR_UPDATE_FAILED_);
		xpos = (320 - strWidthGet(str)) >> 1;
		ypos = (240 - 32) >> 1;
		sp1kOsdStrDrawExt(xpos, ypos, str, 1);
		
		app_set_led(0);
		sp1kHalWait(200);
		app_set_led(1);
		sp1kHalWait(200);
		app_set_led(0);
	}
	else
	{	
		sp1kOsdIndexSave();
		sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK, 4);
		sp1kOsdRectDraw(0, 0, 320 , 240);
		sp1kOsdIndexRestore();

		strGet(str, sizeof(str), ID_STR_UPDATE_COMPLETED_);
		xpos = (320 - strWidthGet(str)) >> 1;
		ypos = (240- 32) >> 1;
		sp1kOsdStrDrawExt(xpos, ypos, str, 1);
		
		appPowerShutDown();
	}
	
	#if SUPPORT_SPI_WRITE_CONTROL
	mastWriteSpiFlag = 0;
	#endif
	

}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appDramReadDWord(UINT32 addr ,UINT16* val_x,UINT16* val_y)
{
	UINT8 tmp0,tmp1;
	
	HAL_DramStartAddrSet(addr, 1);
	HAL_DramWordDataRead(&tmp0, &tmp1);
	*val_x=(UINT16)tmp1 | ((UINT16)tmp0<<8);
	HAL_DramWordDataRead(&tmp0, &tmp1);
	*val_y =(UINT16) tmp1 | ((UINT16)tmp0<<8);
	//printf("X:%x,Y:%x\n",*val_x,*val_y );
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void appDramWriteDWord(UINT32 addr,UINT16 a, UINT16 b)
{
	BP_DRAM_SetStartAddress(addr, 0);
	BP_DRAM_WriteWord( (a>>8)&0xff,a&0xff );
	BP_DRAM_WriteWord( (b>>8)&0xff,b&0xff );
}


void appNumshow(UINT16 x, UINT16 y, UINT32 value)
{
	UINT8 str[128]={0};
	sprintf(str, "%ld", value);
	sp1kOsdClearCharExt(x+4,y+4,70,32);
	sp1kOsdStrDrawExt(x+4, y+4, str, 0);
}

UINT32 appAfWinowValueGet(void)
{
	UINT32 AFWinvalue;
	#if (FUNC_HOST_AF_FOCUS_ONE_WIN  == 0)
	UINT32 timeout = 10000;
	XBYTE[0x21ee]|=0x10;
	while ((XBYTE[0x21ee]&0x10)==0x00) 
	{
		timeout--;
		if(timeout < 10)
		{
			return 0;
		}
	}   //will: wait for AF Done	
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	sp1kHALReadAFWindow(&AFWinvalue);
	return AFWinvalue;
	#else
	return AFWinvalue;
	#endif
}

void appCalAfWindowCfg(UINT8 value)
{
	#if (FUNC_HOST_AF_FOCUS_ONE_WIN  == 0)
	UINT16 frontHSize,frontVSize;
	UINT16 hStart,hEnd,vStart,vEnd;
	UINT16 mWindowH = 300;//main window
	UINT16 mWindowV = 300;//main window
	UINT16 WindowH = 200;//
	UINT16 WindowV = 200;//
	UINT16 xOffset = 30;
	UINT16 yOffset = 30;
	UINT8 afFlt2Type = 1;
	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	//printf("f_hsize=%d,vsize=%d\n",frontHSize,frontVSize);
	switch(value)
	{
		case 0:
			hStart = xOffset;
			hEnd = hStart+WindowH;
			vStart = yOffset;
			vEnd = vStart+WindowV;
			break;
		case 1:
			hEnd = frontHSize - xOffset;
			hStart = hEnd - WindowH;
			vStart = xOffset;
			vEnd = vStart + WindowV;
			break;
		case 2:
			hStart = xOffset;
			hEnd = hStart+WindowH;
			vEnd = frontVSize -yOffset ;
			vStart = vEnd-WindowV;
			break;
		case 3:
			hEnd = frontHSize - xOffset;
			hStart = hEnd - WindowH;
			vEnd = frontVSize -yOffset ;
			vStart = vEnd-WindowV;
			break;
		case 4:
			hStart = (frontHSize-mWindowH)/2;
			hEnd = hStart+mWindowH;
			vStart = (frontVSize-mWindowV)/2;
			vEnd = vStart+mWindowV;
			break;
		default:
			hStart=hEnd=vStart=vEnd=0;
			break;
	}
 	//printf("hStart=%d,hEnd=%d,vStart=%d,vEnd=%d\n",hStart,hEnd,vStart,vEnd);
	sp1kHALCdspAFwinsizeSet(hStart,hEnd,vStart,vEnd);
	sp1kHALCdspAFWinEn(1,0,0,afFlt2Type,1);
	#else
	value = value;
	#endif
}

#define AF_ONE_D_FILTER		15
#define AF_NORMALIZATION_FACTOR		1000

UINT32 averageAfValue;
UINT32 afValueArray[AF_ONE_D_FILTER];
UINT8 afValueArrayIdx;

void appCalAfOsdShow(void)
{
	#if (FUNC_HOST_AF_FOCUS_ONE_WIN == 1)
	UINT8 i;
	UINT32 afValue;
	
	afValue = sp1kAfWinValueGet();
	
	afValue = afValue /AF_NORMALIZATION_FACTOR;

	afValueArray[afValueArrayIdx++] = afValue;

	if (afValueArrayIdx >= AF_ONE_D_FILTER) {
		afValueArrayIdx = 0;
	}

	for (i = 0; i<AF_ONE_D_FILTER; i++){
		averageAfValue = ((averageAfValue + afValueArray[i]) >> 1);
	}

	i = 0;

	afBuf[i] = averageAfValue;
	#if(FOCUS_MODE == TV_FOCUS)
	appNumshow(AFWINDOW[i].xpos,AFWINDOW[i].ypos,afBuf[i]);
	#endif
	
	#else
	UINT8 i;
	UINT32 afValue;
	UINT32 error;
	for(i=0;i<5;i++)
	{	
		appCalAfWindowCfg(i);
		afValue = appAfWinowValueGet();
		//printf("AF Value=%ld\n",afValue);
		afValue = afValue /1000;
		#if(FILTER == 1)
		afValue = afValue/2 +olderAfBuf[i]/2;
		olderAfBuf[i] =afValue;
		#elif(FILTER == 2)
		afValue = afValue/2 +olderAfBuf[i]*3/8 +oldestAfbuf[i]/8;
		oldestAfbuf[i] = olderAfBuf[i];
		olderAfBuf[i] = afValue;
		#endif
		error=afBuf[i]/25;//  4%
		if((afValue < (afBuf[i] +error)) && (afValue > (afBuf[i] -error)))
		{
		}
		else
		{
			afBuf[i] = afValue;
			#if(FOCUS_MODE == TV_FOCUS)
			appNumshow(AFWINDOW[i].xpos,AFWINDOW[i].ypos,afBuf[i]);
			#endif
		}

	}
	#endif
}

#define AF_AUTO_ROI		1
void appAfFrameInit(void)
{
	#if (FUNC_HOST_AF_FOCUS_ONE_WIN == 1)
	UINT16 frontHSize,frontVSize;
	UINT16 cropHSize,cropVSize;
	UINT8 En = 1;
	UINT32 FrameInfoAddr = K_SDRAM_CommonEndAddr;
	UINT16 value;
	UINT16 *pvalue;
	UINT8 tmp0,tmp1, i;
	UINT16 afHShift,afVShift,afHStart,afHEnd,afVStart,afVEnd;

	averageAfValue = 0;
	afValueArrayIdx = 0;
	for (i = 0; i<AF_ONE_D_FILTER; i++){
		afValueArray[i] = 0;
	}

	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewCropSizeGet(&cropHSize,&cropVSize);
	
	sp1kAfWinCfg(frontHSize,frontVSize,cropHSize,cropVSize);
	
	sp1kAfWinEnSet(En);

	/* show af ui config */
	#if (AF_AUTO_ROI == 1)

	value = value;
	pvalue = pvalue;
	tmp0 = tmp0;
	tmp1 = tmp1;
		
	i = 0;
	afHShift = (frontHSize - cropHSize)/2;
	afVShift = (frontVSize - cropVSize)/2;
	
	afHStart = 7 + afHShift;
	afHEnd = 16 + afHShift;

	afVStart = 7 + afVShift;
	afVEnd = 8 + afVShift;

	/* based 320*240 rate, 320*180 rate */
	AFWINDOW[i].xpos = afHStart/(cropHSize/320);
	AFWINDOW[i].ypos = afVStart/(cropHSize/320);
	AFWINDOW[i].width = (cropHSize - afHEnd)/(cropHSize/320);
	AFWINDOW[i].heigth = (cropVSize - afVEnd)/(cropHSize/320);

	#if(FOCUS_MODE == TV_FOCUS)
	sp1kOsdClear(0); 
	sp1kOsdSelWinSet(AFWINDOW[i].xpos , AFWINDOW[i].ypos , AFWINDOW[i].width , AFWINDOW[i].heigth , 2 , OSD_ATTR_COLOR_RED);
	#endif

	#if 1
	for ( i = 0 ; i < 5 ; i++ ) {
		//printf("AFWINDOW[%bu] = %d,%d,%d,%d\n",i,AFWINDOW[i].xpos,AFWINDOW[i].ypos,AFWINDOW[i].width ,AFWINDOW[i].heigth);
	}
	#endif
	
	#else	
	sp1kNandRsvRead(0xA, FrameInfoAddr << 1);

	pvalue = (UINT16*)AFWINDOW;
	
	HAL_DramStartAddrSet(FrameInfoAddr, 1);
	
	for ( i = 0 ; i < 20 ; i++ ) {
		HAL_DramWordDataRead(&tmp0, &tmp1);
		value = (UINT16)tmp1 |((UINT16)tmp0 <<8);
		*pvalue = value;
		pvalue++;
	}

	#if(FOCUS_MODE == TV_FOCUS)
	sp1kOsdClear(0); 
	for(i=0;i<5;i++)
	{
		sp1kOsdSelWinSet(AFWINDOW[i].xpos , AFWINDOW[i].ypos , AFWINDOW[i].width , AFWINDOW[i].heigth , 2 , OSD_ATTR_COLOR_RED);
	}
	#endif
	
	#endif	

	#else
	UINT8 i;
	UINT16 value;
	UINT16 *pvalue;
	UINT8 tmp0,tmp1;
	UINT32 FrameInfoAddr = K_SDRAM_CommonEndAddr;
	UINT8 afRgbMode = 1;
	UINT8 afPgType = 0;
	UINT8 afFlt1Wt = 8;
	UINT8 afFlt2Wt = 0;
	UINT8 afCorThr = 10;
	UINT8 afCorVal = 0;
	#if 0
	UINT32 filesize;
	RD_Card2DRAM_ReadFile("AFWINDOWBIN", FrameInfoAddr, &filesize);
	#else
	sp1kNandRsvRead(0xA, FrameInfoAddr << 1);
	#endif
	pvalue = (UINT16*)AFWINDOW;
	HAL_DramStartAddrSet(FrameInfoAddr, 1);
	for(i=0;i<20;i++)
	{
		HAL_DramWordDataRead(&tmp0, &tmp1);
		value = (UINT16)tmp1 |((UINT16)tmp0 <<8);
		*pvalue = value;
		pvalue++;
	}
	sp1kHALCdspAFfilterwtSet(afFlt1Wt,afFlt2Wt);
	sp1kHALCdspAFdataModeSet(afRgbMode,afPgType);
	sp1kHALCdspAFcoringSet(afCorThr,afCorVal);   // coring threshold and gain  
#if(FOCUS_MODE == TV_FOCUS)
	sp1kOsdClear(0); 
	for(i=0;i<5;i++)
	{
		sp1kOsdSelWinSet(AFWINDOW[i].xpos , AFWINDOW[i].ypos , AFWINDOW[i].width , AFWINDOW[i].heigth , 2 , OSD_ATTR_COLOR_RED);
	}
#endif
	#endif
}


void appCalAF(void)
{
	appAfFrameInit();
	#if (FUNC_HOST_AF_FOCUS_ONE_WIN == 1)
	sp1kTimer1Config(16, pFunc_updateAfwindow, NULL);
	#else
	sp1kTimer1Config(400, pFunc_updateAfwindow, NULL);
	#endif
	sp1kTimer1Start();
}

void appCalAF2AStop(void)
{
	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbDisable();
}

void appCalAF2AStart(void)
{
	sp1kAeSetModeEn(AE_ENABLE);
	sp1kAwbEnable();
}

#if FUNC_HOST_TOUCH_PANEL
void appCalTouchPanel(void)
{
	static UINT8 drawFlag=1;
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();
	sp1kOsdClear(0);
	sp1kHalCtrlScreen(2);
	tpCalibrationEnter();
	dbgWdtSet(0);
	while(tpCalModeGet())
	{
		if((tpGetCurrCalPoint() == 1)&&(drawFlag==1))
		{
			sp1kOsdClear(0);
			appCrossLineDraw(280-2, 30-2,OSD_ATTR_COLOR_YELLOW,12);
			drawFlag=2;
		}
		else if((tpGetCurrCalPoint() == 2)&&(drawFlag==2))
		{
			sp1kOsdClear(0);
			appCrossLineDraw(280-2, 190-2,OSD_ATTR_COLOR_YELLOW,12);
			drawFlag=3;
		}
		else if((tpGetCurrCalPoint() == 3)&&(drawFlag==3))
		{
			sp1kOsdClear(0);
			appCrossLineDraw(40-2, 190-2,OSD_ATTR_COLOR_YELLOW,12);
			drawFlag=1;
		}
	}
	
	tpCalDatSave(0xfb, K_SDRAM_CommonEndAddr);
	osMsgPost(SP1K_MSG_TP_CAL_COMPLETE);
	puiPara->tpCorrect=0;
	dbgWdtSet(1);
	sp1kHalCtrlScreen(3);
}
#endif
void appCalStorageTest(void)
{
	UINT8 name[13]="SPEDTESTTXT";
	UINT32 v1= 0x100000;
	UINT32 v2= 0x100000;
	UINT32 t0,t1;
	UINT8 str[20]={0};
	
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	sp1kHalReadGTimer(&t0);
	if(sp1kDramFileSave(name, v1, v2)){
		sp1kHalReadGTimer(&t1);
		sprintf(str,"wSpeed:%ld KB/S",1000*v2/(1024*(t1-t0)));
		sp1kOsdStrDraw(1, 1, str,0);
	}
	sp1kHalReadGTimer(&t0);
	if(sp1kDramFileLoad(name, v1, &v2)){
		sp1kHalReadGTimer(&t1);
		sprintf(str,"rSpeed:%ld KB/S",1000*v2/(1024*(t1-t0)));
		sp1kOsdStrDraw(1, 2, str,0);
	}
	sp1kOsdDispBuffShow();
}

#if SUPPORT_AF


typedef struct _afinfo {
	UINT32 value;
	UINT16 postions;
} AFINFO;

static AFINFO afInfos[33] = {0};

#if SUPPORT_AWB_ADJ
extern UINT8 awb_auto_disable_flag;
extern UINT8 awb_adj_start_flag;
#endif


static void _AF_BubbleSort(int length){
    UINT32 tempValue;
	UINT16 tempPos;
	int i = 0, j = 0;
	
    for(i = length - 1; i > 0; i--){
        for(j = 0; j < i; j++){
            if(afInfos[j].value > afInfos[j+1].value){
				tempValue = afInfos[j].value;
				tempPos = afInfos[j].postions;

				afInfos[j].value = afInfos[j+1].value;
				afInfos[j].postions = afInfos[j+1].postions;

				afInfos[j+1].value = tempValue;
				afInfos[j+1].postions = tempPos;
            }
        }
    }
}


void AF_CalAF(void)
{
	UINT8 i;
	UINT32 afValue;

	static UINT8 idx = 0;
	static UINT32 stableCnt = 0;
	static UINT32 halfPos = 512;
	static UINT8 timeoutCnt = 20;
	static UINT32 avgAfValue = 0;
	UINT32 MaxAFValue = 0,MaxAFPos = 0,MaxIdx = 0, difference = 0;

	SINT32 tempval = 0;
	
	uiPara_t* puiPara = appUiParaGet();
	
	#if 1
		//dbgTimerStart();
		if (!sp1kHALReadAFWindow(&afValue)) {
			//dbgTimerStop();
			
			afValue = afValue /AF_NORMALIZATION_FACTOR;
			averageAfValue = afValue;

			#if 0
			printf("AF Value : [%ld]\n", averageAfValue);
			printf("AF Posti : [%d]\n", AF_GetPos());

			stableCnt++;
			if (stableCnt == 60) {
				stableCnt = 0;
				currPos += 128;
				AF_Set(currPos);
			}
			
			return;
			#endif
		}
		else {
			//printf("AF Get FAIL\n");
			
			return;
		}
	#else	
		afValue = sp1kAfWinValueGet();
		afValue = afValue /AF_NORMALIZATION_FACTOR;
	
		afValueArray[afValueArrayIdx++] = afValue;

		if (afValueArrayIdx >= AF_ONE_D_FILTER) {
			afValueArrayIdx = 0;
		}

		for (i = 0; i<AF_ONE_D_FILTER; i++){
			averageAfValue = ((averageAfValue + afValueArray[i]) >> 1);
		}

		#if 0
		printf("AF Value : [%ld]\n", averageAfValue);
		printf("AF Posti : [%d]\n", AF_GetPos());
		return;
		#endif
		
	#endif
	
	if (averageAfValue > 10) {
		stableCnt++;
		avgAfValue += averageAfValue;
		
		if (stableCnt == 1) {
			stableCnt = 0;

			avgAfValue = avgAfValue / 1;
			currPos = AF_GetPos();

			afInfos[idx].value = avgAfValue;
			afInfos[idx].postions = currPos;

			#if 1
			for (i = 0; i<=idx; i++){
				if(afInfos[i].value > MaxAFValue){
					MaxAFValue = afInfos[i].value;
					MaxAFPos = afInfos[i].postions;
					MaxIdx = i;
				}
			}
			#endif
			
			//printf("AF Value : [%ld]\n", avgAfValue);
			//printf("AF Posti : [%d]\n", currPos);
			
			if (idx >= 13) {
				difference = idx - 13;
			}
			
			if (MaxIdx && MaxIdx  <= difference) {
				// Í£Ö¹µ÷½¹
				sp1kTimer1Stop();
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);

				//printf("End AF Value : [%ld]\n", MaxAFValue);
				//printf("End AF Posti : [%ld]\n", MaxAFPos);
				
				AF_Set(MaxAFPos);
				
				puiPara->AdjustMacro = MaxAFPos;
				idx = 0;
				timeoutCnt = 20;
				memset(afInfos, 0x00, sizeof(AFINFO));
				
				sp1kHALCdspAFWinEn(0,0,0,1,0);
				
				sp1kAeSetModeEn(AE_ENABLE);
				sp1kAwbEnable();
				sp1kBtnEnableAll();
				
				osMsgPost(SP1K_MSG_SYS_AF_SUCCESS);
				#if SUPPORT_AWB_ADJ
				awb_adj_start_flag = 1;
				#endif
			}
			else if (currPos >= 512) {
				// Í£Ö¹µ÷½¹
				sp1kTimer1Stop();
				osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);

				_AF_BubbleSort(33);

				//printf("End AF Value : [%ld]\n", afInfos[32].value);
				//printf("End AF Posti : [%d]\n", afInfos[32].postions);
				
				AF_Set(afInfos[32].postions);
				
				puiPara->AdjustMacro = afInfos[32].postions;
				idx = 0;
				timeoutCnt = 20;
				memset(afInfos, 0x00, sizeof(AFINFO));
				
				sp1kHALCdspAFWinEn(0,0,0,1,0);
				
				sp1kAeSetModeEn(AE_ENABLE);
				sp1kAwbEnable();
				sp1kBtnEnableAll();

				osMsgPost(SP1K_MSG_SYS_AF_SUCCESS);
				#if SUPPORT_AWB_ADJ
				awb_adj_start_flag = 1;
				#endif
			} else {
				idx++;
				currPos += 16;

				AF_Set(currPos);
			}

			avgAfValue = 0;
		}
	} else {
		// ¾µÍ·±»¸Ç×¡
		if (--timeoutCnt == 0) {
			// Í£Ö¹µ÷½¹
			sp1kTimer1Stop();

			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			
			idx = 0;
			stableCnt = 0;
			halfPos = 512;
			timeoutCnt = 20;
			afInfos[0].postions = 0;
			afInfos[0].value = 0;
			afInfos[1].postions = 0;
			afInfos[1].value = 0;
			afInfos[2].postions = 0;
			afInfos[2].value = 0;

			AF_Set(0);
			
			sp1kHALCdspAFWinEn(0,0,0,1,0);
			
			sp1kAeSetModeEn(AE_ENABLE);
			sp1kAwbEnable();
			sp1kBtnEnableAll();

			osMsgPost(SP1K_MSG_SYS_AF_FAIL);
			#if SUPPORT_AWB_ADJ
			awb_adj_start_flag = 1;
			#endif
		}
	}

}


void AF_startCalAF(void)
{
	UINT16 frontHSize,frontVSize;
	UINT16 cropHSize,cropVSize;
	UINT8 En = 1;
	UINT8 i;
	UINT16 afHShift,afVShift,afHStart,afHEnd,afVStart,afVEnd;

	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbDisable();

	sp1kBtnDisableAll();
		
	#if SUPPORT_AWB_ADJ
	awb_adj_start_flag = 0;
	#endif

	averageAfValue = 0;
	afValueArrayIdx = 0;
	for (i = 0; i<AF_ONE_D_FILTER; i++){
		afValueArray[i] = 0;
	}

	sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewCropSizeGet(&cropHSize,&cropVSize);
	
	sp1kAfWinCfg(frontHSize,frontVSize,cropHSize,cropVSize);
	
	sp1kAfWinEnSet(En);

	/* show af ui config */
	i = 0;
	afHShift = (frontHSize - cropHSize)/2;
	afVShift = (frontVSize - cropVSize)/2;
	
	afHStart = 7 + afHShift;
	afHEnd = 16 + afHShift;

	afVStart = 7 + afVShift;
	afVEnd = 8 + afVShift;

	/* based 320*240 rate, 320*180 rate */
	AFWINDOW[i].xpos = afHStart/(cropHSize/320);
	AFWINDOW[i].ypos = afVStart/(cropHSize/320);
	AFWINDOW[i].width = (cropHSize - afHEnd)/(cropHSize/320);
	AFWINDOW[i].heigth = (cropVSize - afVEnd)/(cropHSize/320);
	
	sp1kTimer1Config(16, pFunc_AfCal, NULL);
	
	sp1kTimer1Start();
}
#endif


#endif
