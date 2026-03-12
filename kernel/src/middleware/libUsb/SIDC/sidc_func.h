/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      sidc_func.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SIDC_FUNC_H__
#define __SIDC_FUNC_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "string.h"

#include "hal.h"
#include "hal_usb.h"
#include "hal_dram.h"
#include "hal_gprm.h"

#include "main.h"
#include "doslink.h"
#include "dosvar.h"
//#include "dosusr.h"
#include "dcf_api.h"
#include "os_msg.h"

#include "msdc_dma.h"

#include "usbopt.h"//will

#include "sidc_struct.h"
#include "sidc_param.h"
#include "sidc_api.h"
#include "sidc_dbg.h"

#include "dps_addrDef.h"
#include "dps_res.h"
#include "dps_api.h"

#include "ctlsetup.h"
#include "bulkout.h"
#include "sp1k_rsvblk_api.h"
#include "intr.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SIDC_ADDR_TMP K_SDRAM_CommonEndAddr
#define CODE_SIZE	K_SDRAM_CodeSize

#define SIDC_RES_ID	0x47 
#define SIDC_RES_Device 1
#define SIDC_RES_Storage 0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define sidcGetBulkSize() (G_bUSBSpeed?512:64)
//--handle:
//--path||file
//--dev|dir||type|idx
#define sidcWriteHandlePath(handle,dev,dir) ((UINT16*)&handle)[0]=((UINT16)dev<<12)|((UINT16)dir)
#define sidcWriteHandleFile(handle,type,idx) ((UINT16*)&handle)[1]=((UINT16)type<<14)|((UINT16)idx)

#define sidcReadHandleDev(handle,dev) dev=(*(UINT8*)&handle)>>4
#define sidcReadHandleDir(handle,dir) dir=(*(UINT16*)&handle)&0x0fff
#define sidcReadHandleType(handle,type) type=(((UINT8*)&handle)[2])>>6
#define sidcReadHandleFileIndex(handle,idx) idx=((UINT16*)&handle)[1]&0x3fff

#define sidcIsJpgFile(handle) (((((UINT8*)&handle)[2])>>6)==1)
#define sidcIsScriptFile(handle) (handle>=DPS_MASK_HAND)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern UINT32 G_ulStorDataIndex;
extern UINT16 G_USR_FileCount;
extern UINT16 G_USR_Dir1Count;

extern UINT8 G_ucStorData[];

extern UINT8 sidcResponseData[24];
extern UINT8 sidcResponseSize;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**************************************************************************
 *               PIMA  	      *
 **************************************************************************/
UINT8 pimaGetDeviceInfo();
UINT8 pimaOpenSession();
UINT8 pimaCloseSession();
UINT8 pimaGetStorageIDs();
UINT8 pimaGetStorageInfo();
UINT8 pimaGetNumObjects();
UINT8 pimaGetObjectHandles();//);
UINT8 pimaGetObjectInfo();//);
UINT8 pimaGetObject();//);
UINT8 pimaGetThumb();//);
//UINT8 pimaDeleteObject();
UINT8 pimaSendObjectInfo();
UINT8 pimaSendObject();
//void pimaInitiateCapture();
//void pimaFormatStore();
//void pimaResetDevice();
//void pimaSelfTest();
//void pimaSetObjectProtection();
//void pimaPowerDown();
//void pimaGetDevicePropDesc();
//void pimaGetDevicePropValue();
//void pimaSetDevicePropValue();
//void pimaSetObjectProtection();
//void pimaTerminateOpenCapture();
//void pimaMoveObject();
//void pimaCopyObject();
UINT8 pimaGetPartialObject();//);
//UINT8 pimaInitiateOpenCapture();
//UINT8 PIMADATA_SendObjectInfo(void);

//UINT8 pimaGetBatteryLevelDesc(, UINT8* pPayload);
//UINT8 pimaGetImageSizeDesc(, UINT8* pPayload);
//UINT8 pimaGetDateTimeDesc(, UINT8* pPayload);
//UINT8 pimaGetBatteryLevelValue(, UINT8* pPayload);
//UINT8 pimaGetImageSizeValue(, UINT8* pPayload);
//UINT8 pimaGetDateTimeValue(, UINT8* pPayload);
//UINT8 PIMADATA_SetDevicePropValue(void);
//UINT8 PIMADATA_SetDateTimeValue(UINT8* pPayload);
//UINT8 pimaResetDevicePropValue();
//UINT8 pimaResetDateTimeValue(void);

/**************************************************************************
 *               INTR  	      *
 **************************************************************************/
#define sidcIntrDramWriteByte(val0,val1)	 	HAL_DramWriteByte(val0,val1)	
#define sidcIntrDramWrite16(val16)			HAL_DramWrite16(val16)	
#define sidcIntrDramWrite32(val32) 			HAL_DramWrite32(val32)

#define sidcIntrDramReadByte(val0,val1)	HAL_DramReadByte(val0,val1)
#define sidcIntrDramRead16(val16)		HAL_DramRead16(val16)
#define sidcIntrDramRead32(val32)		HAL_DramRead32(val32)	

UINT32 sidcIntrDataOut();
UINT32 sidcIntrDataIn(UINT32 size);
UINT8 sidcIntrResponse(UINT16 rst, UINT8 cnt, UINT32 * ps);
void sidcIntrCommand();
void sidcIntrLoad();
UINT32 sidcIntrRes(UINT8 *pt,UINT8 idx);

/**************************************************************************
 *               MAIN  	      *
 **************************************************************************/
UINT32 sidcMakeHandles(UINT32 addr,UINT8 typeFilter);
UINT32 sp1kSidcMakeHandle(UINT16 fileIndex);
UINT8 sp1kSidcParsingHandle(UINT32 handle,UINT8 *fileName);
UINT8 sidcGetFileInfo(UINT32 handle,UINT32 addr,sidc_file_info_t* pInfo);
void sidcInit();
//void sidcBuildContainer(UINT8 *pData,UINT32 size);

#define sidcDramWriteBytes(pt,addrW,cnt32)	HAL_DramWriteBytesAddr(addrW,(UINT8*)pt,cnt32,0)
#define sidcDramReadBytes(pt,addrW,cnt32)	HAL_DramReadBytesAddr(addrW,(UINT8*)pt,cnt32,0)

//UINT32 sidcBulkIn(UINT32 addrW,UINT8*pt,UINT32 size);
//UINT32 sidcBulkOut(UINT32 addrW,UINT8*pt,UINT32 size);
//#define sidcDram2Usb(addrW, size) sidcBulkIn(addrW, NULL, size)
//#define sidcUsb2Dram(addrW, size) sidcBulkOut(addrW, NULL, size)
//#define sidcSram2Usb(pt, size) sidcBulkIn(0, pt, size)
//#define sidcUsb2Sram(pt, size) sidcBulkOut(0, pt, size)
//UINT32 sidcContainerBuild(UINT8 xdata* pt,UINT32 size);

UINT8 sidcResponse(UINT16 rst, UINT8 cnt, UINT32 * ps);

UINT32 sidcDataIn(UINT32 addr,UINT32 size);
UINT8 sidcInterruptIn(UINT8 * point,UINT16 size);

UINT32 usbIntrDma(UINT32 addr, UINT32 sizeByte, UINT8 isWait,UINT8 isToUsb);
UINT32 usbDma(UINT32 addr, UINT32 sizeByte, UINT8 isWait,UINT8 isToUsb);
#define sidcIntrBulkIn(addrOrPt, size)	usbIntrDma((UINT32)addrOrPt,size,1,1)
#define sidcIntrBulkOut(addrOrPt, size)	usbIntrDma((UINT32)addrOrPt,size,1,0)
#define sidcBulkIn(addrOrPt, size)		usbDma((UINT32)addrOrPt,size,1,1)
#define sidcBulkOut(addrOrPt, size)	usbDma((UINT32)addrOrPt,size,1,0)
/**************************************************************************
 *               RES  	      *
 **************************************************************************/


#endif  /* __TEMPLATE_HDR_H__ */
