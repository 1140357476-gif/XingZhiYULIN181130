/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_spi.h"
#include "sf.h"
//#include "spifunc.h"
#include "main.h"
#include "uiflow.h"
#include "setmode.h"
#include "asicreg.h"
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#ifdef DISK_DRV_SPI

#define DEBUG 0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define CLK_DRIVING_HIGH_SET	//\
//		XBYTE[0x2048] = (XBYTE[0x2048] & ~0x30) | 0x10;

#define CLK_DRIVING_DEFAULT_SET	//\
//		XBYTE[0x2048] = (XBYTE[0x2048] & ~0x30) | 0x10;
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
UINT8 HAL_SpiDram(UINT32 dramAddr, UINT16 nByte1, UINT8 read);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = TRUE / FALSE.
 * @see       function X
 *************************************************************************/
static UINT8 
sfQuadEableStsGet(
	UINT8 cmd, 
	UINT8 checker
)
{
	UINT8 sts;
	UINT32 retry = 100000;

	while(retry --){
		SPI_CE_LO;
		HAL_SPIWritePort(cmd);
		sts = HAL_SPIReadPort();
		SPI_CE_HI;
		
		//printf("qe:%bx\n", sts);
		if((sts & checker) == checker)
			return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return QE sts
 * @see       function X
 *************************************************************************/
static UINT8 
sfQuadEable(
	UINT8 en
)
{
	UINT8 sts0, sts1;
	UINT8 ret = en;

	switch(g_spiModeSel.makerId){
		case SPI_MAKER_MXIC:
			if(en){
				SPI_CE_LO;
				HAL_SPIWritePort(SPI_CMD_RD_STS);
				sts0 = HAL_SPIReadPort();
				SPI_CE_HI;
				if((sts0 & 0x40) != 0x40){
					sts0 |= 0x40; 		//enable QE
					
					SPI_WRITE_EN;
					SPI_CE_LO;
					HAL_SPIWritePort(SPI_CMD_WRT_STS); 
					HAL_SPIWritePort(sts0);
					SPI_CE_HI;				

					ret = sfQuadEableStsGet(SPI_CMD_RD_STS, 0x40);
				}
			}
			break;
			
		case SPI_MAKER_ESMT:
		case SPI_MAKER_WINBOND:
		case SPI_MAKER_SPANSION:
		case SPI_MAKER_GD:
			if(en){
				SPI_CE_LO;
				HAL_SPIWritePort(SPI_CMD_RD_STS);
				sts0 = HAL_SPIReadPort();
				SPI_CE_HI;
				
				SPI_CE_LO;
				HAL_SPIWritePort(0x35);
				sts1 = HAL_SPIReadPort();
				SPI_CE_HI;

				if((sts1 & 0x02) != 0x02){
					sts1 |= 0x02; 		//enable QE
					SPI_WRITE_EN;
					SPI_CE_LO;
					HAL_SPIWritePort(SPI_CMD_WRT_STS); 
					HAL_SPIWritePort(sts0);
					HAL_SPIWritePort(sts1);
					SPI_CE_HI;

					ret = sfQuadEableStsGet(0x35, 0x02);
				}
			}
			break;
			
		case SPI_MAKER_EON: // not support Q program???
			break;
		case SPI_MAKER_SST:						
			if(en){
				sfSimpleCmd(0x38);/* enable Quad */
				SPI_4BIT_MODE_SET
			}
			else{		
				if(g_spiModeSel.memType== 0x26){
					break;
				}
					
				sfSimpleCmd(0xFF);/* disable Quad */
				SPI_1BIT_MODE_SET;
			}
			break;
		default: 
			ret = FAIL;
			break;
	}
	
	return ret; 
}


static void 
sfPPCmd2Port(
	UINT16 pageAddr, 
	UINT8 isQE
)
{
	if(isQE){
//printf("w4");
		switch(g_spiModeSel.makerId){
			case SPI_MAKER_MXIC:
				HAL_SPIWritePort(0x38);                 /*4PP*/
				SPI_4BIT_MODE_SET;                   //buswidth
				HAL_SPIWritePort(pageAddr >> 8);	/* A23~A16 */
				HAL_SPIWritePort(pageAddr);	/* A15~A08 */
				HAL_SPIWritePort(0);			/* A07~A00 */
				break;
				
			case SPI_MAKER_ESMT:
			case SPI_MAKER_WINBOND:
			case SPI_MAKER_SPANSION:
				HAL_SPIWritePort(0x32);                 /*4PP*/				
				HAL_SPIWritePort(pageAddr >> 8);	/* A23~A16 */
				HAL_SPIWritePort(pageAddr);	/* A15~A08 */
				HAL_SPIWritePort(0);			/* A07~A00 */				
				SPI_4BIT_MODE_SET;                   //buswidth
				break;
				
			case SPI_MAKER_EON:				
			case SPI_MAKER_SST:
			case SPI_MAKER_GD:
				HAL_SPIWritePort(0x02);                 /*4PP*/
				HAL_SPIWritePort(pageAddr >> 8);	/* A23~A16 */
				HAL_SPIWritePort(pageAddr);	/* A15~A08 */
				HAL_SPIWritePort(0);			/* A07~A00 */
				break;
				
			default:
				break;
		}
	}
	else{
//printf("w1");
		HAL_SPIWritePort(SPI_CMD_PAGE_PROG);		
		HAL_SPIWritePort(pageAddr >> 8);	/* A23~A16 */
		HAL_SPIWritePort(pageAddr); /* A15~A08 */
		HAL_SPIWritePort(0);			/* A07~A00 */
	}
}
/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
static void
sfDramPageDmaWrite(
	UINT16 pageAddr,
	UINT16 pageNum,
	UINT32 dramAddr
)
{
	UINT8 isQE = FALSE;

	//printf("sfDramPageDmaWrite\n");
	
	while (pageNum) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_WRITE_EN;
		SPI_CE_LO;
		sfPPCmd2Port(pageAddr, isQE);

		if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){
			XBYTE[REG_Dma_DmaIdle] = 0x01;	// reset DMA machine
			XBYTE[REG_Dma_DmaIdle] = 0x00;
			XBYTE[REG_Dma_DmaSrcDst] = (2 << 4) | 0;	//DMA FROM DRAM TO SPI */
			XBYTE[REG_Dma_DmaSizeL] = 0xff;	/* size = 512//256 bytes */
			XBYTE[REG_Dma_DmaSizeM] = 0;
			XBYTE[REG_Dma_DmaCmp] = 0x01;
			HAL_DmaSetDramAddr(dramAddr);
			XBYTE[REG_Dma_DmaStart] = 0x01;	// trigger DMA
			while (XBYTE[REG_Dma_DmaCmp] == 0)	// wait for DMA complete
			{
				//printf("p");
			}
			XBYTE[REG_Dma_DmaCmp] = 0x01;
		}
		else{
			HAL_SpiDram(dramAddr, 256, 0);
		}
		dramAddr += 128;
			
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);			
			if(g_spiModeSel.makerId != SPI_MAKER_SST || g_spiModeSel.memType != 0x26){// sst26 only 4bit write
				SPI_1BIT_MODE_SET;			
			}
		}
		isQE = FALSE;

		sfIsBusy(PAGE_WRITE_TIMEOUT);
		
		pageNum --;
		pageAddr ++;
	}
}

static void
sfUsbPageDmaWrite(
	UINT16 pageAddr,
	UINT16 pageNum
)
{
	UINT8 i;
	UINT8 isQE = FALSE;

	while (pageNum) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}
		
		SPI_WRITE_EN;
		SPI_CE_LO;
		sfPPCmd2Port(pageAddr, isQE);
                

		HAL_DmaDo(4,2,256,1,0);
		if (G_bUSBSpeed == 0) {
			for (i = 0; i < 4; i++) {
				while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
					printf("1 %bx\n",XBYTE[K_USB_CLASS_OUT_BUF_SIZE]);
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
				//patch4.5@richie@msdc plug out exception
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)){  //wait pc ack
					printf("REG_Usb_EpAckIntSts %bx\n",XBYTE[0x2500]);
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
		
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
			SPI_1BIT_MODE_SET;
		}
		isQE = FALSE;

		sfIsBusy(PAGE_WRITE_TIMEOUT);
		
		pageNum --;
		pageAddr ++;
	}
}

#if 0 // dma not support
static void
sfSramPageDmaWrite(
	UINT16 pageAddr,
	UINT16 pageNum,
	UINT32 byteAddr
)
{
	UINT8 isQE = FALSE;

	while (pageNum) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_WRITE_EN;
		SPI_CE_LO;
		sfPPCmd2Port(pageAddr, isQE);                

            HAL_DmaSetSramAddr(byteAddr);
            HAL_DmaDo(1,2,256,0,0);
            byteAddr += 256;
		
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
			SPI_1BIT_MODE_SET;
        }
		isQE = FALSE;

		sfIsBusy(PAGE_WRITE_TIMEOUT);
		
		pageNum --;
		pageAddr ++;
	}
}
#endif

static void
sfDramPageIOWrite(
	UINT16 pageAddr,
	UINT16 pageNum,
	UINT32 addr
)
{
	UINT8 i;
	UINT8 tmp0, tmp1;
	UINT8 isQE = FALSE;

	M_DRAM_SetStartAddress((UINT32) addr,1);
	while (pageNum) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_WRITE_EN;
		SPI_CE_LO;
		sfPPCmd2Port(pageAddr, isQE);

		for (i = 0; i < 256 / 2; i++)	/* 256 bytes/page */ {
			M_DRAM_ReadWord(tmp0,tmp1);
			HAL_SPIWritePort(tmp0);
			HAL_SPIWritePort(tmp1);
		}
		
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
			SPI_1BIT_MODE_SET;
             }
		isQE = FALSE;

		sfIsBusy(PAGE_WRITE_TIMEOUT);

		pageNum --;
		pageAddr ++;
	}
}

static void
sfDramAAIWrite(
	UINT32 byteAddr,
	UINT16 bytes,
	UINT32 dramAddr
)
{
	UINT16 wordCnt; 
	UINT8 tmp0, tmp1;

	//printf("sfDramAAIWrite\n");
	
	wordCnt = bytes >> 1;

	M_DRAM_SetStartAddress((UINT32) dramAddr,1);
	M_DRAM_ReadWord(tmp0,tmp1);

	if (wordCnt) {	
		sfSimpleCmd(0x50); 	//disable lock
		sfStatusWrite(0x0);
				
		SPI_WRITE_EN;
		sfIsBusy(PAGE_WRITE_TIMEOUT);
		
		SPI_CE_LO;
		HAL_SPIWritePort(SPI_CMD_AAI_PROG);                
		HAL_SPIWritePort(byteAddr >> 16);	/* A23~A16 */
		HAL_SPIWritePort(byteAddr >> 8);	/* A15~A08 */
		HAL_SPIWritePort(byteAddr);			/* A07~A00 */

		HAL_SPIWritePort(tmp0);
		HAL_SPIWritePort(tmp1);	
		SPI_CE_HI;
		
		wordCnt --;
				
		M_DRAM_ReadWord(tmp0,tmp1);
		
		sfIsBusy(PAGE_WRITE_TIMEOUT);
			
		while(wordCnt){						
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_AAI_PROG);
			
			HAL_SPIWritePort(tmp0);
			HAL_SPIWritePort(tmp1); 
			SPI_CE_HI;

			wordCnt --;	
			
			M_DRAM_ReadWord(tmp0,tmp1);
			
			sfIsBusy(PAGE_WRITE_TIMEOUT);
		}
		SPI_WRITE_DIS;		
	}
}

static void
sfDramByteWrite(
	UINT32 byteAddr,
	UINT16 bytes,
	UINT32 dramAddr
)
{
	UINT8 tmp0, tmp1;
	UINT8 read;
	
	//printf("sfDramByteWrite\n");

	M_DRAM_SetStartAddress((UINT32) dramAddr,1);
	M_DRAM_ReadWord(tmp0,tmp1);
	read = 1;
	
	sfSimpleCmd(0x50);	//disable lock
	sfStatusWrite(0x0);
	
	while (bytes) {
		SPI_WRITE_EN;
		sfIsBusy(PAGE_WRITE_TIMEOUT);

		SPI_CE_LO;
		HAL_SPIWritePort(SPI_CMD_PAGE_PROG); 			   
		HAL_SPIWritePort(byteAddr >> 16);	/* A23~A16 */
		HAL_SPIWritePort(byteAddr >> 8);	/* A15~A08 */
		HAL_SPIWritePort(byteAddr); 		/* A07~A00 */

		HAL_SPIWritePort(tmp0);
		SPI_CE_HI;
		
		bytes --;
		byteAddr ++;
				
		if(read){
			tmp0 = tmp1;
		}
		else{
			M_DRAM_ReadWord(tmp0,tmp1);
		}
		read = !read;
				
		sfIsBusy(PAGE_WRITE_TIMEOUT);		
	}
	SPI_WRITE_DIS;		
}

static void
sfSramPageIOWrite(
	UINT16 pageAddr,
	UINT16 pageNum,
	UINT16 sramAddr
)
{
	UINT8 isQE = FALSE;
	UINT8 i; 

	//printf("sfSramPageIOWrite\n");
	
	while (pageNum) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_WRITE_EN;
		SPI_CE_LO;
		sfPPCmd2Port(pageAddr, isQE); 

		i = 0;
		while(1){
			HAL_SPIWritePort(XBYTE[sramAddr]);
			sramAddr ++;
			i ++;
			if(i == 0)
				break;
		}
		
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
			if(g_spiModeSel.makerId != SPI_MAKER_SST || g_spiModeSel.memType != 0x26){// sst26 only 4bit write
				SPI_1BIT_MODE_SET;
			}			
		}
		isQE = FALSE;

		sfIsBusy(PAGE_WRITE_TIMEOUT);

		pageNum --;
		pageAddr ++;
	}
}

static void
sfSramAAIWrite(
	UINT32 byteAddr,
	UINT16 bytes,
	UINT16 addr
)
{
	UINT16 wordCnt; 

	//printf("sfSramAAIWrite\n");
	
	wordCnt = bytes >> 1;

	if (wordCnt) {
		sfSimpleCmd(0x50); 	//disable lock
		sfStatusWrite(0x0);
				
		SPI_WRITE_EN;
		sfIsBusy(PAGE_WRITE_TIMEOUT);

		SPI_CE_LO;
		HAL_SPIWritePort(SPI_CMD_AAI_PROG);                
		HAL_SPIWritePort(byteAddr >> 16);	/* A23~A16 */
		HAL_SPIWritePort(byteAddr >> 8);	/* A15~A08 */
		HAL_SPIWritePort(byteAddr);			/* A07~A00 */

		HAL_SPIWritePort(XBYTE[addr ++]);
		HAL_SPIWritePort(XBYTE[addr ++]);		
		SPI_CE_HI;
		
		wordCnt --;
				
		sfIsBusy(PAGE_WRITE_TIMEOUT);
			
		while(wordCnt){			
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_AAI_PROG);
			
			HAL_SPIWritePort(XBYTE[addr ++]);
			HAL_SPIWritePort(XBYTE[addr ++]);
			SPI_CE_HI;

			wordCnt --;	
			
			sfIsBusy(PAGE_WRITE_TIMEOUT);
		}
		SPI_WRITE_DIS;		
	}
}

static void
sfSramByteWrite(
	UINT32 byteAddr,
	UINT16 bytes,
	UINT16 addr
)
{
	//printf("sfSramByteWrite\n");

	while (bytes) {
		sfSimpleCmd(0x50);	//disable lock
		sfStatusWrite(0x0);

		SPI_WRITE_EN;
		sfIsBusy(PAGE_WRITE_TIMEOUT);

		SPI_CE_LO;
		HAL_SPIWritePort(SPI_CMD_PAGE_PROG); 			   
		HAL_SPIWritePort(byteAddr >> 16);	/* A23~A16 */
		HAL_SPIWritePort(byteAddr >> 8);	/* A15~A08 */
		HAL_SPIWritePort(byteAddr); 		/* A07~A00 */

		HAL_SPIWritePort(XBYTE[addr ++]);
		SPI_CE_HI;
		
		bytes --;
		byteAddr ++;
				
		sfIsBusy(PAGE_WRITE_TIMEOUT);			
	}
	SPI_WRITE_DIS;		
}

static void
sfRWAddrLatch(
	UINT32 phyAddr
)
{
	XBYTE[REG_Fm_SPIpioWdata] = READ8(phyAddr, 1);  /* A23~A16 */
	while (XBYTE[REG_Fm_SPIbusy] & 0x01);
	
	XBYTE[REG_Fm_SPIpioWdata] = READ8(phyAddr, 2);  /* A15~A08 */
	while (XBYTE[REG_Fm_SPIbusy] & 0x01);	
	
	XBYTE[REG_Fm_SPIpioWdata] = READ8(phyAddr, 3);  /* A07~A00 */
	while (XBYTE[REG_Fm_SPIbusy] & 0x01);	
}

/**************************************************************************
 * @name	     
 * @brief	 	
 * @param
 * @retval
 *************************************************************************/
static void 
sfReadCmd2Port(
	UINT32 byteAddr, 
	UINT8 isQE
)
{
	if(isQE){
//printf("r4");
		CLK_DRIVING_HIGH_SET;	//6mA
		switch(g_spiModeSel.makerId){
			case SPI_MAKER_SST:
				HAL_SPIWritePort(0x0B);
				sfRWAddrLatch(byteAddr);
				HAL_SPIWritePort((UINT8)0);
				
				break;
								
			default:
				HAL_SPIWritePort(SPI_CMD_4READ);
				SPI_4BIT_MODE_SET;                   //buswidth

				sfRWAddrLatch(byteAddr);
				HAL_SPIWritePort((UINT8)0);
				HAL_SPIWritePort((UINT8)0);
				HAL_SPIWritePort((UINT8)0);
				
				break;
		}	
	}
	else if(g_spiModeSel.bitMode == SPI_2BIT_MODE){
//printf("r2");
		CLK_DRIVING_HIGH_SET;	//6mA
		if(g_spiModeSel.memType == 0x30){//for winbond ef30xx, not support "BB" command
	        HAL_SPIWritePort(0x3B);                 /*2 I/O READ*/
			sfRWAddrLatch(byteAddr);
	        HAL_SPIWritePort(0x00);                 /*dummy clock*/
	        SPI_2BIT_MODE_SET;                   //buswidth
		}
		else{
	        HAL_SPIWritePort(SPI_CMD_2READ);                 /*2 I/O READ*/
	        SPI_2BIT_MODE_SET;                   //buswidth
	        
			sfRWAddrLatch(byteAddr);

	        HAL_SPIWritePort(0x00);                 /*dummy clock*/
		}
	}
	else{
//printf("r1");
		HAL_SPIWritePort(SPI_CMD_READ); 
		
		sfRWAddrLatch(byteAddr);
	}
}
/**************************************************************************
 * @name	     
 * @brief	 read spi to dram by DMA mode	
 * @param	[in] phyByteAddr
 * @param	[in] bytes
 * @param	[in] destination dram address in word
 * @retval
 *************************************************************************/
static void
sfDramDmaRead(
	UINT32 phyByteAddr,
	UINT32 dmaSize,
	UINT32 dmaCnt,
	UINT32 addr
)
{
	UINT8 isQE = FALSE;

	UINT8 tmp0, tmp1; 
	UINT32 bufOffset;

	dmaSize--;
	tmp0 = READ8(dmaSize, 3);
	tmp1 = READ8(dmaSize, 2);;
	dmaSize++;
	
	bufOffset = (dmaSize >> 1);

	if (dmaCnt) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}
		
		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);

		//do dma
		if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){
			while(dmaCnt){			
				XBYTE[REG_Dma_DmaIdle] = 0x01;	// reset DMA machine
				XBYTE[REG_Dma_DmaIdle] = 0x00;
				XBYTE[REG_Dma_DmaSrcDst] = (0 << 4) | 2;	//DMA FROM SPI TO DRAM */
				XBYTE[REG_Dma_DmaSizeL] = tmp0;	/* size = 512 bytes */
				XBYTE[REG_Dma_DmaSizeM] = tmp1;
				XBYTE[REG_Dma_DmaCmp] = 0x01;
		                
				HAL_DmaSetDramAddr(addr);
				XBYTE[REG_Dma_DmaStart] = 0x01;	// trigger DMA
				while (XBYTE[REG_Dma_DmaCmp] == 0)	// wait for DMA complete
				{
					//printf("r");
				}
				XBYTE[REG_Dma_DmaCmp] = 0x01;
				
				addr += bufOffset;//256;
				dmaCnt --;			
			}
		}
		else{			
			while(dmaCnt){			
				HAL_SpiDram(addr, dmaSize, 1);
				addr += bufOffset;//256;
				dmaCnt --;			
			}
		}
		
		SPI_CE_HI;
		CLK_DRIVING_DEFAULT_SET;	//4mA
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}
}

static void
sfUsbDmaRead(
	UINT32 phyByteAddr,
	UINT32 dmaSize,
	UINT32 dmaCnt
)
{
	UINT8 isQE = FALSE;
	UINT32 i;

	if (dmaCnt) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}
		
		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);
			
		//do dma
		HAL_DmaDo(2, 4, dmaSize * dmaCnt, 1, 1);
		if (G_bUSBSpeed == 0) {
			dmaCnt = (8 * dmaCnt);
			for (i = 0; i < dmaCnt; i++) {
				while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait for 0 in bulk buffer
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
		} 
		else {
			for( i = 0; i < dmaCnt ; i++){
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
		SPI_CE_HI;
		CLK_DRIVING_DEFAULT_SET;	//4mA
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}
}

#if 0 
// not support dma
static void
sfSramDmaRead(
	UINT32 phyByteAddr,
	UINT32 dmaSize,
	UINT32 dmaCnt,
	UINT16 sramAddr
)
{
	UINT8 isQE = FALSE;

	if (dmaCnt) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}
		
		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);

		//do dma
		HAL_DmaSetSramAddr((UINT16)sramAddr);
		HAL_DmaDo(2,1,dmaCnt * dmaSize,0,0);
			
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}
}

/**************************************************************************
 * @name	     
 * @brief	 read spi to dram by PIO mode	
 * @param	[in] phyByteAddr
 * @param	[in] bytes
 * @param	[in] destination dram address in word
 * @retval
 *************************************************************************/
//no needed
static void
sfDramIORead(
	UINT32 phyByteAddr,
	UINT32 bytes,
	UINT32 addr
)
{
	UINT8 isQE = FALSE;
	UINT8 tmp0, tmp1;

	M_DRAM_SetStartAddress((UINT32) addr,0);
	
	if (bytes) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);

		while(bytes) {
			tmp0 = HAL_SPIReadPort();
			tmp1 = HAL_SPIReadPort();

			M_DRAM_WriteWord(tmp0,tmp1);
			bytes -= 2;
		}
		
		SPI_CE_HI;
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}
}
#endif

static void
sfSramDmaRead(
	UINT32 phyByteAddr,
	UINT32 bytes
)
{
	UINT8 isQE = FALSE;

	if (bytes) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);

		HAL_DmaSetSramAddr(0x00);
		HAL_DmaDo(2, 1, bytes, 0, 0);
		
		SPI_CE_HI;
		CLK_DRIVING_DEFAULT_SET;	//4mA
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}	
}

/**************************************************************************
 * @name	     
 * @brief	 read spi to sram by PIO mode	
 * @param	[in] phyByteAddr
 * @param	[in] bytes
 * @param	[in] destination sram address in byte
 * @retval
 *************************************************************************/
static void
sfSramIORead(
	UINT32 phyByteAddr,
	UINT32 bytes,
	UINT16 addr
)
{
	UINT8 isQE = FALSE;
	UINT32 i;

	if (bytes) {
		if(g_spiModeSel.bitMode == SPI_4BIT_MODE){
			isQE = sfQuadEable(1);
		}

		SPI_CE_LO;		
		sfReadCmd2Port(phyByteAddr, isQE);

		for(i = 0; i < bytes; i++){
			XBYTE[addr + i] = HAL_SPIReadPort();
		}
		
		SPI_CE_HI;
		CLK_DRIVING_DEFAULT_SET;	//4mA
		
		if(isQE){
			sfQuadEable(0);
		}
		SPI_1BIT_MODE_SET;
	}
}

/**************************************************************************
 * @name	sfPhySectorW     
 * @brief		
 * @param     [in]	startSector: start sector addres
 * @param     [in]	dmaCnt: count of dma
 * @param     [in]	dmaSize: size of one dma
 * @param     [in]	buf: source address, it is a word address in K_SPI2DRAM, 
 * @			it is a byte address in K_SPI2SRAM mode, it is no use in K_SPI2USB mode
 * @param     [in] mode: source mode, K_SPI2DRAM/K_SPI2USB/K_SPI2SRAM
 * @retval    bytes of written
 * @see
 *************************************************************************/
UINT32 
sfPhySectorW(
	UINT32 startSector, 
	UINT16 dmaCnt,
	UINT32 dmaSize, 
	UINT32 buf, 
	UINT32 mode
)
{
	UINT16 pageAddr;
	UINT16 pageNum;
	UINT32 byteAddr;
	UINT32 byteNum;
	spiFlowMode_t flowMode;

	if(dmaCnt){
		byteAddr = startSector << 9;
		byteNum = (UINT32) dmaCnt * dmaSize;
		pageAddr = byteAddr >> PAGE_SIZE_B;
		pageNum = (UINT16)(byteNum >> PAGE_SIZE_B); 
		flowMode = g_spiModeSel.flowMode & SPI_FLOW_WR_MASK;

		if( mode == K_DRAM2SPI ){
			if(flowMode == SPI_FLOW_PAGE_DMA){
				sfDramPageDmaWrite(pageAddr, pageNum, buf);
			}
			else if(flowMode == SPI_FLOW_PAGE_IO_WRT){
				sfDramPageIOWrite(pageAddr,pageNum, buf);
			}
			else if(flowMode == SPI_FLOW_AAI_WRT){
				sfDramAAIWrite(byteAddr, byteNum, buf);
			}
			else{//SPI_FLOW_BYTE_WRT
				sfDramByteWrite(byteAddr, byteNum, buf);
			}
		}
		else if(mode == K_USB2SPI){
			sfUsbPageDmaWrite(pageAddr, pageNum);
		}
		else{//K_SRAM2SPI
			if(buf == 0){
				buf = 0x3800;
			}

			if(!flowMode || flowMode == SPI_FLOW_PAGE_IO_WRT){ // default, storage not support sram dma??
				sfSramPageIOWrite(pageAddr, pageNum, (UINT16)buf);
			}
			else if(flowMode == SPI_FLOW_AAI_WRT){				
				sfSramAAIWrite(byteAddr, byteNum, (UINT16)buf);
			}
			else{//SPI_FLOW_BYTE_WRT
				sfSramByteWrite(byteAddr, byteNum, (UINT16)buf);
			}
		}

	}

	return 	(((UINT32)pageNum) << PAGE_SIZE_B);
}

/**************************************************************************
 * @name	sfPhySectorR     
 * @brief		
 * @param     [in]	startSector: start sector addres
 * @param     [in]	dmaCnt: count of dma
 * @param     [in]	dmaSize: size of one dma
 * @param     [in]	buf: dstination address, it is a word address in K_SPI2DRAM, 
 * @			it is a byte address in K_SPI2SRAM mode, it is no use in K_SPI2USB mode
 * @param     [in] mode: dstination mode, K_SPI2DRAM/K_SPI2USB/K_SPI2SRAM
 * @retval    bytes of read 
 * @see
 *************************************************************************/
UINT32 
sfPhySectorR(
	UINT32 startSector, 
	UINT16 dmaCnt,
	UINT32 dmaSize, 
	UINT32 buf, 
	UINT32 mode
)
{
	UINT32 byteAddr;
	UINT32 bytes;

	if(dmaCnt){
		byteAddr = startSector << 9;
		bytes = ((SINT32) dmaCnt * dmaSize);

		if(mode == K_SPI2DRAM){
			sfDramDmaRead(byteAddr, dmaSize, dmaCnt, buf);
		}
		else if(mode == K_SPI2USB){
			sfUsbDmaRead(byteAddr, dmaSize, dmaCnt);
		}
		else{ //K_SPI2SRAM 
#if 0
			if(buf == 0){
				buf = 0x3800;
			}
			sfSramIORead(byteAddr, bytes, (UINT16)buf); // only from dram and sram, buf is zero if from sram
#else
			sfSramDmaRead(byteAddr, bytes);
#endif
		}
	}
	return 	bytes;
}
#include "sys_mem_init.h"
void 
sfPhySectorSramR(
	UINT32 phyAddr, 
	UINT16 sramAddr,
	UINT32 sizeByte,
	UINT32 mode
)
{
	UINT16 tmpSize;
	UINT32 dramAddr;

	tmpSize = ((UINT16)phyAddr) & (512 - 1);
	if (!tmpSize) {
		sfPhySectorR(phyAddr >> 9, 1, 512, 0, 0);
	} else {
		phyAddr &= ~(512 - 1);
		dramAddr = sysMemSizeGet(MEM_IDX_TEXT) + ((0xc000 - 0x4000) >> 1);
		HAL_DmaSetDramAddr(dramAddr);
		sfPhySectorR(phyAddr >> 9, 2, 512, dramAddr, 1);
		tmpSize = 512 - tmpSize;
		if (!(tmpSize & 0x01)) {
			HAL_DmaSetSramAddr(sramAddr);
			HAL_DmaSetDramAddr(dramAddr + (tmpSize >> 1));
			HAL_DmaDo(0, 1, 512, 0, 0);
		} else {
			XBYTE[0x3800 + sramAddr++] = G_FPGAMemPool[tmpSize++];
			HAL_DmaSetSramAddr(sramAddr);
			HAL_DmaSetDramAddr(dramAddr + (UINT32)(tmpSize >> 1));
			HAL_DmaDo(0, 1, 512-2, 0, 0);
			sramAddr += 512-2;
			tmpSize += 512-2;
			XBYTE[0x3800 + sramAddr] = G_FPGAMemPool[tmpSize];
		}
	}

	sizeByte = sizeByte;
	mode = mode;
}

#if 1 // for test
void 
sfStatusRegRead(
	UINT8 *pSts0,
	UINT8 *pSts1
)
{
	UINT8 sts0, sts1;

	switch(g_spiModeSel.makerId){
		case SPI_MAKER_MXIC:
		case SPI_MAKER_SST:
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_RD_STS);
			sts0 = HAL_SPIReadPort();
			SPI_CE_HI;
			printf("sts: %bx\n", sts0);
			break;
			
		case SPI_MAKER_WINBOND:
		case SPI_MAKER_SPANSION:
		case SPI_MAKER_GD:
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_RD_STS);
			sts0 = HAL_SPIReadPort();
			SPI_CE_HI;
			
			SPI_CE_LO;
			HAL_SPIWritePort(0x35);
			sts1 = HAL_SPIReadPort();
			SPI_CE_HI;

			printf("sts: %bx, %bx\n", sts0, sts1);
			break;
						
		default: 
			break;
	}

	*pSts0 = sts0;
	*pSts1 = sts1;
}

void 
sfStatusRegWrite(
	UINT8 sts0,
	UINT8 sts1
)
{

	switch(g_spiModeSel.makerId){
		case SPI_MAKER_MXIC:
		case SPI_MAKER_SST:
			SPI_WRITE_EN;
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_WRT_STS); 
			HAL_SPIWritePort(sts0);
			SPI_CE_HI;
			
			printf("%bx ->sts\n", sts0);
			
			break;
			
		case SPI_MAKER_ESMT:
			sfSimpleCmd(0x50);				
		case SPI_MAKER_WINBOND:
		case SPI_MAKER_SPANSION:
		case SPI_MAKER_GD:
			SPI_WRITE_EN;
			SPI_CE_LO;
			HAL_SPIWritePort(SPI_CMD_WRT_STS); 
			HAL_SPIWritePort(sts0);
			HAL_SPIWritePort(sts1);
			SPI_CE_HI;

			printf("%bx, %bx ->sts\n", sts0, sts1);
			break;
						
		default: 
			break;
	}
	
}
#endif

UINT8 HAL_SpiDram(UINT32 dramAddr, UINT16 nByte1, UINT8 read)
{
    UINT32 timeout_count = 0xFFFF;
    UINT8 status;
    
#if DRAM_ASSERT
    if (dramAddr <= 0x40000){
    printf("@ASSERT %s %d %lx\n",__FILE__, (UINT16)__LINE__, tmp1);
    while(1);
    }
#endif
//    if(read)
//        printf("R...%u, %bx\n", nByte1, XBYTE[REG_Fm_evt]);
//    else
//        printf("W...%u\n", nByte1);

    ENTER_CRITICAL(status);

    XBYTE[REG_Fm_evt] = 0x20;                      //clr dma done evt 

    if(read)
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x01;                       //read mode
    else 
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x00;                       //write mode

    nByte1 --;
    XBYTE[REG_Fm_drambytecnt_Low] = READ8(nByte1, 1);   
    XBYTE[REG_Fm_drambytecnt_High] = READ8(nByte1, 0);

    XBYTE[REG_Fm_datstraddr_0] = READ8(dramAddr, 3);
    XBYTE[REG_Fm_datstraddr_1] = READ8(dramAddr, 2);
    XBYTE[REG_Fm_datstraddr_2] = READ8(dramAddr, 1);
    XBYTE[REG_Fm_datstraddr_3] = READ8(dramAddr, 0)&0x01;

    XBYTE[REG_Fm_dramstr] = 1;  //triger
    while ((XBYTE[REG_Fm_evt] & 0x20) == 0x00)
    {
        if (timeout_count >	0)
        {
            timeout_count--;
        }
        else
        {
            //printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
		printf("HAL_SDReadDMA: %bx, %bx,%bx\n", XBYTE[REG_Fm_evt], XBYTE[REG_Fm_dramwbufptr],XBYTE[REG_Fm_dramrbufptr]);
            EXIT_CRITICAL(status);
            return HALK_ERROR_GENERAL;
        }
    }
 
    EXIT_CRITICAL(status);
    XBYTE[REG_Fm_evt] = 0x20; //clr
    
    return HALK_SUCCESS;
}

#endif

