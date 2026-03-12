/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __APP_MENU_EVENT_H__
#define __APP_MENU_EVENT_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MENU_PROMPT_OK_OFF	0x00
#define MENU_PROMPT_MENU_OFF	0x00

#define MENU_PROMPT_OK_ON	0x01
#define MENU_PROMPT_MENU_ON	0x02

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
void appMenuState(UINT32 cmd,UINT32 param);
void menuProcDisp (void);
void menuProcDisp2Layer (void);
void menuProcDispYesNo(UINT8 idx);
void menuEVDisp(void);
void menuRTCDisp(void);
void menuProcDispPalette(UINT16 msg);
void menuSpecialSequenceDisp(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 appMenuEvent_VerShow(void);
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
UINT8 appMenuEvent_PbFrameSave(UINT8 item);
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
#endif  /* __APP_MENU_EVENT_H__ */


