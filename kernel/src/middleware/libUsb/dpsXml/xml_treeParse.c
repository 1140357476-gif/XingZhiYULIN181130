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
 * @file      xml_treeParse.c
 * @brief     xml tree parse
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
 * @fn        UINT8 xmlTreeParse(xdata* pData, UINT32 DramAddr, UINT16 size)
 * @brief     xmlTreeParse
 * @param     [in] pData
 * @param     [in] DramAddr
 * @param     [in] size
 * @retval    return = tree type
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 xmlTreeParse( UINT8 xdata* pData, /*UINT32 DramAddr*/UINT8 xdata* ptSrc, UINT16 size ){
	UINT8 name[ ITEM_NAME_LEN ];
	//UINT8 headerSize;
	UINT8* pXmlIdx = ptSrc;//G_ucStorData;
	UINT8 tagName[ 20 ];

	memset(pData,0,DPS_PARA_Max_Size);
	//copy xml content from dram 2 sram first
	//dpsDramReadBytes( pXmlIdx, DramAddr, size );
	//pXmlIdx[size]=0;
	//memcpy(pXmlIdx,ptSrc,size);

	ptSrc[size]=0;
	dpsPrintfXml("parser:%s\n",ptSrc);

	xmlGetGeneralName(4, tagName );
	//skip xml header
	xmlSkip2Item( &pXmlIdx, &size, tagName );
	xmlSkipUntil( &pXmlIdx, &size, '>' );
	//pXmlIdx += headerSize;
	//size -= headerSize;
	
	//is notify status or other type xml? notify has input item
	xmlGetGeneralName( 1, tagName );
	xmlGetItemName( &pXmlIdx, &size, name );
	pXmlIdx++;
	size--;
	if( strcmp( tagName, name ) == 0 ) {
		return xmlNotifyTreeParse( pXmlIdx, size, pData );
	} else {
		 return xmlOutputTreeParse( pXmlIdx, size, pData );
	}
}
#endif
