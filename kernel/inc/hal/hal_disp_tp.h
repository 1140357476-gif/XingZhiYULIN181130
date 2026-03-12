/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#ifndef __HAL_DISP_TP_H__
#define __HAL_DISP_TP_H__

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
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void HAL_TpIntrEvt(void);
UINT8 HAL_TpPreCfg(void);
void HAL_TpXadcCfg(void);
void HAL_TpYadcCfg(void);
void HAL_TpXadcStart(void);
void HAL_TpYadcStart(void);
void HAL_TpadcvalPrep(void);
UINT8 HAL_TpPenupChek(void);
UINT8 HAL_TpReCfg(void);
UINT16 HAL_TpXadcvalMsure(void);
UINT16 HAL_TpYadcvalMsure(void);
#endif
 
