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
extern code UINT8 awbQueueTbl[1024*8] ;
#define pawbParaTbl ((UINT32)awbQueueTbl+1024*7)
const awb_ctt_t * pawbctc = (const awb_t *)(pawbParaTbl+AWB_INFO_CTT_Offset);
const awb_t * pawb = (const awb_t *)(pawbParaTbl+AWB_CTT_PARAM_Offset);
const awb_addr_t * pawbaddr = (const awb_t *)(pawbParaTbl+AWB_INFO_ADDR_Offset);
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define AWB_WRBACK_FW	0
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
void awbInit(){
	awb_addr_t addr;

	//printf("QueueInit\n");
	//printf("%lx,%lx\n",(UINT32)awbQueueTbl,(UINT32)sizeof(awb_moe_normal_t)+sizeof(awb_moe_outdoor_t));
	//printf("%lx,%lx,%lx\n",(UINT32)AWB_INFO_CTT_Offset,(UINT32)AWB_CTT_PARAM_Offset,(UINT32)AWB_INFO_ADDR_Offset);
	//printf("%lx,%lx,%lx,%lx,%lx\n",(UINT32)awbQueueTbl,(UINT32)pawbctc,(UINT32)pawb,(UINT32)pawbaddr,(UINT32)sizeof(pawbaddr));
	//UINT32 t0,t1;
	//HAL_GlobalTimerRead10us(&t0);
	awbInitStatus(&addr);
	if(awbInitCheck(&addr)){
		printf(BLUE("Code\n"));
		memcpy(&awb,pawb,sizeof(awb_t));
		//memcpy(&awbAddr,pawbaddr,sizeof(awb_addr_t));
	}else{
		XBYTE[REG_Dram_AddressErr] &= ~0x01;
		sp1kNandRsvRead(AWB_ID_Queue,addr.que<<1);
		//HAL_GprmDramDmaAdv(pawbaddr->moe<<1, pawbaddr->que<<1, 1024*8, 1);
		if(awbInitCheck(&addr)){
			printf(BLUE("Load\n"));
			memcpy(&awb,pawb,sizeof(awb_t));
		}else{
			//UINT8 xdata* buf = G_ucStorData;
			UINT8 buf[256+64];
			printf(BLUE("Create\n"));
			HAL_DramWriteBytesAddr(addr.addr,&addr,sizeof(awb_addr_t),0);
			awbInitCtt(buf,AWB_ID_Ctt,NULL);//,0xff);
			awbMoeQueueCreate(pawbctc);

			//awbInitCheck(&addr);
			sp1kNandRsvWrite(AWB_ID_Queue,pawbaddr->que<<1);
		}
		//dumpData(1024*8, addr.que, 1);
#if AWB_WRBACK_FW
		codeWriteBack(0,(UINT16)awbQueueTbl,pawbaddr->que,1024*8);
#endif
		//dramFileSave("AWB.BIN",pawbaddr->que,8*1024);
		XBYTE[REG_Dram_AddressErr] |= 0x01; //address_err_enable
	}
	//HAL_GlobalTimerRead10us(&t1);
	//printf("Init:%lx\n",t1-t0);
}

UINT8 awbInitCheck(awb_addr_t* paddr){

	/*printf(BLUE("AwbCheck(%lx,%lx)(%lx,%lx)(%lx,%lx)(%lx,%lx)(%bx,%bx,%bx)(%bx,%bx)\n"),
		pawbaddr->buf,paddr->buf
		, pawbaddr->que , paddr->que
		, pawbaddr->wnd,paddr->wnd
		, pawbaddr->moe,paddr->moe
		, pawb->queueCreate, awb.queueCreate,0x5a
		, pawb->rawFmt, awb.rawFmt
		);*/

	return!(	pawbaddr->buf!=paddr->buf
		|| pawbaddr->queue[0] != paddr->queue[0]
		|| pawbaddr->wnd!=paddr->wnd
		|| pawbaddr->moe!=paddr->moe
		|| pawb->queueCreate != awb.queueCreate
		|| pawb->queueCreate != 0x5a
		|| pawb->rawFmt != awb.rawFmt
		);
}
void awbInitStatus(awb_addr_t* paddr){
	UINT16 bayerpv,bayersn;

	///////////////////////////////////////////////////////////////////////////
	// init sensor bayer
	frontDeviceInfoGet(0,FRONT_DEV_INFO_BAYER_FMT,&bayerpv,&bayersn);
	awb.rawFmt = (bayersn<<4) | (bayerpv & 0x0f);

	//////////////////////////////////////////////////////////////////////////
	// init status
	awb.queueCreate = 0x5a;

	///////////////////////////////////////////////////////////////////////////
	// init dram addr
	paddr->que = (32ul*1024*(0+1) + ((UINT16)awbQueueTbl - 0x8000)) >> 1;
	paddr->buf=K_SDRAM_DPSStartAddr;
	paddr->queue[AWB_SCENE_Normal] 		= paddr->que;
	paddr->queue[AWB_SCENE_Outdoor]		= paddr->queue[AWB_SCENE_Normal]	+sizeof(awb_moe_normal_t)/2;
	paddr->queue[AWB_SCENE_RectOne]		= paddr->queue[AWB_SCENE_Outdoor]	+sizeof(awb_moe_outdoor_t)/2;
	paddr->wnd=K_SDRAM_PvAwbWndAddr;
	paddr->moe=K_SDRAM_PvMoeTmpAddr;

	paddr->ctc= paddr->que + ((UINT16)pawbctc-(UINT16)awbQueueTbl)/2;
	paddr->para= paddr->que + ((UINT16)pawb-(UINT16)awbQueueTbl)/2;
	paddr->addr= paddr->que + ((UINT16)pawbaddr-(UINT16)awbQueueTbl)/2;

#if 0
{
	UINT8 i;
	UINT32 xdata* ptdata = (UINT32 xdata*)paddr;
	UINT32 code* ptcode = (UINT32 code*)pawbaddr;
	printf("initAddr\n");
	for(i=0;i<12;i++)printf("%bx-%lx,%lx-%ld\n",i,*ptcode,*ptdata,(SINT32)*ptcode++ - *ptdata++);
}
#endif
}
/**
 * @fn        UINT32 awbCttSet(awb_ctt_t* ctt, UINT8 fileId, UINT8* fname, UINT8 opt)
 * @brief    set awb ctt
 * @param     [in] buf		:source sram
 * @param     [in] fileId		:source awbctt.bin
 * @param     [in] fname	:source file
 * @param     [in] opt		:[0]update moe
 * 						:[1]update awb struct
 *
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-12
 * @todo      N/A
 * @bug       N/A
*/
UINT32
awbInitCtt(
	UINT8* buf,
	UINT8 fileId,
	UINT8* fname//,
	//UINT8 opt	//[0] update moe//[1] update awb
	)
{
	//UINT8 buf[sizeof(awb_ctt_t)+8];
	//UINT8 xdata* buf = G_ucStorData;
	awb_ctt_t xdata* ptCtt = (awb_ctt_t xdata*)(buf+8);//ctt?ctt:(buf+8);
	UINT8 xdata* pt=(UINT8 xdata*)ptCtt;
	UINT16 xdata* rects = ptCtt->rect;
	UINT8 xdata* ratios = ptCtt->ratio;
	UINT16 xdata* ctc = ptCtt->ctc;
	UINT16 xdata* cal = ptCtt->cal;
	UINT16 xdata* rect = rects;
	UINT8 xdata* ratio = ratios;
	awb_t xdata* para = &(ptCtt->param);
	UINT16 r,b,i;
	UINT16 xdata* pt16=(UINT16 xdata*)pt;
	UINT32 xdata* pt32 =(UINT32 xdata*)buf;

	UINT8 pvBayer,snapBayer;
	UINT16 para1, para2;
	//code UINT32 buf[]={0x55aa0000,0x55aaffff};

	//ctt = ctt;
	if(fileId!=0xff){
		sp1kNandRsvRead(fileId, AWB_ADDR_Ctt<<1);
	}
	else if(fname){
		UINT32 size;
		dramFileLoad(fname,AWB_ADDR_Ctt,&size);
		printf("%s\n",fname);
	}

	//if(!ctt){
		//pt=G_ucStorData;
		//UINT16 xdata* pt16=(UINT16 xdata*)pt;
		//memset(pt,0,256);
		//HAL_DramReadBytesAddr(AWB_ADDR_Ctt,pt,AWB_SIZE_Ctt,0);
		HAL_DramDmaRead(pt,AWB_ADDR_Ctt,AWB_SIZE_Ctt,1);
		if(pt16[0]!=0 && pt16[1]==0){//1528 awbctt.bin
			UINT8 xdata* pt8=(UINT8 xdata*)ctc;
			memset(pt+16,0,sizeof(awb_ctt_t)-16);
			pt8[1]=pt[4];
			pt8[0]=pt[5];
			pt8[9]=pt[6];
			pt8[8]=pt[7];
			pt8[3]=pt[8];
			pt8[2]=pt[9];
			pt8[11]=pt[10];
			pt8[10]=pt[11];
			pt8[5]=pt[12];
			pt8[4]=pt[13];
			pt8[13]=pt[14];
			pt8[12]=pt[15];
			memset(pt,0,16);

		}
	pt32[5] = awbVersion;
	//}

	//ctc
	if(!ctc[0])
	{
		#if 0
			code UINT16 pt[]={
				0x105,0x185,0x1c0,0,
				0x213,0x1a9,0x10d,0
			};
		#else
			code UINT16 pt[]={
				242,323,419,419,
				436,363,269,269
			};
		#endif
		memcpy(ctc,pt,16);
	}

	if(cal[0] && cal[1] && cal[2] && cal[4] && cal[5] && cal[6])
	{
		memcpy(ctc,cal,16);
	}
	else if(cal[3] && cal[7] && ctc[3]&& ctc[7])
	{
		r=cal[3]-ctc[3];
		b=cal[7]-ctc[7];

		for(i=0;i<3;i++){
			ctc[i]	+=r;
			ctc[i+4]	+=b;
		}
	}


	//scene
	ratio = ratios + 16*AWB_SCENE_Normal;
	if(!ratio[1])
	{
		code UINT8 pt[]={
			108,140,110,142,110,155,0,0	//rg
			,118,140,115,142,112,155,0,0,	//bg
			};
		memcpy(ratio,pt,16);
	}

	ratio = ratios + 16*AWB_SCENE_Outdoor;
	if(!ratio[1])
	{
		code UINT8 pt[]={
			//118,135,105,128,100,136,0,0	//rg
			//,118,135,110,128,125,150,0,0,	//bg
			//128,128,105,128,100,155,0,0	//rg
			//,128,128,110,128,125,129,0,0,	//bg
			 128,128,105,128,95,135,0,0	//rg
			,128,128,110,128,100,150,0,0,	//bg
			};
		memcpy(ratio,pt,16);
	}
	//rect
	rect = rects+AWB_RECT_Flash*4;
	if(!rect[1])
	{
		rect[0]=(ctc[0]*0.8);//<<6)>>7;	//left
		rect[1]=(ctc[2]*1.3);//<<6)>>5;	//right
		rect[2]=(ctc[6]*0.7);//<<6)>>7;	//down
		rect[3]=(ctc[5]*0.9);//<<6)>>5;	//up
		memcpy(awbRectRange,rect,8);
	}
	rect = rects+AWB_RECT_Sward*4;
	if(!rect[1])
	{
		rect[0]=ctc[1];//(ctc[0]+ ctc[1])>>1;
		rect[1]=(ctc[2]*1.2);//<<6)/53;
		rect[2]=(ctc[5]);//<<6)>>6;
		rect[3]=(ctc[4]*1.2);//<<6)/53;
	}
	rect = rects+AWB_RECT_Sky*4;
	if(!rect[1])
	{
		rect[0]=ctc[2]*0.8;	//left
		rect[1]=ctc[2]*1.2;	//right
		rect[2]=ctc[6]*0.5;	//down
		rect[3]=ctc[6]*0.95;	//up
	}
	rect = rects+AWB_RECT_Office*4;
	if(!rect[1])
	{
		rect[0]=ctc[2];	//left
		rect[1]=ctc[2]*1.28;	//right
		rect[2]=ctc[6]*0.95;	//down
		rect[3]=(ctc[6]+ctc[5]*2)/3;	//up
		/*
		rect[0]=ctc[2]*1.05;	//left
		rect[1]=ctc[2]*1.28;	//right
		rect[2]=(ctc[6]*4+ctc[5])/5;	//down
		rect[3]=(ctc[6]+ctc[5]*2)/3;	//up
		*/
	}
	rect = rects+AWB_RECT_Calibration*4;
	if(!rect[1])
	{
		rect[0]=100;	//left
		rect[1]=1600;	//right
		rect[2]=100;	//down
		rect[3]=1600;	//up
	}
	//if(opt&0x02)//param awb
	//{
		//jumpMax
		if(!para->jumpMax)
		{
			//para->jumpIdx = 1;
			para->jumpMax = 48;
			para->jumpStable = 12;
			para->jumpErr = 128;
		}
		//globeGain
		if(!para->spotThr)		{para->spotThr=5;}
		//aeinfo
		if(!para->brightGid)	{para->brightGid = 56;}
		if(!para->darkGid)	{para->darkGid = 1;}
		if(!para->brightLuma)	{para->brightLuma = 160;}
		if(!para->darkLuma)	{para->darkLuma = 20;}
		if(!para->brightContrast){para->brightContrast = 180;}
		if(!para->darkContrast){para->darkContrast = 140;}

		//if(!para->rangeRg0){para->rangeRg0=100;}
		//if(!para->rangeRg1){para->rangeRg1=800;}
		//if(!para->rangeBg0){para->rangeBg0=100;}
		//if(!para->rangeBg1){para->rangeRg0=800;}

		memcpy(para->ctcRg,ctc,16);
		//para->avgRg = para->setRg = para->fixRg = para->ctcRg[1];
		//para->avgBg = para->setBg = para->fixBg = para->ctcBg[1];
		//para->setRg = para->fixRg>>2;
		//para->setBg = para->fixBg>>2;
		//printf("awb fix:%u,%u\n",awb.setRg,awb.setBg);
		//para->rawFmt = XBYTE[0x2110];
		para->queueCreate = 0x5a;

		//if(!para->aLightMode)	para->aLightMode=AWB_FUNC_Disable;
		//if(!para->cmMode)
		//if(!para->cmEn)		para->cmEn = AWB_FUNC_Disable;
		//if(!para->satEnDark)	para->satEnDark = AWB_FUNC_Disable;
		//if(!para->satEnAlight)	para->satEnAlight = AWB_FUNC_Disable;
		//if(!para->obEn)		para->obEn = AWB_FUNC_Disable;
		//bayer ptn
		frontDeviceInfoGet(0,FRONT_DEV_INFO_BAYER_FMT,&para1,&para2);
		pvBayer = (UINT8)para1;
		snapBayer = (UINT8)para2;
		para->rawFmt = (snapBayer<<4) | (pvBayer & 0x0f);
		if(pvBayer!=snapBayer){printf(RED("RawFmtErr!%bx\n"),para->rawFmt);}
		//printf("awb ptn:%bx\n",awb.rawFmt);
		//update awb
		//para->mode = AWB_MODE_Force;

		if(!para->spotDelayCnt){para->spotDelayCnt=2;}
		//if(!para->ctmin){para->ctmin=2000;}
		//if(!para->ctmax){para->ctmax=7500;}
		memcpy(&awb,para,sizeof(awb_t));

		//auto ob
		//if(!para->obThr)para->obThr = 8;
		//if(awb.obEn)
		//{
		//	para->obEn = 0;//init Auto Ob
		//	sp1kAwbAutoObSet(1,para->obThr);
		//}
		//memcpy(para,&awb,sizeof(awb_t));
	//}

	//HAL_DramWriteBytesAddr(AWB_ADDR_Ctt,pt,AWB_SIZE_Ctt,0);
	//HAL_DramDmaWrite(pt,AWB_ADDR_Ctt,AWB_SIZE_Ctt,0);
	pt32[0] = 0x55aa0000;
	pt32[1] = 0x55aaffff;
	HAL_DramDmaWrite(buf,AWB_ADDR_Flag,AWB_SIZE_Ctt+8,1);
	HAL_DramDmaWrite(buf, pawbaddr->ctc-4, AWB_SIZE_Ctt+8, 1);
	//HAL_DramWriteBytesAddr(AWB_ADDR_Addr,&awbAddr,AWB_SIZE_Addr,0);
	//HAL_DramDmaWrite(&awbAddr,AWB_ADDR_Addr,AWB_SIZE_Addr,0);

	//if(opt&0x01)
	//{
		//awbMoeQueueCreate(ptCtt);
	//}
#if 0
{
	UINT16 i;
	UINT8 code* ptcode = ((UINT8 code*)pawbctc)-8;
	UINT8 xdata* ptdata = buf;
	for(i=0;i<300;i++){
		printf("%u-%bx,%bx-%d\n",i,*ptdata,*ptcode,(SINT16)*ptdata++ - *ptcode++);
	}
	ptcode = ((UINT8 code*)pawb);
	ptdata = &awb;
	for(i=0;i<sizeof(awb);i++){
		printf("%u-%bx,%bx-%d\n",i,*ptdata,*ptcode,(SINT16)*ptdata++ - *ptcode++);
	}
}
#endif
#if AWB_Debug_Print
{
	UINT8 i=0;
	for(i=0;i<4;i+=1){
		printf("-%bx-(%x,%x)\n",i,ctc[i],ctc[i+4]);
	}
	for(i=0;i<48;i+=4){
		printf("-%bx-(%x,%x,%x,%x)\n",i/4,rects[i],rects[i+1],rects[i+2],rects[i+3]);
	}
}
#endif
return AWB_ADDR_Ctt;

}
