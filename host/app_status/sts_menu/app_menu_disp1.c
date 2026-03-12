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
#include "app_main.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "app_menu.h"
#include "app_menu_api.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_micro.h"
#include "app_menu_event.h"
#include "App_ui_para.h"
#include "app_still_play.h"
#include "sp1k_disp_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "initio.h"
#include "smcfunc.h"
#include "dbg_def.h"
#include "osd_buff.h"
#include "hal_gprm.h"
#include "osd_mem.h"
#include "osd_api.h"
#include "utility.h"



#include "app_gui_obj.h" 
#include "func_gui_def.h" 


extern UINT16 MENU_CHAR_WIDTH;//  16
extern UINT16 MENU_CHAR_HEIGHT;// 30//31

extern void menuIconDispExt(UINT16 x, UINT16 y, UINT16 icon, UINT8 blendOpt);
extern void menuStrDispExt(UINT16 x, UINT16 y, UINT8* str, UINT8 blendOpt);

#if FUNC_HOST_TOUCH_PANEL
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcButIconDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuProcButIconDisp(UINT16 attr,UINT16 x0,UINT16 y0,UINT16 w,UINT16 h,UINT8 objIdx,UINT16 iconIdx,UINT8 funcIdx,UINT16 cnt,UINT16 msg,UINT8 penRepeateEn)
{
	UINT16 iconw=sp1kOsdIconWidthGet((UINT16)iconIdx);
	

	if (iconIdx !=ID_ICON_NONE && attr!=0xff)
	{
		appOsdColorSet(attr);		
		//menuIconDispExt((iconw>=w?x0-(iconw-w)/2:(x0+(w-iconw)/2)), y0, iconIdx,0);
		menuIconDispExt(x0, y0, iconIdx,0);
	}
	//menuGuiDraw(x0, y0, w, h, objIdx, funcGuiHdlGet(FUNC_GUI_HDL_IDX_MENU, funcIdx),cnt,msg);
	menuGuiDraw(x0, y0, w, h, objIdx,(pFuncGui)0,funcGuiHdlGet(FUNC_GUI_HDL_IDX_MENU, funcIdx),(pFuncGui)0,(pFuncGui)0,0,cnt,0, 0,msg,penRepeateEn);
}

void appTPIconReg(UINT8 objId,UINT16 iconIdx,UINT8 funcIdx,funcGuiHdlGrp_t mode,UINT8 cnt,UINT16 msg,UINT8 penRepeateEn)
{
	UINT16 iconW,iconH;
	UINT16 x,y;
	
	sp1kOsdIconPsGet((UINT16)iconIdx,&x,&y);
	iconW=sp1kOsdIconWidthGet((UINT16)iconIdx);
	iconH = sp1kOsdIconHeightGet((UINT16)iconIdx);
	
	//menuGuiDraw(x, y, iconW, iconH, objId, funcGuiHdlGet(mode, funcIdx),cnt,msg);
	menuGuiDraw(x, y, iconW, iconH, objId, (pFuncGui)0    ,funcGuiHdlGet(mode, funcIdx),(pFuncGui)0,(pFuncGui)0, 0 ,cnt,0, 0,msg,penRepeateEn);
}

void appTPRangeReg(UINT8 objId,UINT16 x,UINT16 y,UINT16 width,UINT16 height,UINT8 funcIdx,funcGuiHdlGrp_t mode,UINT8 cnt,UINT16 msg,UINT8 penRepeateEn)
{
	if(width && height)
	{
		//menuGuiDraw(x, y, width, height, objId, funcGuiHdlGet(mode, funcIdx),cnt,msg);
		menuGuiDraw(x, y, width, height, objId, (pFuncGui)0,funcGuiHdlGet(mode, funcIdx),(pFuncGui)0,(pFuncGui)0,0, cnt, 0, 0,msg,penRepeateEn);
	}
}



/**************************************************************************
 *                                                                        *
 *  Function Name: appGuiObjIconClr                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

void appGuiObjIconClr(UINT8 sIdx,UINT8 eIdx)
{
	printf("GUI clean!!\n");
	menuGuiClr(sIdx, eIdx);
}
#endif
