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
 * @file      cdsp_filter.h
 * @brief     initial cdsp
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __CDSP_FILTER_H__
#define __CDSP_FILTER_H__


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define EDGE_REG_NUM 	(UINT16)16
#define EDGE_LEVEL 		(UINT16)11
#define CORING_REG_NUM	(UINT16)17
#define CORING_LEVEL 	(UINT16)6

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
void cdspPvDynamicTempDenoiseParaSet(UINT8 tdlowyval,UINT8 tdhighyval,UINT8 tdlowythr,UINT8 tdywid,UINT8 tdlowuvval,UINT8 tdhighuvval,UINT8 tdlowuvthr,UINT8 tduvwid);
void cdspEdgeFilterSet(UINT8 level,UINT8 opt);
void cdspCoringFilterSet(UINT8 level);
float logFun(float val);


#endif //__SP1K_OSD_API_H__

