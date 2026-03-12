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

#ifndef __APP_STS_PWROFF_H__
#define __APP_STS_PWROFF_H__

void appPowerOff(UINT16 msg);
void appPowerOffProc(void);
void appUDFSave(void);
UINT8 appUDFLoad(void);
UINT8 appADFLoad(void);
UINT8 appMENULoad(void);
UINT8 appUDFCheckSum(void);
void appPowerOffBGShow(void);
void appPowerShutDown(void);
void appPowerHoldPullDown(void);

#endif /*__APP_STS_PWROFF_H__*/

