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

#ifndef __VNDREQ_H__
#define __VNDREQ_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//richie@vc0422
#define K_CARD_EXIST_CHECK_INDEX	2
#define K_NAND_FLASH_EXIST_CHECK_INDEX	1
//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
typedef struct _MY_SYS_TIME{ 
	UINT16 wYear;	//(1601-30827) 
	UINT16 wMonth;	//(1-12) 
	UINT16 wDay;		//(1-31) 
	UINT16 wHour;	//(0-23) 
	UINT16 wMinute;	//(0-59) 
	UINT16 wSecond;	//(0-59) 
} MYSYSTIME; 
//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void VNDREQ_NoDataPhase(void);
void VNDREQ_PreInDataPhase(void);
void VNDREQ_PreOutDataPhase(void);
void VNDREQ_PostInDataPhase(void);
void VNDREQ_PostOutDataPhase(void);

#endif /*__VNDREQ_H__*/