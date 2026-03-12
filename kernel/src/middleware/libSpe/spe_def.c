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
 * @file	  spe_def.c
 * @brief	  base function for spe
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "spe_def.h"
#include "sp1k_rsvblk_api.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "hal.h"
#if EN_SPE
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
SPE_IMG_DESC img _at_ 0x7600;
MAP_BANK map _at_ 0x7400;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
/**
 * @fn		  void speLoadRes2Dram(UINT8 id,UINT32 distAddr)
 * @brief	  speLoadRes2Dram
 * @param	  [in] id   : speDistortID_t 
 * @param	  [in] tmpAddr: tmp DRAM address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2011-4-26
 * @todo	  some thing need to be done
 * @bug 	  a bug need to be done
*/
void speLoadRes2Sram(UINT8 id,UINT32 tmpAddr){
	UINT8 resID;
	switch(id){	
		case SPE_DISTORT_V_HOURGLASS:
		case SPE_DISTORT_H_HOURGLASS:
			resID = 0x34;
			break;
		case SPE_DISTORT_WAVY:
			resID = 0x35;
			break;
		case SPE_DISTORT_BUBBLE:
		case SPE_DISTORT_BUBBLE_FILL:
			resID = 0x36;
			break;
		default:   //no need to load resouce file 
			resID = 0xff;
			// SPE_DISTORT_H_PINCH:				
			// SPE_DISTORT_H_PUNCH:
			// SPE_DISTORT_V_PINCH:
			// SPE_DISTORT_V_PUNCH:
			// SPE_DISTORT_H_STRETCH:
			// SPE_DISTORT_H_SHRINK:
			// SPE_DISTORT_V_STRETCH:
			// SPE_DISTORT_V_SHRINK:
			break;
	}
	if(resID!=0xff){
		sp1kNandRsvRead((UINT32)resID, tmpAddr);

		//xian 20120926 move to G_ucStorData, 
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(tmpAddr/2);
		HAL_DmaDo(0, 1, 1024, 0, 0);	
	}
}
/**
 * @fn		  void speWriteToMap(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len, ,UINT32 mapAddrWord)
 * @brief	  speWriteToMap
 * @param	  [in] DramAddrWord
 * @param	  [in] imgWidth
 * @param	  [in] vSize
 * @param	  [in] len
 * @param	  [in] mapAddrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speWriteToMap(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len,UINT32 mapAddrWord )
{
	while(vSize--){
		HAL_GprmDramDma((UINT32)DramAddrWord, (UINT32)mapAddrWord, (UINT32)len, 1);
		DramAddrWord+=(UINT32)imgWidth;
		mapAddrWord+=(UINT32)len;
	}
}
/**
 * @fn		  void speWriteToMap_Ex(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len, ,UINT32 mapAddrWord)
 * @brief	  speWriteToMap_Ex
 * @param	  [in] DramAddrWord
 * @param	  [in] imgWidth
 * @param	  [in] vSize
 * @param	  [in] len
 * @param	  [in] mapAddrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speWriteToMap_Ex(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len,UINT32 mapAddrWord )
{
	while(vSize--){
		HAL_GprmDramDma((UINT32)DramAddrWord, (UINT32)mapAddrWord, (UINT32)len, 1);
		DramAddrWord-=(UINT32)imgWidth;
		mapAddrWord+=(UINT32)len;
	}
}
/**
 * @fn		  void speReadFromMap(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len, ,UINT32 mapAddrWord)
 * @brief	  speReadFromMap
 * @param	  [in] DramAddrWord
 * @param	  [in] imgWidth
 * @param	  [in] vSize
 * @param	  [in] len
 * @param	  [in] mapAddrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speReadFromMap(UINT32 DramAddrWord, UINT16 imgWidth, UINT16 vSize, UINT16 len,UINT32 mapAddrWord )
{
	while(vSize--){
		HAL_GprmDramDma((UINT32)mapAddrWord, (UINT32)DramAddrWord, (UINT32)len, 1);
		DramAddrWord+=(UINT32)imgWidth;
		mapAddrWord+=(UINT32)len;
	}
}

//load res table to XDATA_BUF_ADDR16
/**
 * @fn		  UINT8 speLoadEffectTable(UINT32 addr32Word, addr32Word,UINT8 resID)
 * @brief	  speLoadEffectTable
 * @param	  [in] addr32Word
 * @param	  [in] resID
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 xdata* speLoadEffectTable(UINT32 addr32Word,UINT8 resID)
{
#if EN_PRINTF_NAME
	printf("loadResTable\n");
#endif
	resID = resID;  //clear warning
	addr32Word = addr32Word; //clear warnning.
	return (UINT8 xdata*)G_ucStorData ;
}

/**
 * @fn		  void speSetStructImg(UINT32 srcAddrWord, UINT16 srcHsize, UINT16 srcVsize, UINT16 hOffset, UINT16 vOffset, UINT16 hSize, UINT16 vSize, UINT32 tmpAddrWord)
 * @brief	  speSetStructImg
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
void speSetStructImg(
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
	img.srcBuf=srcAddrWord*2;
	img.tmpBuf=tmpAddrWord*2;

	img.winHoff=(hOffset & (~0x0007))*2;
	img.winVoff=vOffset;
	
	img.hSize=hSize;
	img.vSize=vSize;
	img.srcVsize=srcVsize;
	
	img.hStart=hOffset&0x0007;
	img.hEnd=img.hStart+img.hSize;
	
	img.imgWidth=srcHsize<<1;
	img.winWidth=((img.hStart<<1) +(hSize<<1) +15) &0xfff0;

	img.winBuf=img.srcBuf + img.winHoff+(UINT32)img.imgWidth*(UINT32)vOffset;
}

/**
 * @fn		  void speSetStructMap(UINT32 imgAddrWord, UINT32 mapAddrWord, UINT16 mapWidth, UINT16 mapBlkSize,UINT8 id)
 * @brief	  speSetStructMap
 * @param	  [in] imgAddrWord
 * @param	  [in] mapAddrWord
 * @param	  [in] mapWidth
 * @param	  [in] mapBlkSize
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speSetStructMap	(
	UINT32 imgAddrWord,
	UINT32 mapAddrWord,
	UINT16 mapWidth,
	UINT16 mapBlkSize,
	UINT8 id
)
{
	UINT16 mapBank;
	map.ramMapBank=0x8000;
	map.mapWidth=mapWidth;//
	map.mapBlkSize=mapBlkSize;
	
	map.dataSrcAddr=imgAddrWord;
	map.dataDstAddr=imgAddrWord;
	map.mapSrcAddr=	(mapAddrWord+(UINT32)(map.ramMapBank-1))&(~(UINT32)(map.ramMapBank-1));
	map.mapDstAddr=	map.mapSrcAddr+(UINT32)map.mapBlkSize;

	mapBank=(UINT16)(map.mapSrcAddr>>15);
	
	speLoadRes2Sram(id,map.mapSrcAddr);

	HAL_CpuRamMapSet(map.mapSrcAddr>>1, 32UL*1024UL, (void xdata* xdata*)&map.pSrcData);
	map.pDstData=(UINT8 xdata*)map.pSrcData+map.mapBlkSize;//
}
#endif
