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
 * @file      app_gui_obj.h
 * @brief     GUI object related function
 * @author    Phil Lin
 * @since     2011-09-02
 * @date      2011-09-02
*/

#ifndef __APP_GUI_OBJ_H__
#define __APP_GUI_OBJ_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "gui_api.h" 
#include "func_gui_def.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
	This definition could NOT more than 32
	Or refer to the private macro "GUI_OBJ_CTRL_MAX" in libGui
*/
#define GUI_OBJ_MAX	16  
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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT8 appGuiObjAdd(guiObj_t *pObj);
UINT8 appGuiObjDraw(guiObj_t *pObj, UINT8 objIdx);
UINT8 appGuiObjClr(UINT8 idx);
UINT8 appStillGuiExit(void);
UINT8 appVideoGuiExit(void);
UINT8 appStillPbGuiExit(void);
UINT8 appMenuGuiExit(void);

typedef enum _STILL_FUNC_s
{
	STILL_FUNC_NULL				= (unsigned char) 0x00,
	STILL_FUNC_WB					= (unsigned char) 0x01,
	STILL_FUNC_AEMETERING		= (unsigned char) 0x02,
	STILL_FUNC_EV					= (unsigned char) 0x03,
	STILL_FUNC_IMGSIZE				= (unsigned char) 0x04,
	STILL_FUNC_QUALITY			= (unsigned char) 0x05,

	
	STILL_FUNC_MAX = GUI_OBJ_MAX,
	
}STILL_FUNC;

typedef enum _VIDEO_FUNC_s
{
	VIDEO_FUNC_NULL				= (unsigned char) 0x00,
	VIDEO_FUNC_WB					= (unsigned char) 0x01,
	VIDEO_FUNC_AEMETERING		= (unsigned char) 0x02,
	VIDEO_FUNC_VIDSIZE			= (unsigned char) 0x03,
	
	VIDEO_FUNC_MAX = GUI_OBJ_MAX,
	
}VIDEO_FUNC;


typedef enum _STILL_PB_FUNC_s
{
	STILLPB_FUNC_NULL				= (unsigned char) 0x00,
	STILLPB_FUNC_SELIMG			= (unsigned char) 0x01,

	STILLPB_FUNC_MAX 				= GUI_OBJ_MAX,
	
}STILL_PB_FUNC;


typedef enum _MENU_FUNC_s
{
	MENU_FUNC_NULL		= (unsigned char) 0x00,
	MENU_FUNC_MENUMODE	= (unsigned char) 0x01,
	MENU_FUNC_SELITEM	= (unsigned char) 0x02,		

	MENU_FUNC_MAX = GUI_OBJ_MAX,
	
}MENU_FUNC;

typedef enum _STILL_OBJ_s
{
	STILL_OBJ_0			= (unsigned char) 0x00,
	STILL_OBJ_1			= (unsigned char) 0x01,
	STILL_OBJ_2			= (unsigned char) 0x02,
	STILL_OBJ_3			= (unsigned char) 0x03,
	STILL_OBJ_4			= (unsigned char) 0x04,
	STILL_OBJ_5			= (unsigned char) 0x05,
	STILL_OBJ_6			= (unsigned char) 0x06,
	STILL_OBJ_7			= (unsigned char) 0x07,
	STILL_OBJ_8			= (unsigned char) 0x08,
	STILL_OBJ_9			= (unsigned char) 0x09,
	STILL_OBJ_10		= (unsigned char) 0x0A,
	STILL_OBJ_11		= (unsigned char) 0x0B,
	STILL_OBJ_12		= (unsigned char) 0x0C,
	STILL_OBJ_13		= (unsigned char) 0x0D,
	STILL_OBJ_14		= (unsigned char) 0x0E,
	STILL_OBJ_15		= (unsigned char) 0x0F,
	STILL_OBJ_MAX 		= GUI_OBJ_MAX,
}STILL_OBJ;
typedef enum _VIDEO_OBJ_s
{
	VIDEO_OBJ_0			= (unsigned char) 0x00,
	VIDEO_OBJ_1			= (unsigned char) 0x01,
	VIDEO_OBJ_2			= (unsigned char) 0x02,
	VIDEO_OBJ_3			= (unsigned char) 0x03,
	VIDEO_OBJ_4			= (unsigned char) 0x04,
	VIDEO_OBJ_5			= (unsigned char) 0x05,
	VIDEO_OBJ_6			= (unsigned char) 0x06,
	VIDEO_OBJ_7			= (unsigned char) 0x07,
	VIDEO_OBJ_8			= (unsigned char) 0x08,
	VIDEO_OBJ_9			= (unsigned char) 0x09,
	VIDEO_OBJ_10			= (unsigned char) 0x0A,
	VIDEO_OBJ_11			= (unsigned char) 0x0B,
	VIDEO_OBJ_12			= (unsigned char) 0x0C,
	VIDEO_OBJ_13			= (unsigned char) 0x0D,
	VIDEO_OBJ_14			= (unsigned char) 0x0E,
	VIDEO_OBJ_15			= (unsigned char) 0x0F,
	VIDEO_OBJ_MAX 		= GUI_OBJ_MAX,
}VIDEO_OBJ;



typedef enum _PB_OBJ_s
{
	PB_OBJ_0		= (unsigned char) 0x00,
	PB_OBJ_1		= (unsigned char) 0x01,
	PB_OBJ_2		= (unsigned char) 0x02,
	PB_OBJ_3		= (unsigned char) 0x03,
	PB_OBJ_4		= (unsigned char) 0x04,
	PB_OBJ_5		= (unsigned char) 0x05,
	PB_OBJ_6		= (unsigned char) 0x06,
	PB_OBJ_7		= (unsigned char) 0x07,
	PB_OBJ_8		= (unsigned char) 0x08,
	PB_OBJ_9		= (unsigned char) 0x09,
	PB_OBJ_10		= (unsigned char) 0x0A,
	PB_OBJ_11		= (unsigned char) 0x0B,
	PB_OBJ_12		= (unsigned char) 0x0C,
	PB_OBJ_13		= (unsigned char) 0x0D,
	PB_OBJ_14		= (unsigned char) 0x0E,
	PB_OBJ_15		= (unsigned char) 0x0F,
	
	PB_OBJ_MAX 	= GUI_OBJ_MAX,
	PB_OBJ_ICON_PLAY,
	PB_OBJ_ICON_STOP,
	PB_OBJ_ICON_LEFT,
	PB_OBJ_ICON_DOWN,
}PB_OBJ;

typedef enum _MENU_OBJ_s
{
	MENU_OBJ_MODE_NOR	= (unsigned char) 0x00,
	MENU_OBJ_MODE_SETUP,	
	MENU_OBJ_ICON_OK,
	MENU_OBJ_ICON_BACK,
	
	MENU_OBJ_ITEM0,
	MENU_OBJ_ITEM1,
	MENU_OBJ_ITEM2,
	MENU_OBJ_ITEM3,
	MENU_OBJ_ITEM4,
	MENU_OBJ_ITEM5,

	MENU_OBJ_ICON_UP,
	MENU_OBJ_ICON_DOWN,
	MENU_OBJ_ICON_LEFT,
	MENU_OBJ_ICON_RIGHT,

	MENU_OBJ_MAX = GUI_OBJ_MAX,
	
}MENU_OBJ;


/*still view*/
UINT8 appStillGuiHdl_EvIncrease(void *ptr);
UINT8 appStillGuiHdl_ImgSizeCrease(void *ptr);
UINT8 appStillGuiHdl_WbCrease(void *ptr);
UINT8 appStillGuiHdl_AeMeteringCrease(void *ptr);
UINT8 appStillGuiHdl_ImgQualityCrease(void *ptr);

/*video view*/
UINT8 appVideoGuiHdl_WbCrease(void *ptr);
UINT8 appVideoGuiHdl_AeMeteringCrease(void *ptr);
UINT8 appVideoGuiHdl_SizeCrease(void *ptr);





/*still playback*/

UINT8 appStillPbGuiHdl_MultiObjSel(void *ptr);



/*menu*/

UINT8 appMenuGuiHdl_MenuMode(void *ptr);
UINT8 appMenuGuiHdl_SelItem(void *ptr);

void appGuiEnableSet(UINT8 enable);

UINT8 appGuiEnableGet(void);

 
#endif  /* __APP_GUI_OBJ_H__ */

