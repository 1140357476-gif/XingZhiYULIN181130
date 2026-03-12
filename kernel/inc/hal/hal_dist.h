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
 * @file      hal_dist.h
 * @brief     header file of hal_dist.c
 * @author    Tq.zhang
 * @since     2010-04-22
 * @date      2010-04-22
*/
#ifndef _HAL_DIST_H__
#define _HAL_DIST_H__

//DIST
void HAL_Dist2ndPathOpen(void);
void HAL_Dist2ndPathClose(void);
void HAL_DistMeIntervalSet(UINT8 Intval);
void HAL_DistMeIntrEn(	UINT8 start,UINT8 done);
void HAL_DistEn(	UINT8 opt);
void HAL_DistDisable();
void HAL_DistSwRst(UINT8 rst);
void HAL_DistGateClkSet(UINT8 clkOpt,UINT8 sel);
void HAL_DistBlkNumSet(UINT8 BlkNum);
void HAL_DistMeModeSet(UINT8 aut);
UINT8  HAL_DistMeOutAddrSet(UINT32 addr);
#endif
