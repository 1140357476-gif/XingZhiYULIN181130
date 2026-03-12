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

#ifndef __SMCVAR_H__
#define __SMCVAR_H__

//#include "chipopt.h"

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------

//-------------------------------------
//smcascic.c
//-------------------------------------

//-------------------------------------
//smccore.c
//-------------------------------------
extern  xdata   UINT8    G_Card_TotalBank;
extern  xdata   UINT8    G_Card_CurrBank;
extern  xdata   UINT16    G_Card_BlockExtSize;
extern  xdata   UINT16    G_Card_BlockWholeSize;
extern  xdata   UINT16    G_Card_PagePerBlock;	// @012403@wyeo@support 256M nand
extern  xdata   UINT32   G_Card_PhysicalBankSize;

extern 	xdata   UINT16    G_SMC_AllocatTable[];
extern 	xdata   UINT8    G_SMC_EntryTable[];

extern 	xdata   UINT16    G_SMC_AllocatTableSize;
extern 	xdata   UINT8    G_SMC_EntryTableSize;

extern 	xdata   UINT32   G_SMC_CisBlockAddress;

extern 	xdata   UINT8    G_SMC_EccByte[];
extern 	xdata   UINT8    G_SMC_EccErr;
extern 	xdata   UINT16	G_SMC_ErrCode;
extern 	xdata   UINT16    G_SMC_FirstCISFound;
extern 	xdata	UINT8	G_SMC_BuildBankMap[];
extern 	xdata	UINT32	G_SMC_EccErrCount;	// HIS101b-0009
extern 	xdata	UINT32	G_SMC_Ecc1BitErrCount;	// HIS101b-0009
extern 	xdata	UINT32	G_SMC_Ecc2BitErrCount;	// HIS101b-0009
extern  xdata   UINT8   G_SMC_AddrMode;	
// wyeo@0514
extern  UINT16    G_NAND_ReservBlockTotal; 
//extern  UINT16    G_NAND_ReservBlockMax; 
extern  xdata   UINT16    G_SMC_CisBlockFound;
extern  xdata   UINT32   G_SMC_NewCisBlockAddress;

// @012403@wyeo@support 256M nand, begin
extern	xdata UINT8 G_NANDF_Maker_Byte;
extern	xdata UINT8 G_NANDF_3RD_Byte;
extern	xdata UINT8 G_NANDF_4TH_Byte;
extern	xdata UINT8 G_NANDF_5TH_Byte;
extern	xdata UINT8 G_NANDF_Category; // 0 is old, 1 is new and SMC2_
extern	xdata UINT16 G_NANDF_PageSize;
extern	xdata UINT16 G_NANDF_PageExtSize;
extern	xdata UINT32 G_NANDF_BlockSize;
extern	xdata UINT8 G_NANDF_BlockSizeShift;

extern	xdata UINT8 G_NANDF_HasOldBlk;
extern	xdata UINT32 G_NANDF_StartLogAddr;
extern	xdata UINT32 G_NANDF_EndBlkAddr;
extern	xdata UINT32 G_NANDF_NewPhyAddr;
extern	xdata UINT32 G_NANDF_OldPhyStartAddr;
extern	xdata UINT16 G_NANDF_BlockNum;
extern	xdata UINT32 G_NANDF_OldPhyAddr;
extern	xdata UINT8 G_NANDF_FindHeaderPage;
extern	xdata UINT8 G_NANDF_SetHdrPageAddr;
extern	xdata UINT8 G_NANDF_FirstEnter;
extern	xdata UINT8 G_NANDF_RsvIndex;
extern	xdata UINT8 G_NANDF_StartPage;
extern	xdata UINT32 G_SMC_GoodSectorPerCard;
extern	xdata UINT8 G_BadBlk_Mark;

// @012403@wyeo@support 256M nand, end
//-------------------------------------
//smcdata.h
//-------------------------------------
extern 	code UINT8 G_SMC_CIS[];
extern 	code UINT8 G_SMC_CisPartial[];

//-------------------------------------
//smcdos.c
//-------------------------------------

//-------------------------------------
//smcecc.c
//-------------------------------------
extern	data   UINT8    G_SMC_EccReg0;
extern	data   UINT8    G_SMC_EccReg1;
extern	data   UINT8    G_SMC_EccReg2;
extern	data   UINT8    G_SMC_EccCount;
extern	data   UINT8    G_SMC_EccData;

//-------------------------------------
//smcecctb.c
//-------------------------------------
extern	code	UINT8	G_SMC_EccTable[];

#endif /*__SMCVAR_H__*/