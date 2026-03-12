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
 * @file	  spe_loop_1v.c
 * @brief	  vertical loop 1
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/

#include "spe_loop_1.h"
#include "hal_gprm.h"
#if EN_SPE
#pragma OT(8,SPEED)
/**
 * @fn		  void speSimpleV(UINT8 ID, ID,UINT8 flag)
 * @brief	  Vertical simple
 * @param	  [in] ID
 * @param	  [in] flag
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speSimpleV(UINT8 ID,UINT8 flag)
{
	UINT16 xdata *table=(UINT16 xdata*)(map.pDstData+map.mapWidth);
	#if EN_PRINTF_NAME
	//printf("speSimpleV ID:%u\n",(UINT16)ID);
	#endif
	HAL_GprmRot(
			4, // fill black at right down ward. so just exchang x and y, otherwise offset may be error
			img.srcBuf>>1, 
			img.tmpBuf>>1, 
			img.imgWidth>>1, 
			img.srcVsize, 
			1);
	#if EN_PRINTF
	printf("\nimg: 0x%lx\n\n 0x%lx\n 0x%lx\n %u,%u\n %u\n %u,%u\n %u,%u\n %u,%u\n \n",
		img.srcBuf,
		img.winBuf,
		img.tmpBuf,
		img.imgWidth,img.srcVsize,
		img.winWidth,
		img.winHoff,img.winVoff,
		img.hSize,img.vSize,
		img.hStart,img.hEnd
		);
	#endif
	speSetStructImg
		(
		img.tmpBuf>>1, 
		
		img.srcVsize,//UINT16 srcHsize, 
		img.imgWidth>>1,//UINT16 srcVsize, 
		
		img.winVoff,//UINT16 hOffset, 
		(img.winHoff+img.hStart)>>1,//UINT16 vOffset, 

		img.vSize,//UINT16 hSize, 
		img.hSize,//UINT16 vSize, 
		
		img.srcBuf>>1
		);
	#if EN_PRINTF
	printf("\nimg: 0x%lx\n\n 0x%lx\n 0x%lx\n %u,%u\n &u\n %u,%u\n %u,%u\n %u,%u\n \n",
		img.srcBuf,
		img.winBuf,
		img.tmpBuf,
		img.imgWidth,img.srcVsize,
		img.winWidth,
		img.winHoff,img.winVoff,
		img.hSize,img.vSize,
		img.hStart,img.hEnd
		);
	#endif
	speSetStructMap
		(
		img.winBuf,//UINT32 imgAddr,
		img.tmpBuf,//UINT32 mapAddr,
		img.winWidth,//UINT16 mapWidth,
		img.winWidth,//UINT16 mapBlkSize
		0xff //ID, xian 20120926, dont read res again
		);
	speSimple(ID, flag);
	HAL_GprmRot(
			4, 
			img.srcBuf>>1, 
			img.tmpBuf>>1, 
			img.imgWidth>>1, 
			img.srcVsize, 
			1);
}
#endif
