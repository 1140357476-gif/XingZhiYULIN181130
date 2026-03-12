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
 * @file      xml_outputTreeParse.c
 * @brief     xml tree parse
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "xml_func.h"
#include "dbg_def.h"
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
 * @fn        UINT8 xmlOutputTreeParse(UINT8* pXmlIdx, UINT16 size, xdata* pData)
 * @brief     xmlOutputTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] size
 * @param     [in] pData
 * @retval    return = tree type
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlOutputTreeParse( UINT8* pXmlIdx, UINT16 size, UINT8 xdata* pData ) {
	UINT8 name[ ITEM_NAME_LEN ];
	pXmlIdx[size]=0;
	sidcPrintf("size:0x%x\nxml:\n%s\n",size,pXmlIdx);

	if( !xmlIsResultLegal( &pXmlIdx, &size ) )
		return DPS_ACTION_Err_Result;
			
	//Is this ConfigurePrintService?
	xmlGetCpsItemName( 0, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		if( xmlConfigPrintServiceTreeParse( pXmlIdx, size, pData ) == 0 )
			return DPS_ACTION_ConfigurePrintService;
	}
	
	//Is this getCapability?
	xmlGetCapabilityItemName( 0, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		return xmlGetCapabilityTreeParse( pXmlIdx, size, pData );
	}
	
	//Is this getJobStatus?
	xmlGetJobStatusItemName( 6, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		if( xmlJobStatusTreeParse( pXmlIdx, size, pData ) == 0 )
			return DPS_ACTION_GetJobStatus;
	}
	
	//is this getDeviceStatus?
	xmlGetDeviceStatusItemName( 8, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		if( xmlDeviceStatusTreeParse( pXmlIdx, size, pData ) == 0 )
			return DPS_ACTION_GetDeviceStatus;
	}
	
	//is this abourtJob?
	xmlGetStatusTypeName( 4, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		return DPS_ACTION_AbortJob;
	}
	
	//is this continueJob?
	xmlGetStatusTypeName( 6, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		return DPS_ACTION_ContinueJob;
	}
//wangjg++
	//is this startJob
	xmlGetJobConfigItemName( 0, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		return DPS_ACTION_StartJob;
	}
	sidcPrintfErr("is Null!\n");
	return DPS_ACTION_Err_Result;
}

/**
 * @fn        UINT8 xmlConfigPrintServiceTreeParse(UINT8* pXmlIdx, UINT16 xmlSize, xdata* pData)
 * @brief     xmlConfigPrintServiceTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] xmlSize
 * @param     [in] pData
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlConfigPrintServiceTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData ) {
	UINT8 result;
	
	UINT8 name[ ITEM_NAME_LEN ];
	UINT8 itemData[ ITEM_DATA_LEN ];
	PDPS_PARA_ConfigPrintService cps = ( PDPS_PARA_ConfigPrintService )pData;
	
	//xmlSize = strlen( pXmlIdx );
	//size = strlen( dpsXmlHeader );
	//skip xml header
	//pXmlIdx += size;
	//xmlSize -= size;
	//printServiceAvaiable
	xmlGetCpsItemName( 1, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		UINT32 ul = strtoul( itemData, 0, 16 );
		cps->printServiceAvailable = ul;
	}
 
	//dpsVersions
	xmlGetCpsItemName( 2, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( cps->dpsVersions, itemData );
		
	//vendorName
	xmlGetCpsItemName( 3, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( cps->vendorName, itemData );
		
	//vendorSpecificVersion
	xmlGetCpsItemName( 4, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( cps->vendorSpecVer, itemData );
		
	//productName
	xmlGetCpsItemName( 5, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( cps->productName, itemData );
		
	//serialNo
	xmlGetCpsItemName( 6, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( cps->serialNo, itemData );
		
	//sram2Dram
	//sidcDramWriteBytes( ( UINT8* )&cps , K_DPS_ADDR_ConfigPrintService, DPS_PARA_ConfigPrintService_Size );

	//print
	xmlPrintfInfo( "%d\n", cps->printServiceAvailable );
	xmlPrintfInfo( "cps.dpsVersions = %s\n", cps->dpsVersions );
	xmlPrintfInfo( "cps.vendorName = %s\n", cps->vendorName );
	xmlPrintfInfo( "cps.vendorSpecVer = %s\n", cps->vendorSpecVer );
	xmlPrintfInfo( "cps.productName = %s\n", cps->productName );
	xmlPrintfInfo( "cps.serialNo = %s\n", cps->serialNo );
	return 0;
}

/**
 * @fn        UINT8 xmlGetCapabilityTreeParse(UINT8* pXmlIdx, UINT16 xmlSize, xdata* pData)
 * @brief     xmlGetCapabilityTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] xmlSize
 * @param     [in] pData
 * @retval    return = tree type
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlGetCapabilityTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData ) {
	UINT8  result, ret;
	UINT32 val;
	UINT8* pOriIdx = pXmlIdx;
	
	UINT8 name[ ITEM_NAME_LEN ], attrName[ 20 ], attrCmpName[ ITEM_NAME_LEN ];
	UINT8 itemData[ ITEM_DATA_LEN ];
	PDPS_PARA_Capability_Elem pc = ( PDPS_PARA_Capability_Elem )pData;
	
	UINT16 size = xmlSize;
	
	xmlGetCapabilityItemName( 1, name );
	xmlSkip2Item( &pXmlIdx, &xmlSize, name );
	xmlGetItemName( &pXmlIdx, &xmlSize, name );
	result = xmlGetCapabilityItemName( 0xff, name );
	
	if( result == 0xff )
		return DPS_ACTION_Err_Item;
	
	result += ( DPS_ACTION_GetCapability - 2 );
	
	if( !xmlGetAttribute( &pXmlIdx, &xmlSize, attrName, 20, itemData, ITEM_DATA_LEN ) ) {
	//should compare attrName and attrData here
	xmlGetCapabilityItemName( 12, attrCmpName );
	if( strcmp( attrName, attrCmpName ) == 0 ) {
		val = strtoul( itemData, 0, 16 );
		if( val & 0xff000000 != 0x51000000 )
			return DPS_ACTION_Err_Item;
	}
	}
	ret = xmlFindItemDataIdx( &pOriIdx, &size, name );
	if( ret == 0 ){
		pc->count= (UINT16)xmlAttrDataParse( pOriIdx, size, pc->values );
	}

	//CPrintf2("pc->count:%u,result:0x%bx\n",pc->count,result);
	//dpsSaveGetCapability_Item(result,pc);
	return result;
}

#endif
