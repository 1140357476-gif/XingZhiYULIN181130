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
 * @file      xml_func.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __XML_FUNC_H__
#define __XML_FUNC_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "stdlib.h"
#include "string.h"

#include "hal_gprm.h"

#include "dps_res.h"
#include "dps_param.h"
#include "dps_struct.h"
#include "sp1k_dps_api.h"

#include "xml_api.h"

#include "sidc_api.h"
#include "usbopt.h"

#include "sidc_dbg.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ITEM_NAME_LEN 48
#define ITEM_DATA_LEN 48 

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _Xml_Item_Attr *PXml_Item_Attr;
typedef struct  _Xml_Item_Attr{
	UINT8 attrName[ ITEM_NAME_LEN ];
	UINT8 attrData[ ITEM_DATA_LEN ];
	PXml_Item_Attr next;
} Xml_Item_Attr;

//typedef struct _Xml_Item_with_Attr *pXml_Item_with_Attr;
typedef struct  {
	UINT8 itemName[ ITEM_NAME_LEN ];
	PXml_Item_Attr attr;
}Xml_Item_with_Attr;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 G_ucStorData[];
extern code UINT8 dpsXmlHeader[]; 
extern UINT8 G_ucStorData[];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//xml tree make
UINT16 xmlConfigPrintServiceTreeMake( PDPS_PARA_ConfigPrintService pCps );
//UINT16 xmlStartJobTreeMake( PDPS_PARA_ConfigStartJob pCsj  ,UINT8 single_Multi);
UINT16 xmlStartJobTreeMake(dps_data_t xdata* dpsData,UINT32 addr);
UINT16 xmlResultTreeMake( UINT8 type ,UINT32 param);
UINT16 xmlGetCapabilityTreeMake( UINT8 capType, UINT32 param ) ;
UINT16 xmlGetStatusTreeMake(UINT8 type) ;
UINT16 xmlAbortJobTreeMake(UINT32 value);
UINT16 xmlContinueJobTreeMake();


//xml tree parse
UINT8 xmlNotifyTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );
UINT8 xmlOutputTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );
UINT8 xmlConfigPrintServiceTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );
UINT8 xmlJobStatusTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );
UINT8 xmlDeviceStatusTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );
UINT8 xmlGetCapabilityTreeParse( UINT8* pXmlIdx, UINT16 xmlSize, UINT8 xdata* pData );

void xmlInputItemTag( UINT8** pXmlIdx, UINT8* itemName, UINT8 tagType );
void xmlInputItem( UINT8** pXmlIdx, UINT8* itemName, UINT8* itemData );
void xmlInputNullItem( UINT8** pXmlIdx, UINT8* itemName );
void xmlInputItemTagwithAttr( UINT8** pXmlIdx, Xml_Item_with_Attr* item );
void xmlInputNullItemwithAttr( UINT8** pXmlIdx, Xml_Item_with_Attr* item );

void xmlConvertUINT32toStr( UINT32 val, UINT8* buf, UINT16 bufSize );

UINT8 xmlSkipUntil( UINT8** pXmlIdx, UINT16* xmlSize, UINT8 ch );
UINT8 xmlSkip2Item( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name );
UINT8 xmlFindItemData( UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name, UINT8* itemData );
UINT8 xmlFindItemDataIdx( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* name );
UINT8 xmlHasItem( UINT8* pXmlIdx, UINT16 xmlSize, UINT8* name );
UINT8 xmlGetAttribute( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* attrName, UINT16 nameSize, UINT8* attrData, UINT16 dataSize );
UINT8 xmlAttrDataParse( UINT8* attrData, UINT16 xmlSize, UINT32* values );
UINT8 xmlIsResultLegal( UINT8** pXmlIdx, UINT16* xmlSize );

UINT8 xmlGetItemName( UINT8** pXmlIdx, UINT16* xmlSize, UINT8* itemName ) ;


UINT16 xmlConstructJobConfigItem( PDPS_PARA_ConfigStartJob pCsj, UINT8** pXmlIdx );
UINT16 xmlConstructSinglePrintInfoItem( DPS_PARA_ConfigPrintInfo* singlePrintInfo, UINT8** pXmlIdx, PDPS_PARA_ConfigStartJob pCsj );
UINT16 xmlConstructMultiPrintInfoItem( UINT32 dramAddr, UINT8** pXmlIdx, PDPS_PARA_ConfigStartJob pCsj ) ;

#endif  /* __TEMPLATE_HDR_H__ */

