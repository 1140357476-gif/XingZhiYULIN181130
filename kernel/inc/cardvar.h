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

#ifndef _cardvar_H_
#define _cardvar_H_

//#include "chipopt.h"
#include "cardopt.h"

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
//-------------------------------------
//cardptch.c
//-------------------------------------
extern 	xdata 	UINT8 	G_Card_Type;
extern 	xdata 	UINT16 	G_Card_Supported;
extern 	xdata 	UINT16 	G_Card_Module;
extern  xdata   UINT16    G_Card_ExptCode;
extern 	xdata 	UINT16	G_Card_UI_Action;               // HIS101a-0001
extern  xdata   UINT32   G_Card_DOS_SDRAM_FAT1Addr;      // HIS101a-0002

#endif /*__cardvar_H__*/