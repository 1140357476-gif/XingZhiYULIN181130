/**************************************************************************
 *
 *      Copyright (c) 2005-2007 by Sunplus mMedia Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus mMedia
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Technology Co., Ltd.
 *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *
 **************************************************************************/

/**
 * @file		hal_cpu.c
 * @brief		Hal API for CPU module
 * @author		Allen Lin
 * @since		2007-12-18
 * @date		2007-12-18
 */
//=============================================================================
//Header file
//=============================================================================
#define __FILE __FILE_ID_HAL_CPU__
#include "general.h"
#include "reg_def.h"
#include "hal.h"
#include "dbg_def.h"
#include "dbg_file_list.h"
#include "reg_def.h"

//=============================================================================
//Symbol
//=============================================================================

typedef struct cpuRamMap_s {
	UINT8 mapEn;
	UINT8 bakReg[8];
} cpuRamMap_t;
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

static cpuRamMap_t xdata sCpuRamMap _at_ 0x1f00;

static logBank_t xdata sCpuImemSdBank;
static UINT16 xdata sCpuImemSdAddr;

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
sfr RamPage = 0x9B;
//sfr MemStretch = 0x8E;

//=============================================================================
//Program
//=============================================================================
#if 0
//-----------------------------------------------------------------------------
//HAL_InitCPU
//-----------------------------------------------------------------------------
/**
 * @brief	Cpu interface hardware initialization
 * @param	CamMode	: Camera operation mode
 * @retval	HALK_SUCCESS : 0 is success,1 is fail.
 * @see
 * @author	Allen Lin
 * @since	2007-12-18
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 HAL_InitCPU(UINT8 CamMode) USING_0
{
        //PRINT_HAL("        HAL_InitCPU: Enter HAL_InitCPU\n");

        if(CamMode > 6) return 0x01;

        if(CamMode == 0)
        {
                MemStretch = 0x00;                        // memory stretch = 0
                XBYTE[REG_Cpu_MemEn] = 0x0F;                     // rampageen, rompageen, SRAM enable
                XBYTE[0x2024] = 0x01;                     // CPU clock
                if(HALK_CPUInternal == 1)
                {
                        XBYTE[REG_Cpu_P1oeSel] = HALK_P1OE;               // P1 & P3 are always drive
                        XBYTE[REG_Cpu_P3oeSel] = HALK_P3OE;
                        //P1 = 0xff;
                        P3 = 0xff;
                }
                else
                {
                        XBYTE[REG_Cpu_P1oeSel] = 0x00;
                        XBYTE[REG_Cpu_P3oeSel] = 0x00;
                }
                if(HALK_CPUP1 == 1)
                        XBYTE[0x201A] = (XBYTE[0X201A] & 0x0f); // P1 is used for GPIO
                else
                        XBYTE[0x201A] = (XBYTE[0X201A] | 0x10);

                RamPage = 0x00;
        }

        //PRINT_HAL("        HAL_InitCPU: Exit HAL_InitCPU\n");

        return HALK_SUCCESS;
}
#endif
/**
 * @brief	wait for signal event.
 * @brief	High bank 4k sram mode selection
 * @param	Mode 	0: addressible SRAM (0x1000 - 0x1FFF)
              1: access data port (0x2C10) (0x0000 - 0x0FFF)
              2: used for DMA buffer (0x0000 - 0x0FFF)
 * @retval	0 is success,1 is fail.
 * @see
 * @author	Allen Lin
 * @since	2007-12-18
*/
/* Pre: HAL_Set4KSRAMMode */
UINT8 HAL_CpuSramModeSet(UINT8 Mode) USING_0
{
	UINT8 tmp0;
	UINT8 status;

	ENTER_CRITICAL( status );
	tmp0 = XBYTE[REG_Cpu_MemEn];
	XBYTE[REG_Cpu_MemEn] = tmp0 & 0x1d;

	//PRINT_HAL("        HAL_CpuSramModeSet: Enter HAL_CpuSramModeSet\n");

	if(Mode == 0)        XBYTE[REG_Cpu_SramMod] = 0x00;
	else if(Mode == 1)   XBYTE[REG_Cpu_SramMod] = 0x11;
	else if(Mode == 2)   XBYTE[REG_Cpu_SramMod] = 0x12;
	else                {XBYTE[REG_Cpu_MemEn] = tmp0 | 0x02; EXIT_CRITICAL( status );return 0x01;}

	XBYTE[REG_Cpu_MemEn] = tmp0 | 0x02;
	EXIT_CRITICAL( status );
	//PRINT_HAL("        HAL_CpuSramModeSet: Exit HAL_CpuSramModeSet\n");

        return SUCCESS;
}

/**
 * @fn        UINT8 HAL_CpuRamMapSet(UINT32 dramAddrWord, UINT16 mapSizeByte, xdata* ppMem)
 * @brief     set the cpu ram mapping
 * @param     [in] dramAddrWord
 * @param     [in] mapSizeByte
 * @param     [in] ppMem
 * @retval    return = SUCCESS / FAIL
 * @see       HAL_CpuRamMapRcv
 * @author    Phil Lin
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 HAL_CpuRamMapSet(UINT32 dramAddrWord, UINT16 mapSizeByte, void xdata* xdata* ppMem)
{
#define MAP_BLK_SIZE 0x2000 //8KB
#define MAP_SIZE_MAX 0x8000 //32KB

	UINT32 mapSWord; //mapping start address
	UINT8 blkCnt, i;
	UINT8 xdata* ptr;
	UINT16 noAlmSize;

	/* check the mapping size */
	noAlmSize = ((dramAddrWord << 1) & (MAP_BLK_SIZE - 1));
	mapSizeByte += noAlmSize;
	if (mapSizeByte > MAP_SIZE_MAX) {
		//ASSERT(0, 1);
		return FAIL;
	}

	/* if already mapped */
	if (sCpuRamMap.mapEn) {
		HAL_CpuRamMapRcv();
	}
	
	/* check the mapping start address */
	mapSWord = dramAddrWord & ~((MAP_BLK_SIZE >> 1) - 1);
	blkCnt = mapSizeByte / MAP_BLK_SIZE;
	if (mapSizeByte & (MAP_BLK_SIZE - 1)) blkCnt++;
	if (blkCnt > 4 || !blkCnt) {
		//ASSERT(0, 1);
		return FAIL;
	}
	
	XBYTE[REG_Cpu_RamEn] &= ~0xf0; //disable dram mapping
	
	/* save the mapping setting */
	ptr = (UINT8 xdata*)0x2C34;
	for (i = 0; i < 8; i++) {
		sCpuRamMap.bakReg[i] = *ptr++;
	}

	/* set the new mapping address */
	mapSWord = (mapSWord << 1) / MAP_BLK_SIZE;
	ptr = (UINT8 xdata*)0x2C34;
	ptr += ((4 - blkCnt) << 1);
	for (i = 0; i < blkCnt; i++) {
		*ptr++ = READ8(mapSWord, 3);
		*ptr++ = READ8(mapSWord, 2);
		mapSWord++;
	}

	/* pop the mapping address */
	if (ppMem) {
		*ppMem = (void*)(0x8000 + MAP_BLK_SIZE * (4 - blkCnt) + noAlmSize);
	}

	XBYTE[REG_Cpu_RamEn] |= 0xf0; //enable dram mapping
	sCpuRamMap.mapEn = 1;
	
	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_CpuRamMapRcv(void)
 * @brief     recover the cpu ram mapping
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       HAL_CpuRamMapSet
 * @author    Phil Lin
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 HAL_CpuRamMapRcv(void)
{
	UINT8 i;
	UINT8 xdata* ptr;
	
	if (!sCpuRamMap.mapEn) {
		return FAIL;
	}

	XBYTE[REG_Cpu_RamEn] &= ~0xf0; //disable dram mapping
	
	/* recover the mapping setting */
	ptr = (UINT8 xdata*)0x2C34;
	for (i = 0; i < 8; i++) {
		*ptr++ = sCpuRamMap.bakReg[i];
	}

	XBYTE[REG_Cpu_RamEn] |= 0xf0; //enable dram mapping
	sCpuRamMap.mapEn = 0;

	return SUCCESS;
}


/**
 * @fn		UINT8 HAL_CpuRomSdToDram(void)
 * @brief	Move code from rom to dram
 * @param	NULL
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
/* Pre: Hal_RomShadowToDram */
UINT8 HAL_CpuRomSdToDram(void) USING_0
{
	/* REMOVED by Phil */
	return SUCCESS;
}

/**
 * @fn		UINT8 HAL_CpuDramSdToImem(void)
 * @brief	Move code from dram to imem
 * @param	NULL
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
/* Pre: Hal_DramShadowToImem */
UINT8 HAL_CpuDramSdToImem(void) USING_0
{
	return SUCCESS;
}

/**
 * @fn		void HAL_CpuImemSdGet(logBank_t *Bank, UINT16 *Addr)
 * @brief	Get Imem shadow bank and address
 * @param	*Bank: the logic bank number
            *Addr: the common bank begin with 0x0000 to 0x7fff, others from 0x8000 to 0xffff
 * @retval	None
 * @see
 * @author	Phil Lin
 * @since	2008-07-24
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
/* Pre: Hal_ImemShadowGet */
void HAL_CpuImemSdGet(logBank_t *Bank, UINT16 *Addr)
{
	(*Bank) = sCpuImemSdBank;
	(*Addr) = sCpuImemSdAddr;
}

/**
 * @fn		UINT8 HAL_CpuImemSdSet(logBank_t Bank, UINT16 Addr)
 * @brief	Move code from dram to imem
 * @param	Bank: the logic bank number
            Addr: the common bank begin with 0x0000 to 0x7fff, others from 0x8000 to 0xffff
 * @retval	SUCCESS /FAIL
 * @see
 * @author	Phil Lin
 * @since	2008-07-24
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
/* Pre: Hal_ImemShadow */
UINT8 HAL_CpuImemSdSet(logBank_t Bank, UINT16 Addr)
{
	UINT8 ret = SUCCESS;
	UINT8 sts;
	UINT8 log34, log35, log01;
	const UINT16 codeBankSize = 0x8000;
	const UINT16 ramBankSize = 0x2000;
	const UINT16 imemSize = 0x800;
	UINT16 i;
	UINT8 xdata* idata pSrc;
	UINT8 xdata* idata pDst;
	UINT32 phyAddr;
	UINT16 ramMapBank;
	UINT16 romMapBank;
	UINT16 ramOffset;

	Addr &= ~(imemSize - 1);
		
	if (Bank) {
		if (Bank >= LOG_BANK_MAX) {
			ret |= FAIL;
		}
		/* not common bank */
		if (Addr < codeBankSize) {
			ret |= FAIL;
		} else {
			phyAddr = (UINT32)codeBankSize * (UINT32)(Bank - 1) + (UINT32)Addr;
		}
	} else {
		/* common bank */
		if (Addr >= codeBankSize) {
			ret |= FAIL;
		} else {
			phyAddr = (UINT32)Addr;
		}
	}

	if (ret != SUCCESS) {
		return ret;
	}

	sCpuImemSdBank = Bank;
	sCpuImemSdAddr = Addr;
	/*
		Enter critical? why ?
		Because we should use the 0x8000 ~ 0xffff ram mapping area,
		and the area may be use by interrupt.
	*/
	ENTER_CRITICAL(sts);
	//printf("phyAddr: %lx\n", phyAddr);
	/* get block num */
	ramMapBank = (phyAddr & ~((UINT32)ramBankSize - 1)) / (UINT32)ramBankSize;
	ramOffset = phyAddr % (UINT32)ramBankSize;
	if ((UINT32)(phyAddr % (UINT32)codeBankSize) + (UINT32)imemSize > (UINT32)codeBankSize) {
		/* the Rom mapping area is in the bottom 2K(IMEM size) in Ram mapping area(32K)*/
		ramOffset = ramBankSize - imemSize;
		sCpuImemSdAddr &= ~(imemSize - 1);
	}
	//printf("ramOffset: %lx\n", (UINT32)ramOffset);
	
	XBYTE[REG_Cpu_MemEn] |= 0x0F;	/* Enable imem2k function */
	XBYTE[REG_Cpu_ShadowEn] &= ~0x01;	/* Shadow imem disable */

	log01 = XBYTE[REG_Cpu_RamEn];
	XBYTE[REG_Cpu_RamEn] = 0x10;	/* RAM to DRAM page enable */

	/* save setting */
	log34 = XBYTE[REG_Cpu_Ram2DramP0L];
	log35 = XBYTE[REG_Cpu_Ram2DramP0H];

	XBYTE[REG_Cpu_Ram2DramP0L] = READ8(ramMapBank, 1);	/*(unit:8K) will mapping to RAM space */
	XBYTE[REG_Cpu_Ram2DramP0H] = READ8(ramMapBank, 0);
	//printf("ramMapBank: %u\n", ramMapBank);
	
	/* Copy the content  to imem area (0x3000-0x3FFF) */

	//dbgTimerStart();
	pSrc = (UINT8 xdata*)(0x8000 + ramOffset);
	pDst = (UINT8 xdata*)0x3000;
	i = imemSize;
	//printf("i: %x\n", i);
	do {
		*pDst++ = *pSrc++;
	} while (--i);
	//dbgTimerStop();
	//printf("done\n");

	romMapBank = (phyAddr & ~((UINT32)imemSize - 1)) / (UINT32)imemSize;
	XBYTE[REG_Cpu_ShadowImemL] = READ8(romMapBank, 1);	/* ROM space mapping to imem */
	XBYTE[REG_Cpu_ShadowImemH] = READ8(romMapBank, 0);
	XBYTE[REG_Cpu_ShadowEn] |= 0x01;	/* Shadow imem enable */

	XBYTE[REG_Cpu_Ram2DramP0L] = log34;
	XBYTE[REG_Cpu_Ram2DramP0H] = log35;
	XBYTE[REG_Cpu_RamEn] = log01;

	EXIT_CRITICAL(sts);
	return ret;
}

#if 0
//-----------------------------------------------------------------------------
//HAL_4KSRAMBufIdx
//-----------------------------------------------------------------------------
/**
 * @brief	4k sram buffer index management
 * @param	Mode	 	0: read the buffer input index & buffer output index
              1: Set start index (from 0x0000 - 0x0FFF)
 * @param	StartIdx	: 4K buffer start index
 * @param	InIdx 	: 4K buffer input index
 * @param	OutIdx 	: 4K buffer output index
 * @retval	HALK_SUCCESS : 0 is success,1 is fail.
 * @see
 * @author	Allen Lin
 * @since	2007-12-18
*/
#if 1
UINT8 HAL_4KSRAMBufIdx(UINT8 Mode, UINT16 StartIdx, UINT16* InIdx, UINT16* OutIdx) USING_0
{
        //PRINT_HAL("        HAL_4KSRAMBufIdx: Enter HAL_4KSRAMBufIdx\n");
        if(Mode!=0)
        {
                XBYTE[REG_Cpu_SramRstIdxL] = (UINT8)StartIdx&0xFF;
                XBYTE[REG_Cpu_SramRstIdxH] = (UINT8)(StartIdx>>8)&0x0F;
				#if 0
                XBYTE[REG_Cpu_SramMod] |= 0x04;
                XBYTE[REG_Cpu_SramMod] &= 0xFB;
				#endif
        }
        *InIdx   = (UINT16)XBYTE[REG_Cpu_SramPioWrIdxL];
        *InIdx  |= (UINT16)XBYTE[REG_Cpu_SramPioWrIdxH]<<8;
        *OutIdx  = (UINT16)XBYTE[REG_Cpu_SramPioRdIdxL];
        *OutIdx |= (UINT16)XBYTE[REG_Cpu_SramPioRdIdxH]<<8;

        //PRINT_HAL("        HAL_4KSRAMBufIdx: Exit HAL_4KSRAMBufIdx\n");

        return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_Write4KSRAMPort
//-----------------------------------------------------------------------------
/**
 * @brief	Write 4K SRAM port
 * @param	Data	: write data
 * @retval	None
 * @author	Allen Lin
 * @since	2007-12-18
*/
void HAL_Write4KSRAMPort(UINT8 Data) USING_0
{
        //PRINT_HAL("        HAL_Write4KSRAMPort: Enter HAL_Write4KSRAMPort\n");

        XBYTE[REG_Cpu_PioDat] = Data;

        //PRINT_HAL("        HAL_Write4KSRAMPort: Exit HAL_Write4KSRAMPort\n");
}

//-----------------------------------------------------------------------------
//HAL_Read4KSRAMPort
//-----------------------------------------------------------------------------
/**
 * @brief	Read 4K SRAM port
 * @param	Data	: read data
 * @retval	None.
 * @see
 * @author	Allen Lin
 * @since	2007-12-18
*/
void HAL_Read4KSRAMPort(UINT8* Data) USING_0
{
        //PRINT_HAL("        HAL_Read4KSRAMPort: Enter HAL_Read4KSRAMPort\n");

        *Data = XBYTE[REG_Cpu_PioDat];

        //PRINT_HAL("        HAL_Read4KSRAMPort: Exit HAL_Read4KSRAMPort\n");
}

#endif /*_DBG_CLR_WARNING_*/



//-----------------------------------------------------------------------------
//HAL_GetCPUStack
//-----------------------------------------------------------------------------
/**
 * @brief	Get the current status of cpu stack
 * @param	*freeStack	: the size of the free stack (Byte)
 * @retval	None
 * @see
 * @author	Allen Lin
 * @since	2007-12-18
*/

void HAL_GetFreeStack(UINT8* freeStack)
{
	UINT8 data* pData = 0xff;

	*freeStack = 0;
	while(*pData++ == 0) {
		(*freeStack)++;
	}
	printf("=> Current Free Stack is %d <=\n", *freeStack);
}
#endif
