/**************************************************************************
 *         Copyright(c) 2009 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#include "general.h"
#include "host_func_opt.h"
#include "app_init.h"
#if (FUNC_HOST_DPS == 1)
#include "uiflow.h"
#include "app_ui_para.h"
#include "dps_struct.h"
#include "app_menu_micro.h"
#include "dps_api.h"
#include "app_still_play.h"
#include "sp1k_pb_api.h"
#include "sp1k_dps_api.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_main.h"
#include "app_dps.h"
#include "audio_rw.h"
#include "sp1k_disp_api.h"
#include "app_menu_tab.h"
#include "app_ui_osd.h"
#include "sp1k_aud_api.h"
#include "sp1k_hal_api.h"
#include "osd_api.h"
#include "sp1k_osd_api.h"
#include "app_osd_icon.h"
#include "key_scan.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "sp1k_util_api.h"
#include "app_msg.h"
#include "app_battery.h"
#include "app_sts_pwroff.h"
#include "app_menu_core.h"
#include "smcfunc.h"
#include "sp1k_dos_api.h"
#include "sp1k_dcf_api.h"
#include "dbg_def.h"
#include "dbg_mem.h"
#include "sp1k_gpio_api.h"

#include "exif_def.h"
#include "exif_param.h"
#include "exif_entry.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 DpsImageChooseFlag = 0;
UINT16 printTable[24][2]={0,1}; 

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 PrintStatus;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 xdata  storageReadyflag;
//extern UINT8 dramFileLoad( UINT8 *name, UINT32 startAddr, UINT32 *size );
//extern UINT8 exifParsingFile(UINT32 DramAddr, UINT32 srcSize, BIT ShowThumb,UINT8 *DateTime,struct Exif_Decompress_Parameter *ImagePara,struct Exif_Decompress_Parameter *ThumbPara);
extern UINT16 LCD_WIDTH;
extern UINT16 LCD_HEIGHT;
extern dps_data_t *dpsData;
/*
 * @brief		Connect Printer
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Connect Printer
*/
void 
appDpsConnectPrinter(
	void
)
{
	sp1kBtnDisableAll();
	sp1kHalUSBSwPlugOut();
	gUsbMode = K_UIOPTION_STORAGE_SIDC;
	G_UIStatus = K_UISTATUS_USBMODE_MASS;
	sp1kHalUSBSwPlugIn();
}
/*
 * @brief		App Dps Quality Set
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		App Dps Quality Set
*/
void 
appDpsQualitySet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	switch (uiPara ->DPSQuality )
	{
		case DPS_QUALITY_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_Quality_Default;
			break;
		case DPS_QUALITY_FINE:
			printCapability = K_DPS_HEX_PARAVAL_Quality_Fine;
			break;
		case DPS_QUALITY_NORMAL:
			printCapability = K_DPS_HEX_PARAVAL_Quality_Normal;
			break;
		case DPS_QUALITY_DRAFT:
			printCapability = K_DPS_HEX_PARAVAL_Quality_Draft;
			break;
		default :
			printCapability = K_DPS_HEX_PARAVAL_Quality_Default;
			break;
	}
	//dpsLoadGetCapability_Qualities(pCapabilityElem);
	//sp1k_dps_getCapability(DPS_ACTION_GetCapability_Qualities,  pCapabilityElem);
	sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_Qualities, printCapability);
}
void 
appDpsLayoutSet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	/**  mask index or standard set  **/
	if (1){//( uiPara->DPSPrintMode == DPS_PRINTMODE_STANDARD) {
			switch ( uiPara ->DPSLayout ) {
				case DPS_LAYOUT_AUTO:
					printCapability = K_DPS_HEX_PARAVAL_Layout_Default;
					break;
				case DPS_LAYOUT_1:
					printCapability = K_DPS_HEX_PARAVAL_Layout_1UpLayout;
					break;
				case DPS_LAYOUT_2:
					printCapability = K_DPS_HEX_PARAVAL_Layout_2UpLayout;
					break;
				case DPS_LAYOUT_4:
					printCapability = K_DPS_HEX_PARAVAL_Layout_4UpLayout;
					break;
				case DPS_LAYOUT_6:
					printCapability = K_DPS_HEX_PARAVAL_Layout_6UpLayout;
					break;
				case DPS_LAYOUT_8:
					printCapability = K_DPS_HEX_PARAVAL_Layout_8UpLayout;
					break;
				case DPS_LAYOUT_FULL:
					printCapability = K_DPS_HEX_PARAVAL_Layout_1UpBorderLess;
					break;
				case DPS_LAYOUT_INDEX:
					printCapability = K_DPS_HEX_PARAVAL_Layout_IndexPrint;
					break;
				default :	
					printCapability = K_DPS_HEX_PARAVAL_Layout_Default;
					break;
			}
	}
	else {
		printCapability = K_DPS_HEX_PARAVAL_Layout_IndexPrint;//index print
	}
		//dpsLoadGetCapability_Qualities(pCapabilityElem);
		//sp1k_dps_getCapability(DPS_ACTION_GetCapability_Layouts,  pCapabilityElem);
		sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_Layouts, printCapability);


}
void 
appDpsPapersizeSet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	switch ( uiPara ->DPSPageSize ) {
		case DPS_PAPERSIZE_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Default;
			break;
		case DPS_PAPERSIZE_L:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_L;
			break;
		case DPS_PAPERSIZE_2L:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_2L;
			break;
		case DPS_PAPERSIZE_POSTCARD:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_HagakiPostcard;
			break;
		case DPS_PAPERSIZE_100_150_MM:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_100mmX150mm;
			break;
		case DPS_PAPERSIZE_4_6_INCH:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_4X6in;
			break;
		case DPS_PAPERSIZE_8_10_INCH:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_8X10in;
			break;
		case DPS_PAPERSIZE_LETTER:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Letter;
			break;
		case DPS_PAPERSIZE_A4:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_A4;
			break;
		case DPS_PAPERSIZE_A3:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_A3;
			break;
		default :	
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Default;
			break;
	}
	//dpsLoadGetCapability_Qualities(pCapabilityElem);
	//sp1k_dps_getCapability(DPS_ACTION_GetCapability_PaperSizes,  pCapabilityElem);
	sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_PaperSizes, printCapability);
}

UINT32
appDpsPapersizeGet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;

	uiPara = appUiParaGet();

	switch ( uiPara ->DPSPageSize ) {
		case DPS_PAPERSIZE_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Default;
			break;
		case DPS_PAPERSIZE_L:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_L;
			break;
		case DPS_PAPERSIZE_2L:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_2L;
			break;
		case DPS_PAPERSIZE_POSTCARD:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_HagakiPostcard;
			break;
		case DPS_PAPERSIZE_100_150_MM:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_100mmX150mm;
			break;
		case DPS_PAPERSIZE_4_6_INCH:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_4X6in;
			break;
		case DPS_PAPERSIZE_8_10_INCH:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_8X10in;
			break;
		case DPS_PAPERSIZE_LETTER:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Letter;
			break;
		case DPS_PAPERSIZE_A4:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_A4;
			break;
		case DPS_PAPERSIZE_A3:
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_A3;
			break;
		default :	
			printCapability = K_DPS_HEX_PARAVAL_PaperSize_Default;
			break;
	}
	return printCapability;
}

void
appDpsPapertypeSet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	switch ( uiPara ->DPSPageType ) {
		case DPS_PAPERTYPE_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_PaperType_Default;
			break;
		case DPS_PAPERTYPE_PLAIN:
			printCapability = K_DPS_HEX_PARAVAL_PaperType_Plain;
			break;
		case DPS_PAPERTYPE_PHOTO:
			printCapability = K_DPS_HEX_PARAVAL_PaperType_Photo;
			break;
		case DPS_PAPERTYPE_F_PHOTO:
			printCapability = K_DPS_HEX_PARAVAL_PaperType_FastPhotp;
			break;
		default :
			printCapability = K_DPS_HEX_PARAVAL_PaperType_Default;
			break;
	}
	//dpsLoadGetCapability_Qualities(pCapabilityElem);
	//sp1k_dps_getCapability(DPS_ACTION_GetCapability_PaperTypes,  pCapabilityElem);
	sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_PaperTypes, printCapability);
}

void 
appDpsDateprintSet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	switch ( uiPara->DPSDate ) {
		case DPS_DATE_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_DatePrint_Default;
			break;
		case DPS_DATE_OFF:
			printCapability = K_DPS_HEX_PARAVAL_DatePrint_Off;
			break;
		case DPS_DATE_ON:
			printCapability = K_DPS_HEX_PARAVAL_DatePrint_On;
			break;
		default :
			printCapability = K_DPS_HEX_PARAVAL_DatePrint_Default;
			break;
	}
	//dpsLoadGetCapability_Qualities(pCapabilityElem);
	//sp1k_dps_getCapability(DPS_ACTION_GetCapability_DatePrints,  pCapabilityElem);
	sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_DatePrints, printCapability);


}

void 
appDpsFilenameprintSet(
	void
)
{	
	UINT32 printCapability;
	uiPara_t* uiPara;
	//UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	//PDPS_PARA_Capability_Elem pCapabilityElem = (PDPS_PARA_Capability_Elem)pData;
	uiPara = appUiParaGet();

	switch ( uiPara->DPSFileName ) {
		case DPS_FILENAME_AUTO:
			printCapability = K_DPS_HEX_PARAVAL_FileNamePrint_Default;
			break;
		case DPS_FILENAME_OFF:
			printCapability = K_DPS_HEX_PARAVAL_FileNamePrint_Off;
			break;
		case DPS_FILENAME_ON:
			printCapability = K_DPS_HEX_PARAVAL_FileNamePrint_On;
			break;
		default :
			printCapability = K_DPS_HEX_PARAVAL_FileNamePrint_Default;
			break;
	}
	//dpsLoadGetCapability_Qualities(pCapabilityElem);
	//sp1k_dps_getCapability(DPS_ACTION_GetCapability_FileNamePrints,  pCapabilityElem);
	sp1k_dps_setPrtCapability(DPS_ACTION_GetCapability_FileNamePrints, printCapability);
}

/*
 * @brief		Dps Start Job
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Dps Start Job
*/
void  
appDpsStartJob(
	void
)
{
	UINT32 handle;// = sp1k_dps_makeHandle(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
	UINT8 fileDate[20];
	UINT8 fileName[20];
	UINT8 bShowThumb = 0;
	//UINT32 fileAddr = K_SDRAM_ImagePlaybackBufAddr+ (UINT32)960*(UINT32)720;
	UINT32 size = 0;
	UINT16 width,height;
	UINT16 imageCnt = 0;
	UINT8 ret = 0;
	UINT8 i;
	ExifImage_t image;
	uiPara_t* uiPara;
	uiPara = appUiParaGet();
	//CPrintf1("pbfilename:%s\n",pbFile.FileName);
	sp1kPbDispQVSizeGet(&width,&height);
	//printf("QV:w%u,h%u\n",width,height);
	sp1kDispImgShow(sp1kPbDispBuffGet(1), width, height,100);
	appAutoOffEn(AUTO_POWEROFF_DISABLE);
	//sp1k_dps_init();
	
	appDpsParaSet();
	strcpy(fileName,pbFile.FileName);
	//printf("addr#=%lx\n",sp1kPbDispBuffGet(1));
	if(uiPara->DPSImageMode == 1){
		ret = sp1k_dps_addImageToPrint(0, NULL, uiPara->DPSCopies+1, 0, 0, 0, 0, NULL);
	}else if(uiPara->DPSImageMode == 0){
		sp1kDramFileLoad(fileName,K_SDRAM_ImagePlaybackBufAddr, &size);
		handle = sp1k_dps_makeHandle(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
		//exifParsingFile( K_SDRAM_ImagePlaybackBufAddr, size, 0, fileDate, NULL, NULL);
		exifParserStart(exifGetMainInfo, &image, K_SDRAM_ImagePlaybackBufAddr,size);
		memset(fileDate,0,20);
		exifGetIFDValue(EXIF_IFD_0, EXIF_TAG_DATE_TIME, 0,fileDate);
		sp1kDpsClearImageList();
		ret = sp1k_dps_addImageToPrint(handle, fileName, uiPara->DPSCopies+1, 0, 0, 0, 0, fileDate);
	}else if(uiPara->DPSImageMode == 2){
			imageCnt = pbFile.dwIndex;
			for(i = 0;i < 24;i ++){					
				if(printTable[i][0] != 0){
					pbFile.dwIndex = printTable[i][0];
					appPbFileInfoUpdate();
					memset(fileName,0,20);
					memset(fileDate,0,20);
					strcpy(fileName,pbFile.FileName);
					if(pbFile.wType == SP1K_PB_FILE_TYPE_JPG){
						sp1kDramFileLoad(fileName,K_SDRAM_ImagePlaybackBufAddr, &size);
						handle = sp1k_dps_makeHandle(sp1kPbFileIndexByOrderGet(pbFile.dwIndex));
						//exifParsingFile( K_SDRAM_ImagePlaybackBufAddr, size, 0, fileDate, NULL, NULL);
						exifParserStart(exifGetMainInfo, &image, K_SDRAM_ImagePlaybackBufAddr,size);
						exifGetIFDValue(EXIF_IFD_0, EXIF_TAG_DATE_TIME, 0,fileDate);
						sp1k_dps_addImageToPrint(handle, fileName, printTable[i][1], 0, 0, 0, 0, fileDate);
					}
				}
			}
			pbFile.dwIndex = imageCnt;
		}
	if(ret || (DpsImageChooseFlag == 1)){
		//printf("PrintStart!\n");
		for(i=0;i<24;i++){
			printTable[i][0] = 0;
			printTable[i][1] = 1;
			uiPara->DPSCopies = 0;
		}
		sp1k_dps_doAction(DPS_ACTION_StartJob,0);
	}else{
		//printf("PrintListIsEmpty!\n");
	}
}
/*
 * @brief		app Dps Init
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		app Dps Init
*/
void
appDpsInit(
	void
)
{
	UINT8 lastMode;
	UINT8 initMode;

	lastMode = appPreStateGet(0);
	switch ( lastMode ) {
		case APP_STATE_MENU:
		case APP_STATE_PAN_SHOW:
		case APP_STATE_SLIDE_SHOW:
		case APP_STATE_AUDIO_PLAY:
		case APP_STATE_DPS:
			initMode = SP1K_PB_INIT_SMART;
			break;
		case APP_STATE_STILL_VIEW:
		case APP_STATE_VIDEO_VIEW:
			initMode = SP1K_PB_INIT_FULL;
			appPbPreStateSet(lastMode);
			break;
		case APP_STATE_AUDIO_REC:
			if (sp1kAudioRecTypeGet() == AUDIO_TYPE_VOICE) {
				initMode = SP1K_PB_INIT_SMART;
			} 
			else {
				initMode = SP1K_PB_INIT_FULL;
				appPbPreStateSet(lastMode);
			}
			break;
		default:
			initMode = SP1K_PB_INIT_FULL;
			break;
	}
	if ( pbFile.dwIndex == 0xffff ) {    //xian ++, invalid index for #31928
		initMode = SP1K_PB_INIT_FULL;
		pbFile.dwIndex = sp1kPbFileCntGet();
	}
	if ( appStillPlayInit(initMode) != SUCCESS ) {
		pbFile.bFileErr = TRUE;
		if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
			sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);//2008-7-28 mantis #28190 step 1
			uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);//for mantis #26980
		}else{
    		uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
		}		
	}
	//sp1k_dps_setParam(DPS_PARAM_SidcSurportImageCountMax,0);
	//sp1k_dpsInit();
}
/*
 * @brief		Check File Number
 * @param	NONE
 * @retval	noJpgFlg
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		check the number of file in the disk
*/
UINT8
appDpsCheckFileNum(
	UINT32 *lastJpgIndex
)
{
	UINT16 pbIndex;
	UINT8 dpsJpgFlg;
	uiPara_t* puiPara ;	

	puiPara = appUiParaGet();
	if ( sp1kPbFileCntGet() == 0) {
	       appDpsPrinterStatusSet(DPS_PRINT_CANCEL);
		puiPara->USBMode = USB_MASS;
		gUsbMode = K_UIOPTION_STORAGE_MSDC;
		G_UIStatus = K_UISTATUS_USBMODE_MASS;
		dpsJpgFlg = DPS_JPG_NOT_EXIST;
		return dpsJpgFlg;
	}
	pbIndex=pbFile.dwIndex;
	pbFile.dwIndex = sp1kPbFileCntGet();
	appPbFileInfoUpdate();
	while ( (pbFile.wType !=SP1K_PB_FILE_TYPE_JPG) && (pbFile.dwIndex>0) ) {
		pbFile.dwIndex--;
		appPbFileInfoUpdate();				
	}//point to the last jpeg
	if ( pbFile.dwIndex ) {
		*lastJpgIndex = pbFile.dwIndex;
		dpsJpgFlg = DPS_JPG_EXIST;
	}
	else {
		*lastJpgIndex = 1;
		dpsJpgFlg = DPS_JPG_NOT_EXIST;
	}
	pbFile.dwIndex=pbIndex;
	appPbFileInfoUpdate();
	return dpsJpgFlg;
}
/*
 * @brief		Dps New Job Status Handle
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		NONE
*/
void 
appDpsNewJobStatusHandle(
	void
)
{
	UINT8 attr = OSD_ATTR_COLOR_WHITE | OSD_ATTR_MODE_HALFTONE | OSD_ATTR_BACK_EN;
	UINT16 iconX,iconY;
	uiPara_t *puiPara;
	DPS_PARA_JobStatus  dpsJobStatus;

	puiPara = appUiParaGet();
	sp1k_dps_getJobStatus(&dpsJobStatus);
	appPbOsdInit();

	if(PrintStatus == DPS_PRINT_ABORT){  //mantis 42461
		return;
	}
	if((puiPara->DPSImageMode != DPS_IMAGE_SIGNLE) || (puiPara->DPSCopies > 0)){
		osdIconPsGet(ID_ICON_WARNING,&iconX,&iconY);
		sp1kOsdStrDrawExt((LCD_WIDTH - strWidthGet(dpsJobStatus.progress))/2,iconY,dpsJobStatus.progress,0);
	}
}
void
appDpsPaperError(
	UINT32 err,
	UINT8 xdata* errString
)
{
	UINT8 xdata* pt = errString+6;
	strcpy(errString,"Paper Error");
	switch(err)
	{
		case K_DPS_HEX_MINOR_ReasonPaper_Default:
			break;
		case K_DPS_HEX_MINOR_ReasonPaper_PaperEmpty:
			strcpy(pt,"Empty");			
			break;
		case K_DPS_HEX_MINOR_ReasonPaper_LoadError:
		case K_DPS_HEX_MINOR_ReasonPaper_EjectError:		
		case K_DPS_HEX_MINOR_ReasonPaper_MediaError:		
		case K_DPS_HEX_MINOR_ReasonPaper_PaperJam:		
		case K_DPS_HEX_MINOR_ReasonPaper_FewPaper:		
		case K_DPS_HEX_MINOR_ReasonPaper_WrongComb:	
			break;
	}
}
appDpsInkError(
	UINT32 err,
	UINT8 xdata* errString
)
{
	UINT8 xdata* pt = errString+4;
	strcpy(errString,"Ink Error");
	switch(err)
	{
		case K_DPS_HEX_MINOR_ReasonInk_Default:
			break;
		case K_DPS_HEX_MINOR_ReasonInk_InkEmpty:
			strcpy(pt,"Empty");			
			break;
		case K_DPS_HEX_MINOR_ReasonInk_InkLow:
		case K_DPS_HEX_MINOR_ReasonInk_WasteError:	
			break;
	}
}
void
appDpsHardWareError(
	UINT32 err,
	UINT8 xdata* errString
)
{
	UINT8 xdata* pt = errString+9;
	strcpy(errString,"HardWare Error");
	switch(err)
	{
		case K_DPS_HEX_MINOR_ReasonHardWare_Default:	//0x0000
		case K_DPS_HEX_MINOR_ReasonHardWare_FatalErr:	//0x0100
		case K_DPS_HEX_MINOR_ReasonHardWare_ServiceCall://0x0200
		case K_DPS_HEX_MINOR_ReasonHardWare_PrinterOcc:	//0x0300
		case K_DPS_HEX_MINOR_ReasonHardWare_PrinterBusy://0x0400
		case K_DPS_HEX_MINOR_ReasonHardWare_LeverErr:	//0x0500
		case K_DPS_HEX_MINOR_ReasonHardWare_CoverOpen:	//0x0600
		case K_DPS_HEX_MINOR_ReasonHardWare_NoHead:		//0x0700
		case K_DPS_HEX_MINOR_ReasonHardWare_InkCoverOpen://0x0800
		case K_DPS_HEX_MINOR_ReasonHardWare_NoInkCart:	//0x0900
			break;
	}
}
void
appDpsFileError(
	UINT32 err,
	UINT8 xdata* errString
)
{
	UINT8 xdata* pt = errString+5;
	strcpy(errString,"File Error");
	switch(err)
	{
		case K_DPS_HEX_MINOR_ReasonFile_Default:		//0x0000
		case K_DPS_HEX_MINOR_ReasonFile_PrintInfoErr:	//0x0100
		case K_DPS_HEX_MINOR_ReasonFile_FileDecodeErr:	//0x0200
			break;
	}
}

void 
appDpsDeviceFatalError(
	UINT32 err
)
{
	UINT8 errString[20] = "Error";
	UINT32 errReason=err&(K_DPS_HEX_PARAVAL_ErrorReason_NoReason | 0x00ff0000);
	UINT32 errReasonType=err&0x0000ffff;
	UINT8 attr = OSD_ATTR_COLOR_WHITE | OSD_ATTR_MODE_HALFTONE | OSD_ATTR_BACK_EN;
	UINT16 iconX,iconY;

	switch(errReason)
	{
		case K_DPS_HEX_PARAVAL_ErrorReason_NoReason:			
			break;
		case K_DPS_HEX_PARAVAL_ErrorReason_Paper:
			appDpsPaperError(errReasonType,errString);
			break;
		case K_DPS_HEX_PARAVAL_ErrorReason_Ink:
			appDpsInkError(errReasonType,errString);
			break;
		case K_DPS_HEX_PARAVAL_ErrorReason_Hardware:
			appDpsHardWareError(errReasonType,errString);
			break;
		case K_DPS_HEX_PARAVAL_ErrorReason_File:
			appDpsFileError(errReasonType,errString);
			break;
	}
	osdIconPsGet(ID_ICON_WARNING,&iconX,&iconY);
	//osdStrDraw( 5, 4, errString, attr);
	sp1kOsdIconShow(ID_ICON_BUSY_1,ICON_CLEAR);
	osdStrDrawExt((LCD_WIDTH - strWidthGet(errString))/2,iconY,errString,0);

}
/*
 * @brief		Dps New Device Status Handle
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		check the printer status ,such as paper checking...
*/
void
appDpsNewDeviceStatusHandle(
	void
)
{
	//UINT8	attr;
	DPS_PARA_DeviceStatus  dpsDeviceStatus;
	//attr = OSD_ATTR_COLOR_WHITE | OSD_ATTR_MODE_HALFTONE | OSD_ATTR_BACK_EN;
	uiPara_t *uiPara;
	
	uiPara = appUiParaGet();
	sp1k_dps_getDeviceStatus( &dpsDeviceStatus );
	appPbOsdInit();
	switch(dpsDeviceStatus.printServiceStatus)	
	{
		case K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing:
		case K_DPS_HEX_PARAVAL_PrintSrvStatus_Idle:
		case K_DPS_HEX_PARAVAL_PrintSrvStatus_Paused:
			break;
	}
	switch(dpsDeviceStatus.jobEndReason)	
	{
		case K_DPS_HEX_PARAVAL_JobEndReason_JobNotEnded:
		case K_DPS_HEX_PARAVAL_JobEndReason_EndedNormal:
		case K_DPS_HEX_PARAVAL_JobEndReason_AbortImm:
		case K_DPS_HEX_PARAVAL_JobEndReason_AbortNext:
		case K_DPS_HEX_PARAVAL_JobEndReason_OtherReason:
			break;
	}
	switch(dpsDeviceStatus.errorStatus)		
	{
		case K_DPS_HEX_PARAVAL_ErrorStatus_NoError:			
			break;
		case K_DPS_HEX_PARAVAL_ErrorStatus_FatalError:
			appDpsDeviceFatalError(dpsDeviceStatus.errorReason);//(dpsDeviceStatus.errorStatus);
			break;
		case K_DPS_HEX_PARAVAL_ErrorStatus_Warning:
			appDpsDeviceFatalError(dpsDeviceStatus.errorReason);//(dpsDeviceStatus.errorStatus);
			break;
	}
		
	if(dpsDeviceStatus.disconnectEnable
		== K_DPS_HEX_PARAVAL_DisconEnable_True)
	{
	}
	if(dpsDeviceStatus.capabilityChanged
		== K_DPS_HEX_PARAVAL_CapChanged_True)
	{
		sp1k_dps_setParam(DPS_PARAM_DpsStatus,0);
		appDpsParaRst();
		//appDpsParaSet();
	}
	if ( dpsDeviceStatus.newJobOK 
		== K_DPS_HEX_PARAVAL_NewJobOk_True ) 
	{
		//printf("NewJobOk: %lx\n",dpsDeviceStatus.newJobOK);	
		uiPara->DPSImageMode = 0;
		DpsImageChooseFlag = 0;
		sp1kDpsClearImageList();
		appDpsPrinterStatusSet(DPS_PRINT_CON_OK);
		
		appPbOsdInit();
		appAutoOffEn(AUTO_POWEROFF_ENABLE);
		sp1kBtnEnableAll();
	}
}

/*
 * @brief		Dps Para Set
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Dps Para Set
*/

void
appDpsParaSet(
	void
)
{
	//if(sp1kDpsGetException())return;
	appDpsQualitySet();
	if(sp1kDpsGetException())return;
	appDpsLayoutSet();
	if(sp1kDpsGetException())return;
	appDpsPapersizeSet();
	if(sp1kDpsGetException())return;
	appDpsPapertypeSet();
	if(sp1kDpsGetException())return;
	appDpsDateprintSet();
	if(sp1kDpsGetException())return;
	appDpsFilenameprintSet();
}

/*
 * @brief		Dps Para Reset
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Dps Para Reset
*/
void 
appDpsParaRst(
	void
)
{
	uiPara_t* uiPara;
	uiPara = appUiParaGet();

	uiPara->DPSQuality = 0x00;
	uiPara->DPSLayout = 0x00;
	uiPara->DPSPageSize = 0x00;
	uiPara->DPSPageType = 0x00;
	uiPara->DPSDate = 0x00;
	uiPara->DPSFileName = 0x00;
	uiPara->DPSCopies = 0x00;
	uiPara->DPSImageMode = 0x00;
	DpsImageChooseFlag = 0;
}
/*
 * @brief		Dps printer status set
 * @param	NONE
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Dps printer status set
*/
void
appDpsPrinterStatusSet(
	UINT8 printerStatusSet
)
{
	PrintStatus = printerStatusSet;
}
/*
 * @brief		Dps printer status get
 * @param	NONE
 * @retval	printstatus
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Dps printer status get
*/
UINT8
appDpsPrinterStatusGet(
	void
)
{
	return PrintStatus;
}
//-----------------------------------------------------------------------------
//appDps
//-----------------------------------------------------------------------------
/*
 * @brief		DPS mode
 * @param	receive msg to DPS mode
 * @retval	NONE
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		DPS operation
*/
void 
appDps(
	UINT16  msg
) USING_0
{
	static UINT8 dpsJpgFlg = DPS_JPG_EXIST;
	UINT8 stateBeforePrevious;
	UINT8 printerStatus;
	UINT8 playsts;
	UINT32 lastJpgIdx;
	UINT16 width,height;
	uiPara_t* puiPara ;
	UINT8 i;
	
	sp1kPbDispQVSizeGet(&width,&height);
	puiPara = appUiParaGet();
	printerStatus = appDpsPrinterStatusGet();

	switch ( msg ) {
		case SP1K_MSG_STATE_INIT:
			//printf("Dps init\n");
			stateBeforePrevious = appPreStateGet(1);		
			dpsJpgFlg = appDpsCheckFileNum(&lastJpgIdx);			
			appDpsInit();
			if(DpsImageChooseFlag == 0){
				for(i=0;i<24;i++){
					printTable[i][0] = 0;
					printTable[i][1] = 1;
				}
			}
			if( stateBeforePrevious != APP_STATE_DPS ) {
	       		appDpsPrinterStatusSet(DPS_TO_CON_PRINT);
				
				appUIUSBModeSet();
				appDpsConnectPrinter();
			}
			appPbOsdInit();
			appAutoOffTimeSet();//jintao.liu 2008-3-28 add for mantis 21604
		break;
		
		case SP1K_MSG_KEY_PRESS_LEFT:
			if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			if ( printerStatus == DPS_TO_CON_PRINT ) {
				break;
			}
			if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
			playsts = appStillPlayNext(0);
			if(DpsImageChooseFlag == 1){
				puiPara->DPSCopies = 0;
			}
			if(playsts != SUCCESS)
			{
				if(playsts == PBABORT)
				{
					pbAbortMsgSeek();
					break; 
				}
				else
				{
					pbFile.bFileErr = TRUE;
                  	if(sp1kPbFileCntGet()){
    					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
								uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
								break;
							}else{
    							uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
								break;
							}
                  		}
                        else{
                   		    uiOsdDialogDisp(ID_STR_NO_FILE, 0);
							break;
                        }						
				}
			}
			else if(pbFile.wType != SP1K_PB_FILE_TYPE_JPG){
				osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
				break;
			}
			else {
					appPbOsdInit();
			}
			if ( printerStatus == DPS_PRINT_CON_OK ) {
				appPbOsdInit();
			}
			break;
		case SP1K_MSG_KEY_REPEATE_LEFT:
			if ( printerStatus != DPS_PRINT_CANCEL ) {
				break;
			}
			if(DpsImageChooseFlag == 1){
				puiPara->DPSCopies = 0;
			}
			if ( appStillPlayNext(0) != SUCCESS ) {
				pbFile.bFileErr = TRUE;
                if ( sp1kPbFileCntGet() ) {
					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
						uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
						break;
					}else{
    					uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
						break;
					}
                 }
                 else {
  	                  uiOsdDialogDisp(ID_STR_NO_FILE, 0);
                 }
			}
			else {
					appPbOsdInit();
			}
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
			break;
		case SP1K_MSG_KEY_PRESS_RIGHT:
			if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			if (printerStatus == DPS_TO_CON_PRINT ) {
				break;
			}
			if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
			if(DpsImageChooseFlag == 1){
				puiPara->DPSCopies = 0;
			}
			playsts = appStillPlayNext(1);
			if(playsts != SUCCESS)
			{
				if(playsts == PBABORT)
				{
					pbAbortMsgSeek();
					break; 
				}
				else
				{
					pbFile.bFileErr = TRUE;
                  		if(sp1kPbFileCntGet()){
    					if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
								uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
								break;
							}else{
    							uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
								break;
							}
                  		}
                        else{
                   		    uiOsdDialogDisp(ID_STR_NO_FILE, 0);
							break;
                        	}
				}
			}
			else {
					appPbOsdInit();
			}
			if ( printerStatus == DPS_PRINT_CON_OK ) {
				appPbOsdInit();
			}
			break;
		case SP1K_MSG_KEY_REPEATE_RIGHT:
			if ( printerStatus != DPS_PRINT_CANCEL ) {
				break;
			}
			if(DpsImageChooseFlag == 1){
				puiPara->DPSCopies = 0;
			}
			if ( appStillPlayNext(1) != SUCCESS ) {
				pbFile.bFileErr = TRUE;
                if ( sp1kPbFileCntGet() ) {
    				if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
							uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
							break;
						}else{
    						uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
							break;
						}
                   	}
				else {
              	     uiOsdDialogDisp(ID_STR_NO_FILE, 0);
				}
			} 
			else {
					appPbOsdInit();
			}
			osMsgFlush(MSG_CTRL_FLUSH_BTN);
			break;
		case SP1K_MSG_KEY_PRESS_UP:
		case SP1K_MSG_KEY_REPEATE_UP:
			if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			 if ( printerStatus == DPS_TO_CON_PRINT ) {
			 	break;
			 }
			 if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
			if(puiPara->DPSCopies == 98){
				puiPara->DPSCopies = 0;
			}else{
				puiPara->DPSCopies++;
			}
			for(i = 0;i < 24;i ++){
				if(printTable[i][0] == pbFile.dwIndex){
					printTable[i][1] = puiPara->DPSCopies+1;
					break;
				}
			}
			appPbOsdInit();
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:
		case SP1K_MSG_KEY_REPEATE_DOWN:
			if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			 if ( printerStatus == DPS_TO_CON_PRINT ) {
			 	break;
			 }
			 if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
			if(puiPara->DPSCopies == 0){
				puiPara->DPSCopies = 98;
			}else{
				puiPara->DPSCopies--;
			}
			for(i = 0;i < 24;i ++){
				if(printTable[i][0] == pbFile.dwIndex){
					printTable[i][1] = puiPara->DPSCopies+1;
					break;
				}
			}
			appPbOsdInit();
			break;
		case SP1K_MSG_KEY_PRESS_F1:
			 if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			 if ( printerStatus == DPS_TO_CON_PRINT ) {
			 	break;
			 }
			 if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
		 	 if ( printerStatus  == DPS_PRINT_CON_OK ){
				appDpsPrinterStatusSet(DPS_PRINT_DOING);
				appPbOsdInit();
				appDpsStartJob();
			}
			break;

		case SP1K_MSG_KEY_PRESS_MENU:
			if(printerStatus == DPS_PRINT_ABORT){
				break;
			}
			if ( printerStatus == DPS_PRINT_DOING ) {
				appDpsPrinterStatusSet(DPS_PRINT_ABORT);
				sp1k_dps_doAction(DPS_ACTION_AbortJob,K_DPS_HEX_PARAVAL_AbortStyle_Immediately);
				uiOsdDialogDisp(ID_STR_WAITING___, 100);
			}
			else {
				osMsgPost(SP1K_MSG_STATE_TO_MENU);
			}
			break;
	    case SP1K_MSG_KEY_PRESS_SNAP:
			 if(pbFile.bFileErr == TRUE)
			 {
				break;
			 }
			 if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			 if ( printerStatus  == DPS_TO_CON_PRINT ) {
			 	break;
			 }
			 if (printerStatus == DPS_PRINT_ABORT){
				break;
			 }
			 if ( printerStatus == DPS_PRINT_CON_OK ) {
			 	if(pbFile.wType != SP1K_PB_FILE_TYPE_JPG)
				{
					//osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
					//osMsgPost(SP1K_MSG_DPS_NEW_DEVICE_STATUS);
					break;
				}
				if(DpsImageChooseFlag == 1){
					UINT8 temp = 0;
					for(i = 0;i < 24;i ++){
						if(printTable[i][0] !=0 ){
							temp = 1;
						}
					}
					if(!temp){
						uiOsdDialogDisp(ID_STR_PLEASE_SELECT_A_PICTURE,2000);
						appPbOsdInit();
						return;
					}
				}
				appDpsPrinterStatusSet(DPS_PRINT_DOING);
				appPbOsdInit();
				appDpsStartJob();
				break;
			 }
		case SP1K_MSG_KEY_PRESS_S2:
			if(DpsImageChooseFlag == 1)
			{	
				UINT8 printflag = TRUE;
				UINT8 tableFull = TRUE;
				
				if(pbFile.bFileErr == TRUE)
					break;
				appPbFileInfoUpdate();
				
				for(i = 0;i < 24;i ++){
					if(printTable[i][0] == pbFile.dwIndex){
						printTable[i][0] = 0;
						printTable[i][1] = 1;
						printflag = FALSE;
						break;
					}
				}		
		
				if(printflag == FALSE){
					//sp1kDpsRemoveImage(i);	// i = the remove image position in imagelist				
				}else if(printflag == TRUE){
					for(i = 0;i < 24;i ++){
						if(printTable[i][0] == 0){
							tableFull = FALSE;
							printTable[i][0] = pbFile.dwIndex;
							printTable[i][1] = puiPara->DPSCopies+1;
							//sp1k_dps_addImageToPrint(sp1k_dps_makeHandle(sp1kPbFileIndexByOrderGet(pbFile.dwIndex)),
							//						  NULL, puiPara->DPSCopies+1, 0, 0, 0, 0, NULL);
							break;
						}
					}
					if(tableFull == TRUE){
						uiOsdDialogDisp(ID_STR_SELECT_UP_TO_24,2000);
					}
				}	
				appPbOsdInit();
			}
			break;
	    case SP1K_MSG_KEY_PRESS_PB:
			if ( printerStatus != DPS_PRINT_CANCEL ) {
				break;
			}
			#if(TV_OPTION == TV_PLAY_OPTION)
			if ( appPanelTypeGet() == 0 ) {
				return;
			}
			#endif
			switch (appPbPreStateGet() ) {
				case APP_STATE_STILL_VIEW:
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;
					break;
				case APP_STATE_VIDEO_VIEW:
					msg = SP1K_MSG_STATE_TO_VIDEO_VIEW;
					break;

				case APP_STATE_AUDIO_REC:
					sp1kAudioRecTypeSet(AUDIO_TYPE_NORMAL);
					msg = SP1K_MSG_STATE_TO_AUDIO_REC;
					break;
				default:
					msg = SP1K_MSG_STATE_TO_STILL_VIEW;
					break;
			}
			#if(TV_OPTION == TV_PLAY_OPTION)
				if ( appPanelTypeGet() == 0) {
					osMsgPost(SP1K_MSG_STATE_TO_STILL_PLAY);
				} 
				else {
					osMsgPost(msg);
				}
			#else
				osMsgPost(msg);
			#endif
			break;
		case SP1K_MSG_CARD_PLUG_IN:
			appStorageMount(K_MEDIA_SDF);	
			dpsJpgFlg = appDpsCheckFileNum(&lastJpgIdx);
			//sp1kDispImgShow(sp1kPbDispBuffGet(1), width, height,100);
			appDpsInit();
			osMsgPost(SP1K_MSG_DPS_NEW_CONFIG_PRINTER);
			break;
		case SP1K_MSG_CARD_PLUG_OUT:
			if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
				{
					osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
					sp1kDispImgWinEn(0);
					uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
				}
			break;
	    case SP1K_MSG_USB_PLUG_IN:
			if ( storageReadyflag == SUCCESS ) {
				if ( USB_DBG ) {
					sp1kHalUSBSwPlugIn();
				} 
			}
			break;
		case SP1K_MSG_USB_PLUG_OUT:			
			if ( gUsbMode == K_UIOPTION_STORAGE_SIDC ) {
				sp1kHalWait(2000);
				if(!sp1kUSB_PlugGet()){
					appPowerOffProc();
				}else{
					osMsgPost(SP1K_MSG_STATE_INIT);
				}
			}
			break;
		case SP1K_MSG_DPS_NEW_HDISCVRY:		
			sp1k_dps_doAction(DPS_ACTION_ConfigurePrintService,0);
			if(pbFile.bFileErr == TRUE){
				/*if(pbFile.dwIndex== 1){
					uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
					break;
				}
				osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
				sp1kHalWait(1000); 
				osMsgPost(SP1K_MSG_DPS_NEW_HDISCVRY);*/
				if(sp1kDcfIsOurBaseName(pbFile.FileName)==TRUE){
					uiOsdDialogDisp(ID_STR_FILE_ERROR_, 0);
					break;
				}else{
    				uiOsdDialogDisp(ID_STR_NOT_SUPPORT_, 0);
					break;
				}
				break;
			}
			if(pbFile.wType != SP1K_PB_FILE_TYPE_JPG){
				osMsgPost(SP1K_MSG_KEY_PRESS_LEFT);
				sp1kHalWait(1000); 
				osMsgPost(SP1K_MSG_DPS_NEW_HDISCVRY);
				break;
			}		
			break;
		case SP1K_MSG_DPS_NEW_CONFIG_PRINTER:
			if( dpsJpgFlg ) {
				gprmBlackFill(sp1kPbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
				sp1kDispImgShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);
				sp1kDispImgWinEn(1); 
				uiOsdDialogDisp(ID_STR_NO_JPEG, 0);
				sp1kHalWait(600);
				appPowerOffProc();
				break;
			}
			appDpsPrinterStatusSet(DPS_PRINT_CON_OK); 			
			appPbOsdInit();
			break;
		case SP1K_MSG_DPS_NEW_DEVICE_STATUS:
			appDpsNewDeviceStatusHandle();			
			break;
		case SP1K_MSG_DPS_NEW_JOB_STATUS:	
			appDpsNewJobStatusHandle();
			break;
		case SP1K_MSG_DPS_ERROR:
			if((PrintStatus == DPS_PRINT_DOING) || (PrintStatus ==DPS_PRINT_ABORT)){
				uiOsdDialogDisp(ID_STR_PRINT_ERROR,3000);
				appDpsPrinterStatusSet(DPS_PRINT_CON_OK);
				osMsgPost(SP1K_MSG_STATE_INIT);
			}
			break;
		case sp1k_MSG_DPS_NOT_FIND_PRINTER:
			uiOsdDialogDisp(ID_STR_NOT_FIND_PRINTER,1000);
			break;
	    case SP1K_MSG_TV_PLUG_IN:
		case SP1K_MSG_TV_PLUG_OUT:
			appDispSet();
			if ( printerStatus != DPS_PRINT_CANCEL ) {
				appPbOsdInit();
			}
			break;
		case SP1K_MSG_BAT_LV_UPDATE:
			 if ( printerStatus == DPS_PRINT_DOING ) {
		  		break;
			 }
			//uiUpdateBatteryLevel();
			appBattStsHandle();
			break;

		
		#if SUPPORT_PANEL_PROTECT
		case SP1K_MSG_PANEL_PROTECT:
			appPanelAutoProt();
			break;

		case SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT:
			app_set_lcd_backlight_status(1);
			break;
		#endif
			
		default:
			break;			
	}
}

#endif
