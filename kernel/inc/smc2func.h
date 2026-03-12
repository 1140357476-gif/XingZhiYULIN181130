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
#ifndef __SMC2FUNC_H__
#define __SMC2FUNC_H__


UINT8 SMC2_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_RedunCard2Sram(UINT32 startAddress);
UINT8 SMC2_EraseOneBlock(UINT32 startAddress);
UINT8 SMC2_Sram2Card(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_Card2Sram(UINT32 startAddress);
UINT8 SMC2_Dram2Card(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr);
UINT8 SMC2_Card2Dram(UINT32 startAddress, UINT32 ramAddr);
UINT8 SMC2_Card2DramFast(UINT32 startAddress, UINT32 ramAddr);
UINT8 SMC2_Usb2Card(UINT32 startAddress, UINT16 dstBlockNum);
UINT8 SMC2_Card2Usb(UINT32 startAddress);
UINT8 SMC2_Card2UsbFast(UINT32 startAddress); 
UINT8 SMC2_DMAWriteRedundantArea(UINT32 startAddress, UINT16 blockAddress);
//UINT8 SMC2_PIOWriteRedundantArea(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_DMAReadRedundantArea(UINT32 startAddress, UINT8 checkEcc);
//UINT8 SMC2_PIOReadRedundantArea(UINT32 startAddress, UINT8 checkEcc);

UINT8 SMC2_DMAWriteFromSram(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_DMAWriteFromDram(UINT32 startAddress, UINT16 blockAddress, UINT32 ramAddr);
//UINT8 SMC2_PIOWriteFromSram(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_DMAWriteFromUsb(UINT32 startAddress, UINT16 blockAddress);
UINT8 SMC2_WriteStart(UINT32 startAddress);
//UINT8 SMC2_WriteEnd();
UINT8 SMC2_ReadStart(UINT32 startAddress);
//UINT8 SMC2_ReadEnd();
UINT8 SMC2_DMAReadToSram(UINT32 startAddress);
//UINT8 SMC2_PIOReadToSram(UINT32 startAddress);
UINT8 SMC2_DMAReadToDram(UINT32 startAddress, UINT32 ramAddr);
//UINT8 SMC2_ReadData2Fifo(UINT32 startAddress, UINT8 mode);
//UINT8 SMC2_PIOReadToDram(UINT32 startAddress, UINT32 ramAddr);
UINT8 SMC2_ReadRandomColumn(UINT16 columnAddress);
UINT8 SMC2_WriteRandom(UINT32 startAddress, UINT16 columnAddress);
UINT8 SMC2_WriteRandomColumn(UINT16 columnAddress);

void SMC2_ReadWriteAddressLatchCycle(UINT32 startAddress, UINT16 columnAddress);
void SMC2_ReadWriteColumnLatchCycle(UINT32 startAddress);
//void SMC2_EraseAddressLatchCycle(UINT32 startAddress);

UINT8 SMC2_CopyBackStart(UINT32 srcStartAddress);
UINT8 SMC2_CopyAll512Pages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 copySize);
UINT8 SMC2_CopyPages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 numOfPage);
UINT8 SMC2_Copy512Pages(UINT32 srcStartAddress, UINT32 dstStartAddress, UINT16 dstBlockNum, UINT32 numOf512);

//UINT8 SMC2_Card2Fifo(UINT8 mode, UINT32 startAddress);
//UINT8 SMC2_Fifo2Card(UINT8 mode, UINT32 startAddress, UINT16 dstBlockNum);
//UINT8 SMC2_DMAWriteFromFifo(UINT8 mode, UINT32 startAddress, UINT16 dstBlockNum);

//UINT8 SMC2_Dram2CardPIO(UINT32 startAddress, UINT16 dstBlockNum, UINT32 ramAddr);
//UINT8 SMC2_Dram2CardPIOAdvance(UINT32 startAddress, UINT16 dstBlockNum, UINT32 ramAddr);
//UINT8 SMC2_PIOWriteFromDram(UINT32 startAddress, UINT16 dstBlockNum, UINT32 ramAddr);
UINT8 SMC2_DMAReadToUsb(UINT32 startAddress);

#endif /*__SMC2FUNC_H__*/
