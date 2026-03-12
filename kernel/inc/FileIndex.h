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

#ifndef __FILEINDEX_H__
#define __FILEINDEX_H__

void DOS_FileOrderTblInit(void);
void DOS_FIleOrderRst(UINT16 index);
void DOS_FileOrderWrite(UINT16 OrderIndex, UINT16 FileTrueIndex);
UINT16 DOS_FileIndexGet(UINT16 order) ;
void DOS_FileLastIndexClear(void);
#if 0 //xian --, mask uncalled func
void DOS_FileIndexClearAll(void) ;
#endif	
#endif

