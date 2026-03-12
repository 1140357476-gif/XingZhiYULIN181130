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

#ifndef _usbopt_H_
#define _usbopt_H_

#include "knl_func_opt.h"
//------------------------------------------------------------------------------
//USB Mass Storage Device Class Option =
//                              0 Disable
//                              1 Control/Bulk/Interrupt (CBI) Transport Protocol
//                              2 Bulk-Only Transport Protocol
//------------------------------------------------------------------------------
#define USBMSDC_OPTION          2

#define USBSIDC_OPTION_OFF    		0x00
#define USBSIDC_OPTION_SIDC_ONLY   	\
	(FUNC_KNL_SIDC ? 0x01 : 0x00) //0x01
#define USBSIDC_OPTION_SIDC_DPS    	\
	((FUNC_KNL_SIDC && FUNC_KNL_DPS) ? 0x02 : USBSIDC_OPTION_SIDC_ONLY) //0x02

#if FUNC_KNL_DPS
#define USBSIDC_OPTION 2//USBSIDC_OPTION_OFF  //0: disable    1:sidc en    2:sidc&dps en
#else
#define USBSIDC_OPTION 0//USBSIDC_OPTION_OFF  //0: disable    1:sidc en    2:sidc&dps en
#endif
#define USBAUDC_OPTION 0


#define NAND_SUPPORT_ONLY	0


#define K_USB_CLASS_IN_OPEN		0x08
#define K_USB_CLASS_OUT_OPEN		0x10
#define K_USB_CLASS_IN_BUF_SIZE		0x2563		/* allen changed*/
#define K_USB_CLASS_OUT_BUF_SIZE	0x2565		/* allen changed*/
//XBYTE[0x2500]
#define K_USB_CLASS_IN_ACK_MASK		0x08
#define K_USB_CLASS_IN_ACK_RESET	0x08
#define K_USB_CLASS_IN_ACK_DISABLE	0xF7
#define K_USB_CLASS_OUT_ACK_MASK	0x10
#define K_USB_CLASS_OUT_ACK_RESET	0x10
#define K_USB_CLASS_OUT_ACK_DISABLE	0xEF
//XBYTE[0x25e8]/*allen think*/
#define K_USB_CLASS_IN_STALL		0x08		/* allen changed*/
//patch4.5@richie@cardlock
#define K_USB_CLASS_OUT_STALL		0x10		/* allen changed*/

#define K_USB_CLASS_IN_DATA		    0x2541
#define K_USB_CLASS_OUT_DATA		0x2542
/* allen think*/
#define K_USB_CLASS_DMA_IN		K_DMA_UsbBulkIn2
#define K_USB_CLASS_DMA_OUT		K_DMA_UsbBulkOut2

//XBYTE[0x2546]
#define K_USB_AP_IN_OPEN		0x08			/*allen changed*/
#define K_USB_AP_OUT_OPEN		0x10			/*allen changed*/
#define K_USB_AP_IN_BUF_SIZE		0x2563
#define K_USB_AP_OUT_BUF_SIZE_LSB		0x2565		/* allen changed */
#define K_USB_AP_OUT_BUF_SIZE_MSB		0x2566		/* allen changed */
//XBYTE[0x25c2]
#define K_USB_AP_IN_ACK_MASK		0x08
#define K_USB_AP_IN_ACK_RESET		0x08
#define K_USB_AP_OUT_ACK_MASK		0x10
#define K_USB_AP_OUT_ACK_RESET		0x10

#define K_USB_AP_IN_DATA		0x2541
#define K_USB_AP_OUT_DATA		0x2542

#define K_USB_AP_DMA_IN			K_DMA_UsbBulkIn
#define K_USB_AP_DMA_OUT		K_DMA_UsbBulkOut



#endif /*__usbopt_H__*/

