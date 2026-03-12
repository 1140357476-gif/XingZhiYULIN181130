/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  hal_pb.c
 * @brief	  pb hal function
 * @author	  Tq.zhang
 * @since	  2010-04-28
 * @date	  2010-04-28
*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal_pb.h"
#include "hal_dram.h"
#include "reg_def.h"

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

/***************************************/
/*									   */
/*		   Still Play back Function    */
/*									   */
/***************************************/

/**
 * @fn		  void HAL_PbBufAddrSet(UINT32 addrWord)
 * @brief	  set Play back buffer word address and sync all pb parameter to disp sof
 * @param	  [in] addrWord : play back image buffer address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_PbBufAddrSet(UINT32 addrWord)
{
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x20;

	XBYTE[REG_Dram_PBBAddr_B0] = READ8(addrWord, 3);
	XBYTE[REG_Dram_PBBAddr_B1] = READ8(addrWord, 2);
	XBYTE[REG_Dram_PBBAddr_B2] = READ8(addrWord, 1);
	XBYTE[REG_Dram_PBBAddr_B3] = READ8(addrWord, 0);

	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);

	return;
}

/**
 * @fn		  void HAL_PbBufAddrGet(UINT32* addrWord)
 * @brief	  get play back buffer word address
 * @param	  [in] addrWord
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_PbBufAddrGet(UINT32* addrWord)
{
	*((UINT8*)addrWord + 3) = XBYTE[REG_Dram_PBBAddr_B0];
	*((UINT8*)addrWord + 2) = XBYTE[REG_Dram_PBBAddr_B1];
	*((UINT8*)addrWord + 1) = XBYTE[REG_Dram_PBBAddr_B2];
	*((UINT8*)addrWord + 0) = XBYTE[REG_Dram_PBBAddr_B3];
	return;
}

/**
 * @fn		  void HAL_PbBufHsizeSet(UINT16 width)
 * @brief	  set Play back buffer size
 * @param	  [in] width : play back image buffer size(pixel size)
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_PbBufHsizeSet(UINT16 width)
{

	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x20;
	XBYTE[REG_Dram_PBBHSize_Low] = READ8(width, 1);
	XBYTE[REG_Dram_PBBHSize_High] = READ8(width, 0);
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB); // For update display controller registers
	return;
}

/**
 * @fn        void HAL_PbSyncPara2DispSof(void)
 * @brief     Sync all playback related parameters to Display  sof
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
void HAL_PbSyncPara2DispSof(void)
{
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
}

/***************************************/
/*									   */
/*		   Video Play back Function    */
/*									   */
/***************************************/
