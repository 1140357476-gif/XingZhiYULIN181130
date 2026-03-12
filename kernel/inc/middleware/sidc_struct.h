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
 * @file      sidc_struct.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SIDC_STRUCT_H__
#define __SIDC_STRUCT_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "sidc_param.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SIDC_CONTAINER_SIZE        0x000c//sizeof(sidc_container_t)    //not counting paramteter size
#define SIDC_DEVICEINFO_SIZE	
#define SIDC_STRAGEINFO_SIZE 	sizeof(sidc_storage_info_t)
#define K_USBSIDC_ParamCount                    3
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct 
{
        UINT32   ulContainerLength;
        UINT16  usContainerType;
        UINT16  usCode;
        UINT32   ulTransactionId;
        UINT32   ulParameter[K_USBSIDC_ParamCount];
} sidc_container_t; 

typedef struct 
{
	UINT32   storageIDsSize;
	UINT32   storageIDs[K_PIMADATA_StorageIDsSize];
} sidc_storage_t;

typedef struct
{
        UINT16  storageType;
        UINT16  filesystemType;
        UINT16  accessCapability;
        UINT8   maxCapacity[K_PIMADATA_MaxCapacitySize];
        UINT8   freeSpaceInByte[K_PIMADATA_FreeSpaceInByteSize];
	UINT32   freeSpaceInImage;
        //STR0x00 storageDescription;
        //STR0x00 volumnLabel;
        UINT8   storageDescription;
        UINT8   volumnLabel;
} sidc_storage_info_t;

typedef struct 
{
        UINT32	storageID;
        UINT16  objectFormat;
        UINT16	protectionStatus;
        UINT32	objectCompressedSize;
        UINT16	thumbFormat;
	UINT32	thumbCompressedSize;
	UINT32	thumbPixWidth;
	UINT32	thumbPixHeight;
	UINT32	imagePixWidth;
	UINT32	imagePixHeight;
	UINT32	imageBitDepth;
	UINT32	parentObject;
	UINT16	associationType;
	UINT32	associationDesc;
	UINT32	sequenceNumber;
	UINT8	fileNameBuf[K_PIMADATA_ObjectInfoFileNameBufSize];
	UINT8	captureDate[K_PIMADATA_ObjectInfoCaptureDate];
	UINT8 	modificationDate[K_PIMADATA_ObjectInfoModificationDate];
	UINT8	keyword[K_PIMADATA_ObjectInfoKeyword];
}sidc_object_info_t;

typedef struct 
{
	UINT32 handle;
	UINT8 filePath[33];
	UINT8* fileName;
	UINT16 fileType;
	UINT32 fileSize;
	UINT16 width;
	UINT16 height;
	UINT32 thembAddr;
	UINT32 thembSize;
	UINT16 thembWidth;
	UINT16 thembHeight;
	UINT16 other;
}sidc_file_info_t;



typedef struct 
{
        UINT8   numChars;
} STR0x00;

typedef struct 
{
        UINT8   numChars;
        UINT16  stringChars[0x08];
} STR0x08;

typedef struct 
{
        UINT8   numChars;
        UINT16  stringChars[0x10];
} STR0x10;

typedef struct 
{
        UINT8   numChars;
        UINT16  stringChars[0x18];
} STR0x18;

typedef struct 
{
        UINT8   numChars;
        UINT16  stringChars[0x20];
} STR0x20; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif
