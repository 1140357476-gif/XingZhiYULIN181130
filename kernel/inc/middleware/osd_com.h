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
 * @file      osd_com.h
 * @brief     osd common definition
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __OSD_COM_H__
#define __OSD_COM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "knl_func_opt.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MODULE_OSD_OPT   FUNC_KNL_OSD 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum osdFmt_e {
	OSD_FMT_8BIT = (UINT16)0x00,
	OSD_FMT_2BIT = (UINT16)0X01,
} osdFmt_t;

typedef struct gOsdFmt_s {
	osdFmt_t lang; //language char
	osdFmt_t icon; //icon 
} gOsdFmt_t;

typedef struct osdPen_s {
	UINT32 color;
	UINT16 width;
} osdPen_t;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* __OSD_COM_H__ */
