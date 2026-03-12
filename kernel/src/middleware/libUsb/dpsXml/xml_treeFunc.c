/**************************************************************************
 *				 Copyright(c) 2010 by i-Catch Technology Co., Ltd.							*
 *																																				*
 *	This software is copyrighted by and is the property of i-Catch Tech-	*
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co., 	*
 *	Ltd. This software may only be used in accordance with the						*
 *	corresponding license agreement. Any unauthorized use, duplication, 	*
 *	distribution, or disclosure of this software is expressly forbidden.	*
 *																																				*
 *	This Copyright notice "M U S T" not be removed or modified without		*
 *	prior written consent of i-Catch Technology Co., Ltd. 								*
 *																																				*
 *	i-Catch Technology Co., Ltd. reserves the right to modify this				*
 *	software without notice.																							*
 *																																				*
 *	i-Catch Technology Co., Ltd.																					*
 *	19-1, Innovation First Road, Science-Based Industrial Park, 					*
 *	Hsin-Chu, Taiwan, R.O.C.																							*
 *																																				*
 *************************************************************************/
 
/**
 * @file			xml_treeFunc.c
 * @brief 		Brief
 * @author	 wangjg
 * @since 		2010-01-01
 * @date			2010-01-01
*/

/**************************************************************************
 *												H E A D E R 	F I L E S
 **************************************************************************/
#include "xml_func.h"
#if USBSIDC_OPTION == 2 
/**************************************************************************
 *													 C O N S T A N T S														*
 **************************************************************************/
 
/**************************************************************************
 *															M A C R O S 															*
 **************************************************************************/
 
/**************************************************************************
 *													D A T A 	 T Y P E S													*
 **************************************************************************/
 
/**************************************************************************
 *												 G L O B A L		D A T A 												*
 **************************************************************************/
 
/**************************************************************************
 *								 E X T E R N A L		R E F E R E N C E S 								*
 **************************************************************************/
 
/**************************************************************************
 *							 F U N C T I O N		D E C L A R A T I O N S 							*
 **************************************************************************/

/**********************************tree make function **********************************************/
/**
 * @fn				void xmlFormItemTag(UINT8* itemName, UINT8* tagName, UINT8 tagType)
 * @brief 		xmlFormItemTag
 * @param 		[in] itemName
 * @param 		[in] tagName
 * @param 		[in] tagType
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlFormItemTag( UINT8* itemName, UINT8* tagName, UINT8 tagType ) {
	UINT8 itemTag[ ITEM_NAME_LEN ];
	UINT8* itemTagIdx = itemTag;
	int idx = 0;
	for( ; idx < ITEM_NAME_LEN - 1; idx++ )
		itemTag[ idx ] = 0;
	
	*itemTagIdx = '<';
	itemTagIdx++;
	if( tagType == 1 ) { /*end tag*/
		*itemTagIdx = '/';
		itemTagIdx++;
	}
	strcpy( itemTagIdx, itemName );
	itemTagIdx += strlen( itemName );
	if( tagType == 2 ) { /*null item*/
		*itemTagIdx = '/';
		itemTagIdx++;
	}
	*itemTagIdx = '>';
	strcpy( tagName, itemTag );
}

/**
 * @fn				void xmlInputItemTag(UINT8** pXmlIdx, UINT8* itemName, UINT8 tagType)
 * @brief 		xmlInputItemTag
 * @param 		[in] pXmlIdx
 * @param 		[in] itemName
 * @param 		[in] tagType
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputItemTag( UINT8** pXmlIdx, UINT8* itemName, UINT8 tagType ) {
	UINT8 tagName[ ITEM_NAME_LEN ];
	int idx = 0;
	for( ; idx < ITEM_NAME_LEN ; idx++ )
		tagName[ idx ] = 0;
	
	xmlFormItemTag( itemName, tagName, tagType );
	strcpy( *pXmlIdx, tagName );
	*pXmlIdx += strlen( tagName );
}

/**
 * @fn				void xmlInputItemData(UINT8** pXmlIdx, UINT8* itemData)
 * @brief 		xmlInputItemData
 * @param 		[in] pXmlIdx
 * @param 		[in] itemData
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputItemData( UINT8** pXmlIdx, UINT8* itemData ) {
	strcpy( *pXmlIdx, itemData );
	*pXmlIdx += strlen( itemData );
}

/**
 * @fn				void xmlInputItem(UINT8** pXmlIdx, UINT8* itemName, UINT8* itemData)
 * @brief 		xmlInputItem
 * @param 		[in] pXmlIdx
 * @param 		[in] itemName
 * @param 		[in] itemData
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputItem( UINT8** pXmlIdx, UINT8* itemName, UINT8* itemData ) {
	xmlInputItemTag( pXmlIdx, itemName, 0 );
	xmlInputItemData( pXmlIdx, itemData );
	xmlInputItemTag( pXmlIdx, itemName, 1 );
}

/**
 * @fn				void xmlInputNullItem(UINT8** pXmlIdx, UINT8* itemName)
 * @brief 		xmlInputNullItem
 * @param 		[in] pXmlIdx
 * @param 		[in] itemName
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputNullItem( UINT8** pXmlIdx, UINT8* itemName ) {
	xmlInputItemTag( pXmlIdx, itemName, 2 );
}

/**
 * @fn		void xmlInputItemTagwithAttrNoEndTag(UINT8** pXmlIdx, Xml_Item_with_Attr* item)
 * @brief 		xmlInputItemTagwithAttrNoEndTag
 * @param 		[in] pXmlIdx
 * @param 		[in] item
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputItemTagwithAttrNoEndTag( UINT8** pXmlIdx, Xml_Item_with_Attr* item ) {
	Xml_Item_Attr* pCurAttr;
	**pXmlIdx = '<';
	( *pXmlIdx )++;
	strcpy( *pXmlIdx, item->itemName );
	*pXmlIdx += strlen( item->itemName );
	
	pCurAttr = item->attr;
	while( pCurAttr != NULL ) {
		**pXmlIdx = ' ';
		( *pXmlIdx )++;
		strcpy( *pXmlIdx, pCurAttr->attrName );
		*pXmlIdx += strlen( pCurAttr->attrName );
		**pXmlIdx = '=';
		( *pXmlIdx )++;
		**pXmlIdx = '"';
		( *pXmlIdx )++;
		strcpy( *pXmlIdx, pCurAttr->attrData );
		*pXmlIdx += strlen( pCurAttr->attrData );
		**pXmlIdx = '"';
		( *pXmlIdx )++;
		
		pCurAttr = pCurAttr->next;
	}
}

/**
 * @fn		void xmlInputItemTagwithAttr(UINT8** pXmlIdx, Xml_Item_with_Attr* item)
 * @brief 		xmlInputItemTagwithAttr
 * @param 		[in] pXmlIdx
 * @param 		[in] item
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputItemTagwithAttr( UINT8** pXmlIdx, Xml_Item_with_Attr* item ) {
	xmlInputItemTagwithAttrNoEndTag( pXmlIdx, item );
	**pXmlIdx = '>';
	( *pXmlIdx )++;
}

/**
 * @fn		void xmlInputNullItemwithAttr(UINT8** pXmlIdx, Xml_Item_with_Attr* item)
 * @brief 		xmlInputNullItemwithAttr
 * @param 		[in] pXmlIdx
 * @param 		[in] item
 * @retval		NONE
 * @author		wangjg
 * @since 		2010-5-12
*/
void xmlInputNullItemwithAttr( UINT8** pXmlIdx, Xml_Item_with_Attr* item ) {
	xmlInputItemTagwithAttrNoEndTag( pXmlIdx, item );
	**pXmlIdx = '/';
	( *pXmlIdx )++;
	**pXmlIdx = '>';
	( *pXmlIdx )++;
}
/***********************************tree parse function ***********************************/
/**
 * @fn		UINT8 xmlSkipUntil(UINT8** pXmlIdx, UINT16* xmlSize, UINT8 ch)
 * @brief 		xmlSkipUntil
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] ch
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlSkipUntil( UINT8** pXmlIdx, UINT16* xmlSize, UINT8 ch ) {
	while( **pXmlIdx != ch && *xmlSize > 0 ) {
		( *pXmlIdx )++;
		( *xmlSize )--;
	}
	if( *xmlSize <= 0 )
		return 1;
	else
		return 0;
}

/**
 * @fn		UINT8 xmlSkipUntilItemStartTag(UINT8** pXmlIdx, UINT16* xmlSize)
 * @brief 		xmlSkipUntilItemStartTag
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlSkipUntilItemStartTag( UINT8** pXmlIdx, UINT16* xmlSize ) {
	xmlSkipUntil( pXmlIdx, xmlSize, '<' );
	while( *( *pXmlIdx + 1 ) == '/' && *xmlSize > 0 ) {
		( *pXmlIdx )++;
		( *xmlSize )--;
		xmlSkipUntil( pXmlIdx, xmlSize, '<' );
	}
	if( *xmlSize <= 0 )
		return 1;
	else
		return 0;
}

/**
 * @fn		UINT8 xmlGetItemName(UINT8** pXmlIdx, UINT16* xmlSize, UINT8* itemName)
 * @brief 		xmlGetItemName
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] itemName
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlGetItemName( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* itemName ) {
	UINT8 i;
	
	if( xmlSkipUntilItemStartTag( pXmlIdx, xmlSize ) )
		return 1;
	/*skip the '<' at the beginning of item tag*/
	( *pXmlIdx )++;
	( *xmlSize )--;
	
	//clean itemName buf
	i = 0;	
	for( ; i < ITEM_NAME_LEN; i++ )
		itemName[ i ] = 0;
	
	while( ( **pXmlIdx != '>'  && **pXmlIdx != ' ' ) && *xmlSize > 0 ) {
		*itemName = **pXmlIdx;
		itemName++;
		( *pXmlIdx )++;
		( *xmlSize )--;
	}
	return 0;
}

/**
 * @fn		UINT8 xmlHasItem(UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name)
 * @brief 		xmlHasItem
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] name
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlHasItem( UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name ) {
	UINT8 result = 0; //1: has the item, 0: not
	UINT8 itemName[ ITEM_NAME_LEN ];
	
	while( xmlSize > 0 ) {
		if( xmlGetItemName( &pXmlIdx, &xmlSize, itemName ) )
			break;
		if( strncmp( name, itemName, strlen( name ) ) == 0 ) {
			result = 1;
			break;
		}
 }
	
	return result;
}

/**
 * @fn		UINT8 xmlSkipWhiteSpace(UINT8** pXmlIdx, UINT16* xmlSize)
 * @brief 		xmlSkipWhiteSpace
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlSkipWhiteSpace( UINT8** pXmlIdx, UINT16* xmlSize ) {
	while( *xmlSize > 0 ) {
		if( **pXmlIdx == ' ' || **pXmlIdx == '\n' || **pXmlIdx == '\t' || **pXmlIdx == 0x0a || **pXmlIdx == 0x0d ) { 
			( *pXmlIdx )++;
			( *xmlSize )--;
		} else {
			break;
		}
	}
	if( *xmlSize <= 0 )
		return 1;
	else
		return 0;
}

/**
 * @fn		UINT8 xmlGetAttribute(UINT8** pXmlIdx, UINT16* xmlSize, UINT8* attrName, UINT16 nameSize, UINT8* attrData, UINT16 dataSize)
 * @brief 		xmlGetAttribute
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] attrName
 * @param 		[in] nameSize
 * @param 		[in] attrData
 * @param 		[in] dataSize
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlGetAttribute( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* attrName, UINT16 nameSize, UINT8* attrData, UINT16 dataSize ) {
	UINT8 result = 1; //1: error, 0: success
	
	if( xmlSkipWhiteSpace( pXmlIdx, xmlSize ) )
		return result;
	while( **pXmlIdx != '=' && **pXmlIdx != '>' && *xmlSize > 0 ) {
		( *pXmlIdx )++;
		( *xmlSize )--;
	}

	if( *xmlSize <= 0 || **pXmlIdx == '>' )
		result = 1;
	else if( **pXmlIdx == '=' ) {
		result = 0;
		memset( attrName, 0, nameSize );
		memset( attrData, 0, dataSize );
		
		/*get attr name*/
		while( *( --( *pXmlIdx ) ) == ' ' )
			;
		while( *( --( *pXmlIdx ) ) != ' ' )
			;
		( *pXmlIdx )++;
		while( **pXmlIdx != '=' ) {
			*attrName++ = **pXmlIdx;
			( *pXmlIdx )++;
		}
		
		/*get attr data*/
		xmlSkipUntil( pXmlIdx, xmlSize, '"' );
		( *pXmlIdx )++;
		( *xmlSize )--;
		
		while( **pXmlIdx != '"' ) {
			*attrData++ = **pXmlIdx;
			( *pXmlIdx )++;
			( *xmlSize )--;
		}
	}
	return result;
}

/**
 * @fn		UINT8 xmlAttrDataParse(UINT8* attrData, UINT16 xmlSize, UINT32* values)
 * @brief 		xmlAttrDataParse
 * @param 		[in] attrData
 * @param 		[in] xmlSize
 * @param 		[in] values
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlAttrDataParse( UINT8* attrData, UINT16 xmlSize, UINT32* values ) {
	UINT8 idx = 0;
	UINT16 size;
	UINT8 value[ 20 ];
	UINT8* valueIdx;
	UINT8* pDataIdx = attrData;
	
	xmlSkipUntil( &pDataIdx, &xmlSize, '<' );
	size = pDataIdx - attrData;

	xmlSkipWhiteSpace( &attrData, &size );
	while( size > 0 ) {
		memset( value, 0, 20 );
		valueIdx = value;
		while( *attrData != ' ' && size > 0 ) {
			*valueIdx++ = *attrData++;
			size--;
		}
		values[ idx++ ] = strtoul( value, 0, 16 );
		if( idx > DPS_PARA_Capability_Elem_MaxCount )
			return idx;
		//printf( "%s 0x%lx\n", value, values[ idx - 1 ] );
		
		xmlSkipWhiteSpace( &attrData, &size );
	}
	//printf( "total Count: %bu\n", idx );
	return idx;
}

/**
 * @fn		UINT8 xmlIsResultLegal(UINT8** pXmlIdx, UINT16* xmlSize)
 * @brief 		xmlIsResultLegal
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlIsResultLegal( UINT8** pXmlIdx, UINT16* xmlSize ) {
	//return 0: false, 1: true 
	UINT8 itemName[ ITEM_NAME_LEN ], tagName[ 20 ];
	UINT32 val;
	UINT8* pItemName = itemName;
	
	//get item name and make sure it is "result"
	xmlGetItemName( pXmlIdx, xmlSize, itemName );
	xmlGetGeneralName( 3, tagName );
	if( strcmp( itemName, tagName ) != 0 )
		return 0;
	
	( *pXmlIdx )++;
	( *xmlSize )--;
	
	//get the return value & compare
	memset( itemName, 0, ITEM_NAME_LEN );
	while( **pXmlIdx != '<' && *xmlSize > 0 ) {
		*pItemName = **pXmlIdx;
		pItemName++;
		( *pXmlIdx )++;
		( *xmlSize )--;
	}
	if( strlen( itemName ) != 8 )
		return 0;
	val = strtoul( itemName, 0, 16 );
	if( val != 0x10000000 )
		return 0;
		
	//skip the item end tag
	while( **pXmlIdx != '>' && *xmlSize > 0 ) {
		( *pXmlIdx )++;
		( *xmlSize )--;
	}
	( *pXmlIdx )++;
	( *xmlSize )--;
		
	return 1;
}

/**
 * @fn		UINT8 xmlSkip2Item(UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name)
 * @brief 		xmlSkip2Item
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] name
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlSkip2Item( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name ) {
	UINT8 result = 1; //1: error, 0: success
	UINT8 itemName[ ITEM_NAME_LEN ];
	
	memset( itemName, 0, ITEM_NAME_LEN );
	
	while( xmlSize > 0 ) {
		xmlGetItemName( pXmlIdx, xmlSize, itemName );
		if( strncmp( name, itemName, strlen( name ) ) == 0 ) {
			result = 0;
			break;
		}
	}
	return result;
}

/**
 * @fn		UINT8 xmlFindItemDataIdx(UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name)
 * @brief 		xmlFindItemDataIdx
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] name
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlFindItemDataIdx( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name ) {
	UINT8 result = 1; //1: error, 0: success
	UINT8 itemName[ ITEM_NAME_LEN ];

	while( *xmlSize > 0 ) {
		xmlGetItemName( pXmlIdx, xmlSize, itemName );
		if( strncmp( name, itemName, strlen( name ) ) == 0 ) {
			result = 0;
			break;
		}
	}

	if( result == 0 ) {
		xmlSkipUntil( pXmlIdx, xmlSize, '>' );
		//skip '>'	of the start tag
		( *pXmlIdx )++;
		( *xmlSize )--;
	}

	return result;
}

/**
 * @fn		UINT8 xmlFindItemData(UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name, UINT8* itemData)
 * @brief 		xmlFindItemData
 * @param 		[in] pXmlIdx
 * @param 		[in] xmlSize
 * @param 		[in] name
 * @param 		[in] itemData
 * @retval		return = TRUE / FALSE
 * @author		wangjg
 * @since 		2010-5-12
*/
UINT8 xmlFindItemData( UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name, UINT8* itemData ) {
	UINT8 result = 1; //1: error, 0: success
	UINT8 itemName[ ITEM_NAME_LEN ];
	
	while( xmlSize > 0 ) {
		xmlGetItemName( &pXmlIdx, &xmlSize, itemName );
		if( strncmp( name, itemName, strlen( name ) ) == 0 ) {
			result = 0;
			break;
		}
	}
	
	if( result == 0 ) {
		xmlSkipUntil( &pXmlIdx, &xmlSize, '>' );
		//skip '>'	of the start tag
		pXmlIdx++;
		xmlSize--;
		
		//get item data
		while( *pXmlIdx != '<' && xmlSize > 0 ) {
			*itemData = *pXmlIdx;
			itemData++;
			pXmlIdx++;
			xmlSize--;
		}
		*itemData = '\0';	
	}
	//printf("itemData:%s\n",itemData);
	return result;
}

#endif
