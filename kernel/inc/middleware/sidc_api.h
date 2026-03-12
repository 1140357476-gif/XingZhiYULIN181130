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
 * @file      sidc_api.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SIDC_API_H__
#define __SIDC_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dps_addrDef.h" 
#include "sidc_param.h"
#include "sidc_struct.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DPS_MAX_INDEX				0x07
#define DPS_HREQUEST_INDEX		0x06
#define DPS_HRSPONSE_INDEX		0x05
#define DPS_DREQUEST_INDEX		0x04
#define DPS_DRSPONSE_INDEX		0x03
#define DPS_HDISCVRY_INDEX			0x02
#define DPS_DDISCVRY_INDEX			0x01
#define DPS_NULL_INDEX				0x00 

#define DPS_MASK_HAND			0x7FFFFF00
#define DPS_MASK_HAND_		0x000000ff

#define DPS_HREQUEST_HND		(DPS_MASK_HAND |(UINT32)DPS_HREQUEST_INDEX)//0x7FFFFF06
#define DPS_HRSPONSE_HND		(DPS_MASK_HAND |(UINT32)DPS_HRSPONSE_INDEX)//0x7FFFFF05
#define DPS_DREQUEST_HND		(DPS_MASK_HAND |(UINT32)DPS_DREQUEST_INDEX)//0x7FFFFF04
#define DPS_DRSPONSE_HND		(DPS_MASK_HAND |(UINT32)DPS_DRSPONSE_INDEX)//0x7FFFFF03
#define DPS_HDISCVRY_HND		(DPS_MASK_HAND |(UINT32)DPS_HDISCVRY_INDEX)//0x7FFFFF02
#define DPS_DDISCVRY_HND		(DPS_MASK_HAND |(UINT32)DPS_DDISCVRY_INDEX)//0x7FFFFF01 

#define SIDC_Null 0x00
#define SIDC_MainLoop 0x01
#define DPS_Null 0x00
#define DPS_MainLoop 0x01
//#define SIDC_Initialize 0x03

#define DPS_STATUS_Idle  0
#define DPS_STATUS_Request  1
#define DPS_STATUS_Response 2 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define checkValue(value,mask)  ((value & mask) == mask)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct 
{
	UINT32 handle;
	UINT8 fileName[13];
	UINT16 fileType;
	UINT32 fileSize;
	UINT8 reserved;
}sidc_node_t;
typedef enum
{
	SIDC_Flag_null=0, 
	SIDC_Flag_info, 
	SIDC_Flag_object, 
}sidc_outFlag_t;
#if 1
typedef struct
{
	UINT32 handle;
	UINT8 name[14];
	UINT16 flag;
	UINT32 size;	
	UINT32 addr;
	UINT8 container[12];
	UINT8 object[512];
}dps_script_t;
typedef struct
{
	UINT8 devData[512];
	dps_script_t hRequest;
	dps_script_t hRsponse;
	//dps_script_t hDiscvry;
	dps_script_t dRequest;
	dps_script_t dRsponse;
	//dps_script_t dDiscvry;
	//dps_script_t xdata*hScript;
	//dps_script_t xdata*dScript;
	dps_script_t hData;
	//dps_script_t hDataTmp;
	UINT16 exception;
}sidc_data_t;

#else
typedef struct
{
	UINT8 devData[1024];
	//UINT32 codeSize;
	//UINT16 responseCode;
	//UINT16 responseCount;
	//UINT32 responseParam[3];
	//UINT16 cmdCode;
	UINT32 bufSize;
	UINT8 bufStart[2];
	
	UINT32 inHandle;	
	UINT32 inSize;
	UINT32 inAddr;
	UINT8 inContainer[12];
	UINT8 inData[1024];
	UINT16 inFlag;//0:null,1:info,2:obj
	
	UINT32 outHandle;
	UINT32 outSize;
	UINT32 outAddr;
	UINT8 outContainer[12];
	UINT8 outData[1024];

	UINT16 outFlag;//0:null,1:info,2:obj
	UINT16 outInfoSize;
	UINT16 outObjSize;
	UINT8 outInfo[256];	
	UINT8 outObj[1024];
	//UINT8 xdata* outData;	

	UINT16 exception;
}sidc_data_t;
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 sidcFlag;
extern UINT8 dpsFlag;
extern UINT8 dpsStatus; 
extern sidc_data_t xdata*sidcData;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void sidcRequestObjectTransfer(UINT32 handle);
void sidcScriptFileNode(UINT32 handle,UINT32 fileSize,UINT8 isOut);
#define sidcOutFileNode(handle,size) sidcScriptFileNode(handle,size,1)
#define sidcInFileNode(handle,size) sidcScriptFileNode(handle,size,0)

void sidcSvcProc();
void sidcCommand();

void sidcIntrBulkOutProc();

void sidcMakeFolderHandle(UINT32* pHandle);
UINT8 sidcMakeFileHandle(UINT8 order,UINT32* pHandle,UINT8* fname,UINT8*pDate);
UINT8 sidcCheckImageFileType(UINT32 handle);

UINT32 sidcMakeHandles(UINT32 addr,UINT8 typeFilter);

#endif  /* __TEMPLATE_HDR_H__ */

