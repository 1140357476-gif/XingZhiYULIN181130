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
 * @file      dps_api.h
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __TEMPLATE_HDR_H__
#define __TEMPLATE_HDR_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dps_param.h"
#include "dps_struct.h"
#include "dps_addrdef.h"
#include "usbopt.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DPS_ACTION_Idle	0x00
#define DPS_ACTION_Err	0x70
#define DPS_ACTION_Err_XMLStruct	0x71	//xml structure err
#define DPS_ACTION_Err_Result		0x72	//response the value of result is not OK(0x10000000)
#define DPS_ACTION_Err_Notify	0x73	//notify xml structure err
#define DPS_ACTION_Err_Item	0x74	//request item name err
#define DPS_ACTION_ConfigurePrintService 0x10
#define DPS_ACTION_GetCapability 0x20
#define DPS_ACTION_GetCapability_Qualities	0x20
#define DPS_ACTION_GetCapability_PaperSizes	0x21
#define DPS_ACTION_GetCapability_PaperTypes	0x22 
#define DPS_ACTION_GetCapability_FileTypes	0x23
#define DPS_ACTION_GetCapability_DatePrints	0x24
#define DPS_ACTION_GetCapability_FileNamePrints	0x25
#define DPS_ACTION_GetCapability_ImageOptimizes	0x26
#define DPS_ACTION_GetCapability_Layouts	0x27
#define DPS_ACTION_GetCapability_FixedSizes	0x28
#define DPS_ACTION_GetCapability_Croppings	0x29
#define DPS_ACTION_StartJob 0x30
#define DPS_ACTION_AbortJob 0x3a
#define DPS_ACTION_ContinueJob 0x3b
#define DPS_ACTION_GetDeviceStatus 0x40
#define DPS_ACTION_GetJobStatus 0x41
#define DPS_ACTION_Response	0x50
#define DPS_ACTION_NotifyJobStatus		0x51
#define DPS_ACTION_NotifyDeviceStatus		0x52 
	
#define DPS_ACTION_MASK	0xF0
#define DPS_ACTION_MASK_	0X0F 
#define dps_checkIsAction_GetCapability(action)	((action & DPS_ACTION_MASK)==DPS_ACTION_GetCapability)
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
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

void dpsSvcProc();
void dpsCmd(UINT8 *param);

UINT8 dpsCheckStauts(UINT32 handle);
UINT8 dpsCheckObjectInfo();
UINT8 dpsCheckObject();

#endif  /* __TEMPLATE_HDR_H__ */
