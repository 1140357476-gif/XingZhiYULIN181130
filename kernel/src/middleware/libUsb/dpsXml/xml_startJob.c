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
 * @file      xml_startJob.c
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "xml_func.h"
#include "dosusr.h"
#include "hal_dram.h"
#include "reg_dram.h"

#if USBSIDC_OPTION == 2 
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


#if 0
UINT16 xmlStartJobTreeMake( PDPS_PARA_ConfigStartJob pCsj ,UINT8 single_Multi) {
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];

	xmlGetGeneralName( 0,pXmlIdx );
	pXmlIdx += strlen( pXmlIdx );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	//startJob start tag
	xmlGetJobConfigItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );
	
	//input Job Config
	xmlConstructJobConfigItem( pCsj, &pXmlIdx );
	
	//input printInfo
	if(single_Multi == DPS_ACTION_JobStart_SinglePrint)
		xmlConstructSinglePrintInfoItem( K_DPS_ADDR_ConfigPrintInfo, &pXmlIdx, pCsj );
	else if(single_Multi == DPS_ACTION_JobStart_MultiPrint)
		xmlConstructMultiPrintInfoItem( K_DPS_ADDR_MultiPrintHandles, &pXmlIdx, pCsj );
	else 
		return 0;
	//startJob end tag
	xmlGetJobConfigItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	//input end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}
#endif

/**
 * @fn        UINT16 xmlStartJobTreeMake(void)
 * @brief     xmlStartJobTreeMake
 * @param     NONE
 * @retval    return = index
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlStartJobTreeMake(dps_data_t xdata* dps,UINT32 addr) 
{
	DPS_PARA_ConfigStartJob *csj = &(dps->startJob);
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];
	UINT16 i,_count,size,_index,_multi,idx=0,cnt,copies;
	UINT32 _addr;
	UINT32 dramIndex=0;
	UINT32 handle = 0;
	DPS_PARA_ConfigPrintInfo* pci = dps->imageList;
	
	xmlGetGeneralName( 0,pXmlIdx );
	pXmlIdx += strlen( pXmlIdx );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	//startJob start tag
	xmlGetJobConfigItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );
	
	//input Job Config
	//dpsLoadConfigStartJob((UINT8 *)&csj);
	xmlConstructJobConfigItem(csj, &pXmlIdx );	
	
	//input printInfo
	dbgWdtFeed(-1);
	/*if(dps->imageMulti){		
		sidcMakeFolderHandle(&handle);
		dbgWdtFeed(100000);
		dps->imageCount = 0;
	}*/	
	size = pXmlIdx - G_ucStorData;
	_count = dps->imageCount;
	_index = dps->imageIndex;
	_multi = dps->imageMax;
	_addr = dps->imageHandles;
	if(_count && _multi){
		if(_index+_multi<_count)
			_count = _index+_multi;
		copies = pci->copies;
		if(copies)
			cnt = _multi/copies;
	}else{
		_multi=0;
		_index=0;
	}
	//dpsLoadConfigPrintInfoCount(_count);	
	dpsPrintfFunc("startJobXML:%4u,%4u,%4u\n",_index,_count,_multi);
	sidcMakeFolderHandle(&handle);
	for(i=_index;i<_count;i++)
	{		
		if(_addr){
			sidcMakeFileHandle(i+1,&handle,NULL,NULL);
			//HAL_DramStartAddrSet(_addr+i*2, 1);
			//HAL_DramRead32(&handle);
			pci->fileID = handle;
			//dpsPrintfInfo("%x,%lx\n",i,handle);
		}else{
			pci=dps->imageList+i;
		}
		if(!sidcCheckImageFileType(pci->fileID))continue;
		if(++idx>cnt)break;
		
		size+=xmlConstructSinglePrintInfoItem( pci//dps->imageList+i//K_DPS_ADDR_ConfigPrintInfo + ((i* DPS_PARA_ConfigPrintInfo_Size_Min +4)>>1)
										, &pXmlIdx
										, csj );
		if(size>=512)
		{
			G_ucStorData[size]=0;
			dpsPrintfXml("%s",G_ucStorData);
			//dpsSaveInData(G_ucStorData, dramIndex, (size & 0xfffe));
			HAL_DramDmaWrite(G_ucStorData, addr+(dramIndex>>1), (size & 0xfffe),0);
			pXmlIdx = G_ucStorData;
			dramIndex += size & 0xfffe;
			if((UINT8)size & 0x01)
			{
				G_ucStorData[0]=G_ucStorData[size-1];
				pXmlIdx++;
			}
			size &=0x0001;
		}
	}		
	dpsPrintfXml("\ncnt:%u-%u-%u-%u\n",idx,cnt,copies,dps->imageCount);
	dps->imageCount = idx*copies;
	//dps->imageCount = 0;
	//dpsClearConfigPrintInfo();
	
	//startJob end tag
	xmlGetJobConfigItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	//input end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

	size = pXmlIdx - G_ucStorData;
	G_ucStorData[size]=0;
	dpsPrintfXml("%s",G_ucStorData);
	//dpsSaveInData(G_ucStorData, dramIndex, size);
	HAL_DramDmaWrite(G_ucStorData, addr+(dramIndex>>1), (size & 0xfffe),0);
	dbgWdtFeed(10000);	
	return dramIndex+size;
	//return pXmlIdx - G_ucStorData;
}

/**
 * @fn        void xmlConvertUINT32toStr(UINT32 val, UINT8* buf, UINT16 bufSize)
 * @brief     xmlConvertUINT32toStr
 * @param     [in] val
 * @param     [in] buf
 * @param     [in] bufSize
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void xmlConvertUINT32toStr( UINT32 val, UINT8* buf, UINT16 bufSize ) {
	UINT16 idx = 0;
	for( ; idx < bufSize; idx++ )
		buf[ idx ] = 0;
		
	sprintf( buf, "%08lx", val );
}

/**
 * @fn        UINT16 xmlConstructJobConfigItem(figStartJob pCsj, UINT8** pXmlIdx)
 * @brief     xmlConstructJobConfigItem
 * @param     [in] pCsj
 * @param     [in] pXmlIdx
 * @retval    return = index
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlConstructJobConfigItem( PDPS_PARA_ConfigStartJob pCsj, UINT8** pXmlIdx ) {
	UINT8* startIdx = *pXmlIdx;
	UINT8 itemName[ ITEM_NAME_LEN ], itemData[ ITEM_NAME_LEN ];
	
	UINT32* pCsjStart = ( UINT32* )pCsj;
	UINT32* pCsjIdx = pCsjStart;
	UINT16 csjSize = sizeof( DPS_PARA_ConfigStartJob ) / sizeof( UINT32 );
	UINT32 val;
	
	//jobConfig start tag
	xmlGetJobConfigItemName(	1, itemName );
	xmlInputItemTag( pXmlIdx, itemName, 0 );
	
	while( pCsjIdx - pCsjStart < csjSize ) {
		val = *pCsjIdx;
		sidcPrintf("--:%lx\n",val);
		if( val != 0 )//&& !checkValue(val,K_DPS_HEX_PARAVAL_MASK_False )) 
		{
		//if((val && 0xff000000) == K_DPS_HEX_PARAVAL_FixedSizes_Default)
		//	{
		//	pCsjIdx++;
		//	continue;
		//	}
			xmlConvertUINT32toStr( val, itemData, ITEM_NAME_LEN );
			xmlGetJobConfigItemName(	pCsjIdx - pCsjStart + 2, itemName );
			xmlInputItem( pXmlIdx, itemName, itemData );
		}
		pCsjIdx++;
	}
	
	//jobConfig end tag
	xmlGetJobConfigItemName(	1, itemName );
	xmlInputItemTag( pXmlIdx, itemName, 1 );
	
	return *pXmlIdx - startIdx;
}



/**
 * @fn        UINT16 xmlConstructSinglePrintInfoItem(UINT32 dramAddr, UINT8** pXmlIdx, figStartJob pCsj)
 * @brief     xmlConstructSinglePrintInfoItem
 * @param     [in] dramAddr
 * @param     [in] pXmlIdx
 * @param     [in] pCsj
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlConstructSinglePrintInfoItem(/*UINT32 dramAddr*/DPS_PARA_ConfigPrintInfo* singlePrintInfo, UINT8** pXmlIdx,  PDPS_PARA_ConfigStartJob pCsj ) {

	UINT8 itemName[ ITEM_NAME_LEN ], itemData[ ITEM_NAME_LEN ];
	UINT8* pXmlStart = *pXmlIdx;
	UINT16 idx;
	
	//DPS_PARA_ConfigPrintInfo singlePrintInfo;
	//UINT16 spiSize = sizeof( DPS_PARA_ConfigPrintInfo );
	
	//HAL_DramReadBytesAddr( dramAddr, ( UINT8* )&singlePrintInfo, spiSize,0 );
	//memcpy( &singlePrintInfo, dramAddr, sizeof( DPS_PARA_ConfigPrintInfo ) );
	xmlPrintfInfo("SinglePrint:%bx\n",singlePrintInfo->inValid);
	xmlPrintfInfo("--%lx\n",singlePrintInfo->fileID);
	xmlPrintfInfo("--%x\n",singlePrintInfo->croppingArea_x);
	xmlPrintfInfo("--%x\n",singlePrintInfo->croppingArea_y);
	xmlPrintfInfo("--%x\n",singlePrintInfo->croppingArea_w);
	xmlPrintfInfo("--%x\n",singlePrintInfo->croppingArea_h);
	xmlPrintfInfo("--%x\n",singlePrintInfo->copies);
	xmlPrintfInfo("--%s\n",singlePrintInfo->fileName);
	xmlPrintfInfo("--%s\n",singlePrintInfo->date);
	if(singlePrintInfo->inValid)return 0;
	//printInfo start tag
	xmlGetSinglePrintInfoItemName( 0, itemName );
	xmlInputItemTag( pXmlIdx, itemName, 0 );
	
	//cropping
	if( (!checkValue(pCsj->cropping,K_DPS_HEX_PARAVAL_MASK_False )
		&& pCsj->cropping != 0)
							&& (	singlePrintInfo->croppingArea_x || 
								singlePrintInfo->croppingArea_y ||
								singlePrintInfo->croppingArea_w ||
								singlePrintInfo->croppingArea_h))// && !checkValue(pCsj->cropping,K_DPS_HEX_PARAVAL_MASK_False) ) 
	{
		xmlGetSinglePrintInfoItemName( 1, itemName );
		
		for( idx = 0; idx < ITEM_NAME_LEN; idx++ )
			itemData[ idx ] = 0;
		sprintf( itemData, "%04x %04x %04x %04x", singlePrintInfo->croppingArea_x, singlePrintInfo->croppingArea_y,
			singlePrintInfo->croppingArea_w, singlePrintInfo->croppingArea_h );
		
		xmlInputItem( pXmlIdx, itemName, itemData );
	}
	
	//fileId
	if( singlePrintInfo->fileID != 0 ) {
		xmlGetSinglePrintInfoItemName( 2, itemName );
		xmlConvertUINT32toStr( singlePrintInfo->fileID, itemData, ITEM_NAME_LEN );
		xmlInputItem( pXmlIdx, itemName, itemData );
	}
	
	//copies
	if( singlePrintInfo->copies != 0 ) {
		xmlGetSinglePrintInfoItemName( 3, itemName );
		//convertUINT32toStr( singlePrintInfo->copies, itemData, ITEM_NAME_LEN );
		memset(itemData,0,ITEM_NAME_LEN);
		sprintf( itemData, "%03u", singlePrintInfo->copies );
		xmlInputItem( pXmlIdx, itemName, itemData );
	}
	#if 0
	//prtPID
	if( singlePrintInfo->prtPID != 0 ) {
		xmlGetSinglePrintInfoItemName( 4, itemName );
		convertUINT32toStr( singlePrintInfo->prtPID, itemData, ITEM_NAME_LEN );
		inputItem( pXmlIdx, itemName, itemData );
	}
	
	//copyID
	if( singlePrintInfo->copyID != 0 ) {
		xmlGetSinglePrintInfoItemName( 5, itemName );
		convertUINT32toStr( singlePrintInfo->copyID, itemData, ITEM_NAME_LEN );
		inputItem( pXmlIdx, itemName, itemData );
	}
	#endif
	//date
	if( !checkValue(pCsj->datePrint,K_DPS_HEX_PARAVAL_MASK_False )
		&&pCsj->datePrint!=0
		&&singlePrintInfo->date[ 0 ] != '\0' ) {
		xmlGetSinglePrintInfoItemName( 6, itemName );
		xmlInputItem( pXmlIdx, itemName, singlePrintInfo->date );
	}
	
	//fileName
	if( !checkValue(pCsj->fileNamePrint,K_DPS_HEX_PARAVAL_MASK_False )
		&&pCsj->fileNamePrint!=0
		&&singlePrintInfo->fileName[ 0 ] != '\0' ) {
		xmlGetSinglePrintInfoItemName( 7, itemName );
		xmlInputItem( pXmlIdx, itemName, singlePrintInfo->fileName );
	}
	#if 0
	//filePath
	if( singlePrintInfo->filePath[ 0 ] != '\0' ) {
		xmlGetSinglePrintInfoItemName( 7, itemName );
		inputItem( pXmlIdx, itemName, singlePrintInfo->filePath );
	}
	#endif	
	//printInfo end tag
	xmlGetSinglePrintInfoItemName( 0, itemName );
	xmlInputItemTag( pXmlIdx, itemName, 1 );

	return *pXmlIdx - pXmlStart;
}

#if 0
UINT16 xmlConstructMultiPrintInfoItem( UINT32 dramAddr, UINT8** pXmlIdx, PDPS_PARA_ConfigStartJob pCsj ) 
{
	//UINT32 handles[DPS_PARA_MultiPrintHandleCount];
	//UINT8 i=0;
	//UINT8 itemName[ ITEM_NAME_LEN ];
	//UINT8 itemData[ ITEM_NAME_LEN ];
	UINT8* pXmlStart = *pXmlIdx;
	//DPS_PARA_ConfigPrintInfo cp;

	UINT16 i=0;
	UINT16 _index,_count,size;
	printf("MultiPrint\n");
	for(i=0;i<10;i++)
		printf("--%lx\n",((UINT32*)pCsj)[i]);
	dpsLoadConfigPrintInfoCount(_count);
	dpsLoadConfigPrintInfoIndex(_index);
	printf("--_count:%x\n",_count);
	printf("--_index:%x\n",_index);
	for(i=_index;i<_count;i++)
	{
		//if((i-_index)>=DPS_PARA_ConfigPrintInfoMaxCountPerPrint)
		//	break;
		//dpsLoadConfigPrintInfoItem((UINT8 xdata*)&cp, i);
		size=xmlConstructSinglePrintInfoItem( K_DPS_ADDR_ConfigPrintInfo + ((i* DPS_PARA_ConfigPrintInfo_Size_Min +4)>>1)
									, pXmlIdx
									, pCsj );
	}
	//if(i>=_count)
	//	dpsClearConfigPrintInfo();
	dpsClearConfigPrintInfo();
	printf("MultiPrint2:%x\n", size);
	printf("%s\n",G_ucStorData);
	#if 0
	sidcDramReadBytes((UINT8 xdata*)handles, dramAddr, DPS_PARA_MultiPrintHandleCount*4);

	while(handles[i])
	{
		 //printInfo start tag
			xmlGetSinglePrintInfoItemName( 0, itemName );
			xmlInputItemTag( pXmlIdx, itemName, 0 );

		//file ID
		xmlGetSinglePrintInfoItemName( 2, itemName );
				convertUINT32toStr( handles[i], itemData, ITEM_NAME_LEN );
				inputItem( pXmlIdx, itemName, itemData );

		//printInfo end tag
			xmlGetSinglePrintInfoItemName( 0, itemName );
			xmlInputItemTag( pXmlIdx, itemName, 1 );
		i++;
	}
	#endif
	return *pXmlIdx - pXmlStart;
}
#endif
#endif
