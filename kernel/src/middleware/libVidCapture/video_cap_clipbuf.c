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
 * @file		video_cap_clip.c
 * @brief		
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h"
#include "common.h"
#include "initio.h"
#include "reg_jpeg.h"
#include "reg_def.h"
#include "intr.h"
#include "utility.h"
#include "video_capdef.h"
#include "video_cap_mem.h"
#include "video_cap_vlcbuf.h"
#include "video_cap_que.h"
#include "video_cap_hal.h"
#include "video_clip.h"

/**************************************************************************
 *				C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *				M A C R O S 
 **************************************************************************/
/**************************************************************************
 *				D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *				G L O B A L   D A T A
 **************************************************************************/


/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
**************************************************************************/
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
 **************************************************************************/

void speedyCaptureClipQueInit(vlcBufList_t *pVlcBufQue,vlcBufList_t *pCashQue)
{
	UINT32 cashNum;
	UINT32 vlcWriteNum;
	UINT32 freeBufNum;
	QueParam_t qParam;
	
	freeBufNum = speedyCaptureFreeBufNumGet();
	vlcWriteNum = 2;
	cashNum = freeBufNum- vlcWriteNum;
	qParam.addr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	qParam.size = speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	qParam.rsvSize = 65536+0x254;
	qParam.capacity = vlcWriteNum;
	

	/* init tmp Write vlc buffer */
	speedyCaptureInitQue(pVlcBufQue,&qParam);
	
	/* init CASHE buffer Write to SD */
	qParam.capacity = cashNum;
	qParam.addr = pVlcBufQue->dataNod[vlcWriteNum-1].bufAddr+qParam.size;
	speedyCaptureInitQue(pCashQue,&qParam);
		
}

extern UINT8 fTFrame;

#if 0
void intrSpeedyCaptureClipBufSet(vlcBufList_t *vlcQue,vlcBufList_t *pCashQue,UINT8 IsHit)
{	
	UINT8 ret;
	UINT32 addr;
	vlcNode_t *pVlcNod = NULL;

	if(fTFrame==0){ //work around way
		if(vlcQue->bufIdx == IDX_A_BUF ){
			XBYTE[REG_Dram_VlcRdy] = 0x01;
		}
		else{
			XBYTE[REG_Dram_VlcRdy] = 0x02;
		}
		vlcQue->bufIdx = 1 - vlcQue->bufIdx;
		fTFrame=1;
		return ;
	}
	
	ret = IntrSpeedyCaptureReadVlcSize(&vlcQue->dataNod[vlcQue->rear].vlcSize);
	if(FAIL == ret){ 
		intrPutUint8(vlcQue->rear);
		return;
	}
	/* VLC size over boundary size, recompress the frame */
	if(vlcQue->dataNod[vlcQue->rear].vlcSize > vlcQue->dataNod[vlcQue->rear].bufSize){
		if(XBYTE[REG_Dram_VlcRdy] & 0x01){
			XBYTE[REG_Dram_VlcRdy] = 0x01;
		}
		else{
			XBYTE[REG_Dram_VlcRdy] = 0x02;
		}
		INTR_putchar('O');
		return;
	}
	pVlcNod = &(vlcQue->dataNod[vlcQue->rear]);
	/* copy data to write cashe buffer */
	if(IsHit){
		if(speedyCaptureIsClipQueFull(pCashQue)){
			goto bufSet;
		}
		speedyCaptureEnterQue(pCashQue,pVlcNod);
	}
	vlcQue->rear = (vlcQue->rear+1)%vlcQue->capacity;
	addr = vlcQue->dataNod[vlcQue->rear].vlcAddr;
bufSet:
	IntrSpeedyCaptureVlcBufSwitch(addr,&vlcQue->bufIdx);
}
#else

extern UINT8 IsVlcReady;
void intrSpeedyCaptureClipBufSet(vlcBufList_t *tmpQue,vlcBufList_t *pCashQue,UINT8 IsHit)
{	
	UINT8 ret;
	UINT32 addr;
	UINT32 vlcSize;

	if(fTFrame==0){ //work around way
		if(tmpQue->bufIdx == IDX_A_BUF ){
			XBYTE[REG_Dram_VlcRdy] = 0x01;
		}
		else{
			XBYTE[REG_Dram_VlcRdy] = 0x02;
		}
		tmpQue->bufIdx = 1 - tmpQue->bufIdx;
		fTFrame=1;
		return ;
	}
	
	ret = IntrSpeedyCaptureReadVlcSize(&vlcSize);
	if(FAIL == ret){ 
		intrPutUint8(tmpQue->rear);
		return;
	}
	
	/* VLC size over boundary size, recompress the frame */
	if(vlcSize > tmpQue->dataNod[tmpQue->rear].bufSize){
		if(XBYTE[REG_Dram_VlcRdy] & 0x01){
			XBYTE[REG_Dram_VlcRdy] = 0x01;
		}
		else{
			XBYTE[REG_Dram_VlcRdy] = 0x02;
		}
		INTR_putchar('O');
		return;
	}
	
	if(IsVlcReady == 1){
		IsVlcReady=0;
		pCashQue->dataNod[pCashQue->rear].vlcSize = vlcSize;
		pCashQue->size++;

		if(IsHit){
			INTR_putchar('V');
		}
	}
	
	tmpQue->rear = (tmpQue->rear+1)%tmpQue->capacity;
	if(IsHit){
		if(speedyCaptureIsClipQueFull(pCashQue)){// drop frame, #64333
			addr =tmpQue->dataNod[tmpQue->rear].vlcAddr;
		}
		else {
			pCashQue->rear = (pCashQue->rear+1)%pCashQue->capacity;
			addr = pCashQue->dataNod[pCashQue->rear].vlcAddr;
			IsVlcReady = 1;
		}
	}
	else{		
		addr = tmpQue->dataNod[tmpQue->rear].vlcAddr;
	}
	
	IntrSpeedyCaptureVlcBufSwitch(addr,&tmpQue->bufIdx);
}

#endif
