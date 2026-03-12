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

#ifndef __INITIO_H__
#define __INITIO_H__

#include "general.h"
#include "icat_stub.h"
#include "sys_mem_init.h"
#include "osd_mem.h"
#include "motion_det.h"
#include "dist_func.h"
//#include "video_mem.h"
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
#define  SUPPORT_720p
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */

#define SUPPORT_QVGA_SENSOR 0
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define CodeInDRAM_Option	1

#if (MOTION_DET_EN) || defined(DIST_FUNC_EN)
	#define FRAME_BUF_NUM	4
#else
	#define FRAME_BUF_NUM	2
#endif

//#if (MOTION_DET_EN)
//#define VIDEO_MD_REC_FRAME_BUF_NUM_CTRL_EN		0
//#else
//#define VIDEO_MD_REC_FRAME_BUF_NUM_CTRL_EN		0
//#endif

//#if (VIDEO_MD_REC_FRAME_BUF_NUM_CTRL_EN)
//#define VIDEO_MD_PREVIEW_FRAME_BUF_NUM 			4
//#define VIDEO_MD_REC_FRAME_BUF_NUM 				2
//#endif


#define Sound_Ctrl_Option	0
//***************************************************************************
//All SDRAM Constant Are Defined in Unit of WORD
//***************************************************************************

//Number of Character font
#define K_OSDFONT_NumChar              ((UINT32)256)
#define K_OSDICON_NumChar              ((UINT32)512)

//Resolution of Graphic font
#define K_GRAFFONT_Width                ((UINT32)256)
#define K_GRAFFONT_Height               ((UINT32)256)
#if 1
//Resolution of Video clip
#ifdef SUPPORT_720p 
#define K_VIDEOCLIP_WIDTH_1080p               ((UINT32)1920)
#define K_VIDEOCLIP_HEIGHT_1080p              ((UINT32)1088) 
#define K_VIDEOCLIP_WIDTH               ((UINT32)1280)
#define K_VIDEOCLIP_HEIGHT              ((UINT32)720)
#define K_VIDEOCLIP_WIDTH_D1            ((UINT32)640) //((UINT32)848) //modify for D1 size
#define K_VIDEOCLIP_HEIGHT_D1           ((UINT32)432) //((UINT32)480)
#define K_VIDEOCLIP_WIDTH_VGA        	((UINT32)640)
#define K_VIDEOCLIP_HEIGHT_VGA        	((UINT32)480)
#else
#define K_VIDEOCLIP_WIDTH               ((UINT32)640)
#define K_VIDEOCLIP_HEIGHT              ((UINT32)480)
#define K_VIDEOCLIP_WIDTH_D1            ((UINT32)640)
#define K_VIDEOCLIP_HEIGHT_D1           ((UINT32)480)
#define K_VIDEOCLIP_WIDTH_VGA         	((UINT32)640)
#define K_VIDEOCLIP_HEIGHT_VGA        	((UINT32)480)
#endif

#endif

#if  SUPPORT_QVGA_SENSOR
#define K_Preview_Width                 (UINT16)320 //((UINT32)320) //((UINT32)720)
#define K_Preview_Height                (UINT16)240//480  //((UINT32)240) //((UINT32)480)
#else
#define K_Preview_Width                 (UINT16)640 //((UINT32)320) //((UINT32)720)
#define K_Preview_Height                (UINT16)480//480  //((UINT32)240) //((UINT32)480)
#endif

#define K_PREVIEW_BUF_WIDTH				(UINT32)640 
#define K_PREVIEW_BUF_HEIGHT			(UINT32)480 


//Size of total SDRAM = 16M word = 32M byte
//16M word = 0x00000000 - 0x00ffffff word

#define K_SDRAM_TotalSize               sysMemTotSizeGet()

/* Allen added for awbctt.bin buf address */
#ifdef LOAD_AWBCTT_BIN
#define K_SDRAM_AWBCTT_Buf_Address		((sysMemAddrGet(MEM_IDX_DATA) << 1) + (0x7400 - 0x4000)) /* mapping to xdata 0x7400 */
#endif

//If change the max num of bad pixel ,you must rebuild some library
/*
calibration.c 
bp_main.c 
sys_mem_init.c 
snap_core.c c
dsp_taskc. 
*/
#define BADPIXEL_CAP_NUM_MAX	((UINT32)8*1024)
#define BADPIXEL_PV_NUM_MAX		((UINT32)4*1024)

//***************************************************************************
//Buffer Size
//***************************************************************************
//*************************************
//Common
//*************************************
//code size
#if(CodeInDRAM_Option)
#define K_SDRAM_CodeSize 		sysMemSizeGet(MEM_IDX_TEXT) //refer to the definition "SYS_MEM_SIZE_CODE	"
#else
#define K_SDRAM_CodeSize 		((UINT32)0)
#endif

/*
	        Low Addr
	 -------------------------                         
    | 0x4000 ~ 0x7fff mapping |
     -------------------------
    | 0x8000 ~ 0xffff mapping |
     -------------------------
            High Addr
*/

#define K_SDRAM_ShadowSize		((UINT32)(8 + 16) * (UINT32)1024) //48KB

#if 1  //will
//Size of Bad pixel position buffer'
#define K_SDRAM_BadPixel_PrvBufSize		(BADPIXEL_PV_NUM_MAX*2)		//((UINT32)8 * (UINT32)1024) //16KB
//#define K_SDRAM_BadPixel_CapBufSize		((UINT32)16 * (UINT32)1024) //32KB
#define K_SDRAM_BadPixel_CapBufSize_Upper		BADPIXEL_CAP_NUM_MAX//((UINT32)8 * (UINT32)1024) //16KB
#define K_SDRAM_BadPixel_CapBufSize_Lower		BADPIXEL_CAP_NUM_MAX//((UINT32)8 * (UINT32)1024) //16KB

#define K_SDRAM_BadPixel_CapScaleBufSize	BADPIXEL_CAP_NUM_MAX//((UINT32)8 * (UINT32)1024) //16KB ((UINT32)32 * (UINT32)1024) //64KB
#define K_SDRAM_BadPixel_DummyBufSize		((UINT32)8)/*fan.xiao modify for RAW shift , it need 1 word*///((UINT32)6 * (UINT32)1024) //12KB reserved, compatible for old buffer map

#define K_SDRAM_BadPixelCoordBufSize \
	(K_SDRAM_BadPixel_PrvBufSize  + K_SDRAM_BadPixel_CapBufSize_Upper + K_SDRAM_BadPixel_CapBufSize_Lower + K_SDRAM_BadPixel_CapScaleBufSize + K_SDRAM_BadPixel_DummyBufSize)

#else
//Size of Bad pixel coordinate buffer'
#define K_SDRAM_BadPixel_PrvBufSize		((UINT32)8 * (UINT32)1024) //16KB
#define K_SDRAM_BadPixel_CapBufSize		((UINT32)16 * (UINT32)1024) //32KB
#define K_SDRAM_BadPixel_CapScaleBufSize	((UINT32)32 * (UINT32)1024) //64KB
#define K_SDRAM_BadPixel_DummyBufSize		((UINT32)6 * (UINT32)1024) //12KB reserved, compatible for old buffer map

#define K_SDRAM_BadPixelCoordBufSize \
	(K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize + K_SDRAM_BadPixel_CapScaleBufSize + K_SDRAM_BadPixel_DummyBufSize)
#endif

#define K_SDRAM_LscTableBufSize       (UINT32)10344  //(42*123*4+24)/2
#define K_SDRAM_ZoomTableBufSize      (UINT32)2048   //(4*1024)/2

//Size of Osd Display buffer
#define K_SDRAM_OsdDispBufSize       ((UINT32)320*240/2)//37.5KB
#define K_SDRAM_OsdHiddenBufSize        ((UINT32)320*240/2)//37.5KB

//Size of Character font buffer
#define K_SDRAM_OsdFontBufSize         (K_OSDFONT_NumChar * 128)

//Size of Character font buffer
#define K_SDRAM_OsdIconBufSize         (K_OSDICON_NumChar * 128)
#define K_SDRAM_iconLayoutBufSize         ((UINT32)256)
//size of resource
#define K_SDRAM_OsdResBufSize		((UINT32)288*32/2)//9KB

//size of osd
#define K_SDRAM_OsdBufSize			osdMemSizeInit()
//Size of Graphic font buffer
#define K_SDRAM_GrafFontBufSize         (K_GRAFFONT_Width * K_GRAFFONT_Height + 4096 + 2048) // 4096 + 2048 is for DS which needs 70208 word

//#define K_SDRAM_DOS_WorkSize            ((UINT32)0x00020000)     //128K word = 256 byte, at least dos cluster size
#define K_SDRAM_DOS_WorkSize            ((UINT32)0x00020000 - 0x4000)     //112K word = 224 byte, at least dos cluster size

//Size of DOS FAT1 buffer
#define K_SDRAM_DOS_Fat1BufSize         ((UINT32)0x00010000)     //64K word = 128K byte

//Size of DCF directory buffer
#define K_SDRAM_DcfDirBufSize           ((UINT32)0x0001FA4)      //director index 100 to 999 => 900 * 9 = 8100

//patch4.5@ada@Record DCF file name begin
#define K_SDRAM_JPG_FdbParameterBufSize ((UINT32)0x00018696) //10 * 9999
//#define K_SDRAM_JPG_FdbParameterBufSize ((UINT32)0x00013878) //8 * 9999

//#define K_SDRAM_AVI_FdbParameterBufSize ((UINT32)0x00018696) //10 * 9999
#define K_SDRAM_AVI_FdbParameterBufSize ((UINT32)0x00000000)
//patch4.5@ada@Record DCF file name end

#define K_SDRAM_WAV_FdbParameterBufSize ((UINT32)0x00000000)

#define K_SDRAM_KeySoundSize		((UINT32)2560)
//#define K_SDRAM_PwrCtrlParameterBufSize		((UINT32)512)

#define K_SDRAM_DosRamBufSize		sysMemSizeGet(MEM_IDX_DOS_RAM)    //((UINT32)0)//NO DOS RAM

#ifdef ICAT_OPTION
	#define K_SDRAM_IcatCdspParamBufSize	(UINT32)1024 /* 2048/2 */
	//#define K_SDRAM_IcatGammaBufSize		(UINT32)1024
	//#define K_SDRAM_IcatLcBufSize			(UINT32)1024
	
	//#define K_SDRAM_ICatRscBufSize		  (UINT32)1024	
#endif

#define K_SDRAM_PreviewDistMeSize		(UINT32)(12*12*2) 
#define K_SDRAM_VideoDistMeSize			K_SDRAM_PreviewDistMeSize



//patch5.0@richie@SDRAM begin
//*************************************
//Preview Capture
//*************************************
//Size of Preview Buffer

#define K_SDRAM_PreviewBufSizeA 	   (K_PREVIEW_BUF_WIDTH * K_PREVIEW_BUF_HEIGHT)
#define K_SDRAM_PreviewBufSizeB 	   (K_PREVIEW_BUF_WIDTH * K_PREVIEW_BUF_HEIGHT)
#define K_SDRAM_PreviewBufSizeC 	   (K_PREVIEW_BUF_WIDTH * K_PREVIEW_BUF_HEIGHT)
#define K_SDRAM_PreviewBufSizeD 	   (K_PREVIEW_BUF_WIDTH * K_PREVIEW_BUF_HEIGHT)

//*************************************
//Preview Sencod Path
//*************************************
#define K_SDRAM_Preview2BufSizeA		((UINT32)320 * (UINT32)240) 
#define K_SDRAM_Preview2BufSizeB		((UINT32)320 * (UINT32)240) 
#define K_SDRAM_Preview2BufSizeC		((UINT32)320 * (UINT32)240) 
#define K_SDRAM_Preview2BufSizeD		((UINT32)320 * (UINT32)240) 
//*************************************
//photoframe buffer
//*************************************
#define K_SDRAM_PvPFYUVSize			((UINT32)320 * (UINT32)240) 
#define K_SDRAM_PvPFJpgSize				((UINT32)512 * (UINT32)1024) 


//*************************************
//Single Capture
//*************************************

//*************************************
//Preview/AVI Capture
//*************************************

#if FRAME_BUF_NUM==2
	#define K_SDRAM_AviPvwBufSizeA_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC_1080p			0
	#define K_SDRAM_AviPvwBufSizeD_1080p	        0
	#define K_SDRAM_AviPvwBufSizeA			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC			0
	#define K_SDRAM_AviPvwBufSizeD			0

	#define K_SDRAM_AviPvwBufSizeA_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeB_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeC_D1		0
	#define K_SDRAM_AviPvwBufSizeD_D1		0

	#define K_SDRAM_AviPvwBufSizeA_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA		0
	#define K_SDRAM_AviPvwBufSizeD_VGA		0

	#define K_SDRAM_AviPvwBufSizeA_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeD_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
		
#elif FRAME_BUF_NUM==3
	#define K_SDRAM_AviPvwBufSizeA_1080p		(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeD_1080p	        0
	
	#define K_SDRAM_AviPvwBufSizeA			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeD			0
	
	#define K_SDRAM_AviPvwBufSizeA_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeB_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeC_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeD_D1		0

	#define K_SDRAM_AviPvwBufSizeA_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeD_VGA		0

	#define K_SDRAM_AviPvwBufSizeA_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeD_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	
#else
    #define K_SDRAM_AviPvwBufSizeA_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC_1080p			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeD_1080p	        (K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeA			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeB			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeC			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)
	#define K_SDRAM_AviPvwBufSizeD			(K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT)

	#define K_SDRAM_AviPvwBufSizeA_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeB_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeC_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)
	#define K_SDRAM_AviPvwBufSizeD_D1		(K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1)

	#define K_SDRAM_AviPvwBufSizeA_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeD_VGA		(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)

	#define K_SDRAM_AviPvwBufSizeA_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeB_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeC_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	#define K_SDRAM_AviPvwBufSizeD_VGA_60FPS	(K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA)
	
#endif


#if FRAME_BUF_NUM==2
	#define K_SDRAM_Avi2BufSizeA		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeB		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeC		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeD		((UINT32)320 * (UINT32)240) 
#elif FRAME_BUF_NUM==3
	#define K_SDRAM_Avi2BufSizeA		((UINT32)320 * (UINT32)240) 	
	#define K_SDRAM_Avi2BufSizeB		((UINT32)320 * (UINT32)240) 	
	#define K_SDRAM_Avi2BufSizeC		((UINT32)320 * (UINT32)240) 	
	#define K_SDRAM_Avi2BufSizeD		0	
#else
	#define K_SDRAM_Avi2BufSizeA		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeB		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeC		((UINT32)320 * (UINT32)240) 
	#define K_SDRAM_Avi2BufSizeD		((UINT32)320 * (UINT32)240) 
#endif

#if 0
#if FRAME_BUF_NUM==2
	#define K_SDRAM_AviCapVLCBufSizeA_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 12) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 12) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 12) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 12) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 12) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 12) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 12) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 6) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 6) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 6) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 6) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)  // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)  // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)
#elif FRAME_BUF_NUM==3
	#define K_SDRAM_AviCapVLCBufSizeA		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 16) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 16) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 16) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 16) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 8) + 130 + 6)  // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 8) + 130 + 6)  // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 8) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 8) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)  // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)  // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 8) + 130 + 6)
#else
       #define K_SDRAM_AviCapVLCBufSizeA_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 10) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 10) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p / 10) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_1080p		((K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /10) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 10) + 130 + 6)	 // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 10) + 130 + 6)	 // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 10) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD		((K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT / 10) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 5) + 130 + 6)  // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 5) + 130 + 6)  // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 5) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_D1		((K_VIDEOCLIP_WIDTH_D1 * K_VIDEOCLIP_HEIGHT_D1 / 5) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeA_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 5) + 130 + 6)  // +6 to keep the aligment of 8
	#define K_SDRAM_AviCapVLCBufSizeB_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 5) + 130 + 6)  // *3/2 for HD solution, 20091110
	#define K_SDRAM_AviCapVLCBufSizeC_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 5) + 130 + 6)
	#define K_SDRAM_AviCapVLCBufSizeD_VGA		((K_VIDEOCLIP_WIDTH_VGA * K_VIDEOCLIP_HEIGHT_VGA / 5) + 130 + 6)
#endif

//patch5.2.1@ada@Fix AVI recording 448x336 frame corrupt end
#define K_SDRAM_AviCapAudioBufSize		((UINT32)128* (UINT32)1024)

#if FRAME_BUF_NUM==2
    #define K_SDRAM_AviCapFragBufSizeA_1080p		((UINT32)112 * (UINT32)1024 * (UINT32)2 + K_SDRAM_AviCapVLCBufSizeA_1080p)
	#define K_SDRAM_AviCapFragBufSizeA		((UINT32)112 * (UINT32)1024 * (UINT32)2 + K_SDRAM_AviCapVLCBufSizeA)
	#define K_SDRAM_AviCapFragBufSizeA_D1		((UINT32)112 * (UINT32)1024 * (UINT32)2 + K_SDRAM_AviCapVLCBufSizeA_D1)
	#define K_SDRAM_AviCapFragBufSizeA_VGA		((UINT32)112 * (UINT32)1024 * (UINT32)2 + K_SDRAM_AviCapVLCBufSizeA_VGA)
#elif FRAME_BUF_NUM==3
	#define K_SDRAM_AviCapFragBufSizeA		((UINT32)48 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA)
	#define K_SDRAM_AviCapFragBufSizeA_D1		((UINT32)48 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA_D1)
	#define K_SDRAM_AviCapFragBufSizeA_VGA		((UINT32)48 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA_VGA)
#else
    #define K_SDRAM_AviCapFragBufSizeA_1080p		((UINT32)128 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA_1080p)
	#define K_SDRAM_AviCapFragBufSizeA		((UINT32)128 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA)
	#define K_SDRAM_AviCapFragBufSizeA_D1		((UINT32)128 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA_D1)
	#define K_SDRAM_AviCapFragBufSizeA_VGA		((UINT32)128 * (UINT32)1024 * (UINT32)4 + K_SDRAM_AviCapVLCBufSizeA_VGA)
#endif
#endif
#if 0
#define K_SDRAM_AviCapHeaderBufSize		((UINT32)256)
//#define K_SDRAM_AviCaptureIdx1Size		(remain size)

#define K_SDRAM_AviCapQTabNum           10
#define K_SDRAM_AviCapQTabSize          ((UINT32)65*K_SDRAM_AviCapQTabNum)
#endif
#define K_SDRAM_MoeTmpBufSize		(512ul*16)
#define K_SDRAM_FdIIBufSize			650000ul//1400000//650000ul//(403916)
#define K_SDRAM_USBBufSize		(512ul*4)
//*************************************
//Playback 1/4/9
//*************************************

#define  K_SDRAM_FileOrderTableSize				((UINT32)9999)

//#define SP1K_PB_DISP_BUF_SIZE_A   ((K_SDRAM_TotalSize <= 0x400000)?((UINT32)640 * (UINT32)480):((UINT32)960 * (UINT32)720))
//#define SP1K_PB_DISP_BUF_SIZE_B   ((K_SDRAM_TotalSize <= 0x400000)?((UINT32)640 * (UINT32)480):((UINT32)960 * (UINT32)720))

#define SP1K_PB_DISP_BUF_SIZE_A		((UINT32)960 * (UINT32)720)
#define SP1K_PB_DISP_BUF_SIZE_B		((UINT32)960 * (UINT32)720)

#define K_SDRAM_ImagePlaybackDisplayBufSize	(SP1K_PB_DISP_BUF_SIZE_A + SP1K_PB_DISP_BUF_SIZE_B)

#define SP1K_PB_DISP_BUF_SIZE_HDMI_A		((UINT32)960 * (UINT32)720)
#define SP1K_PB_DISP_BUF_SIZE_HDMI_B		((UINT32)960 * (UINT32)720)
#define K_SDRAM_ImagePlaybackDisplayBufSize_HDMI	(SP1K_PB_DISP_BUF_SIZE_HDMI_A + SP1K_PB_DISP_BUF_SIZE_HDMI_B)

#define K_SDRAM_ImagePlaybackBufSize			((UINT32)640 * (UINT32)480)

//*************************************
//Playback AVI
//*************************************
#if 1 //xian -- 20110903, not related to  frame buf num, (FRAME_BUF_NUM==2)
//#define K_SDRAM_AviPbBufSizeA		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT * 3/4) //((UINT32)320 * (UINT32)240)
//#define K_SDRAM_AviPbBufSizeB		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT * 3/4) //((UINT32)320 * (UINT32)240)
//#define K_SDRAM_AviPbBufSizeC		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT* 3/4) //((UINT32)320 * (UINT32)240)
//#define K_SDRAM_AviPbFragSize		((UINT32)768 * (UINT32)1024+128*(UINT32)1024)  // 2M for play, 20091110

//related to max video decode size
#define K_SDRAM_AviPbBufSizeA		((UINT32)960 * 720) //((UINT32)320 * (UINT32)240)
#define K_SDRAM_AviPbBufSizeB		((UINT32)960 * 544) //((UINT32)320 * (UINT32)240)
#define K_SDRAM_AviPbBufSizeC		((UINT32)960 * 544) //((UINT32)320 * (UINT32)240)
//#define K_SDRAM_AviPbFragSize		((UINT32)1024 * (UINT32)1024 + 180*(UINT32)1024)  // variable base on mem size
#define K_SDRAM_AviPbFragSize	sysMemSizeGet(MEM_IDX_VIDEO_PB_FRAG)

#define K_SDRAM_AviPbVideoBufSize	((UINT32)(0) )//320 * 240 / 4 + 256 + 256
#define K_SDRAM_AviPbAudioBufSize	((UINT32)64 * (UINT32)1024)//((UINT32)4096)//for upsampling
#define K_SDRAM_AviPbAudioChunkBufSize	((UINT32)16 * (UINT32)1024 + (UINT32)512)
//#define K_SDRAM_AviPbIdx1BufSize	(remain size)
#define K_SDRAM_AviAeSize			(64ul*64*2)
#else
#define K_SDRAM_AviPbBufSizeA		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT) //((UINT32)320 * (UINT32)240)
#define K_SDRAM_AviPbBufSizeB		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT) //((UINT32)320 * (UINT32)240)
#define K_SDRAM_AviPbBufSizeC		((UINT32)K_VIDEOCLIP_WIDTH * K_VIDEOCLIP_HEIGHT) //((UINT32)320 * (UINT32)240)
#define K_SDRAM_AviPbFragSize		((UINT32)1024 * (UINT32)1024+128*(UINT32)1024)  // 2M for play, 20091110
#define K_SDRAM_AviPbVideoBufSize	((UINT32)(0) )//320 * 240 / 4 + 256 + 256
#define K_SDRAM_AviPbAudioBufSize	((UINT32)64 * (UINT32)1024)//((UINT32)4096)
#define K_SDRAM_AviPbAudioChunkBufSize	((UINT32)16 * (UINT32)1024 + (UINT32)512)
//#define K_SDRAM_AviPbIdx1BufSize	(remain size)
#define K_SDRAM_AviAeSize			(64ul*64*2)
#endif

//*************************************
//ALC
//*************************************
#define K_SDRAM_AlcMoeExtraBufSize			((UINT32)2048) //extra buffer size:16bit mode =>window_size(word)
#define K_SDRAM_AlcMoeCmdBufSize		((UINT32)24)   //command buffer size : 24(word)

//*************************************
//DPS
//*************************************
#define K_SDRAM_DPSBufSize 	((UINT32)8UL*1024UL)

//*************************************
//Code Sentry Redirect buffer size
//*************************************
#define K_SDRAM_CodeSentryRedirectBufSize 	((UINT32)32)

//*************************************
//sound conctrol
//*************************************
#if (Sound_Ctrl_Option)
#define K_SDRAM_SoundCtrlSize ((UINT32)4*(UINT32)1024)//4K WORD
#else
#define K_SDRAM_SoundCtrlSize ((UINT32)(0) )//((UINT32)4*(UINT32)1024)//4K WORD
#endif
//***************************************************************************
//Buffer Address
//***************************************************************************
//*************************************
//Common
//*************************************
#define K_SDRAM_CodeAddr		sysMemAddrGet(MEM_IDX_TEXT)
#define K_SDRAM_ShadowAddr		sysMemAddrGet(MEM_IDX_DATA)

#define K_SDRAM_BadPixelCoordBufAddr	sysMemAddrGet(MEM_IDX_BP)
#define K_SDRAM_LSCTableBufAddr			sysMemAddrGet(MEM_IDX_LSC)
#define K_SDRAM_ZoomTableBufAddr			sysMemAddrGet(MEM_IDX_ZOOM)


#define K_SDRAM_OsdBufAddr		sysMemAddrGet(MEM_IDX_OSD)

/*
#define K_SDRAM_OsdDispBufAddr_A		sysMemAddrGet(MEM_IDX_OSD_DISP_A)
#define K_SDRAM_OsdDispBufAddr_B		sysMemAddrGet(MEM_IDX_OSD_DISP_B)
#define K_SDRAM_OsdFontBufAddr			sysMemAddrGet(MEM_IDX_OSD_FONT)
#define K_SDRAM_OsdIconBufAddr			sysMemAddrGet(MEM_IDX_OSD_ICON)
*/

#define K_SDRAM_GrafFontBufAddr         sysMemAddrGet(MEM_IDX_GRAF_FONT)
#define K_SDRAM_DOS_WorkAddr            sysMemAddrGet(MEM_IDX_DOS_WORK)
#define K_SDRAM_DOS_FAT1Addr            sysMemAddrGet(MEM_IDX_DOS_FAT1)
#define K_SDRAM_DcfDirBufAddr           sysMemAddrGet(MEM_IDX_DCF_DIR)
#define K_SDRAM_JPG_FdbParameterBufAddr sysMemAddrGet(MEM_IDX_FDB)
#define K_SDRAM_AVI_FdbParameterBufAddr sysMemAddrGet(MEM_IDX_FDB)
#define K_SDRAM_WAV_FdbParameterBufAddr sysMemAddrGet(MEM_IDX_FDB)
#define K_SDRAM_FileOrderTableAddr      sysMemAddrGet(MEM_IDX_FILE_ORDER_TAB)
#define K_SDRAM_KeySoundAddr            sysMemAddrGet(MEM_IDX_KEY_SOUND)
//#define K_SDRAM_PwrCtrlParameterBuf     sysMemAddrGet(MEM_IDX_PWR_CTRL_PARAM)
#define K_SDRAM_DosRamStartAddr         sysMemAddrGet(MEM_IDX_DOS_RAM)
#define K_SDRAM_DPSStartAddr         	sysMemAddrGet(MEM_IDX_DPS)
#define K_SDRAM_AlcMoeCommandBufAddr	sysMemAddrGet(MEM_IDX_ALC_MOE_CMD)
#define K_SDRAM_AlcMoeExtraBufAddr		sysMemAddrGet(MEM_IDX_ALC_MOE_EXTRA)
#define K_SDRAM_CodeSentryRedirectAddr  sysMemAddrGet(MEM_IDX_CODE_SENTRY_REDIR)
#define K_SDRAM_IconLayoutBufAddr		sysMemAddrGet(MEM_IDX_ICON_LAYOUT)
#define K_SDRAM_SoundCtrlAddr			sysMemAddrGet(MEM_IDX_SOUND_CTRL)
#define K_SDRAM_UsbAddr			sysMemAddrGet(MEM_IDX_USB)
#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
	#define K_SDRAM_IcatCdspParamBuf        sysMemAddrGet(MEM_IDX_ICAT_CDSP_PARAM)
//	#define K_SDRAM_IcatGammaBuf            sysMemAddrGet(MEM_IDX_ICAT_GAMMA)
//	#define K_SDRAM_IcatLcBuf               sysMemAddrGet(MEM_IDX_ICAT_LC)
	#define K_SDRAM_CommonEndAddr           sysMemAddrGet(MEM_IDX_COMMON_END)
#else
	#define K_SDRAM_CommonEndAddr           sysMemAddrGet(MEM_IDX_COMMON_END)
#endif

//*************************************
//Preview Capture
//*************************************
#define K_SDRAM_PvAwbWndAddr			sysMemAddrGet(MEM_IDX_AWB_WIN)
#define K_SDRAM_PvMoeTmpAddr			sysMemAddrGet(MEM_IDX_MOE_TMP)

#define K_SDRAM_PvAeAddr				sysMemAddrGet(MEM_IDX_STILL_VIEW_AE)
#define K_SDRAM_PvAeWndAddr				sysMemAddrGet(MEM_IDX_STILL_VIEW_AE_WIN)
#define K_SDRAM_PvFdIIAddr				sysMemAddrGet(MEM_IDX_STILL_VIEW_FD)
#define K_SDRAM_PvDistEdgeWndAddrA		sysMemAddrGet(MEM_IDX_STILL_VIEW_EDGE_WINA)
#define K_SDRAM_PvDistEdgeWndAddrB		sysMemAddrGet(MEM_IDX_STILL_VIEW_EDGE_WINB)
#define K_SDRAM_PvDistMeAddr			sysMemAddrGet(MEM_IDX_STILL_VIEW_DIST_ME)

#define K_SDRAM_PvZoomFactorAddr			sysMemAddrGet(MEM_IDX_SMOOTH_ZOOM_FACTOR)
#define K_SDRAM_PvZoomRegTblAddr			sysMemAddrGet(MEM_IDX_SMOOTH_REG_TABLE)
#define K_SDRAM_PvZoomRegValAddr			sysMemAddrGet(MEM_IDX_SMOOTH_REG_VALUE)

#define K_SDRAM_PvBufAddrA    			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV_A)
#define K_SDRAM_PvBufAddrB   			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV_B)
#define K_SDRAM_PvBufAddrC				sysMemAddrGet(MEM_IDX_STILL_VIEW_PV_C)
#define K_SDRAM_PvBufAddrD				sysMemAddrGet(MEM_IDX_STILL_VIEW_PV_D)

#define K_SDRAM_Pv2BufAddrA 			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV2_A)
#define K_SDRAM_Pv2BufAddrB 			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV2_B)
#define K_SDRAM_Pv2BufAddrC 			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV2_C)
#define K_SDRAM_Pv2BufAddrD 			sysMemAddrGet(MEM_IDX_STILL_VIEW_PV2_D)

#define K_SDRAM_PvPFYUVAddr			sysMemAddrGet(MEM_IDX_STILL_VIEW_PF_YUV)//photoframe YUV
#define K_SDRAM_PvPFJpgAddr				sysMemAddrGet(MEM_IDX_STILL_VIEW_PF_JPG)//photoframe jpg

#define K_SDRAM_PvPanoramaTmpMeAddr		sysMemAddrGet(MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME)
#define K_SDRAM_PvPanoramaTmpGmvAddr	sysMemAddrGet(MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV)
#define K_SDRAM_PvPanoramaDstYuvAddr	sysMemAddrGet(MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV)
#define K_SDRAM_PvPanoramaTmpYuvAddr	sysMemAddrGet(MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV)



//#ifdef ICAT_OPTION
	//#define icat_TestBuf_SIZE 				4096
	//#define icat_TestBufAddr                sysMemAddrGet(MEM_IDX_ICAT_TEST)
//#endif

//*************************************
//Preview/AVI Capture
//*************************************
#define K_SDRAM_AviAwbWndAddr			sysMemAddrGet(MEM_IDX_AWB_WIN)
#define K_SDRAM_AviMoeTmpAddr			sysMemAddrGet(MEM_IDX_MOE_TMP)

#define K_SDRAM_AviAeAddr				sysMemAddrGet(MEM_IDX_VIDEO_VIEW_AE)
#define K_SDRAM_AviAeWndAddr			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_AE_WIN)
#define K_SDRAM_AviDistEdgeWndAddrA		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_EDGE_WINA)
#define K_SDRAM_AviDistEdgeWndAddrB		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_EDGE_WINB)
#define K_SDRAM_AviDistMeAddr			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_DIST_ME)

#define K_SDRAM_AviZoomFactorAddr			sysMemAddrGet(MEM_IDX_SMOOTH_ZOOM_FACTOR)
#define K_SDRAM_AviZoomRegTblAddr			sysMemAddrGet(MEM_IDX_SMOOTH_REG_TABLE)
#define K_SDRAM_AviZoomRegValAddr			sysMemAddrGet(MEM_IDX_SMOOTH_REG_VALUE)

#define K_SDRAM_AviPvwBufAddrA			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_A)
#define K_SDRAM_AviPvwBufAddrB			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_B)
#define K_SDRAM_AviPvwBufAddrC		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_C)
#define K_SDRAM_AviPvwBufAddrD		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D)
#if 0
#if FRAME_BUF_NUM==2
	#define K_SDRAM_AviPvwBufAddrC			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_A)
	#define K_SDRAM_AviPvwBufAddrD			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_B)
#else
	#define K_SDRAM_AviPvwBufAddrC		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_C)
	#define K_SDRAM_AviPvwBufAddrD		sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D)
#endif
#endif
#define K_SDRAM_AviPvwBufAddrC_60FPS	sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_C)
#define K_SDRAM_AviPvwBufAddrD_60FPS	sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D)


#define K_SDRAM_Avi2PvwBufAddrA			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV2_A)
#define K_SDRAM_Avi2PvwBufAddrB			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV2_B)
#define K_SDRAM_Avi2PvwBufAddrC			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV2_C)
#define K_SDRAM_Avi2PvwBufAddrD			sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV2_D)



#if 0
#define K_SDRAM_AviCapVLCBufAddrA		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A)
#define K_SDRAM_AviCapVLCBufAddrB		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_B)
#define K_SDRAM_AviCapVLCBufAddrC		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_C)
#define K_SDRAM_AviCapVLCBufAddrD		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_D)
#define K_SDRAM_AviCapVLCBufAddrE		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_E)
#define K_SDRAM_AviCapVLCBufAddrF		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_F)
#define K_SDRAM_AviCapVLCBufAddrG		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_G)
#define K_SDRAM_AviCapVLCBufAddrH		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_H)
#define K_SDRAM_AviCapVLCBufAddrI		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_I)
#define K_SDRAM_AviCapVLCBufAddrJ		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_J)
#define K_SDRAM_AviCapVLCBufAddrK		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_K)
#define K_SDRAM_AviCapVLCBufAddrL		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_L)
#define K_SDRAM_AviCapVLCBufAddrM		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_M)
#define K_SDRAM_AviCapVLCBufAddrN		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_N)
#define K_SDRAM_AviCapVLCBufAddrO		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_O)
#define K_SDRAM_AviCapVLCBufAddrP		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_P)
#define K_SDRAM_AviCapVLCBufAddrQ		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_Q)
#define K_SDRAM_AviCapVLCBufAddrR		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_R)
#define K_SDRAM_AviCapVLCBufAddrS		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_S)
#define K_SDRAM_AviCapVLCBufAddrT		sysMemAddrGet(MEM_IDX_VIDEO_REC_VLC_T)


#define K_SDRAM_AviCapVLCBufAddrA_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_A)
#define K_SDRAM_AviCapVLCBufAddrB_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_B)
#define K_SDRAM_AviCapVLCBufAddrC_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_C)
#define K_SDRAM_AviCapVLCBufAddrD_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_D)
#define K_SDRAM_AviCapVLCBufAddrE_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_E)
#define K_SDRAM_AviCapVLCBufAddrF_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_F)
#define K_SDRAM_AviCapVLCBufAddrG_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_G)
#define K_SDRAM_AviCapVLCBufAddrH_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_H)
#define K_SDRAM_AviCapVLCBufAddrI_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_I)
#define K_SDRAM_AviCapVLCBufAddrJ_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_J)
#define K_SDRAM_AviCapVLCBufAddrK_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_K)
#define K_SDRAM_AviCapVLCBufAddrL_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_L)
#define K_SDRAM_AviCapVLCBufAddrM_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_M)
#define K_SDRAM_AviCapVLCBufAddrN_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_N)
#define K_SDRAM_AviCapVLCBufAddrO_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_O)
#define K_SDRAM_AviCapVLCBufAddrP_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_P)
#define K_SDRAM_AviCapVLCBufAddrQ_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_Q)
#define K_SDRAM_AviCapVLCBufAddrR_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_R)
#define K_SDRAM_AviCapVLCBufAddrS_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_S)
#define K_SDRAM_AviCapVLCBufAddrT_Intr		sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_T)


#define K_SDRAM_AviCapVLCBufAddrTEMP		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_TMP)
#define K_SDRAM_AviCapVLCBufAddrTEMP_Intr	vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_TMP)
#define K_SDRAM_AviQTabAddr             	vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_QTAB)
#define K_SDRAM_AviQTabAddr_Intr			vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_QTAB)
#define K_SDRAM_AviCapAudioBufAddr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_AUDIO)

#define K_SDRAM_AviCapFragBufAddrA			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)

#define K_SDRAM_AviCapHeaderBufAddr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR)
#define K_SDRAM_AviCaptureIdx1Addr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)
#endif
//*************************************
//Playback 1/4/9
//*************************************

#define K_SDRAM_ImagePlaybackDisplayBufAddr	sysMemAddrGet(MEM_IDX_STILL_PB_DISP)
#define K_SDRAM_ImagePlaybackBufAddr		sysMemAddrGet(MEM_IDX_STILL_PB_GEN_A)
#define K_SDRAM_ImagePlaybackGeneralBufAddr	sysMemAddrGet(MEM_IDX_STILL_PB_GEN_B)

//*************************************
//Playback AVI
//*************************************
#define K_SDRAM_AviPbBufAddrA		sysMemAddrGet(MEM_IDX_VIDEO_PB_GEN_A)
#define K_SDRAM_AviPbBufAddrB		sysMemAddrGet(MEM_IDX_VIDEO_PB_GEN_B)
#define K_SDRAM_AviPbBufAddrC		sysMemAddrGet(MEM_IDX_VIDEO_PB_GEN_C)
#define K_SDRAM_AviPbFragAddr		sysMemAddrGet(MEM_IDX_VIDEO_PB_FRAG)

#define K_SDRAM_AviPbVideoBufAddr		sysMemAddrGet(MEM_IDX_VIDEO_PB_VIDEO)
#define K_SDRAM_AviPbAudioBufAddr   	sysMemAddrGet(MEM_IDX_VIDEO_PB_AUDIO)
#define K_SDRAM_AviPbAudioChunkBufAddr	sysMemAddrGet(MEM_IDX_VIDEO_PB_AUDIO_CHUNK)
#define K_SDRAM_AviPbIdx1BufAddr		sysMemAddrGet(MEM_IDX_VIDEO_PB_IDX)

//*************************************
//Maximum number of AVI chuncks
//*************************************
#define K_MAX_AVI_CHUNCKS   (UINT32)( ((UINT32)K_SDRAM_AviPvwBufAddrD - K_SDRAM_AviCaptureIdx1Addr) / 8 )

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

extern UINT8 xdata bUsbPwr;
//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void INITIO_Initialize(void);

void INITIO_SoftwareReset(void);
void INITIO_SetCameraSize(void); //cch
void INITIO_GetCameraSizeIndex(UINT16 index); //cch
void INITIO_SetCameraSizeIndex(UINT16 index); //cch
void INITIO_GetProductInfo(void);
void INITIO_GetInformation(UINT16 infoType);


#endif /*__INITIO_H__*/

