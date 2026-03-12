/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  usb_init.c
 * @brief	  USB initial functions
 * @author	  Allen
 * @since	  2010-01-01
 * @date	  2010-01-01
*/
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "sp1k_rsvblk_api.h"
#include "hal.h" 
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#define USB_MODE_PCCAM 1
#define USB_MODE_MSDC  2
#define USB_MODE_SIDC  3 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT8 endLine[] = {0x0d,0x0a};
UINT8 prixtable[14] ={13,20,19,19,21,16,17,14,15,16,17,10,19,17};

/* instead of "G_CameraOptions.Storage" */
UINT8 xdata gUsbMode; /* 0x00:NONE 0x01: MSDC 0x02: SIDC */

UINT8 xdata G_ucPktStorIntrf; //current	packet belongs to which	storage	interface

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern xdata	UINT8	K_DeviceDescriptor[ 0x12 ];
extern xdata	UINT8	K_MSDC_DeviceDescriptor[ 0x12 ];
extern xdata	UINT8	G_ucStorData[];

extern xdata	UINT8	K_SIDC_DeviceDescriptor[ 0x12 ];
extern xdata	UINT8	K_ManufacturerString[ 0x24 ];
extern xdata	UINT8	K_ProductString[ 0x2a ];
extern xdata	UINT8	K_SerialNumberString[ 0x1e ];
extern xdata	UINT16	K_PIMADATA_Manufacturer[ 0x10 ];
extern xdata	UINT16	K_PIMADATA_Model[ 0x18 ];
extern xdata	UINT16	K_PIMADATA_DeviceVersion[ 0x08 ];
extern xdata	UINT16	K_PIMADATA_SerialNumber[ 0x08 ];
extern xdata	UINT16	K_PIMADATA_VendorExtensionDesc[ 0x10 ];
extern xdata	 UINT8	 G_ScsiVendorInformation[ 0x0a ];
extern xdata	 UINT8	 G_ScsiProductIdentification[ 0x13 ];
extern xdata	 UINT8	 G_ScsiProductRevisionLevel[ 0x04 ]; 
//extern xdata	 UINT8	 K_ConfigurationDescriptor_VS_FORMAT_USB20[ 0x131];
extern xdata UINT8 G_ucMSDC_MLUN;
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
UINT8 usbStrElemParse( UINT8** resIdx, UINT32* resSize, UINT8* strName, UINT8* strVal );
UINT8 usbStrSet( UINT32 pBuf, UINT32 resSize );
UINT8  usbStrToNumConv(UINT8 *ptr);
UINT16 usbStr2Num( UINT8* str );
UINT32 usbVidSet(UINT16 vid);
UINT32 usbPidSet(UINT32 mode,UINT16 pid);
UINT8 UsbStrNameGet( UINT8 idx, UINT8* strName );
UINT32 usbStringSet( UINT32 type, UINT8* pStrSrc, UINT32 lenSrc );
UINT32 usbMsdcStringSet( UINT32 type, UINT8* pStrSrc, UINT32 lenSrc );
//UINT32 usbDefaultsizeSet(UINT8* pStrSrc);
//-----------------------------------------------------------------------------
//sp1kUsbStrSet
//-----------------------------------------------------------------------------
//#if TAG_TODO/* mark for 1628 FPGA */
/**
 * @fn		  UINT8 sp1kUsbStrSet(UINT32 resIdx)
 * @brief	  Set USB string from resource file
 * @param	  [in] resIdx
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  allenlin
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 sp1kUsbStrSet( UINT32 resIdx ) 
{	
	UINT32 resSize;

	sp1kNandRsvSizeGet( resIdx, NULL, &resSize );
	sp1kNandRsvRead( resIdx, (K_SDRAM_CommonEndAddr << 1) );
	return usbStrSet( K_SDRAM_CommonEndAddr, resSize );
}
//#endif

/**
 * @fn		  UINT8 usbStrSet(UINT32 pBuf, UINT32 resSize)
 * @brief	  usbStrSet
 * @param	  [in] pBuf
 * @param	  [in] resSize
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  allenlin
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 usbStrSet( UINT32 pBuf, UINT32 resSize ) 
{
	UINT8 ret = 0;
	UINT8* pCurr;
	xdata UINT8 xdata strName[ 40 ];
	xdata UINT8 xdata strVal[ 64 ];
	UINT16 pid, vid;
	UINT8 idx;
	UINT16 dmaSize = ( resSize <= 512 ) ? 512 : 1024;

	/* copy usbstring.txt to high bank sram */
	HAL_DmaSetDramAddr(pBuf);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(0, 1, dmaSize, 0, 0);
	HAL_CpuSramModeSet( 0x00 );

	pCurr = G_ucStorData;
	while( resSize > 0 ) {
		usbStrElemParse( &pCurr, &resSize, strName, strVal );
		idx = UsbStrNameGet( 0xFF, strName );
		switch( idx ) {
		case 0x00:
			vid = usbStr2Num( strVal );
			usbVidSet( vid );
			break;
		case 0x01:
			pid = usbStr2Num( strVal );
			usbPidSet( USB_MODE_PCCAM, pid );
			break;
		case 0x02:
			pid = usbStr2Num( strVal );
			usbPidSet( USB_MODE_MSDC, pid );
			break;
		case 0x03:
			pid = usbStr2Num( strVal );
			usbPidSet( USB_MODE_SIDC, pid );
			break;
		case 0x04:
		case 0x05:
		case 0x06:
			usbStringSet( idx, strVal, strlen( strVal ) );		
			break;
		case 0x07:
		case 0x08:
		case 0x09:
			usbMsdcStringSet( idx, strVal, strlen( strVal ) );
			break;
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		case 0x0e:
			usbStringSet( idx, strVal, strlen( strVal ) );
           //   case 0x0f:
            //         usbDefaultsizeSet(strVal);
			break;
		}
	}
	return SUCCESS;
}

/**
 * @fn		  UINT8 getUsbStrName(UINT8 idx, UINT8* strName)
 * @brief	  UsbStrNameGet
 * @param	  [in] idx
 * @param	  [in] strName
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 UsbStrNameGet( UINT8 idx, UINT8* strName ) 
{
	code UINT8 usbVendorId[] = "USB_VENDOR_ID";  // 0x00
	code UINT8 usbPccamProductId[] = "USB_PCCAM_PRODUCT_ID";  // 0x01
	code UINT8 usbMsdcProductId[] = "USB_MSDC_PRODUCT_ID";	// 0x02
	code UINT8 usbSidcProductId[] = "USB_SIDC_PRODUCT_ID";	// 0x03
	code UINT8 usbManufactureName[] = "USB_MANUFACTURER_NAME";	// 0x04
	code UINT8 usbProductName[] = "USB_PRODUCT_NAME";  // 0x05
	code UINT8 usbSerialNumber[] = "USB_SERIAL_NUMBER"; // 0x06
	code UINT8 msdcVendorId[] = "MSDC_VENDOR_ID";  // 0x07
	code UINT8 msdcProductId[] = "MSDC_PRODUCT_ID";  // 0x08
	code UINT8 msdcRevisionId[] = "MSDC_REVISION_ID"; // 0x09
	code UINT8 sidcManufacture[] = "SIDC_MANUFACTURER";  // 0x0a
	code UINT8 sidcModel[] = "SIDC_MODEL";	// 0x0b
	code UINT8 sidcDeviceVersion[] = "SIDC_DEVICE_VERSION";  // 0x0c
	code UINT8 sidcSerialNumber[] = "SIDC_SERIALNUMBER"; // 0x0d
	code UINT8 sidcVendorExtDesc[] = "SIDC_VENDOREXT_DESC"; // 0x0e
	code UINT8 usbPccamDefaultSize[]  = "USB_PCCAM_DEFSIZE";  //0x0f

	code UINT8* list[] = { 
		usbVendorId,
		usbPccamProductId,
		usbMsdcProductId,
		usbSidcProductId,
		usbManufactureName,
		usbProductName,
		usbSerialNumber,
		msdcVendorId,
		msdcProductId,
		msdcRevisionId,
		sidcManufacture,
		sidcModel,
		sidcDeviceVersion,
		sidcSerialNumber,
		sidcVendorExtDesc,
		usbPccamDefaultSize};
		
	UINT8 listCnt = sizeof( list ) / sizeof( UINT8* );
	if( idx == 0xFF ) {
		UINT8 idx;
		for( idx = 0; idx < listCnt; idx++ ) {
			if( strcmp( strName, list[ idx ] ) == 0 )
				return idx;
		}
		return 0xFF;
	}
	else if( idx < listCnt )
		strcpy( strName, list[ idx ] );
	else
		return FAIL;

	return SUCCESS;
}

/**
 * @fn		  UINT16 usbStr2Num(UINT8* str)
 * @brief	  usbStr2Num
 * @param	  [in] str
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  allenlin
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16 usbStr2Num( UINT8* str ) {
	UINT16 val;
	UINT8 ret;

	ret = usbStrToNumConv( &str[ 2 ] );
	val = ret << 12;
	ret = usbStrToNumConv( &str[ 3 ] );
	val |= ret << 8;
	ret = usbStrToNumConv( &str[ 4 ] );
	val |= ret << 4;
	ret = usbStrToNumConv( &str[ 5 ] );
	val |= ret;
	return val;
}

/**
 * @fn		  UINT32 usbStringSet(UINT32 type, UINT8* pStrSrc, UINT32 lenSrc)
 * @brief	  usbStringSet
 * @param	  [in] type
 * @param	  [in] pStrSrc
 * @param	  [in] lenSrc
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 usbStringSet( UINT32 type, UINT8* pStrSrc, UINT32 lenSrc ) {
	UINT8* destAddr;
	UINT32 destLen = 0;
	UINT32 i, j;

	if( type == 0x04 ) { //usbManufactureName
		destAddr = K_ManufacturerString;
		destLen = sizeof( K_ManufacturerString );
	} else if( type == 0x05 ) { //product name
		destAddr = K_ProductString;
		destLen = sizeof( K_ProductString );
	} else if ( type == 0x06 ) { //serial number
		destAddr = K_SerialNumberString;
		destLen = sizeof( K_SerialNumberString );
	} 
#if TAG_TODO/* mark for FPGA use */
	else if ( type ==  0x0a ) { // SIDC Manufacture
		destAddr = ( UINT8* )K_PIMADATA_Manufacturer;
		destLen = sizeof( K_PIMADATA_Manufacturer );
	} else if ( type == 0x0b ) { // SIDC Model
		destAddr = ( UINT8* )K_PIMADATA_Model;
		destLen = sizeof( K_PIMADATA_Model );
	} else if ( type == 0x0c ) { // SIDC DeivceVersion
		destAddr = ( UINT8* )K_PIMADATA_DeviceVersion;
		destLen = sizeof( K_PIMADATA_DeviceVersion );
	} else if ( type == 0x0d ) { //SIDC SerialNumber
		destAddr = ( UINT8* )K_PIMADATA_SerialNumber;
		destLen = sizeof( K_PIMADATA_SerialNumber );
	} else if ( type == 0x0e ) { // SIDC VendorExtensionDesc 
		destAddr = ( UINT8* )K_PIMADATA_VendorExtensionDesc;
		destLen = sizeof( K_PIMADATA_VendorExtensionDesc );
	}
#endif
	else
		return FAIL;

	i = 2;
	if( ( destLen - 2 ) / 2 < lenSrc )
		lenSrc = ( destLen - 2 ) / 2;
	
	for( j = 0; j < lenSrc; j++ ) {
		destAddr[i++] = pStrSrc[j];
		destAddr[i++] = 0x00;
	}

  //fill string end character after modify the string
	for( ; i < destLen; i++ )
		destAddr[i] = 0x00;
	
	return SUCCESS;
}

/**
 * @fn		  UINT32 usbMsdcStringSet(UINT32 type, UINT8* pStrSrc, UINT32 lenSrc)
 * @brief	  usbMsdcStringSet
 * @param	  [in] type
 * @param	  [in] pStrSrc
 * @param	  [in] lenSrc
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 usbMsdcStringSet( UINT32 type, UINT8* pStrSrc, UINT32 lenSrc ) {
	UINT8* destAddr;
	UINT32 destLen = 0;
	UINT32 j ;

	if( type == 0x07 ) { // MSDC Vendor Id
		destAddr = G_ScsiVendorInformation;
		destLen = sizeof( G_ScsiVendorInformation );
	} else if ( type == 0x08 ) {  // MSDC Product Id
		destAddr = G_ScsiProductIdentification;
		destLen = sizeof( G_ScsiProductIdentification );
	} else if ( type == 0x09 ) {  // MSDC Revision Id
		destAddr = G_ScsiProductRevisionLevel;
		destLen = sizeof( G_ScsiProductRevisionLevel );
	} else
		return FAIL;

	lenSrc = lenSrc <= destLen ? lenSrc : destLen;
	for( j = 0; j < lenSrc; j++ ) {
		destAddr[ j ] = pStrSrc[ j ];
	}
	if( type != 0x09 ) {
		while( j < destLen )
			destAddr[j++] = ' ';
	}
	
	return SUCCESS;
	}


/**
 * @fn		  UINT8 usbStrElemParse(UINT8** resIdx, UINT32* resSize, UINT8* strName, UINT8* strVal)
 * @brief	  usbStrElemParse
 * @param	  [in] resIdx
 * @param	  [in] resSize
 * @param	  [in] strName
 * @param	  [in] strVal
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 usbStrElemParse( UINT8** resIdx, UINT32* resSize, UINT8* strName, UINT8* strVal ) 
{
	UINT8* beginIdx;

	if( *resSize > 0 /*&& *( *resIdx )-- == 0x0A*/ ) {
		/*get strName*/
		while( **resIdx != '=' && *resSize > 0 ) {
			*strName++ = **resIdx;
			( *resIdx )++;
			( *resSize )--;
		}
		if( *resSize == 0 )
			return 1;
		*strName = '\0';
		( *resIdx )++;
		( *resSize )--;

		strName--;
		/*remove whitespace in strName*/
		while( *strName == ' ' ) {
			*strName = '\0';
			strName--;
		}

		/*skip to strVal*/
		while( **resIdx != '"' ) {
			( *resIdx )++;
			( *resSize )--;
		}
		( *resIdx )++;
		( *resSize )--;
		/*get strVal*/
		beginIdx = strVal;
		while( **resIdx != '"' ) {
			*strVal++ = **resIdx;
			( *resIdx )++;
			( *resSize )--;
		}
		*strVal = '\0';

		/*skip to next line*/
		while( **resIdx != 0x0D /*&& *( *resIdx )++ != 0x0A*/ && *resSize > 0 ) {
			( *resIdx )++;
			( *resSize )--;
		}
		if( *resSize > 0 ) {
			*resIdx += 2;
			*resSize -= 2;
			}
		}
	else 
		return 1;

	return 0;
}

/**
 * @fn		  UINT8 usbStrToNumConv(UINT8 *ptr)
 * @brief	  usbStrToNumConv
 * @param	  [in] ptr
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
usbStrToNumConv(
	UINT8 *ptr
)
{
	UINT8 ret = 0;
	if((*ptr>=0x30) &&(*ptr<=0x39)){
		ret = *ptr-0x30;
	}
	else if((*ptr>=0x41) &&(*ptr<=0x46)){
		ret = *ptr-0x37;
	}
	else if((*ptr>=0x61) &&(*ptr<=0x66)){
		ret = *ptr-0x57;
	}
	return ret;
}

/**
 * @fn		  UINT32 usbVidSet(UINT16 vid)
 * @brief	  usbVidSet
 * @param	  [in] vid
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32
usbVidSet(
	UINT16 vid
)
{
	if (vid == 0)
		return FAIL;

	K_DeviceDescriptor[8] = (UINT8)(vid & 0x00FF);
	K_DeviceDescriptor[9] = (UINT8)(vid >> 8);
	K_MSDC_DeviceDescriptor[8] = (UINT8)(vid & 0x00FF);
	K_MSDC_DeviceDescriptor[9] = (UINT8)(vid >> 8);
	K_SIDC_DeviceDescriptor[8] = (UINT8)(vid & 0x00FF);
	K_SIDC_DeviceDescriptor[9] = (UINT8)(vid >> 8);

	return SUCCESS;
}


/**
 * @fn		  UINT32 usbPidSet(UINT32 mode, UINT16 pid)
 * @brief	  usbPidSet
 * @param	  [in] mode
 * @param	  [in] pid
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32
usbPidSet(
	UINT32 mode,
	UINT16 pid
)
{
	if (mode == USB_MODE_PCCAM) {
		K_DeviceDescriptor[10] = (UINT8)(pid & 0x00FF);
		K_DeviceDescriptor[11] = (UINT8)(pid >> 8);
	}
	else if (mode == USB_MODE_MSDC) {
		K_MSDC_DeviceDescriptor[10] = (UINT8)(pid & 0x00FF);
		K_MSDC_DeviceDescriptor[11] = (UINT8)(pid >> 8);
	}
	else if (mode == USB_MODE_SIDC) {
		K_SIDC_DeviceDescriptor[10] = (UINT8)(pid & 0x00FF);
		K_SIDC_DeviceDescriptor[11] = (UINT8)(pid >> 8);
	}
	else
		return FAIL;

	return SUCCESS;
}

#if 0
UINT32
usbDefaultsizeSet(UINT8* pStrSrc)
{
    if(strcmp(pStrSrc,"1280" )==0)
        {
        //default size
        }
    else if(strcmp(pStrSrc,"640")==0)
        {//MJPG
            K_ConfigurationDescriptor_VS_FORMAT_USB20[29]=0x02;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[31]=0x80;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[32]=0x02;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[33]=0xe0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[34]=0x01;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[59]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[61]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[62]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[63]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[64]=0x02;
            
            //YUY2
            K_ConfigurationDescriptor_VS_FORMAT_USB20[182]=0x02;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[184]=0x80;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[185]=0x02;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[186]=0xe0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[187]=0x01;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[212]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[214]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[215]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[216]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[217]=0x02;
        }
    else if(strcmp(pStrSrc,"320")==0)
        {
        //MJPG
            K_ConfigurationDescriptor_VS_FORMAT_USB20[29]=0x03;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[31]=0x40;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[32]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[33]=0xF0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[34]=0x00;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[89]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[91]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[92]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[93]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[94]=0x02;
            
            //YUY2
            K_ConfigurationDescriptor_VS_FORMAT_USB20[182]=0x03;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[184]=0x40;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[185]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[186]=0xF0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[187]=0x00;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[252]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[254]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[255]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[256]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[257]=0x02;
        }
    else if(strcmp(pStrSrc,"160")==0)
        {
         //MJPG
            K_ConfigurationDescriptor_VS_FORMAT_USB20[29]=0x04;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[31]=0xa0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[32]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[33]=0x78;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[34]=0x00;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[119]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[121]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[122]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[123]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[124]=0x02;
            
            //YUY2
            K_ConfigurationDescriptor_VS_FORMAT_USB20[182]=0x04;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[184]=0xa0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[185]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[186]=0x78;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[187]=0x00;
            
            K_ConfigurationDescriptor_VS_FORMAT_USB20[282]=0x01;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[284]=0x00;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[285]=0x05;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[286]=0xd0;
            K_ConfigurationDescriptor_VS_FORMAT_USB20[287]=0x02;
        }
    else
        {
        return FAIL;
        }
    return SUCCESS;
}
#endif
/**
 * @fn        void sp1kUsbMsdcMLunSet(UINT8 en)
 * @brief     sp1kUsbMsdcMLunSet
 * @param     [in] en
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kUsbMsdcMLunSet(
	UINT8 en
)
{
	if (en) {
		G_ucMSDC_MLUN = 1;
	} else {
		G_ucMSDC_MLUN = 0;
	}

}
