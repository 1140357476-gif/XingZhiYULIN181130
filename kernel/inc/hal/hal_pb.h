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
 * @file	  hal_pb.h
 * @brief	  header file of hal_pb.c
 * @author	  Tq.zhang
 * @since	  2010-04-28
 * @date	  2010-04-28
*/
#ifndef _HAL_PB_H__
#define _HAL_PB_H__
#include "general.h"

//Still PB
void HAL_PbBufAddrSet(UINT32 addrWord);
void HAL_PbBufAddrGet(UINT32* addrWord);
void HAL_PbBufHsizeSet(UINT16 width);
void HAL_PbSyncPara2DispSof(void);

//Video PB

#endif
