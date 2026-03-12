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
 * @file      gui_tp.h
 * @brief     touch panel scan definition
 * @author    Phil Lin
 * @since     2012-02-01
 * @date      2012-02-01
*/

#ifndef __GUI_TP_H__
#define __GUI_TP_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TP_OPEN 1
#define TP_CLOSE 0
#define TP_PENDOWN 0
#define TP_PENUP 1
#define TP_WAITINTR -1

#define TP_CALIBRATION_EN 1
#define TP_CALIBRATION_DIS 0
#define TP_CALIBRATION_POIT1 1
#define TP_CALIBRATION_POIT2 2
#define TP_CALIBRATION_POIT3 3
#define TP_CALIBRATION_POIT4 4 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	TP_PROC_PD = 0x01 << 0,
	TP_PROC_PU = 0x01 << 1,
} tpProcType_t; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 tpScanProcGet(void);
void tpScanProcAdd(UINT8 proc);
void tpScanProcHdl(UINT8 proc);
UINT8 tpCurrStsGet(void);
void tpCurrStsSet(UINT8 sts);

#endif  /* __GUI_TP_H__ */

