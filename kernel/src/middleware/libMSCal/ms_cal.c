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
 * @file      ms_cal.c
 * @brief     Standalone calibration Mechanical Shutter
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/




/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"	 
#include "ms_cal.h"
#include "front_logi.h"
#include "sp1k_calibration_api.h"
#include "sp1k_awb_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_snap_api.h"
#include "hal_front_common.h"
#include "ms_debug.h"

#ifdef MS_CALIB_ENABLE

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/ 

#define MSCAL_TIMES 10

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/


UINT16 calLum[MSCAL_TIMES],calTime[MSCAL_TIMES];
UINT16 mSDelay,mSExpTime;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        UINT16 frontOpMSCalibration(void)
 * @brief     frontOpMSCalibration
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-12-30
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
msCalMSCalibration(
	void
)
{
		
	UINT8 i,idx;
	UINT16 shtDelay,shtStep;
	UINT16 lumRoll,gainRoll;
	UINT8 gIdVal,freq;
	UINT32 expRoll;
	UINT16 lumCenter,lumMax;
	UINT8 rawAvg[SP1K_CALIBRATION_OB_MAX];
	
	
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_SKIP_PV, 0x01 );
	sp1kSnapParamSet(SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,0);
		
	sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB ); /* Get average value of R,Gr,Gb,B */
	sp1kCalibrationGammaSet(0); 					/* Gamma Function Disable*/
	sp1kCalibrationAWBSet(0);						/* AWB OFF */
	
	frontSnapMethodSet(SNAP_MSCAL_ROLLING);
	sp1kCalibrationAESet(1);  		/* turn on AE */
	i=0;	
	
	sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 45);//120
	sp1kCalibrationAEWinConfigSet(LUMA_CAL_LSC); /* change AE mode and set luma target */
		
	while(sp1kAeReadyGet(1)!=SUCCESS);	/*wait ae ready for snap, timeout= 1s*/
	while(sp1kAeReadyGet(1)!=SUCCESS);	/*wait ae ready for snap, timeout= 1s*/
	while(sp1kAeReadyGet(1)!=SUCCESS);	/*wait ae ready for snap, timeout= 1s*/
	
	sp1kCalibrationAESet(0);	 /* AE OFF */
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	sp1kSnapTask();
	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	sp1kCalibrationOBDataGet(rawAvg);
	lumRoll = (rawAvg[1]  + rawAvg[3])/2;  /* G= (Gr+Gb)/2 */

	sp1kAeStatusGet(SP1K_AE_gId, &gIdVal); 
	sp1kAeStatusGet(SP1K_AE_G_AETblSel,&freq);
	
	HAL_FrontAeGet(gIdVal,freq,&expRoll,&gainRoll);		
	//msPrintf("gid=0x%bx,fre=%bd\n",gIdVal,freq);
	//msPrintf("exp=%ld,gain=0x%x\n",expRoll,gainRoll);
	
	expRoll=10*100000/expRoll;
	
	frontSnapMethodSet(SNAP_MSCAL_FRAME);
	//sp1kCalibrationAESet(0);	 /* AE OFF */
	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
	
	mSDelay=0;
	shtStep=(UINT16)(expRoll/10);
	mSExpTime= (UINT16)expRoll;	
	mSExpTime= mSExpTime - shtStep*5;

	//sp1kSnapParamSet( SP1K_SNAP_PARAM_DEBUG_RAW_ONLY, 1 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 0 );
	idx=0;
	lumMax=0;
	for(i= 0; i< MSCAL_TIMES;i++) {
				
		//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);
		sp1kSnapTask();
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);

		sp1kCalibrationOBDataGet(rawAvg);
		lumCenter=((UINT16)rawAvg[1] + (UINT16)rawAvg[3])/2 ;
	
		if ((lumCenter<245)&& (lumCenter>lumMax)){ 			 
			calLum[idx] = lumCenter;
			calTime[idx]=mSExpTime;			
			idx++;
			lumMax=lumCenter;
		}
			
		mSExpTime +=shtStep;	
	}
		
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_DEBUG_RAW_ONLY, 0 );	 
	shtDelay=msCalShutterDealyGet(calLum,calTime,idx,lumRoll,expRoll);

	frontSnapMethodSet(SNAP_MS_NORMAL);
	sp1kCalibrationAESet(1);	/*AE ON*/ 
 	sp1kCalibrationAWBSet(1);	/* AWB ON */

	sp1kCalibrationGammaSet(1); 		  // Gamma Function ON

	#if 1	  
		for(i= 0; i< idx;i++){ 
			msPrintf("[%bd] lum=%d, shutterTime=%d\n",i,calLum[i],calTime[i]);
		}
		
		msPrintf("rolling ExpTime=%ld,lum=%d\n",expRoll,lumRoll);
		msPrintf("sh delay=%d\n",shtDelay);
	#endif 
	return shtDelay;
	
}
	
/**
 * @fn        UINT16 GetMSDealy(UINT16 *Lum_Frame, UINT16 *Time_Frame, UINT8 Num, UINT16 Lum_Roll, UINT16 Time_Roll)
 * @brief     Get shutter delay by using least square method.
 * 			  y=ax+b ;   y: luminance   , x : shutter time
 *            y=a(x-d)+b	  d=(ax+b-y)/a	;  y: rolling luminance , x : rolling shutter time  
 * @param     [in] Lum_Frame : y
 * @param     [in] Time_Frame : x
 * @param     [in] Num
 * @param     [in] Lum_Roll
 * @param     [in] Time_Roll
 * @retval    return = shutter delay , unit: 10us
 * @see       NULL
 * @author    Will Chiu
 * @since     2009-11-12
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
msCalShutterDealyGet(
	UINT16 *Lum_Frame,
	UINT16 *Time_Frame,
	UINT8 num,
	UINT16 Lum_Roll,
	UINT16 Time_Roll
)
{
	
	float SumXY,SumX,SumY,a,b,Num_Entry;
	float SumXX;
	UINT8 i;
	UINT16 delay;
		
	SumXX=0;
	SumXY=0;
	SumX=0;
	SumY=0;
	Num_Entry=(FINT32)num;
		
	for(i=0;i<num;i++){
		SumXX+=(float)Time_Frame[i]*(float)Time_Frame[i];
		SumXY+=(float)Time_Frame[i]*(float)Lum_Frame[i];
		SumX+=(float)Time_Frame[i];
		SumY+=(float)Lum_Frame[i];
		//msPrintf("xy=%f\n",SumXY);
	}
			
	a= (Num_Entry*SumXY-SumX*SumY)/(Num_Entry*SumXX-SumX*SumX);
	b= SumY/Num_Entry-a*SumX/Num_Entry;

	delay=(UINT16)((a*(float)Time_Roll+b-(float)Lum_Roll)/a);	

	//msPrintf("a=%f,b=%f\n",a,b);
	//msPrintf("XX=%f,XY=%f,X=%f,Y=%f,Num=%f\n",SumXX,SumXY,SumX,SumY,Num_Entry);
		
	return delay;
}

void
msCalShutterParGet(
	UINT32 *time,
	UINT16 *delay
)
{
	*time=mSExpTime;
	*delay=mSDelay;
}

#endif
