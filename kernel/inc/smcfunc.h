/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __SMCFUNC_H__
#define __SMCFUNC_H__

//------------------------------------------------------------------------------
//Define
//------------------------------------------------------------------------------
#define flashCacheCtrl    sp1kFlashCacheCtrl

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
UINT8 sp1kDiskTypeGet(void);
UINT8 sp1kDiskRsvRead(UINT32 idx, UINT32 addr);
UINT8 sp1kDiskRsvWrite(UINT32 idx, UINT32 addr);
UINT8 sp1kDiskRsvSizeGet(UINT32 idx, UINT32 *offset, UINT32 *size);

void sp1kFlashCacheCtrl(UINT8 on); 
UINT8 flashCacheCtrlSts(void); 
void flashLoadDramPara(UINT32 ramAddr);

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------

//-------------------------------------
//smc.c
//-------------------------------------
UINT8 SMC_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC_GetID(void);
UINT8 SMC_Card2Sram(UINT32 phyAddr);
UINT8 SMC_Sram2Card(UINT32 phyAddr, UINT16 blockNum);
UINT8 SMC_Card2Dram(UINT32 phyAddr, UINT32 ramAddr);
UINT8 SMC_Card2DramFast(UINT32 phyAddr, UINT32 ramAddr);
//UINT8 SMC_Card2DramByPort(UINT32 startAddress, UINT32 ramAddr); // HIS101b-0009
UINT8 SMC_Dram2Card(UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr);
UINT8 SMC_Card2Usb(UINT32 phyAddr);
UINT8 SMC_Card2UsbFast(UINT32 phyAddr);
UINT8 SMC_EraseOneBlock(UINT32 phyAddr);
//UINT8 SMC_Sram2CardByPort(UINT32 phyAddr, UINT16 blockNum);
//UINT8 SMC_Card2SramByPort(UINT32 phyAddr);
//UINT8 SMC_Dram2CardByPort(UINT32 phyAddr, UINT16 blockNum,UINT32 ramAddr);
//UINT8 SMC_Dram2CardByPort2(UINT32 phyAddr, UINT16 blockNum);
//UINT8 SMC_VideoFifo2Card(UINT32 phyAddr, UINT16 blockNum);
UINT8 SMC_Card2SramForExt(UINT32 phyAddr);
UINT8 SMC_RedunCard2Sram(UINT32 phyAddr);

//UINT8 SMC_Card2Fifo(UINT8 mode, UINT32 startAddress);		//HIS101a-0023
//UINT8 SMC_Fifo2Card(UINT8 mode, UINT32 phyAddr, UINT16 blockNum);	//HIS101a-0023
//void SMC_Dram2Fifo(UINT32 ramAddr);
//void SMC_Fifo2Dram(UINT32 ramAddr);
UINT8 SMC_CheckEcc(void);
void SMC_FillRedunArea(UINT16 blockAddress, UINT8 * badBlk);

//UINT8 SMC_Dram2CardSram(UINT32 startAddress, UINT16 blockAddress);
//richie@mi0415
UINT8 SMC_Usb2Card(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC_AutoCmdSet(UINT8 const xdata* pCmd, UINT8 CmdCnt, UINT32 Address);

// @012403@wyeo@support 256M nand, begin
UINT8 NANDF_LWriteStart(UINT8 srcPath, UINT32 logAddr, UINT32 numSector);
UINT8 NANDF_LWriteContinue(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr, UINT32 *pNumSector);
UINT8 NANDF_LWriteEnd(UINT8 srcPath, UINT32 copySize, UINT8 *pEndContinuous);
// @012403@wyeo@support 256M nand, end

//void SMC_SequentialDataInputCommand(UINT32 phyAddr);
//void SMC_Read1Command_1stHalf(UINT32 phyAddr);
//void SMC_Read1Command_2ndHalf(UINT32 phyAddr);
//void SMC_Read2Command(UINT32 phyAddr);
//void SMC_ReadComplete(void);
//void SMC_PageProgramCommand(void);
//void SMC_ClearEcc(void);
//void SMC_ReadEcc2(void);
//void SMC_500ReadEcc(UINT8 unit);
//void SETMODE_CpuWriteFlash(void);
//void SETMODE_CpuReadFlash(void);
//void SETMODE_UsbReadFlash(void);

//void SMC_Dram2Sram(void);
//void SMC_Sram2Dram(void);

UINT8 SMC_Card2Sram(UINT32 startAddress);
UINT8 SMC_Sram2Card(UINT32 startAddress, UINT16 blockAddress);

//UINT8 SMC_Card2Sram2(UINT32 startAddress);
//UINT8 SMC_Sram2Card2(UINT32 startAddress, UINT16 blockAddress);
//void SMC_Fifo2Sram(void);
//void SMC_Sram2Fifo(void);
UINT8 SMC_BadBlockMark(UINT16 blockAddress);

//-------------------------------------
//smccore.c
//-------------------------------------
UINT8 SMC_CardDetect(void);
UINT8 SMC_Initialize(UINT8 doDOSInitial);
UINT8 SMC_LCard2Sram(UINT32 logAddr);
UINT8 SMC_LCard2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 size);
UINT8 SMC_LCard2Usb(UINT32 logAddr, UINT32 size);
UINT8 SMC_LCard2UsbExt(void);
//richie@mi0415
UINT8 SMC_LUsb2Card(UINT32 logAddr, UINT32 size);
UINT8 SMC_LDram2Card(UINT32 logAddr, UINT32 ramAddr, UINT32 size);

//UINT8 SMC_PhysicalTest(void);
//UINT8 SMC_LogicalTest(void);
UINT8 SMC_ReadLpaToPpa(UINT32 logAddr, UINT32* phyAddr);
UINT8 SMC_WriteLbaToPba(UINT32 logAddr, UINT32 *oldPhyAddr, UINT32 *newPhyAddr, UINT16 *blockNum);
UINT16 SMC_CalcLbaField(UINT16 logBlockAddr);
UINT16 SMC_SearchNewBlock(void);
UINT8 SMC_FormatCheck(void);
UINT8 SMC_BlockStsCheck(UINT32 phyAddress,UINT8 stsData, UINT8 stsData2); //HIS101b-0019
UINT8 SMC_DataStsCheck(UINT8 stsData);
UINT8 SMC_CisCheck(void);
void SMC_MakeTable(UINT8 bank);
UINT8 SMC_ParityCheck(UINT8 d1, UINT8 d2);
UINT16 SMC_CalcLba(UINT8 d1, UINT8 d2);
UINT8 SMC_EraseTotalBlock(void); // HIS101b-0017
//void SMC_FillExtByte(UINT16 blockNum);
UINT8 SMC_SearchValidBlock(UINT32 *pPhyAddr);
UINT8 SMC_CisIdi(UINT32 phyAddr);
UINT16 SMC_GetBehavior(void);
void SMC_SetError(UINT16 errCode);
extern void SMC_ClearError(void);
extern UINT16 SMC_GetError(void);
void SMC_UpdateBankTable( UINT8 bank, UINT8 toSram ); // HIS101a-0037
void SMC_BuildBankTable( UINT8 bankCount ); // HIS101a-0037
UINT8 SMC_CorrectPage(UINT8 page, UINT32 ramAddr, UINT8 doCorrect);	// HIS101b-0009
UINT32 SMC_Ecc1BitErrCount(void);				// HIS101b-0009
UINT8 SMC_CheckValidBlock(UINT32 *pPhyAddr);

//-------------------------------------
//smcdata.h
//-------------------------------------

//-------------------------------------
//smcdef.h
//-------------------------------------

//-------------------------------------
//smcecc.c
//-------------------------------------
//void SMC_InitializeEcc(void);
//void SMC_CalculateEcc(void);
//void SMC_TransformEcc(UINT8 unit);
//void SMC_TestEcc(void);

//-------------------------------------
//smcecctb.c
//-------------------------------------

//-------------------------------------
//smchw.c
//-------------------------------------
void SMC_HwInitialize(void);
//void SMC_InitializeOperation(void);
//void SMC_CommandLatchCycle(UINT8 command);
void SMC_IdAddressLatchCycle(void);
void SMC_ReadWriteAddressLatchCycle(UINT32 address);
//void SMC_EraseAddressLatchCycle(UINT32 address);
UINT8 SMC_CheckReady(void);
UINT8 SMC_ReadIdData(void);
//UINT8 SMC_ReadData2Sram(void); 		// HIS101b-0009
//UINT8 SMC_ReadData2Sdram(UINT32 ramAddr);	// HIS101b-0009
//UINT8 SMC_ReadData2SramForExt(void); 	// HIS101b-0009
//UINT8 SMC_ReadData2Fifo(void);		// HIS101b-0009
//void SMC_ReadRedunData(void);
//void SMC_WriteData(UINT16 blockAddress);
//void SMC_WriteDataDma(UINT16 blockAddress);
//void SMC_WriteSdramData(UINT16 blockAddress);
//UINT8 SMC_ReadStatusData(void);
//void SMC_CompleteOperation(void);
//void SMC_ReadEcc(void);
//void SMC_FillSramData(void);
//void B_MD_To_Sdram();
//void B_MD_To_Storage();

//-------------------------------------
//smc.c
//-------------------------------------
//UINT8 nandFwRead(UINT32 hdr_addr,UINT32 fw_addr);
//UINT8 nandRawRead(UINT32 hdr_addr,UINT32 addr);
//UINT8 nandAllRead(UINT32 addr);
UINT8 nandDetect(void);
UINT8 nandRsvBlkParse(void);
//UINT8 nandBrnFileCheck(UINT32 addr);
//UINT32 nandBrnHdrSize(UINT32 addr, UINT32 *fwSiZe, UINT32 *rawA, UINT32 *rawB, UINT32 *hdrSize);
//void nandConvertRawFromBrn(UINT32 src,UINT32 dst);
UINT8 nandRsvRead(UINT32 ,UINT32 );
UINT8 nandRsvSizeGet(UINT32 idx,UINT32 *offset,	UINT32 *);
//void dataFillDump( UINT32 total , UINT8 *addr, UINT8 WR);
//UINT8 MemRawCmp(UINT32 addr1, UINT32 addr2, UINT32 num);
//UINT8 nandPhyBlkRd(UINT32 A, UINT32 blk, UINT32 addr);
UINT8 nandRsvWrite(UINT32 ,UINT32 );
//UINT32 nandReadAllRaw(UINT32 addr);
UINT8 NANDF_LDram2CardCache(UINT32 logAddr, UINT32 ramAddr, UINT32 size);


//UINT8 SMC_ReadData2UsbDma(void);
void SMC_ReadRedunData2Sram(void);
//void SMC_ReadData2DramDma(void);
//void SMC_WriteDataFromSram(UINT16 blockAddress);
//void SMC_WriteDataFromDramDma(UINT16 blockAddress);


// wyeo@0514
UINT8 NANDF_AccessReserveBlock(UINT8 path, UINT8 blkNum, UINT8 pageNum, UINT8 pageCount, UINT32 ramAddr);


void SMC_NandAutoFlowCfg(UINT32 eccAddrWord, UINT32 redundAddrWord);
UINT8 SMC_NandAutoReadStart(UINT32 startAddr,UINT32 dramAddr,UINT16 pageNum,UINT8 wait);
UINT8 SMC_NandAutoWriteStart(UINT32 startAddr,UINT32 dramAddr,UINT16 pageNum,UINT8 wait);

#endif /*__SMCFUNC_H__*/
