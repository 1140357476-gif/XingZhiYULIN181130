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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_device.h"
#include "hal.h"
#include "hal_spi.h"
#include "sf.h"
#include "spi.h"
#include "dbg_wdt.h"
#include "reg_def.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#ifdef DISK_DRV_SPI

#define DEBUG 1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
spiModeSel_t g_spiModeSel = {0,0,0,0,0,0x20}; //eraseCmd=0x20

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sfBitModeSet(UINT8 bitMode)
{
printf("bitset:%bu, %bu\n", g_spiModeSel.bitMode, bitMode);
	g_spiModeSel.bitMode = bitMode;
//move to read flow
//	if(bitMode == SPI_1BIT_MODE){
//		XBYTE[0x2048] = (XBYTE[0x2048] & ~0x30) | 0x10; // set clk driving 4mA
//	}
//	else{
//		XBYTE[0x2048] = (XBYTE[0x2048] & ~0x30) | 0x20; // set clk driving 6mA
//	}
}

void sfFlowModeSet(spiFlowMode_t mode)
{
	g_spiModeSel.flowMode = mode;
}

void sfEraseCmdSet(UINT8 cmd)
{
	g_spiModeSel.eraseCmd = cmd;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] cmd
 * @retval    none
 * @see
 *************************************************************************/
void sfSimpleCmd(UINT8 cmd)
{
	SPI_CE_LO;
	HAL_SPIWritePort(cmd);
	SPI_CE_HI;
}

 /**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
void sfIdRead(UINT8 id[3])
{
	sfSimpleCmd(0xff); // reset 1bit for some special, e.g. sst26vf016 ... 
	SPI_1BIT_MODE_SET;

	SPI_CE_LO;
	HAL_SPIWritePort(SPI_CMD_RD_ID);		/*read ID command*/
	id[0] = HAL_SPIReadPort();		/* Manufacturer's ID, 20h for STMicroelectronics. */
	id[1] = HAL_SPIReadPort();		/* Memory Type     */
	id[2] = HAL_SPIReadPort();		/* Memory Capacity */
	SPI_CE_HI;

	g_spiModeSel.makerId = id[0];
	g_spiModeSel.memType = id[1];
	g_spiModeSel.capacity = id[2];

	XBYTE[REG_Fm_spipinmux] = 0x01;					//spi pin mux ?? do once

#if DEBUG
	printf("sfid=%02BX %02BX %02BX\n",id[0],id[1],id[2]);
#endif	

	return;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
UINT8 sfStatusRead(void)
{
	UINT8 ret;
	SPI_CE_LO;
	HAL_SPIWritePort(SPI_CMD_RD_STS);
	ret = HAL_SPIReadPort();
	SPI_CE_HI;
	return ret;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
void sfStatusWrite(UINT8 sts)
{
	SPI_CE_LO;
	HAL_SPIWritePort(SPI_CMD_WRT_STS);
	HAL_SPIWritePort(sts);
	SPI_CE_HI;
	return;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = busy sts: TRUE / FALSE.
 * @see       function X
 *************************************************************************/
UINT8 sfIsBusy(UINT32 timeout)
{
	UINT8 busyMask = 0x01;
	UINT8 sts, ret;

	if(g_spiModeSel.makerId == SPI_MAKER_SST && g_spiModeSel.memType == 0x26){
		busyMask = 0x80;
	}
	
	do{
		sts = sfStatusRead();				
		
		if (!(sts & busyMask)) {
			ret = FALSE;
			//printf("brk:%bx\n", sts);
			break;
		}
		else{
			//printf("sts:%bx\n", sts);
			ret = TRUE;
		}
	}while(timeout --);
	
	return ret;
}


/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
void sfSectorErase(UINT32 addr)
{
	dbgWdtFeed(10000); //10sec
	
	sfSimpleCmd(0x50);//en write r

	sfStatusWrite(0x02);
	SPI_WRITE_EN;
	SPI_CE_LO;
	HAL_SPIWritePort(g_spiModeSel.eraseCmd);
	HAL_SPIWritePort(addr >> 16);	/* A23~A16 */
	HAL_SPIWritePort(addr >> 8);		/* A15~A08 */
	HAL_SPIWritePort(addr);			/* A07~A00 */
	SPI_CE_HI;

	sfIsBusy(0x1000000);
	return;
}

/**************************************************************************
 * @name     
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 *************************************************************************/
void sfChipErase(void)
{
	dbgWdtFeed(10000); //10sec
	
	sfSimpleCmd(0x50);//en write r

	sfStatusWrite(0x02);
	SPI_WRITE_EN;
	SPI_CE_LO;
	HAL_SPIWritePort(SPI_CMD_ERASE_ALL);
	SPI_CE_HI;

	sfIsBusy(0xffffffff);
	return;
}

void sst26UnlockAll(UINT8 len)
{
	UINT8 i;

	//read
	SPI_CE_LO;
	HAL_SPIWritePort(0x72);
	for(i = 0; i < len; i ++){
		printf("%bx ", HAL_SPIReadPort());
	}
	SPI_CE_HI;
	printf("\n");

	//write
	SPI_WRITE_EN;
	SPI_CE_LO;
	HAL_SPIWritePort(0x42);
	for(i = 0; i < len; i ++){
		HAL_SPIWritePort(0);
	}
	SPI_CE_HI;
}

#endif
