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
 * @file      sidc_param.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SIDC_PARAM_H__
#define __SIDC_PARAM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//---- PIMA 15740 Datatype Code ------
#define K_PIMATYP_UNDEF                         0x0000
#define K_PIMATYP_INT8                          0x0100
#define K_PIMATYP_UINT8                         0x0200
#define K_PIMATYP_INT16                         0x0300
#define K_PIMATYP_UINT16                        0x0400
#define K_PIMATYP_INT32                         0x0500
#define K_PIMATYP_UINT32                        0x0600
#define K_PIMATYP_INT64                         0x0700
#define K_PIMATYP_UINT64                        0x0800
#define K_PIMATYP_INT128                        0x0900
#define K_PIMATYP_UINT128                       0x0a00
#define K_PIMATYP_AINT8                         0x0140
#define K_PIMATYP_AUINT8                        0x0240
#define K_PIMATYP_AINT16                        0x0340
#define K_PIMATYP_AUINT16                       0x0440
#define K_PIMATYP_AINT32                        0x0540
#define K_PIMATYP_AUINT32                       0x0640
#define K_PIMATYP_AINT64                        0x0740
#define K_PIMATYP_AUINT64                       0x0840
#define K_PIMATYP_AINT128                       0x0940
#define K_PIMATYP_AUINT128                      0x0a40
#define K_PIMATYP_STR                           0xffff
                                                     
//---- PIMA 15740 Operation Code ------
#define K_PIMAOPE_Undefined                     0x0010
#define K_PIMAOPE_GetDeviceInfo                 0x0110
#define K_PIMAOPE_OpenSession                   0x0210
#define K_PIMAOPE_CloseSession                  0x0310
#define K_PIMAOPE_GetStorageIDs                 0x0410
#define K_PIMAOPE_GetStorageInfo                0x0510
#define K_PIMAOPE_GetNumObjects                 0x0610
#define K_PIMAOPE_GetObjectHandles              0x0710
#define K_PIMAOPE_GetObjectInfo                 0x0810
#define K_PIMAOPE_GetObject                     0x0910
#define K_PIMAOPE_GetThumb                      0x0a10 
#define K_PIMAOPE_DeleteObject                  0x0b10
#define K_PIMAOPE_SendObjectInfo                0x0c10
#define K_PIMAOPE_SendObject                    0x0d10
#define K_PIMAOPE_InitiateCapture               0x0e10
#define K_PIMAOPE_FormatStore                   0x0f10
#define K_PIMAOPE_ResetDevice                   0x1010
#define K_PIMAOPE_SelfTest                      0x1110
#define K_PIMAOPE_SetObjectProtection           0x1210
#define K_PIMAOPE_PowerDown                     0x1310
#define K_PIMAOPE_GetDevicePropDesc             0x1410
#define K_PIMAOPE_GetDevicePropValue            0x1510
#define K_PIMAOPE_SetDevicePropValue            0x1610
#define K_PIMAOPE_ResetDevicePropValue          0x1710
#define K_PIMAOPE_TerminateOpenCapture          0x1810
#define K_PIMAOPE_MoveObject                    0x1910
#define K_PIMAOPE_CopyObject                    0x1a10
#define K_PIMAOPE_GetPartialObject              0x1b10
#define K_PIMAOPE_InitiateOpenCapture           0x1c10
                                                      
//---- PIMA 15740 Response Code -------
#define K_PIMARES_Undefined                     0x0020
#define K_PIMARES_OK                            0x0120
#define K_PIMARES_GeneralError                  0x0220
#define K_PIMARES_SessionNotOpen                0x0320
#define K_PIMARES_InvalidTransactionID          0x0420
#define K_PIMARES_OperationNotSupported         0x0520
#define K_PIMARES_ParameterNotSupported         0x0620
#define K_PIMARES_IncompleteTransfer            0x0720
#define K_PIMARES_InvalidStorageID              0x0820
#define K_PIMARES_InvalidObjectHandle           0x0920
#define K_PIMARES_DevicePropNotSupported        0x0a20
#define K_PIMARES_InvalidObjectFormatCode       0x0b20
#define K_PIMARES_StoreFull                     0x0c20 
#define K_PIMARES_ObjectWriteProtected          0x0d20
#define K_PIMARES_StoreReadOnly                 0x0e20
#define K_PIMARES_AccessDenied                  0x0f20
#define K_PIMARES_NoThumbnailPresent            0x1020
#define K_PIMARES_SelfTestFailed                0x1120
#define K_PIMARES_PartialDeletion               0x1220 
#define K_PIMARES_StoreNotAvailable             0x1320 
#define K_PIMARES_SpecByFormatUnsupported       0x1420
#define K_PIMARES_NoValidObjectInfo             0x1520
#define K_PIMARES_InvalidCodeFormat             0x1620
#define K_PIMARES_UnknownVendorCode             0x1720
#define K_PIMARES_CaptureAlreadyTerminated      0x1820
#define K_PIMARES_DeviceBusy                    0x1920
#define K_PIMARES_InvalidParentObject           0x1a20
#define K_PIMARES_InvalidDevicePropFormat       0x1b20
#define K_PIMARES_InvalidDevicePropValue        0x1c20
#define K_PIMARES_InvalidParameter              0x1d20
#define K_PIMARES_SessionAlreadyOpen            0x1e20
#define K_PIMARES_TransactionCancelled          0x1f20
#define K_PIMARES_SpecOfDestinationUnsupported  0x2020

//---- PIMA 15740 Object Format Code -------
#define K_PIMAOBJ_AncillaryUndefined            0x0030
#define K_PIMAOBJ_Association                   0x0130
#define K_PIMAOBJ_Script                        0x0230
#define K_PIMAOBJ_Executable                    0x0330
#define K_PIMAOBJ_Text                          0x0430
#define K_PIMAOBJ_HTML                          0x0530
#define K_PIMAOBJ_DPOF                          0x0630
#define K_PIMAOBJ_AIFF                          0x0730
#define K_PIMAOBJ_WAV                           0x0830
#define K_PIMAOBJ_MP3                           0x0930
#define K_PIMAOBJ_AVI                           0x0a30
#define K_PIMAOBJ_MPEG                          0x0b30
#define K_PIMAOBJ_ASF                           0x0c30
#define K_PIMAOBJ_ImageUndefined0               0x0038
#define K_PIMAOBJ_EXIF_JPEG                     0x0138
#define K_PIMAOBJ_TIFF_EP                       0x0238
#define K_PIMAOBJ_FlashPix                      0x0338
#define K_PIMAOBJ_BMP                           0x0438
#define K_PIMAOBJ_CIFF                          0x0538
#define K_PIMAOBJ_ImageUndefined1               0x0638
#define K_PIMAOBJ_GIF                           0x0738
#define K_PIMAOBJ_JFIF                          0x0838
#define K_PIMAOBJ_PCD                           0x0938
#define K_PIMAOBJ_PICT                          0x0a38
#define K_PIMAOBJ_PNG                           0x0b38
#define K_PIMAOBJ_ImageUndefined2               0x0c38
#define K_PIMAOBJ_TIFF                          0x0d38
#define K_PIMAOBJ_TIFF_IT                       0x0e38
#define K_PIMAOBJ_JP2                           0x0f38
#define K_PIMAOBJ_JPX                           0x1038
                                                      
//---- PIMA 15740 Event Code -------
#define K_PIMAEVE_Undefined                     0x0040
#define K_PIMAEVE_CancelTransaction             0x0140
#define K_PIMAEVE_ObjectAdded                   0x0240
#define K_PIMAEVE_ObjectRemoved                 0x0340
#define K_PIMAEVE_StoreAdded                    0x0440
#define K_PIMAEVE_StoreRemoved                  0x0540
#define K_PIMAEVE_DevicePropChanged             0x0640
#define K_PIMAEVE_ObjectInfoChanged             0x0740
#define K_PIMAEVE_DeviceInfoChanged             0x0840
#define K_PIMAEVE_RequestObjectTransfer         0x0940
#define K_PIMAEVE_StoreFull                     0x0a40
#define K_PIMAEVE_DeviceReset                   0x0b40
#define K_PIMAEVE_StorageInfoChanged            0x0c40
#define K_PIMAEVE_CaptureComplete               0x0d40
#define K_PIMAEVE_UnreportedStatus              0x0e40
                                                      
//---- PIMA 15740 Device Property Code -------
#define K_PIMADEV_Undefined                     0x0050
#define K_PIMADEV_BatteryLevel                  0x0150
#define K_PIMADEV_FunctionMode                  0x0250
#define K_PIMADEV_ImageSize                     0x0350
#define K_PIMADEV_CompressionSetting            0x0450
#define K_PIMADEV_WhiteBalance                  0x0550
#define K_PIMADEV_RGBGain                       0x0650
#define K_PIMADEV_F_Number                      0x0750
#define K_PIMADEV_FocalLength                   0x0850
#define K_PIMADEV_FocusDistance                 0x0950
#define K_PIMADEV_FocusMode                     0x0a50
#define K_PIMADEV_ExposureMeteringMode          0x0b50
#define K_PIMADEV_FlashMode                     0x0c50 
#define K_PIMADEV_ExposureTime                  0x0d50
#define K_PIMADEV_ExposureProgramMode           0x0e50
#define K_PIMADEV_ExposureIndex                 0x0f50
#define K_PIMADEV_ExposureBiasCompensation      0x1050
#define K_PIMADEV_DateTime                      0x1150
#define K_PIMADEV_CaptureDelay                  0x1250 
#define K_PIMADEV_StillCaptureMode              0x1350
#define K_PIMADEV_Contrast                      0x1450
#define K_PIMADEV_Sharpness                     0x1550
#define K_PIMADEV_DigitalZoom                   0x1650
#define K_PIMADEV_EffectMode                    0x1750
#define K_PIMADEV_BurstNumber                   0x1850
#define K_PIMADEV_BurstInterval                 0x1950
#define K_PIMADEV_TimelapseNumber               0x1a50
#define K_PIMADEV_TimelapseInterval             0x1b50
#define K_PIMADEV_FocusMeteringMode             0x1c50
#define K_PIMADEV_UploadURL                     0x1d50
#define K_PIMADEV_Artist                        0x1e50
#define K_PIMADEV_CopyrightInfo                 0x1f50
#define K_PIMADEV_All                           0xffff

//Storage ID
#define K_PIMADATA_IDStorage			0x05050505
#define K_PIMADATA_StorageIDsSize		0x00000001

//Storage Info
#define K_PIMADATA_MaxCapacitySize		0x08
#define K_PIMADATA_FreeSpaceInByteSize		0x08

#define K_PIMADATA_StorageType_Undefined	0x0000
#define K_PIMADATA_StorageType_FixedRom		0x0100
#define K_PIMADATA_StorageType_RemovableRom	0x0200
#define K_PIMADATA_StorageType_FixedRam		0x0300
#define K_PIMADATA_StorageType_RemovableRam	0x0400

#define K_PIMADATA_FilesystemType_Undefined	0x0000
#define K_PIMADATA_FilesystemType_GenericFlat	0x0100
#define K_PIMADATA_FilesystemType_GenericHier	0x0200
#define K_PIMADATA_FilesystemType_DCF		0x0300

#define K_PIMADATA_AccessCapability_ReadWrite			0x0000
#define K_PIMADATA_AccessCapability_ReadOnlyWithoutDelete	0x0100
#define K_PIMADATA_AccessCapability_ReadOnlyWitDelete		0x0200

#define K_PIMADATA_StorageDescriptionSize	0x00            //size of storage desc 
#define K_PIMADATA_VolumnLabelSize		0x00            //size of volumn label 

//Object Info
//#define K_PIMADATA_OBJINF_LEN 82//(78+4)
#define K_PIMADATA_ObjectInfoFileNameBufSize	(13*2+1)//0x1A0
#define K_PIMADATA_ObjectInfoCaptureDate		(0*2+1)//0x1A0
#define K_PIMADATA_ObjectInfoModificationDate	(0*2+1)//0x1A0
#define K_PIMADATA_ObjectInfoKeyword			(0*2+1)//0x1A0

#define K_PIMADATA_StandardVersion              		0x6400          //1.00
#define K_PIMADATA_VendorExtensionID            	0x00000000      //no extesnions
#define K_PIMADATA_VendorExtensionVersion       	0x6400          //1.00
#define K_PIMADATA_FunctionalMode               		0x0000          //standard mode

//
#define K_PIMADATA_ContainerType_Undefined                     0x0000         
#define K_PIMADATA_ContainerType_CommandBlock                  0x0100         
#define K_PIMADATA_ContainerType_DataBlock                     0x0200         
#define K_PIMADATA_ContainerType_ResponseBlock                 0x0300
#define K_PIMADATA_ContainerType_EventBlock                    0x0400
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

#endif 
