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
 * @file      audio_pb.c
 * @brief     middle ware  API for audio playback module
 * @author    sunyong
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "common.h"
//#include "cardfunc.h"
#include "initio.h"
//#include "dosusr.h"
#include "doslink.h"
#include "audio_rw.h"
#include "os_api.h"
#include "os_main.h"
#include "app_main.h"
#include "osd_api.h"
#include "app_ui_osd.h"
#include "os_msg.h"
#include "func_ptr.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "gpio.h"
#include "dos32.h"
#include "hal_audio.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "hal_global.h"
#include "hal.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "sp1k_gpio_api.h"
#include "disp_api.h"
#include "reg_def.h"
#include "pb_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define K_PCM_Format			(UINT16)0x0001
#define K_IMA_ADPCM_Format	    (UINT16)0x0011
#define K_Monaural 				(UINT16)0x0001
#define K_Stereo   				(UINT16)0x0002
#define K_Audupsmp_1sector		(UINT8) 0x01	//for upsampling
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appAudPbCtrl_t xdata audPbCtrl = {
	AUD_PB_IDLE,
	{0},
	0,
	0,
	0,
	0,
	0,//K_SDRAM_AviPbAudioChunkBufAddr/*+K_SDRAM_ImagePlaybackBufSize*/, //mark by phil@2009.01.09
	AUD_PB_SPEED_NORMAL,
	0,
	0,
	0, //K_SDRAM_AviPbAudioBufAddr/*+K_SDRAM_ImagePlaybackBufSize*/, //mark by phil@2009.01.09
	0,
 	0,
};

UINT32 xdata bytePerMs = 0;
UINT32 AudPb_GT_zero = 0;
UINT32 AudPb_GT_pause = 0;
UINT32 xdata gTime=0;
UINT8 red_cdsp;
UINT8 red_front;
UINT8 G_APBupSmpflag = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 G_VPBupSmpflag;

/* add for FPGA test */

extern void HAL_AudStart(UINT32 mode,UINT32 sampleRate,UINT32 sampleBit,UINT32 channel);

/*----------------*/

extern void audioUpSample(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT8 sector) ;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//audioPbInit
//-----------------------------------------------------------------------------
/**
 * @brief		audio play initiate
 * @param	ID	:	file index
 * @retval	0 is success,1 is fail
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 audioPbInit(UINT16 ID) USING_0
{
	UINT8  sts ;
	UINT8 i;
	PBFile_t pFileInfo;
#if 0
	//version4.0@ada@0513 for DCF
//	UINT8  FileType;
//	UINT16 startCluster;
	//patch5.1@ada@Play kinds of audio file format begin
	UINT32 RiffckSize = 0;
	UINT32 fmt_ckSize = 0;
	UINT32 nSamplesPerSec = 0;
	UINT32 nAvgBytesPerSec = 0;
	UINT16 wBitsPerSample = 0;
	UINT16 TempHeaderOffset = 0;
	UINT16 cbSize = 0;
	UINT16 wSamplesPerBlock = 0;
	UINT32 fact_ckSize = 0;
	UINT32 dwSampleLength = 0;
	UINT16 RecordingTime = 0;
	UINT32 LIST_ckSize = 0;
	UINT16 TempChunkOffset = 0;
	UINT32 TempChunkSize = 0;
	UINT16 AudioAnnotation_JpgFileIndex = 0;
	UINT8 AudFormat = 0;
#endif
	UINT16 nChannels = 0;
	UINT16 nBlockAlign = 0;
	UINT16 wFormatTag = 0;
	UINT32 Playfreq = 0;
	UINT32 k = 5000;
	//patch5.1@ada@Play kinds of audio file format end

	/* power control -- audio recording */
	//appModePowerCtrl(MODE_AUDIOPB);  // mark for FPGA test
	while((XBYTE[REG_Audio_AudEn]&0x02) ==0x02){
		if(!(k--)) {
			XBYTE[REG_Audio_DataThdIntSts] |= 0x02;
			XBYTE[REG_Audio_DataThdIntEn] &=  (~0x02);
			XBYTE[REG_Audio_AudEn] &=  (~0x02);
			break;
		}

	}
	red_cdsp = XBYTE[REG_Cdsp_Cdspinten_lo];
	red_front = XBYTE[REG_Front_Finten_low];

	XBYTE[0x21ec] = 0; //disable all cdsp interrupt
	XBYTE[REG_Front_Finten_low] = 0;//disable all front interrupt
#if 0
	//patch3.2@yichang@0426 prebuild smc banking table into sdram
	Card_Specific( 1, 0xff );
#endif

	if ((pbFileInfoGet(ID, &pFileInfo) != SUCCESS)) {  //hy.ren 2012/07/11 #45435
		return FAIL;
	}

	memcpy(audPbCtrl.name, pFileInfo.FileName, sizeof(audPbCtrl.name));
	audPbCtrl.name[8] = 'W';
	audPbCtrl.name[9] = 'A';
	audPbCtrl.name[10] = 'V';

	//dcfMediaNameGet(audPbCtrl.name , (UINT16)ID,MEDIA_WAV);  // hy.ren

	//audioPbGlobalInit();
#if 0
	startCluster = dcfFileDramFdbParameterRead(K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)ID - 1)) * 10,&FileType);
#endif

	audPbCtrl.fHandle = vfsFileOpen(audPbCtrl.name , K_DOS_ReadFile, 0 /*startCluster*/);

	if (!audPbCtrl.fHandle)
	{
		//printf("Cannot open WAV file\n");
		return FAIL;
	}

	//Add for Video Play rewind eric@20081015
	DOS_FileReadMakeFatEnable(1, audPbCtrl.fHandle);

	sts = audioWAVHdrParse(&wFormatTag ,&Playfreq , &nBlockAlign ,&nChannels);
	if(sts) {
		//printf("WAV header parse err\n");
		return FAIL;
	}
	if((dispDevActGet() == 4)&&(Playfreq == 11025)) {	//for upsampling
		G_APBupSmpflag =  1;
		Playfreq = Playfreq<<2;
	}

#if 0
	printf("wFormatTag = %x\n",wFormatTag);
	printf("Playfreq = %lx\n",Playfreq);
	printf("nBlockAlign = %x\n",nBlockAlign);
	printf("nChannels = %x\n",nChannels);
#endif
	HAL_AudPause();
	audioPbSet(audPbCtrl.AudioAddr,(UINT32)AUD_RINGBUF_SIZE,(UINT16)wFormatTag,(UINT32) Playfreq,  (UINT32)nBlockAlign, (UINT8)nChannels ,16);
	for(i=0;i<8;i++) {
		audioPlayWaveChunck(audPbCtrl.fHandle , audPbCtrl.tmpCnt, (UINT32)audPbCtrl.headerSize , audPbCtrl.AudioAddr,1);
	}

	HAL_GlobalReadGTimer(&AudPb_GT_zero);
	audioPbGTSet(AudPb_GT_pause);

	HAL_AudResume();

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//audioPbStart
//-----------------------------------------------------------------------------
/**
 * @brief		audio start parater set
 * @param	Format 		: 	audio data format
 * @param	playFreq 		: 	audio  play freq.
 * @param	blockAlign 	: 	size of the block
 * @param	channel 		: 	1 is monaural, 2 is stereo
 * @retval	value
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbSet(UINT32 audAddr,UINT32 audBufSize,UINT16 Format, UINT32 playFreq, UINT32  Block,UINT8 channels ,UINT8 smpBit) USING_0
{
#if 0
	printf("Format = %lx\n", (UINT32)Format);
	printf("playFreq = %lx\n", (UINT32)playFreq);
	printf("BlockAlign = %lx\n", (UINT32)Block);
	printf("channel = %lx\n", (UINT32)channels);
#endif

	if( Format == K_PCM_Format ) {
		HAL_AudAdpcmSet( 1, 0, 0);
	}
	else {
		HAL_AudAdpcmSet( (UINT32)1, (UINT32)1,  (UINT32)Block);
	}
	HAL_AudRingBufferSet( (UINT32)(audAddr<<1), (UINT32)audBufSize);
	HAL_AudInIndexSet((UINT32)0);
	HAL_AudOutIndexSet((UINT32)0);
	if(Format == K_PCM_Format ) {
		HAL_AudStart( (UINT32)1,  (UINT32)playFreq, (UINT32)smpBit,  (UINT32)channels );
	} else {
		HAL_AudStart( (UINT32)1,  (UINT32)playFreq, (UINT32)AUD_SAMPLE_BIT,  (UINT32)channels );
	}
	if(pkgChipIdGet()== 0x1626)
	{
		if((0x20 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in or HDMI plug in
			HAL_AudSpeakerPwrSet(0);
		}
		else {
			HAL_AudSpeakerPwrSet(1);
		}
	}
	else
	{
		if((0x04 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in
			HAL_AudSpeakerPwrSet(0);
		}
		else {
			HAL_AudSpeakerPwrSet(1);
		}
	}


}

//-----------------------------------------------------------------------------
//audioPb
//-----------------------------------------------------------------------------
/**
 * @brief		audio play
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPb(void) USING_0
{
	UINT32 cnt;
	UINT32 time=0;
	static UINT32 audTime = 0;

	if((audPbCtrl.status == AUD_PB_PAUSE) || (audPbCtrl.status == AUD_PB_RW)|| (audPbCtrl.status == AUD_PB_FW)) {

		if(audPbCtrl.speed != AUD_PB_SPEED_NORMAL) {

			if(audPbCtrl.status == AUD_PB_RW){
				audioPbRW(audPbCtrl.speed);
			} else {
				audioPbFW(audPbCtrl.speed);
			}
			audioPbMsGet(&time);
			time = time/1000;
			if(audTime != time) {
				uiUpdateAudPlayTime(time);
				audTime = time;
			}
		if(audPbCtrl.tmpCnt >= audPbCtrl.AudChunckCnt) {
			HAL_AudResume();
			audioPbAbort();
			return;
		}
		if(audPbCtrl.tmpCnt <= 0) {
			/*
			audPbCtrl.tmpCnt = 0;
			audPbCtrl.idle = AUD_PB_RUN;
			audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
			audPbCtrl.audInIdx = 0;
			HAL_AudResume();
			*/
			HAL_AudResume();
			audioPbAbort();
		}

		}

		return;
	}

	//if(audPbCtrl.tmpCnt % 16 == 0) {
		audioPbMsGet(&time);
		time = time / 1000;
		if(audTime != time) {
			uiUpdateAudPlayTime(time);
			audTime = time;
		}
	//}

	cnt = audPbCtrl.AudChunckCnt - audPbCtrl.tmpCnt;
	if(cnt > 2)
	//if(audPbCtrl.tmpCnt < audPbCtrl.AudChunckCnt)
	{
		audioPlayWaveChunck(audPbCtrl.fHandle , audPbCtrl.tmpCnt, (UINT32)audPbCtrl.headerSize , audPbCtrl.AudioAddr,1);//for upsampling
	} else if(cnt > 0) {
		audioPlayWaveChunck(audPbCtrl.fHandle , audPbCtrl.tmpCnt, (UINT32)audPbCtrl.headerSize , audPbCtrl.AudioAddr,cnt);
	} else {
		HAL_AudRingBufCntRead(&cnt);
		if(cnt > 32) {
			return;
		} else {
			audioPbAbort();
		}
	}
}

//-----------------------------------------------------------------------------
//audioPbKeyOnPlay
//-----------------------------------------------------------------------------
/**
 * @brief		audio key on play process
 * @param	ID : audio file index
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbStart(UINT16 ID) USING_0
{
	UINT8 sts;

	audioPbGlobalInit();
	if(audPbCtrl.status == AUD_PB_IDLE) {
		sts = audioPbInit(ID);
		if(sts) {
			return;
		}
		osActiveFuncSet(pFunc_AudioPb);  //mark for FPGA test
		audPbCtrl.status = AUD_PB_RUN;
	}
}

//-----------------------------------------------------------------------------
//audioPbStop
//-----------------------------------------------------------------------------
/**
 * @brief		audio key on play process
 * @param	ID : audio file index
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbStop(void)
{
	if(audPbCtrl.status != AUD_PB_IDLE) {
		audioPbAbort();
	}
}

//-----------------------------------------------------------------------------
//audioPbKeyOnPause
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio key on pause process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbPause(void) USING_0
{
	HAL_AudPause();
	audPbCtrl.status = AUD_PB_PAUSE;
	HAL_GlobalReadGTimer(&audPbCtrl.time);
	AudPb_GT_pause = audPbCtrl.time;
	//HAL_StopGTimer();
	//printf("pause\n");
}

//-----------------------------------------------------------------------------
//audioPbKeyOnResume
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio key on resume process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbResume(void) USING_0
{
	if(audPbCtrl.status == AUD_PB_IDLE) {
		return;
	}

	if(audPbCtrl.status == AUD_PB_PAUSE) {
		HAL_AudResume();
		audPbCtrl.status = AUD_PB_RUN;
		HAL_GlobalReadGTimer(&audPbCtrl.time);
		AudPb_GT_pause = audPbCtrl.time - AudPb_GT_pause;
		audioPbGTSet(AudPb_GT_pause);
	} else {
		audioPbFwFrRelease();
	}
	//printf("resume\n");
}

//-----------------------------------------------------------------------------
//audioPbSpeedSet
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio FR speed set
 * @param	speed : audio play speed
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbRWSpeedSet(UINT8 speed) USING_0
{
	UINT8 rate;

	if((audiopbStatusGet() == AUD_PB_PAUSE) || (audiopbStatusGet() == AUD_PB_IDLE)) {
		return;
	}

	rate = speed;
	if(rate == 1) {
		return;
	}
	if(audPbCtrl.status == AUD_PB_FW) {
		//rate = 1;
	}
	audPbCtrl.status = AUD_PB_RW;
	if(rate == 2){
		audPbCtrl.tmpCnt = audioPbIndexGet() ;
	}
	audPbCtrl.speed = rate;
/*
	switch(rate) {
		case 1:
			audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
			audPbCtrl.idle = AUD_PB_RUN;
			break;
		case 2:
			audPbCtrl.speed = aud_pb_speed_FR1;
			audPbCtrl.tmpCnt = audioPbIndexGet() ;
			break;
		case 4:
			audPbCtrl.speed = aud_pb_speed_FR2;
			break;
		case 8:
			audPbCtrl.speed = aud_pb_speed_FR3;
			break;
		default:
			audPbCtrl.idle = AUD_PB_RUN;
			audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
			break;
	}
*/
	HAL_GlobalReadGTimer(&gTime);
#if 0
	printf("%d time : %ld\n",(UINT16)__LINE__,gTime);
	printf("speed :%bx\n",audPbCtrl.speed);
#endif
}

//-----------------------------------------------------------------------------
//audioPbSpeedSet
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio FW speed set
 * @param	speed : audio play speed
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbFWSpeedSet(UINT8 speed) USING_0
{
	UINT8 rate;

	if((audiopbStatusGet() == AUD_PB_PAUSE) || (audiopbStatusGet() == AUD_PB_IDLE)) {
		return;
	}

	rate = speed;
	if (rate == 1) {
		return;
	}
	if(audPbCtrl.status == AUD_PB_RW) {
		//rate = 1;
	}
	audPbCtrl.status = AUD_PB_FW;
	if(rate == 2){
		audPbCtrl.tmpCnt = audioPbIndexGet() ;
	}
	audPbCtrl.speed = rate;
/*
	switch(rate) {
		case 1:
			audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
			audPbCtrl.idle = AUD_PB_RUN;
			break;
		case 2:
			audPbCtrl.speed = AUD_PB_SPEED_FW1;
			audPbCtrl.tmpCnt = audioPbIndexGet() ;
			break;
		case 4:
			audPbCtrl.speed = AUD_PB_SPEED_FW2;
			break;
		case 8:
			audPbCtrl.speed = AUD_PB_SPEED_FW3;
			break;
		default:
			audPbCtrl.idle = AUD_PB_RUN;
			audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
			break;
	}
*/
	HAL_GlobalReadGTimer(&gTime);
#if 0
	printf("%d time : %ld\n",(UINT16)__LINE__,gTime);
	printf("speed :%bx\n",audPbCtrl.speed);
#endif
}

//-----------------------------------------------------------------------------
//audioPbSpeedGet
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio speed get
 * @param	NONE
 * @retval	audio play speed
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 audioPbSpeedGet(void) USING_0
{
	return audPbCtrl.speed;
}

//-----------------------------------------------------------------------------
//audioPbFW
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio key on FW process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbFW(UINT8 rate)
{
	UINT32 curTime =0;
	UINT32 tmp;
	UINT32 index;
	UINT8 speed;

	HAL_GlobalReadGTimer(&curTime);
	tmp = curTime - gTime;
	speed = rate *4;
	index = ((tmp * bytePerMs * (UINT32)speed)>>9);
	if(index) {
		audPbCtrl.tmpCnt = audPbCtrl.tmpCnt  + index;
		//audPbCtrl.tmpCnt = audioPbIndexGet() + (UINT32)index ;
		gTime = curTime;
		HAL_AudPause();
	}
}

//-----------------------------------------------------------------------------
//audioPbFR
//-----------------------------------------------------------------------------
/**
 * @brief		playback audio key on FR process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbRW(UINT8 rate)
{
	UINT32 curTime =0;
	UINT32 tmp;
	UINT32 index;
	UINT8 speed;

	HAL_GlobalReadGTimer(&curTime);
	tmp = curTime - gTime;
	speed = rate *4;
	index = ((tmp * bytePerMs * (UINT32)speed)>>9);
	if(index)
	{
		if(audPbCtrl.tmpCnt > index)
		{
			audPbCtrl.tmpCnt = audPbCtrl.tmpCnt  - index ;
		}
		else
		{
			audPbCtrl.tmpCnt =0;
		}
		gTime = curTime;
		HAL_AudPause();
	}
}

//-----------------------------------------------------------------------------
//audioPbKeyRelease
//-----------------------------------------------------------------------------
/**
 * @brief		Exit FW/FR process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbFwFrRelease(void)
{
#if 1
	UINT32 outIndex;
	UINT8 i;

	audPbCtrl.status = AUD_PB_RUN;
	audPbCtrl.speed = AUD_PB_SPEED_NORMAL;

	HAL_AudOutIndexGet(&outIndex);
	//printf("out:%ld\n",outIndex);

	if(outIndex % 512 != 0) {
		if((AUD_RINGBUF_SIZE - outIndex) > (512 - (outIndex%512))) {
			HAL_GprmDramFill(audPbCtrl.AudioAddr +outIndex/2, (512 - (outIndex%512)), 0);
		} else {
			HAL_GprmDramFill(audPbCtrl.AudioAddr +outIndex/2, (AUD_RINGBUF_SIZE - outIndex), 0);
			HAL_GprmDramFill(audPbCtrl.AudioAddr, (512 - (outIndex%512) - (AUD_RINGBUF_SIZE - outIndex)), 0);
		}
		outIndex = (((outIndex >> 9) +1)<<9);
	}

	audPbCtrl.audInIdx = outIndex;
	audPbCtrl.audInIdx = audPbCtrl.audInIdx % AUD_RINGBUF_SIZE;
	HAL_AudInIndexSet(audPbCtrl.audInIdx );
	for(i= 0;i<2;i++) {
		audioPlayWaveChunck(audPbCtrl.fHandle , audPbCtrl.tmpCnt, (UINT32)audPbCtrl.headerSize , audPbCtrl.AudioAddr,1);
	}
	HAL_AudResume();
#else
	audPbCtrl.status = AUD_PB_RUN;
	audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
	audPbCtrl.audInIdx =  0;
	audioPbStart(audPbCtrl.AudioAddr,(UINT32)AUD_RINGBUF_SIZE,(UINT16)0x11,(UINT32) 48000,  (UINT32)256, (UINT8)1);
#endif
}

//-----------------------------------------------------------------------------
//audioPlayWaveChunck
//-----------------------------------------------------------------------------
/**
 * @brief		read one chunck to audio ring buffer
 * @param	fHandle			:	file handle
 * @param	ChunckNo		:	chunck number
 * @param	HeaderOffset		:	WAV header offset
 * @param	audioBufferAddr	:	audio ring buffer address
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPlayWaveChunck(UINT16 fHandle, UINT32 ChunckNo, UINT32 HeaderOffset, UINT32 audioBufferAddr, UINT32 chuncks) USING_0
{
	UINT32  pbAudioChunkAddr;
	UINT32  AudBufCnt =0;
	UINT32 index;
	UINT8 status;
	UINT8 sts;

	HAL_AudRingBufCntRead(&index );
	if(index < 30000) {
		//printf("%d  %ld\n",(UINT16)__LINE__, ChunckNo);
		pbAudioChunkAddr =  audPbCtrl.PBAudioChunkBufAddr/*+K_SDRAM_ImagePlaybackBufSize*/;
		ENTER_CRITICAL( status );
		sts = vfsFileRead(fHandle, K_DOS_DstDram, (UINT32)(chuncks+1) * (UINT32)512 , pbAudioChunkAddr, (UINT32)(ChunckNo<<9));
		EXIT_CRITICAL( status );

		if(sts == FALSE){//xian ++, read file error, #41746
			audPbCtrl.tmpCnt = audPbCtrl.AudChunckCnt;
			return;
		}

		if((dispDevActGet() == 4) && (G_APBupSmpflag)) {	//for upsampling
			audioUpSample(((pbAudioChunkAddr)+(HeaderOffset>>1)),(UINT32)(audioBufferAddr) + (audPbCtrl.audInIdx>>1),K_Audupsmp_1sector);
			audPbCtrl.audInIdx = audPbCtrl.audInIdx+(UINT32)(chuncks) * (UINT32)2048;
		}
		else {
			HAL_GprmDramDma((pbAudioChunkAddr<<1)+HeaderOffset,(UINT32)(audioBufferAddr <<1) + audPbCtrl.audInIdx, (UINT32)(chuncks) * (UINT32)512, 1);
			audPbCtrl.audInIdx = audPbCtrl.audInIdx+(UINT32)(chuncks) * (UINT32)512;
		}
		audPbCtrl.audInIdx = audPbCtrl.audInIdx % AUD_RINGBUF_SIZE;
		HAL_AudInIndexSet(audPbCtrl.audInIdx );
		audPbCtrl.tmpCnt += chuncks;
	}
}

//-----------------------------------------------------------------------------
//audioPbAbort
//-----------------------------------------------------------------------------
/**
 * @brief		audio play abort process
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbAbort(void) USING_0
{
//	UINT32  AudBufCnt;

	//printf("aud pb abort\n");
	ACTIVE_FUNC_CLR();
	audPbCtrl.status = AUD_PB_IDLE;
	audPbCtrl.tmpCnt = 0;
	G_APBupSmpflag = 0; //for upsampling
/*
	//HAL_AudInIndexSet( audPbCtrl.audInIdx );
	HAL_AudRingBufCntRead( 1, &AudBufCnt );
	printf("Cnt : %ld\n",AudBufCnt);
	while( (AudBufCnt > 4)&&(AudBufCnt <32) ) {
		HAL_AudRingBufCntRead( 1, &AudBufCnt );
	}
*/
	//HAL_StopGTimer();
	/*For HDMI beep Voice*/
	HAL_AudPause(); //HAL_AudStop( 1 );
	//Add for Video Play rewind eric@20081015
	DOS_FileReadMakeFatEnable(0, audPbCtrl.fHandle);

	audPbCtrl.audInIdx = 0;
	audPbCtrl.audPbSize = 0;
	audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
	vfsFileClose(audPbCtrl.fHandle, audPbCtrl.name,  K_DOS_ReadFile, 0);
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	osMsgPost(SP1K_MSG_MEDIA_SOUND_FINISH);

	XBYTE[REG_Cdsp_Cdspinten_lo] = red_cdsp;
	XBYTE[REG_Front_Finten_low] = red_front;
}

//-----------------------------------------------------------------------------
//audioPbGlobalInit
//-----------------------------------------------------------------------------
/**
 * @brief		audio play  globale variable initiate
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbGlobalInit(void) USING_0
{
	audPbCtrl.status = AUD_PB_IDLE;
	audPbCtrl.speed = AUD_PB_SPEED_NORMAL;
	audPbCtrl.PBAudioChunkBufAddr  = K_SDRAM_AviPbAudioChunkBufAddr/*+K_SDRAM_ImagePlaybackBufSize*/;
    	audPbCtrl.AudioAddr = K_SDRAM_AviPbAudioBufAddr/*+K_SDRAM_ImagePlaybackBufSize*/;
	audPbCtrl.audInIdx = 0;
	audPbCtrl.audPbSize = 0;
	audPbCtrl.tmpCnt = 0;
	AudPb_GT_pause = 0;
	AudPb_GT_zero = 0;
}

#if 0
//-----------------------------------------------------------------------------
//audioBufSet
//-----------------------------------------------------------------------------
/**
 * @brief		audio play  buffer set for image voice play
 * @param	addr : audio play buffer
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioBufSet(UINT32 addr) USING_0
{
	addr = (((addr>>3)+1)<<3);

    	audPbCtrl.AudioAddr = addr;
	audPbCtrl.PBAudioChunkBufAddr  = audPbCtrl.AudioAddr + K_SDRAM_AviPbAudioBufSize;

	return;
}
#endif

//-----------------------------------------------------------------------------
//audioWAVHdrParse
//-----------------------------------------------------------------------------
/**
 * @brief		parse WAV header
 * @param	*pFormat 	: 	audio data format
 * @param	*pFreq 		: 	audio sample frequce
 * @param	*pBlockAlign	: 	the size of audio block
 * @param	*pChannel 	: 	data channel
 * @retval	0 is SUCCESS , 1 is fail
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 audioWAVHdrParse(UINT16 *pFormat,UINT32 *pFreq,UINT16 *pBlockAlign ,UINT16 *pChannel) USING_0
{
	UINT8  sts ;
	//version4.0@ada@0513 for DCF
//	UINT8  FileType;
//	UINT16 startCluster;
	//patch5.1@ada@Play kinds of audio file format begin
	UINT32 RiffckSize = 0;
	UINT32 fmt_ckSize = 0;
	UINT32 nSamplesPerSec = 0;
	UINT32 nAvgBytesPerSec = 0;
	UINT16 wBitsPerSample = 0;
	UINT16 TempHeaderOffset = 0;
	UINT16 cbSize = 0;
	UINT16 wSamplesPerBlock = 0;
	UINT32 fact_ckSize = 0;
	UINT32 dwSampleLength = 0;
	UINT16 RecordingTime = 0;
	UINT32 LIST_ckSize = 0;
	UINT16 TempChunkOffset = 0;
	UINT32 TempChunkSize = 0;
	UINT16 AudioAnnotation_JpgFileIndex = 0;
	UINT8 AudFormat = 0;

	sts = vfsFileRead(audPbCtrl.fHandle, K_DOS_DstDram, 512, audPbCtrl.PBAudioChunkBufAddr , 0);
	if(!sts)
	{
		//printf("Cannot read WAV header\n");
		return FAIL;
	}

	HAL_DmaSetSramAddr(0x0000); /* 1628 */
	HAL_DmaSetDramAddr(audPbCtrl.PBAudioChunkBufAddr);
	HAL_DmaDo(0,1,512,0,0);

	//Check chunk ID /* 1628 */
	if ((XBYTE[0x3800] != 'R') || (XBYTE[0x3801] != 'I') || (XBYTE[0x3802] != 'F') || (XBYTE[0x3803] != 'F'))	//4 bytes
	{
		//printf("WAV header wrong 1\n");
		return FAIL;
	}

	RiffckSize = audioReadSramLong(0x3804);	//4 bytes
//	printf("RiffckSize = %lx\n",RiffckSize);

	//Check form type
	if ((XBYTE[0x3808] != 'W') || (XBYTE[0x3809] != 'A') || (XBYTE[0x380a] != 'V') || (XBYTE[0x380b] != 'E'))	//4 bytes
	{
		//printf("WAV header wrong 2\n");
		return FAIL;
	}

	//Check sub-chunk "fmt " ID
	if ((XBYTE[0x380c] != 'f') || (XBYTE[0x380d] != 'm') || (XBYTE[0x380e] != 't') || (XBYTE[0x380f] != ' '))	//4 bytes
	{
		//printf("WAV header wrong 3\n");
		return FAIL;
	}

	fmt_ckSize = audioReadSramLong(0x3810);	//4 bytes
	//printf("fmt_ckSize = %lx\n",fmt_ckSize);

	//Check format tag, 2 bytes
	if (XBYTE[0x3814] == 0x01)	//PCM
	{
		*pFormat = K_PCM_Format;
		AudFormat &= 0xfd;	//8 bit per sample
		AudFormat &= 0xf7;	//non-compressed PCM
		AudFormat |= 0x04;	//Sampled data value range (0 ~ 255)
	}
	else if (XBYTE[0x3814] == 0x11)	//ADPCM
	{
		*pFormat = K_IMA_ADPCM_Format;
		AudFormat |= 0x02;	//16 bit per sample
		AudFormat |= 0x08;	//compressed IMA-ADPCM
		AudFormat &= 0xfb;	//Sampled data value range (-32768 ~ 32767)
		AudFormat |= 0x40;	//compression condition for HAL_PlayAud
	}
	else
	{
		//printf("Audio format not support\n");	//Not support u_Law format
		return FAIL;
	}
	//printf("wFormatTag = %x\n",wFormatTag);

	//Check number of channels, 2 bytes
	if (XBYTE[0x3816] == 0x01)
	{
		*pChannel = K_Monaural;
		AudFormat &= 0xfe;
	}
	else if (XBYTE[0x3816] == 0x02)
	{
		*pChannel = K_Stereo;
		AudFormat |= 0x01;
	}
	//printf("nChannels = %x\n",nChannels);

	nSamplesPerSec = audioReadSramLong(0x3818);	//4 bytes

	*pFreq = nSamplesPerSec;
//	printf("Playfreq= %lx\n",*pFreq);

	nAvgBytesPerSec = audioReadSramLong(0x381c);	//4 bytes
	bytePerMs = nAvgBytesPerSec / (UINT32)1000;
	//printf("bytePerMs :%lx\n",bytePerMs);
	*pBlockAlign  = (UINT16)XBYTE[0x3820] | (UINT16)XBYTE[0x3821] << 8;	//2 bytes
//	printf("nAvgBytesPerSec = %lx\n",nAvgBytesPerSec);
//	printf("nBlockAlign = %x\n",*pBlockAlign );

	wBitsPerSample = (UINT16)XBYTE[0x3822] | (UINT16)XBYTE[0x3823] << 8;	//2 bytes
//	printf("wBitsPerSample = %x\n",wBitsPerSample);

	// PCM wave parser fix beg
	//TempHeaderOffset = 0x1c24;
	if(*pFormat ==  K_IMA_ADPCM_Format) {
		TempHeaderOffset = audioReadSramLong(0x3810)+0x3810;	//4 bytes
	}else {
		TempHeaderOffset = audioReadSramLong(0x3810)+0x3814;
	}

	//PCM format don't have these two fields
	if (*pFormat  == K_IMA_ADPCM_Format)
	{
		cbSize = (UINT16)XBYTE[TempHeaderOffset] | (UINT16)XBYTE[TempHeaderOffset + 1] << 8;	//2 bytes
		TempHeaderOffset += 2;
		wSamplesPerBlock = (UINT16)XBYTE[TempHeaderOffset ] | (UINT16)XBYTE[TempHeaderOffset + 1]<<8;	//2 bytes
		TempHeaderOffset += 2;
//		printf("cbSize = %x\n",cbSize);
//		printf("wSamplesPerBlock = %x\n",wSamplesPerBlock);
	}

	if (*pFormat  == K_IMA_ADPCM_Format)
	{
		//Check sub-chunk "fact" ID

		if ((XBYTE[TempHeaderOffset] != 'f') || (XBYTE[TempHeaderOffset + 1] != 'a') || (XBYTE[TempHeaderOffset + 2] != 'c') || (XBYTE[TempHeaderOffset + 3] != 't'))	//4 bytes
		{
			//printf("offset:%x\n",TempHeaderOffset);
			//printf("%bx %bx %bx %bx\n",XBYTE[TempHeaderOffset] ,XBYTE[TempHeaderOffset+1] ,XBYTE[TempHeaderOffset+2] ,XBYTE[TempHeaderOffset+3] );
			//printf("WAV header wrong 4\n");
			return FAIL;
		}
		else
		{
			TempHeaderOffset += 4;	//"fact"
			fact_ckSize = audioReadSramLong(TempHeaderOffset);	//4 bytes
			TempHeaderOffset += 4;
			dwSampleLength = audioReadSramLong(TempHeaderOffset);	//4 bytes
			TempHeaderOffset += 4;
#if 0
			printf("fact_ckSize = %lx\n",fact_ckSize);
			printf("dwSampleLength = %lx\n",dwSampleLength);
#endif
		}
	}

	//  PCM wave parser fix beg
	if (*pFormat  == K_PCM_Format)
	{
		if ((XBYTE[TempHeaderOffset] == 'f') && (XBYTE[TempHeaderOffset + 1] == 'a') && (XBYTE[TempHeaderOffset + 2] == 'c') && (XBYTE[TempHeaderOffset + 3] == 't'))	//4 bytes
		{
			TempHeaderOffset += 4;
			TempHeaderOffset += 4 + audioReadSramLong(TempHeaderOffset);
		}
	}
	//PCM wave parser fix end

	RecordingTime = (UINT16)(dwSampleLength / nSamplesPerSec);	//unit in second
//	printf("RecordingTime = %x\n",RecordingTime);

	//Check sub-chunk "LIST" ID
	if ((XBYTE[TempHeaderOffset] == 'L') && (XBYTE[TempHeaderOffset + 1] == 'I') && (XBYTE[TempHeaderOffset + 2] == 'S') && (XBYTE[TempHeaderOffset + 3] == 'T'))	//4 bytes
	{
		TempHeaderOffset += 4;	//"LIST"
		LIST_ckSize = audioReadSramLong(TempHeaderOffset);	//4 bytes
		TempHeaderOffset += 4;
//		printf("LIST_ckSize = %lx\n",LIST_ckSize);
	}

	//Check sub-chunk "INFO" ID in "LIST" chunk
	if ((XBYTE[TempHeaderOffset] == 'I') && (XBYTE[TempHeaderOffset + 1] == 'N') && (XBYTE[TempHeaderOffset + 2] == 'F') && (XBYTE[TempHeaderOffset + 3] == 'O'))
	{
		TempHeaderOffset += (UINT16)LIST_ckSize;
	}

	//Check sub-chunk "LIST" ID (EXIF WAV file need to be in another LIST chunk)
	if ((XBYTE[TempHeaderOffset] == 'L') && (XBYTE[TempHeaderOffset + 1] == 'I') && (XBYTE[TempHeaderOffset + 2] == 'S') && (XBYTE[TempHeaderOffset + 3] == 'T'))	//4 bytes
	{
		TempHeaderOffset += 4;	//"LIST"
		LIST_ckSize = audioReadSramLong(TempHeaderOffset);	//4 bytes
		TempHeaderOffset += 4;
		//printf("LIST_ckSize = %lx\n",LIST_ckSize);
	}

#if   0
	//Check sub-chunk "exif" ID in "LIST" chunk
    if ((XBYTE[TempHeaderOffset] == 'e') && (XBYTE[TempHeaderOffset + 1] == 'x') && (XBYTE[TempHeaderOffset + 2] == 'i') && (XBYTE[TempHeaderOffset + 3] == 'f'))	//4 bytes
    {
		TempChunkOffset = TempHeaderOffset + 4;	//"exif"
		sts = FALSE;

		//Find the JPG file for audio annotation function
		do
		{
		    if ((XBYTE[TempChunkOffset] != 'e') || (XBYTE[TempChunkOffset + 1] != 'r') || (XBYTE[TempChunkOffset + 2] != 'e') || (XBYTE[TempChunkOffset + 3] != 'l'))	//4 bytes
			{
				TempChunkOffset += 4;
				TempChunkSize = audioReadSramLong(TempChunkOffset);	//4 bytes
				TempChunkOffset += (UINT16)(4 + TempChunkSize);

				if (XBYTE[TempChunkOffset] == 0x00)	//Add 1 bytes(ASCII code terminated with "NULL")
				{
					TempChunkOffset += 1;
				}
			}
			else
			{
				TempChunkOffset += 4;	//"erel"
				TempChunkSize = audioReadSramLong(TempChunkOffset) + 1;	//Add 1 bytes(ASCII code terminated with "NULL")
				TempChunkOffset += 4;
				AudioAnnotation_JpgFileIndex = ((UINT16)(XBYTE[TempChunkOffset + 4] - '0')) * 1000
										+ ((UINT16)(XBYTE[TempChunkOffset + 5] - '0')) * 100
										+ ((UINT16)(XBYTE[TempChunkOffset + 6] - '0')) * 10
										+ ((UINT16)(XBYTE[TempChunkOffset + 7] - '0'));

				sts = TRUE;
				//printf("AudioAnnotation_JpgFileIndex = %x\n",AudioAnnotation_JpgFileIndex);
			}
		} while ((sts == FALSE) && (TempChunkOffset < 0x1e00));

		TempHeaderOffset += (UINT16)LIST_ckSize;
	}
#endif

	//Check sub-chunk "data" ID
	if ((XBYTE[TempHeaderOffset] != 'd') || (XBYTE[TempHeaderOffset + 1] != 'a') || (XBYTE[TempHeaderOffset + 2] != 't') || (XBYTE[TempHeaderOffset + 3] != 'a'))
	{
		//printf("WAV header wrong 5\n");
		return FAIL;
	}
	else
	{
		TempHeaderOffset += 4;
		audPbCtrl.AudChunckCnt  = audioReadSramLong(TempHeaderOffset);
		TempHeaderOffset += 4;
		audPbCtrl.headerSize = (UINT8)(TempHeaderOffset - 0x3800);
//		printf("AudChunckCnt = %lx\n",audPbCtrl.AudChunckCnt);
//		printf("headerSize = %bx\n",audPbCtrl.headerSize);
	}

	//AudChunckCnt = ReadSramLong(0x1C38);
	audPbCtrl.AudChunckCnt = audPbCtrl.AudChunckCnt / 512;
	//DbgPrint("AudChunckCnt=%ld\n", audPbCtrl.AudChunckCnt);

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//audioReadSramLong
//-----------------------------------------------------------------------------
/**
 * @brief		read 4 bytes from sram
 * @param	addr 	: 	sram address
 * @retval	value
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 audioReadSramLong(UINT32 addr) USING_0
{
	  UINT32	value;

	  value	= (UINT32)XBYTE[addr] | (UINT32)XBYTE[addr+1]<<8;
	  value	= value	| (UINT32)XBYTE[addr+2]<<16 | (UINT32)XBYTE[addr+3]<<24;

	  return value;
}

//-----------------------------------------------------------------------------
//audiopbStatusGet
//-----------------------------------------------------------------------------
/**
 * @brief		audio play status get
 * @param	NONE
 * @retval	audio play status
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT8 audiopbStatusGet(void) USING_0
{
	return audPbCtrl.status;
}

#if 0
//-----------------------------------------------------------------------------
//audiopbStatusGet
//-----------------------------------------------------------------------------
/**
 * @brief		image voice  play set
 * @param	addr: voice play adress
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void imageVoiPbSet(UINT32 addr) USING_0
{
	audioPbGlobalInit();
	audioBufSet(addr);
	//audioPbKeyOnPlay(nameIdx);
}
#endif

//-----------------------------------------------------------------------------
//audioPbMsGet
//-----------------------------------------------------------------------------
/**
 * @brief		audio play time get
 * @param	*pTime : play time
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbMsGet(UINT32 *pTime) USING_0
{
	if(audPbCtrl.speed == AUD_PB_SPEED_NORMAL) {
		*pTime  = (audioPbIndexGet() <<9) / bytePerMs;
	} else {
		*pTime  = (audPbCtrl.tmpCnt <<9) / bytePerMs;
	}
}

//-----------------------------------------------------------------------------
//audioPbIndexGet
//-----------------------------------------------------------------------------
/**
 * @brief		audio play index get
 * @param	NONE
 * @retval	chunk  index
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 audioPbIndexGet(void) USING_0
{
	UINT32 index;
	HAL_AudRingBufCntRead(&index );
	if((dispDevActGet() == 4) && (G_APBupSmpflag))
	{	//for upsampling
		if((audPbCtrl.tmpCnt <<9) > (index>>2))
		{
			index = (audPbCtrl.tmpCnt <<9) - (index>>2);

		}
		else
		{
			index = 0;
		}
	}
	else
	{
		if((audPbCtrl.tmpCnt <<9) > index)
		{
			index = (audPbCtrl.tmpCnt <<9) - index;
		}
		else
		{
			index = 0;
		}

	}
	index = (index >>9);

	return index;
}

//-----------------------------------------------------------------------------
//audioPbGTSet
//-----------------------------------------------------------------------------
/**
 * @brief		Set audio play globle time
 * @param	time: GT start time
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
void audioPbGTSet(UINT32 pausetime)
{
	AudPb_GT_zero  += pausetime;
}
//-----------------------------------------------------------------------------
//audioUpSample
//-----------------------------------------------------------------------------
/**
 * @brief		Up audio sample when play
 * @param	sector: 1: 512byte/sector
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
#pragma OT(8, SPEED)
void audioUpSample(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT8 sector)
{
        UINT8 data i /*, j=1*/;
        UINT8 xdata* data ptr;
        SINT16 data dat0, dat1, dat2, dat3, dat4;

        XBYTE[REG_Dram_IndirectAddr_B0] = READ8(dstAddrWord, 3);
        XBYTE[REG_Dram_IndirectAddr_B1] = READ8(dstAddrWord, 2);
        XBYTE[REG_Dram_IndirectAddr_B2] = READ8(dstAddrWord, 1);
        XBYTE[REG_Dram_IndirectAddr_B3] = READ8(dstAddrWord, 0);
        XBYTE[REG_Dram_IndirectPrefetch] = 0x00;

	 //if ( mode==1 )
        //	j = 4;

        do {
	        /* set sram dma address */
	        XBYTE[REG_Cpu_SramRstIdxL] = 0;
	        XBYTE[REG_Cpu_SramRstIdxH] = 0;

	        /* set dram dma address */
	        XBYTE[REG_Dma_BaseAddrL] = READ8(srcAddrWord, 3);
	        XBYTE[REG_Dma_BaseAddrML] = READ8(srcAddrWord, 2);
	        XBYTE[REG_Dma_BaseAddrMH] = READ8(srcAddrWord, 1);
	        XBYTE[REG_Dma_BaseAddrH] = READ8(srcAddrWord, 0);

	        HAL_DmaDo(0, 1, 512+2, 0, 0);
	        if ((sector == 1)&&(G_VPBupSmpflag)) {
			XBYTE[0x3a00] = XBYTE[0x39fe];
			XBYTE[0x3a01] = XBYTE[0x39ff];
	        }

	        i = 0;
	        ptr = 0x3800;

		WRITE8(dat0, 1,  ptr[0]);
		WRITE8(dat0, 0,  ptr[1]);
		WRITE8(dat4, 1,  ptr[2]);
		WRITE8(dat4, 0,  ptr[3]);
		ptr += 2;

	        do {
			/*if( (j==1) && (i==1) && (mode)){
				dat4 = dat0;
			}*/
			dat2 = (dat0 >> 1) +( dat4 >> 1);

			XBYTE[REG_Dram_IndirectRData_Low] = READ8(dat0, 1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(dat0, 0);

			dat1 = (dat0 >> 1) + (dat2 >> 1);

			XBYTE[REG_Dram_IndirectRData_Low] = READ8(dat1, 1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(dat1, 0);

			dat3 = (dat2 >> 1) + (dat4 >> 1);

			XBYTE[REG_Dram_IndirectRData_Low] = READ8(dat2, 1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(dat2, 0);

			WRITE8(dat0, 1,  ptr[0]);
			WRITE8(dat0, 0,  ptr[1]);
			WRITE8(dat4, 1,  ptr[2]);
			WRITE8(dat4, 0,  ptr[3]);
			ptr += 2;

			XBYTE[REG_Dram_IndirectRData_Low] = READ8(dat3, 1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(dat3, 0);
	        } while (--i);

	        srcAddrWord += 256;
        } while(--sector);

}
#if 0
//-----------------------------------------------------------------------------
//audioPbGTGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get audio play globle time
 * @param	NONE
 * @retval	GT start time
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 audioPbGTGet(void)
{
	return AudPb_GT_zero ;
}

//-----------------------------------------------------------------------------
//audioRecTimeGet
//-----------------------------------------------------------------------------
/**
 * @brief		Get audio play globle time
 * @param	NONE
 * @retval	play time
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
UINT32 audioPbGlobalTimeGet(void)
{
	UINT32 time=0;

	if(audPbCtrl.status == AUD_PB_RUN) {
		HAL_GlobalReadGTimer(&time);
	} else if(audPbCtrl.recIdle == AUD_PB_PAUSE) {
		time = AudPb_GT_pause;
	}else {
		HAL_GlobalReadGTimer(&time);
	}

	time = time - AudPb_GT_zero;

	return time ;
}
#endif

