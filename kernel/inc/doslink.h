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

#ifndef __DOSLINK__H__
#define __DOSLINK__H__ 

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------

//-------------------------------------
//dosdir.c
//-------------------------------------
//UINT16 DOS_RootFindName(UINT8* name);
//UINT16 DOS_RootMakeDir(UINT8* name, UINT16 numCluster);
//UINT8 DOS_RootAddBaseFdb(UINT8* name, UINT16 startCluster, UINT32 fileSize);
//UINT8 DOS_RootAddFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon);
//UINT16 DOS_FindName(UINT8* name);
//UINT16 DOS_MakeDir(UINT8* name, UINT16 numCluster);
//UINT16 DOS_GetCurrDir(void);
//void DOS_SetCurrDir(UINT16 currDir);
//UINT8 DOS_AddBaseFdb(UINT8* name, UINT16 startCluster, UINT32 fileSize);
//UINT8 DOS_AddFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon);
//UINT16 DOS_FindFdb(UINT8 matchCond, UINT32 refCon, UINT32 logAddr, UINT32 siz, UINT16 *fdbOffset, UINT32 *fdbLogAddr);
//void DOS_FillBaseFdb(UINT8 *fdb, UINT8 *name, UINT16 startCluster, UINT32 fileSize);
//UINT8 DOS_WriteDirInitData(UINT16* fatReq, UINT16 currentStart, UINT16 parentStart);
//UINT8 DOS_DeleteFile(UINT8 *buf, UINT16 fdbOffset, UINT32 logAddr);
//UINT16 DOS_SearchFdb(UINT8 matchCond, UINT32 refCon, UINT32 ramStartAddr, UINT32 ramEndAddr, UINT16 *pLastCluster, UINT16 *fdbOffset);
//UINT32 DOS_FindFile(UINT16 startCluster, UINT8 matchCond, UINT32 refCon, UINT16 *fdbOffset);

// WWWW0410
//UINT8 DOS_RootModifyFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon, UINT8 modifyOffset, UINT8 modifyLen);
//UINT8 DOS_ModifyFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon, UINT8 modifyOffset, UINT8 modifyLen);
//UINT8 DOS_GetFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon, UINT32 *fdbLogAddr);
//UINT8 DOS_SetFdb(UINT8 *fdb, UINT32 fdbLogAddr);
//UINT8 DOS_RootGetFdb(UINT8 *fdb, UINT8 matchCond, UINT32 refCon, UINT32 *fdbLogAddr);
//UINT8 DOS_RootSetName(UINT8 *name, UINT16 startCluster, UINT32 fileSize);


//-------------------------------------
//dosfile.c
//-------------------------------------
//UINT8 File_Make(UINT8 srcPath, UINT32 ramAddr, UINT8* name, UINT32 fileSize);
//UINT16 vfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster);
//UINT8 vfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize);
//UINT8 File_CloseErr(UINT16 fHandle, UINT8 rwMode);
//UINT8 vfsFileDelete(UINT8* name);
//UINT8 File_Delete1File(UINT8 matchCond, UINT32 refCon);
//UINT8 vfsFileDeleteAll(UINT8 matchCond, UINT32 refCon);

//-------------------------------------
//dosfat.c
//-------------------------------------
#if 0 //xian --
UINT16 DOS_GetClusterSize(void);
UINT8 DOS_Initialize(void);
UINT8 DOS_LoadMBS(UINT32* pbsAddr);
UINT8 DOS_LoadPBS(UINT32 pbsAddr);
UINT8 DOS_LoadFat(void);
UINT8 DOS_SaveFat(UINT16 startCluster);
UINT8 DOS_MakeFatChain(UINT16* fatReq, UINT8 continueSearh );
UINT8 DOS_MakeFat12Chain(UINT16* fatReq, UINT8 continueSearh );
UINT8 DOS_MakeFat16Chain(UINT16* fatReq, UINT8 continueSearh);
UINT8 DOS_SetFatChain(UINT16 fatReq);
UINT8 DOS_SetFat12Chain(UINT16 fatReq);
UINT8 DOS_SetFat16Chain(UINT16 fatReq);
UINT8 DOS_ConnectFatChain(void);
UINT8 DOS_ConnectFat12Chain(void);
UINT8 DOS_ConnectFat16Chain(void);
UINT16 DOS_GetNextCluster(UINT16 currCluster);
UINT16 DOS_GetNextCluster12(UINT16 currCluster);
UINT16 DOS_GetNextCluster16(UINT16 currCluster);
UINT8 DOS_ClearFatChain(UINT16 startCluster);
UINT8 DOS_ClearFat16Chain(UINT16 startCluster);
UINT8 DOS_ClearFat12Chain(UINT16 startCluster);
UINT32 DOS_ClusterLogAddr(UINT32 startCluster);
UINT8 DOS_ReadSector(UINT8 dstPath, UINT32 logAddr, UINT32 ramAddr);
UINT8 DOS_ReadMoreSector(UINT8 dstPath, UINT32 logAddr, UINT32 ramAddr, UINT32 size);
UINT8 DOS_WriteSector(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr);
UINT8 DOS_WriteMoreSector(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr, UINT32 size);

UINT8 DOS_CheckDiskSpace(UINT16 *reqCluster);
void DOS_SetError(UINT16 errCode);
void DOS_ClearError(void);
UINT16 DOS_GetError(void);
UINT32 DOS_ReadLong(UINT8 *dataAry, UINT16 offset);
void DOS_FillLongAtSdram(UINT32 ul);

// K_DOS_HaveLinearFAT
UINT8 DOS_File_Build_LinearFAT(UINT16 startCluster);
UINT8 DOS_Build_LinearFAT(void);
UINT16 DOS_CalNumCluster(UINT32 siz);
#endif
//-------------------------------------
//dosrw.c
//-------------------------------------
//UINT8 vfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage);
//UINT8 vfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);
//void File_FifoWritePage2Card(UINT32 startLogAddr,UINT8 hasOldBlk);
//UINT8 File_ReadDataToDram(UINT16 fHandle, UINT32 readSize, UINT32 ramAddr, UINT32 fOffset);
//UINT16 File_FindPattern(UINT16 fHandle, UINT8 *pat, UINT16 patSize, UINT32 *pFOffset, UINT32 *pSize, UINT32 *pID);
//UINT32 File_ReadValue(UINT16 fHandle, UINT32 *pFOffset, UINT8 littleEndian, UINT8 valueSize);
//UINT8 File_ReadSeek(UINT16 fHandle, UINT32 fpos);
//UINT8 File_LWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage);
//UINT8 File_LRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);  

//-------------------------------------
//dosusb.c
//-------------------------------------
//UINT8 DOS_UploadFdb(UINT16 fileCount, UINT32 ramAddr);
//UINT8 DOS_UploadEccInfo(UINT16 eccCount);

//UINT8 DOS_Upload1File(UINT8 matchCond, UINT32 refCon);
//patch4.5@richie@partial file upload
//UINT8 DOS_UploadPartialFile(UINT32 offset, UINT32 length, UINT16 fileIndex);

//UINT8 DOS_GetUsbDataReady(void);         // 500 use


//UINT8 dramFileSave(UINT8 *name,UINT32 startAddr, UINT32 size);
//UINT8 dramFileLoad(UINT8 *name,UINT32 startAddr, UINT32 *size);

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//-------------------------------------
//dosfat.c
//-------------------------------------
#define K_DOS_SectorSize	512

#define K_DOS_DstSram		1
#define K_DOS_DstDram		2
#define K_DOS_DstUsb		3
//richie@vc0422
#define K_DOS_DstBulk2Usb	4

#define K_DOS_SrcSram		1
#define K_DOS_SrcDram		2
#define K_DOS_SrcUsb		3

//-------------------------------------
//dosfile.c
//-------------------------------------
#define K_File_SrcAviFifo2Card                 0
#define K_File_SrcAviDram2Card                 1
#define K_File_SrcDataDram2Card                2
#define K_File_SrcDataDram2CardByPort          3	// HIS120-5

#define K_DOS_MatchName                         1
#define K_DOS_MatchFree                         2
#define K_DOS_MatchMinUsr                       50
#define K_DOS_MatchMaxUsr                       255

// access mode for file_open
#define K_DOS_NewFile                       	0x80
#define K_DOS_ReadFile                       	0x01
#define K_DOS_RootReadFile                      0x02

// WWWW0408
#define K_DOS_HaveLinearFAT                     1
#define K_DOS_NewFile_LinearFAT                 0x81
#define K_DOS_ReadFile_LinearFAT                0x03
#define K_DOS_RootReadFile_LinearFAT            0x04

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
#if 0 //move to dosvar.h
//-------------------------------------
//dosfat.c
//-------------------------------------
extern	xdata   UINT8    G_DOS_SystemId;
extern	xdata   UINT16    G_DOS_SectorSize;
extern	xdata   UINT32    G_DOS_ClusterSize;
extern	xdata   UINT16    G_DOS_SectorPerFat;
extern	xdata   UINT16    G_DOS_TotalRootDir;
extern	xdata   UINT8    G_DOS_FileSystemType;

extern	xdata   UINT32   G_DOS_FatAddr;
//extern	xdata   UINT32   G_DOS_FatEndAddr;
//extern	xdata   UINT32   G_DOS_Fat2Addr;
extern	xdata   UINT32   G_DOS_FatSize;

//extern	xdata   UINT32   G_DOS_RootDirAddr;
//extern	xdata   UINT32   G_DOS_DataAddr;

extern	xdata   UINT16    G_Card_TotalSizeMB;
extern	xdata   UINT16	G_DOS_TotalFreeCluster;
//extern	xdata   UINT16	G_DOS_ErrCode;
extern	xdata   UINT32   G_Card_SectorPerCard;
extern	xdata   UINT16	G_Card_BytePerSector;

extern	data    BIT     G_File_AudioEnable;
extern	data    BIT     G_File_SDRAM_Read;
extern	idata   UINT32   G_DOS_SDRAM_NextIntRetAddr;
extern	xdata	UINT32 	G_DOS_SDRAM_FAT1Addr;
extern	xdata   UINT8    G_DOS_Stop_Initial;
extern	xdata   UINT8    G_DOS_Initial_Status;

//extern	xdata   UINT32   G_DOS_SectorPerCard;        
//extern	xdata   UINT32   G_DOS_HiddenSector;
//extern	xdata   UINT16    G_DOS_ReserveSector;
//extern	xdata   UINT32   G_DOS_PBSAddr;
//extern	xdata   UINT16    G_DOS_Head;
//extern	xdata   UINT16    G_DOS_SectorPerTrack;
//extern	xdata   UINT16    G_DOS_Cylinder;
//extern	xdata   UINT16    G_DOS_SectorPerCluster;

extern	data    BIT 	G_DOS_CardErrDetected;	// patch 4.4-2@wyeo@abnormal action recovery

// WWWW0408
extern	xdata	UINT16	G_DOS_FileStartCluster;
#if (K_DOS_HaveLinearFAT)
extern	xdata	UINT32 	G_DOS_SDRAM_LinearFATAddr;
extern	data    BIT     G_DOS_UseLinearFAT;
extern	xdata	UINT16    G_DOS_LinearFATMax;
//extern	xdata	UINT16    G_DOS_LinearFATTotal;
//extern	xdata	UINT16    G_DOS_LinearFATIndex;
//extern	data    BIT     G_DOS_FATIsLinear;
#endif

// wyeo@0517
#define K_PreWriteOldBlock		0	// patch 4.4-4@wyeo@function not test, MUST NOT TURN ON

#if (K_PreWriteOldBlock)
extern	data    BIT     G_SMC_PreWriteOldBlock;
extern	data    BIT     G_SMC_EnablePreWriteOldBlock;
extern	data    BIT     G_SMC_OldBlockIsNew;
extern	data    BIT     G_SMC_FreshOldBlk;
//UINT8 SMC_PreWriteOldBlock(void);
#endif

//-------------------------------------
//dosfile.c
//-------------------------------------
extern	xdata	UINT32 	G_DOS_SDRAM_MaxAddr;
extern	xdata	UINT32	G_DOS_FilePos;
// @012403@wyeo@new nand write module, begin
extern	xdata	UINT32	G_DOS_FileWriteSize;
extern	xdata	UINT32	G_DOS_FileBulkSize;
// @012403@wyeo@new nand write module, end

// avi related
extern	xdata	UINT16	G_File_WritePageCount;
extern	idata	UINT8	G_File_PevDone;
//extern	xdata   UINT32   G_File_HdrLogAddr;
extern	xdata   UINT32   G_File_HdrPhyAddr;
extern	xdata   UINT16    G_File_HdrLogBlock;
//extern	xdata   UINT32   G_File_FrameHdrLogAddr;
extern	xdata   UINT32   G_File_FrameHdrPhyAddr;
extern	xdata   UINT16    G_File_FrameHdrLogBlock;

// HIS120-0001 start
extern	xdata   UINT32   G_File_ReserveLogAddr[];
extern	xdata   UINT32   G_File_ReservePhyAddr[];
extern	xdata   UINT16    G_File_ReserveLogBlock[];
extern	xdata   UINT32   G_File_FrameReserveLogAddr[];
extern	xdata   UINT32   G_File_FrameReservePhyAddr[];
extern	xdata   UINT16    G_File_FrameReserveLogBlock[];
// HIS120-0001 end

//-------------------------------------
//dosdir.c
//-------------------------------------
//extern	xdata   UINT16    G_DOS_CurrDirCluster;
extern	xdata 	UINT32 	G_DOS_FileSize;
extern  xdata	UINT32 	G_DOS_SDRAM_WorkAddr;
extern  xdata	UINT32 	G_DOS_SDRAM_WorkSize;
extern  xdata   UINT16    G_DOS_CreateTime;
extern  xdata   UINT16    G_DOS_CreateDate;

//ada@0501 for card write protect
extern    xdata   UINT8   G_DOS_FileAttribute;
extern    xdata   UINT32   G_DOS_FdbAddress;
#endif
#endif /*__DOSLINK_H__*/
