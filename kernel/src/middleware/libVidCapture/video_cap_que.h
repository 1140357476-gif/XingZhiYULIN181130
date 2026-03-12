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
 * @file	  video_cap_que.h
 * @brief	 
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_CAP_QUE_H__
#define __Video_CAP_QUE_H__

#include "video_cap_vlcbuf.h"

void speedyCaptureInitQue(vlcBufList_t *pBufQue,QueParam_t *pQueParm);
vlcNode_t* speedyCaptureNodOutQue(vlcBufList_t *vlcQue);
UINT8 speedyCaptureIsStillQueFull(vlcBufList_t *vlcQue);
UINT8 speedyCaptureIsClipQueFull(vlcBufList_t *vlcQue);
UINT8 speedyCaptureIsQueEmpty(vlcBufList_t *pVlcCashQue);
/*void speedyCaptureEnterQue(vlcBufList_t *pQue,vlcNode_t* node);*/


#endif