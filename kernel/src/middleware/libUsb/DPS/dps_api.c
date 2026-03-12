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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dps_func.h"
#include "uiflow.h"
#include "hal_gprm.h" 
#include "dbg_mem.h"
#include "dcf_api.h"
#include "app_dps.h"
#if FUNC_HOST_DPS
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
//UINT32 dps_param[DPS_PARAM_MAX];
dps_data_t *dpsData = (dps_data_t xdata*)(G_FPGAMemPool+sizeof(sidc_data_t));

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 sp1kDpsAction(UINT8 action, UINT32 param)
 * @brief     dps do action
 * @param     [in] action	dps action idx
 * @param     [in] param	dps action param
 * @retval    return = DPS_ACTION_Idle / DPS_ACTION_Err
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsAction(UINT8 action, UINT32 param)
{
#if (USBSIDC_OPTION==2)
	UINT8 ret = DPS_ACTION_Idle;
	sidcPrintfFunc("doAction",action);
	switch(action)
	{
	case DPS_ACTION_ConfigurePrintService:
		dpsActionConfigurePrintService();
		break;
	//case DPS_API_ACTION_GetCapability:
	case DPS_ACTION_GetCapability_Qualities	:
	case DPS_ACTION_GetCapability_PaperSizes	:
	case DPS_ACTION_GetCapability_PaperTypes	: 
	case DPS_ACTION_GetCapability_FileTypes	:
	case DPS_ACTION_GetCapability_DatePrints	:
	case DPS_ACTION_GetCapability_FileNamePrints	:
	case DPS_ACTION_GetCapability_ImageOptimizes	:
	case DPS_ACTION_GetCapability_Layouts	:
	case DPS_ACTION_GetCapability_FixedSizes	:
	case DPS_ACTION_GetCapability_Croppings	:
		dpsActionGetCapability(action ,param);
		break;
	case DPS_ACTION_StartJob:
		dpsActionStartJob();
		break;
	case DPS_ACTION_AbortJob:
		dpsActionAbortJob( param);
		break;
	case DPS_ACTION_ContinueJob:
		dpsActionContinueJob();
		break;
	case DPS_ACTION_GetDeviceStatus:
		dpsActionGetDeviceStatus();
		break;
	case DPS_ACTION_GetJobStatus:
		dpsActionGetJobStatus();
		break;
	default:
		ret=DPS_ACTION_Err;
		break;
	
	}

	//g_dpsLastNotResponseAction= action;
	return ret;
#else
	 action = action;
	 param = param;
	 return 0;
#endif
}
/*
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
*/
/**
 * @fn        UINT8 sp1kDpsAddImage(UINT32 handle, UINT8 *pfileName, UINT16 copies, UINT16 cropping_x, UINT16 cropping_y, UINT16 cropping_w, UINT16 cropping_h, UINT8 *pDate)
 * @brief     dps add image to print list
 * @param     [in] handle
 * @param     [in] pfileName
 * @param     [in] copies
 * @param     [in] cropping_x
 * @param     [in] cropping_y
 * @param     [in] cropping_w
 * @param     [in] cropping_h
 * @param     [in] pDate
 * @retval    return = print file list count
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsAddImage(UINT32 handle, UINT8 *pfileName, UINT16 copies,
	UINT16 cropping_x, UINT16 cropping_y, UINT16 cropping_w, UINT16 cropping_h,
	UINT8 *pDate
	)

{
#if (USBSIDC_OPTION==2)
	UINT16 i;
	DPS_PARA_ConfigPrintInfo *cpi = dpsData->imageList+dpsData->imageCount;
	DPS_PARA_ConfigStartJob *csj = &(dpsData->startJob);
	sidcPrintfFunc("dpsAddFile-%lx\n",handle);
	if(dpsData->imageMax==0)dpsData->imageMax = 999;
	if(dpsData->imageListMax==0)dpsData->imageListMax = 24;
	if(dpsData->imageListMax>dpsData->imageMax)
		dpsData->imageListMax=dpsData->imageMax;	
	if(handle==0){
		sidcPrintfInfo("printf All\n");
		cpi = dpsData->imageList;	
		dbgWdtFeed(-1);
		dpsData->imageHandles = K_DPS_HandlesAddr;		
		dpsData->imageCount = dcfFileCountGet();//sidcMakeHandles(dpsData->imageHandles-2, K_DCF_JPG_TYPE);
		/*if(dpsData->imageCount*copies>dpsData->imageMax){
			return 0xffff;
		}*/
	}else{
		sidcPrintfInfo("printf One\n");
		if(dpsData->imageHandles!=0 /*|| dpsData->action == DPS_ACTION_AbortJob*/){
			dpsData->imageCount = 0;
			dpsData->imageHandles = 0;
		}
		if(!sidcCheckImageFileType(handle))
			return dpsData->imageCount;		
		if(dpsData->imageCount >= dpsData->imageListMax)
			return dpsData->imageCount;

		//in choose print mode,add new image info to empty or invalid position
		dpsData->imageCount++;
		for(i = 0;i < dpsData->imageListMax;i ++){	 
			if(dpsData->imageList[i].inValid == 1){
				sidcPrintfInfo("invalid:%u\n",i);
				cpi = &(dpsData->imageList) + i;
				dpsData->imageCount--;
				break;
			}
		}
	}
	dpsData->imageStep = 0;
	dpsData->imageIndex = 0;
	/*
	dpsPrintfInfo("addImg:%lx,%u\n",handle,dpsData->imageCount);
	dpsPrintfInfo("-f-%s\n",pfileName);
	dpsPrintfInfo("-c-%x\n",copies);
	dpsPrintfInfo("-x-%x\n",cropping_x);
	dpsPrintfInfo("-y-%x\n",cropping_y);
	dpsPrintfInfo("-w-%x\n",cropping_w);
	dpsPrintfInfo("-h-%x\n",cropping_h);
	dpsPrintfInfo("-d-%s\n",pDate);
	*/
	
	//dpsLoadConfigStartJob(&csj);
	
	memset(cpi,0,sizeof(DPS_PARA_ConfigPrintInfo));
	cpi->fileID = handle;
	cpi->copies = copies;
	cpi->croppingArea_x = cropping_x;
	cpi->croppingArea_y = cropping_y;
	cpi->croppingArea_w = cropping_w;
	cpi->croppingArea_h = cropping_h;	
	if(copies==1)cpi->copies=0;
	if(pfileName && csj->fileNamePrint == K_DPS_HEX_PARAVAL_FileNamePrint_On) {	
		strcpy(cpi->fileName,pfileName);
	}
	if(pDate && csj->datePrint == K_DPS_HEX_PARAVAL_DatePrint_On){
		strcpy(cpi->date,pDate);
	}
	//dpsPrintf("-copies-%x,%x\n",cpi->copies,copies);
	dpsPrintfInfo("Print:%bx\n",cpi->inValid);
	dpsPrintfInfo("-h-%lx\n",cpi->fileID);
	dpsPrintfInfo("-x-%x\n",cpi->croppingArea_x);
	dpsPrintfInfo("-y-%x\n",cpi->croppingArea_y);
	dpsPrintfInfo("-w-%x\n",cpi->croppingArea_w);
	dpsPrintfInfo("-h-%x\n",cpi->croppingArea_h);
	dpsPrintfInfo("-c-%x\n",cpi->copies);
	dpsPrintfInfo("-f-%s\n",cpi->fileName);
	dpsPrintfInfo("-d-%s\n",cpi->date);

	dpsPrintfInfo("startJob:%lx\n",csj->quality);
	dpsPrintfInfo("--%lx\n",csj->paperSize);
	dpsPrintfInfo("--%lx\n",csj->paperType);
	dpsPrintfInfo("--%lx\n",csj->fileNamePrint);
	dpsPrintfInfo("--%lx\n",csj->datePrint);
	dpsPrintfInfo("--%lx\n",csj->fileNamePrint);
	dpsPrintfInfo("--%lx\n",csj->imageOptimize);
	dpsPrintfInfo("--%lx\n",csj->layout);
	dpsPrintfInfo("--%lx\n",csj->fixedSize);
	dpsPrintfInfo("--%lx\n",csj->cropping);
	
	return dpsData->imageCount;
#else
	handle = handle;
	 copies = copies;
	 cropping_x = cropping_x;
	 cropping_y = cropping_y;
	 cropping_w = cropping_w;
	 cropping_h = cropping_h;	 
	 pfileName = pfileName;
	 pDate = pDate;
	 return 0;
#endif
}
#if 0
UINT8 dps_printMulti()
{
	UINT16 fileStartIdx = dpsGetParam(DPS_PARAM_DpsPrintIndex);
	UINT16 fileEndIdx = dpsGetParam(DPS_PARAM_DpsPrintCount);
	
	UINT32 handle;
	UINT16 i;

	if(fileEndIdx-fileStartIdx>DPS_PARA_ConfigPrintInfoMaxCount)
	{
		fileEndIdx = fileStartIdx+DPS_PARA_ConfigPrintInfoMaxCount;
		dpsAddFlagMulPrint();
	}
	else
	{
		dpsClearFlagMulPrint();
	}
	if(fileEndIdx>dcfFileCountGet())
	{
		fileEndIdx = G_USR_FileCount;
		dpsClearFlagMulPrint();
	}
	sp1kDpsSetParam(DPS_PARAM_DpsPrintIndex, fileEndIdx);
	dpsClearConfigPrintInfo();	
	for(i=fileStartIdx;i<fileEndIdx;i++)
	{
		handle =sidc_makeHandleFromOrd(i);
		handle = M_ByteSwapOfDword(handle);
		Sp1kDpsAddImage(handle, NULL, 0, 0, 0, 0, 0, NULL);		
	}
	return sp1kDpsAction(DPS_ACTION_StartJob, 0);
}

UINT8 dps_doActionPrintMulti(UINT16 fileIdx,UINT16 fileCnt)
{
	sp1kDpsSetParam(DPS_PARAM_DpsPrintIndex,fileIdx);
	sp1kDpsSetParam(DPS_PARAM_DpsPrintCount,fileCnt);
	
	return dps_printMulti();
}
UINT8 dps_doActionPrintAll()
{
	return dps_doActionPrintMulti(0,dcfFileCountGet());
}
  
UINT8 dps_doPausePrintMulti(UINT32 param)
{
	//dpsClearFlagMulPrint();
	dpsActionAbortJob(param);
}
#endif
UINT8 sp1kDpsRemoveImage(UINT8 idx)
{
#if (USBSIDC_OPTION==2)
	sidcPrintfInfo("RemoveImage:%bu,count:%u\n",idx,dpsData->imageCount);
	//dpsClearConfigPrintInfo();
	if(idx>=dpsData->imageCount)
		return FALSE;
	dpsData->imageList[idx].inValid= 1;	
	return TRUE;
#else
	idx = idx;
	return 0;
#endif

}
/**
 * @fn        UINT8 sp1kDpsClearImageList(void)
 * @brief     clear printf list
 * @param     NONE
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsClearImageList()
{
	sidcPrintfFunc("clearList");
	//dpsClearConfigPrintInfo();
	dpsData->imageCount = 0;
	return TRUE;
}
dps_data_t* sp1kDpsGetInfo()
{
	return dpsData;
}

/**
 * @fn        UINT16 sp1kDpsGetServiceInfo(tService pcps)
 * @brief     get printer service info
 * @param     [out] pcps
 * @retval    return = is info new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetServiceInfo(PDPS_PARA_ConfigPrintService pcps)
{
#if (USBSIDC_OPTION==2)

	UINT16 ret=dpsIsNewConfigurePrintService();
	if(pcps)
	{
		//dpsLoadConfigService(pcps);
		if(ret==0)sp1kDpsAction(DPS_ACTION_ConfigurePrintService, 0);
		memcpy(pcps,&(dpsData->printService),sizeof(DPS_PARA_ConfigPrintService));
		dpsClearFlagConfigurePrintService();
		
		#if 0
		dpsPrintfInfo("printServiceInfo:\n");
		dpsPrintfInfo("printServiceAvailable:0x%lx\n",pcps->printServiceAvailable);
		dpsPrintfInfo("dpsVersions:%s\n",pcps->dpsVersions);
		dpsPrintfInfo("vendorName:%s\n",pcps->vendorName);
		dpsPrintfInfo("vendorSpecVer:%s\n",pcps->vendorSpecVer);
		dpsPrintfInfo("productName:%s\n",pcps->productName);
		dpsPrintfInfo("serialNo:%s\n",pcps->serialNo);
		#else
		dpsPrintfInfo("\nprtInfo%x:\n",ret);
		dpsPrintfInfo("-%lx\n",pcps->printServiceAvailable);
		dpsPrintfInfo("-%s\n",pcps->dpsVersions);
		dpsPrintfInfo("-%s\n",pcps->vendorName);
		dpsPrintfInfo("-%s\n",pcps->vendorSpecVer);
		dpsPrintfInfo("-%s\n",pcps->productName);
		dpsPrintfInfo("-%s\n\n",pcps->serialNo);
		#endif
	}
	return ret;
#else
	pcps = pcps;
	return 0;
#endif

}
/**
 * @fn        UINT16 sp1kDpsGetCapability(UINT8 type, PDPS_PARA_Capability_Elem pce)
 * @brief     get capability data
 * @param     [in] type
 * @param     [out] pce
 * @retval    return = is info is new or not:0 is old, 
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetCapability(UINT8 type, PDPS_PARA_Capability_Elem pce)
{
#if (USBSIDC_OPTION==2)
	static UINT32 prepapersize = 0;
	static UINT8 pretype = 0;
	UINT16 ret=dpsIsNewCapability(type);
	pce->paperSize = appDpsPapersizeGet();
	if(pce)
	{
		dpsPrintfInfo("papersize:%lx\ngetcapability type:0x%bx\nret:%u\n",pce->paperSize,type,ret);
		if(!dps_checkIsAction_GetCapability(type)){
			return FALSE;
		}
		//if papersize had changed,getcapability again
		if((prepapersize != pce->paperSize) || (pretype != type)){
			prepapersize = pce->paperSize;
			pretype = type;
			ret = 0;			
		}
		if(!ret){
			dpsActionGetCapability(type ,pce->paperSize);
		}
		//dpsLoadGetCapability_Item(type, (UINT8 *)pce);	
		memcpy(pce,dpsData->capability+type - DPS_ACTION_GetCapability,sizeof(DPS_PARA_Capability_Elem));
		{
			UINT8 i,_count;
			dpsPrintfInfo("Cap:0x%bx\n",type);
			dpsPrintfInfo("-Count:0x%x\n",pce->count);
			_count=pce->count;
			if(pce->count > 24)_count=24;
			for(i=0;i<_count;i++){
				dpsPrintfInfo("-0x%lx\n",pce->values[i]);
			}
			//printf("\n\n******************************E\n");
		}

	}
	return ret;
#else
	type = type;
	pce = pce;
	return 0;
#endif

}
DPS_PARA_JobStatus* sp1kDpsJobMsg()
{	
	DPS_PARA_JobStatus* pjs = &(dpsData->jobStatus);
	dpsPrintfInfo("dpsJobMsg:%u/%u-%u",dpsData->imageIndex,dpsData->imageCount,dpsData->imageMax);	  
	if(dpsData->imageCount && dpsData->imageMax){
		UINT8 chrs[20];
		UINT32 idx,cnt,printed;//,cnt; 
		UINT32 layout = dpsData->startJob.layout;
		UINT16 index;
		strcpy(chrs,pjs->imagesPrinted);
		printed = strtoul(chrs,0,0);		
		strcpy(chrs,pjs->progress);
		chrs[3]=0;
		idx = strtoul(chrs,0,0);		
		cnt = strtoul(chrs+4,0,0);
		dpsData->imageIndex = printed + dpsData->imageStep*dpsData->imageMax;
		if(dpsData->imageIndex>dpsData->imageCount)
			dpsData->imageIndex=dpsData->imageCount;
		
		if(layout<K_DPS_HEX_PARAVAL_Layout_2UpLayout || layout>K_DPS_HEX_PARAVAL_Layout_IndexPrint){
			index = dpsData->imageIndex+(dpsData->imageIndex<dpsData->imageCount)?1:0;
			sprintf(pjs->progress,"%04u/%04u",index,dpsData->imageCount);
		}
	}
	return pjs;
}
DPS_PARA_DeviceStatus* sp1kDpsDeviceMsg()
{
	DPS_PARA_DeviceStatus* pds = &(dpsData->deviceStatus);
	dpsPrintfInfo("dpsJobMsg:%u/%u-%u\n",dpsData->imageIndex,dpsData->imageCount,dpsData->imageMax);

	if(pds->newJobOK==K_DPS_HEX_PARAVAL_NewJobOk_True/*0x76010000*/){
		if((dpsData->imageMax!=0)&&
			(dpsData->imageCount>dpsData->imageMax)&&
			(dpsData->imageStep<dpsData->imageCount/dpsData->imageMax)&&
			(dpsData->action != DPS_ACTION_AbortJob)
		  ){				
			pds->newJobOK=K_DPS_HEX_PARAVAL_NewJobOk_False;
			dpsData->imageStep++;
			dpsActionStartJob();
		}
	}	
	return pds;
}

/**
 * @fn        UINT16 sp1kDpsGetDeviceStatus(PDPS_PARA_DeviceStatus pds)
 * @brief     ger device status data
 * @param     [out] pds
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetDeviceStatus(PDPS_PARA_DeviceStatus pds)
{
#if (USBSIDC_OPTION==2)

	UINT16 ret =dpsIsNewDeviceStatus();
	//dpsPrintfInfo("status:%lx\n",dpsDataStatus);
	if(pds)
	{
		if(!ret)sp1kDpsAction(DPS_ACTION_GetDeviceStatus, 0);
		//else sp1kDpsDeviceMsg();
		
		//dpsLoadDeviceStatus((UINT8 *)pds);
		memcpy(pds,&(dpsData->deviceStatus),sizeof(DPS_PARA_DeviceStatus));
		dpsClearFlagDeviceStatus();
	#if 0
		dpsPrintfInfo("DeviceStatus:%x\n",ret);
		dpsPrintfInfo("dpsPrintServiceStatus:%s\n",pds->printServiceStatus);
		dpsPrintfInfo("jobEndReason:%s\n",pds->jobEndReason);
		dpsPrintfInfo("errorStatus:%s\n",pds->errorStatus);
		dpsPrintfInfo("errorReason:%s\n",pds->errorReason);
		dpsPrintfInfo("disconnectEnable:%s\n",pds->disconnectEnable);
		dpsPrintfInfo("capabilityChanged:%s\n",pds->capabilityChanged);
		dpsPrintfInfo("newJobOK:%s\n",pds->newJobOK);
	#else
		dpsPrintfInfo("----Dev:%x\n",ret);
		dpsPrintfInfo("-%lx\n",pds->printServiceStatus);
		dpsPrintfInfo("-%lx\n",pds->jobEndReason);
		dpsPrintfInfo("-%lx\n",pds->errorStatus);
		dpsPrintfInfo("-%lx\n",pds->errorReason);
		dpsPrintfInfo("-%lx\n",pds->disconnectEnable);
		dpsPrintfInfo("-%lx\n",pds->capabilityChanged);
		dpsPrintfInfo("-%lx\n\n",pds->newJobOK);
	#endif
	}
	return ret;
#else
	pds = pds;
	return 0;
#endif

}
/**
 * @fn        UINT16 sp1kDpsGetJobStatus(PDPS_PARA_JobStatus pjs)
 * @brief     get job status data
 * @param     [out] pjs
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetJobStatus(PDPS_PARA_JobStatus pjs)
{
#if (USBSIDC_OPTION==2)

	UINT16 ret =dpsIsNewJobStatus();
	if(pjs)		
	{
		//dpsLoadJobStatus((UINT8 *)pjs);
		if(!ret)sp1kDpsAction(DPS_ACTION_GetJobStatus, 0);
		//else sp1kDpsJobMsg();
		
		memcpy(pjs,&(dpsData->jobStatus),sizeof(DPS_PARA_JobStatus));
		dpsClearFlagJobStatus();
	#if 0
		dpsPrintfInfo("DeStatus:%x\n",ret);
		dpsPrintfInfo("prtPID:%s\n",pjs->prtPID);
		dpsPrintfInfo("filePath:%s\n",pjs->filePath);
		dpsPrintfInfo("copyID:%s\n",pjs->copyID);
		dpsPrintfInfo("progress:%s\n",pjs->progress);
		dpsPrintfInfo("imagesPrinted:%s\n",pjs->imagesPrinted);
	#else
		dpsPrintfInfo("----Job:%x\n",ret);
		dpsPrintfInfo("-%s\n",pjs->prtPID);
		dpsPrintfInfo("-%s\n",pjs->filePath);
		dpsPrintfInfo("-%s\n",pjs->copyID);
		dpsPrintfInfo("-%s\n",pjs->progress);		
		dpsPrintfInfo("-%s\n\n",pjs->imagesPrinted);		
	#endif

	}
	return ret;
#else
	pjs = pjs;
	return 0;
#endif

}

/**
 * @fn        UINT8 sp1kDpsGetPrintCapability(UINT8 type, UINT32 *pValue)
 * @brief     get print capability data
 * @param     [in] type
 * @param     [out] pValue
 * @retval    return =  TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsGetPrintCapability(UINT8 type, UINT32 *pValue)
{
	//dpsLoadConfigStartJobItem(*pValue,type);
	*pValue = ((UINT32 xdata*)&(dpsData->startJob))[type-DPS_ACTION_GetCapability];
	dpsPrintfInfo("PrintCap-%lx\n",*pValue);

	return  TRUE;
}
/**
 * @fn        UINT8 sp1kDpsSetPrintCapability(UINT8 type, type,UINT32 value)
 * @brief     set print capability
 * @param     [in] type
 * @param     [in] value
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsSetPrintCapability(UINT8 type,UINT32 value)
{
#if (USBSIDC_OPTION==2)

	DPS_PARA_Capability_Elem ce;
	UINT8 i,_count;	
	UINT32 xdata*csj = (UINT32 xdata*)&(dpsData->startJob);
	UINT8 rst = 0;
	//dpsLoadGetCapability_Item(type, &ce);
	sp1kDpsGetCapability(type,&ce);
	_count = ce.count;
	if(_count > DPS_PARA_Capability_Elem_MaxCount)
		_count=24;

	dpsPrintfInfo("value:0x%lx\n",value);
	dpsPrintfInfo("count:0x%x\n",ce.count);
	//for(i=0;i<_count;i++){
	//	sidcPrintf("--0x%lx\n",ce.values[i]);
	//}
	for(i=0;i<_count;i++)
	{
		dpsPrintfInfo("--0x%lx\n",ce.values[i]);
		if(ce.values[i]==value)
			rst = 1;
	}
	if(!rst)
	{
		sidcPrintfErr("not surpport!\n");
		/*After setting papertype and layout, once again set the papersize, 
		does not support has been set of papertype and layout, that,restore 
		the papertype and layout to default value*/
		if(type == DPS_ACTION_GetCapability_Layouts){
			value = K_DPS_HEX_PARAVAL_Layout_Default;
		}else if(type == DPS_ACTION_GetCapability_PaperTypes){
			value = K_DPS_HEX_PARAVAL_PaperType_Default;
		}else{
			value =0;
		}
		//value &= 0xff000000;
	}

	//fixedSizes or Layouts
	if(type == DPS_ACTION_GetCapability_FixedSizes)
	{
		//UINT32 tmp1=0;
		//dpsLoadConfigStartJobItem(tmp1,DPS_ACTION_GetCapability_Layouts);
		if( csj[DPS_ACTION_GetCapability_Layouts-DPS_ACTION_GetCapability])
			value =0;
	}
	else if(type == DPS_ACTION_GetCapability_Layouts)
	{
		//UINT32 tmp1=0;
		//dpsSaveConfigStartJobItem(tmp1, DPS_ACTION_GetCapability_FixedSizes);
		csj[DPS_ACTION_GetCapability_FixedSizes-DPS_ACTION_GetCapability] = 0;
	}
	//if(0 == value & 0x00ff0000)value = 0;
	csj[type-DPS_ACTION_GetCapability] = value;
	//dpsSaveConfigStartJobItem(value,type);
	return TRUE;
#else
	type = type;
	value = value;
	return 0;
#endif

}
/**
 * @fn        UINT8 sp1kDpsGetPrintConfigInfo(UINT8 index, PDPS_PARA_ConfigPrintInfo pcpi)
 * @brief     get config info in print list 
 * @param     [in] index
 * @param     [out] pcpi
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsGetPrintConfigInfo(UINT8 index, PDPS_PARA_ConfigPrintInfo pcpi)
{
	//dpsLoadConfigPrintInfoItem((UINT8 *)pcpi, index);
	memcpy(pcpi,&(dpsData->imageList+index),sizeof(DPS_PARA_ConfigPrintInfo));
	return TRUE;
}
/**
 * @fn        UINT32 sp1kDpsSetParam(UINT8 idx,UINT32 val)
 * @brief     set dps param
 * @param     [in] idx
 * @param     [in] val :0xffffffff return current param
 * @retval    return = current param value
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sp1kDpsSetParam(UINT8 idx, UINT32 val)
{
	UINT32 read = 0xffffffff;	
#define __DpsSetParam(item,val)if(val==read)val=item;else item=val
	switch(idx)
	{
		case DPS_PARAM_DpsStatus:
			__DpsSetParam(dpsData->dataStatus,val);			
		break;
		case DPS_PARAM_AddFileCountMax:
			__DpsSetParam(dpsData->imageListMax,val);			
		break;
		case DPS_PARAM_PrintFileCountMax:
			__DpsSetParam(dpsData->imageMax,val);			
		break;
	}
	return read;
	/*
	if(idx>=DPS_PARAM_MAX)
		return 0;
	if(val!=0xffffffff)
		dpsData->paramList[idx] = val;		
	return dpsData->paramList[idx];
	*/
}
UINT8 sp1kDpsGetException()
{
	return sidcData->exception;
}

void dpsCmd(	UINT8 *param)
{
#if 1
	param = param;
	printf("dps(%lu,%lu\n)",(UINT32)sizeof(sidc_data_t),(UINT32)sizeof(dps_data_t));
#else
	UINT8 *ptr;

	UINT8 fileName[] = "SUNP0001";
	UINT8 fileDate[]="2008.05.05";
	UINT8 pData[256];
	PDPS_PARA_ConfigPrintService pPrtService = (PDPS_PARA_ConfigPrintService)pData;
	PDPS_PARA_ConfigPrintInfo pPrtInfo = (PDPS_PARA_ConfigPrintInfo)pData;
	PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	PDPS_PARA_JobStatus pJobStatus = (PDPS_PARA_JobStatus)pData;
	PDPS_PARA_DeviceStatus pDeviceStatus = (PDPS_PARA_DeviceStatus)pData;

	sidcPrintfFunc("dps cmd: %s\n", param);
	sidcPrintfInfo("sidc:%lx,dps:%lx\n",(UINT32)sizeof(sidc_data_t),(UINT32)sizeof(dps_data_t));
	ptr = param;

	
#if 1//DPS_CMD_ENABLE	
	
	#if 0
	
	
	if ( strcmp(ptr, "00") == 0 ) 
	{
		sp1kDpsGetServiceInfo(pPrtService) ;

	}
	if ( strcmp(ptr, "17") == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_ContinueJob,0);
		//sp5kDpsPrtAction(SP5K_ACTION_CONTINUE);
		
	}
	if ( strcmp(ptr, "19") == 0 ) {
		sp1kDpsAction(DPS_ACTION_AbortJob,K_DPS_HEX_PARAVAL_AbortStyle_Immediately);
	}
	
	if ( strncmp(ptr, "f",1) == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_GetCapability + ptr[1] -'0', strtoul( &(ptr[3]), 0, 16 ));
	}

	if ( strcmp(ptr, "t0") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59000000);

		Sp1kDpsAddImage(0x00640001, NULL, 0, 0, 0, 0, 0, NULL);

		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t9") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 1, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}

	if ( strcmp(ptr, "t8") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59000000);

		Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 1, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t7") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57010000);
		//sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		//Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 1, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t6") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57020000);
		//sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		//Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 2, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t5") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57ff0000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		//Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 2, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t4") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57fe0000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 2, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	if ( strcmp(ptr, "t3") == 0 )
	{
		
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		Sp1kDpsAddImage(0x00640001, fileName, 2, 10, 30, 400, 355, fileDate);
		Sp1kDpsAddImage(0x00640002, fileName, 2, 110, 30, 300, 355, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
#endif
	if ( strncmp(ptr, "setp",4) == 0 )
	{
		UINT8 idx;
		UINT32 val;
		ptr+=5;
		idx=(UINT8)atoi(ptr);
		ptr+=2;
		val=(UINT32)atoi(ptr);
		sp1kDpsSetParam(idx, val);
		//printf("dpsP(%bx,%ld)\n",idx,sp1kDpsGetParam(idx));	
	}
	#if 0
	else if (strncmp(ptr,"cs",2)==0)
	{		
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Qualities,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_PaperSizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_PaperTypes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FileTypes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_DatePrints,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FileNamePrints,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_ImageOptimizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Layouts,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FixedSizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Croppings,  pCapabilityElem);
	}
	else if ( strncmp(ptr, "c",1) == 0 ) 
	{
		
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Qualities + ptr[1] -'0',  pCapabilityElem);
	
		//sp1kDpsAction(DPS_ACTION_GetCapability + ptr[1] -'0', strtoul( &(ptr[3]), 0, 16 ));
	}

	else if ( strcmp(ptr, "gp") == 0 )
	{
		sp1kDpsGetServiceInfo(pPrtService);
	}
	else if ( strcmp(ptr, "gj") == 0 )
	{
		sp1kDpsGetJobStatus( pJobStatus);
	}
	else if ( strcmp(ptr, "gd") == 0 )
	{
		sp1kDpsGetDeviceStatus(pDeviceStatus);
	}
	else if ( strcmp(ptr, "gc") == 0 ) 
	{
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Qualities,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_PaperSizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_PaperTypes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FileTypes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_DatePrints, pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FileNamePrints, pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_ImageOptimizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Layouts,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_FixedSizes,  pCapabilityElem);
		sp1kDpsGetCapability(DPS_ACTION_GetCapability_Croppings,  pCapabilityElem);
	
	}
	else if ( strcmp(ptr, "g0") == 0 ) 
	{
		UINT32 value;
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_Qualities,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_PaperSizes,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_PaperTypes,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_FileTypes,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_DatePrints, &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_Layouts,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_FixedSizes,  &value);
		sp1kDpsGetPrintCapability(DPS_ACTION_GetCapability_Croppings,  &value);

	}
///////////////
	else if ( strncmp(ptr, "s",1) == 0 )
	{
		sp1kDpsSetPrintCapability((DPS_ACTION_GetCapability |((ptr[1]-'0')& 0x0f)), strtoul( (char*)(ptr +3), 0, 16 ));
	}
	#endif
	else if ( strncmp(ptr, "ad",2) == 0 )
	{
		sp1kDpsAddImage(strtoul( (char*)(ptr +3), 0, 16 ), fileName, 2, 21, 23, 502, 499, fileDate);
	}
	else if ( strcmp(ptr, "cl") == 0 )
	{
		sp1kDpsClearImageList();
	}
	else if ( strcmp(ptr, "p0") == 0 )
	{
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	else if ( strncmp(ptr, "mx",2) == 0 )
	{
		sp1kDpsSetParam(DPS_PARAM_PrintFileCountMax,atol( (char*)(ptr +3)));
		printf("imageMax:%u\n",dpsData->imageMax);
	}
	#if 0
	else if ( strcmp(ptr, "p1") == 0 ) 
	{
		sp1kDpsAddImage(0x00644001, fileName, 1, 21, 23, 502, 499, fileDate);
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	else if ( strcmp(ptr, "p2") == 0 ) 
	{
		sp1kDpsAddImage(0x00644001, fileName, 1, 0, 0, 0, 0, fileDate);
		sp1kDpsAddImage(0x00644002, fileName, 2, 21, 23, 502, 499, fileDate);
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
////////////////
	else if ( strcmp(ptr, "pa") == 0 ) 
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0);

		sp1kDpsAddImage(0x00644001, NULL, 0, 0, 0, 0, 0, NULL);

		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	else if ( strcmp(ptr, "pb") == 0 )
	{
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Qualities, 0x50010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperSizes, 0x51010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_PaperTypes, 0x52000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileTypes, 0x53010000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_DatePrints, 0x54020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FileNamePrints, 0x55020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_ImageOptimizes, 0x56020000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Layouts, 0x57000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_FixedSizes, 0x58000000);
		sp1kDpsSetPrintCapability(DPS_ACTION_GetCapability_Croppings, 0x59020000);

		sp1kDpsAddImage(0x00644001, fileName, 2, 10, 30, 400, 355, fileDate);
		sp1kDpsAddImage(0x00644002, NULL, 1, 110, 30, 300, 353, fileDate);
		
		sp1kDpsAction(DPS_ACTION_StartJob, 0);
	}
	
	else if ( strcmp(ptr, "j0") == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_AbortJob,K_DPS_HEX_PARAVAL_AbortStyle_Immediately);
	}
	else if ( strcmp(ptr, "j1") == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_ContinueJob,0);
		//sp5kDpsPrtAction(SP5K_ACTION_CONTINUE);
		
	}
	else if ( strcmp(ptr, "j2") == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_AbortJob,K_DPS_HEX_PARAVAL_AbortStyle_AfterCurrPage);
	}
	else if ( strcmp(ptr, "cps") == 0 ) 
	{
		sp1kDpsAction(DPS_ACTION_ConfigurePrintService,0);
	}
	#endif
#else
	param = param;
	
#endif
//////////////
#endif
}

#endif


