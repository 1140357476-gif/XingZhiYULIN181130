#include "general.h"
#include "impt.h"
//#include "cardfunc.h"
#include "cardvar.h"
#include "cardui.h"
//#include "doscon.h"
#include "dosvar.h"
#include "doserr.h"
#include "doslink.h"
#include "sdfunc.h" 
#include "dosusr.h"
#include "smcfunc.h"
#include "exptcode.h"
#include "disk.h" 
#include "copyDisk.h"
//#include "dosusr.h"
//#include "dosfunc.h"
//#include "cffunc.h"
#include "smcvar.h"
//#include "mmcfunc.h"
#include "dramfunc.h"
#include "cardlink.h"
#include "sp1k_pb_api.h"
#include "sp1k_hal_api.h"
#include "dos32.h"

  
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define Dos16_copydisk_c_noused 0	//wangjg++ bank6 overflow,clear noused function
#define COPYDISK_5  1
#define K_DOS_FreeFatTableSize  8				// you could reduce this to 80
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 
typedef struct _COPYFILE_SAVE_XDATA
{	
	UINT8	dosSystemId;
	UINT16	dosSectorPerFat;
	UINT16	dosTotalRootDir;
	UINT32	dosRootDirAddr;
	UINT16	dostotalfreeCluster;
	UINT32	cardSectorPerSector;
	UINT16	cardBytePerSector;
	UINT32	dosSDRAMNextIntRetAddr;
	UINT32	dosSectorPerCard;     
	UINT32	dosHiddenSector;
	UINT16	dosReserveSector;
	UINT32	dosPBSAddr;
	UINT16	dosHead;
	UINT16	dosSectorPerTrack;
	UINT16	dosCylinder;
	UINT16	dosFileStartCluster;
	UINT32 	dosMaxAddr;
	UINT32	dosFilePos;
	UINT16	dosCurrDirCluster;
	UINT32 	dosFileSize;
	UINT16	dosCreateTime;
	UINT16	dosCreateDate;
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
	UINT16	preCluster;
	UINT32	fatNextAddr;
	UINT16	fatNextCluster;
	UINT8	fatNextFlag;
	UINT16	dosMaxCluster; /* bi-dir */
    UINT16	dir0Cluster; /* bi-dir */
	#if (NANDF_OPTION == 1 && SMC_OPTION ==1)
	UINT32 	smcBankBufferAddr;
	UINT16	cardToatalSizeMB; 		//G_Card_TotalSizeMB;
	UINT16	cardBlockSize;			//G_Card_BlockSize;
	UINT8	cardBlockSizeShift;		//G_Card_BlockSizeShift;
	UINT16	mediaBehavior; 			//G_Media_Behavior;
	UINT8	smcCardTotalBank;		//G_Card_TotalBank;
	UINT8	smcCardCurrBank;		//G_Card_CurrBank;
	UINT16	smcCardBlackExtSize;	//G_Card_BlockExtSize;
	UINT16	smcCardBlockWholeSize; 	//G_Card_BlockWholeSize;
    #if (COPYDISK_5)
	UINT16	smcCardpageperBlock;	//G_Card_PagePerBlock;  // @122002@wyeo@support 256M nand
    #else
 	UINT8	smcCardpageperBlock;
    #endif

	UINT32	smcPhysicalbanlSize; 	//G_Card_PhysicalBankSize;
	
	UINT16	smcAllocateTableSize; 	// G_SMC_AllocatTableSize;
	UINT8	smcEntryTAbleSize; 		//G_SMC_EntryTableSize;
	
	UINT32	smcCisBlockAddr;		//G_SMC_CisBlockAddress;
	
	UINT8	smcEccByte[K_SMC_EccByteSize];	//G_SMC_EccByte[K_SMC_EccByteSize];
	UINT8	smcEccErr; 						//G_SMC_EccErr = 0;
	UINT16	smcErrCode;						//G_SMC_ErrCode = 0;
	UINT16	smcFirstCISFound;				//G_SMC_FirstCISFound;	// K_Check_Nand_Bad_Block_Infomation
	UINT32	smcEccErrCount;					//G_SMC_EccErrCount;
	UINT32	smcEcc1BitErrCount; 			//G_SMC_Ecc1BitErrCount;
	UINT32	smcEcc2BitErrCount;				//G_SMC_Ecc2BitErrCount;
	
	UINT16	nandReserveBlockTotal; 			//G_NAND_ReservBlockTotal; 
	UINT16	nandReserveBlockMax;			//G_NAND_ReservBlockMax;
	UINT16	smcCisBlockFound; 				//G_SMC_CisBlockFound;
	UINT32	smcNewCisBlockAddr;				//G_SMC_NewCisBlockAddress;

	#if 1//(K_HAVE_HAL)
	UINT8   smcAddrMode; //G_SMC_AddrMode;
	#endif
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	UINT8	smcBuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE]; //G_SMC_BuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE];
	#endif

    #if (COPYDISK_5)
    // @122002@wyeo@support 256M nand, begin
        UINT32	smcGoodSectorPerCard;           // G_SMC_GoodSectorPerCard
        UINT8	nandMakerByte;                  // G_NANDF_Maker_Byte
        UINT8	nand3RD_Byte;                   // G_NANDF_3RD_Byte
        UINT8	nand4TH_Byte;                   // G_NANDF_4TH_Byte          
        UINT8	nand5TH_Byte;                   // G_NANDF_5TH_Byte
        UINT8	nandCategory;                   // G_NANDF_Category // 0 is old, 1 is new and SMC2_
        UINT16	nandPageSize;                   // G_NANDF_PageSize
        UINT16	nandPageExtSize;                // G_NANDF_PageExtSize
        UINT32	nandBlockSize;                  // G_NANDF_BlockSize
        UINT8	nandBlockSizeShift;             // G_NANDF_BlockSizeShift
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
        UINT8	nandPageLastByte;               // G_NANDF_PageLastByte
        #endif
        // @122002@wyeo@support 256M nand, end
        #endif

	#endif
} COPYFILE_SAVE_XDATA, *PCOPYFILE_SAVE_XDATA;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//from dos16_dir.c
// public
xdata   UINT16	G_DOS_CurrDirCluster;
xdata 	UINT32 	G_DOS_FileSize;
xdata	UINT32 	G_DOS_SDRAM_WorkAddr;
xdata	UINT32 	G_DOS_SDRAM_WorkSize;
xdata   UINT16	G_DOS_CreateTime;
xdata   UINT16	G_DOS_CreateDate;
//xdata   UINT8	G_DOS_FdbFound; // patch 4.4-2@wyeo@abnormal action recovery

//ada@0501 for DCF
xdata   UINT8   G_DOS_FileAttribute;
xdata   UINT32	G_DOS_FdbAddress;

//from dos16_fat.c
// public
xdata	UINT8	G_DOS_SystemId;
xdata	UINT16	G_DOS_SectorSize;
xdata	UINT32	G_DOS_ClusterSize = 1;
xdata	UINT16	G_DOS_SectorPerFat;
xdata	UINT16	G_DOS_TotalRootDir;
xdata	UINT8	G_DOS_FileSystemType;

xdata	UINT32	G_DOS_FatAddr;
//xdata	UINT32	G_DOS_FatEndAddr;
//xdata	UINT32	G_DOS_Fat2Addr;
xdata	UINT32	G_DOS_FatSize;

xdata	UINT32	G_DOS_RootDirAddr;
xdata	UINT32	G_DOS_DataAddr;

xdata	UINT16	G_Card_TotalSizeMB;
xdata	UINT16	G_DOS_TotalFreeCluster;
//xdata	UINT16	G_DOS_ErrCode;	//move to smc_core.c
xdata	UINT32	G_Card_SectorPerCard;
xdata	UINT16	G_Card_BytePerSector;

xdata	UINT8	G_File_AudioEnable = 0;
xdata	UINT8	G_File_SDRAM_Read =	0;
xdata	UINT32	G_DOS_SDRAM_NextIntRetAddr;
xdata	UINT32	G_DOS_SDRAM_FAT1Addr;
xdata	UINT8	G_DOS_Stop_Initial = FALSE;
xdata	UINT8	G_DOS_Initial_Status;

xdata	UINT32	G_DOS_SectorPerCard;
xdata	UINT32	G_DOS_HiddenSector;
xdata	UINT16	G_DOS_ReserveSector;
xdata	UINT32	G_DOS_PBSAddr;

xdata	UINT8	G_DOS_CardErrDetected =	0; // patch	4.4-2@wyeo@abnormal	action recovery
xdata	UINT16	G_DOS_MaxCluster;

//richie@vc0422
#if	1//(K_HAVE_HAL)
data	BIT	G_USB_IO_Bulk2 = 0;
#endif

// WWWW0408
#if	(K_DOS_HaveLinearFAT)
xdata	UINT32	G_DOS_SDRAM_LinearFATAddr;
xdata	UINT8	G_DOS_UseLinearFAT;
xdata	UINT16	G_DOS_LinearFATMax;
//xdata	UINT16	G_DOS_LinearFATTotal;
//xdata	UINT16	G_DOS_LinearFATIndex;
//data	BIT		G_DOS_FATIsLinear;
#endif

/*#if	(K_HAVE_HAL || CODE_IS_MULTIBANK	|| (SMC_OPTION == 0	&& NANDF_OPTION	== 0) || \
	((K_MEDIA_ONLYONE == 0)	&& (NANDF_OPTION !=	0) && (SMC_OPTION == 0)) )
*/
#if 1
xdata	UINT16	G_DOS_Head;
xdata	UINT16	G_DOS_SectorPerTrack;
xdata	UINT16	G_DOS_Cylinder;
#endif

xdata	UINT16	G_DOS_SectorPerCluster;

// private
xdata	UINT16	G_DOS_FreeFatTable[K_DOS_FreeFatTableSize];
xdata	UINT16	G_DOS_FreeTableIndex;
xdata	UINT16	G_DOS_PrevCluster;
xdata	UINT32	G_DOS_FatNextAddr;
xdata	UINT16	G_DOS_FatNextCluster;
xdata	UINT8	G_DOS_FatNextFlag;
xdata	UINT16	G_Card_BlockSize;
xdata	UINT8	G_Card_BlockSizeShift;
xdata	UINT16	G_Media_Behavior;
//xdata	UINT16	G_DOS_FileEndCluster;
xdata	UINT32	G_Card_TotalCardSize;

//from dos16_file.c
xdata	UINT32 	G_DOS_SDRAM_MaxAddr;
xdata	UINT32	G_DOS_FilePos;

//xdata   UINT32   G_File_HdrLogAddr;
xdata   UINT32   G_File_HdrPhyAddr;
xdata   UINT16    G_File_HdrLogBlock;
//xdata   UINT32   G_File_FrameHdrLogAddr;
xdata   UINT32   G_File_FrameHdrPhyAddr;
xdata   UINT16    G_File_FrameHdrLogBlock;

xdata   UINT32   G_File_ReservePhyAddr[3];
xdata   UINT16    G_File_ReserveLogBlock[3];
xdata   UINT32   G_File_FrameReservePhyAddr[3];
xdata   UINT16    G_File_FrameReserveLogBlock[3];

//xdata	UINT32	G_DOS_DstStartLogAddr;
//xdata	UINT16	G_DOS_LeftClusterSize;
xdata   UINT16  G_DOS_StartClusterNumber;
//xdata	UINT16	G_DOS_FileCurrCluster;
xdata	UINT16	G_DOS_FileStartCluster;
//xdata	UINT16	G_DOS_FileClusterIndex;
//xdata	UINT32	G_DOS_FileSizeKeep;
xdata	UINT16	G_DOS_FreeTableTotal;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#define	FIND_UNFRAG_CLUSTER	1

#if 1//(COPY2CARD_OPTION && SINGLE_DIRECTORY == 0)
#if Dos16_copydisk_c_noused
xdata UINT32 G_SDRAM_COPYDISK_TempBufAddr;
xdata UINT32 G_SDRAM_COPYDISK_fdbTempAddr1;
xdata UINT32 G_SDRAM_COPYDISK_fdbTempAddr2;
xdata UINT32 G_SDRAM_COPYDISK_SaveFATAddr;
xdata UINT32 G_SDRAM_COPYDISK_SaveDcfDirAddr;
xdata UINT32 G_SDRAM_COPYDISK_SaveSMCBankBufAddr;
xdata UINT32 G_SDRAM_COPYDISK_SaveImgBufAddr;

UINT8	COPYDISK_CopyCurDir(UINT8 srcMedia, UINT8 dstMedia, UINT8 *curDirName, UINT8 matchCond, UINT32 refCon, UINT8 flag, UINT8 storageInit, COPYFILE_SAVE_XDATA *pSaveSrcGlobal,  COPYFILE_SAVE_XDATA *pSaveDstGlobal);
UINT8	COPYDISK_CopyFile(UINT8 srcMedia, UINT8 dstMedia, UINT16 startCluster, UINT8 *name, UINT8 flag, COPYFILE_SAVE_XDATA * pSaveSrcGlobal, COPYFILE_SAVE_XDATA * pSaveDstGlobal);

UINT8	COPYDISK_Initialize(UINT8 srcMedia, UINT8 dstMedia, UINT8 bCopyCond, COPYFILE_SAVE_XDATA *pSrcGlobal, COPYFILE_SAVE_XDATA *pDstGlobal);
void	COPYDISK_SaveGlobal(COPYFILE_SAVE_XDATA* pGlobal);
void	COPYDISK_RestoreGlobal(COPYFILE_SAVE_XDATA* pGlobal);
UINT8	COPYDISK_SetTransferMode(UINT8 cardType);
UINT8	COPYDISK_SwitchCard(UINT8 *srcMedia, UINT8 *dstMedia, UINT8 direction);
UINT8 	COPYDISK_StorageInitialize(UINT8 cardType);
void 	COPYDISK_SetTempBuffer(UINT32 copyDiskWorkingBufAddr);
void 	CopyDisk_SetFileName(UINT8 *name, UINT16 fileIndex);
UINT8 	CopyDisk_CreateNewDir1(void);
UINT8 	CopyDisk_CheckDcfDirName(UINT8 *fdb);
UINT8 	CopyDisk_CheckDcfFileName(UINT8 *fdb);
UINT16	CopyDisk_FindUnfragmentSector(void);
UINT8 	CopyDisk_SMReadCluster2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 readSize);

UINT8 COPYDISK_TotalFileCnt(UINT16 dirIndex, UINT32 *pFileCount); /* CopyDisk get total file count */

UINT32 DOS_ClusterLogAddr(UINT32 startCluster);

UINT32 G_saveCurrDirCluster;
xdata UINT32	totalReadTime, totalWriteTime;
xdata UINT32 totalFileCount = 0;
#endif
xdata UINT32 curFileCount = 0;
UINT8 	CopyDisk_CreateNewDir1(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if 0
UINT8 USR_CopyAllDir(UINT32 copyDiskTempBuffer, UINT8 direction) USING_0
{
	UINT8 	sts = TRUE;
	UINT8 	srcMedia, dstMedia;
    UINT8 	saveCardType;
    UINT16    saveDir1Cnt, saveCurCluster;
	
	curFileCount = 0;
	M_DOS_ClearError();
	File_SendMessage( K_File_UI_Start_CopyDisk, 0 );

	sts = TRUE;
	#if ( NANDF_OPTION == 1 || DOSRAM_OPTION == 1)
	
 	COPYDISK_SetTempBuffer(copyDiskTempBuffer);

   	saveCardType = G_Card_Type;
    saveDir1Cnt = G_USR_Dir1Cluster;
    saveCurCluster = G_DOS_CurrDirCluster;
	sts = COPYDISK_SwitchCard(&srcMedia, &dstMedia, direction);
	if (sts)
	{
//Mask by Phil @ 08.04.30	
/*
	#if 1//(K_HAVE_HAL)
        HAL_ConfigGTimer(5); //up count and timebase 1 ms 
        HAL_WriteGTimer(0);
        HAL_StartGTimer(); 
		totalReadTime = 0;
		totalWriteTime = 0;
	#endif
*/	
		sts = COPYDISK_CopyDir(srcMedia, dstMedia, K_DCF_ALL_DCF_FILE, K_DCF_OUR_TYPE,0);
		
		G_Card_Type = saveCardType;
		G_ucStorageType = G_Card_Type;
	    COPYDISK_StorageInitialize(G_Card_Type);
        G_USR_Dir1Cluster = saveDir1Cnt;
        G_DOS_CurrDirCluster = saveCurCluster;
        dcfFileInitialize();
        //G_USR_FileIndex++;  // [33792] By phil@08.11.13 #Already added in dcf init
    }
	#endif
/*
	#if 1//(K_HAVE_HAL)
	{
		UINT32 time;

		HAL_GlobalReadGTimer(&time);
		HAL_StopGTimer();
//		printf("USR_CopyAllDir time %ld\n", time);
//		printf("totalReadTime = %ld\n", totalReadTime);
//		printf("totalWriteTime = %ld\n", totalWriteTime);
	}
	#endif*/

	File_SendMessage( K_File_UI_End_CopyDisk, 0 );
	
	return sts;
}
#endif
UINT8 CopyDisk_DirNameGet(UINT16 rem, UINT8 *name) USING_0
{
	UINT8	i, ch;
	if( rem < 100 ) rem += 100;
	for ( i = 0; i < 12; i++ )
		name[i]  = G_USR_Dir1Name[i];
	
	for (i = 2; i != 0xff ; i--)
	{
		ch = (UINT8) (rem % 10) + '0';
		rem /= 10;
		name[i] = ch;
	}                
	name[8]  = '\0';
	return TRUE;
}
UINT8 USR_1FileBaseCpy(UINT32 copyDiskTempBuffer, UINT16 fileIndex, UINT8 direction, UINT8 *dirName ) USING_0
{
	PBFile_t FileInfo;
	UINT32 fd0,fd1,size,cnt,seg;
	UINT32 srcIndex = fileIndex;
	//printf("fileIndex=%u, srcIndex=%u, dir=%s\n",fileIndex, srcIndex, dirName);
	if(direction == K_Copy_Nand2Card){
		G_Card_Type = K_MEDIA_NANDF;
		DIR_Change("C:\\DCIM\\");
		DIR_Change("100MEDIA");
	}
	else{
		G_Card_Type = K_MEDIA_SDF;
		DIR_Change("D:\\DCIM\\");
		DIR_Change(dirName);
	}
	if(fileIndex == 0){ 
		dcfFileInitialize();
		if( G_USR_FileIndex <= 1 ){
			return FAIL;
		}
		fileIndex = G_USR_FileIndex - 1;	
	}
	//printf("fileIndex=%u\n",fileIndex);
	if ((pbFileInfoGet(fileIndex, &FileInfo) != SUCCESS)){
		return FAIL;
	}
	//printf("Src File %s\n",FileInfo.FileName);
	fd0 = DOS_FileOpen(FileInfo.FileName, 'r');
	//printf("fd0=%lu\n",fd0);
	if(fd0 == 0) {
		DOS_FileClose(fd0);
	}
	if(direction == K_Copy_Nand2Card){
		G_Card_Type = K_MEDIA_SDF;
		DIR_Change("D:\\DCIM\\");
		DIR_Change(dirName);
	}
	else{
		G_Card_Type = K_MEDIA_NANDF;
		DIR_Change("C:\\DCIM\\");
		DIR_Change("100MEDIA");
	}
	dcfBaseNameGet(FileInfo.FileName,srcIndex);
	//printf("target File %s, index= %lu\n",FileInfo.FileName, srcIndex);
	fd1 = DOS_FileOpen(FileInfo.FileName, 'w');
	//printf("fd0=%lu\n",fd0);
	if(fd1 == 0) {
		DOS_FileClose(fd0);
	}
	DOS_FileInfoGet(fd0, FILE_SIZE, (UINT8 *)&size);
	//printf("size=%lu\n",size);
	cnt = 0;
	while(1){ 
		if( size > 65536){
			if(direction == K_Copy_Nand2Card){
				G_Card_Type = K_MEDIA_NANDF;
			}
			else{
				G_Card_Type = K_MEDIA_SDF;
			}
			seg = DOS_FileRead(fd0, copyDiskTempBuffer, 65536, cnt);
			if(direction == K_Copy_Nand2Card){
				G_Card_Type = K_MEDIA_SDF;
			}
			else{
				G_Card_Type = K_MEDIA_NANDF;
			}
			DOS_FileWrite(fd1, copyDiskTempBuffer, 65536);
			size -= 65536;
		}
		else if( size > 0 ){
			if(direction == K_Copy_Nand2Card){
				G_Card_Type = K_MEDIA_NANDF;
			}
			else{
				G_Card_Type = K_MEDIA_SDF;
			}
			seg = DOS_FileRead(fd0, copyDiskTempBuffer, size, cnt);
			if(direction == K_Copy_Nand2Card){
				G_Card_Type = K_MEDIA_SDF;
			}
			else{
				G_Card_Type = K_MEDIA_NANDF;
			}
			DOS_FileWrite(fd1, copyDiskTempBuffer, size);
			size = 0;
		}
		else{
			break;
		}
		cnt += seg;
		//printf("cnt=%lu,seg=%lu\n",cnt,seg);			
	}
	DOS_FileClose(fd0);
	DOS_FileClose(fd1);
}

UINT8 USR_Copy1File(UINT32 copyDiskTempBuffer, UINT16 fileIndex, UINT8 direction) USING_0
{
	UINT8		sts = TRUE;

	M_DOS_ClearError();
	File_SendMessage( K_File_UI_Start_Copy1File, G_USR_FileCount ); // hlc 0714
	curFileCount = 0;

	#if ( NANDF_OPTION == 1 || DOSRAM_OPTION == 1 )
	{
		UINT8 name[12];		
		CopyDisk_DirNameGet(G_USR_Dir1Count, name);
		USR_1FileBaseCpy(copyDiskTempBuffer, fileIndex, direction, name);
		diskInitialize(1);
		///printf("G_USR_FileCount=%u\n",G_USR_FileCount);
	}
#endif
	
	File_SendMessage( K_File_UI_End_Copy1File, G_USR_FileCount ); // hlc 0714

	return sts;
 }


UINT8 USR_Copy1Dir(UINT32 copyDiskTempBuffer, UINT16 dirIndex, UINT8 direction) USING_0
{
	UINT8					sts = TRUE;

	//UINT32 t0,t1;
	//sp1kHalReadGTimer(&t0);
	dirIndex = dirIndex;

	M_DOS_ClearError();
	File_SendMessage( K_File_UI_Start_Copy1Dir, G_USR_Dir1Count ); // hlc 0714
	curFileCount = 0;

	#if ( NANDF_OPTION == 1 || DOSRAM_OPTION == 1)

	{
		UINT16 fileIndex;
		UINT8 name[12];

		CopyDisk_CreateNewDir1();
		CopyDisk_DirNameGet(G_USR_Dir1Count, name);

		if(direction == K_Copy_Nand2Card){
			DIR_Change("C:\\DCIM\\");
			DIR_Change("100MEDIA");
		}
		else{
			DIR_Change("D:\\DCIM\\");
			DIR_Change("100MEDIA");
		}
		sts = dcfFileInitialize();
		//printf("G_USR_FileCount=%u\n",G_USR_FileCount);
		for( fileIndex = 1; fileIndex <= G_USR_FileCount; fileIndex ++ ){	
			USR_1FileBaseCpy(copyDiskTempBuffer, fileIndex, direction, name);
		}
		diskInitialize(1);
		//printf("G_USR_FileCount=%u\n",G_USR_FileCount);
		//sp1kHalReadGTimer(&t1);
//		printf("CpyAll=%lu\n",t1-t0);
	}
	#endif

	File_SendMessage( K_File_UI_End_Copy1Dir, G_USR_Dir1Count ); // hlc 0714

   	return sts;
 }
 

#if Dos16_copydisk_c_noused
UINT8 COPYDISK_Initialize(UINT8 srcMedia, UINT8 dstMedia, UINT8 bCopyCond, COPYFILE_SAVE_XDATA *pSrcGlobal, COPYFILE_SAVE_XDATA *pDstGlobal) USING_0
{
    UINT8 	sts = TRUE;
    UINT32   srcFatSize;
	bCopyCond = bCopyCond;
	// reserve 	2*512 bytes  for temporary memory moving..
	G_SDRAM_COPYDISK_fdbTempAddr1 = G_SDRAM_COPYDISK_TempBufAddr;
	G_SDRAM_COPYDISK_fdbTempAddr2 = G_SDRAM_COPYDISK_fdbTempAddr1+(K_DOS_SectorSize>>1);

    srcFatSize = G_DOS_FatSize;

	// Copy source fat data in SDRAM to  G_SDRAM_COPYDISK_SaveFATAddr before switch cards
	G_SDRAM_COPYDISK_SaveFATAddr = G_SDRAM_COPYDISK_fdbTempAddr2 + (K_DOS_SectorSize>>1);
	sts = DOS_ReadMoreSector(K_DOS_DstDram, G_DOS_FatAddr, G_SDRAM_COPYDISK_SaveFATAddr, G_DOS_FatSize);
	if (!sts)
		return FALSE;

	// Make whole table and save the table to G_SDRAM_COPYDISK_SaveSMCBankBufAddr
	#if ( NANDF_OPTION == 1 && SMC_OPTION == 1 )
	G_SDRAM_COPYDISK_SaveSMCBankBufAddr = G_SDRAM_COPYDISK_SaveFATAddr + (G_DOS_FatSize>>1);
	G_DOS_SDRAM_WorkAddr = G_SDRAM_COPYDISK_SaveSMCBankBufAddr;
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		SMC_BuildBankTable(0xff);
    #endif
	#endif

	// Save Source storage globals
	G_DOS_SDRAM_FAT1Addr = G_SDRAM_COPYDISK_SaveFATAddr;

	COPYDISK_SaveGlobal(pSrcGlobal);

	// Set destination as active media
	G_Card_Type = dstMedia;	
	G_ucStorageType = G_Card_Type;


	COPYDISK_StorageInitialize(G_Card_Type);

			
 	#if ( NANDF_OPTION == 1 && SMC_OPTION == 1 )
	G_SDRAM_COPYDISK_SaveImgBufAddr = G_SDRAM_COPYDISK_SaveSMCBankBufAddr + G_DOS_SDRAM_WorkSize;
	#else
	G_SDRAM_COPYDISK_SaveImgBufAddr = G_SDRAM_COPYDISK_SaveFATAddr + (srcFatSize>>1)/*(G_DOS_FatSize>>1)*/;
	#endif

	// Save Destination storage globals
	COPYDISK_SaveGlobal(pDstGlobal);
	COPYDISK_SetTransferMode(srcMedia);
	COPYDISK_RestoreGlobal(pSrcGlobal);

	return sts;
 }


UINT8 COPYDISK_SetTransferMode(UINT8 cardType) USING_0
{
	UINT8 sts = TRUE;

	G_Card_Type = cardType;

	switch (cardType)
	{
	#if ( DOSRAM_OPTION == 1)
	case K_MEDIA_DOSRAM:
		sts=DRAM_CardDetect();
		break;
	#endif

	#if ( SDF_OPTION == 1)
	case K_MEDIA_SDF:
		sts=SD_CardDetect(K_MEDIA_SDF);
		break;
	#endif
	
	#if ( EMMC_OPTION == 1)
	case K_MEDIA_EMMC:
		sts=SD_CardDetect(K_MEDIA_EMMC);
		break;
	#endif

	#if ( SMC_OPTION == 1 || NANDF_OPTION == 1)
	case K_MEDIA_SMC:
	case K_MEDIA_Nandf:
		SMC_HwInitialize();
		SMC_CardDetect();
		break;
	#endif
	#if ( CFC_OPTION == 1)
	case K_MEDIA_CFC:
		sts = CF_CardDetect();
		break;
	#endif
	default:
		break;
	}

		return sts;
 }


 void  COPYDISK_RestoreGlobal( COPYFILE_SAVE_XDATA* pSaveGlobal) USING_0
 {
	#if (SMC_OPTION == 1 && NANDF_OPTION ==1)
	UINT16 idx;
	#endif

	if (!pSaveGlobal)
		return;
	G_DOS_SystemId 					= pSaveGlobal->dosSystemId;
	G_DOS_SectorPerFat 				= pSaveGlobal->dosSectorPerFat;
 	G_DOS_TotalRootDir 				= pSaveGlobal->dosTotalRootDir;
	G_DOS_RootDirAddr 				= pSaveGlobal->dosRootDirAddr;
	G_DOS_TotalFreeCluster 			= pSaveGlobal->dostotalfreeCluster;
	G_Card_SectorPerCard 			= pSaveGlobal->cardSectorPerSector;
	G_Card_BytePerSector 			= pSaveGlobal->cardBytePerSector;
	G_DOS_SDRAM_NextIntRetAddr 		= pSaveGlobal->dosSDRAMNextIntRetAddr;
	G_DOS_SectorPerCard 			= pSaveGlobal->dosSectorPerCard;     
	G_DOS_HiddenSector 				= pSaveGlobal->dosHiddenSector;
	G_DOS_ReserveSector 			= pSaveGlobal->dosReserveSector;
	G_DOS_PBSAddr 					= pSaveGlobal->dosPBSAddr;
	G_DOS_Head 						= pSaveGlobal->dosHead;
	G_DOS_SectorPerTrack 			= pSaveGlobal->dosSectorPerTrack;
	G_DOS_Cylinder 					= pSaveGlobal->dosCylinder;
	G_DOS_FileStartCluster 			= pSaveGlobal->dosFileStartCluster;
	G_DOS_SDRAM_MaxAddr 			= pSaveGlobal->dosMaxAddr;
	G_DOS_FilePos 					= pSaveGlobal->dosFilePos;
	G_DOS_CurrDirCluster 			= pSaveGlobal->dosCurrDirCluster;
	G_DOS_FileSize 					= pSaveGlobal->dosFileSize;
	G_DOS_CreateTime 				= pSaveGlobal->dosCreateTime;
	G_DOS_CreateDate 				= pSaveGlobal->dosCreateDate;
	G_DOS_CurrDirCluster 			= pSaveGlobal->currDirCluster;
	G_DOS_ClusterSize 				= pSaveGlobal->dosCluserSize;
	G_DOS_SectorPerCluster 			= pSaveGlobal->sectorPerCluser;
	G_DOS_SectorSize 				= pSaveGlobal->sectorSize;
	G_DOS_DataAddr 					= pSaveGlobal->dosDataAddr;
	G_DOS_FileSize 					= pSaveGlobal->dosFilesize;
	G_DOS_FileSystemType 			= pSaveGlobal->fileSystemType;
	G_DOS_FatAddr 					= pSaveGlobal->dosFatAddr;
	G_DOS_SDRAM_FAT1Addr 			= pSaveGlobal->SDRAMFat1Addr;
	G_USR_TotalDirectory 			= pSaveGlobal->totalDir;
//	G_USR_Dir1CountIndex 			= pSaveGlobal->dir1CountIndex;
	G_USR_MaxDir1Count 				= pSaveGlobal->maxDir1Count;
	G_USR_Dir1Count 				= pSaveGlobal->dir1Count;
//	G_USR_MaxOurDir1CountIndex 		= pSaveGlobal->maxdir1CountIndex;
	G_USR_Dir1Cluster 				= pSaveGlobal->dir1Cluster;
	G_USR_FileType 					= pSaveGlobal->fileType; 
	G_USR_FileIndex 				= pSaveGlobal->fileindex;
	G_USR_FileCount 				= pSaveGlobal->fileCount;
	G_DOS_PrevCluster				= pSaveGlobal->preCluster;
	G_DOS_FatNextAddr				= pSaveGlobal->fatNextAddr;
	G_DOS_FatNextCluster			= pSaveGlobal->fatNextCluster;
	G_DOS_FatNextFlag				= pSaveGlobal->fatNextFlag;
    G_DOS_MaxCluster                = pSaveGlobal->dosMaxCluster; /* bi-dir */
    G_USR_Dir0Cluster               = pSaveGlobal->dir0Cluster; /* bi-dir */

	// restore SMC globals
	// Save SMC globals
	#if (SMC_OPTION == 1 && NANDF_OPTION == 1)
	G_DOS_SDRAM_WorkAddr  			= pSaveGlobal->smcBankBufferAddr;
	G_Card_TotalSizeMB 				= pSaveGlobal->cardToatalSizeMB;
	G_Card_BlockSize 				= pSaveGlobal->cardBlockSize;
	G_Card_BlockSizeShift 			= pSaveGlobal->cardBlockSizeShift;
	G_Media_Behavior 				= pSaveGlobal->mediaBehavior;
	G_Card_TotalBank 				= pSaveGlobal->smcCardTotalBank;
	G_Card_CurrBank 				= pSaveGlobal->smcCardCurrBank;
	G_Card_BlockExtSize 			= pSaveGlobal->smcCardBlackExtSize ;
	G_Card_BlockWholeSize 			= pSaveGlobal->smcCardBlockWholeSize;
	G_Card_PagePerBlock 			= pSaveGlobal->smcCardpageperBlock;
	G_Card_PhysicalBankSize 		= pSaveGlobal->smcPhysicalbanlSize;
	
	G_SMC_AllocatTableSize 			= pSaveGlobal->smcAllocateTableSize;
	G_SMC_EntryTableSize 			= pSaveGlobal->smcEntryTAbleSize;
	G_SMC_CisBlockAddress 			= pSaveGlobal->smcCisBlockAddr; 

	for (idx = 0; idx < K_SMC_EccByteSize; idx++)
		 G_SMC_EccByte[idx] 		= pSaveGlobal->smcEccByte[idx];

	G_SMC_EccErr					= pSaveGlobal->smcEccErr;
	G_SMC_ErrCode 					= pSaveGlobal->smcErrCode;
	G_SMC_FirstCISFound 			= pSaveGlobal->smcFirstCISFound;
	G_SMC_EccErrCount 				= pSaveGlobal->smcEccErrCount;
	G_SMC_Ecc1BitErrCount 			= pSaveGlobal->smcEcc1BitErrCount;
	G_SMC_Ecc2BitErrCount 			= pSaveGlobal->smcEcc2BitErrCount;
	G_NAND_ReservBlockTotal 		= pSaveGlobal->nandReserveBlockTotal;
	G_NAND_ReservBlockMax 			= pSaveGlobal->nandReserveBlockMax;
	G_SMC_CisBlockFound 			= pSaveGlobal->smcCisBlockFound;
	G_SMC_NewCisBlockAddress 		= pSaveGlobal->smcNewCisBlockAddr;

	#if 1//(K_HAVE_HAL)
	G_SMC_AddrMode 					= pSaveGlobal->smcAddrMode;
	#endif
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		for (idx = 0; idx < K_SMC_BUILD_BANK_MAP_SIZE; idx++)
			G_SMC_BuildBankMap[idx]  	= pSaveGlobal->smcBuildBankMap[idx];
  		SMC_UpdateBankTable( G_Card_CurrBank, TRUE );
	#else
		SMC_MakeTable(G_Card_CurrBank);
	#endif

    #if (COPYDISK_5)
// @122002@wyeo@support 256M nand, begin
        G_SMC_GoodSectorPerCard = pSaveGlobal->smcGoodSectorPerCard;
        G_NANDF_Maker_Byte      = pSaveGlobal->nandMakerByte;
        G_NANDF_3RD_Byte        = pSaveGlobal->nand3RD_Byte;
        G_NANDF_4TH_Byte        = pSaveGlobal->nand4TH_Byte;          
        G_NANDF_5TH_Byte        = pSaveGlobal->nand5TH_Byte;
        G_NANDF_5TH_Byte        = pSaveGlobal->nandCategory; // 0 is old, 1 is new and SMC2_
        G_NANDF_PageSize        = pSaveGlobal->nandPageSize;
        G_NANDF_PageExtSize     = pSaveGlobal->nandPageExtSize;
        G_NANDF_BlockSize       = pSaveGlobal->nandBlockSize;
        G_NANDF_BlockSizeShift  = pSaveGlobal->nandBlockSizeShift;
        /*
        G_NANDF_HasOldBlk = pSaveGlobal->nandHasOldBlk;
        G_NANDF_StartLogAddr = pSaveGlobal->nandStartLogAddr;
        G_NANDF_EndBlkAddr = pSaveGlobal->nandEndBlkAddr;
        G_NANDF_NewPhyAddr = pSaveGlobal->nandNewPhyAddr;
        G_NANDF_OldPhyStartAddr = pSaveGlobal->nandOldPhyStartAddr;
        G_NANDF_OldPhyStartAddr = pSaveGlobal->nandBlockNum;
        G_NANDF_OldPhyAddr = pSaveGlobal->nandOldPhyAddr;
        G_NANDF_FindHeaderPage = pSaveGlobal->nandFindHeaderPage;
        G_NANDF_SetHdrPageAddr = pSaveGlobal->nandSetHdrPageAddr;
        G_NANDF_FirstEnter = pSaveGlobal->nandFirstEnter;
        G_NANDF_RsvIndex = pSaveGlobal->nandRsvIndex;
        G_NANDF_StartPage = pSaveGlobal->nandStartPage;
        */

    // @122002@wyeo@support 256M nand, end
    #endif

	#if ( DOSRAM_OPTION == 1 )	
	if (G_Card_Type == K_MEDIA_DOSRAM)
	{
		if (G_ulDRAMSTORFAT1AddrTemp != 0xFFFFFFFF)
		{
			G_DOS_SDRAM_FAT1Addr = G_ulDRAMSTORFAT1AddrTemp;
		}
	}
	#endif
	#endif
 }

void  COPYDISK_SaveGlobal( COPYFILE_SAVE_XDATA* pSaveGlobal) USING_0
{
	#if (SMC_OPTION == 1 && NANDF_OPTION ==1)
	UINT16 idx;
	#endif
	if (!pSaveGlobal)
		return;

	pSaveGlobal->dosSystemId 			= G_DOS_SystemId;
	pSaveGlobal->dosSectorPerFat 		= G_DOS_SectorPerFat;
 	pSaveGlobal->dosTotalRootDir 		= G_DOS_TotalRootDir;
	pSaveGlobal->dosRootDirAddr 		= G_DOS_RootDirAddr;
	pSaveGlobal->dostotalfreeCluster 	= G_DOS_TotalFreeCluster;
	pSaveGlobal->cardSectorPerSector 	= G_Card_SectorPerCard;
	pSaveGlobal->cardBytePerSector 		= G_Card_BytePerSector;
	pSaveGlobal->dosSDRAMNextIntRetAddr = G_DOS_SDRAM_NextIntRetAddr;
	pSaveGlobal->dosSectorPerCard		= G_DOS_SectorPerCard;     
	pSaveGlobal->dosHiddenSector 		= G_DOS_HiddenSector;
	pSaveGlobal->dosReserveSector 		= G_DOS_ReserveSector;
	pSaveGlobal->dosPBSAddr 			= G_DOS_PBSAddr;
	pSaveGlobal->dosHead 				= G_DOS_Head;
	pSaveGlobal->dosSectorPerTrack 		= G_DOS_SectorPerTrack;
	pSaveGlobal->dosCylinder 			= G_DOS_Cylinder;
	pSaveGlobal->dosFileStartCluster 	= G_DOS_FileStartCluster;
	pSaveGlobal->dosMaxAddr 			= G_DOS_SDRAM_MaxAddr;
	pSaveGlobal->dosFilePos 			= G_DOS_FilePos;
	pSaveGlobal->dosCurrDirCluster 		= G_DOS_CurrDirCluster;
	pSaveGlobal->dosFileSize 			= G_DOS_FileSize;
	pSaveGlobal->dosCreateTime 			= G_DOS_CreateTime;
	pSaveGlobal->dosCreateDate 			= G_DOS_CreateDate;
	pSaveGlobal->currDirCluster 		= G_DOS_CurrDirCluster;
	pSaveGlobal->dosCluserSize 			= G_DOS_ClusterSize;
	pSaveGlobal->sectorPerCluser 		= G_DOS_SectorPerCluster;
	pSaveGlobal->sectorSize 			= G_DOS_SectorSize;
	pSaveGlobal->dosDataAddr 			= G_DOS_DataAddr;
	pSaveGlobal->dosFilesize 			= G_DOS_FileSize;
	pSaveGlobal->fileSystemType 		= G_DOS_FileSystemType;
	pSaveGlobal->dosFatAddr 			= G_DOS_FatAddr;
	pSaveGlobal->SDRAMFat1Addr 			= G_DOS_SDRAM_FAT1Addr;  
	pSaveGlobal->totalDir 				= G_USR_TotalDirectory;
//	pSaveGlobal->dir1CountIndex 		= G_USR_Dir1CountIndex;
	pSaveGlobal->maxDir1Count 			= G_USR_MaxDir1Count;
//	pSaveGlobal->maxdir1CountIndex 		= G_USR_MaxOurDir1CountIndex;
	pSaveGlobal->dir1Count 				= G_USR_Dir1Count;
	pSaveGlobal->dir1Cluster 			= G_USR_Dir1Cluster;
	pSaveGlobal->fileType 				= G_USR_FileType; 
	pSaveGlobal->fileindex 				= G_USR_FileIndex;
	pSaveGlobal->fileCount 				= G_USR_FileCount;
	pSaveGlobal->preCluster				= G_DOS_PrevCluster;
	pSaveGlobal->fatNextAddr 			= G_DOS_FatNextAddr;
	pSaveGlobal->fatNextCluster 		= G_DOS_FatNextCluster;
	pSaveGlobal->fatNextFlag 			= G_DOS_FatNextFlag;
    pSaveGlobal->dosMaxCluster          = G_DOS_MaxCluster; /*bi-dir */
    pSaveGlobal->dir0Cluster            = G_USR_Dir0Cluster; /* bi-dir */
	// Save SMC globals
	#if (SMC_OPTION == 1 && NANDF_OPTION ==1)
	pSaveGlobal->smcBankBufferAddr 		= G_DOS_SDRAM_WorkAddr;
	pSaveGlobal->cardToatalSizeMB 		= G_Card_TotalSizeMB;
	pSaveGlobal->cardBlockSize 			= G_Card_BlockSize;
	pSaveGlobal->cardBlockSizeShift 	= G_Card_BlockSizeShift;
	pSaveGlobal->mediaBehavior 			= G_Media_Behavior;
	pSaveGlobal->smcCardTotalBank 		= G_Card_TotalBank;
	pSaveGlobal->smcCardCurrBank 		= G_Card_CurrBank;
	pSaveGlobal->smcCardBlackExtSize 	= G_Card_BlockExtSize;
	pSaveGlobal->smcCardBlockWholeSize 	= G_Card_BlockWholeSize;
	pSaveGlobal->smcCardpageperBlock 	= G_Card_PagePerBlock;
	pSaveGlobal->smcPhysicalbanlSize 	= G_Card_PhysicalBankSize;
	
	pSaveGlobal->smcAllocateTableSize 	= G_SMC_AllocatTableSize;
	pSaveGlobal->smcEntryTAbleSize  	= G_SMC_EntryTableSize;
	pSaveGlobal-> smcCisBlockAddr 		= G_SMC_CisBlockAddress;

	for (idx = 0; idx < K_SMC_EccByteSize; idx++)
		pSaveGlobal->smcEccByte[idx] = G_SMC_EccByte[idx];

	pSaveGlobal->smcEccErr 				= G_SMC_EccErr;
	pSaveGlobal->smcErrCode 			= G_SMC_ErrCode;
	pSaveGlobal->smcFirstCISFound 		= G_SMC_FirstCISFound;
	pSaveGlobal->smcEccErrCount 		= G_SMC_EccErrCount;
	pSaveGlobal->smcEcc1BitErrCount 	= G_SMC_Ecc1BitErrCount;
	pSaveGlobal->smcEcc2BitErrCount 	= G_SMC_Ecc2BitErrCount;
	
	pSaveGlobal->nandReserveBlockTotal 	= G_NAND_ReservBlockTotal; 
	pSaveGlobal->nandReserveBlockMax 	= G_NAND_ReservBlockMax;
	pSaveGlobal->smcCisBlockFound 		= G_SMC_CisBlockFound;
	pSaveGlobal->smcNewCisBlockAddr		= G_SMC_NewCisBlockAddress;

	#if 1//(K_HAVE_HAL)
		pSaveGlobal->smcAddrMode = G_SMC_AddrMode;
	#endif
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		for (idx = 0; idx < K_SMC_BUILD_BANK_MAP_SIZE; idx++)
			pSaveGlobal->smcBuildBankMap[idx] = G_SMC_BuildBankMap[idx];
   		SMC_UpdateBankTable( G_Card_CurrBank, FALSE );
	#else
 		SMC_MakeTable(G_Card_CurrBank);
	#endif

    #if (COPYDISK_5)
    // @122002@wyeo@support 256M nand, begin
        pSaveGlobal->smcGoodSectorPerCard = G_SMC_GoodSectorPerCard;
        pSaveGlobal->nandMakerByte        = G_NANDF_Maker_Byte;
        pSaveGlobal->nand3RD_Byte         = G_NANDF_3RD_Byte;
        pSaveGlobal->nand4TH_Byte         = G_NANDF_4TH_Byte;          
        pSaveGlobal->nand5TH_Byte         = G_NANDF_5TH_Byte;
        pSaveGlobal->nandCategory         = G_NANDF_Category; // 0 is old, 1 is new and SMC2_
        pSaveGlobal->nandPageSize         = G_NANDF_PageSize;
        pSaveGlobal->nandPageExtSize      = G_NANDF_PageExtSize;
        pSaveGlobal->nandBlockSize        = G_NANDF_BlockSize;
        pSaveGlobal->nandBlockSizeShift   = G_NANDF_BlockSizeShift;
        /*
        pSaveGlobal->nandHasOldBlk = G_NANDF_HasOldBlk;
        pSaveGlobal->nandStartLogAddr = G_NANDF_StartLogAddr;
        pSaveGlobal->nandEndBlkAddr = G_NANDF_EndBlkAddr;
        pSaveGlobal->nandNewPhyAddr = G_NANDF_NewPhyAddr;
        pSaveGlobal->nandOldPhyStartAddr = G_NANDF_OldPhyStartAddr;
        pSaveGlobal->nandBlockNum = G_NANDF_OldPhyStartAddr;
        pSaveGlobal->nandOldPhyAddr = G_NANDF_OldPhyAddr;
        pSaveGlobal->nandFindHeaderPage = G_NANDF_FindHeaderPage;
        pSaveGlobal->nandSetHdrPageAddr = G_NANDF_SetHdrPageAddr;
        pSaveGlobal->nandFirstEnter = G_NANDF_FirstEnter;
        pSaveGlobal->nandRsvIndex = G_NANDF_RsvIndex;
        pSaveGlobal->nandStartPage = G_NANDF_StartPage;
        */

// @122002@wyeo@support 256M nand, end
    #endif
	#endif
 }

void COPYDISK_SetTempBuffer(UINT32 copyDiskWorkingBufAddr) USING_0
{
	G_SDRAM_COPYDISK_TempBufAddr = copyDiskWorkingBufAddr;
}

UINT8 COPYDISK_SwitchCard(UINT8 *srcMedia, UINT8 *dstMedia, UINT8 direction)
{
	UINT8 sts, saveCardType;
    UINT8 interalMedia = K_MEDIA_None;
    UINT8 externalMedia = K_MEDIA_None;

	sts = TRUE;

	saveCardType = G_Card_Type;
//ada test
	#if ( NANDF_OPTION == 1 || DOSRAM_OPTION == 1 )
/*		#if ( CFC_OPTION == 1)
			G_Card_Type = K_MEDIA_CFC; 
			if (CF_CardDetect())
			{
				externalMedia = K_MEDIA_CFC;
			}
			else if (CF_Initialize(TRUE))
			{
				externalMedia = K_MEDIA_CFC;	
			}
		#endif

		#if ( SMC_OPTION == 1)
			G_Card_Type = K_MEDIA_SMC; 
			if (SMC_CardDetect())
				externalMedia = K_MEDIA_SMC;	
			else if (SMC_Initialize(TRUE))
				externalMedia = K_MEDIA_SMC;	
		#endif

		#if (SDF_OPTION || EMMC_OPTION)
			G_Card_Type = K_MEDIA_SDF; 
			if (SD_CardDetect(K_MEDIA_SDF))
				externalMedia = K_MEDIA_SDF;
			else if (SD_Initialize(TRUE))
				externalMedia = K_MEDIA_SDF;				
			else {
				G_Card_Type = K_MEDIA_EMMC; 
				if (EMMC_CardDetect())
					externalMedia = K_MEDIA_EMMC;
				else if (EMMC_Initialize(TRUE))
				externalMedia = K_MEDIA_EMMC;
		   }
		#endif	
*/
		switch (G_Card_Type)
		{
			#if ( CFC_OPTION == 1)
			case K_MEDIA_CFC:
				if (CF_CardDetect())
					externalMedia = K_MEDIA_CFC;
				else if (CF_Initialize(TRUE))
					externalMedia = K_MEDIA_CFC;	
				break;
			#endif

			#if ( SMC_OPTION == 1)
			case K_MEDIA_SMC:
				if (SMC_CardDetect())
					externalMedia = K_MEDIA_SMC;	
				else if (SMC_Initialize(TRUE))
					externalMedia = K_MEDIA_SMC;	
				break;
			#endif

			#if (SDF_OPTION == 1)
			case K_MEDIA_SDF:
                #if 0
				if (SD_CardDetect(K_MEDIA_SDF))
					externalMedia = K_MEDIA_SDF;
				else if (SD_Initialize(TRUE))

               #endif
					externalMedia = K_MEDIA_SDF;				
				break;		
			#endif
#if TAG_TODO
			#if (EMMC_OPTION == 1)
			case K_MEDIA_EMMC:
				if (SD_CardDetect(K_MEDIA_SDF))
					externalMedia = K_MEDIA_EMMC;
				else if (EMMC_Initialize(TRUE))
					externalMedia = K_MEDIA_EMMC;
				break;
			#endif
#endif		
		}

        if (direction == 0) /* Internal nand to card */
            *dstMedia = externalMedia;
        else
            *srcMedia = externalMedia;
          
	// modify by wesley
//	G_ucStorageType = G_Card_Type;
//		sts = COPYDISK_StorageInitialize(G_Card_Type);
		
		G_Card_Type =  saveCardType; 
		if ((externalMedia) != K_MEDIA_None)	// Set nand flash as active storage.
		{
			#if (NANDF_OPTION == 1)
				interalMedia =  K_MEDIA_NANDF;
			//ada test
			HAL_FlashMode(K_MEDIA_None,K_MEDIA_NANDF,1);
			HAL_FlashMode(K_MEDIA_None,K_MEDIA_SMC,1);
			HAL_FlashMode(K_MEDIA_None,K_MEDIA_CFC,1);
			HAL_FlashMode(K_MEDIA_None,K_MEDIA_SDF,1);
			HAL_FlashMode(K_MEDIA_None,K_MEDIA_EMMC,1);

			    if (interalMedia == G_Card_Type)
				{
                    SMC_CardDetect();
				}
			#endif
			#if (DOSRAM_OPTION == 1)
				interalMedia =  K_MEDIA_DOSRAM;
			    if (interalMedia == G_Card_Type)
                    DRAM_CardDetect();

			#endif

            if (direction == 0) /* Internal nand to card */
                *srcMedia = interalMedia;
            else
                *dstMedia = interalMedia;

           	//printf("G_Card_Type = %bx, srcMedia = %bx, dstMedia = %bx, sts = %bx\n", G_Card_Type, *srcMedia, *dstMedia, sts);
		   	//ada test
           	//if (*srcMedia != G_Card_Type)   
		   	{ 
    			G_ucStorageType = G_Card_Type = *srcMedia;
			    G_saveCurrDirCluster = G_DOS_CurrDirCluster;
			    sts = COPYDISK_StorageInitialize(G_Card_Type); 
                #if 0
                if (*srcMedia == G_Card_Type) // Fixed NewTrend copy 1 file bug
                    G_DOS_CurrDirCluster = G_saveCurrDirCluster;
                #endif
            }
		}
		else // no card is inserted
			sts = FALSE;
	#else
		sts = FALSE;
	#endif


		return sts;
}


UINT8 COPYDISK_StorageInitialize(UINT8 cardType) USING_0 
{
	UINT8 sts; 

	#if (CARD_OPTION == 1)
          diskMemInit();
          diskGlobalInit();
	#endif

	G_ucStorageType = cardType;
    G_Card_Type = cardType;
    G_ucStorageType = 0xff;

   	#if ( DOSRAM_OPTION == 1 )	
	if (G_Card_Type == K_MEDIA_DOSRAM)
	{
		if (G_ulDRAMSTORFAT1AddrTemp != 0xFFFFFFFF)
		{
			G_DOS_SDRAM_FAT1Addr = G_ulDRAMSTORFAT1AddrTemp;
		}
	}
	#endif

	sts = diskDetect();
	if (sts)
	{
	    sts = diskInitialize(TRUE);
	     if (sts == FALSE)
	     {
		//	printf("======= diskInitialize fail and format\n");
            sts = diskFormat(0xc1);
            if (sts)
                    sts = diskInitialize(sts);
	      }
	 }

	return sts;
}


void CopyDisk_SetFileName(UINT8 *name, UINT16 fileIndex) USING_0
{
        UINT8 i;
        UINT8 ch;

        for (i = 7; i > 3; i--)
        {
                ch = (UINT8) (fileIndex % 10) + '0';
                fileIndex /= 10;
                name[i] = ch;
        }
}


#endif
UINT8 CopyDisk_CreateNewDir1(void) USING_0
{
	UINT8		sts = TRUE;
	UINT8 		name[12];
	UINT16 		rem;

	M_DOS_ClearError();
	if(DOS_SUCCESS != DIR_Change("\\DCIM")){
		return FALSE;
	}
	G_USR_MaxDir1Count = 100;
	while ( G_USR_MaxDir1Count < 999 )
	{
		//G_USR_MaxDir1Count ++;
		G_USR_Dir1Count = G_USR_MaxDir1Count;
		rem = G_USR_Dir1Count;
		CopyDisk_DirNameGet(rem, name);
		if(DOS_SUCCESS == DIR_Change(name)){
			DIR_Change("\\DCIM");
			//G_USR_MaxOurDir1Count = G_USR_Dir1Count;
			G_USR_MaxDir1Count++;
			//G_USR_TotalDirectory++;
		}
		else{
			printf("DIR=%s\n",name);
			DIR_Create(name);
			DIR_Change(name);
			break;
		}
	}
	return sts;
}
#if Dos16_copydisk_c_noused

UINT8 CopyDisk_CheckDcfDirName(UINT8 *fdb) USING_0
{
     UINT8 i;

     for (i = 0; i < 3; i++)
     {
          if (('0' > fdb[i]) || (fdb[i] > '9'))
               return FALSE;
     }

    for (i = 3; i < 8; i++)
     {
//          if ((('A' > fdb[i]) || (fdb[i] > 'Z')) && (fdb[i] != '_'))
          if ( (('A' > fdb[i]) || (fdb[i] > 'Z')) && (('0' > fdb[i]) || (fdb[i] > '9')) && (fdb[i] != '_'))
               return FALSE;
     }

     return TRUE;
}

UINT8 CopyDisk_CheckDcfFileName(UINT8 *fdb) USING_0
{
     UINT8 i;

     for (i = 0; i < 4; i++)
     {
//          if ((('A' > fdb[i]) || (fdb[i] > 'Z')) && (fdb[i] != '_'))
          if ((('A' > fdb[i]) || (fdb[i] > 'Z')) && (('0' > fdb[i]) || (fdb[i] > '9')) && (fdb[i] != '_'))
          {
               //printf("Wrong name 1\n");
               return FALSE;
          }
     }

     for (i = 4; i < 8; i++)
     {
          if (('0' > fdb[i]) || (fdb[i] > '9'))
          {
               //printf("Wrong name 2\n");
               return FALSE;
          }
     }

     return TRUE;
}

#if (FIND_UNFRAG_CLUSTER)
UINT16 CopyDisk_FindUnfragmentSector(void) USING_0
{
	UINT16 	tempTableIndex = G_DOS_FreeTableIndex;
	UINT16	totalUnfrag = 0;

	if (G_DOS_FreeTableIndex >= G_DOS_FreeTableTotal)
		return totalUnfrag;

	totalUnfrag = 1;
	while (tempTableIndex < (G_DOS_FreeTableTotal-1))
	{
		if ((G_DOS_FreeFatTable[tempTableIndex] + 1) == (G_DOS_FreeFatTable[tempTableIndex+1]))
		{	
			totalUnfrag++;
		}
		else
			break;

		tempTableIndex++;
	}

	return totalUnfrag;
}
#endif

#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
UINT8 CopyDisk_SMReadCluster2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 readSize) USING_0
{
	UINT8 sts = TRUE;
	UINT32 blkStartLogAddr;
	UINT32 blkEndLogAddr;
	UINT16 blkSize;	
	UINT32 phyAddr = 0, reqLogAddr;
	UINT16 k = 0;
	UINT16 j;
	UINT32	totalReadSize;

	totalReadSize = readSize;

	reqLogAddr = logAddr;

	while (totalReadSize && (sts == TRUE))
	{
//		if (G_Media_Behavior & K_MEDIA_NewForOldBlk)
		{
        	blkStartLogAddr = (reqLogAddr >> G_Card_BlockSizeShift) << G_Card_BlockSizeShift;
        	blkEndLogAddr = blkStartLogAddr + G_Card_BlockSize;
        	blkSize = reqLogAddr - blkStartLogAddr;
//			printf("reqLogAddr = %lx, blkStartLogAddr =%lx, blkSize = %d\n",reqLogAddr, blkStartLogAddr,  blkSize);
	     }
	        
//		if (G_Media_Behavior & K_MEDIA_NewForOldBlk)
		{
			sts = SMC_ReadLpaToPpa(blkStartLogAddr, &phyAddr);
			phyAddr += blkSize;
		}
	
		k=0;
		j = G_DOS_ClusterSize - blkSize;
		while (k < j)
		{
			sts = diskPhySectorRead(reqLogAddr, phyAddr, ramAddr);
			ramAddr += (K_Card_PageSize>>1);
			if (sts)
			{
				reqLogAddr += K_Card_PageSize;
				k += K_Card_PageSize;

				#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
//				if (G_Media_Behavior & K_MEDIA_NewForOldBlk)
				{
        				phyAddr += K_Card_PageSize;
        				blkSize += K_Card_PageSize;
        				if (blkSize == G_Card_BlockSize)
        					break;
        			}
				#endif
			}
			else
				break;
		}

		totalReadSize -= k;
//		printf("totalReadSize  =%lx, k = %d\n", readSize, k);
	}

	return sts;
}

#endif

UINT8 COPYDISK_TotalFileCnt(UINT16 dirIndex, UINT32 *pFileCount) USING_0 /* CopyDisk get total file count */
{
    UINT8  sts = TRUE;
	UINT16 searchCluster,startCluster,i, j;
    UINT32 logAddr;
	UINT8 dirIndexFound = FALSE;

    *pFileCount = 0;

    searchCluster = G_USR_Dir0Cluster;
	while (searchCluster != 0xffff && sts) {
		logAddr = DOS_ClusterLogAddr(searchCluster);
		
       	for (j = 0; (j < G_DOS_ClusterSize ) && ( sts == TRUE ); j += K_DOS_SectorSize) {
            sts = DOS_ReadSector(K_DOS_DstDram, logAddr+j, G_SDRAM_COPYDISK_fdbTempAddr1);
       		if ( sts ) {
                for (i = 0; i < K_DOS_SectorSize; i += 32) {
                    SDRAM_Dram2Sram(G_SDRAM_COPYDISK_fdbTempAddr1, (UINT8 *)G_ucDataBuffer, 512);
                    if ((G_ucDataBuffer[i] != 0x00) && (G_ucDataBuffer[i] != 0xe5)&& (G_ucDataBuffer[i] != 0x2e)) {    
                        if (CopyDisk_CheckDcfDirName(&G_ucDataBuffer[i])) {
                            G_USR_Dir1Count = ((UINT16)(G_ucDataBuffer[i] - '0')) * 100 + ((UINT16)(G_ucDataBuffer[i+1] - '0')) * 10 + (UINT16)(G_ucDataBuffer[i+2] - '0');
                            
                            if ((G_USR_Dir1Count < 100) || (G_USR_Dir1Count >= 1000)) {
                                return FALSE;
                            }
                            else {                            
                                if (dirIndex > 0 && G_USR_Dir1Count ==  dirIndex)
								    dirIndexFound =  TRUE;

							    if (dirIndexFound || dirIndex == 0) {
                                    startCluster = ((UINT16) G_ucDataBuffer[i+26]) |
                                    (((UINT16) G_ucDataBuffer[i+27]) << 8);

                                    #if 0
                                    G_DOS_FileSize = DOS_ReadLong(G_ucDataBuffer, i+28);
                                    for (k = 0; k < 12; k++)
                                    curDirName[k] = G_ucDataBuffer[i+k];
                                    #endif
    
                                    G_USR_Dir1Cluster = startCluster;		
#if TAG_TODO
                                    DOS_SetCurrDir(startCluster);
#endif
                                    sts = dcfFileInitialize();
                                    
                                    *pFileCount += (UINT32)G_USR_FileCount;
                               }
							    if (dirIndexFound)
								   return TRUE;
                            }		
						}
           			}
                }
           	}
       	} //  end for j

		//searchCluster = DOS_GetNextCluster(searchCluster); //xian -- null function
   }

   return sts;
}

UINT32 DOS_ClusterLogAddr(UINT32 startCluster)
{
	UINT8 DevId;
	DevId = DOS_CurrDriverGet();
	return FAT_Cluster2Sector(DevId, startCluster);
}
#endif
#endif
