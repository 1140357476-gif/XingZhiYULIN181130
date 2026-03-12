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
#ifndef __OS_API_H__
#define __OS_API_H__
//=============================================================================
//Header file
//=============================================================================
//#include "os_custom.h"
//=============================================================================
//Symbol
//=============================================================================

//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
#define OS_TASK_MAX 8	/* 1 task at least */

#define ACTIVE_FUNC_CLR()	osActiveFuncSet(osTaskIdle)
//------------------------------------------------------------------------------
//Data type
//------------------------------------------------------------------------------
typedef UINT8 (code* pFunc)(void);

typedef enum osSysMode_e {
	OS_SYS_MODE_DEFAULT		= 	(UINT8)0x00,
	OS_SYS_MODE_STILL_PV	= 	(UINT8)0x01,
	OS_SYS_MODE_STILL_Cap 	= 	(UINT8)0x02,
	OS_SYS_MODE_STILL_PB	= 	(UINT8)0x03,
	OS_SYS_MODE_VIDEO_PV	= 	(UINT8)0x04,
	OS_SYS_MODE_VIDEO_REC	= 	(UINT8)0x05,
	OS_SYS_MODE_VIDEO_PB	= 	(UINT8)0x06,
	OS_SYS_MODE_AUDIO_REC	= 	(UINT8)0x07,
	OS_SYS_MODE_AUDIO_PB	= 	(UINT8)0x08,
	OS_SYS_MODE_USB_PCCAM	= 	(UINT8)0x09,
	OS_SYS_MODE_USB_MSDC	= 	(UINT8)0x0A,
	OS_SYS_MODE_USB_SIDC	= 	(UINT8)0x0B,
	
	OS_SYS_MODE_MAX = (UINT8)0x0C
} osSysMode_t;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void osInit(void);
UINT8 osTaskCreate(pFunc TaskFunc, UINT8 TaskPrio);
void osSched(void);
void osTimeTick(void);
void osActiveFuncSet(pFunc Func);
void osTaskSuspend(UINT8 TaskID);
void osTaskResume(UINT8 TaskID);
UINT8 osTaskIdle(void);

void osSysModeSet(osSysMode_t mode);
osSysMode_t osSysModeGet(void);
#endif //__OS_API_H__
