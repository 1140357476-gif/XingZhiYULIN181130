#include "general.h"
#include "string.h"
#include "asicreg.h"
#include "sdramhw.h"
#include "spifunc.h"
#include "main.h"
#include "hal.h"

#ifdef DISK_DRV_SPI

#define DEBUG 0
#define	SPI_DMA_ENABLE	1

#define	CMD_RD_ID				0x9F
#define	CMD_RD_DATA_25MHZ			0x03
#define	CMD_RD_DATA_50MHZ			0x0B
#define	CMD_SECTOR_ERASE		0x20
#define	CMD_ERASE			0xD8
#define	CMD_ERASE_ALL			0xC7	/* 13.0s */
#define	CMD_BYTE_PROG				0x02
#define	CMD_BYTE_PROG_AAI			0xAD
#define	CMD_WRT_STS_EN				0x50
#define	CMD_WRT_STS				0x01
#define	CMD_RD_STS				0x05
#define	CMD_WRT_EN				0x06
#define	CMD_WRT_DIS				0x04
#define	PAGE_SIZE		256
#define	PAGE_SIZE_B		8
#define	PAGE_PER_SECTOR	256
#define	SECTOR_PER_DIE	32
#define	SECTOR_NUM_PER_ERASE	((long)PAGE_PER_SECTOR*PAGE_SIZE/512)

#define	SPI_CE_LO	XBYTE[REG_Fm_SPIcsnn] = 0;
#define	SPI_CE_HI	XBYTE[REG_Fm_SPIcsnn] = 1;
#define	SF_WRITE_EN		sfSimpleCmd(CMD_WRT_EN);
#define	SF_WRITE_DIS	sfSimpleCmd(CMD_WRT_DIS);
#define	SF_STS_WRT_EN	sfSimpleCmd(CMD_EN_WRT_STS);

//#define	UINT8  unsigned char
//#define	UINT16 unsigned short
//#define	UINT32 unsigned long

void HAL_SPIWritePort(UINT8 PortData);
UINT8 HAL_SPIReadPort(void);
UINT8 HAL_SPIInit(UINT8 Freq,UINT8 Mode);
void sfChipErase(void);
void sfSimpleCmd(UINT8 cmd);
UINT8 sfStatusRead(void);

void sfSimpleCmd(UINT8 cmd)
{
	SPI_CE_LO;
	HAL_SPIWritePort(cmd);
	SPI_CE_HI;
}

void sfStatusWrite(UINT8 dat)
{
	SPI_CE_LO;
	HAL_SPIWritePort(CMD_WRT_STS_EN);
	SPI_CE_HI;
	SPI_CE_LO;
	HAL_SPIWritePort(CMD_WRT_STS);
	HAL_SPIWritePort(dat);
	SPI_CE_HI;
	return;
}

UINT8 sfStatusRead(void)
{
	UINT8 ret;
	SPI_CE_LO;
	HAL_SPIWritePort(CMD_RD_STS);
	ret = HAL_SPIReadPort();
	SPI_CE_HI;
	return ret;
}

/*
*/
void sfChipErase(void)
{
	sfStatusWrite(0x02);
	SF_WRITE_EN;
	SPI_CE_LO;
	HAL_SPIWritePort(CMD_ERASE_ALL);
	SPI_CE_HI;

	while (1) {
		if (!(sfStatusRead() & 0x01))	/* sfStatusRead() > 4.8us */ {
			break;
		}
	}
	return;
}


void sfIdRead(UINT8 id[3])
{
	SPI_CE_LO;
	HAL_SPIWritePort(CMD_RD_ID);		/*read ID command*/
	id[0] = HAL_SPIReadPort();		/* Manufacturer's ID, 20h for STMicroelectronics. */
	id[1] = HAL_SPIReadPort();		/* Memory Type     */
	id[2] = HAL_SPIReadPort();		/* Memory Capacity */
	SPI_CE_HI;
#if DEBUG
	printf("%s %d, id=%02BX %02BX %02BX\n",__FILE__,(UINT16) __LINE__,id[0],
		id[1],id[2]);
#endif
	return;
}

void sfSectorErase(UINT32 addr)
{
	//SF_STS_WRT_EN;	/* unlock all blocks */
	UINT32 i;
#if DEBUG
	printf("%s %d,addr=%lx\n",__FILE__,(UINT16) __LINE__,addr);
#endif
	sfStatusWrite(0x02);
	SF_WRITE_EN;

	SPI_CE_LO;
	HAL_SPIWritePort(spiDev.eraseCmd);
	HAL_SPIWritePort(addr >> 16);	/* A23~A16 */
	HAL_SPIWritePort(addr >> 8);		/* A15~A08 */
	HAL_SPIWritePort(addr);			/* A07~A00 */
	SPI_CE_HI;

	/* MAX_TIME for SECTOR/BLOCK ERASE is 25ms */
	i = 0;
	while (1) {
		if (!(sfStatusRead() & 0x01))	/* sfStatusRead() > 4.8us */ {
			break;
		}
		i++;
		if( i > 0x0fffffff) printf("%s(%ld)\n",__FILE__,(UINT32)__LINE__);
		//printf("e");
	}
	return;
}


void sfPageProg(UINT32 page, UINT16 num, UINT32 buf, UINT32 sram)
{
	UINT8 tmp0, tmp1;
	UINT32 retry, number, i;

	number = (UINT32)num;
	if( sram == K_DRAM2SPI ){
		SF_WRITE_EN;
		SDRAM_CardSetStartAddress(buf, TRUE);
		M_DRAM_ReadWord(tmp0,tmp1);
		SPI_CE_LO;
		HAL_SPIWritePort(CMD_BYTE_PROG_AAI);
		HAL_SPIWritePort(page >> 8);	/* A23~A16 */
		HAL_SPIWritePort(page);	/* A15~A08 */
		HAL_SPIWritePort(0);			/* A07~A00 */
		HAL_SPIWritePort(tmp0);		
		HAL_SPIWritePort(tmp1);
		SPI_CE_HI;
		number = ( number << 8 ) - 2;
		while (number) {
#if DEBUG
		printf("%s %d, page=%ld num=%ld buf=%08lx\n",__FILE__, (UINT16)__LINE__, page, number, buf);
#endif
			M_DRAM_ReadWord(tmp0,tmp1);
			SPI_CE_LO;
			HAL_SPIWritePort(CMD_BYTE_PROG_AAI);
			HAL_SPIWritePort(tmp0);		
			HAL_SPIWritePort(tmp1);
			SPI_CE_HI;
			number -= 2;
		}
	}
	else if( sram == K_DRAM2SPI ){
	}
	else{
		SF_WRITE_EN;
		tmp0 = G_ucStorData[0];
		tmp1 = G_ucStorData[1];
		SPI_CE_LO;
		HAL_SPIWritePort(CMD_BYTE_PROG_AAI);
		HAL_SPIWritePort(page >> 8);	/* A23~A16 */
		HAL_SPIWritePort(page);	/* A15~A08 */
		HAL_SPIWritePort(0);			/* A07~A00 */
		HAL_SPIWritePort(tmp0);		
		HAL_SPIWritePort(tmp1);
		SPI_CE_HI;
		number = ( number << 8 ) - 2;
		i = 2;
		while (number) {
#if DEBUG
		printf("%s %d, page=%ld num=%ld buf=%08lx\n",__FILE__, (UINT16)__LINE__, page, number, buf);
#endif
			tmp0 = G_ucStorData[0 + i];
			tmp1 = G_ucStorData[1 + i];
			SPI_CE_LO;
			HAL_SPIWritePort(CMD_BYTE_PROG_AAI);
			HAL_SPIWritePort(tmp0);		
			HAL_SPIWritePort(tmp1);
			SPI_CE_HI;
			number -= 2;
			i += 2;
		}
	}

	SF_WRITE_DIS;
	retry = 0;
	while (1) {
		retry++;
		if (!(sfStatusRead() & 0x01) || retry > 1000) {
			if (retry > 1000) {
				printf("sts fail %bx\n",sfStatusRead());
			}
			break;
		}
	}
}

UINT32 sfPhySectorW(UINT32 start,UINT16 count,UINT32 dmaSize, UINT32 buf, UINT32 sram)
{
	UINT16 WrtCount = count, num, Wnum;
	UINT32 startIni = start;	

	while (count > 0) {
		//num = (count > (UINT16) SECTOR_NUM_PER_ERASE) ? SECTOR_NUM_PER_ERASE : count;
		num = count;
#if DEBUG
		printf("%s %d, <sfPhySectorW> s=%ld c=%d n=%d %d buf=%08lx\n", \
			__FILE__,(UINT16) __LINE__,start,count,num,
			(UINT16) SECTOR_NUM_PER_ERASE,buf);
#endif
		startIni = (start * dmaSize) >> PAGE_SIZE_B;
		Wnum = (UINT16) (((SINT32) num * dmaSize) >> PAGE_SIZE_B); 
		sfPageProg(startIni, Wnum, buf, sram);
		buf += (UINT32)num << PAGE_SIZE_B;
		count -= num;
		start += num;
	}
#if DEBUG
	printf("%x %x %lx\n",WrtCount,count,(UINT32) (WrtCount - count) * 512);
#endif
	return (WrtCount - count) * 512;
}

void sfPhySectorR(UINT32 start,UINT16 count,UINT32 dmaSize, UINT32 buf, UINT32 sram )
{
#if (SPI_DMA_ENABLE == 0)
	UINT32 j;
	UINT8 tmp,k;
#endif
	UINT32 retry;
	UINT8 tmp0 = (UINT8)(dmaSize - 1), tmp1 = (UINT8)((dmaSize - 1) >> 8);
	UINT32 i, bufOffset = (dmaSize >> 1);
	UINT32 addr = start * dmaSize;
	//printf("%s %d, tmp0=%bx,tmp1=%bx,bufOffset=%lx\n",__FILE__,(UINT16) __LINE__,tmp0,tmp1,bufOffset);
	//printf("%s %d, addr=%lx\n",__FILE__,(UINT16) __LINE__,addr);

	#if SPI_DMA_ENABLE
	XBYTE[REG_Fm_ECCMode] = 0x00;
	#else
	M_DRAM_SetStartAddress(buf, 0);
	#endif

	if (count) {
#if DEBUG
		printf("%s %d, start=%d buf=%08LX\n",__FILE__,(UINT16) __LINE__,
			(UINT16) start,buf);
#endif
		SPI_CE_LO;
		HAL_SPIWritePort(CMD_RD_DATA_25MHZ);	/* READ, clk < 25MHz */
		#if 0
		HAL_SPIWritePort((start >> 7) & 0xFF);		/* A23~A16 */
		HAL_SPIWritePort((start << 1) & 0xFF);		/* A15~A08 */
		HAL_SPIWritePort(0);				/* A07~A00 */
		#else
		HAL_SPIWritePort(addr >> 16);		/* A23~A16 */
		HAL_SPIWritePort(addr >> 8);		/* A15~A08 */
		HAL_SPIWritePort(addr);				/* A07~A00 */
		#endif

#if SPI_DMA_ENABLE
		if( sram == K_SPI2DRAM ){
			for (i = 0; i < count; i++) {
#if DEBUG
				printf("%s %d, start=%d buf=%08LX\n",__FILE__,(UINT16) __LINE__,
					(UINT16) start,buf);
#endif
				XBYTE[REG_Dma_DmaIdle] = 0x01;	// reset DMA machine
				XBYTE[REG_Dma_DmaIdle] = 0x00;
				XBYTE[REG_Dma_DmaSrcDst] = (0 << 4) | 2;	//DMA FROM SPI TO DRAM */
				XBYTE[REG_Dma_DmaSizeL] = tmp0;	/* size = 512 bytes */
				XBYTE[REG_Dma_DmaSizeM] = tmp1;
				XBYTE[REG_Dma_DmaCmp] = 0x01;
				//	HAL_DmaSetDramAddr(buf/2);
				HAL_DmaSetDramAddr(buf);
				XBYTE[REG_Dma_DmaStart] = 0x01;	// trigger DMA
				retry = 0;
				while (XBYTE[REG_Dma_DmaCmp] == 0)	// wait for DMA complete
				{
					retry ++;//printf("r");
					if( retry > 100000 ){
						printf("time out");
						break;
					}
				}
				XBYTE[REG_Dma_DmaCmp] = 0x01;
				buf += bufOffset;//256;
			}
		}
		else{
			HAL_DmaSetSramAddr((UINT16)sram);
			HAL_DmaDo(2,1,count * 512,0,0);
		}
#else
		for (i = 0; i < count*2; i++) {
			printf("%s %d, start=%d buf=%08LX\n",__FILE__,(UINT16) __LINE__,
				(UINT16) start,buf);
			if( sram == K_SPI2DRAM ){
				for (j = 0; j < 256 / 2; j++) {
					//printf("r");
					tmp0 = HAL_SPIReadPort();
					tmp1 = HAL_SPIReadPort();
					M_DRAM_WriteWord(tmp0,tmp1);
#if DEBUG
					if((j % 8)==0) printf("\n%08lX ", buf+j);
					printf("%02BX %02BX ", tmp0, tmp1);
					#endif
				}
				buf += 256;
			}
			else{
				for (j = 0; j < 256 ; j++) {
					//printf("r");
					tmp0 = HAL_SPIReadPort();
					XBYTE[0x3800 + (i* 256) + j] = tmp0;
				}
				buf += 256;
			}
		}
#endif
		SPI_CE_HI;
	}
	return;
}
#endif

