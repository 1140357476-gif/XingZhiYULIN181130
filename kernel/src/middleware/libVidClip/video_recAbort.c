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
#include "exif_entry.h"
#include "dbg_mem.h"

#define __FILE	  __FILE_ID_VIDEO_RECABORT__
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
 extern UINT8 pbAviMMsFun(UINT8 *fileName,UINT16 width,UINT16 height);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT8 videoAbortCheckAvi()
{
	devAttr_t* pDev;//add by phil for disk driver lib release
	//if video frames less than 10 , this file will be not	stored
	
	/* add by phil for disk driver lib release s*/
	if(G_ucStorageType != K_MEDIA_SDF){//fix 41831
		UINT8 idx;
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			sp1kFlashCacheCtrl(0);
			for (idx = 0; idx < G_VideoVlcBufNum; idx++) {
		  		 videoRec();
			}
			return TRUE;
		}
		__PRINTF_POS();
		return FALSE;
	}
	else if(sD_CdGet()){
		
		DOS_FileClose(bgFileMov.fno);
		DOS_FileClose(bgFileHdr.fno);
		DOS_FileClose(bgFileIdx.fno);
		bgStop();
		__PRINTF_POS();
		return FALSE;
	}
	return TRUE;
}
#if 1
UINT8 videoAbortCloseFile()
{
	UINT32 aviSize = (UINT32) (vidClipCtrl.SecAddr  << SECPERCHK) + vidClipCtrl.hdrSize + vidClipCtrl.idxSize;
	if(vidBgEn){
		dcfMediaNameGet(vidClipCtrl.name_hdr,dcfFileIndexGet(),MEDIA_AVI);
		bgFileJoint(&bgFileHdr, vidClipCtrl.name_hdr, 1);
		bgStop();
		bgPrintfNodes(&bgFileHdr);
		bgPrintfNodes(&bgFileMov);
		bgPrintfNodes(&bgFileIdx);
		bgPrintfNodes(NULL);
	}else{
		bgFileHdr.clusterFirst= dos.file[vidClipCtrl.fHandle_hdr].FirstCluster;
		bgFileIdx.clusterFirst = dos.file[vidClipCtrl.fHandle_idx].FirstCluster;
		DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle);
		DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle_idx);
		DOS_FileClose(vidClipCtrl.fHandle_hdr);		
	}
	dcfNewFileDramFdbParameterUpdate(K_DCF_AVI_TYPE, aviSize);
	return TRUE;
}
#else
UINT8 videoAbortCloseFile()
{
	UINT32 aviSize = (UINT32) (vidClipCtrl.SecAddr  << SECPERCHK) + vidClipCtrl.hdrSize + vidClipCtrl.idxSize - 8;
	DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle);
	DOS_FileJoint(vidClipCtrl.fHandle_hdr,vidClipCtrl.fHandle_idx);
	DOS_FileClose(vidClipCtrl.fHandle_hdr);

	 //xian add new api
	dcfNewFileDramFdbParameterUpdate(K_DCF_AVI_TYPE, aviSize + 8);
	return TRUE;
}
UINT8 videoAbortCloseFileBg()
{
	dcfMediaNameGet(vidClipCtrl.name_hdr,dcfFileIndexGet(),MEDIA_AVI);
	bgFileJoint(&bgFileHdr, vidClipCtrl.name_hdr, 1);
	//vidClipCtrl.fHandle = bgFileClose(&bgFileMov, NULL, 0);	
	//vidClipCtrl.fHandle_hdr= bgFileClose(&bgFileHdr, vidClipCtrl.name_hdr, 0);	
	//vidClipCtrl.fHandle_idx= bgFileClose(&bgFileIdx, NULL, 0);
	bgStop();
	//printf("handle(%u,%u,%u)\n",fHandle,fHandle_hdr,fHandle_idx);
	//vidPrintfAbort("%lu,%lu(%lu,%lu)\n",vidClipCtrl.AudChunkCnt ,vidClipCtrl.VidChunkCnt,vidClipCtrl.AudChunkCnt + vidClipCtrl.VidChunkCnt,vidClipCtrl.totalChuck);
	return TRUE;
}
#endif
UINT8 videoAbortMjpg()
{
	ACTIVE_FUNC_CLR();
	//HAL_StopGTimer();
	HAL_AudStop( 0 );
	/*For HDMI beep Voice*/
	HAL_AudPause(); 
	HAL_JpegMjpgAbort();

	videoRecEofIntrDisable();
	XBYTE[REG_Dram_VlcRdy] = 3;

	ASSERT(XBYTE[REG_Dram_ClipIdle] !=0, 1);
	//for can not rec when rec abort XBYTE[REG_Dram_ClipIdle]==0 
	if(XBYTE[REG_Dram_ClipIdle]==0){
		XBYTE[REG_Jpeg_Swreset]=0x01;    //JPG software reset
		XBYTE[REG_Jpeg_Swreset]=0x00;
	}

	//write FDB parameter
	vidClipCtrl.recIdle = VID_REC_IDLE;
	
	//Preview_Init();
	jpegQTableSet( K_Qxx, NULL, NULL, 1 );

	//G_Image_QTableIndex = XBYTE[REG_Jpeg_Yqtable];
	
	/*++++++TQ add for clear vlc buffer index to 0 +++*/
	// 20100630
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |=0x10; 
	XBYTE[REG_Dram_VlcRegIdx] =0x00; 
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &=0xef; 
	/*------------------------------------------*/	
	/*+++++for clear JPEG first status+++++*/
	//20100701
	
	XBYTE[REG_Dram_Priority0_lvl2_master] = 0x01; // default value
	XBYTE[REG_Dram_Priority1_lvl2_master] = 0x00;
	HAL_JpegVlcBoundSet(0, 0,0xffffffff);	
	return TRUE;
}
#if VideoNoUsed
UINT8 videoAbortWriteVlc()
{
	UINT32 t0 = videoRecDbgTime;
	//write vlc buf to frag buf;
	while(idxVlcHead != VD_BUF_IDX_EOF){
		if(FAIL == videoRecVlcDataProcess(0)){break;}
		videoRecAudDataProcess(1);
		if(videoRecDbgTime>t0 && videoRecDbgTime-t0>100000){
			__PRINTF_POS();
			break;
		}
	}
	return TRUE;
}
#endif
UINT8 videoAbortWriteMovie()
{	
	if(vid_TmpNum)
	{
		UINT32 bufSize = (vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_FRAG)<<1)&0xffff8000;
		UINT32 junkAddr = vid_ByteAddr + (vidClipCtrl.CurDMATagAddr<<1);
		//UINT32 dataOffset = vid_ByteAddr;
		//UINT32 dataSize = vid_TmpNum;

		UINT32 junkSize = 0x8000 -(vid_TmpNum&0x7fff);
		if(junkSize){
			videoRecAddJunk(junkAddr/2 , junkSize+512-8);
			videoRecWriteToFragBuf(junkAddr, junkSize);
			vidDbgDataFlow(2,3,junkSize);
			videoRecFragSaveCard(0);
		}
		vidPrintfAbort("junk:%lu,%lu\n",junkAddr,junkSize);
		if(vid_TmpNum){
			videoRecFragSaveCard(0);
		}
	}
	
	return TRUE;
}
//-----------------------------------------------------------------------------
//videoAbortWriteMovieData
//-----------------------------------------------------------------------------
/**
 * @fn		 UINT8 videoAbortWriteMovieData(void)
 * @brief	  write frag data to card when video rec abort
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-10-09
 * @todo	  N/A
 * @bug 	  N/A
*/
#if VideoNoUsed
UINT8 videoAbortModifyMovie()
{
	UINT32 MoviSize = (UINT32) (vidClipCtrl.SecAddr << SECPERCHK) - 8 + 512;
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)<<1;
	DOS_FileRead(vidClipCtrl.fHandle, addr, 512, 0);
	videoFillDramLong(vidClipCtrl.CurSrcImgAddr + 2, MoviSize );
	DOS_FileSeek(vidClipCtrl.fHandle, 0 ,0);
	DOS_FileWrite(vidClipCtrl.fHandle, addr,512);	
	return TRUE;
}
#endif

/**
 * @fn		 void videoAbortSaveHeader()
 * @brief	  fill avi header
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-10-09
 * @todo	  N/A
 * @bug 	  N/A
*/
#if 1
UINT8 videoAbortModifyHeader()
{
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR);//vidClipCtrl.AVIHeadAddr;
	UINT32 dwScale = 0;
	UINT32 dwRate = 0;
	
	float frameRate = 0;
//	UINT8 rwMode;
	UINT32 num=0;
	
	UINT32 hdrSize = vidClipCtrl.hdrSize - 512 + 0x1E0;
	UINT32 hdrJunkSize = vidClipCtrl.hdrSize  - 512 + 0xB4;
	UINT32 idxSize = vidClipCtrl.idxSize-512;
	//UINT32 movSize = vidClipCtrl.SecAddr  << SECPERCHK;
	//UINT32 movJunkSize;
	UINT32 AviSize = vidClipCtrl.hdrSize +  (vidClipCtrl.SecAddr  << SECPERCHK) + vidClipCtrl.idxSize - 8;	
	UINT32 tmp0;
	UINT32 firstSector0,firstSector1;
	printf("size:%lx=%lx+%lx+%lx\n",AviSize,vidClipCtrl.hdrSize,(vidClipCtrl.SecAddr  << SECPERCHK) , vidClipCtrl.idxSize);
	//dramFileSave("HDR2", addr, 0x200);
	//printf("(%lx,%lx)(%lx,%lx)\n",hdrSize,vidClipCtrl.hdrSize);
	//printf("idx:%lu,%lu+%lu=%lu\n",tmp0,vidClipCtrl.AudChunkCnt,vidClipCtrl.VidChunkCnt,(vidClipCtrl.AudChunkCnt + vidClipCtrl.VidChunkCnt) << 4);
	videoFillDramLong(addr + 2, AviSize);//avi size
	videoFillDramLong(addr + 8, hdrSize);//hdr size
	videoFillDramLong(addr + 158, hdrJunkSize);//hdr junk size
	videoFillDramLong(addr + 256-2, idxSize );//index size

	if(!vidClipCtrl.AudChunkCnt) {
		videoFillDramLong(addr + 28, 1);
	}	

	if(vidClipCtrl.imgWidth == 1280){	//xian ++ 20091023, for pc used
		videoFillDramLong(addr + 30, (UINT32)0x300000); //SuggestedBufferSize
		videoFillDramLong(addr + 72, (UINT32)0x300000);
	}
    	if(vidClipCtrl.imgWidth == 1920){	//xian ++ 20091023, for pc used
		videoFillDramLong(addr + 30, (UINT32)0x300000); //SuggestedBufferSize
		videoFillDramLong(addr + 72, (UINT32)0x300000);
	}
	//audio		
		if( g_vidParam[VD_SLOW_REC_MOD] || g_vidParam[VD_SLOW_PLAY_RATE]){
		//streams, mask aud stream
		videoFillDramLong(addr+(0x38>>1), 0x7364756E);
		videoFillDramLong(addr+(0xD4>>1), 0x4B4E554A);	//junk
		videoFillDramLong(addr+(0xE8>>1), 0x7364756E);//nuds
		if(g_vidParam[VD_SLOW_PLAY_RATE]){
			frameRate = g_vidParam[VD_SLOW_PLAY_RATE];
		}else{
			frameRate = vidClipCtrl.frameRate;
		}
		tmp0 = 1000000 / frameRate;
		dwScale = 1000000 / frameRate;
		dwRate = 1000000;
		videoFillDramLong(addr + 70, idxSize/16);
		videoFillDramLong(addr + 24, idxSize/16);
		videoFillDramLong(addr + 64, dwScale);
		videoFillDramLong(addr + 66, dwRate);	
		videoFillDramLong(addr + 16, tmp0);
	}else{
		dwRate =vidClipCtrl.VidChunkCnt * 1000 ;
		if(vidClipCtrl.AudChunkCnt) {
			if(vidClipCtrl.audFmt == VID_ADPCMFMT) {			
				dwScale =  (UINT32)((float)vidClipCtrl.AudChunkCnt *(float)1017 /((float)vidClipCtrl.sampRate *(float)BLOCK_ALIGN/ ((float)CHUNKSIZE*1000)));
			}
			else {
				dwScale =  (UINT32)((float)vidClipCtrl.AudChunkCnt *(float)1000 /((float)vidClipCtrl.sampRate *(float)2/ (float)CHUNKSIZE));
			}
			frameRate = (float) dwRate/ (float) dwScale;
		}
		else {
			ASSERT(0, 1);
			dwScale = dwScale;
		}
		
		tmp0 = 1000000 / frameRate;
		videoFillDramLong(addr + 70, vidClipCtrl.VidChunkCnt);
		videoFillDramLong(addr + 24, vidClipCtrl.VidChunkCnt);
		videoFillDramLong(addr + 64, dwScale);
		videoFillDramLong(addr + 66, dwRate);	
		videoFillDramLong(addr + 16, tmp0);
		
		if(vidClipCtrl.audFmt == VID_ADPCMFMT) {
			UINT32 avgBytePerSec;
			UINT32 samplePerBlock;
			
			samplePerBlock = (BLOCK_ALIGN - 4 ) * 2 + 1;
			avgBytePerSec = (UINT32)vidClipCtrl.sampRate * BLOCK_ALIGN / SMPPERBLK;
			videoFillDramLong(addr + 158+8, hdrJunkSize-0x10);//hdr junk size
			videoFillDramLong(addr + 0x108/2, vidClipCtrl.AudChunkCnt*(CHUNKSIZE/0x200));
			videoFillDramLong(addr + 0xfc/2, samplePerBlock);
			videoFillDramLong(addr + 0x100/2, (UINT32)vidClipCtrl.sampRate);
			videoFillDramLong(addr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
			videoFillDramLong(addr + 0x130/2, avgBytePerSec);
			samplePerBlock =( (samplePerBlock & 0xffff) << 16 )| 0x02;
			videoFillDramLong(addr + 0x138/2, samplePerBlock);
		}
		else {		
			videoFillDramLong(addr + 0x108/2, (vidClipCtrl.AudChunkCnt <<SECPERCHK)/(SAMPLE_BIT/8));
			videoFillDramLong(addr + 0xfc/2, 1000);
			videoFillDramLong(addr + 0x100/2, (UINT32)vidClipCtrl.sampRate *1000);
			videoFillDramLong(addr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
			videoFillDramLong(addr + 0x130/2, (UINT32)vidClipCtrl.sampRate<<1);
		}
	}
	
	videoDebugAviInfo(addr);	

	//if(vidBgEn){
		firstSector0 = bgFileHdr.clusterFirst;
		firstSector1 = bgFileIdx.clusterFirst;
	//}else{
	//	firstSector0 = dos.file[vidClipCtrl.fHandle_hdr].FirstCluster;
	//	firstSector1 = dos.file[vidClipCtrl.fHandle_idx].FirstCluster;
	//}
	//printf("%lx,%lx\n",firstSector0,firstSector1);
	DOS_SectorWrite(dos.CurrDrv,FAT_Cluster2Sector(dos.CurrDrv,firstSector0), 1, addr);
	DOS_SectorWrite(dos.CurrDrv, FAT_Cluster2Sector(dos.CurrDrv,firstSector1), 1, addr);
	//dramFileSave("HDR3", addr, 0x200);
	//DOS_FileSeek(vidClipCtrl.fHandle_hdr, 0 ,0);
	//DOS_FileWrite(vidClipCtrl.fHandle_hdr, addr<<1, 512);
	//DOS_FileSeek(vidClipCtrl.fHandle_idx, 0 ,0);
	//DOS_FileWrite(vidClipCtrl.fHandle_idx, addr<<1, 512);
	return TRUE;
}
#else
/**
 * @fn		 void videoAbortSaveHeader()
 * @brief	  fill avi header
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-10-09
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoAbortModifyHeader()
{
	UINT32 dwScale = 0;
	UINT32 dwRate = 0;
	
	float frameRate = 0;
	UINT8 rwMode;
	UINT32 num=0;
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX);//vidClipCtrl.AVIHeadAddr;
	
//	UINT8 recMode;
	UINT32 AviSize;
	UINT32 tmp0 = vidClipCtrl.idxSize-512-8;//(vidClipCtrl.AudChunkCnt + vidClipCtrl.VidChunkCnt) << 4;
	rwMode = K_DOS_NewFile;
	//printf("idx:%lu,%lu+%lu=%lu\n",tmp0,vidClipCtrl.AudChunkCnt,vidClipCtrl.VidChunkCnt,(vidClipCtrl.AudChunkCnt + vidClipCtrl.VidChunkCnt) << 4);
	AviSize = (UINT32) (vidClipCtrl.SecAddr  << SECPERCHK) + vidClipCtrl.hdrSize + vidClipCtrl.idxSize - 8;	
	vidPrintfAbort("size:%lu=%lu+%lu+%lu\n",AviSize,(UINT32) (vidClipCtrl.SecAddr  << SECPERCHK) , vidClipCtrl.hdrSize , vidClipCtrl.idxSize);
	vidPrintfAbort("chunk:%lu=%lu+%lu\n",vidClipCtrl.totalChuck,vidClipCtrl.AudChunkCnt , vidClipCtrl.VidChunkCnt);
	DOS_FileRead(vidClipCtrl.fHandle_hdr, addr<<1, 1024, 0);
	
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
		ASSERT(0, 1);
		dwScale = dwScale;
		dwRate = vidClipCtrl.VidChunkCnt * 1000;
	}
	videoFillDramLong(addr + 8, 0x1E0 + vidClipCtrl.hdrSize - 512+12);//hdr size
	if(vidClipCtrl.audFmt == VID_ADPCMFMT)
		videoFillDramLong(addr + 158+8, 0xA4 + vidClipCtrl.hdrSize - 512+12);//junk size
	else
		videoFillDramLong(addr + 158, 0xB4 + vidClipCtrl.hdrSize - 512+12);//junk size
	videoFillDramLong(addr + 512/2+2, tmp0 );//index size

	if(!vidClipCtrl.AudChunkCnt) {
		videoFillDramLong(addr + 28, 1);
	}
	tmp0 = 1000000 / frameRate;
	videoFillDramLong(addr + 70, vidClipCtrl.VidChunkCnt);
	videoFillDramLong(addr + 24, vidClipCtrl.VidChunkCnt);
	videoFillDramLong(addr + 64, dwScale);
	videoFillDramLong(addr + 66, dwRate);
	videoFillDramLong(addr + 2, AviSize);
	videoFillDramLong(addr + 16, tmp0);

	if(vidClipCtrl.imgWidth == 1280){	//xian ++ 20091023, for pc used
		videoFillDramLong(addr + 30, (UINT32)0x300000); //SuggestedBufferSize
		videoFillDramLong(addr + 72, (UINT32)0x300000);
	}
    if(vidClipCtrl.imgWidth == 1920){	//xian ++ 20091023, for pc used
		videoFillDramLong(addr + 30, (UINT32)0x300000); //SuggestedBufferSize
		videoFillDramLong(addr + 72, (UINT32)0x300000);
	}
	if(vidClipCtrl.audFmt == VID_ADPCMFMT) {
		UINT32 avgBytePerSec;
		UINT32 samplePerBlock;

		samplePerBlock = (BLOCK_ALIGN - 4 ) * 2 + 1;
		avgBytePerSec = (UINT32)vidClipCtrl.sampRate * BLOCK_ALIGN / SMPPERBLK;

		videoFillDramLong(addr + 0x108/2, vidClipCtrl.AudChunkCnt*(CHUNKSIZE/0x200));
		videoFillDramLong(addr + 0xfc/2, samplePerBlock);
		videoFillDramLong(addr + 0x100/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(addr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(addr + 0x130/2, avgBytePerSec);
		samplePerBlock =( (samplePerBlock & 0xffff) << 16 )| 0x02;
		videoFillDramLong(addr + 0x138/2, samplePerBlock);
	}
	else {
		videoFillDramLong(addr + 0x108/2, (vidClipCtrl.AudChunkCnt <<SECPERCHK)/(SAMPLE_BIT/8));
		videoFillDramLong(addr + 0xfc/2, 1000);
		videoFillDramLong(addr + 0x100/2, (UINT32)vidClipCtrl.sampRate *1000);
		videoFillDramLong(addr + 0x12c/2, (UINT32)vidClipCtrl.sampRate);
		videoFillDramLong(addr + 0x130/2, (UINT32)vidClipCtrl.sampRate<<1);
	}
	videoDebugAviInfo(addr);
	DOS_FileSeek(vidClipCtrl.fHandle_hdr, 0 ,0);
	DOS_FileWrite(vidClipCtrl.fHandle_hdr, addr<<1, 1024);
	DOS_FileSeek(vidClipCtrl.fHandle_idx, 0 ,0);
	DOS_FileWrite(vidClipCtrl.fHandle_idx, addr<<1, 1024);
	return TRUE;
}
#endif
#if VideoNoUsed
UINT8 videoAbortModifyIndex()
{
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)<<1;
	UINT32 fsize = vidClipCtrl.idxSize;
	UINT32 size = fsize;
	UINT32 csize= 32ul*1024;

	vidPrintfAbort("%lx,%lx\n",vidClipCtrl.idxSize,vidClipCtrl.hdrSize);
	DOS_FileSeek(vidClipCtrl.fHandle_idx, 0 ,0);
	while(size){	
		if(size<csize)csize = size;
		DOS_FileRead(vidClipCtrl.fHandle_hdr, addr, csize, fsize-size);
		if(size == fsize){videoFillDramLong((addr>>1) + 2, fsize-8);}
		DOS_FileWrite(vidClipCtrl.fHandle_idx, addr, csize);
		size-=csize;
		//dumpData(csize, addr>>1, 1);
	}	
	return TRUE;
}
#endif
//-----------------------------------------------------------------------------
//videoRecKeyOnSnap
//-----------------------------------------------------------------------------
/**
 * @fn	void videoRecPvRegAddrUpdate(UINT8 frameBufNum)
 * @brief	  update 0x2780~0x278F
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoAbortPvRegAddrUpdate(UINT8 frameBufNum){

	HAL_Dram1stPathFrmBufAAddrSet(K_SDRAM_AviPvwBufAddrA);
	HAL_Dram1stPathFrmBufBAddrSet(K_SDRAM_AviPvwBufAddrB);
	if(frameBufNum==2){
		HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrA);
		HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_AviPvwBufAddrB);
	}
	else{
		HAL_Dram1stPathFrmBufCAddrSet(K_SDRAM_AviPvwBufAddrC);
		HAL_Dram1stPathFrmBufDAddrSet(K_SDRAM_AviPvwBufAddrD);
	}
	
}
/**
 * @fn		  void videoPvMemInit(void)
 * @brief	  video record dynamic mem initial
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-8-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoAbortInitPvMem(void){

	UINT8 size;
	UINT8 pvFrameBufNum;
	UINT8 vidRecFrameBufNum;
	UINT16 pvHsize,pvVsize;
	//UINT8 curVidRecFrameBufNum;

	size = vidRecSizeGet();
	pvFrameBufNum=vidRecFramebufModeGet();
	vidRecFrameBufNum = vidRecFrameBufNumGet();
	/* check video rec buffer number mode switch */
	if ( vidRecFrameBufNum != pvFrameBufNum ) {
		vidRecFrameBufNumSet(pvFrameBufNum);
		/* set front2cdps gating enable */
		HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

		/* disable frame ctrl */
		HAL_CdspFrmCtrlEnSet(0);	

		/* video preview addr Update */
		pvSizeGet(&pvHsize,&pvVsize);
		sysMemVideoPvAddrUpdate(size,pvHsize,pvVsize);

		/* video record addr Update */
		vidRecMemFrameBufAddrUpdate();
		videoAbortPvRegAddrUpdate(pvFrameBufNum);
		/* set frame buffer number 2 (1st path) */
		if(pvFrameBufNum==2){
			HAL_DramFrmBufNumSet(0);
		}
		else if(pvFrameBufNum==3){
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
#if VideoNoUsed
UINT8 videoAbortStampLast()
{
	UINT8 recMode;
	UINT8 cdspIdx, mjpgIdx, cdspIdxLast;
	UINT8 idx;
	//xian ++ 20081223
	g_bReserveStampFrameSize = 0;	//release reserved size for end frame of stamp
	recMode = videoRecStampRecModeGet();
	if(vidDateStampSts && (recMode == VD_STAMP_REC_MODE_END_FRAME || recMode == VD_STAMP_REC_MODE_FST_END_FRAME)){
		//save vlc buffer to file
		for (idx = 0; idx < G_VideoVlcBufNum; idx++)
			videoRec();

		//wait cdsp eof
		XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x02;
		while (!(XBYTE[REG_Dram_Cdsp1Eof_Sts] & 0x02));
		XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x02;
		cdspIdx = HAL_CdspWriteBufIdxGet();
		
		cdspIdxLast = cdspIdx > 0 ? cdspIdx - 1 : 3; //last finished

		HAL_GprmCopy(videoRecStampAddrGet(), g_vidDateStamp.stampW, g_vidDateStamp.stampH, 0, 0,
						dtnaddr[cdspIdxLast], g_vidDateStamp.imgW, g_vidDateStamp.imgH,
						g_vidDateStamp.disHOff, g_vidDateStamp.disVOff, g_vidDateStamp.stampW, g_vidDateStamp.stampH, 
						dsParam[SP1K_DS_PARAM_STAMP_PASTETHD], 1); //xian modify 20090522

		HAL_GprmCopy(videoRecStampAddrGet(), g_vidDateStamp.stampW, g_vidDateStamp.stampH, 0, 0,
						dtnaddr[cdspIdx], g_vidDateStamp.imgW, g_vidDateStamp.imgH,
						g_vidDateStamp.disHOff, g_vidDateStamp.disVOff, g_vidDateStamp.stampW, g_vidDateStamp.stampH, 
						dsParam[SP1K_DS_PARAM_STAMP_PASTETHD], 0); //xian modify 20090522

		//wait stamp to vlc
		mjpgIdx = XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] & 0x03;
		while (mjpgIdx != cdspIdxLast) {	//cdspIdxLast is finished, cdspIdx is doing
			mjpgIdx = XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] & 0x03;
		}

		//keep the stamp is the last frame
		HAL_AudStop( 0 );
		/*For HDMI beep Voice*/
		HAL_AudPause(); 
		XBYTE[REG_Audio_AudEn] &= 0x06;
		HAL_JpegMjpgAbort();

		for (idx = 0; idx < G_VideoVlcBufNum; idx++) {
		   videoRec();
		}
	}
	return TRUE;
}
#endif
void videoAbortWriteIndex()
{
	UINT32 size = vidIndexNum;	
	UINT32 size0 = vidIndexNum & (videoClusterSize-1);
	UINT32 addr = (vidClipCtrl.AVIIdxAddr<<1)-size;
	
	if(size0){
		size0 = videoClusterSize - size0;
	}
	else{
		size0 = videoClusterSize;
	}
	HAL_GprmDramFill(vidClipCtrl.AVIIdxAddr, size0, 0);
	videoRecWriteMovIDToDram((addr+size+size0-12)>>1);
	if(size){
		if(vidBgEn){	
			//bgFileWrite(&bgFileHdr, addr, size);
			//bgFileWrite(&bgFileIdx, addr, size);
			UINT32 rst0 ,rst1;
			rst0 = bgFileWrite(&bgFileHdr, addr, size+size0, 0);
			rst1 = bgFileWrite(&bgFileIdx, addr, size, 0);
			if((rst0<size)||(rst1<size))
			{	__PRINTF(__FILE,  __LINE__);
				//osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
				return;
			}		
		}else{
			DOS_FileWrite(vidClipCtrl.fHandle_hdr, addr, size+size0);
			DOS_FileWrite(vidClipCtrl.fHandle_idx, addr, size);
		}
		vidDbgDataFlow(5,4,size);
		vidIndexNum=0;
		vidClipCtrl.hdrSize+=size+size0;
		vidClipCtrl.idxSize+=size;
	}
	//printf("<%lu,%lu><%lu,%lu>\n",size,size0,vidClipCtrl.hdrSize,vidClipCtrl.idxSize);
}

//-----------------------------------------------------------------------------
//videoRecAbort
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoRecAbort(void)
 * @brief	  video clip abort process
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT8 videoRecAbort (void) USING_0
{
	UINT32 time0,time1;
	HAL_GlobalReadGTimer(&time0);
	//videoAbortStampLast();
	videoAbortMjpg();
	if(!videoAbortCheckAvi()){__PRINTF_POS();return FAIL;}
	//videoAbortWriteVlc();
	videoAbortWriteMovie();	
	videoAbortWriteIndex();
	videoAbortCloseFile();
	videoAbortModifyHeader();
	//videoAbortModifyIndex();
	videoAbortInitPvMem();
	
	HAL_GlobalReadGTimer(&time1);
	printf("Abort:%lu\n",time1-time0);
	//vidDbgDataFlowSave(vidClipCtrl.name_hdr);
	if(vidBgEn){
		FAT_TableWriteBack(dos.CurrDrv);
		FAT_CopyTable(dos.CurrDrv);
	}

	if(g_vidParam[VD_MMS]){
		pbAviMMsFun(vidClipCtrl.name_hdr, vidClipCtrl.imgWidth,vidClipCtrl.imgHight);
	}
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	osMsgPost(SP1K_MSG_MEDIA_REC_OK);
	
	return SUCCESS;
}
UINT8 videoRecAbortSeamless (void) USING_0
{
	UINT32 time0,time1;
	HAL_GlobalReadGTimer(&time0);
	//videoAbortStampLast();
	//videoAbortMjpg();
	//if(!videoAbortCheckAvi())return FAIL;
	//videoAbortWriteVlc();
	videoAbortWriteMovie();
	videoAbortWriteIndex();			
	videoAbortCloseFile();
	videoAbortModifyHeader();
	//videoAbortModifyIndex();
	//videoAbortInitPvMem();

	HAL_GlobalReadGTimer(&time1);
	//printf("slAbort:%lu\n",time1-time0);
	return SUCCESS;
}
