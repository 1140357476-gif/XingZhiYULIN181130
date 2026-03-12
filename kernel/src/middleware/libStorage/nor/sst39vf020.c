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
//Define
//=============================================================================
#define MAKER_ID                0xBF    /* SST Manufacturer's ID code   */
//#define DEVICE_ID               0xD5    /* SST 39VF010 device code      */
#define DEVICE_ID               0xD6    /* SST 39VF020 device code      */
//#define DEVICE_ID               0xD7    /* SST 39VF040 device code      */

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "asicreg.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "sdramhw.h"
#include "doslink.h"

#include "isp\isp.h"

void ispToggleReadyCheck (UINT32 Dst);

void 
isp1MicroSecondsDelay(
	void
)
{
	UINT32 Gtimer;
	HAL_ConfigGTimer(0x01); //up count mode and time stick: 10us
	HAL_WriteGTimer(0x000000);
	
	HAL_StartGTimer();
	do
	{
		//printf("Gtimer %lx \n", Gtimer);
		HAL_GlobalReadGTimer(&Gtimer);
	} 
	while(Gtimer < 2);
	HAL_StopGTimer();
}

void 
isp10MilliSecondsDelay(
	void
)
{
	HAL_Wait(10);
}

/*************************************************************************/
/* PROCEDURE:   ispIdCheck                                       */
/*                                                                      */
/* This procedure  decides whether a  physical hardware device has a    */
/* SST 39VF040 4 Mbit Multi-Purpose Flash installed or not.             */
/*                                                                      */
/* Input:                                                               */
/*          None                                                        */
/*                                                                      */
/* Output:                                                              */
/*          return TRUE:  indicates a SST 39VF040                       */
/*          return FALSE: indicates not a SST 39VF040                   */
/************************************************************************/
UINT8 
ispNorIdCheck(
	UINT32 offset
)
{
	UINT16 Temp;
	UINT8 id1;
	UINT8 id2;
	UINT32  ReturnStatus;

	printf("offset %lx\n",offset);
	/*  Issue the Software Product ID code to 39VF040   */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] = 0xAA;                       /* write data 0xAA to the address       */
	Temp  = offset + 0x2AAA; /* set up address to be A000:2AAAh      */
	XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] = 0x90;                   /* write data 0x90 to the address       */

	isp1MicroSecondsDelay();       /* check DATABOOK for the most  */
	                                /* accurate value -- Tida       */
	/* Read the product ID from 39VF040 */

	id1  = XBYTE[offset + 0x0000]; /* set up address to be A000:0000h      */
	id2  = XBYTE[offset + 0x0001];/* set up address to be A000:0001h      */

	/* Determine whether there is a SST 39VF040 installed or not */

	printf("Maker  ID  [0x%2bx]\n", id1);
	printf("Device ID  [0x%2bx]\n", id2);
	if ((id1 == MAKER_ID) && (id2 == DEVICE_ID)){
	        ReturnStatus = TRUE;
	}
	else {
	       ReturnStatus = FALSE;
	}

	/* Issue the Soffware Product ID Exit code thus returning the 39VF040   */
	/* to the read operating mode                                           */

	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] =0xAA;                   /* write data 0xAA to the address       */
	Temp  = offset + 0x2AAA; /* set up address to be A000:2AAAh      */
	XBYTE[Temp] =0x55;                   /* write data 0x55 to the address       */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] =0xF0;                    /* write data 0xF0 to the address       */

	isp1MicroSecondsDelay();       /* check DATABOOK for the most  */
	                                /* accurate value -- Tida       */

	return ReturnStatus;
}


/*************************************************************************/
/* PROCEDURE:   ispEntireChipErase                                       */
/*                                                                      */
/* This procedure can be used to erase the entire chip.                 */
/*                                                                      */
/* Input:                                                               */
/*      Src     SOURCE address which the erase operation will start     */
/*                                                                      */
/* Output:                                                              */
/*      NONE                                                            */
/************************************************************************/
UINT8 
ispNorEntireChipErase(
	UINT32 offset
)
{
	UINT16 Temp;
	UINT32 i;

	printf("offset[%lx]\n",offset);
	/*  Issue the Sector Erase command to 39VF040   */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] =0xAA;                   /* write data 0xAA to the address       */
	Temp  = offset + 0x2AAA; /* set up address to be A000:2AAAh      */
	XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] = 0x80;                   /* write data 0x80 to the address       */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] = 0xAA;                   /* write data 0xAA to the address       */
	Temp  = offset + 0x2AAA; /* set up address to be A000:2AAAh      */
	XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
	Temp  = offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] = 0x10;                   /* write data 0x55 to the address       */
	#if 1
	ispToggleReadyCheck((UINT32)Temp);    /* wait for TOGGLE bit to get ready     */
	#else
	for( i = 0 ; i < 50 ; i++ ){ 
		isp10MilliSecondsDelay();      /* check DATABOOK for the most  */
	}                                  /* accurate value -- Tsce       */
	#endif	
	return TRUE;										
}

/************************************************************************/
/* PROCEDURE:   ispOneByteProgram                                        */
/*                                                                      */
/* This procedure can be used to program ONE byte of date to the        */
/* 39VF040.                                                             */
/*                                                                      */
/* NOTE:  It is VERY important the sector containing the byte to be     */
/*        programmed was ERASED first.                                  */
/*                                                                      */
/* Input:                                                               */
/*           Src     The BYTE which will be written to the 39VF040.     */
/*           Dst     DESTINATION address which will be written with the */
/*                   data passed in from Src                            */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/
void 
ispNorOneByteProgram (
	UINT32 offset,
	UINT8 SrcByte,
	UINT32 Dst
)
{
	UINT16 Temp,i;
	Temp =  offset + 0x5555; /* set up address to be A000:555h       */
	XBYTE[Temp] = 0xAA;                   /* write data 0xAA to the address       */
	Temp =  offset + 0x2AAA; /* set up address to be A000:2AAAh      */
	XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
	Temp =  offset + 0x5555; /* set up address to be A000:5555h      */
	XBYTE[Temp] =0xA0;                   /* write data 0xA0 to the address       */
	XBYTE[(UINT16)Dst] = SrcByte;             /* transfer the byte to destination     */
	//printf("offset[%lx]SrcByte[%bx]Dst[%lx]\n",offset,SrcByte,Dst);
	#if 1
	ispToggleReadyCheck(Dst);    /* wait for TOGGLE bit to get ready     */
	#else
	for(i=0;i<4;i++){
		isp1MicroSecondsDelay();
	}
	#endif
}

#if 0
/*************************************************************************/
/* PROCEDURE:   ispOneSectorErase                                        */
/*                                                                      */
/* This procedure can be used to erase a total of 4096 bytes.           */
/*                                                                      */
/* Input:                                                               */
/*      Src     SOURCE address which the erase operation will start     */
/*                                                                      */
/* Output:                                                              */
/*      NONE                                                            */
/************************************************************************/
UINT32
ispOneSectorErase(
	UINT32 offset,
	UINT8 *Dst
)
{
    UINT8 *Temp;
	UINT32 i;

    /*  Issue the Sector Erase command to 39VF040   */

    Temp  = (UINT8 *)(offset + 0x5555); /* set up address to be A000:5555h      */
    XBYTE[Temp] = 0xAA;                   /* write data 0xAA to the address       */
    Temp  = (UINT8 *)(offset + 0x2AAA); /* set up address to be A000:2AAAh      */
    XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
    Temp  = (UINT8 *)(offset + 0x5555); /* set up address to be A000:5555h      */
    XBYTE[Temp] = 0x80;                   /* write data 0x80 to the address       */
    Temp  = (UINT8 *)(offset + 0x5555); /* set up address to be A000:5555h      */
    XBYTE[Temp] = 0xAA;                   /* write data 0xAA to the address       */
    Temp  = (UINT8 *)(offset + 0x2AAA); /* set up address to be A000:2AAAh      */
    XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address       */
    Temp  = Dst;                    /* set up starting address to be erased */
    XBYTE[Temp] = 0x30;                   /* write data 0x30 to the address       */

	for( i = 0 ; i < 3 ; i++ ){ 
    	isp10MilliSecondsDelay();      /* check DATABOOK for the most  */
    }                                /* accurate value -- Tsce       */
                                    /* accurate value -- Tse        */
	return TRUE;										
}

/************************************************************************/
/* PROCEDURE:   ispOneSectorProgram                                      */
/*                                                                      */
/* This procedure can be used to program a total of 4096 bytes of date  */
/* to the SST's 39VF040.                                                */
/*                                                                      */
/* Input:                                                               */
/*           Src     SOURCE address containing the data which will be   */
/*                   written to the 39VF040.                            */
/*           Dst     DESTINATION address which will be written with the */
/*                   data passed in from Src                            */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/
void 
ispOneSectorProgram (
	UINT32 offset,
	UINT8 *Src,    
	UINT8 *Dst
)
{
	printf("ispOneSectorProgram\n");
    UINT8 *Temp;
    UINT8 *SourceBuf;
    UINT8 *DestBuf;
	UINT32 index;

    SourceBuf = Src;
    DestBuf = Dst;

    ispOneSectorErase(offset,Src);          /* erase the sector first */

    for (index = 0; index < SECTOR_SIZE; index++){
        Temp =  (UINT8 *)(offset + 0x5555); /* set up address to be A000:555h           */
        XBYTE[Temp] = 0xAA;                   /* write data 0xAA to the address           */
        Temp =  (UINT8 *)(offset + 0x2AAA); /* set up address to be A000:2AAAh          */
        XBYTE[Temp] = 0x55;                   /* write data 0x55 to the address           */
        Temp =  (UINT8 *)(offset + 0x5555); /* set up address to be A000:5555h          */
        XBYTE[Temp] = 0xA0;                   /* write data 0xA0 to the address           */
        Temp = DestBuf;                 /* save the original Destination address    */
        XBYTE[DestBuf] =XBYTE[SourceBuf];      /* transfer data from source to destination */
		DestBuf ++;
		SourceBuf ++;
        ispToggleReadyCheck(Temp);       /* wait for TOGGLE bit to get ready         */
    }
}
#endif

/************************************************************************/
/* PROCEDURE:    ispToggleReadyCheck                                     */
/*                                                                      */
/* During the internal program cycle, any consecutive read operation    */
/* on DQ6 will produce alternating 0's and 1's i.e. toggling between    */
/* 0 and 1. When the program cycle is completed, DQ6 of the data will   */
/* stop toggling. After the DQ6 data bit stops toggling, the device is  */
/* ready for next operation.                                            */
/*                                                                      */
/* Input:                                                               */
/*           Dst        must already set-up by the caller               */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/
void 
ispToggleReadyCheck (
	UINT32 Dst
)
{
	UINT8 Loop = TRUE;
	UINT8 PreData;
	UINT8 CurrData;
	UINT32 TimeOut = 0;
	PreData = XBYTE[(UINT16)Dst];
	PreData = PreData & 0x40;
	//CurrData = !PreData;
	while ((TimeOut < 0x07FFFFFF) && (Loop)){
		CurrData = XBYTE[(UINT16)Dst];
		CurrData = CurrData & 0x40;
		//printf("%bx %bx\n",	CurrData,PreData);
		if (PreData == CurrData){
			Loop = FALSE;   /* ready to exit the while loop */
		}
		PreData = CurrData;
		TimeOut++;
	}
	if(TimeOut > 0x07FFFFFF ){
			printf("Toggle Check Time Out, %bx %bx\n",CurrData,PreData);
	} 
}

#if 0
/************************************************************************/
/* PROCEDURE:   ispDataPollingCheck                                      */
/*                                                                      */
/* During the internal program cycle, any attempt to read DQ7 of the    */
/* last byte loaded during the page/byte-load cycle will receive the    */
/* complement of the true data.  Once the program cycle is completed,   */
/* DQ7 will show true data.                                             */
/*                                                                      */
/* Input:                                                               */
/*           Dst        must already set-up by the caller               */
/*           True       Datathis is the original (true) data            */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/
void 
ispDataPollingCheck (
	UINT8 *Dst,       
	UINT8 TrueData
)
{
	UINT32 Loop = TRUE;
	UINT8 CurrData;
	UINT32 TimeOut = 0;

	TrueData = TrueData &  0x80;
	while ((TimeOut< 0x07FFFFFF) && (Loop)){
		CurrData = XBYTE[Dst];
		CurrData = CurrData & 0x80;
		if (TrueData == CurrData){
			Loop = FALSE;   /* ready to exit the while loop  */
		}
		TimeOut++;
	}
}
#endif
#endif
