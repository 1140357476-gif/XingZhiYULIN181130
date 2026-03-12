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
 * @file	  pb_dzoom.c
 * @brief	  play back digital zoom base function
 * @author	  TQ.zhang
 * @since	  2010-05-05
 * @date	  2010-05-05
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "os_msg.h"
#include "hal_disp.h"
#include "hal_jpeg.h"
#include "disp_api.h"
#include "pb_general.h"
#include "pb_api.h"
#include "pb_debug.h"
#include "initio.h"
#include "exiffunc.h"
//#include "exifhdr.h"
#include "doslink.h"
#include "cardlink.h"
#include "hal_cdsp.h"
#include "hal.h"
#include "hal_dram.h"
#include "snap_qtable.h"
#include "vfs_api.h"
#include "hal_dram.h"
#include "reg_def.h"
#include "app_msg.h"
#include "Hal_gprm.h"
#include "hal_disp.h"

#include "utility.h"

#include "exif_def.h"
#include "exif_entry.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#if (!STILL_LIB_PB_DMY)
#define PB_DZOOM_SMOOTH
#define PB_DZOOM_STEP	4

#define PB_DZOOM_FACTOR_MAX	4
#define PB_DZOOM_DEC_WIDTH	960//864//1024	//display max width
#define PB_DZOOM_DEC_HEIGHT	720//648//768		//display max height

#define PB_DZOOM_DISP_BUFF	K_SDRAM_ImagePlaybackBufAddr

#if 1
#define PB_CHECK_ABORT()	do {\
								if(osMsgSeek(SP1K_MSG_KEY_PRESS_LEFT) ||osMsgSeek(SP1K_MSG_KEY_PRESS_RIGHT)) {\
									return FAIL;\
								}\
							} while(0)
#else
#define PB_CHECK_ABORT()	do {if(osMsgQuery()) {return FAIL;}} while(0)
#endif

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT8 xdata pbDZoomSts;
//UINT8 facSts = 0;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern void dispImgWinSet(UINT16 xoff, UINT16 yoff, UINT16 w, UINT16 h);
extern UINT8 SDRAM_Dram2Sram(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz);

extern PBFile_t xdata MediaInfo;
//extern xdata	UINT32	G_DOS_FileSize;
extern xdata UINT8 exifVlcAtOdd;
extern xdata UINT8 exifAppN;
extern xdata UINT32 exifQvYTblAddr;
extern xdata UINT32 exifQvUVTblAddr;
extern xdata UINT8 exifQvVlcAtOdd;

#endif

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//pbDZoomFactorSet()
//-----------------------------------------------------------------------------
/**
 * @fn        void pbDZoomFactorSet(UINT16 Factor)
 * @brief     pbDZoomFactorSet
 * @param     [in] Factor : set zoom factor
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if STILL_LIB_PB_DMY
void pbDZoomFactorSet(UINT16 Factor) USING_0
{
	Factor=Factor;
}
#else

void pbDZoomFactorSet(UINT16 Factor) USING_0
{
	UINT16 decWidth;
	UINT16 decHeight;
	//UINT8 dev;
	//UINT8 sts;
	
	if (pbDZoomSts != TRUE) {
		pbDZoomStart();
	}
	#if 0
	sts = Factor / 100;
	dev = dispDevActGet();
	
	if(dev != 1){
		pbDecodeSizeGet(&decWidth, &decHeight);
	}
	if(dev == 1 ){
		pbDecodeSizeGet(&decWidth, &decHeight);
		if(facSts != 1 ){
			HAL_DispImgXyUpdate(decWidth, decHeight);
			dispShow( PB_DZOOM_DISP_BUFF,decWidth, decHeight,Factor);
			facSts	= 1;
			return;
		}

	}
	#else
	pbDecodeSizeGet(&decWidth, &decHeight);
	#endif

	#if 0
	HAL_DispScaleSet(decWidth, decHeight, Factor,0);
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	#else
	HAL_DispScaleSet(decWidth, decHeight, Factor,0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
	#endif

}
#endif

/**
 * @fn        UINT8 pbImgPanSet(SINT16 stepX, SINT16 stepY)
 * @brief     for display pan set
 * @param     [in] stepX
 * @param     [in] stepY
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbImgPanSet(SINT16 stepX, SINT16 stepY) USING_0
{
	stepX=stepX;
	stepY=stepY;
	return SUCCESS;
}
#else
UINT8 pbImgPanSet(SINT16 stepX, SINT16 stepY) USING_0
{
	UINT16 i;
	UINT8 ret = SUCCESS;
	SINT16 zoomStep;

	//printf("x %d, y%d\n", stepX, stepY);
	if (!stepX && !stepY) {
		ret = HAL_DispImgPanSet(stepX, stepY);
		return ret;
	}
#ifdef PB_DZOOM_SMOOTH
	if (stepX && stepY)
#else
	if (1)
#endif
	{
		//can't move smoothly
		ret = HAL_DispImgPanSet(stepX, stepY);
		HAL_DramZUpdateWait(0x02, 10000);
		HAL_DramZfUpdTrig(0x02);
	} else if (stepX) {
	
		if (stepX & 0x8000) {
			i = -stepX;
			zoomStep = -PB_DZOOM_STEP;
		} else {
			i = stepX;
			zoomStep = PB_DZOOM_STEP;
		}
		
		i = i / PB_DZOOM_STEP;
		HAL_DramZfUpdTrig(0x02); //Phil: Trigger the current setting for moving smoothly at the beginning
		do {
		/*
			if (osMsgSeek(SP1K_MSG_KEY_RELEASE_LEFT) || osMsgSeek(SP1K_MSG_KEY_RELEASE_RIGHT)) {
				break;
			}
		*/
			ret = HAL_DispImgPanSet(zoomStep, 0);
			HAL_DramZUpdateWait(0x02, 10000);
			HAL_DramZfUpdTrig(0x02);
		} while(--i);
	} else if(stepY){
	
		if (stepY & 0x8000) {
			i = -stepY;
			zoomStep = -PB_DZOOM_STEP;
		} else {
			i = stepY;
			zoomStep = PB_DZOOM_STEP;
		}
		
		i = i / PB_DZOOM_STEP;
		HAL_DramZfUpdTrig(0x02); //Phil: Trigger the current setting for moving smoothly at the beginning
		do {
		/*
			if (osMsgSeek(SP1K_MSG_KEY_RELEASE_UP) || osMsgSeek(SP1K_MSG_KEY_RELEASE_DOWN)) {
				break;
			}
		*/
			ret = HAL_DispImgPanSet(0, zoomStep);
			HAL_DramZUpdateWait(0x02, 10000);
			HAL_DramZfUpdTrig(0x02);
		} while(--i);
	}
	return ret;

}
#endif

void pbDispDevW_H_Get(UINT16 *width,UINT16 *height,UINT16 srcWidth,UINT16 srcHeight)
{
	UINT8 id;
	UINT8 dev;
	
	struct pbDecSize{
		UINT16 width;
		UINT16 height;
	};

	struct pbDecSize *pdecSize = NULL;
	struct pbDecSize tv[] ={
		{1280,960},
		{1280,720}    /* 16 : 9 */ 
	};

	struct pbDecSize hdmi[] ={
		{960,720},
		{640,360}   /* 16 : 9 */
	};

	struct pbDecSize lcd[] ={
		{640,480},
		{640,360}   /* 16 : 9 */
	};
	
	dev = dispDevActGet();
	id = ((srcWidth*3 == srcHeight*4) || (srcWidth*4 == srcHeight*3))?0:1;

	switch(dev){
		case 1:
			pdecSize = &lcd;
		break;
		case 2:
		case 3:
			pdecSize = &tv;
		break;
		case 4:
			pdecSize = &hdmi;
		break;					
	}
	
	*width = pdecSize[id].width;
	*height = pdecSize[id].height;
	
	if(*width > srcWidth){  // mantis bug #45543,jpg encode can't scale down
		*width = *width/2;
		*height = *height/2;
	}
}


/**
 * @fn        UINT8 pbDZoomStart(void)
 * @brief     pbDZoomStart
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if (!STILL_LIB_PB_DMY)
UINT8 pbDZoomStart(void)
{
	UINT8 sts = TRUE;
	UINT8 bufNE = FALSE;
	UINT16 fHandle = TRUE;
	UINT32 decSize = 0;
	UINT16 decWidth = PB_DZOOM_DEC_WIDTH;
	UINT16 decHeight = PB_DZOOM_DEC_HEIGHT;
	UINT32 srcAddr;
	UINT32 freeBuffBtm = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA, 2 Bytes for odd file size
	UINT32 freeBuffTop = PB_DZOOM_DISP_BUFF;
	PBFile_t* pFile = &MediaInfo;
	ExifImage_t mainImage;
	ExifImage_t qvImage;
	UINT32 decBuff;
	UINT8 rotMode;
	Pb_Dec_Parameter decPara;

	UINT16 decWidthSv;
	UINT16 decHeightSv;
	UINT8 device;
	
	if (pbDZoomSts == TRUE) {
		return SUCCESS;
	}
	
	//4 To do a completely open process, start cluster must be 0
	fHandle =  vfsFileOpen(pFile->FileName, K_DOS_ReadFile, 0/*pFile->FileStartCluster*/);
	if (!fHandle) {
		DBG_pbFuncTrack();
		return FAIL;
	}
	decSize = vfsFileSizeGet(fHandle);
	if ((decSize >> 1) > (freeBuffBtm - freeBuffTop)) {
		DBG_pbFuncTrack();
		bufNE = TRUE;  //rhy++ if qv success,but decode main image buffer not enough ,decode qv again...
	}
	if(bufNE == TRUE){
       	//decSize = 64*1024;
		decWidth = PB_DECODE_WIDTH;
		decHeight = PB_DECODE_HEIGHT;
		pbDecodeSizeSet(decWidth, decHeight);
	}
	srcAddr = freeBuffBtm - (decSize >> 1);
	pbDecodeSizeGet(&decWidthSv, &decHeightSv);
	//pbDecodeSizeSet(decWidth, decHeight);  //cx mask
	sts = vfsFileRead(fHandle, K_DOS_DstDram, decSize, srcAddr, 0);
	
	if (sts != TRUE) {
		vfsFileClose(fHandle, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}
	sts = exifParserStart(exifGetMainInfo,&mainImage, srcAddr, decSize);
	vfsFileClose(fHandle,  pFile->FileName, K_DOS_ReadFile, 0);
	if (sts != TRUE) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	pFile->MediaWidth = mainImage.width;
	pFile->MediaHeight = mainImage.height;
	pbLogPrintf("w is %u, h is %u \n", mainImage.width, mainImage.height);

	//xian ++ 20080623
	pbParamSet(PB_PARAM_EXIF_ORIENT, (UINT16)mainImage.orientation);
	rotMode = pbDecRotModeGet();
	pbLogPrintf("rotMode=%bu\n", rotMode);

	/*get decode size from different device */
	device = dispDevActGet();
	pbDispDevW_H_Get(&decWidth,&decHeight,mainImage.width,mainImage.height);
	decBuff = freeBuffTop;// + (UINT32)PB_DZOOM_DEC_WIDTH * 32UL;
	decPara.DecBuff = decBuff;
	decPara.RotMode = rotMode;
	decPara.DecW = decWidth;
	decPara.DecH = decHeight;

	sts = FAIL; //fix 41427
	if(pbCheckAbort(0,__LINE__)){
		return PBABORT;
	}
	if(mainImage.width < 8191 && bufNE != TRUE) {   
		decPara.ImgType = 0;
		sts = pbImgDeCmpRot(&mainImage, &decPara, 0);

		if(sts==PBABORT){ //Not decode QV again,There maybe exits someproblem....
			return sts;
		}
	}
     
	//xian ++ 20090827
	if(exifAppN && sts == FAIL || bufNE == TRUE){
		UINT8 xdata qTable[128];
		UINT16 i;
		exifParserStart(exifGetQVInfo, &qvImage, srcAddr, decSize);
		qvImage.dataType = mainImage.dataType;
		
		SDRAM_CardSetStartAddress(exifQvYTblAddr , TRUE );
		if (exifQvVlcAtOdd){
			UINT8 Dummy;
			M_DRAM_ReadWord(Dummy, qTable[0]);
			for (i = 1; i < 127; i += 2){
				M_DRAM_ReadWord(qTable[i], qTable[i+1]);
			}
			M_DRAM_ReadWord(qTable[127], Dummy);
		}else{
			for (i = 0; i < 128; i += 2){
				M_DRAM_ReadWord(qTable[i], qTable[i+1]);	
			}
		}
		jpegQTableSet(0, qTable, (UINT8*)(qTable+64), 1);

		decPara.ImgType = 2;
		decPara.DecW = 320;
		decPara.DecH = 240;
		sts = pbImgDeCmpRot(&qvImage, &decPara, 0);
		//pbDecodeSizeSet(decPara.DecW, decPara.DecH);
	}
	else if ( sts == FAIL){
		pbDecodeSizeSet(decWidthSv, decHeightSv);
		pbDZoomSts = TRUE;
		return SUCCESS;
	}
	//xian -- 20090827
	pbLogPrintf("w %u, h %u\n", decWidth, decHeight);

	//xian mask 20080708, fix bug 27912	PB_CHECK_ABORT();
	
	pbDecodeSizeGet(&decWidth, &decHeight);
	if(sts==SUCCESS){
		//printf("show W=%u,H=%u\n",decWidth,decHeight);
		//dramFileSave("MMMMVVVV.YUV",freeBuffTop, (UINT32)decWidth*decHeight*2);
		dispShow(freeBuffTop, decWidth, decHeight, 100);
	}  
	pbDispBufModeSet(PB_DISP_BUF_MODE_ZOOM);
	pbDZoomSts = TRUE;
	return SUCCESS;
}
#endif
//xian ++ 20080623
/**
 * @fn        void pbDZoomPause(void)
 * @brief     pause to zoom
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if STILL_LIB_PB_DMY
void pbDZoomPause(void) USING_0
{
}
#else

#if 0
void pbCropDispBuffer2ABBuf()
{
	UINT32 ImgX0, ImgX1, ImgY0, ImgY1;
	UINT16 recWidth,recHeight;
	UINT16 srcWidth,srcHeight;
	UINT32 dispAddr;
	UINT32 dispAddr_tmp = pbDispBuffGet(1);
	
	HAL_DispImgXyGet(&ImgX0, &ImgY0, &ImgX1, &ImgY1);
	HAL_DispPbFrameBufferGet(&dispAddr,&srcWidth,&srcHeight);

	recWidth = ((ImgX1 - ImgX0 + 1)+7)&~(0x7);
	recHeight = ((ImgY1 - ImgY0 + 1)+1)&~(0x1);

	HAL_GprmCopy(
		dispAddr,
		srcWidth,srcHeight,ImgX0,ImgY0,
		dispAddr_tmp,
		recWidth,recHeight,0,0,
		recWidth,recHeight,
		0,
		1
	);

	dispShow(pbDispBuffGet(1), recWidth, recHeight, 100);
	pbDispBufModeSet(PB_DISP_BUF_MODE_AB);
	pbDecodeSizeSet(recWidth, recHeight);	
}
#else
void pbCropDispBuffer2ABBuf()
{
	UINT32 ImgX0, ImgX1, ImgY0, ImgY1;
	UINT16 recWidth,recHeight,srcWidth,srcHeight;
	UINT32 dispAddr;
	UINT8 needScale = 0;
	UINT32 tmpAddr;
	UINT32 dispAddr_tmp = pbDispBuffGet(1);
	UINT32 abBufSize;
	
	abBufSize = (UINT32)960*720;
	HAL_DispImgXyGet(&ImgX0, &ImgY0, &ImgX1, &ImgY1);
	HAL_DispPbFrameBufferGet(&dispAddr,&srcWidth,&srcHeight);

	recWidth = ImgX1 - ImgX0 + 1;
	recHeight = ImgY1 - ImgY0 + 1;
	recWidth = recWidth &~(0x7);
	recHeight = recHeight &~(0x1);

	if((UINT32)srcWidth*srcHeight > abBufSize){
		needScale = 1;
		dispAddr_tmp = dispAddr +(UINT32)srcWidth*srcHeight;
	}
	
	HAL_GprmCopy(
		dispAddr,
		srcWidth,srcHeight,ImgX0,ImgY0,
		dispAddr_tmp,
		recWidth,recHeight,0,0,
		recWidth,recHeight,
		0,
		1
	);

	if(needScale){
		tmpAddr = dispAddr_tmp + (UINT32)recWidth*recHeight;
		if(tmpAddr > K_SDRAM_TotalSize ){
			printf("buf not ...\n");
		}
		
		HAL_GprmScale(
			dispAddr_tmp,
			recWidth, recHeight, 
			pbDispBuffGet(1),
			recWidth>>1,recHeight>>1,
			tmpAddr, 
			1
		);
		recWidth = recWidth>>1;
		recHeight = recHeight>>1 ;
		
	}
	
	dispShow(pbDispBuffGet(1), recWidth, recHeight, 100);
	pbDispBufModeSet(PB_DISP_BUF_MODE_AB);
	pbDecodeSizeSet(recWidth, recHeight);	
}

#endif
void pbDZoomPause(void) USING_0
{
	UINT32 addr=pbDispBuffGet(1);
	UINT16 w=PB_DECODE_WIDTH;
	UINT16 h=PB_DECODE_HEIGHT;

	if(pbDZoomSts != TRUE){
		//facSts	= 0;
		return;
	}

/*	pbDecodeSizeGet(&w, &h);
	if(w==PB_DZOOM_DEC_WIDTH && h==PB_DZOOM_DEC_HEIGHT){
		w = PB_DECODE_WIDTH;
		h = PB_DECODE_HEIGHT;
	}*/
	//suny +
	#if 0
	if(dispDevActGet() ==4){
		HAL_DispPbFrameBufferGet(&addr,NULL,NULL);
		pbDecodeSizeGet(&w, &h);
	}
	else{//cx ++
		pbDispQVSizeGet(&w,&h); //xian 20110214
	}
	#else
	pbDispQVSizeGet(&w,&h); //xian 20110214
	#endif
	dispShow(addr, w, h, 100);
	pbDispBufModeSet(PB_DISP_BUF_MODE_AB);
	pbDecodeSizeSet(w, h);	

}
#endif
//xian --

