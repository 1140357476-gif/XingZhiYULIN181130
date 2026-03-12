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
#include "general.h"
#define TMR_TASK_INTERNAL 
#include "timer.h"
#include "func_ptr.h"
#include "func_timer_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/* Timer task for timer 1 */	
/* The max number of this list is TMR1_TASK_MAX */
TMR_TASK_DCL pFuncTmr1 code timerFuncCallList1[] = {

	(pFuncTmr1)pFunc_SlideShowProc,
	(pFuncTmr1)pFunc_VoiceMemoStop,
	(pFuncTmr1)pFunc_SelfSnapMSGSendShell,
//	(pFuncTmr1)pFunc_osdBusyShow,
	(pFuncTmr1)pFunc_USBPwrSave,
	(pFuncTmr1)pFunc_TestSnap,
	(pFuncTmr1)pFunc_TestPb,

    NULL,
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


