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

#ifndef __DOSVAR_H__
#define __DOSVAR_H__
//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------

//-------------------------------------
//dosdat.h
//-------------------------------------
//extern	code	UINT8	G_DOS_SIG[];
//extern	code	UINT8	G_DOS_MBSP_128M[];
//extern	code	UINT8	G_DOS_PBSP_128M[];
//extern	code	UINT8	G_DOS_MBSP_64M[];
//extern	code	UINT8	G_DOS_PBSP_64M[];
//extern	code	UINT8	G_DOS_MBSP_32M[];
//extern	code	UINT8	G_DOS_PBSP_32M[];
//extern	code	UINT8	G_DOS_MBSP_16M[];
//extern	code	UINT8	G_DOS_PBSP_16M[];
//extern	code	UINT8	G_DOS_MBSP_8M[];
//extern	code	UINT8	G_DOS_PBSP_8M[];
//extern	code	UINT8	G_DOS_MBSP_4M[];
//extern	code	UINT8	G_DOS_PBSP_4M[];
//extern	code	UINT8	G_DOS_DirCurrentParent[];

//-------------------------------------
//dosdef.h
//-------------------------------------

//-------------------------------------
//dosdir.c
//-------------------------------------

//-------------------------------------
//dosfat.c
//-------------------------------------
//extern	xdata   UINT16    G_DOS_FreeFatTable[];
//extern	xdata	UINT16	G_DOS_FreeTableIndex;
//extern	xdata   UINT16    G_DOS_PrevCluster;
//extern	xdata	UINT32	G_DOS_FatNextAddr;
//extern	xdata	UINT16	G_DOS_FatNextCluster;
//extern	xdata	UINT8	G_DOS_FatNextFlag;
extern	xdata   UINT16    G_Card_BlockSize;
extern	xdata   UINT8    G_Card_BlockSizeShift;
extern	xdata	UINT16	G_Media_Behavior;
//extern	xdata	UINT16	G_DOS_FileEndCluster;
extern	xdata   UINT32   G_Card_TotalCardSize; 
//extern	xdata   UINT16    G_DOS_MaxCluster;	// @012403@wyeo@prevent dir hang

//-------------------------------------
//dosfile.c
//-------------------------------------
//extern	xdata	UINT32	G_DOS_DstStartLogAddr;	// @012403@wyeo@support 256M nand
//extern	xdata	UINT16	G_DOS_LeftClusterSize;	// @012403@wyeo@support 256M nand
extern	xdata   UINT16    G_DOS_StartClusterNumber;
//extern	xdata   UINT16    G_DOS_MidCluster;
//extern	xdata	UINT16	G_DOS_FreeTableTotal;
//extern	xdata   UINT8    G_File_FirstEnter;
//extern	xdata 	UINT32 	G_File_FrameSize;
//extern	xdata	UINT16	G_DOS_FileStartCluster;
//extern	xdata	UINT32	G_DOS_FileSizeKeep;
//extern	xdata	UINT16	G_DOS_FileCurrCluster;
//extern	xdata	UINT16	G_DOS_FileClusterIndex;
extern	xdata	UINT16	G_DOS_PrevFileCurrCluster;	// WWWW0327
extern	xdata	UINT16	G_DOS_PrevFileClusterIndex;	// WWWW0327

//-------------------------------------
//doscore.c
//-------------------------------------
extern	xdata   UINT16    G_Card_PrevLogBlock;
extern	idata	UINT32	G_Card_EndBlkLogAddr;
extern  idata	UINT32 	G_Card_OldPhyAddr;
extern  idata	UINT32	G_Card_NewPhyAddr;
extern  xdata	UINT32 	G_Card_OldPhyStartAddr;
extern  xdata	UINT32 	G_Card_OldLogStartAddr;
extern  xdata   UINT32   G_Card_PhyBlockAddr;

//-------------------------------------
//dosusb.c
//-------------------------------------
extern	xdata   UINT8    G_DOS_USBDataReady;    // 500 use

#if 1 //move from doslink.h
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

extern	xdata	UINT8     G_File_AudioEnable;
extern	xdata	UINT8     G_File_SDRAM_Read;
extern	xdata   UINT32   G_DOS_SDRAM_NextIntRetAddr;
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

extern	xdata	UINT8 	G_DOS_CardErrDetected;	// patch 4.4-2@wyeo@abnormal action recovery

// WWWW0408
extern	xdata	UINT16	G_DOS_FileStartCluster;
//#if (K_DOS_HaveLinearFAT)
extern	xdata	UINT32 	G_DOS_SDRAM_LinearFATAddr;
extern	xdata	UINT8     G_DOS_UseLinearFAT;
extern	xdata	UINT16    G_DOS_LinearFATMax;
//extern	xdata	UINT16    G_DOS_LinearFATTotal;
//extern	xdata	UINT16    G_DOS_LinearFATIndex;
//extern	data    BIT     G_DOS_FATIsLinear;
//#endif

// wyeo@0517
#define K_PreWriteOldBlock		0	// patch 4.4-4@wyeo@function not test, MUST NOT TURN ON

#if (K_PreWriteOldBlock)
extern	xdata	UINT8     G_SMC_PreWriteOldBlock;
extern	xdata	UINT8     G_SMC_EnablePreWriteOldBlock;
extern	xdata	UINT8     G_SMC_OldBlockIsNew;
extern	xdata	UINT8     G_SMC_FreshOldBlk;
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

extern UINT32 xdata* data pdiskFAT_BankStartCluster;
extern UINT32 xdata* data pdiskFAT_Bank2Cluster;

#endif /*__DOSVAR_H__*/
