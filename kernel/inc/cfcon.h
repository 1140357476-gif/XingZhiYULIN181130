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

#ifndef _cfcon_H_
#define _cfcon_H_

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//-------------------------------------
//cfascic.c
//-------------------------------------
//----- CF-card ATA command set --------
#define CF_IDdriver                     0xEC    //Identify drive
#define CF_Idle                         0xE3    //standby
#define CF_Standby                      0xE2    //standby
#define CF_ReadSect                     0x20    //read sector
#define CF_WriteSect_Erase              0x30    //write sector with erase
#define CF_WriteSect                    0x38    //write sector
#define CF_EraseSect                    0xC0    //Erase sector
#define CF_ReadBuffer                   0xE4    //read buffer
#define CF_SetFeature                   0xEF    //set feature
//----- CF-card parameter --------------
//#define CF_CylindNumb                ((WORD) 0x0200)
//#define CF_HeadNumb                  ((BYTE) 9)
//#define CF_Byte_Sect                 ((BYTE) 0x10)
//#define CF_Sect_Track
//#define CF_Sect_Card                 ((WORD) 0x0100)
//#define CF_BufferSize                512
#define CF_ECCbyte_Sect                 4
#define CF_MaxSect_MultiRW              1024    //Phy <--> LBA mapping table#
//----- CF-card function ---------------
#define CF_Mode                         0       //00=Mem,01=IO,10=IDE
#define IDEWordRW                       1       //1=only word R/W in IDE mode
#define CF_Struct                       0       //0/1=LBA/CHS
#define CF_BusFlow                      0       //0/1=DMA/PIO
#define CF_TotalSect                    (CF_CylindNumb * CF_HeadNumb *1)
#define CF_TotalSect_LBA                1


#define CFdataL         0x2430
#define CFdataH         0x2431
#define CFaddrL         0x2432
#define CFaddrH         0x2433
#define CFmode          0x2434
#define CFck            0x2435
#define CFslect         0x2436
#define CFreg           0x2439
#define CFreset         0x243A
#define CFready         0x243B          //CPU R/W CF ready ?
#define CFbsyint        0x243C          //CF Ready/IRQ/INT pin ?

//-------------------------------------
//cfcore.c
//-------------------------------------

#endif /*__cfcon_H__*/