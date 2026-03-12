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
 * @file		exif_entry.h
 * @brief		exif standard 2.2
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#ifndef __EXIF_ENTRY__H__
#define __EXIF_ENTRY__H__

#include "exif_def.h"
#include "exif_param.h"
#include "common.h"
#include "general.h"
#include "exif_fun.h"
#include "hal_dram.h"
#include "snap_task.h"
#include <stdarg.h>

typedef struct exifJpgFileInfo{
	UINT32 mainVlcAddrW;
	UINT16 mainWidth;
	UINT16 mainHeight;
	UINT8  mainQidx;
	
	UINT32 qvVlcAddrW;
	UINT32 qvSize;
	UINT16 qvWidth;
	UINT16 qvHeight;
	UINT16 qvCropWidth;
	UINT16 qvCropHeight;
	UINT8  qvQidx;
	
	UINT32 thbVlcAddr;
	UINT32 thbSize;
	UINT16 thbWidth;
	UINT16 thbHeight;
	UINT8  thbQidx;

	UINT8 rotOpt;
	UINT8 fileFormat;
	UINT8 exifCom[16];
	UINT32 dbgInfoAddr;
	UINT32 dbgInfoSize;
	UINT8 viViLinkEn;
	UINT8 viViLinkBuf[8];
}exifJpgFileInfo_t;

typedef void (*exifCallBackFun)(ExifImage_t *image) ;

void exifFlashModeSet(UINT8 flag);


UINT8 exifParserStart(exifCallBackFun func,ExifImage_t *image,UINT32 addrByte,UINT32 size);
UINT8 exifSetIFDData(ExifIfd exififdtype, ExifTag  exiftag,UINT32 va_count,...);
UINT8 exifGetIFDValue(ExifIfd exififdtype,ExifTag exiftag,UINT32 va_count,...);
void exifGetMainInfo(ExifImage_t *mainImage);
void exifGetThumbInfo(ExifImage_t *thumbImage);
void exifGetQVInfo(ExifImage_t *qvImage);
UINT32 exifQvAvailableSizeGet(UINT32 thbSize);
dateStc_t* exifRtcSet(dateStc_t* ptRtc);
UINT32 exifIFDTagOffsetGet(UINT32 addrw,ExifIfd exififdtype,ExifTag exifTag,UINT8 bypass);
/*void exifUserCommentSet(UINT8 cmCode[8], UINT8 en);*/
/*UINT8 exifUserCommentGet(UINT8 cmCode[8]);*/
UINT32 exifAddMmsHdr(UINT32 thubAddr,UINT32 thubSize,UINT16 thbW,UINT16 thbH,UINT8 qIdx);
UINT32 exifCreateFile(const exifJpgFileInfo_t *jpgInfo);
/*void exifParamSet(ExifParam_e id,UINT16 value);*/
/*UINT16 exifParamGet(ExifParam_e id);*/

extern UINT8 exifRenameIFD(ExifIfd exififdtype, ExifTag  exiftag,	IFD_t *newIFD);
extern UINT8 exifCreateFileStart(const exifJpgFileInfo_t *jpgInfo);
extern UINT32 exifCreateFileEnd(const exifJpgFileInfo_t *jpgInfo);

#endif

