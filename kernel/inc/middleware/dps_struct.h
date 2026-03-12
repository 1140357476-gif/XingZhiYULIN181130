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
 * @file     dps_struct.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DPS_STRUCT__
#define __DPS_STRUCT__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h" 

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DPS_PARA_Max_Size	256 
#define DPS_PARA_ConfigPrintService_Size (sizeof(DPS_PARA_ConfigPrintService))
#define DPS_PARA_Capability_Elem_MaxCount 	10
#define DPS_PARA_Capability_Elem_Size	(sizeof(DPS_PARA_Capability_Elem))
#define DPS_PARA_ConfigStartJob_Size (sizeof(DPS_PARA_ConfigStartJob))
#define DPS_PARA_ConfigPrintInfoMaxCount 24
#define DPS_PARA_ConfigPrintInfoMaxCountPerPrint 4
#define DPS_PARA_ConfigPrintInfo_Size (sizeof(DPS_PARA_ConfigPrintInfo))
#define DPS_PARA_ConfigPrintInfo_Size_Min (DPS_PARA_ConfigPrintInfo_Size-10)
#define DPS_PARA_JobStatus_Size  (sizeof(DPS_PARA_JobStatus))
#define DPS_PARA_DeviceStatus_Size  (sizeof(DPS_PARA_DeviceStatus))

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


////////////////////////////////////////////////////////
// ConfigPrintService

typedef struct 
{
	UINT32 printServiceAvailable;
	UINT8 dpsVersions[ 9 ];
  UINT8 vendorName[ 65 ];
  UINT8 vendorSpecVer[ 9 ];
  UINT8 productName[ 65 ];
  UINT8 serialNo[ 65 ];
  UINT8 reserved;
} DPS_PARA_ConfigPrintService,* PDPS_PARA_ConfigPrintService;

//////////////////////////////////////////////////////////
// print capability


#if 0
#define DPS_CAPA_QUALITY_MAX_NUM          	4
#define DPS_CAPA_PAPER_SIZE_MAX_NUM       	24
#define DPS_CAPA_PAPER_TYPE_MAX_NUM       	4
#define DPS_CAPA_FILE_TYPE_MAX_NUM        	24
#define DPS_CAPA_DATE_PRINT_MAX_NUM       	3
#define DPS_CAPA_FILE_NAME_PRINT_MAX_NUM  	3
#define DPS_CAPA_IMAGE_OPTIMIZE_MAX_NUM   	6
#define DPS_CAPA_LAYOUT_MAX_NUM           	8
#define DPS_CAPA_FIXED_SIZE_MAX_NUM       	20
#define DPS_CAPA_CROPPING_MAX_NUM         	3

#define DPS_PARA_Capability_Qualities_Size			((DPS_CAPA_QUALITY_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_PaperSizes_Size			((DPS_CAPA_PAPER_SIZE_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_PaperTypes_Size			((DPS_CAPA_PAPER_TYPE_MAX_NUM <<2)+2)
#define DPS_PARA_Capability_FileTypes_Size			((DPS_CAPA_FILE_TYPE_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_DatePrints_Size			((DPS_CAPA_DATE_PRINT_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_FileNamePrints_Size		((DPS_CAPA_FILE_NAME_PRINT_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_ImageOptimizes_Size		((DPS_CAPA_IMAGE_OPTIMIZE_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_Layouts_Size				((DPS_CAPA_LAYOUT_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_FixedSizes_Size			((DPS_CAPA_FIXED_SIZE_MAX_NUM<<2)+2)
#define DPS_PARA_Capability_Croppings_Size			((DPS_CAPA_CROPPING_MAX_NUM<<2)+2)

#define DPS_PARA_Capability_Size		(DPS_PARA_Capability_Qualities_Size+ \
									DPS_PARA_Capability_PaperSizes_Size+ \
									DPS_PARA_Capability_PaperTypes_Size+ \
									DPS_PARA_Capability_FileTypes_Size+ \
									DPS_PARA_Capability_DatePrints_Size+ \
									DPS_PARA_Capability_FileNamePrints_Size+ \
									DPS_PARA_Capability_ImageOptimizes_Size+ \
									DPS_PARA_Capability_Layouts_Size+ \
									DPS_PARA_Capability_FixedSizes_Size+ \
									DPS_PARA_Capability_Croppings_Size)
#endif


typedef struct 
{
	UINT16 count;
	UINT32 paperSize;
	UINT32 values[DPS_PARA_Capability_Elem_MaxCount];
}DPS_PARA_Capability_Elem,*PDPS_PARA_Capability_Elem;

#if 0
#define DPS_PARA_Capability_ElemArray_MaxCount	 DPS_PARA_Capability_Elem_MaxCount	
#define DPS_PARA_Capability_ElemArray_Size	(sizeof(DPS_PARA_Capability_ElemArray))
typedef struct _DPS_PARA_Capability_ElemArray
{
	UINT16 count;
	DPS_PARA_Capability_Elem elem[DPS_PARA_Capability_ElemArray_MaxCount];
}DPS_PARA_Capability_ElemArray,*PDPS_PARA_Capability_ElemArray;
#endif

/////////////////////////////////////////////////
// start job
typedef struct 
{
	UINT32 quality;
	UINT32 paperSize;
	UINT32 paperType;
	UINT32 fileType;
	UINT32 datePrint;
	UINT32 fileNamePrint;
	UINT32 imageOptimize;
	UINT32 layout ;
	UINT32 fixedSize;
	UINT32 cropping;
}DPS_PARA_ConfigStartJob,*PDPS_PARA_ConfigStartJob;


typedef struct 
{
	UINT16 croppingArea_x;
	UINT16 croppingArea_y;
	UINT16 croppingArea_w;
	UINT16 croppingArea_h;
	UINT32 fileID;
	UINT16 copies;				/* dex 999 Max */
	UINT8 date[25];				/* buffer size 24 + 1 start of mem alloc */
	UINT8 fileName[25];			/* buffer size 24 + 1 share the mem with date */
	UINT32 prtPID;				/* !!! DPOF restart only 3-digits */
	UINT32 copyID;				/* !!! DPOF restart only 3-digits */	
	UINT8 filePath[1];
	UINT8 inValid;
}DPS_PARA_ConfigPrintInfo,*PDPS_PARA_ConfigPrintInfo;

/////////////////////////////////////////////////////////
// JobStatus Data 

typedef struct  {
	UINT8 prtPID[4];
	UINT8 filePath[64];
	UINT8 copyID[4];
	UINT8 progress[16];
	UINT8 imagesPrinted[8];
} DPS_PARA_JobStatus,*PDPS_PARA_JobStatus;

///////////////////////////////////////////////////////////
// DeviceStatus Data 

typedef struct  {
	UINT32 printServiceStatus;
	UINT32 jobEndReason;
	UINT32 errorStatus;
	UINT32 errorReason;
	UINT32 disconnectEnable;
	UINT32 capabilityChanged;
	UINT32 newJobOK;
} DPS_PARA_DeviceStatus,*PDPS_PARA_DeviceStatus; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
typedef enum  
{
	//DPS_PARAM_SidcSurportImageCountMax=0,
	DPS_PARAM_DpsStatus,
	DPS_PARAM_AddFileCountMax,
	DPS_PARAM_PrintFileCountMax,
	DPS_PARAM_MAX,
}dps_param_t;

typedef struct{
	DPS_PARA_ConfigPrintService printService;
	DPS_PARA_Capability_Elem capability[10];
	DPS_PARA_ConfigStartJob startJob;
	DPS_PARA_JobStatus jobStatus;
	DPS_PARA_DeviceStatus deviceStatus;
	//UINT16 paramCount;
	//UINT32 paramList[DPS_PARAM_MAX];
	UINT16 action;
	UINT32 dataStatus;
	UINT16 imageIndex;
	UINT16 imageCount;
	UINT16 imageMax;
	UINT16 imageListMax;
	UINT16 imageStep;	
	//UINT16 imageAll;
	UINT32 imageHandles;
	DPS_PARA_ConfigPrintInfo imageList[100];
}dps_data_t;

#endif
