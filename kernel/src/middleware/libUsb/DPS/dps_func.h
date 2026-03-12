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
 * @file      dps_func.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DPS_FUNC_H__
#define __DPS_FUNC_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sidc_api.h" 
#include "sp1k_dps_api.h"
#include "dps_res.h"
#include "dps_api.h"
#include "xml_api.h"
#include "usbopt.h"
#include "sidc_dbg.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

//dpsDataStatus
#define DPS_DATA_Capability_Qualities	0x00000001ul
#define DPS_DATA_Capability_PaperSizes	0x00000002ul
#define DPS_DATA_Capability_PaperTypes	0x00000004ul 
#define DPS_DATA_Capability_FileTypes	0x00000008ul
#define DPS_DATA_Capability_DatePrints	0x00000010ul
#define DPS_DATA_Capability_FileNamePrints	0x00000020ul
#define DPS_DATA_Capability_ImageOptimizes	0x00000040ul
#define DPS_DATA_Capability_Layouts			0x00000080ul
#define DPS_DATA_Capability_FixedSizes		0x00000100ul
#define DPS_DATA_Capability_Croppings		0x00000200ul
#define DPS_DATA_ConfigurePrintService	0x00001000ul
#define DPS_DATA_JobStatus				0x00002000ul
#define DPS_DATA_DeviceStatus			0x00004000ul
#define DPS_DATA_HDiscvry				0x00008000ul
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define dpsTransformActionToDataStatusCapability(action) 		(0x0001<<(action - DPS_ACTION_GetCapability))
#define dpsIsNewCapability(action)			(dpsDataStatus & dpsTransformActionToDataStatusCapability(action))
#define dpsIsNewQualites()					(dpsDataStatus & DPS_DATA_Capability_Qualities)
#define dpsIsNewPaperSizes()				(dpsDataStatus & DPS_DATA_Capability_PaperSizes)
#define dpsIsNewPaperTypes()				(dpsDataStatus & DPS_DATA_Capability_PaperTypes)
#define dpsIsNewFileTypes()					(dpsDataStatus & DPS_DATA_Capability_FileTypes)
#define dpsIsNewDatePrints()				(dpsDataStatus & DPS_DATA_Capability_DatePrints)
#define dpsIsNewFileNamePrints()			(dpsDataStatus & DPS_DATA_Capability_FileNamePrints)
#define dpsIsNewImageOptimizes()			(dpsDataStatus & DPS_DATA_Capability_ImageOptimizes)
#define dpsIsNewLayouts()					(dpsDataStatus & DPS_DATA_Capability_Layouts)
#define dpsIsNewFixedSizes()				(dpsDataStatus & DPS_DATA_Capability_FixedSizes)
#define dpsIsNewCroppings()					(dpsDataStatus & DPS_DATA_Capability_Croppings)
#define dpsIsNewConfigurePrintService()		(dpsDataStatus & DPS_DATA_ConfigurePrintService)
#define dpsIsNewJobStatus()					(dpsDataStatus & DPS_DATA_JobStatus)
#define dpsIsNewDeviceStatus()				(dpsDataStatus & DPS_DATA_DeviceStatus)
#define dpsIsNewHDiscvry()					(dpsDataStatus & DPS_DATA_HDiscvry)

#define dpsAddFlagCapability(action)		dpsDataStatus |= dpsTransformActionToDataStatusCapability(action)
#define dpsAddFlagQualites()				dpsDataStatus |= DPS_DATA_Capability_Qualities
#define dpsAddFlagPaperSizes()				dpsDataStatus |= DPS_DATA_Capability_PaperSizes
#define dpsAddFlagPaperTypes()				dpsDataStatus |= DPS_DATA_Capability_PaperTypes
#define dpsAddFlagFileTypes()				dpsDataStatus |= DPS_DATA_Capability_FileTypes
#define dpsAddFlagDatePrints()				dpsDataStatus |= DPS_DATA_Capability_DatePrints
#define dpsAddFlagFileNamePrints()			dpsDataStatus |= DPS_DATA_Capability_FileNamePrints
#define dpsAddFlagImageOptimizes()			dpsDataStatus |= DPS_DATA_Capability_ImageOptimizes
#define dpsAddFlagLayouts()					dpsDataStatus |= DPS_DATA_Capability_Layouts
#define dpsAddFlagFixedSizes()				dpsDataStatus |= DPS_DATA_Capability_FixedSizes
#define dpsAddFlagCroppings()				dpsDataStatus |= DPS_DATA_Capability_Croppings
#define dpsAddFlagConfigurePrintService()	dpsDataStatus |= DPS_DATA_ConfigurePrintService
#define dpsAddFlagJobStatus()				dpsDataStatus |= DPS_DATA_JobStatus
#define dpsAddFlagDeviceStatus()			dpsDataStatus |= DPS_DATA_DeviceStatus
#define dpsAddFlagHDiscvry()				dpsDataStatus |= DPS_DATA_HDiscvry

#define dpsClearFlagCapability(action)		dpsDataStatus &= ~(dpsTransformActionToDataStatusCapability(action))
#define dpsClearFlagQualites()				dpsDataStatus &= ~DPS_DATA_Capability_Qualities
#define dpsClearFlagPaperSizes()			dpsDataStatus &= ~DPS_DATA_Capability_PaperSizes
#define dpsClearFlagPaperTypes()			dpsDataStatus &= ~DPS_DATA_Capability_PaperTypes
#define dpsClearFlagFileTypes()				dpsDataStatus &= ~DPS_DATA_Capability_FileTypes
#define dpsClearFlagDatePrints()			dpsDataStatus &= ~DPS_DATA_Capability_DatePrints
#define dpsClearFlagFileNamePrints()		dpsDataStatus &= ~DPS_DATA_Capability_FileNamePrints
#define dpsClearFlagImageOptimizes()		dpsDataStatus &= ~DPS_DATA_Capability_ImageOptimizes
#define dpsClearFlagLayouts()				dpsDataStatus &= ~DPS_DATA_Capability_Layouts
#define dpsClearFlagFixedSizes()			dpsDataStatus &= ~DPS_DATA_Capability_FixedSizes
#define dpsClearFlagCroppings()				dpsDataStatus &= ~DPS_DATA_Capability_Croppings
#define dpsClearFlagConfigurePrintService()	dpsDataStatus &= ~DPS_DATA_ConfigurePrintService
#define dpsClearFlagJobStatus()				dpsDataStatus &= ~DPS_DATA_JobStatus
#define dpsClearFlagDeviceStatus()			dpsDataStatus &= ~DPS_DATA_DeviceStatus
#define dpsClearFlagHDiscvry()				dpsDataStatus &= ~DPS_DATA_HDiscvry

//msg
#define dpsSendMsg(msg) osMsgPost(msg)
//

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 dpsStatus; 
#define dpsDataStatus (dpsData->dataStatus)//paramList[DPS_PARAM_DpsStatus])
extern dps_data_t *dpsData;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//dps Action
void dpsActionConfigurePrintService();
void dpsActionGetCapability(UINT8 capabilityType,UINT32 param);
void dpsActionStartJob();
void dpsActionAbortJob(UINT32 param);
void dpsActionContinueJob();
void dpsActionGetDeviceStatus();
void dpsActionGetJobStatus();
void dpsActionResponse(void * pData, UINT8 treeType);
void dpsActionRequest(void *pData, UINT8 treeType);
#endif  /* __TEMPLATE_HDR_H__ */

