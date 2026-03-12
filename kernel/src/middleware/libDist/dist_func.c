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
 * @file	dist_func.c
 * @brief	
 * @author	  TQ
 * @since	2010-03-26
 * @date	2010-03-26
 */

 /* notify
	1. REG_Dram_ZfUpdate:   
	[0] update with cdsp sof and disp sof
	[1] update with disp sof
	[2] update with disp sof

 */
 
 /**************************************************************************
 *							  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "cmd.h"
#include "initio.h"
#include "hal_cdsp.h"
#include "hal.h"
#include "hal_dist.h"
#include "dist_func.h"
#include "hal_disp.h"
#include "pv_task.h"

#include "sp1k_front_api.h"
#include "sp1k_dist_api.h"

#include "hal_dram.h"
#include "disp_api.h"
#include "dist_debug.h"
#include "utility.h"
#include "asicreg.h"
#include "reg_def.h"

#ifdef DIST_FUNC_EN

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define DIST_PRINT_LOOP 1

#define DIST_BLOCK_NUM_X	12
#define DIST_BLOCK_NUM_Y	12
#define DIST_BLOCK_NUM (DIST_BLOCK_NUM_X*DIST_BLOCK_NUM_Y)

#define ME_OUT_DEF	32

#define MAX_MV_NUM  70ul

//#define distHALFmargin					((UINT32)64)

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define distGetTimer(t) HAL_GlobalTimerRead10us(t)

#define abs(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x)) 
#define absValue(x) ( (x) > 0 ? (x) : -(x) )
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/

UINT16	DistBlockNumber;
UINT8	distMode;
UINT32 	distMeOutAddr;
UINT8	distMvClampX=32,distMvClampY=24;

UINT8 xdata gDramMvx, gDramMvy;
UINT8 xdata gDramMvxDisp, gDramMvyDisp;

UINT32 distMissCnt=0;

UINT8 distFac=32;
UINT8 distDebugCnt=0;

UINT8 xdata  gDistGmvDo;

static UINT16 accGMvx,accGMvy;

static UINT8 hisGMvX[10],hisGMvY[10];
static UINT8 distCnt,distIdxCnt;
static UINT8 avgGMvX,avgGMvY;
static UINT16 sumGMvX,sumGMvY;
static UINT8 preGMvX,preGMvY;
static UINT8 stableNumMvX,stableNumMvY;
static SINT16 totalMvX,totalMvY;
static UINT8 movingFlagX,movingFlagY;
//UINT8 xdata gDistGmvDo=0;
//UINT8 xdata old_dram_mvx=distHALFmargin, old_dram_mvy=distHALFmargin;

//UINT8 distFlag;

//UINT32		MEOUTADDR	=0x302000;//Notice overlap???
//UINT32		MEOUTADDR	= K_SDRAM_Pv2BufAddrD+(UINT32)2048+(UINT32)320*240;

//static UINT8 saveMECnt;


/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static void maxMvFind(UINT8* MvPt0,UINT8	BlockNum0,UINT8 thrhold,UINT8*	MaxMv);
static UINT8 sp1kDistProcSet(UINT8 proc);


/**
 * @fn		  void maxMvFind(UINT8* MvPt0, UINT8 BlockNum0, UINT8 thrhold, UINT8* MaxMv)
 * @brief	  find Dist Max Mv from Mv array
 * @param	  [in] MvPt0 : input Mv array
 * @param	  [in] BlockNum0 : constrain the max Mv value
 * @param	  [in] thrhold : threshold of Max Mv block number
 * @param	  [out] MaxMv : Output Max Mv value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
#pragma OT(8,SPEED) // optimize with speed first
void
maxMvFind(
	UINT8*	MvPt0,
	UINT8	BlockNum0,
	UINT8	thrhold,
	UINT8*	MaxMv
)
{
	data UINT8	 Mv1stMax,Mv2ndMax;
	data UINT8	 num_mv0,num_mv1,num_mv2;
	data UINT8	 sum_num_mv1stMax=0,sum_num_mv2stMax=0,sum_num_mvCur;
	data UINT8	 i;
	data UINT8 BlockNum;
	data UINT8* MvPt;

	MvPt = MvPt0;
	BlockNum =BlockNum0;

	i=1;
	num_mv0 = *MvPt++;
	num_mv1 = *MvPt++;
	num_mv2 = *MvPt++;
	do
	{
		//if(sldebug) printf("i=%02bd histmv=%02bd\n",i,histmv[i]);
		sum_num_mvCur	 = num_mv0+num_mv1+num_mv2;    
		if ((num_mv1>=num_mv2) && (num_mv1>=num_mv0) && (sum_num_mvCur>sum_num_mv2stMax)){
			if(sum_num_mv1stMax<sum_num_mvCur){
				sum_num_mv1stMax = sum_num_mvCur;
				if (Mv1stMax!=i){
					Mv2ndMax  = Mv1stMax;	/* back up the second max Mv */
					Mv1stMax  = i;			/* record the max Mv(updated the max MV) */
				}
			}
			else if((i-Mv1stMax)>1){
				sum_num_mv2stMax= sum_num_mvCur;
				Mv2ndMax = i;	/* the max Mv needn't update, but the second max Mv need update	*/		
			}
		}
		num_mv0 = num_mv1;
		num_mv1 = num_mv2;
		num_mv2 = *MvPt++;
	}
	
	while(++i<BlockNum);	
	//printf("New 1stMax=%bx,2ndMax=%bx,1stSum=%x,2ndSum=%x\n",Mv1stMax,Mv2ndMax,sum_num_mv1stMax,sum_num_mv2stMax);
	*MaxMv = Mv1stMax-1;/*because input is (mv+1)*/
	if((sum_num_mv1stMax==sum_num_mv2stMax)&&(MvPt[Mv2ndMax]>MvPt[Mv1stMax])){
		*MaxMv = Mv2ndMax-1;
	}
	
	if(sum_num_mv1stMax<thrhold){
		*MaxMv = 32;
	}
	
}

/**
 * @fn		  void gMvGet(UINT16 Number0, UINT8 blknum_threshold, UINT8 *Gmvx, UINT8 *Gmvy)
 * @brief	  Get X&Y Max Mv value
 * @param	  [in] Number0 : total block number
 * @param	  [in] blknum_threshold : threshold of Max Mv block number
 * @param	  [out] Gmvx : output Max Mv x
 * @param	  [out] Gmvy : output Max Mv y
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
 * @notify vecter val range 0~31, MAX_MV_NUM = 70
*/
#pragma OT(8,SPEED) // optimize with speed first
void 
gMvGet(
	UINT16 Number0, 
	UINT8 blknum_threshold, 
	UINT8 *Gmvx, 
	UINT8 *Gmvy
)
{
	data UINT8	i;
	data UINT8	Number;	
	UINT8 xdata* pData;
	UINT8 histmv[MAX_MV_NUM+MAX_MV_NUM];
	UINT8 *histmv2=histmv+MAX_MV_NUM;
	UINT32 MeAddr0=distMeOutAddr;

	//UINT8 histmv[MAX_MV_NUM],histmv2[MAX_MV_NUM];/* input: (mv+1)*/
	//UINT32 time0,time1;
	//UINT8	DistMode;

	#if 0
		/****** save mv ******/
		UINT8 filename[ 12 ]="GMV00000BIN";
		UINT8 filename1[ 12 ]="EDGE0000BIN";
		
		
		//filename[ 4 ] = '0' + saveMECnt / 1000;
		//filename[ 5 ] = '0' + (saveMECnt / 100)-((saveMECnt / 1000)*10);
		filename[ 6 ] = '0' + (saveMECnt / 10)-((saveMECnt / 1000)*100)-(((saveMECnt % 1000)/100)*10);
		filename[ 7 ] = '0' + saveMECnt % 10;
		filename[ 11 ] = 0x20;

		filename1[ 6 ] = filename[ 6 ];
		filename1[ 7 ] = filename[ 7 ];
		filename1[ 11 ] = 0x20;

		if (saveMECnt<20){
			dramFileSave(filename, distMeOutAddr, Number0<<2);
			if((XBYTE[REG_Dram_MpgSb_CdspWRegIdx]&0x01)==0){
				dramFileSave(filename1, K_SDRAM_AviDistEdgeWndAddrB, Number0<<3);		
			}
			else{
				dramFileSave(filename1, K_SDRAM_AviDistEdgeWndAddrA, Number0<<3);		
			}
			
			saveMECnt++;
		}
		/********************/
	#endif
	
	Number = (UINT8)Number0;
	
	//DistModeFuncGet(&DistMode);
	//distMeOutAddrGet(DistMode,&MeAddr0);
	
	//if(sldebug) printf("Number=%d\n",Number);
	memset(histmv, 0, MAX_MV_NUM+MAX_MV_NUM);
	//memset(histmv2, 0, MAX_MV_NUM);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(MeAddr0);
	HAL_DmaDo(0, 1, Number<<2, 0, 0);
	pData = (UINT8 xdata *) 0x3800;	/* 1k SRAM */	    
	
	i=0;
		
	do
	{		
		if(*pData!=0xff){
			histmv[*(pData+1)+1]++;
		}
		
		if((*(pData+2))!=0xff){
			histmv2[*(pData+2)+1]++;
		}
		pData+=4;
	}
	while (++i<Number);
	
	
	maxMvFind(histmv2,(UINT8)MAX_MV_NUM-3,blknum_threshold,Gmvx);
	maxMvFind(histmv,(UINT8)MAX_MV_NUM-3,blknum_threshold,Gmvy);
	
	//printf("gmv x=%bd y=%bd\n",*Gmvx,*Gmvy);
	
	//if(sldebug) printf("max=%02bd\n",max);
	//-----------------------------------------------------------------------
}//gMvGet function End
//------------------------------------------------------------

/**
 * @fn		  void distVectorGet(UINT8 *dram_mvx, UINT8 *dram_mvy)
 * @brief	  constrain the Max Mv and output it
 * @param	  [out] dram_mvx : output Global Mv x
 * @param	  [out] dram_mvy : output Global Mv y
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
#pragma OT(8,SPEED) // optimize with speed first
void distVectorGet()
{
	//SINT8  Gmvx, Gmvy;
	//UINT8 Gmvx, Gmvy;
	UINT8 x,y;
	UINT16 cmpx,cmpy;
	UINT16 ax;//=0;
	UINT16 ay;//=0;
	UINT8 idx;
	UINT8 diff;
	#define zero 32
	//UINT8 i;
	
	gMvGet(DistBlockNumber, 5, &x, &y);

	//printf("b (%2bd,%2bd)(%2bd,%2bd)\n",x,y,preGMvX,preGMvY);
	if (x > zero){
		totalMvX += (x-zero);
	}
	else{
		totalMvX -= (zero-x);
	}
	
	if (y > zero){
		totalMvY += (y-zero);
	}
	else{
		totalMvY -= (zero-y);
	}

    if (absValue(totalMvX) > 128){
    	movingFlagX=1;    	
	}

    if (absValue(totalMvY) > 96){
    	movingFlagY=1;    	
	}


    if ((movingFlagX==1)&&(stableNumMvX>2)){
   		totalMvX=0;
   		movingFlagX=0;
    }
    
    if ((movingFlagY==1)&&(stableNumMvY>2)){
   		totalMvY=0;
   		movingFlagY=0;
    }

	//printf("a (%2bd,%2bd)(%2bd,%2bd)\n",x,y,preGMvX,preGMvY);	

	diff = abs(x,preGMvX );
	if (diff > 36){
		x = preGMvX;
		//printf("x over range\n");
	}

	diff = abs(y,preGMvY );
	if (diff > 36){
		y = preGMvY;
		//printf("y over range\n");
	}
	
	idx = distIdxCnt % 10;
	
    if ((hisGMvX[idx]<48) && (hisGMvX[idx]>16)){
    	if (stableNumMvX>0){
    		stableNumMvX--;
    	}
    }

    if ((hisGMvY[idx]<48) && (hisGMvY[idx]>16)){
    	if (stableNumMvY>0){
    		stableNumMvY--;
    	}
    }

    if ((x<48)&& (x>16)){
    	if (stableNumMvX<10){
    		stableNumMvX++;
    	}
    }

    if ((y<48)&&(y>16)){
    	if (stableNumMvY<10){
    		stableNumMvY++;
    	}
    }
    
	sumGMvX = sumGMvX - hisGMvX[idx] + x;
	sumGMvY = sumGMvY - hisGMvY[idx] + y;
    //printf("sumGMvX=%d\n",sumGMvX);
    
	hisGMvX[idx] = x;
	hisGMvY[idx] = y;

	//for (i=0;i<10;i++){
	//	printf("histX[%bd]=%bd\n",i,hisGMvX[i]);
	//}
	preGMvX = x;
	preGMvY = y;

	
	//printf("sumGMvX=%d,sumGMvY=%d\n",sumGMvX,sumGMvY);	
	distIdxCnt++;
	
	if (distCnt<10){
		distCnt++;		
		avgGMvX=avgGMvY=0;
		return;
	}
	else{
	
		avgGMvX = (UINT8)(sumGMvX/10);
		avgGMvY = (UINT8)(sumGMvY/10);	

		//printf("avgGMvX=%bd\n",avgGMvX);
	}

	if ((movingFlagX != 1)&&(movingFlagY != 1)){
		if(x!=32){//half pexil
			
			ax = accGMvx;
			cmpx = ((UINT16)distMvClampX*distFac)>>4;
			//printf("cmpx=%d\n",cmpx);
			if	(x>avgGMvX)		{ax+=x-avgGMvX;if(ax>=cmpx)ax=cmpx-1;}
			else if	(ax+x>avgGMvX)	{ax-=avgGMvX-x;}
			else				{ax=0;}
			cmpx-=ax+1;
			gDramMvx=cmpx>>1;
			gDramMvxDisp=(cmpx<<4)/distFac;
			accGMvx = ax;
		}

		if(y!=32){//half pexil

			ay = accGMvy;
			cmpy = ((UINT16)distMvClampY*distFac)>>4;
			//printf("cmpy=%d\n",cmpy);
			if	(y>avgGMvY)		{ay+=y-avgGMvY;if(ay>=cmpy)ay=cmpy-1;}
			else if	(ay+y>avgGMvY)	{ay-=avgGMvY-y;}
			else				{ay=0;}
			cmpy-=1+ay;
			gDramMvy=cmpy>>1;
			gDramMvyDisp=(cmpy<<4)/distFac;
			accGMvy=ay;
		}		
	}

	//printf("flagX=%bd,flagY=%bd,stableX=%bd,stableY=%bd\n",movingFlagX,movingFlagY,stableNumMvX,stableNumMvY);
	//printf("(%2bu,%2bu)(%2u,%2u)\n",x,y,accGMvx,accGMvy);
}/*distVectorGet function End*/

/**
 * @fn		  void DistGmvSet(UINT8 GmvX, UINT8 GmvY)
 * @brief	  set Gmvx and Gmvy global variate
 * @param	  [in] GmvX : input Gmv x parameter
 * @param	  [in] GmvY : input Gmv y parameter
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
distGmvSet(UINT8 GmvX, UINT8 GmvY){
	gDramMvx = GmvX;
	gDramMvy = GmvY;
}

/**
 * @fn		  void distGmvGet(UINT8* GmvX, UINT8* GmvY)
 * @brief	  get Gmvx and Gmvy global variate
 * @param	  [out] GmvX : output Gmv x parameter
 * @param	  [out] GmvY : output Gmv y parameter
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
distGmvGet(UINT8* GmvX, UINT8* GmvY){
	*GmvX = gDramMvx;
	*GmvY = gDramMvy;
}

/**
 * @fn		  void distClampSizeSet(UINT8 ClmpX, UINT8 ClmpY)
 * @brief	  set Gmv constrain value, it will clamp the Gmv in this region
 * @param	  [in] ClmpX : input Gmv x constrain value
 * @param	  [in] ClmpY : input Gmv y constrain value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distClampSizeSet(UINT8 ClmpX, UINT8 ClmpY){
	distMvClampX = ClmpX;
	distMvClampY = ClmpY;
	distPrintf("ClpX=%bu, CmpY=%bu\n",distMvClampX,distMvClampY);
	
}

/**
 * @fn		  void distClampSizeGet(UINT8* ClmpX, UINT8* ClmpY)
 * @brief	  get Gmv constrain value, it will clamp the Gmv in this region
 * @param	  [out] ClmpX : output Gmv x constrain value
 * @param	  [out] ClmpY : output Gmv y constrain value
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distClampSizeGet(UINT8* ClmpX, UINT8* ClmpY){
	*ClmpX = distMvClampX;
	*ClmpY = distMvClampY;
}




/**
 * @fn		  void DistSwitchSize(UINT16 SrcXsize, UINT16 SrcYsize)
 * @brief	  for different resolution(QVGA/VGA/D1/HD)to match mv ratio when video record
 * @param	  [in] SrcXsize : resolution x size(pixel value)
 * @param	  [in] SrcYsize : resolution y size(pixel value)
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distSwitchSize(UINT16 SrcXsize, UINT16 SrcYsize){

	UINT8 hoffset,voffset, hsize, vsize, hblock=12, vblock=12;
	UINT16 Srchsize, Srcvsize;
	UINT16 basex=320,basey=180;
	UINT8 mapXfac,mapYfac;
	UINT8 clmpX,clmpY;

	if (SrcXsize==0 || SrcYsize==0){
		return;
	}
	Srchsize = basex;
	Srcvsize = (UINT16)((UINT32)((UINT32)Srchsize*SrcYsize)/SrcXsize);
	hoffset = 16;
	voffset = 16;
	//voffset += (UINT8)(basey-Srcvsize)>>1;
	hsize	= (basex - hoffset*2)/hblock;// 2nd path, cannot over 2nd path size
	
	vsize	= (basey - voffset*2)/vblock;//sedpath=1
	
	HAL_CdspDistEdgeWinSizeSet((UINT8)hsize, (UINT8)vsize);
	HAL_CdspDistEdgeWinOffSet((UINT8)hoffset, (UINT8)voffset);
	mapXfac = (SrcXsize<<5)/Srchsize;
	mapYfac = (SrcYsize<<5)/Srcvsize;
	HAL_CdspDistEdgeWinMap(mapXfac, mapYfac);
	clmpX = Srchsize/10;
	clmpY = Srcvsize/10;
	distClampSizeSet(clmpX,clmpY);
}

/**
 * @fn		  void DistModeFuncSet(UINT8 mode)
 * @brief	  set Dist Mode
 * @param	  [in] mode : '1' for still view, '2' for video view/record
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distModeFuncSet(
	UINT8 mode
)
{
	distMode = mode;
}

/**
 * @fn		  void DistModeFuncGet(UINT8* mode)
 * @brief	  get Dist mode
 * @param	  [out] mode : '1' for still view, '2' for video view/record
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distModeFuncGet(
	UINT8* mode
)
{
	*mode = distMode;
}

/**
 * @fn		  void distEdgeAddrGet(UINT8 mode, UINT32* EdgeAddrA, UINT32* EdgeAddrB)
 * @brief	  get dist edge address set by memory initialize
 * @param	  [in] mode : '1' for still view, '2' for video view/record
 * @param	  [in] EdgeAddrA : edge window A buffer address 
 * @param	  [in] EdgeAddrBA : edge window B buffer address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distEdgeAddrGet(
	UINT8 mode,
	UINT32* EdgeAddrA,
	UINT32* EdgeAddrB
)
{
	if (mode==1){// Still Pv
		*EdgeAddrA = K_SDRAM_PvDistEdgeWndAddrA;
		*EdgeAddrB = K_SDRAM_PvDistEdgeWndAddrB;
	}
	else if(mode==2){// Video Pv/Record
		*EdgeAddrA = K_SDRAM_AviDistEdgeWndAddrA;
		*EdgeAddrB = K_SDRAM_AviDistEdgeWndAddrB;
	}
	else{// Still Pv
		*EdgeAddrA = K_SDRAM_PvDistEdgeWndAddrA;
		*EdgeAddrB = K_SDRAM_PvDistEdgeWndAddrB;
	}
	
}

/**
 * @fn		  void distMeOutAddrGet(UINT8 mode, UINT32* MeAddr0)
 * @brief	  set dist edge address set by memory initialize
 * @param	  [in] mode : '1' for still view, '2' for video view/record
 * @param	  [in] MeAddr0 : Me Mv window buffer address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distMeOutAddrGet(
	UINT8 mode,
	UINT32* MeAddr0
)
{
	if (mode==1){// Still Pv
		*MeAddr0 = K_SDRAM_PvDistMeAddr;
	}
	else if(mode==2){// Video Pv/Record
		*MeAddr0 = K_SDRAM_AviDistMeAddr;
	}
	else{// Still Pv
		*MeAddr0 = K_SDRAM_PvDistMeAddr;
	}
	
}
#endif
/**
 * @fn		  void distDispSet(void)
 * @brief	  Dist Display size and offset set
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
distDispSet(
	void
)
{
#if 0
	/* set pvx0 */
	XBYTE[REG_Disp_PvX0L] = 0x00;
	XBYTE[REG_Disp_PvX0H] = 0x00;

	/* set pvx1 */
	//XBYTE[REG_Disp_PvX1L] = 0xff;
	//XBYTE[REG_Disp_PvX1H] = 0x00;
	XBYTE[REG_Disp_PvX1L] = 0x20;//0xC0;//0x20
	XBYTE[REG_Disp_PvX1H] = 0x01;//0x00;//0x01

	/* set pvy0 */
	XBYTE[REG_Disp_PvY0L] = 0x00;
	XBYTE[REG_Disp_PvY0H] = 0x00;

	/* set pvy1 */
	//XBYTE[REG_Disp_PvY1L] = 0xB7;
	//XBYTE[REG_Disp_PvY1H] = 0x00;
	XBYTE[REG_Disp_PvY1L] = 0xd8;//0x90;//0xd8
	XBYTE[REG_Disp_PvY1H] = 0x00;
#endif

#ifdef DIST_FUNC_EN

	UINT32 TmpY=240;
	//UINT8 i;
	UINT32 x0,x1,y0,y1;
	UINT16 h,v;
	UINT8 sts = sp1kDistStatusGet();

	HAL_DispImgXyGet(&x0,&y0,&x1,&y1);
	distPrintf("(%lu,%lu)(%lu,%lu)\n",x0,y0,x1,y1);
	if(sts){
		x1-=distMvClampX;///distFac;
		y1-=distMvClampY;///distFac;
	}
	else{
		x1+=distMvClampX;///distFac;
		y1+=distMvClampY;///distFac;
	}
	if(x1>=320)x1=319;
	if(y1>=240)y1=239;
	distPrintf("(%lu,%lu)(%lu,%lu)\n",x0,y0,x1,y1);
	//TmpY = (UINT32)320*distMvClampY/distMvClampX;
	HAL_DispImgXySet(x0, y0, x1, y1);	
	HAL_DispScaleUpdate();
	
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB); //update img sync vd

	pvSizeGet(&h,&v);
	dispFitShow(h,v,1);
#endif	
}

UINT8 
sp1kDistProcSet(
	UINT8 proc
)
{
	#ifdef DIST_FUNC_EN
		UINT32 edgeAddrA,edgeAddrB;
		if(proc!=0xff)
		{
			distEdgeAddrGet(
				proc,
				&edgeAddrA,
				&edgeAddrB
			);
			distMeOutAddrGet(
				proc,
				&distMeOutAddr
			);

			HAL_CdspDistEdgeWinAddrSet(edgeAddrA, edgeAddrB);
			HAL_DistMeOutAddrSet(distMeOutAddr);
		}
		return proc;
	#else
		return proc;
	#endif
}

/**
 * @fn		  void distInit(void)
 * @brief	  Dist initial parameters set
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
sp1kDistInit(
	void
)
{
#ifdef DIST_FUNC_EN

	UINT16	hoffset, voffset, hsize, vsize, hblock=8, vblock=6;
	UINT8	hwincnt, vwincnt, ewthrh, ewthrv;
	//UINT8	distMode;
	//UINT32	EdgeAddrA,EdgeAddrB,MeAddr0;
	UINT16 PvHsize,PvVsize;
	UINT16 DistVsize;
	UINT8 mapXfac/*,mapYfac*/;
	UINT8 i;

	pvSizeGet(&PvHsize, & PvVsize);
	
	distPrintf("dist pv:%u,%u\n",PvHsize,PvVsize);
	
	sp1kDistProcSet(2);//avi proc
	/*
	DistModeFuncGet(&distMode);
	distEdgeAddrGet(distMode,&EdgeAddrA,&EdgeAddrB);
	distMeOutAddrGet(distMode,&MeAddr0);
	HAL_CdspDistEdgeWinAddrSet(EdgeAddrA, EdgeAddrB);
	HAL_DistMeOutAddrSet(MeAddr0);
	*/
	DistBlockNumber=hblock*vblock;
	
	hoffset = 16;
	voffset = hoffset * PvVsize/PvHsize;
	//printf("voffset=%d\n",voffset);
	//voffset = 16;
	hsize	= (320 - hoffset*2)/hblock;// 2nd path, cannot over 2nd path size
	DistVsize = (UINT16)((320ul*PvVsize)/PvHsize);
	vsize	= (DistVsize - voffset*2)/vblock;//sedpath=1
	hwincnt = (UINT8)(hblock);
	vwincnt = (UINT8)(vblock);
	ewthrh	= 0x20;
	ewthrv	= 0x20;
	HAL_CdspDistEdgeWinThrSet(ewthrh, ewthrv);
	HAL_CdspDistEdgeWinOffSet((UINT8)hoffset, (UINT8)voffset);
	HAL_CdspDistEdgeWinSizeSet((UINT8)hsize, (UINT8)vsize);
	HAL_CdspDistEdgeWinCntSet(hwincnt,vwincnt);
	mapXfac = (PvHsize<<5)/320;
	//mapXfac = 32;
	//mapYfac = (SrcYsize<<5)/Srcvsize;
	HAL_CdspDistEdgeWinMap(mapXfac, mapXfac);
	HAL_Dist2ndPathOpen();
	HAL_DistMeIntervalSet(0x01);
	HAL_DistBlkNumSet((UINT8)(DistBlockNumber-1));
	//HAL_DistSkipSet(0x00);
	HAL_DistMeModeSet(0x01);
	HAL_CdspDistEdgeWinEn(0x01, 0x00,0x00,0x00,0x01);
	HAL_DramFrmBufNumSet(2);
	
	//distClampSizeSet(32,24);
	//DistVsize = (UINT16)((UINT32)((UINT32)32*PvVsize)/PvHsize);
	HAL_DistMeIntrEn(0,0);

	//XBYTE[0x2277]&=~0x08;
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_DISTEDGE,0x00); /* don't skip edge window */
	//HAL_CdspVDramSkipWrSet(CDSP_SKIP_DISTEDGE,0xff); /* don't skip edge window */
	
	/*****************************************************/
	//distClampSizeSet(32,DistVsize);
	distFac = mapXfac;
	distMissCnt=0;
	
	distMvClampX = 32;
	distMvClampY = 32*PvVsize/PvHsize;

	gDramMvxDisp = (distMvClampX>>1)/*distFac*/;
	gDramMvyDisp = (distMvClampY>>1)/*distFac*/;
	
	gDramMvx=((UINT16)distMvClampX * distFac >> 6);
	gDramMvy=((UINT16)distMvClampY * distFac >> 6);

	accGMvx= (((UINT16)distMvClampX * mapXfac)>>6);
	accGMvy= (((UINT16)distMvClampY * mapXfac)>>6);		
	
	for(i=0;i<16;i+=2)
	{
		XBYTE[REG_Dram_DisMvx_Disp0+i]=gDramMvx; 
		XBYTE[REG_Dram_DisMvy_Disp0+i]=gDramMvy;	
	}

	distCnt=distIdxCnt=0;
	sumGMvX=sumGMvY=320;

	for(i=0;i<10;i++){
		hisGMvX[i]=32;
		hisGMvY[i]=32;
	}

	preGMvX = preGMvY = 32;

	movingFlagX = movingFlagY = 0;
	totalMvX = totalMvY = 0;
	stableNumMvX = stableNumMvY =10;
	//sp1kDistEnable();	
	//saveMECnt=0;
	
#endif
}

/**
 * @fn        void sp1kDistEnable(UINT8 updateDisp)
 * @brief     enable dist function
 * @param     [in] updateDisp
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kDistEnable(
	void
)
{
#ifdef DIST_FUNC_EN

	//distPrintf("dist enable:%bx,%bx\n",sp1kDistStatusGet(),updateDisp);
	distPrintf("dist enable:%bx\n",sp1kDistStatusGet());

	if(sp1kDistStatusGet())
		return;
	
	HAL_DistEn(0xff);
	sp1kVdProcEnable(VD_PROC_DIST,1);
	HAL_CdspDistEdgeWinEn(0x01, 0x00,0x00,0x00,0x01);

	//if(updateDisp){
		distDispSet();
	//}
#endif
}


/**
 * @fn        void sp1kDistDisable(UINT8 updateDisp)
 * @brief     sp1kDistDisable
 * @param     [in] updateDisp
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kDistDisable(
	void
)
{
#ifdef DIST_FUNC_EN

	//UINT16 x,y;	
	//distPrintf("dist disable:%bx,%bx\n",sp1kDistStatusGet(),updateDisp);
	distPrintf("dist disable:%bx\n",sp1kDistStatusGet());
	
	if(!sp1kDistStatusGet())
		return;
	sp1kVdProcEnable(VD_PROC_DIST,0);
	HAL_DistDisable();	
	HAL_CdspDistEdgeWinEn(0x00, 0x00,0x00,0x00,0x01);
	//if(updateDisp){
		distDispSet();
		memset(&(XBYTE[REG_Dram_DisMvx_Disp0]),0,16);
	//}
	//gDramMvx=0;//distMvClampX>>1;
	//gDramMvy=0;//distMvClampY>>1;	
	
	//memset(&(XBYTE[REG_Dram_DisMvx_Disp0]),0,8);
	
	//disable disp move
	//pvSizeGet(&x,&y);
	//distPrintf("%u,%u\n",x,y);
	//HAL_DispImgXySet(0, 0, 320UL - 1, 320ul*y/x - 1);
	//HAL_DispScaleUpdate();
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);

	//disable dist vector Calculate
#endif	
}

/**
 * @fn        void distSvcProc(UINT8* pt)
 * @brief     dist main Proc
 * @param     [in] pt	:vd count pointer
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void 
distSvcProc(
	UINT8* pt
)
{
#ifdef DIST_FUNC_EN
	UINT32 cnt=0x0fff;
	UINT8 dram2mjpgfrm;//=(XBYTE[REG_Dram_VlcRegIdx]&0x30)>>3;

	//UINT32 t0,t1,t2;
	//distGetTimer(&t0);
	//distFlag = 1;
	if(!(XBYTE[REG_Jpeg_Intr_Event]&0x20))
	{
		distPrintf("dist miss!%bx\n",*pt);
		distMissCnt++;
	
		return;
		pt=pt;
	}
	while(!(XBYTE[REG_Jpeg_Intr_Event]&0x40));			
	//distGetTimer(&t1);
	XBYTE[REG_Jpeg_Intr_Event]|=0x60;
	distVectorGet();
	
	
	//if(distFlag){
		dram2mjpgfrm=(XBYTE[REG_Dram_VlcRegIdx]&0x30)>>3;
		XBYTE[REG_Dram_DisMvx_Disp0+dram2mjpgfrm]=gDramMvxDisp; //for disp
		XBYTE[REG_Dram_DisMvy_Disp0+dram2mjpgfrm]=gDramMvyDisp;
		XBYTE[REG_Dram_DisMvx_Mjpeg0+dram2mjpgfrm]=gDramMvx; //for jpg
		XBYTE[REG_Dram_DisMvy_Mjpeg0+dram2mjpgfrm]=gDramMvy;
	//}
	//distPrintfLog("dram (%2bu,%2bu)",gDramMvx,gDramMvy);
	//distPrintfLog("(%2bu,%2bu)\n",gDramMvxDisp,gDramMvyDisp);

	
	#if DIST_PRINT_LOOP
		if(distDebugCnt){
			if(distDebugCnt!=0xff){			
				distDebugCnt--;
			}

			distPrintfLoop("(%2bu,%2bu)%2bu",gDramMvx,gDramMvy,distFac);
			distPrintfLoop("(%2bu,%2bu)",gDramMvxDisp,gDramMvyDisp);
			distPrintfLoop("(%2bu,%2bu)\n",distMvClampX,distMvClampY);
		}
	#endif

	//distGetTimer(&t2);
	distPrintf("dist:%ld,%ld,%ld\n",t1-t0,t2-t1,t2-t0);
#else
	pt = pt;
#endif
}
/**
 * @fn        void distIntr(void)
 * @brief     dist set result, using by intrrupt 
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void 
distIntr(
	void
)
{
#ifdef DIST_FUNC_EN
	UINT8 dram2mjpgfrm=(XBYTE[REG_Dram_VlcRegIdx]&0x30)>>3;
	XBYTE[REG_Dram_DisMvx_Disp0+dram2mjpgfrm]=gDramMvxDisp; //for disp
	XBYTE[REG_Dram_DisMvy_Disp0+dram2mjpgfrm]=gDramMvyDisp;
	XBYTE[REG_Dram_DisMvx_Mjpeg0+dram2mjpgfrm]=gDramMvx; //for jpg
	XBYTE[REG_Dram_DisMvy_Mjpeg0+dram2mjpgfrm]=gDramMvy;
	//distPrintf("-%2bu,%2bu\n",gDramMvx,gDramMvy);
	//distFlag = 0;
#endif
}

void 
distTest(
	UINT8* pt
)
{
#if 0
	UINT8 i;
	UINT32 val[5],c=0,n=0,j=-1;
	memset(val,0,20);
	for(i=0;i<20;i++)
	{
		UINT8 b=pt[i+1];
		UINT8 a=0;
		if(b==0){
			if(j+1!=i)
			{				
				val[n++]=c;
			}	
			break;
		}
		else if(b==' ')
		{
			if(j+1!=i)
			{				
				val[n++]=c;
				c=0;
			}			
			j=i;
			continue;
		}
		else if(b>='0' && b<='9')a=b-'0';
		else if(b>='a' && b<='f')a=b-'a'+10;
		else if(b>='A' && b<='F')a=b-'A'+10;
		else continue;	

		c=c*16+a;
	}

	distPrintf("%s,(%lx,%lx,%lx,%lx,%lx)\n",pt,val[0],val[1],val[2],val[3],val[4]);
	if(*pt=='p'){
		distDebugCnt=val[0];
	}else if(*pt=='m'){
		distPrintf("miss;%bx\n",distMissCnt);
	}else if(*pt=='i'){
		sp1kDistInit();
	}else if(*pt=='0'){
		sp1kDistDisable();
	}else if(*pt=='1'){
		sp1kDistEnable();
	}else if(*pt=='2'){
		sp1kDistInit();
	}else if(*pt=='3'){
		sp1kDistInit();
	}else{
		distPrintf("miss:%lx\n",distMissCnt);
	}
#else
	pt=pt;
#endif
}

/**********************************/
/* unused functions               */
/**********************************/
#if 0
/**
 * @fn        void DistCalMv(void)
 * @brief     DistCalMv
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-6-14
 * @todo      N/A
 * @bug       N/A
*/
void
distCalMv()
{
    if (gDistGmvDo==1){
		UINT8 mvx,mvy;
		//XBYTE[REG_Jpeg_Intr_Enable] &= ~0x40;
		HAL_DistMeIntrEn(0x00, 0x00);
		distVectorGet(&mvx, &mvy);
		distGmvSet(mvx, mvy);
		gDistGmvDo = 2;
		//XBYTE[REG_Jpeg_Intr_Enable] |= 0x40;
		HAL_DistMeIntrEn(0x00, 0x01);
		//distPrintf("dram_mvx=%d,dram_mvy=%d\n",(UINT16)mvx,(UINT16)mvy);
        }
}

/**
 * @fn		  void INTR_DistGmvSet(void)
 * @brief	  set Gmv to background in interrupt
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distGmvSetIntr(void){
	static UINT8 CntReg=0;
	UINT8 tmp, dram2mjpgfrm, mvkeep;
	
	tmp 		=XBYTE[REG_Dram_VlcRegIdx];
	dram2mjpgfrm=(tmp&0x30)>>4;
	mvkeep		=	tmp 	 >>6;
	  
	  if(mvkeep){
		gDramMvx=old_dram_mvx;
		gDramMvy=old_dram_mvy;
		//distPrintf("dram2mjpgfrm=%bd dram_mvx=0x%bx  dram_mvy=0x%bx\n",dram2mjpgfrm,dram_mvx,dram_mvy);
	  }
	  else{
			if (gDistGmvDo==0){
			CntReg = dram2mjpgfrm;
			//XBYTE[REG_Jpeg_Intr_Enable] &= ~0x40;
			HAL_DistMeIntrEn(0x00, 0x00);
			gDistGmvDo = 1;
			   }
		//distVectorGet(DistBlockNumber, &gDramMvx, &gDramMvy);
		if (gDistGmvDo==2){
			//gDramMvx=127-gDramMvx;
			//gDramMvy=127-gDramMvy;
			gDramMvx=distMvClampX-gDramMvx;
			gDramMvy=distMvClampY-gDramMvy;
		}
		//distPrintf("dram2mjpgfrm=%bd dram_mvx=0x%bx  dram_mvy=0x%bx\n",dram2mjpgfrm,gDramMvx,gDramMvy);
	  }
		if (gDistGmvDo==2){
		if (CntReg!=((XBYTE[REG_Dram_VlcRegIdx]>>4)&0x03)){
			gDistGmvDo = 0;
			XBYTE[REG_Jpeg_Intr_Event]=0x40;
			distPrintf("Error frm=%bx,Reg=%bx\n",dram2mjpgfrm,CntReg);
		}
#if 1
			XBYTE[REG_Dram_DisMvx_Disp0+(dram2mjpgfrm<<1)]=gDramMvx; //for disp
			XBYTE[REG_Dram_DisMvy_Disp0+(dram2mjpgfrm<<1)]=gDramMvy;
			XBYTE[REG_Dram_DisMvx_Mjpeg0+(dram2mjpgfrm<<1)]=gDramMvx; //for jpg
			XBYTE[REG_Dram_DisMvy_Mjpeg0+(dram2mjpgfrm<<1)]=gDramMvy;
#else
			XBYTE[REG_Dram_DisMvx_Disp0+(dram2mjpgfrm<<1)]=(distMvClampX+1)>>1;//gDramMvx; //for disp
			XBYTE[REG_Dram_DisMvy_Disp0+(dram2mjpgfrm<<1)]=(distMvClampY+1)>>1;//gDramMvy;
			XBYTE[REG_Dram_DisMvx_Mjpeg0+(dram2mjpgfrm<<1)]=(distMvClampX+1)>>1;//gDramMvx; //for jpg
			XBYTE[REG_Dram_DisMvy_Mjpeg0+(dram2mjpgfrm<<1)]=(distMvClampY+1)>>1;//gDramMvy;
#endif

			old_dram_mvx = gDramMvx;
			old_dram_mvy = gDramMvy;
			gDistGmvDo = 0;
			XBYTE[REG_Jpeg_Intr_Event]=0x40;
			}
		//XBYTE[REG_Jpeg_Intr_Event]=0x40;
}

UINT8 distEnable(UINT8 en)
{//[0]: '1' enable DIST, '0' disable DIST
// [1]: '1' normal case, '0' diable display Mv for Pb	
	if(en!=0xff){
		HAL_DistEn(en);
	}
	return sp1kVdProcEnable(VD_PROC_DIST,en);

}

/**
 * @fn		  void DistFuncOpen(UINT8 mode)
 * @brief	  Open Dist Function 
 * @param	  [in] mode : [0]: '1' enable DIST, '0' disable DIST
 *						  [1]: '1' normal case, '0' diable display Mv for Pb
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
DistFuncOpen(
	UINT8 mode
)
{
	distFuncEn(1);
	HAL_DistEn(mode); //[0]: '1' enable DIST, '0' disable DIST
					// [1]: '1' normal case, '0' diable display Mv for Pb
	HAL_DistMeIntrEn(0x00, 0x01);

}


/**
 * @fn		  void DistFuncEn(UINT8 en)
 * @brief	  set Enable Dist global value
 * @param	  [in] en : dist function enable parameter
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distFuncEn(UINT8 en){
	distEnable = en;
}


/**
 * @fn		  void DistFuncEnGet(UINT8* en)
 * @brief	  get Enable Dist global value
 * @param	  [out] en : dist function enable parameter
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
distFuncEnGet(UINT8* en){
	*en = sp1kDistStatusGet();
}

void gMvGet0(UINT16 Number0, UINT8 blknum_threshold, UINT8 *Gmvx, UINT8 *Gmvy){
	data UINT8	i=Number0;
	//data UINT8 xdata* pData;
	//UINT8 histmv[MAX_MV_NUM],histmv2[MAX_MV_NUM];/* input: (mv+1)*/
	//UINT32 time0,time1;
	UINT8 xdata histmv[MAX_MV_NUM+MAX_MV_NUM];
	data UINT8 xdata* histmv2=histmv+MAX_MV_NUM;
	data UINT8 xdata* MvPt=histmv;

	//UINT32 MeAddr0=distMeOutAddr;
//	UINT8	DistMode;
	data UINT8	Number = Number0;
	data UINT8 v0,v1,v2,vCur,vMax,iSnd,iMax;

	//Number = (UINT8)Number0;
	
	memset(histmv, 0, MAX_MV_NUM+MAX_MV_NUM);
	for(i=0;i<MAX_MV_NUM+MAX_MV_NUM;i++)
		MvPt[i]=0;

	M_DRAM_SetStartAddressV(distMeOutAddr,1);
	for(i=0;i<Number;i++)
	{
		//M_DRAM_ReadWord(v0,v1);
		v0 = M_REG_DRAM_LsbByte; 
		v1 = M_REG_DRAM_MsbByte;
		if(v0!=0xff){
			MvPt[v1+1]++;
		}

		v0 = M_REG_DRAM_LsbByte; 
		v1 = M_REG_DRAM_MsbByte;
		if(v0!=0xff){
			histmv2[v0+1]++;
		}
	}	

	//x
	MvPt=histmv;
	v0 = *MvPt++;
	v1 = *MvPt++;
	v2 = *MvPt++;
	vMax=0;
	for(i=0;i<MAX_MV_NUM-2;i++)
	{
		vCur= v0+v1+v2;    
		if ((v1>=v2) && (v1>=v0) && (vCur>vMax)){
			vMax=vCur;
			iSnd=iMax;
			iMax=i;			
		}
		v0 = v1;
		v1 = v2;
		v2 = *MvPt++;
		
	}
	if(vMax<blknum_threshold) iMax = 32;
	*Gmvx = iMax;
	//y
	MvPt=histmv2;
	v0 = *MvPt++;
	v1 = *MvPt++;
	v2 = *MvPt++;
	vMax=0;
	for(i=0;i<MAX_MV_NUM-2;i++)
	{
		vCur= v0+v1+v2;    
		if ((v1>=v2) && (v1>=v0) && (vCur>vMax)){
			vMax=vCur;
			iSnd=iMax;
			iMax=i;			
		}
		v0 = v1;
		v1 = v2;
		v2 = *MvPt++;
		
	}
	if(vMax<blknum_threshold) iMax = 32;
	*Gmvy = iMax;

}//gMvGet function End


/**
 * @fn		  void sp1kDistDisable(void)
 * @brief	  close Dist function
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-29
 * @todo	  N/A
 * @bug 	  N/A
*/
void
sp1kDistDisable(
)
{
	distFuncEn(0);
	HAL_DistMeIntrEn(0x00, 0x00);
	HAL_DistDisable();
	distClampSizeSet(0,0);
}

void distVectorGet()
{
    SINT8  Gmvx, Gmvy;
	UINT8 x,y;
	UINT8 cmpx = ((UINT16)distMvClampX)*distFac>>5;
	UINT8 cmpy = ((UINT16)distMvClampY)*distFac>>5;

	gMvGet(DistBlockNumber, 5, &Gmvx, &Gmvy);

	Gmvx-=32;
	accGMvx += (SINT16)Gmvx;
	if     (accGMvx>=cmpx)  accGMvx = cmpx-1;
	else if(accGMvx<-cmpx)  accGMvx =-cmpx;

	Gmvy-=32;
	accGMvy += (SINT16)Gmvy;
	if     (accGMvy>=cmpy)  accGMvy = cmpy-1;
	else if(accGMvy<-cmpy)  accGMvy =-cmpy;

	x = (((UINT16)accGMvx+cmpx)>>1)+(((UINT16)accGMvx+cmpx)&0x01);
	y = (((UINT16)accGMvy+cmpy)>>1)+(((UINT16)accGMvy+cmpy)&0x01);
	if (x>=cmpx) x=cmpx-1;
	if (y>=cmpy) y=cmpy-1;

	//x/=distFac;
	//y/=distFac;

	gDramMvx=cmpx-1-x;
	gDramMvy=cmpy-1-y;
	
	//if(distFac!=32){
		gDramMvxDisp=(((UINT16)gDramMvx)<<5)/distFac;
		gDramMvyDisp=(((UINT16)gDramMvy)<<5)/distFac;
	//}
	/*else{
		gDramMvxDisp=gDramMvx;
		gDramMvyDisp=gDramMvy;
	}*/
		
}//distVectorGet function End

#endif

