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

#ifndef __USBSIDC_H__
#define __USBSIDC_H__
#include "sidcinit.h"
#include "common.h"
#include "dps_api.h"
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//Class Request
#define K_USBSIDC_CANCEL_REQUEST                0x64
#define K_USBSIDC_GET_EXTENDED_EVENT_DATA       0x65
#define K_USBSIDC_DEVICE_RESET_REQUEST          0x66
#define K_USBSIDC_GET_DEVICE_STATUS             0x67
//Container Type
#define K_USBSIDC_Undefined                     0x0000         
#define K_USBSIDC_CommandBlock                  0x0100         
#define K_USBSIDC_DataBlock                     0x0200         
#define K_USBSIDC_ResponseBlock                 0x0300
#define K_USBSIDC_EventBlock                    0x0400





//------------------------------------------------------------------------------
//Type Definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
extern  PUSBSIDC_CONTAINER      G_USBSIDC_pCommand;

//extern  data    BIT     G_USBSIDC_btFirstOutPacket;
//extern  xdata   UINT32   G_USBSIDC_ulLogicalBlockAddress;
extern  xdata   UINT16  G_USBSIDC_usSizeProcessed;
extern  xdata   UINT16  G_USBSIDC_usDeviceStatus;
extern  xdata   UINT16  G_USBSIDC_usResponseCode;
extern  xdata   UINT8   G_USBSIDC_ucResponseParamCount;
extern  xdata   UINT32   G_USBSIDC_ulResponseParam[];

//extern  xdata   UINT32   G_USBSIDC_ulSdramBufferAddress;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void USBSIDC_NoDataPhase(void);
void USBSIDC_PreInDataPhase(void);
void USBSIDC_PreOutDataPhase(void);
void USBSIDC_PostInDataPhase(void);
void USBSIDC_PostOutDataPhase(void);
void USBSIDC_BulkInData(void);
void USBSIDC_BulkOutData(void);
void USBSIDC_ClearCommandBuffer(void);
UINT8 USBSIDC_Command(UINT8 len);
UINT8 USBSIDC_Status(void);
void USBSIDC_SetBulkInPacket(UINT8 len);
UINT16 USBSIDC_GetBulkOutPacket(UINT8 xdata* pData);
UINT8 USBSIDC_GetBulkOutPacketToBlock(void);
UINT8 USBSIDC_PostBulkOutDataPhase(void);
void USBSIDC_ParseContainer(void);
//void USBSIDC_PreInBlock(void);
//void USBSIDC_PostInBlock(void);
void USBSIDC_PreOutBlock(void);
void USBSIDC_PostOutBlock(void);
void USBSIDC_StallBulkInOutPacket(void);
void USBSIDC_ClearStallBulkInOutPacket(void);

//richie
//UINT8 USBSIDC_HAL_SetUSBDMA(UINT8 USBDMASrc, UINT8 USBDMADst);
//UINT8 USBSIDC_HAL_SetDRAMDMA(UINT32 DRAMAddr);
//UINT8 USBSIDC_HAL_DoDMA(UINT8 Src,UINT8 Dst, UINT16 nByte, UINT8 Mode, UINT8 FlashPageDMA);
//richie@pa0429
//UINT8 USBSIDC_HAL_ReadRTC(pDateStc_t DatePt);
//UINT8 USBSIDC_L3_ReadRTCData(UINT8 Addr, UINT8* DataPt);
//UINT8 USBSIDC_L3_BinToDate(pDateStc_t BinPt, pDateStc_t DatePt);
//UINT8 USBSIDC_HAL_WriteRTC(pDateStc_t DatePt);
//UINT8 USBSIDC_L3_DateToBin(pDateStc_t DatePt, pDateStc_t BinPt);
//UINT8 USBSIDC_L3_WriteRTCData(UINT8 Addr, UINT8 Data);
void SETMODE_WaitDmaCompletion(void);

#endif /*__USBSIDC_H__*/
