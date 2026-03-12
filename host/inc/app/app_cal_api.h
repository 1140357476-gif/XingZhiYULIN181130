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
#ifndef __APP_CAL_MENU_H__
#define __APP_CAL_MENU_H__

#include "common.h"
#include "host_func_opt.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define TV_FOCUS 0
#define PC_FOCUS 1
#define FOCUS_MODE		TV_FOCUS

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct FramePos_s
{
	UINT16 xpos;
	UINT16 ypos;
	UINT16 width;
	UINT16 heigth;
}FramePos_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appCalInit(void);
void appCalSnapRaw(void);
void appCalSnapPureRaw(void);
void appCalSnapIQRaw(void);
void appCalPvRaw(void);
void appCalPvPureRaw(void);
void appCalPvYuv(void);
UINT8 appCalBpSearch(UINT8 item);
void appCalShowWait(void);
void appCalOB(void);
void appCalAutoOB(void);
void appCalPvOB(UINT16 resId , UINT16 resOff);
void appCalAwb(void);
void appCalAe(void);
void appCardUpdateFW(void);
void appCalStorageTest(void);
#if SUPPORT_MCU_WTD
void appMCUWDTEnterCardISPMode(void);
#endif
void appAfFrameInit(void);
void appCalAfOsdShow(void);
void appCalBpPvModeSet(UINT8 pv_mode);
void appCalAF(void);
void appCalAF2AStart(void);
void appCalAF2AStop(void);

void appCalAwbCalibration(UINT8 item);
#if FUNC_HOST_TOUCH_PANEL
void appCalTouchPanel(void);
#endif

#if SUPPORT_AF
void AF_CalAF(void);
void AF_startCalAF(void);
#endif


#endif /*__APP_CAL_MENU_H__*/

