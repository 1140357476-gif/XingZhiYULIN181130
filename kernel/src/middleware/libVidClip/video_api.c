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
#include "video_def.h" 
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
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT8 videoStorageWriteSpeedTest(void)
 {
 #if 0
	//UINT32 TimeT0,TimeT1,TimeT2,TimeT3,TimeT4;
	UINT32 TimeT1,TimeT4;
	UINT16 fHandle_test;
	//HAL_GlobalTimerRead10us(&TimeT2);
	UINT8 err = SUCCESS;

	if( DOS_GetDiskSpace() > 128){
		fHandle_test = vfsFileOpen("TESTCARDTXT", K_DOS_NewFile, 100);
		//HAL_GlobalTimerRead10us(&TimeT0);
		vfsFileWrite(fHandle_test, K_File_SrcDataDram2Card, (UINT32)16*1024, 0x300000,0);
		HAL_GlobalTimerRead10us(&TimeT4);
		vfsFileWrite(fHandle_test, K_File_SrcDataDram2Card, (UINT32)64*1024, 0x300000,0);
		HAL_GlobalTimerRead10us(&TimeT1);
		DOS_FileClose(fHandle_test);
		vfsFileDelete("TESTCARDTXT");
		//HAL_GlobalTimerRead10us(&TimeT3);
		StorageSpeed = ((UINT32)64*1000*100)/(TimeT1-TimeT4);
	}
	else{
		err = FAIL;
	}
	return err;
#else
	return FAIL;
#endif
}
/**
* @fn 	   UINT8 videoRecFrontRateInterval(void)
* @brief	   video clip front rate set
* @param	   frontRate
* @retval      NONT
* @see	   NULL
* @author    rf.wang
* @since	   2011-4-21
* @todo	   N/A
* @bug	   N/A
*/
void videoRecFrontRateInterval(/*UINT8 frameRate ,*/UINT8  frontRate )
{
           //vidClipCtrl.frameRate = frameRate;
	 //g_vidAviFrameRate = vidAviFrameRate;
	UINT8 frameRate;
	XBYTE[0x2a00] = frontRate;
	if((frontRate & 0x10)==0){
		g_vidAviFrameRate = 30;
		aeFrameIntervalSet(0);
	}
	else if(frontRate==0x11){
		g_vidAviFrameRate = 15;
		aeFrameIntervalSet(1);
	}else {
//			printf("ratesetErr!\n");
	}
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
	if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
		g_vidAviFrameRate=60;
		}
	vidClipCtrl.frameRate = g_vidAviFrameRate;
	
}
 /**
  * @fn 	   UINT8 videoRecFrontRateGet(void)
  * @brief	   video clip front rate Get
  * @param	   NONE
  * @retval      NONT
  * @see	   NULL
  * @author    rf.wang
  * @since	   2011-4-21
  * @todo	   N/A
  * @bug	   N/A
 */
UINT8 videoRecFrontRateGet()
{
	UINT8  frontRate;
	frontRate =XBYTE[0x2a00] ;
	return frontRate;
}
 //-----------------------------------------------------------------------------
//videoRecStatusGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoRecStatusGet(void)
 * @brief		video record status get
 * @param	NONE
 * @retval	record status
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 IntrvideoRecStatusGet(void)
{
	return vidClipCtrl.recIdle;
}
 //-----------------------------------------------------------------------------
//videoRecStatusGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoRecStatusGet(void)
 * @brief		video record status get
 * @param	NONE
 * @retval	record status
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 videoRecStatusGet(void) USING_0
{
	return vidClipCtrl.recIdle;
}
//-----------------------------------------------------------------------------
//videoRecTimeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 videoRecTimeGet(void)
 * @brief		get video record  time
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 videoRecTimeGet(void)
{
	UINT32 time;

	UINT32 audRingCnt = 0;
//xian -- temp for fpga verify
	HAL_AudRingBufCntRead( &audRingCnt);
	audRingCnt += (vidClipCtrl.AudChunkCnt<<SECPERCHK);
	time = (UINT32)((float)audRingCnt/((float)audBytesPerSec/(float)1000));
/*
	{
		//time = audRingCnt*1000/audBytesPerSec;
		UINT32 a0 = audRingCnt/audBytesPerSec * 1000;
		UINT32 a1 = (audRingCnt%audBytesPerSec)*1000/audBytesPerSec;
		UINT32 a2 = a0+a1;
		if(time!=a2){
		printf("@@@@%lu,%lu\n",time,a2);
			}
	}
	*/
	return time;

}

//xian ++ 20090211
/**
 * @fn		  UINT8 videoRecParamSet(UINT8 paraId, UINT16 value)
 * @brief	  set video record parameters by parameter ID
 * @param	  [in] paraId : parameter ID
 * @param	  [in] value : parameter value
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecParamSet(UINT8 paraId, UINT16 value)
{
	if (paraId >= VD_PARAM_MAX) {
		return FAIL;
	}

	g_vidParam[paraId] = value; 			  
	return SUCCESS;
}

/**
 * @fn		  UINT16 videoRecParamGet(UINT8 paraId)
 * @brief	  get record parameters by parameter ID
 * @param	  [in] paraId
 * @retval	  return = parameter value
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16 videoRecParamGet(UINT8 paraId)
{
	if (paraId >= VD_PARAM_MAX) {
		return 0;
	}

	return g_vidParam[paraId];
}
//-----------------------------------------------------------------------------
//videoPvFreeAddrGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 videoPvFreeAddrGet(void)
 * @brief		free address get  when video preview
 * @param	NONE
 * @retval	free address
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 videoPvFreeAddrGet(void)
{
	//return K_SDRAM_AviCaptureIdx1Addr;
	return vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_END);
}

//-----------------------------------------------------------------------------
//videoRecSizeSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecSizeSet(UINT16 size)
 * @brief	  video resolution set
 * @param	  [in] size : image resolution,HD:1280x720,D1:848x480,VGA:640x480 ,QVGA :320x240
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecSizeSet (UINT16 size) USING_0
{
	//g_vidParam[VD_DUMY_MODE] = VD_DUMY_MODE_SIZE0;
	//g_vidParam[VD_DUMY_MODE] = VD_DUMY_MODE_COPY_FRAME;
	g_vidParam[VD_DUMY_MODE] = VD_DUMY_MODE_COPY_IDX;
	if(size == VID_1080p){
		vidClipCtrl.imgWidth  = 1920;
		vidClipCtrl.imgHight = 1088;
		//g_vidParam[VD_DUMY_MODE] = VD_DUMY_MODE_COPY_FRAME;
	}
	if(size == VID_HD){
		vidClipCtrl.imgWidth  = 1280;
		vidClipCtrl.imgHight = 720;
		//g_vidParam[VD_DUMY_MODE] = VD_DUMY_MODE_COPY_FRAME;
	}
	else if(size == VID_D1){
		vidClipCtrl.imgWidth  = 720; //848; //modify for D1 size
		vidClipCtrl.imgHight = 480;
	}
	else if(size == VID_VGA) {
		vidClipCtrl.imgWidth  = 640;
		vidClipCtrl.imgHight = 480;
	}
	else if(size == VID_QVGA){
		vidClipCtrl.imgWidth  = 320;
		vidClipCtrl.imgHight = 240;
	}
	#ifdef videoDebug
	//printf("vidW:%d vidH:%d\n",vidClipCtrl.imgWidth,vidClipCtrl.imgHight);
	#endif
}

//-----------------------------------------------------------------------------
//videoRecFrameRateSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecFrameRateSet(UINT8 rate)
 * @brief	  video frame rate set
 * @param	  [in] rate : frame rate
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecFrameRateSet (UINT8 rate)
{
	extern UINT32 g_bitRate;
	vidClipCtrl.frameRate = rate;
	/*switch(rate) {
		case 0:
			vidClipCtrl.frameRate = 30;
			break;
		case 1:
			vidClipCtrl.frameRate = 15;
			break;
		case 2:
			vidClipCtrl.frameRate = 60;
			break;
	}*/
	if(vidClipCtrl.frameRate){
		vidBrcTagSize = g_brcTargetSize = (g_bitRate>>3)  / vidClipCtrl.frameRate;
	}
	if(g_vidParam[VD_FRONT_FPS] == 0)
		g_vidParam[VD_FRONT_FPS] = 30; //for compatibility
}

void videoRecAviFrameRateSet (UINT8 rate)
{
	if (rate>0){
		g_vidAviFrameRate = rate;
	}else{
		g_vidAviFrameRate = 30;
	}
}
//-----------------------------------------------------------------------------
//videoRecSizeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecSizeGet(UINT16* width, UINT16* height)
 * @brief	  get video record size
 * @param	  [in] width : pixel width
 * @param	  [in] height : pixel height
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecSizeGet (UINT16* width, UINT16* height)
{
	*width = vidClipCtrl.imgWidth;
	*height = vidClipCtrl.imgHight; 
}

//-----------------------------------------------------------------------------
//videoRecQtableSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecQTableSet(UINT8 value)
 * @brief	  videoRecQTableSet
 * @param	  [in] value :	0 is Q_85,1 is Q_80,2 is Q_50
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecQTableSet(UINT8 value)
{
	vidClipCtrl.Qtable = value;
	/*
	switch(value) {
		case 0:
			vidClipCtrl.Qtable = K_Q80;
			break;
		case 1:
			vidClipCtrl.Qtable = K_Q70;
			break;
		case 2:
			vidClipCtrl.Qtable = K_Q50;
			break;
	}
	*/
}


 /**
 * @yuvformat: '0' for yuv422;	<--> VD_REC_YUV_422
 *					 '1' for yuv422_420; <--> VD_REC_YUV_422_420
 *					 '2' for yuv420;  <--> VD_REC_YUV_420
 *					 others is reserved
 */
/**
 * @fn		  void videoRecYUVFormatSet(UINT8 yuvformat)
 * @brief	  set record MJPEG YUV format
 * @param	  [in] yuvformat : VD_REC_YUV_422/VD_REC_YUV_422_420/VD_REC_YUV_420
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void
videoRecYUVFormatSet(
	UINT8 yuvformat
)
{// '0' for yuv422; '1' for yuv422_420; '2' for yuv420; others is reserved
   if(seamLessLpRecEn==0){
		if (yuvformat<3){
			g_vidParam[VD_REC_YUV_FORMAT] = yuvformat;
		}
		else {
			g_vidParam[VD_REC_YUV_FORMAT] = VD_REC_YUV_422;
		}
   	}
   else{
   	  g_vidParam[VD_REC_YUV_FORMAT] = VD_REC_YUV_422;
   	}
}

UINT32 videoRecRemainTimeGet()
{
	UINT32 freeCluster = DOS_FreeDiskSpaceByCluster();
	UINT32 clusterSize = vfsClusterSizeGet();
	UINT32 audbps = (UINT32)vidClipCtrl.sampRate * (UINT32)SAMPLE_BIT / 8;
	UINT32 vidbps = g_brcTargetSize*g_vidAviFrameRate;
	UINT32 time = freeCluster/((audbps+vidbps)/clusterSize);
	//printf("RemainTime:%lu,%lu,%lu,%lu,%lu\n",time,freeCluster,audbps,vidbps,clusterSize);
	return time;
}
