/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/

#ifndef __APP_STILL_PLAY_H__
#define __APP_STILL_PLAY_H__

#include "pb_api.h"
//=============================================================================
//Header file
//=============================================================================

//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
/*Playback mode initialize mode*/
#define SP1K_PB_INIT_FULL		0
#define SP1K_PB_INIT_SMART		1

#define SP1K_FILE_PROC_ONE		0	
#define SP1K_FILE_PROC_ALL		1

#define PLAY_MAX_ZOOM_FACT 400
#define PLAY_MIN_ZOOM_FACT 100
#define PLAY_MAX_ZOOM_LOG 100
#define PLAY_MIN_ZOOM_LOG 25
#define PLAY_ZOOM_logStep 5
//------------------------------------------------------------------------------
//Data type
//------------------------------------------------------------------------------
typedef struct tagIMAGE {
//	UINT32 dwSize;			
//	UINT32 dwExpTime[2]; 		// ExpTime = wExpTime[0] / wExpTime[1]
//	UINT32 dwFNum[2];    			// F Num = wFNum[0] / wFNum[1]
	UINT16 dwIndex;				// Image Index
//	UINT32 ulMPEG4BitRate; 		// For MPEG4,indicate BitRate
	UINT16 wType;				// Image Type: Jpeg, Avi, Tiff, Audio, Unknown ...
	UINT8 dzFactor;
//	UINT16 wLastType;			// Last File Image Type: Jpeg, Avi, Tiff, Audio, Unknown ...
//	UINT16 wDcfDir;				// DCF's Dir  Number
//	UINT16 wDcfFile;				// DCF's File Number
	UINT16 wHSize;     			// Image's Horizontal Size
	UINT16 wVSize;     			// Image's Vertical Size
	UINT8 bVoiceMemo;
//	UINT16 wIso;		 			// ISO Value
	UINT8 bImageProtect;		// Is Image Locked
//	UINT16 nAVIFrameRate;		// If AVI File, frame rate=15, 30... fps
//	UINT16 DpofPrintNum;		// DPOF Print Number
//	BOOL bExistDpof;				// If this Standard,Size or Index Ixist DPOF Index value is TRUE else FALSE.
//	BOOL bDcfFile;				// Is Image DCF Compatible
//	BOOL bMainImgOK; 			// Restore Main OK or Fail
//	BOOL bThumbImgOK; 		// Restore Thumbnail OK or Fail
	BOOL bFileErr;				// Is current file error
//	UINT8 nDimRatio;				// Dimension Ratio 0:recognize fail RATIO_4X3:4x3 RATIO_3X4:3x4()  square=RATIO_4X3
//	UINT8 nSizeLevel;     			// L0=2272x1704 L1=2048x1536....L4=640x480 L5=others
//	UINT8 DirName[9];
	UINT8 FileName[12];  
//	xdcfTime_t Time;
} SIMAGE, * PSIMAGE;
extern SIMAGE xdata pbFile;
//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
typedef enum {
	PB_IMG_ORIENT_UNKNOWN = ((UINT8)0x00),
	PB_IMG_ORIENT_4X3 = ((UINT8)0x01),
	PB_IMG_ORIENT_3X4 = ((UINT8)0x02),
	PB_IMG_ORIENT_3X2 = ((UINT8)0x04),
	PB_IMG_ORIENT_2X3 = ((UINT8)0x08),
	PB_IMG_ORIENT_16X9 = ((UINT8)0x10),
	PB_IMG_ORIENT_9X16 = ((UINT8)0x20),
	PB_IMG_ORIENT_HORIZONTAL = ((UINT8)(PB_IMG_ORIENT_4X3 | PB_IMG_ORIENT_3X2 | PB_IMG_ORIENT_16X9)),
	PB_IMG_ORIENT_VERTICAL = ((UINT8)(PB_IMG_ORIENT_3X4 | PB_IMG_ORIENT_2X3 | PB_IMG_ORIENT_9X16)),
	PB_IMG_ORIENT_16V9 = ((UINT8)(PB_IMG_ORIENT_16X9 | PB_IMG_ORIENT_9X16)),
	PB_IMG_ORIENT_3V2 = ((UINT8)(PB_IMG_ORIENT_3X2 | PB_IMG_ORIENT_2X3)),
	PB_IMG_ORIENT_4V3 = ((UINT8)(PB_IMG_ORIENT_4X3 | PB_IMG_ORIENT_3X4)),
} pbImageOrientDim_t;

void appStillPlay(UINT16 msg);
UINT8 appStillPlayInit(UINT8 InitMode);
UINT8 appStillPlayNext(UINT8 Dir);
UINT8 appAudioPlayBGShow(void);
UINT16 appFileDelete(UINT8 mode);
UINT8 appFileAttrSet(UINT16 FileIndex, UINT8 ReadOnly, UINT8 mode);
UINT8 appCopy2Card(UINT8 mode, UINT8 direction);
void appPbPreStateSet(UINT8 sts);
UINT8  appPbPreStateGet(void);
UINT8 appPbFileInfoUpdate(void);
UINT8 appPbImgResize(PBFile_t* fileInfo, UINT8 size);
void pbAbortMsgSeek();
UINT8 appPbFileAspectRatioGet(void);
void appPbJpgCrop(void);
UINT8 appPbAbortCheck();   

#endif /*__APP_STILL_PLAY_H__*/


