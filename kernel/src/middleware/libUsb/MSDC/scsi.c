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
 * @file	  scsi.c
 * @brief	  Module related to SCSI command
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "usbopt.h"
#if (USBMSDC_OPTION == 2)
#include "scsi.h"
#include "sense.h"

//patch4.5@richie@vender info marked
//#include "scsistr.h"
#include "main.h"
#include "usbmsdc2.h"
#include "doslink.h"
//richie
#include "bulkout.h"
#include "ctlsetup.h"
#include "initio.h"
#include "setmode.h"
#include "sdram.h"
#include "cardlink.h" // WWW1

#include "dos32.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
xdata	 UINT8	 G_ScsiVendorInformation[0x0a]	=
{
		 'S',  'u',	 'n',  'p',	 'l',  'u',	 's', 0x20,
};

xdata	 UINT8	 G_ScsiProductIdentification[0x13] =
{
		 'S',  'u',	 'n',  'p',	 'l',  'u',	 's',  ' ',
		 'S',  'P',	 'C',  'A',	 '1',  'K',	 'C', 0x20,
};

xdata	 UINT8	 G_ScsiProductRevisionLevel[0x04] =
{
		 '1',  '.',	 '0',  '0',
};
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern	code	UINT8	K_ScsiVendorInformation[];
extern	code	UINT8	K_ScsiProductIdentification[];
extern	code	UINT8	K_ScsiProductRevisionLevel[];

extern xdata UINT16	G_Card_TotalSizeMB;
extern xdata UINT16	G_Card_BytePerSector;
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//-----------------------------------------------------------------------------
//SCSICMD_TestUnitReady
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_TestUnitReady(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Inquiry command (0x00)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_TestUnitReady(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}


	*pSize = 0;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_RezeroUnit
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_RezeroUnit(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Rezero command (0x01)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_RezeroUnit(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	*pSize = 0;

	return(TRUE);
}
#endif /*_DBG_CLR_WARNING_*/
//-----------------------------------------------------------------------------
//SCSICMD_RequestSense
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_RequestSense(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Request Sense command (0x03)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_RequestSense(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;
	PSCSIDATA_REQUEST_SENSE pRequestSense;
	UINT8 i;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;
	pRequestSense = (PSCSIDATA_REQUEST_SENSE) G_pucStorDataPointer;
	pRequestSense->BYTE0.valid = 0;
	pRequestSense->BYTE0.errorCode = 0x70;
	pRequestSense->reserved0 = 0;
	pRequestSense->BYTE2.reserved = 0;
	pRequestSense->BYTE2.senseKey = K_SCSISENSE[G_USBMSDC_ucSenseCodeIdx].senseKey;
	pRequestSense->information = 0;
	//richie
	//patch5.0@richie@sdram mapping begin
	pRequestSense->additionalSenseLength = 0x0a;
	//pRequestSense->additionalSenseLength = 0x12;
	//patch5.0@richie@sdram mapping end
	for (i = 0; i < 4; i++)
			pRequestSense->reserved1[i] = 0x00;
	pRequestSense->additionalSenseCode = K_SCSISENSE[G_USBMSDC_ucSenseCodeIdx].additionalSenseCode;
	pRequestSense->additionalSenseCodeQualifier = K_SCSISENSE[G_USBMSDC_ucSenseCodeIdx].additionalSenseCodeQualifier;
	for (i = 0; i < 4; i++)
			pRequestSense->reserved2[i] = 0x00;

	*pSize = SCSIDATA_REQUEST_SENSE_SIZE;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_FormatUnit
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_FormatUnit(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Rezero command (0x04)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_FormatUnit(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	*pSize = 0;

	return(TRUE);
}
#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//SCSICMD_Inquiry
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_Inquiry(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Inquiry command (0x12)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Inquiry(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;
	PSCSIDATA_INQUIRY pInquiry;
	UINT8 i;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;
	pInquiry = (PSCSIDATA_INQUIRY) G_pucStorDataPointer;
	pInquiry->BYTE0.reserved = 0;
	pInquiry->BYTE0.peripheralDeviceType = 0;
	pInquiry->BYTE1.RMB = 1;
	pInquiry->BYTE1.reserved = 0;
	pInquiry->BYTE2.isoVersion = 0;
	pInquiry->BYTE2.emcaVersion = 0;
	pInquiry->BYTE2.ansiVersion = 0;
	pInquiry->BYTE3.reserved = 0;
	//richie
	pInquiry->BYTE3.responseDataFormat = 1;
	//pInquiry->BYTE3.responseDataFormat = 0;
	pInquiry->additionalLength = 0x24;

	for (i = 0; i < 3; i++)
			pInquiry->reserved0[i] = 0x00;
//patch4.5@richie@vender info begin
	for (i = 0; i < 0x0a; i++)
			pInquiry->vendorInformation[i] = G_ScsiVendorInformation[i];
	for (i = 0; i < 0x13; i++)
			pInquiry->productIdentification[i] = G_ScsiProductIdentification[i];
	for (i = 0; i < 0x04; i++)
			pInquiry->productRevisionLevel[i] = G_ScsiProductRevisionLevel[i];
//patch4.5@richie@vender info end

	//richie@1129
	*pSize = SCSIDATA_INQUIRY_SIZE;

	return(SCSIDATA_INQUIRY_SIZE);
}

//-----------------------------------------------------------------------------
//SCSICMD_ModeSelect06
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_ModeSelect06(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Mode Select 06 command (0x15)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_ModeSelect06(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 status = TRUE;
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	*pSize = (UINT32) pSCSI06->dataLength;

	return(status);
}
#endif /*_DBG_CLR_WARNING_*/

//patch4.5@richie@wp xp begin
//-----------------------------------------------------------------------------
//SCSICMD_ModeSense06
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_ModeSense06(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Mode Sense 06 command (0x1a)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_ModeSense06(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 status = TRUE;
	PSCSICMD_06BYTE pSCSI06;
	UINT32 size = SCSIDATA_MODE06_PARAMETER_HEADER_SIZE;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	//patch4.5@richie@wp xp begin

	switch (pSCSI06->logicalBlockAddress & 0x3f00)
	{
		case 0x0100:	//Media Type and Write Protect Page
			size += SCSICMD_ModePage01(G_pucStorDataPointer + size);
			break;

		case 0x1b00:	//Removable Block Access Capacities Page
			size += SCSICMD_ModePage1b(G_pucStorDataPointer + size);
			break;

		case 0x1c00:	//Timer and Protect Page
			size += SCSICMD_ModePage1c(G_pucStorDataPointer + size);
			break;

		case 0x3f00:	//Return All Pages
			size += SCSICMD_ModePage01(G_pucStorDataPointer + size);
			size += SCSICMD_ModePage1b(G_pucStorDataPointer + size);
			size += SCSICMD_ModePage1c(G_pucStorDataPointer + size);
			break;

		default:
			status = FALSE;
			break;
	}
	//patch4.5@richie@wp xp end

	if (status)
	{
		SCSICMD_Mode06ParameterHeader(G_pucStorDataPointer, size);
		*pSize = size;
	}

	return(status);
}
//patch4.5@richie@wp xp end

//-----------------------------------------------------------------------------
//SCSICMD_ModePage01
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 SCSICMD_ModePage01(UINT8* pPage)
 * @brief	  process Mode Page 01
 * @param	  [in] pPage: pointer to SCSI command
 * @retval	  size of data processed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 SCSICMD_ModePage01(UINT8* pPage) USING_1
{
	PSCSIDATA_MODE_RW_ERR_RECOVERY_PAGE pPage01;

	pPage01 = (PSCSIDATA_MODE_RW_ERR_RECOVERY_PAGE) pPage;

	pPage01->BYTE0.PS = 0;
	pPage01->BYTE0.reserved = 0;
	pPage01->BYTE0.pageCode = 0x01;
	pPage01->pageLength = SCSIDATA_MODE_RW_ERR_RECOVERY_PAGE_SIZE - 2;
	pPage01->BYTE2.AWRE = 0;
	pPage01->BYTE2.reserved0 = 0;
	pPage01->BYTE2.RC = 0;
	pPage01->BYTE2.reserved1 = 0;
	pPage01->BYTE2.PER = 0;
	pPage01->BYTE2.reserved2 = 0;
	pPage01->BYTE2.DCR = 0;
	pPage01->readRetryCount = 3;
	pPage01->reserved0 = 0;
	pPage01->writeRetryCount = 3;
	pPage01->reserved1 = 0;

	return(SCSIDATA_MODE_RW_ERR_RECOVERY_PAGE_SIZE);
}

//-----------------------------------------------------------------------------
//SCSICMD_ModePage1b
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 SCSICMD_ModePage1b(UINT8* pPage)
 * @brief	  process Mode Page 1b
 * @param	  [in] pPage: pointer to SCSI command
 * @retval	  size of data processed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 SCSICMD_ModePage1b(UINT8* pPage) USING_1
{
	PSCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE pPage1b;
	UINT8 i;

	pPage1b = (PSCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE) pPage;

	pPage1b->BYTE0.PS = 0;
	pPage1b->BYTE0.reserved = 0;
	pPage1b->BYTE0.pageCode = 0x1b;
	pPage1b->pageLength = SCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE_SIZE - 2;
	pPage1b->BYTE2.SFLP = 0;
	pPage1b->BYTE2.SRFP = 0;
	pPage1b->BYTE2.reserved = 0;
	pPage1b->BYTE3.NCD = 0;
	pPage1b->BYTE3.SML = 0;
	pPage1b->BYTE3.reserved = 0;
	pPage1b->BYTE3.TLUN = 1;
	for (i = 0; i < 8; i++)
			pPage1b->reserved[i] = 0;

	return(SCSIDATA_MODE_REM_BLK_ACC_CAP_PAGE_SIZE);
}

//-----------------------------------------------------------------------------
//SCSICMD_ModePage1c
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 SCSICMD_ModePage1c(UINT8* pPage)
 * @brief	  process Mode Page 1c
 * @param	  [in] pPage: pointer to SCSI command
 * @retval	  size of data processed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 SCSICMD_ModePage1c(UINT8* pPage) USING_1
{
	PSCSIDATA_MODE_TIMER_AND_PROTECT_PAGE pPage1c;
	UINT8 i;

	pPage1c = (PSCSIDATA_MODE_TIMER_AND_PROTECT_PAGE) pPage;

	pPage1c->BYTE0.PS = 0;
	pPage1c->BYTE0.reserved = 0;
	pPage1c->BYTE0.pageCode = 0x1c;
	pPage1c->pageLength = SCSIDATA_MODE_TIMER_AND_PROTECT_PAGE_SIZE - 2;
	pPage1c->reserved0 = 0;
	pPage1c->BYTE3.reserved = 0;
	pPage1c->BYTE3.inactivityTimeMultiplier = 0x01;
	pPage1c->BYTE4.reserved = 0;
	pPage1c->BYTE4.DISP = 0;
	pPage1c->BYTE4.SWPP = 0;
	for (i = 0; i < 3; i++) {
		pPage1c->reserved1[i] = 0;
	}
	return(SCSIDATA_MODE_TIMER_AND_PROTECT_PAGE_SIZE);
}
//patch4.5@richie@wp xp begin
//-----------------------------------------------------------------------------
//SCSICMD_Mode06ParameterHeader
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 SCSICMD_Mode06ParameterHeader(UINT8* pPage, UINT32 size)
 * @brief	  process Mode Page 1c
 * @param	  [in] pPage: pointer to SCSI command
 * @param	  [in] size: size of data processed
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 SCSICMD_Mode06ParameterHeader(UINT8* pPage, UINT32 size) USING_1
{
	PSCSIDATA_MODE06_PARAMETER_HEADER pHeader;
	//UINT8 i;

	pHeader = (PSCSIDATA_MODE06_PARAMETER_HEADER) pPage;

	pHeader->modeDataLength = (UINT8)(size - SCSIDATA_MODE06_PARAMETER_HEADER_SIZE);
	//printf("pHeader->modeDataLength = %bx\n",pHeader->modeDataLength);

	pHeader->mediumTypeCode = 0x00;
	if (G_USBMSDC_ucCardLock == 1)
	{
		pHeader->BYTE3.WP = 1;
	}
	else
	{
		pHeader->BYTE3.WP = 0;
	}

	pHeader->BYTE3.reserved0 = 0;
	pHeader->BYTE3.DPOFUA = 0;
	pHeader->BYTE3.reserved1 = 0;
	//richie
	pHeader->reserved = 0;

	//printf("Pages06=");
	//for (i = 0;i < 10;i++)
	//printf("%2bx ",G_pucStorDataPointer[i]);
	//printf("\n");

	return(SCSIDATA_MODE06_PARAMETER_HEADER_SIZE);
}

//-----------------------------------------------------------------------------
//SCSICMD_Mode10ParameterHeader
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT32 SCSICMD_Mode10ParameterHeader(UINT8* pPage, UINT32 size)
 * @brief	  process Mode Page 1c
 * @param	  [in] pPage: pointer to SCSI command
 * @param	  [in] size
 * @retval	  size of data processed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 SCSICMD_Mode10ParameterHeader(UINT8* pPage, UINT32 size) USING_1
{
	PSCSIDATA_MODE10_PARAMETER_HEADER pHeader;
	UINT8 i;

	pHeader = (PSCSIDATA_MODE10_PARAMETER_HEADER) pPage;

	pHeader->modeDataLength = M_ByteSwapOfWord((UINT16)(size - SCSIDATA_MODE10_PARAMETER_HEADER_SIZE));
	//printf("pHeader->modeDataLength = %x\n",pHeader->modeDataLength);

	pHeader->mediumTypeCode = 0x00;
	if (G_USBMSDC_ucCardLock == 1)
	{
		pHeader->BYTE3.WP = 1;
	}
	else
	{
		pHeader->BYTE3.WP = 0;
	}

	pHeader->BYTE3.reserved0 = 0;
	pHeader->BYTE3.DPOFUA = 0;
	pHeader->BYTE3.reserved1 = 0;
	//richie
	for (i = 0; i < 4; i++) {
		pHeader->reserved[i] = 0;
	}
	//printf("Pages10=");
	//for (i = 0;i < 10;i++)
	//printf("%2bx ",G_pucStorDataPointer[i]);
	//printf("\n");

	return(SCSIDATA_MODE10_PARAMETER_HEADER_SIZE);
}
//patch4.5@richie@wp xp end

//-----------------------------------------------------------------------------
//SCSICMD_StartStopUnit
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_StartStopUnit(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Start Stop Unit command (0x1b)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_StartStopUnit(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 operation;
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;
	operation = (pSCSI06->dataLength) & 0x03;
	if ( operation == 0x00 || operation == 0x02){
		G_USBMSDC_ucEject = 1;
		if (G_ucMSDC_MLUN)
			{
				G_MLUN_Media_ucEject[G_MLUN_Media_Pos_Next] = 1;
			}
	}

	*pSize = 0;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_SendDiagnostic
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_SendDiagnostic(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Send Diagnostic command (0x1d)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_SendDiagnostic(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	*pSize = 0;

	return(TRUE);
}
#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//SCSICMD_PreventAllowMediumRemoval
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_PreventAllowMediumRemoval(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Prevent Allow Medium Removal command (0x1e)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_PreventAllowMediumRemoval(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_06BYTE pSCSI06;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	*pSize = 0;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_ReadCapacity
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_ReadCapacity(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Read Capacity command (0x25)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_ReadCapacity(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;
	PSCSIDATA_READ_CAPACITY pReadCapacity;

	//printf("G_ucStorageType=%bx,G_Card_Type=%bx\n",G_ucStorageType,G_Card_Type);

	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		//printf("G_ucStorageType != G_Card_Type\n");
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}


	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;
	pReadCapacity = (PSCSIDATA_READ_CAPACITY) G_pucStorDataPointer;
	MLUN_CheckCard_Intr();
	//printf("G_Card_TotalSizeMB=%d,G_Card_BytePerSector=%d,G_Card_BytePerSector=%d\n",G_Card_TotalSizeMB,G_Card_BytePerSector,G_Card_BytePerSector);
	if (G_ucStorageType == K_MEDIA_NANDF) {
		#if (DOS_RSV_PART == 1)
		if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
			G_Card_TotalSizeMB = (UINT16)(DOS_TotalSize(DRIVE_NAND_RSV)>>10);
		}
		else {
			G_Card_TotalSizeMB = (UINT16)(DOS_TotalSize(DRIVE_NAND)>>10);
		}
		#else
		G_Card_TotalSizeMB = (UINT16)(DOS_TotalSize(DRIVE_NAND)>>10);
		#endif
	}
	else if (G_ucStorageType == K_MEDIA_SDF) {
		G_Card_TotalSizeMB = (UINT16)(DOS_TotalSize(DRIVE_SD)>>10);
	}
	#if ( DOSRAM_OPTION == 1 )
	else if (G_ucStorageType == K_MEDIA_DOSRAM) {
		G_Card_TotalSizeMB = (UINT16)(DOS_TotalSize(DRIVE_DRAM)>>10);
	}
	#endif
	if ((UINT32) G_Card_TotalSizeMB < 2048) {
		pReadCapacity->lastLogicalBlockAddress =
				(((UINT32) G_Card_TotalSizeMB << 20) / G_Card_BytePerSector) - 1;
	}
	else {
		pReadCapacity->lastLogicalBlockAddress =
				((((UINT32) G_Card_TotalSizeMB << 10) / G_Card_BytePerSector) << 10) - 1;
		// if G_Card_TotalSizeMB is more than 4GB, ((UINT32) G_Card_TotalSizeMB << 20) will overflow,and lastLogicalBlockAddress will be err.
	}
	pReadCapacity->blockLengthInBytes =
			(UINT32) G_Card_BytePerSector;

	*pSize = SCSIDATA_READ_CAPACITY_SIZE;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_Read10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_Read10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Read 10 command (0x28)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Read10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;



	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}
	//richie@1211
	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI10->logicalBlockAddress; /* Solve 4G limitation */ //*(UINT32) G_Card_BytePerSector;
	//richie@1211
	*pSize = (UINT32) pSCSI10->dataLength * (UINT32) G_Card_BytePerSector;

	//richie@1219
	//patch5.0@richie@sdram mapping
	//G_USBMSDC_ulSdramBufferAddress = K_SDRAM_FrameBufAAddr;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_Write10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_Write10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Write 10 command (0x2a)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Write10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	//DbgPrint("G_ucStorageType=%d\n",(WORD)G_ucStorageType);
	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI10->logicalBlockAddress; // * (UINT32) G_Card_BytePerSector;

	*pSize = (UINT32) pSCSI10->dataLength * (UINT32) G_Card_BytePerSector;

	//patch5.0@richie@sdram mapping
	//G_USBMSDC_ulSdramBufferAddress = K_SDRAM_FrameBufAAddr;


	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_Seek10
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_Seek10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Seek 10 command (0x2b)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Seek10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI10->logicalBlockAddress; // * (UINT32) G_DOS_SectorSize;

	*pSize = 0;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_WriteAndVerify10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_WriteAndVerify10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Write And Verify 10 command (0x2e)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_WriteAndVerify10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI10->logicalBlockAddress; // * (UINT32) G_DOS_SectorSize;

	*pSize = (UINT32) pSCSI10->dataLength * (UINT32) G_DOS_SectorSize;


	return(TRUE);
}
#endif /*_DBG_CLR_WARNING_*/
//-----------------------------------------------------------------------------
//SCSICMD_Verify10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_Verify10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Verify 10 command (0x2f)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Verify10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI10->logicalBlockAddress; // * (UINT32) G_DOS_SectorSize;

	*pSize = 0;

	return(TRUE);
}

//-----------------------------------------------------------------------------
//SCSICMD_ModeSelect10
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSICMD_ModeSelect10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Mode Select 10 command (0x55)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_ModeSelect10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 status = TRUE;
	PSCSICMD_10BYTE pSCSI10;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	*pSize = pSCSI10->dataLength;

	return(status);
}
#endif /*_DBG_CLR_WARNING_*/

//patch4.5@richie@wp xp begin
//-----------------------------------------------------------------------------
//SCSICMD_ModeSense10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_ModeSense10(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Mode Sense 10 command (0x5a)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_ModeSense10(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 status = TRUE;
	PSCSICMD_10BYTE pSCSI10;
	UINT32 size = SCSIDATA_MODE10_PARAMETER_HEADER_SIZE;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;


	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}


	switch (pSCSI10->logicalBlockAddress & 0x3f000000)
	{
		case 0x01000000:		//Media Type and Write Protect Page
			size += SCSICMD_ModePage01(G_pucStorDataPointer + size);
			break;

		case 0x1b000000:		//Removable Block Access Capacities Page
			size += SCSICMD_ModePage1b(G_pucStorDataPointer + size);
			break;

		case 0x1c000000:		//Timer and Protect Page
			size += SCSICMD_ModePage1c(G_pucStorDataPointer + size);
			break;

		case 0x3f000000:	//Return All Pages
			size += SCSICMD_ModePage01(G_pucStorDataPointer + size);
			size += SCSICMD_ModePage1b(G_pucStorDataPointer + size);
			size += SCSICMD_ModePage1c(G_pucStorDataPointer + size);
			break;

		default:
			status = FALSE;
			break;
	}

	if (status)
	{
		SCSICMD_Mode10ParameterHeader(G_pucStorDataPointer, size);
		*pSize = size;
	}

	return(status);
}
//patch4.5@richie@wp xp end
//-----------------------------------------------------------------------------
//SCSICMD_Read12
//-----------------------------------------------------------------------------
//
/**
 * @fn		  UINT8 SCSICMD_Read12(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Read 12 command (0xa8)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Read12(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_12BYTE pSCSI12;

	pSCSI12 = (PSCSICMD_12BYTE) pSCSI;

	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI12->logicalBlockAddress; // * (UINT32) G_DOS_SectorSize;

	*pSize = (UINT32) pSCSI12->dataLength * (UINT32) G_Card_BytePerSector;

	return(TRUE);
}

//#ifndef _DBG_CLR_WARNING_

//-----------------------------------------------------------------------------
//SCSICMD_Write12
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSICMD_Write12(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Write 12 command (0xaa)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: data size processed
 * @retval	  valid command or not
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSICMD_Write12(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_12BYTE pSCSI12;

	pSCSI12 = (PSCSICMD_12BYTE) pSCSI;

	//richie@1203 if no medium then set sensekey = 02
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);
	}

	G_USBMSDC_ulLogicalBlockAddress = (UINT32) pSCSI12->logicalBlockAddress; // * (UINT32) G_DOS_SectorSize;

	*pSize = (UINT32) pSCSI12->dataLength * (UINT32) G_Card_BytePerSector;


	//patch5.0@richie@sdram mapping
	//				  G_USBMSDC_ulSdramBufferAddress = K_SDRAM_FrameBufAAddr;


	return(TRUE);
}
//#endif /*_DBG_CLR_WARNING_*/
//-----------------------------------------------------------------------------
//SCSIDATA_FormatUnit
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSIDATA_FormatUnit(UINT8* pSCSI)
 * @brief	  process Format Unit Data (0x04)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @retval	  success or failure of action performed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSIDATA_FormatUnit(UINT8* pSCSI) USING_1
{
	PSCSICMD_06BYTE pSCSI06;
	PSCSIDATA_FORMAT_UNIT pFormatUnit;
	UINT8 status = TRUE;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	//G_ulStorDataLength is the size required
	//G_ulStorDataIndex is the size received
	pFormatUnit = (PSCSIDATA_FORMAT_UNIT) G_pucStorDataPointer;

	//diskFormat(); //????

	return(status);
}

//-----------------------------------------------------------------------------
//SCSIDATA_ModeSelect06
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSIDATA_ModeSelect06(UINT8* pSCSI)
 * @brief	  process Mode Select 06 Data (0x15)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @retval	  success or failure of action performed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSIDATA_ModeSelect06(UINT8* pSCSI) USING_1
{
	PSCSICMD_06BYTE pSCSI06;
	//patch4.5@richie@wp xp
	PSCSIDATA_MODE06_PARAMETER_HEADER pHeader;
	UINT8 status = TRUE;

	pSCSI06 = (PSCSICMD_06BYTE) pSCSI;

	//G_ulStorDataLength is the size required
	//G_ulStorDataIndex is the size received
	//patch4.5@richie@wp xp
	pHeader = (PSCSIDATA_MODE06_PARAMETER_HEADER) G_pucStorDataPointer;

	return(status);
}

//-----------------------------------------------------------------------------
//SCSIDATA_ModeSelect10
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 SCSIDATA_ModeSelect10(UINT8* pSCSI)
 * @brief	  process Mode Select 10 Data (0x55)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @retval	  success or failure of action performed
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSIDATA_ModeSelect10(UINT8* pSCSI) USING_1
{
	PSCSICMD_10BYTE pSCSI10;
//patch4.5@richie@wp xp
	PSCSIDATA_MODE10_PARAMETER_HEADER pHeader;
	UINT8 status = TRUE;

	pSCSI10 = (PSCSICMD_10BYTE) pSCSI;

	//G_ulStorDataLength is the size required
	//G_ulStorDataIndex is the size received
//patch4.5@richie@wp xp
	pHeader = (PSCSIDATA_MODE10_PARAMETER_HEADER) G_pucStorDataPointer;

	return(status);
}

//richie@1129
//-----------------------------------------------------------------------------
//SCSIDATA_ReadFormatCapacities
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn		  UINT8 SCSIDATA_ReadFormatCapacities(UINT8* pSCSI, UINT32* pSize)
 * @brief	  process Read format capacities (0x23)
 * @param	  [in] pSCSI: pointer to SCSI command
 * @param	  [in] pSize: success or failure of action performed
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 SCSIDATA_ReadFormatCapacities(UINT8* pSCSI, UINT32* pSize) USING_1
{
	PSCSICMD_12BYTE pSCSI12;
	PSCSIDATA_READ_FORMAT_MAXIMUM pMax;
	PSCSIDATA_READ_CAPACITY_LIST_HEADER pHeader;
	UINT16 allocLength;

	pSCSI12 = (PSCSICMD_12BYTE) pSCSI;

	allocLength = (UINT8)pSCSI[7];
	allocLength = allocLength << 8;
	allocLength |= (UINT8)pSCSI[8];

			//DbgPrint("alloclen %d\n",allocLength);

	pHeader = (PSCSIDATA_READ_CAPACITY_LIST_HEADER) G_pucStorDataPointer;


	//richei@1203 if no CF in device return zero length data
	//*pSize = 0;
	//return TRUE;

	//richie@1129 if no CF in device
	if (G_ucStorageType != G_Card_Type)
	{
		pHeader->reserved0 = 0;
		pHeader->reserved1 = 0;
		pHeader->reserved2 = 0;
		pHeader->capacityListLength = SCSIDATA_READ_FORMAT_MAXIMUM_SIZE;
		pMax = (PSCSIDATA_READ_FORMAT_MAXIMUM) (G_pucStorDataPointer + SCSIDATA_READ_FORMAT_CAPACITY_LIST_HEADER_SIZE);
		pMax->numberOfBlocks = 0;  //richie ???? not sure
		pMax->BYTE4.reserved = 3;
		pMax->BYTE4.descCode = 0;  //richie no media
		pMax->blockLength[0] = 0;
		pMax->blockLength[1] = 2;
		pMax->blockLength[2] = 0;  //richie ???? not sure
	}
	else
	{
	   //richie@1211 not finish yet
	}

	G_pucStorDataPointer[0] = 0x00;
	G_pucStorDataPointer[1] = 0x00;
	G_pucStorDataPointer[2] = 0x00;
	G_pucStorDataPointer[3] = 0x10;
	G_pucStorDataPointer[4] = 0x00;
	G_pucStorDataPointer[5] = 0x07;
	G_pucStorDataPointer[6] = 0x78;
	G_pucStorDataPointer[7] = 0x3c;
	G_pucStorDataPointer[8] = 0x03;
	G_pucStorDataPointer[9] = 0x00;
	G_pucStorDataPointer[10] = 0x02;
	G_pucStorDataPointer[11] = 0x00;
	G_pucStorDataPointer[12] = 0x00;
	G_pucStorDataPointer[13] = 0x07;
	G_pucStorDataPointer[14] = 0x78;
	G_pucStorDataPointer[15] = 0x3c;
	G_pucStorDataPointer[16] = 0x00;
	G_pucStorDataPointer[17] = 0x00;
	G_pucStorDataPointer[18] = 0x02;
	G_pucStorDataPointer[19] = 0x00;
	*pSize = 20;
	return 20;


	*pSize = SCSIDATA_READ_FORMAT_CAPACITY_LIST_HEADER_SIZE + SCSIDATA_READ_FORMAT_MAXIMUM_SIZE;

	return(SCSIDATA_READ_FORMAT_CAPACITY_LIST_HEADER_SIZE + SCSIDATA_READ_FORMAT_MAXIMUM_SIZE);
}

#endif /*_DBG_CLR_WARNING_*/

#endif
