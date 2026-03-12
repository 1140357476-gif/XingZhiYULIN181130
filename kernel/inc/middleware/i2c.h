/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 * @file		i2c_api.h
 * @brief		i2c module header file.
 * @author		Matt Wang
 * @since		2008-09-08
 * @date		2008-12-29
 */
#ifndef __I2C_H__
#define __I2C_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "hal_i2c.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define I2C_USE_MACRO  1  /* fdk internal usage only. */

typedef enum {
	I2C_TRANS_MODE_NORMAL = (0 << 0),
	I2C_TRANS_MODE_BURST  = (1 << 0),
} i2cTransMode_e;  /* plz keep sync to HAL_I2cTransMode_e. */

typedef enum {
	I2C_REG_CLK_DIV_2048 = 0,
	I2C_REG_CLK_DIV_1024  = 1,
	I2C_REG_CLK_DIV_512  = 2,
	I2C_REG_CLK_DIV_256  = 3,
	I2C_REG_CLK_DIV_128  = 4,
	I2C_REG_CLK_DIV_64  = 5,
	I2C_REG_CLK_DIV_32  = 6,
} i2cRegClkDivMode_e;  /* plz keep sync to HAL_I2cClkCfgSet. */

typedef enum {
	I2C_SOURCE_DIV_1 = 0,
	I2C_SOURCE_DIV_2,
	I2C_SOURCE_DIV_3,
	I2C_SOURCE_DIV_4,
	I2C_SOURCE_DIV_5,
	I2C_SOURCE_DIV_6,
	I2C_SOURCE_DIV_7,
	I2C_SOURCE_DIV_8,
	I2C_SOURCE_DIV_9,
	I2C_SOURCE_DIV_10,
	I2C_SOURCE_DIV_11,
	I2C_SOURCE_DIV_12,
	I2C_SOURCE_DIV_13,
	I2C_SOURCE_DIV_14,
	I2C_SOURCE_DIV_15,
	I2C_SOURCE_DIV_16,
	I2C_SOURCE_DIV_17,
	I2C_SOURCE_DIV_18,
	I2C_SOURCE_DIV_19,
	I2C_SOURCE_DIV_20,
	I2C_SOURCE_DIV_21,
	I2C_SOURCE_DIV_22,
	I2C_SOURCE_DIV_23,
	I2C_SOURCE_DIV_24,
	I2C_SOURCE_DIV_25,
	I2C_SOURCE_DIV_26,
	I2C_SOURCE_DIV_27,
	I2C_SOURCE_DIV_28,
	I2C_SOURCE_DIV_29,
	I2C_SOURCE_DIV_30,
	I2C_SOURCE_DIV_31,
	I2C_SOURCE_DIV_32,	
} i2cSourceDivMode_e;  /* plz keep sync to HAL_I2cRegClkCfgSet. */


/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if I2C_USE_MACRO
#define i2cDeviceAddrSet       HAL_I2cDeviceAddrSet
#define i2cDeviceAddrGet       HAL_I2cDeviceAddrGet
#define i2cRegClkCfgSet        HAL_I2cRegClkCfgSet
#define i2cRegClkCfgGet        HAL_I2cRegClkCfgGet
#define i2cClkCfgSet           HAL_I2cClkCfgSet
#define i2cClkCfgGet           HAL_I2cClkCfgGet
#define i2cCmdTableSend        HAL_I2cCmdTableSend
#define i2cCmdTableSend_Intr   HAL_I2cCmdTableSend_Intr
#define i2cRegRead             HAL_I2cRegRead
#define i2cReg16Read           HAL_I2cReg16Read
#define i2cReg16Read16         HAL_I2cReg16Read16
#else
/* i2c.c */
#if 0
void i2cDeviceAddrSet(UINT8 addr);
void i2cDeviceAddrGet(UINT8* pAddr);
void i2cClkCfgSet(UINT8 div);
void i2cClkCfgGet(UINT8* pDiv);
UINT8 i2cCmdTableSend(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd);
UINT8 i2cRegRead(UINT8 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
UINT8 i2cReg16Read(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
#endif
#endif

#endif  /* __I2C_API_H__ */

