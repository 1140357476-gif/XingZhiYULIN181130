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
#include "host_func_opt.h"

#if FUNC_HOST_CAL
#include "app_init.h"
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "initio.h"

#include "hal.h"
#include "hal_gprm.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "App_ui_para.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"
#include "app_sts_pwroff.h"
#include "app_battery.h"
#include "app_audio_play.h"
#include "app_menu_core.h"
#include "app_menu.h"
#include "app_menu_api.h"
#include "app_menu_micro.h"
#include "app_menu_tab.h"
#include "app_menu_event.h"
#include "app_cal_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_calibration_api.h"
#include "sp1k_snap_api.h"
#include "dbg_wdt.h"
#include "ae_api.h"
#include "Lsc_cal.h"
#include "sp1k_util_api.h"
#include "Sp1k_hal_api.h"
#include "App_cal_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_aud_api.h"
#include "rsc_id.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_pv_api.h"

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
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern  UINT8 id[4];

#if SUPPORT_SPI_WRITE_CONTROL
extern UINT8 mastWriteSpiFlag;
#endif


extern void appOsdModeSet(UINT8 mode);

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
UINT8 menuCalUsbPowerSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuExit();
			 ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalCDSP_OBSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			sp1kCalibrationObSet(item);
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;				
	}
	return ret; 
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
UINT8 menuCalCDSP_LensShadingSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			sp1kCalibrationLscSet(item);
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;				
	}
	return ret; 
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
UINT8 menuCalCDSP_GammaSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			sp1kCalibrationGammaSet(item);
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;				
	}
	return ret; 
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
UINT8 menuCalCDSP_AESet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			if (item == 0 || item == 1) {
				sp1kCalibrationAESet(item);
			}
			else if (item == 2) {
				sp1kCalibrationAEWinConfigSet(LUMA_CAL_LSC);
				menuProcParent ();
				menuProcDisp();
			}
			else if (item == 3) {
				sp1kCalibrationAEWinConfigSet(LUMA_CAL_COLOR_CHECKER);
				menuProcParent ();
				menuProcDisp();
			}
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalCDSP_AEGidSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_UP:
		case SP1K_MSG_KEY_REPEATE_UP:	
			menuProcMoveUp();
			sp1kCalibrationAEConfig(menuProcCurrItemGet(layer),50);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_DOWN:	
		case SP1K_MSG_KEY_REPEATE_DOWN:	
			menuProcMoveDown();
			sp1kCalibrationAEConfig(menuProcCurrItemGet(layer),50);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;	
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;			
	}
	return ret; 
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
UINT8 menuCalCDSP_AWBSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			sp1kCalibrationAWBSet(item);
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
	}
	return ret; 
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
UINT8 menuCalSnapTest (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	layer= layer;
	if(item == 2)
	{
		appOsdModeSet(1);
	}
	else
	{
		appOsdModeSet(0);
	}
	switch (msg) {

		case SP1K_MSG_KEY_PRESS_MENU:
			appOsdModeSw(0);
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			switch(item){
				case 0:/*snap raw*/
					appCalSnapRaw();
					break;
				case 1:/*Snap Pure raw*/
					appCalSnapPureRaw();
					break;
				case 2:/*Snap IQ Raw*/
					appCalSnapIQRaw();
					break;					
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalPvTest (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			switch(item) {
				case 0:/*Pv Raw*/
					appCalPvRaw();
					break;
				case 1:/*PV Pure Raw*/
					appCalPvPureRaw();
					break;
				case 2:/*Pv YUV*/
					appCalPvYuv();
					break;
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}


//#define menuCalAETest_Print	printf
#ifndef menuCalAETest_Print
	#include "Dbg_def.h"
	#define menuCalAETest_Print		/_printf
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
UINT8 menuCalAETest (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	menuCalAETest_Print("id[%bx]=0x%bx\n",item,id[item]);
	if(item<sizeof(id)-1){
		menuCalAETest_Print("id[%bx]=0x%bx\n",item+1,id[item+1]);
	}

	if(id[item+1] <1){
		menuCalAETest_Print("Id value err\n");
		return ret;
	}

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			switch(item) {
				case 0:/*Pv Raw*/
				case 1:/*PV Pure Raw*/	
				case 2:/*Pv YUV*/	
					appCalShowWait();
					sp1kCalibrationAETest( id[item], id[item+1] - 1, 60 );
					break;
				case 3:/*save*/
					sp1kCalibrationAETestSave( id[0], id[item] - 1 );
					sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
					break;
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}



UINT8 menuCalAFTurn(UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT8 afFlt2Type = 1;
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
	
		case SP1K_MSG_KEY_PRESS_MENU:
			sp1kTimer1Stop();
			sp1kHALCdspAFWinEn(0,0,0,afFlt2Type,0);
			break;
		case SP1K_MSG_KEY_PRESS_SNAP:
			
			switch ( item ){
				case 0:
					appCalAF2AStart();
					ret = MENU_CALL_DEFAULT;
					break;
				case 1:
					appCalAF2AStop();
					ret = MENU_CALL_DEFAULT;
					break;
				case 2:
					appCalAF();
					ret = MENU_IGNORE_DEFAULT;
					break;
			}
			
			break;
	}
	return ret; 

}

UINT8 menuCalTp(UINT8 layer, UINT8 item, UINT16 msg)
{

	UINT8 ret = MENU_CALL_DEFAULT;
	layer= layer;
	item=item;
	msg=msg;//just for clearing waring
#if FUNC_HOST_TOUCH_PANEL
	switch (msg) 
	{
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalTouchPanel();
			break;
		case SP1K_MSG_TP_CAL_COMPLETE:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
#endif
	return ret; 

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
UINT8 menuCalOB (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalOB();	
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalAutoOB(UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalAutoOB();	
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalPvOB(UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	//UINT8 i;
	//UINT16 resOff;
	//UINT8 gainIdxTbl[17] = {0,10,20,30,40,50,60,70,71,80,90,102,103,110,120,134,135};
	//UINT16 expTbl[17] = {240,230,220,210,200,180,170,150,150,140,130,120,110,100,90,80,70};
	UINT8 gainIdxTbl[9] = {0,39,71,80,90,103,110,120,134};
	UINT16 expTbl[9] = {2,2,2,2,2,2,2,2,2};//{240,200,180,160,140,130,120,100,70};
	//UINT8 gainIdxTbl[11] = {71,80,90,100,102,103,110,120,130,134,135};
	//UINT16 expTbl[11] = {150,140,130,120,110,110,100,90,80,80,70};
	//UINT8 gainIdxTbl[17] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80};
	//UINT16 expTbl[17] = {240,240,230,230,220,220,210,210,200,200,190,190,180,180,170,170,160};
	//UINT16 frontHSize,frontVSize;
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
		#if OB_CAL_EN
			sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_SAVE_RAW , 0);	
			sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_MULTI_CAL , 1);	
			sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_ONE_POINT_CAL , 1);       //new ob
		
			/*Cal preview mode 0*/
			resOff = 0;
			for( i = 0 ; i < sizeof(gainIdxTbl) ; i ++ ){
				sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_GAIN_INDEX , gainIdxTbl[i]);
				sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_INIT_EXP , expTbl[i]);
				appCalPvOB(SP1K_RSC_ID_MILSCCAL,resOff); /*0x42*/	
				resOff += 5;/*gainIdx-R-Gr-B-Gb , so need 5 word*/
			}
			#if 1
			/*Cal preview mode 1*/	
			resOff =64;
			sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_60);
			sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);			
			sp1kPreviewCropSizeSet(frontHSize,frontVSize);
			sp1kPreviewSet(0, 640, 480, 100, 0);/*preview mode 1*/
			#if 0
			for( i = 0 ; i < sizeof(gainIdxTbl) ; i ++ ){
				sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_GAIN_INDEX , gainIdxTbl[i]);
				sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_INIT_EXP , expTbl[i]);
				appCalPvOB(SP1K_RSC_ID_MILSCCAL,resOff); /*0x42*/
				resOff += 5;/*gainIdx-R-Gr-B-Gb , so need 5 word*/
			}
			#else
			sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_GAIN_INDEX , 0/*gainIdxTbl[0]*/);
			sp1kCalPvOBParaSet(SP1K_CAL_PV_OB_INIT_EXP , 240/*expTbl[0]*/);
			appCalPvOB(SP1K_RSC_ID_MILSCCAL,resOff); /*0x42*/
			#endif
			/*Set back to preview mode 0*/
			sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_20);
			sp1kFrontPreviewSizeGet(0, &frontHSize, &frontVSize);			
			sp1kPreviewCropSizeSet(frontHSize,frontVSize);
			sp1kPreviewSet(0, 640, 480, 100, 0);/*preview mode 0*/			
			#endif
			#endif
			break;
	}
	return ret; 
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
UINT8 menuCalAWBTest (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalAwb();
			break;
	}
	return ret; 
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
UINT8 menuCalBpPro (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalBpSearch(item);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
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
UINT8 menuCalFwUpgrade (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCardUpdateFW();
			osMsgPost(SP1K_MSG_POWER_OFF);
			break;
	}
	return ret; 
}


/**
 * @fn        UINT8 menuCalLsc(UINT8 layer, UINT8 item, UINT16 msg)
 * @brief     menuCalLsc
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-12-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 menuCalLsc (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_OK:
#ifdef LSC_CALIB_ENABLE
			#if SUPPORT_SPI_WRITE_CONTROL
			mastWriteSpiFlag = 1;
			appSpiWpSet(0);
			#endif
			
			appUiBusyShow(0);

			/* configure it here, they're depended on sensor size, shading condition */
			sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_X,16);	/* 8 16 32 */
			sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_Y,16);	/* 8 16 32 */
			sp1kLscCfgSet(SP1K_CALIB_LSC_LUMA_LEVEL, 100);
			sp1kLscCfgSet(SP1K_CALIB_LSC_CHANNEL,LSC_3CH);	/* 3CH 4CH */
			sp1kLscCfgSet(SP1K_CALIB_LSC_RESOLUTION, 8);	/* 3CH:9 8 7; 4CH:7 6 5 */
			
			if(sp1kLscCal())
			{
				//printf("lsc cal err\n"); /* please change it to show fail on the screen */
				sp1kOsdStrDraw(2,5,"lsc cal err",2);
			}
			appUiBusyShow(1);
			menuProcDisp();
#endif
			 ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

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
UINT8 menuCalAECal (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer= layer;
	item=item;//just for clearing waring
	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalAe();
			break;
	}
	return ret; 
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
UINT8 menuCalWdtSet (UINT8 layer, UINT8 item, UINT16 msg) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			menuProcParaSet(layer, item);
			if(item == 1){//watch dog on
				dbgWdtFeed(15000);
			}
			dbgWdtSet(item);
			break;
	}
	return ret; 
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
UINT8 menuCalShowColorPalette (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer=layer;
	item=item;
	switch(msg){
		case SP1K_MSG_KEY_PRESS_SNAP:
		case SP1K_MSG_KEY_PRESS_RIGHT:
		case SP1K_MSG_KEY_PRESS_LEFT:	
		case SP1K_MSG_KEY_PRESS_UP:
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcDispPalette(msg);
			ret = MENU_IGNORE_DEFAULT;	
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;	
			break;
	}
	return ret; 
}

UINT8 menuStorageSpeedTest (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer=layer;
	item=item;
	switch(msg){
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalStorageTest();
			ret = MENU_IGNORE_DEFAULT;	
			break;
		case SP1K_MSG_KEY_PRESS_MENU:
			menuProcParent ();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;	
			break;			
	}

	return ret;
}

UINT8 menuCalAwbCtt (UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	layer= layer;//just for clearing waring

	switch (msg) {
		case SP1K_MSG_KEY_PRESS_SNAP:
			appCalAwbCalibration(item);
			menuProcParent ();
			menuProcDisp();			
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();			
			ret = MENU_IGNORE_DEFAULT;
			break;		
		case SP1K_MSG_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();			
			ret = MENU_IGNORE_DEFAULT;
			break;				
	}
	return ret; 

}
#endif
