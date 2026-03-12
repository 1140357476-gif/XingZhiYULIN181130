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
#include "general.h" 
#include "dbg_str.h"
/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


//-----------------------------------------------------------------------------
//ASSERT
//-----------------------------------------------------------------------------
/**
 * @brief   ASSERT
 * @param
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since   2008-02-20
 * @todo    Something need to be done.
 * @bug		a bug need to be resolved.
*/
#pragma disable
UINT8 __ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT)
{
	UINT8 status;

	if (__EXIT) {
		printf("ASSERT -> F(0x%04x), L(%u)\n", __FILE_ID, __LINE_NUM);
		return FAIL;
	} else {
		printf("STOP @ F(0x%04x), L(%u)\n", __FILE_ID, __LINE_NUM);
		ENTER_CRITICAL(status);
		while(1);
	}
}
void __PRINTF(UINT16 __FILE_ID, UINT16 __LINE_NUM)
{
	printf("File:0x%04x,Line:%u\n",__FILE_ID,__LINE_NUM);
}
/**
 * @brief   INTR ASSERT
 * @param
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since   2008-02-20
 * @todo    Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 __INTR_ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT)
{
	UINT8 status;

	if (__EXIT) {
		printf("ASSERT -> F(0x%04x), L(%u)\n", __FILE_ID, __LINE_NUM);
		return FAIL;
	} else {
		printf("STOP @ F(0x%04x), L(%u)\n", __FILE_ID, __LINE_NUM);
		ENTER_CRITICAL(status);
		while(1);
	}
}


//-----------------------------------------------------------------------------
//dbgRstDump
//-----------------------------------------------------------------------------
/**
 * @brief   dbgRstDump
 * @param
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @sinc    2008-02-20
 * @todo    Something need to be done.
 * @bug		a bug need to be resolved.
*/
void dbgRstDump(void)
{
	UINT8 status;
    
	putchar('$');
	putchar('$');
	putchar('\n');
    
	ENTER_CRITICAL(status);
	while(1);
}

void dbgFuncTrack(UINT16 __FILE_ID, UINT16 __LINE_NUM) 
{
	printf("F(0x%04x), L(%u)\n", __FILE_ID, __LINE_NUM);
}

#if 0
UINT32 xdata G_TimerStartRef;
UINT32 xdata G_TimerEndRef;

UINT32 xdata G_TimerThresholdRef;
UINT8 xdata G_TimerFlag;

UINT16 xdata G_TimerStartLine;
UINT16 xdata G_TimerEndLine;

UINT16 xdata G_TimerExceptionArray[4] = {
	379,427,
	415,501,
};

void
ENTER_CRITICAL_TIME_MEASURE(
	UINT16 sLine
)
{
	if (G_TimerFlag) {
		G_TimerStartLine = sLine;
	}
}

void
EXIT_CRITICAL_TIME_MEASURE(
	UINT16 eLine
)
{
	if ( G_TimerFlag ) {
		
		G_TimerEndLine = eLine;
		
	 	if ( (G_TimerStartLine == G_TimerExceptionArray[0]) && (G_TimerEndLine == G_TimerExceptionArray[1]) ) {
			return;
		}
		
		if ( (G_TimerStartLine == G_TimerExceptionArray[2]) && (G_TimerEndLine == G_TimerExceptionArray[3]) ) {
			return;
		}
		
		if ( (G_TimerEndRef - G_TimerStartRef) > G_TimerThresholdRef ) {
			printf("T%lu,L%u,%u\n",(G_TimerEndRef - G_TimerStartRef),G_TimerStartLine,G_TimerEndLine); 
		}
	}
}
#endif

