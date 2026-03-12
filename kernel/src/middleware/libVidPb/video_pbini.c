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
 * @file	  vid_pbini.c
 * @brief	  middle ware  API for video playback initial  module
 * @author	  tq.zhang
 * @since	  2010-05-11
 * @date	  2010-05-11
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_pb.h" 
#include "doslink.h"
#include "dcf_api.h"
#include "pb_general.h"
#include "dbg_def.h"
#include "awb_api.h"
#include "pb_api.h"
#include "vfs_api.h"
#include "dos32.h"
#include "initio.h"
#include "hal_gprm.h"
#include "hal_disp.h"
#include "disp_api.h"
#include "video_debug.h"
#include "reg_def.h"

#if FUNC_KNL_VDPB_LIB_EN
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define __FILE	  __FILE_ID_VIDEO_PBINIT__

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void videoPbSizeMatchGet(vdPbSizeMatchTable_t *pVal);

extern UINT32 VlcRingBufOutIdx;
extern UINT8 slowMode;
extern xdata UINT8 fastFlg;
extern UINT8 gJpgDecFact;
extern xdata UINT16 G_AVIWidth;
extern xdata UINT16 G_AVIHeight;
extern UINT32 gJpgDecAddrOff;
extern xdata	UINT8     G_DOS_UseLinearFAT;
extern UINT32 vidPbFragBufSize;
extern UINT32 dataOffset;
extern UINT32 bytePerSec;
extern xdata UINT16   G_DOS_FileCurrCluster;
extern xdata UINT16   G_DOS_FileClusterIndex;
extern xdata UINT8 vid_flag;
extern xdata UINT32 recTime;
extern xdata UINT32 secTab[4];
extern xdata UINT8 fastFlg;
extern xdata UINT32 VidPb_GT_zero;
extern UINT32 secAddr_max;
extern UINT32 secAddr_min;
extern UINT32 secAddr_min_vid;
extern UINT32 secAddr_min_aud;
extern UINT32 AviHeaderSize;
extern UINT16 VidHeaderSize;
extern UINT32 VlcRingBufInIdx;

//xian remove 20100722
//extern UINT8 gSilence; //xian ++ 20081211

extern UINT32 gVdJpgCnt;
extern UINT32 gVdDispCnt;

extern UINT32 gAviFileSize;	
extern UINT8 nonAudioFile;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 void videoPbDispInit(void);
 void videoPbJpgDecParamSet(void);

 /**
  * @fn 	   void videoPbJpgDecParamSet(void)
  * @brief	   set playback JPEG decode parameters
  * @param	   NONE
  * @retval    NONE
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-11
  * @todo	   N/A
  * @bug	   N/A
 */
 void videoPbJpgDecParamSet(void)
 {
	 UINT16 pvW, pvH;
	 UINT16 decW, decH;
	 vdPbSizeMatchTable_t val;
 
	 videoPbSizeMatchGet(&val);
	 pvW = val.disW;
	 pvH = val.disH;
	 gJpgDecFact = val.fact;
	 if(gJpgDecFact){
		 decW = G_AVIWidth * gJpgDecFact >> 3;
		 decH = G_AVIHeight * gJpgDecFact >> 3;
		 //printf("sz: %u, %u, %u, %u\n", pvW, pvH, decW, decH);
		 gJpgDecAddrOff = (UINT32)decW*((UINT32)(pvH-decH)>>1) + ((UINT32)(pvW-decW)>>1);	
		 //printf("%bu, 0x%lx\n", gJpgDecFact, gJpgDecAddrOff);
	  }
	 else{
		 gJpgDecAddrOff = 0;
	 }
 
 }

 //-----------------------------------------------------------------------------
 //videoPbInit
 //-----------------------------------------------------------------------------
 /**
  * @fn 	   UINT8 videoPbInit(UINT16 ID)
  * @brief		 playback video inital
  * @param	 ID  :	 AVI file index
  * @retval  0 is SUCCESS, other is fail
  * @todo
  * @author  SunYong
  * @since	 2007-12-25
  */
  UINT8 videoPbInit(UINT16 ID) USING_0
 {
 
	 UINT8 readMode = K_DOS_ReadFile_LinearFAT;
	 UINT8 sts;
	 UINT32 k= 5000;
	 FDBInfo_t fdbInfo;
	 PBFile_t fileInfo;
 
	 //aeModeEnableSet(0);
	 //AAA_AWBModeEnable(0);//wangjg clear awb old function
	 sp1kAwbDisable();//wangjg++
	 //xian ++ 20080919,fix bug 32021
	 if ((pbFileInfoGet(ID, &fileInfo) != SUCCESS)) {
		 return FAIL;
	 }
	 for(sts = 0; sts < 12; sts ++){
		 vidPbCtrl.name[sts] = fileInfo.FileName[sts];
	 }
 
	 vidPbCtrl.fHandle = vfsFileOpen(vidPbCtrl.name , readMode, 0);
 
	 if (!vidPbCtrl.fHandle){
		 ASSERT(vidPbCtrl.fHandle!=0, 1);
		 vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, readMode, 0);//speed up avi playback
		 return  FAIL;
	 }
	 //Add for Video Play rewind eric@20081015
	 DOS_FileReadMakeFatEnable(1, vidPbCtrl.fHandle);

 	 gAviFileSize = vfsFileSizeGet(vidPbCtrl.fHandle);
	 //Fix the zero file size problem begin
	 if (gAviFileSize == 0)
	 {
		 // must call close when there is handle and must with same read mode.
		 DOS_FileReadMakeFatEnable(0, vidPbCtrl.fHandle);
		 vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, readMode, 0);//speed up avi playback
		 ASSERT(vfsFileSizeGet(vidPbCtrl.fHandle) !=0, 1);
		 return  FAIL;
	 }
	 //Fix the zero file size problem end
 
	 sts = vfsFileRead(vidPbCtrl.fHandle, K_DOS_DstDram, 1024, vidPbCtrl.pbAudioChunkAddr, 0);
	 if(!sts)
	 {
		 DOS_FileReadMakeFatEnable(0, vidPbCtrl.fHandle);
		 vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, readMode, 0);//speed up avi playback
		 ASSERT(0, 1);
		 return  FAIL;
	 }
 
	 vidPbCtrl.fHandle_idx= vfsFileOpen(vidPbCtrl.name , readMode, 0);
	 if (!vidPbCtrl.fHandle_idx)
	 {
		 DOS_FileReadMakeFatEnable(0, vidPbCtrl.fHandle);
		 vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, readMode, 0);
		 ASSERT(0, 1);
		 return  FAIL;
	 }
	 sts = AVIHeaderParse();
	 if(sts) {
		 DOS_FileReadMakeFatEnable(0, vidPbCtrl.fHandle);	//xian ++ 20110531 #42714
		 vfsFileClose(vidPbCtrl.fHandle, vidPbCtrl.name, readMode, 0);//speed up avi playback
		 vfsFileClose(vidPbCtrl.fHandle_idx, vidPbCtrl.name, readMode, 0);
		 ASSERT(0, 1);
		 return FAIL;
	 }
	 videoPbJpgDecParamSet(); //xian ++ 20090915
	 videoPbDispInit();
 
	 // only avi with audio require to build LinearFAT
#if	(K_DOS_HaveLinearFAT)
#if 1//( ( PLAYAUDIO_OPTION	== 1 ) && (K_Link_AllFreeCluster ==	1))
	 /* modify by phil for remove used variables @08.09.18 */
	 {
		 G_DOS_UseLinearFAT  = 1;
		 //DOS_File_Build_LinearFAT(G_DOS_FileStartCluster); //xian -- null function
	 }
#endif
#endif
 
	 /* power control -- video recording */
	 //appModePowerCtrl(MODE_VIDEOPB);
	 XBYTE[REG_Audio_AudBufUnf] &= 0x01;
	 while((XBYTE[REG_Audio_AudEn]&0x02) ==0x02){
		 if(!(k--)) {
			 XBYTE[REG_Audio_DataThdIntSts] |= 0x02;
			 XBYTE[REG_Audio_DataThdIntEn] &=  (~0x02);
			 XBYTE[REG_Audio_AudEn] &=  (~0x02);
			 break;
		 }
 
	 }
	 vidPbCtrl.showBG = 1;
 
	 XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x20;
	 XBYTE[REG_Jpeg_Intr_Event]=0xFF;
	 dcfFileDramFdbParameterRead(ID, &fdbInfo);//mantis 27088
	 while(fdbInfo.FileSize < vidPbFragBufSize) {
		 vidPbFragBufSize = vidPbFragBufSize/2;
	 }
#if 1//VD_PB_HDMI_EN
	 videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, 0);
#else
	 videoPbStreamRead(vidPbCtrl.fHandle, dataOffset);
#endif
	 if(vidPbCtrl.audFmt == VID_PCMFMT) {
		 bytePerSec = vidPbCtrl.audFrq* vidPbCtrl.audBit/8;
	 } else {
		 bytePerSec = vidPbCtrl.audFrq * vidPbCtrl.audAlign /((vidPbCtrl.audAlign -4)*2 +1);
	 }
 
	 return SUCCESS;
 }

 /**
  * @fn 	   void videoPbDispInit(void)
  * @brief	   video Playback Display Initial
  * @param	   NONE
  * @retval    NONE
  * @see	   NULL
  * @author    Tq.Zhang
  * @since	   2010-5-11
  * @todo	   N/A
  * @bug	   N/A
 */
void videoPbDispInit()
{
	UINT32 addr;
	UINT16 pbBufWidth, pbBufHeight;
	UINT16 pbRoiHSize, pbRoiVSize, pbRoiHOff, pbRoiVOff;
	UINT32 dstAddr, tmpAddr;
	UINT16 pvW, pvH;

	addr = pbDispBuffGet(1);
	pbDecodeSizeGet(&pbBufWidth, &pbBufHeight);	//pb buffer size
	pbDispRoiSizeGet(&pbRoiHOff, &pbRoiVOff, &pbRoiHSize, &pbRoiVSize); // pb roi
	videoPbSizeGet(&pvW, &pvH);	// video paly buffer size,

	#if 0
	printf("vd disp init %lx\n", addr);
	printf("%u, %u\n", pbBufWidth, pbBufHeight);
	printf("%u, %u\n", pbRoiHOff, pbRoiVOff);
	printf("%u, %u\n", pbRoiHSize, pbRoiVSize);
	printf("%u, %u\n", pvW, pvH);
	#endif

	dstAddr = K_SDRAM_AviPbBufAddrC;
	if(pvW == pbRoiHSize && pvH == pbRoiVSize){
		HAL_GprmCopy(addr, pbBufWidth, pbBufHeight, pbRoiHOff, pbRoiVOff, dstAddr, pvW, pvH, 0, 0, pbRoiHSize, pbRoiVSize,0, 1);		
	} 
	else{//video buff size = dec size 
		if(pbRoiHOff == 0){
			addr = addr + (UINT32)pbBufWidth * pbRoiVOff;
			tmpAddr = dstAddr + (UINT32)pvW * pvH;
			HAL_GprmScale(addr, pbRoiHSize, pbRoiVSize, dstAddr, pvW, pvH, tmpAddr, 1);				
		}
		else{
			tmpAddr = K_SDRAM_AviPbBufAddrB;
			HAL_GprmCopy(addr, pbBufWidth, pbBufHeight, pbRoiHOff, pbRoiVOff, tmpAddr, pvW, pvH, 0, 0, pbRoiHSize, pbRoiVSize,0, 1);		
			
			tmpAddr = dstAddr + (UINT32)pvW * pvH;
			HAL_GprmScale(tmpAddr, pbRoiHSize, pbRoiVSize, dstAddr, pvW, pvH, tmpAddr, 1);				
		}		
	}

	HAL_DispPbFrameBufferSet(dstAddr, pvW, pvH);	
	HAL_DispScaleSet(pvW, pvH, 100 ,0);
	dispFitShow(pvW, pvH, 1);
		  
	if(gJpgDecAddrOff){
		gprmBlackFill(K_SDRAM_AviPbBufAddrA, pvW, pvH);
		gprmBlackFill(K_SDRAM_AviPbBufAddrB, pvW, pvH);
	}

	vidPbCtrl.VLCFlip = 0;
	vidPbCtrl.dispFlip = 2;
}

 //-----------------------------------------------------------------------------
 //videoPbGlobalInit
 //-----------------------------------------------------------------------------
 /**
  * @fn 	   void videoPbGlobalInit(void)
  * @brief		 video playback global variable inital
  * @param	 NONE
  * @retval  NONE
  * @todo
  * @author  SunYong
  * @since	 2007-12-25
  */
 void videoPbGlobalInit(void) USING_0
 {
	 vidPbCtrl.audInIdx = 0;
	 vidPbCtrl.speed = VID_PB_SPEED_NORMAL;
	 vidPbCtrl.showBG = 0;
	 //patch4.4@yichang@for valid signal end
	 vidPbCtrl.Idle = VID_PB_IDLE;
	 vidPbCtrl.TimeExceed = 0;
 
	 vidPbCtrl.playAudFlg  = 0;
	 vidPbCtrl.audAddr = K_SDRAM_AviPbAudioBufAddr;
	 vidPbCtrl.pbAudioChunkAddr = K_SDRAM_AviPbAudioChunkBufAddr;
	 vidPbCtrl.AVIIdxRamAddr = K_SDRAM_AviPbIdx1BufAddr;
 
//printf("---vdAddr: 0x%lx, 0x%lx, 0x%lx\n", K_SDRAM_AviPbFragAddr, vidPbCtrl.audAddr, K_SDRAM_AviPbIdx1BufAddr);
 
//	 vidPbCtrl.lastVidCluster  = G_DOS_FileCurrCluster;
//	 vidPbCtrl.lastVidFileIdx = G_DOS_FileClusterIndex;
//	 vidPbCtrl.lastAudCluster  = G_DOS_FileCurrCluster;
//	 vidPbCtrl.lastAudFileIdx = G_DOS_FileClusterIndex;

	 vidPbCtrl.feedData = 0;
	 vidPbCtrl.lastVidIdx = 0;
	 vidPbCtrl.lastAudIdx = 0;
	 vidPbCtrl.NextVidIdx = 1;
	 vidPbCtrl.nextAudIdx = 1;
	 vidPbCtrl.playVidTime	 = 0;
	 vidPbCtrl.playAudTime = 0;
	 vidPbCtrl.vidDown = 0;
	 vidPbCtrl.audDown = 0;
	 vidPbCtrl.dropVidFlg = 0;
//	 vidPbCtrl.VLCFlip = 1;  //AVI_DEBUG dlchiou - //2002-12-11 10:56
	 vidPbCtrl.firstVidBlock  = 0;
	 vidPbCtrl.vidChunkCnt = 0;
	 vidPbCtrl.audChunkCnt = 0;
	 vidPbCtrl.showBG = 1;
	 recTime = 0;
	 fastFlg =0;
	 vid_flag = 0;
	 secTab[0] =0;
	 secTab[1] =0;
	 secTab[2] =0;
	 secTab[3] =0;
	 VidPb_GT_zero= 0;
 
	 secAddr_max =0;
	 secAddr_min =0;
 
	 secAddr_min_vid =0;
	 secAddr_min_aud =0;
 
	 AviHeaderSize = 512;
	 VidHeaderSize = 252+HUFFMANSIZE;//420 huffman;
	 if((K_SDRAM_AviPbFragSize<<1) > ((UINT32)1024<<12)) //xian ++ 20110725
		 vidPbFragBufSize = ((UINT32)1024<<12);  // 4M	
	 else if((K_SDRAM_AviPbFragSize<<1) > ((UINT32)1024<<11))
		 vidPbFragBufSize = ((UINT32)1024<<11);  // 2M	
	 else
		 vidPbFragBufSize = ((UINT32)1024<<10);  // 1M			
 
	 dataOffset=0;
	 VlcRingBufInIdx = 0;
	 VlcRingBufOutIdx = 0;
	 vidPbCtrl.dummyFrame = 0;
 
	 vidPbCtrl.fHandle_idx = 0;
	 vidPbCtrl.fstIdxInBuf = 0;
	 vidPbCtrl.lstIdxInBuf = 0;
	 vidPbCtrl.idxReadOff = 0;
	 slowMode = 0;	 //xian ++ 20080919, for slow motion
 
//xian remove 20100722
	//gSilence = 0;	 //xian ++ 20081211
	//tlsFile = 0; 
	nonAudioFile =0;
	vidPbCtrl.VLCFlip = 0;
	vidPbCtrl.dispFlip = 2;
	vidPbCtrl.frameTime[0]=0;
	vidPbCtrl.frameTime[1]=0;
	vidPbCtrl.frameTime[2]=0;
	vidPbCtrl.frameDummyCnt[0] = 0;
	vidPbCtrl.frameDummyCnt[1] = 0;
	vidPbCtrl.frameDummyCnt[2] = 0;
	
	gVdJpgCnt = 0;
	gVdDispCnt = 0;

	#if 1
	vdDebugInit();
	#endif
	
	return;
}

#endif
