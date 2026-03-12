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
#include "common.h"
#include "initio.h"
#include "reg_jpeg.h"
#include "reg_def.h"
#include "dos32.h"
#include "gpio.h"
#include "main.h"
#include "timer.h"
#include "intr.h"
#include "os_api.h"
#include "os_main.h"
#include "os_msg.h"
#include "utility.h"
#include "video_clip.h"
#include "dbg_def.h"
#include "dbg_wdt.h"
#include "dbg_assert.h"
#include "front_logi.h"
#include "video_debug.h"
#include "sp1k_rsvblk_api.h"
#include "asicreg.h"
#include "hal_dram.h"
#include "pv_task.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "sp1k_video_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_pv_api.h"
#include "ae_api.h"
#include "sp1k_ae_api.h"
#include "zoom_logi.h"
#include "video_zoom.h"
#include "hal_disp.h"
#include "video_capture.h"	   /* avoid XDATA overflow */
#include "sp1k_pb_api.h"	/* avoid XDATA overflow */
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_VIDEO_ZOOM__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/* add for v2.1.8 compatibility @start  */
UINT16 xdata video_zoomFactor = 100;
UINT8 xdata video_zoomInFlag = 0;
UINT8 xdata video_zoomOutFlag = 0;
/* add for v2.1.8 compatibility @end  */

appVideoZoom_NBlocking_t xdata VideoZoomNBCtrl = {
	100,							//current zoom factor
	400,							//max zoom factor in 4X
	100,							//min zoom factor in 1x
	41,								//max zoom step in 4X (input res lookup table end idx)
	0,								//min zoom step in 1x (input res lookup table start idx)
	0,								//current zoom step
	1,								//zoom interval
	VIDEO_ZOOM_ACTION_INITIAL,		//zoom action
	VIDEO_ZOOM_ACTION_INITIAL,		//zoom PreAction
	0,								//hQDzoomStatus
	0,								//hQDzoomFct
	0,								//hQDzoomStep
	VIDEO_ZOOM_STATUS_FINISH,		//zoomStatus
	0,								//curLoadLog
	0,								//queueOffset
};

intrSmoothZoom xdata intrSmoothZoomCtrl = {
	0,											//zoomRegSize
	0,											//zoomRealTimeUpdate
	INTR_SMOOTH_ZOOM_NOT_RELEASE_KEY,			//zoomReleaseKeySts
	0,											//zoom3AUpdateDlyFrame
	0,											//zoomClampH
	0,											//zoomClampV
	INTR_SMOOTH_ZOOM_MACHINE_STATUS_STOP,		//zoomFactUpdate
	INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE,		//zoomSmoothEn
	0,											//zoomFrameCnt
	INTR_SMOOTH_ZOOM_FRAME_UPDATE_INTERVAL_3,	//zoomFrameUpdateInterval   (zoom frame update parameter)
	INTR_SMOOTH_SENSOR_ZOOM_FLOW_END,			//frontGateDisable
};

static UINT16 xdata zoomSmoothBuf[ZOOM_CTRL_TYPE_MAX][ZOOM_BUF_MAX];
static ZoomSmoothQue_t xdata zoomSmoothQueCtrl[ZOOM_CTRL_TYPE_MAX];

UINT8 dzoomBuf[1024] _at_ 0x4000;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void pvSmoothZoomSyncRegSet(void);
extern UINT8 HAL_GlobalICVerGet(void);
extern UINT8 HAL_JpgZFactorCropSet(UINT16 hfactor,	UINT16 vfactor,UINT16 SrcWidth,UINT16 SrcHeight,UINT8 distClampx,UINT8 distClampy,UINT16 DstWidth,UINT16 DstHeight,UINT8 option);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn		  UINT8 videoZoomDevOpen(	UINT8 type)
 * @brief	        video Zoom Dev Open
 * @param      [in] type : typedef enum ZOOM_LOGI_TYPE
 * @retval       return = SUCCESS / FAIL
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT8
videoZoomDevOpen(
	UINT8 type
)
{
	UINT8 sts,actDevNum;
	zoomOperatingFunc_t dev;

	switch ( type ) {
		case ZOOM_LOGI_BLOCKING_TYPE:
			dev.pfOpZoomResourceLoad 		= videoZoomResLoad_Blocking;
			dev.pfOpZoomSet					= videoRecZoomFacSet_Blocking;
			dev.pfOpZoomFactorGet			= videoZoomFctGet_Blocking;
			dev.pfOpZoomCtrlParaSet			= videoZoomCtrlParaSet_Blocking;
			dev.pfOpZoomCtrlParaGet			= videoZoomCtrlParaGet_Blocking;
			dev.pfOpZoomSmoothSet			= videoSmoothZoomSet_Blocking;
			dev.pfOpZoomSmoothGet 			= videoSmoothZoomGet_Blocking;
			dev.pfOpZoomSmoothStatusGet		= videoSmoothZoomStatusGet_Blocking;
			dev.pfOpZoomSmooth3APreSet		= videoZoom3APreSet_Blocking;
			dev.pfOpZoomSmooth3APostSet		= videoZoom3APostSet_Blocking;
			dev.pfOpZoomSmoothTeleSet		= videoTeleSet_Blocking;
			dev.pfOpZoomSmoothWideSet		= videoWideSet_Blocking;
			break;
		case ZOOM_LOGI_NONBLOCKING_TYPE:
			dev.pfOpZoomResourceLoad 		= videoZoomResLoad_NBlocking;
			dev.pfOpZoomSet					= videoRecZoomFacSet_NBlocking;
			dev.pfOpZoomFactorGet			= videoZoomFctGet_NBlocking;
			dev.pfOpZoomCtrlParaSet			= videoZoomCtrlParaSet_NBlocking;
			dev.pfOpZoomCtrlParaGet			= videoZoomCtrlParaGet_NBlocking;
			dev.pfOpZoomSmoothSet			= videoSmoothZoomSet_NBlocking;
			dev.pfOpZoomSmoothGet			= videoSmoothZoomGet_NBlocking;
			dev.pfOpZoomSmoothStatusGet		= videoSmoothZoomStatusGet_NBlocking;
			dev.pfOpZoomSmooth3APreSet		= videoZoom3APreSet_NBlocking;
			dev.pfOpZoomSmooth3APostSet 	= videoZoom3APostSet_NBlocking;
			dev.pfOpZoomSmoothTeleSet		= videoTeleSet_NBlocking;
			dev.pfOpZoomSmoothWideSet		= videoWideSet_NBlocking;
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
 * @fn		  UINT16 videoZoomCtrlParaGet_NBlocking(UINT8 item)
 * @brief	  video view zoom system crtrl parameter get (NBlocking)
 * @param	 [in] item : typedef enum VIDEO_ZOOM_CTRL_PARA_ITEM
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16
videoZoomCtrlParaGet_NBlocking(
	UINT8 item
)
{
	switch (item) {
		case VIDEO_ZOOM_CTRL_PARA_CURRENT_FACTOR:
			return VideoZoomNBCtrl.factor;
		case VIDEO_ZOOM_CTRL_PARA_MAX_FACTOR:
			return VideoZoomNBCtrl.MaxFct;
		case VIDEO_ZOOM_CTRL_PARA_MIN_FACTOR:
			return VideoZoomNBCtrl.MinFct;
		case VIDEO_ZOOM_CTRL_PARA_MAX_LOG:
			return VideoZoomNBCtrl.maxLog;
		case VIDEO_ZOOM_CTRL_PARA_MIN_LOG:
			return VideoZoomNBCtrl.minLog;
		case VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG:
			return VideoZoomNBCtrl.curLog;
		case VIDEO_ZOOM_CTRL_PARA_LOG_STEP:
			return VideoZoomNBCtrl.logStep;
		default:
			ASSERT(0,1);
			break;
	}
}

/**
 * @fn		  UINT16 videoZoomCtrlParaGet_Blocking(UINT8 item)
 * @brief	  video view zoom system crtrl parameter get (Blocking)
 * @param	 [in] item : typedef enum VIDEO_ZOOM_CTRL_PARA_ITEM
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16
videoZoomCtrlParaGet_Blocking(
	UINT8 item
)
{
	item = item;
	return SUCCESS;
}

/**
 * @fn		  void videoZoomCtrlParaSet_NBlocking(UINT8 item,UINT16 value)
 * @brief	  video view zoom system crtrl parameter set (NBlocking)
 * @param	  [in] item : typedef enum VIDEO_ZOOM_CTRL_PARA_ITEM
 * @param	  [in] value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void
videoZoomCtrlParaSet_NBlocking(
	UINT8 item,
	UINT16 value
)
{
	switch (item) {
		case VIDEO_ZOOM_CTRL_PARA_CURRENT_FACTOR:
			VideoZoomNBCtrl.factor = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_MAX_FACTOR:
			VideoZoomNBCtrl.MaxFct = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_MIN_FACTOR:
			VideoZoomNBCtrl.MinFct = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_MAX_LOG:
			VideoZoomNBCtrl.maxLog = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_MIN_LOG:
			VideoZoomNBCtrl.minLog = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG:
			VideoZoomNBCtrl.curLog = value;
			break;
		case VIDEO_ZOOM_CTRL_PARA_LOG_STEP:
			VideoZoomNBCtrl.logStep = (UINT8)value;
			break;
		default:
			ASSERT(0,1);
			break;
	}
}

/**
 * @fn		  void videoZoomCtrlParaSet_Blocking(UINT8 item,UINT16 value)
 * @brief	  video view zoom system crtrl parameter set (Blocking)
 * @param	  [in] item : typedef enum VIDEO_ZOOM_CTRL_PARA_ITEM
 * @param	  [in] value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void
videoZoomCtrlParaSet_Blocking(
	UINT8 item,
	UINT16 value
)
{
	item = item;
	value = value;
}

/**
 * @fn		  UINT8 videoZoomResLoad_NBlocking(UINT32 *pArray)
 * @brief		load video zoom parameter resource file (NBlocking)
 * @param    [in] pArray : zoomFactorTbl = pArray[0]
 *                                 regTbl            = pArray[1]
 *                                 regValueTbl     = pArray[2]
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
UINT8
videoZoomResLoad_NBlocking(
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

	actId = frontDevActGet();

	videoSmoothZoomSysInfo("videoZoomResLoad\n");

	/* zoom factor table */
	if ( sp1kNandRsvRead(zoomFactorTbl, (K_SDRAM_AviZoomFactorAddr<< 1)) ) {

		sp1kNandRsvSizeGet(zoomFactorTbl, NULL, &size);

		frontDeviceInfoGet(actId,FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,&para1,NULL);

		VideoZoomNBCtrl.maxLog = (UINT16)((size/2)-1);

		if (para1 != 0) {

			M_DRAM_SetStartAddress((UINT32)K_SDRAM_AviZoomFactorAddr, 1);

			for (i=0; i<size; i+=2){
				M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)
				//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);

				WRITE8(fact,0,msb);
				WRITE8(fact,1,lsb);

				if ((hQDzoomFctGet == 0) && (fact >= para1)) {
					VideoZoomNBCtrl.hQDzoomFct = fact;
					VideoZoomNBCtrl.hQDzoomStep = (UINT16)i/2;
					hQDzoomFctGet = 1;
					//printf("fact=%d,hQDzoomStep=%bu\n",fact,VideoZoomNBCtrl.hQDzoomStep);
				}

				if (i == 0) {
					VideoZoomNBCtrl.MinFct = fact;
				}
			}

			VideoZoomNBCtrl.MaxFct = fact;
		}
	}

	/* reg table */
	if ( sp1kNandRsvSizeGet(regTbl, NULL, &intrSmoothZoomCtrl.zoomRegSize) ) {
		if ( sp1kNandRsvRead(regTbl, (K_SDRAM_AviZoomRegTblAddr<< 1)) ) {

			//M_DRAM_SetStartAddress((UINT32)K_SDRAM_AviZoomRegTblAddr, 1);

			//M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)

			//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);
		}
	}
	else {
		intrSmoothZoomCtrl.zoomRegSize = 0;
	}

	/* reg value */
	if ( sp1kNandRsvRead(regValueTbl, (K_SDRAM_AviZoomRegValAddr<< 1)) ) {

		//M_DRAM_SetStartAddress((UINT32)K_SDRAM_AviZoomRegValAddr, 1);

		//M_DRAM_ReadWord(lsb, msb);//res data format : (lsb, msb)

		//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);
	}

	return SUCCESS;
}

/**
 * @fn		  UINT8 videoZoomResLoad_Blocking(UINT32 *pArray)
 * @brief		load video zoom parameter resource file (Blocking)
 * @param    [in] pArray : zoomFactorTbl = pArray[0]
 *                                 regTbl            = pArray[1]
 *                                 regValueTbl     = pArray[2]
 * @retval	NONE
 * @todo
 * @author	LinJieCheng
 * @since	2010-10-07
 */
UINT8
videoZoomResLoad_Blocking(
	UINT32 *pArray
)
{
	pArray = pArray;

	return SUCCESS;
}

/**
 * @fn		UINT8 videoRecZoomFacSet_Blocking(UINT8 noUSed,UINT16 factor,UINT8 videozoom)
 * @brief		zoom facter set (Blocking)
 * @param     noUSed  :      not used
 * @param	factor	:	zoom facter
 * @param	videozoom	:1 is video zoom , 0 is not video zoom
 * @retval	return = FAIL/SUCCESS
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8
videoRecZoomFacSet_Blocking(
	UINT8 noUSed,
	UINT16 factor,
	UINT8 videozoom
)
{
	UINT16 maxHFac,maxVFac;
	UINT16 hSize, vSize;
	UINT16 cropHSize, cropVSize; //for front
	UINT16 yuvHSize,yuvVSize;
	UINT16 cdspZFactor,dispZFactor,sensorZfactor;
	UINT16 secPathVSize;
	UINT8 DistEn=0;
	UINT8 DistClmpX=0,DistClmpY=0;
	UINT8 frameRate;
	UINT16 TmpFac=100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 roiX=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 roiY=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT8 offsetT=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 recSizeH,recSizeV;
	//printf("video zoom\n");
	noUSed = noUSed;
	videoRecSizeGet(&recSizeH,&recSizeV);
	/*if((!video_zoomInFlag) && (!video_zoomOutFlag)) {
		return SUCCESS;
	}*/
	//printf("in=%bu,out=%bu\n",video_zoomInFlag,video_zoomOutFlag);
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
	/* wait for zf done and write new factors to registers then*/
	if (HAL_DramZUpdateWait(((videozoom) ? HAL_DRAM_ZUPDATE_VIDEO : HAL_DRAM_ZUPDATE_PV),
							(UINT16)5) == FAIL) {
		return FAIL;
	}
	video_zoomInFlag = 0;
	video_zoomOutFlag = 0;

	/* get frame rate mode */
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
	//printf("framerate=%bx\n",frameRate);

	/* set video system max cdsp zfactor */
	//printf("vidClipCtrl.imgWidth=%d\n",vidClipCtrl.imgWidth);
	#if (VGA_60FPS_2ST_PATH_EN == 1)
	if ((recSizeH == 640 && recSizeV == 480) &&
		(frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac_VGA_60FPS,VideoCDSPMaxFac_VGA_60FPS,1);
	}
	else if (recSizeH > 320) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	else{
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	#else
	if (recSizeH > 320) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	else{
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	#endif

	pvSizeGet(&hSize, &vSize);
	//printf("hSize=%d,vSize=%d\n",hSize, vSize);

	sp1kPreviewCropSizeGet(&cropHSize, &cropVSize);
	//printf("cropHSize=%d,cropVSize=%d\n",cropHSize, cropVSize);

	HAL_CdspMaxZFactorGet(&maxHFac, &maxVFac);
	//printf("mhf=%d,mvf=%d\n",maxHFac, maxVFac);
	maxHFac = (maxHFac>maxVFac) ? maxVFac : maxHFac;
	//printf("mhf=%d\n",maxHFac);

	#ifdef DIST_FUNC_EN
	DistEn = sp1kDistStatusGet();
	distClampSizeGet(&DistClmpX,&DistClmpY);
	//printf("Dist En=%bd\n",DistEn);
	//DistSwitchSize(vidClipCtrl.imgWidth,vidClipCtrl.imgHight);
	#endif

	//HAL_Cdsp1stPathYuvSizeGet(&yuvHSize,&yuvVSize);

	if (factor> maxHFac) {
		dispZFactor = (UINT32)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		cdspZFactor = maxHFac;
	}
	else {
		dispZFactor = 100;
		cdspZFactor = factor;
	}
	//printf("factor=%d, cdspZFactor=%d, dispZFactor=%d\n",factor,cdspZFactor,dispZFactor);

	#if 0
		if (1){
			UINT8 i;
			printf("Before Wait Zf:\n");
			for(i=0;i<24;i++){
				printf("0x28%bx%bx : %bx\n",i/16+1,(i+8)%16, XBYTE[REG_Jpeg_ImgWidth_Low+i]);
			}
			for(i=0;i<8;i++){
				printf("0x286%bx : %bx\n",i%16, XBYTE[REG_Jpeg_Srcwidth_Low+i]);
			}
		}
	#endif

	#if 0
	/* wait for zf done and write new factors to registers then*/
	if (videozoom==1){
		ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_VIDEO,(UINT16)10);

	}
	else{
		ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV,(UINT16)10);
	}

	if(ZfStatus == FAIL)
		return FAIL;
	#endif

	#if 0
		if (1){
			UINT8 i;
			printf("After Wait Zf:\n");
			for(i=0;i<24;i++){
				printf("0x28%bx%bx : %bx\n",i/16+1,(i+8)%16, XBYTE[REG_Jpeg_ImgWidth_Low+i]);
			}
			for(i=0;i<8;i++){
				printf("0x286%bx : %bx\n",i%16, XBYTE[REG_Jpeg_Srcwidth_Low+i]);
			}
		}
	#endif
	/* check cdspZFactor > maxHFac */
	if (cdspZFactor <= maxHFac) {
		HAL_Cdsp1stPathZFactorSet(SYNCZF, cdspZFactor, cdspZFactor, hSize, vSize, cropHSize, cropVSize, dispZFactor, 100);//cannot during cdspsof/cdspzfupdate
		HAL_Cdsp1stPathYuvSizeGet(&yuvHSize,&yuvVSize);
		//printf("yuvHSize=%d,yuvVSize=%d\n",yuvHSize,yuvVSize);
		if(vidRecFrameBufNumGet()==3){
			#if 0
			/*display data source form 1st path*/
			HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);
			HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor,0);	/* yuvVSize-1 to workaround 2nd Path last line error */
			dispFitShow(yuvHSize,yuvVSize,0);
			HAL_Cdsp2ndPathCfg(1,0,1);
			HAL_Cdsp2ndPathEnSet(0);
			#else
			/*display data source form 2nd path*/
			videoRec2ndFacSet(yuvHSize,yuvVSize);
			secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
			if (DistEn == 0) {
				HAL_DispScaleSet(320, secPathVSize-1, dispZFactor, 0); /* set display zoom factor */ /* secPathVSize-1 to workaround 2nd Path last line error */
			}else {
				//HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, 100, 0); 	/* set display zoom factor */
				HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, dispZFactor, 0); 	/* set display zoom factor */
			}
			dispFitShow(320,secPathVSize,0);
			#endif

		}
		else{
			#if (VGA_60FPS_2ST_PATH_EN == 0)
			if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {

				//printf("factor=%d\n",factor);
				//printf("dispZFactor=%d\n",dispZFactor);
				//printf("yuvHSize=%d,yuvVSize=%d\n",yuvHSize,yuvVSize);

				if (DistEn == 0) {
					HAL_DispScaleSet(yuvHSize, yuvVSize, dispZFactor, 0); /* set display zoom factor */
				}
				else{
					HAL_DispScaleSet(yuvHSize-DistClmpX, yuvVSize-DistClmpY, dispZFactor, 0); 	/* set display zoom factor */
				}

				dispFitShow(yuvHSize,yuvVSize,0);
			}
			#endif
		//}
			{
				if (sp1kSuperPreviewStatusGet(1) == 1) {

					videoRec2ndFacSet(yuvHSize,yuvVSize);

					HAL_DispImgXySet(0,  0,yuvHSize - 1, yuvVSize - 1);

					if (DistEn == 0) {
						HAL_DispScaleSet(yuvHSize, yuvVSize-1, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */
					}
					else {
						HAL_DispScaleSet(yuvHSize-DistClmpX, yuvVSize-DistClmpY, dispZFactor, 0); /* secPathVsize-1 to workaround 2nd Path last line error */
					}

					dispFitShow(yuvHSize,yuvVSize,0);
				}
				else {
					videoRec2ndFacSet(yuvHSize,yuvVSize);

					//HAL_Cdsp2ndPathCfg(0,0,1);
					//HAL_Cdsp2ndPathEnSet(1);

					//printf("400-- video zoom =%d, Srcx=%d, Srcy=%d\n",factor,yuvHSize,yuvVSize);

					//secPathH=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
					//HAL_DispScaleSet(320-DistClmpX, secPathH-DistClmpY, 100); 	/* set display zoom factor */
					//if (factor==maxHFac){
					secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
					if (DistEn == 0) {
						HAL_DispScaleSet(320, secPathVSize-1, dispZFactor, 0); /* set display zoom factor */ /* secPathVSize-1 to workaround 2nd Path last line error */
					}
					else {
						//HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, 100, 0); 	/* set display zoom factor */
						HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, dispZFactor, 0); 	/* set display zoom factor */
					}
					dispFitShow(320,secPathVSize,0);

					//printf("yuvHSize=%d,yuvVSize=%d,secPathVSize=%d\n",yuvHSize,yuvVSize,secPathVSize);
				}
			}
		}
		//#endif

		dzoomDispXZoomFactor = dzoomDispYZoomFactor = 100;

		if (factor > maxHFac) {
			TmpFac = dispZFactor;
			offsetT = 4;
			roiX = yuvHSize * 100 / dispZFactor;
			roiY = yuvVSize * 100 / dispZFactor;
		}
		else if (factor > 100) {
			TmpFac = 101;
			offsetT = 4;
			roiX = yuvHSize - offsetT;	/* avoid mirror issue */
			roiY = yuvVSize - offsetT;
		}
		else {
//			offsetT = 4; //old
			offsetT = 0;
		}

		//if (videozoom == 1) {
			#ifdef DIST_FUNC_EN
				//HAL_JpgZFactorCropSet(100, 100, yuvHSize, yuvVSize, DistClmpX,DistClmpY, vidClipCtrl.imgWidth, vidClipCtrl.imgHight, 0x80|(DistEn<<5));
				HAL_JpgZFactorCropSet(TmpFac, TmpFac, yuvHSize-offsetT, yuvVSize-offsetT, DistClmpX,DistClmpY, recSizeH, recSizeV, 0x80|(DistEn<<5));
			#else
				//printf("video img h=%d,v=%d\n",vidClipCtrl.imgWidth,vidClipCtrl.imgHight);
				//printf("TmpFac=%d\n",TmpFac);
				//printf("yuvHSize=%d,yuvVSize=%d\n",yuvHSize,yuvVSize);
				//printf("roi H=%d,V=%d\n",roiX,roiY);
				//HAL_JpgZFactorCropSet(TmpFac, TmpFac, yuvHSize-offsetX, yuvVSize-offsetY, 0,0, vidClipCtrl.imgWidth, vidClipCtrl.imgHight, 0x80|(0<<5));
				HAL_JpgZFactorCropSet(TmpFac, TmpFac, yuvHSize-offsetT, yuvVSize-offsetT, roiX, roiY, recSizeH, recSizeV, 0x80|(0<<5));
			#endif
		//}

	}
	else {
		maxVFac = (UINT16)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
		HAL_Cdsp1stPathZFactorSet(SYNCZF, maxHFac, maxHFac, hSize, vSize, cropHSize, cropVSize, maxVFac, maxVFac);
		HAL_Cdsp1stPathYuvSizeGet(&yuvHSize,&yuvVSize);

		#if (VGA_60FPS_2ST_PATH_EN == 0)
		if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
			if (DistEn == 0) {
				HAL_DispScaleSet(yuvHSize, yuvVSize, dispZFactor, 0); /* set display zoom factor */
			}
			else {
				//HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, 100, 0);	/* set display zoom factor */
				HAL_DispScaleSet(yuvHSize-DistClmpX, yuvVSize-DistClmpY, dispZFactor, 0);	/* set display zoom factor */
			}

			dispFitShow(hSize,vSize,0);
		}
		else
		#endif
		{
			videoRec2ndFacSet(yuvHSize,yuvVSize);

			//printf("400++ video factor =%d, Srcx=%d, Srcy=%d\n",zfac,clampW,clampH);
			dispZFactor = (UINT32)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
			secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
			//HAL_DispScaleSet(clampW, clampH, dispZFactor); /* set display zoom factor */
			if (DistEn == 0) {
				HAL_DispScaleSet(320, secPathVSize-1, dispZFactor, 0); /* set display zoom factor */ /* secPathVSize-1 to workaround 2nd Path last line error */
			}
			else {
				HAL_DispScaleSet(320-DistClmpX, secPathVSize-DistClmpY, dispZFactor, 0); 	/* set display zoom factor */
			}
		}

		dzoomDispXZoomFactor = dzoomDispYZoomFactor = maxVFac;

		//if (videozoom == 1) {
			#ifdef DIST_FUNC_EN
				HAL_JpgZFactorCropSet(maxVFac, maxVFac, yuvHSize, yuvVSize, DistClmpX,DistClmpY, recSizeH, recSizeV, 0x80|(DistEn<<5));
			#else
				HAL_JpgZFactorCropSet(maxVFac, maxVFac, yuvHSize, yuvVSize, 0,0, recSizeH, recSizeV, 0x80|(0<<5));
			#endif
		//}
	}

	/* videozoom = 1 : clear CDSP/DISP/MJPG zf flag ; vidozoom = 0 : clear CDSP/DISP zf flag */
	HAL_DramZUpdateClear(((videozoom) ? 0xb0 : 0x30));

	/* avoid Clamp and Zf not sync */
	HAL_FrontSignalWait(HAL_FRONT_WAIT_VVALID,FRONT_WAIT_FALLING,1);

	/* clamp size only sync to VD, it needs to be set near zf update */
	HAL_CdspVDramSizeClamp( 1, yuvHSize, 1, yuvVSize, 1 );

	HAL_CdspDarkTDSizeSet(yuvHSize);
	//printf("yuv hsize=%d\n",yuvHSize);

	/* zf trigger */
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);

	/* zfupdate wait */
	//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV,0);
	//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_MPEG,0);

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

	return SUCCESS;
}

/**
 * @fn		UINT8 videoRecZoomFacSet_NBlocking(UINT8 noUSed,UINT16 factor,UINT8 videozoom)
 * @brief		zoom facter set (NBlocking)
 * @param     noUSed  :      not used
 * @param	factor	:	zoom facter
 * @param	videozoom	:1 is video zoom , 0 is not video zoom
 * @retval	return = FAIL/SUCCESS
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8
videoRecZoomFacSet_NBlocking(
	UINT8 noUSed,
	UINT16 factor,
	UINT8 videozoom
)
{
	UINT16 maxHFac,maxVFac;
	UINT16 hSize, vSize;
	UINT16 cdspZFactor,dispZFactor;//,sensorZfactor;
	UINT16 secPathVSize;
	UINT8 DistEn=0;
	UINT8 DistClmpX=0,DistClmpY=0;
	UINT16 TmpFac=100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 roiX=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 roiY=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT8 offsetT=0;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	UINT16 clampH = 0,clampV = 0;
	UINT8 loadLength = 0;
	UINT16 location = 0,log;
	UINT32 addrOffSet;
	UINT16 xdata* pRegIdx;
	UINT8 xdata* pRegVal;
	SINT16 idx;
	UINT8 ZfStatus,num,sts;
	//printf("video zoom\n");
	noUSed = noUSed;
	factor = factor;
	videozoom = videozoom;

	/*----------------------------------------------------------------------*/
	/*                                                zoomAction                                                 */
	/*----------------------------------------------------------------------*/

	switch (VideoZoomNBCtrl.zoomAction) {
		case VIDEO_ZOOM_ACTION_TELE_ONE:
			VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_INITIAL;
			/* zoom in */

			idx = VideoZoomNBCtrl.curLog + VideoZoomNBCtrl.logStep;

			if ( idx <= VideoZoomNBCtrl.maxLog ) {
				log = (UINT16)idx;
			}
			else {
				log = VideoZoomNBCtrl.maxLog;
			}
			VideoZoomNBCtrl.curLog = log;
			break;
		case VIDEO_ZOOM_ACTION_WIDE_ONE:
			VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_INITIAL;
			/* zoom out */

			idx = VideoZoomNBCtrl.curLog - VideoZoomNBCtrl.logStep;

			if ( idx >= VideoZoomNBCtrl.minLog ) {
				log = (UINT16)idx;
			}
			else {
				log = VideoZoomNBCtrl.minLog;
			}
			VideoZoomNBCtrl.curLog = log;
			break;
		default:
			break;
	}

	switch (VideoZoomNBCtrl.zoomAction) {
		case VIDEO_ZOOM_ACTION_INITIAL:
			/* zoom initial */

			/* zoom queue initial */
			zoomSmoothQueueInit(ZOOM_CTRL_TYPE_VIDEO);

			/* wait zf update */
			ZfStatus = HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 300);

			/* reg initial */
			pvSmoothZoomSyncRegSet();

			/* read reg resource */
			HAL_DramDmaRead(dzoomBuf, K_SDRAM_AviZoomRegTblAddr, intrSmoothZoomCtrl.zoomRegSize, 1);
			break;
		case VIDEO_ZOOM_ACTION_TELE:
			/* zoom in */
			if ((VideoZoomNBCtrl.zoomPreAction != VIDEO_ZOOM_ACTION_INITIAL) &&
				(VideoZoomNBCtrl.zoomPreAction != VideoZoomNBCtrl.zoomAction)) {
				/* zoom queue initial */
				ENTER_CRITICAL(sts);
				VideoZoomNBCtrl.curLoadLog = VideoZoomNBCtrl.curLog;
				/* zoom queue initial */
				zoomSmoothQueueInit(ZOOM_CTRL_TYPE_VIDEO);
				EXIT_CRITICAL(sts);
			}
			break;
		case VIDEO_ZOOM_ACTION_WIDE:
			/* zoom out */
			if ((VideoZoomNBCtrl.zoomPreAction != VIDEO_ZOOM_ACTION_INITIAL) &&
				(VideoZoomNBCtrl.zoomPreAction != VideoZoomNBCtrl.zoomAction)) {
				/* zoom queue initial */
				ENTER_CRITICAL(sts);
				VideoZoomNBCtrl.curLoadLog = VideoZoomNBCtrl.curLog;
				/* zoom queue initial */
				zoomSmoothQueueInit(ZOOM_CTRL_TYPE_VIDEO);
				EXIT_CRITICAL(sts);
			}
			break;
		default:
			//printf("action=%bu\n",VideoZoomNBCtrl.zoomAction);
			ASSERT(0,0);
	}

	VideoZoomNBCtrl.zoomPreAction = VideoZoomNBCtrl.zoomAction;

	if (((VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_TELE) && (VideoZoomNBCtrl.curLoadLog != VideoZoomNBCtrl.maxLog)) ||
		((VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_WIDE) && (VideoZoomNBCtrl.curLoadLog != VideoZoomNBCtrl.minLog)) ||
		(VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_INITIAL)) {

		zoomSmoothQueueEntriesGet(ZOOM_CTRL_TYPE_VIDEO,&num);

	videoSmoothZoomSysInfo("video action=%bu,%bu\n",VideoZoomNBCtrl.zoomAction,num);

		while (ZOOM_BUF_MAX > num) {
			switch (VideoZoomNBCtrl.zoomAction) {
				case VIDEO_ZOOM_ACTION_INITIAL:
					/* zoom initial */

					log = VideoZoomNBCtrl.curLog;
					addrOffSet = ((UINT32)VideoZoomNBCtrl.curLog * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);

					VideoZoomNBCtrl.queueOffset = 0;

					break;
				case VIDEO_ZOOM_ACTION_TELE:
					/* zoom in */

					idx = VideoZoomNBCtrl.curLoadLog + VideoZoomNBCtrl.logStep;

					if ( idx <= VideoZoomNBCtrl.maxLog ) {
						log = (UINT16)idx;
					}
					else {
						log = VideoZoomNBCtrl.maxLog;
					}

					addrOffSet = ((UINT32)log * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);

					VideoZoomNBCtrl.queueOffset++;

					break;
				case VIDEO_ZOOM_ACTION_WIDE:
					/* zoom out */

					idx = VideoZoomNBCtrl.curLoadLog - VideoZoomNBCtrl.logStep;

					if ( idx >= VideoZoomNBCtrl.minLog ) {
						log = (UINT16)idx;
					}
					else {
						log = VideoZoomNBCtrl.minLog;
					}

					addrOffSet = ((UINT32)log * (intrSmoothZoomCtrl.zoomRegSize >> 1) >> 1);

					VideoZoomNBCtrl.queueOffset++;
					break;
				default:
					ASSERT(0,0);
			}

			VideoZoomNBCtrl.queueOffset = (VideoZoomNBCtrl.queueOffset % ZOOM_BUF_MAX);
			videoSmoothZoomDbg("[%d],curLog=%d,curLoadLog=%d,nextLoadLog=%d\n",VideoZoomNBCtrl.queueOffset,VideoZoomNBCtrl.curLog,VideoZoomNBCtrl.curLoadLog,log);
			videoSmoothZoomDbg("addrOffSet=%lu,%lx\n",addrOffSet,(intrSmoothZoomCtrl.zoomRegSize >> 1) * VideoZoomNBCtrl.queueOffset);

			HAL_DramDmaRead(dzoomBuf + intrSmoothZoomCtrl.zoomRegSize + ((intrSmoothZoomCtrl.zoomRegSize >> 1)*VideoZoomNBCtrl.queueOffset), K_SDRAM_AviZoomRegValAddr + addrOffSet, (intrSmoothZoomCtrl.zoomRegSize >> 1) * (UINT32)1, 0);

			if (VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_INITIAL) {

				VideoZoomNBCtrl.curLoadLog = VideoZoomNBCtrl.curLog;

				break;
			}

			/* Queue Post */
			zoomSmoothQueuePost(ZOOM_CTRL_TYPE_VIDEO,(intrSmoothZoomCtrl.zoomRegSize >> 1) * VideoZoomNBCtrl.queueOffset);

			zoomSmoothQueueEntriesGet(ZOOM_CTRL_TYPE_VIDEO,&num);

			if (VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_TELE) {
				VideoZoomNBCtrl.curLoadLog = log;
				if (VideoZoomNBCtrl.curLoadLog == VideoZoomNBCtrl.maxLog) {
					videoSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,VideoZoomNBCtrl.curLoadLog);
					break;
				}
			}
			else if (VideoZoomNBCtrl.zoomAction == VIDEO_ZOOM_ACTION_WIDE) {
				VideoZoomNBCtrl.curLoadLog = log;
				if (VideoZoomNBCtrl.curLoadLog == VideoZoomNBCtrl.minLog) {
					videoSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,VideoZoomNBCtrl.curLoadLog);
					break;
				}
			}

			videoSmoothZoomDbg("QueueNum=%bu,nowLoadLog=%d\n",num,VideoZoomNBCtrl.curLoadLog);
		}
	}
	/*----------------------------------------------------------------------*/
	/*                                                zoom timing ctrl                                           */
	/*----------------------------------------------------------------------*/

	switch ( VideoZoomNBCtrl.zoomAction ) {
		case VIDEO_ZOOM_ACTION_INITIAL:

			videoSmoothZoomSysInfo("vdzoom set reg\n");

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
			HAL_CdspDarkTDSizeSet(intrSmoothZoomCtrl.zoomClampH);
			//printf("yuv hsize=%d\n",yuvHSize);
			#endif

			pvSizeGet(&hSize, &vSize);
			//printf("hSize=%d,vSize=%d\n",hSize, vSize);

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
				if (vidRecFrameBufNumGet() == 3) {
					#if 0
					dispFitShow(intrSmoothZoomCtrl.zoomClampH,intrSmoothZoomCtrl.zoomClampV,0);
					#else
					/*disp data is from 2nd path*/
					secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
					//printf("secPathVsize=%d\n",secPathVSize);
					dispFitShow(320,secPathVSize,0);
					#endif
					//secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
						//printf("secPathVsize=%d\n",secPathVSize);
						//dispFitShow(320,secPathVSize,0);
				}
				else {
					if (sp1kSuperPreviewStatusGet(1) == 1) {
						dispFitShow(intrSmoothZoomCtrl.zoomClampH,intrSmoothZoomCtrl.zoomClampV,0);
					}
					else {
						secPathVSize=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);
						//printf("secPathVsize=%d\n",secPathVSize);
						dispFitShow(320,secPathVSize,0);
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

			/* 3a window update */
			osMsgPost(SP1K_MSG_DZOOM_3A_UPDATE);

			VideoZoomNBCtrl.queueOffset = 0;
			break;
		case VIDEO_ZOOM_ACTION_TELE:
		case VIDEO_ZOOM_ACTION_WIDE:
			if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE) {
				videoSmoothZoomDbg("smooth dis\n");
				intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_START;
			}
			break;
	}

	return SUCCESS;
}

/**
 * @fn		  UINT16 videoZoomFctGet_Blocking(void)
 * @brief		video zoom facter get (Blocking)
 * @param	NONE
 * @retval	zoom facter
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT16
videoZoomFctGet_Blocking(
	void
)
{
	return video_zoomFactor;
	//return SUCCESS;
}

/**
 * @fn		  UINT16 videoZoomFctGet_NBlocking(void)
 * @brief		video zoom facter get (NBlocking)
 * @param	NONE
 * @retval	zoom facter
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT16
videoZoomFctGet_NBlocking(
	void
)
{
	UINT8 lsb,msb;
	UINT16 factor;

	//printf("curlog=%d\n",VideoZoomNBCtrl.curLog);

	M_DRAM_SetStartAddress((UINT32)K_SDRAM_AviZoomFactorAddr+(UINT32)VideoZoomNBCtrl.curLog, 1);

	M_DRAM_ReadWord(lsb, msb);//res header format : (lsb, msb)

	//printf("Msb=%bx, Lsb=%bx\n",msb,lsb);

	factor = ((UINT16)((UINT16)msb<<8)|(UINT16)lsb);

	if ( factor >= VideoZoomNBCtrl.MaxFct ) {
		VideoZoomNBCtrl.factor = VideoZoomNBCtrl.MaxFct;
		return VideoZoomNBCtrl.MaxFct;
	}
	else {
		VideoZoomNBCtrl.factor = factor;
		return factor;
	}

}

/**
 * @fn		  UINT8 videoWideSet_NBlocking(void)
 * @brief	        video wide set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoWideSet_NBlocking(
	void
)
{
	UINT8 ZfStatus;

	if ( VideoZoomNBCtrl.curLog <= VideoZoomNBCtrl.minLog ) {
		videoSmoothZoomDbg("host curLog=%d\n",VideoZoomNBCtrl.curLog);
		return FAIL;
	}

	if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE) {
		VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_WIDE;
	}
	else {
		VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_WIDE_ONE;
	}

	if ( videoRecStatusGet() == VID_REC_IDLE ) {
		ZfStatus = videoRecZoomFacSet_NBlocking(0,VideoZoomNBCtrl.factor, 0);
	}
	else {
		ZfStatus = videoRecZoomFacSet_NBlocking(0,VideoZoomNBCtrl.factor, 1);
	}

	return SUCCESS;

}

/**
 * @fn		  UINT8 videoWideSet_Blocking(void)
 * @brief	        video wide set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoWideSet_Blocking(
	void
)
{
		return SUCCESS;
}

/**
 * @fn		  UINT8 videoTeleSet_NBlocking(void)
 * @brief	        video tele set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoTeleSet_NBlocking(
	void
)
{
	UINT8 ZfStatus;

	if ( VideoZoomNBCtrl.curLog >= VideoZoomNBCtrl.maxLog ) {
		videoSmoothZoomDbg("host curLog=%d\n",VideoZoomNBCtrl.curLog);
		return FAIL;
	}

	if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE) {
		VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_TELE;
	}
	else {
		VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_TELE_ONE;
	}

	if ( videoRecStatusGet() == VID_REC_IDLE ) {
		ZfStatus = videoRecZoomFacSet_NBlocking(0,VideoZoomNBCtrl.factor, 0);
	}
	else {
		ZfStatus = videoRecZoomFacSet_NBlocking(0,VideoZoomNBCtrl.factor, 1);
	}

	return SUCCESS;

}

/**
 * @fn		  UINT8 videoTeleSet_Blocking(void)
 * @brief	        video tele set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoTeleSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 videoZoom3APreSet_NBlocking(void)
 * @brief	        video zoom 3A preprocess set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoZoom3APreSet_NBlocking(
	void
)
{
	UINT8 frameRate;
	UINT16 width,height;

	videoSmoothZoomSysInfo("start\n");
	VideoZoomNBCtrl.zoomStatus = VIDEO_ZOOM_STATUS_START;

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

	videoRecSizeGet(&width, &height);

	/* set video system max cdsp zfactor */
	#if (VGA_60FPS_2ST_PATH_EN == 1)
	if ((width == 640 && height == 480) &&
		(frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac_VGA_60FPS,VideoCDSPMaxFac_VGA_60FPS,1);
	}
	else if (width > 320) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	else{
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	#else
	if (width > 320) {
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	else{
		HAL_CdspMaxZFactorSet(VideoCDSPMaxFac,VideoCDSPMaxFac,1);
	}
	#endif

	return SUCCESS;
}

/**
 * @fn		  UINT8 videoZoom3APreSet_Blocking(void)
 * @brief	        video zoom 3A preprocess set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoZoom3APreSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  UINT8 videoZoom3APostSet_NBlocking(void)
 * @brief	        video zoom 3A post process set (NBlocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoZoom3APostSet_NBlocking(
	void
)
{
	UINT16 sensorZfactor,dispZFactor,cdspZFactor,factor;
	UINT16 maxHFac,maxVFac;
	UINT16 awbDispFactor = 100;
	UINT16 crop_hsize,crop_vsize;

	factor = videoZoomFctGet_NBlocking();

	/* get sensor zoom factor */
	frontDeviceInfoGet(frontDevActGet(),FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,&sensorZfactor,NULL);
	//printf("sZf=%d\n",sensorZfactor);

	if (sensorZfactor != 0) {
		if (factor >= sensorZfactor) {
			factor = factor*100/sensorZfactor;
			//printf("sf=%d\n",factor);
		}
	}

	/* get max h,v cdsp zoom factor */
	HAL_CdspMaxZFactorGet(&maxHFac, &maxVFac);
	maxHFac = (maxHFac>maxVFac) ? maxVFac : maxHFac;
	//printf("mhf=%d\n",maxHFac);

	if (factor> maxHFac) {
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
	if (cdspZFactor <= maxHFac) {
		dzoomDispXZoomFactor = dzoomDispYZoomFactor = 100;
	}
	else {
		maxVFac = (UINT16)((UINT32)factor*(UINT32)100/(UINT32)maxHFac);
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

	VideoZoomNBCtrl.zoomStatus = VIDEO_ZOOM_STATUS_FINISH;
	videoSmoothZoomSysInfo("finish\n");

	return SUCCESS;
}

/**
 * @fn		  UINT8 videoZoom3APostSet_Blocking(void)
 * @brief	        video zoom 3A post process set (Blocking)
 * @param	  NONE
 * @retval	  FAIL,SUCCESS
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoZoom3APostSet_Blocking(
	void
)
{
	return SUCCESS;
}

/**
 * @fn		  void videoRec2ndFacSet(UINT16 Src1stX, UINT16 Src1stY)
 * @brief	  2nd path factor set
 * @param	  [in] Src1stX : 1st path X size
 * @param	  [in] Src1stY : 1st path Y size
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void
videoRec2ndFacSet(
	UINT16 Src1stX,
	UINT16 Src1stY
)
{
	UINT16 dstY;
	UINT16 hSize,vSize;
	pvSizeGet(&hSize, &vSize);

	//UINT16 yoff;
	//dstY = (UINT16)(((((UINT32)Src1stY * 320+(Src1stX-1)) / Src1stX)>>2)<<2);
	dstY=(UINT16)((((UINT32)vSize*320/((UINT32)hSize))>>1)<<1);

	//yoff = ((240-dstY)>>1)&~1;
	HAL_Cdsp2ndPathHScaleYuvSet(SYNCZF,Src1stX,320);  //will: et sencond path yuv h scale factor//tq marked for record error@100323
	HAL_Cdsp2ndPathVScaleYuvSet(SYNCZF,Src1stY,dstY);  //will: set sencond path yuv V scale factor//tq marked for record error@100323
	HAL_Cdsp2ndPathWriteOffset(0,0);
	HAL_Cdsp2ndPathClampHSize(1,320);
	if(vidRecFrameBufNumGet()==3){
		#if 0
		/*2nd path disable*/
		HAL_Cdsp2ndPathCfg(1,0,0);
		HAL_Cdsp2ndPathEnSet(0);
		#else
		/*2nd path enable*/
		HAL_Cdsp2ndPathCfg(0,0,1);
		HAL_Cdsp2ndPathEnSet(1);
		#endif
	}
	else{
		HAL_Cdsp2ndPathCfg(0,0,1);
		HAL_Cdsp2ndPathEnSet(1);
	}

}

/**
 * @fn		  void zoomSmoothQueueInit(void)
 * @brief
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void
zoomSmoothQueueInit (
	UINT16 msgGrp
)
{
	UINT16 i;
	ZoomSmoothQue_t* pMsg;

	for (i = msgGrp; i < ZOOM_CTRL_TYPE_MAX; i++) {
		pMsg = &zoomSmoothQueCtrl[i];

		pMsg->QueStart = &zoomSmoothBuf[i][0];
		pMsg->QueEnd = &zoomSmoothBuf[i][ZOOM_BUF_MAX];
		pMsg->QueIn = &zoomSmoothBuf[i][0];
		pMsg->QueOut = &zoomSmoothBuf[i][0];
		pMsg->QueEntries = 0;
		pMsg->QueSize = ZOOM_BUF_MAX;
		pMsg->MsgBuf = &zoomSmoothBuf[i][0];
		pMsg->bLock = 0;

		memset(pMsg->MsgBuf, 0, sizeof(zoom_t) * (UINT32)ZOOM_BUF_MAX);
		//isMsgNull[i] = TRUE;
	}
}

/**
 * @fn		  UINT8 zoomSmoothQueuePost (UINT16 msgGrp, UINT16 offset)
 * @brief	        Post offset value in queue
 * @param	  [in] msgGrp : typedef enum ZoomSmoothCtrlType_e
 * @param	  [in] offset : buffer offset value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
zoomSmoothQueuePost (
	UINT16 msgGrp,
	UINT16 offset
)
{
	UINT8 status;
	ZoomSmoothQue_t* pMsg;

	if (msgGrp >= ZOOM_CTRL_TYPE_MAX) {
		return FAIL;
	}

	ENTER_CRITICAL(status);

	pMsg = &zoomSmoothQueCtrl[msgGrp];
	if (pMsg->QueEntries >= pMsg->QueSize) {
		//printf( "MsgQue over flow!\n" );
		EXIT_CRITICAL(status);
		return FAIL;
	}

	//isMsgNull[msgGrp] = FALSE;
	*pMsg->QueIn++ = offset;
	pMsg->QueEntries++;
	if (pMsg->QueIn == pMsg->QueEnd) {
		pMsg->QueIn = pMsg->QueStart;
	}

	EXIT_CRITICAL(status);
	return SUCCESS;
}

/**
 * @fn		  UINT8 zoomSmoothQueueAccept (UINT16 msgGrp, UINT16* offset)
 * @brief	        Accept offset value in queue
 * @param	  [in] msgGrp : typedef enum ZoomSmoothCtrlType_e
 * @param	  [in] offset : buffer offset value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
zoomSmoothQueueAccept (
	UINT16 msgGrp,
	UINT16* offset
)
{
	UINT8 ret;
	ZoomSmoothQue_t* pMsg;

	//printf( "\n>>>>>>>>>osMsgAccept<<<<<<<<<<\n\n" );
	ret = FAIL;

	/* msg accept poroity "System" >> "Button" >> "Host" */
	pMsg = &zoomSmoothQueCtrl[msgGrp];
	if (pMsg->QueEntries != 0) {
		if (offset) {
			*offset = *pMsg->QueOut;
		}
		*pMsg->QueOut++ = 0x0000;
		pMsg->QueEntries--;
		if (!pMsg->QueEntries) {
			//isMsgNull[msgGrp] = TRUE;
		}
		if (pMsg->QueOut == pMsg->QueEnd) {
			pMsg->QueOut = pMsg->QueStart;
		}
		ret = SUCCESS;
	}

	if (ret != SUCCESS) {
		if (offset) {
			*offset = 0x0000;
		}
	}

	return ret;
}

/**
 * @fn		  UINT8 zoomSmoothQueueEntriesGet (UINT16 msgGrp, UINT8* Entries)
 * @brief	        Get Queue Entries value
 * @param	  [in] msgGrp : typedef enum ZoomSmoothCtrlType_e
 * @param	  [in] offset : buffer offset value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
zoomSmoothQueueEntriesGet (
	UINT16 msgGrp,
	UINT8* Entries
)
{
	UINT8 ret;
	ZoomSmoothQue_t* pMsg;

	//printf( "\n>>>>>>>>>osMsgAccept<<<<<<<<<<\n\n" );
	ret = FAIL;

	/* msg accept poroity "System" >> "Button" >> "Host" */
	pMsg = &zoomSmoothQueCtrl[msgGrp];
	if (pMsg->QueEntries != 0) {
		if (Entries) {
			*Entries = pMsg->QueEntries;
		}
		ret = SUCCESS;
	}

	if (ret != SUCCESS) {
		if (Entries) {
			*Entries = 0x00;
		}
	}

	return ret;
}

/**
 * @fn		  UINT8 INTR_zoomSmoothQueueEntriesGet (UINT16 msgGrp, UINT8* Entries)
 * @brief	        Get Queue Entries value
 * @param	  [in] msgGrp : typedef enum ZoomSmoothCtrlType_e
 * @param	  [in] offset : buffer offset value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
INTR_zoomSmoothQueueEntriesGet (
	UINT16 msgGrp,
	UINT8* Entries
)
{
	UINT8 ret;
	ZoomSmoothQue_t* pMsg;

	//printf( "\n>>>>>>>>>osMsgAccept<<<<<<<<<<\n\n" );
	ret = FAIL;

	/* msg accept poroity "System" >> "Button" >> "Host" */
	pMsg = &zoomSmoothQueCtrl[msgGrp];
	if (pMsg->QueEntries != 0) {
		if (Entries) {
			*Entries = pMsg->QueEntries;
		}
		ret = SUCCESS;
	}

	if (ret != SUCCESS) {
		if (Entries) {
			*Entries = 0x00;
		}
	}

	return ret;
}

/**
 * @fn		  void videoSmoothZoomSet_NBlocking(UINT8 en)
 * @brief	        video smooth zoom enable/disable set (NBlocking)
 * @param	  [in] en : enable/disable
 * @retval	  NULL
 * @see 	        NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void
videoSmoothZoomSet_NBlocking(
	UINT8 en
)
{
	if (en) {
		intrSmoothZoomCtrl.zoomSmoothEn = en;
	}
	else {
		if (intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE) {
			intrSmoothZoomCtrl.zoomReleaseKeySts = INTR_SMOOTH_ZOOM_RELEASE_KEY;
			intrSmoothZoomCtrl.zoom3AUpdateDlyFrame = 2;
		}
		else {
			osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
			osMsgPost(SP1K_MSG_DZOOM_3A_UPDATE);
		}
	}
	videoSmoothZoomDbg("G_ZoomSmoothEn=%bu,%bu,%bu,%bu\n",intrSmoothZoomCtrl.zoomSmoothEn,intrSmoothZoomCtrl.zoomReleaseKeySts,intrSmoothZoomCtrl.zoom3AUpdateDlyFrame,intrSmoothZoomCtrl.zoomFactUpdate);

}

/**
 * @fn		  void videoSmoothZoomSet_Blocking(UINT8 en)
 * @brief	        video smooth zoom enable/disable set (Blocking)
 * @param	  [in] en : enable/disable
 * @retval	  NULL
 * @see 	        NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void
videoSmoothZoomSet_Blocking(
	UINT8 en
)
{
	en = en;
}

/**
 * @fn		  UINT8 en videoSmoothZoomGet_NBlocking(void)
 * @brief	        video smooth zoom enable/disable Get (NBlocking)
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
videoSmoothZoomGet_NBlocking(
	void
)
{
	return intrSmoothZoomCtrl.zoomSmoothEn;
}

/**
 * @fn		  UINT8 en videoSmoothZoomGet_Blocking(void)
 * @brief	        video smooth zoom enable/disable Get (Blocking)
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
videoSmoothZoomGet_Blocking(
	void
)
{
	return INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE;
}

/**
 * @fn		  UINT8 videoSmoothZoomStatusGet_NBlocking(void)
 * @brief	        video smooth zoom status Get (NBlocking)
 * @param	  NONE
 * @retval	  FAIL = not Finish,SUCCESS = Finish
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoSmoothZoomStatusGet_NBlocking(
	void
)
{
	if ((VideoZoomNBCtrl.zoomStatus == VIDEO_ZOOM_STATUS_FINISH) && (intrSmoothZoomCtrl.zoom3AUpdateDlyFrame == 0)) {
		return SUCCESS;
	}
	else {
		videoSmoothZoomSysInfo("%bu,%bu\n",VideoZoomNBCtrl.zoomStatus,intrSmoothZoomCtrl.zoom3AUpdateDlyFrame);
		return FAIL;
	}
}

/**
 * @fn		  UINT8 videoSmoothZoomStatusGet_Blocking(void)
 * @brief	        video smooth zoom status Get (Blocking)
 * @param	  NONE
 * @retval	  FAIL = not Finish,SUCCESS = Finish
 * @see 	  NULL
 * @author	  LinJieCheng
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
videoSmoothZoomStatusGet_Blocking(
	void
)
{
	return SUCCESS;
}

