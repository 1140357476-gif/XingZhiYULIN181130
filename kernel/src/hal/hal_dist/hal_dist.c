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
 * @file	  hal_dist.c
 * @brief	  dist hal function
 * @author	  Tq.zhang
 * @since	  2010-04-22
 * @date	  2010-04-22
*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal_dist.h"
#include "hal_cdsp_core.h"
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

/**
 * @fn		  void HAL_Dist2ndPathOpen(void)
 * @brief	  open dist second path
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_Dist2ndPathOpen(
	void
)
{
	UINT8 tmp;
	//XBYTE[0x2258] = 0x00; // when 2nd path, it should be 0x00 but not 0x01;
	HAL_Cdsp2ndPathCfg(0,0,0);
	
	tmp=XBYTE[REG_Cdsp_YUVscalesvden];
	XBYTE[REG_Cdsp_YUVscalesvden]=0x00;//update 2ndpathen immediately		
	//XBYTE[0x22c1] |= 0x01;	// enable edge window, or edge window always the same, pvupdate to valid this register
	HAL_Cdsp2ndPathEnSet(1);	/* enable edge window, or edge window always the same, pvupdate to valid this register*/
	XBYTE[REG_Cdsp_YUVscalesvden]=tmp;	//printf("Display controller enabled\n");
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV); // will trig all other modules
}

/**
 * @fn		  void HAL_Dist2ndPathClose(void)
 * @brief	  close dist second path
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_Dist2ndPathClose(
	void
)
{
	UINT8 tmp;
	//XBYTE[0x2258] = 0x01; // when 2nd path, it should be 0x00 but not 0x01;
	HAL_Cdsp2ndPathCfg(1,0,0);
	tmp=XBYTE[REG_Cdsp_YUVscalesvden];
	XBYTE[REG_Cdsp_YUVscalesvden]=0x00;//update 2ndpathen immediately		
	//XBYTE[0x22c1] &= 0xfe;	// enable edge window, or edge window always the same, pvupdate to valid this register
	HAL_Cdsp2ndPathEnSet(0);	/* enable edge window, or edge window always the same, pvupdate to valid this register*/
	XBYTE[REG_Cdsp_YUVscalesvden]=tmp;	//printf("Display controller enabled\n");
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV); // will trig all other modules
}

/**
 * @fn		  void HAL_DistMeIntervalSet(UINT8 Intval)
 * @brief	  set dist me interval
 * @param	  [in] Intval : Me interval number
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistMeIntervalSet(
	UINT8 Intval
)
{
	XBYTE[REG_Jpeg_Meinterval] = Intval;//Me interval
}

/**
 * @fn		  void HAL_DistMeIntrEn(UINT8 start, UINT8 done)
 * @brief	  enable/disable dist me interrupt
 * @param	  [in] start : enable/disable Me start interrupt
 * @param	  [in] done : enable disable Me done interrupt
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistMeIntrEn(
	UINT8 start,
	UINT8 done
)
{
	XBYTE[REG_Jpeg_Intr_Enable] =  (XBYTE[REG_Jpeg_Intr_Enable]&0x9f) | ((start&0x01)<<5) | ((done&0x01)<<6);// start: enable/disable Me start interrupt; 
														//done:enable disable Me done interrupt
}

/**
 * @fn		  void HAL_DistEn(UINT8 opt)
 * @brief	  enable/disable dist hal module
 * @param	  [in] opt [0]: '1' enable DIST, '0' disable DIST
 * @param		   opt [1]: '1' normal case, '0' diable display Mv for Pb
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistEn(
	UINT8 opt
)
{
	XBYTE[REG_Dram_LensGainUpdt] =  (XBYTE[REG_Dram_LensGainUpdt]&0xdd) | ((opt&0x01)<<1) | ((opt&0x02)<<4);//[0]: '1' enable DIST, '0' disable DIST
													// [1]: '1' normal case, '0' diable display Mv for Pb
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);
}

/**
 * @fn		  void HAL_DistDisable(void)
 * @brief	  disable dist hal module
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistDisable(
)
{
	XBYTE[REG_Dram_LensGainUpdt] &=  ~0x22;//[0]: '1' enable DIST, '0' disable DIST
							// [1]: '1' normal case, '0' diable display Mv for Pb
}


/**
 * @fn		  void HAL_DistSwRst(UINT8 rst)
 * @brief	  software reset dist ME module in JPEG engine 
 * @param	  [in] rst : should set '1' and then '0' in midware self
 * @retval	  NONE'
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistSwRst(
	UINT8 rst
)
{
	XBYTE[REG_Jpeg_Swreset] = (XBYTE[REG_Jpeg_Swreset]&0xfd) | ((rst&0x01)<<1);
}

/**
 * @fn		  void HAL_DistGateClkSet(UINT8 clkOpt, UINT8 sel)
 * @brief	  set gate clock
 * @param	  [in] clkOpt
 * @param	  [in] sel
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistGateClkSet(
	UINT8 clkOpt,
	UINT8 sel
)
{
	if (sel){// select '1'
		XBYTE[REG_Jpeg_DisGatedClk] |= clkOpt;
	}
	else{// select '0'
		XBYTE[REG_Jpeg_DisGatedClk] &= clkOpt;
	}
}

/**
 * @fn		  void HAL_DistBlkNumSet(UINT8 BlkNum)
 * @brief	  set dist edge window block number
 * @param	  [in] BlkNum
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistBlkNumSet(	
	UINT8 BlkNum
)
{
	XBYTE[REG_Jpeg_Bnumber] = BlkNum;
}

/**
 * @fn		  void HAL_DistMeModeSet(UINT8 mode)
 * @brief	  set ME mode to auto mode
 * @param	  [in] mode : '0' triggered by 0x8830[0], '1' auto trigger
 *						  set auto mode for using
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_DistMeModeSet(	
	UINT8 mode
)
{
	XBYTE[REG_Jpeg_Memode] = mode;//'0' triggered by 0x8830[0], '1' auto trigger
}

/**
 * @fn		  UINT8 HAL_DistMeOutAddrSet(UINT32 addrWord)
 * @brief	  set dist ME word address
 * @param	  [in] addrWord
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-4-22
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8  
HAL_DistMeOutAddrSet(
	UINT32 addrWord
)
{
	XBYTE[REG_Jpeg_Meoutaddr_Byte0] = READ8(&addrWord   , 3);
	XBYTE[REG_Jpeg_Meoutaddr_Byte1] = READ8(&addrWord   , 2);
	XBYTE[REG_Jpeg_Meoutaddr_Byte2] = READ8(&addrWord   , 1);
	XBYTE[REG_Jpeg_Meoutaddr_Byte3] = READ8(&addrWord   , 0);
	return SUCCESS;
}

