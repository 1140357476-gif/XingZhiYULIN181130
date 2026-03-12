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

#ifndef __APP_DEV_STORAGE_H__
#define __APP_DEV_STORAGE_H__
#include "exif_def.h"

#include "hal_device.h"
#ifdef DISK_DRV_SPI
UINT8 appSfWPSet( UINT8 mode );
#endif

#define APP_PWR_RCC_MODE_INTNAL_MOS   1  //power recycle using internal power MOS

void appSDPlugAction(void);
void appStorageMount (UINT8 type);
UINT8 appSDPlugStatusGet(void);

//UINT8 appBGImgDec(UINT32 resId, struct Exif_Decompress_Parameter* pImgAttr, UINT32 srcAddr, UINT32 dstAddr, UINT8 bWait);
UINT8 appBGImgDec(UINT32 resId, ExifImage_t* pImgAttr, UINT32 srcAddr, UINT32 dstAddr, UINT8 bWait);


void appDiskPartRecover(void);
void appSdPowerRecycle(void);
UINT8 appSDProtectStatusGet(void);
UINT8 appCardErrFmtChk(void);
void appCardErrFmtSta(UINT8 sta);
UINT8 appCardErrFmtErrChk(void);
UINT8 appCardErrFmtErrSet(UINT8 sta);
UINT8 appFileDelHotKeyChk(void);
void appFileDelHotKeySta(UINT8 sta);

#if SUPPORT_SPI_WRITE_CONTROL
void appSpiWpSet(UINT8 flag);
#endif


#endif /*__APP_DEV_STORAGE_H__*/

