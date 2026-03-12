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
 * @file      app_tp_scan.h
 * @brief     definition of touch panel scan
 * @author    Phil Lin
 * @since     2012-02-02
 * @date      2012-02-02
*/

#ifndef __APP_TP_SCAN_H__
#define __APP_TP_SCAN_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
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

void tpCalibrationDo(void);
void tpCalibrationEnter(void);
void tpScan(void);
void tpDelay(UINT16 n);
void tpPixAxisGet(SINT16 *x, SINT16 *y);
void tpPixAxisFix(SINT16 x, SINT16 y);
UINT8 tpCalDatSave(UINT16 resIdx, UINT32 freeAddr);
UINT8 tpCalDatLoad(UINT16 resIdx, UINT32 freeAddr);
UINT8 tpCoordFtProc(void);
void tpCoordFtRst(void);
void tpCalModeSet(UINT8 enable);
UINT8 tpCalModeGet(void);
UINT8 tpGetCurrCalPoint(void);

#endif  /* __APP_TP_SCAN_H__ */

