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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_def.h" 
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
//xian ++
#if VIDEO_REC_SMOOTH_DUMY
#define VD_NEXT_DUMY_SPACE	1
videoRecBufTab_t g_vlcIdxTab[VID_VLCBUFNUM_TOTAL];
UINT8 idxA;
UINT8 idxB;
UINT8 idxFree;
UINT8 idxVlcHead;
UINT8 idxVlcTail;
UINT8 idxVlcDumy;
UINT8 idxVlcPreTail;
UINT16 vlcBufFrameCnt; 
UINT8 vidCnt=0;
SINT32 audDataSize=0;

#endif 
UINT8 g_SlowSnapOpen;
UINT32 vlcSizeTmp;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

 //-----------------------------------------------------------------------------
//INTR_videoRecVlcBufSet
//-----------------------------------------------------------------------------
/**
 * @fn		  void INTR_videoRecVlcBufSet(void)
 * @brief	  Reset vlc buffer address
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void INTR_videoRecVlcBufSet(void)
{
	UINT8 sts;

	#ifdef videoDebug
	//printf("intr:%ld\n",totalFrameCnt );
	#endif

	if(1/*vidClipCtrl.recIdle == VID_REC_RUN*/) {
		sts = INTR_videoRecReadVlcSize();
		if (sts==2){// VLC size over boundary size, recompress the frame
	
			return;
		}
		totalFrameCnt ++;
	}
	videoBrcSizeAvg = (vidVlcSize[vlcBufNum] + videoBrcSizeAvg)>>1;
	vlcABSwitch = 1 - vlcABSwitch;
	if(vidDateStampSts){
		videoStampSetIntr();
	}
	INTR_videoRecVlcBufSwitch();

	
}

UINT8 INTR_videoRecVlcFreeBufGet(void)
{
	UINT8 idx, next;
	UINT8 cnt;
	UINT8 i;
	
	if(idxFree != VD_BUF_IDX_EOF){ // get free buf from free queque
		idx = idxFree;
		idxFree = g_vlcIdxTab[idxFree].next ;
	}
	else{ // get free buf from vlc queque
		if(idxVlcDumy == VD_BUF_IDX_EOF || g_vlcIdxTab[idxVlcDumy].next == VD_BUF_IDX_EOF){ //first dumy
			idxVlcDumy = g_vlcIdxTab[idxVlcHead].next;
		}	
		next = g_vlcIdxTab[idxVlcDumy].next; // idxVlcHead < idxVlcDumy < idxVlcTail

		if(idxVlcDumy < G_VideoVlcBufNum && next < G_VideoVlcBufNum){
			cnt = g_vlcIdxTab[idxVlcDumy].dumyCnt + g_vlcIdxTab[next].dumyCnt;
			if(idxVlcPreTail != VD_BUF_IDX_EOF && g_vlcIdxTab[idxVlcPreTail].dumyCnt < cnt){
				//add dummy at tail
				next = g_vlcIdxTab[idxVlcPreTail].next;
				g_vlcIdxTab[idxVlcPreTail].dumyCnt += g_vlcIdxTab[next].dumyCnt + 1;
				g_vlcIdxTab[idxVlcPreTail].next = g_vlcIdxTab[next].next;
				idx = next;

				if(idx == idxVlcTail){ //del tail
					idxVlcTail = idxVlcPreTail;
				}
			}
			else{
				//g_vlcIdxTab[idxVlcDumy].dumyCnt += g_vlcIdxTab[next].dumyCnt + 1;
				g_vlcIdxTab[idxVlcDumy].dumyCnt = cnt + 1;
				g_vlcIdxTab[idxVlcDumy].next = g_vlcIdxTab[next].next;	
				idx = next;

				if(idx == idxVlcTail){ //del tail
					idxVlcTail = idxVlcDumy;
				}
				//next dumy
				for(i = 0; i < VD_NEXT_DUMY_SPACE && next != VD_BUF_IDX_EOF; i ++){
					next = g_vlcIdxTab[next].next;
				}
				if(next == VD_BUF_IDX_EOF){
					next = g_vlcIdxTab[idxVlcHead].next;
				}
				idxVlcDumy = next;
			}
			DummyFrameCnt ++;			
		}
		else{ //error
			INTR_putchar('f');
			intrPutUint8(idxVlcDumy);
			intrPutUint8(next);
			idx = VD_BUF_IDX_EOF;
		}		
	}

	if(idx < G_VideoVlcBufNum){
		vidVlcSize[idx] = 0;	
		g_vlcIdxTab[idx].dumyCnt = 0;
		g_vlcIdxTab[idx].next = VD_BUF_IDX_EOF;
	}
	else{ //error
		INTR_putchar('F');
		intrPutUint8(idx);
		idx = VD_BUF_IDX_EOF;
	}		
	return idx;
}

void INTR_videoRecVlcBufAdd(UINT8 idx)
{
	if(idx < G_VideoVlcBufNum){
		g_vlcIdxTab[idx].next = VD_BUF_IDX_EOF;
		g_vlcIdxTab[idx].dumyCnt = 0;
		
		if(idxVlcTail == VD_BUF_IDX_EOF && idxVlcHead == VD_BUF_IDX_EOF){//first
			idxVlcHead = idx;
		}
		else if(idxVlcTail < G_VideoVlcBufNum && idxVlcHead < G_VideoVlcBufNum){
			g_vlcIdxTab[idxVlcTail].next = idx;
		}
		else{ //error
			INTR_putchar('v');
			intrPutUint8(idx);
			return;
		}
		idxVlcPreTail = idxVlcTail;
		idxVlcTail = idx;
		vlcBufFrameCnt ++;	
	}
	else{ //error
		INTR_putchar('V');
		intrPutUint8(idx);
	}
}

void INTR_videoRecVlcBufSwitch(void)
{
	UINT8 idx;
	UINT32 addr;	
	extern UINT8 brcQValueIdx;
	if(vidClipCtrl.recIdle != VID_REC_PAUSE) {
		if(g_SlowSnapOpen){
			vidVlcSize[vlcBufNum] = 0;		
			idx = vlcBufNum;
		}
		else if((g_vidAviFrameRate == (vidClipCtrl.frameRate<<1)) && vlcBufNum == idxB) {// 30fps drop to 15fps
			vidVlcSize[idxB] = 0;		
			idx = idxB;
		}
		else{	
			INTR_videoRecVlcBufAdd(vlcBufNum);
			idx = INTR_videoRecVlcFreeBufGet();
			if(g_vidParam[VD_SLOW_REC_MOD]){
				g_SlowSnapOpen =1;
			}
			
		}
		
		if(idxA == vlcBufNum){// A jpg eof
			vlcBufNum = idxB;
			idxA = idx;
		}
		else{
			vlcBufNum = idxA;
			idxB = idx;
		}
		brcQValueIdx=idx;
		//addr = sysMemAddrGet_Intr(MEM_IDX_VIDEO_REC_VLC_A + idx) + (UINT32)130;
		addr = vidRecMemAddrGet_Intr(VID_MEM_IDX_VIDEO_REC_VLC_A + idx) + (UINT32)130+(HUFFMANSIZE>>1);//210 huffman
		if(flip == 0) {
			XBYTE[REG_Dram_VlcAAddr_B0] = *( (UINT8*)&addr + 3 );
			XBYTE[REG_Dram_VlcAAddr_B1] = *( (UINT8*)&addr + 2 );
			XBYTE[REG_Dram_VlcAAddr_B2] = *( (UINT8*)&addr + 1 );
			XBYTE[REG_Dram_VlcAAddr_B3] = *( (UINT8*)&addr );

		}
		else {
			XBYTE[REG_Dram_VlcBAddr_B0] = *( (UINT8*)&addr + 3 );
			XBYTE[REG_Dram_VlcBAddr_B1] = *( (UINT8*)&addr + 2 );
			XBYTE[REG_Dram_VlcBAddr_B2] = *( (UINT8*)&addr + 1 );
			XBYTE[REG_Dram_VlcBAddr_B3] = *( (UINT8*)&addr );

		}
#if VD_REC_DEBUG
		vdTimeEvt_Intr(VD_EVT_JPG_EOF);
#endif
	}
	else{ //pause , fix #41872
		if(idxA == vlcBufNum){// A jpg eof
			vlcBufNum = idxB;
			//idx=idxA;
		}
		else{
			vlcBufNum = idxA;
			//idx=idxB;
		}
		idx=vlcBufNum;
	}
	
	#ifdef VIDEO_REC_BRC_EN
	INTR_VidClipBrcSetQtable(idx);
	#endif
	/*intrPutChar('*');
	intrPutUint8(idxA);
	intrPutUint8(idxB);
	intrPutUint8(vlcBufNum);
	intrPutUint8(idx);
	intrPutChar('*');*/
	if (flip==0) {
		XBYTE[REG_Dram_VlcRdy] = 0x01;	//clear vlcardy
	}
	else {
		XBYTE[REG_Dram_VlcRdy] = 0x02;	//clear vlcbrdy
	}
	flip = 1 - flip;
}
UINT8 INTR_videoRecReadVlcSize(void)
{
	//UINT8 sts = 0;

	 if(vlcSizeTmp == 0){
		if((XBYTE[REG_Dram_VlcRdy] & 0x01) && (!vlcABSwitch)) {
			WRITE8(vidVlcSize[vlcBufNum],3,XBYTE[REG_Dram_VlcASize_B0]);
			WRITE8(vidVlcSize[vlcBufNum],2,XBYTE[REG_Dram_VlcASize_B1]);
			WRITE8(vidVlcSize[vlcBufNum],1,XBYTE[REG_Dram_VlcASize_B2]);
		}
		else if((XBYTE[REG_Dram_VlcRdy] & 0x02) && (vlcABSwitch)) {
			WRITE8(vidVlcSize[vlcBufNum],3,XBYTE[REG_Dram_VlcBSize_B0]);
			WRITE8(vidVlcSize[vlcBufNum],2,XBYTE[REG_Dram_VlcBSize_B1]);
			WRITE8(vidVlcSize[vlcBufNum],1,XBYTE[REG_Dram_VlcBSize_B2]);
		}
		else{ // wait sync
			INTR_putchar('W');
		}
		#if VIDREC_JPEG_EOF_EN
		WRITE8(vlcSizeTmp,3,XBYTE[REG_Jpeg_VLCSize_Byte0]);
		WRITE8(vlcSizeTmp,2,XBYTE[REG_Jpeg_VLCSize_Byte1]);
		WRITE8(vlcSizeTmp,1,XBYTE[REG_Jpeg_VLCSize_Byte2]);
		#endif
    }
	else{
		vidVlcSize[vlcBufNum] = vlcSizeTmp;
		WRITE8(vlcSizeTmp,3,XBYTE[REG_Jpeg_VLCSize_Byte0]);
		WRITE8(vlcSizeTmp,2,XBYTE[REG_Jpeg_VLCSize_Byte1]);
		WRITE8(vlcSizeTmp,1,XBYTE[REG_Jpeg_VLCSize_Byte2]);
	}
	
	if (vidVlcSize[vlcBufNum] > vidVlcBufSize){// VLC size over boundary size, recompress the frame
		vidVlcSize[vlcBufNum] = 0;
		INTR_putchar('K');
		return 2;
	}
	return 0;
}
//#pragma disable //disable interrupt
void videoRecVlcFreeBufAdd(void)
{
	UINT8 idx;
	_VIDEO_INTR_DISABLE();
	idx = idxVlcHead; //free buffer
	if(idx < G_VideoVlcBufNum){	
		//remove free bufer from vlc buf
		idxVlcHead = g_vlcIdxTab[idx].next;
		if(idxVlcHead == VD_BUF_IDX_EOF){
			idxVlcTail = VD_BUF_IDX_EOF;
			idxVlcDumy = VD_BUF_IDX_EOF;
			idxVlcPreTail = VD_BUF_IDX_EOF;
			//printf("*vlcH_255*");
		}
		else if(idxVlcDumy == idxVlcHead){
			idxVlcDumy = g_vlcIdxTab[idxVlcHead].next;
		}
		
		if(idxVlcHead >= G_VideoVlcBufNum && idxVlcHead != VD_BUF_IDX_EOF){//error
			printf("#*y%bx,%bx\n",idxVlcHead, idx);			
		}

		if(idxVlcHead == idxVlcPreTail){
			idxVlcPreTail = VD_BUF_IDX_EOF;
		}
	
		//add free
		vidVlcSize[idx] = 0;
		g_vlcIdxTab[idx].next = idxFree;
		idxFree = idx;
		vlcBufFrameCnt -= g_vlcIdxTab[idx].dumyCnt + 1;
	}
	else{ //error
		printf("#Y%bx,%bx\n",idxVlcHead, idx);			
	}
	_VIDEO_INTR_ENABLE();
}


