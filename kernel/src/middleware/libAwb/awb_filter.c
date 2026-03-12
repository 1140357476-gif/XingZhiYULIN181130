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
 * @file      awb_Filter.c
 * @brief     awb scene and filter
 * @author   wangjg
 * @since     2010-04-30
 * @date      2010-04-30
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "awb_moe.h"
#include "awb_func.h"
#include "initio.h"

#include "hal_cdsp.h"
#include "hal_gprm.h"

#include "ae_def.h"
#if AWB_ENABLE
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//#define aeAwbInfo(gid,luma,contrast)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 awbResultPrintEn = 0;

/**
 * @fn        void awbFilter(awb_filter_sum_t *rst)
 * @brief     filter proc
 * @param     [out] rst	:result of job queue  
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-4
*/
UINT8 awbFilter()//awb_filter_sum_t *rst)
{
		
	//awbFilterResult(rst);
	//awbSceneUpdate(rst);

	//HAL_MoeRunQueue(pawbaddr->queue[awb.scene]);	
	if(!HAL_MoeIsBusy(1))
	{
		UINT8 sts;
		awbTimerGet(awbTimer.filter+0);
		awbReadWindow();
		awbTimerGet(awbTimer.filter+1);	
		sts = HAL_MoeTriggerQueue(pawbaddr->queue[awb.scene],MOE_Trigger_Awb);
		awbTimerGet(awbTimer.filter+2);
		//if(sts)moeDbgTrig = MOE_Trigger_Awb;	
		return sts;
		/*
		if(awb.mode==AWB_MODE_Flash || isWait)
		{			
			HAL_MoeRunQueue(pawbaddr->queue[awb.scene]);
		}
		else if(awb.mode == AWB_MODE_Normal || awb.mode == AWB_MODE_Outdoor)
		{
			HAL_MoeTriggerQueue(pawbaddr->queue[awb.scene]);
		}
		*/
	}
}
UINT8 awbGetAeInfo()
{
	UINT8 gid;//=40;
	UINT8 luma;//=120;
	UINT8 contrast;//=40;
	UINT8 ymax,ymin,wpmax;
	UINT8 wp = 32;
	static UINT8 wpMax = 0;
	aeAwbInfo(&gid,&luma,&ymax,&ymin);
	//if(ymin == 0)ymin = luma-(ymax-luma);
	//printf("%bu,%bu,%bu,%bu\n",gid,luma,ymax,ymin);
	contrast = ymax - ymin;
	awb.gid = gid;
	awb.luma = luma;
	awb.contrast = contrast;	
	if(gid<=awb.darkGid && luma<80){		
		ymin=5;ymax=luma+40; wpmax=1;	
	}else{
		ymin=10;ymax=250; wpmax=8;	
	}
	if(wpmax != wpMax){
		awbMoeModifyYThr(ymin, ymax, wpmax);
		wpMax = wpmax;
	}
	//if(gid<=awb.darkGid && luma<awb.darkLuma/3)return FALSE;
	if(	(luma>awb.brightLuma) ||
		( luma<awb.darkLuma && gid>=awb.darkGid) || 
		(luma<awb.darkLuma/4 && gid<awb.darkGid))
		{return FALSE;}
	
	return TRUE;
}
/**
 * @fn        void awbSceneUpdate(void)
 * @brief     update scene param
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-4
*/
UINT8 awbSceneUpdate(awb_filter_sum_t* rst)
{
	UINT8 scene = awb.scene;
	UINT8 gid = awb.gid;//=40;
	UINT8 luma = awb.luma;//=120;
	UINT8 contrast = awb.contrast;//=40;
	//UINT8 ymax,ymin;
	//UINT8 conStep=(256-awb.brightContrast)/4;
	//UINT8 gidStep = awb.brightGid/8;
	UINT8 specMode = awb.specMode;
	
	if(awb.sceneIdx==0xff){
		return FALSE;
	}
	if(awb.globeGain>AWB_GLOBE_GAIN_BASE*8){
		gid-=30;
	}else if(awb.globeGain>AWB_GLOBE_GAIN_BASE*4){
		gid-=20;
	}else if(awb.globeGain>AWB_GLOBE_GAIN_BASE*2){
		gid-=10;
	}
	if(scene == AWB_SCENE_Normal)
	{
		//office
		if        ((rst->wp[AWB_SUM_Office]>AWB_SAMP_CNT/10)&&(specMode == AWB_SPEC_Office))
			{specMode = AWB_SPEC_Office;}
		else if((rst->wp[AWB_SUM_Office]>AWB_SAMP_CNT/5)&&(rst->wp[AWB_SUM_Office]>rst->wp[AWB_SUM_D]/2))		
			{specMode = AWB_SPEC_Office;}
		//outdoor
		else if(gid>=(awb.brightGid))			  
			{specMode = 0;scene = AWB_SCENE_Outdoor;	}
		else if((gid>=awb.brightGid*0.8) && ((UINT16)(awb.brightGid - gid)*2*(240-awb.brightContrast)/awb.brightGid+awb.brightContrast < contrast)&& (rst->wp[AWB_SUM_Office]<AWB_SAMP_CNT/2)	)//office
			{specMode = 0;scene = AWB_SCENE_Outdoor;	}
		//specMode
		else {specMode = 0;}
			
	}
	else if(scene == AWB_SCENE_Outdoor)
	{
		//sward
		if (      (rst->wp[AWB_SUM_Sward]>AWB_SAMP_CNT/10)&&(specMode == AWB_SPEC_Sward))
		  	{specMode = AWB_SPEC_Sward;}
		else if(rst->wp[AWB_SUM_Sward]>AWB_SAMP_CNT/5)
			{specMode = AWB_SPEC_Sward;}
		//sky
		else if((rst->wp[AWB_SUM_Sky]>AWB_SAMP_CNT/10)&&(specMode  ==  AWB_SPEC_Sky))
			{specMode = AWB_SPEC_Sky;}
		else if(rst->wp[AWB_SUM_Sky]>AWB_SAMP_CNT/5)
			{specMode = AWB_SPEC_Sky;}
		//office
		else if(/*(gid<awb.brightGid/2)&&*/(rst->wp[AWB_SUM_Office]*awb.brightGid>AWB_SAMP_CNT*gid/2)&&(rst->wp[AWB_SUM_Office]>rst->wp[AWB_SUM_D]))		
			{specMode = AWB_SPEC_Office;}
		//indoor
		else if((contrast<awb.darkContrast && gid<awb.brightGid)|| /*luma<awb.darkLuma ||*/ gid<awb.brightGid/8)			
			{specMode = 0;scene = AWB_SCENE_Normal;	}
		//specMode
		else {specMode = 0;}
		
	}
	if(specMode&0x0f)
		scene = AWB_SCENE_Outdoor;
	else if(specMode&0xf0)
		scene = AWB_SCENE_Normal;
	awb.specMode = specMode;	
	if(awb.scene!=scene)
	{
		if(awb.sceneIdx){
			awb.sceneIdx--;
		}else{
			awb.scene = scene;			
			if(awb.scene==AWB_SCENE_Normal)
				awb.sceneIdx=5;
			else if(awb.scene==AWB_SCENE_Outdoor)
				awb.sceneIdx=10;		
			return TRUE;
		}
	}
	return FALSE;
	//if(awbDebugCnt)	printf("%bx,gid:%bu,luma:%bu,contrast:%bu\n",awb.scene,gid,luma,contrast);
}






/**
 * @fn        void awbReadWindow(void)
 * @brief     read raw from window and format
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbReadWindow()
{
//printf("awb wnd:%lx,%lx,%lx\n",AWB_ADDR_Wnd0,AWB_ADDR_Wnd1,AWB_SAMP_CNT);
	//HAL_GprmDramDma(AWB_ADDR_Wnd0<<1,(K_SDRAM_JPG_FdbParameterBufAddr+K_SDRAM_JPG_FdbParameterBufSize-512*4)<<1, 1024*4, 1);

	HAL_GprmCopy(AWB_ADDR_Wnd0
				,AWB_SAMP_XCNT*2,AWB_SAMP_YCNT,0,0
				,AWB_ADDR_Wnd1
				,AWB_SAMP_XCNT,AWB_SAMP_YCNT,0,0,
				AWB_SAMP_XCNT,AWB_SAMP_YCNT,
				0,1
	);
	
	HAL_GprmCopy(AWB_ADDR_Wnd0
				,AWB_SAMP_XCNT*2,AWB_SAMP_YCNT,AWB_SAMP_XCNT,0
				,AWB_ADDR_Wnd1+AWB_SAMP_CNT
				,AWB_SAMP_XCNT,AWB_SAMP_YCNT,0,0,
				AWB_SAMP_XCNT,AWB_SAMP_YCNT,
				0,1
	);
	 
}
/**
 * @fn        void awbFilterResult(awb_filter_sum_t *sum)
 * @brief     read jobqueue result from dram
 * @param     [out] sum	:result of jobqueue pointer
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-4
*/
void awbFilterResult(awb_filter_sum_t *sum)
{
	#define len ((UINT16)6*AWB_SUM_NUM)
	UINT32 pt32[len+10];
	UINT8 i,idx=0;
	UINT32 *wpsum = sum->wpsum;
	UINT32 *wp = sum->wp;
	UINT32 *wp0 = sum->wp0;
	UINT32 *rg0 = sum->rg0;
	UINT32 *bg0 = sum->bg0;
	UINT32 *ob = sum->ob;

	awb_sum_spot_t xdata* pt = (awb_sum_spot_t xdata*)pt32;
#if 0	
	//UINT8 pt8[240];
	UINT8 *pt8=G_ucStorData;
	
	UINT8 *wp8 = sum->wp;
	UINT8 *rg8 = sum->rg;
	UINT8 *bg8 = sum->bg;
	
	UINT8 sts;
	ENTER_CRITICAL(sts);
	HAL_CpuRamMapSet(AWB_ADDR_Out, 1024ul, (void xdata* xdata*)&pt8);
	//HAL_DmaSetDramAddr(AWB_ADDR_Out);
	//HAL_DmaSetSramAddr(0x0000);
	//HAL_DmaDo(0,1,240,0,0);
	
	for(i=0;i<AWB_SUM_NUM*4;i+=4)
	{
		idx=i*6;
		wp8[i+0]=pt8[idx+3];
		wp8[i+1]=pt8[idx+2];
		wp8[i+2]=pt8[idx+1];
		wp8[i+3]=pt8[idx+0];
		idx+=8;
		rg8[i+0]=pt8[idx+3];
		rg8[i+1]=pt8[idx+2];
		rg8[i+2]=pt8[idx+1];
		rg8[i+3]=pt8[idx+0];
		idx+=8;
		bg8[i+0]=pt8[idx+3];
		bg8[i+1]=pt8[idx+2];
		bg8[i+2]=pt8[idx+1];
		bg8[i+3]=pt8[idx+0];
		
		//wp[i]=pt32[i*6];
		//rg[i]=pt32[i*6+2];
		//bg[i]=pt32[i*6+4];
	}

	for(i=3;i>0;i--)
	{
		wp[i]=wp[i]	-wp[i-1];
		rg[i]=rg[i]	-rg[i-1];
		bg[i]=bg[i]	-bg[i-1];
	}
	HAL_CpuRamMapRcv();
	EXIT_CRITICAL(sts);
#else
	
	HAL_DramReadBytesAddr(AWB_ADDR_Out,(UINT8*)pt32,sizeof(pt32),2);
#if 0

	for(i=0;i<AWB_SUM_NUM;i++)
	{		
		wp[i]=pt32[i*6];
		rg[i]=pt32[i*6+2];
		bg[i]=pt32[i*6+4];
	}
	for(i=1;i<4;i++)
	{
		wp[i]=pt32[i*6] 	-pt32[(i-1)*6];
		rg[i]=pt32[i*6+2]	-pt32[(i-1)*6+2];
		bg[i]=pt32[i*6+4]	-pt32[(i-1)*6+4];
	}

#else
	#if 1
	for(i=0;i<AWB_SUM_NUM;i++)
	{
		if(awbResultPrintEn)printf("%lu,%lu,%lu\n",pt[i].wp.sum,pt[i].rg.sum,pt[i].bg.sum);
		wp[i] = pt[i].wp.sum;//-pt[i-1].wp.sum;
		wp0[i] = pt[i].wp.sum;//-pt[i-1].wp.sum;		
		rg0[i] = pt[i].rg.sum;//-pt[i-1].rg.sum;
		bg0[i] = pt[i].bg.sum;//-pt[i-1].bg.sum;
#if 0
		if(awbDebugCnt){
			printf("%bu(%lx,%lx,%lx)\n",i,pt[i].wp.sum,pt[i].rg.sum,pt[i].bg.sum);
			}
#endif
	}
	for(i=1;i<5;i++)
	{
		wp0[i] = pt[i].wp.sum-pt[i-1].wp.sum;
		rg0[i] = pt[i].rg.sum-pt[i-1].rg.sum;
		bg0[i] = pt[i].bg.sum-pt[i-1].bg.sum;
	}
	for(i=0;i<5;i++)
	{
		wp[i] = wp0[i]+wp0[i+1];
		wpsum[i] = pt[i].wp.sum;
	}
	wp[5]=0;
#if 0
	if(awbDebugCnt){
		awbPrintfFilter("(%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx,%4lx\n)"	,sum->wp[0],sum->wp[1],sum->wp[2],sum->wp[3],sum->wp[4],sum->wp[5],sum->wp[6],sum->wp[7],sum->wp[8],sum->wp[9]	);
	}
#endif
	#else
	wp0[AWB_SUM_A] = pt[1].wp.sum;
	wp0[AWB_SUM_4] = pt[2].wp.sum - pt[0].wp.sum;
	wp0[AWB_SUM_CW] = pt[3].wp.sum - pt[1].wp.sum;
	wp0[AWB_SUM_5] = pt[4].wp.sum - pt[2].wp.sum;
	wp0[AWB_SUM_D] = pt[5].wp.sum;
	
	rg0[AWB_SUM_A] = pt[1].rg.sum;
	rg0[AWB_SUM_4] = pt[2].rg.sum - pt[0].rg.sum;
	rg0[AWB_SUM_CW] = pt[3].rg.sum - pt[1].rg.sum;
	rg0[AWB_SUM_5] = pt[4].rg.sum - pt[2].rg.sum;
	rg0[AWB_SUM_D] = pt[5].rg.sum;

	bg0[AWB_SUM_A] = pt[1].bg.sum;
	bg0[AWB_SUM_4] = pt[2].bg.sum - pt[0].bg.sum;
	bg0[AWB_SUM_CW] = pt[3].bg.sum - pt[1].bg.sum;
	bg0[AWB_SUM_5] = pt[4].bg.sum - pt[2].bg.sum;
	bg0[AWB_SUM_D] = pt[5].bg.sum;

	wp0[5] = 0;
	rg0[5] = 0;
	bg0[5] = 0;

	for(i=6;i<10;i++)
	{
		//wp[i] = pt[i].wp.sum;//-pt[i-1].wp.sum;
		wp0[i] = pt[i].wp.sum;//-pt[i-1].wp.sum;
		rg0[i] = pt[i].rg.sum;//-pt[i-1].rg.sum;
		bg0[i] = pt[i].bg.sum;//-pt[i-1].bg.sum;
	}
	
	memcpy(wp,wp0,4*10);
	#endif
	//wp0[0] = pt[0].wp.sum;
	//rg0[0] = pt[0].rg.sum;
	//bg0[0] = pt[0].bg.sum;
	
	//wp0[5] = pt[5].wp.sum;
	//rg0[5] = pt[5].rg.sum;
	//bg0[5] = pt[5].bg.sum;
	/*
	wp[AWB_SUM_A] = pt[1].wp.sum;
	wp[AWB_SUM_4] = pt[2].wp.sum - pt[0].wp.sum;
	wp[AWB_SUM_CW] = pt[3].wp.sum - pt[1].wp.sum;
	wp[AWB_SUM_5] = pt[4].wp.sum - pt[2].wp.sum;
	wp[AWB_SUM_D] = pt[5].wp.sum;
	*/
	/*
	for(i=3;i>0;i--)
	{
		wp[i]=wp[i]	-wp[i-1];
		rg[i]=rg[i]	-rg[i-1];
		bg[i]=bg[i]	-bg[i-1];
	}
	*/
	for(i=len;i<len+10;i+=2)
	{
		*ob++=pt32[i];
	}
#endif
#endif
	
#if 0
	if(awbDebugCnt){
		for(i=0;i<6;i++){
			printf("%bu(%lx,%lx,%lx)\n",i,sum->wp0[i],sum->rg0[i],sum->bg0[i]);
			//printf("%bu-(%lx,%lx,%lx)(%lx,%lx,%lx)\n",i,sum->wp[i],sum->rg[i],sum->bg[i],buf[i],buf[i+10],buf[i+20]);
			}
	}
#endif
}

void awbPostRect(UINT16 left,UINT16 right,UINT16 bottom,UINT16 top){
	UINT8 xdata* buf = G_FPGAMemPool;
	awb_moe_raw_t xdata* raw = buf;
	awb_moe_rect_t xdata* rect = buf;
	UINT32 addrQueue = K_SDRAM_ShadowAddr+((UINT32)buf-0x4000)/2;
	UINT8 bayerpv = awb.rawFmt & 0x0f;
	UINT8 bayersn = awb.rawFmt >> 4;
	UINT32 wp0,rg0,bg0;
	UINT16 rg = 550;
	UINT16 bg = 550;
	UINT16 dd = 500;
	UINT16 r0,r1,b0,b1;
	UINT32 size = sizeof(awb_moe_raw_t);
	UINT32 cnt = sizeof(awb_moe_raw_t)/sizeof(moe_queue_t);
	UINT32 addr = AWB_ADDR_Out+ 12ul*(0+6);
	awbReadWindow();
	//UINT32 addrTmp = K_SDRAM_CommonEndAddr+1024;
	//printf("addrQueue:%lx,%lx,%lu\n",addrQueue,K_SDRAM_ShadowAddr,cnt);
	awbMoeRaw(raw,bayerpv);
	HAL_MoeSetQueueEnd((moe_queue_t*)raw,cnt-1);
	//dramFileSave("AAA", addrQueue, size);
	HAL_MoeRunQueue(addrQueue, MOE_Trigger_Awb);
	
	size = sizeof(awb_moe_rect_sum_t);
	cnt = size/sizeof(moe_queue_t);
	//printf("AAA:%lx\n",addrQueue,cnt);
	awbMoeRect((awb_moe_rect_t*)rect,0,2);
	HAL_MoeSetQueueEnd((moe_queue_t*)rect,cnt-1);
	while(dd>=50){		
		if(rg<dd || bg<dd){dd>>=1;}
		r0 = rg-dd;
		r1 = rg+dd;
		b0 = bg-dd;
		b1 = bg+dd;
		if( r0<left){r0 = left;}
		if( r1>right){r1 = right;}
		if( b0<bottom){b0 = bottom;}
		if( b1>top){b1 = top;}
		moeCpWrite(rect->rect_left,	r0);	
		moeCpWrite(rect->rect_right,r1);
		moeCpWrite(rect->rect_down,b0);
		moeCpWrite(rect->rect_up,	b1);		
		
		HAL_MoeRunQueue(addrQueue, MOE_Trigger_Awb);

		HAL_DramReadBytesAddr(addr,(UINT8*)&wp0,4,2);
		HAL_DramReadBytesAddr(addr+4,(UINT8*)&rg0,4,2);
		HAL_DramReadBytesAddr(addr+8,(UINT8*)&bg0,4,2);

		//printf("(%u,%u,%u)",rg,bg,dd);
		if(wp0){
			rg = rg0/wp0;
			bg = bg0/wp0;
		}else{
			break;
		}
		printf("(%u,%u)\n",rg,bg);
		dd>>=1;
	}
	awb.avgRg = rg;
	awb.avgBg = bg;
	awbGainSet(awb.avgRg, awb.avgBg);
}
void awbPostSpots()
{
	UINT8 max0,max1;
	awb_filter_sum_t rst0/*,rst1*/,*rst;	
	UINT8 scene = awb.scene;
	UINT8 specMode = awb.specMode;

	//UINT8 max00,max01,max10,max11;
	awb.flowFlag+=AWB_FLAG_Spot;	

	awbReadWindow();
	HAL_MoeRunQueue(pawbaddr->queue[awb.scene],MOE_Trigger_Awb);	
	awbFilterResult(&rst0);	
	rst = &rst0;
	awbSpotsWp(rst,&max0,&max1);
#if 0
	//indoor	
	awbReadWindow();
	HAL_MoeRunQueue(pawbaddr->queue[AWB_SCENE_Normal],MOE_Trigger_Awb);	
	awbFilterResult(&rst0);	

	//outdoor	
	awbReadWindow();
	HAL_MoeRunQueue(pawbaddr->queue[AWB_SCENE_Outdoor],MOE_Trigger_Awb);	
	awbFilterResult(&rst1);	

	awb.sceneIdx = 0;
	awbSceneUpdate((scene == AWB_SCENE_Normal)?&rst0:&rst1);
	scene = awb.scene;
	
	awb.scene = AWB_SCENE_Normal;
	awb.specMode = 0;
	awbSpotsWp(&rst0,&max00,&max01);
	
	awb.scene = AWB_SCENE_Outdoor;
	awb.specMode = 0;
	awbSpotsWp(&rst1,&max10,&max11);
	
	if(scene == AWB_SCENE_Normal){
		if(rst1.wp[AWB_SUM_D]>AWB_SAMP_CNT/5){
			//to outdoor
			scene = AWB_SPEC_Outdoor;awb.specMode = 0;
		}else if(rst1.wp[AWB_SUM_5]+rst1.wp[AWB_SUM_D]>AWB_SAMP_CNT/3){
			//to outdoor
			scene = AWB_SPEC_Outdoor;awb.specMode = 0;
		}
	}
	awb.scene = scene;
	#if 0
	//printf("A0\n");
	if(awb.gid>=(awb.brightGid)){
		specMode= AWB_SPEC_Outdoor;
	}else if(awb.contrast > awb.brightContrast	){//printf("B0\n");
		specMode= AWB_SPEC_Outdoor;	
	}else if(awb.gid<=awb.brightGid/3){	//printf("B1\n");
		specMode = AWB_SPEC_Indoor;
	}else if(awb.contrast < awb.darkContrast	){//printf("B2\n");
		specMode= AWB_SPEC_Indoor;	
	}else if(max00==max10){//printf("B3\n");
		//scene not change
	}else if(max00<2){//printf("B4\n");
		specMode = AWB_SPEC_Indoor;
	}else if(specMode&AWB_SPEC_Outdoor){//printf("B5\n");
		specMode= AWB_SPEC_Outdoor;
	}else if(specMode&AWB_SPEC_Indoor){//printf("B6\n");
		specMode = AWB_SPEC_Indoor;	
	}else if(scene == AWB_SCENE_Normal){
		if (rst0.wp[AWB_SUM_Sward]>AWB_SAMP_CNT/5){
			//to outdoor
		  	specMode = AWB_SPEC_Sward;
		}else if(rst0.wp[AWB_SUM_Sky]>AWB_SAMP_CNT/10){
			//to outdoor
			specMode = AWB_SPEC_Sky;
		}else if(rst0.wp[AWB_SUM_Office]>AWB_SAMP_CNT/10){
			//not change
			specMode = AWB_SPEC_Office;
		}else if(rst1.wp[AWB_SUM_D]>AWB_SAMP_CNT/5){
			//to outdoor
			specMode = AWB_SPEC_Outdoor;
		}else if(rst1.wp[AWB_SUM_5]+rst1.wp[AWB_SUM_D]>AWB_SAMP_CNT/3){
			//to outdoor
			specMode = AWB_SPEC_Outdoor;
		}else{
			//not change
			specMode = AWB_SPEC_Indoor;
		}
		//printf("A10\n");
	}else if(scene == AWB_SCENE_Outdoor){	
		if(rst1.wp[AWB_SUM_Sward]>AWB_SAMP_CNT/10){
			//scene not change
			specMode = AWB_SPEC_Sward;
		}else if(rst1.wp[AWB_SUM_Sky]>AWB_SAMP_CNT/10){
			//scene not change
			specMode = AWB_SPEC_Sky;
		}else if(rst0.wp[AWB_SUM_Office]>AWB_SAMP_CNT/5){		
			//to indoor
			specMode = AWB_SPEC_Office;
		}else{
			specMode = AWB_SPEC_Outdoor;
		}
		//printf("A20\n");
	}
	if(specMode== AWB_SPEC_Indoor){
		if(rst0.wp[AWB_SUM_Office]>AWB_SAMP_CNT/20){
			specMode = AWB_SPEC_Office;
		}
	}else if(specMode== AWB_SPEC_Outdoor){
		if (rst0.wp[AWB_SUM_Sward]>AWB_SAMP_CNT/20){
			//to outdoor
		  	specMode = AWB_SPEC_Sward;
		}else if(rst0.wp[AWB_SUM_Sky]>AWB_SAMP_CNT/20){
			//to outdoor
			specMode = AWB_SPEC_Sky;
		}
	}
	if(specMode&AWB_SPEC_Outdoor){
		awb.scene = AWB_SCENE_Outdoor;
		if((specMode & AWB_SPEC_Outdoor) != AWB_SPEC_Outdoor){
			awb.specMode = specMode;
		}
		
	}else if(specMode&AWB_SPEC_Indoor){
		awb.scene = AWB_SCENE_Normal;	
		if((specMode & AWB_SPEC_Indoor) != AWB_SPEC_Indoor){
			awb.specMode = specMode;
		}
		
	}
	#endif
	if(awb.scene == AWB_SCENE_Normal){
		rst = &rst0;max0=max00;max1=max01;
	}else{
		rst = &rst1;max0=max10;max1=max11;
	}
#endif
	
//printf("A30\n");
//printf("%bu,%bu,%bu\n",scene,awb.scene,awb.specMode);
	if(!awbSpotsLack(rst,max0)){			
		UINT8 idx = awbSpotsCase(rst, max0, max1);
		awbSpotsAvg(rst,idx);
		awbSpotsSpecial(rst,idx);	
		//printf("++%bu,%bu\n",max0,idx);
		//awb.flowFlag+=idx;
	}		
	awb.avgRg = (awb.avgRg+awb.setRg)>>1;
	awb.avgBg = (awb.avgBg+awb.setBg)>>1;
	if(awb.mode){
		if(awb.avgRg<awb.fixRg0){awb.avgRg=awb.fixRg0;}
		else if(awb.avgRg>awb.fixRg1){awb.avgRg = awb.fixRg1;}
		if(awb.avgBg<awb.fixBg0){awb.avgBg=awb.fixBg0;}
		else if(awb.avgBg>awb.fixBg1){awb.avgBg = awb.fixBg1;}
	}
	//printf("(%u,%u)(%u,%u)\n",awb.avgRg,awb.avgBg,awb.setRg,awb.setBg);
	awbGainSet(awb.avgRg, awb.avgBg);
}
#endif

