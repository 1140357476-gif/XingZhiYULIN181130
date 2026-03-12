
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
#ifndef __APP_UI_OSD_H__
#define __APP_UI_OSD_H__
#include "func_gui_def.h"
#include "app_gui_obj.h"
#include "host_func_opt.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define OSD_ATTR_COLOR_BLUE   0x03

#define OSD_ATTR_COLOR_WHITE	0x3f
#define OSD_ATTR_COLOR_BLACK	0x00
#define OSD_ATTR_COLOR_RED		0x30
#define OSD_ATTR_COLOR_YELLOW   0x3c
#define OSD_ATTR_COLOR_GREEN	0x04
#define OSD_ATTR_COLOR_CYAN		0x0f
#define OSD_ATTR_COLOR_GREY     0x2a
#define OSD_ATTR_COLOR_LINE		0x15
#define OSD_ATTR_COLOR_PINK		0x22
#define OSD_ATTR_COLOR_LIGHT_GREEN		0x0c



#define OSD_ATTR_MODE_TRANS		0x00
#define OSD_ATTR_MODE_HALFT1	0x80
#define OSD_ATTR_MODE_HALFT2	0x80
#define OSD_ATTR_MODE_BLD		0xc0

	
#define OSD_ATTR_BACK_DIS       0x00
#define OSD_ATTR_BACK_EN        0x08
	
#define OSD_ATTR_MODE_NOMAL     0x00
#define OSD_ATTR_MODE_HALFTONE  0x10
#define OSD_ATTR_MODE_EXCHANGE  0x20
#define OSD_ATTR_MODE_BLINKING  0x30
#define OSD_ATTR_MODE_BLIND		0x80

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define FUNC_CLEAR

//Enable LCD and OSD mode switch
#define _ENABLE_LCD_OFF_


//For HD solution black cover
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
enum
{
	SEL_FRAME_MOVE_UP = 0 << 5,
	SEL_FRAME_MOVE_DOWN = 1 << 5,
	SEL_FRAME_MOVE_LEFT = 2 << 5,
	SEL_FRAME_MOVE_RIGHT = 3 << 5,
	SEL_FRAME_ZOOM_IN = 4 << 5,
	SEL_FRAME_ZOOM_OUT = 5<<5,
	SEL_FRAME_STEP = 4,
	SEL_FRAME_MAX,
};

typedef struct SelFrameInfo_s{
	UINT16 xpos;
	UINT16 ypos;
	UINT16 width;
	UINT16 heigth;
} SelFrameInfo;

enum
{
	UI_LCD_ON_WITH_OSD,
	UI_LCD_ON_WO_OSD,
	UI_LCD_OFF,
	UI_OSD_MODE_MAX,
};

  typedef enum _IconMode
{
	ICON_MODE_PV,
	ICON_MODE_VIDEO,
	ICON_MODE_AUDIO,
	ICON_MODE_PB,
	ICON_MODE_DPS,
	ICON_MODE_MAX
}IconMode;

enum
{
	ICON_CLEAR,
	ICON_SHOW,
};
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void uiIconDisp(UINT8 x, UINT8 y, UINT8 icon, UINT8 attr);
void uiUpdateOSDTime(UINT16 x, UINT16 y, UINT32 time, UINT8 attr);
void uiUpdateOSDSize(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateImgQuality(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateImgSize(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateSnapMode(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateWB(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateAEMetering(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateOSDFlashMode(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateVideoSize(UINT8 x, UINT8 y, UINT8 attr);
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
void uiUpdateOsdDps(void);
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
//void uiUpdateOSDZoom(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateOSDPbType(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateEffect(UINT8 x, UINT8 y, UINT8 attr);
void uiSceneModeUpdate(void);

void appStillOsdInit(UINT8 showNum);
void appVideoOsdInit(void);
void appAudioOsdInit(void);
void appPbOsdInit(void);
void appPbVideoOsdInit(void);
void uiUpdateVideoStatus(UINT8 mode	,UINT8 status);
void uiUpdateAudioStatus(UINT8 mode	,UINT8 status);
//void uiUpdateOSDPbNoImage(void);
void uiUpdateOSDMemFull(void);
UINT8 uiUpdateBatteryLevel(void);
void uiUpdateOSDBatEmpty(void);
//void uiUpdateOSDFileLock(void);
void uiUpdateOSDPbFileNum(void);
void uiUpdateEV(UINT8 x, UINT8 y, UINT8 attr);
void uiUpdateAudRecTime(UINT32 time);
void uiUpdateAudPlayTime(UINT32 time);
void uiUpdateVideoRecTime(UINT32 time);
void uiUpdateOSDPvZoom(UINT16 zoomfact, UINT8 mode);
void uiUpdateOSDPvZoom_BAR(UINT16 zoomfact);

void uiUpdateAudRecRemainTime(UINT32* time);
void uiUpdateVideoRecRemainTime(UINT32* time);
void appDirectionIconShow(UINT8 edgeSts);
void uiIsoUpdate(void);

void uiRemainPixInit(void);
void uiRemainPixCount(UINT8 show);
void uiRemainPixShow(void);
void uiUpdateRemainPix(void);
void uiOsdDialogDisp(UINT8 stridx, UINT16 wait);
void uiOsdVerDisp(UINT8 stridx, UINT16 wait);
void appUIUSBModeSet(void);
void appVolumeSet(void);
void appEVSet(UINT8 expval);
void appDateStampSet(void);
void appFramRateSet(void);
void appUiBusyWait(UINT16 Time, UINT8 bFlash);
void appUiBusyExit(void);
void appUiBusyShow(UINT8 rst);
void appUiCoverTVOsd(void);
void uiCardIcon(void);
SelFrameInfo uiUpdateSelFrame(UINT8 value);
void uiCycleIcon(void);

void uiUpdateSlideShowState(UINT8 mode);
void appOsdModeSw(UINT8 ctrl);
void appOsdSelfSnapEnableOsd(UINT8 mode);
UINT8 appGetOsdMode(void);
void appIconPsTabLoad(UINT8 mode );
void  appCurrentTimeShow(UINT8 time);
void appGuiObjIconClr(UINT8 sIdx,UINT8 eIdx);
void appTPIconReg(UINT8 objId,UINT16 iconIdx,UINT8 funcIdx,funcGuiHdlGrp_t mode,UINT8 cnt,UINT16 msg,UINT8 penRepeateEn);
void appTPRangeReg(UINT8 objId,UINT16 x,UINT16 y,UINT16 width,UINT16 height,UINT8 funcIdx,funcGuiHdlGrp_t mode,UINT8 cnt,UINT16 msg,UINT8 penRepeateEn);
void appMultiOsdDirectionShow(void);
void appMultiGuiInit(void);
void appOsdDigitalZoomWinDraw(UINT16 ZoomFact);
void appCrossLineDraw(UINT16 x,UINT16 y,UINT8 color,UINT16 lineW);
void uiUpdateFlashLed(UINT8 x, UINT8 y, UINT8 attr);

#if SUPPORT_USB_ICON
void appChargingIconFlicker();
#endif

#if SUPPORT_AUTO_TEST
void appButtonAutoTest(void);
#endif


#if SUPPORT_AF

#if SUPPORT_FIXED_AF
void AF_uiAfIconShow(void);
#endif

void AF_AfCalStatusShow(UINT8 status, UINT8 flag);
#endif




#endif

