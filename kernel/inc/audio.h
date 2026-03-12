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

#ifndef __AUDIO__H__
#define __AUDIO__H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//AC'97 CODEC register number
#define K_AC_Index_Reset                0x00
#define K_AC_Index_MasterVolume         0x02
#define K_AC_Index_MasterVolumeMono     0x06
#define K_AC_Index_PcBeepVolume         0x0A
#define K_AC_Index_PhoneVolume          0x0C
#define K_AC_Index_MicVolume            0x0E
#define K_AC_Index_LineInVolume         0x10
#define K_AC_Index_CdVolume             0x12
#define K_AC_Index_VideoVolume          0x14
#define K_AC_Index_AuxVolume            0x16
#define K_AC_Index_PcmOutVol            0x18
#define K_AC_Index_RecordSelect         0x1A
#define K_AC_Index_RecordGain           0x1C
#define K_AC_Index_GeneralPurpose       0x20
#define K_AC_Index_3dControl            0x22
#define K_AC_Index_PowerDownCtrlSts     0x26
#define K_AC_Index_SerialConfig         0x74
#define K_AC_Index_MiscCtrlBits         0x76
#define K_AC_Index_SampleRate0          0x78
#define K_AC_Index_SampleRate1          0x7A
#define K_AC_Index_VendorId1            0x7C
#define K_AC_Index_VendorId2            0x7E
//AC'97 CODEC register content        //current //default
#define K_AC_Reset                      0x0400    //0x0400
#define K_AC_MasterVolume               0x8000    //0x8000
#define K_AC_MasterVolumeMono           0x8000    //0x8000
#define K_AC_PcBeepVolume               0x8000    //0x8000
#define K_AC_PhoneVolume                0x8008    //0x8008
#define K_AC_MicVolume                  0x0040    //0x8008
#define K_AC_LineInVolume               0x8808    //0x8808
#define K_AC_CdVolume                   0x8808    //0x8808
#define K_AC_VideoVolume                0x8808    //0x8808
#define K_AC_AuxVolume                  0x8808    //0x8808
#define K_AC_PcmOutVol                  0x8808    //0x8808
#define K_AC_RecordSelect               0x0606    //0x0000
#define K_AC_RecordGain                 0x0f0f    //0x8000
#define K_AC_GeneralPurpose             0x0000    //0x0000
#define K_AC_3dControl                  0x0000    //0x0000
#define K_AC_PowerDownCtrlSts           0x0000    //0x0000
#define K_AC_SerialConfig               0x7000    //0x7000
#define K_AC_MiscCtrlBits               0x0000    //0x0000
#define K_AC_SampleRate0                0x1F40    //0xBB80
#define K_AC_SampleRate1                0xBB80    //0xBB80
#define K_AC_VendorId1                  0x4144    //0x4144
#define K_AC_VendorId2                  0x5303    //0x5303
//Sub-sampling specific
#define K_AUDIO_MaxSampleRate           0x1F40
#define K_AUDIO_SubSampleFactor         (K_AUDIO_MaxSampleRate / K_AC_SampleRate0)
//AC'97 CODEC register read/write retry count
#define K_AC_MAXRETRYCOUNT              1000000

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
extern  xdata   UINT32   G_AUDIO_RateScale;   //@WYEO, 1008
extern  xdata   UINT32   G_AUDIO_Rate;
extern  xdata   UINT8   G_AUDIO_Bits;
extern  xdata   UINT8   G_AUDIO_nChannel;
extern  xdata   UINT8   G_AUDIO_nSampleSize;

extern  code    UINT16  AudAS_SampleRate[];
extern  idata   UINT8   G_File_AudioByteLo;
extern  idata   UINT8   G_File_AudioByteHi;
extern  xdata   UINT32   G_File_SDRAM_AudioBufSize;
extern  data    UINT8   G_File_SubSampleFactor;
extern  idata   UINT8   G_File_AudioSampleCount;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void AUDIO_Initialize(void);
UINT16 AUDIO_AcRegisterRead(UINT8 index);
void AUDIO_AcRegisterWrite(UINT8 index, UINT16 value);
void File_AudioSetMode(void);   // @WYEO, 1008
void File_AudioGetSample(void); // @WYEO, 1008
UINT8 File_PlayWaveStream(UINT32 ramAddr, UINT32 audioSize, UINT32 *pAudsValue); // @WYEO, 103101
UINT32 File_RecordAudioStream(UINT32 ramAddr, UINT32 ramSize, UINT32 *pAudsValue); // @WYEO, 103101
//UINT8 sp1kAudParamSet(UINT8 attrId, UINT8 value);
//UINT8 sp1kAudParamGet(UINT8 attrId);
#endif /*__AUDIO_H__*/
