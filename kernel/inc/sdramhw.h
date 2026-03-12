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

#ifndef __SDRAMHW_H__
#define __SDRAMHW_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define K_SDRAM_NoPrefetch              0
#define K_SDRAM_Prefetch                1

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void SDRAM_HwInitialize(void);

//patch4.5@ada@Add timeout count begin
UINT8 SDRAM_Dram2Sram(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz); //ada@0124 // WWWW
//void SDRAM_Dram2Sram(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz); //ada@0124 // WWWW

UINT8 SDRAM_Sram2Dram(UINT8 *sramAddr, UINT32 ramAddr, UINT16 siz);
//void SDRAM_Sram2Dram(UINT8 *sramAddr, UINT32 ramAddr, UINT16 siz);
//patch4.5@ada@Add timeout count end

//UINT8 SDRAM_Dram2Usb(UINT32 address, UINT16 size);
//void SDRAM_CardSetStartAddress(UINT32 address, UINT8 prefetch);


#endif /*__SDRAMHW_H__*/