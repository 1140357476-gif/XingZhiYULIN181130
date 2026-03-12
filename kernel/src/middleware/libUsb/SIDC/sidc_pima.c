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
#include "sidc_func.h" 
#include "utility.h"
#include "dbg_def.h"
#include "reg_usb.h"
#include "os_msg.h"
#if USBSIDC_OPTION
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
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//pimaGetDeviceInfo
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaGetDeviceInfo(void)
 * @brief     pimaGetDeviceInfo
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetDeviceInfo() USING_1
{
	UINT32 size ;
	sidcPrintfPima("GetDev\n");
	size = sidcIntrRes(G_ucStorData+12,SIDC_RES_Device);
	sidcIntrDataIn(size);
	return sidcIntrResponse(K_PIMARES_OK,0,NULL);
}

//-----------------------------------------------------------------------------
//pimaOpenSession
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaOpenSession(void)
 * @brief     pimaOpenSession
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaOpenSession() USING_1
{
	sidcPrintfPima("OpenSes\n");
	dpsStatus = DPS_STATUS_Idle;
	sidcFlag = SIDC_Null;
	dpsFlag = DPS_Null;
	/*sidcData->bufSize = 
		sizeof(sidc_data_t) - 
		(sidcData->bufStart - G_FPGAMemPool);
	memset(sidcData->bufStart,0,sidcData->bufSize);	*/
	return sidcIntrResponse(K_PIMARES_OK,0,NULL);
}

//-----------------------------------------------------------------------------
//pimaCloseSession
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaCloseSession(void)
 * @brief     pimaCloseSession
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaCloseSession() USING_1
{
	sidcPrintfPima("CloseSes\n");
	dpsStatus = DPS_STATUS_Idle;
	sidcFlag = SIDC_Null;
	dpsFlag = DPS_Null;
	XBYTE[REG_Usb_EpAckIntEn] |=0x3f;//open Ack intr
	return sidcIntrResponse(K_PIMARES_OK,0,NULL);
}

//-----------------------------------------------------------------------------
//pimaGetStorageIDs
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaGetStorageIDs(void)
 * @brief     pimaGetStorageIDs
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetStorageIDs() USING_1
{
	UINT32 size = sizeof(sidc_storage_t);
	sidc_storage_t* pStorageID = (sidc_storage_t*) (G_ucStorData + 12);
	sidcPrintfPima("GetStoIds\n");
	pStorageID->storageIDsSize = M_ByteSwapOfDword(1ul);
	pStorageID->storageIDs[0] = K_PIMADATA_IDStorage;

	sidcIntrDataIn(size);
       return sidcIntrResponse(K_PIMARES_OK,0,0);      
}

//-----------------------------------------------------------------------------
//pimaGetStorageInfo
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaGetStorageInfo(void)
 * @brief     pimaGetStorageInfo
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetStorageInfo() USING_1
{
	//sidc_storage_info_t* pStorageInfo = (G_ucStorData + 12);
	UINT32 size = sidcIntrRes( G_ucStorData + 12,SIDC_RES_Storage);
	sidcPrintfPima("GetStoInfo\n");
	sidcIntrDataIn(size);
	return sidcIntrResponse(K_PIMARES_OK,0,0);
}

//-----------------------------------------------------------------------------
//pimaGetNumObjects
//-----------------------------------------------------------------------------
/**
 * @fn        void pimaGetNumObjects(void)
 * @brief     pimaGetNumObjects
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetNumObjects() USING_1
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT16 objFormatCode =READ16(pCommand->ulParameter[1],0);
	UINT32 cnt = objFormatCode != K_PIMAOBJ_Script?0/*dcfFileCountGet() G_USR_FileCount*/:1;
	sidcPrintfPima("GetNum:%lu\n",cnt);
	
	cnt = M_ByteSwapOfDword(cnt);
	//sidcIntrDataIn(len);
	return sidcIntrResponse(K_PIMARES_OK,1,&cnt);

}


/**
 * @fn        void pimaGetObjectHandles(void)
 * @brief     pimaGetObjectHandles
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetObjectHandles()
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	//UINT32 storageId =M_ByteSwapOfDword( pCommand->ulParameter[0]);
	UINT16 objFormatCode = READ16(pCommand->ulParameter[1],0);
	//UINT32 objHandle = M_ByteSwapOfDword(pCommand->ulParameter[2]);

	sidcPrintfPima("getObjHnds-%x\n",objFormatCode);

	dbgWdtFeed(-1);	
	if(objFormatCode == K_PIMAOBJ_Script)//sript
	{
		UINT32 *pHandle = (UINT32*)(G_ucStorData);
		pHandle[3] = M_ByteSwapOfDword(1);
		pHandle[4] = M_ByteSwapOfDword(DPS_DDISCVRY_HND);
		//sidcContainerBuild(pHandle, 8);
		//sidcBulkIn((UINT16)pHandle, 8);
		sidcDataIn((UINT16)G_ucStorData, 8);
	}
	else
	{
		UINT32 addr = K_SDRAM_FreeAddr;
		UINT32 count = 0;//sidcMakeHandles( addr+6,0);	//mantis #42397
		sidcDataIn(addr,count*4+4);
	/*
		UINT32 *pHandle = (UINT32*)(G_ucStorData);
		pHandle[3] = M_ByteSwapOfDword(1);
		pHandle[4] = M_ByteSwapOfDword(0x00644001);
		//sidcContainerBuild(pHandle, 8);
		sidcDataIn((UINT16)G_ucStorData, 8);
		//sidcBulkIn((UINT16)pHandle, 8);
		//UINT32 fileCnt=sidcMakeHandles();
		//sidcDataIn(SIDC_ADDR_TMP, fileCnt*4);
		*/
		osMsgPost(sp1k_MSG_DPS_NOT_FIND_PRINTER);
	}	

	return sidcResponse(K_PIMARES_OK, 0, NULL);
}

/**
 * @fn        void pimaGetObjectInfo(void)
 * @brief     pimaGetObjectInfo
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetObjectInfo()//UINT32* pSize) 
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle = M_ByteSwapOfDword(pCommand->ulParameter[0]);
	sidc_object_info_t* pObjectInfo 
		= (sidc_object_info_t*)(G_ucStorData + 12);
	//sidc_node_t node;
	UINT8 fileName[13];
	UINT16 fileType;
	UINT32 fileSize;
	UINT16 len = (UINT16)sizeof(sidc_object_info_t);//sizeof(sidc_object_info_t);
	UINT16 i;

	UINT8 ret=0;

	sidcPrintfPima("getObjInfo-%lx\n",handle);
	dbgWdtFeed(-1);	
	//memset(G_ucStorData,0,len+12);
	if (sidcIsScriptFile(handle))
	{	
#if 1
		dps_script_t *script = NULL;
		if(handle == DPS_DREQUEST_HND){
			script = &sidcData->dRequest;
			fileSize = script->size;
		}else if(handle == DPS_DRSPONSE_HND){
			script = &sidcData->dRsponse;
			fileSize = script->size;
		}else if(handle == DPS_DDISCVRY_HND){
			fileSize = 0;
		}else{
		}
		dpsGetScriptInfo(&handle, fileName);
		sidcPrintfImport("size:%lu\n",fileSize);
		fileType= K_PIMAOBJ_Script;
		memset(G_ucStorData,0,len+12);		
#else
		dpsGetScriptInfo(&handle, fileName);
		fileType= K_PIMAOBJ_Script;
		fileSize=(handle == DPS_DDISCVRY_HND)?0:(sidcData->inSize);

		memset(G_ucStorData,0,len+12);
#endif
	}
	else	
	{
		sidc_file_info_t info;
		//sidcGetFileInfoStorage( handle,&info);
		if(!sidcGetFileInfo(handle,K_SDRAM_FreeAddr,&info)){
			return sidcResponse(K_PIMARES_InvalidObjectHandle, 0, NULL);
		}
		strcpy(fileName,info.fileName);
		fileSize=info.fileSize;
		fileType=info.fileType;
		//handle=handle;

		memset(G_ucStorData,0,len+12);
		if(sidcIsJpgFile(handle))
		{
			pObjectInfo->thumbFormat		= 0x0138;	//exif/jpg
			pObjectInfo->thumbCompressedSize	= M_ByteSwapOfDword(info.thembSize);
			pObjectInfo->thumbPixWidth		= M_ByteSwapOfDword(info.thembWidth);
			pObjectInfo->thumbPixHeight		= M_ByteSwapOfDword(info.thembHeight);
			pObjectInfo->imagePixWidth		= M_ByteSwapOfDword(info.width);
			pObjectInfo->imagePixHeight		= M_ByteSwapOfDword(info.height);
			pObjectInfo->imageBitDepth		= (UINT32)0x00;//0x18000000;//0x00;
		}
	}
	//sidcPrintfImport("%s\n",fileName);
	for (i = 0;i < 13 * 2;i += 2)
	{
		pObjectInfo->fileNameBuf[1 + i] = fileName[i/2];
		pObjectInfo->fileNameBuf[1 + i + 1] = 0x00;
	}

	pObjectInfo->objectFormat = fileType;
	pObjectInfo->objectCompressedSize = M_ByteSwapOfDword(fileSize);
	
	pObjectInfo->fileNameBuf[0] = 0x0d;//i/2;//((i/2) + 1) + 3 + 1;//12 * 2 + 2;
	pObjectInfo->storageID = 0xFFFFFFFF;//K_PIMADATA_IDStorage;
	pObjectInfo->protectionStatus =	0x0100;//0x0100;//Read Only//0x0000 R&W//

	//pObjectInfo->parentObject = 0;
	//pObjectInfo->sequenceNumber=0;
	sidcDataIn((UINT16)G_ucStorData, len);
	//printf("end info\n");

	return sidcResponse(K_PIMARES_OK, 0, NULL);
}

/**
 * @fn        void pimaGetObject(void)
 * @brief     pimaGetObject
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetObject()//UINT32 *pSize)
{
	UINT32 size=0;
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle = M_ByteSwapOfDword(pCommand->ulParameter[0]);
	
	sidcPrintfPima("GetObj-%lx\n",handle);
	dbgWdtFeed(-1);	
	if(sidcIsScriptFile(handle))
	{
#if 1
		dps_script_t *script = NULL;
		UINT32 addr;
		if(handle == DPS_DREQUEST_HND){
			script = &(sidcData->dRequest);
		}else if(handle == DPS_DRSPONSE_HND){
			script = &(sidcData->dRsponse);
			if(dpsStatus == DPS_STATUS_Response){
				sidcPrintfImport("response compele\n");
				dpsStatus = DPS_STATUS_Idle;
			}
		}else{
		}
		addr = script->addr?script->addr:(UINT16)script->container;
		sidcDataIn(addr, script->size);	
#else
		//sidc_node_t node;
		//dpsLoadInFileNode((&node));
		UINT32 addr = sidcData->inAddr?sidcData->inAddr:((UINT16)(sidcData->inContainer));
		sidcDataIn(addr, sidcData->inSize);	
		if(dpsStatus == DPS_STATUS_Response 
			&& handle == DPS_DRSPONSE_HND){
			//sidcPrintf("-0-dps status idle\n");
			dpsStatus = DPS_STATUS_Idle;
		}
#endif
	}
	else 
	{
		//UINT8 fd;
		UINT8 type;
		UINT8 path[33];
		UINT32 size;
		//UINT32 fileSize;
		//UINT32 maxSize=512UL*200ul;//0x100000;
		//UINT32 i;
		UINT32 addr =  K_SDRAM_FreeAddr;
		UINT8 dev=DOS_CurrDriverGet();
		type=sp1kSidcParsingHandle(handle,path);

		dramFileLoad(path, addr+6, &size);
		sidcDataIn(addr,size);
	/*
		fd = DOS_FileOpen(path, 'r');
		DOS_CurrDriverSet(dev);
		if(fd==0) 
		{
			sidcDataIn(0, 0);
			return;
		}
		DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&fileSize);

		for(i=0;i<fileSize;i+=maxSize)
		{
			size=fileSize-i;
			size=(size>maxSize)?maxSize:size;
			size=DOS_FileRead(fd, addr<<1, size, i);
			//printf("s=%lx\n",size);
			if(size==0)
				break;
			sidcDram2Usb(addr,size);
			
		}
		DOS_FileClose( fd);
	*/

	}
	//printf("object end\n");

	return sidcResponse(K_PIMARES_OK, 0, NULL);
}
/**
 * @fn        void pimaGetThumb(void)
 * @brief     pimaGetThumb
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8  pimaGetThumb()//UINT32* pSize)
{
	UINT32 addr;
	UINT32 size=0;
	sidc_file_info_t info;
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle=M_ByteSwapOfDword(pCommand->ulParameter[0]);
	//UINT32 dramAddr;
	sidcPrintfPima("getThumb-%lx",handle);
	dbgWdtFeed(-1);	
	sidcGetFileInfo(handle, K_SDRAM_FreeAddr, &info);
	addr=info.thembAddr;
	size=info.thembSize;
	
	if((UINT8)addr & 0x01)//odd
	{
		HAL_GprmDramDma(addr, addr-1, size, 1);
	}

	sidcDataIn(addr/2 - 6, size);
	return sidcResponse(K_PIMARES_OK, 0, NULL);
}
#if 1
/**
 * @fn        void pimaSendObjectInfo(void)
 * @brief     pimaSendObjectInfo
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaSendObjectInfo()
{
	UINT32 params[] = {0xFFFFFFFF,0x00000000,0};
	UINT8 i;	
	dps_script_t *script = &sidcData->hData;
	sidc_container_t* pContainer = (sidc_container_t*) script->container;
	sidc_object_info_t* pObjectInfo  = (sidc_object_info_t*)script->object;	
	UINT32 handle=0;
	UINT8 objName[13];
		
	if(pObjectInfo->objectFormat == K_PIMAOBJ_Script){
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
			osMsgPost(SP1K_MSG_DPS_NEW_HDISCVRY);
		}else{
			
		}
		intrPutUint32(handle);
		intrPutChar('\n');
		if(handle){
			script->handle = handle;
			script->size = M_ByteSwapOfDword(pObjectInfo->objectCompressedSize);
			dpsFlag = DPS_MainLoop;
			//script->addr = dst;
		}
	}
	//sidcPrintfPima("sndObjInfo:%lx,%s\n",handle,objName);
	return sidcIntrResponse(K_PIMARES_OK,3,params);
}

/**
 * @fn        void pimaSendObject(void)
 * @brief     pimaSendObject
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaSendObject()
{
	dps_script_t *script = &sidcData->hData;
	UINT32 handle=script->handle;	
	UINT32 size = script->size;
	intrPutUint32(handle);
	intrPutChar('-');
	intrPutUint32(size);
	intrPutChar('\n');
	if(handle == DPS_HRSPONSE_HND){
		memcpy(&sidcData->hRsponse,script,sizeof(dps_script_t));
	}else if(handle == DPS_HREQUEST_HND){
		memcpy(&sidcData->hRequest,script,sizeof(dps_script_t));
	}else{
	}		
	//sidcFlag = DPS_MainLoop;
	//memcpy(&sidcData->hDataTmp,&sidcData->hData,sizeof(dps_script_t));
	//memcpy((void xdata*)script->addr,&sidcData->hData,sizeof(dps_script_t));
	return sidcIntrResponse(K_PIMARES_OK,0,NULL);
	
}


#else
/**
 * @fn        void pimaSendObjectInfo(void)
 * @brief     pimaSendObjectInfo
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaSendObjectInfo()
{
	UINT32 params[] = {0xFFFFFFFF,0x00000000,0};
	//sidc_container_t* pContainer = (sidc_container_t*) sidcData->outContainer;
	//UINT32 size = M_ByteSwapOfDword(pContainer->ulContainerLength);
	//sidcPrintfPima("sndObjInfo:%lx\n");
	dpsCheckObjectInfo();		
	return sidcIntrResponse(K_PIMARES_OK,3,params);
}

/**
 * @fn        void pimaSendObject(void)
 * @brief     pimaSendObject
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaSendObject()
{
	sidc_container_t* pContainer = (sidc_container_t*) sidcData->outContainer;
	UINT32 size = M_ByteSwapOfDword(pContainer->ulContainerLength);
	sidcPrintfPima("sndObj\n");	
	if(dpsCheckObject()){
		UINT8 rst = sidcResponse(K_PIMARES_OK,0,NULL);
		dpsCommand();
		return rst;
	}else{
		return sidcResponse(K_PIMARES_OK,0,NULL);
	}
}
#endif
#if 1
/**
 * @fn        void pimaGetPartialObject(void)
 * @brief     pimaGetPartialObject
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetPartialObject()//UINT32 *pSize)
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle=M_ByteSwapOfDword(pCommand->ulParameter[0]);
	UINT32 offset=M_ByteSwapOfDword( pCommand->ulParameter[1]);
	UINT32 maxBytes=M_ByteSwapOfDword(pCommand->ulParameter[2]);

	UINT8 dev=DOS_CurrDriverGet();
	UINT32 addr =  K_SDRAM_FreeAddr+6;
	UINT8 path[33];
	UINT8 type = sp1kSidcParsingHandle(handle,path);	
	UINT8 fd = DOS_FileOpen(path, 'r');
	UINT16 rst = K_PIMARES_OK;	
	UINT32 clusterSize = G_DOS_ClusterSize;
	
	sidcPrintfPima("getPartOjb-%lx,%lx,%lx,%lx\n",handle,offset,maxBytes,clusterSize);
	if (fd)
	{
		UINT32 d0 = offset%clusterSize;
		UINT32 dd = (offset+maxBytes)%clusterSize;
		UINT32 d1 = clusterSize - dd;
		UINT32 loadSize = d0+maxBytes+d1;
		UINT32 loadOffset = offset - d0;

		UINT32 inAddr = addr+d0/2-6;
		UINT32 inSize = maxBytes;

		UINT32 fileSize=0;
		UINT32 readSize=0;
		DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&fileSize);		

		if(loadOffset>=fileSize){
			rst = K_PIMARES_ParameterNotSupported;
		}else{
			//last bulk
			if(fileSize<loadOffset+loadSize){
				loadSize=fileSize-loadOffset;		
			}
			readSize = DOS_FileRead(fd, addr<<1, loadSize, loadOffset);
			if(readSize<d0){
				sidcPrintfImport("read1st-%lu\n",readSize);
				readSize = DOS_FileRead(fd, addr<<1, loadSize, loadOffset);				
			}		
			if(inSize>readSize-d0){
				inSize=readSize-d0;
			}
			sidcPrintfImport("(%lu,%lu,%lu)load(%lu,%lu)in(%lx-%lx,%lu)file(%lu/%lu)\n",d0,dd,d1,loadOffset,loadSize,addr,inAddr,inSize,readSize,fileSize);
		
			sidcDataIn(inAddr, inSize);
		}

	}else{
		rst = K_PIMARES_InvalidObjectHandle;
	}
	//printf("addr:(%lx,%lx)file:(%lx,%lx)(%lx,%lx)\n",addr,fileAddr,fileOffset,fileSize,size,readSize);
	DOS_FileClose(fd);
	DOS_CurrDriverSet(dev);
	if(rst==K_PIMARES_OK){
		return sidcResponse(K_PIMARES_OK, 1, &(pCommand->ulParameter[2]));
	}else{
		return sidcResponse(rst,0,0);	
	}
}

#elif 1

UINT8 pimaGetPartialObject()//UINT32 *pSize)
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle=M_ByteSwapOfDword(pCommand->ulParameter[0]);
	UINT32 offset=M_ByteSwapOfDword( pCommand->ulParameter[1]);
	UINT32 maxBytes=M_ByteSwapOfDword(pCommand->ulParameter[2]);

	UINT8 dev=DOS_CurrDriverGet();
	UINT32 addr =  K_SDRAM_FreeAddr+6;
	UINT8 path[33];
	UINT8 type = sp1kSidcParsingHandle(handle,path);	
	UINT8 fd = DOS_FileOpen(path, 'r');
	UINT32 size;
	UINT16 rst = K_PIMARES_OK;	
	UINT32 clusterSize = G_DOS_ClusterSize;
	UINT32 fileOffset = offset - (offset%clusterSize);
	UINT32 fileSize = offset - fileOffset + maxBytes + clusterSize-1;
	UINT32 fileAddr = addr+(offset%clusterSize)/2;
	UINT32 readSize = 0;
	sidcPrintfPima("getPartOjb-%lx,%lx,%lx,%lx\n",handle,offset,maxBytes,clusterSize);
	//type=sp1kSidcParsingHandle(handle,path);
	//fd = DOS_FileOpen(path, 'r');
	if (fd)
	{
		DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
		sidcPrintfInfo("fileSize:%lx\n",size);
		#if 1
		if(fileOffset>=size){
			rst = K_PIMARES_ParameterNotSupported;
		}else{
			//last bulk
			if(fileSize>size-fileOffset){
				fileSize=size-fileOffset;
			}
			readSize = DOS_FileRead(fd, addr<<1, fileSize, fileOffset);
			if(fileSize<readSize){
				fileSize=readSize;
			}
			sidcDataIn(fileAddr-6, fileSize);
		}
		#else
		if(offset>=size){
			rst = K_PIMARES_ParameterNotSupported;
		}else{
			if(maxBytes>size-offset){
				maxBytes=size-offset;
			}
			addr<<=1;
			size = DOS_FileRead(fd, addr-(offset&0x01), maxBytes+clusterSize-1, offset);
			if(maxBytes<size){
				maxBytes=size;
			}
			addr += offset%clusterSize;
			addr>>=1;
			sidcDataIn(addr-6, maxBytes);
		}		
		#endif
	}else{
		rst = K_PIMARES_InvalidObjectHandle;
	}
	printf("addr:(%lx,%lx)file:(%lx,%lx)(%lx,%lx)\n",addr,fileAddr,fileOffset,fileSize,size,readSize);
	DOS_FileClose(fd);
	DOS_CurrDriverSet(dev);
	if(rst==K_PIMARES_OK){
		return sidcResponse(K_PIMARES_OK, 1, &(pCommand->ulParameter[2]));
	}else{
		return sidcResponse(rst,0,0);	
	}
}
#else

/**
 * @fn        void pimaGetPartialObject(void)
 * @brief     pimaGetPartialObject
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 pimaGetPartialObject()//UINT32 *pSize)
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 handle=M_ByteSwapOfDword(pCommand->ulParameter[0]);
	UINT32 offset=M_ByteSwapOfDword( pCommand->ulParameter[1]);
	UINT32 maxBytes=M_ByteSwapOfDword(pCommand->ulParameter[2]);

	UINT32 addr =  K_SDRAM_FreeAddr+6;
//	UINT8 fd;
	UINT8 type;
	UINT8 path[33];
	
	static UINT32 size=0;
	static UINT32 oldHnd=0xffffffff;
	//UINT16 maxBulkSize = K_USBSIDC_BULK_SIZE_MAX;

	dpsPrintfName("getPartOjb",handle);

	if(handle!=oldHnd)
	{
		UINT8 dev=DOS_CurrDriverGet();
		type=sp1kSidcParsingHandle(handle,path);
		if(FALSE==dramFileLoad(path,addr,&size))
		{
			return sidcResponse(K_PIMARES_GeneralError,0,0);
		}
		oldHnd=handle;
		DOS_CurrDriverSet(dev);
	}
	
	if((offset & 0x01) || offset>=size)
	{
		return sidcResponse(K_PIMARES_ParameterNotSupported,0,0);
	}
	if(maxBytes>size-offset)
		maxBytes=size-offset;
	sidcDataIn(addr+offset/2-6, maxBytes);
	return sidcResponse(K_PIMARES_OK, 1, &(pCommand->ulParameter[2]));
/*
	type=sp1kSidcParsingHandle(handle,path);
	fd = DOS_FileOpen(path, 'r');
	
	DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
	if(maxBytes>size-offset)
		maxBytes=size-offset;
	
//#if DPS_PRINT_2_ENABLE
	printf("%lx,%lx,%lx\n",offset,maxBytes,size);
//#endif
	//sidcBuildContainer(G_ucStorData,maxBytes);
	//sidcDramWriteBytes(G_ucStorData, addr, 12);
	
	if(DOS_FileRead(fd, (addr<<1), maxBytes, offset))
	{
		DOS_FileClose(fd);
	}
	else
	{
	printf("nof\n");
		//sidcBuildContainer(G_ucStorData,0);
		//sidcSram2Usb(G_ucStorData,12);
		DOS_FileClose(fd);
		sidcDataIn(0,0);
		return;
	}
	DOS_CurrDriverSet(dev);

	sidcDataIn(addr, maxBytes);
	sidcResponse(K_PIMARES_OK, 1, &(pCommand->ulParameter[2]));
	//sidcDram2Usb(addr,maxBytes+(UINT32)12);

	//response param
	//G_USBSIDC_ulResponseParam[0]=pCommand->ulParameter[2];
	//G_USBSIDC_ucResponseParamCount=1;
*/	

}

#endif
#endif
