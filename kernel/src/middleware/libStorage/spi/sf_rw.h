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

#ifndef __SF_RW_H__
#define __SF_RW_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define K_SPI2SRAM	0
#define K_SPI2DRAM	1
#define K_SPI2USB	2

#define K_SRAM2SPI	0
#define K_DRAM2SPI	1
#define K_USB2SPI	2

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
 
UINT32 
sfPhySectorW(
	UINT32 startSector, 
	UINT16 dmaCnt,
	UINT32 dmaSize, 
	UINT32 buf, 
	UINT32 mode
);

UINT32 
sfPhySectorR(
	UINT32 startSector, 
	UINT16 dmaCnt,
	UINT32 dmaSize, 
	UINT32 buf, 
	UINT32 mode
);

void sfPhySectorSramR(UINT32 phyAddr, UINT16 sramAddr, UINT32 sizeByte, UINT32 mode);
#endif  /* __SF_RW_H__ */

