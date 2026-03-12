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

#ifndef __DPF_SCAN_H__
#define __DPF_SCAN_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#if 0
#define DPF_SCANNER
#endif

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
#ifdef DPF_SCANNER
extern xdata UINT32 snapMainVlcSize;
extern xdata UINT32 snapHdrSize;
extern xdata UINT32 snapJpegVlcAddr;
extern xdata UINT32 snapHdrSize;
extern UINT32 RemainPictureNO;
#endif

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

#endif /*__DPF_SCAN_H__*/