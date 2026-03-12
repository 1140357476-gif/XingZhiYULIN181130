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

#ifndef __APP_DPS_H__
#define __APP_DPS_H__

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 enum _DPS_JPG_FLG
{
	DPS_JPG_EXIST	= (unsigned char) 0x00,
	DPS_JPG_NOT_EXIST	= (unsigned char) 0x01,
};

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void appDps(UINT16 msg);
void appDpsConnectPrinter(void);
void appDpsParaSet(void);
void  appDpsStartJob(void);
void appDpsQualitySet(void);
void appDpsLayoutSet(void);
void appDpsPapersizeSet(void);
void appDpsPapertypeSet(void);
void appDpsDateprintSet(void);
void appDpsFilenameprintSet(void);
void appDpsInit(void);
UINT8 appDpsCheckFileNum(UINT32 *lastJpgIndex);
void appDpsNewDeviceStatusHandle(void);
void  appDpsParaRst(void);
void appDpsPrinterStatusSet(UINT8 printerStatusSet);
UINT8 appDpsPrinterStatusGet(void);
UINT32 appDpsPapersizeGet(void);
#endif /*__APP_DPS_H__*/

