/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		HAL_SD.c

Abstract:

		Module related to HAL NAND flash	functions

Environment:

		Keil C51 Compiler

Revision History:

		11/12/2001		WZH	   created
--*/



//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal.h"
#include "reg_def.h"

#define __FILE		__FILE_ID_HAL_SD_INTR__

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

UINT8 INTR_HAL_SDDram(UINT32 dramAddr, UINT16 nByte1, UINT8 read)
{
    UINT32 timeout_count = 0xFFFF;
    
	#if DRAM_ASSERT
	if (dramAddr <= 0x40000){
    	//printf("@ASSERT 0x%x %d\n",__FILE, (UINT16)__LINE__);
		while(1);
	}
	#endif

    XBYTE[REG_Fm_evt] = 0x20;                      //clr dma done evt 

    if(read)
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x01;                       //read mode
    else 
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x00;                       //write mode

    nByte1 --;
    XBYTE[REG_Fm_drambytecnt_Low] = READ8(nByte1, 1);   
    XBYTE[REG_Fm_drambytecnt_High] = READ8(nByte1, 0);

    XBYTE[REG_Fm_datstraddr_0] = READ8(dramAddr, 3);
    XBYTE[REG_Fm_datstraddr_1] = READ8(dramAddr, 2);
    XBYTE[REG_Fm_datstraddr_2] = READ8(dramAddr, 1);
    XBYTE[REG_Fm_datstraddr_3] = READ8(dramAddr, 0)&0x01;

    XBYTE[REG_Fm_dramstr] = 1;  //triger
    
    return HALK_SUCCESS;
}

