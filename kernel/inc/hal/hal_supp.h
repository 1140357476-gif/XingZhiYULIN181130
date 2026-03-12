/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __HAL_SUPP_H__
#define __HAL_SUPP_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
//UINT8 HAL_CheckReadData2UsbDmaDone(UINT16 size);
//UINT8 HAL_WriteZigZagQTable(UINT8 component, UINT8* pTable);
//UINT8 HAL_GetUsbAudioAlternateSetting(void);
//cytsai@1211
//UINT8 HAL_InitPatternGenerator(void);
//UINT8 HAL_DoSnapPatternGenerator(UINT32* pSSize);
//void HAL_WriteReg(UINT16 address, UINT8 value);
//void HAL_WriteAndReg(UINT16 address, UINT8 value);
//void HAL_WriteOrReg(UINT16 address, UINT8 value);
//UINT8 HAL_ReadReg(UINT16 address);
UINT8 HAL_SRAMLow2High(UINT16 size,UINT8* buffer);
UINT8 HAL_SRAMHigh2Low(UINT16 size,UINT8* buffer);
//UINT8 HAL_SetCFDMA(BIT source,UINT32 LBAaddr, UINT32 size);
//UINT8 HAL_SDRAM_Dram2SramDMA(UINT32 ramAddr, UINT8 *sramAddr, UINT16 siz);
//UINT8 HAL_SDRAM_Sram2DramDMA(UINT8 *sramAddr, UINT32 ramAddr, UINT16 siz);
//void HAL_CFEnsable(void);
//void HAL_CFDisable(void);
UINT8 HAL_CardDetect(UINT8* Detect);
//UINT8 HAL_SetSMCDMA(BIT source,UINT32 phyAddr);

#endif /*__HAL_SUPP_H__*/

