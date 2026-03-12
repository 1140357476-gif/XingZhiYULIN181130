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
 * @file      calibration.c
 * @brief     The main file of calibration mode control
 * @author    Wen-Guo
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "front_logi.h"
#include "asicreg.h"
#include "initio.h"
#include "doslink.h"
#include "sp1k_bp_api.h"
//#include "dosusr.h"

#include "awb_api.h"//wangjg++
#include "sp1k_calibration_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_ae_api.h"
#include "cdsp_init.h"

#include "app_still_view.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"

#include "pv_task.h"
#include "hal_dram.h"
#include "cdsp_task.h"
#include "hal_front_para.h"
#include "hal_front_common.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_global.h"
#include "snap_core.h" 
#include "utility.h"
#include "dcf_api.h"
#include "ae_api.h"
#include "front_logi.h"
#include "dbg_def.h"
#include "sp1k_disp_api.h"
#include "rsc_id.h"
#include "knl_func_opt.h"
#include "reg_def.h"
#include "ae_def.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define CALIBRATION_RAW_CROP_X_SIZE (UINT16)256
#define CALIBRATION_RAW_CROP_Y_SIZE (UINT16)256

//4 //fan.xiao modified 2011-04-07 , 14:07:03
#define RAW_ROI_START ((float)5/(float)11)
#define RAW_ROI_END ((float)6/(float)11)
//4 //fan.xiao modified 2011-04-07 , 14:07:04

#define CAL_BP_BAD_BUF K_SDRAM_CommonEndAddr
#define CAL_BP_RAW_BUF (CAL_BP_BAD_BUF + K_SDRAM_BadPixelCoordBufSize)

#define PV_OB_BLOCK_WIDTH 80
#define PV_OB_BLOCK_HEIGHT 64
#define PV_OB_STRIP_DMA_WIDTH   PV_OB_BLOCK_WIDTH
#define PV_OB_STRIP_DMA_HEIGHT 8

#define ABS(x,y)  (((x)>(y)) ? ((x)-(y)) : ((y)-(x))) 

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define __FILE	__FILE_ID_CALIBRATION__


/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
static UINT8 calibrationMode = 0;
static UINT8 calibrationResult = 1;

xdata UINT8 calibrationAEOn = 1;
xdata UINT8 calibrationAWBOn = 1;

xdata UINT16 calibrationLuma[ 128 ];

xdata UINT8 calibrationObData[ SP1K_CALIBRATION_OB_MAX ];
xdata UINT8 calibrationAwbData[ SP1K_CALIBRATION_AWB_MAX ];
xdata UINT8 calibrationPvObData[ SP1K_CALIBRATION_PV_OB_MAX ];
xdata UINT32 calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_MAX ];
xdata UINT8 calibrationEv10;

static UINT32 calibrationRaw0 = 0;
static UINT32 calibrationRaw1 = 0;
static UINT32 calibrationRaw2 = 0;
static UINT32 calibrationRaw3 = 0;

static UINT16 calibrationRaw0PixSum = 0;
static UINT16 calibrationRaw1PixSum = 0;
static UINT16 calibrationRaw2PixSum = 0;
static UINT16 calibrationRaw3PixSum = 0;

static UINT8 calibrationRawRAvg = 0;
static UINT8 calibrationRawGrAvg = 0;
static UINT8 calibrationRawBAvg = 0;
static UINT8 calibrationRawGbAvg = 0;

static UINT16 calibrationPvRawCnt = 0;
static UINT16 calibrationPvYuvCnt = 0;

#ifdef PREVIEW_RAW_SAVE_MODE
static UINT32 prvRawAddr = K_SDRAM_CommonEndAddr;
#endif

static UINT8 xdata calLogFile[13];

static UINT16 calPvOBGainIndex = 0;
static UINT16 calPvOBInitExp = 300;
static UINT8 calPvOBOnePoint = 0;
calPvOBData_t calPvOBAvgData[9];

static UINT8 calPvObRawSave = 0;

static UINT8 calPvOBMulti = 0 ;

extern  xdata   UINT8   G_ucStorData[];
UINT8 pvObBpBlockThr,pvObBpWhiteThr,pvObBpThr = 100;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 G_AWBEN;
//extern appPvZoom_t xdata PvZoomCtrl ;   // will : declare here
extern xdata UINT8 G_GAIN;
extern xdata UINT32 G_SHUTTER;
extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;

extern SINT16 gId;   //will : declare here  temporarily
extern UINT8 aeGainExpSetEn;

#if OB_CAL_EN
extern UINT8 aeCmdTest ;
extern UINT8 aeCalPvOB;
extern UINT8 aeMoeNew;
extern UINT8 g_AE_Stable;
#endif

extern UINT8 aeProc;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/

UINT8 calibrationRaw( UINT32 addr, UINT16 xSize, UINT16 ySize );
UINT8 calibrationPvRaw( void );
void calibrationObGammeLoad( void );

//extern void HAL_FrontAeGet( UINT16 idx, UINT8 opt, UINT32 *shutter, UINT16 *gain );
extern UINT8 aeFlowGetCurLuma( void );

/**************************************************************************
 *               S T A T I C   L O C A L   F U N C T I O N
 **************************************************************************/
void calibrationHalRawAddrSet(UINT32 Addr);
static UINT8 calBpProcRaw(bpMode_t mode);
static UINT8 calBpProcFind(bpMode_t mode);
void pvObRawBpCal(UINT32 addr,UINT16 xSize,UINT16 ySize);

/**************************************************************************
 *					   G L O B A L	 F U N C T I O N
 **************************************************************************/
/**
 * @fn		  void sp1kCalibrationModeSet(UINT8 mode)
 * @brief	  sp1kCalibrationModeSet
 * @param	  [in] mode
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationModeSet(
	UINT8 mode
)
{
	calibrationMode = mode;
	
	if( calibrationMode == SP1K_CALIBRATION_MODE_OB || calibrationMode == SP1K_CALIBRATION_MODE_PV_OB ) {
		//calibrationObGammeLoad();
		HAL_CdspGammaEnSet(0);
	}

	if( calibrationMode == SP1K_CALIBRATION_MODE_OFF ) {
		//sp1kCdspGammaInit( SP1K_RSC_ID_GAMMA );
		HAL_CdspGammaEnSet(1);
	}

	
}

/**
 * @fn		  void sp1kCalibrationObSet(UINT8 enable)
 * @brief	  sp1kCalibrationObSet
 * @param	  [in] enable
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationObSet(
	UINT8 enable
)
{
	HAL_CdspWBOffsetEnSet( (UINT8) ( enable & 0x01 ) );

	return;
}

/**
 * @fn		  void sp1kCalibrationGammaSet(UINT8 enable)
 * @brief	  sp1kCalibrationGammaSet
 * @param	  [in] enable
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationGammaSet(
	UINT8 enable
)
{
	XBYTE[ REG_Cdsp_Lutgammaen ] = (UINT8) ( enable & 0x01 );

	return;
}

/**
 * @fn		  void sp1kCalibrationLscSet(UINT8 enable)
 * @brief	  sp1kCalibrationLscSet
 * @param	  [in] enable
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationLscSet(
	UINT8 enable
)
{
	HAL_CdspLscEnSet(enable);
}

/**
 * @fn        void sp1kCalibrationWDRSet(UINT8 enable)
 * @brief     Enable/Disable WDR 
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-6-28
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kCalibrationWDRSet(
	UINT8 enable
)
{
	HAL_CdspWDREnSet(enable);

}

/**
 * @fn		  void sp1kCalibrationAESet(UINT8 enable)
 * @brief	  sp1kCalibrationAESet
 * @param	  [in] enable
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationAESet(
	UINT8 enable
)
{

	calibrationAEOn = enable;

	if( enable == 1 ) {
		aeModeEnableSet(1);
	}
	else {
		aeModeEnableSet(0);
	}

	return;
}


/**
 * @fn		  void sp1kCalibrationAWBSet(UINT8 enable)
 * @brief	  sp1kCalibrationAWBSet
 * @param	  [in] enable
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationAWBSet(
	UINT8 enable
)
{

	XBYTE[ REG_Cdsp_gain_en ] = enable&0x01;

}


/**
 * @fn		  void sp1kCalibrationAEConfig(UINT16 idx, UINT8 opt)
 * @brief	  sp1kCalibrationAEConfig
 * @param	  [in] idx
 * @param	  [in] opt
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationAEConfig(
	UINT16 idx,
	UINT8 opt
)
{
	UINT32 shutter;
	UINT16 gain;

	HAL_FrontAeGet( idx, opt, &shutter, &gain );

	G_SHUTTER = shutter;
	G_GAIN = gain;

	G_AEReady = AE_READY;
	aeGainExpSetEn = 1;

	while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH ) );

	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 2);
	aeGainExpSetEn = 0;
}

/**
 * @fn		  UINT8 sp1kCalibrationRawCal(UINT32 addr, UINT16 xSize, UINT16 ySize)
 * @brief	  sp1kCalibrationRawCal
 * @param	  [in] addr
 * @param	  [in] hSize
 * @param	  [in] vSize
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationRawCal(
	UINT32 addr,
	UINT16 hSize,
	UINT16 vSize
)
{
	UINT16 avg;

	if( calibrationMode == SP1K_CALIBRATION_MODE_OFF ) {
		return 0;
	}
	else {
		#if OB_CAL_EN
		if(calibrationMode == SP1K_CALIBRATION_MODE_PV_OB){
			pvObRawBpCal( addr, hSize, vSize );
		}else
		#endif
		{
			calibrationRaw( addr, hSize, vSize );
		}
	}

	if( calibrationMode == SP1K_CALIBRATION_MODE_OB ) {
		avg = ( calibrationRawRAvg + calibrationRawGrAvg + calibrationRawBAvg + calibrationRawGbAvg ) >> 2;

		if( avg < (UINT16)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_OB_LOW_THD ] ||
			avg > (UINT16)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_OB_HIGH_THD ] ) {
			calibrationResult = 0;
		}
		else {
			calibrationResult = 1;
		}

		calibrationObData[ SP1K_CALIBRATION_OB_R ] = calibrationRawRAvg;
		calibrationObData[ SP1K_CALIBRATION_OB_Gr ] = calibrationRawGrAvg;
		calibrationObData[ SP1K_CALIBRATION_OB_B ] = calibrationRawBAvg;
		calibrationObData[ SP1K_CALIBRATION_OB_Gb ] = calibrationRawGbAvg;
	}
	else if( calibrationMode == SP1K_CALIBRATION_MODE_AWB ) {
		calibrationAwbData[ SP1K_CALIBRATION_AWB_R ] = (UINT32)calibrationRawRAvg;
		calibrationAwbData[ SP1K_CALIBRATION_AWB_Gr ] = (UINT32)calibrationRawGrAvg;
		calibrationAwbData[ SP1K_CALIBRATION_AWB_B ] = (UINT32)calibrationRawBAvg;
		calibrationAwbData[ SP1K_CALIBRATION_AWB_Gb ] = (UINT32)calibrationRawGbAvg;
	}
	else if( calibrationMode == SP1K_CALIBRATION_MODE_PV_OB ) {
		calibrationPvObData[ SP1K_CALIBRATION_PV_OB_R ] = calibrationRawRAvg;
		calibrationPvObData[ SP1K_CALIBRATION_PV_OB_Gr ] = calibrationRawGrAvg;
		calibrationPvObData[ SP1K_CALIBRATION_PV_OB_B ] = calibrationRawBAvg;
		calibrationPvObData[ SP1K_CALIBRATION_PV_OB_Gb ] = calibrationRawGbAvg;
	}

	return 0;
}
#if OB_CAL_EN
void
sp1kCalPvOBGainShutterSet(
	UINT16 gain,
	UINT32 shutter,
	UINT8 wait
)
{
	G_SHUTTER = shutter;
	G_GAIN = gain;

	G_AEReady = AE_READY;
	aeGainExpSetEn = 1;
	aeMoeNew = 1;

	dbgWdtFeed(1000);
	while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH ) );
	dbgWdtFeed(1000);
	//while(aeMoeNew == 1);   //->0
	//while(aeMoeNew == 0);   //->1
	if(wait){
		//HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, wait);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VVALID, 1, wait);
	}
	
//	aeGainExpSetEn = 0;
}

calPvOBData_t *sp1kCalPvOBAvgDataGet(void)
{
	return calPvOBAvgData;
}

void sp1kCalPvOBParaGet(UINT8 infoId,UINT16 *pPara1,UINT16 *pPara2)
{
	UINT16 para1,para2;
	switch(infoId)
	{
		case  SP1K_CAL_PV_OB_GAIN_INDEX:
			para1 = calPvOBGainIndex;
			break;
		case SP1K_CAL_PV_OB_INIT_EXP:
			para1 = calPvOBInitExp;
			break;
		case  SP1K_CAL_PV_OB_LINE_TOL:
			frontDeviceInfoGet( 0, FRONT_DEV_INFO_PREVIEW_LINE_TOL, &para1, NULL);
			break;
		case SP1K_CAL_PV_OB_PCLK:
			frontDeviceInfoGet( 0, FRONT_DEV_INFO_PREVIEW_PCLK, &para1, &para2);
			break;		
		case SP1K_CAL_PV_OB_ONE_POINT_CAL:
			para1 = calPvOBOnePoint;
			break;
	}
	if ( pPara1 != NULL ) {
		*pPara1 = para1;
	}
	if ( pPara2 != NULL ) {
		*pPara2 = para2;
	}	
}

void sp1kCalPvOBParaSet(UINT8 infoId,UINT16 para)
{
	switch(infoId)
	{
		case  SP1K_CAL_PV_OB_GAIN_INDEX:
			calPvOBGainIndex = para;
			break;
		case SP1K_CAL_PV_OB_INIT_EXP:
			calPvOBInitExp = para;
			break;
		case SP1K_CAL_PV_OB_SAVE_RAW:
			if(para != 0){
				calPvObRawSave = 1;
			}
			else{
				calPvObRawSave = 0;
			}
			calibrationPvRawCnt = 0;
			break;			
		case SP1K_CAL_PV_OB_MULTI_CAL:
			calPvOBMulti = para;	
			break;
		case SP1K_CAL_PV_OB_ONE_POINT_CAL:
			calPvOBOnePoint = para;	
			break;
		case SP1K_CAL_PV_OB_BAD_PIXEL_THR:
			pvObBpThr = (UINT8)para;
			break;
	}
}

UINT8 sp1kCalPvOBResultWrite(UINT8* result , UINT16 resId , UINT16 resOff)
{
	UINT8 sts;

	resId = resId;
	resOff = resOff;
	
	if(calPvOBMulti == 0){
		do {
			sts = sp1kNandRsvRead(SP1K_RSC_ID_CDSPPAR, (K_SDRAM_CommonEndAddr<< 1));
		}while(!sts);		
		M_DRAM_WaitReady;
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+7), 0);
		M_DRAM_WriteWord(result[1],0x00);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+9), 0);
		M_DRAM_WriteWord(result[0],0xFC);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+11), 0);
		M_DRAM_WriteWord(result[3],0x00);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+13), 0);
		M_DRAM_WriteWord(result[2],0xFC);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+15), 0);
		M_DRAM_WriteWord(result[5],0x00);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+17), 0);
		M_DRAM_WriteWord(result[4],0xFC);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+19), 0);
		M_DRAM_WriteWord(result[7],0x00);
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+21), 0);
		M_DRAM_WriteWord(result[6],0xFC);
		sp1kNandRsvWrite(SP1K_RSC_ID_CDSPPAR, (K_SDRAM_CommonEndAddr<< 1));
	}
	else{/*Muti cal*/
		do {
			sts = sp1kNandRsvRead(resId, (K_SDRAM_CommonEndAddr<< 1));
		}while(!sts);			
		M_DRAM_WaitReady;
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr + resOff), 0);
		M_DRAM_WriteWord((UINT8)(calPvOBGainIndex>>8),(UINT8)(calPvOBGainIndex&0xff));
		M_DRAM_WriteWord(result[0],result[1]);
		M_DRAM_WriteWord(result[2],result[3]);
		M_DRAM_WriteWord(result[4],result[5]);
		M_DRAM_WriteWord(result[6],result[7]);
		
		//dramFileSave("CAL00000BIN",K_SDRAM_CommonEndAddr,(UINT32)412);
		sp1kNandRsvWrite(resId, (K_SDRAM_CommonEndAddr<< 1));

	}
	return 1;
}

UINT8 sp1kCalPvOBResultGet(UINT8* result,UINT8 mode)  /*mode = 0 : 8bits RAW , 1 : 10bits RAW*/
{
	float a,b,rgbData[18];
	UINT8 j,i,tmpLow,tmpHigh;
	UINT16 pvOBPara[9*5];

	if(calPvOBOnePoint == 0){ /* 9 points OB Cal */ 
		for(i = 0 ; i < 4 ; i++){
			for( j = 0 ; j < 9 ; j++){
				if(calPvOBAvgData[j].rAvg == 0 || calPvOBAvgData[j].grAvg == 0 
					||calPvOBAvgData[j].bAvg == 0 || calPvOBAvgData[j].gbAvg == 0){
					printf("ERROR!!AVG = 0\n");
					return 0;
				}
				switch( i ){
					case 0: 
						rgbData[j*2+1] = calPvOBAvgData[j].rAvg;
						//printf("R = %f\n",rgbData[j*2+1]);
						break;  	/*R*/
					case 1:
						rgbData[j*2+1] = calPvOBAvgData[j].grAvg;
						//printf("Gr = %f\n",rgbData[j*2+1]);					
						break; 	/*Gr*/
					case 2: 
						rgbData[j*2+1] = calPvOBAvgData[j].bAvg;
						//printf("B = %f\n",rgbData[j*2+1]);					
						break;	/*B*/
					case 3: 
						rgbData[j*2+1] = calPvOBAvgData[j].gbAvg;
						//printf("Gb = %f\n",rgbData[j*2+1]);					
						break;	/*Gb*/
				}
				rgbData[j*2] = calPvOBAvgData[j].expLine;
				pvOBPara[i*9+j] = rgbData[j*2+1];
			}
			if(sp1kCalPvOBLinearRegression((float *)rgbData, 9, &a, &b) != 1){
				return 0;
			}
			else{
				if(mode == 0){
					a = a*(float)4;
				}
				if(a > 0){
					if((UINT8)(a + 0.5) == 0){
						tmpLow = 0x00;
						tmpHigh = 0x00;
					}
					else{
						tmpLow = 0xff - (UINT8)(a + 0.5) + 1;//0xff - (int)((a>0)?(a+0.5):(a-0.5))*4;
						tmpHigh = 0x03;
					}
				}
				else{
					tmpLow = (UINT8)(0 - (a - 0.5));//-(int)((a>0)?(a+0.5):(a-0.5))*4;
					tmpHigh = 0x00;
				}
				result[i*2] = tmpHigh;
				result[i*2+1] = tmpLow;
			}
		}
	}
	else{/* 1 points OB Cal */ 
		float avgBuf[4];
		if(mode == 0){  /*8Bits*/
			avgBuf[0] = calPvOBAvgData[0].rAvg*4;  
			avgBuf[1] = calPvOBAvgData[0].grAvg*4;  
			avgBuf[2] = calPvOBAvgData[0].bAvg*4;  
			avgBuf[3] = calPvOBAvgData[0].gbAvg*4;  
		}
		else{ 	/*10Bits*/
			avgBuf[0] = calPvOBAvgData[0].rAvg;  
			avgBuf[1] = calPvOBAvgData[0].grAvg;  
			avgBuf[2] = calPvOBAvgData[0].bAvg;  
			avgBuf[3] = calPvOBAvgData[0].gbAvg;  
		}
		for(i = 0 ; i < 4 ; i++){
			a = avgBuf[i];
			if(a > 0){
				if((UINT8)(a + 0.5) == 0){
					tmpLow = 0x00;
					tmpHigh = 0x00;
				}
				else{
					tmpLow = 0xff - (UINT8)(a + 0.5) + 1;//0xff - (int)((a>0)?(a+0.5):(a-0.5))*4;
					tmpHigh = 0x03;
				}
			}
			else{
				tmpLow = (UINT8)(0 - (a - 0.5));//-(int)((a>0)?(a+0.5):(a-0.5))*4;
				tmpHigh = 0x00;
			}
			result[i*2] = tmpHigh;
			result[i*2+1] = tmpLow;
		}
	}
	/*SAVE 10Bit INFO*/
	if(mode == 1 && calPvObRawSave ==1){
		UINT8 f[ 13 ] = "PVOB0000BIN";
		static UINT8 obCalCnt = 0;
		UINT32 addr = K_SDRAM_CommonEndAddr;

		f[ 5 ] = '0' + obCalCnt / 100;
		f[ 6 ] = '0' + obCalCnt / 10;
		f[ 7 ] = '0' + obCalCnt % 10;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;
		
		for( i = 0 ; i < 9 ; i++){
			pvOBPara[36+i] = calPvOBAvgData[i].expLine;   //Exposure Lines
		}
		/*for( i = 0 ; i < 45 ; i++){    
			printf("pvOBPara[%bd] = %u\n",i,pvOBPara[i]);
		}*/
		if(HAL_DramDmaWrite(pvOBPara, addr, 90, 1) == FAIL){
			//printf("Save PV OB para ERR!");
		}
		else{
			dramFileSave(f,addr,90);
		}
		obCalCnt ++ ;
	}
	return 1;
}

UINT8 sp1kCalPvOBLinearRegression(float *rgbData, UINT8 rows, float *a, float *b)
{
	UINT8 m;
	float *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
	if (rgbData == 0 || a == 0 || b == 0 || rows < 1)
		return 0;
	for (p = rgbData, m = 0; m < rows; m ++)
	{
		xa += *p ++;
		ya += *p ++;
	}
	xa /= rows;                                     
	ya /= rows;                                     
	for (p = rgbData, m = 0; m < rows; m ++, p += 2)
	{
		Lxx += ((*p - xa) * (*p - xa));             
		Lxy += ((*p - xa) * (*(p + 1) - ya));       
	}
	*b = Lxy / Lxx;                                 // b = Lxy / Lxx
	*a = ya - *b * xa;                              // a = Ya - b*Xa
	return 1;
}

void sp1kCalPvOBAEWinRGBValueGet(UINT8 mode)
{
	UINT8 i,lowByte,highByte,iniFlag;
	UINT32 R,Gr,B,Gb;
	float rAvg,grAvg,bAvg,gbAvg;
	mode = mode;
	iniFlag = 1; 
	R = Gr = B= Gb =0;
	rAvg = grAvg = bAvg = gbAvg = 0;
	while(aeMoeNew);
	while(aeMoeNew == 0 &&  iniFlag){
		i = 25;
		M_DRAM_WaitReady;
		M_DRAM_SetStartAddress(K_SDRAM_PvAeWndAddr, 1);
		do{
			M_DRAM_ReadWord(lowByte,highByte);
			R += ((UINT16)highByte<<8) + (UINT16)lowByte;	
			M_DRAM_ReadWord(lowByte,highByte);
			Gr += ((UINT16)highByte<<8) + (UINT16)lowByte;	
			M_DRAM_ReadWord(lowByte,highByte);
			B += ((UINT16)highByte<<8) + (UINT16)lowByte;	
			M_DRAM_ReadWord(lowByte,highByte);
			Gb += ((UINT16)highByte<<8) + (UINT16)lowByte;					
		}while(--i);		
		iniFlag = 0;
	}
	rAvg = (float)R/(float)25;
	grAvg = (float)Gr/(float)25;
	bAvg = (float)B/(float)25;
	gbAvg = (float)Gb/(float)25;
	calPvOBAvgData[mode].rAvg = rAvg;
	calPvOBAvgData[mode].grAvg = grAvg;
	calPvOBAvgData[mode].bAvg = bAvg;
	calPvOBAvgData[mode].gbAvg = gbAvg;
	//printf("rAvg1 = %f , grAvg1 = %f , bAvg = %f , gbAvg = %f\n",rAvg,grAvg,bAvg,gbAvg);
}

void sp1kCalPvOBAEWinConfigSet(UINT8 mode)
{
#if 1
	UINT16 aeHSize,aeVSize;
	UINT16 aeHOffset,aeVOffset;
	UINT8 AEhaccfactor,AEvaccfactor,divNum, distanceH, distanceV;
	UINT16 hSize, vSize;
	UINT16 cropHsize, cropVsize; 
	static UINT8 aeProcTmp,first = 1;
	
	/*OB LSC AWB Gamma turning off*/
	sp1kCalibrationObSet(0);
	sp1kCalibrationLscSet(0);
	sp1kCalibrationAWBSet(0);
	sp1kCalibrationGammaSet(0);
	HAL_CdspBPEnSet(0);
	
	sp1kAeSetModeEn(AE_DISABLE);	/* disable AE */
	if(first == 1){
		aeProcTmp = aeProc;
		first = 0;
	}

	frontPreviewSizeGet(0, &hSize, &vSize); 
	cropHsize = hSize;
	cropVsize = vSize;
	
	if(mode == 0){/**/
	#if (USE_RGB_WIN == 0)
		aeCmdTest = 1;
		aeCalPvOB = 1;

		aeProc = 0;//ae window
		aeWinModeSet(AE_WIN_MODE_RGB);		/* ae mode set 0:y window, 1:RGB window */
		aeCalculateLumaModeSet(LUMA_CAL_LSC);		/* set ae get luma mode  */
	
		sp1kPreviewCropSizeSet(cropHsize,cropVsize);
		//printf("cropHsize=%d, cropVsize=%d\n",cropHsize,cropVsize);
		/* all region, 5*5 block number, target lima 40 */	

		divNum = 5; /* RGB Window 5*5 */
		aeHSize = ((UINT16)(cropHsize*RAW_ROI_END) - (UINT16)(cropHsize*RAW_ROI_START));
		aeVSize = ((UINT16)(cropVsize*RAW_ROI_END) - (UINT16)(cropVsize*RAW_ROI_START));
		aeHSize = aeHSize/divNum;
		aeVSize = aeVSize/divNum;
		distanceH = 2;	/* h interval 2 */
		distanceV = 2;	/* v interval 2 */

		/* H,V offset set */
		aeHOffset  =  (cropHsize - aeHSize*divNum - distanceH*(divNum-1)) >> 1 ;
		aeVOffset  =  (cropVsize - aeVSize*divNum - distanceV*(divNum-1)) >> 1;			
			
		if (aeHSize > 128)		AEhaccfactor =7;
		else if (aeHSize> 64)	AEhaccfactor =6;
		else if (aeHSize > 32)	AEhaccfactor =5;
		else if (aeHSize > 16)	AEhaccfactor =4;
		else if (aeHSize > 8)	AEhaccfactor =3;
		else if (aeHSize > 4)	AEhaccfactor =2;
		else if (aeHSize > 2)	AEhaccfactor =1;
		else if (aeHSize >= 0)	AEhaccfactor =0;
		if (aeVSize > 128)		AEvaccfactor =7;
		else if (aeVSize > 64)	AEvaccfactor =6;
		else if (aeVSize > 32)	AEvaccfactor =5;
		else if (aeVSize > 16)	AEvaccfactor =4;
		else if (aeVSize > 8)	AEvaccfactor =3;
		else if (aeVSize > 4)	AEvaccfactor =2;
		else if (aeVSize > 2)	AEvaccfactor =1;
		else if (aeVSize >= 0)	AEvaccfactor =0;
			
		/* AE_WIN_MODE_RGB, RGB Window */
		HAL_CdspAEwinoffset   ( aeHOffset, aeVOffset);
		HAL_CdspAEwinsizeset  ( aeHSize, aeVSize);
		HAL_CdspAEwinfactorset( AEhaccfactor, AEvaccfactor );
		HAL_CdspAEwinintvlset( distanceH, distanceV );		/* default to 0 */
		HAL_CdspAEwinCntset(divNum, divNum);			/* ae window cnt */
		HAL_CdspAEwinAddrset( K_SDRAM_PvAeWndAddr );
		sp1kAeSetModeEn(AE_ENABLE); /* enable AE */
		HAL_CdspAEWinTest(1);				//ae test enable

	#else
		aeCmdTest = 1;
		aeCalPvOB = 1;	
		aeProc = 2;//RGB window
		//sp1kAeInit();
		{/*sp1kAeInit();*/
			aeInit(aeProc);
			aeWinModeSet(AE_WIN_MODE_RGB);
			aeCalculateLumaModeSet(LUMA_NORMAL);
			sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 120);//120
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
			sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
			//sp1kAeParamSet(SP1K_AE_USING_AWB_GAIN, 0);
			g_AE_Stable = 0;	/* Clear AE stable flag */
		}
		{ /*aeWinConfigSet()*/ 
			UINT8 aeWinMode;
			aeWinMode = HAL_CdspAEWinModeGet();
			/* windows matrix set */
			switch (aeWinMode) {
				case AE_WIN_MODE_RGB:
					divNum = 5; /* RGB Window 5*5 */
					distanceH = 2;	/* h interval 2 */
					distanceV = 2;	/* v interval 2 */
					break;
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}

			/* H,V windows size set */
			aeHSize = ((UINT16)(cropHsize*RAW_ROI_END) - (UINT16)(cropHsize*RAW_ROI_START));
			aeVSize = ((UINT16)(cropVsize*RAW_ROI_END) - (UINT16)(cropVsize*RAW_ROI_START));	

			aeHSize = aeHSize/divNum;
			aeVSize = aeVSize/divNum;	

			if (aeHSize >= 128)		aeHSize = 128;
			else if (aeHSize > 64)	aeHSize = 64;
			else if (aeHSize > 32)	aeHSize = 32;
			else if (aeHSize > 16)	aeHSize = 16;
			else if (aeHSize > 8)	aeHSize = 8;
			else if (aeHSize > 4)	aeHSize = 4;
			else if (aeHSize > 2)	aeHSize = 2;
			else if (aeHSize >= 0)	aeHSize = 0;
			if (aeVSize >= 128)		aeVSize = 128;
			else if (aeVSize > 64)	aeVSize = 64;
			else if (aeVSize > 32)	aeVSize = 32;
			else if (aeVSize > 16)	aeVSize = 16;
			else if (aeVSize > 8)	aeVSize = 8;
			else if (aeVSize > 4)	aeVSize = 4;
			else if (aeVSize > 2)	aeVSize = 2;
			else if (aeVSize >= 0)	aeVSize = 0;

			/* H,V offset set */
			aeHOffset  =  (cropHsize -aeHSize*divNum -distanceH*(divNum-1)) >> 1 ;
			aeVOffset  =  (cropVsize - aeVSize*divNum - distanceV*(divNum-1)) >> 1;	
		
			/* H,V normalizing factor set */
			if (aeHSize > 128)		AEhaccfactor = 7;
			else if (aeHSize> 64)	AEhaccfactor = 6;
			else if (aeHSize > 32)	AEhaccfactor = 5;
			else if (aeHSize > 16)	AEhaccfactor = 4;
			else if (aeHSize > 8)	AEhaccfactor = 3;
			else if (aeHSize > 4)	AEhaccfactor = 2;
			else if (aeHSize > 2)	AEhaccfactor = 1;
			else if (aeHSize >= 0)	AEhaccfactor = 0;
			if (aeVSize > 128)		AEvaccfactor = 7;
			else if (aeVSize > 64)	AEvaccfactor = 6;
			else if (aeVSize > 32)	AEvaccfactor = 5;
			else if (aeVSize > 16)	AEvaccfactor = 4;
			else if (aeVSize > 8)	AEvaccfactor = 3;
			else if (aeVSize > 4)	AEvaccfactor = 2;
			else if (aeVSize > 2)	AEvaccfactor = 1;
			else if (aeVSize >= 0)	AEvaccfactor = 0;
			
			if (aeWinMode == AE_WIN_MODE_RGB) {
				AEhaccfactor = AEhaccfactor-1;
				AEvaccfactor = AEvaccfactor-1;
			}
			/* all reg set */
			HAL_CdspAEwinoffset( aeHOffset, aeVOffset); 			/* H,V offset set */
			HAL_CdspAEwinsizeset( aeHSize, aeVSize );				/* H,V windows size set */
			HAL_CdspAEwinfactorset( AEhaccfactor, AEvaccfactor );	/* H,V normalizing factor set */
			HAL_CdspAEwinintvlset( distanceH, distanceV );			/* set ae window interval */
			HAL_CdspAEwinCntset(divNum, divNum);					/* set ae window cnt */
			HAL_CdspAEwinAddrset( K_SDRAM_PvAeWndAddr );			/* set ae output address */
			aeNormalizationFactorSet(aeWinMode);
			XBYTE[REG_Cdsp_pSkipwr] &= (~0x04); //don't skip writing AE data to DRAM	disable
		}
		sp1kAeSetModeEn(AE_ENABLE); /* enable AE */
		HAL_CdspAEWinTest(1);				//ae test enable
		
	#endif
	#if 0
		printf("======luma mode=====\n");
		printf("hSize=%d, vSize=%d\n",hSize,vSize);
		printf("aeHSize=%d, aeVSize=%d\n",aeHSize,aeVSize);
		printf("aeHOffset=%d, aeVOffset=%d\n",aeHOffset,aeVOffset);
		printf("AEhaccfactor=%bu, AEvaccfactor=%bu\n",AEhaccfactor,AEvaccfactor);
		printf("AEwinHintvl=2, AEwinVintvl=2\n");
		printf("divNum=%bu\n",divNum);
	#endif				
	}
	else if(mode == 1){
		/*OB LSC AWB Gamma turning on*/
		aeProc = aeProcTmp;  /*restore aeProc*/
		aeCmdTest = 0;
		aeCalPvOB = 0;		
		/* AE initial */
		sp1kAeInit();
		sp1kCalibrationObSet(1);
		sp1kCalibrationLscSet(1);
		sp1kCalibrationAWBSet(1);
		sp1kCalibrationGammaSet(1);
		HAL_CdspBPEnSet(1);		
		sp1kAeSetModeEn(AE_ENABLE);
		first = 1;
	}
#endif		
}
#endif
void
sp1kCalibrationOBDataGet(
	UINT8 *ob
)
{
	*ob	   =calibrationObData[SP1K_CALIBRATION_OB_R];
	*(ob+1)=calibrationObData[SP1K_CALIBRATION_OB_Gr];
	*(ob+2)=calibrationObData[SP1K_CALIBRATION_OB_B];
	*(ob+3)=calibrationObData[SP1K_CALIBRATION_OB_Gb];
}


/**
 * @fn		  UINT8 sp1kCalibrationPvRaw(void)
 * @brief	  sp1kCalibrationPvRaw
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationPvRaw(
	void
)
{
	return calibrationPvRaw();
}

/**
 * @fn		  void sp1kCalibrationAETest(UINT16 fromIdx, UINT16 toIdx, UINT8 opt)
 * @brief	  sp1kCalibrationAETest
 * @param	  [in] fromIdx
 * @param	  [in] toIdx
 * @param	  [in] opt
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationAETest(
	UINT16 fromIdx,
	UINT16 toIdx,
	UINT8 opt
)
{

	UINT16 i;
	//UINT32 shutter;
	//UINT16 gain;
	//UINT8 val;
	UINT8 aeCurMode;
	//UINT8 y[64];
	//UINT16 luma;

	aeCurMode = sp1kAeGetModeEn();
	
	sp1kAeInit();
	sp1kAeSetModeEn(AE_DISABLE);

	HAL_CdspAEWinEnSet(1, 1, 0);		//ae enable, ae holdsvd enable, ae sizezf disable
	HAL_CdspAEWinHoldSet(0);			//ae hold disable
	HAL_CdspAEWinTest(0);				//ae test disable	

	HAL_CdspVDramSkipWrSet(CDSP_SKIP_AE,0);//don't skip writing AE data to DRAM	disable
	HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	//while(XBYTE[REG_Cdsp_pSkipwr]&0x04);

	G_AEReady = AE_NOT_READY;

	HAL_GlobalWDTSet(0);

	//printf("[from Idx] = %d [to Idx] = %d\n", fromIdx, toIdx);
	
	for( i = fromIdx; i <= toIdx; i++ ) {

		#if 0
		//HAL_FrontAeGet( i, opt, &shutter, &gain );

		//G_SHUTTER = shutter;
		//G_GAIN = gain;
		//G_AEReady = 1;

		//while( !( G_AEReady == 0 && G_AESetting == 0 ) );

		//frontExposureTimeSet(0,G_SHUTTER,1,0);
		//frontGainSet(0,G_GAIN,1,0);
		#else
		sp1kCalibrationAEConfig(i, opt);
		#endif

		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);

		#if 0
		HAL_ReadAEWindow(&y);

		// get luma
		calibrationLuma[ i ] = luma = 0;

		for( j = 0; j < 64; j++ ) {
		luma += y[ j ];
		}

		calibrationLuma[ i ] = luma;
		#else
		calibrationLuma[ i ] = aeFlowGetCurLuma();
		//printf("idx=%d,luma=0x%x\n",i,calibrationLuma[ i ]);
		#endif
		
		//printf( "gid %d, luma %d\n", i, calibrationLuma[ i ] );
	}

	HAL_GlobalWDTSet(1);

	sp1kAeSetModeEn(aeCurMode);

	return;
}

/**
 * @fn		  void sp1kCalibrationAETestSave(UINT16 fromIdx, UINT16 toIdx)
 * @brief	  sp1kCalibrationAETestSave
 * @param	  [in] fromIdx
 * @param	  [in] toIdx
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationAETestSave(
	UINT16 fromIdx,
	UINT16 toIdx
)
{
	UINT32 addr = K_SDRAM_CommonEndAddr;
	UINT16 i, value;
	UINT8 t0, t1;
	UINT8 f[ 13 ] = "AE_LUMA0DAT";

	//printf( "=====AE Gid and Luma=====\n" );
	for( i = fromIdx; i <= toIdx; i++ ) {
		//printf( "%d, %d\n", i, calibrationLuma[ i ] );
	}

	M_DRAM_SetStartAddress((UINT32)addr, 0);

	for( i = fromIdx; i < toIdx; i++ ) {
		t0 = *( (UINT8 *)&i + 1 );
		t1 = *( (UINT8 *)&i );

		M_DRAM_WriteWord(t0, t1);

		value = calibrationLuma[ i ];
		t0 = *( (UINT8 *)&value + 1 );
		t1 = *( (UINT8 *)&value );

		M_DRAM_WriteWord(t0, t1);
	}

	f[ 11 ] = 0x20;
	f[ 12 ] = 0x00;

	dramFileSave( f, addr, ( toIdx - fromIdx + 1 ) << 2 );

	return;
}


#if TAG_TODO//will mark
/**
 * @fn		  UINT8 sp1kCalibrationLoadCriteria(UINT8 id)
 * @brief	  sp1kCalibrationLoadCriteria
 * @param	  [in] id
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationLoadCriteria(
	UINT8 id
)
{
	UINT8 sts;

	do {
		sts = sp1kNandRsvRead( id, ( K_SDRAM_CommonEndAddr << 1 ) );
	}while(!sts);

	criteriaDataRead( K_SDRAM_CommonEndAddr );

	return 0;
}
#endif


/**
 * @fn		  UINT8 sp1kCalibrationAeTestGidStepGet(UINT8 *idx0, UINT8 *idx1, UINT8 *idx2, UINT8 *idx3)
 * @brief	  sp1kCalibrationAeTestGidStepGet
 * @param	  [in] idx0
 * @param	  [in] idx1
 * @param	  [in] idx2
 * @param	  [in] idx3
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationAeTestGidStepGet(
	UINT8 *idx0,
	UINT8 *idx1,
	UINT8 *idx2,
	UINT8 *idx3
)
{
	*idx0 = (UINT8)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_AE_GID0 ];
	*idx1 = (UINT8)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_AE_GID1 ];
	*idx2 = (UINT8)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_AE_GID2 ];
	*idx3 = (UINT8)calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_AE_GID3 ];

	return 0;
}

#if TAG_TODO
/**
 * @fn		  UINT8 sp1kCalibrationLoadIq(UINT8 id)
 * @brief	  sp1kCalibrationLoadIq
 * @param	  [in] id
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationLoadIq(
	UINT8 id
)
{
	UINT8 sts;

	do {
		sts = sp1kNandRsvRead( id, ( K_SDRAM_CommonEndAddr << 1 ) );
	}while(!sts);


	ReadIqData( K_SDRAM_CommonEndAddr );

	return 0;
}
#endif

#if TAG_TODO //will mark
/**
 * @fn		  UINT8 sp1kCalibrationSaveIq(UINT8 id)
 * @brief	  sp1kCalibrationSaveIq
 * @param	  [in] id
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationSaveIq(
	UINT8 id
)
{
	UINT8 sts;

	iqDataWrite( id, K_SDRAM_CommonEndAddr );

	do {
		sts = sp1kNandRsvWrite( id, ( K_SDRAM_CommonEndAddr << 1 ) );
	}while(!sts);
	return 0;
}
#endif


/**
 * @fn		  void sp1kCalibrationDefaultSizeSet(void)
 * @brief	  sp1kCalibrationDefaultSizeSet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationDefaultSizeSet(
	void
)
{
	UINT16 snapW, snapH;


	frontSnapSizeGet(0, &snapW, &snapH);
	sp1kSnapSizeSet(snapW, snapH);

	return;
}

/**
 * @fn		  UINT8 sp1kCalibrationPvYuv(void)
 * @brief	  sp1kCalibrationPvYuv
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalibrationPvYuv(
	void
)
{

		UINT32 addr = K_SDRAM_CommonEndAddr;
		UINT32 addrPv;
	
		UINT16 pvXSize, pvYSize;
		UINT16 frameHSize,frameVSize;
		UINT8 f[ 12 ] = "PV000000.YUV";
		UINT8 idx;
	
		pvSizeGet( &pvXSize, &pvYSize );
		
		//pvClampSizeGet(&frameHSize,&frameVSize);	//will add
		sp1kPreviewCropSizeGet(&frameHSize,&frameVSize);
	   
				
		idx = HAL_CdspWriteBufIdxGet();
		
		if( idx == 0 ) {
			addrPv = K_SDRAM_PvBufAddrD;
		}
		else if( idx == 1 ) {
			addrPv = K_SDRAM_PvBufAddrA;
		}
		else if( idx == 2 ) {
			addrPv = K_SDRAM_PvBufAddrB;
		}
		else if( idx == 3 ) {
			addrPv = K_SDRAM_PvBufAddrC;
		}
	
		//HAL_GprmDramDma( addrPv << 1, addr << 1, (UINT32)pvXSize * pvXSize * 2, 1 ); //will mark
	
		f[ 5 ] = '0' + calibrationPvYuvCnt / 100;
		f[ 6 ] = '0' + calibrationPvYuvCnt / 10;
		f[ 7 ] = '0' + calibrationPvYuvCnt % 10;
	
		//f[ 10 ] = 0x20;  //will mark
		//f[ 11 ] = 0x00;  //will mark

		//printf("preview buf address=%lx\n",addrPv);
		
		//dramFileSave( f, addr, (UINT32)pvXSize * pvYSize * 2 );//will mark
		//dramFileSave( f, addrPv, (UINT32)frameHSize * frameVSize * 2 );	//will change
		dramFileSave( f, addrPv, (UINT32)pvXSize * pvYSize * 2 );	//will change
		
		calibrationPvYuvCnt++;

		//dramFileSave( "PV00000A.YUV",K_SDRAM_Avi2PvwBufAddrA , (UINT32)320 * 240 * 2 );	//will change
		//dramFileSave( "PV00000B.YUV",K_SDRAM_Avi2PvwBufAddrB , (UINT32)320 * 240 * 2 );	//will change
		//dramFileSave( "PV00000C.YUV",K_SDRAM_Avi2PvwBufAddrC , (UINT32)320 * 240 * 2 );	//will change
		//dramFileSave( "PV00000D.YUV",K_SDRAM_Avi2PvwBufAddrD , (UINT32)320 * 240 * 2 );	//will change

		//dramFileSave( "PV00000A.YUV",K_SDRAM_AviPvwBufAddrA , (UINT32)640 * 480 * 2 );	//will change
		//dramFileSave( "PV00000B.YUV",K_SDRAM_AviPvwBufAddrB , (UINT32)640 * 480 * 2 );	//will change
		
		return 0;

}

#if OB_CAL_EN
static void pvObRawBpStripRead(UINT32 srcAddr, UINT32 dmaOff)
{
	//dma read to sram, stripW < 1024
	//r gr r gr ... 128 pixles
	//r gr r gr ...
	
	UINT8 stripH = PV_OB_STRIP_DMA_HEIGHT;
	UINT16 dstAddr = 0x0000;

	while (stripH--) {
		HAL_DmaSetSramAddr(dstAddr);
		HAL_DmaSetDramAddr(srcAddr);
		HAL_DmaDo(0, 1, PV_OB_STRIP_DMA_WIDTH, 0, 0);
		srcAddr += dmaOff;
		dstAddr += PV_OB_STRIP_DMA_WIDTH;
	}
}

static void pvObRawBpStripAvgs(float* pAvg, UINT8 cnt)
{
	UINT16 off;
	UINT8 i, j, k;
	UINT16 avg1, avg2;
	UINT8 start, width; 
	UINT8 blockSize;

	blockSize = (UINT8)PV_OB_STRIP_DMA_HEIGHT*PV_OB_STRIP_DMA_HEIGHT;

	start = 0;
	width = (UINT8)PV_OB_STRIP_DMA_WIDTH - ((UINT8)PV_OB_STRIP_DMA_HEIGHT<<1);
	
	for (k = 0; k < cnt;) {
		off = (UINT16) start;
		avg1 = 0;
		avg2 = 0;
		
		j = PV_OB_STRIP_DMA_HEIGHT;
		do {
			i = PV_OB_STRIP_DMA_HEIGHT;
			do {
				avg1 += G_ucStorData[off++];
				avg2 += G_ucStorData[off++];
			} while (--i);
			off += width;
		} while (--j);
		
		//pAvg[k++] = (UINT8)(avg1>>6);
		//pAvg[k++] = (UINT8)(avg2>>6);
		pAvg[k++] = (float)avg1/blockSize;
		pAvg[k++] = (float)avg2/blockSize;
		start += ((UINT8)PV_OB_STRIP_DMA_HEIGHT<<1);
	}
}

static void pvObRawBpStripSearch( float* pAvg,UINT8 cnt, UINT8 upOrDown)
{
	UINT16 off;
	UINT8 i, j, k;
	UINT8 val1, val2;
	UINT8 start, width;

	UINT32 *pRawSum0,*pRawSum1;
	UINT16 *pPixSum0,*pPixSum1;

	if(upOrDown == 0){	/*R Gr*/
		pRawSum0 = &calibrationRaw0;
		pPixSum0 = &calibrationRaw0PixSum;
		pRawSum1 = &calibrationRaw1;
		pPixSum1 = &calibrationRaw1PixSum;
	}
	else{	/*B Gb*/
		pRawSum0 = &calibrationRaw2;
		pPixSum0 = &calibrationRaw2PixSum;
		pRawSum1 = &calibrationRaw3;
		pPixSum1 = &calibrationRaw3PixSum;
	}
	
	start = 0;
	width = (UINT8)PV_OB_STRIP_DMA_WIDTH - ((UINT8)PV_OB_STRIP_DMA_HEIGHT<<1);
	#if 0
	if(0){
		UINT8 mm;
		for(mm = 0 ; mm < 10 ; mm ++){
			printf("%f-",pAvg[mm]);
		}
		
		printf("\n");
	}
	if(0){
		UINT16 mm;
		for(mm = 0 ; mm < 640 ; mm ++){
			printf("%bu-",G_ucStorData[mm]);
		}
		printf("\n");
	}
	#endif
	for (k = 0; k < cnt;) {
		off = (UINT16) start;
		val1 = 0;
		val2 = 0;
		
		j = PV_OB_STRIP_DMA_HEIGHT;
		do {
			i = PV_OB_STRIP_DMA_HEIGHT;
			do {
				val1 = G_ucStorData[off++];
				val2 = G_ucStorData[off++];
				//printf("%bu-%bu-",val1,val2);
				if(ABS(val1,pAvg[k]) < pvObBpThr){
					(*pRawSum0) += val1;
					(*pPixSum0)++;
				}
				if(ABS(val2,pAvg[k+1]) < pvObBpThr){
					(*pRawSum1) += val2;
					(*pPixSum1)++;
				}
			} while (--i);
			off += width;
		} while (--j);
		
		k += 2;
		start += ((UINT8)PV_OB_STRIP_DMA_HEIGHT<<1);
	}
	
	//printf("\n");
}

static void pvObRawBpSumAvg( float* avgBuf , UINT8 upOrDowm)
{
	UINT8 cnt;

	cnt = PV_OB_STRIP_DMA_WIDTH/PV_OB_STRIP_DMA_HEIGHT;
		
	pvObRawBpStripAvgs(avgBuf,cnt);
	pvObRawBpStripSearch(avgBuf,cnt,upOrDowm);

}

void pvObRawBpCal(UINT32 addr,UINT16 xSize,UINT16 ySize)
{
	static UINT16 startX,startY,endX,endY=0xffff;
	float R,Gr,B,Gb;
	float tmp1,tmp2,tmp3,tmp4;
	UINT8 bayer;
	UINT16 j;
	static UINT8 num = 0;
	float avgBuf[PV_OB_BLOCK_WIDTH/PV_OB_STRIP_DMA_HEIGHT];
	UINT8 dmaYOff;
	UINT16 dmaXOff;
	UINT32 readStrAddr,readAddr,readOff;
	UINT8 status;

	if(calPvOBOnePoint == 1){
		num = 0; 
	}
	
	bayer = XBYTE[REG_Cdsp_pixsel] & 0x03;

	ENTER_CRITICAL( status );

	/*clean the Statistic global variables*/
	calibrationRaw0 = calibrationRaw1 = calibrationRaw2 = calibrationRaw3 = 0;
	calibrationRaw0PixSum = calibrationRaw1PixSum = calibrationRaw2PixSum = calibrationRaw3PixSum = 0;

//	pvObBpThr = 100;

	/*Get the ROI coordinate*/
	startX = ((xSize>>1)%2)?((xSize>>1)-(PV_OB_BLOCK_WIDTH>>1)-1):((xSize>>1)-(PV_OB_BLOCK_WIDTH>>1));
	startY = ((ySize>>1)%2)?((ySize>>1)-(PV_OB_BLOCK_HEIGHT>>1)-1):((ySize>>1)-(PV_OB_BLOCK_HEIGHT>>1));	
	endX = startX + PV_OB_BLOCK_WIDTH;
	endY = startY + PV_OB_BLOCK_HEIGHT;
	
	//printf("s(%u,%u),e(%u,%u)\n",startX,startY,endX,endY);
	
	readStrAddr = addr + (((UINT32)startY*xSize + startX)>>1);
	readAddr = readStrAddr;
	//printf("Addr(%lu),str(%lu)\n",addr,readAddr);
	readOff = (UINT32)xSize*PV_OB_STRIP_DMA_HEIGHT;  /*offset is 8*2 lines , but addr is word*/
		
	dmaYOff = ((UINT8)PV_OB_STRIP_DMA_HEIGHT<<1);
	dmaXOff = xSize;   /* interval is one xSize*/

	/*read RAW , Get 8x8 block Avg , eliminate bad pixel , Get R/Gr/B/Gb avgs without badpixel*/
	for(j = startY ; j < endY ; j += dmaYOff){	/*upper*/
		pvObRawBpStripRead(readAddr,dmaXOff);
		pvObRawBpSumAvg(avgBuf,0);
		#if 0
		if(1){
			UINT16 mm;
			printf("\033[33m");
			for(mm = 0 ; mm < 640 ; mm ++){
				printf("%bu-",G_ucStorData[mm]);
			}
			printf("\033[32m");
		}
		#endif
		readAddr += readOff;
	}

	readAddr = readStrAddr + ((UINT32)xSize>>1);  /*offset one line*/
	
	//printf("Addr(%lu),str(%lu)\n",addr,readAddr);
	for(j = startY+1 ; j < endY ; j += dmaYOff){ 	/*lower*/
		pvObRawBpStripRead(readAddr,dmaXOff);
		pvObRawBpSumAvg(avgBuf,1);
		#if 0
		if(1){
			UINT16 mm;
			printf("\033[32m");
			for(mm = 0 ; mm < 640 ; mm ++){
				printf("%bu-",G_ucStorData[mm]);
			}
			printf("\033[0m\n");
		}
		#endif
		readAddr += readOff;
	}
	
	EXIT_CRITICAL( status );
	
	tmp1 = (float)calibrationRaw0 / calibrationRaw0PixSum;
	tmp2 = (float)calibrationRaw1 / calibrationRaw1PixSum;
	tmp3 = (float)calibrationRaw2 / calibrationRaw2PixSum;
	tmp4 = (float)calibrationRaw3 / calibrationRaw3PixSum;
	
	//printf("%lu-%lu-%lu-%lu\n",calibrationRaw0,calibrationRaw1,calibrationRaw2,calibrationRaw3);
	//printf("%u-%u-%u-%u\n",calibrationRaw0PixSum,calibrationRaw1PixSum,calibrationRaw2PixSum,calibrationRaw3PixSum);
	switch(bayer){
		case 0: 	 //GR	
			R = tmp2;
			Gr = tmp1;
			B = tmp3;
			Gb = tmp4;
			break;
		case 1: 	 //RG
			R = tmp1;
			Gr = tmp2;
			B = tmp4;
			Gb = tmp3;		
			break;
		case 2:    //BG
			R = tmp4;
			Gr = tmp3;
			B = tmp1;
			Gb = tmp2;			
			break;
		case 3:    //GB
			R = tmp3;
			Gr = tmp4;
			B = tmp2;
			Gb = tmp1;		
			break;
	}

	calibrationRawRAvg = R;
	calibrationRawGrAvg = Gr;
	calibrationRawBAvg = B;
	calibrationRawGbAvg = Gb;

	if( calibrationMode == SP1K_CALIBRATION_MODE_PV_OB && num < 9) {
		calPvOBAvgData[num].rAvg = R;
		calPvOBAvgData[num].grAvg = Gr;
		calPvOBAvgData[num].bAvg = B;
		calPvOBAvgData[num].gbAvg = Gb;
		/*if(num == 8)
		{
			UINT8 j;
			for(j == 0 ; j < 9 ; j++){			
				printf("\nR=%f,Gr=%f,B=%f,Gb=%f\n\n",calPvOBAvgData[j].rAvg,calPvOBAvgData[j].grAvg,calPvOBAvgData[j].bAvg,calPvOBAvgData[j].gbAvg);	
			}
		}*/
		num++;
		if(num == 9){
			num = 0;
		}
	}
	//printf("R=%f,Gr=%f,B=%f,Gb=%f\n",R,Gr,B,Gb);	
	

	
}
#endif
/**************************************************************************
 *					 I N T E R N A L   U S E   O N L Y					  *
 **************************************************************************/
/**
 * @fn		  UINT8 calibrationRaw(UINT32 addr, UINT16 xSize, UINT16 ySize)
 * @brief	  calibrationRaw
 * @param	  [in] addr
 * @param	  [in] xSize
 * @param	  [in] ySize
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
calibrationRaw(
	UINT32 addr,
	UINT16 xSize,
	UINT16 ySize
)
{

#if OB_CAL_EN
	static UINT16 startX,startY,endX,endY=0xffff;
	float R,Gr,B,Gb;
	float tmp1,tmp2,tmp3,tmp4;
	UINT32 tmp11,tmp22,tmp33,tmp44;
	UINT32 pixelSum,startAddr;
	UINT8 bayer;
	UINT16 i,j;
	UINT16 sizeX;
	UINT16 sizeY;
	UINT8 status;	
	static UINT8 num = 0;
	bayer = XBYTE[REG_Cdsp_pixsel] & 0x03;
	
	if(calPvOBOnePoint == 1){
		num = 0; 
	}
	
	if(endY == 0xffff || num == 0){
		#if 0
		startX = ((UINT16)(xSize*RAW_ROI_START)%2)?(xSize * RAW_ROI_START-1):(xSize * RAW_ROI_START);
		startY = ((UINT16)(ySize*RAW_ROI_START)%2)?(ySize* RAW_ROI_START-1):(ySize* RAW_ROI_START);
		endX = ((UINT16)(xSize*RAW_ROI_END)%2)?(xSize * RAW_ROI_END):(xSize * RAW_ROI_END-1);
		endY = ((UINT16)(ySize*RAW_ROI_END)%2)?(ySize* RAW_ROI_END):(ySize* RAW_ROI_END-1);	
		#else
		startX = ((xSize>>1)%2)?((xSize>>1)-(PV_OB_BLOCK_WIDTH>>1)-1):((xSize>>1)-(PV_OB_BLOCK_WIDTH>>1));
		startY = ((ySize>>1)%2)?((ySize>>1)-(PV_OB_BLOCK_HEIGHT>>1)-1):((ySize>>1)-(PV_OB_BLOCK_HEIGHT>>1));	
		endX = startX + PV_OB_BLOCK_WIDTH -1;
		endY = startY + PV_OB_BLOCK_HEIGHT - 1;
		#endif
	}
	
	//printf("s(%u,%u),e(%u,%u)\n",startX,startY,endX,endY);
	sizeX = endX -startX + 1;
	sizeY = endY -startY + 1;
	
	tmp1 = tmp2 = tmp3 = tmp4 = pixelSum = 0;
	tmp11 = tmp22 = tmp33 = tmp44 = 0;
	
	ENTER_CRITICAL( status );

#if 0
	
	for( j = 0; j < sizeY; j++ ) {
		startAddr = (UINT32)addr + ( ( ( (UINT32)startY + j ) * xSize ) >> 1 ) + ( startX >> 1 );
		M_DRAM_SetStartAddress( startAddr, 1 );

		for( i = 0; i < ( sizeX >> 1 ); i++ ) {
			M_DRAM_ReadWord( lowByte, highByte );			
			if( ( j % 2 ) == 0 ) {
				tmp1 += (float)lowByte;
				tmp2 += (float)highByte;
			}
			else {
				tmp3 += (float)lowByte;
				tmp4 += (float)highByte;
			}
		}
	}
#else
	startAddr = (UINT32)addr + ( ( ( (UINT32)startY ) * xSize ) >> 1 ) + ( startX >> 1 );
	//printf("Addr(%lu),str(%lu)\n",addr,startAddr);

	xSize >>= 1; //convert to word
	sizeX >>=1;
	for( j = 0; j < sizeY; j += 2 ) {
		M_DRAM_SetStartAddressV( startAddr, 1 );
		i = sizeX;
		do {
			tmp11 += M_REG_DRAM_LsbByte;
			tmp22 += M_REG_DRAM_MsbByte;
		} while (--i);
		/*for( i = 0; i < sizeX ; i++ ) {
			M_DRAM_ReadWord( lowByte, highByte );			
			tmp11 += lowByte;
			tmp22 += highByte;
		}*/
		
		startAddr = startAddr + xSize;
		M_DRAM_SetStartAddressV( startAddr, 1 );
		i = sizeX;
		do {
			tmp33 += M_REG_DRAM_LsbByte;
			tmp44 += M_REG_DRAM_MsbByte;
		} while (--i);	
		/*for( i = 0; i < sizeX ; i++ ) {
			M_DRAM_ReadWord( lowByte, highByte );			
			tmp33 += lowByte;
			tmp44 += highByte;
		}	*/
		startAddr = startAddr + xSize;		
	}	
#endif
	EXIT_CRITICAL( status );

	pixelSum = (endX-startX+1)*(endY-startY+1)/4;

	//printf("pixelSum(%lu)\n",pixelSum);
	
	tmp1 = tmp11;
	tmp2 = tmp22;
	tmp3 = tmp33;
	tmp4 = tmp44; 

	calibrationRaw0 = tmp1;
	calibrationRaw1 = tmp2;
	calibrationRaw2 = tmp3;
	calibrationRaw3 = tmp4;
	//printf("%lu-%lu-%lu-%lu\n",calibrationRaw0,calibrationRaw1,calibrationRaw2,calibrationRaw3);
	tmp1 = tmp1/pixelSum;
	tmp2 = tmp2/pixelSum;
	tmp3 = tmp3/pixelSum;
	tmp4 = tmp4/pixelSum;

	switch(bayer){
		case 0: 	 //GR	
			R = tmp2;
			Gr = tmp1;
			B = tmp3;
			Gb = tmp4;
			break;
		case 1: 	 //RG
			R = tmp1;
			Gr = tmp2;
			B = tmp4;
			Gb = tmp3;		
			break;
		case 2:    //BG
			R = tmp4;
			Gr = tmp3;
			B = tmp1;
			Gb = tmp2;			
			break;
		case 3:    //GB
			R = tmp3;
			Gr = tmp4;
			B = tmp2;
			Gb = tmp1;		
			break;
	}
	
	calibrationRawRAvg = R;
	calibrationRawGrAvg = Gr;
	calibrationRawBAvg = B;
	calibrationRawGbAvg = Gb;
	if( calibrationMode == SP1K_CALIBRATION_MODE_PV_OB && num < 9) {
		calPvOBAvgData[num].rAvg = R;
		calPvOBAvgData[num].grAvg = Gr;
		calPvOBAvgData[num].bAvg = B;
		calPvOBAvgData[num].gbAvg = Gb;
		/*if(num == 8)
		{
			UINT8 j;
			for(j == 0 ; j < 9 ; j++){			
				printf("\nR=%f,Gr=%f,B=%f,Gb=%f\n\n",calPvOBAvgData[j].rAvg,calPvOBAvgData[j].grAvg,calPvOBAvgData[j].bAvg,calPvOBAvgData[j].gbAvg);	
			}
		}*/
		num++;
		if(num == 9){
			num = 0;
			endY = 0xffff;
		}
	}
	//printf("Avg =%bu\n",(UINT8)((R+Gr+B+Gb)/4));
	//printf("R=%f,Gr=%f,B=%f,Gb=%f\n",R,Gr,B,Gb);	

#else
	UINT16 xCent, yCent;
	UINT16 xPost, yPost;
	UINT32 startAddr;
	UINT8 temp0, temp1;
	UINT32 total;
	UINT16 i, j;
	UINT8 status;
	UINT8 bayer;

	xCent = xSize >> 1;
	yCent = ySize >> 1;

	xPost = xCent - ( CALIBRATION_RAW_CROP_X_SIZE >> 1 );
	yPost = yCent - ( CALIBRATION_RAW_CROP_Y_SIZE >> 1 );

	// alignment??
	xPost = xPost >> 1 << 1;
	yPost = yPost >> 1 << 1;

	calibrationRaw0 = calibrationRaw1 = calibrationRaw2 = calibrationRaw3 = 0;

	ENTER_CRITICAL( status );

	for( j = 0; j < CALIBRATION_RAW_CROP_Y_SIZE; j++ ) {
		startAddr = (UINT32)addr + ( ( ( (UINT32)yPost + j ) * xSize ) >> 1 ) + ( xPost >> 1 );

		M_DRAM_SetStartAddress( startAddr, 1 );

		for( i = 0; i < ( CALIBRATION_RAW_CROP_X_SIZE >> 1 ); i++ ) {
			M_DRAM_ReadWord( temp0, temp1 );			
			if( ( j % 2 ) == 0 ) {
				calibrationRaw0 += (UINT32)temp0;
				calibrationRaw1 += (UINT32)temp1;
			}
			else {
				calibrationRaw2 += (UINT32)temp0;
				calibrationRaw3 += (UINT32)temp1;
			}
		}
	}

	EXIT_CRITICAL( status );

	total = ( (UINT32)CALIBRATION_RAW_CROP_X_SIZE * CALIBRATION_RAW_CROP_Y_SIZE ) >> 2;

	//printf( "R [%lx], Gr[%lx], B[%lx], Gb[%lx]\n", calibrationRaw0, calibrationRaw1,	
	//calibrationRaw2, calibrationRaw3 );
	//printf("total =%lx\n",total); 


	calibrationRaw3 = ( calibrationRaw3 / total );
	calibrationRaw2 =( calibrationRaw2 / total );
	calibrationRaw0 = ( calibrationRaw0 / total );
	calibrationRaw1 =( calibrationRaw1 / total );
	
	bayer = XBYTE[REG_Cdsp_pixsel]&0x03;

	#if 1
	switch (bayer){
		case 0:		
			calibrationRawRAvg = READ8(calibrationRaw1,3);
			calibrationRawGrAvg =READ8(calibrationRaw0,3);
			calibrationRawBAvg = READ8(calibrationRaw2,3);
			calibrationRawGbAvg =READ8(calibrationRaw3,3);
		break;
		case 1:		
			calibrationRawRAvg = READ8(calibrationRaw0,3);
			calibrationRawGrAvg =READ8(calibrationRaw1,3);
			calibrationRawBAvg = READ8(calibrationRaw3,3);
			calibrationRawGbAvg =READ8(calibrationRaw2,3);	
		break;
		case 2:		
			calibrationRawRAvg = READ8(calibrationRaw3,3);
			calibrationRawGrAvg =READ8(calibrationRaw2,3);
			calibrationRawBAvg = READ8(calibrationRaw0,3);
			calibrationRawGbAvg =READ8(calibrationRaw1,3);		
		break;
		case 3:		
			calibrationRawRAvg = READ8(calibrationRaw2,3);
			calibrationRawGrAvg =READ8(calibrationRaw3,3);
			calibrationRawBAvg = READ8(calibrationRaw1,3);
			calibrationRawGbAvg =READ8(calibrationRaw0,3);				
		break;
		
		
	}	
	#else
	if( XBYTE[ 0x2110 ] == 0x00 ) {
		calibrationRawRAvg = (UINT8)( calibrationRaw1 / total );
		calibrationRawGrAvg = (UINT8)( calibrationRaw0 / total );
		calibrationRawBAvg = (UINT8)( calibrationRaw2 / total );
		calibrationRawGbAvg = (UINT8)( calibrationRaw3 / total );
	}
	else if( XBYTE[ 0x2110 ] == 0x01 ) {
		calibrationRawRAvg = (UINT8)( calibrationRaw0 / total );
		calibrationRawGrAvg = (UINT8)( calibrationRaw1 / total );
		calibrationRawBAvg = (UINT8)( calibrationRaw3 / total );
		calibrationRawGbAvg = (UINT8)( calibrationRaw2 / total );
	}
	else if( XBYTE[ 0x2110 ] == 0x10 ) {
		calibrationRawRAvg = (UINT8)( calibrationRaw3 / total );
		calibrationRawGrAvg = (UINT8)( calibrationRaw2 / total );
		calibrationRawBAvg = (UINT8)( calibrationRaw0 / total );
		calibrationRawGbAvg = (UINT8)( calibrationRaw1 / total );
		
	}
	else if( XBYTE[ 0x2110 ] == 0x11 ) {
		calibrationRawRAvg = (UINT8)( calibrationRaw2 / total );
		calibrationRawGrAvg = (UINT8)( calibrationRaw3 / total );
		calibrationRawBAvg = (UINT8)( calibrationRaw1 / total );
		calibrationRawGbAvg = (UINT8)( calibrationRaw0 / total );
	}
	#endif
	
	#if 1
		//printf( "R [%bx], Gr[%bx], B[%bx], Gb[%bx]\n", calibrationRawRAvg, calibrationRawGrAvg,
		//calibrationRawBAvg, calibrationRawGbAvg );
	#endif
#endif
	return 0;
}

/**
 * @fn		  UINT8 calibrationPvRaw(void)
 * @brief	  calibrationPvRaw
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
calibrationPvRaw(
	void
)
{
	UINT32 addr = K_SDRAM_CommonEndAddr;
	UINT16 prevW, prevH;
	UINT8 f[ 13 ] = "00000000RAW";
	UINT8 val;

	frontPreviewSizeGet(0, &prevW, &prevH);

	XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~(UINT8)0x20);
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_HRGBscaleen]=0;

	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

	// skip AE/AWB data to dram
	HAL_CdspVDramSkipWrGet(&val);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);
	HAL_CdspVDramSkipWrSyncSet(1);
	//HAL_CdspVDramSkipImgWrSet(1);
	//HAL_CdspVDramSkipAeWrSet(1);
	//HAL_CdspVDramSkipAwbWrSet(1);
	
	HAL_GlobalCamModeSet(2);

	HAL_DramRawFrmBufAddrSet(addr);
	HAL_DramRawFrmBufSizeSet(prevW/2, prevH);

	cdspSnapDramImgTypeSet(CDSP_SNAP_8RAW);

	HAL_CdspRawImgSource(0);

	HAL_CdspVDramSizeClamp(1, prevW, 1, prevH, 1);

	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);

	HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);
	
	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);
	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspEofWait(1);

	HAL_CdspRawImgSource(1);

	//XBYTE[0x21f0] = 0x01; //local reset
	//XBYTE[0x21f0] = 0x00;
	HAL_CdspRstBlock(CDSPRstCDSP);

	if( calibrationMode == SP1K_CALIBRATION_MODE_PV_OB ) {
		sp1kCalibrationRawCal(addr, prevW, prevH);
	}

	f[ 5 ] = '0' + calibrationPvRawCnt / 100;
	f[ 6 ] = '0' + calibrationPvRawCnt / 10;
	f[ 7 ] = '0' + calibrationPvRawCnt % 10;

	f[ 11 ] = 0x20;
	f[ 12 ] = 0x00;

	if( calibrationMode != SP1K_CALIBRATION_MODE_PV_OB ) {//4 //fan.xiao modified 2011-04-11 , 15:21:57  SAVE raw need about 7 sec
		dramFileSave(f, addr, (UINT32)prevW * prevH);
	}
	else if( calibrationMode == SP1K_CALIBRATION_MODE_PV_OB && calPvObRawSave == 1 ) {//4 //fan.xiao modified 2011-04-11 , 15:21:57  SAVE raw need about 7 sec
		UINT16 expSave = calPvOBAvgData[calibrationPvRawCnt%9].expLine;
		UINT16 bayerPatten = XBYTE[REG_Cdsp_pixsel] & 0x03;
		M_DRAM_SetStartAddress((UINT32)addr - 1, 1);
		M_DRAM_WriteWord(expSave & 0xff,expSave >> 8);
		M_DRAM_WriteWord(bayerPatten & 0xff,bayerPatten >> 8);
		M_DRAM_WriteWord(prevW & 0xff,prevW >> 8);
		M_DRAM_WriteWord(prevH & 0xff,prevH >> 8);
		dramFileSave(f, addr, (UINT32)prevW * prevH);
	}	
	strcpy(calLogFile, f); /* add by Phil for save the file name for calibration */

	calibrationPvRawCnt++;

	//sp1kPreviewSet(0, K_Preview_Width, K_Preview_Height, PvZoomCtrl.factor, 0);  //will mark
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,val);
	HAL_CdspVDramSkipWrSyncSet(val>>4);
	return 0;
}


/**
 * @fn		  void calibrationObGammeLoad(void)
 * @brief	  calibrationObGammeLoad
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
calibrationObGammeLoad(
	void
)
{
	UINT16 i, value;
	UINT8 t0, t1, t2;

	XBYTE[REG_Cdsp_macroSel]= 0x04;
	XBYTE[REG_Cdsp_switchclk]= 0x04;

	t1 = 0x55;
	t2 = 0x15;

	for (i=0;i<0x80 ;i++) {
		value = i * 8;

		if( value > 255 ) {
			t0 = 255;
			t1 = t2 = 0;
		}
		else {
			t0 = (UINT8)value;
		}

		XBYTE[REG_Cdsp_macropagsel]= 0x00;
		XBYTE[0x2b00+i] =t0;

		XBYTE[REG_Cdsp_macropagsel]= 0x01;
		XBYTE[0x2b00+i] =t1;

		XBYTE[REG_Cdsp_macropagsel]= 0x02;
		XBYTE[0x2b00+i] =t2;
	}

	XBYTE[REG_Cdsp_macroSel]= 0x00;
	XBYTE[REG_Cdsp_switchclk]= 0x00;
	XBYTE[REG_Cdsp_macropagsel]= 0x00;

	return;
}

/**
 * @fn		  void sp1kCalibrationPrvRawModeSet(UINT8 mode)
 * @brief	  sp1kCalibrationPrvRawModeSet
 * @param	  [in] mode
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationPrvRawModeSet(
	UINT8 mode
)
{
#ifdef PREVIEW_RAW_SAVE_MODE
	UINT16 prevW, prevH;
	UINT8 f[ 13 ] = "00000000RAW";

	HAL_CdspVDramSkipWrSet(	CDSP_SKIP_AWBAEEDGE,0xff);
	HAL_CdspVDramSkipWrSyncSet(1);

	frontPreviewSizeGet(0, &prevW, &prevH);

	
	HAL_CdspFrmCtrlEnSet(0);
	XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~(UINT8)0x20);
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_HRGBscaleen] = 0;

	HAL_DramRawFrmBufAddrSet(K_SDRAM_CommonEndAddr);
	HAL_DramRawFrmBufSizeSet(prevW/2, prevH);
	if (mode == 0) {//8 bit
		HAL_CdspWriteDramMode(1, 1);
	 }
	else  {// 10 bit
		HAL_CdspWriteDramMode(2, 1);
	}

	HAL_CdspVDramSizeClamp(1, prevW, 1, prevH, 1);
	HAL_CdspEofWait(1);
	XBYTE[REG_Dram_Cdsp1eof_inten] |= 0x02;

	HAL_CdspVDramSkipWrSet(	CDSP_SKIP_ALL, 0);
#endif
	mode = mode;
}

/**
 * @fn		  void sp1kCalibrationPrvRawSave(UINT8 mode)
 * @brief	  sp1kCalibrationPrvRawSave
 * @param	  [in] mode
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kCalibrationPrvRawSave(
	UINT8 mode
)
{
#ifdef PREVIEW_RAW_SAVE_MODE
	UINT8 f[ 13 ] = "00000000RAW";
	UINT32 addr = K_SDRAM_CommonEndAddr;
	UINT16 prevW, prevH;
	UINT32 nextPrvRawAddr;
	UINT32 rawsizeInWord;


	frontPreviewSizeGet(0, &prevW, &prevH);
	rawsizeInWord = (UINT32)prevW * (UINT32)prevH;

	if (mode == 0) {
		rawsizeInWord = rawsizeInWord/2;
	}

	nextPrvRawAddr = addr + rawsizeInWord;
	while ( nextPrvRawAddr <= K_SDRAM_TotalSize) {
		f[ 5 ] = '0' + calibrationPvRawCnt / 100;
		f[ 6 ] = '0' + calibrationPvRawCnt / 10;
		f[ 7 ] = '0' + calibrationPvRawCnt % 10;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		//printf("save addr = %lx, %s\n", addr, f);
		dramFileSave( f, addr, rawsizeInWord * 2);

		calibrationPvRawCnt++;
		addr = nextPrvRawAddr;
		nextPrvRawAddr = nextPrvRawAddr + rawsizeInWord;
	}

	//pvSizeGet(&prevW, &prevH);  //will
	sp1kPreviewSet(0, prevW, prevH, 100, 0);
#endif
	mode = mode;
}


/**
 * @fn		  void INTR_calibrationPrvRawAddrSet(void)
 * @brief	  INTR_calibrationPrvRawAddrSet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void
INTR_calibrationPrvRawAddrSet(
	void
)
{
#ifdef PREVIEW_RAW_SAVE_MODE
	UINT16 prevW, prevH;
	UINT32 nextPrvRawAddr;


	frontPreviewSizeGet(0, &prevW, &prevH);
	nextPrvRawAddr = prvRawAddr + (UINT32)prevW * (UINT32)prevH/2;

	if (nextPrvRawAddr	>= K_SDRAM_TotalSize) {
		prvRawAddr = K_SDRAM_CommonEndAddr;
		XBYTE[REG_Dram_Cdsp1eof_inten] &= 0xFD;
		XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0x02;

		return;
	}

	calibrationHalRawAddrSet(prvRawAddr);
	prvRawAddr = nextPrvRawAddr;

	XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0x02;
#endif
}

/**
 * @fn		  void calibrationHalRawAddrSet(UINT32 Addr)
 * @brief	  calibrationHalRawAddrSet
 * @param	  [in] Addr
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void calibrationHalRawAddrSet(UINT32 Addr)
{
#ifdef PREVIEW_RAW_SAVE_MODE
	XBYTE[REG_Dram_SnapWstAddr_B0] = READ8(&Addr, 3);
	XBYTE[REG_Dram_SnapWstAddr_B1] = READ8(&Addr, 2);
	XBYTE[REG_Dram_SnapWstAddr_B2] = READ8(&Addr, 1);
	XBYTE[REG_Dram_SnapWstAddr_B3] = READ8(&Addr, 0);
#endif
	Addr = Addr;
}

#if FUNC_KNL_BP
/**
 * @fn		  UINT8 calBpProcRaw(bpMode_t mode)
 * @brief	  calBpProcRaw
 * @param	  [in] mode
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
extern xdata	UINT16	G_USR_LastFileIndex;
static UINT8
calBpProcRaw(
	bpMode_t mode
)
{
	UINT8 ret = SUCCESS;

	bpPara_t* pPara;

	bpCfgGet(&pPara);

	/* param check */
	if ( mode >= BP_MODE_MAX ) {
		return UNKNOWN;
	}
	
	sp1kAeSetModeEn(0);
	sp1kAwbDisable();

	/* get the raw image */
	if ( mode == BP_MODE_PRV_VGA || mode == BP_MODE_PRV_HD) {
		ret = calibrationPvRaw();
	} else { /* mode == BP_MODE_CAP */
		/* snap param cfg */
		sp1kSnapSizeSet( pPara->capWidth, pPara->capHeight );
		
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_QV_PERIOD, 0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 0x01 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BAD_PIXEL_OPT, SNAP_BP_DISABLE ); 
		
		/* trig snap task */
		ret = sp1kSnapTask();

		/* save the file name */
		//dcfJpgNameGet( calLogFile, G_USR_LastFileIndex );
		dcfMediaNameGet( calLogFile, G_USR_LastFileIndex,MEDIA_JPG);
		calLogFile[8] = 'R';
		calLogFile[9] = 'A';
		calLogFile[10] = 'W';
		calLogFile[11] = 0x20;
		calLogFile[12] = 0x00;
	}
	return ret;
}


/**
 * @fn		  UINT8 calBpProcFind(bpMode_t mode)
 * @brief	  calBpProcFind
 * @param	  [in] mode
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
static UINT8
calBpProcFind(
	bpMode_t mode
)
{

	UINT8 ret = SUCCESS;
	UINT8 sts = TRUE;
	UINT32 rawAddr = (UINT32)CAL_BP_RAW_BUF;
	UINT32 badAddr = (UINT32)CAL_BP_BAD_BUF;
	UINT16 rawW, rawH;
	UINT32 badTotal;
	UINT32 time1 = 0, time2 = 0;
	UINT32 rawSize;
	UINT16 tBank, tAddr;
	bpPara_t* pPara;

	bpCfgGet(&pPara);
	
	/* param check */
	if ( mode >= BP_MODE_MAX ) {
		return UNKNOWN;
	}
	
	/* cdsp gating disable */
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspRawImgSource(1);

	/* read the raw image to dram */
	//printf("calLogFile: %s\n", calLogFile);
	//printf("LOAD RAW\n");
	sts = dramFileLoad(calLogFile, rawAddr, &rawSize);
	if(sts != TRUE){
	//	printf("%s not find!\n", calLogFile);
		HAL_CdspRawImgSource(0);
		HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);
		return FAIL;
	}

	/* save IMEM shadow configuration */	
	HAL_CpuImemSdGet((logBank_t *)&tBank, &tAddr);
	
	/* shadow badpixel function to IMEM */
	HAL_CpuImemSdSet(LOG_BANK_16, 0xc000);

	/* check the image width&height */
	if (mode == BP_MODE_PRV_HD) {
		rawW = pPara->prvWidth_HD;
		rawH = pPara->prvHeight_HD;
	}
	else if ( mode == BP_MODE_PRV_VGA ) {
		rawW = pPara->prvWidth;
		rawH = pPara->prvHeight;
	} 
	else {
		rawW = pPara->capWidth;
		rawH = pPara->capHeight;
	}
	/* badpixel find process */
//	printf("FIND\n");
	sp1kBpFind(mode, rawAddr, rawW, rawH, badAddr, &badTotal);

	/* restore IMEM shadow configuration */
	HAL_CpuImemSdSet(tBank, tAddr);

	/* cdsp gating enable */
	HAL_CdspRawImgSource(0);
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

	return SUCCESS;
}


/**
 * @fn		  UINT8 sp1kCalBpProc(UINT8 mode)
 * @brief	  sp1kCalBpProc
 * @param	  [in] mode
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
sp1kCalBpProc(
	UINT8 mode
)
{
	UINT8 ret = SUCCESS;

	/* param check */
	if ( mode >= BP_MODE_MAX ) {
		return UNKNOWN;
	}

	/* process start */
	dbgTimerStart();
	/* snap raw image */
	sp1kDispImgWinEn(0);
	//printf("CAP RAW\n");
	ret = calBpProcRaw( mode );
	if ( ret != SUCCESS ) {
		//printf("CAP RAW FAIL! mode(%bu)",mode);
		return ret;
	}

	sp1kDispImgWinEn(0);
	/* badpixel find */
	ret = calBpProcFind( mode );
	if ( ret != SUCCESS ) {
		return ret;
	}
	/* all done */
	dbgTimerStop();
	return ret;
}
#endif


/**
 * @fn		  UINT8 Sp1kAeCalibration(void)
 * @brief	  Sp1kAeCalibration
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 Sp1kAeCalibration(void)
{
	UINT8 gid_date1,gid_date2,gid_date3,gid_date4;
	SINT16 gid_ev10;
	UINT32 tmpAddress;
	UINT8 dummy = 0;   /* avoid XDATA overflow ,don't modify*/

	gid_ev10 = gId;

	//printf("gid:%x\n",gid_ev10);

	gid_date1 = READ8(gid_ev10, 0);
	gid_date2 = READ8(gid_ev10,1 );

	gid_ev10 =~ gid_ev10;

	//printf("gid:%x\n",gid_ev10);

	gid_date3 = READ8(gid_ev10,0 );
	gid_date4 = READ8(gid_ev10, 1);

	tmpAddress = sp1kPvFreeBuffGet();

	if ( 0 == sp1kNandRsvRead(0x02, tmpAddress << 1) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}

	HAL_DramStartAddrSet(tmpAddress+2, 0);
	HAL_DramWordDataWrite(gid_date2, gid_date1);
	HAL_DramWordDataWrite(gid_date4, gid_date3);
	HAL_DramWordDataWrite(0x55, 0xAA);
	HAL_DramWordDataWrite(0x55, 0xAA);

	if( 0 == sp1kNandRsvWrite(0x02, tmpAddress << 1) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}
	
	if(dummy == 1)          
		sp1kCalBpProc(0);  /* avoid XDATA overflow */
	else
		return SUCCESS;
}
	
/**
 * @fn		  UINT8 Sp1kCalibrationEvParamInit(void)
 * @brief	  Sp1kCalibrationEvParamInit
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 Sp1kCalibrationEvParamInit(void)
{

	UINT32 tmpAddress;
	SINT16 gid_ev10,gid_evoff10;
	UINT8 sign[4];
	UINT8 gid_date1,gid_date2,gid_date3,gid_date4;
	frontPrevCapabDesc_t *pcap;

	
	frontCapabilityGet(0, &pcap, NULL);
	
	calibrationEv10 = pcap->ev10ExpIdx;
	
	tmpAddress = sp1kPvFreeBuffGet();
	
	if ( 0 == sp1kNandRsvRead(0x02, tmpAddress << 1) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}
	
	HAL_DramStartAddrSet(tmpAddress, 1);
	HAL_DramWordDataRead(&sign[0], &sign[1]);
	HAL_DramWordDataRead(&sign[2], &sign[3]);
	HAL_DramWordDataRead(&gid_date2, &gid_date1);
	HAL_DramWordDataRead(&gid_date4, &gid_date3);
	WRITE8(gid_ev10, 0, gid_date1);
	WRITE8(gid_ev10, 1, gid_date2);
	WRITE8(gid_evoff10, 0, gid_date3);
	WRITE8(gid_evoff10, 1, gid_date4);

	//printf(":%bx,%bx,%bx,%bx,%x,%x,%bx,%bx,%bx,%bx\n",sign[0],sign[1],sign[2],sign[3],gid_ev10,gid_evoff10,gid_date1,gid_date2,gid_date3,gid_date4);
	if ( (gid_ev10 != (~gid_evoff10)) || (gid_ev10 == 0xffff) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}
	if ( (sign[0] != 0x41) || (sign[1] != 0x45) || (sign[2] != 0x31) || (sign[3] != 0x30) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}
	
	//HAL_DramStartAddrSet(tmpAddress+4, 1);
	HAL_DramWordDataRead(&sign[0], &sign[1]);
	HAL_DramWordDataRead(&sign[2], &sign[3]);
	if ( (sign[0] != 0x55) || (sign[1] != 0xAA) || (sign[2] != 0x55) || (sign[3] != 0xAA) ) {
		__ASSERT(__FILE, (unsigned short)__LINE, 1);
		return FAIL;
	}
	
	calibrationEv10 = (UINT8)gid_ev10;

	return SUCCESS;
}	

/**
 * @fn		  UINT8 Sp1kCalibrationEvValueGet(void)
 * @brief	  Sp1kCalibrationEvValueGet
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 Sp1kCalibrationEvValueGet(void)
{
	return calibrationEv10;
}

/**
 * @fn		  void sp1kCalibrationAEWinConfigSet(UINT mode, UINT8 colorRow, UINT8 colorColumn)
 * @brief	  sp1kCalibrationAEWinConfigSet
 * @param	  [in]mode: LUMA_CAL_COLOR_CHECKER, LUMA_CAL_LSC
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kCalibrationAEWinConfigSet(UINT8 mode)
{
#if 1
	UINT16 aeHSize,aeVSize;
	UINT16 aeHOffset,aeVOffset;
	UINT8 AEhaccfactor,AEvaccfactor;
	UINT16 hSize, vSize;
	UINT16 forceHsize, forceVsize, cropHsize, cropVsize; 

	/*OB LSC AWB Gamma turning off*/
	sp1kCalibrationObSet(0);
	sp1kCalibrationLscSet(0);
	sp1kCalibrationAWBSet(0);
	sp1kCalibrationGammaSet(0);

	frontPreviewSizeGet(0, &hSize, &vSize);

	//printf("hSize=%d, vSize=%d\n",hSize,vSize);

	sp1kAeSetModeEn(AE_DISABLE);	/* disable AE */

	aeProc = 0;

	aeWinModeSet(AE_WIN_MODE_RGB);		/* ae mode set 0:y window, 1:RGB window */
	aeCalculateLumaModeSet(mode);		/* set ae get luma mode  */

	if (mode == LUMA_CAL_COLOR_CHECKER) {
		if(hSize*3 > vSize*4)//16:9
		{
			cropHsize=vSize*4/3;
			cropVsize=vSize;
		}
		else
		{
			cropHsize=hSize;
			cropVsize=vSize;
		}
		sp1kPreviewCropSizeSet(cropHsize,cropVsize);
		//printf("cropHsize=%d, cropVsize=%d\n",cropHsize,cropVsize);
		/* 1/9 region, 5*4 block number, target lima 200 */
		forceHsize = ((cropHsize / 3) >> 1) << 1;	/* front in h size 1/9 */
		forceVsize = ((cropVsize / 3) >> 1) << 1;	/* front in v size 1/9 */
		sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 160);
		sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
		aeHSize    = forceHsize/6;
		aeVSize    = forceVsize/4;
		aeHOffset  = ((cropHsize - forceHsize) + (hSize - cropHsize)) >> 1 ;
		aeVOffset  = (cropVsize - forceVsize) >> 1;
	}
	else if (mode == LUMA_CAL_LSC) {
		cropHsize=hSize;
		cropVsize=vSize;
		sp1kPreviewCropSizeSet(cropHsize,cropVsize);
		//printf("cropHsize=%d, cropVsize=%d\n",cropHsize,cropVsize);
		/* all region, 5*5 block number, target lima 40 */
		forceHsize = cropHsize - 12;
		forceVsize = cropVsize - 12;
		sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 40);
		sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
		aeHSize    = forceHsize/5;
		aeVSize    = forceVsize/5;
		aeHOffset  = ((cropHsize - forceHsize)+ (hSize - cropHsize)) >> 1 ;
		aeVOffset  = (cropVsize - forceVsize) >> 1;
	}
	else {//LUMA_NORMAL
		return;
	}

	if (aeHSize > 128)		AEhaccfactor =7;
	else if (aeHSize> 64)   AEhaccfactor =6;
	else if (aeHSize > 32)  AEhaccfactor =5;
	else if (aeHSize > 16)  AEhaccfactor =4;
	else if (aeHSize > 8)   AEhaccfactor =3;
	else if (aeHSize > 4)   AEhaccfactor =2;
	else if (aeHSize > 2)   AEhaccfactor =1;
	else if (aeHSize >= 0)  AEhaccfactor =0;


	if (aeVSize > 128)		AEvaccfactor =7;
	else if (aeVSize > 64)  AEvaccfactor =6;
	else if (aeVSize > 32)  AEvaccfactor =5;
	else if (aeVSize > 16)  AEvaccfactor =4;
	else if (aeVSize > 8)   AEvaccfactor =3;
	else if (aeVSize > 4)   AEvaccfactor =2;
	else if (aeVSize > 2)   AEvaccfactor =1;
	else if (aeVSize >= 0)  AEvaccfactor =0;

	#if 0
	printf("======luma mode=====\n");
	printf("hSize=%d, vSize=%d\n",hSize,vSize);
	printf("aeHSize=%d, aeVSize=%d\n",aeHSize,aeVSize);
	printf("aeHOffset=%d, aeVOffset=%d\n",aeHOffset,aeVOffset);
	printf("AEhaccfactor=%bu, AEvaccfactor=%bu\n",AEhaccfactor,AEvaccfactor);
	printf("AEwinHintvl=2, AEwinVintvl=2\n");
	printf("AEwinHCnt=8, AEwinVCnt=8\n");
	#endif
	
	/* AE_WIN_MODE_RGB, RGB Window */
	HAL_CdspAEwinoffset   ( aeHOffset, aeVOffset);
	HAL_CdspAEwinsizeset  ( aeHSize, aeVSize);
	HAL_CdspAEwinfactorset( AEhaccfactor, AEvaccfactor );

	if (mode == LUMA_CAL_COLOR_CHECKER) {
		HAL_CdspAEwinintvlset( 14, 6 );		/* default to 0 */
		HAL_CdspAEwinCntset(5, 4);			/* ae window cnt */
	}
	else if (mode == LUMA_CAL_LSC) {
		HAL_CdspAEwinintvlset( 2, 2 );		/* default to 0 */
		HAL_CdspAEwinCntset(5, 5);			/* ae window cnt */
	}

	HAL_CdspAEwinAddrset( K_SDRAM_PvAeWndAddr );

	sp1kAeSetModeEn(AE_ENABLE);	/* enable AE */

	HAL_CdspAEWinTest(1);				//ae test enable
#endif
}


