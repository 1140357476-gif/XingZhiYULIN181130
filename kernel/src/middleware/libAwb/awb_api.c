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
#include "awb_func.h" 
#include "awb_ver.h"
//#include "sp1k_fd_api.h"
//#include "sp1k_ae_api.h"

#include "hal_gprm.h"
#include "reg_def.h"

#include "initio.h"
#include "intr.h"

#include "cardlink.h"
#include "Sp1k_vfs_api.h"
#include "os_api.h"
#include "pv_task.h"
#include "cmd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
awb_addr_t awbAddr _at_ 0x6800;
awb_t awb _at_ 0x6830;
UINT16 awbctmin=2000,awbctmax=7500;

UINT8 awbRunIdx;

UINT8 awbUVSat;//,satEn;
//UINT8 awbProcFlag;
//UINT8 awbProcIdx;
//UINT8 awbCmA[18],awbCmD[18];
UINT16 awbRectRange[4];
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/




/**
 * @fn        UINT16 awbGlobleGainSetIntr(UINT16 gain)
 * @brief     set globe gain* awb gain  to cdsp(using for Intr)
 * @param     [in] gain :base on 256, if gain==0xffff,return globle gain only
 * @retval    last globle gain
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
awbGlobeGainSetIntr(
	UINT16 gain
	)
{
	if(gain==0){
		if(awb.newGain==1){//update gain for VD Rising 
			awbGainSetIntr(0xffff, 0xffff);
			awb.newGain=0;
		}
	}else if(gain!=0xffff){
		awb.globeGain = gain;
		awbGainSetIntr(0xffff, 0xffff);	
		awb.newGain=0;
	}
	return awb.globeGain;
}

//#define sp1kAwbInit_Print	printf
#ifndef sp1kAwbInit_Print
	#define sp1kAwbInit_Print	/_printf
#endif

void sp1kAwbInit()
{
	//UINT32 t0,t1;
	//HAL_GlobalTimerRead10us(&t0);
	sp1kAwbInit_Print(BLUE("AwbInit\n"));		
	
	if(sizeof(awb_ctt_t)>AWB_SIZE_Ctt){
		sp1kAwbInit_Print(RED("CTT:%lu>%lu\n"),(UINT32)sizeof(awb_ctt_t),(UINT32)AWB_SIZE_Ctt);
	}
	//printf("awbInit:%bx,%x\n",isCreate,(UINT16)sizeof(awb_t));
	awbTimerGet(awbTimer.init+0);
	HAL_CdspAWBwinAddrset( K_SDRAM_PvAwbWndAddr );
	HAL_CdspAWBWndGainSet(0x40,0x40);
	//HAL_CdspWBGainSet(0x40,0x40,0x40,0x40);
	
	HAL_CdspAWBWinEn( 1, 0, 0, 1 ,0);
	HAL_CdspWBOffsetCfgSet(1,1);
	HAL_CdspWBOffsetEnSet(1);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0);
	
	HAL_MoeReset();
	awbTimerGet(awbTimer.init+1);
	awbInit();

	awbTimerGet(awbTimer.init+2);
	sp1kAwbModeSet(AWB_MODE_Force);	

	if(awb.cmEn)	{		
		sp1kNandRsvRead(AWB_ID_CM_A, AWB_ADDR_Tmp<<1);
		//HAL_DramWriteBytesAddr(AWB_ADDR_CM+18, &XBYTE[0x2192], 18, 0);
		HAL_DramReadBytesAddr(AWB_ADDR_Tmp, awbCmA, 18, 0);
		memcpy(awbCmD,&XBYTE[REG_Cdsp_ColorM_A00_low],18);
	}
	if(awb.obEn){
		UINT16 ob[4];
		HAL_CdspWBOffsetGet(ob,ob+1,ob+2,ob+3);
		awb.ob0[0] = (ob[0]>0x00ff)?(UINT8)ob[0]:0xff;
		awb.ob0[1] = (ob[1]>0x00ff)?(UINT8)ob[1]:0xff;
		awb.ob0[2] = (ob[2]>0x00ff)?(UINT8)ob[2]:0xff;
		awb.ob0[3] = (ob[3]>0x00ff)?(UINT8)ob[3]:0xff;
	}
	if(awb.satEnDark){
		awbUVSat = XBYTE[REG_Cdsp_UVsat];
		if(awbUVSat == 0)awbUVSat = 0x20;
	}
	HAL_MoeInit();
	awbTimerGet(awbTimer.init+3);
	//awb.initComplete=1;
		
	//printf(",addr(%lx,%lx,%lx)(%lx,%lx,%lx)"	,pawbaddr->buf,pawbaddr->wnd,pawbaddr->moe	,K_SDRAM_DPSStartAddr,K_SDRAM_PvAwbWndAddr,K_SDRAM_PvMoeTmpAddr,K_SDRAM_DPSStartAddr,K_SDRAM_AviAwbWndAddr,K_SDRAM_AviMoeTmpAddr);
	//printf(",ob:(%bx,%bx,%bx,%bx)\n",awb.ob0[0],awb.ob0[1],awb.ob0[2],awb.ob0[3]);
	//HAL_GlobalTimerRead10us(&t1);
	//printf("INIT:%lx\n",t1-t0);
}
/**
 * @fn        UINT8 sp1kAwbParamSet(UINT8 idx, UINT8 val)
 * @brief     set awb param list
 * @param     [in] idx	:param index
 * @param     [in] val	:param val(0xff:get val)
 * @retval    return = param val
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32 sp1kAwbParamSet(awb_param_t idx,UINT32 val)
{
	//printf("awbParaSet:%4lx,%8lx\n",(UINT32)idx,val);
	switch(idx)
	{
		case AWB_PARA_RunIdx:
			if(val != 0xffffffff)	awbRunIdx = val;
			return awbRunIdx;
		case AWB_PARA_Stable:
			if(val != 0xffffffff)	awb.stable = (UINT8)val;
			return awb.stable & 0x01;
		case AWB_PARA_DarkSatMode:
			if(val == AWB_FUNC_Disable || val == AWB_FUNC_Enable){
				awb.satEnDark = val;
			}
			return awb.satEnDark;
		case AWB_PARA_AeInfoAddr:
			return AWB_ADDR_Ae;
		case AWB_PARA_SpotSwitchDelayCnt:
			if(val != 0xffffffff)	awb.spotDelayCnt = (UINT8)val;
			return awb.spotDelayCnt;
		case AWB_PARA_ColorTemperatureMin:
			if(val != 0xffffffff)	awbctmin= (UINT16)val;
			return awbctmin;
		case AWB_PARA_ColorTemperatureMax:
			if(val != 0xffffffff)	awbctmax= (UINT16)val;
			return awbctmax;
		case AWB_PARA_JumpStep :
			if(val != 0xffffffff)	awb.jumpStep= (UINT8)val;
			return awb.jumpStep;
		break;
		default:
			return 0;
			
	}
}

#if AWB_NoUsed
/**
 * @fn        UINT8 sp1kAwbAutoObSet(UINT8 en,UINT8 thr)
 * @brief     auto crrect wb offset for dark enable
 * @param     [in] en	0:dis, 1:en, 0xff:get status
 * @param     [in] thr	0<thr<20 valid
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kAwbAutoObSet(
	UINT8 en,
	UINT8 thr
	)
{	
	if(en==0xff)
	{
		//return awb.obEn;
	}
	else if(awb.obEn && !en)
	{		
		HAL_CdspWBOffsetSet(
			awb.ob0[0]|0x0300,
			awb.ob0[1]|0x0300,
			awb.ob0[2]|0x0300,
			awb.ob0[3]|0x0300
			);
		awb.obEn = en;
	}
	else if(!awb.obEn && en)
	{
		UINT16 ob[4];
		HAL_CdspWBOffsetGet(ob,ob+1,ob+2,ob+3);
		awb.ob0[0] = (ob[0]>0x00ff)?(UINT8)ob[0]:0xff;
		awb.ob0[1] = (ob[1]>0x00ff)?(UINT8)ob[1]:0xff;
		awb.ob0[2] = (ob[2]>0x00ff)?(UINT8)ob[2]:0xff;
		awb.ob0[3] = (ob[3]>0x00ff)?(UINT8)ob[3]:0xff;
		awb.obEn = en;
		/*printf("ob:(%x,%x,%x,%x)(%bx,%bx,%bx,%bx)\n"	,ob[0],ob[1],ob[2],ob[3],awb.ob0[0],awb.ob0[1],awb.ob0[2],awb.ob0[3]);*/
	}

	if(thr && thr<20)
		awb.obThr = thr;
	
	return awb.obEn;
}
#endif
#if AWB_TIME
	//#define awbTimerView_Print	printf
	#ifndef awbTimerView_Print
	#define awbTimerView_Print	/_printf
	#endif
	
void awbTimerView()
{	
	int i;
	awbTimer.initAll = awbTimer.init[3]-awbTimer.init[0];
	awbTimer.initMoeQueue = awbTimer.init[2] - awbTimer.init[1];

	awbTimer.filterAll = awbTimer.filter[2] - awbTimer.filter[0];
	awbTimer.filterWndCpy = awbTimer.filter[1]-awbTimer.filter[0];
	awbTimer.filterMoeRun = awbTimer.filter[2]-awbTimer.filter[1];

	awbTimer.deciderAll = awbTimer.decider[5] - awbTimer.decider[0];
	awbTimer.deciderReadResult= awbTimer.decider[2]-awbTimer.decider[1];
	awbTimer.deciderUpdateScene= awbTimer.decider[3]-awbTimer.decider[2];
	awbTimer.deciderSpot= awbTimer.decider[4]-awbTimer.decider[3];
	#if AWB_TIME_Printf
	for(i=0;i<10;i++)
	{
		awbTimerView_Print("timer:%8lu,%8lu,%lu\n",awbTimer.init[i],awbTimer.filter[i],awbTimer.decider[i]);
	}
	awbTimerView_Print("init:%lu,queue:%lu\n",awbTimer.initAll,awbTimer.initMoeQueue);
	awbTimerView_Print("filter:%lu,wnd:%lu,moe:%lu\n",awbTimer.filterAll,awbTimer.filterWndCpy,awbTimer.filterMoeRun);
	awbTimerView_Print("decider:%lu,res:%lu,scene:%lu,spot%lu\n",awbTimer.deciderAll,awbTimer.deciderReadResult,awbTimer.deciderUpdateScene,awbTimer.deciderSpot);
	#endif
}
#endif
/**
 * @fn        void awbFileInfo(UINT32 addr)
 * @brief     save awb info to exif(call by snap)
 * @param     [in] addr
 * @retval    return awb info addrW(word)
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32 awbFileInfo(UINT32 addrW)
{
//printf("\n\nawbFileInfo %lx\n\n",addrW);	
	HAL_DramWriteBytesAddr(AWB_ADDR_Param, &awb, sizeof(awb), 0);
#if AWB_TIME
	awbTimerView();
	HAL_DramWriteBytesAddr(AWB_ADDR_Timer, &awbTimer, sizeof(awb_timer_t), 2);
#endif
	if(addrW)  {  		
		//HAL_GprmDramDma(AWB_ADDR_Info<<1,addrW<<1,1024*4,1);
		//HAL_GprmDramDma((K_SDRAM_JPG_FdbParameterBufAddr+K_SDRAM_JPG_FdbParameterBufSize-512*4)<<1, (addrW<<1)-1024*4, 1024*4, 1);
	}	
	return AWB_ADDR_Info; 
}


UINT16 G_awbRGainAdjPer = 256;
UINT16 G_awbBGainAdjPer = 256;
/*adjust base is 256*/
void sp1kAwbGainAdjust(UINT16 rGainPer , UINT16 bGainPer)
{
	if(rGainPer > 170 && bGainPer > 170 && rGainPer < 350 && bGainPer < 350){
		G_awbRGainAdjPer = rGainPer; 
		G_awbBGainAdjPer = bGainPer;
	}
	else{
		// printf("AWB adj per set ERR!\n");
	}
}



/**
 * @fn        void awbGainSetIntr(UINT16 rg, UINT16 bg)
 * @brief     set gain to cdsp(using for Intr)
 * @param     [in] rg	:0xffff set current rgain to cdsp
 * @param     [in] bg	:0xffff set current bgain to cdsp
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbGainSetIntr(UINT16 rg,UINT16 bg)
{
	UINT16 gg=awb.globeGain;	
	
	if(rg == 0xffff){
		rg = awb.setRg;
	}
	if( bg == 0xffff){
		bg = awb.setBg;
	}
	//printf("+%4x,%4x,%4x\n",rg,bg,gg);		
	if(gg==0 || gg==AWB_GLOBE_GAIN_BASE){
		//awb.globeGain=AWB_GLOBE_GAIN_BASE;
		gg=AWB_CDSP_GAIN_BASE;
		
		//rg >>= 2;
		//bg >>= 2;	
		rg = (((UINT32)rg * G_awbRGainAdjPer) >>8/*adjust base is 256*/)>>2;
		bg = (((UINT32)bg * G_awbBGainAdjPer) >>8)>>2;
	}else{
		UINT16 gbase = AWB_GLOBE_GAIN_BASE<<2;
		rg = ((UINT32)rg * gg) / gbase;
		bg = ((UINT32)bg * gg) / gbase;		
		gg >>= 2;
	}
	rg = rg>0x7ff? 0x7ff: rg;
	bg = bg>0x7ff? 0x7ff: bg;
	gg = gg>0x7ff? 0x7ff: gg;
	//printf("-%4x,%4x,%4x\n",rg,bg,gg);	

	//HAL_CdspWBGainSet(rg,g,bg,g);
	XBYTE[ REG_Cdsp_prgain_low ] = READ8( rg,1 );
	XBYTE[ REG_Cdsp_prgain_high ] = READ8( rg,0 );

	XBYTE[ REG_Cdsp_pgrgain_low ] = READ8( gg,1 );
	XBYTE[ REG_Cdsp_pgrgain_high ] = READ8( gg,0 );

	XBYTE[ REG_Cdsp_pbgain_low ] = READ8( bg,1 );
	XBYTE[ REG_Cdsp_pbgain_high ] = READ8( bg,0 );

	XBYTE[ REG_Cdsp_pgbgain_low ] = READ8( gg,1 );
	XBYTE[ REG_Cdsp_pgbgain_high ] = READ8( gg,0 );
	//HAL_CdspWBGainEnSet(1);
}

/**
 * @fn        void awbGainSet(UINT16 rg, UINT16 bg)
 * @brief     set gain to cdsp
 * @param     [in] rg	:0xffff set current rgain to cdsp
 * @param     [in] bg	:0xffff set current bgain to cdsp
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-5
*/
void awbGainSet(UINT16 rg,UINT16 bg)
{
	UINT16 gg=awb.globeGain;	
	
	if(rg == 0xffff){
		rg = awb.setRg;
	}
	if( bg == 0xffff){
		bg = awb.setBg;
	}
	//printf("+%4x,%4x,%4x\n",rg,bg,gg);		
	if(gg==0 || gg==AWB_GLOBE_GAIN_BASE){
		//awb.globeGain=AWB_GLOBE_GAIN_BASE;
		gg=AWB_CDSP_GAIN_BASE;
		
		//rg >>= 2;
		//bg >>= 2;	
		rg = (((UINT32)rg * G_awbRGainAdjPer) >>8/*adjust base is 256*/)>>2;
		bg = (((UINT32)bg * G_awbBGainAdjPer) >>8)>>2;
	}else{
		UINT16 gbase = AWB_GLOBE_GAIN_BASE<<2;
		rg = ((UINT32)rg * gg) / gbase;
		bg = ((UINT32)bg * gg) / gbase;		
		gg >>= 2;
	}
	rg = rg>0x7ff? 0x7ff: rg;
	bg = bg>0x7ff? 0x7ff: bg;
	gg = gg>0x7ff? 0x7ff: gg;
	//printf("-%4x,%4x,%4x\n",rg,bg,gg);	

	HAL_CdspWBGainSet(rg,gg,bg,gg);
	//HAL_CdspWBGainEnSet(1);
}

void awbCMSet(awb_cm_t mode)
{
#if 0
{
	int i;
	printf("cm%bx,%bx,%bx\n",awb.cmEn,mode,awb.spotIdx);
	for(i=0;i<18;i++){
		printf("%bx ",XBYTE[REG_Cdsp_ColorM_A00_low+i]);
	}
	printf("\n");
}
#endif	
	if(awb.cmEn!=AWB_FUNC_Enable) return;
	switch(mode)
	{		
		//case AWB_CM_INIT:
		//	sp1kNandRsvRead(AWB_ID_CM_A, AWB_ADDR_CM<<1);
		//	HAL_DramWriteBytesAddr(AWB_ADDR_CM+18, &XBYTE[0x2192], 18, 0);
		//	return;
		case AWB_CM_A:
			//if(awb.cmEn!=AWB_FUNC_Enable) return;
			if(awb.spotIdx>1)return;
			memcpy(&XBYTE[REG_Cdsp_ColorM_A00_low],awbCmA,18);
			//HAL_DramReadBytesAddr(AWB_ADDR_CM, &XBYTE[0x2192], 18, 0);
			//HAL_CdspYEdgeEnSet(0);	/*will: turn of YEdge*/
			break;
		/*case AWB_CM_CW:
			//if(awb.cmEn!=AWB_FUNC_Enable)  return;
			if(awb.spotIdx>1)return;
			memcpy(&XBYTE[0x2192],awbCmCw,18);
			//HAL_DramReadBytesAddr(AWB_ADDR_CM+9, &XBYTE[0x2192], 18, 0);
			break;*/
		case AWB_CM_D:
			//if(awb.cmEn!=AWB_FUNC_Enable)  return;
			//if(awb.spotIdx>1)return;
			memcpy(&XBYTE[REG_Cdsp_ColorM_A00_low],awbCmD,18);
			//HAL_DramReadBytesAddr(AWB_ADDR_CM+18, &XBYTE[0x2192], 18, 0);
			break;	
		case AWB_CM_Auto:
			break;	

	}
#if 0	
	//printf("cm%bx,%bx:",awb.cmEn,mode);
	for(i=0;i<18;i++)
		printf("%bx ",XBYTE[0x2192+i]);
	printf("\n");
#endif
}

//#define awbEnable_Print	printf
#ifndef awbEnable_Print
#define awbEnable_Print	/_printf
#endif

UINT8 
awbEnable(
	UINT8 en//0:dis,1:en,2:reset
	)
{
	//return 0;
	awbEnable_Print(BLUE("awb en:%bx\n"),en);
	
	if(en==2){
		en=1;
		awb.queueCreate = 0;
	}
	if(en==1){		
		UINT8 proc = osSysModeGet();
		awb_addr_t addr;
		//awb.passIdx=3;
		awbInitStatus(&addr);
		if(!awbInitCheck(&addr))awbInit();
		if(proc==OS_SYS_MODE_STILL_PV){			
			awb.stepCnt = 4;
		}else{
			awb.stepCnt = 8;
		}
		awb.passIdx = 2;
		//awb.jumpIdx = 1;
	}
	return sp1kVdProcEnable(VD_PROC_AWB,en)?1:0;
}

/**
 * @fn        UINT8 sp1kAwbModeSet(UINT8 mode)
 * @brief     sp1kAwbModeSet
 * @param     [in] mode :0xff return current mode
 * @retval    return = last mode
 * @author    wangjg
 * @since     2010-5-5
*/

//#define sp1kAwbModeSet_Print		printf
#ifndef sp1kAwbModeSet_Print
#define sp1kAwbModeSet_Print	/_printf
#endif

UINT8 sp1kAwbModeSet(UINT8 para)
{
	code UINT8 modes[] = {
		//spot		fixWp scene idx
		 1,1,1,1,1  ,0  ,0,0		//auto
		,4,1,0,0,0  ,12  ,0,0xff	//Tungsten
		,2,4,1,0,0  ,12  ,0,0xff	//Fluorescent_L
		,0,1,2,1,0  ,12  ,0,0xff	//Fluorescent
		,0,0,1,4,2  ,12  ,0,0xff	//Fluorescent_H
		,0,0,0,1,4  ,13  ,0,0xff	//DayLight

		,0,0,0,1,4  ,20  ,1,0xff	//sunny
		,0,0,0,1,7  ,13  ,1,0xff	//cloudy
		,0,0,0,1,7  ,12  ,1,0xff	//sunset
		,0,0,1,3,6  ,12  ,1,0xff	//sward
		,0,0,3,6,1  ,12  ,1,0xff	//forest
		
	};
	UINT8 mode = para;
	UINT8* pt = modes+mode*8;
	UINT8 rst = awb.mode;
	

	//UINT16 *rg = (UINT16*)G_ucStorData;
	//UINT16 rg[8];
	//UINT16 *bg = rg + 4;
	UINT16 *rg=awb.ctcRg;
	UINT16 *bg=awb.ctcBg;
	UINT8 idx = 0;
	UINT16 rgw,bgw;
	
	sp1kAwbModeSet_Print(BLUE("awbMode:%bx,%bx\n"),awb.mode,mode);
	if(mode==AWB_MODE_Force){
		mode = awb.mode;
	}else if(mode == awb.mode){
		return mode;
	}
	//HAL_DramReadBytesAddr(AWB_ADDR_Ctt+8, rg, 16, 0);
	switch(mode)
	{	
	case AWB_MODE_Get:
		return rst;
		/*
	case AWB_MODE_Enable:
		sp1kAwbEnable();
		return rst;
	case AWB_MODE_Disable:
		sp1kAwbDisable();
		return rst;
		*/
	//Auto
	case AWB_MODE_Spots:		
		awb.fixRg = rg[2];//(rg[1]+rg[2])/2;
		awb.fixBg = bg[2];//(bg[1]+bg[2])/2;
		idx = 0;
		break;
	//meun	
	/* indoor */
	case AWB_MODE_Tungsten:		
		awb.fixRg=rg[0];
		awb.fixBg=bg[0];
		idx = 1;
		break;
	case AWB_MODE_Fluorescent_L:		
		awb.fixRg=(rg[1]+rg[0])/2;
		awb.fixBg=(bg[1]+bg[0]*2)/3;
		idx = 2;
		break;
	case AWB_MODE_Fluorescent:		
		awb.fixRg=rg[1];
		awb.fixBg=bg[1];
		idx = 3;
		break;
	case AWB_MODE_Fluorescent_H:
		awb.fixRg= rg[2]*1.05;//(rg[1]+rg[2])>>1;
		awb.fixBg=(bg[2]*2+bg[1])/3;
		idx = 4;
		break;
	case AWB_MODE_DayLight:		
		awb.fixRg=rg[2];
		awb.fixBg=bg[2];
		idx = 5;
		break;
		
	/* outdoor */
	case AWB_MODE_Sunny:
		awb.fixRg=(rg[1]+rg[2]*3)>>2;
		awb.fixBg=(bg[1]+bg[2]*3)>>2;	
		idx = 6;
		break;	
	case AWB_MODE_Cloudy:
		awb.fixRg=(rg[1]+rg[2])>>1;//(rg[1]+rg[2]*3)>>2;
		awb.fixBg=bg[2];//(bg[1]+bg[2]*2)/3;	
		idx = 7;
		break;	
	case AWB_MODE_Sunset:		
		awb.fixRg=rg[1];
		awb.fixBg=bg[2];	
		idx = 8;
		break;
	case AWB_MODE_Sward:
		awb.fixRg=(rg[2]*2+rg[1])/3;
		awb.fixBg=(bg[2]*2+bg[1])/3;
		idx = 9;
		break;
	case AWB_MODE_Forest:	
		awb.fixRg=(rg[1]*2+rg[2])/3;
		awb.fixBg=(bg[1]*2+bg[2])/3;
		idx = 10;
		break;
	// rect	
	case AWB_MODE_Calibation:
		//awb.scene = AWB_SCENE_Calibration;
		//awb.sceneIdx = 0xff;
		//awb.jumpIdx=1;
		awb.proc = 2;
		return rst;
		//break;
	case AWB_MODE_Flash:
		//awb.scene = AWB_SCENE_Flash;
		//awb.sceneIdx = 0xff;
		//awb.jumpIdx=1;
		awb.proc = 1;
		return rst;
		
	default:
		return rst;
	}

	pt=modes+idx*8;
	awb.spotEn[AWB_SUM_A]	=pt[0];
	awb.spotEn[AWB_SUM_4]	=pt[1];
	awb.spotEn[AWB_SUM_CW]	=pt[2];
	awb.spotEn[AWB_SUM_5]	=pt[3];
	awb.spotEn[AWB_SUM_D]	=pt[4];		
	awb.fixWp 				=pt[5];
	awb.scene				=pt[6];
	awb.sceneIdx 			=pt[7];
	rgw = (UINT32)awb.fixRg*awb.fixWp>>7;
	bgw = (UINT32)awb.fixBg*awb.fixWp>>7;
	awb.fixRg0 = awb.fixRg-rgw;
	awb.fixRg1 = awb.fixRg+rgw;
	awb.fixBg0 = awb.fixBg-bgw;
	awb.fixBg1 = awb.fixBg+bgw;
	
	awb.mode = mode;	
	awb.jumpIdx=1;
	awb.proc = 0;

	if(para || mode==AWB_MODE_Force){
		awb.setRg = awb.avgRg = awb.tagRg = awb.fixRg;
		awb.setBg = awb.avgBg = awb.tagBg = awb.fixBg;
		awb.gainSetCnt = 0;
		awbGainReset();
	}
	return rst;
}
#if AWB_CALIBRATION
void awbCalibStart()
{
	extern UINT8 G_AEEN;
	G_AEEN = 0;
	awbEnable(0);
	sp1kAwbInit();	
	awbEnable(0);
/*	printf("(%u,%u)(%u,%u)(%u,%u)(%u,%u)\n"
		,awb.ctcRg[0],awb.ctcBg[0]
		,awb.ctcRg[1],awb.ctcBg[1]
		,awb.ctcRg[2],awb.ctcBg[2]
		,awb.ctcRg[3],awb.ctcBg[3]
		);	 */
	memset(awb.ctcRg,0,16);
}
void awbCalibDo(UINT8 idx)
{
//	printf("--%bu\n",idx);
	awbPostRect(100,800,100,800);
	awb.ctcRg[idx] = awb.avgRg;
	awb.ctcBg[idx] = awb.avgBg;
//	printf("--%u,%u\n",awb.setRg,awb.setBg);
//	printf("--%bu(%u,%u)\n",idx,awb.setRg,awb.setBg);
}
void awbCalibCtt(UINT8 isCalib)
{	
	UINT32 addr = AWB_ADDR_Ctt;
	UINT8 idx = isCalib?16:8;
	sp1kNandRsvRead(AWB_ID_Ctt, addr<<1);
	HAL_DramWriteBytesAddr(addr+idx,awb.ctcRg, 16, 0);
/*	printf("(%u,%u)(%u,%u)(%u,%u)(%u,%u)\n"
		,awb.ctcRg[0],awb.ctcBg[0]
		,awb.ctcRg[1],awb.ctcBg[1]
		,awb.ctcRg[2],awb.ctcBg[2]
		,awb.ctcRg[3],awb.ctcBg[3]
		);*/
	sp1kNandRsvWrite(AWB_ID_Ctt, addr<<1);
	//sp1kAwbInit(1);
	//awbQueueCreate();
	//awbQueueSave();
	sp1kAwbReset();
}

#endif

void awbPostProc()
{	
#if AWB_ENABLE	
	extern UINT8 awbResultPrintEn;
	awb.flowFlag = AWB_FLAG_Post;
	awbResultPrintEn = 1;
	awbGetAeInfo();
	if(awb.gid>10)awbPostSpots();
	else if(awb.luma>80)awbPostSpots();
	else awbPostRect(awbRectRange[0],awbRectRange[1],awbRectRange[2],awbRectRange[3]);
	awbResultPrintEn = 0;
#endif
}
/**
 * @fn        void awbSvcProc(void* ptr)
 * @brief     main loop call back function
 * @param     [in] ptr
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbSvcProc(void* ptr)
{
#if AWB_ENABLE	
	//UINT8 cnt = awb.stepCnt;
	UINT8 idx = (*(UINT8*)ptr) & 3;
	UINT8 step = (*(UINT8*)ptr) / awb.stepCnt;
	if(idx!=2) //ae
	{
		if(step!=awb.stepIdx || awbRunIdx)	{			
			if(awb.passIdx)
			{
				if(awb.passIdx==1)awbFilter();				
				if(awb.passIdx!=0xff)awb.passIdx--;
			}else{
				awb.stepIdx = step;
				awbFilter();
				awbDecider();
			}			
			if(awbRunIdx)awbRunIdx--;
		}
	}
	awbGainJump();	
#endif
}
