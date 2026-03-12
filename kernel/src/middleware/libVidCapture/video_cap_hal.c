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
 * @file	  video_cap_hal.c
 * @brief	  
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
 */
#include "video_cap_hal.h"

void IntrSpeedyCaptureVlcBufSwitch(UINT32 addr,UINT8 *bufIdx)
{
	if(addr == 0){
		goto Skip;
	}
	if(*bufIdx == IDX_A_BUF) { /*A buffer*/
		XBYTE[REG_Dram_VlcAAddr_B0] = *((UINT8*)&addr + 3 );
		XBYTE[REG_Dram_VlcAAddr_B1] = *((UINT8*)&addr + 2 );
		XBYTE[REG_Dram_VlcAAddr_B2] = *((UINT8*)&addr + 1 );
		XBYTE[REG_Dram_VlcAAddr_B3] = *((UINT8*)&addr );
		XBYTE[REG_Dram_VlcRdy] = 0x01;
	}
	else {
		XBYTE[REG_Dram_VlcBAddr_B0] = *((UINT8*)&addr + 3 );
		XBYTE[REG_Dram_VlcBAddr_B1] = *((UINT8*)&addr + 2 );
		XBYTE[REG_Dram_VlcBAddr_B2] = *((UINT8*)&addr + 1 );
		XBYTE[REG_Dram_VlcBAddr_B3] = *((UINT8*)&addr );
		XBYTE[REG_Dram_VlcRdy] = 0x02;
	}
Skip:	
	*bufIdx = 1 - *bufIdx;
}


UINT8 IntrSpeedyCaptureReadVlcSize(UINT32 *vlcSize)
{
	#if 0 // mantis bug #45601
	if((XBYTE[REG_Dram_VlcRdy] & 0x01) && (!bufIdx)) {
		WRITE8(*vlcSize,3,XBYTE[REG_Dram_VlcASize_B0]);
		WRITE8(*vlcSize,2,XBYTE[REG_Dram_VlcASize_B1]);
		WRITE8(*vlcSize,1,XBYTE[REG_Dram_VlcASize_B2]);
	}
	else if((XBYTE[REG_Dram_VlcRdy] & 0x02) && (bufIdx)) {
		WRITE8(*vlcSize,3,XBYTE[REG_Dram_VlcBSize_B0]);
		WRITE8(*vlcSize,2,XBYTE[REG_Dram_VlcBSize_B1]);
		WRITE8(*vlcSize,1,XBYTE[REG_Dram_VlcBSize_B2]);
	}
	else{ // wait sync
		if(XBYTE[REG_Dram_VlcRdy] & 0x01){
			XBYTE[REG_Dram_VlcRdy] = 0x01;	//clear vlcardy
		}
		else{
			XBYTE[REG_Dram_VlcRdy] = 0x02;	//clear vlcbrdy
		}
		INTR_putchar('W');
		return FAIL;
	}
	#else
	/*DRAM latch vlcsize的時間點跟JPG engine latch vlcsize的時間點不同步*/
	/*WRITE8(*vlcSize,3,XBYTE[REG_Jpeg_VLCSize_Byte0]);
	WRITE8(*vlcSize,2,XBYTE[REG_Jpeg_VLCSize_Byte1]);
	WRITE8(*vlcSize,1,XBYTE[REG_Jpeg_VLCSize_Byte2]);*/
	((UINT8 *)vlcSize)[3] = XBYTE[REG_Jpeg_VLCSize_Byte0];
	((UINT8 *)vlcSize)[2] = XBYTE[REG_Jpeg_VLCSize_Byte1];
	((UINT8 *)vlcSize)[1] = XBYTE[REG_Jpeg_VLCSize_Byte2];
	((UINT8 *)vlcSize)[0] = 0;
	#endif
	
	return SUCCESS;
}



