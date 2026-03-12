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




/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define 	BUFSIZE		64

//#define MENU_MODE_X_ODDSET
static UINT16 MENU_MODE_Y_OFFSET;// 4

UINT16 MENU_CHAR_WIDTH;//  16
UINT16 MENU_CHAR_HEIGHT;// 30//31

static UINT16 MENU_LAYER1_STR_X_OFFSET;// 32
static UINT16 MENU_LAYER1_STR_Y_OFFSET;// 		(43+MENU_MODE_Y_OFFSET)//44
static SINT16 MENU_LAYER1_ICON_X_OFFSET;// (-16)
static UINT16 MENU_LAYER1_ICON_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET)//44
static UINT16 MENU_LAYER2_STR_X_OFFSET;// 56
static UINT16 MENU_LAYER2_STR_Y_OFFSET;// 60
static UINT16 MENU_LAYER2_ICON_X_OFFSET;// 4
static UINT16 MENU_LAYER2_ICON_Y_OFFSET;// 60

static UINT16 MENU_BAR_LAYER1_X_OFFSET;// 24
static UINT16 MENU_BAR_LAYER1_Y_OFFSET;// (43+MENU_MODE_Y_OFFSET-3)//44
static UINT16 MENU_BAR_LAYER2_X_OFFSET;// 48
static UINT16 MENU_BAR_LAYER2_Y_OFFSET;// 61-3
static UINT16 MENU_BAR_LAYER3_X_OFFSET;// 32
static UINT16 MENU_BAR_LAYER3_Y_OFFSET;// 44

static UINT16 MENU_BODY_LAYER1_X_OFFSET;// 16
static UINT16 MENU_BODY_LAYER1_Y_OFFSET;// (33+MENU_MODE_Y_OFFSET)//34
static UINT16 MENU_BODY_LAYER2_X_OFFSET;// 40
static UINT16 MENU_BODY_LAYER2_Y_OFFSET;// 47//49
static UINT16 MENU_BODY_LAYER3_X_OFFSET;// 14
static UINT16 MENU_BODY_LAYER3_Y_OFFSET;// 96-3

static UINT16 MENU_OFFSET; 
static UINT16 MENU_MU2BARSL_WIDTH;
static UINT16 MENU_HEADER_OFFSET;
static UINT16 MENU_BACK_ON_OFFSET;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 menuXOff=1;
static UINT8 menuYOff=1;
UINT32 menuPosBackup;
UINT16 LCD_WIDTH;// ((UINT16)320)
UINT16 LCD_HEIGHT;// ((UINT16)240)
UINT16 strLen1,strLen2;


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//"MM/DD/YYYY",
//"DD/MM/YYYY",
//"YYYY/MM/DD",
UINT8 code rtc_disp[3][6]={{1, 2, 0, 3, 4, 5}, {2, 1, 0, 3, 4, 5}, {0, 1, 2, 3, 4, 5}};
//UINT8 code exposure[]="\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e\x7f\x7f\x7e";

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern UINT8 OsdMaxX;
//extern UINT8 OsdMaxY;
//extern UINT8 OsdMaxNum;
extern SIMAGE xdata pbFile;
extern UINT8 enterMacroadjust;

void appOsdStrLenCalculate(void)
{
	UINT8 str1[]="444:44:44";
	UINT8 str2[]="44:44:44";
	strLen1 =strWidthGet(str1);
	strLen2 =strWidthGet(str2);
}

void menuDispParaInit(void)
{
	UINT16 charw,charh;
	
	sp1kOsdFontSizeGet(&charw,&charh);
	appOsdStrLenCalculate();

	if(appPanelTypeGet()== 1)
	{
		switch(charh)
		{
		case 32:
			LCD_WIDTH = 320;
			LCD_HEIGHT = 240;
			//printf("font 32\n");
			MENU_MODE_Y_OFFSET = 4;

			MENU_CHAR_WIDTH = 16;
			MENU_CHAR_HEIGHT = 30;//31

			MENU_LAYER1_STR_X_OFFSET = 32;
			MENU_LAYER1_STR_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);//44
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET = -MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = 0;//(-16);
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);//44
			MENU_LAYER2_STR_X_OFFSET = 56;
			MENU_LAYER2_STR_Y_OFFSET = 60;
			MENU_LAYER2_ICON_X_OFFSET = 4;
			MENU_LAYER2_ICON_Y_OFFSET = 60;

			MENU_BAR_LAYER1_X_OFFSET = 24;
			MENU_BAR_LAYER1_Y_OFFSET = (43+MENU_MODE_Y_OFFSET-3);//44
			MENU_BAR_LAYER2_X_OFFSET = 48;
			MENU_BAR_LAYER2_Y_OFFSET = 61-3;
			MENU_BAR_LAYER3_X_OFFSET = 32;
			MENU_BAR_LAYER3_Y_OFFSET = 44;

			MENU_BODY_LAYER1_X_OFFSET = 16;
			MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);//34
			MENU_BODY_LAYER2_X_OFFSET = 40;
			MENU_BODY_LAYER2_Y_OFFSET = 47;//49
			MENU_BODY_LAYER3_X_OFFSET = 14;
			MENU_BODY_LAYER3_Y_OFFSET = 96-3;

			MENU_OFFSET = 12;
			MENU_MU2BARSL_WIDTH = 54;
			MENU_HEADER_OFFSET = 6;
			MENU_BACK_ON_OFFSET = 3;
			break;
		case 24:
			LCD_WIDTH = 220;
			LCD_HEIGHT = 176;
			//printf("font 24\n");
			MENU_MODE_Y_OFFSET = 2;

			MENU_CHAR_WIDTH = 12;
			MENU_CHAR_HEIGHT = 24;//31

			MENU_LAYER1_STR_X_OFFSET = 24;
			MENU_LAYER1_STR_Y_OFFSET = (28+MENU_MODE_Y_OFFSET);
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET = (-8)-MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = (-8);
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (30+MENU_MODE_Y_OFFSET);
			MENU_LAYER2_STR_X_OFFSET = 41;
			MENU_LAYER2_STR_Y_OFFSET = 46;
			MENU_LAYER2_ICON_X_OFFSET = 0;
			MENU_LAYER2_ICON_Y_OFFSET = 47;

			MENU_BAR_LAYER1_X_OFFSET = 16;
			MENU_BAR_LAYER1_Y_OFFSET = (32+MENU_MODE_Y_OFFSET-3);
			MENU_BAR_LAYER2_X_OFFSET = 33;
			MENU_BAR_LAYER2_Y_OFFSET = 46;
			MENU_BAR_LAYER3_X_OFFSET = 21;
			MENU_BAR_LAYER3_Y_OFFSET = 35;

			MENU_BODY_LAYER1_X_OFFSET = 8;
			MENU_BODY_LAYER1_Y_OFFSET = (23+MENU_MODE_Y_OFFSET);
			MENU_BODY_LAYER2_X_OFFSET = 25;
			MENU_BODY_LAYER2_Y_OFFSET = 37;
			MENU_BODY_LAYER3_X_OFFSET = 10;
			MENU_BODY_LAYER3_Y_OFFSET = 73;

			MENU_OFFSET = 0;
			MENU_MU2BARSL_WIDTH = 43;
			MENU_HEADER_OFFSET = 6;
			MENU_BACK_ON_OFFSET = 2;
			break;
		case 16:
			LCD_WIDTH = 160;
			LCD_HEIGHT = 128;
			//printf("font 16\n");
			MENU_MODE_Y_OFFSET = 4;

			MENU_CHAR_WIDTH = 8;
			MENU_CHAR_HEIGHT = 16;

			MENU_LAYER1_STR_X_OFFSET = 22;
			MENU_LAYER1_STR_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
			#if FUNC_HOST_TOUCH_PANEL
			MENU_LAYER1_ICON_X_OFFSET = 4-MENU_CHAR_WIDTH*2;
			#else
			MENU_LAYER1_ICON_X_OFFSET = 4;
			#endif
			MENU_LAYER1_ICON_Y_OFFSET = (21+MENU_MODE_Y_OFFSET);
			MENU_LAYER2_STR_X_OFFSET = 37;
			MENU_LAYER2_STR_Y_OFFSET = 32;
			MENU_LAYER2_ICON_X_OFFSET = 4;
			MENU_LAYER2_ICON_Y_OFFSET = 32;

			MENU_BAR_LAYER1_X_OFFSET = 16;
			MENU_BAR_LAYER1_Y_OFFSET = (24+MENU_MODE_Y_OFFSET-3);
			MENU_BAR_LAYER2_X_OFFSET = 28;
			MENU_BAR_LAYER2_Y_OFFSET = 32;
			MENU_BAR_LAYER3_X_OFFSET = 20;
			MENU_BAR_LAYER3_Y_OFFSET = 27;

			MENU_BODY_LAYER1_X_OFFSET = 10;
			MENU_BODY_LAYER1_Y_OFFSET = (16+MENU_MODE_Y_OFFSET);
			MENU_BODY_LAYER2_X_OFFSET = 20;
			MENU_BODY_LAYER2_Y_OFFSET = 24;
			MENU_BODY_LAYER3_X_OFFSET = 12;
			MENU_BODY_LAYER3_Y_OFFSET = 33;

			MENU_OFFSET = 10;
			MENU_MU2BARSL_WIDTH = 38;
			MENU_HEADER_OFFSET = 3; 
			MENU_BACK_ON_OFFSET = 2;
			break;
		}
	}else{
		LCD_WIDTH = 320;
		LCD_HEIGHT = 240;
		//printf("TV font 32\n");
		MENU_MODE_Y_OFFSET = 4;

		MENU_CHAR_WIDTH = 16;
		MENU_CHAR_HEIGHT = 30;

		MENU_LAYER1_STR_X_OFFSET = 32;
		MENU_LAYER1_STR_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);
		#if FUNC_HOST_TOUCH_PANEL
		MENU_LAYER1_ICON_X_OFFSET = 0-MENU_CHAR_WIDTH*2;
		#else
		MENU_LAYER1_ICON_X_OFFSET = 0;
		#endif
		MENU_LAYER1_ICON_Y_OFFSET = (43+MENU_MODE_Y_OFFSET);
		MENU_LAYER2_STR_X_OFFSET = 56;
		MENU_LAYER2_STR_Y_OFFSET = 60;
		MENU_LAYER2_ICON_X_OFFSET = 4;
		MENU_LAYER2_ICON_Y_OFFSET = 60;

		MENU_BAR_LAYER1_X_OFFSET = 24;
		MENU_BAR_LAYER1_Y_OFFSET = (43+MENU_MODE_Y_OFFSET-3);
		MENU_BAR_LAYER2_X_OFFSET = 48;
		MENU_BAR_LAYER2_Y_OFFSET = 61-3;
		MENU_BAR_LAYER3_X_OFFSET = 32;
		MENU_BAR_LAYER3_Y_OFFSET = 44;

		MENU_BODY_LAYER1_X_OFFSET = 16;
		MENU_BODY_LAYER1_Y_OFFSET = (33+MENU_MODE_Y_OFFSET);
		MENU_BODY_LAYER2_X_OFFSET = 40;
		MENU_BODY_LAYER2_Y_OFFSET = 47;
		MENU_BODY_LAYER3_X_OFFSET = 14;
		MENU_BODY_LAYER3_Y_OFFSET = 96-3;

		MENU_OFFSET = 12;
		MENU_MU2BARSL_WIDTH = 54;
		MENU_HEADER_OFFSET = 6;
		MENU_BACK_ON_OFFSET = 3;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuLanguageGet                                        *
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
UINT8 menuLanguageGet(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if (puiPara->Language >= 24/*LANGUAGE_MAX*/)
		puiPara->Language = ENGLISH;

	return puiPara->Language;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuDateItemConv                                       *
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
UINT8 menuDateItemConv(UINT8 item)
{
	uiPara_t* puiPara = appUiParaGet();

	return rtc_disp[puiPara->DateStyle][item];
}



/**************************************************************************
 *                                                                        *
 *  Function Name: menuLayerSizeGet                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *0
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuLayerSizeGet(UINT8 layer, UINT8* line, UINT8* col)
{
	UINT8 OsdMaxX, OsdMaxY;
	UINT16 cw,ch;
	
	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	sp1kOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;
		
	if (layer != 2)
	{
		*col = OsdMaxX - 2;
		*line = OsdMaxY - 2;
	}
	else
	{
		*col = OsdMaxX-5;
		*line = OsdMaxY-3;
	}
}

void appOsdColorSet(UINT8 attr)
{
	UINT8 idxBg, idxBlack, idxBld0, idxBld1;
	switch (attr)
	{
		case MENU_TAB_GET_FOCUS:
		case MENU_BAR_GET_FOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			if (menuProcCurrRootGet() == MENU_MARK_SETUP)
			{
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;
			}
			else
			{
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;//OSD_ATTR_COLOR_GREEN;
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;//OSD_ATTR_COLOR_GREEN;
			}
			break;
		case MENU_TAB_LOST_TFOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			if (menuProcCurrRootGet() == MENU_MARK_SETUP)
			{
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;
			}
			else
			{
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;//OSD_ATTR_COLOR_GREEN;
			}
			break;
		case MENU_TAB_DISABLE:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			if (menuProcCurrRootGet() == MENU_MARK_SETUP)
			{
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			}
			else
			{
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
				idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			}
			break;
		case MENU_BAR_DISABLE:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case MENU_BODY_GET_FOCUS:
			idxBg = OSD_ATTR_MODE_HALFT1 | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			if (menuProcCurrRootGet() == MENU_MARK_SETUP)
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;
			else
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;//OSD_ATTR_COLOR_GREEN;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case MENU_BODY_LOST_FOCUS:
			idxBg = OSD_ATTR_MODE_HALFT1 | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREEN;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case MENU_BODY_DISABLE:
			idxBg = OSD_ATTR_MODE_HALFT1 | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case MENU_STR_GET_FOCUS:
		case MENU_ICON_GET_FOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			break;
		case MENU_STR_LOST_FOCUS:
		case MENU_ICON_LOST_FOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			break;
		case MENU_STR_DISABLE:
		case MENU_ICON_DISABLE:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case MENU_PROMPT_GET_FOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			break; 
		case MENU_DLG_GET_FOCUS:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_HALFT1 | OSD_ATTR_COLOR_BLACK;
			if (menuProcCurrRootGet() == MENU_MARK_SETUP)
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;
			else
				idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_CYAN;//OSD_ATTR_COLOR_GREEN;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case OSD_COLOR_WHITE:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			break;
		case OSD_COLOR_BLACK:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			break;

		case OSD_COLOR_RED:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_RED;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_RED;
			break;
		case OSD_COLOR_YELLOW:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_YELLOW;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			break;
		case OSD_COLOR_GREEN:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREEN;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREEN;
			break;
		case OSD_COLOR_BLUE:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLUE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLUE;
			break;

		case OSD_COLOR_GREY:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREY;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREY;
			break;
		case OSD_COLOR_PURPLE:
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_LINE;
			break;
		case OSD_COLOR_PINK:
			idxBld0 = OSD_ATTR_MODE_BLD| OSD_ATTR_COLOR_PINK;//OSD_ATTR_MODE_BLD
			idxBld1 = OSD_ATTR_MODE_BLD| OSD_ATTR_COLOR_PINK;//OSD_ATTR_MODE_BLD
			break;
		default:
			idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
			idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
			idxBld0 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			idxBld1 = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_WHITE;
			break;
	}

	if((attr<=27) && (attr>=19))
	{
		idxBg = OSD_ATTR_MODE_TRANS | OSD_ATTR_COLOR_BLACK;
		idxBlack = OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLACK;
	}
	osdMapTabSet(idxBg,idxBlack,idxBld0,idxBld1);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuStrDisp                                            *
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
UINT8 menuStrDisp(UINT8 x, UINT8 y, UINT8* str, UINT8 blendOpt, UINT8 layer)
{
	UINT8 strlegth=0;
	UINT16 offset = x*MENU_CHAR_WIDTH;
	blendOpt = 1;

	if(layer == 2)
	{
		sp1kOsdStrDrawExt(offset+MENU_LAYER2_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, str,blendOpt);
	}
	else
	{
		sp1kOsdStrDrawExt(offset+MENU_LAYER1_STR_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_STR_Y_OFFSET, str,blendOpt);
	}
	strlegth=strlen(str);
	return strlegth;
}

void menuStrDispExt(UINT16 x, UINT16 y, UINT8* str, UINT8 blendOpt)
{
	sp1kOsdStrDrawExt(x,y,str,blendOpt);
}

void menuBarDisp(UINT8 line, UINT8 layer)
{
	UINT8 attr = MENU_BAR_GET_FOCUS;

	if ((layer == 1) && (menuProcLayerGet() == 2))
		attr = MENU_BAR_DISABLE;

	appOsdColorSet(attr);
	if(layer == 2){
		if( menuProcSubItemGet(layer,menuProcScrollGet(layer))){
			osdIconDrawExt(MENU_BAR_LAYER2_X_OFFSET, (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET+line, ID_ICON_MUSUSBAR, 1);
		}else {
			osdIconDrawExt(MENU_BAR_LAYER2_X_OFFSET, (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET+line, ID_ICON_MUSBAR, 1);
		}

	}
	else if(layer == 3)
		osdIconDrawExt(MENU_BAR_LAYER3_X_OFFSET, (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER3_Y_OFFSET, ID_ICON_MUDLGBAR, 1);		
	else
		osdIconDrawExt(MENU_BAR_LAYER1_X_OFFSET, (UINT16)line*MENU_CHAR_HEIGHT+MENU_BAR_LAYER1_Y_OFFSET+line, ID_ICON_MUBAR, 1);

}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuIconDisp                                           *
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
void menuIconDisp(UINT8 x, UINT8 y, UINT8 icon, UINT8 blendOpt, UINT8 layer)
{
	if (icon == ID_ICON_NONE)
		return;

	blendOpt = 1;
	if(layer == 2) {
		osdIconDrawExt((UINT16)x*MENU_CHAR_WIDTH+MENU_LAYER2_ICON_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER2_ICON_Y_OFFSET, icon, blendOpt);
	}
	else {
		osdIconDrawExt((UINT16)(x-1)*MENU_CHAR_WIDTH+MENU_LAYER1_ICON_X_OFFSET, (UINT16)y*MENU_CHAR_HEIGHT+MENU_LAYER1_ICON_Y_OFFSET, icon, blendOpt);
	}
}

void menuIconDispExt(UINT16 x, UINT16 y, UINT16 icon, UINT8 blendOpt)
{
	osdIconDrawExt(x, y, icon, blendOpt);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuHeaderDisp                                         *
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
void menuHeaderDisp(UINT8 layer)
{
	UINT16 icon;

	if(layer == -1)
	{
		return;
	}
	
	if (layer == 0)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
	
			
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);

			appOsdColorSet(MENU_STR_GET_FOCUS);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
			}
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);

			appOsdColorSet(MENU_STR_GET_FOCUS);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
			
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);

			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
			}
		}
	}
	else if (layer == 1)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);	
			appOsdColorSet(MENU_ICON_DISABLE);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
	
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			
			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
			}
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			
			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
			}
		}
	}
	else if (layer == 2)
	{
		if (menuProcCurrRootGet() == MENU_MARK_SETUP)
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
	
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);
			appOsdColorSet(MENU_ICON_DISABLE);
			
			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
			}
		}
		else
		{
			icon = menuProcIconIDGetByPos(menuPosBackup);
	
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			osdIconDrawExt(2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET-2, MENU_MODE_Y_OFFSET, ID_ICON_MUTAB,0);
			appOsdColorSet(MENU_ICON_DISABLE);
			osdIconDrawExt(3*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,1);
	
			appOsdColorSet(MENU_ICON_DISABLE);
			icon = menuProcIconIDGetByPos(MENU_MARK_SETUP);
			
			if(appPreStateGet(0) != APP_STATE_DPS){
				osdIconDrawExt(6*MENU_CHAR_WIDTH, MENU_MODE_Y_OFFSET+2, icon,0);
			}
		}
	}
	#if FUNC_HOST_TOUCH_PANEL
	menuProcButIconDisp(0xff,2*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_NOR,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x00,SP1K_MSG_NULL,0);
	
	menuProcButIconDisp(0xff,5*MENU_CHAR_WIDTH+MENU_HEADER_OFFSET,MENU_MODE_Y_OFFSET,\
		3*MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT,\
		MENU_OBJ_MODE_SETUP,ID_ICON_NONE,MENU_FUNC_MENUMODE,0x01,SP1K_MSG_NULL,0);
	#endif	
}
void menuBodyDisp(UINT8 layer)
{
	UINT8 attr = MENU_BODY_GET_FOCUS;
	#if FUNC_HOST_TOUCH_PANEL
	UINT8 line,col;
	UINT8 nodeItem;
	if(layer)
	{
		menuLayerSizeGet(layer, &line, &col);
		nodeItem = menuProcNodeItemGet(layer, 0);
	}
	else
	{
		menuLayerSizeGet(1, &line, &col);
		nodeItem = menuProcNodeItemGet(1, 0);
	}
	#endif
	if ((layer == 1) && (menuProcLayerGet() == 2))
	{
		attr = MENU_BODY_DISABLE;
	}
	else if (layer == 3)
	{
		attr = MENU_DLG_GET_FOCUS;
	}

	appOsdColorSet(attr);

	//sp1kOsdIndexSave();
	//sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLUE, 4);
	//sp1kOsdRectDraw(0, 0, 320 , 240);
	//sp1kOsdIndexRestore();
	
	if(layer == 2)
	{
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,1);
		#if FUNC_HOST_TOUCH_PANEL
		if(nodeItem<=line)
		{
			attr=MENU_BODY_DISABLE;
		}

		menuProcButIconDisp(attr,MENU_BODY_LAYER2_X_OFFSET + 246-MENU_CHAR_WIDTH,MENU_BODY_LAYER2_Y_OFFSET+39-MENU_CHAR_HEIGHT,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_UP,0);
		menuProcButIconDisp(attr,MENU_BODY_LAYER2_X_OFFSET + 246-MENU_CHAR_WIDTH,MENU_BODY_LAYER2_Y_OFFSET+104,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_DOWN,0);		
		#endif
	}
	else if (layer == 3)
	{
		osdIconDrawExt(MENU_BODY_LAYER3_X_OFFSET, MENU_BODY_LAYER3_Y_OFFSET, ID_ICON_MUDLG,1);
	}
	else
	{
		osdIconDrawExt(MENU_BODY_LAYER1_X_OFFSET, MENU_BODY_LAYER1_Y_OFFSET, ID_ICON_MUBODY,1);
		#if FUNC_HOST_TOUCH_PANEL
		if(nodeItem<=line)
		{
			attr=MENU_BODY_DISABLE;
		}
		menuProcButIconDisp(attr,MENU_BODY_LAYER1_X_OFFSET + 268-MENU_CHAR_WIDTH,MENU_BODY_LAYER1_Y_OFFSET+35-MENU_CHAR_HEIGHT,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_UP,0);
		menuProcButIconDisp(attr,MENU_BODY_LAYER1_X_OFFSET + 268-MENU_CHAR_WIDTH,MENU_BODY_LAYER1_Y_OFFSET+132,\
			MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
			MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_DOWN,0);
		#endif
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuPromptDisp                                         *
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
void menuPromptDisp(UINT16 attr)
{
	UINT8	line, col;
	UINT8 OsdMaxX, OsdMaxY;
	UINT8 buf[BUFSIZE];
	UINT16 cw,ch;
	
	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	sp1kOsdFontSizeGet(&cw, &ch);
	if(ch <= 16)OsdMaxY-=1;
	
	line = OsdMaxY;
	col = OsdMaxX;

	appOsdColorSet(MENU_PROMPT_GET_FOCUS);
	if (attr & MENU_PROMPT_OK_ON)
	{
	#if FUNC_HOST_TOUCH_PANEL
		menuIconDispExt((MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, ID_ICON_BUTTON_SET,0);
		strGet(buf, BUFSIZE,ID_STR_ENTER);
		menuStrDispExt((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, buf, 0);

		menuProcButIconDisp(0xff,(MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH,(line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET,\
		(2*MENU_CHAR_WIDTH+2+strWidthGet(buf)),MENU_CHAR_HEIGHT,\
		MENU_OBJ_ICON_OK,ID_ICON_NONE,MENU_FUNC_NULL,0x00,SP1K_MSG_TP_OK,0);	
	#else
		menuIconDispExt((MENU_BACK_ON_OFFSET)*MENU_CHAR_WIDTH, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, ID_ICON_BUTTON_OK,0);
		strGet(buf, BUFSIZE,ID_STR_ENTER);
		menuStrDispExt((MENU_BACK_ON_OFFSET+2)*MENU_CHAR_WIDTH+2, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, buf, 0);
	#endif
	}
	

	
	
	if (attr & MENU_PROMPT_MENU_ON)
	{
	#if FUNC_HOST_TOUCH_PANEL
		menuIconDispExt((MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH-2, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, ID_ICON_BACK,0);
		strGet(buf, BUFSIZE,ID_STR_BACK);
		menuStrDispExt((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2-2, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET,buf, 0);

		menuProcButIconDisp(0xff,(MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH,(line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET,\
		(2*MENU_CHAR_WIDTH+2+strWidthGet(buf)),MENU_CHAR_HEIGHT,\
		MENU_OBJ_ICON_BACK,ID_ICON_NONE,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_MENU,0);	

	#else
		menuIconDispExt((MENU_BACK_ON_OFFSET+10)*MENU_CHAR_WIDTH, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET, ID_ICON_BUTTON_MENU/*ID_ICON_BUTTON_MENU*/,0);
		strGet(buf, BUFSIZE,ID_STR_BACK);
		menuStrDispExt((MENU_BACK_ON_OFFSET+12)*MENU_CHAR_WIDTH+2, (line-1)*(MENU_CHAR_HEIGHT+2)+MENU_OFFSET,buf, 0);

	#endif
	}
		
}
extern UINT8 specialMenuOrder[30];
extern UINT8 specialMenuCurrItemIdx;
extern UINT8 specialMenuItemNum;

void menuSpecialSequenceDisp(void)
{	
	UINT8 	buf[BUFSIZE];
	UINT8	icon;
	UINT8	nodeItem = 0;
	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	layer = 0;
	UINT8	item = 0;
	UINT8	line, col;
	UINT16	i;
	UINT32	menuRoot;

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	layer = 1;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	#if FUNC_HOST_TOUCH_PANEL
	menuGuiClr(0, MENU_OBJ_MAX);
	#endif

	menuHeaderDisp(layer);
	menuBodyDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDisp(0, i, buf, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
	}


	layer = 2;
	item = 0;
	#if FUNC_HOST_TOUCH_PANEL
	menuGuiClr(0, MENU_OBJ_MAX);
	#endif
	menuHeaderDisp(layer);
	menuBodyDisp(layer);

	menuLayerSizeGet(layer, &line, &col);

	nodeItem = specialMenuItemNum;

	item = (specialMenuCurrItemIdx/line)*line;
	nodeItem = nodeItem - item;
	if(nodeItem > line)
	{
		nodeItem = line;
	}
	

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,specialMenuOrder[item],buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, specialMenuOrder[item]);

		if (!menuProcIsActive(layer, specialMenuOrder[item]))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			if(menuRoot == MENU_MARK_PLAYBACK)
			{
				menuStrDisp(0, i, buf, 0, layer);
			}
			else
			{
				menuStrDisp(4, i, buf, 0, layer);
				menuIconDisp(4, i, icon, 0, layer);
			}
		}
		else if (specialMenuCurrItemIdx== item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			
			if(menuRoot == MENU_MARK_PLAYBACK)
			{
				menuStrDisp(0, i, buf, 1, layer);
			}
			else
			{
				menuStrDisp(4, i, buf, 1, layer);
				appOsdColorSet(MENU_ICON_GET_FOCUS);
				menuIconDisp(4, i, icon, 1, layer);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			if(menuRoot == MENU_MARK_PLAYBACK) 
			{
				menuStrDisp(0, i, buf, 0, layer);
			}
			else
			{
				menuStrDisp(4, i, buf, 0, layer);
				menuIconDisp(4, i, icon, 0, layer);
			}
		}
		#if FUNC_HOST_TOUCH_PANEL
		menuProcButIconDisp(0xff,MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
			sp1kOsdIconWidthGet(ID_ICON_MUSBAR),sp1kOsdIconHeightGet(ID_ICON_MUSBAR),\
			 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
			);
		#endif
	}
	#if FUNC_HOST_TOUCH_PANEL
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	#else
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
	#endif
	sp1kOsdDispBuffShow();

}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcDisp                                           *
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
void menuProcDisp (void)
{
	UINT8 	buf[BUFSIZE];
	UINT8	icon;
	UINT8	nodeItem = 0;
	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	layer = 0;
	UINT8	item = 0;
	UINT8	line, col;
	UINT16	i;
	UINT16	buffW,buffH;
	sp1kDispCapabDesc_t dispCap;
	UINT32	menuRoot;
	UINT8	xpos=1;
	UINT8	ypos=1;
	UINT8	attr=0;
	UINT8 OsdMaxX, OsdMaxY;
	uiPara_t* puiPara = appUiParaGet();

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	sp1kDispCapabilityGet(sp1kDispDevActGet(), &dispCap);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);
	menuRoot = menuProcCurrRootGet();
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	
	#if 1
	 /* Highlight the tab */
	if (menuProcLayerGet() == 0)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif
		menuHeaderDisp(0);
		menuBodyDisp(0);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		

		for(i=0; i<nodeItem; i++,item++)
		{
			menuStrGet(layer,item,buf,BUFSIZE);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);
			
			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}
			if (menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDisp(0, i, buf, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDisp(0, i, buf, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				(sp1kOsdIconWidthGet(ID_ICON_MUBAR)),sp1kOsdIconHeightGet(ID_ICON_MUBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif

	#if 1 /* Highlight the setting menu */
	else if (menuProcLayerGet() == 1)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif
		if(puiPara->redEyePb == REDEYEPB_SAVE)
		{
			menuHeaderDisp(-1);
		}
		else
		{
			menuHeaderDisp(layer);
		}

		menuBodyDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);


		for(i=0; i<nodeItem; i++,item++)
		{
			menuStrGet(layer,item,buf,BUFSIZE);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);

			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDisp(0, i, buf, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDisp(0, i, buf, 1, layer);
				appOsdColorSet(MENU_ICON_GET_FOCUS);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDisp(0, i, buf,  0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER1_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER1_STR_Y_OFFSET,\
				(sp1kOsdIconWidthGet(ID_ICON_MUBAR)),sp1kOsdIconHeightGet(ID_ICON_MUBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif

		}

		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif

	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 2)
	{
		layer = 1;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif

		menuHeaderDisp(layer);
		menuBodyDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuStrGet(layer,item,buf,BUFSIZE);
			icon = menuProcIconIDGet(layer, item);
			subItem = menuProcSubItemGet(layer, item);
			
			if (subItem > 0)
			{
				selItem = menuProcParaGet (layer, item);
				icon = menuProcChildIconIDGet(layer, item, selItem);
			}

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDisp(0, i, buf, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDisp(0, i, buf, 1, layer);
				menuIconDisp(col-2, i, icon, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDisp(0, i, buf, 0, layer);
				menuIconDisp(col-2, i, icon, 0, layer);
			}
		}
 

		layer = 2;
		item = 0;
		#if FUNC_HOST_TOUCH_PANEL
		menuGuiClr(0, MENU_OBJ_MAX);
		#endif
		menuHeaderDisp(layer);
		menuBodyDisp(layer);

		menuLayerSizeGet(layer, &line, &col);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);;

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);
 

		for(i=0; i<nodeItem; i++,item++)
		{
			menuStrGet(layer,item,buf,BUFSIZE);
			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDisp(0, i, buf, 0, layer);
				}
				else
				{
					menuStrDisp(4, i, buf, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i, layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				
				if(menuRoot == MENU_MARK_PLAYBACK)
				{
					menuStrDisp(0, i, buf, 1, layer);
				}
				else
				{
					menuStrDisp(4, i, buf, 1, layer);
					appOsdColorSet(MENU_ICON_GET_FOCUS);
					menuIconDisp(4, i, icon, 1, layer);
				}
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				if(menuRoot == MENU_MARK_PLAYBACK) 
				{
					menuStrDisp(0, i, buf, 0, layer);
				}
				else
				{
					menuStrDisp(4, i, buf, 0, layer);
					menuIconDisp(4, i, icon, 0, layer);
				}
			}
			#if FUNC_HOST_TOUCH_PANEL
			menuProcButIconDisp(0xff,MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
				sp1kOsdIconWidthGet(ID_ICON_MUSBAR),sp1kOsdIconHeightGet(ID_ICON_MUSBAR),\
				 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);
			#endif
		}
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
		#endif
	}
	#endif

	#if 1 /* Highlight the option menu */
	else if (menuProcLayerGet() == 3)
	{
		layer = 3;
		item = 0;
		menuLayerSizeGet(layer, &line, &col);

		//menuHeaderDisp(layer);
		menuBodyDisp(layer);

		nodeItem = menuProcNodeItemGet(layer, item);
		nodeItem -= menuProcScrollGet(layer);;

		if (nodeItem > line)
			nodeItem = line;
		item = menuProcScrollGet(layer);

		for(i=0; i<nodeItem; i++,item++)
		{
			menuStrGet(layer,item,buf,BUFSIZE);
			icon = menuProcIconIDGet(layer, item);

			if (!menuProcIsActive(layer, item))
			{
				appOsdColorSet(MENU_STR_DISABLE);
				menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
			}
			else if (menuProcCurrItemGet(layer) == item)
			{
				menuBarDisp(i + (line - nodeItem), layer);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDisp(2, i + (line - nodeItem), buf, 1, layer);
			}
			else
			{
				appOsdColorSet(MENU_STR_LOST_FOCUS);
				menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
			}
		}

		item = menuProcCurrItemGet(layer-1);
		menuStrGet(layer-1,item,buf,BUFSIZE);

		menuBarDisp(line-nodeItem-1, layer);
		appOsdColorSet(MENU_STR_GET_FOCUS);
		menuStrDispExt((LCD_WIDTH-strWidthGet(buf))/2, (line-nodeItem)*MENU_CHAR_HEIGHT, buf, 1);
		#if FUNC_HOST_TOUCH_PANEL
		menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_OFF);
		#else
		menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);
		#endif
	}
	#endif
	
	sp1kOsdDispBuffShow();
}

void menuEVDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col;
	UINT8	nodeItem;
	UINT16	buffW,buffH;
	UINT8 count=0;
	UINT16 xpos,ypos,width,heigth;
	UINT8 depth,color;
	//UINT16 fontW=strWidthGet(exposure)/(sizeof(exposure)-1);
	UINT8 buf[BUFSIZE];

	menuStrGet(layer,item,buf,BUFSIZE);
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	menuLayerSizeGet(layer, &line, &col);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);
	appOsdColorSet(MENU_STR_LOST_FOCUS);
	nodeItem = menuProcNodeItemGet(layer, item);
	menuStrDisp((col-6)/2, line-2, "EV", 0, layer);
	menuStrDisp((col-6)/2+3, line-2, buf, 0, layer);
	//menuStrDisp((col-6)/2-2, line-1, exposure, 0, layer);
	xpos=MENU_CHAR_WIDTH*(col/4);
	ypos =(MENU_CHAR_HEIGHT<=16)?(line*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET): (LCD_HEIGHT-2*MENU_CHAR_HEIGHT);
	width=MENU_CHAR_WIDTH/2;
	heigth=MENU_CHAR_HEIGHT/4;
	depth=MENU_CHAR_WIDTH/4;
	
	for(count=0;count<13;count++)
	{	xpos+=MENU_CHAR_WIDTH;
		if(item == count)
		{
			color=0x04;
		}
		else
		{
			color=0x3e;
		}
		if(count%3 ==0)
		{
			sp1kOsdSelWinSet(xpos,ypos-heigth,width,heigth*3,depth,color);

		}
		else
		{
			sp1kOsdSelWinSet(xpos,ypos,width,heigth*2,depth,color);
		}

	}
	
	//appOsdColorSet(MENU_TAB_GET_FOCUS);
	//osdCharDrawExt((((UINT16)col-6)/2 + menuXOff)*MENU_CHAR_WIDTH+item*fontW +8 , ((UINT16)line+menuYOff)*32 -10, exposure[item],1);
	#if FUNC_HOST_TOUCH_PANEL
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	#else
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);
	#endif
	sp1kOsdDispBuffShow();
}

#if FUNC_HOST_TOUCH_PANEL
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, xpos=0, ypos=1;
 	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];
	UINT16 iconXpos;

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	
	menuGuiClr(0,MENU_OBJ_MAX);
	
	menuLayerSizeGet(layer, &line, &col);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

 	nodeItem = menuProcNodeItemGet(layer, item);
	
	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuChildStrGet(layer, menuDateItemConv(item), selItem, buf, BUFSIZE);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item))
		{
			if(menuProcCurrItemGet(layer)==5)
			{
				iconXpos = (xpos+2)*MENU_CHAR_WIDTH+strWidthGet(buf)/2-14;
			}
			else
			{
				iconXpos = (xpos+2)*MENU_CHAR_WIDTH;
			}
			appOsdColorSet(OSD_COLOR_YELLOW);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			menuProcButIconDisp(MENU_PROMPT_GET_FOCUS,iconXpos,(ypos-1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				strWidthGet(buf),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ICON_UP,ID_ICON_TBUTTON_UP,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_UP,0);
			menuProcButIconDisp(MENU_PROMPT_GET_FOCUS,iconXpos,(ypos+1)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				strWidthGet(buf),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ICON_DOWN,ID_ICON_TBUTTON_DOWN,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_DOWN,0);		
			menuProcButIconDisp(0xff,(xpos+2)*MENU_CHAR_WIDTH,(ypos-0)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				strWidthGet(buf),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);

			xpos += strlen(buf);

		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			menuProcButIconDisp(0xff,(xpos+2)*MENU_CHAR_WIDTH,(ypos-0)*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET,\
				strWidthGet(buf),MENU_CHAR_HEIGHT,\
				MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
				);

			xpos += strlen(buf);
		}
		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, "/",0);
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, ":",0);
			xpos += strlen(":");
					
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 3;
			
			ypos += 2;
			
		}
		else
			xpos++;
	}
	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);
	
	sp1kOsdDispBuffShow();
}

void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	

	menuHeaderDisp(layer);
	menuBodyDisp(layer);
	
	if(layer == 2)
	{
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,0);
	}
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);


	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDisp(0, i, buf, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}	


	}

	
	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);
	menuGuiClr(0,MENU_OBJ_MAX);

 	menuHeaderDisp(layer);
	menuBodyDisp(layer);
	menuGuiClr(MENU_OBJ_ICON_UP,MENU_OBJ_ICON_DOWN+1);

	if(layer == 2)
	{
		appGuiObjClr(MENU_OBJ_ICON_UP);
		osdIconDrawExt(MENU_BODY_LAYER2_X_OFFSET, MENU_BODY_LAYER2_Y_OFFSET, ID_ICON_MUSUBSC,0);
	}

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;


	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(54-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			//menuBarDisp(i, layer);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			menuStrDisp(0, i, buf, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				menuIconDispExt((col-2)*MENU_CHAR_WIDTH, (i+2)*MENU_CHAR_HEIGHT-2, ID_ICON_MU2BARSL, 1);
				
				menuProcButIconDisp(MENU_BODY_DISABLE,(col-2-2)*MENU_CHAR_WIDTH,(i+2)*MENU_CHAR_HEIGHT-2,\
					MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
					MENU_OBJ_ICON_LEFT,ID_ICON_TBUTTON_LEFT,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_LEFT,0);
				menuProcButIconDisp(MENU_BODY_DISABLE,(col-2)*MENU_CHAR_WIDTH+sp1kOsdIconWidthGet(ID_ICON_MU2BARSL),(i+2)*MENU_CHAR_HEIGHT-2,\
					MENU_CHAR_WIDTH*2,MENU_CHAR_HEIGHT,\
					MENU_OBJ_ICON_RIGHT,ID_ICON_TBUTTON_RIGHT,MENU_FUNC_NULL,0x00,SP1K_MSG_KEY_PRESS_RIGHT,0);

				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(0, i, buf, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		menuProcButIconDisp(0xff,MENU_LAYER2_STR_X_OFFSET,i * MENU_CHAR_HEIGHT + MENU_LAYER2_STR_Y_OFFSET,\
			(sp1kOsdIconWidthGet(ID_ICON_MUSBAR)/2),sp1kOsdIconHeightGet(ID_ICON_MUSBAR),\
			 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
			);

	}

	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);

	sp1kOsdDispBuffShow();
}

void menuProcDispYesNo(UINT8 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	menuGuiClr(MENU_OBJ_ITEM0, MENU_OBJ_MAX);
	menuLayerSizeGet(layer, &line, &col);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);;

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDisp(2, i + (line - nodeItem), buf, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
		}
		menuProcButIconDisp(0xff,MENU_BAR_LAYER3_X_OFFSET,(i + (line - nodeItem)) * MENU_CHAR_HEIGHT+MENU_BAR_LAYER3_Y_OFFSET,\
			(sp1kOsdIconWidthGet(ID_ICON_MUDLGBAR)),sp1kOsdIconHeightGet(ID_ICON_MUDLGBAR),\
			 MENU_OBJ_ITEM0+i,ID_ICON_NONE,MENU_FUNC_SELITEM,i,SP1K_MSG_NULL,0\
			);

	}

	strGet(buf,BUFSIZE,idx);
	appOsdColorSet(MENU_STR_GET_FOCUS);
	menuStrDispExt((LCD_WIDTH-strWidthGet(buf))/2, (line-nodeItem)*(MENU_CHAR_HEIGHT+1), buf, 1);

	menuPromptDisp(MENU_PROMPT_OK_OFF| MENU_PROMPT_MENU_ON);

	sp1kOsdDispBuffShow();
}

void menuProcDispPalette(UINT16 msg)
{
	UINT16 x, y;
	UINT8 i=0;
	UINT16 charW,charH;
	UINT8 str[32]={0};
	static UINT8 cnt=0;
	
	sp1kOsdFontSizeGet(&charW, &charH);
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	for (y=0; y<8; y++)
	{
		for (x=0; x<8; x++)
		{
			osdMapTabSet(0xc0 | i,0xc0 | i,0xc0 | i,0xc0 | i);
			menuStrDispExt(x * (charH-1), y * (charH-1), "       ", 0);
			i++;
		}
	}
	if(msg==SP1K_MSG_KEY_PRESS_UP){
		if(cnt >= 8)cnt -= 8;
	}else if(msg==SP1K_MSG_KEY_PRESS_DOWN){
		if(cnt < 7*8)cnt += 8;
	}else if (msg==SP1K_MSG_KEY_PRESS_LEFT){
		if(cnt)cnt--;
	}else if(msg==SP1K_MSG_KEY_PRESS_RIGHT){
		cnt++;
	}
	cnt%=64;	
	//sprintf(str,"IDX:%bu",cnt);
	appOsdColorSet(MENU_STR_LOST_FOCUS);
	menuStrDispExt(charW*16,charH*2,str,0);
	sp1kOsdSelWinSet((cnt%8)*(charH-1),  cnt/8*(charH-1) ,(charH-1),(charH-1),2,0);
	sp1kOsdDispBuffShow();
}



#else
void menuRTCDisp(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, xpos=0, ypos=1;
 	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	menuLayerSizeGet(layer, &line, &col);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

 	nodeItem = menuProcNodeItemGet(layer, item);

	for(i=0, item=0; i<nodeItem; i++,item++)
	{
		selItem = menuProcParaGet (layer, menuDateItemConv(item));
		menuChildStrGet(layer, menuDateItemConv(item), selItem, buf, BUFSIZE);
		if (menuProcCurrItemGet(layer) == menuDateItemConv(item))
		{
			appOsdColorSet(OSD_COLOR_YELLOW);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			xpos += strlen(buf);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, buf,0);
			xpos += strlen(buf);
		}
		if ((item == 0) || (item == 1))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, "/",0);
			xpos += strlen("/");
		}
		else if (item == 3)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			sp1kOsdStrDrawExt((xpos+2)*MENU_CHAR_WIDTH, ypos*MENU_CHAR_HEIGHT+MENU_LAYER2_STR_Y_OFFSET, ":",0);
			xpos += strlen(":");
		}
		else if (item == 4)
		{
			if(MENU_CHAR_WIDTH<=8)
				xpos = 2;
			else
				xpos = 3;
			
			ypos += 2;
		}
		else
			xpos++;
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);

	sp1kOsdDispBuffShow();
}
void menuProcDisp2Layer(void)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem, selItem, subItem;
	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

	layer--;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

	menuHeaderDisp(layer);
	menuBodyDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDisp(0, i, buf, 1, layer);
			menuIconDisp(col-2, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			menuIconDisp(col-2, i, icon, 0, layer);
		}
	}

	layer++;
	item = 0;
	menuLayerSizeGet(layer, &line, &col);

 	menuHeaderDisp(layer);
	menuBodyDisp(layer);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);

	if (nodeItem > line)
		nodeItem = line;



	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(0, i, buf, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(54-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			//menuBarDisp(i, layer);
			appOsdColorSet(MENU_TAB_GET_FOCUS);
			menuStrDisp(0, i, buf, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
				menuIconDispExt((col-2)*MENU_CHAR_WIDTH, (i+2)*MENU_CHAR_HEIGHT-2, ID_ICON_MU2BARSL, 1);

				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				appOsdColorSet(MENU_STR_GET_FOCUS);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(0, i, buf, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuChildStrGet(layer, item, selItem, buf, BUFSIZE);
				menuStrDispExt((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-strWidthGet(buf))/2, (i+2)*MENU_CHAR_HEIGHT-2, buf, 1);
			}
		}
	}

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_ON);

	sp1kOsdDispBuffShow();
}

void menuProcDispYesNo(UINT8 idx)
{
	UINT8 layer = menuProcLayerGet();
	UINT8 item = menuProcCurrItemGet(layer);
	UINT8 icon = menuProcIconIDGet(layer, item);
	UINT8 line, col, i;
	UINT8 nodeItem;
	UINT16	buffW,buffH;
	UINT8 buf[BUFSIZE];

	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	menuLayerSizeGet(layer, &line, &col);
	sp1kOsdDispBuffSizeGet(&buffW, &buffH);

	menuBodyDisp(3);

	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);;

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);

	for(i=0; i<nodeItem; i++,item++)
	{
		menuStrGet(layer,item,buf,BUFSIZE);
		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
		}
		else if (menuProcCurrItemGet(layer) == item)
		{
			menuBarDisp(i + (line - nodeItem), layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDisp(2, i + (line - nodeItem), buf, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(2, i + (line - nodeItem), buf, 0, layer);
		}
	}

	strGet(buf,BUFSIZE,idx);
	appOsdColorSet(MENU_STR_GET_FOCUS);
	menuStrDispExt((LCD_WIDTH-strWidthGet(buf))/2,  (line-nodeItem)*MENU_CHAR_HEIGHT, buf, 1);

	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);

	sp1kOsdDispBuffShow();
}

void menuProcDispPalette(UINT16 msg)
{
	UINT16 x, y;
	UINT8 i=0;
	UINT16 charW,charH;
	UINT8 str[32]={0};
	static UINT8 cnt=0;
	
	sp1kOsdFontSizeGet(&charW, &charH);
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);

	for (y=0; y<8; y++)
	{
		for (x=0; x<8; x++)
		{
			osdMapTabSet(0xc0 | i,0xc0 | i,0xc0 | i,0xc0 | i);
			menuStrDispExt(x * (charH-1), y * (charH-1), "       ", 0);
			i++;
		}
	}
	if(msg==SP1K_MSG_KEY_PRESS_UP){
		if(cnt >= 8)cnt -= 8;
	}else if(msg==SP1K_MSG_KEY_PRESS_DOWN){
		if(cnt < 7*8)cnt += 8;
	}else if (msg==SP1K_MSG_KEY_PRESS_LEFT){
		if(cnt)cnt--;
	}else if(msg==SP1K_MSG_KEY_PRESS_RIGHT){
		cnt++;
	}
	cnt%=64;	
	//sprintf(str,"IDX:%bu",cnt);
	appOsdColorSet(MENU_STR_LOST_FOCUS);
	menuStrDispExt(charW*16,charH*2,str,0);
	sp1kOsdSelWinSet((cnt%8)*(charH-1),  cnt/8*(charH-1) ,(charH-1),(charH-1),2,0);
	sp1kOsdDispBuffShow();
}


#endif

// 绘制删除对话框
void app_quick_delete_dialog(UINT8 cur_idx) {
	UINT8 buf[BUFSIZE];

	UINT16 dialog_x = 0, dialog_y = 0;
	UINT16 select_bar_x = 0, select_bar_y = 0;
	UINT16 title_x = 0, title_y = 0;
	
	
	// 对话框
	dialog_x = MENU_BODY_LAYER3_X_OFFSET;
	dialog_y = MENU_BODY_LAYER3_Y_OFFSET;
	
	// appOsdColorSet(MENU_DLG_GET_FOCUS);
	appOsdColorSet(OSD_COLOR_WHITE);
	osdIconDrawExt(dialog_x, dialog_y, ID_ICON_MUDLG,0);

	

	// 选择条
	select_bar_x = dialog_x + 20;
	select_bar_y = dialog_y + ((cur_idx + 1) * 30) + 10;
	select_bar_y = cur_idx ? select_bar_y + 4 : select_bar_y;
	
	appOsdColorSet(MENU_BAR_GET_FOCUS);
	osdIconDrawExt(select_bar_x, select_bar_y, ID_ICON_MUDLGBAR, 1);

	

	// 按键提示符
	menuPromptDisp(MENU_PROMPT_OK_ON | MENU_PROMPT_MENU_OFF);

	

	// 对话框标题
	if(enterMacroadjust == 1){
		strGet(buf,BUFSIZE,ID_STR_ADJUST_MACRO_);
		//enterMacroadjust = 0;
	}else{
		strGet(buf,BUFSIZE,ID_STR_DELETE_THIS_FILE_);
	}
	title_x = (LCD_WIDTH-strWidthGet(buf))/2;
	title_y = dialog_y + 4;
	
	appOsdColorSet(MENU_STR_GET_FOCUS);
	menuStrDispExt(title_x,  title_y, buf, 1);

	

	// 文字:确认和取消
	strGet(buf,BUFSIZE,ID_STR_ENTER);	
	appOsdColorSet(OSD_COLOR_WHITE);
	menuStrDispExt(select_bar_x + 85,  dialog_y + 30 + 12, buf, 1);

	strGet(buf,BUFSIZE,ID_STR_BACK);
	menuStrDispExt(select_bar_x + 85,  dialog_y + 2 * 30 + 16, buf, 1);
}

