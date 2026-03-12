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
 * @file      awb_func.h
 * @brief     awb lib all function
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __AWB_FUNC_H__
#define __AWB_FUNC_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "awb_moe.h"
#include "awb_api.h"
#include "awb_dbg.h"
#include "hal_dram.h"
#include "hal_cdsp_iq.h"
#include "hal_cdsp.h"
#include "hal_gprm.h"
#include "initio.h"
#include "utility.h"
#include "sp1k_rsvblk_api.h"

#include "hal_global.h"
#include "hal.h"

#include "dbg_mem.h"
#include "dbg_def.h"

#include "front_logi.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define AWB_ID_Ctt 0x04
#define AWB_ID_Queue 0x802		//0x4f
#define AWB_ID_CM_A 0x43

#if 0
#define AWB_SAMP_CNT 256
#define AWB_SAMP_XCNT 16
#define AWB_SAMP_YCNT 16
#define AWB_SAMP_WCNT 128
#else
#define AWB_SAMP_CNT 1024ul//256
#define AWB_SAMP_XCNT 32//16
#define AWB_SAMP_YCNT 32//16
#define AWB_SAMP_WCNT 512//128
#endif
#define awbSramBuf 	G_ucStorData
#define AWB_ADDR_Buf	(pawbaddr->buf)//K_SDRAM_DPSStartAddr	//16k
#define AWB_ADDR_Tmp	(pawbaddr->moe)//K_SDRAM_PvMoeTmpAddr	//16k
#define AWB_ADDR_Wnd0	(pawbaddr->wnd)//K_SDRAM_PvAwbWndAddr 	//4k

#define AWB_ADDR_Queue_normal 	(pawbaddr->queue[AWB_SCENE_Normal])
#define AWB_ADDR_Queue_outdoor 	(pawbaddr->queue[AWB_SCENE_Outdoor])
#define AWB_ADDR_Queue_rectOne 	(pawbaddr->queue[AWB_SCENE_RectOne])
//#define AWB_ADDR_Queue_flash 	(pawbaddr->queue[AWB_SCENE_Flash])
//#define AWB_ADDR_Queue_calib 	(pawbaddr->queue[AWB_SCENE_Calibration])

#define AWB_ADDR_Tmp0	(AWB_ADDR_Tmp)
#define AWB_ADDR_Tmp1	(AWB_ADDR_Tmp+512ul)
#define AWB_ADDR_Tmp2	(AWB_ADDR_Tmp+512ul*2)
#define AWB_ADDR_Tmp3	(AWB_ADDR_Tmp+512ul*3)
#define AWB_ADDR_Tmp4	(AWB_ADDR_Tmp+512ul*4)
#define AWB_ADDR_Tmp5	(AWB_ADDR_Tmp+512ul*5)
#define AWB_ADDR_Tmp6	(AWB_ADDR_Tmp+512ul*6)
#define AWB_ADDR_Tmp7	(AWB_ADDR_Tmp+512ul*7)
#define AWB_ADDR_Tmp8	(AWB_ADDR_Tmp+512ul*8)
#define AWB_ADDR_Tmp9	(AWB_ADDR_Tmp+512ul*9)


#define AWB_ADDR_Wnd1	(AWB_ADDR_Tmp+10*512ul)	//4k
#define AWB_ADDR_Gain	(AWB_ADDR_Tmp+10*512ul)	//4k
#define AWB_ADDR_Wp	(AWB_ADDR_Tmp+15*512ul)  	//512
#define AWB_ADDR_RGain	(AWB_ADDR_Tmp+10*512ul)	//4k
#define AWB_ADDR_BGain	(AWB_ADDR_Tmp+12*512ul)	//4k

#define AWB_ADDR_Queue	(pawbaddr->queue[0])		// 8k
#define AWB_ADDR_Info	(AWB_ADDR_Buf+8*512ul)		// 4k
//#define AWB_ADDR_CM	(AWB_ADDR_Info+4*512)		//54byte
//#define AWB_ADDR_SatHue (AWB_ADDR_CM+54/2)		//14byte



#define AWB_INFO_CTT_Offset 8
#define AWB_CTT_PARAM_Offset (160+AWB_INFO_CTT_Offset)
#define AWB_INFO_ADDR_Offset (AWB_SIZE_Ctt+AWB_INFO_CTT_Offset)
#define AWB_INFO_OUT_Offset (sizeof(awb_addr_t)+AWB_INFO_ADDR_Offset)
#define AWB_INFO_AE_Offset (1024-32)
#define AWB_INFO_Timer_Offset (1024-256)
//#define AWB_INFO_ADDR_Offset (sizeof(awb_ctt_t)+8+240)
#define AWB_INFO_SAMP_Offset 1024

#define AWB_ADDR_Flag	(AWB_ADDR_Info)	//8
#define AWB_ADDR_Ctt	(AWB_ADDR_Info+AWB_INFO_CTT_Offset/2)	// 256
#define AWB_ADDR_Param	(AWB_ADDR_Info+AWB_CTT_PARAM_Offset/2) //include in ctt
#define AWB_ADDR_Addr	(AWB_ADDR_Info+AWB_INFO_ADDR_Offset/2)
#define AWB_ADDR_Out	(AWB_ADDR_Info+AWB_INFO_OUT_Offset/2) //240
#define AWB_ADDR_Ae		(AWB_ADDR_Info+AWB_INFO_AE_Offset/2)
#define AWB_ADDR_Timer		(AWB_ADDR_Info+AWB_INFO_Timer_Offset/2)

//#define AWB_ADDR_Addr	(AWB_ADDR_Info+AWB_INFO_ADDR_Offset/2) //32
#define AWB_ADDR_Samp	(AWB_ADDR_Info+AWB_INFO_SAMP_Offset/2)	//3k
#define AWB_ADDR_R		(AWB_ADDR_Samp)//1k
#define AWB_ADDR_G		(AWB_ADDR_Samp+512ul)//1k
#define AWB_ADDR_B		(AWB_ADDR_Samp+1024ul)//1k


#define AWB_FLAG_Spot			1000
#define AWB_FLAG_Spot_Lack		 100
#define AWB_FLAG_Spot_Delay	 200
#define AWB_FLAG_Gain			  	10
#define AWB_FLAG_Gain_Jump	  	20
//#define AWB_FLAG_Gain_Stable	  	30
#define AWB_FLAG_Gain_StableDelay   40
#define AWB_FLAG_Gain_JumpErr   	50
#define AWB_FLAG_Special	 400

#define AWB_FLAG_Rect			2000
#define AWB_FLAG_Rects			 100
#define AWB_FLAG_RectOne		 200
#define AWB_FLAG_Flash			 300
#define AWB_FLAG_Flash_Lack	  10
#define AWB_FLAG_Flash_Last	  20

#define AWB_FLAG_Decide_Pass	10000
#define AWB_FLAG_AutoOb		20000
#define AWB_FLAG_AE_Pass	30000
#define AWB_FLAG_Scene_Pass	40000
#define AWB_FLAG_Post		50000

#define AWB_SPEC_Outdoor	0x0f
#define AWB_SPEC_Sward		0x01
#define AWB_SPEC_Sky		0x02
#define AWB_SPEC_Indoor		0xf0
#define AWB_SPEC_Office		0x10
typedef enum
{
	AWB_SPOT_A = 0,
	AWB_SPOT_CW,
	AWB_SPOT_D,

	AWB_SPOT_4,
	AWB_SPOT_5,

	AWB_SPOT_MAX,
	AWB_SPOT_NUM=3,
}awb_spot_t;
typedef enum
{
	AWB_RECT_Sward=0,
	AWB_RECT_Sky,
	AWB_RECT_Office,
	AWB_RECT_Flash,
	AWB_RECT_Calibration,

	AWB_RECT_SPOT,
	AWB_RECT_MID,

	AWB_RECT_MAX,
	AWB_RECT_NUM=4,
	AWB_RECT_One=AWB_RECT_Flash,

}awb_rect_t;
typedef enum
{
	AWB_SUM_A = 0,
	AWB_SUM_4,
	AWB_SUM_CW,
	AWB_SUM_5,
	AWB_SUM_D,
	AWB_SUM_Rect,
	AWB_SUM_Sward,
	AWB_SUM_Sky,
	AWB_SUM_Office,
	AWB_SUM_Flash ,
	AWB_SUM_Calibration,
	AWB_SUM_NUM,//=10,
	AWB_SUM_NUM_SPOT=5,



}awb_sum_t;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define __ABS(x) ((x>0)?(x):(-x))
 #define _ABS(x,y) ((x>y)?(x-y):(y-x))
 #define _MAX(x,y) (((x)>(y))?(x):(y))
 #define _DIS(x,y,x0,y0) ((_ABS(x,x0)+_ABS(y,y0))>>1)

#define AWB_AUTO_ColorMatrix 0
#define AWB_AUTO_WBOffset 1

#define AWB_NoUsed 0

#define AWB_ENABLE 1
#define AWB_MOE_ENABLE AWB_ENABLE
#define AWB_ENABLE_TEST AWB_ENABLE

#define AWB_CALIBRATION 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if 0
typedef struct
{
	// 0
	UINT16 header[2];
	// 1
	UINT16 rg[4];
	UINT16 bg[4];
	// 2 3 4 5
	UINT8 ratio[16*4];//ratio
	// 6 7 8 9
	UINT16 gain[8*4];
	// 10 11 12 13
	UINT16 rect[4*8];
	// 14 15
	UINT16 param[8*2];
}awb_ctt_v4001_t;
#endif
typedef struct
{
	UINT32 wpsum[5];
	//UINT32 wp[AWB_SUM_NUM],rg[AWB_SUM_NUM],bg[AWB_SUM_NUM],ob[5];
	UINT32 wp[AWB_SUM_NUM];
	UINT32 wp0[AWB_SUM_NUM];
	UINT32 rg0[AWB_SUM_NUM];
	UINT32 bg0[AWB_SUM_NUM];
	UINT32 ob[5];
}awb_filter_sum_t;
/*
typedef struct
{
	UINT32 queue[AWB_SCENE_NUM];
	UINT32 infoPara;		// 512-16
	UINT32 iInfoCtt; 		// 256
	UINT32 out;			// 240=24*10
	UINT32 samp;		// 3k
	UINT32 tmp;
	UINT32 wnd;
	UINT32 wndRst;
}awb_addr_t;
*/
typedef struct
{
	UINT32 init[10];
	UINT32 initAll;
	UINT32 initMoeQueue;

	UINT32 filter[10];
	UINT32 filterAll;
	UINT32 filterWndCpy;
	UINT32 filterMoeRun;

	UINT32 decider[10];
	UINT32 deciderAll;
	UINT32 deciderReadResult;
	UINT32 deciderUpdateScene;
	UINT32 deciderSpot;
}awb_timer_t;
#define AWB_TIME 0
#define AWB_TIME_Printf 1
#if AWB_TIME
#define awbTimerGet(t0) HAL_GlobalTimerRead10us(t0)
#else
#define awbTimerGet(t0)
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern awb_t awb;
//extern awb_addr_t awbAddr;
extern UINT8 xdata G_ucStorData[];
extern UINT16 awbCmA[],awbCmCw[],awbCmD[];
extern UINT8 awbCmASign[],awbCmDSign[];
extern UINT8 awbUVSat;//,satEn;

extern awb_timer_t awbTimer;
extern UINT8 awbDebugCnt;
extern UINT16 awbctmin,awbctmax;
//extern UINT16 awbfixRg0,awbfixRg1,awbfixBg0,awbfixBg1;
//extern UINT16 awbRangeRg0,awbRangeRg1,awbRangeBg0,awbRangeBg1;
extern UINT16 awbRectRange[4];

extern code UINT8 awbQueueTbl[1024*8] ;
extern const awb_ctt_t * pawbctc ;
extern const awb_t * pawb ;
extern const awb_addr_t * pawbaddr ;

extern UINT8 codeWriteBack(UINT32 bank,UINT32 offset,UINT32 addrw,UINT32 size);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void awbMoeQueueCreate(awb_ctt_t* ptCtt);


void awbDecider();//awb_filter_sum_t* rst);
void awbDeciderInit();
void awbRectsDecider(awb_filter_sum_t* rst);
void awbRectsFlash(awb_filter_sum_t* rst);
//void awbRectOne(awb_filter_sum_t* rst,UINT8 idx,UINT16 xdata*rg,UINT16 xdata* bg);
void awbSpotsDecider(awb_filter_sum_t* rst);
void awbSpotsWp(awb_filter_sum_t* rst,UINT8 *max0,UINT8 *max1);
void awbSpotsAvg(awb_filter_sum_t* rst,UINT8 idx);
UINT8 awbSpotsCase(awb_filter_sum_t* rst,UINT8 max0,UINT8 max1);
UINT8 awbSpotsDelay(UINT8 idx);
void awbSpotsSpecial(awb_filter_sum_t* rst,UINT8 idx);
UINT8 awbSpotsLack(awb_filter_sum_t* rst,UINT8 idx);
UINT8 awbConvergeGain(void);
void awbGainAvg(void);
void awbGainJump(void);
//void awbPostRect(UINT8 idx);
void awbPostSpots();
//void awbPostFlash();
//void awbPostCalibration();
void awbGainSet(UINT16 rg,UINT16 bg);
UINT8 awbProcSet(UINT8 proc);
//UINT8* awbDeciderParamTable(UINT8 idx);
//UINT16 awbGainSet(UINT8 idx,UINT16 val);
void awbDeciderInfo();
void awbDefaultProc();

//void awbFilter();//awb_filter_sum_t *rst);
UINT8 awbFilter();
//void awbFilterInit();
UINT8 awbGetAeInfo();
UINT8 awbSceneUpdate(awb_filter_sum_t *sum);
void awbReadWindow();
void awbFilterResult(awb_filter_sum_t *sum);
//UINT8* awbFilterParamTable(UINT8 idx);
//void awbFilterInfo();
//awb_addr_t* awbAddrSet(UINT32 addrBufW,UINT32 addrQueueW,UINT32 addrWndW,UINT32 addrMoeW);
UINT32 awbInitCtt(UINT8* ptctt,UINT8 fileId,UINT8* fname);

void awbMoeModifyYThr(UINT8 ymin,UINT8 ymax,UINT8 wpmax);

void awbMoeRaw(awb_moe_raw_t*ptCmd,UINT8 bayerPtn);
void awbMoeRect(
	awb_moe_rect_t*ptCmd
	,UINT8 idx
	,UINT8 mode // 0:wp 1:sum wp 2:sum gain 3:sum wp gain
	);
void awbPostRect(UINT16 rg0,UINT16 rg1,UINT16 bg0,UINT16 bg1);
void awbInit();
//void awbQueueCreate();
//void awbQueueLoad();
//void awbQueueSave();
UINT8 awbInitCheck(awb_addr_t* paddr);
void awbInitStatus(awb_addr_t* paddr);
//void awbQueueLoad(UINT8 fid,UINT8* fname);
#endif  /* __TEMPLATE_HDR_H__ */
