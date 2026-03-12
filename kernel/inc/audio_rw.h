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
 * @file      audio_rw.h
 * @brief     The header file of audio flow
 * @author    sunyong
 * @since     2010-01-01
 * @date      2010-01-01
*/
#ifndef __AUDIO_RW_H__
#define  __AUDIO_RW_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define  RECORD_FREQUENCE_8KHZ        	1//8khz
#define  RECORD_FREQUENCE_1KHZ        	2//11.025khz
#define  RECORD_FREQUENCE_12KHZ        	3//12khz
#define  RECORD_FREQUENCE_16KHZ        	4//16khz
#define  RECORD_FREQUENCE_22KHZ        	5//22.05khz
#define  RECORD_FREQUENCE_32KHZ       	6//32khz
#define  RECORD_FREQUENCE_44KHZ       	7//44.1khz
#define  RECORD_FREQUENCE_48KHZ       	8//48khz 

//audio rec
#define AUD_SAMPLE_RATE_8K	(UINT32)8000
#define AUD_SAMPLE_RATE_11K	(UINT32)11025
#define AUD_SAMPLE_RATE_12K	(UINT32)12000
#define AUD_SAMPLE_RATE_16K	(UINT32)16000
#define AUD_SAMPLE_RATE_22K	(UINT32)22050
#define AUD_SAMPLE_RATE_32K	(UINT32)32000
#define AUD_SAMPLE_RATE_44K	(UINT32)44100
#define AUD_SAMPLE_RATE_48K	(UINT32)48000

#define AUD_ADPCM_FMT 		1
#define AUD_PCM_FMT 			0

#define AUD_BLOCK_ALIGN 	256

#define AUD_SAMPLE_BIT	16
#define AUD_CHANNEL		1
#define AUD_RECORD_MODE  	0

#define REC_CHUNK_SIZE	((UINT32)1024 )
#define AUD_RINGBUF_SIZE ((UINT32)64 * (UINT32)1024) //K_SDRAM_AviCapAudioBufSize
#define AUD_FRAG_SIZE ((UINT32)64 * (UINT32)1024)

#define AUD_REC_IDLE		0
#define AUD_REC_RUN		1
#define AUD_REC_PAUSE	2


//audio pb
#define AUD_PB_IDLE		0x00
#define AUD_PB_RUN		0x01
#define AUD_PB_PAUSE	0x02
#define AUD_PB_FW		0x04
#define AUD_PB_RW		0x08

#define AUD_PB_SPEED_NORMAL 		1
#define AUD_PB_SPEED_FW1 		2
#define AUD_PB_SPEED_FW2 		4
#define AUD_PB_SPEED_FW3 		8

#define AUD_PB_SPEED_RW1 		2
#define AUD_PB_SPEED_RW2 		4
#define AUD_PB_SPEED_RW3 		8

#define RMV_ABORT_NOISE	28

#define AUDIO_TYPE_NORMAL		0
#define AUDIO_TYPE_VOICE		1

#define AUDIO_VOICE_MEMO_LENTH	30
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appAudRecCtrl_s {
			UINT8	mode;
			UINT8	recStatus;
			UINT16	fHandle;
			UINT8	name[12];
			UINT16	fileIdx;
			UINT32	freq;		
			UINT32	SecAddr;
			UINT32	SecCnt;
			UINT32	SecNum;			
			UINT32	DRAMAudBufCnt;
			UINT32	AudChunkCnt;

			UINT32	freeCluster;
			UINT32	usedCluster;

			UINT32	CurDMATagAddr;
			UINT32	CurSrcAudAddr;
			UINT32	fragBufAddr;

			UINT32	AudioAddr;
			UINT32	AVIHeadAddr;
			UINT16	headerSize;
			UINT32	dataSizeOffset;
			UINT32	time;
			//UINT8   *pHeader;   
}appAudRecCtrl_t;


typedef struct appAudPbCtrl_s {
	UINT8   status;
	UINT8   name[12];
	UINT16  fHandle; 
	UINT32  AudBufIdx;
	UINT32  AudBufCnt;
	UINT32  AudChunckCnt;
	UINT32  PBAudioChunkBufAddr;
	UINT8 	speed;
	UINT8	headerSize;
	UINT32	time;
	UINT32	AudioAddr;
	UINT32	audInIdx;
	UINT32	audPbSize;
	UINT32	tmpCnt ;
}appAudPbCtrl_t; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 //audio record
UINT8 audioRecInit(void);
void audioRec(void);
void audioRecAbort(void);
void audioRecStart(void);
void audioRecStop(void); 
void audioRecPause(void);
void audioRecResume(void);
UINT8 audioRecGlobalInit(void);	
void audioRecBufSet(UINT32 addr);
void audioRecFileIdxSet(UINT16 index);
void imageVoiRecSet(UINT32 addr,UINT16 nameIdx);
void imageVoiRecImgNameSet(UINT8 *name);
UINT8 audioRecStatusGet(void);
void audioRecFreqSet(UINT8 freq) ;
void audioRecFmtSet(UINT8 fmt);
void audioRecGTSet(UINT32 );
UINT32 audioRecGTGet(void);
UINT32 audioRecTimeGet(void);

//audio play
UINT8 audioPbInit(UINT16 ID);
void audioPbSet(UINT32 audAddr,UINT32 audBufSize,UINT16 Format, UINT32 playFreq, UINT32  Block,UINT8 channels ,UINT8 ) ;
void audioPb(void);
void audioPbStart(UINT16 ID);
void audioPbStop(void) ;
void audioPbPause(void);
void audioPbResume(void);
void audioPbFwFrRelease(void);
void audioPbFWSpeedSet(UINT8 speed);
void audioPbRWSpeedSet(UINT8 speed);
UINT8 audioPbSpeedGet(void) ;
void audioPbFW(UINT8 rate);
void audioPbRW(UINT8 rate);
void audioPlayWaveChunck(UINT16 fHandle, UINT32 ChunckNo, UINT32 HeaderOffset, UINT32 audioBufferAddr,UINT32 chuncks) ;
void audioPbAbort(void);
void audioPbGlobalInit(void);
void audioBufSet(UINT32 addr);
UINT8 audioWAVHdrParse(UINT16 *pFormat,UINT32 *pFreq,UINT16 *pBlockAlign ,UINT16 *pChannel);
UINT32 audioReadSramLong(UINT32 addr);
void imageVoiPbSet(UINT32 addr);
UINT8 audiopbStatusGet(void) ;
void audioPbMsGet(UINT32 *pTime);
UINT32 audioPbIndexGet(void) ;
void audioPbGTSet(UINT32 pausetime);
UINT32 audioPbGTGet(void);
void audioRecTypeSet(UINT8 type);
UINT8 audioRecTypeGet(void);
void soundctl(void);

#endif

