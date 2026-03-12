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

#ifndef _cardfunc_
#define _cardfunc_

/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        cardfunc.h

Abstract:

        Header file related to file layer function declaration

Environment:

        Keil C51 Compiler

Revision History:

        09/04/2001      William Yeo    created

--*/
//#include "chipopt.h"
#include "cardopt.h"

//------------------------------------------------------------------------------
//Macro prototype
//------------------------------------------------------------------------------

//#define M_SDRAM_SetEndAddress(addr)

#if 0//( K_MEDIA_ONLYONE )
	#if ( K_MEDIA_TYPE == K_MEDIA_SMC || K_MEDIA_TYPE == K_MEDIA_NANDF )
	#define M_Card_Initialize				SMC_Initialize
	#define M_Card_GetID				        SMC_GetID
	#define M_Card_Detect				        SMC_CardDetect
	#define M_Card_GetBehavior				SMC_GetBehavior
	#define M_Card_LCard2Sram				SMC_LCard2Sram
	#define M_Card_LCard2Dram(a,b) 				SMC_LCard2Dram(a,b,512)
	#define M_Card_LCard2Usb				SMC_LCard2Usb
	#define	M_Card_LDram2Card(a,b)                        	SMC_LDram2Card(a,b,512)
	#define M_Card_LPCard2Sram(a, b)			SMC_Card2Sram(b)
	#define	M_Card_LPSram2Card(a,b,c)			SMC_Sram2Card(b,c)
	#define M_Card_LPCard2Dram(a,b,c)			SMC_Card2Dram(b,c)
	#define M_Card_LPDram2Card(a,b,c,d)			SMC_Dram2Card(b,c,d)
	#define M_Card_LPDram2CardByPort(a,b,c,d)		SMC_Dram2CardByPort(b,c,d)
	#define	M_Card_LPSram2CardByPort(a,b,c)		        SMC_Sram2CardByPort(b,c)

	// HIS101a-0030

	#define	M_Card_LPVideoFifo2Card(a,b,c)			SMC_Fifo2Card(K_FIFO_DRAM2NANDF,b,c)

	#define M_Card_EraseTotalBlock                		SMC_EraseTotalBlock
	#define M_Card_EraseOneBlock(a,b)                       SMC_EraseOneBlock(b)
	#define M_Card_BlockDram2Card				BlockDram2Card
	#define M_Card_BlockCard2Dram				BlockCard2Dram
	#define	M_Card_File_Write				vfsFileWrite
	#define	M_Card_File_Flush			        File_Flush

	#define	M_Card_File_Read				vfsFileRead

	#elif ( K_MEDIA_TYPE == K_MEDIA_CFC )
	#define M_Card_Initialize				CF_Initialize
	#define M_Card_GetID				        CF_GetID
	#define M_Card_Detect				        CF_CardDetect
	#define M_Card_GetBehavior				CF_GetBehavior
	#define M_Card_LCard2Sram				CF_LCard2Sram
	#define M_Card_LCard2Dram(a,b)				CF_LCard2Dram(a,b)
	#define M_Card_LCard2Usb				CF_LCard2Usb
	#define	M_Card_LDram2Card(a,b)                          CF_LDram2Card(a,b)
	#define M_Card_LPCard2Sram(a,b)				CF_LCard2Sram(a)
	#define	M_Card_LPSram2Card(a,b,c)			CF_LSram2Card(a)
	#define M_Card_LPCard2Dram(a,b,c)			CF_LCard2Dram(a,c)
	#define M_Card_LPDram2Card(a,b,c,d)			CF_LDram2Card(a,d)
	#define M_Card_LPDram2CardByPort(a,b,c,d)		CF_LDram2CardByPort(a,d)
	#define	M_Card_LPSram2CardByPort(a,b,c)		        CF_LSram2CardByPort(a)
	#define	M_Card_LPVideoFifo2Card(a,b,c)			CF_LVideoFifo2Card(a)
	#define M_Card_EraseTotalBlock                    	CF_EraseTotalBlock
	#define M_Card_EraseOneBlock(a,b)                       CF_EraseOneBlock(a)
	#define M_Card_BlockDram2Card				BlockDram2Card
	#define M_Card_BlockCard2Dram				BlockCard2Dram
	                                                        // HIS101b-0013
	#define	M_Card_File_Write				File_LWrite
	#define	M_Card_File_Flush			        File_Flush
								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

//patch4.2@richie@ds0511 begin
	#elif ( K_MEDIA_TYPE == K_MEDIA_DOSRAM )
	#define M_Card_Initialize				DRAM_Initialize
	#define M_Card_GetID				        DRAM_GetID
	#define M_Card_Detect				        DRAM_CardDetect
	#define M_Card_GetBehavior				DRAM_GetBehavior
	#define M_Card_LCard2Sram				DRAM_LCard2Sram
	#define M_Card_LCard2Dram(a,b)				DRAM_LCard2DramMore(a,b,1)
	#define M_Card_LCard2Usb				DRAM_LCard2Usb
	#define	M_Card_LDram2Card(a,b)                          DRAM_LDram2CardMore(a,b,1)
	#define M_Card_LPCard2Sram(a,b)				DRAM_LCard2Sram(a)
	#define	M_Card_LPSram2Card(a,b,c)			DRAM_LSram2Card(a)
	#define M_Card_LPCard2Dram(a,b,c)			DRAM_LCard2DramMore(a,c,1)
	#define M_Card_LPDram2Card(a,b,c,d)			DRAM_LDram2CardMore(a,d,1)
	#define M_Card_LPDram2CardByPort(a,b,c,d)		DRAM_LDram2CardByPort(a,d)
	#define	M_Card_LPSram2CardByPort(a,b,c)		        DRAM_LSram2CardByPort(a)
	#define	M_Card_LPVideoFifo2Card(a,b,c)			DRAM_LVideoFifo2Card(a)
	#define M_Card_EraseTotalBlock                    	DRAM_EraseTotalBlock
	#define M_Card_EraseOneBlock(a,b)                       DRAM_EraseOneBlock(a)
	#define M_Card_BlockDram2Card				BlockDram2Card
	#define M_Card_BlockCard2Dram				BlockCard2Dram
	                                                        // HIS101b-0013
	#define	M_Card_File_Write				File_LWrite
	#define	M_Card_File_Flush			        File_Flush
								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif
//patch4.2@richie@ds0511 end

     	#elif (K_MEDIA_TYPE == K_MEDIA_SDF)
	#define M_Card_Initialize				SD_Initialize
	#define M_Card_GetID				     	SD_GetID
	#define M_Card_Detect				     	SD_CardDetect
	#define M_Card_GetBehavior				SD_GetBehavior
	#define M_Card_LCard2Sram				SD_LCard2Sram
	#define M_Card_LCard2Dram(a,b)				SD_LCard2Dram(a,b)
	#define M_Card_LCard2Usb				SD_LCard2Usb
	#define M_Card_LDram2Card(a,b)               		SD_LDram2Card(a,b)
	#define M_Card_LPCard2Sram(a,b)				SD_LCard2Sram(a)
	#define M_Card_LPSram2Card(a,b,c)		     	SD_LSram2Card(a)
	#define M_Card_LPCard2Dram(a,b,c)			SD_LCard2Dram(a,c)
	#define M_Card_LPDram2Card(a,b,c,d)			SD_LDram2Card(a,d)
	#define M_Card_LPDram2CardByPort(a,b,c,d)		SD_LDram2CardByPort(a,d)
	#define M_Card_LPSram2CardByPort(a,b,c)	     		SD_LSram2CardByPort(a)
	#define M_Card_LPVideoFifo2Card(a,b,c)	     		SD_LVideoFifo2Card(a)
	#define M_Card_EraseOneBlock(a,b)            		SD_EraseOneBlock(a)
	#define M_Card_BlockDram2Card				BlockDram2Card
	#define M_Card_BlockCard2Dram				BlockCard2Dram
	#define M_Card_File_Write				File_LWrite
	#define M_Card_File_Flush			     	File_Flush
								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	//wyeo@0509, 1.1.1 start
     	#elif (K_MEDIA_TYPE == K_MEDIA_EMMC)
	#define M_Card_Initialize				EMMC_Initialize
	#define M_Card_GetID				     	MMC_GetID
	#define M_Card_Detect				     	EMMC_CardDetect
	#define M_Card_GetBehavior				MMC_GetBehavior
	#define M_Card_LCard2Sram				MMC_LCard2Sram
	#define M_Card_LCard2Dram(a,b)				MMC_LCard2Dram(a,b)
	#define M_Card_LCard2Usb				MMC_LCard2Usb
	#define M_Card_LDram2Card(a,b)               		MMC_LDram2Card(a,b)
	#define M_Card_LPCard2Sram(a,b)				MMC_LCard2Sram(a)
	#define M_Card_LPSram2Card(a,b,c)		     	MMC_LSram2Card(a)
	#define M_Card_LPCard2Dram(a,b,c)			MMC_LCard2Dram(a,c)
	#define M_Card_LPDram2Card(a,b,c,d)			MMC_LDram2Card(a,d)
	#define M_Card_LPDram2CardByPort(a,b,c,d)		MMC_LDram2CardByPort(a,d)
	#define M_Card_LPSram2CardByPort(a,b,c)	     		MMC_LSram2CardByPort(a)
	#define M_Card_LPVideoFifo2Card(a,b,c)	     		MMC_LVideoFifo2Card(a)
	#define M_Card_EraseTotalBlock               		MMC_EraseTotalBlock
	#define M_Card_EraseOneBlock(a,b)            		MMC_EraseOneBlock(a)
	#define M_Card_BlockDram2Card				BlockDram2Card
	#define M_Card_BlockCard2Dram				BlockCard2Dram
	#define M_Card_File_Write				File_LWrite
	#define M_Card_File_Flush			     	File_Flush

	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#endif
	//wyeo@0509, 1.1.1 end

#else
	//#define M_Card_Initialize				Card_Initialize
	//#define M_Card_GetID				        Card_GetID
	//#define M_Card_Detect				        Card_Detect
	//#define M_Card_GetBehavior				Card_GetBehavior
	//#define M_Card_LCard2Sram				Card_LCard2Sram
	//#define M_Card_LCard2Dram(a,b)				Card_LCard2Dram(a,b)
	//#define M_Card_LCard2Usb				Card_LCard2Usb
	//#define	M_Card_LDram2Card(a,b)                          Card_LDram2Card(a,b)
	//#define M_Card_LPCard2Sram(a,b)				Card_LPCard2Sram(a,b)
	//#define	M_Card_LPSram2Card(a,b,c)			Card_LPSram2Card(a,b,c)
	//#define M_Card_LPCard2Dram(a,b,c)			Card_LPCard2Dram(a,b,c)
	//#define M_Card_LPDram2Card(a,b,c,d)			Card_LPDram2Card(a,b,c,d)
	//#define M_Card_LPDram2CardByPort(a,b,c,d)		Card_LPDram2CardByPort(a,b,c,d)
	//#define	M_Card_LPSram2CardByPort(a,b,c)		        Card_LPSram2CardByPort(a,b,c)
	//#define	M_Card_LPVideoFifo2Card(a,b,c)			Card_LPVideoFifo2Card(a,b,c)
	//#define M_Card_EraseTotalBlock                		Card_EraseTotalBlock
	//#define M_Card_EraseOneBlock(a,b)                       Card_EraseOneBlock(a,b)
	//#define M_Card_BlockDram2Card				BlockDram2Card
	//#define M_Card_BlockCard2Dram				BlockCard2Dram

	// HIS101b-0013
	#if 1//(CODE_IS_MULTIBANK)
	//#define	M_Card_File_Write				Card_File_Write
	#elif (SMC_OPTION == 0 && NANDF_OPTION == 0)
	#define	M_Card_File_Write				File_LWrite
	#else
	#define	M_Card_File_Write				vfsFileWrite
	#endif

	// HIS101c-0004
	#if 1//(K_HAVE_HAL && CODE_IS_MULTIBANK)
	//#define	M_Card_File_Read				Card_File_Read
	#elif (K_HAVE_HAL && SMC_OPTION == 0 && NANDF_OPTION == 0)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	//#define	M_Card_File_Flush			        File_Flush
#endif

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
//-------------------------------------
//cardptch.c
//-------------------------------------
void Card_SetExptCode(UINT16 exptCode);
void Card_ClearExptCode(void);
UINT16 Card_GetExptCode(void);
void Card_ClearAllError(void);
//UINT16 Card_GetInformation(UINT8 *buf);
void Card_InitLibGlobal(void); // HIS101a-0018
//UINT8 Card_Specific(UINT16 action, UINT32 refCon); // HIS101a-0037
//void diskInactivate(void); // patch4.4-9@wyeo@select nand, sdram as default

#if 0//( K_MEDIA_ONLYONE )

#else
UINT8 Card_Initialize(UINT8 doDOSInitial);
//UINT8 Card_GetID(void);
UINT8 Card_Detect(void);
UINT16 Card_GetBehavior(void);
UINT8 Card_LCard2Sram(UINT32 logAddr);
UINT8 Card_LPCard2Sram(UINT32 logAddr, UINT32 phyAddr);
//UINT8 diskPhySectorRead(UINT32 logAddr, UINT32 phyAddr, UINT32 ramAddr);
//UINT8 diskSectorRead(UINT32 logAddr, UINT32 ramAddr);
//UINT8 diskPhyBlockWrite(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr);
//UINT8 Card_LPDram2CardByPort(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr);
UINT8 Card_LCard2Usb(UINT32 logAddr);
//UINT8 diskSectorWrite(UINT32 logAddr, UINT32 ramAddr);
UINT8 Card_LPSram2Card(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum); // HIS101b-0017
//UINT8 Card_LPSram2CardByPort(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum); // HIS101b-0017
//UINT8 Card_LPVideoFifo2Card(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum); // HIS101b-0017
UINT8 Card_EraseTotalBlock(void);
//UINT8 Card_EraseOneBlock(UINT32 logAddr, UINT32 phyAddr);
//UINT8 Card_BlockDram2Card( UINT32 srcRamAddr, UINT32 dstStartLogAddr, UINT32 writeSize );
//UINT8 Card_BlockCard2Dram( UINT32 dstRamAddr, UINT32 dstStartLogAddr, UINT32 readSize );
//UINT8 Card_File_Write( UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage );
//UINT8 Card_File_Flush();

// HIS101c-0004
//UINT8 Card_File_Read(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);

#endif
#endif
