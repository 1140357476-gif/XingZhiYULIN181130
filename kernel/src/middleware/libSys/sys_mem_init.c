/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
/**
 * @file		sys_mem_init.c
 * @brief		system memory initialization related function
 * @author		Phil Lin
 * @since		2009-01-06
 * @date		2009-01-09
 */

#define __FILE __FILE_ID_SYS_MEM_INIT__  
#include "general.h"
#include "sys_mem_init.h"
#include "initio.h"
#include "dbg_def.h"
#include "dbg_file_list.h"
#include "reg_def.h"
#include "date_stamp.h"
#include "video_clip.h"
#include "sp1k_ae_api.h"
#include "App_ui_para.h"
#include "pv_task.h"
#include "sp1k_video_api.h"
#include "video_mem.h"
#include "hal_dram.h"
#include "app_video_view.h"
#include "panorama_def.h"
#include "sp1k_pv_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
#define MEM_GROWTH_UP		1
#define MEM_GROWTH_DOWN		0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static memCb_t xdata sgMemCb[MEM_IDX_MAX];
extern UINT8 seamLessLpRecEn;
extern xdata UINT16 G_UIStatus;

/*
	DO NOT modify this address, 
	or you should modify the batch file "post.bat" & "STARTUP.A51"
*/
static UINT32 code fwSize _at_ 0x7FF0;//4 Bytes reserved for recording fw size

static void sysMemSizeInit_Global(void);
static void sysMemCbAddrSet(memCb_t* pMemCb, memCbIdx_t sIdx, memCbIdx_t eIdx, UINT8 mode, UINT8 bAgn);
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

//-----------------------------------------------------------------------------
//sysMemSizeInit
//-----------------------------------------------------------------------------
/**
 * @brief	System memory pool initialization
 * @param	NULL
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since	2009-01-09
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
void sysMemSizeInit(void)
{
	memCb_t xdata* pMemCb = &sgMemCb[0];
	UINT8 i = 0;
	UINT32 varSize;
		
	sysMemSizeInit_Global();
	#if 0
	#if (VIDEO_MD_REC_FRAME_BUF_NUM_CTRL_EN == 1)
	vidRecFrameBufNumSet(VIDEO_MD_PREVIEW_FRAME_BUF_NUM);
	#else
	vidRecFrameBufNumSet(2);
	#endif
	#endif
	//printf("###syMemSize\n");
	/* common area memory block */

#if SYS_MEM_OPT_TEXT_DYNAMIC
	/* The fw size had recorded into code segment by "post.bat" in compiling process */
	if (fwSize) {
		printf("FwSize  : 0x%lX\n", fwSize);

		/* Check the F/W size */
		//ASSERT(fwSize <= SYS_MEM_SIZE_CODE_MAX, 0); 
		
		pMemCb[MEM_IDX_TEXT].size = fwSize >> 1;
	} else {
		/* If the fw size had not been successfully recorded :( */
		ASSERT(fwSize, 1);
		pMemCb[MEM_IDX_TEXT].size = SYS_MEM_SIZE_CODE; 
	}
#else
	pMemCb[MEM_IDX_TEXT].size = SYS_MEM_SIZE_CODE; 
#endif //SYS_MEM_OPT_TEXT_DYNAMIC

	pMemCb[MEM_IDX_DATA].size = K_SDRAM_ShadowSize;
	pMemCb[MEM_IDX_BP].size = K_SDRAM_BadPixelCoordBufSize;
	pMemCb[MEM_IDX_LSC].size = K_SDRAM_LscTableBufSize;
	pMemCb[MEM_IDX_ZOOM].size = K_SDRAM_ZoomTableBufSize;
	pMemCb[MEM_IDX_OSD].size = K_SDRAM_OsdBufSize;
	/*
	pMemCb[MEM_IDX_OSD_DISP_A].size = K_SDRAM_OsdDispBufSize;
	pMemCb[MEM_IDX_OSD_DISP_B].size = K_SDRAM_OsdDispBufSize;
	pMemCb[MEM_IDX_OSD_FONT].size = K_SDRAM_OsdFontBufSize;
	pMemCb[MEM_IDX_OSD_ICON].size = K_SDRAM_OsdFontBufSize;*/
	pMemCb[MEM_IDX_GRAF_FONT].size = K_SDRAM_GrafFontBufSize;
	pMemCb[MEM_IDX_DOS_WORK].size = K_SDRAM_DOS_WorkSize;
	pMemCb[MEM_IDX_DOS_FAT1].size = K_SDRAM_DOS_Fat1BufSize;
	pMemCb[MEM_IDX_DCF_DIR].size = K_SDRAM_DcfDirBufSize;
	pMemCb[MEM_IDX_FDB].size = K_SDRAM_JPG_FdbParameterBufSize;
	pMemCb[MEM_IDX_FILE_ORDER_TAB].size = K_SDRAM_FileOrderTableSize;
	pMemCb[MEM_IDX_KEY_SOUND].size = K_SDRAM_KeySoundSize;
	//pMemCb[MEM_IDX_PWR_CTRL_PARAM].size = K_SDRAM_PwrCtrlParameterBufSize;	
	//sysMemSizeInit_DosRam();	
	pMemCb[MEM_IDX_DOS_RAM].size = 0;
	pMemCb[MEM_IDX_DPS].size = K_SDRAM_DPSBufSize;
	pMemCb[MEM_IDX_ALC_MOE_CMD].size = K_SDRAM_AlcMoeCmdBufSize;
	pMemCb[MEM_IDX_ALC_MOE_EXTRA].size = K_SDRAM_AlcMoeExtraBufSize;
	pMemCb[MEM_IDX_CODE_SENTRY_REDIR].size = K_SDRAM_CodeSentryRedirectBufSize;
	pMemCb[MEM_IDX_ICON_LAYOUT].size = K_SDRAM_iconLayoutBufSize;
	pMemCb[MEM_IDX_SOUND_CTRL].size = K_SDRAM_SoundCtrlSize;

	pMemCb[MEM_IDX_AWB_WIN].size = (UINT32)32*32*4+64;  //4096 + 64;
	pMemCb[MEM_IDX_MOE_TMP].size = (UINT32)K_SDRAM_MoeTmpBufSize;        //jg:moe tmp buf for awb or other  
	pMemCb[MEM_IDX_USB].size = (UINT32)K_SDRAM_USBBufSize;  
	pMemCb[MEM_IDX_SMOOTH_ZOOM_FACTOR].size = 128ul;
	pMemCb[MEM_IDX_SMOOTH_REG_TABLE].size = 128ul;
	pMemCb[MEM_IDX_SMOOTH_REG_VALUE].size = 2048ul * 2ul;
 	#ifdef ICAT_OPTION	
		pMemCb[MEM_IDX_ICAT_CDSP_PARAM].size = K_SDRAM_IcatCdspParamBufSize;
		//pMemCb[MEM_IDX_ICAT_GAMMA].size = K_SDRAM_IcatGammaBufSize;
		//pMemCb[MEM_IDX_ICAT_LC].size = K_SDRAM_IcatLcBufSize;
	#endif	

	/* still view mode */	

	pMemCb[MEM_IDX_STILL_VIEW_AE].size =(UINT32) K_SDRAM_AviAeSize;
	pMemCb[MEM_IDX_STILL_VIEW_AE_WIN].size = (UINT32)25*8;        //will: max ae window size
	pMemCb[MEM_IDX_STILL_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
	pMemCb[MEM_IDX_STILL_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
	pMemCb[MEM_IDX_STILL_VIEW_DIST_ME].size = K_SDRAM_PreviewDistMeSize;           //DIST Mv output value        
   	pMemCb[MEM_IDX_STILL_VIEW_PV_A].size = K_SDRAM_PreviewBufSizeA;
	pMemCb[MEM_IDX_STILL_VIEW_PV_B].size = K_SDRAM_PreviewBufSizeB;
	pMemCb[MEM_IDX_STILL_VIEW_PV_C].size = K_SDRAM_PreviewBufSizeC;
	pMemCb[MEM_IDX_STILL_VIEW_PV_D].size = K_SDRAM_PreviewBufSizeD;
	pMemCb[MEM_IDX_STILL_VIEW_PV2_A].size = K_SDRAM_Preview2BufSizeA;  //will: preview second path
	pMemCb[MEM_IDX_STILL_VIEW_PV2_B].size = K_SDRAM_Preview2BufSizeB;
	pMemCb[MEM_IDX_STILL_VIEW_PV2_C].size = K_SDRAM_Preview2BufSizeC;
	pMemCb[MEM_IDX_STILL_VIEW_PV2_D].size = K_SDRAM_Preview2BufSizeD;
	pMemCb[MEM_IDX_STILL_VIEW_FD].size = (UINT32)K_SDRAM_FdIIBufSize;        //jg:FD jobQueue,tbl and II tmpbuf 
	pMemCb[MEM_IDX_STILL_VIEW_PF_YUV].size = (UINT32)K_SDRAM_PvPFYUVSize;        //photoframe yuv
	pMemCb[MEM_IDX_STILL_VIEW_PF_JPG].size = (UINT32)K_SDRAM_PvPFJpgSize;        //photoframe jpg
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = 0;
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = 0;
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size = 0;
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size = 0;
	//#ifdef ICAT_OPTION	
	//	pMemCb[MEM_IDX_ICAT_TEST].size = icat_TestBuf_SIZE;
	//#endif

	/* video view mode */
	pMemCb[MEM_IDX_VIDEO_VIEW_AE].size = (UINT32)K_SDRAM_AviAeSize;
	pMemCb[MEM_IDX_VIDEO_VIEW_AE_WIN].size = (UINT32)25*8; 
	#if 0
	#if FRAME_BUF_NUM==4
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = (UINT32)12*12*2;        //TQ: dist Mv output buffer
	#else
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)0;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)0;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = (UINT32)0;        //TQ: dist Mv output buffer
	#endif
	#endif
	#if 1
	pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
	pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
	pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = K_SDRAM_VideoDistMeSize;        //TQ: dist Mv output buffer
	#endif 
	#if 0
	if(vidRecFrameBufNumGet()==4){
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = (UINT32)12*12*2;        //TQ: dist Mv output buffer
		}
	else{
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)0;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)0;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = (UINT32)0;        //TQ: dist Mv output buffer
		}
	#endif
	pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV2_A].size = K_SDRAM_Avi2BufSizeA;  //will: preview second path
	pMemCb[MEM_IDX_VIDEO_VIEW_PV2_B].size = K_SDRAM_Avi2BufSizeB;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV2_C].size = K_SDRAM_Avi2BufSizeC;
	pMemCb[MEM_IDX_VIDEO_VIEW_PV2_D].size = K_SDRAM_Avi2BufSizeD;
	//pMemCb[MEM_IDX_VIDEO_VIEW_FD].size = (UINT32)K_SDRAM_FdIIBufSize;        //jg:FD jobQueue,tbl and II tmpbuf 

	/* video record mode */
	//vidMemSizeInit();
	#if 0
	pMemCb[MEM_IDX_VIDEO_REC_VLC_A].size = K_SDRAM_AviCapVLCBufSizeA;
	pMemCb[MEM_IDX_VIDEO_REC_VLC_B].size = K_SDRAM_AviCapVLCBufSizeB;
	pMemCb[MEM_IDX_VIDEO_REC_VLC_C].size = K_SDRAM_AviCapVLCBufSizeC;
	pMemCb[MEM_IDX_VIDEO_REC_VLC_D].size = K_SDRAM_AviCapVLCBufSizeD;
	
	for(i = 4; i < VLCBUFNUM_TOTAL; i ++){
		pMemCb[MEM_IDX_VIDEO_REC_VLC_A + i].size = 0;
	}
	
	pMemCb[MEM_IDX_VIDEO_REC_VLC_TMP].size = K_SDRAM_AviCapVLCBufSizeA;
	pMemCb[MEM_IDX_VIDEO_REC_QTAB].size = K_SDRAM_AviCapQTabSize;
	pMemCb[MEM_IDX_VIDEO_REC_AUDIO].size = K_SDRAM_AviCapAudioBufSize;
	pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA;
	pMemCb[MEM_IDX_VIDEO_REC_HDR].size = K_SDRAM_AviCapHeaderBufSize;
	pMemCb[MEM_IDX_VIDEO_REC_IDX].size = -1; //for all free memory
	#endif
	vidRecMemSizeInit();
	/* still playback mode */
	
	pMemCb[MEM_IDX_STILL_PB_DISP].size = K_SDRAM_ImagePlaybackDisplayBufSize;
	pMemCb[MEM_IDX_STILL_PB_GEN_A].size = K_SDRAM_ImagePlaybackBufSize;
	pMemCb[MEM_IDX_STILL_PB_GEN_B].size = -1;//for all free memory

	/* video playback mode */
	
	pMemCb[MEM_IDX_VIDEO_PB_GEN_A].size = K_SDRAM_AviPbBufSizeA;
	pMemCb[MEM_IDX_VIDEO_PB_GEN_B].size = K_SDRAM_AviPbBufSizeB;
	pMemCb[MEM_IDX_VIDEO_PB_GEN_C].size = K_SDRAM_AviPbBufSizeC;
	pMemCb[MEM_IDX_VIDEO_PB_FRAG].size = 0;//variable size  K_SDRAM_AviPbFragSize;
	pMemCb[MEM_IDX_VIDEO_PB_VIDEO].size = K_SDRAM_AviPbVideoBufSize;
	pMemCb[MEM_IDX_VIDEO_PB_AUDIO].size = K_SDRAM_AviPbAudioBufSize;
	pMemCb[MEM_IDX_VIDEO_PB_AUDIO_CHUNK].size = K_SDRAM_AviPbAudioChunkBufSize;
	pMemCb[MEM_IDX_VIDEO_PB_IDX].size = -1;//for all free memory

	varSize = sgMemCb[MEM_IDX_GLOBAL].size; // total size
	for(i = MEM_IDX_COMMON_START; i < MEM_IDX_COMMON_END; i ++){// common size
		varSize -= pMemCb[i].size;
	}
	for(i = MEM_IDX_VIDEO_PB_GEN_A; i < MEM_IDX_VIDEO_PB_IDX; i ++){
		varSize -= pMemCb[i].size;
	}
	varSize -= (UINT32)64*1024;	//reserved for index
	varSize -= (UINT32)180*1024;
	//vidPbFragBufSize should 2^n, may be not good, but...
	if(varSize & 0x400000){ //4M word
		varSize = 0x400000;
	}
	else if(varSize &0x200000){
		varSize = 0x200000;
	}
	else if(varSize &0x200000){
		varSize = 0x200000;
	}
	else{
		varSize = 0x100000;
	}
	pMemCb[MEM_IDX_VIDEO_PB_FRAG].size = varSize + (UINT32)180*1024;
}

//-----------------------------------------------------------------------------
//sysMemAddrUpdate
//-----------------------------------------------------------------------------
/**
 * @brief	System memory pool initialization
 * @param	NULL
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since	2009-01-09
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
void sysMemAddrUpdate(void)
{
	memCb_t xdata* pMemCb = &sgMemCb[0];
	pMemCb[MEM_IDX_COMMON_START].addr = 0;
	sysMemCbAddrSet(pMemCb, MEM_IDX_COMMON_START, MEM_IDX_COMMON_END, MEM_GROWTH_UP, 1);

	pMemCb[MEM_IDX_STILL_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_STILL_VIEW_START].size;
	//pMemCb[MEM_IDX_STILL_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_STILL_VIEW_START].size-(UINT32)(320*240);	
	sysMemCbAddrSet(pMemCb, MEM_IDX_STILL_VIEW_START, MEM_IDX_STILL_VIEW_END, MEM_GROWTH_DOWN, 1);
	pMemCb[MEM_IDX_VIDEO_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_VIDEO_VIEW_START].size;//-(UINT16)3200;
	sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_VIEW_START, MEM_IDX_VIDEO_VIEW_END, MEM_GROWTH_DOWN, 1);
	//pMemCb[MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
	//sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_REC_START, MEM_IDX_VIDEO_REC_END, MEM_GROWTH_UP, 1);
	vidRecMemAddrInit();
	pMemCb[MEM_IDX_STILL_PB_START].addr = K_SDRAM_CommonEndAddr;
	sysMemCbAddrSet(pMemCb, MEM_IDX_STILL_PB_START, MEM_IDX_STILL_PB_END, MEM_GROWTH_UP, 1);

	pMemCb[MEM_IDX_VIDEO_PB_START].addr = K_SDRAM_CommonEndAddr;
	sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_PB_START, MEM_IDX_VIDEO_PB_END, MEM_GROWTH_UP, 1);

	/* Code sentry setting */
	HAL_DramCodeSentry(0x00, K_SDRAM_CodeSize-1, K_SDRAM_CodeSentryRedirectAddr);
}
#if 0
void 
sysMemFrameBufAddrUpdate(
	UINT8 size
)
{
	memCb_t xdata* pMemCb = &sgMemCb[0];
	UINT8 vlcBufNum, vlcBufTotal, cnt;
	UINT32 vlcBufSize;
	extern UINT8 G_VideoVlcBufNum;
	UINT8 frameRate;
	//uiPara_t* puiPara ;
	//puiPara= appUiParaGet();
	//VLCBUFNUM_TOTAL=sysMemRemainSizeGet();
	vlcBufTotal = VLCBUFNUM_TOTAL; //max, if mem size > 8M
	vlcBufNum = VLCBUFNUM_TOTAL;
	//printf("MEM_IDX_VIDEO_REC_AUDIO=%ld\n",pMemCb[MEM_IDX_VIDEO_REC_AUDIO].size);
	switch(size){
		case MEM_FB_VGA:
			aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
			//printf("frame rate=%bd\n",frameRate);
			#if (VGA_60FPS_2ST_PATH_EN == 0)
			if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_VGA_60FPS;			

				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_A].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_B].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_C].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_D].size = 0;				
			}
			else
			#endif
			{
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA_VGA;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB_VGA;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_VGA;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_VGA;
			}	
			
			vlcBufSize = K_SDRAM_AviCapVLCBufSizeA_VGA;
			/*if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 20;			
			}*/
			pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA_VGA;
			break;
			
		case MEM_FB_D1:
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_D1;

			vlcBufSize = K_SDRAM_AviCapVLCBufSizeA_D1;
			/*if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 14;			
			}*/
			pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA_D1;
		break;
	    case MEM_FB_1080p:
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA_1080p;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB_1080p;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_1080p;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_1080p;	
			vlcBufSize = K_SDRAM_AviCapVLCBufSizeA_1080p;
			pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA_1080p;	
			break;
	   case MEM_FB_HD_960: //only support FRAME_BUF_NUM=2
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = 960L*540L;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = 960L*540L;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = 0;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = 0;
			/*if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 18;
			}*/
			
			vlcBufSize = K_SDRAM_AviCapVLCBufSizeA;
			pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA;
			break;
			
		case MEM_FB_HD:
		default:	
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD;

			vlcBufSize = K_SDRAM_AviCapVLCBufSizeA;

			/*#if (FRAME_BUF_NUM==2)
			if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 8;			
			}
			#else
			if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 10;
			}
			#endif*/
			pMemCb[MEM_IDX_VIDEO_REC_FRAG].size = K_SDRAM_AviCapFragBufSizeA;
		break;
	}
	vlcBufNum=sysMemRemainSizeGet(vlcBufSize);
	for(cnt = 0; cnt < vlcBufNum; cnt ++){
		pMemCb[MEM_IDX_VIDEO_REC_VLC_A+cnt].size = vlcBufSize;
	}
	for(; cnt < vlcBufTotal; cnt ++){
		pMemCb[MEM_IDX_VIDEO_REC_VLC_A+cnt].size = 0;
	}
	pMemCb[MEM_IDX_VIDEO_REC_VLC_TMP].size = 0;//edit by wangrf 
	//printf("ae ii size=%ld\n",pMemCb[MEM_IDX_VIDEO_VIEW_AE].size);
	
	//pMemCb[MEM_IDX_VIDEO_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_VIDEO_VIEW_START].size;
	sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_VIEW_DIST_ME, MEM_IDX_VIDEO_VIEW_END, MEM_GROWTH_DOWN, 1);
	pMemCb[MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
	sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_REC_START, MEM_IDX_VIDEO_REC_END, MEM_GROWTH_UP, 1);

	G_VideoVlcBufNum =vlcBufNum;
	printf("vlcNum:%bu, %lx, %lx\n",vlcBufNum, vlcBufSize, K_SDRAM_TotalSize);
	//printf("addr:%lx, %lx\n", pMemCb[MEM_IDX_VIDEO_REC_VLC_A].addr, pMemCb[MEM_IDX_VIDEO_REC_FRAG].addr);
	//printf("addr:%lx, %lx\n", pMemCb[MEM_IDX_VIDEO_REC_HDR].addr, pMemCb[MEM_IDX_VIDEO_VIEW_END].addr);
	//printf("addr:%lx, %lx\n", pMemCb[MEM_IDX_VIDEO_REC_VLC_A].addr, pMemCb[MEM_IDX_VIDEO_REC_FRAG].addr);
}
#endif
void
sysMemStillPbDispAddrUpdate(UINT8 HDMI_On){
	UINT8 i;
	memCb_t xdata* pMemCb = &sgMemCb[0];
	//printf("HDMI: %bu\n", HDMI_On);
	
	if (HDMI_On){
		pMemCb[MEM_IDX_STILL_PB_DISP].size = K_SDRAM_ImagePlaybackDisplayBufSize_HDMI;
	}
	else {
		pMemCb[MEM_IDX_STILL_PB_DISP].size = K_SDRAM_ImagePlaybackDisplayBufSize;
	}
	pMemCb[MEM_IDX_STILL_PB_START].addr = K_SDRAM_CommonEndAddr;
	sysMemCbAddrSet(pMemCb, MEM_IDX_STILL_PB_START, MEM_IDX_STILL_PB_END, MEM_GROWTH_UP, 1);
	//printf("\n3\n");
	for(i=MEM_IDX_STILL_PB_START;i< MEM_IDX_STILL_PB_END;i++){
		//printf("pMemCb[i]=%lx\n",pMemCb[i].addr,pMemCb[i].size);
    }
}

static void sysMemCbAddrSet(
	memCb_t* pMemCb,
	memCbIdx_t sIdx,
	memCbIdx_t eIdx,
	UINT8 mode,
	UINT8 bAgn
)
{
	UINT8 i;
	//static UINT32 LastAddr=0;
	//UINT32 LastAddr=0;
	
	if (!pMemCb || sIdx > eIdx) {
		return;
	}
	
	for (i = sIdx + 1; i <= eIdx; i++) {
		if (mode == MEM_GROWTH_UP) {
			/* growth up */
			pMemCb[i].addr = pMemCb[i - 1].addr + pMemCb[i - 1].size;
		} else {
			/* growth down */
			pMemCb[i].addr = pMemCb[i - 1].addr - pMemCb[i].size;
		}

		/* alignment to 8 words */
		if (bAgn) {
			pMemCb[i].addr = (pMemCb[i].addr + 7) & ~7;
		}
		
		//printf(">>addr %08lx\n", (UINT32)pMemCb[i].addr);
		//printf(">>addr %08lx, size: %lx\n", (UINT32)pMemCb[i].addr, ((UINT32)pMemCb[i].addr-LastAddr)*2);
		//LastAddr = (UINT32)pMemCb[i].addr;
	}
}

void sysMemSizeSet(memCbIdx_t idx, UINT32 size)
{
	if (idx < MEM_IDX_MAX) {
		sgMemCb[idx].size = size;
	}
}

UINT32 sysMemSizeGet(memCbIdx_t idx)
{
	if (idx < MEM_IDX_MAX) {
		return sgMemCb[idx].size;
	} else {
		return UNKNOWN;
	}
}


UINT32 sysMemTotSizeGet(void)
{
	return sgMemCb[MEM_IDX_GLOBAL].size;
}

UINT32 sysMemTotSizeGet_Intr(void)
{
	return sgMemCb[MEM_IDX_GLOBAL].size;
}

UINT32 sysMemAddrGet(memCbIdx_t idx)
{
	if (idx < MEM_IDX_MAX) {
		return sgMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}

UINT32 sysMemAddrGet_Intr(memCbIdx_t idx)
{
	if (idx < MEM_IDX_MAX) {
		return sgMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}

static void sysMemSizeInit_Global(void)
{
	UINT32* pSize = &(sgMemCb[MEM_IDX_GLOBAL].size);
	UINT16 dramSize;
	
	switch (XBYTE[REG_Dram_SdramType]) {		
		case 0x00:
			*pSize = DEV_MEM_SIZE_16M;
			break;
		case 0x02:
			*pSize = DEV_MEM_SIZE_64M;
			break;
		case 0x03:
			*pSize = DEV_MEM_SIZE_128M;
			break;
		case 0x04:
			*pSize = DEV_MEM_SIZE_256M;
			break;
		case 0x05:
			*pSize = DEV_MEM_SIZE_512M;
			break;
		default:
			*pSize = DEV_MEM_SIZE_64M;
			break;
	}

	dramSize = READP16(pSize, 0) >> 4;
	printf("DRAM: %ux16Mb@%uMHz\n", dramSize, 522/(XBYTE[0x2025]+1));
}
#if 0
UINT32 sysMemRemainSizeGet(UINT32 vlcBufSize)
{
	UINT32 remainSize;
	UINT32 remainAddrEnd,remainAddrStart;
	UINT32 Q_IdxSize=0,DistME_VidViewPVASize=0;
	UINT32 vlcnum=0;
	UINT16 i;
	memCb_t xdata* pMemCb = &sgMemCb[0];
	
	for(i=MEM_IDX_VIDEO_REC_QTAB;i<MEM_IDX_VIDEO_REC_IDX;i++)
		{
			Q_IdxSize+=pMemCb[i].size;
		}
	Q_IdxSize+=((UINT32)FRAG_SIZE_IDX * 5);
	//printf("++++Q_IdxSize=%lx\n",Q_IdxSize);
	//Q_IdxSize=pMemCb[MEM_IDX_VIDEO_REC_QTAB].size+pMemCb[MEM_IDX_VIDEO_REC_AUDIO].size+pMemCb[MEM_IDX_VIDEO_REC_FRAG].size+pMemCb[MEM_IDX_VIDEO_REC_HDR].size+(UINT32)FRAG_SIZE_IDX;
   	 for(i=(MEM_IDX_VIDEO_VIEW_DIST_ME)+1;i< MEM_IDX_VIDEO_VIEW_END;i++)
    	{
    		DistME_VidViewPVASize+=pMemCb[i].size;
			//printf("pMemCb[i]=%lx",pMemCb[i].size);
    	}
	if(DistME_VidViewPVASize>(sysMemAddrGet(MEM_IDX_VIDEO_VIEW_DIST_ME)-sysMemAddrGet(MEM_IDX_COMMON_END)))
		{
		   vlcnum=0;
		   return vlcnum;
		}
   	 remainAddrEnd=sysMemAddrGet(MEM_IDX_VIDEO_VIEW_DIST_ME)-DistME_VidViewPVASize;
	remainAddrEnd=remainAddrEnd-Q_IdxSize-videoRecStampSizeGet();
	//printf("++++remainAddrEnd=%lx\n",remainAddrEnd);
	remainAddrStart = sysMemAddrGet(MEM_IDX_COMMON_END);
	if(remainAddrEnd <= remainAddrStart)
		{
		  vlcnum=0;
		  return vlcnum;
		}
	else
		{
			remainSize=remainAddrEnd - remainAddrStart;
			//printf("remainSize=%lx\n",remainSize);
			vlcnum=remainSize/vlcBufSize;
			//printf("pMemCb[MEM_IDX_VIDEO_REC_VLC_A].size=%lx\n",vlcBufSize);
		}
	//printf("MEM_IDX_MAX=%bd\n",MEM_IDX_MAX);
	if(MEM_IDX_MAX>255)
		{
		   printf("vlc num is to big\n");
		   vlcnum= vlcnum-(MEM_IDX_MAX-255);
		}
	if(vlcnum>80)
		{
			vlcnum=80;
		}
	//printf("++++vlcnum=%lx\n",vlcnum);
	return vlcnum;
}
#endif

void sysMemStillPvPanoramaAddrUpdate(UINT8 size,UINT16 pvHsize,UINT16 pvVsize,UINT8 frameNum,UINT16 dstHsize,UINT16 dstVsize) 
{
	memCb_t xdata* pMemCb = &sgMemCb[0];

	#if 0
	printf("size(%lx,%lx,%lx)\n",
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size,
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size,
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size);
	#endif

	#if 0
	printf("dstH %d,%d\n",dstHsize,dstVsize);
	#endif
	
	#if 1
	switch(size){	
		
		case MEM_FB_QVGA:
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
			break;
		case MEM_FB_VGA:
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
			break;
		case MEM_FB_D1:
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
			break;
		case MEM_FB_1080p:
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
			break;
		case MEM_FB_HD_960:
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
			break;
		 case MEM_FB_HD:
		 
		 default:
		 	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
		 	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
			pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;
	}
	#else
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = (UINT32)12*12*2*frameNum;
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].size = (UINT32)4*(frameNum+2);
 	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size =(UINT32)dstHsize*dstVsize;
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size =(UINT32)pvHsize*pvVsize*frameNum;	
	#endif

	#if 0
	printf("size(%lx,%lx,%lx)\n",
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size,
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].size,
		pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].size);
	#endif

	#if (DBG_DRAMDMA_ME_BY_FRAME == 0)
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].size = 0;
	#endif

	pMemCb[MEM_IDX_STILL_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_STILL_VIEW_START].size;
	sysMemCbAddrSet(pMemCb, MEM_IDX_STILL_VIEW_START, MEM_IDX_STILL_VIEW_END, MEM_GROWTH_DOWN, 1);

	#if 0
	{
		UINT8 i;
		for (i=0;i<MEM_IDX_MAX;i++){
			printf("[%bu]%lx,%lx\n",i,sgMemCb[i].addr,sgMemCb[i].size);
		}
	}
	#endif

	#if 0
	printf("BUF(0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)\n",
	pMemCb[MEM_IDX_COMMON_END].addr,
	pMemCb[MEM_IDX_STILL_VIEW_START].addr,
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV].addr,
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME].addr,
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV].addr,
	pMemCb[MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV].addr
	pMemCb[MEM_IDX_STILL_VIEW_END].addr);
	#endif
}

void sysMemVideoPvAddrUpdate(UINT8 size,UINT16 pvHsize,UINT16 pvVsize)
{
	memCb_t xdata* pMemCb = &sgMemCb[0];
	//UINT16 recHsize,recVsize;
	UINT8 frameRate;
	UINT8 framebufnum;
	framebufnum=vidRecFrameBufNumGet();
	
	switch(size){	
		case MEM_FB_VGA:
			aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);
			#if (VGA_60FPS_2ST_PATH_EN == 0)
			if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = K_SDRAM_AviPvwBufSizeA_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = K_SDRAM_AviPvwBufSizeB_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_VGA_60FPS;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_VGA_60FPS;			

				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_A].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_B].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_C].size = 0;
				//pMemCb[MEM_IDX_VIDEO_VIEW_PV2_D].size = 0;				
			}
			else
			#endif
			//vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size =(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/12+136;;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size =(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size =(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA;	
			break;
		case MEM_FB_D1:
			//vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size = (UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/12+136;;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = (UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = (UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1;
			break;
		case MEM_FB_1080p:
			//vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size = (UINT32)K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12+136;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
			break;
		case MEM_FB_HD_960: //only support FRAME_BUF_NUM=2
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = 960L*540L;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = 960L*540L;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = 960L*540L;
			pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = 960L*540L;
			/*if (K_SDRAM_TotalSize<0x800000){
				vlcBufNum = 18;
			}*/
			break;
		 case MEM_FB_HD: //only support FRAME_BUF_NUM=2
		 default:
		 	//vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136;
		 	if(sp1kHDPvsizeStatusGet()){//pvsize=8/8
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
				pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
		 	}else{
				if((G_UIStatus & 0xff00) == 0x2100 /*K_UISTATUS_USBMODE_PCCAM*/)
				{
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = (UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT;
				}
				else{
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size = 960L*540L;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_B].size = 960L*540L;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = 960L*540L;
					pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = 960L*540L;
				}
		 	}
			break;
	}
	if(framebufnum==2){
		pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size =0;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =0;
			//vidpMemCb [VID_MEM_IDX_VIDEO_REC_FRAG].size = ((UINT32)112 * (UINT32)1024 * (UINT32)2 + vidpMemCb[MEM_IDX_VIDEO_REC_VLC_A].size);
	}
	else if(framebufnum==3){
		pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size =0;
	}
	#if (VGA_60FPS_2ST_PATH_EN == 0)
	if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
		pMemCb[MEM_IDX_VIDEO_VIEW_PV_C].size = K_SDRAM_AviPvwBufSizeC_VGA_60FPS;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV_D].size = K_SDRAM_AviPvwBufSizeD_VGA_60FPS;						
	}
	#endif
	secondPathPvMenSizeSet();
	if(framebufnum==3){
		pMemCb[MEM_IDX_VIDEO_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_VIDEO_VIEW_START].size;//-(UINT16)3200;
		sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_VIEW_START, MEM_IDX_VIDEO_VIEW_END, MEM_GROWTH_DOWN, 1);
	}else{
		sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_VIEW_DIST_ME, MEM_IDX_VIDEO_VIEW_END, VID_MEM_GROWTH_DOWN, 1);
	}
	if((pvHsize*pvVsize)>(pMemCb[MEM_IDX_VIDEO_VIEW_PV_A].size)){
		ASSERT(0, 1);
	}
}
void secondPathPvMenSizeSet(void)
{
	memCb_t xdata* pMemCb = &sgMemCb[0];
	if(vidRecFrameBufNumGet()==3){
		pMemCb[MEM_IDX_VIDEO_VIEW_AE].size = (UINT32)K_SDRAM_AviAeSize;
		pMemCb[MEM_IDX_VIDEO_VIEW_AE_WIN].size = (UINT32)25*8; 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = K_SDRAM_VideoDistMeSize;        //TQ: dist Mv output buffer
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_A].size = K_SDRAM_Avi2BufSizeA;  //will: preview second path
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_B].size = K_SDRAM_Avi2BufSizeB;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_C].size = K_SDRAM_Avi2BufSizeC;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_D].size = 0;
	}else{
		pMemCb[MEM_IDX_VIDEO_VIEW_AE].size = (UINT32)K_SDRAM_AviAeSize;
		pMemCb[MEM_IDX_VIDEO_VIEW_AE_WIN].size = (UINT32)25*8; 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINA].size = (UINT32)12*12*4;        //will: edge window bufferA 
		pMemCb[MEM_IDX_VIDEO_VIEW_EDGE_WINB].size = (UINT32)12*12*4;        //will: edge window bufferB 
		pMemCb[MEM_IDX_VIDEO_VIEW_DIST_ME].size = K_SDRAM_VideoDistMeSize;        //TQ: dist Mv output buffer
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_A].size = K_SDRAM_Avi2BufSizeA;  //will: preview second path
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_B].size = K_SDRAM_Avi2BufSizeB;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_C].size = K_SDRAM_Avi2BufSizeC;
		pMemCb[MEM_IDX_VIDEO_VIEW_PV2_D].size = K_SDRAM_Avi2BufSizeD;
	}
	//pMemCb[MEM_IDX_VIDEO_VIEW_START].addr = K_SDRAM_TotalSize - pMemCb[MEM_IDX_VIDEO_VIEW_START].size;//-(UINT16)3200;
	//sysMemCbAddrSet(pMemCb, MEM_IDX_VIDEO_VIEW_START, MEM_IDX_VIDEO_VIEW_END, MEM_GROWTH_DOWN, 1);
}
