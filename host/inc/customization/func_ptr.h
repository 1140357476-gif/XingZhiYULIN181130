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
/**
 * @file		func_ptr.h
 * @brief		function pointer jump list
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __PTR_FUNC_H__
#define __PTR_FUNC_H__
 //=============================================================================
//Header file
//=============================================================================
#include "host_func_opt.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------
UINT8 pFunc_TestSnap(UINT16* ptr);
UINT8 pFunc_TestPb(UINT16* ptr);
void pFunc_KeyScan(void);
void pFunc_TpScan(void);
void pFunc_SelfSnapMSGSendShell(void);
UINT8 pFunc_AudioPb(void);
UINT8 pFunc_AudioRec(void);
UINT8 pFunc_VideoPb(void);
UINT8 pFunc_VideoRec(void);
UINT8 pFunc_VideoCap(void);
UINT8 pFunc_GuiProc(void);
UINT8 pFunc_TVDetect(void);
UINT8 pFunc_CardDetect(void);
UINT8 pFunc_USBDetect(void);
void pFunc_USBPwrSave(void);
UINT8 pFunc_osdBusyShow(void);
UINT8 pFunc_VoiceMemoStop(void);
UINT8 pFunc_SlideShowProc(void);
UINT8 pFunc_TestDscLife_span(void);
UINT8 pFunc_whiteLedCtrl(void);
UINT8 pFunc_SnapBurst( UINT8 *ptr );
void pFunc_TimerCount(void);
UINT8 pFunc_BatteryChgDetect(void);
void pFunc_CheckCountforTestUnitReady(void);
UINT8 pFunc_SoundCtrl(void);
UINT8 pFunc_pbAbort(void);
void pFunc_updateAfwindow(void);
void pFunc_MDTimeProcess(void);
#if SUPPORT_AF
void pFunc_AfCal(void);
#endif

UINT8* pFunc_AeUserDefineMeter(UINT8 w,	UINT8 h);
UINT8 pFunc_AeDynamicUserDefineMeter(void);

UINT8 pFunc_SnapBackgroundResLoad(void *ptr);
UINT8 pFunc_SnapHostStamp(void *ptr);

UINT8 pFunc_SfWPSet( UINT8 *ptr );
UINT8 pFunc_VideoCapRec(void);

#endif /*__PTR_FUNC_H__*/

