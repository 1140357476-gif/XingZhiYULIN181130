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

#ifndef __OPTION_H__
#define __OPTION_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//DPOF_OPTION =                 0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define DPOF_OPTION             1

//------------------------------------------------------------------------------
//Audio Option =                0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define AUDIO_OPTION            0       //always set to 0 for backward compatibility

//------------------------------------------------------------------------------
//OSD Option =                  0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define OSD_OPTION              0       //always set to 0 for backward compatibility

//------------------------------------------------------------------------------
//Power Option =                0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define POWER_OPTION            0

//------------------------------------------------------------------------------
//Debug Option =                0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define DEBUG_OPTION            0       //always set to 0 for backward compatibility

//------------------------------------------------------------------------------
//DbgPrint Option =             0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define DBGPRINT_OPTION         1

//------------------------------------------------------------------------------
//PlayBack Option =             0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define PLAYBACK_OPTION         1

//------------------------------------------------------------------------------
//USB Audio Device Class Option =
//                              0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
//#define USBAUDC_OPTION          1

//------------------------------------------------------------------------------
//USB Mass Storage Device Class Option =
//                              0 Disable
//                              1 Control/Bulk/Interrupt (CBI) Transport Protocol
//                              2 Bulk-Only Transport Protocol
//------------------------------------------------------------------------------
#define USBMSDC_OPTION          2

//patch4.5@richie@mlun begin

//------------------------------------------------------------------------------
//USB Still Image Device Class Option =
//                              0 Disable
//                              1 PIMA 15740 Bulk-Only Transport Protocol
//------------------------------------------------------------------------------
//#define USBSIDC_OPTION          1

//patch5.1@Add copy to card function begin
//------------------------------------------------------------------------------
//COPY2CARD_OPTION Option =
//                              0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define COPY2CARD_OPTION		1
//patch5.1@Add copy to card function end

//------------------------------------------------------------------------------
//AE/AWB/AF Option =
//                              0 Disable
//                              1 Enable
//------------------------------------------------------------------------------

//patch5.1@richie@interpolation begin
//------------------------------------------------------------------------------
//INTERPOLAT_OPTION Option =
//                              0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define INTERPOLAT_OPTION		0
//patch5.1@richie@interpolation end

// @patch5.2.1@mattwang@ baudrate select begin
//------------------------------------------------------------------------------
//BAUDRATE_OPTION=
//  0: 9600bps, use Timer 1 as BaudRate Generator
//  1: 57600bps, use Timer 2 as BaudRate Generator
//------------------------------------------------------------------------------
#define BAUDRATE_OPTION         0
// @patch5.2.1@mattwang@ baudrate select end

//patch4.2@andrew@0523

#ifdef  ICM107B
#define AAA_OPTION		1
#endif

#ifdef  ICM110
#define AAA_OPTION		1
#endif

// patch 5.2.1_26@mattwang@add MI360 beg
#ifdef  MI360
#define AAA_OPTION		1   //It will open AE AWB
#endif
// patch 5.2.1_26@mattwang@add MI360 end

// patch 5.2.1_24@mattwang@MI1300 beg
#ifdef  MI1300
#define AAA_OPTION		1   //It will open AE AWB
#endif
// patch 5.2.1_24@mattwang@MI1300 end

// patch 5.2.1_36@mattwang@add MI1310 CMOS sensor beg
#ifdef  MI1310
#define AAA_OPTION		1   //It will open AE AWB
#endif
// patch 5.2.1_36@mattwang@add MI1310 CMOS sensor end

// patch 5.2.1_23@mattwang@MI2000 beg
#ifdef  MI2000
#define AAA_OPTION		1   //It will open AE AWB
#endif
// patch 5.2.1_23@mattwang@MI2000 end

// patch 5.2.1_27@mattwang@add MI3100 beg
#ifdef  MI3100
#define AAA_OPTION		1   //hua@031229 add
#endif
// patch 5.2.1_27@mattwang@add MI3100 end

#ifdef  OV13
#define AAA_OPTION		1
#endif

#ifdef  OV2610  //patch5.0@jhyu@OV2610AE
#define AAA_OPTION		1
#endif

#ifdef  OV3610  //patch5.2.1@jhyu@OV3610
#define AAA_OPTION		1
#endif

// patch 5.2.1_34@mattwang@add OV3620 CMOS sensor beg
#ifdef  OV3620
#define AAA_OPTION		1
#endif
// patch 5.2.1_34@mattwang@add OV3620 CMOS sensor end

// patch 5.2.1_37@mattwang@add OV5610 CMOS sensor beg
#ifdef  OV5610
#define AAA_OPTION		1
#endif
// patch 5.2.1_37@mattwang@add OV5610 CMOS sensor end

// patch 5.2.1_38@mattwang@add SOI268 CMOS sensor beg
#ifdef  SOI268
#define AAA_OPTION		1
#endif
// patch 5.2.1_38@mattwang@add SOI268 CMOS sensor end

//patch4.3@andrew@0618
//Now table only for MN39472+TLV990
#ifdef  MN39472
#ifdef TLV990
#define AAA_OPTION		1
#endif
#endif

#ifdef  MCM20027			//patch4.3@jhyu@0612
#define AAA_OPTION		1
#endif

//patch4.5@andrew@IR3Y48A_A
#ifdef  SHARP_LZ24BP
#ifdef  IR3Y48A
#define AAA_OPTION		1
#endif
#endif

//patch4.5@andrew@sony442_AE/AWB_1
#ifdef SONY442
#ifdef AD9843
#define AAA_OPTION		1
#endif
#endif

#ifndef  AAA_OPTION
#define AAA_OPTION		0
#endif

// wyeo@0509
//------------------------------------------------------------------------------
//AVI Free Cluster Option =
//                              0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
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
//to 0xFFFF(maximal clusters) whcih search whole flash.
#define K_Link_AllFreeCluster   1

//version4.0@ada@0513 for Multi-Directory
//------------------------------------------------------------------------------
//Single Directory Option =     0 Disable
//                              1 Enable
//------------------------------------------------------------------------------
#define SINGLE_DIRECTORY        0

//------------------------------------------------------------------------------
//Flash Card Option
//------------------------------------------------------------------------------


// wyeo@0509
/* obsolete, use K_Use_Pat_FAT at cardopt.h, wyeo@0509
//------------------------------------------------------------------------------
// Search DOS FAT free clusters using Pattern Search function
//
//------------------------------------------------------------------------------
//yichang
//#define SEARCHPATTERN
#define NONSEACHPATTERN
*/

//------------------------------------------------------------------------------
// Options for AVI
//
//------------------------------------------------------------------------------

#define AVIAUDIO_OPTION         1 //In videoclip, the option to record audio
#define PLAYAUDIO_OPTION        1 //In playback video, the option to play audio

// wyeo@0509
//To record a large file, turn on the option to search
//all free clusters before recording
//#define K_Link_AllFreeCluster             0

//patch4.4@yichang@0705 UI for ISP
#define UI_FOR_ISP              1

//patch4.5@richie@checksum isp
#define ISPCHECKSUM_OPTION	0

//for FPGA Test Function
#define FPGA_TEST 0

#endif /*__OPTION_H__*/