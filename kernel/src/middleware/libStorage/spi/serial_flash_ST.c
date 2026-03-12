#include "general.h"
#include "hal_device.h"
#include "hal.h"
#include "usbopt.h"
//#include "cardlink.h"
/*
    some key performance of Winbond 64Mb
    1. Page program ( 256 byte ) ~= 1.5ms
    2. Sector Erase ( 4KB ) ~= 68ms
    3. 1MB write tooks 285 sector erase
    4. 1MB write tooks 4096 page program
*/
#include "main.h"
#include "uiflow.h"
#include "string.h"
#include "asicreg.h"
#include "spifunc.h"
#ifdef DISK_DRV_SPI

#define DEBUG 0
#define	SPI_DMA_ENABLE	1

#define	CMD_RD_ID				0x9F
#define	CMD_RD_DATA_25MHZ		0x03
#define	CMD_RD_DATA_50MHZ		0x0B
#define	CMD_SECTOR_ERASE		0x20
#define	CMD_ERASE			0xD8
#define	CMD_ERASE_ALL			0xC7	/* 13.0s */
#define	CMD_BYTE_PROG			0x02
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

UINT8 g_spiBusWidth = 1;


void HAL_SPIWritePort(UINT8 PortData);
UINT8 HAL_SPIReadPort(void);
UINT8 HAL_SPIInit(UINT8 Freq,UINT8 Mode);
void sfChipErase(void);
void sfSimpleCmd(UINT8 cmd);

void sfSimpleCmd(UINT8 cmd)
{
	SPI_CE_LO;
	HAL_SPIWritePort(cmd);
	SPI_CE_HI;
}

void sfStatusWrite(UINT8 dat)
{
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

//UINT32 G_Erase_Count = 0;
//UINT32 G_Write_Count = 0;
void sfSectorErase(UINT32 addr)
{
	UINT32 i;
	//UINT32 t0,t1;
	//HAL_GlobalReadGTimer(&t0);
#if DEBUG
	printf("%s %d,addr=%lx,blk=%lx\n",__FILE__,(UINT16) __LINE__,addr, addr/spiDev.blkSize );
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
		if( i > 0x00ffffff) printf("%s(%ld)\n",__FILE__,(UINT32)__LINE__);
		//printf("e");
	}
	//HAL_GlobalReadGTimer(&t1);
	//G_Erase_Count += (t1- t0);
	//printf("erase %lu, %lu\n",t1-t0, G_Erase_Count);
	return;
}


void sfPageProg(UINT16 page,UINT16 num, UINT32 buf, UINT32 sram)
{
	UINT8 i;
#if SPI_DMA_ENABLE
	//UINT32 t0,t1;
#else
	UINT8 tmp0, tmp1;
#endif
	UINT32 retry;

	while (num) {
            //HAL_GlobalReadGTimer(&t0);
            SF_WRITE_EN;
            if(g_spiBusWidth == 4){
                XBYTE[REG_Fm_SPIcsnn] = 0x00;                   // SPIcsnn
                HAL_SPIWritePort(0x01);                 /*WRSR*/
                HAL_SPIWritePort(0x40);
                XBYTE[REG_Fm_SPIcsnn] = 0x01;                   // SPIcsnn

                SF_WRITE_EN;
    
                XBYTE[REG_Fm_SPIcsnn] = 0x00;                   // SPIcsnn
                HAL_SPIWritePort(0x38);                 /*4PP*/
                XBYTE[REG_Fm_spibuswidth] = 0x02;                   //buswidth
                XBYTE[REG_Fm_spipinmux] = 0x01;					//spi pin mux
            }
            else{
                SPI_CE_LO;
                HAL_SPIWritePort(CMD_BYTE_PROG);                
            }
            
            HAL_SPIWritePort(page >> 8);	/* A23~A16 */
            HAL_SPIWritePort(page);	/* A15~A08 */
            HAL_SPIWritePort(0);			/* A07~A00 */
                

#if SPI_DMA_ENABLE
		if( sram == K_DRAM2SPI ){
			XBYTE[REG_Dma_DmaIdle] = 0x01;	// reset DMA machine
			XBYTE[REG_Dma_DmaIdle] = 0x00;
			XBYTE[REG_Dma_DmaSrcDst] = (2 << 4) | 0;	//DMA FROM DRAM TO SPI */
			XBYTE[REG_Dma_DmaSizeL] = 0xff;	/* size = 512//256 bytes */
			XBYTE[REG_Dma_DmaSizeM] = 0;
			XBYTE[REG_Dma_DmaCmp] = 0x01;
			HAL_DmaSetDramAddr(buf);
			XBYTE[REG_Dma_DmaStart] = 0x01;	// trigger DMA
			while (XBYTE[REG_Dma_DmaCmp] == 0)	// wait for DMA complete
			{
				//printf("p");
			}
			XBYTE[REG_Dma_DmaCmp] = 0x01;
			buf += 128;
		}
		else if(sram == K_USB2SPI){//USB -> SPI 
//printf("1\n");
			HAL_DmaDo(4,2,256,1,0);
//printf("%bx,%x=%bx\n",G_bUSBSpeed,K_USB_CLASS_OUT_BUF_SIZE, XBYTE[K_USB_CLASS_OUT_BUF_SIZE]);
			if (G_bUSBSpeed == 0) {
				for (i = 0; i < 4; i++) {
					while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
						printf("1 %bx\n",XBYTE[K_USB_CLASS_OUT_BUF_SIZE]);
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
					//patch4.5@richie@msdc plug out exception
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)){  //wait pc ack
						printf("0x2500 %bx\n",XBYTE[0x2500]);
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
				}
				while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;						
				}
			} 
			else {
				while (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8)) > 0)	{  //wait for 0	in bulk	buffer
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)) break;	
				}
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
				while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)) break;	
				}
				XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
				while ((XBYTE[REG_Dma_DmaCmp] != 0x01)&& (G_UI_USBConnect == K_UI_USB_CONNECT)){
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
			}
                    #if DEBUG
                    printf("%s %d page=%x,blk=%d,num=%d\n",__FILE__, (UINT16)__LINE__ ,(page << 8), (page << 8)/(UINT16)spiDev.blkSize, num);
                    #endif
		}
		else{
                #if DEBUG
                printf("%s %d, page=%d num=%d sram=%lx\n",__FILE__, (UINT16)__LINE__, page, num, sram);
                #endif
                HAL_DmaSetSramAddr(sram);
                HAL_DmaDo(1,2,256,0,0);
                sram += 256;
		}
#else
		M_DRAM_SetStartAddress((UINT32) buf,1);
		for (i = 0; i < 256 / 2; i++)	/* 256 bytes/page */ {
			//printf("p");
			M_DRAM_ReadWord(tmp0,tmp1);
                    #if	0
                    tmp0 = num & 0xff;
                    tmp1 = i & 0xff;
                    #endif
			//if((i % 8)==0)
			//	printf("\n%08LX ", buf+i*2);
			//	printf("%02BX %02BX ", tmp0, tmp1);
			HAL_SPIWritePort(tmp0);
			HAL_SPIWritePort(tmp1);
		}
		buf += 128;
#endif
		SPI_CE_HI;
            if(g_spiBusWidth != 1){
                XBYTE[REG_Fm_spibuswidth] = 0x00;                   //buswidth
                XBYTE[REG_Fm_spipinmux] = 0x00;
             }

		retry = 0;
		while (1) {
			retry++;
			//printf("%bx ", sfStatusRead());
			if (!(sfStatusRead() & 0x01) || retry > 100000) {
				/* sfStatusRead() > 4.8us */
				if (retry > 100000) {
					printf("sts fail %bx\n",sfStatusRead());
				}
				break;
			}
		
			//printf("w");
		}
		num--;
		page++;
		//HAL_GlobalReadGTimer(&t1);
		//printf("pg %lu\n",t1-t0);
	}
}

UINT32 sfPhySectorW(UINT32 start,UINT16 count,UINT32 dmaSize, UINT32 buf, UINT32 sram)
{
    UINT16 WrtCount = count, num, Wnum;
    UINT32 startIni = start;	
    //UINT32 t0,t1;
    //HAL_GlobalReadGTimer(&t0);
    #if 0
    printf("%s %d p=%lx,b=%lx\n",__FILE__,(UINT16) __LINE__,start << 9,(start << 9)/4096);
    #endif
    
    while (count > 0) {
        //num = (count > (UINT16) SECTOR_NUM_PER_ERASE) ? SECTOR_NUM_PER_ERASE : count;
        num = count;
        #if DEBUG
        printf("%s %d, <sfPhySectorW> s=%ld c=%d n=%d %d buf=%08lx\n", \
        	__FILE__,(UINT16) __LINE__,start,count,num,
        	(UINT16) SECTOR_NUM_PER_ERASE,buf);
        #endif
        //sfSectorErase(start);
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
    //HAL_GlobalReadGTimer(&t1);
    //G_Write_Count += (t1- t0);
    return (WrtCount - count) * 512;
}

void sfPhySectorR(UINT32 start,UINT16 count,UINT32 dmaSize, UINT32 buf, UINT32 sram )
{
#if (SPI_DMA_ENABLE == 0)
	UINT32 j;
	UINT8 tmp,k;
#endif
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
            if(g_spiBusWidth == 4){
                XBYTE[REG_Fm_SPIcsnn] = 0x00;                   // SPIcsnn
                HAL_SPIWritePort(0x01);                 /*WRSR*/
                HAL_SPIWritePort(0x40);
                XBYTE[REG_Fm_SPIcsnn] = 0x01;                   // SPIcsnn

                /*Read serial flash*/
                XBYTE[REG_Fm_SPIcsnn] = 0x00;                   // SPIcsnn
                HAL_SPIWritePort(0xEB);                 /*4 I/O READ*/
                XBYTE[REG_Fm_spibuswidth] = 0x02;                   //buswidth
                XBYTE[REG_Fm_spipinmux] = 0x01;					//spi pin mux

                HAL_SPIWritePort(addr >> 16);		/* A23~A16 */
                HAL_SPIWritePort(addr >> 8);		/* A15~A08 */
                HAL_SPIWritePort(addr);				/* A07~A00 */

                HAL_SPIWritePort(0x00);                 /*dummy clock*/
                HAL_SPIWritePort(0x00);                 /*dummy clock*/
                HAL_SPIWritePort(0x00);                 /*dummy clock*/
            }
            else if(g_spiBusWidth == 2){
                XBYTE[REG_Fm_SPIcsnn] = 0x00;                   // SPIcsnn
                HAL_SPIWritePort(0xBB);                 /*2 I/O READ*/
                XBYTE[REG_Fm_spibuswidth] = 0x01;                   //buswidth
                
                HAL_SPIWritePort(addr >> 16);		/* A23~A16 */
                HAL_SPIWritePort(addr >> 8);		/* A15~A08 */
                HAL_SPIWritePort(addr);				/* A07~A00 */

                HAL_SPIWritePort(0x00);                 /*dummy clock*/
            }
            else{
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
            }


#if	SPI_DMA_ENABLE
		if( sram == K_SPI2DRAM ){//SPI -> DRAM
			for (i = 0; i < count; i++) {
                        #if DEBUG
                        printf("%s %d, start=%x buf=%lx\n",__FILE__,(UINT16) __LINE__,
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
				while (XBYTE[REG_Dma_DmaCmp] == 0)	// wait for DMA complete
				{
					//printf("r");
				}
				XBYTE[REG_Dma_DmaCmp] = 0x01;
				buf += bufOffset;//256;
			}
		}
		else if( sram == K_SPI2USB ){//SPI -> USB 
			HAL_DmaDo(2, 4, (count * 512), 1, 1);
			if (G_bUSBSpeed == 0) {
				count = (8 * count);
				for (i = 0; i < count; i++) {
					while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait for 0 in bulk buffer
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait pc ack
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
				}
			} 
			else {
				for( i = 0; i < count ; i++){
					while ((XBYTE[K_USB_AP_IN_BUF_SIZE + 1] != 0x02) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
						if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
					}
					XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
				}
			}
		}
		else{
			HAL_DmaSetSramAddr((UINT16)sram);
			HAL_DmaDo(2,1,count * dmaSize,0,0);
		}
#else
		for (i = 0; i < count*2; i++) {
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
#endif
		SPI_CE_HI;
            if(g_spiBusWidth != 1){
                XBYTE[REG_Fm_spibuswidth] = 0x00;                   //buswidth
                XBYTE[REG_Fm_spipinmux] = 0x00;
             }
	}
	return;
}
#endif //DISK_DRV_SPI
