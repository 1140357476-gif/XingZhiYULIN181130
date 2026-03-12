/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        sdramhw.c

Abstract:

        Module related to hardware access of SDRAM

Environment:

        Keil C51 Compiler

Revision History:

        08/28/2001      Chi-Yeh Tsai    created

--*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "asicreg.h"
#include "hal.h"
#include "hal_dram.h"
#if ( CARD_OPTION == 1 )
#include "doslink.h"
#endif

#include "sdramhw.h"
#include "main.h"
#include "setmode.h"
#include "reg_def.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//=============================================================================
//Program
//=============================================================================
#if 1
//-------------------------------------
//SDRAM_HwInitialize
//-------------------------------------
#ifndef _DBG_CLR_WARNING_
void SDRAM_HwInitialize(void) USING_0
{
        HAL_FlashMode(K_MEDIA_None, K_MEDIA_None, K_MEDIA_PageSize512);
}
#endif /*_DBG_CLR_WARNING_*/
#endif

//-------------------------------------
//SDRAM_Dram2Sram
//-------------------------------------
// WWWW start
//patch4.5@ada@Add timeout count begin
UINT8 SDRAM_Dram2Sram(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz) USING_0
//void SDRAM_Dram2Sram(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz) USING_0
//patch4.5@ada@Add timeout count end
{
        UINT16	i, k;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end
        
        SDRAM_CardSetStartAddress( ramAddr, TRUE );

	#if 1//(K_HAVE_HAL)
	if ((UINT16)sramAddr >= 0x3800)
	{
		i = 0;
		while (siz > 0)
		{
			if (siz >= 1024)
				k = 1024;
			else
				k = siz;
			HAL_DmaSetDramAddr(ramAddr+(i>>1));
	     		HAL_DmaSetSramAddr((UINT16)sramAddr - (UINT16)0x3800 + (UINT16)i); // 0x0c00

			//patch4.5@ada@Add timeout count begin
			error = HAL_DmaDo(0,1,k,0,0); 	

			if (error != 0)
			{
				return FALSE;
			}
			//patch4.5@ada@Add timeout count end

			i += k;
			siz -= k;
		}
	}
	else
	{
		// wyeo@0527, patch 4.2
        	for (i = 0; i < siz; i += 2)
		{
			M_DRAM_ReadWord(sramAddr[i], sramAddr[i+1]);
		}
	}
	#else
		// wyeo@0527, patch 4.2
        	for (i = 0; i < siz; i += 2)
		{
			M_DRAM_ReadWord(sramAddr[i], sramAddr[i+1]);
		}
	#endif

	//patch4.5@ada@Add timeout count begin
	return TRUE;
	//patch4.5@ada@Add timeout count end
}

//-------------------------------------
//SDRAM_Sram2Dram
//-------------------------------------
//patch4.5@ada@Add timeout count begin
UINT8 SDRAM_Sram2Dram(UINT8 *sramAddr, UINT32 ramAddr, UINT16 siz) USING_0
//void SDRAM_Sram2Dram(UINT8 *sramAddr, UINT32 ramAddr, UINT16 siz) USING_0
//patch4.5@ada@Add timeout count end
{
	UINT16	i, k;

	//patch4.5@ada@Add timeout count begin	
	UINT8 error;
	//patch4.5@ada@Add timeout count end
	
        SDRAM_CardSetStartAddress( ramAddr, FALSE );

	#if 1//(K_HAVE_HAL)
	if ((UINT16)sramAddr >= 0x3800)
	{
		i = 0;
		while ( siz > 0 )
		{
			if (siz >= 1024)
				k = 1024;
			else
				k = siz;
	     		HAL_DmaSetSramAddr((UINT16)sramAddr - (UINT16)0x3800 + (UINT16)i);
	     		HAL_DmaSetDramAddr(ramAddr+(i>>1));

				//patch4.5@ada@Add timeout count begin
	     		error = HAL_DmaDo(1,0,k,0,0); 		          

				if (error != 0)
				{
					return FALSE;
				}
				//patch4.5@ada@Add timeout count end

			i += k;
			siz -= k;
		}
	}
	else
	{
		// wyeo@0527, patch 4.2
        	for (i = 0; i < siz; i += 2)
		{
			M_DRAM_WriteWord(sramAddr[i], sramAddr[i+1]);
		}
	}
	#else
		// wyeo@0527, patch 4.2
        	for (i = 0; i < siz; i += 2)
		{
			M_DRAM_WriteWord(sramAddr[i], sramAddr[i+1]);
		}
	#endif

	//patch4.5@ada@Add timeout count begin
	return TRUE;
	//patch4.5@ada@Add timeout count end
}

#if 0 //mark by phil 
void SDRAM_CardSetStartAddress(UINT32 address, UINT8 prefetch) USING_0
{
     	//HAL_DramStartAddrSet(address, prefetch);
	M_DRAM_SetStartAddressV(address, prefetch);

	#if ( CARD_OPTION == 1 )
    G_DOS_SDRAM_NextIntRetAddr = address;      
    if ( prefetch )
		G_File_SDRAM_Read = 1;
	else
		G_File_SDRAM_Read = 0;
	#endif
}
#endif
// WWWW end

#if 0
//-----------------------------------------------------------------------------
//SDRAM_Dram2Usb
//-----------------------------------------------------------------------------
UINT8 SDRAM_Dram2Usb(UINT32 address, UINT16 size) USING_0
{
        UINT8 status = TRUE;
	UINT8 bi;
	UINT16 offset;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end

        G_ucState3 &= 0xfe;                             //FIFO prepare data doing
		
        //cytsai@1218
	//richie@vc0422
        //HAL_DramStartAddrSet(address, K_SDRAM_NoPrefetch);
//	HAL_SetUSBDMA(K_DMA_UsbDontCare,K_USB_AP_DMA_IN);//for ap //BULK 2

////printf("size=%d\n",size);
	offset = 0;
	while (size > 1024)
	{
		HAL_DmaSetDramAddr(address + (offset / 2));

		//patch4.5@ada@Add timeout count begin
	     error = HAL_DmaDo(K_DMA_DRAM, K_DMA_USB, 1024, K_DMA_AsyncRet, K_DMA_NoPadding0s);

		if (error != 0)
		{
			return FALSE;
		}
		//patch4.5@ada@Add timeout count end

	        G_ucState3 |= 0x01;

		//richie@vc0422
		for (bi=0;bi<(1024 / 64);bi++)
		{
////printf("w bi = %bx\n",bi);
			while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk 2 buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in BULK2
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}
		size -= 1024;
		offset += 1024;
	}

	if (size > 0)
	{
		HAL_DmaSetDramAddr(address + (offset / 2));

		//patch4.5@ada@Add timeout count begin
	     error = HAL_DmaDo(K_DMA_DRAM, K_DMA_USB, size, K_DMA_AsyncRet, K_DMA_NoPadding0s);

		if (error != 0)
		{
			return FALSE;
		}
		//patch4.5@ada@Add timeout count end

	        G_ucState3 |= 0x01;

		//richie@vc0422
		for (bi=0;bi<(size / 64);bi++)
		{
////printf("i bi = %bx\n",bi);
			while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk 2 buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in BULK2
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}


		if ((size % 64) != 0)
		{
			while (XBYTE[K_USB_AP_IN_BUF_SIZE] < (size % 64)) ;  //wait for 64 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in BULK2
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}
	}


	//richie@vc0422
        //status = HAL_CheckReadData2UsbDmaDone(size);

        G_ucState3 &= 0xf0;

        return status;
}
#endif

