
/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		smccore.c

Abstract:

		Module uniquely belong to smc card access and charateristic

Environment:

		Keil C51 Compiler

Revision History:

		09/03/2001  	William Yeo    created  			   

--*/
//=============================================================================
//Header file
//=============================================================================

#include "general.h"
#include "reg_def.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_nand.h"

#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )

#ifdef DISK_DRV_NAND
//#include "cardui.h" 
#include "asicreg.h"
#include "impt.h"
//#include "cardebug.h"
//#include "cardcon.h"
//#include "cardfunc.h"
#include "cardvar.h"

#include "doslink.h"
#include "dosvar.h"

#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"

#include "smcdata.h"
#include "smcerr.h"
#include "solution.h"
#include "hal_dram.h"
#include "Sp1k_hal_api.h"
#include "dos32.h"

#include "reg_def.h"
// @012403@wyeo@support 256M nand, begin
#if 1//(K_SUPPORT_NEWNAND)
#include "smc2func.h"
#endif
#define K_DEBUG_SMC	0
#define USE_NEW_WR      0
// @012403@wyeo@support 256M nand, end

extern	xdata   UINT16 G_Card_BlockSize;
extern	xdata   UINT8 G_Card_BlockSizeShift;
extern	xdata	UINT16 G_Media_Behavior;

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//patch4.5@richie@msdc plug out exception
#include "uiflow.h"
#include "mtd_general.h"
#include "mtd_macro_api.h"

xdata   UINT8 G_Card_TotalBank;
xdata   UINT8 G_Card_CurrBank;
xdata   UINT16 G_Card_BlockExtSize;
xdata   UINT16 G_Card_BlockWholeSize;
xdata   UINT16 G_Card_PagePerBlock;	// @012403@wyeo@support 256M nand	
xdata   UINT32 G_Card_PhysicalBankSize;

#if (/*K_HAVE_HAL &&*/ SIMULATE == 0)
xdata   UINT16    G_SMC_AllocatTable[K_SMC_MaxAllocatTableSize] _at_ 0x1000;
xdata   UINT8    G_SMC_EntryTable[K_SMC_MaxEntryTableSize] _at_ 0x1800;
#else
xdata   UINT16 G_SMC_AllocatTable[K_SMC_MaxAllocatTableSize];
xdata   UINT8 G_SMC_EntryTable[K_SMC_MaxEntryTableSize];
#endif

xdata   UINT16 G_SMC_AllocatTableSize;
xdata   UINT8 G_SMC_EntryTableSize;

xdata   UINT32 G_SMC_CisBlockAddress;

xdata   UINT8 G_SMC_EccByte[K_SMC_EccByteSize];
xdata   UINT8 G_SMC_EccErr = 0;
xdata   UINT16 G_SMC_ErrCode = 0;
xdata   UINT16 G_SMC_FirstCISFound;	// K_Check_Nand_Bad_Block_Infomation
xdata	UINT32 G_SMC_EccErrCount;
xdata	UINT32 G_SMC_Ecc1BitErrCount;
xdata	UINT32 G_SMC_Ecc2BitErrCount;

// wyeo@0514
xdata   UINT16 G_NAND_ReservBlockTotal = 1; 
//xdata   UINT8 G_NAND_ReservBlockMax = 20;
xdata   UINT16 G_SMC_CisBlockFound;
xdata   UINT32 G_SMC_NewCisBlockAddress;

// @patch 4.5@wyeo@make table make faster, begin
#if (SIMULATE == 0)
xdata   UINT32   G_SMC_GoodSectorPerCard _at_ 0x10c;
#else
xdata   UINT32 G_SMC_GoodSectorPerCard;
#endif
//end

// @012403@wyeo@remove K_PreWriteOldBlock

#if 1//(K_HAVE_HAL)
xdata   UINT8 G_SMC_AddrMode;
#endif

#if ( K_SMC_BUILD_BANK_AT_SDRAM )
xdata	UINT8 G_SMC_BuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE];
#endif

// @012403@wyeo@support 256M nand, begin
xdata UINT8 G_NANDF_Maker_Byte;
xdata UINT8 G_NANDF_3RD_Byte;
xdata UINT8 G_NANDF_4TH_Byte;
xdata UINT8 G_NANDF_5TH_Byte;
xdata UINT8 G_NANDF_Category; // 0 is old, 1 is new and SMC2_
xdata UINT16 G_NANDF_PageSize;
xdata UINT16 G_NANDF_PageExtSize;
xdata UINT32 G_NANDF_BlockSize;
xdata UINT8 G_NANDF_BlockSizeShift;

xdata UINT8 G_NANDF_HasOldBlk;
xdata UINT32 G_NANDF_StartLogAddr;
xdata UINT32 G_NANDF_EndBlkAddr;
xdata UINT32 G_NANDF_NewPhyAddr;
xdata UINT32 G_NANDF_OldPhyStartAddr;
xdata UINT16 G_NANDF_BlockNum;
xdata UINT32 G_NANDF_OldPhyAddr;
xdata UINT8 G_NANDF_FindHeaderPage = 0;
xdata UINT8 G_NANDF_SetHdrPageAddr;
xdata UINT8 G_NANDF_FirstEnter;
xdata UINT8 G_NANDF_RsvIndex;
xdata UINT8 G_NANDF_StartPage;

//SUPPORT 512Mbit NAND FLASH FOR POWER FLASH
xdata UINT16 SMC_LogicalBlockPerBank=1000;

// @012403@wyeo@support 256M nand, end

xdata UINT16 G_NANDF_Bank0TotalBlk;	// @patch 5.1@012403@wyeo@fix cross nand bank bug
//xdata UINT16 G_NANDF_LastBankTotalBlk;
#if(SIMULATE == 0)
xdata UINT32 G_NANDF_BadBank _at_ 0xfc;	// @patch 5.1@012403@wyeo@fix cross nand bank bug
// each bit map to each bank, when bit is 1, 
// means that the bank have at least 1001 
// good block. if not, the nand could not be used.
#else
xdata UINT32 G_NANDF_BadBank;
#endif
xdata UINT8 G_NANDF_OnCache = 0;
xdata UINT8 G_NANDF_FristCache = 0;

xdata	UINT16	G_DOS_ErrCode;

//=============================================================================
//Program   	
//=============================================================================
//-----------------------------------------------------------------------------
//SMC_CardDetect
//-----------------------------------------------------------------------------
UINT8 SMC_CardDetect(void)
{
	UINT8 sts = TRUE;
	UINT8 deviceCode;

	HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_SDF, 1);
	sp1kHalWait(50);
	deviceCode = SMC_GetID();

	G_Card_BlockSize = 0x4000;  			//16K byte/block
	G_Card_BlockSizeShift = 14; 			//2^14 = 16K byte/block
	G_Card_BlockExtSize = 0x0200;   		//512 byte/block
	G_SMC_AllocatTableSize = 0x0400;		//1024 block/bank
	G_Card_PhysicalBankSize = 0x01000000;   //16 MB/bank = 1024 physical-block/bank * 16 KB/block
	G_Card_TotalBank = 1;

	G_NANDF_BlockSize = 0x4000; 			//16K byte/block
	G_NANDF_BlockSizeShift = 14;
	G_NANDF_Category = 0;
	G_NANDF_PageSize = 512;
	G_NANDF_PageExtSize = 16;
	SMC_LogicalBlockPerBank = 1000;

	switch (deviceCode) {

		case NAND_DEV_ID_64M_PF:
			G_Card_BlockExtSize = 0x1000;   		//4096 byte/block
			G_SMC_AllocatTableSize = 0x0200;		//512 block/bank
			G_Card_PhysicalBankSize = 0x04000000;   //64 MB/bank = 512 physical-block/bank * 128 KB/block
			G_Card_TotalSizeMB = 64;   		 //128 MB
			G_Card_TotalBank = 1;   				//1 banks of 64 MB
			G_SMC_AddrMode = 3;
			G_NANDF_Category = 1;
			G_NANDF_PageSize = 2048;
			G_NANDF_PageExtSize = 64;
			G_NANDF_BlockSize = 128 * 1024L;
			G_NANDF_BlockSizeShift = 17;
			SMC_LogicalBlockPerBank = 500;
			
			break;
	        
		case NAND_DEV_ID_128M_1:
			//2048 page 128 MB
		case NAND_DEV_ID_128M_2:
		case NAND_DEV_ID_128M_3:	//For TOSHIBA nand ID=0x98,0xd1;
			G_Card_BlockExtSize = 0x1000;   		//4096 byte/block
			G_SMC_AllocatTableSize = 0x0400;		//1024 block/bank
			G_Card_PhysicalBankSize = 0x08000000;   //128 MB/bank = 1024 physical-block/bank * 128 KB/block
			G_Card_TotalSizeMB = 128;   		 //128 MB
			G_Card_TotalBank = 1;   				//1 banks of 128 MB
			G_SMC_AddrMode = 3;
			G_NANDF_Category = 1;
			G_NANDF_PageSize = 2048;
			G_NANDF_PageExtSize = 64;
			G_NANDF_BlockSize = 128 * 1024L;
			G_NANDF_BlockSizeShift = 17;
			break;

		case NAND_DEV_ID_8G_1:
		case NAND_DEV_ID_8G_2:
			G_Card_TotalSizeMB = 0x2000;			//8GB
			G_Card_TotalBank = 64;  				//64 banks of 128 MB
			break;

		case NAND_DEV_ID_4G_1:
		case NAND_DEV_ID_4G_2:
			G_Card_TotalSizeMB = 0x1000;			//4GB
			G_Card_TotalBank = 32;  				//32 banks of 128 MB
			break;

		case NAND_DEV_ID_2G_1:
			//2048 page 2GB
		case NAND_DEV_ID_2G_2:
			/*case 0xd5:*/  	//2GB
			G_Card_TotalSizeMB = 0x0800;			//2GB
			G_Card_TotalBank = 16;  				//16 banks of 128 MB
			break;

		case NAND_DEV_ID_1G_1:
			//2048 page 1GB
		case NAND_DEV_ID_1G_2:
			/*case 0xd3: */ 	//1GB
			G_Card_TotalSizeMB = 0x0400;			//1GB
			G_Card_TotalBank = 8;   			   //8 banks of 128 MB
			break;

		case NAND_DEV_ID_512M_1:
			//2048 page 512 MB
		case NAND_DEV_ID_512M_2:
			/*case 0xdc:*/  	//512 MB
			G_Card_TotalSizeMB = 0x0200;			//512 MB
			G_Card_TotalBank = 4;   			   //4 banks of 128 MB
			break;

		case NAND_DEV_ID_256M_1:
			//2048 page 256 MB
		case NAND_DEV_ID_256M_2:
			G_Card_TotalBank = 2;   	 	//2 banks of 128 MB
			G_Card_TotalSizeMB = 256;    //256 MB
			break;

		case SMC_DEV_ID_128M_1:
			//512 page 128 MB
		case SMC_DEV_ID_128M_2:
			G_Card_TotalSizeMB = 0x0080;			//128 MB
			G_Card_TotalBank = 8;   				//8 banks of 16 MB
			G_SMC_AddrMode = 2; 					//mode of address cycle setting
			break;

		case SMC_DEV_ID_64M_1:
		case SMC_DEV_ID_64M_2:
			G_Card_TotalSizeMB = 0x0040;			//64 MB
			G_Card_TotalBank = 4;   				//4 banks of 16 MB
			G_SMC_AddrMode = 2; 					//mode of address cycle setting
			break;

		case SMC_DEV_ID_32M_1:
		case SMC_DEV_ID_32M_2:
			G_Card_TotalSizeMB = 0x0020;			//32 MB
			G_Card_TotalBank = 2;   				//2 banks of 16 MB
			G_SMC_AddrMode = 1; 					//mode of address cycle setting
			break;

		case SMC_DEV_ID_16M_1:
		case SMC_DEV_ID_16M_2:
			G_Card_TotalSizeMB = 0x0010;			//16 MB
			G_SMC_AddrMode = 1; 					//mode of address cycle setting
			break;

		case SMC_DEV_ID_8M_1:
		case SMC_DEV_ID_8M_2:
			//8 MB
			G_NANDF_BlockSize = 0x2000; 			//8K byte/block
			G_NANDF_BlockSizeShift = 13;
			G_Card_TotalSizeMB = 0x0008;			//8 MB
			G_Card_BlockSize = 0x2000;  			//8K byte/block
			G_Card_BlockSizeShift = 13; 			//2^13 = 8K byte/block
			G_Card_BlockExtSize = 0x0100;   		//256 byte/block
			G_Card_PhysicalBankSize = 0x00800000;   //8 MB/bank = 1024 physical-block/bank * 8 KB/block
			G_SMC_AddrMode = 1; 					//mode of address cycle setting
			break;

		default:
			//unknown
			printf("NF unknown:%bx\n", deviceCode);
			sts = FALSE;
			break;
	}

	if (G_Card_TotalSizeMB >= 0x0100) {
		G_Card_BlockExtSize = 0x1000;   		//4096 byte/block
		G_SMC_AllocatTableSize = 0x0400;		//1024 block/bank
		G_Card_PhysicalBankSize = 0x08000000;   //128 MB/bank = 1024 physical-block/bank * 128 KB/block
		G_SMC_AddrMode = 4;
		G_NANDF_Category = 1;
		G_NANDF_PageSize = 2048;
		G_NANDF_PageExtSize = 64;
		G_NANDF_BlockSize = 128 * 1024L;
		G_NANDF_BlockSizeShift = 17;
	}

	if (sts) {
		G_Card_BytePerSector = K_Card_PageSize;
		G_Card_SectorPerCard = (UINT32) G_Card_TotalSizeMB * 2000L;

		G_Card_TotalCardSize = G_Card_SectorPerCard << K_Card_PageSizeShift;
		G_Card_BlockWholeSize = G_NANDF_BlockSize + G_Card_BlockExtSize;	// @012403@wyeo@support 256M nand
		G_SMC_EntryTableSize = G_SMC_AllocatTableSize >> 3;

		//G_Card_TotalBlock = ((UINT32) G_Card_TotalSizeMB << 20) >> G_Card_BlockSizeShift;
		//printf("G_NANDF_BlockSizeShift =%bu, \n", G_NANDF_BlockSizeShift);
		//G_Card_PagePerBlock = G_NANDF_BlockSize >> K_Card_PageSizeShift;   // @012403@wyeo@support 256M nand
		//G_Card_PagePerBlock = 64;
		G_Card_PagePerBlock = G_NANDF_BlockSize / G_NANDF_PageSize;
		//printf("G_Card_PagePerBlock  = %u %04X xxx\n", G_Card_PagePerBlock, G_Card_PagePerBlock);
		G_Card_Supported |= (((UINT16) 1) << K_SMC_SUPPORTED_BIT);

		//@090902@wyeo@fix sector per card of smc, nand, begin
		G_Card_SectorPerCard -= (2L * G_Card_PagePerBlock); // @012403@wyeo@prevent format sudden plug problem

#if	0
	G_Card_SectorPerCard -= ((UINT32)G_NAND_ReservBlockTotal*G_Card_PagePerBlock);
	#endif

		// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
		//G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank;
		G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal - NAND_BANK0_SWAP_BLK;
		// @patch 5.1@012403@wyeo@fix cross nand bank bug, end
	}
	
	printf("NF_PagSize: %u\n", G_NANDF_PageSize);
	printf("NF_BlkSize: %lu\n", G_NANDF_BlockSize);
	printf("NF_TotSize: %uMB\n", G_Card_TotalSizeMB);
	#if 0
	//printf("G_Card_BlockExtSize    =%u, \n",G_Card_BlockExtSize);
	//printf("G_SMC_AllocatTableSize =%u, \n",G_SMC_AllocatTableSize);
	//printf("G_Card_PhysicalBankSize=%lu, \n",G_Card_PhysicalBankSize     );
	//printf("G_Card_TotalBank       =%bu, \n",G_Card_TotalBank 		   );
	//printf("G_Card_TotalSizeMB     =%u, \n",G_Card_TotalSizeMB		  );
	//printf("G_SMC_AddrMode         =%bu, \n",G_SMC_AddrMode   		   );
	//printf("G_NANDF_Category       =%bu, \n",G_NANDF_Category 		   );
	//printf("G_NANDF_PageSize       =%u, \n",G_NANDF_PageSize  		  );
	//printf("G_NANDF_PageExtSize    =%u, \n",G_NANDF_PageExtSize   	  );
	printf("G_NANDF_BlockSize      =%lu, \n", G_NANDF_BlockSize);
	printf("G_NANDF_BlockSizeShift =%bu, \n", G_NANDF_BlockSizeShift);

	//printf("G_Card_BytePerSector = %u\n",G_Card_BytePerSector);
	printf("G_Card_SectorPerCard = %lu ???\n", G_Card_SectorPerCard);
	printf("G_Card_TotalCardSize = %lu ???\n", G_Card_TotalCardSize);
	printf("G_Card_BlockWholeSize= %u\n", G_Card_BlockWholeSize);
	//printf("G_SMC_EntryTableSize = %bu\n",G_SMC_EntryTableSize);
	printf("G_Card_PagePerBlock  = %u xxx\n", G_Card_PagePerBlock);
	printf("=G_Card_Supported    = %04X\n", G_Card_Supported);
	//printf("G_NANDF_Bank0TotalBlk= %u\n",G_NANDF_Bank0TotalBlk);
	//	printf("G_NANDF_LastBankTotalBlk %d\n",G_NANDF_LastBankTotalBlk);
#endif
	if (sts == TRUE) {
		//patch 4.4-7@wyeo@remove debug message
		//printf("NAND detected, %uMB\n", G_Card_TotalSizeMB);
	}

	return sts;
}

//-----------------------------------------------------------------------------
//SMC_Initialize
//-----------------------------------------------------------------------------
UINT8 SMC_Initialize(UINT8 doDOSInitial)
{
	UINT8 sts = FALSE;
	UINT8 k;
	devAttr_t* pDev;//add by phil for disk driver lib release

	G_NANDF_BadBank = 0;// @patch 5.1@012403@wyeo@fix cross nand bank bug

	// @patch 5.1@012403@wyeo@remove K_PreWriteOldBlock

	// @patch 4.5@wyeo@make table make faster, begin
	G_SMC_Ecc1BitErrCount = 0;
	G_SMC_Ecc2BitErrCount = 0;
	//end

	G_SMC_EccErr = 0;
	G_SMC_ErrCode = 0;

	G_SMC_EccErrCount = 0;

	G_DOS_Initial_Status = 1;
	SMC_GetBehavior();
	G_DOS_CardErrDetected = 0;	// patch 4.4-2@wyeo@abnormal action recovery 

	// wyeo@0514 start // smc simulate nand
	/* //0528@wyeo
	#if (SMC_OPTION != 0 || NANDF_OPTION == 0)
	// when nand option only, respect customer value
	if (G_Card_Type == K_MEDIA_SMC)
	{
		G_NAND_ReservBlockTotal = 0; 
		G_NAND_ReservBlockMax = 20;
	}
	#endif
	*/
	G_SMC_NewCisBlockAddress = 0;
	// wyeo@0514 end

#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	// initial bank build table to 0 to indicate
	// that logical to physical table not yet build
	for (k = 0; k < K_SMC_BUILD_BANK_MAP_SIZE; k++)
		G_SMC_BuildBankMap[k] = 0;
#endif

	/* add by phil for disk driver lib release s*/
	pDev = sp1kDevHdlGet();
	pDev += DEV_ID_DISK;
	if (pDev->devType == DEV_TYPE_DISK_NAND) {
		sts = nandDetect();
	}
	/* add by phil for disk driver lib release e*/
	
	//printf("%s,%d,sts=%bx\n",__FILE__,__LINE__, sts);				
	if (sts) {
		//M_DEBUG_SHOWLED(0xfe);

#if 0//(SQUEEZE_CODE_SPACE>=8)
		SETMODE_ClearAsUploadMode();
#else
		SETMODE_ClearPostBuffer();  		//clear post buffer
		SETMODE_SetUploadMode();			//set upload mode
#endif

		if (doDOSInitial == 2)
			sts = TRUE;
		else {
			SMC_HwInitialize();
			sts = SMC_FormatCheck();
			//printf("%s,%d,sts=%lx\n",__FILE__,(UINT16)__LINE__, (UINT32)sts);				
			// @012403@wyeo@prevent format sudden plug problem, begin
			// cis not found, try assume is is at 0
			if (sts == FALSE) {
				//printf("SMC_FormatCheck not found = %lx\n", G_Card_SectorPerCard);

#if (NANDF_OPTION || K_SMC_AS_NANDF)
#if (K_SMC_AS_NANDF == 0)
				if (G_Card_Type == K_MEDIA_NANDF) {
					/* to next blk after nand hdr */	
					UINT32 phyAddr = G_SMC_CisBlockAddress + G_NANDF_BlockSize;
					//printf("1 %s,%d,=%lx\n",__FILE__,__LINE__, G_SMC_CisBlockAddress);				

					if (FALSE == SMC_SearchValidBlock(&phyAddr)) {
						return FALSE;
					}
					G_SMC_NewCisBlockAddress = phyAddr;
					//printf("1 %s,%d,G_SMC_CisBlockAddress=%lx\n",__FILE__,__LINE__, G_SMC_CisBlockAddress);				
					SMC_SearchValidBlock(&G_SMC_NewCisBlockAddress);
					G_SMC_CisBlockAddress = G_SMC_NewCisBlockAddress;
					// write cis address
					//printf("2 %s,%d,=%lx\n",__FILE__,__LINE__, G_SMC_CisBlockAddress);				
					SMC_CisIdi(G_SMC_NewCisBlockAddress);
				}
#endif
#endif
				sts = TRUE;
			}
			// @012403@wyeo@prevent format sudden plug problem, end
		}
		if (sts) {
			sts = FALSE;

			if (doDOSInitial != 2) {
				G_Card_CurrBank = 0;
{				
#if TAG_TODO
				logBank_t bank;
				UINT16 addr;
#endif
				UINT16 tagAddr = SMC2_RedunCard2Sram;
				
//				HAL_CpuImemSdGet(&bank, &addr);
//				HAL_CpuImemSdSet(LOG_BANK_5, tagAddr);
				
				SMC_MakeTable(0);
				SMC_BuildBankTable(0xff);	
				
//				HAL_CpuImemSdSet(bank, addr);
}
			}

#if 1//(SQUEEZE_CODE_SPACE<=2)
			if (!G_DOS_Stop_Initial)
									#endif
			{
				if (doDOSInitial) {
					if (DOS_Initialize()) {
						sts = TRUE;
						G_DOS_Initial_Status = 0xFF;
						//M_DEBUG_SHOWLED(0xe0);
					}
					// @patch5.1@wyeo@fix don't correctly set initial status@begin
#if 0
						else
			{
							G_DOS_Initial_Status = 5;
			}
			#endif
					// @patch5.1@wyeo@fix don't correctly set initial status@end

				} else {
					sts = TRUE;
				}
			}
#if 1//(SQUEEZE_CODE_SPACE<=2)
			else
				G_DOS_Initial_Status = 2;
#endif
		} else
			G_DOS_Initial_Status = 4;
		SETMODE_SetCameraIdle();
	} else {
		G_DOS_Initial_Status = 3;
		//printf("%s,%d,G_DOS_Initial_Status=%bx\n", __FILE__, __LINE__, G_DOS_Initial_Status);
	}

	return sts;
}

//-----------------------------------------------------------------------------
//SMC_LCard2Sram
//-----------------------------------------------------------------------------
UINT8 SMC_LCard2Sram(UINT32 logAddr)
{
	UINT32 phyAddr;
	UINT8 sts;

	if (SMC_ReadLpaToPpa(logAddr, &phyAddr)) {
		SMC_Card2Sram(phyAddr);
		sts = TRUE;
	} else
		sts = FALSE;

	return sts;
}

#if 1//def DOS32_SUPPORT
//-----------------------------------------------------------------------------
//SMC_LCard2Dram
//-----------------------------------------------------------------------------
UINT8 SMC_LCard2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
	//UINT32 t0,t1;
	UINT32 phyAddr, nrPage;
	UINT8 sts;
	UINT32 z0, z1, z2;
	UINT32 i, sectorPerpage, tmp;
	sts = size;
	sectorPerpage = G_NANDF_PageSize;
	MTD_SelfShiftR8(sectorPerpage);
	sectorPerpage = sectorPerpage >> 1;
	//Mask by phil @ 08.07.11 for already set before entering this function
	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//first non-block alignment data
	nrPage = G_NANDF_BlockSize;// 9.2us
	MTD_SelfShiftR8(nrPage);// 3.6us
	nrPage = nrPage >> 1;// 9.2us
	
	z0 = G_NANDF_BlockSize - (logAddr % G_NANDF_BlockSize); 
	//printf("%s %u z0=%lx\n", __FILE__, (unsigned short)__LINE__,z0);
	//printf("z0: %lx\n", (UINT32)z0);
	
	if (size > z0) {
		size = size - z0;
	} else {
		z0 = size;
		size = 0;
	}
	SDRAM_CardSetStartAddress(ramAddr, FALSE);
	if (z0 > 0) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z0);
			return sts;
		}
		//printf("%s %u phyAddr=%lx,ramAddr=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,phyAddr,ramAddr,logAddr);
		logAddr = logAddr + z0;
	
		/*phil: div and shift operation in 8051 is too slowly...*/
		//z0 = z0 / 512; // 51us
		//z0 = z0 >> 9; // 16.8us
		MTD_SelfShiftR8(z0);// 3.6us
		z0 = z0 >> 1;// 9.2us
		i = z0;
		if( ramAddr ){
			#if 0
			if(!G_NANDF_Category){
				while (i) {
					SMC_Card2Dram(phyAddr, ramAddr);
					phyAddr += 512L;
					ramAddr += 256L;
					i--;
				}
			}
			else 
			#endif
			{
				while (i) {
					if ((sectorPerpage == 1) || (((UINT16)phyAddr) & (G_NANDF_PageSize - 1)) || i < sectorPerpage) {
						//printf("1phyAddr: %lx\n", (UINT32)phyAddr);
						SMC_Card2Dram(phyAddr, ramAddr);
						phyAddr += 512L;
						ramAddr += 256L;
						i--;
					} else {
						//printf("%lx\n", i);
						//printf("2phyAddr: %lx\n", (UINT32)phyAddr);
						SMC_Card2DramFast(phyAddr, ramAddr);
						phyAddr += 2048UL;
						ramAddr += 1024UL;
						i = i - 4;					
					}
				}
			}			
		}
		else{
			while (i) {
				SMC_Card2Usb(phyAddr);// WW5W
				phyAddr += 512L;
				i--;
			}
		}
	}
	//second is block alignment data
	//z1 = size / G_NANDF_BlockSize;
	z1 = size >> G_NANDF_BlockSizeShift;
	//printf("%s %u z1=%lx\n", __FILE__, (unsigned short)__LINE__,z1);
	//printf("z1: %lx\n", (UINT32)z1);
	
	//SDRAM_CardSetStartAddress( ramAddr, TRUE );
	tmp = nrPage / sectorPerpage;
	while (z1) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		//tmp = nrPage / sectorPerpage;
		i = tmp;
		if(ramAddr){
			while (i) {
				SMC_Card2DramFast(phyAddr, ramAddr);
				phyAddr += G_NANDF_PageSize;
				ramAddr += (G_NANDF_PageSize >> 1);
				i--;
			}
		}
		else{
			while (i) {
				SMC_Card2UsbFast(phyAddr);// WW5W
				phyAddr += G_NANDF_PageSize;
				i--;
			}
		}
		z1 --;
		logAddr += G_NANDF_BlockSize;
	}
	//3rd is the last block alignment data
	
	z2 = size % G_NANDF_BlockSize;
	//printf("%s %u z2=%lx\n", __FILE__, (unsigned short)__LINE__,z2);
	//printf("z2: %lx\n", (UINT32)z2);	
	
	MTD_SelfShiftR8(z2);
	z2 = z2 >> 1;
	//SDRAM_CardSetStartAddress( ramAddr, TRUE );
	if (z2 > 0) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		tmp = z2 / sectorPerpage;
		z2 = z2 % sectorPerpage; 
		if( ramAddr ){
			i = tmp;
			while (i) {
				SMC_Card2DramFast(phyAddr, ramAddr);
				phyAddr += G_NANDF_PageSize;
				ramAddr += (G_NANDF_PageSize >> 1);
				i--;
			}
			i = z2;
			while (i) {
				SMC_Card2Dram(phyAddr, ramAddr);// WW5W
				phyAddr += 512L;
				ramAddr += 256L;
				i--;
			}
		}
		else{
			i = tmp;
			while (i) {
				SMC_Card2UsbFast(phyAddr);// WW5W
				phyAddr += G_NANDF_PageSize;
				i--;
			}
			i = z2;
			while (i) {
				SMC_Card2Usb(phyAddr);// WW5W
				phyAddr += 512L;
				i--;
			}
		}
	}
	//HAL_GlobalReadGTimer(&t1);
	sts = TRUE;
	return sts;
}
#else
//-----------------------------------------------------------------------------
//SMC_LCard2Dram
//-----------------------------------------------------------------------------
UINT8 SMC_LCard2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
	UINT32 phyAddr;
	UINT8 sts;
	sts = size;
	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//printf("R %s %u logAddr=%lx,ramAddr=%lx,size=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,ramAddr,size);
	SDRAM_CardSetStartAddress(ramAddr, FALSE);
	if (SMC_ReadLpaToPpa(logAddr, &phyAddr)) {
		SMC_Card2Dram(phyAddr, ramAddr);// WW5W
		sts = TRUE;
	} else
		sts = FALSE;

	return sts;
}
#endif

#if 1//def DOS32_SUPPORT
UINT8 SMC_LDram2Card(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
#if 0//(USE_NEW_WR == 1)
	UINT32 i, nrPage;
	UINT32 oldPhyAddr,newPhyAddr,src,dst,sts;
	UINT32 z0, z1, z2, tmp;
	UINT16 blkNum;
	//printf("W %s %u logAddr=%lx,ramAddr=%lx,size=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,ramAddr,size);
	//HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);

	nrPage = G_NANDF_BlockSize;// 9.2us
	MTD_SelfShiftR8(nrPage);// 3.6us
	nrPage = nrPage >> 1;// 9.2us

	MTD_SelfShiftL8(size);// 3.6us
	size = size << 1;// 9.2us
	//First non-block alignment
	//dumpData(512, (ramAddr), 1);
	z0 = 0;
	SDRAM_CardSetStartAddress(ramAddr, TRUE);
	if(logAddr % G_NANDF_BlockSize){
		z0 = G_NANDF_BlockSize - (logAddr % G_NANDF_BlockSize); 
		if (size > z0) {
			size = size - z0;
		} else {
			z0 = size;
			size = 0;
		}
	}
	//printf("%s %u z0=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,z0,logAddr);
	if (z0 != 0) {
		sts = SMC_WriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z0);
			//return sts;
		}
		tmp = logAddr % G_NANDF_BlockSize;
		logAddr = logAddr + z0;
		MTD_SelfShiftR8(z0);// 3.6us
		z0 = z0 >> 1;// 9.2us
		MTD_SelfShiftR8(tmp);// 3.6us
		tmp = tmp >> 1;// 9.2us
		src = oldPhyAddr;
		dst = newPhyAddr;
		if(oldPhyAddr != 0xffffffff ) {
			for (i = 0; i < tmp ; i ++) {
//printf("%s %u src=%lx,dst=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst);
				SMC_Card2Sram(src);
				SMC_Sram2Card(dst,blkNum);
				src += 512L;
				dst += 512L;
			}
		}
		if( ramAddr ){
			for (i = tmp; i < (tmp + z0) ; i ++) {
//printf("%s %u src=%lx,dst=%lx,ramAddr=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst,ramAddr);
				SMC_Dram2Card(dst, blkNum,ramAddr);
				ramAddr += 256L;
				src += 512L;
				dst += 512L;
			}
		}
		else{
			for (i = tmp; i < (tmp + z0) ; i ++) {
				SMC_Usb2Card(dst, blkNum);
				src += 512L;
				dst += 512L;
			}
		}
		if(oldPhyAddr != 0xffffffff ) {
			for (i = (tmp + z0); i < nrPage ; i ++) {
//printf("%s %u src=%lx,dst=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst);
				SMC_Card2Sram(src);
				SMC_Sram2Card(dst,blkNum);
				src += 512L;
				dst += 512L;
			}
			SMC_EraseOneBlock(oldPhyAddr);
		}
	}
	//Next block alignment
	z1 = size >> G_NANDF_BlockSizeShift;
	//printf("%s %u z1=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,z1,logAddr);
	while (z1) {
		sts = SMC_WriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			//return sts;
		}
		src = oldPhyAddr;
		dst = newPhyAddr;
		if( ramAddr ){ 
			for (i = 0; i < nrPage ; i ++) {
//printf("%s %u src=%lx,dst=%lx,ramAddr=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst,ramAddr);
				//SMC_Dram2Card(dst, blkNum,ramAddr);
				ramAddr += 256L;
				src += 512L;
				dst += 512L;
			}
		}
		else{
			for (i = 0; i < nrPage ; i ++) {
				SMC_Usb2Card(dst, blkNum);
				src += 512L;
				dst += 512L;
			}
		}
		z1--;
		logAddr += G_NANDF_BlockSize;
		size -= G_NANDF_BlockSize;
	}
	//Last remain not enough 1 block
	z2 = size % G_NANDF_BlockSize;
	//printf("%s %u z2=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,z2,logAddr);
	MTD_SelfShiftR8(z2);// 3.6us
	z2 = z2 >> 1;// 9.2us
	if (z2 > 0) {
		sts = SMC_WriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			//return sts;
		}
		src = oldPhyAddr;
		dst = newPhyAddr;
		if( ramAddr ){
			for (i = 0; i < z2 ; i ++) {
//printf("%s %u src=%lx,dst=%lx,ramAddr=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst,ramAddr);
				SMC_Dram2Card(dst, blkNum,ramAddr);
				ramAddr += 256L;
				src += 512L;
				dst += 512L;
			}
		}
		else{
			for (i = 0; i < z2 ; i ++) {
				SMC_Usb2Card(dst, blkNum);
				src += 512L;
				dst += 512L;
			}
		}
		if(oldPhyAddr != 0xffffffff ) {
			for (i = (nrPage - z2); i < nrPage ; i ++) {
//printf("%s %u src=%lx,dst=%lx\n", __FILE__, (unsigned short)__LINE__,src,dst);
				SMC_Card2Sram(src);
				SMC_Sram2Card(dst,blkNum);
				src += 512L;
				dst += 512L;
			}
			SMC_EraseOneBlock(oldPhyAddr);
		}
	}
	return sts;
#else
	UINT8 sts = TRUE;
	if(G_NANDF_OnCache == 1){//for AVI use
		//NANDF_LDram2CardCache(logAddr,  ramAddr,  size << 9);
		NANDF_LDram2CardCache(logAddr,  ramAddr,  size);
	}
	else{
	{
	//	UINT8 sts;
		UINT32 numSector;
		UINT8 endContinuous;
	
		//printf("W %s %u logAddr=%lx,ramAddr=%lx,size=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,ramAddr,size);
		//HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
		G_NANDF_FindHeaderPage = 0;
		sts = NANDF_LWriteStart(K_DOS_SrcDram, logAddr, size/*>>9*/);//eric@20080502
		if (sts) {
			numSector = size/*>>9*/;//eric@20080502
			sts = NANDF_LWriteContinue(K_DOS_SrcDram, logAddr, ramAddr, &numSector);
			if (sts)
				sts = NANDF_LWriteEnd(K_DOS_SrcDram, 0xffffffff, &endContinuous);
		}
	}
	}

	return sts;
#endif
}
#else
// @012403@wyeo@support 256M nand, begin
UINT8 SMC_LDram2Card(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
	UINT8 sts;
	UINT32 numSector;
	UINT8 endContinuous;

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);

	G_NANDF_FindHeaderPage = 0;
	sts = NANDF_LWriteStart(K_DOS_SrcDram, logAddr, size >> 9);
	if (sts) {
		numSector = size >> 9;
		sts = NANDF_LWriteContinue(K_DOS_SrcDram, logAddr, ramAddr, &numSector);
		if (sts)
			sts = NANDF_LWriteEnd(K_DOS_SrcDram, 0xffffffff, &endContinuous);
	}

	return sts;
}
// @012403@wyeo@support 256M nand, end
#endif

// @012403@wyeo@support 256M nand, begin
UINT8 SMC_LUsb2Card(UINT32 logAddr, UINT32 size)
{
#if 0//(USE_NEW_WR == 1)
	MTD_SelfShiftR8(size);// 3.6us
	size = size >> 1;// 9.2us
	return SMC_LDram2Card(logAddr, 0, size);
#else
	UINT8 sts;
	UINT32 numSector;
	UINT8 endContinuous;
	G_NANDF_FindHeaderPage = 0;
	sts = NANDF_LWriteStart(K_DOS_SrcUsb, logAddr, size >> 9);
	if (sts) {
		numSector = size >> 9;
		sts = NANDF_LWriteContinue(K_DOS_SrcUsb, logAddr, 0, &numSector);
		if (sts)
			sts = NANDF_LWriteEnd(K_DOS_SrcUsb, 0xffffffff, &endContinuous);
	}

	return sts;
#endif
}
// @012403@wyeo@support 256M nand, end
UINT8 SMC_LCard2UsbExt(void)
{
//	UINT32 phyAddr;
//	UINT8 sts;
	UINT16 i;
	UINT8 error;
	
	//transmit 0xFF to
	for (i = 0; i < 512; i++)
		G_ucStorData[i] = 0xFF;

	HAL_DmaSetSramAddr(0x0000);
	HAL_NANDChkRdy();
	error = HAL_DmaDo(DMA_PATH_SRAM, DMA_PATH_USB, K_Card_PageSize, K_DMA_AsyncRet, K_DMA_Padding0s);

	if (error != 0) {
		return FALSE;
	}

	//FOR BULK 1 !!!
	if (G_bUSBSpeed == 0) {
		for (i = 0; i < 8; i++) {
			while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}
	} else {
		while (((XBYTE[K_USB_CLASS_IN_BUF_SIZE] + XBYTE[K_USB_CLASS_IN_BUF_SIZE + 1] << 8) < 512) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
		XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
		while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
		XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
	}

	HAL_CpuSramModeSet(0x00);  

	return TRUE;
}
UINT8 SMC_LCard2Usb_Old(UINT32 logAddr)
{
	UINT32 phyAddr;
	UINT8 sts;
	UINT16 i;

	UINT8 error;

	//printf("\n%lx\n", logAddr);

	if (SMC_ReadLpaToPpa(logAddr, &phyAddr))
		sts = SMC_Card2Usb(phyAddr);
	else {
		sts = FALSE;

		//richie@vc0422 richie@scandisk begin
		//transmit 0xFF to
		for (i = 0; i < 512; i++)
			G_ucStorData[i] = 0xFF;

		//			HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_CLASS_DMA_IN);//for class

		HAL_DmaSetSramAddr(0x0000);

		//patch4.5@ada@Add timeout count begin
		error = HAL_DmaDo(K_DMA_SRAM, K_DMA_USB, K_Card_PageSize, K_DMA_AsyncRet, K_DMA_Padding0s);

		if (error != 0) {
			return FALSE;
		}
		//patch4.5@ada@Add timeout count end

		//FOR BULK 1 !!!
		//richie@mi0415 begin
		if (G_bUSBSpeed == 0) {
			for (i = 0; i < 8; i++) {
				//patch4.5@richie@msdc plug out exception
				while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				//patch4.5@richie@msdc plug out exception
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
		} else {
			while (((XBYTE[K_USB_CLASS_IN_BUF_SIZE] + XBYTE[K_USB_CLASS_IN_BUF_SIZE + 1] << 8) < 512) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait for 0 in bulk buffer
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			//patch4.5@richie@msdc plug out exception
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait pc ack
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}
		//richie@mi0415 end

		HAL_CpuSramModeSet(0x00);  
		sts = TRUE;
		//richie@vc0422 richie@scandisk end
	}

	return sts;
}
#if 1
//-----------------------------------------------------------------------------
//SMC_LCard2Usb
//-----------------------------------------------------------------------------
UINT8 SMC_LCard2Usb(UINT32 logAddr, UINT32 size)
{
	UINT32 phyAddr, nrPage;
	UINT8 sts;
	UINT32 z0, z1, z2;
	UINT32 i, sectorPerpage, tmp;
	
	if (!size) {
		return SMC_LCard2Usb_Old(logAddr);
	}
//	printf("size: %lx\n", size);
//	printf("logAddr: %lx\n", logAddr);
	sts = size;
	sectorPerpage = G_NANDF_PageSize;
	MTD_SelfShiftR8(sectorPerpage);
	sectorPerpage = sectorPerpage >> 1;
	
	//Mask by phil @ 08.07.11 for already set before entering this function
	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	
	//first non-block alignment data
	
	nrPage = G_NANDF_BlockSize;// 9.2us
	MTD_SelfShiftR8(nrPage);// 3.6us
	nrPage = nrPage >> 1;// 9.2us
	z0 = G_NANDF_BlockSize - (logAddr % G_NANDF_BlockSize); 
//	printf("%s %u z0=%lx\n", __FILE__, (unsigned short)__LINE__,z0);
	
	if (size > z0) {
		size = size - z0;
	} else {
		z0 = size;
		size = 0;
	}

	if (z0 > 0) {
		do {
			sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
			if (sts == TRUE) {
				break;
			}
//			printf("1 logAddr %lx, phyAddr %lx\n", logAddr, phyAddr);

			/* send dummy data to usb host */ 
			sts = SMC_LCard2UsbExt();
			if (sts != TRUE) {
				return sts;
			}
			
			if (z0 >= 512UL) {
				z0 -= 512UL;
				logAddr += 512UL;							
			} else {
				z0 = 0; // usually not enter here!
//				printf("L %u, -> z0=%lx\n", (UINT16)__LINE__, z0);
			}
		} while (z0);

		//printf("%s %u phyAddr=%lx,ramAddr=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,phyAddr,ramAddr,logAddr);
		logAddr = logAddr + z0;
	
		MTD_SelfShiftR8(z0);// 3.6us
		z0 = z0 >> 1;// 9.2us
		i = z0;
	
		while (i) {
			SMC_Card2Usb(phyAddr);
			phyAddr += 512L;
			i--;
		}
	}
	
	//second is block alignment data
	

	z1 = size >> G_NANDF_BlockSizeShift;
//	printf("%s %u z1=%lx\n", __FILE__, (unsigned short)__LINE__,z1);

	tmp = nrPage / sectorPerpage;
	while (z1) {
		do {
			sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
			if (sts == TRUE) {
				break;
			}
//			printf("2 logAddr %lx, phyAddr %lx\n", logAddr, phyAddr);

			/* send dummy data to usb host */ 
			i = tmp;
#if 1
			while (i) {
				sts = SMC_LCard2UsbExt();
				if (sts != TRUE) {
					return sts;
				}
				i--;
			}	
#else
			i = i << 2;
			while (i) {
				sts = SMC_LCard2UsbExt();
				if (sts != TRUE) {
					return sts;
				}
				i--;
			}	
#endif			
			logAddr += G_NANDF_BlockSize;							

		} while (--z1);
		
		i = tmp;
#if	1		
		while (i) {
			SMC_Card2UsbFast(phyAddr);// WW5W
			phyAddr += G_NANDF_PageSize;
			i--;
		}
#else
		i = i << 2;
		while (i) {
			SMC_Card2Usb(phyAddr);
			phyAddr += 512UL;
			i--;
		}			
#endif
		z1 --;
		logAddr += G_NANDF_BlockSize;
	}
	
	//3rd is the last block alignment data
	
	z2 = size % G_NANDF_BlockSize;	
	MTD_SelfShiftR8(z2);
	z2 = z2 >> 1;
//	printf("%s %u z2=%lx\n", __FILE__, (unsigned short)__LINE__,z2);

	if (z2 > 0) {
		do {
			sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
			if (sts == TRUE) {
				break;
			}
//			printf("3 logAddr %lx, phyAddr %lx\n", logAddr, phyAddr);

			/* send dummy data to usb host */ 
			sts = SMC_LCard2UsbExt();
			if (sts != TRUE) {
				return sts;
			}
			
			logAddr += 512UL;							
		} while (--z2);

		tmp = z2 / sectorPerpage;
		z2 = z2 % sectorPerpage; 

		i = tmp;
#if 1			
		while (i) {
			SMC_Card2UsbFast(phyAddr);// WW5W
			phyAddr += G_NANDF_PageSize;
			i--;
		}
#else
		i = i << 2;
		while (i) {
			SMC_Card2Usb(phyAddr);
			phyAddr += 512UL;
			i--;
		}	
#endif
		i = z2;
		while (i) {
			SMC_Card2Usb(phyAddr);
			phyAddr += 512L;
			i--;
		}

	}

	sts = TRUE;
	return sts;
}
#endif

#if 0
//-----------------------------------------------------------------------------
//SMC_PhysicalTest
//-----------------------------------------------------------------------------
UINT8 SMC_PhysicalTest(void) 
{
		UINT8 sts = TRUE;
		UINT32 pageAddress;
		UINT16 i;

		sts = SMC_CardDetect();

		if (sts)
		{
		#if 0//(SQUEEZE_CODE_SPACE>=8)
		SETMODE_ClearAsUploadMode();
		#else
			SETMODE_ClearPostBuffer();  		//clear post buffer
			SETMODE_SetUploadMode();			//set upload mode
		#endif

				//read all pages
				for (pageAddress = 0x00000000;
					 pageAddress < ((UINT32) G_Card_TotalSizeMB << 20);
					 pageAddress += K_Card_PageSize)
				{
						SMC_Card2Sram(pageAddress);
						if ((G_ucDataBuffer[K_Card_PageSize +  6] != 0xff) ||
							(G_ucDataBuffer[K_Card_PageSize +  7] != 0xff) ||
							(G_ucDataBuffer[K_Card_PageSize + 11] != 0xff) ||
							(G_ucDataBuffer[K_Card_PageSize + 12] != 0xff))
								i++;
				}
		}

		return sts;
}
#endif


// @012403@wyeo@support 256M nand, begin
#if 1 /*(USE_NEW_WR == 0)*/
//-----------------------------------------------------------------------------
//NANDF_LWriteStart
//-----------------------------------------------------------------------------
UINT8 NANDF_LWriteStart(UINT8 srcPath, UINT32 logAddr, UINT32 numSector)
{
	UINT8 sts = FALSE;
	srcPath = 0;
	numSector = 0;
	G_NANDF_HasOldBlk = FALSE;
	G_NANDF_StartLogAddr = (logAddr >> G_NANDF_BlockSizeShift) << G_NANDF_BlockSizeShift;
	G_NANDF_EndBlkAddr = G_NANDF_StartLogAddr;
	G_NANDF_NewPhyAddr = G_NANDF_StartLogAddr;
	sts = SMC_WriteLbaToPba(G_NANDF_StartLogAddr, &G_NANDF_OldPhyStartAddr, &G_NANDF_NewPhyAddr, &G_NANDF_BlockNum);
	if (sts) {
		G_NANDF_OldPhyAddr = G_NANDF_OldPhyStartAddr;
		G_NANDF_EndBlkAddr = G_NANDF_StartLogAddr + G_NANDF_BlockSize;
		if (G_NANDF_OldPhyAddr != 0xffffffff)
			G_NANDF_HasOldBlk = TRUE;
		else
			G_NANDF_HasOldBlk = FALSE;
		if (G_NANDF_HasOldBlk) {
			// copy old pages
			//
//CPrintf("C");
#if 1//(K_SUPPORT_NEWNAND)
#if (K_NANDF_USE_COPYBACK)
			if (G_NANDF_Category) {
				UINT32 copySize;

				copySize = logAddr - G_NANDF_StartLogAddr;
				sts = SMC2_CopyAll512Pages(G_NANDF_OldPhyAddr, G_NANDF_NewPhyAddr, G_NANDF_BlockNum, copySize);
				G_NANDF_OldPhyAddr += copySize;
				G_NANDF_NewPhyAddr += copySize;
				G_NANDF_StartLogAddr += copySize;
			} else
#endif
#endif
				//
			{
#if 1//(K_SUPPORT_ONLYNEWNAND == 0 || K_NANDF_USE_COPYBACK == 0)
				while (G_NANDF_StartLogAddr < logAddr) {
					SMC_Card2Sram(G_NANDF_OldPhyAddr);
					SMC_Sram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);

					G_NANDF_OldPhyAddr += K_Card_PageSize;
					G_NANDF_NewPhyAddr += K_Card_PageSize;
					G_NANDF_StartLogAddr += K_Card_PageSize;
				}
#endif
			}
		} else {
			if (G_NANDF_StartLogAddr != logAddr) {
				SMC_Sram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
				G_NANDF_OldPhyAddr += (logAddr - G_NANDF_StartLogAddr);
				G_NANDF_NewPhyAddr += (logAddr - G_NANDF_StartLogAddr);
			}
			G_NANDF_StartLogAddr = logAddr;
		}
	}

	return sts;
}

//-----------------------------------------------------------------------------
//NANDF_LWriteContinue
//-----------------------------------------------------------------------------
UINT8 NANDF_LWriteContinue(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr, UINT32* pNumSector)
{
	UINT8 sts = TRUE;
	UINT32 numSector;
	UINT8 findHeaderPage = G_NANDF_FindHeaderPage;
	UINT16 columnAddress;
	UINT8 callStart;
	UINT8 writeSpare;
	logAddr = 0;
	numSector = *pNumSector;

	LContinue:
	callStart = TRUE;
	writeSpare = 0;
	if (srcPath == K_DOS_SrcDram)
		SDRAM_CardSetStartAddress(ramAddr, TRUE);
	while (sts && (G_NANDF_StartLogAddr < G_NANDF_EndBlkAddr)) {
		if (numSector == 0)
			break;
		if (G_NANDF_FindHeaderPage == 0) {
#if 1//(K_SUPPORT_NEWNAND)
#if (K_NANDF_USE_PAGEALIGNED)
			if (G_NANDF_Category) {
				writeSpare = 0;
				columnAddress = G_NANDF_NewPhyAddr & (G_NANDF_PageSize - 1);
				if (columnAddress == (G_NANDF_PageSize - 512)) {
					writeSpare = 2;
				} else {
					if (columnAddress == 0)
						callStart = TRUE;
				}

				if (callStart) {
					sts = SMC2_WriteStart(G_NANDF_NewPhyAddr);
					if (sts)
						callStart = FALSE;
#if (K_DEBUG_SMC)
					else	
						//printf("SMC2_WriteStart fail=%bx\n", sts);
						;
#endif
				}

				if (sts) {
					if (srcPath == K_DOS_SrcDram)
						sts = SMC2_DMAWriteFromDram(G_NANDF_NewPhyAddr, G_NANDF_BlockNum, ramAddr);
					else if (srcPath == K_DOS_SrcUsb)
						sts = SMC2_DMAWriteFromUsb(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
					else
						sts = SMC2_DMAWriteFromSram(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
#if (K_DEBUG_SMC)
					if (sts == FALSE)
						//printf("SMC2_WriteContinue fail=%bx\n", sts);
						;
#endif	
				}
				if (sts && (writeSpare == 2)) {
					writeSpare = 5;
#if 0//ndef MTD_NEW		
					sts = SMC2_WriteEnd();
#if (K_DEBUG_SMC)
					if (sts == FALSE)
						//printf("SMC2_WriteEnd fail 1=%bx\n", sts);
						;
#endif
#else
				MTD_NandSendCmd(K_SMC_PageProgram);
				MTD_NandCtrl(0x03);
				sts = TRUE;
#endif
				}
			} else
									#endif
									#endif
			{
				if (srcPath == K_DOS_SrcDram)
					sts = SMC_Dram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum, ramAddr);
				else if (srcPath == K_DOS_SrcUsb)
					sts = SMC_Usb2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
				else {
					sts = SMC_Sram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
				}
			}
			ramAddr += (K_Card_PageSize >> 1);
			numSector -= 1;
		} else {
			if (G_NANDF_SetHdrPageAddr) {
				G_File_FrameHdrPhyAddr = G_NANDF_NewPhyAddr;
				G_File_FrameHdrLogBlock = G_NANDF_BlockNum;
				if (G_NANDF_FirstEnter == TRUE) {
					G_File_HdrPhyAddr = G_NANDF_NewPhyAddr;
					G_File_HdrLogBlock = G_NANDF_BlockNum;
				}
				G_NANDF_SetHdrPageAddr = FALSE;
				//printf("G_NANDF_SetHdrPageAddr=%lx, %x, %lx, %x %lx\n", G_File_FrameHdrPhyAddr, G_File_FrameHdrLogBlock, G_File_HdrPhyAddr, G_File_HdrLogBlock, G_NANDF_StartLogAddr);
			} else {
				G_File_FrameReservePhyAddr[G_NANDF_RsvIndex] = G_NANDF_NewPhyAddr;
				G_File_FrameReserveLogBlock[G_NANDF_RsvIndex] = G_NANDF_BlockNum;
				if (G_NANDF_FirstEnter == TRUE) {
					G_File_ReservePhyAddr[G_NANDF_RsvIndex] = G_NANDF_NewPhyAddr;
					G_File_ReserveLogBlock[G_NANDF_RsvIndex] = G_NANDF_BlockNum;
				}
				G_NANDF_RsvIndex++;
				//printf("G_NANDF_SetHdrPageAddr false=%lx, %x, %lx, %x %lx\n", G_File_FrameReservePhyAddr[G_NANDF_RsvIndex], G_File_FrameReserveLogBlock[G_NANDF_RsvIndex], G_File_ReservePhyAddr[G_NANDF_RsvIndex], G_File_ReserveLogBlock[G_NANDF_RsvIndex], G_NANDF_StartLogAddr);
			}

			if (G_NANDF_FindHeaderPage == 1)
				G_NANDF_FirstEnter = FALSE; 
			G_NANDF_FindHeaderPage--;
			numSector -= 1;
		}

		G_NANDF_OldPhyAddr += K_Card_PageSize;
		G_NANDF_NewPhyAddr += K_Card_PageSize;
		G_NANDF_StartLogAddr += K_Card_PageSize;
	} // end while

#if 1//(K_SUPPORT_NEWNAND)
#if (K_NANDF_USE_PAGEALIGNED)
	if (callStart == FALSE && sts) {
		// have erver call start, should have end
		if (writeSpare != 5) {
#if 0//ndef MTD_NEW		
			sts = SMC2_WriteEnd();
#else
			MTD_NandSendCmd(K_SMC_PageProgram);
			MTD_NandCtrl(0x03);
			sts = TRUE;
#endif
		}
	}
#endif
#endif

	if (G_NANDF_HasOldBlk) {
		if (G_NANDF_StartLogAddr == G_NANDF_EndBlkAddr) {
			SMC_EraseOneBlock(G_NANDF_OldPhyStartAddr);
			G_NANDF_HasOldBlk = FALSE;
		}
	}

	if (numSector && sts) {
		sts = SMC_WriteLbaToPba(G_NANDF_StartLogAddr, &G_NANDF_OldPhyStartAddr, &G_NANDF_NewPhyAddr, &G_NANDF_BlockNum);
#if 0
	if(sts == FALSE)
		printf("NANDF_LWriteContinue:SMC_WriteLbaToPba fail G_NANDF_StartLogAddr=%lx G_NANDF_OldPhyStartAddr=%lx G_NANDF_NewPhyAddr=%lx\n", \
			G_NANDF_StartLogAddr, G_NANDF_OldPhyStartAddr,G_NANDF_NewPhyAddr);	
	#endif		
		G_NANDF_OldPhyAddr = G_NANDF_OldPhyStartAddr;
		G_NANDF_EndBlkAddr = G_NANDF_StartLogAddr + G_NANDF_BlockSize;
		if (G_NANDF_OldPhyAddr != 0xffffffff)
			G_NANDF_HasOldBlk = TRUE;
		else
			G_NANDF_HasOldBlk = FALSE;
		if (sts) {
			goto LContinue;
		}
	}

	if (findHeaderPage) {
		*pNumSector = 0;
	} else
		*pNumSector = *pNumSector - numSector;

	return sts;
}

UINT8 flashCacheCtrlSts(void) 
{
	return G_NANDF_OnCache;
}

void sp1kFlashCacheCtrl(UINT8 on) 
{
	UINT8 endContinuous;
	UINT8 sts;
	
//printf("on=%bx\n",on);
	if (on == 0) {
		if (G_NANDF_OnCache && !G_NANDF_FristCache) {
			sts = NANDF_LWriteEnd(K_DOS_SrcDram, 0xffffffff, &endContinuous);
		}
	}
	G_NANDF_StartLogAddr = 0xffffffff;
	G_NANDF_OnCache = on;
	G_NANDF_FristCache = on;
}

UINT8 NANDF_LDram2CardCache(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
	UINT8 sts;
	UINT32 numSector;
	UINT8 endContinuous;
	//HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//numSector = size >> 9;
	numSector = size;
	if (G_NANDF_FristCache) {
//printf("logAddr=%lx, ramAddr=%lx, size=%lx\n",logAddr,  ramAddr,  size);
		sts = NANDF_LWriteStart(K_DOS_SrcDram, logAddr, numSector);
		if (sts == FALSE)
			return FALSE;
		G_NANDF_FristCache = 0;
		G_NANDF_FindHeaderPage = 0;
	}
	
	if (logAddr >= G_NANDF_StartLogAddr && logAddr < G_NANDF_EndBlkAddr) {
//printf("1 logAddr=%lx, StartLogAddr=%lx, EndBlkAddr=%lx,size=%lx\n",logAddr,  G_NANDF_StartLogAddr,  G_NANDF_EndBlkAddr,  size);
		if (logAddr > G_NANDF_StartLogAddr) {
			sts = NANDF_LWriteEnd(K_DOS_SrcDram, logAddr - G_NANDF_StartLogAddr, &endContinuous);
//printf("1 endContinuous=%bx, StartLogAddr=%lx, EndBlkAddr=%lx,size=%lx\n",endContinuous,  G_NANDF_StartLogAddr,  G_NANDF_EndBlkAddr,  size);
		}
//		logAddr = G_NANDF_StartLogAddr;
//printf("3 logAddr=%lx, StartLogAddr=%lx, EndBlkAddr=%lx,size=%lx\n",logAddr,  G_NANDF_StartLogAddr,  G_NANDF_EndBlkAddr,  size);
		sts = NANDF_LWriteContinue(K_DOS_SrcDram, logAddr, ramAddr, &numSector);
	}
	else {
//printf("2 logAddr=%lx, StartLogAddr=%lx, EndBlkAddr=%lx,size=%lx\n",logAddr,  G_NANDF_StartLogAddr,  G_NANDF_EndBlkAddr,  size);
		sts = NANDF_LWriteEnd(K_DOS_SrcDram, 0xffffffff, &endContinuous);
		sts = NANDF_LWriteStart(K_DOS_SrcDram, logAddr, numSector);
		if (sts) {
			sts = NANDF_LWriteContinue(K_DOS_SrcDram, logAddr, ramAddr, &numSector);
		}
	}
	
	return sts;
}

//-----------------------------------------------------------------------------
//NANDF_LWriteEnd
//-----------------------------------------------------------------------------
UINT8 NANDF_LWriteEnd(UINT8 srcPath, UINT32 copySize, UINT8* pEndContinuous)
{
	UINT8 sts = TRUE;
	srcPath = 0;
	if (G_NANDF_StartLogAddr >= G_NANDF_EndBlkAddr) {
		if (G_NANDF_HasOldBlk) {
			SMC_EraseOneBlock(G_NANDF_OldPhyStartAddr);
			G_NANDF_HasOldBlk = FALSE;
		}

		*pEndContinuous = TRUE;
	} else
		*pEndContinuous = FALSE;	
	if (copySize && (*pEndContinuous == FALSE)) {
		UINT32 leftBlkSize;

		leftBlkSize = G_NANDF_EndBlkAddr - G_NANDF_StartLogAddr;
		if (copySize > leftBlkSize)
			copySize = leftBlkSize;

		G_NANDF_StartLogAddr += copySize;

		if (G_NANDF_HasOldBlk) {
			//
#if 1//(K_SUPPORT_NEWNAND)
#if (K_NANDF_USE_COPYBACK)
			if (G_NANDF_Category) {
				SMC2_CopyAll512Pages(G_NANDF_OldPhyAddr, G_NANDF_NewPhyAddr, G_NANDF_BlockNum, copySize);
				G_NANDF_OldPhyAddr += copySize;
				G_NANDF_NewPhyAddr += copySize;
			} else
#endif
#endif
				//
			{
#if 1//(K_SUPPORT_ONLYNEWNAND == 0 || K_NANDF_USE_COPYBACK == 0)
				while (copySize) {
					SMC_Card2Sram(G_NANDF_OldPhyAddr);
					SMC_Sram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
					G_NANDF_OldPhyAddr += K_Card_PageSize;
					G_NANDF_NewPhyAddr += K_Card_PageSize;
					copySize -= K_Card_PageSize;
				}
#endif
			}
			if (G_NANDF_StartLogAddr == G_NANDF_EndBlkAddr) {
				SMC_EraseOneBlock(G_NANDF_OldPhyStartAddr);
				G_NANDF_HasOldBlk = FALSE;
				*pEndContinuous = TRUE;
			} else
				*pEndContinuous = FALSE;
		} else {
			if (G_NANDF_StartLogAddr == G_NANDF_EndBlkAddr) {
				// write any data to the bottom of block even if
				// write area don't include this end page. This
				// is useful for error handling.
				if ((G_NANDF_NewPhyAddr & (G_NANDF_BlockSize - 1)) != 0) {
					G_NANDF_NewPhyAddr = (G_NANDF_NewPhyAddr >> G_NANDF_BlockSizeShift) << G_NANDF_BlockSizeShift;
					G_NANDF_NewPhyAddr += (G_NANDF_BlockSize - K_Card_PageSize);
					G_NANDF_OldPhyAddr += (G_NANDF_BlockSize - K_Card_PageSize);
					SMC_Sram2Card(G_NANDF_NewPhyAddr, G_NANDF_BlockNum);
				}
				*pEndContinuous = TRUE;
			} else {
				G_NANDF_OldPhyAddr += copySize;
				G_NANDF_NewPhyAddr += copySize;
				*pEndContinuous = FALSE;
			}
		}
	}

	return TRUE;
}
// @012403@wyeo@support 256M nand, end
#endif

//-----------------------------------------------------------------------------
//SMC_ReadLpaToPpa
//-----------------------------------------------------------------------------
UINT8 SMC_ReadLpaToPpa(UINT32 logAddr, UINT32* phyAddr)
{
	UINT8 sts;
	UINT16 logBlockAddr, phyBlockAddr;
	UINT8 bank = 0;
	UINT16 bank0TotalBlk;	// @patch 5.1@012403@wyeo@fix cross nand bank bug

	logBlockAddr = logAddr >> G_NANDF_BlockSizeShift; // @012403@wyeo@support 256M nand
	//printf("R logBlockAddr=%x,logAddr=%lx\n",logBlockAddr,logAddr);

	// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
	bank0TotalBlk = K_SMC_LogicalBlockPerBank;
#if (NANDF_OPTION || K_SMC_AS_NANDF)
#if (K_SMC_AS_NANDF == 0)
	if (G_Card_Type == K_MEDIA_NANDF)
			#endif
			#endif
		bank0TotalBlk = G_NANDF_Bank0TotalBlk;
	//printf("%s(%d),b0=%x\n",__FILE__,(UINT16)(UINT16)__LINE__, bank0TotalBlk);				

	if (logBlockAddr >= bank0TotalBlk) {
		bank = 1;
		logBlockAddr -= bank0TotalBlk;
		while (logBlockAddr >= K_SMC_LogicalBlockPerBank) {
			logBlockAddr -= K_SMC_LogicalBlockPerBank;
			bank++;
		}
	}
	if (bank > G_Card_TotalBank) {
		(*phyAddr) = 0xffffffff;
		//printf("1 phyAddr=%lx\n",*phyAddr);
		return FALSE;
	}
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, end

	if (bank != G_Card_CurrBank) {
#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		// move current updated copy at sram to dram
		SMC_UpdateBankTable(G_Card_CurrBank, FALSE);
#endif

		if (bank < G_Card_TotalBank) {
			G_Card_CurrBank = bank;
			SMC_MakeTable(bank);
		}
		//@012403@wyeo@prevent illegal logical address, begin
		else {
			(*phyAddr) = 0xffffffff;
			//printf("2 phyAddr=%lx\n",*phyAddr);
			return FALSE;
		}
		//@012403@wyeo@prevent illegal logical address, end
	}

//dumpData(32, &G_SMC_AllocatTable[0] ,0);
	if ((phyBlockAddr = G_SMC_AllocatTable[logBlockAddr]) != 0xffff) {
		// @012403@wyeo@support 256M nand, begin
		(*phyAddr) = ((UINT32) (G_Card_CurrBank * G_Card_PhysicalBankSize)) + ((UINT32) (phyBlockAddr) << G_NANDF_BlockSizeShift) + ((UINT32) (logAddr & (G_NANDF_BlockSize - 1)));
		// @012403@wyeo@support 256M nand, end

		sts = TRUE;
	} else {
		(*phyAddr) = 0xffffffff;

		sts = FALSE;
	}

	//printf("phyAddr=%lx\n",*phyAddr/G_NANDF_BlockSize);
	return sts;
}

//-----------------------------------------------------------------------------
//SMC_WriteLbaToPba
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//SMC_WriteLbaToPba
//-----------------------------------------------------------------------------
UINT8 SMC_WriteLbaToPba(UINT32 logAddr, UINT32* oldPhyAddr, UINT32* newPhyAddr, UINT16* blockNum)
{
	UINT8 sts = TRUE;
	UINT16 logBlockAddr, newPhyBlockAddr, oldPhyBlockAddr;
	UINT8 bank = 0;
	UINT16 bank0TotalBlk;	// @patch 5.1@012403@wyeo@fix cross nand bank bug

	logBlockAddr = logAddr >> G_NANDF_BlockSizeShift; // @012403@wyeo@support 256M nand
	//printf("0 logBlockAddr=%x,logAddr=%lx\n",logBlockAddr,logAddr);

	// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
	bank0TotalBlk = K_SMC_LogicalBlockPerBank;
#if (NANDF_OPTION || K_SMC_AS_NANDF)
#if (K_SMC_AS_NANDF == 0)
	if (G_Card_Type == K_MEDIA_NANDF)
			#endif
			#endif
		bank0TotalBlk = G_NANDF_Bank0TotalBlk;

	if (logBlockAddr >= bank0TotalBlk) {
		bank = 1;
		logBlockAddr -= bank0TotalBlk;
		while (logBlockAddr >= K_SMC_LogicalBlockPerBank) {
			logBlockAddr -= K_SMC_LogicalBlockPerBank;
			bank++;
		}
	}
	if (bank > G_Card_TotalBank) {
		(*oldPhyAddr) = 0xffffffff;
		(*newPhyAddr) = 0xffffffff;
		(*blockNum) = 0xffff;
		//printf("1 newPhyAddr=%lx\n",*newPhyAddr/G_NANDF_BlockSize);
		return FALSE;
	}	
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, end

	if (bank != G_Card_CurrBank) {
#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		// move current updated copy at sram to dram
		SMC_UpdateBankTable(G_Card_CurrBank, FALSE);
#endif

		if (bank < G_Card_TotalBank) {
			G_Card_CurrBank = bank;
			//printf("bank=%bx\n",bank);
			SMC_MakeTable(bank);
		}
		//@012403@wyeo@prevent illegal logical address, begin
		else {
			(*oldPhyAddr) = 0xffffffff;
			(*newPhyAddr) = 0xffffffff;
			(*blockNum) = 0xffff;
			//printf("2 newPhyAddr=%lx\n",*newPhyAddr/G_NANDF_BlockSize);
			return FALSE;
		}
		//@012403@wyeo@prevent illegal logical address, end
	}

	if ((oldPhyBlockAddr = G_SMC_AllocatTable[logBlockAddr]) == 0xffff) {
		(*oldPhyAddr) = 0xffffffff;
		//printf("4 oldPhyAddr=0x%lx\n",*oldPhyAddr/G_NANDF_BlockSize);
	} else {
		// @012403@wyeo@support 256M nand, begin
		//printf("5 oldPhyBlockAddr=0x%x\n",oldPhyBlockAddr);
		(*oldPhyAddr) = ((UINT32) (G_Card_CurrBank * G_Card_PhysicalBankSize)) + ((UINT32) (oldPhyBlockAddr) << G_NANDF_BlockSizeShift);
		//printf("5 oldPhyAddr=0x%lx\n",*oldPhyAddr/G_NANDF_BlockSize);
		// @012403@wyeo@support 256M nand, end

		// @012403@wyeo@remove K_PreWriteOldBlock
	}

	if ((newPhyBlockAddr = SMC_SearchNewBlock()) == 0xffff) {
		//printf("3 newPhyBlockAddr=0x%x\n",newPhyBlockAddr);
		//printf("3 logBlockAddr=0x%lx\n",logBlockAddr/G_NANDF_BlockSize);
		//printf("3 newPhyAddr=0x%lx\n",*newPhyAddr/G_NANDF_BlockSize);
		//dumpData( 512 , G_SMC_EntryTable, 0);
		//dumpData( 2048 , G_SMC_AllocatTable, 0);
		(*oldPhyAddr) = 0xffffffff; //@012403@wyeo@prevent illegal logical address
		(*newPhyAddr) = 0xffffffff;
		(*blockNum) = 0xffff;
		//printf("3 newPhyAddr=0x%lx\n",*newPhyAddr/G_NANDF_BlockSize);
		sts = FALSE;	//used physical block full
	} else {
		// @012403@wyeo@support 256M nand, begin
		(*newPhyAddr) = ((UINT32) (G_Card_CurrBank * G_Card_PhysicalBankSize)) + ((UINT32) (newPhyBlockAddr) << G_NANDF_BlockSizeShift);
		// @012403@wyeo@support 256M nand, end

		G_SMC_AllocatTable[logBlockAddr] = newPhyBlockAddr;
		//printf("6 newPhyBlockAddr=0x%x\n",newPhyBlockAddr);
		//dumpData( 16 , &G_SMC_AllocatTable[logBlockAddr], 0);
		G_SMC_EntryTable[newPhyBlockAddr >> 3] &= ~(0x01 << (newPhyBlockAddr % 8)); 	  //mark un-usable
		//dumpData( 16 , &G_SMC_EntryTable[newPhyBlockAddr >> 3], 0);
		(*blockNum) = SMC_CalcLbaField(logBlockAddr);
		//printf("6 *blockNum=0x%x\n",*blockNum);
		sts = TRUE;
	}

	if (oldPhyBlockAddr != 0xffff) {
		G_SMC_EntryTable[oldPhyBlockAddr >> 3] |= (0x01 << (oldPhyBlockAddr % 8));  	  //mark re-usable
	}

//dumpData(32, &G_SMC_AllocatTable[0] ,0);
	//printf("newPhyAddr=0x%lx,0x%lx, %lx\n",*newPhyAddr,*newPhyAddr/G_NANDF_BlockSize,(*oldPhyAddr));
	return sts;
}

//-------------------------------------
//SMC_CalcLbaField
//-------------------------------------
UINT16 SMC_CalcLbaField(UINT16 logBlockAddr)
{
	UINT16 adrs = logBlockAddr; 			   

	adrs = (adrs << 1) | 0x1000;
//	adrs |= SMC_ParityCheck((UINT8) (adrs >> 8), (UINT8) (adrs & 0xff));
	adrs |= SMC_ParityCheck(READ8(adrs, 0), READ8(adrs, 1));

	return adrs;
}

//-----------------------------------------------------------------------------
//SMC_SearchNewBlock
//-----------------------------------------------------------------------------
UINT16 SMC_SearchNewBlock(void)
{
	UINT16 phyBlockAddr = 0xffff;
	UINT8 i, j, bt;

	for (i = 0; i < G_SMC_EntryTableSize; i++) {
		if (G_SMC_EntryTable[i]) {
			bt = 0x01;
			j = 0;
			while (!(G_SMC_EntryTable[i] & bt)) {
				bt <<= 1;
				j++;
			}
			G_SMC_EntryTable[i] &= ~bt;
			phyBlockAddr = i * 8 + j;
			break;
		}
	}

	return phyBlockAddr;
}

// @patch 4.5@wyeo@make table make faster, begin
//-----------------------------------------------------------------------------
//SMC_FormatCheck
//-----------------------------------------------------------------------------
UINT8 SMC_FormatCheck(void)
{
	UINT16 maxCisBlock;
	UINT32 phyAddr;
	UINT32 pageAddr;
	UINT16 k;
	UINT8 first = TRUE;

	//G_SMC_CisBlockAddress = 0;
	G_SMC_CisBlockFound = FALSE;
	G_SMC_FirstCISFound = FALSE;

#if 0//(SQUEEZE_CODE_SPACE>=8)
	SETMODE_ClearAsUploadMode();
#else
	SETMODE_ClearPostBuffer();  		//clear post buffer
	SETMODE_SetUploadMode();			//set upload mode
#endif

	maxCisBlock = 11/* + G_NAND_ReservBlockMax*/;  //maximal reserver block is 20
	phyAddr = G_SMC_CisBlockAddress + G_NANDF_BlockSize;
	k = 0;
	//printf("phyAddr=%lx\n",phyAddr);

	//L_FIND_CIS:     
	while (k < maxCisBlock) {
		if (SMC_SearchValidBlock(&phyAddr)) {
			//printf("%s,%d,phyAddr=%lx,G_SMC_CisBlockAddress=%lx\n",__FILE__,(UINT16)__LINE__, phyAddr,G_SMC_CisBlockAddress);				
			//M_DEBUG_SHOWLED(0xfc);
			// @012403@wyeo@support 256M nand, begin
			for (pageAddr = phyAddr; pageAddr < (phyAddr + G_NANDF_BlockSize); pageAddr += K_Card_PageSize)
				// @012403@wyeo@support 256M nand, end
			{
				SMC_Card2Sram(pageAddr);
				if (SMC_DataStsCheck(G_ucDataBuffer[K_Card_PageSize + 4])) //skeptic
				{
					if ((G_ucDataBuffer[K_Card_PageSize + 6] == 0x00) &&   //logical block address 0x0000
						(G_ucDataBuffer[K_Card_PageSize + 7] == 0x00))   //skeptic
					{
						if (SMC_CisCheck()) {
							//M_DEBUG_SHOWLED(0xf8);
							G_SMC_FirstCISFound = TRUE;
							G_SMC_CisBlockFound = TRUE;
							G_SMC_CisBlockAddress = phyAddr;
						}
					}
					break;
				}
			} // end for
			break;
		} else {
			phyAddr += G_NANDF_BlockSize;
			k++;
		}
	} // end while


	if (G_Card_Type == K_MEDIA_NANDF) {
		//G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank;
		G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal - NAND_BANK0_SWAP_BLK;
		//		G_NANDF_LastBankTotalBlk =  K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal;
	}

	return G_SMC_CisBlockFound;
}
// end

// @patch 4.5@wyeo@make table make faster, begin

#if (K_NANDF_STRICT_BAD_CHECK)
// according Samsung spec:
// All device locations are erased(FFh) except locations where the invalid
// block(s) information is written prior to shipping. The invalid block(s)
// status is defined by the 6th byte in the spare area. Samsung makes sure
// that either the 1st or 2nd page of every invalid block has non-FFh data
// at the column address of 517. Since the invalid block information is also
// erasable in most cases, it is impossible to recover the information once
// it has been erased. Threfore, the system must be able to recognized the
// invalid block(s) based on the original invalid block information and
// create the invalid block table via the following suggested flow chart.
UINT8 SMC_BlockStsCheck(UINT32 phyAddress, UINT8 stsData, UINT8 stsData2)
{
#if ( SMC_OPTION ) 
	data UINT8 i, mask, sum;
#endif
	data UINT8 sts = FALSE;
	//phyAddress = phyAddress;

	/* mask by phil @ 0815 for G_Card_Type might be not initialized by host */
	//if (G_Card_Type == K_MEDIA_NANDF) {
	if ((stsData != 0xff) || (stsData2 != 0xff))
		// first page or second of redudant not ff, so block is bad
		sts = FALSE;
	else
		sts = TRUE;

	if (!sts) {
		printf("bad=%lu\n", phyAddress / G_NANDF_BlockSize);
	}	
	return sts;
	//}

#if ( SMC_OPTION ) 
	if (stsData != 0xff) {
		sum = 0;
		mask = 0x01;
		for (i = 0; i < 8; i++) {
			if (stsData & mask)
				sum++;
			mask <<= 1;
		}
		if (sum > 6)
			sts = TRUE;
		else
			sts = FALSE;
	} else
		sts = TRUE;

	return sts;

#else
	return sts;
#endif
}

#else

//-------------------------------------
//SMC_BlockStsCheck
//-------------------------------------
// K_Check_Nand_Bad_Block_Infomation
UINT8 SMC_BlockStsCheck(UINT32 phyAddress, UINT8 stsData, UINT8 stsData2)
{
	data UINT8 i, mask, sum, sts;

#if ( NANDF_OPTION )
	// please use physical block 0 bad block information
	// to set StsData according to phyAddress
	// phyAddress is always blocksize multiple  
	//StsData = 0xff;
#endif

	if (stsData != 0xff) {
		sum = 0;
		mask = 0x01;
		for (i = 0; i < 8; i++) {
			if (stsData & mask)
				sum++;
			mask <<= 1;
		}
		if (sum > 6)
			sts = TRUE;
		else
			sts = FALSE;
	} else
		sts = TRUE;

	return sts;
	phyAddress = phyAddress;
	stsData2 = stsData2;
}
#endif
// @patch 4.5@wyeo@make table make faster, end

//-------------------------------------
//SMC_DataStsCheck
//-------------------------------------
UINT8 SMC_DataStsCheck(UINT8 stsData)
{
	UINT8 i, mask, sum, sts;

	if (stsData != 0xff) {
		sum = 0;
		mask = 0x01;
		for (i = 0; i < 8; i++) {
			if (stsData & mask)
				sum++;
			mask <<= 1;
		}
		if (sum > 4)
			sts = TRUE;
		else
			sts = FALSE;
	} else
		sts = TRUE;

	return sts;
}

//-------------------------------------
//SMC_CisCheck
//-------------------------------------
UINT8 SMC_CisCheck(void)
{
	UINT8 i, sts;

	sts = TRUE;

	for (i = 0; i < K_SMC_CisPartialSize; i++) {
		if (G_SMC_CisPartial[i] != G_ucDataBuffer[i])
			sts = FALSE;
	}

	return sts;
}

// @patch 4.5@wyeo@make table make faster, begin
#if 1
void SMC_MakeTablePrintf(UINT16 blkIndex)
{
	printf("bad=0x%x\n", blkIndex);
}

//-------------------------------------
//SMC_MakeTable
//-------------------------------------
void SMC_MakeTable(UINT8 bank)
{
	UINT32 bankStart;
	UINT16 wTemp;

#if 0
	   data UINT8 bt;
	   data UINT16 tmp;
	   data UINT8 d3, d4;
data UINT8 stsData2 = 0xFF;
data UINT8 stsData1;
data UINT32 pageAddress;
data UINT8 d1, d2;	
data BIT doOldBlk;
data BIT flag = 0;
data BIT useRegData;
data BIT sts;
data UINT16 blkIndex;
data UINT32 blkSize; // @012403@wyeo@support 256M nand
data UINT8 cardType;
data UINT16 blkMax;
#endif

#if 1
	UINT8 bt;
	UINT16 tmp;
	UINT8 d3, d4;
	UINT8 stsData2 = 0xFF;
	UINT8 stsData1;
	UINT32 pageAddress;
	UINT8 d1, d2;
	// @patch5.2.1-24@wyeo@fix initiliaze nand become mess@110603, begin	
	UINT8 doOldBlk;// BIT
	UINT8 flag = 0;// BIT
	UINT8 useRegData;// BIT
	UINT8 sts;// BIT
	// @patch5.2.1-24@wyeo@fix initiliaze nand become mess@110603, end
	UINT16 blkIndex;
	UINT32 blkSize; // @012403@wyeo@support 256M nand
	UINT8 cardType;
	UINT16 blkMax;
	UINT16 resBlk;
#endif
printf("bank: %lx\n", (UINT32)bank);
	if (bank == 0/*(G_Card_TotalBank - 1)*/) {//Nand Hdr change to right after header
		resBlk = G_NAND_ReservBlockTotal;
		//printf("resBlk=%bx\n", G_NAND_ReservBlockTotal);
	} else {
		resBlk = 0;
	}
	// @012403@wyeo@support 256M nand, begin
	blkMax = (UINT16) (G_Card_PhysicalBankSize / G_NANDF_BlockSize);
	blkSize = G_NANDF_BlockSize;
	// @012403@wyeo@support 256M nand, end

	//printf(",blkMax=%x,blkSize=%lx,bank=%bx\n", blkMax, blkSize, bank);				
	//dbgTimerStart();
	//XBYTE[REG_Cpu_P1oeSel] |= 0x01 << 6;
	cardType = G_Card_Type;

#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	// check if this bank table already build in sdram
	// if yes then just copy it from sdram
	d1 = bank >> 3;
	bt = 0x01 << (bank % 8);
	if (G_SMC_BuildBankMap[d1] & bt) {
		SMC_UpdateBankTable(bank, TRUE);
		return;
	} 
#endif

	//initialize tables
	for (tmp = 0; tmp < G_SMC_AllocatTableSize; tmp++)
		G_SMC_AllocatTable[tmp] = 0xffff;
	for (d1 = 0; d1 < G_SMC_EntryTableSize; d1++)
		G_SMC_EntryTable[d1] = 0x00;

	if (bank == 0) {
		pageAddress = G_SMC_CisBlockAddress + blkSize;
		blkIndex = pageAddress >> G_NANDF_BlockSizeShift; // @012403@wyeo@support 256M nand
		bt = 0x01 << (blkIndex % 8);
	} else {
		pageAddress = 0x00000000;
		bt = 0x01;
		blkIndex = 0;
	}
	bankStart = bank * G_Card_PhysicalBankSize;
	pageAddress = pageAddress + bankStart;
	
	while (blkIndex < blkMax) {
	//printf("blk=%x\n",blkIndex);
	//printf("pageAddress: %lx\n", (UINT32)pageAddress);
		if (G_NANDF_Category) {
			SMC2_RedunCard2Sram(pageAddress);
		} else {
			SMC_RedunCard2Sram(pageAddress);
		}
//dumpData(16, &G_ucDataBuffer[K_Card_PageSize] ,0);
		// @012403@wyeo@support 256M nand, begin
#if 1//(K_SUPPORT_NEWNAND)
		if (G_NANDF_Category)
			stsData1 = G_ucDataBuffer[K_Card_PageSize];
		else
#endif
			stsData1 = G_ucDataBuffer[K_Card_PageSize + 5];
		// @012403@wyeo@support 256M nand, end

#if (/*CODE_IS_MULTIBANK &&*/ NANDF_OPTION) 		
		if (cardType == K_MEDIA_NANDF) {
			if (stsData1 != 0xff) {
				SMC_MakeTablePrintf(blkIndex);
				blkIndex += 1;
				pageAddress += blkSize;
				if ((bt <<= 1) == 0)
					bt = 0x01;
				continue;
			} else
				sts = 1;
		}
#endif

		d1 = G_ucDataBuffer[K_Card_PageSize + 6];
		d2 = G_ucDataBuffer[K_Card_PageSize + 7];

#if (K_STRICT_MAKE_TABLE)
		flag = 0;
		d3 = G_ucDataBuffer[K_Card_PageSize + 11];
		d4 = G_ucDataBuffer[K_Card_PageSize + 12];

		// @012403@wyeo@support 256M nand, begin
		if (G_NANDF_Category) {
			SMC2_RedunCard2Sram(pageAddress + G_NANDF_PageSize); 
		} else {
			SMC_RedunCard2Sram(pageAddress + G_NANDF_PageSize);
		}
		
#if 1//(K_SUPPORT_NEWNAND)
		if (G_NANDF_Category)
			stsData2 = G_ucDataBuffer[K_Card_PageSize];
		else
#endif
			stsData2 = G_ucDataBuffer[K_Card_PageSize + 5];
		// @012403@wyeo@support 256M nand, end

#if (/*CODE_IS_MULTIBANK &&*/ NANDF_OPTION) 
		if (cardType == K_MEDIA_NANDF) {
			//if (stsData1 != 0xff)  // 20040304 wyeo mark
			if (stsData2 != 0xff)  // 20040304 wyeo should be stsData2 instead of stsData1
			{
				SMC_MakeTablePrintf(blkIndex);
				blkIndex += 1;
				pageAddress += blkSize;
				if ((bt <<= 1) == 0)
					bt = 0x01;
				continue;
			} else
				sts = 1;
		} else
#endif
#endif
			sts = SMC_BlockStsCheck(pageAddress, stsData1, stsData2);

		if (sts) //skeptic
		{
			doOldBlk = 0;
			if (d1 != 0xff || d2 != 0xff) {
#if (K_STRICT_MAKE_TABLE)
				flag = 1;
#endif
				doOldBlk = 1;
				//M_Card_Printf("first page of block is not ffff\n");
			} else {
#if (K_STRICT_MAKE_TABLE)
				d1 = G_ucDataBuffer[K_Card_PageSize + 6];
				d2 = G_ucDataBuffer[K_Card_PageSize + 7];
				if (d1 != 0xff || d2 != 0xff) {
					doOldBlk = 1;
					//M_Card_Printf("second page of block is not ffff\n");
				} else 
#endif  	  
				{
#if (K_RECOVER_MAKE_TABLE)
					if (G_NANDF_Category) {
						SMC2_RedunCard2Sram(pageAddress + (blkSize - K_Card_PageSize));
					} else {
						SMC_RedunCard2Sram(pageAddress + (blkSize - K_Card_PageSize));
					}
					d1 = G_ucDataBuffer[K_Card_PageSize + 6];
					d2 = G_ucDataBuffer[K_Card_PageSize + 7];
					if (d1 != 0xff || d2 != 0xff) {
						doOldBlk = 1;
						//M_Card_Printf("last page of block is not ffff\n");
					}
#endif
				}
			}

			if (doOldBlk) {
				sts = 1;

				if (SMC_ParityCheck(d1, d2) != 0x00) //even parity
				{
					//printf("parity error 1 d1 %bx\n", d1);
					//printf("parity error 1 d2 %bx\n", d2);
					//end
#if (K_STRICT_MAKE_TABLE)
					if (flag) {
						d1 = d3;
						d2 = d4;
					} else
#endif
					{
						d1 = G_ucDataBuffer[K_Card_PageSize + 11];
						d2 = G_ucDataBuffer[K_Card_PageSize + 12];
					}

					if (SMC_ParityCheck(d1, d2) != 0x00) //even parity
					{
						//printf("parity error 2 d1 %bx\n", d1);
						//printf("parity error 2 d2 %bx\n", d2);
						//end

						sts = 0;
					} else {
#if (K_Error_Recovery)
						if ((d1 != 0xff) || (d2 != 0xff)) {
							// ????
							tmp = SMC_CalcLba(d1, d2);
							//M_Card_Printf("block number not same %x\n", tmp);
							SMC_RedunSram2Card(pageAddress, SMC_CalcLbaField(tmp));
						}
#else
						sts = 0;
#endif
					}
					 
					if (((d1 == 0xff) && (d2 == 0xff)) || (sts == 0)) {
						SMC_EraseOneBlock(pageAddress);
						G_SMC_EntryTable[blkIndex >> 3] |= bt; 
						sts = 0;
						//printf("erase problem block %x\n", blkIndex);
					}
				}

				if (sts) {
					tmp = SMC_CalcLba(d1, d2);

					if (G_SMC_AllocatTable[tmp] != 0xffff) {
						//printf("block replace");
#if (K_Error_Recovery)
						G_DOS_CardErrDetected = 1;
#endif

						// get current block last page block number
						SMC_RedunCard2Sram(pageAddress + (blkSize - K_Card_PageSize));

						d1 = G_ucDataBuffer[K_Card_PageSize + 6];
						d2 = G_ucDataBuffer[K_Card_PageSize + 7];
						useRegData = 1;
						if ((d1 != 0xff) || (d2 != 0xff)) {
							if (SMC_ParityCheck(d1, d2) != 0x00) //even parity
							{
								d1 = G_ucDataBuffer[K_Card_PageSize + 11];// @patch5.2.1-24@wyeo@fix initiliaze nand become mess@110603
								d2 = G_ucDataBuffer[K_Card_PageSize + 12];
								if (SMC_ParityCheck(d1, d2) == 0x00) //even parity
									useRegData = 0;
							} else
								useRegData = 0;
						}

						/*
						M_Card_Printf("same block number d1 %bx\n", d1);
						M_Card_Printf("same block number d2 %bx\n", d2);
						M_Card_Printf("same block number d3 %bx\n", d3);
						M_Card_Printf("same block number d4 %bx\n", d4);
						M_Card_Printf("same block number tmp %x\n", tmp);
						M_Card_Printf("same block number G_SMC_AllocatTable[tmp] %x\n", G_SMC_AllocatTable[tmp]);
						M_Card_Printf("same block number wTemp %x\n", wTemp);
						*/
						//end

						if (useRegData) {
							// already register block is valid data, so erase current block
							// erase block with overlapped address
							SMC_EraseOneBlock(pageAddress);
							// update entry table
							G_SMC_EntryTable[blkIndex >> 3] |= bt;
						} else {
							// current block data is valid, so erase register block
							wTemp = G_SMC_AllocatTable[tmp];
							// @012403@wyeo@support 256M nand, begin
							SMC_EraseOneBlock(bankStart + ((UINT32) wTemp << G_NANDF_BlockSizeShift));
							// @012403@wyeo@support 256M nand, end
							G_SMC_EntryTable[wTemp >> 3] |= (0x01 << (wTemp % 8)); 
							G_SMC_AllocatTable[tmp] = blkIndex;
						}
					} else
						G_SMC_AllocatTable[tmp] = blkIndex;
					// end
				}
			} else
				//block is new erased block
				G_SMC_EntryTable[blkIndex >> 3] |= bt;
		} else {
			printf("blkIndex=%ld\n", blkIndex);
		}

		if ((bt <<= 1) == 0)
			bt = 0x01;

		/*
						  #if 1//(SQUEEZE_CODE_SPACE<=2)
						  if (G_DOS_Stop_Initial)
						  	break;
						  #endif
						  */

		blkIndex += 1;
		pageAddress += blkSize;
	}

	//For "A" Area access
	SMC_Card2SramForExt(bankStart);
	
	//dbgTimerStop();	
}
#endif

// @patch 4.5@wyeo@make table make faster, begin

//-------------------------------------
//SMC_ParityCheck
//-------------------------------------
UINT8 SMC_ParityCheck(UINT8 d1, UINT8 d2)
{
	data UINT8 p;

	p = d1 ^ d2;					//byte
	p = (p >> 4) ^ (p & 0x0f);  	//nibble
	p = (p >> 2) ^ (p & 0x03);  	//2-bit
	p = (p >> 1) ^ (p & 0x01);  	//1-bit

	return p;
}

//-------------------------------------
//SMC_CalcLba
//-------------------------------------
UINT16 SMC_CalcLba(UINT8 d1, UINT8 d2)
{
	data UINT16 adrs;

	adrs = d2;
	adrs |= ((d1 & 0x07) << 8);
	adrs >>= 1;

	return adrs;
}
// @patch 4.5@wyeo@make table make faster, end

// @patch 4.5@wyeo@make table make faster, begin
UINT8 SMC_EraseTotalBlock(void)
{
	UINT32 k;
	//  	  UINT16 j;
	UINT8 sts;
	UINT32 phyAddr;
	UINT32 phyMaxAddr;
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
	UINT32 bankMaxAddr;
	UINT16 bankGoodBlkCount;
	UINT32 bankBit;
	UINT8 sts1;
	UINT8 bankidx;
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, end

#if 0//(SQUEEZE_CODE_SPACE>=8)
	SETMODE_ClearAsUploadMode();
#else
	SETMODE_ClearPostBuffer();  		//clear post buffer
	SETMODE_SetUploadMode();			//set upload mode
#endif

	G_DOS_ErrCode = 0;	// @patch 5.1@012403@wyeo@fix cross nand bank bug
	G_NANDF_BadBank = 0;	// @patch 5.1@012403@wyeo@fix cross nand bank bug

	// find requested number of good block for reserve
	phyAddr = 0;
	sts = TRUE;
	phyMaxAddr = (UINT32) G_Card_TotalSizeMB << 20;
	//printf("%s(%d),phyMaxAddr=%lx\n",__FILE__,(UINT16)__LINE__, phyMaxAddr);					
	phyMaxAddr = phyMaxAddr - G_NANDF_BlockSize;
	//printf("%s(%d),phyMaxAddr=%lx\n", __FILE__, (UINT16) __LINE__, phyMaxAddr);					
	G_SMC_GoodSectorPerCard = 0;

	// @patch 4.5@wyeo@simulate smc as nand, begin
#if (NANDF_OPTION || K_SMC_AS_NANDF)
#if (K_SMC_AS_NANDF == 0)
	if (G_Card_Type == K_MEDIA_NANDF)
		#endif
		//end
	{
		phyAddr = G_SMC_CisBlockAddress;
		//printf("%s,%d,phyAddr=%lx,G_SMC_CisBlockAddress=%lx\n",__FILE__,(UINT16)__LINE__, phyAddr,G_SMC_CisBlockAddress);				
		SMC_SearchValidBlock(&phyAddr);
		//printf("%s,%d,phyAddr=%lx,G_SMC_CisBlockAddress=%lx\n",__FILE__,(UINT16)__LINE__, phyAddr,G_SMC_CisBlockAddress);				
		if (phyAddr >= phyMaxAddr) {
			sts = FALSE;
		}
	}
#endif

	if (sts) {
		G_SMC_NewCisBlockAddress = phyAddr;
		SMC_SearchValidBlock(&G_SMC_NewCisBlockAddress);
		phyAddr = G_SMC_NewCisBlockAddress;
		G_SMC_GoodSectorPerCard += G_Card_PagePerBlock;

		SMC_FormatCheck();

		if (G_SMC_CisBlockAddress < phyAddr) {
			if (G_SMC_CisBlockAddress == 0) {
#if (SMC_OPTION)
				if (G_Card_Type == K_MEDIA_SMC)
					phyAddr = 0;
#endif
			} else
				phyAddr = G_SMC_CisBlockAddress;
		}

		// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
#if (NANDF_OPTION || K_SMC_AS_NANDF)
#if (K_SMC_AS_NANDF == 0)
		if (G_Card_Type == K_MEDIA_NANDF)
						#endif
						#endif
			//G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank;
		G_NANDF_Bank0TotalBlk = K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal - NAND_BANK0_SWAP_BLK;
		//			G_NANDF_LastBankTotalBlk =  K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal;
		// @patch 5.1@012403@wyeo@fix cross nand bank bug, end

		// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
		bankMaxAddr = 1024L * (UINT32) G_NANDF_BlockSize;
		bankBit = 0x01;
		bankGoodBlkCount = G_SMC_GoodSectorPerCard / G_Card_PagePerBlock;
		// @patch 5.1@012403@wyeo@fix cross nand bank bug, end
		bankidx = 1;
		for (k = 0; sts && (phyAddr < phyMaxAddr);) {
			if (SMC_SearchValidBlock(&phyAddr)) {
				sts1 = SMC_EraseOneBlock(phyAddr);// @012403@wyeo@support 256M nand
				// calculate the good sector per card
				if (sts) {
					if (phyAddr > G_SMC_NewCisBlockAddress) {
						bankGoodBlkCount++; // @patch 5.1@012403@wyeo@fix cross nand bank bug
						G_SMC_GoodSectorPerCard += G_Card_PagePerBlock;
						k += G_Card_PagePerBlock;
					}
				}
				phyAddr += G_NANDF_BlockSize; // @012403@wyeo@support 256M nand
			}
			// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
			if (((phyAddr % bankMaxAddr) == 0) || (phyAddr >= phyMaxAddr) || (sts == FALSE)) {
				// each bank must have at least 1001 block
				if (bankGoodBlkCount < 1001 && ( bankidx != 1 )) {//
					G_DOS_ErrCode = 1001;
					printf("1001=%x\n", bankGoodBlkCount);
				} else
					G_NANDF_BadBank |= bankBit;
				//printf("cross bank=%x, %lx\n", bankGoodBlkCount, G_NANDF_BadBank);

				bankGoodBlkCount = 0;
				bankBit <<= 1;
				bankidx ++;
			}
			// @patch 5.1@012403@wyeo@fix cross nand bank bug, end					
		}

		// If good sector per card smaller than assume 
		// sector per card, than use this count.
		// This may happen when there are many bad block
		// or there are many block reserve for nand flash.
		if (k < G_Card_SectorPerCard) {
			// @patch5.2.1-24@wyeo@fix initiliaze nand
			G_Card_SectorPerCard = k - G_Card_PagePerBlock;
		}
		/* @012403@wyeo@prevent format sudden plug problem
					if (G_Card_SectorPerCard > G_Card_PagePerBlock)
						G_Card_SectorPerCard -= G_Card_PagePerBlock;
				else
						sts = FALSE;
				*/
		M_Card_Printf("G_Card_SectorPerCard %lx\n", G_Card_SectorPerCard);
		M_Card_Printf("G_Card_SectorPerCard(k) %lx\n", k);
		M_Card_Printf("G_SMC_GoodSectorPerCard %lx\n", G_SMC_GoodSectorPerCard);
	} else
		G_Card_SectorPerCard = 0;

	//patch 5.2.1@wyeo@ allow less than 1001 blocks for nand with one bank begin
	if (G_Card_TotalBank == 1 && G_DOS_ErrCode == 1001)
		G_DOS_ErrCode = 0;
	//patch 5.2.1@wyeo@ allow less than 1001 blocks for nand with one bank end

	return sts;
}
//end

// @012403@wyeo@support 256M nand, begin
//-----------------------------------------------------------------------------
//SMC_SearchValidBlock
//-----------------------------------------------------------------------------
UINT8 SMC_SearchValidBlock(UINT32* pPhyAddr)
{
	UINT8 sts = TRUE;
	UINT8 good;
	UINT8 stsData2 = 0xFF;
	UINT8 stsData1;
	UINT32 maxPhyAddr;

	maxPhyAddr = (UINT32) G_Card_TotalBank * G_Card_PhysicalBankSize - G_NANDF_BlockSize;
	/* add by eric@20080213 */
	//maxPhyAddr = maxPhyAddr - (UINT32) (((UINT32) G_NAND_ReservBlockTotal << G_NANDF_BlockSizeShift));
	do {
#if (K_NANDF_STRICT_BAD_CHECK)
		SMC_RedunCard2Sram(*pPhyAddr + G_NANDF_PageSize); 
#if 1//(K_SUPPORT_NEWNAND)
		if (G_NANDF_Category)
			stsData2 = G_ucDataBuffer[K_Card_PageSize];
		else
#endif
			stsData2 = G_ucDataBuffer[K_Card_PageSize + 5];
		sts = SMC_Card2SramForExt(*pPhyAddr);
#else
		//For "A" Area access
		sts = SMC_Card2SramForExt(*pPhyAddr);
#endif

#if 1//(K_SUPPORT_NEWNAND)
		if (G_NANDF_Category)
			stsData1 = G_ucDataBuffer[K_Card_PageSize];
		else
#endif
			stsData1 = G_ucDataBuffer[K_Card_PageSize + 5];
		if ((good = SMC_BlockStsCheck(*pPhyAddr, stsData1, stsData2)) == FALSE)
			// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
		{
			if (*pPhyAddr >= maxPhyAddr) {
				sts = FALSE;
				//printf("exceed max phy address=%lx\n", *pPhyAddr);
			} else
				*pPhyAddr += G_NANDF_BlockSize;
		}
		// @patch 5.1@012403@wyeo@fix cross nand bank bug, end  				  
	} while (good == FALSE && sts == TRUE);

	return sts;
}
// @012403@wyeo@support 256M nand, end
#if 0
UINT8 SMC_CheckValidBlock(UINT32 *pPhyAddr) 
{
		UINT8 sts = TRUE;
		UINT8 good;
	UINT8 stsData2 = 0xFF;
	UINT8 stsData1;
	UINT32 maxPhyAddr;
	
	maxPhyAddr = (UINT32)G_Card_TotalBank * G_Card_PhysicalBankSize;
	#if (K_NANDF_STRICT_BAD_CHECK)
	SMC_RedunCard2Sram(*pPhyAddr + G_NANDF_PageSize); 
	#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		stsData2 = G_ucStorData[K_Card_PageSize];
	else
	#endif
		stsData2 = G_ucStorData[K_Card_PageSize + 5];
		sts = SMC_Card2SramForExt(*pPhyAddr);
	#else
		//For "A" Area access
		sts = SMC_Card2SramForExt(*pPhyAddr);
		#endif
		
		#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		stsData1 = G_ucStorData[K_Card_PageSize];
	else
	#endif
	stsData1 = G_ucStorData[K_Card_PageSize + 5];
	if ((good = SMC_BlockStsCheck(*pPhyAddr, stsData1, stsData2)) == FALSE)
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
	{
		sts = FALSE;
		//printf("exceed max phy address=%lx\n", *pPhyAddr);
	}
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, end  				  
											   
		return sts;
}
#endif
//-----------------------------------------------------------------------------
//SMC_CisIdi
//-----------------------------------------------------------------------------
UINT8 SMC_CisIdi(UINT32 phyAddr)
{
	UINT8 sts = TRUE;
	UINT16 i, j;
	//CIS/IDI
	//Should be already erased under physical fmt */
	for (i = 0; i < K_Card_PageSize; i++)
		G_ucDataBuffer[i] = (UINT8) 0x00;
	for (i = 0, j = K_Card_PageSize / 2; i < K_Card_PageSize / 4; i++) {
		G_ucDataBuffer[i] = G_ucDataBuffer[i + j] = (UINT8) G_SMC_CIS[i];
	}
	SMC_Sram2Card(phyAddr, 0x0000);
	SMC_Sram2Card(phyAddr + (UINT32) K_Card_PageSize, 0x0000);
	return sts;
}

UINT16 SMC_GetBehavior(void)
{
	G_Media_Behavior = K_MEDIA_SMC_Behavior;
	return G_Media_Behavior;
}

#if ( K_SMC_ErrOption )
void SMC_SetError(UINT16 errCode)
{
	if (!G_SMC_ErrCode)
		G_SMC_ErrCode = errCode;
}

void SMC_ClearError(void)
{
	G_SMC_ErrCode = 0;
}

UINT16 SMC_GetError(void)
{
	return G_SMC_ErrCode;
}
#endif

#if ( K_SMC_BUILD_BANK_AT_SDRAM )
//-----------------------------------------------------------------------------
//SMC_UpdateBankTable
//-----------------------------------------------------------------------------
/*++

Routine Description:
	Load the requested bank's logical to physical table to sdram
Arguments:
		none

Return Value:

		none

--*/
// patch 4.4, 0615@wyeo
void SMC_UpdateBankTable(UINT8 bank, UINT8 toSram)
{
	UINT32 save_SDRAM_NextIntRetAddr;
	UINT8 save_SDRAM_Read;
	UINT32 ramAddr;
	UINT8 bTemp;
	UINT8 bt;
	UINT16 k;//, m;// 0615@wyeo

	save_SDRAM_NextIntRetAddr = G_DOS_SDRAM_NextIntRetAddr;
	save_SDRAM_Read = G_File_SDRAM_Read;

	// assume work buffer size is 128K, cluster size not greater than 48K
	// so left 80K work buffer for smc banking table
	// each bank want 2176 byte ( G_SMC_AllocatTable + G_SMC_EntryTable size )
	// so allow 80K/2176 = 37 bank, means 37 * 16M max smc disk size is 592M
#if 	0
	ramAddr = G_DOS_SDRAM_WorkAddr + K_SMC_MAX_ClusterSize + \
	(((K_SIZEOF_G_SMC_AllocatTable + K_SIZEOF_G_SMC_EntryTable) * (UINT32) bank) >> 1);	
#else
	/*80K for smc banking table, so the max bank is 80K/2176=37bank, max size of smc disk is 592M*/
	if (bank > 37) {
		printf("smc bank over flow...\n");
		while(1);
	}
	ramAddr = G_DOS_SDRAM_WorkAddr + \
	(((K_SIZEOF_G_SMC_AllocatTable + K_SIZEOF_G_SMC_EntryTable) * (UINT32) bank) >> 1);	
#endif 
	k = 0;
	if (toSram) {
		HAL_DramDmaRead(&G_SMC_AllocatTable[k], ramAddr, sizeof(G_SMC_AllocatTable), 1);
		ramAddr += K_SMC_MaxAllocatTableSize;
		HAL_DramDmaRead(&G_SMC_EntryTable[k << 1], ramAddr, sizeof(G_SMC_EntryTable), 1);
	} else {
		HAL_DramDmaWrite(&G_SMC_AllocatTable[k], ramAddr, sizeof(G_SMC_AllocatTable), 1);
		ramAddr += K_SMC_MaxAllocatTableSize;
		HAL_DramDmaWrite(&G_SMC_EntryTable[k << 1], ramAddr, sizeof(G_SMC_EntryTable), 1);

		bTemp = bank >> 3;
		bt = 0x01 << (bank % 8);
		G_SMC_BuildBankMap[bTemp] |= bt;
	}

	M_AUDIO_DISABLE_8051_INT;
	G_DOS_SDRAM_NextIntRetAddr = save_SDRAM_NextIntRetAddr;
	G_File_SDRAM_Read = save_SDRAM_Read;
	M_AUDIO_ENABLE_8051_INT;
}
#if 1
//-----------------------------------------------------------------------------
//SMC_BuildBankTable
//-----------------------------------------------------------------------------
/*++

Routine Description:
	Load the requested bank's logical to physical table to sdram
Arguments:
		bankCount if 0xff, build all bank, other value build until requested
		number bank sastified

Return Value:

		none

--*/
void SMC_BuildBankTable( UINT8 bankCount ) 
{
	UINT8 bank;
	UINT8 bTemp;
	UINT8 bt;
	UINT8 orgBank;
	UINT8 recCount;// @patch5.2.1@wyeo@support xd card@072903

	// @012403@wyeo, begin
	if (G_Card_TotalBank == 1)
		return;

	#if ( K_SMC_BUILD_BANK_AT_SDRAM == 0 )
		return;
	#endif
	// @012403@wyeo, end

		#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		// move current updated copy at sram to dram
		SMC_UpdateBankTable( G_Card_CurrBank, FALSE );
		#endif
	
	orgBank = G_Card_CurrBank;

	// @patch5.2.1@wyeo@support xd card@072903, begin
	if ( bankCount > G_Card_TotalBank )
		bankCount = G_Card_TotalBank;
	recCount = bankCount;
	bankCount = G_Card_TotalBank;
	// @patch5.2.1@wyeo@support xd card@072903, end

	for ( bank = 0; bank < bankCount; bank++ )
	{
		bTemp = bank >> 3;
		bt = 0x01 << (bank % 8);

		if ( ( G_SMC_BuildBankMap[bTemp] & bt ) == 0 )
		{
			SMC_MakeTable(bank);
						#if ( K_SMC_BUILD_BANK_AT_SDRAM )
						// move prebuild copy at sram to dram
						SMC_UpdateBankTable( bank, FALSE );
						#endif
			G_Card_CurrBank = bank;

			// @patch5.2.1@wyeo@support xd card@072903, begin
			if ((--recCount)==0)
				break;
			// @patch5.2.1@wyeo@support xd card@072903, end
		}
	}
	// switch back to original bank
	if ( orgBank != G_Card_CurrBank )
	{
		// @012403@wyeo, begin
	#if 1//( K_SMC_BUILD_BANK_AT_SDRAM )
		// move from sdram to sram
		SMC_UpdateBankTable( orgBank, TRUE );
	#else
		SMC_MakeTable(orgBank);
	#endif
		// @012403@wyeo, end

		G_Card_CurrBank = orgBank;
	}	
}
#endif
#endif

#if ( K_SMC_DO_ECC_CORECTION )
// Suppose IC created ECC have been read to G_SMC_EccByte.
// Also ECC from card page have been read to G_ucDataBuffer 
// 520, 525. 520 have 3 byte for SECOND 256 page byte, 525 have 
// 3 byte for FIRST 256 page byte. Also actual card page data 
// have been read to G_ucDataBuffer 0 to 511 if ramAddr is 0 
// else at the ramAddr (word unit).
UINT8 SMC_CorrectPage(UINT8 page, UINT32 ramAddr, UINT8 doCorrect)
{
	data UINT32 bitPat;
	data UINT32 eccLong = 0;
	data UINT8 j, k, byteOffset, a, b, bitOffset;
	UINT8 sts = TRUE;
	//printf("\n....ECC error...\n");

    #if 0
    printf("ramAddr:%lx\n", ramAddr);
    {
        UINT8 i;
        for(i = 0; i < 6; i++){
            printf("%bx ", G_SMC_EccByte[i]);
        }
        printf("\n");
        for(i = 0; i < 16; i++){
            printf("%bx ", G_ucStorData[512+i]);
        }
    }   
    printf("\n");
    #endif
    
	if (page == 1)
		page = 3;
	else
		page = 6;
	for (k = 0; k < page;) {
		if (k >= 3)
			eccLong |= ((UINT32) (G_SMC_EccByte[k] ^ G_ucDataBuffer[517 + k]) << (16 - ((k - 3) << 3)));
		else
			eccLong |= ((UINT32) (G_SMC_EccByte[k] ^ G_ucDataBuffer[525 + k]) << (16 - (k << 3)));

		k++;
		if (k == 3 || k == 6) {
			if (eccLong != 0) {
				// CORRECTABLE is 0x00555554L
				if (((eccLong ^ (eccLong >> 1)) & 0x00555554L) == 0x00555554L) {
					// One bit ECC error

					bitPat = 0x00800000L; 	//  BIT23
					byteOffset = 0;
					a = 0x80; 		//  BIT7

					for (j = 0; j < 8; j++) {
						if ((eccLong & bitPat) != 0) {
							byteOffset |= a;
						}

						bitPat >>= 2;
						a >>= 1;
					}

					bitOffset = 0;
					b = 0x04;		//  BIT2

					for (j = 0; j < 3; j++) {
						if ((eccLong & bitPat) != 0) {
							bitOffset |= b;
						}

						bitPat >>= 2;
						b >>= 1;
					}

					byteOffset = (((byteOffset & 0xF0) >> 4) | ((byteOffset & 0x0F) << 4));
					//printf("offset:0x%bx, 0x%bx\n", byteOffset, bitOffset);
					if (doCorrect) {
						if (bitOffset < 0x08) {
							if (ramAddr != 0) {
								// patch 4.4, 0615@wyeo
								M_AUDIO_DISABLE_8051_INT;
								if (k == 3)
									SDRAM_CardSetStartAddress(ramAddr + (byteOffset >> 1), TRUE);
								else
									// @patch5.2.1-24@wyeo@fix bit correct@111203, begin
									//SDRAM_CardSetStartAddress(ramAddr+256+(byteOffset>>1), TRUE);
									SDRAM_CardSetStartAddress(ramAddr + 128 + (byteOffset >> 1), TRUE);
								// @patch5.2.1-24@wyeo@fix bit correct@111203, end
								M_DRAM_ReadWord(a, b);
								if (byteOffset & 0x01)
									b ^= (0x01 << bitOffset);
								else
									a ^= (0x01 << bitOffset);
								if (k == 3)
									SDRAM_CardSetStartAddress(ramAddr + (byteOffset >> 1), FALSE);
								else
									// @patch5.2.1-24@wyeo@fix bit correct@111203, begin
									//SDRAM_CardSetStartAddress(ramAddr+256+(byteOffset>>1), FALSE);
									SDRAM_CardSetStartAddress(ramAddr + 128 + (byteOffset >> 1), FALSE);
								// @patch5.2.1-24@wyeo@fix bit correct@111203, end

								M_DRAM_WriteWord(a, b);
								M_AUDIO_ENABLE_8051_INT;
								// patch 4.4, 0615@wyeo
							} else {
								if (k == 3)
									G_ucDataBuffer[byteOffset] ^= (0x01 << bitOffset);
								else
									G_ucDataBuffer[(UINT16) byteOffset + 256] ^= (0x01 << bitOffset);
							}
						} else
							// should not happen
							sts = FALSE;
						G_SMC_Ecc1BitErrCount++;
					}
#if (K_SMC_DO_USB_ECC_BY_HOST)
					else {
						// record the ecc bit offset and fpos into G_DOS_SDRAM_WorkAddr
						ramAddr = G_SMC_Ecc1BitErrCount * 6;
						if (ramAddr + 6 < K_SMC_MAX_ClusterSize) {
							ramAddr = G_DOS_SDRAM_WorkAddr + (ramAddr >> 1);
							SDRAM_CardSetStartAddress(ramAddr, FALSE);
							//DOS_FillLongAtSdram(G_DOS_FilePos + byteOffset);
							HAL_DramWrite32(G_DOS_FilePos + byteOffset);
							/*
															  M_DRAM_WriteWord(G_DOS_FilePos + byteOffset, 
															  		 (G_DOS_FilePos + byteOffset) >> 8);
															  M_DRAM_WriteWord((G_DOS_FilePos + byteOffset) >> 16, 
															  		 (G_DOS_FilePos + byteOffset) >> 24);
															  M_DRAM_WriteWord(bitOffset, 
															  		 bitOffset >> 8);
															 	*/
							G_SMC_Ecc1BitErrCount++;
						}
					}
#endif
				} else {
					// more than 1 bit error
                    printf("more\n");
					sts = FALSE;
					G_SMC_Ecc2BitErrCount++;
				}
			}
			eccLong = 0;
		}
	} // end for

	return sts;
}

#if K_SMC_DO_USB_ECC_BY_HOST

#ifndef _DBG_CLR_WARNING_
UINT32 SMC_Ecc1BitErrCount(void) USING_1 {
	return G_SMC_Ecc1BitErrCount;
}
#endif /*_DBG_CLR_WARNING_*/

#endif

#endif

#endif //DISK_DRV_NAND
#endif
// @012404@wyeo@remove K_PreWriteOldBlock
