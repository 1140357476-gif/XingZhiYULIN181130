//patch4.2@richie@ds0503
/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        dramcore.c

Abstract:

        Module uniquely belong to dram access and charateristic

Environment:

        Keil C51 Compiler

Revision History:

        05/03/2002      Richie lee    created                 

--*/
//=============================================================================
//Header file
//=============================================================================

#include "general.h"
#include "dbg_def.h"
#include "hal.h"
#include "setmode.h"
#include "doslink.h"
#include "dosvar.h"
#include "main.h"
#include "dramfunc.h"
#include "initio.h"
#include "sdramhw.h"
//#include "sidcinit.h"
#include "mtd_macro_api.h"
#include "hal_gprm.h"

#include "dos32.h"
#include "reg_def.h"
#define __FILE		__FILE_ID_DOSRAM__
#define	DEBUG	0

xdata	UINT32	G_DosRamTotalSize = 0;
xdata	UINT32	G_ulDRAMSTORFAT1AddrTemp = 0;
xdata	UINT32	G_DosRamStartAddr = 0;

void DRAM_DosRamBufGet(UINT32 *startAddr, UINT32 *size)
{
	*startAddr = G_DosRamStartAddr;
	*size = (G_DosRamTotalSize >> 1);
}

#if ( DOSRAM_OPTION == 1 )

extern xdata UINT16 G_Card_Supported;
xdata UINT32 DOSRAM_PhySectors;
//patch4.5@richie@msdc plug out exception
#include "uiflow.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//xdata	UINT32	G_ulDRAMSTORLogicalZeroAddr;
//patch4.4@richie@dsi begin
//add variable for dram total size boundary
//xdata	UINT32	G_ulDRAMTotalSize = 0;
//xdata	UINT32	G_ulDRAMSTORFAT1AddrTemp;
//patch4.4@richie@dsi end
//=============================================================================
//Program       
//=============================================================================
//-----------------------------------------------------------------------------
//DRAM_CardDetect
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kDosRamBufSet(UINT32 size)
 * @brief     Set size of DOS Ram.
 * @param     [in] size	DOS Ram size in word units.
 * @retval    return = TRUE
 * @see       NULL
*/
UINT8 sp1kDosRamBufSet(UINT32 size)
{
	G_DosRamStartAddr = K_SDRAM_DosRamStartAddr;
	//G_DosRamStartAddr = K_SDRAM_CommonEndAddr;
	G_DosRamTotalSize = size << 1;
	DRAM_ResetMBR(TRUE);
	G_ulDRAMSTORFAT1AddrTemp = 0xFFFFFFFF;
	//printf("G_DosRamStartAddr=%lx, %lx\n",G_DosRamStartAddr, G_DosRamTotalSize);
	return TRUE;
}
	
UINT32 DRAM_StartAddrGet(void)
{
	return (G_DosRamStartAddr << 1);
}

#if 0
void sp1kFlashCacheCtrl(UINT8 on) 
{
	on = on;
}

UINT8 flashCacheCtrlSts(void) 
{
	return 0;
}
#endif

//-----------------------------------------------------------------------------
//DRAM_CardDetect
//-----------------------------------------------------------------------------
UINT8 DRAM_CardDetect(void) USING_0
{
        UINT8 sts = TRUE;

	if( G_DosRamTotalSize == 0 ){
		G_DosRamStartAddr = K_SDRAM_CommonEndAddr;
		G_DosRamTotalSize = (UINT32)(K_SDRAM_TotalSize - K_SDRAM_CommonEndAddr) << 1;
		DRAM_ResetMBR(TRUE);
		G_ulDRAMSTORFAT1AddrTemp = 0xFFFFFFFF;
	}
	if( G_DosRamStartAddr > K_SDRAM_CommonEndAddr ){
		__ASSERT(__FILE, __LINE__, 1);
	}
	//G_Card_SectorPerCard = 2 * 1024 * 10;//4MB
				//patch4.4@richie@dsi
				//add variable G_ulDRAMTotalSize for dram total size boundary
	G_Card_SectorPerCard = (G_DosRamTotalSize >> 9);//((UINT32)G_ulDRAMTotalSize - (UINT32)G_ulDRAMSTORLogicalZeroAddr) >> 8;
//printf("G_Card_SectorPerCard=%lx,=%lx\n",G_Card_SectorPerCard,G_DosRamTotalSize);
	DOSRAM_PhySectors = G_Card_SectorPerCard;
//printf("DOSRAM_PhySectors=%lx\n",DOSRAM_PhySectors);
	if (G_Card_SectorPerCard < 2048000L)		// 1G
	{
		if (G_Card_SectorPerCard <= 32768L)	// 16M
			G_Card_BlockSize = 4096L;	// 4K
		else if (G_Card_SectorPerCard <= 65536L)// 32M
			G_Card_BlockSize = 2048L;	// 2K
		else
			G_Card_BlockSize = 16384L;	// 16K
	}
	else if (G_Card_SectorPerCard < 4096000L)	// 2G
		G_Card_BlockSize = 32768L;		// 32K
	else if (G_Card_SectorPerCard < 6144000L)	// 3G
		// not power of 2, card reader work ?
		G_Card_BlockSize = 49152L;		// 48K
	else	
		// not power of 2, card reader work ?	// 3.xG
		G_Card_BlockSize = 64512L;		// 63K
	
	// bigger cluster size make our avi capture rate better, but...
	// It is stange that the 64M CF card must 2K cluster for card
	// reader to work properly.
	if (G_Card_SectorPerCard > 98304L && G_Card_SectorPerCard <= 131072L)
		// for 64M CF card, the cluster size make 2K
		G_Card_BlockSize = 2048L;

	//Note:block size and cluster size is word
	//G_Card_BlockSize = 2048L;
	//G_Card_BlockSize = 8192L;
	G_Card_Supported |= (((UINT16)1)<<K_DOSRAM_SUPPORTED_BIT);
	
	G_Card_TotalSizeMB = G_Card_SectorPerCard >> 11; 
	
	G_Card_BytePerSector = 512;
	
	return sts;
}

//-----------------------------------------------------------------------------
//DRAM_ResetMBR
//-----------------------------------------------------------------------------
UINT8 DRAM_ResetMBR(UINT8 doDOSInitial) USING_0
{
	UINT8 sts = doDOSInitial;

	sts = DRAM_SectorFillByteData(G_DosRamStartAddr,0x00);

	return sts;
}

//-----------------------------------------------------------------------------
//DRAM_GetBehavior
//-----------------------------------------------------------------------------
UINT16 DRAM_GetBehavior(void) USING_0
{
	G_Media_Behavior = K_MEDIA_DRM_Behavior;
	return G_Media_Behavior;
}

//-----------------------------------------------------------------------------
//DRAM_SectorFillByteData
//-----------------------------------------------------------------------------
UINT8 DRAM_SectorFillByteData(UINT32 sectorAddr,UINT8 bData) USING_0
{
/*
	UINT16 i;
	for (i = 0;i < K_Card_PageSize;i++)
	{
		G_ucStorData[i] = bData;
	}

	HAL_DmaSetSramAddr(0x0c00);
	HAL_DmaSetDramAddr(sectorAddr);
	HAL_DmaDo(K_DMA_SRAM,K_DMA_DRAM,K_Card_PageSize,0,0);
	HAL_CpuSramModeSet(0x00);  
*/
	HAL_GprmDramFill(sectorAddr, K_Card_PageSize, bData);

	return TRUE;
}

//-----------------------------------------------------------------------------
//DRAM_LCard2UsbMore
//-----------------------------------------------------------------------------
UINT8 DRAM_LCard2UsbMore(UINT32 address, UINT32 size) USING_0
{
	UINT8 status = TRUE;
	UINT32 bi;
	UINT8 error;

	MTD_SelfShiftL8(address);// 3.6us
	address = address+ G_DosRamStartAddr;
	//dumpData(512, address, 1);
	if ( size ){

		HAL_DmaSetDramAddr(address);
		error = HAL_DmaDo(K_DMA_DRAM, K_DMA_USB, (UINT16)size<<9, K_DMA_AsyncRet, K_DMA_NoPadding0s);
		if (error != 0)
			return FALSE;

		if (G_bUSBSpeed == 0){
			for(bi= 0; bi < (size << 3); bi++)
			{
				while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 64)	{  //wait for 0	in bulk	buffer
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)) {  //wait pc ack
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}

		}
		else {
			for (bi = 0; bi < size; bi++)
			{
				while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 512)	{  //wait for 0	in bulk	buffer
//printf("4\n");
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)) {  //wait pc ack
//printf("5\n");
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
		}
	}

        return status;
}

//-----------------------------------------------------------------------------
//DRAM_LUsb2CardMore
//-----------------------------------------------------------------------------
UINT8 DRAM_LUsb2CardMore(UINT32 address, UINT32 size) USING_0
{
        UINT8 status = TRUE;
	UINT32 bi;
	UINT8 error;

	MTD_SelfShiftL8(address);// 3.6us
	address = address + G_DosRamStartAddr;
	if ( size ){
		if (G_bUSBSpeed == 0){
			for (bi = 0; bi < (UINT32)(size<<3); bi++)
			{
				HAL_DmaSetDramAddr(address);
				error = HAL_DmaDo(K_DMA_USB, K_DMA_DRAM, 64, K_DMA_AsyncRet, K_DMA_NoPadding0s);
				if (error != 0)
					return FALSE;

				while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
				while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
				address += 32;
			}
		}
		else {
			for (bi = 0; bi < size; bi++)
			{
				HAL_DmaSetDramAddr(address);
				error = HAL_DmaDo(K_DMA_USB, K_DMA_DRAM, 512, K_DMA_AsyncRet, K_DMA_NoPadding0s);
				if (error != 0)
					return FALSE;

				while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
//printf("1\n");
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
				while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
//printf("2");
					if((G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
				}
				XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
				address += 256;
			}
		}

	}

        return status;
}


//-----------------------------------------------------------------------------
//DRAM_LCard2Sram
//-----------------------------------------------------------------------------
UINT8 DRAM_LCard2Sram(UINT32 logAddr) USING_0
{
#if 0
	UINT16 siz = 512;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end

	logAddr = (logAddr / 2) + G_DosRamStartAddr;
    
	HAL_DmaSetSramAddr(0x0c00);
	HAL_DmaSetDramAddr(logAddr);             

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(0,1,siz,0,0); 		          //DMA from DRAM to sram for "size" bytes and wait till end and no nandf

	if (error != 0)
	{
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end
#else
	logAddr = logAddr;
#endif
        return TRUE;          
}

//-----------------------------------------------------------------------------
//DRAM_LSram2Card
//-----------------------------------------------------------------------------
UINT8 DRAM_LSram2Card(UINT32 logAddr) USING_0
{
#if 0
	UINT16 siz = 512;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end

	logAddr = (logAddr / 2) + G_DosRamStartAddr;
    
	HAL_DmaSetSramAddr(0x0c00);
	HAL_DmaSetDramAddr(logAddr);             

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(1,0,siz,0,0); 		          //DMA from CF to sram for "size" bytes and wait till end and no nandf

	if (error != 0)
	{
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end
#else
	logAddr = logAddr;
#endif
        return TRUE;          
}


//-----------------------------------------------------------------------------
//DRAM_LCard2DramMore
//-----------------------------------------------------------------------------
UINT8 DRAM_LCard2DramMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector) USING_0
{
#if DEBUG
	printf("%s %u sector=%lx,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,numSector,dramAddr);
#endif
	logAddr = logAddr + G_DosRamStartAddr;

#if DEBUG
	printf("%s %u sector=%lx,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,numSector,dramAddr);
#endif
	//HAL_DoDRAMDMA((logAddr * 2),(dramAddr * 2),(numSector << 9));
	HAL_GprmDramDma((logAddr << 1), (dramAddr << 1), (numSector << 9), 1);
	
	return TRUE;
}

//-----------------------------------------------------------------------------
//DRAM_LDram2CardMore
//-----------------------------------------------------------------------------
UINT8 DRAM_LDram2CardMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector) USING_0
{
#if DEBUG
	printf("%s %u sector=%lx,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,numSector,dramAddr);
#endif
	logAddr = logAddr + G_DosRamStartAddr;
#if DEBUG
	printf("%s %u sector=%lx,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,logAddr,numSector,dramAddr);
#endif

	//HAL_DoDRAMDMA((dramAddr * 2),(logAddr * 2),(numSector << 9));
	//dumpData(32, dramAddr, 1);
	HAL_GprmDramDma((dramAddr << 1), (logAddr << 1), (numSector << 9), 1);
	
	return TRUE;
}

//for AP upload file
//-----------------------------------------------------------------------------
//DRAM_LCard2Usb
//-----------------------------------------------------------------------------
UINT8 DRAM_LCard2Usb(UINT32 address) USING_0
{
        UINT8 status = TRUE;
	UINT8 bi;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end

	address = (address >> 1) + G_DosRamStartAddr;

	//HAL_SetUSBDMA(K_DMA_UsbDontCare,K_USB_AP_DMA_IN);//for class //BULK 1

	HAL_DmaSetDramAddr(address);

	//patch4.5@ada@Add timeout count begin
     	error = HAL_DmaDo(K_DMA_DRAM, K_DMA_USB, K_Card_PageSize, K_DMA_AsyncRet, K_DMA_NoPadding0s);

	if (error != 0)
	{
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	//richie@vc0422
	for (bi=0;bi<(K_Card_PageSize / 64);bi++)
	{
		while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk 1 buffer
		XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x01;//open bulk in BULK1
		while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
		XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
	}

        return status;
}

//-----------------------------------------------------------------------------
//DRAM_EraseTotalBlock
//-----------------------------------------------------------------------------
UINT8 DRAM_EraseTotalBlock(void) USING_0
{
	return TRUE;
}

//-----------------------------------------------------------------------------
//DRAM_Initialize
//-----------------------------------------------------------------------------
UINT8 DRAM_Initialize(UINT8 doDOSInitial) USING_0
{
        UINT8 sts = FALSE;
	doDOSInitial = TRUE;

	G_DOS_Initial_Status = 1;

	DRAM_CardDetect();
        //M_DEBUG_SHOWLED(0xfe);

	//SETMODE_ClearPostBuffer();           		//clear post buffer 
        //SETMODE_SetUploadMode();            		//set upload mode
		
	if (DOS_Initialize())
	{
               	sts = TRUE;
               	G_DOS_Initial_Status = 0xFF;
                //M_DEBUG_SHOWLED(0xe0);
	}
	else
       		G_DOS_Initial_Status = 5;

	//SETMODE_SetCameraIdle();			// @WYEO, 102401

        return sts;	
}
#endif
