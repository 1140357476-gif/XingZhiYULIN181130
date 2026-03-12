/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
/**
 * @file		video_clip.c
 * @brief		middle ware  API for video module
 * @author		Suny
 * @since		2007-12-19


 * @date		2008-12-12
 */
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "video_def.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
//#pragma OT(8,SPEED) // optimize with speed first

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define __FILE	  __FILE_ID_VIDEO_CLIP__
//#undef videoDebug
//#define debug		{printf("__%d__\n",(UINT16)__LINE__);}
#define CardSize_4G	((UINT32)0xffffffff)
//#define videoTest  0
//#define alignment_64K  1

//#define VID_VLC_BUFA_Intr  (K_SDRAM_AviCapVLCBufAddrA_Intr + (UINT32)130)
//#define VID_VLC_BUFB_Intr  (K_SDRAM_AviCapVLCBufAddrB_Intr + (UINT32)130)
//#define VID_VLC_BUFC_Intr  (K_SDRAM_AviCapVLCBufAddrC_Intr + (UINT32)130)
//#define VID_VLC_BUFD_Intr  (K_SDRAM_AviCapVLCBufAddrD_Intr + (UINT32)130)

//#define Loop1FrmCnt 2
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT8 G_Image_QTableIndex = K_Qxx;

appVidClipCtrl_t vidClipCtrl;

UINT8 vid_FragCnt;
UINT32 vid_ByteAddr,vid_TmpNum;

//UINT32 VID_VLC_BUF_STATUS=0;
//UINT32 VID_VLC_BUF_STATUS_HIGH=0;

UINT32 vidVlcSize[VID_VLCBUFNUM_TOTAL] = {0};
UINT32 vidVlcBufChgCnt=0;
UINT8 flip=0;
UINT8 dropFrameFlagA=0;
UINT8 dropFrameFlagB=0;
UINT32 vidVlcDMACnt=0;
UINT32 DummyFrameCnt=0;
UINT32 totalFrameCnt=0;
UINT32 totalDummyFrame=0;
UINT8 vlcBufNum =0;
UINT32 audBytesPerSec;
UINT32 vidBytesPerSec;
UINT8 vlcABSwitch =0;
//UINT16 FileIdx;
//UINT32 chkFlg=0;
//UINT32 dropFrmCnt=0;
//#if (VIDEO_FRAME_DEBUG_LEVEL>0)
UINT32 vlcOverflowCnt=0;
//#endif
//extern UINT8 slflag;
//#ifdef OpenTwoFile
UINT32 vidIndexNum=0;
//UINT8 idx_f=1;
//UINT8 wf=0;
//#endif

//UINT32 w_idx_T=0;

//#if AVSYNCTEST
//UINT32 vidChunkCntTest;
//#endif

UINT16 g_vidParam[VD_PARAM_MAX] = {0};

UINT8 g_dumyCopy;
UINT8 VideoRecBegin=0;//0 Intr422->420 1:normal 2:main422->420
UINT8 VideoRecFrameAB=0;
//#if	(VIDEO_FRAME_DEBUG_LEVEL>0)
//UINT32 KBPerSec;
//#ifdef VIDEO_REC_BRC_EN
//UINT8 QIdx;
UINT8 QIdxMax=0,QIdxMin=100;
//#endif
UINT32 dumyIdxTotal = 0;
//#endif
//UINT32 G_FragBufSize;
//UINT8 G_FragBufNum;
//UINT16 G_SecPerFrag;

UINT8 g_vidAviFrameRate = 30;
//tq++ 20101116
//UINT8 VlcBufStatus=0;//'0' <half buffer, '1' >= half buffer
//UINT8 DropBalanceEnable=0;
//UINT8 DropFrmDmyCnt[VID_VLCBUFNUM_TOTAL];
//UINT8 DropSglFrmCnt=0;
//UINT32 g_lastMovOff,g_lastMovSize;

//tq--
 UINT8 videoStampFlg=0;//wangrf++
 UINT32 videoBrcSizeAvg = 128;//vlc buf size avg

UINT32 vidUiTimeLast;
UINT32 g_vidCustomUpdateEntry;

UINT32 vidVlcBufSize;
UINT32 vidFragBufSize;
UINT32 vidBrcTagSize;
UINT8 vidTestCnt= 0xff;
UINT8 vidBrcCnt;
UINT32 vidSLDbgTime,vidSLDbgVlcCnt0,vidSLDbgVlcCnt1;
UINT8 vidQestimate;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern vidRecDbgTimeCheck_t vidRecDbgTimeCtrl;
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
//=============================================================================
//Program
//=============================================================================

void videoRecStart (void) USING_0
{
	UINT8 sts;
	/* video record dynamic mem initial */
	//videoRecInitMem();
	//videoRecInitGlobal();
	//if (vidClipCtrl.mode == VID_MODE) {
		//if (vidClipCtrl.recIdle == VID_REC_IDLE) {
			sts = videoRecInit();
			//videoZfUpdateFlag=0;	/*clear zf update fw flag*/
			HAL_DramZUpdateClear(0xb0); /* clear CDSP/DISP zf flag */
			if((vidRecFrameBufNumGet()==4)&& (K_SDRAM_TotalSize<0x800000))
		        {
				ASSERT(0, 1);
				sts=FAIL;
		        }

			if(!sts) {
				vidClipCtrl.recIdle = VID_REC_RUN;
				osActiveFuncSet(pFunc_VideoRec);
			}
			else {
				ASSERT(0, 1);
			}
		//}
	//}
	vidDbgTimerReset();
}

//-----------------------------------------------------------------------------
//videoRecStop
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecStop(void)
 * @brief	  video key off snap process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecStop (void)
{

	vidTimeSliceCheck(1);
	HAL_GlobalRTCSecondIntrSet(0);
	if (vidClipCtrl.recIdle == VID_REC_IDLE) {
		return;
	}
	videoRecAbort();
	vidClipCtrl.recIdle = VID_REC_IDLE;
	vidDbgTimerPrintf();
}

//-----------------------------------------------------------------------------
//videoRecKeyOnPause
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecPause(void)
 * @brief	  video key on Pause	process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecPause (void) USING_0
{
	UINT8 status;
	UINT8 num=0;
	//UINT8 i;
	//UINT8 vlcEn = XBYTE[REG_Dram_VlcIntEn];
	//UINT8 jpgEn = XBYTE[REG_Jpeg_Intr_Enable];
	if(vidClipCtrl.recIdle != VID_REC_RUN) {
		return;
	}
	//ENTER_CRITICAL( status );

	//XBYTE[REG_Dram_VlcIntEn] &= ~0x03;
	//XBYTE[REG_Jpeg_Intr_Enable]&=~0x02;
	ENTER_CRITICAL( status );
	HAL_AudPause();
	vidClipCtrl.recIdle = VID_REC_PAUSE;
	//printf("*p*");
	EXIT_CRITICAL( status );
	/*XBYTE[REG_Front_Dummy0] =0x01;
	while(idxVlcHead != VD_BUF_IDX_EOF){
		UINT8 rst = videoRecVlcDataProcess(0);

		if(rst == FAIL){
			EXIT_CRITICAL( status );
			return;
			}
		videoRecAudDataProcess(1);
		vidVlcDMACnt ++;
	}*/
	//i=i;
	//XBYTE[REG_Front_Dummy0] =0x03;
	//EXIT_CRITICAL( status );
	//XBYTE[REG_Dram_VlcIntEn] = vlcEn;
	//XBYTE[REG_Jpeg_Intr_Enable]=jpgEn;
}

//-----------------------------------------------------------------------------
//videoRecKeyOnResume
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecResume(void)
 * @brief	  video key on Resume process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecResume (void) USING_0
{
	UINT8 status;
	if(vidClipCtrl.recIdle != VID_REC_PAUSE) {
		return;
	}
	//ENTER_CRITICAL( status );
	//#if VLC_BUF_CHG_EN
	//vidVlcBufChgCnt = (vidVlcDMACnt-2)%G_VideoVlcBufNum;
	//#endif
	ENTER_CRITICAL( status );
	videoRecStampSyncRtc();
	vidClipCtrl.recIdle = VID_REC_RUN;
	//printf("*S*");
	HAL_AudResume();
	//XBYTE[0x2709]|=(0x03);
	//EXIT_CRITICAL( status );
	EXIT_CRITICAL( status );
}

//-----------------------------------------------------------------------------
//videoRec
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoRec(void)
 * @brief	  video clip
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRec (void) USING_0
{
	UINT8 i = 1;
	UINT8 ret=SUCCESS;
	vidTimeCheckDbgEnd((UINT32)50,"host",&(vidRecDbgTimeCtrl.vidTimeCheckHost));
	if(idxVlcHead == VD_BUF_IDX_EOF && (vidClipCtrl.VidChunkCnt==0) ){return SUCCESS; }// wait the first vd frame only for fpga's slow vd
	if(VideoRecBegin==0){return SUCCESS;}
	if(vidBgEn){
		if(sD_CdGet()){
			__PRINTF_POS();
			vidTimeCheckDbgStart();
			return FAIL;
		}
	}
	if((XBYTE[0x262c]& 0x01)==0x01){
		__PRINTF_POS();
		osMsgPost(SP1K_MSG_MEDIA_REC_FAIL);
		vidTimeCheckDbgStart();
		return FAIL;
	}

	vidTimeSliceCheck(0);
	//pause process
	if(vidClipCtrl.recIdle != VID_REC_PAUSE) {//{return SUCCESS;}
		videoRecTimeUpdate();
		videoRecTimeSync();
	}
	while((i--) && ( idxVlcHead != VD_BUF_IDX_EOF)){
		ret = videoRecVlcDataProcess(0);
		if(vidClipCtrl.VidChunkCnt){
			videoRecAudDataProcess(0);
		}
		if(ret==FAIL){break;}
	}
	if(vidIndexNum>=videoClusterSize){
		videoRecWriteIndex();
	}
	vidTimeCheckDbgStart();
	return ret;
}
void videoRecTimeSync()
{
	//ENTER_CRITICAL( status );
	UINT32 tmp0 = vidClipCtrl.time /1000;
	UINT32 targetFrameCnt = tmp0 * g_vidAviFrameRate;
	UINT32 frameCnt = vidClipCtrl.VidChunkCnt;// + (UINT32)vlcBufFrameCnt;
	UINT32 time = vidClipCtrl.time %1000;
	_VIDEO_INTR_DISABLE();
	frameCnt+=vlcBufFrameCnt;
	_VIDEO_INTR_ENABLE();
	//targetFrameCnt +=(UINT32)((float)time /msPerFrm );
	targetFrameCnt +=time*g_vidAviFrameRate/1000;
	g_dumyCopy = 0;
	if(!g_vidParam[VD_SLOW_REC_MOD]){
		if(!g_vidParam[VD_DUMY_SPACE]){
			if(targetFrameCnt >(frameCnt+1)) {
				g_dumyCopy = targetFrameCnt - frameCnt;
				//printf("D-%bx\n",g_dumyCopy);
			}
		}
	}
	//EXIT_CRITICAL( status );
}
void videoRecSaveVlc()
{
	UINT8 freeBufStatus;
	UINT32 vlcAddr = vidClipCtrl.DramDMAaddr <<1;
	UINT32 vlcSize = vidClipCtrl.Compsize + 260+HUFFMANSIZE;//420 huffman len
	UINT8 vlcIdx = idxVlcHead;
	UINT8 frameCnt = g_vlcIdxTab[vlcIdx].dumyCnt + g_dumyCopy;
	UINT32 dataAddr = (vidClipCtrl.CurDMATagAddr<<1)+vid_ByteAddr;
	//UINT16 cnt=4;
	UINT8 tmp =3;

	if(g_vidParam[VD_SLOW_REC_MOD]){
		tmp =2;
	}
	freeBufStatus=videoRecWriteToFragBuf(vlcAddr,vlcSize);
	if(freeBufStatus){
		vidDbgDataFlow(0,3,vlcSize);
		//release vlcbuf
		//if(!((slflag==1)&&(seamLessLpRecEn==1))){
			//videoRecVlcFreeBufAdd();
		//}
		if(VideoRecBegin < tmp){//422->420
			UINT8 t0,t1;
			UINT32 addr = (dataAddr>>1)+119+(HUFFMANSIZE>>1);//210: huffman len
			HAL_DramSetAddr(addr, 1);
			HAL_DramWordDataRead(&t0, &t1);
			HAL_DramSetAddr(addr, 0);
			HAL_DramWordDataWrite(0x21, t1);
			VideoRecBegin++;
		}
		//update index
		//do{
		switch(g_vidParam[VD_DUMY_MODE]){
			case VD_DUMY_MODE_SIZE0:
				videoRecWriteVidIndex2Dram(vlcSize-8,1);
				videoRecAddDumy_Size0(0,frameCnt);
				break;
			case VD_DUMY_MODE_COPY_IDX:
				while(frameCnt--){videoRecWriteVidIndex2Dram(vlcSize-8,0);}
				videoRecWriteVidIndex2Dram(vlcSize-8,1);
				break;
			case VD_DUMY_MODE_COPY_FRAME:
				break;
			default:
				while(frameCnt--){videoRecWriteVidIndex2Dram(vlcSize-8,0);}
				videoRecWriteVidIndex2Dram(vlcSize-8,1);
				break;
		}
		//	}while(videoRecWriteToFragBuf(vlcAddr,vlcSize) && --cnt);
		//videoRecVlcFreeBufAdd();
		g_dumyCopy=0;
	}
	if(!videoRecFragSaveCard(0x8000)){
		osMsgPost(SP1K_MSG_MEDIA_REC_FAIL);
	}
	if(freeBufStatus){
		videoRecVlcFreeBufAdd();
	}

}
UINT8 videoRecCheckSpace(UINT32 vlcSize)
{
	UINT32 freeSize = DOS_GetDiskSpace();
	UINT32 aviSize = vidClipCtrl.SecAddr
			+(vidClipCtrl.hdrSize >>SECPERCHK)
			+(vidClipCtrl.idxSize >>SECPERCHK)
			+(vidFragBufSize>>SECPERCHK)
			+((videoClusterSize<<4)>>SECPERCHK)
			;
	UINT32 reqSize = vlcSize+videoClusterSize
			+ vidClipCtrl.hdrSize +videoClusterSize+videoClusterSize
			+ vidFragBufSize+videoClusterSize
			;
	//_VIDEO_INTR_DISABLE();
	//aviSize+=(videoClusterSize*vlcBufFrameCnt)>>SECPERCHK;
	//_VIDEO_INTR_ENABLE();
	reqSize>>=10;
	//printf("<%lx,%lx,%lx,%lx>\n",reqSize,freeSize,vidClipCtrl.Compsize,aviSize);
	if(reqSize>freeSize){
		osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		__PRINTF_POS();
		//printf("card full!!\n");
		//__PRINTF(__FILE,  __LINE__);
		return FALSE;
	}

	if(aviSize>=(0xffffffff>>SECPERCHK)){
		if(DOS_FreeDiskSpaceByCluster()<20){
			__PRINTF_POS();
			osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		}else{
			__PRINTF_POS();
			osMsgPost(SP1K_MSG_MEDIA_REC_FILE_MAX);
		}
		//printf("File Max!!\n");
		//__PRINTF(__FILE,  __LINE__);
		return FALSE;
	}
	return TRUE;
}
UINT8 videoRecCheckSpaceBg(UINT32 vlcSize)
{
	UINT32 cnt=0;
	//_VIDEO_INTR_DISABLE();
	//cnt =  vlcBufFrameCnt;
	//_VIDEO_INTR_ENABLE();
	//cnt =cnt*g_brcTargetSize/videoClusterSize;
	cnt+=(vid_TmpNum + vlcSize) /videoClusterSize;
	cnt+=5;//64;

	if(	(bgGetFree(&bgFileIdx)<=2)||
		(bgGetFree(&bgFileHdr)<=2)||
		(bgGetFree(&bgFileMov)<=cnt)||
		(bgGetFree(NULL)<=cnt+4)){

		/*printf("(%u*%lu+%lu)/%lu=%lu+%lu>%lu\n"
			,vlcBufFrameCnt,g_brcTargetSize,vid_TmpNum,videoClusterSize
			,cnt,bgFileMov.fsize,bgFileMov.fsizeMax);*/
		if(DOS_FreeDiskSpaceByCluster()<20){
			__PRINTF_POS();
			osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		}else{
			__PRINTF_POS();
			osMsgPost(SP1K_MSG_MEDIA_REC_FILE_MAX);
		}
		//printf("File Max(%lx,%lx)!!\n",bgFileMov.fsize,bgFileMov.fsizeMax);
		//__PRINTF(__FILE,  __LINE__);
		//__PRINTF_POS();
		return FALSE;
	}
	return TRUE;
	vlcSize = vlcSize;
}
UINT8 videoRecCheckSpaceSeamless(UINT32 vlcSize)
{
	UINT32 aviSize = vidClipCtrl.SecAddr
		+(vidClipCtrl.hdrSize >>SECPERCHK)
		+(vidClipCtrl.idxSize >>SECPERCHK)
		+(vidFragBufSize>>SECPERCHK)
		+((videoClusterSize<<2)>>SECPERCHK)
		;
	UINT32 freeSize = DOS_FreeDiskSpaceByCluster() + bgGetFree(NULL);
	UINT32 reqSize = videoSeamlessFilesize ;
	if(videoSeamlessFilesize==0xffffffff){
		reqSize= videoSeamlessRecTime*vidClipCtrl.frameRate*vidBrcTagSize;
	}
	reqSize/= videoClusterSize;
	reqSize+=reqSize>>1;
	if(reqSize>freeSize){
		UINT16 order = 1;
		printf("%lx,%lx\n",reqSize,freeSize);
		do{
			UINT8 rst = dcfDeleteFile(order++,1,K_DCF_OUR_TYPE);
			if(rst==0){
				break;
			}else if(rst==2){
				//printf("del err!\n");
				osMsgPost(sp1k_MSG_REC_SEAMLESS_DISK_FULL);
				return FALSE;
			}
		}while(1);
	}
	if((G_USR_FileIndex>=9999)//g_vidParam[VD_SLREC_RenameMode])
		&& (vlcBufFrameCnt<(G_VideoVlcBufNum>>1))
		&& (g_vidParam[VD_SLREC_RenameMode])
		){
		__PRINTF_POS();
		osMsgPost(sp1k_MSG_REC_SEAMLESS_NOT_CHANGENAME);
		videoRecRenameSeamless((g_vidParam[VD_SLREC_RenameMode]==2)?1:0,K_DCF_OUR_TYPE);
		return TRUE;
	}
	//if(	((g_vidParam[VD_SLREC_MODE]==0)&&(aviSize>=(videoSeamlessFilesize>>SECPERCHK)))
	//	||((g_vidParam[VD_SLREC_MODE]==1)&&(slRecTime>=(videoSeamlessRecTime*1000))))
	if((aviSize>=(videoSeamlessFilesize>>SECPERCHK)) ||(slRecTime>=((videoSeamlessRecTime)*1000)))
	{
		UINT32 t0,t1;
		__PRINTF_POS();
		/*printf("%bx(%lx,%lx)(%lx,%lx)\n",g_vidParam[VD_SLREC_MODE],aviSize,videoSeamlessFilesize>>SECPERCHK,slRecTime,videoSeamlessRecTime*1000);
		if(G_USR_FileIndex>=9500){
			osMsgPost(sp1k_MSG_REC_SEAMLESS_NOT_CHANGENAME);
		}else{
			osMsgPost(sp1k_MSG_REC_SEAMLESS_CLOSEFILE);
		}*/
		osMsgPost(sp1k_MSG_REC_SEAMLESS_CLOSEFILE);
		vidSLDbgVlcCnt0 = vlcBufFrameCnt;
		HAL_GlobalReadGTimer(&t0);
		videoRecAbortSeamless();
		videoRecInitSeamless();
		HAL_GlobalReadGTimer(&t1);
		vidSLDbgTime = t1-t0;
		vidSLDbgVlcCnt1 = vlcBufFrameCnt;
		printf("sl:%lu(%lu/%lu)\n",vidSLDbgTime,vidSLDbgVlcCnt0,vidSLDbgVlcCnt1);
		return FALSE;
	}
	return TRUE;
	vlcSize = vlcSize;
}
UINT8  videoRecVlcDataProcess(UINT8 Only_WCard)
{
	UINT8 vlcIdx = idxVlcHead;
	UINT32 vlcSize = (vidVlcSize[vlcIdx] + 3UL)&0xfffffffc;
	UINT8 ret=SUCCESS;
	UINT32 brcAvgSize;

	if(!vidTestCnt)	{printf("%u-%bu(%ld)=",vlcBufFrameCnt,g_jpgQValue,g_brcTargetSize>>10);}
	if(vidBgEn){
		if(sD_CdGet()){
			__PRINTF_POS();
			return FAIL;
		}
	}
	if(vlcIdx > G_VideoVlcBufNum){
		UINT8 status;
		UINT8 i;
		__PRINTF_POS();
		osMsgPost(SP1K_MSG_MEDIA_REC_FAIL);

		ENTER_CRITICAL( status );
		for(i = 0; i < G_VideoVlcBufNum; i ++){
			printf("[%bu]=%bx, %bx\n", i, g_vlcIdxTab[i].next, g_vlcIdxTab[i].dumyCnt);
		}
		//printf("%bx, %bx\n", idxA, idxB);
		//printf("%bx, %bx\n", idxVlcHead, idxVlcTail);
		//printf("%bx, %bx\n", idxVlcDumy, idxFree);
		EXIT_CRITICAL( status );
		//while(1);
		return FAIL;
	}
	if(!vlcSize){
		//printf("vlcSize[%bu]==0\n",vlcIdx);
		videoRecVlcFreeBufAdd();
		return SUCCESS;
	}
	//videoRecBrcAdjust();
	vidClipCtrl.Compsize = vlcSize;
	vlcSize += (252+HUFFMANSIZE); //420 huffman len

	if(seamLessLpRecEn)	{
		if(!videoRecCheckSpaceSeamless(vlcSize)){
			//__PRINTF_POS();
			return FAIL;
		}
	}
	if(vidBgEn) {
		if(!videoRecCheckSpaceBg(vlcSize)){
			//__PRINTF_POS();
			return FAIL;
		}
	}else					{
		if(!videoRecCheckSpace(vlcSize)){
			//printf("*****%u\n",__LINE__);
			//__PRINTF_POS();
			return FAIL;
		}
	}

	//write frame size to jpeg header start
	vidClipCtrl.DramDMAaddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A + vlcIdx);
	HAL_DramStartAddrSet(vidClipCtrl.DramDMAaddr + 2, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(READ8(vlcSize, 3),READ8(vlcSize, 2));
	HAL_DramWordDataWrite(READ8(vlcSize, 1),READ8(vlcSize, 0));
	videoDebugFrameInfo(vidClipCtrl.DramDMAaddr + 7);

	//if(vidClipCtrl.recIdle != VID_REC_PAUSE) {//{return SUCCESS;}
		_VIDEO_INTR_DISABLE();
		brcAvgSize = videoBrcSizeAvg + 252+HUFFMANSIZE;//420 huffman len
		videoBrcQTabUpdate(vlcIdx,  brcAvgSize);
		_VIDEO_INTR_ENABLE();
	//}
	//DMA VLC buffer data to temp buffer
	vidTimeCheckDbgStart();
	videoRecSaveVlc();
	vidTimeCheckDbgEnd((UINT32)66,"Card",&vidRecDbgTimeCtrl.vidTimeCheckCard);
	if(!vidTestCnt)			{vidTestCnt=50;printf("\n");}
	else if(vidTestCnt!=0xff)	{vidTestCnt--;}
	return ret;
	Only_WCard = Only_WCard;
}




void videoRecTimeUpdate(void)
{
	UINT32 tmpTime,audSize;
    	UINT8 i,idx;
	*((UINT8*)&audSize ) = 0;
	*((UINT8*)&audSize+1) = XBYTE[REG_Audio_DataCnt2];
	*((UINT8*)&audSize+2) = XBYTE[REG_Audio_DataCnt1];
	*((UINT8*)&audSize+3) = XBYTE[REG_Audio_DataCnt0];

	audSize = (audSize<< 1) % ringBufSize;
	audSize += vidClipCtrl.AudChunkCnt<<SECPERCHK;
	tmpTime = audSize/audBytesPerSec;//s

	if(vidDateStampSts) {
		for(i=0,idx=g_vidDateStamp.disBufId+1;i<3;i++,idx++){
			if(idx>=4){idx=0;}
			if(videoDtFlag[idx]==0){
				videoRecStampLoad(idx);
				break;
			}
		}
	}

	if(vidUiTimeLast +vidClipCtrl.slowRecFps < tmpTime){
		vidUiTimeLast = tmpTime;
		vidTimeCheckDbgStart();
		uiUpdateVideoRecTime(tmpTime);
		vidTimeCheckDbgEnd((UINT32)33,"osd ",&vidRecDbgTimeCtrl.vidTimeCheckOsd);
	}

	//msvideo
	tmpTime *= 1000;
	if(tmpTime>(UINT32)4*60*60*1000){
		__PRINTF_POS();
		osMsgPost(SP1K_MSG_MEDIA_REC_FILE_MAX);
		return;
	}
	tmpTime += (audSize%audBytesPerSec)*1000/audBytesPerSec;
	vidClipCtrl.time = tmpTime;//videoRecTimeGet();
	if(seamLessLpRecEn ==1){
		audSize = vidClipCtrl.AudChunkCnt<<SECPERCHK;
		//slRecTime = audSize*1000/audBytesPerSec;
		slRecTime = audSize/audBytesPerSec*1000;
		slRecTime+=(audSize%audBytesPerSec)*1000/audBytesPerSec;

		//if(slopenfiletimeflag==0){
		//	slopenfiletime=0;
		//	slopenfiletimeflag=1;
		//}else{
			//UINT32 saveTime =(vidClipCtrl.AudChunkCnt<<SECPERCHK)*1000/audBytesPerSec;
			//_VIDEO_INTR_DISABLE();
			//slRecTime = vlcBufFrameCnt*1000/vidClipCtrl.frameRate;
			//_VIDEO_INTR_ENABLE();
			//slRecTime=saveTime;// -slopenfiletime;//vlcBufFrameCnt*1000/vidClipCtrl.frameRate;
		//}
	}
}

UINT32 VidIdxCount=0;
void videoRecWriteIndex()
{
	UINT32 size = videoClusterSize;
	UINT32 addr0= vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)<<1;
	UINT32 addr1=addr0+videoClusterSize;
	UINT32 addr = (vidClipCtrl.AVIIdxAddr<<1)-vidIndexNum;
	if(vidBgEn){
		UINT32 rst0 ,rst1;
		rst0 = bgFileWrite(&bgFileHdr, addr, size, 2);
		rst1 = bgFileWrite(&bgFileIdx, addr, size, 2);
		if((!rst0)||(!rst1)){
			__PRINTF(__FILE,  __LINE__);
			osMsgPost(SP1K_MSG_MEDIA_REC_FAIL);
			return;
		}
		/*UINT32 rst0 ,rst1;
		rst0 = bgFileWrite(&bgFileHdr, addr, size);
		//bgFileWrite(&bgFileIdx, addr+512, size);
		rst1 = bgFileWrite(&bgFileIdx, addr, size);
		if((rst0<size)||(rst1<size))
		{	__PRINTF(__FILE,  __LINE__);
			osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
			return;
		}*/
	}else{
		DOS_FileWrite(vidClipCtrl.fHandle_hdr, addr, size);
		DOS_FileWrite(vidClipCtrl.fHandle_idx, addr, size);
	}
	vidDbgDataFlow(5,4,size);
	vidIndexNum -= size;
	vidClipCtrl.hdrSize+=size;
	vidClipCtrl.idxSize+=size;
	if(addr==addr1){
		vidClipCtrl.AVIIdxAddr = (addr0+vidIndexNum)>>1;
		if(vidIndexNum){HAL_GprmDramDmaAdv(addr +size , addr0, vidIndexNum, 1);}
		//printf("BBB\n");
	}
	printf("<%lu,%lu>%lx,%lx\n",size,vidIndexNum,VidIdxCount++,vidClipCtrl.SecAddr);
}

void videoRecBrcAdjust()
{
#if 0
	UINT16 skip = G_VideoVlcBufNum>>3;
	UINT32 val = vidBrcTagSize>>5;//4k
	UINT16 tmp = skip<<1;
	UINT32 size =0;
	UINT16 vlcCnt;//= vlcBufFrameCnt;
	if(!skip)return;
	_VIDEO_INTR_DISABLE();
	vlcCnt = vlcBufFrameCnt;
	_VIDEO_INTR_ENABLE();
	if(vidBrcCnt++<4)return ;
	vidBrcCnt = 0;
	if(vlcCnt>tmp){
		tmp+=skip;
		while(vlcCnt>tmp){
			tmp+=skip;
			size+=val;
		};
		/*
		if(vlcBufFrameCnt>skip*7)size=val*5;//1024*20;
		else if(vlcBufFrameCnt>skip*6)size=val*4;//1024*16;
		else if(vlcBufFrameCnt>skip*5)size=val*3;//1024*12;
		else if(vlcBufFrameCnt>skip*4)size=val*2;//1024*8;
		else if(vlcBufFrameCnt>skip*3)size=val;//1024*4;
		*/
	}else if(vlcCnt>skip){
		size=-val;//(1024*4);
	}else{
		size=-(val<<1);//(1024*8);
	}
	g_brcTargetSize = vidBrcTagSize - size;
#endif
}
void videoRecIntr()
{
	UINT16 ysize;
	 UINT16 disHOff,disVOff;
	extern UINT8 frameNum;
	extern  UINT8 brcQValueIdx;
	//INTR_putchar('c');
	if (VideoRecBegin){
#if VIDREC_JPEG_EOF_EN
		if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){
			INTR_videoRecVlcBufSet();
		}
#else
		INTR_videoRecVlcBufSet();
#endif
		//INTR_putchar('0');
	}else{
		#if VIDREC_JPEG_EOF_EN
		if(((XBYTE[REG_Dram_VlcRdy]&0x03)==0x01)&&(frameNum)){
			brcQValueIdx=idxB;
			frameNum=0;
		}
		#endif
		if ((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){
			 //wangrf++
			ysize=(g_vidDateStamp.stampH>>4)& 0x01FF;
			disHOff=g_vidDateStamp.disHOff>>4;
			disVOff=g_vidDateStamp.disVOff>>4;

			XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&disHOff+1); //stamp h offset
			XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&disHOff+0);
			XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&disVOff+1);//stamp v offset
			XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&disVOff+0);

			XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1);//stamp v size
			XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);
			//wangrf++ end
			XBYTE[REG_Dram_PauseReq_Low] |= 0x20;
			XBYTE[REG_Jpeg_YUV_Type] &= 0xFC;
			XBYTE[REG_Jpeg_YUV_Type] |= 0x01;
			XBYTE[REG_Dram_PauseReq_Low] &= ~0x20;
#if !VIDREC_JPEG_EOF_EN
			XBYTE[REG_Dram_VlcIntEn]|=0x03;
#endif
			INTR_videoRecVlcBufSet();
			VideoRecBegin = 1;
			//INTR_putchar('2');
		}
		else
		{
#if !VIDREC_JPEG_EOF_EN
			XBYTE[REG_Dram_VlcIntEn]&=~(XBYTE[REG_Dram_VlcRdy]&0x03);
#endif
			//INTR_putchar('1');
		}

	}
}

UINT8 videoRecDma0Wait(UINT8 mask)
{
	//return;	LastWait=LastWait;
	//if (LastWait==0)
	UINT32 cnt = 100000;
	while (cnt-- && (XBYTE[REG_Gprm_DmaBusy] & mask));// wait DRAM DMA channel 0 free in HAL_GprmDramDmaAdv function
	return  cnt?1:0;
}/*
 void
videoRecDma0Wait(UINT8 mask)
{
	while (XBYTE[REG_Gprm_DmaBusy] & mask);// wait DRAM DMA channel 0 free in HAL_GprmDramDmaAdv function
}*/
