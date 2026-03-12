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
 * @file      pv_photoframe.h
 * @brief     preview photo frame header file
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/


#ifndef __PV_PHOTOFRAME_H__
#define __PV_PHOTOFRAME_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	PRV_BLEND_PHOTOFRM = 1,
	PRV_BLEND_ROTATION,
	PRV_BLEND_DATESTAMP,
	PRV_BLEND_END,
}; 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DBG_PV_PF_TIMING			0

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
UINT8 pvPhoFrmCdspEofGet(void);
void pvPhoFrmCdsoEofSet(UINT8 eof);
void pvPhoFrmCdspEofIntrSet(UINT8 eof);

void pvPhofrmBlend(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);
void pvPhofrmBlendIntr(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);

void pvPhofrmInit(void);


UINT8 pvPhofrmEnableGet(void);

void sp1kPvDlyPhofrmBlendCntSet(UINT8 dlyCnt);







#endif 
