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
 * @file	  video_pb.h
 * @brief	  header for playback video
 * @author	  tq.zhang
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_PB_H__
#define __Video_PB_H__

#include "general.h"
#include "knl_func_opt.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define FUNC_KNL_VDPB_LIB_EN	FUNC_KNL_VDPB

#define	VID_PB_IDLE			0
#define	VID_PB_RUN				1
#define	VID_PB_PAUSE			2
#define	VID_PB_FW				3
#define	VID_PB_RW				4
#define  JPG_1080P_PB				0
#define VID_PB_SPEED_NORMAL 			1


//video clip & pb use
#define SECPERCHK	((UINT32)11)
#define CHUNKSIZE ( (UINT32)0x01<<SECPERCHK)
#define VID_PCMFMT			((UINT16)0x0001)
#define VID_ADPCMFMT		((UINT16)0x0011)
#define AUDBUFSIZE ((UINT32)64 *(UINT32)1024)//K_SDRAM_AviCapAudioBufSize//for upsampling
#ifdef VIDEOHUFFMAN
#define HUFFMANSIZE  0x1A4
#else
#define HUFFMANSIZE  0x00
#endif

#define VIDEO_PB_BRC_EN

//#define VD_PB_HDMI_EN	1

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
typedef enum{
	VD_PB_STS_START = (UINT8)0,
	VD_PB_STS_IDX_FOUND = (UINT8)1,
	VD_PB_STS_VLC_RDY = (UINT8)2,
	VD_PB_STS_JPG_WAIT_VLC = (UINT8)3,
	VD_PB_STS_JPG_WAIT_VLC_RDY = (UINT8)4,
	VD_PB_STS_JPG_TRIG = (UINT8)5,
	VD_PB_STS_LAST_FRAME_RDY = (UINT8)0xff,
}vdPbSts_e;

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
typedef struct vdPbSizeMatchTable_s {
	UINT8 fact;
	UINT16 disW;
	UINT16 disH;
}vdPbSizeMatchTable_t;

typedef struct appVidPbCtrl_s{
	UINT8	Idle;
	UINT16	fHandle;
	UINT8	name[12];
	UINT8	speed;
	UINT32	  chunckCnt;
	UINT32	  vidFrameTime;
	UINT32	playAudTime;
	UINT8	feedData;
	UINT8	mediaType;
	UINT8	audDown;
	UINT8	vidDown;
	UINT8	showBG;

	UINT32	audInIdx;
	UINT32	lastAudCluster;
	UINT32	lastAudFileIdx;
	UINT32	audAddr;
	UINT32	pbAudioChunkAddr;
	UINT32	nextAudIdx;
	UINT32	lastAudIdx;
	UINT8	playAudFlg;

	UINT32	AVIIdxAddr;
	UINT32	lastVidFileIdx;
	UINT32	lastVidCluster;
	UINT8	VLCFlip;
	UINT8	dropVidFlg;
	UINT32	firstVidBlock;
	UINT32	TimeExceed;
	UINT32	AVIIdxRamAddr;
	UINT32	lastVidIdx;
	UINT32	NextVidIdx;
	UINT32	playVidTime;
	UINT32	 msPerFrame;
	UINT32	vidChunkCnt;
	UINT32	audChunkCnt;
	UINT32	audChunk;
	UINT32	vidChunk;
	UINT32	 audFmt;
	UINT32	 audChnnel;
	UINT32	 audFrq;
	UINT32	 audAlign;
	UINT32	 audBit;
	UINT8	dummyFrame;
	UINT32 frameRate;

	UINT16	fHandle_idx;
	UINT32	fstIdxInBuf;
	UINT32	lstIdxInBuf;
	UINT32	idxReadOff;

	UINT8 dispFlip;	//dispFlipe which displayed

	UINT32 frameTime[3];//pts
	UINT8 frameDummyCnt[3];

	UINT32 jpgStartAddr;
	UINT32 secAddr_jpgEnd;

}appVidPbCtrl_t;
extern appVidPbCtrl_t xdata vidPbCtrl;//for FPGA test video

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
UINT8* vidPbLibVerGet(void);
UINT8 videoPbInit(UINT16 ID);
void videoPb(void) ;
void videoPbAbort(void) ;
void videoPbStart(UINT16 ID);
void videoPbStop(void);
void videoPbPause(void);
void videoPbResume(void);
 void videoPbRelease(void);
void videoPbFWSpeedSet(UINT8 speed);
void videoPbRWSpeedSet(UINT8 speed);
 void videoPbFW(void);
 void videoPbFR(void);
UINT8 videoPbAudioChunck(UINT32 AVIIdxAddr, UINT32 audioBufferAddr, UINT32 AudioIdx) ;
UINT32 videoPbSearchAVIIndex(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx,UINT32 indexcount, UINT8 type);
UINT32 videoPbSearchAVIIndexBackward(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx,UINT32 indexcount,	UINT8 type ) ;
UINT8 videoPbReadAVIIdx(UINT32	AVIIdxRamAddr, UINT16 fHandle);
void videoPbGlobalInit(void) ;
UINT32 videoPbReadDword(UINT32 dramAddr) ;
UINT8 AVIHeaderParse(void);
void videoPbMsGet(UINT32 *pTime);
void SetPbBufHsize(UINT16 width);
UINT8 videoPbStatusGet(void) ;
UINT8 videoPbSpeedGet(void);
void videoPbSizeGet(UINT16 *phSize, UINT16 *pvSize);
UINT8 videoPbFrameRead(UINT32 AVIIdxAddr, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip);
UINT32 videoPbTimeGet(void);
void videoPbStreamRead(UINT16 fHandle, UINT32 dataOffSet, UINT32 maxSize);
UINT8 videoPbVLCRingBufCntGet(UINT32 *cnt);
void videoPbAudioPlay(void);
UINT32 videoPbAviLengthGet(void);
void videoPbSlowFWSpeedSet(UINT8 speed);
void videoPbSlowRWSpeedSet(UINT8 speed);
UINT32 videoPbTimeLengthGet(void);

#endif /*__Video_PB_H__*/
