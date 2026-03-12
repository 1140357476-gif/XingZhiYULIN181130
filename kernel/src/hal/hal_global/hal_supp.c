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
 * @file		hal_supp.c
 * @brief		Hal API for supplying all modules
 * @author
 * @since		2007-12-18
 * @date		2007-12-18
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "reg_def.h"
#include "hal.h"
#include "main.h"

#include "quantdef.h"
#include "setmode.h"
//#include "dosusr.h"
#include "doslink.h"
#include "smcfunc.h"
#include "smccon.h"

//version4.0@ada@0513 for Card Write Protect
//#include "cardusr.h"

//patch5.1@Add all storage target begin
#include "cardlink.h"
//patch5.1@Add all storage target end

//extern xdata UINT8 zigzagTable[];

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//xdata   UINT8    G_Card_WriteProtect;

//=============================================================================
//Program
//=============================================================================

//-----------------------------------------------------------------------------
//HAL_SRAMLow2High
//-----------------------------------------------------------------------------
/**
 * @brief	Move high bank sram data to specific buffer address
 * @param	siz : transfer size
 * @param	buffer : The specific buffer address
 * @retval	HALK_SUCCESS : 0 is success,1 is fail.
 * @author	Allen
 * @since	2007-12-18
 */
UINT8 HAL_SRAMLow2High(UINT16 size,UINT8* buffer) USING_0
{
	UINT16 i;
	UINT8 status;

	XBYTE[REG_Cpu_SramMod] &= 0xFC;   //High bank sram can is addressed from 0x1000 to 0x1fff

	ENTER_CRITICAL( status );

	for (i = 0; i < size; i++) {
		XBYTE[0x3800 + i] = buffer[i];
	}

	EXIT_CRITICAL( status );

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_SRAMHigh2Low
//-----------------------------------------------------------------------------
/**
 * @brief	Move the data of specific buffer to high bank sram
 * @param	siz : transfer size
 * @param	buffer : The specific buffer address
 * @retval	HALK_SUCCESS : 0 is success,1 is fail.
 * @author	Allen
 * @since	2007-12-18
 */
UINT8 HAL_SRAMHigh2Low(UINT16 size,UINT8* buffer) USING_0
{
	UINT16 i;
	UINT8 status;

	ENTER_CRITICAL( status );

	XBYTE[REG_Cpu_SramMod] &= 0xFC;   //High bank sram can is addressed from 0x1000 to 0x1fff

	for (i = 0; i < size; i++) {
		buffer[i] = XBYTE[0x3800 + i];
	}

	EXIT_CRITICAL( status );

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_CardDetect
//-----------------------------------------------------------------------------
/*
routine description:
		Detect whether storage media card is inserted
arguments:
		detect:
		  0: Card is not inserted
		1: Card is inserted
return value:
		0x00   - success
		others - error
*/
/**
 * @brief	Check if reading data to USB has done
 * @param	size : size of DMA
 * @retval	status : 0 is success,1 is fail.
 * @author	Ted
 * @todo
 * @since	2007-12-18
 */
#if 0
UINT8 HAL_CardDetect(UINT8* detect) USING_0
{
	*detect = 1;
	G_Card_WriteProtect = 0;
	return HALK_SUCCESS;
}
#endif
