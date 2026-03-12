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
 * @author   wangjg
 * @since     2011-11-01
 * @date      2011-11-01
*/

#ifndef __TEMPLATE_HDR_H__
#define __TEMPLATE_HDR_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "common.h"
#include "initio.h"
#include "reg_jpeg.h"
#include "reg_def.h"
#include "dos32.h"
#include "gpio.h"
#include "main.h"
#include "timer.h"
#include "intr.h"
#include "os_api.h"
#include "os_main.h"
#include "os_msg.h"
#include "utility.h"

#include "sd_def.h"
#include "sdfunc.h"
#include "ae_api.h"
#include "awb_api.h"

#include "hal.h"
#include "hal_device.h"
#include "asicreg.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "hal_audio.h"
#include "hal_gprm.h"
#include "hal_jpeg.h"
#include "hal_global.h"
#include "hal_disp.h"
#include "hal_front.h"
#include "hal_front_para.h"

#include "quantdef.h"
#include "cardlink.h"
#include "doslink.h"
#include "dosvar.h"
#include "func_ptr.h"
#include "sdramhw.h"
#include "smcfunc.h"
#include "fileIndex.h"
#include "snap_qtable.h"
#include "gprm.h"
#include "smcfunc.h"
#include "pb_debug.h"
#include "date_stamp.h"

#include "disp_api.h"
#include "dcf_api.h"
#include "vfs_api.h"
#include "dbg_def.h"
#include "dbg_wdt.h"
#include "dbg_assert.h"
#include "front_logi.h"
#include "pv_task.h"
#include "pv_photoframe.h"

#include "video_recDbg.h"
#include "video_mem.h"
#include "video_clip.h" 
#include "video_dtstmp.h"
#include "video_debug.h" 
#include "video_slclip.h"
#include "sd_bg.h"
#include "stamp.h"

#include "dist_func.h"
#include "util_api.h"

#include "pv_task.h"
#include "sp1k_pv_api.h"
#include "snap_qtable.h"
#include "sp1k_pb_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_dist_api.h"
#include "sp1k_disk_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_aud_api.h"

#include "app_still_view.h"
#include "app_video_view.h"
#include "app_still_play.h"
#include "app_ui_osd.h"
#include "app_ui_para.h"

#include "cmd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define AVSYNCTEST 1
#define VLC_BUF_CHG_EN	1
#define VLC_AE_INFO_EN	0
#define VLC_MOE_INFO_EN	0
#define DmaWaitOpt 0
#define JPG_1080P_Rec  1
#define VIDEO_SHOW_AE_INFO_EN	0
//#define VIDEO_ZOOM_FACTOR_LOOKUP_TABLE	1 //Digital zoom used look up table switch

#define VideoNoUsed 0

#define BLOCK_ALIGN	((UINT32)512)//256
#define SAMPLE_BIT	16
#define CHANNEL		1
#define REC_MODE		0
#define SMPPERBLK ((BLOCK_ALIGN - 4 ) * 2 + 1)
#ifdef VIDEOHUFFMAN
#define HUFFMANSIZE 0x1A4
#else
#define HUFFMANSIZE 0x00
#endif
 #if 0
#define A_BIT		0
#define B_BIT		1
#define C_BIT		2
#define D_BIT		3
#define E_BIT		4
#define F_BIT		5
#define G_BIT		6
#define H_BIT		7
#define I_BIT		8
#define J_BIT		9
#define K_BIT		10
#define L_BIT		11
#define M_BIT		12
#define N_BIT		13
#define O_BIT		14
#define P_BIT		15
#define Q_BIT		16
#define R_BIT		17
#define S_BIT		18
#define T_BIT		19
#endif
//for date stamp


#define VD_STAMP_FMT_YYYYMMDD 0x00
#define VD_STAMP_FMT_DDMMYYYY 0x01
#define VD_STAMP_FMT_MMDDYYYY 0x02
#define VD_STAMP_REE_MODE_TWO_LINE 0x0100 //wangrf++ 1.13 for stamp Two Line
#define VD_STAMP_FMT_HHMM	  0x10
#define VD_STAMP_FMT_HHMMAM   0x20
#define VD_STAMP_FMT_HHMMSS   0x30
#define VD_STAMP_FMT_STR_WITH_DATE			  0x40
#define VD_STAMP_FMT_STR_ONLY					  0x80 

#define VID_STAMP_FNT_NUM 21	//date num=10(YYYY/MM/DD) time num=8(hh:mm:ss)
#define VID_STAMP_FNT_MAX_H_SIZE	DATE_STAMP_H_SIZE_2//should multi of 8
#define VID_STAMP_FNT_MAX_V_SIZE	DATE_STAMP_V_SIZE_2
#define VID_STAMP_MAX_WIDTH  720 //K_VIDEOCLIP_WIDTH 
#define VID_STAMP_MAX_HIGH	VID_STAMP_FNT_MAX_V_SIZE
#define K_SDRAM_AviStampBufSize ((UINT32)VID_STAMP_MAX_WIDTH*VID_STAMP_MAX_HIGH)
#define K_SDRAM_AviStampFontBufSize ((UINT32)VID_STAMP_FNT_MAX_H_SIZE*VID_STAMP_FNT_MAX_V_SIZE*DATE_STAMP_TEMP_NUM)
#define K_SDRAM_AviStampBufTotalSize ((UINT32)K_SDRAM_AviStampBufSize*5+K_SDRAM_AviStampFontBufSize)  //A+B+C+D+img+Font
//#define VID_VLC_BOUND_SIZE ((K_SDRAM_AviCapVLCBufSizeA-136)<<1)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define VIDREC_JPEG_EOF_EN 1 //rf(20111010)++ for block bad frame and datestamp alling side 

#define _MAX(x,y) ((x)>(y)?(x):(y))
#define _MIN(x,y) ((x)<(y)?(x):(y))
#define _ABS(x,y) ((x)>(y)?(x)-(y):(y)-(x))

#define _VIDEO_INTR_DISABLE() 	XBYTE[REG_Dram_VlcIntEn] &=~0x03;XBYTE[REG_Jpeg_Intr_Enable] &= ~0x02
#define _VIDEO_INTR_ENABLE() 	XBYTE[REG_Dram_VlcIntEn] |= videoRegVlcIntEn;XBYTE[REG_Jpeg_Intr_Enable] |= videoRegJpgEn	
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appVidClipCtrl_s {
			UINT8 mode;
			UINT8 recIdle;
			UINT16 fHandle;
			UINT16 fHandle_idx;
			UINT16 fHandle_hdr;
			UINT8 name[12];
			UINT8 name_idx[12];
			UINT8 name_hdr[12];
			UINT8 Qtable;
			UINT32 time;
			UINT16 imgWidth;
			UINT16 imgHight;
			UINT32 Compsize;		
			UINT32 SecAddr;
			UINT32 idxSize;
			UINT32 hdrSize;
			UINT32 SecCnt;
			UINT32 DRAMAudBufCnt;
			UINT32 MoviOfset;
			UINT32 VidChunkCnt;
			UINT32 AudChunkCnt;

			UINT32 freeCluster;
			UINT32 usedCluster;

			UINT32 DramDMAaddr;
			UINT32 CurDMATagAddr;
			UINT8 CurDMATagStatus;
			UINT32 CurSrcImgAddr;
			UINT8 CurSrcImgStatus;
			UINT32 totalChuck;	
			UINT32 usedSize;
			UINT32 AudioAddr;
			UINT32 AVIHeadAddr;
			UINT32 AVIIdxAddr;
			UINT8 audFmt;
			UINT16 sampRate;
			UINT8 frameRate;
			UINT8 slowRecFps;
}appVidClipCtrl_t; 

typedef struct videoRecBufTab_s {
	UINT8 dumyCnt;
	UINT8 next;
}videoRecBufTab_t;
typedef struct{
UINT32 vidTimeCheckCurr;
UINT32 vidTimeCheckLast;
UINT32 vidTimeCheckSliceLast;
UINT32 vidTimeCheckOsd;
UINT32 vidTimeCheckHost;
UINT32 vidTimeCheckCard;/*for video time test*/
}vidRecDbgTimeCheck_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 videoRegVlcIntEn,videoRegJpgEn; 
extern UINT8 seamLessLpRecStop;

extern appVidClipCtrl_t vidClipCtrl;
extern UINT16 video_zoomFactor ;
//extern appVideoZoom_t VideoZoomCtrl;


extern UINT32 VID_VLC_BUF_STATUS;
extern UINT32 VID_VLC_BUF_STATUS_HIGH;
extern UINT32 audBytesPerSec,vidBytesPerSec;
extern UINT16 g_vidParam[];
extern UINT8 VideoRecBegin;
extern UINT8 g_bNoAudio;
extern UINT8   G_FrameRate;
extern UINT32 vid_ByteAddr,vid_TmpNum;
extern UINT32 vid_AudOutIdx;
extern UINT8 vlcBufNum;
extern UINT32 vidVlcSize[];
extern UINT8 vid_FragCnt;
extern UINT32 vidVlcBufChgCnt;
extern UINT8 flip;
extern UINT8 dropFrameFlagA;
extern UINT8 dropFrameFlagB;
extern UINT32 vidVlcDMACnt;
extern UINT32 DummyFrameCnt;
extern UINT32 totalFrameCnt;
extern UINT32 totalDummyFrame;
extern UINT8 vlcABSwitch;
extern UINT32 dropFrmCnt;

extern UINT32 vlcOverflowCnt;
extern UINT8 QIdxMax,QIdxMin;
extern UINT32 dumyIdxTotal;

extern UINT8 g_dumyCopy;
extern UINT8 g_bReserveStampFrameSize;
extern UINT8 vidDateStampSts; //xian ++ 20081029

extern UINT32 vidIndexNum;


extern UINT8 G_VideoVlcBufNum;
extern UINT8 videoQValueTab[];

extern SINT8 alc_gain_idx ; //recover alc_gain_idx
extern UINT8 alc_default_gain_index; //recover alc_gain_idx

extern UINT8 g_vidAviFrameRate;

extern videoRecBufTab_t g_vlcIdxTab[];
extern UINT8 idxA;
extern UINT8 idxB;
extern UINT8 idxFree;
extern UINT8 idxVlcHead;
extern UINT8 idxVlcTail;
extern UINT8 idxVlcDumy;
extern UINT8 idxVlcPreTail;
extern UINT16 vlcBufFrameCnt;
extern UINT8 seamLessLpRecEn;
extern UINT32 videoSeamlessFilesize;
extern UINT32 videoSeamlessRecTime;
//extern UINT8 slflag;
extern UINT32 g_bitRate;
extern UINT32 g_brcTargetSize;
//extern UINT32 slopenfiletimeflag; //flag for sl open new video file 
//extern UINT32 slopenfiletime; 	// the time for sl open new video file
//extern UINT32 VID_TOTAL_FRAGSIZE;
//extern UINT32 FRAG_BUF_SIZE;
//extern UINT32 FRAGBUFCNT;
//extern UINT32 SEC_PER_FRAG_BUG;
extern UINT32 g_vidCustomUpdateEntry;
#if AVSYNCTEST
extern UINT32 vidChunkCntTest;
#endif 

extern UINT32 videoSeamlessFilesize;
extern UINT32 videoSeamlessRecTime;
extern UINT32 slRecTime; //time for sl rec :judge when to stop one video file
extern UINT32 slopenfiletimeflag; //flag for sl open new video file 
extern UINT32 slopenfiletime; 	// the time for sl open new video file;
extern UINT8 deletefilenum;
//extern	SIMAGE xdata pbFile;
extern UINT8 sldeletefinename[];
extern UINT32 ringBufSize; //rf++
extern UINT8 G_VideoVlcBufNum;
extern  UINT32 VID_TOTAL_FRAGSIZE;
extern UINT32 FRAG_BUF_SIZE;
extern UINT16  G_USR_FileIndex;
extern UINT8 seamLessLpRecStop;

extern UINT8 G_Card_Type;
extern UINT32 dtnaddr[];
extern UINT16 dsParam[];	//xian 20090522

extern UINT16 dzoomDispXZoomFactor;
extern UINT16 dzoomDispYZoomFactor;

extern UINT8 vidDateStampSts; //xian ++ 20081029
extern UINT8 g_bReserveStampFrameSize;
extern videoStampCfg_t g_vidDateStamp;

//extern UINT32 G_Video_Bound_Size;

#if (VLC_MOE_INFO_EN == 1)
extern UINT8 moeDbgTrig;
#endif

#if (VIDEO_FRONT_CDSP_JPG_DBG == 1)
extern UINT8 videoStatisticsEn;
extern UINT32 videoJpgTriggerCnt;
extern UINT32 videoFrontInputCnt;
#endif
extern UINT32 chkFlg;
extern UINT8 blendingMode;

extern UINT16 g_vidParam[];

extern UINT8 videoDtFlag[];
extern UINT32 videoBrcSizeAvg;
extern UINT32 vidFragBufSize;
extern DOS dos;
extern UINT32 videoClusterSize;
extern BgFileObj_t bgFileMov,bgFileHdr,bgFileIdx;
extern UINT8 vidBgEn;
extern UINT8 vidBgEnDef;
extern SINT32 audDataSize;
extern UINT32 vidStorageSpeed;
extern UINT32 videoRecDbgTime;

extern UINT8 *brcQTbls;
extern UINT32 vidBrcTagSize;
extern UINT8 g_jpgQValue;
extern UINT32 vidVlcBufSize;
extern UINT8 vidTestCnt;
extern UINT32 vidFileMaxSize;
extern UINT32 vidSLDbgTime,vidSLDbgVlcCnt0,vidSLDbgVlcCnt1;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern UINT8 HAL_JpgZFactorCropSet(UINT16 hfactor,	UINT16 vfactor,UINT16 SrcWidth,UINT16 SrcHeight,UINT8 distClampx,UINT8 distClampy,UINT16 DstWidth,UINT16 DstHeight,UINT8 option);
extern void videoBrcUpdateQValBuf();
extern UINT8 HAL_GlobalICVerGet(void);
extern void DOS_FileOrderTblInit(void); //xian ++ 20080806
extern UINT8 dramFileSave(UINT8 *name,UINT32 startAddr,	UINT32 size);
extern void DOS_FileOrderWrite(UINT16 OrderIndex, UINT16 FileTrueIndex) ;

extern void hw_AudRecord(UINT32 sdramAddr,UINT16 samplingRate,UINT8  audFormat) ;

extern void jpegYQTableGen(SINT32 quality,	UINT8 *qtable);
extern void jpegUVQTableGen(SINT32 quality, UINT8 *qtable);
extern void INTR_halJpegQTableWrite(
	UINT8 QTableIndex,
	UINT8 *yQTable_0,
	UINT8 *uvQTable_0,
	UINT8 zigzagOpt
);
void videoRecWriteAudID2Dram(UINT32 addr);
void videoRecWriteVidIndex2Dram(UINT32 size,UINT8 isUpdateMovi);/*UINT32 MoviOfset, */// , UINT32 AVIIdxAddr);
void videoRecWriteAudIndex2Dram(UINT32 chunkSize);//UINT32 MoviOfset);//, UINT32 AVIIdxAddr) ;
void videoRecAudDataProcess(UINT8 );
UINT8  videoRecVlcDataProcess(UINT8 Only_WCard);

void videoRecYUVFormatSet(UINT8 yuvformat);
UINT8 videoRecDma0Wait(UINT8 mask);
void videoAddBalanceDmyFrame(UINT32 MovOff, UINT32 size, UINT8 num);
UINT32 vidCustomUpdate2Vlc(UINT8 idx, UINT32 vlcSize);
void vidCustomUpdate2Idx(UINT32 idxAddr);
//function when video rec abort

void videoDebugAviInfo(UINT32 addr);


void videoRecVlcFreeBufAdd(void);

void videoStampSet();
void videoRecStampSyncRtc();
void videoRecEofIntrEnable(void);
void videoRecEofIntrDisable(void);

UINT8 videoRecInit();
void videoRecInitHdr(void);
UINT8 videoRecInitSeamless();
void videoRecInitGlobal(void);
void videoRecInitMem(void);

UINT8 videoRecAbort(void);
UINT8 videoRecAbortSeamless (void);
void videoAbortInitPvMem(void);
void videoAbortPvRegAddrUpdate(UINT8 frameBufNum);
UINT8 videoAbortMjpg();
UINT8 videoAbortCheckAvi();
UINT8 videoAbortWriteMovieData();	
UINT8 videoAbortModifyHeader();
UINT8 videoAbortModifyMovie();
UINT8 videoAbortModifyIndex();
UINT8 videoAbortCloseFile();
UINT8 videoAbortCloseFileBg();

void videoRecWriteIndex();
void videoAbortWriteIndex();
void videoRecSaveVlc();
UINT8 videoRecFragSaveCard(UINT32 minSize);
UINT8 videoRecWriteToFragBuf(UINT32 addrbyte,UINT32 size);

UINT8  videoRecWriteIndexToCard(UINT8 integer,UINT32 size);
void videoRecAddJunk(UINT32 addr, UINT32 size);
void videoRecWriteMovIDToDram(UINT32 movAddr);
void videoRecTimeSync();
UINT8 videoRecCheckSpace(UINT32 vlcSize);
UINT8 videoRecCheckSpaceBg(UINT32 vlcSize);
UINT8 videoRecCheckSpaceSeamless(UINT32 vlcSize);
void videoRecAddDumy_Size0(UINT32 size,UINT8 cnt);

UINT8 videoRecRemoveEndVoice(UINT32 idxSize, UINT32 rmvIdxNum);
void videoRecBrcAdjust();
void videoBrcQTabUpdate(UINT8 vlcBufIdx, UINT32 frameSize);
void videoBrcTgSizeSet(UINT32 bPerSec, UINT8 fps);
void videoBrcQTabGen(UINT32 dramAddrWord);

void videoDebugFrameInfo(UINT32 addr);

void vidDbgDataFlow(UINT8 src,UINT8 dst,UINT32 size);
void vidDbgDataFlowInit();
void vidDbgDataFlowSave(UINT8 fname[]);

UINT8 videoRecRenameSeamless(UINT8 protectedFilter,UINT8 typeFilter);

#endif  /* __TEMPLATE_HDR_H__ */
