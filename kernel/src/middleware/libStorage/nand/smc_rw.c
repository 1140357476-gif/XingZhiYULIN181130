/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		smc.c

Abstract:

		Module related to access of SmartMedia Card

Environment:

		Keil C51 Compiler

Revision History:

		08/28/2001  	Chi-Yeh Tsai	created

--*/

//=============================================================================
//Header file
//=============================================================================
#define __FILE		__FILE_ID_SMC_533__

#include "general.h"
#include "reg_def.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_nand.h"

#ifdef DISK_DRV_NAND
#include "main.h"
#include "initio.h"
#include "setmode.h"
#include "sdramhw.h"

#if (SMC || NAND)
#include "asicreg.h"
//#include "cardcon.h"
//#include "cardfunc.h"

#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"
//#include "dosusr.h" 	//cch@10/30
#include "doslink.h"	//wyeo@0521, pathc 4.2

// @012403@wyeo@support 256M nand, begin
#if 1//(K_SUPPORT_ONLYNEWNAND == 0)
#if 1//(K_SUPPORT_NEWNAND)
#include "smc2func.h"
#endif
// @012403@wyeo@support 256M nand, end
#include "mtd_general.h"
#include "mtd_macro_api.h"

#include "impt.h"
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
//patch4.5@richie@msdc plug out exception
#include "uiflow.h"

//richie@vc0422
extern  data 	BIT G_USB_IO_Bulk2;
xdata UINT8 G_BadBlk_Mark = 0;
UINT32 xdata nandDma;

extern xdata UINT16	G_Card_TotalSizeMB;

UINT8 SMC_ChkEcc(void);
//=============================================================================
//Program
//=============================================================================

//#if (K_PreWriteOldBlock) //wyeo@0521, pathc 4.2
#if 1
#if 0
UINT8 smcBlock0Assert(UINT32 startAddress)
{
	//UINT32 blk = startAddress/G_NANDF_BlockSize;
	UINT8 status;

	if(( startAddress >= 0 ) && ( startAddress < G_NANDF_BlockSize )){
		ENTER_CRITICAL( status );
		printf("ASSERT = %lx\n",startAddress);
		while(1);
	}
	return TRUE;
}
#endif
UINT8 SMC_BadBlockMark(UINT16 blockAddress) 
{
#if	NAND_BAD_BLOCK_MARK == 1
	UINT8 i;
	UINT8 nrPage;
	UINT32 addr; 

	addr = blockAddress* G_NANDF_BlockSize;
	//nrPage = G_NANDF_BlockSize >> 9;
	nrPage = READ8(G_NANDF_BlockSize, 2) >> 1; // G_NANDF_BlockSize /512, and nrPage doesn't larger than 0x80;
	for (i = 0 ; i < nrPage ; i ++) {
		G_BadBlk_Mark = 1;
		SMC_RedunSram2Card(addr, blockAddress);
		addr += 512L;
	}
#else
	blockAddress = blockAddress;
#endif
	return TRUE;
}

//-----------------------------------------------------------------------------
//SMC_RedunCard2Sram
//-----------------------------------------------------------------------------
UINT8 SMC_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress) 
{
	UINT8 status = TRUE;
	//UINT8 ioStatus;
	const UINT8 cmdBuf[2] = {K_SMC_Read2, K_SMC_SequentialDataInput};
	//printf("%s(%d),addr=%lx,%x\n",__FILE__,(UINT16)__LINE__, startAddress,blockAddress);	
	
#if 0//ndef MTD_NEW		
	UINT8 k, i;

	HAL_NANDCheckRdy(&i);

#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_RedunSram2Card(startAddress, blockAddress);
#endif

	HAL_NANDSendCmd(K_SMC_Read2);   		  	//command = Read 2
	HAL_NANDSendCmd(K_SMC_SequentialDataInput); 		//command = Sequential Data Input
	SMC_ReadWriteAddressLatchCycle(startAddress);   	//address = startAddress

	HAL_ECCMode(1); 								 	//disable ECC
	for (k = 0; k < 6; k++)
		G_SMC_EccByte[k] = 0xFF;

	SMC_FillRedunArea(blockAddress, &G_BadBlk_Mark);

	for (i = 0; i < 16; i++) {
		XBYTE[REG_Fm_NandDataPort] = G_ucStorData[K_Card_PageSize + i];		//wyeo@0521, pathc 4.2
	}

	HAL_NANDSendCmd(K_SMC_PageProgram); 			  //command = Page Program
#if 0
		if (SMC_CheckReady())
		{
		 	HAL_NANDSendCmd(K_SMC_ReadStatus);  	  //command = Read Status
		 	ioStatus = HAL_NANDReadPort();  			  //I/O status
		}
		else
		{
		 	HAL_NANDSendCmd(K_SMC_Reset);   		//reset
		 	status = FALSE;
		}
#endif

	HAL_NANDCompleteOperation();
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {		
		/*Wait nand ready*/
		HAL_NANDChkRdy();
		// Disable WriteProtect
		HAL_NANDEnableWriteProtect(0);
		return SMC2_RedunSram2Card(startAddress, blockAddress);
	}
#endif
#if 0
	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 2, startAddress);

	/*Wait nand ready*/
	HAL_NANDChkRdy();
	
	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}
#else	
	/*Wait nand ready*/
	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);

	MTD_NandSendCmd(K_SMC_Read2);
	MTD_NandSendCmd(K_SMC_SequentialDataInput);
	SMC_ReadWriteAddressLatchCycle(startAddress);   	//address = startAddress
#endif

	/*Disable ECC*/
	MTD_EccDisable();

	SMC_FillRedunArea(blockAddress, &G_BadBlk_Mark);

	MTD_NandSendCmd(K_SMC_PageProgram);
	
	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	/*Complete Nand operation*/
	MTD_NandCtrl(0x03);
	
#endif
	return status;
}
#endif

//-----------------------------------------------------------------------------
//SMC_Card2Sram
//-----------------------------------------------------------------------------
UINT8 SMC_Card2Sram(UINT32 startAddress) 
{
	UINT8 status = TRUE;
	//patch4.5@ada@Add timeout count begin
	const UINT8 cmdBuf[2] = {K_SMC_Read1};
	//printf("%s(%d),addr=%lx,blk=%lx\n",__FILE__,(UINT16)__LINE__, startAddress, startAddress/G_NANDF_BlockSize);				
	//patch4.5@ada@Add timeout count end
#if 0 //ndef MTD_NEW	
	UINT8 i;
	UINT8 error;
	//#if 1
	HAL_NANDCheckRdy(&i);

	// @012403@wyeo@support 256M nand, begin
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		status = SMC2_Card2Sram(startAddress);
		return status;
	}
#endif
	// @012403@wyeo@support 256M nand, end

	HAL_DmaSetSramAddr(0x0000);
	//HAL_SetSMCDMA(1,startAddress);
	HAL_NANDSendCmd(K_SMC_Read1);    //Read from nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(2, 1, K_Card_PageSize, 0, 0);

	if (error != 0) {
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);
#if 1
	for (i = 0; i < 16; i++) {
		G_ucStorData[K_Card_PageSize + i] = HAL_NANDReadPort();
	}
#else
	HAL_DmaSetSramAddr(0x0e00);				//wyeo@0521, pathc 4.2,	
	SMC_CheckReady();
	HAL_NANDSendCmd(0x50);    
	SMC_ReadWriteAddressLatchCycle(startAddress);
	HAL_NANDSendCmd(0x00);    
	HAL_DmaDo(2, 1, 16, 0, 0);
#endif

	HAL_NANDCompleteOperation();

	// WWWW start
	status = SMC_CheckEcc();
#if ( K_SMC_DO_ECC_CORECTION )
	if (status == FALSE) {
		status = SMC_CorrectPage(2, 0, 1);
		if (!status) {
			status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}
#endif
// WWWW end

//dumpData( 512 , 0x1c00, 0);
//dumpData(512, &XBYTE[0x1c00], 0);
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();

		status = SMC2_Card2Sram(startAddress);
		return status;
	}
#endif
	/*Set sram DMA source address*/
	HAL_DmaSetSramAddr(0x0000);

	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 1, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Trigger sram DMA*/
	MTD_DMATrig();
	/*Wait sram DMA ready*/
	HAL_NANDDoDMAWait();
	
	/*Disable ECC*/
	MTD_EccDisable();

	/*Check ECC*/
	if (SMC_ChkEcc() != SUCCESS) {
		CPrintf("E");
		G_SMC_EccErrCount++;
		HAL_ReadECC(6, G_SMC_EccByte);		//lryy, mantis #38137
		status = SMC_CorrectPage(2, 0, 1);
		if (!status) {
			status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}

	/*Complete Nand operation*/
	MTD_NandCtrl(0x03);

#endif	
	return status;
}

//-----------------------------------------------------------------------------
//SMC_Sram2Card
//-----------------------------------------------------------------------------
UINT8 SMC_Sram2Card(UINT32 startAddress, UINT16 blockAddress)   //ada@1221
{
	UINT8 status = TRUE;
	//UINT8 ioStatus;
	const UINT8 cmdBuf[2] = {K_SMC_SequentialDataInput};

	//printf("%s(%d),addr=%lx,%x\n",__FILE__,(UINT16)__LINE__, startAddress, blockAddress);	
#if 0//ndef MTD_NEW	
	UINT8 error;
	UINT8 i;
	//#if 1	
	HAL_NANDCheckRdy(&i);

	// @012403@wyeo@support 256M nand, begin
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_Sram2Card(startAddress, blockAddress);
#endif
	// @012403@wyeo@support 256M nand, end

	HAL_DmaSetSramAddr(0x0000);
	//HAL_SetSMCDMA(0,startAddress);
	HAL_NANDSendCmd(K_SMC_SequentialDataInput);    //Write to nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(1, 2, K_Card_PageSize, 0, 0);

	if (error != 0) {
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	SMC_FillRedunArea(blockAddress, 0);

	for (i = 0; i < 16; i++) {
		//HAL_NANDWritePort(G_ucStorData[i]);		//wyeo@0521, pathc 4.2
		XBYTE[REG_Fm_NandDataPort] = G_ucStorData[K_Card_PageSize + i];		//wyeo@0521, pathc 4.2
	}

	HAL_NANDSendCmd(K_SMC_PageProgram); 			  //command = Page Program
#if 0
if (SMC_CheckReady())
{
	HAL_NANDSendCmd(K_SMC_ReadStatus);  	  //command = Read Status
	ioStatus = HAL_NANDReadPort();  			  //I/O status
}
else
{
	HAL_NANDSendCmd(K_SMC_Reset);   		//reset
	status = FALSE;
}
#endif
	HAL_NANDCompleteOperation();

	//dumpData(512, &XBYTE[0x1c00], 0);
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();

		return SMC2_Sram2Card(startAddress, blockAddress);
	}
#endif
	/*Set sram DMA source address*/
	HAL_DmaSetSramAddr(0x0000);;

	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(1, 2, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Trigger sram DMA*/
	MTD_DMATrig();
	/*Wait sram DMA ready*/
	HAL_NANDDoDMAWait();
	
	/*Disable ECC*/
	MTD_EccDisable();

	/*Read ECC data, Fill redundant area (16B)*/
	MTD_EccRedunFill(blockAddress);

	/*Send page program command to nand*/
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	//Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);
	
	MTD_NandCtrl(0x03);

#endif
	return status;
}

//-----------------------------------------------------------------------------
//SMC_Card2Dram
//-----------------------------------------------------------------------------
UINT8 SMC_Card2Dram(UINT32 startAddress, UINT32 ramAddr) 
{
	UINT8 status = TRUE;
	const UINT8 cmdBuf[2] = {K_SMC_Read1};
	//printf("%s(%d),addr=%lx,%lx,blk=%lx\n",__FILE__,(UINT16)__LINE__, startAddress, ramAddr, startAddress/G_NANDF_BlockSize);				 
	//XBYTE[0x2030] &= 0xfe;
#if 0 //ndef MTD_NEW
	UINT8 i;
	UINT8 error;
	HAL_NANDCheckRdy(&i);

#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_Card2Dram(startAddress, ramAddr);
#endif

	HAL_DmaSetDramAddr(ramAddr);
	//HAL_SetSMCDMA(1,startAddress);
	HAL_NANDSendCmd(K_SMC_Read1);    //Read from nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	error = HAL_DmaDo(2, 0, K_Card_PageSize, 0, 0);
	//HAL_DoNandDMA(0x02,511);		

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	for (i = 0; i < 16; i++) {
		G_ucStorData[K_Card_PageSize + i] = HAL_NANDReadPort();
	}

	HAL_NANDCompleteOperation();

	status = SMC_CheckEcc();
#if ( K_SMC_DO_ECC_CORECTION )
	if (status == FALSE) {
		status = SMC_CorrectPage(2, ramAddr, 1);
		if (!status) {
			status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}
#endif

//XBYTE[0x2030] |= 0x01;
#else

#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {		
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();
		
		return SMC2_Card2Dram(startAddress, ramAddr);
	}
#endif


	/*Set dram DMA source address*/
	HAL_NANDSetDRAMDMA(ramAddr);

	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 0, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Trigger dram DMA*/
	MTD_DMATrig();
	/*Wait dram DMA ready*/
	HAL_NANDDoDMAWait();

	/*Disable ECC*/
	MTD_EccDisable();	

	/*Check ECC*/
	if (SMC_ChkEcc() != SUCCESS) {
		CPrintf("E");
		G_SMC_EccErrCount++;
//		HAL_ReadECC(6, G_SMC_EccByte);		//lryy, mantis #38137
		status = SMC_CorrectPage(2, ramAddr, 1);
		if (!status) {
			printf("B->%lx\n", startAddress / G_NANDF_BlockSize);
			status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}

	/*Complete Nand operation*/
	MTD_NandCtrl(0x03);

#endif	
	return status;
}

UINT8 SMC_Card2DramFast(UINT32 startAddress, UINT32 ramAddr) 
{
	if (G_NANDF_Category) {
		return SMC2_Card2DramFast(startAddress, ramAddr);
	}
	else{
		return SMC_Card2Dram(startAddress, ramAddr);
	}		
}

//-----------------------------------------------------------------------------
//SMC_Dram2Card
//-----------------------------------------------------------------------------
UINT8 SMC_Dram2Card(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr) 
{
	UINT8 status = TRUE;
	//UINT8 ioStatus;
	const UINT8 cmdBuf[2] = {K_SMC_SequentialDataInput};
	//printf("%s(%d),addr=%lx,%lx,%x\n",__FILE__,(UINT16)__LINE__, startAddress, ramAddr,blockAddress);				
	//XBYTE[0x2030] &= 0xfe;
	//smcBlock0Assert(startAddress);
#if 0//ndef MTD_NEW
	UINT8 i;
	UINT8 error;
	HAL_NANDCheckRdy(&i);

#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		status = SMC2_Dram2Card(startAddress, blockAddress, ramAddr);
		return status;
	}
#endif

	HAL_DmaSetDramAddr(ramAddr);

	HAL_NANDSendCmd(K_SMC_SequentialDataInput);    //Write to nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC
	error = HAL_DmaDo(0, 2, K_Card_PageSize, 0, 0);
	//HAL_DoNandDMA(0x20,511);		
	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);	
	SMC_FillRedunArea(blockAddress, 0);

	for (i = 0; i < 16; i++) {
		XBYTE[REG_Fm_NandDataPort] = G_ucStorData[K_Card_PageSize + i];		//wyeo@0521, pathc 4.2
	}
	HAL_NANDSendCmd(K_SMC_PageProgram); 			  //command = Page Program
#if 0
if (SMC_CheckReady())
{
	HAL_NANDSendCmd(K_SMC_ReadStatus);  	  //command = Read Status
	ioStatus = HAL_NANDReadPort();  			  //I/O status
}
else
{
	HAL_NANDSendCmd(K_SMC_Reset);   		//reset
	status = FALSE;
}
HAL_NANDCompleteOperation();
#else
#endif

	//	XBYTE[0x2030] |= 0x01;
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();

		status = SMC2_Dram2Card(startAddress, blockAddress, ramAddr);
		return status;
	}
#endif

	/*Set dram DMA source address*/
	HAL_NANDSetDRAMDMA(ramAddr);

	HAL_NANDChkRdy();
	MTD_NandSendCmd(K_SMC_Read1);	//xian ++

	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(0, 2, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();
	// dISable WriteProtect
	HAL_NANDEnableWriteProtect(0);

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}
	
	/*Trigger dram DMA*/
	MTD_DMATrig();
	/*Wait dram DMA ready*/
	HAL_NANDDoDMAWait();

	/*Disable ECC*/
	MTD_EccDisable();

	/*Read ECC data, Fill redundant area (16B)*/
	MTD_EccRedunFill(blockAddress);

	/*Send page program command to nand*/
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	MTD_NandCtrl(0x03);
	
#endif
	return status;
}

//richie@mi0415
//-----------------------------------------------------------------------------
//SMC_Usb2Card
//-----------------------------------------------------------------------------
UINT8 SMC_Usb2Card(UINT32 startAddress, UINT16 blockAddress) 
{
	UINT8 status = TRUE;
	//UINT8 ioStatus;
	UINT8 i;

	//patch4.5@ada@Add timeout count begin
	//patch4.5@ada@Add timeout count end
	const UINT8 cmdBuf[2] = {K_SMC_SequentialDataInput};
#if 0//ndef MTD_NEW
//#if 1
	UINT8 error;
	HAL_NANDCheckRdy(&i);

	// @012403@wyeo@support 256M nand, begin
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_Usb2Card(startAddress, blockAddress);
#endif
	// @012403@wyeo@support 256M nand, end

	//     HAL_SetUSBDMA(K_USB_CLASS_DMA_OUT, K_DMA_UsbDontCare);//for class
	//HAL_SetSMCDMA(0,startAddress);
	HAL_NANDSendCmd(K_SMC_SequentialDataInput);    //Write to nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	//						async
	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(K_DMA_USB, K_DMA_FLASH, K_Card_PageSize, 1, 0);

	if (error != 0) {
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	if (G_bUSBSpeed == 0) {
		for (i = 0; i < 8; i++) {
			//patch4.5@richie@msdc plug out exception
			while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
			//patch4.5@richie@msdc plug out exception
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		}

		while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
	} else {
		//while (((XBYTE[K_USB_CLASS_OUT_BUF_SIZE]+XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
		XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
		XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
		//patch4.5@richie@msdc plug out exception
		while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
		XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		while ((XBYTE[REG_Dma_DmaCmp] != 0x01)&& (G_UI_USBConnect == K_UI_USB_CONNECT));

	}

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	SMC_FillRedunArea(blockAddress, 0);

	for (i = 0; i < 16; i++) {
		//HAL_NANDWritePort(G_ucStorData[i]);		//wyeo@0521, pathc 4.2
		XBYTE[REG_Fm_NandDataPort] = G_ucStorData[K_Card_PageSize + i];		//wyeo@0521, pathc 4.2
	}

	HAL_NANDSendCmd(K_SMC_PageProgram); 			  //command = Page Program
#if 0
if (SMC_CheckReady())
{
  HAL_NANDSendCmd(K_SMC_ReadStatus);		//command = Read Status
  ioStatus = HAL_NANDReadPort();				//I/O status
}
else
{
  HAL_NANDSendCmd(K_SMC_Reset); 		  //reset
  status = FALSE;
}

HAL_NANDCompleteOperation();
#endif
#else

#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();

		return SMC2_Usb2Card(startAddress, blockAddress);
	}
#endif
	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);
	
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(K_DMA_USB, K_DMA_FLASH, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Trigger dram DMA*/
	MTD_DMATrig();

	/*Don't wait USB DMA ready*/

	if (G_bUSBSpeed == 0) {
		for (i = 0; i < 8; i++) {
			//patch4.5@richie@msdc plug out exception
			while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
			//patch4.5@richie@msdc plug out exception
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
				if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
			}
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		}

		while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
	} else {
		//while (((XBYTE[K_USB_CLASS_OUT_BUF_SIZE]+XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) > 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
		XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
		XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;//0x02;//open bulk out
		//patch4.5@richie@msdc plug out exception
		while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
		XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		while ((XBYTE[REG_Dma_DmaCmp] != 0x01)&& (G_UI_USBConnect == K_UI_USB_CONNECT)){
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
	}

	/*Disable ECC*/
	MTD_EccDisable();

	/*Read ECC data, Fill redundant area (16B)*/
	MTD_EccRedunFill(blockAddress);

	/*Send page program command to nand*/
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	MTD_NandCtrl(0x03);

#endif
	return status;
}

//-----------------------------------------------------------------------------
//SMC_Card2Usb
//-----------------------------------------------------------------------------
UINT8 SMC_Card2Usb(UINT32 startAddress) 
{
	UINT8 status = TRUE;
	UINT8 sts = TRUE;
	UINT8 i;
	const UINT8 cmdBuf[2] = {K_SMC_Read1};
	
#if 0//ndef MTD_NEW
//#if 1
	UINT8 timeout;
	HAL_NANDCheckRdy(&timeout);

	// @012403@wyeo@support 256M nand, begin
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_Card2Usb(startAddress);
	#endif
	// @012403@wyeo@support 256M nand, end

	//patch4.4.1@richie@stat mark
	//G_ucState3 &= 0xfe;   						  //FIFO prepare data doing

	//richie@vc0422 begin
	if (G_USB_IO_Bulk2 == 1) {
		//			HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_AP_DMA_IN);//for ap
	} else {
		//			HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_CLASS_DMA_IN);//for class
	}
	//richie@vc0422 end

	HAL_NANDSendCmd(K_SMC_Read1);   		  //command = Read 1

	SMC_ReadWriteAddressLatchCycle(startAddress);   //address = startAddress

	if (SMC_CheckReady()) {
		//patch4.4.1@richie@stat mark
		//G_ucState3 |= 0x01;   				 //FIFO prepare data done

		sts = SMC_ReadData2UsbDma();			//read data thru DMA
#if (K_SMC_DO_USB_ECC_BY_HOST)
		if (sts == FALSE) {
			sts = SMC_CorrectPage(2, 0, 0);
			if (!sts) {
				status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
			}
		}
#endif
	} else {
		HAL_NANDSendCmd(K_SMC_Reset);     //reset
		status = FALSE;
	}

	HAL_NANDCompleteOperation();						//complete
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		/*Wait nand ready, it's in page program mode maybe*/
		//HAL_NANDChkRdy();
		
		return SMC2_Card2Usb(startAddress);
	}
#endif

	if (G_USB_IO_Bulk2 == 1) {
		//HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_AP_DMA_IN);//for ap
	} else {
		//HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_CLASS_DMA_IN);//for class
	}

	/*Enable ECC*/
	MTD_EccEnable();

	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(K_DMA_FLASH, K_DMA_USB, K_Card_PageSize, K_DMA_Padding0s);

	if (HAL_NANDChkRdy() == SUCCESS) {

		/*Check if auto command buffer ready*/
		if (XBYTE[REG_Fm_cmdsize_addrsize]) {
			/*Enable CE, Disable WP*/	
			MTD_NandCtrl(0x02);
			/*Trigger auto command/address mode*/
			MTD_NandAutoCmdTrig(); 
			/*Wait auto command/address finished*/
			MTD_NandAutoCmdWait();
		}
		
		HAL_NANDDoDMACfg(K_DMA_FLASH, K_DMA_USB, K_Card_PageSize, K_DMA_Padding0s);

		/*Trigger USB DMA*/
		MTD_DMATrig();

		/*Don't wait USB DMA ready*/

		if (G_USB_IO_Bulk2 == 1) {
			if (G_bUSBSpeed == 0) {
				for (i = 0; i < 8; i++) {
					while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk buffer
					XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
					while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
					XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
				}
			}
			else {
				while ((XBYTE[K_USB_AP_IN_BUF_SIZE] + XBYTE[K_USB_AP_IN_BUF_SIZE + 1] << 8) < 512) ;  //wait for 64 in bulk buffer
				XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
				while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
			}
		} else {
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
				//printf("$%x\n",(XBYTE[K_USB_AP_IN_BUF_SIZE]+XBYTE[K_USB_AP_IN_BUF_SIZE+1]<<8));
				//while (((XBYTE[K_USB_AP_IN_BUF_SIZE]+XBYTE[K_USB_AP_IN_BUF_SIZE+1]<<8)< 512) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
				while ((XBYTE[K_USB_AP_IN_BUF_SIZE + 1] != 0x02) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait for 0 in bulk buffer
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
					if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
				}
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
		}
		//HAL_CheckReadData2UsbDmaDone(K_Card_PageSize);

		/*Disable ECC*/
		MTD_EccDisable();

		/*Check ECC*/
		if (SMC_ChkEcc() != SUCCESS) {
			CPrintf("E");
			G_SMC_EccErrCount++;
			HAL_ReadECC(6, G_SMC_EccByte);		//lryy, mantis #38137
			status = SMC_CorrectPage(2, 0, 0);
			if (!status) {
				status = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
			}
		}

	} else {
		/*already reset in nand ready check*/
		//MTD_NandSendCmd(K_SMC_Reset);
		status = FALSE;
	}

	/*NANDCompleteOperation*/
	MTD_NandCtrl(0x03);
#endif
	return status;
}

UINT8 SMC_Card2UsbFast(UINT32 startAddress) 
{
	if (G_NANDF_Category) {
		return SMC2_Card2UsbFast(startAddress);
	}
	else{
		return SMC_Card2Usb(startAddress);
	}		
}
//-----------------------------------------------------------------------------
//SMC_EraseOneBlock
//-----------------------------------------------------------------------------
UINT8 SMC_EraseOneBlock(UINT32 startAddress) 
{
	UINT8 status = TRUE;
	//UINT8 ioStatus;
	//HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	//printf("%s(%d),addr=%lx,blk=%lx\n",__FILE__,(UINT16)(UINT16)__LINE__, startAddress,startAddress/G_NANDF_BlockSize);				
	const UINT8 cmdBuf[2] = {K_SMC_BlockErase1, K_SMC_BlockErase2};

//	putchar('.');
//	putchar('\n');

#if 0//ndef MTD_NEW
	UINT8 timeout;
	HAL_NANDCheckRdy(&timeout);
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_EraseOneBlock(startAddress);
#endif
	// @012403@wyeo@support 256M nand, end

	HAL_NANDSendCmd(K_SMC_BlockErase1); 			  //command = Block Erase 1st cycle

	SMC_EraseAddressLatchCycle(startAddress);   			//address = startAddress

	HAL_NANDSendCmd(K_SMC_BlockErase2); 			  //command = Block Erase 2nd cycle
#if 0
	   if (SMC_CheckReady())
	   {
			   HAL_NANDSendCmd(K_SMC_ReadStatus);   	 //command = Read Status
			   ioStatus = HAL_NANDReadPort();   			 //I/O status
	   }
	   else
	   {
			   HAL_NANDSendCmd(K_SMC_Reset);			 //reset
			   status = FALSE;
	   }
#endif
#else
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		//HAL_NANDChkRdy();
		return SMC2_EraseOneBlock(startAddress);
	}
#endif

#if 1
//	MTD_NandAutoCmdWait();

	//if (XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] != 0x02)
		//CPrintf1("ctrl (%bu)" , XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] );
	//MTD_NandCtrl(0x02);
	//HAL_NANDChkRdy();
	SMC_AutoCmdSet(cmdBuf, 2, startAddress);
{
	UINT32 addr;
	
	WRITE8(addr, 0, 0);
	WRITE8(addr, 1, READ8(startAddress, 0));
	WRITE8(addr, 2, READ8(startAddress, 1));
	WRITE8(addr, 3, READ8(startAddress, 2));

	if (G_SMC_AddrMode) {
		addr = addr >> 1;
	}
	if (G_SMC_AddrMode == 0) {
		XBYTE[REG_Fm_addrbuf_0] = READ8(addr, 3);
		XBYTE[REG_Fm_cmdsize_addrsize] = 0x12;
	} else if (G_SMC_AddrMode == 1) {
		XBYTE[REG_Fm_addrbuf_0] = READ8(addr, 3);
		XBYTE[REG_Fm_addrbuf_1] = READ8(addr, 2);
		XBYTE[REG_Fm_cmdsize_addrsize] = 0x22;
	} else if (G_SMC_AddrMode == 2) {
		XBYTE[REG_Fm_addrbuf_0] = READ8(addr, 3);
		XBYTE[REG_Fm_addrbuf_1] = READ8(addr, 2);
		XBYTE[REG_Fm_addrbuf_2] = READ8(addr, 1);
		XBYTE[REG_Fm_cmdsize_addrsize] = 0x32;		
	}
}
	
	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);
	
	MTD_NandAutoCmdTrig();
	
	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);
//	MTD_NandAutoCmdWait();

#else
	HAL_NANDChkRdy();

	MTD_NandSendCmd(K_SMC_BlockErase1);

	SMC_EraseAddressLatchCycle(startAddress); 

	MTD_NandSendCmd(K_SMC_BlockErase2);
#endif
#endif
	return status;
}


//-----------------------------------------------------------------------------
//SMC_Card2SramForExt
//-----------------------------------------------------------------------------
UINT8 SMC_Card2SramForExt(UINT32 startAddress)   //ada@1221
{
//	data UINT8 status = TRUE;
	data UINT8 status = TRUE;
	UINT8 i;

	const UINT8 cmdBuf[2] = {K_SMC_Read1};
	
#if 0//ndef MTD_NEW
	UINT8 error;
	HAL_NANDCheckRdy(&i);
	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	// @012403@wyeo@support 256M nand, begin
#if (K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_RedunCard2Sram(startAddress);
#endif
	// @012403@wyeo@support 256M nand, end

	HAL_DmaSetSramAddr(0x0200);				//wyeo@0521, pathc 4.2,
	//HAL_SetSMCDMA(1,startAddress);
	HAL_NANDSendCmd(K_SMC_Read1);    //Read from nandf
	SMC_ReadWriteAddressLatchCycle(startAddress);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ReadECC(6, G_SMC_EccByte);  								//clear ECC

	//patch4.5@ada@Add timeout count begin
	error = HAL_DmaDo(2, 1, K_Card_PageSize, 0, 0);

	if (error != 0) {
		return FALSE;
	}
	//patch4.5@ada@Add timeout count end

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	for (i = 0; i < 16; i++) {
		G_ucStorData[K_Card_PageSize + i] = HAL_NANDReadPort();
	}

	HAL_NANDCompleteOperation();

	//SMC_CheckEcc(); // WWWW
#else

#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category) {
		//HAL_NANDChkRdy();
		return SMC2_RedunCard2Sram(startAddress);
	}
#endif

	/*Set sram DMA source address*/
	HAL_DmaSetSramAddr(0x0000);

	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();

	/*Configure nand auto command/address send mode*/
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 1, K_Card_PageSize, 0);

	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();

	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Trigger sram DMA*/
	MTD_DMATrig();
	/*Wait sram DMA ready*/
	if (HAL_NANDDoDMAWait() != SUCCESS) {
		return FALSE;
	}

	/*Disable ECC*/
	MTD_EccDisable();

	HAL_ReadECC(6, G_SMC_EccByte);

	for (i = 0; i < 16; i++) {
		G_ucStorData[K_Card_PageSize + i] = HAL_NANDReadPort();
	}

	/*Complete Nand operation*/
	MTD_NandCtrl(0x03);

#endif
	return status;
}

// @patch 4.5@wyeo@make table make faster, begin

#if 1
UINT8 SMC_RedunCard2Sram(UINT32 startAddress) 
{
	data UINT16 i;
//	UINT16 i;
	data UINT16 timeOut = 6000;
	//data BIT sts;// @patch5.2.1-24@wyeo@fix initiliaze nand become mess@110603
	UINT8 sts;// @patch5.2.1-24@wyeo@fix initiliaze nand become mess@110603
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);

	HAL_NANDCheckRdy(&sts);

	HAL_FlashMode(K_MEDIA_Nandf, K_MEDIA_Sdf, 1);
	// @012403@wyeo@support 256M nand, begin
#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		return SMC2_RedunCard2Sram(startAddress);
#endif
	// @012403@wyeo@support 256M nand, end

	//HAL_NANDSendCmd(K_SMC_Read2); 			//command = Read 2

	//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
	if (G_Card_Type == K_MEDIA_SMC) {
		SMC_CS_ENABLE;
		SMC_CS_OUTPUT_ENABLE;
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0B & (~WPBIT))|tmp;
	} else		
#endif     
	//patch5.2.1@ada@Different chip select pin for SMC end
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0A & (~WPBIT))|tmp;

	XBYTE[REG_Fm_NandDataPort] = K_SMC_Read2;

	//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
	if (G_Card_Type == K_MEDIA_SMC) {
		SMC_CS_ENABLE;
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x03 & (~WPBIT))|tmp;
	} else
#endif
	//patch5.2.1@ada@Different chip select pin for SMC end
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;

	//SMC_ReadWriteAddressLatchCycle(startAddress);   //address = startAddress

	//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
	if (G_Card_Type == K_MEDIA_SMC) {
		SMC_CS_ENABLE;
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x07 & (~WPBIT))|tmp;
	} else
#endif     
	//patch5.2.1@ada@Different chip select pin for SMC end

	// CLE  ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;   //   0    1    1	0

	if ((G_Card_TotalSizeMB == 0x0001) ||     // 1MB || 2MB
		(G_Card_TotalSizeMB == 0x0002)) {
		XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress & 0xff);  	   //A0~A7
		XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 8) & 0xff);  //A8~A15
		XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 16) & 0xff); //A16~A23
	} else  								  // > 2MB
	{
		XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress & 0xff);  	   //A0~A7
		XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 9) & 0xff);  //A9~A16
		XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 17) & 0xff); //A17~A24
		//if (G_Card_TotalSizeMB > 0x0020) // > 32MB
		if (G_SMC_AddrMode > 1) // > 32MB
			XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 25) & 0xff); //A25~A31
	}

	//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
	if (G_Card_Type == K_MEDIA_SMC) {
		SMC_CS_ENABLE;
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x03 & (~WPBIT))|tmp;
	} else
#endif
	//patch5.2.1@ada@Different chip select pin for SMC end
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;   //   0    0    1	0

	//HAL_NANDCheckRdy(&timeout);
	sts = 1;
	while ((XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01) == 0x00) {
		timeOut--;
		if (timeOut == 0) {
			sts = 0;
			break;
		}
	}
	if (sts) {
		//SMC_ReadRedunData2Sram(); 			  //read redundant area
		for (i = K_Card_PageSize; i < K_SMC_PageWholeSize; i += 2) {
			G_ucStorData[i] = HAL_NANDReadPort();
			G_ucStorData[i + 1] = HAL_NANDReadPort();
		}
	} else {
		HAL_NANDSendCmd(K_SMC_Reset);     //reset
		return FALSE;
	}

	//HAL_NANDCompleteOperation();  					  //complete

	//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
	if (G_Card_Type == K_MEDIA_SMC) {
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x03 & (~WPBIT))|tmp;
			SMC_CS_DISABLE;
	} else
#endif 
	//patch5.2.1@ada@Different chip select pin for SMC end

	// CLE  ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x03 & (~WPBIT))|tmp;   //   0    0    1	1

	return TRUE;
}
#endif


// @patch 4.5@wyeo@make table make faster, end
#endif
// @012403@wyeo@support 256M nand, end

//-----------------------------------------------------------------------------
//SMC_GetID
//-----------------------------------------------------------------------------
UINT8 SMC_GetID(void) 
{
	HAL_NANDChkRdy();
	//HAL_NANDEnableWriteProtect(1);
	SMC_HwInitialize(); 
	HAL_NANDSendCmd(K_SMC_ReadId);  		  //command = Read ID
	SMC_IdAddressLatchCycle();  					//address = 0x00
	return(SMC_ReadIdData());
}


//-----------------------------------------------------------------------------
//SMC_CheckEcc
//-----------------------------------------------------------------------------
// WWWW
UINT8 SMC_CheckEcc(void) 
{
	UINT8 status = TRUE;
#if 0//ndef MTD_NEW
	if ((G_ucStorData[K_Card_PageSize + 8] != G_SMC_EccByte[3]) || (G_ucStorData[K_Card_PageSize + 9] != G_SMC_EccByte[4]) || (G_ucStorData[K_Card_PageSize + 10] != G_SMC_EccByte[5]) || (G_ucStorData[K_Card_PageSize + 13] != G_SMC_EccByte[0]) || (G_ucStorData[K_Card_PageSize + 14] != G_SMC_EccByte[1]) || (G_ucStorData[K_Card_PageSize + 15] != G_SMC_EccByte[2])) {
		G_SMC_EccErrCount++;
		status = FALSE;
	}
#else
	if (G_ucStorData[K_Card_PageSize + 8] != XBYTE[REG_Fm_ECC_3] ||\
		G_ucStorData[K_Card_PageSize + 9] != XBYTE[REG_Fm_ECC_4] ||\
		G_ucStorData[K_Card_PageSize + 10] != XBYTE[REG_Fm_ECC_5] ||\
		G_ucStorData[K_Card_PageSize + 13] != XBYTE[REG_Fm_ECC_0] ||\
		G_ucStorData[K_Card_PageSize + 14] != XBYTE[REG_Fm_ECC_1] ||\
		G_ucStorData[K_Card_PageSize + 15] != XBYTE[REG_Fm_ECC_2]
	) {
		CPrintf("E");
		G_SMC_EccErrCount++;
		return FALSE;
	}	
#endif
	return status;
}

//-----------------------------------------------------------------------------
//SMC_FillRedunArea
//-----------------------------------------------------------------------------
void SMC_FillRedunArea(UINT16 blockAddress, UINT8* pBadBlk) 
{
#if 0 //ndef MTD_NEW
	//User Data Field
	G_ucStorData[K_Card_PageSize + 0] = 0xff;
	G_ucStorData[K_Card_PageSize + 1] = 0xff;
	G_ucStorData[K_Card_PageSize + 2] = 0xff;
	G_ucStorData[K_Card_PageSize + 3] = 0xff;

	//Data Status Byte
	G_ucStorData[K_Card_PageSize + 4] = 0xff;

	//Block Status Byte
	if ((pBadBlk != NULL) && (*pBadBlk)) {
		*pBadBlk = 0;
		printf("Bad %d\n", blockAddress);
		G_ucStorData[K_Card_PageSize + 5] = 0xf0;
	} else {
		G_ucStorData[K_Card_PageSize + 5] = 0xff;
	}

	//Block Address Field-1 and Block Address Field-2
	G_ucStorData[K_Card_PageSize + 6] = G_ucStorData[K_Card_PageSize + 11] = (UINT8) ((blockAddress >> 8) & 0xff); 
	G_ucStorData[K_Card_PageSize + 7] = G_ucStorData[K_Card_PageSize + 12] = (UINT8) (blockAddress & 0xff);

	//ECC Field-2
	G_ucStorData[K_Card_PageSize + 8] = G_SMC_EccByte[3];
	G_ucStorData[K_Card_PageSize + 9] = G_SMC_EccByte[4];
	G_ucStorData[K_Card_PageSize + 10] = G_SMC_EccByte[5];

	//ECC Field-1
	G_ucStorData[K_Card_PageSize + 13] = G_SMC_EccByte[0];
	G_ucStorData[K_Card_PageSize + 14] = G_SMC_EccByte[1];
	G_ucStorData[K_Card_PageSize + 15] = G_SMC_EccByte[2];
#else
	XBYTE[REG_Fm_NandDataPort] = 0xff;	
	XBYTE[REG_Fm_NandDataPort] = 0xff;	
	XBYTE[REG_Fm_NandDataPort] = 0xff;		
	XBYTE[REG_Fm_NandDataPort] = 0xff;	
	XBYTE[REG_Fm_NandDataPort] = 0xff;
	if ((pBadBlk != NULL) && (*pBadBlk)) {
		*pBadBlk = 0;
		XBYTE[REG_Fm_NandDataPort] = 0xf0;		
	} else {
		XBYTE[REG_Fm_NandDataPort] = 0xff;		
	}
	XBYTE[REG_Fm_NandDataPort] = READ8((blockAddress), 0);	
	XBYTE[REG_Fm_NandDataPort] = READ8((blockAddress), 1);		
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_3];			
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_4];		
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_5];			
	XBYTE[REG_Fm_NandDataPort] = READ8((blockAddress), 0);		
	XBYTE[REG_Fm_NandDataPort] = READ8((blockAddress), 1);		
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_0];			
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_1];			
	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_2];	
#endif
}

UINT8 SMC_ChkEcc(void)
{
	volatile UINT8 data Data;
	UINT8 ret = SUCCESS;
    UINT8 cnt = 0, i;
#if 0
	Data = XBYTE[REG_Fm_NandDataPort];  // 0
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 1
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 2
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 3
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 4
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 5
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 6
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 7
	Data = XBYTE[REG_Fm_NandIpioRdata];

	Data = XBYTE[REG_Fm_NandDataPort];  // 8
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_3]) {
		return FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // 9
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_4]) {
		return FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // 10
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_5]) {
		return FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // 11
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 12
	Data = XBYTE[REG_Fm_NandIpioRdata];
	Data = XBYTE[REG_Fm_NandDataPort];  // 13
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_0]) {
		return FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // 14
	Data = XBYTE[REG_Fm_NandIpioRdata];	
	if (Data != XBYTE[REG_Fm_ECC_1]) {
		return FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // 15
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_2]) {
		return FAIL;
	}
#else
	Data = XBYTE[REG_Fm_NandDataPort];  // 0
	Data = XBYTE[REG_Fm_NandDataPort];  // 1
	Data = XBYTE[REG_Fm_NandDataPort];  // 2
	Data = XBYTE[REG_Fm_NandDataPort];  // 3
	Data = XBYTE[REG_Fm_NandDataPort];  // 4
	Data = XBYTE[REG_Fm_NandDataPort];  // 5
	Data = XBYTE[REG_Fm_NandDataPort];  // 6
	Data = XBYTE[REG_Fm_NandDataPort];  // 7
	Data = XBYTE[REG_Fm_NandDataPort];  // 8

#if 0//xian --
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 8 trig 9
	if (Data != XBYTE[REG_Fm_ECC_3]) {
		ret = FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 9 trig 10
	if (Data != XBYTE[REG_Fm_ECC_4]) {
		ret = FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 10 trig 11
	if (Data != XBYTE[REG_Fm_ECC_5]) {
		ret = FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 11 trig 12
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 12 trig 13
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 13 trig 14
	if (Data != XBYTE[REG_Fm_ECC_0]) {
		ret = FAIL;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 14 trig 15
	if (Data != XBYTE[REG_Fm_ECC_1]) {
		ret = FAIL;
	}
	//Data = XBYTE[REG_Fm_NandDataPort];  // 15
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_2]) {
		ret = FAIL;
	}

#else
    while(1){
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 8 trig 9
        if (Data != XBYTE[REG_Fm_ECC_3]) {
            cnt = 1;
            break;
        }
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 9 trig 10
        if (Data != XBYTE[REG_Fm_ECC_4]) {
            cnt = 2;
            break;
        }
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 10 trig 11
        if (Data != XBYTE[REG_Fm_ECC_5]) {
            cnt = 3;
            break;
        }
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 11 trig 12
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 12 trig 13
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 13 trig 14
       if (Data != XBYTE[REG_Fm_ECC_0]) {
            cnt = 6;
            break;
        }
        Data = XBYTE[REG_Fm_NandDataPort];  // dat 14 trig 15
        if (Data != XBYTE[REG_Fm_ECC_1]) {
            cnt = 7;
            break;
        }
        //Data = XBYTE[REG_Fm_NandDataPort];  // 15
        Data = XBYTE[REG_Fm_NandIpioRdata];
        if (Data != XBYTE[REG_Fm_ECC_2]) {
            cnt = 8;
            break;
        }

        break;

    }
    
    if(cnt){
        ret = FAIL;
        HAL_ReadECC(6, G_SMC_EccByte);

        for(i = 0; i < 6; i++){
		if (i >= 3)
                G_ucDataBuffer[517 + i] = G_SMC_EccByte[i];
            else
                G_ucDataBuffer[525 + i] = G_SMC_EccByte[i];
        }
        switch(cnt){
            case 1:
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
            case 2:
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
            case 3:
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
            case 6:
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandDataPort];
            case 7:
                G_ucDataBuffer[519 + cnt] = Data;
                cnt ++;
                Data = XBYTE[REG_Fm_NandIpioRdata];
            case 8:
                G_ucDataBuffer[519 + cnt] = Data;
        }
     }
#endif
#endif
	//printf("ret: %bu\n", ret);
	return ret;
}

//-----------------------------------------------------------------------------
//SMC_AutoCmdSet
//-----------------------------------------------------------------------------
/* 
routine description:
		SMC_AutoCmdSet
arguments:
	Cmd : 
	address 
return value:
		None
*/
UINT8 SMC_AutoCmdSet(UINT8 const xdata* pCmd, UINT8 CmdCnt, UINT32 Address)
{
	UINT32 Addr;
	UINT32 clumAddr;
	UINT8 addrCnt;

	/*The max command count is two*/
	if (CmdCnt > 2) {
		return FAIL;
	}

	XBYTE[REG_Fm_waiten] = 0x13;   	//nand auto cmd/addr act/rcv time, wait rdy pin enable
	//MTD_NandAutoCmdStsClr();	

	XBYTE[REG_Fm_cmdbuf_0] = pCmd[0];
	XBYTE[REG_Fm_cmdbuf_1] = pCmd[1];
	
	if (!G_NANDF_Category) {
		if (G_SMC_AddrMode == 0) {
			Addr = Address;
			XBYTE[REG_Fm_cmdsize_addrsize] = 0x20 | CmdCnt; //two addr

			XBYTE[REG_Fm_addrbuf_0] = READ8(Addr, 3);
			XBYTE[REG_Fm_addrbuf_1] = READ8(Addr, 2);
			
		} else if (G_SMC_AddrMode == 1) {
			Addr = (Address >> 1);
			XBYTE[REG_Fm_cmdsize_addrsize] = 0x30 | CmdCnt ; //three addr

			XBYTE[REG_Fm_addrbuf_0] = READ8(Addr, 3);
			XBYTE[REG_Fm_addrbuf_1] = READ8(Addr, 2);
			XBYTE[REG_Fm_addrbuf_2] = READ8(Addr, 1);
			
		} else if (G_SMC_AddrMode == 2) {
			Addr = (Address >> 1);
			XBYTE[REG_Fm_cmdsize_addrsize] = 0x40 | CmdCnt; //four addr

			XBYTE[REG_Fm_addrbuf_0] = READ8(Address, 3);
			XBYTE[REG_Fm_addrbuf_1] = READ8(Addr, 2);
			XBYTE[REG_Fm_addrbuf_2] = READ8(Addr, 1);
			XBYTE[REG_Fm_addrbuf_3] = READ8(Addr, 0);
		}

	} else  {
		clumAddr = ((UINT16)Address) & (G_NANDF_PageSize -1);
		
		XBYTE[REG_Fm_addrbuf_0] = READ8(clumAddr, 1);
		XBYTE[REG_Fm_addrbuf_1] = READ8(clumAddr, 0);

		Address >>= 3;
		XBYTE[REG_Fm_addrbuf_2] = READ8(Address, 2);
		XBYTE[REG_Fm_addrbuf_3] = READ8(Address, 1);

		addrCnt = 0x40;

		//if (G_Card_TotalSizeMB > 128) {//mantis-29575,29768
		if (G_SMC_AddrMode > 3) {
			XBYTE[REG_Fm_addrbuf_4] = READ8(Address, 0);
			addrCnt = 0x50;
		}

		XBYTE[REG_Fm_cmdsize_addrsize] = addrCnt | CmdCnt; //four / five addr cycle
 		
	}
	
       XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|(XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT));   //   0    0    1    0
	return SUCCESS;
}

#endif


#endif //DISK_DRV_NAND

