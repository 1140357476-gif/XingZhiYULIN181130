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
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "Cmd_test.h"
#include "cardlink.h"
#include "disk.h"
#include "os_api.h"
#include "cmd.h"
#include "audio_rw.h"
//#include "cardfunc.h"
#include "sp1k_snap_api.h"
#include "initio.h"
#include "uiflow.h"
#include "usbmsdc2.h"
#include "panel.h"
#include "asicreg.h"
#include "fileindex.h"
#include "sp1k_aud_api.h"
#include "smcfunc.h"
#include "initio.h"
#include "timer.h"
#include "app_still_play.h"
#include "app_still_view.h"
#include "sdramhw.h"
#include "smcvar.h"
#include "func_ptr.h"
#include "main.h"
#include "hal_front.h"
#include "front_logi.h"

#include "key_scan.h"
#include "sp1k_snap_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_cdsp_api.h"
#include "solution.h"
#include "dos32.h"
#include "sp1k_hal_api.h"
#include "sp1k_rsvblk_api.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "sp1k_video_api.h"

#include "dbg_wdt.h"
#include "app_init.h"
#include "sp1k_ae_api.h"
#include "cdsp_filter.h"
#include "sp1k_gpio_api.h"
#include "hal_front_common.h"
#include "ms_cal.h"
#include "sp1k_calibration_api.h"
#include "motion_det.h"
#include "ae_debug.h"

#if (CDSP_IQ_TEST ==1)
#include "app_audio_play.h"
#endif

#if (SIZE_TEST == 1)
#include "hal_front.h"
#endif

#if (I2C_TEST == 1)
#include "i2c.h"
#endif

#if (RAW_TEST == 1)
#include "awb_api.h"
#include "sp1k_calibration_api.h"
#endif

#if LIFE_SPAN_TEST
#include "app_ui_osd.h"
#include "sp1k_video_api.h"
#endif
#include "cdsp_task.h"
#include "cdsp_misc.h"
#include "snap_qtable.h"

#if BADPIXEL_TEST   //xian
#include "sp1k_bp_api.h"
//768K bytes raw data
#define TEST_BP_RAW_BUF (K_SDRAM_PvBufAddrD - 1024UL*768/2)
#define TEST_BP_BAD_BUF (TEST_BP_RAW_BUF - (64UL*1024)*4/2)
void testBp(UINT8* ptr);
#endif

#define DISTORT_TEST    0
#if DISTORT_TEST
#include "spe_distort.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//#define printf(a)
#if !TAG_TODO
#define halJpgZFactorSet(a, b, c, d, e, f, g)
#define halJpegEncode(a, b, c, d, e)
#define HAL_JpegReadEncodeSize(a)
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 #if LIFE_SPAN_TEST
static UINT8 life_span=0;
UINT8 snapRdy=0;
UINT8 vidClipRdy=1;
UINT8  pv=1;
UINT8  vid=0;
UINT8  pb=0;
UINT8  pbNext=0;

 #endif
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 #if LIFE_SPAN_TEST
extern UINT16 RemainPictureNO;
 #endif
extern xdata	UINT8	G_File_Display;
extern xdata	UINT8	G_ucStorData[];

#ifdef SNAP_IQ_FLOW
extern xdata UINT8 snapIQLoadRaw;
extern xdata UINT8 snapIQLoadCdspParam;
extern xdata UINT8 snapIQSizeConfig;
extern xdata UINT16 snapIQConfigXSize;
extern xdata UINT16 snapIQConfigYSize;
#endif


//extern xdata UINT8 testQV;  //xian

extern UINT8 HAL_InitIISCodec(void);

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

#if 0
void appCmdProc(void) USING_0
{
	//printf("cmdPrco \n");
	cmdMonitor();
	osTaskSuspend(0);
}
#endif

#if FRONT_TEST
void
testFrontGain(
	UINT8* ptr
) USING_0
{
	UINT32 value, opt;

	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		value = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		opt = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	frontGainSet(0, value, 0, opt);

	printf( "Gain set value %ld, opt %ld\n", value, opt );

	return;
}

void
testFrontTime(
	UINT8* ptr
) USING_0
{
	UINT32 value, opt;

	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		value = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		opt = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	frontExposureTimeSet(0, value, 0, opt);

	printf( "Exp time set value %ld, opt %ld\n", value, opt );

	return;
}
#endif


#if CDSP_IQ_TEST
void
testEdgeFilter(
	UINT8* ptr
) 
{
	UINT8 level;
	level=atoi(ptr);
	
	sp1kCdspEdgeFilterSet(level,1);
}	

void
testCoringFilter(
	UINT8* ptr
) 
{
	UINT8 level;
	level=atoi(ptr);
	
	sp1kCdspCoringFilterSet(level);
}	

void
testSnapIQ(
	UINT8* ptr
)
{
	UINT8 level,i;


	//sp1kJpegQTableSet( 85, 0, 0, 1 );
	
	//sp1kSnapSizeSet( 2592, 1944 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_FORMAT, SP1K_SNAP_FORMAT_422 );

	//sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
	
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_PRE_BRC, 0 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_POST_BRC, 0 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_COMPRESS_RATIO, 10 );

	sp1kSnapParamSet(SP1K_SNAP_PARAM_10_BIT_RAW_OPT,0);
	sp1kSnapParamSet(SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,0);
	
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 0 );
    sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_YUV_OPT, 0 );
    sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 1 );

  
    //sp1kSnapParamSet(SP1K_SNAP_PARAM_DZOOM_OPT, 1 );
    //sp1kSnapParamSet(SP1K_SNAP_PARAM_CHANGERAW, 0 );

	//sp1kSnapParamSet( SP1K_SNAP_PARAM_DOFLOW_OPT, 0 );


	sp1kAeSetModeEn(0);
	level= ptr[1]-'0';
	if(*ptr=='e'){		
		for (i=0;i<EDGE_LEVEL;i++){
			sp1kCdspCoringFilterSet(level);
			sp1kCdspEdgeFilterSet(i,0);		
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
			sp1kSnapTask();
		}
	}else if(*ptr=='c'){
		for (i=0;i<CORING_LEVEL;i++){
			sp1kCdspEdgeFilterSet(level,0);						
			sp1kCdspCoringFilterSet(i);
			appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
			sp1kSnapTask();
		}
	}

}


void 
testTD(
	UINT8* ptr
)
{
	UINT32 en, level;

	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		en = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		level = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}

	sp1kCdspTDEn((UINT8)en,(UINT8)level);

}

#endif

#if LSC_CAL_TEST
void 
lscCal(
	void
)
{		
	sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_X,16);
	sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_Y,16);
	sp1kLscCfgSet(SP1K_CALIB_LSC_LUMA_LEVEL,100);
	sp1kLscCfgSet(SP1K_CALIB_LSC_CHANNEL,LSC_3CH);
	sp1kLscCfgSet(SP1K_CALIB_LSC_RESOLUTION,8);

	sp1kLscCal();
}
#endif

#if SHUTTER_TEST
void
shutterOnOff(
	UINT8* ptr
)
{
	UINT16 en;
	//UINT8 t1,t2,t3,t4;
	en= atoi(ptr);



	sp1kSHT_ACfg();
	sp1kSHT_BCfg();
	
	//printf("sh en=%d\n",en);
	
	HAL_GlobalTimer2Start();
	
	if (en==0){
		sp1kSHT_BSet(1);	/*SHT Close*/
		sp1kSHT_ASet(1);	/*enable*/

		//HAL_Front10UsWait(1000);
		HAL_GlobalTimerWait(10);
		//HAL_GlobalTimerWait10us(200);
		//printf("off\n");
		//t1=XBYTE[0x20c1];
		//t2=XBYTE[0x20c2];
		sp1kSHT_ASet(0);	/*disable*/
		//t3=XBYTE[0x20c1];
		//t4=XBYTE[0x20c2];

		//printf("start=%bx,%bx\n",t1,t2);
		//printf("end=%bx,%bx\n",t3,t4);
		
	}
	else{
		sp1kSHT_BSet(0);	/*SHT Open*/
		sp1kSHT_ASet(1);	/*enable*/
		//HAL_Front10UsWait(1000);
		HAL_GlobalTimerWait(10);
		//HAL_Front10UsWait(1000);
		sp1kSHT_ASet(0);	/*disable*/
		
	}	
}

void 
shutterCal(
	void
)
{
	msCalMSCalibration();
}

void 
sutterAETest(
	void
)
{
	UINT8 i;
	UINT8 rawAvg[SP1K_CALIBRATION_OB_MAX];
	UINT16 lumCenter;
	UINT16 lum[100];
	//sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_AE_TEST );
	
	//sp1kCalibrationAETest( 40, 50, 60 );


	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB ); /* Get average value of R,Gr,Gb,B */
	
	sp1kCalibrationGammaSet(0); 					/* Gamma Function Disable*/
	sp1kCalibrationAWBSet(0);						/* AWB OFF */
	
	sp1kCalibrationAESet(0);  		/* turn off AE */

	
	for(i= 0; i<100 ;i++) {
		//sp1kCalibrationAEConfig(i, 50);	
		sp1kAeParamSet(SP1K_AE_gId,i);
		sp1kCalibrationAEConfig(i,50);
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		sp1kSnapTask();
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);

		sp1kCalibrationOBDataGet(rawAvg);
		lumCenter=((UINT16)rawAvg[1] + (UINT16)rawAvg[3])/2 ;
		lum[i]=lumCenter;		
	}

	for(i= 0; i<100 ;i++) {
		printf("idx=%b2x,lum=0x%x\n",i,lum[i]);	
	}
	
	//sp1kCalibrationAETestSave( id0, id3 - 1 );
	
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	

}

#endif

#if (MD_TEST==1)
void
motionDetTest(
	UINT8* ptr
)
{
	UINT8 opt;
	UINT16 value;

	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		opt = *ptr;
		ptr++;
		//opt = strtoul(ptr, 0, 0);
	}
	else {
		//printf( "Invalid input\n" );
	}

	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);

		value = strtoul(ptr,0,0);
	}
	else {
		//printf( "Invalid input\n" );
	}
	
	//printf("opt=%c value=%d\n",opt,value);

	if (opt=='i'){
		//sp1kMDInit();	
		sp1kMDEnSet(1);
		sp1kMDLearningStart(1);	
	}
	else if (opt=='c'){
		sp1kMDEnSet(0);
	}	
	else if (opt=='l'){
		sp1kMDLearningStart(1);	
	}
	else if (opt=='s'){
		sp1kMDSenSet((UINT8)value);	
	}
	
}
#endif

#if (SIZE_TEST == 1)
/**
  * @fn 	   void pvReportSize(void)
  * @brief		   prrpsize Cmd Test
  * @param	   NONE
  * @retval    NONE
  * @see		   NULL
  * @author    LinJie
  * @since	   2010-7-09
  * @todo	   N/A
  * @bug	   N/A
 */
 void TestReportSize(void)
 {
    UINT16 ImageSize;
    UINT8 SizeH,SizeL;

	XBYTE[0x22ef]=1; //reset test size 
	XBYTE[0x22ef]=0; //reset test size 
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

	SizeL=XBYTE[0x22f0];
	SizeH=XBYTE[0x22f1];

	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
    printf("Front HSize=%d\n",ImageSize);

    SizeL=XBYTE[0x22f2];
    SizeH=XBYTE[0x22f3];


	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
    printf("Front VSize=%d\n",ImageSize);


#if 1 
	SizeL=XBYTE[0x22f4];
	SizeH=XBYTE[0x22f5];
	 
    ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
    printf("Scale RGB Scale HSize=%d\n",ImageSize);

	SizeL=XBYTE[0x22f6];
	SizeH=XBYTE[0x22f7];
	 
    ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
    printf("Scale RGB Scale Crop VSize=%d\n",ImageSize);	 
#endif
	SizeL=XBYTE[0x22b4];
	SizeH=XBYTE[0x22b5];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	//printf("Core Mux HSize=%d\n",ImageSize);	 

	SizeL=XBYTE[0x22b6];
	SizeH=XBYTE[0x22b7];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	//printf("Core Mux VSize=%d\n",ImageSize);	 


	SizeL=XBYTE[0x22f8];
	SizeH=XBYTE[0x22f9];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	//printf("YUV HSize=%d\n",ImageSize);
	 
	SizeL=XBYTE[0x22fA];
	SizeH=XBYTE[0x22fB];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	//printf("YUV VSize=%d\n",ImageSize);	 


	SizeL=XBYTE[0x22b8];
	SizeH=XBYTE[0x22b9];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	printf("YUV 1st HScale HSize=%d\n",ImageSize);	 

	SizeL=XBYTE[0x22ba];
	SizeH=XBYTE[0x22bb];
	 
	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
	printf("YUV 1st VScale VSize=%d\n",ImageSize);	 

 	SizeL=XBYTE[0x22bc];
	SizeH=XBYTE[0x22bd];
 
 	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
 	printf("YUV 2nd HScale HSize=%d\n",ImageSize);  

 	SizeL=XBYTE[0x22be];
 	SizeH=XBYTE[0x22bf];
 
 	ImageSize=(UINT16)SizeH*256+(UINT16)SizeL;
 	printf("YUV 2nd VScale VSize=%d\n",ImageSize);  

	 
 }

#endif

#if (RAW_TEST == 1)
/**
 * @fn		  void TestRAW(UINT8 *pt)
 * @brief	  RAW Cmd Test
 * @param	  NONE
 * @retval	  NONE
 * @see 	  	  NULL
 * @author	  LinJie
 * @since	  2010-7-09
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
TestRAW(
	UINT8 *pt
)
{
	UINT32 arg[8] = {0};
	
	cmdArgGet(&pt, arg, 8, 1);
	
	//printf("rawTest\n");

	if (strncmp((UINT8*)arg[0], "pvraw", 5) == 0) {
		#if 0
		sp1kAeSetModeEn(AE_DISABLE);
		sp1kAwbDisable();
		sp1kCalibrationObSet(0);
		sp1kCalibrationLscSet(0);
		sp1kCalibrationAWBSet(0);
		sp1kCalibrationGammaSet(0);
		calibrationPvRaw();
		#endif
	}
	else if (strncmp((UINT8*)arg[0], "capraw", 6) == 0) {
		#if 0
		/*OB LSC AWB Gamma turning off*/
		sp1kAeSetModeEn(AE_DISABLE);
		sp1kAwbDisable();
		sp1kCalibrationObSet(0);
		sp1kCalibrationLscSet(0);
		sp1kCalibrationAWBSet(0);
		sp1kCalibrationGammaSet(0);
		
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 1 ); 
		appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
		sp1kSnapTask();
		#endif
	}
	else if (strncmp((UINT8*)arg[0], "pvyuv", 5) == 0) {
		#if 0
		sp1kCalibrationPvYuv();
		#endif
	}
}

#endif

#if (AE_TEST == 1)
/**
 * @fn		  void TestAE(UINT8 *pt)
 * @brief	  Ae Cmd Test
 * @param	  NONE
 * @retval	  NONE
 * @see 	  	  NULL
 * @author	  LinJie
 * @since	  2010-7-09
 * @todo	  N/A
 * @bug 	  N/A
*/
#define AF_FIXTURE_CMD_EN	0
extern UINT8 aeCmdTest;
extern UINT8 G_AEReady;
extern UINT8 G_AESetting;
#if (AF_FIXTURE_CMD_EN == 1)
extern void sp1kPreviewCropSizeSet(UINT16 cropHSize, UINT16 cropVSize);
extern UINT8 sp1kPreviewSet(UINT8 vidMode,UINT16 pvHSize,UINT16 pvVSize,UINT16 fact,UINT8 pbFreeze);
#endif
void 
TestAE(
	UINT8 *pt
)
{
	UINT32 arg[8] = {0};
	#if (AF_FIXTURE_CMD_EN == 1)
	UINT16 frontHSize,frontVSize,cropHsize,cropVSize;
	#endif
	#if (AE_EXTERN_COMMAND_DEBUG_OPTION == 1)
	UINT16 SetLuma;
	UINT16 idx,eIdx;
	UINT8 freq;	
	#endif
	
	cmdArgGet(&pt, arg, 8, 1);
	
	//printf("aeTest\n");
	if (strncmp((UINT8*)arg[0], "table", 5) == 0) {
		aeCurrentTableCheck(AE_TABLE_AE);
	}
	else if (strncmp((UINT8*)arg[0], "gain", 5) == 0) {
		aeCurrentTableCheck(AE_TABLE_GAIN);
	}
	#if (AF_FIXTURE_CMD_EN == 1)
	if (strncmp((UINT8*)arg[0], "afraw", 5) == 0) {
		#if 1
		//sp1kAeSetModeEn(AE_DISABLE);
		//sp1kAwbDisable();
		sp1kCalibrationObSet(1);
		sp1kCalibrationLscSet(1);
		sp1kCalibrationAWBSet(0);
		sp1kCalibrationGammaSet(0);
		calibrationPvRaw();

		sp1kCalibrationAWBSet(1);
		sp1kCalibrationGammaSet(1);
		/* preview setting */
		frontPreviewSizeGet(0, &frontHSize, &frontVSize);
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
		
		sp1kPreviewCropSizeSet(cropHsize,cropVSize);
		
		sp1kPreviewSet(0, 640, 480, 100/*PvZoomCtrl.factor*/, 0);	
		#endif
	}
	#endif
	#if (AE_EXTERN_COMMAND_DEBUG_OPTION == 1)
	else if (strncmp((UINT8*)arg[0], "on", 2) == 0) {
		//printf("cmd set ae enable\n");
		sp1kAeSetModeEn(AE_ENABLE);
	}
	else if (strncmp((UINT8*)arg[0], "off", 3) == 0) {
		//printf("cmd set ae disable\n");
		sp1kAeSetModeEn(AE_DISABLE);
	}
	else if (strncmp((UINT8*)arg[0], "luma", 4) == 0) {
		//printf("cmd set ae luma\n");
		SetLuma = (UINT16)arg[1];
		printf("set Luma = %d\n", SetLuma);
		sp1kAeParamSet(SP1K_AE_TARGET_LUMA, SetLuma);
		sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
	}
	else if (strncmp((UINT8*)arg[0], "fdon", 4) == 0) {
		//printf("cmd set fd ae enable\n");
		sp1kFdAeSetModeEn(AE_ENABLE);
	}
	else if (strncmp((UINT8*)arg[0], "fdoff", 5) == 0) {
		//printf("cmd set fd ae disable\n");
		sp1kFdAeSetModeEn(AE_DISABLE);
	}
	else if (strncmp((UINT8*)arg[0], "idx", 3) == 0) {
		//printf("cmd set ae idx\n");
		idx = (UINT16)arg[1];
		aeStatusGet(SP1K_AE_G_AETblSel,&freq);
		printf("idx=%d, freq=%bu\n",idx,freq);
		sp1kCalibrationAEConfig(idx,freq);	
	}
	else if (strncmp((UINT8*)arg[0], "scan", 4) == 0) {
		//printf("cmd set scan ae table\n");
		eIdx = (UINT16)arg[2];
		aeStatusGet(SP1K_AE_G_AETblSel,&freq);
		aeCmdTest = 1;
		for (idx = (UINT16)arg[1];idx<=eIdx;idx++) {
			aeParamSet(SP1K_AE_gId,idx);
			dbgWdtFeed(1000);
			while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH ) );
			dbgWdtFeed(1000);
			HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 12);
			printf("idx=%d, freq=%bu, luma16=%d, luma=%bu\n",idx,freq,(UINT16)aeFlowGetCurLumaU16(),(UINT8)aeFlowGetCurLuma());
		}

		aeCmdTest = 0;
	}
	#endif
}
#endif

#if (I2C_TEST == 1)
/**
 * @fn		  void TestI2C(UINT8 *ptr)
 * @brief		  i2c Cmd Test
 * @param	  NONE
 * @retval	  NONE
 * @see 		  NULL
 * @author	  LinJie
 * @since	  2010-7-09
 * @todo	  N/A
 * @bug 	  N/A
*/
#define WRITE_I2C	0
#define READ_I2C	1

void TestI2C(UINT8 *ptr)
{
	#if 0

	#if 1
	UINT16 REG_ADDR16;
	UINT8 REG_ADDR[2];

	for (REG_ADDR16 = 0xC800; REG_ADDR16<0xC993; REG_ADDR16++) {
		i2cReg16Read16(&REG_ADDR16,&REG_ADDR[0],2,1,1,1,0);
		printf("R0x%x=0x%02bx%02bx\n",REG_ADDR16,REG_ADDR[0],REG_ADDR[1]);
	}

	for (REG_ADDR16 = 0xDC00; REG_ADDR16<0xDC03; REG_ADDR16++) {
		i2cReg16Read16(&REG_ADDR16,&REG_ADDR[0],2,1,1,1,0);
		printf("R0x%x=0x%02bx%02bx\n",REG_ADDR16,REG_ADDR[0],REG_ADDR[1]);
	}
	
	#else
	UINT8	REG_ADDR[2];
	UINT8	ADDR_LEN, DATA_LEN;
	UINT16	idx;

	ADDR_LEN = 1;
	DATA_LEN = 1;

	for(idx = 0; idx<=0x0000; idx++) {
		REG_ADDR[0] = (UINT8)idx;
		REG_ADDR[1] = 0x00;
		
		printf("read REG[0x%02bx] = ",REG_ADDR[0]);
		i2cRegRead(&REG_ADDR[0],&REG_ADDR[1],DATA_LEN,1,1,1,0);
		printf("0x%02bx\n",REG_ADDR[1]);

		
	}
	#endif

	
	#else
	UINT8	regType, idx;
	UINT8	REG_ADDR[4];
	UINT16	REG_VALUE16;
	UINT16	REG_ADDR16;
	UINT8	ADDR_LEN, DATA_LEN, REG_ADDR8;

	UINT32 arg[8] = {0};

	
	cmdArgGet(&ptr, arg, 8, 1);

	/* read  or write mode */
	if (strncmp((UINT8*)arg[0], "w", 1) == 0) {
        regType = WRITE_I2C;
    } else if (strncmp((UINT8*)arg[0], "r", 1) == 0) {
        regType = READ_I2C;
    } else {
        printf("Format Error!!!\n");
		return;
    }

	/* set addr len */
	idx = (UINT8)arg[1];
	if (idx == 2) {
        ADDR_LEN = 2;
    } else if (idx == 1) {
        ADDR_LEN = 1;
    } else {
        printf("Format Error!!!\n");
		return;
    }
	printf("ADDR_LEN = 0x%bx\n",ADDR_LEN);

	/* set data len */
	idx = (UINT8)arg[2];
	if (idx == 2) {
        DATA_LEN = 2;
    } else if (idx == 1) {
        DATA_LEN = 1;
    } else {
        printf("Format Error!!!\n");
		return;
    }
	printf("DATA_LEN = 0x%bx\n",DATA_LEN);

	
	REG_ADDR8	= (UINT8)arg[3];
	REG_ADDR16	= (UINT16)arg[3];
	REG_VALUE16	= (UINT16)arg[4];

	idx = 0;

	switch (regType) {
		case WRITE_I2C:
			if(ADDR_LEN == 2){
				REG_ADDR[idx++] = (UINT8)((REG_ADDR16 & 0xff00)>>8);	//high byte
				REG_ADDR[idx++] = (UINT8)(REG_ADDR16 & 0x00ff);			//low byte
				printf("write REG[0x%02bx%02bx] = ",REG_ADDR[idx-2],REG_ADDR[idx-1]);
			}
			else if(ADDR_LEN == 1){
				REG_ADDR[idx++] = (UINT8)(REG_ADDR16 & 0x00ff);
				printf("write REG[0x%02bx] = ",REG_ADDR[idx-1]);
			}
			else {
				printf("not support\n");
				return;
			}
	
			if(DATA_LEN == 2){
				REG_ADDR[idx++] = (UINT8)((REG_VALUE16 & 0xff00)>>8);
				REG_ADDR[idx++] = (UINT8)(REG_VALUE16 & 0x00ff);
				printf("0x%02bx%02bx\n", REG_ADDR[idx-2],REG_ADDR[idx-1]); 
			}
			else if(DATA_LEN == 1){
				REG_ADDR[idx++] = (UINT8)(REG_VALUE16 & 0x00ff);
				printf("0x%02bx ",REG_ADDR[idx-1]);
			}
			else {
				printf("not support\n");
				return;
			}
	
			if (ADDR_LEN == 2 && DATA_LEN == 2){
				ADDR_LEN = 3;
			}
	
			i2cCmdTableSend(&REG_ADDR[0], ADDR_LEN, 1, I2C_TRANS_MODE_NORMAL, 0, 0);
			break;
		case READ_I2C:
			if(ADDR_LEN == 2){
				REG_ADDR[idx++] = (UINT8)((REG_ADDR16 & 0xff00)>>8);
				REG_ADDR[idx++] = (UINT8)(REG_ADDR16 & 0x00ff);
				printf("read REG[0x%02bx%02bx] = ",REG_ADDR[idx-2],REG_ADDR[idx-1]);
				
				if(DATA_LEN == 1){
					i2cReg16Read(&REG_ADDR16,&REG_ADDR[idx++],DATA_LEN,1,1,1,0); 
					printf("0x%02bx\n",REG_ADDR[idx-1]);
				}
				else{
					i2cReg16Read16(&REG_ADDR16,&REG_ADDR[idx++],DATA_LEN,1,1,1,0); 
					printf("0x%02bx%02bx\n",REG_ADDR[idx-1],REG_ADDR[idx]);
				}
			}
			else if(ADDR_LEN == 1){
				REG_ADDR[idx++] = (UINT8)(REG_ADDR16 & 0x00ff);
				printf("read REG[0x%02bx] = ",REG_ADDR[idx-1]);
				i2cRegRead(&REG_ADDR[idx-1],&REG_ADDR[idx++],DATA_LEN,1,1,1,0);

				if(DATA_LEN == 1)
					printf("0x%02bx\n",REG_ADDR[idx-1]);
				else
					printf("0x%02bx%02bx\n",REG_ADDR[idx-1],REG_ADDR[idx]);
			}
			else {
				printf("not support\n");
				return;
			}
			break;
		default:
			printf("Format Error!!!\n"); 
			break;
	}

	#endif
}
#endif

#if 0//mark for code size
void testFunc(UINT8* ptr) USING_0

{
	if (strcmp(ptr , "string") == 0) {
//		printf("Test string\n");
	} else if (strtoul(ptr , NULL, 0) == 100UL) {
//		printf("Test int is %lu\n", strtoul(ptr , NULL, 0));
	}

//	printf("Cmd Line Test\n");
}

void testVoidFunc(void) USING_0
{
//	printf("This is a void func!\n");
}
void CardInitTest(void) USING_0{
	UINT8 ret;
		diskMemInit();
		diskGlobalInit();
		G_Card_Type = K_MEDIA_SDF;
		ret = diskDetect();
		ret = M_Card_Initialize(1);
		TestCard(0,0,0x100000);
}

void
TestPreview(
	void
) USING_0
{
#if 0 // hlc 0318 add, start
	Preview_Init(K_Preview_Width,K_Preview_Height,0);
#else
	sp1kPreviewSet( 0, K_Preview_Width, K_Preview_Height, 100, 0);
#endif // hlc 0318 add, end
}

void
testShutterSound(
	UINT8* ptr
) USING_0
{
	UINT32 size;

	UINT8 f[ 13 ] = "SHUTTER0BIN";

	f[ 11 ] = 0x20;
	f[ 12 ] = 0x00;

	dramFileLoad( f, K_SDRAM_CommonEndAddr, &size );

	if (strcmp(ptr, "1") == 0) {
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT, 1 );

//		sp1kSoundPlay( K_SDRAM_CommonEndAddr );
	}
	else {
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT, 0 );
	}
}

void
TestSnap(
	UINT8* ptr
) USING_0
{
	ptr = ptr;//Clear compiling warning
#if 0
	sp1kSnapInit( K_SDRAM_CommonEndAddr, K_SDRAM_TotalSize );
#else
	sp1kSnapInit( 8, K_SDRAM_CommonEndAddr, K_SDRAM_TotalSize + 0x3E0000UL );
#endif

	// no BRC, if with bRC, just set quality
	sp1kJpegQTableSet( 85, 0, 0, 1 );

#if 0
	sp1kSnapSizeSet( 4096, 3072 );
#endif
#if 1
	sp1kSnapSizeSet( 2592, 1944 );
#endif
#if 0
	sp1kSnapSizeSet( 2048, 1536 );
#endif

#if 0
	sp1kSnapSizeSet( 1280, 960 );
#endif

#if 0
	sp1kSnapSizeSet( 640, 480 );
#endif

	sp1kSnapDzoomFactorSet( 100, 100 );

	sp1kSnapParamSet( SP1K_SNAP_PARAM_QUALITY, SP1K_SNAP_QUALITY_SUPER_FINE );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_FORMAT, SP1K_SNAP_FORMAT_422 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_PRE_BRC, 1 );
	sp1kSnapParamSet( SP1K_SNAP_PARAM_POST_BRC, 0 );

	sp1kSnapParamSet( SP1K_SNAP_PARAM_COMPRESS_RATIO, 10 );

	sp1kSnapTask();

	printf( "finish test\n" );
}

void TestVideo(UINT8* ptr) USING_0{
	if (strcmp(ptr , "videomode") == 0) {
		videoRecPreviewInit();
		//printf("VM\n");
	}else if(strcmp(ptr , "vga") == 0){// video resolution set to VGA
		sp1kVideoRecSizeSet((UINT16)640);
		//printf("VS\n");
	}else if(strcmp(ptr , "qvga") == 0){// video resolution set to QVGA
		sp1kVideoRecSizeSet((UINT16)320);
		//printf("VS\n");
	}else if(strcmp(ptr , "vs") == 0){
		sp1kVideoRecStar();
		//printf("VS\n");
	} else if(strcmp(ptr , "vp") == 0){
		sp1kVideoRecPause();
		//printf("VP\n");
	} else if(strcmp(ptr , "vr") == 0){
		sp1kVideoRecResume();
		//printf("VR\n");
	}else if(strcmp(ptr , "pl") == 0){// video play or stop
		 videoPbGlobalInit();
		sp1kVideoPbStart(1);
		//printf("Vpl\n");
	}else if(strcmp(ptr , "pp") == 0){// video_play pause
		videoPbPause();
		//printf("Vpp\n");
	}else if(strcmp(ptr , "pr") == 0){// video_play resume
		videoPbResume();
		//printf("Vpr\n");
	}
}

void Testaudio(UINT8* ptr) USING_0{

	if (strcmp(ptr , "audiomode") == 0) {
		audioRecGlobalInit();
		//HAL_InitIISCodec();
		//printf("in audmode\n");
	}else if( strcmp(ptr , "s") == 0) {
		audioRecStart();
		//printf("aud_keyOnSnap\n");
	}else if (strcmp(ptr , "p") == 0){
		audioRecPause();
		//printf("aud_pause\n");
	}else if(strcmp(ptr , "r") == 0){
		audioRecResume();
		//printf("aud_resume\n");
	}else if(strcmp(ptr , "pl") == 0){
		audioPbStartOrStop(0);
	}else if(strcmp(ptr , "pp") == 0){
		audioPbPause();
	}else if(strcmp(ptr , "pr") == 0){
		audioPbResume();
	}else if(strcmp(ptr , "pfw") == 0){
		audioPbFW(0);
	}else if(strcmp(ptr , "pfr") == 0){
		audioPbFR(0);
	}else if(strcmp(ptr , "ok") == 0){
		audioPbFwFrRelease();
	}
}

void TestDisplay(UINT8* ptr) USING_0
{
	static UINT8 tmp;
#if	1
	static UINT8 x0, y0;
	x0 = XBYTE[REG_Disp_VlineL];
	y0 = XBYTE[REG_Disp_HpixelL];
	if(ptr[0]=='-') {
		if(ptr[1]=='x') {
			x0--;
		}
		else if(ptr[1]=='y') {
			y0--;
		}
	}
	else if(ptr[0]=='+') {
		if(ptr[1]=='x') {
			x0 += 3;
		}
		else if(ptr[1]=='y') {
			y0++;
		}
	}
	if(ptr[0]=='+' || ptr[0]=='-') {
//		printf("x0=%bu y0=%bu\n", x0, y0);
		if((XBYTE[REG_Disp_VsVdIntSts] & 0x0f)==0) {
			dispPnlVhSet(858, 525, x0, y0, 720, 240);
			dispSelWinSet(0, 0, 720, 240);
		}
		else if((XBYTE[REG_Disp_VsVdIntSts] & 0x0f)==1) {
			dispPnlVhSet(864, 625, x0, y0, 720, 288);
			dispSelWinSet(0, 0, 720, 288);
		}
		else {
			dispPnlVhSet(1560/2, 263, x0, y0, 640, 240);
			dispSelWinSet(0, 0, 320, 240);
		}
		//dispPnlVhSet(823, 262, x0, y0, 640, 240);
		//dispPnlVhSet(390, 264, x0, y0, 320, 240);
		//dispPnlVhSet(360,262,x0,y0,280,220);
		//dispSelWinSet(0, 0, 320*2, 240);
		dispSelWinAttrSet(SET_SEL_WIN_DEPTH,9);
		dispSelWinAttrSet(SET_SEL_WIN_COLOR,0);
		dispSelWinEn(1);
		osdPaletteSet(0,255,255,255);
		osdPaletteSet(1,255,0,0);
		osdPaletteSet(2,0,255,0);
		osdPaletteSet(3,0,0,255);
		XBYTE[REG_Disp_OsdX0H] = 1;
	}
//printf("%s %d\n", __FILE__, (unsigned short)__LINE__);
#else
	if(ptr[0]=='w') {
		tmp %= 3;
//		printf("tmp=%bd\n", tmp);
		//dispSelWinSet(0, 0, 320, 240);
		dispSelWinSet(0, 0, 720, 240);
		dispSelWinAttrSet(SET_SEL_WIN_DEPTH,13);
		dispSelWinAttrSet(SET_SEL_WIN_COLOR,tmp+1);
		//dispSelWinAttrSet(SET_SEL_WIN_COLOR,1);
		dispSelWinEn(1);
		osdPaletteSet(0,255,255,255);
		osdPaletteSet(1,255,0,0);
		osdPaletteSet(2,0,255,0);
		osdPaletteSet(3,0,0,255);
		XBYTE[REG_Disp_OsdX0H] = 1;
	}
	else {
		dispColorSet(tmp);
		XBYTE[REG_Disp_OsdX0H] = 1;
	}
//printf("%s %d\n", __FILE__, (unsigned short)__LINE__);
#endif
	if(ptr[0]=='c') {
//		printf("tmp=%bd\n", tmp);
		XBYTE[REG_Disp_PvV1L] = tmp;
	}
	if(ptr[0]=='p') {
		if(ptr[1]=='8') {
			printf("A025BN01\n");
			A025BN01_Init(8);
			dispImgWinEn(1);
			dispRegUpd();
		}
		#if	0
		else if(ptr[1]=='0') {
			printf("NTSC\n");
			Tv_Init(0);
			dispImgWinEn(1);
			dispRegUpd();
		}
		else if(ptr[1]=='1') {
			printf("PAL\n");
			Tv_Init(1);
			dispImgWinEn(1);
			dispRegUpd();
		}
		#endif
	}
	if(ptr[0]=='f') {
		HAL_DispScaleSet(640, 240, 100);
	}
}

void TestExp(void) USING_0
{
  //  UINT8  regaddr[8], regdata[8];
  // XBYTE[0x20e1]=0x09; //probe mode
  // XBYTE[0x2ae4]=0x10;
  //  regaddr[0]=0xa;
  //  HAL_ReadSSC(regaddr[0], regdata, 4,1);
  pvBufSave();

#if 0
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);

    printf("regaddr[1] =0x%bx\n",regaddr[1]);
    printf("regdata[1] =0x%bx\n",regdata[1]);

    printf("regaddr[2] =0x%bx\n",regaddr[2]);
    printf("regdata[2] =0x%bx\n",regdata[2]);

    printf("regaddr[3] =0x%bx\n",regaddr[3]);
    printf("regdata[3] =0x%bx\n",regdata[3]);
#endif

/*    HAL_FrontExpTimeSet(300,0);
    regaddr[0] = 0x10;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x04;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x2d;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x2e;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);	*/
    /*
    HAL_FrontExpTimeSet(50,0);
    regaddr[0] = 0x10;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x04;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x2d;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
    regaddr[0] = 0x2e;
    HAL_ReadSSC(regaddr[0], regdata, 1,1);
    printf("regaddr[0] =0x%bx\n",regaddr[0]);
    printf("regdata[0] =0x%bx\n",regdata[0]);
*/
}


void
TestUSB(
	UINT8* ptr
) USING_0
{
	if (strcmp(ptr, "hs") == 0) {
		printf( "Change to hs\n" );
		/* USB disconnect */
		/* USB force to hs */
		XBYTE[0x2517] = 0x10;
		HAL_Wait(500);
		/* USB connect */
		XBYTE[0x2517] = 0x00;
	}
	else if(strcmp(ptr, "fs") == 0){
		printf( "Change to fs\n" );
		/* USB disconnect */
		/* USB force to fs */
		XBYTE[0x2517] = 0x12;
		HAL_Wait(500);
		/* USB connect */
		XBYTE[0x2517] = 0x02;
	}

}

void
CustomTest(
	UINT8* ptr
) USING_0
{
	if (strcmp(ptr, "dramtest") == 0) {
		printf( "dramtest..\n" );
		DRAMReliabilityTest();
	}

	else if(strcmp(ptr, "adc") == 0){
		/*Initialization:*/
		UINT16 value;

		XBYTE[0x260B] |= 0x01;		// enable TP interrupt

	 	XBYTE[0x264A] = 0x03;                 // clock sel: 2MHz (125K)

	 	XBYTE[0x2648] = 0x03;                 // enable ADC, TP

		// read ADC value;

		XBYTE[0x2649] = 0x00;                 // sel channel 2
		XBYTE[0x264C] = 0x01;                 // snap ADC data
		value = ( (XBYTE[0x264d]^0x02) <<8) |  (XBYTE[0x264c]);
	 	printf("ADC0 value = 0x%03x%\n",value);

		XBYTE[0x2649] = 0x01;                 // sel channel 2
		XBYTE[0x264C] = 0x01;                 // snap ADC data
		value = ( (XBYTE[0x264d]^0x02) <<8) |  (XBYTE[0x264c]);
	 	printf("ADC1 value = 0x%03x%\n",value);

		XBYTE[0x2649] = 0x02;                 // sel channel 2
		XBYTE[0x264C] = 0x01;                 // snap ADC data
		value = ( (XBYTE[0x264d]^0x02) <<8) |  (XBYTE[0x264c]);
	 	printf("ADC2 value = 0x%03x%\n",value);

	}

	else if(strcmp(ptr, "aeoff") == 0){
		printf("AE off\n");
		XBYTE[0x2AD0] = 0x00;
		XBYTE[0x21EC] = 0x00;

	}

}


//Flow 4
void
initIntr(void) USING_0
{

	printf("-----Initial Preview & Interrupt-----\n");
//	XBYTE[0x2Ad0] = XBYTE[0x2Ad0] |0x83;
	XBYTE[0x2Ad0] = XBYTE[0x2Ad0] |0x03;
	XBYTE[0x2Ac4] = 0x11;//set VD rising & falling number
//	XBYTE[0x2Ac4] = 0x20;//set inthnum0
	XBYTE[0x2038] = 0x1f;//Enable GPIO
	XBYTE[0x21EC] = XBYTE[0x21EC] |0x04;//AE,AWB interrupt

	#if 0
	/* probe front 0117 preview crash after setting this*/
	XBYTE[0x20e1]=0x09;
       XBYTE[0x2ae4]=0x10;
	#endif


	HAL_FrontExpTimeSet(200,0);
	HAL_FrontGainSet(10,0);


	Shadow_ROMtoDRAMTest();
	Shadow_ImemTest();

	aeWinConfigSet();
//	AFTest();
//	AWBtest();

#if 0
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x80;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x81;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x82;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x83;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x84;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x85;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x86;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x87;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x88;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x89;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8a;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8b;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8c;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8d;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8e;//blue
	sp1kTimeDelay1s();sp1kTimeDelay1s();
	XBYTE[0x2ae0]=0x8f;//blue
#endif

}
void TestCDSP(UINT8* ptr) USING_0
{
	UINT8 i;

		if (strcmp(ptr, "pv") == 0){
#if 1
		for(i=0;i<5 ;i++)
			aeWinConfigSet();

	//	CDSPCheck();
#else
	for(i=127;i>=0;i--){
		//HAL_FrontVdWait(0, 1);
		HAL_FrontExpTimeSet(LVtbl60[i].shutter,1);
		HAL_FrontGainSet(LVtbl60[i].gain,1);
			printf("index is 0x%bx\n\n",i);
			//HAL_FrontVdWait(0, 1);
			//HAL_FrontVdWait(0, 1);
		sp1kTimeDelay1s();
			if ( i == 0) break;

	}


		for(i=0;i<128 ;i++){
			//HAL_FrontVdWait(0, 1);
			HAL_FrontExpTimeSet(LVtbl60[i].shutter,1);
			HAL_FrontGainSet(LVtbl60[i].gain,1);
			printf("index is 0x%bx\n\n",i);
			//HAL_FrontVdWait(0, 1);
			//HAL_FrontVdWait(0, 1);
		sp1kTimeDelay1s();

	      	}
      #endif
}

}
void
DRAMReliabilityTest(void)
{
	UINT32 i;
	UINT32 BufAddr0,BufAddr1,BufAddr2;
	UINT8 RMSB,RLSB,RMSB1,RLSB1,RMSB2,RLSB2;
	UINT8 err= 0;

	printf("2750=%bx\n",XBYTE[REG_Dram_SdramType]);
	switch(XBYTE[REG_Dram_SdramType]) {
		case 0x02:
			printf("64M\n");
			BufAddr0 = 0xF8000;
			BufAddr1 = 0x1F8000;
			BufAddr2 = 0x2F8000;
			break;
		case 0x03:
			printf("128M\n");
	BufAddr0 = 0x1F8000;
	BufAddr1 = 0x3F8000;
	BufAddr2 = 0x5F8000;
			break;
		case 0x04:
			printf("256M\n");
			BufAddr0 = 0x3F8000;
			BufAddr1 = 0x7F8000;
			BufAddr2 = 0xBF8000;
			break;
		default:
			printf("dram size not support\n");
			break;

				}
	/* Buf0 = 0x1F8000~0x208000(Word address), size=0x10000 Words
	 * Buf1 = 0x3F8000~0x408000
	 * Buf2 = 0x5F8000~0x608000									*/

	/* Fill data to Buf0 */
	M_DRAM_CardSetStartAddressV(BufAddr0,0);
	for (i=0;i<0x10000;i++)
	{
		M_DRAM_WriteWord((UINT8)i, (UINT8)(0xFFFF-i));
	}
	/* Copy Buf0 to Buf1 */
	HAL_GprmDramDma(BufAddr0<<1, BufAddr1<<1,  (UINT32)0x20000, 1);
	/* Copy Buf1 to Buf2 */
	HAL_GprmDramDma(BufAddr1<<1, BufAddr2<<1,  (UINT32)0x20000, 1);
	printf("WD");
	/* Compare and show the different add/data */
	#if 1
	for (i=0;i<0x10000;i++)
	{
		if ((i%0x1000) == 0x00) printf(".");
		M_DRAM_CardSetStartAddress((UINT32)(BufAddr0+i),1);
		M_DRAM_ReadWord(RLSB, RMSB);
		M_DRAM_CardSetStartAddress((UINT32)(BufAddr1+i),1);
		M_DRAM_ReadWord(RLSB1, RMSB1);
		M_DRAM_CardSetStartAddress((UINT32)(BufAddr2+i),1);
		M_DRAM_ReadWord(RLSB2, RMSB2);
		if ((RLSB!=RLSB1)||(RLSB1!=RLSB2)||(RMSB!=RMSB1)||(RMSB1!=RMSB2))
		{
			printf("fail in add%lx\n",i);
			//printf("l0,m0,l1,m1,l2,m2=%bx,%bx,%bx,%bx,%bx,%bx\n",RLSB,RMSB,RLSB1,RMSB1,RLSB2,RMSB2);
			err = 1;
		}

	}
	if (!err) printf("DRAM compare success\n");
	printf("done\n");
	#endif

}

void TestUI(UINT8* ptr) USING_0
{
	if (strcmp(ptr, "pv") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
	} else if (strcmp(ptr, "vv") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
	} else if (strcmp(ptr, "ar") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_AUDIO_REC);
	} else if (strcmp(ptr, "pb") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
	} else if (strcmp(ptr, "vp") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_VIDEO_PLAY);
	} else if (strcmp(ptr, "ap") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_AUDIO_PLAY);
	} else if (strcmp(ptr, "usb") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_USB);
	} else if (strcmp(ptr, "menu") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_MENU);
	} else if (strcmp(ptr, "s1") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_S1);
	} else if (strcmp(ptr, "u") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_UP);
	} else if (strcmp(ptr, "d") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_DOWN);
	} else if (strcmp(ptr, "l") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
	} else if (strcmp(ptr, "r") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_RIGHT);
	} else if (strcmp(ptr, "ok") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_OK);
	} else if (strcmp(ptr, "t") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_TELE);
	} else if (strcmp(ptr, "w") == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_WIDE);
	} else {
		dbgPrintf(CSTR_CMD_UI_HELP);
	}

}

void LoadBinFile(void)
{

	ReadLCBin( "LENSLUT0BIN", (UINT32)(K_SDRAM_PvAwbWndAddr-0x2000) );
	gammaBinRead( "GAMMA000BIN", (UINT32)(K_SDRAM_PvAwbWndAddr-0x2000) );
	cdspBinRead( "CDSPPARABIN", (UINT32)(K_SDRAM_PvAwbWndAddr-0x2000) );
}

void osdApiTest(void)
{
#if 1
	//LoadOsd2Dram();
	osdInit();



#endif
}

void
close3A(
		UINT8* ptr
)
{
		if (strcmp(ptr, "aeon") == 0) {
			sp1kAeSetModeEn(AE_ENABLE);
		}
		else if (strcmp(ptr, "aeoff") == 0) {
			sp1kAeSetModeEn(AE_DISABLE);
		}
		else if(strcmp(ptr, "awbon") == 0){
			sp1kAwbModeSet(AWB_MODE_Auto);
		}
		else if(strcmp(ptr, "awboff") == 0){
			sp1kAwbModeSet(AWB_PROC_Disable);
		}

}

void
testAlc(
	UINT8* ptr
) USING_0
{
	if (strcmp(ptr, "1") == 0) {
//		HB/LB TBD
//		sp1kAudParamSet( SP1K_AUD_PARAM_ALC_HB, 0 );
//		sp1kAudParamSet( SP1K_AUD_PARAM_ALC_LB, 0 );

		sp1kAudParamSet( SP1K_AUD_PARAM_ALC_WIN_SIZE, 1024 );
		sp1kAudParamSet( SP1K_AUD_PARAM_ALC_OPT, 1 );
	}
	else {
		sp1kAudParamSet( SP1K_AUD_PARAM_ALC_OPT, 0 );
	}
}
#if 1
void testCradIsp(UINT8* ptr) USING_0
{
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
	}
	else {
		//printf( "Invalid input\n" );
	}
	sp1kCardFirmwareUpdate(ptr);
}
void testCradBoot(UINT8* ptr) USING_0
{
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
	}
	else {
		//printf( "Invalid input\n" );
	}
	sp1kCardFirmwareBoot(ptr);
}
#endif
#endif
#if 0
UINT8 MemRawCmp(UINT32 addr1, UINT32 addr2, UINT32 num){
	UINT8 err=0;
	UINT8 tmp0, tmp1, tmp2, tmp3;
	UINT32 i;
	//printf("num=%lx,addr1=%lx,addr2=%lx\n", num, addr1, addr2);
	for(i=0; i<num/2; i++)
	{
		M_DRAM_SetStartAddress(addr1 + i, TRUE);
		M_DRAM_ReadWord(tmp0, tmp1);
		M_DRAM_SetStartAddress(addr2 + i, TRUE);
		M_DRAM_ReadWord(tmp2, tmp3);
		if((tmp0 == tmp0) && (tmp1 == tmp3)){
			continue;
		}
		printf("i=%06lx <%02bX%02bX%02bX%02bX != %08lx>\n", i, tmp0, tmp1,tmp2, tmp3, i);
		err++;
		return 1;
	}
	printf("comp end, err=%bd\n", err);
	return err;
}
#include "spifunc.h"
void testRawAccess(UINT8* ptr) USING_0
{
	UINT32 i=0, j, addr, size;
	UINT32 v1,v2,v3,v4;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		v1 = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		v2 = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		v3 = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		v4 = strtoul(ptr, 0, 0);
	}
	else {
		printf( "Invalid input\n" );
	}
	printf("v1=%lx,v2=%lx,v3=%lx,v4=%lx\n",v1,v2,v3,v4);
	XBYTE[0x2AD0] = 0x00;
	XBYTE[0x21EC] = 0x00;
	if( v1 == 0x01 ){
		EA = 0;
		sp1kNandRsvRead(v2,v3);
		#if 1
		dumpData(256, (v3 >> 1), 1);
		#endif
		EA =1;
	}
	if( v1 == 0x02 ){
		HAL_FlashMode(K_MEDIA_Spi,0,0);
		#if 1
		sfPhySectorR(v2 >> 9 , (UINT16)(v3 >> 9) , 512, 0x200000, 0);
		dumpData(v3, 0x200000, 1);
		#endif
		spiEraseSector(v2,v3,0);
		sfPhySectorR(v2 >> 9 , (UINT16)(v3 >> 9) , 512, 0x200000, 0);
		dumpData(v3, 0x200000, 1);
	}
	if( v1 == 0x03 ){
		for( i = 0; i < 100 ; i ++){
			sp1kNandRsvRead(0x0f,0x200000);
			sp1kNandRsvWrite(0x21,0x200000);
		}
	}
	if( v1 == 0x04 ){
		if(( v3 - v2 ) == 1 ){ v1 = 5;}
		while( v1 ){
			v1--;
			for( i = v2; i < v3 ; i ++){
				if(FALSE != sp1kNandRsvRead(i,0x200000)){
					//dumpData(512, 0x100000 ,1);
					sp1kNandRsvWrite(i,0x200000);
				}
			}
		}
	}
	if( v1 == 0x05 ){
		sp1kNandRsvRead(v2,0x200000);
		sp1kNandRsvWrite(v2,0x200000);
	}
	if( v1 == 0x06 ){
		if( v4 == 0 ){
		    v4 = 1;
		}
		while( 1 ){
			//HAL_GlobalReadGTimer(&i);
			//i = rand(i);
			for( i = v2; i < v3 ; i ++){
				if(FALSE == sp1kNandRsvSizeGet(i, &j, &size )) continue;
				printf("idx=%lx,offset=%lx,size=%lx\n",i,j,size);
				size = spiReadAllRaw(0x280000);
				//printf("idx=%lx\n",i);
				if(FALSE != sp1kNandRsvRead(i,0x280000)){
					//dumpData(512, 0x100000 ,1);
					sp1kNandRsvWrite(i,0x280000);
					//sp1kHalWait(1000);
					#if 1
					size = spiReadAllRaw(0x180000);
					if(0 != MemRawCmp(0x180000, 0x280000, size)){
						return;
					}
					#endif
				}
			}
			if( v4 <= 0 ){
				return;
			}
			v4--;
			if( v4 <= 0 ){
				return;
			}
		}
	}
}
#endif

#if LIFE_SPAN_TEST
void DscLife_span(
	UINT8* ptr
){
	ptr=ptr;
	life_span =1;
	osTaskCreate(pFunc_TestDscLife_span, OS_PRIO_TEST);
}
UINT8  testDscLife_span(
	void
){

	static UINT16 fileCnt;
	static UINT8 pvFlg=0;
	static UINT8 vidFlg=0;
	static UINT8 pbFlg=1;
	UINT32 time;
	dateStc_t dateInf;
	static UINT8 cnt=0;

	if(life_span==0){
		return FAIL;
	}
	if(vid){//enter video mode
		if(vidFlg==0){
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			vidFlg =1;
			sp1kHalReadRTC(&dateInf);
			printf("%d-%bu-%bu  %bu:%bu\n",2000U+(UINT16)dateInf.Year,dateInf.Month,dateInf.Day,dateInf.Hour,dateInf.Minute);

		}
		if(vidClipRdy==0){
			printf("%d\n",(UINT16)__LINE__);
			vid=0;
			pb=1;
			vidClipRdy=1;
			cnt =0;
		}
	} else if(pb){//enter pb
		printf("%d\n",(UINT16)__LINE__);
		cnt++;
		if(cnt<10)
			return FAIL;

		printf("pb\n");
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		fileCnt= G_USR_FileCount;
		pb=0;
		pbNext =1;
	}else if(pbNext) {//pb next
		printf("%d\n",(UINT16)__LINE__);
		if((sp1kVideoPbStatusGet()==0)&&(fileCnt)) {
			if(appCurrStateGet() != APP_STATE_STILL_PLAY)
				return FAIL;
			//osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			if(pbFlg==0){
				cnt++;
				if(cnt<10)
					return FAIL;
				osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
				sp1kHalReadRTC(&dateInf);
				fileCnt --;
				pbFlg= 1-pbFlg;
				printf("%d-%bu-%bu  %bu:%bu\n",2000U+(UINT16)dateInf.Year,dateInf.Month,dateInf.Day,dateInf.Hour,dateInf.Minute);
			}else{
				osMsgPost(SP1K_MSG_KEY_PRESS_OK);
				pbFlg= 1-pbFlg;
				cnt=0;
			}
		} else if(fileCnt==0){
			if(sp1kVideoPbStatusGet()==0){
				printf("del all\n");
				appFileDelete(1);//delete all
			}
			else
				return FAIL;

			if(appPreStateGet(0) == APP_STATE_STILL_VIEW) {
				vid =1;
				vidFlg =0;
				pbNext =0;
			}else {
				pv=1;
				pvFlg=0;
				pbNext =0;
			}
		}
	}else if(pv){//enter pv mode
		printf("%d\n",(UINT16)__LINE__);
		if(pvFlg ==0) {
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			printf("pv--\n");
			osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			pvFlg=1;
		}
		if(RemainPictureNO&&snapRdy){
			sp1kHalReadRTC(&dateInf);
			printf("%d-%bu-%bu  %bu:%bu\n",2000U+(UINT16)dateInf.Year,dateInf.Month,dateInf.Day,dateInf.Hour,dateInf.Minute);
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
		}else if(RemainPictureNO==0){
			pv=0;
			pb=1;
		}
	}

	return SUCCESS;

}
#endif

#if 0 //xian mask
void TestPb(UINT8* ptr) USING_0
{
//    PBFile_t fileInfo;

	if (strcmp(ptr, "init") == 0) {
		osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
	} else if (strcmp(ptr, "l") == 0) {
		pbMultiPlayLayoutInit(9, 1);
		printf("layout\n");
	} else if (strcmp(ptr, "m") == 0) {
		if (*(ptr + 2) == '1') {
			printf("pb 1\n");
			pbMultiPlay();
		} else {
			printf("pb 9\n");
			pbMultiPlay();
		}
		//printf("play >>>>\n");
    //xain ++
#if 0
    }else if(strcmp(ptr, "qv") == 0){
        testQV = !testQV;
printf("testQV=%bd\n", testQV);
    } else if(strcmp(ptr, "r") == 0){
//		appUiBusyWait(250, 0);
		sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
//		appUiBusyExit();
        sp1kPbParamSet(PB_PARAM_ROT, 1);

		sp1kPbSingleImgPlay(DOS_FileIndexGet(pbFile.dwIndex));
    } else if(strcmp(ptr, "rs") == 0){
        sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
        sp1kPbImgRotate(&fileInfo);
        sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
    } else if(strcmp(ptr, "rb") == 0){
        sp1kPbParamSet(PB_PARAM_ROT, 0);    //clear
#endif
    //xian --
	} else if (ptr != NULL){
		//sp1kPbSingleImgPlay(sp1kPbFileIndexByOrderGet((UINT16)strtoul(ptr , NULL, 0)));
		//pbMultiPlay(1, (UINT8)strtoul(ptr , NULL, 0));
		//sp1kPbDZoomFactorSet((UINT16)strtoul(ptr , NULL, 0));
		if (0){
			UINT32 size;
			UINT32 srcAddr = K_SDRAM_CommonEndAddr;
			UINT32 vlcAddr = K_SDRAM_CommonEndAddr + 320UL * 240UL;
			UINT32 vlcSize;

			dramFileLoad("OPEN0001YUV", srcAddr, &size);

			sp1kJpegQTableSet( SP1K_STARTUP_IMG_QTABLE, 0, 0, 1 );

			HAL_JpgZFactorSet( 100, 100, 320, 240, 320, 240, 0x00 );

			HAL_JpegEncode( srcAddr, vlcAddr, 0, 0x20, 1 );

			HAL_JpegReadEncodeSize( &vlcSize );

			dramFileSave( "DCT00001JPG", vlcAddr, vlcSize);
		}
	}
}
#endif

#if 1
void testTSnap(UINT8* ptr) USING_0
{
	static UINT16 TestTime;
#if 0
	if ( ptr != NULL ) {
		TestTime = (UINT16)strtoul(ptr , NULL, 0);
		appStillViewInit(0);
		sp1kTimer1Config(10000, pFunc_TestSnap, (UINT8*)&TestTime);
		sp1kTimer1Start();
	}
#else
	//printf("Snap test begin !\n");
	if ( ptr != NULL ) {
		TestTime = (UINT16)strtoul(ptr , NULL, 0);
		//appStillViewInit(0);
		sp1kTimer1Config(3000, pFunc_TestSnap, (UINT8*)&TestTime);
		sp1kTimer1Start();
	}
#endif
}

UINT8 testTSnapRoutine(UINT16* ptr)
{
	static UINT16 time = 0;

#if 0
	if (ptr == NULL) {
		return FAIL;
	}
	if (*ptr) {
		osMsgPost(SP1K_MSG_KEY_PRESS_S2);
		(*ptr)--;
	} else {
		sp1kTimer1Stop();
	}
#else

	if (*ptr == 0) {
		osMsgPost(SP1K_MSG_KEY_PRESS_S2);
	} else {
		if (time < *ptr) {
			time++;
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
		} else {
			time = 0;
			sp1kTimer1Stop();
			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
			osMsgPost(SP1K_MSG_KEY_PRESS_S1);
			osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
		}
	}
#endif
	return SUCCESS;
}
void testTSnapStop(UINT8* ptr)
{
	ptr = ptr;
	sp1kTimer1Stop();
}
void testTPlayback(UINT8* ptr) USING_0
{
#if 1
	static UINT16 TestTime;
	if ( ptr != NULL ) {
		TestTime = (UINT16)strtoul(ptr , NULL, 0);
		appStillPlayInit(0);
		sp1kTimer1Config(5000, pFunc_TestPb, (UINT8*)&TestTime);
		sp1kTimer1Start();
	}
#else
	if (ptr[0] == '1') {
		if (strcmp(ptr+2 , "fe") == 0) {
			printf("bingo\n");
			sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0xfe);
		} else {
			printf("adc 0 enable\n");
			sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0xff);
		}
	}
	if (strcmp(ptr, "11") == 0) {
		printf("adc 0 disable\n");
		sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0);
	}
	if (strcmp(ptr, "2") == 0) {
		printf("adc 1 enable\n");
		sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0xff);
	}
	if (strcmp(ptr, "22") == 0) {
		printf("adc 1 disable\n");
		sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0);
	}
	if (strcmp(ptr, "3") == 0) {
		printf("gpio enable\n");
		sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0xff);
	}
	if (strcmp(ptr, "33") == 0) {
		sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0);
		printf("gpio disable\n");
	}
#endif

}

UINT8 testTPlaybackRoutine(UINT16* ptr)
{
	if (ptr == NULL) {
		return FAIL;
	}

	if (*ptr) {
		if(appCurrStateGet() != APP_STATE_STILL_PLAY) {
			osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
		}
		osMsgPost(SP1K_MSG_KEY_PRESS_RIGHT);
		(*ptr)--;
	} else {
		sp1kTimer1Stop();
	}

	return SUCCESS;
}
#endif

#if 0 //(NAND_CMD_OPTION == 1)
void nandTest(UINT8* ptr) USING_0
{
	UINT32 i=0;
	UINT32 v1,v2,v3,v4;
	UINT8 str[32];
	UINT8 *pstr;
	UINT32 strLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	strLen = (UINT8 *)ptr - pstr;
	for( i == 0; i < strLen ; i++){
		str[i] = pstr[i];
	}
	str[strLen] = '\0';
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v3 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v4 = strtoul(ptr, 0, 0);
	printf("str=%s,v1=%lx,v2=%lx,v3=%lx,,v4=%lx\n",str,v1,v2,v3,v4);
	if (strcmp(str, "info") == 0) {
		//nand info
		{
			nandDetect();
			#if 0
			dumpData((v1 + 1)*512, G_Dram_Buf_Addr, 1);
			#endif
		}
		return;
	}
	if (strcmp(str, "rd") == 0) {
		{
			UINT8 sts;
			UINT32 phyaddr = v2;
			UINT32 k,ramAddr= (v3 >> 1);
			if( v4 == 0 ) v4 =1;
			printf("read phyaddr=%lx,ramAddr=%lx\n",phyaddr,ramAddr);
			//phy page read
			for (k = 0; k < v4; k++)
			{
				SDRAM_CardSetStartAddress(ramAddr, TRUE);
				sts= SMC_Card2Dram(phyaddr, ramAddr);
				if( sts != TRUE ){
					printf("Error phyaddr=%lx,ramAddr=%lx\n",phyaddr,ramAddr);
				}
				#if 1
				if( v1 == 1 ){
					dumpData(512, ramAddr, 1);
				}
				#endif
				phyaddr += 512L;
				ramAddr += 256L;
			}
		}
		return;
	}
	if (strcmp(str, "wr") == 0) {
		//phy page write
		{
			UINT8 sts;
			UINT32 phyaddr = v2;
			UINT32 k,ramAddr= (v3 >> 1);
			if( v4 == 0 ) v4 =1;
			//phy page read
			printf("write phyaddr=%lx,ramAddr=%lx\n",phyaddr,ramAddr);
			for (k = 0; k < v4; k++)
			{
				SDRAM_CardSetStartAddress(ramAddr, TRUE);
				sts = SMC_Dram2Card(phyaddr, 0xffff, ramAddr);
				if( sts != TRUE ){
					printf("Error phyaddr=%lx,ramAddr=%lx\n",phyaddr,ramAddr);
				}
				#if 1
				if( v1 == 1 ){
					dumpData(512, ramAddr, 1);
				}
				#endif
				phyaddr += 512L;
				ramAddr += 256L;
			}
		}
		return;
	}
	if (strcmp(str, "erase") == 0) {
		//block erase
		{
			UINT8 sts;
		UINT32 i,j;
		if( v1 == 0xffff ){
			//Total erase
			if( v2 == 0 ){
				{
				//Total erase non bad block
				UINT32 total = (G_Card_TotalBank * G_Card_PhysicalBankSize) / G_NANDF_BlockSize;
				printf("total=%lx\n",total);
				for( i = 0; i < total; i++){
					j = i * G_NANDF_BlockSize;
					sts = SMC_CheckValidBlock(&j);
					if( sts == TRUE ){
						printf("Erase phyaddr=%lx,block=%lx\n",j,j/G_NANDF_BlockSize);
						sts = SMC_EraseOneBlock(j);
						if( sts != TRUE ){
							printf("Error phyaddr=%lx\n",j);
						}
					}
				}
				}
			}
			else if( v2 == 1 ){
				{
				//Total erase non factory block
				UINT8 blksts;
				UINT32 total = (G_Card_TotalBank * G_Card_PhysicalBankSize) / G_NANDF_BlockSize;
				printf("total=%lx\n",total);
				for( i = 0; i < total; i++){
					j = i * G_NANDF_BlockSize;
					sts = SMC_CheckValidBlock(&j);
					if (G_NANDF_Category)
						blksts = G_ucStorData[512];
					else
						blksts = G_ucStorData[512 + 5];
					if( blksts != 0x00 ){
						printf("Erase block=%lx,phyaddr=%lx\n",j/G_NANDF_BlockSize,j);
						sts = SMC_EraseOneBlock(j);
						if( sts != TRUE ){
							printf("Error phyaddr=%lx\n",j);
						}
					}
				}
				}
			}
			else if( v2 == 2 ){
				//Total erase
				{
				UINT32 total = (G_Card_TotalBank * G_Card_PhysicalBankSize) / G_NANDF_BlockSize;
				printf("total=%lx\n",total);
				for( i = 0; i < total; i++){
					j = i * G_NANDF_BlockSize;
					printf("Erase block=%lx,phyaddr=%lx\n",j/G_NANDF_BlockSize,j);
					sts = SMC_EraseOneBlock(j);
					if( sts != TRUE ){
						printf("Error phyaddr=%lx\n",j);
					}
				}
				}
			}

		}
		else{
			{
			UINT8 sts;
			UINT32 phyaddr = v1 * G_NANDF_BlockSize;
			printf("Erase phyaddr=%lx,block=%lx\n",phyaddr,phyaddr/G_NANDF_BlockSize);
			sts = SMC_EraseOneBlock(phyaddr);
			if( sts != TRUE ){
				printf("Error phyaddr=%lx\n",phyaddr);
			}
			}
		}
		}
		return;
	}
	if (strcmp(str, "redun") == 0) {
		//redun data info
		{
			UINT8 sts;
			UINT32 phyaddr = v1 * G_NANDF_BlockSize;
			printf("phyaddr=%lx,block=%lx\n",phyaddr,v2);
			sts = SMC_Card2SramForExt(phyaddr);
			if( sts != TRUE ){
				printf("Error phyaddr=%lx\n",phyaddr);
			}
		}
		#if 1
		dumpData(16, &G_ucStorData[512], 0);
		#endif
		return;
	}
	if (strcmp(str, "scan") == 0) {
		//scan bad block
		{
			UINT8 sts, bad = 0;
			UINT32 i,j;
			UINT32 total = (G_Card_TotalBank * G_Card_PhysicalBankSize) / G_NANDF_BlockSize;
			printf("total=%lx\n",total);
			for( i = 0; i < total; i++){
				printf("\b\b\b\b%4lx",i);
				j = i * G_NANDF_BlockSize;
				sts = SMC_CheckValidBlock(&j);
				#if 0
				dumpData(16, &G_ucStorData[512], 0);
				#endif

				if( sts != TRUE ){
					bad ++;
					printf("\n%lx=bad, tal=%bx\n",i, bad);
				}
			}
		}
		#if 0
		dumpData(16, &G_ucStorData[512], 0);
		#endif
		return;
	}
	if (strcmp(str, "markbad") == 0) {
		//mark bad block
		{
			UINT8 sts;
			UINT32 phyaddr = v1 * G_NANDF_BlockSize;
			UINT32 page = G_NANDF_BlockSize >> 9;
			sts = SMC_EraseOneBlock(phyaddr);
			sts = SMC_BadBlockMark(v2);
			while( page ){
				sts = SMC_CheckValidBlock(&phyaddr);
				#if 1
				dumpData(16, &G_ucStorData[512], 0);
				#endif
				if( sts != TRUE ){
					printf("%lx=bad, %lx\n",v2/G_NANDF_BlockSize,phyaddr);
				}
				phyaddr += 512L;
				page--;
			}
		}
		return;
	}
}
#endif

#if 0//(SD_CMD_OPTION == 1)
void sdTest(UINT8* ptr) USING_0
{
	UINT32 i=0;
	UINT32 v1,v2,v3,v4;
	UINT8 str[32];
	UINT8 *pstr;
	UINT32 strLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	cmdTokenStrip(&ptr);
	strLen = (UINT8 *)ptr - pstr;
	for( i == 0; i < strLen ; i++){
		str[i] = pstr[i];
	}
	str[strLen] = '\0';
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v3 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v4 = strtoul(ptr, 0, 0);
	printf("str=%s,v1=%lx,v2=%lx,v3=%lx,,v4=%lx\n",str,v1,v2,v3,v4);
	if (strcmp(str, "info") == 0) {
	}
}
#endif
#if (SECTOR_CMD_OPTION == 1)
void sectorTest(UINT8* ptr) USING_0
{
	UINT32 i=0;
	UINT32 v1,v2,v3,v4;
	UINT8 str[32];
	UINT8 *pstr;
	UINT32 strLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	cmdTokenStrip(&ptr);
	strLen = (UINT8 *)ptr - pstr;
	for( i == 0; i < strLen ; i++){
		str[i] = pstr[i];
	}
	str[strLen] = '\0';
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v3 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v4 = strtoul(ptr, 0, 0);
	printf("str=%s,v1=%lx,v2=%lx,v3=%lx,,v4=%lx\n",str,v1,v2,v3,v4);
	if (strcmp(str, "info") == 0) {
	}
}
#endif
#if 0
#ifdef DOS32_SUPPORT
xdata UINT32 Cmd_Disk_Size;
void dosCpyCmd(UINT8* ptr) USING_0
{
	UINT32 fd0, fd1, size, cnt, i,seg;
	UINT8 name0[13];
	UINT8 name1[13];
	UINT32 v1;
	UINT8 name[14];
	UINT8 *pstr;
	UINT32 nameLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	nameLen = (UINT8 *)ptr - pstr;
	dosNameParsing(nameLen,pstr,name);
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	//printf("v1 = %lu\n",v1);
	cmdTokenStrip(&ptr);

	//DIR_Change("D:\\DCIM\\100MEDIA\\");
	fd0 = DOS_FileOpen(name, 'r');
	if(fd0 == 0) {
		DOS_FileClose(fd0);
	}
	#if 0
	DOS_FileInfoGet(fd0, FILE_SIZE, (UINT8 *)&size);
	seg = DOS_FileRead(fd0, 0x200000, size, 0);
	DOS_FileClose(fd0);
	#endif
	printf("name = %s\n",name);
	for( i = 0; i < 13; i++){
		name1[i] = name[i];
	}
	name1[12] = '\0';
	printf("name1 = %s\n",name1);
	if( v1 == 0 ) v1 = 1;
	//DIR_Change("D:\\DCIM\\100MEDIA\\");
	while( v1 ){
		if( v1 != 0xffff ){
			v1 --;
		}
		dcfBaseNameGet(name1, G_USR_FileIndex);
		name1[12] = '\0';
		printf("name = %s\n",name1);
		fd1 = DOS_FileOpen(name1, 'w');
		if(fd1 == 0) {
			DOS_FileClose(fd1);
		}
		#if 1
		DOS_FileInfoGet(fd0, FILE_SIZE, (UINT8 *)&size);
		printf("size=%lu\n",size);
		#endif
		cnt = 0;
		#if 0
		cnt = DOS_FileWrite(fd1, 0x200000, size);
		printf("size=%lu,cnt=%lu\n", size,cnt);
		DOS_FileClose(fd1);
		#else
		while(1){
			if( size > 16384){
				seg = DOS_FileRead(fd0, 0x200000, 16384, cnt);
				DOS_FileWrite(fd1, 0x200000, 16384);
				size -= 16384;
			}
			else if( size > 0 ){
				seg = DOS_FileRead(fd0, 0x200000, size, cnt);
				DOS_FileWrite(fd1, 0x200000, size);
				size = 0;
			}
			else{
				break;
			}
			cnt += seg;
			//printf("cnt=%lu,seg=%lu\n",cnt,seg);

		}
		DOS_FileClose(fd0);
		DOS_FileClose(fd1);
		#endif
		for( i = 0; i < 13; i++){
			name0[i] = name1[i];
		}
		name1[13] = '\0';
		#if 1
		fd0 = DOS_FileOpen(name0, 'r');
		if(fd0 == 0) {
			DOS_FileClose(fd0);
		}
		#endif
		printf("Disk %lu\n",DOS_GetDiskSpace());
		if( Cmd_Disk_Size == DOS_GetDiskSpace()){
			#if 1
			DOS_FileClose(fd0);
			DOS_FileClose(fd1);
			#endif
			break;
		}
		else{
			Cmd_Disk_Size = DOS_GetDiskSpace();
			if((Cmd_Disk_Size * 1024) <= size ) break;
		}
		G_USR_FileIndex++;
		if( G_USR_FileIndex >= 9999 ) break;
	}
}
#endif
#endif

extern xdata UINT8 G_SD_Phase;
extern xdata UINT8 G_Sd_Detect;
void CustomTest(UINT8* ptr)
{
	#if 0
	UINT8 i = 0;
	UINT8 j = 0;
	UINT32 k = 0;
	UINT32 v1,v2,v3,v4;
	//ptr = ptr;
	UINT8 str[32];
	UINT8 *pstr;
	UINT32 strLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	strLen = (UINT8 *)ptr - pstr;
	for( i == 0; i < strLen ; i++){
		str[i] = pstr[i];
	}
	str[strLen] = '\0';
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v3 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v4 = strtoul(ptr, 0, 0);
	printf("str=%s,v1=%lx,v2=%lx,v3=%lx,,v4=%lx\n",str,v1,v2,v3,v4);
	if (strcmp(str, "snap") == 0) {
		{
			static UINT16 times;
			times = (UINT16)v1;
			appStillViewInit(0);
			sp1kTimer1Config(2000, pFunc_TestSnap, (UINT8*)&times);
			sp1kTimer1Start();
		}
		return;
	}
	if (strcmp(str, "sdphase") == 0) {
		G_SD_Phase = v1;
		return;
	}
	if (strcmp(str, "sdtest") == 0) {
		while(1){
			appStorageMount(K_MEDIA_SDF);
			dosDirCmd("");
			i = 0;
			while( 1 ){
				printf("0\n");
				dosReadCmd("1.BIN 0x200000");
				printf("1\n");
				dosDirCmd("");
				printf("2\n");
				dosDelCmd("1.BIN");
				printf("3\n");
				dosDirCmd("");
				printf("4\n");
				dosWriteCmd("1.BIN 0 0x100000");
				printf("5\n");
				dosDirCmd("");
				printf("6\n");
				i++;
				if( i == 100 ){
					break;
				}
			}
			j ++;
			if( j == 2 ){
				break;
			}
		}
		return;
	}
	#endif
	ptr = ptr;
}

#if DISTORT_TEST
UINT32 getCmdVal(UINT8 **pptr)
{
    UINT8 *ptr;
    UINT32 val = 0;

    ptr = *pptr;
    while ( *ptr >= '0' && *ptr <= '9') {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    *pptr = ptr;
    return val;
}
#endif

#if 0
void testPbEffect(UINT8* ptr)
{
#if DISTORT_TEST
    UINT32 srcAddr = K_SDRAM_ImagePlaybackBufAddr;
    UINT32 tmpAddr = srcAddr + (UINT32)320*240;
    UINT32 time1, time2;
    UINT32 rawSize;
    UINT8 filename[12] = "00000000YUV";
    UINT16 imgW = 320, imgH = 240;
    UINT16 offX, offY, effW, effH, val[4];
    UINT8 i;
    UINT8 effId;

//    if(*ptr == '?'){
//        printf("effect [effectId(0~%bu)] [fileId(0~9)] [offX] [offY] [offW] [offH]\n", SPE_DISTORT_MAX-1);
//        return;
//    }

    effId = 0;
    if(*ptr >= '0' && *ptr <= '9'){
        effId = getCmdVal(&ptr);;
    	cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
    }

    //printf("effId = %bu\n", effId);
    //filename
    if(*ptr >= '0' && *ptr <= '9'){
        filename[7] = *ptr;
        if(*ptr == '4'){//1024x768
            imgW = 1024;
            imgH = 768;
            tmpAddr = srcAddr + (UINT32)1024*768;
            dramFileLoad(filename, srcAddr, &rawSize);
        }
        else
            dramFileLoad(filename, srcAddr, &rawSize);

    	cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        //printf("filename=%s\n", filename);
    }

	HAL_GprmCopy(
		srcAddr,
		imgW, imgH, 0, 0,
		pbDispBuffGet(1),
		320, 240, 0, 0,
		320, 240,
		0,
		1
	);

    val[0] = 0;
    val[1] = 0;
    val[2] = imgW;
    val[3] = imgH;

    for(i = 0; i < 4; i++){
        if(*ptr >= '0' && *ptr <= '9'){
            val[i] = getCmdVal(&ptr);
    	    cmdTokenStrip(&ptr);
            cmdWhiteSpaceStrip(&ptr);
        }
        else
            break;
    }
    offX = val[0];
    offY = val[1];
    effW = val[2];
    effH = val[3];

    printf("offX=%u, offY=%u, effW=%u, effH=%u\n", offX, offY, effW, effH);

    HAL_GlobalReadGTimer(&time1);
    speDistort(effId, srcAddr, imgW, imgH, offX, offY, effW, effH, tmpAddr);
    HAL_GlobalReadGTimer(&time2);

    printf("%s eff time: %lu\n", ptr, time2-time1);
    dramFileSave( "EFF_0001YUV", srcAddr, (UINT32)imgW*imgH*2 );
	HAL_GprmCopy(
		srcAddr,
		imgW, imgH, 0, 0,
		pbDispBuffGet(1),
		320, 240, 0, 0,
		320, 240,
		0,
		1
	);
#endif
	ptr = ptr;
#if 0
	UINT8 status;
	PBFile_t fileInfo;

	if (0) {
		printf("file index is %u\n", (UINT16)strtoul(ptr, NULL, 0));
		pbFileInfoGet((UINT16)strtoul(ptr, NULL, 0), &fileInfo);
	} else {
		UINT16 xSize, ySize, doYsize, yDis;
		UINT32 dstAddr, srcAddr;
		UINT32 fSize;
#if 1
		UINT8 fSrc[ 13 ] = "03200240YUV";
#else
		UINT8 fSrc[ 13 ] = "06400480YUV";
#endif
		UINT8 fDst[ 13 ] = "00000000YUV";

		ENTER_CRITICAL( status );
#if 1
		xSize = 320;
		ySize = 240;
#else
		xSize = 640;
		ySize = 480;
#endif

		doYsize = 32;
		yDis = doYsize * 4;

		dstAddr = K_SDRAM_CommonEndAddr;
		dstAddr = ( dstAddr + 16 ) >> 4 << 4;

		srcAddr = dstAddr + ( (UINT32)xSize * yDis );
		srcAddr = ( srcAddr + 16 ) >> 4 << 4;

		HAL_GprmDramFill( dstAddr, ((UINT32)xSize * ySize) << 1, 0x00);

		fSrc[ 11 ] = 0x20;
		fSrc[ 12 ] = 0x00;

		dramFileLoad( fSrc, srcAddr, &fSize );

#if 1
		HAL_CdspEdgeCoringMedianEnSet( 0, 0, 0, 0, 0 );
#else
		XBYTE[ 0x21d0 ] = 0x1c;
#endif
		cdspNegativeEffectSet();

		cdspYuv2Yuv( srcAddr, dstAddr, xSize, ySize, doYsize );

		cdspNormalEffectSet();

		fDst[ 11 ] = 0x20;
		fDst[ 12 ] = 0x00;

		dramFileSave( fDst, dstAddr, (UINT32)xSize * ySize * 2 );
		EXIT_CRITICAL( status );
		return;
	}

	sp1kPbEffectSet(&fileInfo, PB_IMG_EFFECT_NEGTIVE);
	printf("test finished\n");
#endif
}
#endif
#if NAND_RW
extern UINT32 xdata nandDma;
void testNandWR(UINT8* ptr)
{
	UINT32 p1, p2, p3, p4;
	UINT8 fName[12] = "SUNP0001JPG";

	//get param from cmd line
	cmdWhiteSpaceStrip(&ptr);
	p1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);

	cmdWhiteSpaceStrip(&ptr);
	p2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);

	cmdWhiteSpaceStrip(&ptr);
	p3 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);

	cmdWhiteSpaceStrip(&ptr);
	p4 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);

	printf("p1(%lu), p2(%lu), p3(%lu), p4(%lu)\n", p1, p2, p3, p4);

	fName[7] = '0' + (UINT8)p1;

#if 0
	switch (p1) {
		case 1:
			printf("SMC_Card2Sram\n");
XBYTE[0x2038] |= 0x01;
XBYTE[0x2030] |= 0x01;
			SMC_Card2Sram(0);
XBYTE[0x2030] &= ~0x01;
			break;
		case 2:
			printf("SMC_Sram2Card\n");
XBYTE[0x2038] |= 0x01;
XBYTE[0x2030] |= 0x01;
			SMC_Sram2Card(0x800, 0x200);
XBYTE[0x2030] &= ~0x01;
			break;
		case 3:
			printf("SMC_Card2Dram\n");
XBYTE[0x2038] |= 0x01;
XBYTE[0x2030] |= 0x01;
			SMC_Card2Dram(0, 0x100000);
XBYTE[0x2030] &= ~0x01;
			break;
		case 4:
			printf("SMC_Dram2Card\n");
XBYTE[0x2038] |= 0x01;
//XBYTE[0x2030] |= 0x01;
			SMC_Dram2Card(0x800, 0x200, 0x100000);
//XBYTE[0x2030] &= ~0x01;
			break;
		case 5:
			printf("SMC_EraseOneBlock\n");
XBYTE[0x2038] |= 0x01;
XBYTE[0x2030] |= 0x01;
			SMC_EraseOneBlock(0x200);
XBYTE[0x2030] &= ~0x01;
			break;
		case 6:
			break;
	}
#else
	XBYTE[0x2030] &= ~0x01;
	nandDma = 0;
	if (p3 == 1) {
		printf("w\n");
		HAL_GprmDramFill(0x100000UL, 0x100000 << 1, 0xaa);
		dramFileSave(fName, 0x100000UL, p2);
		printf("\n");
		cmdMemDump("0x1000");
	} else if (p3 == 2) {
		printf("32w\n");
		SMC_LDram2Card(0x10000, 0x100000UL, 0x80);
	} else {
		printf("r\n");
		HAL_GprmDramFill(0x200000UL, 0x100000 << 1, 0);
		dramFileLoad(fName, 0x200000UL, &p4);
		printf("Read data: 0x%lx\n", p4);
		if (0) {
			UINT32 add1 = 0x200000UL;
			UINT32 add2 = 0x200000UL + (p4 >> 1);
			UINT8 LSB, MSB;

			HAL_DramStartAddrSet(add1, 1);
			while (add1++ < add2) {
				HAL_DramWordDataRead(&LSB, &MSB);
				if (LSB != 0xaa || MSB != 0xaa) {
					CPrintf2("LSB %bu, MSB %bu", LSB, MSB);
				}
			}
		}
	}
	XBYTE[0x2030] &= ~0x01;
	printf("nandDma Loop is %lu\n", nandDma);
#endif
#if 0
	dbgTimerStart();
	XBYTE[0x2038] |= 0x01;
	XBYTE[0x2030] |= 0x01;
	M_Card_File_Write(1, K_File_SrcDataDram2Card, p1, 0x100000, 0);
	XBYTE[0x2030] &= ~0x01;
	dbgTimerStop();
#endif
}
#endif

#if CMD_BTN_TEST
UINT8 xdata testBtn;
extern UINT8 xdata cmdTest;
extern UINT8 pFunc_btnTestTask(void);
void btnTestTask(void)
{
	UINT16 adcVal1;
	UINT16 adcVal2;
	UINT16 adcVal0;
	static UINT8 bInit = 0;

	if (!bInit) {
		dbgPrintf(CSTR_CMD_ADC_TAST_START);
		bInit = 1;
	}
	if (!testBtn) {
		return;
	}
	testBtn = 0;
	if (SBUF0 == '1') {
		//SBUF0 = '\b';
		XBYTE[0x2649] = 0x01;
		HAL_Wait(20);
		XBYTE[0x264c] = 0x01;
		*((UINT8*)&adcVal1 + 1) = XBYTE[0x264c];
		*((UINT8*)&adcVal1 + 0) = (XBYTE[0x264d]^0x02);
		printf("\nAdc 1 val %u\n", adcVal1);
	}

	if (SBUF0 == '2') {
		//SBUF0 = '\b';
		XBYTE[0x2649] = 0x02;
		HAL_Wait(20);
		XBYTE[0x264c] = 0x01;
		*((UINT8*)&adcVal2 + 1) = XBYTE[0x264c];
		*((UINT8*)&adcVal2 + 0) = (XBYTE[0x264d]^0x02);
		printf("\nAdc 2 val %u\n", adcVal2);
	}
	if (SBUF0 == '0') {
		//SBUF0 = '\b';
		XBYTE[0x2649] = 0x00;
		HAL_Wait(20);
		XBYTE[0x264c] = 0x01;
		*((UINT8*)&adcVal0 + 1) = XBYTE[0x264c];
		*((UINT8*)&adcVal0 + 0) = (XBYTE[0x264d]^0x02);
		printf("\nAdc 0 val %u\n", adcVal0);
	}
}

#define TEST_CYCLE 300     //1 define the test cycle
#define ADC_DEV		5	//1 define the adc dev val

void testButton(UINT8* ptr)
{
	UINT16 i;
	UINT16 adcLastVal1;
	UINT16 adcLastVal2;
	UINT16 adcValErr1 = 0;
	UINT16 adcValErr2 = 0;
	UINT16 adcVal1;
	UINT16 adcVal2;

	if (strcmp(ptr, "s") == 0) {
		dbgPrintf(CSTR_CMD_ADC_TEST_S);

		/*Stop the key scan*/
		TIMER0_Stop();

		sp1kTimer1Config(100, pFunc_btnTestTask, NULL);
		dbgPrintf(CSTR_CMD_ADC_TEST_INFO);

		sp1kTimer1Start();
	} else if (strcmp(ptr, "e") == 0){
		dbgPrintf(CSTR_CMD_ADC_TEST_E);
		//osTaskSuspend(4);
		sp1kTimer1Stop();
		TIMER0_Start();
	} else if (strcmp(ptr, "a") == 0){
		dbgPrintf(CSTR_CMD_BTN_TEST_INFO);
		TIMER0_Stop();
		while (!getchar());

RESTART:
		putchar('\n');
		XBYTE[0x2649] = 0x01;
		HAL_Wait(20);
		XBYTE[0x264c] = 0x01;
		*((UINT8*)&adcLastVal1 + 1) = XBYTE[0x264c];
		*((UINT8*)&adcLastVal1 + 0) = (XBYTE[0x264d]^0x02);

		XBYTE[0x2649] = 0x02;
		HAL_Wait(20);
		XBYTE[0x264c] = 0x01;
		*((UINT8*)&adcLastVal2 + 1) = XBYTE[0x264c];
		*((UINT8*)&adcLastVal2 + 0) = (XBYTE[0x264d]^0x02);

		for (i = 1; i <= TEST_CYCLE; i++) {

			XBYTE[0x2649] = 0x01;
			HAL_Wait(20);
			XBYTE[0x264c] = 0x01;
			*((UINT8*)&adcVal1 + 1) = XBYTE[0x264c];
			*((UINT8*)&adcVal1 + 0) = (XBYTE[0x264d]^0x02);

			XBYTE[0x2649] = 0x02;
			HAL_Wait(20);
			XBYTE[0x264c] = 0x01;
			*((UINT8*)&adcVal2 + 1) = XBYTE[0x264c];
			*((UINT8*)&adcVal2 + 0) = (XBYTE[0x264d]^0x02);

			if (adcVal1 > (adcLastVal1 + ADC_DEV) || ((adcLastVal1 > ADC_DEV) && adcVal1 < (adcLastVal1 - ADC_DEV))) {
				printf("\nAdc 1 val skip from %u to %u, %u\n", adcLastVal1, adcVal1, ++adcValErr1);
			}
			if (adcVal2 > (adcLastVal2 + ADC_DEV) || ((adcLastVal2 > ADC_DEV) && adcVal2 < (adcLastVal2 - ADC_DEV))) {
				printf("\nAdc 2 val skip from %u to %u, %u\n", adcLastVal2, adcVal2, ++adcValErr2);
			}

			if (!(i % (TEST_CYCLE / 40))) {
				putchar('>');
			}
		}
		//printf("Test finished!\n");
		if (adcValErr1||adcValErr2) {
			printf("\nAdc 1 val err (%u)\nAdc 2 val err (%u)\n", adcValErr1, adcValErr2);
		} else {
			dbgPrintf(CSTR_CMD_BTN_TEST_NOERR);
		}
		dbgPrintf(CSTR_CMD_BTN_TEST_RETRY);
		while(1) {
			UINT8 tmpChar = getchar();
			if (tmpChar == 'r') {
				adcLastVal1 = 0;
				adcLastVal2 = 0;
				adcValErr1 = 0;
				adcValErr2 = 0;
				goto RESTART;
			} else if (tmpChar) {
				break;
			}
		}
		TIMER0_Start();
	}else {
		dbgPrintf(CSTR_CMD_ADC_HELP);
	}
}

#endif /*CMD_BTN_TEST*/

#if BADPIXEL_TEST
void testBp(UINT8* ptr)
{
    UINT32 rawAddr = (UINT32)TEST_BP_RAW_BUF;
    UINT32 badAddr = (UINT32)TEST_BP_BAD_BUF;
    UINT16 rawW = 1024, rawH = 768;
    UINT32 badTotal;
    UINT32 time1, time2;
    UINT8  cnt, low, high;
    UINT32 rawSize;
    UINT8 ret;
    UINT16 tBank, tAddr;

printf("addr : 0x%lx, 0x%lx\n", rawAddr, badAddr);
printf("img : %u, %u\n", rawW, rawH);

    if(strcmp(ptr, "raw") == 0){
        printf("snap raw\n");
        sp1kSnapParamSet(SP1K_SNAP_PARAM_SAVE_RAW_OPT, 0x01);
        osMsgPost(SP1K_MSG_KEY_PRESS_S2);
    }else if(strcmp(ptr, "find") == 0 || strcmp(ptr, "all") == 0){
        //read raw file
        printf("read raw\n");
        ret = dramFileLoad("RAW_0000RAW", rawAddr, &rawSize);
        if(ret != TRUE){
            printf("RAW_0000.RAW not found!\n");
            return;
        }

        //search bad pixel
        sp1kBpCfg(1, 20, (UINT32)1024*64);

        HAL_GlobalReadGTimer(&time1);
        badTotal = 0;
        HAL_CpuImemSdGet((logBank_t *)&tBank, &tAddr);
        HAL_CpuImemSdSet(LOG_BANK_16, 0xd000);
        sp1kBpFind(1, rawAddr, rawW, rawH, badAddr, &badTotal);
	 HAL_CpuImemSdSet(tBank, tAddr);	
        HAL_GlobalReadGTimer(&time2);

printf("find time: %lu\n", time2-time1);
printf("bad=%lu\n", badTotal);
#if 1
        if(badTotal!=0){
            HAL_DramStartAddrSet( badAddr, 1 );
            for(cnt=0; cnt < 10 && cnt < badTotal; cnt++){
                HAL_DramWordDataRead(&low, &high);
                printf("(0x%x, ", ((UINT16)high<<8)+low);
                HAL_DramWordDataRead(&low, &high);
                printf("0x%x)\n", ((UINT16)high<<8)+low);
            }
        }
#endif
        if(strcmp(ptr, "all") == 0){
            HAL_GlobalReadGTimer(&time1);
            bpBadPixelFix(rawAddr, rawW, rawH, badAddr);
            HAL_GlobalReadGTimer(&time2);
printf("fix time: %lu\n", time2-time1);
        }
    }
    else if(strcmp(ptr, "fix") == 0){
        ret = dramFileLoad("RAW_0000RAW", rawAddr, &rawSize);
        if(ret != TRUE){
            printf("RAW_0000.RAW not found!\n");
            return;
        }

        HAL_GlobalReadGTimer(&time1);
        bpBadPixelFix(rawAddr, rawW, rawH, badAddr);
        HAL_GlobalReadGTimer(&time2);
printf("fix time: %lu\n", time2-time1);
    }

}
#endif

#if FILE_ATTR_TEST
void testFileAttr(
	UINT8* ptr
)
{
	UINT8* p1, *p2, *p3, *p4;
	UINT8 fName1[12] = "SUNP0001WAV";
	UINT8 fName2[12] = "SUNP0001WAV";

	dcfFileAttrSet(fName1, fName2, ATTR_R | ATTR_A, ATTR_MODIFY_ATTR);
	//dcfFileAttrSet(fName1, fName2, ATTR_A, *ptr);
}

#endif //FILE_ATTR_TEST

void cmdMsgPost(UINT8* ptr)
{
	UINT32 arg[8];

	cmdArgGet(&ptr, arg, 8, 1);

	printf("msg post: %lx\n", arg[0]);
	osMsgPost(arg[0]);

}

void testBitRate(UINT8* ptr)
{
	UINT32 arg[8];
	cmdArgGet(&ptr, arg, 8, 1);
	arg[0] <<= 13; //arg[0] * 1024 * 8
	arg[0] = arg[0] * 30;
	sp1kVideoRecBitRateSet(arg[0]);
}
#if 0
void rtctest(void)
{
	UINT8 reliable;
	UINT32 pre_time;
	dateStc_t	time;
	dateStc_t	timeGet;

	#if 0
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	printf("%s\n",ptr);
	time.Year = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	time.Month = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	time.Day = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	time.Hour = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	time.Minute = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	time.Second = strtoul(ptr, 0, 0);
	#else
	time.Year = (UINT8)(2008 - 1980);
	time.Month = 9;
	time.Day = 23;
	time.Hour = 10;
	time.Minute = 20;
	time.Second = 0;
	#endif
	sp1kHalWriteRTCReg(0x02, 0x5a); //Secint/alarmint enable
	printf("Set Time ...\n");
	printf("Year    [%4d]\n",(UINT16)(1980 + time.Year));
	printf("Month   [%2bd]\n",time.Month);
	printf("Day     [%2bd]\n",time.Day);
	printf("Hour    [%2bd]\n",time.Hour);
	printf("Minute  [%2bd]\n",time.Minute);
	printf("Second  [%2bd]\n",time.Second);
	printf("\n\n");
	sp1kHalWriteRTC(&time);
	pre_time = time.Minute;
	time.Minute = time.Minute + 1;
	sp1kHalWriteRTCAlarm(&time);
	sp1kHalAlarmRTCIntrSet(1); /* enable alarm and clear alarm flag */
	sp1kHalReadRTC(&timeGet);
	printf("\n[%4d][%2bd][%2bd][%2bd][%2bd][%2bd]\n",(UINT16)(timeGet.Year + 1980),timeGet.Month,timeGet.Day
		,timeGet.Hour,timeGet.Minute,timeGet.Second);
	while(1){
		sp1kHalReadRTC(&timeGet);
		printf("\b\b\b\b[%2bd]",timeGet.Second);
		if( timeGet.Minute != pre_time ){
			pre_time = timeGet.Minute;
			printf("\n[%4d][%2bd][%2bd][%2bd][%2bd][%2bd]\n",(UINT16)(timeGet.Year + 1980),timeGet.Month,timeGet.Day
				,timeGet.Hour,timeGet.Minute,timeGet.Second);

			reliable = sp1kHalAlarmRTCIntrSet(1);/* enable alarm and clear alarm flag */
			if(reliable){
				printf("Alarm !\n");
				time.Minute = time.Minute + 1;
				pre_time = pre_time + 1;
				sp1kHalWriteRTCAlarm(&time);
				#if 0//suspend/resume test by using alarm interrupt
				//enter suspend, let alarm resume
				printf("Suspend ..\n");
				sp1kSuspendResume();
				printf("Resume ..\n");
				time.Minute = time.Minute + 1;
				//printf("time.Minute=%2bd\n",time.Minute);
				sp1kHalWriteRTCAlarm(&time);
				#endif
			}
		}
	}
}
#endif
#if FW_RED_SD_ON//meili add for fw read rfom sd 20101201
void FWreadfrmSD(void)
{
sp1kCardFirmwareBoot("FW.BIN", K_SDRAM_CommonEndAddr);
}
#endif


UINT8 TestFlag;
UINT8 AutoTestMode=0;
#if 0
void cmdCPUSts(UINT8* ptr) 
{
    UINT32 arg[8] = {0};
    cmdArgGet(&ptr, arg, 8, 1);

    if (strncmp((UINT8*)arg[0], "info", 4) == 0) {
	 printf("IE=%bx\n",IE);
	 printf("TCON=%bx\n",TCON);
	 printf("IP=%bx\n",IP);
	 printf("TH0=%bx\n",TH0);
	 printf("TL0=%bx\n",TL0);
	 printf("TMOD=%bx\n",TMOD);
     	 printf("0x2648=0x%bx,%bx,%bx\n",XBYTE[0X2648],XBYTE[0X2649],XBYTE[0X264A]);
    } else if (strncmp((UINT8*)arg[0], "IE", 2) == 0) {
        IE=arg[1];
    } else if (strncmp((UINT8*)arg[0], "TCON", 4) == 0) {
        TCON=arg[1];
    } else if (strncmp((UINT8*)arg[0], "IP", 2) == 0) {
        IP=arg[1];
    } else if (strncmp((UINT8*)arg[0], "flag", 4) == 0) {

    TestFlag=arg[1];
    printf("testflag=%bu\n",TestFlag);
    
    }

}
#endif
extern xdata UINT8 G_aeSpeed;
void cmdAutoTest(UINT8* ptr)
{
	UINT32 arg[8] = {0};
	cmdArgGet(&ptr, arg, 8, 1);
	if (strncmp((UINT8*)arg[0], "cap", 3) == 0) 
	{
	    AutoTestMode=0x55;
    	   if( appCurrStateGet() != APP_STATE_STILL_VIEW)
    	   {
		    osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
    	   }
	    osMsgPost(SP1K_MSG_KEY_PRESS_S2);
	} 
	else if (strncmp((UINT8*)arg[0], "video", 5) == 0) 
	{
	    AutoTestMode=0xAA;
	   if( appCurrStateGet() != APP_STATE_VIDEO_VIEW)
	   {
		    osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
	   }
	    osMsgPost(SP1K_MSG_KEY_PRESS_S2);
	} 
	else if (strncmp((UINT8*)arg[0], "stop", 4) == 0) 
	{
	    AutoTestMode=0;
	    osMsgPost(SP1K_MSG_KEY_PRESS_S2);
	} 
	else if (strncmp((UINT8*)arg[0], "aehi", 4) == 0) 
	{
	    G_aeSpeed = 0;
	} 
	else if (strncmp((UINT8*)arg[0], "aelo", 4) == 0) 
	{
	    G_aeSpeed = 1;
	}

}

/**
 * @fn        UINT32 cmdTestMain(UINT8* ptr)
 * @brief     cmdTestMain
 * @param     [in] ptr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-8-11
 * @todo      N/A
 * @bug       N/A
*/

#if 0
void
cmdTestMain(
	UINT8* ptr
)
{
    UINT32 arg[8] = {0};

    /* get the arguments */
    cmdArgGet(&ptr, arg, 8, 1);

    if (strncmp((UINT8*)arg[0], "cpu", 3) == 0) {
		cmdCPUSts(ptr);
    }
}

#endif
