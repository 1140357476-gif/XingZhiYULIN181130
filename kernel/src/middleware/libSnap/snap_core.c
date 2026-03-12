/***************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 ***************************************************************************/
/**
 * @filename 	snap_task.c
 * @brief 	The main file of snap flow
 * @author 	Wen-Guo
 * @since 	2007-12-01
 * @date 	2008-10-22
 */
#include "general.h"
#include "snap_debug.h"
#include "snap_core.h"
#include "snap_task.h"
#include "snap_task_internal.h"
#include "sp1k_snap_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "front_logi.h"
#include "cdsp_task.h"
#include "hal_dram.h"
#include "hal_jpeg.h"
#include "hal_front.h"
#include "reg_front.h"
#include "cdsp_init.h"
#include "gprm.h"
#include "utility.h"
#include "reg_def.h"
#include "gpio_api.h"
#include "initio.h"
#include "hal_moe.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_SNAP_CORE__
#define JOB_TABL_ADDR (UINT32)0xc000



/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

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
 
*     E X T E R N A L  F U N C T I O N    D E C L A R A T I O N S         *
 
**************************************************************************/
void cmdMemDump(UINT8 *param);

/**************************************************************************
 
*       S T A T I C    F U N C T I O N    D E C L A R A T I O N S        *
 **************************************************************************/
static UINT8 cdspDoJobTbl(UINT8 hNum,UINT8 vNum,UINT8 wrOffEn,UINT8 rawMode,UINT8 doVlc,UINT8 path,cdspjobtable_t xdata* jobaddr);
static UINT8 cdspDoFrontJobTbl(UINT8 hNum,UINT8 vNum,UINT8 rawMode,cdspjobtable_t xdata* jobaddr);
static UINT8 cdspDoPhotoFrmJobTbl(UINT8 hNum,UINT8 vNum,UINT8 wrVOffEn,UINT8 rawMode,UINT8 path,cdspjobtable_t xdata* jobaddr);
static UINT8 snapDoCdsp(UINT8 wrVOffEn,UINT8 rawMode,UINT8 path,UINT16 idx,cdspjobtable_t xdata* jobaddr);

static void jobTblDump(cdspjobtable_t xdata* jobaddr,UINT16 num);
static void snapDoFrontMoeBpUpInit(void);
static void snapDoFrontMoeBpLowTrig(void);

/**************************************************************************
 
*             F U N C T I O N S                                          *
 **************************************************************************/

/**
 * @fn        UINT32 snapCdspDoImg(UINT8 method)
 * @brief     make a image
 * @param     [in] method : 
 *\n					0: cdsp do a main image after front input a whole raw data
 *\n					1: cdsp starts to do main image immediately when front input enough raw data 
 *\n					2: do a quick view 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
snapCdspDoImg(
	UINT8 method
)
{
	UINT32 vlcSize;
	UINT16 para1, para2;
	
	frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );
	
	//XBYTE[REG_Dram_SdckMode]&= (~0x40);				/* MA14 pin enable, if DRAM size is equal or larger than 512M bits */ 

	cdspPostProcessCtrl(0,method); 		

	switch(method){
		case CDSP_DO_RAW:			
			vlcSize=cdspDoRaw();
		break;
		case CDSP_DO_FRONT:
			vlcSize=cdspDoFront();
		break;
		case CDSP_DO_QV:
			if( para1 == FRONT_DATA_FMT_YUV ) { // YUV sensor
				vlcSize=cdspDoQV(CDSP_SNAP_YUV,snapQvAddr,snapQvHSize,snapQvVSize);
			}
			else{
				vlcSize=cdspDoQV(CDSP_SNAP_8RAW,snapQvAddr,snapQvHSize,snapQvVSize); 
			}						
		break;
		case CDSP_DO_YUV:
			vlcSize=cdspDoYuv();
		break;
		case CDSP_DO_BLINK:
			if( para1 == FRONT_DATA_FMT_YUV ) { // YUV sensor				
				vlcSize=cdspDoQV(CDSP_SNAP_YUV,snapJpegVlcAddr,SNAP_CDSP_BLINK_HSIZE,SNAP_CDSP_BLINK_VSIZE);
			}
			else{				
				vlcSize=cdspDoQV(CDSP_SNAP_8RAW,snapJpegVlcAddr,SNAP_CDSP_BLINK_HSIZE,SNAP_CDSP_BLINK_VSIZE); 
			}								
		break;
		default:
		break;
	}
		
	//XBYTE[REG_Dram_SdckMode]|= 0x40;                /* MA14 pin disable */ 
	
 	return vlcSize;
}

/**
 * @fn        UINT32 cdspDoRaw(void)
 * @brief     cdsp do a main image after front input a whole raw data.
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
cdspDoRaw(
	void
)
{	
	cdspjobtable_t xdata* jobaddr;
	UINT8 jobhcnt, jobvcnt;
	UINT8 pixelSw; 
	UINT16 jobsize;	
	UINT16 hfact, vfact;	
	UINT8 vDramPath;
	UINT8 phFrmOpt = (UINT8)snapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT);

	//UINT8 dumpPara1[15]="0x2000";
	//UINT8 dumpPara2[15]="0x2700 +0x200";

	//UINT32 time1,time2;
	//chgRawEn = sp1kSnapParamGet(SP1K_SNAP_PARAM_CHANGERAW);
	
	HAL_CdspClockSet(1,7);			/* CDSP Clock set */

	HAL_CdspDramVInterval(0x0060);
	//XBYTE[REG_Cdsp_Dvtestcnt] = 0x13;    /* lower asking data rate from dram */
	
	HAL_CdspDramVModeSet(0);	/* DRAMV 8Bit raw mode */

	#ifdef DOYUV_FROM_CORE
		vDramPath=1;
		HAL_CdspVDramDataType(0x00,0x01,0x00);	/* will: YUV Path, sync with VD,YUV 0~256 */
	#else	
		vDramPath=0;
		HAL_CdspVDramDataType(0x00,0x01,0x00);	/*  YUV Path, sync with VD,YUV 0~256 */
	#endif	

	HAL_Cdsp1stPathWriteOffset(0,0);
	HAL_CdspVDramSizeClamp(1, snapRawHSize, 1, snapRawVSize, 0); 

	#ifdef DOYUV_FROM_CORE		
	
		HAL_Cdsp2ndPathClampHSize(1,0);	/* Enable Vdram1 h clamp */
		HAL_Cdsp2ndPathWriteOffset(0,0);
	
		//HAL_DramDispBufSel(0);	/*enable disp 2nd path*/	
		//HAL_Dram2ndPathFrmBufAAddrSet(snapYuv1Addr);
		//HAL_Dram2ndPathFrmBufBAddrSet(snapYuv2Addr);	
		//HAL_Dram2ndPathFrmBufSizeSet(snapRawHSize);
		
		HAL_Cdsp2ndPathCfg(0,0,0);/* Disable Skip vdram1 */
		HAL_Cdsp2ndPathEnSet(0);
		HAL_CdspVDramSkipWrSyncSet(0);	/* sync immediate*/
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff); 	/* skip AWB/AE/Edge/VDRAM1 Img data  */		
		
	#else	
		HAL_Cdsp2ndPathCfg(1,0,0);		/* disable 2nd Path */
		HAL_Cdsp2ndPathEnSet(0);		
		HAL_CdspVDramSkipWrSyncSet(0);	/* sync immediate*/				
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xfe);	

	#endif
	
	HAL_CdspIntplmir(1,1,1,1);		/* enable h left/right, v top/down */
	HAL_CdspYuvFilterBufSet(1);

	#if 0	
		if (chgRawEn!=1){
			HAL_CdspIntplCfg( 0x00,0xff,0x00,0x00); /* interpolation threshold settings */
		}
	#endif

	#ifdef DOYUV_CROP
		HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);  
	#endif


	#ifdef DOYUV_FROM_CORE		
		//XBYTE[0x2A3A] = 0x10; /* disable front gating */
		XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0xFF;	/* Clear wrap interrupt*/
		HAL_DramSnapRip(0); 	/* disable raw wrap */
	
		HAL_CdspCoreImgSource(1,0);	/* Set Core mux Source is from SDRAM */	
		HAL_CdspRawImgSource(0);
	#else
		HAL_CdspCoreImgSource(0,0);	/* Set Core mux Source is from GAMMA */
	#endif
	
	cdspDoBufInit( snapRawDataAddr, snapYuv1Addr, snapYuv2Addr, 
				snapCdspYuvToYuvBufAddr0,snapCdspYuvToYuvBufAddr1,
				snapRawHSize, snapRawVSize, 
				snapYuvHSize, snapYuvLines, 
				snapYuvABBufOpt, snapRawMode );   

	snapPrintfDoRaw("YUV A address=%lx\n",snapYuv1Addr);	
	snapPrintfDoRaw("YUV B address=%lx\n",snapYuv2Addr);				
	snapPrintfDoRaw("SNAP HSize=%d,VSize=%d\n",snapHSize,snapVSize);	
	snapPrintfDoRaw("RAW HSize=%d,VSize=%d\n",snapRawHSize,snapRawVSize);
	snapPrintfDoRaw("YUV HSize=%d,VSize=%d\n",snapYuvHSize,snapYuvVSize);
	snapPrintfDoRaw("Crop HSize=%d,VSize=%d\n",snapSensorCropHSize,snapSensorCropVSize);
	
	/* Jpeg initialize */				 
	HAL_JpgInit(snapYuv1Addr,snapJpegVlcAddr); /* Jpg buff initialize */	
	HAL_JpgZFactorSet(100,100,snapYuvHSize,snapYuvVSize,snapHSize,snapVSize,0); 
		
	/* Get Job Table */
	jobaddr = (cdspjobtable_t xdata*)JOB_TABL_ADDR;  /* set job table address*/
	pixelSw = XBYTE[REG_Cdsp_pixsel] & 0x03;

	//HAL_GlobalReadGTimer( &time1 );
	#ifdef DOYUV_CROP        /* will: crop while doing job table */
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 
							snapSensorCropHSize, snapSensorCropVSize,  							
							snapYuvHSize, snapYuvVSize, 							
							CDSPJOB_HSIZE,CDSPJOB_VSIZE,
							pixelSw, 0, 0, 1, jobaddr);   
	#else
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 
							snapRawHSize, snapRawVSize,       /* crop it first in crop engine */														
							snapYuvHSize, snapYuvVSize,
							CDSPJOB_HSIZE,CDSPJOB_VSIZE,
							pixelSw, 0, 0, 1, jobaddr);   	
	#endif							   
	//HAL_GlobalReadGTimer( &time2 );
	//printf("job table time =%ld \n",time2-time1);
	
	hfact = jobaddr->hscaleaccini;
	vfact = jobaddr->vscaleaccini;
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_YUVhscaleen] = 0x11;				  /* H YUV scale down enable */
	XBYTE[REG_Cdsp_YUVvscaleen] = 0x11;				  /* V YUV scale down enable */

	XBYTE[REG_Cdsp_YUVhscalefactor_low] = hfact&0xFF; 		
	XBYTE[REG_Cdsp_YUVhscalefactor_high] = (hfact>>8)&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_low] = vfact&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_high] = (vfact>>8)&0xFF;
	
	jobhcnt = jobsize&0xFF;
	jobvcnt = (jobsize>>8)&0xFF;

	snapPrintf("Job Table H Size = %bd \n", jobhcnt);	
	snapPrintf("Job Table V Size = %bd \n", jobvcnt);	

	XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;	/* CDSP write width idx */
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;					/* CDSP clear write idx */

	#ifdef DOYUV_FROM_CORE
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x11;	/* Write YUV via VDRAM1,JPG A/B buffer auto map */
		HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 
		HAL_DramWrYuvVDram1(1);		/* Write YUV via VDRAM1 */ 
	#else	
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & 0xef;	
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x10;	/* Write YUV via VDRAM ,JPG A/B buffer auto map */ 
		HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 		
	#endif


	if (phFrmOpt==1){
		XBYTE[REG_Dram_TargetYuv] = 0x00;					/* Jpeg doesn't start encoding for date stamp  */	   
	}
	else{
		XBYTE[REG_Dram_TargetYuv] = 0x01;					/* clear CDSP skip fram */	   
	}		

	
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx]|= 0x08;			/* cdsp skip frame */
	
	XBYTE[REG_Jpeg_Intr_Event] = 0x02;					/* Clear JPG interrupt */

	#if (FRONT_SNAP_BREAK_DOWN == 3)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);//Main vlc
	#endif
	
	XBYTE[REG_Jpeg_Swcontrol] = 0x01;					/* Trigger JPEG Compress */
	
	//snapRegDump(0);

	//HAL_GlobalReadGTimer(&time1);

	snapPrintf("0x21d0=0x%bx\n",XBYTE[0x21d0]);

	if (phFrmOpt==1){
		#if FUNC_KNL_FRM
		cdspDoPhotoFrmJobTbl(jobhcnt,jobvcnt,snapYuvABBufOpt,CDSP_SNAP_8RAW,vDramPath,jobaddr);
		#endif
	}
	else{	
		cdspDoJobTbl(jobhcnt,jobvcnt,snapYuvABBufOpt,CDSP_SNAP_8RAW,1,vDramPath,jobaddr);
	}
	
	//HAL_GlobalReadGTimer(&time2);

	//printf("job time=%ld\n",time2-time1);
	
	snapPrintfLog("JPEG  End \n"); 	  /* Jpg encode done */	

	#if (FRONT_SNAP_BREAK_DOWN == 3)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);//Main vlc end
	#endif

	//cmdMemDump(dumpPara1);
	//cmdMemDump(dumpPara2);

	#ifdef DOYUV_FROM_CORE
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x11);  
		HAL_DramABBufAutoMap(0);	
		HAL_DramWrYuvVDram1(0); 			
	#else	
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x10);   
		HAL_DramABBufAutoMap(0);	
	#endif
	
	XBYTE[REG_Cdsp_pixsel] = pixelSw; 

	snapPrintf("0x2800=0x%bx\n",XBYTE[REG_Jpeg_Intr_Event]);
	snapPrintf("VLC Size = %lu \n", snapVlcSizeGet()); 	

	HAL_CdspCoreImgSource(0,0);	/* set core mux source is from LUT-GAMMA */

	return snapVlcSizeGet();	
}

/**
 * @fn        UINT32 cdspDoFront(void)
 * @brief     cdsp starts to do main image immediately when front input enough raw data
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
cdspDoFront(
	void
)
{
	cdspjobtable_t xdata* jobaddr;
	UINT8 jobhcnt, jobvcnt, jobcnt;
	UINT8 pixelSw;
	UINT16 jobsize;	
	UINT16 hfact, vfact;	 


	
	snapDoFrontMoeBpUpInit();
	//XBYTE[0x2121] &=0x0f;
 	//HAL_DramDispBufSel(2);	/*enable disp 2nd path*/	
 	HAL_DramDispBufSel(1);	/*enable disp 2nd path*/	
 	
	HAL_CdspClockSet(1,7);					/* CDSP Clock set */
		
	HAL_CdspDramVInterval(0x0060);
	//HAL_CdspDramVInterval(0x00ff);
	
	HAL_CdspVDramDataType(0x01,0x01,0x00);	//will: Raw 8 bit mode , sync with VD,YUV 0~256
	HAL_CdspDramVModeSet(0);	/* DRAMV 8Bit raw mode */	
	
	// V DRAM1 Setting -------------------------------------------------------------------
	//XBYTE[0x2286] = 0x11;	
	HAL_CdspCoreImgSource(1,1);	/* Set Core mux Source is from SDRAM */
	HAL_Cdsp2ndPathCfg(0,0,0);/* Disable Skip vdram1 */
	HAL_Cdsp2ndPathClampHSize(1,0);	/* Enable Vdram1 h clamp */
	HAL_Cdsp2ndPathWriteOffset(0,0);
	
	HAL_Dram2ndPathFrmBufAAddrSet(snapYuv1Addr);
	HAL_Dram2ndPathFrmBufBAddrSet(snapYuv2Addr);	
	HAL_Dram2ndPathFrmBufSizeSet(snapRawHSize);


	cdspDoBufInit( snapRawDataAddr, snapYuv1Addr, snapYuv2Addr, 
				snapCdspYuvToYuvBufAddr0,snapCdspYuvToYuvBufAddr1, 
				snapRawHSize,snapRawVSize/2, 
				snapYuvHSize,snapYuvLines, 
				snapYuvABBufOpt, snapRawMode );
				
	
	snapPrintfDoFront("YUV A address=%lx\n",snapYuv1Addr);
	snapPrintfDoFront("YUV B address=%lx\n",snapYuv2Addr);  	 
	snapPrintfDoFront("RAW HSize=%d,VSize=%d\n",snapRawHSize,snapRawVSize);
	snapPrintfDoFront("YUV HSize=%d,VSize=%d\n",snapYuvHSize,snapYuvVSize);
	snapPrintfDoFront("Crop HSize=%d,VSize=%d\n",snapSensorCropHSize,snapSensorCropVSize);
	snapPrintfDoFront("SNAP HSize=%d,VSize=%d\n",snapHSize,snapVSize);	
	snapPrintfDoFront("YuvLines=%d\n",snapYuvLines);
	snapPrintfDoFront("ABBufOpt=%bd,RawMode=%bd\n",snapYuvABBufOpt,snapRawMode);
	

	HAL_JpgInit(snapYuv1Addr,snapJpegVlcAddr); /* Jpg initial buffer */
	HAL_JpgZFactorSet(100,100,snapYuvHSize,snapYuvVSize,snapHSize,snapVSize,0); 

	//snapRegDump(0);
	/* Get Jobs */
	jobaddr = (cdspjobtable_t xdata*)JOB_TABL_ADDR;  /* add table address to 04000 */
	pixelSw = XBYTE[REG_Cdsp_pixsel] & 0x03;

	
	#ifdef DOYUV_CROP
		jobsize = cdspjobget (snapRawHSize, snapRawVSize, 
							snapSensorCropHSize, snapSensorCropVSize,  
							snapYuvHSize, snapYuvVSize, 														
							CDSPJOB_HSIZE_DOFRONT,CDSPJOB_VSIZE_DOFRONT,
							pixelSw, 0, 0, 1, jobaddr);
	#else
		jobsize = cdspjobget (snapRawHSize, snapRawVSize, 
							snapRawHSize, snapRawVSize, 
							snapYuvHSize, snapYuvVSize,		/* zoom:  crop it first in crop engine */													
							CDSPJOB_HSIZE_DOFRONT,CDSPJOB_VSIZE_DOFRONT,
							pixelSw, 0, 0, 1, jobaddr);
	#endif							   
	 
	jobhcnt = jobsize&0xFF;
	jobvcnt = (jobsize>>8)&0xFF;

	snapPrintfDoFront("Job Table H Size = %bd \n", jobhcnt);
	snapPrintfDoFront("Job Table V Size = %bd \n", jobvcnt);
	
	jobcnt = (UINT16)(jobhcnt)*(jobvcnt)-1;
	
	/* Scaling Factor Setting */
	
	hfact = jobaddr->hscaleaccini;
	vfact = jobaddr->vscaleaccini;
	
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;	 
	XBYTE[REG_Cdsp_YUVhscaleen] = 0x11;				  /* H YUV scale down enable */
	XBYTE[REG_Cdsp_YUVvscaleen] = 0x11;				  /* V YUV scale down enable */

	XBYTE[REG_Cdsp_YUVhscalefactor_low] = hfact&0xFF;
	XBYTE[REG_Cdsp_YUVhscalefactor_high] = (hfact>>8)&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_low] = vfact&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_high] = (vfact>>8)&0xFF;

	snapPrintf("hfact = 0x%x \n", hfact);
	snapPrintf("vfact = 0x%x \n", vfact);	
	
	HAL_Cdsp1stPathWriteOffset(0,0);

	// Rst CDSP

	//HAL_CdspRstBlock(CDSPRstCDSP);/* Rst CDSP */


	#ifdef DOYUV_CROP
		HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);  
	#endif
	
	HAL_CdspVDramSizeClamp(1, snapRawHSize, 1, snapRawVSize, 0); 
	
	XBYTE[REG_Front_F2CdspGate] = 0x10;	/* disable front gating */
	
	//HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	 
	XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0xFF; //Clear wrap interrupt
	
	HAL_DramSnapRip(2);       /* Enable raw wrap, 1/2 */

	#if 1	//linjie.cheng fixed
	//gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);	
	HAL_CdspEofWait(1);	/*wait EOF for real time bad pixel */	
	//frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	//gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif

	XBYTE[REG_Cdsp_evnt1] = 0xFF;	/* Clear interrupt */
	XBYTE[REG_Cdsp_evnt2] = 0xFF;
	 
	//XBYTE[REG_Front_Snapnum] = 0x11;	/* Snap trigger */
	HAL_FrontSnapTrig(1,1);		/* Snap trigger */

	HAL_CdspVDramSkipWrSyncSet(0); /* sync immediately */
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);	/* skip AWB/AE/Edge data , sync immediately */ 

	XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;  /* Set Dram to auto mode */
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;
	
	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x11;	
	HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 
	HAL_DramWrYuvVDram1(1); 	/* Write YUV via VDRAM1 */ 
	
	XBYTE[REG_Dram_TargetYuv] = 0x01;				   /* clear CDSP skip fram */				  
	

	XBYTE[REG_Jpeg_Intr_Event] = 0x02;	/* Clear JPG interrupt */
	XBYTE[REG_Jpeg_Swcontrol] = 0x01;	/* Trigger JPEG Compress */
	
	//XBYTE[0x2284] = 0x00;
	//HAL_FrontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_FALLING, 1);	
	
	if (cdspDoFrontJobTbl(jobhcnt,jobvcnt,CDSP_SNAP_8RAW,jobaddr)){
		return 0;	/*do cdsp error , return vlc size = 0 */
	}	
	
	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x10);  
	HAL_DramABBufAutoMap(0);
	HAL_DramWrYuvVDram1(0);
	
	//XBYTE[0x2286]=0x00;     
	HAL_CdspCoreImgSource(0,0);	/* set core mux source is from LUT-GAMMA */

	XBYTE[REG_Cdsp_pixsel] = pixelSw; 

	//HAL_CdspVDramSkipWrSyncSet(0);	/*sync immediately */
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);	/* skip AWB/AE/Edge data , sync VD */ 
	
	//HAL_DramDispBufSel(reg_27b8);
	return snapVlcSizeGet();	
}

/**
 * @fn        UINT32 cdspDoQV(void)
 * @brief     do a quick view 
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
cdspDoQV(
	UINT8 rawType,
	UINT32 dstAddr,
	UINT16 dstHSize,
	UINT16 dstVSize
)
{
	cdspjobtable_t xdata* jobaddr;	
	UINT8 jobhcnt, jobvcnt;
	UINT8 pixelSw; 
	UINT16 jobsize;	
	UINT16 hfact, vfact;	
	UINT16 test_cnt=0; 
	UINT8 vDramPath;
	//UINT16 qvHSize,qvVSize;

	HAL_CdspClockSet(1,7);			/* CDSP Clock set */

	HAL_CdspDramVInterval(0x0060);
	//XBYTE[REG_Cdsp_Dvtestcnt] = 0x13;    /* lower asking data rate from dram */
	
	if(rawType==CDSP_SNAP_8RAW){
		HAL_CdspDramVModeSet(0x00);		/* DRAMV 8Bit raw mode */
	}	
	else{
		HAL_CdspDramVModeSet(0x02);		/* DRAMV 16Bit YUV mode */       		
	}

	#ifdef DOYUV_FROM_CORE
		vDramPath=1;
		HAL_CdspVDramDataType(0x00,0x01,0x00);	/* will: YUV Path, sync with VD,YUV 0~256 */
	#else
		vDramPath=0;
		HAL_CdspVDramDataType(0x00,0x01,0x00);	/* will: YUV Path, sync with VD,YUV 0~256 */
	#endif
		
	HAL_Cdsp1stPathWriteOffset(0,0);
	//HAL_CdspVDramSizeClamp(1, SNAP_CDSP_QV_HSIZE, 1, SNAP_CDSP_QV_VSIZE, 0); 
	HAL_CdspVDramSizeClamp(1, dstHSize, 1, dstVSize, 0); //4 //fan.xiao modified 2011-12-21 , 16:41:16 for 16:9 image
	
	#ifdef DOYUV_FROM_CORE			
		//HAL_DramDispBufSel(0);	/*enable disp 2nd path*/	
		HAL_Cdsp2ndPathClampHSize(1,0);	/* Enable Vdram1 h clamp */		
		HAL_Cdsp2ndPathWriteOffset(0,0);
	
		//HAL_Dram2ndPathFrmBufAAddrSet(snapQvAddr);
		//HAL_Dram2ndPathFrmBufBAddrSet(snapQvAddr);	
		//HAL_Dram2ndPathFrmBufSizeSet(SNAP_CDSP_QV_HSIZE);
		
		HAL_Cdsp2ndPathCfg(0,0,0);/* Disable Skip vdram1 */			
		HAL_Cdsp2ndPathEnSet(0);
		
		HAL_CdspVDramSkipWrSyncSet(0);	/* sync immediate*/
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);	/* skip AWB/AE/Edge/VDRAM1 Img data  */		
				
	#else
		HAL_Cdsp2ndPathCfg(1,0,0);		/* disable 2nd Path */
		HAL_Cdsp2ndPathEnSet(0);	
		HAL_CdspVDramSkipWrSyncSet(0);	/* sync immediate*/
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xfe); /* skip AWB/AE/Edge data  */ 
		
	#endif

		   
	HAL_CdspIntplmir(1,1,1,1);	     /* enable h left/right, v top/down */
	
	HAL_CdspYuvFilterBufSet(1);
	//HAL_CdspIntplCfg( 0x00,0xff,0x00,0x00); /* interpolation threshold settings */


	#ifdef DOYUV_CROP
		HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);   
	#endif
		
	
	#ifdef DOYUV_FROM_CORE		
		//XBYTE[0x2A3A] = 0x10; /* disable front gating */
		XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0xFF;	/* Clear wrap interrupt*/
		HAL_DramSnapRip(0); 	/* disable raw wrap */
	
		HAL_CdspCoreImgSource(1,0);	/* Set Core mux Source is from SDRAM */	
		HAL_CdspRawImgSource(0);
	#else
		HAL_CdspCoreImgSource(0,0);	/* Set Core mux Source is from GAMMA */	
	#endif	

	cdspDoBufInit(  snapRawDataAddr, dstAddr, dstAddr, 
					dstAddr, dstAddr, 
					snapRawHSize, snapRawVSize, 
					dstHSize, dstVSize, 
					snapYuvABBufOpt, snapRawMode );

	//HAL_CdspYEdgeEnSet(0);/*turn off YEdge*/
	
	snapPrintf("QV YUV A address=%lx\n",snapYuv1Addr);
	snapPrintf("QV YUV B address=%lx\n",snapYuv2Addr);				
	snapPrintf("QV RAW HSize=%d,VSize=%d\n",snapRawHSize,snapRawVSize);
	snapPrintf("QV YUV HSize=%d,VSize=%d\n",snapYuvHSize,snapYuvVSize);
	snapPrintf("QV Crop HSize=%d,VSize=%d\n",snapSensorCropHSize,snapSensorCropVSize);
	
	/* Get Job Table */
	jobaddr = (cdspjobtable_t xdata*)JOB_TABL_ADDR; 
	pixelSw = XBYTE[REG_Cdsp_pixsel] & 0x03;


	
	#ifdef DOYUV_CROP		 /* will: crop while doing job table */
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 								
						   	snapSensorCropHSize, snapSensorCropVSize, 
						   	dstHSize, dstVSize,
						   	CDSPJOB_HSIZE,CDSPJOB_VSIZE,
						   	pixelSw, rawType, 0, 1, jobaddr);			
	#else
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 
							snapRawHSize, snapRawVSize,   /* crop it first in crop engine */									
							dstHSize, dstVSize,
							CDSPJOB_HSIZE,CDSPJOB_VSIZE,							
							//SNAP_CDSP_QV_HSIZE, SNAP_CDSP_QV_VSIZE,
							//snapQvHSize,snapQvVSize,							
							pixelSw, rawType, 0, 1, jobaddr);   	
	#endif
	
	hfact = jobaddr->hscaleaccini;
	vfact = jobaddr->vscaleaccini;
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_YUVhscaleen] = 0x11;				  /* H YUV scale down enable */
	XBYTE[REG_Cdsp_YUVvscaleen] = 0x11;				  /* V YUV scale down enable */
	XBYTE[REG_Cdsp_YUVhscalefactor_low] = hfact&0xFF; 		
	XBYTE[REG_Cdsp_YUVhscalefactor_high] = (hfact>>8)&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_low] = vfact&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_high] = (vfact>>8)&0xFF;
	
	jobhcnt = jobsize&0xFF;
	jobvcnt = (jobsize>>8)&0xFF;


	snapPrintf("Job Table H Size = %bd \n", jobhcnt);	
	snapPrintf("Job Table V Size = %bd \n", jobvcnt);	

	
	XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;	/* CDSP write width idx */
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;					/* CDSP clear write idx */

	#ifdef DOYUV_FROM_CORE
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x11;	/* Write YUV via VDRAM1,JPG A/B buffer auto map */
		HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 
		HAL_DramWrYuvVDram1(1); 	/* Write YUV via VDRAM1 */ 
	#else
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & 0xef;	
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x10;	/* Write YUV via VDRAM,JPG A/B buffer auto map */
		HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 
	#endif
	
	XBYTE[REG_Dram_TargetYuv] = 0x01;					/* clear CDSP skip fram */	   
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx]|= 0x08;					/* cdsp skip frame */	

	#if (FRONT_SNAP_BREAK_DOWN == 3)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);//QV yuv Job (Raw to yuv)
	#endif
	
	cdspDoJobTbl(jobhcnt,jobvcnt,1,rawType,0,vDramPath,jobaddr);
	
	#ifdef DOYUV_FROM_CORE
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x11);   
		HAL_DramABBufAutoMap(0);
		HAL_DramWrYuvVDram1(0); 
		
	#else
		//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x10);   
		HAL_DramABBufAutoMap(0);
	#endif
	
	XBYTE[REG_Cdsp_pixsel] = pixelSw; 

	snapPrintf("VLC Size = %ld \n", snapVlcSizeGet()); 	

	#if (FRONT_SNAP_BREAK_DOWN == 3)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);//QV yuv Job (Raw to yuv) end
	#endif
	
	HAL_CdspCoreImgSource(0,0);	/* set core mux source is from LUT-GAMMA */

	return snapVlcSizeGet();	

}

UINT32 
cdspDoYuv(
	void
)
{	
	cdspjobtable_t xdata* jobaddr;
	UINT8 jobhcnt, jobvcnt;	
	UINT8 yuvSeq; 
	//UINT8 chgRawEn;
	UINT16 jobsize;	
	UINT16 hfact, vfact;	
	UINT8 phFrmOpt = (UINT8)snapParamGet(SP1K_SNAP_PARAM_PHOTO_FRM_OPT);
	
	//chgRawEn = sp1kSnapParamGet(SP1K_SNAP_PARAM_CHANGERAW);		

	HAL_CdspVDramSkipWrSyncSet(0); /* sync immediate*/
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xfe);/* skip AWB/AE/Edge data  */ 
	
	HAL_CdspClockSet(1,7);			/* CDSP Clock set */
	
	HAL_Cdsp2ndPathCfg(1,0,0);		
	
	HAL_Cdsp2ndPathEnSet(0);		/* disable 2nd Path */
	   
	HAL_CdspIntplmir(1,1,1,1);		/* enable h left/right, v top/down */

	HAL_CdspYuvFilterBufSet(1);
	
	#if 0	
		if (chgRawEn!=1){
			HAL_CdspIntplCfg( 0x00,0xff,0x00,0x00); /* interpolation threshold settings */
		}
	#endif
	
	HAL_CdspDramVInterval(0x0030);
	HAL_CdspVDramDataType(0x00,0x01,0x00);	/*  YUV Path, sync with VD,YUV 0~256 */
	
	
	#if 0	/* test no scale down first */
		if (rawhsize <= dsthsize)	hfact = 0x0000;
		else 				hfact = ((UINT32)dsthsize*65536)/rawhsize;
		
		if (rawvsize <= dstvsize)	vfact = 0x0000;
		else 				vfact = ((UINT32)dstvsize*65536)/rawvsize;
	#endif		
	
	cdspDoBufInit( snapRawDataAddr, snapYuv1Addr, snapYuv2Addr, 
				snapCdspYuvToYuvBufAddr0,snapCdspYuvToYuvBufAddr1,
				snapRawHSize, snapRawVSize, 
				snapYuvHSize, snapYuvLines, 
				snapYuvABBufOpt, snapRawMode );

	snapPrintfDoYUV("YUV A address=%lx\n",snapYuv1Addr);
	snapPrintfDoYUV("YUV B address=%lx\n",snapYuv2Addr);				
	snapPrintfDoYUV("RAW HSize=%d,VSize=%d\n",snapRawHSize,snapRawVSize);
	snapPrintfDoYUV("YUV HSize=%d,VSize=%d\n",snapYuvHSize,snapYuvVSize);
	snapPrintfDoYUV("Crop HSize=%d,VSize=%d\n",snapSensorCropHSize,snapSensorCropVSize);
	
	/* Jpeg initialize */				 

	HAL_JpgInit(snapYuv1Addr,snapJpegVlcAddr); /* Jpg buff initialize */
	//HAL_JpgInit(snapRawDataAddr,snapJpegVlcAddr,1); /* Jpg buff initialize */
	
	HAL_JpgZFactorSet(100,100,snapYuvHSize,snapYuvVSize,snapHSize,snapVSize,0); 

	#ifdef DOYUV_CROP
		HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);  
	#endif


	HAL_CdspDramVModeSet(0x02);	/* DRAMV YUV mode */       
	/* Get Job Table */
	jobaddr = (cdspjobtable_t xdata*)JOB_TABL_ADDR;  /* set job table address*/
	yuvSeq = XBYTE[REG_Front_Uvsel] & 0x03;

	
	snapPrintf("yuvSeq=%bd\n",yuvSeq);
	
	
	#ifdef DOYUV_CROP        								/* will: crop while doing job table */
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 
							snapSensorCropHSize, snapSensorCropVSize,  							
							snapYuvHSize, snapYuvVSize, 							
							CDSPJOB_HSIZE,CDSPJOB_VSIZE,
							yuvSeq, 1, 0, 1, jobaddr);   
	#else
		jobsize = cdspjobget(snapRawHSize, snapRawVSize, 
							snapRawHSize, snapRawVSize,       /* crop it first in crop engine */														
							snapYuvHSize, snapYuvVSize,
							CDSPJOB_HSIZE,CDSPJOB_VSIZE,
							yuvSeq, 1, 0, 1, jobaddr);   	
	#endif							   
	
	hfact = jobaddr->hscaleaccini;
	vfact = jobaddr->vscaleaccini;
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_YUVhscaleen] = 0x11;				  /* H YUV scale down enable */
	XBYTE[REG_Cdsp_YUVvscaleen] = 0x11;				  /* V YUV scale down enable */
	XBYTE[REG_Cdsp_YUVhscalefactor_low] = hfact&0xFF; 		
	XBYTE[REG_Cdsp_YUVhscalefactor_high] = (hfact>>8)&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_low] = vfact&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_high] = (vfact>>8)&0xFF;
	
	jobhcnt = jobsize&0xFF;
	jobvcnt = (jobsize>>8)&0xFF;

	snapPrintfDoYUV("Job Table H Size = %bd \n", jobhcnt);	
	snapPrintfDoYUV("Job Table V Size = %bd \n", jobvcnt);	
	
	XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;	/* CDSP write width idx */
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;					/* CDSP clear write idx */

	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & 0xef;	
	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x10;	/* JPG A/B buffer auto map */ 
	HAL_DramABBufAutoMap(1);	/*JPG A/B buffer auto map */ 

	XBYTE[REG_Dram_TargetYuv] = 0x01;					/* will : clear CDSP skip fram */	   
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx]|= 0x08;					/* cdsp skip frame */	
	
	XBYTE[REG_Jpeg_Intr_Event] = 0x02;					/* Clear JPG interrupt */

	
	XBYTE[REG_Jpeg_Swcontrol] = 0x01;					/* Trigger JPEG Compress */
	
	HAL_Cdsp1stPathWriteOffset(0,0);

	#if 1
		if (phFrmOpt==1){	
			#if FUNC_KNL_FRM
			cdspDoPhotoFrmJobTbl(jobhcnt,jobvcnt,snapYuvABBufOpt,CDSP_SNAP_YUV,0,jobaddr);
			#endif
		}
		else{
			cdspDoJobTbl(jobhcnt,jobvcnt,snapYuvABBufOpt,CDSP_SNAP_YUV,1,0,jobaddr);
		}
	#else
		for (j = 0; j < jobvcnt; j++) {
		
			for (i = 0; i < jobhcnt; i++) {
					
				XBYTE[0x2268] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->rhoffset		   & 0xFF;
				XBYTE[0x2269] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->rhoffset>>8)	   & 0xFF;

				XBYTE[0x226C] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->rvoffset		   & 0xFF;
				XBYTE[0x226D] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->rvoffset>>8)	   & 0xFF;
				XBYTE[0x2266] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->rhsize		   & 0xFF;
				XBYTE[0x2267] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->rhsize>>8)	   & 0xFF;
				XBYTE[0x226A] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->rvsize		   & 0xFF;
				XBYTE[0x226B] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->rvsize>>8)	   & 0xFF;
				XBYTE[0x2278] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->whoffset		   & 0xFF;
				XBYTE[0x2279] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->whoffset>>8)	   & 0xFF;
		
				#ifdef ONE_YUVBUF
					XBYTE[0x227A] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->wvoffset		   & 0xFF;	/* it's for YUV one buffer mode */
					XBYTE[0x227B] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->wvoffset>>8)	   & 0xFF;	/* it's for YUV one buffer mode */
				#endif
		
				XBYTE[REG_Cdsp_Yhscaleaccini_low] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->hscaleaccini	   & 0xFF;
				XBYTE[REG_Cdsp_Yhscaleaccini_high] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->hscaleaccini>>8) & 0xFF;
				XBYTE[0x21CE] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->vscaleaccini	   & 0xFF;
				XBYTE[0x21CF] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->vscaleaccini>>8) & 0xFF;
				XBYTE[0x227C] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->clamphsize		& 0xFF;
				XBYTE[0x227D] =  ((jobaddr+(UINT16)j*jobhcnt+ i)->clamphsize>>8)	& 0xFF;
				//XBYTE[0x2110] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->pixsw;
				XBYTE[0x2265] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->skippixel;
				XBYTE[REG_Cdsp_mirrorline] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->dummyline;
				

				#ifdef DOYUV_LSC_ON
					XBYTE[REG_Cdsp_mirrorline] |= 0x80;  /* will: do raw to raw while LSC is on */
				#endif	
				
				//XBYTE[REG_Cdsp_Dvtestcnt] = 0x18;    /* will: lower asking data rate from dram */
		
				XBYTE[0x2168] =  ( jobaddr+(UINT16)j*jobhcnt+ i)->hmirren | (((jobaddr+(UINT16)j*jobhcnt+ i)->vmirren)<<2);  /* Interpolation */ 
				XBYTE[0x21D1] =  ((XBYTE[0x21D0] & 0x10) ==0x10) ? XBYTE[0x2168] : 0x00;
				
				//XBYTE[0x2168] =  0x0f;
		
				//XBYTE[0x22ef] = 0x01; 		//Rst Report Size
				//XBYTE[0x22ef] = 0x00;

				//printf("2268=%bx,2269=%bx\n",XBYTE[0x2268],XBYTE[0x2269]);
				//printf("226c=%bx,226d=%bx\n",XBYTE[0x226c],XBYTE[0x226d]);

				
				XBYTE[REG_Cdsp_evnt1] = 0x02;					/* Clear Do cdsp interrupt */
				XBYTE[REG_Cdsp_Remode] |= 0x10; 		/* Trigger Do CDSP RGB */
						
				#if DBG_LEVEL>1 
					DBG_PRINTF("wait CDSP done\n");			
				#endif	
				
				test_cnt = 0;
				while ((XBYTE[REG_Cdsp_evnt1]&0x02) == 0x00){  /* Wait for CDSP Done */				
					test_cnt++;
					if (test_cnt>0x8000){
						//pvReportSize();
						#if DBG_LEVEL>0 
							MSG_PRINTF("Cdsp Done err %bd %\n",i+j*jobhcnt);
						#endif
						//printf("XBYTE[REG_Dram_MpgSb_CdspWRegIdx]=0x%bx\n",XBYTE[REG_Dram_MpgSb_CdspWRegIdx]);					
						return FAIL;
					}	
				} 
		
				//pvReportSize();
				//return 1;
				//printf("Segment 0x%02x done\n", (UINT16) (j*jobhcnt+ i));
			}
		
			if (j%2)	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;	 /* set cdsp write index */
			else		XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x01;
			
			docdsp_y =	docdsp_y + CDSPJOB_VSIZE;
			
			jpg_y	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
			//printf("DoCDSP_y = 0x%x, jpg_y = 0x%x \n", docdsp_y, jpg_y);
		
			while ((docdsp_y - jpg_y) > CDSPJOB_VSIZE) {
				jpg_y = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
				printf("cdsp_y=%d, jpg_y = %d \n", docdsp_y,jpg_y);
			}		
			
		}	
		
		#if DBG_LEVEL>0 
			MSG_PRINTF("Do CDSP End \n");
		#endif
		
		while ((XBYTE[REG_Jpeg_Intr_Event]&0x02) == 0x00){ /* wait for jpg encoder */ 
			jpg_y	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
			//printf("jpg2_y = 0x%x \n", jpg_y);
		}
	#endif
	
	snapPrintfLog("JPEG  End \n"); 	  /* Jpg encode done */	
	
	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x10);   
	HAL_DramABBufAutoMap(0);
	
	XBYTE[REG_Front_Uvsel] = yuvSeq; 

	snapPrintf("VLC Size = %ld \n", snapVlcSizeGet()); 	
	
	return snapVlcSizeGet();	
}


UINT8
cdspYuv2Yuv(
	UINT32 srcAddr,
	UINT32 dstAddr,
	UINT16 srcHSize,
	UINT16 srcVSize,
	UINT16 dstHSize, 
	UINT16 dstVSize,
	UINT16 doVSize
)
{	
	UINT8 yuvSeq;
	UINT8  jobhcnt, jobvcnt;
	UINT16 jobsize;	
	UINT16 hfact, vfact;
	UINT8 skipVal;
	
	cdspjobtable_t xdata* jobaddr;
	//HAL_Dram1stPathFrmBufAAddrSet( dstAddr );
	//HAL_Dram1stPathFrmBufSizeSet( hSize, vSize );
	//HAL_Dram1stPathFrmBufDAddrSet( srcAddr );
	//HAL_DramDoYuvSrcHSizeSet( hSize );

	cdspDoBufInit( srcAddr, dstAddr, dstAddr, 
				dstAddr, dstAddr,
				srcHSize, srcVSize, 
				dstHSize, dstVSize, 
				SNAP_ONE_YUVBUF, CDSP_SNAP_YUV);
	
	//HAL_CdspFrmCtrlEnSet(0);
	//HAL_CdspWriteBufIdxSet( 0 );
	//HAL_CdspVDramMode( 0, 1 );
	cdspSnapDramImgTypeSet( CDSP_SNAP_YUV );

	HAL_CdspVDramSkipWrGet(&skipVal);
	HAL_CdspVDramSkipWrSyncSet(0); /* sync immediate*/
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xfe);/* skip AWB/AE/Edge data  */ 

	HAL_Cdsp2ndPathCfg(1,0,0);	/*skip VDRAM 1*/


	HAL_CdspRawImgSource( 1 );

	HAL_CdspDramVModeSet(0x02);	/* DRAMV YUV mode */    
	
	HAL_CdspClockSet(1,7);
	HAL_CdspIntplmir(1,1,1,1);		/* enable h left/right, v top/down */

	HAL_CdspDramVInterval(0x0030);
	HAL_CdspVDramDataType(0x00,0x01,0x00);	/*  YUV Path, sync with VD,YUV 0~256 */

	HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);  

	jobaddr = (cdspjobtable_t xdata*)JOB_TABL_ADDR;  /* set job table address*/

	yuvSeq = XBYTE[REG_Front_Uvsel] & 0x03;	

	jobsize = cdspjobget(srcHSize, srcVSize, 
						srcHSize, srcVSize,  							
						dstHSize, dstVSize, 							
						CDSPJOB_HSIZE,doVSize,
						yuvSeq, CDSP_SNAP_YUV, 0, 1, jobaddr);   
						
	
	hfact = jobaddr->hscaleaccini;
	vfact = jobaddr->vscaleaccini;
	XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
	XBYTE[REG_Cdsp_YUVhscaleen] = 0x11;				  /* H YUV scale down enable */
	XBYTE[REG_Cdsp_YUVvscaleen] = 0x11;				  /* V YUV scale down enable */
	XBYTE[REG_Cdsp_YUVhscalefactor_low] = hfact&0xFF; 		
	XBYTE[REG_Cdsp_YUVhscalefactor_high] = (hfact>>8)&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_low] = vfact&0xFF;
	XBYTE[REG_Cdsp_YUVvscalefactor_high] = (vfact>>8)&0xFF;
	
	jobhcnt = jobsize&0xFF;
	jobvcnt = (jobsize>>8)&0xFF;

	snapPrintf("Y2Y Job Tbl H = %bd \n", jobhcnt);	
	snapPrintf("Y2Y Job Tbl V = %bd \n", jobvcnt);	


	
	XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;	/* CDSP write width idx */
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;					/* CDSP clear write idx */

	#if 0	
		XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & 0xef;	
		XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x10;	/* JPG A/B buffer auto map */ 
	#else	
		XBYTE[REG_Dram_TargetYuv] = 0x01;					/* clear CDSP skip fram */	   
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]|= 0x08;					/* cdsp skip frame */	
	
		cdspDoJobTbl(jobhcnt,jobvcnt,1,CDSP_SNAP_YUV,0,0,jobaddr);
	#endif

	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,skipVal);/* restore skip data  */ 

	//XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] & (~0x10);   

	//cdspSenClockSwitch( 1 );

	//HAL_CdspImgDo( 2, 0, hSize, 0, vSize, 3, 3, 240, doVSize, 0 );  /* small */

	//cdspSenClockSwitch( 0 );
	return  SUCCESS;
}

UINT32 
snapDoJpeg(
	UINT16 hfactor,
	UINT16 vfactor,
	UINT32 srcYuvAddr,
	UINT16 srcYuvWidth,
	UINT16 srcYuvHeight,
	UINT32 dstJpegVlcAddr,
	UINT16 dstWidth,
	UINT16 dstHeight
)
{		
	UINT8 jpegOpt = 0x20;
	
	
	/* Jpeg initialize */				 

	HAL_JpgInit(srcYuvAddr,dstJpegVlcAddr); /* Jpg buff initialize */
	//HAL_JpgInit(snapRawDataAddr,snapJpegVlcAddr,1); /* Jpg buff initialize */
	
	HAL_JpgZFactorSet(hfactor,vfactor,srcYuvWidth,srcYuvHeight,dstWidth,dstHeight,0); 

	HAL_JpegEncode( srcYuvAddr, dstJpegVlcAddr, 0, jpegOpt, 1 );

	snapPrintf("VLC Size = %ld \n", snapVlcSizeGet()); 	
	
	//cmdMemDump("0x2000 0x3000");
	return snapVlcSizeGet();	
}

/**
 * @fn        UINT8 cdspDoJobTbl(UINT8 hNum, UINT8 vNum, UINT8 wrVOffEn, UINT8 rawMode, UINT8 doVlc, UINT8 path, xdata* jobaddr)
 * @brief     cdspDoJobTbl
 * @param     [in] hNum
 * @param     [in] vNum
 * @param     [in] wrVOffEn
 * @param     [in] rawMode
 * @param     [in] doVlc
 * @param     [in] path
 * @param     [in] jobaddr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-6
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
cdspDoJobTbl(
	UINT8 hNum,
	UINT8 vNum,
	UINT8 wrVOffEn,
	UINT8 rawMode,
	UINT8 doVlc,
	UINT8 path,
	cdspjobtable_t xdata* jobaddr  
	
)
{
	UINT8 i,j;
	UINT16 jpgY, doCdspY;
	UINT16 idx;

	jpgY	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
	
	doCdspY  = 0;	

	#ifndef DOYUV_FROM_CORE
		HAL_CdspRstBlock(CDSPRstCDSP);/* Rst CDSP */		
		//HAL_CdspRstBlock(CDSPRstDramV);
	#else
		//HAL_CdspRstBlock(CDSPRstVDram1); /*it needs to reset VDram1 because we turn off VDram1 immediately*/
	#endif
	
	
	for (j = 0; j < vNum; j++) {
	
		for (i = 0; i < hNum; i++) {
			idx = (UINT16)j * hNum + i;		
			snapDoCdsp(wrVOffEn,rawMode,path,idx,jobaddr);			
		}

		snapPrintf("do j=%bd\n",j);
		/* set cdsp write index */

		if (j%2==1){	
			XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;
		}
		else{
			XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x01;
		}		

		doCdspY =	doCdspY + CDSPJOB_VSIZE;

		jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;

		snapPrintf("DoCDSP_y = %d, jpg_y = %d \n", doCdspY, jpgY);		
		
		if(doVlc==1){
			while ((doCdspY - jpgY) > CDSPJOB_VSIZE ) {
				jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;				
				snapPrintf("cdsp_y=%d, jpg_y = %d \n", doCdspY,jpgY);						
			}							

		}		
	}	
	
	snapPrintfLog("Do CDSP End \n");

	if (doVlc==1){
		while ((XBYTE[REG_Jpeg_Intr_Event]&0x02) == 0x00){ /* wait for jpg encoder */ 
			jpgY	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
			snapPrintf("jpg2_y = 0x%x \n", jpgY);
		}	
	}
	
	return SUCCESS;
}

static UINT8
cdspDoFrontJobTbl(
	UINT8 hNum,
	UINT8 vNum,
	UINT8 rawMode,
	cdspjobtable_t xdata* jobaddr  
	
)
{
	UINT8 i,j;

	UINT16 snapRawY,maxSnapRawY,doCdspY,swBpTblRawCheckPoint;
	UINT16 jpg_y; 
	UINT16 testCnt; 
	UINT16 idx;
	UINT8 sts;
	UINT8 swBpTbl;
	UINT16 rvSize,rvOffset;
	UINT8 badPixelOpt;

	ENTER_CRITICAL(sts);

	badPixelOpt = (UINT8)sp1kSnapParamGet(SP1K_SNAP_PARAM_BAD_PIXEL_OPT);
	doCdspY = 0;
	swBpTbl=0;

	swBpTblRawCheckPoint = snapRawVSize >> 1;
	
	maxSnapRawY = snapRawVSize >> 4 << 4;
	
	snapPrintfDoFront("Do CDSP Start \n");	

	snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
	//printf("snapRawY=%x,%bx\n",snapRawY,XBYTE[REG_Dram_Cdsp1Eof_Inten]);
	
	#if 1	//linjie.cheng dixed

	//need check 0x2e03[4]=always '1'
	if ((XBYTE[0x2e03]&0x10) != 0x10) {
		XBYTE[0x2e03] |= 0x10;
		ASSERT(0,1);
		//printf("sync\n");
	}	

	#if 1 
	/*wait SOF for real time bad pixel */	
	//gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	while((XBYTE[0x2700]&0x20)!=0x20);
	//gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x00);
		
	//printf("s=%x\n",snapRawY);
	#endif
	
	if (badPixelOpt!=SNAP_BP_DISABLE){
		HAL_CdspStillBpTrigger(1);	/*trigger real time bad pixel compensation */
		snapPrintf("bp trigger\n");
	}
	#endif
	
	rvSize = jobaddr->rvsize;
	while (snapRawY > rvSize){		/* fix bug, snapRawY wouldn't be clear until snap do strats */ 
		snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
		snapPrintf("1 snaprawy=%x \n", snapRawY);
	}
	
	i=0;	
	for (j = 0; j < vNum; j++) {

		idx = (UINT16)j * hNum; 	
		
		snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
		snapPrintf("2 snaprawy=%x \n", snapRawY);
		rvSize = (jobaddr + idx)->rvsize;
		rvOffset = (jobaddr + idx)->rvoffset;
		while((snapRawY != maxSnapRawY)&&(snapRawY < (rvOffset + rvSize))){
			snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
		}
			
		for (i = 0; i < hNum; i++) {
			idx = (UINT16)j * hNum + i;			
			XBYTE[REG_Cdsp_Rhoffset_low] 	= READ8((jobaddr + idx)->rhoffset,1);
			XBYTE[REG_Cdsp_Rhoffset_high] 	= READ8((jobaddr + idx)->rhoffset,0);
			XBYTE[REG_Cdsp_Rvoffset_low] 	= READ8((jobaddr + idx)->rvoffset,1);
			XBYTE[REG_Cdsp_Rvoffset_high] 	= READ8((jobaddr + idx)->rvoffset,0);
			XBYTE[REG_Cdsp_Rhsize_low] 		= READ8((jobaddr + idx)->rhsize,1);
			XBYTE[REG_Cdsp_Rhsize_high] 	= READ8((jobaddr + idx)->rhsize,0);			
			XBYTE[REG_Cdsp_Rvsize_low]		= READ8((jobaddr + idx)->rvsize,1);
			XBYTE[REG_Cdsp_Rvsize_high] 	= READ8((jobaddr + idx)->rvsize,0);
			
			XBYTE[REG_Cdsp_whoffset1_low] 	= READ8((jobaddr + idx)->whoffset,1);
			XBYTE[REG_Cdsp_whoffset1_high] 	= READ8((jobaddr + idx)->whoffset,0);
			
			XBYTE[REG_Cdsp_clamphsize1_low] = READ8((jobaddr + idx)->clamphsize,1);
			XBYTE[REG_Cdsp_clamphsize1_high]= READ8((jobaddr + idx)->clamphsize,0);

			XBYTE[REG_Cdsp_Yhscaleaccini_low]  = READ8((jobaddr + idx)->hscaleaccini,1);
			XBYTE[REG_Cdsp_Yhscaleaccini_high] = READ8((jobaddr + idx)->hscaleaccini,0);
			XBYTE[REG_Cdsp_Yvscaleaccini_low]  = READ8((jobaddr + idx)->vscaleaccini,1);
			XBYTE[REG_Cdsp_Yvscaleaccini_high] = READ8((jobaddr + idx)->vscaleaccini,0);
				
			if (rawMode != CDSP_SNAP_YUV){
				XBYTE[REG_Cdsp_CorePixSel] =  (jobaddr + idx)->pixsw;
			}
			
			XBYTE[REG_Cdsp_Skippixel] 	=  (jobaddr + idx)->skippixel;
			XBYTE[REG_Cdsp_mirrorline] 	=  (jobaddr + idx)->dummyline;
			XBYTE[REG_Cdsp_intplmirror] =  ((jobaddr + idx)->hmirren) | (((jobaddr + idx)->vmirren)<<2);  //Interpolation
			XBYTE[REG_Cdsp_EsmirEn] 	=  ((XBYTE[REG_Cdsp_EdgeEn] & 0x10) ==0x10) ? XBYTE[REG_Cdsp_intplmirror] : 0x00;
			XBYTE[REG_Cdsp_evnt1] 		= 0x02;			    /*Clear Do cdsp interrupt */

			XBYTE[REG_Dram_PauseReq_Low] = 0x01;	/* pause CDSP1 to avoid LSC error */
			XBYTE[REG_Dram_PauseReq_Low] = 0x01;	/* delay */
			XBYTE[REG_Dram_PauseReq_Low] = 0x01;	/* delay */
			XBYTE[REG_Cdsp_Remode] 	|= 0x10;	/*Trigger Do CDSP RGB */
			XBYTE[REG_Dram_PauseReq_Low] = 0x00;
			
			//snapPrintf("doCdspY = %x, snapRawY=%x \n", doCdspY, snapRawY);
			//snapPrintf("docdsp_y = %d, jpg_y = %d, snaprawy=%d \n", docdsp_y, jpg_y, snaprawy);
			testCnt =0;
			while ((XBYTE[REG_Cdsp_evnt1] & 0x02) == 0x00){ /* Waiting CDSP Done */
				testCnt++;
												
				if (testCnt > 0xf000) {
					snapPrintf("Do CDSP Time out \n");
					snapPrintf("job h=%bd,v=%bd\n",i,j);
					snapRawY= (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
					snapPrintf("snaprawy=0x%x \n", snapRawY);
					//jobTblDump(jobaddr,idx);

					#if 0	
						j=0;
						while(j<10){
							i=0;
							for (i=1;i<8;i++){
								XBYTE[0x21FC] = 0x1E;
								XBYTE[0x21FD] = i;
								printf("DramV%bd -> 0x21FE=0x%bx\n",i, XBYTE[0x21FE]);
							}
							/*
							XBYTE[0x21FC] = 0x0d;
							XBYTE[0x21FD] = 0x02;
							printf("LSC 2 -> 0x21FE=0x%bx\n",XBYTE[0x21FE]);

							i=0;

							for (i=1;i<7;i++){
								XBYTE[0x21FC] = 0x1d;
								XBYTE[0x21FD] = i;
								printf("VDram%bd -> 0x21FE=0x%bx\n",i, XBYTE[0x21FE]);
							}
							*/
							j++;
						}
					#endif		
					ASSERT(FALSE,0);
					//while(1);

					//return FAIL;
					//HAL_CdspRstBlock(CDSPRstCDSP);
					//i--;
					//break;
				}				
			}	
			
		}

		#if 1
		if ( (XBYTE[0x2700]&0x10) == 0x10 ) {
			//snapPrintfLog("Raw w-wrap over r-wrap\n");
			ASSERT(0,1);
			XBYTE[0x2700]=0x10;
		}
		#endif
		
		if (j%2){
			XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;
		}
		else{
			XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x01;
		}

		doCdspY += CDSPJOB_VSIZE_DOFRONT;
		jpg_y	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;

		snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
		//snapPrintfLog("doCdspY = %x, jpg_y=%x \n", doCdspY, jpg_y);
		
		while ((doCdspY - jpg_y) > CDSPJOB_VSIZE_DOFRONT)	{
			jpg_y = (UINT16)XBYTE[REG_Jpeg_Currenty]*16; 
			//snapPrintf("wait jpegy\n");
			snapPrintfDoFront("doCdspY = %x, jpg_y=%x \n", doCdspY, jpg_y);
		}

		if ( badPixelOpt != SNAP_BP_DISABLE ) {
			if ( swBpTbl == 0 ) {
				if ( (XBYTE[REG_Moe_Intr]&0x02) != 0 ) {
					if  ( snapRawY >= swBpTblRawCheckPoint ) {					
						XBYTE[REG_Moe_Intr] = 0x02;		
						snapDoFrontMoeBpLowTrig();
						swBpTbl = 1;
						snapPrintf("bp %d\n",snapRawY);
					}
				}
			}
		}
	}

	snapPrintfDoFront("Do CDSP End \n");

	if (badPixelOpt!=SNAP_BP_DISABLE){
		while ( (XBYTE[0x2E00]&0x02) == 0x00){;}	/*wait for MOE BP done*/
	}
	
	while ((XBYTE[REG_Jpeg_Intr_Event]&0x02) == 0x00){;}	/*wait for JPG done*/

	snapPrintfDoFront("JPEG Compress End \n");	

	while(snapRawY < maxSnapRawY){	/* wait unitl all of raw data are finished */
		snapRawY = (UINT16)XBYTE[REG_Cdsp_RawCurY]*16;
	}

	EXIT_CRITICAL(sts);
	
	//gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x30);	
	
	return SUCCESS;
}

#if FUNC_KNL_FRM

static UINT8
cdspDoPhotoFrmJobTbl(
	UINT8 hNum,
	UINT8 vNum,
	UINT8 wrVOffEn,
	UINT8 rawMode,
	UINT8 path,
	cdspjobtable_t xdata* jobaddr  
	
)
{
	UINT8 i,j;
	UINT16 jpgY, doCdspY;
	UINT16 idx;

	jpgY	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
	
	doCdspY  = 0;	

	#ifndef DOYUV_FROM_CORE
		HAL_CdspRstBlock(CDSPRstCDSP);/* Rst CDSP */		
		//HAL_CdspRstBlock(CDSPRstDramV);
	#else
		//HAL_CdspRstBlock(CDSPRstVDram1); /*it needs to reset VDram1 because we turn off VDram1 immediately*/
	#endif
	
	
	for (j = 0; j < vNum; j++) {
	
		for (i = 0; i < hNum; i++) {
			idx = (UINT16)j * hNum + i;		
			snapDoCdsp(wrVOffEn,rawMode,path,idx,jobaddr);		
		}

		if (j<(vNum-1)){
	
			if (j%4 == 0){
				HAL_Dram1stPathFrmBufAAddrSet(snapYuv1Addr + ((UINT32)snapYuvHSize * snapYuvLines >>1));			
			}
			else if (j%4 == 1){
				HAL_Dram1stPathFrmBufAAddrSet(snapYuv1Addr);	
				XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x01;		
				snapPhotoFrmPaste(snapYuv1Addr,j/2);
			}
			else if (j%4 == 2){
				HAL_Dram1stPathFrmBufBAddrSet(snapYuv2Addr + ((UINT32)snapYuvHSize * snapYuvLines >>1));
			}
			else{
				HAL_Dram1stPathFrmBufBAddrSet(snapYuv2Addr);
				XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;		
				snapPhotoFrmPaste(snapYuv2Addr,j/2);
			}
			
			doCdspY =	doCdspY + CDSPJOB_VSIZE;				
		}
		else{
			if ((j%4==0)||(j%4==1)){			
				snapPhotoFrmPaste(snapYuv1Addr,j/2);
			}
			else{
				snapPhotoFrmPaste(snapYuv2Addr,j/2);
			}

			if (j%2 == 0){				
				doCdspY =	doCdspY + (CDSPJOB_VSIZE);
			}
			else{
				doCdspY =	doCdspY + (CDSPJOB_VSIZE << 1);
			}
		}
		
		snapPrintf("do j=%bd\n",j);
		/* set cdsp write index */

		/* read JPEG Y from encode output, but it's not accurate */
		//jpgY = (UINT16)XBYTE[REG_Jpeg_Scl_Mcuy_High]*256 + (UINT16)XBYTE[REG_Jpeg_Scl_Mcuy_Low];
		//jpgY = ((jpgY+1)<<3);	
		//jpgY = (UINT16)((UINT32)jpgY * (UINT32)snapYuvVSize / (UINT32)snapVSize);
		
		jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;	

		
		snapPrintf("DoCDSP_y = %d, jpg_y = %d \n", doCdspY, jpgY);
		snapPrintf("cur Y=%d\n",(UINT16)XBYTE[REG_Jpeg_Currenty]*16);
		
		if (j==1){
			XBYTE[REG_Dram_TargetYuv] = 0x01;					/* Jpeg starts for photo frame  */	   
		}
		else if(j== (vNum-1)) {
			XBYTE[REG_Dram_Cdsp1or2] |= 0x20;					/* force jpg free-run to finish */
		}			
		else{
							
				if (snapHSize > snapYuvHSize){
					while ((doCdspY - jpgY) > ((CDSPJOB_VSIZE<<1)+16)) {						
						jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;	
						snapPrintf("cdsp_y=%d, jpg_y = %d \n", doCdspY,jpgY);
					}
				}
				else{
					while ((doCdspY - jpgY) > (CDSPJOB_VSIZE<<1)) {						
						jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
						snapPrintf("cdsp_y=%d, jpg_y = %d \n", doCdspY,jpgY);
					}
				}
					
				//jpgY = (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
				
				//testCdspCnt = XBYTE[REG_Dram_DoCdspYuvCurrentY_Low] + (UINT16)XBYTE[REG_Dram_DoCdspYuvCurrentY_High]*256;						
				//snapPrintfLog("cdsp_y=%d, jpg_y = %d \n", testCdspCnt,jpgY);
				//snapRegDump(0);
				//while(1);				
		}
	}
							
	snapPrintfLog("Do CDSP End \n");

	while ((XBYTE[REG_Jpeg_Intr_Event]&0x02) == 0x00){ /* wait for jpg encoder */ 
		jpgY	= (UINT16)XBYTE[REG_Jpeg_Currenty]*16;
		snapPrintf("jpg2_y = 0x%x \n", jpgY);
	}	
	
	XBYTE[REG_Dram_Cdsp1or2] &= 0xdf;	/* A/B Buffer auto-map function */
	
	return SUCCESS;

}
#endif

static UINT8
snapDoCdsp(
	UINT8 wrVOffEn,
	UINT8 rawMode,
	UINT8 path,
	UINT16 idx,
	cdspjobtable_t xdata* jobaddr  
)
{
	UINT16 test_cnt=0; 

	XBYTE[REG_Cdsp_Rhoffset_low]	= READ8((jobaddr + idx)->rhoffset,1);
	XBYTE[REG_Cdsp_Rhoffset_high]	= READ8((jobaddr + idx)->rhoffset,0);
	XBYTE[REG_Cdsp_Rvoffset_low]	= READ8((jobaddr + idx)->rvoffset,1);
	XBYTE[REG_Cdsp_Rvoffset_high]	= READ8((jobaddr + idx)->rvoffset,0);
	XBYTE[REG_Cdsp_Rhsize_low]		= READ8((jobaddr + idx)->rhsize,1);
	XBYTE[REG_Cdsp_Rhsize_high] 	= READ8((jobaddr + idx)->rhsize,0);
	XBYTE[REG_Cdsp_Rvsize_low]		= READ8((jobaddr + idx)->rvsize,1);
	XBYTE[REG_Cdsp_Rvsize_high] 	= READ8((jobaddr + idx)->rvsize,0);
	
	if (path==1){
		XBYTE[REG_Cdsp_whoffset1_low]	= READ8((jobaddr + idx)->whoffset,1);
		XBYTE[REG_Cdsp_whoffset1_high]	= READ8((jobaddr + idx)->whoffset,0);
		XBYTE[REG_Cdsp_clamphsize1_low] = READ8((jobaddr + idx)->clamphsize,1);
		XBYTE[REG_Cdsp_clamphsize1_high]= READ8((jobaddr + idx)->clamphsize,0); 						
	}
	else{
		XBYTE[REG_Cdsp_Whoffset_low]	= READ8((jobaddr + idx)->whoffset,1);
		XBYTE[REG_Cdsp_Whoffset_high]	= READ8((jobaddr + idx)->whoffset,0);	
		XBYTE[REG_Cdsp_Clamphsize_low]	= READ8((jobaddr + idx)->clamphsize,1);
		XBYTE[REG_Cdsp_Clamphsize_high] = READ8((jobaddr + idx)->clamphsize,0); 						
	}
	
	if(wrVOffEn==1){	/* it's for YUV one buffer mode */
		if (path==1){
			XBYTE[REG_Cdsp_wvoffset1_low]	= READ8((jobaddr + idx)->wvoffset,1);	
			XBYTE[REG_Cdsp_wvoffset1_high]	= READ8((jobaddr + idx)->wvoffset,0);	
		}
		else{
			XBYTE[REG_Cdsp_Wvoffset_low]	= READ8((jobaddr + idx)->wvoffset,1);	
			XBYTE[REG_Cdsp_Wvoffset_high]	= READ8((jobaddr + idx)->wvoffset,0);	
		}
	}	
	
	XBYTE[REG_Cdsp_Yhscaleaccini_low]	= READ8((jobaddr + idx)->hscaleaccini,1);
	XBYTE[REG_Cdsp_Yhscaleaccini_high]	= READ8((jobaddr + idx)->hscaleaccini,0);
	XBYTE[REG_Cdsp_Yvscaleaccini_low]	= READ8((jobaddr + idx)->vscaleaccini,1);
	XBYTE[REG_Cdsp_Yvscaleaccini_high]	= READ8((jobaddr + idx)->vscaleaccini,0);
		
	if (rawMode!=CDSP_SNAP_YUV){
		if (path==1){
			XBYTE[REG_Cdsp_CorePixSel] =  ( jobaddr + idx)->pixsw;
		}
		else{
			XBYTE[REG_Cdsp_pixsel] =  ( jobaddr + idx)->pixsw;				
		}
	}	
	
	XBYTE[REG_Cdsp_Skippixel] =  ( jobaddr + idx)->skippixel;
	XBYTE[REG_Cdsp_mirrorline] =  ( jobaddr + idx)->dummyline;
	
	#ifdef DOYUV_LSC_ON
		XBYTE[REG_Cdsp_mirrorline] |= 0x80;  /* will: do raw to raw while LSC is on */
	#endif	
	
	//XBYTE[REG_Cdsp_Dvtestcnt] = 0x18;    /* lower asking data rate from dram */
	
	XBYTE[REG_Cdsp_intplmirror] =  ( jobaddr + idx)->hmirren | (((jobaddr + idx)->vmirren)<<2);  /* Interpolation */ 
	XBYTE[REG_Cdsp_EsmirEn] =  ((XBYTE[REG_Cdsp_EdgeEn] & 0x10) ==0x10) ? XBYTE[REG_Cdsp_intplmirror] : 0x00;
	
	
	//XBYTE[REG_Cdsp_RstSize] = 0x01;		//Rst Report Size
	//XBYTE[REG_Cdsp_RstSize] = 0x00;
	
	XBYTE[REG_Cdsp_evnt1] = 0x02;				/* Clear Do cdsp interrupt */
	XBYTE[REG_Cdsp_Remode] |= 0x10; 		/* Trigger Do CDSP RGB */
			
	snapPrintf("wait CDSP done\n"); 		
	
	test_cnt = 0;
	while ((XBYTE[REG_Cdsp_evnt1]&0x02) == 0x00){  /* Wait for CDSP Done */ 			
		test_cnt++;
		if (test_cnt>0x8000){
			
			snapPrintfLog("CDSP DONE ERR\n");
	
			snapPrintf("job tbl=%d\n",(UINT16)i+(UINT16)j*hNum);
	
			//pvReportSize();
			//jobTblDump(jobaddr,hNum*vNum);
	
			#if 0
				j=0;
				while(j<10){
					i=0;
					for (i=1;i<8;i++){
						XBYTE[0x21FC] = 0x1E;
						XBYTE[0x21FD] = i;
						printf("DramV%bd -> 0x21FE=0x%bx\n",i, XBYTE[0x21FE]);
					}
	
					
					XBYTE[0x21FC] = 0x0d;
					XBYTE[0x21FD] = 0x02;
					printf("LSC 2 -> 0x21FE=0x%bx\n",XBYTE[0x21FE]);
	
					i=0;
	
					for (i=1;i<7;i++){
						XBYTE[0x21FC] = 0x1d;
						XBYTE[0x21FD] = i;
						printf("VDram%bd -> 0x21FE=0x%bx\n",i, XBYTE[0x21FE]);
					}
					j++;
				}
			#endif
			//return FAIL;
			while(1);	/* error happens , stay here to wait watch dog timer */
	
		}	
	} 

	return SUCCESS;
}

/**
 * @fn        UINT16 cdspjobget(UINT16 srcHSize, UINT16 srcVSize, UINT16 cropHSize, UINT16 cropVSize, UINT16 dstHSize, UINT16 dstVSize, UINT16 jobHSize, UINT16 jobVSize, UINT8 pixelSw, UINT8 yuvDoCdsp, UINT8 yuvScale, UINT8 get, xdata* pJobTbl)
 * @brief     create a job table
 * @param     [in] srcHSize : raw data horizontal size
 * @param     [in] srcVSize : raw data vertical size
 * @param     [in] cropHSize : raw data crop horizontal size
 * @param     [in] cropVSize : raw data crop vertical size
 * @param     [in] dstHSize : yuv destination horiztontal size (it can't be bigger than srcHSize)
 * @param     [in] dstVSize : yuv destination vertical size (it can't be bigger than srcVSize)
 * @param     [in] jobHSize : job table horizontal size
 * @param     [in] jobVSize : job table vertical size
 * @param     [in] pixelSw : pixel switch (bayer pattern)
 * @param     [in] yuvDoCdsp
 * @param     [in] yuvScale
 * @param     [in] upd : 
 *\n					0: don't updata job table 
 *\n					1: updata jobtable
 * @param     [in] pJobTbl : job table address 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
cdspjobget(
	UINT16 srcHSize, 
	UINT16 srcVSize,
	UINT16 cropHSize, 
	UINT16 cropVSize,
	UINT16 dstHSize, 
	UINT16 dstVSize,
	UINT16 jobHSize, 
	UINT16 jobVSize,
	UINT8  pixelSw, 
	UINT8 yuvDoCdsp, 
	UINT8 yuvScale, 
	UINT8 upd,
	cdspjobtable_t xdata* pJobTbl
)
{
	UINT16 i, j, k;
	UINT8		Skippixel, mrr_hpixel, mrr_vline, CDSPHcsm, CDSPVcsm, yuvmode, Hmrren, Vmrren;
	UINT8	  yuvmir, suppen;
	UINT16	srco_Hs, srco_Vs, src_Hs, src_Vs, dst_Hs, dst_Vs, src_HOFST, src_VOFST, src_HEND, src_VEND;
	UINT16	one, half, halfr, win_h0, src_HOFST8, src_HEND8, src_HOFSTR8;
	UINT16	Rhoffset, Rhsize, Whoffset, Clamphsize;
	UINT16	net_pixel, num_Hpixel, acc_Hpixel;
	UINT16	Rvoffset, Rvsize, Wvoffset, num_Vline, acc_Vline, sw;
	UINT32  Hsfactor, Vsfactor, Hsfactorini, Vsfactorini, Hscaleini, Vscaleini;

	

	srco_Hs	  = srcHSize;
	srco_Vs   = srcVSize;
	src_Hs    = cropHSize;
	//src_Hs	  = (src_Hs>>4)<<4;
	src_Vs    = cropVSize;
	dst_Hs    = dstHSize;
	dst_Vs    = dstVSize;

	src_HOFST = (srco_Hs - src_Hs)/2;
	src_VOFST = (srco_Vs - src_Vs)/2;
	src_HEND  = src_HOFST + src_Hs;
	src_VEND  = src_VOFST + src_Vs;

	yuvmode   = yuvDoCdsp | yuvScale;

	snapPrintfJobTbl("srco_Hs    = %d\n",srco_Hs);
	snapPrintfJobTbl("srco_Vs    = %d\n",srco_Vs);
	snapPrintfJobTbl("src_Hs     = %d\n",src_Hs);
	snapPrintfJobTbl("src_Vs     = %d\n",src_Vs);
	snapPrintfJobTbl("dst_Hs     = %d\n",dst_Hs);
	snapPrintfJobTbl("dst_Vs     = %d\n",dst_Vs);
	snapPrintfJobTbl("src_HOFST  = %d\n",src_HOFST);
	snapPrintfJobTbl("src_VOFST  = %d\n",src_VOFST);
	snapPrintfJobTbl("src_HEND   = %d\n",src_HEND );
	snapPrintfJobTbl("src_VEND   = %d\n",src_VEND );

	/**************************************************
 	* calculate the mirror/scaling factor            *
 	**************************************************/
 	suppen = ((XBYTE[REG_Cdsp_EdgeEn] & 0x10) == 0x10);
	yuvmir     =  suppen ? 2 : 0;
	mrr_hpixel = (suppen & ~yuvmode) ? 4 : 2;
	mrr_vline  = (suppen & ~yuvmode) ? 4 : 2;
  	//mrr_vline  = (suppen & ~yuvmode) ? 6 : 4;	/*will try it*/
  	
	CDSPHcsm   = (yuvScale==1) ? 1 : mrr_hpixel*2;
	CDSPVcsm   = mrr_vline*2;

	if	(dst_Hs <  src_Hs)	{Hsfactor = ((UINT32)dst_Hs*65536 + src_Hs - 1)/src_Hs;}
	else                  	{Hsfactor = 0x10000;}

	if	(dst_Vs < src_Vs)	{Vsfactor = ((UINT32)dst_Vs*65536 + src_Vs -1 )/src_Vs;}
	else 					{Vsfactor = 0x10000;}

	Hsfactorini = Hsfactor;
	Vsfactorini = Vsfactor;
	
	#if 0
		printf("Hsfactorini   =%lx\n", Hsfactorini   );
		printf("Vsfactorini   =%lx\n", Vsfactorini   );
	#endif

	/**************************************************
 	* window size decisdion                          *
 	**************************************************/
 	one = 16;
 	half  = yuvDoCdsp ? 8 : 16;
 	halfr = 8; //Raw data half for hoffset
	win_h0      = jobHSize/one*one;

	src_HOFST8  = src_HOFST/half*half;
	src_HOFSTR8 = src_HOFST/halfr*halfr;
 	src_HEND8   = (src_HEND + (half-1)) / half*half;
	if (yuvScale == 0) {
		if ((src_HOFST%8)!=0){
			snapPrintf("Err job: Start X is not 8n\n");
		}
		if ((src_Hs   %8)!=0){
			snapPrintf("Err job: Rd Hsize is not 8n\n");
		}
	}
	/**************************************************
 	* Horizontal partition                           *
 	**************************************************/
	i =0; j =0;
	if ((src_HEND8 - src_HOFSTR8) <= win_h0){
		if (yuvScale==1) {
			Hmrren     = 3;
			Skippixel  = ((src_HOFST % half)==0) ? 0 : (src_HOFST % half);
		}
		else {
			Hmrren     = (mrr_hpixel==0)         ? 0 : 3;
			Skippixel  = ((mrr_hpixel%half)==0)  ? 0 : (half - mrr_hpixel%half);
		}
		Rhoffset   = src_HOFSTR8;
		Rhsize     = src_HEND8 - src_HOFST8;
		Whoffset   = 0;
		Hscaleini  = Hsfactorini;
		Clamphsize  = dst_Hs;

		if (upd!=0) {
			pJobTbl->hmirren      = Hmrren;
			pJobTbl->rhoffset     = (Rhoffset/half*half);
			pJobTbl->rhsize       = (Rhsize  /half*half);
			pJobTbl->whoffset     = Whoffset;
			pJobTbl->hscaleaccini = Hscaleini;
			pJobTbl->skippixel    = 0;
			pJobTbl->clamphsize    = Clamphsize;
		}
	}
	else {
	/**************************************************
 	* Horizontal first partition                     *
 	**************************************************/
		if (yuvScale==1) {
			Hmrren     = 1;
			Skippixel  = ((src_HOFST%half)==0) ? 0 : (src_HOFST%half);
		}
		else {
			Hmrren     = (mrr_hpixel==0) ? 0 : 1;
			Skippixel  = 0;
		}

		Rhoffset   = src_HOFSTR8;
		Rhsize     = win_h0;
		Whoffset   = 0;
		Hscaleini  = Hsfactorini;
		net_pixel  = (Rhsize - Skippixel - (yuvmode ? 0 : 2));
		num_Hpixel = ((Hscaleini + (UINT32)Hsfactor*(net_pixel-1)) >> 16) - yuvmir;
		num_Hpixel = num_Hpixel/half*half;
		acc_Hpixel = num_Hpixel + Whoffset;
		Clamphsize  = num_Hpixel;
		if (upd!=0) {
			pJobTbl->hmirren      = Hmrren;
			pJobTbl->rhoffset     = Rhoffset;
			pJobTbl->rhsize       = (Rhsize/half*half);
			pJobTbl->whoffset     = Whoffset;
			pJobTbl->hscaleaccini = Hscaleini;
			pJobTbl->skippixel    = Skippixel;
			pJobTbl->clamphsize    = Clamphsize;
		}

		/**************************************************
		* Horizontal second..... partition               *
		**************************************************/
		while ( (Rhoffset+Rhsize) < src_HEND8) {
			i = i + 1;
			mrr_hpixel = (yuvScale==1) ? 0 : mrr_hpixel;
			Whoffset  = acc_Hpixel;
			Rhoffset  = ((((UINT32)(Whoffset-yuvmir)*65536)-Hsfactorini)/Hsfactor+1) - (yuvmode ? 0 : 2) + src_HOFST;
			Hscaleini = (Hsfactorini + ((UINT32)Hsfactor*((Rhoffset - src_HOFST + (yuvmode ? 0 : 2))))) & 0xFFFF;
			if (Hscaleini==0)  { Hscaleini = Hsfactorini; }
			Skippixel = Rhoffset%half;
			Rhoffset  = Rhoffset/half*half;
			if ((Rhoffset + win_h0) < src_HEND8) {
				Hmrren    = 0;
				Rhsize    = win_h0;
				net_pixel = (Rhsize - Skippixel - (yuvmode ? 0 : 4));
			}
			else if ((Rhoffset + win_h0) < src_HEND8) {
				Hmrren    = 0;
				Rhsize    = win_h0;
				net_pixel = (Rhsize - Skippixel - (yuvmode ? 0 : 4));
			}
			else {
				Hmrren    = (yuvScale==1) ? 0 : 2;
				Rhsize    = ( src_HEND8) - Rhoffset;
				net_pixel = (src_HEND - Rhoffset - Skippixel - (yuvmode ? 0 : 2));
			}
			num_Hpixel = ((Hscaleini+Hsfactor*(net_pixel-1))>>16)- (Hmrren ? yuvmir : (yuvmir*2));
			num_Hpixel = num_Hpixel/half*half;
			acc_Hpixel = num_Hpixel + acc_Hpixel;
			Clamphsize  = num_Hpixel;
			if (upd!=0) {
				(pJobTbl+i)->hmirren       = Hmrren;
				(pJobTbl+i)->rhoffset     = (Rhoffset/half*half);
				(pJobTbl+i)->rhsize       = (Rhsize  /half*half);
				(pJobTbl+i)->whoffset     = Whoffset;
				(pJobTbl+i)->hscaleaccini = Hscaleini;
				(pJobTbl+i)->skippixel    = Skippixel;
				(pJobTbl+i)->clamphsize    = Clamphsize;
			}
		}
	}

	/**************************************************
 	* Vertical partition                             *
 	**************************************************/
	if (dst_Vs <= jobVSize) {
		Vmrren    = (mrr_vline==0) ? 0 : 3;
		if (upd!=0) {
			for (k=0; k<=i; k++) {
				(pJobTbl+k)->vmirren      = Vmrren;
				(pJobTbl+k)->rvoffset     = src_VOFST;
				(pJobTbl+k)->rvsize       = src_Vs;
				(pJobTbl+k)->wvoffset     = 0;
				(pJobTbl+k)->vscaleaccini = Vsfactorini;
				if ((Vmrren&0x02) == 2)
				(pJobTbl+k)->dummyline    = mrr_vline | 0x40;
				else
				(pJobTbl+k)->dummyline    = 0x00;
			}
		}
	}
	else {

	/**************************************************
	* Vertical first partition                       *
 	**************************************************/
		Vmrren    = (mrr_vline==0) ? 0 : 1;
		num_Vline = jobVSize;
		Rvoffset  = src_VOFST;
		Wvoffset  = 0;
		Vscaleini = Vsfactorini;
		Rvsize    = (((UINT32)(num_Vline + yuvmir)*65536)-Vscaleini + Vsfactor-1)/Vsfactor + 1 + (yuvmode ? 0 : 2);
		acc_Vline = Wvoffset + num_Vline;
		acc_Vline = acc_Vline;
		if (upd!=0) {
			for (k=0;k<=i;k++) {
				(pJobTbl+k)->vmirren      = Vmrren;
				(pJobTbl+k)->rvoffset     = Rvoffset;
				(pJobTbl+k)->rvsize       = Rvsize;
				(pJobTbl+k)->wvoffset     = Wvoffset ;
				(pJobTbl+k)->vscaleaccini = Vscaleini;
				if ((Vmrren&0x02) == 2)
				(pJobTbl+k)->dummyline    = mrr_vline | 0x40;
				else
				(pJobTbl+k)->dummyline    = 0x00;
			}
		}
	/**************************************************
	* Vertical second..... partition                 *
	**************************************************/
		while ( (Wvoffset + num_Vline) < dst_Vs ) {
			j = j + 1;
			Wvoffset  = acc_Vline;
			Rvoffset  = ((((UINT32)(Wvoffset-yuvmir)*65536)-Vsfactorini)/Vsfactor+1) - (yuvmode ? 0 : 2) + src_VOFST;

			snapPrintfJobTbl("jobVSize  =%x\n", (UINT16)jobVSize);
			snapPrintfJobTbl("Wvoffset     =%x\n", (UINT16)Wvoffset);
			snapPrintfJobTbl("Rvoffset     =%x\n", (UINT16)Rvoffset);
			snapPrintfJobTbl("yuvmir       =%x\n", (UINT16)yuvmir);
			snapPrintfJobTbl("Vsfactorini  =%lx\n", (UINT32)Vsfactorini);
			snapPrintfJobTbl("Vsfactor     =%lx\n", (UINT32)Vsfactor);

			Vscaleini = (Vsfactorini + (Vsfactor * (Rvoffset - src_VOFST + (yuvmode ? 0 : 2)))) & 0xFFFF;
			if (Vscaleini==0)  { Vscaleini = Vsfactorini; }
			if ((Wvoffset + jobVSize) < dst_Vs) {
				Vmrren    = 0;
				num_Vline = jobVSize;
				Rvsize    = (((num_Vline + (Vmrren ? yuvmir : (yuvmir*2)))*(UINT32)65536)-Vscaleini + Vsfactor-1)/Vsfactor + 1 + (yuvmode ? 0 : 4);
			}
			else {
				Vmrren    = (mrr_vline==0) ? 0 : 2;
				num_Vline = dst_Vs - Wvoffset;
				Rvsize    = (((num_Vline + (Vmrren ? yuvmir : (yuvmir*2)))*(UINT32)65536)-Vscaleini + Vsfactor-1)/Vsfactor + 1 + (yuvmode ? 0 : 2);
			}
			acc_Vline = num_Vline + acc_Vline;

			snapPrintfJobTbl("jobVSize  =%x\n", (UINT16)jobVSize);
			snapPrintfJobTbl("Wvoffset     =%x\n", (UINT16)Wvoffset);
			snapPrintfJobTbl("Rvoffset     =%x\n", (UINT16)Rvoffset);
			snapPrintfJobTbl("Vscaleini    =%x\n", (UINT16)Vscaleini);
			snapPrintfJobTbl("num_Vline    =%x\n", (UINT16)num_Vline);
			snapPrintfJobTbl("Rvsize       =%x\n", (UINT16)Rvsize);
			snapPrintfJobTbl("acc_Vline    =%x\n\n", (UINT16)acc_Vline);

			if (upd!=0) {
				for (k=0;k<=i;k++) {
					(pJobTbl+j*(i+1)+k)->hmirren      = (pJobTbl+k)->hmirren;
					(pJobTbl+j*(i+1)+k)->rhoffset     = (pJobTbl+k)->rhoffset;
					(pJobTbl+j*(i+1)+k)->rhsize       = (pJobTbl+k)->rhsize;
					(pJobTbl+j*(i+1)+k)->whoffset     = (pJobTbl+k)->whoffset;
					(pJobTbl+j*(i+1)+k)->hscaleaccini = (pJobTbl+k)->hscaleaccini;
					(pJobTbl+j*(i+1)+k)->skippixel    = (pJobTbl+k)->skippixel;
					(pJobTbl+j*(i+1)+k)->clamphsize    = (pJobTbl+k)->clamphsize;
					(pJobTbl+j*(i+1)+k)->vmirren      = Vmrren;
					(pJobTbl+j*(i+1)+k)->rvoffset     = Rvoffset;
					(pJobTbl+j*(i+1)+k)->rvsize       = Rvsize;
					(pJobTbl+j*(i+1)+k)->wvoffset     = Wvoffset;
					(pJobTbl+j*(i+1)+k)->vscaleaccini = Vscaleini;
					if ((Vmrren&0x02) == 2)
					(pJobTbl+j*(i+1)+k)->dummyline    = mrr_vline | 0x40;
					else
					(pJobTbl+j*(i+1)+k)->dummyline    = 0x00;
				}
			}
		}
	}
	/**************************************************
 	* Put the end mark                               *
 	* Change the linesel/pixelsel for every job      *
 	**************************************************/
	(pJobTbl              )->skippixel 	= (pJobTbl              )->skippixel | 0x4000; 	/*Start*/
	(pJobTbl+(j+1)*(i+1)-1)->skippixel 	= (pJobTbl+(j+1)*(i+1)-1)->skippixel | 0x8000;	/*end*/
	if (upd!=0) {
		for (k=0;k<(i+1)*(j+1);k++) {
			sw = ((pJobTbl+k)->rvoffset & 0x0fff) + ((((pJobTbl+k)->rvoffset>>15)==1) ? mrr_vline : 0);
			sw = sw * 2 + (((pJobTbl+k)->skippixel & 0xf)%2);
			sw = (sw & 0x3) ^ pixelSw;
			(pJobTbl+k)->pixsw = sw;
		}
	}


	//jobTblDump(pJobTbl,j+1);
	//jobTblDump(pJobTbl,20);
	
	return ((j+1)*256 + (i+1));

}

/**
 * @fn        UINT32 snapVlcSizeGet(void)
 * @brief     Get vlc size
 * @param     NONE
 * @retval    return = vlc size
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
snapVlcSizeGet(
	void
)
{
	UINT32 vlcSize;

	((UINT8 *)&(vlcSize))[3] = XBYTE[REG_Jpeg_VLCSize_Byte0];
	((UINT8 *)&(vlcSize))[2] = XBYTE[REG_Jpeg_VLCSize_Byte1];
	((UINT8 *)&(vlcSize))[1] = XBYTE[REG_Jpeg_VLCSize_Byte2];
	((UINT8 *)&(vlcSize))[0] = 0;

	return vlcSize;
}

void 
snapDoFrontMoeBpUpInit(
	void
)
{
	/* real time bad pixel compensation */
	HAL_MoeReset();
	HAL_MoeEnable();
	HAL_CdspStillBpCfg(0,1,0,snapRawHSize); 

	HAL_CdspStillBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize);		  	
	HAL_CdspStillBpRAWAddrSet(snapRawDataAddr);
	
	HAL_CdspStillBpEnSet(1);
	//sp1kSnapParamSet(SP1K_SNAP_PARAM_BAD_PIXEL_OPT, SNAP_BP_REALTIME);		
	XBYTE[REG_Moe_Intr] = 0xFF;	/* clear interrupt status */

	//printf("0x2e30 = 0x%bx ,0x%bx,0x%bx,0x%bx\n",XBYTE[0x2e30],XBYTE[0x2e31],XBYTE[0x2e32],XBYTE[0x2e33]);
	//printf("0x2e10=%bx\n",XBYTE[0x2e10]);

}

void
snapDoFrontMoeBpLowTrig(
	void
)
{
	UINT32 addr;

	addr = snapRawDataAddr - (((UINT32)snapRawHSize*snapRawVSize)>>2);
	HAL_CdspStillBpRAWAddrSet(addr);
	addr = K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + K_SDRAM_BadPixel_CapBufSize_Upper;
	HAL_CdspStillBpTblAddrSet(addr);
	//snapPrintf("bp addr=0x%lx\n",addr);
	HAL_CdspStillBpTrigger(1);	/*trigger real time bad pixel compensation */				
	
}
#if 0
static void
jobTblDump(
	cdspjobtable_t xdata* jobaddr,
	UINT16 num
)
{


		//*jobaddr=*jobaddr;
		//num=num;
	//#else
		UINT16 k;
		for (k=0;k<num;k++) {
		//for (k=425;k<427;k++){
			//k=num;
			snapPrintfJobTblDump("%d \n",k);
			snapPrintfJobTblDump("rhoffset     = %d \n",(UINT16)(jobaddr+k)->rhoffset);
			snapPrintfJobTblDump("rvoffset     = %d \n",(UINT16)(jobaddr+k)->rvoffset);
			snapPrintfJobTblDump("rhsize       = %d \n",(UINT16)(jobaddr+k)->rhsize);
			snapPrintfJobTblDump("rvsize       = %d \n",(UINT16)(jobaddr+k)->rvsize);
			snapPrintfJobTblDump("whoffset     = %d \n",(UINT16)(jobaddr+k)->whoffset);
			snapPrintfJobTblDump("wvoffset     = %d \n",(UINT16)(jobaddr+k)->wvoffset);
			snapPrintfJobTblDump("hscaleaccini = 0x%04x \n",(UINT16)(jobaddr+k)->hscaleaccini);
			snapPrintfJobTblDump("vscaleaccini = 0x%04x \n",(UINT16)(jobaddr+k)->vscaleaccini);
			snapPrintfJobTblDump("clamphsize   = %d \n",(UINT16)(jobaddr+k)->clamphsize);
			snapPrintfJobTblDump("skippixel    = 0x%02x \n",(UINT16)(jobaddr+k)->skippixel);
			snapPrintfJobTblDump("pixsw        = 0x%02x \n",(UINT16)(jobaddr+k)->pixsw);
			snapPrintfJobTblDump("dummyline    = 0x%02x \n",(UINT16)(jobaddr+k)->dummyline);
			snapPrintfJobTblDump("hmiren       = 0x%02x \n",(UINT16)(jobaddr+k)->hmirren);
			snapPrintfJobTblDump("vmiren       = 0x%02x \n\n",(UINT16)(jobaddr+k)->vmirren);
		}	
		//snapRegDump(2);
}


#endif

