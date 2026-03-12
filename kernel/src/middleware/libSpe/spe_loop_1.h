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
 * @file      spe_loop_1.h
 * @brief     loop 1
 * @author    tq.zhang
 * @since     2010-05-12
 * @date      2010-05-12
*/
#ifndef __SPE_LOOP_1__
#define __SPE_LOOP_1__

#include "spe_def.h"
extern void speMirror();
extern void speSimple(UINT8 ID,UINT8 flag);
extern void speSimpleV(UINT8 ID,UINT8 flag);

extern void speCreatPinchTable(
	UINT16 DATA vSize,
	UINT16 xdata* DATA table
	);
extern void speCreatHourglass(
	UINT16 vSize,
	UINT16 xdata *DATA table,
	UINT32 tmpAddr);
	
extern void speH_Loop_1(
	UINT32 dataAddr,
	UINT16 hSize, UINT16 vSize,
	UINT16 imgWidth,
	UINT32 mapSrcAddr,UINT32 mapDstAddr, 
	UINT16 mapWidth,
	UINT16 hStart,UINT16 hEnd,
	UINT16 xdata*  incTable,
	UINT8 xdata* DATA pSrcData,
	UINT8 xdata* DATA pDstData,
	UINT8  flag
	);	
#endif