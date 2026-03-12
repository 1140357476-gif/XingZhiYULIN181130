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
 * @file      File name
 * @brief     Brief
 * @author   wangjg
 * @since     2011-10-01
 * @date      2011-10-01
*/

#ifndef __SD_BG_def_H__
#define __SD_BG_def_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "dbg_def.h"
#include "sdfunc.h"
#include "dos32.h"
#include "hal_sd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SdBgDebugIO 0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define bgNodeGet() (bgNodes+bgNode)
#define bgNodeLastGet() (bgNodes+bgNodeLast)
#define bgNodePre() ( (bgNode==0)?(bgNode=BgNodeCount-1):(--bgNode))
#define bgNodeNext() ( (bgNode>=BgNodeCount-1)?(bgNode=0):(++bgNode))
#define bgNodeAdd() ( (bgNodeLast>=BgNodeCount-1)?(bgNodeLast=0):(++bgNodeLast))  

#define BG_ENTER_CRITICAL() XBYTE[REG_Fm_intEn] &= ~4
#define BG_EXIT_CRITICAL() XBYTE[REG_Fm_intEn] |= bgIntrReg&4
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8 bgIntrReg; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern DOS dos;
extern UINT8 RspBuf[];
extern UINT8 sdModePostIdle;

extern UINT32 G_SD_MultiSecNum;
extern UINT8 sdByteMode;
extern UINT8 G_SD_WritePhase;
extern UINT8 G_Card_Type;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//UINT32 bgFnoWrite(UINT8 fno, UINT32 addrByte, UINT32 cnt);
UINT8 bgWait(UINT32 timeout,UINT32 file,UINT32 line);
void bgCreateList(BgFileObj_t *bgfile,UINT32 count);
void bgCreateFat(BgFileObj_t *bgfile);
void bgCreateFile(BgFileObj_t *bgfile,UINT8 isClose);
void bgModifyDisk(BgFileObj_t *bgfile);
UINT32 bgFreeClusterGet(BgFileObj_t *bgfile,UINT32 cnt,UINT32* next);
void bgFillFat(UINT8 fno,UINT32 next,UINT32 cnt);
void bgDbgPrintf(UINT32 val);

#define BG_INTR_TIMR_ENABLE 0
#define bgPutchar(c) //INTR_putchar(c)
#define sdputchar(c) //putchar(c)
#define bgputcharIntr(c) //INTR_putchar(c)
//#define bgPrintfImport printf
//#define bgPrintfTimer printf
#ifndef bgPrintfImport
#define bgPrintfImport /_printf
#endif
#ifndef bgPrintfTimer
#define bgPrintfTimer /_printf
#define bgTimerGet /_printf
#else
#define bgTimerGet HAL_GlobalReadGTimer
#endif
#endif  /* __TEMPLATE_HDR_H__ */
