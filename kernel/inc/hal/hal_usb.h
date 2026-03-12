/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      hal_usb.h
 * @brief     Brief
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_USB_H__
#define __HAL_USB_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
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
UINT8 HAL_UsbBulkInFromDRAM(UINT32 DRAMAddrWord, UINT16 nByte);
UINT8 HAL_UsbBulkOutToDRAM(UINT32 DRAMAddrWord, UINT16 nByte);
UINT8 HAL_UsbBulkInFromSRAM(UINT32 SRAMAddrByte, UINT16 nByte);
UINT8 HAL_UsbBulkOutToSRAM(UINT32 SRAMAddrByte, UINT16 nByte);
UINT8 HAL_UsbBulkIn(UINT32 *bulksize);
UINT8 HAL_UsbBulkOut(UINT32 *bulksize);
void HAL_UsbSwPlugOut(void);
void HAL_UsbSwPlugOut_Intr(void);
void HAL_UsbSwPlugIn(void);
void HAL_UsbPccamInit(UINT8 FormatIndex);
UINT8 HAL_UsbAutoCompress(UINT32 VLCAAddrWord, UINT32 VLCBAddrWord, UINT8 Option);
UINT8 HAL_UsbBulkEnable(UINT8 BulkDir);
void HAL_UsbForceFS(UINT8 usbconfigflag);
UINT8 HAL_UsbParaSet(UINT8 selector, UINT8 value);
UINT8 HAL_UsbBulkFlashAddrSet(UINT32 flashtype, UINT32 flashaddr);
UINT32 HAL_UsbBulkFlashAddrGet(UINT32 flashtype);
void HAL_UsbBulkAutoModeSet(UINT8 mode);
void HAL_UsbEP7IntrDataTx(UINT8 *pdataAddr,UINT16 dataSize);

#endif  /* __HAL_USB_H__ */
