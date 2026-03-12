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

#ifndef __EXPTCODE_H__
#define __EXPTCODE_H__

#define K_Card_ExptOption	0

extern xdata UINT16 G_Card_ExptCod;
extern void Card_SetExptCode(UINT16 exptCode);
extern void Card_ClearExptCode(void);
extern UINT16 Card_GetExptCode(void);

#if ( K_Card_ExptOption == 1 )
#define M_Card_SetExptCode(a)	Card_SetExptCode(a)
#define M_Card_ClearExptCode()	Card_ClearExptCode()
#define M_Card_GetExptCode()	Card_GetExptCode()
#else
#define M_Card_SetExptCode(a)
#define M_Card_ClearExptCode()
#define M_Card_GetExptCode()
#endif

#define	K_Card_Expt0001		0x0001
#define	K_Card_Expt0002		0x0002
#define	K_Card_Expt0003		0x0003
#define	K_Card_Expt0004		0x0004
#define	K_Card_Expt0005		0x0005
#define	K_Card_Expt0006		0x0006
#define	K_Card_Expt0007		0x0007
#define	K_Card_Expt0008		0x0008
#define	K_Card_Expt0009		0x0009
#define	K_Card_Expt000A		0x000A
#define	K_Card_Expt000B		0x000B
#define	K_Card_Expt000C		0x000C
#define	K_Card_Expt000D		0x000D
#define	K_Card_Expt000E		0x000E
#define	K_Card_Expt000F		0x000F
#define	K_Card_Expt0010		0x0010
#define	K_Card_Expt0011		0x0011
#define	K_Card_Expt0012		0x0012
#define	K_Card_Expt0013		0x0013 //
#define	K_Card_Expt0014		0x0014
#define	K_Card_Expt0015		0x0015
#define	K_Card_Expt0016		0x0016
#define	K_Card_Expt0017		0x0017
#define	K_Card_Expt0018		0x0018
#define	K_Card_Expt0019		0x0019
#define	K_Card_Expt002A		0x001A
#define	K_Card_Expt002B		0x001B
#define	K_Card_Expt002C		0x001C
#define	K_Card_Expt002D		0x001D
#define	K_Card_Expt002E		0x001E
#define	K_Card_Expt002F		0x001F
#define	K_Card_Expt0030		0x0020

#endif /*__EXPTCODE_H__*/
