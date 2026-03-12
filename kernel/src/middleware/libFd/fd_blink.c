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
#include "sp1k_fd_api.h"
#include "fd_debug.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_FD_BLINK__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 fdBlinkEn=0; 
UINT32 fdBlinkAddr=0;
UINT8 blinkDetectSts=0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn		  void pbFDProcFace(UINT32 addrQVGA)
 * @brief	  
 * @param	  [in] 
 * @retval	  
 * @author	  wangjg
 * @since	  2010-6-21
*/
void 
pbFDProcFace(
	UINT32 addrQVGA
)
{
#if 0
	//UINT8 cdspIdx = HAL_CdspWriteBufIdxGet();
	UINT32 dbData;
	UINT32 t0,t1;
	UINT32 timeInterval = 500;
	HAL_DramSetAddr(fdTblAddr, 1);
	HAL_DramRead32(&dbData);
	
	if (dbData!=fdDBData) {//dram data modify
		sp1kFdReset();
		//return;
	}
	
	HAL_DramSetAddr(fdJobAddr+7, 0);
	HAL_DramWrite32(addrQVGA);
	HAL_DramSetAddr(fdJobAddr+7+16, 0);
	HAL_DramWrite32(addrQVGA);

	if (fdFlow!=FD_FLOW_Face) {		
		HAL_FdSetDataBase(FD_ADDR_DBFace);
		HAL_FdWinStepSet(1);
		HAL_FdStage1Set(fdDbgStage0);
		HAL_FdBufMaxCntSet(FD_DECTED_OutCnt);
		HAL_FdTagEnable(0,0);

		fdFlow = FD_FLOW_Face;
	}
	
	if (HAL_FdIITrig(fdJobAddr,0)) {
		iiDbgTrig = II_Trigger_Fd;
	}

	HAL_GlobalReadGTimer(&t1);
	t0=t1+timeInterval;
	
	while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);

	fdGroup();
#else
	addrQVGA = addrQVGA;
#endif
}

/**
 * @fn		  UINT8 sp1kPbFdProcBlink(UINT32 addrQVGA, UINT32 addrVGA)
 * @brief	  
 * @param	  [in] 
 * @retval	  
 * @author	  wangjg
 * @since	  2010-6-21
*/
UINT8 
sp1kPbFdProcBlink(
	UINT32 addrQVGA, 
	UINT32 addrVGA
)
{	
#if 0
	UINT8 ret = 0, count=0;
	UINT32 timeInterval = 500;
	UINT8 smileEn = fdSmileEn;
	fdSmileEn = 1;
	fdBlinkEn = 1;	
	fdBlinkAddr = addrVGA;
	fdSubCnt0 = 0;
	fdSubCnt1 = 0;
	
	sp1kFdReset();
	sp1kNandRsvRead(FD_ID_Eye, FD_ADDR_DBSub0<<1);
	sp1kNandRsvRead(FD_ID_CloseEye, FD_ADDR_DBSub1<<1);

	while (((!fdFaceNum) || fdKeyStep>=FD_DECTED_Cnt )&& count<3 ) {
		fdStep = 0;
		pbFDProcFace(addrQVGA);
		count++;
	}

	if ((fdFaceNum==0) || fdKeyStep>=FD_DECTED_Cnt || count == 3 ) {
		return FALSE;	
	}

	fdStep = 0;
	if (fdProcSmile()) {
		UINT32 t0,t1;	
		
		HAL_GlobalReadGTimer(&t1);
		t0=t1+timeInterval;
		while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);

		if (t1<t0) {
			fdStep = 0;
			fdProcNonSmile();
		
			HAL_GlobalReadGTimer(&t1);
			t0=t1+timeInterval;
			while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);

			if (t1<t0) {
				fdGroupSub(0);
				if (fdSubCnt0<fdSubCnt1 ) {
					ret = TRUE;
				}
			}
			else {
				ret = FALSE;
			}
		}
		else {
			ret = FALSE;
		}
		
	}
	else {
		ret = FALSE;
	}
	fdSmileEn = smileEn;
	fdBlinkEn = 0;
	fdBlinkAddr = 0;
	
	return ret;
#else
	addrQVGA = addrQVGA;
	addrVGA=addrVGA;
	return FALSE;
#endif
}

#if 0
/**
 * @fn		 UINT8 fdProcBlink(UINT32 addr)
 * @brief	  
 * @param	  [in] 
 * @retval	  
 * @author	  wangjg
 * @since	  2010-6-21
*/
UINT8 
fdProcBlink(
	UINT32 addr
)
{	
	UINT8 ret = 0;
	UINT32 timeInterval = 1000;
	UINT8 smileEn = fdSmileEn;
	fdSmileEn = 1;
	fdBlinkEn = 1;	
	fdBlinkAddr = addr;
	fdSubCnt0 = 0;
	fdSubCnt1 = 0;
	sp1kFdReset();
	sp1kNandRsvRead(FD_ID_Eye, FD_ADDR_DBSub0<<1);
	sp1kNandRsvRead(FD_ID_CloseEye, FD_ADDR_DBSub1<<1);
	fdStep = 0;
	if (fdProcSmile()) {
		UINT32 t0,t1;	
		
		HAL_GlobalReadGTimer(&t1);
		t0=t1+timeInterval;
		while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);
		
		if (t1<t0) {
			fdProcBlinkPrintf("b0:%ld\n",t1-t0+1000);
			fdStep = 0;
			fdProcNonSmile();
		
			HAL_GlobalReadGTimer(&t1);
			t0=t1+timeInterval;
			while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);
			if (t1<t0) {
				fdGroupSub(0);
				if (fdSubCnt0>fdSubCnt1 ) {
					ret = 4;
				}
				fdProcBlinkPrintf("b0:%ld\n",t1-t0+1000);
			}
			else {
				ret = 3;
			}
		}
		else {
			ret = 2;
		}
	}
	else {
		ret = 1;
	}
	fdSmileEn = smileEn;
	fdBlinkEn = 0;
	fdBlinkAddr = 0;
	
	return ret;
}
#endif

#if 0
/**
 * @fn		 void fdProcBlink(void)
 * @brief	  
 * @param	  [in] 
 * @retval	  
 * @author	  wangjg
 * @since	  2010-6-21
*/
void fdProcBlink(void)
{
	UINT32 addr = fdGetCdspBuf(fdCdspIdx,1);//HAL_CdspWriteBufIdxGet());
	UINT8 r=((fdKeyRtS+0x0f)>>4);
	UINT8 x=(fdKeyRtX>>4);
	UINT8 y=(fdKeyRtY>>4)-1;
	UINT8 i;
	UINT16 k=((0x0001<<(r))-1)<<x;
	UINT16 tag[10];
	
	if (fdKeyStep>=FD_DECTED_Cnt || (r==0) ||(y+r>=10)) {
		return;
	}

	if (r>2) {
		r>>=1;
	}
	
	//set tag
	memset(tag,0,16);
	for (i=y;i<y+r;i++) {
		tag[i]=k;	
	}
	HAL_FdTagSet(tag);
	HAL_FdTagEnable(1,1);

	if (fdFlow!=FD_FLOW_Smile) {
		HAL_FdSetDataBase(FD_ADDR_DBSub0);
		HAL_FdWinStepSet(1);
		HAL_FdStage1Set(fdDbgStage1);
		HAL_FdBufMaxCntSet(FD_DECTED_OutCntSub);

		fdFlow = FD_FLOW_Smile;
	}
	
	HAL_FdIITrig(fdJobAddrSub,1000000);
	
	//fdGroup();
	
}
#endif

#if 0
/**
 * @fn		 UINT8 fdProcBlind(UINT32 addr,UINT16 hsize,UINT16 vsize)
 * @brief	  
 * @param	  [in] 
 * @retval	  
 * @author	  wangjg
 * @since	  2010-6-21
*/
UINT8 
fdProcBlind(
	UINT32 addr,
	UINT16 hsize,
	UINT16 vsize
)
{
	//UINT32 addr = fdGetCdspBuf(fdCdspIdx);//HAL_CdspWriteBufIdxGet());
	UINT16 x = fdKeyRtX;
	UINT16 y = fdKeyRtY;
	UINT16 s = fdKeyRtS;
	UINT16 xx,yy;
	UINT32 t0,t1=0,ts;	
	//UINT8 sn[]="BII";
	//sn[2] = fdMode+'0';

	//xx=(x+s<160)?s:160-x;
	//yy= xx*3/4;
	x=x*hsize/160;
	y=y*hsize/160;
	s=s*hsize/160;
	
	xx=s/6;
	yy=s;
	if (x<xx) {
		x=0;
	}
	if (x>320-xx-s) {
		x=320-xx-s;
	}
	
	HAL_FdIISIIQueue(
		hsize,//vsize,
		x,y,xx,yy,
		160,120,
		addr,  fdIISrcAddr,  fdSIIAddr,
		fdJobAddrSub
	);
	
	fdSetDB(1);
	HAL_FdIITrig(fdJobAddrSub);
	fdStep=0;

	HAL_GlobalReadGTimer(&t0);
	t0+=1000;
	t1=0;
	while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);

	dramFileSave("II", fdIISrcAddr, 160ul*120*4);
	dramFileSave("SII", fdSIIAddr, 160ul*120*4);

	fdStep=0;			
	fdSetDB(2);
	fdTrig();

	HAL_GlobalReadGTimer(&t0);
	t0+=1000;
	t1=0;
	while(t1<t0 && fdStep<FD_DECTED_Cnt)HAL_GlobalReadGTimer(&t1);

	return fdGroupSub(0);
}
#endif

