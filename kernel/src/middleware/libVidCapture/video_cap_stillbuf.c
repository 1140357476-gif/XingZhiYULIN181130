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
 * @author    hy.ren
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "video_capdef.h"
#include "reg_dram.h"
#include "video_mem.h"
#include "intr.h"
#include "video_cap_mem.h"
#include "video_clip.h"
#include "video_capture.h"
#include "dbg_def.h"
#include "reg_jpeg.h"
#include "video_cap_que.h"
#include "video_cap_hal.h"
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern UINT8 fTFrame;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//#define _SpeedyCapturePintf printf
#ifndef _SpeedyCapturePintf
#define _SpeedyCapturePintf /_printf
#endif

void speedyCaptureStillQueInit(vlcBufList_t *pVlcBufQue)
{
	UINT32 snapNum;
	QueParam_t quePram;
	
	//capacity = speedyCaptureFreeBufNumGet();
	snapNum = speedyCaptureParamGet(VD_CAP_COUNT);
	quePram.capacity = snapNum+2;
	quePram.addr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	quePram.size = speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	quePram.rsvSize = 65536+252;
	speedyCaptureInitQue(pVlcBufQue,&quePram);
}


/**
 * @fn		 void intrSpeedyCaptureStillBufSet(vlcBufList_t *vlcQue)
 * @brief	          
 * @param	  [in] vlcQue 
 * @retval	  return none
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-01-25
 * @todo	  N/A
 * @bug 	  N/A
*/
void intrSpeedyCaptureStillBufSet(vlcBufList_t *vlcQue)
{	
	UINT8 ret;
	UINT8 idx;
	UINT32 addr;

	intrPutChar('B');
	/*work around solution:fixed DRAM latch vlcsize time not sync JPG engine latch vlcsize time */
	if(fTFrame==0){ 
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
	
	vlcQue->size +=1;
	if(speedyCaptureIsStillQueFull(vlcQue)){
		return;
	}
	idx = vlcQue->rear+1;
	vlcQue->rear = vlcQue->rear+1;
	if(idx > (vlcQue->capacity)){
		addr = 0;
	}
	else{
		addr = vlcQue->dataNod[idx].vlcAddr;
	}
	
	IntrSpeedyCaptureVlcBufSwitch(addr,&vlcQue->bufIdx);
}

