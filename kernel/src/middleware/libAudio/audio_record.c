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
 * @file	  audio_record.c
 * @brief	  middle ware  API for audio module
 * @author	  sunyong
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "timer.h"
#include "common.h"
#include "doslink.h"
//#include "dosusr.h"
#include "cardlink.h"
//#include "cardfunc.h"
#include "video_clip.h"
#include "audio_rw.h"
#include "os_api.h"
#include "os_main.h"
#include "app_main.h"
#include "app_ui_osd.h"
#include "os_msg.h"
#include "func_ptr.h"
#include "fileindex.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "dos32.h"//eric@20080502
#include "solution.h"//eric@20080502
#include "hal_audio.h"//eric@20080502
#include "hal_global.h"
#include "hal_gprm.h" 
#include "hal.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "sp1k_gpio_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_aud_api.h"
#include "reg_def.h"
#include "video_mem.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
code UINT8	  G_IMA_PCM_WAVE_Header[]=
{
/*0x0000*/	0x52, 0x49, 0x46, 0x46, // "RIFF"
/*0x0004*/	 0x00,	0x00,0x00, 0x00, //size
/*0x0008*/	 0x57,	0x41,0x56, 0x45, //WAVE
/*0x000c*/	 0x66,	0x6d,0x74, 0x20, //fmt

/*0x0010*/	 0x10,	0x00,0x00, 0x00, //size
/*0x0014*/	 0x01,	0x00,0x01, 0x00, //format tag,channel
/*0x0018*/	 0x40,	0x1f,0x00, 0x00, //samples per second
/*0x001c*/	 0x40,	0x1f,0x00, 0x00, //averagebytespersecond
/*0x0020*/	 0x01,	0x00,0x10, 0x00, //block aligment ,bitspersample
/*0x0024*/	 0x64,	0x61,0x74, 0x61, //DATA
/*0x0028*/	 0x00,	0x00,0x00, 0x00, //size
};

code UINT8	G_IMA_WAVE_Header[] =
{

/*0x0000*/	0x52 ,0x49,0x46,0x46,
/*0x0004*/	0x34 ,0x5a,0x2,0x0,
/*0x0008*/	0x57 ,0x41,0x56,0x45,
/*0x000c*/	0x66 ,0x6d,0x74,0x20,
/*0x0010*/	0x14 ,0x0,0x0,0x0,
/*0x0014*/	0x11 ,0x0,0x1,0x0,
/*0x0018*/	0xe0 ,0x2e,0x0,0x0,
/*0x001c*/	0xc3 ,0x17,0x0,0x0,
/*0x0020*/	0x0 ,0x1,0x4,0x0,
/*0x0024*/	0x2 ,0x0,0xf9,0x1,
/*0x0028*/	0x66 ,0x61,0x63,0x74,
/*0x002c*/	0x4 ,0x0,0x0,0x0,
/*0x0030*/	0x1b ,0xa2,0x4,0x0,
/*0x0034*/	0x64 ,0x61,0x74,0x61,
/*0x0038*/	0x0 ,0x5a,0x2,0x0,

};

//patch5.1@ada@Audio annotation begin
code	UINT8	G_IMA_JPEG_WAVE_Header[] =
{
	0
}; 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
appAudRecCtrl_t xdata audRecCtrl;
static UINT8 xdata audioType;
static UINT32 count ;
static UINT32 outIdx ;

UINT32 tmpNum,ByteAddr;

UINT32 AudRec_GT_zero = 0;
UINT32 AudRec_GT_pause = 0; 
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//extern xdata UINT16 G_USR_FileIndex;
//extern xdata UINT8 G_Card_WriteProtect;
extern xdata SINT8 alc_gain_idx ; //recover alc_gain_idx
extern xdata UINT8 alc_default_gain_index; //recover alc_gain_idx
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//-----------------------------------------------------------------------------
//audioRecInit
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 audioRecInit(void)
 * @brief	  audio record initiate
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 audioRecInit(void) USING_0
{
	UINT8 sts;
	UINT16 i;
	UINT32 k = 5000;
	UINT32 clusterSize = vfsClusterSizeGet();
//	UINT16 fileIndex = G_USR_FileIndex - 1;
	UINT32 audResv = AUD_FRAG_SIZE+ clusterSize+ 500 * 1024UL;

	while((XBYTE[REG_Audio_AudEn]&0x02) ==0x02){
		if(!(k--)) {
			XBYTE[REG_Audio_DataThdIntSts] |= 0x02;
			XBYTE[REG_Audio_DataThdIntEn] &=  (~0x02);
			XBYTE[REG_Audio_AudEn] &=  (~0x02);
			sp1kHalSpeakerSet(0);
			break;
		}
	}
	sp1kHalSpeakerSet(0);

	if(audioRecTypeGet() == AUDIO_TYPE_NORMAL || audRecCtrl.name[0] == 0){ //xian ++ 20091010, #39169 
		//dcfWaveNameGet(audRecCtrl.name, audRecCtrl.fileIdx);
		dcfMediaNameGet(audRecCtrl.name,audRecCtrl.fileIdx,MEDIA_WAV);
		
	}
		//eric@20080502,begin
	audRecCtrl.freeCluster = DOS_FreeDiskSpaceByCluster();

	sts = FAIL;

#if 0 //xian -- 20090104
	  audRecCtrl.usedCluster = 1 + ((UINT32)500L*1024L)/(UINT32)G_DOS_ClusterSize;

	  if (audRecCtrl.freeCluster > audRecCtrl.usedCluster)
	  {
		audRecCtrl.freeCluster -= audRecCtrl.usedCluster;
		audRecCtrl.usedCluster = (500L*1024L)/G_DOS_ClusterSize;
			if (audRecCtrl.usedCluster <= audRecCtrl.freeCluster)
				sts = SUCCESS;
		}
#else
	//xian ++ 20090104, #33803
	audRecCtrl.usedCluster = 1 + audResv / (UINT32)clusterSize;
	if (audRecCtrl.freeCluster > audRecCtrl.usedCluster){
		audRecCtrl.freeCluster -= audRecCtrl.usedCluster;
		if(audRecCtrl.freeCluster > 1)
			sts = SUCCESS;
	}
	//xian --
#endif
	if ( sts == FAIL)
	{
		//osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		//printf("card full\n");
		return FAIL;
	}

#if 0
	HAL_GprmDramFill(audRecCtrl.fragBufAddr, 0x80000 << 1, 0);
	HAL_GprmDramFill( audRecCtrl.AudioAddr, ((UINT32)AUD_RINGBUF_SIZE+0x8000) << 1, 0);
#endif

#if 1//def DOS32_SUPPORT
//xian mask, 20081203, voice memo donot care file index
//#ifdef MULTI_FOLDER_AS_ONE  //xian ++ 20080806, voice memo donot care file index
#if 0//xian --, 20090116
		if(G_USR_FileIndex > 9999 && audioRecTypeGet() == AUDIO_TYPE_VOICE){
			G_USR_FileIndex --;
			audRecCtrl.fHandle	= vfsFileOpen(audRecCtrl.name,K_DOS_NewFile,audRecCtrl.freeCluster); //eric@20080502,begin
			G_USR_FileIndex ++;
		}
		else
#endif			  
			audRecCtrl.fHandle	= vfsFileOpen(audRecCtrl.name,K_DOS_NewFile,audRecCtrl.freeCluster); //eric@20080502,begin
//#else
//		audRecCtrl.fHandle	= vfsFileOpen(audRecCtrl.name,K_DOS_NewFile,audRecCtrl.freeCluster); //eric@20080502,begin
//#endif
		if ( audRecCtrl.fHandle == 0 )
			return FAIL;

		//eric@20080502,begin
		//write 512 dummy for reserve header
		vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, 512, audRecCtrl.AudioAddr,1);
		//eric@20080502,end
#else
		audRecCtrl.fHandle	= vfsFileOpen(0,K_DOS_NewFile,audRecCtrl.freeCluster);
		if ( audRecCtrl.fHandle == 0 )
			return FAIL;

#endif
		//HAL_AudReset();
		if(audRecCtrl.mode == AUD_ADPCM_FMT) {
			HAL_AudAdpcmSet( (UINT32)AUD_RECORD_MODE, (UINT32)1, (UINT32)AUD_BLOCK_ALIGN);
		} else {
			HAL_AudAdpcmSet( (UINT32)AUD_RECORD_MODE, (UINT32)0, (UINT32)0);
		}
		HAL_AudRingBufferSet((UINT32)(audRecCtrl.AudioAddr <<1), (UINT32)AUD_RINGBUF_SIZE);
		HAL_AudOutIndexSet( (UINT32)0 );
		HAL_AudThdSet( REC_CHUNK_SIZE );

		if(audRecCtrl.mode == AUD_ADPCM_FMT) {
			audRecCtrl.headerSize = sizeof(G_IMA_WAVE_Header);
			audRecCtrl.dataSizeOffset = 0x38;
		}
		else {
			audRecCtrl.headerSize = sizeof(G_IMA_PCM_WAVE_Header);
			audRecCtrl.dataSizeOffset = 0x28;
		}


		
		audRecCtrl.SecNum = (UINT32)audRecCtrl.headerSize; //WAV Header

		for(i=0;i< audRecCtrl.headerSize;i++){
			if(audRecCtrl.mode == AUD_ADPCM_FMT) {
					XBYTE[0x3800+i] = G_IMA_WAVE_Header[i];
			}
			else {
				XBYTE[0x3800+i] = G_IMA_PCM_WAVE_Header[i];
			}
		}

		HAL_DmaSetDramAddr(audRecCtrl.AVIHeadAddr);
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaDo(1,0,audRecCtrl.headerSize,0,0);
		audRecCtrl.SecAddr = 0;
		//HAL_AudVolumeSet(3, 0);//sp1kAudioVolumeSet(2);
		HAL_AudPause();
		//HAL_AudALCModeSet(1); //disable HW ALC
		HAL_AudStart( (UINT32)AUD_RECORD_MODE, (UINT32)audRecCtrl.freq , (UINT32)AUD_SAMPLE_BIT, (UINT32)AUD_CHANNEL );
		HAL_GlobalTimerWait(180);	//for avoid snap key pluse!
		//printf("0x2641=%bx,0x26c7=%bx,0x26c8=%bx\n",XBYTE[0x2641],XBYTE[0x26c7],XBYTE[0x26c8]);
		//HAL_AudPGAGainSet(0x0B, 3);
		HAL_GlobalReadGTimer(&AudRec_GT_zero);
		audioRecGTSet(AudRec_GT_pause);
		//printf("exit audioinit\n");
		HAL_AudResume();
		//printf("R:0x2641=%bx,0x26c7=%bx,0x26c8=%bx\n",XBYTE[0x2641],XBYTE[0x26c7],XBYTE[0x26c8]);


		return(sts);
}

//-----------------------------------------------------------------------------
//audioRec
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRec(void)
 * @brief	  audio record
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRec(void) USING_0
{
	//UINT32 usedSize;
	UINT32 time;
	UINT32 temp;
	//UINT32 clusterSize = vfsClusterSizeGet();
	temp = 0;


	if(audRecCtrl.recStatus == AUD_REC_PAUSE) {
		return;
	}
	#if 0 /*mark for mantis #42652 S .but do not know why add these at before!*/
	usedSize = ((audRecCtrl.SecAddr + 1) <<9) + clusterSize+ AUD_FRAG_SIZE;
	audRecCtrl.usedCluster = usedSize / clusterSize;
	if(audRecCtrl.usedCluster  > audRecCtrl.freeCluster)
	{
		osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		//DbgPrint("usedCluster=%d\n",audRecCtrl.usedCluster);
		//printf("card full\n");
		audioRecAbort();
		return;
	}
	#endif /*mark for mantis #42652 E*/
	//HAL_GlobalReadGTimer(&time);
	time = audioRecTimeGet();
	time = time / 1000;
	if(count != time) {
		//show osd		
		uiUpdateAudRecTime(time);
		count = time;
	}
	while( HAL_AudIntStsGet( 0 ) == (UINT8)0){
		HAL_AudRingBufCntRead(&audRecCtrl.DRAMAudBufCnt);
		//printf("%d %ld\n",(UINT16)__LINE__,audRecCtrl.DRAMAudBufCnt);
	}

	HAL_AudRingBufCntRead( &audRecCtrl.DRAMAudBufCnt);
	if(audRecCtrl.DRAMAudBufCnt > 1024)
	{
		 if(audRecCtrl.SecAddr == 0){
			//printf("first\n");
			temp = (UINT32)512 - audRecCtrl.SecNum;
			HAL_GprmDramDma((UINT32)(audRecCtrl.AudioAddr <<1), (UINT32)((audRecCtrl.AVIHeadAddr << 1) + audRecCtrl.SecNum), temp, 1);
			outIdx += temp;

			HAL_GprmDramDma((UINT32)((audRecCtrl.AudioAddr <<1) + outIdx) ,(UINT32)((audRecCtrl.fragBufAddr<<1) + ByteAddr ), (UINT32) audRecCtrl.SecNum, 1);
			ByteAddr  +=  audRecCtrl.SecNum;
			tmpNum	+=	audRecCtrl.SecNum;
			outIdx +=  audRecCtrl.SecNum;
			audRecCtrl.SecAddr += 1;
			audRecCtrl.DRAMAudBufCnt -= outIdx;
		 }
		audRecCtrl.SecCnt = (audRecCtrl.DRAMAudBufCnt >>9);
		if(audRecCtrl.SecCnt  > 0){
			temp = ((UINT32)(audRecCtrl.SecCnt << 9));
			if((UINT32)(outIdx + temp) <= (UINT32)AUD_RINGBUF_SIZE){
				HAL_GprmDramDma((UINT32)((audRecCtrl.AudioAddr<<1) + outIdx),(UINT32)((audRecCtrl.fragBufAddr<<1) + ByteAddr) , (UINT32)temp, 1);
			}
			else{
				HAL_GprmDramDma((UINT32)((audRecCtrl.AudioAddr<<1) + outIdx) , (UINT32)((audRecCtrl.fragBufAddr<<1) + ByteAddr) , (UINT32)(AUD_RINGBUF_SIZE - outIdx), 1);
				HAL_GprmDramDma((UINT32)(audRecCtrl.AudioAddr<<1), (UINT32)((audRecCtrl.fragBufAddr<<1) + ByteAddr + AUD_RINGBUF_SIZE - outIdx) , (UINT32)(temp + outIdx - AUD_RINGBUF_SIZE) , 1);
			}

			ByteAddr += temp;
			tmpNum	+= temp;
			outIdx += temp;

			outIdx = outIdx % (UINT32)AUD_RINGBUF_SIZE ;
			HAL_AudOutIndexSet( (UINT32)outIdx);
			audRecCtrl.SecAddr += audRecCtrl.SecCnt ;
		}
	}

	if(tmpNum >= AUD_FRAG_SIZE){
		if(audRecCtrl.AudChunkCnt == 0) {
			vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, AUD_FRAG_SIZE , audRecCtrl.fragBufAddr, 1);
		} else {
			vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, AUD_FRAG_SIZE, audRecCtrl.fragBufAddr, 0);
		}
		audRecCtrl.AudChunkCnt += (AUD_FRAG_SIZE>>9);
		tmpNum -= AUD_FRAG_SIZE;
		if(tmpNum > 0){
			HAL_GprmDramDma((UINT32)((audRecCtrl.fragBufAddr<<1) + AUD_FRAG_SIZE), (UINT32)( audRecCtrl.fragBufAddr<<1), (UINT32)tmpNum ,1);
		}
		ByteAddr = tmpNum;
	}
	//count ++;
}

//-----------------------------------------------------------------------------
//audioRecAbort
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecAbort(void)
 * @brief	  audio record abort
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecAbort(void) USING_0
{
	UINT32 audioSize = 0;
	UINT32 samplePerBlock;
	UINT32 avgBytePerSec;
	UINT32 alignment;

	ACTIVE_FUNC_CLR();
	HAL_AudStop( 0 );
	/*For HDMI beep Voice*/
	HAL_AudPause(); 
	
	//XBYTE[REG_Audio_AgcFuncEN] = 0x00; //disable alc
	//HAL_StopGTimer();
	audRecCtrl.recStatus = AUD_REC_IDLE;
	audRecCtrl.SecCnt = (tmpNum>>9);
	if(tmpNum % 512) {
		audRecCtrl.SecCnt++;
	}
	if( audRecCtrl.AudChunkCnt == 0 ){
		vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, audRecCtrl.SecCnt * (UINT32)512, audRecCtrl.fragBufAddr, 1);
	}
	else{
		vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, audRecCtrl.SecCnt * (UINT32)512, audRecCtrl.fragBufAddr, 0);
	}
	audRecCtrl.AudChunkCnt += audRecCtrl.SecCnt;
	audRecCtrl.AudChunkCnt -= RMV_ABORT_NOISE;
	if(audRecCtrl.mode == AUD_ADPCM_FMT) {

		samplePerBlock = (AUD_BLOCK_ALIGN - 4 )*2 + 1;
		avgBytePerSec = audRecCtrl.freq  * AUD_BLOCK_ALIGN / samplePerBlock;

		samplePerBlock = (samplePerBlock & 0xffff) << 16 | 0x02;
		audioSize = audRecCtrl.AudChunkCnt * (UINT32)512 ;
	//	printf(" avgBytePerSec: %ld samplePerBlock :%ld \n",avgBytePerSec,samplePerBlock);
	//	printf("audio size :%ldk\n",(UINT32)audioSize/1024);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 2, audioSize + 52 );
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 12, audRecCtrl.freq);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 14, avgBytePerSec);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 18, samplePerBlock);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 28, audioSize);
	} else {
		audioSize = audRecCtrl.AudChunkCnt * (UINT32)512 ;
		alignment = (UINT32)AUD_CHANNEL * AUD_SAMPLE_BIT/8;
		alignment = ((UINT32)AUD_SAMPLE_BIT & 0xffff) <<16 | alignment;

		videoFillDramLong(audRecCtrl.AVIHeadAddr + 2, audioSize + 36);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 12, audRecCtrl.freq);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 14, audRecCtrl.freq *2);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 16, alignment);
		videoFillDramLong(audRecCtrl.AVIHeadAddr + 20, audioSize);
	}
	audioSize += (UINT32)(audRecCtrl.headerSize  - 8 );
#if 0

	//printf("%d%ld\n",(UINT16)__LINE__,audRecCtrl.DRAMAudBufCnt);
	audioSize = audRecCtrl.AudChunkCnt * 512;
	videoFillDramLong(audRecCtrl.AVIHeadAddr + audRecCtrl.dataSizeOffset/2, audioSize);
	audioSize += (audRecCtrl.headerSize  - 8 );
	videoFillDramLong(audRecCtrl.AVIHeadAddr +2, audioSize);

#if RecordFrequence24khz  //ljh@03.03.18
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0c, 0x5dc0);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0e, 0x2f33);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x24, 0x5dc0);
#elif RECORD_FREQUENCE_48KHZ // ljh@03.03.18
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0c, 0xbb80);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0e, 0x5e65);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x24, 0xbb80);
#elif RECORD_FREQUENCE_44KHZ // ljh@03.03.18
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0c, 0xac44);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x0e, 0x56ba);
	videoFillDramLong(audRecCtrl.AVIHeadAddr+0x24, 0xac44);
#endif
#endif
#if 1//def DOS32_SUPPORT
	//eric@20080502,begin
	DOS_FileSeek(audRecCtrl.fHandle, 0 ,0);
	vfsFileWrite(audRecCtrl.fHandle, K_File_SrcDataDram2Card, 512 , audRecCtrl.AVIHeadAddr, 1);
	//eric@20080502,end

	//patch5.2.1@wyeo@solve host format avi fail@begin
	//patch5.1@wyeo@solve avi sync problem on sd, mmc@begin
	if (G_Card_Type == K_MEDIA_SMC ||  G_Card_Type == K_MEDIA_NANDF)
	{	//eric@20080502
		//M_Card_LPDram2Card(G_File_HdrLogAddr, G_File_HdrPhyAddr, G_File_HdrLogBlock, audRecCtrl.AVIHeadAddr);
		vfsFileClose(audRecCtrl.fHandle,audRecCtrl.name, K_DOS_NewFile, audioSize + (UINT32)8);
	}
	else
	{	//eric@20080502
		vfsFileClose(audRecCtrl.fHandle, audRecCtrl.name, K_DOS_NewFile, audioSize + (UINT32)8);
		//M_Card_LPDram2Card(G_File_HdrLogAddr, G_File_HdrPhyAddr, G_File_HdrLogBlock, audRecCtrl.AVIHeadAddr);
	}
#else
	//patch5.2.1@wyeo@solve host format avi fail@begin
	//patch5.1@wyeo@solve avi sync problem on sd, mmc@begin
	if (G_Card_Type == K_MEDIA_SMC ||  G_Card_Type == K_MEDIA_NANDF)
	{
		M_Card_LPDram2Card(G_File_HdrLogAddr, G_File_HdrPhyAddr, G_File_HdrLogBlock, audRecCtrl.AVIHeadAddr);
		vfsFileClose(1,audRecCtrl.name, K_DOS_NewFile, audioSize + (UINT32)8);
	}
	else
	{
		vfsFileClose(1, audRecCtrl.name, K_DOS_NewFile, audioSize + (UINT32)8);
		M_Card_LPDram2Card(G_File_HdrLogAddr, G_File_HdrPhyAddr, G_File_HdrLogBlock, audRecCtrl.AVIHeadAddr);
	}
#endif

	//printf("name %s, idx %u\n", audRecCtrl.name, G_USR_FileIndex);
	if (audioRecTypeGet() != AUDIO_TYPE_VOICE) {
#if 1
		dcfNewFileDramFdbParameterUpdate(K_DCF_WAV_TYPE, audioSize + 8);
#else
		G_USR_NewFileSizeInFdb = audioSize + 8;

		dcfFileDramFdbParameterWrite(K_DCF_WAV_TYPE,G_USR_FileIndex);
		G_USR_LastFileIndex = G_USR_FileIndex; //yichang@0510

		//patch4.5@ada@Fix file index bug begin
		if (G_USR_FileCount == 0)
		{
			G_USR_FirstFileIndex = G_USR_FileIndex;
		}
		//patch4.5@ada@Fix file index bug end

#if 0//def MULTI_FOLDER_AS_ONE	//xian ++ 20080806
		USR_UpdateCurFolderInfo(K_UPDATE_MODE_ADD_ONE);  //xian ++ 20080806
#endif
		G_USR_FileIndex ++;
		G_USR_FileCount++;
		DOS_FileOrderWrite(G_USR_FileCount , G_USR_LastFileIndex);
#endif
	}

	//HAL_GlobalCamModeSet(0);
	//HAL_GlobalCamModeSet(1);
	//RDpreview();
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	osMsgPost(SP1K_MSG_MEDIA_REC_OK);
	/*kai++ debug for volume change*/
	//HAL_AudVolumeSet(VoiceData, 0x00 );
	/*kai++ debug for volume change*/



//	printf("return preview\n");
}

//-----------------------------------------------------------------------------
//audioRecKeyOnSnap
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecStart(void)
 * @brief	  audio record key on snap process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecStart(void)
{
	UINT8 sts;

	sts = audioRecGlobalInit();
	if(sts == FAIL){
		printf("aud_init error!\n");
		return ;
	}
	if(audRecCtrl.recStatus == AUD_REC_IDLE) {
		sts = audioRecInit();
		if(!sts) {

			audRecCtrl.recStatus = AUD_REC_RUN;
			osActiveFuncSet(pFunc_AudioRec);
		}else {
			//printf("aud_init err\n");
		}
	}
}

//-----------------------------------------------------------------------------
//audioRecKeyOnSnap
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecStop(void)
 * @brief	  audio record key on snap process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecStop(void)
{
	if(audRecCtrl.recStatus == AUD_REC_IDLE) {
		return;
	}

	audioRecAbort();
}

//-----------------------------------------------------------------------------
//audioRecKeyOnPause
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecPause(void)
 * @brief	  audio record key on pause process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecPause(void) USING_0
{
	if(audRecCtrl.recStatus != AUD_REC_RUN) {
		return;
	}

	HAL_AudPause();
	HAL_GlobalReadGTimer(&audRecCtrl.time);
	AudRec_GT_pause = audRecCtrl.time;
	//HAL_StopGTimer();
	audRecCtrl.recStatus = AUD_REC_PAUSE;
}

//-----------------------------------------------------------------------------
//audioRecKeyOnResume
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecResume(void)
 * @brief	  audio record key on resume process
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecResume(void) USING_0
{
	if(audRecCtrl.recStatus != AUD_REC_PAUSE) {
		return;
	}
	//HAL_ConfigGTimer(0x05);
	//HAL_WriteGTimer(audRecCtrl.time);
	//HAL_StartGTimer();
	HAL_GlobalReadGTimer(&audRecCtrl.time);
	AudRec_GT_pause = audRecCtrl.time - AudRec_GT_pause;
	audioRecGTSet(AudRec_GT_pause);
	HAL_AudResume();
	audRecCtrl.recStatus = AUD_REC_RUN;
}

//-----------------------------------------------------------------------------
//audioRecBufSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecBufSet(UINT32 addr)
 * @brief	  audio record buffer set for image voice
 * @param	  [in] addr
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecBufSet(UINT32 addr)
{
	audRecCtrl.AudioAddr = (((addr>>3)+1)<<3);
	audRecCtrl.fragBufAddr = audRecCtrl.AudioAddr +K_SDRAM_AviCapAudioBufSize;
	audRecCtrl.AVIHeadAddr = audRecCtrl.fragBufAddr +  ((AUD_FRAG_SIZE +AUD_FRAG_SIZE)>>1);
}

//-----------------------------------------------------------------------------
//audioRecFileIdxSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecFileIdxSet(UINT16 index)
 * @brief	  audio file index set
 * @param	  [in] index
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecFileIdxSet(UINT16 index)
{
	audRecCtrl.fileIdx = index;
}

//-----------------------------------------------------------------------------
//audioRecGlobalInit
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecGlobalInit(void)
 * @brief	  audio record global variable initiate
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 audioRecGlobalInit(void) USING_0
{
	AudRec_GT_pause = 0;
	AudRec_GT_zero = 0;
	tmpNum = 0;
	ByteAddr = 0;
	outIdx = 0;
	count =1;
	audRecCtrl.time = 0;
	audRecCtrl.recStatus = AUD_REC_IDLE;
	audRecCtrl.DRAMAudBufCnt = 0;
	audRecCtrl.AudChunkCnt = 0;
	if(audioRecTypeGet() == AUDIO_TYPE_NORMAL){
		if(!dcfFileIndexUpdate()){	//xian ++ 20081203
			return FAIL;
		}
		audRecCtrl.fileIdx = dcfFileIndexGet();
		#if 1
		audRecCtrl.AudioAddr = K_SDRAM_AviCapAudioBufAddr;
		audRecCtrl.fragBufAddr = K_SDRAM_AviCapFragBufAddrA;
		audRecCtrl.AVIHeadAddr = K_SDRAM_AviCapHeaderBufAddr;
		#endif
		#if 0
		audRecCtrl.AudioAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_AUDIO);
		audRecCtrl.fragBufAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG);
		audRecCtrl.AVIHeadAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR);
		#endif
	}
	alc_gain_idx = alc_default_gain_index; //recover alc_gain_idx
	sp1kAudParamSet(SP1K_AUD_PARAM_ALC_GAIN_SET,alc_gain_idx);

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//imageVoiRecSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void imageVoiRecSet(UINT32 addr, addr,UINT16 nameIdx)
 * @brief	  image voice record set
 * @param	  [in] addr
 * @param	  [in] nameIdx
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void imageVoiRecSet(UINT32 addr,UINT16 nameIdx)
{
	audioRecBufSet(addr);
	audioRecFileIdxSet(nameIdx);
	audRecCtrl.name[0] = 0; //xian ++ 20091010, #39169
}

//xian ++ 20091010, #39169 
/**
 * @fn		  void imageVoiRecImgNameSet(UINT8 *name)
 * @brief	  imageVoiRecImgNameSet
 * @param	  [in] name
 * @retval	  NONE
 * @see 	  NULL
 * @author	  xianxin
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void imageVoiRecImgNameSet(UINT8 *name)
{
	if(name){
		strcpy(audRecCtrl.name, name);
		audRecCtrl.name[8] = 'W';
		audRecCtrl.name[9] = 'A';
		audRecCtrl.name[10] = 'V';
		audRecCtrl.name[11] = 0x20;
	}
}
//xian --

//-----------------------------------------------------------------------------
//audioRecStatusGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 audioRecStatusGet(void)
 * @brief	  get audio record status
 * @param	  NONE
 * @retval	  record status
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 audioRecStatusGet(void) USING_0
{
	return audRecCtrl.recStatus;
}

//-----------------------------------------------------------------------------
//audioRecFreqSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecFreqSet(UINT8 freq)
 * @brief	  get audio record freq. set
 * @param	  [in] freq: audio sample frequency
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecFreqSet(UINT8 freq) USING_0
{
	switch(freq) {
		case RECORD_FREQUENCE_8KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_8K;
			break;
		case RECORD_FREQUENCE_1KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_11K;
			break;
		case RECORD_FREQUENCE_12KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_12K;
			break;
		case RECORD_FREQUENCE_16KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_16K;
			break;
		case RECORD_FREQUENCE_22KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_22K;
			break;
		case RECORD_FREQUENCE_32KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_32K;
			break;
		case RECORD_FREQUENCE_44KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_44K;
			break;
		case RECORD_FREQUENCE_48KHZ:
			audRecCtrl.freq = AUD_SAMPLE_RATE_48K;
			break;
		default:
			audRecCtrl.freq = AUD_SAMPLE_RATE_8K;
			break;
	}
}

//-----------------------------------------------------------------------------
//audioRecFmtSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecFmtSet(UINT8 fmt)
 * @brief	  get audio record format set
 * @param	  [in] fmt: audio data format
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecFmtSet(UINT8 fmt)
{
	if(fmt) {
		audRecCtrl.mode = AUD_ADPCM_FMT;

	}
	else {
		audRecCtrl.mode = AUD_PCM_FMT;
	}
}

//-----------------------------------------------------------------------------
//audioRecGTSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecGTSet(UINT32 pausetime)
 * @brief	  Set audio record globle time
 * @param	  [in] pausetime: GT start time
 * @retval	  NONE
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecGTSet(UINT32 pausetime)
{
	AudRec_GT_zero	+= pausetime;
}


//-----------------------------------------------------------------------------
//audioRecTimeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 audioRecTimeGet(void)
 * @brief	  Get audio record globle time
 * @param	  NONE
 * @retval	  record time
 * @see 	  NULL
 * @author	  SunYong
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 audioRecTimeGet(void)
{
	UINT32 time=0;

	if(audRecCtrl.recStatus == AUD_REC_RUN) {
		HAL_GlobalReadGTimer(&time);
	}
	else if(audRecCtrl.recStatus == AUD_REC_PAUSE) {
		time = AudRec_GT_pause;
	}

	time = time - AudRec_GT_zero;

	return time ;
}

//-----------------------------------------------------------------------------
//audioRecTypeSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void audioRecTypeSet(UINT8 type)
 * @brief	  audioRecTypeSet
 * @param	  [in] type
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Phil Lin
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
void audioRecTypeSet(UINT8 type)
{
	audioType = type;
}

//-----------------------------------------------------------------------------
//audioRecTypeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 audioRecTypeGet(void)
 * @brief	  audioRecTypeGet
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Phil Lin
 * @since	  2010-5-14
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 audioRecTypeGet(void)
{
	return audioType;
}

