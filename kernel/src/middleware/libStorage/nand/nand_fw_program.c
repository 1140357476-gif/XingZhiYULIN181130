/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		main.c

Abstract:

		Module related to main routine

Environment:

		Keil C51 Compiler

Revision History:

--*/
#ifdef ISP_PROGRAM
//=============================================================================
//Define
//=============================================================================
#define IS_NAND_SMALL_BLKS        0
#define IS_NAND_UNKNOWN           1
#define IS_NAND_BIG_BLKS          2
#define SIZE_BEFORE_MAPPING       4096
#define K_RESERVE_BLOCK           10

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "asicreg.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "sdramhw.h"
#include "doslink.h"

#include "isp\isp.h"
/* For compatible with SPCA5000 serials */
typedef struct nandRsvHeader_s {
	UINT8  asciiName[8];  /* 0x0  ~ 0x7  */
	UINT16 signature;     /* 0x8  ~ 0x9  */
	UINT16 rsvBlkOffset;  /* 0xa  ~ 0xb  */
	UINT16 nrRsvBlks;     /* 0xc  ~ 0xd  */
	UINT16 checkSum;      /* 0xe  ~ 0xf  */
	UINT32 fwStartBlk;    /* 0x10 ~ 0x13 */
	UINT32 nrFwBlks;      /* 0x14 ~ 0x17 */
	UINT32 nrBadInRsv;    /* 0x18 ~ 0x1b */
	UINT32 fwStartBlk2;   /* 0x1c ~ 0x1f */
	UINT32 nrFwBlks2;     /* 0x20 ~ 0x23 */
	UINT16 nrRsvA;        /* 0x24 ~ 0x25 *//* vfs format */
	UINT16 nrRsvB;        /* 0x26 ~ 0x27 *//* vfs format */
	UINT16 nrRsvC;        /* 0x28 ~ 0x29 *//* raw format */
	#if 1
	UINT16 reserve;       /* 0x2a ~ 0x2b */
	UINT32 sizeFw1;       /* 0x2c ~ 0x2f */
	UINT32 sizeFw2;       /* 0x30 ~ 0x33 */
	UINT8  rsv0[0xfc8];
	UINT32 rsvVer;        /* 0x4092 ~ 0x4095 */
	#else
	UINT8  rsv0[4096 - 0x2a];
	#endif
	UINT16 mapTable[NR_RSV_BLKS]; /* mapping of reserved blocks */
} nandRsvHeader_t;

//=============================================================================
//ProtoType
//=============================================================================
UINT8 nandFwProgram(void);
UINT8 nandFwWrite(UINT32 hdr_adrr, UINT32 fw_addr);
UINT8 nandFillHeader(UINT32);
UINT8 nandSearchFwMappingTable(UINT32 addr, UINT32 nrRsv);

//=============================================================================
//Variables
//=============================================================================
xdata	UINT32   fwStartBlock;
xdata	UINT32   nrFwBlocks;
xdata	UINT32   nrBadInRsv;
xdata	UINT32   nrBadInFw;
xdata	UINT32   G_First_Valid_Block;
xdata	UINT16 G_Card_TotalSizeMB;

//=============================================================================
//Extern Reference
//=============================================================================
extern xdata	UINT32	G_Firmware_Addr;
extern xdata	UINT32	G_Firmware_Size;
extern xdata	UINT32   G_Dram_Buf_Addr;
extern xdata	UINT8	G_ucStorData[];
extern UINT8 SMC_CheckValidBlock(UINT32 *pPhyAddr);

//=============================================================================
//Program
//=============================================================================
UINT8 nandFwProgram(void) USING_0
/*++

Routine	Description:

		main routine

Arguments:

		none

Return Value:

		none

--*/
{
	UINT8 ret=TRUE;
	UINT32 headerSize,nrPage;
	UINT32 phyAddr = 0;
	UINT32 i;
	UINT32 ramAddr = G_Dram_Buf_Addr;

	printf("nandFwProgram\n");
	ret = SMC_CardDetect();
	/* Mount methods to this device */
	if ( ret == TRUE ) {
		/* Read reserved block's information */
		/* phyaddr is bytes with sector alignment */ 
		ret = SMC_SearchValidBlock(&phyAddr);
		#if 1
		G_First_Valid_Block = phyAddr;
		printf("Valid Block = %x\n",phyAddr);
		#endif
		if ( ret == TRUE ) {
			/* Erase 1st valid block as RSV header */
			if( TRUE != SMC_EraseOneBlock(phyAddr * G_NANDF_BlockSize)){
				printf("Erase 1st Valid Block Fail\n");
				return FALSE;
			}
			/* Fill header to Dram */
			nandFillHeader(ramAddr);
			/* Write Rsv Header */
			headerSize = sizeof(nandRsvHeader_t);
			phyAddr = phyAddr * G_NANDF_BlockSize;
			nrPage = headerSize >> 9;
			for (i = 0; i < nrPage; i++) 
			{
				SDRAM_CardSetStartAddress(ramAddr, TRUE);
				if(TRUE != SMC_Dram2Card(phyAddr, G_First_Valid_Block, ramAddr)){
					printf("Write Rsv Header Fail\n");
					return FALSE;
				}
				phyAddr += 512L;
				ramAddr += 256L;
			}

		}
		else {
			/* Valid block is equal to total block */
			return FALSE;
		}
	}
	else {
		printf("Id Fail\n");
		return FALSE;
	}

	ret = nandFwWrite(G_Dram_Buf_Addr,G_Firmware_Addr);
	if( ret!= TRUE ){
		printf("nandFwWrite Fail\n");
		ret = FALSE;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandFwWrite                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *    Write Firmware to NAND's reserved blocks                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *    The return value of read operation.                                 *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
nandFwWrite(
	UINT32 hdr_addr,
	UINT32 fw_addr
)
{
	UINT8 ret=TRUE;
	UINT8 buf[2];
	UINT16 blk;
	UINT32 i,k;
	//UINT32 l,m;
	UINT32 ramAddr = hdr_addr;
	UINT32 fwAddr = fw_addr;
	UINT32 phyAddr = G_First_Valid_Block, nrPage;
	printf("hdr_addr = %lx,fw_addr = %lx\n",hdr_addr,fw_addr);
	nrPage = (((UINT32)sizeof(nandRsvHeader_t))>>9);
	phyAddr = phyAddr * G_NANDF_BlockSize;
	#if 0
	for (i = 0; i < nrPage; i++) 
	{
		SDRAM_CardSetStartAddress(ramAddr, TRUE);
		SMC_Card2Dram(phyAddr, ramAddr);
		phyAddr += 512L;
		ramAddr += 256L;
	}
	#endif
	#if 1
	SDRAM_CardSetStartAddress(hdr_addr + (SIZE_BEFORE_MAPPING >> 1), TRUE);
	for(k=0;k<1;k++){
		for( i=0;i<16;i=i+2){
			M_DRAM_ReadWord(buf[0],buf[1]);	
			printf("%2bx %2bx ",buf[0],buf[1]);	
		}
		printf("\n");
		
	}
	#endif
	printf("nrFwBlocks=%lx\n",nrFwBlocks);
	for( i = 0; i < nrFwBlocks; i++ ){
		SDRAM_CardSetStartAddress(hdr_addr + (SIZE_BEFORE_MAPPING >> 1) + i + fwStartBlock, TRUE);
		M_DRAM_ReadWord(buf[0],buf[1]);
		blk = (UINT16)buf[0] | (UINT16)buf[1] << 8;
		printf("Program blk=%x\n",blk);
		if( TRUE != SMC_EraseOneBlock(blk * G_NANDF_BlockSize)){
			printf("Erase 1st Valid Block Fail\n");
			return FALSE;
		}
		#if 0
		SDRAM_CardSetStartAddress(fwAddr, TRUE);
		for(l=0;l<16;l++){
			for( m=0;m<16;m=i+2){
				M_DRAM_ReadWord(buf[0],buf[1]);	
				printf("%2bx %2bx ",buf[0],buf[1]);	
			}
			printf("\n");
			
		}
		#endif
		phyAddr = blk * G_NANDF_BlockSize;
		printf("secNum=%lx\n",(G_NANDF_BlockSize>>9));
		nrPage = (G_NANDF_BlockSize>>9);
		for (k = 0; k < nrPage; k++) 
		{
			//printf("fwAddr=%lx,phyAddr=%lx\n",fwAddr,phyAddr);
			SDRAM_CardSetStartAddress(fwAddr, TRUE);
			if(TRUE != SMC_Dram2Card(phyAddr, blk, fwAddr)){
				printf("Write Firmware Fail on block 0x%x\n", blk);
				return FALSE;
			}
			phyAddr += 512L;
			fwAddr += 256L;
		}
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandFillHeader                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *    The return value of read operation.                                 *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
nandFillHeader(
	UINT32 dramAddr
)
{
	UINT32 i;//,k;
	UINT16 rsvBlkOffset;  /* 0xa  ~ 0xb  */
	UINT16 nrRsvBlks;     /* 0xc  ~ 0xd  */
	UINT16 checkSum;      /* 0xe  ~ 0xf  */
	//UINT8 buf[2];	

	SDRAM_CardSetStartAddress(dramAddr + 512, FALSE);
	for( i = 0; i < sizeof(nandRsvHeader_t) - 1024; i=i+2 ){
		M_DRAM_WriteWord(0xff, 0xff);
	}
	nrRsvBlks = (G_Firmware_Size + G_NANDF_BlockSize - 1)/G_NANDF_BlockSize + K_RESERVE_BLOCK;
	nandSearchFwMappingTable(dramAddr + (SIZE_BEFORE_MAPPING >> 1), nrRsvBlks);
	for( i = 0; i < 0x34; i++ ){
		G_ucStorData[i]=0;
	}
	for( i = 0x34; i < 512; i++ ){
		G_ucStorData[i]=0xff;
	}
	/* Fill "NAND RSV" */
	G_ucStorData[0x0] = 'N';
	G_ucStorData[0x1] = 'A';
	G_ucStorData[0x2] = 'N';
	G_ucStorData[0x3] = 'D';
	G_ucStorData[0x4] = ' ';
	G_ucStorData[0x5] = 'R';
	G_ucStorData[0x6] = 'S';
	G_ucStorData[0x7] = 'V';
	/* Fill signature marker */
	G_ucStorData[0x8] = 0xef;
	G_ucStorData[0x9] = 0xbe;
	/* Fill rsvBlkOffset */
	rsvBlkOffset = 	nrRsvBlks;//G_Firmware_Size >> 9;
	G_ucStorData[0xa] = (UINT8)rsvBlkOffset;
	G_ucStorData[0xb] = (UINT8)(rsvBlkOffset >> 8);
	/* Fill nrRsvBlks */
	//nrRsvBlks = (G_Firmware_Size + G_NANDF_BlockSize - 1)/G_NANDF_BlockSize + K_RESERVE_BLOCK;
	G_ucStorData[0xc] = (UINT8)nrRsvBlks;
	G_ucStorData[0xd] = (UINT8)(nrRsvBlks >> 8);
	/* Fill checkSum */
	checkSum = 0x10000 - nrRsvBlks;
	G_ucStorData[0xe] = (UINT8)checkSum;
	G_ucStorData[0xf] = (UINT8)(checkSum >> 8);
	/* Fill fwStartBlk */
	fwStartBlock = nrBadInRsv + K_RESERVE_BLOCK;
	G_ucStorData[0x10] = (UINT8)fwStartBlock;
	G_ucStorData[0x11] = (UINT8)(fwStartBlock >> 8);
	G_ucStorData[0x12] = (UINT8)(fwStartBlock >> 16);
	G_ucStorData[0x13] = (UINT8)(fwStartBlock >> 24);
	/* Fill nrFwBlks */
	nrFwBlocks = (UINT32)((G_Firmware_Size + G_NANDF_BlockSize - 1)/G_NANDF_BlockSize);
	G_ucStorData[0x14] = (UINT8)nrFwBlocks;
	G_ucStorData[0x15] = (UINT8)(nrFwBlocks >> 8);
	G_ucStorData[0x16] = (UINT8)(nrFwBlocks >> 16);
	G_ucStorData[0x17] = (UINT8)(nrFwBlocks >> 24);
	/* Fill nrBadInRsv */
	nrBadInRsv = nrBadInRsv + nrBadInFw;
	G_ucStorData[0x18] = (UINT8)nrBadInRsv;
	G_ucStorData[0x19] = (UINT8)(nrBadInRsv >> 8);
	G_ucStorData[0x1a] = (UINT8)(nrBadInRsv >> 16);
	G_ucStorData[0x1b] = (UINT8)(nrBadInRsv >> 24);
	/* Fill nrRsvC */
	G_ucStorData[0x28] = (UINT8)K_RESERVE_BLOCK;
	G_ucStorData[0x29] = (UINT8)(K_RESERVE_BLOCK >> 8);
	/* Fill sizeFw1 */
	G_ucStorData[0x2c] = (UINT8)G_Firmware_Size;
	G_ucStorData[0x2d] = (UINT8)(G_Firmware_Size >> 8);
	G_ucStorData[0x2e] = (UINT8)(G_Firmware_Size >> 16);
	G_ucStorData[0x2f] = (UINT8)(G_Firmware_Size >> 24);
	#if 1
	printf("rsvBlkOffset 0x%x\n",rsvBlkOffset);
	printf("nrRsvBlks    0x%x\n",nrRsvBlks);
	printf("checkSum     0x%x\n",checkSum);
	printf("fwStartBlk   0x%lx\n",fwStartBlock);
	printf("nrFwBlocks   0x%lx\n",nrFwBlocks);
	printf("nrBadInRsv   0x%lx\n",nrBadInRsv);
	printf("sizeFw1      0x%lx\n",G_Firmware_Size);
	#endif
	/* sizeof(nandRsvHeader_t) = 8192 = 4 * NR_RSV_BLKS  */ 
	/* NR_RSV_BLKS */
	SDRAM_CardSetStartAddress(dramAddr, FALSE);
	for( i = 0; i < 512; i=i+2 ){
		M_DRAM_WriteWord(G_ucStorData[i], G_ucStorData[i+1]);
	}
	#if 0
	SDRAM_CardSetStartAddress(dramAddr, TRUE);
	for(k=0;k<32;k++){
		for( i=0;i<16;i=i+2){
			M_DRAM_ReadWord(buf[0],buf[1]);	
			printf("%2bx %2bx ",buf[0],buf[1]);	
		}
		printf("\n");
		
	}
	#endif
	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandFillHeader                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *    The return value of read operation.                                 *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
nandSearchFwMappingTable(
	UINT32 addr,
	UINT32 nrRsv
)
{
	UINT32 i=0;
	UINT32 phyAddr = ((UINT32)G_Card_TotalBank * G_Card_PhysicalBankSize) - G_NANDF_BlockSize;
	UINT32 blk = (G_Card_PhysicalBankSize / G_NANDF_BlockSize) - 1;
	UINT8 ret = TRUE;
	nrBadInRsv = 0;
	nrBadInFw = 0;
	nrFwBlocks = (G_Firmware_Size + G_NANDF_BlockSize - 1)/G_NANDF_BlockSize;
	nrRsv = nrRsv - nrFwBlocks;
	#if 0
	/* fill 0xff */
	SDRAM_CardSetStartAddress(addr, FALSE);
	for( i = 0; i < NR_RSV_BLKS * 2; i=i+2 ){
		M_DRAM_WriteWord(0xff, 0xff);
	}
	#endif
	/* Here below is mapping table */
	SDRAM_CardSetStartAddress(addr, FALSE);
	printf("addr=%lx\n",addr);
	i= 0;
	while( i < nrRsv ){
		printf("i=0x%lx, nrRsv=%lx\n",i,nrRsv);
		ret = SMC_CheckValidBlock(&phyAddr);
		if( ret == TRUE ){
			blk = phyAddr / G_NANDF_BlockSize;
			printf("blk=%lx\n",blk);
			M_DRAM_WriteWord((UINT8)blk, (UINT8)(blk >> 8))
			phyAddr = phyAddr - G_NANDF_BlockSize;
			i++;		
		}
		else{
			nrBadInRsv ++;
			printf("nrBadInRsv=%lx\n",nrBadInRsv);			
			blk = phyAddr / G_NANDF_BlockSize;
			printf("Fail Blk phyAddr=%lx,blk=%lx\n",phyAddr,blk);			
			phyAddr = phyAddr - G_NANDF_BlockSize;		
		}
	}
	i= 0;
	while( i < nrFwBlocks ){
		printf("i=0x%lx, nrFwBlocks=%lx\n",i,nrFwBlocks);
		ret = SMC_CheckValidBlock(&phyAddr);
		if( ret == TRUE ){
			blk = phyAddr / G_NANDF_BlockSize;
			printf("blk=%lx\n",blk);
			M_DRAM_WriteWord((UINT8)blk, (UINT8)(blk >> 8))
			phyAddr = phyAddr - G_NANDF_BlockSize;
			i++;		
		}
		else{
			nrBadInFw ++;
			printf("nrBadInFw=%lx\n",nrBadInFw);			
			blk = phyAddr / G_NANDF_BlockSize;
			printf("Fail Blk phyAddr=%lx,blk=%lx\n",phyAddr,blk);			
			phyAddr = phyAddr - G_NANDF_BlockSize;		
		}
	}
	return TRUE;
}

#endif
