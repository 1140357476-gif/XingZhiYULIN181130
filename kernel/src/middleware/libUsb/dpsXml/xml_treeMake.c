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
 * @file      xml_treeMake.c
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "xml_func.h"
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
/**
 * @fn        UINT16 xmlTreeMake(xdata* pData, UINT32 DramAddr, UINT8 treeType)
 * @brief     xmlTreeMake
 * @param     [in] pData
 * @param     [in] DramAddr
 * @param     [in] treeType
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlTreeMake( void xdata* pData, UINT32 DramAddr, UINT8 treeType )
{
	UINT16 outSize = 0;
	sidcPrintfFunc("dpsXmlTreeMake:%bx\n",treeType);
	dbgWdtFeed(-1);	
	switch( treeType ) {
	case DPS_ACTION_ConfigurePrintService:
		 outSize = xmlConfigPrintServiceTreeMake( ( PDPS_PARA_ConfigPrintService )pData );
		break;
	//case DPS_ACTION_GetCapability:
	case DPS_ACTION_GetCapability_Qualities:
	case DPS_ACTION_GetCapability_PaperSizes:
	case DPS_ACTION_GetCapability_PaperTypes:
	case DPS_ACTION_GetCapability_FileTypes:
	case DPS_ACTION_GetCapability_DatePrints	:
	case DPS_ACTION_GetCapability_FileNamePrints	:
	case DPS_ACTION_GetCapability_ImageOptimizes	:
	case DPS_ACTION_GetCapability_Layouts	:
	case DPS_ACTION_GetCapability_FixedSizes	:
	case DPS_ACTION_GetCapability_Croppings	:
		outSize = xmlGetCapabilityTreeMake( treeType,*((UINT32 xdata*)pData) );
		break;
	case DPS_ACTION_StartJob://DPS_ACTION_JobStart_MultiPrint:
		//outSize = xmlStartJobTreeMake( ( PDPS_PARA_ConfigStartJob )pData,treeType);
		outSize = xmlStartJobTreeMake(pData,DramAddr);
		return outSize;
		//break;	
	case DPS_ACTION_GetDeviceStatus:
		 outSize = xmlGetStatusTreeMake(1);
		break;
	 case DPS_ACTION_GetJobStatus:
		outSize = xmlGetStatusTreeMake(0);
		break;
	case DPS_ACTION_NotifyDeviceStatus:
			 outSize = xmlResultTreeMake(3,*((UINT32 xdata*)pData));
		break;
	case DPS_ACTION_NotifyJobStatus:
		outSize = xmlResultTreeMake(2,*((UINT32 xdata*)pData));
		break;
	case DPS_ACTION_AbortJob:
	outSize = xmlAbortJobTreeMake(((UINT32*)pData)[0]);
		break;
	case DPS_ACTION_ContinueJob:
		outSize = xmlContinueJobTreeMake();
		break;
	case DPS_ACTION_Err_Item:
		break;
	case DPS_ACTION_Err_Result:
	break;
	case DPS_ACTION_Err_Notify:
		outSize = xmlResultTreeMake(0,*((UINT32 xdata*)pData));
	break;
	default:
		break;
	
	}
	G_ucStorData[outSize]=0;
	dpsPrintfXml("%s\n",G_ucStorData);
	dpsPrintfXml("outSize:%x\n",outSize);
	dpsDramWriteBytes( G_ucStorData, DramAddr, outSize );
	//memcpy(ptDst,G_ucStorData,outSize);
	return outSize;
}

/**
 * @fn        UINT16 xmlResultTreeMake(UINT8 type, UINT32 param)
 * @brief     xmlResultTreeMake
 * @param     [in] type
 * @param     [in] param
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlResultTreeMake( UINT8 type ,UINT32 param)
{
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];
	UINT8 itemData[ ITEM_NAME_LEN ];
	//UINT8 type = *jobType;
	
	xmlGetGeneralName( 0,pXmlIdx );
	pXmlIdx += strlen( pXmlIdx );

	xmlGetGeneralName( 2, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	//printf("++param:%lx\n",param);
	xmlGetGeneralName( 3, itemName );
	sprintf(itemData,"%08lx",param);
	xmlInputItem( &pXmlIdx, itemName, itemData );

	if(type)
	{
		xmlGetStatusTypeName( type , itemName );
		xmlInputNullItem( &pXmlIdx, itemName );
	}
	
	xmlGetGeneralName( 2, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}

/**
 * @fn        UINT16 xmlGetCapabilityTreeMake(UINT8 capType, UINT32 attrData)
 * @brief     xmlGetCapabilityTreeMake
 * @param     [in] capType
 * @param     [in] attrData
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlGetCapabilityTreeMake( UINT8 capType, UINT32 attrData ) 
{
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];
	//UINT8 capType = *capData;
	//UINT32 attrData = *( UINT32* )( capData + 1 );
	
	xmlGetGeneralName( 0,pXmlIdx );
	pXmlIdx += strlen( pXmlIdx );
	
	//input start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );
	//getCapability start tag
	xmlGetCapabilityItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );
	//capability start tag
	xmlGetCapabilityItemName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );
	
	//which capability you want to get?
 
	xmlGetCapabilityItemName( (capType & DPS_ACTION_MASK_) +2, itemName );

	sidcPrintf("capType:%bx\n",capType);
	sidcPrintf("name:%s\n",itemName);

	if( /*attrData != 0 &&*/
		( (capType ==DPS_ACTION_GetCapability_PaperTypes )
			|| (capType == DPS_ACTION_GetCapability_Layouts )
		) 
		) 
	{
		Xml_Item_with_Attr item;
		Xml_Item_Attr attr;
		UINT8 dataBuf[20];
		//sidcPrintf("paperSize:%lx\n",attrData);
		if(attrData == 0 || (attrData&0xff000000)!=K_DPS_HEX_PARAVAL_PaperSize_Default)
			attrData = K_DPS_HEX_PARAVAL_PaperSize_Default;
		//sidcPrintf("paperSize:%lx\n",attrData);
		strcpy( item.itemName, itemName );
	
		xmlGetCapabilityItemName( 12, itemName );
		strcpy( attr.attrName, itemName );
		xmlConvertUINT32toStr( attrData, dataBuf ,20 );
		//sidcPrintf("paperSize:%lx-%s\n",attrData,dataBuf);
		strcpy( attr.attrData, dataBuf );
		attr.next = NULL;
		
		item.attr = &attr;
		xmlInputNullItemwithAttr( &pXmlIdx, &item );
	} else {
		xmlInputNullItem( &pXmlIdx, itemName );
	}
	 //capability end tag
	xmlGetCapabilityItemName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
 
	//getCapability end tag
	xmlGetCapabilityItemName( 0, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

 //input end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}

/**
 * @fn        UINT16 xmlConfigPrintServiceTreeMake(tService pCps)
 * @brief     xmlConfigPrintServiceTreeMake
 * @param     [in] pCps
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlConfigPrintServiceTreeMake( PDPS_PARA_ConfigPrintService pCps ) 
{
	UINT8* pXmlCps = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];
	
	xmlGetGeneralName( 0,pXmlCps );
	pXmlCps += strlen( pXmlCps );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlCps, itemName, 0 );
	
	// configurePritService  start tag
	xmlGetCpsItemName( 0, itemName );
	xmlInputItemTag( &pXmlCps, itemName, 0 );
	
	//dpsVersions
	xmlGetCpsItemName( 2, itemName );
	xmlInputItem( &pXmlCps, itemName, pCps->dpsVersions );
	
	//vendorName
	xmlGetCpsItemName( 3, itemName );
	xmlInputItem( &pXmlCps, itemName, pCps->vendorName );

	//vendorSpeicificVersion
	xmlGetCpsItemName( 4, itemName );
	xmlInputItem( &pXmlCps, itemName, pCps->vendorSpecVer );
	
	//pruductName
	xmlGetCpsItemName( 5, itemName );
	xmlInputItem( &pXmlCps, itemName, pCps->productName );

	//serialNo
	xmlGetCpsItemName( 6, itemName );
	xmlInputItem( &pXmlCps, itemName, pCps->serialNo );
	
	//configurePritService end tag
	xmlGetCpsItemName( 0, itemName );
	xmlInputItemTag( &pXmlCps, itemName, 1 );
	
	//input end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlCps, itemName, 1 );
	
	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlCps, itemName, 1 );
	
	return pXmlCps - G_ucStorData;
}

/**
 * @fn        UINT16 xmlGetStatusTreeMake(UINT8 type)
 * @brief     xmlGetStatusTreeMake
 * @param     [in] type
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlGetStatusTreeMake(UINT8 type)
{
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];

	xmlGetGeneralName( 0, pXmlIdx);
	pXmlIdx += strlen( G_ucStorData );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	xmlGetStatusTypeName(type, itemName);
	xmlInputNullItem( &pXmlIdx, itemName );
	 
	//input  end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}

/**
 * @fn        UINT16 xmlAbortJobTreeMake(UINT32 value)
 * @brief     xmlAbortJobTreeMake
 * @param     [in] value
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlAbortJobTreeMake(UINT32 value)
{
		UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];
	UINT8 itemData[9];

	xmlGetGeneralName( 0, pXmlIdx);
	pXmlIdx += strlen( G_ucStorData );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	//abortJob start
	xmlGetStatusTypeName(4, itemName);
	 xmlInputItemTag( &pXmlIdx, itemName ,0);

	sprintf(itemData,"%lx\0",value);
	xmlGetStatusTypeName(5, itemName);
	xmlInputItem( &pXmlIdx, itemName, itemData );
		//abortJob end
	xmlGetStatusTypeName(4, itemName);
	 xmlInputItemTag( &pXmlIdx, itemName ,1 );
	 
	//input  end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}

/**
 * @fn        UINT16 xmlContinueJobTreeMake(void)
 * @brief     xmlContinueJobTreeMake
 * @param     NONE
 * @retval    return = size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 xmlContinueJobTreeMake() 
{
	UINT8* pXmlIdx = G_ucStorData;
	UINT8 itemName[ ITEM_NAME_LEN ];

	xmlGetGeneralName( 0, pXmlIdx);
	pXmlIdx += strlen( G_ucStorData );
	
	//input  start tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 0 );

	xmlGetStatusTypeName(6, itemName);
	xmlInputNullItem( &pXmlIdx, itemName );
	 
	//input  end tag
	xmlGetGeneralName( 1, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );

	//dps end tag
	xmlGetGeneralName( 4, itemName );
	xmlInputItemTag( &pXmlIdx, itemName, 1 );
	
	return pXmlIdx - G_ucStorData;
}
#endif
