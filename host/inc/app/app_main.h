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

#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_STATE_BUFF_SIZE	2

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define STATE_DEFAULT	SP1K_MSG_STATE_TO_STILL_VIEW

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (code* pfpSTATE)(UINT16);

typedef struct appState_s {
	pfpSTATE StateFunc;
	UINT8 FuncBank;
} appState_t;

typedef struct CamState_s
{
	UINT8 PrevState[APP_STATE_BUFF_SIZE];
	UINT8 ActiveState;
} CamState_t;

typedef enum {
	APP_STATE_NULL = 0, 
		
	APP_STATE_POWER_ON,
	APP_STATE_STILL_VIEW, 
	APP_STATE_VIDEO_VIEW,
	APP_STATE_AUDIO_REC,
	APP_STATE_STILL_PLAY, 
	APP_STATE_MUTI_PLAY,
	APP_STATE_VIDEO_PLAY, 
	APP_STATE_AUDIO_PLAY, 
	APP_STATE_MENU, 
	APP_STATE_USB,
	APP_STATE_SLIDE_SHOW,
	APP_STATE_PAN_SHOW,
	APP_STATE_IDLE,
	APP_STATE_POWER_OFF,
	APP_STATE_DPS,
	APP_STATE_UNKNOWN = 0xff,
} appStateList_t;

typedef enum appStateId_e{
	STATE_FUNC_ID_POWER_ON, 	
	STATE_FUNC_ID_STILL_VIEW, 
	STATE_FUNC_ID_VIDEO_VIEW,
	STATE_FUNC_ID_AUDIO_REC,	
	STATE_FUNC_ID_STILL_PLAY, 
	STATE_FUNC_ID_VIDEO_PLAY,
	STATE_FUNC_ID_AUDIO_PLAY, 
	STATE_FUNC_ID_MUTI_PLAY,	
	STATE_FUNC_ID_USB,			
	STATE_FUNC_ID_MENU, 
	STATE_FUNC_ID_SLIDE_SHOW, 
	STATE_FUNC_ID_PAN_SHOW, 
	STATE_FUNC_ID_IDLE,
	STATE_FUNC_ID_POWER_OFF,
	STATE_FUNC_ID_DPS,	
	STATE_FUNC_ID_TOTAL,
} appStateId_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appMain (void);
UINT8 appPreStateGet(UINT8 PreLev);
UINT8 appCurrStateGet(void);

#endif  /* __APP_MAIN_H__ */
