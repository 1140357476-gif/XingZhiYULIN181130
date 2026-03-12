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

#ifndef __FICON_H__
#define __FICON_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//-------------------------------------
//fiavi.c
//-------------------------------------

//-------------------------------------
//fijpg.c
//-------------------------------------

//-------------------------------------
//fiplay.c
//-------------------------------------
#define K_File_Avi_LIST_Chunk			(UINT32)0x4C495354
#define K_File_Avi_hdrl_Chunk			(UINT32)0x6864726C
#define K_File_Avi_avih_Chunk			(UINT32)0x61766968
#define K_File_Avi_strl_Chunk			(UINT32)0x7374726C
#define K_File_Avi_strh_Chunk			(UINT32)0x73747268
#define K_File_Avi_strf_Chunk			(UINT32)0x73747266
#define K_File_Avi_strd_Chunk			(UINT32)0x73747264
#define K_File_Avi_vids_Chunk			(UINT32)0x76696473
#define K_File_Avi_auds_Chunk			(UINT32)0x61756473
#define K_File_Avi_MJPG_Chunk			(UINT32)0x4D4A5047
#define K_File_Avi_00dc_Chunk			(UINT32)0x30306463
#define K_File_Avi_01wb_Chunk			(UINT32)0x30317762
#define K_File_Avi_00db_Chunk			(UINT32)0x30306462
#define K_File_Avi_movi_Chunk			(UINT32)0x6D6F7669
#define K_File_Avi_JUNK_Chunk			(UINT32)0x4A554E4B
#define K_File_Avi_idx1_Chunk			(UINT32)0x69647831


#define K_File_Avi_SP54_Handler			(UINT32)0x53503534
#define K_File_Avi_mjpg_Handler			(UINT32)0x6D6A7067

#define K_File_Avi_TotalFrames_Offset		16
#define K_File_Avi_Width_Offset			32
#define K_File_Avi_Height_Offset		36
#define K_File_Avi_fccHandler_Offset		4
#define K_File_Avi_TimpStamp_Offset		20

#define K_File_Avi_SampleScale_Offset		20
#define K_File_Avi_SampleRate_Offset		24
#define K_File_Avi_SampleSize_Offset		44

// value index in array
#define K_File_Avi_TotalFrames_Value		0
#define K_File_Avi_Width_Value			1
#define K_File_Avi_Height_Value			2
#define K_File_Avi_vidsHandler_Value		3
#define K_File_Avi_TimpStamp_Value		4
#define K_File_Avi_moviOffset_Value		5
#define K_File_Avi_idx1Offset_Value		6
#define K_File_Avi_vidssEnd_Value		7

#define K_File_Avi_audsHandler_Value		0
#define K_File_Avi_SampleScale_Value		1
#define K_File_Avi_SampleRate_Value		2
#define K_File_Avi_SampleSize_Value		3
#define K_File_Avi_wFormatTag_Value		4
#define K_File_Avi_nChannels_Value		5
#define K_File_Avi_nSamplesPerSec_Value		6
#define K_File_Avi_nAvgBytesPerSec_Value	7
#define K_File_Avi_wBitsPerSample_Value		8
#define K_File_Avi_audsEnd_Value		9

#define K_File_Wave_fmt_Chunk			(UINT32)0x666D7420
#define K_File_Wave_data_Chunk			(UINT32)0x64617461

#define K_File_Wave_wFormatTag_Value		0
#define K_File_Wave_nChannels_Value		1
#define K_File_Wave_nSamplesPerSec_Value	2
#define K_File_Wave_nAvgBytesPerSec_Value	3
#define K_File_Wave_blockAlign_Value		4
#define K_File_Wave_wBitsPerSample_Value	5
#define K_File_Wave_audioSize_Value		6
#define K_File_Wave_audioFOffset_Value		7
#define K_File_Wave_End_Value			8

#endif /*__FICON_H__*/