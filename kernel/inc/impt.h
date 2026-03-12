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

#ifndef __IMPT_H__
#define __IMPT_H__

//#include "chipopt.h"
#include "main.h"
//#include "ctlsetup.h"
//#include "initio.h"
#include "setmode.h"
//#include "quantdef.h"
#include "sdramhw.h"
//#include "uiflow.h"
//#include "audio.h"      // WWW1
//#include "timer.h"      // WWW1

#define G_ucDataBuffer		G_ucStorData

#define SETMODE_ClearPostBuffer()
#define SETMODE_SetUploadMode()
#define SETMODE_SetCameraIdle()
//#define SETMODE_SetFifoPath(a)
//#define K_FIFO_CPU2CPU				1
//#define SDRAM_SetStartAddress(a)        HAL_DramStartAddrSet((a), K_SDRAM_NoPrefetch)  

// extfunc.h
//void INITIO_SetQTable(UINT8 qIndex);
//extern UINT32 File_JpgStillImage(void);
//void UTIL_TimerStart(void);
//UINT32 UTIL_TimerElasped(UINT8 stop);

// extvar.h
//extern  data    BIT     G_ucStopStream;

#endif /*__IMPT_H__*/
