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
 * @file	  video capture
 * @brief	  video capture define .h 
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __VIDEO_CAPDEF_H__
#define __VIDEO_CAPDEF_H__

#include "general.h"

#define MAXVLC_BUFNUM 35

typedef struct vlcNode
{
	UINT32 vlcAddr;
	UINT32 vlcSize;
	UINT32 bufAddr;
	UINT32 bufSize;
}vlcNode_t; 

typedef struct vlcBufList
{
	vlcNode_t dataNod[MAXVLC_BUFNUM];
	UINT8 front;
	UINT8 rear;
	UINT8 capacity;
	UINT8 size;
	UINT8 snapCnt;
	UINT8 bufIdx;
}vlcBufList_t;

#endif/*__VIDEO_CAPDEF_H__ */



