/**************************************************************************
 *  	   Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.	   *
 *  																	   *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *  																	   *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.   	   *
 *  																	   *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.											   *
 *  																	   *
 *  Sunplus mMedia Technology Co., Ltd. 								   *
 *  19-1, Innovation First Road, Science-Based Industrial Park, 		   *
 *  Hsin-Chu, Taiwan, R.O.C.											   *
 *  																	   *
 **************************************************************************/
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "main.h"
#include "setmode.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_nand.h"
#include "reg_def.h"

#if (SMC || NAND)
#include "asicreg.h"

//#include "cardcon.h"
//#include "cardfunc.h"
#include "cardvar.h"

#include "doslink.h"

#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"

//#include "dosusr.h"
#include "mtd_general.h"

#include "disk.h"

#ifdef DISK_DRV_NAND

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
//#include "uiflow.h"

//richie@vc0422
//extern  data 	BIT G_USB_IO_Bulk2;

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//SMC_HwInitialize
//-----------------------------------------------------------------------------
void SMC_HwInitialize(void)
{
	UINT8 size;
#if (SMC)
	UINT8 detected; 	//ada@0218
#endif

	diskInactivate();// patch4.4-9@wyeo@select nand, sdram as default

	if (K_Card_PageSize == 0x0100)
		size = K_MEDIA_PageSize256;  //256 byte/page
	else if (K_Card_PageSize == 0x0200)
		size = K_MEDIA_PageSize512;  //512 byte/page
	else if (K_Card_PageSize == 0x0400)
		size = K_MEDIA_PageSize1024; //1024 byte/page

#if (NAND)
	if (G_Card_Type == K_MEDIA_Nandf)	// WWWW0404
		HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_None, size);
#endif

#if (SMC)
	if (G_Card_Type == K_MEDIA_Smc) 	// WWWW0404
	{
		HAL_CardDetect(&detected);  //ada@0207

		if (detected) {
			HAL_FlashMode(K_MEDIA_Smc, K_MEDIA_Nandf, size);
		}
	}
#endif

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_None, size);
	/*
	if (G_Card_Type == K_MEDIA_Sdf){
		   	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, size);
	}
	*/
	//HAL_NANDInit(K_SMC_ActiveCycle, K_SMC_RecoveryCycle);
}
#if 0
//-----------------------------------------------------------------------------
//SMC_CommandLatchCycle
//-----------------------------------------------------------------------------
void SMC_CommandLatchCycle(UINT8 command) 
{
		HAL_NANDSendCmd(command);
}
#endif
//-----------------------------------------------------------------------------
//SMC_IdAddressLatchCycle
//-----------------------------------------------------------------------------
void SMC_IdAddressLatchCycle(void)
{
	HAL_NANDSendAddr(1, 0x00);
}
//-----------------------------------------------------------------------------
//SMC_ReadWriteAddressLatchCycle
//-----------------------------------------------------------------------------
void SMC_ReadWriteAddressLatchCycle(UINT32 address)   //ada@0227
{
	UINT32 Addr;
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;
	if (G_SMC_AddrMode == 0) {
		Addr = address;
		HAL_NANDSendAddr(2, Addr);  //row address
	} else if (G_SMC_AddrMode == 1) {
		//Addr = ((address >> 1) & 0xffffff00) | (address & 0x000000ff);
		Addr = (address >> 1);
//		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 3);
		XBYTE[REG_Fm_NandDataPort] = READ8(address, 3);  //xian modify
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 2);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 1);
		//HAL_NANDSendAddr(3, Addr,address);  //row address
	} else if (G_SMC_AddrMode == 2) {
		//Addr = ((address >> 1) & 0xffffff00) | (address & 0x000000ff);
#if 1
		Addr = (address >> 1);
		XBYTE[REG_Fm_NandDataPort] = READ8(address, 3);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 2);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 1);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 0);
#endif
		//HAL_NANDSendAddr(4, Addr);  //row address
	}
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;
}
#if 0//ndef MTD_NEW


//-----------------------------------------------------------------------------
//SMC_EraseAddressLatchCycle
//-----------------------------------------------------------------------------
void SMC_EraseAddressLatchCycle(UINT32 address)
{
	UINT32 addr;
	WRITE8(addr, 0, 0);
	WRITE8(addr, 1, READ8(address, 0));
	WRITE8(addr, 2, READ8(address, 1));
	WRITE8(addr, 3, READ8(address, 2));
	if (G_SMC_AddrMode == 0) {
		HAL_NANDSendAddr(1, addr);  //row address
	} else if (G_SMC_AddrMode == 1) {
		HAL_NANDSendAddr(2, addr >> 1);  //row address
	} else if (G_SMC_AddrMode == 2) {
		HAL_NANDSendAddr(3, addr >> 1);  //row address
	}
}
//-----------------------------------------------------------------------------
//SMC_CheckReady
//-----------------------------------------------------------------------------
UINT8 SMC_CheckReady(void)
{
	//UINT8 timeout;

	//HAL_NANDCheckRdy(&timeout);

	if (HAL_NANDChkRdy() != SUCCESS)
		return FALSE;
	else
		return TRUE;
}
#endif /* MTD_NEW */

// @012403@wyeo@support 256M nand, begin
//-----------------------------------------------------------------------------
//SMC_ReadIdData
//-----------------------------------------------------------------------------
UINT8 SMC_ReadIdData(void)
{
	UINT8 codeData;
	
	G_NANDF_Maker_Byte = HAL_NANDReadPort();	//maker	code
	codeData = HAL_NANDReadPort();

	//printf("%s %d, id=%02bX %02BX ", __FILE__, (UINT16) __LINE__, G_NANDF_Maker_Byte, codeData);
	printf("NF_ID: %02bX%02BX\n", G_NANDF_Maker_Byte, codeData);
#if 1//(K_SUPPORT_NEWNAND)
	// K9K2G08UOM		K9F1G08UOM		ASU1GA30HT, ASU12A30HT FOR POWER-FLASH( F1 & F0 )
	if (codeData == 0xda || codeData == 0xf1 || codeData == 0xf0) {
		G_NANDF_3RD_Byte = HAL_NANDReadPort();
		G_NANDF_4TH_Byte = HAL_NANDReadPort();
		G_NANDF_5TH_Byte = HAL_NANDReadPort();
		//printf("%02bX %02bX %02BX", G_NANDF_3RD_Byte,G_NANDF_4TH_Byte,G_NANDF_5TH_Byte);
	}
#endif
	//printf("\n");
	HAL_NANDCompleteOperation();	 //ada@1220
	return(codeData);
}
// @012403@wyeo@support 256M nand, end

#if 0//ndef MTD_NEW
//-----------------------------------------------------------------------------
//SMC_ReadData2UsbDma
//-----------------------------------------------------------------------------
UINT8 SMC_ReadData2UsbDma(void) 	// WWWW
{
	UINT16 i;

	//patch4.5@ada@Add timeout count begin
	UINT8 error;
	//patch4.5@ada@Add timeout count end

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(K_DMA_FLASH, K_DMA_USB, K_Card_PageSize, K_DMA_AsyncRet, K_DMA_Padding0s);

	if (error != 0) {
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	//richie@vc0422 begin
	if (G_USB_IO_Bulk2 == 1) {
		if (G_bUSBSpeed == 0) {
			//richie@mi0415 begin
			for (i = 0; i < 8; i++) {
				while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk buffer
				XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
				while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
			}
		}//richie@mi0415 end
		else {
			while ((XBYTE[K_USB_AP_IN_BUF_SIZE] + XBYTE[K_USB_AP_IN_BUF_SIZE + 1] << 8) < 512) ;  //wait for 64 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}
	} else {
		//richie@vc0422 end
		if (G_bUSBSpeed == 0) {
			//richie@mi0415 begin
			for (i = 0; i < 8; i++) {
				//patch4.5@richie@msdc plug out exception
				while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				//patch4.5@richie@msdc plug out exception
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
			//richie@mi0415 end
		} else {
			//printf("$%x\n",(XBYTE[K_USB_AP_IN_BUF_SIZE]+XBYTE[K_USB_AP_IN_BUF_SIZE+1]<<8));
			//while (((XBYTE[K_USB_AP_IN_BUF_SIZE]+XBYTE[K_USB_AP_IN_BUF_SIZE+1]<<8)< 512) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
			while ((XBYTE[K_USB_AP_IN_BUF_SIZE + 1] != 0x02) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}
		//richie@vc0422
	}
	//richie@mi0415 marked
	//HAL_CheckReadData2UsbDmaDone(K_Card_PageSize);

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);
	for (i = K_Card_PageSize; i < K_SMC_PageWholeSize; i++) {
		G_ucStorData[i] = HAL_NANDReadPort();		//wyeo@0521, pathc 4.2
	}
	return SMC_CheckEcc();	// WWWW
}
#endif /* MTD_NEW */
// @patch 4.5@wyeo@make table make faster, begin
#if 1
//-----------------------------------------------------------------------------
//SMC_ReadRedunData2Sram
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
void SMC_ReadRedunData2Sram(void)
{
	data UINT16 i;
	for (i = K_Card_PageSize; i < K_SMC_PageWholeSize; i += 2) {
		G_ucStorData[i] = HAL_NANDReadPort();
		G_ucStorData[i + 1] = HAL_NANDReadPort();
	}
}
#endif /*_DBG_CLR_WARNING_*/
#endif

#if 0
//-----------------------------------------------------------------------------
//SMC_ReadRedunData2Sram
//-----------------------------------------------------------------------------
void SMC_ReadRedunData2Sram(void) 
{
		UINT16 i;
		for (i = K_Card_PageSize; i < K_SMC_PageWholeSize; i++)
		{
		G_ucStorData[i] = HAL_NANDReadPort();		//wyeo@0521, pathc 4.2
		}
}
#endif

// @patch 4.5@wyeo@make table make faster, end

#if 0
//-----------------------------------------------------------------------------
//SMC_ReadStatusData
//-----------------------------------------------------------------------------
UINT8 SMC_ReadStatusData(void) 
{
	return HAL_NANDReadPort();		//I/O status //wyeo@0521, pathc	4.2
}

//-----------------------------------------------------------------------------
//SMC_CompleteOperation
//-----------------------------------------------------------------------------
void SMC_CompleteOperation(void) 
{
		HAL_NANDCompleteOperation();
}
//-----------------------------------------------------------------------------
//SMC_ReadEcc
//-----------------------------------------------------------------------------
void SMC_ReadEcc(void) 
{
		HAL_ReadECC(6, G_SMC_EccByte);
}
#endif
#endif
#endif //DISK_DRV_NAND
