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
 * @file      hal_moe.h
 * @brief     matrix operation engine
 * @author    wangjg
 * @since     2010-04-21
 * @date      2010-04-21
*/

#ifndef _HAL_MOE_API_H__
#define _HAL_MOE_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MOE_INTR_PROC_Enable 0

typedef enum
{
	// 1:awb 2:ae 3:alc 4:bp
	MOE_Trigger_None=0,
	MOE_Trigger_Awb,
	MOE_Trigger_Ae,
	MOE_Trigger_Alc,
	MOE_Trigger_Bp,
	MOE_Trigger_Other,
}moe_trigger_t;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MOE_EQUATION_Add 0x00
#define MOE_EQUATION_Subtract 0x10
#define MOE_EQUATION_Multiply 0x20
#define MOE_EQUATION_Division 0x30
#define MOE_EQUATION_Absolute 0x40
#define MOE_EQUATION_Max 0x50
#define MOE_EQUATION_Min 0x60
#define MOE_EQUATION_Summation 0x70

#define MOE_FUNC_PointWithPoint 0
#define MOE_FUNC_PointWithCommon 1

#define MOE_FUNC_NoExchange 0
#define MOE_FUNC_Exchange 1

#define MOE_QUEUE_SIZE sizeof(moe_queue_t)
#define MOE_CMD_SIZE sizeof(moe_cmd_t)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct 
{
	UINT8 CmdQueueEnd;// 1 is end
	UINT8 CmdValid;// 1 is valid, 0 GPE Drop this cmd
	UINT8 clipSrc1En;
	UINT8 clipSrc2En;
	UINT8 clipDstEn;
	
	UINT8 func; // 0:PBP opertation 1: PBC Operation	
	UINT8 exchange; // 0 :disable 1: enable
	UINT8 sum; // 0:mul_sum 1: div_sum
	UINT8 equation;	
	// 0: Add 
	// 1: Subtract 
	// 2: Multiply 
	// 3: Division 
	// 4: Absolute difference 
	// 5: Max 3¡¯
	// 6: Min 3¡¯
	// 7: summation function

	UINT16 ROIWidth;

	UINT32 min;	
	UINT32 max;

	UINT32 src1;	
	UINT8 src1Sign;
	UINT8 src1Bit;//0 :8bit 1: 16bit
	UINT8 src1Shift;
	
	UINT32 src2;
	UINT8 src2Sign;//cpSign
	UINT8 src2Bit;
	UINT8 src2Shift;//cpShift

	UINT16 cp;

	UINT32 dst;	
	UINT8 dstSign;
	UINT8 dstBit;
	UINT8 dstShift;
}moe_cmd_t;
typedef struct 
{
	UINT8 opt;
	UINT8 func;
	UINT8 ROIWidth0;
	UINT8 ROIWidth1;
	
	UINT8 min0;
	UINT8 min1;
	UINT8 min2;
	UINT8 min3;
	
	UINT8 max0;
	UINT8 max1;
	UINT8 max2;
	UINT8 max3;

	UINT8 src10;
	UINT8 src11;
	UINT8 src12;
	UINT8 src13;

	UINT8 src20;
	UINT8 src21;
	UINT8 src22;
	UINT8 src23;

	UINT8 dst0;
	UINT8 dst1;
	UINT8 dst2;
	UINT8 dst3;
}moe_queue_t;
typedef struct
{
	UINT8 end:1;// 1 is end
	UINT8 valid:1;// 1 is valid, 0 GPE Drop this cmd
	UINT8 null0:2;
	UINT8 clipSrc1En:1;
	UINT8 clipSrc2En:1;
	UINT8 clipDstEn:1;
	UINT8 null10:1;
}moe_cmd_opt_t;
typedef struct
{
	UINT8 func:1; // 0:PBP opertation 1: PBC Operation	
	UINT8 exchange:1; // 0 :disable 1: enable
	UINT8 sum:1;// 0 :mulSam 1:devSam
	UINT8 null1:1;
	UINT8 equation:3;
	UINT8 null2:1;
}moe_cmd_func_t;
typedef struct
{
	UINT8 src3:1;
	UINT8 sign:1;
	UINT8 bit8:1;//0 :8bit 1: 16bit
	UINT8 shift:5;
}moe_cmd_src_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 moeDbgTrig;
extern UINT32 moeDbgAddr; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//control
void HAL_MoeSetIntrEnable(UINT8 cmdEn,UINT8 cmdqEn,UINT8 div0En,UINT8 shiftOverflowEn);
void HAL_MoeReset();
void HAL_MoeEnable();
void HAL_MoeSetDiv0Value(UINT16 val);
void HAL_MoeSetMode(UINT8 isAuto);
void HAL_MoeSetSum(UINT8 isWriteToDram);
void HAL_MoeInit();
void HAL_MoeSetQueueAddr(UINT32 addr);
UINT8 HAL_MoeIsBusy(UINT8 isQueue);
void HAL_MoeSetCmd(moe_queue_t* pt);
void HAL_MoeGetCmd(moe_queue_t* pt);
//triger
UINT8 HAL_MoeTrigger(moe_trigger_t src);
UINT8 HAL_MoeTriggerCmd(moe_cmd_t* pt,moe_trigger_t src);
UINT8 HAL_MoeTriggerQueue(UINT32 addrW,moe_trigger_t src);
UINT8 HAL_MoeRunCmd(moe_cmd_t *pt,moe_trigger_t src);
UINT8 HAL_MoeRunQueue(UINT32 addrW,moe_trigger_t src);
UINT32 HAL_MoeGetSum();
UINT16 HAL_MoeGetMax();
UINT16 HAL_MoeGetMin();
void HAL_MoeDefCmd(moe_cmd_t* pt);
void HAL_MoeDefQueue(moe_queue_t* pt);
void HAL_MoeConvertToQueue(moe_cmd_t* ptSrc,moe_queue_t* ptDst);
void HAL_MoeConvertToCmd(moe_queue_t* ptSrc,moe_cmd_t* ptDst) ;
void HAL_MoeSetQueueEnd(moe_queue_t *pt,UINT8 idx);
UINT8 HAL_MoeRunQueueIntr(UINT32 cmdaddr,moe_trigger_t src);
UINT8 HAL_MoeTriggerQueueIntr(UINT32 cmdaddr,moe_trigger_t trigger);

UINT8 HAL_MoeTriggerIntr(moe_trigger_t src);

void HAL_MoeDbgIntr();
#endif


