/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		main.c

Abstract:

		Module related to main routine

Environment:

		Keil C51 Compiler

Revision History:

--*/
#ifdef ISP_PROGRAM
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "main.h"
#include "setmode.h"
#include "asicreg.h"
//#include "cardcon.h"
#include "cardfunc.h"
#include "cardvar.h"
#include "doslink.h"
#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"
//#include "dosusr.h"
//#include "uiflow.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//SMC_CheckValidBlock
//-----------------------------------------------------------------------------
UINT8 SMC_CheckValidBlock(UINT32 *pPhyAddr) USING_0
{
        UINT8 sts = TRUE;
        UINT8 good;
	UINT8 stsData2 = 0xFF;
	UINT8 stsData1;
	UINT32 maxPhyAddr;
	
	maxPhyAddr = (UINT32)G_Card_TotalBank * G_Card_PhysicalBankSize;
	#if (K_NANDF_STRICT_BAD_CHECK)
	SMC_RedunCard2Sram(*pPhyAddr + G_NANDF_PageSize); 
	#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		stsData2 = G_ucStorData[K_Card_PageSize];
	else
	#endif
		stsData2 = G_ucStorData[K_Card_PageSize + 5];
	    sts = SMC_Card2SramForExt(*pPhyAddr);
	#else
	    //For "A" Area access
	    sts = SMC_Card2SramForExt(*pPhyAddr);
	#endif
	    
	#if 1//(K_SUPPORT_NEWNAND)
	if (G_NANDF_Category)
		stsData1 = G_ucStorData[K_Card_PageSize];
	else
	#endif
	stsData1 = G_ucStorData[K_Card_PageSize + 5];
	if ((good = SMC_BlockStsCheck(*pPhyAddr, stsData1, stsData2)) == FALSE)
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, begin
	{
		if (*pPhyAddr >= maxPhyAddr)
		{
			sts = FALSE;
			//printf("exceed max phy address=%lx\n", *pPhyAddr);
		}
	}
	// @patch 5.1@012403@wyeo@fix cross nand bank bug, end                    
                                               
        return sts;
}
#endif
