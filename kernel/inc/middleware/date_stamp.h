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

#ifndef __DATE_STAMP_H__
#define __DATE_STAMP_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "hal.h"
#include "hal_global.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DATE_STAMP_DEFAULT_H_SIZE	32
#define DATE_STAMP_DEFAULT_V_SIZE	64
		
#define DATE_STAMP_H_SIZE_1	32
#define DATE_STAMP_V_SIZE_1	64
	
#define DATE_STAMP_H_SIZE_2	24
#define DATE_STAMP_V_SIZE_2	48
	
#define DATE_STAMP_H_SIZE_3	16
#define DATE_STAMP_V_SIZE_3	32 
	
#define DATE_STAMP_H_SIZE_4	12
#define DATE_STAMP_V_SIZE_4	24
	
#define DATE_STAMP_H_SIZE_5	10
#define DATE_STAMP_V_SIZE_5	20
	
#define DATE_STAMP_H_SIZE_6	8
#define DATE_STAMP_V_SIZE_6	16
	
#define DATE_STAMP_H_SIZE_7	6
#define DATE_STAMP_V_SIZE_7	10
	
#define DATE_STAMP_TEMP_NUM (UINT16)16
#define DATE_STAMP_NUM		(UINT16)24//if not exit custom str,you can set 20
	
	
#define DATE_STAMP_CUSTOM_STR_MAXSIZE   16

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
void dateStampPrepare( UINT32 addr, UINT32 tmpAddr, UINT16 dateHSize, UINT16 dateVSize );
UINT16 dateStampActive(dateStc_t dateInf,UINT32 srcAddr,UINT16 dateHSize,UINT16 dateVSize,UINT32 dtnAddr,UINT16 dtnHSize,UINT16 dtnVSize,UINT16 dtnHOffset,UINT16 dtnVOffset,UINT16 mode);
UINT32 videoRecStampSizeGet();
#endif  /* __DATE_STAMP_H__ */


