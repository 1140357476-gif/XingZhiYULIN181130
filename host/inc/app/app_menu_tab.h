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
#ifndef _MENU_TAB_H_
#define _MENU_TAB_H_

#include "general.h"
#include "app_menu_api.h"

#define	MENU_MARK_STILL_NOUSE	0x01000000
#define	MENU_MARK_VIDEO_NOUSE	0x02000000
#define	MENU_MARK_AUDIO	0x03000000
#define	MENU_MARK_PLAYBACK	0x04000000
#define	MENU_MARK_STILL	0x05000000
#define	MENU_MARK_VIDEO	0x06000000
#define	MENU_MARK_SETUP	0x07000000
#define	MENU_MARK_FORMAT_YES	0x08010101
#define	MENU_MARK_FORMAT_NO	0x08010102
#define	MENU_MARK_DELONE_YES	0x08010201
#define	MENU_MARK_DELONE_NO	0x08010202
#define	MENU_MARK_BLINK_SAVE	0x08010301
#define	MENU_MARK_BLINK_CANCEL	0x08010302
#define	MENU_MARK_JPG_CROP_YES	0x08010401
#define	MENU_MARK_JPG_CROP_NO	0x08010402
#define	MENU_MARK_REDEYE_WRITE	0x09010000
#define	MENU_MARK_REDEYE_SAVE	0x09020000
#define	MENU_MARK_REDEYE_CANCEL	0x09030000
#define	MENU_MARK_DPS	0x0a000000
#define	MENU_MARK_CALIBRATION	0x0b000000
#define	MENU_MARK_USB	0x0c000000

#define ID_STR_ENTER		0x0000
#define ID_STR_BACK		0x0001
#define ID_STR_DELETE_THIS_FILE_		0x0002
#define ID_STR_DELETE_ALL_FILE_		0x0003
#define ID_STR_FORMATTING___		0x0004
#define ID_STR_HAS_NO_SIZE_TO_RISIZE_		0x0005
#define ID_STR_FILE_LOCKED_		0x0006
#define ID_STR_PROTECT_FILE_		0x0007
#define ID_STR_UNPROTECT_FILE_		0x0008
#define ID_STR_PROTECT_ALL_		0x0009
#define ID_STR_UNPROTECT_ALL_		0x000a
#define ID_STR_NO_FILE		0x000b
#define ID_STR_NO_CARD_		0x000c
#define ID_STR_REMOVE_CARD_		0x000d
#define ID_STR_CARD_ERROR		0x000e
#define ID_STR_WAITING___		0x000f
#define ID_STR_CARD_LOCKED_		0x0010
#define ID_STR_PAUSE		0x0011
#define ID_STR_REC		0x0012
#define ID_STR_FILE_ERROR_		0x0013
#define ID_STR_PLEASE_PLUG_OUT_CARD		0x0014
#define ID_STR_MEMORY_FULL		0x0015
#define ID_STR_WAITING_FOR_PRINTER		0x0016
#define ID_STR_NO_JPEG		0x0017
#define ID_STR_FORMAT_FAIL_		0x0018
#define ID_STR_LOW_BATTERY		0x0019
#define ID_STR_NOT_SUPPORT_		0x001a
#define ID_STR_CANCEL		0x001b
#define ID_STR_SELECT_UP_TO_24		0x001c
#define ID_STR_PLEASE_SELECT_A_PICTURE		0x001d
#define ID_STR_PRINT_ERROR		0x001e
#define ID_STR_NOT_FIND_PRINTER		0x001f
#define ID_STR_CROP_THIS_PHOTO_		0x0020
#define ID_STR_UP_NO		0x0021
#define ID_STR_UP_YES		0x0022
#define ID_STR_DOWN_NO		0x0023
#define ID_STR_DOWN_YES		0x0024
#define ID_STR_OK_NO		0x0025
#define ID_STR_OK_YES		0x0026
#define ID_STR_PWR_NO		0x0027
#define ID_STR_PWR_YES		0x0028
#define ID_STR_USB_NO		0x0029
#define ID_STR_USB_YES		0x002a
#define ID_STR_PB_NO		0x002b
#define ID_STR_PB_YES		0x002c
#define ID_STR_SET_NO		0x002d
#define ID_STR_SET_YES		0x002e
#define ID_STR_ADJUST_MACRO_		0x002f
#define ID_STR_SOFTWARE_UPDATE___		0x0030
#define ID_STR_UPDATE_COMPLETED_		0x0031
#define ID_STR_UPDATE_FAILED_		0x0032
#define ID_STR_DC403S_5647_9851_221220		0x0033

extern UINT8 code* code* code CustomMenuPtr[];
extern UINT8 code* code* code CustomStrPtr[];
extern UINT8 code* code CustomIconPtr[];
extern UINT32 code* code CustomMenuRootStart[];
extern UINT32 code* code CustomMenuIndex[];
extern UINT8 code* code CustomMenuNodeItem[];
extern UINT8 code* code CustomMenuSubItem[];
extern menufunc code* code CustomMenuFunc[];
extern menuexe code* code CustomMenuExe[];
extern UINT8 code* code CustomMenuParaTab[];

#endif /* _MENU_TAB_H_ */
