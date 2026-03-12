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
 * @file      osd_dot.c
 * @brief     osd dot draw function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 

#include "hal_dram.h" 

#include "osd_api.h" 
#include "osd_buff.h" 

#if MODULE_OSD_OPT
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

/**
 * @fn        UINT8 osdDotDraw(SINT16 x, SINT16 y)
 * @brief     osdDotDraw
 * @param     [in] x
 * @param     [in] y
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdDotDraw(
	SINT16 x,
	SINT16 y
)
{
	UINT32 buffAddrWord;
	UINT16 buffSizeW, buffSizeH;
	UINT32 oftByte;
	UINT8 LSB, MSB;
	osdPen_t* pen;
	
	buffAddrWord = osdDispBuffAddrGet(1);
	osdDispBuffSizeGet(&buffSizeW, &buffSizeH);

	osdPenGet(&pen);

	oftByte = (UINT32)y * buffSizeW + (UINT32)x;
	HAL_DramStartAddrSet(buffAddrWord + (oftByte >> 1), 1);
	HAL_DramWordDataRead(&LSB, &MSB);
	HAL_DramWordDataWrite(
		(oftByte & 0x01) ? LSB : pen->color,
		(oftByte & 0x01) ? pen->color : MSB
	);
	
	return SUCCESS;
}

/**
 * @fn        UINT8 osdDotRead(SINT16 x, SINT16 y, UINT16* val)
 * @brief     osdDotRead
 * @param     [in] x
 * @param     [in] y
 * @param     [in] val
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdDotRead(
	SINT16 x,
	SINT16 y,
	UINT16* val
)
{
	UINT32 buffAddrWord;
	UINT16 buffSizeW, buffSizeH;
	UINT32 oftByte;
	UINT8 LSB, MSB;

	buffAddrWord = osdDispBuffAddrGet(1);
	osdDispBuffSizeGet(&buffSizeW, &buffSizeH);
	
	oftByte = (UINT32)y * buffSizeW + (UINT32)x;
	HAL_DramStartAddrSet(buffAddrWord + (oftByte >> 1), 1);
	HAL_DramWordDataRead(&LSB, &MSB);

	if (oftByte & 0x01) {
		*val = MSB;
	} else {
		*val = LSB;
	}

	return SUCCESS;
}
#endif /* MODULE_OSD_OPT */
