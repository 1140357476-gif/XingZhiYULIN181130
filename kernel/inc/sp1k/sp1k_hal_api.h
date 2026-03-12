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
#ifndef __SP1K_HAL_API_H__
#define __SP1K_HAL_API_H__

/** \file sp1k_hal_api.h
 * @brief		Spca1528 hardware related interface
 */

#include "hal.h"
#include "hal_jpeg.h"
#include "hal_disp.h"
#include "sp1k_def.h"
#include "hal_global.h"
#include "hal_usb.h"
#include "hal_audio.h"
#include "Hal_cdsp.h"
#include "Hal_dram.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	USB_PARA_SINGLE_LUN_SHOW_NAND = 0,
	USB_PARA_MAX_ID,
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn void sp1kHalUSBSwPlugIn(void)
 * @brief	Execute USB software plug in, it should be called while USB plug in for starting USB protocol.
 * @param	None
 * @retval 	None
 * @see sp1kHalUSBSwPlugOut()
 */
void sp1kHalUSBSwPlugOut(void);
/**
 * @fn void sp1kHalUSBSwPlugOut(void)
 * @brief	Execute USB software plug out, it should be called while USB plug out.
 * @param	None
 * @retval 	None
 * @see sp1kHalUSBSwPlugIn()
 */
void sp1kHalUSBSwPlugIn(void);

/**
 * @fn UINT8 sp1kHalDispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac)
 * @brief	set image zoom factor
 * @param[in]	hsize	: width of src image. unit:pixel
 * @param[in]	vsize	: high of src image. unit:pixel
 * @param[in]	zoomfac	: zoom factor. 1x=100,2x=200,...etc
 * @retval		err: 0=OK, if not zero:
 *\n 				bit0=X_UNDERFLOW, bit1=X_OVERFLOW
 *\n				bit2=X_UNDERFLOW, bit3=X_OVERFLOW
 */
UINT8 sp1kHalDispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac, UINT8 update);

/**
 * @fn void sp1kHalReadGTimer(UINT32* GtimerPt)
 * @brief	Read the global timer value that is specified with the unit of mini-second.
 * @param[out] GtimerPt 	Pointer to the buffer containing the Golbal timer count (ms).
 * @retval 	None
 */
void sp1kHalReadGTimer(UINT32* GtimerPt);

/**
 * @fn UINT8 sp1kHalGetGPIOBit(UINT8 Index, UINT8* GPIOBitValPt)
 * @brief	Get the value of one GPIO. The function only get one GPIO pin value when the function is called once
 * @param[in] Index 			the index of a GPIO pin
 * @param[out] GPIOBitValPt		when it is zero, the corresponding GPIO is cleared,otherwise the GPIO is set.
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalGetGPIOBit(UINT8 Index, UINT8* GPIOBitValPt);

/**
 * @fn UINT8 sp1kHalGPIOBitSet(UINT8 Index, UINT8 Value)
 * @brief	Set the value of one GPIO. The function is used in the GPIO pins of output
 *\n		type and only controls one GPIO pin when the function is called once. There
 *\n		is a table to map the GPIO index to the pin number on SPCA1528.
 * @param[in]	Index : the index of a GPIO pin
 * @param[in]	Value : when it is zero, the corresponding GPIO will be cleared;
 *\n     				otherwise the GPIO will be set.
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalGPIOBitSet(UINT8 Index, UINT8 Value);

/**
 * @fn void sp1kHalPanelOn(void)
 * @brief	Turn On LCD/LCM via SPI cmd
 * @param 	None
 * @retval 	None
 * @see sp1kHalPanelOff()
 
void sp1kHalPanelOn(void);
*/

/**
 * @fn void sp1kHalPanelOff(void)
 * @brief	Turn Off LCD/LCM via SPI cmd
 * @param 	None
 * @retval 	None
 * @see sp1kHalPanelOn()

void sp1kHalPanelOff(void);
*/

/**
 * @fn UINT8 sp1kHalDRAMSetStartAddr(UINT32 DRAMAddr, UINT8 Read)
 * @brief	Set the SDRAM read/write starting address (word address)
 * @param[in]	DRAMAddr :SDRAM read/write starting address
 * @param[in]	Read	 :Read/write should set to 1/0
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalDRAMSetStartAddr(UINT32 DRAMAddr, UINT8 Read);

/**
 * @fn UINT8 sp1kHalDRAMWriteWord(UINT8 LowByteData, UINT8 HighByteData)
 * @brief	Write a word to the SDRAM
 * @param[in]	LowByteData		:The low byte data to be written to the SDRAM
 * @param[in]	HighByteData	:The high byte data to be written to the SDRAM
 * @retval error code -  success: 0, fail: 1
 * @see sp1kHalDRAMReadWord()
 */
UINT8 sp1kHalDRAMWriteWord(UINT8 LowByteData, UINT8 HighByteData);

/**
 * @fn UINT8 sp1kHalDRAMReadWord(UINT8 *LowByteData, UINT8 *HighByteData)
 * @brief	Read a word from the SDRAM
 * @param[out]	LowByteData		: Pointer to the buffer containing Low byte data
 * @param[out]	HighByteData	: Pointer to the buffer containing High byte data
 * @retval error code -  success: 0, fail: 1
 * @see sp1kHalDRAMWriteWord()
 */
UINT8 sp1kHalDRAMReadWord(UINT8 *LowByteData, UINT8 *HighByteData);

/**
 * @fn void sp1kHalGprmScale(UINT32 srcaddr, UINT16 srchsize, UINT16 srcvsize,UINT32 dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT32 tmpaddr, UINT8 wait)
 * @brief	GPRM scaling function.
 * @param[in]	srcaddr 	: source image address (word address)
 * @param[in]	srchsize 	: source image hsize
 * @param[in]	srcvsize	: source image vsize
 * @param[in]	dtnaddr		: destination address (word address)
 * @param[in]	dtnhsize	: destination hsize
 * @param[in]	dtnvsize	: destination vsize
 * @param[in]	tmpaddr		: temporary address (word address, the temp buffer size should larger than source and destination size)
 * @param[in]	wait		: 1 - wait for GPRM done, 0: function return immediately
 * @retval 	None
 */
void sp1kHalGprmScale(UINT32 srcaddr, UINT16 srchsize, UINT16 srcvsize,UINT32 dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT32 tmpaddr, UINT8 wait);

/**
 * @fn void sp1kHalGprmCpy(UINT32  srcaddr, UINT16 srchsize, UINT16 srcvsize, UINT16 srchoft, UINT16 srcvoft,UINT32  dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT16 dtnhoft, UINT16 dtnvoft,UINT16 cphsize, UINT16 cpvsize,  UINT16 pastethd, UINT8 wait)
 * @brief	GPRM copy with color key function.
 * @param	srcaddr 	: source image address
 * @param	srchsize 	: source image hsize
 * @param	srcvsize	: source image vsize
 * @param	srchoft 	: source image copy hoffset
 * @param	srcvoft 	: source image copy voffset
 * @param	dtnaddr 	: destination image address
 * @param	dtnhsize 	: destination image hsize
 * @param	dtnvsize 	: destination image hsize
 * @param	dtnhoft 	: destination image paste hoffset
 * @param	dtnvoft 	: destination image paste voffset
 * @param	cphsize 	: copy hsize
 * @param	cpvsize 	: copy vsize
 * @param	pastethd 	: threshold of color key. bit0~6 is threshould;bit7 is enable bit.
 * @param[in]	wait	: 1 - wait for GPRM done, 0 - function return immediately
 * @retval	NONE
 */
void sp1kHalGprmCpy(UINT32  srcaddr, UINT16 srchsize, UINT16 srcvsize, UINT16 srchoft, UINT16 srcvoft,UINT32  dtnaddr, UINT16 dtnhsize, UINT16 dtnvsize, UINT16 dtnhoft, UINT16 dtnvoft,UINT16 cphsize, UINT16 cpvsize,  UINT16 pastethd, UINT8 wait);

/**
 * @fn UINT8 sp1kHalFlashModeSet(UINT8 ActFlash, UINT8 InactFlash, UINT8 PageSize)
 * @brief	Flash type selection
 * @param[in]	ActFlash 	: active flash type
 *\n       					0: None (fmgpio[17:0] is used as GPIO's)
 *\n						1: NAND gate flash
 *\n						5: SD
 *\n						6: EMMC
 *\n						7: SPI interface
 * @param[in]	InactFlash 	: inactive flash type
 *\n				        0: None (only one storage media type use in SPCA1528)
 *\n				        1: NAND gate flash
 *\n						5: SD
 *\n						6: EMMC
 *\n						7: SPI interface
 * @param[in]	PageSize 	: page size of active flash
 *\n      					0: 256 bytes/page
 *\n						1: 512 bytes/page
 *\n						2: 1024 bytes/page
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalFlashModeSet(UINT8 ActFlash, UINT8 InactFlash, UINT8 PageSize);

/**
 * @fn void sp1kHalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015)
 * @brief		Power control setting for every module.
 * @param[in]	cpuclk 		: CPU clock setting. The real cpu clock is 96MHz/cpuclk
 * @param[in]	dramclk 	: DRAM clock setting. The real DRAM clock is 96MHz/dramclk
 * @param[in]	reg2013 	: Turn on/off specfic module clock.
 * @param[in]	reg2014 	: Turn on/off specfic module clock.
 * @param[in]	reg2015 	: Turn on/off specfic module clock.
 * @retval 	None
 */
void sp1kHalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015);

/**
 * @fn UINT8 sp1kHalWriteRTC(pDateStc_t DatePt)
 * @brief		Write the value of the date structure, translate them to 48-bit binary
 *\n 			count and then program the binary count to the RTC counter.
 * @param[in]	* DatePt : the value of the date structure that is composed of the data defined as follows:
 *\n			UINT8 Year,
 *\n			UINT8 Month,
 *\n			UINT8 Day,
 *\n			UINT8 Hour,
 *\n			UINT8 Minute,
 *\n			UINT8 Second.
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalWriteRTC(pDateStc_t DatePt);

/**
 * @fn UINT8 sp1kHalReadRTC(pDateStc_t DatePt)
 * @brief		Read the value of the RTC date structre
 * @param[out]	* DatePt : the value of the date structure that is composed of the data defined as follows:
 *\n			UINT8 Year,
 *\n			UINT8 Month,
 *\n			UINT8 Day,
 *\n			UINT8 Hour,
 *\n			UINT8 Minute,
 *\n			UINT8 Second.
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalReadRTC(pDateStc_t DatePt);

/**
 * @fn UINT8 sp1kHalUsbBulkEnable(UINT8 BulkDir)
 * @brief		USB bulk out/in enable
 * @param[in]	BulkDir : 0: bulk out enable, 1:bulk in enable
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalUsbBulkEnable(UINT8 BulkDir);

/**
 * @fn void sp1kGlobalTimerInit(void)
 * @brief	Initialize the global timer (set initial value and start to count).
  * @param	None
 * @retval 	None
 */
void sp1kGlobalTimerInit(void);

/**
 * @fn UINT8 sp1kHalWait(UINT32 WaitTime)
 * @brief	Pause the firmware execution by calling this API for waiting a period of time.
 *\n		The timing is calculated by the up-count mode of the global timer.
 * @param[in]	WaitTime: The wait time that is specified by the unit of mini-second.
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalWait(UINT32 WaitTime);

/**
 * @fn UINT8 sp1kHalBulkInFromDRAM(UINT32 DRAMAddr, UINT16 nByte)
 * @brief	Bulk in from DRAM to USB (DMA).
 * @param	DRAMAddr	:Source DRAM word address
 * @param	nByte		:Transfer size
 * @retval error code -  success: 0, fail: 1
 */
UINT8 sp1kHalBulkInFromDRAM(UINT32 DRAMAddr, UINT16 nByte);

/**
* @fn UINT8 sp1kHalConfigPG(UINT8 mode, UINT8 polarity, UINT8 idlstate, UINT16 lowDuty, UINT16 highDuty, UINT8 state);
 * @brief	Config  the PWM waveform parameter
 * @param[in]	mode :
 *\n			0 slecet PWM0 output //gpio7
 *\n			1 slecet PWM1 output //gpio8
* @param[in]	polarity :
 *\n			0  normal waveform
 *\n			1  reverse waveform
* @param[in]	idlstate :
 *\n			0  signal stays at 0 in idle state
 *\n			1  signal stays at 1 in idle state
* @param[in]	lowDuty : Low period .PWM0: 14bit(MAX value : 16384-1);PWM1: 11bit(MAX value : 2048-1);
* @param[in]	highDuty : High period.PWM0: 14bit(MAX value : 16384-1);PWM1: 11bit(MAX value : 2048-1);
* @param[in]	state:
 *\n			0 PWM in idle state
 *\n			1 PWM in opration
 * @retval	0 is success,1 is  general error
 */
UINT8  sp1kHalConfigPG(UINT8 mode, UINT8 polarity, UINT8 idlstate, UINT16 lowDuty, UINT16 highDuty, UINT8 state);

/**
* @fn sp1kHalStartPG(UINT8 mode);
 * @brief	Trig PWM waveform output
 * @param[in]	mode :	0 slecet PWM0 output, 1 slecet PWM1 output
 * @retval	NONE
 */
void  sp1kHalStartPG(UINT8 mode);

/**
* @fn sp1kHalStopPG(UINT8 mode);
 * @brief	Stop PWM waveform output
 * @param[in]	mode :	0 Stop PWM0 output, 1 Stop PWM1 output
 * @retval	NONE
 */
void  sp1kHalStopPG(UINT8 mode);


/**
* @fn sp1kHalDramClkSet(UINT8 DramClk);
 * @brief	Dram clock switching
 * @param[in]	DramClk :	Only allow 48/96
 * @retval	NONE
 */
void  sp1kHalDramClkSet(UINT8 DramClk);

/**
* @fn sp1kHalBaudRateSet(UINT32 BaudRate);
 * @brief	Baud Rate setting.
 * @param[in]	BaudRate :	Baud rate (Allow: 57600/19200)
 * @retval	NONE
 */
void  sp1kHalBaudRateSet(UINT32 BaudRate);

/**
* @fn sp1kHalUsbparaSet(UINT8 selector,UINT8 value);
 * @brief	USB related parameters setting.
 * @param[in]	selector :	usb parameter selector
 * @param[in]	value :	value for corresponding selector.
 * @retval	0 is success,1 is  general error
 */
UINT8  sp1kHalUsbparaSet(UINT8 selector,UINT8 value);

/**
* @fn sp1kHalSpeakerPwrSet(UINT8 on);
 * @brief	Audio speaker power seting.
 * @param[in]	on :	speaker power en
 * @retval	NONE
 */
void sp1kHalSpeakerSet(UINT8 on);

/**
* @fn sp1kHalAudPwerSave(UINT8 on);
 * @brief	set the audio module power consumption
 * @param	  [in] on : 1 : save power
 					 0 : resume
 * @retval	NONE
 */
void sp1kHalAudPwerSave(UINT8 on);


/**
 * @fn        void sp1kHalPwrMosSet(UINT8 en)
 * @brief     HAL_GlobalPwrMosSet
 * @param     [in] en: 1: enable card power MOS
 				    0: Disable card power MOS	
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
void sp1kHalPwrMosSet(en);

/**
 * @fn sp1kHalAudI2SSet(UINT8 en);
 * @brief	    set the audio output to GPIO19~22
 * @param[in]	1: enable, 0: disable
 * @retval	NONE
 */
void sp1kHalAudI2SSet(UINT8 en);
/**
 * @fn UINT8 sp1kHalAlarmRTCIntrSet(UINT8 enable)
 * @brief	Set RTC alarm interrupt.
 * @param[in] enable	0: disable RTC interrupt.
 *                                1: enable RTC interrupt
 * @retval     Alarm IntrEvent
 */
UINT8 sp1kHalAlarmRTCIntrSet(UINT8 enable);


#else
#define	sp1kHalUSBSwPlugIn             HAL_UsbSwPlugIn
#define	sp1kHalUSBSwPlugOut            HAL_UsbSwPlugOut
#define	sp1kHalDispScaleSet            HAL_DispScaleSet
#define	sp1kHalReadGTimer              HAL_GlobalReadGTimer
#define	sp1kHalGetGPIOBit              HAL_GetGPIOBit
#define	sp1kHalReadIOTrap              HAL_GlobalReadIOTrap
//#define	sp1kHalPanelOn                 HAL_PanelOn
//#define	sp1kHalPanelOff                HAL_PanelOff
#define	sp1kHalDRAMSetStartAddr        HAL_DramStartAddrSet
#define	sp1kHalDRAMWriteWord           HAL_DramWordDataWrite
#define	sp1kHalDRAMReadWord            HAL_DramWordDataRead
#define	sp1kHalGprmScale               HAL_GprmScale
#define	sp1kHalGprmCpy                 HAL_GprmCopy
#define	sp1kHalGPIOBitSet              HAL_SetGPIOBit
#define	sp1kHalFlashModeSet            HAL_FlashMode
#define	sp1kHaRomShadowToDram          HAL_CpuRomSdToDram
#define	sp1kHalDramShadowToImem        HAL_CpuDramSdToImem
#define	sp1kHalPwrCtrl                 HAL_GlobalPwrCtrl
#define	sp1kHalWriteRTC                HAL_GlobalWriteRTC
#define	sp1kHalReadRTC                 HAL_GlobalReadRTC
#define	sp1kHalWriteRTCAlarm           HAL_GlobalWriteAlarm
#define	sp1kHalReadRTCAlarm            HAL_GlobalReadAlarm
#define	sp1kHalReadRTCReg              HAL_GlobalReadRTCData
#define	sp1kHalWriteRTCReg             HAL_GlobalWriteRTCData
#define sp1kHalAlarmRTCIntrSet         Hal_GlobalAlarmRTCIntrSet
#define	sp1kHalSkipWrite2Dram          HAL_DramSkipWrite2Dram
#define	sp1kHalUsbBulkEnable           HAL_UsbBulkEnable
#define	sp1kHalDisplayEnable           //HAL_Display_enable
#define	sp1kHalDramDispSrcSel          HAL_DramDispSrcSel
#define	sp1kGlobalTimerInit	           HAL_GlobalTimerInit
#define	sp1kHalWait                    HAL_GlobalTimerWait
#define sp1kHalBulkInFromDRAM          HAL_UsbBulkInFromDRAM
#define sp1kHalConfigPG                HAL_GlobalConfigPG
#define sp1kHalStartPG                 HAL_GlobalStartPG
#define sp1kHalStopPG                  HAL_GlobalStopPG
#define sp1kHalDramClkSet              HAL_GlobalDramClkSet
#define sp1kHalBaudRateSet             HAL_GlobalBaudRateSet
#define sp1kHalPwrMosSet               HAL_GlobalPwrMosSet
#define sp1kHalUsbparaSet              HAL_UsbParaSet
#define sp1kHalSpeakerSet              HAL_AudSpeakerPwrSet
#define sp1kHalAudPwerSave             HAL_AudPwrSave
#define sp1kHalAudI2SSet               HAL_AudI2SSet
#define sp1kHALCdspAFWinEn             HAL_CdspAFWinEn
#define sp1kHALCdspAFdataModeSet       HAL_CdspAFDataModeSet
#define sp1kHALCdspAFfilterwtSet       HAL_CdspAFFilterWtSet
#define sp1kHALCdspAFcoringSet         HAL_CdspAFCoringSet
#define sp1kHALCdspAFwinsizeSet        HAL_CdspAFWinSizeSet
#define sp1kHALReadAFWindow            HAL_ReadAFWindow
#define sp1kHALDispImgXyGet			HAL_DispImgXyGet
#define sp1kHALDispImgSizeGet		HAL_DispImgSizeGet
#define sp1kHALDispPbFrameBufferGet	HAL_DispPbFrameBufferGet
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn void sp1kHalCamModeSet(sp1kCamMode_t CamMode)
 * @brief	Set camera operation mode, the related initialization must be done before calling this function.
 * @param[in]	CamMode: the camera operation mode
 *\n			0: idle mode
 *\n			1: preview mode
 *\n			2: still camera mode
 *\n			3: video clip mode
 *\n			4: PC-camera mode
 *\n			5: playback mode
 *\n			6: Upload/Download mode
 * @retval none
 */
void sp1kHalCamModeSet(sp1kCamMode_t CamMode);

/**
 * @fn void sp1kHalJpegDecode( UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize,  UINT8 Option, UINT16 MCURstNo );
 * @brief	jpeg decode function. It decodes 422, 420, 400 size up to 4095 vlc bitstream to YUV data
 * @param[in] SrcAddr		The source vlc address
 * @param[in] SrcHsize		The horizontal size of source image
 * @param[in] SrcVsize		The vertical size of source image
 * @param[in] DstAddr		The destination YUV address
 * @param[in] PbRescalSize	Jpeg decompress rescale factor.
 *\n                        0: full image size
 *\n                        1: 1/8 image size
 *\n                        2: 2/8 image size
 *\n                        3: 3/8 image size
 *\n                        4: 4/8 image size
 *\n                        5: 5/8 image size
 *\n                        6: 6/8 image size
 *\n                        7: 7/8 image size
 * @param[in] Option 		bit[1:0] Jpeg format 0 - YUV422
 *                                               1 - YUV420
 *                                               2 - Black/White
 *                          bit[2]   JFIF compatible VLC data
 *                          bit[3]   0 - not exit this function untill decompress done. Blocking service.
 *                                   1 - hit and run. Non-blocking service.
 * @param[in] MCURstNo		MCU restart number
 * @retval none
 */
void sp1kHalJpegDecode( UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize,  UINT8 Option, UINT16 MCURstNo );

/**
 * @fn void sp1kHalMjpgClipAbort( void );
 * @brief	abort video clip action.
 * @retval none
 */
void sp1kHalMjpgClipAbort( void );

/**
 * @fn void sp1kHalJpegDecodeWait( void );
 * @brief	it waits jpeg to finish decode process
 * @retval none
 */
void sp1kHalJpegDecodeWait( void );

/**
 * @fn UINT8 sp1kHalCtrlScreen(UINT8 attrib)
 * @brief	Set attributes to control screen.
 * @param[in] attrib	0: Show background color, no image, no OSD.
 *\n                    1: Show images, no OSD.
 *\n                    2: Show OSD, no images.
 *\n                    3: Show images and OSD.
 * @retval error code -  always reurn success(0)
 */
UINT8 sp1kHalCtrlScreen(UINT8 attrib);

#endif //__SP1K_HAL_API_H__

