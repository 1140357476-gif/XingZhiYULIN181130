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

#ifndef _cardopt_H_
#define _cardopt_H_

#include "hal_device.h"

#define K_DELETE_LFN		1

#define K_CARD_TOTAL 3
#define K_CARD_TYPE 0x0066//support SD,DosRam;
//#define K_CARD_TYPE 0x0024//support SD,Nand

//#define NAND            NANDF_OPTION
//#define SMC             SMC_OPTION
//#define SD              SDF_OPTION
//#define MMC             EMMC_OPTION

//#define CFA             CFC_OPTION

//#define K_MEDIA_Smc_Behavior    K_MEDIA_SMC_Behavior

//Media Type
#define K_MEDIA_None                    0x00
#define K_MEDIA_Nandf                   0x01
#define K_MEDIA_Smc                     0x02
#define K_MEDIA_CfcIde                  0x03
#define K_MEDIA_CfcMem                  0x04
#define K_MEDIA_eMMC                     0x05
#define K_MEDIA_Sdf                     0x06
#define K_MEDIA_Spi                     0x07
#define K_MEDIA_Nextf                   0x08
#define K_MEDIA_Sdram                   0x09

//Page Size
#define K_MEDIA_PageSize256             0x00
#define K_MEDIA_PageSize512             0x01
#define K_MEDIA_PageSize1024            0x02


// WWWW start
#ifndef K_SDRAM_SUPPORTED_BIT
#define K_SDRAM_SUPPORTED_BIT           0       // 0x0001
#endif

#ifndef K_DOSRAM_SUPPORTED_BIT
#define K_DOSRAM_SUPPORTED_BIT          1       // 0x0002
#endif

#ifndef K_NANDF_SUPPORTED_BIT
#define K_NANDF_SUPPORTED_BIT           2       // 0x0004
#endif

#ifndef K_SMC_SUPPORTED_BIT
#define K_SMC_SUPPORTED_BIT             3       // 0x0008
#endif

#ifndef K_CFC_SUPPORTED_BIT
#define K_CFC_SUPPORTED_BIT             4       // 0x0010
#endif

#ifndef K_EMMC_SUPPORTED_BIT
#define K_EMMC_SUPPORTED_BIT            5       // 0x0040
#endif

#ifndef K_SDF_SUPPORTED_BIT
#define K_SDF_SUPPORTED_BIT             6       // 0x0020
#endif

#ifndef K_NORF_SUPPORTED_BIT
#define K_NORF_SUPPORTED_BIT            7       // 0x0080
#endif

#ifndef K_NEXTF_SUPPORTED_BIT
#define K_NEXTF_SUPPORTED_BIT           8       // 0x0100
#endif

#ifndef K_SDRAM_SUPPORTED
#define K_SDRAM_SUPPORTED               (1<<K_SDRAM_SUPPORTED_BIT)
#endif

#ifndef K_DOSRAM_SUPPORTED
#define K_DOSRAM_SUPPORTED              (1<<K_DOSRAM_SUPPORTED_BIT)
#endif

#ifndef K_NANDF_SUPPORTED
#define K_NANDF_SUPPORTED               (1<<K_NANDF_SUPPORTED_BIT)
#endif

#ifndef K_SMC_SUPPORTED
#define K_SMC_SUPPORTED                 (1<<K_SMC_SUPPORTED_BIT)
#endif

#ifndef K_CFC_SUPPORTED
#define K_CFC_SUPPORTED                 (1<<K_CFC_SUPPORTED_BIT)
#endif

#ifndef K_EMMC_SUPPORTED
#define K_EMMC_SUPPORTED                (1<<K_EMMC_SUPPORTED_BIT)
#endif

#ifndef K_SDF_SUPPORTED
#define K_SDF_SUPPORTED                 (1<<K_SDF_SUPPORTED_BIT)
#endif

#ifndef K_NORF_SUPPORTED
#define K_NORF_SUPPORTED                (1<<K_NORF_SUPPORTED_BIT)
#endif

#ifndef K_NEXTF_SUPPORTED
#define K_NEXTF_SUPPORTED               (1<<K_NEXTF_SUPPORTED_BIT)
#endif

//---- Option Constant Define  ---------
//------------------------------------------------------------------------------
// wyeo@0509
//#define CODE_IS_MULTIBANK       1 //must 1
//#define SQUEEZE_CODE_SPACE     	0//must 0 

#if (K_CARD_TYPE > 1)
#define CARD_OPTION             1
#else
#define CARD_OPTION             0
#endif

#define SDRAM_OPTION            0

//patch4.2@richie@ds0509 begin
#if (K_CARD_TYPE & K_DOSRAM_SUPPORTED)
#define DOSRAM_OPTION           1	//lib dosRAM always be compiled
#else
#define DOSRAM_OPTION           0
#endif
//patch4.2@richie@ds0509 end

#if (K_CARD_TYPE & K_NANDF_SUPPORTED)
#define NANDF_OPTION            1
#else
#define NANDF_OPTION            0
#endif

#if (K_CARD_TYPE & K_SMC_SUPPORTED)
#define SMC_OPTION              1
#else
#define SMC_OPTION              0
#endif

#if (K_CARD_TYPE & K_NORF_SUPPORTED)
#define NORF_OPTION             1
#else
#define NORF_OPTION             0
#endif

#if (K_CARD_TYPE & K_CFC_SUPPORTED)
#define CFAIDE                  1  // either CFAIDE or CFAMEM is 1
#define CFAMEM                  0
#define CFC_OPTION              1  // always 1 here
#else
#define CFAIDE                  0
#define CFAMEM                  0
#define CFC_OPTION              0
#endif

#if (K_CARD_TYPE & K_EMMC_SUPPORTED)
#define EMMC_OPTION              1
#else
#define MMC_OPTION              0
#endif

#if (K_CARD_TYPE & K_NEXTF_SUPPORTED)
#define NEXTF_OPTION            1
#else
#define NEXTF_OPTION            0
#endif

#if (K_CARD_TYPE & K_SDF_SUPPORTED)
#define SDF_OPTION              1
#else
#define SDF_OPTION              0
#endif
// WWWW end


// this case should not happen K_SUPPORT_NEWNAND = 0, K_SUPPORT_ONLYNEWNAND = 1
// thse 2 option apply only to nand
#if (NANDF_OPTION)
#define K_SUPPORT_NEWNAND	1
#define K_SUPPORT_ONLYNEWNAND	0
#else
#define K_SUPPORT_NEWNAND	0
#define K_SUPPORT_ONLYNEWNAND	0
#endif

#if 0//(K_SUPPORT_ONLYNEWNAND)
#undef P_NAND_SIZE
#define P_NAND_SIZE		256
#endif

#define CARDLIB_RELEASE		1
#define K_SLASH /

#if CARDLIB_RELEASE
#define M_Card_Printf;/K_SLASH
#else
#define M_Card_Printf printf
#endif

#define K_MEDIA_NONE                    K_MEDIA_None
#ifdef DISK_DRV_SPI
#define K_MEDIA_NANDF                   K_MEDIA_Spi
#else
#define K_MEDIA_NANDF                   K_MEDIA_Nandf
#endif
#define K_MEDIA_SMC                     K_MEDIA_Smc
#if (CFAIDE)
#define K_MEDIA_CFC                     K_MEDIA_CfcIde
#else
#define K_MEDIA_CFC                     K_MEDIA_CfcMem
#endif

#define K_MEDIA_SDF                     K_MEDIA_Sdf
#define K_MEDIA_EMMC                     K_MEDIA_eMMC
#define K_MEDIA_NEXTF                   K_MEDIA_Nextf
#define K_MEDIA_DOSRAM			K_MEDIA_Sdram

#ifndef K_MEDIA_TYPE

#if (K_CARD_TYPE & K_NANDF_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_NANDF
#elif (K_CARD_TYPE & K_NORF_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_NORF
#elif (K_CARD_TYPE & K_NEXTF_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_NEXTF
//patch4.2@richie@ds0511 begin
#elif (K_CARD_TYPE & K_DOSRAM_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_DOSRAM
//patch4.2@richie@ds0511 end
#elif (K_CARD_TYPE & K_CFC_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_CFC
#elif (K_CARD_TYPE & K_SMC_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_SMC
#elif (K_CARD_TYPE & K_SDF_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_SDF
#elif (K_CARD_TYPE & K_EMMC_SUPPORTED)
#define K_MEDIA_TYPE			K_MEDIA_EMMC
#endif

#endif

//HIS101b-0025
#ifndef K_MEDIA_ONLYONE
#if (K_CARD_TOTAL <= 1)
#define K_MEDIA_ONLYONE			1
#else
#define K_MEDIA_ONLYONE			0
#endif
#endif

#ifndef K_MEDIA_UI_OPTION
#define K_MEDIA_UI_OPTION		0
#endif

#if (K_CARD_TYPE & K_NANDF_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_NANDF
#elif (K_CARD_TYPE & K_NORF_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_NORF
#elif (K_CARD_TYPE & K_NEXTF_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_NEXTF
//patch4.2@richie@ds0509 begin
#elif (K_CARD_TYPE & K_DOSRAM_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_DOSRAM
//patch4.2@richie@ds0509 end
#elif (K_CARD_TYPE & K_SDF_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_SDF
#elif (K_CARD_TYPE & K_EMMC_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_EMMC
#elif (K_CARD_TYPE & K_SMC_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_SMC
#elif (K_CARD_TYPE & K_CFC_SUPPORTED)
#define K_MEDIA_BUILT_IN			K_MEDIA_CFC
#endif

// remark above if you are sure that the built in media is sdram
//#define K_MEDIA_BUILT_IN			K_MEDIA_NONE

#if (K_CARD_TYPE & K_CFC_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_CFC
#elif (K_CARD_TYPE & K_SMC_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_SMC
#elif (K_CARD_TYPE & K_SDF_SUPPORTED)		// wyeo@0509

//version4.0@ada@0513 for MMC
#define K_MEDIA_CARD				K_MEDIA_SDF
#elif (K_CARD_TYPE & K_EMMC_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_EMMC

#elif (K_CARD_TYPE & K_NANDF_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_NANDF
#elif (K_CARD_TYPE & K_NORF_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_NORF
#elif (K_CARD_TYPE & K_NEXTF_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_NEXTF
//patch4.2@richie@ds0509 begin
#elif (K_CARD_TYPE & K_DOSRAM_SUPPORTED)
#define K_MEDIA_CARD				K_MEDIA_DOSRAM
//patch4.2@richie@ds0509 end
#endif
// WWWW end

#if (K_CARD_TYPE == 0x007C)
#define SMC_CS_DISABLE			XBYTE[0x2031] |= 0x10
#define SMC_CS_ENABLE			XBYTE[0x2031] &= 0xef
#define SMC_CS_OUTPUT_ENABLE	XBYTE[0x2039] |= 0x10
#endif


#define NAND            NANDF_OPTION
#define SMC             SMC_OPTION
#define SD              SDF_OPTION
#define MMC             EMMC_OPTION

#define CFA             CFC_OPTION

#define K_MEDIA_NewForOldBlk		0x0001
#define K_MEDIA_HasBlockBoundary	0x0002
#define K_MEDIA_EraseBefOverWrite	0x0004
#define K_MEDIA_AutoAdress		0x0008

#define K_MEDIA_SMC_Behavior		( K_MEDIA_NewForOldBlk | K_MEDIA_HasBlockBoundary )
#define K_MEDIA_CF_Behavior		( K_MEDIA_AutoAdress )
#define K_MEDIA_DRM_Behavior		( K_MEDIA_AutoAdress ) // HIS101a-0002
#define K_MEDIA_SD_Behavior        	K_MEDIA_AutoAdress
#define K_MEDIA_MMC_Behavior       	( K_MEDIA_AutoAdress )

//When record avi WITH audio, should turn on this option.
//This option search all free clusters before recording. This
//prevent audio and video not sync problem when the clusters
//are not very contiguous.But this option require many pre-processing
//time when searching whole BIG capacity flash with SMALL cluster.
//It is suggested that you define the time to record for your product,
//then caluclate approximate size require to search. For example, if
//you define your product could record 200 second, also assume that
//frame rate for your product is 10, and maximal size of each frame
//is 12K, the require size is 200*10*12K = 24,000K + 500K = 24,500K.
//If G_DOS_ClusterSize is 2K, then 24,500K/2K=12,250 cluster. Then
//set this value to G_DOS_LinearFATMax before calling file recording
//function. G_DOS_LinearFATMax is initial in diskGlobalInit at cardlink.c
//to 0xFFFF(maximal clusters) whcih search whole flash.								// use asic find pattern to do FAT search
//#define K_Use_Pat_FAT			0			// 1528 could be 1

#define K_File_HaveVideo	        1

// K_File_HaveVideo must on, when this option is on
//#define K_File_HaveMpegAvi	        1			// HIS120-0001 // used by 503, 504

//#define K_File_HaveWaveCardFull	        1			// HIS120-0005 // used by 503, 504
//#define K_File_HaveAviInterlace	        0			// HIS120-0006 // used by 503, 504

//#if (K_File_HaveMpegAvi == 0)					// used by 503, 504
// sorry, old avi format doesn't support interlace
//#undef K_File_HaveAviInterlace
//#define K_File_HaveAviInterlace         0
//#endif

// when this is on, it means capture the video image to SDRAM
//#define K_File_PutVideoAtDram			1

// DEBUG mode
// this flag has effect only when K_File_PutVideoAtDram is 1
// when this is on, it means capture only the first frame of video image
//#define K_File_RptSameVideoAtDram		0

// HIS101a-0003
//#define K_Audio_Codec_OnlyNonCompressed		0
//#define K_Audio_Codec_OnlyIMACompressed		1

//move from cardcon.h
#define K_Card_PageSize            ((UINT16) 0x0200)
#define K_Card_PageSizeShift		((UINT8) 9)

#endif /*__cardopt_H__*/

