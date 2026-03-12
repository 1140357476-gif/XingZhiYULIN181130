
//=============================================================================
//Header file
//=============================================================================
#include <stdlib.h>
#include "general.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_spi.h"
#ifdef DISK_DRV_SPI
#include "asicreg.h"
//#include "cardcon.h"
#include "sdramhw.h"
#include "initio.h"
#include "cardlink.h"
//#include "spifunc.h"
#include "main.h"
//#include "uiflow.h"

#include "smcfunc.h"
#include "smccon.h"
#include "smcvar.h"
#include "smcdata.h"

#include "spi.h"
#include "sf.h"
#include "sf_rw.h"

#include "dos32.h"
#include "dosvar.h"
#include "reg_def.h"

#message "Load disk driver SPI"

//=============================================================================
//Define
//=============================================================================
#if 0
#define smcprint printf
#else
#define smcprint
#endif

#define	DEBUG	0
#define SIZE_BEFORE_MAPPING       4096
#define K_START_OFFSET       0
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
#define NR_RSV_A                  1024//256
#define NR_RSV_B                  256
#define RAW_A_MAXIDX            0x800//0x80
#define RAW_B_MAXIDX            0x1000//0x100
#define FW_INDEX                0x1000//0x100
#if 1
#define major_printf printf
#else
#define majors_printf;/K_SLASH
#endif

/* For compatible with SPCA5000 serials */
typedef struct spiRsvHeader_s {
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
	UINT32 blkSize;       /* 0x3c ~ 0x3f */
	UINT32 spiId;         /* 0x40 ~ 0x43 */
	UINT8  bufBlkNum;     /* 0x44 ~ 0x44 */
	UINT8  eraseCmd;      /* 0x45 ~ 0x45 */
	UINT32 tolblk;        /* 0x46 ~ 0x49 */
	UINT8  rsv0[0xfb2];
	UINT32 rsvVer;        /* 0x4092 ~ 0x4095 */
	#else
	UINT8  rsv0[4096 - 0x2a];
	#endif
	UINT16 mapTable[1024]; /* mapping of reserved blocks */
} spiRsvHeader_t;

typedef struct spiId_s {
	UINT32 id;
	UINT32 blkSize;
	UINT32 tolSize;
	UINT8  bitBlkSize;
	UINT8  bufBlk;
	UINT8  eraseCmd;
} spiId_t;

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
data	UINT32   rawBIdxNum;
xdata	UINT32   rsvASizeByte;
xdata	UINT32   rsvBSizeByte;
xdata	UINT16   rsvAFisrtBlk;
xdata	UINT16   rsvBFisrtBlk;
xdata	UINT32   freeBlkIndexA;
xdata	UINT32   freeBlkIndexB;
xdata	UINT16   rsvAMap[NR_RSV_A];
//xdata	UINT16   rsvAFreeMap[NR_RSV_A];
xdata	UINT16   rsvBMap[NR_RSV_B];
//xdata	UINT16   rsvBFreeMap[NR_RSV_B];
xdata	UINT8    G_Firmware_Update;
extern xdata	UINT8    G_ShadowData[];
extern xdata   UINT16   G_Card_BlockSize;
extern xdata   UINT8    G_Card_BlockSizeShift;

xdata    UINT32   diskPhySectors;
xdata   UINT32    G_SPI_CisBlockAddress;
xdata	UINT32   spiHeaderSize = 0;
xdata	UINT8    G_Buf_Blk_Index = 0;
xdata   spiId_t   spiDev;
xdata	UINT8    G_Spi_Erase = 1;
static UINT8 xdata sDiskType = (UINT8)DEV_TYPE_DISK_SPI;
#if _ENABLERLE_
xdata	UINT8    isRleFile = 0;/*zouchd add for rle function*/
#endif
xdata UINT8 G_ShadowData[0x1000]	_at_ 0xF000;

static UINT8 (code *pspiWPSet)(void *ptr);

#if SUPPORT_SPI_WRITE_CONTROL
// 0 : no write
// 1 : write
UINT8 mastWriteSpiFlag = 0;
#endif
//=============================================================================
//Extern Reference
//=============================================================================

//=============================================================================
//Program
//=============================================================================
UINT8 spiWriteLbaToPba(UINT32 logAddr, UINT32* oldPhyAddr, UINT32* newPhyAddr, UINT16* blockNum);
extern void sst26UnlockAll(UINT8 len);

//spi set WP call back function
 void
 sfCallbackReg(
 	UINT32 idx,
 	void *pfunct
 )
{
	switch(idx)
	{
		case SP1K_SPI_CALLBACK_WP_SET:
			pspiWPSet = pfunct;
			break;

		default:
			break;
	}
	//printf("spi set WP reg\n");
}

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
	return spiRsvRead(idx, addr);
}

UINT8
sp1kDiskRsvWrite(
	UINT32 idx,
	UINT32 addr
)
{
	return spiRsvWrite(idx, addr);
}

UINT8
sp1kDiskRsvSizeGet(
	UINT32 idx,
	UINT32 *offset,
	UINT32 *size
)
{
	return spiRsvSizeGet(idx, offset, size);
}

void sp1kFlashCacheCtrl(UINT8 on)
{
	on = on;
}

UINT8 flashCacheCtrlSts(void)
{
	return 0;
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
#include "dbg_def.h"
UINT8
spiRsvRead(
	UINT32 idx,
	UINT32 addrDecByte
)
{
	UINT32 offset, size, k, offsetAlign, temp, blkidx,tmpSize;
	UINT32 alignOffset, nrPage, curBlk, rdSize;
	UINT8 ret, idxbank = 0;
	UINT32 phyAddr = 0;
#if _ENABLERLE_
	UINT32 addr = sp1kRsvGetReadBuf();
#else
	UINT32 addr = addrDecByte;
#endif
	UINT32 tmp_addr = addr;
	UINT8 status;

	ENTER_CRITICAL( status );
//dbgTimerStart();
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	//printf("%s(%ld) idx=%lx,addr =%lx\n",__FILE__,(UINT32)__LINE__,idx,addr);
	if( addr == 0 ){
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		EXIT_CRITICAL( status );
		return FALSE;
	}
	ret = spiRsvSizeGet(idx, &offset, &size);
	//printf("%s(%ld) idx=%lx,offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,idx,offset,size);
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
	nrPage = (spiDev.blkSize >> 9);
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
	else if( idxbank == BANK_FW ){
		phyAddr = (K_START_OFFSET + (UINT32)sizeof(spiRsvHeader_t)) >> 9;
		sfPhySectorR(phyAddr, G_Firmware_Size >> 9 , 512 ,tmp_addr >> 1,K_SPI2DRAM);
		EXIT_CRITICAL( status );
		return TRUE;
	}

	if(((tmp_addr>>1) > sysMemTotSizeGet()) ||
		(((tmp_addr + size)>>1) > sysMemTotSizeGet())){
		printf("rsv:%lu, %lx, %lu\n", idx, tmp_addr, size);
		EXIT_CRITICAL( status );
		return FALSE;
	}

	phyAddr = curBlk * spiDev.blkSize;
	//printf("0 blk=0x%lx,phyAddr=%lx\n",curBlk,phyAddr);
	//dumpData(16, &G_ucStorData[0x200] ,0);
	phyAddr = phyAddr + (((offset >> 9) << 9) - (temp * spiDev.blkSize));
	//printf("phyAddr=%lx,%lx\n",phyAddr,((offset >> 9) << 9));
	rdSize = 0;
	while( rdSize < size ){
	//printf("rdSize=%lx,size =%lx\n",rdSize,size);
		if( offsetAlign ){/* not 512 byte alignment */
			//printf("00 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
 			sfPhySectorR(phyAddr >> 9, 1 , 512 ,0, 0x0000);
			//SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), 512L);
			if((512L-offsetAlign)>size){
				SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), size);
				tmpSize = size;
			}else{
				SDRAM_Sram2Dram(&G_ucStorData[offsetAlign], (addr >> 1), (512L-offsetAlign));
				tmpSize = 512L-offsetAlign;
			}
			phyAddr += 512L;
			addr = addr + tmpSize;//( 512L - offsetAlign );
			rdSize += tmpSize;//( 512L - offsetAlign );
			if( rdSize >= size ){
				break;
			}
			temp = (phyAddr - (curBlk * spiDev.blkSize)) >> 9;
			//printf("0 phyAddr=%lx,temp =%lx\n",phyAddr,temp);
			for (k = temp; k < nrPage; k++) {
				if( rdSize >= size ){
					break;
				}
				if ( rdSize + 512UL > size ) {
					temp = (size - rdSize);
					sfPhySectorR(phyAddr >> 9, 1 , 512 ,0, 0x0000);
					SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), temp);
					phyAddr += temp;
					addr	+= temp;
					rdSize += temp;
				} else {
					//printf("0 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
					sfPhySectorR(phyAddr >> 9, 1 , 512 ,(addr >> 1), K_SPI2DRAM);
					phyAddr += 512L;
					addr    += 512L;
					rdSize += 512L;
				}
			}
		}
		else{/* Is 512 byte alignment */
			if (size <= 0x200){
				//printf("5 phyAddr=%lx,addr=%lx, rdSize=%lx,curBlk=%lx,nrPage=%lx\n",phyAddr, addr, rdSize,curBlk,nrPage);
				sfPhySectorR(phyAddr >> 9, 1 , 512 ,0, 0x0000);
				SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), (size - rdSize));
				break;
			}
			else
			{
				temp = (phyAddr - (curBlk * spiDev.blkSize)) >> 9;
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
						sfPhySectorR(phyAddr >> 9, 1 , 512 ,0, 0x0000);
						SDRAM_Sram2Dram(&G_ucStorData[0], (addr >> 1), temp);
						phyAddr += temp;
						addr	+= temp;
						rdSize += temp;
					} else {
						sfPhySectorR(phyAddr >> 9, 1 , 512 ,(addr >> 1),K_SPI2DRAM);
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
			curBlk = G_ucStorData[blkidx*2 + fwStartBlock*2] | G_ucStorData[blkidx*2 + fwStartBlock*2 + 1] << 8;
			//printf("blk=0x%lx,idxbank=0x%bx\n",curBlk,idxbank);
		}
		phyAddr = curBlk * spiDev.blkSize;
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
 *  Function Name: spiFreeBlkReq                                         *
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
spiFreeBlkReq(
	UINT32 idx,
	UINT32 num,
	UINT16 *phy,
	UINT32 bank
)
{
#if 1 //Use the NR_OF_BSWAP_SPI for bufferring
	UINT32 temp = rsvRawB - spiDev.bufBlk;

	#if SUPPORT_SPI_WRITE_CONTROL
	if (mastWriteSpiFlag) {
	#endif

		if( G_Buf_Blk_Index >= spiDev.bufBlk ){//Reserve Block as bufferring
			G_Buf_Blk_Index = 0;
		}
		if( bank == BANK_A ){
			*phy = rsvBMap[temp + G_Buf_Blk_Index];
			//printf("A  *phy=%x, %lx, %lx, %bx\n",*phy,rsvRawA,rsvRawB,G_Buf_Blk_Index);
		}
		else if( bank == BANK_B ){
			*phy = rsvBMap[temp + G_Buf_Blk_Index];
			//printf("B  *phy=%x, %lx, %lx, %bx\n",*phy,rsvRawA,rsvRawB,G_Buf_Blk_Index);
		}
		G_Buf_Blk_Index ++;
		temp = (UINT32)*phy;
		temp <<= spiDev.bitBlkSize;
		//spiEraseSector(temp, spiDev.blkSize, 0);
		sfSectorErase(temp);
		return 1;
		idx = idx;
		num = num;

	#if SUPPORT_SPI_WRITE_CONTROL
	}
	#endif
#else
	UINT16 blk;//,temp;
	UINT32 i, sizeLimit,freeBlkIndex;//, temp;
	num = num;
	//printf("spiFreeBlkReq bank=%lx\n",bank);
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
						//SMC_EraseOneBlock(rsvAMap[idx] * spiDev.blkSize);
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
						//SMC_EraseOneBlock(rsvBMap[idx] * spiDev.blkSize);
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
#endif
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
spiRawWrite(
	UINT32 addr,
	UINT32 offset,
	UINT32 size,
	UINT32 bank
)
{
	UINT32 nrPage, nrBlk, num, i, erase;
	UINT32 blkidx = 0;
	UINT32 srcPhyAddr = 0, dstPhyAddr;
	UINT32 tmp_addr = addr;
	UINT32 tmpSrc,tmpdst;
	UINT32 oldSize0, oldSize1, newSize0, newSize1, newSize2, tmpEraseSize;
	UINT16 phyblk;

	#if SUPPORT_SPI_WRITE_CONTROL
	if (mastWriteSpiFlag) {
	#endif

		/*if( pspiWPSet != NULL ){
			sts = (*pspiWPSet)(&mode);
			printf("UNLOCK(%bx)\n",sts);
		}*/

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
	#if DEBUG
	printf("addr=%lx,offset=%lx,size=%lx,bank=%lx\n",addr,offset,size,bank);
	#endif
	oldSize0 = offset % spiDev.blkSize;
	nrBlk = ((oldSize0 + size + spiDev.blkSize - 1) / spiDev.blkSize);
	if( nrBlk == 1 ){
		newSize0 = size;
		oldSize1 = spiDev.blkSize - oldSize0 - newSize0;
	}
	else if( nrBlk == 2 ){
		newSize0 = spiDev.blkSize - oldSize0;
		newSize2 = size - newSize0;
		oldSize1 = spiDev.blkSize - newSize2;
	}
	else{
		newSize0 = spiDev.blkSize - oldSize0;
		newSize1 = (( size - newSize0 ) / spiDev.blkSize ) * spiDev.blkSize;
		newSize2 = size - newSize0 - newSize1;
		oldSize1 = spiDev.blkSize - newSize2;
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
	nrPage = (spiDev.blkSize >> 9);
	blkidx = (offset >> 9) / (spiDev.blkSize >> 9);
	if( bank == BANK_A ){
		srcPhyAddr = (UINT32)rsvAMap[blkidx] * spiDev.blkSize;
	}
	else if( bank == BANK_B ){
		srcPhyAddr = (UINT32)rsvBMap[blkidx] * spiDev.blkSize;
	}
	else if( bank == 2 ){
		if( G_Firmware_Update != 0x5a ){
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
		//major_printf("%s(%ld), FW Update ..\n",__FILE__,(UINT32)__LINE__);
		/* FW bank */
		dstPhyAddr = (K_START_OFFSET + (UINT32)sizeof(spiRsvHeader_t)) >> 9;

		tmpEraseSize = K_START_OFFSET + (UINT32)sizeof(spiRsvHeader_t);
		tmpEraseSize = ((G_Firmware_Size + tmpEraseSize + spiDev.blkSize - 1) >> spiDev.bitBlkSize) << spiDev.bitBlkSize;
		tmpEraseSize -= (K_START_OFFSET + (UINT32)sizeof(spiRsvHeader_t));

		spiEraseSector(K_START_OFFSET + (UINT32)sizeof(spiRsvHeader_t), tmpEraseSize, 1);/*G_Firmware_Size*/
		sfPhySectorW(dstPhyAddr, (G_Firmware_Size + 511) >> 9 , 512 , addr >> 1, K_DRAM2SPI);
		//dumpData(512, &G_ucStorData[0] ,0);
		return TRUE;
	}
	addr = addr >> 1;
	num = spiFreeBlkReq(blkidx, 1, &phyblk, bank);
	//printf("nrBlk=%lx,num=%lx,phyblk=%x\n",nrBlk,num,phyblk);
	/* write oldsize0 */
	dstPhyAddr = (UINT32)phyblk * spiDev.blkSize;
	//printf("srcPhyAddr=%lx,dstPhyAddr=%lx,phyblk=%lx\n",srcPhyAddr,dstPhyAddr,srcPhyAddr/spiDev.blkSize);
	if( nrBlk == 1 ){
		#if DEBUG
		printf("case 0 - 1 block\n");
		#endif
		erase = srcPhyAddr;
		if( oldSize0 ){
			while( oldSize0 >= 512 ){
				#if DEBUG
				printf("0 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				//dumpData(16, &G_ucStorData[0] ,0);
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 -= 512L;
			}
			/* remain of oldSize0 not 512 alignment */
			if( oldSize0 || newSize0 ){
				#if DEBUG
				printf("1 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 ,0x0000);
				//dumpData(512, &G_ucStorData[0] ,0);
				//dumpData(newSize0, addr ,1);
				M_DRAM_SetStartAddressV(addr , TRUE)
				if( newSize0 >= (512 - oldSize0)){
					for( i = oldSize0 ; i < 512 ; i = i + 2){
						M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
					}
					addr = addr + (512 - oldSize0)/2;
					newSize0 -= (512 - oldSize0);
					#if DEBUG
					printf("11 oldSize0=%lx,newSize0=%lx,oldSize1=%lx\n",oldSize0,newSize0,oldSize1);
					#endif
				}
				else{
					for( i = oldSize0 ; i < (oldSize0 + newSize0) ; i = i + 2){
						M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
					}
					addr = addr + (newSize0/2);
					oldSize1 = oldSize1 - (512 - oldSize0 - newSize0);
					newSize0 = 0;
					#if DEBUG
					printf("22 oldSize0=%lx,newSize0=%lx,oldSize1=%lx\n",oldSize0,newSize0,oldSize1);
					#endif
				}
				//dumpData(512 - oldSize0, addr ,1);
				//dumpData(512 - oldSize0, &G_ucStorData[oldSize0] ,0);
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 = 0;
			}
		}
		/* newSize0 */
		if( newSize0 ){
			while( newSize0 >= 512 ){
				#if DEBUG
				printf("6 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
				#endif
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , addr, K_DRAM2SPI);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				addr += 256L;
				newSize0 -= 512L;
			}

			/* remain of oldSize0 not 512 alignment */
			if( newSize0 || oldSize1 ){
				#if DEBUG
				printf("7 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 ,0x0000);
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
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				newSize0 = 0;
			}
		}

		/* oldSize1 */
		if( oldSize1 ){
			while( oldSize1 >= 512 ){
				#if DEBUG
				printf("8 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize1=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize1,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0xc00);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize1 -= 512L;
			}
		}

		spiEraseSector(erase , spiDev.blkSize, 0);
		//Copy from dst to original block
		tmpSrc = (UINT32)phyblk * spiDev.blkSize;
		tmpdst = erase;
		for( i = 0; i < nrPage ; i++ ){
			sfPhySectorR(tmpSrc >> 9, 1 , 512 , 0 , 0x0000);
			sfPhySectorW(tmpdst >> 9, 1 , 512 , 0 , 0x0000);
			tmpSrc += 512L;
			tmpdst += 512L;
		}
	}
	else{
		#if DEBUG
		printf("case 2 - over 1 block\n");
		#endif
		erase = srcPhyAddr;
		if( oldSize0 ){
			//erase = srcPhyAddr;
			while( oldSize0 >= 512 ){
				#if DEBUG
				printf("0 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 ,0x0000);
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize0 -= 512L;
			}
			/* remain of oldSize0 not 512 alignment */
			if( oldSize0 ){
				#if DEBUG
				printf("1 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize0,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 ,0x0000);
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
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				addr = addr + (512 - oldSize0)/2;
			}
			//spiEraseSector(erase , spiDev.blkSize, 0);
		}
		//spiEraseSector(erase , spiDev.blkSize, 0);
		/* newSize0 */
		while( newSize0 >= 512 ){
			#if DEBUG
			printf("2 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize0=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize0,addr);
			#endif
			//dumpData(512, addr ,1);
			sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , addr , K_DRAM2SPI);
			dstPhyAddr += 512L;
			addr += 256L;
			newSize0 -= 512L;
		}
		spiEraseSector(erase , spiDev.blkSize, 0);
		//Copy from dst to original block
		tmpSrc = (UINT32)phyblk * spiDev.blkSize;
		tmpdst = erase;
		for( i = 0; i < nrPage ; i++ ){
			sfPhySectorR(tmpSrc >> 9, 1 , 512 , 0 , 0x0000);
			sfPhySectorW(tmpdst >> 9, 1 , 512 , 0 , 0x0000);
			tmpSrc += 512L;
			tmpdst += 512L;
		}
		/* newSize1 */
		num = (newSize1 / spiDev.blkSize);
		while( num ){
			blkidx ++;
			num--;
			#if 0
			spiFreeBlkReq(blkidx, 1, &phyblk, bank);
			dstPhyAddr = phyblk * spiDev.blkSize;
			if( bank == BANK_A ){
				if( freeBlkIndexA != 0 ){
					tmp = freeBlkIndexA - 1;
				}
				srcPhyAddr = rsvAFreeMap[tmp] * spiDev.blkSize;
			}
			else if( bank == BANK_B ){
				if( freeBlkIndexB != 0 ){
					tmp = freeBlkIndexB - 1;
				}
				srcPhyAddr = rsvBFreeMap[tmp] * spiDev.blkSize;
			}
			#else
			if( bank == BANK_A ){
				dstPhyAddr = rsvAMap[blkidx] * spiDev.blkSize;
			}
			else if( bank == BANK_B ){
				dstPhyAddr = rsvBMap[blkidx] * spiDev.blkSize;
			}
			#endif
			spiEraseSector(dstPhyAddr, spiDev.blkSize, 0);
			//printf("dstPhyAddr=%lx,blkidx=%lx\n",dstPhyAddr,blkidx);
			for( i = 0; i < nrPage ; i ++ ){
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , addr , K_DRAM2SPI);
				dstPhyAddr += 512L;
				srcPhyAddr += 512L;
				addr += 256L;
				#if DEBUG
				printf("3 srcPhyAddr=%lx,dstPhyAddr=%lx,%lx\n",srcPhyAddr,dstPhyAddr,addr);
				#endif
			}
			newSize1 -= spiDev.blkSize;
		}
		/* newSize2 */
		if( newSize2 == 0 ){
			return TRUE;
		}
		if( newSize2 ){
			blkidx ++;
			if( bank == BANK_A ){
				srcPhyAddr = rsvAMap[blkidx] * spiDev.blkSize;
			}
			else if( bank == BANK_B ){
				srcPhyAddr = rsvBMap[blkidx] * spiDev.blkSize;
			}
			num = spiFreeBlkReq(blkidx,1, &phyblk, bank);
			dstPhyAddr = phyblk * spiDev.blkSize;
		}
		while( newSize2 >= 512 ){
			#if DEBUG
			printf("4 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize2=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize2,addr);
			#endif
			sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , addr , K_DRAM2SPI);
			srcPhyAddr += 512L;
			dstPhyAddr += 512L;
			newSize2 -= 512L;
			addr += 256L;
		}
		erase = ( srcPhyAddr / spiDev.blkSize )  * spiDev.blkSize;
		/* remain of newSize2 not 512 alignment */
		if( newSize2 ){
			#if DEBUG
			printf("5 srcPhyAddr=%lx,dstPhyAddr=%lx,newSize2=%lx,%lx\n",srcPhyAddr,dstPhyAddr,newSize2,addr);
			#endif
			sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 ,0x0000);
			//dumpData(512, &G_ucStorData[0] ,0);
			M_DRAM_SetStartAddressV(addr , TRUE)
			for( i = 0 ; i < newSize2 ; i = i + 2){
				M_DRAM_ReadWord(G_ucStorData[i],G_ucStorData[i+1]);
			}
			//dumpData(512, &G_ucStorData[0] ,0);
			sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
			srcPhyAddr += 512L;
			dstPhyAddr += 512L;
			addr = addr + (newSize2 / 2);
			//printf("52 oldSize1=%lx,newSize2=%lx,%lx\n",oldSize1,newSize2);
			if( oldSize1 >= ( 512 - newSize2 )){
				oldSize1 = oldSize1 - ( 512 - newSize2 );
				//printf("53 oldSize1=%lx,newSize2=%lx,%lx\n",oldSize1,512 - newSize2);
				newSize2 = 0;
			}
		}
		/* oldSize1 */
		if( oldSize1 ){
			while( oldSize1 >= 512 ){
				#if DEBUG
				printf("6 srcPhyAddr=%lx,dstPhyAddr=%lx,oldSize1=%lx,%lx\n",srcPhyAddr,dstPhyAddr,oldSize1,addr);
				#endif
				sfPhySectorR(srcPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				sfPhySectorW(dstPhyAddr >> 9, 1 , 512 , 0 , 0x0000);
				srcPhyAddr += 512L;
				dstPhyAddr += 512L;
				oldSize1 -= 512L;
			}
		}
		spiEraseSector(erase , spiDev.blkSize, 0);
		//Copy from dst to original block
		tmpSrc = (UINT32)phyblk * spiDev.blkSize;
		tmpdst = erase;
		for( i = 0; i < nrPage ; i++ ){
			sfPhySectorR(tmpSrc >> 9, 1 , 512 , 0 , 0x0000);
			sfPhySectorW(tmpdst >> 9, 1 , 512 , 0 , 0x0000);
			tmpSrc += 512L;
			tmpdst += 512L;
		}
	}
	return TRUE;
	#if SUPPORT_SPI_WRITE_CONTROL
	}
	#endif
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
spiRsvWrite(
	UINT32 idx,
	UINT32 addr
)
{
	UINT32 offset, size;
	UINT8 ret, idxbank = 0;
	UINT8 sts,mode = SPI_STS_UNLOCK;//SPI_STS_LOCK_ALL
#if SUPPORT_SPI_WRITE_CONTROL
if (mastWriteSpiFlag) {
#endif
#if !TAG_TODO
	//return TRUE;//only for spi write err
#endif
	if( pspiWPSet != NULL ){
		sts = (*pspiWPSet)(&mode);
		mode = SPI_STS_LOCK_ALL;
		////printf("UNLOCK(%bx)\n",sts);
	}

	HAL_FlashMode(K_MEDIA_Spi,0,0);
	//printf("%s(%ld) idx=%lx,addr =%lx\n",__FILE__,(UINT32)__LINE__,idx,addr);
	if( addr == 0 ){
		major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		if( pspiWPSet != NULL ){
			sts = (*pspiWPSet)(&mode);
			//printf("LOCK(%bx)\n",sts);
		}
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
	ret = spiRsvSizeGet(idx, &offset, &size);
	//printf("idx %lx\n", idx);
	if(ret == FALSE){
		//major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		if( pspiWPSet != NULL ){
			sts = (*pspiWPSet)(&mode);
			//printf("LOCK(%bx)\n",sts);
		}
		return FALSE;
	}
	//printf("idxbank %bx\n", idxbank);
	ret = spiRawWrite(addr, offset, size, idxbank);

		if( pspiWPSet != NULL ){
			sts = (*pspiWPSet)(&mode);
			//printf("LOCK(%bx)\n",sts);
		}
		
	#if SUPPORT_SPI_WRITE_CONTROL
	}
	#endif
	
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: spiRsvBlkParse                                        *
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
spiRsvBlkParse(
	void
)
{
	UINT32 i,phyAddr;
	UINT16 blk,j,raw,cnt;//tmp;
	//UINT8 random;
	UINT16 rsvBlkOffset;  /* 0xa  ~ 0xb  */
	UINT16 nrRsvBlks;     /* 0xc  ~ 0xd  */
	UINT16 checkSum;      /* 0xe  ~ 0xf  */
	#if 0
	dumpData(512, (G_Dram_Buf_Addr), 1);
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
	/* Was isp by 1528 */
	//WRITE8(icMarker,1,G_ucStorData[0x2a]);
	//WRITE8(icMarker,0,G_ucStorData[0x2b]);
	/* read nrRsvC */
	/* read sizeFw1 */
	WRITE8(G_Firmware_Size,3,G_ucStorData[0x2c]);
	WRITE8(G_Firmware_Size,2,G_ucStorData[0x2d]);
	WRITE8(G_Firmware_Size,1,G_ucStorData[0x2e]);
	WRITE8(G_Firmware_Size,0,G_ucStorData[0x2f]);

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
	/* read spi sector size */
	WRITE8(spiDev.blkSize,3,G_ucStorData[0x3c]);
	WRITE8(spiDev.blkSize,2,G_ucStorData[0x3d]);
	WRITE8(spiDev.blkSize,1,G_ucStorData[0x3e]);
	WRITE8(spiDev.blkSize,0,G_ucStorData[0x3f]);
	/* read spi id */
	WRITE8(spiDev.id,3,G_ucStorData[0x40]);
	WRITE8(spiDev.id,2,G_ucStorData[0x41]);
	WRITE8(spiDev.id,1,G_ucStorData[0x42]);
	WRITE8(spiDev.id,0,G_ucStorData[0x43]);

	spiDev.bufBlk = G_ucStorData[0x44];
	spiDev.eraseCmd = G_ucStorData[0x45];
	/* read spi id */
	WRITE8(spiDev.tolSize,3,G_ucStorData[0x46]);
	WRITE8(spiDev.tolSize,2,G_ucStorData[0x47]);
	WRITE8(spiDev.tolSize,1,G_ucStorData[0x48]);
	WRITE8(spiDev.tolSize,0,G_ucStorData[0x49]);
	//get the bit number of block size
	{
		UINT32 j = spiDev.blkSize;
		while( j > 1 ){
			spiDev.bitBlkSize ++;
			j = j >> 1;
		}
		//printf("spiDev.blkSize 0x%lx,0x%bx,0x%bx\n",spiDev.blkSize, spiDev.bitBlkSize,spiDev.eraseCmd);
	}

	#if DEBUG
	printf("rsvBlkOffset 0x%x\n",rsvBlkOffset);
	printf("nrRsvBlks    0x%x\n",nrRsvBlks);
	printf("checkSum     0x%x\n",checkSum);
	printf("fwStartBlk   0x%lx\n",fwStartBlock);
	printf("nrFwBlocks   0x%lx\n",nrFwBlocks);
	printf("rsvRawA      0x%lx\n",rsvRawA);
	printf("rsvRawB      0x%lx\n",rsvRawB);
	printf("G_Firmware_Size 0x%lx\n",G_Firmware_Size);
	printf("rsvRawAbyte  0x%lx\n",rsvASizeByte);
	printf("rsvRawBbyte  0x%lx\n",rsvBSizeByte);
	#endif
	if( NR_RSV_A < rsvRawA ){
		major_printf("Error %s(%ld), NR_RSV_A=%lx, rsvRawA=%lx\n",__FILE__,(UINT32)__LINE__,(UINT32)NR_RSV_A,(UINT32)rsvRawA);
		while(1);
	}
	if( NR_RSV_B < rsvRawB ){
		major_printf("Error %s(%ld), NR_RSV_B=%lx, rsvRawB=%lx\n",__FILE__,(UINT32)__LINE__,(UINT32)NR_RSV_B,(UINT32)rsvRawB);
		while(1);
	}
	phyAddr = ( K_START_OFFSET + SIZE_BEFORE_MAPPING ) >> 9;
	for( i = 0; i < 8 ; i++){
		sfPhySectorR(phyAddr , (UINT16)1 , 512 , 0, 0x0000);
		phyAddr ++;
		memcpy(&G_ShadowData[i*512], &G_ucStorData[0], 512);
	}
	for( i = 0; i < NR_RSV_A ; i ++ ){
		rsvAMap[i] = 0xffff;
//		rsvAFreeMap[i] = 0xffff;
	}
	for( i = 0; i < NR_RSV_B ; i ++ ){
		rsvBMap[i] = 0xffff;
//		rsvBFreeMap[i] = 0xffff;
	}
	freeBlkIndexA = 0;
	freeBlkIndexB = 0;
	cnt = 0;
	#if 0
	//Here is going to deal swap method
	#else
	//Here is not using swap
	for( j = 0; j < 2 ; j++ ){
		/* raw A ( factory area ) use fixed block mapping */
		if( j == 0 ){
			raw = rsvRawA;
		}
		else{
			raw = rsvRawB;
		}
		for( i = 0; i < raw ; i ++ ){
			WRITE8(blk,1, G_ShadowData[cnt]);
			cnt++;
			WRITE8(blk,0, G_ShadowData[cnt]);
			cnt++;
			if( j == 0 ){
				rsvAMap[i] = blk;
			}
			else{
			 	rsvBMap[i] = blk;
			}
		}
	#endif
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
	diskPhySectors = spiDev.tolSize - (rsvRawA + rsvRawB) - spiDev.bufBlk - \
		((G_Firmware_Size + spiDev.blkSize - 1) >> spiDev.bitBlkSize) - \
		(((UINT32)sizeof(spiRsvHeader_t) + spiDev.blkSize - 1) >> spiDev.bitBlkSize);
	G_SPI_CisBlockAddress = spiDev.tolSize - diskPhySectors;
	diskPhySectors = (diskPhySectors * spiDev.blkSize) >> 9;
	printf("diskPhySectors 0x%lx,G_SPI_CisBlockAddress=%lx\n",diskPhySectors,G_SPI_CisBlockAddress);

	//set param...
	sfEraseCmdSet(spiDev.eraseCmd);

	return TRUE;
}
#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: spiType                                         *
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
spiType(
	UINT8 id[3]
)
{
	UINT32 i;
	for( i = 0; i < ISP_IS_MAXNUM; i ++ ){
		//printf("%bx %bx\n",id[0],K_SPI_ID[i].id);
		if( id[0] == K_SPI_ID[i].id ) break;
	}
	if( i == ISP_IS_MAXNUM ){
		printf("Unknown Device\n");
		return TRUE;
	}
	else{
		spiDev.blkSize = K_SPI_ID[i].blkSize;
	}
	printf("Maker %s\n",K_SPI_ID[i].string);
	return TRUE;
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_CardDetect                                        *
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
UINT8 SMC_CardDetect(void)
{
	UINT8 id[3], ret;
	UINT32 id32;

	spiHeaderSize = (UINT32)sizeof(spiRsvHeader_t);
	//printf("SMC_CardDetect\n");
	HAL_FlashMode(K_MEDIA_Spi,0,0);
	//HAL_SPIInit(1,0);
	sfIdRead(id);
	printf("%bx%bx%bx \n",id[0],id[1],id[2]);
	//spiType(id);
	//sfPhySectorR(0,spiHeaderSize >> 9 ,512,0, 0xc00);
#if 1//(FPGA_VERIFY != 1) //xian mask for FPGA test
	sfPhySectorR(0, 1 ,512,0, 0x0000);
	nrFwBlocks = 0;
	if( spiRsvBlkParse() != TRUE ){
		ret = FALSE;

		#if 1 //for special spi, test
		id32 = ((UINT32)id[0]<<16) | ((UINT32)id[1]<<8) |((UINT32)id[2]);
		spiDev.blkSize = (UINT32)4*1024; //default
		spiDev.bitBlkSize = 12;

		switch(id[0]){
			case SPI_MAKER_SPANSION:
			case SPI_MAKER_ST:
				spiDev.eraseCmd =  0xd8;
				break;

			default:
				spiDev.eraseCmd =  0x20;
				break;
		}

		printf("spec spi: %lx\n", id32);
		switch(id32){
			case 0xBF258E:	//SST
			case 0xBF258d:	//SST
			case 0xBF254a:	//SST

			case 0x8C2014:	//	ESMT
			case 0x8C2013:	//	ESMT
			case 0x8C2015:	//	ESMT
				g_spiModeSel.flowMode = SPI_FLOW_AAI_WRT; // only support SPI_FLOW_AAI_WRT & SPI_FLOW_BYTE_WRT
				break;

			case 0xC22014:	// MXIC,
				g_spiModeSel.flowMode = SPI_FLOW_BYTE_WRT;
				break;

			case 0x010214:	// Spansion
			case 0x202014:	// St M25P80, only PP
			case 0x202013:	// St M25P80, only PP
			case 0x202017:	// St M25P80, only PP
				spiDev.blkSize = (UINT32)64*1024;
				spiDev.bitBlkSize = 16;
				spiDev.eraseCmd =  0xd8;
				g_spiModeSel.flowMode = SPI_FLOW_PAGE_DMA;
				break;

			case 0x010215:	// Spansion 32Mbit - Q, ??? secort size 4K, 64K
				spiDev.blkSize = (UINT32)4*1024;
				spiDev.eraseCmd =  0xd8;
				spiDev.bitBlkSize = 12;
				g_spiModeSel.flowMode = SPI_FLOW_PAGE_DMA;
				break;

			case 0x1c2016:	// EON
				spiDev.blkSize = (UINT32)64*1024;
				spiDev.eraseCmd =  0xd8;
				spiDev.bitBlkSize = 12;
				g_spiModeSel.flowMode = SPI_FLOW_PAGE_DMA;
				break;

			default:
				g_spiModeSel.flowMode = SPI_FLOW_PAGE_DMA;
				break;
		}
		sfEraseCmdSet(spiDev.eraseCmd);
		#else
		return ret;
		#endif
	}
	else{ //temp for spi rsv write because SPI not ready
		printf("rsv hdr..%lu, %bu, %bx\n", spiDev.blkSize, spiDev.bitBlkSize, spiDev.eraseCmd);
		id32 = ((UINT32)id[0]<<16) | ((UINT32)id[1]<<8) |((UINT32)id[2]);

		switch(id32){
			case 0x010214:	// Spansion
				spiDev.blkSize = (UINT32)64*1024;
				spiDev.bitBlkSize = 16;
				spiDev.eraseCmd =  0xd8;
				break;

			case 0x010215:	// Spansion 32Mbit - Q, ??? secort size 4K, 64K
				spiDev.blkSize = (UINT32)4*1024;
				spiDev.eraseCmd =  0xd8;
				spiDev.bitBlkSize = 12;
				g_spiModeSel.flowMode = SPI_FLOW_PAGE_DMA;
				break;
		}
		ret = TRUE;
	}

if(id[0]== 0xBF && id[1] == 0x26 && id[2] == 0x01){//there some error
	spiDev.bitBlkSize = 12;
	//g_spiModeSel.bitMode = SPI_4BIT_MODE;
	sfSimpleCmd(0x38);/* enable Quad */
	SPI_4BIT_MODE_SET;

	sst26UnlockAll(6);

	if(g_spiModeSel.bitMode != SPI_4BIT_MODE){
		sfSimpleCmd(0xff);/* disable Quad */
		SPI_1BIT_MODE_SET;
	}
}

	G_Card_TotalSizeMB = diskPhySectors / 2048;
	G_Card_BlockSize = spiDev.blkSize;
	G_Card_BlockSizeShift = spiDev.bitBlkSize;
#else
    spiDev.blkSize = (UINT32)64*1024;
    spiDev.eraseCmd =  0x20; // erase sect 4K
#endif
printf("..%lu, %bu, %bx\n", spiDev.blkSize, spiDev.bitBlkSize, spiDev.eraseCmd);

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
spiRsvSizeGet(
	UINT32 idx,
	UINT32 *offset,
	UINT32 *size
)
{
	UINT32 temp = 0;
	UINT8 idxbank = 0;
	UINT32 phyAddr = 0;
//	UINT16 i;
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	//printf("spiHeaderSize=%lx\n",spiHeaderSize);

	if(offset)
		*offset = 0;
	if(size)
		*size = 0;

	if( spiHeaderSize == 0 ){
		if( FALSE == SMC_CardDetect()) {
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
		if(( rsvASizeByte == 0 ) && ( rsvBSizeByte == 0 ) ){
			major_printf("Error %s(%ld)\n",__FILE__,(UINT32)__LINE__);
			return FALSE;
		}
	}
	spiHeaderSize = (UINT32)sizeof(spiRsvHeader_t);
	//printf("%s(%ld) rsvB=%x,rsvA=%x\n",__FILE__,(UINT32)__LINE__,rsvAFisrtBlk,rsvBFisrtBlk);
	rsvAFisrtBlk = rsvAMap[0];
	rsvBFisrtBlk = rsvBMap[0];
	//printf("%s(%ld) rsvAFisrtBlk=%x,rsvBFisrtBlk=%x\n",__FILE__,(UINT32)__LINE__,rsvAFisrtBlk,rsvBFisrtBlk);
	if(idx < RAW_A_MAXIDX ){
		idxbank = BANK_A;/* to access raw A */
		phyAddr = (UINT32)rsvAFisrtBlk * spiDev.blkSize;
	}
	else if( idx < RAW_B_MAXIDX ){
		idxbank = BANK_B;/* to access raw B */
		phyAddr = (UINT32)rsvBFisrtBlk * spiDev.blkSize;
	}
	else if( idx == FW_INDEX ){
		idxbank = BANK_FW;/* to access FW */
	}
	else{
		major_printf("Error %s(%ld) %lu\n",__FILE__,(UINT32)__LINE__, idx);
		return FALSE;
	}
	#if 0
	temp = 512 - (phyAddr % 512);
	phyAddr = phyAddr >> 9;
	sfPhySectorR(phyAddr, (UINT16)1 ,512 , 0, 0x0000);
	for( i = 0; i < temp; i++){
		G_ShadowData[i] = G_ucStorData[(512 - temp) + i ];
	}
	if( temp != 512 ){
		phyAddr += 1;
		//printf("%s(%ld) phyAddr=%lx,temp=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr,temp);
		sfPhySectorR(phyAddr, (UINT16)1 ,512 , 0, 0x0000);
		for( i = temp; i < 512; i++){
			G_ShadowData[temp + i] = G_ucStorData[i - temp];
		}
	}
	for( i = 0; i < 512; i++){
		G_ucStorData[i] = G_ShadowData[i];
	}
	#else
	temp = phyAddr & (512-1);
	sfPhySectorSramR(phyAddr, 0, 512, 0);
	phyAddr >>= 9;
	if (temp) {
		phyAddr++;
	}
	#endif
	#if 0
	dumpData( 512 , G_ucStorData, 0);
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
	phyAddr += (temp >> 9);
#if 0
	//printf("%s(%ld) phyAddr=%lx,temp=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr,temp);
	sfPhySectorR(phyAddr, (UINT16)1 ,512 , 0, 0x0000);
	for( i = 0; i < 512; i++){
		G_ShadowData[i] = G_ucStorData[i];
	}
	sfPhySectorR(phyAddr + 1, (UINT16)1 ,512 , 0, 0x0000);
	for( i = 512; i < 1024; i++){
		G_ShadowData[i] = G_ucStorData[i - 512];
	}
	temp = temp % 512;
#else
	sfPhySectorR(phyAddr, 2, 512, sysMemSizeGet(MEM_IDX_TEXT) + ((0xf000 - 0x4000) >> 1), 1);
	temp &= 512-1;
#endif
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
	//printf("%s(%ld) offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,*offset,*size);
	if((*offset == 0xffffffff) || (*size == 0xffffffff)) {//mantis-26878
		return FALSE;
	}
	//printf("%s(%ld) offset=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,*offset,*size);
       	return TRUE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: spiEraseSector                                         *
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
spiEraseSector(
	UINT32 phyAddr,
	UINT32 size,
	UINT8 print
)
{
	UINT32 num,z0,z1,z2,nrPage, page, i, src, dst,tmp;
	UINT32 z00;
	UINT32 eraseAddr, addr = phyAddr;
	UINT16 phyblk;

	#if SUPPORT_SPI_WRITE_CONTROL
	if (mastWriteSpiFlag) {
	#endif
	
		if( spiDev.blkSize == 0 ){
			SMC_CardDetect();
		}
		
		#if DEBUG
		printf("%s(%ld) addr=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,addr,size);
		#endif
		
		num = size / spiDev.blkSize;
		//printf("%s(%ld) num=%lx\n",__FILE__,(UINT32)__LINE__,num);

	//if we try to erase non-sector alignment data, we have to keep old data
	//and write this old data back after erase.
	/*
	  z0:the start unalign to spiDev.blkSize
	  z1:data align to spiDev.blkSize
	  z2:remain not enough spiDev.blkSize byte
	*/
	z0 = 0;
	z1 = 0;
	z2 = 0;
	//num = 0;
	nrPage = spiDev.blkSize >> 9;
	z0 = addr % spiDev.blkSize;
	if(z0){
		z00 = spiDev.blkSize - z0;
		/*if(z00 > size){
			tmpSize = z00 - size;
		}else{
			tmpSize = size - z00;
		}*/
		num ++;
	}
	else{
		z00 = 0;
	}
	/*num += tmpSize / spiDev.blkSize;*/
	if( size >= z00 ){//!!!!error  wangjg!!!!!
		//printf("%s(%ld) size=%lx,z0=%lx,spiDev.bitBlkSize=%bx\n",__FILE__,(UINT32)__LINE__,size,z0, spiDev.bitBlkSize);
		z1 = ((size - z00) >> spiDev.bitBlkSize) << spiDev.bitBlkSize;
		//z1 = (tmpSize >> spiDev.bitBlkSize) << spiDev.bitBlkSize;
		//tmpSize -= z1;
	}
	if( size >= (z00 + z1) ){
		z2 = size - z00 - z1;
		//z2 = tmpSize;
		//tmpSize = 0;
	}
	if( z2 ){
		num ++;
	}
	/*
	if( z0 || z2 ) {
		printf("Err %s(%ld) %lx,%lx,%lx\n",z0,z1,z2);
		while(1);
	}
	*/
	//printf("%s(%ld) z0=%lu,z1=%lu,z2=%lu,num=%lx\n",__FILE__,(UINT32)__LINE__,z0,z1,z2,num);
	//addr = addr >> spiDev.bitBlkSize;
	#if 1
	if( z0 ){
		//printf("Err %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		spiFreeBlkReq(0, 1, &phyblk, 0);
		page = z0 >> 9;
		src = ( addr >> spiDev.bitBlkSize ) << spiDev.bitBlkSize;
		eraseAddr = src;
		dst = (UINT32)phyblk << spiDev.bitBlkSize;
		tmp = 0;
		for( i = 0; i < page; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z0=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z0,tmp);
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
			z0 -= 512L;
			tmp ++;
		}
		//remain < 512
		if( z0 ){
			//printf("%s(%ld) src=%lx,dst=%lx,z0=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z0,tmp);
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			for( i = z0 ; i < 512 ; i = i + 2){
				G_ucStorData[i + 0] = 0xFF;
				G_ucStorData[i + 1] = 0xFF;
			}
			sfPhySectorW(dst >> 9, 1 , 512 , 0 , 0x0000);
			src += 512L;
			dst += 512L;
			z0 = 0;
			tmp ++;
		}
		for( i = 0 ; i < 512 ; i = i + 2){
			G_ucStorData[i + 0] = 0xFF;
			G_ucStorData[i + 1] = 0xFF;
		}
		for( i = tmp; i < nrPage; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z0=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z0,tmp);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
		//Erase original blk
		sfSectorErase(eraseAddr);
		//Copy from dst to original block
		src = (UINT32)phyblk * spiDev.blkSize;
		dst = ( addr >> spiDev.bitBlkSize ) << spiDev.bitBlkSize;
		for( i = 0; i < nrPage ; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z0=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z0,tmp);
			sfPhySectorR(src >> 9, 1 , 512 , 0 , 0x0000);
			sfPhySectorW(dst >> 9, 1 , 512 , 0 , 0x0000);
			src += 512L;
			dst += 512L;
		}
		eraseAddr += spiDev.blkSize;
		addr = eraseAddr;
		num --;
	}
	#endif
	while( num ){
		if(print) printf(".");
		//printf("%s(%ld) addr=%lx\n",__FILE__,(UINT32)__LINE__,addr);
//sfPhySectorR(addr, (UINT16)1 ,spiDev.blkSize, G_Free_Addr, 0);
//dumpData(spiDev.blkSize, (G_Free_Addr), 1);
		if( z2 && (num <= 1)){
			break;
		}
		sfSectorErase(addr);
//sfPhySectorR(addr, (UINT16)1 ,spiDev.blkSize, G_Free_Addr, 0);
//dumpData(spiDev.blkSize, (G_Free_Addr), 1);
		addr += spiDev.blkSize;
		num --;
		/*if( z2 ){
			if( num <= 1 ) break;
		}
		else{
			if( num == 0 ) break;
		}*/
	}
	if(print) printf("\n");
	#if 1
	if( z2 ){
		//printf("Err %s(%ld)\n",__FILE__,(UINT32)__LINE__);
		spiFreeBlkReq(0, 1, &phyblk, 0);
		for( i = 0 ; i < 512 ; i = i + 2){
			G_ucStorData[i + 0] = 0xFF;
			G_ucStorData[i + 1] = 0xFF;
		}
		page = z2 >> 9;
		src = addr;
		dst = (UINT32)phyblk << spiDev.bitBlkSize;
		tmp = 0;
		for( i = 0; i < page; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z2=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z2,tmp);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
			z2 -= 512L;
			tmp ++;
		}
		//remain < 512
		if( z2 ){
			//printf("%s(%ld) src=%lx,dst=%lx,z2=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z2,tmp);
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			for( i = 0 ; i < z2 ; i = i + 2){
				G_ucStorData[i + 0] = 0xFF;
				G_ucStorData[i + 1] = 0xFF;
			}
			sfPhySectorW(dst >> 9, 1 , 512 , 0 , 0x0000);
			src += 512L;
			dst += 512L;
			z2 = 0;
			tmp ++;
		}
		for( i = tmp; i < nrPage; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z2=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z2,tmp);
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
		//Erase original blk
		sfSectorErase(addr);
		//Copy from dst to original block
		src = (UINT32)phyblk * spiDev.blkSize;
		dst = addr;
		for( i = 0; i < nrPage ; i++ ){
			//printf("%s(%ld) src=%lx,dst=%lx,z2=%lx,tmp=%lx\n",__FILE__,(UINT32)__LINE__,src,dst,z2,tmp);
			sfPhySectorR(src >> 9, 1 , 512 , 0 , 0x0000);
			sfPhySectorW(dst >> 9, 1 , 512 , 0 , 0x0000);
			src += 512L;
			dst += 512L;
		}
		addr += spiDev.blkSize;
		num --;
	}
	#endif
	return TRUE;
	#if SUPPORT_SPI_WRITE_CONTROL
	}
	#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_HwInitialize                                         *
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
void
SMC_HwInitialize(void)
{
	HAL_FlashMode(K_MEDIA_Spi, K_MEDIA_None, 0);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_LCard2Dram                                         *
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
UINT32
spiSectorAddrRead(UINT32 logAddr)
{
	UINT32 Addr;
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	//printf("%s(%ld) logAddr=%lx,ramAddr=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,logAddr,ramAddr,size );
	SMC_ReadLpaToPpa(logAddr, &Addr);
	Addr = Addr >> 9;
	sfPhySectorR(Addr, 1, 4 , 0 , 0x0000);
	//dumpData(32, G_ucStorData, 0);
	WRITE8(Addr, 3,G_ucStorData[0]);
	WRITE8(Addr, 2,G_ucStorData[1]);
	WRITE8(Addr, 1,G_ucStorData[2]);
	WRITE8(Addr, 0,G_ucStorData[3]);
	//printf("addr= %lx\n", Addr);
	return Addr;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_LCard2Dram                                         *
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
SMC_LCard2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 size){
	//UINT32 t0,t1;
	UINT32 phyAddr;
	UINT8 sts;
	UINT32 z0, z1, z2;
	//UINT32 i;
	sts = size;
	//printf("%s(%ld) logAddr=%lx,ramAddr=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,logAddr,ramAddr,size );
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	//first non-block alignment data
	z0 = spiDev.blkSize - (logAddr % spiDev.blkSize);
	//printf("%s %u z0=%lx\n", __FILE__, (unsigned short)__LINE__,z0);
	if (size > z0) {
		size = size - z0;
	}
	else {
		z0 = size;
		size = 0;
	}
	if (z0 > 0) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		phyAddr = (phyAddr << spiDev.bitBlkSize) + (logAddr % spiDev.blkSize);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z0);
			return sts;
		}
	}
	//printf("%s %u phyAddr=%lx,ramAddr=%lx,logAddr=%lx\n", __FILE__, (unsigned short)__LINE__,phyAddr,ramAddr,logAddr);
	sfPhySectorR(phyAddr >> 9, z0 >> 9, 512 , ramAddr , K_SPI2DRAM);
	//dumpData(512, (ramAddr), 1);
	logAddr = logAddr + z0;
	phyAddr += z0;
	ramAddr += (z0 >> 1);
	//second is block alignment data
	z1 = size / spiDev.blkSize;
	//printf("%s %u z1=%lx\n", __FILE__, (unsigned short)__LINE__,z1);
	while (z1) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		phyAddr = phyAddr << spiDev.bitBlkSize;
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		sfPhySectorR(phyAddr >> 9, spiDev.blkSize >> 9, 512 , ramAddr , K_SPI2DRAM);
		phyAddr += spiDev.blkSize;
		ramAddr += (spiDev.blkSize >> 1);
		z1--;
		logAddr += spiDev.blkSize;
	}
	//3rd is the last block alignment data
	z2 = size % spiDev.blkSize;
	//printf("%s %u z2=%lx\n", __FILE__, (unsigned short)__LINE__,z2);
	if (z2 > 0) {
		sts = SMC_ReadLpaToPpa(logAddr, &phyAddr);
		phyAddr = phyAddr << spiDev.bitBlkSize;
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		sfPhySectorR(phyAddr >> 9, z2 >> 9, 512 , ramAddr , K_SPI2DRAM);
		phyAddr += z2;
		ramAddr += (z2 >> 1);
	}
	//HAL_GlobalReadGTimer(&t1);
	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_LDram2Card                                         *
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
SMC_LDram2Card(UINT32 logAddr, UINT32 ramAddr, UINT32 size)
{
	UINT32 i, nrPage;
	UINT32 /*phyAddr,*/oldPhyAddr,newPhyAddr,src,dst,sts;
	UINT32 z0, z1, z2, tmp;
	UINT16 blkNum;
	//printf("%s %u logAddr=%lx,ramAddr=%lx,size=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,ramAddr,size);
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	nrPage = spiDev.blkSize >> 9;
	size = size << 9;
	//First non-block alignment
	//dumpData(512, (ramAddr), 1);
	z0 = 0;
	if(logAddr % spiDev.blkSize){
		z0 = spiDev.blkSize - (logAddr % spiDev.blkSize);
		if (size > z0) {
			size = size - z0;
		} else {
			z0 = size;
			size = 0;
		}
	}
	//printf("%s %u z0=%lx\n", __FILE__, (unsigned short)__LINE__,z0);
	if (z0 != 0) {
		sts = spiWriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z0);
			return sts;
		}
		tmp = logAddr % spiDev.blkSize;
		logAddr = logAddr + z0;
		z0 = z0 >> 9;
		tmp = tmp >> 9;
		src = oldPhyAddr << spiDev.bitBlkSize;
		dst = newPhyAddr << spiDev.bitBlkSize;
		for (i = 0; i < tmp ; i ++) {
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
		for (i = tmp; i < (tmp + z0) ; i ++) {
			if( ramAddr ){
				sfPhySectorW(dst >> 9, (UINT16)1, 512, ramAddr, K_DRAM2SPI);
				ramAddr += 256L;
			}
			else{
				sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, K_USB2SPI);
			}
			src += 512L;
			dst += 512L;
		}
		for (i = (tmp + z0); i < nrPage ; i ++) {
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
		dst = oldPhyAddr << spiDev.bitBlkSize;
		src = newPhyAddr << spiDev.bitBlkSize;
		sfSectorErase(dst);
		for (i = 0; i < nrPage; i ++) {
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
	}
	//Next block alignment
	z1 = size / spiDev.blkSize;
	//printf("%s %u z1=%lx\n", __FILE__, (unsigned short)__LINE__,z1);
	while (z1) {
		G_Spi_Erase = 0;
		sts = spiWriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		sfSectorErase(oldPhyAddr << spiDev.bitBlkSize);
		if( ramAddr ){
			sfPhySectorW(oldPhyAddr, (UINT16)1, spiDev.blkSize, ramAddr, K_DRAM2SPI);
			ramAddr += (spiDev.blkSize / 2);
		}
		else{
			sfPhySectorW(oldPhyAddr, (UINT16)1, spiDev.blkSize, 0, K_USB2SPI);
		}
		z1--;
		logAddr += spiDev.blkSize;
		size -= spiDev.blkSize;
	}
	//Last remain not enough 1 block
	z2 = size % spiDev.blkSize;
	//printf("%s %u z2=%lx\n", __FILE__, (unsigned short)__LINE__,z2);
	z2 = z2 >> 9;
	if (z2 > 0) {
		sts = spiWriteLbaToPba(logAddr, &oldPhyAddr, &newPhyAddr, &blkNum);
		//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__, (UINT32)__LINE__,logAddr,oldPhyAddr,newPhyAddr);
		if (sts == FALSE) {
			//printf("%s %u err, z0=%lu\n", __FILE__, (unsigned short)__LINE__,z1);
			return sts;
		}
		src = oldPhyAddr << spiDev.bitBlkSize;
		dst = newPhyAddr << spiDev.bitBlkSize;
		for (i = 0; i < z2 ; i ++) {
			if( ramAddr ){
				sfPhySectorW(dst >> 9, (UINT16)1, 512, ramAddr, K_DRAM2SPI);
				ramAddr += 256L;
			}
			else{
				sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, K_USB2SPI);
			}
			src += 512L;
			dst += 512L;
		}
		for (i = (nrPage - z2); i < nrPage ; i ++) {
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
		dst = oldPhyAddr << spiDev.bitBlkSize;
		src = newPhyAddr << spiDev.bitBlkSize;
		sfSectorErase(dst);
		for (i = 0; i < nrPage; i ++) {
			sfPhySectorR(src >> 9, (UINT16)1 ,512, 0, 0x0000);
			sfPhySectorW(dst >> 9, (UINT16)1, 512, 0, 0x0000);
			src += 512L;
			dst += 512L;
		}
	}
	return TRUE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_LCard2Usb                                         *
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
SMC_LCard2Usb(UINT32 logAddr, UINT32 size)
{
	UINT32 phyAddr;
	UINT8 sts;
	//UINT16 i;
	size = size;
#if 0
	printf("%s(%ld) logAddr=%lx\n",__FILE__,(UINT32)__LINE__,logAddr );
#endif
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	if (SMC_ReadLpaToPpa(logAddr, &phyAddr)){
		phyAddr = (phyAddr << spiDev.bitBlkSize) + (logAddr % spiDev.blkSize);
#if 0
		printf("%s(%ld) phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr );
#endif
		sfPhySectorR(phyAddr >> 9, 1 , 512 , 0 , K_SPI2USB);
	}
	return sts;
}

UINT8
SMC_LUsb2Card(UINT32 logAddr, UINT32 size)
{
	UINT32 sts = TRUE;
	printf("%s(%ld) logAddr=%lx,size=%lx\n",__FILE__,(UINT32)__LINE__,logAddr, size);
	sts = SMC_LDram2Card(logAddr, 0, size);
	return sts;
}

UINT8 SMC_Dram2Card(UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr){
	//printf("%s(%ld) phyAddr=%lx,ramAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr,ramAddr );
	blockNum = blockNum;
	sfPhySectorW(phyAddr >> 9, 1 , 512 , ramAddr >> 1, K_DRAM2SPI);
	return TRUE;
}
UINT8 SMC_Card2Dram(UINT32 phyAddr, UINT32 ramAddr){
	//printf("%s(%ld) phyAddr=%lx,ramAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr,ramAddr );
	sfPhySectorR(phyAddr >> 9, 1 , 512 , ramAddr >> 1, K_SPI2DRAM);
	return TRUE;
}
UINT8 SMC_Card2Sram(UINT32 phyAddr){
	//printf("%s(%ld) phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr );
	sfPhySectorR(phyAddr >> 9, 1 , 512 , 0 , 0x0000);
	return TRUE;
}
UINT8 SMC_Sram2Card(UINT32 phyAddr, UINT16 blockNum){
	//printf("%s(%ld) phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr );
	sfPhySectorW(phyAddr >> 9, 1 , 512 , 0 , 0x0000);
	return TRUE;
	blockNum = blockNum;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: SMC_EraseTotalBlock                                         *
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
UINT8 SMC_EraseTotalBlock(void)
{
	HAL_FlashMode(K_MEDIA_Spi, 0, 0);
	//printf("%s(%ld) TotalBlk=%lx\n",__FILE__,(UINT32)__LINE__,(diskPhySectors << 9) >> spiDev.bitBlkSize);
	//spiEraseSector( G_SPI_CisBlockAddress << spiDev.bitBlkSize, diskPhySectors << 9, 0);
	return TRUE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: spiReadLpaToPpa                                         *
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
UINT8 SMC_ReadLpaToPpa(UINT32 logAddr, UINT32* phyAddr){
	UINT32 blkAddr = logAddr  >> spiDev.bitBlkSize;
	(*phyAddr) = 1 + G_SPI_CisBlockAddress + blkAddr;
	//printf("%s(%ld) logAddr=%lx,phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,logAddr,(*phyAddr));
	return TRUE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: spiWriteLbaToPba                                         *
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
UINT8 spiWriteLbaToPba(UINT32 logAddr, UINT32 *oldPhyAddr, UINT32 *newPhyAddr, UINT16* blockNum)
{
	UINT16 phy;
	UINT32 blkAddr;
	blkAddr = logAddr >> spiDev.bitBlkSize;
	(*oldPhyAddr) = 1 + G_SPI_CisBlockAddress + blkAddr;
	#if 0//why error ??
	spiFreeBlkReq(0, 1, &phy, 0);
	(*newPhyAddr) = (UINT32)phy;
	(*blockNum) = 0xffff;
	//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__,(UINT32)__LINE__,logAddr,(*oldPhyAddr),(*newPhyAddr));
	#else
	if( G_Spi_Erase ){
		UINT32 offsetBlk = rsvRawB - spiDev.bufBlk;
		if( G_Buf_Blk_Index >= spiDev.bufBlk ){//Reserve Block as bufferring
			G_Buf_Blk_Index = 0;
		}
		phy = rsvBMap[offsetBlk + G_Buf_Blk_Index];
		G_Buf_Blk_Index ++;
		//printf("A  phy=%x, %lx, %lx, %bx\n",phy,rsvRawA,rsvRawB,G_Buf_Blk_Index);
		spiEraseSector(((UINT32)phy) << spiDev.bitBlkSize, spiDev.blkSize, 0);
	}
	G_Spi_Erase = 1;
	(*newPhyAddr) = (UINT32)phy;
	(*blockNum) = 0xffff;
	//printf("%s(%ld) logAddr=%lx,oldPhyAddr=%lx,newPhyAddr=%lx\n", __FILE__,(UINT32)__LINE__,logAddr,(*oldPhyAddr),(*newPhyAddr));
	#endif
	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: spiCisIdi                                         *
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
UINT8 spiCisIdi(UINT32 phyAddr)
{
	UINT8 sts = TRUE;
	UINT16 i, j;
	printf("%s(%ld) phyAddr=%bx\n",__FILE__,(UINT32)__LINE__,phyAddr );
	//CIS/IDI
	//Should be already erased under physical fmt */
	for (i = 0; i < K_Card_PageSize; i++)
		G_ucStorData[i] = (UINT8) 0x00;
	for (i = 0, j = K_Card_PageSize / 2; i < K_Card_PageSize / 4; i++) {
		G_ucStorData[i] = G_ucStorData[i + j] = (UINT8) G_SMC_CIS[i];
	}
	spiEraseSector(phyAddr, spiDev.blkSize, 0);
	//dumpData(512, G_ucStorData, 0);
	sfPhySectorW(phyAddr >> 9, 1 , 512 , 0 , 0x0000);
	phyAddr = phyAddr + (UINT32) K_Card_PageSize;
	sfPhySectorW(phyAddr >> 9, 1 , 512 , 0 , 0x0000);
	return sts;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: spiCisCheck                                         *
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
UINT8 spiCisCheck(void)
{
	UINT8 i, sts;
	UINT32 phyAddr = G_SPI_CisBlockAddress;
	sts = TRUE;
	//printf("%s(%ld) phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr);
	phyAddr = phyAddr << spiDev.bitBlkSize;
	sfPhySectorR(phyAddr >> 9, 1 , 512 , 0 , 0x0000);
	//printf("%s(%ld) phyAddr=%lx\n",__FILE__,(UINT32)__LINE__,phyAddr);
	//dumpData(512, G_ucStorData, 0);
	for (i = 0; i < K_SMC_CisPartialSize; i++) {
		if (G_SMC_CisPartial[i] != G_ucStorData[i])
			sts = FALSE;
	}

	return sts;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: spiEraseSector                                         *
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
UINT8 SMC_Initialize(UINT8 doDOSInitial)
{
	UINT8 sts = TRUE;
	//printf("%s(%ld) doDOSInitial=%bx\n",__FILE__,(UINT32)__LINE__,doDOSInitial );
	if( spiDev.blkSize == 0 ){
		sts = SMC_CardDetect();
	}
	if (sts) {
		if (doDOSInitial == 2)
			sts = TRUE;
		else {
			sts = TRUE;//spiCisCheck();
			if (sts == FALSE) {
				spiCisIdi(G_SPI_CisBlockAddress << spiDev.bitBlkSize);
			}
			sts = TRUE;
		}
	}
	if (sts) {
		if (doDOSInitial) {
			sts = DOS_Initialize();
		}
	}
	return sts;
}

UINT8 SMC_LCard2Sram(UINT32 logAddr)
{
#if !TAG_TODO
	logAddr = logAddr;
	return TRUE;
#endif
}

UINT16 SMC_GetBehavior(void)
{
#if !TAG_TODO
	return 0;
#endif
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
	//Dram para at offset 512 bytes
	//dumpData(512, ramAddr, 1);
	HAL_FlashMode(K_MEDIA_Spi,0,0);
	sfPhySectorR(1, 1 , 512 , ramAddr, K_SPI2DRAM);
	//dumpData(512, ramAddr, 1);
}

UINT8 codeWriteBack(UINT32 bank,UINT32 offset,UINT32 addrw,UINT32 size){

	UINT32 addrb = 32ul*1024*(bank+1) + (offset - 0x8000) + K_START_OFFSET+(UINT32)sizeof(spiRsvHeader_t);
	UINT8 sts,mode = SPI_STS_UNLOCK;//SPI_STS_LOCK_ALL

	//printf("SPI:%lx*%lx=%lx,%lx(%lx,%lx)%lx,%lx\n",spiDev.blkSize,fwStartBlock,spiDev.blkSize*fwStartBlock,addrb,addrw,size,(UINT32)sizeof(spiRsvHeader_t),G_Firmware_Size);
	if(spiDev.blkSize>1024*8 || addrb%spiDev.blkSize || size %spiDev.blkSize ){
		return FALSE;
	}

	if( pspiWPSet != NULL ){
		sts = (*pspiWPSet)(&mode);
		mode = SPI_STS_LOCK_ALL;
		////printf("UNLOCK(%bx)\n",sts);
	}

	spiEraseSector(addrb, size, 1);
	sfPhySectorW(addrb>>9, size >> 9 , 512 , addrw, K_DRAM2SPI);

	if( pspiWPSet != NULL ){
		sts = (*pspiWPSet)(&mode);
		//printf("LOCK(%bx)\n",sts);
	}
	return TRUE;
}
#if 0
UINT32
spiReadAllRaw(UINT32 addr)
{
	UINT32 nrPage, phy;
	UINT32 i, k, size;
	size = 0;
	HAL_FlashMode(K_MEDIA_Spi,0,0);
	for( k = 0; k < rsvRawA ; k ++){
		phy = rsvAMap[k];
		if( phy == 0xffff ){
			continue;
		}
		phy = phy * spiDev.blkSize;
		nrPage = spiDev.blkSize >> 9;
		#if 0
		printf("nrPage=%lx,phy=%lx,blk=%lx\n",nrPage,phy, phy / spiDev.blkSize);
		#endif
		for (i = 0; i < nrPage; i++)
		{
			sfPhySectorR(phy >> 9 , (UINT16)1 ,512, addr, 0);
			phy += 512L;
			addr += 256L;
			size += 512L;
		}

	}
	for( k = 0; k < (rsvRawB - spiDev.bufBlk) ; k ++){
		phy = rsvBMap[k];
		if( phy == 0xffff ){
			continue;
		}
		phy = phy * spiDev.blkSize;
		nrPage = spiDev.blkSize >> 9;
		#if 0
		printf("nrPage=%lx,phy=%lx,blk=%lx\n",nrPage,phy, phy / spiDev.blkSize);
		#endif
		for (i = 0; i < nrPage; i++)
		{
			sfPhySectorR(phy >> 9 , (UINT16)1 ,512, addr, 0);
			phy += 512L;
			addr += 256L;
			size += 512L;
		}

	}
	return size;
}
#endif
#endif //DISK_DRV_SPI
