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
 * @file      hal_moe.c
 * @brief     Matrix operation engine
 * @author    wangjg
 * @since     2010-04-01
 * @date      2010-04-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "hal_moe.h"
#include "reg_def.h"
#include "hal.h"
#include "hal_global.h"
#include "dbg_mem.h"
#include "dbg_file_list.h"
#include "intr.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MOE_PRINTF 0
#define MOE_NoUsed 0

#if MOE_PRINTF
UINT32 moeT0,moeT1;
#define moePrintfTime0() HAL_GlobalTimerRead10us(&moeT0);
#define moePrintfTime1(str) HAL_GlobalTimerRead10us(&moeT1);printf("--%s:%lu--\n",str,moeT1-moeT0)
#else
#define moePrintfTime0()
#define moePrintfTime1(str)
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/***********************************************
 *	typedef enum moe_trigger_t
 *	0: N/A
 *	1: AWB flow MOE trigger
 *	2: AE flow MOE trigger
 *	3: ALC flow MOE trigger
 *	4: BP flow MOE trigger
 ***********************************************/
UINT8 moeDbgTrig = MOE_Trigger_None;
UINT32 moeDbgAddr = 0; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void HAL_MoeSetIntrEnable(UINT8 cmdEn, ,UINT8 queueEn, ,UINT8 div0En, ,UINT8 shiftOverflowEn)
 * @brief     moe set interrupt enable
 * @param     [in] cmdEn
 * @param     [in] queueEn
 * @param     [in] div0En
 * @param     [in] shiftOverflowEn
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetIntrEnable(UINT8 cmdEn,UINT8 queueEn,UINT8 div0En,UINT8 shiftOverflowEn)
{
	XBYTE[REG_Moe_IntrEnable]=(cmdEn?0x01:0)|(queueEn?0x02:0)|(div0En?0x04:0)|(shiftOverflowEn?0x08:0);
}

/**
 * @fn        void HAL_MoeReset(void)
 * @brief     reset Moe
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeReset()
{
	XBYTE[REG_Moe_ModuleControl]=0x01;
}

/**
 * @fn        void HAL_MoeEnable(void)
 * @brief     enable moe
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeEnable()
{
	XBYTE[REG_Moe_ModuleControl]=0x00;
}

/**
 * @fn        void HAL_MoeSetDiv0Value(UINT16 val)
 * @brief     set Moe divsion 0 result
 * @param     [in] val
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetDiv0Value(UINT16 val)
{
	XBYTE[REG_Moe_Div0Data0] = READ8(val, 1);
	XBYTE[REG_Moe_Div0Data1] = READ8(val, 0);
}

/**
 * @fn        void HAL_MoeSetMode(UINT8 isAuto)
 * @brief     set moe control mode
 * @param     [in] isAuto
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetMode(UINT8 isAuto)
{
	XBYTE[REG_Moe_OperationMode] |= (isAuto)?1:0;
}

/**
 * @fn        void HAL_MoeSetSum(UINT8 isWriteToDram)
 * @brief     set sum write result to dram
 * @param     [in] isWriteToDram
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetSum(UINT8 isWriteToDram)
{
	XBYTE[REG_Moe_SumOutputEn]=isWriteToDram?1:0;
}

/**
 * @fn        void HAL_MoeInit(void)
 * @brief     initialize moe 
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeInit()
{
	HAL_MoeReset();
	HAL_MoeEnable();
	HAL_MoeSetIntrEnable(0,0,0,0);
	HAL_MoeSetDiv0Value(0x0);
	HAL_MoeSetMode(0);
	HAL_MoeSetSum(1);
}

/**
 * @fn        void HAL_MoeSetQueueAddr(UINT32 addrW)
 * @brief     set moe queue addr(w)
 * @param     [in] addrW(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetQueueAddr(UINT32 addrW)
{
	XBYTE[REG_Moe_CmdQueueAddr0] = READ8(addrW,3);
	XBYTE[REG_Moe_CmdQueueAddr1] = READ8(addrW,2);
	XBYTE[REG_Moe_CmdQueueAddr2] = READ8(addrW,1);
	XBYTE[REG_Moe_CmdQueueAddr3] = READ8(addrW,0);	
}

/**
 * @fn        UINT8 HAL_MoeIsBusy(UINT8 isQueue)
 * @brief     check moe is busy or not
 * @param     [in] isQueue
 * @retval    return = true/false
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeIsBusy(UINT8 isQueue)
{
	return !(XBYTE[REG_Moe_CmdStatus]&(isQueue?0x01:0x02));
}

/**
 * @fn        void HAL_MoeSetCmd(moe_queue_t* pt)
 * @brief     set one cmd queue to register 
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetCmd(moe_queue_t* pt)
{
	UINT8 *pt0 = pt;
	UINT8 *pt1 = &(XBYTE[REG_Moe_CmdOpt]);
	memcpy(pt1,pt0,12);
	memcpy(pt1+0x10,pt0+12,12);
	if(pt->func&0x01)
	{
		XBYTE[REG_Moe_CmdCP0]=pt->src20;
		XBYTE[REG_Moe_CmdCP1]=pt->src21;
	}
}
#if MOE_NoUsed
/**
 * @fn        void HAL_MoeGetCmd(moe_queue_t* pt)
 * @brief     get one cmd queue from register  
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeGetCmd(moe_queue_t* pt)
{
	UINT8 *pt0 = pt;
	UINT8 *pt1 = &(XBYTE[REG_Moe_CmdOpt]);
	memcpy(pt0,pt1,12);
	memcpy(pt0+12,pt1+0x10,12);
	if(pt->func&0x01)
	{
		pt->src20=XBYTE[REG_Moe_CmdCP0];
		pt->src21=XBYTE[REG_Moe_CmdCP1];
	}
}
#endif
/**
 * @fn        void HAL_MoeTrigger(void)
 * @brief     trigger moe
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeTrigger(moe_trigger_t trigger)
{
	//if(XBYTE[0x0e02])XBYTE[0x0e02]=0;//wangjg++ enable moe
	UINT8 sts,rst=0;
	ENTER_CRITICAL(sts);
	//if(!(XBYTE[REG_Moe_CmdStatus]&0x03))return;//busy
	//if(XBYTE[REG_Moe_ModuleControl])
	//	XBYTE[REG_Moe_ModuleControl]=0;//wangjg++ enable moe
	//if(XBYTE[REG_Moe_Intr]&0x03)	
		//XBYTE[REG_Moe_Intr] =0xff;//clear interrupt status
	//if(XBYTE[REG_Moe_CmdStatus])	{
	if(XBYTE[REG_Moe_CmdStatus]&0x03 == 0x03)	{
		//if(XBYTE[REG_Moe_Intr]&0x03)	
			XBYTE[REG_Moe_Intr] =0xff;//clear interrupt status
		//XBYTE[REG_Moe_OperationMode] |= 0x01;
		XBYTE[REG_Moe_CmdTrigger]=1 ;		
		rst =1;
		moeDbgTrig = trigger;
	}
	EXIT_CRITICAL(sts);
	return rst;
}

/**
 * @fn        void HAL_MoeTriggerCmd(moe_cmd_t* pt)
 * @brief     set one cmd to register and trigger moe
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeTriggerCmd(moe_cmd_t* pt,moe_trigger_t trigger)
{
	if(pt)
	{
		moe_queue_t queue;
		HAL_MoeConvertToQueue(pt, &queue);		
		HAL_MoeSetCmd(&queue);		
	}
	HAL_MoeSetMode(0);
	return HAL_MoeTrigger(trigger);
}

/**
 * @fn        void HAL_MoeTriggerQueue(UINT32 addrW)
 * @brief     set cmd queue dram addr to register and trigger moe
 * @param     [in] addrW(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeTriggerQueue(UINT32 addrW,moe_trigger_t trigger)
{
	UINT8 sts,rst=0;
	ENTER_CRITICAL(sts);//wangjg++ disable ie when set moe reg, otherwise reg maybe error
	if(XBYTE[REG_Moe_CmdStatus]&0x03 == 0x03)	{
		if(addrW){
			moeDbgAddr= addrW;
			HAL_MoeSetQueueAddr(addrW);
		}
		HAL_MoeSetMode(1);
		//if(XBYTE[REG_Moe_CmdStatus]&0x01)
			rst = HAL_MoeTrigger(trigger);
		//else
		//	return 0;
	}
	EXIT_CRITICAL(sts);//#0042298 
	return rst;
}

/**
 * @fn        void HAL_MoeRunCmd(moe_cmd_t *pt)
 * @brief     trigger one cmd and waiting for done
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeRunCmd(moe_cmd_t *pt,moe_trigger_t trigger)
{
	UINT32 cnt = 1000000;
	UINT8 rst;
	
	moePrintfTime0();
	//if(XBYTE[0x0e02])XBYTE[0x0e02]=0;//wangjg++ enable moe
	rst = HAL_MoeTriggerCmd(pt,trigger);
	while((!(XBYTE[REG_Moe_CmdStatus]&0x02)) && (--cnt ) && (!XBYTE[REG_Moe_ModuleControl]));
	
	moePrintfTime1("runcmd");
	return rst;
}

/**
 * @fn        void HAL_MoeRunQueue(UINT32 addrW)
 * @brief     trigger cmd queue and waiting for done
 * @param     [in] addrW
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_MoeRunQueue(UINT32 addrW,moe_trigger_t trigger)
{
	UINT32 cnt = 100000;
	UINT8 rst;
moePrintfTime0();
	//if(XBYTE[0x0e02])XBYTE[0x0e02]=0;//wangjg++ enable moe
	rst = HAL_MoeTriggerQueue(addrW,trigger);
	if(rst){
		//while((!(XBYTE[REG_Moe_CmdStatus]&0x01)) &&  (--cnt ) && (!XBYTE[REG_Moe_ModuleControl]));
		while((!(XBYTE[REG_Moe_Intr]&0x01)) &&  (--cnt ) && (!XBYTE[REG_Moe_ModuleControl]));
	}
	if(!cnt){printf("moe time out!\n");rst = 0;}
moePrintfTime1("runCmdq");
	return rst;
}
#if MOE_NoUsed
/**
 * @fn        UINT32 HAL_MoeGetSum(void)
 * @brief    get sum result from register
 * @param     NONE
 * @retval    return = sum
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT32 HAL_MoeGetSum()
{
	UINT32 temp=0;
	WRITE8(temp, 0, XBYTE[REG_Moe_DataSum3]);
	WRITE8(temp, 1, XBYTE[REG_Moe_DataSum2]);
	WRITE8(temp, 2, XBYTE[REG_Moe_DataSum1]);
	WRITE8(temp, 3, XBYTE[REG_Moe_DataSum0]);
	return temp;
}

/**
 * @fn        UINT16 HAL_MoeGetMax(void)
 * @brief     get max value in src1 result from register
 * @param     NONE
 * @retval    return = max
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT16 HAL_MoeGetMax()
{
	UINT16 temp=0;
	WRITE8(temp, 2, XBYTE[REG_Moe_DataMax1]);
	WRITE8(temp, 3, XBYTE[REG_Moe_DataMax0]);
	return temp;
}

/**
 * @fn        UINT16 HAL_MoeGetMin(void)
 * @brief     get min value in src1 result from register
 * @param     NONE
 * @retval    return = min
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT16 HAL_MoeGetMin()
{
	UINT16 temp=0;
	WRITE8(temp, 2, XBYTE[REG_Moe_DataMin1]);
	WRITE8(temp, 3, XBYTE[REG_Moe_DataMin0]);
	return temp;
}
#endif
/**
 * @fn        void HAL_MoeDefCmd(moe_cmd_t* pt)
 * @brief     create one cmd and fill default value
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeDefCmd(moe_cmd_t* pt)
{
	memset(pt,0,sizeof(moe_cmd_t));
	pt->CmdValid = 1;
	pt->clipDstEn = 1;
	pt->max = 0x7f7f7f7f;
}
#if MOE_NoUsed
/**
 * @fn        void HAL_MoeDefQueue(moe_queue_t* pt)
 * @brief     generate one cmd queue and fill default value
 * @param     [in] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeDefQueue(moe_queue_t* pt)
{
	moe_cmd_t cmd;
	HAL_MoeDefCmd(&cmd);
	HAL_MoeConvertToQueue(&cmd, pt);
}
#endif
/**
 * @fn        void HAL_MoeConvertToQueue(moe_cmd_t* ptSrc, ptSrc,moe_queue_t* ptDst)
 * @brief     convert one cmd to queue
 * @param     [in] ptSrc
 * @param     [in] ptDst
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeConvertToQueue(moe_cmd_t* ptSrc,moe_queue_t* ptDst)
{	
	UINT8* pt = (UINT8*)ptDst;
	//printf("ConvertCmd-%x\n",(UINT16)ptDst);
	pt[0] = (ptSrc->CmdQueueEnd?0x01:0 )
		|(ptSrc->CmdValid?0x02:0) 
		|(ptSrc->clipSrc1En?0x10:0)
		|(ptSrc->clipSrc2En?0x20:0)
		|(ptSrc->clipDstEn?0x40:0);

	pt[1] = (ptSrc->func?0x01:0)
		|(ptSrc->exchange?0x02:0)
		|(ptSrc->sum?0x04:0)
		|(ptSrc->equation&0x70);

	pt[2] = READ8( ptSrc->ROIWidth,1);
	pt[3] = READ8( ptSrc->ROIWidth,0);
	
	pt[4] = READ8( ptSrc->min,3);
	pt[5] = READ8( ptSrc->min,2);
	pt[6] = READ8( ptSrc->min,1);
	pt[7] = READ8( ptSrc->min,0);	

	pt[8] = READ8(ptSrc->max,3);
	pt[9] = READ8(ptSrc->max,2);
	pt[10] = READ8(ptSrc->max,1);
	pt[11] = READ8(ptSrc->max,0);

	pt[12] = READ8(ptSrc->src1,3);	
	pt[13] = READ8(ptSrc->src1,2);	
	pt[14] = READ8(ptSrc->src1,1);	
		

	pt[15] = (READ8(ptSrc->src1,0)&0x01)	
		|(ptSrc->src1Sign?0x02:0)
		|(ptSrc->src1Bit?0x04:0)
		|(ptSrc->src1Shift<<3);

	if(ptSrc->func)
	{
		pt[16] = READ8(ptSrc->cp,1);	
		pt[17] = READ8(ptSrc->cp,0);	
		pt[18] = 0;
		pt[19] = 0;
	}
	else
	{
		pt[16] = READ8(ptSrc->src2,3);	
		pt[17] = READ8(ptSrc->src2,2);	
		pt[18] = READ8(ptSrc->src2,1);	
		pt[19] = READ8(ptSrc->src2,0);	
	}	
	pt[19] = (pt[19] &0x01)	
		|(ptSrc->src2Sign?0x02:0)
		|(ptSrc->src2Bit?0x04:0)
		|(ptSrc->src2Shift<<3);


	pt[20] =  READ8(ptSrc->dst,3);
	pt[21] =  READ8(ptSrc->dst,2);
	pt[22] =  READ8(ptSrc->dst,1);
	pt[23] = (READ8(ptSrc->dst,0)&0x01)	
		|(ptSrc->dstSign?0x02:0)
		|(ptSrc->dstBit?0x04:0)
		|(ptSrc->dstShift<<3);

#if MOE_PRINTF==2
if(1)
{
	moe_cmd_t *pt=ptSrc;
	UINT8 *q=ptDst;
	printf("-opt-%bx-%bx,%bx,%bx,%bx,%bx\n",
		q[0],
		pt->CmdQueueEnd,
		pt->CmdValid,
		pt->clipSrc1En,
		pt->clipSrc2En,
		pt->clipDstEn
		);
	printf("-func-%bx-%bx,%bx,%bx,%bx\n",
		q[1],
		pt->func,
		pt->exchange,
		pt->sum,
		pt->equation
	);
	printf("-RIO-%bx,%bx-%x\n",
		q[2],q[3],
		pt->ROIWidth
	
	);
	printf("-min-%bx,%bx,%bx,%bx-%lx\n",
		q[4],q[5],q[6],q[7],
		pt->min	
	);
	printf("-max-%bx,%bx,%bx,%bx-%lx\n",
		q[8],q[9],q[10],q[11],
		pt->max
	);	
	printf("-src1-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[12],q[13],q[14],q[15],
		pt->src1,
		pt->src1Sign,
		pt->src1Bit,
		pt->src1Shift
	);
	printf("-src2-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[16],q[17],q[18],q[19],
		pt->src2,
		pt->src2Sign,
		pt->src2Bit,
		pt->src2Shift
	);
	printf("-dst-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[20],q[21],q[22],q[23],
		pt->dst,
		pt->dstSign,
		pt->dstBit,
		pt->dstShift
	);
}
#endif

}
#if MOE_NoUsed
/**
 * @fn        void HAL_MoeConvertToCmd(moe_queue_t* ptSrc, ptSrc,moe_cmd_t* ptDst)
 * @brief     convert one queue to cmd
 * @param     [in] ptSrc
 * @param     [in] ptDst
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeConvertToCmd(moe_queue_t* ptSrc,moe_cmd_t* ptDst) 
{
	//printfName("ConvertQueue");
	UINT8* pt = (UINT8*)ptSrc;
	
	ptDst->CmdQueueEnd =	(pt[0]&0x01)?1:0;//ptSrc->CmdQueueEnd ;
	ptDst->CmdValid=		(pt[0]&0x02)?1:0;//ptSrc->CmdValid;
	ptDst->clipSrc1En = 		(pt[0]&0x10)?1:0;//ptSrc->clipSrc1En;
	ptDst->clipSrc2En = 		(pt[0]&0x20)?1:0;//ptSrc->clipSrc1En;
	ptDst->clipDstEn = 		(pt[0]&0x40)?1:0;//ptSrc->clipDstEn;
	
	ptDst->func = 			(pt[1]&0x01)?1:0;//ptSrc->func; 
	ptDst->exchange = 		(pt[1]&0x02)?1:0;//ptSrc->exchange; 
	ptDst->sum = 			(pt[1]&0x04)?1:0;//ptSrc->sum;
	ptDst->equation = 		(pt[1]&0x70);//ptSrc->equation;
	
	WRITE8(ptDst->ROIWidth, 1, pt[2]);//ptSrc->ROIWidth0);
	WRITE8(ptDst->ROIWidth, 0, pt[3]);//ptSrc->ROIWidth1);

	WRITE8(ptDst->min, 3, pt[4]);//ptSrc->min0);
	WRITE8(ptDst->min, 2, pt[5]);//ptSrc->min1);
	WRITE8(ptDst->min, 1, pt[6]);//ptSrc->min2);
	WRITE8(ptDst->min, 0, pt[7]);//ptSrc->min3);
	WRITE8(ptDst->max, 3, pt[8]);//ptSrc->max0);
	WRITE8(ptDst->max, 2, pt[9]);//ptSrc->max1);
	WRITE8(ptDst->max, 1, pt[10]);//ptSrc->max2);
	WRITE8(ptDst->max, 0, pt[11]);//ptSrc->max3);

	WRITE8(ptDst->src1, 3, pt[12]);//ptSrc->src10);
	WRITE8(ptDst->src1, 2, pt[13]);//ptSrc->src11);
	WRITE8(ptDst->src1, 1, pt[14]);//ptSrc->src12);
	WRITE8(ptDst->src1, 0, pt[15]&0x01);//ptSrc->src13);	
	ptDst->src1Sign = 	(pt[15]&0x02)?1:0;//ptSrc->src1Sign;
	ptDst->src1Bit = 		(pt[15]&0x04)?1:0;//ptSrc->src1Bit;
	ptDst->src1Shift = 	(pt[15]&0x78)>>3;//ptSrc->src1Shift;

	if(ptSrc->func)
	{
		WRITE8(ptDst->cp, 1, pt[16]);
		WRITE8(ptDst->cp, 0, pt[17]);
		ptDst->src2=0;
		ptDst->src2=0;	

	}
	else
	{
		WRITE8(ptDst->src2, 3, pt[16]);
		WRITE8(ptDst->src2, 2, pt[17]);
		WRITE8(ptDst->src2, 1, pt[18]);
		WRITE8(ptDst->src2, 0, pt[19]&0x01);	
	}
	ptDst->src2Sign = 	(pt[19]&0x02)?1:0;
	ptDst->src2Bit = 		(pt[19]&0x03)?1:0;
	ptDst->src2Shift = 	(pt[19]&0x78);

	WRITE8(ptDst->dst, 3, pt[20]);//ptSrc->dst0);
	WRITE8(ptDst->dst, 2, pt[21]);//ptSrc->dst1);
	WRITE8(ptDst->dst, 1, pt[22]);//ptSrc->dst2);
	WRITE8(ptDst->dst, 0, pt[23]&0x01);//ptSrc->dst3);	
	ptDst->dstSign = 	(pt[23]&0x02)?1:0;//ptSrc->dstSign;
	ptDst->dstBit = 	(pt[23]&0x03)?1:0;//ptSrc->dstBit;
	ptDst->dstShift = 	(pt[23]&0xf8)>>3;//ptSrc->dstShift;

#if MOE_PRINTF==2
if(1)
{
	moe_cmd_t *pt=ptDst;
	UINT8 *q=ptSrc;
	printf("-opt-%bx-%bx,%bx,%bx,%bx,%bx\n",
		q[0],
		pt->CmdQueueEnd,
		pt->CmdValid,
		pt->clipSrc1En,
		pt->clipSrc2En,
		pt->clipDstEn
		);
	printf("-func-%bx-%bx,%bx,%bx,%bx\n",
		q[1],
		pt->func,
		pt->exchange,
		pt->sum,
		pt->equation
	);
	printf("-RIO-%bx,%bx-%x\n",
		q[2],q[3],
		pt->ROIWidth
	
	);
	printf("-min-%bx,%bx,%bx,%bx-%lx\n",
		q[4],q[5],q[6],q[7],
		pt->min	
	);
	printf("-max-%bx,%bx,%bx,%bx-%lx\n",
		q[8],q[9],q[10],q[11],
		pt->max
	);	
	printf("-src1-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[12],q[13],q[14],q[15],
		pt->src1,
		pt->src1Sign,
		pt->src1Bit,
		pt->src1Shift
	);
	printf("-src2-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[16],q[17],q[18],q[19],
		pt->src2,
		pt->src2Sign,
		pt->src2Bit,
		pt->src2Shift
	);
	printf("-dst-%bx,%bx,%bx,%bx-%lx,%bx,%bx,%bx\n",
		q[20],q[21],q[22],q[23],
		pt->dst,
		pt->dstSign,
		pt->dstBit,
		pt->dstShift
	);
}
#endif
}
#endif
/**
 * @fn        void HAL_MoeSetQueueEnd(moe_queue_t *pt, moe_queue_t *pt,UINT8idx)
 * @brief     mask one cmd in queue using end flag; 
 * @param     [in] pt
 * @param     [in] idx
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_MoeSetQueueEnd(moe_queue_t *pt,UINT8 idx)
{
	((UINT8*)(&(pt[idx])))[0]|=0x01;
}
void HAL_MoeSetModeIntr(UINT8 isAuto)
{
	XBYTE[REG_Moe_OperationMode] |= (isAuto)?1:0;
}

/**
 * @fn        void HAL_MoeRunQueueIntr(UINT32 cmdaddr)
 * @brief     HAL_MoeRunQueueIntr
 * @param     [in] cmdaddr
 * @retval    NONE
 * @author    wangjg
 * @since     2010-10-9
*/
UINT8 HAL_MoeRunQueueIntr(UINT32 cmdaddr,moe_trigger_t trigger)
{
	UINT32 cnt = 10000;
	UINT8 rst = 0;
	if(XBYTE[REG_Moe_CmdStatus]&0x03 == 0x03)	{

		rst = HAL_MoeTriggerQueueIntr( cmdaddr,trigger);
		if(rst)while(((XBYTE[0x2e00]&0x01) == 0) &&  (--cnt ) && (!XBYTE[0x2e02]));
		//if(!cnt) printf("moe time out!\n");
	}
	return rst;
}
UINT8 HAL_MoeTriggerQueueIntr(UINT32 cmdaddr,moe_trigger_t trigger)
{
	/*if(!(XBYTE[REG_Moe_OperationMode] & 0x01)){
		//HAL_MoeSetModeIntr(1);
	}*/
	if(XBYTE[REG_Moe_CmdStatus]&0x03 == 0x03){
		//return HAL_MoeTriggerIntr( cmdaddr,trigger);
		if(cmdaddr)
		{
			moeDbgAddr = cmdaddr;			
			XBYTE[REG_Moe_CmdQueueAddr0] = *((UINT8*)&cmdaddr +3);
			XBYTE[REG_Moe_CmdQueueAddr1] = *((UINT8*)&cmdaddr +2);
			XBYTE[REG_Moe_CmdQueueAddr2] = *((UINT8*)&cmdaddr +1);
			XBYTE[REG_Moe_CmdQueueAddr3] = *((UINT8*)&cmdaddr +0);
		}
		XBYTE[REG_Moe_Intr] =0xff;//clear interrupt status
		XBYTE[REG_Moe_OperationMode] = 0x01;
		XBYTE[REG_Moe_CmdTrigger] =0x01; 		
		moeDbgTrig = trigger;
		return 1;
	}
	return 0;	
}
UINT8 HAL_MoeTriggerIntr(moe_trigger_t trigger)
{
	//if(XBYTE[REG_Moe_ModuleControl])XBYTE[REG_Moe_ModuleControl]=0;//wangjg++ enable moe
	if(XBYTE[REG_Moe_CmdStatus]&0x03 == 0x03)//nobusy	
	{
		XBYTE[REG_Moe_Intr] =0xff;
		//XBYTE[REG_Moe_OperationMode] |= 0x01;
		XBYTE[REG_Moe_CmdTrigger] =0x01; 		
		moeDbgTrig = trigger;
		return 1;
	}
	return 0;
}
void HAL_MoeDbgIntr()
{
	UINT8 i;
	UINT8 regs[80];
	//if(XBYTE[REG_Moe_CmdStatus]&0x01)return;	
	memcpy(regs,&XBYTE[REG_Moe_Intr],80);
#if 1//reset moe engine
	//XBYTE[REG_Moe_ModuleControl]=0x01;
	XBYTE[REG_Moe_ModuleControl]=0x01;
	XBYTE[REG_Moe_ModuleControl]=0x00;
#endif
	//printf moe queue addr
	intrPutUint32(moeDbgAddr);
	intrPutChar(',');
	//printf moe trigger src
	intrPutUint8(moeDbgTrig);
	intrPutChar('\n');
#if 1//printf moe reg
	for(i=0;i<80;i++)
	{
		if(!(i&0x0f)){
			intrPutChar('\n');
			intrPutUint16(0x2e00+i);
			intrPutChar(':');
		}
		intrPutUint8(regs[i]);
		intrPutChar(' ');
	}
	intrPutChar('\n');
#endif
	
#if 0
	dumpData(0xf0,(UINT32)&XBYTE[0x2e00],0);
	dumpData(4096,moeDbgAddr,1);
#endif
}

