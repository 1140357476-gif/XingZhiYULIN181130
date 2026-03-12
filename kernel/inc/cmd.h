/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _CMD_H_
#define _CMD_H_

#ifdef  CMD_PARSE
#define CMD_PARSE_DCL
#else
#define CMD_PARSE_DCL extern
#endif

#ifdef  CMD_MEM
#define CMD_MEM_DCL
#else
#define CMD_MEM_DCL extern
#endif

#ifdef  CMD_MISC
#define CMD_MISC_DCL
#else
#define CMD_MISC_DCL extern
#endif

#define TRUE          1
#define FALSE         0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (code* cmdHandler)(UINT8 *param);

typedef struct cmd_s {
	const UINT8  code*cmd;
	cmdHandler   phandler;
//	const UINT8  *phelp;
//	struct cmd_s code*pnext;
} cmd_t;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cmdVerManager(void);
void cmdMonitor(void* ptr);
void cmdRegister(cmd_t code* code* ppCmd);
void cmdInit(void);
void cmdDefaultSet(UINT32);
void cmdMemDump(UINT8 *);
void cmdMemFill(UINT8 *);
void cmdMemSearch(UINT8 *);
void cmdMemInit(void);
void cmdWhiteSpaceStrip(UINT8 **);
void cmdTokenStrip(UINT8 **);
void cmdArgGet(UINT8** pptr, UINT32 pArg[], UINT8 cnt, UINT8 bakPtr);
UINT8 cmdMemCmp(UINT32 addrWord0, UINT32 addrWord1, UINT32 cmpLenthWord, UINT32 stopCnt);
UINT8 cmdArgValueGet(UINT8 *cmdStr, UINT32 *arg, UINT8 argNum);
void cmdRst(void);
void cmdAutoTest(UINT8* ptr);
#endif


