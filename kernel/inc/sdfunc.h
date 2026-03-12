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
#ifndef __SDFUNC_H__
#define __SDFUNC_H__
//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------

//-------------------------------------
//cfasic.c
//-------------------------------------
UINT8 SD_HwInitialize(UINT8 cardType);

UINT8 SD_LCard2Sram(UINT32);
UINT8 SD_LSram2Card(UINT32);

UINT8 SD_LCard2Dram(UINT32,UINT32);
UINT8 SD_LDram2Card(UINT32,UINT32);

UINT8 SD_LDram2CardMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector);   //ada@0306
UINT8 SD_LCard2DramMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector);   //ada@0306

//richie@mi0415
UINT8 SD_LCard2UsbMore(UINT32 logAddr, UINT32 numSector);
//richie@mi0415
UINT8 SD_LUsb2CardMore(UINT32 logAddr, UINT32 numSector);


/*UINT8 SD_LCard2SramByPort(UINT32);     //ada@0218
UINT8 SD_LSram2CardByPort(UINT32);

UINT8 SD_LDram2CardByPort(UINT32,UINT32);
UINT8 SD_LCard2DramByPort(UINT32,UINT32);

UINT8 SD_LCard2DramByVLC(UINT32,UINT32);
UINT8 SD_LDram2CardByVLC(UINT32,UINT32);
*/
//-------------------------------------
//sdcore.c
//-------------------------------------
UINT8 SD_Initialize(UINT8);
UINT16 SD_GetBehavior(void);
UINT8 SD_CardDetect(UINT8 cardType);
UINT8 SD_LCard2Usb(UINT32); // WWW1, Dummy

// @patch5.1@wyeo@solve avi sync problem on sd, mmc@begin
//UINT8 SD_LWriteStart(UINT8 srcPath, UINT32 logAddr, UINT32 numSector);
//UINT8 SD_LWriteContinue(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr, UINT32 *pNumSector);
//UINT8 SD_StopWrite(void);
//UINT8 SD_LWriteEnd(UINT8 srcPath, UINT32 copySize, UINT8 *pEndContinuous);

// @patch 5.2.1-5@wyeo@fix Samdisk erase file bug, begin
//UINT8 SD_LWriteSkipPage(UINT8 srcPath, UINT32 *pNumSector, UINT8 *pEndContinuous, UINT8 writeDummy);
//UINT8 SD_LWriteDummyPage(UINT8 srcPath, UINT32 numSector, UINT32 ramAddr);
// @patch 5.2.1-5@wyeo@fix Samdisk erase file bug, end

// @patch5.1@wyeo@solve avi sync problem on sd, mmc@end
UINT8 sdWaitDat0High(void);
UINT8 sdFastWrite(void);
UINT8 sdBgRwStsGet(void);
UINT8 sdBgWriteEn(UINT8 en);
UINT8 sdBgReadEn(UINT8 en);
void sdBgWriteEnd(void);
void sdBgReadEnd(void);
void sdIntDisable(void);
void sdBgCheckRdy(void);

UINT8 sdBgRwFreeStsWait(void);

#endif /*__SDFUNC_H__*/
