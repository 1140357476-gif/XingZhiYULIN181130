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
 * @file      scsi.h
 * @brief     Module related to SCSI command
 * @author    Chi-Yeh Tsai
 * @since     2010-01-01
 * @date      2010-01-01
*/
#ifndef __SCSI_H__
#define __SCSI_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//---- SCSI-2 Commands ----------------
#define K_SCSICMD_TEST_UNIT_READY                 0x00
#define K_SCSICMD_REZERO_UNIT                     0x01
//#define K_SCSICMD_VENDOR_SPECIFIC               0x02
#define K_SCSICMD_REQUEST_SENSE                   0x03
#define K_SCSICMD_FORMAT_UNIT                     0x04
#define K_SCSICMD_READ_BLOCK_LIMITS               0x05
//#define K_SCSICMD_VENDOR_SPECIFIC               0x06
#define K_SCSICMD_REASSIGN_BLOCKS                 0x07
#define K_SCSICMD_READ_06                         0x08
//#define K_SCSICMD_VENDOR_SPECIFIC               0x09
#define K_SCSICMD_WRITE_06                        0x0a
#define K_SCSICMD_SEEK_06                         0x0b
//#define K_SCSICMD_VENDOR_SPECIFIC               0x0c
//#define K_SCSICMD_VENDOR_SPECIFIC               0x0d
//#define K_SCSICMD_VENDOR_SPECIFIC               0x0e
#define K_SCSICMD_READ_REVERSE                    0x0f
#define K_SCSICMD_WRITE_FILEMARKS                 0x10
#define K_SCSICMD_SPACE                           0x11
#define K_SCSICMD_INQUIRY                         0x12
#define K_SCSICMD_VERIF06                         0x13
#define K_SCSICMD_RECOVER_BUFFERED_DATA           0x14
#define K_SCSICMD_MODE_SELECT_06                  0x15
#define K_SCSICMD_RESERVE                         0x16
#define K_SCSICMD_RELEASE                         0x17
#define K_SCSICMD_COPY                            0x18
#define K_SCSICMD_ERASE                           0x19
#define K_SCSICMD_MODE_SENSE_06                   0x1a
#define K_SCSICMD_START_STOP_UNIT                 0x1b
#define K_SCSICMD_RECEIVE_DIAGNOSTIC_RESULTS      0x1c
#define K_SCSICMD_SEND_DIAGNOSTIC                 0x1d
#define K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL    0x1e
//#define K_SCSICMD_VENDOR_SPECIFIC               0x1f
//#define K_SCSICMD_VENDOR_SPECIFIC               0x20
//#define K_SCSICMD_VENDOR_SPECIFIC               0x21
//#define K_SCSICMD_VENDOR_SPECIFIC               0x22
#define K_SCSICMD_READ_FORMAT_CAPACITIES          0x23
#define K_SCSICMD_SET_WINDOW                      0x24
#define K_SCSICMD_READ_CAPACITY                   0x25
//#define K_SCSICMD_VENDOR_SPECIFIC               0x26
//#define K_SCSICMD_VENDOR_SPECIFIC               0x27
#define K_SCSICMD_READ_10                         0x28
#define K_SCSICMD_READ_GENERATION                 0x29
#define K_SCSICMD_WRITE_10                        0x2a
#define K_SCSICMD_SEEK_10                         0x2b
#define K_SCSICMD_ERASE_10                        0x2c
#define K_SCSICMD_READ_UPDATED_BLOCK              0x2d
#define K_SCSICMD_WRITE_AND_VERIFY_10             0x2e
#define K_SCSICMD_VERIFY_10                       0x2f
#define K_SCSICMD_SEARCH_DATA_HIGH_10             0x30
#define K_SCSICMD_SEARCH_DATA_EQUAL_10            0x31
#define K_SCSICMD_SEARCH_DATA_LOW_10              0x32
#define K_SCSICMD_SET_LIMITS                      0x33
#define K_SCSICMD_PREFETCH                        0x34
#define K_SCSICMD_SYNCHRONIZE_CACHE               0x35
#define K_SCSICMD_LOCK_UNLOCK_CACHE               0x36
#define K_SCSICMD_READ_DEFECT_DATA_10             0x37
#define K_SCSICMD_MEDIUM_SCAN                     0x38
#define K_SCSICMD_COMPARE                         0x39
#define K_SCSICMD_COPAND_VERIFY                   0x3a
#define K_SCSICMD_WRITE_BUFFER                    0x3b
#define K_SCSICMD_READ_BUFFER                     0x3c
#define K_SCSICMD_UPDATE_BLOCK                    0x3d
#define K_SCSICMD_READ_LONG                       0x3e
#define K_SCSICMD_WRITE_LONG                      0x3f
#define K_SCSICMD_CHANGE_DEFINITION               0x40
#define K_SCSICMD_WRITE_SAME                      0x41
#define K_SCSICMD_READ_SUBCHANNEL                 0x42
#define K_SCSICMD_READ_TOC                        0x43
#define K_SCSICMD_READ_HEADER                     0x44
#define K_SCSICMD_PLAY_AUDIO_10                   0x45
//#define K_SCSICMD_VENDOR_SPECIFIC               0x46
#define K_SCSICMD_PLAY_AUDIO_MSF                  0x47
#define K_SCSICMD_PLAY_AUDIO_TRACK_INDEX          0x48
#define K_SCSICMD_PLAY_TRACK_RELATIVE             0x49
//#define K_SCSICMD_VENDOR_SPECIFIC               0x4a
#define K_SCSICMD_PAUSE_RESUME                    0x4b
#define K_SCSICMD_LOG_SELECT                      0x4c
#define K_SCSICMD_LOG_SENSE                       0x4d
//#define K_SCSICMD_VENDOR_SPECIFIC               0x4e        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x4f        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x50        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x51        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x52        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x53        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x54        
#define K_SCSICMD_MODE_SELECT_10                  0x55        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x56        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x57        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x58        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x59        
#define K_SCSICMD_MODE_SENSE_10                   0x5a        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x5b        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x5c        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x5d        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x5e        
//#define K_SCSICMD_VENDOR_SPECIFIC               0x5f        
#define K_SCSICMD_READ_12                         0xa8
#define K_SCSICMD_WRITE_12                        0xaa 

#define SCSICMD_MODE_CURRENT_VALUE      0x00
#define SCSICMD_MODE_CHANGEABLE_VALUE   0x40
#define SCSICMD_MODE_DEFAULT_VALUE      0x80
#define SCSICMD_MODE_SAVED_VALUE        0xc0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//---- SCSI-2 Commands ----------------
//6-byte Command
typedef struct _SCSICMD_06BYTE
{
    UINT8   operationCode;
    UINT8   serviceAction;
    UINT16  logicalBlockAddress;
    UINT8   dataLength;
    UINT8   controlByte;
} SCSICMD_06BYTE, *PSCSICMD_06BYTE;

//10-byte Command
typedef struct _SCSICMD_10BYTE
{
    UINT8   operationCode;
    UINT8   serviceAction;
    UINT32   logicalBlockAddress;
    UINT8   reserved;
    UINT16  dataLength;
    UINT8   controlByte;
} SCSICMD_10BYTE, *PSCSICMD_10BYTE;

//12-byte Command
typedef struct _SCSICMD_12BYTE
{
    UINT8   operationCode;
    UINT8   serviceAction;
    UINT32   logicalBlockAddress;
    UINT32   dataLength;
    UINT8   reserved;
    UINT8   controlByte;
} SCSICMD_12BYTE, *PSCSICMD_12BYTE;

//16-byte Command
typedef struct _SCSICMD_16BYTE
{
    UINT8   operationCode;
    UINT8   serviceAction;
    UINT32   logicalBlockAddress;
    UINT32   addtionalCdbData;
    UINT32   dataLength;
    UINT8   reserved;
    UINT8   controlByte;
} SCSICMD_16BYTE, *PSCSICMD_16BYTE; 

//REQUEST SENSE
#define SCSIDATA_REQUEST_SENSE_SIZE                     0x12
typedef struct _SCSIDATA_REQUEST_SENSE
{
    struct
    {
	//richie switch order
            UINT8   errorCode:7;
            UINT8   valid:1;                     
    } BYTE0;

    UINT8   reserved0;

    struct
    {
	//richie switch order
            UINT8   senseKey:4;
            UINT8   reserved:4;
    } BYTE2;

    UINT32   information;
    UINT8   additionalSenseLength; //0x0a                              
    UINT8   reserved1[4];
    UINT8   additionalSenseCode;
    UINT8   additionalSenseCodeQualifier;
    UINT8   reserved2[4];
} SCSIDATA_REQUEST_SENSE, *PSCSIDATA_REQUEST_SENSE;

//FORMAT UNIT  
#define SCSIDATA_FORMAT_UNIT_SIZE                       0x0c
typedef struct _SCSIDATA_FORMAT_UNIT  
{
    struct
	{
        UINT8   reserved;
        struct
    	{
			//richie switch order
            UINT8   side:1;                     
            UINT8   immediate:1;                     
            UINT8   reserved:2;                     
            UINT8   singleTrack:1;                     
            UINT8   DCRT:1;
            UINT8   extend:1;                     
            UINT8   FOV:1;                     
    	} BYTE1;
        UINT16  defectListLength;
	} defectListHeader;

    struct
    {
        UINT32   numberOfBlocks;
        UINT8   reserved;
        UINT8   blockLengthHi;
        UINT16  blockLengthLo;
    } formatDescriptor;
} SCSIDATA_FORMAT_UNIT, *PSCSIDATA_FORMAT_UNIT;

//INQUIRY
#define SCSIDATA_INQUIRY_SIZE                           0x29
typedef struct _SCSIDATA_INQUIRY
{
    struct
    {
		//richie switch order
        UINT8   peripheralDeviceType:5;
        UINT8   reserved:3;                     
    } BYTE0;

    struct
    {
		//richie switch order
        UINT8   reserved:7;
        UINT8   RMB:1;
    } BYTE1;

    struct
    {
		//richie switch order
        UINT8   ansiVersion:3;
        UINT8   emcaVersion:3;
        UINT8   isoVersion:2;
    } BYTE2;

    struct
    {
		//richie switch order
        UINT8   responseDataFormat:4;
        UINT8   reserved:4;
    } BYTE3;

    UINT8   additionalLength; //0x1f                              
    UINT8   reserved0[3];
    UINT8   vendorInformation[0x0a];
    UINT8   productIdentification[0x13];
    UINT8   productRevisionLevel[0x04];
} SCSIDATA_INQUIRY, *PSCSIDATA_INQUIRY;

//MODE SENSE 06
//MODE SELECT 06
//Mode Parameter Header
//patch4.5@richie@wp xp begin
#define SCSIDATA_MODE06_PARAMETER_HEADER_SIZE             0x04
typedef struct _SCSIDATA_MODE06_PARAMETER_HEADER
{
    UINT8	modeDataLength;
    UINT8   mediumTypeCode;
    struct
    {
		//richie
        UINT8   reserved1:4;
        UINT8   DPOFUA:1;
        UINT8   reserved0:2;
        UINT8   WP:1;
    } BYTE3;
    UINT8 reserved;
} SCSIDATA_MODE06_PARAMETER_HEADER, *PSCSIDATA_MODE06_PARAMETER_HEADER;

//Mode Parameter Header
#define SCSIDATA_MODE10_PARAMETER_HEADER_SIZE             0x08
typedef struct _SCSIDATA_MODE10_PARAMETER_HEADER
{
    UINT16  modeDataLength;
    UINT8   mediumTypeCode;
    struct
    {
		//richie
        UINT8   reserved1:4;
        UINT8   DPOFUA:1;
        UINT8   reserved0:2;
        UINT8   WP:1;
    } BYTE3;
    UINT8 reserved[4];
} SCSIDATA_MODE10_PARAMETER_HEADER, *PSCSIDATA_MODE10_PARAMETER_HEADER;
//patch4.5@richie@wp xp end

//Read-Write Error Recovery Page (0x01)
#define SCSIDATA_MODE_RW_ERR_RECOVERY_PAGE_SIZE         0x0c
typedef struct _SCSIDATA_MODE_RW_ERR_RECOVERY_PAGE
{
    struct
    {
		//richie switch order
        UINT8   pageCode:6;
        UINT8   reserved:1;
        UINT8   PS:1;
    } BYTE0;

    UINT8   pageLength;

    struct
    {
		//richie switch order
        UINT8   DCR:1;
        UINT8   reserved2:1;
        UINT8   PER:1;
        UINT8   reserved1:1;
        UINT8   RC:1;
        UINT8   reserved0:2;
        UINT8   AWRE:1;
    } BYTE2;

    UINT8   readRetryCount;
    UINT8   reserved0;
    UINT8   writeRetryCount;
    UINT8   reserved1;
} SCSIDATA_MODE_RW_ERR_RECOVERY_PAGE, *PSCSIDATA_MODE_RW_ERR_RECOVERY_PAGE;

//Flexible Disk Page (0x05)
#define SCSIDATA_MODE_FLEXIBLE_DISK_PAGE_SIZE           0x20
typedef struct _SCSIDATA_MODE_FLEXIBLE_DISK_PAGE
{
    struct
    {
		//richie switch order
        UINT8   pageCode:6;
        UINT8   reserved:1;
        UINT8   PS:1;
    } BYTE0;

    UINT8   pageLength;
    UINT16  transferRate;
    UINT8   numberOfHeads;
    UINT8   sectorsPerTrack;
    UINT16  dataBytesPerSector;
    UINT16  numberOfCylinders;
    UINT8   reserved0[9];
    UINT8   motorOnDelay;
    UINT8   motorOffDelay;
    UINT8   reserved1[7];
    UINT16  mediumRotationRate;
    UINT8   reserved2[2];
} SCSIDATA_MODE_FLEXIBLE_DISK_PAGE, *PSCSIDATA_MODE_FLEXIBLE_DISK_PAGE;

//Removable Block Access Capabilities Page (0x1b)
#define SCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE_SIZE         0x0c
typedef struct _SCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE
{
    struct
    {
		//richie switch order
        UINT8   pageCode:6;
        UINT8   reserved:1;
        UINT8   PS:1;
    } BYTE0;

    UINT8   pageLength;

    struct
    {
		//richie switch order
        UINT8   reserved:6;
        UINT8   SRFP:1;
        UINT8   SFLP:1;
    } BYTE2;

    struct
    {
		//richie switch order
        UINT8   TLUN:3;
        UINT8   reserved:3;
        UINT8   SML:1;
        UINT8   NCD:1;
    } BYTE3;

    UINT8   reserved[8];
} SCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE, *PSCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE;

//Timer and Protect Page (0x1c)
#define SCSIDATA_MODE_TIMER_AND_PROTECT_PAGE_SIZE       0x08
typedef struct _SCSIDATA_MODE_TIMER_AND_PROTECT_PAGE
{
    struct
    {
	//richie
        UINT8   pageCode:6;
        UINT8   reserved:1;
        UINT8   PS:1;
    } BYTE0;

    UINT8   pageLength;
    UINT8   reserved0;

    struct
    {
	//richie
        UINT8   inactivityTimeMultiplier:4;
        UINT8   reserved:4;
    } BYTE3;

    struct
    {
	//richie
        UINT8   SWPP:1;
        UINT8   DISP:1;
        UINT8   reserved:6;
    } BYTE4;

    UINT8   reserved1[3];
} SCSIDATA_MODE_TIMER_AND_PROTECT_PAGE, *PSCSIDATA_MODE_TIMER_AND_PROTECT_PAGE;

//READ CAPACITY
#define SCSIDATA_READ_CAPACITY_SIZE                     0x08
typedef struct _SCSIDATA_READ_CAPACITY
{
    UINT32   lastLogicalBlockAddress;
    UINT32   blockLengthInBytes;
} SCSIDATA_READ_CAPACITY, *PSCSIDATA_READ_CAPACITY;

//READ FORMAT CAPACITY
#define SCSIDATA_READ_FORMAT_CAPACITY_LIST_HEADER_SIZE          0x04
typedef struct _SCSIDATA_READ_CAPACITY_LIST_HEADER
{
    UINT8   reserved0;
    UINT8   reserved1;
    UINT8   reserved2;
    UINT8   capacityListLength;
} SCSIDATA_READ_CAPACITY_LIST_HEADER, *PSCSIDATA_READ_CAPACITY_LIST_HEADER;


#define SCSIDATA_READ_FORMAT_MAXIMUM_SIZE                        0x08
typedef struct _SCSIDATA_READ_FORMAT_MAXIMUM
{
    UINT32   numberOfBlocks;

    struct
    {
		//richie switch order
        UINT8   descCode:2;
        UINT8   reserved:6;
	} BYTE4;
    
	UINT8   blockLength[3];
} SCSIDATA_READ_FORMAT_MAXIMUM, *PSCSIDATA_READ_FORMAT_MAXIMUM;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern  code    UINT8   K_VendorInformation[];
extern  code    UINT8   K_ProductIdentification[];
extern  code    UINT8   K_ProductRevisionLevel[]; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 SCSICMD_TestUnitReady(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_RezeroUnit(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_RequestSense(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_FormatUnit(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Inquiry(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_ModeSelect06(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_ModeSense06(UINT8* pSCSI, UINT32* pSize);
UINT32 SCSICMD_ModePage01(UINT8* pPage);
UINT32 SCSICMD_ModePage1b(UINT8* pPage);
UINT32 SCSICMD_ModePage1c(UINT8* pPage);
//patch4.5@richie@wp xp begin
UINT32 SCSICMD_Mode06ParameterHeader(UINT8* pPage, UINT32 size);
UINT32 SCSICMD_Mode10ParameterHeader(UINT8* pPage, UINT32 size);
//patch4.5@richie@wp xp end
UINT8 SCSICMD_StartStopUnit(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_SendDiagnostic(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_PreventAllowMediumRemoval(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_ReadCapacity(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Read10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Write10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Seek10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_WriteAndVerify10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Verify10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_ModeSelect10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_ModeSense10(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Read12(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSICMD_Write12(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSIDATA_FormatUnit(UINT8* pSCSI);
UINT8 SCSIDATA_ModeSelect06(UINT8* pSCSI);
UINT8 SCSIDATA_ModeSelect10(UINT8* pSCSI);
UINT8 SCSICMD_Read12(UINT8* pSCSI, UINT32* pSize);
UINT8 SCSIDATA_ReadFormatCapacities(UINT8* pSCSI, UINT32* pSize);

#endif /*__SCSI_H__*/
