/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		main.c

Abstract:

		Module related to main routine

Environment:

		Keil C51 Compiler

Revision History:

		08/28/2001		Chi-Yeh	Tsai	created

--*/

//=============================================================================
//Define
//=============================================================================

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "reg_def.h"
//=============================================================================
//Program
//=============================================================================

/**
 * @fn        void jump_main(void)
 * @brief     jump_main
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-11-22
 * @todo      N/A
 * @bug       N/A
*/
#pragma OT(8, SIZE)
void jump_main(void)
{
    UINT16 i;

    /* enable dram shadow */
    XBYTE[REG_Cpu_ShadowEn] = 0x04;

	/* several delay */
    i = 8;
    while(--i);

	/* jump to "C_C51STARTUP" */
    (*(void(code*)())0x1000)();
}