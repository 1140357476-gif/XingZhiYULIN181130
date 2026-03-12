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
//sfr     RAMPAGE = 0x9B;
sfr     RAMPAGE = 0x9B;
#define romAddr 0x8000
#define ISP_PRINT_PORGRESS 1

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

UINT32 ispNorIdCheck(UINT32 offset);
UINT32 ispNorEntireChipErase(UINT32 offset);
void ispNorOneByteProgram (UINT32 offset,UINT8 SrcByte,UINT32 Dst);
extern xdata UINT32 G_Firmware_Addr;
extern xdata UINT32 G_Firmware_Size;

static UINT8 xdata sDiskType = (UINT8)DEV_TYPE_DISK_NOR;

//-----------------------------------------------------------------------------
//sp1kDiskTypeGet
//-----------------------------------------------------------------------------

UINT8 sp1kDiskTypeGet(void)
{
	return sDiskType;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: norFwProgram                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8
norFwProgram(
	void
)
{
	UINT8 buf[2];
	UINT32 index1,index2, ret, tmp1, tmp2;
	UINT32 dramAddr = G_Firmware_Addr;
	UINT32 step;
	UINT8 progress = 0;

	/* Get ID */
	printf("norFwProgram\n");
	RAMPAGE = 0;
	XBYTE[REG_Cpu_RamEn] = 0x01;  //enable ram to rom page 
	ret = ispNorIdCheck((UINT32)romAddr);
	if( ret == TRUE ){
		printf("Device check correct\n");
	}
	else {
		printf("Device ID check error ...\n");
		/*return;*/
	}
	/* chip-Erase */
	ispNorEntireChipErase((UINT32)romAddr);
	printf("Erase Done\n");
	
	printf("Start to program\n");
	dramAddr = (UINT32)dramAddr;
	printf("DramAddr = %lx\n",dramAddr);
	SDRAM_CardSetStartAddress(dramAddr, TRUE);
	M_DRAM_ReadWord(buf[0],buf[1]);
	printf("1st [0x%2x][0x%2x]\n",buf[0],buf[1]);
	SDRAM_CardSetStartAddress(dramAddr + (G_Firmware_Size - 2) >> 1, TRUE);
	M_DRAM_ReadWord(buf[0],buf[1]);
	printf("last[0x%2x][0x%2x]\n",buf[0],buf[1]);
	#ifdef ISP_PRINT_PORGRESS
	step = G_Firmware_Size / 100;
	printf("Progress =   0%%");
	#endif
	SDRAM_CardSetStartAddress(dramAddr, TRUE);
	tmp1 = (G_Firmware_Size + 32*1024L - 1) / (32 * 1024L);
	//printf("Total %lx 32KB\n",tmp1);
	for(index2 = 0; index2 < tmp1; index2 ++) {
		if( G_Firmware_Size <  32*1024L ){
			tmp2 = G_Firmware_Size;	
		}
		else{
			tmp2 = 32 * 1024L;	
		}
		for(index1 = 0; index1 < tmp2; index1 = index1 + 2) {
			#ifdef ISP_PRINT_PORGRESS
			if (((index1 + index2 * 32 * 1024L ) % step) == 0) {
				printf("\b\b\b\b%3bd%%",progress);
				progress++;
				//ispPercentSet(progress);
			}
			#endif
			M_DRAM_ReadWord(buf[0],buf[1]);
			ispNorOneByteProgram((UINT32)romAddr, buf[0], (UINT32)(romAddr + index1));
			ispNorOneByteProgram((UINT32)romAddr, buf[1], (UINT32)(romAddr + index1 + 1));
		}
		RAMPAGE ++;
	}
	return TRUE;
}
#endif
