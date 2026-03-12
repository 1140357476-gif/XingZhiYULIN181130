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
#ifndef _MENU_CORE_H_
#define _MENU_CORE_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#ifndef MENU_UNKNOWN
#define MENU_UNKNOWN    0xffff
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MENU_LAYER_0	0
#define MENU_LAYER_1	1
#define MENU_LAYER_2	2
#define MENU_LAYER_3	3

#define SCROLL_MODE_LINE	0
#define SCROLL_MODE_PAGE	1

#define MENU_IGNORE_DEFAULT	0
#define MENU_CALL_DEFAULT	1

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void menuLayerSizeGet(UINT8 layer, UINT8* line, UINT8* col);
UINT8 menuLanguageGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8	menuProcMoveUp (void);
UINT8	menuProcMoveDown (void);
UINT8	menuProcParent (void);
UINT8	menuProcChild (UINT8 sel);
UINT8	menuProcParaGet(UINT8 layer, UINT8 item);
void	menuProcParaSet(UINT8 layer, UINT8 item);
UINT8	menuProcIsActive(UINT8 layer, UINT8 item);
UINT8	menuProcExecute(UINT8 layer, UINT8 item, UINT16 msg);
UINT8	menuScrollModeSet(UINT8 mode);
UINT8	menuProcDbgMsg(UINT8 mode);

extern UINT32	menuProcCurrPosGet (void);
extern UINT32	menuProcCurrRootGet (void);
extern UINT8	menuProcCurrPosSet (UINT32 pos);
extern UINT8	menuProcCurrItemGet (UINT8 layer);
extern UINT8	menuProcScrollGet (UINT8 layer);
extern UINT8	menuProcLayerGet (void);
extern UINT8	strGet (UINT8 buf[], UINT8 bufSize, UINT16 index);
extern UINT8*	menuProcPtrGet (UINT8 layer, UINT8 item);
extern UINT8 menuStrGet(UINT8 layer, UINT8 item,UINT8 buf[], UINT8 bufSize);
extern UINT8*	menuProcPtrGetByPos (UINT32 pos);
extern UINT8*	menuProcChildPtrGet (UINT8 layer, UINT8 item, UINT8 sel);
extern UINT8 menuChildStrGet(UINT8 layer, UINT8 item,UINT8 sel,UINT8 buf[], UINT8 bufSize);
UINT16 strWidthGet(UINT8 *str);
#if 0
extern UINT8*	menuProcIconPtrGet (UINT8 layer, UINT8 item);
#else
extern UINT8	menuProcIconIDGet (UINT8 layer, UINT8 item);
#endif
extern UINT8	menuProcIconIDGetByPos (UINT32 pos);
extern UINT8	menuProcChildIconIDGet (UINT8 layer, UINT8 item, UINT8 sel);
extern UINT8	menuProcItemLghGet (UINT8 layer, UINT8 item);
extern UINT8	menuProcNodeItemGet (UINT8 layer, UINT8 item);
extern UINT8	menuProcNodeLghGet (UINT8 layer, UINT8 item);
extern UINT8	menuProcSubItemGet (UINT8 layer, UINT8 item);
UINT8 menuProcTPSet (UINT8 tpLine, UINT8 isChild);
UINT8 menuProcPageUp (void);
UINT8 menuProcPageDown (void);
 UINT8 menuSpecialMoveUp(void);
 UINT8 menuSpecialMoveDown(void);

#endif /* _MENU_CORE_H_ */
