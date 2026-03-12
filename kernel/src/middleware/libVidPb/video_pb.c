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
 * @file	  video_pb.c
 * @brief	  middle ware  API for video pb module
 * @author	  tq.zhang
 * @since	  2010-05-11
 * @date	  2010-05-11
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "common.h"
#include "hal.h"
#include "hal_dram.h"
#include "hal_cdsp.h"
#include "hal_global.h"
#include "dbg_def.h"
//#include "cardfunc.h"
#include "doslink.h"
//#include "dosusr.h"
#include "initio.h"
#include "quantdef.h"
#include "video_pb.h"
#include "hal_jpeg.h"
#include "os_api.h"
#include "os_main.h"
#include "pb_api.h"
#include "app_ui_osd.h"
#include "os_msg.h"
#include "app_main.h"
#include "func_ptr.h"
#include "pv_task.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "gpio.h"
#include "disp_api.h"
#include "sp1k_pv_api.h"
#include "awb_api.h"//wangjg++
#include "pb_api.h"
#include "dos32.h"
#include "snap_qtable.h"
#include "hal_gprm.h"

#include "main.h"
//#include "aviheader.h"
#include "hal_disp.h"
#if (FPGA_VERIFY == 1)
#include "cmd_func_fpga.h"
#endif
#include "hal_audio.h"
#include "hal_pb.h"

#include "vfs_api.h"
#include "dcf_api.h"

#include "video_debug.h"
#include "reg_def.h"

#if FUNC_KNL_VDPB_LIB_EN
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
// dec size = disp buf size
code vdPbSizeMatchTable_t vdTable_H4_V3[8] = {//disp 4:3, max buf 640*480
	{0, 320, 240}, //img: 320*240, disp: 320*240, dec: 320*240, fact: 0
	{0,640,480/*4, 320, 240*/}, //img: 640*480, disp: 320*240, dec: 320*240, fact: 4
	{0,720,480/*4, 424, 240*/}, //img: 848*480, disp: 424*240, dec: 424*240, fact: 4 //modify for D1 size
	//{0, 1280, 720},//img: 1280*720, disp: 1280*720, dec: 1280*720, fact: 0 
	{4, 640, 360},//img: 1280*720, disp: 1280*720, dec: 1280*720, fact: 0  // for HDMI drop frame
	{4, 960, 544},//img: 1920*1088, disp: 1280*720, dec: 960*544, fact: 4  // for HDMI drop frame
	{4, 512, 384},//img:1024*768
	{2, 400, 300},//img:1600*1200
};

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#undef	VID_DUG
#define __FILE	__FILE_ID_VIDEO_PB__

UINT32 IDX_START;	//read start , 512 aligment
UINT32 IDXHD_SIZE;	//index start offse from IDX_START
//#define IDX_START		0x200
//#define IDXHD_SIZE		8
#define IDXDATA_SHIFT	4 //16 bytes
#define IDX_BUF_SIZE	((((K_SDRAM_TotalSize-K_SDRAM_AviPbIdx1BufAddr)*2)>>9)<<9)
#define AVI_IDX_OFF(idx)	(((idx)<<IDXDATA_SHIFT)+IDX_START+IDXHD_SIZE)
#define AVI_512_ALIGN(x)   ((x)&0xfffffe00)

#define VD_UPSAMPLING_TEST	0
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
xdata UINT16 G_AVIWidth;
xdata UINT16 G_AVIHeight;
xdata UINT32 gPlayTime = 0;
xdata UINT32 VidPb_GT_zero = 0;
xdata UINT8 vid_flag=0;
xdata UINT32 recTime = 1;
xdata UINT8 preStatus = 0;

xdata UINT32 secTab[4] ={0};
xdata UINT8 fastFlg =0;
appVidPbCtrl_t xdata vidPbCtrl;
appVidPbCtrl_t xdata vidPbCtrl = {
	VID_PB_IDLE,
	0,	
	{0},
	VID_PB_SPEED_NORMAL
};

UINT32 secAddr_max =0;
UINT32 secAddr_min =0;

UINT32 secAddr_min_vid =0;
UINT32 secAddr_min_aud =0;

UINT32 AviHeaderSize = 512;
UINT16 VidHeaderSize = 260+HUFFMANSIZE;
UINT32 vidPbFragBufSize = ((UINT32)1024<<11);
UINT32 dataOffset=0;
UINT32 VlcRingBufInIdx = 0;
UINT32 VlcRingBufOutIdx = 0;
UINT32 bytePerSec;
UINT32 audChunkSize;
UINT32 aviFileLength;
UINT32 indexSize;
//xian ++ 20080919, for slow motion
UINT8 slowMode = 0;
UINT32 gPlayStartTime = 0;
//xian --
//xian remove 20100722
//UINT8 gSilence = 0; //xian ++ 20081211

UINT8 gJpgDecFact;
UINT32 gJpgDecAddrOff;

UINT32 gVdJpgCnt = 0;
UINT32 gVdDispCnt = 0;

UINT8 resumeAud = 0;
UINT8 gJpgPiroSv;
UINT8 G_VPBupSmpflag = 0;

UINT32 gAviFileSize;
UINT32 g_msPerFrame_fraction;
//UINT8 tlsFile = 0; //hy.ren 20111009
UINT8 nonAudioFile =0;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern void audioPbSet(UINT32 audAddr,UINT32 audBufSize,UINT16 Format, UINT32 playFreq, UINT32	Block,UINT8 channels,UINT8 );

extern xdata	UINT8		G_DOS_UseLinearFAT;
xdata UINT16   G_DOS_FileCurrCluster;
xdata UINT16   G_DOS_FileClusterIndex;

//extern	xdata UINT32   G_DOS_FileSizeKeep;
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
UINT32 GetIdxInBuf(UINT32 idx);
#ifdef VIDEO_PB_BRC_EN 
UINT8 videoPbQtabSet(UINT32 vlcAddr);
#endif
UINT8 videoPbDispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac);
void videoPbExit(void);
void updateRingBufIdx();

UINT8 videoPbDecode(UINT32 addrWord, UINT8 VLCFlip, UINT8 wait, UINT32 aviAddrEnd);
void videoFillAudRingBuf(UINT8 cnt);
void videoPbStreamRead(UINT16 fHandle, UINT32 pos, UINT32 maxSize);
void videoPbFast(void);
UINT8 HAL_DispSignalWait_Imem(UINT8 src, UINT8 mode);
void videoJpgWait(void);
void videoPbFreeBufSpeedup(void);

//=============================================================================
//Program
//=============================================================================
/**
 * @fn		  UINT32 videoPbAviLengthGet(void)
 * @brief	  get avi file length
 * @param	  NONE
 * @retval	  return = file length
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 videoPbAviLengthGet(void){
	return aviFileLength;
}
 //-----------------------------------------------------------------------------
//videoPbStreamRead
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbStreamRead(UINT16 fHandle, UINT32 pos)
 * @brief		Read stream from memory
 * @param	fHandle : file handle
 * @param	dataOffSet : data offset in file
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
 #if 0//!VD_PB_HDMI_EN
 void videoPbStreamRead(UINT16 fHandle, UINT32 pos)
{
	UINT32 VlcCount;
	videoPbVLCRingBufCntGet(&VlcCount);
	if(VlcCount>=2048) {
		VlcCount = ((VlcCount>>11)<<11);
		vfsFileRead(fHandle,	K_DOS_DstDram, VlcCount , K_SDRAM_AviPbFragAddr + (VlcRingBufInIdx>>1), pos);
		dataOffset += VlcCount ;
		VlcRingBufInIdx += VlcCount;
		secAddr_max =  dataOffset - AviHeaderSize; //xian 20081015
	} else {
		//printf("buf full\n");
		return;
	}

	if(VlcRingBufInIdx > vidPbFragBufSize) {
		VlcRingBufInIdx = VlcRingBufInIdx&(vidPbFragBufSize-1);
		HAL_GprmDramDma((K_SDRAM_AviPbFragAddr <<1) + vidPbFragBufSize, (K_SDRAM_AviPbFragAddr <<1), VlcRingBufInIdx, 1);
	}

}
#endif
//-----------------------------------------------------------------------------
//videoPbVLCRingBufCntGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoPbVLCRingBufCntGet(UINT32 *cnt)
 * @brief		Get VLC buffer remain space
 * @param	*cnt : buffer space
 * @retval	0:VlcRingBufOutIdx <= VlcRingBufInIdx  1: VlcRingBufOutIdx > VlcRingBufInIdx
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
UINT8 videoPbVLCRingBufCntGet(UINT32 *cnt)
{
	UINT8 sts = 0;

	if(VlcRingBufOutIdx <= VlcRingBufInIdx) {
		*cnt =vidPbFragBufSize -(VlcRingBufInIdx - VlcRingBufOutIdx);
		if((*cnt == vidPbFragBufSize) &&  secAddr_max) {
			*cnt = 0;
		} //xian ++ 20081015
		else if(VlcRingBufOutIdx >= ((UINT32)K_SDRAM_AviPbFragSize<<1) - vidPbFragBufSize){
			*cnt = ((UINT32)K_SDRAM_AviPbFragSize<<1) - VlcRingBufInIdx;
		}
	} else {
		sts = 1;
		*cnt =VlcRingBufOutIdx - VlcRingBufInIdx;
	}
		
	return	sts;
}

//-----------------------------------------------------------------------------
//videoPb
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPb(void)
 * @brief		video play
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
#if 0//!VD_PB_HDMI_EN
void videoPb(void) USING_0
{
	UINT8 i;
	UINT32 AudBufCnt, VlcBufCnt;
	UINT32 time =0;
	UINT32 tmpTime =0;
	UINT8 sts;
	UINT32 srchVidStep;

	if(XBYTE[REG_Audio_AudBufUnf] & 0x01){
		printf("data over\n");
		XBYTE[REG_Audio_AudBufUnf] &= 0x01;
	}
	if(vidPbCtrl.Idle != VID_PB_RUN) {
		if(vidPbCtrl.Idle == VID_PB_PAUSE) {
			return;
		}

		if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL) {
			if(vidPbCtrl.Idle == VID_PB_FW && !slowMode) {//xian 20080919, add slowMode
				videoPbFW();
				if((vidPbCtrl.lastVidIdx == 0) || (vidPbCtrl.lastVidIdx >= vidPbCtrl.chunckCnt)) {
					videoPbAbort();
					return;
				}
			} else if(vidPbCtrl.Idle == VID_PB_RW) {
				videoPbFR();
				if(vidPbCtrl.lastVidIdx == 0) {
					videoPbAbort();
					return;
				}
			}
		}

	}
	HAL_AudRingBufCntRead(&AudBufCnt );
	if(AudBufCnt < AUDBUFSIZE/2 ) {
		vidPbCtrl.feedData = 1;
	}	
	if(!vidPbCtrl.audDown) {
		if( ((vidPbCtrl.feedData == 1) || (!vidPbCtrl.playAudFlg )) && (!vidPbCtrl.audDown) &&(vidPbCtrl.speed == VID_PB_SPEED_NORMAL)){

			vidPbCtrl.firstVidBlock = 1;

			if(vidPbCtrl.playAudFlg){
				G_DOS_FileCurrCluster = (UINT16)vidPbCtrl.lastAudCluster;
				G_DOS_FileClusterIndex = (UINT16)vidPbCtrl.lastAudFileIdx;
			}

			if(!vidPbCtrl.playAudFlg) {
				//for non-audio	AVI
				HAL_AudPause();
				audioPbSet(vidPbCtrl.audAddr ,(UINT32)AUDBUFSIZE, (UINT16)vidPbCtrl.audFmt, (UINT32)vidPbCtrl.audFrq, (UINT32)vidPbCtrl.audAlign, (UINT8)vidPbCtrl.audChnnel,(UINT8)vidPbCtrl.audBit);
				for(i = 8;i>0;i--) {
					vidPbCtrl.nextAudIdx  = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1,1);
					if(vidPbCtrl.nextAudIdx ) {//if audio only, 0th chunck == 01wb
						sts = videoPbAudioChunck(vidPbCtrl.AVIIdxAddr,vidPbCtrl.audAddr ,vidPbCtrl.nextAudIdx);
						if(sts == SUCCESS) {
							vidPbCtrl.audChunkCnt ++;
							vidPbCtrl.lastAudIdx = vidPbCtrl.nextAudIdx;
						} else	{
							break;
						}
					} else{
						vidPbCtrl.audDown = 1;
						break;
					}
					if(secAddr_min_aud>secAddr_min_vid){
						if(secAddr_min_aud-secAddr_min_vid>819200)break;
					}					
				}
				HAL_AudRingBufCntRead(&AudBufCnt );
				//printf("audData:%ldK\n",AudBufCnt/1024);
				vidPbCtrl.playAudFlg  = 1;
//xian remove 20100722
//				if(gSilence){//xian ++ 20081211
//					HAL_GlobalReadGTimer(&gPlayStartTime);
//					gPlayTime = gPlayStartTime;
//				}
//				else
					HAL_AudResume();
			} else {

			for(i=0;i< 8;i++) {
				HAL_AudRingBufCntRead(&AudBufCnt );
				if(AudBufCnt > AUDBUFSIZE/2) {
					break;
				}	
				//xian -- 20081015, vidPbCtrl.feedData = 0;
				vidPbCtrl.nextAudIdx  = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1,1);
				if(i == 0) {
					if(vidPbCtrl.nextAudIdx ) {//if audio only, 0th chunck == 01wb
						sts = videoPbAudioChunck(vidPbCtrl.AVIIdxAddr,vidPbCtrl.audAddr ,vidPbCtrl.nextAudIdx);
						if(sts == SUCCESS) {
							vidPbCtrl.audChunkCnt ++;
							vidPbCtrl.lastAudIdx = vidPbCtrl.nextAudIdx;
							vidPbCtrl.feedData = 0; //xian ++ 20081015, read aud success
						} else {
							break;
						}
					} else{
						vidPbCtrl.audDown = 1;
						break;
					}
				} else if(vidPbCtrl.nextAudIdx == vidPbCtrl.lastAudIdx + 1) {
					sts = videoPbAudioChunck(vidPbCtrl.AVIIdxAddr,vidPbCtrl.audAddr ,vidPbCtrl.nextAudIdx);
					if(sts == SUCCESS) {
						vidPbCtrl.audChunkCnt ++;
					vidPbCtrl.lastAudIdx = vidPbCtrl.nextAudIdx;
					} else {
						break;
					}
				}else {
					break;
				}

			}
			}
			//xian -- 20081015, vidPbCtrl.feedData = 0;
			vidPbCtrl.lastAudCluster = (UINT32)G_DOS_FileCurrCluster;
			vidPbCtrl.lastAudFileIdx = (UINT32)G_DOS_FileClusterIndex;
		} //end if
		}
		if( !vidPbCtrl.vidDown) {
			if(!vid_flag) {
				if(vidPbCtrl.Idle == VID_PB_RW) {
					vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr , vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt, 1, 0);

				}else {
					srchVidStep = vidPbCtrl.TimeExceed / vidPbCtrl.msPerFrame + 1; //xian ++ 20081231, #34616
					vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr ,vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt ,srchVidStep, 0);
				}

				if(vidPbCtrl.NextVidIdx || (vidPbCtrl.mediaType == 0)) {
					sts = videoPbFrameRead(vidPbCtrl.AVIIdxAddr , vidPbCtrl.fHandle,vidPbCtrl.NextVidIdx , vidPbCtrl.VLCFlip);
					if(sts == SUCCESS) {
						if(vidPbCtrl.dummyFrame ==0) {
						vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
						}
					}else {
					
						//printf("%d\n",(UINT16)__LINE__);
						vidPbCtrl.dummyFrame =0;
						HAL_AudRingBufCntRead(&AudBufCnt );
						if(AudBufCnt <= 64) {
							//printf("%d\n",(UINT16)__LINE__);
							ASSERT(0, 1);
							videoPbAbort();
							return;
						}
						return ;
					}
				} else {
					//printf("%d\n",(UINT16)__LINE__);
					ASSERT(0, 1);
					videoPbAbort();
					return;
				}
			}
			videoPbMsGet(&time);
			vidPbCtrl.vidFrameTime = videoPbTimeGet();
			vidPbCtrl.vidFrameTime =(UINT32)(vidPbCtrl.vidFrameTime << 8);
			//xian ++, maybe force vid go ahead because aud data short
			HAL_AudRingBufCntRead(&AudBufCnt );
			videoPbVLCRingBufCntGet( &VlcBufCnt );
			if(!(vidPbCtrl.feedData && AudBufCnt < 64 && //aud data short
				VlcBufCnt < 2048 && //buf full
				sts //vid data next frame ready
				)){
				vidPbCtrl.feedData = 0;
			}			 
			//xian --

			if(((vidPbCtrl.vidFrameTime >= (time - vidPbCtrl.msPerFrame )) && (!vidPbCtrl.vidDown)) 
				|| (vidPbCtrl.speed != VID_PB_SPEED_NORMAL && !slowMode)
				|| vidPbCtrl.feedData) { //xian 20081015
				tmpTime = (time>>8)/1000;
				if((recTime) != tmpTime) {
					uiUpdateVideoRecTime(tmpTime);
					recTime = tmpTime;
				}

				if(vidPbCtrl.vidFrameTime < time) {
					vidPbCtrl.TimeExceed = 0;
				}else {
					vidPbCtrl.TimeExceed =	vidPbCtrl.vidFrameTime	- time;
				}

				if(vidPbCtrl.firstVidBlock)
				{
					G_DOS_FileCurrCluster = (UINT16)vidPbCtrl.lastVidCluster;
					G_DOS_FileClusterIndex	= (UINT16)vidPbCtrl.lastVidFileIdx;
					vidPbCtrl.firstVidBlock  = 0;
				}
				//printf("idx:%ld\n",vidPbCtrl.NextVidIdx);

					if(((vidPbCtrl.TimeExceed <= (vidPbCtrl.msPerFrame )) || (vidPbCtrl.speed != VID_PB_SPEED_NORMAL))&&((!vidPbCtrl.dummyFrame)||(vidPbCtrl.speed != VID_PB_SPEED_NORMAL))) {
						#if 0
						videoPbFrame(vidPbCtrl.AVIIdxAddr ,vidPbCtrl.fHandle ,vidPbCtrl.NextVidIdx ,vidPbCtrl.VLCFlip);
						#endif
						vidPbCtrl.dummyFrame = 0;
						if(vidPbCtrl.VLCFlip==0) {
							HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrA);
						}else if(vidPbCtrl.VLCFlip==1){
							HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrB);
						}else {
							HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrC);
						}						
						gVdDispCnt ++;
						if(!vidPbCtrl.showBG) {
							//if(G_AVIWidth == 320 ) {
								//while((XBYTE[REG_Disp_PnlSel] & 0x04) != 0x04);
								//while((XBYTE[REG_Disp_PnlSel] & 0x04) == 0x04);
								//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
							//}
							HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);							
						}
						if(vidPbCtrl.VLCFlip==0){
							vidPbCtrl.VLCFlip = 1 ;
						} else if(vidPbCtrl.VLCFlip==1){
							vidPbCtrl.VLCFlip = 2 ;
						} else {
							vidPbCtrl.VLCFlip = 0 ;
						}
						//when	read video chunck, record file position

						vidPbCtrl.lastVidCluster = (UINT32)G_DOS_FileCurrCluster;
						vidPbCtrl.lastVidFileIdx = (UINT32)G_DOS_FileClusterIndex;

						//patch4.4@yichang@for valid signal begin
						if(vidPbCtrl.showBG) {
							videoPbDispScaleSet(G_AVIWidth,G_AVIHeight,100); //xian 20090915
							//while((XBYTE[REG_Disp_PnlSel] & 0x04) != 0x04);
							//while((XBYTE[REG_Disp_PnlSel] & 0x04) == 0x04);
							//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING); //phil: No waiting needed, use the internal sync!
								SetPbBufHsize((UINT16)G_AVIWidth);

							//xian remove 20100722
							//if(!gSilence){	 //xian ++ 20081211
								HAL_AudResume();
							//}
							vidPbCtrl.showBG = 0;
						}

						if(((preStatus == VID_PB_RW) || (preStatus == VID_PB_FW)) && (vidPbCtrl.speed == VID_PB_SPEED_NORMAL 
							/*&& !gSilence*/)) {//xian ++ 20081211
								HAL_AudResume();
						}

						if(vidPbCtrl.mediaType == 0) {
							vidPbCtrl.mediaType = 2;//for play 0th chunck of 00dc
						}

						//printf("%d\n",(UINT16)__LINE__);
					}else {
						if(!vidPbCtrl.NextVidIdx) {
							//printf("%d\n",(UINT16)__LINE__);
							videoPbAbort();
							return;
						} else if(vidPbCtrl.dummyFrame) {
							vidPbCtrl.dummyFrame = 0;
							//printf("dumy\n");	
						} else {
							//xian -- 20081015 ASSERT(0, 1);
						}
					}
					//printf("%d\n",(UINT16)__LINE__);
				if((!vidPbCtrl.NextVidIdx)&& vidPbCtrl.mediaType  && vidPbCtrl.showBG) {
					vidPbCtrl.vidDown = 1;
					//printf("%d\n",(UINT16)__LINE__);
					videoPbAbort();
					return;
				}
				vid_flag = 0;
				//printf("%d\n",(UINT16)__LINE__);
			}else {
				//printf("%d\n",(UINT16)__LINE__);
				//printf("T:%ld\n",vidPbCtrl.vidFrameTime);
				//printf("VT:%ld\n",time);
				vid_flag = 1;
				if(vidPbCtrl.vidDown){
					videoPbAbort();
					return;
				}
				if(vidPbCtrl.audDown){
				HAL_AudRingBufCntRead(&AudBufCnt );
					if(AudBufCnt <= 64) {
						//printf("%d\n",(UINT16)__LINE__);
						videoPbAbort();
						return;
					}
					//printf("audio done\n");
				}
				videoPbStreamRead(vidPbCtrl.fHandle, dataOffset);
				if((vidPbCtrl.speed != VID_PB_SPEED_NORMAL) && (fastFlg) && !slowMode) {//xian 20080919, add slowMode
					HAL_GlobalReadGTimer(&time);
					gPlayTime = time;
					fastFlg = 1 - fastFlg;
				}
				return;

			}


			if(vidPbCtrl.audDown) {
				HAL_AudRingBufCntRead(&AudBufCnt );
				if(AudBufCnt <= 64) {
					ASSERT(0, 1);
					videoPbAbort();
					return;
				} else {
					UINT32 aud_index;
					HAL_AudInIndexGet(&aud_index);
					HAL_GprmDramFill(vidPbCtrl.audAddr +aud_index/2, 2048, 0x00);
				}
			}

			if((vidPbCtrl.speed != VID_PB_SPEED_NORMAL) && (fastFlg)) {
				HAL_GlobalReadGTimer(&time);
				gPlayTime = time;
				fastFlg = 1 - fastFlg;
			}

	}else {
		videoPbAbort();
		return;
	}

	return;
}
#endif
//-----------------------------------------------------------------------------
//videoPbAbort
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbAbort(void)
 * @brief		video abort
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbAbort(void) USING_0
{
	UINT8 readMode = K_DOS_ReadFile_LinearFAT;
	UINT16 timeout=3000;// add timeout for #43427
	/*For HDMI beep Voice*/
	HAL_AudPause();//HAL_AudStop( 1 );

	ACTIVE_FUNC_CLR();
	vidPbCtrl.Idle = VID_PB_IDLE;
	vidPbCtrl.audInIdx = 0;
	G_VPBupSmpflag = 0; 	//for upsampling

#if 1//VD_PB_HDMI_EN
	while((XBYTE[REG_Jpeg_Intr_Event] & 0x04) == 0x00) {
		if (timeout<2) break;
		timeout--;
	}//wait decode finished

	XBYTE[REG_Jpeg_Swreset]=0x01;	 //xian ++ 20110602, #42727,  JPG software reset
	XBYTE[REG_Jpeg_Swreset]=0x00;	

	XBYTE[0x2002]=0x20;
	XBYTE[0x2002]=0x00;

	XBYTE[REG_Jpeg_Intr_Event]=0xFF;
#endif

	XBYTE[REG_Dram_Priority0_lvl2_master] = gJpgPiroSv;

	#if 1
	printf("dispCnt=%lu, jpgCnt=%lu\n", gVdDispCnt, gVdJpgCnt);
	//printf("idx=%lu, %lu\n", vidPbCtrl.lastVidIdx, vidPbCtrl.lastAudIdx);
	//vdDebugShow();
	#endif
	
	//Add for Video Play rewind eric@20081015
	DOS_FileReadMakeFatEnable(0, vidPbCtrl.fHandle);

	vfsFileClose(vidPbCtrl.fHandle,vidPbCtrl.name , readMode, 0);// patch5.1@wyeo@speed up avi playback
	vfsFileClose(vidPbCtrl.fHandle_idx, vidPbCtrl.name, readMode, 0);

	vidPbCtrl.audInIdx = 0;
	/* modify by phil for remove used variables @08.09.18 */

	//videoPbExit();	//xian ++ 20090915
	videoPbGlobalInit();
	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	osMsgPost(SP1K_MSG_MEDIA_PLAY_END);

	return;
}

//-----------------------------------------------------------------------------
//videoPbKeyOnPlay
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbStart(UINT16 ID)
 * @brief		key on PLAY when video playback
 * @param	ID	:	AVI file index
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbStart(UINT16 ID)USING_0
{
	UINT8 sts;
	
	HAL_CpuImemSdSet(LOG_BANK_4, 0xa000);
	gJpgPiroSv = XBYTE[REG_Dram_Priority0_lvl2_master];
	XBYTE[REG_Dram_Priority0_lvl2_master] = 0x4;

	videoPbGlobalInit();
	if(vidPbCtrl.Idle == VID_PB_IDLE) {
		sts = videoPbInit(ID);
		if(!sts) {
			osActiveFuncSet(pFunc_VideoPb);
			vidPbCtrl.Idle = VID_PB_RUN;
			appPbVideoOsdInit();
		}else {
			//vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, K_DOS_ReadFile_LinearFAT, 0);//speed up avi playback
			//vfsFileClose(vidPbCtrl.fHandle_idx, vidPbCtrl.name, K_DOS_ReadFile_LinearFAT, 0);//speed up avi playback
			ASSERT(0, 1);
			return;
		}
	}

	return;
}

/**
 * @fn		  void videoPbStop(void)
 * @brief	  video Play back Abort
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoPbStop(void)
{
	videoPbAbort();
}

//-----------------------------------------------------------------------------
//videoPbKeyOnPause
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbPause(void)
 * @brief		key on PAUSE when video playback
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbPause(void)USING_0
{
	if(vidPbCtrl.Idle == VID_PB_IDLE) {
		return;
	}
	if(!nonAudioFile){
		HAL_AudPause();
	}
	vidPbCtrl.Idle = VID_PB_PAUSE;

	return;
}

//-----------------------------------------------------------------------------
//videoPbKeyOnResume
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbResume(void)
 * @brief		key on RESUME when video playback
 * @param	NONE
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbResume(void)USING_0
{

	if(vidPbCtrl.Idle == VID_PB_IDLE) {
		return;
	}

	HAL_GlobalReadGTimer(&gPlayTime);
	gPlayStartTime = gPlayTime - videoPbTimeLengthGet(); //xian ++ 20081211, for no voice

	if(vidPbCtrl.Idle == VID_PB_PAUSE) {
		//xian remove 20100722
		if(!nonAudioFile){//xian ++ 20081211
			HAL_AudResume();
		}
		vidPbCtrl.Idle = VID_PB_RUN;
	} else {
		videoPbRelease();
	}

	return;
}

//-----------------------------------------------------------------------------
//videoPbKeyRelease
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbRelease(void)
 * @brief		FW or FR Exit
 * @param	on: always 1
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-18
 */
 #if 0//!VD_PB_HDMI_EN
 void videoPbRelease(void)
{
	UINT32 outIndex, inIndex;
	UINT32 time;
	UINT32 curTime;
	UINT8 status;
	UINT32 tmp0 = 4;
	UINT32 tmp1;

	ENTER_CRITICAL( status );

	preStatus = vidPbCtrl.Idle;
	vidPbCtrl.Idle = VID_PB_RUN;

	vidPbCtrl.audInIdx = (vidPbCtrl.audChunkCnt * audChunkSize) % AUDBUFSIZE;
	HAL_AudOutIndexGet(&outIndex);	//cannot be modified
	outIndex %= AUDBUFSIZE;
	while(outIndex > vidPbCtrl.audInIdx && vidPbCtrl.lastAudIdx){
		vidPbCtrl.lastAudIdx = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1,1);
		vidPbCtrl.audInIdx += audChunkSize; 	   
		vidPbCtrl.audChunkCnt ++;
	}
	vidPbCtrl.audInIdx %= AUDBUFSIZE;
	inIndex = vidPbCtrl.audInIdx;
	if(outIndex < inIndex){
		HAL_GprmDramFill(vidPbCtrl.audAddr + outIndex/2, (inIndex - outIndex + 1), 0);
	}
	else if(outIndex > inIndex){
		HAL_GprmDramFill(vidPbCtrl.audAddr + outIndex/2, (AUDBUFSIZE - outIndex + 1), 0);
		HAL_GprmDramFill(vidPbCtrl.audAddr, inIndex, 0);
	}
	
	time = (UINT32)((float)(vidPbCtrl.audChunkCnt * audChunkSize)/((float)bytePerSec/(float)1000));
	tmp1 = (time << 8) / vidPbCtrl.msPerFrame; //vid frame cnt;
	while(tmp1 > vidPbCtrl.lastVidIdx && vidPbCtrl.lastVidIdx){
		vidPbCtrl.lastVidIdx = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt , 1,0);
	}

	if(!vidPbCtrl.lastVidIdx || !vidPbCtrl.lastAudIdx){
		EXIT_CRITICAL( status );
		return;
	}
	HAL_AudInIndexSet(vidPbCtrl.audInIdx);
	if(vidPbCtrl.lastVidIdx < vidPbCtrl.lastAudIdx)
		dataOffset = videoPbReadDword(vidPbCtrl.AVIIdxAddr+GetIdxInBuf(vidPbCtrl.lastVidIdx)*8+4);
	else
		dataOffset = videoPbReadDword(vidPbCtrl.AVIIdxAddr+GetIdxInBuf(vidPbCtrl.lastAudIdx)*8+4);

	dataOffset = (UINT32)((dataOffset>>9)<<9);
	secAddr_max = 0;
	secAddr_min = dataOffset;
	secAddr_min_vid =dataOffset;
	secAddr_min_aud =dataOffset;
	VlcRingBufInIdx = 0;
	VlcRingBufOutIdx = 0;
	dataOffset += AviHeaderSize;
#if VD_PB_HDMI_EN
	videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, 0); //max size
#else
	videoPbStreamRead(vidPbCtrl.fHandle, dataOffset);
#endif
	vidPbCtrl.speed = VID_PB_SPEED_NORMAL;

	videoPbMsGet(&time);

	time =( time >>8);
	curTime = videoPbTimeGet();
	if(preStatus == VID_PB_FW) {
		VidPb_GT_zero -= (time - curTime);
	}else {
		VidPb_GT_zero += (curTime - time);
	}

	while(tmp0 --)
	{
		videoPbAudioPlay();
	}
	EXIT_CRITICAL( status );
	HAL_GlobalReadGTimer(&gPlayTime);
	gPlayStartTime = gPlayTime - videoPbTimeLengthGet(); //xian ++ 20081211, for no voice
	//xian remove 20100722
	//if(!gSilence){	 //xian ++ 20081211
		HAL_AudResume();
	//}

//	printf("resume\n");
	return;
}
#endif
//-----------------------------------------------------------------------------
//videoPbReadDword
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 videoPbReadDword(UINT32 dramAddr)
 * @brief		read double word from sdram
 * @param	dramAddr	:	sdram address
 * @retval	DwValue		:	The value of the double word
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
UINT32 videoPbReadDword(UINT32 dramAddr) USING_0
{
	  UINT32	 DwValue;
	  UINT8	 tmp2, tmp3;

	  HAL_DramStartAddrSet(dramAddr, 1);
	  HAL_DramWordDataRead(&tmp2,&tmp3);
	  DwValue = (UINT32)tmp2 | (UINT32)tmp3<<8;

	  HAL_DramStartAddrSet(dramAddr+1, 1);
	  HAL_DramWordDataRead(&tmp2,&tmp3);
	  DwValue = DwValue  |	(UINT32)tmp2<<16 | (UINT32)tmp3<<24;

	  return DwValue;
}
//-----------------------------------------------------------------------------
//AVIHeaderParse
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 AVIHeaderParse(void)
 * @brief		parse AVI header
 * @param	NNONE
 * @retval	SUCCESS or FAIL
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
UINT8 AVIHeaderParse(void) USING_0
{
	UINT16 i,j;
	UINT8 sts = SUCCESS;

	UINT8 QTable[128];
	UINT32 dwRate, dwScale;
	float	 frameRate;
	UINT8 StreamNum;
	UINT32 tmp1;
	UINT8 tmp2,tmp3,tmp4,tmp5;
	UINT8 readMode = K_DOS_ReadFile_LinearFAT;
	UINT32 offset_addr = 0;
	UINT16 tmpAddr = 0;
	UINT32 idxPos;
	UINT8 qTSize;
	UINT16 dmaBytes;
	
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	if((tmp2 == 'R')&&(tmp3 == 'I')&&(tmp4 == 'F')&&(tmp5 == 'F')){
		sts = SUCCESS;
	} else {
		ASSERT(0, 1);
		sts = FAIL;
		return sts;
	}
	offset_addr += 2;

	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	if(((tmp2 == 'A')&&(tmp3 == 'V')&&(tmp4 == 'I')&&(tmp5 == 0x20)) || ((tmp2 == 'a')&&(tmp3 == 'v')&&(tmp4 == 'i')&&(tmp5 == 0x20))){
		sts = SUCCESS;
	} else {
		ASSERT(0, 1);
		sts = FAIL;
		return sts;
	}
	offset_addr += 2;

	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	tmp1 = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	AviHeaderSize = tmp1 + 20;
	dataOffset = AVI_512_ALIGN(AviHeaderSize);
	offset_addr += 2;

	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	if(((tmp2 == 'A')&&(tmp3 == 'V')&&(tmp4 == 'I')&&(tmp5 == 'H')) || ((tmp2 == 'a')&&(tmp3 == 'v')&&(tmp4 == 'i')&&(tmp5 == 'h'))){
		sts = SUCCESS;
	} else {
		ASSERT(0, 1);
		sts = FAIL;
		return sts;
	}
	offset_addr += 2;

	offset_addr += 10;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	vidPbCtrl.vidChunk = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	//printf("vidChunk:%ld\n",vidPbCtrl.vidChunk);
	offset_addr += 2;

	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	StreamNum = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	//DbgPrint("StreamNum=%bx\n",StreamNum);
	offset_addr += 2;

	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	G_AVIWidth = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	offset_addr += 2;

	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	G_AVIHeight = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	#if  JPG_1080P_PB
	if(G_AVIHeight==1080){
		G_AVIHeight=1088;
	}
	#endif
	offset_addr += 2;
	//DbgPrint("G_AVIWidth=%d,G_AVIHeight=%d\n",G_AVIWidth,G_AVIHeight);

	for(i = 0; i< 512; i++) {
		HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'v')&&(tmp3 == 'i')&&(tmp4 == 'd')&&(tmp5 == 's'))|| ((tmp2 == 'V')&&(tmp3 == 'I')&&(tmp4 == 'D')&&(tmp5 == 'S'))){
			sts = SUCCESS;
			break;
		} else {
			sts = FAIL;
			offset_addr += 2;
		}
	}

	if(sts == SUCCESS) {
		offset_addr += 10;
	} else {
		ASSERT(0, 1);
		return sts;
	}
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	dwScale = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	offset_addr += 2;

	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	dwRate = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	offset_addr += 2;

	frameRate =	(float)dwRate/(float)dwScale;
	vidPbCtrl.frameRate = (UINT32)frameRate;
	vidPbCtrl.msPerFrame = (UINT32)((float)256000 / frameRate);  //yichang@0416
	g_msPerFrame_fraction = (UINT32)((float)vidPbCtrl.msPerFrame / (((float)256000 / frameRate) - vidPbCtrl.msPerFrame));
	printf("vidChunk:%ld, fps:%f\n",vidPbCtrl.vidChunk, frameRate);
//	printf("msPerFrame=%ld, %ld\n",vidPbCtrl.msPerFrame, g_msPerFrame_fraction);
	#ifdef VID_DUG
	DbgPrint("dwRate=%ld,	dwScale=%ld\n",	dwRate,	dwScale);
	DbgPrint("frameRate=%f\n",frameRate);
	DbgPrint("msPerFrame=%ld\n",vidPbCtrl.msPerFrame);
	#endif

	for(i = 0; i< 512; i++) {
		HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'a')&&(tmp3 == 'u')&&(tmp4 == 'd')&&(tmp5 == 's'))|| ((tmp2 == 'A')&&(tmp3 == 'U')&&(tmp4 == 'D')&&(tmp5 == 'S'))){
			//nonAudioFile = 1;
			//goto skipGetAudInfo	 ;
			sts = SUCCESS;
			break;
		} else if((tmp2 == 'x')&&(tmp3 == 'u')&&(tmp4 == 'd')&&(tmp5 == 's')){//xian ++ 20081211
			//printf("slince..\n");
			//xian remove 20100722
			//gSilence = 1;
			sts = SUCCESS;
			break;
		} else if((tmp2 == 'n')&&(tmp3 == 'u')&&(tmp4 == 'd')&&(tmp5 == 's')){
			nonAudioFile = 1;
			goto skipGetAudInfo; //no need to parser audio infomation
		}
		else {
			sts = FAIL;
			offset_addr += 2;
		}
	}
	if(sts == SUCCESS) {
		offset_addr += 16;
		HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		vidPbCtrl.audChunk = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
		offset_addr += 2;
		#if 0
		vidPbCtrl.audChunk = (vidPbCtrl.audChunk >>SECPERCHK);
		#endif
	} else {
		ASSERT(0, 1);
		return sts;

	}

	offset_addr += 12;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	offset_addr += 2;

	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	offset_addr += 2;
	vidPbCtrl.audFmt = (UINT32)tmp2 | ((UINT32)tmp3<<8);
	if((vidPbCtrl.audFmt != 0x0001) && (vidPbCtrl.audFmt != 0x0011)) {
		ASSERT(0, 1);
		return FAIL;
	}
	vidPbCtrl.audChnnel = (UINT32)tmp4 | ((UINT32)tmp5<<8);
	if((vidPbCtrl.audChnnel != 0x01) && (vidPbCtrl.audFmt != 0x02)) {
		ASSERT(0, 1);
		return FAIL;
	}

	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	vidPbCtrl.audFrq = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
#if VD_UPSAMPLING_TEST
	if(vidPbCtrl.audFrq == 11025){
		G_VPBupSmpflag = 1;
	}
#else
	if((dispDevActGet() == 4)&&(vidPbCtrl.audFrq == 11025)) {//for upsampling
		G_VPBupSmpflag = 1;
	}	
#endif

	offset_addr += 2;
	offset_addr += 2;
	HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + offset_addr, 1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	offset_addr += 2;
	vidPbCtrl.audAlign = (UINT32)tmp2 | ((UINT32)tmp3<<8);
	vidPbCtrl.audBit = (UINT32)tmp4 | ((UINT32)tmp5<<8);
	if(vidPbCtrl.audFmt == 0x0001){
		vidPbCtrl.audChunk = (vidPbCtrl.audChunk >>SECPERCHK)*(vidPbCtrl.audBit/8);
//		printf("PCM\n");
	}
	else{
		vidPbCtrl.audChunk = vidPbCtrl.audChunk/(CHUNKSIZE/0x200);
//		printf("ADPCM\n");
	}
//	printf("audChunk:%ld, frq:%ld\n",vidPbCtrl.audChunk, vidPbCtrl.audFrq);
skipGetAudInfo:
	for(i	 = 0; i< 1024/4;i++) {
		HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'i')&&(tmp3 == 'd')&&(tmp4 == 'x')&&(tmp5 == 0x31))) {
			sts = SUCCESS;
			break;
		} else {
			sts = FAIL;
		}
	}
	if(sts == SUCCESS) {
		HAL_DramStartAddrSet(vidPbCtrl.pbAudioChunkAddr + ((i+1)*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		indexSize = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
		idxPos = i*4 + 8;
		IDX_START = AVI_512_ALIGN(idxPos);
		IDXHD_SIZE = idxPos - IDX_START;
		//printf("idxPos=%lx\n",idxPos);
	}else {
		ASSERT(0, 1);
		return FAIL;
	}
	vidPbCtrl.chunckCnt = vidPbCtrl.audChunk + vidPbCtrl.vidChunk;
	if(vidPbCtrl.chunckCnt	== 0)
	{
		vfsFileClose(vidPbCtrl.fHandle ,vidPbCtrl.name , readMode, 0);//speed up avi playback
		ASSERT(0, 1);
		return	FAIL;
	}

	//printf("idxst:%lu, %lu\n", IDX_START, IDXHD_SIZE);
	//printf("avi:%lu, %u\n", AviHeaderSize, VidHeaderSize);

	vidPbCtrl.AVIIdxAddr =vidPbCtrl.AVIIdxRamAddr + (IDXHD_SIZE>>1); // read file 512 aligment
	videoPbReadAVIIdx(vidPbCtrl.AVIIdxRamAddr ,vidPbCtrl.fHandle_idx);

	/* Get Timelapse video file FF DB offset  */
	i = 0;
	//printf("offset=%ld\n",((tmp1 + 20)>>9)<<9);
	vfsFileRead(vidPbCtrl.fHandle, K_DOS_DstDram, 1024, vidPbCtrl.pbAudioChunkAddr ,  ((tmp1 + 20)>>9)<<9);
	dmaBytes = 1024;
	
	//vfsFileRead(vidPbCtrl.fHandle, K_DOS_DstDram, 512, vidPbCtrl.pbAudioChunkAddr ,  ((tmp1 + 20)>>9)<<9);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(vidPbCtrl.pbAudioChunkAddr);
	HAL_DmaDo(0,1,dmaBytes,0,0);
	for(; i< dmaBytes;i+=4) {
		if(((XBYTE[0x3800+i] == 0xFF )&&(XBYTE[0x3800+i+1] == 0xDB)&&(XBYTE[0x3800+i+2] == 0x00)&&((XBYTE[0x3800+i+3] == 0xC5)||(XBYTE[0x3800+i+3] == 0x84)))) {
			j = i;
			qTSize = XBYTE[0x3800+i+3];
			sts = SUCCESS;
			break;
		} else {
			sts = FAIL;
		}
	}
	if(sts == FAIL) {
		ASSERT(0, 1);
		return	sts;
	} else {
		tmpAddr = 0x3800+i;
		tmpAddr += 4;
	}
	//printf("tmpAddr : %x\n", tmpAddr);
	for(i=0;i<64;i++ ) {
		QTable[i] = XBYTE[tmpAddr+1+i];
	}
	for(i=64;i<128;i++) {
		QTable[i] = XBYTE[tmpAddr +2+i];
	}

	jpegQTableSet( 0, QTable, QTable + 64, 1 );

	tmpAddr = (UINT16)(0x3800+2+qTSize+j);
	//printf("sz=%u\n",4+qTSize+j);
	if((XBYTE[tmpAddr] == 0xFF )&&(XBYTE[tmpAddr+1] == 0xC0)) {
		if(XBYTE[tmpAddr+11] == 0x22){
			G_AVIHeight = (G_AVIHeight +15)&(~15);
		}	
	} 
	

	vidPbCtrl.mediaType 	= 0;

#if 1//VD_PB_HDMI_EN
	vidPbCtrl.TimeExceed = vidPbCtrl.msPerFrame; //dec from the second
#endif	

	#ifdef VID_DUG
	printf("media Type %bx\n ",vidPbCtrl.mediaType);
	#endif

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//videoPbMsGet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbMsGet(UINT32 *pTime)
 * @brief		play time Get
 * @param	*pTime	:	play time
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbMsGet(UINT32 *pTime)
{
#if 1//VD_PB_HDMI_EN
//	*pTime = (vidPbCtrl.vidChunkCnt - 1) * vidPbCtrl.msPerFrame;
	*pTime = ((vidPbCtrl.vidChunkCnt - 1) + (vidPbCtrl.vidChunkCnt - 1)/g_msPerFrame_fraction) * vidPbCtrl.msPerFrame;
#else
	*pTime = vidPbCtrl.vidChunkCnt * vidPbCtrl.msPerFrame;
#endif
	return;
}

//-----------------------------------------------------------------------------
//SetPbBufHsize
//-----------------------------------------------------------------------------
/**
 * @fn		  void SetPbBufHsize(UINT16 width)
 * @brief		Set play buffer Hsize
 * @param	width	:	buffer width
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
void SetPbBufHsize(UINT16 width)
{
	UINT16 height;
	videoPbSizeGet(&width, &height);	//xian ++ 20090915
	HAL_PbBufHsizeSet(width);
	//XBYTE[REG_Dram_PBBHSize_Low] = READ8(width, 1);
	//XBYTE[REG_Dram_PBBHSize_High] = READ8(width, 0);
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB); //For update display controller registers
	return;
}

//-----------------------------------------------------------------------------
//videoPbStatusGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoPbStatusGet(void)
 * @brief		Get play Status
 * @param	NONE
 * @retval	play status
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
UINT8 videoPbStatusGet(void) USING_0
{
	return vidPbCtrl.Idle;
}

//-----------------------------------------------------------------------------
//videoPbSpeedGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 videoPbSpeedGet(void)
 * @brief		Get play speed
 * @param	NONE
 * @retval	play speed
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
UINT8 videoPbSpeedGet(void) USING_0
{
	return vidPbCtrl.speed ;
}

//-----------------------------------------------------------------------------
//videoPbSizeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbSizeMatchGet(vdPbSizeMatchTable_t *pVal)
 * @brief		Get play buffer size
 * @param	*phSize : buffer width
  * @param	*pvSize : buffer height
 * @retval	NONE
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
//xian ++ 20090915
void videoPbSizeMatchGet(vdPbSizeMatchTable_t *pVal)
{
	UINT8 ratX, ratY;
	UINT8 i = 0; //default 320*240	  

	dispPnlAspectRatioGet(&ratX, &ratY);
	if(G_AVIWidth == 320)       i = 0;
	else if(G_AVIWidth == 640)	i = 1;
	else if(G_AVIWidth == 720)	i = 2;//else if(G_AVIWidth == 848)	i = 2;//modify for D1 size
	else if(G_AVIWidth == 1280) i = 3;
    else if(G_AVIWidth == 1920) i = 4;
	else if(G_AVIWidth == 1024) i = 5;
	else if(G_AVIWidth == 1600) i = 6;
	{//  aspect ratio: 4:3
		pVal->disW = vdTable_H4_V3[i].disW;
		pVal->disH = vdTable_H4_V3[i].disH;
		pVal->fact = vdTable_H4_V3[i].fact;
	}
	if(G_AVIHeight == 1080){
		pVal->disH = 540;
	}
//	  printf("pb size: %u, %u, %bu\n", pVal->disW, pVal->disH, pVal->fact);
}
void videoPbSizeGet(UINT16 *phSize, UINT16 *pvSize)
{
	vdPbSizeMatchTable_t val;
	
	videoPbSizeMatchGet(&val);	
	*phSize = val.disW;
	*pvSize = val.disH;
}
//xian -- 20090915
//-----------------------------------------------------------------------------
//videoPbGTGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 videoPbTimeGet(void)
 * @brief		Get video play	time
 * @param	NONE
 * @retval	video play	time
 * @todo		NONE
 * @author	SunYong
 * @since	2007-12-25
 */
UINT32 videoPbTimeGet(void)
{
	UINT32 time;
	UINT32 audRingCnt;

	if(vidPbCtrl.Idle == VID_PB_IDLE){
		return 0;
	}
	HAL_AudRingBufCntRead( &audRingCnt);
	if((G_VPBupSmpflag)) {	//for upsampling	
		audRingCnt>>=2;
	}	
	if(audRingCnt <= (vidPbCtrl.audChunkCnt*audChunkSize)){
		audRingCnt = (vidPbCtrl.audChunkCnt*audChunkSize) - audRingCnt;
	}
	else{
		audRingCnt = (vidPbCtrl.audChunkCnt*audChunkSize) ;
	}

	if(/*gSilence ||*/nonAudioFile ||slowMode && vidPbCtrl.Idle == VID_PB_FW){//xian ++, 20080919 for slow motion
		HAL_GlobalReadGTimer(&time);
		time = time - gPlayTime;
		if(vidPbCtrl.speed > 1){
			time = time/vidPbCtrl.speed;
		}

		time = gPlayTime - gPlayStartTime + time;
		//xian remove 20100722
		//if(gSilence){ //xian ++ 20081211
		//	HAL_AudInIndexGet(&audRingCnt);
		//	HAL_AudOutIndexSet(audRingCnt);
		//}
	}
	else{
		if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL)
			audRingCnt = (vidPbCtrl.audChunkCnt*audChunkSize) ;
		time = (UINT32)((float)audRingCnt/((float)bytePerSec/(float)1000));
	}
	return time;
}

//xian ++ 20080912
/**
 * @fn		  void videoPbRamMove(UINT32 srcRamAddr, UINT32 dstRamAddr, UINT32 size)
 * @brief	  video Playback Ram Move
 * @param	  [in] srcRamAddrWord
 * @param	  [in] dstRamAddrWord
 * @param	  [in] size
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoPbRamMove(UINT32 srcRamAddrWord, UINT32 dstRamAddrWord, UINT32 size)
{
	UINT32 sizeTmp;
	UINT32 srcTmp, dstTmp;

	srcRamAddrWord <<= 1;
	dstRamAddrWord <<= 1;
	if(srcRamAddrWord == dstRamAddrWord || !size)
		return;

	if(srcRamAddrWord < dstRamAddrWord){//move down
		sizeTmp = dstRamAddrWord - srcRamAddrWord;
		srcTmp = srcRamAddrWord + (size - sizeTmp);
		dstTmp = dstRamAddrWord + (size - sizeTmp);
		HAL_GprmDramDma(srcTmp, dstTmp, sizeTmp, 1);

		size -= sizeTmp;
	}

	HAL_GprmDramDma(srcRamAddrWord, dstRamAddrWord, size, 1);		 

	return;
}

//should 512 alignment
//idxRamAddr, idxRamSize not include IDXHD_SIZE
/**
 * @fn		  UINT8 videoPbReadNextAVIIdx(UINT32 idxRamAddr, UINT32 idxRamSize, UINT16 fHandle, UINT32 startIdx)
 * @brief	  videoPbReadNextAVIIdx
 * @param	  [in] idxRamAddrWord
 * @param	  [in] idxRamSize
 * @param	  [in] fHandle
 * @param	  [in] startIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoPbReadNextAVIIdx(UINT32	idxRamAddrWord, UINT32 idxRamSize, UINT16 fHandle, UINT32 startIdx)
{
	UINT32 first;
	UINT32 readSize, moveSize, remainSize;	//in bytes
	UINT32 readOff, firstReadOff;	//should 512 alignment
	UINT32 idxRamReadAddr;		 

	idxRamReadAddr = idxRamAddrWord;
	first = vidPbCtrl.fstIdxInBuf;

	if(startIdx >= vidPbCtrl.chunckCnt){ 	
		return FAIL;
	}
	readOff = AVI_IDX_OFF(startIdx);//not 512 alignment
	firstReadOff = AVI_IDX_OFF(first);

	if(startIdx > first && readOff < vidPbCtrl.idxReadOff){//countine read
		moveSize = vidPbCtrl.idxReadOff - readOff;
		readOff = vidPbCtrl.idxReadOff; //must be 512 alignment

		idxRamReadAddr = idxRamAddrWord + ((readOff - firstReadOff - moveSize)>>1);
		videoPbRamMove(idxRamReadAddr, idxRamAddrWord, moveSize);
		idxRamReadAddr = idxRamAddrWord + (moveSize>>1);
	}
	else{//readOff cannot equ to vidPbCtrl.idxReadOff, because 512 alignment
		moveSize = 0;
		readOff = AVI_512_ALIGN(readOff);
		
		idxRamReadAddr = idxRamAddrWord - (IDXHD_SIZE>>1);
	}
		
	readSize = idxRamSize - moveSize;//idxRamSize not include IDXHD_SIZE  
	readSize = AVI_512_ALIGN(readSize);
	remainSize = indexSize - (readOff-IDX_START-IDXHD_SIZE - 511); //at least read 512
	remainSize = AVI_512_ALIGN(remainSize);
	if(readSize > remainSize)
		readSize = remainSize;
				   
	vfsFileRead(fHandle, K_DOS_DstDram, readSize, idxRamReadAddr/*idxRamAddr+(moveSize>>1)*/, readOff);

	vidPbCtrl.idxReadOff = readOff + readSize;
	vidPbCtrl.lstIdxInBuf = ((vidPbCtrl.idxReadOff - IDX_START - IDXHD_SIZE) >> IDXDATA_SHIFT);
	if(!moveSize /*&& readOff == IDX_START*/){ // xian fix 41824
		readSize -= IDXHD_SIZE; 	   
	}
	vidPbCtrl.fstIdxInBuf = vidPbCtrl.lstIdxInBuf - ((readSize + moveSize) >> IDXDATA_SHIFT);

	vidPbCtrl.lstIdxInBuf -= 1;
	if(vidPbCtrl.lstIdxInBuf >= vidPbCtrl.chunckCnt){
		vidPbCtrl.lstIdxInBuf = vidPbCtrl.chunckCnt;
	}

	//printf("next idx buf: 0x%lx, first=%ld, last=%ld, off=0x%lx\n", idxRamAddr, vidPbCtrl.fstIdxInBuf, vidPbCtrl.lstIdxInBuf, vidPbCtrl.idxReadOff);

	return SUCCESS;
}

/**
 * @fn		  UINT8 videoPbReadPrevAVIIdx(UINT32 idxRamAddr, UINT32 idxRamSize, UINT16 fHandle, UINT32 endIdx)
 * @brief	  videoPbReadPrevAVIIdx
 * @param	  [in] idxRamAddrWord
 * @param	  [in] idxRamSize
 * @param	  [in] fHandle
 * @param	  [in] endIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoPbReadPrevAVIIdx(UINT32	idxRamAddrWord, UINT32 idxRamSize, UINT16 fHandle, UINT32 endIdx)
{
	UINT32 startIdx;
	UINT32 idxCnt;

	idxCnt = AVI_512_ALIGN(idxRamSize-512) >> IDXDATA_SHIFT;
	if(endIdx < idxCnt)
		idxCnt = endIdx;

	startIdx = endIdx - idxCnt;
	//printf("%lu---------%lu\n", endIdx, startIdx);   

	return videoPbReadNextAVIIdx(idxRamAddrWord, idxRamSize, fHandle, startIdx);
}

//first read idx, read file should 512 alignment
/**
 * @fn        UINT8 videoPbReadAVIIdx(UINT32 AVIIdxRamAddr, UINT32 AVIIdxAddr, UINT16 fHandle, dle,UINT32* ChunkCnt, UINT8 onlyFirst)
 * @brief     videoPbReadAVIIdx
 * @param     [in] AVIIdxRamAddr
 * @param     [in] AVIIdxAddrWord
 * @param     [in] fHandle
 * @param     [in] ChunkCnt
 * @param     [in] onlyFirst
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbReadAVIIdx(UINT32	AVIIdxRamAddr, UINT16 fHandle) USING_0
{
	UINT32 idxBufSize, readSize;
	   
	idxBufSize = IDX_BUF_SIZE;
		
	//printf("idx buf:%ldKB\n",idxBufSize/1024);
	readSize = AVI_512_ALIGN(indexSize+512);   //should 512 alignment
	if(readSize > idxBufSize)
		readSize = idxBufSize;
		   
	vfsFileRead(fHandle, K_DOS_DstDram, readSize, AVIIdxRamAddr, IDX_START);
	vidPbCtrl.idxReadOff = IDX_START + readSize;
	vidPbCtrl.fstIdxInBuf = 0;
	vidPbCtrl.lstIdxInBuf = ((readSize - IDXHD_SIZE) >> IDXDATA_SHIFT) - 1;

	return SUCCESS;
}

/**
 * @fn        UINT32 videoPbSearchAVIIndex(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx, maxIdx,UINT32 indexcount, UINT8 type)
 * @brief     video Pb Search AVI Index
 * @param     [in] AVIIdxRamAddr
 * @param     [in] defaultIdx
 * @param     [in] maxIdx
 * @param     [in] indexcount
 * @param     [in] type
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT32 videoPbSearchAVIIndex(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx,UINT32 indexcount,	UINT8 type ) USING_0
{
	UINT32 i;	// in buffer
	UINT32 idx;
	UINT8	tmp2, tmp3;
	UINT32 cnt=0;
	UINT32 idxBufSize;

	idxBufSize = IDX_BUF_SIZE - IDXHD_SIZE;

	idx = (defaultIdx+1) ;
	if((defaultIdx == 0) && (vidPbCtrl.vidChunkCnt == 0)){
		idx = 0;
	}

	i = GetIdxInBuf(idx);			 
	for(; idx < (maxIdx -1); idx++)
	{
		if(idx > vidPbCtrl.lstIdxInBuf){//out of buf			
			videoPbReadNextAVIIdx(AVIIdxRamAddr, idxBufSize, vidPbCtrl.fHandle_idx, idx - 256);
			i = GetIdxInBuf(idx);			 
		}
		else if(idx < vidPbCtrl.fstIdxInBuf){//out of buf, #42068
			videoPbReadPrevAVIIdx(AVIIdxRamAddr, idxBufSize, vidPbCtrl.fHandle_idx, idx + 256);
			i = GetIdxInBuf(idx);			 
		}
			
		HAL_DramStartAddrSet(AVIIdxRamAddr+(UINT32)i*8, 1);
		HAL_DramWordDataRead(&tmp2,&tmp3);
		i++;

		if(type == 0)
		{
			if(tmp2==0x30 && tmp3==0x30) { //00dc
				vidPbCtrl.vidChunkCnt ++;
				if(vidPbCtrl.vidChunkCnt  >= (vidPbCtrl.vidChunk -1)) {
					vidPbCtrl.vidChunkCnt = vidPbCtrl.vidChunk;
					return 0;//vidPbCtrl.chunckCnt;
				}
				cnt ++;
				if(cnt == indexcount)
					return idx;
			}else if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL/* || gSilence*/){
				vidPbCtrl.audChunkCnt ++;
				vidPbCtrl.lastAudIdx = idx;
				
			}
		}
		else
		{
			if(tmp2==0x30 && tmp3==0x31) {//01wb
				cnt ++;
				if(cnt == indexcount)
					return idx;
			}
		}
	}
	return 0;
}

/**
 * @fn        UINT32 videoPbSearchAVIIndexBackward(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx, maxIdx,UINT32 indexcount, UINT8 type)
 * @brief     video PbSearch AVI Index Backward
 * @param     [in] AVIIdxRamAddr
 * @param     [in] defaultIdx
 * @param     [in] maxIdx
 * @param     [in] indexcount
 * @param     [in] type
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT32 videoPbSearchAVIIndexBackward(UINT32 AVIIdxRamAddr, UINT32 defaultIdx, UINT32 maxIdx,UINT32 indexcount,	UINT8 type ) USING_0
{
	UINT32 i;	// in buffer
	UINT32 idx;
	UINT8	tmp2, tmp3;
	UINT32 cnt=0;
	UINT32 idxBufSize;

	idxBufSize = IDX_BUF_SIZE - IDXHD_SIZE;

	maxIdx = maxIdx;
	if(defaultIdx <= 1) {
		vidPbCtrl.vidChunkCnt = 0;
		return 0;
	}

	idx	= (defaultIdx-1);
	i = GetIdxInBuf(idx);			 
	for( ; idx > 0; idx--)
	{

		if(idx < vidPbCtrl.fstIdxInBuf){//out of buf			
			videoPbReadPrevAVIIdx(AVIIdxRamAddr, idxBufSize, vidPbCtrl.fHandle_idx, idx + 256);
			i = GetIdxInBuf(idx);			 
		}
		else if(idx > vidPbCtrl.lstIdxInBuf){//out of buf			
			videoPbReadNextAVIIdx(AVIIdxRamAddr, idxBufSize, vidPbCtrl.fHandle_idx, idx - 256);
			i = GetIdxInBuf(idx);			 
		}
		
		HAL_DramStartAddrSet(AVIIdxRamAddr+(UINT32)i*8, 1);
		HAL_DramWordDataRead(&tmp2,&tmp3);
		i--;

		if(type == 0)
		{
			if(tmp2==0x30 && tmp3==0x30) { //00dc
				if(vidPbCtrl.vidChunkCnt <= 1) {
					vidPbCtrl.vidChunkCnt = 0;
					return 0;
				}
				vidPbCtrl.vidChunkCnt --;

				cnt ++;
				if(cnt == indexcount)
					return idx;
			}else if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL/* || gSilence*/){
				vidPbCtrl.audChunkCnt --;
				vidPbCtrl.lastAudIdx = idx;
			}
		}
		else
		{
			if(tmp2==0x30 && tmp3==0x31) {//01wb
				vidPbCtrl.audChunkCnt --;
				cnt ++;
				if(cnt == indexcount)
					return idx;
			}
		}
	}
	//printf("search idx % ld\n",i);
	return 0;

}

/**
 * @fn        UINT32 GetIdxInBuf(UINT32 idx)
 * @brief     get index in buffer
 * @param     [in] idx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT32 GetIdxInBuf(UINT32 idx)
{
	return (idx - vidPbCtrl.fstIdxInBuf);
}

//xian ++ 20081015
#if 1
/**
 * @fn        void updateRingBufIdx(void)
 * @brief     update Ring Buffer Index
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void updateRingBufIdx()
{
	UINT32 save_secAddr_min;

	save_secAddr_min = secAddr_min;

	if( nonAudioFile ){
		secAddr_min_aud = secAddr_min_vid;
	}
	secAddr_min = secAddr_min_aud < secAddr_min_vid ? secAddr_min_aud : secAddr_min_vid;

	save_secAddr_min = secAddr_min - save_secAddr_min;
	if(save_secAddr_min){
		VlcRingBufOutIdx = VlcRingBufOutIdx + save_secAddr_min;
		VlcRingBufOutIdx = VlcRingBufOutIdx &(vidPbFragBufSize-1);
	}
}

/**
 * @fn        void videoPbDecode(UINT32 addr, UINT8 VLCFlip, UINT8 wait)
 * @brief     video Pb Decode
 * @param     [in] addrWord
 * @param     [in] VLCFlip
 * @param     [in] wait
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
#if 0//!VD_PB_HDMI_EN
void videoPbDecode(UINT32 addrWord, UINT8 VLCFlip, UINT8 wait)
{
	UINT8 opt;
	UINT32 dstAddr;
	UINT16 dramread;

	if(wait)
		opt = 0x04;
	else
		opt = 0x0c;
	
	if(VLCFlip == 0) {
		dstAddr = K_SDRAM_AviPbBufAddrA;
	} else if(VLCFlip == 1){
		dstAddr = K_SDRAM_AviPbBufAddrB;
	} else {
		dstAddr = K_SDRAM_AviPbBufAddrC;
	}
	if (1){// tq.zhang video yuv420
		dramread=videoPbReadDword(addrWord-(UINT32)11);
		//printf("dramread=0x%x=%d\n",dramread,dramread);
		dramread &= 0x00ff;
		if (dramread==0x0021){
			opt &= 0xfc;
			opt |= 0x00;
		}
		else if (dramread==0x0022){
			opt &= 0xfc;
			opt |= 0x01;
		}
	}
	dstAddr += gJpgDecAddrOff;
	HAL_JpegDecode(addrWord, G_AVIWidth, G_AVIHeight, dstAddr, gJpgDecFact, opt, 0x00);
	gVdJpgCnt ++;
}

/**
 * @fn        UINT8 videoPbFrameRead_Fast(UINT32 AVIIdxAddrWord, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip)
 * @brief     videoPbFrameRead_Fast
 * @param     [in] AVIIdxAddrWord
 * @param     [in] fHandle
 * @param     [in] frameNo
 * @param     [in] VLCFlip
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbFrameRead_Fast(UINT32 AVIIdxAddrWord, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip) USING_0
{
	UINT32	 AviSize;
	UINT32	 AviAddr;
	UINT32	 PBVideoAddr;
	UINT32	 pos;

	AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+4);
	AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+6);

	if(AviSize == 0) {//skip dummy frame
		vidPbCtrl.dummyFrame = 1;

		while((!AviSize)&&vidPbCtrl.NextVidIdx) {	
			if(vidPbCtrl.Idle == VID_PB_RW) {
				vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr , vidPbCtrl.NextVidIdx , vidPbCtrl.chunckCnt, 1, 0);
			}else {
				vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr ,vidPbCtrl.NextVidIdx , vidPbCtrl.chunckCnt ,1, 0);
			}
			AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(vidPbCtrl.NextVidIdx)*8+4);
			AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(vidPbCtrl.NextVidIdx)*8+6);
		}
		if(vidPbCtrl.NextVidIdx ==0) {
			vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
			return FAIL;
		}
		vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
	}
	
#ifdef VIDEO_PB_BRC_EN
	AviAddr = AviAddr + (UINT32)AviHeaderSize + 16;
#else	 
	AviAddr = AviAddr + (UINT32)AviHeaderSize + 16 + (UINT32)VidHeaderSize;//point to VLC
	AviSize -= VidHeaderSize;	//vlc size
#endif
	pos = (UINT32)(AviAddr>>9) << 9; //read file offset
	dataOffset = pos;	//for release reading 

	PBVideoAddr = K_SDRAM_AviPbFragAddr + ((AviAddr - pos) >> 1);
	AviSize += AviAddr - pos;
	AviSize = ((AviSize>>9) + 1) << 9;
	
	vfsFileRead(fHandle,	K_DOS_DstDram, AviSize, K_SDRAM_AviPbFragAddr, pos);
		
#ifdef VIDEO_PB_BRC_EN 
	videoPbQtabSet(PBVideoAddr);
	PBVideoAddr += VidHeaderSize>>1;
#endif
	videoPbDecode(PBVideoAddr, VLCFlip, 1);
	return SUCCESS;
}

/**
 * @fn        UINT8 videoPbFrameRead(UINT32 AVIIdxAddr, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip)
 * @brief     read frame data
 * @param     [in] AVIIdxAddrWord
 * @param     [in] fHandle
 * @param     [in] frameNo
 * @param     [in] VLCFlip
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbFrameRead(UINT32 AVIIdxAddrWord, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip) USING_0
{
	UINT32	AviAddrEnd;
	UINT32	AviSize;
	UINT32	AviAddr;
	UINT32	tmp_SecAddr;
	UINT32	tmp_SecAddrEnd;
	UINT32	copySize;

	if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL) {
		return videoPbFrameRead_Fast(AVIIdxAddrWord, fHandle, frameNo, VLCFlip);
	}

	AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+4);
	AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+6);

	if(AviSize == 0) {
		vidPbCtrl.dummyFrame = 1;
		vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
		secAddr_min_vid = (UINT32)16 + AviAddr;
		updateRingBufIdx();
		return SUCCESS;	
	}	
#ifdef VIDEO_PB_BRC_EN
	AviAddr = (UINT32)16 + AviAddr;
#else
	AviAddr = (UINT32)16 + AviAddr + VidHeaderSize; //point to vlc
	AviSize -= VidHeaderSize;
#endif
	AviAddrEnd = AviAddr + AviSize;

	if(AviAddr >= secAddr_min) {
		if(AviAddrEnd > secAddr_max){//data not enough
			videoPbStreamRead(vidPbCtrl.fHandle, dataOffset);		 
		}
		
		if(AviAddrEnd <= secAddr_max) { //data enougth
			tmp_SecAddr = VlcRingBufOutIdx + (AviAddr - secAddr_min);
			tmp_SecAddr = tmp_SecAddr & (vidPbFragBufSize-1);								 
			tmp_SecAddrEnd = tmp_SecAddr + AviSize;
		   
			if(tmp_SecAddrEnd > vidPbFragBufSize){							   
				// copy data			
				copySize = tmp_SecAddrEnd - vidPbFragBufSize;
				if(copySize < ((UINT32)K_SDRAM_AviPbFragSize<<1) - vidPbFragBufSize){
					HAL_GprmDramDma(((UINT32)K_SDRAM_AviPbFragAddr <<1), ((UINT32)K_SDRAM_AviPbFragAddr <<1) + vidPbFragBufSize, copySize, 1);
				}
				else{//buf not enough
					return FAIL;
				}
			}
			
#ifdef VIDEO_PB_BRC_EN 
			//xian ++ 20090701 fix bug
			videoPbQtabSet(K_SDRAM_AviPbFragAddr + (tmp_SecAddr>>1));
			tmp_SecAddr += (UINT32)VidHeaderSize;
			//xian --
#endif
			videoPbDecode(K_SDRAM_AviPbFragAddr + (tmp_SecAddr>>1), VLCFlip, 0);
			videoPbStreamRead(vidPbCtrl.fHandle, dataOffset);
		} 
		else {//data not enough		
			return FAIL;
		}
	} 
	else {
		//xian ++ 20090915
		vidPbCtrl.dummyFrame = 1;
		vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
		updateRingBufIdx();
		return SUCCESS;	
		//xian -- 20090811
	}

	secAddr_min_vid = AviAddrEnd;		  
	updateRingBufIdx();

	while((XBYTE[REG_Jpeg_Intr_Event] & 0x04) == 0x00) {
	}//wait decode finished
	XBYTE[REG_Jpeg_Intr_Event]=0xFF;

	return SUCCESS;
}
#endif
#endif
//xian --

/**
 * @fn        UINT32 videoPbTimeLengthGet(void)
 * @brief     get video Pb Time Length
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT32 videoPbTimeLengthGet(void)
{
	UINT32 time;
#if 1//VD_PB_HDMI_EN
	if(vidPbCtrl.frameRate <=5){
		time = vidPbCtrl.frameTime[vidPbCtrl.dispFlip];
	}
	else{
		time = (vidPbCtrl.vidChunkCnt - 1) * vidPbCtrl.msPerFrame / vidPbCtrl.speed;
	}
	
#else
	time = vidPbCtrl.vidChunkCnt * vidPbCtrl.msPerFrame / vidPbCtrl.speed;
#endif
	time = time >> 8;
	return time;
}

#ifdef VIDEO_PB_BRC_EN
/**
 * @fn        UINT8 videoPbQtabSet(UINT32 aviHdrAddr)
 * @brief     video Pb Q table Set
 * @param     [in] aviHdrAddrWord
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbQtabSet(UINT32 aviHdrAddrWord)
{
	UINT8 sts = FAIL;
	UINT8 i =0;
	UINT8 *QTable;
	UINT32 offset = 0;
	
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(aviHdrAddrWord - offset );
	HAL_DmaDo(0,1,512,0,0);
	for(; i< 512/4;i++) {			 
		if(((XBYTE[0x3800+i] == 0xFF )&&(XBYTE[0x3800+i+1] == 0xDB)&&(XBYTE[0x3800+i+2] == 0x00)&&((XBYTE[0x3800+i+3] == 0xC5)||(XBYTE[0x3800+i+3] == 0x84)))) {
			sts = SUCCESS;
			break;
		} 
	}

	if(sts == FAIL) {		 
		ASSERT(0, 1);
		return sts;
	} 
	QTable = G_ucStorData + i + 5;

	jpegQTableSet( 0, QTable, QTable + 65, 1 );

	return sts;
}
#endif

//xian ++ 20090811
/**
 * @fn        UINT8 videoPbDispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac)
 * @brief     video Playback Display Scale Set
 * @param     [in] hsize
 * @param     [in] vsize
 * @param     [in] zoomfac
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbDispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac)
{
	videoPbSizeGet(&hsize, &vsize);
	return HAL_DispScaleSet(hsize, vsize, zoomfac,1);
}

/**
 * @fn		  void videoPbExit(void)
 * @brief	  video Playback Exit
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoPbExit(void)
{
	UINT32 srcAddr, dstAddr, tmpAddr;
	UINT16 pvW, pvH;
	UINT8 dispFlip;
	UINT16 pbBufWidth, pbBufHeight;
	UINT16 pbRoiHSize, pbRoiVSize, pbRoiHOff, pbRoiVOff;

#if 1//VD_PB_HDMI_EN
	dispFlip = (vidPbCtrl.dispFlip + 1) % 3;
#else
	dispFlip = vidPbCtrl.VLCFlip;
#endif

	if(dispFlip==0){ //get last displayed buffer
		srcAddr = K_SDRAM_AviPbBufAddrC;
	}		 
	else if(dispFlip==1){
		srcAddr = K_SDRAM_AviPbBufAddrA;
	}
	else{
		srcAddr = K_SDRAM_AviPbBufAddrB;
	}
	videoPbSizeGet(&pvW, &pvH);
	pbDecodeSizeGet(&pbBufWidth, &pbBufHeight);	//pb buffer size
	pbDispRoiSizeGet(&pbRoiHOff, &pbRoiVOff, &pbRoiHSize, &pbRoiVSize); // pb roi

#if 0
	if(pvW == 320 && pvH == 240){ //same with pb buffer size
		pbDispInit(srcAddr, pvW, pvH);
	}
	else{
		dstAddr = K_SDRAM_AviPbBufAddrC + (UINT32)pvW * pvH;
		tmpAddr = dstAddr + (UINT32)320 * 240;

		HAL_GprmScale(srcAddr, pvW, pvH, 
			dstAddr, (UINT16)320, (UINT16)240, tmpAddr, 1);
		HAL_DispScaleSet(320, 240, 100);
		HAL_PbBufHsizeSet(320);
		//while((XBYTE[REG_Disp_PnlSel] & 0x04) != 0x04);
		//while((XBYTE[REG_Disp_PnlSel] & 0x04) == 0x04);
		HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);
		HAL_PbBufAddrSet(dstAddr);

		HAL_GprmCopy(dstAddr, 320, 240, 0, 0, srcAddr, 320, 240, 0, 0, 320, 240, 0, 1);
		pbDispInit(srcAddr, (UINT16)320, (UINT16)240);		
	}
#else//video buff size = dec size
	if(pvW != pbBufWidth || pvH != pbBufHeight){ //same with pb buffer size	
		dstAddr = K_SDRAM_AviPbBufAddrC + (UINT32)pvW * pvH;
		tmpAddr = dstAddr + (UINT32)pbRoiHSize * pbRoiVSize;
		
		HAL_GprmScale(srcAddr, pvW, pvH, 
			dstAddr, (UINT16)pbRoiHSize, (UINT16)pbRoiVSize, tmpAddr, 1); 
		
		if(pbRoiHOff || pbRoiVOff){
			srcAddr = dstAddr;
			dstAddr = tmpAddr;
			
			gprmBlackFill(dstAddr, pbBufWidth, pbBufHeight);			
			HAL_GprmCopy(srcAddr, pbRoiHSize, pbRoiVSize, 0, 0, 
				dstAddr, pbBufWidth, pbBufHeight, pbRoiHOff, pbRoiVOff, pbRoiHSize, pbRoiVSize, 0, 1);
			
			HAL_DispPbFrameBufferSet(dstAddr, pbBufWidth, pbBufHeight);	
			HAL_DispScaleSet(pbBufWidth, pbBufHeight, 100,1);
			dispFitShow(pbBufWidth, pbBufHeight, 1);
		}
	}
	else{
		dstAddr = srcAddr;
	}
	
	pbDispInit(dstAddr, pbBufWidth, pbBufHeight);
#endif	
}

//xian --

#if 1//VD_PB_HDMI_EN
void videoFillAudRingBuf(UINT8 cnt)
{
	UINT32 AudBufCnt;
	UINT8 maxCnt;
	UINT8 sts;
	
	HAL_AudRingBufCntRead(&AudBufCnt );
	
	maxCnt = (AUDBUFSIZE - AudBufCnt)/CHUNKSIZE;
	if(G_VPBupSmpflag){
		maxCnt >>= 2;
	}
	if(cnt > maxCnt){
		cnt = maxCnt;
	}
	
	if(AudBufCnt < AUDBUFSIZE*7/8) 
	{
		while(cnt--){
			vidPbCtrl.nextAudIdx  = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1,1);
			if(vidPbCtrl.nextAudIdx ) {//if audio only, 0th chunck == 01wb
				sts = videoPbAudioChunck(vidPbCtrl.AVIIdxAddr,vidPbCtrl.audAddr ,vidPbCtrl.nextAudIdx);
				if(sts == SUCCESS) {
					vidPbCtrl.audChunkCnt ++;
					vidPbCtrl.lastAudIdx = vidPbCtrl.nextAudIdx;
					vidPbCtrl.feedData = 0; //xian ++ 20081015, read aud success
				} else {
					//printf("%d: %lu, %bu\n",(UINT16)__LINE__, vidPbCtrl.audChunkCnt, cnt);
					break;
				}
			} else{
				//printf("%d\n",(UINT16)__LINE__);
				vidPbCtrl.audDown = 1;
				break;
			}

//			if(vidPbCtrl.nextAudIdx != vidPbCtrl.lastAudIdx + 1) {
//				break;
//			}
		}
		//printf("aud idx:%lu\n", vidPbCtrl.nextAudIdx);
	}
}

void videoPbStreamRead(UINT16 fHandle, UINT32 pos, UINT32 maxSize)
{
	UINT32 VlcCount;
	UINT8 sts;
	videoPbVLCRingBufCntGet(&VlcCount);
	if(VlcCount>= 2048) {
//		VlcCount = ((VlcCount>>11)<<11);
		if(!maxSize){
			VlcCount >>= 1; //half
		}
		else if(VlcCount > maxSize){ // maxSize 2048 aligment
			VlcCount = maxSize;
		}
		VlcCount = ((VlcCount>>11)<<11);

		if(VlcCount + pos > gAviFileSize){ //xian fix #41851
			VlcCount = gAviFileSize - pos;
		}
		sts = vfsFileRead(fHandle,	K_DOS_DstDram, VlcCount , K_SDRAM_AviPbFragAddr + (VlcRingBufInIdx>>1), pos);
		if(sts==FALSE){//read file error, #41746
			vidPbCtrl.audDown = 1;
			vidPbCtrl.vidDown = 1;
			return;
		}
		
		dataOffset += VlcCount ;
		VlcRingBufInIdx += VlcCount;
		secAddr_max =  dataOffset - AVI_512_ALIGN(AviHeaderSize); //xian 20081015
	} else {
		//printf("buf full\n");
		return;
	}

	if(VlcRingBufInIdx > vidPbFragBufSize) {
		VlcRingBufInIdx = VlcRingBufInIdx&(vidPbFragBufSize-1);
		HAL_GprmDramDma((K_SDRAM_AviPbFragAddr <<1) + vidPbFragBufSize, (K_SDRAM_AviPbFragAddr <<1), VlcRingBufInIdx, 1);
	}

}

void videoPb(void) USING_0
{
	UINT32 AudBufCnt;//, VlcBufCnt;
	UINT32 time =0;
	UINT32 tmpTime =0;
	UINT8 sts;
	UINT32 srchVidStep;
	UINT8 audOver = 0;
	UINT8 dispFlip;

	if(vidPbCtrl.Idle != VID_PB_RUN) {
		if(vidPbCtrl.Idle == VID_PB_PAUSE) {
			return;
		}
		if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL || slowMode) {
			videoPbFast();
		}
		return;
	}

	if(nonAudioFile){
		HAL_AudPause();
		if( !vidPbCtrl.playAudFlg ){
			vidPbCtrl.playAudFlg  = 1;
	    	HAL_GlobalReadGTimer(&gPlayStartTime); 
	    	gPlayTime = gPlayStartTime;
		}		
	}else{
		if(XBYTE[REG_Audio_AudBufUnf] & 0x01){
			#if VD_DEBUG_STATUTE
			if(vdTotalLoop){
				printf("data over: %lu\n", vdTotalLoop);
				audOver = 1;
			}
			#else
			if(vidPbCtrl.playAudFlg == 1){
				printf("data over %bu!\n", vid_flag);
				//printf("    :%lu, %lu\n", vidPbCtrl.lastVidIdx, vidPbCtrl.vidChunkCnt);
				//printf("    :%lu, %lu\n", vidPbCtrl.lastAudIdx, vidPbCtrl.audChunkCnt);
				audOver = 1;
			}
			#endif
			XBYTE[REG_Audio_AudBufUnf] &= 0x01;
		}

		#if VD_DEBUG_STATUTE
		vdTotalLoop ++;
		#endif
		
	/////read audio at first
		if(!vidPbCtrl.playAudFlg && !vidPbCtrl.audDown) {
			HAL_AudPause();
			if((G_VPBupSmpflag)){		//for upsampling	
				audioPbSet(vidPbCtrl.audAddr ,(UINT32)AUDBUFSIZE, (UINT16)vidPbCtrl.audFmt, (UINT32)vidPbCtrl.audFrq<<2, (UINT32)vidPbCtrl.audAlign, (UINT8)vidPbCtrl.audChnnel,(UINT8)vidPbCtrl.audBit);
				videoFillAudRingBuf(6);
			}
			else {
				audioPbSet(vidPbCtrl.audAddr ,(UINT32)AUDBUFSIZE, (UINT16)vidPbCtrl.audFmt, (UINT32)vidPbCtrl.audFrq, (UINT32)vidPbCtrl.audAlign, (UINT8)vidPbCtrl.audChnnel,(UINT8)vidPbCtrl.audBit);
				videoFillAudRingBuf(24);
			}
					
			//printf("IDX1=%lx,cnt=%lx\n",vidPbCtrl.audInIdx,vidPbCtrl.audChunkCnt);
			vidPbCtrl.playAudFlg  = 1;
			HAL_AudResume();
		}
		else{
			HAL_AudRingBufCntRead(&AudBufCnt );
			if((G_VPBupSmpflag)){		//for upsampling	
				if(AudBufCnt <= 16) {
					//printf("%d\n",(UINT16)__LINE__);
					//ASSERT(0, 1);
					videoPbAbort();
					return;
				}
			}
			else {
				if(AudBufCnt <= 64) {
					//printf("%d\n",(UINT16)__LINE__);
					//ASSERT(0, 1);
					videoPbAbort();
					return;
				}
			}			
		}
	}
/////search video frame	
	srchVidStep = 0;
	if( !vidPbCtrl.vidDown && vid_flag != 0xff) {
		if(!vid_flag || vid_flag == VD_PB_STS_JPG_WAIT_VLC) {				
			srchVidStep = vidPbCtrl.TimeExceed / vidPbCtrl.msPerFrame + 1; //xian ++ 20081231, #34616

			//printf("t=%ld\n",srchVidStep);
			if(vid_flag == VD_PB_STS_JPG_WAIT_VLC){//xian ++ 20110511 #42609
				if(srchVidStep>3){
					srchVidStep = 3;
				}
			}
			
			#if VD_DEBUG_STATUTE
			if(srchVidStep > 1){
				//vdMaxExceed += srchVidStep - 1;
				//printf("vd exceed:%lu\n", srchVidStep);
				if(srchVidStep > vdMaxExceed)
					vdMaxExceed = srchVidStep;
			}
			#endif

			//printf("step1=%ld\n",srchVidStep);
			vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr ,vidPbCtrl.lastVidIdx , vidPbCtrl.chunckCnt ,srchVidStep, 0);
			#if VD_TEST_MODE
			if(audOver||testMode){
				printf("---srch: step=%lu, next=%lu, cnt=%lu\n", srchVidStep, vidPbCtrl.NextVidIdx, vidPbCtrl.vidChunkCnt);
			}
			#endif
			//printf("next=%ld\n",vidPbCtrl.NextVidIdx);
			if(vidPbCtrl.NextVidIdx /*|| (vidPbCtrl.mediaType == 0)*/) {
				if(!vid_flag){
					vid_flag = VD_PB_STS_IDX_FOUND; //find next index
				}
			}
			else{
				printf("%d\n",(UINT16)__LINE__);
				videoPbAbort();
				return;
			}
		}


/////read video frame			
//		if(vid_flag == VD_PB_STS_IDX_FOUND) {
		if(vid_flag == VD_PB_STS_IDX_FOUND || vid_flag == VD_PB_STS_JPG_WAIT_VLC) {
			#if VD_TEST_MODE
			if(audOver||testMode){
				printf("---rd:%lu, %lu\n", vidPbCtrl.NextVidIdx, vidPbCtrl.vidChunkCnt);
			}
			#endif

			sts = videoPbFrameRead(vidPbCtrl.AVIIdxAddr , vidPbCtrl.fHandle,vidPbCtrl.NextVidIdx , vidPbCtrl.VLCFlip);
			if(sts == SUCCESS) {
				if(vid_flag == VD_PB_STS_IDX_FOUND){
					vid_flag = VD_PB_STS_VLC_RDY; //found vlc, the dumy frame is processed in videoPbDecode()
				}
				else{
					vid_flag = VD_PB_STS_JPG_WAIT_VLC_RDY;
				}
			}else {					
				vidPbCtrl.vidChunkCnt -= srchVidStep;	//#41981, maybe search idx again
			
				vidPbCtrl.dummyFrame =0;
				if(!nonAudioFile){
					HAL_AudRingBufCntRead(&AudBufCnt );				
					if((G_VPBupSmpflag)){ 	//for upsampling	
						if(AudBufCnt <= 16) {
							printf("%d\n",(UINT16)__LINE__);
							//ASSERT(0, 1);
							videoPbAbort();
							return;
						}
						videoFillAudRingBuf(1);
					}
					else {
						if(AudBufCnt <= 64) {
							printf("%d\n",(UINT16)__LINE__);
							//ASSERT(0, 1);
							videoPbAbort();
							return;
						}
						videoFillAudRingBuf(3);
					}
					//printf("IDX2=%lx\n",vidPbCtrl.audInIdx);
					printf("%d, %lu, %lu\n",(UINT16)__LINE__, vidPbCtrl.lastVidIdx, vidPbCtrl.lastAudIdx);
					//printf("** 0x%lx, 0x%lx\n", secAddr_min, secAddr_max);
					//printf("** 0x%lx, 0x%lx\n", secAddr_min_aud, secAddr_min_vid);
					//printf("** 0x%lx, 0x%lx\n", VlcRingBufOutIdx, VlcRingBufInIdx);
					//return ; // data or buf, should check jpg decode eof to release buffer
				}
			}
			
		}

/////dec vlc
		//should check jpg decode eof to release buffer
		sts = videoPbDecode(vidPbCtrl.jpgStartAddr, vidPbCtrl.VLCFlip, 0, vidPbCtrl.secAddr_jpgEnd);
		//if(vid_flag == VD_PB_STS_VLC_RDY){
		if(vid_flag == VD_PB_STS_JPG_TRIG){
			#if VD_TEST_MODE
			if(audOver){
				printf("---dec: idx=%lu\n", vidPbCtrl.NextVidIdx);
			}
			#endif
			//sts = videoPbDecode(vidPbCtrl.jpgStartAddr, vidPbCtrl.VLCFlip, 0, vidPbCtrl.secAddr_jpgEnd);
			if(sts ==SUCCESS){
				vid_flag = VD_PB_STS_START;
				videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, 102400);	
				if(!nonAudioFile){
					if((G_VPBupSmpflag)){ 	//for upsampling	
						videoFillAudRingBuf(1);
					}
					else {
						videoFillAudRingBuf(3);
					}	
				}
				//printf("IDX3=%lx\n",vidPbCtrl.audInIdx);
			}
		}

//////disp		
		HAL_AudRingBufCntRead( &AudBufCnt);
		vidPbCtrl.vidFrameTime = videoPbTimeGet();		//audio time
		vidPbCtrl.vidFrameTime =(UINT32)(vidPbCtrl.vidFrameTime << 8);

		#if VD_TEST_MODE
		if(audOver||testMode){
			printf("here..%lx, %lx\n", vidPbCtrl.vidFrameTime, vidPbCtrl.frameTime[vidPbCtrl.dispFlip]);
			printf("%lx, %lx, %lx\n", AudBufCnt, vidPbCtrl.audChunkCnt, audChunkSize);
		}
		#endif
		vidPbCtrl.TimeExceed = 0;
		time = vidPbCtrl.vidFrameTime; //audio time
		
		if(!nonAudioFile){
			if(vidPbCtrl.audFrq == 11025 && !G_VPBupSmpflag){ // should not speed up too often if 11025 audio
				AudBufCnt <<= 2;
			}
		}
		#if 0	//xian -- 20110725	
		if(vid_flag != VD_PB_STS_START){//speed up display if wait buff
			if(AudBufCnt < AUDBUFSIZE/2 ){
				vidPbCtrl.feedData = 1; // should display to free buffer, aud maybe over
			}
			//time += vidPbCtrl.msPerFrame;
		}
		else if(AudBufCnt < AUDBUFSIZE/4){
			vidPbCtrl.feedData = 1; // should display to free buffer, aud maybe over
		}
		#else
		if(!nonAudioFile){
			if(AudBufCnt < AUDBUFSIZE/4){
				vidPbCtrl.feedData = 1; // should display to free buffer, aud maybe over
			}
		}
		#endif
		if(((time >= vidPbCtrl.frameTime[vidPbCtrl.dispFlip]/* + (vidPbCtrl.msPerFrame>>1)*/))
			|| (vidPbCtrl.playAudFlg == 2) //release from fw/rw, timer not work yet, so disp directly
			|| (vidPbCtrl.feedData))
		{ //xian 20081015
			tmpTime = (vidPbCtrl.vidFrameTime>>8)/1000;
			if((recTime) != tmpTime) {
				uiUpdateVideoRecTime(tmpTime);
				recTime = tmpTime;
			}

			//xian -- for 1528 avi
			//vidPbCtrl.dummyFrame = 0; 
			if(vidPbCtrl.frameDummyCnt[vidPbCtrl.dispFlip] == 0){
				if(vidPbCtrl.dispFlip==0){
					dispFlip = 1 ;	//next
				} else if(vidPbCtrl.dispFlip==1){
					dispFlip = 2 ;
				} else {
					dispFlip = 0 ;
				}
			}
			else{ //xian ++, 20110923, current frame has dummy
				dispFlip = vidPbCtrl.dispFlip;
				vidPbCtrl.frameTime[dispFlip] += vidPbCtrl.msPerFrame;///g_msPerFrame_fraction; 
				vidPbCtrl.frameDummyCnt[vidPbCtrl.dispFlip] --;
			}
			
			time = vidPbCtrl.frameTime[dispFlip]; //next disp frame time	
			if(vidPbCtrl.vidFrameTime > time) { // audio play time < video frame time, vid is not slow
				vidPbCtrl.TimeExceed =	vidPbCtrl.vidFrameTime	- time; //vd slow
			}

			if(vidPbCtrl.feedData && (secAddr_min_vid + vidPbFragBufSize - 150000L < secAddr_min_aud)){ //150K almost one frm size
				vidPbCtrl.TimeExceed += vidPbCtrl.msPerFrame; //speed up vd to free buf, #41554
			}
			
			//printf("down: %bx\n", XBYTE[REG_Jpeg_Intr_Event]);
			//printf("---disp: yuv[%bu], jpg[%bu]\n", dispFlip, vidPbCtrl.VLCFlip);
			//printf("   pts[3]: %lu, %lu, %lu\n", vidPbCtrl.frameTime[0], vidPbCtrl.frameTime[1], vidPbCtrl.frameTime[2]);	
			if(dispFlip != vidPbCtrl.VLCFlip || vidPbCtrl.vidDown){
				if(dispFlip != vidPbCtrl.dispFlip){
					vidPbCtrl.dispFlip = dispFlip; //current disp

					if(dispFlip==0) {
						HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrA);
					}else if(dispFlip==1){
						HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrB);
					}else {
						HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrC);
					}
				}

				gVdDispCnt ++;
				
				#if VD_DEBUG_TIMING
				vdTimerGapTrig(0);
				#endif

				if(vidPbCtrl.vidDown){
					vid_flag = VD_PB_STS_LAST_FRAME_RDY; //last frame
				}

				if(vidPbCtrl.playAudFlg == 2){ //release from fw/rw
					HAL_AudResume();
					vidPbCtrl.playAudFlg = 1; //normal
				}				
			}
			else{
				#if VD_DEBUG_STATUTE
				vdDispWaitBuf ++;
				#endif
				//printf("pb drop\n");
			}
		}
		else {
			if(vidPbCtrl.vidDown){
				printf("%d\n",(UINT16)__LINE__);
				videoPbAbort();
				return;
			}
			
			if(vidPbCtrl.audDown && !nonAudioFile){
				HAL_AudRingBufCntRead(&AudBufCnt );
				if((G_VPBupSmpflag)){ 	//for upsampling	
					if(AudBufCnt <= 16) {
						printf("%d\n",(UINT16)__LINE__);
						videoPbAbort();
						return;
					}
				}
				else {
					if(AudBufCnt <= 64) {
						printf("%d\n",(UINT16)__LINE__);
						videoPbAbort();
						return;
					}
				}
				//printf("audio done\n");
			}			
		}

	}
	else{
		printf("%d\n",(UINT16)__LINE__);	
		videoPbAbort();
		return;
	}

	if(vidPbCtrl.feedData && !nonAudioFile){
		if((G_VPBupSmpflag)){ 	//for upsampling	
			videoFillAudRingBuf(1); // not disp, so audio may be over		
		}
		else {
			videoFillAudRingBuf(12); // not disp, so audio may be over	
		}	
		//printf("IDX4=%lx\n",vidPbCtrl.audInIdx);
		vidPbCtrl.feedData = 0;
	}
	//xian ++ 20110511 #42609
	if(!nonAudioFile){
		videoPbFreeBufSpeedup();
	}
	updateRingBufIdx();
	//xian --
	
	return;
}

UINT8 videoPbFrameRead_Fast(UINT32 AVIIdxAddrWord, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip) USING_0
{
	UINT32	 AviSize;
	UINT32	 AviAddr;
	UINT32	 PBVideoAddr;
	UINT32	 pos;
	UINT32 AviAddrEnd;

	AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+4);
	AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+6);

	if(AviSize == 0) {//skip dummy frame
		vidPbCtrl.dummyFrame = 1;

		while((!AviSize)&&vidPbCtrl.NextVidIdx) {
			if(vidPbCtrl.Idle == VID_PB_RW) {
				vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr , vidPbCtrl.NextVidIdx , vidPbCtrl.chunckCnt, 1, 0);
			}else {
				vidPbCtrl.NextVidIdx =	videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr ,vidPbCtrl.NextVidIdx , vidPbCtrl.chunckCnt ,1, 0);
			}
			
			AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(vidPbCtrl.NextVidIdx)*8+4);
			AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(vidPbCtrl.NextVidIdx)*8+6);
		}
		if(vidPbCtrl.NextVidIdx ==0) {
			vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
			return FAIL;
		}
		vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
	}
	
#ifdef VIDEO_PB_BRC_EN
	AviAddr = AviAddr + (UINT32)AviHeaderSize + 16;
#else	 
	AviAddr = AviAddr + (UINT32)AviHeaderSize + 16 + (UINT32)VidHeaderSize;//point to VLC
	AviSize -= VidHeaderSize;	//vlc size
#endif
	AviAddrEnd = AviAddr + AviSize;

	pos = (UINT32)(AviAddr>>9) << 9; //read file offset
	dataOffset = pos;	//for release reading 

	PBVideoAddr = K_SDRAM_AviPbFragAddr + ((AviAddr - pos) >> 1);
	AviSize += AviAddr - pos;
	AviSize = ((AviSize>>9) + 1) << 9;


	while((XBYTE[REG_Jpeg_Intr_Event] & 0x04) == 0x00) {//should wait jpg end
	}
	
	vfsFileRead(fHandle,K_DOS_DstDram, AviSize, K_SDRAM_AviPbFragAddr, pos);
		
#ifdef VIDEO_PB_BRC_EN 
	//videoPbQtabSet(PBVideoAddr);
	//PBVideoAddr += VidHeaderSize>>1;
	PBVideoAddr += VidHeaderSize>>1;

#endif
	//videoPbDecode(PBVideoAddr, VLCFlip, 1);
	vidPbCtrl.jpgStartAddr = PBVideoAddr;
	vidPbCtrl.secAddr_jpgEnd = AviAddrEnd;
	return SUCCESS;
	VLCFlip = VLCFlip;
}

#pragma OT(8, SPEED)
void videoJpgWait(void)
{		
	while((XBYTE[REG_Jpeg_Intr_Event] & 0x04) == 0x00) {
	}//wait decode finished 	
}

#pragma OT(8, SPEED)
UINT8
HAL_DispSignalWait_Imem(
	UINT8 src,
	UINT8 mode
)
{
#if 1
	src = src;
	mode = mode;
	return 0;
#else //xian --
	UINT16 timeout;


	/* if dispEn or dispPhaseClkEn disabled, no need to wait. */
	if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
		return FAIL;
	}

	/* wait for specified event. */
	timeout = HAL_DISP_WAIT_TIMEOUT;

	while ( 1 ) {
		if ( XBYTE[REG_Disp_DispSignal] & src ) {  /* high detected */
			if ( mode == HAL_DISP_WAIT_FALLING ) {
				break;
			}
			if ( mode == HAL_DISP_WAIT_HIGH ) {
				return SUCCESS;
			}
		}
		else {  /* !(XBYTE[REG_Disp_DispSignal] & src) */  /* low detected */
			if ( mode == HAL_DISP_WAIT_RISING ) {
				break;
			}
			if ( mode == HAL_DISP_WAIT_LOW ) {
				return SUCCESS;
			}
		}

		timeout--;
		if ( timeout == 0 ) {
			return FAIL;
		}
	}

	while ( 1 ) {
		if ( !(XBYTE[REG_Disp_DispSignal] & src) ) {  /* low detected */
			if ( mode == HAL_DISP_WAIT_FALLING ) {
				return SUCCESS;
			}
		}
		else {  /* XBYTE[REG_Disp_DispSignal] & src */  /* high detected */
			if ( mode == HAL_DISP_WAIT_RISING ) {
				return SUCCESS;
			}
		}

		timeout--;
		if ( timeout == 0 ) {
			return FAIL;
		}
	}
#endif
}
UINT8 videoPbDecode(UINT32 addrWord, UINT8 VLCFlip, UINT8 wait, UINT32 aviAddrEnd)
{
	UINT8 opt;
	UINT32 dstAddr, srcAddr;
	UINT16 dramread;
	UINT32 time;
//	UINT16 w, h;
	static UINT32 lastAviAddrEnd;

	if(vid_flag == VD_PB_STS_JPG_WAIT_VLC){
		if(secAddr_min_vid < secAddr_min_aud){ //xian ++, also free aud chunk #41624 
			secAddr_min_vid = secAddr_min_aud;		
			updateRingBufIdx();
		}
		
		return SUCCESS;		
	}
	else if(vid_flag == VD_PB_STS_JPG_WAIT_VLC_RDY){
		VLCFlip = vidPbCtrl.VLCFlip;		
	}	
	else if(!wait && gVdJpgCnt){
		//if jpg dec finished	
		if(vidPbCtrl.VLCFlip==0){
			srcAddr = K_SDRAM_AviPbBufAddrA;
			dstAddr = K_SDRAM_AviPbBufAddrB;
			VLCFlip = 1 ;	//next buffer
		} else if(vidPbCtrl.VLCFlip==1){
			srcAddr = K_SDRAM_AviPbBufAddrB;
			dstAddr = K_SDRAM_AviPbBufAddrC;
			VLCFlip = 2 ;
		} else {
			srcAddr = K_SDRAM_AviPbBufAddrC;
			dstAddr = K_SDRAM_AviPbBufAddrA;
			VLCFlip = 0 ;
		}


		if(VLCFlip == vidPbCtrl.dispFlip  ){
			#if VD_DEBUG_STATUTE
			vdVlcWaitBuf ++;
			#endif
			//printf("vlc drop\n");
			return FAIL; //dont wait jpg end if next buf not free
		}
			
		#if 0 //xian -- 20110923, move down			
		vidPbCtrl.VLCFlip = VLCFlip;
		#endif
		//printf("Vlc: %bx\n",VLCFlip);
		if(vidPbCtrl.playAudFlg != 2){ //fix #40967, lastAviAddrEnd is not correct when resume
			secAddr_min_vid = lastAviAddrEnd;		  
		}
		//vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx; //xian move down, #41226
		updateRingBufIdx();

		
		#if VD_DEBUG_TIMING
		//vdTimerStart(2);	
		#endif
		
		videoJpgWait();
		if(vidPbCtrl.dummyFrame){ //for 1528 avi
			//printf("dumy:%lu\n", vidPbCtrl.NextVidIdx);
			#if 0//xian -- 20110923
			HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0); //wait pbzfupdate done ready
			videoPbSizeGet(&w, &h);
			HAL_GprmDramDma(srcAddr<<1, dstAddr<<1, (UINT32)w*h<<1, 1);
			gVdJpgCnt ++;
			
			videoPbMsGet(&time);	//this fram time
			vidPbCtrl.frameTime[VLCFlip] = time;
			#else
			vidPbCtrl.frameDummyCnt[vidPbCtrl.VLCFlip] ++;
			#endif

			vidPbCtrl.dummyFrame = 0;
			vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx; //fix 41500
			vid_flag = VD_PB_STS_JPG_TRIG;
			return SUCCESS;
		}
		#if 1// xian ++ 20110923
		else{
			vidPbCtrl.VLCFlip = VLCFlip;
			vidPbCtrl.frameDummyCnt[VLCFlip] = 0;
		}
		#endif		
		if(vid_flag != VD_PB_STS_VLC_RDY){//xian ++ 20100928, for release buf, #41256
			videoPbMsGet(&time);
			vidPbCtrl.frameTime[VLCFlip] = time;
			vid_flag = VD_PB_STS_JPG_WAIT_VLC;
			return SUCCESS;
		}
		
		#if VD_DEBUG_TIMING
		//vdTimerAvgLengthEnd(2);
		#endif

	}
	
	XBYTE[REG_Jpeg_Intr_Event]=0xFF; 
	lastAviAddrEnd = aviAddrEnd; //init lastAviAddrEnd here, cannot move away
	vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx; //xian ++, #41226
	
	if(wait){
		opt = 0x04;
	}
	else{
		opt = 0x0c;
	}
	if(VLCFlip == 0) {
		dstAddr = K_SDRAM_AviPbBufAddrA;
	} else if(VLCFlip == 1){
		dstAddr = K_SDRAM_AviPbBufAddrB;
	} else {
		dstAddr = K_SDRAM_AviPbBufAddrC;
	}
	if (1){// tq.zhang video yuv420
		dramread=videoPbReadDword(addrWord-(UINT32)11);
		//printf("dramread=0x%x=%d\n",dramread,dramread);
		dramread &= 0x00ff;
		if (dramread==0x0021){
			opt &= 0xfc;
			opt |= 0x00;
		}
		else if (dramread==0x0022){ //420
			opt &= 0xfc;
			opt |= 0x01;
		}
	}

	dstAddr += gJpgDecAddrOff;
	videoPbQtabSet(addrWord - ((UINT32)VidHeaderSize>>1));
	//videoPbQtabSet(addrWord - ((UINT32)VidHeaderSize>>1));
	if( dispDevActGet() != 4 || G_AVIWidth < 1280 ){ //xian ++ 20110725, don't wait dramzupdate to speed up hdmi, workaround
		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0); //wait pbzfupdate done ready, #43043
	}	
	HAL_JpegDecode(addrWord, G_AVIWidth, G_AVIHeight, dstAddr, gJpgDecFact, opt, 0x00);

	#if VD_DEBUG_TIMING
	vdTimerGapTrig(1);
	#endif
	gVdJpgCnt ++;
	
	videoPbMsGet(&time);	//this fram time
	vidPbCtrl.frameTime[VLCFlip] = time;

	vid_flag = VD_PB_STS_JPG_TRIG;
	
	return SUCCESS; // trigger success 
}

UINT8 videoPbFrameRead(UINT32 AVIIdxAddrWord, UINT16 fHandle, UINT32 frameNo, UINT8 VLCFlip) USING_0
{
	UINT32	AviAddrEnd;
	UINT32	AviSize;
	UINT32	AviAddr;
	UINT32	tmp_SecAddr;
	UINT32	tmp_SecAddrEnd;
	UINT32	copySize;
	static UINT32 lastFrameNo;
	//UINT8 sts, i;
	//UINT32 AudBufCnt;
//	UINT32 maxRead = 102400;//307200; //300K  33ms
	

//	if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL) {
//		return videoPbFrameRead_Fast(AVIIdxAddrWord, fHandle, frameNo, VLCFlip);
//	}

	AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+4);
	AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(frameNo)*8+6);

	if(AviSize == 0) {
		vidPbCtrl.dummyFrame = 1;
		vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
		secAddr_min_vid = (UINT32)16 + AviAddr;
		updateRingBufIdx();
		return SUCCESS;	
	}	
#ifdef VIDEO_PB_BRC_EN
	AviAddr = (UINT32)16 + AviAddr + AviHeaderSize%512 ;
#else
	AviAddr = (UINT32)16 + AviAddr + VidHeaderSize; //point to vlc
	AviSize -= VidHeaderSize;
#endif
	AviAddrEnd = AviAddr + AviSize;

	if(AviAddr >= secAddr_min) {
		if(AviAddrEnd > secAddr_max){//data not enough
			videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, AviAddrEnd - secAddr_max + 20480); // only one fram enough	
		}
		
		if(AviAddrEnd <= secAddr_max) { //data enougth
			tmp_SecAddr = VlcRingBufOutIdx + (AviAddr - secAddr_min);
			tmp_SecAddr = tmp_SecAddr & (vidPbFragBufSize-1);								 
			tmp_SecAddrEnd = tmp_SecAddr + AviSize;
		   
			if(tmp_SecAddrEnd > vidPbFragBufSize){							   
				// copy data			
				copySize = tmp_SecAddrEnd - vidPbFragBufSize;
				if(copySize < ((UINT32)K_SDRAM_AviPbFragSize<<1) - vidPbFragBufSize){
					HAL_GprmDramDma(((UINT32)K_SDRAM_AviPbFragAddr <<1), ((UINT32)K_SDRAM_AviPbFragAddr <<1) + vidPbFragBufSize, copySize, 1);
				}
				else{//buf not enough
					ASSERT(0, 1);
					return FAIL;
				}
			}			
#ifdef VIDEO_PB_BRC_EN 
			//xian ++ 20090701 fix bug
//			videoPbQtabSet(K_SDRAM_AviPbFragAddr + (tmp_SecAddr>>1));//move to videoPbdecode()
			//tmp_SecAddr += (UINT32)VidHeaderSize;
			tmp_SecAddr += (UINT32)VidHeaderSize;
			//xian --
#endif
#if 0//move to videoPbdecode()
			sts = videoPbDecode(K_SDRAM_AviPbFragAddr + (tmp_SecAddr>>1), VLCFlip, 0, AviAddrEnd);
			if(sts == SUCCESS){
				videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, maxRead);				
				videoFillAudRingBuf(4);
			}
			else{ //wait yuv buf
				videoFillAudRingBuf(2);
			}
#else
			vidPbCtrl.jpgStartAddr = K_SDRAM_AviPbFragAddr + (tmp_SecAddr>>1);
			if(vidPbCtrl.secAddr_jpgEnd != AviAddrEnd){
				vidPbCtrl.secAddr_jpgEnd = AviAddrEnd;
			}
			else if(lastFrameNo != frameNo){ // for 1528 avi
				//printf("dm2:%lu, %lx, %lx\n", vidPbCtrl.NextVidIdx, AviAddrEnd, frameNo);
				vidPbCtrl.dummyFrame = 1;
			}
			lastFrameNo = frameNo;
#endif

			
		} 
		else {//data not enough	
			//printf("%lu, %lu\n", vidPbCtrl.lastVidIdx, vidPbCtrl.vidChunkCnt);
			//printf("%lu, %lu\n", vidPbCtrl.lastAudIdx, vidPbCtrl.audChunkCnt);
			//ASSERT(0, 1);
			return FAIL;
		}
	} 
	else {
		#if VD_DEBUG_TIMING		
		vdTestCnt[0] ++;
		#endif
		
		//xian ++ 20090915
		vidPbCtrl.dummyFrame = 1;
		//printf("dm1:%lu\n", vidPbCtrl.NextVidIdx);
		//xian -- for 1528 avi
		//vidPbCtrl.lastVidIdx = vidPbCtrl.NextVidIdx;
		//updateRingBufIdx();
		return SUCCESS;	
		//xian -- 20090811
	}

//xian -- test, move to voidPbDecode
//	secAddr_min_vid = AviAddrEnd;		  
//	updateRingBufIdx();
//
//	while((XBYTE[REG_Jpeg_Intr_Event] & 0x04) == 0x00) {
//	}//wait decode finished
//	XBYTE[REG_Jpeg_Intr_Event]=0xFF;

	return SUCCESS;
	fHandle = fHandle;
	VLCFlip = VLCFlip;	
}

void videoPbFast(void)
{
	UINT8 sts;
	UINT8 dispFlip;
	UINT32 time;

	if(vidPbCtrl.Idle == VID_PB_FW) {//xian 20080919, add slowMode
		videoPbFW();
		if((vidPbCtrl.lastVidIdx == 0) || (vidPbCtrl.lastVidIdx >= vidPbCtrl.chunckCnt)) {
			videoPbAbort();
			return;
		}
	} else if(vidPbCtrl.Idle == VID_PB_RW) {
		videoPbFR();
		if(vidPbCtrl.vidChunkCnt== 0) {
			videoPbAbort();
			return;
		}
	}

	#if VD_DEBUG_STATUTE
	vdTotalLoop ++;
	#endif

	/////read frame 		
	if(vid_flag==VD_PB_STS_IDX_FOUND){
		vidPbCtrl.NextVidIdx = vidPbCtrl.lastVidIdx;
		//printf("rd: %lu, %lu\n", vidPbCtrl.NextVidIdx, vidPbCtrl.vidChunkCnt);
		sts = videoPbFrameRead_Fast(vidPbCtrl.AVIIdxAddr , vidPbCtrl.fHandle,vidPbCtrl.NextVidIdx , vidPbCtrl.VLCFlip);
		if(sts == SUCCESS) {
			vid_flag = VD_PB_STS_VLC_RDY; //found vlc
		}
		else { 				
			videoPbAbort();
			return;
		}
	}
	
	/////dec vlc
	if(vid_flag == VD_PB_STS_VLC_RDY){
		sts = videoPbDecode(vidPbCtrl.jpgStartAddr, vidPbCtrl.VLCFlip, 0, vidPbCtrl.secAddr_jpgEnd);
		if(sts ==SUCCESS){
			vid_flag = VD_PB_STS_START;
		}
	}

	/////disp
	vidPbCtrl.dummyFrame = 0;
	
	if(vidPbCtrl.dispFlip==0){
		dispFlip = 1 ;	//next
	} else if(vidPbCtrl.dispFlip==1){
		dispFlip = 2 ;
	} else {
		dispFlip = 0 ;
	}
				
	if(dispFlip != vidPbCtrl.VLCFlip || vidPbCtrl.vidDown){
		vidPbCtrl.dispFlip = dispFlip; //current disp

		//HAL_DispSignalWait_Imem(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);
		HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);
		videoJpgWait();
		if(dispFlip==0) {
			HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrA);
		}else if(dispFlip==1){
			HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrB);
		}else {
			HAL_PbBufAddrSet(K_SDRAM_AviPbBufAddrC);
		}
		gVdDispCnt ++;
	}
	
	time = vidPbCtrl.frameTime[vidPbCtrl.dispFlip];
	time = (time>>8)/1000;
	if((recTime) != time) {
		uiUpdateVideoRecTime(time);
		recTime = time;
	}

}

void videoPbRelease(void)
{
	UINT32 time;
	UINT8 status;
	UINT32 audChunkCnt;
	UINT32 audOutBytes;

	ENTER_CRITICAL( status );

	vidPbCtrl.Idle = VID_PB_RUN;
	vidPbCtrl.playAudFlg = 2;	//resume
	//printf("\nrelease... %lu,%lu,%lu,%lu\n", vidPbCtrl.lastAudIdx, vidPbCtrl.lastVidIdx, vidPbCtrl.audChunkCnt, vidPbCtrl.vidChunkCnt);
	time = vidPbCtrl.frameTime[vidPbCtrl.VLCFlip];
	//printf("tm:%lu, %lu, %lu\n", time, bytePerSec, audChunkSize);
	if(!nonAudioFile){
		audOutBytes = (UINT32)(((float)time * (float)bytePerSec / (float)256000) );// should be float for #41065
		audChunkCnt = audOutBytes / audChunkSize ;	
		audOutBytes = audOutBytes % audChunkSize;	
		//printf("%lu, %lu\n", audChunkCnt, audOutBytes);

		//xian ++ 20100906
		if(vidPbCtrl.lastAudIdx < vidPbCtrl.lastVidIdx){
			vidPbCtrl.audChunkCnt = vidPbCtrl.lastVidIdx;
		}
		else{
			vidPbCtrl.audChunkCnt = vidPbCtrl.lastAudIdx;	
		}
		vidPbCtrl.audChunkCnt = vidPbCtrl.audChunkCnt + 1 - vidPbCtrl.vidChunkCnt;
		//printf("--   %lu,%lu,%lu,%lu\n", vidPbCtrl.lastAudIdx, vidPbCtrl.lastVidIdx, vidPbCtrl.audChunkCnt, vidPbCtrl.vidChunkCnt);
		//xian --
		
		if(audChunkCnt < vidPbCtrl.audChunkCnt){
			audChunkCnt = vidPbCtrl.audChunkCnt-audChunkCnt;
			vidPbCtrl.lastAudIdx = videoPbSearchAVIIndexBackward(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , audChunkCnt, 1);
		}
		/* xian -- 20100906
		while(audChunkCnt > vidPbCtrl.audChunkCnt){
			vidPbCtrl.lastAudIdx = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1, 1);
			if(!vidPbCtrl.lastAudIdx)
				break;
			vidPbCtrl.audChunkCnt ++;
		}
		*/
		//printf("--   %lu,%lu,%lu,%lu\n", vidPbCtrl.lastAudIdx, vidPbCtrl.lastVidIdx, vidPbCtrl.audChunkCnt, vidPbCtrl.vidChunkCnt);

		if(!vidPbCtrl.lastVidIdx || !vidPbCtrl.lastAudIdx){
			EXIT_CRITICAL( status );
			return;
		}

		//set audio index	
		vidPbCtrl.audInIdx = (vidPbCtrl.audChunkCnt * audChunkSize) % AUDBUFSIZE;
		audOutBytes = ((vidPbCtrl.audInIdx + audOutBytes) % AUDBUFSIZE) & ~3;
		//printf("-- 0x%lx, 0x%lx\n", vidPbCtrl.audInIdx, audOutBytes);

		HAL_AudInIndexSet(vidPbCtrl.audInIdx);
		HAL_AudOutIndexSet(vidPbCtrl.audInIdx);
		
		secAddr_min_vid = videoPbReadDword(vidPbCtrl.AVIIdxAddr+GetIdxInBuf(vidPbCtrl.lastVidIdx)*8+4); 
		secAddr_min_aud = videoPbReadDword(vidPbCtrl.AVIIdxAddr+GetIdxInBuf(vidPbCtrl.lastAudIdx)*8+4);
		if(vidPbCtrl.lastVidIdx < vidPbCtrl.lastAudIdx){
			dataOffset = secAddr_min_vid;
		}
		else{
			dataOffset = secAddr_min_aud;
		}
		//printf("++ 0x%lx, 0x%lx\n", secAddr_min_aud, secAddr_min_vid);
	}else{
		secAddr_min_vid = videoPbReadDword(vidPbCtrl.AVIIdxAddr+GetIdxInBuf(vidPbCtrl.lastVidIdx)*8+4); 
		dataOffset = secAddr_min_vid;
	}
	dataOffset = (UINT32)((dataOffset>>9)<<9);
	secAddr_max = 0;
	secAddr_min = dataOffset;
	//secAddr_min_vid =dataOffset; // xian --, last vid or maybe not in the buffer, the flow may hung if vid is out off buffer
	//secAddr_min_aud =dataOffset;
	VlcRingBufInIdx = 0;
	VlcRingBufOutIdx = 0;
	dataOffset += AVI_512_ALIGN(AviHeaderSize);
	videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, /*0*/K_SDRAM_AviPbFragSize<<1); //max size
	//printf("** 0x%lx, 0x%lx\n", secAddr_min, secAddr_max);
	//printf("** 0x%lx, 0x%lx\n", secAddr_min_aud, secAddr_min_vid);
	//printf("** 0x%lx, 0x%lx\n", VlcRingBufOutIdx, VlcRingBufInIdx);
	
	vidPbCtrl.speed = VID_PB_SPEED_NORMAL;
	if(!nonAudioFile){
		if((G_VPBupSmpflag)){		//for upsampling	
			videoFillAudRingBuf(6);
		}
		else {
			videoFillAudRingBuf(24);
		}	
		HAL_AudOutIndexSet(audOutBytes);
	}
	//printf("** 0x%lx, 0x%lx\n", secAddr_min, secAddr_max);
	//printf("** 0x%lx, 0x%lx\n", secAddr_min_aud, secAddr_min_vid);
	//printf("** 0x%lx, 0x%lx\n", VlcRingBufOutIdx, VlcRingBufInIdx);

	
	//set frame pts
	if(vidPbCtrl.VLCFlip){
		vidPbCtrl.dispFlip = vidPbCtrl.VLCFlip - 1;
	}
	else {
		vidPbCtrl.dispFlip = 2;
	}		
	//printf("VLC: %bx, %bx\n",vidPbCtrl.VLCFlip, vidPbCtrl.dispFlip);
	vidPbCtrl.frameTime[vidPbCtrl.dispFlip] = vidPbCtrl.frameTime[vidPbCtrl.VLCFlip] - vidPbCtrl.msPerFrame;
	
	EXIT_CRITICAL( status );

	HAL_GlobalReadGTimer(&gPlayTime);
	gPlayStartTime = gPlayTime - videoPbTimeLengthGet(); //xian ++ 20081211, for no voice
	if(!nonAudioFile){	 //xian ++ 20081211
		HAL_AudResume();
	}

	vid_flag = VD_PB_STS_START; //for hdmi
	XBYTE[REG_Audio_DataThdIntSts] = 0x03;


//	printf("resume\n");
	return;
}

void videoPbFreeBufSpeedup(void)
{
	UINT32 nextIdx, i;
	UINT32 idxAddr;
	UINT32 aviAddr,aviSize;
	UINT8 tmp2, tmp3;

	nextIdx = vidPbCtrl.lastAudIdx + 1;
	
	if(nextIdx < vidPbCtrl.lastVidIdx){// audIdx is far from videoIdx
		if(nextIdx > vidPbCtrl.fstIdxInBuf && vidPbCtrl.lastVidIdx < vidPbCtrl.lstIdxInBuf){// index in index buffer
			idxAddr = vidPbCtrl.AVIIdxAddr;
			while(nextIdx < vidPbCtrl.lastVidIdx){
				i = GetIdxInBuf(nextIdx);			 
				HAL_DramStartAddrSet(idxAddr+i*8, 1);
				HAL_DramWordDataRead(&tmp2,&tmp3);
				
				if(tmp2==0x30 && tmp3==0x30) { //00dc
					aviAddr = videoPbReadDword(idxAddr+i*8+4);
					aviSize = videoPbReadDword(idxAddr+i*8+6);
					aviAddr = (UINT32)16 + aviAddr + aviSize; //end addr
					secAddr_min_aud = aviAddr;
				}
				else{
					break;
				}
				
				nextIdx ++;
			}
		}
	}
}
#endif

#else
void videoJpgWait(void)
{
}
UINT8
HAL_DispSignalWait_Imem(
	UINT8 src,
	UINT8 mode
)
{
	return 0;
	src = src;
	mode = mode;
}
#endif

