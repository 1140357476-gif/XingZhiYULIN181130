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

#ifndef __INFODATA_H__
#define __INFODATA_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//---- Get imformation Size     -------
#define K_ProductDescriptorSize         ((UINT16) 0x0040)
#define K_SensorSizeInformation         ((UINT16) 0x002c)
#define K_DscSizeMapping                ((UINT16) 0x0040)
#define K_ClipSizeMapping               ((UINT16) 0x0040)
#define K_PcCameraSizeMapping1          ((UINT16) 0x0040)
#define K_PcCameraSizeMapping2          ((UINT16) 0x0040)

code UINT8 ProductDescriptor1[];

//--------------------------------------------------------------------------
//for PC AP Size mapping
//include
//      set sensor width ,height
//
//--------------------------------------------------------------------------
code UINT8 SensorSizeInformation[];

//--------------------------------------------------------------------------
//for Firmware Size mapping
//include
//      set sensor width ,height ,Hsubsample ,Vsubsample
//
//--------------------------------------------------------------------------
//DscSizeMapping
code UINT8 DscSizeMapping[];

//ClipSizeMapping
code UINT8 ClipSizeMapping[];

//PcCameraSizeMapping
code UINT8 PcCameraSizeMapping1[];

//PcCameraSizeMapping
code UINT8 PcCameraSizeMapping2[];

#endif /*__INFODATA_H__*/

