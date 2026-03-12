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
#ifndef __L1_H__
#define __L1_H__


//-----------------------------------------------------------------------------
// Data type
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define L1K_SUCCESS                     0x00
#define L1K_ERROR_GENERAL               0x01
#define L1K_ERROR_PARAMETER             0x02

#define L1K_VEXTEND                     0x06

#define PRINT_L1        printf

#define AUDIO

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
//extern  xdata   UINT8   HALG_XXX;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

// System
void  L1_InitPowerOn(void);
void  L1_InitPreview(void);
void  L1_InitPlayback(void);
void  L1_InitPCCam(void);
void  L1_InitVideoClip(void);
void  L1_InitDSC(void);
void  L1_ISP(UINT32,UINT32,UINT32,UINT16);
void  L1_ISPUpdateRomCode(void);
void  L1_UICtrl(void);

//richie@ae0226
UINT8  L1_3A(UINT8 Mode3A);
UINT8  L1_AE(UINT8);
void  L1_AWB(void);
//void  L1_AF(void);

UINT8 L1_UpLoadGOSD(UINT32, UINT32, UINT16, UINT16);
UINT8 L1_UpLoadFOSD(UINT32, UINT32);
UINT8 L1_UpLoadBadPxlCoor(UINT16 RomAddr);
void  L1_TestOSD(UINT16 L1_ID);

// Preview
//pathc@4.3@richie@zo
//patch5.0@richie@sdram mapping begin
//patch4.5@richie@smooth zoom begin
UINT16 L1_DisplayZoomIn(UINT8 factor);
//patch4.5@richie@smooth zoom end
//patch5.0@richie@sdram mapping end
UINT8 L1_PVZoom(UINT8);
UINT8 L1_SnapOne(UINT32*,UINT32*);        //ada@0111
UINT8 L1_ContiShot(UINT8);
UINT8 L1_VideoClip(void);
UINT8 L1_DoAllBadPixel(UINT32,UINT16,UINT16);
UINT8 L1_PutDateStamp(UINT32, UINT16, UINT16, UINT16, UINT16, UINT8, UINT8, UINT8, UINT8, UINT8);
//patch4.2@yichang@0513 for continue snapping
void postSnapping(UINT32* pSize, UINT32* ptmbsize);

/* // WWWW
UINT8 L1_AE(UINT8);
UINT8 L1_AWB(UINT8);
UINT8 L1_AF(UINT8);
*/
UINT8 L1_PBVideo(void);

// Playback
UINT8 L1_PBOne(UINT8);
UINT8 L1_PBFour(UINT8);
UINT8 L1_PBNine(void);
UINT8 L1_PBZoom(UINT8);		// WWWW
UINT8 L1_PBPan(UINT16, UINT16);
UINT8 L1_PBInfo(UINT8);

// Audio
UINT8 L1_RecordAudio(UINT32, UINT32, UINT32, UINT8);
UINT8 L1_PlaybackAudio(UINT32, UINT32, UINT32, UINT8, UINT8);


// PC
UINT8 L1_DownloadISP(UINT32, UINT32, UINT32, UINT16);
UINT8 L1_UploadFAT(UINT8);
UINT8 L1_UploadFile(UINT8);
UINT8 L1_UploadAll(UINT8);
UINT8 L1_DownloadToFlash(UINT32, UINT32);
UINT8 L1_UploadFromFlash(UINT32, UINT32);


// OSD
UINT8 L1_SimUpLoadFOSD(UINT8);
UINT8 L1_SimDownLoadFOSD(UINT8);
UINT8 L1_GOSDEnter(void);
UINT8 L1_GOSDExit(void);
UINT8 L1_GOSDShiftR(void);
UINT8 L1_GOSDShiftL(void);


// FileSystem
UINT8 L1_DeleteImage(void);
UINT8 L1_LockImage(UINT8);
UINT8 L1_ReadImage(UINT8);
UINT8 L1_WriteImage(UINT8);


//Storage
UINT8 L1_NANDtoDRAM(UINT32, UINT32, UINT32);
UINT8 L1_DRAMtoNAND(UINT32, UINT32, UINT32);

#ifdef CFAIDE
UINT8 L1_DMAWrCFIDE(UINT8, UINT8, UINT8, UINT8, UINT8, UINT32, UINT16);
UINT8 L1_DMARdCFIDE(UINT8, UINT8, UINT8, UINT8, UINT8, UINT32, UINT16);
#endif
#ifdef CFAMEM
UINT8 L1_DMAWrCFMEM(UINT8, UINT8, UINT8, UINT8, UINT8, UINT32, UINT16);
UINT8 L1_DMARdCFMEM(UINT8, UINT8, UINT8, UINT8, UINT8, UINT32, UINT16);
#endif

#if 0//def SD
UINT8 L1_SDIdentification(UINT8, UINT8, UINT16);
UINT8 L1_DMAWrSD(UINT8, UINT32 , UINT32, UINT16 );
UINT8 L1_DMARdSD(UINT8, UINT32 , UINT32, UINT16 );
//richie@vc0422
UINT8 L1_DMARdSDBulk2(UINT8 Dst, UINT32 SAdr, UINT32 BlockCnt, UINT16 BlockSize);

UINT8 L1_SDCheckState(void);
UINT8 L1_SDSetBusMode(UINT8, UINT8, UINT8);
UINT8 L1_SDEraseData(UINT32 , UINT32 );
#endif

//patch4.5@ada@Add timeout count begin
UINT8  L1_ReadNandID(UINT8*, UINT8*);
//void  L1_ReadNandID(UINT8*, UINT8*);

UINT8  L1_EraseNandBlock(UINT8, UINT32, UINT8*);
//void  L1_EraseNandBlock(UINT8, UINT32, UINT8*);
//patch4.5@ada@Add timeout count end

UINT8 L1_DMAWrNAND(UINT8, UINT8, UINT32, UINT32, UINT16);
UINT8 L1_DMARdNAND(UINT8, UINT8, UINT32, UINT32, UINT16);


// Dummy code
extern code unsigned char Dummy[];
extern code unsigned char Dummy0[];
extern code unsigned char Dummy1[];

// WWWW0409 start
void SetStdColor2( void );
void SetStdColor( void );
void CamStatus( void );
void GetAvgLum(UINT16* Avg);

// WWWW0409 end

// 3A
void L1_AppSnapOne();
void L1_AppSnapRaw();

//patch4.4@yichang@badpixel begin
void L1_AppSnapRawSub();
UINT8 L1_BadPixelDRAMToNAND(void);
//patch4.4@yichang@badpixel end

// Test
void  L1_Demo(void);
void  L1_PCcamPower(void);
//patch5.0@richie@sdram mapping begin
//void  L1_CapturePower(void);
//void  L1_CDSPPower(void);
//void  L1_CompressPower(void);
//void  L1_DeCompressPower(void);
void  L1_PreviewPower(void);
void  L1_UpLoadPower(void);
void  L1_DnLoadPower(void);
//void  L1_DoCDSP2(void);
//patch5.0@richie@sdram mapping end
void Set_Edge(void );

// Test
#ifdef TestModeEn

#endif

// TAG(), TAG_FUNC(), PAUSE() are for debugging.

#define TAG_FUNC(str) \
	printf("%s() %d\n", str, (UINT16)__LINE__); \

#define PAUSE() \
	TAG(); \
	getchar(); \

#endif /*__L1_H__*/

