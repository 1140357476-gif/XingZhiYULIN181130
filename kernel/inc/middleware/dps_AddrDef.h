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
 * @file      dps_addrdef.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DPS_DRAMADDRDEF_H__
#define __DPS_DRAMADDRDEF_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "hal_dram.h"
#include "initio.h"
#include "dps_struct.h" 
#include "Dos32.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define K_DPS_ADDR_Start 				K_SDRAM_DPSStartAddr
#define K_DPS_ADDR_End					(K_DPS_ADDR_TempBuf)

#define K_DPS_ADDR_Size					((K_DPS_ADDR_End - K_DPS_ADDR_Start)<<1)

//io buf
//#define K_DPS_ADDR_InData_Container_Size		(12ul)
#define K_DPS_ADDR_InData_Size			(1024UL*8UL)
#define K_DPS_ADDR_OutData_Size		(1024UL*2UL)
//file node buf
#define K_DPS_ADDR_FileNode_Size 		((UINT32)sizeof(sidc_node_t))
//print set buf
#define K_DPS_ADDR_ConfigStartJob_Size	((UINT32)DPS_PARA_ConfigStartJob_Size)
#define K_DPS_ADDR_ConfigPrintInfo_Size	(((UINT32)DPS_PARA_ConfigPrintInfo_Size_Min)*(UINT32)DPS_PARA_ConfigPrintInfoMaxCount +(UINT32)4UL)
//status save buf
#define K_DPS_ADDR_DeviceStatus_Size 		((UINT32)DPS_PARA_DeviceStatus_Size)
#define K_DPS_ADDR_JobStatus_Size 			((UINT32)DPS_PARA_JobStatus_Size)
#define K_DPS_ADDR_ConfigPrintService_Size 	((UINT32)DPS_PARA_ConfigPrintService_Size)
#define K_DPS_ADDR_Capatility_Size 			((UINT32)DPS_PARA_Capability_Elem_Size*10UL)
#define K_DPS_ADDR_Capatility_Item_Size		((UINT32)DPS_PARA_Capability_Elem_Size)
//#define K_DPS_ADDR_Capatility_ItemArray_Size		(DPS_PARA_Capability_ElemArray_Size)
//////////////////////////////

//io buf
#define K_DPS_ADDR_InData 		(K_SDRAM_DPSStartAddr + 6)//(K_DPS_ADDR_InData_Container+  (K_DPS_ADDR_InData_Container_Size>>1))
#define K_DPS_ADDR_OutData 	(K_DPS_ADDR_InData +  (K_DPS_ADDR_InData_Size>>1)+6)
//file node buf
#define K_DPS_ADDR_FileNode	(K_DPS_ADDR_OutData +  (K_DPS_ADDR_OutData_Size>>1)+6)
#define K_DPS_ADDR_InFileNode 	(K_DPS_ADDR_OutData +  (K_DPS_ADDR_OutData_Size>>1)+6)
#define K_DPS_ADDR_OutFileNode 	(K_DPS_ADDR_InFileNode +  (K_DPS_ADDR_OutData_Size>>1)+6)
//print set buf
#define K_DPS_ADDR_ConfigStartJob 		(K_DPS_ADDR_FileNode +  (K_DPS_ADDR_FileNode_Size>>1))
#define K_DPS_ADDR_ConfigPrintInfo 		(K_DPS_ADDR_ConfigStartJob +  (K_DPS_ADDR_ConfigStartJob_Size>>1))
//status save buf
#define K_DPS_ADDR_DeviceStatus 		(K_DPS_ADDR_ConfigPrintInfo +  (K_DPS_ADDR_ConfigPrintInfo_Size>>1))
#define K_DPS_ADDR_JobStatus 			(K_DPS_ADDR_DeviceStatus + ( K_DPS_ADDR_DeviceStatus_Size>>1))

#define K_DPS_ADDR_ConfigPrintService 	(K_DPS_ADDR_JobStatus +  (K_DPS_ADDR_JobStatus_Size>>1))

#define K_DPS_ADDR_Capatility 			(K_DPS_ADDR_ConfigPrintService + K_DPS_ADDR_ConfigPrintService_Size)

#define K_DPS_ADDR_Capatility_Qualities			(K_DPS_ADDR_Capatility)
#define K_DPS_ADDR_Capatility_PaperSizes		(K_DPS_ADDR_Capatility_Qualities 		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_PaperTypes		(K_DPS_ADDR_Capatility_PaperSizes 		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_FileTypes			(K_DPS_ADDR_Capatility_PaperTypes		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_DatePrints			(K_DPS_ADDR_Capatility_FileTypes		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_FileNamePrints	(K_DPS_ADDR_Capatility_DatePrints		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_ImageOptimizes	(K_DPS_ADDR_Capatility_FileNamePrints	+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_Layouts			(K_DPS_ADDR_Capatility_ImageOptimizes	+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_FixedSizes		(K_DPS_ADDR_Capatility_Layouts 		+ (K_DPS_ADDR_Capatility_Item_Size>>1))
#define K_DPS_ADDR_Capatility_Croppings			(K_DPS_ADDR_Capatility_FixedSizes 		+ (K_DPS_ADDR_Capatility_Item_Size>>1))

//#define K_DPS_ADDR_Capatility_ItemArrayStart 	(K_DPS_ADDR_Capatility 		+ (K_DPS_ADDR_Capatility_Size>>1))
//#define K_DPS_ADDR_Capatility_PaperTypesArray	K_DPS_ADDR_Capatility_ItemArrayStart
//#define K_DPS_ADDR_Capatility_LayoutsArray		(K_DPS_ADDR_Capatility_PaperTypesArray 		+ (K_DPS_ADDR_Capatility_ItemArray_Size>>1))

#define K_DPS_ADDR_StorSram0x1000					(K_DPS_ADDR_Capatility+ (K_DPS_ADDR_Capatility_Size>>1))
//////////////////////////////
//temp dram buf
#define K_DPS_ADDR_TempBuf			(K_DPS_ADDR_StorSram0x1000 + 512)

//SIDC usd temp buf
#define K_SDRAM_FreeAddr 				(K_SDRAM_ImagePlaybackBufAddr+2*10000+100)
#define K_DPS_HandlesAddr K_SDRAM_ImagePlaybackBufAddr

 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define dpsDramReadBytes(pt, addrW, cnt) HAL_DramReadBytesAddr(addrW, pt, cnt, 0)
#define dpsDramWriteBytes(pt, addrW, cnt) HAL_DramWriteBytesAddr(addrW, pt, cnt, 0)
//io buf
//#define dpsSaveInDataContainer(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_InData_Container, K_DPS_ADDR_InData_Container_Size)
#define dpsSaveInData(pData,index,size)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_InData+(index>>1), size)
#define dpsSaveOutData(pData,index,size)	dpsDramWriteBytes((UINT8 *)(pData),K_DPS_ADDR_OutData+(index>>1), size)
//file node buf
#define dpsSaveFileNode(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_FileNode, K_DPS_ADDR_FileNode_Size)
#define dpsSaveInFileNode(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_InFileNode, K_DPS_ADDR_FileNode_Size)
#define dpsSaveOutFileNode(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_OutFileNode, K_DPS_ADDR_FileNode_Size)

//print set buf
#define dpsSaveConfigStartJob(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigStartJob,K_DPS_ADDR_ConfigStartJob_Size)
#define dpsSaveConfigStartJobItem(data,type)	dpsDramWriteBytes((UINT8 *)&(data), K_DPS_ADDR_ConfigStartJob+2*(type-DPS_ACTION_GetCapability),4)

#define dpsSaveConfigPrintInfo(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintInfo, K_DPS_ADDR_ConfigPrintInfo_Size)
#define dpsSaveConfigPrintInfoCount(_count)	dpsDramWriteBytes((UINT8 *)&(_count), K_DPS_ADDR_ConfigPrintInfo, 2)
#define dpsSaveConfigPrintInfoIndex(_index)	dpsDramWriteBytes((UINT8 *)&(_index), K_DPS_ADDR_ConfigPrintInfo + 1, 2)
#define dpsSaveConfigPrintInfoItem(pData,_item)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintInfo + ((_item* DPS_PARA_ConfigPrintInfo_Size_Min +4)>>1), DPS_PARA_ConfigPrintInfo_Size_Min)

// getCapatility
#define dpsSaveGetCapability_Item(dataType,pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility + (((dataType-DPS_ACTION_GetCapability) *K_DPS_ADDR_Capatility_Item_Size)>>1), K_DPS_ADDR_Capatility_Item_Size) //dps_capabilityDataWriteOrRead(dataType,pData,0)
#define dpsSaveGetCapability_Qualities(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Qualities, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_PaperSizes(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_PaperSizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_PaperTypes(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_PaperTypes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_FileTypes(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_FileTypes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_DatePrints(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_DatePrints, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_FileNamePrints(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_FileNamePrints, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_ImageOptimizes(pData)	dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_ImageOptimizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_Layouts(pData)			dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Layouts, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_FixedSizes(pData)		dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_FixedSizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsSaveGetCapability_Croppings(pData)		dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Croppings, K_DPS_ADDR_Capatility_Item_Size)

//#define dpsSaveGetCapability_PaperTypesArrayCount(_count)	dpsDramWriteBytes((UINT8 *)&_count, K_DPS_ADDR_Capatility_PaperTypesArray,2)
//#define dpsSaveGetCapability_PaperTypesArray(index,pData)	dpsDramWriteBytes(pData, K_DPS_ADDR_Capatility_PaperTypesArray+1 + ((K_DPS_ADDR_Capatility_Item_Size*index)>>1), K_DPS_ADDR_Capatility_ItemArray_Size)
//#define dpsSaveGetCapability_LayoutsArray(index,pData)	dpsDramWriteBytes(pData, K_DPS_ADDR_Capatility_LayoutsArray+1 + ((K_DPS_ADDR_Capatility_Item_Size*index)>>1), K_DPS_ADDR_Capatility_ItemArray_Size)
// Notify/get status
#define dpsSaveDeviceStatus(pData)		dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_DeviceStatus, DPS_PARA_DeviceStatus_Size)
#define dpsSaveJobStatus(pData)			dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_JobStatus, DPS_PARA_JobStatus_Size)

//sram storage
#define dpsSaveGucStorData()			dpsDramWriteBytes(G_ucStorData, K_DPS_ADDR_StorSram0x1000, 1024)
#define dpsSaveGucStorData_intr()		sidcIntrDramWriteBytes(G_ucStorData, K_DPS_ADDR_StorSram0x1000, 1024)

//configure service
#define dpsSaveConfigService(pData)			dpsDramWriteBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintService, K_DPS_ADDR_ConfigPrintService_Size)

//io buf
//#define dpsLoadInDataContainer(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_InData_Container, K_DPS_ADDR_InData_Container_Size)
#define dpsLoadInData(pData,index,size)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_InData+(index>>1), size)
#define dpsLoadOutData(pData,index,size)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_OutData+(index>>1), size)
//file node buf
#define dpsLoadFileNode(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_FileNode, K_DPS_ADDR_FileNode_Size)
#define dpsLoadInFileNode(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_InFileNode, K_DPS_ADDR_FileNode_Size)
#define dpsLoadOutFileNode(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_OutFileNode, K_DPS_ADDR_FileNode_Size)

//print set buf
#define dpsLoadConfigStartJob(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigStartJob,K_DPS_ADDR_ConfigStartJob_Size)
#define dpsLoadConfigStartJobItem(data,type)	dpsDramReadBytes((UINT8*)&(data), K_DPS_ADDR_ConfigStartJob+2*(type-DPS_ACTION_GetCapability),4)
#define dpsLoadConfigPrintInfo(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintInfo, K_DPS_ADDR_ConfigPrintInfo_Size)
#define dpsLoadConfigPrintInfoCount(_count)	dpsDramReadBytes((UINT8 *)&(_count), K_DPS_ADDR_ConfigPrintInfo, 2)
#define dpsLoadConfigPrintInfoIndex(_index)	dpsDramReadBytes((UINT8 *)&(_index), K_DPS_ADDR_ConfigPrintInfo + 1, 2)
#define dpsLoadConfigPrintInfoItem(pData,_item)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintInfo + ((_item* DPS_PARA_ConfigPrintInfo_Size_Min +4)>>1), DPS_PARA_ConfigPrintInfo_Size_Min)
#define dpsClearConfigPrintInfo() HAL_GprmDramFill(K_DPS_ADDR_ConfigPrintInfo, K_DPS_ADDR_ConfigPrintInfo_Size, 0)


#define dpsLoadGetCapability_Item(dataType,pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility +(( (dataType-DPS_ACTION_GetCapability) *K_DPS_ADDR_Capatility_Item_Size)>>1), K_DPS_ADDR_Capatility_Item_Size) //dps_capabilityDataWriteOrRead(dataType,pData,1)
#define dpsLoadGetCapability_Qualities(pData)		dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Qualities, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_PaperSizes(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_PaperSizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_PaperTypes(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_PaperTypes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_FileTypes(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_FileTypes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_DatePrints(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_DatePrints, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_FileNamePrints(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_FileNamePrints, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_ImageOptimizes(pData)	dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_ImageOptimizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_Layouts(pData)			dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Layouts, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_FixedSizes(pData)		dpsDramReadBytes((UINT8 *)(pData, K_DPS_ADDR_Capatility_FixedSizes, K_DPS_ADDR_Capatility_Item_Size)
#define dpsLoadGetCapability_Croppings(pData)		dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_Capatility_Croppings, K_DPS_ADDR_Capatility_Item_Size)

//#define dpsLoadGetCapability_PaperTypesArrayCount(_count)	dpsDramReadBytes((UINT8 *)&_count, K_DPS_ADDR_Capatility_PaperTypesArray,2)
//#define dpsLoadGetCapability_PaperTypesArray(index,pData)	dpsDramReadBytes(pData, K_DPS_ADDR_Capatility_PaperTypesArray+1+ ((K_DPS_ADDR_Capatility_Item_Size*index)>>1), K_DPS_ADDR_Capatility_ItemArray_Size)
//#define dpsLoadGetCapability_LayoutsArray(index,pData)		dpsDramReadBytes(pData, K_DPS_ADDR_Capatility_LayoutsArray+1 + ((K_DPS_ADDR_Capatility_Item_Size*index)>>1), K_DPS_ADDR_Capatility_ItemArray_Size)

#define dpsLoadDeviceStatus(pData)		dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_DeviceStatus, K_DPS_ADDR_DeviceStatus_Size)
#define dpsLoadJobStatus(pData)			dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_JobStatus, K_DPS_ADDR_JobStatus_Size)

//sram storage
#define dpsLoadGucStorData()			dpsDramReadBytes(G_ucStorData, K_DPS_ADDR_StorSram0x1000, 1024)
#define dpsLoadGucStorData_intr()		sidcIntrDramReadBytes(G_ucStorData, K_DPS_ADDR_StorSram0x1000, 1024)
//configure service

#define dpsLoadConfigService(pData)			dpsDramReadBytes((UINT8 *)(pData), K_DPS_ADDR_ConfigPrintService, K_DPS_ADDR_ConfigPrintService_Size)

 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/




#endif
