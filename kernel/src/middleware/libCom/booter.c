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

//=============================================================================
//Header file
//=============================================================================
#define __FILE  __FILE_ID_BOOTER__
#include "general.h"
#include "asicreg.h"
#include "main.h"
#include "sdramhw.h"
#include "smcfunc.h"
#include "hal_dram.h"
#include "dbg_mem.h"
#include "dbg_def.h"
#include "dbg_file_list.h"
#include "reg_def.h"

//=============================================================================
//Symbol
//=============================================================================
void jump_main(void);

//=============================================================================
//Constant
//=============================================================================

/* Defined function "bootMoveCode" in code address */
#define FUNC_BOOT_MOVE_ADDR 0x2000 

//=============================================================================
//Variable
//=============================================================================

//=============================================================================
//Extern 
//=============================================================================
extern xdata	UINT32	G_ISPDRAMAddr;
extern xdata	UINT32	G_BulkSize;

//=============================================================================
//Program
//=============================================================================
/**
 * @fn        void bootMoveCode(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 dmaSizeByte)
 * @brief     bootMoveCode
 * @param     [in] srcAddrByte
 * @param     [in] dstAddrByte
 * @param     [in] dmaSizeByte
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-11-22
 * @todo      N/A
 * @bug       N/A
*/
void 
bootMoveCode(
	UINT32 srcAddrByte,
	UINT32 dstAddrByte,
	UINT32 dmaSizeByte
)
{
	//printf("sAddr=%lx,dAddr=%lx,size=%lx\n", srcAddrByte, dstAddrByte, dmaSizeByte);

	/* sram shadow */
	XBYTE[REG_Cpu_ShadowImemL] = 0x04;/* The offset address 0x2000 */
	XBYTE[REG_Cpu_ShadowImemH] = 0x00;/* The offset address 0x2000 */
	XBYTE[REG_Cpu_ShadowEn] |= 0x01;/* enable IMEM shadow */

	/* move code from dram to dram */
	XBYTE[REG_Gprm_Dma0SrcAddr0] = READ8(srcAddrByte,3); //GPRMSB addr
	XBYTE[REG_Gprm_Dma0SrcAddr1] = READ8(srcAddrByte,2);
	XBYTE[REG_Gprm_Dma0SrcAddr2] = READ8(srcAddrByte,1);
	XBYTE[REG_Gprm_Dma0SrcAddr3] = READ8(srcAddrByte,0);
	
	XBYTE[REG_Gprm_Dma0DstAddr0] = READ8(dstAddrByte,3); //GPRMDB addr
	XBYTE[REG_Gprm_Dma0DstAddr1] = READ8(dstAddrByte,2);
	XBYTE[REG_Gprm_Dma0DstAddr2] = READ8(dstAddrByte,1);
	XBYTE[REG_Gprm_Dma0DstAddr3] = READ8(dstAddrByte,0);

	dmaSizeByte--;
	XBYTE[REG_Gprm_Dma0ByteCnt0] = READ8(dmaSizeByte,3);
	XBYTE[REG_Gprm_Dma0ByteCnt1] = READ8(dmaSizeByte,2);
	XBYTE[REG_Gprm_Dma0ByteCnt2] = READ8(dmaSizeByte,1) & 0x1f;

	XBYTE[REG_Gprm_FillDramEn] &= ~0x01; //dram dma mode 
	XBYTE[REG_Gprm_DmaIntSts] |= 0x01; //clear interrupt status
	
	/* start DMA */
	XBYTE[REG_Gprm_Dma0Start] = 0x01;

	/* wait dram DMA ready */
	while(XBYTE[REG_Gprm_DmaBusy] & 0x01);

	/* jump back to dram "C_C51STARTUP" */
	(*(void(code*)())0x1000)();
}

/**
 * @fn        void bootCore(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT32 bootSizeByte)
 * @brief     bootCore
 * @param     [in] srcAddrWord
 * @param     [in] dstAddrWord
 * @param     [in] bootSizeByte
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-11-22
 * @todo      N/A
 * @bug       N/A
*/
void 
bootCore(
	UINT32 srcAddrWord,
	UINT32 dstAddrWord,
	UINT32 bootSizeByte
)
{
	UINT8 sts;
	
	ENTER_CRITICAL(sts);
	
	XBYTE[REG_Usb_Status] = 0x10; //Disconnect USB, enable SoftPlugOut !!
	XBYTE[REG_Dram_AddressErr] = 0x00; //disable code sentry
	XBYTE[REG_Cpu_ShadowEn] &= 0xfe;//disable IMEM shadow

	/* move function "bootMoveCode" to IMEM (addr:0x3000, size:0x800) */
	HAL_DramDmaRead(0x3000, FUNC_BOOT_MOVE_ADDR  >> 1, 0x800, 1);

	#if 0
	//dumpData(512, 0x3000, 0);
	//dumpData(32, G_ISPDRAMAddr, 1);
	//dumpData(512, 0, 1);
	//printf("bootMoveCode %lx %lx\n",G_ISPDRAMAddr, G_BulkSize);
	#endif

	/* move code to dram address 0 */
	bootMoveCode(srcAddrWord << 1, dstAddrWord, bootSizeByte);

	/* boot error */
	ASSERT(0, 0);

	#if 0
	dumpData(512, 0, 1);
	#endif
}
