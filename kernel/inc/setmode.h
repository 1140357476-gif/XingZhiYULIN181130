
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
#ifndef __SETMODE_H__
#define __SETMODE_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//Camera mode
#define K_MODE_IDLE                     0x00
#define K_MODE_PREVIEW                  0x01
#define K_MODE_DSC                      0x02
#define K_MODE_VIDEOCLIP                0x03
#define K_MODE_PCCAM                    0x04
#define K_MODE_PLAYBACK                 0x05
#define K_MODE_UPDOWNLOAD               0x06

//DMA source/destination
#define K_DMA_DRAM                      0x00
#define K_DMA_SRAM                      0x01
#define K_DMA_FLASH                     0x02
#define K_DMA_AUDIO                     0x03
#define K_DMA_USB                       0x04
#define K_DMA_CPU                       0x05

//DMA source/destination for USB
#define K_DMA_UsbDontCare               0x00
#define K_DMA_UsbBulkOut                0x00
#define K_DMA_UsbBulkOut2               0x01
#define K_DMA_UsbBulkIn                 0x00
#define K_DMA_UsbIntIn                  0x01
#define K_DMA_UsbBulkIn2                0x02
#define K_DMA_UsbIntIn2                 0x03

//DMA return mode
#define K_DMA_SyncRet                   0x00
#define K_DMA_AsyncRet                  0x01

//DMA padding 0's
#define K_DMA_NoPadding0s               0x00
#define K_DMA_Padding0s                 0x01

//Limit of timeout count 
#define K_TimeoutLimit                  0x10000000

//=================================================================madc in bulk set 1 and ap in bulk set2
#if (0)
//richie@bu0426
//XBYTE[0x2546]
#define K_USB_CLASS_IN_OPEN		0x01
#define K_USB_CLASS_OUT_OPEN		0x02
#define K_USB_CLASS_IN_BUF_SIZE		0x25b3
#define K_USB_CLASS_OUT_BUF_SIZE	0x25b4
//XBYTE[0x25c2]
#define K_USB_CLASS_IN_ACK_MASK		0x01
#define K_USB_CLASS_IN_ACK_RESET	0xfe
#define K_USB_CLASS_OUT_ACK_MASK	0x02
#define K_USB_CLASS_OUT_ACK_RESET	0xfd
//XBYTE[0x25e8]
#define K_USB_CLASS_IN_STALL		0x04
//patch4.5@richie@cardlock
#define K_USB_CLASS_OUT_STALL		0x08

#define K_USB_CLASS_IN_DATA		0x2506
#define K_USB_CLASS_OUT_DATA		0x2507

#define K_USB_CLASS_DMA_IN		K_DMA_UsbBulkIn
#define K_USB_CLASS_DMA_OUT		K_DMA_UsbBulkOut

//XBYTE[0x2546]
#define K_USB_AP_IN_OPEN		0x08
#define K_USB_AP_OUT_OPEN		0x10
#define K_USB_AP_IN_BUF_SIZE		0x25b6
#define K_USB_AP_OUT_BUF_SIZE		0x25b7
//XBYTE[0x25c2]
#define K_USB_AP_IN_ACK_MASK		0x08
#define K_USB_AP_IN_ACK_RESET		0xf7
#define K_USB_AP_OUT_ACK_MASK		0x10
#define K_USB_AP_OUT_ACK_RESET		0xef

#define K_USB_AP_IN_DATA		0x2509
#define K_USB_AP_OUT_DATA		0x250a

#define K_USB_AP_DMA_IN			K_DMA_UsbBulkIn2
#define K_USB_AP_DMA_OUT		K_DMA_UsbBulkOut2

//=================================================================madc in bulk set 1 and ap in bulk set2
#else
//richie@bu0426
//XBYTE[0x2546]
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

#endif

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void SETMODE_ChangeCameraMode(UINT8 mode);
void SETMODE_WaitDmaCompletion(void);

#endif /*__SETMODE_H__*/
