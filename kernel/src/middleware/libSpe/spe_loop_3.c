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
 * @file	  spe_loop_3.c
 * @brief	  bubble function
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/
#include "spe_loop_3.h"
#include "sp1k_rsvblk_api.h"
#include "hal_gprm.h"
#if EN_SPE
#pragma OT(8,SPEED)

/**
 * @fn		  void speBubble(UINT8 isFillBlack)
 * @brief	  speBubble
 * @param	  [in] isFillBlack
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speBubble(UINT8 isFillBlack)
{
#if EN_SPE_BUBBLE
	//--DATA DEF--//
	UINT16 radius;
	UINT16 rMax=SPE_DISTORT_BUBBLE_RADIUS_MAX;
	UINT16 cx,cy;

	//--DATA--//
	#if EN_PRINTF_NAME
	printf("++	Bubble (isFill=%u) ++\n",(UINT16)isFillBlack);
	#endif
	radius=(img.hSize<img.vSize)?(img.hSize>>1):(img.vSize>>1);
	radius&=~0x07;
	
	if(radius>rMax)
		radius=rMax;
	else if(radius <16)
	{
		if(isFillBlack)
			gprmBlackFill(img.srcBuf>>1,img.imgWidth>>1,img.srcVsize);
		return;
	}
	printf("radius=%d\n",(UINT16)radius);
	cx=(img.winHoff+img.hStart+img.hSize)&(0xfff0);
	cy=(img.winVoff+(img.vSize>>1));
	img.hSize=radius+radius;
	img.vSize=img.hSize;
	img.winHoff=cx-img.hSize;
	img.winVoff=cy-radius;
		
	img.winBuf=img.srcBuf+(UINT32)img.imgWidth*(UINT32)img.winVoff+(UINT32)img.winHoff;
	map.dataSrcAddr=img.winBuf;
	map.dataDstAddr=map.dataSrcAddr;
	img.winWidth=img.hSize+img.hSize;

	map.mapWidth=radius+radius;
	map.mapBlkSize=0x8000-map.mapWidth;//map.mapWidth*radius;//0x4000;
	map.pDstData=map.pSrcData+map.mapBlkSize;
	map.mapDstAddr=	map.mapSrcAddr+(UINT32)map.mapBlkSize;
	
	//--//
	speCreatBubbleTable(radius,SPE_TABLE_ID_BUBBLE);
	if(isFillBlack)
	{
		speBubFillBlack();
		gprmBlackFill(map.mapDstAddr/2,radius,2);
	}

#if EN_PRINTF
	printf("img:0x%lx,0x%lx,%u,%u\n",img.srcBuf,img.winBuf,img.winWidth,img.imgWidth);
	printf("map:0x%lx,0x%lx\nmap:0x%lx,0x%lx\nmap:0x%x,0x%x\nmap:%u,%u\n",
			map.dataSrcAddr,map.dataDstAddr,map.mapSrcAddr,map.mapDstAddr,
			map.pSrcData,map.pDstData,
			map.mapBlkSize,map.mapWidth);
#endif
	//--1--
	speDoBubble1_4(map.dataSrcAddr
				,radius,1,isFillBlack);
	//--2--
	speDoBubble1_4(map.dataSrcAddr+(UINT32)map.mapWidth
				,radius,2,isFillBlack);
	//--3--
	speDoBubble1_4(map.dataSrcAddr+(UINT32)(radius +radius-1)*(UINT32)img.imgWidth
				,radius,3,isFillBlack);
	//--4--
	speDoBubble1_4(map.dataSrcAddr+(UINT32)(radius +radius-1)*(UINT32)img.imgWidth+(UINT32)map.mapWidth
				,radius,4,isFillBlack);
	/////////////////////////////
#endif	  
}


#if EN_SPE_BUBBLE
/**
 * @fn		  void speCreatBubbleTable(UINT16 radius, radius,UINT32 resID)
 * @brief	  speCreatBubbleTable
 * @param	  [in] radius
 * @param	  [in] resID
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speCreatBubbleTable(UINT16 radius,UINT32 resID)
{
	UINT16 k=SPE_TABLE_LEN_BUBBLE*256/radius;
	
	UINT16 i,index;
	UINT16 r_14=radius>>2;

	UINT16 xdata* t2=(UINT16 xdata*)G_ucStorData;
	UINT8 xdata* t1=G_ucStorData+radius+radius+2 ;
	
	UINT8 xdata* table=map.pSrcData ;

	resID = resID;
#if EN_PRINTF_NAME
	printf("++ speCreatBubbleTable ++");
#endif
	
	//sp1kNandRsvRead((UINT32)resID, map.mapSrcAddr);
	table[0]=1;
	for(i=0;i<radius;i++)
	{
		index=i*k;
		t1[i]=256-(UINT16)table[READ8(index,0)]*r_14/i;
		t2[i]=i*i;
	}
	t1[0]=0;
	t1[radius]=255;
	t2[radius]=65535;
}

/**
 * @fn		  void speDoBubble1_4(UINT32 srcAddrWord, srcAddrWord,UINT16 radius, radius,UINT8 flag, flag,UINT8 isFillBlack)
 * @brief	  speDoBubble1_4
 * @param	  [in] srcAddrWord
 * @param	  [in] radius
 * @param	  [in] flag
 * @param	  [in] isFillBlack
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speDoBubble1_4(UINT32 srcAddrWord,UINT16 radius,UINT8 flag,UINT8 isFillBlack)
{
	UINT16 count_=(float)radius/3.9+1;

	UINT16 x;//,y;

	DATA UINT16 srcX,srcY;
	DATA UINT16 absX,absY;

	UINT16 xdata* table;
	UINT8 xdata* bubbleTable;
	UINT16 value;
	UINT16 index;

	UINT16 Y_2;
	UINT16 X_2;
	UINT16 R_2;
	UINT16 r_2;
	
	DATA UINT16 wPos;
	DATA UINT16 rPos;
	DATA UINT8 t1;
	DATA UINT8 t0;
	DATA UINT8 xdata* rPointer;
	UINT8 xdata* DATA pSrcData;
	UINT8 xdata* DATA pDstData;
	UINT16 mapWidth=map.mapWidth;

	UINT32 copyDstAddr;
	
	UINT32 dstAddr;
	
	UINT16 len;
	UINT32 mapAddr1;
	UINT32 mapAddr2;
	
	UINT8 flagX,flagY;

	//r scale
	R_2=radius*radius;
	
	//table
	bubbleTable=G_ucStorData+radius+radius+2 ;
	table=(UINT16 xdata*)G_ucStorData;

	//buf
	len=map.mapWidth*(count_-1);
	mapAddr1=map.mapSrcAddr+(UINT32)map.mapWidth;
	mapAddr2=map.mapSrcAddr+(UINT32)len;
	
	if(isFillBlack)
		copyDstAddr=map.mapDstAddr+(UINT32)map.mapWidth;
	else
		copyDstAddr=map.mapSrcAddr;
	
	dstAddr=srcAddrWord;
	//printf("addr:0x%lx,0x%lx\n",srcAddr,dstAddr);	
	if(flag==1||flag==3)flagX=1;//вС
	else flagX=0;//ср

	if(flag==1||flag==2)flagY=1;//ио
	else flagY=0;//об
	
	
	if(flagY)//+y
	{
		//map
		speWriteToMap(srcAddrWord, img.imgWidth, count_, map.mapWidth, map.mapSrcAddr);
		srcAddrWord+=(UINT32)img.imgWidth*(UINT32)count_;//+y	
	}
	else
	{
		//map
		speWriteToMap_Ex(srcAddrWord, img.imgWidth, count_,map.mapWidth, map.mapSrcAddr);
		srcAddrWord-=(UINT32)img.imgWidth*(UINT32)count_;//+y
	}
	
	rPointer=map.pSrcData;
	pDstData=map.pDstData;
	
	for(absY=radius;absY-- >0;)
	{
		HAL_GprmDramDma(copyDstAddr,map.mapDstAddr,map.mapWidth, 0x01);
		//printf("- absY=%d\n",absY);		
		//y=radius-absY-1;//+y
		Y_2=absY*absY;
		index=absY;
		for(absX=0; absX<radius; ++absX) 
		{
			X_2=absX*absX;

			r_2=Y_2+X_2;
			if(r_2>=R_2)break;
			if(r_2> table[index+1])index++;
			value=bubbleTable[index];

			srcX=value*absX;
			srcY=value*absY;
			
			if(flagX)
			{
				x=radius-absX-1;//+x
				srcX=radius-1-READ8(srcX, 0);//+x
			}
			else
			{
				x=absX;
				srcX=READ8(srcX, 0);//+x
			}
			
			srcY=absY-READ8(srcY, 0);//+y
			//
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
		//printf("(%u,%u)=>(%u,%u),%u,%u\n",x,y,srcX,srcY,index,value);
		//printf("\ntable:%u,%u\nsrc:%u,%u\nabs:%u,%u\n",index,value,srcX,srcY,absX,absY);
		}
		//write data to img
		HAL_GprmDramDma(map.mapDstAddr, dstAddr,map.mapWidth, 1);
		//load next map data
		HAL_GprmDramDma(mapAddr1, map.mapSrcAddr, len, 1);
		HAL_GprmDramDma(srcAddrWord, mapAddr2, map.mapWidth, 1);
		if(flagY)
		{
			dstAddr+=img.imgWidth;
			srcAddrWord+=img.imgWidth;
		}
		else
		{
			dstAddr-=img.imgWidth;//+y
			srcAddrWord-=img.imgWidth;//+y
		}
	}
	//printf("MAX:%u\ncount:%u\n",maxCount,count_);
}

#endif

/**
 * @fn		  void speBubFillBlack(void)
 * @brief	  Bubble Fill Black
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void speBubFillBlack()
{
	UINT16 i;
	UINT32 addr;
	UINT16 len;
		
	gprmBlackFill(map.mapSrcAddr/2,img.imgWidth,2);
	addr=img.srcBuf/2;
	len=img.winVoff;
	if(len>0)
		HAL_GprmScale
		(
			map.mapSrcAddr/2,
			img.imgWidth/2, 2,
			addr,
			img.imgWidth/2, len,
			map.mapDstAddr/2,
			1
		);
	
	addr=img.srcBuf/2+(UINT32)(img.winVoff+img.vSize)*img.imgWidth/2;
	len=img.srcVsize-img.winVoff-img.vSize;
	if(len>0)
		HAL_GprmScale
		(
			map.mapSrcAddr/2,
			img.imgWidth/2, 2,
			addr,
			img.imgWidth/2, len,
			map.mapDstAddr/2,
			1
		);

	
	addr=img.winBuf-(UINT32)img.winHoff;
	len=img.winHoff;
	if(len>0)
		HAL_GprmDramDma(map.mapSrcAddr, addr,len , 1);

	i=img.vSize-1;
	addr=img.winBuf+(UINT32)img.winWidth;
	len=img.imgWidth-img.winWidth;
	if(len>0)
		while(i--)
		{
			HAL_GprmDramDma(map.mapSrcAddr, addr,len , 1);
			addr+=(UINT32)img.imgWidth;
		}
	len=img.imgWidth-img.winWidth-img.winHoff;
	if(len>0)
		HAL_GprmDramDma(map.mapSrcAddr, addr,len , 1);
	

}
#endif