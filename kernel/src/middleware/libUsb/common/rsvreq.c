/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        rsvreq.c

Abstract:

        Module related to reserved request

Environment:

        Keil C51 Compiler

Revision History:

        08/28/2001      Chi-Yeh Tsai    created                 

--*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "rsvreq.h"
#include "hal.h"
#include "main.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//RSVREQ_NoDataPhase
//-----------------------------------------------------------------------------
void RSVREQ_NoDataPhase(void) USING_1
/*++

Routine Description:

        process reserved request without data phase

Arguments:

        none

Return Value:

        none

--*/
{
}

//-----------------------------------------------------------------------------
//RSVREQ_PreInDataPhase
//-----------------------------------------------------------------------------
void RSVREQ_PreInDataPhase(void) USING_1
/*++

Routine Description:

        pre-process setting of following in data phase

Arguments:

        none

Return Value:

        none

--*/
{
        G_pucCtrlDataPointer = G_ucCtrlData;        
}

//-----------------------------------------------------------------------------
//RSVREQ_PreOutDataPhase
//-----------------------------------------------------------------------------
void RSVREQ_PreOutDataPhase(void) USING_1
/*++

Routine Description:

        pre-process setting of following out data phase

Arguments:

        none

Return Value:

        none

--*/
{
        G_pucCtrlDataPointer = G_ucCtrlData;
}

//-----------------------------------------------------------------------------
//RSVREQ_PostInDataPhase
//-----------------------------------------------------------------------------
void RSVREQ_PostInDataPhase(void) USING_1
/*++

Routine Description:

        post-process data of complete in data phase

Arguments:

        none

Return Value:

        none
                             
--*/
{
}

//-----------------------------------------------------------------------------
//RSVREQ_PostOutDataPhase
//-----------------------------------------------------------------------------
void RSVREQ_PostOutDataPhase(void) USING_1
/*++

Routine Description:

        post-process data of complete out data phase

Arguments:

        none

Return Value:

        none

--*/
{
}

