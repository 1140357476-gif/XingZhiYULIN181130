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
 * @file      awb_decider.c
 * @brief    awb decider 
 * @author    wangjg
 * @since     2010-05-05
 * @date      2010-05-05
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "awb_moe.h" 
#include "awb_func.h"
#if AWB_ENABLE
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define AWB_GAIN_COUNT 5
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT16 awbCmA[9],/*awbCmCw[9],*/awbCmD[9];
//UINT8 awbCmASign[8],awbCmDSign[8];

//UINT32 awbCountNormal=0,awbCountOutdoor=0;
//UINT8 awbSpotDelayIdx=0;
UINT16 awbRgains[AWB_GAIN_COUNT+1],awbBgains[AWB_GAIN_COUNT+1];
UINT8 awbGainsCnt=0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void awbAutoOb(UINT32 *ob)
 * @brief     awbAutoOb
 * @param     [in] ob
 * @retval    NONE
 * @author    wangjg
 * @since     2010-8-30
*/
void awbAutoOb(UINT32 *ob)
{
#if AWB_AUTO_WBOffset
	UINT16 rg = awb.setRg;
	UINT16 bg = awb.setBg;
	UINT32 wp = ob[4];
	UINT16 obs[4];
	UINT32 avg,val;
	UINT8 i,err,dt,d0,d1;
	UINT8 thr = awb.obThr;

	awb.flowFlag += AWB_FLAG_AutoOb;
	if(wp<AWB_SAMP_CNT/64)
		return;		
	if(awb.contrast<awb.darkContrast){
		awb.obThr = 0;
	}else if(awb.scene == AWB_SCENE_Outdoor){
		awb.obThr = 8;
	}else if(awb.gid>awb.darkGid){
		awb.obThr = 4;
	}else if(awb.luma>awb.darkLuma){
		awb.obThr = 2;
	}else{
		awb.obThr = 0;
	}
	if(awb.luma<awb.darkLuma)thr*=2;
	HAL_CdspWBOffsetGet(obs,obs+1,obs+2,obs+3);
	ob[0]=(ob[0]*rg)>>8;
	ob[2]=(ob[2]*bg)>>8;
	avg = (ob[0]+ob[1]+ob[2]+ob[3])/4;

	for(i=0;i<4;i++){
		val = ob[i];
		if(val>=avg){
			err = -(UINT8)((val-avg)/wp);
		}
		else{
			err = (UINT8)((avg-val)/wp);
		}

		//if(err>2) err<<=2;
		//else if(err>1) err<<=1;

		dt = ((UINT8)obs[i])+err;

		d0 = awb.ob0[i]+thr;
		d1 = awb.ob0[i]-thr/2;

		if(d0<0x80)d0=0xff;
		
		if(dt<0x80)	dt=0xff;
		if(dt>d0) dt = d0;
		else if(dt<d1)	dt = d1;
		
		//if(awb.spotIdx<=1)dt/=2;
		awb.ob[i] = dt;
		
		obs[i] = (obs[i]&0x0300) | dt;
	}
	
	HAL_CdspWBOffsetSet(obs[0],obs[1],obs[2],obs[3]);
#if 0	
	awbPrintf("(%d,%d)%ld(%ld,%ld,%ld,%ld)",awb.setRg,awb.setBg,wp,ob[0]/wp,ob[1]/wp,ob[2]/wp,ob[3]/wp);
	awbPrintf("(%bx,%bx,%bx,%bx)(%bx,%bx,%bx,%bx)\n",awb.ob[0],awb.ob[1],awb.ob[2],awb.ob[3]	,awb.ob0[0],awb.ob0[1],awb.ob0[2],awb.ob0[3]);
#endif	
#else
	ob=ob;
#endif
}
#if AWB_NoUsed
void awbAutoColorMatrix()
{
	UINT16 cm[9],i=9;
	UINT16 rg = awb.setRg;
	UINT16 bg = awb.setBg;
	UINT16 rg0 = awb.ctcRg[0];
	UINT16 bg0 = awb.ctcBg[0];
	UINT16 rg1 = awb.ctcRg[1];
	UINT16 bg1 = awb.ctcBg[1];
	UINT16 d0 = _ABS(rg, rg0)+_ABS(bg, bg0);
	UINT16 d1 = _ABS(rg, rg1)+_ABS(bg, bg1);
	UINT16 d = d0+d1;

	if(bg+rg1>bg1+rg)//bg-rg<bg1-rg1
	{
		d0 = ((UINT32)d0<<8)/d;
		d1 = ((UINT32)d1<<8)/d;
		
		while(i--)
		{
			UINT32 v0 = awbCmA[i]*d1;
			UINT32 v1 = awbCmD[i]*d0;
			UINT32 val;
			UINT16 sign=0;
			if(awbCmASign[i])v0=-v0;
			if(awbCmDSign[i])v1=-v1;
			val = v0+v1;
			if(val&0x8000){sign = 0x0200;val=-val;}
			val>>=8;
			if(val>0x01ff)val = 0x01ff;
			cm[i] = (UINT16)val | sign;
		}
		
	}
	else
	{
		while(i--)cm[i]=awbCmD[i]|(awbCmDSign[i]?0x0200:0);
	}
	memcpy(&(XBYTE[0x2192]),cm,18);
}
#endif	

void awbAutoHueSat(UINT8 satEnDark)
{
#if 1
	UINT8 luma = awb.luma;
	UINT8 sat = awbUVSat;

	if(luma<awb.darkLuma && (satEnDark == AWB_FUNC_Enable)	)
	{
		sat = (UINT16)(awbUVSat/2)*luma/awb.darkLuma+awbUVSat/2;
	}	
	if(sat!= awbUVSat)
	{		
		XBYTE[0x21B7] = sat ;
	}
#endif
}
/**
 * @fn        void awbDecider(awb_filter_sum_t* rst)
 * @brief     awb decider flow
 * @param     [in] rst	:filter result
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbDecider()//awb_filter_sum_t* rst)
{
	awb_filter_sum_t rst;
	UINT8 obEn = awb.obEn;
	UINT8 satEnDark = awb.satEnDark;
	awbTimerGet(awbTimer.decider+0);
	awb.flowFlag = 0;	
	awbTimerGet(awbTimer.decider+1);
	awbFilterResult(&rst);	
	awbTimerGet(awbTimer.decider+2);
	if(obEn == AWB_FUNC_Enable){
		awbAutoOb(rst.ob);
	}
	if(awbSceneUpdate(&rst)){
		awb.flowFlag = AWB_FLAG_Scene_Pass;
		return;
	}
	if(!awbGetAeInfo())	{
		awb.flowFlag = AWB_FLAG_AE_Pass;
		return;
	}	
	awbTimerGet(awbTimer.decider+3);
	awbSpotsDecider(&rst);		
	awbTimerGet(awbTimer.decider+4);
	
	if(satEnDark == AWB_FUNC_Enable){
		awbAutoHueSat(satEnDark);
	}
	awbTimerGet(awbTimer.decider+5);
	if(awbDebugCnt)
	{
		awbPrintfDecider("%bu(%4u,%4u)(%4u,%4u)"	,awb.scene,awb.avgRg,awb.avgBg,awb.setRg,awb.setBg	);	
		//awbPrintfDecider("(%bx,%bx,%bx,%bx,%bx)"	,awb.spotEn[0],awb.spotEn[1],awb.spotEn[2],awb.spotEn[3],awb.spotEn[4]);
		awbPrintfDecider("(%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx)"	,rst.wp[0],rst.wp[1],rst.wp[2],rst.wp[3],rst.wp[4],rst.wp[5],rst.wp[6],rst.wp[7],rst.wp[8],rst.wp[9]	);
		//awbPrintf("%bx,%bx\n",XBYTE[0x2277],awb.stable);
		awbPrintfDecider("(%bu,%bu,%bu)",awb.gid,awb.luma,awb.contrast);
		awbPrintfDecider("\n");
		//dumpData(16, (UINT32)&(XBYTE[0x2110]), 0);
		//dumpData(16, (UINT32)&(XBYTE[0x2150]), 0);
		if(awbDebugCnt!=0xff)
			awbDebugCnt--;
	}
}

/**
 * @fn        void awbSpotsDecider(awb_filter_sum_t* rst)
 * @brief     spots decider
 * @param     [in] rst
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbSpotsDecider(awb_filter_sum_t* rst)
{
	UINT8 max0,max1;
	UINT8 lstIdx = awb.spotIdx;
	UINT8 idx;
	awb.flowFlag += AWB_FLAG_Spot;
	awbSpotsWp(rst,&max0,&max1);
	if(!awbSpotsLack(rst,max0))
	{		
		idx = awbSpotsCase(rst, max0, max1);
		if(!awbSpotsDelay(idx))
		{			
			awbSpotsAvg(rst,idx);
			awbSpotsSpecial(rst,idx);
		}		
	}
	awbGainAvg();	
	if(awbDebugCnt){
		awbPrintfSpotDecider("avg(%u,%u),tag(%u,%u)\n",awb.avgRg,awb.avgBg,awb.tagRg,awb.tagBg);
	}
}

/**
 * @fn        void awbSpotsWp(awb_filter_sum_t* rst, UINT8 *max0, UINT8 *max1)
 * @brief     weighting spots
 * @param     [in] rst
 * @param     [in] max0
 * @param     [in] max1
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbSpotsWp(awb_filter_sum_t* rst,UINT8 *max0,UINT8 *max1)
{
	UINT8 i;
	UINT8 wi0=0,wi1=0;
	UINT32 w0=0,w1=0;
	UINT32 *wp = rst->wp;//[AWB_SUM_NUM_SPOT];
	UINT8 lstIdx = awb.spotIdx;
	//memcpy(wp,awb.spotEn,AWB_SUM_NUM_SPOT);
	//memset(wp,1,AWB_SUM_NUM_SPOT);
	
	if(awb.scene==AWB_SCENE_Outdoor)
	{
		wp[AWB_SUM_A]=0;
		wp[AWB_SUM_4]=0;
		wp[AWB_SUM_CW]=0;
		//wp[AWB_SUM_5]<<=1;
		//wp[AWB_SUM_D]<<=1;
		/*if(awb.specMode == AWB_SPEC_Sward)
		{
			//wp[AWB_SUM_CW]=0;
			wp[AWB_SUM_5]<<=1;
			//wp[AWB_SUM_D]>>=1;
		}
		else if(awb.specMode == AWB_SPEC_Sky)
		{
			//wp[AWB_SUM_CW]=
			wp[AWB_SUM_5]<<=1;
			//wp[AWB_SUM_D]>>=1;
		}
		
		if(awb.gid>awb.brightGid)
		{
			wp[AWB_SUM_CW]=0;
		}*/
		if(awb.gid>65)
		{
			//wp[AWB_SUM_D]=0;
			//wp[AWB_SUM_D]+=wp[AWB_SUM_Sky];
			//rst->wp0[AWB_SUM_D]+=wp[AWB_SUM_Sky];
			//rst->rg0[AWB_SUM_D]+=rst->rg0[AWB_SUM_Sky];
			//rst->bg0[AWB_SUM_D]+=rst->bg0[AWB_SUM_Sky];
		}
	}
	else if(awb.scene==AWB_SCENE_Normal)
	{
		if(awb.specMode == AWB_SPEC_Office)
		{
			wp[AWB_SUM_A]=0;
			wp[AWB_SUM_4]=0;
			wp[AWB_SUM_CW]=0;
		}
		/*if(awb.gid < awb.darkGid && awb.luma<awb.darkLuma)
		{
			wp[AWB_SUM_D]=0;
			//if(awb.spotIdx<=1)wp[AWB_SUM_5] = 0;
			
		}*/
		//wp[AWB_SUM_A]<<=1;
		//wp[AWB_SUM_CW]<<=1;
		//wp[AWB_SUM_D]<<=1;
	}
	
	//if(lstIdx<AWB_SUM_NUM_SPOT)
	//{
	//	wp[lstIdx]<<=1;
	//}
	for(i=0;i<AWB_SUM_NUM_SPOT;i++)
	{
		//UINT8 k=wp[i];
		UINT32 w=awb.spotEn[i]*wp[i];
		//UINT32 r=rst->rg[i];
		//UINT32 b=rst->bg[i];		

		//w*=k;
		//r*=k;
		//b*=k;		
		
		rst->wp[i] = w;
		//rst->rg[i ]=r;
		//rst->bg[i] =b;	

		if(w>w0)	{
			w1=w0;
			wi1=wi0;
			w0=w;
			wi0=i;
		}else if(w>w1){
			w1=w;
			wi1=i;
		}
	}	
	if(wi0 == lstIdx)
	{
		wp[lstIdx]<<=1;
	}			
	
	*max0=wi0;
	*max1=wi1;
}
/**
 * @fn        void awbSpotsAvg(awb_filter_sum_t* rst, UINT8 idx)
 * @brief     compute gain using weighting average
 * @param     [in] rst
 * @param     [in] idx
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbSpotsAvg(awb_filter_sum_t* rst,UINT8 max0)
{
	/* main peak 	200%
	 *  neighbor 	100%
	 *  >60%		100%
	 *  >20%		50%
	 *  other 	0%
	 */
	UINT8 i,idx;//,wpi;
	UINT32 *wp = rst->wp;
	UINT32 *wp0 = rst->wp0;
	UINT32 *rgs = rst->rg0;
	UINT32 *bgs = rst->bg0;
	UINT32 ws=0;//rst->wp0[idx];
	UINT32 rs=0;//rst->rg0[idx];
	UINT32 bs=0;//rst->bg0[idx];
	//UINT16 div = wp>1024?wp/512:1;// wp over flow
	UINT32 wps[6];	
	
	UINT32 k;
	UINT8 i0=max0-1,i1=max0,i2=max0+1,i3=max0+2;

	if(wp0[i1]>wp0[i2])idx=i1;
	else idx=i2;	
	if(max0==0)
	{
		k = wp0[i1]+wp0[i2];
		awb.avgRg = (rgs[i1]+rgs[i2])/k;
		awb.avgBg = (bgs[i1]+bgs[i2])/k;
		return;
	}
	else if(max0==4)
	{
		ws = wp0[i1]+wp0[i2];
		rs = rgs[i1]+rgs[i2];
		bs = bgs[i1]+bgs[i2];
		if(idx==i1){
			ws += wp0[i0]+wp0[idx]+ws;
			rs += rgs[i0]+rgs[idx]+rs;
			bs += bgs[i0]+bgs[idx]+bs;
		}
		awb.avgRg = rs/ws;
		awb.avgBg = bs/ws;		
		return;
	}
	/*
	if(max0==0 || max0==4)
	{
		k = wp0[i1]+wp0[i2];
		awb.avgRg = (rgs[i1]+rgs[i2])/k;
		awb.avgBg = (bgs[i1]+bgs[i2])/k;
		return;
	}
	*/
	//init wps
	memset(wps,0,6*4);
	if(wp[i0])	wps[i0]=wp[i0]+wp0[i0];	
	if(wp[i2])wps[i3]=wp[i2]+wp0[i3];
	wps[i1]=wp[i1]+wp0[i1];
	wps[i2]=wp[i1]+wp0[i2];	
	
	if(awb.scene == AWB_SCENE_Normal)
	{
		#if 0
		UINT16 dd0,dd1,rg0,bg0,rg1,bg1;
		SINT16 d0,d1,d;
		// 0 //////////////////
		i = idx/2;
		if(i&1){
			rg0 = awb.ctcRg[i];
			bg0=awb.ctcBg[i];
			rg1 = (awb.ctcRg[i]+awb.ctcRg[i+1])/2;
			bg1= (awb.ctcBg[i]+awb.ctcBg[i+1])/2;	
			
		}else{
			rg1 = awb.ctcRg[i];
			bg1=awb.ctcBg[i];
			rg0 = (awb.ctcRg[i]+awb.ctcRg[i-1])/2;
			bg0= (awb.ctcBg[i]+awb.ctcBg[i-1])/2;
		}
		d0=bg0-rg0;	
		d1=bg1-rg1;		
		d = bgs[idx]-rgs[idx];
		dd0 = _ABS(d, d0);
		dd1 = _ABS(d, d1);
		if(idx==i1)
		{
			wp0[i3]=0;
			wps[i1]<<=1;
			if(dd0<dd1){
				wps[i0]<<=1;
				//wpi = i0;
			}else if(dd0>dd1)	{			
				wps[i2]<<=1;
				//wpi = i2;
			}
		}else{
			wp0[i0]=0;
			wps[i2]<<=1;
			if(dd0<dd1){
				wps[i1]<<=1;
				//wpi = i1;
			}else if(dd0>dd1)	{			
				wps[i3]<<=1;
				//wpi = i3;	
			}
		}		
		#endif
		if(awb.specMode== AWB_SPEC_Office)
			wps[4]<<=2;
	}	
	if(awb.scene == AWB_SCENE_Outdoor)
	{
	#if 0
		wps[idx]<<=1;
		wps[i1]<<=1;
		wps[i2]<<=1;
		if(idx==i1 && wp[i2]>wp[i3])
			wp[i3]=0;
		else
			wps[i3]<<=1;

		if(awb.specMode==AWB_SPEC_Sward)
			wps[i1]<<=1;
		if(awb.specMode==AWB_SPEC_Sky){
			wps[i2]<<=1;
			wps[i3]<<=1;
		}	
	#endif
	}
	for(i=i0;i<=i3;i++)
	{
		if(rst->wp0[i]){
			//k= wps[i]/rst->wp0[i];
			k=wps[i];
			if(k){
				ws+=k;
				rs+=rst->rg0[i]/rst->wp0[i] *k;
				bs+=rst->bg0[i]/rst->wp0[i] *k;
			}
		}
	}
	if(ws)
	{
		awb.avgRg = rs/ws;
		awb.avgBg = bs/ws;
	}
	#if 0
	/*UINT16 wp95 = wp*0.95;
	UINT16 wp9 = wp*0.9;	
	UINT16 wp8 = wp*0.8;
	UINT16 wp7 = wp*0.7;
	UINT16 wp6 = wp*0.6;
	UINT16 wp5 = wp*0.5;
	UINT16 wp3 = wp*0.3;
	UINT32 wk;*/
	//UINT8 div;
	//UINT8 k=1;
	/*
	gain		samp		wp	wpSpot			
	1024	1024	8	8	16	16	4294967296	100000000
		1							2	4294967296	100000000
		0.5							1	4294967296	100000000
	*/
	memset(wps,0,6*4);
	for(i=0;i<AWB_SUM_NUM_SPOT;i++)//wp curve
	{
		wk = rst->wp[i];
		//if(i-idx==1 || idx-i==1 || idx==i)k=16;
		if(wk>=wp95)k=8;		
		else if(wk>wp9)k=6;
		else if(wk>wp8)k=4;
		else if(wk>wp6)k=2;
		else if(wk>wp3)k=1;
		else k=0;

		if(rst->wp0[i] + rst->wp0[i+1])
			k= k*wk/(rst->wp0[i]+rst->wp0[i+1]);
		wps[i]+=k;
		wps[i+1]+=k;
		if(i)wps[i]>>=1;
	}
	//if(idx==0)
	//	wps[0]=wps[1];
	//else if(idx==4)
	//	wps[5]=wps[4];
	wp = 0;
	for(i=0;i<=AWB_SUM_NUM_SPOT;i++)
		if(wp<rst->wp0[i]){wp = rst->wp0[i];idx = i;}
	wp = rst->wp0[idx];
	//wp95 = wp*0.95;
	//wp9 = wp*0.9;
	wp8 = wp*0.8;
	wp5 = wp*0.5;
	wp3 = wp*0.3;
	for(i=0;i<=AWB_SUM_NUM_SPOT;i++)//wp0 curve
	{	
		wk = rst->wp0[i];
		//if(wk>=wp95)k=1;
		if(i==idx+1 || i==idx-1 || idx==i)k=2;
		//else if(wk>wp9)k=2;
		else if(wk>wp8)k=3;
		else if(wk>wp5)k=4;
		else if(wk>wp3)k=5;
		else k=8;
		
		k= wps[i]/k;
		//printf("%bu,%lu(%lu,%lu,%lu,%lu)",i,k,rst->wp[i],rst->wp0[i],rst->rg0[i],rst->bg0[i]);
		if(k){		
			//ws+=rst->wp0[i]*k;
			//rs+=rst->rg0[i]*k;
			//bs+=rst->bg0[i]*k;	
			ws+=k;
			rs+=rst->rg0[i]/rst->wp0[i] *k;
			bs+=rst->bg0[i]/rst->wp0[i] *k;
		}
	}
	//printf("\n");
	/*
	for(i=0;i<AWB_SUM_NUM_SPOT;i++)
	{	
		UINT32 k= rst->wp[i]/div;
		if(k){		
			ws+=rst->wp0[i]*k;
			rs+=rst->rg0[i]*k;
			bs+=rst->bg0[i]*k;	
		}
	}*/
	#endif
	#if 0
	//memset(div,1,5);
	for(i=0;i<AWB_SUM_NUM_SPOT;i++)
	{		
		if(i == idx){//*2
			div = 1;
		}else if(i!=idx+1 && i!=idx-1){//*1
			div = 2;
		//if(idx!=i && i!=idx+1 && i!=idx-1)
		}else{
			UINT32 w=rst->wp[i]*10;
			
			if(w>wp*6){//*1
				div = 2;
			}else if(w>wp*2){//*0.5
				div = 4;
				//rst->wp[i]>>=1;
				//rst->rg[i]>>=1;
				//rst->bg[i]>>=1;
			}else{//*0
				div=0;
				//rst->wp[i]=0;
				//rst->rg[i]=0;
				//rst->bg[i]=0;
			}			
		}
		if(div==0 || rst->wp[i]==0 || rst->wp0[i]==0)continue;
		k=(rst->wp[i]*32)/(rst->wp0[i]*div);////(rst->wp0[i]+rst->wp0[i+1]);		
		printf("%bu-%bu-%bu ",i,k,div);
		//k/=div;
		if(k){
			//ws+=(rst->wp0[i]+rst->wp0[i+1])*k;
			//rs+=(rst->rg0[i]+rst->rg0[i+1])*k;
			//bs+=(rst->bg0[i]+rst->bg0[i+1])*k;
			ws+=(rst->wp0[i])*k;
			rs+=(rst->rg0[i])*k;
			bs+=(rst->bg0[i])*k;	
		}
	}
	/*
	if(awb.mode == AWB_MODE_Auto){
		ws-=rst->wp[1]+rst->wp[3];
		rs-=rst->rg[1]+rst->rg[3];
		bs-=rst->bg[1]+rst->bg[3];	
	}*/
	
	if(ws)
	{
		awb.avgRg = rs/ws;
		awb.avgBg = bs/ws;
	}
	#endif
}
/**
 * @fn        UINT8 awbSpotsCase(UINT8 max0,UINT8 max1)
 * @brief     spots max case
 * @param     [in] max0
 * @param     [in] max1
 * @retval    return = idx
 * @author    wangjg
 * @since     2010-5-5
*/
UINT8 awbSpotsCase(awb_filter_sum_t* rst,UINT8 max0,UINT8 max1)
{
	UINT8 idx=max0;	
	max1=max1;
	#if 0
	switch(max0)/*  0 4 1 5 2  */
	{
	case AWB_SUM_A:
		break;
	case AWB_SUM_4:		
		break;
	case AWB_SUM_CW:
		break;
	case AWB_SUM_5:
		break;
	case AWB_SUM_D:
		break;
	default:
		break;	
	}
	#endif
	if(awb.scene == AWB_SCENE_Normal)
	{
		if(max0 == AWB_SUM_4 || max0==AWB_SUM_5){
			if(rst->wp0[max0]>rst->wp0[max0+1]*2)
				idx = max0-1;
			else if(rst->wp0[max0]*2<rst->wp0[max0+1])
				idx = max0+1;
		}
	}
	/*else if(awb.scene == AWB_SCENE_Outdoor)
	{
		idx=AWB_SUM_D;
	}*/
	awb.flowFlag += idx;
	return idx;
	rst=rst;
}
/**
 * @fn        UINT8 awbSpotsDelay(UINT8 idx)
 * @brief    delay when spaning spot
 * @param     [in] idx
 * @retval    return = TRUE/FALSE
 * @author    wangjg
 * @since     2010-5-5
*/
UINT8 awbSpotsDelay(UINT8 idx)
{
	if(idx == awb.spotIdx){		
		awb.spotDelayIdx = awb.spotDelayCnt;	
		if(awb.gid<=awb.darkGid && awb.luma<80){
			UINT8 sh = 90 - awb.luma;
			if(awb.spotDelayIdx<sh)awb.spotDelayIdx=sh;
			if(awb.spotDelayIdx>10)awb.spotDelayIdx=10;
		}
	//}else if(_ABS(awb.spotIdx, idx)>2){
		//printf("+%bu\n",awb.spotDelayIdx);
		//awb.spotDelayIdx--;
		//awb.spotIdx = idx;
		//awb.jumpIdx = 1;
		//awb.spotDelayIdx = 0;
	}else if( awb.spotDelayIdx){
		//printf("%bu\n",awb.spotDelayIdx);
		awb.spotDelayIdx--;
		awb.flowFlag += AWB_FLAG_Spot_Delay;
		return 1;
	}else{		
		awb.spotIdx = idx;
		awb.jumpIdx = 1;
	}	
	return 0;
#if 0
	if(idx != awb.spotIdx)
	{
		if(awb.spotDelayIdx){
			awb.spotDelayIdx--;
			awb.flowFlag += AWB_FLAG_Spot_Delay;
			return 1;
		}else{		
			awb.spotIdx = idx;
			awb.stable = 0;
			awb.spotDelayIdx = 3;
			/*
			if(awb.scene==AWB_SCENE_Outdoor)
			{
				if(awb.spotIdx==5){
					awbSpotDelayIdx=10;
				}else if(idx==5){
					awbSpotDelayIdx=2;
				}
			}
			*/
		}
		//awb.spotIdx = idx;
		//awb.stable = 0;
		//awb.flowFlag += AWB_FLAG_Spot_Delay;
		//return 1;
	}
	else
	{
		return 0;
	}
#endif
	
#if 0
	if(awb.sp!=spot_idx/* && (spot_idx==idx)*/)
	{
		UINT8 th=4;
		spot_delay_cnt++;
		if(spot_delay_idx==5 )
		{
			th=5;
			if(spot_idx==0)
				th=3;
		}
		if(spot_idx==5)
			th=2;
		if(spot_delay_cnt<th)
		{
			awbFlag|=AWB_FLAG_delay;
			return;
		}
	}
	
	spot_delay_cnt=0;
	spot_delay_idx=spot_idx;
#endif
	//return 0;
	//idx = idx;

}
/**
 * @fn        void awbSpotsSpecial(void)
 * @brief     special case
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbSpotsSpecial(awb_filter_sum_t* rst,UINT8 idx)
{	
	UINT16 rg = awb.fixRg;
	UINT16 bg = awb.fixBg;
	UINT8 k = 0;//awb.fixWp;
	awb.avgRg += awb.shiftRg;
	awb.avgBg += awb.shiftBg;
	rst = rst;
	idx = idx;
	if(awb.fixWp)	{
		//UINT8 fixWp = awb.fixWp;
		//awb.avgRg = (awb.avgRg + awb.fixRg*fixWp)/(fixWp+1);
		//awb.avgBg = (awb.avgBg + awb.fixBg*fixWp)/(fixWp+1);
	//}else if(rst->wp[AWB_SUM_5]>AWB_SAMP_CNT/2 && awb.scene == AWB_SCENE_Outdoor)	{
			
			
	}else if(awb.specMode==AWB_SPEC_Sward){
		rg = awb.ctcRg[1];//(awb.ctcRg[1]+awb.ctcRg[2])/2;
		bg = (awb.ctcBg[1]+awb.ctcBg[2])/2;
		if(awb.avgBg>bg){
			k=1;
		}
		/*if(idx==AWB_SUM_CW){
			//rg = (awb.ctcRg[1]*2+awb.ctcRg[2])/3;
			//bg = (awb.ctcBg[1]*2+awb.ctcBg[2])/3;
			k=2;
		}else if(idx==AWB_SUM_D){
			//rg = (awb.ctcRg[1]+awb.ctcRg[2]*2)/3;
			//bg = (awb.ctcBg[1]+awb.ctcBg[2]*2)/3;
			k=2;
		}else if(idx == AWB_SUM_5){
			//rg = (awb.ctcRg[1]+awb.ctcRg[2])/2;
			//bg = (awb.ctcBg[1]+awb.ctcBg[2])/2;
			k=0;
		}else{
			//rg = (awb.ctcRg[1]+awb.ctcRg[2])/2;
			//bg = (awb.ctcBg[1]+awb.ctcBg[2])/2;
			k=4;
		}*/
		//awb.spotIdx=AWB_SUM_5;
	}
	else if(awb.specMode==AWB_SPEC_Sky)
	{
		//wp[AWB_SUM_CW]=wp[AWB_SUM_5]=0;
		//wp[AWB_SUM_D]<<=2;
		//k=1;
	}
	else if(awb.specMode==AWB_SPEC_Office)
	{
		//rg = rst->rg0[AWB_SUM_Office]/rst->wp0[AWB_SUM_Office];
		//bg = rst->bg0[AWB_SUM_Office]/rst->wp0[AWB_SUM_Office];
		/*k = rst->wp0[AWB_SUM_Office]*2/AWB_SAMP_CNT;
		if(k==0)k=1;*/
		//if(rst->wp[AWB_SUM_D]>rst->wp0[AWB_SUM_Office]){k=0;awb.specMode=0;}
	}
	if(k){
		awb.avgRg = (awb.avgRg + rg*k)/(k+1);
		awb.avgBg = (awb.avgBg + bg*k)/(k+1);	
		awb.flowFlag+= AWB_FLAG_Special;
	}
	/*
	if(awb.gid>awb.brightGid && awb.contrast>awb.brightContrast)
	{
		UINT16 rg = (awb.ctcRg[1]+awb.ctcRg[2]*3)/4;
		UINT16 bg = (awb.ctcBg[1]+awb.ctcBg[2]*3)/4;
		awb.avgRg = (rg+awb.avgRg)/2;
		awb.avgBg = (bg+awb.avgBg)/2;
	}
	*/
#if 0//True A light mode
	if(	awb.aLightMode &&
		awb.avgBg>awb.avgRg)//a light
	{
		UINT16 rg = (awb.ctcRg[0]+awb.ctcRg[1])/2;
		UINT16 bg = (awb.ctcBg[0]+awb.ctcBg[1])/2;
		if((awb.avgBg-awb.avgRg) 	> (bg-rg))
		{
			UINT16 d =( awb.avgBg - (bg - rg) - awb.avgRg);///(1.414*1.414);
			//awb.avgRg = awb.ctcRg[0];
			//awb.avgRg -= (bg - rg)/1.4;
			//d*=5;
			awb.avgBg -= d*2;
			//awb.avgRg += d/2;
			if(awb.avgRg<256)awb.avgRg = 256;
			awb.flowFlag+=AWB_FLAG_Spot_SpecialA;
		}
	}
#endif
}
/**
 * @fn        UINT8 awbSpotsLack(awb_filter_sum_t* rst, UINT8 idx)
 * @brief    process of lack vaild white pointer
 * @param     [in] rst
 * @param     [in] idx
 * @retval    return = TRUE/FALSE
 * @author    wangjg
 * @since     2010-5-5
*/
UINT8 awbSpotsLack(awb_filter_sum_t* rst,UINT8 idx)
{
	UINT32 w = rst->wp[idx];
	UINT16 rg = awb.avgRg;
	UINT16 bg = awb.avgBg;
	UINT8 flag=0;
	if(w>(awb.spotThr*AWB_SAMP_CNT>>8))
	{		
		return FALSE;
	}
	else if(awb.mode!=AWB_MODE_Auto)
	{
		rg = awb.fixRg;
		bg = awb.fixBg;
	}
	else if(awb.scene==AWB_SCENE_Outdoor)
	{
		if(rst->wp[AWB_SUM_Sward]>AWB_SAMP_CNT/3)//sward
		{
			if(awb.spotIdx!=AWB_SUM_5){
				rg = (awb.ctcRg[1]+awb.ctcRg[2])/2;
				bg = (awb.ctcBg[1]+awb.ctcBg[2])/2;
				//awb.spotIdx=AWB_SUM_5;
			}
			awb.specMode = AWB_SPEC_Sward;
		}
		else if(rst->wp[AWB_SUM_Sky]>AWB_SAMP_CNT/5)//sky
		{
			if(awb.spotIdx!=AWB_SUM_D){
				rg = awb.ctcRg[2];
				bg = awb.ctcBg[2];
				//awb.spotIdx=AWB_SUM_D;
			}
			awb.specMode = AWB_SPEC_Sky;
		}			
		else//outdoor
		{
			rg = (awb.ctcRg[1]+awb.ctcRg[2])/2;
			bg = (awb.ctcBg[1]+awb.ctcBg[2])/2;
			flag = 1;
		}
	}
	else if(awb.scene == AWB_SCENE_Normal){
		flag = 1;
	}	
	if(flag)
	{
		if(awb.stable)	{
			rg = awb.avgRg;
			bg = awb.avgBg;
		}else if(awb.spotIdx>=AWB_SUM_NUM_SPOT){
			rg = awb.fixRg;
			bg = awb.fixBg;
		}else if(awb.spotIdx==AWB_SUM_A){
			rg = awb.ctcRg[AWB_SPOT_A];
			bg = awb.ctcBg[AWB_SPOT_A];
		}else if(awb.spotIdx==AWB_SUM_4){
			rg = (awb.ctcRg[AWB_SPOT_A]+awb.ctcRg[AWB_SPOT_CW])>>1;
			bg = (awb.ctcBg[AWB_SPOT_A]+awb.ctcBg[AWB_SPOT_CW])>>1;
		}else if(awb.spotIdx==AWB_SUM_CW){
			rg = awb.ctcRg[AWB_SPOT_CW];
			bg = awb.ctcBg[AWB_SPOT_CW];
		}else if(awb.spotIdx==AWB_SUM_5){
			rg = (awb.ctcRg[AWB_SPOT_D]+awb.ctcRg[AWB_SPOT_CW])>>1;
			bg = (awb.ctcBg[AWB_SPOT_D]+awb.ctcBg[AWB_SPOT_CW])>>1;
		}else if(awb.spotIdx==AWB_SUM_D){
			rg = awb.ctcRg[AWB_SPOT_D];
			bg = awb.ctcBg[AWB_SPOT_D];
		}else{
			rg = awb.fixRg;
			bg = awb.fixBg;
		}
	}
	awb.avgRg = (awb.avgRg + rg)>>1;
	awb.avgBg = (awb.avgBg + bg)>>1;
	
	awb.flowFlag += AWB_FLAG_Spot_Lack;
	//awbConvergeGain();
	return TRUE;
}
#if AWB_NoUsed
/**
 * @fn        void awbConvergeGain(void)
 * @brief     cdsp gain converge on current gain 
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
UINT8 awbConvergeGain(void)
{	
	UINT16	rgain_diff, bgain_diff, diff;
	//UINT16 step,rgStep,bgStep;

	rgain_diff = _ABS(awb.avgRg,awb.setRg);
	bgain_diff = _ABS(awb.avgBg,awb.setBg);
	diff = _MAX(rgain_diff, bgain_diff);
	
	// stable
	/*if(diff < awb.jumpStable){
		awb.stable ++;
		if(awb.stable>200)awb.stable=200;
		awb.flowFlag += AWB_FLAG_Gain_Stable;
		return 0;
	// jump
	}else */if(awb.jumpIdx)	{
		awb.jumpIdx--;
		awb.setRg = awb.avgRg;
		awb.setBg = awb.avgBg;
		//awb.setRg = awb.tmpRg >>2;
		//awb.setBg = awb.tmpBg >>2;
		awb.stable = 0;
		awb.flowFlag += AWB_FLAG_Gain_Jump;
		//awbGainSet(awb.setRg,awb.setBg);
		return 1;
	// stabled delay 1 frame	
	}else if(awb.stable && !(awb.flowFlag&AWB_FLAG_Spot_Lack)){
		awb.stable = 0;
		awb.flowFlag += AWB_FLAG_Gain_StableDelay;
		return 0;
	//jump err	
	}else if(diff>=awb.jumpErr){
		awb.flowFlag += AWB_FLAG_Gain_JumpErr;
		return 0;
#if 0
	}else if(diff>awb.jumpMax){
		awb.setRg = (awb.setRg+awb.avgRg)/2;
		awb.setBg = (awb.setBg+awb.avgBg)/2;
		awb.stable = 0;
	}else{
		awb.setRg = awb.avgRg;
		awb.setBg = awb.avgBg;
		awb.stable = 1;
#else
	}else{
		awb.setRg = (awb.setRg+awb.avgRg)/2;
		awb.setBg = (awb.setBg+awb.avgBg)/2;
		
		if(diff < awb.jumpStable)
			awb.stable =1;
		else
			awb.stable = 0;

		awb.flowFlag += AWB_FLAG_Gain;
		return 1;
	}
#endif
	//awb.stable = 0;	
	//step = diff/awb.jumpMax;
	/*	 if(diff>awb.jumpMax*4)	step=5;//awb.jumpMax;
	else if(diff>awb.jumpMax*3)	step=4;//awb.jumpMax;
	else if(diff>awb.jumpMax*2)	step=3;//awb.jumpMax;
	else if(diff>awb.jumpMax)		step=2;//awb.jumpStable;
	else							step=1;*/
	
	//if(step){
	//	awb.setRg = (awb.setRg+(UINT32)awb.avgRg*step)/(step+1);
	//	awb.setBg = (awb.setBg+(UINT32)awb.avgBg*step)/(step+1);
	//}
	/*
	if(rgain_diff> bgain_diff){		
		rgStep = step;
		bgStep = (UINT32)step*bgain_diff/rgain_diff;		
	}else{		
		rgStep = (UINT32)step*rgain_diff/bgain_diff;
		bgStep = step;
	}
	awb.setRg = (awb.avgRg > awb.setRg)? (awb.setRg+rgStep): (awb.setRg-rgStep);
	awb.setBg = (awb.avgBg > awb.setBg)? (awb.setBg+bgStep): (awb.setBg-bgStep);
	*/
	#if 0
	if(rgain_diff<awb.jumpStable){
		step=1;
	}else if(rgain_diff<awb.jumpMax){
		step=awb.jumpStable;
	}else {
		step=awb.jumpMax;
	}
	awb.setRg = (awb.avgRg > awb.setRg)? (awb.setRg+step): (awb.setRg-step);		
	//awb.setRg = awb.tmpRg >> 2;
	
	if(bgain_diff<awb.jumpStable){
		step=1;
	}else if(bgain_diff<awb.jumpMax){
		step=awb.jumpStable;
	}else {
		step=awb.jumpMax;
	}
	awb.setBg = (awb.avgBg > awb.setBg)? (awb.setBg+step): (awb.setBg-step);
	//awb.setBg = awb.tmpBg >> 2;
	
	awb.flowFlag += AWB_FLAG_Gain;
	//awbGainSet(awb.setRg,awb.setBg);
	return 1;
	#endif
}
#endif
void awbGainAvg(void)
{

	if(awb.jumpIdx)	{
		awb.jumpIdx--;
		awb.tagRg = awb.avgRg;
		awb.tagBg = awb.avgBg;
		awbGainsCnt = 0;		
		awb.flowFlag += AWB_FLAG_Gain_Jump;
	}else{
		UINT8 i,wp;
		UINT32 rgs=0,bgs=0,wps=0;
		UINT32 rg,bg;
		UINT16	rgain_diff, bgain_diff, diff;	
#if 0
		awb.tagRg = awb.avgRg;
		awb.tagBg = awb.avgBg;
#else
		if(awbGainsCnt < AWB_GAIN_COUNT)
			awbGainsCnt++;
		awbRgains[awbGainsCnt] = awb.avgRg;
		awbBgains[awbGainsCnt] = awb.avgBg;
		for(i=0;i<awbGainsCnt;i++)
		{
			wp = i+1;
			awbRgains[i] = awbRgains[wp];
			awbBgains[i] = awbBgains[wp];
			rgs+=awbRgains[i]*wp;
			bgs+=awbBgains[i]*wp;
			wps+=wp;
		}
		if(wps){
			rg = rgs/wps;
			bg = bgs/wps;

			rgain_diff = _ABS(awb.avgRg,rg);
			bgain_diff = _ABS(awb.avgBg,bg);
			diff = _MAX(rgain_diff, bgain_diff);			
			if(diff > awb.jumpErr){
				awb.stable =0;
			}else if(diff > awb.jumpMax){
				awb.tagRg = (rg+awb.tagRg)/2;
				awb.tagBg = (bg+awb.tagBg)/2;
				awb.stable =0;
			}else if(diff > awb.jumpStable){
				awb.tagRg = rg;
				awb.tagBg = bg;				
				awb.stable = 0;
			}else{
				awb.tagRg = (rg+awb.avgRg)/2;
				awb.tagBg = (bg+awb.avgBg)/2;
				awb.stable = 2;
			}			
		}
#endif
		awb.flowFlag += AWB_FLAG_Gain;
	}		
}

void awbGainJump(void)
{
	if(awb.setRg != awb.tagRg || awb.setBg != awb.tagBg || awb.gainSetCnt)
	{
		SINT16 rgain_diff = (SINT16)awb.tagRg-awb.setRg;
		SINT16 bgain_diff = (SINT16)awb.tagBg-awb.setBg;
		UINT16 setRg = awb.tagRg,setBg = awb.tagBg;
		SINT16 step=awb.jumpStep;
		if (__ABS(rgain_diff)<=3 && __ABS(bgain_diff)<=3)return;
		if(step ==0){
			setRg =(awb.setRg+awb.tagRg)/2;			
			setBg =(awb.setBg+awb.tagBg)/2;			
		}else{			
			if(rgain_diff>step)			{setRg = awb.setRg + step;}
			else if(rgain_diff<-step)	{setRg = awb.setRg -  step;}
			//else 					{setRg = awb.tagRg;}

			if(bgain_diff>step)			{setBg = awb.setBg + step;}
			else if(bgain_diff<-step)	{setBg = awb.setBg -  step;}
			//else 					{setBg = awb.tagBg;}
		}
		
		awb.newGain = 0;
		awb.setRg = setRg;//(rgain_diff<=1)?(awb.tagRg):((awb.setRg+awb.tagRg)/2);
		awb.setBg = setBg;//(bgain_diff<=1)?(awb.tagBg):((awb.setBg+awb.tagBg)/2);
		awb.newGain = 1;
		if((awb.stable & 0x2) && (_MAX(rgain_diff,bgain_diff)<awb.jumpStable))
		{
			awb.stable |= 0x1;
		}
		//printf("%2bx(%3u,%3u)(%3u,%3u)(%3d,%3d)\n",awb.jumpStep,awb.setRg,awb.setBg,awb.tagRg,awb.tagBg,rgain_diff,bgain_diff);
		//awbGainReset();

		if(awb.gainSetCnt)
			awb.gainSetCnt--;
	}
	if(awb.mode){
		if(awb.setRg<awb.fixRg0){awb.setRg=awb.fixRg0;}
		else if(awb.setRg>awb.fixRg1){awb.setRg = awb.fixRg1;}
		if(awb.setBg<awb.fixBg0){awb.setBg=awb.fixBg0;}
		else if(awb.setBg>awb.fixBg1){awb.setBg = awb.fixBg1;}
	}
}

/*void awbPostRect(UINT8 idx)
{
	//UINT8 idx = awb.scene;
	awb_filter_sum_t rst;
	UINT32 wps;// = rst.wp0[idx];
	UINT32 rgs;// = rst.rg0[idx];
	UINT32 bgs;// = rst.bg0[idx];
	UINT16 rg,  bg;
	awbReadWindow();
	HAL_MoeRunQueue(pawbaddr->queue[AWB_SCENE_Normal],MOE_Trigger_Awb);
	awbFilterResult(&rst);
	wps = rst.wp0[idx];
	rgs = rst.rg0[idx];
	bgs = rst.bg0[idx];
	awb.flowFlag += AWB_FLAG_RectOne;
//	printf("%lu,%lu,%lu\n",wps,rgs,bgs);
	if(wps>20){
		rg = rgs/wps;
		bg = bgs/wps;
	}else{
		rg = awb.avgRg;
		bg = awb.avgBg;
	}
	awb.avgRg = rg;
	awb.avgBg = bg;
	awbGainSet(rg, bg);
}*/
/*void awbPostFlash()
{
	UINT8 dis = _ABS(awb.luma,120);
	if(dis<20)awbPostSpots();
	else awbPostRect(AWB_SUM_Flash);
}*/
/*void awbPostCalibration()
{
	awbPostRect(AWB_SUM_Calibration);
}*/

#endif
