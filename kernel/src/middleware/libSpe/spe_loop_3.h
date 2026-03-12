/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      spe_loop_3.h
 * @brief     bubble
 * @author    tq.zhang
 * @since     2010-05-12
 * @date      2010-05-12
*/
#ifndef __SPE_LOOP_3__
#define __SPE_LOOP_3__

#include "spe_def.h"
#include "gprm.h"

#define yIndex(x)  (UINT16)((((x) & ~7) << 1) | ((x) & 7))
#define uIndex(x)  (UINT16)((UINT16)(x) + (UINT16)uOffTab[(UINT8)(x) & 7] )
#define vIndex(x)  (UINT16)((UINT16)(x) + (UINT16)vOffTab[(UINT8)(x) & 7] )

#define 	UU(x) (UINT16)(uOffTab[(UINT8)(x) & 7] )
#define 	VV(x) (UINT16)(vOffTab[(UINT8)(x) & 7] )

#define READ8_(a) (*((unsigned char *)&(a)))
#define	READ16_(a) (*(unsigned short *)(&(a)))

static UINT8 code uOffTab[] = {8, 7, 7, 6, 6, 5, 5, 4};
static UINT8 code vOffTab[] = {12, 11, 11, 10, 10, 9, 9, 8};


extern void speBubble(UINT8 isFillBlack);

extern void speCreatBubbleTable(UINT16 radius,UINT32 resID);

extern void speDoBubble1_4(UINT32 srcAddr,UINT16 radius,UINT8 flag,UINT8 isFillBlack);

extern void speBubFillBlack();
#endif