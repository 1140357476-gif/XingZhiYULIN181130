/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
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
#include "os_msg.h"
#include "app_msg.h"
#include "main.h"
#include "key_scan.h"
//#include "dos32.h"
#include "sp1k_dos_api.h"

#include "sp1k_aud_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_gpio_api.h"

#include "app_main.h"
#include "app_menu_event.h"
#include "app_menu_micro.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu.h"
#include "app_still_play.h"
#include "app_multi_play.h"
#include "app_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_ui_para.h"
#include "app_osd_icon.h"
#include "app_still_view.h"
#include "sp1k_disk_api.h"
#include "dps_struct.h"
#include "sp1k_dps_api.h"
#include "dps_param.h"
#include "app_dps.h"
#include "dps_AddrDef.h"
#include "dbg_def.h"
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
extern SIMAGE xdata pbFile;
extern UINT32 RemainPictureNO;
extern UINT8 specialMenuCurrItemIdx;//for ui tool
extern UINT8 specialMenuOrder[30];//for ui tool

/**
 * @fn        UINT8 menuOptOnlyInJpeg(void)
 * @brief     menuOptOnlyInJpeg
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-12-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8  menuOptOnlyInJpeg(void)
{
	if ( APP_STATE_MUTI_PLAY == appPreStateGet(0))
		return FALSE;

	if (pbFile.bFileErr == TRUE)
		return FALSE;//2008-7-28 mantis #28190
	
	if (sp1kPbFileCntGet())
	{
		if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
			return TRUE;
	}

	return FALSE;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSIDCIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring
	return FALSE;//By phil for demo
}


UINT8 menuUSBPowerIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if(sp1kUSB_PlugGet() == USB_PLUG_IN)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuPhotoframeIsActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		return FALSE;
	}

	if(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		return FALSE;
	}

	if((uipara->FDMode == FDMODE_OFF) && ((sysMemTotSizeGet() >  DEV_MEM_SIZE_64M)))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProtectIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if ( APP_STATE_MUTI_PLAY == appPreStateGet(0))
		return FALSE;//mantis #27903
	
	if (pbFile.bFileErr == TRUE)
		return FALSE;//2008-7-28 mantis #28190

	if (sp1kPbFileCntGet())
	{
		return TRUE;
	}

	return FALSE;
}
UINT8 menuBlinkDetectIsActive (UINT8 item) USING_0
{
	item=item;
	if(sysMemTotSizeGet()== DEV_MEM_SIZE_64M)
	{
		return FALSE;
	}
	return TRUE;

}

/**
 * @fn        UINT8 menuVideoSizeIsActive(UINT8 item)
 * @brief     menuVideoSizeIsActive
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-9-15
 * @todo      N/A
 * @bug       N/A
*/
UINT8 menuVideoSizeIsActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();

	if ((sysMemTotSizeGet()<=DEV_MEM_SIZE_64M) &&\
		(specialMenuOrder[specialMenuCurrItemIdx]==MOV_SIZE_1080P||\
		specialMenuOrder[specialMenuCurrItemIdx]==MOV_SIZE_HD||\
		specialMenuOrder[specialMenuCurrItemIdx]==MOV_SIZE_D1)){
		return FALSE;
	}
	if((appSDPlugStatusGet()==0) &&\
		(sp1kDiskTypeGet()==DEV_TYPE_DISK_NAND))
	{
		return FALSE;
	}

	/*
	if(appSDPlugStatusGet()==0)
	{
		return FALSE;
	}
	else if(sp1kSDSpeedClassGet()<SD_SPEED_CLASS4)
	{
		return FALSE;
	}*/

	return TRUE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuDeleteIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if (sp1kPbFileCntGet())
	{
		return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSlideStartIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if (pbFile.bFileErr == TRUE)
		return FALSE;//2008-7-28 mantis #28190
	
	if (sp1kPbFileCntGet())
	{
		if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
			return TRUE;
	}

	return FALSE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuSlideShowIsActive (UINT8 item) USING_0
{
	PBFile_t pFileInfo;
	item=item;//just for clearing waring

	if (pbFile.bFileErr == TRUE)
		return FALSE;//2008-7-28 mantis #28190

	if (sp1kPbFileCntGet())
	{
		sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);  //  mantis 0043808
		pbFile.wType = pFileInfo.FileType;
		
		//appPbFileInfoUpdate();//2008-7-9 mantis #27986
		
		if (pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
			return TRUE;
	}

	return FALSE;
}
UINT8 menuDriverModeActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();

	if(uipara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		return FALSE;
	}

	if(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		return FALSE;
	}

	return TRUE;
}
UINT8 menuSpeedyCapActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->FDMode == FDMODE_ON)
	{
		return FALSE;
	}
	else if(uipara->DriverMode != PRV_SNAP_SINGLE)
	{
		return FALSE;
	}
	else if(uipara->PhotoFrame != PRV_PHOTO_FRAME_OFF)
	{
		return FALSE;
	}
	else if(uipara->FDMetering == FD_AE_ON)
	{
		return FALSE;
	}
	else if(uipara->ImageSize == PRV_SIZE_2M_16_9 || uipara->ImageSize == PRV_SIZE_7M_16_9)
	{
		return FALSE;
	}
	else if( sysMemTotSizeGet() <= DEV_MEM_SIZE_64M )
	{
		return FALSE;
	}
	else if(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		return FALSE;
	}
	else if(uipara->FrameRate == MOV_FRM_HIGH){

		return FALSE;
	}
	return TRUE;
}

UINT8 menuSpeedyCapRecActive(UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->FDMode == FDMODE_ON)
	{
		return FALSE;
	}
	else if(uipara->DriverMode != PRV_SNAP_SINGLE)
	{
		return FALSE;
	}
	else if(uipara->PhotoFrame != PRV_PHOTO_FRAME_OFF)
	{
		return FALSE;
	}
	else if(uipara->FDMetering == FD_AE_ON)
	{
		return FALSE;
	}
	else if( sysMemTotSizeGet() <= DEV_MEM_SIZE_64M )
	{
		return FALSE;
	}
	else if(uipara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		return FALSE;
	}
	else if(uipara->FrameRate == MOV_FRM_HIGH){

		return FALSE;
	}
	return TRUE;
}

/**
 * @fn        UINT8 menuBurstIsActive(UINT8 item)
 * @brief     menuBurstIsActive
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-8-31
 * @todo      N/A
 * @bug       N/A
*/
UINT8 menuBurstIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if(RemainPictureNO<3)
	{
		return FALSE;
	}

	return TRUE;
}

UINT8 menuImageSizeActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		return FALSE;
	}
	return TRUE;
}

UINT8 menuSizeIsActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		return FALSE;
	}
	return TRUE;
}

UINT8 menuPvEffectActive(UINT8 item) USING_0
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();

	if(uipara->FDMode == FDMODE_ON)
	{
		return FALSE;
	}
	return TRUE;
}

UINT8 menuFDIsActive(UINT8 item)
{
	uiPara_t* uipara;
	item = item;
	uipara=appUiParaGet();
	if(uipara->PhotoFrame != PRV_PHOTO_FRAME_OFF)
	{
		return FALSE;
	}

	if(uipara->ImageEffect == PRV_EFFECT_NEGATIVE)
	{
		return FALSE;
	}

	if(uipara->SpeedyCap == SPEEDY_CAPTURE_ON)
	{
		return FALSE;
	}

	if(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)
	{
		return FALSE;
	}

	return TRUE;
}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuEffectIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring
	return menuOptOnlyInJpeg();
}

/**
 * @fn        UINT8 menuRedEyeIsActive(UINT8 item)
 * @brief     menuRedEyeIsActive
 * @param     [in] item
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-12-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8 menuRedEyeIsActive(UINT8 item)
{
	item=item;//just for clearing waring
	return menuOptOnlyInJpeg();
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuRotateIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring
	return menuOptOnlyInJpeg();
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuResizeIsActive (UINT8 item) USING_0
{
	PBFile_t fileInfo;
	item=item;//just for clearing waring
	sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
	if(fileInfo.MediaWidth <=640 || fileInfo.MediaWidth == 1920)
	{
		return FALSE;
	}
	return menuOptOnlyInJpeg();
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuResizeSizeIsActive (UINT8 item) USING_0
{
	PBFile_t pFileInfo;
	UINT16 w, h;

	if (item == 0)
	{
		w = 2592;
		h = 1944;
	}
	else if (item == 1)
	{
		w = 2048;
		h = 1536;
	}
	else if (item == 2)
	{
		w = 1600;
		h = 1200;
	}
	else if (item == 3) // 2M 16:9
	{
		w = 1920;
		h = 1080;
	}
	else if (item == 4)
	{
		w = 1024;
		h = 768;
	}
	else
	{
		w = 640;
		h = 480;
	}
	if (sp1kPbFileCntGet())
	{
		sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(pbFile.dwIndex), &pFileInfo);
		if (pFileInfo.FileType == SP1K_PB_FILE_TYPE_JPG)
		{	 //printf("imgw=%ld\n", (long)pFileInfo.MediaWidth);  printf("imgh=%ld\n", (long)pFileInfo.MediaHeight);
			if ((UINT32)pFileInfo.MediaWidth*3 > (UINT32)pFileInfo.MediaHeight<<2) {//16:9
				if ((pFileInfo.MediaWidth > w) && (pFileInfo.MediaHeight > h)&&(UINT32)w*3 > ((UINT32)h<<2))
				{
					return TRUE;
				}
			}
			else {
				if ((pFileInfo.MediaWidth > w) && (pFileInfo.MediaHeight > h)&&((UINT32)w*3 == ((UINT32)h<<2)))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuVoiceMemoIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring
	return menuOptOnlyInJpeg();
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuStartupIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring
	return menuOptOnlyInJpeg();
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuVoiceMemoAddIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if (sp1kPbFileCntGet())
	{
		if(pbFile.wType == SP1K_PB_FILE_TYPE_JPG)
			return TRUE;
	}

	return FALSE;
}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuVoiceMemoDeleteIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	if (sp1kPbFileCntGet())
	{
		if(pbFile.wType == SP1K_PB_FILE_TYPE_JPG && pbFile.bVoiceMemo == TRUE)
			return TRUE;
	}

	return FALSE;
}

#if !TAG_TODO
UINT8 menuForceInactive(void) USING_0
{
	return FALSE;
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 
UINT8 menuFrameIsActive (UINT8 item) USING_0
{
	uiPara_t* uipara;
	uipara=appUiParaGet();
	/* yp begin for mantis 42848*/
	if(uipara->VideoSize == MOV_SIZE_1080P){
		switch(item){
			case 2:
			return TRUE;
		}
	}
	if( (uipara->VideoSize == MOV_SIZE_HD)||(uipara->VideoSize == MOV_SIZE_D1)||(uipara->VideoSize == MOV_SIZE_QVGA))
		{
		switch(item) {
			case 0:
				return FALSE;
			case 2:
				return FALSE;
		}
	}
	if (uipara->VideoSize == MOV_SIZE_VGA){
		switch(item){
			case 0:
				if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_NAND))
				{
					return FALSE;
				}
				break;
			case 2:
				return FALSE;
		} // yp end for mantis 42848
	}
	if ((uipara->SpeedyCap==SPEEDY_CAPTURE_ON)||(uipara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON)){

		return FALSE;
	}
}
#if (FUNC_HOST_DPS == 1)
/*
 * @brief		Check printer capability
 * @param	capabilityClassPara :
 * @param	capabilityItemPara :
 * @retval	status : 0 is success,1 is fail.
 * @author	Li tuhong
 * @since		2009-11-16
 * @todo		Check printer capability.If printer have the specific capability,the fuction will return TRUE,or FALSE
*/
static UINT8
menuDpsCheckCapability(
	UINT8 capabilityClassPara,
	UINT32 capabilityItemPara
)
{
	UINT8 pData[ sizeof( DPS_PARA_Capability_Elem ) ];      
	UINT8 i,_count;
	PDPS_PARA_Capability_Elem pce = (PDPS_PARA_Capability_Elem)pData;
	//dps_capability_para_t *dpspara;
	UINT8 printerStatus = appDpsPrinterStatusGet();
	//printf("printerstatus:%bu\n",printerStatus);
	if ( printerStatus == DPS_PRINT_CON_OK ) {

		//pce->paperSize = appDpsPapersizeGet();	//get papersize information
		sp1k_dps_getCapability( capabilityClassPara, pce);
		//dpsLoadGetCapability_Item(capabilityClassPara,pce);
		_count = pce->count;
		if( ( pce->count ) > 24 ) {
			_count = 24;
		}
		for( i=0; i<_count; i++ ) {
 			if( ( pce->values[i])== capabilityItemPara ) {
				return TRUE;
			}
		}
 		return FALSE;
	}
	else {
		return FALSE;
	}
}
#if 0
UINT8 menuDpsIndexPrintIsActive (UINT8 item) USING_0
{
	item=item;//just for clearing waring

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_Layouts, K_DPS_HEX_PARAVAL_Layout_IndexPrint );
}/*thumbnail print*/
#endif

UINT8 menuDpsQualityIsActive (UINT8 item) USING_0
{
	UINT32 para;

   	if (item == 0)
		return TRUE;
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_Quality_Fine;
	else if (item == 2)
		para = K_DPS_HEX_PARAVAL_Quality_Normal;
	else if (item == 3) 
		para = K_DPS_HEX_PARAVAL_Quality_Draft;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_Qualities,  para);
}

UINT8 menuDpsLayoutIsActive (UINT8 item) USING_0
{
	UINT32 para;

	if (item == 0)
		return TRUE;
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_Layout_1UpLayout;
	else if (item == 2)
		para = K_DPS_HEX_PARAVAL_Layout_2UpLayout;
	else if (item == 3)
		para = K_DPS_HEX_PARAVAL_Layout_4UpLayout;
	else if (item == 4)
		para = K_DPS_HEX_PARAVAL_Layout_6UpLayout;
	else if (item == 5)
		para = K_DPS_HEX_PARAVAL_Layout_8UpLayout;
	else if (item == 6)
		para = K_DPS_HEX_PARAVAL_Layout_1UpBorderLess;
	else
		para = K_DPS_HEX_PARAVAL_Layout_IndexPrint;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_Layouts,  para);		
}

UINT8 menuDpsPaperSizeIsActive (UINT8 item) USING_0
{
	UINT32 para;

	if (item == 0){
		return TRUE;
		}
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_PaperSize_L;
	else if (item == 2)
		para = K_DPS_HEX_PARAVAL_PaperSize_2L;
	else if (item == 3)
		para = K_DPS_HEX_PARAVAL_PaperSize_100mmX150mm;
	else if (item == 4)
		para = K_DPS_HEX_PARAVAL_PaperSize_HagakiPostcard;
	else if (item == 5)
		para = K_DPS_HEX_PARAVAL_PaperSize_4X6in;
	else if (item == 6)
		para = K_DPS_HEX_PARAVAL_PaperSize_8X10in;
	else if (item == 7)
		para = K_DPS_HEX_PARAVAL_PaperSize_Letter;
	else if (item == 8)
		para = K_DPS_HEX_PARAVAL_PaperSize_A4;
	else
		para = K_DPS_HEX_PARAVAL_PaperSize_A3;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_PaperSizes, para);	
}

UINT8 menuDpsTypeIsActive (UINT8 item) USING_0
{
	UINT32 para;

	if (item == 0)
		return TRUE;
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_PaperType_Plain;
	else if (item == 2)
		para = K_DPS_HEX_PARAVAL_PaperType_Photo;
	else
		para = K_DPS_HEX_PARAVAL_PaperType_FastPhotp;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_PaperTypes,  para);
}

UINT8 menuDpsDateIsActive (UINT8 item) USING_0
{
	UINT32 para;
	
	if (item == 0)
		return TRUE;
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_DatePrint_Off;
	else
		para = K_DPS_HEX_PARAVAL_DatePrint_On;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_DatePrints, para);	
}

UINT8 menuDpsNameIsActive (UINT8 item) USING_0
{
	UINT32 para;

	if (item == 0)
		return TRUE;
	else if (item == 1)
		para = K_DPS_HEX_PARAVAL_FileNamePrint_Off;
	else
		para = K_DPS_HEX_PARAVAL_FileNamePrint_On;

	return menuDpsCheckCapability( DPS_ACTION_GetCapability_FileNamePrints, para );	
}
#endif

