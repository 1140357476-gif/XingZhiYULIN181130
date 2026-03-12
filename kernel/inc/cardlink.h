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

#ifndef _cardlink_H_
#define _cardlink_H_

//#include "chipopt.h"
#include "cardopt.h"
#include "doslink.h"
//#include "dosusr.h"
#include "cardui.h"

//------------------------------------------------------------------------------
//Macro prototype
//------------------------------------------------------------------------------

#if 0//( K_MEDIA_ONLYONE )

	#if ( K_MEDIA_TYPE == K_MEDIA_SMC || K_MEDIA_TYPE == K_MEDIA_NANDF )
	#define M_Card_Initialize				SMC_Initialize
	#define M_Card_GetID		        		SMC_GetID
	#define M_Card_Detect					SMC_CardDetect
	// HIS101b-0013
	#define	M_Card_File_Write				vfsFileWrite
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#if ( K_MEDIA_TYPE == K_MEDIA_CFC )
	#define M_Card_Initialize				CF_Initialize
	#define M_Card_GetID		        		CF_GetID
	#define M_Card_Detect					CF_CardDetect
	// HIS101b-0013
	#define	M_Card_File_Write				File_LWrite
								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#endif

	//patch4.2@richie@ds0511 begin
	#if ( K_MEDIA_TYPE == K_MEDIA_DOSRAM )
	#define M_Card_Initialize				DRAM_Initialize
	#define M_Card_GetID		        		DRAM_GetID
	#define M_Card_Detect					DRAM_CardDetect
	// HIS101b-0013
	#define	M_Card_File_Write				File_LWrite
								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#endif
	//patch4.2@richie@ds0511 end

	#if ( K_MEDIA_TYPE == K_MEDIA_SDF )
	#define M_Card_Initialize				SD_Initialize
	#define M_Card_GetID		     			SD_GetID
	#define M_Card_Detect					SD_CardDetect
	#define	M_Card_File_Write				File_LWrite

								// HIS101c-0004
	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#endif

	//wyeo@0509, 1.1.1 start
	#if ( K_MEDIA_TYPE == K_MEDIA_EMMC )
	#define M_Card_Initialize				EMMC_Initialize
	#define M_Card_GetID		     			MMC_GetID
	#define M_Card_Detect					EMMC_CardDetect
	#define	M_Card_File_Write				File_LWrite

	#if 1//(K_HAVE_HAL)
	#define	M_Card_File_Read				File_LRead
	#else
	#define	M_Card_File_Read				vfsFileRead
	#endif

	#endif
	//wyeo@0509, 1.1.1 end

#else
	//#define M_Card_Initialize				Card_Initialize
	//#define M_Card_GetID		        		Card_GetID
	//#define M_Card_Detect					Card_Detect

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

#endif

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
//-------------------------------------
//fiplay.c	--file layer
//-------------------------------------
//extern	UINT8 File_PlayAvi(UINT8 *name, UINT16 startCluster, UINT32 vidRamAddr, UINT32 vidRamSize, UINT32 audRamAddr, UINT32 audRamSize);
//extern UINT8 File_PlayExifFile(UINT8 *name, UINT16 startCluster, UINT32 DramAddr, UINT16 *ImgWidth, UINT16 *ImgHeight, UINT8 *date, BIT ShowThumb);


// @WYEO, 1023
//extern	UINT8 File_PlayWave(UINT8 *name, UINT16 startCluster, UINT32 ramAddr, UINT32 ramSize);
//extern	UINT32 File_ReadWaveToDram(UINT8 *name, UINT16 startCluster, UINT32 ramAddr, UINT32 ramSize, UINT32 *pAudsValue);

//-------------------------------------
//fiaud.c
//-------------------------------------
//UINT8 File_RecordWave(UINT8 *name, UINT32 ramAddr, UINT32 ramSize, UINT16 wFormatTag);

//-------------------------------------
//fiavi.c	--file layer
//-------------------------------------
//extern	UINT8 File_RecordAvi(UINT8 *name, UINT8 bAudio);

//-------------------------------------
//jpglib.c	--lib layer
//-------------------------------------
//void File_FillLong(UINT16 offset, UINT32 ul);

//-------------------------------------
//cardlink.c	--card layer
//-------------------------------------
//extern void diskGlobalInit(void);
//extern void diskMemInit(void);

//-------------------------------------
//cardptch.c	--card layer
//-------------------------------------
//extern	UINT8 Card_Initialize(UINT8 doDOSInitial);
//extern	UINT8 Card_GetID(void);
//extern	UINT8 Card_Detect(void);
//extern	void Card_ClearAllError(void);
//extern	UINT16 Card_GetInformation(UINT8 *buf);
//extern	void Card_InitLibGlobal(void); // HIS101a-0018
//extern	UINT8 Card_Specific(UINT16 action, UINT32 refCon); // HIS101a-0037
//extern void diskInactivate(void);	// patch4.4-9@wyeo@select nand, sdram as default

// HIS101c-0004
//UINT8 Card_File_Write( UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage );
//UINT8 Card_File_Read(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);

//-------------------------------------
//smccore.c	--smc layer
//-------------------------------------
//extern	UINT8 SMC_Initialize(UINT8 doDOSInitial);
//extern	UINT8 SMC_CardDetect(void);
//extern	UINT32 SMC_Ecc1BitErrCount(void);	// HIS101b-0009

// wyeo@0514
//extern	UINT8 NANDF_AccessReserveBlock(UINT8 path, UINT8 blkNum, UINT8 pageNum, UINT8 pageCount, UINT32 ramAddr);

//-------------------------------------
//smc.c         --smc layer
//-------------------------------------
//extern	UINT8 SMC_GetID(void);

//-------------------------------------
//cfcore.c	--cf layer
//-------------------------------------
//extern	UINT8 CF_Initialize(UINT8 doDOSInitial);
//extern	UINT8 CF_CardDetect(void);

//-------------------------------------
//cfasic.c	--cf layer
//-------------------------------------
//extern	UINT8 CF_GetID(void);

//-------------------------------------
//sdcore.c	--sd layer
//-------------------------------------
//extern	UINT8 SD_Initialize(UINT8 doDOSInitial);
//extern	UINT8 SD_CardDetect(UINT8);
//extern	UINT8 SD_GetID(void);

//wyeo@0509, 1.1.1
//-------------------------------------
//mmccore.c	-mmc layer
//-------------------------------------
//extern	UINT8 EMMC_Initialize(UINT8 doDOSInitial);
//extern	UINT8 EMMC_CardDetect(void);

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//-------------------------------------
//carddef.h	--card layer
//-------------------------------------
//#define K_Card_DataBuffer_Size       		(512+16)        //size of G_ucDataBuffer

//-------------------------------------
//fiaud.c
//-------------------------------------
//#define K_FormatTag_NonCompress			0x01
//#define K_FormatTag_IMA				0x11

//-------------------------------------
//smccore.c
//-------------------------------------
//#define K_SMC_ECC_STRUC_LEN			6	// HIS101b-0009

// wyeo@0514, path for read write nand reserve block
//#define K_NANDF_Reserve_FlashToSram		0
//#define K_NANDF_Reserve_SramToFlash		1
//#define K_NANDF_Reserve_FlashToDram		2
//#define K_NANDF_Reserve_DramToFlash		3

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
#if 0
//-------------------------------------
//fiavi.c	--file layer
//-------------------------------------
extern xdata	UINT32 	G_File_SDRAM_AviIdx1BufSize;

//-------------------------------------
//avilib.c	--lib layer
//-------------------------------------
extern xdata	UINT32 	G_File_SDRAM_AviIdx1BufAddr;
#endif
// HIS120-5
//extern	xdata	UINT32 	G_File_SDRAM_AudioEndBufAddr;
//extern	xdata	UINT32 	G_File_SDRAM_AudioRecAddr;
//extern	xdata	UINT32 	G_File_SDRAM_AudioGetAddr;
//extern	xdata	UINT32 	G_File_AudioRecSize;
//extern	xdata	UINT8	G_File_AudioRing;
//extern	xdata	UINT16	G_File_AudioFirstSize;	// in word unit
//extern	xdata	UINT32 	G_File_SDRAM_AudioJunkAddr;

//-------------------------------------
//aavilib.c	--lib layer
//-------------------------------------
//extern  idata   UINT32   G_File_AudioSize;
//extern	xdata	UINT32 	G_File_SDRAM_AudioBufAddr;

//-------------------------------------
//cardptch.c	--card layer
//------------------------------------
extern xdata UINT8 G_Card_Type;
//extern xdata UINT16 G_Card_Supported;
//extern xdata UINT16 G_Card_Module;
//extern xdata UINT16 G_Card_ExptCode;
extern xdata UINT16 G_Card_UI_Action;	        // HIS101a-0001
//extern xdata UINT32 G_Card_DOS_SDRAM_FAT1Addr;   // HIS101a-0002


// HIS101a-0002
//-------------------------------------
//drm.c	--card layer
//------------------------------------
//extern  xdata	UINT32	G_DRM_SDRAM_StartAddr;
//extern  xdata	UINT32	G_DRM_SDRAM_CardSize;

// wyeo@0514
//-------------------------------------
//smccore.c	--smc layer
//-------------------------------------
extern  xdata   UINT16    G_NAND_ReservBlockTotal;
extern  xdata   UINT16    G_NAND_ReservBlockMax;

extern xdata   UINT8    G_Image_QTableIndex;
//extern xdata   UINT32    G_SDRAM_ImageAddr;

#endif /*__cardlink_H__*/
