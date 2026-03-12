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
 * @file		video_recaud.c
 * @brief		middle ware  API for audio record in Video record module
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
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 g_bNoAudio = 0; //xian ++ 20081211
UINT32 vid_AudOutIdx;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

 //-----------------------------------------------------------------------------
 //videoRecAudDataProcess
 //-----------------------------------------------------------------------------
 /**
  * @fn 	   void videoRecAudDataProcess(UINT8 flg)
  * @brief	   audio data process
  * @param	   [in] flg
  * @retval    NONE
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-10
  * @todo	   N/A
  * @bug	   N/A
 */

void videoRecAudDataProcess(UINT8 flg)
{
	UINT32 audleftsize,audChunkCnt,audDataCnt;
	UINT32 i;
	UINT32 vlcCnt;
	UINT32 audOutIdx;

	HAL_AudRingBufCntRead( &audDataCnt);
	
	//ENTER_CRITICAL( status );
	_VIDEO_INTR_DISABLE();
	vlcCnt = vlcBufFrameCnt;
	_VIDEO_INTR_ENABLE();
	//audleftsize=msPerFrm *vlcCnt*vidClipCtrl.sampRate*2/1000;
	audleftsize = vlcCnt*vidClipCtrl.sampRate*2/g_vidAviFrameRate;
	if(audDataCnt>audleftsize){
		audDataCnt-=audleftsize;		
	}else{
		audDataCnt = 0;
	}
	audChunkCnt=audDataCnt>>SECPERCHK;
	
	//for(i = vidClipCtrl.DRAMAudBufCnt; i >0; i--) {
	//if(vidClipCtrl.DRAMAudBufCnt){
	for(i = audChunkCnt; i >0; i--) {
	//if(audChunkCnt){
		//videoRecWriteAudID2Dram(vidClipCtrl.AudioAddr+ (vid_AudOutIdx>>1)-4);
		//if(!videoRecWriteToFragBuf((vidClipCtrl.AudioAddr<<1) + vid_AudOutIdx-8, CHUNKSIZE+8)){return;}
		HAL_AudOutIndexGet(&audOutIdx);
		if(g_vidParam[VD_SLOW_REC_MOD] || g_vidParam[VD_SLOW_PLAY_RATE]){
			vidClipCtrl.AudChunkCnt++;
		}else {
			videoRecWriteAudID2Dram(vidClipCtrl.CurDMATagAddr + (vid_ByteAddr>>1));
			vid_ByteAddr += 8;
			vid_TmpNum  += 8;
			if (!videoRecWriteToFragBuf((vidClipCtrl.AudioAddr<<1) + audOutIdx, CHUNKSIZE)){
				vid_ByteAddr -= 8;
				vid_TmpNum  -= 8;
				//putchar('A');
				return;
			}
		}
		vidDbgDataFlow(1,3,CHUNKSIZE);
		audOutIdx += CHUNKSIZE;
		if(audOutIdx>=VIDAUDBUFSIZE){
			audOutIdx = (audOutIdx & (VIDAUDBUFSIZE-1)) ;
		}
		HAL_AudOutIndexSet( audOutIdx );
		if(!(g_vidParam[VD_SLOW_REC_MOD] || g_vidParam[VD_SLOW_PLAY_RATE])){
			videoRecWriteAudIndex2Dram(CHUNKSIZE);//vidClipCtrl.MoviOfset);
		}	
	//}
	}
	XBYTE[REG_Audio_DataThdIntSts] = 0x01;	//xian ++ 20100309
	flg = flg;
}

//-----------------------------------------------------------------------------
//videoRecAudFmtSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecAudFmtSet(UINT8 fmt)
 * @brief	  video record audio format set
 * @param	  [in] fmt : '0' PCM, '1' ADPCM
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAudFmtSet(UINT8 fmt)
{
	if(fmt == 0x00) {
		vidClipCtrl.audFmt = VID_PCMFMT;
	}
	else {
		vidClipCtrl.audFmt = VID_ADPCMFMT;
	}
}

/**
 * @fn		  UINT8 videoRecAudioStsGet(void)
 * @brief	  get audio status
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecAudioStsGet(void)
{
	return !g_bNoAudio;
}

/**
 * @fn		  void videoRecAudioStsSet(UINT8 en)
 * @brief	  set audio status
 * @param	  [in] en
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAudioStsSet(UINT8 en)
{
	g_bNoAudio = !en;
}


//-----------------------------------------------------------------------------
//videoRecAudSampleRateGet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecAudSampleRateGet(UINT16* rt)
 * @brief	  Get audio data format
 * @param	  [in] rt : output sample rate value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAudSampleRateGet(UINT16* rt)
{
	*rt = vidClipCtrl.sampRate;
}

//-----------------------------------------------------------------------------
//videoRecAudSampleRateSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecAudSampleRateSet(UINT16 rt)
 * @brief	  sample rate value
 * @param	  [in] rt : sample rate
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAudSampleRateSet(UINT16 rt)
{
	if(rt==48000 || rt==44100 ||rt==32000 ||rt==22050 ||
		rt==16000 ||rt==11025 ||rt==8000 ||rt==12000 || rt==24000 ){
		vidClipCtrl.sampRate = rt;
		return;
	}else {
		vidClipCtrl.sampRate = 44100;
	}
}

//-----------------------------------------------------------------------------
//videoRecWriteAudID2Dram
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecWriteAudID2Dram(UINT32 addr)
 * @brief	  write audio ID to sdram
 * @param	  [in] addr : ID addr
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecWriteAudID2Dram(UINT32 addr) USING_0
{
	UINT32 size;

	size = CHUNKSIZE;
	HAL_DramStartAddrSet(addr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x31);
	HAL_DramWordDataWrite(0x77,0x62);
	HAL_DramWordDataWrite(READ8(size,3),READ8(size,2));
	HAL_DramWordDataWrite(READ8(size,1),READ8(size,0));
}

//-----------------------------------------------------------------------------
//videoRecWriteAudIndex2Dram
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecWriteAudIndex2Dram(UINT32 MoviOfset, UINT32 AVIIdxAddr)
 * @brief		write audio index to sdram
 * @param	MoviOfset	:	current movie data offset
 * @param	AVIIdxAddr	:	AVI index address
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
#if 1
void videoRecWriteAudIndex2Dram(UINT32 chunkSize)//UINT32 MoviOfset)//, UINT32 AVIIdxAddr) USING_0
{
	UINT32 AVIIdxAddr=vidClipCtrl.AVIIdxAddr;
	UINT32 MoviOfset = vidClipCtrl.MoviOfset;
	UINT32 size = chunkSize;
	//size = CHUNKSIZE;

	HAL_DramStartAddrSet(AVIIdxAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x31);
	HAL_DramWordDataWrite(0x77,0x62);
	HAL_DramWordDataWrite(0x00,0x00);
	HAL_DramWordDataWrite(0x00,0x00);
	HAL_DramWordDataWrite(READ8(MoviOfset, 3),READ8(MoviOfset, 2));
	HAL_DramWordDataWrite(READ8(MoviOfset, 1),READ8(MoviOfset, 0));
	HAL_DramWordDataWrite(READ8(size,3),READ8(size,2));
	HAL_DramWordDataWrite(READ8(size,1),READ8(size,0));
	vidClipCtrl.AVIIdxAddr += 8;
	vidIndexNum += 16;

	vidClipCtrl.MoviOfset += 8+chunkSize;
	vidClipCtrl.AudChunkCnt++;
	vidClipCtrl.totalChuck++;
}
 #else
void videoRecWriteAudIndex2Dram(UINT32 MoviOfset, UINT32 AVIIdxAddr) USING_0
{
	UINT32 size;
	size = CHUNKSIZE;

	HAL_DramStartAddrSet(AVIIdxAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x31);
	HAL_DramWordDataWrite(0x77,0x62);
	HAL_DramWordDataWrite(0x00,0x00);
	HAL_DramWordDataWrite(0x00,0x00);
	HAL_DramWordDataWrite(READ8(MoviOfset, 3),READ8(MoviOfset, 2));
	HAL_DramWordDataWrite(READ8(MoviOfset, 1),READ8(MoviOfset, 0));
	HAL_DramWordDataWrite(READ8(size,3),READ8(size,2));
	HAL_DramWordDataWrite(READ8(size,1),READ8(size,0));
#ifdef OpenTwoFile
	vidIndexNum += 16;
#endif
}
#endif

/**
 * @fn		  UINT8 videoRecRemoveEndVoice(UINT32 idxSize, UINT32 rmvIdxNum)
 * @brief	  video Record Remove the noise in the End
 * @param	  [in] idxSize : total index size
 * @param	  [in] rmvIdxNum : remove index number
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecRemoveEndVoice(UINT32 idxSize, UINT32 rmvIdxNum)
{
	UINT32 num;
	UINT32 totalNum;
	UINT32 addr;// = K_SDRAM_AviCaptureIdx1Addr;
	UINT8 tmp1,tmp2;
	UINT8 tmp3,tmp4;
	totalNum = idxSize>>4;

	if(idxSize==0){
		return FAIL;
	}
	num = rmvIdxNum;
	if(totalNum < rmvIdxNum){
		num= totalNum;
	}
	#if 0
	//HAL_GprmDramFill(addr+((totalNum-num)<<4)/2, num*16, 0);
	addr = vidClipCtrl.AVIIdxAddr - (num<<3);
	HAL_GprmDramFill(addr, num<<4, 0);
	#else
	while(num>0){
		addr = vidClipCtrl.AVIIdxAddr - (num<<3);
		HAL_DramStartAddrSet(addr, 1);
		HAL_DramWordDataRead(&tmp1, &tmp2);
		HAL_DramWordDataRead(&tmp3, &tmp4);
		
		if(tmp1==0x30 && tmp2==0x30 && tmp3==0x64 && tmp4==0x63){
			}
		else{
			 HAL_GprmDramFill(addr, 16, 0);
			}
		num--;
		}
	#endif

	return SUCCESS;
}
UINT32  audDateSizeGet()
{
	UINT32 inIdx;
	

	*((UINT8*)&inIdx  ) = 0;
	*((UINT8*)&inIdx+1) = XBYTE[REG_Audio_DataCnt2];
	*((UINT8*)&inIdx+2) = XBYTE[REG_Audio_DataCnt1];
	*((UINT8*)&inIdx+3) = XBYTE[REG_Audio_DataCnt0];
	return inIdx;
}
