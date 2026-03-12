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
#include "initio.h"
#include "hal_global.h"
#include "hal.h"
#include "utility.h"
#include "video_debug.h"
#include "video_clip.h"
#include "dbg_wdt.h"

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
#if VD_DEBUG_STATUTE
UINT32 vdTotalLoop = 0;
UINT32 vdDropDisp = 0;
UINT32 vdVlcWaitBuf = 0;
UINT32 vdDispWaitBuf = 0;
UINT32 vdMaxExceed = 0;
#endif

#if VD_DEBUG_TIMING
vdTimer_t vdTimeTest[VD_TIMER_MAX];
UINT32 vdTestCnt[VD_COUNT_MAX];
#endif

#if VD_TEST_MODE
UINT8 testMode=0;
#endif

#if VD_REC_DEBUG
UINT16 xdata vdTimeEvtCnt = 0;
UINT32 vdBigVlcSizeSum = 0;
UINT32 vdBigVlcCnt = 0;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void vdDebugInit(void)
{
#if VD_DEBUG_STATUTE
	vdTotalLoop = 0;
	vdDropDisp = 0;
	vdVlcWaitBuf = 0;
	vdDispWaitBuf = 0;
	vdMaxExceed = 0;
#endif

#if VD_DEBUG_TIMING
	memset((void*)vdTimeTest, 0, sizeof(vdTimeTest));
	memset(vdTestCnt, 0, sizeof(vdTestCnt));
#endif

#if VD_TEST_MODE
	testMode=0;
#endif

#if VD_REC_DEBUG
	vdTimeEvtCnt = 0;
	vdBigVlcSizeSum = 0;
	vdBigVlcCnt = 0;
	
	//printf("%bubuf,%buvlc(%luK), wr(%luK)\n", (UINT8)FRAME_BUF_NUM, (UINT8)G_VideoVlcBufNum,
	//	(UINT32)K_SDRAM_AviCapVLCBufSizeA/1024, G_FragBufSize/1024);
#endif

}

void vdDebugShow(void)
{
#if VD_DEBUG_STATUTE
	printf("loop: %lu, %lu\n", vdTotalLoop, vdDropDisp);
	printf("wait:%lu, %lu\n", vdVlcWaitBuf, vdDispWaitBuf);
	printf("exceed: %lu\n", vdMaxExceed);
#endif

#if VD_DEBUG_TIMING
	{
		UINT8 i;

		vdTimerEnd(0);
		vdTimerEnd(1);
		vdTimerShow(0, VD_TIMER_MODE_GAP_AVG);
		vdTimerShow(1, VD_TIMER_MODE_GAP_AVG);
		//vdTimerShow(2, VD_TIMER_MODE_LENGTH_AVG);
		//vdTimerShow(3, VD_TIMER_MODE_LENGTH_AVG);
		//vdTimerShow(4, VD_TIMER_MODE_LENGTH_AVG);
		//vdTimerShow(5, VD_TIMER_MODE_LENGTH_AVG);
		for(i = 0; i < VD_COUNT_MAX; i ++){
			printf("%lu\n",vdTestCnt[i]);
		}
		//printf("vidChunkcnt=%lu\n", vidPbCtrl.vidChunkCnt);
		//printf("audChunkcnt=%lu\n", vidPbCtrl.audChunkCnt);
	}
#endif

#if VD_REC_DEBUG
	//vdTimeEvtShow();
#endif
}

#if VD_DEBUG_TIMING
void vdTimerStart(UINT8 idx)
{
	UINT32 t;
	HAL_GlobalReadGTimer(&t);
	vdTimeTest[idx].t0 = t;
}

void vdTimerEnd(UINT8 idx)
{
	UINT32 t;
	HAL_GlobalReadGTimer(&t);
	vdTimeTest[idx].t1 = t;
	//printf("t0:%lu\n", t);
}

//t0:start;	t1:end; 	cnt:cnt
void vdTimerGapTrig(UINT8 idx)
{
	UINT32 t;
	
	HAL_GlobalReadGTimer(&t);
	
	if(vdTimeTest[idx].cnt == 0){ //first
		vdTimeTest[idx].t0 = t;
	}
	else{
		vdTimeTest[idx].t1 = t;
	}
	
	vdTimeTest[idx].cnt ++;	
	return;
}

//t0:start,  cnt:cnt, val:sum
void vdTimerAvgLengthEnd(UINT8 idx)
{
	UINT32 t;
	
	HAL_GlobalReadGTimer(&t);

	vdTimeTest[idx].val += t - vdTimeTest[idx].t0;
	vdTimeTest[idx].t0 = t;	
	vdTimeTest[idx].cnt ++;	
	return;
}

void vdTimerMaxLengthEnd(UINT8 idx)
{
	UINT32 t, t2;
	
	HAL_GlobalReadGTimer(&t);

	t2 = t - vdTimeTest[idx].t0;
	vdTimeTest[idx].t0 = t;
	if(vdTimeTest[idx].val < t2){
		vdTimeTest[idx].val = t2;
	}
	return;
}

void vdTimerShow(UINT8 idx, UINT8 mode)
{
	switch(mode){
		default:
		#if 0			
		case VD_TIMER_MODE_LENGTH:
			vdTimeTest[idx].val = vdTimeTest[idx].t1 - vdTimeTest[idx].t0;			
			printf("\n---timer%bu[%bu] : [%lu, %lu, %lu, %lu]\n", mode, idx,
				vdTimeTest[idx].t0, vdTimeTest[idx].t1, vdTimeTest[idx].cnt, vdTimeTest[idx].val);
			
			break;
		#endif
		case VD_TIMER_MODE_GAP_AVG:
			vdTimeTest[idx].val = vdTimeTest[idx].t1 - vdTimeTest[idx].t0;
			
			printf("\n---timer%bu[%bu] : [%lu, %lu, %lu, %lu] %ffps\n", mode, idx,
				vdTimeTest[idx].t0, vdTimeTest[idx].t1, vdTimeTest[idx].cnt, vdTimeTest[idx].val,
				(float)vdTimeTest[idx].cnt * 1000/vdTimeTest[idx].val );
			break;
			
		#if 0			
		case VD_TIMER_MODE_LENGTH_AVG:	
			printf("\n---timer%bu[%bu] : [%lu, %lu, %lu, %lu] %f ms\n", mode, idx,
				vdTimeTest[idx].t0, vdTimeTest[idx].t1, vdTimeTest[idx].cnt, vdTimeTest[idx].val,
				(float)vdTimeTest[idx].val/vdTimeTest[idx].cnt );
			break;

		case VD_TIMER_MODE_LENGTH_MAX:
			printf("\n---timer%bu[%bu] : [%lu, %lu, %lu, %lu]\n", mode, idx,
				vdTimeTest[idx].t0, vdTimeTest[idx].t1, vdTimeTest[idx].cnt, vdTimeTest[idx].val);
			
			break;
		#endif			
	}

	return;
}

#endif

#if VD_TEST_MODE
void vdPbTestModeSet(UINT8 mode)
{
	//printf("vd pb testMode:%bu\n", mode);
	testMode = mode;
}
#endif

#if VD_REC_DEBUG
void HAL_GlobalTimerRead10us_Intr(UINT32 xdata* G10ustimerPt) USING_1
{
	BIT mode;
	UINT32 val;

	mode = XBYTE[0x20C5] & 0x01;

	do {
		*((UINT8 xdata*)G10ustimerPt + 3) = XBYTE[0x20C1];
		*((UINT8 xdata*)G10ustimerPt + 2) = XBYTE[0x20C2];
		*((UINT8 xdata*)G10ustimerPt + 1) = XBYTE[0x20C3];
		*((UINT8 xdata*)G10ustimerPt + 0) = XBYTE[0x20C4];
		
		WRITE8(val, 0, XBYTE[0x20C4]);
		WRITE8(val, 1, XBYTE[0x20C3]);
		WRITE8(val, 2, XBYTE[0x20C2]);
		WRITE8(val, 3, XBYTE[0x20C1]);
	} while(mode ? *G10ustimerPt > val : *G10ustimerPt < val);
}

void vdTimeEvt(UINT8 type)
{
	UINT32 xdata *pt;
	UINT8 x;
	
	pt = (UINT32 xdata *)G_FPGAMemPool;

	ENTER_CRITICAL(x);
	if(vdTimeEvtCnt < sizeof(G_FPGAMemPool)/4){
		HAL_GlobalTimerRead10us(pt+vdTimeEvtCnt);		
		if(vdTimeEvtCnt){
			*(pt+vdTimeEvtCnt) -= *pt;
			*(pt+vdTimeEvtCnt) |= ((UINT32)type) << 24;
		}
		vdTimeEvtCnt ++;
	}
	EXIT_CRITICAL(x);
}

void vdTimeEvt_Intr(UINT8 type)
{
	UINT32 xdata *pt;
	
	pt = (UINT32 xdata *)G_FPGAMemPool;

	if(vdTimeEvtCnt < sizeof(G_FPGAMemPool)/4){
		HAL_GlobalTimerRead10us_Intr(pt+vdTimeEvtCnt);		
		if(vdTimeEvtCnt){
			*(pt+vdTimeEvtCnt) -= *pt;
			*(pt+vdTimeEvtCnt) |= ((UINT32)type) << 24;
		}
		vdTimeEvtCnt ++;
	}
}

void vdTimeEvtSaveFile(UINT8 *aviFilename)
{
	UINT8 filename[12];
	UINT8 i;
	UINT32 dramAddr;

	dbgWdtFeed(10000);
	//HAL_DmaSetSramAddr(0xc000);
	//HAL_DmaSetDramAddr(K_SDRAM_AviCapHeaderBufAddr);
	//HAL_DmaDo(1,0,vdTimeEvtCnt*4,0,0);
	//dramAddr = (0x100000UL + 0xC000UL - 0x4000UL)>>1;
	dramAddr = K_SDRAM_CodeSize+(((UINT16)G_FPGAMemPool -0x4000)>>1);

	for(i = 0; i < 8; i ++){
		filename[i] = aviFilename[i];
	}
    filename[8]  = 'T';
    filename[9]  = 'I';
    filename[10] = 'M';
    filename[11] = '\0';
	dramFileSave(filename, dramAddr, vdTimeEvtCnt*4);	
}

void vdTimeEvtShow(void)
{
	UINT16 i;
	UINT32 xdata *pt;
	UINT8 type;
	UINT32 time;
	extern UINT32 g_bitRate;
	extern UINT32 g_brcTargetSize;

	//printf("brc: %lu, %lu\n", g_bitRate, g_brcTargetSize);
	if(vdBigVlcCnt){
		printf("big: %luK, %lu\n", vdBigVlcSizeSum/vdBigVlcCnt/1024UL, vdBigVlcCnt);
	}
	
#if 0
	pt = (UINT32 xdata *)G_FPGAMemPool;
	printf("vdTimeEvtCnt:%u, (unit:10us)\n", vdTimeEvtCnt);
	printf("type\tjpg\tdma\twr\tdrop\tBound\n");
	for(i = 0; i < vdTimeEvtCnt; i++){
		//printf("%08lx\n", pt[i]);

		type = *(UINT8 xdata *)&pt[i];
		time = pt[i] & 0xffffff;
		printf("%bu\t", type);
		while(type--){
			putchar('\t');
		}
		printf("%lu\n", time);
	}
#endif	
}
#endif
