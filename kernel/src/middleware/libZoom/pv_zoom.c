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
#include "pv_task.h"
#include "initio.h"
#include "app_still_view.h"
#include "sp1k_snap_api.h"
#include "snap_qtable.h"
#include "doslink.h"
#include "disp_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal.h"
#include "sp1k_pv_api.h"
#include "hal_dram.h" 
#include "pv_debug.h"
#include "utility.h"
#include "sp1k_disp_api.h"
#include "hal_disp.h"
#include "os_api.h"
#include "hal_global.h"
#include "front_logi.h"
#include "hal_front.h"
#include "app_video_view.h"
#include "sp1k_ae_api.h"
//#include "video_mem.h"
#include "sp1k_video_api.h"
#include "Reg_cdsp.h" 
#include "Reg_front.h" 
#include "app_video_view.h"
#include "sp1k_rsvblk_api.h"
#include "os_msg.h"
#include "sp1k_front_api.h"
#include "zoom_logi.h"
#include "pv_zoom.h"
#include "video_zoom.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_PV_ZOOM__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define TEST_PFUNC		0
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#define DEFAULT_2ND_PATH_YUV_H	320
#define DEFAULT_2ND_PATH_YUV_V	240

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appPvZoom_NBlocking_t xdata pvZoomNBCtrl = {
	100,							//current zoom factor
	400,							//max zoom factor in 4X
	100,							//min zoom factor in 1x
	41,								//max zoom step in 4X (input res lookup table end idx)
	0,								//min zoom step in 1x (input res lookup table start idx)
	0,								//current zoom step
	1,								//zoom interval
	PV_ZOOM_ACTION_INITIAL,			//zoom action
	PV_ZOOM_ACTION_INITIAL,			//zoom PreAction
	0,								//hQDzoomStatus
	0,								//hQDzoomFct
	0,								//hQDzoomStep
	PV_ZOOM_STATUS_FINISH,			//zoomStatus
	0,								//curLoadLog
	0,								//queueOffset
};

UINT8 G_PvStart;
UINT8 G_PvFreeze;

static UINT8 G_ZFUpdateFailFlag = 0;/*fan.xiao Add for mantis:0045941 @2012/9/25*/


extern UINT8 xdata G_FPGAMemPool[0x3000];

extern intrSmoothZoom xdata intrSmoothZoomCtrl;

UINT16 dzoomDispXZoomFactor = 100;
UINT16 dzoomDispYZoomFactor = 100;

extern appPvZoom_t xdata PvZoomCtrl;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 HAL_GlobalICVerGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if (TEST_PFUNC == 1)
UINT8 FUNC_A (
	UINT32 *pArray
) 
{
	printf("FUNCA=%lx,%lx,%lx\n",(UINT32)pArray[0],(UINT32)pArray[1],(UINT32)pArray[2]);
	return SUCCESS;
}

UINT8 FUNC_B (
	UINT32 *pArray
) 
{
	printf("FUNCB=%lx,%lx,%lx\n",(UINT32)pArray[0],(UINT32)pArray[1],(UINT32)pArray[2]);
	return SUCCESS;
}
#endif

/**
 * @fn		  UINT8 pvZoomDevOpen(	UINT8 type)
 * @brief			pv Zoom Dev Open
 * @param	   [in] type : typedef enum ZOOM_LOGI_TYPE
 * @retval		 return = SUCCESS / FAIL
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT8 
pvZoomDevOpen(
	UINT8 type
)
{
	UINT8 sts, actDevNum;
	zoomOperatingFunc_t dev;

	switch ( type ) {
		case ZOOM_LOGI_BLOCKING_TYPE:
			#if (TEST_PFUNC == 1)
			dev.pfOpZoomResourceLoad = FUNC_A;
			#else
			dev.pfOpZoomResourceLoad		= pvZoomResLoad_Blocking;
			dev.pfOpZoomSet					= pvZoomSet_Blocking;
			dev.pfOpZoomFactorGet			= pvZoomFctGet_Blocking;
			dev.pfOpZoomCtrlParaSet			= pvZoomCtrlParaSet_Blocking;
			dev.pfOpZoomCtrlParaGet			= pvZoomCtrlParaGet_Blocking;
			dev.pfOpZoomSmoothSet			= pvSmoothZoomSet_Blocking;
			dev.pfOpZoomSmoothGet			= pvSmoothZoomGet_Blocking;
			dev.pfOpZoomSmoothStatusGet		= pvSmoothZoomStatusGet_Blocking;
			dev.pfOpZoomSmooth3APreSet		= pvZoom3APreSet_Blocking;
			dev.pfOpZoomSmooth3APostSet		= pvZoom3APostSet_Blocking;
			dev.pfOpZoomSmoothTeleSet		= pvTeleSet_Blocking;
			dev.pfOpZoomSmoothWideSet		= pvWideSet_Blocking;
			#endif
			break;
		case ZOOM_LOGI_NONBLOCKING_TYPE:
			#if (TEST_PFUNC == 1)
			dev.pfOpZoomResourceLoad = FUNC_B;
			#else
			dev.pfOpZoomResourceLoad		= pvZoomResLoad_NBlocking;
			dev.pfOpZoomSet					= pvZoomSet_NBlocking;
			dev.pfOpZoomFactorGet			= pvZoomFctGet_NBlocking;
			dev.pfOpZoomCtrlParaSet			= pvZoomCtrlParaSet_NBlocking;
			dev.pfOpZoomCtrlParaGet			= pvZoomCtrlParaGet_NBlocking;
			dev.pfOpZoomSmoothSet			= pvSmoothZoomSet_NBlocking;
			dev.pfOpZoomSmoothGet			= pvSmoothZoomGet_NBlocking;
			dev.pfOpZoomSmoothStatusGet		= pvSmoothZoomStatusGet_NBlocking;
			dev.pfOpZoomSmooth3APreSet		= pvZoom3APreSet_NBlocking;
			dev.pfOpZoomSmooth3APostSet		= pvZoom3APostSet_NBlocking;
			dev.pfOpZoomSmoothTeleSet		= pvTeleSet_NBlocking;
			dev.pfOpZoomSmoothWideSet		= pvWideSet_NBlocking;
			#endif
			break;
	}

	actDevNum = zoomDevOpen(&dev, sizeof(zoomOperatingFunc_t));

	if ( actDevNum == NULL ) {
		sts = FAIL;
	}
	else {
		sts = SUCCESS;
	}

	return sts;
}

/**
 * @fn		  UINT16 pvZoomCtrlParaGet_NBlocking(UINT8 item)
 * @brief	  still view zoom system crtrl parameter get (NBlocking)
 * @param	 [in] item : typedef enum PV_ZOOM_CTRL_PARA_ITEM
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16 
pvZoomCtrlParaGet_NBlocking(
	UINT8 item
)
{
	switch (item) {
		case PV_ZOOM_CTRL_PARA_CURRENT_FACTOR:
			return pvZoomNBCtrl.factor;
		case PV_ZOOM_CTRL_PARA_MAX_FACTOR:
			return pvZoomNBCtrl.MaxFct;
		case PV_ZOOM_CTRL_PARA_MIN_FACTOR:
			return pvZoomNBCtrl.MinFct;
		case PV_ZOOM_CTRL_PARA_MAX_LOG:
			return pvZoomNBCtrl.maxLog;
		case PV_ZOOM_CTRL_PARA_MIN_LOG:
			return pvZoomNBCtrl.minLog;
		case PV_ZOOM_CTRL_PARA_CURRENT_LOG:
			return pvZoomNBCtrl.curLog;
		case PV_ZOOM_CTRL_PARA_LOG_STEP:
			return pvZoomNBCtrl.logStep;
		default:
			ASSERT(0,1);
			break;
	}
}

/**
 * @fn		  UINT16 pvZoomCtrlParaGet_Blocking(UINT8 item)
 * @brief	  still view zoom system crtrl parameter get (Blocking)
 * @param	 [in] item : typedef enum PV_ZOOM_CTRL_PARA_ITEM
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16 
pvZoomCtrlParaGet_Blocking(
	UINT8 item
)
{
	item = item;
	
	return SUCCESS;
}

/**
 * @fn		  void pvZoomCtrlParaSet_NBlocking(UINT8 item,UINT16 value)
 * @brief	  still view zoom system crtrl parameter set (NBlocking)
 * @param	[in] item : typedef enum PV_ZOOM_CTRL_PARA_ITEM
 * @param     [in] value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void
pvZoomCtrlParaSet_NBlocking(
	UINT8 item,
	UINT16 value
)
{
	switch (item) {
		case PV_ZOOM_CTRL_PARA_CURRENT_FACTOR:
			pvZoomNBCtrl.factor = value;
			break;
		case PV_ZOOM_CTRL_PARA_MAX_FACTOR:
			pvZoomNBCtrl.MaxFct = value;
			break;
		case PV_ZOOM_CTRL_PARA_MIN_FACTOR:
			pvZoomNBCtrl.MinFct = value;
			break;
		case PV_ZOOM_CTRL_PARA_MAX_LOG:
			pvZoomNBCtrl.maxLog = value;
			break;
		case PV_ZOOM_CTRL_PARA_MIN_LOG:
			pvZoomNBCtrl.minLog = value;
			break;
		case PV_ZOOM_CTRL_PARA_CURRENT_LOG:
			pvZoomNBCtrl.curLog = value;
			break;
		case PV_ZOOM_CTRL_PARA_LOG_STEP:
			pvZoomNBCtrl.logStep = (UINT8)value;
			break;
		default:
			ASSERT(0,1);
			break;
	}
}

/**
 * @fn		  void pvZoomCtrlParaSet_Blocking(UINT8 item,UINT16 value)
 * @brief	  still view zoom system crtrl parameter set (Blocking)
 * @param	[in] item : typedef enum PV_ZOOM_CTRL_PARA_ITEM
 * @param     [in] value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void
pvZoomCtrlParaSet_Blocking(
	UINT8 item,
	UINT16 value
)
{
	item = item;
	value = value;
	//return SUCCESS;
}

/**
 * @fn        UINT8 pvZoomResLoad_NBlocking(UINT32 *pArray)
 * @brief     load still view zoom parameter resource file (NBlocking)
 * @param    [in] pArray : zoomFactorTbl = pArray[0]
 *                                 regTbl            = pArray[1]
 *                                 regValueTbl     = pArray[2]
 * @retval    NONE
 * @author    LinJieCheng
 * @since     2010-10-07
*/
UINT8
pvZoomResLoad_NBlocking(
	UINT32 *pArray
) 
{
	UINT8 lsb,msb;
	//UINT16 resLen;
	UINT32 size,i;
	UINT8 actId,hQDzoomFctGet = 0;
	UINT16 para1,fact=0;
	UINT32 zoomFactorTbl = pArray[0];
	UINT32 regTbl = pArray[1];
	UINT32 regValueTbl = pArray[2];

	//printf("FUNC=%lx,%lx,%lx\n",(UINT32)pArray[0],(UINT32)pArray[1],(UINT32)pArray[2]);

	actId = frontDevActGet();

	pvSmoothZoomSysInfo("pvZoomResLoad\n");

	/* zoom factor table */
	if ( sp1kNandRsvRead(zoomFactorTbl, (K_SDRAM_PvZoomFactorAddr<< 1)) ) {

		sp1kNandRsvSizeGet(zoomFactorTbl, NULL, &size);
		
		frontDeviceInfoGet(actId,FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,&para1,NULL);

		pvZoomNBCtrl.maxLog = (UINT16)((size/2)-1);

		if (para1 != 0) {
		
			M_DRAM_SetStartAddress((UINT32)K_SDRAM_PvZoomFactorAddr, 1);

			for (i=0; i<size; i+=2){
				M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)
				//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);

				WRITE8(fact,0,msb);
				WRITE8(fact,1,lsb);
				
				if ((hQDzoomFctGet == 0) && (fact >= para1)) {
					pvZoomNBCtrl.hQDzoomFct = fact;
					pvZoomNBCtrl.hQDzoomStep = (UINT16)i/2;
					hQDzoomFctGet = 1;
					//printf("fact=%d,hQDzoomStep=%bu\n",fact,pvZoomNBCtrl.hQDzoomStep);
				}

				if (i == 0) {
					pvZoomNBCtrl.MinFct = fact;
				}
			}

			pvZoomNBCtrl.MaxFct = fact;
		}	
	}

	/* reg table */
	if ( sp1kNandRsvSizeGet(regTbl, NULL, &intrSmoothZoomCtrl.zoomRegSize) ) {
		if ( sp1kNandRsvRead(regTbl, (K_SDRAM_PvZoomRegTblAddr<< 1)) ) {
			
			//M_DRAM_SetStartAddress((UINT32)K_SDRAM_PvZoomRegTblAddr, 1);

			//M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)

			//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);
		}
	}
	else {
		intrSmoothZoomCtrl.zoomRegSize = 0;
	}

	/* reg value */
	if ( sp1kNandRsvRead(regValueTbl, (K_SDRAM_PvZoomRegValAddr<< 1)) ) {
	
		//M_DRAM_SetStartAddress((UINT32)K_SDRAM_PvZoomRegValAddr, 1);

		//M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)

		//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 pvZoomResLoad_Blocking(UINT32 *pArray)
 * @brief     load still view zoom parameter resource file (Blocking)
 * @param    [in] pArray : zoomFactorTbl = pArray[0]
 *                                 regTbl            = pArray[1]
 *                                 regValueTbl     = pArray[2]
 * @retval    NONE
 * @author    LinJieCheng
 * @since     2010-10-07
*/
UINT8
pvZoomResLoad_Blocking(
	UINT32 *pArray
) 
{
	pArray = pArray;
	
	return SUCCESS;
}

/**
 * @fn        UINT8 pvZoomSet_Blocking(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze)
 * @brief     Set zoom factor in preview mode. (Blocking)
 * @param     [in] pvStart :  1: preview is working  0: preview is off
 * @param     [in] factor :  100:1x , 200: 2x , 250: 2.5x
 * @param     [in] pbFreeze  : 0: It shows image immedaitely.
 *\n						   1: It doesn't show image 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
pvZoomSet_Blocking(
	UINT8 pvStart, 
	UINT16 factor, 
	UINT8 pbFreeze
) 
{
	UINT16 maxHFac,maxVFac;
	UINT16 hSize,vSize;
    UINT16 cropHSize, cropVSize;
	UINT16 secPathVsize;
	UINT16 yuvHSize,yuvVSize;
	UINT8 vidMode;
	UINT16 cdspZFactor,dispZFactor,sensorZfactor;
	UINT8 frameRate;
	//UINT8 ZfStatus;
	//UINT32 timer1,timer2,timer3;

	//HAL_GlobalReadGTimer(&timer1);

	//printf("pv zoom\n");
	
	vidMode = osSysModeGet();
	
	/* hold AWB window */
	HAL_CdspAWBWinEn( 1, 1, 0, 0 ,0); 

	/* hold AE window */
	HAL_CdspAEWinHoldSet(1);

	/* get frame rate mode */
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
	//printf("framerate=%bx\n",frameRate);
	
	pvSizeGet(&hSize, &vSize);
	//printf("hSize=%d,vSize=%d\n",hSize, vSize);
	
    //pvClampSizeGet(&clampW, &clampH);
    
    sp1kPreviewCropSizeGet(&cropHSize,&cropVSize);
	//pvZoomSetPrintf("cropHSize=%d,cropVSize=%d\n",cropHSize, cropVSize);

	/* set still system max cdsp zfactor */
	#if (VGA_60FPS_2ST_PATH_EN == 1)
	if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac_VGA_60FPS,VideoCDSPMaxFac_VGA_60FPS,1);
	}
	else{
		HAL_CdspMaxZFactorSet(StillCDSPMaxFac,StillCDSPMaxFac,1);
	}
	#else
	HAL_CdspMaxZFactorSet(StillCDSPMaxFac,StillCDSPMaxFac,1);
	#endif
	
	HAL_CdspMaxZFactorGet(&maxHFac,&maxVFac);
	//printf("mhf=%d,mvf=%d\n",maxHFac, maxVFac);
	
	//HAL_CdspMaxZFactorSet(maxHFac,maxVFac,1);
	
	maxHFac = (maxHFac>maxVFac) ? maxVFac : maxHFac;	
	pvZoomSetPrintf("maxHFac=%d\n",maxHFac);	

	/* get sensor zoom factor */
	frontDeviceInfoGet(frontDevActGet(),FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,&sensorZfactor,NULL);
	//printf("sZf=%d\n",sensorZfactor);

	if (sensorZfactor != 0) {
		/* sensor digital zoom */
		if (SUCCESS == frontPreviewSensorZoomModeSet(frontDevActGet(),factor)) {
			if (factor >= sensorZfactor) {
				factor = factor*100/sensorZfactor;
				//printf("sf=%d\n",factor);
			}
		}
	}

	if(factor> maxHFac){		
		dispZFactor = (UINT32)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		cdspZFactor = maxHFac;
		//pvZoomSetPrintf("dispZFactor=%d\n",dispZFactor);
	}
	else{
		dispZFactor = 100;
		cdspZFactor = factor;
	}
	//printf("factor=%d, cdspZFactor=%d, dispZFactor=%d\n",factor,cdspZFactor,dispZFactor);

	//HAL_GlobalReadGTimer(&timer2);
	
	//pvZoomSetPrintf("zoom time cal =%ld\n",timer2-timer1);
	
	/***********************************************************/
	/* wait for zf done and write new factors to registers then*/
	/***********************************************************/

	if (!pvStart)  {
		//pvZoomSetPrintf("wait zf\n");
		//ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300);
		while(HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300)){
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);	
		}
		
		//while((XBYTE[REG_Dram_Cdsp1ZfUpdateDone]&0x30)!=0x30);
	}

	/*
	if (ZfStatus == FAIL) {
		pvZoomSetPrintf("zf failed\n");
		return FAIL;		
	}
	*/

	/* check cdspZFactor > maxHFac */
	if(cdspZFactor <= maxHFac) { 	/*  TODO: reverse display/CDSP calling sequence will crash?? */		
		
		HAL_Cdsp1stPathZFactorSet(SYNCZF,cdspZFactor,cdspZFactor,hSize, vSize, cropHSize, cropVSize, dispZFactor, dispZFactor); /* sync with zoomfactor update */
		HAL_Cdsp1stPathYuvSizeGet(&yuvHSize, &yuvVSize);
		
		if (vidMode == OS_SYS_MODE_STILL_PV){                        /* still mode , no scale in second YUV scale engine */
			pvZoomSetPrintf("s yuv h=%d,v=%d\n",yuvHSize,yuvVSize);			
			secPathVsize = (UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 

			/*printf("\033[32m2Path V(%u)\033[0m\n",secPathVsize);*/

			HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,yuvHSize,DEFAULT_2ND_PATH_YUV_H);   /* set sencond path yuv h scale factor */
			HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,/*DEFAULT_2ND_PATH_YUV_V*/secPathVsize);   /* set sencond path yuv V scale factor */   

			if (sp1kSuperPreviewStatusGet(0) == 1) {

				HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);
				
				HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */

				dispFitShow(yuvHSize,yuvVSize,0);

				HAL_Cdsp2ndPathClampHSize(1,DEFAULT_2ND_PATH_YUV_H);
			}
			else {
				if( pbFreeze == 0 ) {
					//HAL_DispScaleSet(DEFAULT_2ND_PATH_YUV_H, vSize, dispZFactor, 0); 
					HAL_DispScaleSet(DEFAULT_2ND_PATH_YUV_H, /*DEFAULT_2ND_PATH_YUV_V*/secPathVsize-1, dispZFactor, 0); /* DEFAULT_2ND_PATH_YUV_V-1 to workaround 2nd Path last line error */
				}
			
				//dispFitShow(DEFAULT_2ND_PATH_YUV_H,vSize,0);
				dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVsize,0);

				//HAL_Cdsp2ndPathCfg(1,0,1);			
				//HAL_Cdsp2ndPathEnSet(0);
				HAL_Cdsp2ndPathClampHSize(1,DEFAULT_2ND_PATH_YUV_H);
				//HAL_Cdsp2ndPathCfg(0,0,0);			
				//HAL_Cdsp2ndPathEnSet(1);
			}
			
		}
		else{	    	                          /* video mode */
			aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
			HAL_Cdsp1stPathYuvSizeGet(&yuvHSize, &yuvVSize); 		
			#if 0
			#if FRAME_BUF_NUM==3
				//pvZoomSetPrintf("yuvH=%d,yuvV=%d\n",yuvHSize,yuvVSize);
				if( pbFreeze == 0 ) {					
					HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);
					HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor,0); 	/* secPathVsize-1 to workaround 2nd Path last line error */					
				}
				secPathVsize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);	
				dispFitShow(yuvHSize,yuvVSize,0);
				
				//HAL_Cdsp2ndPathCfg(1,0,1);
				//HAL_Cdsp2ndPathEnSet(0);			
			#else
				#if (VGA_60FPS_2ST_PATH_EN == 0)
				if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){	
					//printf("VGA 60fps h=%d,v=%d\n",hSize,vSize);
					if( pbFreeze == 0 ) {
						HAL_DispScaleSet(hSize, vSize, dispZFactor, 0); 
					}	

					dispFitShow(hSize,vSize,0);
				
				}
				else
				#endif
				{
					HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,yuvHSize,320);				
					secPathVsize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1); 
					
					if (secPathVsize>240){
						HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,240);		
					}
					else{
						HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,secPathVsize);						
					}	
								
					if( pbFreeze == 0 ) {
						HAL_DispScaleSet(320, secPathVsize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */
					}	

					dispFitShow(320,secPathVsize,0);
					
					HAL_Cdsp2ndPathClampHSize(1,320);
					//HAL_Cdsp2ndPathCfg(0,0,1);
					//HAL_Cdsp2ndPathEnSet(1);				
				}
			
				
			#endif
			#endif
			 #if 1
			if(sp1kVideoRecFrameBufNumGet()==3){
				//pvZoomSetPrintf("yuvH=%d,yuvV=%d\n",yuvHSize,yuvVSize);
					#if 0
					if( pbFreeze == 0 ) {					
						HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);
						HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor,0); 	/* secPathVsize-1 to workaround 2nd Path last line error */					
					}
					secPathVsize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);	
					dispFitShow(yuvHSize,yuvVSize,0);
					#else
						HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,yuvHSize,DEFAULT_2ND_PATH_YUV_H);				
						secPathVsize=(UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 
						
						if (secPathVsize>DEFAULT_2ND_PATH_YUV_V){
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,DEFAULT_2ND_PATH_YUV_V);		
						}
						else{
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,secPathVsize);						
						}	
									
						if( pbFreeze == 0 ) {
							HAL_DispScaleSet(DEFAULT_2ND_PATH_YUV_H, secPathVsize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */
						}	

						dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVsize,0);
						
						HAL_Cdsp2ndPathClampHSize(1,DEFAULT_2ND_PATH_YUV_H);
					#endif
				}
			else{
				#if (VGA_60FPS_2ST_PATH_EN == 0)
				if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){	
					//printf("VGA 60fps h=%d,v=%d\n",hSize,vSize);
					if( pbFreeze == 0 ) {
						HAL_DispScaleSet(hSize, vSize, dispZFactor, 0); 
					}	

					dispFitShow(hSize,vSize,0);				
				}
				else
				#endif
				{
					if (sp1kSuperPreviewStatusGet(1) == 1) {
						/*//rf++ 
						HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,yuvHSize,320);				
						secPathVsize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1); 
						
						if (secPathVsize>240){
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,240);		
						}
						else{
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,secPathVsize);						
						}	*/	
						//rf++end

						HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);
						
						HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */

						dispFitShow(yuvHSize,yuvVSize,0);

						HAL_Cdsp2ndPathClampHSize(1,DEFAULT_2ND_PATH_YUV_H);
					}
					else {
						HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,yuvHSize,DEFAULT_2ND_PATH_YUV_H);				
						secPathVsize=(UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 
						
						if (secPathVsize>DEFAULT_2ND_PATH_YUV_V){
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,DEFAULT_2ND_PATH_YUV_V);		
						}
						else{
							HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,yuvVSize,secPathVsize);						
						}	
									
						if( pbFreeze == 0 ) {
							HAL_DispScaleSet(DEFAULT_2ND_PATH_YUV_H, secPathVsize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */
						}	

						dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVsize,0);
						
						HAL_Cdsp2ndPathClampHSize(1,DEFAULT_2ND_PATH_YUV_H);
						//HAL_Cdsp2ndPathCfg(0,0,1);
						//HAL_Cdsp2ndPathEnSet(1);
					}	
				}
			}
			#endif
			pvZoomSetPrintf("2nd Path Vsize=%d\n",secPathVsize);

		}		 
		
		sp1kSnapDzoomFactorSet( cdspZFactor, cdspZFactor );
		dzoomDispXZoomFactor = dzoomDispYZoomFactor = cdspZFactor;
	}
	else{
		maxVFac = (UINT16)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		
		HAL_Cdsp1stPathZFactorSet(SYNCZF,maxHFac,maxHFac,hSize, vSize, cropHSize, cropVSize, maxVFac, maxVFac); /* sync with zoomfactor update */
		
        HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,cropHSize,DEFAULT_2ND_PATH_YUV_H);  /* set sencond path yuv h scale factor */
		HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,cropVSize,DEFAULT_2ND_PATH_YUV_V);  /* set sencond path yuv V scale factor */

		if( pbFreeze == 0 ) {
			HAL_DispScaleSet(hSize, vSize-1, maxVFac,0); /* vsize-1 to workaround 2nd Path last line error */
		}
		sp1kSnapDzoomFactorSet( maxVFac, maxVFac );
		dzoomDispXZoomFactor = dzoomDispYZoomFactor = maxVFac;
	}

	HAL_DramZUpdateClear(0x30); /* clear CDSP/DISP zf flag */
	

	
	HAL_FrontSignalWait(FRONT_WAIT_VVALID,FRONT_WAIT_FALLING,1); 
	HAL_CdspVDramSizeClamp( 1, yuvHSize, 1, yuvVSize, 1 );	
	//HAL_CdspDarkTDSizeSet(yuvHSize);
	
	pvZoomSetPrintf("zf update\n");		
	
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);


	//HAL_GlobalReadGTimer(&timer3);
	//pvZoomSetPrintf("zoom time wait z =%ld\n",timer3-timer2);
	
	HAL_CdspAWBWinEn( 1, 0, 0, 1 ,0); /* unhold window */
	HAL_CdspAEWinHoldSet(0);

	return SUCCESS;
}

/**
 * @fn        void pvSmoothZoomSyncRegSet(void) 
 * @brief     Set Smooth Zoom Sync Reg
 * @param    N/A
 * @retval    N/A
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
void 
pvSmoothZoomSyncRegSet(
	void
) 
{

	/* dzoom sync setting */

	/*----------------------------------------------------------------------*/
	/*												 cropping				*/
	/*----------------------------------------------------------------------*/
	
	/* 1st crop size enable */
	XBYTE[ REG_Cdsp_Cropen ] = 0x23;//h,v crop enable; crop sync zfupdate

	/*----------------------------------------------------------------------*/
	/*												 RGB scale				*/
	/*----------------------------------------------------------------------*/
	
	/* disable RGB H Scale factor =0 */
	XBYTE[ REG_Cdsp_HRGBscalefactor_low ]  = 0;
	XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = 0;			
	XBYTE[ REG_Cdsp_HRGBscaleinieve_low ]  = 0;
	XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = 0;			
	XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ]  = 0;
	XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = 0;
	
	/*----------------------------------------------------------------------*/
	/*												  1st path				*/
	/*----------------------------------------------------------------------*/

	/* 1st path enable YUV H,V scale */
	XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;//filter[4]:1, drop[4]:0
	XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;//filter[4]:1, drop[4]:0

	/* 1st sync zfupdate */
	XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x10;//ref zfupdate

	/*----------------------------------------------------------------------*/
	/*												  VDRAM 1 clamp				*/
	/*----------------------------------------------------------------------*/
	
	XBYTE[ REG_Cdsp_ClampSizeSvd ] = 0x01;
	
	/* 1st clamp H offset */
	XBYTE[ REG_Cdsp_Whoffset_low ] = 0x00;
	XBYTE[ REG_Cdsp_Whoffset_high ] = 0x00;

	/* 1st clamp V offset */
	XBYTE[ REG_Cdsp_Wvoffset_low ] = 0x00;
	XBYTE[ REG_Cdsp_Wvoffset_high ] = 0x00;

	/* 1st vdram clamp setting */
	XBYTE[ REG_Cdsp_clamphsizeen ] = 0x01;

	/*----------------------------------------------------------------------*/
	/*												  2st path				*/
	/*----------------------------------------------------------------------*/
	/* 2st path enable YUV H,V scale */
	XBYTE[ REG_Cdsp_YUVhscaleen2 ] = 0x11;//filter[4]:1, drop[4]:0
	XBYTE[ REG_Cdsp_YUVvscaleen2 ] = 0x11;//filter[4]:1, drop[4]:0

	/* 2st sync zfupdate */
	XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x10;//ref zfupdate

	/*----------------------------------------------------------------------*/
	/*												  VDRAM 2 clamp				*/
	/*----------------------------------------------------------------------*/

	/* 2st clamp H offset */
	XBYTE[ REG_Cdsp_whoffset1_low ] = 0x00;
	XBYTE[ REG_Cdsp_whoffset1_high ] = 0x00;

	/* 2st clamp V offset */	
	XBYTE[ REG_Cdsp_wvoffset1_low ] = 0x00;
	XBYTE[ REG_Cdsp_wvoffset1_high ] = 0x00;

	/* 2st vdram clamp setting */
	XBYTE[ REG_Cdsp_clamphsizeen1	 ] = 0x01;
	
}

/**
 * @fn        UINT8 pvZoomSet_NBlocking(UINT8 pvStart, UINT16 factor, UINT8 pbFreeze)
 * @brief     Set zoom factor in preview mode. (NBlocking)
 * @param     [in] pvStart :  1: preview is working  0: preview is off
 * @param     [in] factor :  100:1x , 200: 2x , 250: 2.5x
 * @param     [in] pbFreeze  : 0: It shows image immedaitely.
 *\n						   1: It doesn't show image 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
pvZoomSet_NBlocking(
	UINT8 pvStart, 
	UINT16 factor, 
	UINT8 pbFreeze
) 
{
	UINT16 maxHFac,maxVFac;
	UINT16 hSize,vSize;
	//UINT16 cropHSize, cropVSize;
	UINT16 secPathVSize;
	UINT16 yuvHSize,yuvVSize;
	UINT8 vidMode;
	UINT16 cdspZFactor,dispZFactor;
	//UINT8 yuvSensor;
	UINT16 location = 0,log;
	UINT32 addrOffSet;
	UINT16 xdata* pRegIdx;
	UINT8 xdata* pRegVal;
	SINT16 idx;
	extern UINT8 dzoomBuf[];
	//UINT16 i;

	UINT8 ZfStatus,num,sts;

	vidMode = osSysModeGet();

	factor = factor;
	
	G_PvStart = pvStart;
	G_PvFreeze = pbFreeze;	

	/*----------------------------------------------------------------------*/
	/*                                                zoomAction                                                 */
	/*----------------------------------------------------------------------*/
	
	switch (pvZoomNBCtrl.zoomAction) {
		case PV_ZOOM_ACTION_TELE_ONE:
			pvZoomNBCtrl.zoomAction = PV_ZOOM_ACTION_INITIAL;
			/* zoom in */

			idx = pvZoomNBCtrl.curLog + pvZoomNBCtrl.logStep;

			if ( idx <= pvZoomNBCtrl.maxLog ) {
				log = (UINT16)idx;
			}
			else {
				log = pvZoomNBCtrl.maxLog;
			}
			pvZoomNBCtrl.curLog = log;
			break;
		case PV_ZOOM_ACTION_WIDE_ONE:
			pvZoomNBCtrl.zoomAction = PV_ZOOM_ACTION_INITIAL;
			/* zoom out */

			idx = pvZoomNBCtrl.curLog - pvZoomNBCtrl.logStep;
		
			if ( idx >= pvZoomNBCtrl.minLog ) {
				log = (UINT16)idx;
			}
			else {
				log = pvZoomNBCtrl.minLog;
			}
			pvZoomNBCtrl.curLog = log;
			break;
		default:
			break;
	}
	
	switch (pvZoomNBCtrl.zoomAction) {
		case PV_ZOOM_ACTION_INITIAL:
			/* zoom initial */
			
			/* smooth zoom queue initial */
			zoomSmoothQueueInit(ZOOM_CTRL_TYPE_STILL);

			/* wait zf update */
			ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 300);
			
			/* reg initial */
			pvSmoothZoomSyncRegSet();
			
			/* read reg resource */
			HAL_DramDmaRead(dzoomBuf, K_SDRAM_PvZoomRegTblAddr, intrSmoothZoomCtrl.zoomRegSize, 1);
			
			pvZoomNBCtrl.zoomPreAction = pvZoomNBCtrl.zoomAction;
			break;
		case PV_ZOOM_ACTION_TELE:
			/* zoom in */
			if ((pvZoomNBCtrl.zoomPreAction != PV_ZOOM_ACTION_INITIAL) &&
				(pvZoomNBCtrl.zoomPreAction != pvZoomNBCtrl.zoomAction)) {
				/* zoom queue initial */
				ENTER_CRITICAL(sts);
				pvZoomNBCtrl.curLoadLog= pvZoomNBCtrl.curLog;
				/* zoom queue initial */
				zoomSmoothQueueInit(ZOOM_CTRL_TYPE_STILL);
				EXIT_CRITICAL(sts);
			}
			break;
		case PV_ZOOM_ACTION_WIDE:
			/* zoom out */
			if ((pvZoomNBCtrl.zoomPreAction != PV_ZOOM_ACTION_INITIAL) &&
				(pvZoomNBCtrl.zoomPreAction != pvZoomNBCtrl.zoomAction)) {
				/* zoom queue initial */
				ENTER_CRITICAL(sts);
				pvZoomNBCtrl.curLoadLog= pvZoomNBCtrl.curLog;
				/* zoom queue initial */
				zoomSmoothQueueInit(ZOOM_CTRL_TYPE_STILL);
				EXIT_CRITICAL(sts);
			}
			break;
		default:
			//printf("action=%bu\n",pvZoomNBCtrl.zoomAction);
			ASSERT(0,0);
	}
	
	pvZoomNBCtrl.zoomPreAction = pvZoomNBCtrl.zoomAction;
	
	if (((pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_TELE) && (pvZoomNBCtrl.curLoadLog != pvZoomNBCtrl.maxLog)) ||
		((pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_WIDE) && (pvZoomNBCtrl.curLoadLog != pvZoomNBCtrl.minLog)) ||
		(pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_INITIAL)) {
			
		zoomSmoothQueueEntriesGet(ZOOM_CTRL_TYPE_STILL,&num);
		
		pvSmoothZoomSysInfo("still action=%bu,%bu\n",pvZoomNBCtrl.zoomAction,num);
	
		while (ZOOM_BUF_MAX > num) {
			switch (pvZoomNBCtrl.zoomAction) {
				case PV_ZOOM_ACTION_INITIAL:
					/* zoom initial */

					log = pvZoomNBCtrl.curLog;
					addrOffSet = ((UINT32)pvZoomNBCtrl.curLog * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);
					
					pvZoomNBCtrl.queueOffset = 0;
			
					break;
				case PV_ZOOM_ACTION_TELE:
					/* zoom in */

					idx = pvZoomNBCtrl.curLoadLog + pvZoomNBCtrl.logStep;

					if ( idx <= pvZoomNBCtrl.maxLog ) {
						log = (UINT16)idx;
					}
					else {
						log = pvZoomNBCtrl.maxLog;
					}

					addrOffSet = ((UINT32)log * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);

					pvZoomNBCtrl.queueOffset++;

					break;
				case PV_ZOOM_ACTION_WIDE:
					/* zoom out */

					idx = pvZoomNBCtrl.curLoadLog - pvZoomNBCtrl.logStep;
				
					if ( idx >= pvZoomNBCtrl.minLog ) {
						log = (UINT16)idx;
					}
					else {
						log = pvZoomNBCtrl.minLog;
					}
				
					addrOffSet = ((UINT32)log * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);

					pvZoomNBCtrl.queueOffset++;
					break;
				default:
					ASSERT(0,0);
			}
			
			pvZoomNBCtrl.queueOffset = (pvZoomNBCtrl.queueOffset % ZOOM_BUF_MAX);
			pvSmoothZoomDbg("[%d],curLog=%d,curLoadLog=%d,nextLoadLog=%d\n",pvZoomNBCtrl.queueOffset,pvZoomNBCtrl.curLog,pvZoomNBCtrl.curLoadLog,log);
			pvSmoothZoomDbg("addrOffSet=%lu,%lx\n",addrOffSet,(intrSmoothZoomCtrl.zoomRegSize >> 1) * pvZoomNBCtrl.queueOffset);
			
			HAL_DramDmaRead(dzoomBuf + intrSmoothZoomCtrl.zoomRegSize + ((intrSmoothZoomCtrl.zoomRegSize >> 1)*pvZoomNBCtrl.queueOffset), K_SDRAM_PvZoomRegValAddr + addrOffSet, (intrSmoothZoomCtrl.zoomRegSize >> 1) * (UINT32)1, 0);
			
			if (pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_INITIAL) {

				pvZoomNBCtrl.curLoadLog = pvZoomNBCtrl.curLog;
				
				break;
			}
			
			/* Queue Post */
			zoomSmoothQueuePost(ZOOM_CTRL_TYPE_STILL,(intrSmoothZoomCtrl.zoomRegSize >> 1) * pvZoomNBCtrl.queueOffset);
			
			zoomSmoothQueueEntriesGet(ZOOM_CTRL_TYPE_STILL,&num);
			
			if (pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_TELE) {
				pvZoomNBCtrl.curLoadLog = log;
				if (pvZoomNBCtrl.curLoadLog == pvZoomNBCtrl.maxLog) {
					pvSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,pvZoomNBCtrl.curLoadLog);
					break;
				}
			}
			else if (pvZoomNBCtrl.zoomAction == PV_ZOOM_ACTION_WIDE) {
				pvZoomNBCtrl.curLoadLog = log;
				if (pvZoomNBCtrl.curLoadLog == pvZoomNBCtrl.minLog) {
					pvSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,pvZoomNBCtrl.curLoadLog);
					break;
				}
			}
			
			pvSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,pvZoomNBCtrl.curLoadLog);
		}
	}

	/*----------------------------------------------------------------------*/
	/*                                                zoom timing ctrl                                           */
	/*----------------------------------------------------------------------*/

	switch ( pvZoomNBCtrl.zoomAction ) {
		case PV_ZOOM_ACTION_INITIAL:
			
			pvSmoothZoomSysInfo("pvzoom set reg\n");
			
			pRegIdx = (UINT16*)&dzoomBuf[0];
			pRegVal = &dzoomBuf[intrSmoothZoomCtrl.zoomRegSize];
			
			while(1) {
				if (*pRegIdx == 0xffff) {
					break;
				}
				else if ( *pRegIdx == REG_Cdsp_Clamphsize_low ) {
					XBYTE[*pRegIdx] = *pRegVal;
					XBYTE[REG_Cdsp_3Dsnapsubwidth_low] = *pRegVal;
					WRITEP8(&intrSmoothZoomCtrl.zoomClampH,1,*pRegVal);
				}
				else if ( *pRegIdx == REG_Cdsp_Clamphsize_high) {
					XBYTE[*pRegIdx] = *pRegVal;
					XBYTE[REG_Cdsp_3Dsnapsubwidth_high] = *pRegVal;
					WRITEP8(&intrSmoothZoomCtrl.zoomClampH,0,*pRegVal);
				}
				else if ( *pRegIdx == REG_Cdsp_Clampvsize_low ) {
					XBYTE[*pRegIdx] = *pRegVal;
					WRITEP8(&intrSmoothZoomCtrl.zoomClampV,1,*pRegVal);
				}
				else if ( *pRegIdx == REG_Cdsp_Clampvsize_high ) {
					XBYTE[*pRegIdx] = *pRegVal;
					WRITEP8(&intrSmoothZoomCtrl.zoomClampV,0,*pRegVal);
				}
				else {
					//printf("0x%x=0x%bx\n",*pRegIdx,*pRegVal);
					XBYTE[*pRegIdx] = *pRegVal;
				}

				pRegIdx++;
				pRegVal++;
			}

			#if 0	//move to intr reg-setting flow
			HAL_CdspDarkTDSizeSet(intrSmoothZoomCtrl.ZoomClampH);
			//printf("yuv hsize=%d\n",yuvHSize);
			#endif	
			
			pvSizeGet(&hSize, &vSize);
			//printf("%d,%d\n",hSize, vSize);

			HAL_Cdsp1stPathYuvSizeGet(&yuvHSize, &yuvVSize);

			/* get max h,v cdsp zoom factor */
			HAL_CdspMaxZFactorGet(&maxHFac, &maxVFac);
			maxHFac = (maxHFac>maxVFac) ? maxVFac : maxHFac;
			//printf("mhf=%d\n",maxHFac);
			
			if ( factor > maxHFac ) {		
				dispZFactor = (UINT32)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
				cdspZFactor = maxHFac;
			}
			else {
				dispZFactor = 100;
				cdspZFactor = factor;
			}
			//printf("factor=%d, cdspZFactor=%d, dispZFactor=%d\n",factor,cdspZFactor,dispZFactor);

			if (cdspZFactor <= maxHFac) {
				if (vidMode == OS_SYS_MODE_STILL_PV){
					if (sp1kSuperPreviewStatusGet(0) == 1) {
						dispFitShow(intrSmoothZoomCtrl.zoomClampH,intrSmoothZoomCtrl.zoomClampV,0);
					}
					else {

						secPathVSize = (UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 
						//printf("secPathVsize=%d\n",secPathVSize);
						
						if( pbFreeze == 0 ) {
							//HAL_DispScaleSet(320, vSize, dispZFactor, 0);
							//HAL_DispScaleSet(320, /*DEFAULT_2ND_PATH_YUV_V*/secPathVSize-1, dispZFactor, 0); /* DEFAULT_2ND_PATH_YUV_V-1 to workaround 2nd Path last line error */
						}

						dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVSize,0);
					}
				}
				else {
					if (sp1kVideoRecFrameBufNumGet() == 3) {
						#if 0
						dispFitShow(intrSmoothZoomCtrl.zoomClampH,intrSmoothZoomCtrl.zoomClampV,0);
						#else
						/*disp data source from 2nd path*/
						secPathVSize=(UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 
						dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVSize,0);
						#endif
					}
					else {
						if (sp1kSuperPreviewStatusGet(1) == 1) {
							dispFitShow(intrSmoothZoomCtrl.zoomClampH,intrSmoothZoomCtrl.zoomClampV,0);
						}
						else {
							printf("@@PV_NB\n");
							secPathVSize=(UINT16)((((UINT32)vSize*DEFAULT_2ND_PATH_YUV_H/((UINT32)hSize))>>1)<<1); 
							dispFitShow(DEFAULT_2ND_PATH_YUV_H,secPathVSize,0);
						}
					}
				}
			}
			else {
				ASSERT(0,1);
			}
			
			/* clear CDSP/DISP zf flag */
			HAL_DramZUpdateClear(0x30); 
			
			/* zf update trigger */
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);
			
			/* zf update wait done */
			ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 300);
			if(ZfStatus == FAIL){	/*fan.xiao Add for mantis:0045941 @2012/9/25*/
				G_ZFUpdateFailFlag = 1;
			}
			
			/* 3a window update */
			osMsgPost(SP1K_MSG_DZOOM_3A_UPDATE);
			
			pvZoomNBCtrl.queueOffset = 0;
			break;
		case PV_ZOOM_ACTION_TELE:
		case PV_ZOOM_ACTION_WIDE:
			if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE) {
				pvSmoothZoomDbg("smooth dis\n");
				intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_START;
			}
			break;
	}
	
	return SUCCESS;
}

/**
 * @fn		  UINT16 pvZoomFctGet_NBlocking(void)
 * @brief		still zoom facter get (NBlocking)
 * @param	NONE
 * @retval	zoom facter
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT16 
pvZoomFctGet_NBlocking(
	void
)
{
	UINT8 lsb,msb;
	UINT16 factor;

	//printf("curlog=%d\n",pvZoomNBCtrl.curLog);
	
	M_DRAM_SetStartAddress((UINT32)K_SDRAM_PvZoomFactorAddr+(UINT32)pvZoomNBCtrl.curLog, 1);
	
	M_DRAM_ReadWord(lsb, msb);//res header format : (lsb, msb)

	//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);

	factor = ((UINT16)((UINT16)msb<<8)|(UINT16)lsb);

	if ( factor >= pvZoomNBCtrl.MaxFct ) {
		pvZoomNBCtrl.factor = pvZoomNBCtrl.MaxFct;
		return pvZoomNBCtrl.MaxFct;
	}
	else {
		pvZoomNBCtrl.factor = factor;
		return factor;
	}

}

/**
 * @fn		  UINT16 pvZoomFctGet_Blocking(void)
 * @brief		still zoom facter get (Blocking)
 * @param	NONE
 * @retval	zoom facter
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT16 
pvZoomFctGet_Blocking(
	void
)
{
	return PvZoomCtrl.factor;
}

/**
 * @fn		  UINT8 pvWideSet_NBlocking(void)
 * @brief	        still wide set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvWideSet_NBlocking(
	void
)
{
	//SINT16 idx;
	UINT8 ZfStatus;

	if ( pvZoomNBCtrl.curLog <= pvZoomNBCtrl.minLog) {
		pvSmoothZoomDbg("host curLog=%d\n",pvZoomNBCtrl.curLog);
		return FAIL;
	}

	if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE) {
		pvZoomNBCtrl.zoomAction= PV_ZOOM_ACTION_WIDE;
	}
	else {
		pvZoomNBCtrl.zoomAction= PV_ZOOM_ACTION_WIDE_ONE;
	}

	//ZfStatus = pvZoomSet(0, pvZoomCtrl.factor, 0);
	ZfStatus=zoomDevZoomSet(ZOOM_LOGI_STILL_MODE,0, pvZoomNBCtrl.factor, 0);

	return SUCCESS;
}

/**
 * @fn		  UINT8 pvWideSet_Blocking(void)
 * @brief	        still wide set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvWideSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 pvTeleSet_NBlocking(void)
 * @brief	        still tele set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvTeleSet_NBlocking(
	void
)
{
	//SINT16 idx;
	UINT8 ZfStatus;
	
	if ( pvZoomNBCtrl.curLog >= pvZoomNBCtrl.maxLog) {
		pvSmoothZoomDbg("host curLog=%d\n",pvZoomNBCtrl.curLog);
		return FAIL;
	}

	if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE) {
		pvZoomNBCtrl.zoomAction= PV_ZOOM_ACTION_TELE;
	}
	else {
		pvZoomNBCtrl.zoomAction= PV_ZOOM_ACTION_TELE_ONE;
	}
	
	//ZfStatus = pvZoomSet(0, pvZoomCtrl.factor, 0);
	ZfStatus = zoomDevZoomSet(ZOOM_LOGI_STILL_MODE,0, pvZoomNBCtrl.factor, 0);
	
	return SUCCESS;
	
}

/**
 * @fn		  UINT8 pvTeleSet_Blocking(void)
 * @brief	        still tele set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvTeleSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 pvZoom3APreSet_NBlocking(void)
 * @brief	        still zoom 3A preprocess set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvZoom3APreSet_NBlocking(
	void
)
{
	UINT8 frameRate;

	pvSmoothZoomSysInfo("start\n");
	pvZoomNBCtrl.zoomStatus = PV_ZOOM_STATUS_START;
	
	/* hold AWB window, for zoom crash fix */
	if (HAL_GlobalICVerGet() == 0x00) {
		HAL_CdspVDramSkipWrSyncSet(0); /* sync immediately */
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,1);		/* don't skip writing AWB data to DRAM */
	}
	else {
		HAL_CdspAWBWinEn( 0x01, 0x1, 0x00, 0x0, 0 );
	}

	/* hold AE window */ 
	HAL_CdspAEWinHoldSet(1);

	/* get frame rate mode */
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
	//printf("framerate=%bx\n",frameRate);

	/* set still system max cdsp zfactor */
	#if (VGA_60FPS_2ST_PATH_EN == 1)
	if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac_VGA_60FPS,VideoCDSPMaxFac_VGA_60FPS,1);
	}
	else{
		HAL_CdspMaxZFactorSet(StillCDSPMaxFac,StillCDSPMaxFac,1);
	}
	#else
	HAL_CdspMaxZFactorSet(StillCDSPMaxFac,StillCDSPMaxFac,1);
	#endif

	return SUCCESS;
}

/**
 * @fn		  UINT8 pvZoom3APreSet_Blocking(void)
 * @brief	        still zoom 3A preprocess set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvZoom3APreSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 pvZoom3APostSet_Blocking(void)
 * @brief	        still zoom 3A postprocess set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvZoom3APostSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 pvZoom3APostSet_NBlocking(void)
 * @brief	        still zoom 3A postprocess set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvZoom3APostSet_NBlocking(
	void
)
{
	UINT16 sensorZfactor,dispZFactor,cdspZFactor,factor;
	UINT16 maxHFac,maxVFac;
	UINT16 awbDispFactor = 100;
	UINT16 crop_hsize,crop_vsize;

	factor = pvZoomFctGet_NBlocking();

	if(G_ZFUpdateFailFlag){/*fan.xiao Add for mantis:0045941 @2012/9/25*/
		#if 0
		/UINT8 ZfStatus;
			/* clear CDSP/DISP zf flag */
		HAL_DramZUpdateClear(0x30); 
		
		/* zf update trigger */
		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);
		
		/* zf update wait done */
		ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 300);
		#endif
		G_ZFUpdateFailFlag = 0;
		osMsgPost(SP1K_MSG_DZOOM_3A_UPDATE);
		//printf("11-%bu\n",ZfStatus);
	}


	
	/* get sensor zoom factor */
	frontDeviceInfoGet(frontDevActGet(),FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,&sensorZfactor,NULL);
	//printf("sZf=%d\n",sensorZfactor);

	if ( sensorZfactor != 0 ) {
		if (factor >= sensorZfactor) {
			factor = factor*100/sensorZfactor;
			//printf("sf=%d\n",factor);
		}
	}

	/* get max h,v cdsp zoom factor */
	HAL_CdspMaxZFactorGet(&maxHFac, &maxVFac);
	maxHFac = (maxHFac>maxVFac) ? maxVFac : maxHFac;
	//printf("mhf=%d,mvf=%d\n",maxHFac, maxVFac);
	
	if ( factor > maxHFac ) {		
		dispZFactor = (UINT32)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		cdspZFactor = maxHFac;
	}
	else {
		dispZFactor = 100;
		cdspZFactor = factor;
	}
	//printf("factor=%d, cdspZFactor=%d, dispZFactor=%d\n",factor,cdspZFactor,dispZFactor);

	crop_hsize = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcrophsize_high ] << 8 ) | XBYTE[ REG_Cdsp_pcrophsize_low ];
	crop_vsize = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcropvsize_high ] << 8 ) | XBYTE[ REG_Cdsp_pcropvsize_low ];	

	HAL_Cdsp1st3AWinSet(crop_hsize,crop_vsize,dispZFactor,awbDispFactor);

	#if 0
	if(cdspZFactor <= maxHFac) {
		sp1kSnapDzoomFactorSet( cdspZFactor, cdspZFactor );
		dzoomDispXZoomFactor = dzoomDispYZoomFactor = cdspZFactor;
	}
	else {
		maxVFac = (UINT16)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		sp1kSnapDzoomFactorSet( maxVFac, maxVFac );
		dzoomDispXZoomFactor = dzoomDispYZoomFactor = maxVFac;
	}
	#endif
	
	/* unhold AWB window */
	if (HAL_GlobalICVerGet() == 0x00) {
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0);		/* don't skip writing AWB data to DRAM */
		HAL_CdspVDramSkipWrSyncSet(1); /* sync immediately */
	}
	else {
		HAL_CdspAWBWinEn( 0x01, 0x0, 0x00, 0x1, 0 );
	}
	
	/* unhold AE window */
	HAL_CdspAEWinHoldSet(0);
	
	pvZoomNBCtrl.zoomStatus = PV_ZOOM_STATUS_FINISH;
	pvSmoothZoomSysInfo("finish\n");

	return SUCCESS;
}

/**
 * @fn		  void pvSmoothZoomSet_Blocking(UINT8 en)
 * @brief	        still smooth zoom enable/disable set (Blocking)
 * @param	  [in] en : enable/disable
 * @retval	  NULL
 * @see 	        NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
pvSmoothZoomSet_Blocking(
	UINT8 en
)
{
	videoSmoothZoomSet_Blocking(en);
}

/**
 * @fn		  void pvSmoothZoomSet_NBlocking(UINT8 en)
 * @brief	        still smooth zoom enable/disable set (NBlocking)
 * @param	  [in] en : enable/disable
 * @retval	  NULL
 * @see 	        NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
pvSmoothZoomSet_NBlocking(
	UINT8 en
)
{
	videoSmoothZoomSet_NBlocking(en);
}

/**
 * @fn		  UINT8 pvSmoothZoomGet_Blocking(void)
 * @brief	        still smooth zoom enable/disable Get (Blocking)
 * @param	  NONE
 * @retval	  typedef enum INTR_SMOOTH_ZOOM_CTRL_STATUS.
 *                                   0 = INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE
 *                                   1 = INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8  
pvSmoothZoomGet_Blocking(
	void
)
{
	return INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE;
}

/**
 * @fn		  UINT8 pvSmoothZoomGet_NBlocking(void)
 * @brief	        still smooth zoom enable/disable Get (NBlocking)
 * @param	  NONE
 * @retval	  typedef enum INTR_SMOOTH_ZOOM_CTRL_STATUS.
 *                                   0 = INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE
 *                                   1 = INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8  
pvSmoothZoomGet_NBlocking(
	void
)
{
	return videoSmoothZoomGet_NBlocking();
}

/**
 * @fn		  UINT8 pvSmoothZoomStatusGet_NBlocking(void)
 * @brief	        still smooth zoom status Get (NBlocking)
 * @param	  NONE
 * @retval	  FAIL = not Finish,SUCCESS = Finish
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvSmoothZoomStatusGet_NBlocking(
	void
)
{
	if ((pvZoomNBCtrl.zoomStatus == PV_ZOOM_STATUS_FINISH) && (intrSmoothZoomCtrl.zoom3AUpdateDlyFrame == 0)) {
		return SUCCESS;
	}
	else {
		pvSmoothZoomSysInfo("%bu,%bu\n",pvZoomNBCtrl.zoomStatus,intrSmoothZoomCtrl.zoom3AUpdateDlyFrame);	
		return FAIL;
	}
}

/**
 * @fn		  UINT8 pvSmoothZoomStatusGet_Blocking(void)
 * @brief	        still smooth zoom status Get (Blocking)
 * @param	  NONE
 * @retval	  FAIL = not Finish,SUCCESS = Finish
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pvSmoothZoomStatusGet_Blocking(
	void
)
{
	return SUCCESS;
}

