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
 * @file      hal_i2c.c
 * @brief     hal i2c module api
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_global.h"
#include "Reg_front.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HAL_I2C_REG16_READ_VERIFY  0

#define HAL_I2C_MAIN_DELAY  1
#define HAL_I2C_CLK_DELAY  1 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* debug, remove ";/K_SLASH  //" when you want to debug. */
#define DBG_PRINT_CMD_TABLE_SEND0;/K_SLASH  //printf
#define DBG_PRINT_CMD_TABLE_SEND1;/K_SLASH  //printf
#define DBG_PRINT_REG_READ0;/K_SLASH        //printf
#define DBG_PRINT_REG_READ1;/K_SLASH        //printf
#define DBG_PRINT_REG16_READ0;/K_SLASH      //printf
#define DBG_PRINT_REG16_READ1;/K_SLASH      //printf
#define DBG_PRINT_BYTE_GPIO_WRITE;/K_SLASH  //printf
#define DBG_PRINT_BYTE_GPIO_READ;/K_SLASH   //printf

#define HAL_I2C_IS_BUSY()  (XBYTE[REG_Front_Busy]	/*& 0x01*/)

#define HAL_I2C_BUSY_WAIT() \
	while ( HAL_I2C_IS_BUSY() ); \

#define SCK_H  XBYTE[REG_Front_Tggpioo] |= 0x10		/* sck output set to high. */
#define SCK_L  XBYTE[REG_Front_Tggpioo] &= ~0x10	/* sck output set to low. */
#define SDA_H  XBYTE[REG_Front_Tggpioo] |= 0x20		/* sda output set to high. */
#define SDA_L  XBYTE[REG_Front_Tggpioo] &= ~0x20	/* sda output set to low. */
#define SDA_O  XBYTE[REG_Front_Tggpiooen] |= 0x20	/* sda direction set to output. */
#define SDA_I  XBYTE[REG_Front_Tggpiooen] &= ~0x20	/* sda direction set to input. */
#define SDA_R  (XBYTE[REG_Front_Tggpioi] & 0x20)	/* sda read. */

#if 0
#define MAIN_DELAY  HAL_10usWait(HAL_I2C_MAIN_DELAY)
#else
#define MAIN_DELAY
#endif

#if 1  /* must be 1! otherwise read back value maybe wrong. */
#define CLK_DELAY  HAL_GlobalTimerWait10us(HAL_I2C_CLK_DELAY)
#else
#define CLK_DELAY
#endif

#define TRANS_START \
	SCK_H; \
	SDA_O; \
	SDA_H; \
	CLK_DELAY; \
	SDA_L; \
	CLK_DELAY; \

#define TRANS_STOP \
	SCK_H; \
	SDA_O; \
	SDA_L; \
	CLK_DELAY; \
	SDA_H; \
	CLK_DELAY; \ 
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
 *                 I N T E R N A L    D E C L A R A T I O N S             *
 **************************************************************************/
static void HAL_I2cByteGpioWrite(UINT8 val);
static void HAL_I2cWordGpioWrite(UINT16 *val);

static UINT8 HAL_I2cByteGpioRead(void);
static void HAL_I2cWordGpioRead(UINT8 *val); 

 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void HAL_I2cDeviceAddrSet(UINT8 addr)
 * @brief     set i2c device (slave) address
 * @param     [in] addr: device (slave) address, 0x00~0xFF
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cDeviceAddrSet(
	UINT8 addr
)
{
	/* set Slaveaddr. */
	XBYTE[REG_Front_Slaveaddr] = addr;
}

/**
 * @fn        void HAL_I2cDeviceAddrGet(UINT8* pAddr)
 * @brief     Get i2c device (slave) address
 * @param     [out] pAddr: device (slave) address, 0x00~0xFF
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cDeviceAddrGet(
	UINT8* pAddr
)
{
	/* Get Slaveaddr. */
	if (pAddr) {
		*pAddr = XBYTE[REG_Front_Slaveaddr];
	}
}

/**
 * @fn        UINT8 HAL_I2cRegClkCfgSet(UINT8 div)
 * @brief     set i2c reg clk configuration
 * @param     [in] regClkDiv : division factor, 0~31. reg_clk = (source/(regclkdiv+1))
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cRegClkCfgSet(
	UINT8 regClkDiv
)
{
	if ( regClkDiv > 31 ) {
		regClkDiv = 31;
	}

	XBYTE[0x2021] = regClkDiv;
}

/**
 * @fn        void HAL_I2cRegClkCfgGet(UINT8* pRegClkDiv)
 * @brief     get i2c reg clk configuration
 * @param     [out] pRegClkDiv : division factor, 0~31. reg_clk = (source/(regclkdiv+1))
 * @retval    NULL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cRegClkCfgGet(
	UINT8* pRegClkDiv
)
{
	if (pRegClkDiv) {
		*pRegClkDiv = XBYTE[0x2021];
	}
}

/**
 * @fn        UINT8 HAL_I2cClkCfgSet(UINT8 div)
 * @brief     set i2c clk configuration
 * @param     [in] div : division factor, 0~7. i2c_clk = reg_clk / (2 ^ (11 - div))
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cClkCfgSet(
	UINT8 div
)
{
	/* set Freq. */
	if ( div > 7) {
		div = 7;
	}
	XBYTE[REG_Front_Freq] = div;

	/* set i2cclkinen & clkcentpos. */
	if ( div > 6 ) {
		div = 6;
	}
	div = 0x01 << (6 - div);
	if ( div >= 0x10 ) {
		div--;
	}
	XBYTE[REG_Front_clkcentpos] = 0x80 | div;  /* i2cclkinen always 1. */
}

/**
 * @fn        void HAL_I2cClkCfgGet(UINT8* pDiv)
 * @brief     get i2c clk configuration
 * @param     [out] pDiv : division factor, 0~7. i2c_clk = reg_clk / (2 ^ (11 - div))
 * @retval    NULL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_I2cClkCfgGet(
	UINT8* pDiv
)
{
	if (pDiv) {
		*pDiv = XBYTE[REG_Front_Freq];
	}
}

/**
 * @fn        UINT8 HAL_I2cCmdTableSend_Intr(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd)
 * @brief     send command table to i2c interface
 * @param     [in] cmdTbl : command table
 * @param     [in] cmdLen : length of each command, 1~8, only used in normal mode
 * @param     [in] cmdTot : Amount of commands in the command table
 * @param     [in] transMode : transfer mode
 *								0: normal mode
 *								1: burst mode
 * @param     [in] isr : called by 
 *						0: regular function
 *						1: isr
 * @param     [in] upd : 
 *						0: immediately update
 *						1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_I2cCmdTableSend_Intr(
	UINT8 *cmdTbl,
	UINT8 cmdLen,
	UINT8 cmdTot,
	UINT8 transMode,
	UINT8 isr,
	UINT8 upd
)
{
	UINT16 addrReg, dataReg;
	UINT8 sr, cnt;
	#if HAL_I2C_REG16_READ_VERIFY
		UINT16 addrReg16;
		UINT8 dataW, dataR;
	#endif

	isr = isr;
	sr = sr;

	DBG_PRINT_CMD_TABLE_SEND0("HAL_I2cCmdTableSend 0x%06lX %bd %bd %bd %bd %bd\n",(UINT32)cmdTbl, cmdLen, cmdTot, transMode, isr, upd);

	/* check validation. */
	if ( cmdTot == 0
	|| (transMode == HAL_I2C_TRANS_MODE_NORMAL && (cmdLen == 0 || cmdLen > 8)) ) {
		return FAIL;
	}
	
	#if 0//LinJieCheng intr not need ENTER_CRITICAL
	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}
	#endif
	
	/* wait previous access done. */
	HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

	/* set Transmode. */
	XBYTE[REG_Front_Transmode] = transMode;

	/* set Vdupdate, I2Csyncsel & I2csvd. */
	XBYTE[REG_Front_I2Csyncsel] = upd ? 0x11 : 0x00;  /* I2Csyncsel always 0. */

	/* send command table. */
	if ( transMode == HAL_I2C_TRANS_MODE_NORMAL ) {
		/* set Wrcount. */
		XBYTE[REG_Front_Wrcount] = (XBYTE[REG_Front_Wrcount] & ~0x07) | (cmdLen & 0x07);

		while ( cmdTot > 0 ) {
			#if HAL_I2C_REG16_READ_VERIFY
				addrReg16 = (cmdTbl[0] << 8) | cmdTbl[1];
				dataW = cmdTbl[2];
			#endif
			/* wait previous access done. */
			HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

			/* set Addr0. */
			DBG_PRINT_CMD_TABLE_SEND1(" A0=0x%02bX", *cmdTbl);
			XBYTE[REG_Front_Addr0] = *(cmdTbl++);

			/* set Data0 ~ Data7 (trigger write when set last data). */
			dataReg = REG_Front_Data0;
 			for ( cnt = cmdLen; cnt > 0; cnt-- ) {
				DBG_PRINT_CMD_TABLE_SEND1(" D%u=0x%02bX", (dataReg-REG_Front_Data0)>>2, *cmdTbl);
				XBYTE[dataReg] = *(cmdTbl++);
				dataReg += 4;
			}
			DBG_PRINT_CMD_TABLE_SEND1("\n", 0);

			#if HAL_I2C_REG16_READ_VERIFY
				HAL_I2cReg16Read(&addrReg16, &dataR, 1, 1, 1, 0, 0);
				printf("reg[0x%04X]= 0x%02bX=>0x%02bX :%s\n", addrReg16, dataW, dataR, dataW==dataR?"same":"diff");
			#endif

			cmdTot--;
		}
	}
	else {  /* HAL_I2C_TRANS_MODE_BURST. */
		while ( cmdTot > 0 ) {
			if ( cmdTot >= 8 ) {
				cnt = 8;
				cmdTot -= 8;
			}
			else {
				cnt = cmdTot;
				cmdTot = 0;
			}

			/* wait previous access done. */
			HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

			/* set Wrcount. */
			XBYTE[REG_Front_Wrcount] = (XBYTE[REG_Front_Wrcount] & ~0x07) | (cnt & 0x07);

			addrReg = REG_Front_Addr0;
			dataReg = REG_Front_Data0;
			for ( ; cnt > 0; cnt-- ) {
				/* set Addr0~Addr7. */
				DBG_PRINT_CMD_TABLE_SEND1(" A%d=0x%02bX", (addrReg-REG_Front_Addr0)>>2, *cmdTbl);
				XBYTE[addrReg] = *(cmdTbl++);
				addrReg += 4;

				/* set Data0~Data7 (trigger write when set last data). */
				DBG_PRINT_CMD_TABLE_SEND1(" D%u=0x%02bX", (dataReg-REG_Front_Data0)>>2, *cmdTbl);
				XBYTE[dataReg] = *(cmdTbl++);
				dataReg += 4;
			}
			DBG_PRINT_CMD_TABLE_SEND1("\n", 0);
		}
	}

	#if 0//LinJieCheng intr not need ENTER_CRITICAL
	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}
	#endif

	XBYTE[REG_Front_Dummy0] = XBYTE[REG_Front_Slaveaddr];
	XBYTE[REG_Front_Dummy1] = XBYTE[REG_Front_ACKerr];
	XBYTE[REG_Front_Dummy2] = XBYTE[REG_Front_Addr0];
	XBYTE[REG_Front_Dummy3] = XBYTE[REG_Front_Data0];

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_I2cCmdTableSend(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd)
 * @brief     send command table to i2c interface
 * @param     [in] cmdTbl : command table
 * @param     [in] cmdLen : length of each command, 1~8, only used in normal mode
 * @param     [in] cmdTot : how many commands in the command table
 * @param     [in] transMode : transfer mode, one of HAL_I2cTransMode_e
 * @param     [in] isr : called by 
 *							0: regular function
 *							1: isr
 * @param     [in] upd
 *						0: immediately update
 *						1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_I2cCmdTableSend(
	UINT8 *cmdTbl,
	UINT8 cmdLen,
	UINT8 cmdTot,
	UINT8 transMode,
	UINT8 isr,
	UINT8 upd
)
{
	UINT16 addrReg, dataReg;
	UINT8 sr, cnt;
	#if HAL_I2C_REG16_READ_VERIFY
		UINT16 addrReg16;
		UINT8 dataW, dataR;
	#endif

	DBG_PRINT_CMD_TABLE_SEND0("HAL_I2cCmdTableSend 0x%06lX %bd %bd %bd %bd %bd\n",(UINT32)cmdTbl, cmdLen, cmdTot, transMode, isr, upd);

	/* check validation. */
	if ( cmdTot == 0
	|| (transMode == HAL_I2C_TRANS_MODE_NORMAL && (cmdLen == 0 || cmdLen > 8)) ) {
		return FAIL;
	}

	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}

	/* wait previous access done. */
	HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

	/* set Transmode. */
	XBYTE[REG_Front_Transmode] = transMode;

	/* set Vdupdate, I2Csyncsel & I2csvd. */
	XBYTE[REG_Front_I2Csyncsel] = upd ? 0x11 : 0x00;  /* I2Csyncsel always 0. */

	/* send command table. */
	if ( transMode == HAL_I2C_TRANS_MODE_NORMAL ) {
		/* set Wrcount. */
		XBYTE[REG_Front_Wrcount] = (XBYTE[REG_Front_Wrcount] & ~0x07) | (cmdLen & 0x07);

		while ( cmdTot > 0 ) {
			#if HAL_I2C_REG16_READ_VERIFY
				addrReg16 = (cmdTbl[0] << 8) | cmdTbl[1];
				dataW = cmdTbl[2];
			#endif
			/* wait previous access done. */
			HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

			/* set Addr0. */
			DBG_PRINT_CMD_TABLE_SEND1(" A0=0x%02bX", *cmdTbl);
			XBYTE[REG_Front_Addr0] = *(cmdTbl++);

			/* set Data0 ~ Data7 (trigger write when set last data). */
			dataReg = REG_Front_Data0;
 			for ( cnt = cmdLen; cnt > 0; cnt-- ) {
				DBG_PRINT_CMD_TABLE_SEND1(" D%u=0x%02bX", (dataReg-REG_Front_Data0)>>2, *cmdTbl);
				XBYTE[dataReg] = *(cmdTbl++);
				dataReg += 4;
			}
			DBG_PRINT_CMD_TABLE_SEND1("\n", 0);

			#if HAL_I2C_REG16_READ_VERIFY
				HAL_I2cReg16Read(&addrReg16, &dataR, 1, 1, 1, 0, 0);
				printf("reg[0x%04X]= 0x%02bX=>0x%02bX :%s\n", addrReg16, dataW, dataR, dataW==dataR?"same":"diff");
			#endif

			cmdTot--;
		}
	}
	else {  /* HAL_I2C_TRANS_MODE_BURST. */
		while ( cmdTot > 0 ) {
			if ( cmdTot >= 8 ) {
				cnt = 8;
				cmdTot -= 8;
			}
			else {
				cnt = cmdTot;
				cmdTot = 0;
			}

			/* wait previous access done. */
			HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

			/* set Wrcount. */
			XBYTE[REG_Front_Wrcount] = (XBYTE[REG_Front_Wrcount] & ~0x07) | (cnt & 0x07);

			addrReg = REG_Front_Addr0;
			dataReg = REG_Front_Data0;
			for ( ; cnt > 0; cnt-- ) {
				/* set Addr0~Addr7. */
				DBG_PRINT_CMD_TABLE_SEND1(" A%d=0x%02bX", (addrReg-REG_Front_Addr0)>>2, *cmdTbl);
				XBYTE[addrReg] = *(cmdTbl++);
				addrReg += 4;

				/* set Data0~Data7 (trigger write when set last data). */
				DBG_PRINT_CMD_TABLE_SEND1(" D%u=0x%02bX", (dataReg-REG_Front_Data0)>>2, *cmdTbl);
				XBYTE[dataReg] = *(cmdTbl++);
				dataReg += 4;
			}
			DBG_PRINT_CMD_TABLE_SEND1("\n", 0);
		}
	}

	XBYTE[REG_Front_Dummy0] = XBYTE[REG_Front_Slaveaddr];
	XBYTE[REG_Front_Dummy1] = XBYTE[REG_Front_ACKerr];
	XBYTE[REG_Front_Dummy2] = XBYTE[REG_Front_Addr0];
	XBYTE[REG_Front_Dummy3] = XBYTE[REG_Front_Data0];

	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_I2cRegRead(UINT8 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd)
 * @brief     read desired register data from i2c interface
 * @param     [in] addrTbl : register address table
 * @param     [in] dataTbl : read back data
 * @param     [in] dataLen : how many bytes to read in each register, 1~8
 * @param     [in] regTot : how many registers to read
 * @param     [in] subAddrEn : sub-address 
 *									0: disabled
 *									1:enabled
 * @param     [in] restartMode : 
 *								0: stop+start
 *								1: repeat start
 * @param     [in] upd
 *						0: immediately update
 *						1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_I2cRegRead(
	UINT8 *addrTbl,
	UINT8 *dataTbl,
	UINT8 dataLen,
	UINT8 regTot,
	UINT8 subAddrEn,
	UINT8 restartMode,
	UINT8 upd
)
{
	UINT16 dataReg;
	UINT8 len;


	DBG_PRINT_REG_READ0("HAL_I2cRegRead 0x%06lX 0x%06lX %bd %bd %bd %bd %bd\n",(UINT32)addrTbl, (UINT32)dataTbl,dataLen, regTot, subAddrEn, restartMode, upd);

	/* check validation. */
	if ( regTot == 0 || dataLen == 0 || dataLen > 8 ) {
		return FAIL;
	}

	/* wait previous access done. */
	HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

	/* set Transmode. */
	XBYTE[REG_Front_Transmode] = 0;

	/* set Vdupdate, I2Csyncsel & I2csvd. */
	XBYTE[REG_Front_I2Csyncsel] = 0x10 | upd;  /* Vdupdate always 1, I2Csyncsel always 0. */

	/* set Rdcount. */
	XBYTE[REG_Front_Wrcount] = (XBYTE[REG_Front_Wrcount] & ~0x70) | ((dataLen & 0x07) << 4);

	while ( regTot ) {
		/* set Addr0. */
		DBG_PRINT_REG_READ1(" A0=0x%02bX", *addrTbl);
		XBYTE[REG_Front_Addr0] = *(addrTbl++);

		/* set Prefetch, Restarten & Subaddren (trigger read). */
		XBYTE[REG_Front_Subaddren] = 0x10 | (restartMode << 1) | subAddrEn;  /* Prefetch set 1 to trigger read. */

		/* wait previous access done. */
		HAL_I2C_BUSY_WAIT();  /* busy wait after trigger read. */

		/* get read back data. */
		dataReg = REG_Front_Data0;
		len = dataLen;
		while ( len > 0 ) {
			DBG_PRINT_REG_READ1(" D%u=0x%02bX", (dataReg-REG_Front_Data0)>>2, XBYTE[dataReg]);
			*(dataTbl++) = XBYTE[dataReg];
			dataReg += 4;

			len--;
		}
		DBG_PRINT_REG_READ1("\n", 0);

		regTot--;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_I2cReg16Read(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd)
 * @brief     read desired 16-bit register data from i2c interface
 * @param     [out] addrTbl : 16-bit register address table
 * @param     [out] dataTbl : read back data
 * @param     [in] dataLen : how many bytes to read in each register, 1~8
 * @param     [in] regTot : how many registers to read
 * @param     [in] subAddrEn : sub-address 
 *								0: disabled
 *								1:enabled 
 * @param     [in] restartMode
 *								0: stop+start
 *								1: repeat starts
 * @param     [in] upd
 *						0: immediately update
 *						1: sync to vsync falling 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_I2cReg16Read(
	UINT16 *addrTbl,
	UINT8 *dataTbl,
	UINT8 dataLen,
	UINT8 regTot,
	UINT8 subAddrEn,
	UINT8 restartMode,
	UINT8 upd
)
{
	UINT8 devAddrR, devAddrW, len, dataR;

	upd=upd;

	DBG_PRINT_REG16_READ0("HAL_I2cReg16Read 0x%06lX 0x%06lX %bd %bd %bd %bd %bd\n",(UINT32)addrTbl, (UINT32)dataTbl,dataLen, regTot, subAddrEn, restartMode, upd);

	/* check validation. */
	if ( regTot == 0 || dataLen == 0 || dataLen > 8 ) {
		return FAIL;
	}

	/* wait previous access done. */
	HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

	/* set tggpio[5:4] (i2c_sda, i2c_sck) to gpio pin output mode. */
	XBYTE[REG_Front_Tggpiosel] |= 0x30;
	XBYTE[REG_Front_Tggpiooen] |= 0x30;

	devAddrR = XBYTE[REG_Front_Slaveaddr] | 0x01;
	devAddrW = XBYTE[REG_Front_Slaveaddr] & ~0x01;

	while ( regTot ) {
		if ( subAddrEn ) {
			TRANS_START;
			MAIN_DELAY;

			/* send device write addr. */
			HAL_I2cByteGpioWrite(devAddrW);
			MAIN_DELAY;

			/* send reg addr. */
			DBG_PRINT_REG16_READ1(" A0=0x%04X", *addrTbl);
			HAL_I2cByteGpioWrite(*addrTbl >> 8);
			MAIN_DELAY;
			HAL_I2cByteGpioWrite(*addrTbl & 0xFF);
			MAIN_DELAY;
			addrTbl++;

			if ( !restartMode ) {
				TRANS_STOP;
				MAIN_DELAY;
			}
		}

		TRANS_START;
		MAIN_DELAY;

		/* send device read addr. */
		HAL_I2cByteGpioWrite(devAddrR);
		MAIN_DELAY;

		/* get read back data. */
		len = dataLen;
		while ( len > 0 ) {
			dataR = HAL_I2cByteGpioRead();
			MAIN_DELAY;
			DBG_PRINT_REG16_READ1(" D%bu=0x%02bX", dataLen - len, dataR);
			*(dataTbl++) = dataR;

			len--;
		}
		DBG_PRINT_REG16_READ1("\n", 0);

		TRANS_STOP;

		regTot--;
	}

	/* restore tggpio[5:4] (i2c_sda, i2c_sck) to function pin. */
	XBYTE[REG_Front_Tggpiosel] &= ~0x30;

	return SUCCESS;
}

UINT8 
HAL_I2cReg16Read16(
	UINT16 *addrTbl,
	UINT8 *dataTbl,
	UINT8 dataLen,
	UINT8 regTot,
	UINT8 subAddrEn,
	UINT8 restartMode,
	UINT8 upd
)
{
	UINT8 devAddrR, devAddrW, len, dataR[2];

	upd = upd;

	DBG_PRINT_REG16_READ0("HAL_I2cReg16Read 0x%06lX 0x%06lX %bd %bd %bd %bd %bd\n", (UINT32)addrTbl, (UINT32)dataTbl, dataLen, regTot, subAddrEn, restartMode, upd);


	/* check validation. */
	if ( regTot == 0 || dataLen == 0 || dataLen > 8 ) {
		return FAIL;
	}

	/* wait previous access done. */
	HAL_I2C_BUSY_WAIT();  /* busy wait must be before updating any reg. */

	/* set tggpio[5:4] (i2c_sda, i2c_sck) to gpio pin output mode. */
	XBYTE[REG_Front_Tggpiosel] |= 0x30;
	XBYTE[REG_Front_Tggpiooen] |= 0x30;

	devAddrR = XBYTE[REG_Front_Slaveaddr] | 0x01;
	devAddrW = XBYTE[REG_Front_Slaveaddr] & ~0x01;

	while ( regTot ) {
		if ( subAddrEn ) {
			TRANS_START;
			MAIN_DELAY;

			/* send device write addr. */
			HAL_I2cByteGpioWrite(devAddrW);
			MAIN_DELAY;

			/* send reg addr. */
			DBG_PRINT_REG16_READ1(" A0=0x%04X", *addrTbl);
			HAL_I2cByteGpioWrite(*addrTbl >> 8);
			MAIN_DELAY;
			HAL_I2cByteGpioWrite(*addrTbl & 0xFF);
			MAIN_DELAY;
			addrTbl++;

			if ( !restartMode ) {
				TRANS_STOP;
				MAIN_DELAY;
			}
		}

		TRANS_START;
		MAIN_DELAY;

		/* send device read addr. */
		HAL_I2cByteGpioWrite(devAddrR);
		MAIN_DELAY;

		/* get read back data. */
		len = dataLen;
		while ( len > 0 ) {
			HAL_I2cWordGpioRead(dataR);
			MAIN_DELAY;
			
			*(dataTbl++) = dataR[0];
            *(dataTbl++) = dataR[1];
			
			len--;
		}
		DBG_PRINT_REG16_READ1("\n", 0);

		TRANS_STOP;

		regTot--;
	}

	/* restore tggpio[5:4] (i2c_sda, i2c_sck) to function pin. */
	XBYTE[REG_Front_Tggpiosel] &= ~0x30;

	return SUCCESS;
}



/**
 * @fn        void HAL_I2cByteGpioWrite(UINT8 val)
 * @brief     write a byte via gpio simulated i2c interface.
 * @param     [in] val : written data
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
static void
HAL_I2cByteGpioWrite(
	UINT8 val
)
{
	UINT8 i;


	DBG_PRINT_BYTE_GPIO_WRITE("HAL_I2cByteGpioWrite 0x%02bX\n", val);

//	SCK_L;
	SDA_O;
//	SDA_L;
	CLK_DELAY;

	for ( i = 8+1; i > 0; i-- ) {  /* "+1" is ack (9th bit, don't care bit). */
		SCK_L;
		if ( val & 0x80 ) {
			SDA_H;
		}
		else {
			SDA_L;
		}
		CLK_DELAY;
		SCK_H;
		CLK_DELAY;
		val <<= 1;
	}
	SCK_L;
}

/**
 * @fn        UINT8 HAL_I2cByteGpioRead(void)
 * @brief     read a byte via gpio simulated i2c interface
 * @param     NONE
 * @retval    return = read data
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
HAL_I2cByteGpioRead(
	void
)
{
	UINT8 i, val;


//	SCK_L;
	SDA_I;
	CLK_DELAY;

	for ( i = 8; i > 0; i-- ) {
		val <<= 1;
		SCK_L;
		CLK_DELAY;
		SCK_H;
		CLK_DELAY;
		if ( SDA_R ) {
			val |= 0x01;
		}
		else {
			val &= ~0x01;
		}
	}
	SCK_L;
	SDA_O;
	SDA_H;  /* 9th bit is ack, always high. */
	CLK_DELAY;
	SCK_H;
	CLK_DELAY;
	SCK_L;

	DBG_PRINT_BYTE_GPIO_READ("HAL_I2cByteGpioRead 0x%02bX\n", val);

	return val;
}

/**
 * @brief	read 2 bytes via gpio simulated i2c interface.
 * @param	None.
 * @retval	return = read data.
 * @see
 * @author	Matt Wang
 * @since	2008-12-26
 * @todo	N/A
 * @bug		N/A
*/
static void
HAL_I2cWordGpioRead(
	UINT8 *val
)
{
	UINT8 i;


//	SCK_L;
	SDA_I;
	CLK_DELAY;

	for ( i = 8; i > 0; i-- ) {
		val[0] <<= 1;
		SCK_L;
		CLK_DELAY;
		SCK_H;
		CLK_DELAY;
		if ( SDA_R ) {
			val[0] |= 0x01;
		}
		else {
			val[0] &= ~0x01;
		}
	}
	SCK_L;
	SDA_O;
	SDA_L;  /* 9th bit is ack, always Low. */
	CLK_DELAY;
	SCK_H;
	CLK_DELAY;
	SCK_L;

	SDA_I;
	CLK_DELAY;
	for ( i = 8; i > 0; i-- ) {
		val[1] <<= 1;
		SCK_L;
		CLK_DELAY;
		SCK_H;
		CLK_DELAY;
		if ( SDA_R ) {
			val[1] |= 0x01;
		}
		else {
			val[1] &= ~0x01;
		}
	}
	SCK_L;
	SDA_O;
	SDA_H;       /* Nack */
	CLK_DELAY;
	SCK_H;
	CLK_DELAY;
	SCK_L;
	
}


