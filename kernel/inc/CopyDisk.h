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

#ifndef _CopyDisk_h_
#define _CopyDisk_h_

//#include "smccon.h"
#include "doslink.h"

//#define COPYDISK_5  1	//move to dos16_copydisk.c

#define K_Copy_Nand2Card 0
#define K_Copy_Card2Nand 1

#define K_File_UI_Start_CopyDisk	101	// Start copy 1 file.
#define K_File_UI_End_CopyDisk		102	// End cpy 1 file. 
#define K_File_UI_Err_CopyDisk		103	// Error copying 1 file
#define K_File_UI_Start_Copy1Dir	104	// Start copy 1 dir.
#define K_File_UI_End_Copy1Dir		105	// End cpy 1 dir. 
#define K_File_UI_Start_Copy1File	106	// Start copy 1 file.
#define K_File_UI_End_Copy1File		107	// End cpy 1 file. 
#define K_File_UI_CopyDisk_TotalCnt 110 
#define K_File_UI_CopyDisk_CurCnt   111 

#if 0 //xian move to dos16_copydisk.c
typedef struct _COPYFILE_SAVE_XDATA
{	
	UINT8    dosSystemId;
	UINT16    dosSectorPerFat;
	UINT16    dosTotalRootDir;
	UINT32   dosRootDirAddr;
	UINT16	dostotalfreeCluster;
	UINT32   cardSectorPerSector;
	UINT16	cardBytePerSector;
	UINT32   dosSDRAMNextIntRetAddr;
	UINT32   dosSectorPerCard;     
	UINT32   dosHiddenSector;
	UINT16    dosReserveSector;
	UINT32   dosPBSAddr;
	UINT16    dosHead;
	UINT16    dosSectorPerTrack;
	UINT16    dosCylinder;
	UINT16	dosFileStartCluster;
	UINT32 	dosMaxAddr;
	UINT32	dosFilePos;
	UINT16    dosCurrDirCluster;
	UINT32 	dosFileSize;
	UINT16    dosCreateTime;
	UINT16    dosCreateDate;
	UINT16	totalDir;
	UINT16	dir1CountIndex;
	UINT16	maxDir1Count;
	UINT16	dir1Count;
	UINT16	maxdir1CountIndex;
	UINT16	dir1Cluster;
	UINT16	currDirCluster;
	UINT32	dosCluserSize;
	UINT16	sectorPerCluser;
	UINT16	sectorSize;
	UINT32	dosDataAddr;
	UINT32	dosFilesize;
	UINT8	fileType; 
	UINT16	fileindex;
	UINT16	fileCount;
	UINT8	fileSystemType;
	UINT32	dosFatAddr;
	UINT32	SDRAMFat1Addr;
	UINT16  	preCluster;
	UINT32	fatNextAddr;
	UINT16	fatNextCluster;
	UINT8	fatNextFlag;
	UINT16    dosMaxCluster; /* bi-dir */
    UINT16    dir0Cluster; /* bi-dir */
	#if (NANDF_OPTION == 1 && SMC_OPTION ==1)
	UINT32 	smcBankBufferAddr;
	UINT16	cardToatalSizeMB; 		//G_Card_TotalSizeMB;
	UINT16    cardBlockSize;			//G_Card_BlockSize;
	UINT8    cardBlockSizeShift;		//G_Card_BlockSizeShift;
	UINT16	mediaBehavior; 			//G_Media_Behavior;
	UINT8    smcCardTotalBank;		//G_Card_TotalBank;
	UINT8    smcCardCurrBank;		//G_Card_CurrBank;
	UINT16    smcCardBlackExtSize;	//G_Card_BlockExtSize;
	UINT16    smcCardBlockWholeSize; 	//G_Card_BlockWholeSize;
    #if (COPYDISK_5)
	UINT16    smcCardpageperBlock;	//G_Card_PagePerBlock;  // @122002@wyeo@support 256M nand
    #else
 	UINT8    smcCardpageperBlock;
    #endif

	UINT32   smcPhysicalbanlSize; 	//G_Card_PhysicalBankSize;
	
	UINT16    smcAllocateTableSize; 	// G_SMC_AllocatTableSize;
	UINT8    smcEntryTAbleSize; 		//G_SMC_EntryTableSize;
	
	UINT32   smcCisBlockAddr;		//G_SMC_CisBlockAddress;
	
	UINT8    smcEccByte[K_SMC_EccByteSize];	//G_SMC_EccByte[K_SMC_EccByteSize];
	UINT8	smcEccErr; 						//G_SMC_EccErr = 0;
	UINT16	smcErrCode;						//G_SMC_ErrCode = 0;
	UINT16    smcFirstCISFound;				//G_SMC_FirstCISFound;	// K_Check_Nand_Bad_Block_Infomation
	UINT32	smcEccErrCount;					//G_SMC_EccErrCount;
	UINT32	smcEcc1BitErrCount; 			//G_SMC_Ecc1BitErrCount;
	UINT32	smcEcc2BitErrCount;				//G_SMC_Ecc2BitErrCount;
	
	UINT16    nandReserveBlockTotal; 			//G_NAND_ReservBlockTotal; 
	UINT16    nandReserveBlockMax;			//G_NAND_ReservBlockMax;
	UINT16    smcCisBlockFound; 				//G_SMC_CisBlockFound;
	UINT32   smcNewCisBlockAddr;				//G_SMC_NewCisBlockAddress;

	#if 1//(K_HAVE_HAL)
	UINT8   smcAddrMode; //G_SMC_AddrMode;
	#endif
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	UINT8	smcBuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE]; //G_SMC_BuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE];
	#endif

    #if (COPYDISK_5)
    // @122002@wyeo@support 256M nand, begin
        UINT32   smcGoodSectorPerCard;           // G_SMC_GoodSectorPerCard
        UINT8    nandMakerByte;                  // G_NANDF_Maker_Byte
        UINT8    nand3RD_Byte;                   // G_NANDF_3RD_Byte
        UINT8    nand4TH_Byte;                   // G_NANDF_4TH_Byte          
        UINT8    nand5TH_Byte;                   // G_NANDF_5TH_Byte
        UINT8    nandCategory;                   // G_NANDF_Category // 0 is old, 1 is new and SMC2_
        UINT16    nandPageSize;                   // G_NANDF_PageSize
        UINT16    nandPageExtSize;                // G_NANDF_PageExtSize
        UINT32   nandBlockSize;                  // G_NANDF_BlockSize
        UINT8    nandBlockSizeShift;             // G_NANDF_BlockSizeShift
        /*
        BYTE    nandHasOldBlk;                  // G_NANDF_HasOldBlk
        UINT32   nandStartLogAddr;               // G_NANDF_StartLogAddr
        UINT32   nandEndBlkAddr;                 // G_NANDF_EndBlkAddr
        UINT32   nandNewPhyAddr;                 // G_NANDF_NewPhyAddr
        UINT32   nandOldPhyStartAddr;            // G_NANDF_OldPhyStartAddr
        WORD    nandBlockNum;                   // G_NANDF_OldPhyStartAddr
        UINT32   nandOldPhyAddr;                 // G_NANDF_OldPhyAddr
        BYTE    nandFindHeaderPage;             // G_NANDF_FindHeaderPage
        BYTE    nandSetHdrPageAddr;             // G_NANDF_SetHdrPageAddr
        BYTE    nandFirstEnter;                 // G_NANDF_FirstEnter
        BYTE    nandRsvIndex;                   // G_NANDF_RsvIndex
        BYTE    nandStartPage;                  // G_NANDF_StartPage
        */
        #if 1//(K_SUPPORT_NEWNAND)
        UINT8    nandPageLastByte;               // G_NANDF_PageLastByte
        #endif
        // @122002@wyeo@support 256M nand, end
        #endif

	#endif
} COPYFILE_SAVE_XDATA, *PCOPYFILE_SAVE_XDATA;
#endif

// export functions
UINT8 USR_CopyAllDir(UINT32 copyDiskTempBuffer, UINT8 direction);
UINT8 USR_Copy1File(UINT32 copyDiskTempBuffer, UINT16 fileIndex, UINT8 direction);
UINT8 USR_Copy1Dir(UINT32 copyDiskTempBuffer, UINT16 dirIndex, UINT8 direction);

//UINT8 COPYDISK_CopyDir(UINT8 srcMedia, UINT8 dstMedia, UINT8 matchCond, UINT32 refCon, UINT8 flag);
//UINT8 COPYDISK_Copy1File(UINT8 srcMedia, UINT8 dstMedia, UINT8 matchCond, UINT32 refCon, UINT8 flag);

#endif /*__CopyDisk_h__*/
