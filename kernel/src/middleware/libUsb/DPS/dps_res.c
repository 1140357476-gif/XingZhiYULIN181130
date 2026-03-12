//#include "sidcdef.h"
#include "dps_func.h"

//get name from handle, or get handle from name
//the len of name is 13,example of "HREQUEST.DPS" 
void dpsGetScriptInfo(UINT32 *handle,UINT8 *name)
{
#if (USBSIDC_OPTION==2)
	code UINT8 names[][13]=
	{
		{"HREQUEST.DPS"},
		{"HRSPONSE.DPS"},
		{"DREQUEST.DPS"},
		{"DRSPONSE.DPS"},
		{"HDISCVRY.DPS"},
		{"DDISCVRY.DPS"},
	} ;
	
	if(*handle==0x00000000)
	{
		if (strcmp(name,names[0]) == 0)
		{
			*handle = DPS_HREQUEST_HND;
		}
		else if (strcmp(name,names[1]) == 0)
		{
			*handle = DPS_HRSPONSE_HND;
		}
		else if (strcmp(name,names[2]) == 0)
		{
			*handle = DPS_DREQUEST_HND;
		}
		else if (strcmp(name,names[3]) == 0)
		{
			*handle = DPS_DRSPONSE_HND;
		}
		else if (strcmp(name,names[4]) == 0)
		{
			*handle = DPS_HDISCVRY_HND;
			
		}
		else if (strcmp(name,names[5]) == 0)
		{
			*handle = DPS_DDISCVRY_HND;
			
		}
		else
		{
		}
	}
	else
	{
		if (*handle == DPS_HREQUEST_HND)
		{
			strcpy(name,names[0]);
		}
		else if (*handle == DPS_HRSPONSE_HND)
		{
			strcpy(name,names[1]);
		}
		else if (*handle == DPS_DREQUEST_HND)
		{
			strcpy(name,names[2]);
		}
		else if (*handle == DPS_DRSPONSE_HND)
		{
			strcpy(name,names[3]);
			}
		else if (*handle == DPS_HDISCVRY_HND)
		{
			strcpy(name,names[4]);
			
		}
		else if (*handle == DPS_DDISCVRY_HND)
		{
			strcpy(name,names[5]);
			
		}
		else
		{
		}
	}
	dpsPrintf("%lx--%s\n",*handle,name);
#else
	handle = handle;
	name = name;
#endif
}
#if USBSIDC_OPTION == 2

void dpsGetPrintConfigData( UINT8 index, UINT8* name ) 
{
	code UINT8 dpsVersions[] = "1.0";
	code UINT8 vendorName[] = "ICatchTek";
	code UINT8 vendorSpecVer[] = "1.0";
	code UINT8 productName[] = "SPCA1628";
	code UINT8 serialNo[] = "S0000001";

	code UINT8* list[] = {dpsVersions,vendorName,vendorSpecVer,productName,serialNo};
	UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  	memset(name,0,10);

	//printf("listCount:%bx\n",listCount);
	if(index<listCount)
		strcpy( name, list[index]);
}
#if 0
UINT32 dps_getCapabilityItem( UINT8 *itemflag, UINT32 *addr)
{
	UINT8 sizes[]=
		{
		DPS_PARA_Capability_Qualities_Size,
		DPS_PARA_Capability_PaperSizes_Size,
		DPS_PARA_Capability_PaperTypes_Size,
		DPS_PARA_Capability_FileTypes_Size,
		DPS_PARA_Capability_DatePrints_Size	,
		DPS_PARA_Capability_FileNamePrints_Size	,
		DPS_PARA_Capability_ImageOptimizes_Size,
		DPS_PARA_Capability_Layouts_Size,
		DPS_PARA_Capability_FixedSizes_Size,
		DPS_PARA_Capability_Croppings_Size
		};
#if 0 //mark by phil@2009.01.09
	UINT32 addrs[]=
		{
		K_DPS_ADDR_Capatility_Qualities,
		K_DPS_ADDR_Capatility_PaperSizes,
		K_DPS_ADDR_Capatility_PaperTypes,
		K_DPS_ADDR_Capatility_FileTypes	,
		K_DPS_ADDR_Capatility_DatePrints	,
		K_DPS_ADDR_Capatility_FileNamePrints,
		K_DPS_ADDR_Capatility_ImageOptimizes,
		K_DPS_ADDR_Capatility_Layouts	,
		K_DPS_ADDR_Capatility_FixedSizes,
		K_DPS_ADDR_Capatility_Croppings	
		};
#else
	UINT32 addrs[10];
#endif
	UINT32 items[]=
		{
		DPS_STATUS_GetCapability_Qualities	,
		DPS_STATUS_GetCapability_PaperSizes	,
		DPS_STATUS_GetCapability_PaperTypes	, 
		DPS_STATUS_GetCapability_FileTypes	,
		DPS_STATUS_GetCapability_DatePrints	,
		DPS_STATUS_GetCapability_FileNamePrints	,
		DPS_STATUS_GetCapability_ImageOptimizes	,
		DPS_STATUS_GetCapability_Layouts	,
		DPS_STATUS_GetCapability_FixedSizes	,
		DPS_STATUS_GetCapability_Croppings	
		};
	UINT8 i;

	/* add by phil@2009.01.09 s */
	addrs[0] = K_DPS_ADDR_Capatility_Qualities;
	addrs[1] = K_DPS_ADDR_Capatility_PaperSizes;
	addrs[2] = K_DPS_ADDR_Capatility_PaperTypes;
	addrs[3] = K_DPS_ADDR_Capatility_FileTypes;
	addrs[4] = K_DPS_ADDR_Capatility_DatePrints;
	addrs[5] = K_DPS_ADDR_Capatility_FileNamePrints;
	addrs[6] = K_DPS_ADDR_Capatility_ImageOptimizes;
	addrs[7] = K_DPS_ADDR_Capatility_Layouts;
	addrs[8] = K_DPS_ADDR_Capatility_FixedSizes;
	addrs[9] = K_DPS_ADDR_Capatility_Croppings;	
	/* add by phil@2009.01.09 e */

	if(*itemflag>DPS_STATUS_GetCapability && *itemflag<=DPS_STATUS_GetCapability_Croppings)
	{
		for(i=0;i<10;i++)
			if(*itemflag == items[i])
			{
				*addr = addrs[i];
				return sizes[i];
			}
	}
	if(*addr>K_DPS_ADDR_Capatility && *addr<=K_DPS_ADDR_Capatility_Croppings)
		for(i=0;i<10;i++)
			if(*addr == addrs[i])
			{
				*itemflag = items[i];
				return sizes[i];
			}
	return 0;
}
#endif
UINT8 xmlGetGeneralName( UINT8 index, UINT8* name ) 
{
	code UINT8 header[] = "<?xml version=\"1.0\"?><dps xmlns=\"http://www.cipa.jp/dps/schema/\">"; // 0
	code UINT8 input[] = "input";  // 1
	code UINT8 output[] = "output";  // 2
	code UINT8 result[] = "result"; // 3
	code UINT8 dps[] = "dps"; // 4
	
	code UINT8* list[] = {header,input,output,result,dps};
	UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));

  	if( name != NULL ) {
	  if(index<listCount)
	    strcpy( name, list[index]);
	}
	return strlen( list[ index ] );
}
void xmlGetCpsItemName( UINT8 index, UINT8* name ) {
  code UINT8 configureService[] = "configurePrintService";  // 0
  code UINT8 printServiceAvaiable[] = "printServiceAvailable"; // 1
  code UINT8 dpsVersion[] = "dpsVersions"; // 2
  code UINT8 vendorName[] = "vendorName"; // 3
  code UINT8 vendorSpecificVersion[] = "vendorSpecificVersion"; // 4
  code UINT8 productName[] = "productName"; // 5
  code UINT8 serialNo[] = "serialNo"; // 6
  
  code UINT8* list[] = {
    configureService,
    printServiceAvaiable,
    dpsVersion,
    vendorName,
    vendorSpecificVersion,
    productName, 
    serialNo};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}

void xmlGetJobConfigItemName( UINT8 index, UINT8* name ) {
  code UINT8 startJob[] = "startJob";  //0
  code UINT8 jobConfig[] = "jobConfig"; // 1
  code UINT8 quality[] = "quality";  // 2
  code UINT8 paperSize[] = "paperSize"; // 3
  code UINT8 paperType[] = "paperType"; // 4
  code UINT8 fileType[] = "fileType"; // 5 
  code UINT8 datePrint[] = "datePrint"; // 6 
  code UINT8 fileNamePrint[] = "fileNamePrint"; // 7
  code UINT8 imageOptimize[] = "imageOptimize"; // 8
  code UINT8 layout[] = "layout"; // 9
  code UINT8 fixedSize[] = "fixedSize"; // 10
  code UINT8 cropping[] = "cropping"; // 11
  
  code UINT8* list[] = {
    startJob,
    jobConfig,
    quality,
    paperSize,
    paperType,
    fileType, 
    datePrint,
    fileNamePrint,
    imageOptimize,
    layout,
    fixedSize,
    cropping};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}

void xmlGetSinglePrintInfoItemName( UINT8 index, UINT8* name ) 
	{
  code UINT8 printInfo[] = "printInfo"; // 0
  code UINT8 croppingArea[] = "croppingArea"; // 1
  code UINT8 fileID[] = "fileID"; // 2
  code UINT8 copies[] = "copies"; // 3
  code UINT8 prtPID[] = "prtPID";  // 4
  code UINT8 copyID[] = "copyID"; // 5 
  code UINT8 date[] = "date"; // 6
  code UINT8 fileName[] = "fileName"; // 7
  code UINT8 filePath[] = "filePath"; // 8
  
  code UINT8* list[] = {
    printInfo,
    croppingArea,
    fileID,
    copies,
    prtPID,
    copyID, 
    date,
    fileName,
    filePath};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}

UINT8 xmlGetCapabilityItemName( UINT8 index, UINT8* name ) {
  code UINT8 getCapability[] = "getCapability"; // 0
  code UINT8 capability[] = "capability";  // 1
  code UINT8 quailities[] = "qualities"; // 2
  code UINT8 paperSizes[] = "paperSizes"; // 3
  code UINT8 paperTypes[] = "paperTypes"; // 4
  code UINT8 fileTypes[] = "fileTypes"; // 5
  code UINT8 datePrints[] = "datePrints"; // 6
  code UINT8 fileNamePrints[] = "fileNamePrints"; // 7
  code UINT8 imageOptimizes[] = "imageOptimizes"; // 8
  code UINT8 layouts[] = "layouts"; // 9
  code UINT8 fixedSizes[] = "fixedSizes"; // 10
  code UINT8 croppings[] = "croppings"; // 11
  code UINT8 paperSize[] = "paperSize"; // 12
  
  code UINT8* list[] = {
    getCapability,
    capability,
    quailities,
    paperSizes,
    paperTypes,
    fileTypes, 
    datePrints,
    fileNamePrints,
    imageOptimizes,
    layouts,
    fixedSizes,
    croppings,
    paperSize};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
	if( index == 0xff ) {
		UINT8 idx = 0;
		for( ; idx < listCount; idx++ ) {
			if( strcmp( name, list[ idx ] ) == 0 )
				return idx;
		}
	} else {
	  
  if(index<listCount)
			strcpy( name, list[index]);
	}
	return 0xff;
}
void xmlGetStatusTypeName( UINT8 index, UINT8 *name ) 
{
  code UINT8 getJobStatus[] = "getJobStatus";		// 0
  code UINT8 getDeviceStatus[] = "getDeviceStatus";	// 1
  code UINT8 notifyJobStatus[] = "notifyJobStatus";	// 2
  code UINT8 notifyDeviceStatus[] = "notifyDeviceStatus";	//3// 3
  code UINT8 abortJob[] = "abortJob";	//4// 4
  code UINT8 abortStyle[] = "abortStyle";	// 5
  code UINT8 continueJob[] = "continueJob";	// 6
  
 code UINT8* list[] = {
    getJobStatus,
    getDeviceStatus,
    notifyJobStatus,
    notifyDeviceStatus,
    abortJob,
    abortStyle,
    continueJob};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}

void xmlGetJobStatusItemName( UINT8 index, UINT8* name ) {
  code UINT8 notifyJobStatus[] = "notifyJobStatus"; // 0
  code UINT8 prtPID[] = "prtPID"; // 1
  code UINT8 filePath[] = "filePath"; // 2
  code UINT8 copyID[] = "copyID"; // 3
  code UINT8 progress[] = "progress"; // 4
  code UINT8 imagesPrinted[] = "imagesPrinted"; // 5
  code UINT8 getJobStatus[] = "getJobStatus"; // 6
  
  code UINT8* list[] = {
    notifyJobStatus,
    prtPID,
    filePath,
    copyID,
    progress,
    imagesPrinted,
    getJobStatus};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}

void xmlGetDeviceStatusItemName( UINT8 index, UINT8* name ) {
  code UINT8 notifyDeviceStatus[] = "notifyDeviceStatus"; // 0
  code UINT8 dpsPrintServiceStatus[] = "dpsPrintServiceStatus"; // 1
  code UINT8 jobEndReason[] = "jobEndReason"; // 2
  code UINT8 errorStatus[] = "errorStatus"; // 3
  code UINT8 errorReason[] = "errorReason"; // 4
  code UINT8 disconnectEnable[] = "disconnectEnable"; // 5
  code UINT8 capabilityChanged[] = "capabilityChanged"; // 6
  code UINT8 newJobOK[] = "newJobOK"; // 7
  code UINT8 getDeviceStatus[] = "getDeviceStatus"; // 8 
  
  code UINT8* list[] = {
    notifyDeviceStatus,
    dpsPrintServiceStatus,
    jobEndReason,
    errorStatus,
    errorReason,
    disconnectEnable,
    capabilityChanged,
    newJobOK,
    getDeviceStatus};
  UINT8 listCount = sizeof(list)/(sizeof(UINT8 *));
  
  if(index<listCount)
		strcpy( name, list[index]);
}


#endif
