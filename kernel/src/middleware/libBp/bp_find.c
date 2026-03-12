/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

/**
 * @file		bp_find.c
 * @brief		badpixel find function
 * @author	Phil Lin
 * @since		2008-11-19
 * @date		2008-11-18
 */
 
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h" 
#include "initio.h"
#include "main.h" 
#include "hal.h" 
#include "doslink.h"
#include "dbg_def.h"

#include "bp_private.h" 
#include "bp_api.h"

#include "sp1k_rsvblk_api.h"
#include "hal_dram.h"
#include "hal_gprm.h"

#include "utility.h"

#if BP_USE_MOE
#include "Hal_Moe.h"
#include "sp1k_util_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"
#endif

#if MODULE_BP_OPT
/**************************************************************************
 *  						 C O N S T A N T S  						  *
 **************************************************************************/
#define __FILE	__FILE_ID_BP_FIND__

#define DBG_BP_EN	0
/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/
#define ABS(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x)) 
	
#if BP_CMP_BY_FW /* little endian */

#if BP_USE_MOE
	
#define bpBadPixelAddrSet(A,B)\
	BP_DRAM_SetStartAddress(A, B);

#define bpBadPixelSave(X, Y) \
	if(g_badPixelCnt != *gp_bpNumMax){\
		g_badPixelCnt ++;	\
		BP_DRAM_WriteWord(READ8(X, 0), READ8(X, 1));\
		BP_DRAM_WriteWord(READ8(Y, 0), READ8(Y, 1));\
	}
	
#else

#define bpBadPixelAddrSet(A,B)\
	HAL_DramStartAddrSet((UINT32)A, B);	

#define bpBadPixelSave(X, Y) \
	if(g_badPixelCnt != *gp_bpNumMax){\
		g_badPixelCnt ++;	\
		HAL_DramWordDataWrite((UINT8)((X)>>8), (UINT8)(X)); \
		HAL_DramWordDataWrite((UINT8)((Y)>>8), (UINT8)(Y)); \
	}
	
#endif
	
#else /* big endian */
#define bpBadPixelSave(X, Y) \
	if(g_badPixelCnt != *gp_bpNumMax){\
		g_badPixelCnt ++;	\
		HAL_DramWordDataWrite((UINT8)(X), (UINT8)((X)>>8)); \
		HAL_DramWordDataWrite((UINT8)(Y), (UINT8)((Y)>>8)); \
	}
#endif

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *  					   G L O B A L    D A T A   					  *
 **************************************************************************/
static UINT32 s_bpBadpixelAddr;
static UINT32 s_bpBadpixelFlagAddr;
static UINT16 s_bpRawWidth, s_bpRawHeight;
static UINT32 s_bpRawSize;

#if BP_USE_MOE
static UINT32 g_bpMoeQueueAddr; //buffer for Moe queue and Moe result
static UINT32 g_bpMoeRawAddr; //buffer for store the RAW Moe need
static UINT32 g_bpMoeResAddr;

static UINT8 g_w7HighByte,g_w7LowByte;
static UINT16 g_bpMoeRoiWidth; 

static UINT32 g_bpMoeRawBlk1Addr;
static UINT32 g_bpMoeRawBlk2Addr;

static UINT32 g_bpPreBlkAvg1;
static UINT32 g_bpPreBlkAvg2;
static UINT32 g_bpPreBlkAvg;
UINT8 testSaveFlag;
#endif

/**************************************************************************
 *  			   E X T E R N A L    R E F E R E N C E S   			  *
 **************************************************************************/

/**************************************************************************
 *  			 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static void bpStripRead(UINT32 srcAddr, UINT32 dmaOff);
static void bpStripAvgs(UINT8* pAvg, UINT8 cnt);
static void bpStripSearch(UINT16 startX, UINT16 startY, UINT8* avgBuf,UINT8 mode);

static UINT8 bpDataSortProc(UINT32 srcAddr, UINT32 tmpAddr, UINT16 rawW, UINT16 rawH);
static UINT8 bpDataSort_Fill(UINT32 srcAddr, UINT32 tmpAddr, UINT16 rawW, UINT16 rawH);
static UINT8 bpDataSort_Strip(UINT32 srcAddr, UINT32 tmpAddr);

static void bpBadPixelFlagSave(UINT8 flagVal, UINT16 X, UINT16 Y,UINT32 bpflgAddr);
static void bpBadPixelDirSet(UINT32 BadpixelAddr);
static UINT8 bpBadPixelFlagValGet(UINT16 X, UINT16 Y);

#if BP_USE_MOE
static UINT32  bpMoeInit(UINT32 rawAddr,UINT32 moeAddr,UINT16 roiWidth)
{	
	UINT32 bpMoeSrc1Addr ;	/*RAW Addr*/
	UINT32 bpMoeDstAddr;
	UINT32 bpMoeQueueAddr;
	
	moe_cmd_t cmd;
	UINT8 buf[48];
	moe_queue_t xdata* queue = (moe_queue_t xdata*)buf;
	UINT16 size;	
	UINT8 idx;
	UINT8 winLength = roiWidth;
	UINT8 bitn;
#if 0
	{
		UINT32 sum = 0;
		UINT16 i;
		UINT8 LowByteData,HighByteData;
		HAL_DramStartAddrSet(rawAddr,1);
		for(i = 0; i < roiWidth ;i++){
			HAL_DramWordDataRead(&LowByteData,&HighByteData);
			sum += HighByteData;
		}
		printf("sum = %#lx\n",sum);
	}
#endif
	bpMoeSrc1Addr = rawAddr;
	bpMoeQueueAddr = moeAddr; 
	bpMoeDstAddr = bpMoeQueueAddr + 24/*(UINT32)(sizeof(buf)>>1)*/;
	
//	printf("\033[32mbpMoeQueueAddr = %#lx ,  bpMoeDstAddr =%#lx\033[0m\n",bpMoeQueueAddr,bpMoeDstAddr);
//	printf("\033[32mbpMoeSrc1Addr = %#lx ,	bpMoeDstAddr =%#lx\033[0m\n",bpMoeSrc1Addr,bpMoeDstAddr);
		
	bitn = 1;		/*16Bits*/

	HAL_MoeDefCmd(&cmd);
	cmd.ROIWidth = winLength;
	cmd.equation = MOE_EQUATION_Summation;
	cmd.func = MOE_FUNC_PointWithCommon;	
	
	cmd.sum = 0;
	cmd.max = 0xffffffff;	
	cmd.min = 0x00; 
	
	cmd.src1Bit = bitn; 
	cmd.src1Shift = 0;
	cmd.src1Sign = 0;
	cmd.src1= bpMoeSrc1Addr;
	
/*	cmd.src2Bit = 0;

	cmd.src2Shift = 0;
	cmd.src2Sign = 0;
	cmd.src2 = bpMoeSrc2Addr;*/
	
	cmd.cp = 1;
	
	cmd.dstBit = 1;
	cmd.dstSign = 0;
	cmd.dstShift = 8;
	cmd.dst = bpMoeDstAddr;
	
	cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, queue++);
	
	size = (UINT16)((UINT8 xdata*)queue) - (UINT16)buf; 
	idx = size/sizeof(moe_queue_t) -1;

	//printf("\033[32msize = %u ,  idx =%bu\033[0m\n",size,idx);
	
	HAL_MoeSetQueueEnd((moe_queue_t *) buf, idx);
	HAL_DramWriteBytesAddr(bpMoeQueueAddr, buf,48, 0);
		
	//dramFileSave("MOE", bpMoeQueueAddr, (UINT16)((UINT8*)queue) - (UINT16)buf);

	/*MOE trigger*/
	//HAL_MoeRunQueue(bpMoeQueueAddr , MOE_Trigger_Other);
	
	HAL_DramStartAddrSet(bpMoeQueueAddr+7,1);
	HAL_DramWordDataRead(&g_w7LowByte,&g_w7HighByte);	
	
	HAL_MoeReset();
	HAL_MoeEnable();
	//printf("L7=%#bx , H7 = %#bx\n",g_w7LowByte,g_w7HighByte);
	
	/*printf("\n0x[2E18] = %#bx\n",XBYTE[0x2E18]);
	printf("0x[2E19] = %#bx\n",XBYTE[0x2E19]);
	printf("0x[2E1A] = %#bx\n",XBYTE[0x2E1A]);
	printf("0x[2E1B] = %#bx\n",XBYTE[0x2E1B]);
	printf("0x[2E1C] = %#bx\n",XBYTE[0x2E1C]);
	printf("0x[2E1D] = %#bx\n",XBYTE[0x2E1D]);
	printf("0x[2E1E] = %#bx\n",XBYTE[0x2E1E]);
	printf("0x[2E1F] = %#bx\n",XBYTE[0x2E1F]);*/
	
	//dramFileSave("TESTSAVEBIN",bpMoeSrc1Addr,winLength*2);

	return bpMoeDstAddr;
}


static void bpMoeTrigger(void)
{
	/*MOE trigger*/
	//UINT32 str,end;
	//UINT32 i;
	//UINT8 w6LowByte,w6HighByte,w7LowByte,w7HighByte;
	UINT32 bpMoeQueueSrc1Addr,bpMoeQueueRoiWidthAddr;
	//UINT8 bpMoeRoiWidthLow,bpMoeRoiWidthHigh;

	bpMoeQueueSrc1Addr = g_bpMoeQueueAddr + 6;
	bpMoeQueueRoiWidthAddr = g_bpMoeQueueAddr +1;
	
	//dbgWdtFeed(10000000);	/* give 10s counter value to  watch dog */
	//HAL_GlobalReadGTimer(&str);
	//printf("g_bpMoeRawAddr = %#lx\n",g_bpMoeRawAddr);
	/*for(i =0 ; i<(UINT32)78732;i++)*/{
		//printf("g_bpMoeRawAddr = %#lx\n",g_bpMoeRawAddr);
		//printf("g_bpMoeRoiWidth = %u\n",g_bpMoeRoiWidth);
		//w6LowByte = READ8(g_bpMoeRawAddr,3);
		//w6HighByte = READ8(g_bpMoeRawAddr,2);
		//w7LowByte = READ8(g_bpMoeRawAddr,1);
		//w7HighByte =  (g_w7HighByte & 0xfe) |((READ8(g_bpMoeRawAddr,0))&0x01);
		
		//printf("L1=%#bx , H1 = %#bx\n",w6LowByte,w6HighByte);
		//printf("L2=%#bx , H2 = %#bx\n",w7LowByte,w7HighByte);

		/*modify src1 Addr*/
		M_DRAM_WaitReady;  /*MUST wait for DRAM ready , else will write to the error  DRAM Addr*/
		BP_DRAM_SetStartAddress(bpMoeQueueSrc1Addr,0);
		BP_DRAM_WriteWord(READ8(g_bpMoeRawAddr,3), READ8(g_bpMoeRawAddr,2));
		BP_DRAM_WriteWord(READ8(g_bpMoeRawAddr,1), (g_w7HighByte & 0xfe) |((READ8(g_bpMoeRawAddr,0))&0x01)); 

		/*modify src1 Roi Width*/	
		//bpMoeRoiWidthLow = (UINT8)(g_bpMoeRoiWidth&0xff);
		//bpMoeRoiWidthHigh= (UINT8)((g_bpMoeRoiWidth>>8)&0xff);
		M_DRAM_WaitReady;  /*MUST wait for DRAM ready , else will write to the error  DRAM Addr*/
		BP_DRAM_SetStartAddress(bpMoeQueueRoiWidthAddr,0);	
		BP_DRAM_WriteWord(READ8(g_bpMoeRoiWidth, 1), READ8(g_bpMoeRoiWidth, 0));
		
		//dramFileSave("MOE1", g_bpMoeQueueAddr,24);
		M_DRAM_WaitReady;  /*MUST wait for DRAM ready , else will write to the error  DRAM Addr*/		
		
		#if 1
		HAL_MoeRunQueue(g_bpMoeQueueAddr , MOE_Trigger_Other);
		#else
		if(HAL_MoeRunQueue(g_bpMoeQueueAddr , MOE_Trigger_Other) == 0){
			dramFileSave("MOEQUEUE", g_bpMoeQueueAddr,24);
			printf("g_bpMoeQueueAddr = %#lx,g_bpMoeRawAddr=%#lx,g_bpMoeRoiWidth=%#x",g_bpMoeQueueAddr,g_bpMoeRawAddr,g_bpMoeRoiWidth);
			printf("bpMoeQueueRoiWidthAddr = %#lx",bpMoeQueueRoiWidthAddr);
			printf("bpMoeRoiWidthLow = %#bx,bpMoeRoiWidthHigh=%#bx",bpMoeRoiWidthLow,bpMoeRoiWidthHigh);
			while(1);
		}
		#endif
		/*printf("\n0x[2E18] = %#bx\n",XBYTE[0x2E18]);
		printf("0x[2E19] = %#bx\n",XBYTE[0x2E19]);
		printf("0x[2E1A] = %#bx\n",XBYTE[0x2E1A]);
		printf("0x[2E1B] = %#bx\n",XBYTE[0x2E1B]);
		printf("0x[2E1C] = %#bx\n",XBYTE[0x2E1C]);
		printf("0x[2E1D] = %#bx\n",XBYTE[0x2E1D]);
		printf("0x[2E1E] = %#bx\n",XBYTE[0x2E1E]);
		printf("0x[2E1F] = %#bx\n",XBYTE[0x2E1F]);*/
	}
	//HAL_GlobalReadGTimer(&end);	
	//printf("time MOE : %lu\n",end-str);
}

/******************************************
 * @fn        void bpMoeResGet(UINT32 resAddr, resAddr,UINT32 *sum, *sum,UINT8 *max, *max,UINT8 *min)
 * @brief        bpMoeResGet
 * @param     [in] resAddr
 * @param     [in] sum
 * @param     [in] max
 * @param     [in] min
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-08 , 17:18:48
 * @todo        N/A
 * @bug         N/A
*******************************************/
void bpMoeResGet(UINT32 resAddr,UINT16 *sum,UINT8 *max,UINT8 *min)
{
	UINT8 LowByteData,HighByteData;
	
	BP_DRAM_SetStartAddress(resAddr,1);
	
	BP_DRAM_Readword(LowByteData,HighByteData);
	//printf("L1=%#bx , H1 = %#bx\n",LowByteData,HighByteData);
	//*sum |= (UINT32)LowByteData + ((UINT32)HighByteData<<8);
	//WRITEP8(sum, 3, LowByteData);
	//WRITEP8(sum, 3, HighByteData);
	WRITEP8(sum, 1, LowByteData);
	WRITEP8(sum, 0, HighByteData);
	BP_DRAM_Readword(LowByteData,HighByteData);/*High 16bits is not used*/
	//printf("L2=%#bx , H2 = %#bx\n",LowByteData,HighByteData);
	//*sum |= ((UINT32)LowByteData<<16) + ((UINT32)HighByteData<<24);
	//WRITEP8(sum, 1, LowByteData);
	//WRITEP8(sum, 0, HighByteData);
	
	BP_DRAM_Readword(LowByteData,HighByteData);
	//printf("L3=%#bx , H3 = %#bx\n",LowByteData,HighByteData);
	*min = HighByteData;
	
	BP_DRAM_Readword(LowByteData,HighByteData);
	//printf("L4=%#bx , H4 = %#bx\n",LowByteData,HighByteData);
	*max = HighByteData;

#if 0
	{
		UINT16 sum1 = 0;
		UINT16 i;
		UINT8 LowByteData,HighByteData;
		HAL_DramStartAddrSet(g_bpMoeRawAddr,1);
		for(i = 0; i < g_bpMoeRoiWidth ;i++){
			HAL_DramWordDataRead(&LowByteData,&HighByteData);
			sum1 += HighByteData;
		}
		//printf("sum = %#x\n",sum1);
		//*sum = sum1;
		if(ABS(sum1, *sum) > 64){
			printf("ERR SUM!\n");
			while(1);
		}
	}
#endif

	//printf("sum(%#lx),max(%#bx),min(%#bx)\n",*sum,*max,*min); 
}

/******************************************
 * @fn        void bpBlockRead(UINT32 srcAddr, srcAddr,UINT32 dstAddr, UINT32 dmaOff)
 * @brief        bpBlockRead  : read 8x8 black
 * @param     [in] srcAddr : word
 * @param     [in] dstAddr : word
 * @param     [in] dmaOff  : word
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-08 , 17:06:43
 * @todo        N/A
 * @bug         N/A
*******************************************/
static void bpBlockMoeRead(
	UINT32 srcAddr,
	UINT16 rawSizeH,
	UINT16 rawSizeV,
	UINT16 rawOffH,
	UINT16 rawOffV,
	UINT32 dstBlk1Addr, 
	UINT32 dstBlk2Addr,
	UINT16 blkSizeH,
	UINT16 blkSizeV
)
{
	#if 0
	UINT8 stripW = /*g_stripWidth*/16;
	UINT8 stripH = /*g_stripHeight*/8;
	
	while (stripH--) {
		HAL_GprmDramDma((srcAddr<<1) , (dstAddr<<1) , stripW, 1);
		srcAddr += dmaOff;
		dstAddr += stripW>>1;
	}
	#endif
	#if 1
	HAL_GprmCopy(
		srcAddr,  rawSizeH, rawSizeV,
		rawOffH, rawOffV,
		dstBlk1Addr,  blkSizeH,  blkSizeV,
		0 , 0,
		blkSizeH,  blkSizeV,
		0, 1
	);
	if(testSaveFlag){
		sp1kDramFileSave("11111111RAW",dstBlk1Addr,(UINT32)blkSizeH*blkSizeV<<1);
	}
	
	/*Blk2*/
	HAL_GprmCopy(
		dstBlk1Addr,  blkSizeH<<1, blkSizeV>>1,
		blkSizeH , 0,
		dstBlk2Addr,  blkSizeH,  blkSizeV>>1,
		0 , 0,
		blkSizeH,blkSizeV>>1,
		0, 1
	);
	
	if(testSaveFlag){
	sp1kDramFileSave("22222222RAW",dstBlk2Addr,(UINT32)blkSizeH*blkSizeV);
	}
	/*Blk1*/
	HAL_GprmCopy(
		dstBlk1Addr,  blkSizeH<<1, blkSizeV>>1,
		0 , 0,
		dstBlk1Addr,  blkSizeH,  blkSizeV>>1,
		0 , 0,
		blkSizeH,blkSizeV>>1,
		0, 1
	);
	
	if(testSaveFlag){
	sp1kDramFileSave("33333333RAW",dstBlk1Addr,(UINT32)blkSizeH*blkSizeV);
	}
	//while(1);
	#endif
	
}

/******************************************
 * @fn        UINT8 bpBlockMoeSearch(UINT16 blkPixelSum, UINT8 *avg)
 * @brief        bpBlockMoeSearch
 * @param     [in] blkPixelSum : pixel sum of this block 
 * @param     [in] avg : the avg of this block
 * @retval      return : 1: this block has bad pixel
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-09 , 11:01:01
 * @todo        N/A
 * @bug         N/A
*******************************************/
static UINT8 bpBlockMoeSearch(UINT16 blkPixelSum , UINT8 *avg)
{
	UINT16 sum,blkAvgTmp16;
	UINT8 max,min,diff1,diff2;
	UINT8 blkAvg;
	UINT8 ret;

	sum = max = min = ret = 0;
	
	bpMoeTrigger();
	bpMoeResGet(g_bpMoeResAddr , &sum , &max , &min);

	blkAvgTmp16 = sum/blkPixelSum;
	blkAvg = (UINT8)(blkAvgTmp16 > 0xff ? 0xff : blkAvgTmp16);
	
	//blkAvg =  (UINT8)((sum/blkPixelSum)&0xff);
	
	//printf("sum = %lu,blkSum = %u,blkAvg = %bu\n",sum,blkPixelSum,blkAvg);
	
	//g_bpPreBlkAvg = 0;  //1 only for test
	
	if(g_bpPreBlkAvg){
		*avg = (UINT8)(((UINT16)blkAvg + g_bpPreBlkAvg)>>1);
	}
	else{
		*avg = blkAvg;
	}

	blkAvg = *avg;
	g_bpPreBlkAvg = blkAvg;
	
	//printf("Avg = %bu\n",blkAvg);
	
	//printf("Avg = %bu ,g_threshold = %bu\n",blkAvg,g_threshold);
	diff1 = ABS(blkAvg, max); 
	diff2 = ABS(blkAvg, min); 
	//printf("d1=%bu,d2=%bu\n",diff1,diff2);
	if (diff1 > g_threshold || diff2 > g_threshold) {
		ret = 1;
	}
	
	//printf("ret=%bu\n",ret);
	return ret;
}


/******************************************
 * @fn        void bpBlockRead(UINT32 srcAddr, UINT32 dmaSize)
 * @brief        bpBlockRead : read the block to SRAM
 * @param     [in] srcAddr : the data in DRAM Addr
 * @param     [in] dmaSize
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-09 , 11:04:18
 * @todo        N/A
 * @bug         N/A
*******************************************/
static void bpBlockRead(UINT32 srcAddr, UINT32 dmaSize) 
{
	//UINT16 dstAddr = 0x0000;

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(srcAddr);
	HAL_DmaDo(0, 1, dmaSize, 0, 0);
}

/******************************************
 * @fn        void bpBlockSearch(UINT16 startX, UINT16 startY, UINT8 avg)
 * @brief        bpBlockSearch ,search bad pixel in the block
 * @param     [in] startX
 * @param     [in] startY
 * @param     [in] avg
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-09 , 11:05:21
 * @todo        N/A
 * @bug         N/A
*******************************************/
#pragma OT(8, SPEED)

static void bpBlockSearch(UINT16 startX, UINT16 startY, UINT8 avg)
{
	UINT8 diff;
	UINT8 i,j;
	UINT8 val,off;
	UINT8 planeHeightW = g_planeHeight << 1;
	UINT8 planeWidthW = g_planeWidth << 1;
	//printf("g_planeWidth = %bu,g_planeHeight = %bu\n",g_planeWidth,g_planeHeight);

	//printf("startX = %u,startY=%u\n",startX,startY);
	off = 1;
	for(j = 0 ; j < planeHeightW ; j+=2){
		for (i = 0; i < planeWidthW; i+=2) {
			val = G_ucStorData[off];		
			//printf("VAL = %bu\n",val);
			diff = ABS(val, avg); 
			if (diff > g_threshold) {
				bpBadPixelFlagSave(0x01, startX, startY, s_bpBadpixelFlagAddr);
				//type=bpTypeGet(startX,off-1,mode);
				bpBadPixelAddrSet(s_bpBadpixelAddr, 0); 
				bpBadPixelSave(startX, startY);
				s_bpBadpixelAddr +=2;
			#if 0
				printf("X(%u), Y(%u)\n", (startX + (i << 1)), (startY + (j << 1)));
				printf("avg(%bu),val(%bu)\n", avg,val);
			#endif 
			}
			startX += 2;
			off += 2;
		}
		startY += 2;
		startX -= planeWidthW;
	}
}


/******************************************
 * @fn        void bpRawShift(UINT32 srcAddr, srcAddr,UINT32 dstAddr, UINT32 dmaOff)
 * @brief        bpRawShift  : shift the RAW data by one byte
 * @param     [in] srcAddr
 * @param     [in] dstAddr
 * @param     [in] dmaOff
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-10-09 , 11:06:04
 * @todo        N/A
 * @bug         N/A
*******************************************/
static void bpRawShift(UINT32 srcAddr,UINT32 dstAddr, UINT32 dmaSize)
{
	HAL_GprmDramDma((srcAddr<<1) , (dstAddr<<1) + 1 , dmaSize, 1);
	
	//sp1kDramFileSave("SHIF1111RAW",dstAddr,dmaSize);
}

#else
//-----------------------------------------------------------------------------
//bpStripRead
//-----------------------------------------------------------------------------
/**
 * @brief		read specified strip of raw data
 * @param	srcAddr: raw data source address
 			dmaOff: the offset between every two strip of raw data
 * @retval	none
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-19
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpStripRead(UINT32 srcAddr, UINT32 dmaOff)
{
	//dma read to sram, stripW < 1024
	//r gr r gr ... 128 pixles
	//r gr r gr ...
	
	UINT8 stripH = g_stripHeight;
	UINT16 dstAddr = 0x0000;

	while (stripH--) {
		HAL_DmaSetSramAddr(dstAddr);
		HAL_DmaSetDramAddr(srcAddr);
		HAL_DmaDo(0, 1, g_stripWidth, 0, 0);
		srcAddr += dmaOff;
		dstAddr += g_stripWidth;
	}
}


//-----------------------------------------------------------------------------
//bpStripAvgs
//-----------------------------------------------------------------------------
/**
 * @brief		caculate the avgs for specified plane
 * @param	pAvg: pointer target avgs array
 			cnt: the count of plane
 * @retval	none
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-19
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpStripAvgs(UINT8* pAvg, UINT8 cnt)
{
	UINT16 off;
	UINT8 i, j, k,t;
	UINT16 avg1, avg2;
	UINT8 start, width;
	UINT16 t_val;
	bpPara_t *pPara;

	bpCfgGet(&pPara);
	
	start = 0;
	width = g_planeWidth << 1;
	for (k = 0; k < cnt;) {
		off = (UINT16) start;
		avg1 = 0;
		avg2 = 0;
		j = g_planeHeight;
		do {
			i = g_planeWidth;
			do {
				avg1 += G_ucStorData[off++];
				avg2 += G_ucStorData[off++];
			} while (--i);
		} while (--j);
		if(pPara->preAvg){
			t=k;
			pAvg[k++] = (UINT8) (((avg1 / (g_planeHeight * g_planeWidth))+pPara->preAvg*63)>>6);
			t_val = pAvg[t];
			t=k;
			pAvg[k++] = (UINT8) (((avg2 / (g_planeHeight * g_planeWidth))+pPara->preAvg*63)>>6);
			t_val += pAvg[t];
		}else{
			t=k;
			pAvg[k++] = (UINT8) (avg1 / (g_planeHeight * g_planeWidth));
			t_val = pAvg[t];
			t=k;				
			pAvg[k++] = (UINT8) (avg2 / (g_planeHeight * g_planeWidth));
			t_val += pAvg[t];
		}
		pPara->preAvg = t_val/2;
		start += width;
	}
}



/**
 * @fn        UINT16 bpTypeGet(UINT16 startX, startX,UINT16 off)
 * @brief     bpTypeGet
 * @param     [in] startX
 * @param     [in] off
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
static UINT16 bpTypeGet(UINT16 startX,UINT16 off,UINT8 mode)
{
	UINT16 type;
	if(mode)
	{
		if((startX==0)&&(off % g_stripWidth < 2))
		{
			type = BP_TYPE_RIGHT;
		}
		else
		{
			type = BP_TYPE_LEFT;
		}

	}
	else
	{
		type = BP_TYPE_LEFT;
	}

	return type;
}






//-----------------------------------------------------------------------------
//bpStripSearch
//-----------------------------------------------------------------------------
/**
 * @brief		search the badpixel in specified plane
 * @param	startX: the horizontal offset of the raw image
 			startY: the vertical offset of raw image
 			avgBuf: point to the avg buffer
 * @retval	none
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-19
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpStripSearch(UINT16 startX, UINT16 startY, UINT8* avgBuf,UINT8 mode)
{
	UINT16 off;
	UINT8 val, diff, avg1, avg2;
	UINT8 cnt;
	UINT8 i, j, k;
	UINT16 type;
	cnt = g_stripWidth / g_planeWidth;
	bpStripAvgs(avgBuf, cnt);

	off = 0;
	for (k = 0; k < g_stripHeight; k ++) {
		for (j = 0; j < cnt;) {
			avg1 = (UINT8) (avgBuf[j++]);
			avg2 = (UINT8) (avgBuf[j++]);
			for (i = 0; i < g_planeWidth; i ++) {
				val = G_ucStorData[off++];
				diff = ABS(val, avg1); 
				if (diff > g_threshold) {
					bpBadPixelFlagSave(0x01, startX + (j - 2) * g_planeWidth + (i << 1), (startY + (k << 1)), s_bpBadpixelFlagAddr);
					type=bpTypeGet(startX,off-1,mode);
					bpBadPixelAddrSet(s_bpBadpixelAddr, 0);	
					bpBadPixelSave(startX + (j - 2) * g_planeWidth + (i << 1), (startY + (k << 1)));
					
					s_bpBadpixelAddr +=2;
#if DBG_BP_EN
					printf("X(%u), Y(%u)\n", startX + (j-2)*g_planeWidth + (i<<1), (startY + (k << 1)));
#endif //DBG_BP_EN
				}
				//else {
				//	bpBadPixelFlagSave(0x00,startX + (j - 2) * g_planeWidth + (i << 1), (startY + (k << 1)) ,s_bpBadpixelFlagAddr);
				//}
				
				val = G_ucStorData[off++];
				diff = ABS(val, avg2); 
				if (diff > g_threshold) {
					bpBadPixelFlagSave(0x01,startX + (j - 2) * g_planeWidth + (i << 1) + 1, (startY + (k << 1)) ,s_bpBadpixelFlagAddr);
					type=bpTypeGet(startX,off-1,mode);
					bpBadPixelAddrSet(s_bpBadpixelAddr, 0);					
					bpBadPixelSave(startX + (j - 2) * g_planeWidth + (i << 1) + 1, (startY + (k << 1)));		
					s_bpBadpixelAddr +=2;
#if DBG_BP_EN					
					printf("X(%u), Y(%u)\n", startX + (j-2)*g_planeWidth + (i<<1) + 1, (startY + (k << 1)));					
#endif //DBG_BP_EN				
				}
				
				//else {
				//	bpBadPixelFlagSave(0x00,startX + (j - 2) * g_planeWidth + (i << 1) + 1, (startY + (k << 1)) ,s_bpBadpixelFlagAddr);
				//}
			}
		}
	}
}
#endif

#if 0
#include "sp1k_bp_api.h"

extern/*static */UINT8 appCalBpCapDataArrange(void);
extern UINT8 appCalBpPro(void);

#define RAW_X_SIZE 2272
#define RAW_Y_SIZE 1704

#define CAL_BP_BAD_BUF K_SDRAM_CommonEndAddr
#define CAL_BP_RAW_BUF (CAL_BP_BAD_BUF + K_SDRAM_BadPixelCoordBufSize)


  static bpPara_t code sBpPara = {
	 1280, /* preview HD raw image width */
	 720,  /* preview HD raw image height */
 
	 560,//640,  /* preview raw image width */
	 420,//480,  /* preview raw image width */
	 
	 2272,//2592, /* capture raw image width */
	 1704,//1944, /* capture raw image height */
 
	 18,//20,//20, /* threshold */
	 10,//9,//30, /* black badPixel threshold */
 
	 BADPIXEL_PV_NUM_MAX,//4096, /* preview badpixel max number */
	 BADPIXEL_PV_NUM_MAX,//4096, /* preview badpixel max number */
	 BADPIXEL_CAP_NUM_MAX -4,//8000, /* capture badpixel max number */
	 0,
	 0,
 
	 0x55aa, /* EOF */
 };

void bpMoeTest(void)
{
	UINT32 rawSize,strTime,endTime;
	UINT8 ret;
	UINT32 badTotal;
	
	UINT8 sts;

	UINT32 rawAddr = (UINT32)CAL_BP_RAW_BUF;
	UINT32 badAddr = (UINT32)CAL_BP_BAD_BUF;

	dbgWdtSet(0);
	sp1kAeSetModeEn(0);
	sp1kAwbDisable();

	ENTER_CRITICAL(sts);
	HAL_GlobalReadGTimer(&strTime);

	/*read RAW*/
	ret = sp1kDramFileLoad(/*"SUNPBLAKRAW SUNPWITERAW"*/"SUNPWITERAW", rawAddr, &rawSize);
	if(ret != TRUE){
		printf("SUNPBLAKRAW not found!\n");
		return;
	}
	printf("SUNPBLAKRAW  found!\n");
	
	sp1kBpCfg(&sBpPara);
	g_threshold = 10;
	
	g_bpNumMaxCap = g_bpNumMaxPrv_HD = g_bpNumMaxPrv= 8000;
	printf("g_badPixelCnt1 = %lu\n",g_badPixelCnt);
	sp1kBpFind(2, rawAddr, RAW_X_SIZE, RAW_Y_SIZE, badAddr, &badTotal);
	HAL_GlobalReadGTimer(&endTime);
	printf("All time : %lu\n",endTime-strTime);
	
	EXIT_CRITICAL(sts);//wangjg++ disable ie when set moe reg, otherwise reg maybe error
}
#endif

#if BP_USE_MOE
//-----------------------------------------------------------------------------
//bpFind
//-----------------------------------------------------------------------------
/**
 * @brief		badpixel find process
 * @param	mode: 0:preview mode, 1: capture mode
 			rawAddr: raw image source address(word)
 			rawWidth: the width of raw image
 			rawHeight: the height of raw image
 			badPixAddr: badpixel description data destination address
 			badPixNum: badpixel total number
 * @retval	0: success
 			1: fail
 * @see		see
 * @author	Phil Lin
 * @since		2008-11-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 
bpFind(
	bpMode_t mode,
	UINT32 rawAddr,
	UINT16 rawWidth,
	UINT16 rawHeight,
	UINT32 badPixAddr,
	UINT32* badPixNum
)
{
	UINT8 ret;
	UINT16 startX, startY;
	//UINT16 g_stripWidthSv,g_stripHeightSv;   //add 20091124
	
	UINT8 g_planeWidthSv,g_planeHeightSv;   //add 20091124
	UINT32 rsvSize;
	UINT32 tagSize;
	resBpCb_t* pResBp;
	UINT32 resAddr, bpAddrtemp;//bpFlagAddrtemp;
	static UINT8 preMode_HD =0;  //add 20110309
	static UINT8 preMode=0; //add 20091124
	static UINT8 capMode=0; //add 20091124

	UINT16 moeRoiSize,moeRoiSizeSv;
	UINT8 blkAvg;
	UINT8 badFlag,i;
	UINT32 bpRawAddr;
	UINT32 allBlk;
	//UINT32 badBlk;
	UINT8 sts , saveResFlag;
	UINT16 rawHeight_half = rawHeight >> 1;
	//UINT32 strTime,endTime;
	
	saveResFlag = 0;
	allBlk = 0;
	//badBlk = 0;
	testSaveFlag = 0;
	printf("\033[32mrawAddr = %#lx , badPixAddr = %#lx\033[0m\n",rawAddr , badPixAddr);

	ENTER_CRITICAL(sts);//wangjg++ disable ie when set moe reg, otherwise reg maybe error
	if ( mode >= BP_MODE_MAX ) {
		return FAIL;
	} else if ( mode == BP_MODE_PRV_HD) {
		gp_bpNumMax = &g_bpNumMaxPrv_HD;
	} else if ( mode == BP_MODE_PRV_VGA){
		gp_bpNumMax = &g_bpNumMaxPrv;
	} else {
		gp_bpNumMax = &g_bpNumMaxCap;
	}
		
	moeRoiSize = (UINT16)g_planeHeight*g_planeWidth;
	moeRoiSizeSv = moeRoiSize;

	g_planeWidthSv = g_planeWidth;
	g_planeHeightSv = g_planeHeight;
	
	/* buffer init */
	//HAL_GprmDramFill(badPixAddr, K_SDRAM_BadPixel_CapBufSize << 1, 0xff);
	s_bpRawWidth = rawWidth;
	s_bpRawHeight = rawHeight;
	s_bpBadpixelAddr = badPixAddr;
	s_bpRawSize =(UINT32) rawWidth *rawHeight;

	bpRawAddr = rawAddr;
	
	g_bpMoeRawBlk1Addr = rawAddr + (s_bpRawSize>>1);
	g_bpMoeRawBlk2Addr = g_bpMoeRawBlk1Addr + ((UINT32)g_planeWidth*rawHeight);
	g_bpMoeQueueAddr = g_bpMoeRawBlk2Addr + (((UINT32)g_planeWidth*rawHeight)>>1);
	
	g_bpMoeRawAddr = g_bpMoeRawBlk1Addr;
	//s_bpBadpixelFlagAddr = rawAddr + (s_bpRawSize>>1);
	s_bpBadpixelFlagAddr = g_bpMoeQueueAddr + 28;  /* MOE queue : 24 word (2 queue) + MOE reslut buffer : 4 word = 28 word*/

	//printf("g_bpMoeRawAddr=%#lx,g_bpMoeTmpAddr=%#lx,s_bpBadpixelFlagAddr=%#lx\n",g_bpMoeRawAddr,g_bpMoeTmpAddr,s_bpBadpixelFlagAddr);
	//bpFlagAddrtemp = s_bpBadpixelFlagAddr; /*save bpflag address*/
	bpAddrtemp = s_bpBadpixelAddr;
	//HAL_DramStartAddrSet(badPixAddr, 0);
		
	s_bpRawSize >>=3; /*every bit stand for one pixel*/
	//printf("s_bpRawSize=%lu\n",s_bpRawSize);
	
	#if 0  /*it need 4500ms , when RAW size is 5M , GPRM fill function only need 15ms*/   
	BP_DRAM_SetStartAddress(s_bpBadpixelFlagAddr,0);
	while (s_bpRawSize--) {		
		BP_DRAM_WriteWord(0,0);
	}	
	#else
	//HAL_GlobalReadGTimer(&strTime);
	HAL_GprmDramFill(s_bpBadpixelFlagAddr,s_bpRawSize,0x00);
	//HAL_GlobalReadGTimer(&endTime);
	#endif
	
	//sp1kDramFileSave("FLAGYYYYBIN",s_bpBadpixelFlagAddr,s_bpRawSize);
	//printf("bpaddr0=%lx\n",badPixAddr);

	g_bpMoeResAddr = bpMoeInit(g_bpMoeRawAddr,g_bpMoeQueueAddr,moeRoiSize);
	
	//printf("\033[32mg_bpMoeRawAddr1 = %#lx , g_bpMoeRawAddr2 = %#lx\033[0m\n",g_bpMoeRawBlk1Addr,g_bpMoeRawBlk2Addr);

	for(i = 0 ; i < 2 ; i++){
		UINT8 offX;
		switch(i){
			case 0:			
				offX = 1;
				bpRawAddr = rawAddr;
			break;			
			case 1:
				offX = 0;
				bpRawShift(rawAddr , rawAddr -1,(UINT32) rawWidth *rawHeight);
				bpRawAddr = rawAddr -1;
			break;
		}
		
		g_bpPreBlkAvg = g_bpPreBlkAvg1 = g_bpPreBlkAvg2 = 0;
		for (startX= 0 ; startX < rawWidth; startX += (g_planeWidth<<1)) {
			//printf("\033[34mstartX = %u\033[0m\n",startX);
			g_planeWidth = g_planeWidthSv;
			g_planeHeight = g_planeHeightSv;
			moeRoiSize = moeRoiSizeSv;	
			g_bpMoeRawAddr = g_bpMoeRawBlk1Addr;
			g_bpPreBlkAvg = g_bpPreBlkAvg1;	/*restore block 1 Avg*/
			testSaveFlag = 0;
			if(rawWidth - startX < (g_planeWidth<<1)){
				g_planeWidth = (rawWidth - startX)>>1;
				moeRoiSize = (UINT16)g_planeWidth* g_planeHeight;
			}
			/*if( i == 1 && startX == (UINT16)2576 ){
				testSaveFlag = 1;
				printf("moeRoiSize = %u\n",moeRoiSize);
			}*/
			bpBlockMoeRead(bpRawAddr,
				rawWidth>>1 , rawHeight,
				startX>>1 , 0,
				g_bpMoeRawBlk1Addr , g_bpMoeRawBlk2Addr,
				g_planeWidth , rawHeight);
				
			/*1st block*/
			for(startY = 0 ; startY < rawHeight_half; startY += g_planeHeight){
				//printf("\033[32mstartY = %u\033[0m\n",startY);
				if(rawHeight_half < g_planeHeight + startY){
					g_planeHeight = rawHeight_half - startY;
					moeRoiSize = (UINT16)g_planeWidth* g_planeHeight;
					//printf("g_planeHeight1 = %bu\n",g_planeHeight);
				}
				g_bpMoeRoiWidth = moeRoiSize;
	
				allBlk ++;
				badFlag = bpBlockMoeSearch(moeRoiSize,&blkAvg);
				if(badFlag){
					//badBlk++;
					//printf("\033[32m+\033[0m\n");
					if(g_badPixelCnt < *gp_bpNumMax){
						bpBlockRead(g_bpMoeRawAddr, (moeRoiSize<<1));	
						bpBlockSearch(startX+offX, (startY<<1), blkAvg);
					}
				}
				g_bpMoeRawAddr += moeRoiSize;
				
				//printf("moeRoiSize = %u\n",moeRoiSize);
				//printf("g_bpMoeRawAddr1 = %#lx\n",g_bpMoeRawAddr);
			}
			
			/*2nd block*/
			g_planeHeight = g_planeHeightSv;
			moeRoiSize = (UINT16)g_planeWidth* g_planeHeight;
			g_bpMoeRawAddr = g_bpMoeRawBlk2Addr;
			g_bpPreBlkAvg1 = g_bpPreBlkAvg;	/*Save block 1 Avg*/
			g_bpPreBlkAvg = g_bpPreBlkAvg2;	/*restore block 2 Avg*/
			
			for(startY = 0 ; startY < rawHeight_half; startY += g_planeHeight){
				//printf("\033[33mstartY1 = %u\033[0m\n",startY);
				if(rawHeight_half < g_planeHeight + startY){
					g_planeHeight = rawHeight_half - startY;
					moeRoiSize = (UINT16)g_planeWidth* g_planeHeight;
					//printf("g_planeHeight2 = %bu\n",g_planeHeight);
					/*if( i == 1 && startX == (UINT16)2576 ){
						//testSaveFlag = 1;
						sp1kDramFileSave("LASTBKKKRAW",g_bpMoeRawAddr,(moeRoiSize<<1));
						printf("moeRoiSize = %u\n",moeRoiSize);
					}*/
				}
				g_bpMoeRoiWidth = moeRoiSize;
				allBlk ++;
				badFlag = bpBlockMoeSearch(moeRoiSize,&blkAvg);
				if(badFlag){
					//badBlk++;				
					//printf("\033[32m+\033[0m\n");
					if(g_badPixelCnt < *gp_bpNumMax){
						bpBlockRead(g_bpMoeRawAddr, (moeRoiSize<<1));	
						bpBlockSearch(startX+offX, (startY<<1)+1, blkAvg);
					}
				}
				g_bpMoeRawAddr +=  moeRoiSize;
				//printf("moeRoiSize = %u\n",moeRoiSize);
				//printf("g_bpMoeRawAddr2 = %#lx\n",g_bpMoeRawAddr);
			}
			g_bpPreBlkAvg2 = g_bpPreBlkAvg;   	/*Save block 2 Avg*/
		}
	}
	printf("allBlk = %lu , g_badPixelCnt = %lu\n",allBlk,g_badPixelCnt);
	
	//return 1;
//IMG_END:  //modify for bad pixel the last team small than sixteen
	
	//printf("Bad total:%lu\n", g_badPixelCnt);
	if (g_badPixelCnt >= *gp_bpNumMax) {
		__ASSERT(__FILE, __LINE, 1);
		//printf("There're too much badpixel!\n");
	}
	
	/* save EOF (It had already reserved memory for EOF saving in function bpCfg()) */
	g_badPixelCnt--; /* for EOF write back */
	bpBadPixelAddrSet(s_bpBadpixelAddr, 0);	
{
	UINT16 eof = 0xffff;
	bpBadPixelSave(eof, eof);
}

	/*recover address*/
	//s_bpBadpixelFlagAddr = bpFlagAddrtemp;
	s_bpBadpixelAddr = bpAddrtemp;
	
	/* return the badpixel total number */
	*badPixNum = g_badPixelCnt;
	
	/* 4 bytes/pixel */
	rsvSize = (g_badPixelCnt << 2) + 4;
		
	/*L->U->R->D algorithm*/
	bpBadPixelDirSet(badPixAddr);

	/* bp data in preview mode should be sorting and convert to specified format */
    if (mode == BP_MODE_PRV_HD) {
		resAddr = badPixAddr + (rsvSize >> 1);	
		
		//suny add for preview bad pixel 
		if(preMode_HD ==0){
			preMode_HD = 1;
			dramFileSave("BPPREV01BIN", badPixAddr, rsvSize);
		}else if(preMode_HD==1){
			UINT16 rawX,rawY;
			UINT32 resAddr_temp=badPixAddr;
			UINT32 fstPreBpSize=0;

			preMode_HD =0;
			saveResFlag = 1;
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPPREV02BIN", badPixAddr, rsvSize);
			while(1){
				if(g_badPixelCnt==0)break;
				BP_DRAM_SetStartAddress(resAddr_temp, 1);
				BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
				BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);	
				if(rawX== 0xffff || rawY == 0xffff){
					break;
				}
				resAddr_temp += 2; 
			}
			dramFileLoad("BPPREV01BIN", resAddr_temp, &fstPreBpSize);
			rsvSize += (fstPreBpSize-4);
			dramFileSave("BPPREV03BIN", badPixAddr, rsvSize);
			resAddr = badPixAddr + (rsvSize >> 1);	
		}
		//suny add for preview bad pixel 
		
#if DBG_BP_EN
		dramFileSave("SRC00000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		bpDataSortProc(badPixAddr, resAddr, rawWidth, rawHeight);
#if DBG_BP_EN		
		dramFileSave("TEMP0000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		
		tagSize = bpFmt_Cvt(badPixAddr, resAddr);
		
		resAddr = resAddr << 1;
	}
	else if ( mode == BP_MODE_PRV_VGA ) {
		resAddr = badPixAddr + (rsvSize >> 1);	
		
		//suny add for preview bad pixel 
		if(preMode ==0){
			preMode = 1;
			dramFileSave("BPPREV04BIN", badPixAddr, rsvSize);
		}else if(preMode ==1){
			UINT16 rawX,rawY;
			UINT32 resAddr_temp=badPixAddr;
			UINT32 fstPreBpSize=0;

			preMode =0;
			saveResFlag = 1;
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPPREV05BIN", badPixAddr, rsvSize);
			while(1){
				if(g_badPixelCnt==0)break;
				BP_DRAM_SetStartAddress(resAddr_temp, 1);
				BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
				BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);	
				if(rawX== 0xffff || rawY == 0xffff){
					break;
				}
				resAddr_temp += 2; 
			}
			dramFileLoad("BPPREV04BIN", resAddr_temp, &fstPreBpSize);
			rsvSize += (fstPreBpSize-4);
			dramFileSave("BPPREV06BIN", badPixAddr, rsvSize);
			resAddr = badPixAddr + (rsvSize >> 1);	
		}
		//suny add for preview bad pixel 
		
#if DBG_BP_EN
		dramFileSave("SRC00000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		bpDataSortProc(badPixAddr, resAddr, rawWidth, rawHeight);
#if DBG_BP_EN		
		dramFileSave("TEMP0000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		
		tagSize = bpFmt_Cvt(badPixAddr, resAddr);
		
		resAddr = resAddr << 1;
	} 
	else {
		tagSize = rsvSize;
		resAddr = badPixAddr << 1;
		/*add for use only one resource file to make capture bad pixel*/
		if(capMode==0){
			printf("S C 0\n");
			dramFileSave("BPCAP000BIN",badPixAddr,rsvSize);
			capMode=1;
			}
		else{
			printf("S C 1\n");
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPCAP001BIN",badPixAddr,rsvSize);
			capMode=0;
			}
			/*add for use only one resource file to make capture bad pixel*/
	}

	bpResInfoGet(mode, &pResBp);
	
#if DBG_BP_EN
	printf("pResBp->resId: %lx\n", (UINT32)pResBp->resId);
	dramFileSave("TAG00000BIN", badPixAddr, tagSize);
#endif /* DBG_BP_EN */

	/* check the badpixel description data size */
	sp1kDiskRsvSizeGet(pResBp->resId, NULL, &rsvSize);
	if(mode == BP_MODE_CAP){
		rsvSize = rsvSize<<1;
	}
	if ( tagSize >= rsvSize ) {
		__ASSERT(__FILE, __LINE, 1);
	}

	/* save the target resource file to disk */
	//printf("saveId=%u\n",pResBp->resId);
	if(pResBp->resId==0x05)
	{
		dramFileSave("RESID005BIN", resAddr>>1, 0x4000);

	}
	
	ret = TRUE;

	if(saveResFlag){
		//printf("resId: %#lx ,rsvSize : %#lx\n", (UINT32)pResBp->resId , rsvSize);
		ret = sp1kDiskRsvWrite(pResBp->resId, resAddr);
	}
	
	EXIT_CRITICAL(sts);
	if ( ret != TRUE ) {
		return FAIL;
	} else {
		return SUCCESS;
	}
}
#else
//-----------------------------------------------------------------------------
//bpFind
//-----------------------------------------------------------------------------
/**
 * @brief		badpixel find process
 * @param	mode: 0:preview mode, 1: capture mode
 			rawAddr: raw image source address(word)
 			rawWidth: the width of raw image
 			rawHeight: the height of raw image
 			badPixAddr: badpixel description data destination address
 			badPixNum: badpixel total number
 * @retval	0: success
 			1: fail
 * @see		see
 * @author	Phil Lin
 * @since		2008-11-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 
bpFind(
	bpMode_t mode,
	UINT32 rawAddr,
	UINT16 rawWidth,
	UINT16 rawHeight,
	UINT32 badPixAddr,
	UINT32* badPixNum
)
{
	UINT8 ret;
	UINT16 startX, startY, offY;
	UINT32 rawOff1, rawOff2, rawOff1Sv,rawOff2Sv;  //add 20091124
	UINT8 avgBuf[16];   //1024/(8*8)=16
	UINT16 g_stripWidthSv,g_stripHeightSv;   //add 20091124
	UINT32 rsvSize;
	UINT32 tagSize;
	resBpCb_t* pResBp;
	UINT32 resAddr, bpAddrtemp;//bpFlagAddrtemp;
	static UINT8 preMode_HD =0;  //add 20110309
	static UINT8 preMode=0; //add 20091124
	static UINT8 capMode=0; //add 20091124

	UINT8 saveResFlag;
	
	saveResFlag = 0;
		
	if ( mode >= BP_MODE_MAX ) {
		return FAIL;
	} else if ( mode == BP_MODE_PRV_HD) {
		gp_bpNumMax = &g_bpNumMaxPrv_HD;
	} else if ( mode == BP_MODE_PRV_VGA){
		gp_bpNumMax = &g_bpNumMaxPrv;
	} else {
		gp_bpNumMax = &g_bpNumMaxCap;
	}
	
	rawOff1 = (UINT32) (g_stripWidth >> 1);
	rawOff1Sv = rawOff1;
	rawOff2 = rawWidth * (g_stripHeight - 1);
	rawOff2Sv=rawOff2;//add 20091124
	offY = (g_stripHeight << 1) - 1;

	/* buffer init */
	//HAL_GprmDramFill(badPixAddr, K_SDRAM_BadPixel_CapBufSize << 1, 0xff);
	s_bpRawWidth = rawWidth;
	s_bpRawHeight = rawHeight;
	s_bpBadpixelAddr = badPixAddr;
	s_bpRawSize =(UINT32) rawWidth *rawHeight;
	s_bpBadpixelFlagAddr = rawAddr + (s_bpRawSize>>1);
	//printf("rawaddr=%lx,flgaddr=%lx\n",rawAddr,s_bpBadpixelFlagAddr);
	//bpFlagAddrtemp = s_bpBadpixelFlagAddr; /*save bpflag address*/
	bpAddrtemp = s_bpBadpixelAddr;
	//HAL_DramStartAddrSet(badPixAddr, 0);
	g_stripWidthSv = g_stripWidth;
	g_stripHeightSv=g_stripHeight;//add 20091124
	s_bpRawSize >>=3; /*every bit stand for one pixel*/
	BP_DRAM_SetStartAddress(s_bpBadpixelFlagAddr,0);
	while (s_bpRawSize--) {		
		BP_DRAM_WriteWord(0,0);
	}	
	printf("bpaddr0=%lx\n",badPixAddr);
	for (startY = 0; startY < (rawHeight); /*startY += g_stripHeight*/) {
	//	putchar('.');
		for (startX = 0; startX < rawWidth; startX += g_stripWidth) {
			if (rawWidth - startX < g_stripWidth) {
				g_stripWidthSv = g_stripWidth;
				g_stripWidth = rawWidth - startX;
				rawOff1Sv = rawOff1;
				rawOff1 = (UINT32) (g_stripWidth >> 1);
			}
			if (rawHeight - startY < (g_stripHeight << 1)) {
				/*modify for bad pixel the last team small than sixteen*/
				g_stripHeightSv=g_stripHeight;
				g_stripHeight=((rawHeight-startY)>>1);
				//printf("y: %u, h: %bu\n", startY, g_stripHeight);
				//goto IMG_END;
				/*modify for bad pixel the last team small than sixteen*/
			}

			bpStripRead(rawAddr, rawWidth); //read 2 plane once 		   
			bpStripSearch(startX, startY, avgBuf,mode);

			rawAddr += rawOff1;
		}
		g_stripWidth = g_stripWidthSv;
		rawOff1 = rawOff1Sv;
		g_stripHeight=g_stripHeightSv; //modify for bad pixel the last team small than sixteen
		startY ++;
		for (startX = 0; startX < rawWidth; startX += g_stripWidth) {
			if (rawWidth - startX < g_stripWidth) {
				g_stripWidthSv = g_stripWidth;
				g_stripWidth = rawWidth - startX;
				rawOff1Sv = rawOff1;
				rawOff1 = (UINT32) (g_stripWidth >> 1);
			}
			if (rawHeight - startY < (g_stripHeight << 1)) {
				/*modify for bad pixel the last team small than sixteen*/
				g_stripHeightSv=g_stripHeight;
				g_stripHeight=((rawHeight-startY)>>1);
				rawOff2Sv=rawOff2;
				rawOff2= rawWidth*(g_stripHeight-1);
				//goto IMG_END;
				/*modify for bad pixel the last team small than sixteen*/
			}

			bpStripRead(rawAddr, rawWidth); //read 2 plane once 		   
			bpStripSearch(startX, startY, avgBuf,mode);

			rawAddr += rawOff1;
		}
		g_stripWidth = g_stripWidthSv;
		g_stripHeight=g_stripHeightSv; //modify for bad pixel the last team small than sixteen
		rawOff1 = rawOff1Sv;

		startY += offY;
		rawAddr += rawOff2;
		rawOff2=rawOff2Sv;//modify for bad pixel the last team small than sixteen
	}

//IMG_END:  //modify for bad pixel the last team small than sixteen
	
	//printf("Bad total:%lu\n", g_badPixelCnt);
	if (g_badPixelCnt >= *gp_bpNumMax) {
		__ASSERT(__FILE, __LINE, 1);
		//printf("There're too much badpixel!\n");
	}
	
	/* save EOF (It had already reserved memory for EOF saving in function bpCfg()) */
	g_badPixelCnt--; /* for EOF write back */
	bpBadPixelAddrSet(s_bpBadpixelAddr, 0);	
	bpBadPixelSave(0xffff, 0xffff);

	/*recover address*/
	//s_bpBadpixelFlagAddr = bpFlagAddrtemp;
	s_bpBadpixelAddr = bpAddrtemp;
	
	/* return the badpixel total number */
	*badPixNum = g_badPixelCnt;

	/* 4 bytes/pixel */
	rsvSize = (g_badPixelCnt << 2) + 4;
	
	/*L->U->R->D algorithm*/
	bpBadPixelDirSet(badPixAddr);

	/* bp data in preview mode should be sorting and convert to specified format */
    if (mode == BP_MODE_PRV_HD) {
		resAddr = badPixAddr + (rsvSize >> 1);	
		
		//suny add for preview bad pixel 
		if(preMode_HD ==0){
			preMode_HD = 1;
			dramFileSave("BPPREV01BIN", badPixAddr, rsvSize);
		}else if(preMode_HD==1){
			UINT16 rawX,rawY;
			UINT32 resAddr_temp=badPixAddr;
			UINT32 fstPreBpSize=0;

			saveResFlag =1;

			preMode_HD =0;
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPPREV02BIN", badPixAddr, rsvSize);
			while(1){
				if(g_badPixelCnt==0)break;
				BP_DRAM_SetStartAddress(resAddr_temp, 1);
				BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
				BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);	
				if(rawX== 0xffff || rawY == 0xffff){
					break;
				}
				resAddr_temp += 2; 
			}
			dramFileLoad("BPPREV01BIN", resAddr_temp, &fstPreBpSize);
			rsvSize += (fstPreBpSize-4);
			dramFileSave("BPPREV03BIN", badPixAddr, rsvSize);
			resAddr = badPixAddr + (rsvSize >> 1);	
		}
		//suny add for preview bad pixel 
		
#if DBG_BP_EN
		dramFileSave("SRC00000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		bpDataSortProc(badPixAddr, resAddr, rawWidth, rawHeight);
#if DBG_BP_EN		
		dramFileSave("TEMP0000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		
		tagSize = bpFmt_Cvt(badPixAddr, resAddr);
		
		resAddr = resAddr << 1;
	}
	else if ( mode == BP_MODE_PRV_VGA ) {
		resAddr = badPixAddr + (rsvSize >> 1);	
		
		//suny add for preview bad pixel 
		if(preMode ==0){
			preMode = 1;
			dramFileSave("BPPREV04BIN", badPixAddr, rsvSize);
		}else if(preMode ==1){
			UINT16 rawX,rawY;
			UINT32 resAddr_temp=badPixAddr;
			UINT32 fstPreBpSize=0;

			saveResFlag =1;

			preMode =0;
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPPREV05BIN", badPixAddr, rsvSize);
			while(1){
				if(g_badPixelCnt==0)break;
				BP_DRAM_SetStartAddress(resAddr_temp, 1);
				BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
				BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);	
				if(rawX== 0xffff || rawY == 0xffff){
					break;
				}
				resAddr_temp += 2; 
			}
			dramFileLoad("BPPREV04BIN", resAddr_temp, &fstPreBpSize);
			rsvSize += (fstPreBpSize-4);
			dramFileSave("BPPREV06BIN", badPixAddr, rsvSize);
			resAddr = badPixAddr + (rsvSize >> 1);	
		}
		//suny add for preview bad pixel 
		
#if DBG_BP_EN
		dramFileSave("SRC00000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		bpDataSortProc(badPixAddr, resAddr, rawWidth, rawHeight);
#if DBG_BP_EN		
		dramFileSave("TEMP0000BIN", badPixAddr, rsvSize);
#endif /* DBG_BP_EN */
		
		tagSize = bpFmt_Cvt(badPixAddr, resAddr);
		
		resAddr = resAddr << 1;
	} 
	else {
		tagSize = rsvSize;
		resAddr = badPixAddr << 1;
		/*add for use only one resource file to make capture bad pixel*/
		if(capMode==0){
			dramFileSave("BPCAP000BIN",badPixAddr,rsvSize);
			capMode=1;
			}
		else{
			//printf("bpaddr1=%lx\n",badPixAddr);
			dramFileSave("BPCAP001BIN",badPixAddr,rsvSize);
			capMode=0;
			}
			/*add for use only one resource file to make capture bad pixel*/
	}

	bpResInfoGet(mode, &pResBp);
	
#if DBG_BP_EN
	printf("pResBp->resId: %lx\n", (UINT32)pResBp->resId);
	dramFileSave("TAG00000BIN", badPixAddr, tagSize);
#endif /* DBG_BP_EN */

	/* check the badpixel description data size */
	sp1kDiskRsvSizeGet(pResBp->resId, NULL, &rsvSize);
	if(mode == BP_MODE_CAP){
		rsvSize = rsvSize<<1;
	}
	if ( tagSize >= rsvSize ) {
		__ASSERT(__FILE, __LINE, 1);
	}

	/* save the target resource file to disk */
	//printf("saveId=%u\n",pResBp->resId);
	if(pResBp->resId==0x05)
	{
		dramFileSave("RESID005BIN", resAddr>>1, 0x4000);

	}
	ret = TRUE;
	if(saveResFlag){
		ret = sp1kDiskRsvWrite(pResBp->resId, resAddr);
	}
	
	if ( ret != TRUE ) {
		return FAIL;
	} else {
		return SUCCESS;
	}
}
#endif
//-----------------------------------------------------------------------------
//bpDataSortProc
//-----------------------------------------------------------------------------
/**
 * @brief		sorting the bp data from small to large
 * @param	srcAddr: the badpixel data start address
 			tmpAddr: the temp buffer start address
 			rawW: the width of raw image 
 			rawH: the height of raw image
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-19
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 bpDataSortProc(UINT32 srcAddr, UINT32 tmpAddr, UINT16 rawW, UINT16 rawH)
{
	const UINT8 bInitVal = 0xfe;
	UINT32 tmpSize;

	tmpSize = ((UINT32) rawW * rawH) << 1;

	/* input param check */
	if (tmpAddr + tmpSize > K_SDRAM_TotalSize) {
		__ASSERT(__FILE, __LINE, 1);
		return FAIL;
	}

	/* buffer init */
	HAL_GprmDramFill(tmpAddr, tmpSize << 1, bInitVal);

	/* fill the badpixel data to specified address */
	bpDataSort_Fill(srcAddr, tmpAddr, rawW, rawH);
		
	/* badpixel data sort strip */
	bpDataSort_Strip(srcAddr, tmpAddr);
	
	/* finished */
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpDataSort_Fill
//-----------------------------------------------------------------------------
/**
 * @brief		fill the badpixel data to specified position
 * @param	srcAddr: the badpixel data start address
 			tmpAddr: the temp buffer start address
 			rawW: the width of raw image 
 			rawH: the height of raw image
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 bpDataSort_Fill(UINT32 srcAddr, UINT32 tmpAddr, UINT16 rawW, UINT16 rawH)
{
	const UINT8 bEof = 0xff;
	const UINT16 wEof = 0xffff;
	UINT32 dstAddr;
	UINT16 rawX, rawY;	
	
	/* badpixel position sorting */
	while (1) {
		/* source data read */
		BP_DRAM_SetStartAddress(srcAddr, 1);
		BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
		BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);
		srcAddr += 2;

		/* EOF */
		if (rawX == wEof || rawY == wEof) {
			/* write EOF to the destination buffer */
			dstAddr = tmpAddr + (((UINT32) rawW * rawH) << 1);
			BP_DRAM_SetStartAddress(dstAddr, 0);
			BP_DRAM_WriteWord(bEof, bEof);
			BP_DRAM_WriteWord(bEof, bEof);	
			break;
		}
		/*mark the highest 4 bit*/
		rawX &= 0x0fff;
		rawY &= 0x0fff;

		/* position caculate */
		dstAddr = tmpAddr + ((((UINT32) rawW * rawY) + (UINT32) rawX) << 1);
		
#if BP_ASSERT_EN
		if (dstAddr < tmpAddr || dstAddr> K_SDRAM_TotalSize) {
			/* address error */
			__ASSERT(__FILE, __LINE, 1);
			return FAIL;
		}
#endif //BP_ASSERT_EN		

		BP_DRAM_SetStartAddress(dstAddr, 0);

		/* temp data write */
		BP_DRAM_WriteWord(READ8(rawX, 0), READ8(rawX, 1));
		BP_DRAM_WriteWord(READ8(rawY, 0), READ8(rawY, 1));
	}

	return SUCCESS;
}

static void bpDataSort_Dma(UINT32 dstAddr , UINT32 dmaSize)
{
	HAL_DmaSetSramAddr(0x0000);  /*sec Addr*/
	HAL_DmaSetDramAddr(dstAddr);
	HAL_DmaDo(1, 0, dmaSize, 0, 0); /*SRAM -> DRAM*/
}
//-----------------------------------------------------------------------------
//bpDataSort_Strip
//-----------------------------------------------------------------------------
/**
 * @brief		fill the badpixel data to specified position
 * @param	srcAddr: the badpixel data start address
 			tmpAddr: the temp buffer start address
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#pragma OT(8, SPEED)
static UINT8 bpDataSort_Strip(UINT32 srcAddr, UINT32 tmpAddr)
{
	const UINT16 wInitVal = 0xfefe;
	const UINT16 wEof = 0xffff;
	UINT16 rawX, rawY;	
	
	UINT16 xdata* pSrcTmpBuff;
	UINT16 dmaCnt;
	UINT8 exitFlag;
	
	exitFlag = 0;
	
	/* shadow badpixel function to IMEM */
	HAL_CpuImemSdSet(LOG_BANK_16, 0xe000);
	
	HAL_DmaSetSramAddr(0x0000);  /*sec Addr*/
	
	BP_DRAM_SetStartAddress(tmpAddr, 1);
	while (1) {
		/* skip what is dummy data */
		dmaCnt = 0;
		pSrcTmpBuff = (UINT16 xdata*)G_ucStorData;
		while (1) {
			BP_DRAM_ReadPword((UINT8 *) &rawX + 0, (UINT8 *) &rawX + 1);
			BP_DRAM_ReadPword((UINT8 *) &rawY + 0, (UINT8 *) &rawY + 1);
			//tmpAddr += 2;
			//if (*((UINT16 xdata*)pSrcBuff) == wInitVal) continue;
			if(rawX == wInitVal) continue;
			if(rawX != wEof){
			//if(*((UINT16 xdata*)pSrcBuff) != wEof ){
				*(pSrcTmpBuff++) = rawX;
				*(pSrcTmpBuff++) = rawY;
				//pSrcBuff += 4;
				if((++dmaCnt) == (UINT16)256){/*256 * 4Byte = 1K*/
					break;
				}
			}
			else{				
				exitFlag = 1;
				break;
			}
			/* wInitVal == 0xfefe; */
			//}
		}
		
#if DBG_BP_EN		
		printf("rawX: %lx, rawY: %lx", (UINT32)rawX, (UINT32)rawY);
#endif //DBG_BP_EN

		#if 0
		/* EOF */
		if (rawX == wEof || rawY == wEof) {
			break;
		}
		/* write data back to srcAddr*/
		BP_DRAM_SetStartAddress(srcAddr, 0);
		BP_DRAM_WriteWord(READ8(rawX, 0), READ8(rawX, 1));
		BP_DRAM_WriteWord(READ8(rawY, 0), READ8(rawY, 1));		
		srcAddr += 2;
		#else
		
		//bpDataSort_Dma(srcAddr,(dmaCnt<<2));
		HAL_DmaSetDramAddr(srcAddr);
		HAL_DmaDo(1, 0, (dmaCnt<<2), 0, 0); /*SRAM -> DRAM*/
		 
		srcAddr += (UINT32)(dmaCnt<<1);
		if(exitFlag){
			break;
		}
		#endif
	}

	return SUCCESS;
}
   
//-----------------------------------------------------------------------------
//bpBadPixelFlagSave
//-----------------------------------------------------------------------------
/**
 * @brief		save the badpixel flag to sdram 
 *			one flag bit stand for one pixel
 * @param	flagVal: 0: good pixel
 * 				     1:bad pixel
 *			X,Y: the badpixel coordinate
 *                  bpflgAddr: badpixel flag start address
 * @retval	none
 * @see		none
 * @author	kai.wang
 * @since		2011-3-2
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpBadPixelFlagSave(UINT8 flagVal, UINT16 X, UINT16 Y,UINT32 bpflgAddr)
{
	//UINT8 a,b;
	//static UINT16 bpflg = 0;
	//static UINT32 pixelcnt;
	//UINT16 temp = 1;
	UINT32 idx,addrtemp;
	UINT16 offsetword, offsetbit, bpflagval;
#if 0
	if(flagVal) {
		bpflg |= temp;
		temp << i;		
	}	
	i++;
	pixelcnt ++;
	if ((i == 0x10) ||(pixelcnt== s_bpRawSize)) {
		BP_DRAM_SetStartAddress(*bpflgAddr,0);
		BP_DRAM_WriteWord(READ8(bpflg,0),READ8(bpflg,1));
		i = 0;
		bpflg = 0;
		(*bpflgAddr) ++;
	}
#endif	
	idx = (UINT32)s_bpRawWidth * Y + X;
	offsetword = idx >> 4;
	offsetbit = idx & 0x0f;	
	addrtemp = bpflgAddr + (UINT32)offsetword;
	
	BP_DRAM_SetStartAddress(addrtemp, 1);
	BP_DRAM_Readword(READ8(bpflagval, 1), READ8(bpflagval, 0));
	
	if ( flagVal ) {
		bpflagval |= (UINT16)(1 << offsetbit );
	}
	else {
		bpflagval &= ~((UINT16)(1 << offsetbit ));
	}
	
	BP_DRAM_SetStartAddress(addrtemp, 0);
	BP_DRAM_WriteWord(READ8(bpflagval, 1),READ8(bpflagval, 0));

}

//-----------------------------------------------------------------------------
//bpBadPixelFlagValGet
//-----------------------------------------------------------------------------
/**
 * @brief		get the badpixel flag value 
 *			one flag bit stand for one pixel
 * @param	X,Y: the badpixel coordinate
 * @retval	badpixel flag value
 * @see		none
 * @author	kai.wang
 * @since		2011-3-2
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 bpBadPixelFlagValGet(UINT16 X, UINT16 Y)
{
	UINT32 idx,addrtemp;
	UINT16 offsetword, offsetbit, bpflagval;
	UINT8 val;

	if ((X & 0x1000) || (Y & 0x1000)	|| (X>s_bpRawWidth) || (Y>s_bpRawHeight)) {
		//printf("X=%u,Y=%u,val=1\n",X,Y);
		return FAIL;
	}
	
	idx = s_bpRawWidth * Y + X;
	offsetword = idx /16;
	offsetbit = idx % 16;	
	addrtemp = (s_bpBadpixelFlagAddr+(UINT32)offsetword);
	BP_DRAM_SetStartAddress(addrtemp,1);
	BP_DRAM_ReadPword((UINT8 *) &bpflagval+ 1, (UINT8 *) &bpflagval + 0);

	if(bpflagval & ((UINT16)(1 << offsetbit ))) { /*bad pixel*/
		val = 0x01;
		//printf("fval=%x\n",bpflagval);
	}
	else { /*good pixel*/
		val = 0x00;
	}
//	printf("X=%u,Y=%u,fval=%x,val=%bx,addr2=%lx\n",X,Y,bpflagval,val,addrtemp);
	return val;
}

//-----------------------------------------------------------------------------
//bpBadPixelFlagValGet
//-----------------------------------------------------------------------------
/**
 * @brief		Set the badpixel direction 
 *			left -> up ->right -> down
 * @param	Badpixel start Address
 * @retval	none
 * @see		none
 * @author	kai.wang
 * @since		2011-3-2
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void bpBadPixelDirSet(UINT32 BadpixelAddr)
{
	UINT16 X,Y, i,j;
	UINT8 loopcnt ;
	UINT8 *pbuf = G_ucStorData;
	UINT8 xoff ;
	UINT8 yoff ;
	UINT8 dmacnt; 
	static UINT32 badpixelcnt = 0;
	
	dmacnt = ((g_badPixelCnt >>8)+1); /*256pixel every time dma*/ 
	printf("dmacnt=%bx,num=%lu\n",dmacnt,g_badPixelCnt);
	if(g_badPixelCnt==0){
		return;
	}
	while (dmacnt --) {
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(BadpixelAddr);
		HAL_DmaDo(0, 1, 1024, 0, 0); /*1024/4= 256 pixel */
		j = 0;
		for (i = 0; i < 256; i ++) { /* 256 pixel*/
			X = (UINT16) (pbuf[j+0]<<8) + pbuf[j+1] ;
			Y = (UINT16) (pbuf[j+2]<<8) + pbuf[j+3] ;		
			
			xoff  = 2;
			yoff = 2;
			loopcnt = 0;
			/*L->U->R->D*/
			if ((X != 0xFFFF || Y != 0xFFFF)&&(badpixelcnt <= g_badPixelCnt)) {	
				X &= 0x0fff;
				Y &= 0x0fff;	
				printf("x=%u,y=%u\n",X,Y);
			#if 1	
				if (bpBadPixelFlagValGet(X-xoff, Y)) {
					if (bpBadPixelFlagValGet(X, Y-yoff)){
						if (bpBadPixelFlagValGet(X+xoff, Y)){
							if (bpBadPixelFlagValGet(X, Y+yoff)){
								loopcnt ++;
								xoff += 2;
								yoff += 2;
							//	printf("p0=%bx\n",pbuf[j+2]);
							}
							else { 
								/*use down*/
								pbuf[j+2] |= BP_TYPE_DOWN>>8;
							//	printf("p1=%bx\n",pbuf[j+2]);
							}
						}
						else {
							/*use right*/	
							pbuf[j+2] |= BP_TYPE_RIGHT>>8;
				//			printf("p2=%bx\n",pbuf[j+2]);
						}
					}
					else{
						/*use up*/
						pbuf[j+2] |= BP_TYPE_UP>>8;
				//		printf("p3=%bx\n",pbuf[j+2]);
					}
				}
				else {
					/*use left */
					pbuf[j+2] |= BP_TYPE_LEFT>>8;
				//	printf("p4=%bx\n",pbuf[j+2]);
				}
				//printf("loopcnt=%bx\n",loopcnt);
				while ( (loopcnt < (s_bpRawWidth>>1))&& (loopcnt != 0)) {				
					if (bpBadPixelFlagValGet(X-xoff, Y)) {						
						if (bpBadPixelFlagValGet(X, Y-yoff)) {
							loopcnt ++;
							xoff += 2;
							yoff += 2;
							if (((X-xoff) & 0x1000) && ((Y-yoff) & 0x1000)) {/*when X,Y overflow*/
								/*default use left*/
							//	printf("p5=%bx\n",pbuf[j+2]);
								break;
							}								
						}						
						else { /*up end*/							
							/*use up end*/
							pbuf[j+2] |= BP_TYPE_UP>>8;
						//	printf("p6=%bx\n",pbuf[j+2]);
							break;							
						}
					}				
					else { /*left end*/ 					
						/*use left end*/
						pbuf[j+2] |= BP_TYPE_LEFT>>8;
					//	printf("p7=%bx\n",pbuf[j+2]);
						break;					
					}
				}
			#endif	
				j += 4; /*next pixel*/
			}
			badpixelcnt++;
		}
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(BadpixelAddr);
		HAL_DmaDo(1, 0, 1024, 0, 0); // sram to dram
		BadpixelAddr += 512;
	}
}

#endif /* MODULE_BP_OPT */