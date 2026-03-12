#define __FILE	__FILE_ID_SMC_2533__
#include "general.h"
#include "reg_def.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_nand.h"

#ifdef DISK_DRV_NAND

#if 1//(K_SUPPORT_NEWNAND)
#include "main.h"
#include "initio.h"
#include "setmode.h"
#include "hal_dram.h"

#if (SMC || NAND)
#include "asicreg.h"
//#include "cardcon.h"
//#include "cardfunc.h"
#include "cardvar.h"

#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"
//#include "dosusr.h" 
#include "doslink.h" 
#include "smc2func.h"
#include "uiflow.h"

#include "mtd_general.h"
#include "mtd_macro_api.h"
#include "reg_def.h"

// lryy  for 4Gbit nand ECC correction
#include "impt.h"
extern  xdata	UINT8	G_ucDataBuffer[K_DMA_MaxSize];

//extern BIT data sramflag;

#define K_DEBUG_SMC2	0

extern data BIT G_USB_IO_Bulk2;
extern xdata UINT16	G_Card_TotalSizeMB;

#if (K_Error_Recovery)
#if 0//(K_SUPPORT_ONLYNEWNAND)
//UINT8 SMC_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress) 
#else
UINT8 SMC2_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress) 
#endif
{
	UINT8 sts = TRUE;
#if 0//ndef MTD_NEW	
	UINT8 k;

	for (k=0; k < 6; k++)
		G_SMC_EccByte[k] = 0xFF;
        sts = SMC2_WriteStart(startAddress);
#endif
	if (sts)
		sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_WriteEnd();
#else
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);
	
	MTD_NandCtrl(0x03);
#endif
	return sts;
}
#endif

#if 0//(K_SUPPORT_ONLYNEWNAND)
UINT8 SMC_RedunCard2Sram(UINT32 startAddress) 
#else
UINT8 SMC2_RedunCard2Sram(UINT32 startAddress) 
#endif
{
	UINT8 sts;
	UINT16 columnAddress, spareOffset;
	UINT8 tmp;
	
#if 0
	sts = SMC2_ReadStart(startAddress);
	if (sts)
		sts = SMC2_DMAReadRedundantArea(startAddress, FALSE);
#if 0 //ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif
#else

	columnAddress = ((UINT16)startAddress) & (G_NANDF_PageSize - 1);

	// write 00h
	MTD_NandSendCmd(K_SMC_Read1); 

	// write address
	SMC2_ReadWriteAddressLatchCycle(startAddress, columnAddress);

	// write 30h
	MTD_NandSendCmd(0x30); 

	/* timer start */
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);
	
	if (!(XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01)) {
		HAL_NANDChkRdy();
	}
	/* timer end -> 6.4us */
	
	/* timer start */
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0A & (~WPBIT))|tmp; // CLE enable
	XBYTE[REG_Fm_NandDataPort] = 0x05; //cmd 0x05

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;  //   0    1    1    0
	XBYTE[REG_Fm_NandDataPort] = READ8(spareOffset, 1);         	//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = READ8(spareOffset, 0) & 0x0f;  	//A8~A10, column address
	
	MTD_NandSendCmd(0xE0);
	/* timer end -> 4us */

	/* set sram dma address to 0x0e00 */
	XBYTE[REG_Cpu_SramRstIdxL] = 0x00;
	XBYTE[REG_Cpu_SramRstIdxH] = 0x0e;

	/* timer start */
	/*Configure sram DMA*/
	XBYTE[REG_Dma_DmaSrcDst] = (0x01 << 4) | 0x02; //storage to sram
	
	XBYTE[REG_Dma_DmaIdle] = 0x01;  // reset DMA machine
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* dma size: 15+1 bytes */
	XBYTE[REG_Dma_DmaSizeL] = 15;
	XBYTE[REG_Dma_DmaSizeM] = 0;

	XBYTE[REG_Cpu_MemEn] |= 0x07;		// enable sram
	XBYTE[REG_Cpu_SramMod] = 0x12;	
	//sramflag = 1;
	HAL_NANDSramFlagSet();
	/* timer end -> 4us */
	
	/* timer start */
	if (!(XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01)) {
		HAL_NANDChkRdy(); // 5us
	}
	/* timer end -> 0.6us */
	
	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/ // 6us
	sts = HAL_NANDDoDMAWait();
	if (sts != SUCCESS) {
		sts = FALSE;
	} else {
		sts = TRUE;
	}
	
#endif

	MTD_NandCtrl(0x03);
	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
//UINT8 SMC_EraseOneBlock(UINT32 startAddress) 
#else
UINT8 SMC2_EraseOneBlock(UINT32 startAddress) 
#endif
{

	UINT8 sts = TRUE;
#if 0//ndef MTD_NEW
	UINT8 ioStatus;

	//SMC_InitializeOperation();					  	//initialize
	SMC_CheckReady();
	HAL_NANDSendCmd(K_SMC_BlockErase1); 			  //command = Block Erase 1st cycle

	SMC2_EraseAddressLatchCycle(startAddress);

	HAL_NANDSendCmd(K_SMC_BlockErase2); 			  //command = Block Erase 2nd cycle

	if (SMC_CheckReady()) {
		HAL_NANDSendCmd(K_SMC_ReadStatus);  	  //command = Read Status
		ioStatus = HAL_NANDReadPort();  			  //I/O status
		if (ioStatus & 0x01)
			sts = FALSE;
		else
			sts = TRUE;
	} else {
		HAL_NANDSendCmd(K_SMC_Reset);   		  //reset
		sts = FALSE;
#if (K_DEBUG_SMC2)
		printf("SMC2_EraseOneBlock fail %lx\n", startAddress);
#endif
	}
	
	HAL_NANDCompleteOperation();
#else

	UINT8 cmdBuf[2] = {K_SMC_BlockErase1, K_SMC_BlockErase2}; 
	UINT8 addrBuf[6];
	UINT8 addrCnt = 2;

	startAddress >>= 3;
	MTD_SelfShiftR8(startAddress);
	addrBuf[0] = READ8(startAddress, 3);
	addrBuf[1] = READ8(startAddress, 2);
	addrBuf[2] = READ8(startAddress, 1);

	//if (G_Card_TotalSizeMB > 128)//mantis-29575,29768
	if (G_SMC_AddrMode > 3) {
		addrCnt = 3;
	}
	HAL_NANDAutoCmdCfg(cmdBuf, 2, addrBuf, addrCnt);

	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);
	
	MTD_NandAutoCmdTrig();

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

#endif
	return sts;
}
#if 0
UINT8 MTD_DMAReadRedunCfg(UINT32 startAddress)
{
	UINT8 sts = TRUE;	
	UINT16 columnAddress;
	UINT16 spareOffset;
	UINT8 cmdBuf[2] = {0x05, 0xE0}; 
	UINT8 addrBuf[6] = {0}; 

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);
	
	addrBuf[0] = READ8(spareOffset, 3);         	//A0~A7, column address
	addrBuf[1] = READ8(spareOffset, 2) & 0x0f;  	//A8~A10, column address

	HAL_NANDAutoCmdCfg(cmdBuf, 2, addrBuf, 2);
	return SUCCESS;
}
#endif
//76us
UINT8 SMC2_DMAReadRedundantArea(UINT32 startAddress, UINT8 checkEcc)
{
	//UINT8 error;
	UINT8 sts = TRUE;	
	UINT16 columnAddress;
	UINT16 spareOffset;
	UINT8 tmp;
#if 0 //ndef MTD_NEW
XBYTE[REG_Fm_cmdsize_addrsize] = 0; //flush auto command count
	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);
HAL_NANDChkRdy();

	SMC2_ReadRandomColumn(spareOffset);

	HAL_DmaSetSramAddr(0x0200);
	error = HAL_DmaDo(2, 1, 16, 0, 0);
	if (error != 0) {
		sts = FALSE;
	}
	if (checkEcc)
		sts = SMC_CheckEcc();
#else

	/* timer start */
	columnAddress = startAddress & (G_NANDF_PageSize - 1); 
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);

	if (!(XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01)) {
		HAL_NANDChkRdy();
	}
	/* timer end -> 6.4us */
	 
	/* timer start */
	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);	
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}
	/* timer end -> 23us */
	
	/* timer start */
#if 0	
	SMC2_ReadRandomColumn(spareOffset); // 11us
#else
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0A & (~WPBIT))|tmp; // CLE enable
	XBYTE[REG_Fm_NandDataPort] = 0x05; //cmd 0x05
	//MTD_NandSendCmd(0x05);

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;   				//   0    1    1    0
	XBYTE[REG_Fm_NandDataPort] = READ8(spareOffset, 1);         	//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = READ8(spareOffset, 0) & 0x0f;  	//A8~A10, column address
	//XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x02; 
	
	MTD_NandSendCmd(0xE0);
#endif
	/* timer end -> 4us */

#if 1
	/* set sram dma address to 0x0e00 */
	XBYTE[REG_Cpu_SramRstIdxL] = 0x00;
	XBYTE[REG_Cpu_SramRstIdxH] = 0x0e;
#else
	HAL_DmaSetSramAddr(0x0e00); // 7us
#endif
#if 1

#if 1

#if 0	
	/*Configure sram DMA*/
	HAL_NANDDoDMACfg(2, 1, 16, 0); // 14us
#else
	/* timer start */
	/*Configure sram DMA*/
	XBYTE[REG_Dma_DmaSrcDst] = (0x01 << 4) | 0x02; //storage to sram
	
	XBYTE[REG_Dma_DmaIdle] = 0x01;  // reset DMA machine
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* dma size: 15+1 bytes */
	XBYTE[REG_Dma_DmaSizeL] = 15;
	XBYTE[REG_Dma_DmaSizeM] = 0;

	XBYTE[REG_Cpu_MemEn] |= 0x07;		// enable sram
	XBYTE[REG_Cpu_SramMod] = 0x12;	
	//sramflag = 1;
	HAL_NANDSramFlagSet();
	/* timer end -> 4us */
#endif
	/* timer start */
	if (!(XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01)) {
		HAL_NANDChkRdy(); // 5us
	}
	/* timer end -> 0.6us */

	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/ // 6us
	sts = HAL_NANDDoDMAWait();

	if (sts != SUCCESS) {
		return FALSE;
	} else {
		sts = TRUE;
	}

#else	
	error = HAL_DmaDo(2, 1, 16, 0, 0); // 82 us
	if (error != 0) {
		sts = FALSE;
	}
#endif	
	if (checkEcc)
		sts = SMC_CheckEcc(); // 8us

#else

	sts = SMC_ChkEcc();
	if (sts) {
		CPrintf("E");
		G_SMC_EccErrCount++;
	}
#endif
#endif

	return sts;
}
#if 0
UINT8 MTD_DMAWriteRedunCfg(UINT32 startAddress, UINT16 blockAddress)
{
	UINT16 columnAddress;
	UINT16 spareOffset;
	UINT8 cmdBuf[2] = {K_SMC_RandomDataInput}; 
	UINT8 addrBuf[6] = {0}; 

	columnAddress = startAddress & (G_NANDF_PageSize-1);
	spareOffset = G_NANDF_PageSize+(columnAddress>>5);
	
	addrBuf[0] = READ8(spareOffset, 3);         	//A0~A7, column address
	addrBuf[1] = READ8(spareOffset, 2) & 0x03;  	//A8~A10, column address
	
	HAL_NANDAutoCmdCfg(cmdBuf, 1, addrBuf, 2);
	
	G_ucStorData[K_Card_PageSize + 0] = 0xff;
	G_ucStorData[K_Card_PageSize + 1] = 0xff;
	G_ucStorData[K_Card_PageSize + 2] = 0xff;
	G_ucStorData[K_Card_PageSize + 3] = 0xff;

	//Data Status Byte
	G_ucStorData[K_Card_PageSize + 4] = 0xff;

	//Block Status Byte
	if (G_BadBlk_Mark) {
		G_BadBlk_Mark = 0;
		printf("Bad %d\n", blockAddress);
		G_ucStorData[K_Card_PageSize + 5] = 0xf0;
	} else {
		G_ucStorData[K_Card_PageSize + 5] = 0xff;
	}

	//Block Address Field-1 and Block Address Field-2
	G_ucStorData[K_Card_PageSize + 6] = READ8(blockAddress, 0); 
	G_ucStorData[K_Card_PageSize + 7] = READ8(blockAddress, 1); 

	//ECC Field-2
	G_ucStorData[K_Card_PageSize + 8] = XBYTE[REG_Fm_ECC_3];
	G_ucStorData[K_Card_PageSize + 9] = XBYTE[REG_Fm_ECC_4];	
	G_ucStorData[K_Card_PageSize + 10] = XBYTE[REG_Fm_ECC_5];
	G_ucStorData[K_Card_PageSize + 11] = READ8(blockAddress, 0); 
	G_ucStorData[K_Card_PageSize + 12] = READ8(blockAddress, 1); 

	//ECC Field-1
	G_ucStorData[K_Card_PageSize + 13] = XBYTE[REG_Fm_ECC_0];
	G_ucStorData[K_Card_PageSize + 14] = XBYTE[REG_Fm_ECC_1];
	G_ucStorData[K_Card_PageSize + 15] = XBYTE[REG_Fm_ECC_2];

	return SUCCESS;
}
#endif 
UINT8 SMC2_DMAWriteRedundantArea(UINT32 startAddress, UINT16 blockAddress)
{
#if 0//ndef MTD_NEW
	UINT8 error;
#endif
	UINT8 sts = TRUE;
	UINT16 columnAddress;
	UINT16 spareOffset;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);
	SMC2_WriteRandomColumn(spareOffset);
	SMC_FillRedunArea(blockAddress, &G_BadBlk_Mark);  
#if 0//ndef	MTD_NEW
	HAL_DmaSetSramAddr(0x0200);
	error = HAL_DmaDo(1, 2, 16, 0, 0);
	if (error != 0)
		sts = FALSE;
#endif	
	return sts;
}


UINT8 SMC2_DMAWriteFromSram(UINT32 startAddress, UINT16 blockAddress)
{
	UINT8 sts = TRUE;
	UINT16 columnAddress;
#if 0//ndef MTD_NEW
	UINT8 error;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	SMC2_WriteRandomColumn(columnAddress);

	HAL_DmaSetSramAddr(0x0000);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	error = HAL_DmaDo(1, 2, K_Card_PageSize, 0, 0);
	if (error != 0) {
		return FALSE;
	}

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#else
	columnAddress = startAddress & (G_NANDF_PageSize - 1);

	/*Set sram DMA source address*/
	HAL_DmaSetSramAddr(0x0000);
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

	/*Nand send random data input*/
	MTD_NandSendCmd(K_SMC_RandomDataInput);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress);

	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();

	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/
	sts = HAL_NANDDoDMAWait();
	if (sts != SUCCESS) {
		return FALSE;
	}

	/*Disable ECC*/
	MTD_EccDisable();
#if 0
	/*Read ECC and configure auto command*/
	MTD_DMAWriteRedunCfg(startAddress, blockAddress);

	/*Trigger auto command/address mode*/
	MTD_NandAutoCmdTrig(); 
	
	HAL_DmaSetSramAddr(0x0e00);
	
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(1, 2, 16, 0);
	
	/*Wait auto command/address finished*/
	MTD_NandAutoCmdWait();
		
	/*Trigger sram DMA*/
	MTD_DMATrig();
	
	/*Wait sram DMA ready*/
	sts = HAL_NANDDoDMAWait();

	if (sts != SUCCESS) {
		return FALSE;
	}
	
	/*Send page program command to nand*/
	//MTD_NandSendCmd(K_SMC_PageProgram);
#else
	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#endif

#endif
	return sts;
}

UINT8 SMC2_DMAWriteFromDram(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr)
{
	UINT8 sts = TRUE;
	UINT16 columnAddress;
#if 0//ndef MTD_NEW
	UINT8 error;
	
	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	SMC2_WriteRandomColumn(columnAddress);

	HAL_DmaSetDramAddr(ramAddr);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	error = HAL_DmaDo(0, 2, K_Card_PageSize, 0, 0);
	if (error != 0) {
		return FALSE;
	}
	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#else
	columnAddress = startAddress & (G_NANDF_PageSize - 1);

	HAL_NANDSetDRAMDMA(ramAddr);

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(0, 2, K_Card_PageSize, 0);
	
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
	
	/*Nand send random data input*/
	MTD_NandSendCmd(K_SMC_RandomDataInput);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress);

	/*Phil: Ecc operation MUST NOT be set before nand send command*/
	
	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();
	
	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/
	sts = HAL_NANDDoDMAWait();	
	if (sts != SUCCESS) {
		return FALSE;
	}

	/*Disable ECC*/
	MTD_EccDisable();
#if 0
	/*Read ECC and configure auto command*/
	MTD_DMAWriteRedunCfg(startAddress, blockAddress);

	/*Trigger auto command/address mode*/
	MTD_NandAutoCmdTrig(); 
	
	HAL_DmaSetSramAddr(0x0e00);
	
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(1, 2, 16, 0);
	
	/*Wait auto command/address finished*/
	MTD_NandAutoCmdWait();
		
	/*Trigger sram DMA*/
	MTD_DMATrig();
	
	/*Wait sram DMA ready*/
	sts = HAL_NANDDoDMAWait();
#else
	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#endif
#endif
	return sts;
}


UINT8 SMC2_DMAWriteFromUsb(UINT32 startAddress, UINT16 blockAddress)
{
	UINT8 sts = TRUE;
	UINT16 columnAddress;
	UINT8 i;
#if 0//ndef MTD_NEW
	UINT8 error;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	SMC2_WriteRandomColumn(columnAddress);

	//	HAL_SetUSBDMA(K_USB_CLASS_DMA_OUT, K_DMA_UsbDontCare);//for class

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	//						async
	error = HAL_DmaDo(K_DMA_USB, K_DMA_FLASH, K_Card_PageSize, 1, 0);

	if (error != 0) {
		return FALSE;
	}
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

	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#else
	columnAddress = startAddress & (G_NANDF_PageSize - 1);

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
	
	/*Nand send random data input*/
	MTD_NandSendCmd(K_SMC_RandomDataInput);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress);

	/*Phil: Ecc operation MUST NOT be set before nand send command*/
	
	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();
	
	/*Trigger USB DMA without waiting*/
	MTD_DMATrig();	

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
		while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_OUT_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) { //wait pc ack
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
		XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		while ((XBYTE[REG_Dma_DmaCmp] != 0x01)&& (G_UI_USBConnect == K_UI_USB_CONNECT)){
			if ((XBYTE[REG_Usb_Status] & 0x10) == 0x10) break;
		}
	}

	/*Disable ECC*/
	MTD_EccDisable();
	sts = SMC2_DMAWriteRedundantArea(startAddress, blockAddress);
#endif
	return sts;

}

UINT8 SMC2_WriteStart(UINT32 startAddress)
{
	UINT8 sts = TRUE;
	UINT8 const cmdBuf[2] = {K_SMC_SequentialDataInput};
#if 0//ndef MTD_NEW
	UINT16 columnAddress;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	//SMC_InitializeOperation();					  	//initialize

	HAL_NANDSendCmd(K_SMC_SequentialDataInput); 	  //command = Sequential Data Input

	SMC2_ReadWriteAddressLatchCycle(startAddress, columnAddress);
#else
	SMC_AutoCmdSet(cmdBuf, 1, startAddress);
#endif
	return sts;
}

#if 0//ndef MTD_NEW
UINT8 SMC2_WriteEnd()
{
	UINT8 sts = TRUE;
	UINT8 ioStatus;

	HAL_NANDSendCmd(K_SMC_PageProgram);	//command = Page Program
	if (SMC_CheckReady()) {
		HAL_NANDSendCmd(K_SMC_ReadStatus);//command = Read Status
		ioStatus = HAL_NANDReadPort();  	  //I/O status

		// @patch5.1@wyeo@fix new nand program fail@begin
#if 0
	if (ioStatus & 0x01)
	{
		// page program fail
		#if (K_DEBUG_SMC2)
		printf("SMC2_WriteEnd fail\n");
		#endif
		sts = FALSE;
	}
	#endif
		// @patch5.1@wyeo@fix new nand program fail@end
	} else {
		HAL_NANDSendCmd(K_SMC_Reset);	//reset
		sts = FALSE;
	}

	HAL_NANDCompleteOperation();

	return sts;
}
#endif

UINT8 SMC2_ReadStart(UINT32 startAddress)
{
	UINT8 sts = TRUE;
#if 0 //ndef MTD_NEW
	UINT16 columnAddress;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);

	//SMC_InitializeOperation();					  	//initialize

	// write 00h
	HAL_NANDSendCmd(K_SMC_Read1); 

	// write address
	SMC2_ReadWriteAddressLatchCycle(startAddress, columnAddress);

	// write 30h
	HAL_NANDSendCmd(0x30); 

	sts = SMC_CheckReady();

	if (sts == FALSE) {
#if (K_DEBUG_SMC2)
		printf("SMC2_ReadStart fail 1\n");
#endif

		HAL_NANDSendCmd(K_SMC_Reset);     //reset
		HAL_NANDCompleteOperation();			   	//complete
	}
#else
	UINT8 cmdBuf[2] = {K_SMC_Read1, 0x30};

#if 1
	SMC_AutoCmdSet(cmdBuf, 2, startAddress);
#else
	UINT8 addrBuf[6] = {0};
	UINT8 addrCnt;


	columnAddress = startAddress & (G_NANDF_PageSize - 1);

	addrBuf[0] = READ8(columnAddress, 1);
	addrBuf[1] = READ8(columnAddress, 0);

	startAddress >>= 3;
	addrBuf[2] = READ8(startAddress, 2);
	addrBuf[3] = READ8(startAddress, 1);
	addrBuf[4] = READ8(startAddress, 0);		

	//if (G_Card_TotalSizeMB > 128)//mantis-29575,29768
	if (G_SMC_AddrMode > 3) {
		addrCnt = 5;
	} else {
		addrCnt = 4;
	}

	HAL_NANDAutoCmdCfg(cmdBuf, 2, addrBuf, addrCnt);
#endif	
#endif
	return sts;
}
#if 0//ndef MTD_NEW
UINT8 SMC2_ReadEnd()
{
	HAL_NANDCompleteOperation();					   //complete

	return TRUE;
}
#endif
UINT8 SMC2_DMAReadToSram(UINT32 startAddress)
{
	UINT8 sts = TRUE;
	UINT16 columnAddress;
#if 0 //ndef MTD_NEW
	UINT8 error;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	SMC2_ReadRandomColumn(columnAddress);

	HAL_DmaSetSramAddr(0x0000);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	error = HAL_DmaDo(2, 1, K_Card_PageSize, 0, 0);
	if (error != 0) {
		return FALSE;
	}

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	sts = SMC2_DMAReadRedundantArea(startAddress, TRUE);

#if ( K_SMC_DO_ECC_CORECTION )
	if (sts == FALSE) {
#if (K_DEBUG_SMC2)
		printf("SMC2_DMAReadToSram ecc error=%lx\n", startAddress);
#endif
		sts = SMC_CorrectPage(2, 0, 1);
		if (!sts) {
			sts = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}
#endif

	//for (k=0; k < 512; k+=2)
	//	printf("G_ucStorData[%x]=%bx %bx\n", k, G_ucStorData[k], G_ucStorData[k+1]);
#else
	columnAddress = startAddress & (G_NANDF_PageSize-1);

	/*Set sram DMA source address*/
	HAL_DmaSetSramAddr(0x0000);
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 1, K_Card_PageSize, 0);
	
	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();
	
	/*Check if auto command buffer ready*/
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
//		CPrintf("R2S");
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}

	/*Nand send random data output*/
	MTD_NandSendCmd(0x05);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress);
	/*Nand send random data output*/
	MTD_NandSendCmd(0xE0);

	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();

	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/
	sts = HAL_NANDDoDMAWait();
	if (sts != SUCCESS) {
		return FALSE;
	}

	/*Disable ECC*/
	MTD_EccDisable();
	/*Check ECC*/
	sts = SMC2_DMAReadRedundantArea(startAddress, TRUE);
	
	#if ( K_SMC_DO_ECC_CORECTION )
	if (sts == FALSE)
	{
		#if (K_DEBUG_SMC2)
		printf("SMC2_DMAReadToSram ecc error=%lx\n", startAddress);
		#endif
		sts = SMC_CorrectPage(2, 0, 1);
		if( !sts ){
			sts = SMC_BadBlockMark(startAddress/G_NANDF_BlockSize);
		}
	}
	#endif

#endif
	return sts;
}

UINT8 SMC2_DMAReadToDram(UINT32 startAddress, UINT32 ramAddr)
{
	UINT8 sts = TRUE;
	volatile UINT8 data Data;
	UINT16 spareOffset;
	UINT8 cmdBuf[2] = {0x05, 0xE0};
	UINT8 addBuf[6];	
	UINT16 columnAddress;
	UINT8 cnt = 0, i; 
#if 0//ndef MTD_NEW
	UINT8 error;

	columnAddress = startAddress & (G_NANDF_PageSize - 1);
	SMC2_ReadRandomColumn(columnAddress);

	HAL_DmaSetDramAddr(ramAddr);

	HAL_ECCMode(0); 								 //enable ECC
	HAL_ClearECC(); 								 //clear ECC

	error = HAL_DmaDo(2, 0, K_Card_PageSize, 0, 0);

	if (error != 0) {
		return FALSE;
	}

	HAL_ECCMode(1); 								 //disable ECC
	HAL_ReadECC(6, G_SMC_EccByte);

	sts = SMC2_DMAReadRedundantArea(startAddress, TRUE);
#if ( K_SMC_DO_ECC_CORECTION )
	if (sts == FALSE) {
#if (K_DEBUG_SMC2)
		printf("SMC2_DMAReadToDram ecc error=%lx\n", startAddress);
#endif

		sts = SMC_CorrectPage(2, ramAddr, 1);
		if (!sts) {
			sts = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}
#endif
#else
	columnAddress = startAddress & (G_NANDF_PageSize-1); // 6us

	/*Set dram DMA source address*/
	HAL_NANDSetDRAMDMA(ramAddr); // 12us
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 0, K_Card_PageSize, 0); // 14us
	
	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy(); // 5us
	
	/*Check if auto command buffer ready*/  //27us
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
//		CPrintf("R2D");	
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}
	/*Nand send random data output*/
	MTD_NandSendCmd(0x05);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress); // 7us
	/*Nand send random data output*/ // 3us
	MTD_NandSendCmd(0xE0);

	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();

	/*Trigger sram DMA*/
	MTD_DMATrig();	
#if 1
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);
	addBuf[0] = READ8(spareOffset, 1);         	//A0~A7, column address
	addBuf[1] = READ8(spareOffset, 0) & 0x0f;         	//A8~A10, column address
	HAL_NANDAutoCmdCfg(cmdBuf, 2, addBuf, 2);
#endif	
	/*Wait sram DMA ready*/ // 41us
	if (HAL_NANDDoDMAWait() != SUCCESS) {
		return FALSE;
	}

	/*Disable ECC*/
	MTD_EccDisable();
	
#if 0
	/*Check ECC*/
	sts = SMC2_DMAReadRedundantArea(startAddress, TRUE);	// 144us
	//sts = TRUE;	// 144us

	#if ( K_SMC_DO_ECC_CORECTION )
	if (sts == FALSE)
	{
		#if (K_DEBUG_SMC2)
		printf("SMC2_DMAReadToSram ecc error=%lx\n", startAddress);
		#endif
		sts = SMC_CorrectPage(2, 0, 1);
		if( !sts ){
			sts = SMC_BadBlockMark(startAddress/G_NANDF_BlockSize);
		}
	}
	#endif
#else
	sts = TRUE;
#if 0
	spareOffset = G_NANDF_PageSize + (columnAddress >> 5);

	SMC2_ReadRandomColumn(spareOffset); // 24us	
//	HAL_NANDChkRdy();
#else
{
	/*Trigger auto command/address mode*/
	MTD_NandAutoCmdTrig(); 
	/*Wait auto command/address finished*/
	MTD_NandAutoCmdWait();
}
#endif
#if 1
	Data = XBYTE[REG_Fm_NandDataPort];  // 0
	Data = XBYTE[REG_Fm_NandDataPort];  // 1
	Data = XBYTE[REG_Fm_NandDataPort];  // 2
	Data = XBYTE[REG_Fm_NandDataPort];  // 3
	Data = XBYTE[REG_Fm_NandDataPort];  // 4
	Data = XBYTE[REG_Fm_NandDataPort];  // 5
	Data = XBYTE[REG_Fm_NandDataPort];  // 6
	Data = XBYTE[REG_Fm_NandDataPort];  // 7
	Data = XBYTE[REG_Fm_NandDataPort];  // 8

#if 0 //xian fix bug of ecc correct
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 8 trig 9
	if (Data != XBYTE[REG_Fm_ECC_3]) {
		sts = FALSE;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 9 trig 10
	if (Data != XBYTE[REG_Fm_ECC_4]) {
		sts = FALSE;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 10 trig 11
	if (Data != XBYTE[REG_Fm_ECC_5]) {
		sts = FALSE;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 11 trig 12
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 12 trig 13
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 13 trig 14
	if (Data != XBYTE[REG_Fm_ECC_0]) {
		sts = FALSE;
	}
	Data = XBYTE[REG_Fm_NandDataPort];  // dat 14 trig 15
	if (Data != XBYTE[REG_Fm_ECC_1]) {
		sts = FALSE;
	}
	//Data = XBYTE[REG_Fm_NandDataPort];  // 15
	Data = XBYTE[REG_Fm_NandIpioRdata];
	if (Data != XBYTE[REG_Fm_ECC_2]) {
		sts = FALSE;
	}
#else //xian ++ to fix ecc bug
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
#endif

//	if (sts  != TRUE) {
    if(cnt) { 
        //xian ++ to fix ecc bug
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
        //xian --
    
		CPrintf("1E");
		G_SMC_EccErrCount++;
#if ( K_SMC_DO_ECC_CORECTION )

#if (K_DEBUG_SMC2)
		printf("SMC2_DMAReadToSram ecc error=%lx\n", startAddress);
#endif
		sts = SMC_CorrectPage(2, ramAddr, 1); 
		if( !sts ){
		sts = SMC_BadBlockMark(startAddress/G_NANDF_BlockSize);
		}

#endif
	} 
#else	
	
	if (SMC_ChkEcc()	 != SUCCESS) {
		CPrintf("1E");
		G_SMC_EccErrCount++;
	} 
#endif

#endif
#endif
	return sts;
}

UINT8 SMC2_ReadRandomColumn(UINT16 columnAddress)
{
	UINT8 sts = TRUE;

	// write 05h, random data output in a page
	MTD_NandSendCmd(0x05);
	SMC2_ReadWriteColumnLatchCycle(columnAddress);

	// write E0h, random data output in a page
	MTD_NandSendCmd(0xE0);
	
	return sts;
}
#if 0//ndef MTD_NEW
UINT8 SMC2_WriteRandom(UINT32 startAddress, UINT16 columnAddress)
{
	HAL_NANDSendCmd(0x85); 
	SMC2_ReadWriteAddressLatchCycle(startAddress, columnAddress); 

	return TRUE;
}
#endif
UINT8 SMC2_WriteRandomColumn(UINT16 columnAddress)
{
	UINT8 sts = TRUE;

	// random data input
	HAL_NANDSendCmd(0x85);  
	SMC2_ReadWriteColumnLatchCycle(columnAddress);

	return sts;
}
void SMC2_ReadWriteAddressLatchCycle(UINT32 startAddress, UINT16 columnAddress)
{
#if 0//ndef MTD_NEW
	// CLE  ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x06;   				//   0    1    1	0
	
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (columnAddress); 			//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (columnAddress >> 8);  	//A8~A11, column address

	XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 11); //A11~A18, row address
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 19); //A19~A26, row address
	//if (G_Card_TotalSizeMB > 128)//mantis-29575,29768
	if (G_SMC_AddrMode > 3)
		XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 27); //A27, row address

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x02;   //   0    0    1	0
#else
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);
	// CLE	ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;					//	 0	  1    1	0

	XBYTE[REG_Fm_NandDataPort] = READ8(columnAddress, 1);			//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = READ8(columnAddress, 0);	//A8~A11, column address

	startAddress >>= 3;
	XBYTE[REG_Fm_NandDataPort] = READ8(startAddress, 2); //A11~A18, row address
	XBYTE[REG_Fm_NandDataPort] = READ8(startAddress, 1); //A19~A26, row address
	//if (G_Card_TotalSizeMB > 128)//mantis-29575,29768
	if (G_SMC_AddrMode > 3)
		XBYTE[REG_Fm_NandDataPort] = READ8(startAddress, 0); //A27, row address

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;	//	 0	  0    1	0
	
#endif /* MTD_NEW */
}
void SMC2_ReadWriteColumnLatchCycle(UINT32 startAddress)
{
#if 0//ndef MTD_NEW
                                				// CLE  ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x06;   				//   0    1    1	0
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress & 0xff);  	   	//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = (UINT8) ((startAddress >> 8) & 0x0f);  	//A8~A10, column address
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x02;   //   0    0    1	0
#else
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;   				//   0    1    1    0
	XBYTE[REG_Fm_NandDataPort] = READ8(startAddress, 3);         	//A0~A7, column address
	XBYTE[REG_Fm_NandDataPort] = READ8(startAddress, 2) & 0x0f;  	//A8~A10, column address
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;   //   0    0    1    0
	
#endif
}
#if 0//ndef MTD_NEW
void SMC2_EraseAddressLatchCycle(UINT32 startAddress)
{
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x06;
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 11); //A11~A18, row address
	XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 19); //A19~A26, row address

	//if (G_Card_TotalSizeMB > 128)//mantis-29575,29768
	if (G_SMC_AddrMode > 3)
		XBYTE[REG_Fm_NandDataPort] = (UINT8) (startAddress >> 27); //A27, row address

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = 0x02;
}
#endif /* MTD_NEW */
#endif

#if 0//(K_SUPPORT_ONLYNEWNAND)
UINT8 SMC_Sram2Card(UINT32 startAddress, UINT16 blockAddress) 
#else
UINT8 SMC2_Sram2Card(UINT32 startAddress, UINT16 blockAddress) 
#endif
{
	UINT8 sts;

	sts = SMC2_WriteStart(startAddress);
	if (sts)
		sts = SMC2_DMAWriteFromSram(startAddress, blockAddress);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_WriteEnd();
#else
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	MTD_NandCtrl(0x03);
#endif

	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
//UINT8 SMC_Card2Sram(UINT32 startAddress) 
#else
UINT8 SMC2_Card2Sram(UINT32 startAddress) 
#endif
{
	UINT8 sts;
	sts = SMC2_ReadStart(startAddress);
	if (sts)
		sts = SMC2_DMAReadToSram(startAddress);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif

//printf("%s(%ld) Addr=%lx\n",__FILE__,(UINT32)__LINE__,startAddress);
	#if 0
	dumpData( 16 , G_ucStorData, 0);
	#endif
	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
//UINT8 SMC_Dram2Card(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr) 
#else
UINT8 SMC2_Dram2Card(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr) 
#endif
{
	UINT8 sts;

	sts = SMC2_WriteStart(startAddress);
	if (sts)
		sts = SMC2_DMAWriteFromDram(startAddress, blockAddress, ramAddr);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_WriteEnd();
#else
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	MTD_NandCtrl(0x03);
#endif
	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
//UINT8 SMC_Card2Dram(UINT32 startAddress, UINT32 ramAddr) 
#else
UINT8 SMC2_Card2Dram(UINT32 startAddress, UINT32 ramAddr) 
#endif
{
	UINT8 sts;

	sts = SMC2_ReadStart(startAddress);
	if (sts)
		sts = SMC2_DMAReadToDram(startAddress, ramAddr);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif
	return sts;
}
void SMC2_ReadECC(UINT8 i)
{
	UINT8 xdata* data pData = &G_ucStorData[K_Card_PageSize+ i * 12];

	/* The first 512B */
	*pData++ = XBYTE[REG_Fm_ECC_3];		// ECC3
	*pData++ = XBYTE[REG_Fm_ECC_4];		// ECC4
	*pData++ = XBYTE[REG_Fm_ECC_5];	// ECC5
	*pData++ = XBYTE[REG_Fm_ECC_0];	// ECC0
	*pData++ = XBYTE[REG_Fm_ECC_1];	// ECC1
	*pData++ = XBYTE[REG_Fm_ECC_2];	// ECC2
	
	/* The continued 512B */
	*pData++ = XBYTE[REG_Fm_ECC_9];		// ECC9
	*pData++ = XBYTE[REG_Fm_ECC_a];		// ECC10
	*pData++ = XBYTE[REG_Fm_ECC_b];	// ECC11
	*pData++ = XBYTE[REG_Fm_ECC_6];	// ECC6
	*pData++ = XBYTE[REG_Fm_ECC_7];	// ECC7
	*pData++ = XBYTE[REG_Fm_ECC_8];	// ECC8
}

/*
* errIndicator used to indicate which sub-page is ecc-err.
*/
UINT8 SMC2_CheckECC(UINT8* errIndicator)
{
	UINT8 error;	
	UINT8 data i;
	UINT8 sts = TRUE;	
//	UINT16 columnAddress;
//	UINT16 spareOffset;
	UINT8 xdata* data pData;
	UINT8 errPosition = 0;	// ====> bit 0.1.2.3 indicate sub-Page 0.1.2.3

	/* 
		Read redundant area
	*/

	HAL_NANDChkRdy();
	SMC2_ReadRandomColumn(G_NANDF_PageSize); // 24us

	HAL_DmaSetSramAddr(0x0200 + 0x40); // 8us	
#if 0
{
UINT8 sts;
ENTER_CRITICAL(sts);
	/*Configure sram DMA*/
	HAL_NANDDoDMACfg(2, 1, 64, 0); // 14us
//	HAL_NANDChkRdy(); // 5us
	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/ // 41us
	sts = HAL_NANDDoDMAWait();
	if (sts != SUCCESS) {
		CPrintf("DMA FAIL");
		EXIT_CRITICAL(sts);	
		return FALSE;
	}
EXIT_CRITICAL(sts);	
}
#else	
	error = HAL_DmaDo(2, 1, 64, 0, 0); // 82 us
	if (error != 0) {
		sts = FALSE;
	}
#endif	
	/* 
		Check ECC 
	*/
	i = 3;
	do {
		pData = &G_ucStorData[K_Card_PageSize + i * 6];
		if (*pData++ != G_ucStorData[K_Card_PageSize + 8 + (i + 4) * (UINT16)16] ||\
			*pData++ != G_ucStorData[K_Card_PageSize + 9 + (i + 4) * (UINT16)16] ||\
			*pData++ != G_ucStorData[K_Card_PageSize + 10 + (i + 4) * (UINT16)16] ||\
			*pData++ != G_ucStorData[K_Card_PageSize + 13 + (i + 4) * (UINT16)16] ||\
			*pData++ != G_ucStorData[K_Card_PageSize + 14 + (i + 4) * (UINT16)16] ||\
			*pData++ != G_ucStorData[K_Card_PageSize + 15 + (i + 4) * (UINT16)16]
		) {
			CPrintf1("i = %bu", i);
			//break;
			G_SMC_EccErrCount++;
			sts = FALSE;
			errPosition |= (1<<i);	//if err, save position.
		}	
	} while (i--);

	/* lryy Grrr.. add, indicate which sub-page was wrong */
	*errIndicator = errPosition;
	
	return sts;
}
#if ( K_SMC_DO_ECC_CORECTION )
/*
* 
*
* this function copied from smc533 and modified a little.
* I think it should work well..
*/
UINT8 SMC2_CorrectPage(UINT32 ramAddr, UINT8 doCorrect, UINT16 sramOff)
{
	data UINT32 bitPat;
	data UINT32 eccLong = 0;
	data UINT8 j, k, byteOffset, a, b, bitOffset;
	UINT8 sts = TRUE;

	for (k = 0; k < 6 ;) 
	{
		if (k < 3)
		{
			eccLong |= ((UINT32) (G_ucDataBuffer[K_Card_PageSize + sramOff * 6 + k]  ^ \
						G_ucDataBuffer[K_Card_PageSize + 64 + sramOff * 16 + 8+ k]) << \
						(16 - ((k - 0) << 3)));

			/*
			printf("H[%u] = 0x%02bx\n",sramOff * 6 + k, \
					G_ucDataBuffer[K_Card_PageSize + sramOff * 6 + k]);
			
			printf("G[%u] = 0x%02bx\n",64 + sramOff * 16 + 8 + k, \
					G_ucDataBuffer[K_Card_PageSize + 64 + sramOff * 16 + 8 + k]);*/
		}
			
		else
		{
			eccLong |= ((UINT32) (G_ucDataBuffer[K_Card_PageSize + sramOff * 6 + k]  ^ \
						G_ucDataBuffer[K_Card_PageSize + 64 + sramOff * 16 + 10 + k]) << \
						(16 - ((k - 3) << 3)));

			/*
			printf("H[%u] = 0x%02bx\n",sramOff * 6 + k, \
					G_ucDataBuffer[K_Card_PageSize + sramOff * 6 + k]);

			printf("G[%u] = 0x%02bx\n",64 + sramOff * 16 + 10 + k, \
					G_ucDataBuffer[K_Card_PageSize + 64 + sramOff * 16 + 10 + k]);*/
		}

		k++;
		if (k == 3 || k == 6) {
			if (eccLong != 0) {
				// CORRECTABLE is 0x00555554L
				if (((eccLong ^ (eccLong >> 1)) & 0x00555554L) == 0x00555554L) {
					// One bit ECC error
					//printf("ECC : 1bit err\n");


					bitPat = 0x00800000L; 	//  BIT23
					byteOffset = 0;
					a = 0x80; 		//  BIT7

					for (j = 0; j < 8; j++) {
						if ((eccLong & bitPat) != 0) {
							byteOffset |= a;
						}

						bitPat >>= 2;
						a >>= 1;
					}
					//printf("ECC byte off = %bu\n", byteOffset);

					bitOffset = 0;
					b = 0x04;		//  BIT2

					for (j = 0; j < 3; j++) {
						if ((eccLong & bitPat) != 0) {
							bitOffset |= b;
						}

						bitPat >>= 2;
						b >>= 1;
					}
					//printf("ECC bit  off = %bu\n", bitOffset);

					byteOffset = (((byteOffset & 0xF0) >> 4) | ((byteOffset & 0x0F) << 4));

					if (doCorrect) {
						if (bitOffset < 0x08) {
							if (ramAddr != 0) {
								// patch 4.4, 0615@wyeo
								M_AUDIO_DISABLE_8051_INT;
								if (k == 3)//lryy Grrr
									SDRAM_CardSetStartAddress(ramAddr + 256 * (UINT32)sramOff + 128 + (byteOffset >> 1), TRUE);
								else if (k == 6)
									SDRAM_CardSetStartAddress(ramAddr + 256 * (UINT32)sramOff + (byteOffset >> 1), TRUE);
								
								// @patch5.2.1-24@wyeo@fix bit correct@111203, end
								M_DRAM_ReadWord(a, b);
								
								if (byteOffset & 0x01)
									b ^= (0x01 << bitOffset);
								else
									a ^= (0x01 << bitOffset);
								
								if (k == 3)
									SDRAM_CardSetStartAddress(ramAddr + 256 * (UINT32)sramOff + 128 + (byteOffset >> 1), FALSE);
								else if (k == 6)
									SDRAM_CardSetStartAddress(ramAddr + 256 * (UINT32)sramOff + (byteOffset >> 1), FALSE);

								// @patch5.2.1-24@wyeo@fix bit correct@111203, end
								M_DRAM_WriteWord(a, b);
								M_AUDIO_ENABLE_8051_INT;
								// patch 4.4, 0615@wyeo
							} 
							
							/* smc2 read to dram fast no need handle sram situation */
							/*
							else {
								if (k == 3)
									G_ucDataBuffer[byteOffset] ^= (0x01 << bitOffset);
								else
									G_ucDataBuffer[(UINT16) byteOffset + 256] ^= (0x01 << bitOffset);
							}
							*/
						} else
							// should not happen
							sts = FALSE;
						G_SMC_Ecc1BitErrCount++;
					}
#if 0/*(K_SMC_DO_USB_ECC_BY_HOST)*/
					else {
						// record the ecc bit offset and fpos into G_DOS_SDRAM_WorkAddr
						ramAddr = G_SMC_Ecc1BitErrCount * 6;
						if (ramAddr + 6 < K_SMC_MAX_ClusterSize) {
							ramAddr = G_DOS_SDRAM_WorkAddr + (ramAddr >> 1);
							SDRAM_CardSetStartAddress(ramAddr, FALSE);
							DOS_FillLongAtSdram(G_DOS_FilePos + byteOffset);
							/*
															  M_DRAM_WriteWord(G_DOS_FilePos + byteOffset, 
															  		 (G_DOS_FilePos + byteOffset) >> 8);
															  M_DRAM_WriteWord((G_DOS_FilePos + byteOffset) >> 16, 
															  		 (G_DOS_FilePos + byteOffset) >> 24);
															  M_DRAM_WriteWord(bitOffset, 
															  		 bitOffset >> 8);
															 	*/
							G_SMC_Ecc1BitErrCount++;
						}
					}
#endif
				} else {
					// more than 1 bit error
					sts = FALSE;
					G_SMC_Ecc2BitErrCount++;
				}
			}
			eccLong = 0;
		}
	} // end for

	return sts;
}
#endif

UINT8 SMC2_CorrectAllPage(UINT32 ramAddr, UINT8 subPageMsk, UINT8 doCorrect)
{
	UINT8 i;
	UINT8 falseCnt = 0;
	UINT8 sts = TRUE;

	/* correct corresponding sub-page */
	for (i=0; i<4; i++)
	{
		if (subPageMsk & (1 << i))
		{
			if ( FALSE == SMC2_CorrectPage(ramAddr, doCorrect, i))
			{
				//printf("Do correct failed %bu\n",i);
				falseCnt++;
			}
		}
	}

	/* if more than 0 err, return false */
	if (falseCnt) 
		sts = FALSE;

	return sts;
}

#if 1
UINT8 SMC2_BadBlockMark(UINT16 blockAddress) 
{
#if	NAND_BAD_BLOCK_MARK == 1
	//UINT8 i;
	UINT8  pagePerBlock;
	UINT32 row;
	UINT8  sts = TRUE;
	UINT8  ioStatus;

	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);

	//startAddr = blockAddress * G_NANDF_BlockSize;
	//columnAddr = G_NANDF_PageSize;
	//printf("ECC MK addr = 0x%lx\n", startAddr);

	G_BadBlk_Mark = 1;

	HAL_NANDChkRdy();
	// Disable WriteProtect
	HAL_NANDEnableWriteProtect(0);

	/* Timing start */
	MTD_NandSendCmd(K_SMC_SequentialDataInput);

	/* address */
#if 0//Note : need verify
	SMC_ReadWriteAddressLatchCycle(startAddr & ~(G_NANDF_BlockSize - 1));
#else
	pagePerBlock = G_NANDF_BlockSize / G_NANDF_PageSize;
	row = blockAddress * pagePerBlock;
	
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;	//	 0	  1    1	0
	XBYTE[REG_Fm_NandDataPort] = 0;						// 1st cycle -- No column
	XBYTE[REG_Fm_NandDataPort] = 0;						// 2nd cycle -- No column
	XBYTE[REG_Fm_NandDataPort] = row & 0xFF; 			// 3rd cycle
	XBYTE[REG_Fm_NandDataPort] = (row & 0xFF00) >> 8;	// 4th cycle

	/*
	* Note : Here cannot use G_SMC_AddrMode because
	*  G_SMC_AddrMode can not indicate 4 cycle addr
	*  or 5 cycle addr.
	*/
	if (G_Card_TotalSizeMB > 128)
	{
		XBYTE[REG_Fm_NandDataPort] = (row & 0xFF0000) >> 16; // 5th cycle
	}
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;
#endif

	/* 0x85 cmd */
	MTD_NandSendCmd(K_SMC_RandomDataInput);
	
	/* column addr */
	MTD_ColumnLatch(G_NANDF_PageSize);	//for ex: 4G08 use col 2048 to store the bad blk mark

	/* data write */
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;	//	 0	  0    1	0	// set!!!!!
	XBYTE[REG_Fm_NandDataPort] = 0xF0;

	/*Send page program command to nand*/
	MTD_NandSendCmd(K_SMC_PageProgram);
	
	/* waiting for finish */
	sts = HAL_NANDChkRdy(); // 5us
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	/* 0x70 cmd */
	MTD_NandSendCmd(K_SMC_ReadStatus);
	ioStatus = HAL_NANDReadPort();

	/* finished */
	HAL_NANDCompleteOperation();
	
#else
	blockAddress = blockAddress;
#endif
	return sts;
}
#endif

UINT8 SMC2_Card2DramFast(UINT32 startAddress, UINT32 ramAddr) 
{
	UINT8 i = 0;
	UINT8 sts;
	UINT16 nrPage = G_NANDF_PageSize;
	UINT16 columnAddress;
	UINT8 errPos;
	UINT32 ramSave;

	ramSave = ramAddr;

	sts = SMC2_ReadStart(startAddress);
	if (sts){
		MTD_SelfShiftR8(nrPage);
		nrPage = nrPage >> 2;
		XBYTE[REG_Fm_ECCMode] = 0x02; //ECC mode set to 1024B	
		do {
#if 0
			sts = SMC2_DMAReadToDram(startAddress, ramAddr);
#else
	columnAddress = startAddress & (G_NANDF_PageSize-1); // 6us
	/*Set dram DMA source address*/
	HAL_NANDSetDRAMDMA(ramAddr); // 12us
	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(2, 0, 1024, 0); // 14us
	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy(); // 5us
	/*Check if auto command buffer ready*/  //27us
	if (XBYTE[REG_Fm_cmdsize_addrsize]) {
//		CPrintf("R2D");	
		/*Enable CE, Disable WP*/	
		MTD_NandCtrl(0x02);
		/*Trigger auto command/address mode*/
		MTD_NandAutoCmdTrig(); 
		/*Wait auto command/address finished*/
		MTD_NandAutoCmdWait();
	}
	/*Nand send random data output*/
	MTD_NandSendCmd(0x05);
	/*set address*/
	SMC2_ReadWriteColumnLatchCycle(columnAddress); // 7us
	/*Nand send random data output*/ // 3us
	MTD_NandSendCmd(0xE0);
	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();
	/*Trigger sram DMA*/
	MTD_DMATrig();	
	/*Wait sram DMA ready*/ // 41us
	sts = HAL_NANDDoDMAWait();
	if (sts != SUCCESS) {
		return FALSE;
	}
	/*Disable ECC*/
	MTD_EccDisable();
	SMC2_ReadECC(i++);
#endif
			startAddress += 1024L;
			ramAddr += 512L;
		}while(--nrPage); 
		
		sts = SMC2_CheckECC(&errPos);
		XBYTE[REG_Fm_ECCMode] = 0x01; //ECC mode set to 512B	


#if 1//LRYY Grrr For ECC Correction
	if (sts == FALSE)
	{
		ramSave = ramSave;
		//printf("sv = %ld\n",ramSave);
		//printf("bf = %ld\n",ramAddr - 1024);
		sts = SMC2_CorrectAllPage(ramSave/*ramAddr - 1024*/, errPos, 1);
		if (!sts) {
			printf("FB-%lx\n", startAddress / G_NANDF_BlockSize);
			sts = SMC2_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
	}
#endif
	}


	
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif
	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
UINT8 SMC_Usb2Card(UINT32 startAddress, UINT16 blockAddress) 
#else
UINT8 SMC2_Usb2Card(UINT32 startAddress, UINT16 blockAddress) 
#endif
{
	UINT8 sts;

	sts = SMC2_WriteStart(startAddress);
	if (sts)
		sts = SMC2_DMAWriteFromUsb(startAddress, blockAddress);
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_WriteEnd();
#else
	MTD_NandSendCmd(K_SMC_PageProgram);

	HAL_NANDChkRdy();
	// Enable WriteProtect
	HAL_NANDEnableWriteProtect(1);

	MTD_NandCtrl(0x03);
#endif

	return sts;
}

#if 0//(K_SUPPORT_ONLYNEWNAND)
UINT8 SMC_Card2Usb(UINT32 startAddress) 
#else
UINT8 SMC2_Card2Usb(UINT32 startAddress) 
#endif
{
	UINT8 sts ;

	sts = SMC2_ReadStart(startAddress);
	if (sts) {
		if (G_USB_IO_Bulk2 == 1) {
			//				   HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_AP_DMA_IN);//for ap
		} else {
			//				   HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_CLASS_DMA_IN);//for class
		}
		/*Check if auto command buffer ready*/
		if (XBYTE[REG_Fm_cmdsize_addrsize]) {
	//		CPrintf("R2D");	
			HAL_NANDChkRdy();
			/*Enable CE, Disable WP*/	
			MTD_NandCtrl(0x02);
			/*Trigger auto command/address mode*/
			MTD_NandAutoCmdTrig(); 
			/*Wait auto command/address finished*/
			MTD_NandAutoCmdWait();
		}
		SMC2_ReadRandomColumn(startAddress & (G_NANDF_PageSize - 1));
		sts = SMC2_DMAReadToUsb(startAddress);
	}
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif

	return sts;
}

UINT8 SMC2_Card2UsbFast(UINT32 startAddress) 
{
	UINT8 sts ;
	UINT16 nrPage = G_NANDF_PageSize;

	sts = SMC2_ReadStart(startAddress);
	if (sts) {
		if (G_USB_IO_Bulk2 == 1) {
			//				   HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_AP_DMA_IN);//for ap
		} else {
			//				   HAL_SetUSBDMA(K_DMA_UsbDontCare, K_USB_CLASS_DMA_IN);//for class
		}
		/*Check if auto command buffer ready*/
		if (XBYTE[REG_Fm_cmdsize_addrsize]) {
	//		CPrintf("R2D");	
			HAL_NANDChkRdy();
			/*Enable CE, Disable WP*/	
			MTD_NandCtrl(0x02);
			/*Trigger auto command/address mode*/
			MTD_NandAutoCmdTrig(); 
			/*Wait auto command/address finished*/
			MTD_NandAutoCmdWait();
		}
	}
	if (sts){
		MTD_SelfShiftR8(nrPage);
		nrPage = nrPage >> 1;
		do{
			SMC2_ReadRandomColumn(startAddress & (G_NANDF_PageSize - 1));
			sts = SMC2_DMAReadToUsb(startAddress);
			startAddress += 512L;
		}while(--nrPage); 
	}
#if 0//ndef MTD_NEW		
	if (sts)
		sts = SMC2_ReadEnd();
#else
	MTD_NandCtrl(0x03);
#endif

	return sts;
}
#if 0//(K_SUPPORT_ONLYNEWNAND)
UINT8 SMC_Card2SramForExt(UINT32 startAddress)
{
	return SMC_RedunCard2Sram(startAddress);
}
#endif

#if (K_NANDF_USE_COPYBACK)

UINT8 SMC2_CopyBackStart(UINT32 srcStartAddress) 
{	
#ifndef MTD_NEW
	UINT8 sts = TRUE;

	HAL_NANDSendCmd(0x00);
        SMC2_ReadWriteAddressLatchCycle(srcStartAddress, srcStartAddress & (G_NANDF_PageSize-1)); 
	HAL_NANDSendCmd(0x35);
	sts = SMC_CheckReady();
	
	return sts;
#else
	if (HAL_NANDChkRdy() != SUCCESS) {
		return FAIL;
	}
	MTD_NandSendCmd(0x00);
        SMC2_ReadWriteAddressLatchCycle(srcStartAddress, srcStartAddress & (G_NANDF_PageSize-1)); 
	MTD_NandSendCmd(0x35);
#endif

}

UINT8 SMC2_CopyAll512Pages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 copySize)
{
	UINT8 sts = TRUE;
	UINT16 srcHeadColumn;
	UINT16 headResidue;
	UINT32 tempSize;

	//printf("SMC2_CopyAll512Pages = %lx, %lx, %lx\n", srcStartAddress, dstStartAddress, copySize);
	srcHeadColumn = srcStartAddress & (G_NANDF_PageSize - 1);
	if (srcHeadColumn) {
		headResidue = G_NANDF_PageSize - srcHeadColumn;
		if (headResidue > copySize)
			headResidue = copySize;	
		if (headResidue) {
			sts = SMC2_Copy512Pages(srcStartAddress, dstStartAddress, dstBlockNum, headResidue >> 9);
			copySize -= headResidue;
			srcStartAddress += headResidue;
			dstStartAddress += headResidue;
		}
	}
	tempSize = copySize / G_NANDF_PageSize;
	if (sts && tempSize) {
		sts = SMC2_CopyPages(srcStartAddress, dstStartAddress, dstBlockNum, tempSize);
		//sts = SMC2_Copy512Pages(srcStartAddress, dstStartAddress, dstBlockNum, (tempSize * G_NANDF_PageSize)>>9);
		tempSize = tempSize * G_NANDF_PageSize;
		copySize -= tempSize;
		srcStartAddress += tempSize;
		dstStartAddress += tempSize;
	}

	if (sts && copySize) {
		sts = SMC2_Copy512Pages(srcStartAddress, dstStartAddress, dstBlockNum, copySize >> 9);
		srcStartAddress += copySize;
		dstStartAddress += copySize;
	}

	return sts;
}

UINT8 SMC2_CopyPages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 numOfPage)
{
	UINT8 sts = TRUE;
	UINT8 ioStatus;
	UINT32 k;

	if (numOfPage == 0)
		return FALSE;

	//printf("SMC2_CopyPages = %lx, %lx, %lx\n", srcStartAddress, dstStartAddress, numOfPage);
	for (k = 0; sts && (k < numOfPage); k++) {
		sts = SMC2_CopyBackStart(srcStartAddress);
		if (sts) {
			SMC2_WriteRandom(dstStartAddress, 0);
			HAL_NANDSendCmd(K_SMC_PageProgram);
			if (SMC_CheckReady()) {
				HAL_NANDSendCmd(K_SMC_ReadStatus);//command = Read Status
				ioStatus = HAL_NANDReadPort();  	  //I/O status
				if (ioStatus & 0x01) {
					// page program fail
#if (K_DEBUG_SMC2)
					printf("SMC2_CopyPages fail 1\n");
#endif
					// try not using copy back
					sts = SMC2_Copy512Pages(srcStartAddress, dstStartAddress, dstBlockNum, G_NANDF_PageSize >> 9);
				}
			} else {
#if (K_DEBUG_SMC2)
				printf("SMC2_CopyPages fail 2\n");
#endif
				HAL_NANDSendCmd(K_SMC_Reset);	//reset
				sts = FALSE;
			}
			HAL_NANDCompleteOperation();
		}
#if (K_DEBUG_SMC2)
		else
			printf("SMC2_CopyPages fail 3\n");
#endif		
		srcStartAddress += G_NANDF_PageSize;
		dstStartAddress += G_NANDF_PageSize;
	}

	return sts;
}

UINT8 SMC2_Copy512Pages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 numOf512)
{
	UINT8 sts = TRUE;
	UINT32 k;

	if (numOf512 == 0)
		return FALSE;

	//printf("SMC2_Copy512Pages=%lx, %lx, %bx, %bx\n", srcStartAddress, dstStartAddress, numOf512, writeSpare);

	for (k = 0; sts && (k < numOf512); k++) {
#if 0//(K_SUPPORT_ONLYNEWNAND)
		SMC_Card2Sram(srcStartAddress);
		SMC_Sram2Card(dstStartAddress, dstBlockNum);
#else
		SMC2_Card2Sram(srcStartAddress);
		SMC2_Sram2Card(dstStartAddress, dstBlockNum);
#endif
		srcStartAddress += 512;
		dstStartAddress += 512;
	}

	return sts;
}
#endif

//-----------------------------------------------------------------------------
//SMC2_DMAReadToUsb
//-----------------------------------------------------------------------------
UINT8 SMC2_DMAReadToUsb(UINT32 startAddress)
{
	UINT16 i;
#if 0//ndef MTD_NEW
	UINT8 sts;
	UINT8 error;

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
		//richie@mi0415 begin
		for (i = 0; i < 8; i++) {
			while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}
		//richie@mi0415 end
	} else {
		//richie@vc0422 end
		//richie@mi0415 begin
		if (G_bUSBSpeed == 0) {
			for (i = 0; i < 8; i++) {
				//patch4.5@richie@msdc plug out exception
				while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
				XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
				//patch4.5@richie@msdc plug out exception
				while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
				XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
			}
		} else {
			printf("#%x\n", (XBYTE[K_USB_CLASS_IN_BUF_SIZE] + XBYTE[K_USB_CLASS_IN_BUF_SIZE + 1] << 8));
			while (((XBYTE[K_USB_CLASS_IN_BUF_SIZE] + XBYTE[K_USB_CLASS_IN_BUF_SIZE + 1] << 8) < 512) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait for 0 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			//patch4.5@richie@msdc plug out exception
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0) && (G_UI_USBConnect == K_UI_USB_CONNECT)) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}
		//richie@mi0415 end
		//richie@vc0422
	}
	//richie@mi0415 marked
	//HAL_CheckReadData2UsbDmaDone(K_Card_PageSize);

	HAL_ECCMode(1); 								 //disable ECC

	HAL_ReadECC(6, G_SMC_EccByte);

	return SMC2_DMAReadRedundantArea(startAddress, TRUE);
#else

	/*Configure nand DMA*/
	HAL_NANDDoDMACfg(K_DMA_FLASH, K_DMA_USB, K_Card_PageSize, K_DMA_Padding0s);
	
	/*Wait nand ready, it's in page program mode maybe*/
	HAL_NANDChkRdy();

	/*Phil: Ecc operation MUST NOT be set before nand send command*/
	
	/*Enable ECC*/
	MTD_EccEnable();
	/*Clear ECC*/
	MTD_EccClear();
	
	/*Trigger USB DMA without waiting*/
	MTD_DMATrig();	

	if (G_USB_IO_Bulk2 == 1) {
		//richie@mi0415 begin
		for (i = 0; i < 8; i++) {
			while (XBYTE[K_USB_AP_IN_BUF_SIZE] < 64) ;  //wait for 64 in bulk buffer
			XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;//open bulk in
			while ((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0) ;  //wait pc ack
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_IN_ACK_RESET;//reset 0x25c2
		}
		//richie@mi0415 end
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

	/*Disable ECC*/
	MTD_EccDisable();
#if 0 //Do ECC 
	if (SMC_ChkEcc() != SUCCESS) {
		CPrintf("E");
		G_SMC_EccErrCount++;
		sts = SMC_CorrectPage(2, 0, 0);
		if (!sts) {
			sts = SMC_BadBlockMark(startAddress / G_NANDF_BlockSize);
		}
		return FALSE;
	}
#endif
	startAddress = startAddress;
	return TRUE;
	//return SMC2_DMAReadRedundantArea(startAddress, TRUE);
#endif
}


#endif
#endif //DISK_DRV_NAND
