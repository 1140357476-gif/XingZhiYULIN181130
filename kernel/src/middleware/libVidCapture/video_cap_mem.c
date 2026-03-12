/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  video_cap_mem.c
 * @brief	  video  memory initialization related function
 * @author	  hy.ren
 * @since	  2011-07-29
 * @date	  2011-07-29
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "initio.h"
#include "sp1k_video_api.h"
#include "video_mem.h"
#include "video_cap_mem.h"
#include "video_capture.h"

#define __FILE __FILE_ID_VIDEO_MEM__
 /**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 g_VidCapVlcBufNum;
static vidMemCb_t xdata speedyCaptureMemCb[VID_MEM_IDX_MAX];
#define HDR_SIZE (UINT32)65536

 /**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void speedyCaptureMemSizeInit(void)
{
	UINT8 i;
	vidMemCb_t xdata* vidpMemCb = &speedyCaptureMemCb[0];
	
	/* video record mode */
	for(i = 4; i < VID_VLCBUFNUM_TOTAL; i ++){
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A+i,0);
	}
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_QTAB,0);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_AUDIO,0);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_FRAG,(vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size*2 + 1024ul*64));
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_HDR,0);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampA,(UINT32)720* 48);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampB,(UINT32)720* 48);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampC,(UINT32)720* 48);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampD,(UINT32)720* 48);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_Font,(UINT32) 24*48*21);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_Text,(UINT32) 24*48*20*4);	
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_IMG,(UINT32) 720* 48);
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_IDX,0);
}
  

void speedyCaptureMemAddrInit(void)
{
  	vidMemCb_t xdata* vidpMemCb = &speedyCaptureMemCb[0];
	vidpMemCb[VID_MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
  	vidRecMemAddrSet(vidpMemCb, VID_MEM_IDX_VIDEO_REC_START, VID_MEM_IDX_VIDEO_REC_END, VID_MEM_GROWTH_UP, 1);	
}
 
void speedyCaptureMemFrameBufSizeUpdate(void)
{
	UINT8 i;
	UINT8 imgSize;
	vidMemCb_t xdata* vidpMemCb = &speedyCaptureMemCb[0];
	
	speedyCaptureMemSizeInit();
	imgSize = speedyCaptureParamGet(VD_CAP_SIZE);
	
	if(imgSize == VD_CAP_VGA){
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/4 + HDR_SIZE);
	}
	else if(imgSize == VD_CAP_D1 ){
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/4 + HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/4 + HDR_SIZE);
	}
	else if(imgSize == VD_CAP_HD ){
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/4+ HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/4+ HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/4+ HDR_SIZE);
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/4+ HDR_SIZE);
	}
	
	for(i = 4; i < VID_VLCBUFNUM_TOTAL; i ++){
		speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A+i,0);
	}
	speedyCaptureMemSizeSet(VID_MEM_IDX_VIDEO_REC_FRAG,(vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size*4 + 1024ul*64));
	
}

void speedyCaptureMemFrameBufAddrUpdate(void)
{  	
	vidMemCb_t xdata* vidpMemCb = &speedyCaptureMemCb[0];
	UINT8 vlcBufNum, vlcBufTotal=VID_VLCBUFNUM_TOTAL, cnt;
	UINT32 vlcBufSize;
	UINT8 i;
	UINT32 regularsize=0;
	UINT32 endAddr;

	speedyCaptureMemFrameBufSizeUpdate();
	vidpMemCb[VID_MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
	vlcBufSize=speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	for(i=VID_MEM_IDX_VIDEO_REC_StampA;i<=VID_MEM_IDX_VIDEO_REC_FRAG;i++){
		regularsize+=speedyCaptureMemSizeGet(i);
	}
	regularsize += 1024; //reserved size for free buffer
	endAddr = sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D);
	vlcBufNum=speedyCaptureVlcBufNumGet(vlcBufSize,K_SDRAM_CommonEndAddr+regularsize,endAddr);
	
	for(cnt = 0; cnt <vlcBufNum; cnt ++){
		vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A+cnt].size = vlcBufSize;
	}
	//printf("cc=%bu\n",cnt);
	for(; cnt < vlcBufTotal; cnt ++){
		vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A+cnt].size = 0;
	}
	g_VidCapVlcBufNum = vlcBufNum;
	printf("vlc=%bu\n",vlcBufNum);
	
	vidRecMemAddrSet(vidpMemCb,VID_MEM_IDX_VIDEO_REC_START, VID_MEM_IDX_VIDEO_REC_END, VID_MEM_GROWTH_UP, 1);
	vidpMemCb[VID_MEM_IDX_VIDEO_REC_END].size = endAddr - speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_END);


}

UINT8 speedyCaptureFreeBufNumGet()
{
	return g_VidCapVlcBufNum;
}

UINT8 speedyCaptureVlcBufNumGet(UINT32 vlcBufSize,UINT32 vlcBufAddrStart,UINT32 vlcBufAddrEnd)
{
	UINT32 vlcBufNum;
	
	vlcBufNum=((vlcBufAddrEnd-vlcBufAddrStart)/vlcBufSize);
	if(vlcBufNum>VID_VLCBUFNUM_TOTAL){
		vlcBufNum=VID_VLCBUFNUM_TOTAL;
	}
	return vlcBufNum;
}

void speedyCaptureMemSizeSet(vidmemCbIdx_t idx, UINT32 size)
{
	if (idx < VID_MEM_IDX_MAX) {
		speedyCaptureMemCb[idx].size = size;
	}
}

UINT32 speedyCaptureMemSizeGet(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return speedyCaptureMemCb[idx].size;
	} else {
		return UNKNOWN;
	}
}

UINT32 speedyCaptureMemAddrGet(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return speedyCaptureMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}

UINT32 speedyCaptureMemAddrGet_Intr(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return speedyCaptureMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}
