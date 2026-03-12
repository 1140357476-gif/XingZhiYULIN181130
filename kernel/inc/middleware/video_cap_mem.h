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
 * @file	  video_cap_mem.h
 * @brief	  allocate buffer 
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
*/
#ifndef __Video_CAP_MEM_H__
#define __Video_CAP_MEM_H__

#include "video_mem.h"

UINT32 speedyCaptureMemAddrGet_Intr(vidmemCbIdx_t idx);
UINT32 speedyCaptureMemAddrGet(vidmemCbIdx_t idx);
void speedyCaptureMemSizeSet(vidmemCbIdx_t idx, UINT32 size);
UINT8 speedyCaptureVlcBufNumGet(UINT32 vlcBufSize,UINT32 vlcBufAddrStart,UINT32 vlcBufAddrEnd);
UINT32 speedyCaptureMemSizeGet(vidmemCbIdx_t idx);
UINT8 speedyCaptureFreeBufNumGet();

#endif
