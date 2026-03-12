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
 * @file      xml_notifyTreeParse.c
 * @brief     xml notify tree pares
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "xml_func.h"
#include "sp1k_pb_api.h"
#include "dbg_mem.h"
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
 * @fn        UINT8 xmlNotifyTreeParse(UINT8* pXmlIdx, UINT16 size, xdata* pData)
 * @brief     xmlNotifyTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] size
 * @param     [in] pData
 * @retval    return = tree type
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlNotifyTreeParse( UINT8* pXmlIdx, UINT16 size, UINT8 xdata* pData ) {
	UINT8 name[ ITEM_NAME_LEN ];
	UINT8 result;
	
	//Is this NotifyJobStatus tree?
	xmlGetJobStatusItemName( 0, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		if( ( result = xmlJobStatusTreeParse( pXmlIdx, size, pData ) ) == 0 )
			return DPS_ACTION_NotifyJobStatus;
		else
			return result;
	}
	
	//Is this NotifyDeviceStatus tree:
	xmlGetDeviceStatusItemName( 0, name );
	if( xmlHasItem( pXmlIdx, size, name ) ) {
		if( ( result = xmlDeviceStatusTreeParse( pXmlIdx, size, pData ) ) == 0 )
			return DPS_ACTION_NotifyDeviceStatus;
		else
			return result;
	}
	
	*( UINT32* )pData = 0x10030000; //not recognized
	return DPS_ACTION_Err_Notify;
}

/**
 * @fn        UINT8 xmlConvertStrtoValidUint32(UINT8* str, UINT32 begin, UINT32 end, UINT32* val)
 * @brief     xmlConvertStrtoValidUint32
 * @param     [in] str
 * @param     [in] begin
 * @param     [in] end
 * @param     [in] val
 * @retval    return = TRUE/FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlConvertStrtoValidUint32( UINT8* str, UINT32 begin, UINT32 end, UINT32* val ) {
	const UINT32 notSupported = 0x10020000;
	UINT32 tmpVal;
	
	if( strlen( str ) != 8 ) {
		*val = notSupported;
		return 1;
	}
	
	tmpVal = strtoul( str, 0, 16 );
	if( tmpVal & 0xffff != 0 ) {
		*val = notSupported;
		return 1;
	}
	
	*val = tmpVal;
	begin = ( begin >> 0x10 ) & 0xffff;
	end = ( end >> 0x10 ) & 0xffff;
	tmpVal = ( tmpVal >> 0x10 ) & 0xffff;
	if( tmpVal < begin || tmpVal > end ) {
		*val = notSupported;
		return 1;
	}

	return 0;
}
/**
 * @fn        UINT8 xmlJobStatusTreeParse(UINT8* pXmlIdx, UINT16 xmlSize, xdata* pData)
 * @brief     xmlJobStatusTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] xmlSize
 * @param     [in] pData
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlJobStatusTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData ) {
	UINT8 result;
	
	UINT8 name[ ITEM_NAME_LEN ];
	UINT8 itemData[ ITEM_DATA_LEN ];
	PDPS_PARA_JobStatus js = ( PDPS_PARA_JobStatus )pData;
	memset( pData, 0, sizeof( DPS_PARA_JobStatus ) );
	
	//find prtPID
	xmlGetJobStatusItemName( 1, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( js->prtPID, itemData );
	
	//find filePath
	xmlGetJobStatusItemName( 2, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( js->filePath, itemData );
		
	//find copyID
	xmlGetJobStatusItemName( 3, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( js->copyID, itemData );
		
	//find progress
	xmlGetJobStatusItemName( 4, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( js->progress, itemData );
		
	//find imagesPrinted
	xmlGetJobStatusItemName( 5, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 )
		strcpy( js->imagesPrinted, itemData );
		
	//print result
	xmlPrintfInfo( "Output NotifyJobStatus tree parse result:\n" );
	xmlPrintfInfo( "js->prtPID: %s\n", js->prtPID );
	xmlPrintfInfo( "js->filePath: %s\n", js->filePath );
	xmlPrintfInfo( "js->copyID: %s\n", js->copyID );
	xmlPrintfInfo( "js->progress: %s\n", js->progress );
	xmlPrintfInfo( "js->imagesPrinted: %s\n", js->imagesPrinted );
	//end
		
	return 0;
}

/**
 * @fn        UINT8 xmlDeviceStatusTreeParse(UINT8* pXmlIdx, UINT16 xmlSize, xdata* pData)
 * @brief     xmlDeviceStatusTreeParse
 * @param     [in] pXmlIdx
 * @param     [in] xmlSize
 * @param     [in] pData
 * @retval    return = Tree type
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlDeviceStatusTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData ) {
	UINT8 result;
	UINT32 val;
	
	UINT8 name[ ITEM_NAME_LEN ];
	UINT8 itemData[ ITEM_DATA_LEN ];
	PDPS_PARA_DeviceStatus ds = ( PDPS_PARA_DeviceStatus )pData;
	memset( pData, 0, sizeof( DPS_PARA_DeviceStatus ) );
 
	//find dpsPrintServiceStatus;
	xmlGetDeviceStatusItemName( 1, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		//val = strtoul( itemData, 0, 16 );
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing, K_DPS_HEX_PARAVAL_PrintSrvStatus_Paused, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->printServiceStatus = val;
	}

	//find jobEndReason
	xmlGetDeviceStatusItemName( 2, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_JobEndReason_JobNotEnded, K_DPS_HEX_PARAVAL_JobEndReason_OtherReason, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->jobEndReason = val;
	}

	//find errorStatus;
	xmlGetDeviceStatusItemName( 3, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_ErrorStatus_NoError, K_DPS_HEX_PARAVAL_ErrorStatus_FatalError, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->errorStatus = val;
	}

	//find errorReason
	xmlGetDeviceStatusItemName( 4, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_ErrorReason_NoReason, K_DPS_HEX_PARAVAL_ErrorReason_File, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->errorReason = val;
	}

	//find disconnectEnable
	xmlGetDeviceStatusItemName( 5, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_DisconEnable_False, K_DPS_HEX_PARAVAL_DisconEnable_True, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->disconnectEnable = val;
	}

	//find capabilityChanged
	xmlGetDeviceStatusItemName( 6, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_CapChanged_False, K_DPS_HEX_PARAVAL_CapChanged_True, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->capabilityChanged = val;
	}

	//find newJobOK
	xmlGetDeviceStatusItemName( 7, name );
	result = xmlFindItemData( pXmlIdx, xmlSize, name, itemData );
	if( result == 0 ) {
		if( xmlConvertStrtoValidUint32( itemData, K_DPS_HEX_PARAVAL_NewJobOk_False, K_DPS_HEX_PARAVAL_NewJobOk_True, &val ) ) {
			*( UINT32* )pData = val;
			return DPS_ACTION_Err_Notify;
		}
		ds->newJobOK = val;
	}

	//print result
	xmlPrintfInfo( "Output NotifyDeviceStatus tree parse result:\n" );
	xmlPrintfInfo( "ds->printServiceStatus: %lx\n", ds->printServiceStatus );
	xmlPrintfInfo( "ds->jobEndReason: %lx\n", ds->jobEndReason );
	xmlPrintfInfo( "ds->errorStatus: %lx\n", ds->errorStatus );
	xmlPrintfInfo( "ds->errorReason: %lx\n", ds->errorReason );
	xmlPrintfInfo( "ds->disconnectEnalbe: %lx\n", ds->disconnectEnable );
	xmlPrintfInfo( "ds->capabilityChanged: %lx\n", ds->capabilityChanged );
	xmlPrintfInfo( "ds->newJobOK: %lx\n", ds->newJobOK );
	//end

	return 0;
}

#endif
