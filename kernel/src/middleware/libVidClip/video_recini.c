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
 * @file		video_recini.c
 * @brief		middle ware  API for video initial module
 * @author		tq.zhang
 * @since		2010-05-28
 * @date		2010-05-28
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
#define __FILE	  __FILE_ID_VIDEO_RECINIT__

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//UINT8 QTblVlcOv[130];
//UINT32 G_Video_Bound_Size;
//UINT32 StorageSpeed = 2500; // KB/sec
//UINT8 audVidCnt=3;

//wangjg++ for background
UINT32 videoClusterSize;
UINT8 *vidBgBuf;//[1024*4];
BgFileObj_t bgFileMov,bgFileHdr,bgFileIdx;

UINT8 vidBgEnDef = 6;
UINT8 vidBgEn ;//0:disable, 1:All List, 2:all fat, 3:movFat,otherList, 4, 5, 6(def,best)
UINT32 vidStorageSpeed;
UINT32 vidFileMaxSize,vidFileMaxSizeDef = 0x400000;//KB
UINT8 g_SnapInterTime;
extern void vidTimePosInit();
extern videoMjpgBitRateControl(UINT32 frameSize);
UINT8 videoHeaderQValueSet(UINT32 tmpAddr);
void videoQvalueSet();
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 vidUiTimeLast;
extern UINT8 g_SlowSnapOpen;
extern UINT8 vidTimeCheckEn;
extern vidRecDbgTimeCheck_t vidRecDbgTimeCtrl;
extern UINT32 vlcSizeTmp;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn		  UINT8 videoRecHeaderResLoad(UINT8 idx, UINT32 addr)
 * @brief	  load AVI resource file
 * @param	  [in] idx : file index
 * @param	  [in] addrWord : destination word address
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecHeaderResLoad(UINT8 idx, UINT32 addrWord)
{
	UINT8 ResId;
	switch (idx) {
		case RES_AVIHeader:
			ResId = RES_AVIHeader;
			break;
		case RES_AVIHeader_PCM:
			ResId = RES_AVIHeader_PCM;
			break;
		case RES_VideoHeader:
			ResId = RES_VideoHeader;
			break;
		default:
			ResId = RES_AVIHeader;
			return FAIL;
	}
	sp1kNandRsvRead(ResId, addrWord<<1);
	return SUCCESS;
}

UINT8 videoRecInitFile()
{
	UINT8 rwMode = K_DOS_NewFile;
	vfsFileDelete(vidClipCtrl.name_hdr);
	vfsFileDelete(vidClipCtrl.name);
	vfsFileDelete(vidClipCtrl.name_idx);
	vidClipCtrl.fHandle_hdr= vfsFileOpen(vidClipCtrl.name_hdr, rwMode, 100);
	vidClipCtrl.fHandle = vfsFileOpen(vidClipCtrl.name, rwMode, 100);
	vidClipCtrl.fHandle_idx= vfsFileOpen(vidClipCtrl.name_idx, rwMode, 100);
	if ((vidClipCtrl.fHandle_idx== 0)||(vidClipCtrl.fHandle_hdr== 0)||(vidClipCtrl.fHandle == 0)) {
		ASSERT(0, 1);
		return FALSE;
	}
	return TRUE;
}
UINT8 videoRecInitFileBg()//kb
{
	UINT32 size =  vidFileMaxSize;//0x400000;//4*1024ul*1024;//kb // 4Gb
	UINT32 freeSize= DOS_GetDiskSpace();//kb
	UINT32 useSize = (freeSize>1024)?(freeSize-1024):0;
	UINT32 fileMaxSize = ((size>freeSize)?useSize:size);//<<10;//0x3ff00000;//kb
	UINT32 width = vidClipCtrl.imgWidth;
	UINT32 frameSize =  ((width*9/16*width/32)*3+2048)/4/1024;//kb
	UINT32 idxMaxSize = ((fileMaxSize/frameSize)+1)*16/1024+1024;//128;//kb
	UINT8 xdata* pt = vidBgBuf;
	UINT8 ret = TRUE;
	//UINT8 i;
	printf("vidBgEn=%bu,%lx\n",vidBgEn,vidFileMaxSize);
	bgInit();
	if(useSize==0){osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);return FALSE;}
	memset(&bgFileMov,0,sizeof(bgFileMov));
	memset(&bgFileHdr,0,sizeof(bgFileHdr));
	memset(&bgFileIdx,0,sizeof(bgFileIdx));
	bgFileMov.listMax = 1024*6/8;
	bgFileHdr.listMax = 1024/8;
	bgFileIdx.listMax = 1024/8;
	bgFileMov.list = (BgClusters_t*)pt;	
	bgFileHdr.list = (BgClusters_t*)(pt+1024*6);
	bgFileIdx.list = (BgClusters_t*)(pt+1024*7);
	if(vidBgEn==1){
		bgFileMov.mode = 1;
		bgFileHdr.mode = 1;
		bgFileIdx.mode = 1;
	}else if(vidBgEn==2){
		bgFileMov.mode = 0;
		bgFileHdr.mode = 0;
		bgFileIdx.mode = 0;
	}else if(vidBgEn==3){
		bgFileMov.mode = 0;
		bgFileHdr.mode = 1;
		bgFileIdx.mode = 1;
	}else if(vidBgEn==4){
		bgFileMov.mode = 2;
		bgFileHdr.mode = 1;
		bgFileIdx.mode = 1;
	}else if(vidBgEn==5){
		ret = bgSetSpace(pt, 1024*8, size);
		bgFileMov.mode = 3;
		bgFileHdr.mode = 3;
		bgFileIdx.mode = 3;
		idxMaxSize = size;
		fileMaxSize= size;
	}else if(vidBgEn==6){
		ret = bgSetSpace(pt, 1024*6, size);
		bgFileMov.mode = 3;
		bgFileHdr.mode = 4;
		bgFileIdx.mode = 4;
		idxMaxSize = size;
		fileMaxSize= size;
	}
	if(!ret){__PRINTF_POS();return FALSE;}
	/*memcpy(vidClipCtrl.name,vidClipCtrl.name_hdr,8);
	memcpy(vidClipCtrl.name_idx,vidClipCtrl.name_hdr,8);
	for(i=8;i<11;i++) {
		vidClipCtrl.name[i] = 'A';
		vidClipCtrl.name_idx[i] = 'B';
	}
	vidClipCtrl.name[11] =  0;
	vidClipCtrl.name_idx[11] = 0;*/
	if(fileMaxSize>idxMaxSize+idxMaxSize){fileMaxSize -= idxMaxSize+idxMaxSize;}
	bgFileOpen(&bgFileHdr, idxMaxSize, vidClipCtrl.name_hdr);
	bgFileOpen(&bgFileIdx, idxMaxSize, vidClipCtrl.name_idx);
	bgFileOpen(&bgFileMov, fileMaxSize, vidClipCtrl.name);

	bgFileHdr.nextObj = &bgFileMov;
	bgFileMov.nextObj = &bgFileIdx;
	return TRUE;
}
UINT8 videoRecInitFileName()
{
	UINT8 i;
	if(!dcfFileIndexUpdate()){	//xian ++ 20081203
 		ASSERT(0, 1);
		return FALSE;
	}
	//dcfAviNameGet(vidClipCtrl.name_hdr, dcfFileIndexGet());
	dcfMediaNameGet(vidClipCtrl.name_hdr,dcfFileIndexGet(),MEDIA_AVI);
	for(i=0;i<12;i++) {
		vidClipCtrl.name[i] = 'A';
	}
	vidClipCtrl.name[11] = '\0';
	for(i=0;i<12;i++) {
		vidClipCtrl.name_idx[i] = 'B';
	}
	vidClipCtrl.name_idx[11] = '\0';
	return TRUE;
}
  UINT8 videoRecInitStorage()
 {
 	
	devAttr_t* pDev;//add by phil for disk driver lib release
	

	/*++++++ storage speed detect +++++++*/
	#if 0 // xian -- move to 	videoStorageWriteSpeedTest()		
		//UINT32 TimeT0,TimeT1,TimeT2,TimeT3,TimeT4;
		UINT32 TimeT1,TimeT4;
		UINT16 fHandle_test;
		UINT32 StorageSpeed;
		//HAL_GlobalTimerRead10us(&TimeT2);
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
	#endif			

	if (!sD_CdGet()){
		if (sp1kSDSpeedClassGet()>=SD_SPEED_CLASS4){//'0':class 0, '1': class 2, '2':class 4, '3':class 6
			vidStorageSpeed = 0x1888;
		}else if (sp1kSDSpeedClassGet()==SD_SPEED_CLASS2){
			vidStorageSpeed = 0x0BBB;
		}else{
			vidStorageSpeed = 0x0888;
		}
	}else{
		vidStorageSpeed = 0x0888;
	}

	//printf("T total: %ld, write: %ld\n",TimeT3-TimeT2,TimeT1-TimeT0);
	//printf("1st: %ld, 2nd: %ld\n",TimeT4-TimeT0, TimeT1-TimeT4);
	//printf("2nd: %ld\n",TimeT1-TimeT4);
	//printf("Test Card Speed: %ldKB\n",StorageSpeed);
	/*if (StorageSpeed<2500){
		G_FragBufSize >>= 2;
		G_FragBufNum <<= 2;
		G_SecPerFrag >>= 2;
	}
	else if(StorageSpeed<4000){
		G_FragBufSize >>= 1;
		G_FragBufNum <<= 1;
		G_SecPerFrag >>= 1;
	}*/
	//printf("G_FragBufSize: %ld\n",G_FragBufSize);
	//printf("G_FragBufNum: %bu\n",G_FragBufNum);
	//sprintf("G_SecPerFrag: %d\n",G_SecPerFrag);
		
	
	/* add by phil for disk driver lib release s*/
	if(G_ucStorageType != K_MEDIA_SDF){//fix 41831
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			sp1kFlashCacheCtrl(1);
		}
	}
	return TRUE;
 }
  UINT8 videoRecInitCheckSpace()
  {
  	if(seamLessLpRecEn==0){
		vidClipCtrl.freeCluster = DOS_FreeDiskSpaceByCluster();//eric@20080502
		vidClipCtrl.usedCluster = 1 + ((UINT32) 500L * 1024L) / videoClusterSize;
		printf("%lu<%lu,%lu>%lu\n",vidClipCtrl.usedSize,vidClipCtrl.usedCluster, vidClipCtrl.freeCluster,videoClusterSize);
		if (vidClipCtrl.freeCluster > vidClipCtrl.usedCluster) {
			vidClipCtrl.freeCluster -= vidClipCtrl.usedCluster;
			vidClipCtrl.usedCluster = (500L * 1024L) / videoClusterSize;
			if (vidClipCtrl.usedCluster > vidClipCtrl.freeCluster) {
				 osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
				 vidPrintfAbort("videoRecInitCheckSpace\n");
				 return FALSE;    	
			}
		}
	}
	else{
		if(FAIL == videoSLRecMemCheck()){
			osMsgPost(sp1k_MSG_REC_SEAMLESS_DISK_FULL);
			return FALSE;
		}

	}
	/*if((g_vidParam[VD_SLREC_MODE]==1)&&(seamLessLpRecEn==1))
	{
		videoSeamlessFilesize=(UINT32)videoSeamlessRecTime*g_brcTargetSize*vidClipCtrl.frameRate;
		//printf("videoSeamlessFilesize=%ld\n",videoSeamlessFilesize);
	}*/
	return TRUE;
  }
  void videoQvalueSet(){
	UINT32  srcAddr,vlcAddr;
	UINT16 srcHsize,srcVSize,dstHsize,dstVsize ;
	UINT32 jpgSize;
	extern UINT8 g_brcQValue;
	UINT32 dis ;
	UINT16 k ;
	UINT8 s;
	srcAddr=K_SDRAM_AviPvwBufAddrA;
	vlcAddr=vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A + 3);	
	pvSizeGet(&srcHsize,&srcVSize);
	videoRecSizeGet(&dstHsize,&dstVsize);
	jpegQTableSet(K_Qxx, NULL, NULL, 1); 
	HAL_JpgZFactorSet( 100, 100, srcHsize, srcVSize, dstHsize, dstVsize, 0);
	HAL_JpegEncode( srcAddr, vlcAddr, 0,0x20, 1 );
	HAL_JpegReadEncodeSize( &jpgSize); 
	dis = _ABS(jpgSize,g_brcTargetSize);
	k=(dis<<8) /g_brcTargetSize;
	if(k>=192){s=20;}
	else if(k<=32){s=0;}//k/256*120
	else {s=(k-32)>>3;}
	G_Image_QTableIndex=K_Qxx;
	if(jpgSize>g_brcTargetSize){
		if(K_Qxx>s )G_Image_QTableIndex=K_Qxx-s;
	}else{
	 	if(K_Qxx>s )G_Image_QTableIndex=K_Qxx+s;
	}
}
 UINT8 videoRecInitBrc()
 {
 	//videoBrcUpdateQValBuf();
 	videoQvalueSet();
 	videoBrcUpdateQValBuf();	
	//msPerFrm =(float) 1* 1000/(float)g_vidAviFrameRate;		// g_vidAviFrameRate is avi vd rate
	printf("@g_jpgQValue=%bu\n",g_jpgQValue);
	//videoBrcTgSizeSet(audBytesPerSec, vidClipCtrl.frameRate);
	return TRUE;
 }

 /**
 * @fn		UINT8 videoRecInitCdsp(void)
 * @brief		  video Rec Init Cdsp
 * @param	  NONE
 * @retval	   NONE
 * @todo
 * @author	SunYong
 * @since	  2007-12-18
 */
#include "hal_jpeg.h"
UINT8 videoRecInitCdsp(UINT16 width,UINT16 height)
{
	UINT8 option;
	UINT16 thumbWSize;
	UINT16 fHSize,fVSize;
	UINT8 bypass, rot, mode,disten;
	UINT8 sizeRoiEn; // '0' scale factor ROI, '1' set x/y size, x/y offset ROI.
	UINT8 vdZoom;
	UINT16 hfacT=100,vfacT=100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points 
	UINT16 SrcWidth,SrcHeight,DstWidth,DstHeight;
	UINT8 DistEn,type;
	#ifdef DIST_FUNC_EN	
	UINT8 DistClmpX, DistClmpY;
	#endif
	
	#ifdef DIST_FUNC_EN	
	DistEn = sp1kDistStatusGet();
	distClampSizeGet(&DistClmpX,&DistClmpY);
	//printf("Dist En=%bd\n",DistEn);
	//DistSwitchSize(vidClipCtrl.imgWidth,vidClipCtrl.imgHight);		
	#endif
	
	option = 0x80;
	option |= (DistEn << 5);	
	rot    = (option   )&0x07;
	mode   = (option>>3)&0x01;
	bypass = (option>>4)&0x01;
	disten = (option>>5)&0x01;
	sizeRoiEn = (option>>6)&0x01; // when it is on, DIST should be close
	vdZoom = (option>>7)&0x01; //when record video, image size should be the same with MPEG buffer size

	zoomDevActGet(ZOOM_CTRL_TYPE_VIDEO,&type);
	
	if ( type == ZOOM_LOGI_NONBLOCKING_TYPE ) {
		VideoZoomNBCtrl.zoomAction = 0;
		//videoRecZoomFacSet_NBlocking(0,videoZoomFctGet_NBlocking(),0);
	}else{
		printf("@@@blocking\n");
		//videoRecZoomFacSet_Blocking(0,videoZoomFctGet_Blocking(),0);
	}
		
	zoomDevZoomSet(ZOOM_CTRL_TYPE_VIDEO,0,com_zoomDevZoomFactorGet(ZOOM_CTRL_TYPE_VIDEO),0);
	
	SrcWidth = (UINT16)(XBYTE[REG_Cdsp_Clamphsize_high]<<8) | XBYTE[REG_Cdsp_Clamphsize_low];
	SrcHeight = (UINT16)(XBYTE[REG_Cdsp_Clampvsize_high]<<8) | XBYTE[REG_Cdsp_Clampvsize_low];
	
	DstWidth = width;//vidClipCtrl.imgWidth;
	DstHeight = height;//vidClipCtrl.imgHight;

	if ( mode == 0x00 ) {
		/* set Stillsb_hsize. */
		HAL_JpegSetDramHSize(SrcWidth);

		if (vdZoom){
			HAL_Dram1stPathFrmBufSizeGet(&fHSize,&fVSize);	
			//fHSize = (roi_hsize>>16) + (hsfini>>16);
			//fVSize = (roi_vsize>>16) + (vsfini>>16);
			HAL_JpegSetSourceSize(fHSize, fVSize);
		}else{
			HAL_JpegSetSourceSize(SrcWidth, SrcHeight);
		}

		if ( rot & 0x01 ) {  /* rotate. */
			HAL_JpegSetVlcSize(DstHeight, DstWidth);

			if( bypass == 1 ) {
				thumbWSize = DstHeight;
			}
			else {
				thumbWSize = DstHeight>>3;
			}
		}
		else {  /* no rotate. */
			HAL_JpegSetVlcSize(DstWidth, DstHeight);

		if( bypass == 1 ) {
			thumbWSize = DstWidth;
			}
			else {
				thumbWSize = DstWidth>>3;
			}
		}

		/* set TMBhsize. */
		thumbWSize = ( thumbWSize + 7 ) & ~7;
		XBYTE[REG_Dram_TMBHSize_Low]=*((UINT8*)&thumbWSize+1  );
		XBYTE[REG_Dram_TMBHSize_High]=*((UINT8*)&thumbWSize    );
	}

	if ( mode == 0x01 ) {
		XBYTE[REG_Jpeg_ZfPop] = 1; // PUSH
	}
	return TRUE;
}

 UINT8 videoRecInitAudio()
 {
 	//UINT32 k= 5000;	
	/*while((XBYTE[REG_Audio_AudEn]&0x02) ==0x02){
	if(!(k--)) {
		XBYTE[REG_Audio_DataThdIntSts] |= 0x02;
		XBYTE[REG_Audio_DataThdIntEn] &=  (~0x02);
		XBYTE[REG_Audio_AudEn] &=  (~0x02);
		break;
	}
	}*/
 	HAL_AudSwRst(0xff);
	if( vidClipCtrl.audFmt == VID_PCMFMT) {
		HAL_AudAdpcmSet( REC_MODE, 0, 0);
	}
	else {
		HAL_AudAdpcmSet( REC_MODE, 1, (UINT32)BLOCK_ALIGN);
	}
	HAL_AudPause();
	HAL_AudRingBufferSet( (vidClipCtrl.AudioAddr<<1), vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_AUDIO)*2);	
	HAL_AudStart( (UINT32)REC_MODE, (UINT32)vidClipCtrl.sampRate, (UINT32)SAMPLE_BIT, (UINT32)CHANNEL );
	return SUCCESS;
 }
 UINT8 videoRecInitMjpg()
 {
 	UINT32 tmpVLCAddrA = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A)+ 130+(HUFFMANSIZE>>1); //210 huffman len
	UINT32 tmpVLCAddrB = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_B)+ 130+(HUFFMANSIZE>>1);//210  huffman len
	
	
	HAL_DramAVLCAddrSet(tmpVLCAddrA );
	HAL_DramBVLCAddrSet(tmpVLCAddrB);		
	//G_Video_Bound_Size = (vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A)-136)<<1;
	HAL_JpegVlcBoundSet(1, 1, vidVlcBufSize-272);		
	videoRecEofIntrEnable();
	if (g_vidParam[VD_REC_YUV_FORMAT] != VD_REC_YUV_420){
		HAL_JpegMjpgTrig(tmpVLCAddrA, tmpVLCAddrB, 0);
	}
	else {
		HAL_JpegMjpgTrig(tmpVLCAddrA, tmpVLCAddrB, 1);	
	}
	
	HAL_AudResume();
	return TRUE;
 }
 

 /**
  * @fn 	   void videoRecInitHdr(void)
  * @brief	   video clip header initial
  * @param	   NONE
  * @retval    NONE
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-10
  * @todo	   N/A
  * @bug	   N/A
 */
 UINT8 videoHeaderQValueSet(UINT32 tmpAddr)
{
	UINT32 dramAddr;	
	extern UINT8 g_brcVlcQIndexBuf[];
	UINT8 *pidx = g_brcVlcQIndexBuf;
	dramAddr =tmpAddr;
	//dramAddr += vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A) * vlcBufIdx;
	dramAddr += 16;
	//HAL_GprmDramDmaAdv(qTabAddr<<1, dramAddr<<1, 130, 1);
	//HAL_GprmDramDmaExt(qTabAddr<<1, dramAddr<<1, 130, DmaWaitOpt,1);
	//HAL_DramDmaWrite(g_brcQTabBuf, dramAddr, 130, DmaWaitOpt);
	HAL_DramDmaWrite(brcQTbls+(UINT16)*pidx*130, dramAddr, 130, 1);
	return SUCCESS;
}

 void videoRecInitHdr(void)
 {
	UINT32 tmp0;
	UINT8 i;
	UINT8 *pt;
	UINT32 sizeA = vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	UINT32 addrA = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR);

	if(vidClipCtrl.imgWidth==1920){
		vidClipCtrl.imgHight=1080;
		}

	//prepare AVI header
	if(vidClipCtrl.audFmt == VID_ADPCMFMT) {
		videoRecHeaderResLoad(RES_AVIHeader,addr);
		if(g_bNoAudio){
			XBYTE[0x39A0] = 'x';	//[0x01A0] = "auds" - "xuds":no voice
		}
	}
	else {
		videoRecHeaderResLoad(RES_AVIHeader_PCM,addr);
		if(g_bNoAudio){
			XBYTE[0x38E8] = 'x';	//[0x00E8] = "auds" - "xuds":no voice
		}
	}
 	//tq 20090606    
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR));
	HAL_DmaDo(0,1,0x0200,0,0);

	XBYTE[0x3840] = (UINT8)(vidClipCtrl.imgWidth); XBYTE[0x3841] = (UINT8)(vidClipCtrl.imgWidth>>8);
	XBYTE[0x3844] = (UINT8)(vidClipCtrl.imgHight); XBYTE[0x3845] = (UINT8)(vidClipCtrl.imgHight>>8);
	XBYTE[0x38A0] = (UINT8)(vidClipCtrl.imgWidth); XBYTE[0x38A1] = (UINT8)(vidClipCtrl.imgWidth>>8);
	XBYTE[0x38A2] = (UINT8)(vidClipCtrl.imgHight); XBYTE[0x38A3] = (UINT8)(vidClipCtrl.imgHight>>8);
	XBYTE[0x38B0] = (UINT8)(vidClipCtrl.imgWidth); XBYTE[0x38B1] = (UINT8)(vidClipCtrl.imgWidth>>8);
	XBYTE[0x38B4] = (UINT8)(vidClipCtrl.imgHight); XBYTE[0x38B5] = (UINT8)(vidClipCtrl.imgHight>>8);
	tmp0 = (UINT32)vidClipCtrl.imgWidth*vidClipCtrl.imgHight*3;
	XBYTE[0x38C0] = (UINT8)(tmp0);		XBYTE[0x38C1] = (UINT8)(tmp0>>8);
	XBYTE[0x38C2] = (UINT8)(tmp0>>16);	XBYTE[0x38C3] = (UINT8)(tmp0>>24);
	
	if(G_FrameRate == 10 || G_FrameRate == 20) {
		XBYTE[0x3820] = 0xA0; XBYTE[0x3821] = 0x86;
		XBYTE[0x3822] = 0x01; XBYTE[0x3823] = 0x00;
		XBYTE[0x3880] = 0xA0; XBYTE[0x3881] = 0x86;
		XBYTE[0x3882] = 0x01; XBYTE[0x3883] = 0x00;
	}
	else if(G_FrameRate == 15 || G_FrameRate == 30) {
		XBYTE[0x3820] = 0x6B; XBYTE[0x3821] = 0x04;
		XBYTE[0x3822] = 0x01; XBYTE[0x3823] = 0x00;
		XBYTE[0x3880] = 0x6B; XBYTE[0x3881] = 0x04;
		XBYTE[0x3882] = 0x01; XBYTE[0x3883] = 0x00;
	}
	
	XBYTE[0x39f8] = 0x69;//idx flag
	XBYTE[0x39f9] = 0x64;
	XBYTE[0x39fa] = 0x78;
	XBYTE[0x39fb] = 0x31;

	//xian ++ 20110809 for viviLink
	pt = (UINT8*)&g_vidParam[VD_USER_COMMENT_CODE0];
	for(i = 0; i < 8; i ++){
	    XBYTE[0x39e0 + i] = *pt ++;
	}
	//xian --

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addr);
	HAL_DmaDo(1,0,0x0200,0,0);
	
	//prepare JPEG header
	videoRecHeaderResLoad(RES_VideoHeader,addrA);//vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A));
	videoHeaderQValueSet(addrA);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addrA);//vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A));
	HAL_DmaDo(0,1,0x0104+HUFFMANSIZE,0,0);
	XBYTE[0x38E8+HUFFMANSIZE] = (UINT8)(vidClipCtrl.imgHight>>8); XBYTE[0x38E9+HUFFMANSIZE] = (UINT8)(vidClipCtrl.imgHight);
	XBYTE[0x38EA+HUFFMANSIZE] = (UINT8)(vidClipCtrl.imgWidth>>8); XBYTE[0x38EB+HUFFMANSIZE] = (UINT8)(vidClipCtrl.imgWidth);
	if (g_vidParam[VD_REC_YUV_FORMAT]!=VD_REC_YUV_422){XBYTE[0x38EE+HUFFMANSIZE] = 0x22;}
	VideoRecBegin=0xff;
	tmp0 = addrA;
	for(i = 0; i < G_VideoVlcBufNum; i ++){
		HAL_DmaSetSramAddr(0x0000);          // fill video clip buffer
		HAL_DmaSetDramAddr(tmp0);
		HAL_DmaDo(1,0,0x0104+HUFFMANSIZE,0,0);
		tmp0 += sizeA;
	}

	if (g_vidParam[VD_REC_YUV_FORMAT]==VD_REC_YUV_422_420){
		VideoRecBegin=0;	
	}


	if(vidClipCtrl.imgWidth==1920){
		vidClipCtrl.imgHight=1088;
	}
}
void videoRecInitGlobalSeamless()
{
	//printf("62-1\n");
	audDataSize-=(SINT32)(vidClipCtrl.AudChunkCnt<<SECPERCHK);
	vidClipCtrl.AVIIdxAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)+256;//K_SDRAM_AviCaptureIdx1Addr;
	vidClipCtrl.totalChuck = 0;
	vidClipCtrl.MoviOfset = 4;
	vidClipCtrl.VidChunkCnt = 0; 
	vidClipCtrl.AudChunkCnt = 0;
	vidClipCtrl.Compsize = 0;
	vidClipCtrl.SecAddr = 0;
	vidClipCtrl.idxSize = 0;
	vidClipCtrl.hdrSize = 0;
	//vidClipCtrl.DRAMAudBufCnt = 0;  //fix #42292
	vidClipCtrl.freeCluster = 0; 
	vidClipCtrl.usedCluster = 0;  
	vidClipCtrl.fHandle = 0;
	vidClipCtrl.DramDMAaddr = 0;
	vidClipCtrl.usedSize = 0;  //3
	vidClipCtrl.time = 0;
   	vidClipCtrl.CurSrcImgAddr =vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG);// K_SDRAM_AviCapFragBufAddrA;
	vid_TmpNum =0;//12; //For cluster  alignment
	vid_ByteAddr =0;// 12;//For cluster	alignment
	vid_FragCnt = 0;


	vidVlcDMACnt=0;
	totalFrameCnt=0;
	totalDummyFrame = 0;

	

	vidIndexNum = 512;
	vidUiTimeLast = 0;
	//slopenfiletimeflag=0;
	//slopenfiletime=0;

	//if(g_vidParam[VD_SLREC_RenameMode]>9999){g_vidParam[VD_SLREC_RenameMode]=9999;}
}
 
 //-----------------------------------------------------------------------------
 //videoRecGlobalInit
 //-----------------------------------------------------------------------------
 /**
  * @fn 	   void videoRecInitGlobal(void)
  * @brief	   video clip global variable initial
  * @param	   NONE
  * @retval    NONE
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-10
  * @todo	   N/A
  * @bug	   N/A
 */
 void videoRecInitGlobal (void)
 {
	UINT8 i;
	vidClipCtrl.mode = VID_MODE;
	vidClipCtrl.recIdle = VID_REC_IDLE;
	vidClipCtrl.AudioAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_AUDIO);
	vidClipCtrl.CurDMATagAddr =vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG);
	vidClipCtrl.CurSrcImgAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG);
	vidClipCtrl.AVIHeadAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR);
	vidClipCtrl.AVIIdxAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)+256;	
//printf("CMD END: %lx\n", sysMemAddrGet(MEM_IDX_VIDEO_REC_START));
//printf("\n%lx, %lx\n", sysMemAddrGet(MEM_IDX_STILL_VIEW_END), sysMemAddrGet(MEM_IDX_STILL_VIEW_END));
//printf("%lx, %lx\n", sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_A), sysMemAddrGet(MEM_IDX_VIDEO_REC_AUDIO));
//printf("%lx, %lx\n", sysMemAddrGet(MEM_IDX_VIDEO_REC_FRAG), sysMemAddrGet(MEM_IDX_VIDEO_REC_HDR));
//printf("%lx, %lx\n", sysMemAddrGet(MEM_IDX_VIDEO_REC_IDX), sysMemAddrGet(MEM_IDX_VIDEO_REC_END));
//printf("%lx, %lx\n", sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV2_D), sysMemAddrGet(MEM_IDX_VIDEO_VIEW_END));

	vidClipCtrl.AudChunkCnt = 0;
	vidClipCtrl.totalChuck = 0;
	vidClipCtrl.MoviOfset = 4;
	vidClipCtrl.VidChunkCnt = 0;
	vidClipCtrl.AudChunkCnt = 0;
	vidClipCtrl.Compsize = 0;
	vidClipCtrl.SecAddr = 0;
	vidClipCtrl.idxSize = 0;
	vidClipCtrl.hdrSize = 0;
	vidClipCtrl.DRAMAudBufCnt = 0;
	vidClipCtrl.freeCluster = 0;
	vidClipCtrl.usedCluster = 0;
	vidClipCtrl.fHandle = 0;
	vidClipCtrl.DramDMAaddr = 0;
	vidClipCtrl.usedSize = 0;
	vidClipCtrl.time = 0;

	vid_ByteAddr = 0;
	vid_TmpNum = 0;

	vid_FragCnt = 0;
	vid_AudOutIdx = 0;	
	videoClusterSize =vfsClusterSizeGet();

	vidBgBuf = G_FPGAMemPool;//8k
	brcQTbls = G_FPGAMemPool+1024*8;//2600byte
	
	g_brcTargetSize = vidBrcTagSize ;
#if VLC_BUF_CHG_EN
	vlcBufNum =0;
	for(i=0;i<VID_VLCBUFNUM_TOTAL;i++) {
		vidVlcSize[i] = 0;
	}
	flip = 0;
	vidVlcBufChgCnt=0;
	//VID_VLC_BUF_STATUS = 0;
	//VID_VLC_BUF_STATUS_HIGH = 0;
	dropFrameFlagA=0;
	dropFrameFlagB=0;

	vidVlcDMACnt=0;
	totalFrameCnt=0;
	DummyFrameCnt = 0;
	totalDummyFrame = 0;
#endif
	if(vidClipCtrl.audFmt == VID_PCMFMT) {
		audBytesPerSec = (UINT32)vidClipCtrl.sampRate * (UINT32)SAMPLE_BIT / 8;
	}
	else {
		audBytesPerSec = (UINT32)vidClipCtrl.sampRate * BLOCK_ALIGN / SMPPERBLK;
	}
	vidBytesPerSec = g_bitRate>>3;

	videoBrcSizeAvg = vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A)<<1;//*8/10;
	vidVlcBufSize = vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A)<<1;
	vidFragBufSize=( ((vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_FRAG)<<1)-vidVlcBufSize)&0xffff8000);
	vidFileMaxSize = vidFileMaxSizeDef;
	if(	vidBgEnDef
		&& (DOS_CurrDriverGet()== DRIVE_SD )//drive is sd
		&& (videoClusterSize>=8*1024)		//cluster size >16K
		&& (dos.disk[dos.CurrDrv].FsType!=12) 	//fsType is not fat12
		//&& (dos.disk[dos.CurrDrv].FsType==32)//fsType is FAT32
		){
		vidBgEn = vidBgEnDef;	
		if((vidBrcTagSize*vidClipCtrl.frameRate<=55ul*1024*30)&&(vidClipCtrl.frameRate<60)){
			vidBgEn = 0;//5;
		}
 		if(seamLessLpRecEn){
			UINT32 fsize = (audBytesPerSec+vidBytesPerSec)*videoSeamlessRecTime;
			
			vidFileMaxSize =_MIN(fsize,videoSeamlessFilesize);
			if(vidFileMaxSize<0xffffffff -0x7ffff){vidFileMaxSize+=0x7ffff;}
			vidFileMaxSize/=1000;
 		}
	}else{
		printf("noBG:%bu,%bu\n",DOS_CurrDriverGet(),dos.disk[dos.CurrDrv].FsType);
		vidBgEn = 0;
		if(DOS_CurrDriverGet()!= DRIVE_SD){
			g_brcTargetSize=30ul*1024;
		}
		
	}

	vlcOverflowCnt=0;
	QIdxMax=0;
	QIdxMin=100;
	dumyIdxTotal = 0;
	vlcABSwitch = 0;
	vidIndexNum = 512;

	g_dumyCopy = 0;
	g_bReserveStampFrameSize = 0;
	if(vidDateStampSts){
		videoRecStampInit();
	}
	alc_gain_idx = alc_default_gain_index; //recover alc_gain_idx
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_GAIN_SET,alc_gain_idx);

#if VD_REC_DEBUG
	vdDebugInit();
#endif

#if VIDEO_REC_SMOOTH_DUMY
	for(i = 0; i < G_VideoVlcBufNum; i ++){
		g_vlcIdxTab[i].dumyCnt = 0;
		g_vlcIdxTab[i].next = i + 1;
	}
	g_vlcIdxTab[G_VideoVlcBufNum-1].next = VD_BUF_IDX_EOF;
	idxA = 0;
	idxB = 1;
	{
		extern UINT8 brcQValueIdx;
		extern UINT8 frameNum;
		brcQValueIdx=idxA;
		frameNum=1;
	}
	idxFree = 2;
	idxVlcHead = VD_BUF_IDX_EOF;
	idxVlcTail = VD_BUF_IDX_EOF;
	idxVlcDumy = VD_BUF_IDX_EOF;
	idxVlcPreTail = VD_BUF_IDX_EOF;
	vlcBufFrameCnt = 0;
#endif
 	//slflag=0;
	//slopenfiletimeflag=1;
	//slopenfiletime=0;
	
	g_vidCustomUpdateEntry = 0;
//#if AVSYNCTEST
// 	vidChunkCntTest=0;
//#endif
	//seamLessLpRecStop=0;

	#if (VIDEO_FRONT_CDSP_JPG_DBG == 1)
	videoJpgTriggerCnt = 0;
	videoFrontInputCnt = 0;
	videoStatisticsEn = 1;
	#endif
	vidClipCtrl.slowRecFps = 0;
	vidUiTimeLast = 0;	
	g_SlowSnapOpen = 0;
	if( g_vidParam[VD_SLOW_REC_MOD] ){
		vidClipCtrl.slowRecFps = g_vidParam[VD_SLOW_REC_FPS];
		if(!vidDateStampSts){
			HAL_GlobalRTCSecondIntrSet(1);
		}
	}
	vidRecDbgTimeCtrl.vidTimeCheckOsd=0;
	vidRecDbgTimeCtrl.vidTimeCheckHost=0;
	vidRecDbgTimeCtrl.vidTimeCheckCard=0;
	vidRecDbgTimeCtrl.vidTimeCheckSliceLast=0;
	vidTimeCheckEn=1;
	vlcSizeTmp = 0;
}

//-----------------------------------------------------------------------------
//videoRecKeyOnSnap
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecMemInit(void)
 * @brief	  video record dynamic mem initial
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-8-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecInitMem(void)
{
	UINT8 size;
	UINT8 pvFrameBufNum;
	UINT8 vidRecFrameBufNum;
	UINT16 pvHsize,pvVsize;
	//UINT8 curVidRecFrameBufNum;

	size = vidRecSizeGet();
	pvFrameBufNum=vidRecFramebufModeGet();
	vidRecFrameBufNum =vidRecFrameBufNumGet();
	
	/* check video rec buffer number mode switch */
	if ( vidRecFrameBufNum != pvFrameBufNum ) {
		/* set front2cdps gating enable */
		HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

		/* disable frame ctrl */
		HAL_CdspFrmCtrlEnSet(0);	

		/* video preview addr Update */
		pvSizeGet(&pvHsize,&pvVsize);
		sysMemVideoPvAddrUpdate(size,pvHsize,pvVsize);
		//sysMemVideoPvAddrUpdate(size);

		/* video record addr Update */
		vidRecMemFrameBufAddrUpdate();
		
		videoAbortPvRegAddrUpdate(vidRecFrameBufNum);

		/* set frame buffer number 2 (1st path) */
		if(vidRecFrameBufNum==2){
			HAL_DramFrmBufNumSet(0);
		}
		else if(vidRecFrameBufNum==3){
			HAL_DramFrmBufNumSet(1);
		}
		else{
			HAL_DramFrmBufNumSet(2);
		}
		/* enable frame ctrl */
		HAL_CdspFrmCtrlEnSet(1);

		/* set front2cdps gating disable */
		HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);
	}	
}

 //-----------------------------------------------------------------------------
 //videoRecInit
 //-----------------------------------------------------------------------------
 /**
  * @fn 	   UINT8 videoRecInit(void)
  * @brief	   video clip initialization
  * @param	   NONE
  * @retval    return = SUCCESS / FAIL
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-10
  * @todo	   N/A
  * @bug	   N/A
 */
UINT8 videoRecInit (void) USING_0
 {
 	UINT32 time0,time1;
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	printf("rate:%bu,%bu\n",g_vidAviFrameRate,vidClipCtrl.frameRate);
	printf("clust(%lx,%lx)\n",disk->FAT_FirstFreeCluster,disk->FreeCluster);
	HAL_GlobalReadGTimer(&time0);
	HAL_CpuImemSdSet(LOG_BANK_5, 0x8000);
	videoRecInitMem();
	videoRecInitGlobal();
	videoRecInitBrc();
	videoRecInitAudio();
	videoRecInitHdr();
	if(!videoRecInitCheckSpace()){__PRINTF_POS();return FAIL;}
	if(!videoRecInitFileName()){__PRINTF_POS();return FAIL;}
	if(!videoRecInitStorage()){__PRINTF_POS();return FAIL;}
	
	if(!videoRecInitCdsp(vidClipCtrl.imgWidth,vidClipCtrl.imgHight)){__PRINTF_POS();return FAIL;}
	
	if(vidBgEn){if(!videoRecInitFileBg()){__PRINTF_POS();return FAIL;}}
	else{if(!videoRecInitFile()){__PRINTF_POS();return FAIL;}}
	
	if(!videoRecInitMjpg()){__PRINTF_POS();return FAIL;}

	HAL_GlobalReadGTimer(&time1);
	printf("Init:%lu-%bu\n",time1-time0,G_VideoVlcBufNum);
	printf("File_idx:%d\n",G_USR_FileIndex);
	vidDbgDataFlowInit();
	vidTimeCheckDbgStart();
	return SUCCESS;
}
UINT8 videoRecInitSeamless()
{
	UINT32 time0,time1;
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	printf("clust(%lx,%lx)\n",disk->FAT_FirstFreeCluster,disk->FreeCluster);
	HAL_GlobalReadGTimer(&time0);
	if(!videoRecInitFileName()){return FAIL;}
	videoRecInitGlobalSeamless();	
	//videoRecWriteMovIDToDram(vidClipCtrl.CurDMATagAddr);
	
	if(vidBgEn){if(!videoRecInitFileBg()){return FAIL;}}
	else{if(!videoRecInitFile()){return FAIL;}}
	
	HAL_GlobalReadGTimer(&time1);
	//printf("slInit:%lu-%u/%bu\n",time1-time0,vlcBufFrameCnt,G_VideoVlcBufNum);

	return SUCCESS;
}
