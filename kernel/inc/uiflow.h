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

#ifndef __UIFLOW_H__
#define __UIFLOW_H__

//USB mode
#define K_UISTATUS_USBMODE		     0x2000
#define K_UISTATUS_USBMODE_PCCAM	 0x2100
#define K_UISTATUS_USBMODE_MASS	     0x2200

#define K_UIOPTION_STORAGE_NONE      0x00
#define K_UIOPTION_STORAGE_MSDC      0x01
#define K_UIOPTION_STORAGE_SIDC      0x02

//USB connect
#define K_UI_USB_DISCONNECT          0x00
#define K_UI_USB_CONNECT             0x01

extern xdata UINT8  G_UI_USBConnect;
extern xdata UINT16 G_UIStatus;
extern UINT8 xdata gUsbMode;

void UI_UsbModeSet(UINT32 mode);

#endif /*__UIFLOW_H__*/
