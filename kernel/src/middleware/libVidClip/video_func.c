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
#define __FILE __FILE_ID_VIDEO_FUNC__
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
UINT8 videoRegVlcIntEn,videoRegJpgEn; 
UINT32 videoRecDbgTime;
UINT8 vidTimeCheckEn;
vidRecDbgTimeCheck_t vidRecDbgTimeCtrl;
//void vidTimePosPrintf();
void vidTimePosInit();

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//videoRecEofIntrEnable
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecEofIntrEnable(void)
 * @brief	  Enable video compression completed interrupt
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecEofIntrEnable(void)
{
//	UINT8 recMode;
	
	if(vidRecFrameBufNumGet()==2){
		HAL_DramFrmBufNumSet(0);
	}
	else if(vidRecFrameBufNumGet()==3){
		HAL_DramFrmBufNumSet(1);
	}
	else{
		HAL_DramFrmBufNumSet(2);
	}
	if(vidDateStampSts) {//xian ++ 20081103
		videoStampSet();
		#if 0//wangjg++
		#if VIDREC_JPEG_EOF_EN
		XBYTE[REG_Jpeg_DisGatedClk]|= 0x08;
		#endif
		recMode = videoRecStampRecModeGet();
		if(recMode == VD_STAMP_REC_MODE_ALL_FRAME){//xian ++ 20081223
			videoRecStampEnable(1, 1);
		}
		else{
			pvPhofrmInit();
		}

		if(recMode != VD_STAMP_REC_MODE_END_FRAME){
			//cdspIdx = XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03;
			#if 0
			cdspIdx = HAL_CdspWriteBufIdxGet();
			HAL_GprmCopy(videoRecStampAddrGet(), g_vidDateStamp.stampW, g_vidDateStamp.stampH, 0, 0,
							dtnaddr[cdspIdx], g_vidDateStamp.imgW, g_vidDateStamp.imgH,
							g_vidDateStamp.disHOff, g_vidDateStamp.disVOff, g_vidDateStamp.stampW, g_vidDateStamp.stampH, 
							dsParam[SP1K_DS_PARAM_STAMP_PASTETHD]/*0*//*0x9F*/, 0); //xian modify 20090522

			cdspIdx = cdspIdx > 0 ? cdspIdx - 1 : 3;
			HAL_GprmCopy(videoRecStampAddrGet(), g_vidDateStamp.stampW, g_vidDateStamp.stampH, 0, 0,
							dtnaddr[cdspIdx], g_vidDateStamp.imgW, g_vidDateStamp.imgH,
							g_vidDateStamp.disHOff, g_vidDateStamp.disVOff, g_vidDateStamp.stampW, g_vidDateStamp.stampH, 
							dsParam[SP1K_DS_PARAM_STAMP_PASTETHD], 0); //xian modify 20090522
			#endif
			videoStampSet();//wangrf++
		}
		#endif
	}

	XBYTE[REG_Jpeg_Intr_Event] |= 0x0a; /*clear VLC overflow interrupt*/ //fixed #41993	
	XBYTE[REG_Dram_VlcRdy]|=0x03;//clear
	#if !VIDREC_JPEG_EOF_EN
	XBYTE[REG_Dram_VlcIntEn] |= 0x03;
	XBYTE[REG_Jpeg_Intr_Enable]&=~0x02; //rf++
	#else
	XBYTE[REG_Dram_VlcIntEn] &= ~0x03;
	XBYTE[REG_Jpeg_Intr_Enable]|=0x02; //rf++
	//printf("+++=%bx\n",XBYTE[REG_Jpeg_Intr_Enable]);
	#endif

	videoRegVlcIntEn = XBYTE[REG_Dram_VlcIntEn]&0x03;	
	videoRegJpgEn=XBYTE[REG_Jpeg_Intr_Enable]&0x02;
	//printf("IntrEn:%bx,%bx\n",videoRegVlcIntEn,videoRegJpgEn);
}

//-----------------------------------------------------------------------------
//videoRecEofIntrDisable
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecEofIntrDisable(void)
 * @brief	  Disable video compression completed interrupt
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecEofIntrDisable(void)
{
	XBYTE[REG_Dram_VlcIntEn] &= ~0x03;
	XBYTE[REG_Jpeg_Intr_Enable] &= ~0x02;
	videoRegVlcIntEn = XBYTE[REG_Dram_VlcIntEn]&0x03;
	videoRegJpgEn=XBYTE[REG_Jpeg_Intr_Enable]&0x02;
	
	if(vidDateStampSts) //xian ++ 20081103
	{
	  #if VIDREC_JPEG_EOF_EN
	  XBYTE[REG_Jpeg_DisGatedClk] &= ~0x08;
	  #endif	  
	  //videoRecStampEnable(0, 0);//wangjg++
	  XBYTE[REG_Jpeg_StampEn]=0x00;
	}
}

//-----------------------------------------------------------------------------
//videoFillDramLong
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoFillDramLong(UINT32 address, UINT32 value)
 * @brief		write 4 bytes to sdram
 * @param	address 	:	sdram address
 * @param	value	:	long int data
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void videoFillDramLong(UINT32 address, UINT32 value) USING_0
{
	HAL_DramStartAddrSet(address, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite((UINT8)value,(UINT8)(value>>8));
	HAL_DramWordDataWrite((UINT8)(value>>16),(UINT8)(value>>24));
}
void videoRecAddDumy_Size0(UINT32 size,UINT8 cnt)
{
	UINT32 tmp0,tmp1;
	UINT8 i;
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)+(vid_ByteAddr>>1);
	
	HAL_DramSetAddr(addr,0);
	for(i=0;i<cnt;i++){
		tmp0=0x63643030;						
		tmp1=0x00000000;		
		HAL_DramWrite32(tmp0);
		HAL_DramWrite32(tmp1);

		videoRecWriteVidIndex2Dram(0,1);
	}
	tmp0 = 8ul*cnt;
	vid_ByteAddr += tmp0;
	vid_TmpNum += tmp0;
	size =size;
}

//-----------------------------------------------------------------------------
//videoRecWriteVidIndex2Dram
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecWriteVidIndex2Dram(UINT32 size)
 * @brief		write video index to dram
 * @param	size			:	image frame size
 * @param	AVIIdxAddr	:	AVI index address
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void videoRecWriteVidIndex2Dram(UINT32 size,UINT8 isUpdateMovi)
{
	UINT32 pt[4] ; 	
	UINT32 MoviOfset = vidClipCtrl.MoviOfset;
	pt[0]=0x63643030;						
	pt[1]=0x00000010;
	pt[2] = MoviOfset;
	pt[3] = size;
	HAL_DramWriteBytesAddr(vidClipCtrl.AVIIdxAddr, (UINT8*)pt, 16, 2);
	vidClipCtrl.AVIIdxAddr+=8;
	vidIndexNum += 16;
	//videoRecIdx++;
	vidClipCtrl.VidChunkCnt ++;
	vidClipCtrl.totalChuck ++;
	if(isUpdateMovi){
		vidClipCtrl.MoviOfset += size + 8;
	}
}
/*
void videoRecWriteVidIndex2Dram(UINT32 MoviOfset, UINT32 size , UINT32 AVIIdxAddr) USING_0
{
#if 1
	UINT32 pt[16] ; 
	pt[0]=0x63643030;						
	pt[1]=0x00000010;
	pt[2] = MoviOfset;
	pt[3] = size;
	HAL_DramWriteBytesAddr(AVIIdxAddr, (UINT8*)pt, 16, 2);
#else
	HAL_DramStartAddrSet(AVIIdxAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x30);
	HAL_DramWordDataWrite(0x64,0x63);
	HAL_DramWordDataWrite(0x10,0x00);
	HAL_DramWordDataWrite(0x00,0x00);

	HAL_DramWordDataWrite(READ8(MoviOfset, 3),READ8(MoviOfset, 2));
	HAL_DramWordDataWrite(READ8(MoviOfset, 1),READ8(MoviOfset, 0));
	HAL_DramWordDataWrite(READ8(size, 3),READ8(size, 2));
	HAL_DramWordDataWrite(READ8(size, 1),READ8(size, 0));
#endif

	vidIndexNum += 16;

}
*/
 //For first file cluster alignment
/**
 * @fn		  void videoRecAddJunk(UINT32 addr, UINT32 size)
 * @brief	  video Record Add Junk
 * @param	  [in] addrWord : Junk word address
 * @param	  [in] size : junk size
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAddJunk(UINT32 addrWord, UINT32 size)
{
	UINT32 Addr;
	Addr = addrWord;
	videoFillDramLong(Addr, 0x4B4E554A);
//	Addr += 1;
//	videoFillDramLong(Addr, 0x4B4E);
	Addr += 2;
	videoFillDramLong(Addr, size -8);
	Addr += 2;
	HAL_GprmDramFill(Addr, size, 0);
}
//wangjg++20110920
UINT8 videoRecWriteToFragBuf(UINT32 addrbyte,UINT32 size)
{
	UINT32 bufSize = vidFragBufSize;
	UINT32 dataAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)<<1;
	UINT32 dataOffset = vid_ByteAddr;	
	UINT32 dataSize = vid_TmpNum;//+saveSize;
	//UINT32 freeSize=bufSize-dataSize;
	UINT32 saveSize = 0;

	if(vidBgEn){saveSize = bgSaveSizeGet();}
	//if(saveSize>bufSize){__ASSERT_POS(0);}
	
	if(size+saveSize+dataSize<=bufSize ){//enough buf for vlc
		if((dataOffset+size)>(bufSize+vidVlcBufSize)){return FALSE;}
		//printf("%lu,%lu+%lu=%lu,%lu\n",addrbyte,dataAddr,dataOffset,dataAddr+dataOffset,size);
		HAL_GprmDramDmaAdv(addrbyte, dataAddr+dataOffset, size, 0);	
		if((dataOffset+size)>(bufSize+vidVlcBufSize)){
			ASSERT(0, 1);
		}
		//__PRINTF_POS();
		vid_ByteAddr = dataOffset+size;
		vid_TmpNum = dataSize+size;
		return TRUE;
	}else{
		//putchar('G');
		//__PRINTF_POS();
		//printf("(%lu+%lu+%lu>%lu)\n",size,dataSize,saveSize,bufSize);
		return FALSE;
	}

}
UINT8 videoRecFragSaveCard(UINT32 minSize)
{
	UINT32 bufSize = vidFragBufSize;
	UINT32 dataAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)<<1;
	UINT32 dataOffset = vid_ByteAddr;
	UINT32 dataSize = vid_TmpNum;

	//if(dataOffset<dataSize){__ASSERT(__FILE,  __LINE__,0);}
	if(dataSize>=minSize){
		UINT32 addr = dataAddr + dataOffset - dataSize;
		UINT32 size = dataSize;// = dataSize & 0xffff8000;
		UINT32 dsize;		
		if(minSize){size -= dataSize%(_MAX(videoClusterSize,minSize));}
		if(!size){__PRINTF_POS();return TRUE;}
		if(!videoRecDma0Wait(3)){__PRINTF_POS();}
		if(vidBgEn){
			size = bgFileWrite(&bgFileMov, addr, size, 0);
		}else{
			size = DOS_FileWrite(vidClipCtrl.fHandle, addr, size);
		}
		if(!size){	__PRINTF_POS();	return FALSE;}
		if(dataSize<size){	__PRINTF_POS();	return FALSE;}
		vidDbgDataFlow(3,4,size);
		dsize = dataSize-size;
		if(dataOffset>=bufSize){
			if(dataSize>size){
				if(!videoRecDma0Wait(3)){__PRINTF_POS();}
				HAL_GprmDramDmaAdv(addr+size, dataAddr, dsize, 1);
			}			
			vid_ByteAddr = dsize;
		}		
		vid_TmpNum = dsize;
		vidClipCtrl.SecAddr  +=  size>>SECPERCHK;
	}	
	else{
		//putchar('C');
		//__PRINTF_POS();
		//printf("%lu<%lu,%lu\n",dataSize,minSize,bgSaveSizeGet());
	}
	return TRUE;
}
void videoDebugFrameInfo(UINT32 addr)
{
	UINT8 vlcIndex = idxVlcHead;
	extern UINT8 g_brcVlcQIndexBuf[];
	extern UINT8 videoQValueTab[];

	UINT8 qval = videoQValueTab[g_brcVlcQIndexBuf[vlcIndex]];	
	HAL_GlobalTimerRead10us(&videoRecDbgTime);
	
	HAL_DramStartAddrSet(addr, K_SDRAM_NoPrefetch );
	//HAL_DramWriteBytes(vidTimeSkip, 8, 1);	
	HAL_DramWordDataWrite(READ8(videoRecDbgTime, 0),READ8(videoRecDbgTime, 1));
	HAL_DramWordDataWrite(READ8(videoRecDbgTime, 2),READ8(videoRecDbgTime, 3));
	HAL_DramWordDataWrite(READ8(vlcBufFrameCnt, 0),READ8(vlcBufFrameCnt, 1));
	HAL_DramWordDataWrite(g_vlcIdxTab[vlcIndex].dumyCnt,g_dumyCopy);
	HAL_DramWordDataWrite(qval,vlcIndex);
	if(QIdxMax<qval)QIdxMax = qval;
	if(QIdxMin>qval)QIdxMin = qval;

#if (VLC_AE_INFO_EN == 1)
	{
	UINT8 aeTabIdx, curLuma, curContrast;
	UINT16 yuvHSize,yuvVSize;
	aeAwbInfo(&aeTabIdx, &curLuma, &curContrast,&curContrast);	
	HAL_DramWordDataWrite(aeTabIdx, curLuma);
	}
#endif
}

//-----------------------------------------------------------------------------
//videoDebugInfo2AVIHeader
//-----------------------------------------------------------------------------
/**
 * @fn		 void videoDebugInfo2AVIHeader(void)
 * @brief	  write AVI debug info to avi header when video rec abort
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  rf.wang
 * @since	  2011-10-09
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoDebugAviInfo(UINT32 addr)
{
	UINT8 Reg22B8,Reg22B9,Reg22BA,Reg22BB;
	UINT8 Reg281A,Reg281B,Reg2818,Reg2819,Reg2860,Reg2861,Reg2862,Reg2863;
	UINT8 Reg21C8,Reg21CC,Reg21CD,Reg21CE,Reg21CF;
	//UINT32 FRAME_CNT;
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
	//UINT16 FallFrmNum;
	//FRAME_CNT = vidVlcDMACnt + 1;
	videoFillDramLong(addr+160, vidClipCtrl.totalChuck);//FRAME_CNT);
	videoFillDramLong(addr+162, DummyFrameCnt);
	videoFillDramLong(addr+164, vlcOverflowCnt);
	videoFillDramLong(addr+166, vidStorageSpeed);

	videoFillDramLong(addr+168, (UINT32)QIdxMax);
	videoFillDramLong(addr+170, (UINT32)QIdxMin);

	videoFillDramLong(addr+172, (UINT32)dumyIdxTotal);

	videoFillDramLong(addr+174, (UINT32)vidBgEn);

	HAL_DramStartAddrSet(addr + 176, K_SDRAM_NoPrefetch );
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
/**
 * @fn		  void videoRecWriteMovIDToDram(UINT32 movAddr)
 * @brief	  video Record Write Movie ID To Dram
 * @param	  [in] movAddrWord : movie ID word start address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecWriteMovIDToDram(UINT32 movAddrWord)
{
	UINT32 vals[3];
	vals[0] = 0x5453494c;
	vals[1] = (UINT32) (vidClipCtrl.SecAddr << SECPERCHK)+4 + 512-8;
	vals[2] = 0x69766f6d;
	HAL_DramWriteBytesAddr(movAddrWord, vals, 12, 2);
	/*
	0x4C, 0x49, 0x53, 0x54,    //-Id = "LIST"
	0x38, 0xA0, 0x06, 0x00,    //-Size = 0x????????
	0x6D, 0x6F, 0x76, 0x69,   //movi
	*/

	//HAL_DramStartAddrSet(movAddrWord, K_SDRAM_NoPrefetch );
	//HAL_DramWordDataWrite(0x4C,0x49);
	//HAL_DramWordDataWrite(0x53,0x54);
	//HAL_DramWordDataWrite(0x38,0xA0);
	//HAL_DramWordDataWrite(0x06,0x00);
	//HAL_DramWordDataWrite(0x6D,0x6F);
	//HAL_DramWordDataWrite(0x76,0x69);

	//vid_ByteAddr = 12;
	//vid_TmpNum = 12;
}


UINT32 vidTimePosstart,vidTimePosLast;

void vidTimePosInit()
{
	UINT32 time;	
	HAL_GlobalReadGTimer(&time);
	//vidTimePosstart=0;
	vidTimePosLast=time;
}

//wangjg debug
#if VIDEO_REC_TIMER
UINT16 vidTimeSkip[10];
UINT32 vidTimeSum[10];
//UINT32 vidIntr0Cnt,vidTime0Cnt;
UINT32 vidTimeCnt;
UINT32 vidTimeStart,vidTimeLast;
UINT8 vidTimeIdx;


void vidDbgTimerReset()
{
	UINT32 gtimer;
	HAL_GlobalReadGTimer(&gtimer);
	vidTimeStart=gtimer;
	vidTimeLast = gtimer;
	memset(vidTimeSum,0,40);		
	vidTimeCnt = 0;	
}
void vidDbgTimerPos(UINT8 frameStart)
{
	UINT32 gtimer;
	if(frameStart){
		vidTimeIdx=0;
		vidTimeCnt++;
	}else{
		vidTimeIdx++;
	}
	HAL_GlobalReadGTimer(&gtimer);	
	vidTimeSkip[vidTimeIdx] = gtimer-vidTimeLast;
	vidTimeSum[vidTimeIdx] += vidTimeSkip[vidTimeIdx];		
	vidTimeLast = gtimer;
	//printf("vidTimeIdx%bu\n",vidTimeIdx);
	
}
void vidDbgTimerPrintf()
{
	UINT8 i;
	//printf("\n\n%lu-%lu=%lu,%lu,%lu,%lu\n",vidTimeLast,vidTimeStart,vidTimeLast-vidTimeStart,vidIntr0Cnt,vidTime0Cnt,vidTimeCnt);
	//printf("%lu,%lu,%lu,%lu\n",vidTimeSkip[0],vidTimeSkip[1],vidTimeSkip[2],vidTimeSkip[3]);
	//printf("%lu,%lu,%lu,%lu\n\n",vidTimeSum[0]/vidTimeCnt,vidTimeSum[1]/vidTimeCnt,vidTimeSum[2]/vidTimeCnt,vidTimeSum[3]/vidTimeCnt);
	UINT32 time = vidTimeLast-vidTimeStart;
	for(i=0;i<4;i++){
		printf("%lu,",vidTimeSum[i]*1024/time);		
	}
	printf("%lu-%lu\n",time,vidTimeCnt);

}
#endif
#define __vidDbgDataFlow__ 0
UINT32 vidDbgDataFlowAddr0;
UINT32 vidDbgDataFlowSize0,vidDbgDataFlowOffset;
typedef struct{	
	UINT32 time;
	UINT32 size;
	UINT8 src;
	UINT8 dst;	
	UINT16 fragIn;
	UINT16 fragOut;
	UINT16 res;
}video_deb_dataflow_t;

//0:vlc, 1:audio, 2:junk, 3:frag, 4:sd, 5:index,0xff:none
void vidDbgDataFlow(UINT8 src,UINT8 dst,UINT32 size)
{
	#if __vidDbgDataFlow__
	UINT32 t0;
	video_deb_dataflow_t flow;
	UINT32 addr = (vidDbgDataFlowAddr0+vidDbgDataFlowOffset)>>1;
	UINT32 size0 = sizeof(video_deb_dataflow_t);

	HAL_GlobalTimerRead10us(&t0);	
	//*((UINT8 xdata*)t0 + 3) = XBYTE[0x20C1];
	//*((UINT8 xdata*)t0 + 2) = XBYTE[0x20C2];
	//*((UINT8 xdata*)t0 + 1) = XBYTE[0x20C3];
	//*((UINT8 xdata*)t0 + 0) = XBYTE[0x20C4];
	flow.src = src;
	flow.dst = dst;
	flow.size = size;
	flow.time = t0;
	
	if(vidDbgDataFlowOffset+size0>vidDbgDataFlowSize0){
		//vidDbgDataFlowOffset = 0;
		return;
	}

	HAL_DramWriteBytesAddr(addr, &flow,size0 , 0);
	vidDbgDataFlowOffset+=size0;
	#else
	return;
	src = src;dst=dst;size=size;
	#endif
}

void vidDbgDataFlowInit()
{
	vidDbgDataFlowAddr0 = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)*2+FRAG_SIZE_IDX*3;
	vidDbgDataFlowSize0	= vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_IDX)*2-FRAG_SIZE_IDX*3;
	vidDbgDataFlowOffset = 0;
	//printf("dbg:%lx,%lx\n",vidDbgDataFlowAddr0,vidDbgDataFlowSize0);
}
void vidDbgDataFlowSave(UINT8 fname[] )
{
	#if __vidDbgDataFlow__
	fname[8]='A';
	fname[9]='A';
	fname[10]='A';
	fname[11]=0;
	dramFileSave(fname, vidDbgDataFlowAddr0/2, vidDbgDataFlowOffset);
	#else
	return;
	fname= fname;
	#endif
}
void videoTest(UINT8* pt)
{
	UINT32 arg[8];	
	cmdArgGet(&pt, arg, 8, 1);
	if(strncmp(pt, "bg", 2) == 0){
		vidBgEnDef= arg[1];
	}else if(strncmp(pt, "sl", 2) == 0){	
		seamLessLpRecEn = arg[1];
		if(arg[2])videoSeamlessLPRecTimeSet(arg[2]);
		if(arg[3])videoSeamlessLPRecFilesizeSet(arg[3]);

		if(arg[4])videoRecParamSet(VD_SLREC_RenameMode, arg[4]);
		printf("slrec:%bu,%lu,0x%lx,%u\n",seamLessLpRecEn,videoSeamlessRecTime,videoSeamlessFilesize,videoRecParamGet(VD_SLREC_RenameMode));
	/*}else if(strncmp(pt, "if", 2) == 0){
		vidTestCnt = arg[1];
	}else if(strncmp(pt, "fm", 2) == 0){
		vidFileMaxSize = arg[1];
	*/	
#if 0	
	}else if(strncmp(pt, "rt", 2) == 0){
		//HAL_GlobalAlarmRTCIntrSet_Intr();
		//Hal_GlobalRTCSecondIntrSet(1);
		printf("rtc intr:%bu\n",arg[1]);

	}else if(strncmp(pt,"st",2)==0){
		stampDateTimeFmtSet(arg[1],arg[2]);
	}else{
		extern UINT32 intrTime;
		extern UINT32 bgIntrTime;
		printf("%lu-%lu\n",intrTime,bgIntrTime);
		intrTime = 0;
		bgIntrTime = 0;
#endif
	}else{
		//printf("vlc(%lu,%bu)(%lu,%lu,%lu,%lu)\n",vidVlcBufSize,G_VideoVlcBufNum,slRecTime,videoSeamlessRecTime,vidClipCtrl.AudChunkCnt,audBytesPerSec);
		DISK xdata* disk = dos.disk+dos.CurrDrv;
		//printf("free=%lxKB\n",DOS_GetDiskSpace());
		printf("clust(%lx,%lx)\n",disk->FAT_FirstFreeCluster,disk->FreeCluster);
		printf("sl%lu(%lu,%lu)\n",vidSLDbgTime,vidSLDbgVlcCnt0,vidSLDbgVlcCnt1);
		bgPrintfNodes(&bgFileMov);
		bgPrintfNodes(&bgFileHdr);
		bgPrintfNodes(&bgFileIdx);
	}
}
UINT8 vidTimeCheckDbgEnd(UINT32 timeLimte,UINT8 *str,UINT32 *timerecord)
{	
	UINT32 costTime;
	UINT8 ret=SUCCESS;
	if(vidTimeCheckEn){
		HAL_GlobalReadGTimer(&vidRecDbgTimeCtrl.vidTimeCheckCurr);
		//costTime=vidRecDbgTimeCtrl.vidTimeCheckCurr-vidRecDbgTimeCtrl.vidTimeCheckLast;
		costTime=vidRecDbgTimeCtrl.vidTimeCheckCurr+(UINT32)(~vidRecDbgTimeCtrl.vidTimeCheckLast)+1;
		*timerecord+=costTime;
		if(costTime>timeLimte)
		{
			printf("\n@%s:",str);
			printf("%ld\n",costTime);
			ASSERT(0,1);
		}
	}
	return ret;

}
void vidTimeCheckDbgStart(){
	if(vidTimeCheckEn){
		HAL_GlobalReadGTimer(&vidRecDbgTimeCtrl.vidTimeCheckLast);
	}
}
void vidTimeSliceCheck(UINT8 flag){
	UINT32 videoFramCnt;
	UINT32 timecheck=(UINT32)120;//uint in s
	if(vidTimeCheckEn){
		if(flag==1){
			vidTimeCheckEn=vidClipCtrl.time/1000;
		}
		sp1kVdAvgTimeSvcProc();
		if(((vidClipCtrl.time-vidRecDbgTimeCtrl.vidTimeCheckSliceLast)>=(timecheck*1000))|| (flag)){
			videoFramCnt=vidClipCtrl.VidChunkCnt+vlcBufFrameCnt;
			printf("@osd=%ld\n",vidRecDbgTimeCtrl.vidTimeCheckOsd);
			printf("@Host=%ld\n",vidRecDbgTimeCtrl.vidTimeCheckHost);	
			printf("@Card=%ld\n",vidRecDbgTimeCtrl.vidTimeCheckCard);
			if((vidRecDbgTimeCtrl.vidTimeCheckOsd>=(timecheck*2*15))){
				ASSERT(0, 1);
			}
			if(vidRecDbgTimeCtrl.vidTimeCheckCard>=(vidClipCtrl.VidChunkCnt*25)){
				
				ASSERT(0, 1);
			}
			if(vidRecDbgTimeCtrl.vidTimeCheckHost>=(timecheck*vidClipCtrl.frameRate*15)){
				ASSERT(0, 1);
			}
			vidRecDbgTimeCtrl.vidTimeCheckHost=0;
			vidRecDbgTimeCtrl.vidTimeCheckOsd=0;
			vidRecDbgTimeCtrl.vidTimeCheckCard=0;
			vidRecDbgTimeCtrl.vidTimeCheckSliceLast=vidClipCtrl.time;
			
		}
	}
}