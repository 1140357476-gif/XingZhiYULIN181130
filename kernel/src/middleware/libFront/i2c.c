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
 * @file	   i2c.c
 * @brief	   i2c related function
 * @author    Will Chiu
 * @since	   2010-01-01
 * @date	   2010-01-01
*/


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_i2c.h"
#include "i2c.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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

#if !I2C_USE_MACRO
#if 0
/**
 * @fn        void i2cDeviceAddrSet(UINT8 addr)
 * @brief     set i2c device (slave) address
 * @param     [in] addr : device (slave) address, 0x00~0xFF
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
i2cDeviceAddrSet(
	UINT8 addr
)
{
	HAL_I2cDeviceAddrSet(addr);
}

/**
 * @fn        void i2cDeviceAddrSet(UINT8 addr)
 * @brief     Get i2c device (slave) address
 * @param     [out] pAddr : device (slave) address, 0x00~0xFF
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
i2cDeviceAddrGet(
	UINT8* pAddr
)
{
	HAL_I2cDeviceAddrGet(pAddr);
}

/**
 * @fn        void i2cClkCfgSet(UINT8 div)
 * @brief     set i2c clk configuration
 * @param     [in] div : division factor, 0~7. i2c_clk = reg_clk / (2 ^ (11 - div))
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
i2cClkCfgSet(
	UINT8 div
)
{
	HAL_I2cClkCfgSet(div);
}

/**
 * @fn        void i2cClkCfgGet(UINT8* pDiv)
 * @brief     get i2c clk configuration
 * @param     [out] pDiv : division factor, 0~7. i2c_clk = reg_clk / (2 ^ (11 - div))
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
i2cClkCfgSet(
	UINT8* pDiv
)
{
	HAL_I2cClkCfgGet(pDiv);
}


/**
 * @fn        UINT8 i2cCmdTableSend(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd)
 * @brief     send command table to i2c interface
 * @param     [in] cmdTbl : command table
 * @param     [in] cmdLen : length of each command, 1~8, only used in normal mode
 * @param     [in] cmdTot : how many commands in the command table
 * @param     [in] transMode : transfer mode, one of HAL_I2cTransMode_e
 * @param     [in] isr : called by 0: regular function, 1: isr
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
i2cCmdTableSend(
	UINT8 *cmdTbl,
	UINT8 cmdLen,
	UINT8 cmdTot,
	UINT8 transMode,
	UINT8 isr,
	UINT8 upd
)
{
	return HAL_I2cCmdTableSend(cmdTbl, cmdLen, cmdTot, transMode, isr, upd);
}


/**
 * @fn        UINT8 i2cRegRead(UINT8 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd)
 * @brief     read register data from i2c interface
 * @param     [in] addrTbl : register address table
 * @param     [in] dataTbl : read back data
 * @param     [in] dataLen : how many bytes to read in each register
 * @param     [in] regTot : how many registers to read
 * @param     [in] subAddrEn : sub-address 0: disabled, 1:enabled
 * @param     [in] restartMode : 0: stop+start, 1: repeat start
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
i2cRegRead(
	UINT8 *addrTbl,
	UINT8 *dataTbl,
	UINT8 dataLen,
	UINT8 regTot,
	UINT8 subAddrEn,
	UINT8 restartMode,
	UINT8 upd
)
{
	return HAL_I2cRegRead(addrTbl, dataTbl, dataLen, regTot, subAddrEn, restartMode, upd);
}

/**
 * @fn        UINT8 i2cReg16Read(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd)
 * @brief     read desired 16-bit register data from i2c interface
 * @param     [in] addrTbl : 16-bit register address table
 * @param     [in] dataTbl : read back data
 * @param     [in] dataLen : how many bytes to read in each register, 1~8
 * @param     [in] regTot : how many registers to read
 * @param     [in] subAddrEn : sub-address 0: disabled, 1:enabled
 * @param     [in] restartMode : 0: stop+start, 1: repeat start
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
i2cReg16Read(
	UINT16 *addrTbl,
	UINT8 *dataTbl,
	UINT8 dataLen,
	UINT8 regTot,
	UINT8 subAddrEn,
	UINT8 restartMode,
	UINT8 upd
)
{
	return HAL_I2cReg16Read(addrTbl, dataTbl, dataLen, regTot, subAddrEn, restartMode, upd);
}
#endif
#endif

