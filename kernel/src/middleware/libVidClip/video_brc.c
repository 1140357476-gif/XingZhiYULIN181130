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
 * @file		video_brc.c
 * @brief		middle ware  API for video bit rate control module
 * @author		tq.zhang
 * @since		2010-05-28
 * @date		2010-05-28
 */

 /**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_def.h"
#include "quantdef.h"

//#define _ABS(x,y) (((x)>(y))?((x)-(y)):((y)-(x)))
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#pragma OT(8,SPEED) // optimize with speed first 
#define __FILE	  __FILE_ID_VIDEO_BRC__ 
#if 0
//#define VIDEO_QTAB_VALUES {K_Q50, 55, 60, 65, K_Q70, 75, K_Q80, 85, K_Q90, 100}
//#define VIDEO_QTAB_VALUES {30, 35, 40, 45, 50, 55, 60, 65, 70, 75}
//#define VIDEO_QTAB_VALUES {31, 32, 33, 34, 35, 36, 37, 38, 39, 40}
#define VIDEO_QTAB_VALUES {31, 33, 35, 38, 40, 42, 44, 46, 48, 50}

#define QSearchOrder 0
#define QStepNum 12
#define QStepValM {2048, 1536, 1331, 1229, 1180, 1126, 1096, 1075, 1056, 1040, 1034, 1029}
#define QStepValL {103, 512, 717, 819, 868, 922, 952, 973, 992, 1008, 1014, 1019}
#define QStepDef {120, 110, 100, 80, 60, 40, 32, 24, 18, 10, 4, 1}
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 g_bitRate = 0;//((UINT32)360*(UINT32)8000);
UINT32 g_brcTargetSize=110ul*1024;
UINT8 g_brcQValue;
UINT8 g_jpgQValue;
UINT8 g_brcQIndex=0;
UINT8 g_jpgQIndex=0;
UINT8 *g_brcQTabBuf;//[130] _at_ 0x4000;
//UINT8 g_brcVlcQValueBuf[VLCBUFNUM_TOTAL+1];// = {0};
UINT8 g_brcVlcQIndexBuf[VID_VLCBUFNUM_TOTAL+1];// = {0};
UINT8 g_brcVlcQIndexLst[VID_VLCBUFNUM_TOTAL+1];// = {0};

#define K_SDRAM_AviCapQTabNum_ 20
UINT8 videoQValueTab[K_SDRAM_AviCapQTabNum_] 
	//= {5,14, 20, 24, 28, 32, 36, 38, 42, 46
	//, 48, 52,56, 60, 64, 68, 72, 76, 80, 84};
	= {15,23, 26, 29, 32, 35, 38, 41, 43, 46
	, 49, 52,55, 57, 60, 64, 68, 72, 78, 84};
	//={5, 20, 24, 28, 32, 36, 38, 42, 46, 48, 
	//52,56, 60, 64, 68, 72, 76, 80, 84, 88};

UINT8 brcOvfCnt=0;
UINT8 *brcQTbls;// = G_FPGAMemPool+8*1024;
UINT8 brcPreQValue;
UINT8 brcQValueIdx;
UINT8 frameNum;
//UINT32 brcQTblAddr;
//#define K_SDRAM_AviQTabAddr_ (K_SDRAM_CodeSize+(((UINT16)brcQTbls-0x4000)>>1))
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//static void videoBrcQTabGen(UINT32 dramAddr);
//static UINT8 videoBrcQTabFillVlcBuf(UINT8 vlcBufIdx);
//static void videoMjpgBitRateControl(UINT32 frameSize);
//void INTR_halJpegQTableWrite(UINT8 QTableIndex,UINT8 *yQTable,UINT8 *uvQTable,UINT8 zigzagOpt);
//void INTR_videoClipBrcVlcOvfSetQtable(void);

/**
 * @fn		  UINT8 videoBrcQTabFillVlcBuf(UINT8 vlcBufIdx)
 * @brief	  set Q table to vlc buffer
 * @param	  [in] vlcBufIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoBrcQTabFillVlcBuf(UINT8 vlcBufIdx)
{
	UINT32 dramAddr;	
	UINT8 *pidx = g_brcVlcQIndexBuf+vlcBufIdx;
	UINT8 *plstIdx = g_brcVlcQIndexLst+vlcBufIdx;
	if(*pidx==*plstIdx){return SUCCESS;}
	//UINT32 qTabAddr = K_SDRAM_AviQTabAddr_ + idx*65;
	*plstIdx = *pidx;
	dramAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	dramAddr += vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A) * vlcBufIdx;
	dramAddr += 16;
	//HAL_GprmDramDmaAdv(qTabAddr<<1, dramAddr<<1, 130, 1);
	//HAL_GprmDramDmaExt(qTabAddr<<1, dramAddr<<1, 130, DmaWaitOpt,1);
	//HAL_DramDmaWrite(g_brcQTabBuf, dramAddr, 130, DmaWaitOpt);
	HAL_DramDmaWrite(brcQTbls+(UINT16)*pidx*130, dramAddr, 130, 1);
	return SUCCESS;
}

/**
 * @fn		  UINT32 videoMjpgBrcQValueGet(UINT32 tagetSize, UINT32 preQValue, UINT32 preFrameSize)
 * @brief	  get Q index
 * @param	  [in] tagetSize : target VLC size
 * @param	  [in] preQValue : previous Q index
 * @param	  [in] preFrameSize : previous VLC size
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/

/**
 * @fn		  void videoMjpgBitRateControl(UINT32 frameSize)
 * @brief	  Bit Rate Control Main function
 * @param	  [in] frameSize
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/


//UINT8 brcOvQIndex=0;
void videoMjpgBitRateControl(UINT32 frameSize)// 2
{
	UINT32 tagetSize = g_brcTargetSize;
	UINT8 qval = g_jpgQValue;
	UINT8 idx = g_jpgQIndex;
	UINT32 dis = _ABS(frameSize,tagetSize);
	UINT16 k = (dis<<8) / tagetSize;
	//UINT8 k = k0;//>255?255:k0;
	UINT8 i,s;// = (UINT16)k*k/400;
	//UINT8 ovIdx = brcOvQIndex;
	if(frameSize>tagetSize){k<<=1;}
	if(k>=192){s=20;}
	else if(k<=32){s=0;}//k/256*120
	else {s=(k-32)>>3;}
//printf(",%bu,%bu",i,qval);	
	if(idx==0){s<<=1;}
	if(s){
		if(frameSize>tagetSize){
			qval = (qval>s)?(qval-s):0;
			//qval-=i;//D12[i]/10;//(qval*10+D12[i])/10;//D12[i];
			for(i=idx;i>0;i--){
				if(qval>=videoQValueTab[i]){break;}
			}
			if(i!=idx)i++;
		}else{
			qval+=s;//D12[i]/10;//i;//D12[i];
			for(i=idx;i<sizeof(videoQValueTab);i++){
				if(qval<=videoQValueTab[i]){break;}
			}	
			if(i!=idx)i--;
		}		
		if(_ABS(qval,brcPreQValue)>5 || qval>5){
			
			brcPreQValue = qval;
			g_brcQIndex = i;
			g_brcQValue = videoQValueTab[i];
			
			//if(g_jpgQValue!=g_brcQValue){
			//	g_brcQTabBuf = brcQTbls+(UINT16)130*g_brcQIndex;
				//UINT32 qTblAddr = K_SDRAM_AviQTabAddr_+i+((UINT32)i<<6);//65*idx;		
				//HAL_DramReadBytesAddr(qTblAddr,g_brcQTabBuf,130,0);
				//UINT32 addr = K_SDRAM_AviQTabAddr_+g_brcQIndex+((UINT32)g_brcQIndex<<6);//65*idx;	;
				//HAL_GprmDramDmaExt(addr<<1,K_SDRAM_CodeSize<<1,130,0,1);
			//}

			/*brcOvQIndex = g_brcQIndex>5?g_brcQIndex-5:0;
			if(ovIdx!=brcOvQIndex){
				UINT32 qTblAddr = K_SDRAM_AviQTabAddr_+brcOvQIndex+((UINT32)brcOvQIndex<<6);//65*idx;		
				HAL_DramReadBytesAddr(qTblAddr,QTblVlcOv,130,0);
			}*/
		}
	}
	/*printf("--(%bu,%bu)(%bu,%bu)(%bu,%bu,%bu,%bu,%bu,%bu)(%lu,%lu)(%bu,%bu,%bu,%bu,%bu)%lu\n",
		g_jpgQValue,g_brcQValue,g_brcQIndex,brcOvQIndex
		,k,i,s,idx,qval,brcPreQValue
		,tagetSize,frameSize
		,videoQValueTab[g_brcQIndex]
		,videoQValueTab[19],videoQValueTab[18]
		,videoQValueTab[17],videoQValueTab[16]
		,(UINT32)sizeof(videoQValueTab));
	*/	
}

/**
 * @fn        void videoBrcQTabUpdate(UINT8 vlcBufIdx, UINT32 frameSize)
 * @brief     videoBrcQTabUpdate
 * @param     [in] vlcBufIdx : set Q table to the Vlc Buffer
 * @param     [in] frameSize : current frame vlc size
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-28
 * @todo      N/A
 * @bug       N/A
*/
//extern void HAL_GlobalTimerRead10us(UINT32 xdata * G10ustimerPt);
void
videoBrcQTabUpdate(UINT8 vlcBufIdx, UINT32 frameSize)
{
	if(g_brcTargetSize){
		//HAL_CpuImemSdSet(LOG_BANK_12, 0xa000);
		videoBrcQTabFillVlcBuf(vlcBufIdx);
		if(vidClipCtrl.recIdle != VID_REC_PAUSE) {//{return SUCCESS;}
			videoMjpgBitRateControl(frameSize);
		}
	}
}

/**
 * @fn        void INTR_VidClipBrcSetQtable(UINT8 status)
 * @brief     INTR_VidClipBrcSetQtable
 * @param     [in] status : '0' normal, '1' buffer busy, 'error' start buffer index
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-28
 * @todo      N/A
 * @bug       N/A
*/
#if 0
void
INTR_VidClipBrcSetQtable(UINT8 status)
{
	if(g_brcTargetSize){
		#if VIDREC_JPEG_EOF_EN
		g_brcVlcQIndexBuf[vlcBufNum] = g_jpgQIndex;
		#else
		g_brcVlcQIndexBuf[vlcBufNum] = g_brcQIndex;
		#endif
		if(g_brcQValue != g_jpgQValue){
			g_brcQTabBuf = brcQTbls+(UINT16)g_brcQIndex*130;//=*128+*2;
			INTR_halJpegQTableWrite(g_brcQValue, g_brcQTabBuf+1, g_brcQTabBuf+66, 1);
			g_jpgQValue = g_brcQValue;
			g_jpgQIndex = g_brcQIndex;
		}		
	}
	brcOvfCnt = 0;
	status = 0;
}
#else
void
INTR_VidClipBrcSetQtable(UINT8 status)
{
	if(g_brcTargetSize){
		if(g_brcQValue != g_jpgQValue){
			g_brcQTabBuf = brcQTbls+(UINT16)g_brcQIndex*130;//=*128+*2;
			INTR_halJpegQTableWrite(g_brcQValue, g_brcQTabBuf+1, g_brcQTabBuf+66, 1);
			g_jpgQValue = g_brcQValue;
			g_jpgQIndex = g_brcQIndex;
			//g_brcVlcQIndexBuf[status] = g_brcQIndex;
		}	
	}
	brcOvfCnt = 0;
	#if !VIDREC_JPEG_EOF_EN
	g_brcVlcQIndexBuf[vlcBufNum] = g_brcQIndex;
	#else
	g_brcVlcQIndexBuf[status] = g_jpgQIndex;
	#endif
	//status = 0;
}
#endif
void
  INTR_videoClipBrcVlcOvfSetQtable(void)
{
	UINT8 ii=g_jpgQValue;
	UINT8 cdspIdx, jpgIdx; //clear warning
	vlcOverflowCnt++;
	//while(XBYTE[REG_Dram_VlcRdy]!=0){if (ii++>3000) break;};
	//while((XBYTE[REG_Jpeg_Intr_Event] & 0x02)==0){/*if (ii++>3000) break*/;};
	//XBYTE[REG_Jpeg_Intr_Event] = 0x01;//
	//XBYTE[REG_Jpeg_Intr_Event] = 0x08; // clear Overflow interupt
	XBYTE[REG_Jpeg_Intr_Event] = 0x0A;// clear Overflow & MJPG eof  interupt
	//if(brcOvfCnt++<3){
		g_brcQIndex = g_brcQIndex>5?g_brcQIndex-5:0;
	/*}else{
		g_brcQIndex = 0;
		brcOvfCnt = 2;
	}*/
	g_jpgQIndex = g_brcQIndex; 
	g_jpgQValue = g_brcQValue = videoQValueTab[g_brcQIndex];
	g_brcQTabBuf = brcQTbls+(UINT16)130*g_jpgQIndex;
	brcPreQValue = 0;
	if(ii!=g_jpgQValue){
		INTR_halJpegQTableWrite(g_jpgQValue, g_brcQTabBuf+1, g_brcQTabBuf+66, 1);
		//INTR_halJpegQTableWrite(5, brcQTbls+1, brcQTbls+66, 1);
		#if !VIDREC_JPEG_EOF_EN
		g_brcVlcQIndexBuf[vlcBufNum]= g_jpgQIndex;	
		#else
		g_brcVlcQIndexBuf[brcQValueIdx]= g_jpgQIndex;	
		{
			if(frameNum==1){
				g_brcVlcQIndexBuf[idxB]= g_jpgQIndex;
			}
		}
		#endif
		//g_brcVlcQIndexBuf[idxA] = g_brcVlcQIndexBuf[idxB] = g_jpgQIndex;	
	}	
#if 1 //xian ++ 20110915
	cdspIdx = XBYTE[REG_Dram_MpgSb_CdspWRegIdx];
	jpgIdx = (XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] + 1) & 0xF3;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x0A;
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = cdspIdx;
	XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] = jpgIdx;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x0A;
#endif	
	XBYTE[REG_Jpeg_Swcontrol] = 0x01; // SF trigger sof
	#if 0
	intrPutChar('^');
	intrPutUint8(ii);	
	intrPutUint8(g_jpgQValue);
	intrPutChar('+');
	intrPutUint8(XBYTE[REG_Jpeg_VLCBound_Byte2]);
	intrPutUint8(XBYTE[REG_Jpeg_VLCBound_Byte1]);
	intrPutUint8(XBYTE[REG_Jpeg_VLCBound_Byte0]);
	#endif
	//intrPutUint8(ii);
	//intrPutUint8(g_jpgQValue);
	//intrPutUint8(brcOvfCnt);
	//intrPutUint8(XBYTE[REG_Dram_VlcRdy]);
	//intrPutChar('\n');
//xian -- 20110915	XBYTE[REG_Jpeg_Swcontrol] = 0x01; // SF trigger sof
	//INTR_halJpegQTableWrite(g_jpgQValue, g_brcQTabBuf+1, g_brcQTabBuf+66, 1);
#if 0
#if (VIDEO_FRAME_DEBUG_LEVEL>0)
	vlcOverflowCnt++;
#endif
	#if VIDEO_REC_BRC_OV_RECOMP
	if (g_brcQValue == K_Q_OV){
		XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x08;
		XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] = (XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]&0xFC) | ((XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] + 1)&0x03); 
		XBYTE[REG_Dram_MpgSb_RegIdx_En] &= 0xF7;
	}else{
		g_brcQValue = K_Q_OV;
	}
	
	if (vlcBufNum != G_VideoVlcBufNum){
		g_brcVlcQValueBuf[vlcBufNum] = g_brcQValue;
	}else{
		g_brcVlcQValueBuf[G_VideoVlcBufNum] = g_brcQValue;
	}
	if (g_brcQValue != K_Q_OV){
		INTR_halJpegQTableWrite(g_brcQValue, QTblVlcOv, QTblVlcOv+64, 1);
	}
	#else
		//XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x08;
		//XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] = (XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]&0xFC) | ((XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] + 1)&0x03); 
		//XBYTE[REG_Dram_MpgSb_RegIdx_En] &= 0xF7;
		if (g_brcQValue != videoQValueTab[0]){
			INTR_halJpegQTableWrite(videoQValueTab[0], QTblVlcOv, QTblVlcOv+64, 1);
		}

		g_brcQValue = videoQValueTab[0];
		g_jpgQValue = g_brcQValue;
		g_brcQIndex = 0;
		if (vlcBufNum != G_VideoVlcBufNum){
			g_brcVlcQValueBuf[vlcBufNum] = g_brcQValue;
		}else{
			g_brcVlcQValueBuf[G_VideoVlcBufNum] = g_brcQValue;
		}

	#endif
	XBYTE[REG_Jpeg_Swcontrol] = 0x01; // SF trigger sof
#endif
}

/**
 * @fn		  void videoRecBitRateSet(UINT32 bitRate)
 * @brief	  set bit rate
 * @param	  [in] bitRate
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecBitRateSet(UINT32 bitRate)
{
	g_bitRate = bitRate;	
	if(vidClipCtrl.frameRate){
		vidBrcTagSize = g_brcTargetSize = (g_bitRate>>3)  / vidClipCtrl.frameRate;
	}
}

UINT32 videoRecBitRateGet()
{
	return g_bitRate;
}
/**
 * @fn        void videoBrcUpdateQValBuf(void)
 * @brief     videoBrcUpdateQValBuf
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-28
 * @todo      N/A
 * @bug       N/A
*/
void
videoBrcUpdateQValBuf()
{
	UINT8 i;
	//videoBrcQTabGen(0);
	videoBrcQTabGen(0);//K_SDRAM_AviQTabAddr_);
	//g_brcQValue = G_Image_QTableIndex; //index < 100, xian ++ 20090107
	for(i=0;i<K_SDRAM_AviCapQTabNum_;i++)
	{
		//printf("%bu\n",videoQValueTab[i]);
		if(videoQValueTab[i]>=G_Image_QTableIndex){
			break;
		}
	}
	g_jpgQIndex = g_brcQIndex = i;	
	g_jpgQValue = g_brcQValue = videoQValueTab[g_brcQIndex];
	g_brcQTabBuf = brcQTbls+(UINT16)g_brcQIndex *130;
	for(i = 0; i < G_VideoVlcBufNum + 1; i ++){
		g_brcVlcQIndexBuf[i] = g_brcQIndex;
		//g_brcVlcQValueBuf[i] = g_brcQValue;
		g_brcVlcQIndexLst[i] = g_brcQIndex;
	}
	//printf("Q:%bu,%bu\n",g_jpgQIndex,g_jpgQValue);
	HAL_JpegQTableWrite( g_jpgQValue, g_brcQTabBuf+1, g_brcQTabBuf+66, 1 );
	//printf("qidx:%bu,%bu,%bu\n", G_Image_QTableIndex,g_brcQIndex,g_brcQValue);

	brcOvfCnt = 0;
	brcPreQValue=0;
}

/*void
videoBrcTgSizeSet(UINT32 bPerSec, UINT8 fps)
{

	g_brcTargetSize = bPerSec + ((bPerSec>>SECPERCHK + fps)<<5); //audio size per second,bPerSec + ((bPerSec/CHUNKSIZE + fps)*16*2)
	if((g_bitRate>>3) > g_brcTargetSize){
		g_brcTargetSize = ((g_bitRate>>3) - g_brcTargetSize) / fps;
	}
	else{
	   g_brcTargetSize = 0;
	}
	if((g_brcTargetSize>>1) > (vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A)) ||
	   g_brcTargetSize < 1024){//no brc
	   g_brcTargetSize = 0;
	   g_bitRate = 0;
	}
	
}*/

/**
 * @fn		  void videoBrcQTabGen(UINT32 dramAddr)
 * @brief	  Q table generate
 * @param	  [in] dramAddrWord : destination DRAM address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
static void videoBrcQTabGen(UINT32 dramAddrWord)
{
	SINT32 quality;
	UINT8 i;
	UINT8* pt = brcQTbls;
//	UINT8 qval = 12;
#if 0
	XBYTE[0x3800] = 0x00;
	XBYTE[0x3841] = 0x01;

	for(i = 0; i < K_SDRAM_AviCapQTabNum_; i++){//95, 90, 85 ...
		quality = videoQValueTab[i];
		jpegYQTableGen(quality, G_ucStorData + 1);
		jpegUVQTableGen(quality, G_ucStorData + 66);

		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(dramAddrWord);
		HAL_DmaDo(1, 0, 130, 0, 0);

		dramAddrWord += 65;
	}
#endif
	for(i = 0; i < K_SDRAM_AviCapQTabNum_; i++){//95, 90, 85 ...
		quality = videoQValueTab[i];// = qval;
		pt[0x00] = 0x00;
		pt[0x41] = 0x01;
		jpegYQTableGen(quality, pt + 1);
		jpegUVQTableGen(quality, pt + 66);
		pt+=130;	
		//qval+=4;
	}
	dramAddrWord = dramAddrWord;
}


/**
 * @fn		  void videoQTabUpdate(UINT8 qval)
 * @brief	  update Q table
 * @param	  [in] qval
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoQTabUpdate(UINT8 qval[K_SDRAM_AviCapQTabNum_])
{
	UINT8* pt;
	pt = qval;
#if 0
	UINT8 i;
	if (	qval[K_SDRAM_AviCapQTabNum_-1]<qval[0]
		|| qval[K_SDRAM_AviCapQTabNum_-1]>100
		|| qval[0]<5){
			return;
	}
	for (i=0;i<K_SDRAM_AviCapQTabNum_;i++)
		{
			videoQValueTab[i] = qval[i];
		}
#endif
}



