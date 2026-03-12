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
 * @file	  pb_photoFrame.h
 * @brief	  api for photoframe
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/
#ifndef __PB_PHOTOFRAME_H__
#define __PB_PHOTOFRAME_H__
#include "exif_def.h"

#include "pb_general.h"

UINT8 pbPhotoFrameView(UINT16 fileIndex,  UINT32 frameResIdx);
UINT8 pbPhotoFrameSave(UINT16 fileIndex, UINT32 frameResIdx);
UINT8 pbDecodeImage(UINT16 fileIndex,UINT32 *decAddr,UINT32 endAddr,UINT8 decOpt,UINT16 *imageWidth,UINT16 *imageHeight,UINT16 *destWidth,UINT16 *destHeight);
UINT8 pbDecodeImageData( ExifImage_t* imagePara,UINT32 *decAddr, UINT32 endAddr, UINT16 *destWidth, UINT16 *destHeight );
UINT8 pbCalImageScalFac4PhotoFrame(UINT32 bufSize, UINT16 srcWidth, UINT16 srcHeight,UINT16 *destWidth, UINT16 *destHeight );
UINT32 pbMoveImage2Center(UINT32 startAddr, UINT32 freeEndAddr, UINT16 width, UINT16 height, UINT8 dmyX, UINT8 dmyY );
void pbDisplayImage(UINT32 imageBufAddr, UINT16 imageWidth, UINT16 imageHeight );
void pbPhotoFrameCombine(UINT32 imageBufAddr, UINT16 imageWidth, UINT16 imageHeight, UINT32 frameBufAddr, UINT16 frameWidth,UINT16 frameHeight);
void pbEncodeImage(UINT32 mainImageAddr, UINT16 width, UINT16 height, UINT16 destWidth, UINT16 destHeight );
UINT32 pbEditFileLoad(PBFile_t* pFile, ExifImage_t* mainPara,  ExifImage_t* thumbPara,  ExifImage_t* qvPara);
UINT8 pbEditFileMake(UINT32 startAddr, UINT32 size);
UINT32 pbEditImgMoveBufEnd(UINT32 srcAddr, UINT16 imgW, UINT32 imgH, UINT32 endBufAddr);

#endif
