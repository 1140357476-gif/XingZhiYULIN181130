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
 * @file      pb_general.h
 * @brief     parameters for playback module
 * @author    tq.zhang
 * @since     2010-05-12
 * @date      2010-05-12
*/
#ifndef __PB_GENERAL_H__
#define __PB_GENERAL_H__

#define STILL_LIB_PB_DMY (!FUNC_KNL_STLPB)
#define STILL_PHOTOFRAME_DMY 0
//=============================================================================
//Header file
//=============================================================================

//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define K_File_DataType_YUV420          0x01
#define K_File_DataType_YUV422          0x02

#define K_PLAYBACK_TYPE_THUMB         (UINT8)0x00
#define K_PLAYBACK_TYPE_JPG           (UINT8)0x01
#define K_PLAYBACK_TYPE_AVI           (UINT8)0x02
#define K_PLAYBACK_TYPE_WAV           (UINT8)0x03
#define K_PLAYBACK_TYPE_VIDONLY       (UINT8)0x04
#define K_PLAYBACK_TYPE_UNKNOWN       (UINT8)0xFF

#define PB_DISP_BUF_A	0
#define PB_DISP_BUF_B	1

#define PB_DECODE_OPT_QVGA		(1 << 0)
#define PB_DECODE_OPT_THUMB		(1 << 1)
#define PB_DECODE_OPT_FULL			(1 << 2)

#define PB_DECODE_QV_WIDTH	(UINT16)(320)    
#define PB_DECODE_QV_HEIGHT (UINT16)(240)

#define PB_DECODE_THB_WIDTH  (UINT16)(160)
#define PB_DECODE_THB_HEIGHT (UINT16)(120)


#define PB_DECODE_WIDTH	(320L)
#define PB_DECODE_HEIGHT	(240L)

#define PB_MULTIPLAY_LAYOUT_4		4 // 2*2
#define PB_MULTIPLAY_LAYOUT_9		9 // 3*3
#define PB_MULTIPLAY_LAYOUT_MAX	PB_MULTIPLAY_LAYOUT_9

#define SP1K_STARTUP_IMG_WIDTH	320
#define SP1K_STARTUP_IMG_HEIGHT	240
#define SP1K_STARTUP_IMG_QTABLE	85
#define SP1K_STARTUP_IMG_TYPE		K_File_DataType_YUV422

#define PB_MT_ICON_W   32
#define PB_MT_ICON_H   32

#define PB_ICON_LEFT    0x0000
#define PB_ICON_CENTER  0x0100
#define PB_ICON_RIGHT   0x0200
#define PB_ICON_TOP     0x0000
#define PB_ICON_VCENTER 0x1000
#define PB_ICON_BOTTOM  0x2000

#define PB_PARAM_MT_RESID_MASK  0x00ff
#define PB_PARAM_MT_ALIG_MASK   0xff00

#define PB_FILE_ERROR_W 160
#define PB_FILE_ERROR_H 120

typedef enum {
	SP1K_PB_CALLBACK_ABORT = 0,
	SP1K_PBABORT_CALLBACK_MAX
} PbCallback_e;

typedef enum PBSPlayMode_e{
	PB_SPLAY_MODE_THUMB=0,
	PB_SPLAY_MODE_QV=1,
	PB_SPLAY_MODE_MAIN,

	PB_SPLAY_MODE_ZOOM1X=0x80,
}PBSPlayMode_t;
//------------------------------------------------------------------------------
//Data type
//------------------------------------------------------------------------------
typedef enum PBParam_e {
	PB_PARAM_AUDIO_RES_ID,

	PB_PARAM_SLIDE_SHOW_RPT,
	PB_PARAM_SLIDE_SHOW_FREQ,

	PB_PARAM_ROT,
	PB_PARAM_EXIF_ORIENT,

	PB_PARAM_MT_LOCK,
	PB_PARAM_MT_MEMO,
	PB_PARAM_MT_VIDEO,

	PB_PARAM_FILE_ERR_RES,

	PB_PARAM_SPLAY_MODE,	

	PB_PARAM_DISP_BUF_W,
	PB_PARAM_DISP_BUF_H,

	PB_PARAM_THUMB_START_X_Y,
	PB_PARAM_THUMB_W_H,

	PB_PARAM_JPG_ROT,
	PB_PARAM_MAX,
} PBParam_t;

typedef struct PBFile_s {
	UINT16 FileIdx;
	UINT8 FileType;
	UINT8 FileAttrib;
	UINT16 FileStartCluster;
	UINT8 FileName[12];
	UINT16 MediaWidth;
	UINT16 MediaHeight;
	UINT8 MediaTime[20];
} PBFile_t;

typedef struct PBMultiImg_s {
	struct PBFile_s pImgInfo;
	UINT16 OffsetX;
	UINT16 OffsetY;
	UINT16 ImgWidth;
	UINT16 ImgHeight;
	UINT8 VoiceMemo;
	UINT8 err;
    UINT16 dirCnt;
}PBMultiImg_t;

typedef struct PBMultiLayout_s {
	UINT8 Type;
	UINT8 Max;
	UINT8 Total;
	UINT8 HNum;
	UINT8 VNum;
} PBMultiLayout_t; 

typedef struct{
    UINT32 DecBuff;
    UINT16 DecW;
    UINT16 DecH;
    UINT8 RotMode;
    UINT8 ImgType; //0: main, 1: thumb, 2: QVGA
}Pb_Dec_Parameter;

typedef struct{
	UINT16 hOff;
	UINT16 vOff;
	UINT16 hSize;
	UINT16 vSize;
}RECT16;

typedef enum {
	PB_SHIFT_UP,
	PB_SHIFT_DOWN,
	PB_SHIFT_LEFT,
	PB_SHIFT_RIGHT,
	PB_SHIFT_PAGE_UP,
	PB_SHIFT_PAGE_DOWN
} PBShiftDir_t;

/*
typedef enum {
	PB_IMG_EFFECT_NORMAL,
	PB_IMG_EFFECT_RED,
	PB_IMG_EFFECT_GREEN,
	PB_IMG_EFFECT_BLUE,
	PB_IMG_EFFECT_BINARIZE,
	PB_IMG_EFFECT_NEGTIVE,
	PB_IMG_EFFECT_SEPIA,
	PB_IMG_EFFECT_SUNSET,
	PB_IMG_EFFECT_WARM,
	PB_IMG_EFFECT_COLD,
	PB_IMG_EFFECT_BLACKWHITE,
	PB_IMG_EFFECT_CONTRAST,

	PB_IMG_EFFECT_MAX
} PBEffectList_t;*/

typedef enum pbEditType_e{
	PB_IMG_EFFECT = 0,
	PB_IMG_EFFECT_NORMAL,
	PB_IMG_EFFECT_RED,
	PB_IMG_EFFECT_GREEN,
	PB_IMG_EFFECT_BLUE,
	PB_IMG_EFFECT_BINARIZE,
	PB_IMG_EFFECT_NEGTIVE,
	PB_IMG_EFFECT_SEPIA,
	PB_IMG_EFFECT_SUNSET,
	PB_IMG_EFFECT_WARM,
	PB_IMG_EFFECT_COLD,
	PB_IMG_EFFECT_BLACKWHITE,
	PB_IMG_EFFECT_CONTRAST,

	/*ROTATION*/
	PB_ROT = 0x10,

	/*RESIZE*/
	PB_RESIZE,

	/*CROP*/
	PB_CROP,

	PB_IMG_EFFECT_MAX
}pbEditType_t;

typedef enum {
    PB_ROT_0    = 0x00,
    PB_ROT_90,
    PB_ROT_180,
    PB_ROT_270,
    PB_ROT_SET_NUM
} PBRotParam_t;

typedef enum {
    PB_GPRM_ROT_0    = 0x00,
    PB_GPRM_ROT_90   = 0x05,
    PB_GPRM_ROT_180  = 0x03,
    PB_GPRM_ROT_270  = 0x06
} PBGPRMRotParam_t;


typedef enum PBDispBufMode_e {
	PB_DISP_BUF_MODE_INIT = (UINT8)0,
	PB_DISP_BUF_MODE_AB = (UINT8)1,
	PB_DISP_BUF_MODE_ZOOM = (UINT8)2,
	
} PBDispBufMode_t;



typedef struct pbCropImage{
	UINT32 srcAddrW;
	UINT16 srcWidth;
	UINT16 srcHeight;
	UINT16 recWidth;
	UINT16 recHeight;
	UINT16 startX;
	UINT16 startY;
}PbCropImage_t;

typedef struct pbImageDecPara{
	UINT32 srcAddr;
	UINT32 decAddr;
	UINT16 srcWidth;
	UINT16 srcHeight;
	UINT16 dstWidth;
	UINT16 dstHeight;
	UINT16 decOriWidth;
	UINT16 decOriHeight;
	UINT16 scaleHeight;
	//UINT8 fit;
	UINT8 fac;
	UINT8 opt;
	UINT8 rotMode;
	UINT8 dataType;
	UINT8 imgeType;
	UINT16 dmyX;
	UINT16 dmyY;
}pbImageDecPara_t;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

#endif /*__PB_GENERAL_H__*/



