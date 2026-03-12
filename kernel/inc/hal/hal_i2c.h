/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		hal_i2c.h
 * @brief		hal i2c header file.
 * @author		Matt Wang
 * @since		2008-09-08
 * @date		2008-12-29
 */
#ifndef __HAL_I2C_API_H__
#define __HAL_I2C_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	HAL_I2C_TRANS_MODE_NORMAL = (0 << 0),
	HAL_I2C_TRANS_MODE_BURST  = (1 << 0),
} HAL_I2cTransMode_e; 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void HAL_I2cDeviceAddrSet(UINT8 addr);
void HAL_I2cDeviceAddrGet(UINT8* pAddr);
void HAL_I2cRegClkCfgSet(UINT8 regClkDiv);
void HAL_I2cRegClkCfgGet(UINT8* pRegClkDiv);
void HAL_I2cClkCfgSet(UINT8 div);
void HAL_I2cClkCfgGet(UINT8* pDiv);
UINT8 HAL_I2cCmdTableSend_Intr(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd);
UINT8 HAL_I2cCmdTableSend(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd);
UINT8 HAL_I2cRegRead(UINT8 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
UINT8 HAL_I2cReg16Read(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
UINT8 HAL_I2cReg16Read16(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);


#endif  /* __HAL_I2C_API_H__ */

