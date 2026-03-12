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
 * @file	  video_slclip.c
 * @brief	  videoseamless loop  rec
 * @author	  rf.wang
 * @since	  2011-03-14
 * @date	  2011-03-14
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "video_def.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define Video_SL_Old 0 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
// #define __FILE	  __FILE_ID_VIDEO_SLCLIP__
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/


/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT8 seamLessLpRecEn=0,seamLessLpRecStop;  //0: disable , 1: enable
UINT32 videoSeamlessFilesize=0xffffffff;//(UINT32) 10L * 1024L * 1024L;
UINT32 videoSeamlessRecTime=(UINT32)10;
UINT8 slflag;
UINT32 slRecTime; //the time when to stop a video file for slrec
//UINT32 slopenfiletimeflag; //flag for sl open new video file 
//UINT32 slopenfiletime; 	// the time for sl open new video file

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 #if 0
extern  xdata   UINT8   G_ucStorageType;
extern UINT16 dzoomDispXZoomFactor;
extern xdata UINT8 G_Image_QTableIndex;
#if (VIDEO_NEW_ZOOM == 0)
extern UINT16 xdata video_zoomFactor ;
#else
extern appVideoZoom_t xdata VideoZoomCtrl;
#endif
//extern appVidClipCtrl_t xdata vidClipCtrl;
extern UINT32 vid_ByteAddr,vid_TmpNum;
extern UINT8 g_dumyCopy;
extern UINT8 g_bReserveStampFrameSize;
extern xdata UINT8 vidDateStampSts; 
#ifdef OpenTwoFile
extern UINT32 vidIndexNum;
extern UINT8 idx_f;
extern UINT8 wf;
extern UINT32 dropFrmCnt;

#endif

extern UINT32 vidStorageSpeed;
extern UINT32 vlcOverflowCnt;
#ifdef VIDEO_REC_BRC_EN
extern UINT8 QIdxMax,QIdxMin;
#endif
extern UINT32 dumyIdxTotal;

extern UINT8 G_VideoVlcBufNum;
extern UINT8 videoQValueTab[];
extern UINT8 vid_FragCnt;
extern UINT32 vidVlcBufChgCnt;
extern UINT8 flip;
extern UINT8 dropFrameFlagA;
extern UINT8 dropFrameFlagB;
extern UINT32 vidVlcDMACnt;
extern UINT32 DummyFrameCnt;
extern UINT32 totalFrameCnt;
extern UINT32 totalDummyFrame;
extern UINT32 bytesPerSec;
extern UINT8 VideoRecBegin;
extern UINT8 g_bNoAudio;
extern  xdata   UINT8   G_FrameRate;
extern UINT32 vid_ByteAddr,vid_TmpNum;
extern UINT32 vid_AudOutIdx;
extern UINT8 vlcBufNum;

extern UINT8 g_vidAviFrameRate;
extern UINT16 dzoomDispXZoomFactor;
extern UINT16 dzoomDispYZoomFactor;
extern UINT32 vidStorageSpeed; // KB/sec
extern UINT8 QTblVlcOv[130];
extern UINT16 g_vidParam[VD_PARAM_MAX];
//extern UINT32 *oldSLFilename;
//static UINT16 oldFilenameIndex;
extern UINT32 chkFlg;
extern xdata UINT16 G_USR_FileIndex;
extern UINT32 g_brcTargetSize;
extern SINT32 audDataSize;
//extern  UINT32 VID_TOTAL_FRAGSIZE;
//extern UINT32 FRAG_BUF_SIZE;
//extern UINT32 FRAGBUFCNT;
//extern UINT32 SEC_PER_FRAG_BUG;
extern void videoRec2ndFacSet(UINT16 Src1stX,UINT16 Src1stY);
extern UINT8 HAL_JpgZFactorCropSet(UINT16 hfactor,	UINT16 vfactor,UINT16 SrcWidth,UINT16 SrcHeight,UINT8 distClampx,UINT8 distClampy,UINT16 DstWidth,UINT16 DstHeight,UINT8 option);
extern void videoBrcUpdateQValBuf();
#endif
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
 
  /**
 * @fn               UINT8 videoSLRecCloseFile(void)
 * @brief	     video seamless loop rec
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
 #if Video_SL_Old
 UINT8 videoSLRecCloseFile(void)
{
    
    UINT32 tmp0;
	UINT32 dwScale = 0;
	UINT32 dwRate = 0;
	UINT32 MoviSize = 0;
	UINT32 AviSize = 0;
	UINT8 rwMode;
	float frameRate = 0; 
//	UINT32 time;
	UINT32 num=0;
	UINT32 cluster,clusterAlignmentSize=0;
	devAttr_t* pDev;//add by phil for disk driver lib release
//	UINT8 recMode;
	UINT32 junkSize, alignSize;
	UINT32 tmpSct;
	#if	(VIDEO_FRAME_DEBUG_LEVEL>0)
	UINT8 Reg22B8,Reg22B9,Reg22BA,Reg22BB;
	UINT8 Reg281A,Reg281B,Reg2818,Reg2819,Reg2860,Reg2861,Reg2862,Reg2863;
	UINT8 Reg21C8,Reg21CC,Reg21CD,Reg21CE,Reg21CF;
	#endif
	if(vid_ByteAddr >= (G_FragBufSize*(UINT32)vid_FragCnt)) {
		if(vid_TmpNum >= G_FragBufSize) {
			tmpSct = vid_TmpNum/ G_FragBufSize;
			if(vidClipCtrl.SecAddr > (1-chkFlg)) {
				vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)tmpSct *G_FragBufSize, vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt,0);
			}
			else {
				vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)tmpSct *G_FragBufSize, vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt,1);
			}
			vidClipCtrl.SecAddr  +=  (G_SecPerFrag * (UINT32)tmpSct);
			vid_TmpNum =  vid_TmpNum%G_FragBufSize;
			#if 0
			HAL_GprmDramDmaAdv(((vidClipCtrl.CurSrcImgAddr + (FRAG_BUF_SIZE>>1) *(UINT32)vid_FragCnt)<<1)+(UINT32)tmpSct *FRAG_BUF_SIZE,
				(vidClipCtrl.CurDMATagAddr<<1), vid_TmpNum, 1);
			#else
			vidClipCtrl.CurSrcImgAddr = vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt+(UINT32)tmpSct *(G_FragBufSize>>1);
			#endif
		}
		else {
			if(vid_ByteAddr > VID_TOTAL_FRAGSIZE) {
				vidClipCtrl.CurSrcImgAddr = vidClipCtrl.CurSrcImgAddr +VID_TOTAL_FRAGSIZE/2;
			}
			else {
				vidClipCtrl.CurSrcImgAddr = vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt;
			}
		}
	}
	else {
		if(vid_TmpNum > G_FragBufSize) {
			if(vidClipCtrl.SecAddr > (1-chkFlg)) {
				vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)(G_FragBufNum - vid_FragCnt) *G_FragBufSize, vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt,0);
			}
			else {
				vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)(G_FragBufNum -vid_FragCnt)*G_FragBufSize, vidClipCtrl.CurSrcImgAddr + (G_FragBufSize>>1) *(UINT32)vid_FragCnt,1);
			}
			vid_FragCnt = 0;
			vid_TmpNum -=  (G_FragBufSize* (UINT32)(G_FragBufNum -vid_FragCnt));
			vidClipCtrl.SecAddr  +=  (G_SecPerFrag * (UINT32)(G_FragBufNum -vid_FragCnt));

		}
		else {
			//error appear
			//ASSERT(0, 0);
			printf("errr\n");
		}
	}
	alignSize = DOS_ClusterSizeGet();
	if(alignSize < CHUNKSIZE){//vidClipCtrl.SecAddr is used to cal. movi size, cluster=512,1024,2048..., CHUNKSIZE=2048
		alignSize = CHUNKSIZE;
	}
	junkSize = ((UINT32) (vidClipCtrl.SecAddr << SECPERCHK) + vid_TmpNum) % alignSize; //movi data size
	//xian --
	#if 1
	num = vid_TmpNum;
    #ifdef OpenTwoFile
	if(num != 0 || junkSize) {
		#if 0 //xian -- 20101020
		videoRecAddJunk(vidClipCtrl.CurSrcImgAddr +num/2 , G_FragBufSize -num );
		if(vidClipCtrl.SecAddr > (1-chkFlg)) {
			vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)G_FragBufSize, vidClipCtrl.CurSrcImgAddr,0);
		}
		else {
			vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)G_FragBufSize, vidClipCtrl.CurSrcImgAddr,1);
		}
		vidClipCtrl.SecAddr  += G_SecPerFrag;
		#else //xian ++ 20101020
		if(junkSize){
			junkSize = alignSize - junkSize;  
			if(junkSize <= 8){
				junkSize += alignSize;
			}		
		  videoRecAddJunk(vidClipCtrl.CurSrcImgAddr +num/2 , junkSize);		
		}
		vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, num + junkSize, vidClipCtrl.CurSrcImgAddr,1);		
		vidClipCtrl.SecAddr += (num + junkSize)>> SECPERCHK;
		#endif
	}
    #endif
	#endif
	rwMode = K_DOS_NewFile;
	
  /*if(vidClipCtrl.AudChunkCnt== 0) {
		ASSERT(0, 1);
	}  *///wangrf edit 2011.3.14
	#if 1 //wangrf 3-22
	#if	(VIDEO_FRAME_DEBUG_LEVEL>0)
	if (1){
		Reg22B8 = XBYTE[0x22b8];
		Reg22B9 = XBYTE[0x22b9];
		Reg22BA = XBYTE[0x22ba];
		Reg22BB = XBYTE[0x22bb];
		Reg2818 = XBYTE[REG_Jpeg_ImgWidth_Low];
		Reg2819 = XBYTE[REG_Jpeg_ImgWidth_High];
		Reg281A = XBYTE[REG_Jpeg_ImgHeight_Low];
		Reg281B = XBYTE[REG_Jpeg_ImgHeight_High];
		Reg2860 = XBYTE[REG_Jpeg_Srcwidth_Low];
		Reg2861 = XBYTE[REG_Jpeg_Srcwidth_High];
		Reg2862 = XBYTE[REG_Jpeg_Srcheight_Low];
		Reg2863 = XBYTE[REG_Jpeg_Srcheight_High];
		Reg21C8 = XBYTE[0x21C8];
		Reg21CC = XBYTE[0x21CC];
		Reg21CD = XBYTE[0x21CD];
		Reg21CE = XBYTE[0x21CE];
		Reg21CF = XBYTE[0x21CF];
		
	}
	#endif

	//++ASSERT(XBYTE[0x27e3] !=0, 1);
	//printf("00: %bx\n", XBYTE[0x27e3]);
	#ifdef VIDEO_BRC_TEST
	printf("qval:%bu, %bu, %bu\n", g_brcTestQ[0], g_brcTestQ[1], g_brcTestQ[2]);
	printf("frmsz:%lu, %lu, %lu\n", g_brcTestFrmSz[0], g_brcTestFrmSz[1], g_brcTestFrmSz[2]);
	#endif
	#if 0
	//xian ++ 20061224
	recMode = videoRecStampRecModeGet();
	if(!vidDateStampSts || (recMode != VD_STAMP_REC_MODE_END_FRAME && recMode != VD_STAMP_REC_MODE_FST_END_FRAME)){
	videoRecRemoveEndVoice(vidIndexNum,0); //mask for date stamp
	}
	//xian --
	#endif
    #endif //wangrf 3-22
	//videoRecWriteIndexToCard(0, 0);//write index to index file
	vidClipCtrl.idxSize += vidIndexNum;
	tmp0 = (vidClipCtrl.AudChunkCnt + vidClipCtrl.VidChunkCnt) << 4;
	cluster = DOS_ClusterSizeGet();
	if((vidIndexNum + vidClipCtrl.hdrSize)%cluster) {
		clusterAlignmentSize = cluster - (vidIndexNum + vidClipCtrl.hdrSize)%cluster;
		//printf("cluster++ size:%ld\n",((vidIndexNum + vidClipCtrl.hdrSize)%cluster));
		HAL_GprmDramFill(vidClipCtrl.AVIIdxAddr, clusterAlignmentSize, 0);
		vidIndexNum += clusterAlignmentSize;
	}
	
	videoRecWriteIndexToCard(0, 1);//write index to header file

	/* add by phil for disk driver lib release s*/
	if(G_ucStorageType != K_MEDIA_SDF){//fix 41831
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			sp1kFlashCacheCtrl(0);
		}
	}
	/* add by phil for disk driver lib release e*/

    #ifdef OpenTwoFile
	MoviSize = (UINT32) (vidClipCtrl.SecAddr << SECPERCHK) - 8;
	AviSize = (UINT32) (vidClipCtrl.SecAddr  << SECPERCHK) + vidClipCtrl.hdrSize + vidClipCtrl.idxSize - 8;
    #else
	MoviSize = (UINT32) ((vidClipCtrl.SecAddr << SECPERCHK) - 512) +chkFlg*512 +  (UINT32) num - 508;
	AviSize = (UINT32) ((vidClipCtrl.SecAddr  << SECPERCHK) - 512) + chkFlg*512+tmp0 + (UINT32) num - 8;
    #endif

	#if 1
	DOS_FileRead(vidClipCtrl.fHandle, vidClipCtrl.CurSrcImgAddr<<1, 512, 0);
	videoFillDramLong(vidClipCtrl.CurSrcImgAddr + 2, MoviSize );
	DOS_FileSeek(vidClipCtrl.fHandle, 0 ,0);
	vfsFileWrite(vidClipCtrl.fHandle, K_File_SrcDataDram2Card, (UINT32)512, vidClipCtrl.CurSrcImgAddr,0);
	#endif
	//account frame rate
	if(vidClipCtrl.AudChunkCnt) {
		if(vidClipCtrl.audFmt == VID_ADPCMFMT) {
			dwRate =vidClipCtrl.VidChunkCnt * 1000 ;
			dwScale =  (UINT32)((float)vidClipCtrl.AudChunkCnt *(float)1017 /((float)vidClipCtrl.sampRate *(float)BLOCK_ALIGN/ ((float)CHUNKSIZE*1000)));
		}
		else {
			dwRate = vidClipCtrl.VidChunkCnt *1000;
			dwScale =  (UINT32)((float)vidClipCtrl.AudChunkCnt *(float)1000 /((float)vidClipCtrl.sampRate *(float)2/ (float)CHUNKSIZE));
		//	printf("audT:%ld\n",dwScale);
		//	printf("GT:%ld\n",time);
		}
		frameRate = (float) dwRate/ (float) dwScale;
	}
	else {
		//ASSERT(0, 1); // wangrf edit 2011.3.14
		dwScale = dwScale;
		dwRate = vidClipCtrl.VidChunkCnt * 1000;
	}

	DOS_FileRead(vidClipCtrl.fHandle_hdr, vidClipCtrl.AVIHeadAddr<<1, 1024, 0);
	
    #if 1 //wangrf++ 3-18
    #if	(VIDEO_FRAME_DEBUG_LEVEL>0)
	if (1){
	UINT32 FRAME_CNT;
	extern UINT32 g_brcTargetSize;
	//UINT16 FallFrmNum;
	FRAME_CNT = vidVlcDMACnt + 1;
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+160, FRAME_CNT);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+162, DummyFrameCnt);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+164, vlcOverflowCnt);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+166, KBPerSec);
	
   #ifdef VIDEO_REC_BRC_EN
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+168, (UINT32)QIdxMax);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+170, (UINT32)QIdxMin);
   #endif
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+172, (UINT32)dumyIdxTotal);
   #if DebugDropFrame
	videoFillDramLong(vidClipCtrl.AVIHeadAddr+174, (UINT32)g_brcTargetSize);
   #endif
	HAL_DramStartAddrSet(vidClipCtrl.AVIHeadAddr + 176, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite('C', 'D');
	HAL_DramWordDataWrite(Reg22B8, Reg22B9);
	HAL_DramWordDataWrite(Reg22BA, Reg22BB);
	HAL_DramWordDataWrite(Reg2818, Reg2819);
	HAL_DramWordDataWrite(Reg281A, Reg281B);
	HAL_DramWordDataWrite(Reg2860, Reg2861);
	HAL_DramWordDataWrite(Reg2862, Reg2863);
	HAL_DramWordDataWrite(Reg21C8, 0x88);
	HAL_DramWordDataWrite(Reg21CC, Reg21CD);
	HAL_DramWordDataWrite(Reg21CE, Reg21CF);
	HAL_DramWordDataWrite((UINT8)videoZoomFctGet_NBlocking(), videoZoomFctGet_NBlocking()>>8);
	}
   #endif  
   #endif //wangrf++ 3-18
   #if 1  //wangrf++ 3-22
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 8, 0x1E0 + vidClipCtrl.hdrSize - 512+12);//hdr size
	if(vidClipCtrl.audFmt == VID_ADPCMFMT)
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 158+8, 0xA4 + vidClipCtrl.hdrSize - 512+12);//junk size
	else
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 158, 0xB4 + vidClipCtrl.hdrSize - 512+12);//junk size
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 512/2+2, tmp0 );//index size

	if(!vidClipCtrl.AudChunkCnt) {
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 28, 1);
	}
	tmp0 = 1000000 / frameRate;
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 70, vidClipCtrl.VidChunkCnt);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 24, vidClipCtrl.VidChunkCnt);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 64, dwScale);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 66, dwRate);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 2, AviSize);
	videoFillDramLong(vidClipCtrl.AVIHeadAddr + 16, tmp0);

	if(vidClipCtrl.imgWidth == 1280){	//xian ++ 20091023, for pc used
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 30, (UINT32)0x300000); //SuggestedBufferSize
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 72, (UINT32)0x300000);
	}

	if(vidClipCtrl.audFmt == VID_ADPCMFMT) {
		UINT32 avgBytePerSec;
		UINT32 samplePerBlock;

		samplePerBlock = (BLOCK_ALIGN - 4 ) * 2 + 1;
		avgBytePerSec = (UINT32)vidClipCtrl.sampRate * BLOCK_ALIGN / SMPPERBLK;

		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x108/2, vidClipCtrl.AudChunkCnt*(CHUNKSIZE/0x200));
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0xfc/2, samplePerBlock);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x100/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x130/2, avgBytePerSec);
		samplePerBlock =( (samplePerBlock & 0xffff) << 16 )| 0x02;
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x138/2, samplePerBlock);
	}
	else {
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x108/2, (vidClipCtrl.AudChunkCnt <<SECPERCHK)/(SAMPLE_BIT/8));
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0xfc/2, 1000);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x100/2, (UINT32)vidClipCtrl.sampRate *1000);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(vidClipCtrl.AVIHeadAddr + 0x130/2, (UINT32)vidClipCtrl.sampRate<<1);
	}
      #endif //wangrf 3-22
	//rewrite AVI header to card
	//eric@20080502,begin
	DOS_FileSeek(vidClipCtrl.fHandle_hdr, 0 ,0);
	vfsFileWrite(vidClipCtrl.fHandle_hdr, K_File_SrcDataDram2Card, 1024, vidClipCtrl.AVIHeadAddr, 1);
  
	#ifdef OpenTwoFile

	{
		UINT32 fsize= 1048576;
		UINT32 wsize;
		UINT32 pos=512;
		UINT32 tmp;

		//xian ++ 20091119, 1M buf is covered by preview buf
		//vidClipCtrl.AVIHeadAddr = sysMemAddrGet(MEM_IDX_VIDEO_REC_IDX);
		vidClipCtrl.AVIHeadAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX);//sysMemAddrGet(MEM_IDX_VIDEO_REC_IDX);
		while(vidClipCtrl.idxSize) {
			wsize = vidClipCtrl.idxSize > fsize ? fsize : vidClipCtrl.idxSize;
			if(wsize%512) {
				tmp= ((wsize+511)>>9)<<9;
			}else{
				tmp=wsize;
			}
			#if (SD_R_INT_EN || SD_W_INT_EN)
			// wait until SD finish writing last file
			while( sdBgRwStsGet() ==  SD_BG_WRITE);
			#endif
			//printf("cnt=%lu\n",DOS_FileRead(vidClipCtrl.fHandle_hdr, vidClipCtrl.AVIHeadAddr<<1, tmp, pos));
			//printf("cnt=%lu\n",DOS_FileWrite(vidClipCtrl.fHandle_idx, vidClipCtrl.AVIHeadAddr<<1, wsize));
			DOS_FileRead(vidClipCtrl.fHandle_hdr, vidClipCtrl.AVIHeadAddr<<1, tmp, pos);
			DOS_FileWrite(vidClipCtrl.fHandle_idx, vidClipCtrl.AVIHeadAddr<<1, wsize);
			pos += wsize;
			vidClipCtrl.idxSize -= wsize;
		}
	}
	videoRecTimeUpdate();
	DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle);
	DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle_idx);
	DOS_FileClose(vidClipCtrl.fHandle_hdr);
	#else
	if (G_Card_Type == K_MEDIA_SMC || G_Card_Type == K_MEDIA_NANDF) {
		//eric@20080502
		vfsFileClose(vidClipCtrl.fHandle, vidClipCtrl.name, rwMode, AviSize + 8);
	}
	else {
		//eric@20080502
		vfsFileClose(vidClipCtrl.fHandle, vidClipCtrl.name, rwMode, AviSize + 8);
	}
	#endif

	//write FDB parameter
	
	dcfNewFileDramFdbParameterUpdate(K_DCF_AVI_TYPE, AviSize + 8);
	
    //G_Image_QTableIndex = XBYTE[REG_Jpeg_Yqtable];
	return SUCCESS;
}

  
 /**
 * @fn              UINT8 videoSLRecOpenFile(void)
 * @brief	    open three file for clip
 * @param[in]   NONE.
 * @retval         NONE
 * @see videoSLRecOpenFile(void)
 */

  UINT8 videoSLRecOpenFile(void)
{
	UINT8 rwMode;
	UINT16 i;
	UINT16 fileCnt;
	UINT16 fileIndex;
	UINT8 fileOldname[12],filenewname[12];
	UINT8 filterType=K_DCF_OUR_TYPE;
	//UINT16 index;
	//UINT32 t0,t1;
	#if VIDEO_SLCLIP_FILE_NOT_DELETE_READONLY
	UINT8 z,filenextname[12];
	UINT8 type,attr,attrNext;
	UINT16 j=0,fileIdexnext;
	if(G_USR_FileIndex>9999){
         fileCnt = pbFileCntGet();
	 //printf(" fileCnt=%x\n",fileCnt)
	   for(i=0;i<=fileCnt;i++){
	   	
	  	fileIndex=DOS_FileIndexGet(i);
		//fileIdexnext=DOS_FileIndexGet(j+1);
		//if(i==fileIndex){}
		//else{
		//HAL_GlobalReadGTimer(&t0);
		type = dcfGetFileName(fileOldname,fileIndex,&attr);
		if((attr&0x01)||(filterType && (type!=filterType))){
			j=fileIndex;
			}
	  	else{
			j++;
			//while(z!=0){
				if(type==1){
					dcfJpgNameGet(filenewname, j);
				}
				else{
		 			dcfAviNameGet(filenewname, j);
				}
				fileIdexnext=DOS_FileIndexGet(i+1);
				dcfGetFileName(filenextname,fileIdexnext,&attrNext);
				for(z=0;z<12;z++){
					
					if(filenextname[z]!=filenewname[z]){
						//j=j+2;
						//z=0;
						break;
					}
				}
					//HAL_GlobalReadGTimer(&t1);
			//printf("___*=%ld\n",t1-t0);
				if(z>=12){
					j=j+1;
					i--;
					}
			else{
	  	 	dcfFileAttrSet(fileOldname,filenewname,0,ATTR_MODIFY_NAME);
			 // dcfFileInitialize();
			 }
	  }
	   	}
	   dcfFileInitialize();
	  sp1kPbFileOrdertableInit(); 
	}
	#else
	if(G_USR_FileIndex>9999){
      		fileCnt = pbFileCntGet();
	 //printf(" fileCnt=%x\n",fileCnt);
	 	 for(i=0;i<=fileCnt;i++){
		  	fileIndex=DOS_FileIndexGet(i);
		  	 dcfAviNameGet(fileOldname, fileIndex);
			 dcfAviNameGet(filenewname, i);
		  	 dcfFileAttrSet(fileOldname,filenewname,0,ATTR_MODIFY_NAME);
	 	 }
	 
	 	 dcfFileInitialize();
	  	sp1kPbFileOrdertableInit(); 
	}
	#endif
	if(!dcfFileIndexUpdate()){	//xian ++ 20081203
		return FAIL;
	}
	dcfAviNameGet(vidClipCtrl.name_hdr, dcfFileIndexGet());
	rwMode = K_DOS_NewFile;
	vfsFileDelete(vidClipCtrl.name_hdr);
	vfsFileDelete(vidClipCtrl.name);
	vfsFileDelete(vidClipCtrl.name_idx);
	vidClipCtrl.fHandle_hdr= vfsFileOpen(vidClipCtrl.name_hdr, rwMode, 100);
	vidClipCtrl.fHandle = vfsFileOpen(vidClipCtrl.name, rwMode, 100);
	vidClipCtrl.fHandle_idx= vfsFileOpen(vidClipCtrl.name_idx, rwMode, 100);
	if ((vidClipCtrl.fHandle_idx== 0)||(vidClipCtrl.fHandle_hdr== 0)||(vidClipCtrl.fHandle == 0)) {
		//ASSERT(0, 1); //wangrf edit 2011.3.14
		return FAIL;
	}
	return SUCCESS;
}


 /**
 * @fn              UINT8 videoSLRecInitBF (void)
 * @brief	    videoSLRecInitBF
 * @param[in]   NONE.
 * @retval         NONE
 * @see            videoSLRecInitBF()
 */
UINT8 videoSLRecInitBF () 
{
    //UINT16 FileIdx;
	UINT32 k= 5000;

	devAttr_t* pDev;//add by phil for disk driver lib release

	//UINT32 clusterSize = vfsClusterSizeGet();
	#if 0
	while((XBYTE[0x2668]&0x02) ==0x02){
		if(!(k--)) {
			XBYTE[0x2600] |= 0x02;
			XBYTE[0x2608] &=  (~0x02);
			XBYTE[0x2668] &=  (~0x02);
			break;
		}
	}
	#endif
	//HAL_CpuImemSdSet(LOG_BANK_5, 0x8000);
	if (1){

		if (!sD_CdGet()){
			if (sp1kSDSpeedClassGet()>=SD_SPEED_CLASS4){//'0':class 0, '1': class 2, '2':class 4, '3':class 6
				StorageSpeed = 0x1888;
			}else if (sp1kSDSpeedClassGet()==SD_SPEED_CLASS2){
				StorageSpeed = 0x0BBB;
			}else{
				StorageSpeed = 0x0888;
			}
		}else{
			StorageSpeed = 0x0888;
		}

		#if (VIDEO_FRAME_DEBUG_LEVEL>0)
		KBPerSec = StorageSpeed;
		#endif
		if (StorageSpeed<2500){
				G_FragBufSize >>= 2;
				G_FragBufNum <<= 2;
				G_SecPerFrag >>= 2;
			}
			else if(StorageSpeed<4000){
				G_FragBufSize >>= 1;
				G_FragBufNum <<= 1;
				G_SecPerFrag >>= 1;
			}
	}
	//printf("62-2\n");	
	HAL_DramStartAddrSet(vidClipCtrl.AVIIdxAddr, K_SDRAM_NoPrefetch);
	HAL_DramWordDataWrite(0x69, 0x64);
	HAL_DramWordDataWrite(0x78, 0x31);

	#ifdef OpenTwoFile
	vidIndexNum += 8;//index ID + size
	#endif

	vidClipCtrl.AVIIdxAddr += 4;
	
	videoSLRecOpenFile();
	#if 1
	/* add by phil for disk driver lib release s*/
	if(G_ucStorageType != K_MEDIA_SDF){//fix 41831
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			sp1kFlashCacheCtrl(1);
		}
	}
	/* add by phil for disk driver lib release e*/
	#endif
	videoRecWriteMovIDToDram(vidClipCtrl.CurDMATagAddr);
	#if 0
	//start video and audio record
	if (g_vidParam[VD_REC_YUV_FORMAT] != VD_REC_YUV_422){
		VideoRecBegin = 1; //tq.zhang video yuv420
	}
	else{
		VideoRecBegin = 0;
	}	
	
	HAL_AudResume();
	#endif
	//#ifdef videoDebug
	//printf("exit videoClipINit\n");
	//#endif
	//printf("exit videoClipINit\n");
	return SUCCESS;
}


 /**
 * @fn              void videoSLRecInit(void)
 * @brief	    videoSLRecInit
 * @param[in]   NONE.
 * @retval         None
 * @see videoSLRecInit()
 */
void videoSLRecInit(void){
	//printf("62-1\n");
	audDataSize-=(SINT32)(vidClipCtrl.AudChunkCnt<<SECPERCHK);
	vidClipCtrl.AVIIdxAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX);//K_SDRAM_AviCaptureIdx1Addr;
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
	vid_TmpNum =0; //For cluster  alignment
	vid_ByteAddr = 0;//For cluster	alignment
	vid_FragCnt = 0;
	//vid_AudOutIdx = 0;
    #if VLC_BUF_CHG_EN
	vidVlcDMACnt=0;
	totalFrameCnt=0;
	//DummyFrameCnt = 0; 
	totalDummyFrame = 0;
    #endif
	#if 1 //2011.6.1
	G_FragBufSize = FRAG_BUF_SIZE;
	G_FragBufNum = FRAGBUFCNT;
	G_SecPerFrag = SEC_PER_FRAG_BUG; 
	#endif
	
	//dropFrmCnt=0;
    #ifdef OpenTwoFile
	vidIndexNum = 0;
	idx_f = 1;
	wf=0;
    #endif
	#if DebugDropFrame
	//Drop12=0;
	#endif
	//g_dumyCopy = 0;
	//g_bReserveStampFrameSize = 0; //2011.6.1
	#if VD_REC_DEBUG
	vdDebugInit();
	#endif
	slflag=0;
	slopenfiletimeflag=0;
	slopenfiletime=0;
    videoSLRecInitBF();
}
 #endif
 /**
 * @fn void sp1kVideoSeamlessLPRecStsSet (UINT8 en)
 * @brief	Enable or disable VideoSeamlessLPRec  
 * @param[in] en  Secify VideoSeamlessLPRec  status. 1:enable, 0:disable. 
 * @retval none
 */ 
void videoSeamlessLPRecStsSet(UINT8 en)
{

   seamLessLpRecEn=en;
}
 /**
 * @fn void sp1kVideoSeamlessLPRecFilesizeSet(UINT32 filesize)
 * @brief	 VideoSeamlessLPRec  each file size set
 * @param[in] en  Secify VideoSeamlessLPRec  file size. 
 * @retval none
 */ 
void videoSeamlessLPRecFilesizeSet(UINT32 filesize)
{
	videoSeamlessRecTime=0xffffffff;
     	videoSeamlessFilesize=filesize;	
}
 void videoSeamlessLPRecTimeSet(UINT32 rectime)
 {
 	videoSeamlessRecTime=rectime;
	videoSeamlessFilesize=0xffffffff;	
 }
/* wangjg++ 还未实现
 * 1、seamless rec 专门建立一种目录：XXXSLREC，与我们标准的XXXMEDIA区别开来，当然这个名字符合DCF规范，可以由客户指定，但不能与正常录影拍照的目录名相同； 
 * 2、XXXSLREC这个目录下，seamless rec lib具有完全读写权限，也就是说这个目录下，无缝录影时可以任意删除DCF文件（不负责任）； 
 * 3、正常拍照录影不会写到这个目录； 
 * 4、无缝录影开始时，会到DCF层去search这种目录，如果不存在，创建新的；如果存在多个，选择最后一个。 
*/
#if Video_SL_Old
UINT8 videoSLRecMemCheck()
{
	UINT32 clusterSize = vfsClusterSizeGet();
	if(!seamLessLpRecEn){return SUCCESS;}
	vidClipCtrl.freeCluster = DOS_FreeDiskSpaceByCluster();
	vidClipCtrl.usedCluster=1+videoSeamlessFilesize/(UINT32)clusterSize;
	printf("%lu,%lu,%lu,%lu\n",videoSeamlessFilesize,clusterSize,videoSeamlessFilesize /clusterSize,vidClipCtrl.freeCluster);
	if((videoSeamlessFilesize*3 /clusterSize) > vidClipCtrl.freeCluster){
		return FAIL;
	}
	return SUCCESS;
}

UINT8 videoSLRecMemCheck()
{
	UINT8 ret = SUCCESS;
	UINT16 idx = dcfDir1IndexGet();	
	//UINT16 i = dcfDir1IndexGet();	
	FDBMediaFolderInfo_t *info;
	while(ret==SUCCESS){
		ret = USR_GetMediaFolderInfo(idx, info);		
		if(0==strncmp(info->name+3,"SLREC",5)){
			break;
		}	
		idx = info->prevCnt;
	};
	if(ret==FAIL){
		ret=SUCCESS;
		idx = dcfDir1IndexGet();
	}
	if(FAIL==dcfMediaFolderAssign(1,idx)){
		return FAIL;
	}
	
	UINT32 clusterSize = vfsClusterSizeGet();
	if(!seamLessLpRecEn){return SUCCESS;}
	vidClipCtrl.freeCluster = DOS_FreeDiskSpaceByCluster();
	vidClipCtrl.usedCluster=1+videoSeamlessFilesize/(UINT32)clusterSize;
	printf("%lu,%lu,%lu,%lu\n",videoSeamlessFilesize,clusterSize,videoSeamlessFilesize /clusterSize,vidClipCtrl.freeCluster);
	while((videoSeamlessFilesize*3 /clusterSize) > vidClipCtrl.freeCluster){
		if(FAIL==dcfDeleteFile(1,1)){
			ret = FAIL;
			break;
		}
		printf("%lu,%lu,%lu,%lu\n",videoSeamlessFilesize,clusterSize,videoSeamlessFilesize /clusterSize,vidClipCtrl.freeCluster);
	}
	return ret;
}


#endif
UINT8 videoSLRecMemCheck()
{
	UINT8 ret;
	extern UINT32 vidBrcTagSize;
	UINT32 clusterSize = vfsClusterSizeGet();

	UINT32 freeSize = DOS_FreeDiskSpaceByCluster();
	UINT32 reqSize = videoSeamlessFilesize;
	UINT16 order = 1;
	if(!seamLessLpRecEn){return SUCCESS;}
	if(reqSize==0xffffffff){
		reqSize= videoSeamlessRecTime*vidClipCtrl.frameRate*vidBrcTagSize;
	}
	reqSize/= clusterSize;	
	reqSize+=reqSize>>1;
	//printf("sl:%lu(%lu,%lu)%lu(%lu,%lu)\n",clusterSize,videoSeamlessFilesize ,videoSeamlessRecTime,vidBrcTagSize,reqSize,freeSize);
	while(reqSize > freeSize) {		
		ret = dcfDeleteFile(order,1,K_DCF_OUR_TYPE);		
		if(ret==FAIL){
			order++;
			//osMsgPost(sp1k_MSG_REC_SEAMLESS_DISK_FULL);
			//printf("del err!\n");
			//return FAIL;
		}else if(ret ==2){
			//break;
			return FAIL;
		}
		freeSize = DOS_FreeDiskSpaceByCluster();
		//printf("--%bu-%lu-%lu\n",order,reqSize,freeSize);
	}	
	
	return SUCCESS;
}
UINT8 videoRecRenameSeamless(UINT8 protectedFilter,UINT8 typeFilter)
{
	UINT16 i;
	UINT16 fileCnt;
	UINT8 type,attr;
	UINT16 idx=dcfFileIndexGet(),idxNew=0;
	UINT8 fname[13],fnameNew[13];
	UINT32 t0,t1;
	UINT32 addr = K_SDRAM_JPG_FdbParameterBufAddr<<1;
	printf("rename ");
	//if(idx<9999){return TRUE;}
	HAL_GlobalReadGTimer(&t0);
	fileCnt = pbFileCntGet();
	for(i=1;i<=fileCnt;i++){
		idx=DOS_FileIndexGet(i);		
		type = dcfGetFileName(fname,idx,&attr);
		
		if((protectedFilter && (attr&0x01))||(typeFilter && (type != typeFilter))){
			idxNew = idx;		
		}
		else{
			idxNew++;
			if(idx>idxNew){
				dcfMediaNameGet(fnameNew, idxNew,type-1);//MEDIA_AVI);
				dcfFileAttrSet(fname,fnameNew,0,ATTR_MODIFY_NAME);
				HAL_GprmDramDmaAdv(addr+((UINT32)idx - 1) * 20, addr+((UINT32)idxNew - 1) * 20, 20, 1);
				HAL_GprmDramFillExt(addr+((UINT32)idx - 1) * 20, 20, 0, 0, 0);
				DOS_FileOrderWrite(i,idxNew);
			}
		}
	}
	G_USR_FileIndex = idxNew+1;
	HAL_GlobalReadGTimer(&t1);
	//dcfFileInitialize();
	//DOS_FileOrderTblInit(); 	
	//HAL_GlobalReadGTimer(&t2);
	printf("+:%lu\n",t1-t0);
	return idx<9999;
}

