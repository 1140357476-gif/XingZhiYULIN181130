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
 * @file      osd_buff.h
 * @brief     osd buffer related function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __OSD_BUFF_H__
#define __OSD_BUFF_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "osd_com.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct osdDispBuff_s{
	UINT32* pAct;   //point to active buffer address
	UINT32* pInAct; //point to inactive buffer address
	UINT32 addrA; //dram word address
	UINT32 addrB; //dram word address
	UINT16 buffW; //buff width
	UINT16 buffH; //buff height
} osdDispBuff_t;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if MODULE_OSD_OPT

UINT8 osdDispBuffInit(void);
UINT32 osdDispBuffSw(void);
void osdDispBuffRst(void);
UINT32 osdDispBuffShow(void);
UINT32 osdDispBuffAddrGet(UINT8 bAct);
void osdDispBuffSizeGet(UINT16* buffW, UINT16* buffH);
UINT8 osdDispBuffCpy(UINT8 bActSrc, UINT8 bActDst);
UINT8 osdDispBuffFill(UINT8 bActDst, UINT8 bColor);

#else

#define osdDispBuffInit()
#define osdDispBuffSw()
#define osdDispBuffRst()
#define osdDispBuffShow()
#define osdDispBuffAddrGet(bAct)	0 /* the return value */
#define osdDispBuffSizeGet(buffW, buffH)	(*buffW = *buffH = 0)
#define osdDispBuffCpy(bActSrc, bActDst)
#define osdDispBuffFill(bActDst, bColor)

#endif /* MODULE_OSD_OPT */
#endif  /* __OSD_BUFF_H__ */
