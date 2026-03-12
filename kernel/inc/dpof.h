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

#ifndef __DPOF_H__
#define __DOPF_H__

#if (APP_DPOF_INIT)	/*Add by Phil@07.10.02*/

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
void DPOF_SetDpofDir0Name(UINT8* name);
void DPOF_SetDpofFileName(UINT8* name);
void DPOF_GetDpofDir1Name(void);

//version4.0@ada@0513 for DPOF
void DPOF_ClearDramDpofArea(void);

UINT8 DPOF_DirInitialize(void);

//patch4.4@ada@DPOF begin
void DPOF_CheckJpgFileExist(void);
//patch4.4@ada@DPOF end

UINT8 DPOF_FileInitialize(void);
UINT8 DPOF_CheckDpofFileExist(void);
UINT8 DPOF_OpenDpofFile(UINT8* name,UINT16 *fHandle,UINT16 startCluster);
void DPOF_WriteDpofFileHeaderSection(void);
UINT8 DPOF_RecordDpofJobSection(void);
void DPOF_WriteSunplusImageField(void);
UINT8 DPOF_BuildDpofFile(void);
UINT8 DPOF_DeleteFile(UINT8 *name);
void DPOF_EraseDpofJobParameter(UINT16 ImgSrcIdx,UINT8 File_Still_Exist);
UINT8 DPOF_CheckDpofJobFieldExist(UINT16 ImgSrcIdx);
UINT8 DPOF_CheckDpofJobFileExist(UINT16 ImgSrcIdx);
void DPOF_RecordDpofJobParameter(UINT16 ImgSrcIdx,/*UINT16 PrtPid,*/UINT8 PrtTyp,UINT16 PrtQty,UINT8 ImgFmt);
void DPOF_EraseDpofJobParameter(UINT16 ImgSrcIdx,UINT8 File_Still_Exist);
UINT8 DPOF_ReadDpofJobParameter(UINT16 ImgSrcIdx,/*UINT16 *PrtPid,*/UINT8 *PrtTyp,UINT16 *PrtQty,UINT8 *ImgFmt);

//version4.0@ada@0513 for DPOF
UINT8 DPOF_SelectSunplusImage(UINT16 ImgSrcIdx,/*UINT16 PrtPid,*/UINT8 PrtTyp,UINT16 PrtQty/*,UINT8 ImgFmt*/);

UINT8 DPOF_DeselectSunplusImage(UINT16 ImgSrcIdx,UINT8 File_Still_Exist);
UINT8 DPOF_ModifySunplusImagePRTQTY(UINT16 ImgSrcIdx,UINT16 PrtQty);
void DPOF_CopyJobSection(UINT32 JobSection_StartDramAddr,UINT32 JobSection_Length,UINT32 ByteCount);
UINT8 DPOF_FindDpofJobSection(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT32 *JobSection_StartDramAddr,UINT32 *JobSection_StartByteCount);
UINT8 DPOF_FindDpofIMGSRC(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *ImgSrcIdx);
UINT8 DPOF_FindDpofPRTPID(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *PrtPid);
UINT8 DPOF_ModifyDpofPRTPID(UINT32 NewDramAddr,UINT32 ByteCount);
UINT8 DPOF_FindDpofPRTTYP(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT8 *PrtTyp);
UINT8 DPOF_FindDpofPRTQTY(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *PrtQty);
UINT8 DPOF_FindDpofIMGFMT(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT8 *ImgFmt);
void DPOF_BuildOneDpofFileJobSection(UINT16 ImgSrcIdx,UINT8 PrtTyp,UINT16 PrtQty,UINT8 ImgFmt);
void DPOF_WriteDpofFileJobParameter(UINT8 *ParameterTable,UINT16 TableLength);
void DPOF_WriteDpofFileJobSectionTitle(void);
void DPOF_WriteDpofFileParameter_PRTPID(void);
void DPOF_WriteDpofFileParameter_PRTTYP(UINT8 PrintType);   
void DPOF_WriteDpofFileParameter_PRTQTY(UINT16 PrintQuantity);   
void DPOF_WriteDpofFileParameter_IMGFMT(UINT8 ImageFormat);  
void DPOF_WriteDpofFileParameter_IMGSRC(UINT8 PrintType,UINT16 FileIndex);  

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
#define K_DPOF_STD            (UINT8)1
#define K_DPOF_IDX            (UINT8)2

#define K_DPOF_CIFF1_IMAGE    (UINT8)1
#define K_DPOF_EXIF1_J_IMAGE  (UINT8)2
#define K_DPOF_EXIF1_T_IMAGE  (UINT8)3
#define K_DPOF_EXIF2_J_IMAGE  (UINT8)4
#define K_DPOF_EXIF2_T_IMAGE  (UINT8)5
#define K_DPOF_JFIF_IMAGE     (UINT8)6
#define K_DPOF_FPX1_IMAGE     (UINT8)7
#define K_DPOF_UNDEF_IMAGE    (UINT8)8     

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
extern UINT16 G_DPOF_Dir0Cluster;
extern UINT32 NewDpofFileDramAddr;
extern UINT32 NewDpofFileByteCount;
extern UINT16 NewDpofJobSectionPRTPID;
extern UINT16 TotalSunplusDpofFileNumber;
extern UINT8 Dpof_File_Modify;
extern UINT8 Other_DpofJobSection_Exist;

//version4.0@ada@0513 for DPOF
extern UINT8 DpofDir1Name[];

extern code UINT8 DPOF_Dir0Name[];  
#endif /*APP_DPOF_INIT*/

#endif /*__DPOF_H__*/