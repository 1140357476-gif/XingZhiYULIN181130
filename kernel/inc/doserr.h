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

#ifndef __DOSERR_H__
#define __DOSERR_H__

#define K_DOS_ErrOption		0

//extern xdata UINT16 G_DOS_ErrCode;
extern void DOS_SetError(UINT16 errCode);
extern void DOS_ClearError(void);
extern UINT16 DOS_GetError(void);

#if ( K_DOS_ErrOption == 1 )
#define M_DOS_SetError(a)	DOS_SetError(a)
#define M_DOS_ClearError()	DOS_ClearError()
#define M_DOS_GetError()	DOS_GetError()
#else
#define M_DOS_SetError(a)
#define M_DOS_ClearError()
#define M_DOS_GetError()	0
#endif

#define K_DOS_ReadLogical0Err			1
#define K_DOS_ReadMBSSigErr			2
#define K_DOS_ReadPBSErr			3
#define K_DOS_ReadPBSSigErr			4
#define K_DOS_SectorNot512Err			5
#define K_DOS_UnknownFSErr			6
#define K_DOS_Fat32NotSupportErr		7
#define K_DOS_CardDetectErr			8
#define K_DOS_ReadFdbErr			9
#define K_DOS_CreateDirFailErr			10
#define K_DOS_DiskFullErr			11
#define K_DOS_SaveFatErr			12
#define K_DOS_LoadFatErr			13
#define K_DOS_AddFdbErr				14
#define K_DOS_DelFdbErr				15
#endif /*__DOSERR_H__*/
