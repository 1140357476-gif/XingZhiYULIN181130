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
 * @file	  spe_distort.c
 * @brief	  special effect main function
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"

#include "spe_api.h"
#include "spe_distort.h"

#include "spe_distort.h"
#include "pb_photoFrame.h"
#include "Pb_api.h"
#include "pb_general.h"
#include "hal_gprm.h"
#include "hal_global.h"
#include "utility.h"
#include "dbg_def.h"
#include "hal.h"
#include "vfs_api.h"
#if EN_SPE
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define ROT_0 0x01
#define ROT_90 0x06
#define ROT_180 0x03
#define ROT_270 0x08

#define OrderToAddr(addr, offset)		((addr) = ((addr) + ((offset) - 1)) &~ ((offset) - 1))

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT16 g_speParams[SPE_PARAM_MAX];

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern void dispShow(UINT32 addr, UINT16 w, UINT16 h, UINT16 factor);

extern RECT16 g_pb_rec_RotOut;
//extern xdata UINT16 exifMainXSize;
//extern xdata UINT16 exifMainYSize;

extern xdata UINT16 scaleChunk;
extern xdata UINT8 g_pbDecAlignOpt;
extern UINT8 imgScaleFac;

UINT8 yuvflag _at_ 0x7300;
UINT32 speBuffSize;
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void speDistortParamConversion(
	UINT16 *hOff_, 
	UINT16 *vOff_, 
	UINT16 *hSize_, 
	UINT16 *vSize_
);

/**
 * @fn		  void speDistort(UINT8 id, UINT32 srcAddrWord, UINT16 srcHsize, UINT16 srcVsize, UINT16 hOffset, UINT16 vOffset, UINT16 hSize, UINT16 vSize, UINT32 tmpAddrWord)
 * @brief	  specify distort type
 * @param	  [in] id : distort type
 * @param	  [in] srcAddrWord
 * @param	  [in] srcHsize
 * @param	  [in] srcVsize
 * @param	  [in] hOffset
 * @param	  [in] vOffset
 * @param	  [in] hSize
 * @param	  [in] vSize
 * @param	  [in] tmpAddrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 speDistort(
	UINT8 id, 
	UINT32 srcAddrWord, 
	UINT16 srcHsize, 
	UINT16 srcVsize, 
	UINT16 hOffset, 
	UINT16 vOffset, 
	UINT16 hSize, 
	UINT16 vSize, 
	UINT32 tmpAddrWord
)
{
	UINT8 sts;
#if EN_PRINTF_TIME
	UINT32 time1,time2;
	HAL_GlobalReadGTimer(&time1);
#endif	  
	ENTER_CRITICAL(sts);
	
	//----//
	//printf("distort:%u,%u, %u,%u, %u,%u\n",srcHsize,srcVsize,hOffset,vOffset,hSize,vSize);
	if(srcHsize<32 || srcVsize <24 )
	{
		printf("size err!");
		return FAIL;
	}
	if(hOffset + hSize > srcHsize || vOffset + vSize > srcVsize || hSize<8 || vSize<8)
	{
		hOffset=0;
		hSize=srcHsize;
		vOffset=0;
		vSize=srcVsize;
		printf("size err!");
		return FAIL;
	}
	
	speSetStructImg(
		srcAddrWord, 
		srcHsize, 
		srcVsize, 
		hOffset& ~0x0001, 
		vOffset& ~0x0001, 
		hSize & ~0x0001, 
		vSize & ~0x0001, 
		tmpAddrWord
		);

	speSetStructMap(
		img.winBuf, 
		img.tmpBuf, 
		img.winWidth, 
		img.winWidth,
		id
		);

	//----//
	switch (id) 	   
	{
	case SPE_DISTORT_H_PINCH:	
		spePinch_punch(0x01);
		break;

	case SPE_DISTORT_H_PUNCH:
		spePinch_punch(0x00);
		break;

	case SPE_DISTORT_V_PINCH:
		spePinch_punch_V(0x01);
		break;

	case SPE_DISTORT_V_PUNCH:
		spePinch_punch_V(0x00);
		break;

	case SPE_DISTORT_H_STRETCH:
		speShrink_stretch(0x00);
		break;

	case SPE_DISTORT_H_SHRINK:
		speShrink_stretch(0x01);
		break;

	case SPE_DISTORT_V_STRETCH:
		speShrink_stretch_V(0x00);
		break;

	case SPE_DISTORT_V_SHRINK:
		speShrink_stretch_V(0x01);
		break;

	case SPE_DISTORT_H_HOURGLASS:
		speHourglass(0x01);
		break;

	case SPE_DISTORT_V_HOURGLASS:
		speHourglass_V(0x01);
		break;
	case SPE_DISTORT_WAVY:
		speWavy();
		break;

	case SPE_DISTORT_BUBBLE:
		speBubble(0x00);
		break;

	case SPE_DISTORT_BUBBLE_FILL:
		speBubble(0x01);
		break;
	case SPE_DISTORT_H_MIRROR:
		speMirror_H();
		break;
	case SPE_DISTORT_V_MIRROR:
		speMirror_V();
		break;		
		
	}

	HAL_CpuRamMapRcv();
	EXIT_CRITICAL(sts);
#if EN_PRINTF_TIME
	HAL_GlobalReadGTimer(&time2);
	printf("\n++  time: %lu  ++\n", time2-time1);
#endif	  
   	return SUCCESS;
}
#endif

void speDistRoiGet(
	UINT16 srcImgHSize,
	UINT16 srcImgVSize,
	UINT16 *hOff_, //for return roi
	UINT16 *vOff_, 
	UINT16 *hSize_, 
	UINT16 *vSize_,
	UINT16 orient 
)
{
	UINT16 hOff; 
	UINT16 vOff; 
	UINT16 hSize; 
	UINT16 vSize;
	UINT16 refW;
	UINT16 refH;

	hSize = g_speParams[SPE_PARAM_H_SIZE];
	vSize = g_speParams[SPE_PARAM_V_SIZE];
	hOff = g_speParams[SPE_PARAM_H_OFF];
	vOff = g_speParams[SPE_PARAM_V_OFF];
	refW = PB_DECODE_WIDTH;
	refH = PB_DECODE_HEIGHT;

	switch(orient){//change roi from view to decode orient
	case ROT_180:
		hOff = refW - hSize - hOff;
		vOff = refH - vSize - vOff;
		break;
	case ROT_270:
		vOff = refH - vSize - vOff;
		hSize = g_speParams[SPE_PARAM_V_SIZE];
		vSize = g_speParams[SPE_PARAM_H_SIZE];

		refW = PB_DECODE_HEIGHT;
		refH = PB_DECODE_WIDTH;
		break;
	case ROT_90:
		hOff = refW - hSize - hOff;
		hSize = g_speParams[SPE_PARAM_V_SIZE];
		vSize = g_speParams[SPE_PARAM_H_SIZE];

		refW = PB_DECODE_HEIGHT;
		refH = PB_DECODE_WIDTH;
		break;
	}	

	hSize = (UINT32)srcImgHSize * hSize / refW;
	vSize = (UINT32)srcImgVSize * vSize / refH;	
	hOff = (UINT32)srcImgHSize * hOff / refW;
	vOff = (UINT32)srcImgVSize * vOff / refH;

	if(hOff <= g_pb_rec_RotOut.hOff)
	{
		hOff=g_pb_rec_RotOut.hOff+1;
	}
	hOff=(hOff+1)&~1;
	if(vOff <= g_pb_rec_RotOut.vOff)
	{
		vOff=g_pb_rec_RotOut.vOff+1;
	}		
	vOff=(vOff+1)&~1;
	
	if(hOff + hSize > g_pb_rec_RotOut.hOff + g_pb_rec_RotOut.hSize - 2){
		hSize=g_pb_rec_RotOut.hOff + g_pb_rec_RotOut.hSize - hOff - 2;
	}
	if(vOff + vSize > g_pb_rec_RotOut.vOff + g_pb_rec_RotOut.vSize - 2){
		vSize=g_pb_rec_RotOut.vOff + g_pb_rec_RotOut.vSize - vOff - 2;
	}
	hSize=hSize & ~1;
	vSize=vSize & ~1;

	*hSize_=(UINT16)hSize;
	*vSize_=(UINT16)vSize;
	*hOff_=(UINT16)hOff;
	*vOff_=(UINT16)vOff;
	//printf("roi: %u,%u %u,%u, %u,%u\n", srcImgHSize, srcImgVSize, g_pb_rec_RotOut.hOff, g_pb_rec_RotOut.vOff, g_pb_rec_RotOut.hSize, g_pb_rec_RotOut.vSize);
	//printf(" %u,%u, %u,%u\n", hOff, vOff, hSize, vSize);
}


/**
 * @fn		  UINT8 speDistortView(UINT16 fileIndex, UINT8 speDisIdx)
 * @brief	  distort view, should after pb because some dec info is needed.
 * @param	  [in] fileIndex
 * @param	  [in] speDisIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 speDistortView(
	UINT8 speDisIdx 
) 
{
#if EN_SPE
	UINT8 ret = SUCCESS;

	UINT32 endAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 yDis;//=(UINT32)PB_DECODE_WIDTH*(UINT32)PB_DECODE_HEIGHT;//1024*320;
	UINT32 imgAddr;
	UINT32 tmpAddr;
	
	UINT16 decWidth, decHeight;
	UINT16 decMainW, decMainH; 
	
	UINT16 hSize;//=g_speParams[SPE_PARAM_H_SIZE];
	UINT16 vSize;//=g_speParams[SPE_PARAM_V_SIZE];
	UINT16 hOff;//=g_speParams[SPE_PARAM_H_OFF];
	UINT16 vOff;//=g_speParams[SPE_PARAM_V_OFF];
	UINT16 dMax;
	UINT16 shortSide;
	UINT8 fac;


	pbDecodeSizeGet(&decWidth, &decHeight); // buff size
	speDistRoiGet(decWidth, decHeight, &hOff, &vOff, &hSize, &vSize, ROT_0);
	
	if(speDisIdx==SPE_DISTORT_BUBBLE || speDisIdx==SPE_DISTORT_BUBBLE_FILL)	
	{//estimate valid radius when save file
		decMainW = 0;
		decMainH = 0;
		fac = pbCalImageScalFac4PhotoFrame(endAddr - startAddr - (vfsFileSizeGet(0)>>1), // after pb, same with  speDistortSave()
			0, 0, &decMainW, &decMainH);
		if(fac == -1){
			return FAIL;
		}

		shortSide = decWidth;
		if(shortSide > decHeight){
			shortSide = decHeight;
		}
		if(decMainW > decMainH){
			decMainW = decMainH;
		}
		
		if((UINT16)SPE_DISTORT_BUBBLE_RADIUS_MAX*2 < decMainW){
			dMax = (UINT16)SPE_DISTORT_BUBBLE_RADIUS_MAX*2;
			dMax = (UINT32)dMax * shortSide / decMainW;
		}
		else{
			dMax = shortSide;
		}
	
		if(hSize>dMax || vSize>dMax)
		{
			UINT16 cx,cy;
			cx=hOff+(hSize>>1);
			cy=vOff+(vSize>>1);
			hSize=dMax;
			vSize=dMax;
			hOff=cx-(dMax>>1);
			vOff=cy-(dMax>>1);
			//printf("hSize:%u",hSize);
		}
	}
	
	yDis = (UINT32)decWidth * decHeight; //xian 20120926
	//addr	
	OrderToAddr( startAddr, 16 );
	imgAddr = startAddr + yDis;
	OrderToAddr( imgAddr, 16 );
	tmpAddr = imgAddr +yDis;
	OrderToAddr( tmpAddr, 16 );
	
	HAL_GprmDramDma(pbDispBuffGet( 1 )<<1, imgAddr<<1, (UINT32)yDis<<1, 0x01);
	speDistort(
		speDisIdx, 
		imgAddr,
		decWidth, decHeight,
		hOff, vOff, hSize, vSize,
		tmpAddr);
	
	HAL_GprmDramDma(imgAddr<<1,pbDispBuffGet( 1 )<<1,  (UINT32)yDis<<1, 0x01);
	dispShow(pbDispBuffGet(1), decWidth, decHeight, 100); 

	return SUCCESS;
#else
	speDisIdx = speDisIdx;
	return FAIL;
#endif
}

/**
 * @fn		  UINT8 speDistortSave(UINT16 fileIndex, UINT8 speDisIdx)
 * @brief	  save distort
 * @param	  [in] fileIndex
 * @param	  [in] speDisIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT8 speDistortSave( 
	UINT16 fileIndex, 
	UINT8 speDisIdx 
	) 
{ 
#if EN_SPE
	UINT8 ret = SUCCESS;
	
	UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 yDis=0;//(UINT32)PB_DECODE_WIDTH*(UINT32)PB_DECODE_HEIGHT;//1024*320;
	UINT32 imgAddr;
	UINT32 tmpAddr;
	UINT32 endAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA

	UINT16 decW=0, decH=0;
	UINT16 imgW=0, imgH=0;

	UINT16 hSize,vSize,hOff,vOff;
	UINT8 decOpt = PB_DECODE_OPT_FULL;
	UINT8 decAlignSave;
	
	OrderToAddr( startAddr, 16 );
	imgAddr = startAddr + yDis;
	OrderToAddr( imgAddr, 16 );
	endAddr = (startAddr + endAddr)>>1; //xian : not used in pbDecodeImage()
	scaleChunk = 0x20;

	decAlignSave = g_pbDecAlignOpt;
	g_pbDecAlignOpt = 1; // dec to top_left
	ret=pbDecodeImage( 
				fileIndex, 
				&imgAddr, 
				endAddr, 
				decOpt, 
				&imgW, 
				&imgH, 
				&decW, 
				&decH
				);
	g_pbDecAlignOpt = decAlignSave; // dec to top_left	
	if(ret)
	{
		//printf("++decode err++\n");
		return ret;
	}

	if(g_speParams[SPE_PARAM_H_SIZE_DST] 
		&& g_speParams[SPE_PARAM_H_SIZE_DST]<decW
		&& g_speParams[SPE_PARAM_V_SIZE_DST]<=1024
		)
	{
		imgW=g_speParams[SPE_PARAM_H_SIZE_DST];
		imgH=g_speParams[SPE_PARAM_V_SIZE_DST];
		//printf("imgW:%d,h:%d\n",imgW,imgH);
		HAL_GprmScale( imgAddr, decW, decH, 
			imgAddr, imgW, imgH, 
			imgAddr, 1 
			);
		decW=imgW;
		decH=imgH;
	}

	decOpt=pbParamGet(PB_PARAM_EXIF_ORIENT);
	//printf("rot:%u\n",(UINT16)decOpt);
				
	//printf("dec:%u,%u, %u,%u\n", decW, decH, imgW, imgH);	
	speDistRoiGet(decW, decH, &hOff, &vOff, &hSize, &vSize, decOpt);

	//printf("ID:%u\n",(UINT16)speDisIdx);
	//printf("off:(%u,%u);size:(%u,%u)\n",hOff,vOff,hSize,vSize);
	switch(decOpt)
	{
	case ROT_90:		
	case ROT_270:
		if(speDisIdx>SPE_DISTORT_H_ && speDisIdx<SPE_DISTORT_V_)
			speDisIdx+=SPE_DISTORT_V_;
		else if(speDisIdx>SPE_DISTORT_V_ && speDisIdx<SPE_DISTORT_M_)
			speDisIdx-=SPE_DISTORT_V_;

		break;
	}
	//printf("ID:%u\n",(UINT16)speDisIdx);
	//printf("off:(%u,%u);size:(%u,%u)\n",hOff,vOff,hSize,vSize);

	tmpAddr=imgAddr + (UINT32)decW*(UINT32)decH;
	OrderToAddr( tmpAddr, 16 );
	dbgWdtSet(0);
	ret=speDistort(
			speDisIdx, 
			imgAddr,
			decW, 
			decH,
			hOff, 
			vOff, 
			hSize, 
			vSize,
			tmpAddr);
	dbgWdtSet(1);
	
	if(ret==FAIL)
	{
		return ret;
	}
	else
	{
		if(imgScaleFac){
			decW = imgW*imgScaleFac>>3;
			decH = imgH*imgScaleFac>>3;
		}
		else{
			decW = imgW;
			decH = imgH;
		}
		//encode

		pbEncodeImage( 
			imgAddr, 
			decW, 
			decH, 
			imgW, 
			imgH
			);
	

	}
	
	return ret;	
#else
	fileIndex = fileIndex;
	speDisIdx = speDisIdx;
	return FAIL;
#endif
}
#if EN_SPE
void speBuffSizeSet(UINT32 size)
{
	speBuffSize=size;
}

UINT32 speBuffSizeGet(void)
{
	return speBuffSize;
}

#endif

