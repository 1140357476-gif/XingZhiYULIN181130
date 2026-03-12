/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __DRAMFUNC_H__
#define __DRAMFUNC_H__


extern xdata	UINT32	G_ulDRAMSTORLogicalZeroAddr;
//patch4.4@richie@dsi begin
//add variable for dram total size boundary
extern	xdata	UINT32	G_ulDRAMTotalSize;
extern	xdata	UINT32	G_ulDRAMSTORFAT1AddrTemp;
//patch4.4@richie@dsi end


UINT8 DRAM_CardDetect(void);
UINT8 DRAM_ResetMBR(UINT8 doDOSInitial);
UINT16 DRAM_GetBehavior(void);
UINT8 DRAM_SectorFillByteData(UINT32 sectorAddr,UINT8 bData);
UINT8 DRAM_LCard2UsbMore(UINT32 logAddr, UINT32 numSector);
UINT8 DRAM_LUsb2CardMore(UINT32 address, UINT32 size);
UINT8 DRAM_LCard2Sram(UINT32 logAddr);
UINT8 DRAM_LSram2Card(UINT32 logAddr);
UINT8 DRAM_LCard2DramMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector);
UINT8 DRAM_LDram2CardMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector);
UINT8 DRAM_LCard2Usb(UINT32 address);
UINT8 DRAM_EraseTotalBlock(void);
UINT8 DRAM_Initialize(UINT8 doDOSInitial);
UINT32 DRAM_StartAddrGet(void);
void DRAM_DosRamBufGet(UINT32 *startAddr, UINT32 *size);


#endif /*__DRAMFUNC_H__*/