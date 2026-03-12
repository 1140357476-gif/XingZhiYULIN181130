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
 * @file	  spe_def.h
 * @brief	  parameters and function for special effect
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
 */
 
#ifndef __SPE_DEF__
#define __SPE_DEF__
#include "general.h"
#include "initio.h"
#include "spe_distort.h"

// function 
#define EN_SPE 1
#define EN_SPE_PINCH_PUNCH		1
#define EN_SPE_STRETCH_SHRINK	1
#define EN_SPE_HOURGLASS		1
#define EN_SPE_WAVY 			1
#define EN_SPE_BUBBLE			1

// printf 
#define EN_PRINTF	0
#define EN_PRINTF_TIME 0
#define EN_PRINTF_NAME 0

// data\idata
#define IDATA idata
#define DATA idata

//	res
#define SPE_TABLE_ID_HOURGLASS 0x34
#define SPE_TABLE_ID_WAVY 0x35
#define SPE_TABLE_ID_BUBBLE 0x36
#define SPE_TABLE_LEN_BUBBLE 240
#define SPE_TABLE_LEN_WAVY 240
#define SPE_TABLE_LEN_BUBBLE 240

//
#define READ8_(a) (*((unsigned char *)&(a)))
#define	READ16_(a) (*(unsigned short *)(&(a)))

// struct
typedef struct
{
	UINT32 dataSrcAddr;
	UINT32 dataDstAddr;

	UINT32 mapSrcAddr;
	UINT32 mapDstAddr;
	UINT8 xdata* pSrcData;
	UINT8 xdata* pDstData;

	
	UINT16 mapBlkSize;
	UINT16 mapWidth;

	UINT16 ramMapBank;

} MAP_BANK;
typedef struct
{
	UINT32 srcBuf;
	UINT32 tmpBuf;

	UINT32 winBuf;
	UINT16 winHoff;
	UINT16 winVoff;
	UINT16 winWidth;
	
	UINT16 hSize;
	UINT16 vSize;

	UINT16 imgWidth;
	UINT16 srcVsize;
	
	UINT16 hStart;
	UINT16 hEnd;
} SPE_IMG_DESC;

// gloab value
extern SPE_IMG_DESC img;
extern MAP_BANK map;
extern xdata	UINT8	G_ucStorData[];


// function
extern void speWriteToMap(UINT32 DramAddr, UINT16 imgWidth, UINT16 vSize, UINT16 len,UINT32 mapAddr );
extern void speWriteToMap_Ex(UINT32 DramAddr, UINT16 imgWidth, UINT16 vSize, UINT16 len,UINT32 mapAddr );
extern void speReadFromMap(UINT32 DramAddr, UINT16 imgWidth, UINT16 vSize, UINT16 len, UINT32 mapAddr );
extern UINT8 xdata* speLoadEffectTable(UINT32 addr32,	UINT8 resID);
extern void speSetStructImg(UINT32 srcAddr, UINT16 srcHsize,  UINT16 srcVsize, UINT16 hOffset, UINT16 vOffset, UINT16 hSize, UINT16 vSize, UINT32 tmpAddr);
extern void speSetStructMap(UINT32 imgAddr, UINT32 mapAddr, UINT16 mapWidth, UINT16 mapBlkSize,UINT8 id);
	
#endif