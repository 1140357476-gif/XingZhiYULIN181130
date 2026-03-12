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
#ifndef __SMCCON_H__
#define __SMCCON_H__

#include "cardopt.h"

#define NAND_BAD_BLOCK_MARK       1
#define NAND_RSV_DATA_AT_END	  0
#define NAND_BANK0_SWAP_BLK	  5
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define SMC_DEV_ID_4M_1           0xe3
#define SMC_DEV_ID_4M_2           0xe5

#define SMC_DEV_ID_8M_1           0x39
#define SMC_DEV_ID_8M_2           0xe6
#define SMC_DEV_ID_8M_W1          0x49
#define SMC_DEV_ID_8M_W2          0x59

#define SMC_DEV_ID_16M_1          0x33
#define SMC_DEV_ID_16M_2          0x73
#define SMC_DEV_ID_16M_W1         0x43
#define SMC_DEV_ID_16M_W2         0x53

#define SMC_DEV_ID_32M_1          0x35
#define SMC_DEV_ID_32M_2          0x75
#define SMC_DEV_ID_32M_W1         0x45
#define SMC_DEV_ID_32M_W2         0x55

#define SMC_DEV_ID_64M_1          0x36
#define SMC_DEV_ID_64M_2          0x76
#define SMC_DEV_ID_64M_W1         0x46
#define SMC_DEV_ID_64M_W2         0x56

#define SMC_DEV_ID_128M_1         0x78
#define SMC_DEV_ID_128M_2         0x79
#define SMC_DEV_ID_128M_W1        0x72
#define SMC_DEV_ID_128M_W2        0x74

#define NAND_DEV_ID_64M_PF        0xf0

#define NAND_DEV_ID_128M_1        0xa1
#define NAND_DEV_ID_128M_2        0xf1
#define NAND_DEV_ID_128M_W1       0xb1
#define NAND_DEV_ID_128M_W2       0xc1
#define NAND_DEV_ID_128M_3        0xd1

#define NAND_DEV_ID_256M_1        0xaa
#define NAND_DEV_ID_256M_2        0xda
#define NAND_DEV_ID_256M_W1       0xba
#define NAND_DEV_ID_256M_W2       0xca

#define NAND_DEV_ID_512M_1        0xac
#define NAND_DEV_ID_512M_2        0xdc
#define NAND_DEV_ID_512M_W1       0xbc
#define NAND_DEV_ID_512M_W2       0xcc

#define NAND_DEV_ID_1G_1          0xa3
#define NAND_DEV_ID_1G_2          0xd3
#define NAND_DEV_ID_1G_W1         0xb3
#define NAND_DEV_ID_1G_W2         0xc3

#define NAND_DEV_ID_2G_1          0xa5
#define NAND_DEV_ID_2G_2          0xd5
#define NAND_DEV_ID_2G_W1         0xb5
#define NAND_DEV_ID_2G_W2         0xc5

#define NAND_DEV_ID_4G_1          0xa7
#define NAND_DEV_ID_4G_2          0xd7
#define NAND_DEV_ID_4G_W1         0xb7
#define NAND_DEV_ID_4G_W2         0xc7

#define NAND_DEV_ID_8G_1          0xae
#define NAND_DEV_ID_8G_2          0xde
#define NAND_DEV_ID_8G_W1         0xbe
#define NAND_DEV_ID_8G_W2         0xce

//-------------------------------------
//smcascic.c
//-------------------------------------
#define SMC_USE_PIO_MODE	0		//HIS101a-0023

//-------------------------------------
//smccore.c
//-------------------------------------
#define P_NAND_SIZE 0

#define K_SMC_BUILD_BANK_MAP_SIZE	16	// @patch5.2.1-24@wyeo@fix strange problem@111203
						// @patch5.2.1@wyeo@support xd card@072903
						// 16 byte represent 128 bank map, which
						// allow up to 2G card
#if ( P_NAND_SIZE > 16 || P_NAND_SIZE == 0 )
#define K_SMC_BUILD_BANK_AT_SDRAM	1	// HIS101a-0037
#else
#define K_SMC_BUILD_BANK_AT_SDRAM	0	// HIS101a-0037
#endif

#define K_SMC_MAX_ClusterSize		((UINT32)0x6000)	// HIS101a-0037
							// assume max cluster size is 24K word = 48K byte 
#define K_SMC_DO_ECC_CORECTION		1	// HIS101b-0009
#define K_SMC_DO_USB_ECC_BY_FIRMWARE	0	// HIS101b-0009
#define K_SMC_DO_USB_ECC_BY_HOST	1	// HIS101b-0009
#define K_SMC_ECC_STRUC_LEN		6	// HIS101b-0009

// wyeo@0514, path for read write nand reserve block
#define K_NANDF_Reserve_FlashToSram		0
#define K_NANDF_Reserve_SramToFlash		1
#define K_NANDF_Reserve_FlashToDram		2
#define K_NANDF_Reserve_DramToFlash		3

// HIS101b-0019
#if (NANDF_OPTION)
#define K_NANDF_STRICT_BAD_CHECK	0 // 1
#else
#define K_NANDF_STRICT_BAD_CHECK	0	
#endif

#define K_PreWriteOldBlock		0

// @patch 4.5@wyeo@make table make faster, begin
#define K_STRICT_MAKE_TABLE             1
#define K_RECOVER_MAKE_TABLE            1
#define K_Error_Recovery                1
//end

// @patch 4.5@wyeo@simulate smc as nand
#define K_SMC_AS_NANDF			0	// tetsing mode, musty 0
//end

// @012403@wyeo@support 256M nand, begin
#define K_NANDF_USE_COPYBACK		0	
#define K_NANDF_USE_PAGEALIGNED		1
// @012403@wyeo@support 256M nand, end

//-------------------------------------
//smcdata.h
//-------------------------------------

//support NAND WP, begin...@gudelei
#define K_NAND_WP_ENABLE		1

#if ( K_NAND_WP_ENABLE == 1 )

#define WPBIT		0x02

#else

#define WPBIT		0x00

#endif
//support NAND WP, end

//-------------------------------------
//smcdef.h
//-------------------------------------

//patch4.5@ada@adjust NANDF read write timming begin
#define K_SMC_ActiveCycle       0x04
//#define K_SMC_ActiveCycle       1
#define K_SMC_RecoveryCycle     0x04
//#define K_SMC_RecoveryCycle     1
//patch4.5@ada@adjust NANDF read write timming end

#define K_SMC_CIS_SIZE          0x080
#define K_SMC_CisPartialSize    10

#define K_SMC_SequentialDataInput       0x80
#define K_SMC_RandomDataInput       0x85
#define K_SMC_Read1             	0x00	// K_SMC_Read1_1stHalf
#define K_SMC_Read1_2ndHalf             0x01	// K_SMC_Read1_2ndHalf
#define K_SMC_Read2                     0x50
#define K_SMC_ReadId                    0x90	// K_SMC_ReadID	
#define K_SMC_Reset                     0xFF
#define K_SMC_PageProgram               0x10
#define K_SMC_BlockErase1               0x60
#define K_SMC_BlockErase2               0xD0
#define K_SMC_EraseSuspend              0xB0
#define K_SMC_EraseResume               0xD0
#define K_SMC_ReadStatus                0x70
#define K_SMC_ReadRegister              0xE0

#define K_SMC_PageExtSize               ((UINT8) 0x10)
#define K_SMC_PageWholeSize             ((UINT16) K_Card_PageSize + (UINT16) K_SMC_PageExtSize)
#define K_SMC_PacketPerPage             ((UINT8) (K_Card_PageSize / 0x40)) //0x40 byte/bulk packet

#define K_SMC_EccUnitSize               ((UINT16) 0x0100)
#define K_SMC_EccUnitPerPage            ((UINT8) (K_Card_PageSize / K_SMC_EccUnitSize))

#define K_SMC_EccByteSize               6

//SUPPORT 512Mbit NAND FLASH FOR POWER FLASH
extern xdata UINT16 SMC_LogicalBlockPerBank;

#define K_SMC_LogicalBlockPerBank       SMC_LogicalBlockPerBank            		//1000 logical-block/bank

#define K_SMC_MaxAllocatTableSize       1024                            //size of table storing physical to logical address mapping
#define K_SMC_MaxEntryTableSize         (K_SMC_MaxAllocatTableSize / 8) //size of table storing whether block is empty or not

#define K_SIZEOF_G_SMC_AllocatTable	((UINT32)K_SMC_MaxAllocatTableSize*2L)
#define K_SIZEOF_G_SMC_EntryTable	((UINT32)K_SMC_MaxEntryTableSize)

//-------------------------------------
//smcdos.c
//-------------------------------------

//-------------------------------------
//smcecc.c
//-------------------------------------

//-------------------------------------
//smcecctb.c
//-------------------------------------

#endif /*__SMCCON_H__*/
