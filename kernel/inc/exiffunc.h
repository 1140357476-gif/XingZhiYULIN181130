/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __EXIFFUNC_H__
#define __EXIFFUNC_H__

#include "hal_global.h"
//------------------------------------------------------------------------------
//Type and Constant Definition
//------------------------------------------------------------------------------
void EXIFHDR_Initialize_AllExifHeader( UINT32 exifAddr );
void EXIFHDR_Initialize_ExifHeader0(UINT32* pDramHdrAddr);    

//patch5.1@ada@Audio annotation begin
void EXIFHDR_Initialize_ExifHeader0_Audio(UINT32* pDramHdrAddr); 
//patch5.1@ada@Audio annotation end

void EXIFHDR_Initialize_ExifHeader2(UINT32* pDramHdrAddr);    
void EXIFHDR_Initialize_ExifHeader3(UINT32* pDramHdrAddr);    
void EXIFHDR_Initialize_ExifHeader4(UINT32* pDramHdrAddr);    
void EXIFHDR_ThumbEven_COM(struct Exif_Header2 *hdr1);
void EXIFHDR_ThumbOdd_COM(struct Exif_Header3 *hdr2);
void EXIFHDR_Fill_Text(UINT8* srctable,UINT8* desttable,UINT32 i);
//void EXIFHDR_DQT(UINT8 qindex,struct HDR4 *table);
void EXIFHDR_DHT(struct HDR5 *table);
void EXIFHDR_SOF(struct HDR3 *softable,UINT16 width,UINT16 height);    
void EXIFHDR_SOS(struct HDR6 *sostable);

//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_ExifFile(UINT32 DramAddr, BIT ShowThumb,UINT8 *DateTime,struct Exif_Decompress_Parameter *ImagePara,struct Exif_Decompress_Parameter *ThumbPara);
UINT8 EXIFHDR_Parsing_SOI(void);
UINT8 EXIFHDR_Parsing_APP0(UINT32 DramAddr,BIT ShowThumb);
UINT8 EXIFHDR_Parsing_APP0_Sub(UINT32 DramAddr,BIT ShowThumb,UINT16 EvenOddOffset);
UINT8 EXIFHDR_Parsing_APP1(UINT32 DramAddr,BIT ShowThumb,UINT8 *DateTime);
UINT8 EXIFHDR_Parsing_APP1_Sub1(UINT32 DramAddr,BIT ShowThumb,UINT8 *DateTime,UINT8 EvenOddOffset);
UINT8 EXIFHDR_Parsing_APP1_Sub2(UINT32 DramAddr,UINT8 *DateTime,UINT16 EvenOddOffset);
UINT8 exifParsingFile(UINT32 DramAddr, UINT32 srcSize, BIT ShowThumb,UINT8 *DateTime,struct Exif_Decompress_Parameter *ImagePara,struct Exif_Decompress_Parameter *ThumbPara);

//patch5.1@ada@Audio annotation begin
UINT8 EXIFHDR_Passing_APP1_ExifTag(UINT32 DramAddr); 
//patch5.1@ada@Audio annotation end

UINT8 EXIFHDR_Parsing_APP1_Sub3(UINT32 DramAddr,UINT16 EvenOddOffset);
UINT8 EXIFHDR_Parsing_APP1_Sub4(UINT16 EvenOddOffset,UINT8 *search);
UINT8 EXIFHDR_Parsing_APPn( UINT32 DramAddr );

//version4.0@ada@0513 for EXIF
UINT8 EXIFHDR_Parsing_DQT(void);
//UINT8 EXIFHDR_Parsing_DQT(BIT ShowThumb);

UINT8 EXIFHDR_Parsing_DQT_Sub1(UINT16 EvenOddOffset);

//version4.0@ada@0513 for EXIF
//UINT8 EXIFHDR_Parsing_DQT_Sub2(UINT16 EvenOddOffset);

UINT8 EXIFHDR_Parsing_DHT(void);
UINT8 EXIFHDR_Parsing_DHT_Sub(UINT16 EvenOddOffset);
UINT8 EXIFHDR_Parsing_SOF(struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara);
UINT8 EXIFHDR_Parsing_SOF_Sub(struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara,UINT16 EvenOddOffset);
UINT8 EXIFHDR_Parsing_DRI(void);

//patch4.5@ada@Fix EXIF bug begin
UINT8 EXIFHDR_Parsing_SOS(UINT32 DramAddr,BIT ShowThumb,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara);
//UINT8 EXIFHDR_Parsing_SOS(UINT32 DramAddr,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara);
UINT8 EXIFHDR_Parsing_SOS_Sub(UINT32 DramAddr,BIT ShowThumb,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara,UINT16 EvenOddOffset);
//UINT8 EXIFHDR_Parsing_SOS_Sub(UINT32 DramAddr,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara,UINT16 EvenOddOffset);
//patch4.5@ada@Fix EXIF bug end

UINT8 EXIFHDR_Parsing_EOI(UINT32 DramAddr,BIT ShowThumb);
UINT8 EXIFHDR_Parsing_COM(void);

void EXIFHDR_Load_ExifHeader0_SRAM2DRAM(struct HDR0 *hdr0,UINT32* pDramHdrAddr);  
void EXIFHDR_Load_ExifHeader3_SRAM2DRAM(struct HDR3 *hdr2,UINT32* pDramHdrAddr);  
void EXIFHDR_Load_ExifHeader4_SRAM2DRAM(struct HDR4 *hdr3,UINT32* pDramHdrAddr);  
UINT16 EXIFHDR_ByteSwapOfUSHORT(UINT16 x);
UINT32 EXIFHDR_ByteSwapOfULONG(UINT32 x);

UINT32 EXIFHDR_LoadExifHeader(UINT32 vlcStart, UINT32 exifAddr, UINT32 tmbsize);
  
void EXIFHDR_Load_ExifHeader4(UINT32* pSrcDramHdrAddr,UINT32* pDestDramHdrAddr);     
 
void EXIFHDR_Modify_SRAM_ExifHeader4(struct Exif_Header4 *Hdr3);    
    
void EXIFHDR_Modify_SOF(struct HDR3 *hdr,UINT16 width,UINT16 height);    
void EXIFHDR_Copy_Exif_Header0(struct Exif_Header0 *Hdr0,UINT32* pDestDramHdrAddr); 

//patch5.1@ada@Audio annotation begin
//void EXIFHDR_Copy_Exif_Header0_Audio(struct Exif_Header0_Audio *Hdr0,UINT32* pDestDramHdrAddr);
//patch5.1@ada@Audio annotation end

void EXIFHDR_Copy_Exif_Header4(struct Exif_Header4 *Hdr3,UINT32* pDestDramHdrAddr); 
UINT16 EXIFHDR_NeedSwitchUSHORTOrder(UINT16 byteorder,UINT16 value);
UINT32 EXIFHDR_NeedSwitchULONGOrder(UINT16 byteorder,UINT32 value);


UINT8 EXIFTagValueSet(UINT8 ifdType,UINT16 tagId,UINT16 idfDataType,UINT32 ifdCnt,UINT32 valueOffset,UINT8 *pvalue,	UINT8 bigEndian);
void EXIFJPGCommentSet(UINT8 *txt, UINT8 len);

UINT32 exifQvAvailableSizeGet( UINT32 thbSize );

void EXIFUserCommentSet(UINT8 cmCode[8], UINT8 en);        //20110809
UINT8 EXIFUserCommentGet(UINT8 cmCode[8]);                  //20110809

dateStc_t* exifRtcSet(dateStc_t* ptRtc);
#endif /*__EXIFFUNC_H__*/
