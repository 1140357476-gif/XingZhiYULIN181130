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

#ifndef _SF_H_
#define _SF_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	SPI_CMD_RD_ID				0x9F
//#define	CMD_RD_DATA_25MHZ		0x03
//#define	CMD_RD_DATA_50MHZ		0x0B
#define	SPI_CMD_SECTOR_ERASE		0x20
//#define	CMD_ERASE			0xD8
#define	SPI_CMD_ERASE_ALL			0xC7	/* 13.0s */
#define	SPI_CMD_BYTE_PROG			0x02
#define	SPI_CMD_WRT_STS				0x01
#define	SPI_CMD_RD_STS				0x05
#define	SPI_CMD_WRT_EN				0x06
#define	SPI_CMD_WRT_DIS				0x04

#define	SPI_CMD_PAGE_PROG			0x02
#define	SPI_CMD_AAI_PROG				0xAD

#define	SPI_CMD_READ			0x03
#define	SPI_CMD_FAST_READ		0x0B
#define	SPI_CMD_4READ			0xEB
#define	SPI_CMD_2READ			0xBB

#define	SPI_QUAD_DUMMY		0x05

#define	PAGE_SIZE		256
#define	PAGE_SIZE_B		8
#define	PAGE_PER_SECTOR	256
#define	SECTOR_PER_DIE	32
#define	SECTOR_NUM_PER_ERASE	((long)PAGE_PER_SECTOR*PAGE_SIZE/512)

#define	SPI_MAKER_MXIC		0xC2
#define	SPI_MAKER_WINBOND	0xEF
#define	SPI_MAKER_EON   	0x1C
#define	SPI_MAKER_ESMT   	0x8C
#define	SPI_MAKER_SPANSION  0x01
#define	SPI_MAKER_SST   	0xBF
#define	SPI_MAKER_GD	   	0xC8

#define SPI_MAKER_ST		0x20

#define	SPI_1BIT_MODE	0
#define	SPI_2BIT_MODE	1
#define	SPI_4BIT_MODE	2

#define	SPI_PIO_RW_MODE	0
#define	SPI_DMA_RW_MODE	1

#define K_SPI2SRAM	0
#define K_SPI2DRAM	1
#define K_SPI2USB	2

#define K_SRAM2SPI	0
#define K_DRAM2SPI	1
#define K_USB2SPI	2

#define PAGE_WRITE_TIMEOUT		100000

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define	SPI_CE_LO	XBYTE[REG_Fm_SPIcsnn] = 0;
#define	SPI_CE_HI	XBYTE[REG_Fm_SPIcsnn] = 1;
#define	SPI_4BIT_MODE_SET		XBYTE[REG_Fm_spibuswidth] = 2;
#define	SPI_2BIT_MODE_SET		XBYTE[REG_Fm_spibuswidth] = 1;
#define	SPI_1BIT_MODE_SET		XBYTE[REG_Fm_spibuswidth] = 0;

#define	SPI_WRITE_EN	{SPI_CE_LO; HAL_SPIWritePort(SPI_CMD_WRT_EN); SPI_CE_HI;}
#define	SPI_WRITE_DIS	{SPI_CE_LO; HAL_SPIWritePort(SPI_CMD_WRT_DIS); SPI_CE_HI;}

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum{
	SPI_FLOW_PAGE_DMA 		= (UINT8)0x00, 
		
	SPI_FLOW_PAGE_IO_WRT 	= (UINT8)0x01,
	SPI_FLOW_AAI_WRT		= (UINT8)0x02,
	SPI_FLOW_BYTE_WRT		= (UINT8)0x04,
	SPI_FLOW_WR_MASK		= (UINT8)0xFF,

//no needed
//	SPI_FLOW_PAGE_IO_RD		= 0x0100,
//	SPI_FLOW_RD_MASK			= 0xFF00
}spiFlowMode_t;

 typedef struct{
	UINT8 makerId;
	UINT8 memType;
	UINT8 capacity;
	UINT8 bitMode;
	spiFlowMode_t flowMode;
	UINT8 eraseCmd;
}spiModeSel_t;

 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern spiModeSel_t g_spiModeSel;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sfBitModeSet(UINT8 bitMode);
void sfFlowModeSet(spiFlowMode_t mode);
void sfEraseCmdSet(UINT8 cmd);

void sfSimpleCmd(UINT8 cmd);
void sfIdRead(UINT8 id[3]);
UINT8 sfStatusRead(void);
void sfStatusWrite(UINT8 sts);
UINT8 sfIsBusy(UINT32 timeout);
void sfChipErase(void);
void sfSectorErase(UINT32 addr);

#endif  /* _SF_H_ */
