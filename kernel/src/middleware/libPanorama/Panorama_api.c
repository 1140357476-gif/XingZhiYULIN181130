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
#include "hal_gprm.h"
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
#include "sp1k_panorama_api.h"
#include "sp1k_snap_api.h"
#include "panorama_def.h"
#include "sys_mem_init.h"
#include "intr.h"
#include "dbg_wdt.h"
#include "panorama_api.h"


#if PANORAMA_EN
/**************************************************************************
 *							 C O N S T A N T S							  *
/**************************************************************************/

#define __FILE	__FILE_ID_PANORAMA_API__


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
xdata UINT8 G_PanoramaEn;//0:PANORAMA_DISABLE, 1:PANORAMA_ENABLE
xdata UINT8 G_MEIntrSts;//0:initial, 1:ME start, 2: ME Done
xdata UINT8 G_PanoramaBufSts;//0:not chang, 1:chang

xdata UINT8 G_MEIntrBufIdx;

PanoramaInf_t G_PanoramaInf = {
	{//parameter
		1,
		1,
		PANORAMA_DEFAULT_STITCH_PIXEL,
	},
	{//frame tmp
		0,
		0,
		0,
		0,
		{0,0,0,0},
		0,
	},
	{//me
		0,
		EDGE_TBL_HBLOCKNUM,
		EDGE_TBL_VBLOCKNUM,
	},
	{//dst
		0,
		0,//2880
		0,//400
	},
	{//final
		0,
		0,
	},
	{//gmv
		0,
		0,
	},
	{//sumofgmv
		0,
		0,
	},
};




/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
/**
 * @fn				  void panoramaBufInit(void)
 * @brief				Initial panorama buffer setting.  
 *\n					The function must be called before the system switch to peview mode
 * @param[in]		none
 * @retval			  none
 */
UINT8 
panoramaParamSet(
	UINT8 selector,
	UINT16 val
)
{
	switch ( selector ) {
		case SP1K_PANORAMA_PARA_FRAME_NUM:
			if ( val > PANORAMA_BUF_MAX_IDX ) {
				G_PanoramaInf.frameTmp.totalNum = PANORAMA_BUF_MAX_IDX;
			}
			else {
				G_PanoramaInf.frameTmp.totalNum = (UINT8)val;
			}
			break;
		case SP1K_PANORAMA_PARA_STITCH_PIXEL:
			G_PanoramaInf.param.stitchPixel = val;
			break;
		default:
			return FAIL;
	}

	return SUCCESS;
}

/**
 * @fn                void panoramaBufInit(void)
 * @brief             	Initial panorama buffer setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            none
 */
void 
panoramaBufInit(
	void
)
{
	UINT16 pvHSize, pvVSize;
	UINT16 secPathVSize;
	UINT8 mapXfac;
	//UINT8 i;
	
	pvSizeGet(&pvHSize, & pvVSize);

	G_PanoramaInf.param.scaleRatio = (UINT8)(pvHSize/EDGE_SRC_YUV_H);

	#if 0
	printf("panoramaBuf(%d,%d)\n",pvHSize,pvVSize);
	#endif

	G_PanoramaInf.frameTmp.hSize = pvHSize;
	G_PanoramaInf.frameTmp.vSize = pvVSize;
	G_PanoramaInf.frameTmp.cdsp1stOneWSize = (UINT32)pvHSize*pvVSize;
	G_PanoramaInf.dst.hSize= panoDstWidthGet(G_PanoramaInf.param.direction);
	G_PanoramaInf.dst.vSize = panoDstHeightGet(G_PanoramaInf.param.direction);	
	
	if (((pvHSize == 320) && (pvVSize == 240))) {
		sysMemStillPvPanoramaAddrUpdate(MEM_FB_QVGA,pvHSize,pvVSize,G_PanoramaInf.frameTmp.totalNum,G_PanoramaInf.dst.hSize,G_PanoramaInf.dst.vSize);
	}
	else if (((pvHSize == 640) && (pvVSize == 480))) {
		sysMemStillPvPanoramaAddrUpdate(MEM_FB_VGA,pvHSize,pvVSize,G_PanoramaInf.frameTmp.totalNum,G_PanoramaInf.dst.hSize,G_PanoramaInf.dst.vSize);
	}
	else if (pvHSize == 960 && pvVSize == 540) {
		sysMemStillPvPanoramaAddrUpdate(MEM_FB_HD_960,pvHSize,pvVSize,G_PanoramaInf.frameTmp.totalNum,G_PanoramaInf.dst.hSize,G_PanoramaInf.dst.vSize);
	}
	else if ((pvHSize == 640) && (pvHSize == 432)) { //modify for D1 size
		sysMemStillPvPanoramaAddrUpdate(MEM_FB_D1,pvHSize,pvVSize,G_PanoramaInf.frameTmp.totalNum,G_PanoramaInf.dst.hSize,G_PanoramaInf.dst.vSize);
	}
	else {
		sysMemStillPvPanoramaAddrUpdate(MEM_FB_HD,pvHSize,pvVSize,G_PanoramaInf.frameTmp.totalNum,G_PanoramaInf.dst.hSize,G_PanoramaInf.dst.vSize);
	}
	
	G_PanoramaInf.meTmp.startBufAddr = K_SDRAM_PvPanoramaTmpMeAddr;
	G_PanoramaInf.dst.bufAddr = K_SDRAM_PvPanoramaDstYuvAddr;
	G_PanoramaInf.frameTmp.startBufAddr = K_SDRAM_PvPanoramaTmpYuvAddr;

	HAL_GprmDramFill(K_SDRAM_PvPanoramaTmpGmvAddr,(UINT32)4*2*(G_PanoramaInf.frameTmp.totalNum+2),0x00);
	HAL_GprmDramFill(G_PanoramaInf.dst.bufAddr, (UINT32)G_PanoramaInf.dst.hSize*2*G_PanoramaInf.dst.vSize,0xFF);

	//dramFileSave("SRCP0005.YUV", G_PanoramaInf.dst.bufAddr, (UINT32)G_PanoramaInf.dst.hSize*2*G_PanoramaInf.dst.vSize);
	
	HAL_GprmDramFill(G_PanoramaInf.meTmp.startBufAddr,(UINT32)12*12*2*2*G_PanoramaInf.frameTmp.totalNum,0x00);
	HAL_GprmDramFill(G_PanoramaInf.frameTmp.startBufAddr, G_PanoramaInf.frameTmp.cdsp1stOneWSize*2*G_PanoramaInf.frameTmp.totalNum,0x8f);
	
	#if 0
	printf("P_BUF(0x%lx,0x%lx,0x%lx,0x%lx)\n",
		G_PanoramaInf.meTmp.startBufAddr ,
		K_SDRAM_PvPanoramaTmpGmvAddr,
		G_PanoramaInf.dst.bufAddr,
		G_PanoramaInf.frameTmp.startBufAddr);
	#endif

	HAL_Dram1stPathFrmBufAddrGet(0, &G_PanoramaInf.frameTmp.cdsp1stBufAddr[0]);
	HAL_Dram1stPathFrmBufAddrGet(1, &G_PanoramaInf.frameTmp.cdsp1stBufAddr[1]);
	HAL_Dram1stPathFrmBufAddrGet(2, &G_PanoramaInf.frameTmp.cdsp1stBufAddr[2]);
	HAL_Dram1stPathFrmBufAddrGet(3, &G_PanoramaInf.frameTmp.cdsp1stBufAddr[3]);

	#if 0
	dramFileSave("SRCP0001.YUV", G_PanoramaInf.frameTmp.cdsp1stBufAddr[0], G_PanoramaInf.frameTmp.cdsp1stOneWSize*2);
	dramFileSave("SRCP0002.YUV", G_PanoramaInf.frameTmp.cdsp1stBufAddr[1], G_PanoramaInf.frameTmp.cdsp1stOneWSize*2);
	dramFileSave("SRCP0003.YUV", G_PanoramaInf.frameTmp.cdsp1stBufAddr[2], G_PanoramaInf.frameTmp.cdsp1stOneWSize*2);
	dramFileSave("SRCP0004.YUV", G_PanoramaInf.frameTmp.cdsp1stBufAddr[3], G_PanoramaInf.frameTmp.cdsp1stOneWSize*2);
	#endif

	#if 0
	printf("CDSP(0x%lx,0x%lx,0x%lx,0x%lx)\n",
		G_PanoramaInf.frameTmp.cdsp1stBufAddr[0],
		G_PanoramaInf.frameTmp.cdsp1stBufAddr[1],
		G_PanoramaInf.frameTmp.cdsp1stBufAddr[2],
		G_PanoramaInf.frameTmp.cdsp1stBufAddr[3]);
	#endif
	
}


/**
 * @fn                void panoramaInit(void)
 * @brief             	Initial panorama setting.  
 *\n                    The function must be called before the system switch to peview mode
 * @param[in]       none
 * @retval            none
 */
void 
panoramaInit(
	void
)
{
	/* panorama buffer initial */
	panoramaBufInit();

	/* motion detect intial */
	mdInit();
}

/**
 * @fn				  void panoramaEnSet(UINT8 en);
 * @brief				To Set Panorama On/off Status function.
 * @param[in]	   en : (The status of Panorama)
 *\n							  PANORAMA_DISABLE: Panorama turn OFF.
 *\n							  PANORAMA_ENABLE_ENABLE: Panorama turn ON.
 * @retval			  none			
 */
void 
panoramaEnSet(
	UINT8 en
)
{
	if (G_PanoramaEn == en) {
		return;
	}
	
	mdEnSet(en);
	
	if ( en ) {
		/* claer ME Start Sts */
		XBYTE[REG_Jpeg_Intr_Event] = 0x60;
		HAL_DistMeIntrEn(1,1);		
	}
	else {
		/* claer ME Start Sts */
		XBYTE[REG_Jpeg_Intr_Event] = 0x60;
		HAL_DistMeIntrEn(0,0);
	}

	G_PanoramaInf.gmv.x = G_PanoramaInf.gmv.y = 0;
	G_PanoramaInf.sogmv.x = G_PanoramaInf.sogmv.y = 0;

	G_PanoramaEn = en;	
	G_MEIntrSts = 0;
	G_PanoramaBufSts = 0;
	G_MEIntrBufIdx = 0;

	if ( en ) {
		panoramaBufSwitchSet();
	}
}


/**
 * @fn                UINT8 panoramaDstFileSave(UINT8 *name);
 * @brief	            To Get Panorama On/off Status function.
 * @param[in]      none
 * @retval          The status of Panorama
 *\n                              PANORAMA_DISABLE: Panorama turn OFF.
 *\n                              PANORAMA_ENABLE_ENABLE: Panorama turn ON.            
 */
UINT8 
panoramaDstFileSave(
	UINT8 *name
	
) 
{
	#if (DBG_DRAMDMA_ME_BY_FRAME == 1)
	#if (DBG_SAVE_ME_BY_FRAME == 1)
	dramFileSave("SPME0001.BIN", G_PanoramaInf.meTmp.startBufAddr, ((UINT32)12*12*2*G_PanoramaInf.frameTmp.totalNum*2));
	#endif
	#endif

	#if (DBG_SAVE_GME_BY_FRAME == 1)
	dramFileSave("SGMV0001.BIN", K_SDRAM_PvPanoramaTmpGmvAddr, ((UINT32)4*(G_PanoramaInf.frameTmp.totalNum+2)*2));
	#endif

	#if (DBG_SAVE_DST_YUV == 1)
	dramFileSave("SDST0001.YUV", K_SDRAM_PvPanoramaDstYuvAddr, ((UINT32)G_PanoramaInf.dst.hSize*G_PanoramaInf.dst.vSize*2));
	#endif

	#if (DBG_SAVE_SRC_YUV_BY_FRAME == 1)
	dramFileSave(name, G_PanoramaInf.frameTmp.startBufAddr, ((UINT32)G_PanoramaInf.frameTmp.cdsp1stOneWSize*G_PanoramaInf.frameTmp.totalNum*2));
	#endif
	
	return SUCCESS;
}


/**
 * @fn                UINT8 panoramaEnGet(void);
 * @brief	            To Get Panorama On/off Status function.
 * @param[in]      none
 * @retval          The status of Panorama
 *\n                              PANORAMA_DISABLE: Panorama turn OFF.
 *\n                              PANORAMA_ENABLE_ENABLE: Panorama turn ON.            
 */
UINT8
panoramaEnGet(
	void
)
{
	return G_PanoramaEn;
}

/**
 * @fn                UINT8 panoramaMeDoneGet(void);
 * @brief	            To Get Panorama ME Done Status function.
 * @param[in]      none
 * @retval          The status of Panorama ME Done
 *\n                              TRUE: Panorama ME Done.
 *\n                              FALSE: Panorama ME Not Done.            
 */
UINT8
panoramaMeDoneGet(
	void
)
{
	return ((G_MEIntrSts == 2) ? TRUE : FALSE);
}

/**
 * @fn                UINT8 panoramaBufSwitchSet(void);
 * @brief	            To Chang Buffer Address function. (Panorama ME Done/1st Path YUV)
 * @param[in]      [idx] buffer index
 * @retval           The status of Buf Switch
 *\n                              SUCCESS: Buf Switch Success.
 *\n                              FAIL: Buf Switch Fail .            
 */
UINT8
panoramaBufSwitchSet(
	void
)
{
	G_PanoramaBufSts = 1;
	return SUCCESS;
}


/**
 * @fn                UINT8 INTR_panoramaBufSwitch(void);
 * @brief	            To Chang Buffer Address function. (Panorama ME Done/1st Path YUV)
 * @param[in]      [idx] buffer index
 * @retval           The status of Buf Switch
 *\n                              SUCCESS: Buf Switch Success.
 *\n                              FAIL: Buf Switch Fail .            
 */
UINT8
INTR_panoramaBufSwitch(
	void
)
{
	UINT8 dispIdx, cdspIdx;
	UINT8 dmaChn = 1;//intr always used 1
	UINT32 src,dst,size;
	UINT32 src1,dst1,size1;

	if ( G_MEIntrBufIdx < G_PanoramaInf.frameTmp.totalNum ) {
		dispIdx = (UINT8)((XBYTE[REG_Dram_MpgSb_CdspWRegIdx]>>4) & 0x03);
		cdspIdx = (UINT8)(XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03);

		if (XBYTE[REG_Gprm_DmaBusy] & dmaChn) {
			return FAIL;
		}	

		src = (G_PanoramaInf.frameTmp.cdsp1stBufAddr[(dispIdx+1)&0x03]<<1);
		dst = ((G_PanoramaInf.frameTmp.startBufAddr+((UINT32)G_MEIntrBufIdx*G_PanoramaInf.frameTmp.cdsp1stOneWSize))<<1);
		size = (G_PanoramaInf.frameTmp.cdsp1stOneWSize<<1);
		
		#if 0
		intrPutChar('[');
		intrPutUint8(G_MEIntrBufIdx);
		intrPutChar(',');
		intrPutUint8(cdspIdx);
		intrPutChar(',');
		intrPutUint8(dispIdx);
		#if 0
		intrPutChar(',');
		intrPutUint32(src);
		intrPutChar(',');
		intrPutUint32(G_PanoramaInf.frameTmp.cdsp1stBufAddr[0]);
		intrPutChar(',');
		intrPutUint32(G_PanoramaInf.frameTmp.cdsp1stBufAddr[1]);
		intrPutChar(',');
		intrPutUint32(G_PanoramaInf.frameTmp.cdsp1stBufAddr[2]);
		intrPutChar(',');
		intrPutUint32(G_PanoramaInf.frameTmp.cdsp1stBufAddr[3]);
		#endif
		intrPutChar(']');
		#endif

		#if (DBG_DRAMDMA_ME_BY_FRAME == 1)
		src1 = ((UINT32)K_SDRAM_PvDistMeAddr<<1);
		dst1 = ((G_PanoramaInf.meTmp.startBufAddr+((UINT32)G_MEIntrBufIdx*12*12*2))<<1);
		size1 = (12*12*2<<1);
		
		HAL_GprmDramDmaExt_Intr(src1,
								dst1,
								size1,
								0, 
								0);
		#endif

		#if (DBG_DRAMDMA_SRC_YUV_BY_FRAME == 1)
		HAL_GprmDramDmaExt_Intr(src,
								dst,
								size,
								0, 
								dmaChn);
		#endif
		
		G_MEIntrBufIdx++;	
		return SUCCESS;
	}
	else {
		osMsgPost(SP1K_MSG_PANORAMA_FINISH);
		return SUCCESS;
	}
}

/**
 * @fn		  void gMvGet(UINT16 Number0, UINT8 blknum_threshold, UINT8 *Gmvx, UINT8 *Gmvy)
 * @brief	  Get X&Y Max Mv value
 * @param	  [in] Number0 : total block number
 * @param	  [in] blknum_threshold : threshold of Max Mv block number
 * @param	  [out] Gmvx : output Max Mv x (half-pixel)
 * @param	  [out] Gmvy : output Max Mv y (half-pixel)
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
panoramaMvGet (
	UINT16 blkNum,
	UINT16 blkThr,
	UINT32 bufAddr,
	UINT32 range,
	#if 1
	UINT16 *pgmvx,
	UINT16 *pgmvy,
	#else
	UINT16 *pgmvx1,
	UINT16 *pgmvy1,
	UINT16 *pgmvx2,
	UINT16 *pgmvy2,
	#endif
	UINT8 *pnumx,
	UINT8 *pnumy
)
#if 1
{
	UINT8 xdata *poutAddr;
	UINT32 i, j, k;
	UINT8 num, max_mvx, max_mvy;
	UINT8 mv_cand[2];
	UINT8 sum_cand[2];
	UINT8 u, v, max, max_value;
	UINT8 defX, defY;
	UINT8 histmv[MD_MAX_MV_NUM];

	defX = ME_H_HALF_PIXEL_NONE;
	defY = ME_V_HALF_PIXEL_NONE;
	
	max_mvx = defX*2 + 3;
	max_mvy = defY*2 + 3;

	memset(histmv, 0, MD_MAX_MV_NUM);

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(bufAddr);
	HAL_DmaDo(0, 1, G_PanoramaInf.meTmp.hBlockNum*G_PanoramaInf.meTmp.vBlockNum<<2, 0, 0);
	poutAddr = (UINT8 xdata *) 0x3800; /* 1k SRAM */

	#if 0//for test
	for (i=0;i<EDGE_TBL_VBLOCKNUM*2;i++){
		for (j=0;j<EDGE_TBL_HBLOCKNUM*2;j++){
			printf("%bx,",*(poutAddr + i*EDGE_TBL_HBLOCKNUM*2 + j));
		}
		printf("\n");
	}

	//return;
	#endif

	max = 0;
	mv_cand[0] = 1;
	mv_cand[1] = max_mvy-2/*MAX_MV_NUM-2*/;
	sum_cand[0] = 0;
	sum_cand[1] = 0;

	for (i=0, j=0; i < blkNum; i++) {
		k = i << 2;
		#if (SWAP_HV_EN)
		u = *(poutAddr + k + 1);//Vertical MVy
		#else
		u = *(poutAddr + k);//Vertical MVy
		#endif

		//printf("%bx,",u);
		
		if (u != 0xFF) {
			#if (SWAP_HV_EN)
			v = *(poutAddr + k);//Vertical MVx
			#else
			v = *(poutAddr + k + 1);
			#endif

			//printf("%bx;",v);
			
			histmv[v+1]++;
			j++;
		}
	}

	//printf("\n");//return;

	#if 0//for test
	printf("\nVhistmv\n");
	for (i=0;i<MD_MAX_MV_NUM;i++) {
		if (histmv[i] != 0) {
			printf("[%bu],%bu\n",(UINT8)i,histmv[i]);
		}
	}
	printf("\n");
	//return;
	#endif

	for (i = 1; i < max_mvy-3/*MAX_MV_NUM-3*/; i++) {
		num = histmv[i-1] + histmv[i] + histmv[i+1];
		max_value = MAX(MAX(histmv[i-1], histmv[i]), histmv[i+1]);
		//printf("[%bu]%bu,%bu\n",(UINT8)i,num,max_value);
		if (histmv[i] == max_value) {
			if (max < num) {
				sum_cand[0] = num;
				max = num;
				if (mv_cand[0] != i) {
					mv_cand[1] = mv_cand[0];
					mv_cand[0] = i;
				}
			}
			else if ((histmv[mv_cand[1]-1] + histmv[mv_cand[1]] + histmv[mv_cand[1]+1]) < num &&
				(i - mv_cand[0]) > 1) {
				sum_cand[1] = num;
				mv_cand[1] = i;
			}
		}
	}
	//printf("Y cand:%d %d\n", mv_cand[0]-(defY+1)/*65*/, mv_cand[1]-(defY+1)/*65*/);

	v = mv_cand[0] - 1;
	if ((sum_cand[0] == sum_cand[1]) && (histmv[mv_cand[1]] > histmv[mv_cand[0]])) {
		v = mv_cand[1] - 1;
	}
	max = histmv[v] + histmv[v+1] + histmv[v+2];

	//printf("%bu,%bu,%bu,%bu\n",max,histmv[v],histmv[v+1],histmv[v+2]);
	if (max < blkThr) {
		v = defY;
	}

	*pgmvy = v;
	*pnumy = max;
	//printf("Y: max:%d, j:%d, v:%d\n", max, j, v);

	memset(histmv, 0, MD_MAX_MV_NUM);
	
	max = 0;
	mv_cand[0] = 1;
	mv_cand[1] = max_mvx-2/*MAX_MV_NUM-2*/;
	sum_cand[0] = 0;
	sum_cand[1] = 0;

	for (i = 0, j = 0; i < blkNum; i++) {
		k = (i << 2) + 2;
		#if (SWAP_HV_EN)
		u = *(poutAddr + k + 1);//Horizontal MVy
		#else
		u = *(poutAddr + k);//Horizontal MVy		
		#endif

		//printf("%bx,",u);
		
		if (u != 0xFF) {
			#if (SWAP_HV_EN)
			v = *(poutAddr + k);//Horizontal MVx
			#else
			v = *(poutAddr + k + 1);//Horizontal MVx
			#endif

			//printf("%bx:",v);
			//printf("%bx:",u);
			
			histmv[u+1]++;
			j++;
		}
	}

	//printf("\n");

	#if 0//for test
	printf("\nHhistmv\n");
	for (i=0;i<MD_MAX_MV_NUM;i++) {
		if (histmv[i] != 0) {
			printf("[%bu],%bu\n",(UINT8)i,histmv[i]);
		}
	}
	printf("\n");
	//return;
	#endif

	for (i = 1; i < max_mvx-3/*MAX_MV_NUM-3*/; i++) {
		num = histmv[i-1] + histmv[i] + histmv[i+1];
		max_value = MAX(MAX(histmv[i-1], histmv[i]), histmv[i+1]);
		//printf("[%bu]%bu,%bu\n",(UINT8)i,num,max_value);
		if (histmv[i] == max_value) {
			//printf("max %bu,%bu\n",max,mv_cand[1]-1);
			if (max < num) {
				sum_cand[0] = num;
				max = num;
				if (mv_cand[0] != i) {
					mv_cand[1] = mv_cand[0];
					mv_cand[0] = i;
				}
			}
			else if ((histmv[mv_cand[1]-1] + histmv[mv_cand[1]] + histmv[mv_cand[1]+1]) < num &&
				(i - mv_cand[0]) > 1) {
				sum_cand[1] = num;
				mv_cand[1]  = i;
			}
		}
	}
	//printf("X cand:%d %d\n", mv_cand[0]-(defX+1)/*65*/, mv_cand[1]-(defX+1)/*65*/);

	u = mv_cand[0] - 1;
	if ((sum_cand[0] == sum_cand[1]) && (histmv[mv_cand[1]] > histmv[mv_cand[0]])) {
		u = mv_cand[1] - 1;
	}
	max = histmv[u] + histmv[u+1] + histmv[u+2];

	//printf("%bu,%bu,%bu,%bu\n",max,histmv[u],histmv[u+1],histmv[u+2]);
	if (max < blkThr) {
		u = defX;
	}

	*pgmvx = u;
	*pnumx = max;
	//printf("X: max:%d, j:%d, u:%d\n", max, j, u);
	//printf("GMV: %d, %d\n", *pgmvx - defX, *pgmvy - defY);

	/*
	MV value always clamp within 0~64.
	    But, depends on search range, its meaning are different.
	    0: (default, search range 32x32, precision: half-pixel)
		X: means -32.0, -31.5, -31.0, ..., 0, 0.5, 1.0, ..., +32.0 pixels.
		Y: means -32.0, -31.5, -31.0, ..., 0, 0.5, 1.0, ..., +32.0 pixels.

	    1: (search range 64x64, precision: full-pixel)
		X: means -64, -63, -62, ..., 0, 1, 2, ..., +64.0 pixels.
		Y: means -64, -63, -62 ..., 0, 1, 2, ..., +64.0 pixels.
	*/
	#if 0
	if( range == -1 ) {
		range = 0;
	}
	#endif

	switch (range) {
	case 1:
		*pgmvy = *pgmvy<<1;
		*pgmvx = *pgmvx<<1;
		break;
	default:
		*pgmvy = *pgmvy;
		*pgmvx = *pgmvx;
		break;
	}
}
#else
{
	UINT32 i;
	SINT16 j;
	UINT8 xdata* poutAddr;
	UINT8 defX, defY;
	UINT8 max_mvx, max_mvy;
	UINT32 mv_cand[6];
	UINT32 sum_cand[6];
	UINT32 k;
	UINT8 u,v;
	SINT16 rangeb=3,ranges=1;
	UINT16 max,max_value;
	UINT32 num=0,weighTH=15,weightdiv=5;
	UINT16 gmvx1;
	UINT16 gmvy1;
	UINT16 gmvx2;
	UINT16 gmvy2;
	UINT8 histmv[MD_MAX_MV_NUM];

	defX = ME_H_HALF_PIXEL_NONE;
	defY = ME_V_HALF_PIXEL_NONE;

	max_mvx = defX*2 + 3;
	max_mvy = defY*2 + 3;

	memset(histmv, 0, MD_MAX_MV_NUM);

	mv_cand[0] = 1;
	mv_cand[1] = max_mvy-2/*MAX_MV_NUM-2*/;
	sum_cand[0] = 0;
	sum_cand[1] = 0;

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(bufAddr);
	HAL_DmaDo(0, 1, G_PanoramaInf.meTmp.hBlockNum*G_PanoramaInf.meTmp.vBlockNum<<2, 0, 0);
	poutAddr = (UINT8 xdata *) 0x3800;	/* 1k SRAM */

	#if 0//for test
	for (i=0;i<EDGE_TBL_VBLOCKNUM*2;i++){
		for (j=0;j<EDGE_TBL_HBLOCKNUM*2;j++){
			printf("%bx,",*(poutAddr + i*EDGE_TBL_HBLOCKNUM*2 + j));
		}
		printf("\n");
	}

	return;
	#endif

	for (i=0, j=0; i < blkNum; i++) {
		k = (i << 2);
		//u = *(poutAddr + k);//Vertical MVy
		u = *(poutAddr + k + 1);//Vertical MVy
		
		//printf("%bx,",u);
		
		if (u != 0xFF) {
			//v = *(poutAddr + k + 1);//Vertical MVx
			v = *(poutAddr + k);//Vertical MVx
			
			//printf("%bx;",v);
			#if 0
     		histmv[v+rangeb]+=weight;
			#else
            histmv[v+rangeb]++;
			#endif
		}
	}

	//printf("\n");

	#if 0//for test
	printf("\nhistmv");
	for (i=0;i<MD_MAX_MV_NUM;i++) {
		if (histmv[i] != 0) {
			printf("[%bu],%bu\n",(UINT8)i,histmv[i]);
		}
	}
	printf("\n");
	//return;
	#endif

	max=0;

	for (i = 1+rangeb; i < max_mvy-1-rangeb/*MAX_MV_NUM-3*/; i++) {
		num=0;
		max_value=0;
		for (j=-rangeb;j<rangeb+1;j++) {
			num += histmv[i+j];
			if (histmv[i+j]>=max_value) {
				max_value=histmv[i+j];
			}
		}
		if (histmv[i]==max_value) {
			if (num>max) {
				max=num;
				if (ABS(i-mv_cand[0])>rangeb*2+1) {
					mv_cand[1] = mv_cand[0];
					sum_cand[1] = sum_cand[0];
				}
				mv_cand[0] = i;
				sum_cand[0] = num;
			}
			else if (sum_cand[1]<num && (i - mv_cand[0]) > rangeb*2) {
				sum_cand[1] = num;
				mv_cand[1] = i;
			}
		}
	}

	max = 0;
	mv_cand[2] = 1;
	mv_cand[3] = max_mvy-2/*MAX_MV_NUM-2*/;
	sum_cand[2] = 0;
	sum_cand[3] = 0;
    for (i=mv_cand[0]-rangeb;i<mv_cand[0]+rangeb+1;i++) {
		num=0;
		max_value=0;
		for (j=-ranges;j<ranges+1;j++) {
			num+=	histmv[i+j];
			if (histmv[i+j]>=max_value) {
				max_value=histmv[i+j];
			}
		}
		if (histmv[i]==max_value) {
			if (num>max) {
				max=num;
				if (ABS(i-mv_cand[2])>ranges*2+1) {
					mv_cand[3] = mv_cand[2];
					sum_cand[3] = sum_cand[2];
				}
				mv_cand[2] = i;
				sum_cand[2] = num;
			}
			else if (sum_cand[3]<num && (i - mv_cand[2]) > ranges) {
				sum_cand[3] = num;
				mv_cand[3] = i;

			}
		}
	}

	if (sum_cand[2]<blkThr) {
		v = defY;
	}
	else {	
		v=mv_cand[2];
		v=v-rangeb; 
	}
	
	gmvy1 = v;
	*pnumy = sum_cand[2];

	if (sum_cand[3]<blkThr || sum_cand[3]<sum_cand[2]/6) {
		v = defY;
	}
	else {	
		v=mv_cand[3];
		v=v-rangeb;
	}

	gmvy2=v;

	if (ABS(mv_cand[1]-mv_cand[0])>rangeb*2  && sum_cand[1]>weighTH) {
		
		max = 0;
		mv_cand[4] = 1;
		mv_cand[5] = max_mvy-2/*MAX_MV_NUM-2*/;
		sum_cand[4] = 0;
		sum_cand[5] = 0;
		
		for (i=mv_cand[1]-rangeb;i<mv_cand[1]+rangeb+1;i++) {
			num=0;
			max_value=0;
			for (j=-ranges;j<ranges+1;j++) {
				num+=	histmv[i+j];
				if (histmv[i+j]>=max_value) {
					max_value=histmv[i+j];
				}
			}
			if (histmv[i]==max_value) {
				if (num>max) {
					max=num;
					mv_cand[5] = mv_cand[4];
					sum_cand[5] = sum_cand[4];
					mv_cand[4] = i;
					sum_cand[4] = num;
				}
				else if (sum_cand[5]<num && (i - mv_cand[4]) > ranges) {
					sum_cand[5] = num;
					mv_cand[5] = i;
				}
			}
		}
		/*
		if (sum_cand[4]<blkThr ) {
			v = defY;
		}
		else
		*/
		{  
			v=mv_cand[4];
			v=v-rangeb;
		}
		
		gmvy2=v;
	}

	memset(histmv, 0, MD_MAX_MV_NUM);

	num=0;
	max = 0;;
	mv_cand[0] = 1;
	mv_cand[1] = max_mvx-2/*MAX_MV_NUM-2*/;
	sum_cand[0] = 0;
	sum_cand[1] = 0;

	for (i=0, j=0; i < blkNum; i++) {
		k = (i << 2)+2;
		//u = *(poutAddr + k);//Horizontal MVy
		u = *(poutAddr + k + 1);//Horizontal MVy

		//printf("%bx,",u);

		if (u != 0xFF) {
			//v = *(poutAddr + k + 1);//Horizontal MVx
			v = *(poutAddr + k);//Horizontal MVx

			//printf("%bx:",v);
			//printf("%bx:",u);
			#if 0
			histmv[u+rangeb]+=weight;
			#else
			histmv[u+rangeb]++;
			#endif
			j++;
		}
	}

	//printf("\n");

	#if 0//for test
	printf("\nhistmv");
	for (i=0;i<MD_MAX_MV_NUM;i++) {
		if (histmv[i] != 0) {
			printf("[%bu],%bu\n",(UINT8)i,histmv[i]);
		}
	}
	printf("\n");
	//return;
	#endif

	max=0;

	for (i = 1+rangeb; i < max_mvx-1-rangeb/*MAX_MV_NUM-3*/; i++) {
		num=0;
		max_value=0;
		for (j=-rangeb;j<rangeb+1;j++) {
			num += histmv[i+j];
			//printf("i+j %bu, num %ld\n",(UINT8)(i+j),num);
			if (histmv[i+j]>=max_value) {
				max_value=histmv[i+j];
			}
		}
		if (histmv[i]==max_value ) {
			if (num>max) {
				max=num;
				if (ABS(i-mv_cand[0])>rangeb*2+1) {
					mv_cand[1] = mv_cand[0];
					sum_cand[1] = sum_cand[0];
				}
				mv_cand[0] = i;
				sum_cand[0] = num;
			}
			else if (sum_cand[1]<num && (i - mv_cand[0]) > rangeb*2) {
				sum_cand[1] = num;
				mv_cand[1] = i;
			}
		}
		//printf("[%bu] mv(%ld,%ld),sumv(%ld,%ld)\n",(UINT8)i,mv_cand[0],mv_cand[1],sum_cand[0],sum_cand[1]);
	}
	printf("mv(%ld,%ld),sumv(%ld,%ld)\n",mv_cand[0],mv_cand[1],sum_cand[0],sum_cand[1]);

	max = 0;
	mv_cand[2] = 1;
	mv_cand[3] = max_mvx-2/*MAX_MV_NUM-2*/;
	sum_cand[2] = 0;
	sum_cand[3] = 0;
	
	printf("c0x%lx\n",mv_cand[0]-rangeb);

	for (i=mv_cand[0]-rangeb;i<mv_cand[0]+rangeb+1;i++) {
		printf("IN\n");
		num=0;
		max_value=0;
		for (j=-ranges;j<ranges+1;j++) {
			num+=	histmv[i+j];
			if (histmv[i+j]>=max_value) {
				max_value=histmv[i+j];
			}
		}
		if (histmv[i]==max_value) {
			if (num>max){
				max=num;
				if (ABS(i-mv_cand[0])>ranges*2+1) {
					mv_cand[3] = mv_cand[2];
					sum_cand[3] = sum_cand[2];
				}
				mv_cand[2] = i;
				sum_cand[2] = num;
			}
			else if(sum_cand[3]<num && (i - mv_cand[2]) > ranges) {
				sum_cand[3] = num;
				mv_cand[3] = i;
			}
		}
	}

	if (sum_cand[2]<blkThr) {
		v = defX;
	}
	else {	
		v=mv_cand[2];
		v=v-rangeb;
	}
	
	gmvx1 = v;
	*pnumx = sum_cand[2];

	if (sum_cand[3]<blkThr || sum_cand[3]<sum_cand[2]/6) {
		v = defX;
	}
	else {	
		v=mv_cand[3];
		v=v-rangeb;
	}
	
	gmvx2=v;

	if (ABS(mv_cand[1]-mv_cand[0])>rangeb*2 && sum_cand[1]>weighTH) {

		max = 0;
		mv_cand[4] = 1;
		mv_cand[5] = max_mvy-2/*MAX_MV_NUM-2*/;
		sum_cand[4] = 0;
		sum_cand[5] = 0;
		for(i=mv_cand[1]-rangeb;i<mv_cand[1]+rangeb+1;i++){
			num=0;
			max_value=0;
			for (j=-ranges;j<ranges+1;j++) {
				num+=	histmv[i+j];
				if (histmv[i+j]>=max_value) {
					max_value=histmv[i+j];
				}
			}
			if (histmv[i]==max_value) {
				if (num>max) {
					max=num;
					mv_cand[5] = mv_cand[4];
					sum_cand[5] = sum_cand[4];
					mv_cand[4] = i;
					sum_cand[4] = num;
				}
				else if (sum_cand[5]<num && (i - mv_cand[4]) > ranges) {
					sum_cand[5] = num;
					mv_cand[5] = i;
				}
			}
		}
		/*
		if (sum_cand[4]<blkThr ) {
			v = defX;
		}
		else
		*/
		{  
			v=mv_cand[4];
			v=v-rangeb;
		}
		gmvx2=v;

	}

	if( range == -1 ) {
		range = 0;
	}

	#if 1
	if( pgmvy1 ) {
		*pgmvy1 = gmvy1<<1;
	}
	if( pgmvx1 ) {
		*pgmvx1 = gmvx1<<1;
	}

	if( pgmvy2 ) {
		*pgmvy2 = gmvy2<<1;
	}
	if( pgmvx2 ) {
		*pgmvx2 = gmvx2<<1;
	}
	#else

	switch (range) {
	case 0:
		if( pgmvy1 ) {
			*pgmvy1 = gmvy1;
		}
		if( pgmvx1 ) {
			*pgmvx1 = gmvx1;
		}

		if( pgmvy2 ) {
			*pgmvy2 = gmvy2;
		}
		if( pgmvx2 ) {
			*pgmvx2 = gmvx2;
		}

		break;

	case 1:
		if( pgmvy1 ) {
			*pgmvy1 = gmvy1<<1;
		}
		if( pgmvx1 ) {
			*pgmvx1 = gmvx1<<1;
		}

		if( pgmvy2 ) {
			*pgmvy2 = gmvy2<<1;
		}
		if( pgmvx2 ) {
			*pgmvx2 = gmvx2<<1;
		}

		break;
	case 2:
		if( pgmvy1 ) {
			*pgmvy1 = gmvy1<<2;
		}

		if( pgmvx1 ) {
			*pgmvx1 = gmvx1<<2;
		}

		if( pgmvy2 ) {
			*pgmvy2 = gmvy2<<2;
		}

		if( pgmvx2 ) {
			*pgmvx2 = gmvx2<<2;
		}

		break;
	}
	#endif
}//gMvGet function End
#endif

/**
 * @fn                UINT8 panoramaGMVByFrameFunc(UINT16 *gmvXByFrame,	UINT16 *gmvYByFrame,	UINT8 *status)
 * @brief	            To Get MD On/off Status function.
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaGMVByFrameFunc (
	SINT16 *gmvXByFrame,
	SINT16 *gmvYByFrame,
	UINT8 *status
)
{
	
	UINT32 range = 0;
	UINT16 gmvx1;
	UINT16 gmvy1;
	UINT16 gmvx2;
	UINT16 gmvy2;
	UINT8 numx;
	UINT8 numy;
	SINT16 mvx,mvy;

	#if 0
	//UINT16 moveX = 10, moveY = 0;	
	//*gmvXByFrame = G_PanoramaInf.gmv.x;// = moveX ;
	//*gmvYByFrame = G_PanoramaInf.gmv.y;// = moveY;
	#endif
	*status = 0;//0:noraml, 1:too fast, 2:too slow

	panoramaMvGet((G_PanoramaInf.meTmp.hBlockNum*G_PanoramaInf.meTmp.vBlockNum),
					PANORAMA_FRAME_MV_THD,
					K_SDRAM_PvDistMeAddr,//G_PanoramaInf.meTmp.startBufAddr,
					range,
					&gmvx1,
					&gmvy1,
					//&gmvx2,
					//&gmvy2,
					&numx,
					&numy);

	gmvx2 = gmvx1;
	gmvy2 = gmvy1;

	#if 1
	if (gmvx1 == ME_H_HALF_PIXEL_NONE) {
		mvx = 0;
	}
	else {
		mvx = ((SINT16)gmvx1-(SINT16)ME_H_HALF_PIXEL_NONE);
	}

	if (gmvy1 == ME_V_HALF_PIXEL_NONE) {
		mvy = 0;
	}
	else {
		mvy = ((SINT16)gmvy1-(SINT16)ME_V_HALF_PIXEL_NONE);
	}

	//printf("ME X(%d,%bu), Y(%d,%bu)\n",mvx,numx,mvy,numy);
	#endif

	*gmvXByFrame = G_PanoramaInf.gmv.x = mvx;
	*gmvYByFrame = G_PanoramaInf.gmv.y = mvy;

	if ( G_PanoramaInf.gmv.x > HALF_PIXEL_FAST_THD ) {
		*status = 1;//0:noraml, 1:too fast, 2:too slow
		osMsgPost(SP1K_MSG_GMV_TOO_FAST_OSD_DRAW);
	}

	if ( G_PanoramaInf.gmv.x < HALF_PIXEL_SLOW_THD ) {
		*status = 2;//0:noraml, 1:too fast, 2:too slow
		osMsgPost(SP1K_MSG_GMV_TOO_SLOW_OSD_DRAW);
	}

	return SUCCESS;
}

/**
 * @fn                UINT8 panoramaSumOfGMVFunc(UINT16 gmvXByFrame, UINT16 gmvYByFrame);
 * @brief	            To Get MD On/off Status function.
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaSumOfGMVFunc (
	SINT16 *sumOfGmvX,
	SINT16 *sumOfGmvY
)
{
	UINT8 checkX;
	SINT32 swapSize;

	G_PanoramaInf.sogmv.x += G_PanoramaInf.gmv.x;
	G_PanoramaInf.sogmv.y += G_PanoramaInf.gmv.y;

	//printf("somvx (%bu,%d)\n",G_MEIntrBufIdx,G_PanoramaInf.sogmv.x);

	//swapSize = (G_MEIntrBufIdx*G_PanoramaInf.frameTmp.hSize);//0.5 frame
	//swapSize = (((SINT32)G_MEIntrBufIdx*(G_PanoramaInf.frameTmp.hSize/3)))<<1;//0.33~ frame
	swapSize = (((SINT32)G_MEIntrBufIdx*(G_PanoramaInf.param.stitchPixel)))*G_PanoramaInf.param.scaleRatio;//0.33~ frame
	checkX = (G_PanoramaInf.sogmv.x > swapSize) ? 1 : 0;

	//printf("swapSize %ld %bu\n",swapSize,checkX);
	
	if (checkX) {
		#if (DBG_DRAMW_GME_BY_FRAME == 1)
		HAL_DramStartAddrSet(K_SDRAM_PvPanoramaTmpGmvAddr+G_MEIntrBufIdx*4,0);	
		HAL_DramWordDataWrite(READ8(G_PanoramaInf.sogmv.x,1), READ8(G_PanoramaInf.sogmv.x,0));
		HAL_DramWordDataWrite(READ8(G_PanoramaInf.sogmv.y,1), READ8(G_PanoramaInf.sogmv.y,0));
		#endif
		/* switch buffer */
		panoramaBufSwitchSet();

		//printf("===%bu===\n",G_MEIntrBufIdx);
	}

	*sumOfGmvX = G_PanoramaInf.sogmv.x;
	*sumOfGmvY = G_PanoramaInf.sogmv.y;

	osMsgPost(SP1K_MSG_SUM_OF_GMV_OSD_DRAW);
	
	return SUCCESS;
}

/**
 * @fn                UINT8 panoramaLocalMVCalculusFunc(void);
 * @brief	            To Get MD On/off Status function.
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaLocalMVCalculusFunc (
	void
)
{
	return  SUCCESS;
}

/**
 * @fn                UINT8 panoramaGprmVerticalRotateFunc(void);
 * @brief	            (FW. Provide GPRM Rotate YUV422 API)Img Size <= 4095*4095
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaGprmVerticalRotateFunc (
	void
)
{
	return  SUCCESS;
}

/**
 * @fn                UINT8 panoramaGprmScaleFunc(void);
 * @brief	            (FW. Provide GPRM Scale YUV422 API)Img Size <= 4095*4095
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaGprmScaleFunc (
	void
)
{
	return  SUCCESS;
}

/**
 * @fn                UINT8 panoramaGprmCopyPasteFunc(void);
 * @brief	            (FW. Provide GPRM Copy & Paste YUV422 API)Img Size <= 4095*4095, ROI Width : 2n, H Offset : 2n
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaGprmCopyPasteFunc (
	void
)
{
	return  SUCCESS;
}

/**
 * @fn                UINT8 panoramaGprmStitchFunc(void);
 * @brief	           
 * @param[in]      none
 * @retval          The status of MD
 *\n                              MD_DISABLE: AE turn OFF.
 *\n                              MD_ENABLE: AE turn ON.            
 */
UINT8 
panoramaGprmStitchFunc (
	void
)
{
	#if 1
	UINT8 direction = G_PanoramaInf.param.direction;
	UINT8 f_num;
	UINT32 srcAddr, tmpAddr, shotsummvAddr;
	UINT32 panoUnitWidth = G_PanoramaInf.frameTmp.hSize;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;
	UINT32 panoFinalWidth = G_PanoramaInf.final.hSize;
	UINT32 panoFinalHeight = G_PanoramaInf.final.vSize;
	UINT16 snapYuvHsize,snapYuvVsize;
	
	srcAddr = G_PanoramaInf.frameTmp.startBufAddr;
	tmpAddr = G_PanoramaInf.dst.bufAddr;
	f_num = G_PanoramaInf.frameTmp.totalNum;
	shotsummvAddr = K_SDRAM_PvPanoramaTmpGmvAddr;
	
	if (direction == 1) {
		Panorama_LR(srcAddr,
					tmpAddr,
					f_num,
					panoUnitWidth,
					panoUnitHeight,
					panoDstWidthGet(direction),
					panoDstHeightGet(direction),
					panoRangeGet(direction),
					EXTEND,
					X_OFFSET,
					direction,
					panoFinalWidthGet(direction),
					shotsummvAddr);
	}

	if (panoFinalWidth == 0 || panoFinalHeight == 0) {
		snapYuvHsize = (UINT16)panoFinalWidthGet(direction);
		snapYuvVsize = (UINT16)panoFinalHeightGet(direction);
		sp1kSnapCropSizeSet(snapYuvHsize, snapYuvVsize);
	}
	else {
		sp1kSnapCropSizeSet((UINT16)panoFinalWidth,(UINT16)panoFinalHeight);
	}
	
	return  SUCCESS;
	#else
	UINT8 i;
	HAL_GprmCopy(G_PanoramaInf.frameTmp.startBufAddr, 
				G_PanoramaInf.frameTmp.hSize, 
				G_PanoramaInf.frameTmp.vSize, 
				0, 
				0,
				G_PanoramaInf.dst.bufAddr, 
				G_PanoramaInf.dst.hSize, 
				G_PanoramaInf.dst.vSize, 
				0, 
				0,
				G_PanoramaInf.frameTmp.hSize, 
				G_PanoramaInf.frameTmp.vSize,  
				0, 
				1);

	for (i = 1; i < G_PanoramaInf.frameTmp.totalNum; i++) {
		HAL_GprmCopy(G_PanoramaInf.frameTmp.startBufAddr+G_PanoramaInf.frameTmp.cdsp1stOneWSize*i, 
					G_PanoramaInf.frameTmp.hSize, 
					G_PanoramaInf.frameTmp.vSize, 
					0, 
					0,
					G_PanoramaInf.dst.bufAddr, 
					G_PanoramaInf.dst.hSize, 
					G_PanoramaInf.dst.vSize, 
					/*G_PanoramaInf.frameTmp.hSize,//*/((G_PanoramaInf.frameTmp.hSize*i/3)>>1)<<1, 
					0,
					G_PanoramaInf.frameTmp.hSize, 
					G_PanoramaInf.frameTmp.vSize,  
					0, 
					1);
	}
	
	//HAL_GprmDramDmaExt(G_PanoramaInf.frameTmp.startBufAddr<<1,
	//						G_PanoramaInf.dst.bufAddr<<1,
	//						(UINT32)G_PanoramaInf.dst.hSize*G_PanoramaInf.dst.vSize*2,
	//							1, 
	//							0);
	return  SUCCESS;
	#endif
}


UINT32
panoDstWidthGet(
	UINT8 direction
)
{
	UINT32 ret = 0;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;
	
	if (direction == 1) {
		if (panoUnitHeight==240) {
			ret=STD_REF_WIDTH * panoFinalWidthGet(direction) / STD_FINAL_WIDTH_480;/*width should be 8x*/
		}
		else if (panoUnitHeight==480) {
			ret=STD_REF_WIDTH * panoFinalWidthGet(direction) / STD_FINAL_WIDTH_480;/*width should be 8x*/
		}
	}
	
	#if 0
	printf("ret %ld,%ld\n",ret,panoUnitHeight);
	#endif

	return ROUND_UP_TO_DIVISIBLE(ret, 8);
}

UINT32
panoDstHeightGet(
	UINT8 direction
)
{
	UINT32 ret = 0;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;
	
	if (direction == 1) {
		if (panoUnitHeight==240) {
			ret=320;
		}
		else if (panoUnitHeight==480) {
			ret=640;
		}
	}

	return ret;
}

void
panoFinalSizeSet(
	UINT32 w,
	UINT32 h
)
{
	G_PanoramaInf.final.hSize = (UINT16)ROUND_DOWN_TO_DIVISIBLE(w, 16);
	G_PanoramaInf.final.vSize = (UINT16)ROUND_DOWN_TO_DIVISIBLE(h, 16);
}

UINT32
panoFinalWidthGet(
	UINT8 direction
)
{
	UINT32 ret = 0;
	UINT32 panoFinalWidth = G_PanoramaInf.final.hSize;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;

	if (direction == 1)
	{
		if( panoFinalWidth == 0 ) {
			if (panoUnitHeight==240) {
				ret=STD_FINAL_WIDTH_240;/*width should be 16x*/
			}
			else if (panoUnitHeight==480) {
				ret=STD_FINAL_WIDTH_480;/*width should be 16x*/
			}
		}
		else {
			ret = panoFinalWidth;
		}
	}

	#if 0
	printf("UnitH %ld,%ld \n",panoUnitHeight,ret) ;
	#endif

	return ret;
}

UINT32
panoFinalHeightGet(
	UINT8 direction
)
{
	UINT32 ret = 0;
	UINT32 panoFinalWidth = G_PanoramaInf.final.hSize;
	UINT32 panoFinalHeight = G_PanoramaInf.final.vSize;
	UINT32 panoUnitWidth = G_PanoramaInf.frameTmp.hSize;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;
	
	if (direction == 1)
	{
		if( panoFinalWidth == 0 ) {
			if (panoUnitHeight==240) {
				ret=STD_FINAL_HEIGHT_240;/*width should be 16x*/
			}
			else if (panoUnitHeight==480) {
				ret=STD_FINAL_HEIGHT_480;/*width should be 16x*/
			}
		}
		else {
			ret = panoFinalHeight;
		}
	}

	#if 0
	printf("panoFinalWidth=%ld,panoUnitHeight=%ld,panoUnitWidth=%ld,ret=%ld \n",panoFinalWidth,panoUnitHeight,panoUnitWidth,ret) ;
	#endif

	return ret;
}

/*計算所需的range*/
UINT32
panoRangeGet(
	UINT8 direction
)
{
	UINT32 ret = 0;
	UINT32 panoUnitWidth = G_PanoramaInf.frameTmp.hSize;
	UINT32 panoUnitHeight = G_PanoramaInf.frameTmp.vSize;
	
	if (direction == 1) {
		if (panoUnitHeight==240) {
			ret=64;
		}
		else if (panoUnitHeight==480) {
			ret=128;
		}
	}

	return ret;
}


extern void dbgMemDump(UINT32 total,UINT32 addr,UINT8 mode);


void 
Panorama_LR(
	UINT32 srcaddr,//word addr (G_PanoramaInf.frameTmp.startBufAddr)
	UINT32 dstAddr,//word addr (G_PanoramaInf.dst.bufAddr)
	UINT32 sh_idx,// 3 (G_PanoramaInf.frameTmp.totalNum)
	UINT32 width,//640 (G_PanoramaInf.frameTmp.hSize)
	UINT32 height,//480 (G_PanoramaInf.frameTmp.vSize)
	UINT32 outW,//1280 (panoDstWidthGet)
	UINT32 outH,//640 (panoDstHeightGet)
	UINT32 xrange,//128 (panoRangeGet)
	UINT32 extend,//0 (EXTEND)
	UINT32 xoff,//0 (X_OFFSET)
	UINT32 direction,// 1
	UINT32 widthall,//1280 (panoFinalWidthGet)
	UINT32 shotsummvAddr 
)
{
#if 1
	UINT32 i,j,n,sit,shiftx=0,shifty=0;
	UINT32 xinit,abext,xcut,sumy,xprewidth,stitchidx;
	UINT32 sumx,xs1,xe1,xs2=0,xe2=0,ys1=0,ye1=0,ys2=0,ye2=0,Ygain=100;
	UINT16 *LedgeAddr,*LsadAddr;
	UINT8 *localbuf,*LmvAddr;
	UINT32 src,dst,cy1,cy2;
	UINT32 Ssize,Dsize,srcW,srcH,localsize;
	UINT32 bxe1,bxs1,blendingwidth,wb1,wb2;
	UINT32 maxys2,minye2,roiW,roiH,ScalF,overlap,meanmm;
	UINT32 time1,DstWidth;
	UINT16  partx,party,pgmvx,pgmvy,pgmvx2,pgmvy2;
	UINT32 mvx,mvy;   
	UINT8 pnumx,pnumy; 
	SINT16 shotsummvx;
	SINT16 shotsummvy;
	UINT32 v1,v2,v3,v4,v5,v6;
	UINT16 srcXOffSet,srcYOffSet;
	UINT16 dstXOffSet,dstYOffSet;
	UINT16 cpXSize,cpYSize;

	//printf("outW %ld,%ld\n",outW,outH);
	
	xinit=xrange*2+xoff;//256
	abext=0;
	xcut=0;
	overlap=0;//pixel
	partx=12;
	party=12;
	sumx=0;
	blendingwidth=PANORAMA_BLENDING_WIDTH;
	sumy=extend/2;//0
	src=srcaddr;
	dst=dstAddr;
	Dsize=outW*outH;//1280*640
	#if 1
	srcW=width+xoff;//640 (640+0)  
	srcH=height+extend;//480 + 0
	Ssize=srcW*srcH;//640*480   
	#else
	srcW=xrange*2*2+xoff;//512 
(128*2*2+0)
	srcH=height+extend;//480 + 0
	Ssize=srcW*srcH;//512*480
	#endif
	
	maxys2=sumy;//0
	minye2=maxys2+height+abext;//480 (0+480+0)
	
#if (0)
	localsize=xrange*2*height;
	localbuf=localBuffer;
#endif
	
#if (0)	
	printf("dstAddr=%x  ,outW=%d,outH=%d \n",dstAddr,outW,outH);
#endif

#if (0)	
	xprewidth=xrange;
#endif

#if (0)	
	stillMvConfig(xrange*2/Scaleratio, height/Scaleratio, partx, party, 32,0);
#endif
	
#if (0)		
	printf("xrange=%d,sh_idx=%d \n" ,xrange,sh_idx);
#endif

	#if (DBG_SHOW_GME_BY_FRAME == 1)
	dbgMemDump((sh_idx+1)*16,K_SDRAM_PvPanoramaTmpGmvAddr,1);
	#endif
	
	for (n=0;n<sh_idx-1;n++) {
		#if (0)
		printf("====LR:%ld\n", n);
		#endif

		cy1=srcaddr+n*Ssize;
		cy2=srcaddr+(n+1)*Ssize;
		//time1=tmrMsTimeGet();
#if (0)	
		Localalign(shotsummvx,shotsummvy,
					n,
					sh_idx,
					xinit,
					xrange,
					xrange*2*2+xoff,
					height+extend,
					&stitchidx,
					&Ygain,
					xprewidth,
					extend,
					xoff,
					pcy1,
					localbuf);
#endif		
		/*
		yuvScaleByGpe(localbuf, xrange*2/Scaleratio, height/Scaleratio, localbuf+localsize*4, xrange*2, height);
		yuvScaleByGpe(localbuf+localsize*2, xrange*2/Scaleratio, height/Scaleratio, localbuf+localsize*6, xrange*2, height);
		*/
#if (0)		
		printf("localalign \n");
#endif
#if (0)
		printf("refaddr=%x,curaddr=%x \n",localbuf+localsize*4,localbuf+localsize*6);
#endif
#if (0)
		printf(a)("Localalign %d ms \n", tmrMsTimeGet() - time1);
#endif
#if (0)	
		LmvAddr=stillMvDo(localbuf+localsize*4,localbuf+localsize*6,xrange*2/Scaleratio, height/Scaleratio);
#endif       
#if (0)
		printf("Still mv %d ms \n", tmrMsTimeGet() - time1);
#endif

		/*
		sadcount(localbuf,partx,party,width,height,xrange,edgex,edgey,sadtbl,mvxtbl,mvytbl);
		*/
		/*
		mvhistsad( LmvAddr,partx, party,histmvx,histmvy);
		Gmvcalc_v2(histmvx,histmvy,&mvx,&mvy);
		*/
#if (0)	
		halMeGMVGet(partx*party, 15, LmvAddr, 0, &pgmvx, &pgmvy ,&pgmvx2, &pgmvy2, &pnumx, &pnumy);
		printf("mvx=%d,mvy=%d \n",pgmvx,pgmvy);
		if (pgmvx==0xFF)
			mvx=0;
		else 	 	
			mvx=(int)pgmvx-DEFAULT_CLAMP_X;
		
		if (pgmvy==0xFF)
			mvy=0;	
		else
			mvy=(int)pgmvy-DEFAULT_CLAMP_Y;
		
		mvx*=Scaleratio;
		mvy*=Scaleratio;
#endif
		stitchidx=xinit;

#if (0)
		printf("xinit=%d,stitchidx=%d,mvx=%d,mvy=%d \n",xinit,stitchidx,mvx,mvy);
#endif
#if (0)
		printf("sadcount & gmv_cal %d ms \n", tmrMsTimeGet() - time1);
#endif
#if (0)	
		for (i=n;i<n+1;i++) {
			shotsummvx[i]+=mvx;
		}
		
		shotsummvy[n]+=mvy;
#endif

		if (n==0) {
			xs1=0;//0
			xe1=xinit+sumx;//256 (256+0)
			ys1=sumy-abext;//0 (0-0)
			ye1=ys1+height;//480 (0+480)
			sit=1;
		}
		else {
			xe1=xinit+sumx;//256+80
			sit=0;
		}

		if (sit) {
			if (ys1>maxys2) maxys2=ys1;//maxys2 = 0
			if (ye1<minye2) minye2=ye1;//minye2 = 480
#if (0)
			printf("1x %d,%d y %d,%d sx %d,%d outW=%d\n",(UINT16)xs1,(UINT16)xe1,(UINT16)ys1,(UINT16)ye1,(UINT16)sumx,(UINT16)sumy,(UINT16)outW);
#endif
#if (1)
			#if (0)
			printf("1.x %d,%d y %d,%d\n",
										(UINT16)xs1,(UINT16)xe1,
										(UINT16)ys1,(UINT16)ye1);
			#endif
			#if (0)
			printf("1.GPRM Src %ld,%ld\nDst %ld,%ld\nSize %ld,%ld\n",
							(xs1-sumx),
							(ys1-sumy+(extend/2)),
							xs1,
							ys1,
							(xe1+overlap-xs1),
							(ye1-ys1));
			#endif
			HAL_GprmCopy( cy1, 
						(UINT16) srcW, 
						(UINT16) srcH, 
						(UINT16) (xs1-sumx), 
						(UINT16) (ys1-sumy+(extend/2)),
						dst, 
						(UINT16) outW, 
						(UINT16) outH, 
						(UINT16) xs1, 
						(UINT16) ys1,
						(UINT16) (xe1+overlap-xs1), 
						(UINT16) (ye1-ys1),  
						0, 
						1);
#else
			for (j=ys1;j<ye1;j++) {
				memcpy(pdst+j*outW+xs1,pcy1+(j-sumy+extend/2)*srcW+(xs1-sumx),(xe1+overlap-xs1));
				memcpy(pdst+j*outW/2+xs1/2+Dsize,pcy1+(j-sumy+extend/2)*srcW/2+(xs1-sumx)/2+Ssize,(xe1+overlap-xs1+1)/2);
				memcpy(pdst+j*outW/2+xs1/2+Dsize*3/2,pcy1+(j-sumy+extend/2)*srcW/2+(xs1-sumx)/2+Ssize*3/2,(xe1+overlap-xs1+1)/2);
			}
#endif
		}

		HAL_DramStartAddrSet(shotsummvAddr+((n+1)*4),1);	
		HAL_DramWordDataRead(&(((UINT8*)&shotsummvx)+1),&(((UINT8*)&shotsummvx)+0)); 
		HAL_DramWordDataRead(&(((UINT8*)&shotsummvy)+1),&(((UINT8*)&shotsummvy)+0)); 
	
		#if 0
		HAL_DramStartAddrSet(K_SDRAM_PvPanoramaTmpGmvAddr,1);	
		for (shotsummvxx = 0; shotsummvxx<(n+1);shotsummvxx++) {
			HAL_DramWordDataRead(&ssxl,&ssxh); 
			HAL_DramWordDataRead(&ssyl,&ssyh); 
			printf("A %bx,%bx,%bx,%bx",ssxl,ssxh,ssyl,ssyh);
			HAL_DramWordDataRead(&ssxl,&ssxh); 
			HAL_DramWordDataRead(&ssyl,&ssyh); 
			printf(" %bx,%bx,%bx,%bx\n",ssxl,ssxh,ssyl,ssyh);
		}
		#endif
		
		shiftx=shotsummvx/2;//80
		shifty=shotsummvy/2;//0
#if (0)
		printf("GMV %x,%x %lx,%lx\n",
				shotsummvx,shotsummvy,
				shiftx,shifty);
#endif
		sumx=shiftx;//0+80
		sumy=shifty;//0+0
#if (0)
		printf("sumx %ld,%ld\n",sumx,sumy);
#endif
		xs2=GPRM_OFFSET_2N_CLAMP(xe1);//256
		xe2=GPRM_OFFSET_2N_CLAMP(xinit+sumx+xrange);//464 (256+80+128)
		ys2=GPRM_OFFSET_2N_CLAMP(sumy-abext);//0 (0-0)
		ye2=GPRM_OFFSET_2N_CLAMP(ys2+height);//480(0+480)

		#if (0)
		printf("max %ld,%ld\n",maxys2,minye2);//0,480
		#endif

		if (ys2 & 0x80000000) {
			//ys -
		}
		else {
			//ys +
			if (ys2>maxys2)
				maxys2=ys2;//0xfffff
			else
				ys2=maxys2;
		}
		
		if (ye2<minye2)
			minye2=ye2;//463
		else
			ye2=minye2;

#if (0)
		variableline(pcy1,pcy2, xrange*2*2, height, stitchidx+xrange, ys2-sumy+extend/2
			            , ye2-sumy+extend/2, shiftx,shifty, linebuf, overlap);
#endif
	   
		//printf("variableline %d ms", tmrMsTimeGet() - time1);

		/*memset(linebuf,0,sizeof(SINT32)*1080);*/
#if (0)
		printf("2.[x]%ld,%ld[y]%ld,%ld\n",
						xs2,xe2,
						ys2,ye2);
#endif

		v1 = ys2;

		if (v1 & 0x80000000) {
			v2 = 0xFFFFFFFF-v1+1;
			v3 = 0;
			v4 = v2;
			v5 = v3;
			v6 = ye2; 
		}
		else {
			v2 = 0;
			v3 = v1;
			v4 = v2;
			v5 = v3;
			v6 = ye2-v3;
		}
		//v4 = v2+v3+(extend/2);

		srcXOffSet = (UINT16)GPRM_OFFSET_2N_CLAMP(xs2-sumx);
		srcYOffSet = (UINT16)GPRM_OFFSET_2N_CLAMP(v4);
		dstXOffSet = (UINT16)GPRM_OFFSET_2N_CLAMP(xs2);
		dstYOffSet = (UINT16)GPRM_OFFSET_2N_CLAMP(v5);
		cpXSize = (UINT16)GPRM_OFFSET_2N_CLAMP(xe2+overlap-xs2);
		cpYSize = (UINT16)GPRM_OFFSET_2N_CLAMP(v6);

#if (0)
		printf("v %ld,%ld,%ld,%ld,%ld,%ld\n",
						v1,v2,v3,v4,v5,v6);
#endif	

#if (0)
		printf("2.GPRM[SrcOffSet]%d,%d[DstOffSet]%d,%d[Size]%d,%d\n",
						srcXOffSet,
						srcYOffSet,
						dstXOffSet,
						dstYOffSet,
						cpXSize,
						cpYSize);

		//printf("Size %d,%d\n",
		//				cpXSize,
		//				cpYSize);
#endif


#if (1)
		HAL_GprmCopy( cy2, 
					(UINT16) srcW, 
					(UINT16) srcH, 
					(UINT16) srcXOffSet, 
					(UINT16) srcYOffSet,
					dst, 
					(UINT16) outW, 
					(UINT16) outH, 
					(UINT16) dstXOffSet, 
					(UINT16) dstYOffSet,
					(UINT16) cpXSize, 
					(UINT16) cpYSize,  //((ye2-v2+1) & 0xFFFFFFFE),  
					0, 
					1);
#else
		HAL_GprmCopy( cy2, 
					(UINT16) srcW, 
					(UINT16) srcH, 
					(UINT16) (xs2-sumx), 
					(UINT16) (ys2-sumy+(extend/2)),
					dst, 
					(UINT16) outW, 
					(UINT16) outH, 
					(UINT16) xs2, 
					(UINT16) ys2,
					(UINT16) (xe2+overlap-xs2), 
					(UINT16) (ye2-ys2),  
					0, 
					1);
		for (j=ys2;j<ye2;j++) {
			memcpy(pdst+j*outW+xs2+linebuf[j-ys2],pcy2+(j-sumy+extend/2)*srcW+(xs2+linebuf[j-ys2]-sumx),(xe2+overlap-xs2-linebuf[j-ys2]));
			memcpy(pdst+j*outW/2+(xs2+linebuf[j-ys2])/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xs2+linebuf[j-ys2]-sumx)/2+Ssize,(xe2+overlap-xs2-linebuf[j-ys2]+1)/2);
			memcpy(pdst+j*outW/2+(xs2+linebuf[j-ys2])/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xs2+linebuf[j-ys2]-sumx)/2+Ssize*3/2,(xe2+overlap-xs2-linebuf[j-ys2]+1)/2);
		}
#endif	   
		/*
		if (n==sh_idx-1) {
			for (j=ys2;j<ye2;j++) {
				memcpy(pdst+j*outW+xs2,pcy2+(j-sumy+extend/2)*srcW+(xe2-sumx),srcW-xinit);
				memcpy(pdst+j*outW/2+xs2/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xe2-sumx)/2+Ssize,(srcW-xinit+1)/2);
				memcpy(pdst+j*outW/2+xs2/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xe2-sumx)/2+Ssize*3/2,(srcW-xinit+1)/2);
			}
		}
		*/
#if (0)
		printf("maxys2=%d,minye2=%d \n",maxys2,minye2);
#endif
/***********last frame to extend from xe2 to the end******************/
#if (0)
		if (n==sh_idx-2) {
			for (j=ys2;j<ye2;j++) {
				memcpy(pdst+j*outW+(xcut+sumx),pcy2+(j-sumy+extend/2)*srcW+xcut,(xs2-xcut-sumx));
				memcpy(pdst+j*outW/2+(xcut+sumx)/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xcut)/2+Ssize,(xs2-xcut-sumx+1)/2);
				memcpy(pdst+j*outW/2+(xcut+sumx)/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xcut)/2+Ssize*3/2,(xs2-xcut-sumx+1)/2);
			}
		}
#endif

#if (0)
		printf	(a)("stitch %d ms\n", tmrMsTimeGet() - time1);
#endif
		/*-------------blending-------------------------*/
#if (1)
		if (1) {
			UINT8 y[PANORAMA_BLENDING_WIDTH+4];
			UINT8 uv[PANORAMA_BLENDING_WIDTH+4];
			UINT8 yCnt;
			UINT8 wy1,wy2;
			UINT32 YOffset;
			printf("blending\n");

			dbgWdtFeed(10000);
			
			for(j=maxys2;j<minye2;j++){
				bxs1 = dstXOffSet-blendingwidth/2;
				bxe1 = dstXOffSet+blendingwidth/2;

				yCnt = 0;
				for (i=bxs1-2;i<bxe1+2;i+=2){
					YOffset = ((i>>3)<<3)+((i&0x07)>>1);
					HAL_DramStartAddrSet(dst+j*outW+YOffset,1);
					HAL_DramWordDataRead(&y[yCnt],&y[yCnt+1]); 

					yCnt+=2;
				}
				//printf("yCnt %bu\n",yCnt);

				yCnt = 0;
				for (i=bxs1;i<bxe1;i+=2){
					YOffset = ((i>>3)<<3)+((i&0x07)>>1);

					wy1 = (UINT8)(((UINT16)y[i-bxs1+0]+y[i-bxs1+1]+y[i-bxs1+2]+y[i-bxs1+3]+y[i-bxs1+4])/5);
					wy2 = (UINT8)(((UINT16)y[i-bxs1+1]+y[i-bxs1+2]+y[i-bxs1+3]+y[i-bxs1+4]+y[i-bxs1+5])/5);
					
					HAL_DramStartAddrSet(dst+j*outW+YOffset,0);
					HAL_DramWordDataWrite(wy1,wy2); 

					yCnt+=2;
				}
				//printf("yCnt %bu\n",yCnt);
			}
		}
#else

		for(j=maxys2;j<minye2;j++){
			bxs1= xs2+linebuf[j-ys2]-blendingwidth/2;
			bxe1= xs2+linebuf[j-ys2]+blendingwidth/2;
			for (i=bxs1;i<bxe1;i++){
				wb2=bxe1-i;
				wb1=blendingwidth-wb2;
				printf_4("pdst[%d]=%d,",j*outW+i,pdst[j*outW+i]);
				pdst[j*outW+i]=(UINT8)(((int)pcy2[(j-sumy+extend/2)*srcW+i-sumx]*wb1+(int)pcy1[(j-sumy+shifty+extend/2)*srcW+i-sumx+shiftx]*wb2)/(wb1+wb2));
				printf_4("pdst[%d]=%d,\n",j*outW+i,pdst[j*outW+i]);
			}
		}
#if (0)
		printf("blending %d ms \n", tmrMsTimeGet() - time1);
#endif
#endif
		DstWidth=panoDstWidthGet(direction);

		if (xe2>DstWidth-xrange) {
			//printf("Breakx=%ld,DstWidth=%ld \n",xe2,DstWidth);  	
			break;
		}	   
	   
	}/*end of n*/

#if (0)
	stillMvEnd();
#endif

#if (0)
	//time1=tmrMsTimeGet();
	
	if ((minye2-maxys2)>height) {
		minye2=maxys2+height;
	}
	else if((minye2-maxys2)%2==1) {
		minye2-=1;
	}
#endif

#if (0)
    printf("minye2=%d, maxys2=%d,ScalF=%d \n ",minye2,maxys2,ScalF );
#endif

#if (0)
	meanmm=(minye2+maxys2)/2;
#endif

#if (0)
	printf("height=%d \n",height);
#endif

	/*
	if (height==480 || height==720){
	  maxys2=meanmm-(height/2/4*3);
	  minye2=meanmm+(height/2/4*3);
	}
	*/

#if (0)	
	ScalF = ((minye2-maxys2)<<7)/height;

	roiW=(widthall*ScalF)>>7;

    maxys2=(maxys2+15)/16*16;
#endif
	
#if (0)	
    printf("xe2=%d,maxys2=%d,minye2=%d \n",xe2,maxys2,minye2);
#endif

#if 0	
    /***************************MOE *********************************/
	/***width and height should be 16x */

	roiW=(roiW/16)*16;
	roiH=(minye2-maxys2)/16*16;
       moeQue_t *pqueue;
	UINT8 *pack,*unpack,*packUV,*unpackUV;
	UINT32 roisize;
	SNOOP_AUTO_ARRAY_DECLARE(moeQue_t, 2, cmdque);
      memset(cmdque, 0, 2*64);
	printf_4("cmdque=%x \n",cmdque);
       unpack=pdst;
	pack=srcaddr;
	printf_4("unpack=%x,pack=%x \n",unpack,pack);
	roisize=(roiW)|(roiH<<16);
	printf("roiW=%d,roiH=%d \n",roiW,roiH);
	printf("outW=%d,outH=%d \n",outW,outH);

       /* Y(linear)  unpack to Y(block): {unpack} -> {pack} */
	 pqueue = &cmdque[0];
	 pqueue->dw0 = 0x00000004 | (0<<16);
	 pqueue->dw2 = roisize; /* roi */
	 pqueue->dw3 = LOGI_CACH_BADDR_TO_PHY_WADDR(unpack) | (1<<29); /*linear*/
	 pqueue->dw4 = (outW)|(outH<<16); /* srcsize */
	 pqueue->dw5 = (0x0000)|(maxys2<<16);  /* srcoffset */
	 /* pqueue->dw7 = 0; */ /* scalefac */
	 /* pqueue->dw8 = 0; */ /* scaleini */
	  pqueue->dw9 = LOGI_CACH_BADDR_TO_PHY_WADDR(pack) | (0<<29); /*block*/
	  pqueue->dw10 = roisize; /* dstsize */
	  /* pqueue->dw11 = 0; */ /* dstoffset */
    
        packUV=pack+16*16;
	 unpackUV=unpack+outW*outH;
	 /* UV(block)  unpack to UV(linear): {unpack} -> {pack} */
	 pqueue = &cmdque[1];
	 pqueue->dw0 = 0x00000005 | (0<<16);
	 pqueue->dw2 = roisize; /* roi */
	 pqueue->dw3 = LOGI_CACH_BADDR_TO_PHY_WADDR(unpackUV) | (3<<29); /*linear*/
	 pqueue->dw4 = (outW)|(outH<<16); /* srcsize */
	 pqueue->dw5 = (0x0000)|(maxys2<<16);  /* srcoffset */
	 /* pqueue->dw7 = 0; */ /* scalefac */
	 /* pqueue->dw8 = 0; */ /* scaleini */
	 pqueue->dw9 = LOGI_CACH_BADDR_TO_PHY_WADDR(packUV) | (2<<29); /*block*/
	 pqueue->dw10 = roisize; /* dstsize */
	 /* pqueue->dw11 = 0; */ /* dstoffset */


       cpuDCacheWB(cmdque, 2*64);
	halMoeLock();
       halMoeStart(LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(cmdque));
       halMoeWait();
	halMoeUnlock();
	cpuDCacheWB(srcaddr, (roiW)*(roiH)*2);
       cpuDCacheWB(pdst, outW*outH*2);

	/*fsSimpleWrite( "D:\\RESULT1.YUV", srcaddr, (roiW)*(roiH)*2 );
       fsSimpleWrite( "D:\\RESULT2.YUV", pdst, outW*outH*2 );*/
	

	printf_4("widthall=%d, height=%d \n",widthall,height);
       halJpegReset(); 
	yuvScaleByJpeg(pdst,widthall,height,srcaddr,roiW,roiH,0); /*dest W x16, dest H x16*/
	/*cdspDebugWriteFile( "D:\\RESULT.YUV", pdst, widthall*height*2 );*/
#endif
#if (0)
	printf("pack & scale %d ms \n", tmrMsTimeGet() - time1);
#endif
	
	#if 0
	printf("dst %lx (%ld,%ld) %ld\n",dst,outW,outH,maxys2);
	#endif

	#if 1
	HAL_GprmCopy( dst, 
					(UINT16) outW, 
					(UINT16) outH, 
					(UINT16) 0, 
					(UINT16) maxys2,
					dst, 
					(UINT16) panoFinalWidthGet(direction), 
					(UINT16) panoFinalHeightGet(direction), 
					(UINT16) 0, 
					(UINT16) 0,
					(UINT16) panoFinalWidthGet(direction), 
					(UINT16) panoFinalHeightGet(direction),
					0, 
					1);
	#endif
#endif
}

void 
Panorama_LR_Ori(
	UINT32 srcaddr,//word addr (G_PanoramaInf.frameTmp.startBufAddr)
	UINT32 dstAddr,//word addr (G_PanoramaInf.dst.bufAddr)
	UINT32 sh_idx,// 3 (G_PanoramaInf.frameTmp.totalNum)
	UINT32 width,//640 (G_PanoramaInf.frameTmp.hSize)
	UINT32 height,//480 (G_PanoramaInf.frameTmp.vSize)
	UINT32 outW,//1280 (panoDstWidthGet)
	UINT32 outH,//640 (panoDstHeightGet)
	UINT32 xrange,//128 (panoRangeGet)
	UINT32 extend,//0 (EXTEND)
	UINT32 xoff,//0 (X_OFFSET)
	UINT32 direction,// 1
	UINT32 widthall,//1280 (panoFinalWidthGet)
	UINT32 shotsummvAddr 
)
{
#if 0
	UINT32 i,j,n,sit,shiftx=0,shifty=0;
	UINT32 xinit,abext,xcut,sumy,xprewidth,stitchidx;
	UINT32 sumx,xs1,xe1,xs2=0,xe2=0,ys1=0,ye1=0,ys2=0,ye2=0,Ygain=100;
	UINT16 *LedgeAddr,*LsadAddr;
	UINT8 *localbuf,*LmvAddr;
	UINT32 src,dst,cy1,cy2;
	UINT32 Ssize,Dsize,srcW,srcH,localsize;
	UINT32 bxe1,bxs1,blendingwidth,wb1,wb2;
	UINT32 maxys2,minye2,roiW,roiH,ScalF,overlap,meanmm;
	UINT32 time1,DstWidth;
	UINT16  partx,party,pgmvx,pgmvy,pgmvx2,pgmvy2;
	UINT32 mvx,mvy;   
	UINT8 pnumx,pnumy; 
	SINT16 shotsummvx;
	SINT16 shotsummvy;

	//printf("outW %ld,%ld\n",outW,outH);
	
	xinit=xrange*2+xoff;//256
	abext=0;
	xcut=0;
	overlap=0;//pixel
	partx=12;
	party=12;
	sumx=0;
	blendingwidth=16;
	sumy=extend/2;//0
	src=srcaddr;
	dst=dstAddr;
	Dsize=outW*outH;//1280*640
	#if 1
	srcW=width+xoff;//640 (640+0)  
	srcH=height+extend;//480 + 0
	Ssize=srcW*srcH;//640*480   
	#else
	srcW=xrange*2*2+xoff;//512 
(128*2*2+0)
	srcH=height+extend;//480 + 0
	Ssize=srcW*srcH;//512*480
	#endif
	
	maxys2=sumy;//0
	minye2=maxys2+height+abext;//480 (0+480+0)
	
#if (0)
	localsize=xrange*2*height;
	localbuf=localBuffer;
#endif
	
#if (0)	
	printf("dstAddr=%x  ,outW=%d,outH=%d \n",dstAddr,outW,outH);
#endif

#if (0)	
	xprewidth=xrange;
#endif

#if (0)	
	stillMvConfig(xrange*2/Scaleratio, height/Scaleratio, partx, party, 32,0);
#endif
	
#if (0)		
	printf("xrange=%d,sh_idx=%d \n" ,xrange,sh_idx);
#endif

	#if (DBG_SHOW_GME_BY_FRAME == 1)
	dbgMemDump((sh_idx+1)*16,K_SDRAM_PvPanoramaTmpGmvAddr,1);
	#endif
	
	for (n=0;n<sh_idx-1;n++) {
		#if (1)
		printf("====LR:%d\n", n);
		#endif

		cy1=srcaddr+n*Ssize;
		cy2=srcaddr+(n+1)*Ssize;
		//time1=tmrMsTimeGet();
#if (0)	
		Localalign(shotsummvx,shotsummvy,
					n,
					sh_idx,
					xinit,
					xrange,
					xrange*2*2+xoff,
					height+extend,
					&stitchidx,
					&Ygain,
					xprewidth,
					extend,
					xoff,
					pcy1,
					localbuf);
#endif		
		/*
		yuvScaleByGpe(localbuf, xrange*2/Scaleratio, height/Scaleratio, localbuf+localsize*4, xrange*2, height);
		yuvScaleByGpe(localbuf+localsize*2, xrange*2/Scaleratio, height/Scaleratio, localbuf+localsize*6, xrange*2, height);
		*/
#if (0)		
		printf("localalign \n");
#endif
#if (0)
		printf("refaddr=%x,curaddr=%x \n",localbuf+localsize*4,localbuf+localsize*6);
#endif
#if (0)
		printf(a)("Localalign %d ms \n", tmrMsTimeGet() - time1);
#endif
#if (0)	
		LmvAddr=stillMvDo(localbuf+localsize*4,localbuf+localsize*6,xrange*2/Scaleratio, height/Scaleratio);
#endif       
#if (0)
		printf("Still mv %d ms \n", tmrMsTimeGet() - time1);
#endif

		/*
		sadcount(localbuf,partx,party,width,height,xrange,edgex,edgey,sadtbl,mvxtbl,mvytbl);
		*/
		/*
		mvhistsad( LmvAddr,partx, party,histmvx,histmvy);
		Gmvcalc_v2(histmvx,histmvy,&mvx,&mvy);
		*/
#if (0)	
		halMeGMVGet(partx*party, 15, LmvAddr, 0, &pgmvx, &pgmvy ,&pgmvx2, &pgmvy2, &pnumx, &pnumy);
		printf("mvx=%d,mvy=%d \n",pgmvx,pgmvy);
		if (pgmvx==0xFF)
			mvx=0;
		else 	 	
			mvx=(int)pgmvx-DEFAULT_CLAMP_X;
		
		if (pgmvy==0xFF)
			mvy=0;	
		else
			mvy=(int)pgmvy-DEFAULT_CLAMP_Y;
		
		mvx*=Scaleratio;
		mvy*=Scaleratio;
#endif
		stitchidx=xinit;

#if (0)
		printf("xinit=%d,stitchidx=%d,mvx=%d,mvy=%d \n",xinit,stitchidx,mvx,mvy);
#endif
#if (0)
		printf("sadcount & gmv_cal %d ms \n", tmrMsTimeGet() - time1);
#endif
#if (0)	
		for (i=n;i<n+1;i++) {
			shotsummvx[i]+=mvx;
		}
		
		shotsummvy[n]+=mvy;
#endif

		if (n==0) {
			xs1=0;//0
			xe1=xinit+sumx;//256 (256+0)
			ys1=sumy-abext;//0 (0-0)
			ye1=ys1+height;//480 (0+480)
			sit=1;
		}
		else {
			xe1=xinit+sumx;
			sit=0;
		}

		if (sit) {
			if (ys1>maxys2) maxys2=ys1;//maxys2 = 0
			if (ye1<minye2) minye2=ye1;//minye2 = 480
#if (0)
			printf("1x %d,%d y %d,%d sx %d,%d outW=%d\n",(UINT16)xs1,(UINT16)xe1,(UINT16)ys1,(UINT16)ye1,(UINT16)sumx,(UINT16)sumy,(UINT16)outW);
#endif
#if (1)
			#if (1)
			printf("1.x %d,%d y %d,%d\n",
										(UINT16)xs1,(UINT16)xe1,
										(UINT16)ys1,(UINT16)ye1);
			#endif
			#if (1)
			printf("1.GPRM Src %ld,%ld\nDst %ld,%ld\nSize %ld,%ld\n",
							(xs1-sumx),
							(ys1-sumy+(extend/2)),
							xs1,
							ys1,
							(xe1+overlap-xs1),
							(ye1-ys1));
			#endif
			HAL_GprmCopy( cy1, 
						(UINT16) srcW, 
						(UINT16) srcH, 
						(UINT16) (xs1-sumx), 
						(UINT16) (ys1-sumy+(extend/2)),
						dst, 
						(UINT16) outW, 
						(UINT16) outH, 
						(UINT16) xs1, 
						(UINT16) ys1,
						(UINT16) (xe1+overlap-xs1), 
						(UINT16) (ye1-ys1),  
						0, 
						1);
#else
			for (j=ys1;j<ye1;j++) {
				memcpy(pdst+j*outW+xs1,pcy1+(j-sumy+extend/2)*srcW+(xs1-sumx),(xe1+overlap-xs1));
				memcpy(pdst+j*outW/2+xs1/2+Dsize,pcy1+(j-sumy+extend/2)*srcW/2+(xs1-sumx)/2+Ssize,(xe1+overlap-xs1+1)/2);
				memcpy(pdst+j*outW/2+xs1/2+Dsize*3/2,pcy1+(j-sumy+extend/2)*srcW/2+(xs1-sumx)/2+Ssize*3/2,(xe1+overlap-xs1+1)/2);
			}
#endif
		}

		HAL_DramStartAddrSet(shotsummvAddr+((n+1)*4),1);	
		HAL_DramWordDataRead(&(((UINT8*)&shotsummvx)+1),&(((UINT8*)&shotsummvx)+0)); 
		HAL_DramWordDataRead(&(((UINT8*)&shotsummvy)+1),&(((UINT8*)&shotsummvy)+0)); 
	
		#if 0
		HAL_DramStartAddrSet(K_SDRAM_PvPanoramaTmpGmvAddr,1);	
		for (shotsummvxx = 0; shotsummvxx<(n+1);shotsummvxx++) {
			HAL_DramWordDataRead(&ssxl,&ssxh); 
			HAL_DramWordDataRead(&ssyl,&ssyh); 
			printf("A %bx,%bx,%bx,%bx",ssxl,ssxh,ssyl,ssyh);
			HAL_DramWordDataRead(&ssxl,&ssxh); 
			HAL_DramWordDataRead(&ssyl,&ssyh); 
			printf(" %bx,%bx,%bx,%bx\n",ssxl,ssxh,ssyl,ssyh);
		}
		#endif
		
		shiftx=shotsummvx/2;
		shifty=shotsummvy/2;
#if (1)
		printf("GMV %x,%x %lx,%lx\n",
				shotsummvx,shotsummvy,
				shiftx,shifty);
#endif
		sumx=shiftx;//80
		sumy=shifty;//0
#if (1)
		printf("sumx %ld,%ld\n",sumx,sumy);
#endif
		xs2=xe1;//256
		xe2=xinit+sumx+xrange;//464 (256+80+128)
		ys2=sumy-abext;//0
		ye2=ys2+height;//480 (0+480)

		if (ys2>maxys2)
			maxys2=ys2;
		else
			ys2=maxys2;//ys2 = 0
		
		if (ye2<minye2)
			minye2=ye2;
		else
			ye2=minye2;//ye2 = 480-

#if (0)
		variableline(pcy1,pcy2, xrange*2*2, height, stitchidx+xrange, ys2-sumy+extend/2
			            , ye2-sumy+extend/2, shiftx,shifty, linebuf, overlap);
#endif
	   
		//printf("variableline %d ms", tmrMsTimeGet() - time1);

		/*memset(linebuf,0,sizeof(SINT32)*1080);*/
#if (1)
		printf("2.x %d,%d y %d,%d m %d,%d\n",
						(UINT16)xs2,(UINT16)xe2,
						(UINT16)ys2,(UINT16)ye2,
						(UINT16)maxys2,(UINT16)minye2);
#endif

#if (1)
		HAL_GprmCopy( cy2, 
					(UINT16) srcW, 
					(UINT16) srcH, 
					(UINT16) (xs2-sumx), 
					(UINT16) (ys2-sumy+(extend/2)),
					dst, 
					(UINT16) outW, 
					(UINT16) outH, 
					(UINT16) xs2, 
					(UINT16) ys2,
					(UINT16) (xe2+overlap-xs2), 
					(UINT16) (ye2-ys2),  
					0, 
					1);
#else
		for (j=ys2;j<ye2;j++) {
			memcpy(pdst+j*outW+xs2+linebuf[j-ys2],pcy2+(j-sumy+extend/2)*srcW+(xs2+linebuf[j-ys2]-sumx),(xe2+overlap-xs2-linebuf[j-ys2]));
			memcpy(pdst+j*outW/2+(xs2+linebuf[j-ys2])/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xs2+linebuf[j-ys2]-sumx)/2+Ssize,(xe2+overlap-xs2-linebuf[j-ys2]+1)/2);
			memcpy(pdst+j*outW/2+(xs2+linebuf[j-ys2])/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xs2+linebuf[j-ys2]-sumx)/2+Ssize*3/2,(xe2+overlap-xs2-linebuf[j-ys2]+1)/2);
		}
#endif	   
		/*
		if (n==sh_idx-1) {
			for (j=ys2;j<ye2;j++) {
				memcpy(pdst+j*outW+xs2,pcy2+(j-sumy+extend/2)*srcW+(xe2-sumx),srcW-xinit);
				memcpy(pdst+j*outW/2+xs2/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xe2-sumx)/2+Ssize,(srcW-xinit+1)/2);
				memcpy(pdst+j*outW/2+xs2/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xe2-sumx)/2+Ssize*3/2,(srcW-xinit+1)/2);
			}
		}
		*/
#if (0)
		printf("maxys2=%d,minye2=%d \n",maxys2,minye2);
#endif
/***********last frame to extend from xe2 to the end******************/
#if (0)
		if (n==sh_idx-2) {
			for (j=ys2;j<ye2;j++) {
				memcpy(pdst+j*outW+(xcut+sumx),pcy2+(j-sumy+extend/2)*srcW+xcut,(xs2-xcut-sumx));
				memcpy(pdst+j*outW/2+(xcut+sumx)/2+Dsize,pcy2+(j-sumy+extend/2)*srcW/2+(xcut)/2+Ssize,(xs2-xcut-sumx+1)/2);
				memcpy(pdst+j*outW/2+(xcut+sumx)/2+Dsize*3/2,pcy2+(j-sumy+extend/2)*srcW/2+(xcut)/2+Ssize*3/2,(xs2-xcut-sumx+1)/2);
			}
		}
#endif

#if (0)
		printf	(a)("stitch %d ms\n", tmrMsTimeGet() - time1);
#endif
		/*-------------blending-------------------------*/
#if (0)

		for(j=maxys2;j<minye2;j++){
			bxs1= xs2+linebuf[j-ys2]-blendingwidth/2;
			bxe1= xs2+linebuf[j-ys2]+blendingwidth/2;
			for (i=bxs1;i<bxe1;i++){
				wb2=bxe1-i;
				wb1=blendingwidth-wb2;
				printf_4("pdst[%d]=%d,",j*outW+i,pdst[j*outW+i]);
				pdst[j*outW+i]=(UINT8)(((int)pcy2[(j-sumy+extend/2)*srcW+i-sumx]*wb1+(int)pcy1[(j-sumy+shifty+extend/2)*srcW+i-sumx+shiftx]*wb2)/(wb1+wb2));
				printf_4("pdst[%d]=%d,\n",j*outW+i,pdst[j*outW+i]);
			}
		}
#if (timetest)
		printf("blending %d ms \n", tmrMsTimeGet() - time1);
#endif
#endif
		DstWidth=panoDstWidthGet(direction);

		if (xe2>DstWidth-xrange) {
			//printf("Breakx=%ld,DstWidth=%ld \n",xe2,DstWidth);  	
			break;
		}	   
	   
	}/*end of n*/

#if (0)
	stillMvEnd();
#endif

	//time1=tmrMsTimeGet();
	
	if ((minye2-maxys2)>height) {
		minye2=maxys2+height;
	}
	else if((minye2-maxys2)%2==1) {
		minye2-=1;
	}	

#if (0)
    printf("minye2=%d, maxys2=%d,ScalF=%d \n ",minye2,maxys2,ScalF );
#endif

	meanmm=(minye2+maxys2)/2;

#if (0)
	printf("height=%d \n",height);
#endif

	/*
	if (height==480 || height==720){
	  maxys2=meanmm-(height/2/4*3);
	  minye2=meanmm+(height/2/4*3);
	}
	*/
	
	ScalF = ((minye2-maxys2)<<7)/height;

	roiW=(widthall*ScalF)>>7;

    maxys2=(maxys2+15)/16*16;
	
#if (0)	
    printf("xe2=%d,maxys2=%d,minye2=%d \n",xe2,maxys2,minye2);
#endif

#if 0	
    /***************************MOE *********************************/
	/***width and height should be 16x */

	roiW=(roiW/16)*16;
	roiH=(minye2-maxys2)/16*16;
       moeQue_t *pqueue;
	UINT8 *pack,*unpack,*packUV,*unpackUV;
	UINT32 roisize;
	SNOOP_AUTO_ARRAY_DECLARE(moeQue_t, 2, cmdque);
      memset(cmdque, 0, 2*64);
	printf_4("cmdque=%x \n",cmdque);
       unpack=pdst;
	pack=srcaddr;
	printf_4("unpack=%x,pack=%x \n",unpack,pack);
	roisize=(roiW)|(roiH<<16);
	printf("roiW=%d,roiH=%d \n",roiW,roiH);
	printf("outW=%d,outH=%d \n",outW,outH);

       /* Y(linear)  unpack to Y(block): {unpack} -> {pack} */
	 pqueue = &cmdque[0];
	 pqueue->dw0 = 0x00000004 | (0<<16);
	 pqueue->dw2 = roisize; /* roi */
	 pqueue->dw3 = LOGI_CACH_BADDR_TO_PHY_WADDR(unpack) | (1<<29); /*linear*/
	 pqueue->dw4 = (outW)|(outH<<16); /* srcsize */
	 pqueue->dw5 = (0x0000)|(maxys2<<16);  /* srcoffset */
	 /* pqueue->dw7 = 0; */ /* scalefac */
	 /* pqueue->dw8 = 0; */ /* scaleini */
	  pqueue->dw9 = LOGI_CACH_BADDR_TO_PHY_WADDR(pack) | (0<<29); /*block*/
	  pqueue->dw10 = roisize; /* dstsize */
	  /* pqueue->dw11 = 0; */ /* dstoffset */
    
        packUV=pack+16*16;
	 unpackUV=unpack+outW*outH;
	 /* UV(block)  unpack to UV(linear): {unpack} -> {pack} */
	 pqueue = &cmdque[1];
	 pqueue->dw0 = 0x00000005 | (0<<16);
	 pqueue->dw2 = roisize; /* roi */
	 pqueue->dw3 = LOGI_CACH_BADDR_TO_PHY_WADDR(unpackUV) | (3<<29); /*linear*/
	 pqueue->dw4 = (outW)|(outH<<16); /* srcsize */
	 pqueue->dw5 = (0x0000)|(maxys2<<16);  /* srcoffset */
	 /* pqueue->dw7 = 0; */ /* scalefac */
	 /* pqueue->dw8 = 0; */ /* scaleini */
	 pqueue->dw9 = LOGI_CACH_BADDR_TO_PHY_WADDR(packUV) | (2<<29); /*block*/
	 pqueue->dw10 = roisize; /* dstsize */
	 /* pqueue->dw11 = 0; */ /* dstoffset */


       cpuDCacheWB(cmdque, 2*64);
	halMoeLock();
       halMoeStart(LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(cmdque));
       halMoeWait();
	halMoeUnlock();
	cpuDCacheWB(srcaddr, (roiW)*(roiH)*2);
       cpuDCacheWB(pdst, outW*outH*2);

	/*fsSimpleWrite( "D:\\RESULT1.YUV", srcaddr, (roiW)*(roiH)*2 );
       fsSimpleWrite( "D:\\RESULT2.YUV", pdst, outW*outH*2 );*/
	

	printf_4("widthall=%d, height=%d \n",widthall,height);
       halJpegReset(); 
	yuvScaleByJpeg(pdst,widthall,height,srcaddr,roiW,roiH,0); /*dest W x16, dest H x16*/
	/*cdspDebugWriteFile( "D:\\RESULT.YUV", pdst, widthall*height*2 );*/
#endif
#if (0)
	printf("pack & scale %d ms \n", tmrMsTimeGet() - time1);
#endif
#endif

}


#endif
