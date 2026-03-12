/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus mMedia
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Technology Co., Ltd.
 *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *
 **************************************************************************/

/**
 * @file		sysMemInit.h
 * @brief		system memory initialization definition
 * @author		Phil Lin
 * @since		2009-01-06
 * @date		2008-01-06
 */
#ifndef __SYS_MEM_INIT_H__
#define __SYS_MEM_INIT_H__
 //=============================================================================
//Header file
//=============================================================================
#include "icat_stub.h"
#include "lsc_cal.h"
#include "usbOpt.h"
#include "motion_det.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
#define DEV_MEM_SIZE_16M     0x00100000UL
#define DEV_MEM_SIZE_64M     0x00400000UL
#define DEV_MEM_SIZE_128M    0x00800000UL
#define DEV_MEM_SIZE_256M    0x01000000UL
#define DEV_MEM_SIZE_512M    0x02000000UL

/*
	Define the dynamic code size support.
	We read the code size recorded in "fw.bin" in system memory initial process
*/
#define SYS_MEM_OPT_TEXT_DYNAMIC    1

/* Define the default code size (in word) */
#define SYS_MEM_SIZE_CODE	(512UL * 1024UL / 2UL)

/* Define the max dynamic code size */
#define SYS_MEM_CODE_BANK	(23UL + 1UL)

#if 0
#if	defined(LSC_CALIB_ENABLE) || defined(ICAT_OPTION) ||  FUNC_KNL_MSCAL || USBSIDC_OPTION ||FUNC_KNL_BP || (MOTION_DET_EN)
	#define SYS_MEM_CODE_BANK	(18UL + 1UL)
#else
	#define SYS_MEM_CODE_BANK	(16UL + 1UL)
#endif
#endif
#define SYS_MEM_SIZE_CODE_MAX	(32UL * 1024UL * SYS_MEM_CODE_BANK) //0x88000 bytes

#define VLCBUFNUM_TOTAL  80
//#define TEST_BIG_VLC_NUM 1 //temp for test,

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------
typedef enum memCbIdx_e {

	/* common area memory block */
	MEM_IDX_COMMON_START = 0, /* Always be 0 */
	MEM_IDX_TEXT = MEM_IDX_COMMON_START, 
	MEM_IDX_DATA,
	MEM_IDX_BP,
	MEM_IDX_LSC,
	MEM_IDX_ZOOM,
	MEM_IDX_OSD,
	/*
	MEM_IDX_OSD_DISP_A,
	MEM_IDX_OSD_DISP_B,
	MEM_IDX_OSD_FONT,
	MEM_IDX_OSD_ICON,*/
	MEM_IDX_GRAF_FONT,
	MEM_IDX_DOS_WORK,
	MEM_IDX_DOS_FAT1,
	MEM_IDX_DCF_DIR,
	MEM_IDX_FDB,
	MEM_IDX_FILE_ORDER_TAB,
	MEM_IDX_KEY_SOUND,
	//MEM_IDX_PWR_CTRL_PARAM,
	MEM_IDX_DOS_RAM,
	MEM_IDX_DPS,
	MEM_IDX_ALC_MOE_CMD,
	MEM_IDX_ALC_MOE_EXTRA,
	MEM_IDX_CODE_SENTRY_REDIR,
	MEM_IDX_ICON_LAYOUT,
	MEM_IDX_SOUND_CTRL,
	
	MEM_IDX_AWB_WIN ,	/* will: move awb window/moe to here */
	MEM_IDX_MOE_TMP,

	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
		MEM_IDX_ICAT_CDSP_PARAM,
		//MEM_IDX_ICAT_GAMMA,
		//MEM_IDX_ICAT_LC,
	#endif
	MEM_IDX_USB,

	MEM_IDX_SMOOTH_ZOOM_FACTOR,
	MEM_IDX_SMOOTH_REG_TABLE,
	MEM_IDX_SMOOTH_REG_VALUE,
	
	MEM_IDX_COMMON_END,

	/* still view mode */	
	MEM_IDX_STILL_VIEW_START,
	MEM_IDX_STILL_VIEW_PV2_A = MEM_IDX_STILL_VIEW_START,
	MEM_IDX_STILL_VIEW_PV2_B,
	MEM_IDX_STILL_VIEW_PV2_C,
	MEM_IDX_STILL_VIEW_PV2_D, 
	
	//MEM_IDX_STILL_VIEW_AWB_WIN = MEM_IDX_STILL_VIEW_START,
	MEM_IDX_STILL_VIEW_AE,
	MEM_IDX_STILL_VIEW_AE_WIN,	
	MEM_IDX_STILL_VIEW_FD,
	MEM_IDX_STILL_VIEW_EDGE_WINA,
	MEM_IDX_STILL_VIEW_EDGE_WINB,
	MEM_IDX_STILL_VIEW_DIST_ME,		
	
	MEM_IDX_STILL_VIEW_PV_A,
	MEM_IDX_STILL_VIEW_PV_B,
	MEM_IDX_STILL_VIEW_PV_C,
	MEM_IDX_STILL_VIEW_PV_D,
	
	MEM_IDX_STILL_VIEW_PF_YUV,
	MEM_IDX_STILL_VIEW_PF_JPG,
	MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV,
	MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME,
	MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV,
	MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV,
	MEM_IDX_STILL_VIEW_END,

	/* video view mode */
	MEM_IDX_VIDEO_VIEW_START,
	MEM_IDX_VIDEO_VIEW_PV2_A = MEM_IDX_VIDEO_VIEW_START,
	MEM_IDX_VIDEO_VIEW_PV2_B,
	MEM_IDX_VIDEO_VIEW_PV2_C,
	MEM_IDX_VIDEO_VIEW_PV2_D, 

	MEM_IDX_VIDEO_VIEW_AE,
	MEM_IDX_VIDEO_VIEW_AE_WIN,
	MEM_IDX_VIDEO_VIEW_EDGE_WINA,
	MEM_IDX_VIDEO_VIEW_EDGE_WINB,
	MEM_IDX_VIDEO_VIEW_DIST_ME,
	
	//MEM_IDX_VIDEO_VIEW_FD,
	//MEM_IDX_VIDEO_VIEW_PV2_A,
	//MEM_IDX_VIDEO_VIEW_PV2_B,
	//MEM_IDX_VIDEO_VIEW_PV2_C,
	//MEM_IDX_VIDEO_VIEW_PV2_D,	
	MEM_IDX_VIDEO_VIEW_PV_A,
	MEM_IDX_VIDEO_VIEW_PV_B,
	MEM_IDX_VIDEO_VIEW_PV_C,
	MEM_IDX_VIDEO_VIEW_PV_D, //56

	MEM_IDX_VIDEO_VIEW_END,

	/* video record mode */
	#if 0
	MEM_IDX_VIDEO_REC_START,
	MEM_IDX_VIDEO_REC_VLC_A = MEM_IDX_VIDEO_REC_START,
	MEM_IDX_VIDEO_REC_VLC_B,
	MEM_IDX_VIDEO_REC_VLC_C,
	MEM_IDX_VIDEO_REC_VLC_D,

	MEM_IDX_VIDEO_REC_VLC_TMP = (MEM_IDX_VIDEO_REC_VLC_A + VLCBUFNUM_TOTAL),

    MEM_IDX_VIDEO_REC_QTAB,
	MEM_IDX_VIDEO_REC_AUDIO,
	MEM_IDX_VIDEO_REC_FRAG,
	MEM_IDX_VIDEO_REC_HDR,
	MEM_IDX_VIDEO_REC_IDX,
	MEM_IDX_VIDEO_REC_END,
	#endif
	/* still playback mode */
	MEM_IDX_STILL_PB_START,
	MEM_IDX_STILL_PB_DISP = MEM_IDX_STILL_PB_START,
	MEM_IDX_STILL_PB_GEN_A,
	MEM_IDX_STILL_PB_GEN_B,
	MEM_IDX_STILL_PB_END,

	/* video playback mode */
	MEM_IDX_VIDEO_PB_START,
	MEM_IDX_VIDEO_PB_GEN_A = MEM_IDX_VIDEO_PB_START,
	MEM_IDX_VIDEO_PB_GEN_B,
	MEM_IDX_VIDEO_PB_GEN_C,
	MEM_IDX_VIDEO_PB_FRAG,
	MEM_IDX_VIDEO_PB_VIDEO,
	MEM_IDX_VIDEO_PB_AUDIO,
	MEM_IDX_VIDEO_PB_AUDIO_CHUNK,
	MEM_IDX_VIDEO_PB_IDX,
	MEM_IDX_VIDEO_PB_END,

	/* the whole memory block */
	MEM_IDX_GLOBAL,
	
	MEM_IDX_MAX,
} memCbIdx_t;

typedef struct memCb_s {
	UINT32 addr;
	UINT32 size;
} memCb_t;

typedef enum memFBSize_e {
	MEM_FB_QVGA,
	MEM_FB_1080p,
	MEM_FB_HD,
	MEM_FB_D1,	
	MEM_FB_VGA,
	MEM_FB_HD_960,
} memFBSize_t;

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
extern const UINT32 xdata devMemSize;
//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------
void sysMemSizeInit(void);
void sysMemAddrUpdate(void);
void sysMemFrameBufAddrUpdate(UINT8 size);
void sysMemStillPbDispAddrUpdate(UINT8 HDMI_On);

void sysMemSizeSet(memCbIdx_t idx, UINT32 size);
UINT32 sysMemAddrGet(memCbIdx_t idx);
UINT32 sysMemAddrGet_Intr(memCbIdx_t idx);
UINT32 sysMemSizeGet(memCbIdx_t idx);
UINT32 sysMemSizeGet_Intr(memCbIdx_t idx);
UINT32 sysMemTotSizeGet(void);
UINT32 sysMemRemainSizeGet(UINT32);
void sysMemStillPvPanoramaAddrUpdate(UINT8 size,UINT16 pvHsize,UINT16 pvVsize,UINT8 frameNum,UINT16 dstHsize,UINT16 dstVsize);

void sysMemVideoPvAddrUpdate(UINT8 size,UINT16 pvHSize,UINT16 pvVsize);
void secondPathPvMenSizeSet();
#endif /*__SYS_MEM_INIT_H__*/

