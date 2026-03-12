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
 * @file      pb_api
 * @brief     api for playback module
 * @author    tq.zhang
 * @since     2010-05-12
 * @date      2010-05-12
*/
#ifndef __PB_API_H__
#define __PB_API_H__

//=============================================================================
//Header file
//=============================================================================
#include "pb_general.h"
#include "spe_distort.h"
#include "hal_pb.h"
#include "gprm.h"
#include "exif_def.h"

//=============================================================================
//Symbol
//=============================================================================
#define SLIDE_PLAY		1
#define SLIDE_STOP		0

#ifndef PBABORT
#define PBABORT 2
#endif


typedef struct{
	UINT16 x0;
	UINT16 y0;
	UINT16 w;
	UINT16 h;
}multiPbSelWin_t; 

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Data type
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

#define pbDecRotModeGet() pbRotModeGet(0)
#define pbJpgRotModeGet() pbRotModeGet(1)

void pbInit(void);
UINT8 pbParamSet(UINT8 paraId, UINT16 value);
UINT16 pbParamGet(UINT8 paraId);
void pbDispInit(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);
UINT16 pbFileCntGet(void);
UINT8 pbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo);
UINT8 pbSingleImgPlay(UINT16 FileIndex);
UINT8 pbVideoFramePlay(UINT16 FileIndex);
UINT32 pbDispBuffGet(UINT8 opt);
UINT32 pbDispBuffSwitch(void);
UINT8 pbImgDecode(PBFile_t* pFile, UINT32 DecBuff, UINT8 DecOpt);
UINT8 pbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex);
void pbMultiPlay(void);
UINT8 pbMultiPlayShift(UINT16* fileIdx, UINT8 shift);
void pbMultiPlayObjFrameDraw(UINT16 idx, UINT8 enable);
void pbDZoomFactorSet(UINT16 Factor);
UINT32 pbFreeBuffGet(void);
void pbDecodeSizeGet(UINT16* Width, UINT16* Height);
void pbDecodeSizeSet(UINT16 Width, UINT16 Height);
UINT8 pbImgDeCmp(ExifImage_t* ImagePara, UINT32 DecBuff, UINT8 imgType);
//UINT8 pbImgDeCmp(struct Exif_Decompress_Parameter* ImagePara, UINT32 DecBuff, UINT8 ImgType);
UINT8 pbVoiceMemoCheck(UINT16 FileIndex);
void HAL_PbBufAddrSet(UINT32 addr);
void HAL_PbBufAddrGet(UINT32* addr);
void HAL_PbBufHsizeSet(UINT16 width);
UINT8 pbImgResize(PBFile_t* pFile, UINT16 dstWidth, UINT16 dstHeight);
//UINT8 pbEffectSet(PBFile_t* pFile, PBEffectList_t imgEffect);
UINT8 pbEffectSet(PBFile_t* pFile, pbEditType_t imgEffect);	
UINT8 pbFilePlayFadeIn(UINT16 Time);
UINT32 pbUserStartImgGet(UINT32 *vlcAddr);
UINT8 pbImgPanSet(SINT16 stepX, SINT16 stepY);
UINT8 pbDZoomStart(void);
//UINT8 pbDecRotModeGet(void);
UINT8 pbImgDeCmpRot(ExifImage_t* ImagePara, Pb_Dec_Parameter* DecPara, UINT8 fillBlack);
//UINT8 pbImgDeCmpRot(struct Exif_Decompress_Parameter* ImagePara, Pb_Dec_Parameter* DecPara, UINT8 fillBlack);
UINT16 pbExifOrientGet(void);
UINT8 pbImgRotate(PBFile_t* pFile);
UINT8 pbSlideShowStart(void);
UINT8 pbSlideShowAbort(void);
#define pbSlideShowStop pbSlideShowAbort
UINT8 pbSlideShowContinue(void);
UINT8 pbSlideShowStateGet(void);
void pbSlideShowStateSet(UINT8 state);
UINT8 pbSlideShowProc(UINT16* FileIndex);
void pbDZoomPause(void);
UINT8 pbDecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddr, UINT32 endAddr, UINT16 *frameWidth, UINT16 *frameHeight );

void pbDispRoiSizeGet(UINT16 *pHOff, UINT16 *pVOff, UINT16 *pHSize, UINT16 *pVSize);

void pbDispBufModeSet(UINT8 mode);
UINT8 pbDispBufModeGet(void);
void pbDZoomKeySet(UINT8 value);
UINT8 pbDZoomKeyGet(void);
void pbCallbackReg(UINT32 id,void *pfunct);
UINT8 pbCheckAbort( UINT8 imgType,UINT16 line);

void pbDispQVSizeSet(UINT16 Width, UINT16 Height);
void pbDispQVSizeGet(UINT16 *Width, UINT16 *Height);
UINT8 pbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType);
UINT8 pbRotModeGet(UINT8 option);
void pbCropDispBuffer2ABBuf();
multiPbSelWin_t * pbMultiPosGet(void);

UINT8 pbEdit_ImgDec(PBFile_t* pFile,pbEditType_t imgEffect,PbCropImage_t *cropRec);
UINT8 pbEdit_Save( pbEditType_t imgEffect,PbCropImage_t cropRec );
void pbDispDevW_H_Get(UINT16 *width,UINT16 *height,UINT16 srcWidth,UINT16 srcHeiht);
UINT8 pbFileInfoGetExt(UINT16 FileIndex, PBFile_t* FileInfo,UINT8 opt);
#if 0//def MULTI_FOLDER_AS_ONE  //xian ++ 20080806
UINT32 pbFileTotalCntGet();
UINT32 pbFirstFileIndexGet();
#endif

UINT8 pbEditUserCommentCodeModify(PBFile_t* pFile,  UINT8 *cmCode, UINT8 len);
UINT8 pbEditUserCommentCodeGet(PBFile_t* pFile,  UINT8 *cmCode, UINT8 len);
#endif /*__PB_API_H__*/


