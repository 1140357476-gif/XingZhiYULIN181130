/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      hal_dmac.c
 * @brief     Hal function for DMA module
 * @author    Allen
 * @since     2010-04-25
 * @date      2010-04-25
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_HAL_DMAC__ 
#include "general.h"
#include "hal.h"
#include "gpio.h"
#include "dbg_def.h"
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DMA_TIME_OUT_REF     0x08  //8 * 0x10000 * 10us = 2.6s ~ 2.6*2s
#define DMA_TIME_OUT_MASK    0x0C 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DMA_TIME_OUT 0x10000
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/*
	Using the global variables type if "code" and "idata" to avoid the sram mode switch issue
*/
static const UINT8 code sDmaRamSel[3] = {0x12, 0x22, 0x02};
static UINT8 idata sDmaTimeout;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void HAL_DmaWait(void)
 * @brief     HAL_DmaWait
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-10-13
 * @todo      N/A
 * @bug       N/A
*/
#pragma OT(8, SPEED)
static void HAL_DmaWait(void)
{
	/*
		Phil: This function linked to other bank(maybe not the same as hal_dmac.c)
		so, the variables type in "code" or const data could NOT be used by this function!
	*/
	
	while(!XBYTE[REG_Dma_DmaCmp]) {
		/* check the timeout */
		if((XBYTE[0x20C3] & DMA_TIME_OUT_MASK) == sDmaTimeout) {
			sDmaTimeout = -1;
			break;
		}
	}
}

/**
 * @fn		UINT8 HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SRAMmode)
 * @brief	For different sources DMA.
 * @param	Src : the source of DMA
				0: DRAM
				1: CPU SRAM 
				2: Storage Media
				4: USB
				5: DMA data port (0x2300)
 * @param	Dst : the destination of DMA
				0: DRAM
				1: CPU SRAM
				2: Storage Media
				4: USB
				5: DMA data port (0x2300)
 * @param	nByte	: the number of byte in the DMA data transfer (tha maximum size is 2MBytes)
 * @param	Mode	:
				0: the function return after the DMA is completed
				1: the function return immediately after the DMA is trigger (SRAM client is restricted)
 * @param	SramMode	:
				0: 1K SRAM 	(0x3800~0x3FFF)
				1: IMEM		(0x3000~0x37FF)
				2: 8K SRAM 	(0~0x1FFF)
 * @retval		:
				0: Success
				1: Fail.
 * @see
 * @author	Allen Lin
 * @since	2010-04-25
*/
#if 1

#pragma OT(8, SPEED)
UINT8 HAL_DmaDo(UINT8 Src, UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SramMode)
{
	BIT bEA_bak; //Using the 8051 internal memory to avoid the sram mode switch issue
	BIT bSram; //indicate if DMA from/to SRAM

	/* save the interrupt option */
	bEA_bak = EA;
	EA = 0;
	
	dbgWdtFeed(10000); //xian ++, timeout = 5sec?
	/* set the DMA path */
	XBYTE[REG_Dma_DmaSrcDst] = (Dst << 4) | Src;

	/* reset DMA machine */
	XBYTE[REG_Dma_DmaIdle] = 0x01;   
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	/* clear DMA complete status */
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* speed up the UINT32 subtract */
	if (READ16(nByte, 0)) {
		nByte--;
	} else {
		WRITE16(nByte, 2, READ16(nByte, 2) - 1); //treate the "nByte" as an "UINT16" int.
	}

	/* The DMA size is extended to 2MBytes */
	XBYTE[REG_Dma_DmaSizeL] = READ8(nByte, 3);
	XBYTE[REG_Dma_DmaSizeM] = READ8(nByte, 2);
	XBYTE[REG_Dma_DmaSizeH] = READ8(nByte, 1);

	/* configure the path for sram mode */
	bSram = 0;
	if (XBYTE[REG_Dma_DmaSrcDst] & 0x11) {
		/*
			SRAMmode: 0, 1, 2=1K, 2K, 8K.
			SRAM sel: 0, 1, 2=8K, 1K, 2K.

			Note: We CAN NOT using any local variables or global variables located at (0x0000~0x1fff).
			because the sram access mode had been switched to DMA mode.
		*/
		XBYTE[REG_Cpu_SramMod] = sDmaRamSel[SramMode];
		bSram = 1;
	}

	/* Trigger DMA */
	XBYTE[REG_Dma_DmaStart] = 0x01;

	if (!bSram) {
		/* if it's Not DMA from/to SRAM, restore the interrupt option here */
		EA = bEA_bak;

		/* if the hit-run mode enabled */
		if (Mode) {
			return SUCCESS;
		}
	}

	/* record the current time */
	sDmaTimeout = XBYTE[0x20C3];
	sDmaTimeout += DMA_TIME_OUT_REF; 
	sDmaTimeout &= DMA_TIME_OUT_MASK;

	/* wait DMA ready */
	HAL_DmaWait();

	/* clear the DMA complete status */
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* enable the interrupt after DMEM DMA done */
	if (bSram) {
		XBYTE[REG_Cpu_SramMod] = 0x00; // set sram as normal operation
		EA = bEA_bak;
	}

	/* check if timeout */
	if(sDmaTimeout == -1) {
		ASSERT(0, 1);
		dbgWdtFeed(10000); //xian ++, timeout = 5sec?
		printf("path=%bx, c=%lx\n", XBYTE[REG_Dma_DmaSrcDst], nByte);
		XBYTE[REG_Dma_DmaIdle] = 0x01; // reset DMA machine
		XBYTE[REG_Dma_DmaIdle] = 0x00;
		return FAIL;
	}
	
	return SUCCESS;
}
#else
UINT8 HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SramMode)
{
	UINT8 sramflag; 	
	UINT8 status;
	unsigned long remain_time = DMA_TIME_OUT;

	ENTER_CRITICAL(status);

	if( Src == 0 && Dst == 2 ){
		XBYTE[REG_Dma_DmaSrcDst] = 0x20;
	}
	else if( Src == 2 && Dst == 0 ){
		XBYTE[REG_Dma_DmaSrcDst] = 0x02;
	}
	else{
		XBYTE[REG_Dma_DmaSrcDst] = (Dst<<4)|Src;
	}

	/* reset DMA machine */
	XBYTE[REG_Dma_DmaIdle] = 0x01;   
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	nByte--;
	/* The DMA size is extended to 2MBytes */
	XBYTE[REG_Dma_DmaSizeL] = READ8(nByte, 3);
	XBYTE[REG_Dma_DmaSizeM] = READ8(nByte, 2);
	XBYTE[REG_Dma_DmaSizeH] = READ8(nByte, 1);

	sramflag = 0;

	if(Src==0x01 || Dst==0x01)
	{
		XBYTE[REG_Cpu_MemEn] |= 0x07;	  	// enable sram
		/* SRAMmode: 0, 1, 2=1K, 2K, 8K. */
		if (SramMode == 0){
			XBYTE[REG_Cpu_SramMod] = 0x12;
		}
		else if (SramMode == 1){
			XBYTE[REG_Cpu_SramMod] = 0x22;
		}
		else if (SramMode == 2){
			XBYTE[REG_Cpu_SramMod] = 0x02;
		}
		sramflag = 1;
	}

	/* Trigger DMA */
	XBYTE[REG_Dma_DmaStart] = 0x01;

	EXIT_CRITICAL(status);  //xian move here 20090717
	
	if (Mode)
	{
		return SUCCESS;
	}
	nByte++;
	if(nByte==0){	//64KB
		remain_time = remain_time * 256;
	}
	else{
		remain_time += remain_time * READ8(nByte,0);
	}
	while(remain_time)
	{
		if(XBYTE[REG_Dma_DmaCmp] != 0)// wait for DMA complete
			break;
		remain_time--;
//#if (FPGA_VERIFY != 1)
		if( Src == 2 || Dst == 2 ){ //dma from or to storage
			if( XBYTE[REG_Fm_Mediatype_rst] == 0x06 ){// it is SD/EMMC card
				{
					if ( HAL_GpioByteInGet(G_Detect_PinGrp, G_Detect_PinMux) ) {
						remain_time = 0;
						break;
					}
				}
			}
		}
//#endif        
	}

	XBYTE[REG_Dma_DmaCmp] = 0x01;

	 if (sramflag == 1)
	 {
		  XBYTE[REG_Cpu_SramMod]  = 0x00;    	// set sram as normal operation
	 }
	if(remain_time==0)
	{
		printf("%s(%d), d1, c=%u\n", __FILE__, (UINT16)__LINE__, nByte);
		XBYTE[REG_Dma_DmaIdle] = 0x01;                                // reset DMA machine
		XBYTE[REG_Dma_DmaIdle] = 0x00;
		return FAIL;
	}
	return SUCCESS;
}

#endif
/**
 * @fn		void HAL_DmaReset(void)
 * @brief	Reset DMA controller
 * @param	None
 * @retval	None
 * @author	Allen Lin
 * @since	2007-12-18
*/
void HAL_DmaReset(void)
{
	XBYTE[REG_Dma_DmaIdle] = 0x01;
	XBYTE[REG_Dma_DmaIdle] = 0x00;
}

//-----------------------------------------------------------------------------
//HAL_DmaReadDramAddr
//-----------------------------------------------------------------------------
/**
 * @brief	Read DRAM DMA address
 * @param	DRAMAddrW: DRAM DMA starting address
 * @retval	NONE
 * @author	Allen Lin
 * @since	2007-12-18
*/
void HAL_DmaReadDramAddr(UINT32* DRAMAddrW)
{
	WRITEP8(DRAMAddrW, 3, XBYTE[REG_Dma_BaseAddrL]);
	WRITEP8(DRAMAddrW, 2, XBYTE[REG_Dma_BaseAddrML]);
	WRITEP8(DRAMAddrW, 1, XBYTE[REG_Dma_BaseAddrMH]);
	WRITEP8(DRAMAddrW, 0, XBYTE[REG_Dma_BaseAddrH]);
}

//-----------------------------------------------------------------------------
//HAL_DmaSetDramAddr
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 HAL_DmaSetDramAddr(UINT32 DRAMAddr)
 * @brief	DRAM DMA setting
 * @param	DRAMAddr: DRAM starting address
 * @retval	: 0 is success,1 is fail.
 * @author	Allen Lin
 * @since	2007-12-18
*/
UINT8 HAL_DmaSetDramAddr(UINT32 DRAMAddr)
{
	UINT8 status;

	ENTER_CRITICAL(status);

	XBYTE[REG_Dma_BaseAddrL] = READ8(DRAMAddr, 3);
	XBYTE[REG_Dma_BaseAddrML] = READ8(DRAMAddr, 2);
	XBYTE[REG_Dma_BaseAddrMH] = READ8(DRAMAddr, 1);
	XBYTE[REG_Dma_BaseAddrH] = READ8(DRAMAddr, 0) & 0x01;

	EXIT_CRITICAL(status);
	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_DmaSetSramAddr(UINT16 SramRstIdx)
 * @brief     HAL_DmaSetSramAddr
 * @param     [in] SramRstIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_DmaSetSramAddr(UINT16 SramRstIdx)
{
	XBYTE[REG_Cpu_SramRstIdxL] = READ8(SramRstIdx, 1);
	XBYTE[REG_Cpu_SramRstIdxH] = READ8(SramRstIdx, 0) & 0x1F;

	return SUCCESS;
}


//-----------------------------------------------------------------------------
//INTR_HAL_DmaSetDramAddr
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 INTR_HAL_DmaSetDramAddr(UINT32 DRAMAddr)
 * @brief	DRAM DMA setting
 * @param	DRAMAddr: DRAM starting address
 * @retval	: 0 is success,1 is fail.
 * @author	Allen Lin
 * @since	2007-12-18
*/
#pragma disable
UINT8 INTR_HAL_DmaSetDramAddr(UINT32 DRAMAddr)
{
	//UINT8 status;

	//ENTER_CRITICAL(status);

	XBYTE[REG_Dma_BaseAddrL] = READ8(DRAMAddr, 3);
	XBYTE[REG_Dma_BaseAddrML] = READ8(DRAMAddr, 2);
	XBYTE[REG_Dma_BaseAddrMH] = READ8(DRAMAddr, 1);
	XBYTE[REG_Dma_BaseAddrH] = READ8(DRAMAddr, 0) & 0x01;

	//EXIT_CRITICAL(status);
	return SUCCESS;
}

/**
 * @fn        UINT8 INTR_HAL_DmaSetSramAddr(UINT16 SramRstIdx)
 * @brief     INTR_HAL_DmaSetSramAddr
 * @param     [in] SramRstIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable
UINT8 INTR_HAL_DmaSetSramAddr(UINT16 SramRstIdx)
{
	XBYTE[REG_Cpu_SramRstIdxL] = READ8(SramRstIdx, 1);
	XBYTE[REG_Cpu_SramRstIdxH] = READ8(SramRstIdx, 0) & 0x1F;

	return SUCCESS;
}

/**
 * @fn		UINT8 HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SRAMmode)
 * @brief	For different sources DMA.
 * @param	Src : the source of DMA
				0: DRAM
				1: CPU SRAM 
				2: Storage Media
				4: USB
				5: DMA data port (0x2300)
 * @param	Dst : the destination of DMA
				0: DRAM
				1: CPU SRAM
				2: Storage Media
				4: USB
				5: DMA data port (0x2300)
 * @param	nByte	: the number of byte in the DMA data transfer (tha maximum size is 2MBytes)
 * @param	Mode	:
				0: the function return after the DMA is completed
				1: the function return immediately after the DMA is trigger (SRAM client is restricted)
 * @param	SramMode	:
				0: 1K SRAM 	(0x3800~0x3FFF)
				1: IMEM		(0x3000~0x37FF)
				2: 8K SRAM 	(0~0x1FFF)
 * @retval		:
				0: Success
				1: Fail.
 * @see
 * @author	Allen Lin
 * @since	2010-04-25
*/
#pragma disable
UINT8 INTR_HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SramMode)
{
	BIT bEA_bak; //Using the 8051 internal memory to avoid the sram mode switch issue
	BIT bSram; //indicate if DMA from/to SRAM
	UINT8 idata sDmaTimeout;
	
	/* save the interrupt option */
	bEA_bak = EA;
	EA = 0;
	
	//dbgWdtFeed(10000); //xian ++, timeout = 5sec?
	/* set the DMA path */
	XBYTE[REG_Dma_DmaSrcDst] = (Dst << 4) | Src;

	/* reset DMA machine */
	XBYTE[REG_Dma_DmaIdle] = 0x01;   
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	/* clear DMA complete status */
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* speed up the UINT32 subtract */
	if (READ16(nByte, 0)) {
		nByte--;
	} else {
		WRITE16(nByte, 2, READ16(nByte, 2) - 1); //treate the "nByte" as an "UINT16" int.
	}

	/* The DMA size is extended to 2MBytes */
	XBYTE[REG_Dma_DmaSizeL] = READ8(nByte, 3);
	XBYTE[REG_Dma_DmaSizeM] = READ8(nByte, 2);
	XBYTE[REG_Dma_DmaSizeH] = READ8(nByte, 1);

	/* configure the path for sram mode */
	bSram = 0;
	if (XBYTE[REG_Dma_DmaSrcDst] & 0x11) {
		/*
			SRAMmode: 0, 1, 2=1K, 2K, 8K.
			SRAM sel: 0, 1, 2=8K, 1K, 2K.

			Note: We CAN NOT using any local variables or global variables located at (0x0000~0x1fff).
			because the sram access mode had been switched to DMA mode.
		*/
		XBYTE[REG_Cpu_SramMod] = sDmaRamSel[SramMode];
		bSram = 1;
	}

	/* Trigger DMA */
	XBYTE[REG_Dma_DmaStart] = 0x01;

	if (!bSram) {
		/* if it's Not DMA from/to SRAM, restore the interrupt option here */
		EA = bEA_bak;

		/* if the hit-run mode enabled */
		if (Mode) {
			return SUCCESS;
		}
	}

	/* record the current time */
	sDmaTimeout = XBYTE[0x20C3];
	sDmaTimeout += DMA_TIME_OUT_REF; 
	sDmaTimeout &= DMA_TIME_OUT_MASK;

	/* wait DMA ready */
	while(!XBYTE[REG_Dma_DmaCmp]) {
		/* check the timeout */
		if((XBYTE[0x20C3] & DMA_TIME_OUT_MASK) == sDmaTimeout) {
			sDmaTimeout = -1;
			break;
		}
	}

	/* clear the DMA complete status */
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	/* enable the interrupt after DMEM DMA done */
	if (bSram) {
		XBYTE[REG_Cpu_SramMod] = 0x00; // set sram as normal operation
		EA = bEA_bak;
	}

	/* check if timeout */
	if(sDmaTimeout == -1) {
		//ASSERT(0, 1);
		//dbgWdtFeed(10000); //xian ++, timeout = 5sec?
		//printf("path=%bx, c=%lx\n", XBYTE[REG_Dma_DmaSrcDst], nByte);
		XBYTE[REG_Dma_DmaIdle] = 0x01; // reset DMA machine
		XBYTE[REG_Dma_DmaIdle] = 0x00;
		return FAIL;
	}
	
	return SUCCESS;
}
