/****************************************************************************
 *                                                                          *
 *         Copyright (c) 2010 by Sunplus-prof Technology Co., Ltd.          *
 *                                                                          *
 *  This software is copyrighted by and is the property of Sunplus-prof     *
 *  Technology Co., Ltd. All rights are reserved by Sunplus-prof Technology *
 *  Co., Ltd. This software may only be used in accordance with the         *
 *  corresponding license agreement. Any unauthorized use, duplication,     *
 *  distribution, or disclosure of this software is expressly forbidden.    *
 *                                                                          *
 *  This Copyright notice MUST not be removed or modified without prior     *
 *  written consent of Sunplus-prof Technology Co., Ltd.                    *
 *                                                                          *
 *  Sunplus-prof Technology Co., Ltd. reserves the right to modify this     *
 *  software without notice.                                                *
 *                                                                          *
 *  Sunplus-prof Technology Co., Ltd.                                       *
 *  2nd Floor,R2-B,Hi-tech.S,7th Road, Shenzhen High-tech Industrial Park   *
 *  Shenzhen, China                                                         *
 *                                                                          *
 *  Author: Joe Zhao                                                        *
 *                                                                          *
 ****************************************************************************/
/* Sunplus-prof   Joe Zhao   March,04,2026  14:49:07 For SPCA1628A */
#ifndef _MENU_API_H_
#define _MENU_API_H_

#include "general.h"

typedef UINT8 (code *menufunc) (UINT8);
typedef UINT8 (code *menuexe) (UINT8, UINT8, UINT16);

UINT8 menuModeStill (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuImageSizeInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuImagesizeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuFDMeteringSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSharpnessSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSaturationSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuContrastSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuWBModeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuAEModeInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuAEModeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSpeedyCapSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSpeedyCapRecSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSnapEffectSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSceneModeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 mneuPhotoFrameSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLastItemProcess (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuIsoSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModeVideo (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuMovieSizeInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuMovieSizeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuFrameRateSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModePlayback (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuFirstItemProcess (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLockFile (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLockOne (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLockAll (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuUnlockAll (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDeleteInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDeleteFile (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDeleteOne (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDeleteAll (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSlideShowInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSlideShowSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuImageEffectSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDistortionSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuRotateSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuResizeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuVoiceMemoSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuStartImageSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDateStampSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuAntishakeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuFDModeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSmileModeSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModeSetup (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuRTCInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuRTCSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLanguageInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuLanguageSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuSpeakSoundSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuFormatSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuResetSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuVersionProcess (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuBlinkSave (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuJpgCrop (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuRedEyeSave (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModeDPS (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDpsImageChoose (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDpsCopyInit (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDpsCpoySet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuDPSResetSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModeCalibration (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalUsbPowerSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_OBSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_LensShadingSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_GammaSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_AESet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_AEGidSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalCDSP_AWBSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalSnapTest (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalPvTest (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAETest (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalOB (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAutoOB (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalPvOB (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAWBTest (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalBpPro (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAFTurn (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalFwUpgrade (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalLsc (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAECal (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalWdtSet (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalShowColorPalette (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuStorageSpeedTest (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuCalAwbCtt (UINT8 layer, UINT8 item, UINT16 msg);
UINT8 menuModeUsbSet (UINT8 layer, UINT8 item, UINT16 msg);

UINT8 menuImageSizeActive (UINT8 item);
UINT8 menuSizeIsActive (UINT8 item);
UINT8 menuFDIsActive (UINT8 item);
UINT8 menuDriverModeActive (UINT8 item);
UINT8 menuBurstIsActive (UINT8 item);
UINT8 menuSpeedyCapActive (UINT8 item);
UINT8 menuSpeedyCapRecActive (UINT8 item);
UINT8 menuPvEffectActive (UINT8 item);
UINT8 menuPhotoframeIsActive (UINT8 item);
UINT8 menuVideoSizeIsActive (UINT8 item);
UINT8 menuFrameIsActive (UINT8 item);
UINT8 menuProtectIsActive (UINT8 item);
UINT8 menuDeleteIsActive (UINT8 item);
UINT8 menuSlideShowIsActive (UINT8 item);
UINT8 menuSlideStartIsActive (UINT8 item);
UINT8 menuEffectIsActive (UINT8 item);
UINT8 menuRotateIsActive (UINT8 item);
UINT8 menuResizeIsActive (UINT8 item);
UINT8 menuResizeSizeIsActive (UINT8 item);
UINT8 menuVoiceMemoIsActive (UINT8 item);
UINT8 menuVoiceMemoAddIsActive (UINT8 item);
UINT8 menuVoiceMemoDeleteIsActive (UINT8 item);
UINT8 menuStartupIsActive (UINT8 item);
UINT8 menuDpsQualityIsActive (UINT8 item);
UINT8 menuDpsLayoutIsActive (UINT8 item);
UINT8 menuDpsPaperSizeIsActive (UINT8 item);
UINT8 menuDpsTypeIsActive (UINT8 item);
UINT8 menuDpsDateIsActive (UINT8 item);
UINT8 menuDpsNameIsActive (UINT8 item);
UINT8 menuUSBPowerIsActive (UINT8 item);

#define StrResMax 0x17e1


#endif /* _MENU_API_H_ */

