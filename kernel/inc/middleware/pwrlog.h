/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2004-2007 by Sunplus Technology Co., Ltd.          *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 5553 $
$Date:: 2010-12-09 20:35:58#$
 */

#ifndef __PWRLOG_H_
#define __PWRLOG_H_

#define LOG2_USE_10BIT
#undef LOG2_USE_8BIT

#define LOG2_OVERFLOW 0x7fffffffL
#define LOG2_RANGE_ULIMIT 32

#ifdef LOG2_USE_10BIT
	#define LOG2_ONE 1024
	#define LOG2_ONE_QBIT 10
	#define LOG2_ON_NTH_PARTITION 256
#endif /*PWR2_USE_10BIT*/
#ifdef LOG2_USE_8BIT
	#define LOG2_ONE 256
	#define LOG2_ONE_QBIT 8
	#define LOG2_ON_NTH_PARTITION 64
#endif /*LOG2_USE_8BIT*/

#define LOG2_DOMAIN_ERR 0x80000000L
#if 0
#define LINEAR2_DOMAIN_ERR 0x80000000L
#endif

SINT32 log2xyND(SINT32 num, SINT32 denom, SINT32 *log2X, SINT32 *log2Y);

#endif
