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
 * @file	  vid_pbaud.c
 * @brief	  middle ware  API for audio playback in video playback module
 * @author	  tq.zhang
 * @since	  2010-05-11
 * @date	  2010-05-11
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_pb.h" 
#include "initio.h"
#include "hal_dram.h"
#include "hal_audio.h"
#include "disp_api.h"

#if FUNC_KNL_VDPB_LIB_EN
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 #define K_Vidupsmp_4sector (UINT8)0x04	//for upsampling
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 GetIdxInBuf(UINT32 idx);
extern void updateRingBufIdx();
extern void audioUpSample(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT8 sector) ;

extern UINT32 audChunkSize;
extern UINT32 secAddr_max, secAddr_min;
extern UINT32 dataOffset;
extern UINT32 VlcRingBufOutIdx;
extern UINT32 vidPbFragBufSize;
extern UINT32 secAddr_min_aud;
extern UINT8 G_VPBupSmpflag;
extern UINT32 AviHeaderSize;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//videoPbAudioPlay
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoPbAudioPlay(void)
 * @brief		playback audio data until decode done
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	SunYong
 * @since	2007-12-25
 */
void videoPbAudioPlay(void)
{

	UINT8 sts;

	if(vidPbCtrl.speed != VID_PB_SPEED_NORMAL){
		return;
	}

	vidPbCtrl.nextAudIdx  = videoPbSearchAVIIndex(vidPbCtrl.AVIIdxAddr,vidPbCtrl.lastAudIdx , vidPbCtrl.chunckCnt , 1,1);

	if(vidPbCtrl.nextAudIdx ) {//if audio only, 0th chunck == 01wb
		sts = videoPbAudioChunck(vidPbCtrl.AVIIdxAddr,vidPbCtrl.audAddr ,vidPbCtrl.nextAudIdx);
		if(sts == SUCCESS) {
			vidPbCtrl.audChunkCnt ++;
			vidPbCtrl.lastAudIdx = vidPbCtrl.nextAudIdx;
		} else {
			return;
		}
	} else{
		vidPbCtrl.audDown = 1;
		return;
	}

}


/**
 * @fn        UINT8 videoPbAudioChunck(UINT32 AVIIdxAddrWord, UINT32 audioBufferAddr, UINT32 AudioIdx)
 * @brief     video Pb Audio Chunck
 * @param     [in] AVIIdxAddrWord
 * @param     [in] audioBufferAddrWord
 * @param     [in] AudioIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 videoPbAudioChunck(UINT32 AVIIdxAddrWord, UINT32 audioBufferAddrWord, UINT32 AudioIdx) USING_0
{
	UINT32	AviSize;
	UINT32	AviAddr;
	UINT32	AviAddrEnd;
	UINT32	tmp_SecAddr;
	UINT32	tmp_SecAddrEnd;
	UINT32 	remainIdx;
	 AviAddr = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(AudioIdx)*8+4);
	 AviSize = videoPbReadDword(AVIIdxAddrWord+GetIdxInBuf(AudioIdx)*8+6);
	audChunkSize=AviSize;
	//AviAddr = (UINT32)16 + AviAddr;
	AviAddr = (UINT32)16 + AviAddr + AviHeaderSize%512 ;
	AviAddrEnd = AviAddr + AviSize;

	if(AviAddr >= secAddr_min) {
		if(AviAddrEnd > secAddr_max){//data not enough
			videoPbStreamRead(vidPbCtrl.fHandle, dataOffset, 102400);		 
		}
		if(AviAddrEnd <= secAddr_max) {
			tmp_SecAddr = VlcRingBufOutIdx + (AviAddr - secAddr_min);
			tmp_SecAddr = tmp_SecAddr & (vidPbFragBufSize-1);
			tmp_SecAddrEnd = tmp_SecAddr + AviSize;
			if(tmp_SecAddrEnd > vidPbFragBufSize) {
				HAL_GprmDramDma((K_SDRAM_AviPbFragAddr <<1), (K_SDRAM_AviPbFragAddr <<1) + vidPbFragBufSize,  AviSize, 1);
			}
			if((G_VPBupSmpflag)) {	//for upsampling
				audioUpSample((K_SDRAM_AviPbFragAddr)+ (tmp_SecAddr>>1),(audioBufferAddrWord) + (vidPbCtrl.audInIdx>>1),K_Vidupsmp_4sector);
			}
			else { 
				HAL_GprmDramDma((K_SDRAM_AviPbFragAddr <<1)+ tmp_SecAddr, (audioBufferAddrWord<<1) + vidPbCtrl.audInIdx,  AviSize, 1);
			}
			secAddr_min_aud = AviAddrEnd;
		} 
		else {
			return FAIL;
		}
	} else {
		vidPbCtrl.audDown = 1;
		return FAIL;
	}

	updateRingBufIdx();
	if((G_VPBupSmpflag)) {	//for upsampling
		vidPbCtrl.audInIdx = vidPbCtrl.audInIdx + (AviSize<<2);
		if(vidPbCtrl.audInIdx > 0x10000){
			remainIdx = vidPbCtrl.audInIdx-0x10000; 
			vidPbCtrl.audInIdx = vidPbCtrl.audInIdx &( AUDBUFSIZE-1);
			HAL_GprmDramDma((audioBufferAddrWord<<1)+0x10000,(audioBufferAddrWord<<1),remainIdx,1);
		}
		else {
			vidPbCtrl.audInIdx = vidPbCtrl.audInIdx &( AUDBUFSIZE-1);
		}	
	}
	else {
		vidPbCtrl.audInIdx += AviSize;
		vidPbCtrl.audInIdx = vidPbCtrl.audInIdx &( AUDBUFSIZE-1);
	}
	
	HAL_AudInIndexSet(vidPbCtrl.audInIdx);
	return	SUCCESS;
}

#endif
