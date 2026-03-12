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

#ifndef __EXIFVAR_H__
#define __EXIFVAR_H__
//------------------------------------------------------------------------------
//Variable Definition
//------------------------------------------------------------------------------
extern  xdata   UINT16 G_ucThumbnailWidth;    
extern  xdata   UINT16 G_ucThumbnailHeight;   
extern  xdata   UINT8 Text_NowDateTime[20];  

//patch3.2@ada@0401 For EXIF
extern  xdata   UINT16 ByteOrder;
extern  xdata   UINT32 ByteOrderOffset;
extern  xdata   UINT32 IFD_ThumbnaiFrameOffset;
extern  xdata   UINT32 IFD_ThumbnaiFrameLength;
extern  xdata   UINT16 IFD_Number;
extern  xdata   UINT16 Tiff_ID;
extern  xdata   UINT32 Tiff_IFDOffset;
extern  xdata   UINT32 NextIFDOffset;
extern  xdata   UINT32 IFD_DateTime_Offset;
extern  xdata   UINT8 Jpeg_Exif_Format;
extern  xdata   UINT32 ExifTmpDramAddr;
extern  xdata   UINT8 Extract_Finished;
extern  xdata   UINT8 Extract_Thumbnail;
extern  xdata   UINT32 HeaderLength;
extern  xdata   UINT8 Parsing_Marker;
extern  xdata   UINT16 APP0Length;
extern  xdata   UINT16 APP1Length;
extern  xdata   UINT16 APPnLength;
extern  xdata   UINT16 TotalAPPnLength;
extern  xdata   UINT16 ThumbAPPnLength;
extern  xdata   UINT16 TotalThumbAPPnLength;
extern  xdata   UINT16 COMLength;
extern  xdata   UINT16 DQTLength;
extern  xdata   UINT16 DHTLength;
extern  xdata   UINT16 SOFLength;
extern  xdata   UINT16 SOSLength;
extern  xdata   UINT16 DRILength;
extern  xdata   UINT8  NeedReadMoreHeader;

#endif /*__EXIFVAR_H__*/
