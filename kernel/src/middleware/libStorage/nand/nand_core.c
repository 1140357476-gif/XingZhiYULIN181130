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
//=============================================================================
//Define
//=============================================================================
#define BANK_A                    0
#define BANK_B                    1
#define BANK_FW                   2
#define IS_NAND_UNKNOWN           1
#define IS_NAND_BIG_BLKS          2
#define IS_NAND_SMALL_BLKS        0
#define IS_NAND_UNKNOWN           1
#define IS_NAND_BIG_BLKS          2
#define SIZE_BEFORE_MAPPING       4096
#define K_RESERVE_BLOCK           10
#define K_BRN_INDEX_SIZE          10
#define BRN_MARKER                32
#define BRN_EOF                   0x246D
#define NR_RSV_BLKS               2048
#define NR_RSV_A                  512//256
#define NR_RSV_B                  512//256//128
#define RAW_A_MAXIDX              0x800// 0x80
#define RAW_B_MAXIDX              0x1000//0x100
#define FW_INDEX                  0x1000//0x100
#if 0
#define major_printf printf
#else
#define major_printf;/K_SLASH
#endif

#if 0
#define smcprint printf
#else
#define smcprint
#endif
//=============================================================================
//Header file
//=============================================================================
#define __FILE   __FILE_ID_SMC__
#include <stdlib.h>
#include "general.h"
#include "reg_def.h"
#include "hal_device.h"
#include "hal.h"

#include "asicreg.h"
#include "smcfunc.h"
#include "smcvar.h"
//#include "cardcon.h"
#include "sdramhw.h"
#include "doslink.h"
#include "initio.h"
//#include "dosusr.h"
#include "dos32.h"
#include "cardlink.h"
#include "smccon.h"
#include "dbg_mem.h"
#include "dbg_def.h"
#include "hal_dram.h"
#include "reg_def.h"

#ifdef DISK_DRV_NAND

#message "Load disk driver NAND"

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
	UINT32 sizeRawA;      /* 0x34 ~ 0x37 */
	UINT32 sizeRawB;      /* 0x38 ~ 0x3b */
	UINT8  rsv0[0xfc0];
	UINT32 rsvVer;        /* 0x4092 ~ 0x4095 */
	#else
	UINT8  rsv0[4096 - 0x2a];
	#endif
	UINT16 mapTable[NR_RSV_BLKS]; /* mapping of reserved blocks */
} nandRsvHeader_t;

//=============================================================================
//ProtoType
//=============================================================================

//=============================================================================
//Variables
//=============================================================================
xdata	UINT32   fwStartBlock;
xdata	UINT32   nrFwBlocks;
xdata	UINT32   G_Firmware_Size;
xdata	UINT32   nrBadInRsv;
xdata	UINT32   nrBadInFw;
xdata	UINT32   G_First_Valid_Block;
xdata	UINT32   rsvRawA;
xdata	UINT32   rsvRawB;
xdata	UINT32   rawAIdxNum;
xdata	UINT32   rawBIdxNum;
xdata	UINT32   rsvASizeByte;
xdata	UINT32   rsvBSizeByte;
xdata	UINT16   rsvAFisrtBlk;
xdata	UINT16   rsvBFisrtBlk;
xdata	UINT32   freeBlkIndexA;
xdata	UINT32   freeBlkIndexB;
xdata	UINT16   rsvAMap[NR_RSV_A];
xdata	UINT16   rsvAFreeMap[NR_RSV_A];
xdata	UINT16   rsvBMap[NR_RSV_B];
xdata	UINT16   rsvBFreeMap[NR_RSV_B];
xdata	UINT8    G_Firmware_Update = 0;
#if _ENABLERLE_
xdata	UINT8    isRleFile = 0;/*zouchd add for rle function*/
#endif
//xdata	UINT32   G_Dram_Buf_Addr = (K_SDRAM_CommonEndAddr >> 1);/*eric temp mark */

xdata UINT32 diskPhySectors;//eric@20080502
static UINT8 xdata sDiskType = (UINT8)DEV_TYPE_DISK_NAND;

xdata UINT8 G_ShadowData[0x1000]	_at_ 0xF000;

//=============================================================================
//Extern Reference
//=============================================================================
extern xdata    UINT32 G_NANDF_BlockSize;
extern xdata	UINT8	G_ucStorData[];
//extern xdata	UINT16   G_NAND_ReservBlockTotal;
extern xdata	UINT32  G_SMC_CisBlockAddress;
extern xdata	UINT16   G_Card_TotalSizeMB;

extern UINT8    SMC_CheckValidBlock(UINT32 *pPhyAddr);

//=============================================================================
//Program
//=============================================================================

#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: dumpData                                  *
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
void dataFillDump( UINT32 total , UINT8 *addr, UINT8 write)
{
	UINT32 i, address=(UINT32)addr;
	if( write == 1 ){
		M_DRAM_SetStartAddressV(address , FALSE)
		for( i = 0 ; i < total ; i = i + 2){
			M_DRAM_WriteWord(G_ucStorData[i],G_ucStorData[i+1]);
		}
	}
	else{
		M_DRAM_SetStartAddressV(address , TRUE)
		for( i = 0 ; i < total ; i = i + 2){
			M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
		}
	}
}
UINT8 MemRawCmp(UINT32 addr1, UINT32 addr2, UINT32 num){
	UINT8 err=0;
	UINT8 tmp0, tmp1, tmp2, tmp3;
	UINT32 i;
	//printf("num=%lx,addr1=%lx,addr2=%lx\n", num, addr1, addr2);
	for(i=0; i<num/2; i++)
	{
		M_DRAM_SetStartAddress(addr1 + i, TRUE);
		M_DRAM_ReadWord(tmp0, tmp1);
		M_DRAM_SetStartAddress(addr2 + i, TRUE);
		M_DRAM_ReadWord(tmp2, tmp3);
		if((tmp0 == tmp0) && (tmp1 == tmp3)){
			continue;
		}
		//printf("i=%06lx <%02bX%02bX%02bX%02bX != %08lx>\n", i, tmp0, tmp1,tmp2, tmp3, i);
		err++;
	}
//	printf("comp end, err=%bd\n", err);
	return err;
}

UINT8
nandPhyBlkRd(UINT32 A, UINT32 blk, UINT32 addr)
{
	UINT32 nrPage, phy;
	UINT32 k;
	if(A){
		phy = rsvAMap[blk];
	}
	else{
		phy = rsvBMap[blk];
	}
	phy = phy * G_NANDF_BlockSize;
	nrPage = G_NANDF_BlockSize >> 9;
	#if 0
	printf("nrPage=%lx,phy=%lx,blk=%lx\n",nrPage,phy, phy / G_NANDF_BlockSize);
	#endif
	for (k = 0; k < nrPage; k++)
	{
		SDRAM_CardSetStartAddress(addr, TRUE);
		SMC_Card2Dram(phy, addr);
		phy += 512L;
		addr += 256L;
	}
	return TRUE;
}
UINT32
nandReadAllRaw(UINT32 addr)
{
	UINT32 nrPage, phy;
	UINT32 i, k, size;
	size = 0;
	for( k = 0; k < rsvRawA ; k ++){
		phy = rsvAMap[k];
		if( phy == 0xffff ){
			continue;
		}
		phy = phy * G_NANDF_BlockSize;
		nrPage = G_NANDF_BlockSize >> 9;
		#if 0
		printf("nrPage=%lx,phy=%lx,blk=%lx\n",nrPage,phy, phy / G_NANDF_BlockSize);
		#endif
		for (i = 0; i < nrPage; i++)
		{
			SDRAM_CardSetStartAddress(addr, TRUE);
			SMC_Card2Dram(phy, addr);
			phy += 512L;
			addr += 256L;
			size += 512L;
		}

	}
	for( k = 0; k < rsvRawB ; k ++){
		phy = rsvBMap[k];
		if( phy == 0xffff ){
			continue;
		}
		phy = phy * G_NANDF_BlockSize;
		nrPage = G_NANDF_BlockSize >> 9;
		#if 0
		printf("nrPage=%lx,phy=%lx,blk=%lx\n",nrPage,phy, phy / G_NANDF_BlockSize);
		#endif
		for (i = 0; i < nrPage; i++)
		{
			SDRAM_CardSetStartAddress(addr, TRUE);
			SMC_Card2Dram(phy, addr);
			phy += 512L;
			addr += 256L;
			size += 512L;
		}

	}
	return size;
}
#endif

//-----------------------------------------------------------------------------
//sp1kDiskTypeGet
//-----------------------------------------------------------------------------

UINT8 sp1kDiskTypeGet(void)
{
	return sDiskType;
}



UINT8
sp1kDiskRsvRead(
	UINT32 idx,
	UINT32 addr
)
{
	return nandRsvRead(idx, addr);
}

UINT8
sp1kDiskRsvWrite(
	UINT32 idx,
	UINT32 addr
)
{
	return nandRsvWrite(idx, addr);
}

UINT8
sp1kDiskRsvSizeGet(
	UINT32 idx,
	UINT32 *offset,
	UINT32 *size
)
{
	return nandRsvSizeGet(idx, offset, size);
}

#if _ENABLERLE_

extern UINT32
cmdHwGprmOsdDecode(
	UINT32 addrSrcWord,
	UINT32 addrDestWord
);

UINT32
sp1kRsvGetReadBuf(
	void
)
{
	//need to be implemented;
}
#endif


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
nandRsvRead(
	UINT32 idx,
	UINT32 addrDecByte
)
{
	UINT32 offset, size, k, offsetAlign, temp, blkidx;
	UINT32 alignOffset, nrPage, curBlk, rdSize;
	UINT8 ret, idxbank = 0;
	UINT32 phyAddr = 0;
	UINT32 fwMapPhyAddr;	//xian ++ 20110413
#if _ENABLERLE_
	UINT32 addr = sp1kRsvGetReadBuf();
#else
	UINT32 addr = addrDecByte;
#endif
	UINT32 tmp_addr = addr;
	UINT8 status;

	ENTER_CRITICAL( status );

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//printf("%s(%ld) idx=%lx,addr =%lx\n",__FILE__,(UINT32)__LINE__,idx,addr);
	if( addr == 0 ){
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		EXIT_CRITICAL( status );
		return FALSE;
	}
	ret = nandRsvSizeGet(idx, &offset, &size);
	if( ret == FALSE ){
		//major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		EXIT_CRITICAL( status );
		return FALSE;
	}
	if(idx < RAW_A_MAXIDX ){
		idxbank = BANK_A;/* to access raw A */
	}
	else if( idx < RAW_B_MAXIDX ){
		idxbank = BANK_B;/* to access raw B */
	}
	else if( idx == FW_INDEX ){
		idxbank = BANK_FW;/* to access FW */
	}
	//printf("%s(%ld) offset =%lx,size =%lx\n",__FILE__,(UINT32)__LINE__,offset,size);
	nrPage = (G_NANDF_BlockSize >> 9);
	//nrPageRemain = ((size + 511) >> 9);
	alignOffset = (offset >> 9);
	offsetAlign = offset % 512;
	//printf("alignOffset=%lx,nrPage=%lx,nrPageRemain=%lx,offsetAlign=%lx\n",alignOffset,nrPage,nrPageRemain,offsetAlign);
	//printf("phyAddr=%lx\n",phyAddr);
	//How many block has to be offset from offset 0
	temp = alignOffset / nrPage;
	//printf("offset blk=%lx,curBlk=%lx\n",temp,curBlk);
	/* check next block is good block */
	blkidx = temp;
	if( idxbank == BANK_B ){/* to access raw B */
		curBlk = rsvBMap[blkidx];
	}
	else if( idxbank == BANK_A ){
		curBlk = rsvAMap[blkidx];
	}
	else if( idxbank == BANK_FW ){//xian modify 20110413,  NR_RSV_A = 512, but only 512byte(256 blks' idx) is used in G_ucStorData
		phyAddr = 0;
		ret = SMC_SearchValidBlock(&phyAddr);
		phyAddr = phyAddr + SIZE_BEFORE_MAPPING + ((fwStartBlock << 1 ) & ~511UL); //xian 20110413, read 512byte once
		SMC_Card2Sram(phyAddr);
		//dumpData(512, &G_ucStorData[0] ,0);
		//curBlk = G_ucStorData[fwStartBlock*2] | G_ucStorData[fwStartBlock*2 + 1] << 8; //xian -- 20110413 

		//xian ++ 20110413, related to the G_ucStorData[0]
		fwMapPhyAddr = phyAddr;
		blkidx = fwStartBlock&0x000000FF; 
		curBlk = G_ucStorData[blkidx*2] | G_ucStorData[blkidx*2 + 1] << 8;
		//xian --
		
		//printf("blk=0x%lx,idxbank=0x%bx\n",curBlk,idxbank);
	}
	phyAddr = curBlk * G_NANDF_BlockSize;
	//printf("0 blk=0x%lx,phyAddr=%lx\n",curBlk,phyAddr);
	//dumpData(16, &G_ucStorData[0x200] ,0);
	phyAddr = phyAddr + (((offset >> 9) << 9) - (temp * G_NANDF_BlockSize));
	//printf("phyAddr=%lx,%lx\n",phyAddr,((offset >> 9) << 9));
	rdSize = 0;
	while( rdSize < size ){
	//printf("rdSize=%lx,size =%lx\n",rdSize,size);
		if( offsetAlign ){/* not 512 byte alignment */
			//printf("00 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
			SMC_Card2Sram(phyAddr);
			//SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), 512L);
			if( size < (512 - offsetAlign)){
				SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), size);
			}
			else{
				SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), (512L-offsetAlign));
			}
			phyAddr += 512L;
			addr = addr + ( 512L - offsetAlign );
			rdSize += ( 512L - offsetAlign );
			if( rdSize >= size ){
				break;
			}
			temp = (phyAddr - (curBlk * G_NANDF_BlockSize)) >> 9;
			//printf("0 phyAddr=%lx,temp =%lx\n",phyAddr,temp);
			for (k = temp; k < nrPage; k++) {
				if( rdSize >= size ){
					break;
				}
				if ( rdSize + 512UL > size ) {
					temp = (size - rdSize);
					SMC_Card2Sram(phyAddr);
					SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), temp);
					phyAddr += temp;
					addr	+= temp;
					rdSize += temp;
				} else {				
					//printf("0 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
					SDRAM_CardSetStartAddress((addr >> 1), TRUE);
					SMC_Card2Dram(phyAddr, (addr >> 1));
					phyAddr += 512L;
					addr	+= 512L;
					rdSize += 512L;
				}
			}
		}
		else{/* Is 512 byte alignment */
			if (size <= 0x200){
				//printf("5 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
				SMC_Card2Sram(phyAddr);
				SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), (size - rdSize));
				break;
			}
			else
			{
				temp = (phyAddr - (curBlk * G_NANDF_BlockSize)) >> 9;
				//printf("1 phyAddr=%lx,temp =%lx\n",phyAddr,temp);
				for (k = temp; k < nrPage; k++) {
					//printf("1 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx,size=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage,size);
					if( rdSize >= size ){
						//addr = addr >> 1;
						//dumpData(512, (addr - 256) ,1);
						break;
					}
					if ( rdSize + 512UL > size ) {
						temp = (size - rdSize);
						SMC_Card2Sram(phyAddr);
						SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), temp);
						phyAddr += temp;
						addr	+= temp;
						rdSize += temp;
					} else {				
							//printf("0 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
						SDRAM_CardSetStartAddress((addr >> 1), TRUE);
						SMC_Card2Dram(phyAddr, (addr >> 1));
						phyAddr += 512L;
						addr    += 512L;
						rdSize += 512L;
					}
				}
			}
		}
		blkidx ++;
		if( idxbank == BANK_B ){/* to access raw B */
			curBlk = rsvBMap[blkidx];
		}
		else if( idxbank == BANK_A ){
			curBlk = rsvAMap[blkidx];
		}
		else if( idxbank == BANK_FW ){
			//curBlk = G_ucStorData[blkidx*2 + fwStartBlock*2] | G_ucStorData[blkidx*2 + fwStartBlock*2 + 1] << 8; //xian -- 20110413

			//xian ++ 20110413
			if(blkidx == 256){
				fwMapPhyAddr += 512L;
				SMC_Card2Sram(fwMapPhyAddr);
				blkidx = 0; //xian ++ 20110413, related to the G_ucStorData[0]
			}
			curBlk = G_ucStorData[blkidx*2] | G_ucStorData[blkidx*2 + 1] << 8;
			//xian --
			
			//printf("blk=0x%lx,idxbank=0x%bx\n",curBlk,idxbank);
		}
		phyAddr = curBlk * G_NANDF_BlockSize;
		//printf("1 blk=0x%lx,phyAddr=%lx\n",curBlk,phyAddr);
		offsetAlign = 0;
	}
	#if 0
	dumpData( 256 , (tmp_addr >> 1), 1);
	#endif

	
#if _ENABLERLE_
	if (isRleFile == 1) {
		cmdHwGprmOsdDecode(tmp_addr>>1, addrDecByte>>1);
	}
#endif
	EXIT_CRITICAL( status );
	return TRUE;
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
UINT32
nandFreeBlkReq(
	UINT32 idx,
	UINT32 num,
	UINT16 *phy,
	UINT32 bank
)
{
	UINT16 blk;//,temp;
	UINT32 i, sizeLimit,freeBlkIndex;//, temp;
	num = num;
	//printf("nandFreeBlkReq bank=%lx\n",bank);
	if( bank == BANK_A ){
		sizeLimit = rsvRawA;
		freeBlkIndex = freeBlkIndexA;
	}
	else if( bank == BANK_B ){
		sizeLimit = rsvRawB;
		freeBlkIndex = freeBlkIndexB;
	}
	i = 0;
	*phy = 0xffff;
	#if 0
	printf("\n");
	printf("A m..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAMap[temp]);

	}
	printf("\n");
	printf("A f..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAFreeMap[temp]);
	}
	printf("\n");
	printf("B m..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBMap[temp]);

	}
	printf("\n");
	printf("B f..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBFreeMap[temp]);
	}
	printf("\n");
	#endif
	while( i == 0 ){
		if( freeBlkIndex >= sizeLimit ){
			freeBlkIndex = 0;
		}
		else{
			if( bank == BANK_A ){
				if( rsvAFreeMap[freeBlkIndex] != 0xffff ){
					if( *phy == 0xffff ){
						blk = rsvAFreeMap[freeBlkIndex];
						*phy = blk;
						//printf("A  frm=%lx,To=%x,freeBlkIndex=%lx,num=%lx\n",(UINT32)rsvAMap[idx],blk,freeBlkIndex,num);
						rsvAFreeMap[freeBlkIndex] = rsvAMap[idx];
						//SMC_EraseOneBlock(rsvAMap[idx] * G_NANDF_BlockSize);
						rsvAMap[idx] = *phy;
						freeBlkIndex ++;
						//num --;
						i ++;
						break;
					}
				}
				else{
					freeBlkIndex ++;
				}
			}
			else if( bank == BANK_B ){
				if( rsvBFreeMap[freeBlkIndex] != 0xffff ){
					if( *phy == 0xffff ){
						blk = rsvBFreeMap[freeBlkIndex];
						*phy = blk;
						//printf("B frm=%lx,To=%x,freeBlkIndex=%lx,num=%lx\n",(UINT32)rsvBMap[idx],blk,freeBlkIndex,num);
						rsvBFreeMap[freeBlkIndex] = rsvBMap[idx];
						//SMC_EraseOneBlock(rsvBMap[idx] * G_NANDF_BlockSize);
						rsvBMap[idx] = *phy;
						freeBlkIndex ++;
						//num --;
						i ++;
						break;
					}
				}
				else{
					freeBlkIndex ++;
				}
			}
		}
	}
	if( bank == BANK_A ){
		freeBlkIndexA = freeBlkIndex;
	}
	else if( bank == BANK_B ){
		freeBlkIndexB = freeBlkIndex;
	}
	#if 0
	printf("\n");
	printf("A m..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAMap[temp]);

	}
	printf("\n");
	printf("A f..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAFreeMap[temp]);
	}
	printf("\n");
	printf("B m..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBMap[temp]);

	}
	printf("\n");
	printf("B f..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBFreeMap[temp]);
	}
	printf("\n");
	#endif
	return i;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandRawWrite                                          *
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
nandRawWrite(
	UINT32 addr,
	UINT32 offset,
	UINT32 size,
	UINT32 bank
)
{
	UINT32 nrPage, nrBlk, num, i, tmp, erase;
	UINT32 blkidx = 0;
	UINT32 srcPhyAddr = 0, dstPhyAddr;
	UINT32 tmp_addr = addr;
	UINT32 oldSize0, oldSize1, newSize0, newSize1, newSize2;
	UINT16 phyblk;
	UINT32 fwMapPhyAddr;

	if( size == 0 ){
		return FALSE;
	}
	/* oldSize0 is the beginning old data of 1st block unalignment segment */
	oldSize0 = 0;
	/* oldSize1 is the remain old data of last block */
	oldSize1 = 0;
	/* newSize0 is the remain new data of last block */
	newSize0 = 0;
	/* newSize1 is the block alignment new data */
	newSize1 = 0;
	/* newSize2 is the beginning new data of last block */
	newSize2 = 0;
	//printf("addr=%lx,offset=%lx,size=%lx,bank=%lx\n",addr,offset,size,bank);
	oldSize0 = offset % G_NANDF_BlockSize;
	nrBlk = ((oldSize0 + size + G_NANDF_BlockSize - 1) / G_NANDF_BlockSize);
	if( nrBlk == 1 ){
		newSize0 = size;
		oldSize1 = G_NANDF_BlockSize - oldSize0 - newSize0;
	}
	else if( nrBlk == 2 ){
		newSize0 = G_NANDF_BlockSize - oldSize0;
		newSize2 = size - newSize0;
		oldSize1 = G_NANDF_BlockSize - newSize2;
	}
	else{
		newSize0 = G_NANDF_BlockSize - oldSize0;
		newSize1 = (( size - newSize0 ) / G_NANDF_BlockSize ) * G_NANDF_BlockSize;
		newSize2 = size - newSize0 - newSize1;
		oldSize1 = G_NANDF_BlockSize - newSize2;
	}
	#if 0
	printf("oldSize0=%lx\n",oldSize0);
	printf("oldSize1=%lx\n",oldSize1);
	printf("newSize0=%lx\n",newSize0);
	printf("newSize1=%lx\n",newSize1);
	printf("newSize2=%lx\n",newSize2);
	printf("nrBlk   =%lx\n",nrBlk);
	#endif
	if( !oldSize0 && !oldSize1 && !newSize0 && !newSize1 && !newSize2 ){
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		return FALSE;
	}
	nrPage = (G_NANDF_BlockSize >> 9);
	blkidx = (offset >> 9) / (G_NANDF_BlockSize >> 9);
	if( bank == BANK_A ){
		srcPhyAddr = rsvAMap[blkidx] * G_NANDF_BlockSize;
	}
	else if( bank == BANK_B ){
		srcPhyAddr = rsvBMap[blkidx] * G_NANDF_BlockSize;
	}
	else if( bank == 2 ){
		if( G_Firmware_Update != 0x5a ){
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
		major_printf("%s(%ld), FW Update ..\n",__FILE__,(UINT32)__LINE__);
		/* FW bank */
		dstPhyAddr = 0;
		SMC_SearchValidBlock(&dstPhyAddr);
		dstPhyAddr = dstPhyAddr + SIZE_BEFORE_MAPPING + ((fwStartBlock << 1 ) & ~511UL); //xian 20110413, read 512byte once
		SMC_Card2Sram(dstPhyAddr);
		//dumpData(512, &G_ucStorData[0] ,0);

		//xian ++ 20110413
		fwMapPhyAddr = dstPhyAddr;
		blkidx = fwStartBlock&0x000000FF; 
		//xian --

		addr = addr >> 1;
		for( i = 0; i < nrFwBlocks ; i ++ ){
			//phyblk = G_ucStorData[i*2 + fwStartBlock*2] | G_ucStorData[i*2 + fwStartBlock*2 + 1] << 8;	//xian -- 20110413

			//xian ++ 20110413, blkidx related to the G_ucStorData[0]
			if(blkidx == 256){
				fwMapPhyAddr += 512;
				blkidx = 0; 
				SMC_Card2Sram(fwMapPhyAddr);
			}
			phyblk = G_ucStorData[blkidx*2] | G_ucStorData[blkidx*2 + 1] << 8;
			//xian --
			dstPhyAddr = phyblk * G_NANDF_BlockSize;
			SMC_EraseOneBlock(dstPhyAddr);
			for( tmp = 0; tmp < nrPage ; tmp ++ ){
				//printf("blk=0x%lx,phyAddr=0x%lx,addr=%lx\n",(UINT32)phyblk,dstPhyAddr,addr);
				SMC_Dram2Card(dstPhyAddr,phyblk, addr);
				addr += 256L;
				dstPhyAddr += 512L;
			}

			blkidx ++;	//xian ++ 20110413
		}
		return TRUE;
	}
	addr = addr >> 1;
	num = nandFreeBlkReq(blkidx, 1, &phyblk, bank);
	//printf("nrBlk=%lx,num=%lx,phyblk=%x\n",nrBlk,num,phyblk);
	/* write oldsize0 */
	dstPhyAddr = phyblk * G_NANDF_BlockSize;
	//printf("srcPhyAddr=%lx,dstPhyAddr=%lx,phyblk=%lx\n",srcPhyAddr,dstPhyAddr,srcPhyAddr/G_NANDF_BlockSize);
	if( nrBlk == 1 ){
		//printf("case 0 - 1 block\n");
		erase = srcPhyAddr;
		if( oldSize0 ){
			while( oldSize0 >= 512 ){
				//printf("0 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				SMC_Card2Sram(srcPhyAddr);
				//dumpData(16, &G_ucStorData[0] ,0);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 -= 512L;
			}
			/* remain of oldSize0 not 512 alignment */
			if( oldSize0 || newSize0 ){
				//printf("1 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				SMC_Card2Sram(srcPhyAddr);
				//dumpData(512, &G_ucStorData[0] ,0);
				//dumpData(newSize0, addr ,1);
				M_DRAM_SetStartAddressV(addr , TRUE)
				if( newSize0 >= (512 - oldSize0)){
					for( i = oldSize0 ; i < 512 ; i = i + 2){
						M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
					}
					addr = addr + (512 - oldSize0)/2;
					newSize0 -= (512 - oldSize0);
					//printf("11 oldSize0=%lx,newSize0=%lx,oldSize1=%lx\n",oldSize0,newSize0,oldSize1);
				}
				else{
					for( i = oldSize0 ; i < (oldSize0 + newSize0) ; i = i + 2){
						M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
					}
					addr = addr + (newSize0/2);
					oldSize1 = oldSize1 - (512 - oldSize0 - newSize0);
					newSize0 = 0;
//					printf("22 oldSize0=%lx,newSize0=%lx,oldSize1=%lx\n",oldSize0,newSize0,oldSize1);
				}
				//dumpData(512 - oldSize0, addr ,1);
				//dumpData(512 - oldSize0, &G_ucStorData[oldSize0] ,0);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 = 0;
			}
		}
		/* newSize0 */
		if( newSize0 ){
			while( newSize0 >= 512 ){
				//printf("6 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
				SMC_Dram2Card(dstPhyAddr,blkidx, addr);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				addr += 256L;
				newSize0 -= 512L;
			}
			/* remain of oldSize0 not 512 alignment */
			if( newSize0 || oldSize1 ){
				//printf("7 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
				SMC_Card2Sram(srcPhyAddr);
				//dumpData(512, &G_ucStorData[0] ,0);
				//dumpData(newSize0, addr ,1);
				M_DRAM_SetStartAddressV(addr , TRUE)
				for( i = 0 ; i < newSize0 ; i = i + 2){
					M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
				}
				addr = addr + newSize0/2;
				//printf("33 oldSize0=%lx,newSize0=%lx,oldSize1=%lx\n",oldSize0,newSize0,oldSize1);
				if( oldSize1 >= (512 - newSize0)){
					oldSize1 -= (512 - newSize0);
				}
				else{
					oldSize1 = 0;
				}
				//dumpData(512 - oldSize0, addr ,1);
				//dumpData(512 - oldSize0, &G_ucStorData[oldSize0] ,0);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				newSize0 = 0;
			}
		}
		/* oldSize1 */
		if( oldSize1 ){
			while( oldSize1 >= 512 ){
				//printf("8 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize1=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize1,addr);
				SMC_Card2Sram(srcPhyAddr);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize1 -= 512L;
			}
		}
		SMC_EraseOneBlock(erase);
	}
	else{
		//printf("case 2 - over 1 block\n");
		if( oldSize0 ){
			erase = srcPhyAddr;
			while( oldSize0 >= 512 ){
				//printf("0 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				SMC_Card2Sram(srcPhyAddr);
				//dumpData(16, &G_ucStorData[0] ,0);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 -= 512L;
			}
			/* remain of oldSize0 not 512 alignment */
			if( oldSize0 ){
				//printf("1 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				SMC_Card2Sram(srcPhyAddr);
				//dumpData(512, &G_ucStorData[0] ,0);
				M_DRAM_SetStartAddressV(addr , TRUE)
				if( newSize0 >= (512 - oldSize0)){
					newSize0 -= (512 - oldSize0);
				}
				for( i = oldSize0 ; i < 512 ; i = i + 2){
					M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
				}
				//dumpData(512 - oldSize0, addr ,1);
				//dumpData(512 - oldSize0, &G_ucStorData[oldSize0] ,0);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				addr = addr + (512 - oldSize0)/2;
			}
			SMC_EraseOneBlock(erase);
		}
		/* newSize0 */
		while( newSize0 >= 512 ){
			//printf("2 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
			//dumpData(512, addr ,1);
			SMC_Dram2Card(dstPhyAddr,blkidx, addr);
			dstPhyAddr += 512L;
			addr += 256L;
			newSize0 -= 512L;
		}
		/* newSize1 */
		num = (newSize1 / G_NANDF_BlockSize);
		while( num ){
			blkidx ++;
			num--;
			nandFreeBlkReq(blkidx, 1, &phyblk, bank);
			dstPhyAddr = phyblk * G_NANDF_BlockSize;
			if( bank == BANK_A ){
				if( freeBlkIndexA != 0 ){
					tmp = freeBlkIndexA - 1;
				}
				srcPhyAddr = rsvAFreeMap[tmp] * G_NANDF_BlockSize;
			}
			else if( bank == BANK_B ){
				if( freeBlkIndexB != 0 ){
					tmp = freeBlkIndexB - 1;
				}
				srcPhyAddr = rsvBFreeMap[tmp] * G_NANDF_BlockSize;
			}
			SMC_EraseOneBlock(srcPhyAddr);
			//printf("srcPhyAddr=%lx,tmp=%lx\n",srcPhyAddr,tmp);
			for( i = 0; i < nrPage ; i ++ ){
				SMC_Dram2Card(dstPhyAddr,blkidx, addr);
				dstPhyAddr += 512L;
				srcPhyAddr += 512L;
				addr += 256L;
			}
			newSize1 -= G_NANDF_BlockSize;
			//printf("3 srcPhyAddr=%lx,dstPhyAddr=%lx,%lx\n",srcPhyAddr,dstPhyAddr,addr);
		}
		/* newSize2 */
		
		//deal special condition begin	@matins#45156
		if( newSize2 == 0 || oldSize1 == G_NANDF_BlockSize )
		{
			return TRUE;
		}
		//deal end; 	thus avoid one time nand block erase 
		
		if( newSize2 ){
			blkidx ++;
			if( bank == BANK_A ){
				srcPhyAddr = rsvAMap[blkidx] * G_NANDF_BlockSize;
			}
			else if( bank == BANK_B ){
				srcPhyAddr = rsvBMap[blkidx] * G_NANDF_BlockSize;
			}
			num = nandFreeBlkReq(blkidx,1, &phyblk, bank);
			dstPhyAddr = phyblk * G_NANDF_BlockSize;
		}
		erase=srcPhyAddr;	   //add  1029
		while( newSize2 >= 512 ){
			//printf("4 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize2=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize2,addr);
			SMC_Dram2Card(dstPhyAddr,blkidx, addr);
			srcPhyAddr += 512L;
			dstPhyAddr += 512L;
			newSize2 -= 512L;
			addr += 256L;
		}
		/* remain of newSize2 not 512 alignment */
		if( newSize2 ){
			//printf("5 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize2=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize2,addr);
			SMC_Card2Sram(srcPhyAddr);
			//dumpData(512, &G_ucStorData[0] ,0);
			M_DRAM_SetStartAddressV(addr , TRUE)
			for( i = 0 ; i < newSize2 ; i = i + 2){
				M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
			}
			//dumpData(512, &G_ucStorData[0] ,0);
			SMC_Sram2Card(dstPhyAddr, blkidx);
			srcPhyAddr += 512L;
			dstPhyAddr += 512L;
			addr = addr + (newSize2 / 2);
			if( oldSize1 >= ( 512 - newSize2 )){
				oldSize1 -= ( 512 - newSize2 );
				newSize2 = 0;
			}
			/*********************************/
			if(oldSize1==0){                                             //if oldSize1 is smaller than 512
				SMC_EraseOneBlock(erase);
				}
			/********************************/
		}
		/* oldSize1 */
		if( oldSize1 ){
			erase = ( srcPhyAddr / G_NANDF_BlockSize )  * G_NANDF_BlockSize;
			while( oldSize1 >= 512 ){
				//printf("6 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize1=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize1,addr);
				SMC_Card2Sram(srcPhyAddr);
				SMC_Sram2Card(dstPhyAddr, blkidx);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize1 -= 512L;
			}
			SMC_EraseOneBlock(erase);
		}
	}
	return TRUE;
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
nandRsvWrite(
	UINT32 idx,
	UINT32 addr
)
{
	UINT32 offset, size;
	UINT8 ret, idxbank = 0;

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//printf("%s(%ld) idx=%lx,addr =%lx\n",__FILE__,(UINT32)__LINE__,idx,addr);
	if( addr == 0 ){
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		return FALSE;
	}
	//printf("%s(%ld) idx=%lx,addr =%lx,offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,idx,addr,offset,size);
	if(idx < RAW_A_MAXIDX ){
		idxbank = BANK_A;/* to access raw A */
	}
	else if( idx < RAW_B_MAXIDX ){
		idxbank = BANK_B;/* to access raw B */
	}
	else if( idx == FW_INDEX ){
		idxbank = BANK_FW;/* to access FW */
	}
	//printf("idx %lx\n", idx);
	ret = nandRsvSizeGet(idx, &offset, &size);
	//printf("idx %lx\n", idx);
	if(ret == FALSE){
		//major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		return FALSE;
	}
	//printf("idxbank %bx\n", idxbank);
	return nandRawWrite(addr, offset, size, idxbank);
}

//only for avoiding one link warning,  begin
UINT8
spiRsvWrite(
	UINT32 idx,
	UINT32 addr
)
{
	idx = idx;
	addr = addr;
	return TRUE;
}
//only for avoiding one link warning,  end

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
nandRsvSizeGet(
	UINT32 idx,
	UINT32 *offset,
	UINT32 *size
)
{
	UINT32 blkTotalnum,temp = 0;
	UINT8 idxbank = 0;
	UINT32 phyAddr = 0;
	UINT16 i;

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	if( G_NANDF_BlockSize == 0 ){
		if( FALSE == nandDetect()) {
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
		if(( rsvRawA == 0 ) && ( rsvRawB == 0 ) ){
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
	}
	blkTotalnum = (G_Card_PhysicalBankSize / G_NANDF_BlockSize);
	rsvAFisrtBlk = rsvAMap[0];
	rsvBFisrtBlk = rsvBMap[0];
	//printf("%s(%ld) rsvB=%x,rsvA=%x\n",__FILE__,(UINT32)__LINE__,rsvAFisrtBlk,rsvBFisrtBlk);
	/*
	if( idx == 0xffff ){
		printf("Read All rsv %s(%ld)",__FILE__,(UINT32)__LINE__);
	}
	*/
	if(idx < RAW_A_MAXIDX ){
		idxbank = BANK_A;/* to access raw A */
		phyAddr = rsvAFisrtBlk * G_NANDF_BlockSize;
	}
	else if( idx < RAW_B_MAXIDX ){
		idxbank = BANK_B;/* to access raw B */
		phyAddr = rsvBFisrtBlk * G_NANDF_BlockSize;
	}
	else if( idx == FW_INDEX ){
		idxbank = BANK_FW;/* to access FW */
	}
	else{
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		return FALSE;
	}
//printf("3 idx %lx\n", idx);
	{
		UINT8 retry = 0;
_nandRsvSizeGet_Retry:
		if( TRUE != SMC_Card2Sram(phyAddr)){
			dumpData( 16 , (UINT32)&G_ucStorData[0], 0);
			if( retry == 0 ){
				retry ++;
				goto _nandRsvSizeGet_Retry;
			}
		}
	}
	#if 0
	dumpData( 16 , G_ucStorData, 0);
	#endif
	if( idxbank == BANK_B ){ /* read B Hdr */
		#if 0
		dumpData( 16 , G_ucStorData, 0);
		#endif
		WRITE8(rawBIdxNum,0,G_ucStorData[0]);
		WRITE8(rawBIdxNum,1,G_ucStorData[1]);
		WRITE8(rawBIdxNum,2,G_ucStorData[2]);
		WRITE8(rawBIdxNum,3,G_ucStorData[3]);
		if((idx - RAW_A_MAXIDX) >= rawBIdxNum ){
			major_printf("Error %s(%ld), %lx, %lx\n",__FILE__,(UINT32)__LINE__,idx,rawBIdxNum);
			if( offset != NULL ){
				*offset = 0;
			}
			if( size != NULL ){
				*size = 0;
			}
			return FALSE;
		}
		temp = idx - RAW_A_MAXIDX;
	}
	else if( idxbank == BANK_A ){
		WRITE8(rawAIdxNum,0,G_ucStorData[0]);
		WRITE8(rawAIdxNum,1,G_ucStorData[1]);
		WRITE8(rawAIdxNum,2,G_ucStorData[2]);
		WRITE8(rawAIdxNum,3,G_ucStorData[3]);
		if( idx >= rawAIdxNum ){
			major_printf("Error %s(%ld), %lx, %lx\n",__FILE__,(UINT32)__LINE__,idx,rawAIdxNum);
			if( offset != NULL ){
				*offset = 0;
			}
			if( size != NULL ){
				*size = 0;
			}
			return FALSE;
		}
		temp = idx;
	}
	else if( idxbank == BANK_FW ){/* FW */
		if( offset != NULL ){
			*offset = 0;
		}
		if( size != NULL ){
			*size = G_Firmware_Size;
			#if _ENABLERLE_
			/*zouchd add start*/
			if ( ((*size) & 0x80000000UL) == 0x80000000UL ) {
				*size &= (~0x80000000UL);
				isRleFile = 1;
			}
			else {
				isRleFile = 0;
			}
			/*zouchd add end*/
			#endif
		}
       		return TRUE;
	}
	/* 4 bytes idx number + 4 byte size + 2byte idx */
	temp = 10 + (temp * K_BRN_INDEX_SIZE);
	phyAddr += temp;
	phyAddr = phyAddr >> 9 << 9;
	//printf("%s(%ld) phyAddr=%lx,temp=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr,temp);
	SMC_Card2Sram(phyAddr);
	for( i = 0; i < 512; i++){
		G_ShadowData[i] = G_ucStorData[i];
	}
	SMC_Card2Sram( phyAddr + 512 );
	for( i = 512; i < 1024; i++){
		G_ShadowData[i] = G_ucStorData[i - 512];
	}
	temp = temp % 512;
	//printf( "%s: %ld,temp=%lx\n", __FILE__, (UINT32)__LINE__ ,temp);
	//dumpData(16, &G_ShadowData[temp + 0] ,0);
	if( offset != NULL ){
		WRITE8(*offset,0,G_ShadowData[temp + 0]);
		WRITE8(*offset,1,G_ShadowData[temp + 1]);
		WRITE8(*offset,2,G_ShadowData[temp + 2]);
		WRITE8(*offset,3,G_ShadowData[temp + 3]);
	}
	if( size != NULL ){
		WRITE8(*size,0,G_ShadowData[temp + 4]);
		WRITE8(*size,1,G_ShadowData[temp + 5]);
		WRITE8(*size,2,G_ShadowData[temp + 6]);
		WRITE8(*size,3,G_ShadowData[temp + 7]);

		#if _ENABLERLE_
		/*zouchd add start*/
		if ( ((*size) & 0x80000000UL) == 0x80000000UL ) {
			*size &= (~0x80000000UL);
			isRleFile = 1;
		}
		else {
			isRleFile = 0;
		}
		/*zouchd add end*/
		#endif
		
		if( *size == 0 ){
			return FALSE;
		}
	}
	if((*offset == 0xffffffff) || (*size == 0xffffffff)) {//mantis-26878
		return FALSE;
	}
	//printf("%s(%ld) idx=%lx,offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,idx,*offset,*size);
	#if 0
	//printf("%s(%ld) idx=%lx,offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,idx,*offset,*size);
	printf("A m..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAMap[temp]);

	}
	printf("\n");
	printf("A f..");
	for( temp = 0; temp < rsvRawA ; temp ++ ){
		printf("[%x]",rsvAFreeMap[temp]);
	}
	printf("\n");
	printf("B m..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBMap[temp]);

	}
	printf("\n");
	printf("B f..");
	for( temp = 0; temp < rsvRawB ; temp ++ ){
		printf("[%x]",rsvBFreeMap[temp]);
	}
	printf("\n");
	#endif
       	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandDetect                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 nandDetect(void)
/*++

Routine	Description:

		main routine

Arguments:

		none

Return Value:

		none

--*/
{
	UINT8 ret = TRUE,sts = TRUE;
	//UINT16 k;
	UINT32 headerSize = (UINT32)sizeof(nandRsvHeader_t);
	UINT32 phyAddr = 0,nrPage;
	//UINT32 ramAddr = G_Dram_Buf_Addr;/* eric Temp setting */
	UINT32 fwStartPage = 0, nrFwPages = 0;
	//DBG_PRINT("nandDetect\n");
	ret = SMC_CardDetect();
	////DBG_PRINT("%bx%bx%bx%bx\n",G_NANDF_Maker_Byte,G_NANDF_2RD_Byte,G_NANDF_3RD_Byte,G_NANDF_4TH_Byte);
	////DBG_PRINT("%bx%bx%bx%bx\n",G_NANDF_Maker_Byte,G_NANDF_2RD_Byte,G_NANDF_3RD_Byte,G_NANDF_4TH_Byte);

	/* Mount methods to this device */
	if ( ret == TRUE ) {
		phyAddr = 0;
		/* Read reserved block's information */
		/* phyaddr is bytes with sector alignment */
		ret = SMC_SearchValidBlock(&phyAddr);
		G_SMC_CisBlockAddress = phyAddr;
		#if 0
		printf("Valid Block = %lx\n",G_SMC_CisBlockAddress);
		#endif
		if ( ret == TRUE ) {
			/* Read parameter setting from nand to ASIC */
			//phyAddr = phyAddr * G_NANDF_BlockSize;
			nrPage = (sizeof(nandRsvHeader_t) + 511) >> 9;
			#if 0
			printf("Hdr nrPage = %lx\n",nrPage);
			#endif
			/* Read header size data from valod block */
			#if 0
			for (k = 0; k < nrPage; k++)
			{
				SDRAM_CardSetStartAddress(ramAddr, TRUE);
				SMC_Card2Dram(phyAddr, ramAddr);
				phyAddr += 512L;
				ramAddr += 256L;
			}
			#endif
			if ( nandRsvBlkParse() != TRUE ) {
				ret = FALSE;
				return ret;
			}
			else{
				ret = TRUE;
			}
		}
		else {
			/* Valid block is equal to total block */
			return FALSE;
		}
	}
	else {
		//DBG_PRINT("Id Fail\n");
		return FALSE;
	}
	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: nandRsvBlkParse                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
nandRsvBlkParse(
	void
)
{
	//UINT32 sum;
	UINT32 i,phyAddr;
	UINT8 random;
	UINT16 rsvBlkOffset;  /* 0xa  ~ 0xb  */
	UINT16 nrRsvBlks;     /* 0xc  ~ 0xd  */
	UINT16 checkSum;      /* 0xe  ~ 0xf  */
	UINT16 blk,tmp,j, raw,cnt;
	#if 0
	UINT32 j,k;
	/* read back //DBG_PRINT */
	SDRAM_CardSetStartAddress(G_Dram_Buf_Addr, TRUE);
	for (k = 0; k < 32; k++)
	{
		for (j = 0; j < 8; j++)
		{
			M_DRAM_ReadWord(G_ucStorData[0],G_ucStorData[1]);
			//DBG_PRINT("%bx %bx ",G_ucStorData[0],G_ucStorData[1]);
		}
		//DBG_PRINT("\n");
	}
	#endif
	/* read header marker */
	#if 1
	phyAddr = G_SMC_CisBlockAddress;
	//printf("1 phyAddr  0x%lx\n",phyAddr);
	SMC_Card2Sram(phyAddr);
	#else
	SDRAM_CardSetStartAddress(G_Dram_Buf_Addr, TRUE);
	for (i = 0; i < 512; i+=2)
	{
		M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
	}
	#endif
	if (( G_ucStorData[0] != 0x4e ) ||( G_ucStorData[1] != 0x41 ) ||( G_ucStorData[2] != 0x4e ) ||( G_ucStorData[3] != 0x44 ) || \
		( G_ucStorData[4] != 0x20 ) ||( G_ucStorData[5] != 0x52 ) ||( G_ucStorData[6] != 0x53 ) ||( G_ucStorData[7] != 0x56 )) {
		//printf("RSV header Fail\n");
		return FALSE;
	}
	/* read signature marker */
	if (( G_ucStorData[8] != 0xef ) && ( G_ucStorData[9] != 0xbe )) {
		//printf("signature Fail\n");
		return FALSE;
	}
	/* read rsvBlkOffset */
	rsvBlkOffset = (UINT16)G_ucStorData[0xa] | (UINT16)G_ucStorData[0xb] << 8;
	/* read nrRsvBlks */
	nrRsvBlks = (UINT16)G_ucStorData[0x0c] | (UINT16)G_ucStorData[0x0d] << 8;
	G_NAND_ReservBlockTotal = nrRsvBlks + 1/* RSV header*/;
	//printf("%s(%d),G_NAND_ReservBlockTotal=%bx\n",__FILE__,(UINT16)__LINE__, G_NAND_ReservBlockTotal);
	//printf("G_NAND_ReservBlockTotal=%bx\n",G_NAND_ReservBlockTotal);
	/* read checkSum */
	checkSum = (UINT16)G_ucStorData[0x0e] | (UINT16)G_ucStorData[0x0f] << 8;
	/* read fwStartBlk */
	WRITE8(fwStartBlock,3,G_ucStorData[0x10]);
	WRITE8(fwStartBlock,2,G_ucStorData[0x11]);
	WRITE8(fwStartBlock,1,G_ucStorData[0x12]);
	WRITE8(fwStartBlock,0,G_ucStorData[0x13]);
	/* read nrFwBlks */
	WRITE8(nrFwBlocks,3,G_ucStorData[0x14]);
	WRITE8(nrFwBlocks,2,G_ucStorData[0x15]);
	WRITE8(nrFwBlocks,1,G_ucStorData[0x16]);
	WRITE8(nrFwBlocks,0,G_ucStorData[0x17]);
	/* read nrRsvA */
	WRITE8(rsvRawA,3,G_ucStorData[0x24]);
	WRITE8(rsvRawA,2,G_ucStorData[0x25]);
	WRITE8(rsvRawA,1,0);
	WRITE8(rsvRawA,0,0);
	/* read nrRsvB */
	WRITE8(rsvRawB,3,G_ucStorData[0x26]);
	WRITE8(rsvRawB,2,G_ucStorData[0x27]);
	WRITE8(rsvRawB,1,0);
	WRITE8(rsvRawB,0,0);
	/* read nrRsvC */
	/* read sizeFw1 */
	#if 1
	WRITE8(G_Firmware_Size,3,G_ucStorData[0x2c]);
	WRITE8(G_Firmware_Size,2,G_ucStorData[0x2d]);
	WRITE8(G_Firmware_Size,1,G_ucStorData[0x2e]);
	WRITE8(G_Firmware_Size,0,G_ucStorData[0x2f]);
	#endif
	/* read size rawA */
	WRITE8(rsvASizeByte,3,G_ucStorData[0x34]);
	WRITE8(rsvASizeByte,2,G_ucStorData[0x35]);
	WRITE8(rsvASizeByte,1,G_ucStorData[0x36]);
	WRITE8(rsvASizeByte,0,G_ucStorData[0x37]);
	/* read size rawB */
	WRITE8(rsvBSizeByte,3,G_ucStorData[0x38]);
	WRITE8(rsvBSizeByte,2,G_ucStorData[0x39]);
	WRITE8(rsvBSizeByte,1,G_ucStorData[0x3a]);
	WRITE8(rsvBSizeByte,0,G_ucStorData[0x3b]);
	#if 0
	printf("rsvBlkOffset 0x%x\n",rsvBlkOffset);
	printf("nrRsvBlks    0x%x\n",nrRsvBlks);
	printf("checkSum     0x%x\n",checkSum);
	printf("fwStartBlk   0x%lx\n",fwStartBlock);
	printf("nrFwBlocks   0x%lx\n",nrFwBlocks);
	printf("rsvRawA      0x%lx\n",rsvRawA);
	printf("rsvRawB      0x%lx\n",rsvRawB);
	printf("rsvRawAbyte  0x%lx\n",rsvASizeByte);
	printf("rsvRawAbyte  0x%lx\n",rsvBSizeByte);
	#endif
	#if 0
	sum = (UINT32)nrRsvBlks + (UINT32)checkSum;
	sum = sum << 16;
	if ( sum != 0 ) {
		//DBG_PRINT("checkSum fail 0x%x\n",sum);
		/*return FALSE;*/
	}
	#endif
	if( NR_RSV_A < rsvRawA ){
		major_printf("Error %s(%ld), NR_RSV_A=%lx, rsvRawA=%lx\n",__FILE__,(UINT32)__LINE__,(UINT32)NR_RSV_A,(UINT32)rsvRawA);
		while(1);
		//return FALSE;
	}
	if( NR_RSV_B < rsvRawB ){
		major_printf("Error %s(%ld), NR_RSV_B=%lx, rsvRawB=%lx\n",__FILE__,(UINT32)__LINE__,(UINT32)NR_RSV_B,(UINT32)rsvRawB);
		while(1);
		//return FALSE;
	}
	#if 1
	phyAddr = G_SMC_CisBlockAddress + SIZE_BEFORE_MAPPING;
	for( i = 0; i < 8 ; i++){
		//printf("2 phyAddr  0x%lx\n",phyAddr);
		SMC_Card2Sram(phyAddr);
		phyAddr += 512L;
		memcpy(&G_ShadowData[i*512], &G_ucStorData[0], 512);
	}
	#else
	SDRAM_CardSetStartAddress(G_Dram_Buf_Addr + (SIZE_BEFORE_MAPPING >> 1), TRUE);
	#endif
	for( i = 0; i < NR_RSV_A ; i ++ ){
		rsvAMap[i] = 0xffff;
		rsvAFreeMap[i] = 0xffff;
	}
	for( i = 0; i < NR_RSV_B ; i ++ ){
		rsvBMap[i] = 0xffff;
		rsvBFreeMap[i] = 0xffff;
	}
	freeBlkIndexA = 0;
	freeBlkIndexB = 0;
	cnt = 0;
	for( j = 0; j < 2 ; j++ ){
		/* raw A ( factory area ) use fixed block mapping */
		HAL_GlobalReadRTCData(0xA0, &random);
		//printf("random=%bx\n",random);
		if( j == 0 ){
			random = random % rsvRawA;
			raw = rsvRawA;
		}
		else{
			random = random % rsvRawB;
			raw = rsvRawB;
		}
		for( i = 0; i < raw ; i ++ ){
			//M_DRAM_ReadWord(G_ShadowData[0],G_ShadowData[1]);
			//WRITE8(rsvAMap[i],1, G_ucStorData[0]);
			//WRITE8(rsvAMap[i],0, G_ucStorData[1]);
			WRITE8(blk,1, G_ShadowData[cnt]);
			cnt++;
			WRITE8(blk,0, G_ShadowData[cnt]);
			cnt++;
			tmp = blk;
			//printf("blk=%x\n",blk);
			/* Only make free table for rsv B  */
			phyAddr = blk * G_NANDF_BlockSize;
			SMC_Card2SramForExt(phyAddr);
			//dumpData(16, &G_ucStorData[0x200] ,0);
		        //Block Address Field-1 and Block Address Field-2
			WRITE8(blk,1, G_ucStorData[K_Card_PageSize +  7]);
			WRITE8(blk,0, G_ucStorData[K_Card_PageSize +  6]);
			if(( G_ucStorData[K_Card_PageSize +  6] != G_ucStorData[K_Card_PageSize + 11] ) || \
		        	(G_ucStorData[K_Card_PageSize +  7] != G_ucStorData[K_Card_PageSize + 12])){
				major_printf("Error %s(%ld), %lx\n",__FILE__,(UINT32)__LINE__,(UINT32)blk);
				//return FALSE;
			}
			//printf("blk=%x\n",blk);
			if( blk != 0xffff ){/* free block */
				if( j == 0 ){
					rsvAMap[blk] = tmp;
				}
				else{
				 	rsvBMap[blk] = tmp;
				}
			}
			else{
				if( j == 0 ){
					if( random >= rsvRawA ){
						random = 0;
					}
					rsvAFreeMap[random] = tmp;
					random++;
				}
				else{
					if( random >= rsvRawB ){
						random = 0;
					}
					rsvBFreeMap[random] = tmp;
					random++;
				}
			}
		}
	}
	#if 0
	for( i = 0; i < rsvRawA ; i ++ ){
		printf("A map=[%x]\n",rsvAMap[i]);

	}
	for( i = 0; i < rsvRawA ; i ++ ){
		printf("Af map=[%x]\n",rsvAFreeMap[i]);

	}
	for( i = 0; i < rsvRawB ; i ++ ){
		printf("B map=[%x]\n",rsvBMap[i]);

	}
	for( i = 0; i < rsvRawB ; i ++ ){
		printf("Bf map=[%x]\n",rsvBFreeMap[i]);
	}
	#endif


	diskPhySectors = G_Card_TotalBank * K_SMC_LogicalBlockPerBank - G_NAND_ReservBlockTotal - NAND_BANK0_SWAP_BLK;
	diskPhySectors = diskPhySectors * G_Card_PagePerBlock * (G_NANDF_PageSize >> 9);
#if (DOS_RSV_PART == 1)
    //printf("old: %lx\n", (UINT32)diskPhySectors);
    if (DOS_RsvPartStsGet()) {
        if (DOS_RsvPartActGet()) {
            //DOS_PartActOffsetSet(diskPhySectors - DOS_RsvPartSizeGet());
            diskPhySectors = DOS_RsvPartSizeGet();
        } else {
            if (diskPhySectors > DOS_RsvPartSizeGet()) {
                DOS_PartActOffsetSet(DOS_RsvPartSizeGet());
                diskPhySectors -= DOS_RsvPartSizeGet();
            } else {
                /* the size of reserved partition is larger than the nand physical total size */
                ASSERT(0, 0);
            }
        }
    }
    //printf("new: %lx\n", (UINT32)diskPhySectors);
#endif

	#if NAND_RSV_DATA_AT_END == 0
	G_SMC_CisBlockAddress = (G_NAND_ReservBlockTotal - 1) * G_NANDF_BlockSize;
	#endif
	#if 0
	printf("diskPhySectors=[%lu]\n",diskPhySectors);
	printf("G_Card_TotalBank=[%bu][%lu][%u]\n",G_Card_TotalBank, (UINT32)K_SMC_LogicalBlockPerBank,G_NAND_ReservBlockTotal);
	printf("G_Card_PagePerBlock=[%u]\n",G_Card_PagePerBlock);
	printf("G_SMC_CisBlockAddress=[%lx]\n",G_SMC_CisBlockAddress);
	#endif
	return TRUE;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: nandRsvBlkParse                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
flashLoadDramPara(
	UINT32 ramAddr
)
{
	UINT32 phyAddr;
	phyAddr = 0;
	/* Read reserved block's information */
	/* phyaddr is bytes with sector alignment */
	SMC_SearchValidBlock(&phyAddr);
	//Dram para at offset 512 bytes
	SDRAM_CardSetStartAddress(ramAddr, TRUE);
	SMC_Card2Dram(phyAddr + 512, ramAddr);
	//dumpData(512, ramAddr, 1);
}

UINT8 codeWriteBack(UINT32 bank,UINT32 offset,UINT32 addrw,UINT32 size){
	return FALSE;
	bank = bank;
	offset = offset;
	addrw = addrw;
	size = size;
}
#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kCardFirmwareUpdate                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
sp1kCardFirmwareUpdate(
	UINT8 *name
)
{
	//UINT16 fispHandle;
	UINT8 sts = TRUE;

	printf("FW file is %s\n",name);
	//printf("dir1cluster=%x\n",G_USR_Dir1Cluster);
	#if 0
	DOS_SetCurrDir(G_DOS_RootDirAddr);
	fispHandle = vfsFileOpen(name, K_DOS_ReadFile, 0);
	//printf("read file=%lx\n",vfsFileSizeGet());
	*size =	vfsFileSizeGet(fispHandle);
	if (fispHandle != 0)
	{
		sts	= vfsFileRead(fispHandle, K_DOS_DstDram, vfsFileSizeGet(), startAddr, 0);

		if(!sts)
		{
			printf("read fail..\n");
		}
		vfsFileClose(fispHandle, name, K_DOS_ReadFile, 0);
	}
	else
	{
		printf("%s not Found",name);
		//HAL_Wait(5000);
		//File_CloseErr(fispHandle, K_DOS_ReadFile);
	}
	#endif

	return sts;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kCardFirmwareUpdate                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
sp1kCardFirmwareBoot(
	UINT8 *name
)
{
	UINT8 sts = TRUE;
	printf("Boot file is %s\n",name);
	return sts;
}
#endif
#endif //DISK_DRV_NAND
