/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		dbg_bench_mark.c
 * @brief		bench mark test
 * @author		Phil Lin
 * @since		2010-02-08
 * @date		2010-02-08
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_global.h"
//#include "dbg_bench_mark.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
//#if BENCH_MARK_EN
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT32 xdata sdwTimeVal;
static UINT8 xdata sbBMRun;
static UINT8 xdata sbBMMode;
static UINT32 xdata sdwMarkCnt;
/**
 * @brief	Bench mark
 * @param	NONE
 * @retval	NONE.
 * @see
 * @author	Phil Lin
 * @since	2010-02-08
 * @todo	N/A
 * @bug		N/A
*/
void dbgBMInit(void)
{
    sdwTimeVal = 0;
    sdwMarkCnt = 0;
    sbBMRun = 0;
    sbBMMode = 0;
}

/**
 * @brief	Bench mark
 * @param	NONE
 * @retval	NONE.
 * @see
 * @author	Phil Lin
 * @since	2010-02-08
 * @todo	N/A
 * @bug		N/A
*/
void dbgBMStart(UINT8 bTmrMode)
{
    sbBMRun = 1;
    sbBMMode = bTmrMode;
    
    /* support 10us / 1ms mode */
    /* start to recode the timer */
    if (!sbBMMode) {
        HAL_GlobalReadGTimer(&sdwTimeVal);
    } else {
        HAL_GlobalTimerRead10us(&sdwTimeVal);
    }
}

/**
 * @brief	Bench mark
 * @param	NONE
 * @retval	NONE.
 * @see
 * @author	Phil Lin
 * @since	2010-02-08
 * @todo	N/A
 * @bug		N/A
*/
void dbgBMShow(UINT8 bBMMode, UINT32* dwTimeTot, UINT32* dwTimeSeg, UINT32 dwLine)
{
    UINT32 time;
    
    if (!bBMMode || !sbBMRun) return;

    /* snap the timer value */
    if (!sbBMMode) {
        HAL_GlobalReadGTimer(&time);
    } else {
        HAL_GlobalTimerRead10us(&time);
    }

    if (dwLine) printf("L(%04lu)>", dwLine);
    printf("S(%02lu)>", sdwMarkCnt);
    if (!sbBMMode) {
        printf("1ms>> ");
    } else {
        printf("10us>> ");
    }
    if (time < sdwTimeVal) {
        sbBMRun = 0;
        printf("timer overflow!\n");
        return;
    }
        
    /* there're two mode to show the timer value */       
    if (bBMMode & 0x01) {
        printf("tot(%lu), ", time);
        if (dwTimeTot) *dwTimeTot = time;
    }
    if (bBMMode & 0x10) {
        printf("seg(%lu)", time - sdwTimeVal);
        if (dwTimeSeg) *dwTimeSeg = time - sdwTimeVal;
    }

    putchar('\n');
    sdwMarkCnt++;

    /* snap the timer value again to recode the next segment */
    if (!sbBMMode) {
        HAL_GlobalReadGTimer(&time);
    } else {
        HAL_GlobalTimerRead10us(&time);
    }
    if (time < sdwTimeVal) {
        sbBMRun = 0;
        printf("timer overflow!\n");
        return;
    }

    sdwTimeVal = time;
}

/**
 * @brief	Bench mark
 * @param	NONE
 * @retval	NONE.
 * @see
 * @author	Phil Lin
 * @since	2010-02-08
 * @todo	N/A
 * @bug		N/A
*/
void dbgBMStop(void)
{
    sdwTimeVal = 0;
    sdwMarkCnt = 0;
    sbBMRun = 0;
    sbBMMode = 0;
}
//#endif /* BENCH_MARK_EN */
