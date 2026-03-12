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
 * @file      sidc_main.c
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sidc_func.h"
//#include "exifhdr.h"
#include "Exiffunc.h"

#include "dos32.h"
#include "cardui.h"
//#include "gpio.h"
#include "sp1k_dps_api.h"
#include "sp1k_pb_api.h"
#include "dbg_def.h"
#include "reg_usb.h"
#include "dosusr.h"
#include "dcf_api.h"

#include "exif_def.h"
#include "exif_entry.h"

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
UINT8 sidcFlag = SIDC_Null;
UINT8 dpsFlag = DPS_Null;
UINT8 dpsStatus; 
sidc_data_t xdata*sidcData = (sidc_data_t xdata*)G_FPGAMemPool;
//extern xdata UINT8 G_Card_Type;

//UINT8 g_sidcMainFlag;
//UINT32 g_sidcFileCount;
//UINT32 g_sidcFileIndex=0;
//#define sidc_handleTableAddr (K_SDRAM_FreeAddr+32L*1024L)
//#define sidc_fileInfoTableAddr (K_SDRAM_FreeAddr+64L*1024L) 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void sidcCommand(void)
 * @brief     sidc Command for main loop
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-13
*/
void sidcCommand(void) 
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT8 ret = TRUE;
	switch(pCommand->usCode)
	{
	case K_PIMAOPE_GetObjectHandles:
		ret = pimaGetObjectHandles();		
		break;
	case K_PIMAOPE_GetObjectInfo:
		ret = pimaGetObjectInfo();		
		break;		
	case K_PIMAOPE_GetObject:
		ret = pimaGetObject();		
		break;
	case K_PIMAOPE_GetThumb:
		ret = pimaGetThumb();		
		break;
	case K_PIMAOPE_SendObjectInfo:
		//ret = pimaSendObjectInfo();		
		break;
	case K_PIMAOPE_SendObject:
		//ret = pimaSendObject();		
		break;
	case K_PIMAOPE_GetPartialObject:
		ret = pimaGetPartialObject();		
		break;	
	default:
		ret = FALSE;
		break;
	
	}
	if(ret == FALSE){
		sidcPrintfErr("sidcRetErr-%x\n",pCommand->usCode);
		sidcResponse(K_PIMARES_GeneralError, 0, NULL);
	}
}
//extern xdata UINT8 G_Detect_PinMux;
/**
 * @fn        UINT32 sidcMakeHandles(void)
 * @brief     make handles
 * @param	  [in]addr
 * @param	  [in]typeFilter
 * @retval    return = SUCCESS / FAIL
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcMakeHandles(UINT32 addr,UINT8 typeFilter)
{
	UINT8 dev=DOS_CurrDriverGet();	
	UINT32 fileCount= dcfFileCountGet();//G_USR_FileCount;
	//UINT8 flag=0x01;
	//UINT32 maxCnt=sp1kDpsGetParam(DPS_PARAM_SidcSurportImageCountMax);
	//printf("dev:%bx,%bx\n",dev,DOS_CurrDriverGet());
	UINT32 addrFdb = K_SDRAM_JPG_FdbParameterBufAddr;
	//UINT32 addrFree = K_SDRAM_FreeAddr;
	UINT32 addrOdr = K_SDRAM_FileOrderTableAddr;// +/* 9999 + */(order - 1);
	UINT16 i;
	UINT8 tmp1,tmp2,tmp3,tmp0;
	UINT16 index;
	UINT16 dir = dcfDir1IndexGet();//G_USR_Dir1Count;
	UINT32 handle;
	UINT8 type;
	UINT32 cnt = 0;
	UINT32 addr32 = addr+2;
	
	sidcWriteHandlePath(handle, dev, dir);
	for(i=0;i<fileCount;i++){	
		//read file index
		HAL_DramStartAddrSet(addrOdr+i, 1);
		HAL_DramWordDataRead(&tmp0, &tmp1);
		index = (UINT16)(((UINT16)tmp1) <<8) +( (UINT16)tmp0);

		//make handle
		//sidcWriteHandlePath(handle, dev, dir);
		HAL_DramStartAddrSet(addrFdb + (((UINT32)index - 1)) * 10+2, 1);
		HAL_DramWordDataRead(&tmp0,&tmp1);
		type = tmp1;
		if(typeFilter && typeFilter!=type)continue;
		sidcWriteHandleFile(handle, type, index);
		sidcPrintfInfo("%4u-%8lx-%2bu,%4u,%2bu,%4u,%4x\n",i,handle,dev,dir,type,index,index);
		sidcPrintfImport("%4u-%8lx-%4x\n",i,handle,index);
		//write handle to dram
		tmp0 = READ8(handle, 3);
		tmp1 = READ8(handle, 2);
		tmp2 = READ8(handle, 1);
		tmp3 = READ8(handle, 0);	
		HAL_DramStartAddrSet(addr32,0);//addr + i*2+2, 0);
		HAL_DramWordDataWrite(tmp0,tmp1);
		HAL_DramWordDataWrite(tmp2,tmp3);
		addr32+=2;
		cnt++;
	}
	tmp0 = READ8(cnt, 3);
	tmp1 = READ8(cnt, 2);
	tmp2 = READ8(cnt, 1);
	tmp3 = READ8(cnt, 0);	
	HAL_DramStartAddrSet(addr, 0);
	HAL_DramWordDataWrite(tmp0,tmp1);
	HAL_DramWordDataWrite(tmp2,tmp3);
	
	DOS_CurrDriverSet(dev);
	sidcPrintfImport("fileCount:%lu-%lu\n",fileCount,cnt);
	return cnt;
}
#if 0
UINT32 sidcMakeCurrDirHandles(UINT32 startIndex)
{

	UINT32 addr =sidc_handleTableAddr +(startIndex<<1);
	UINT16 i;
	UINT16 fileCount = G_USR_FileCount;
	UINT32 handle;
	UINT8 *ph = (UINT8*)&handle;

	for(i=0;i<fileCount;i++)
	{
		handle =sidcMakeHandleFromOrd(i);
		handle = M_ByteSwapOfDword(handle);
		HAL_DramStartAddrSet(addr, 0);
		HAL_DramWordDataWrite(ph[0], ph[1]);
		HAL_DramWordDataWrite(ph[2], ph[3]);
		addr+=2;
	}

	return fileCount;
}
UINT32 sidcMakeHandleFromOrd(UINT16 fileOrd)
{
	UINT8 tmp1=0,tmp2=0;
	UINT16 fileIdx=0;
	
	HAL_DramStartAddrSet(K_SDRAM_FileOrderTableAddr+fileOrd, 1);
	HAL_DramWordDataRead(&tmp1,&tmp2);
	fileIdx=((UINT16)tmp2<<8)+tmp1;

	return sp1kSidcMakeHandle(fileIdx);
}
#endif

/**
 * @fn        UINT8 sidcGetFileInfo(UINT32 handle, UINT32 addr, sidc_file_info_t* pInfo)
 * @brief     get file info
 * @param     [in] handle	
 * @param     [in] addr		:load jpg header
 * @param     [out] pInfo	:return info
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sidcGetFileInfo(UINT32 handle, UINT32 addr, sidc_file_info_t* pInfo)
{
	UINT8 fd;
	UINT8 type;

	UINT32 ret;
	UINT8 dev=DOS_CurrDriverGet();
	
	memset(pInfo,0,sizeof(sidc_file_info_t));
	pInfo->handle = handle;
	type=sp1kSidcParsingHandle(handle,pInfo->filePath);
	switch(type)
	{
	case K_DCF_JPG_TYPE:
		pInfo->fileType=K_PIMAOBJ_EXIF_JPEG;
		break;
	case K_DCF_AVI_TYPE:
		pInfo->fileType=K_PIMAOBJ_AVI;
		break;
	case K_DCF_WAV_TYPE:
		pInfo->fileType=K_PIMAOBJ_WAV;
		break;
	default:
		sidcPrintfErr("typeErr:%bx\n",type);
		return FALSE;
	}
	pInfo->fileName=(UINT8*)(pInfo->filePath+17);

	fd = DOS_FileOpen(pInfo->filePath, 'r');	
	DOS_CurrDriverSet(dev);
	if(fd==0) {
		sidcPrintfErr("no file:%s\n",pInfo->filePath);
		return FALSE;
	}
	
	ret=DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&(pInfo->fileSize));
	if(pInfo->fileType==K_PIMAOBJ_EXIF_JPEG)
	{
		//struct Exif_Decompress_Parameter ThumbPara,ImagePara;
		ExifImage_t thumbPara,imagePara;
		
		UINT32 size=64L*1024L;
		ret=DOS_FileRead(fd, addr<<1, size, 0);
		//printf("%lx\n",ret);
		//ret=exifParsingFile(addr, size, 0,NULL,&ImagePara,&ThumbPara);
		ret = exifParserStart(exifGetMainInfo,&imagePara,addr,size);
		ret = exifParserStart(exifGetThumbInfo,&thumbPara,addr,size);
		//printf("%lx\n",ret);
		pInfo->width = imagePara.width;
		pInfo->height = imagePara.height;
		pInfo->thembWidth = thumbPara.width;
		pInfo->thembHeight = thumbPara.height;
		pInfo->thembAddr = addr*2 +G_ulExifThumbAddr;
		pInfo->thembSize= G_ulExifThumbSize;
	}
	DOS_FileClose(fd);
		
	sidcPrintf("pInfo:\n%s\ntype:%x\n",pInfo->filePath,pInfo->fileType);
	sidcPrintf("hd:%lx\n%s\nsize:%lu\n",pInfo->handle,pInfo->fileName,pInfo->fileSize);
	sidcPrintf("w:%u\nh:%u\n",pInfo->width,pInfo->height);
	sidcPrintf("addr:%lx\ns:%lu\n",pInfo->thembAddr,pInfo->thembSize);	
	sidcPrintf("tw:%x\nth:%x\n",pInfo->thembWidth,pInfo->thembHeight);
	return TRUE;
}
#if 0
void sidcGetFileInfoStorage(UINT32 handle,sidc_file_info_t* pInfo)
{
	sidcDramReadBytes((UINT8 *)pInfo, sidc_fileInfoTableAddr+(g_sidcFileIndex *SIDC_PARA_FILE_INFO_SIZE>>1), SIDC_PARA_FILE_INFO_SIZE);
	//printf("%lx,%lx\n",handle,pInfo->handle);
	if(handle!=pInfo->handle)
	{
		sidcGetFileInfo(handle, K_SDRAM_FreeAddr, pInfo);
		sidcDramWriteBytes((UINT8 *)pInfo, sidc_fileInfoTableAddr+(g_sidcFileIndex*SIDC_PARA_FILE_INFO_SIZE>>1), SIDC_PARA_FILE_INFO_SIZE);
		
	}
	pInfo->fileName=(UINT8*)(pInfo->filePath+17);
	g_sidcFileIndex++;

}
#endif
#if 0
/**
 * @fn        void sidcDataIn(UINT32 addr, UINT32 size)
 * @brief     sidc DataIn phase
 * @param     [in] addr	:data dram addr(w). if 0, DataIn G_ucStorData
 * @param     [in] size
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcDataIn(UINT32 addr, UINT32 size)
{
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	sidc_container_t* pContainer=(sidc_container_t**)G_ucStorData;
	size+=12;
	
	pContainer->ulContainerLength = M_ByteSwapOfDword(size);
	pContainer->usContainerType = K_PIMADATA_ContainerType_DataBlock;
	pContainer->usCode = pCommand->usCode;
	pContainer->ulTransactionId = pCommand->ulTransactionId;

	if(addr)	{
		addr-=6;
		sidcDramWriteBytes(G_ucStorData, addr, size);
		sidcDram2Usb(addr, size);
	}else{
		sidcSram2Usb(G_ucStorData, size);
	}
}
/**
 * @fn        void sidcResponse(UINT8 rst, UINT8 cnt, UINT32* ps)
 * @brief     sidc response phase
 * @param     [in] rst	:result of current command
 * @param     [in] cnt	:param count
 * @param     [in] ps	:param list
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcResponse(UINT8 rst, UINT8 cnt, UINT32* ps)
{
	UINT8 i;
	sidc_container_t* pResponse = (sidc_container_t*) G_ucStorData;	
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 len = 12+cnt*4;

	pResponse = (sidc_container_t*) G_ucStorData;
	pResponse->ulContainerLength = M_ByteSwapOfDword(len);
	pResponse->usContainerType = K_PIMADATA_ContainerType_ResponseBlock;
	pResponse->usCode = rst;
	pResponse->ulTransactionId = pCommand->ulTransactionId;

	if(ps){
	       for (i = 0; i < cnt; i++){
			pResponse->ulParameter[i] = ps[i];
		}
	}

	sidcSram2Usb(G_ucStorData, len);

}
#endif
/**
 * @fn        UINT8 sidcInterruptIn(UINT8* point, UINT16 size)
 * @brief     usb interruptIn
 * @param     [in] point	:data pointer
 * @param     [in] size	:data size
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sidcInterruptIn(UINT8* point, UINT16 size)
{
	UINT32 i;
	UINT8 res = TRUE;
	UINT8 t00,t01,t04,t05,t46,t4a;

	t00 = XBYTE[REG_Usb_EpAckIntSts];
	t01 = XBYTE[REG_Usb_EpNakIntSts];
	t04 = XBYTE[REG_Usb_EpAckIntEn];
	t05 = XBYTE[REG_Usb_EpNakIntEn];
	t46 = XBYTE[REG_Usb_TransactionEn];
	t4a = XBYTE[REG_Usb_BufClr];
	dbgWdtFeed(-1);	
	//printf("2500=%bx\n",XBYTE[0x2500]);
	//printf("2501=%bx\n",XBYTE[0x2501]);
	//printf("2504=%bx\n",XBYTE[0x2504]);
	//printf("2505=%bx\n",XBYTE[0x2505]);
	//printf("2546=%bx\n",XBYTE[0x2546]);
	//printf("254a=%bx\n",XBYTE[REG_Usb_BufClr]);
	//EA=0;
	XBYTE[REG_Usb_EpAckIntEn] &= ~0X20;//K_USB_INT_IN_ACK_MASK
	XBYTE[REG_Usb_EpNakIntEn] &= ~0x20;
	
	XBYTE[REG_Usb_BufClr] |= 0x20;
	//printf("SIZE:0x%x\n",size);
	for (i = 0;i < size; i++)
	{
		//printf("%bx+",point[i]);
		XBYTE[REG_Usb_IntINData] = point[i];
	}
	//printf("\n");
	XBYTE[REG_Usb_EpAckIntSts] |= 0X20;
	XBYTE[REG_Usb_TransactionEn] |= 0X20;//K_USB_INT_IN_OPEN
	XBYTE[REG_Usb_EpAckIntEn] |= 0X20;//K_USB_INT_IN_ACK_MASK

	while((XBYTE[REG_Usb_TransactionEn]) & 0x20)
	{
		i++;
		if(i>=0x80000)
		{
			osMsgPost(SP1K_MSG_DPS_ERROR_SIDC);
			sidcPrintfErr("\nENDP4 TimeOut\n");
			
			res = FALSE;
			break;
		}
	}
	//XBYTE[0x2500] |= 0x20;
	// EA=1;
	 //XBYTE[0x2504] |= 0x38;  //Bulk in 1 acked interrupt enable
	// XBYTE[0x2505] |= 0x20;

	XBYTE[REG_Usb_EpAckIntSts] = t00;
	XBYTE[REG_Usb_EpNakIntSts] = t01;
	XBYTE[REG_Usb_EpAckIntEn] = t04;
	XBYTE[REG_Usb_EpNakIntEn] = t05;
	XBYTE[REG_Usb_TransactionEn] = t46;
	XBYTE[REG_Usb_BufClr] = t4a;
	
	//printf("2500=%bx\n",XBYTE[0x2500]);
	//printf("2501=%bx\n",XBYTE[0x2501]);
	//printf("2504=%bx\n",XBYTE[0x2504]);
	//printf("2505=%bx\n",XBYTE[0x2505]);
	//printf("2546=%bx\n",XBYTE[0x2546]);
	//printf("254a=%bx\n",XBYTE[REG_Usb_BufClr]);
	return res;
}
#if 0
/**
 * @fn        UINT32 sidcBulkInOut(UINT32 addrW, UINT8* pt, UINT32 size)
 * @brief     usb bulk In & out
 * @param     [in] addrW	:data addr(w),0 invalid
 * @param     [in] pt		:data pointer, 0 invalid
 * @param     [in] size		
 * @retval    return = BulkOut size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcBulkIn(UINT32 addrW, UINT8*pt, UINT32 size)
{
	if(!addrW){
		HAL_UsbBulkFlashAddrSet(0,addrW);
	}else if(!pt){
		HAL_UsbBulkFlashAddrSet(1,(UINT16)pt);
	}
	HAL_UsbBulkIn(&size);
	return size;
}

/**
 * @fn        UINT32 sidcBulkOut(UINT32 addrW, UINT8* pt, UINT32 size)
 * @brief     usb bulk In & out
 * @param     [in] addrW	:data addr(w),0 invalid
 * @param     [in] pt		:data pointer, 0 invalid
 * @param     [in] size		
 * @retval    return = BulkOut size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcBulkOut(UINT32 addrW, UINT8*pt, UINT32 size)
{
	if(!addrW){
		HAL_UsbBulkFlashAddrSet(0,addrW);
	}else if(!pt){
		HAL_UsbBulkFlashAddrSet(1,(UINT16)pt);
	}
	HAL_UsbBulkOut(&size);
	return size;
}
#endif
/**
 * @fn        void sp1kSidcSendEvent(UINT32 eventCode, UINT32 param0, UINT32 param1, UINT32 param2)
 * @brief     send sidc event 
 * @param     [in] eventCode
 * @param     [in] param0
 * @param     [in] param1
 * @param     [in] param2
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcSendEvent(
	UINT32 eventCode,
	UINT32 param0,
	UINT32 param1,
	UINT32 param2
) 
{
   
	UINT8 intrBuf[64];
	//UINT8* intrBuf = G_ucStorData;
	sidc_container_t* pContainer;
//	UINT16 i;
	UINT8 paramCnt;
	
	sidcPrintfPima("sendEvt:%lx\n",param0);

	pContainer = (sidc_container_t*) intrBuf;	
	switch (eventCode) {
	case K_PIMAEVE_CancelTransaction:
	case K_PIMAEVE_DeviceInfoChanged:
	case K_PIMAEVE_DeviceReset:
	case K_PIMAEVE_UnreportedStatus:
		paramCnt = 0;
		break;	
		
	case K_PIMAEVE_ObjectAdded:
	case K_PIMAEVE_ObjectRemoved:
	case K_PIMAEVE_StoreAdded:
	case K_PIMAEVE_StoreRemoved:
	case K_PIMAEVE_DevicePropChanged:
	case K_PIMAEVE_ObjectInfoChanged:
	case K_PIMAEVE_RequestObjectTransfer:
	case K_PIMAEVE_StoreFull:
	case K_PIMAEVE_StorageInfoChanged:
	case K_PIMAEVE_CaptureComplete:
		paramCnt = 1;
		break;
				
	default:
		paramCnt = 0;
		break;
	}
	
	pContainer->ulContainerLength = M_ByteSwapOfDword(12 + (paramCnt * (UINT32)4));
	pContainer->usContainerType = K_PIMADATA_ContainerType_EventBlock;
	pContainer->usCode = eventCode;
	pContainer->ulTransactionId = 0xFFFFFFFF;
	pContainer->ulParameter[0] = M_ByteSwapOfDword(param0);
	pContainer->ulParameter[1] = M_ByteSwapOfDword(param1);
	pContainer->ulParameter[2] = M_ByteSwapOfDword(param2);

	sidcInterruptIn(intrBuf , 12 + (paramCnt * (UINT32)4));
		
}
/**
 * @fn        void sidcRequestObjectTransfer(UINT32 handle)
 * @brief     send request object transfer event
 * @param     [in] handle
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcRequestObjectTransfer(UINT32 handle)
{
	sidcSendEvent(K_PIMAEVE_RequestObjectTransfer,handle,0x00000000,0x00000000);
}
void sidcMakeFolderHandle(UINT32* pHandle)
{
	UINT8 dev= DOS_CurrDriverGet();
	UINT16 dir= dcfDir1IndexGet();//G_USR_Dir1Count//DOS_CurrDirGet(dev);
	sidcWriteHandlePath((*pHandle), dev, dir);
	sidcPrintfInfo("MakeFolderHandle:%lx,%bu,%u\n",*pHandle,dev,dir);
}

UINT8 sidcMakeFileHandle(
	UINT8 order,
	UINT32* pHandle,
	UINT8* fname,
	UINT8* pDate
	)
{
	UINT16 idx=sp1kPbFileIndexByOrderGet(order);
	UINT8 type=0;
	//UINT32 handle=0;
	UINT8 tmp1=0,tmp2=0;
	UINT32 addr = K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)idx - 1)) * 10+2;
	HAL_DramStartAddrSet(addr, 1);
	HAL_DramWordDataRead(&tmp1,&tmp2);
	type = tmp2;	
	sidcWriteHandleFile((*pHandle), type, idx);
	//sidcPrintfInfo("MakeFileHandle:%lx,%bu,%u\n",*pHandle,type,idx);
	//if(/*K_DCF_JPG_TYPE*/fileType != type)return 0;
	sidcPrintfInfo("%4u,%8lx,%2bu",idx,*pHandle,type);
	if(fname){
		HAL_DramStartAddrSet(addr+4, 1);
		HAL_DramWordDataRead(fname+0,fname+1);
		HAL_DramWordDataRead(fname+2,fname+3);
		sprintf(fname+4,"%04u.JPG",idx);
		sidcPrintfInfo(",%s",fname);
	}
	if(pDate){
		sidcPrintfInfo(",%s",pDate);
	}
	sidcPrintfInfo("\n");
	return type;
}

UINT8 sidcCheckImageFileType(UINT32 handle)
{
	UINT8 type;
	sidcReadHandleType(handle,type);
	if(K_DCF_JPG_TYPE != type)
	{
		sidcPrintfErr("fileTypeErr:%8lx,%bx\n",handle,type);
		return 0;
	}
	return 1;
}
#if 0
/**
 * @fn        void sidcScriptFileNode(UINT32 handle, UINT32 fileSize, UINT8 isOut)
 * @brief     create file node
 * @param     [in] handle
 * @param     [in] fileSize
 * @param     [in] isOut		1:bulkout file 0:bulkin file
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcScriptFileNode(UINT32 handle, UINT32 fileSize, UINT8 isOut)
{
	sidc_node_t node;
	UINT8 * pNode = (UINT8 *)&node;
	node.handle = handle;
	dpsGetScriptInfo(&handle,node.fileName);
	node.fileType = K_PIMAOBJ_Script;
	node.fileSize = fileSize;
	node.reserved = 0;

	//dpsSaveFileNode(pNode);
	if(isOut)
		dpsSaveOutFileNode(pNode);
	else
		dpsSaveInFileNode(pNode);
	//printf("FILE NODE: \n%lx\n%s\n%x\n%lx\n",node.handle,node.fileName,node.fileType,node.fileSize);
	
}
#endif
#endif

/**
 * @fn        UINT32 sp1kSidcMakeHandle(UINT16 fileIndex)
 * @brief     create file handle in current folder
 * @param     [in] fileIndex
 * @retval    return = handle
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sp1kSidcMakeHandle(UINT16 fileIndex)
{
#if USBSIDC_OPTION
	UINT16 dir=0,idx=fileIndex;
	UINT8 dev=0,type=0;
	UINT32 handle=0;
	UINT8 tmp1=0,tmp2=0;
	
	dev= DOS_CurrDriverGet();
	dir= dcfDir1IndexGet();//G_USR_Dir1Count;//DOS_CurrDirGet(dev);
	sidcWriteHandlePath(handle, dev, dir);
	//printf("--%bx,%x,%lx\n",dev,dir,handle);

	HAL_DramStartAddrSet(K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)idx - 1)) * 10+2, 1);
	HAL_DramWordDataRead(&tmp1,&tmp2);
	type = tmp2;
	sidcWriteHandleFile(handle, type, idx);
	//printf("--%bx,%x,%lx\n",type,idx,handle);
	return handle;
#else
	fileIndex = fileIndex;
	return 0;
#endif
}
/**
 * @fn        UINT8 sp1kSidcParsingHandle(UINT32 handle, UINT8 *fileName)
 * @brief     parse handle to full path
 * @param     [in] handle
 * @param     [out] fileName
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kSidcParsingHandle(
	UINT32 handle, 
	UINT8 *fileName)
{
#if USBSIDC_OPTION

	UINT8 * pt=fileName;
	UINT16 i;
	UINT16 dir,idx;
	UINT8 dev,type;
	FDBMediaFolderInfo_t forderInfo;
	FDBInfo_t fileInfo;
	
	sidcReadHandleDev(handle,dev) ;
	sidcReadHandleDir(handle,dir);
	sidcReadHandleType(handle,type);
	sidcReadHandleFileIndex(handle,idx) ;
	sidcPrintfInfo("dev:%bu,dir:%u,type:%bu,idx:%u\n",dev,dir,type,idx);
	if(dev!= DOS_CurrDriverGet()){
		sidcPrintfErr("IsNotCurDev!\n");
		return FALSE;
	}
	
	USR_GetMediaFolderInfo(dir, &forderInfo);
	dcfMediaFolderAssign(1,dir);
	dcfFileDramFdbParameterRead(idx,&fileInfo);
	
	switch(dev){
        case DRIVE_NAND:
            *pt ++ = 'C';
            break;
        case DRIVE_SD:
            *pt ++ = 'D';
            break;
        case DRIVE_DRAM:
            *pt ++ = 'E';
            break;
#if (DOS_RSV_PART == 1)
        case DRIVE_NAND_RSV:
            *pt ++ = 'F';
            break;
#endif
        default:
            *pt = 0;
            return FALSE;                
    }
    *pt ++ = ':';
    *pt ++ = '\\';
            
	for ( i = 0; i < 4; i++ )
        *pt ++  = G_USR_Dir0Name[i];    //DCIM
    *pt ++ = '\\';
	
	for(i=0;i<8;i++)
	{
		*pt++ = forderInfo.name[i];
	}
	*pt ++ = '\\';
	for(i=0;i<4;i++)
	{
		*pt++ = fileInfo.FileName[i];
	}
	sprintf(pt,"%04U",idx);
	pt+=4;
	/*
    pt += 7;
    for(i = 7; i > 2; i--)
        *pt -- = G_USR_Dir1Name[i];    //; 100MEDIA
    i = 3;
    while(i){
        *pt -- = '0' + dir %10;
        dir /= 10;
        i --;
    }; 
    pt += 9;
    *pt ++ = '\\';
    
    dcfBaseNameGet(pt, idx);    
    pt += 8;
    *pt ++ = '.';*/
	switch(type)
	{
	case K_DCF_JPG_TYPE:
		*pt ++ = 'J';
		*pt ++ = 'P';
		*pt ++ = 'G';
		break;
	case K_DCF_AVI_TYPE:
		*pt ++ = 'A';
		*pt ++ = 'V';
		*pt ++ = 'I';
		break;
	case K_DCF_WAV_TYPE:
		*pt ++ = 'W';
		*pt ++ = 'A';
		*pt ++ = 'V';
		break;
	case K_DCF_OUR_TYPE:
	
		break;
	} 	
    *pt = 0;
	

	sidcPrintfImport("%lx-%s\n",handle,fileName);
    return type;
#else
	handle = handle;
	fileName = fileName;
	return 0;
#endif
}
void sp1kSidcInit()
{
#if USBSIDC_OPTION
	sidcPrintfPima("sidcInit\n");
	//XBYTE[REG_Usb_BulkAutoMode] |=0x18;
	//sidcData = (sidc_data_t xdata*)G_FPGAMemPool;
	memset(G_FPGAMemPool,0,sizeof(G_FPGAMemPool));	
	sp1kNandRsvRead(SIDC_RES_ID, K_DPS_ADDR_Start<<1);
	HAL_DramDmaRead(sidcData->devData, K_DPS_ADDR_Start, 512, 1);
	sidcFlag = SIDC_Null;////SIDC_IntrCommand;
	dpsFlag = DPS_Null;
	//G_ucBulkPhase = K_CommandPhase;
	//sidcData->codeSize = K_SDRAM_CodeSize;
#endif
}

/**
 * @fn        void sidcSvcProc(void)
 * @brief     sidc main loop
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcSvcProc()
{
#if USBSIDC_OPTION
	if(sidcFlag==SIDC_MainLoop){	
		XBYTE[REG_Usb_TransactionEn] &= 0xef;//OFF bulkout
		XBYTE[REG_Usb_EpAckIntEn] &= 0xef;//OFF bulkout interrupt
		sidcFlag=SIDC_Null; 
		sidcCommand();			
		XBYTE[REG_Usb_EpAckIntEn] |= 0x10;//on bulkout interrupt
		XBYTE[REG_Usb_TransactionEn] |= 0x10;	//on bulkout
	}
#endif
}


