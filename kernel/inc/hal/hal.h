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
//-----------------------------------------------------------------------------
// Header file
//-----------------------------------------------------------------------------
#ifndef __HAL_H__
#define __HAL_H__

#include "general.h"
#include "Hal_device.h"   
#include "hal_gpio.h"
//-----------------------------------------------------------------------------
// Data type
//-----------------------------------------------------------------------------
// date
struct DATESTC
{
  UINT8 Year;
  UINT8 Month;
  UINT8 Day;
  UINT8 Hour;
  UINT8 Minute;
  UINT8 Second;
};

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define HALK_SUCCESS                     0x00
#define HALK_ERROR_GENERAL               0x01
#define HALK_ERROR_PARAMETER             0x02

#define HALK_CPUInternal                 0x01   // 0 is for external ICE
                                               // 1 is for internal 8051
#define HALK_CPUP1                       0x01   // 0 is for low byte address
                                               // 1 is for 8051 GPIO P1
#define HALK_P1OE                        0xff
#define HALK_P3OE                        0x3e   // P3o is input for RXD

/* DMA */
#define	DMA_PATH_DRAM	0
#define	DMA_PATH_SRAM	1	//CPU 4K SRAM (0x3800 ~ 0x3FFF)
#define	DMA_PATH_FLASH	2
#define	DMA_PATH_AUDIO	3
#define	DMA_PATH_USB	4
#define	DMA_PATH_PORT	5	//5: DMA data port (0x2300)
/*#define K_Q50                         0x00 //ada@0225
#define K_Q70                           0x01
#define HALK_Q80                         0x02
#define HALK_Q85                         0x03
#define HALK_Q88                         0x04
#define HALK_Q90                         0x05
#define HALK_Q92                         0x06
#define HALK_Q95                         0x07
#define HALK_Q1                          0xff*/

#define HALK_DateWhite                                   0x00
#define HALK_DateYellow                                  0x01
#define HALK_DateCyan                                    0x02
#define HALK_DateGreen                                   0x03
#define HALK_DateMagenta                                 0x04
#define HALK_DateRed                                     0x05
#define HALK_DateBlue                                    0x06
#define HALK_DateBlack                                   0x07

//GLOBAL
#define PPG struct PGSTC *
#define PDATE struct DATESTC *

// for test flash only
#ifdef  TestModeEn
#define HALK_FMactType   0x03       // 0:GPIOs 1:NAND 2:SMC 3:CF(IDE) 4:CF(MEM) 5:SD 6:EMMC 7:SPI 8:NX
#define HALK_FMinactType 0x01       // 0:Only one flash 1:NAND 2:SMC 3:CF(IDE) 4:CF(MEM) 5:SD 6:EMMC 7:SPI 8:NX
#define HALK_FMSize      0x40       // 64M bytes
#define HALK_FMPageSize  0x01       // 0:256 bytes/page 1:512 bytes/page 2:1024 bytes/page
#endif

//patch4.4@richie@move to DEBUG_PRINTHAL
//#define PRINT_HAL        printf

#define SLASH2 /
#ifdef DEBUG_FRONT
#define PRINT_FRONT printf
#else
#define PRINT_FRONT ;/SLASH2
#endif

#ifdef DEBUG_PRINTHAL
#define PRINT_HAL printf
#else
#define PRINT_HAL ;/SLASH2
#endif

/*add by Phil @08.01.02 s*/

/*add by Phil @08.01.02 e*/

typedef enum {
	HAL_DRAM_DISP_SRC_PB  = (0 << 0),
	HAL_DRAM_DISP_SRC_MPG = (2 << 0),
} HAL_DramDispSrc_e;



typedef enum {
	LOG_BANK_COM = 0,
	LOG_BANK_0,
	LOG_BANK_1,
	LOG_BANK_2,
	LOG_BANK_3,
	LOG_BANK_4,
	LOG_BANK_5,
	LOG_BANK_6,
	LOG_BANK_7,
	LOG_BANK_8,
	LOG_BANK_9,
	LOG_BANK_10,
	LOG_BANK_11,
	LOG_BANK_12,
	LOG_BANK_13,
	LOG_BANK_14,
	LOG_BANK_15,
	LOG_BANK_16,
	LOG_BANK_17,
	LOG_BANK_18,
	LOG_BANK_19,
	LOG_BANK_MAX,
} logBank_t;

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
//extern  xdata   UINT8   HALG_XXX;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

// TestMode
#ifdef TestModeEn
UINT8 HAL_TestGlobal(UINT8);
UINT8 HAL_TestSDRAM(UINT8);
UINT8 HAL_TestFront(UINT8);
UINT8 HAL_TestCDSP(UINT8);
UINT8 HAL_TestLCDTV(UINT8);
UINT8 HAL_TestAudio(UINT8);
UINT8 HAL_TestUSB(UINT8);
UINT8 HAL_TestCPU(UINT8);
UINT8 HAL_TestDMAC(UINT8);
UINT8 HAL_TestFlash(UINT8);
UINT8 HAL_TestTVSRAM(UINT8 bank);
#endif

//Global
UINT8  HAL_Suspend(UINT8);
void   HAL_Resume(void);

UINT8 HAL_CpuImemSdSet(logBank_t Bank, UINT16 Addr);
void HAL_CpuImemSdGet(logBank_t *Bank, UINT16 *Addr);
#if 0// 2007.12.05@ suny mark begin
void   HAL_PowerDown(void);
UINT8  HAL_SetModuPowerDown(UINT16);
UINT8  HAL_SetCPUClock(UINT8);
void   HAL_PowerUp(void);
UINT8  HAL_SetModuPowerUp(UINT16);
#endif// 2007.12.05@ suny mark end

#if 0
UINT8  HAL_ConfigGTimer(UINT8);
void   HAL_StartGTimer(void);
void   HAL_StopGTimer(void);
UINT8  HAL_WriteGTimer(UINT32);

//void   HAL_GlobalTimer0Init(void);
//void   HAL_GlobalTimerInit(void);//1628 FPGA
void   HAL_GlobalTimer2Init(void);//1628 FPGA

//void   HAL_GlobalTimerRead10us(UINT32*);
//void   HAL_GlobalReadGTimer(UINT32*);
UINT8  HAL_10usWait(UINT8);
UINT8  HAL_GT2_10usWait(UINT8);//1628 FPGA
//UINT8  HAL_GlobalTimerWait(UINT32);
UINT8  HAL_GT2_Wait(UINT32);//1628 FPGA
//UINT8  HAL_DateToBin(pDateStc_t, pDateStc_t);
UINT8  HAL_WriteRTCData(UINT8, UINT8);
UINT8  HAL_WriteRTC(pDateStc_t);
//UINT8  HAL_GlobalReadRTCData(UINT8, UINT8*);
//UINT8  HAL_BinToDate(pDateStc_t, pDateStc_t);
//UINT8  HAL_GlobalReadRTC(pDateStc_t);
#if 1// 2007.12.05@ suny mark begin
UINT8  HAL_WriteAlarm(pDateStc_t);
UINT8  HAL_ReadAlarm(pDateStc_t);
#endif// 2007.12.05@ suny mark end

UINT8 HAL_ConfigPG(UINT8 mode, UINT8 polarity,UINT8 idlstate,UINT16 lowDuty,UINT16 highDuty,UINT8 state);
void HAL_StartPG(UINT8  mode) ;
void HAL_StopPG(UINT8  mode) ;

#if 0// 2007.12.05@ suny mark begin
UINT8  HAL_ConfigUI(UINT8);
void   HAL_WakeUI(void);
UINT8  HAL_WriteUI(UINT16);
UINT8  HAL_ReadUI(UINT8*);
#endif// 2007.12.05@ suny mark end
UINT8 HAL_ReadRegister(UINT16 reg) ;
void HAL_WriteRegister(UINT16 reg ,UINT8 value);
//UINT8  HAL_ConfigGPIOBit(UINT8, UINT8);
//UINT8  HAL_ConfigGPIOByte(UINT8, UINT8);
//UINT8  HAL_SetGPIOBit(UINT8, UINT8);
//UINT8  HAL_SetGPIOByte(UINT8, UINT8);
//UINT8  HAL_GetGPIOBit(UINT8, UINT8*);
//UINT8  HAL_GetGPIOByte(UINT8, UINT8*);
//UINT8  HAL_InitGlobal(UINT8);
//UINT8  HAL_SetTGPll(UINT8);
//void   HAL_ReadRevID(UINT8*);
UINT8  HAL_SetCamMode(sp1kCamMode_t);
void   HAL_GetCamMode(UINT8*);
//UINT8  HAL_WaitVD(UINT8, UINT8);
UINT8   HAL_ReadIOTrap(void);
void  HAL_RTCTransferText(pDateStc_t,UINT8*);     //ada@0220
void  HAL_SuspendResume(void);
void HAL_PwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015);
void HAL_DramClkSet(UINT8 DramClk);
void HAL_BaudRateSet(UINT32 BaudRate);
#endif

/* map old hal_gpio api to new ones. */
#define HAL_ConfigGPIOBit(idx, val) \
	HAL_GpioByteDirSet(HAL_GPIO_BYTE_GEN0 + (idx >> 3), 1 << (idx & 0x07), val ? 0xFF : 0x00)
#define HAL_SetGPIOBit(idx, val) \
	HAL_GpioByteOutSet(HAL_GPIO_BYTE_GEN0 + (idx >> 3), 1 << (idx & 0x07), val ? 0xFF : 0x00)
#define HAL_GetGPIOBit(idx, pval) \
	*pval = HAL_GpioByteInGet(HAL_GPIO_BYTE_GEN0 + (idx >> 3), 1 << (idx & 0x07))


//UINT8 halCdspDoYuvHScaling( UINT16 srcXSize, UINT16 srcYSize, UINT16 dstXSize, UINT16 dstYSize );

//Flow 9

#if 0 // hlc 0318 add, start
void AEWinInit();
void sp1kAWBConfig();
#endif // hlc 0318 add, end

//void HAL_StillWB(	UINT32 rawBufAddr,	UINT16 rawXSize,	UINT16 rawYSize);
//void AFTest();
void awbDefaultProc(void);
void awbDefaultProc_Video(void);
void awbDefaultProc_PostWB( void );
void aeDefaultProc(void);
void fdDefaultProc(void);
UINT8 HAL_AWBWindowRead1024(void);
void audioAlcCal_Intr(void);

//UINT8 HAL_AWBWindowRead256(void);

////NEWAWB
#if 0
typedef UINT32 raw_pixel_t;
typedef UINT32 raw_gain_t;
typedef UINT16 pixel_cnt_t;
#define UNITY_GAIN	((raw_gain_t)1024)
#else
typedef UINT8 raw_pixel_t;
typedef UINT16 raw_gain_t; /* allow for raw_pixel_t*UNITY_GAIN */
typedef UINT8 pixel_cnt_t; /* 255 sampling max */
#define UNITY_GAIN	((raw_gain_t)256)
#endif
UINT8 awbWhitePointFilter(raw_pixel_t	R,raw_pixel_t	G,raw_pixel_t	B	);
void awbRBGainDecider(void);
void awbThresholdInit(UINT8 mode);
void awbCalConvergeGain(void);

#if 0
static SINT32 Slop_Calculator(	UINT16	start_x,	UINT16	start_y,	UINT16	end_x,	UINT16	end_y	);
#else
static UINT16 Slop_Calculator(	UINT16	start_x,	UINT16	start_y,	UINT16	end_x,	UINT16	end_y	);
static UINT8 Slop_filter(	UINT16	start_x,	UINT16	start_y,	UINT16	end_x,	UINT16	end_y,	UINT16	slop);
#endif
//-----------------------------------------------------------------------------------------

//UINT8 HAL_SetFixBadPxlThd(UINT8);
//UINT8 HAL_FixOneBadPixel(UINT16, UINT16);
//UINT8 HAL_SetStampFontColor(UINT8, UINT8, UINT8);
//UINT8 HAL_SetStampFontScale(UINT8);
//UINT8 HAL_StampOneFont(UINT32, UINT32, UINT16, UINT16, UINT32);
//UINT8 HAL_RotateImage(UINT32, UINT16, UINT16, UINT32, BIT);
//UINT8 HAL_TransYUV420toYUV422(UINT32, UINT16, UINT16);
//UINT8 HAL_CopyImage(UINT32, UINT16, UINT16, UINT16, UINT16, UINT16, UINT16, UINT32, UINT16, UINT16, UINT16, UINT16);
//UINT8 HAL_SetCopyColorKey(BIT, UINT8);
//UINT8 HAL_SubDarkFrame(UINT32, UINT32, UINT16, UINT16);
//UINT8 HAL_ChkDRAMImgRdy(void);
//UINT8 HAL_CheckDRAMStatus(void);
//UINT8 HAL_SetCapint(UINT8);
void  HAL_DRAMsizetest(void);

UINT8 HAL_CpuRomSdToDram(void);
UINT8 HAL_CpuDramSdToImem(void);
UINT8 HAL_DramSkipWrite2Dram(void);



//LCDTV
UINT8 HAL_InitLCDTV(UINT8 CamMode);
UINT8 HAL_SetDisplaySrcImgSize(UINT16 hsize, UINT16 vsize, UINT8 type);
UINT8 HAL_DefineColorKey(UINT8 attrib);

//FRONT
UINT8 HAL_InitFront(UINT8 CamMode);
UINT8 HAL_SSCConfig (UINT8 SlaveAddr, UINT8 Frequency);
UINT8 HAL_WriteSSC (UINT8 *RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
UINT8 HAL_WriteSSC_Intr (UINT8 *RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
UINT8 HAL_ReadSSC(UINT8 RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
UINT8 HAL_SetFrontTGEn(UINT16 LineTotal, UINT16 LineBlank, UINT16 FrameTotal, UINT16 FrameBlank, UINT8 SyncMode, UINT8 TGEn);
UINT8 HAL_SetFrontYUVEn(UINT8 UVSel, UINT8 YSub128en, UINT8 USub128en, UINT8 VSub128en, UINT8 Bt656en, UINT8 YUVen);
UINT8 HAL_SetFrontYUVEn(UINT8 UVSel,UINT8 YSub128en,UINT8 USub128en,
	UINT8 VSub128en,UINT8 Bt656en,UINT8 YUVen);
UINT8 HAL_SetFrontInverse(UINT8 Hinverse, UINT8 Vinverse);
UINT8 HAL_SetFrontImgCrop(UINT16 HOffset, UINT16 VOffset, UINT16 HSize, UINT16 VSize, UINT8 CropMode);
UINT8 HAL_SetFrontReshape(UINT16 HFall, UINT16 HRise, UINT8 HReshen, UINT16 VFall, UINT16 VRise, UINT8 VReshen);
UINT8 HAL_SetFrontIOConfig(UINT8 TGGPIOSel, UINT8 TGInputGate, UINT8 TGGPIOen, UINT8 TGGPIOVal);
UINT8 HAL_SetFrontCLKSet(UINT8 MasterClkDiv, UINT8 PixelClkDiv, UINT8 SyncMode);
UINT8 HAL_SetFrontCLKSource(UINT8 MasterClkSrc, UINT8 PixelClkSrc);
UINT8 HAL_SetFrontInterrupten(UINT16 Enable);
UINT8 HAL_ClrFrontInterrupt(UINT16 Clear);
UINT8 HAL_FrontInterruptCfg(UINT8 VdRintNum, UINT8 VdFintNum, UINT16 IntHNum0, UINT16 IntHNum1, UINT16 IntHNum2, UINT16 IntHNum3);


//CPU
#if 0
UINT8 HAL_InitCPU(UINT8);
#endif
UINT8 HAL_CpuSramModeSet(UINT8);
UINT8 HAL_CpuRamMapSet(UINT32 dramAddrWord, UINT16 mapSizeByte, void xdata* xdata* ppMem);
UINT8 HAL_CpuRamMapRcv(void);

#if 0
UINT8 HAL_4KSRAMBufIdx(UINT8, UINT16, UINT16*, UINT16*);
void  HAL_Write4KSRAMPort(UINT8);
void  HAL_Read4KSRAMPort(UINT8*);
#endif
UINT8 HAL_DownloadROM(UINT16, UINT32, UINT16);
void  HAL_Int0(void);
void  HAL_EnterInterrupt(void);
void  HAL_LeaveInterrupt(void);
void  HAL_DisableInterrupt(void);
void  HAL_EnableInterrupt(void);
void  HAL_CtlInOutEnable(void);

//DMA
UINT8 HAL_NandDMACheck(void);
UINT8 HAL_DoNandDMA(UINT8 , UINT16 );
UINT8 HAL_DmaDo(UINT8, UINT8, UINT32, UINT8, UINT8);
void  HAL_DmaReset(void);
UINT8 HAL_DmaSetDramAddr(UINT32);
UINT8 HAL_DmaSetSramAddr(UINT16);
//UINT8 HAL_SetUSBDMA(UINT8, UINT8);
UINT8 HAL_SetAudDMA(UINT8 );
void  HAL_DmaReadDramAddr(UINT32* );
//UINT8 HAL_SearchPattern(UINT32, UINT8, UINT8, UINT16, UINT16*, UINT16*);
UINT8 INTR_HAL_DmaSetDramAddr(UINT32 DRAMAddr);
UINT8 INTR_HAL_DmaSetSramAddr(UINT16 SramRstIdx);
UINT8 INTR_HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SramMode);

//FLASH
UINT8 HAL_FlashMode(UINT8, UINT8, UINT8);
UINT8 HAL_FlashMode_Intr(UINT8, UINT8, UINT8);
UINT8 HAL_ReadECC(UINT8, UINT8*);
void HAL_ClearECC(void);
void HAL_ECCMode(UINT8);

//NAND
//UINT8 HAL_NANDInit(UINT8 , UINT8);
//UINT8 HAL_NANDSendCmd(UINT8);
//UINT8 HAL_NANDSendAddr(UINT8,UINT32);
//void  HAL_NANDCheckRdy(UINT8*);
//void  HAL_NANDWritePort(UINT8);
//UINT8 HAL_NANDReadPort(void);
//void  HAL_NANDCompleteOperation(void);
//void HAL_NANDEnableWriteProtect(void);
/*Phil add new MTD driver s*/
//UINT8 HAL_NANDChkRdy(void);
//UINT8 HAL_NANDAutoCmdCfg(UINT8 xdata* pCmd, UINT8 CmdCnt, UINT8 xdata* pAddr, UINT8 AddrCnt);
//void HAL_NANDSetDRAMDMA(UINT32 DRAMAddr);
//void HAL_NANDDoDMACfg(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 FlashPageDMA);
//UINT8 HAL_NANDDoDMAWait(void);
/*Phil add new MTD driver e*/
// SD

//UINT8 HAL_SDCardDetect(UINT8*);     //ada@0218
//void HAL_SDInit(UINT8);
//void HAL_SDConfig(UINT8 ,UINT8 ,UINT8);
//void HAL_SDReset(void);
//void HAL_SDBlockSize(UINT16 );

//UINT8 HAL_SDTxCommand(unsigned char, unsigned long, unsigned char, UINT8*);

//void HAL_SDReadRspBuf(UINT8 , UINT8* );
//void HAL_SDRspBufState(UINT8* );
//void HAL_SDDataBufState(UINT8* );
//void HAL_SDReadCRC7(UINT8* );
//void HAL_SDCardCRCStat(UINT8* );
//void HAL_SDTimeOut(UINT8* );
//void HAL_SDTxData(void);
//void HAL_SDRxData(void);
//void HAL_SDRxCRC(void);
//unsigned char HAL_SDTxDummy(void);

//patch4.5@ada@Add timeout count begin
//UINT8 HAL_SDWaitIdle(void);
//UINT8 HAL_SDCmdWaitIdle(void);
//void HAL_SDWaitIdle(void);
//patch4.5@ada@Add timeout count end

//UINT8 HAL_SDCheckDat0(void);
//void HAL_SDReadPort(UINT8* );
//void HAL_SDWritePort(UINT8 );
//UINT8 sdSectorW(UINT32 start, UINT32 num, UINT32 SrcAddr);
//UINT8 sdSectorR(UINT32 start, UINT32 num, UINT32 TarAddr);
//#define	SD_TIME_OUT	(XBYTE[REG_Fm_SD_Status] & (1<<6))
//#define	SD_CRC16_OK	!(XBYTE[0x2400] & 0x08)//(XBYTE[REG_Fm_CRC16cor])


//USB
//UINT8 HAL_BulkInOut(void);
//UINT8 HAL_BulkOutToDRAM(UINT32 DRAMAddr, UINT16 nByte);
//UINT8 HAL_BulkInFromDRAM(UINT32 DRAMAddr, UINT16 nByte);
//UINT8 HAL_InitUSB (UINT8);
//void HAL_USBSwPlugOut(void);
//void HAL_USBSwPlugOut_Intr(void);
//void HAL_USBSwPlugIn(void);
//void HAL_USBPccamInit(void);
//void HAL_USBMjpgQTableWrite(void);
//UINT8 HAL_USBAutoCompress(UINT32 VLCAAddr, UINT32 VLCBAddr, UINT8 Option);
//UINT8 HAL_UsbBulkEnable(UINT8 BulkDir);
//void HAL_USBForceFS(void);
//UINT8 HAL_UsbParaSet(UINT8 selector,UINT8 value);

//UINT8 HAL_AdcGPIOConfig(UINT8 index, UINT8 intEn, UINT16 sarClk);
//UINT8 HAL_AdcGPIOGet(UINT8 index, UINT16* value);



#endif  /* __HAL_H__ */

