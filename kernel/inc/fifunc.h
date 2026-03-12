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

#ifndef __FIFUNC_H__
#define __FIFUNC_H__
//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
//-------------------------------------
//fiavi.c
//-------------------------------------
UINT8 File_RecordAvi(UINT8 *name, UINT8 bAudio);

//-------------------------------------
//fiplay.c
//-------------------------------------
//UINT8 File_PlayExifFile(UINT8 *name, UINT16 startCluster, UINT32 DramAddr, UINT8 *date, BIT ShowThumb);   //ada@0129
//patch4.2@yichang@0520 for recording image width/height
UINT8 File_PlayExifFile(UINT8 *name, UINT16 startCluster, UINT32 DramAddr, UINT16 *ImgWidth, UINT16 *ImgHeight, UINT8 *date, BIT ShowThumb);
UINT8 File_PlayAvi(UINT8 *name, UINT16 startCluster, UINT32 vidRamAddr, UINT32 vidRamSize, UINT32 audRamAddr, UINT32 audRamSize);
UINT8 File_IsAviFile(UINT8 *buf,UINT32 *pSize);
UINT8 File_ParseWave(UINT8 *name, UINT16 startCluster, UINT32 *pAudsValue, UINT16 arraySize);
UINT8 File_PlayWave(UINT8 *name, UINT16 startCluster, UINT32 ramAddr, UINT32 ramSize);
UINT32 File_ReadWaveToDram(UINT8 *name, UINT16 startCluster, UINT32 ramAddr, UINT32 ramSize, UINT32 *pAudsValue);
UINT8 File_IsWaveFile(UINT8 *buf, UINT32 *pSize);

//-------------------------------------
//fiaud.c
//-------------------------------------
UINT8 File_RecordWave(UINT8 *name, UINT32 ramAddr, UINT32 ramSize, UINT16 wFormatTag); // WWW1

#endif /*__FIFUNC_H__*/