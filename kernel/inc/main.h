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

#ifndef __MAIN_H__
#define __MAIN_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//Firmware Version
#define K_FIRMWARE_VERSION              ((UINT8)0x44)
#define K_FIRMWARE_RELEASE              ((UINT8)0x00)
#define K_FIRMWARE_SEQUENCE             ((UINT8)0x00)
#define K_FIRMWARE_CHIPNUMBER           ((UINT8)0x04)
//Size of Buffer
#define K_CtrlCommandSize               8               //size of G_ucCtrlCommand
#define K_CtrlDataSize                  64              //size of G_ucCtrlData
#define K_StorCommandSize               64              //size of G_ucStorCommand
#define K_StorDataSize                  (512+16)        //size of G_ucStorData
#define K_DMA_MaxSize                   1024            //maximun size of DMA //ada@0118
/*++
#define K_Stor2CommandSize              64              //size of G_ucStor2Command
#define K_Stor2DataSize                 (512+16)        //size of G_ucStor2Data
--*/
//Interface Index
#define K_VideoInterface                0
#define K_StorageInterface1             1
#define K_AudioControlInterface         2
#define K_AudioStreamingInterface       3
#define K_StorageInterface2             4

//cytsai@1211
//use in main loop
//capture
#define K_CAPTURE_Image                 0
#define K_CAPTURE_AudioStream           1
#define K_CAPTURE_VideoStream           2
#define K_CAPTURE_AudioVideoStream      3
//delete
#define K_DELETE_ALL                    0
//playback
#define K_PLAYBACK_NineThumbnail        0
#define K_PLAYBACK_FourThumbnail        1
#define K_PLAYBACK_OneImage             2
#define K_PLAYBACK_ImageSlide           3
#define K_PLAYBACK_Video                4
//upload
#define K_UPLOAD_ObjectCount            0
#define K_UPLOAD_ObjectFDBs             1
#define K_UPLOAD_OneObject              2

#define K_Card_PageSize            ((UINT16) 0x0200)      // WWW1
#define K_Card_PageSizeShift		((UINT8) 9)      // WWW1
#define G_3AFreq 3
//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
#if 1 // NO "extern" in header files
extern  xdata    UINT8    G_btStopStream;
extern  xdata    UINT8   G_bRequestLock;
extern  xdata    UINT8   G_bRequestLog;        //cch@10/15
extern  xdata    UINT16  G_wIndexLock;
extern  xdata    UINT16  G_wValueLock;
//#ifdef ICAT_OPTION
extern  xdata    UINT16  G_wLengthLock;			//xian ++
//#endif
extern 	xdata 	UINT8	G_bUSBSpeed;
extern  xdata   UINT8   G_ucState;
extern  xdata   UINT8   G_ucState2;             //cch@10/15
extern  xdata   UINT8   G_ucState3;
extern  xdata   UINT8   G_usbState;
extern  xdata   UINT32   G_ulErrorCode;
extern  xdata   UINT8   G_ucStorageType;
extern  xdata   UINT8   G_ucCtrlCommand[];
extern  xdata   UINT8   G_ucCtrlData[];
extern  xdata   UINT8   G_ucStorCommand[];
extern  xdata   UINT8   G_ucStorData[];
/*++
extern  xdata   UINT8   G_ucStor2Command[];
extern  xdata   UINT8   G_ucStor2Data[];
--*/
extern  UINT8   *G_pucCtrlDataPointer;  //for both CODE and DATA AREA
extern  xdata   UINT8   *G_pucStorDataPointer;
/*++
extern  xdata   UINT8   *G_pucStor2DataPointer;
--*/
extern  xdata   UINT8   G_ucPktStorIntrf;

//L1 global variables
//ISP
extern  xdata   UINT8   G_ISP;
extern  xdata   UINT32   G_ISPDRAMAddr;
extern  xdata   UINT32   G_ISPROMAddr;
extern  xdata   UINT32   G_ISPSecAddr;
extern  xdata   UINT16  G_ISPSecCnt;
//device component
extern  xdata   UINT8   G_SensorID;
//patch5.0@richie@sdram mapping mark
//extern  xdata   UINT8   G_CardType;
extern  xdata   UINT8   G_AudioModuleType;
extern  xdata   UINT8   G_DisplayType;
//mode setting
//extern  xdata   UINT8   G_QtableIndex;     //ada@0225
extern  xdata   UINT8   G_DRAMint;
//user interface
extern  xdata   UINT16  G_ObjectID;
extern  xdata   UINT8   G_ObjectType;
extern  xdata   UINT16  G_KeyState;
//component setting
extern  xdata   UINT8   G_FrameRate;
//cx add
extern  xdata	 UINT8   G_FrameIndex; 
extern  xdata	 UINT8	G_FormatIndex;
extern  xdata	UINT32	G_PixelClk;
extern  xdata	UINT16	G_HTotal;
extern  xdata	UINT16	G_VTotal; 
//extern  xdata   UINT8	G_ucProbeControlFlag;
extern xdata   UINT8	G_ucCommitControlFlag; 
extern xdata	UINT8	G_ucPccamTuningFlag;
extern xdata	UINT8	G_ae_ON;

//cx add
extern  xdata   UINT16  G_Hsize;
extern  xdata   UINT16  G_Vsize;
extern  xdata   UINT16  G_MHSize;
extern  xdata   UINT16  G_MVSize;
extern  xdata   UINT16  G_DSPHsize;
extern  xdata   UINT16  G_DSPVsize;
extern  xdata   UINT8   G_PVZFactor;
//extern  xdata   UINT8   G_PBZFactor;
//extern  xdata   UINT8   G_3ACount;
//extern  xdata   UINT8   G_3AFlag;
extern  xdata   UINT8   G_AWBCount;
extern  xdata   UINT8   G_PBID;
extern  xdata   UINT8   G_GOSDID;
//SDRAM arrangement
extern  xdata   UINT32   G_BadPxlAddr;
extern  xdata   UINT32   G_FOSDAddr;
extern  xdata   UINT32   G_AudioAddr;
extern  xdata   UINT32   G_FreeAddr;
extern  xdata   UINT32   G_RawImageBufAddr; //version4.2@yichang@0513 for continue snapping
//date/time
extern  xdata   UINT8   G_DateStamp;
//extern  xdata   dateStc_t G_DATEINFO;

//USB
//extern  xdata   UINT32   G_BulkSize;
//extern  xdata   UINT32   G_BulkDRAMAddr;
//extern  xdata   UINT32   G_BulkFlashType;
//extern  xdata   UINT32   G_BulkSRAMAddr;
//extern 	xdata 	UINT32 	G_LastBulkSize;

//testing
extern  xdata   UINT8   G_L1_ModuleID;
extern  xdata   UINT8   G_HAL_ModuleID;
extern  xdata   UINT8   G_FuncID;
extern  xdata   UINT8   G_HAL_FuncID;
extern  xdata   UINT32   G_ArgBuf[];

//richie
//patch4.4@richie@usbvd
//for video clip
extern  xdata   UINT8 G_ABVLCIntFlag;
extern  xdata   UINT32 G_AVLCSize;
extern  xdata   UINT32 G_BVLCSize;

//patch3.2@ada@0401 For file system refreshing when back from mass storage
//For system refreshing when back from mass storage
extern  xdata UINT8 G_MassStorage_Mode;

//richie@si0418
extern xdata UINT32 G_ulExifThumbAddr;
extern xdata UINT32 G_ulExifThumbSize;

//patch4.2@ada@0527 for key scan
extern xdata UINT8 G_MainLoopCount;

//patch4.4@richie@flip
extern	xdata	UINT8	G_ucFlip;

//patch4.5@richie@mlun begin
//patch4.5@richie@mlun end

//patch4.5@richie@vender info begin
#if 0
extern	xdata UINT8 G_ucMSDC_CARD;
extern	xdata UINT8 G_ucMSDC_BUILT_IN;
#endif
/*
extern	xdata    UINT8   G_ScsiVendorInformation[];
extern	xdata    UINT8   G_ScsiProductIdentification[];
extern	xdata    UINT8   G_ScsiProductRevisionLevel[];
*/
//patch4.5@richie@vender info end

/* global timer */
extern xdata UINT8 Gt0Cnt;
extern xdata UINT8 Gt1Cnt;
extern xdata UINT8 Gt2Cnt;
#endif 
//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void __main(void);
void MAIN_LockLoop(void);
void STROBE_Charge(void); // WWWW0409

UINT8* sp1kFWVerGet(void);
UINT32 sp1kSvnVerGet(void);

UINT8* sp1kFWDateGet(void);
UINT8* sp1kFWTimeGet(void);

#endif /*__MAIN_H__*/
