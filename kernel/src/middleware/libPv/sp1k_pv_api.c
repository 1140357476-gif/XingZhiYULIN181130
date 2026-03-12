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
#include "Hal_device.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "pv_task.h"
#include "initio.h"
#include "app_still_view.h"
#include "app_video_view.h"
#include "cdsp_init.h"
#include "cdsp_task.h"
#include "cdsp_misc.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_front_api.h"
#include "sp1k_pv_api.h"
#include "ae_api.h"
#include "disp_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "hal.h"
#include "hal_dram.h"
#include "os_api.h"
#include "pv_debug.h"
#include "gprm.h"
#include "sys_mem_init.h"
#include "hal_disp.h"
#include "sp1k_ae_api.h"
#include "App_ui_para.h"
#include "app_menu_micro.h"
#include "sp1k_video_api.h"   //rf.wang++
#include "video_mem.h"
#include "sp1k_disk_api.h"
#include "sp1k_gpio_api.h"
#include "video_cap_mem.h"
#include "pv_zoom.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SP1K_PV_API_TAG_TODO

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

UINT16 pvCropHSize;
UINT16 pvCropVSize;
UINT8 videoSuperPreivewEn;
UINT8 stillSuperPreivewEn;
UINT8 hdPvsizeStatus=0; //0:pv size 6/8, 1: pv size 8/8

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//#if (STILL_NEW_ZOOM == 1)
extern appPvZoom_NBlocking_t xdata pvZoomNBCtrl;
//#else
//extern appPvZoom_t xdata PvZoomCtrl;
//#endif
extern appVideoZoom_NBlocking_t xdata VideoZoomNBCtrl;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//-----------------------------------------------------------------------------
//sp1kFrontInit
//-----------------------------------------------------------------------------

/**
 * @fn        void sp1kFrontInit(void)
 * @brief     Init front setting. It must be called before the system enters peview mode
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kFrontInit(
	void
)
{
	UINT8 sensorId;

	/* init front. */
	sensorId = sp1kFrontSensorInit();
	sp1kFrontDevActSet(sensorId);
	sp1kFrontPowerOn(sensorId);

	sp1kFrontInitPrintf("sensorId = %bu\n",sensorId);
}


/**
 * @fn        UINT8 sp1kPreviewSet(UINT8 vidMode, UINT16 pvHSize, UINT16 pvVSize, UINT16 fact, UINT8 pbFreeze)
 * @brief     enter still/video preview mode
 * @param     [in] vidMode :
 *							0: Still preview mode
 *							1: Video preview mode
 * @param     [in] pvHSize
 *							still preview buffer : 320x240(default), it depends on the size of display usually
 *							video preview buffer : 1280x720(maximum), it depends the size of video you wanna record
 *							yuv422 : h need 16x ; yuv420 : h need 16x
 * @param     [in] pvVSize : the same as pvHSize
 *                                          yuv422 : v need 8x ; yuv420 : v need 16x
 * @param     [in] fact : zoom factor
 * @param     [in] pbFreeze :
 *							0: show preview buffer immediately
 *							1: preview will not show immediately until sp1kPrevewUnfreeze() is called.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kPreviewSet(
	UINT8 vidMode,
	UINT16 pvHSize,
	UINT16 pvVSize,
	UINT16 fact,
	UINT8 pbFreeze
)
{
	UINT8 sts = 0;
	UINT16 frontHSize, frontVSize;
	UINT8 frameRate;
	//uiPara_t* puiPara = appUiParaGet();
	UINT16 recwidth;
	UINT16 recheight;
	UINT8 type;

	sp1kPreviewSetPrintf("vidmode=%bd\n",vidMode);
	sp1kPreviewSetPrintf("pv:%u, %u\n", pvHSize, pvVSize);

	(vidMode == 0) ? osSysModeSet(OS_SYS_MODE_STILL_PV) : osSysModeSet(OS_SYS_MODE_VIDEO_PV);

	/* set front2cdps gating enable */
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

	HAL_CdspFrmCtrlEnSet(0);
	HAL_CdspRawImgSource(1);

	if (vidMode == 0) {
		/* still preview */

		zoomDevActGet(ZOOM_LOGI_STILL_MODE,	&type);

		#if 0
		#if (STILL_NEW_ZOOM == 1)
		pvZoomNBCtrl.zoomAction = PV_ZOOM_ACTION_INITIAL;
		#endif
		#endif

		if ( type == ZOOM_LOGI_NONBLOCKING_TYPE ) {
			pvZoomNBCtrl.zoomAction = PV_ZOOM_ACTION_INITIAL;
		}

		HAL_Dram1stPathFrmBufAAddrSet(K_SDRAM_PvBufAddrA);
		HAL_Dram1stPathFrmBufBAddrSet(K_SDRAM_PvBufAddrB);
		HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_PvBufAddrC);
		HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_PvBufAddrD);

		sp1kPreviewSetPrintf("s fbuf A=0x%lx\n",K_SDRAM_PvBufAddrA);
		sp1kPreviewSetPrintf("s fbuf B=0x%lx\n",K_SDRAM_PvBufAddrB);
		sp1kPreviewSetPrintf("s fbuf C=0x%lx\n",K_SDRAM_PvBufAddrC);
		sp1kPreviewSetPrintf("s fbuf D=0x%lx\n",K_SDRAM_PvBufAddrD);

		HAL_Dram2ndPathFrmBufAAddrSet(K_SDRAM_Pv2BufAddrA);  /* second path disp address */
		HAL_Dram2ndPathFrmBufBAddrSet(K_SDRAM_Pv2BufAddrB);
		HAL_Dram2ndPathFrmBufCAddrSet(K_SDRAM_Pv2BufAddrC);
		HAL_Dram2ndPathFrmBufDAddrSet(K_SDRAM_Pv2BufAddrD);

		sp1kPreviewSetPrintf("s fbuf2 A=0x%lx\n",K_SDRAM_Pv2BufAddrA);
		sp1kPreviewSetPrintf("s fbuf2 B=0x%lx\n",K_SDRAM_Pv2BufAddrB);
		sp1kPreviewSetPrintf("s fbuf2 C=0x%lx\n",K_SDRAM_Pv2BufAddrC);
		sp1kPreviewSetPrintf("s fbuf2 D=0x%lx\n",K_SDRAM_Pv2BufAddrD);

		sp1kPreviewSetPrintf("common end=0x%lx\n",K_SDRAM_CommonEndAddr);
		sp1kPreviewSetPrintf("end addr 0x%lx\n", K_SDRAM_TotalSize );

		sp1kPreviewSetPrintf("s awb win addr=0x%lx\n",K_SDRAM_PvAwbWndAddr);
		sp1kPreviewSetPrintf("s moe addr=0x%lx\n",K_SDRAM_PvMoeTmpAddr);
		sp1kPreviewSetPrintf("s ae ii addr=0x%lx\n",K_SDRAM_PvAeAddr);
		sp1kPreviewSetPrintf("s ae win addr=0x%lx\n",K_SDRAM_PvAeWndAddr);

		sp1kPreviewSetPrintf("s fd ii addr=0x%lx\n",K_SDRAM_PvFdIIAddr);
		sp1kPreviewSetPrintf("s dist winA addr=0x%lx\n",K_SDRAM_PvDistEdgeWndAddrA);
		sp1kPreviewSetPrintf("s dist winB addr=0x%lx\n",K_SDRAM_PvDistEdgeWndAddrB);
		sp1kPreviewSetPrintf("s dist me addr=0x%lx\n",K_SDRAM_PvDistMeAddr);

		//sp1kPreviewSetPrintf("s icat=0x%lx\n",icat_TestBufAddr);
		//sp1kPreviewSetPrintf("s 0x2750=0x%bx\n",XBYTE[REG_Dram_SdramType]);

		/* set 1st & 2st 4 buffer mode */
		//printf("##still =%bu\n",sp1kVideoRecFrameBufNumGet());
		HAL_DramFrmBufNumSet(2);
	}
	else {
		/*video preview */

		#if 0
		#if (VIDEO_NEW_ZOOM == 1)
		VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_INITIAL;
		#endif
		#endif

		zoomDevActGet(ZOOM_LOGI_VIDEO_MODE,&type);

		if ( type == ZOOM_LOGI_NONBLOCKING_TYPE ) {
			VideoZoomNBCtrl.zoomAction = VIDEO_ZOOM_ACTION_INITIAL;
		}
		if ( type == ZOOM_LOGI_BLOCKING_TYPE ) {
			extern UINT8 xdata video_zoomInFlag;
			extern UINT8 xdata video_zoomOutFlag;
			video_zoomInFlag = 1;
			video_zoomOutFlag = 1;
		}//pc cam HD err

		//vidRecFrameBufNumSet(3);
		//sysMemVideoPvAddrUpdate(vidRecFrameBufNumGet());
		#if 1
		#if 0
		if(sysMemTotSizeGet()== DEV_MEM_SIZE_128M){
			sp1kVideoRecFrameBufNumSet(3);
		}else if(sp1kVideoRecParamGet(VD_CAPTURE)) {
			sp1kVideoRecFrameBufNumSet(4);
		}else{
			sp1kVideoRecFrameBufNumSet(3);
		}
		#endif
		sp1kYuvBufModeSet();
		sp1KVideoRecFramebufModeSet(sp1kVideoRecFrameBufNumGet());
		//printf("##video=%bu\n",sp1kVideoRecFrameBufNumGet());
		if (((pvHSize == 640) && (pvVSize == 480)) ||
			((pvHSize == 320) && (pvVSize == 240))) {
			//sysMemFrameBufAddrUpdate(MEM_FB_VGA);
			//vidRecMemFrameBufAddrUpdate(MEM_FB_VGA);
			sysMemVideoPvAddrUpdate(MEM_FB_VGA,pvHSize,pvVSize);

		}
		else if (pvHSize == 960 && pvVSize == 540) {
			//sysMemFrameBufAddrUpdate(MEM_FB_HD_960);
			sysMemVideoPvAddrUpdate(MEM_FB_HD_960,pvHSize,pvVSize);
		}
		else if ((pvHSize == 640) && (pvHSize == 432)) { //modify for D1 size
			//sysMemFrameBufAddrUpdate(MEM_FB_D1);
			sysMemVideoPvAddrUpdate(MEM_FB_D1,pvHSize,pvVSize);
		}
		else {
			//sysMemFrameBufAddrUpdate(MEM_FB_HD);
			sysMemVideoPvAddrUpdate(MEM_FB_HD,pvHSize,pvVSize);
		}

		/*
		if ((puiPara->VideoSize == MOV_SIZE_1080P)) {
			 sysMemFrameBufAddrUpdate(MEM_FB_1080p);
		}
		*/

		sp1kVideoRecSizeGet(&recwidth,&recheight);

		if ((recwidth == 1920 && recheight == 1088) && (pvHSize == 1280 && pvVSize == 720) ) {
			//sysMemFrameBufAddrUpdate(MEM_FB_1080p);
			sysMemVideoPvAddrUpdate(MEM_FB_1080p,pvHSize,pvVSize);
		}
		#endif
		if(videoRecParamGet(VD_CAPTURE))
		{
			speedyCaptureMemFrameBufAddrUpdate();
		}
		else
		{
			vidRecMemFrameBufAddrUpdate();
		}
		HAL_Dram1stPathFrmBufAAddrSet(K_SDRAM_AviPvwBufAddrA);
		HAL_Dram1stPathFrmBufBAddrSet(K_SDRAM_AviPvwBufAddrB);

		/* get frame rate mode */
		aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);

		#if (VGA_60FPS_2ST_PATH_EN == 0)
		if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
			HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrC_60FPS);
			HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_AviPvwBufAddrD_60FPS);
		}
		else
		#endif
		{
			if(sp1kVideoRecFrameBufNumGet()==2){
					HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrA);
					HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_AviPvwBufAddrB);
			}else if(sp1kVideoRecFrameBufNumGet()==3){
					HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrC);
			}else{
					HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrC);
					HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_AviPvwBufAddrD);
			}
		}
		sp1kPreviewSetPrintf("v fbuf A=0x%lx\n",K_SDRAM_AviPvwBufAddrA);
		sp1kPreviewSetPrintf("v fbuf B=0x%lx\n",K_SDRAM_AviPvwBufAddrB);

		#if (VGA_60FPS_2ST_PATH_EN == 0)
		if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
			sp1kPreviewSetPrintf("v fbuf C=0x%lx\n",K_SDRAM_AviPvwBufAddrC_60FPS);
			sp1kPreviewSetPrintf("v fbuf D=0x%lx\n",K_SDRAM_AviPvwBufAddrD_60FPS);
		}
		else
		#endif
		{
			sp1kPreviewSetPrintf("v fbuf C=0x%lx\n",K_SDRAM_AviPvwBufAddrC);
			sp1kPreviewSetPrintf("v fbuf D=0x%lx\n",K_SDRAM_AviPvwBufAddrD);
		}

		HAL_Dram2ndPathFrmBufAAddrSet(K_SDRAM_Avi2PvwBufAddrA);	//will : second path disp address
		HAL_Dram2ndPathFrmBufBAddrSet(K_SDRAM_Avi2PvwBufAddrB);
		HAL_Dram2ndPathFrmBufCAddrSet(K_SDRAM_Avi2PvwBufAddrC);
		HAL_Dram2ndPathFrmBufDAddrSet(K_SDRAM_Avi2PvwBufAddrD);

		sp1kPreviewSetPrintf("v fbuf2 A=0x%lx\n",K_SDRAM_Avi2PvwBufAddrA);
		sp1kPreviewSetPrintf("v fbuf2 B=0x%lx\n",K_SDRAM_Avi2PvwBufAddrB);
		sp1kPreviewSetPrintf("v fbuf2 C=0x%lx\n",K_SDRAM_Avi2PvwBufAddrC);
		sp1kPreviewSetPrintf("v fbuf2 D=0x%lx\n",K_SDRAM_Avi2PvwBufAddrD);

		sp1kPreviewSetPrintf("v awb win addr=0x%lx\n",K_SDRAM_AviAwbWndAddr);
		sp1kPreviewSetPrintf("v moe addr=0x%lx\n",K_SDRAM_AviMoeTmpAddr);
		sp1kPreviewSetPrintf("v ae ii addr=0x%lx\n",K_SDRAM_AviAeAddr);
		sp1kPreviewSetPrintf("v ae win addr=0x%lx\n",K_SDRAM_AviAeWndAddr);
		sp1kPreviewSetPrintf("v dist winA addr=0x%lx\n",K_SDRAM_AviDistEdgeWndAddrA);
		sp1kPreviewSetPrintf("v dist winB addr=0x%lx\n",K_SDRAM_AviDistEdgeWndAddrB);
		sp1kPreviewSetPrintf("v dist me addr=0x%lx\n",K_SDRAM_AviDistMeAddr);

		//HAL_DramCodeSentry(K_SDRAM_AviMoeTmpAddr, K_SDRAM_AviMoeTmpAddr+512, K_SDRAM_CodeSentryRedirectAddr);
		if(sp1kVideoRecFrameBufNumGet()==2){
			#if (VGA_60FPS_2ST_PATH_EN == 0)
			if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
				HAL_DramFrmBufNumSet(2);
			}
			else
			#endif
			{
				HAL_DramFrmBufNumSet(0);
			}
		}
		else if(sp1kVideoRecFrameBufNumGet()==3){
				HAL_DramFrmBufNumSet(1);
			}
		else{
			HAL_DramFrmBufNumSet(2);
		}
	}

	/* limit frame buffer size to be <= front size. */
	frontPreviewSizeGet(0, &frontHSize, &frontVSize);
	sp1kPreviewSetPrintf("frontHSize=%d,frontVSize=%d\n",frontHSize,frontVSize);

	if ( pvHSize > frontHSize ) {

		if(pvHSize*3 > pvVSize*4){//16:9
			pvVSize = (frontHSize * 9/16 );
		}
		else{
			pvVSize = (frontHSize * 3/4 );
		}

		pvHSize = frontHSize;
	}

	sp1kPreviewSetPrintf("pvHSize=%d,pvVSize=%d\n",pvHSize,pvVSize);

	//if ( pvVSize > frontVSize ) {
	//	pvVSize = frontVSize;
	//}

	if (vidMode == 0) {
		/* still mode */

		/* set first path h,v size */
		HAL_Dram1stPathFrmBufSizeSet(pvHSize, pvVSize);

		/* set second path h size */
		HAL_Dram2ndPathFrmBufSizeSet(320);
		pvSizeSet(pvHSize, pvVSize);

		/* set display source & 2st path enable/disable */
		//HAL_DramDispBufSel(0);
		//HAL_Cdsp2ndPathCfg(1,0,0);
		//HAL_Cdsp2ndPathEnSet(0);
		HAL_DramDispBufSel(2);
		//HAL_Cdsp2ndPathCfg(0,0,0);
		HAL_Cdsp2ndPathEnSet(1);

		if (sp1kSuperPreviewStatusGet(0) == 1) {
			HAL_DramDispBufSel(0);
			XBYTE[REG_Dram_Master0b_Sel] = 0x40;	/* set display priority more than CDSP */
	 		//HAL_Cdsp2ndPathEnSet(0);
		}
		else {
			XBYTE[REG_Dram_Master0b_Sel] = 0x00;	/* set CDSP priority more than display */
		}
    }
    else {
		/*video mode*/

		/* set first path h,v size */
		HAL_Dram1stPathFrmBufSizeSet(pvHSize, pvVSize);

		/* set second path h size */
		HAL_Dram2ndPathFrmBufSizeSet(320);
		pvSizeSet(pvHSize, pvVSize);
		printf("pvHsize=%d,pvVsize=%d\n",pvHSize, pvVSize);
		//HAL_Cdsp1stPathWriteOffset(0, 0);
 		//HAL_Cdsp2ndPathWriteOffset(0, 0);

		/* set display source & 2st path enable/disable */
		#if 1
 		if(sp1kVideoRecFrameBufNumGet()==2){
			#if (VGA_60FPS_2ST_PATH_EN == 0)
			if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
	 			HAL_DramDispBufSel(0);
	 			HAL_Cdsp2ndPathEnSet(0);
	 		}
	 		else
			#endif
			{
				HAL_DramDispBufSel(2);
				HAL_Cdsp2ndPathEnSet(1);
	 		}
 		}
		else if(sp1kVideoRecFrameBufNumGet()==3){
			#if 0
			/* 2nd Path disable ,display data source from 1st path */
			HAL_DramDispBufSel(0);
			HAL_Cdsp2ndPathEnSet(0);
			#else
			/* 2nd path enable, display data source from 2nd path*/
			HAL_DramDispBufSel(2);
			HAL_Cdsp2ndPathEnSet(1);
			#endif
		}
		else{
			if ((frameRate == SP1K_FRAME_RATE_60) || (frameRate == SP1K_FRAME_RATE_50)) {
				#if (VGA_60FPS_2ST_PATH_EN == 0)
				HAL_DramDispBufSel(0);
				HAL_Cdsp2ndPathEnSet(0);
				#else
				HAL_DramDispBufSel(2);
				HAL_Cdsp2ndPathEnSet(1);
				#endif
			}
			else{
				HAL_DramDispBufSel(2);
				HAL_Cdsp2ndPathEnSet(1);
			}
		}

		if (sp1kSuperPreviewStatusGet(1) == 1) {
			HAL_DramDispBufSel(0);
			XBYTE[REG_Dram_Master0b_Sel] = 0x40;	/* set display priority more than CDSP */
	 		//HAL_Cdsp2ndPathEnSet(0);
		}
		else {
			XBYTE[REG_Dram_Master0b_Sel] = 0x00;	/* set CDSP priority more than display */
		}
		#endif

		//cdspTDEn(1);
    }

	HAL_Cdsp1stPathWriteOffset(0, 0);
	HAL_Cdsp2ndPathWriteOffset(0, 0);

	frontPreviewModeSet(frontDevActGet());
	frontLensCompensationSet( 0);
	cdspPvParameterSet();

	HAL_CdspIntplmir(1,	1, 1, 1);
	HAL_CdspEdgeSuppressionMir(1 ,1 ,1 ,1);

	HAL_CdspVDramSizeClamp(1, pvHSize, 1, pvVSize, 1);

	/*VDRAM skip sync with VD*/
	HAL_CdspVDramSkipWrSyncSet(1);//mantis  42943

	if (pbFreeze == 1) {
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0xff);
	}
	else {
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);
	}


	/*local reset for 1M capture dram 128 byte overwrite */
    HAL_CdspRstBlock(CDSPRstCDSP);

	HAL_CdspRawImgSource(0);

	/* clear CDSP/DISP zf flag */
	HAL_DramZUpdateClear(0x30);

	HAL_DramPreviewStart(pbFreeze);

	if (vidMode == 0) {
		//pvZoomSet(1, fact, pbFreeze);
		zoomDevZoomSet(ZOOM_LOGI_STILL_MODE,1, fact, pbFreeze);
	}
	else {
		//videoRecZoomFacSet(fact, 0);
		zoomDevZoomSet(ZOOM_LOGI_VIDEO_MODE,0, fact, 0);
	}

	if (pbFreeze == 0) {
		HAL_Cdsp2ndPathCfg(0,0,0);
	}

	/* set front2cdps gating disable */
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

	return sts;
}



/**
 * @fn        void sp1kCDSPInit(void)
 * @brief     CDSP Initialize
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Huiling Chou
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kCDSPInit(
	void
)
{
	cdspInit();
	//cdspWdrInit();
}

//-----------------------------------------------------------------------------
//sp1kPrevewUnfreeze
//-----------------------------------------------------------------------------

/**
 * @fn        void sp1kPrevewUnfreeze(UINT8 opt)
 * @brief     Unfreeze preview after power on
 * @param     [in] opt	0: skip writing frist frame buffer
\n 						1: start writing first frame buffer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-13
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kPrevewUnfreeze(
	UINT8 opt
)
{
	#if 0
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV); //Trigger pv Zupdate
	XBYTE[REG_Dram_LensGainUpdt]=0x04;//{3'b0, mpgsb_usb_en, mpgsb_mjpg_en, mpgsb_dispimg_en, 2'h0}
	XBYTE[REG_Dram_DispSrc_Sel]=0x02;//dispimgsrc_sel
	#else
	//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 0);

	//HAL_DramDispSrcSel(0x02); /* dispimgsrc_sel=mpgSrcBuf */

	//if (osSysModeGet()==OS_SYS_MODE_VIDEO_PV) {
	//	sp1kPrevewUnfreezePrintf("unfre v pv\n");

		if (opt==0) {
			HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,1);
		}
		else {
			HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);	/* if vdram skip write enable, zf update must be done again here*/
		}

		//pvZoomSet(1, PvZoomCtrl.factor, 0); /* video preview mode */

		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300);
		HAL_DramPreviewStart(0);

		HAL_Cdsp2ndPathCfg(0,0,0);

		{	/* mantis bug number 0041135 */
			//#if (STILL_NEW_ZOOM == 1)
			//pvZoomSet(1, pvZoomFctGet(), 0); /* still preview mode */
			zoomDevZoomSet(ZOOM_LOGI_STILL_MODE,1, com_zoomDevZoomFactorGet(ZOOM_LOGI_STILL_MODE), 0);
			//#else
			//HAL_DispScaleSet(320, 240-1, 100, 0); /* 240-1 to workaround 2nd Path last line error */
			//pvZoomSet(1, PvZoomCtrl.factor, 0); /* still preview mode */
			//#endif
			//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);
			//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300);
		}

		//HAL_DispScaleSet(320, 240, 100, 0);
	//}
	//else {
	//	sp1kPrevewUnfreezePrintf("unfre s pv\n");
	//	pvZoomSet(1, PvZoomCtrl.factor, 0); /* still preview mode */
	//}

	#endif
}

/**
 * @fn        void sp1kPreviewCropSizeSet(UINT16 cropHSize, UINT16 cropVSize)
 * @brief     set preview crop size
 * @param     [in] cropHSize : crop horizontal size
 * @param     [in] cropVSize : crop vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kPreviewCropSizeSet(
	UINT16 cropHSize,
	UINT16 cropVSize
)
{
	pvCropHSize = cropHSize;
	pvCropVSize = cropVSize;


	HAL_CdspRoiSizeSet(cropHSize,cropVSize);

	sp1kPreviewCropSizeSetPrintf("pv cropHSize=%d,cropVSize=%d\n",pvCropHSize,pvCropVSize);
}

/**
 * @fn        void sp1kPreviewCropSizeGet(UINT16 *cropHSize, UINT16 *cropVSize)
 * @brief     get preview crop size
 * @param     [out] cropHSize : crop horizontal size
 * @param     [out] cropVSize : crop vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPreviewCropSizeGet(
	UINT16 *cropHSize,
	UINT16 *cropVSize
)
{
   	*cropHSize = pvCropHSize;
	*cropVSize = pvCropVSize;

	sp1kPreviewCropSizeGetPrintf("pv cropHSize=%d,cropVSize=%d\n",pvCropHSize,pvCropVSize);
}

//-----------------------------------------------------------------------------
//sp1k2ndPathCfg
//-----------------------------------------------------------------------------

/**
 * @fn        void sp1k2ndPathCfg(UINT8 en)
 * @brief     2nd Path disp data in dram
 * @param     [in] en : enable 2nd path skip writing image data to dram
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1k2ndPathCfg(
	UINT8 en
)
{
	if (en) {
		HAL_Cdsp2ndPathCfg(0,0,0);
	}
}

/**
 * @fn        void sp1kSuperPreviewSet(UINT8 mode,	UINT8 en)
 * @brief     super preview mode enable/disable (not support record)
 * @param     [in] mode :
 *							0: Still preview mode
 *							1: Video preview mode
 * @param     [in] en :
 *							0: Super Preview Diable
 *							1: Super Preview Enable
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSuperPreviewSet(
	UINT8 mode,
	UINT8 en
)
{
	switch (mode) {
		case 0:
			stillSuperPreivewEn = en;
			break;
		case 1:
			videoSuperPreivewEn = en;
			break;
		default:
			videoSuperPreivewEn = 0;
			stillSuperPreivewEn = 0;
			break;
	}
}


/**
 * @fn        UINT8 sp1kSuperPreviewStatusGet(UINT8 mode)
 * @brief     Get HDMI HD preview mode status
 * @param     [in] mode :
 *							0: Still preview mode
 *							1: Video preview mode
 * @retval     [out] en :
 *							0: Super Preview Diable
 *							1: Super Preview Enable
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kSuperPreviewStatusGet(
	UINT8 mode
)
{
	switch (mode) {
		case 0:
			return stillSuperPreivewEn;
		case 1:
			return videoSuperPreivewEn;
		default:
			return 0;
	}
}
void  sp1kHDPvsizeStatusSet(UINT8 status){
	hdPvsizeStatus=status;
}
UINT8  sp1kHDPvsizeReSet(){
	UINT8 status=0;
	if (!sp1kSD_CdGet()){
		if ((sp1kSDSpeedClassGet() > SD_SPEED_CLASS4) && (sp1kHDPvsizeStatusGet() == 0)&&(sysMemTotSizeGet()== DEV_MEM_SIZE_128M)){//'0':class 0, '1': class 2, '2':class 4, '3':class 6
			sp1kHDPvsizeStatusSet(1);
			status=1;
		}
		else{
			sp1kHDPvsizeStatusSet(0);
			status=0;
		}
	}else{
		sp1kHDPvsizeStatusSet(0);
		status=0;
	}
	return status;
}
UINT8 sp1kHDPvsizeStatusGet(){
	return hdPvsizeStatus;
}
void sp1kYuvBufModeSet(){
	if((sysMemTotSizeGet()== DEV_MEM_SIZE_64M)&&(!(sp1kVideoRecParamGet(VD_CAPTURE)))){
		sp1kVideoRecFrameBufNumSet(2);
	}else{
		sp1kVideoRecFrameBufNumSet(4);
	}
}

