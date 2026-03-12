/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/

#ifndef __DBG_DEF_H__
#define __DBG_DEF_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
 
#include "dbg_file_list.h"
#include "dbg_assert.h"
#include "dbg_str.h"
#include "dbg_wdt.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
**************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define CPrintf(str) 		{\
							printf("[41m ");\
							printf(str);\
							printf(" [49m\n");\
						}

#define CPrintf1(str, a) 		{\
								printf("[41m ");\
								printf(str, a);\
								printf(" [49m\n");\
							}
	
#define CPrintf2(str, a, b) 		{\
								printf("[41m ");\
								printf(str, a, b);\
								printf(" [49m\n");\
							}

#define CPrintf3(str, a, b, c) 		{\
									printf("[41m ");\
									printf(str, a, b, c);\
									printf(" [49m\n");\
								}
	
#define ErrorPrintf(str)	CPrintf(str)
	
#define DBG_Printf
	
//up count and timebase 1 ms 
#if 1
#define dbgTimerStart()		{\
								HAL_GlobalReadGTimer(&G_TimerRef);\
							}
#define dbgTimerStop()		{\	
								UINT32 time;\
								HAL_GlobalReadGTimer(&time);\
								time = time - G_TimerRef; \
								printf(" time %ld\n",time);\
							}
#else
#define dbgTimerStart()	
#define dbgTimerStop()
#endif



//1 Please define this macro at the top of the debug source file, see "file_list.h"
//#define __FILE
#define __LINE	__LINE__
#define ASSERT(a, exit)		do {\
								if (!(a)) {\
									__ASSERT(__FILE, (unsigned short)__LINE, (exit));\
								}\
							} while (0)
#define INTR_ASSERT(a, exit)		do {\
								if (!(a)) {\
									__INTR_ASSERT(__FILE, (unsigned short)__LINE, (exit));\
								}\
							} while (0)

#define __FUNC_TRACK__		dbgFuncTrack(-1, (unsigned short)__LINE)

#ifdef __DEBUG_FUNC_TRACK__
#define __FUNC_ENTRY__		__FUNC_TRACK__
#define __FUNC_EXIT__		__FUNC_TRACK__
#else
#define __FUNC_ENTRY__		
#define __FUNC_EXIT__		
#endif

#define TAG() \
	printf("%s %d\n", __FILE__, (UINT16)__LINE__); \

#define _printf /printf

//////////////////////////////////////////////////////////////////////////////
//wangjg++ 20120625
#define ESC_COLOR_NORMAL   "\033[m"
#define ESC_COLOR_BLACK    "\033[30m"
#define ESC_COLOR_RED      "\033[31m"
#define ESC_COLOR_GREEN    "\033[32m"
#define ESC_COLOR_YELLOW   "\033[33m"
#define ESC_COLOR_BLUE     "\033[34m"
#define ESC_COLOR_MAGENTA  "\033[35m"
#define ESC_COLOR_CYAN     "\033[36m"
#define ESC_COLOR_WHITE    "\033[37m"

#define NORMAL( x )  ESC_COLOR_NORMAL x
#define BLACK( x )   ESC_COLOR_BLACK  x ESC_COLOR_NORMAL
#define RED( x )     ESC_COLOR_RED    x ESC_COLOR_NORMAL
#define GREEN( x )   ESC_COLOR_GREEN  x ESC_COLOR_NORMAL
#define BLUE( x )    ESC_COLOR_BLUE   x ESC_COLOR_NORMAL

//eg. printf(BLUE("Load\n")); 

/*
#define printf_color(color, str, args...)   printf( color str ESC_COLOR_NORMAL "\r", ##args )
#define fprintf_color(color, str, args...)  //fprintf( stderr, color str ESC_COLOR_NORMAL "\r", ##args )

#define profLogPrintf_color(id, color, str, args...)   //profLogPrintf( id, color str ESC_COLOR_NORMAL "\r", ##args )

#define printf_red(str, args...)      printf_color(  ESC_COLOR_RED,    str, ##args )
#define fprintf_red(str, args...)     fprintf_color( ESC_COLOR_RED,    str, ##args )
#define printf_blue(str, args...)     printf_color(  ESC_COLOR_BLUE,   str, ##args )
#define fprintf_blue(str, args...)    fprintf_color( ESC_COLOR_BLUE,   str, ##args )
#define printf_green(str, args...)    printf_color(  ESC_COLOR_GREEN,  str, ##args )
#define fprintf_green(str, args...)   fprintf_color( ESC_COLOR_GREEN,  str, ##args )
#define printf_yellow(str, args...)   printf_color(  ESC_COLOR_YELLOW, str, ##args )
#define fprintf_yellow(str, args...)  fprintf_color( ESC_COLOR_YELLOW, str, ##args )
*/
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 xdata G_TimerRef;
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void dbgBMInit(void);
void dbgBMStart(UINT8 bTmrMode);
void dbgBMShow(UINT8 bBMMode, UINT32* dwTimeTot, UINT32* dwTimeSeg, UINT32 dwLine);
void dbgBMStop(void);
#endif /*__DBG_DEF_H__*/

