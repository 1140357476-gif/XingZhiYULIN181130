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
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "fd_def.h" 
#include "fd_debug.h"
#include "reg_def.h"

#if FD_ENABLE
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_FD_SMILE__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//UINT8 xdata fdDbgSubSize[6];
//UINT8 xdata* fdDbgSubPosX=fdDbgSubSize+2;//[2];
//UINT8 xdata* fdDbgSubPosY=fdDbgSubSize+4;//[2];
UINT16 fdDbgHSize;
UINT8 fdDbgX;
UINT8 fdDbgY;

UINT8 fdDbgSmileCnt=0;


UINT8 fdSubCnt0;
UINT8 fdSubCnt1;
UINT8 fdDbLast=0;
UINT8 fdSmileEn=0;
UINT8 fdSubStep=0;
UINT8 fdSubCnt = 2;

UINT16 smileSrcHSize1st;
UINT16 smileSrcVSize1st;
UINT16 smileSrcHSize2st;

#if (USE_ARRAY_MACROS == 1)
extern UINT8 jumpIdx;
extern code UINT8 fdStepVSizeArray[];
extern code UINT8 fdStepRadiusArray[];
extern code UINT16 fdStepAreaArray[];
extern code UINT8 fdStepFacArray[];
#endif

#if (FD_SAVE_SMILE_II_DBG == 1)
extern UINT8 fdSaveSmileIIDbgPath;
#endif


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn		  UINT8 fdGroupSub(UINT8 isSnap)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
fdGroupSub(
	UINT8 isSnap
)
{
	UINT8* pt = G_ucStorData;
	UINT32 addr = fdOutAddr+FD_SIZE_RstFace/2;
	
	UINT16 k;

	UINT8 keyCnt0=0;
	UINT8 keyCnt1=0;
	
	UINT16 cnt = FD_SIZE_RstSub*2;
	UINT16 key = FD_SIZE_RstSub;


	UINT8 step = 0;
	UINT8 m,n=0;
	UINT8 x,y,r;

	UINT16 x1=0,y1=0,r1=0,c1=0;
	UINT16 x0=0,y0=0,r0=0,c0=0;
	UINT16 f1 = fdDbgHSize/2;
	UINT8 f2 = 160;
#if 0
	UINT8 *hsizes = (!fdDbLast)?
		(fdHSizeFace+FD_DECTED_Cnt):
		(fdHSizeFace+FD_DECTED_Cnt+FD_DECTED_Cnt);
		
	fdDbLast = (fdJobAddrSub==FD_ADDR_QueueSub0)?0:1;
	//UINT8 rst=0;
#else
	UINT8 *hsizes;

	hsizes = fdHSizeSub;
#endif
	/*
	fdDbgSubPosX[0] = 0;
	fdDbgSubPosY[0] = 0;
	fdDbgSubSize[0] = 0;
	fdDbgSubPosX[1] = 0;
	fdDbgSubPosY[1] = 0;
	fdDbgSubSize[1] = 0;
	*/	
	
	if (fdKeyStep>=FD_DECTED_Cnt) {	
		fdSubStep = 0;
		return 0;
	}	

	HAL_DmaSetDramAddr(addr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(0, 1, cnt, 1, 0);
	//fdGroupSubPrintf("dma1-%lx,%u\n",addr,cnt);
	HAL_GprmDramFillExt(addr << 1, cnt, 0, 0, 0);


	for (k=0;k<cnt;k+=2) {
		if (pt[k] && pt[k+1]) {			
			if (k>key) {
				keyCnt1++;

				m = (k-key)>>4;
			}
			else {
				keyCnt0++;
				m = k>>4;
			}
			step += m;
			//f2=fdGetHSizeSub(m);//fdGetHSize(m);
			//r=fdGetRadiusSub(m);
			f2 = hsizes[m];
			#if (USE_ARRAY_MACROS == 1)
			r = fdStepRadiusArray[m + jumpIdx];
			#else
			r = fdStepRadius(f2);
			#endif
			r = (UINT16)r;//*f1/160;
			x = (UINT16)pt[k]*160/f2+r;// +fdKeyRtX;
			y = (UINT16)pt[k+1]*160/f2+r;// +fdKeyRtY+(UINT16)fdKeyRtS/2;
		
			if (	r0==0 //first
				||  (x>=x0-r0 	&& x<=x0+r0 	&&  y>=y0-r0 	&& y<=y0+r0)
				||  (x0>=x-r 		&& x0<=x+r	&&  y0>=y-r 		&& y0<=y+r)
					) {
				x0+=x;
				y0+=y;
				r0+=r;
				c0++;
			}		
			else if (	 r1==0//first
				||(	x>=x1-r1 	&& x<=x1+r1 	&&  y>=y1-r1 	&& y<=y1+r1)
				||(	x1>=x-r 		&& x1<=x+r	&&  y1>=y-r 		&& y1<=y+r)
					) {
				x1+=x;
				y1+=y;
				r1+=r;
				c1++;
			}
					
			n++;
			if (n>=20) {
				break;
			}
		}
	}

	if (n) {
		step/=n;
		if (fdDbgSmileCnt) {
			if (fdDbgSmileCnt!=0xff) {
				fdDbgSmileCnt--;
			}
			fdGroupSubPrintf("#%bx#%bx#%bu#%bu\n",keyCnt0,keyCnt1,fdKeyStep,step);
		}	
	}
	else {		
		fdSubCnt0 = 0;
		fdSubCnt1 = 0;
		fdSubStep = 0;
		//fdSysInfo("No\n");
		return 0;
	}

	x = fdKeyRtX;
	y = fdKeyRtY;
	r = fdKeyRtS;

	fdSubCnt0 = keyCnt0;
	fdSubCnt1 = keyCnt1;
	
	if (c0) {
		r0 = (UINT32)r0*f1/160;
		x0 = (UINT32)x0*f1/160;
		y0 = (UINT32)y0*f1/160;
		
		r0/=c0;
		x0/=c0;
		y0/=c0;
		x0+=fdDbgX;
		y0+=fdDbgY;

		//r0=r0*fdDbgSubK/64;
		//x0=x0*fdDbgSubK/64+fdKeyRtX ;
		//y0=y0*fdDbgSubK/64+fdKeyRtY;
		
		//if (x0>=x && y0>=y && x0<x+r &&  y0<y+r) {
		//	fdDbgSubPosX[0] = (x0-r0);//*r/160+x;
		//	fdDbgSubPosY[0] = (y0-r0);//*r/160+y;
		//	fdDbgSubSize[0] = (r0<<1);//*r/160;
		//}
		
		//fdDbgSubPosX[0] = (x0-r0)/c0;
		//fdDbgSubPosY[0] = (y0-r0)/c0;
		//fdDbgSubSize[0] = (r0<<1)/c0;
	}	
	
	if (c0 && c1 ) {
		r1 = (UINT32)r1*f1/160;
		x1 = (UINT32)x1*f1/160;
		y1 = (UINT32)y1*f1/160;
		
		r1/=c1;
		x1/=c1;
		y1/=c1;
		x1+=fdDbgX;
		y1+=fdDbgY;
		//r1=r1*fdDbgSubK/64;
		//x1=x1*fdDbgSubK/64+fdKeyRtX ;
		//y1=y1*fdDbgSubK/64+fdKeyRtY;
		
		if (!(	  (	x0>=x1-r1 	&& x0<=x1+r1 	&&  y0>=y1-r1 	&& y0<=y1+r1)
			||(	x1>=x0-r0 	&& x1<=x0+r0		&&  y1>=y0-r0 	&& y1<=y0+r0)
			)) {
			
			//if (x1>=x && y1>=y && x1<x+r && y1<y+r) {	
			//	fdDbgSubPosX[1] = (x1-r1);//*r/160+x;
			//	fdDbgSubPosY[1] = (y1-r1);//*r/160+y;
			//	fdDbgSubSize[1] = (r1<<1);//*r/160;
			//}
			
			//fdDbgSubPosX[1] = (x1-r1)/c1;
			//fdDbgSubPosY[1] = (y1-r1)/c1;
			//fdDbgSubSize[1] = (r1<<1)/c1;
		}
	}

	//fdSysInfo("@%bu,$%bu\n",keyCnt0,keyCnt1);
	
	/* key face group result */
	#if 0
	/* old database threshold */
	if (		(keyCnt0>1 && keyCnt1==0)
		||	(keyCnt0>6 &&  keyCnt1<2)) {
	#else
	/* new database threshold */
	if (keyCnt0 > keyCnt1) {		
	#endif
		UINT8 xdata* fdRtShow	=fdRtSize+FD_FACE_CNT*3; 
				
		fdRtShow[fdKeyIdx]|=FD_SHOW_Smile;

		if (isSnap) {
			if (++fdSubStep<fdSubCnt) {
				return 0;
			}
			
			fdSubStep = 0;
			//fdSysInfo("SNAP!\n");
			osMsgPost(SP1K_MSG_FD_SMILE);	
		}
		return keyCnt0-keyCnt1;
	}	
	else {
		fdSubStep = 0;
	}	
	return 0;
}

/**
 * @fn		  void fdProcSmileQueue(void)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
fdProcSmileQueue(
	void
)
{
	UINT32 queue = FD_ADDR_QueueSub0;
	UINT8 cdspIdx;
	UINT8 cdspPath = 1;
	UINT16 hsize;
	UINT32 addr;
	UINT16 s = fdKeyRtS;
	UINT16 x = fdKeyRtX;
	UINT16 y = fdKeyRtY+s/3;//s/2;
	UINT16 yy = s*3/4;
	UINT16 xw,yw;
	//UINT8 queIdx = 0;

	#if (FD_SAVE_SMILE_II_DBG == 1)
	fdSaveSmileIIDbgPath = 1;
	#endif
	
	if ( fdBlinkAddr == 0 ) {
			s = s*4/3;
			if ( s > 120 ) {
				s = 120;
			}
			yy = s;
	}

	if ( y + yy > 120 ) {
		y = 120 - yy;
	}

	if ( fdBlinkEn ) {
		y = fdKeyRtY;
	}

	if ( fdRotate == 1 ) {
		/* 90 degree */
		x = ( fdKeyRtX > s/2 ) ? ( fdKeyRtX - s/2 ) : 0;
		y = fdKeyRtY - s/8;
	}
	else if ( fdRotate == 2 ) {
		/* 270 degree */
		x = ( fdKeyRtX + s + s/2 > 160 ) ? ( 160 - s ) : ( fdKeyRtX + s/2 );
		y = fdKeyRtY - s/8;
	}
	
	//HAL_FdTagEnable(1,1);
	fdJobAddrSub = FD_ADDR_QueueSub0;
	fdHSizeSub = fdHSizeFace + FD_DECTED_Cnt;
	#if (USE_ARRAY_MACROS == 1)
	jumpIdx = FD_DECTED_Cnt;
	#endif
	
	if ( s < 40 ) {//160
		if ( s < 30 ) {//120
			fdJobAddrSub = FD_ADDR_QueueSub1;
			fdHSizeSub = fdHSizeFace + FD_DECTED_Cnt + FD_DECTED_Cnt;
			#if (USE_ARRAY_MACROS == 1)
			jumpIdx = FD_DECTED_Cnt+FD_DECTED_Cnt;
			#endif
		}
		else {
		}
		
		//cdspPath = 1;

		s = 40;
		
		if ( x + s >= 160 ) {
			x = 160 - s;
		}
		
		if ( y + yy >= 120 ) {
			y = 120 - yy;			
		}
	}
	else if ( s < 80 ) {//320
		//cdspPath = 1;		
	}
	else {//>320
		cdspPath = 2;	
	}

	#if (DYNAMIC_SMILE_CTRL_SRC == 2)
	cdspPath = 1;
	#endif
	
	fdDbgHSize = s*2;//640;
	fdDbgX = x;
	fdDbgY = y;

	cdspIdx = HAL_CdspWriteBufIdxGet();

	if (fdBlinkEn && fdBlinkAddr) {
		addr = fdBlinkAddr;
		cdspPath = 1;
	}
	else {
		addr= fdGetCdspBuf(cdspIdx,cdspPath);
		//printf("addr=%lx,path=%bu\n",addr,cdspPath);
	}
		
	switch (cdspPath) {
		case 1:
			hsize = smileSrcHSize1st;
			switch ( hsize ) {
				case 640:
					x <<= 2; 
					y <<= 2; 
					s <<= 2;
					break;
				default:
					x = (UINT16)((UINT32)x*smileSrcHSize1st/160);
					y = (UINT16)((UINT32)y*smileSrcVSize1st/120);
					s = (UINT16)((UINT32)s*smileSrcVSize1st/120);
					break;
			}
			break;
		case 2:
			hsize = smileSrcHSize2st;
			x <<= 1; 
			y <<= 1; 
			s <<= 1;
			break;
		default:
			ASSERT(0,0);
			break;
	}

	#if (FD_SAVE_SMILE_II_DBG == 1)
	fdSaveSmileIIDbgPath = cdspPath;
	#endif

	/* save smile yuv Src */
	fdSaveSmileYuvSrc(addr);

	xw = s;
	yw = (UINT16)((UINT32)s*3/4);

	//printf("%d,%d,%d,%d,%d\n",hsize,x,y,xw,yw);

	//printf("fdIISrcAddr=%lx,fdSIIAddr=%lx,fdJobAddrSub=%lx\n",fdIISrcAddr,fdSIIAddr,fdJobAddrSub);

	HAL_FdIISIIQueue( 
		hsize,
		x, y, xw, yw,
		160, 120,
		addr, fdIISrcAddr, fdSIIAddr,
		G_ucStorData,
		fdJobAddrSub
	);	

}

/**
 * @fn		  UINT8 fdProcSmile(void)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
fdProcSmile(
	void
)
{
	//memset(fdDbgSubSize,0,6);	
	
	if (fdKeyStep>=FD_DECTED_Cnt || (!fdKeyRtS) ) {
		return FALSE;
	}
	
	fdProcSmileQueue();

	//printf("\nsmile II tg jumpIdx=%bx\n",jumpIdx);
	
	if (HAL_FdIITrig(fdJobAddrSub,1000)) {
		iiDbgTrig = II_Trigger_Fd;
	}
	
	if (fdFlow!=FD_FLOW_Smile) {
		HAL_FdSetDataBase(FD_ADDR_DBSub0);
		HAL_FdWinStepSet(FD_SMILE_SUB_WIN_STEP);
		HAL_FdStage1Set(fdDbgStage1);
		HAL_FdBufMaxCntSet(FD_DECTED_OutCntSub);

		fdFlow = FD_FLOW_Smile;
	}	

	if (!(fdBlinkEn && fdBlinkAddr)) {
		fdGroupSub(1);
	}

	return TRUE;
}

#if 0
/**
 * @fn		  void fdProcNonSmileIntr(void)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
fdProcNonSmileIntr(
	void
)
{
	HAL_FdSetDataBaseIntr(FD_ADDR_DBSub1);
	//HAL_FdStage1Set(fdDbgStage2);
	XBYTE[REG_FD_StageMax] = fdDbgStage2;
	fdFlow = FD_FLOW_NonSmile;
	//fdStep = 0;
}
#else
/**
 * @fn		  void fdProcNonSmile(void)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
fdProcNonSmile(
	void
)
{
	UINT8 iihsize = fdHSizeSub[0];
	#if (USE_ARRAY_MACROS == 1)
	UINT8 iivsize = fdStepVSizeArray[0+jumpIdx];
	#else
	UINT8 iivsize = fdStepVSize(iihsize);
	#endif

	/* save smile ii data */
	fdSaveSmileII();
	
	if (fdFlow!=FD_FLOW_NonSmile) {
		HAL_FdSetDataBase(FD_ADDR_DBSub1);
		HAL_FdStage1Set(fdDbgStage2);
		
		fdFlow = FD_FLOW_NonSmile;
	}
	
	HAL_FdSet(
		iihsize,//iiaddr,//(w)
		fdHSizeSub[0],//iihsize,
		fdOutAddr+FD_SIZE_RstFace/2+FD_SIZE_RstSub/2,//fdoutaddr,//(w)
		#if (USE_ARRAY_MACROS == 1)
		fdStepFacArray[0+jumpIdx],//scalefactor
		fdStepAreaArray[0+jumpIdx]//areafactor
		#else
		fdStepFac(iihsize),//scalefactor
		fdStepArea(iihsize)//areafactor
		#endif
	);
	
	HAL_FdRangeSet(
		0,0,//strx, stry, 
		iihsize,iivsize//endx, endy
	);

	//printf("\nnon smile FD tg jumpIdx=%bx\n",jumpIdx);
	
	HAL_FdTrig(0);

	//if (fdDbgSaveIdx) {
	//	UINT8 fn[4];// = "fd00";				
	//	sprintf(fn,"II%bu",fdDbgSaveIdx--);					
	//	dramFileSave(fn,fdIISrcAddr,0x12C00);
	//}
}
#endif
#endif

/**
 * @fn		  UINT8 sp1kFdSetSmile(UINT8 en)
 * @brief	  	 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
sp1kFdSetSmile(
	UINT8 en
)
{
#if FD_ENABLE

	fdSysInfo("smile:%bx\n",en);

	if (en==0xff) {
		return fdSmileEn;
	}
	
	fdSmileEn = en;
	
	if (en) {
		sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub0<<1);
		sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub1<<1);

		#if (DYNAMIC_SMILE_CTRL_SRC == 1)
		WRITE8(&smileSrcHSize1st,1,XBYTE[REG_Dram_MpgSrcHSize_Low]);
		WRITE8(&smileSrcHSize1st,0,XBYTE[REG_Dram_MpgSrcHSize_High]);
		WRITE8(&smileSrcVSize1st,1,XBYTE[REG_Dram_MpgSrcVSize_Low]);
		WRITE8(&smileSrcVSize1st,0,XBYTE[REG_Dram_MpgSrcVSize_High]);
		WRITE8(&smileSrcHSize2st,1,XBYTE[REG_Dram_DispSb_HSize_Low]);
		WRITE8(&smileSrcHSize2st,0,XBYTE[REG_Dram_DispSb_HSize_High]);
		#else
		smileSrcHSize1st = 640;
		smileSrcHSize2st = 320;
		#endif
	}	

	fdSubStep=0;
	//memset(fdDbgSubSize,0,6);

	return fdSmileEn;
#else
	en = en;
	return 0;
#endif	
}


