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
 * @file      dps_action.c
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

#include "dps_func.h" 
#include "os_msg.h"
#include "sidc_dbg.h"
#include "dbg_def.h"
#include "dbg_wdt.h"
#include "reg_usb.h"
#include "Sp1k_gpio_api.h"
#include "hal_dram.h"
#include "dps_param.h"
#include "reg_dram.h"
#include "dcf_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//UINT16 dpsDataStatus;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 #if USBSIDC_OPTION == 2
 #if 0
void dpsPrintAllHandles()
{
	UINT16 cnt = dcfFileCountGet();
	UINT16 i,idx=0;
	UINT32 handle;
	UINT32 addr = K_DPS_HandlesAddr;	
	sidcMakeFolderHandle(&handle);	
	
	for(i=dpsData->imageIndex;i<cnt;i++)
	{
		if(sidcMakeFileHandle(&handle,i))
		{			
			HAL_DramStartAddrSet(addr, 0);
			HAL_DramWrite32(handle);
			idx++;
			addr+=2;
			dpsPrintfInfo("%04u-%04u-%lx\n",i,idx,handle);
		}
	}
	dpsData->imageHandles = addr;	
	dpsData->imageCount = idx;
	dpsData->imageIndex = 0;
	dpsData->imageStep = 0;
	dpsData->imageMax = 24;
	dpsPrintfInfo("%4u-%4u-%4u-%8lx\n",dpsData->imageIndex,dpsData->imageCount,dpsData->imageMax,dpsData->imageHandles);
}

void dpsPrintMultiCount()
{
	dpsData->imageMulti = 80;
	switch(dpsData->startJob.paperSize)
	{
		case K_DPS_HEX_PARAVAL_PaperSize_Default:			//0x51000000
		case	K_DPS_HEX_PARAVAL_PaperSize_L:				//0x51010000
		case	K_DPS_HEX_PARAVAL_PaperSize_2L:				//0x51020000
		case	K_DPS_HEX_PARAVAL_PaperSize_HagakiPostcard:	//0x51030000
		case	K_DPS_HEX_PARAVAL_PaperSize_CardSize:		//0x51040000
		case	K_DPS_HEX_PARAVAL_PaperSize_100mmX150mm:	//0x51050000
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_4X6in:			//0x51060000
			dpsData->imageMulti = 24;
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_8X10in:			//0x51070000
			dpsData->imageMulti = 72;
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_Letter:			//0x51080000
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_11X17in:		//0x510A0000
		case	K_DPS_HEX_PARAVAL_PaperSize_A0:				//0x51100000
		case	K_DPS_HEX_PARAVAL_PaperSize_A1:				//0x51110000
		case	K_DPS_HEX_PARAVAL_PaperSize_A2:				//0x51120000
		case	K_DPS_HEX_PARAVAL_PaperSize_A3:				//0x51130000
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_A4:				//0x51140000
			break;
		case	K_DPS_HEX_PARAVAL_PaperSize_A5:				//0x51150000
		case	K_DPS_HEX_PARAVAL_PaperSize_A6:				//0x51160000
		case	K_DPS_HEX_PARAVAL_PaperSize_A7:				//0x51170000
		case	K_DPS_HEX_PARAVAL_PaperSize_A8:				//0x51180000
		case	K_DPS_HEX_PARAVAL_PaperSize_A9:				//0x51190000
		case	K_DPS_HEX_PARAVAL_PaperSize_B0:				//0x51200000
		case	K_DPS_HEX_PARAVAL_PaperSize_B1:				//0x51210000
		case	K_DPS_HEX_PARAVAL_PaperSize_B2:				//0x51220000
		case	K_DPS_HEX_PARAVAL_PaperSize_B3:				//0x51230000
		case	K_DPS_HEX_PARAVAL_PaperSize_B4:				//0x51240000
		case	K_DPS_HEX_PARAVAL_PaperSize_B5:				//0x51250000
		case	K_DPS_HEX_PARAVAL_PaperSize_B6:				//0x51260000
		case	K_DPS_HEX_PARAVAL_PaperSize_B7:				//0x51270000
		case	K_DPS_HEX_PARAVAL_PaperSize_B8:				//0x51280000
		case	K_DPS_HEX_PARAVAL_PaperSize_B9:				//0x51290000
		case	K_DPS_HEX_PARAVAL_PaperSize_89mmRoll:		//0x51810000
		case	K_DPS_HEX_PARAVAL_PaperSize_127mmRoll:		//0x51820000
		case	K_DPS_HEX_PARAVAL_PaperSize_100mmRoll:		//0x51860000
		case	K_DPS_HEX_PARAVAL_PaperSize_210mmRoll:		//0x51940000
		break;
	}
}
#endif
/**
 * @fn		  void dpsActionResponse(UINT8 *pData, UINT8 treeType)
 * @brief	  dpsActionResponse
 * @param	  [in] pData
 * @param	  [in] treeType
 * @retval	  NONE
 * @author	  wangjg
 * @since	  2010-5-12
*/
void dpsActionResponse(void * pData, UINT8 treeType)
{
	dps_script_t* script = &sidcData->dRsponse;
	UINT32 waitCnt = 100000;
	UINT16 xmlSize = xmlTreeMake(pData, K_DPS_ADDR_InData, treeType);
	UINT32 handle = DPS_DRSPONSE_HND;
	dpsStatus = DPS_STATUS_Response;
	script->handle = handle;		
	script->size = xmlSize;
	script->addr = K_DPS_ADDR_InData-6;
	dpsPrintfFunc("dpsResponse:%bx\n",treeType);
	
	sidcRequestObjectTransfer(handle);
	sidcFlag = SIDC_Null;
	/*
	dbgWdtFeed(-1);	
	XBYTE[0x2504] |= 0x10;		
	XBYTE[0x2546] |= 0x10;		
	while(--waitCnt )//&& dpsIsNewResponse())
	{
		sidcSvcProc();
		dpsSvcProc();
		if(!dpsStatus){
			dpsPrintfAction("Response complete!\n");
			return ;
		}
	}
	sidcPrintfErr("dpsResponse time out:%lx,%bx\n",handle,dpsStatus);
	*/
}
/**
 * @fn		  void dpsActionRequest(UINT8 *pData, UINT8 treeType)
 * @brief	  dpsActionRequest
 * @param	  [in] pData
 * @param	  [in] treeType
 * @retval	  NONE
 * @author	  wangjg
 * @since	  2010-5-13
*/
void dpsActionRequest(void *pData, UINT8 treeType)
{
	dps_script_t* script = &sidcData->dRequest ;
	UINT32 waitCnt = 1000000;
	UINT16 xmlSize;
	UINT32 handle = DPS_DREQUEST_HND;
	
	dpsPrintfFunc("Request:%bx,sts:%bx,flag:%bx\n",treeType,dpsStatus,sidcFlag);
	
	if(dpsStatus || sidcFlag)
	{		
		waitCnt = 10000;//waiting sendObj ok
		XBYTE[REG_Usb_EpAckIntEn] |= 0x10;		
		XBYTE[REG_Usb_TransactionEn] |= 0x10;	
		while(--waitCnt){
			dbgWdtFeed(-1);	
			sidcSvcProc();
			dpsSvcProc();
			if(!sp1kUSB_PlugGet()){
				sidcPrintfErr("USB plug out-1\n");
				return;
			}
			if((!dpsStatus) && (!sidcFlag)){
				break;
			}			
		}
		if(!waitCnt)	{
			sidcData->exception = 1;
			sidcPrintfErr("TimeOut:%lx,sts:%bx\n",handle,dpsStatus);
			return;
		}
	}
	dpsPrintfFunc("sts:%bx,flag:%bx\n",dpsStatus,sidcFlag);
	
	XBYTE[REG_Usb_TransactionEn] &= 0xef;//OFF bulkout
	XBYTE[REG_Usb_EpAckIntEn] &= 0xef;//OFF bulkout interrupt
	dpsStatus = DPS_STATUS_Request;
	
	if(treeType==DPS_ACTION_StartJob) script->addr = K_SDRAM_FreeAddr+4*10000;
	else script->addr = K_DPS_ADDR_InData-6;

	dbgWdtFeed(-1);
	xmlSize = xmlTreeMake(pData, script->addr+6/*K_DPS_ADDR_InData*/, treeType);
	handle = DPS_DREQUEST_HND;

	script->handle = handle;		
	script->size = xmlSize;

	dpsPrintfFunc("Request:%bx,sts:%bx\n",treeType,dpsStatus);
	sidcData->exception = 0;
	 	
	waitCnt = 1000000;	
	sidcRequestObjectTransfer(handle);	
	dbgWdtFeed(-1);	
	XBYTE[REG_Usb_EpAckIntEn] |= 0x10;		
	XBYTE[REG_Usb_TransactionEn] |= 0x10;	
	//HAL_GlobalWDTSet(0);
	while(--waitCnt){
		dbgWdtFeed(-1);
		sidcSvcProc();
		dpsSvcProc();
		if(!sp1kUSB_PlugGet()){
			dpsPrintfAction("USB plug out-2\n");
			return;
		}
		if(!dpsStatus)	{
			dpsData->action = treeType;
			dpsPrintfAction("Compete!\n");
			return;
		}
		if(sidcData->exception){
			sidcPrintfErr("Exception:%bx\n",sidcData->exception);
			return;
		}
	}
	sidcData->exception = 2;
	sidcPrintfErr("TimeOut:%lx,sts:%bx\n",handle,dpsStatus);
		
}

/**
 * @fn        void dpsActionConfigurePrintService(void)
 * @brief     dpsActionConfigurePrintService
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionConfigurePrintService()
{
	UINT8 pData[DPS_PARA_ConfigPrintService_Size];
	PDPS_PARA_ConfigPrintService pdpsOp_ParaConfigPrintService= (PDPS_PARA_ConfigPrintService)pData;
	UINT8 name[10];
	dpsPrintfAction("printService\n");
	memset(name,0,10);
	pdpsOp_ParaConfigPrintService->printServiceAvailable= 0;
	pdpsOp_ParaConfigPrintService->reserved = 0;

	dpsGetPrintConfigData(0,name);	
	strcpy((char *)pdpsOp_ParaConfigPrintService->dpsVersions,(char *)name);
	dpsGetPrintConfigData(1,name);	
	strcpy((char *)pdpsOp_ParaConfigPrintService->vendorName,(char *)name);
	dpsGetPrintConfigData(2,name);	
	strcpy((char *)pdpsOp_ParaConfigPrintService->vendorSpecVer,(char *)name);
	dpsGetPrintConfigData(3,name);	
	strcpy((char *)pdpsOp_ParaConfigPrintService->productName,(char *)name);
	dpsGetPrintConfigData(4,name);	
	strcpy((char *)pdpsOp_ParaConfigPrintService->serialNo,(char *)name);
	
	dpsActionRequest(pData, DPS_ACTION_ConfigurePrintService);
}
/**
 * @fn        void dpsActionGetCapability(UINT8 capabilityType, capabilityType,UINT32 param)
 * @brief     dpsActionGetCapability
 * @param     [in] capabilityType
 * @param     [in] param
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionGetCapability(UINT8 capabilityType,UINT32 param)
{
	dpsPrintfAction("GetCap-%bx,%lx\n",capabilityType,param);
	dpsActionRequest((UINT8 *)&param, capabilityType);//(capabilityType - DPS_ACTION_GetCapability)+DPS_ACTION_GetCapability);
}
/**
 * @fn        void dpsActionStartJob(void)
 * @brief     dpsActionStartJob
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionStartJob()
{
	dpsPrintfAction("startJob\n");
	if(dpsData->imageCount!=0)
		dpsActionRequest(dpsData, DPS_ACTION_StartJob);
}
/**
 * @fn        void dpsActionAbortJob(UINT32 param)
 * @brief     dpsActionAbortJob
 * @param     [in] param
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionAbortJob(UINT32 param)
{
	dpsPrintfAction("AbortJob\n");
	dpsActionRequest((UINT8 xdata*)&param , DPS_ACTION_AbortJob);
}
/**
 * @fn        void dpsActionContinueJob(void)
 * @brief     dpsActionContinueJob
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionContinueJob()
{
	dpsPrintfAction("ContinueJob\n");
	dpsActionRequest(NULL , DPS_ACTION_ContinueJob);
}
/**
 * @fn        void dpsActionGetDeviceStatus(void)
 * @brief     dpsActionGetDeviceStatus
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionGetDeviceStatus()
{
	dpsPrintfAction("GetDev\n");
	dpsActionRequest(NULL , DPS_ACTION_GetDeviceStatus);
}
/**
 * @fn        void dpsActionGetJobStatus(void)
 * @brief     dpsActionGetJobStatus
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsActionGetJobStatus()
{
	dpsPrintfAction("GetJob\n");
	dpsActionRequest(NULL , DPS_ACTION_GetJobStatus);
}
#if 0
dps_script_t* dpsCheckCmd()
{
	dps_script_t *src = &sidcData->hDataTmp;
	dps_script_t *dst= (dps_script_t *)(src->addr);
	UINT32 handle = src->handle;
	UINT32 size = src->size;
	if(dst==NULL){
		return NULL;
	}
	if(handle==0 || size==0){
		return NULL;
	}
	if(handle == DPS_HRSPONSE_HND) {
		dpsPrintfInfo("dpsComplete!\n");
		dpsStatus = DPS_STATUS_Idle;		
	}else if(handle==DPS_HREQUEST_HND)	{
		if(dpsStatus==DPS_STATUS_Request){
			sidcPrintfErr("collision\n");	
			return NULL;	
		}
	}
	memcpy(dst, src, sizeof(dps_script_t));
	return dst;
}
#endif
/**
 * @fn        void dpsCommand(void)
 * @brief     dpsCommand
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void dpsCommand(dps_script_t* script)
{
	UINT8 pData[DPS_PARA_Max_Size];
	UINT32 handle = script->handle;//= sidcData->outHandle;
	UINT16 size = script->size;
	UINT8 treeType;
	UINT8 xdata* ptObj = script->object;	
	dpsPrintfFunc("dpsCmd:%lx,%u\n",handle,size);
	script->handle = 0;
	if(handle==0 || size==0){
		sidcPrintfErr("return\n");	
		return;
	}
	if(handle == DPS_HRSPONSE_HND && dpsStatus==DPS_STATUS_Request){
		dpsStatus = DPS_STATUS_Idle;
	}
	if(handle == DPS_HREQUEST_HND && dpsStatus != DPS_STATUS_Idle/*dpsStatus==DPS_STATUS_Request*/)	{
		sidcPrintfErr("collision\n");	
		return;	
	}	
	dbgWdtFeed(-1);	
	treeType= xmlTreeParse( pData, ptObj/*K_DPS_ADDR_OutData*/, size);
	dpsPrintfInfo("type:%bx\n",treeType);
	switch(treeType)
	{
	// result err
	case DPS_ACTION_Err_Notify:
		sidcData->exception = 3;
		sidcPrintfErr("NotifyErr\n");
		dpsActionResponse(pData, treeType);
		dpsSendMsg(SP1K_MSG_DPS_ERROR);
		break;
	case DPS_ACTION_Err_Item:
		sidcData->exception = 4;
		sidcPrintfErr("ItemErr\n");
		if((handle == DPS_DREQUEST_INDEX)  || (handle == DPS_HREQUEST_INDEX))
			dpsActionResponse(pData, treeType);
		dpsSendMsg(SP1K_MSG_DPS_ERROR);
		break;
	case DPS_ACTION_Err_Result:
		sidcData->exception = 5;
		sidcPrintfErr("ResultErr\n");
		dpsSendMsg(SP1K_MSG_DPS_ERROR);
		break;
	
	// request
	case DPS_ACTION_NotifyDeviceStatus:
		memcpy(&(dpsData->deviceStatus),pData,sizeof(DPS_PARA_DeviceStatus));
		sidcPrintf("notifyDeviceStatus\n");
		//dpsSaveDeviceStatus(pData);
		dpsAddFlagDeviceStatus();
		*((UINT32 *)pData) = K_DPS_HEX_RESULT_Ok;
		dpsActionResponse(pData, treeType);
		//dpsDeviceMsg();
		dpsSendMsg(SP1K_MSG_DPS_NEW_DEVICE_STATUS);
		break;
	case DPS_ACTION_NotifyJobStatus:
		memcpy(&(dpsData->jobStatus),pData,sizeof(DPS_PARA_JobStatus));
		sidcPrintf("notifyJobStatus\n");
		dpsAddFlagJobStatus();
		//dpsSaveJobStatus(pData);
		*((UINT32 *)pData) = K_DPS_HEX_RESULT_Ok;
		dpsActionResponse(pData, treeType);
		//dpsJobMsg();
		dpsSendMsg(SP1K_MSG_DPS_NEW_JOB_STATUS);
		break;
		
	// responce
	case DPS_ACTION_ConfigurePrintService:
		memcpy(&(dpsData->printService),pData,sizeof(DPS_PARA_ConfigPrintInfo));
		sidcPrintf("printService\n");
		//dpsSaveConfigService(pData);
		dpsAddFlagConfigurePrintService();
		dpsSendMsg(SP1K_MSG_DPS_NEW_CONFIG_PRINTER);
		break;
	//case DPS_ACTION_GetCapability:
	case DPS_ACTION_GetCapability_Qualities:			
	case DPS_ACTION_GetCapability_PaperSizes:
	case DPS_ACTION_GetCapability_PaperTypes:
  	case DPS_ACTION_GetCapability_FileTypes:
  	case DPS_ACTION_GetCapability_DatePrints:
  	case DPS_ACTION_GetCapability_FileNamePrints:
  	case DPS_ACTION_GetCapability_ImageOptimizes	:
  	case DPS_ACTION_GetCapability_Layouts	:
  	case DPS_ACTION_GetCapability_FixedSizes	:		
  	case DPS_ACTION_GetCapability_Croppings	:
		memcpy(&(dpsData->capability+treeType - DPS_ACTION_GetCapability_Qualities)
			,pData,sizeof(DPS_PARA_Capability_Elem));
		sidcPrintf("Capability:%bx\n",treeType);
		//dpsSaveGetCapability_Item(treeType, pData);
		dpsAddFlagCapability(treeType);
		break;

	// get status	
	case DPS_ACTION_GetDeviceStatus:
		memcpy(&(dpsData->deviceStatus),pData,sizeof(DPS_PARA_DeviceStatus));
		sidcPrintf("getDeviceStatus\n");
		//dpsSaveDeviceStatus(pData);
		dpsAddFlagDeviceStatus();
		//dpsDeviceMsg();
		//dpsSendMsg(SP1K_MSG_DPS_NEW_DEVICE_STATUS);
		break;
	case DPS_ACTION_GetJobStatus:
		memcpy(&(dpsData->jobStatus),pData,sizeof(DPS_PARA_JobStatus));
		sidcPrintf("GetJobStatus\n");
		//dpsSaveJobStatus(pData);
		dpsAddFlagJobStatus();
		//dpsJobMsg();
		//dpsSendMsg(SP1K_MSG_DPS_NEW_JOB_STATUS);
		break;

	case DPS_ACTION_StartJob:
	case DPS_ACTION_ContinueJob:
		dpsSendMsg(SP1K_MSG_DPS_START_JOB);
		break;
			
	default:
		break;
	}

}
#if 0
UINT8 dpsCheckObjectInfo()
{
	UINT8 i;	
	sidc_container_t* pContainer = (sidc_container_t*) sidcData->outContainer;
	UINT32 size = M_ByteSwapOfDword(pContainer->ulContainerLength);
	sidc_object_info_t* pObjectInfo  = (sidc_object_info_t*)sidcData->outData ;	
	UINT32 handle=0;//=sidcData->outHandle;	
	UINT8 objName[13];
	//UINT8 outFlag = sidcData->outFlag;
	UINT32 objSize = M_ByteSwapOfDword(pObjectInfo->objectCompressedSize);

	sidcData->outFlag = SIDC_Flag_null;
	
	if(pObjectInfo->objectFormat != K_PIMAOBJ_Script){
		//sidcData->outFlag = SIDC_Flag_null;
		return FALSE;
	}
	
	for(i=0;i<12;i++){
		objName[i]=pObjectInfo->fileNameBuf[i*2+1];
	}
	objName[12]=0;
	//dpsGetScriptInfo(&handle, objName);
	if (strcmp(objName,"HREQUEST.DPS") == 0){
		handle = DPS_HREQUEST_HND;
	}else if (strcmp(objName,"HRSPONSE.DPS") == 0){
		handle = DPS_HRSPONSE_HND;
	}else if (strcmp(objName,"HDISCVRY.DPS") == 0){
		handle = DPS_HDISCVRY_HND;
	}else{
		//sidcData->outFlag = SIDC_Flag_null;
		return FALSE;
	}
	//printf("handle name:%s\n",objName);
	if(handle == DPS_HDISCVRY_HND ){
		//dpsStatus = DPS_STATUS_Request;
		//dpsAddFlagHDiscvry();
		//dpsPrintfInfo("newDpsDevice!\n");
		dpsSendMsg(SP1K_MSG_DPS_NEW_HDISCVRY);
		//sidcData->outFlag = SIDC_Flag_null;
		return FALSE;
	}
	else if(dpsStatus==DPS_STATUS_Request && handle==DPS_HREQUEST_HND)
	{
		//sidcPrintfErr("collision\n");	
		//sidcData->outFlag = outFlag;
		return FALSE;	
	}
	
	sidcData->outHandle = handle;
	sidcData->outFlag = SIDC_Flag_info;
	sidcData->outInfoSize = size-12;
	memcpy(sidcData->outInfo,sidcData->outData,size-12);
	//dpsPrintfAction("dpsCheckObjInfo-%lx,%s,%lu,%lu\n",handle,objName,objSize,objSize);
	return TRUE;
}
UINT8 dpsCheckObject()
{
	//UINT8 i;
	sidc_container_t* pContainer = (sidc_container_t*) sidcData->outContainer;
	UINT32 size = M_ByteSwapOfDword(pContainer->ulContainerLength);
	//sidc_object_info_t* pObjectInfo  = (sidc_object_info_t*)sidcData->outInfo ;	
	UINT32 handle=sidcData->outHandle;	
	//UINT32 objSize = M_ByteSwapOfDword(pObjectInfo->objectCompressedSize);
	if(sidcData->outFlag != SIDC_Flag_info)	{
		sidcPrintfErr("collision\n");	
		return FALSE;
	}else if(dpsStatus==DPS_STATUS_Request && handle==DPS_HREQUEST_HND)	{
		sidcPrintfErr("collision\n");	
		return FALSE;	
	}
	sidcData->outFlag = SIDC_Flag_null;
	
	if(handle == DPS_HRSPONSE_HND) {
		dpsPrintfInfo("dpsComplete!\n");
		dpsStatus = DPS_STATUS_Idle;		
	}		
	//sidcFlag=DPS_MainLoop;
	sidcData->outObjSize = size-12;
	memcpy(sidcData->outObj,sidcData->outData,size-12);
	//dpsCommand();
	return TRUE;		
	
}
#endif

void dpsSvcProc()
{
#if USBSIDC_OPTION == 2
	if(dpsFlag==DPS_MainLoop){	
	if(sidcData->hRequest.handle || sidcData->hRsponse.handle){
		XBYTE[0x2546] &= 0xef;
		XBYTE[0x2504] &= 0xef;//OFF bulkout interrupt
		//sidcFlag=SIDC_Null; 
		//dpsCommand();			
		if(sidcData->hRsponse.handle){
			dpsCommand(&sidcData->hRsponse);
		}
		if(sidcData->hRequest.handle){
			dpsCommand(&sidcData->hRequest);
		}
		XBYTE[0x2504] |= 0x10;//on bulkout interrupt
		XBYTE[0x2546] |= 0x10;	
	}}
#endif
}

#endif

