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
 #ifndef __VIDEO_MEM_H__
#define __VIDEO_MEM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 #include "general.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 #if 0
 //Resolution of Video clip
#ifdef SUPPORT_720p 
#define K_VIDEOCLIP_WIDTH_1080p               ((UINT32)1920)
#define K_VIDEOCLIP_HEIGHT_1080p              ((UINT32)1088) 
#define K_VIDEOCLIP_WIDTH               ((UINT32)1280)
#define K_VIDEOCLIP_HEIGHT              ((UINT32)720)
#define K_VIDEOCLIP_WIDTH_D1            ((UINT32)720)//((UINT32)848)modify for D1 size
#define K_VIDEOCLIP_HEIGHT_D1           ((UINT32)480)
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
#define VID_MEM_GROWTH_UP		1
#define VID_MEM_GROWTH_DOWN		0

#define K_SDRAM_AviCapAudioBufSize		((UINT32)128* (UINT32)1024)
#define FRAG_SIZE_IDX	((UINT32)32*1024)
#define VID_VLCBUFNUM_TOTAL  80
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
   typedef enum vidMemCbIdx_e {
   	/* video record mode */
	VID_MEM_IDX_VIDEO_REC_START,
	VID_MEM_IDX_VIDEO_REC_StampA=VID_MEM_IDX_VIDEO_REC_START,
	VID_MEM_IDX_VIDEO_REC_StampB,
	VID_MEM_IDX_VIDEO_REC_StampC,
	VID_MEM_IDX_VIDEO_REC_StampD,
	VID_MEM_IDX_VIDEO_REC_Font,
	VID_MEM_IDX_VIDEO_REC_Text,
	VID_MEM_IDX_VIDEO_REC_IMG,
	VID_MEM_IDX_VIDEO_REC_QTAB,
	VID_MEM_IDX_VIDEO_REC_AUDIO,
	VID_MEM_IDX_VIDEO_REC_HDR,
	VID_MEM_IDX_VIDEO_REC_IDX,
	VID_MEM_IDX_VIDEO_REC_FRAG,
	VID_MEM_IDX_VIDEO_REC_VLC_A, 
	VID_MEM_IDX_VIDEO_REC_VLC_B,
	VID_MEM_IDX_VIDEO_REC_VLC_C,
	VID_MEM_IDX_VIDEO_REC_VLC_D,
	VID_MEM_IDX_VIDEO_REC_VLC_TMP = (VID_MEM_IDX_VIDEO_REC_VLC_A + VID_VLCBUFNUM_TOTAL),
	VID_MEM_IDX_VIDEO_REC_END,
	VID_MEM_IDX_MAX,
} vidmemCbIdx_t;

  typedef struct vidMemCb_s{
   UINT32 addr;
   UINT32 size;
}vidMemCb_t;

 /**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 /**************************************************************************
 *                 buffer addr                 *
 **************************************************************************/
 #if 1
#define K_SDRAM_AviCapVLCBufAddrA		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A)
#define K_SDRAM_AviCapVLCBufAddrB		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_B)
#define K_SDRAM_AviCapVLCBufAddrC		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_C)
#define K_SDRAM_AviCapVLCBufAddrD		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_D)
#if 0
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
#endif

#define K_SDRAM_AviCapVLCBufAddrA_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_A)
#define K_SDRAM_AviCapVLCBufAddrB_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_B)
#define K_SDRAM_AviCapVLCBufAddrC_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_C)
#define K_SDRAM_AviCapVLCBufAddrD_Intr		vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_D)
#if 0
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
#endif

#define K_SDRAM_AviCapVLCBufAddrTEMP		vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_TMP)
#define K_SDRAM_AviCapVLCBufAddrTEMP_Intr	vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_TMP)
#define K_SDRAM_AviQTabAddr             	vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_QTAB)
#define K_SDRAM_AviQTabAddr_Intr			vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_QTAB)
#define K_SDRAM_AviCapAudioBufAddr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_AUDIO)

#define K_SDRAM_AviCapFragBufAddrA			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG)

#define K_SDRAM_AviCapHeaderBufAddr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_HDR)
#define K_SDRAM_AviCaptureIdx1Addr			vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IDX)
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void vidRecMemAddrSet(
	vidMemCb_t* vidpMemCb,
	vidmemCbIdx_t sIdx,
	vidmemCbIdx_t eIdx,
	UINT8 mode,
	UINT8 bAgn
);

UINT32 vidRecMemAddrGet(vidmemCbIdx_t idx);
void vidRecMemSizeSet(vidmemCbIdx_t  idx,UINT32 size);
UINT32 vidRecMemSizeGet(vidmemCbIdx_t  idx);
void vidRecMemSizeInit();
 void vidRecMemAddrInit();
void vidRecMemFrameBufAddrUpdate();
void speedyCaptureMemFrameBufAddrUpdate(void);

void vidRecFrameBufNumSet(UINT8 num);
UINT8 vidRecFrameBufNumGet();
UINT8 vidRecVlcBufNumGet(UINT32 vlcBufSize,UINT32 vlcBufAddrStart,UINT32 vlcBufAddrEnd);
UINT32 vidRecMemAddrGet_Intr(vidmemCbIdx_t idx);
UINT8 vidRecSizeGet();
UINT8 vidRecFramebufModeGet();
void vidRecFramebufModeSet(UINT8 mode);
#endif
