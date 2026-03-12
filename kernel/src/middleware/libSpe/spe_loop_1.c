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
 * @file	  spe_loop_1.c
 * @brief	  special effect loop 1
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/
#include "spe_loop_1.h"
#include "spe_distort.h"
#include "hal_gprm.h"
#include "cdsp_task.h"
#if EN_SPE
#pragma OT(8,SPEED)
/**
 * @fn		  void speSimple(UINT8 ID, ID,UINT8 flag)
 * @brief	  speSimple
 * @param	  [in] ID
 * @param	  [in] flag
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speSimple(UINT8 ID,UINT8 flag)
{
	UINT16 xdata *table=(UINT16 xdata*)(map.pDstData+map.mapWidth);
	switch(ID)
	{
		case SPE_DISTORT_H_PINCH:
			speCreatPinchTable(img.vSize,table);
			break;
		case SPE_DISTORT_H_SHRINK:
			{
			UINT16 i=img.vSize;
			while(i--)
				table[i]=32768;
			}
			break;
		case SPE_DISTORT_H_HOURGLASS:
			speCreatHourglass(img.vSize,table,map.mapSrcAddr);
			break;
		case SPE_DISTORT_H_MIRROR:
			speMirror();
			return;
	}
	speH_Loop_1(
		map.dataSrcAddr,
		img.hSize, img.vSize,
		img.imgWidth,
		map.mapSrcAddr,map.mapDstAddr, 
		map.mapWidth,
		img.hStart,img.hEnd,
		table,
		map.pSrcData,
		map.pDstData,
		flag
		);
}

/**
 * @fn		  void speCreatPinchTable(DATA vSize, DATA table)
 * @brief	  Creat Pinch Table
 * @param	  [in] vSize
 * @param	  [in] table
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speCreatPinchTable(
	UINT16 DATA vSize,
	UINT16 xdata* DATA table
)
{
	UINT32 DATA vInc = 0;
	UINT32 DATA vStep = 0x20000/vSize;
	UINT16 DATA y;
	
	//--Y LOOP--//
	y=(vSize+1)>>1;
	table[y+1]=0;
	table[y]=0;
	table[y-1]=0;
	while(y--)
	{
		vInc+=vStep;
		table[y]=vInc;
		table[vSize-1-y]=vInc;
	}
}

/**
 * @fn		  void speCreatHourglass(UINT16 vSize, xdata *DATAtable, UINT32 tmpAddr)
 * @brief	  Creat Hourglass table
 * @param	  [in] vSize
 * @param	  [in] table
 * @param	  [in] tmpAddrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speCreatHourglass(
	UINT16 vSize,
	UINT16 xdata *DATA table,
	UINT32 tmpAddrWord
)
{
	UINT16 DATA y=(vSize+1)>>1;
	UINT16 xdata *DATA tableRes=(UINT16 xdata*)speLoadEffectTable(tmpAddrWord, SPE_TABLE_ID_HOURGLASS);

	UINT32 DATA inc=-(((UINT32)120<<16)/(UINT32)y);
	UINT32 DATA add=(UINT32)120<<16;
	
	table[y]=tableRes[120];
	table[y+1]=tableRes[120];
	table[y-1]=tableRes[120];
	//printf("++ speCreatHourglass	++\n");
	//printf("inc=0x%lx,add=0x%lx\n",inc,add);
	while(y--)
	{
		add+=inc;//index=(UINT32)y*240/vSize;
		table[y]=tableRes[READ16_(add)];
		table[vSize-y]=tableRes[READ16_(add)];
		//printf("%u||%u\n",y,READ16_(add));
	}
	table[0]=65535;
	table[vSize]=65535;
}

/**
 * @fn		  void speH_Loop_1(UINT32 dataAddrWord, UINT16 hSize, UINT16 vSize, UINT16 imgWidth, UINT32 mapSrcAddrWord, mapSrcAddrWord,UINT32 mapDstAddrWord, UINT16 mapWidth, UINT16 hStart, hStart,UINT16 hEnd, xdata* incTable, DATA pSrcData, DATA pDstData, UINT8 flag)
 * @brief	  H_Loop_1
 * @param	  [in] dataAddrWord
 * @param	  [in] hSize
 * @param	  [in] vSize
 * @param	  [in] imgWidth
 * @param	  [in] mapSrcAddrWord
 * @param	  [in] mapDstAddrWord
 * @param	  [in] mapWidth
 * @param	  [in] hStart
 * @param	  [in] hEnd
 * @param	  [in] incTable
 * @param	  [in] pSrcData
 * @param	  [in] pDstData
 * @param	  [in] flag
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speH_Loop_1(
	UINT32 dataAddrWord,
	UINT16 hSize, UINT16 vSize,
	UINT16 imgWidth,
	UINT32 mapSrcAddrWord,UINT32 mapDstAddrWord, 
	UINT16 mapWidth,
	UINT16 hStart,UINT16 hEnd,
	UINT16 xdata*  incTable,
	UINT8 xdata* DATA pSrcData,
	UINT8 xdata* DATA pDstData,
	UINT8  flag
)
{
	
	UINT16 y ;

	DATA UINT16 x;
	
	DATA UINT16 wPos;
	DATA UINT16 rPos;
	DATA UINT8 t1;
	DATA UINT8 t0;

	DATA UINT32 inc;
	DATA UINT32 step;
	DATA UINT32 add;
	DATA UINT16 add16;
	DATA UINT16 hMiddle=((hStart+hEnd-1)>>1);
	DATA UINT16 hAll=hStart+hEnd-1;//last pos for cal right pos
	
	for(y=0;y<vSize;++y)
	{
		//--DMA TO MAP--//
		HAL_GprmDramDma(dataAddrWord, mapSrcAddrWord, mapWidth, 0x01);
		HAL_GprmDramDma(dataAddrWord, mapDstAddrWord, mapWidth, 0x01);

		add=(UINT32)hMiddle<<16;//hStart<<16;
		inc=(UINT32)(incTable[y]);
		step=((0x10000-inc)<<2)/hSize;
		if(flag)
		{
			inc=0x20000 - inc;
			step=-step;
		}
		inc=-inc;
		step=-step;
		x=hMiddle;
		while(x -- > hStart)
		{
			inc+=step;
			add+=inc;
			add16=READ16_(add);

			add16=(add16>hMiddle)?hMiddle:add16;
			add16=(add16<hStart)?hStart:add16;

			//left
			rPos=add16;
			wPos = x;
			
			t0=(UINT8)(rPos)& 0x07;
			rPos = ((rPos)& 0xfff8)<< 0x01;
				
			t1=(UINT8)wPos& 0x07;
			wPos =((wPos) & 0xfff8) << 0x01;

			//--IMAGE DATA--//
			pDstData[wPos|(UINT16)t1] = pSrcData[rPos|(UINT16)t0];
			if(!(t1 & 0x01))//UV
			{
				t1=(t1>>0x01)|0x08;
				t0=(t0>>0x01)|0x08;
				pDstData[wPos|(UINT16)t1]=pSrcData[rPos|(UINT16)t0];
				pDstData[wPos|(UINT16)(t1|0x04)]=pSrcData[rPos|(UINT16)(t0|0x04)];
			}

			//right
			rPos=hAll-add16;
			wPos = hAll-x;
			
			t0=(UINT8)(rPos)& 0x07;
			rPos = ((rPos)& 0xfff8)<< 0x01;
				
			t1=(UINT8)wPos& 0x07;
			wPos =((wPos) & 0xfff8) << 0x01;

			//--IMAGE DATA--//
			pDstData[wPos|(UINT16)t1] = pSrcData[rPos|(UINT16)t0];
			if(!(t1 & 0x01))//UV
			{
				t1=(t1>>0x01)|0x08;
				t0=(t0>>0x01)|0x08;
				pDstData[wPos|(UINT16)t1]=pSrcData[rPos|(UINT16)t0];
				pDstData[wPos|(UINT16)(t1|0x04)]=pSrcData[rPos|(UINT16)(t0|0x04)];
			}
		}
	
		//--DMA FROM MAP--//
		HAL_GprmDramDma(mapDstAddrWord, dataAddrWord, mapWidth, 0x01);
		//--MAP ADDRESS--//
		dataAddrWord+=imgWidth;
		
	}

}



/**
 * @fn		  void speMirror(void)
 * @brief	  mirror effect
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speMirror()
{
#if EN_PRINTF_NAME
printf("speMirror\n");
#endif
HAL_GprmRot(
			2, 
			img.srcBuf>>1,
			(img.srcBuf+(UINT32)img.imgWidth*(img.srcVsize>>1))>>1,
			img.imgWidth>>1, 
			img.srcVsize>>1, 
			1);

cdspBlendEffectSet(// img.tmpBuf>>1, 
				(img.srcBuf+(UINT32)img.imgWidth*(img.srcVsize>>1))>>1,
				(img.srcBuf+(UINT32)img.imgWidth*(img.srcVsize>>1))>>1, 
				img.imgWidth>>1, 
				img.srcVsize>>1,
				(UINT8)speParamGet(SPE_PARAM_MIRROR_INDEX));

}
#endif
