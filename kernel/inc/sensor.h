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

#ifndef __SENSOR_H__
#define __SENSOR_H__

#if 0  // mattwang remove.
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//-------------------------------------
//Embedded signal generator
//-------------------------------------
//3.0M
//patch5.2.1@ada@Enlarge no sensor buffer begin
#ifdef NOSENSOR
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)2048)
#define K_SENSOR_MonitorHeight          ((UINT16)252)
#define K_AviSkipLine                   ((UINT16)12)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//1.0M
//#ifdef NOSENSOR
//#define K_SENSOR_FrameRate              30
//#define K_SENSOR_CaptureWidth           ((UINT32)1152)
//#define K_SENSOR_CaptureHeight          ((UINT32)864)
//#define K_SENSOR_MonitorWidth           ((UINT16)576)
//#define K_SENSOR_MonitorHeight          ((UINT16)433)
//#define K_AviSkipLine                   ((UINT16)73)
//patch5.1@richie@interpolation begin
//#define K_SENSOR_IntrWidth	((UINT32)1600)
//#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
//#endif
//patch5.2.1@ada@Enlarge no sensor buffer end

//-------------------------------------
//CMOS sensor
//-------------------------------------
//TASC 5130 VGA CMOS
#ifdef TASC5130
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)
#define K_AviSkipLine                   ((UINT16)133)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//TASC 5160 SXVGA CMOS
#ifdef TASC5160
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)960)
#define K_SENSOR_MonitorWidth           ((UINT16)656)//640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)
#define K_AviSkipLine                   ((UINT16)48)////132)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//IC Media 105 VGA CMOS
#ifdef ICM105
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)
#define K_AviSkipLine                   ((UINT16)133)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//IC Media 107 1.0M CMOS
#ifdef ICM107B
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)1152)
#define K_SENSOR_CaptureHeight          ((UINT32)864)
#define K_SENSOR_MonitorWidth           ((UINT16)576)
#define K_SENSOR_MonitorHeight          ((UINT16)433)
#define K_AviSkipLine                   ((UINT16)73)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//IC Media 108 1.3M CMOS
#ifdef ICM108
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth          	((UINT32)1280)// ((UINT32)1152)
#define K_SENSOR_CaptureHeight         ((UINT32)1024)//((UINT32)864)
#define K_SENSOR_MonitorWidth           ((UINT16)656)//((UINT16)576)
#define K_SENSOR_MonitorHeight          ((UINT16)493)//((UINT16)433)
#define K_AviSkipLine                   ((UINT16)48)//((UINT16)73)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

#ifdef ICM109
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth          	((UINT32)1600)// ((UINT32)1152)
#define K_SENSOR_CaptureHeight         ((UINT32)1200)//((UINT32)864)
#define K_SENSOR_MonitorWidth           ((UINT16)800)//((UINT16)576)

//patch5.1@cch@badpixel begin
#define K_SENSOR_MonitorHeight          ((UINT16)613)//((UINT16)433)
#define K_AviSkipLine                   ((UINT16)13)//((UINT16)73)
//#define K_SENSOR_MonitorHeight          ((UINT16)600)//((UINT16)433)
//#define K_AviSkipLine                   ((UINT16)48)//((UINT16)73)
//patch5.1@cch@badpixel end
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

#ifdef ICM110		//patch5.2.1@jhyu@ICM110
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)1024)//512
#define K_SENSOR_MonitorHeight          ((UINT16)781)//384+13
#define K_AviSkipLine                   ((UINT16)73)
#endif

// patch 5.2.1_26@mattwang@add MI360 beg
#ifdef MI360
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)656)
// patch 5.2.1_31@mattwang@modify preview mode of MI360 beg
//#define K_SENSOR_MonitorHeight          ((UINT16)493)  // 20040331 mattwang mark
#define K_SENSOR_MonitorHeight          ((UINT16)253)  // 20040331 mattwang modify
// patch 5.2.1_31@mattwang@modify preview mode of MI360 end
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_26@mattwang@add MI360 end

// patch 5.2.1_24@mattwang@MI1300 beg
#ifdef MI1300
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)//613
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_24@mattwang@MI1300 end

// patch 5.2.1_36@mattwang@add MI1310 CMOS sensor beg
#ifdef MI1310
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)//613
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_36@mattwang@add MI1310 CMOS sensor end

// patch 5.2.1_23@mattwang@MI2000 beg
#ifdef MI2000
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
#define K_SENSOR_MonitorWidth           ((UINT16)816)
#define K_SENSOR_MonitorHeight          ((UINT16)597)//613
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_23@mattwang@MI2000 end

// patch 5.2.1_27@mattwang@add MI3100 beg
#ifdef MI3100
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)668)
#define K_SENSOR_MonitorHeight          ((UINT16)510)//613
#define K_AviSkipLine                   ((UINT16)13)
#endif                                             //hua@031229 add micro sensor
// patch 5.2.1_27@mattwang@add MI3100 end

//OmniVision 9620 1.3M CMOS
#ifdef OV13
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
// patch 5.2.1_35@mattwang@modify OV13 coverage rate beg
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)495) //493)  // 20040805 mattwang modify
// patch 5.2.1_35@mattwang@modify OV13 coverage rate end
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

#ifdef OV2610
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
//patch4.5@richie@ov monitor size adj begin
#define K_SENSOR_MonitorWidth           ((UINT16)816)
#define K_SENSOR_MonitorHeight          ((UINT16)613)
//patch4.5@richie@ov monitor size adj end
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

#ifdef OV3610
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)1024)
#define K_SENSOR_MonitorHeight          ((UINT16)768)	//patch5.2.1@jhyu@OV3610
#define K_AviSkipLine                   ((UINT16)13)
#endif

// patch 5.2.1_34@mattwang@add OV3620 CMOS sensor beg
#ifdef OV3620
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)1024)
#define K_SENSOR_MonitorHeight          ((UINT16)768)
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_34@mattwang@add OV3620 CMOS sensor end

// patch 5.2.1_37@mattwang@add OV5610 CMOS sensor beg
#ifdef OV5610
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2592)
#define K_SENSOR_CaptureHeight          ((UINT32)1944)
#define K_SENSOR_MonitorWidth           ((UINT16)1280)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_GC0316)
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_MI1320)
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_PA6371)
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_MI5100)
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)2592)
#define K_SENSOR_CaptureHeight          ((UINT32)1944)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_SOI768)
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_OV3630)
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)1024)
#define K_SENSOR_MonitorHeight          ((UINT16)768)
#define K_AviSkipLine                   ((UINT16)13)
#endif
#if (SENSOR_TYPE == Sensor_GC1303)
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
#define K_SENSOR_MonitorWidth          ((UINT16)640)
#define K_SENSOR_MonitorHeight        ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif

#if (SENSOR_TYPE == Sensor_OV5620)
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2592)
#define K_SENSOR_CaptureHeight          ((UINT32)1944)
#define K_SENSOR_MonitorWidth           ((UINT16)1280)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
#define K_AviSkipLine                   ((UINT16)13)
#endif
// patch 5.2.1_37@mattwang@add OV5610 CMOS sensor end

// patch 5.2.1_38@mattwang@add SOI268 CMOS sensor beg
#ifdef SOI268
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
//patch4.5@richie@ov monitor size adj begin
#define K_SENSOR_MonitorWidth          ((UINT16)816)
#define K_SENSOR_MonitorHeight        ((UINT16)613)
//patch4.5@richie@ov monitor size adj end
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif
// patch 5.2.1_38@mattwang@add SOI268 CMOS sensor end

//Motorola 20027 1.3M CMOS
#ifdef MCM20027
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)1024)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

#ifdef HP2020
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)656)//640)
#define K_SENSOR_MonitorHeight          ((UINT16)493)
#define K_AviSkipLine                   ((UINT16)48)////132)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//-------------------------------------
//CCD sensor
//-------------------------------------
//Sony 232 1.3M Interlace CCD
#ifdef SONY232
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)960)
#define K_SENSOR_MonitorWidth           ((UINT16)1280)
#define K_SENSOR_MonitorHeight          ((UINT16)246)
#define K_AviSkipLine                   ((UINT16)12)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

//Sony 442 1.3M Progressive CCD
#ifdef SONY442
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)1280)
#define K_SENSOR_CaptureHeight          ((UINT32)960)
#define K_SENSOR_MonitorWidth           ((UINT16)1280)
#define K_SENSOR_MonitorHeight          ((UINT16)246)
#define K_AviSkipLine                   ((UINT16)12)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#endif

// SONY 3.0M 3 field interlace CCD Sensor
#ifdef SONY432
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)2048)
#define K_SENSOR_MonitorHeight          ((UINT16)252)
#define K_AviSkipLine                   ((UINT16)12)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

// SONY 5.0M 3 field interlace CCD Sensor
#ifdef SONY452
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)2592)
#define K_SENSOR_CaptureHeight          ((UINT32)1944)
#define K_SENSOR_MonitorWidth           ((UINT16)2592)
#define K_SENSOR_MonitorHeight          ((UINT16)216)
#define K_AviSkipLine                   ((UINT16)18)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2592)
#define K_SENSOR_IntrHeight	((UINT32)1944)
//patch5.1@richie@interpolation end
#endif

// Panasonic 3.0M 3 field interlace CCD Sensor
#ifdef MN39480
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)2048)
#define K_SENSOR_MonitorHeight          ((UINT16)256)
#define K_AviSkipLine                   ((UINT16)12)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

// Panasonic 4.0M 3 field interlace CCD Sensor
#ifdef MN39482
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)2304)
#define K_SENSOR_CaptureHeight          ((UINT32)1728)
#define K_SENSOR_MonitorWidth           ((UINT16)2304)
#define K_SENSOR_MonitorHeight          ((UINT16)252)
#define K_AviSkipLine                   ((UINT16)12)
#define K_SENSOR_IntrWidth	            ((UINT32)2304)
#define K_SENSOR_IntrHeight	            ((UINT32)1728)
#endif

//Sharp RJ23N3 2.0M Interlace CCD
#ifdef RJ23N3
#define K_SENSOR_FrameRate              10
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
#define K_SENSOR_MonitorWidth           ((UINT16)1600)
#define K_SENSOR_MonitorHeight          ((UINT16)253)
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//Panasonic 39472 2.0M Interlace CCD
#ifdef MN39472
#define K_SENSOR_FrameRate              10
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
#define K_SENSOR_MonitorWidth           ((UINT16)1600)
#define K_SENSOR_MonitorHeight          ((UINT16)304)
#define K_AviSkipLine                   ((UINT16)64)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//patch4.3@richie@mn0604
//Panasonic 39470 2.0M Interlace CCD
#ifdef MN39470
#define K_SENSOR_FrameRate              10
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
#define K_SENSOR_MonitorWidth           ((UINT16)1600)
#define K_SENSOR_MonitorHeight          ((UINT16)304)
#define K_AviSkipLine                   ((UINT16)64)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//Sony 224/284/434 2.0M Interlace CCD
#ifdef SONY224
#define K_SENSOR_FrameRate              15
#define K_SENSOR_CaptureWidth           ((UINT32)1600)
#define K_SENSOR_CaptureHeight          ((UINT32)1200)
#define K_SENSOR_MonitorWidth           ((UINT16)1600)
#define K_SENSOR_MonitorHeight          ((UINT16)304)
#define K_AviSkipLine                   ((UINT16)14)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//3.0M
//SONY 252/262 3.0M Interlace CCD
#ifdef SONY262
#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)2048)
#define K_SENSOR_MonitorHeight          ((UINT16)253)
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif
//patch4.4@andrew@SHARP_LZ24

#ifdef SHARP_LZ24BP
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)640)
#define K_SENSOR_CaptureHeight          ((UINT32)480)
#define K_SENSOR_MonitorWidth           ((UINT16)640)
#define K_SENSOR_MonitorHeight          ((UINT16)480)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)1600)
#define K_SENSOR_IntrHeight	((UINT32)1200)
//patch5.1@richie@interpolation end
#define K_AviSkipLine                   ((UINT16)13)

#endif

#ifdef MN39592

#define K_SENSOR_FrameRate              30
#define K_SENSOR_CaptureWidth           ((UINT32)2048)
#define K_SENSOR_CaptureHeight          ((UINT32)1536)
#define K_SENSOR_MonitorWidth           ((UINT16)2048)
#define K_SENSOR_MonitorHeight          ((UINT16)257)
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif

//patch5.1@andrew@sony406 begin
#ifdef SONY406
#define K_SENSOR_FrameRate              20
#define K_SENSOR_CaptureWidth           ((UINT32)2308)
#define K_SENSOR_CaptureHeight          ((UINT32)1712)
#define K_SENSOR_MonitorWidth           ((UINT16)2308)
#define K_SENSOR_MonitorHeight          ((UINT16)215)
#define K_AviSkipLine                   ((UINT16)13)
//patch5.1@richie@interpolation begin
#define K_SENSOR_IntrWidth	((UINT32)2048)
#define K_SENSOR_IntrHeight	((UINT32)1536)
//patch5.1@richie@interpolation end
#endif
//patch5.1@andrew@sony406 end

//Image capture  resolution
#define K_SNAP_Resolution_Middle_Width  ((UINT16)640)
#define K_SNAP_Resolution_Middle_Height ((UINT16)480)
#define K_SNAP_Resolution_Low_Width     ((UINT16)320)
#define K_SNAP_Resolution_Low_Height    ((UINT16)240)

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
extern  xdata   UINT8    G_Interlace_Enable;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void SENSOR_Initialize(void);
void SENSOR_CaptureAeAwbAf(void);
void SENSOR_OrdinaryAeAwbAf(void);
void SENSOR_GetColorInformation(UINT16 colorIndex);
void SENSOR_SetColorInformation(UINT16 colorIndex);

#endif

#endif /*__SENSOR_H__*/

