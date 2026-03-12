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
 * @file	  video_clip.h
 * @brief	  header file for video record
 * @author	  tq.zhang
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_CLIP_H__
#define __Video_CLIP_H__

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "app_video_view.h"
#include "sys_mem_init.h"
#include "video_zoom.h"

//for datestamp
#define VD_STAMP_LEFT			0x00
#define VD_STAMP_CENTER 		0x01
#define VD_STAMP_RIGHT			0x02
#define VD_STAMP_TOP			0x00
#define VD_STAMP_VCENTER		0x10
#define VD_STAMP_BOTTOM 		0x20

#define	VID_REC_IDLE			0
#define	VID_REC_RUN			1
#define	VID_REC_PAUSE			2

#define  VID_VGA							((UINT16)640)
#define  VID_QVGA						((UINT16)320)
#define  VID_D1 			((UINT16)720) //((UINT16)848) //modify for D1 size
#define  VID_HD 			((UINT16)1280)
#define  VID_1080p 			((UINT16)1920)
#define	VID_MODE		0 

//video clip & pb use
#define SECPERCHK	((UINT32)11)
#define CHUNKSIZE ( (UINT32)0x01<<SECPERCHK) 
#define VID_PCMFMT			((UINT16)0x0001)
#define VID_ADPCMFMT		((UINT16)0x0011)
#define VIDAUDBUFSIZE ((UINT32)256 *(UINT32)1024)//K_SDRAM_AviCapAudioBufSize//for upsampling


#define VD_REC_YUV_422  1
#define VD_REC_YUV_422_420  0
#define VD_REC_YUV_420 2

#define VD_DUMY_MODE_SIZE0 0
#define VD_DUMY_MODE_COPY_IDX	1
#define VD_DUMY_MODE_COPY_FRAME 2

#define VIDEO_REC_BRC_EN
#define VIDEO_REC_BRC_OV_RECOMP 0
#define DebugDropFrame 1
#define DropBalanceFrame 1

#define VIDEO_REC_SMOOTH_DUMY 1
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define VIDEO_FRONT_CDSP_JPG_DBG	0
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
typedef enum VDStampMode_e{
	VD_STAMP_MODE_DATE = 0,
	VD_STAMP_MODE_IMG,
	VD_STAMP_MODE_IMG_DATE,
	VD_STAMP_MODE_END,
}VDStampMode_t;

typedef enum VDStampRecMode_e{
	VD_STAMP_REC_MODE_ALL_FRAME,
	VD_STAMP_REC_MODE_FST_FRAME,
	VD_STAMP_REC_MODE_END_FRAME,	
	VD_STAMP_REC_MODE_FST_END_FRAME,
	VD_STAMP_REC_MODE_END,
}VDStampRecMode_t;

typedef enum VDParam_e {
	VD_STAMP_ALIG_MODE,
	VD_STAMP_DATATIME_FMT,	  
	VD_STAMP_DATETIME_DIS_X_RATIO,
	VD_FRONT_FPS,
	VD_DUMY_MODE,
	VD_REC_YUV_FORMAT,
	VD_DUMY_SPACE,
	VD_STAMP_DISOFFX,//wangrf++ 1.13  for Stamp Offx Set
	VD_STAMP_DISOFFY,//wangrf++ 1.13 for Stamp Offy Set
	VD_SLREC_MODE, //wangrf++ 2011/7/1 for sleamless rec mode set   0:filesize 1:rectime
    VD_USER_COMMENT_CODE0,                      //20110809
    VD_USER_COMMENT_CODE1,
    VD_USER_COMMENT_CODE2,
    VD_USER_COMMENT_CODE3,
    VD_SLREC_RenameMode,//0:no rename,1:rename all,2:rename except readonly
    VD_SLOW_REC_MOD,
    VD_SLOW_REC_FPS,
    VD_SLOW_PLAY_RATE,
    VD_CAPTURE,
    VD_MMS,
	VD_PARAM_MAX,
} VDParam_t;

typedef enum ResAviHeader_e {
	RES_AVIHeader = 0x01,
	RES_AVIHeader_PCM = 0x03,
	RES_VideoHeader = 0x0b,
} ResAviHeader_t;

typedef struct {
	UINT16 imgW;
	UINT16 imgH;

	UINT16 ftW;
	UINT16 ftH;
	UINT32 ftAddr;

	UINT16 dateW;
	UINT16 dateHOff;	//related to stamp image
	UINT16 dateVOff;
	
	UINT16 stampW;
	UINT16 stampH;
	UINT32 stampAddr;	//A, B buffer
	UINT32 imgAddr; 	//img or background img addr
	
	UINT16 disHOff; 	//related to img
	UINT16 disVOff; 	//
	UINT8  disBufId;	//0: A buffer, 1: B buffer		  

	UINT8 pasteThd;
}videoStampCfg_t;


#define VD_BUF_IDX_EOF (UINT8)(0xFF)

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
UINT8* vidClpLibVerGet(	void);

void videoRecSizeSet (UINT16 size);
void videoRecSizeGet (UINT16* width, UINT16* height) ;
void videoRecAudFmtSet(UINT8 fmt);
 void videoRecQTableSet(UINT8 value);
void videoRecStart(void) ;
void videoRecStop (void);

void videoRecPause (void);
void videoRecResume (void) ;
//UINT8 videoRecAbort(void);
//void videoRecInitGlobal(void);
//void videoRecWriteAudID2Dram(UINT32 addr);
//void videoRecWriteVidIndex2Dram(UINT32 MoviOfset, UINT32 size);// , UINT32 AVIIdxAddr);
//void videoRecWriteAudIndex2Dram(UINT32 MoviOfset);//, UINT32 AVIIdxAddr) ;
void videoFillDramLong(UINT32 address, UINT32 value);
UINT8 videoRecStatusGet(void);
UINT8 IntrvideoRecStatusGet(void);
//UINT8  videoRecWriteIndexToCard(UINT8 integer,UINT32 size);
void videoRecAddJunk(UINT32 addr, UINT32 size);
//void videoRecWriteMovIDToDram(UINT32 movAddr);

UINT32 videoPvFreeAddrGet(void);
UINT32 videoRecTimeGet(void);
void videoRecFrameRateSet (UINT8 rate) ;
void videoRecAviFrameRateSet (UINT8 rate);
//void videoRecAudDataProcess(UINT8 );
//void videoRecAddDummy(UINT32 addr);
//UINT8  videoRecVlcDataProcess(UINT8 Only_WCard);
//UINT32 videoRecAVLCAddrGet(void);
//UINT32 videoRecBVLCAddrGet(void);
void videoRecAudSampleRateSet(UINT16 rt);
void videoRecAudSampleRateGet(UINT16* rt);

void videoRecAudioStsSet(UINT8 en);
UINT8 videoRecAudioStsGet(void);

UINT8 videoRecParamSet(UINT8 paraId, UINT16 value);
UINT16 videoRecParamGet(UINT8 paraId);
//UINT8 videoRecStampEofGet(void);
//void videoRecStampEofSet(UINT8 eofFlag);


void videoRecBitRateSet(UINT32 bitRate);
UINT32 videoRecBitRateGet();
void videoQTabUpdate(UINT8 qval[]);
//void videoEnableBalanceFrame(UINT8 en);

//Interrupt copy
void INTR_videoRecVlcBufSet(void);
UINT8 INTR_videoRecReadVlcSize(void);
void INTR_videoRecVlcBufSwitch(void);
//void INTR_videoRecStampEofSet(UINT8 eofFlag);
void INTR_VidClipBrcSetQtable(UINT8 status);


UINT8 videoStorageWriteSpeedTest(void);
void videoRecTimeUpdate(void);
void videoRecFrontRateInterval(UINT8  frontRate );
UINT8 videoRecFrontRateGet();
//UINT32  audDateSizeGet();
void videoStampSetIntr();
void videoRecFreeBufGet(UINT32* addrW,UINT32* size);
UINT8 videoCustomUpdate(UINT32 dramAddrW);

UINT32 videoRecRemainTimeGet();

void videoRecRtcIntr();
void videoRecIntr();
UINT8 videoRec(void);
void videoTest(UINT8* pt);
UINT8 vidTimeCheckDbgEnd(UINT32 timeLimte,UINT8 *str,UINT32 *timecost);/*for video time test*/
void vidTimeCheckDbgStart();
void vidTimeSliceCheck(UINT8 flag);
#endif /*__Video_CLIP_H__*/
