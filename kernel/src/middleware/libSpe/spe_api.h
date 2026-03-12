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
 * @file	  spe_api.h
 * @brief	  api for special effect
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/
#ifndef __SPE_API__
#define __SPE_API__
#include "spe_def.h"

#define spePinch_punch(isPinch) speSimple(SPE_DISTORT_H_PINCH,isPinch)
#define speShrink_stretch(isShrink) speSimple(SPE_DISTORT_H_SHRINK,isShrink)
#define speHourglass(flag) speSimple(SPE_DISTORT_H_HOURGLASS,flag)

#define spePinch_punch_V(isPinch) speSimpleV(SPE_DISTORT_H_PINCH,isPinch)
#define speShrink_stretch_V(isShrink) speSimpleV(SPE_DISTORT_H_SHRINK,isShrink)
#define speHourglass_V(flag) speSimpleV(SPE_DISTORT_H_HOURGLASS,flag)

#define speMirror_H() speSimpleV(SPE_DISTORT_H_MIRROR, 0)
#define speMirror_V() speMirror()

extern void speMirror();
//spe_loop_1
extern void speSimple(UINT8 ID,UINT8 flag);
//spe_loop_1v
extern void speSimpleV(UINT8 ID,UINT8 flag);

 //spe_loop_2
extern void speWavy();

//spe_loop_3
extern void speBubble(UINT8 isFillBlack);

#endif
