/*

Copyright (c) 2007 Sunplus Technology Co., Ltd.

Module Name:

              fileindex.c

Abstract:

              Module related to playback

Environment:

              Keil C51 Compiler

Revision History:

              09/20/2007           suny   created

*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "dosusr.h"
#include "common.h"
#include "initio.h"
#include "fileindex.h"
#include "sdramhw.h"
#include "pb_api.h"
#include "hal_dram.h"
#include "hal_gprm.h"
//=============================================================================
//Constant
//=============================================================================

//#define Dprint printf

//=============================================================================
//Variable
//=============================================================================
//xdata UINT16 LastFileIdx;
//xdata UINT16 FirstFileIdx;

extern xdata UINT16 G_USR_SearchNextFileIndex;
extern xdata UINT16 G_USR_FileCount;
//extern xdata UINT32 G_DOS_FileSize;

//=============================================================================
// DOS_FileOrderTblInit  	经过思考觉得只要保存 file true index就可以了,
//					没有必要保存file-order index
//=============================================================================
void DOS_FileOrderTblInit(void) USING_0
{
	UINT16 i, j;
	UINT16 fileCnt;

	//dbgTimerStart();
	if (!G_USR_FileCount) {
		return;
	}
	
	//CPrintf1("G_USR_FileCount is %u", G_USR_FileCount);
	fileCnt = G_USR_FileCount;
	j = 0;
	
	HAL_DramStartAddrSet(K_SDRAM_FileOrderTableAddr, K_SDRAM_NoPrefetch);

	for (i = 1; i < 10000; i++) {
		if (pbFileInfoGet(i, NULL) == SUCCESS) {
			//printf("-> %u\n", i);
			HAL_DramStartAddrSet(K_SDRAM_FileOrderTableAddr + (UINT32)(j++), K_SDRAM_NoPrefetch);
			HAL_DramWordDataWrite(READ8(i, 1), READ8(i, 0));
			if (!(--fileCnt)) {
				break;
			}
		}
	}
	//dbgTimerStop();
}

//=============================================================================
// DOS_FIleOrderRst
//=============================================================================
void DOS_FIleOrderRst(UINT16 index) USING_0
{
	UINT32 addr;

	if (index <1 || index >G_USR_FileCount) {
		return;
	}

	addr = K_SDRAM_FileOrderTableAddr + (index-1);
	HAL_GprmDramDma((addr +1)<<1, addr<<1, (G_USR_FileCount +1 -index)<<1, 1);

	//Dprint("exit reset fileorder\n");
}

//=============================================================================
// DOS_FileOrderWrite
//=============================================================================
void DOS_FileOrderWrite(UINT16 OrderIndex, UINT16 FileTrueIndex) USING_0
{
	UINT32 addr;

	addr = K_SDRAM_FileOrderTableAddr + /*9999 +*/ (OrderIndex - 1);
	HAL_DramStartAddrSet(addr , K_SDRAM_NoPrefetch);
	HAL_DramWordDataWrite((UINT8) FileTrueIndex & 0xff , (UINT8)(FileTrueIndex>>8) & 0xff);	
	//Dprint("exit write fileindex\n");
}

//=============================================================================
// DOS_FileIndexGet
//=============================================================================
UINT16 DOS_FileIndexGet(UINT16 order) USING_0
{
	UINT16 index;
	UINT32 addr;
	UINT8 tmp1,tmp2;

	if(order > G_USR_FileCount) {
		return 0;
	}
	//printf("order :%d\n",order);
	
	addr = K_SDRAM_FileOrderTableAddr +/* 9999 + */(order - 1);
	HAL_DramStartAddrSet(addr, K_SDRAM_Prefetch);
	HAL_DramWordDataRead(&tmp1, &tmp2);
	index = (UINT16)(((UINT16)tmp2) <<8) +( (UINT16)tmp1);

	//printf("fileindextrue :%d\n",index);
	return index;//
}

//=============================================================================
// DOS_FileLastIndexClear
//=============================================================================
void DOS_FileLastIndexClear(void)USING_0
{
	UINT32 addr;

	addr = K_SDRAM_FileOrderTableAddr + /*9999 + */(G_USR_FileCount + 1 - 1);
	HAL_DramStartAddrSet(addr , K_SDRAM_NoPrefetch);
	HAL_DramWordDataWrite( 0, 0);	
}

//=============================================================================
// DOS_FileIndexClearAll
//=============================================================================
void DOS_FileIndexClearAll(void) USING_0
{
	HAL_GprmDramFill(K_SDRAM_FileOrderTableAddr /*+ 9999*/, (9999L<<1), 0);
}

