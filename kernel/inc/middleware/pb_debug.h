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
 * @file      pb_debug.h
 * @brief     debug option for playback module
 * @author    tq.zhang
 * @since     2010-05-12
 * @date      2010-05-12
*/
#ifndef __PB_DEBUG_H__
#define __PB_DEBUG_H__

//=============================================================================
//Header file
//=============================================================================
#include "dbg_file_list.h"
#include "dbg_def.h"
//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define printBugReproduce printf  //printf debug reproduce info
#if 0
#define pbLogPrintf printf
#else
#define pbLogPrintf /DBG_SLASH
#endif


/*
    Debug level define
    
0: Disable all debug information
1: dbgPrintf enabled
2: dbgPrintf / printf enabled
3: dbgPrintf / printf / func track enabled
*/

#define DBG_Level		0

#if (DBG_Level == 0)
#define DBG_pbFuncTrack()
//#define printf(a)
#define dbgPrintf(StringID)	if
#elif (DBG_Level == 1)
#define DBG_pbFuncTrack()
//#define printf(a)
//#define dbgPrintf(StringID)
#elif (DBG_Level == 2)
#define DBG_pbFuncTrack()	
//#define printf(a)
//#define dbgPrintf(StringID)
#elif (DBG_Level == 3)
#define DBG_pbFuncTrack()	dbgFuncTrack(__FILE_ID_LIB_PB__, (unsigned short)__LINE__)
//#define printf(a)
//#define dbgPrintf(StringID)
#endif

#ifndef printBugReproduce
#define printBugReproduce /_printf
#endif

#endif /*__PB_DEBUG_H__*/




