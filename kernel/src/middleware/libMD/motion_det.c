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
 * @file	motion_det.c
 * @brief	
 * @author	Will
 * @since	2011-02-15
 * @date	2011-02-15
 */

 /* notify
	If sensor input is YUV, please set "#define II_AE_MOTION_DET_EN" to 1 in "ae_def.h".
 */
 
 /**************************************************************************
 *							  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "motion_det.h"
#include "sp1k_md_api.h"
#include "sp1k_front_api.h"
#include "pv_task.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_dist.h"
#include "hal_dram.h"
#include "initio.h"
#include "os_api.h"
#include "hal.h"
#include "utility.h"
#include "dist_func.h"
#include "sp1k_dist_api.h"
#include "os_msg.h"
#include "sp1k_ae_api.h"
#include "motion_debug.h"
#include "reg_def.h"
#include "front_logi.h"
#include "ae_api.h"
#include "video_clip.h"
#include "gpio.h"
#include "dbg_def.h"
#include "cdsp_filter.h"
#include "panorama_def.h"
#include "panorama_api.h"

#if MOTION_DET_EN
/**************************************************************************
 *							 C O N S T A N T S							  *
/**************************************************************************/

#define __FILE	__FILE_ID_MOTION_DET__

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/

#define abs(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x)) 



/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
static UINT8 xdata gBlockNumber;
static UINT32 xdata gMEOutAddr;

static UINT8 xdata gDetectThr;
static UINT8 xdata gDetectTime;
static UINT8 xdata gDetectTimeThr=5;

static UINT8 xdata gDelayCnt; 

static UINT8 xdata gLearningFlag;
static UINT8 xdata gLearningCnt;
static UINT16 xdata gLearningSum;
//static UINT8 xdata mdSaveMECnt;
static UINT8 xdata gMdSensitivity;
static UINT8 xdata gMDEn;
static UINT8 xdata gMDLastGId;

static UINT8 xdata gMDYuvSensor;
static UINT8 xdata gMDLumaCnt;
static UINT8 xdata gMDLumaThr;
static UINT8 xdata gMDLumaStableCnt;
static UINT8 xdata gMDLumaDetTime=2; 
static UINT16 xdata gMDLearnLumaSum;



//linjiecheng
#if 1
UINT8 histMVX[MD_MAX_MV_NUM];//x Histogram
UINT8 histMVY[MD_MAX_MV_NUM];//y Histogram
UINT8 mvXMidNum,mvYMidNum;//x32 cnt number, y32 cnt number
UINT8 mdActionStatus;//0: MD_NOT_ACTION, 1:MD_ACTION, 2:MD_LEARNING
UINT8 tdAdjustByMdEn;//0:MD_ADJUST_TD_DISABLE , 1:MD_ADJUST_TD_ENABLE
#endif

//linjiecheng dynamic MD UI
MD_BAR mdBar = {
	0,
	0,
};

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static UINT8 mdMotionChk(UINT8 maxMVx,UINT8 maxMVy,UINT8 maxMVxNum,UINT8 maxMVyNum);
static void mdLearningCtrl(UINT8 maxMVx,UINT8 maxMVy,UINT8 maxMVxNum,UINT8 maxMVyNum);

/**
 * @fn                void mdInit(void)
 * @brief             	Initial Motion Detection setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            none
 */
void 
mdInit(
	void
)
{	
	UINT16 hOffset, vOffset, hSize, vSize, hBlockNum, vBlockNum;
	UINT32 edgeWinAddrA,edgeWinAddrB;
	UINT16 pvHSize,pvVSize;
	UINT16 secPathVSize;
	UINT8 mapXfac;
	UINT8 tmp;
	UINT16 para1,para2;
	//UINT8 i;
	
	hBlockNum = EDGE_TBL_HBLOCKNUM;
	vBlockNum = EDGE_TBL_VBLOCKNUM;
	
	pvSizeGet(&pvHSize, & pvVSize);
		
	motionPrintf("pv:%u,%u\n",pvHSize,pvVSize);
		
	gBlockNumber=hBlockNum*vBlockNum;
		
	hOffset = EDGE_WIN_HOFFSET;
	vOffset = EDGE_WIN_VOFFSET;
	hSize	= ((UINT16)EDGE_SRC_YUV_H - hOffset*2)/hBlockNum;// 2nd path, cannot over 2nd path size
	secPathVSize = (UINT16)(((UINT32)EDGE_SRC_YUV_H*pvVSize)/pvHSize);
	vSize	= (secPathVSize - vOffset*2)/vBlockNum;//sedpath=1

	/* edge table initial */
	HAL_CdspDistEdgeWinThrSet(EDGE_WIN_H_THR, EDGE_WIN_V_THR);
	HAL_CdspDistEdgeWinOffSet((UINT8)hOffset, (UINT8)vOffset);
	HAL_CdspDistEdgeWinSizeSet((UINT8)hSize, (UINT8)vSize);
	HAL_CdspDistEdgeWinCntSet((UINT8)(hBlockNum),(UINT8)(vBlockNum));
	
	mapXfac = (UINT8)(((UINT32)pvHSize<<5)/((UINT16)EDGE_SRC_YUV_H));
	HAL_CdspDistEdgeWinMap(mapXfac, mapXfac);

	mdEdgeTblAddrGet(osSysModeGet(),&edgeWinAddrA,&edgeWinAddrB);
	mdMeOutAddrGet(osSysModeGet(),&gMEOutAddr);
	HAL_CdspDistEdgeWinAddrSet(edgeWinAddrA, edgeWinAddrB);	
	HAL_DistMeOutAddrSet(gMEOutAddr);


	//HAL_Dist2ndPathOpen();

	tmp = HAL_CdspSyncModeGet();
	HAL_CdspSyncModeSet(IMMEDIATE);
	HAL_Cdsp2ndPathEnSet(1);							
	HAL_Cdsp2ndPathCfg(0,0,0);	
	HAL_CdspSyncModeSet(tmp);
	
	
	HAL_DistMeIntervalSet(0x01);
	HAL_DistBlkNumSet(gBlockNumber-1);
	//HAL_DistSkipSet(0x00);
	HAL_DistMeModeSet(0x01);
	HAL_CdspDistEdgeWinEn(0x01, 0x00,0x00,0x00,0x01);
	HAL_DramFrmBufNumSet(2); /* four frame mode */
		
	HAL_DistMeIntrEn(0,0);
	
	mdEdgeTblFill(hSize,vSize,hOffset,vOffset,hBlockNum,vBlockNum);	

	//mdSaveMECnt=0;
	gDetectTime = 0;
	gLearningFlag = 0;
	//sp1kMDSenSet(MD_SEN_HIGH);
	//sp1kDistEnable();	
	//sp1kDistInit();
	//sp1kDistProcSet(2);//avi proc

	
	//gLastLuma=aeFlowGetCurLuma();

	frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );
	
	if( para1 == FRONT_DATA_FMT_YUV ) { /* YUV sensor */
		gMDYuvSensor = 1;
		//printf("yuv \n");	
	}
	else{
		gMDYuvSensor = 0;
		//aeStatusGet(SP1K_AE_gId,&gMDLastGId);
	}

	gMDLumaCnt = gMDLumaStableCnt = 0;
}

/**
 * @fn                UINT8 mdEdgeTblFill(UINT16 hSize,UINT16 vSize,UINT16 hOffset,UINT16 vOffset,UINT8 hNum,UINT8 vNum)
 * @brief             	md edge table fill data
 *\n                    
 * @param[in]       none
 * @retval            none
 */
UINT8
mdEdgeTblFill(
	UINT16 hSize,
	UINT16 vSize,
	UINT16 hOffset,
	UINT16 vOffset,
	UINT8 hNum,
	UINT8 vNum
)
{
	UINT32 addr;	
	UINT16 i,j;
	UINT16 pos2ndX;
	UINT8 lsb,msb;
	UINT16 idx;
	UINT16 posX[144],posY[144];

	if ( ( hNum*vNum)>144 ) {
		return FAIL;
	}
	
	idx=0;
	/***assgin coordinate to the output of edge window ***/
	addr=K_SDRAM_AviDistEdgeWndAddrA;
	M_DRAM_SetStartAddressV(addr, 0);	
		
	for ( j = 0 ; j < vNum ; j++ ) {
		for ( i = 0 ; i < hNum ; i++ ) {
			posX[idx] = hOffset + (hSize*i) + (hSize>>2);	 /* 1/4 position */
			posY[idx] = vOffset + (vSize*j) + (vSize>>1);
			posX[idx] *= (UINT16)4;
			posY[idx] *= (UINT16)4;
	
			//printf("1 posX=%x\n",posX);
			lsb = (UINT8)(posX[idx]&0xff);
			msb = (UINT8)(posX[idx]>>8);
			M_DRAM_WriteWord(lsb,msb); 	
			lsb = (UINT8)(posY[idx]&0xff);
			msb = (UINT8)(posY[idx]>>8);			
			M_DRAM_WriteWord(lsb,msb); 	
	
			pos2ndX = posX[idx]+(hSize<<1);	/* 3/4 position */ 
			
			lsb = (UINT8)(pos2ndX&0xff);
			msb = (UINT8)(pos2ndX>>8);
			M_DRAM_WriteWord(lsb,msb); 	
			lsb = (UINT8)(posY[idx]&0xff);
			msb = (UINT8)(posY[idx]>>8);			
			M_DRAM_WriteWord(lsb,msb); 	
			
			idx++;	
		}
	}

	idx = 0;
	addr = K_SDRAM_AviDistEdgeWndAddrB;
	M_DRAM_SetStartAddressV(addr, 0);		
	
	for ( j = 0 ; j < vNum ; j++ ) {
		for ( i = 0 ; i < hNum ; i++ ) {

			lsb = (UINT8)(posX[idx]&0xff);
			msb = (UINT8)(posX[idx]>>8);
			M_DRAM_WriteWord(lsb,msb); 	
			lsb = (UINT8)(posY[idx]&0xff);
			msb = (UINT8)(posY[idx]>>8);			
			M_DRAM_WriteWord(lsb,msb); 	

			//posX=hoffset+(hsize*(UINT16)i)+((hsize*3)>>2);   /* 3/4 position */			
			//posX*=(UINT16)4;
			
			pos2ndX = posX[idx] + (hSize<<1);	/* 3/4 position */ 
			
			lsb = (UINT8)(pos2ndX&0xff);
			msb = (UINT8)(pos2ndX>>8);
			M_DRAM_WriteWord(lsb,msb); 	
			lsb = (UINT8)(posY[idx]&0xff);
			msb = (UINT8)(posY[idx]>>8);			
			M_DRAM_WriteWord(lsb,msb); 	
			
			idx++;
		}
	}
		
}

/**
 * @fn                void mdFuncEnter(UINT8 Func)
 * @brief             	md function Enter
 *\n                    
 * @param[in]       none
 * @retval            none
 */
#pragma OT(8,SPEED) // optimize with speed first
UINT8
mdFuncEnter(
	UINT8 vdCnt,
	UINT8 CurrSysMode,
	UINT8 *CurrFuncState
)
{
	UINT8 regVal;
	UINT8 curGId;
	UINT8 panoramaEn;

	vdCnt = vdCnt;

	#if PANORAMA_EN
	panoramaEn = panoramaEnGet();
	#else
	panoramaEn = 0;
	#endif

	if ( panoramaEn ) {
		/* Panorama Flow */

		*CurrFuncState = MOTION_PANORAMA;
		
		if ( CurrSysMode != OS_SYS_MODE_STILL_PV ) {
			return FALSE;
		}

		
	}
	else {
		/* Motion_det Flow */

		*CurrFuncState = MOTION_DETECTION;
		
		if ( !((CurrSysMode == OS_SYS_MODE_STILL_PV) || 
			(CurrSysMode == OS_SYS_MODE_VIDEO_PV) || 
			(CurrSysMode == OS_SYS_MODE_VIDEO_REC)) ) {
			return FALSE;
		}

		if ( gMDYuvSensor == 0 ) {
			sp1kAeStatusGet(SP1K_AE_gId, &curGId);
		}
		
		if ( gDelayCnt > 0 ) {
			gDelayCnt--;
			gMDLastGId = curGId;
			return FALSE;
		}
	}
	
	HAL_CdspVDramSkipWrGet(&regVal);
	
	if ( regVal & 0x01 ) {	/*if 1st path is off, me doesn't work */
		return FALSE;
	}

	if ( panoramaEn ) {
		/*check ME start to done*/
		#if 0
		if ( !panoramaMeDoneGet() ) {
			return FALSE;	
		}
		#endif
	}
	else {
		/*check for ME start*/
		if ( !(XBYTE[REG_Jpeg_Intr_Event] & 0x20) )	{
			return FALSE;		
		}
		
		/*check for ME done*/
		if ( !(XBYTE[REG_Jpeg_Intr_Event]&0x40) ) {
			return FALSE;
		}

		/*clear ME start, ME done interrupt flag */
		XBYTE[REG_Jpeg_Intr_Event] = 0x60; 
	}

	return TRUE;
}


/**
 * @fn                void mdFuncProc(UINT8 Func)
 * @brief             	md function proc
 *\n                    
 * @param[in]       none
 * @retval            none
 */
#pragma OT(8,SPEED) // optimize with speed first
void
mdFuncProc(
	UINT8 Func
)
{
	UINT8 gMVx,gMVy;
	UINT8 gMVxNum,gMVyNum;
	UINT8 curLuma,diff;
	
	switch ( Func ) {
		case MOTION_DETECTION:
			{
				/* Motion_det Flow */
				mdMotionVectorGet(gBlockNumber,5,&gMVx,&gMVy,&gMVxNum,&gMVyNum);
				//printf("mv num x=%bd,y=%bd\n",gMVxNum,gMVyNum);
				
				mdActionStatus = MD_NOT_ACTION;
				
				if ( gLearningFlag == 1 ) {
					mdLearningCtrl(gMVx,gMVy,gMVxNum,gMVyNum);
					mdActionStatus = MD_LEARNING;
					//printf("learning\n");
				}
				else {
				
					if ( gDetectThr < MD_MIN_DETECT_THR ) {
						//printf("ae md\n");

						#if 1	/* Raw/YUV Sensor :  Luma */
							curLuma = aeFlowGetCurLuma();
							
							diff = abs(curLuma,gMDLumaThr);
							
							if ( diff > 10 ) {
								gMDLumaCnt++;
								gMDLumaStableCnt = 0;
							}
							else {
								gMDLumaStableCnt++;
							}

							if ( gMDLumaStableCnt > 40 ) {
								gMDLumaCnt = 0;
								gMDLumaStableCnt = 0;
							}

							mdBar.numeratorMdStatus = ((diff > 128)? 128:diff);
							mdBar.denominatorMdStatus = 128;

							motionPrintf("luma=%bx\n",curLuma);
							motionPrintf("gMDLumaStableCnt=%bd\n",gMDLumaStableCnt);
							
							if ( gMDLumaCnt > gMDLumaDetTime ) {
								gMDLumaCnt = 0;
								gDelayCnt = MD_DELAY_CNT;
								mdActionStatus = MD_ACTION;
							
								osMsgPost(SP1K_MSG_MD_ACTION);					
							}
						
						#else	/* Raw Sensor : AE Index  , YUV Sensor: Luma */
							if (gMDYuvSensor==1){
								curLuma=aeFlowGetCurLuma();
								//printf("luma=%bx\n",curLuma);
								diff=abs(curLuma,gMDLumaThr);

								if (diff > 10){
									gMDLumaCnt++;
								}

								if (gMDLumaCnt>gMDLumaDetTime){
									gMDLumaCnt=0;
									gDelayCnt = MD_DELAY_CNT;

									osMsgPost(SP1K_MSG_MD_ACTION);					
								}
							}
							else{

								motionPrintf("raw ae cur=%bd,pre=%bd\n",curGId,gMDLastGId);
								
								
								if (curGId != gMDLastGId){
									gDetectTime=0;
									gDelayCnt = MD_DELAY_CNT;
									osMsgPost(SP1K_MSG_MD_ACTION);
									//gDetectTime++;
								}
								gMDLastGId=curGId;			
							}
						#endif
					
					}		
					else if ( !mdMotionChk(gMVx,gMVy,gMVxNum,gMVyNum) ) {
						//printf("me md\n");
						gDetectTime = 0;
						gDelayCnt = MD_DELAY_CNT;
						mdActionStatus = MD_NOT_ACTION;
						
						osMsgPost(SP1K_MSG_MD_ACTION);	
						//sp1kMDEnSet(0);	
				
					}	

					motionPrintf("[%bd,%bd],[%bd,%bd]\n",gMVx,gMVy,gMVxNum,gMVyNum);
				}

				/* dynamic adjust tempoal de-noise weight algorithm flow, it run will 4 frame */
				if ( tdAdjustByMdEn == MD_ADJUST_TD_ENABLE ) {
					mdAdjustTempoalDenoiseWeightAlgo(&histMVX[0], &histMVX[0], mdActionStatus, mvXMidNum, mvYMidNum);
				}
			}
			break;
		case MOTION_PANORAMA:
			{
			}
			break;
	}
}



/**
 * @fn                void mdSvcProc(void *cnt)
 * @brief             	md svc proc
 *\n                    
 * @param[in]       none
 * @retval            none
 */
#pragma OT(8,SPEED) // optimize with speed first
void
mdSvcProc(
	void *cnt
)
{
	UINT8 vdCnt = (*(UINT8*)cnt) & 0x03;
	UINT8 CurrSysMode, CurrFuncState;

	CurrSysMode = osSysModeGet();	

	if ( mdFuncEnter(vdCnt, CurrSysMode, &CurrFuncState) ) {
		mdFuncProc(CurrFuncState);
	}
}

/**
 * @fn                void mdMotionVectorGet(UINT8 num, UINT8 blkNumThr, UINT8 *mvX, UINT8 *mvY,UINT8 *mvXNum,UINT8 *mvYNum)
 * @brief             	md Motion Vector Get
 *\n                    
 * @param[in]       none
 * @retval            none
 */
#pragma OT(8,SPEED) // optimize with speed first
void 
mdMotionVectorGet(
	UINT8 num, 
	UINT8 blkNumThr, 
	UINT8 *mvX, 
	UINT8 *mvY,
	UINT8 *mvXNum,
	UINT8 *mvYNum
)
{
	UINT8 i;
	UINT8 *pData;
	#if 0
	UINT8 histMVX[MD_MAX_MV_NUM];
	UINT8 histMVY[MD_MAX_MV_NUM];
	#endif

	UINT8 CurrState;

	CurrState = osSysModeGet();

	//mdMeOutAddrGet(osSysModeGet(),meAddr);
	#if 0
		/****** save mv ******/
		UINT8 filename[ 12 ]="GMV00000BIN";
		UINT8 filename1[ 12 ]="EDGE0000BIN";
		
		
		//filename[ 4 ] = '0' + saveMECnt / 1000;
		//filename[ 5 ] = '0' + (saveMECnt / 100)-((saveMECnt / 1000)*10);
		filename[ 6 ] = '0' + (mdSaveMECnt / 10)-((mdSaveMECnt / 1000)*100)-(((mdSaveMECnt % 1000)/100)*10);
		filename[ 7 ] = '0' + mdSaveMECnt % 10;
		filename[ 11 ] = 0x20;

		filename1[ 6 ] = filename[ 6 ];
		filename1[ 7 ] = filename[ 7 ];
		filename1[ 11 ] = 0x20;

		if (mdSaveMECnt<10){
			dramFileSave(filename, mdMEOutAddr, num<<2);
			if((XBYTE[REG_Dram_MpgSb_CdspWRegIdx]&0x01)==0){
				dramFileSave(filename1, K_SDRAM_AviDistEdgeWndAddrB, num<<3);		
			}
			else{
				dramFileSave(filename1, K_SDRAM_AviDistEdgeWndAddrA, num<<3);		
			}
			
			mdSaveMECnt++;
		}
		/********************/
	#endif

	/*******************************************/
	/* move MV to sram                         */
	/*******************************************/
	//memset(histMVX, 0, (UINT16)MD_MAX_MV_NUM);
	//memset(histMVY, 0, (UINT16)MD_MAX_MV_NUM);
	for ( i = 0 ; i < MD_MAX_MV_NUM ; i++ ) {
		histMVX[i] = 0;
		histMVY[i] = 0;
	}
	
	//linjiecheng fixed 	
	if ( (CurrState == OS_SYS_MODE_STILL_PV) || 
		(CurrState == OS_SYS_MODE_VIDEO_PV && (videoRecStatusGet() == VID_REC_IDLE)) ) {
		//video recording md off used
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(gMEOutAddr);
		HAL_DmaDo(0, 1, ((UINT32)num)<<2, 0, 0);
		pData = (UINT8 xdata *) 0x3800;	/* 1k SRAM */
	}
	else {
		//video recording md on used
		pData = (UINT8 xdata *) 0x3800;	/* 1k SRAM */	
		HAL_DramReadBytesAddr(gMEOutAddr,pData,((UINT32)num)<<2,DRAM_RWFormat_8);
	}	    

	/*******************************************/
	/* Histogram Statistics                         */
	/*******************************************/
	i = 0;		
	do {		
		if ( *pData != 0xff ) {
			histMVY[*(pData+1)+1]++;
		}
		
		if ( (*(pData+2)) != 0xff ) {
			histMVX[*(pData+2)+1]++;
		}
		
		pData += 4;
	}
	while ( ++i < num );

	/*******************************************/
	/* Find X,Y Max Motion Vector                          */
	/*******************************************/
	mdFindMaxMv(histMVX,MD_MAX_MV_NUM-3,blkNumThr,mvX,mvXNum);	
	mdFindMaxMv(histMVY,MD_MAX_MV_NUM-3,blkNumThr,mvY,mvYNum);

	mvXMidNum = *mvXNum;//x32 cnt number
	mvYMidNum = *mvYNum;//y32 cnt number

	//printf("blkNumThr %bu,%bu,%bu\n",blkNumThr,mvX,mvXNum);
	//printf("blkNumThr %bu,%bu,%bu\n",blkNumThr,mvY,mvYNum);
	
	
}//gMvGet function End

/**
 * @fn                void mdFindMaxMv(UINT8 *mvPt,UINT8 blockNum,UINT8 thr,UINT8 *maxMv,UINT8 *maxMvNum)
 * @brief             	find md max MV
 *\n                    
 * @param[in]       none
 * @retval            none
 */
void
mdFindMaxMv(
	UINT8 *mvPt,
	UINT8 blockNum,
	UINT8 thr,
	UINT8 *maxMv,
	UINT8 *maxMvNum
)
{
	UINT8 mv1stMax,mv2ndMax;
	UINT8 mv0Num,mv1Num,mv2Num;
	UINT8 mv1stMaxSum,mv2ndMaxSum,mvCurSum;
	UINT8 i;

	mv1stMaxSum = mv2ndMaxSum = 0;
	i = 1;
	mv0Num = *mvPt++;
	mv1Num = *mvPt++;
	mv2Num = *mvPt++;
	do {
		mvCurSum	 = mv0Num + mv1Num + mv2Num;    
		if ( (mv1Num >= mv2Num) && (mv1Num >= mv0Num) && (mvCurSum>mv2ndMaxSum) ) {
			if ( mv1stMaxSum < mvCurSum ) {
				mv1stMaxSum = mvCurSum;
				if ( mv1stMax != i ) {
					mv2ndMax  = mv1stMax;	/* back up the second max Mv */
					mv1stMax  = i;			/* record the max Mv(updated the max MV) */
				}
			}
			else if ( (i-mv1stMax) > 1 ) {
				mv2ndMaxSum = mvCurSum;
				mv2ndMax = i;	/* the max Mv needn't update, but the second max Mv need update	*/		
			}
		}
		mv0Num = mv1Num;
		mv1Num = mv2Num;
		mv2Num = *mvPt++;
	}
	while ( ++i < blockNum );
	
	motionPrintf("New 1stMax=%bx,2ndMax=%bx,1stSum=%x,2ndSum=%x\n",Mv1stMax,Mv2ndMax,sum_num_mv1stMax,sum_num_mv2stMax);
	*maxMv = mv1stMax-1;/*because input is (mv+1)*/
	*maxMvNum = mv1stMaxSum;	
	
	if ( (mv1stMaxSum==mv2ndMaxSum) && (mvPt[mv2ndMax]>mvPt[mv1stMax]) ) {
		*maxMv = mv2ndMax-1;
		*maxMvNum = mv2ndMaxSum;
	}	
	
	if ( mv1stMaxSum < thr ) {
		*maxMv = 32;
	}

	motionPrintf("maxMv=%bd,num=%bd\n",*maxMv,mv1stMaxSum);
}

/**
 * @fn                void mdMeOutAddrGet(UINT8 mode,UINT32 *meAddr)
 * @brief             me output address get
 *\n                    
 * @param[in]       none
 * @retval            none
 */
void
mdMeOutAddrGet(
	UINT8 mode,
	UINT32 *meAddr
)
{
	motionPrintf("os mode=%bx\n",mode);
	if ( mode == OS_SYS_MODE_VIDEO_PV ) {// Video Pv/Record
		*meAddr = K_SDRAM_AviDistMeAddr;
	}
	else {// Still Pv
		*meAddr = K_SDRAM_PvDistMeAddr;
	}
	
}

/**
 * @fn                void mdEdgeTblAddrGet(UINT8 mode,UINT32 *edgeWinAddrA,UINT32 *edgeWinAddrB)
 * @brief             	md edge table address get
 *\n                    
 * @param[in]       none
 * @retval            none
 */
void
mdEdgeTblAddrGet(
	UINT8 mode,
	UINT32 *edgeWinAddrA,
	UINT32 *edgeWinAddrB
)
{
	if ( mode == OS_SYS_MODE_VIDEO_PV ) {	/* Video Pv */
		*edgeWinAddrA = K_SDRAM_AviDistEdgeWndAddrA;
		*edgeWinAddrB = K_SDRAM_AviDistEdgeWndAddrB;
	}
	else {		/* Still Pv */
		*edgeWinAddrA = K_SDRAM_PvDistEdgeWndAddrA;
		*edgeWinAddrB = K_SDRAM_PvDistEdgeWndAddrB;
	}
}

/**
 * @fn                void mdEnSet(UINT8 en);
 * @brief	            To Set MD On/off Status function.
 * @param[in]      en : (The status of MD)
 *\n                              MD_DISABLE: MD turn OFF.
 *\n                              MD_ENABLE: MD turn ON.
 * @retval            none          
 */
void 
mdEnSet(
	UINT8 en
)
{
	switch ( en ) {
		case MD_ENABLE:
			/* md initial */
			//sp1kMDInit();//linjiecheng mask this, initial need the first
			HAL_DistEn(0xff);
			
			sp1kVdProcEnable(VD_PROC_MD,1);
			HAL_CdspVDramSkipWrSet(0xf6,0x00); /* don't skip edge window */
			break;
		case MD_DISABLE:
			HAL_DistEn(0x00);
			sp1kVdProcEnable(VD_PROC_MD,0);	
			HAL_CdspVDramSkipWrSet(CDSP_SKIP_DISTEDGE,0xff); /* skip edge window */
			break;
		default:
			ASSERT(0,0);
			break;
	}

	gMDEn = en;
	gDelayCnt = MD_DELAY_CNT;
	gDetectTime = 0;
	
}

/**
 * @fn                UINT8 mdEnGet(void);
 * @brief	            To Get MD On/off Status function.
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8
mdEnGet(
	void
)
{
	return gMDEn;
}

/**
 * @fn                void mdSenSet(UINT8 sen);
 * @brief	            To Set MD Sensitivity function.
 * @param[in]      sen : (The status of MD Sensitivity)
 *\n                              MD_SEN_LOW: low Sensitivity.
 *\n                              MD_SEN_MIDDLE: middle Sensitivity.
 *\n                              MD_SEN_HIGH: high Sensitivity.
 * @retval           none
 */
void 
mdSenSet(
	UINT8 sen
)
{
	switch ( sen ) {
		case MD_SEN_HIGH:
			gDetectTimeThr = 2;
			gMDLumaDetTime = 1;
			break;
		case MD_SEN_MIDDLE:
			gDetectTimeThr = 5;
			gMDLumaDetTime = 2;
			break;
		case MD_SEN_LOW:		
		default:
			gDetectTimeThr = 8;
			gMDLumaDetTime = 3;
			break;	
	}
	
	gMdSensitivity = sen;
	
}

/**
 * @fn                UINT8 mdSenGet(void);
 * @brief	            To Get MD Sensitivity function.
 * @param[in]      none
 * @retval           The status of MD Sensitivity:
 *\n                              MD_SEN_LOW: low Sensitivity.
 *\n                              MD_SEN_MIDDLE: middle Sensitivity.
 *\n                              MD_SEN_HIGH: high Sensitivity.
 */
UINT8  
mdSenGet(
	void
)
{
	return gMdSensitivity;
}

/**
 * @fn                UINT8 mdMotionChk(UINT8 maxMVx,UINT8 maxMVy,UINT8 maxMVxNum,UINT8 maxMVyNum);
 * @brief	            md motion detect check
 * @param[in]      none
 * @retval           none
 */
UINT8
mdMotionChk(
	UINT8 maxMVx,
	UINT8 maxMVy,
	UINT8 maxMVxNum,
	UINT8 maxMVyNum
)
{
	
	
	if ( (maxMVx != ME_H_HALF_PIXEL_NONE) || (maxMVy != ME_V_HALF_PIXEL_NONE) ) {
		gDetectTime++;
	}
	else if ( (maxMVxNum <= gDetectThr) && (maxMVyNum <= gDetectThr) ) {
		gDetectTime++;
	}
	else {
		gDetectTime = 0;
	}
	
	mdBar.numeratorMdStatus = (EDGE_TBL_HBLOCKNUM * EDGE_TBL_VBLOCKNUM * 2) - (maxMVyNum + maxMVxNum);
	mdBar.denominatorMdStatus = (EDGE_TBL_HBLOCKNUM * EDGE_TBL_VBLOCKNUM * 2);

    motionPrintf("cnt=%bd\n",gDetectTime);
	motionPrintf("thr=%bd\n",gDetectThr);
	
	if ( gDetectTime > gDetectTimeThr ) {
		gDetectTime = 0;
		
		motionPrintf("detected\n");
		return SUCCESS;
	}
	else {
		return FAIL;
	}


	#if 0	
		maxMVxNum=maxMVxNum;
		maxMVyNum=maxMVyNum;

		if ((maxMVx != ME_H_HALF_PIXEL_NONE)&&(maxMVy != ME_V_HALF_PIXEL_NONE)){
			if(mdDetectTime < 255){
				mdDetectTime ++ ;
			}
		}				

		if (mdDetectTime > mdDetectTimeThr){		
			mdDetectTime=0;
			return SUCCESS;
		}
	#endif

	return FAIL;
}

/**
 * @fn                void mdLearningStart(UINT8 start);
 * @brief	            Set MD Learning function.
 * @param[in]      start : MD Learning function enable/disable
 *\n                              MD_LEARNING_ENABLE: Learning function enable.
 *\n                              MD_LEARNING_DISABLE: Learning function disable.
 * @retval           none
 */
void
mdLearningStart(
	UINT8 start
)
{
	if ( start == MD_LEARNING_ENABLE ) {
		gLearningFlag = 1;
		gLearningCnt = 0;
		gLearningSum = 0;
		gMDLearnLumaSum = 0;
	}
	else {
		gLearningFlag = 0;
	}
}

/**
 * @fn                void mdLearningCtrl(UINT8 maxMVx,UINT8 maxMVy,UINT8 maxMVxNum,UINT8 maxMVyNum);
 * @brief	            MD Learning ctrl flow
 * @param[in]      none
 * @retval           none
 */
void
mdLearningCtrl(
	UINT8 maxMVx,
	UINT8 maxMVy,
	UINT8 maxMVxNum,
	UINT8 maxMVyNum
)
{
	UINT8 luma;
	
	if ( (gLearningCnt < MD_LEARNING_NUM) && (maxMVx==ME_H_HALF_PIXEL_NONE) && (maxMVy==ME_V_HALF_PIXEL_NONE) ) {		
		gLearningSum+= ((maxMVxNum+maxMVyNum)>>1);

		luma = aeFlowGetCurLuma();
		gMDLearnLumaSum += luma;

		gLearningCnt ++;
	}

	if ( gLearningCnt >= MD_LEARNING_NUM ) {
	 	gDetectThr = (UINT8)(gLearningSum/MD_LEARNING_NUM);
	 	gMDLumaThr = (UINT8)(gMDLearnLumaSum/MD_LEARNING_NUM);
	 	motionPrintf("motion thr=%bd\n",gDetectThr);
	 	motionPrintf("motion luma thr=%bx\n",gMDLumaThr);

	 	switch ( sp1kMDSenGet() ) {
			case MD_SEN_MIDDLE:
				if ( gDetectThr > 5 ) {
					gDetectThr -= 5;
				}
				break;
			case MD_SEN_LOW:	
				if ( gDetectThr > 10 ) {
					gDetectThr -= 8;
				}
				break;	
			case MD_SEN_HIGH:
			default:
			break;
	 	}

	 	gLearningFlag = 0;
	 	motionPrintf("motion thr=%bd\n",gDetectThr);
	}

	
}

/**
 * @fn                void mdAdjustTempoalDenoiseWeightEnSet(	UINT8 en)
 * @brief	            md adjust Tempoal Denoise Weight ctrl flow Enable/disable
 * @param[in]     en   MD_ADJUST_TD_DISABLE, MD_ADJUST_TD_ENABLE
 * @retval           none
 */
void 
mdAdjustTempoalDenoiseWeightEnSet(
	UINT8 en
)
{
	if (0x1521 == pkgChipIdGet()) {
		tdAdjustByMdEn = en;
	}
}

/**
 * @fn                void mdAdjustTempoalDenoiseWeightAlgo(UINT8* phistXArray,UINT8* phistYArray,UINT8 mdActionSts,UINT8 mvXMidCntNum,UINT8 mvYMidCntNum);
 * @brief	            MD Learning ctrl flow
 * @param[in]      phistXArray : Histogram of x mv
 *\n                   phistYArray : Histogram of y mv
 *\n                   mdActionSts : 0: MD_NOT_ACTION, 1:MD_ACTION, 2:MD_LEARNING
 *\n                   mvXMidCntNum : mvX32 count number
 *\n                   mvYMidCntNum : mvY32 count number
 * @retval           none
 */
void 
mdAdjustTempoalDenoiseWeightAlgo(
	UINT8* phistXArray,
	UINT8* phistYArray,
	UINT8 mdActionSts,
	UINT8 mvXMidCntNum,
	UINT8 mvYMidCntNum
)
{
	/* Note : Add Ctrl Tempoal Denoise Weight Algorithm */
	UINT8 level;
	UINT8 tdlowyval;
	UINT8 tdhighyval;
	UINT8 tdlowythr;
	UINT8 tdlowuvval;
	UINT8 tdhighuvval;
	UINT8 tdlowuvthr;
	UINT8 tdywid,tduvwid;

	phistXArray = phistXArray;
	phistYArray = phistYArray;
	mdActionSts = mdActionSts;
	mvXMidCntNum = mvXMidCntNum;
	mvYMidCntNum = mvYMidCntNum;

	/* Tempoal Denoise Level */
	switch ( mdActionSts ) {
		case MD_NOT_ACTION:
			level = 2;
			break;
		case MD_ACTION:
			level = 0;
			break;
		case MD_LEARNING:
			level = 3;
			break;
	}

	/* Tempoal Denoise Parameter */
	tdlowyval = 0x18;
	tdhighyval = 0xff;
	tdlowythr = 0x00;
	tdlowuvval = 0x18;
	tdhighuvval = 0xff;
	tdlowuvthr = 0x00;

	/* Tempoal Denoise width */
	switch (level){
		case 0:
			tdywid = 3;
			break;
		case 1:
			tdywid = 4;
			break;
		case 2:
			tdywid = 5;
			break;
		default:
			tdywid = XBYTE[ REG_Cdsp_3Dtdywid ];
			break;
	}

	#if 1
	//linjiecheng algo.
	tduvwid = tdywid;// << 1;
	#else
	//algo. tream
	tduvwid = tdywid << 1;
	#endif
	
	//printf("level=%bu,tdywid=%bu,tduvwid=%bu\n",level,tdywid,tduvwid);

	cdspPvDynamicTempDenoiseParaSet(tdlowyval, tdhighyval, tdlowythr, tdywid, tdlowuvval, tdhighuvval, tdlowuvthr, tduvwid);
}

/**
 * @fn                MD_BAR* mdBarGet (void)
 * @brief	            MD Bar value get
 * @param[in]      NULL
 * @retval           typedef struct _MD_BAR
 */
MD_BAR* mdBarGet (
	void
)
{
	return &mdBar;
}
#endif
