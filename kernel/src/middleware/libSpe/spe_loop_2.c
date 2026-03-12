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
 * @file	  spe_loop_2.c
 * @brief	  wavy
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/
#include "spe_loop_2.h"
#include "hal_gprm.h"
#if EN_SPE
#pragma OT(8,SPEED)
/**
 * @fn		  void speWavy(void)
 * @brief	  Wavy
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speWavy()
{
#if EN_SPE_WAVY
	//load table
	DATA UINT16 xdata* table;

	DATA UINT16 y;

	//--DATA DEF--//
	DATA UINT8 wavecntX=16*4;
	DATA UINT8 wavecntY=12*4;
	
	DATA UINT16 srcX;
	DATA UINT8 srcY;
	UINT8 _pos=4;
	UINT8 _count=9;
	
	DATA UINT16 x;
	
	DATA UINT16 wPos;
	DATA UINT16 rPos;
	DATA UINT8 t1;
	DATA UINT8 t0;
	DATA UINT8 xdata* rPointer;
	UINT8 xdata* DATA pSrcData;
	UINT8 xdata* DATA pDstData;
	UINT8 maxV = 0;
	DATA UINT16 mapWidth=map.mapWidth;
	

	UINT32 scale;
	UINT32 ss;
	/////////////////////////////
#if EN_PRINTF_NAME
	printf("++	Wavy  ++\n");
#endif
	
	
	//wavecntX=(UINT16)16*img.hSize/1280ul;
	//wavecntY=(UINT16)12*img.vSize/960;
	
	wavecntX=((UINT16)wavecntX*128)/img.hSize;
	wavecntY=((UINT16)wavecntY*128)/img.vSize;
#if EN_PRINTF
	printf("wavecntX=%d,wavecntX=%d\n",(UINT16)wavecntX,(UINT16)wavecntY);
#endif	
	map.mapBlkSize=map.mapWidth*_count;
	map.pDstData=(UINT8 xdata*)map.ramMapBank+map.mapBlkSize;

	map.mapDstAddr=	map.mapSrcAddr+(UINT32)map.mapBlkSize;
		
	//--DATA--//
	table=(UINT16 xdata*)speLoadEffectTable(map.mapSrcAddr, SPE_TABLE_ID_WAVY);
	//printf("scale:%lu\n",scale);

	scale=(UINT32)img.srcVsize*65536/960ul;
	for(y=0;y<256;y++)
	{
		ss=(UINT32)table[y]*scale;
		table[y]=READ16_(ss);
		if(maxV<READ8(table[y],0))maxV=READ8(table[y],0);
	}
	ss=_count*scale;
	_count=READ16_(ss);
	_count=(_count<3)?3:_count;
	if(_count <= maxV){ //xian ++, 20090820, #38801
		_count = maxV + 1;		  
	}
	_count |=0x01;
	_pos=_count>>1;
	
	speWriteToMap(map.dataSrcAddr,img.imgWidth,_count-1,map.mapWidth,map.mapSrcAddr+map.mapWidth);
	map.dataDstAddr+=img.imgWidth*_pos;
	map.dataSrcAddr+=img.imgWidth*(_count-1);
	
	//--//

	//printf("c:%u,p:%u\n",(UINT16)_count,(UINT16)_pos);
	rPointer=map.pSrcData;
	pDstData=map.pDstData;
	//--Y LOOP--//
	for(y=_pos;y<img.vSize-_pos;y++)
	{
		//--DMA TO MAP--//
		HAL_GprmDramDma(map.mapSrcAddr+map.mapWidth, map.mapSrcAddr, map.mapWidth*(_count-1), 1);
		HAL_GprmDramDma(map.dataSrcAddr, map.mapSrcAddr+map.mapWidth*(_count-1), map.mapWidth, 1);
		
		HAL_GprmDramDma(map.mapSrcAddr+map.mapWidth*_pos, map.mapDstAddr, map.mapWidth, 1);
		
		//--DATA--//
		
		//--X LOOP--//
		for(x=img.hStart+_pos;x<=img.hEnd-_pos;++x)
		{
			//--DATA--//
			srcX=x-_pos+READ8(table[(UINT8)((UINT8)y*wavecntX)],0);//*wavemovelenX/65536;
			srcY=READ8(table[(UINT8)((UINT8)x*wavecntY)],0);
			//--IMAGE DATA--//
			//--IMAGE DATA--//
		
			//--DATA--//
			rPos=srcX;
			wPos = x;
			
			t0=(UINT8)(rPos)& 0x07;
			rPos = ((rPos)& 0xfff8)<< 0x01;
				
			t1=(UINT8)wPos& 0x07;
			wPos =((wPos) & 0xfff8) << 0x01;

			pSrcData=rPointer+srcY*mapWidth;
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
		HAL_GprmDramDma(map.mapDstAddr, map.dataDstAddr, map.mapWidth, 1);
		//--MAP ADDRESS--//
		map.dataSrcAddr+=img.imgWidth;
		map.dataDstAddr+=img.imgWidth;
	}

#endif	  
}
#endif