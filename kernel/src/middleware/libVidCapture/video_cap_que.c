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
 * @file		video_cap_que.c
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
#include "video_mem.h"
#include "video_capdef.h"
#include "video_cap_mem.h"
#include "video_capture.h"
#include "Sp1k_aud_api.h"
#include "sp1k_hal_api.h"
#include "video_cap_vlcbuf.h"

#include "hal_dram.h"
#include "hal_gprm.h"

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

#if 0 //not used  
void speedyCaptureEnterQue(vlcBufList_t *pQue,vlcNode_t* node)
{
	UINT8 sts;
	
	ENTER_CRITICAL(sts);
	//HAL_GprmDramDmaAdv(node->vlcAddr<<1,pQue->dataNod[pQue->rear].vlcAddr<<1,node->vlcSize,1);
	HAL_GprmDramDmaExt_Intr(node->vlcAddr<<1,pQue->dataNod[pQue->rear].vlcAddr<<1,node->vlcSize,1,1);
	pQue->dataNod[pQue->rear].vlcAddr = node->vlcAddr;
	pQue->dataNod[pQue->rear].vlcSize = node->vlcSize;
	pQue->dataNod[pQue->rear].bufAddr = node->bufAddr;
	pQue->dataNod[pQue->rear].bufSize = node->bufSize;
	pQue->rear = (pQue->rear+1)%(pQue->capacity);
	pQue->size++;
	EXIT_CRITICAL(sts);
}
#endif

UINT8 speedyCaptureIsQueEmpty(vlcBufList_t *pVlcCashQue)
{
	if(pVlcCashQue->size > 0){
		return TRUE;
	}
	
	return FALSE;
}

UINT8 speedyCaptureIsStillQueFull(vlcBufList_t *vlcQue)
{
	UINT8 status;
	
	ENTER_CRITICAL(status);
	if(vlcQue->size >= (vlcQue->capacity)){
		XBYTE[REG_Dram_SetVlcRdy]=0x03;
		XBYTE[REG_Jpeg_Intr_Enable]&=(~0x02); /*stop jpg eof*/
		EXIT_CRITICAL(status);
		return TRUE;
	}
	EXIT_CRITICAL(status);
	
	return FALSE;
}


UINT8 speedyCaptureIsClipQueFull(vlcBufList_t *vlcQue)
{
	if(vlcQue->size + 1>= (vlcQue->capacity)){ // for 46333, but why?
		return TRUE;
	}
	
	return FALSE;
}


vlcNode_t* speedyCaptureNodOutQue(vlcBufList_t *vlcQue)
{
	
	vlcNode_t nod;
	vlcNode_t *pNod = (vlcNode_t*)&nod;
	UINT8 sts;
	
	if(vlcQue->size == 0){
		return NULL;
	}
	ENTER_CRITICAL(sts);
	pNod = (vlcNode_t*)&vlcQue->dataNod[vlcQue->front];
	vlcQue->front =(vlcQue->front+1)%(vlcQue->capacity);
	/*vlcQue->size--;*/
	
	EXIT_CRITICAL(sts);
	
	return pNod;
}


void speedyCaptureInitQue(vlcBufList_t *pBufQue,QueParam_t *pQueParm)
{
	UINT8 i;

	pBufQue->front = 0;
	pBufQue->rear=0; 
	pBufQue->capacity = pQueParm->capacity;
	pBufQue->size = 0;
	pBufQue->bufIdx= 0;

	for(i=0;i <pBufQue->capacity;i++){ 
		 pBufQue->dataNod[i].bufAddr= pQueParm->addr;
		 pBufQue->dataNod[i].bufSize = pQueParm->size<<1;
		 pBufQue->dataNod[i].vlcAddr = pBufQue->dataNod[i].bufAddr + (pQueParm->rsvSize>>1);
		 pBufQue->dataNod[i].vlcSize = 0;
		 pQueParm->addr+=pQueParm->size;
		/* printf("Addr=%lx\n",pBufQue->dataNod[i].bufAddr);*/
	}
	
}
