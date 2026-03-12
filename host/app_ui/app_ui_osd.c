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
/**
 * @file		app_ui_osd.c
 * @brief		UI OSD
 * @author		jintao.liu
 * @since		2008-01-25
 * @date		2008-02-20
 */
#include "general.h"
#include "initio.h"
#include "app_main.h"
#include "os_msg.h"
#include "osd_mem.h"
#include "osd_api.h"
#include "app_msg.h"
#include "App_ui_para.h"
#include "App_ui_osd.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "App_osd_icon.h"
#include "App_menu_micro.h"
#include "App_menu_api.h"
#include "App_menu_core.h"
#include "App_menu_tab.h"
#include "App_still_play.h"
#include "App_audio_rec.h"
#include "app_battery.h"
#include "app_menu.h"
#include "app_def.h"
#include "doslink.h"
#include "video_clip.h"
#include "video_pb.h"
#include "uiflow.h"
#include "sp1k_aud_api.h"
#include "app_still_view.h"
#include "app_video_view.h"
#include "sp1k_disp_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_dps_api.h"
#include "sp1k_tmr_svc.h"
#include "key_scan.h"
#include "func_ptr.h"
//#include "dos32.h"//eric@20080502
#include "sp1k_dos_api.h"
#include "solution.h"//eric@20080502
#include "smcfunc.h"
#include "vfs_api.h"
#include "res_macro.h"
#include "sp1k_video_api.h"
#include "dbg_def.h"
#if (FUNC_HOST_DPS == 1)
#include "app_dps.h"
#endif
#include "dbg_Mem.h"
#include "app_init.h"
#include "reg_dram.h"
#include "sp1k_stamp_api.h"
#include "sp1k_gpio_api.h"


#define FUNC_CLEAR

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
UINT32 AudRecRemainTime;//the free space for recording audio
UINT32 RemainPictureNO = 0;
static UINT8 xdata PrevIE;
extern UINT16 LCD_WIDTH;
extern UINT16 LCD_HEIGHT;
extern UINT16 MENU_CHAR_WIDTH;
extern UINT16 MENU_CHAR_HEIGHT;
extern UINT8 stampTimeFmt;
extern UINT16 stillZFctor;
extern UINT8 versionclearflag;
extern UINT8 afbackfaruiclearflag;


#ifdef _ENABLE_LCD_OFF_
//The current OSD mode
//0   LCD on with OSD
//1   LCD on w/o OSD
//2   LCD off
static xdata UINT8 osdMode;
#endif /*_ENABLE_LCD_OFF_*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern SIMAGE xdata pbFile;
//extern xdata UINT8 OsdMaxX;
//extern xdata UINT8 OsdMaxY;
//extern xdata UINT8 OsdMaxNum;
//extern xdata UINT32 G_DOS_ClusterSize;
extern appAudRecCtrl_t xdata audRecCtrl;
extern UINT16 pbZoomCurLog;
extern UINT8 G_Card_Type;
extern UINT8 PrintStatus;
extern UINT8 isUpdateNum;
#if (STILL_NEW_ZOOM == 0)
extern appPvZoom_t xdata PvZoomCtrl ;
#endif

#if (VIDEO_NEW_ZOOM == 0)
extern UINT16 xdata video_zoomFactor ;
#endif

#if SUPPORT_AUTO_TEST
extern UINT8 autoTestOpenFlag;
// 0 : Key And Usb Test
// 1 : Video Record Test
// 2 : Video Playback Test
extern UINT8 autoTestStatus; 
extern UINT8 upKeyCheck, downKeyCheck, okKeyCheck, pwrKeyCheck, UsbCheck, pbKeyCheck, setKeyCheck;
#endif

extern UINT8 showChargingIcon;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
**************************************************************************/
/**
 * @fn void sp1kOsdNumberShow(UINT16 num, UINT8 x, UINT8 y, UINT8 fillbit ,UINT8 attrib)
 * @brief	Specifes OSD Horizonal offset
 * @param[in] num
 * @param[in] x       X position of OSD drawing area left side.
 * @param[in] y       y position of OSD drawing area left side.
 * @param[in] fillbit
 * @param[in] attr    Please refer to attr description in sp1kOsdStrDraw() for more attribute details.
 * @retval none
 */
void sp1kOsdNumberShow(UINT16 num, UINT8 x, UINT8 y, UINT8 fillbit ,UINT8 attrib) USING_0
{
     UINT8 i,j;
     UINT8 width;
     UINT16 tmp1;
     UINT8 offset;
     UINT8 val;
     offset = 0;

     if(fillbit)
     {
          width = fillbit;
     }
     else
     {
          width = 4;
     }

     if((num == 0) && (!fillbit))
     {
		osdCharDraw(x, y, 0x30, attrib);
		return;
     }

     for(i=width;i>0;i--)
     {

          tmp1 = 1;

          for(j=0;j<i-1;j++)
          {
               tmp1 = tmp1*10;
          }

          if( (num/tmp1) > 0 )
          {
				val = (UINT8)( (num / tmp1) % 10 + 0x30);
				osdCharDraw(x+offset, y, val, attrib);
				offset++;
          }
          else
          {
               if(fillbit)
               {
					osdCharDraw(x+offset, y, 0x30, attrib);
                    offset++;
               }
          }
     }
}

/********************Function  appVolumeSet **************/
//Name:				void  appVolumeSet(void)
//Description:			Update Volume
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-31 14:48
//Last Modify Time:	08-03-31 18:00
/*****************************************************/
void appEVSet(UINT8 expval)
{
	//uiPara_t* puienv;
	//puienv=appUiParaGet();
	//switch(puienv->AEMode)
	switch(expval)
	{
		case PRV_COMP_N20EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M20);
			break;
		case PRV_COMP_N17EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M17);
			break;
		case PRV_COMP_N13EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M15);
			break;
		case PRV_COMP_N10EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M13);
			break;
		case PRV_COMP_N07EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M10);
			break;
		#if SUPPORT_VIDEO_EV
		case PRV_COMP_N03EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M07);
			break;
		#else
		case PRV_COMP_N03EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_M05);
			break;
		#endif
		case PRV_COMP_0EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
			break;
		case PRV_COMP_P03EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P03);
			break;
		case PRV_COMP_P07EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P07);
			break;
		case PRV_COMP_P10EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P10);
			break;
		case PRV_COMP_P13EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P13);
			break;
		case PRV_COMP_P17EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P17);
			break;
		case PRV_COMP_P20EV:
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_P20);//SP1K_AE_EXP_COMP_P20
			break;
	}
}

/********************Function  appVolumeSet **************/
//Name:				void  appVolumeSet(void)
//Description:			Update Volume
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-31 14:48
//Last Modify Time:	08-03-31 18:00
/*****************************************************/
void appVolumeSet(void)
{
	uiPara_t* puienv;

	puienv = appUiParaGet();
	if(appPanelTypeGet() ==1) {
		sp1kAudioVolumeSet(puienv->Volume);
	}
	else {
		sp1kAudioVolumeSet(puienv->Volume +7);
	}
}

void appDateStampSet(void)
{
	uiPara_t* puienv;
	UINT8 onoff;

	puienv = appUiParaGet();

	sp1kStampTextReset();

	switch(puienv->StampMode)
	{
		case PRV_STAMP_OFF:
			onoff=0;
			sp1kStampTextEnable(STAMP_DATE,0);
			sp1kStampTextEnable(STAMP_TIME,0);
			break;
		case PRV_STAMP_DATE:
			onoff=1;
			sp1kStampTextEnable(STAMP_DATE,1);
			sp1kStampTextEnable(STAMP_TIME,0);
			break;
		case PRV_STAMP_DATETIME:
			onoff=1;
			sp1kStampTextEnable(STAMP_DATE,1);
			sp1kStampTextEnable(STAMP_TIME,1);
			break;
		default:
			onoff=0;
			sp1kStampTextEnable(STAMP_DATE,0);
			sp1kStampTextEnable(STAMP_TIME,0);
			break;
	}
	
	switch(puienv->DateStyle)
	{
		case DATESTYLE_MMDDYYYY:
			sp1kStampDateTimeFmtSet(2,2);
			break;
		case DATESTYLE_DDMMYYYY:
			sp1kStampDateTimeFmtSet(1,2);
			break;
		case DATESTYLE_YYYYMMDD:
			sp1kStampDateTimeFmtSet(0,2);
			break;
		default:
			break;
	}

	/* set stamp function enable/disable */
	sp1kSnapParamSet(SP1K_SNAP_PARAM_STAMP_OPT, onoff);
	
	/* set background function disable */
	sp1kStampBackgroundEnable(0);
}

void uiUpdateFlashLed(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	switch(puiPara->FlashLed)
	{
		case 0:
			sp1kOsdIconShow(ID_ICON_FLASH_OFF,ICON_SHOW);
			break;
		case 1:
			sp1kOsdIconShow(ID_ICON_FLASH_ON,ICON_SHOW);
			break;
		case 2:
			sp1kOsdIconShow(ID_ICON_FLASH_AUTO,ICON_SHOW);
			break;
		case 3:
			sp1kOsdIconShow(ID_ICON_FLASH_AUTO_RED_EYE,ICON_SHOW);
			break;
		
			
	}
}


void appFramRateSet(void)
{
	uiPara_t* puienv;

	puienv=appUiParaGet();
	switch(puienv->FrameRate)
	{
		case MOV_FRM_HIGH:
			if(puienv->LightFreq==PRV_FREQ_50)
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
			else
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
			break;
		case MOV_FRM_LOW:
			if(puienv->LightFreq==PRV_FREQ_50)
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_12P5);
			else
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_15);
			break;
		default:
			if(puienv->LightFreq==PRV_FREQ_50)
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_25);
			else
				sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
			break;
	}
}

/********************Function  appUIUSBModeSet **************/
//Name:				void  appUIUSBModeSet(void)
//Description:			Update USB Mode
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-31 14:48
//Last Modify Time:	08-03-31 18:00
/*****************************************************/
void appUIUSBModeSet(void)
{
	uiPara_t* puienv;

	puienv=appUiParaGet();
	switch(puienv->USBMode)
	{
		case USB_MASS:
			gUsbMode = K_UIOPTION_STORAGE_MSDC;
			UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
			//printf(">>>usb mass \n");
			break;
		case USB_SIDC:
			gUsbMode = K_UIOPTION_STORAGE_SIDC;
			UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
			sp1k_dps_sidcInit();
			sp1kSidcInit();
			//printf(">>>usb sidc \n");
			break;
		case USB_PCCAM:
		default:
			gUsbMode = K_UIOPTION_STORAGE_NONE;
			UI_UsbModeSet(K_UISTATUS_USBMODE_PCCAM);
			//printf(">>>usb pccam \n");
			break;
	}
}

/********************Function uiRemainPixInit **************/
//Name:				void uiRemainPixInit(void)
//Description:			Update the number of remain pix
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-11 14:48
//Last Modify Time:	08-03-11 18:00
/*****************************************************/
void uiRemainPixInit(void)
{
	UINT32 freespace;
	UINT16 pixx,pixy;
	UINT32 filesize;
	uiPara_t *puiPara;
	UINT8 saturation;

	puiPara=appUiParaGet();
#if 1//def DOS32_SUPPORT
	freespace=(UINT32)((UINT32)sp1kDosGetDiskSpace()/**(UINT32)1024*/);//eric@20080502
#else
	freespace=(UINT32)(DOS_FreeDiskSpaceByCluster() *(UINT32)G_DOS_ClusterSize / 1024);//jintao.liu 2008-6-16
#endif

#if (!NAND_SUPPORT)
	//Phil add for disable nand flash
	if ( G_Card_Type == K_MEDIA_NANDF) {
		freespace = 0;
	}
#endif


		if(freespace<=1024L/**1024L*/)
			freespace=0;
		else
			freespace-=1024L/**1024L*/;
	
	sp1kSnapSizeGet(&pixx, &pixy);
	//rawsize=(UINT32)((UINT32)pixx*(UINT32)pixy*2UL);
	switch(puiPara->ImageSize)
	{	
		case PRV_SIZE_50M:
			saturation=0x00;
			break;
		case PRV_SIZE_44M:
			saturation=0x00;
			break;
		case PRV_SIZE_42M:
			saturation=0x01;
			break;
		case PRV_SIZE_40M:
			saturation=0x02;
			break;
		case PRV_SIZE_36M:
			saturation=0x10;
			break;
		case PRV_SIZE_30M:
			saturation=0x11;
			break;
		case PRV_SIZE_24M:
			saturation=0x12;
			break;
		case PRV_SIZE_20M:
			saturation=0x20;
			break;
		case PRV_SIZE_18M:
			saturation=0x21;
			break;
		case PRV_SIZE_16M:
			saturation=0x22;
			break;
		case PRV_SIZE_12M:
			saturation=0x30;
			break;
		case PRV_SIZE_10M:
			saturation=0x31;
			break;
		case PRV_SIZE_9M:
			saturation=0x32;
			break;
		case PRV_SIZE_8M:
			saturation=0x40;
			break;
		case PRV_SIZE_7M_16_9:
			saturation=0x60;
			break;
		case PRV_SIZE_5M:
			saturation=0x41;
			break;
		case PRV_SIZE_3M:
			saturation=0x42;
			break;
		case PRV_SIZE_1M:
		case PRV_SIZE_1M_16_9:
			saturation=0x20;
			break;
		case PRV_SIZE_VGA:
			saturation=0x30;
			break ;
		case PRV_SIZE_2M:
		case PRV_SIZE_2M_16_9:
			saturation=0x40;
			break;

		default:
			saturation=0x00;
			break;
	}

	saturation|=0x00;

	switch(saturation)
	{
		case 0x00:
			filesize=2059424L;
			break;
		case 0x01:
			filesize=2039424L;
			break;
		case 0x02:
			filesize=2019424L;
			break;
		case 0x10:
			filesize=1979424L;
			break;
		case 0x11:
			filesize=1919424L;
			break;
		case 0x12:
			filesize=1859424L;
			break;
		case 0x20:
			filesize=1819424L;
			break;
		case 0x21:
			filesize=1799424L;
			break;
		case 0x22:
			filesize=1779424L;
			break;
		case 0x30:
			filesize=1739424L;
			break;
		case 0x31:
			filesize=1719424L;
			break;
		case 0x32:
			filesize=1709424L;
			break;
		case 0x40:
			filesize=1699424L;
			break;
		case 0x41:
			filesize=1669424L;
			break;
		case 0x42:
			filesize=1649424L;
			break;

		case 0x50:
			filesize=3519424L;
			break;
		case 0x51:
			filesize=2239671L;
			break;
		case 0x52:
			filesize=1039808L;
			break;
		case 0x60:
			filesize=4279136L;
			break;
		case 0x61:
			filesize=1839671L;
			break;
		case 0x62:
			filesize=939808L;
			break;	
		default:
			filesize=2519424L;
			break;

	}
	if(filesize!=0)
	{
		filesize /= 1024;//jintao.liu 2008-6-16 filesize kb instead of byte

		RemainPictureNO=(UINT32)(freespace/filesize);
	}
	else
		RemainPictureNO=0;
	if(RemainPictureNO>9999)
		RemainPictureNO=9999;
	//printf(">>>freespace=%lu\n",freespace);
	//printf(">>>rawsize=%lu\n",rawsize);
	//printf(">>>remainpictureNO=%u\n",RemainPictureNO);
}

/********************Function uiUpdateRemainPix **************/
//Name:				void uiUpdateRemainPix(void)
//Description:			Update the number of remain pix
//Input:				None
//Output:			None
//Created By:		Wei.Sun
//Created Time:		08-03-11 14:48
//Last Modify Time:	08-03-11 18:00
/*****************************************************/
void uiUpdateRemainPix(void)
{
	UINT8 str[5]="0000";
	UINT16 tempNo;
	UINT8 counti;
	static UINT8 resolutionstyle;
	static UINT8 qualitystyle;
	uiPara_t* puipara;
	devAttr_t *devState;
	UINT8 Xoffset = 0;
	UINT16 x,y,chrw,chrh;

	sp1kOsdIconPsGet(ID_ICON_MODE_STILL,&x,&y);
	sp1kOsdFontSizeGet(&chrw, &chrh);


	if(appPanelTypeGet() == 0) {
		Xoffset = 0;
	}
	devState=sp1kDevHdlGet();
	puipara=appUiParaGet();
	if(devState[DEV_ID_CARD].devEn==1)
	{
		if(devState[DEV_ID_CARD].devSts!=DEV_STS_RDY)
		{
			sp1kOsdStrDrawExt(LCD_WIDTH - chrw*4-1 +15, y, "        ", 0);	
			return;
		}
	}
	else
	{
		if(devState[DEV_ID_DISK].devSts!=DEV_STS_RDY)
		{
			sp1kOsdStrDrawExt(LCD_WIDTH - chrw*4-1+15, y, "        ", 0);	
			return;
		}
	}
	if(!isUpdateNum)
	{
		uiRemainPixInit();
	}
	isUpdateNum=0;
	qualitystyle=puipara->ImageQuality;
	resolutionstyle=puipara->ImageSize;

	//printf(">>>bRemainPictureNO=%u\n",RemainPictureNO);
	tempNo=RemainPictureNO;
	str[0]=(UINT8)((tempNo/1000)+'0');
	tempNo=tempNo%1000;
	str[1]=(UINT8)((tempNo/100)+'0');
	tempNo=tempNo%100;
	str[2]=(UINT8)((tempNo/10)+'0');
	str[3]=(UINT8)((tempNo%10)+'0');
	//printf(">>>aRemainPictureNO=%u\n",RemainPictureNO);
	for(counti=0;counti<5;counti++)
	{
		if(str[counti]==('0'))
		{
			if(counti!=3)
				str[counti]=' ';
		}
		else
			break;
	}
	
	//sp1kOsdClearChar(OsdMaxX-3+ Xoffset, 0,3, 1);
	//sp1kOsdStrDraw(OsdMaxX-3+ Xoffset, 0, str, 0);
	sp1kOsdStrDrawExt(LCD_WIDTH - chrw*5-1+15, y, "        ", 0);	
	sp1kOsdStrDrawExt(LCD_WIDTH -strWidthGet(str)-1 -14+12, y, str, 0);
	
}

void uiRemainPixShow(void)
{ 
	UINT8 str[5]="0000";
	UINT16 tempNo;
	UINT8 counti;
	UINT16 x,y,chrw,chrh;

	sp1kOsdIconPsGet(ID_ICON_MODE_STILL,&x,&y);
	sp1kOsdFontSizeGet(&chrw, &chrh);
 
	tempNo=RemainPictureNO;
	str[0]=(UINT8)((tempNo/1000)+'0');
	tempNo=tempNo%1000;
	str[1]=(UINT8)((tempNo/100)+'0');
	tempNo=tempNo%100;
	str[2]=(UINT8)((tempNo/10)+'0');
	str[3]=(UINT8)((tempNo%10)+'0');
	for(counti=0;counti<5;counti++)
	{
		if(str[counti]==('0'))
		{
			if(counti!=3)
				str[counti]=' ';
		}
		else
			break;
	}

	//sp1kOsdClearChar(OsdMaxX-3, 0,3, 1);
	//sp1kOsdStrDraw(OsdMaxX-3, 0, str, 0);
	
	sp1kOsdStrDrawExt(LCD_WIDTH - chrw*5-1+15, y, "        ", 0);	
	sp1kOsdStrDrawExt(LCD_WIDTH -strWidthGet(str)-1 -14+12, y, str, 0);
}

void uiRemainPixCount(UINT8 show)
{
	isUpdateNum = 1;
	if(RemainPictureNO>0)
		RemainPictureNO--;
	else
		RemainPictureNO=0;
		
	if ( show )
	{
		//uiRemainPixShow();
		uiUpdateRemainPix();
	}
	//isUpdateNum = 0;
}

void uiFdModeUpdate(void)
{
	uiPara_t* tempUiPara;

	tempUiPara = appUiParaGet();
	if(tempUiPara->FDMode == FDMODE_OFF){
		sp1kOsdIconShow(ID_ICON_FD,ICON_CLEAR);	
	}else {
		sp1kOsdIconShow(ID_ICON_FD,ICON_SHOW);	
	}
}
void uiAntiShakeUpdate(void)
{
	uiPara_t* tempUiPara;

	tempUiPara = appUiParaGet();
	if(tempUiPara->AntiShake == PRV_ANTISHAKE_OFF){
		sp1kOsdIconShow(ID_ICON_ANTISHAKE,ICON_CLEAR);	
	}else {
		sp1kOsdIconShow(ID_ICON_ANTISHAKE,ICON_SHOW);	
	}
}

void uiSmildeUpdate(void)
{
	uiPara_t* tempUiPara;

	tempUiPara = appUiParaGet();
	if(tempUiPara->SmileMode == SMILEMODE_OFF){
		sp1kOsdIconShow(ID_ICON_SMILE,ICON_CLEAR);	
	}else {
		sp1kOsdIconShow(ID_ICON_SMILE,ICON_SHOW);	
	}
}
void uiIsoUpdate(void)
{
	uiPara_t* tempUiPara;

	tempUiPara = appUiParaGet();

	switch(tempUiPara->Iso)
	{
		case PRV_ISO_AUTO:
			sp1kOsdIconShow(ID_ICON_ISO_AUTO,ICON_SHOW);
			break;
		case PRV_ISO_100:
			sp1kOsdIconShow(ID_ICON_ISO_100,ICON_SHOW);
			break;
		case PRV_ISO_200:
			sp1kOsdIconShow(ID_ICON_ISO_200,ICON_SHOW);
			break;
		case PRV_ISO_400:
			sp1kOsdIconShow(ID_ICON_ISO_400,ICON_SHOW);
			break;
		case PRV_ISO_800:
			sp1kOsdIconShow(ID_ICON_ISO_800,ICON_SHOW);
			break;
	}
}

/**
 * @brief	Show a icon
 * @param	x	: axis-x
 			y	: axis-y
			icon:
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiIconDisp(UINT8 x, UINT8 y, UINT8 icon, UINT8 attr)
{
	UINT16 charW,charH;
	
	attr=attr;//just for clear warning
	
	sp1kOsdFontSizeGet(&charW, &charH);	
	osdIconDrawExt((UINT16)x*charW, (UINT16)y*charH, icon,0/*attr*/);
}

void uiIconDispExt(UINT16 x, UINT16 y, UINT8 icon, UINT8 attr)
{
	attr=attr;//just for clear warning
	osdIconDrawExt(x, y, icon,0/*attr*/);
}

//////////////////////////////
//更新time的函数
//-----------------------------------------------------------------------------
//uiUpdateOSDTime
//-----------------------------------------------------------------------------
/**
 * @brief	refresh osd time
 * @param	x	: axis-x
 			y	: axis-y
			time: input time based on ms
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
extern UINT16 strLen1,strLen2;
void uiUpdateOSDTime(UINT16 x, UINT16 y, UINT32 time, UINT8 attr)
{
	UINT32 temp;
	UINT8 min, sec ,hour;
	UINT8 str[10]={0};
	//UINT16 width;
	UINT16 strLen;
	//UINT16 len1,len2;

	hour = (UINT8) (time / 3600);
	temp = time % 3600;
	min = (UINT8) (temp / 60);
	sec = (UINT8) (temp % 60);

	//len1 = strWidthGet("444:44:44");
	//len2=strWidthGet("44:44:44");//jj mark for decreasing osd update time 

	if(hour > 99)
	 {
		strLen= strLen1;//len1;//strWidthGet("444:44:44");//for high capability card
		sprintf(str, "%03bu:%02bu:%02bu", hour,min,sec);
		str[9]='\0';
	 }
	 else
	{
		strLen= strLen2;//len2;//strWidthGet("44:44:44");
		sprintf(str, "%02bu:%02bu:%02bu", hour,min,sec);
		str[8]='\0';
	}

	//x-=strLen;	
	//sprintf(str, "%02bu:%02bu:%02bu", hour,min,sec);
	//str[9]='\0';
	sp1kOsdClearCharExt(x-strLen1,y,strLen1,MENU_CHAR_HEIGHT);	
	/*width = strWidthGet(str);
	if(width < strLen)
	{
	 	sp1kOsdClearCharExt(x+width,y,strLen-width,MENU_CHAR_HEIGHT);
	 	sp1kOsdClearCharExt(LCD_WIDTH- len1,y,len1-len2,MENU_CHAR_HEIGHT);  //mantis 43372
	}*///jj mark for decreasing osd update time 
	if (sp1kVideoRecStatusGet() != VID_REC_IDLE)
		appOsdColorSet(OSD_COLOR_RED);
	sp1kOsdStrDrawExt(x-strLen, y, str, attr);
	appOsdColorSet(OSD_COLOR_WHITE);
}

//更新audio rec的osd时间
//-----------------------------------------------------------------------------
//uiUpdateAudRecTime
//-----------------------------------------------------------------------------
/**
 * @brief	refresh osd time in audio recording
 * @param	time: input time based on ms
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateAudRecTime(UINT32 time)
{
	UINT16 x,y,chrw,chrh;

	sp1kOsdFontSizeGet(&chrw, &chrh);
	uiUpdateOSDTime(LCD_WIDTH/2+strWidthGet("44:44:44")/2, LCD_HEIGHT/2-1, time, 0);
	
	if (sp1kAudioRecTypeGet() != AUDIO_TYPE_VOICE) {
		if(AudRecRemainTime>= time) {
			sp1kOsdIconPsGet(ID_ICON_MODE_VOICE,&x,&y);
			uiUpdateOSDTime(LCD_WIDTH, y, AudRecRemainTime - time, 0);
		}
		else {
			osMsgPost(SP1K_MSG_KEY_PRESS_S2);
		}
	}
}

//更新audio rec的osd时间
//-----------------------------------------------------------------------------
//uiUpdateAudPlayTime
//-----------------------------------------------------------------------------
/**
 * @brief	refresh osd time in audio playing
 * @param	time: input time based on ms
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateAudPlayTime(UINT32 time)
{
	UINT16 chrw,chrh;

	sp1kOsdFontSizeGet(&chrw, &chrh);
	uiUpdateOSDTime(LCD_WIDTH/2+strWidthGet("44:44:44")/2, LCD_HEIGHT/2-1 ,time, 0);

}

//更新video rec的时间
//-----------------------------------------------------------------------------
//uiUpdateVideoRecTime
//-----------------------------------------------------------------------------
/**
 * @brief	refresh osd time in video recording
 * @param	time: input time based on ms
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateVideoRecTime(UINT32 time)
{ 
	UINT16 x,y;
	
//	#if (VIDEO_SHOW_AE_INFO_EN == 1)
//	UINT8 aeTabIdx, curLuma, curContrast;
//	#endif
	sp1kOsdDispBuffCpy(1,0);
	sp1kOsdDispBuffSw();
	if(APP_STATE_VIDEO_VIEW == appCurrStateGet())
		sp1kOsdIconPsGet(ID_ICON_MODE_VIDEO,&x,&y);
	else
		sp1kOsdIconPsGet(ID_ICON_MODE_PLAYBACK,&x,&y);

	#if SUPPORT_AUTO_TEST
	if (autoTestOpenFlag) {
		if (1 == autoTestStatus) {
			if (3 == time) {
				osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
		}
	}
	#endif

	uiUpdateOSDTime(LCD_WIDTH, y,  time, 0);
	
	//if(sp1kVideoRecStampStsGet()&&(APP_STATE_VIDEO_VIEW == appCurrStateGet()))
	//{
	//	appCurrentTimeShow(time);
	//}
	sp1kOsdDispBuffShow();
//	#if (VIDEO_SHOW_AE_INFO_EN == 1)
//	sp1kAeShowInfo();


	
	//aeAwbInfo(&aeTabIdx, &curLuma, &curContrast);
	//sp1kOsdNumberShow(aeTabIdx, OsdMaxX-7, OsdMaxY-1, 3, 0);
	//sp1kOsdNumberShow(curLuma, OsdMaxX-3, OsdMaxY-1, 3, 0);
//	#endif
}

//取得aud record剩余时间
//-----------------------------------------------------------------------------
//uiUpdateAudRecRemainTime
//-----------------------------------------------------------------------------
/**
 * @brief	get audio record remaining time
 * @param	time: output time based on ms
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateAudRecRemainTime(UINT32* time)
{
	UINT32 remaintime;
#if 1//def DOS32_SUPPORT
	float avgKBytePerSec;
	float audResv = (float)(AUD_FRAG_SIZE+ vfsClusterSizeGet()+ 500 * 1024UL)/1024;

	#if(AUD_FMT == ADPCM_FMT)
	float samplePerBlock;
	samplePerBlock = (AUD_BLOCK_ALIGN - 4 )*2 + 1;
	avgKBytePerSec = (float)(audRecCtrl.freq  * AUD_BLOCK_ALIGN)/ samplePerBlock/1024;
	#else
	avgKBytePerSec = (float)(audRecCtrl.freq  * AUD_SAMPLE_BIT) / 8/1024;
	#endif
	
	if(sp1kDosGetDiskSpace() > audResv) {
		remaintime = (UINT32)((sp1kDosGetDiskSpace() -audResv) / avgKBytePerSec);//eric2008@0502
#else
	UINT32 avgKBytePerSec;
	UINT32 audResv = (AUD_FRAG_SIZE+ G_DOS_ClusterSize+ 500 * 1024UL);

	#if(AUD_FMT == ADPCM_FMT)
	UINT32 samplePerBlock;
	samplePerBlock = (AUD_BLOCK_ALIGN - 4 )*2 + 1;
	avgKBytePerSec = audRecCtrl.freq  * AUD_BLOCK_ALIGN/ samplePerBlock;
	#else
	avgKBytePerSec = audRecCtrl.freq  * AUD_SAMPLE_BIT)/ 8;
	#endif
	
	if((UINT32)sp1kDosGetDiskSpace() * G_DOS_ClusterSize > audResv) {
		remaintime = ((UINT32)sp1kDosGetDiskSpace() * G_DOS_ClusterSize -audResv) / avgKBytePerSec;
#endif
	} else {
		remaintime = 0;
	}
#if (!NAND_SUPPORT)
	//Phil add for disable nand flash
	if ( G_Card_Type == K_MEDIA_NANDF) {
		remaintime = 0;
	}
#endif
	*time = remaintime ;
}

//取得video rec剩余时间
//-----------------------------------------------------------------------------
//uiUpdateVideoRecRemainTime
//-----------------------------------------------------------------------------
/**
 * @brief	get video record remaining time
 * @param	time: output time based on ms
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateVideoRecRemainTime(UINT32* time)
{
	*time = sp1kVideoRecRemainTimeGet();
#if 0
#if 1//def DOS32_SUPPORT
	UINT32 freeCluster;
#else
	UINT16 freeCluster;
#endif
	UINT32 spareFrame;
	//UINT16 videoWidth, videoHeight;
	float tt;
	UINT32 audPerSec = 44100UL * 2;
	UINT32 frameRate = 30;
	UINT32 cmpRate = 22;
	extern UINT32 FRAG_BUF_SIZE;
	UINT32 clusterSize = vfsClusterSizeGet();
	float vidResv = ((FRAG_BUF_SIZE) + 512L*3 + clusterSize)/clusterSize;
	//UINT32 freeSpace;
	uiPara_t* tempUiPara;

	tempUiPara = appUiParaGet();
	switch(tempUiPara->FrameRate){
		case MOV_FRM_HIGH:
			frameRate = 30;
			break;
		case MOV_FRM_MIDDLE:
			frameRate=20;
			break;
		case MOV_FRM_LOW:
			frameRate = 15;
			break;
		case MOV_FRM_MAX:
			frameRate = 60;
			break;
		default:
			frameRate = 30;
			break;
	}

	//20080402 @ suny modify start
	switch(tempUiPara->VideoSize)
	{
	   case MOV_SIZE_1080P:
			tt = (float)(frameRate*1920L*1080L*2L/cmpRate  + audPerSec)/(float)clusterSize;
			break;
		case MOV_SIZE_HD:
			tt = (float)(frameRate*1280L*720L*2L/cmpRate  + audPerSec)/(float)clusterSize;
			break;
		case MOV_SIZE_D1:
			tt = (float)(frameRate*720L*480L*2L/cmpRate  + audPerSec)/(float)clusterSize;
			break ;
		case MOV_SIZE_VGA:
			tt = (float)(frameRate*640L*480L*2L/cmpRate  + audPerSec)/(float)clusterSize;
			break;
		case MOV_SIZE_QVGA:
			tt = (float)(frameRate*320L*240L*2L/cmpRate + audPerSec)/(float)clusterSize;
			break;
		case MOV_SIZE_QVGA_HFR:
			tt = (float)(frameRate*2 *320L*240L*2L/cmpRate + audPerSec)/(float)clusterSize;
			break;
		default:
			tt = (float)(frameRate *640L*480L*2L/cmpRate + audPerSec)/(float)clusterSize;
			break;
	}

#if 1//def DOS32_SUPPORT
	freeCluster = DOS_FreeDiskSpaceByCluster();//eric@20080502
#else
	freeCluster = DOS_GetDiskSpace();
#endif


	spareFrame = ((UINT32) freeCluster * (UINT32)clusterSize )&0xffffffff;
	if(spareFrame < vidResv) {
		spareFrame = 0;
	}
	spareFrame = (UINT32)(((float) freeCluster - vidResv)/tt);

	//20080402 @ suny modify end

	if (spareFrame < 8)
		spareFrame = 0;		//lower limit 8s
	else
		spareFrame -= 8;

#if (!NAND_SUPPORT)
		//Phil add for disable nand flash
		if ( G_Card_Type == K_MEDIA_NANDF) {
			spareFrame = 0;
		}
#endif

	*time = spareFrame ;
#endif
}

///////////////////////////////
//更新分辨率图标
//-----------------------------------------------------------------------------
//uiUpdateOSDImgSize
//-----------------------------------------------------------------------------
/**
 * @brief	refresh resolution icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateImgSize(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	switch(puiPara->ImageSize)
	{	
		case PRV_SIZE_50M:
			sp1kSnapSizeSet( 8160, 6120 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_50M,ICON_SHOW);
			break;
		case PRV_SIZE_44M:
			sp1kSnapSizeSet( 7664, 5748 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_44M,ICON_SHOW);
			break;
		case PRV_SIZE_42M:
			sp1kSnapSizeSet( 7488,5616 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_42M,ICON_SHOW);
			break;
		case PRV_SIZE_40M:
			sp1kSnapSizeSet( 7200,5400 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_40M,ICON_SHOW);
			break;
		case PRV_SIZE_36M:
			sp1kSnapSizeSet( 6944,5208 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_36M,ICON_SHOW);
			break;
		case PRV_SIZE_30M:
			sp1kSnapSizeSet( 6320,4740 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_30M,ICON_SHOW);
			break;
		case PRV_SIZE_24M:
			sp1kSnapSizeSet( 5600, 4200 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_24M,ICON_SHOW);
			break;
		case PRV_SIZE_20M:
			sp1kSnapSizeSet( 5120, 3840 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_20M,ICON_SHOW);
			break;
		case PRV_SIZE_18M:
			sp1kSnapSizeSet( 4896, 3672 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_18M,ICON_SHOW);
			break;
		case PRV_SIZE_16M:
			sp1kSnapSizeSet( 4608, 3456 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_16M,ICON_SHOW);
			break;
		case PRV_SIZE_12M:
			sp1kSnapSizeSet( 3888, 2916 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_12M,ICON_SHOW);
			break;
		case PRV_SIZE_10M:
			sp1kSnapSizeSet( 3664, 2748 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_10M,ICON_SHOW);
			break;
		case PRV_SIZE_9M:
			sp1kSnapSizeSet( 3456, 2592 );
			//sp1kSnapSizeSet( 4544, 3408 );
			//sp1kSnapSizeSet( 4608, 3072 );//14M
			//sp1kSnapSizeSet( 4000, 3000 );//12M
			//sp1kSnapSizeSet( 3920, 2940 );//10M
			sp1kOsdIconShow(ID_ICON_RESOLUTION_9M,ICON_SHOW);
			break;
		case PRV_SIZE_8M:
			sp1kSnapSizeSet( 3264, 2448 );
			//sp1kSnapSizeSet( 4544, 3408 );
			//sp1kSnapSizeSet( 4608, 3072 );//14M
			//sp1kSnapSizeSet( 4000, 3000 );//12M
			//sp1kSnapSizeSet( 3920, 2940 );//10M
			sp1kOsdIconShow(ID_ICON_RESOLUTION_8M,ICON_SHOW);
			break;
		case PRV_SIZE_7M_16_9:
			sp1kSnapSizeSet( 3648, 2048 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_7M_16_9,ICON_SHOW);
			break;	
		case PRV_SIZE_5M:
			sp1kSnapSizeSet( 2592, 1944 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_5M,ICON_SHOW);
			break;
		case PRV_SIZE_3M:
			sp1kSnapSizeSet( 2048, 1536 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_3M,ICON_SHOW);
			break;
		case PRV_SIZE_2M:
			sp1kSnapSizeSet( 1600, 1200 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_2M,ICON_SHOW);
			break;
		case PRV_SIZE_2M_16_9:
			sp1kSnapSizeSet( 1920, 1080 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_2M_16_9,ICON_SHOW);
			break;	
		case PRV_SIZE_1M_16_9:
			sp1kSnapSizeSet( 1280, 720 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_1M_16_9,ICON_SHOW);
			break;
		case PRV_SIZE_1M:
			sp1kSnapSizeSet( 1024, 768 );
			//sp1kSnapSizeSet( 1280, 960 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_1M,ICON_SHOW);
			break;
		case PRV_SIZE_VGA:
			sp1kSnapSizeSet( 640, 480 );
			sp1kOsdIconShow(ID_ICON_RESOLUTION_VGA,ICON_CLEAR);
			break;
	}
}

//////////////////////////////////////
//更新拍照质量图标
//-----------------------------------------------------------------------------
//uiUpdateOSDImgQuality
//-----------------------------------------------------------------------------
/**
 * @brief	refresh img quality icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateImgQuality(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	switch(puiPara->ImageQuality)
	{
		case PRV_QUAL_SUPER:
			sp1kOsdIconShow(ID_ICON_QUALITY_SUPER_FINE,ICON_SHOW);
			break;
		case PRV_QUAL_FINE:
			sp1kOsdIconShow(ID_ICON_QUALITY_FINE,ICON_SHOW);
			break;
		case PRV_QUAL_NORMAL:
			sp1kOsdIconShow(ID_ICON_QUALITY,ICON_SHOW);
			break;
	}
}

////////////////////////////////
//更新驱动方式图标
//-----------------------------------------------------------------------------
//uiUpdateOSDSnapMode
//-----------------------------------------------------------------------------
/**
 * @brief	refresh snap mode icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateSnapMode(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning

	switch (puiPara->DriverMode) {
	case PRV_SNAP_SINGLE:
		sp1kOsdIconShow(ID_ICON_SELFTIMER_2SEC,ICON_CLEAR);
		break;

	case PRV_SNAP_BURST:
		sp1kOsdIconShow(ID_ICON_BURST,ICON_SHOW);
		break;

	case PRV_SNAP_SELF_2S:
		sp1kOsdIconShow(ID_ICON_SELFTIMER_2SEC,ICON_SHOW);
		break;

	case PRV_SNAP_SELF_5S:
		sp1kOsdIconShow(ID_ICON_SELFTIMER_5SEC,ICON_SHOW);
		break;

	case PRV_SNAP_SELF_10S:
		sp1kOsdIconShow(ID_ICON_SELFTIMER_10SEC,ICON_SHOW);
		break;

	case PRV_SNAP_CYC_2S:
		sp1kOsdIconShow(ID_ICON_CYC_SNAP_2SEC,ICON_SHOW);
		break;

	case PRV_SNAP_CYC_5S:
		sp1kOsdIconShow(ID_ICON_CYC_SNAP_5SEC,ICON_SHOW);
		break;

	case PRV_SNAP_CYC_10S:
		sp1kOsdIconShow(ID_ICON_CYC_SNAP_10SEC,ICON_SHOW);
		break;
	}
	
	appOsdColorSet(OSD_COLOR_WHITE);
}


/////////////////////////////////
//更新白平衡图标
//-----------------------------------------------------------------------------
//uiUpdateOSDWB
//-----------------------------------------------------------------------------
/**
 * @brief	refresh WB mode icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateWB(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	switch(puiPara->WBMode)
	{
		case PRV_WB_AUTO:
			sp1kOsdIconShow(ID_ICON_WB_AUTO,ICON_SHOW);
			appWBSet(PRV_WB_AUTO);
			break;
		case PRV_WB_DAYLIGHT:
			sp1kOsdIconShow(ID_ICON_WB_DAY,ICON_SHOW);
			appWBSet(PRV_WB_DAYLIGHT);
			break;
		case PRV_WB_CLOUDY:
			sp1kOsdIconShow(ID_ICON_WB_CLOUDY,ICON_SHOW);
			appWBSet(PRV_WB_CLOUDY);
			break;
		case PRV_WB_FLUORESCENT_H:
			sp1kOsdIconShow(ID_ICON_WB_FLOURSCENT_H,ICON_SHOW);
			appWBSet(PRV_WB_FLUORESCENT_H);
			break;
		case PRV_WB_TUNGSTEN:
			sp1kOsdIconShow(ID_ICON_WB_TUNGSTEN,ICON_SHOW);
			appWBSet(PRV_WB_TUNGSTEN);
			break;
	}
}

//////////////////////////////
//更新effect图标
//-----------------------------------------------------------------------------
//uiUpdateOSDEffect
//-----------------------------------------------------------------------------
/**
 * @brief	refresh snap effect icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateEffect(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning

	switch(puiPara->ImageEffect)
	{
		case PRV_EFFECT_NORNAL:
			sp1kOsdIconShow(ID_ICON_DSP_NORMAL,ICON_CLEAR);
			break;
		case PRV_EFFECT_BLACKWHITE:
			sp1kOsdIconShow(ID_ICON_DSP_BW,ICON_SHOW);
			break;
		case PRV_EFFECT_SEPIA:
			sp1kOsdIconShow(ID_ICON_DSP_SEPIA,ICON_SHOW);
			break;
		case PRV_EFFECT_NEGATIVE:
			sp1kOsdIconShow(ID_ICON_DSP_NEGATIVE,ICON_SHOW);
			break;
		case PRV_EFFECT_RED:
			sp1kOsdIconShow(ID_ICON_DSP_R,ICON_SHOW);
			break;
		case PRV_EFFECT_GREEN:
			sp1kOsdIconShow(ID_ICON_DSP_G,ICON_SHOW);
			break;
		case PRV_EFFECT_BLUE:
			sp1kOsdIconShow(ID_ICON_DSP_B,ICON_SHOW);
			break;
	}
}


//////////////////////////////
//更新EV图标
//-----------------------------------------------------------------------------
//uiUpdateOSDEffect
//-----------------------------------------------------------------------------
/**
 * @brief	refresh snap effect icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateEV(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	//return;
	switch(puiPara->AEMode)
	{
		case PRV_COMP_N20EV:
			sp1kOsdIconShow(ID_ICON_EV_N20,ICON_SHOW);
			break;
		case PRV_COMP_N17EV:
			sp1kOsdIconShow(ID_ICON_EV_N17,ICON_SHOW);
			break;
		case PRV_COMP_N13EV:
			sp1kOsdIconShow(ID_ICON_EV_N13,ICON_SHOW);
			break;
		case PRV_COMP_N10EV:
			sp1kOsdIconShow(ID_ICON_EV_N10,ICON_SHOW);
			break;
		case PRV_COMP_N07EV:
			sp1kOsdIconShow(ID_ICON_EV_N07,ICON_SHOW);
			break;
		case PRV_COMP_N03EV:
			sp1kOsdIconShow(ID_ICON_EV_N03,ICON_SHOW);
			break;
		case PRV_COMP_0EV:
			sp1kOsdIconShow(ID_ICON_EV_00,ICON_SHOW);
			break;
		case PRV_COMP_P03EV:
			sp1kOsdIconShow(ID_ICON_EV_P03,ICON_SHOW);
			break;
		case PRV_COMP_P07EV:
			sp1kOsdIconShow(ID_ICON_EV_P07,ICON_SHOW);
			break;
		case PRV_COMP_P10EV:
			sp1kOsdIconShow(ID_ICON_EV_P10,ICON_SHOW);
			break;
		case PRV_COMP_P13EV:
			sp1kOsdIconShow(ID_ICON_EV_P13,ICON_SHOW);
			break;
		case PRV_COMP_P17EV:
			sp1kOsdIconShow(ID_ICON_EV_P17,ICON_SHOW);
			break;
		case PRV_COMP_P20EV:
			sp1kOsdIconShow(ID_ICON_EV_P20,ICON_SHOW);
			break;
	}
}



void uiSceneModeUpdate(void)
{

	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->SceneMode)
	{
		case PRV_SCENE_AUTO:
			sp1kOsdIconShow(ID_ICON_SCENE_AUTO,ICON_CLEAR);
			break;
		case PRV_SCENE_BEACH:
			sp1kOsdIconShow(ID_ICON_SCENE_BEACH,ICON_SHOW);
			break;
		case PRV_SCENE_DINNER:
			sp1kOsdIconShow(ID_ICON_SCENE_DINNER,ICON_SHOW);
			break;
		case PRV_SCENE_LANDSCAPE:
			sp1kOsdIconShow(ID_ICON_SCENE_LANDSCAPE,ICON_SHOW);
			break;
		case PRV_SCENE_NIGHTPORTRAIT:
			sp1kOsdIconShow(ID_ICON_SCENE_NIGHTPORTRAIT,ICON_SHOW);
			break;
		case PRV_SCENE_NIGHTSCENERY:
			sp1kOsdIconShow(ID_ICON_SCENE_NIGHTSCENERY,ICON_SHOW);
			break;
		case PRV_SCENE_PORTRAIT:
			sp1kOsdIconShow(ID_ICON_SCENE_PORTRAIT,ICON_SHOW);
			break;
		case PRV_SCENE_SPORT:
			sp1kOsdIconShow(ID_ICON_SCENE_SPORT,ICON_SHOW);
			break;
		default:
			sp1kOsdIconShow(ID_ICON_SCENE_AUTO,ICON_CLEAR);
			break;
	}
}


///////////////////////////////
//更新ae meter图标
//-----------------------------------------------------------------------------
//uiUpdateOSDAEMetering
//-----------------------------------------------------------------------------
/**
 * @brief	refresh AE meter method icon in pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateAEMetering(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning

	if (appUserMeterModeGet() == 0) {
		switch(puiPara->AEMetering)
		{
			case PRV_METER_MULTI:
				sp1kOsdIconShow(ID_ICON_METERING_MULTI,ICON_SHOW);
				sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
				break;
			case PRV_METER_SPOT:
				sp1kOsdIconShow(ID_ICON_METERING_SPOT,ICON_SHOW);
				sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_SPOT);
				break;
			case PRV_METER_CENTER:
				sp1kOsdIconShow(ID_ICON_METERING_CENTER,ICON_SHOW);
				sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_CENTER);
				break;
			default:
				sp1kOsdIconShow(ID_ICON_METERING_MULTI,ICON_SHOW);
				sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
				break;
		}
	}
	else {
		sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_USER_DEFINE);
	}
}

////////////////////////////////
//更新video size图标
//-----------------------------------------------------------------------------
//uiUpdateOSDVideoSize
//-----------------------------------------------------------------------------
/**
 * @brief	refresh video size in video pv mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateVideoSize(UINT8 x, UINT8 y, UINT8 attr)
{
	uiPara_t* puiPara = appUiParaGet();
	x=x;y=y;attr=attr;//clear warning
	switch(puiPara->VideoSize)
	{
		case MOV_SIZE_QVGA_HFR:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_320_H,ICON_SHOW);
			break;
		case MOV_SIZE_VGA:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_640,ICON_SHOW);
			break;
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
		case MOV_SIZE_D1:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_D1,ICON_SHOW);
			break;			
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
		case MOV_SIZE_QVGA:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_320,ICON_SHOW);
			break;
		case MOV_SIZE_HD:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_HD,ICON_SHOW);
			break;
		case MOV_SIZE_1080P:
			sp1kOsdIconShow(ID_ICON_MOVIE_SIZE_FHD,ICON_SHOW);
			break;
	}
}

////////////////////////////////
//更新still preview的zoom factor的信息
//-----------------------------------------------------------------------------
//uiUpdateOSDPvZoom
//-----------------------------------------------------------------------------
/**
 * @brief	refresh zoom factor in pv mode
 * @param	zoomfact : zoom  factor
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
#define ZOOM_SLIDER_W 12
#define ZOOM_SLIDER_H 5
#define ZOOM_OUTLINE_HEIGHT 2 // 4
#define ZOOM_OUTLINE_WIDTH 10


void uiUpdateOSDPvZoom(UINT16 zoomfact, UINT8 mode)
{
	UINT16 x, y, iconx, icony, iconHeight, iconWidth;	
	UINT8 strFact[3] = {0};

	static UINT16 perFact[3] = 100;
	static UINT8 factStep[3] = 0;
	static UINT8 factDir[3] = 1;
	UINT8 str[] = "x30";
	UINT16 width,height;

	if(zoomfact == 100){
		sp1kOsdClearCharExt(284, 60, 32, 150);
		return;
	}
	
	if (mode >= 3)
		mode = 0;
	
	sp1kOsdIconPsGet(ID_ICON_DIALOG_ZOOM,&iconx,&icony);
	iconWidth = sp1kOsdIconWidthGet(ID_ICON_DIALOG_ZOOM);
	iconHeight = sp1kOsdIconHeightGet(ID_ICON_DIALOG_ZOOM);

	x = (iconx + ZOOM_OUTLINE_WIDTH);
	y = icony + iconHeight - ZOOM_OUTLINE_HEIGHT - ZOOM_SLIDER_H;
	
	if (zoomfact == 100 || zoomfact == 300) {
		if (zoomfact == 100) {
			perFact[mode] = 100;
			factStep[mode] = 0;
			factDir[mode] = 1;
			y = icony + iconHeight - ZOOM_OUTLINE_HEIGHT - ZOOM_SLIDER_H;
		} else {
			perFact[mode] = 300;
			
			if(appCurrStateGet() == APP_STATE_VIDEO_VIEW){
				factStep[mode] = 12;//30;
			}else{
				factStep[mode] = 12;//38;
			}
			
			factDir[mode] = 0;
			y = icony + ZOOM_OUTLINE_HEIGHT;
		}
	} else {
		
		if (perFact[mode] < zoomfact) {
			factDir[mode] = 1;
		} else if (perFact[mode] > zoomfact) {
			factDir[mode] = 0;
		}
		
		if (perFact[mode] != zoomfact) {
			if (factDir[mode]) {
				factStep[mode]++;
			} else {
				factStep[mode]--;
			}
			
			perFact[mode] = zoomfact;
		}

		if(appCurrStateGet() == APP_STATE_VIDEO_VIEW){
			y = y - (factStep[mode] * 11.7 );//4.8 );//6.85
		}else{
			y = y - (factStep[mode] * 11.7 );//6.85
		}
			
	}

	sp1kOsdIconShow(ID_ICON_DIALOG_ZOOM,ICON_SHOW);

	/*
	if(factStep[mode]){
			
		sprintf(str, "x%d", (UINT16)factStep[mode]+1);
		
		sp1kOsdClearCharExt(277, 210,strWidthGet("x30"),MENU_CHAR_HEIGHT);
		
		sp1kOsdStrDrawExt(277, 210, str, 1);
	}else{
		sp1kOsdClearCharExt(277, 210,strWidthGet("x30"),MENU_CHAR_HEIGHT);
	}
	*/
	
	sp1kOsdIndexSave();
	sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_GREEN, 1);
	sp1kOsdRectDraw(x, y, ZOOM_SLIDER_W, ZOOM_SLIDER_H);
	sp1kOsdIndexRestore();

}

#if 1
void uiUpdateOSDPvZoom_BAR(UINT16 zoomfact)
{
	UINT8 factint;//整数部分
	UINT8 factdec;//小数部分
	UINT8 OsdMaxX, OsdMaxY;	
	UINT16 x,y;
	UINT8 attr;
	UINT8 str[128];
	UINT16 width;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	attr = 0x00;
	
	sp1kOsdIconPsGet(ID_ICON_DIALOG_ZOOM,&x,&y);
	
	//如果是1.00倍的时候，应clear osd
	if(zoomfact == 100)
	{
		sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_DIALOG_ZOOM)+strWidthGet("4.44"),MENU_CHAR_HEIGHT); 
		return;
	}


	factint = (UINT8)(zoomfact / 100);
	factdec = (UINT8)(zoomfact % 100);

	sp1kOsdIconShow(ID_ICON_DIALOG_ZOOM,ICON_SHOW);
	sprintf(str,"%bu.%02bu",factint,factdec);
	width = strWidthGet(str);
	if(width < strWidthGet("4.44"))
	{
		sp1kOsdClearCharExt(x+sp1kOsdIconWidthGet(ID_ICON_DIALOG_ZOOM)+width,y,strWidthGet("4.44")-width,MENU_CHAR_HEIGHT);
	}
	sp1kOsdStrDrawExt(x+sp1kOsdIconWidthGet(ID_ICON_DIALOG_ZOOM), y, str, attr);

}
#endif
///////////////////////////////////
//显示pb文件的类型图标
//-----------------------------------------------------------------------------
//uiUpdateOSDPbType
//-----------------------------------------------------------------------------
/**
 * @brief	refresh pb file type in pb mode
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateOSDPbType(UINT8 x, UINT8 y, UINT8 attr)
{
	static UINT8 xdata pre_Type = SP1K_PB_FILE_TYPE_UNKNOWN;
	x=x;y=y;attr=attr;//clear warning

	if( (pre_Type != pbFile.wType) || (appPreStateGet(0)!= appCurrStateGet()) )
	{
		switch(pre_Type)
		{
			case SP1K_PB_FILE_TYPE_JPG:
				break;
			case SP1K_PB_FILE_TYPE_AVI:
			case SP1K_PB_FILE_TYPE_WGI:
				sp1kOsdIconShow(ID_ICON_MODE_VIDEO,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_BUTTON_SNAP,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_CLEAR);
				break;
			case SP1K_PB_FILE_TYPE_WAV:
				sp1kOsdIconShow(ID_ICON_MODE_VOICE,ICON_CLEAR);
				break;
		}
		pre_Type = pbFile.wType;
	}
	else
	{
		return ;
	}
	
	switch(pbFile.wType)
	{
		case SP1K_PB_FILE_TYPE_JPG:
			sp1kOsdIconShow(ID_ICON_MODE_STILL,ICON_SHOW);
			break;
		case SP1K_PB_FILE_TYPE_AVI:
		case SP1K_PB_FILE_TYPE_WGI:
			sp1kOsdIconShow(ID_ICON_MODE_VIDEO,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUTTON_SNAP,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
			
			appOsdColorSet(OSD_COLOR_BLUE);
			//sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN_L,ICON_SHOW);
			//sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN_H,ICON_SHOW);
			appOsdColorSet(OSD_COLOR_WHITE);
			osdIconDrawExt(152,103,ID_ICON_PLAY_STS_RUN_H,1);		
			break;
		case SP1K_PB_FILE_TYPE_WAV:
			sp1kOsdIconShow(ID_ICON_MODE_VOICE,ICON_SHOW);
			break;
	}
}

//-----------------------------------------------------------------------------
//uiUpdateOSDPbFileNum
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 * @brief	refresh pb file number in pb mode
 * @param	NONE
 * @retval	void
 * @see
 * @author	phil.lin
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateOSDPbFileNum(void)
{
	UINT8 str[20];
	UINT16 temp = 0;
	UINT8 Xoffset = 0; 
	UINT16 x,y,chrw,chrh;
 
	sp1kOsdIconPsGet(ID_ICON_MODE_PLAYBACK,&x,&y);
	sp1kOsdFontSizeGet(&chrw, &chrh);
	uiRemainPixInit();

	if(appPanelTypeGet() == 0) {
		Xoffset = 0;
	}
	sprintf(str,"%u/%u",pbFile.dwIndex,sp1kPbFileCntGet());
	temp = osdStrWidthGet(str);

	//sp1kOsdClearChar(OsdMaxX-5+ Xoffset,0,5,1);
	//sp1kOsdStrDrawExt(OsdMaxX*16 - temp + Xoffset, 0, str, 0);
	sp1kOsdClearCharExt(LCD_WIDTH - 9*13-1, y,9*12,chrh);
	//sp1kOsdStrDrawExt(LCD_WIDTH - chrw*4-1, y, "              ", 0);	
	sp1kOsdStrDrawExt(LCD_WIDTH - temp-1 -14, y, str, 0);
	
}

void uiUpdateOSDMultiPbFileNum(void)
{
	UINT8 str[20];
	UINT16 temp = 0;
	UINT8 Xoffset = 0; 
	UINT16 x,y;
 
	sp1kOsdIconPsGet(ID_ICON_MODE_PLAYBACK,&x,&y);

	uiRemainPixInit();

	if(appPanelTypeGet() == 0) {
		Xoffset = 0;
	}
	sprintf(str,"%u/%u",pbFile.dwIndex,sp1kPbFileCntGet());
	temp = osdStrWidthGet(str);
	sp1kOsdStrDrawExt(LCD_WIDTH - temp-1, y, str, 3);
}


///////////////////////////////////
//显示pb时no image图
#ifndef FUNC_CLEAR
//-----------------------------------------------------------------------------
//uiUpdateOSDPbNoImage
//-----------------------------------------------------------------------------
/**
 * @brief	show pb no image
 * @param	x	: axis-x
 			y	: axis-y
			attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateOSDPbNoImage(void)
{
	UINT8 i;
	UINT32 scrAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 dstAddr;
	UINT16 decWidth = PB_DECODE_WIDTH;
	UINT16 decHeight = PB_DECODE_HEIGHT;
	UINT8 OsdMaxX, OsdMaxY;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	sp1kPbDispBuffSwitch();
	dstAddr = sp1kPbDispBuffGet(1);

	sp1kOsdClear(0);
	/*将pv层填充为全黑色*/#if 1
	sp1kHalDRAMSetStartAddr(scrAddr, 0);
	for (i = 0; i < 16; i++)
	{
		if ((i >> 2) == 0 ||(i >> 2) == 2)
		{
			sp1kHalDRAMWriteWord(0, 0);
		}
		else
		{
			sp1kHalDRAMWriteWord(0x80, 0x80);
		}
	}

	sp1kHalGprmScale(
		scrAddr,
		8, 2,
		dstAddr,
		decWidth, decHeight,
		dstAddr,
		1
	);

	sp1kDispImgShow(dstAddr, decWidth, decHeight, 100);
	/*将pv层填充为全黑色 end*/#endif
	sp1kOsdStrDraw((OsdMaxX >> 1) - 4, OsdMaxY >> 1, "No Image", 0);
}
#endif

///////////////////////////////////
//显示memory full图
//-----------------------------------------------------------------------------
//uiUpdateOSDMemFull
//-----------------------------------------------------------------------------
/**
 * @brief	show memory full
 * @param	attr: osd display attr
 * @retval	void
 * @see
 * @author
 * @since	2008-02-25
 * @todo
 * @bug
*/
void uiUpdateOSDMemFull(void)
{
	//sp1kHalCtrlScreen(3);//for mantis #41044
	uiOsdDialogDisp(ID_STR_MEMORY_FULL, 1000);
	//sp1kHalCtrlScreen(3);
}

///////////////////////////////////
//显示battery empty图
//-----------------------------------------------------------------------------
//uiUpdateOSDBatEmpty
//-----------------------------------------------------------------------------
/**
 * @brief	show memory full
 * @param	attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-25
 * @todo
 * @bug
*/
void uiUpdateOSDBatEmpty(void)
{ 
	UINT8 num=0;
 
	//add by phil@08.05.21
	sp1kBtnDisableAll();
	for(num=0;num<5;num++)
	{
		appOsdColorSet(OSD_COLOR_RED);
		sp1kOsdClear(0);
		sp1kHalWait(150);
		uiOsdDialogDisp(ID_STR_LOW_BATTERY, 0);
		sp1kHalWait(150);
		appOsdColorSet(OSD_COLOR_WHITE);
	}
	osMsgFlush(MSG_CTRL_FLUSH_ALL); //ready to power off
	//add by phil@08.05.21
	sp1kBtnEnableAll();
}

///////////////////////////////////
//显示文件lock
#ifndef FUNC_CLEAR
//-----------------------------------------------------------------------------
//uiUpdateOSDFileLock
//-----------------------------------------------------------------------------
/**
 * @brief	show file lock
 * @param	attr: osd display attr
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-25
 * @todo
 * @bug
*/
void uiUpdateOSDFileLock(void)
{
	sp1kOsdClear(0);
	uiOsdDialogDisp(ID_STR_FILE_LOCKED_, 0);
}
#endif

///////////////////////////////////
//更新电池电量图标
//-----------------------------------------------------------------------------
//uiUpdateBatteryLevel
//-----------------------------------------------------------------------------
/**
 * @brief	refresh battery level icon
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-25
 * @todo
 * @bug
*/
UINT8 uiUpdateBatteryLevel(void)
{
	UINT8 Xoffset = 0;
	//powerBatLv_t* pbatlv;
	//pbatlv = appBatLvGet();
	powerBatLv_t batlv; 	
 
	appBatLvGet(&batlv);
	if(appPanelTypeGet() == 0) {
		Xoffset = 1;
	}
	/*
	If power off process is running, keep the battery empty icon
	even the level of battery back to high!
	OR,
	you can stop the power off process when the level of battery back to high
	by using sp1kPwrOffCfg(0, 0) and update the battery icon!
	*/
	if(sp1kPwrOffProcChk()) {
		sp1kOsdIconShow(ID_ICON_BATTERY_EMPTY,ICON_SHOW);
		return batlv;
	}

	switch (batlv)
	{
		case POWER_BAT_FULL:
			sp1kOsdIconShow(ID_ICON_BATTERY_FULL,ICON_SHOW);
			break;
		case POWER_BAT_HALF:
			sp1kOsdIconShow(ID_ICON_BATTERY_HALF,ICON_SHOW);
			break;
		case POWER_BAT_LOW:
			sp1kOsdIconShow(ID_ICON_BATTERY_LOW,ICON_SHOW);
			break;
		case POWER_BAT_EMPTY:
			sp1kOsdIconShow(ID_ICON_BATTERY_EMPTY,ICON_SHOW);
			break;
		case POWER_BAT_BELOWEMPTY:
			sp1kOsdIconShow(ID_ICON_BATTERY_EMPTY,ICON_SHOW);
			break;
		default:
			sp1kOsdIconShow(ID_ICON_BATTERY_FULL,ICON_SHOW);
			break;
	}

	return batlv;
}


void appStillGuiInit(void)
{
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	//MODE (msg)
	appTPIconReg(STILL_OBJ_0, ID_ICON_MODE_STILL,
		STILL_FUNC_NULL,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_KEY_PRESS_LEFT,0);
	//QUALITY (function)
	appTPIconReg(STILL_OBJ_1, ID_ICON_QUALITY,
		STILL_FUNC_QUALITY,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_NULL,0);
	//WB (function)
	appTPIconReg(STILL_OBJ_2, ID_ICON_WB_AUTO,
		STILL_FUNC_WB,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_NULL,0);
	//AE METERING (function)
	appTPIconReg(STILL_OBJ_3, ID_ICON_METERING_SPOT,
		STILL_FUNC_AEMETERING,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_NULL,0);
	//EV (function)
	appTPIconReg(STILL_OBJ_4, ID_ICON_EV_00,
		STILL_FUNC_EV,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_NULL,0);
	//IMG SIZE (function)
	appTPIconReg(STILL_OBJ_5, ID_ICON_RESOLUTION_5M,
		STILL_FUNC_IMGSIZE,FUNC_GUI_HDL_IDX_STILL, 0,SP1K_MSG_NULL,0);
#endif	
		
}

/////////////////////////////////////////////////////////
//snap模式的osd
/////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//appStillOsdInit
//-----------------------------------------------------------------------------
/**
 * @brief	the initial osd of snap pv mode
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void appStillOsdInit(UINT8 showNum)
{ 
	uiPara_t* puiPara ;
	puiPara = appUiParaGet();

	//appIconPsTabLoad(ICON_MODE_PV);
	showNum=showNum;
	
	//清除osd
	sp1kOsdDispBuffSw();
	sp1kOsdClear(0);
	
	//osd flash set
	sp1kOsdLocalFlashSet(3);
	appUiCoverTVOsd();

	//显示拍照图
	appOsdColorSet(OSD_COLOR_PINK);
	sp1kOsdIconShow(ID_ICON_MODE_STILL,ICON_SHOW);
	appOsdColorSet(OSD_COLOR_WHITE);

	//显示sd卡插入 2008-6-3
		uiCardIcon();

	//显示剩余张数
	//sp1kOsdStrDraw(osdW - 4, 0, "999", 0);//masked by wei.sun for rempix update

	//更新分辨率图标
	uiUpdateImgSize(LCD_WIDTH, 0, 0);

	//显示画质图标
	//uiUpdateImgQuality(LCD_WIDTH, 0, 0);

	//add for update rempix @wei.sun 20080311 begin
	if(appSDPlugStatusGet()==1)
		uiUpdateRemainPix();
	//add for update rempix @wei.sun 20080311 end

	//显示白平衡图标
	uiUpdateWB(LCD_WIDTH, 0, 0);

	//显示测光方式图标
	//uiUpdateAEMetering(LCD_WIDTH, 0, 0);

	if (!puiPara->DriverMode) {
		//显示驱动方式图标
		uiUpdateSnapMode(LCD_WIDTH, 0, 0);

		//显示effect图标
		uiUpdateEffect(LCD_WIDTH, 0, 0);
	} else {
		//显示effect图标
		uiUpdateEffect(LCD_WIDTH, 0, 0);

		//显示驱动方式图标
		uiUpdateSnapMode(LCD_WIDTH, 0, 0);
	}

	//EV icon 2008-6-3 add
	uiUpdateEV(LCD_WIDTH, 0, 0);

	//显示ISO图标
	uiIsoUpdate();
	
	//scene mode
	uiSceneModeUpdate();

	//显示电池电量图标
		uiUpdateBatteryLevel();
	//uiIconDisp(0, OsdMaxY- 1 , ID_ICON_BATTERY_FULL,0);
	

	#if (STILL_NEW_ZOOM == 0)
	if(puiPara->SpeedyCap == SPEEDY_CAPTURE_ON || puiPara->SpeedyCapRec == SPEEDY_CAPTURE_REC_ON){	
		uiUpdateOSDPvZoom(video_zoomFactor, 0);
	}else{
		uiUpdateOSDPvZoom(PvZoomCtrl.factor, 0);
	}
	#else
	uiUpdateOSDPvZoom(stillZFctor, 0);
	#endif
	
	uiFdModeUpdate();
	uiSmildeUpdate();
	uiAntiShakeUpdate();

	
	//显示闪光灯图标
	uiUpdateFlashLed(LCD_WIDTH, 0, 0);
	
	#if (SUPPORT_AF && SUPPORT_FIXED_AF)
	AF_uiAfIconShow();
	#endif

	appDateStampSet();

	#if SUPPORT_RTC_OSD_UPDATE
	appCurrentTimeShow(1);
	#endif
	
	appStillGuiInit();

	sp1kOsdDispBuffShow();
}
void appVideoGuiInit(void)
{
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	//MODE (msg)
	appTPIconReg(VIDEO_OBJ_0, ID_ICON_MODE_VIDEO,
		VIDEO_FUNC_NULL,FUNC_GUI_HDL_IDX_VIDEO, 0,SP1K_MSG_KEY_PRESS_MODE,0);
	//WB
	appTPIconReg(VIDEO_OBJ_1, ID_ICON_WB_AUTO,
		VIDEO_FUNC_WB,FUNC_GUI_HDL_IDX_VIDEO, 0,SP1K_MSG_NULL,0);
	//AE METERING
	appTPIconReg(VIDEO_OBJ_2, ID_ICON_METERING_CENTER,
		VIDEO_FUNC_AEMETERING,FUNC_GUI_HDL_IDX_VIDEO, 0,SP1K_MSG_NULL,0);
	//SIZE
	appTPIconReg(VIDEO_OBJ_3, ID_ICON_MOVIE_SIZE_HD,
		VIDEO_FUNC_VIDSIZE,FUNC_GUI_HDL_IDX_VIDEO, 0,SP1K_MSG_NULL,0);
#endif
}

/////////////////////////////////////////////
//video模式的osd
/////////////////////////////////////////////
//-----------------------------------------------------------------------------
//appVideoOsdInit
//-----------------------------------------------------------------------------
/**
 * @brief	the initial osd of video mode
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void appVideoOsdInit(void)
{
	UINT32 remainTime;
	UINT8 Xoffset = 0;
	UINT8 attr;
	UINT16 x,y;
	
	sp1kOsdDispBuffSw();

	if(appPanelTypeGet() == 0) {
		Xoffset = 1;
	}

	appIconPsTabLoad(ICON_MODE_VIDEO);

	//清除osd
	sp1kOsdClear(0);

	//osd flash set
	sp1kOsdLocalFlashSet(3);

	appUiCoverTVOsd();
	attr = 0x00;

	//显示video图
	appOsdColorSet(OSD_COLOR_PINK);
	sp1kOsdIconShow(ID_ICON_MODE_VIDEO,ICON_SHOW);
	appOsdColorSet(OSD_COLOR_WHITE);

	//显示sd卡插入 2008-6-3
	uiCardIcon();

	//显示剩余时间
	uiUpdateVideoRecRemainTime(&remainTime);
	sp1kOsdIconPsGet(ID_ICON_MODE_VIDEO,&x,&y);
	if(appSDPlugStatusGet()==0)
		remainTime = 0;
	uiUpdateOSDTime(LCD_WIDTH , y, remainTime, attr);

	//显示分辨率图标
	uiUpdateVideoSize(LCD_WIDTH, 0, attr);

	//显示闪光灯图标
	uiUpdateFlashLed(LCD_WIDTH, 0, 0);
	
	//册光方式
	//uiUpdateAEMetering(6+ Xoffset, 0, attr);

	//显示白平衡图标
	//uiUpdateWB(4+ Xoffset, 0, attr);

	//显示电池电量图标
	//uiIconDisp(0, OsdMaxY -1 , ID_ICON_BATTERY_FULL,0);
	uiUpdateBatteryLevel();

	#if (VIDEO_NEW_ZOOM == 0)
	uiUpdateOSDPvZoom(video_zoomFactor, 1);
	#endif

	#if (VIDEO_NEW_ZOOM == 1)
	uiUpdateOSDPvZoom(sp1kVideoZoomFctGet(), 1);
	#endif

	#if (SUPPORT_AF && SUPPORT_FIXED_AF)
		AF_uiAfIconShow();
	#endif

	#if SUPPORT_RTC_OSD_UPDATE
	appCurrentTimeShow(1);
	#endif
	
	appVideoGuiInit();

	//显示循环录像时间 
	uiCycleIcon();
	
	sp1kOsdDispBuffShow();

	//appFramRateSet();
}

/////////////////////////////////////////////
//audio模式的osd
/////////////////////////////////////////////
//-----------------------------------------------------------------------------
//appAudioOsdInit
//-----------------------------------------------------------------------------
/**
 * @brief	the initial osd of audio mode
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void appAudioOsdInit(void)
{
	UINT16 x,y,chrw,chrh;
	sp1kOsdDispBuffSw();
	sp1kOsdFontSizeGet(&chrw, &chrh);
	appIconPsTabLoad(ICON_MODE_AUDIO);
	//清除osd
	sp1kOsdClear(0);

	//osd flash set
	sp1kOsdLocalFlashSet(3);

	appUiCoverTVOsd();
	//显示audio图, 显示录制时间
	appOsdColorSet(OSD_COLOR_PINK);
	sp1kOsdIconShow(ID_ICON_MODE_VOICE,ICON_SHOW);
	appOsdColorSet(OSD_COLOR_WHITE);
	uiUpdateOSDTime(LCD_WIDTH/2+strWidthGet("44:44:44")/2, LCD_HEIGHT/2-1, 0, 0);

	//显示sd卡插入 2008-6-3
	if (sp1kAudioRecTypeGet() != AUDIO_TYPE_VOICE)//jintao.liu 2008-6-23 for mantis #26637
		uiCardIcon();

	if (sp1kAudioRecTypeGet() != AUDIO_TYPE_VOICE) {
		//显示剩余时间
		uiUpdateAudRecRemainTime(&AudRecRemainTime);/*取得剩余时间*/
		sp1kOsdIconPsGet(ID_ICON_MODE_VOICE,&x,&y);
		uiUpdateOSDTime(LCD_WIDTH, y, AudRecRemainTime, 0);		
	}

	//显示电池电量图标
	uiUpdateBatteryLevel();
	sp1kOsdDispBuffShow();
}


void uiRedEyeUpdate()
{
	uiPara_t* tempUiPara;
	return;
	tempUiPara = appUiParaGet();
	if(tempUiPara->redEyePb== REDEYEPB_OFF){
		sp1kOsdIconShow(ID_ICON_RED_EYE,ICON_CLEAR);	
	}else {
		sp1kOsdIconShow(ID_ICON_RED_EYE,ICON_SHOW);	
	}
}

void appPbGuiInit(void)
{
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	//MODE
	appTPIconReg(PB_OBJ_0, ID_ICON_MODE_PLAYBACK,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_MODE,0);
	//MENU
	appTPIconReg(PB_OBJ_1, ID_ICON_BUTTON_MENU,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_MENU,0);
	//LEFT
	appTPIconReg(PB_OBJ_2, ID_ICON_TBUTTON_LEFT,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
	//RIGHT	
	appTPIconReg(PB_OBJ_3, ID_ICON_TBUTTON_RIGHT,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_DOWN,0);
	//MULTI	
	appTPIconReg(PB_OBJ_4, ID_ICON_TBUTTON_PLAY9,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_WIDE,0);
	//SLIDE	
	appTPIconReg(PB_OBJ_5, ID_ICON_TBUTTON_SLIDESHOW,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_STATE_TO_SLIDE_SHOW,0);
	//PLAY	
	appTPIconReg(PB_OBJ_6, ID_ICON_MEDIA_RIGHT_ARROW,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_OK,0);

#endif
}


////////////////////////////////////////////////
//PB模式的总osd
////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//appPbOsdInit
//-----------------------------------------------------------------------------
/**
 * @brief	the initial osd of pb root mode
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-20
 * @todo
 * @bug
*/
void appPbOsdInit(void)
{
	UINT8 Xoffset = 0;

#if FUNC_HOST_DPS
	if(pbFile.bFileErr == TRUE){
		return;
	}
#endif	
	appIconPsTabLoad(ICON_MODE_PB);

	sp1kOsdDispBuffSw();
	if(appPanelTypeGet() == 0) {
		Xoffset = 1;
	}
	//清除zoom
	pbZoomCurLog=100;
	uiUpdateOSDPvZoom(pbZoomCurLog, 2);

	//清除osd
	sp1kOsdClear(0);

	if((appSDPlugStatusGet()==0) && (sp1kDiskTypeGet()==DEV_TYPE_DISK_SPI) && (sp1kDosRamStsGet()==FALSE))
	{
		sp1kDispImgWinEn(0);
		uiOsdDialogDisp(ID_STR_NO_CARD_, 0);
		return;
	}

	//osd flash set
	sp1kOsdLocalFlashSet(3);

	appUiCoverTVOsd();

	//显示PB图
	appOsdColorSet(OSD_COLOR_PINK);
	sp1kOsdIconShow(ID_ICON_MODE_PLAYBACK,ICON_SHOW);
	//appOsdColorSet(OSD_COLOR_WHITE);
	
	#if FUNC_HOST_TOUCH_PANEL
	sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW);

	

	if(appCurrStateGet() != APP_STATE_DPS)
	{
		sp1kOsdIconShow(ID_ICON_TBUTTON_LEFT,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_TBUTTON_RIGHT,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_TBUTTON_PLAY9,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_TBUTTON_SLIDESHOW,ICON_SHOW);
	}
	#endif

	//显示红眼
	
	appOsdColorSet(OSD_COLOR_RED);
	uiRedEyeUpdate();
	//显示pb文件的类型图标
	appOsdColorSet(OSD_COLOR_WHITE);
	uiUpdateOSDPbType(0+ Xoffset, LCD_HEIGHT, 5);

	//显示现时张数和总张数(pbFile.dwIndex/sp1kPbFileCntGet())
	uiUpdateOSDPbFileNum();


	//尝试取得lock消息，jintao.liu 2008-3-25

	if(pbFile.bImageProtect == TRUE)
	{
		//printf("LRYY : locked\n");
		sp1kOsdIconShow(ID_ICON_PROTECT,ICON_SHOW);
	}
	else
	{
		//printf("LRYY : no lock\n");
		sp1kOsdIconShow(ID_ICON_PROTECT,ICON_CLEAR);
		//sp1kOsdStrDraw(0+ Xoffset,1, "  ", 0);
	}

	if ((pbFile.bVoiceMemo == TRUE) && (pbFile.wType == SP1K_PB_FILE_TYPE_JPG)) {
		sp1kOsdIconShow(ID_ICON_VOICE_MEMO,ICON_SHOW);
	} else {
		//sp1kOsdStrDraw(0 + Xoffset, 2, "  ", 0);
		sp1kOsdIconShow(ID_ICON_VOICE_MEMO,ICON_CLEAR);
	}
	appPbGuiInit();
#if FUNC_HOST_DPS
	if(appCurrStateGet() == APP_STATE_DPS)
	{
		sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_CLEAR);

		uiUpdateOsdDps();
	}
#endif

	sp1kOsdDispBuffShow();
}

void appMultiGuiInit(void)
{
#if FUNC_HOST_TOUCH_PANEL
	multiPbSelWin_t *pMultiPos=NULL;
	UINT8 num;
	
	pMultiPos=sp1kPbMultiPosGet();

	appGuiObjIconClr(0,GUI_OBJ_MAX);
	//UP
	appTPIconReg(PB_OBJ_0, ID_ICON_TBUTTON_UP,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_TP_UP,0);
	//DOWN
	appTPIconReg(PB_OBJ_1, ID_ICON_TBUTTON_DOWN,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_TP_DOWN,0);
	//multi obj
	for(num=0;num<9;num++)
	{
		appTPRangeReg(PB_OBJ_2+num,pMultiPos->x0,pMultiPos->y0,
			pMultiPos->w,pMultiPos->h, STILLPB_FUNC_SELIMG,
			FUNC_GUI_HDL_IDX_STILLPB,num, SP1K_MSG_NULL,0);
			
		pMultiPos++;
	}
#endif
}
void appMultiOsdDirectionShow(void)
{
	#if FUNC_HOST_TOUCH_PANEL

	appOsdColorSet(OSD_COLOR_YELLOW);
	sp1kOsdIconShow(ID_ICON_TBUTTON_UP , ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_TBUTTON_DOWN , ICON_SHOW);
	appOsdColorSet(OSD_COLOR_WHITE);
	#endif
}



#if (FUNC_HOST_DPS == 1)
//-------------------------
//	uiUpdateOsdDps
//-------------------
extern UINT16 printTable[24][2];
extern UINT8 DpsImageChooseFlag;
void uiUpdateOsdDps(void)
{
	UINT8 str[128];
	UINT16 iconX,iconY;
	UINT8 i;
	//UINT8 Xoffset = 0;
	//UINT8 OsdMaxX, OsdMaxY;
	uiPara_t *uiPara;
	UINT8 printerStatus = appDpsPrinterStatusGet();
	uiPara = appUiParaGet();

	/*if(appPanelTypeGet() == 0) {
		//Xoffset = 1;
	}*/
	//sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	appIconPsTabLoad(ICON_MODE_DPS);
	if(pbFile.bFileErr == TRUE)
		return;

	switch(printerStatus)
	{
		case DPS_TO_CON_PRINT:

			strGet(str, 128, ID_STR_WAITING_FOR_PRINTER);
			//sp1kOsdStrDraw (0+Xoffset, ( OsdMaxY >> 1), str, 2);
			sp1kOsdStrDrawExt(2,110,str,0);
			break;
	     case DPS_PRINT_CON_OK: 	
			//appIconPsTabLoad(ICON_MODE_DPS);
			sp1kOsdIconShow(ID_ICON_BUTTON_OK,ICON_SHOW);
			
			appOsdColorSet(OSD_COLOR_GREEN);
			sp1kOsdIconShow(ID_ICON_PRINT,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUTTON_RIGHT,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUTTON_LEFT,ICON_SHOW);			

			if(DpsImageChooseFlag == 1){
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_DOWN,ICON_SHOW);
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&iconX,&iconY);
				for(i = 0;i < 24;i ++)
				{
					if(printTable[i][0] == pbFile.dwIndex){
						sprintf(str,"%02u",printTable[i][1]);
						str[2] = '\0';
						sp1kOsdIconShow(ID_ICON_DPSCHOOSE,ICON_SHOW);
						break;
					}else{	
						sprintf(str,"%02bu",uiPara->DPSCopies+1);
						str[2] = '\0';
					}
				}
				sp1kOsdStrDrawExt(iconX,iconY+28,str,0);		
			}
			appOsdColorSet(OSD_COLOR_WHITE);
			break;
		case DPS_PRINT_DOING:
			//appIconPsTabLoad(ICON_MODE_DPS);
			
			sp1kOsdIconShow(ID_ICON_BUTTON_OK_1,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW);
			//busy
			//uiIconDisp((OsdMaxX>>1)-2+Xoffset, (OsdMaxY>>1)+1 ,ID_ICON_BUSY_1,2);

			appOsdColorSet(OSD_COLOR_GREEN);
			sp1kOsdIconShow(ID_ICON_PRINT_1,ICON_SHOW);			
			sp1kOsdIconShow(ID_ICON_BUTTON_RIGHT_1,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUTTON_LEFT_1,ICON_SHOW);
			sp1kOsdIconShow(ID_ICON_BUSY_1,ICON_SHOW);
			
			osdIconPsGet(ID_ICON_BUTTON_MENU,&iconX,&iconY);
			strGet(str,128,ID_STR_CANCEL);
			sp1kOsdStrDrawExt(iconX + osdIconWidthGet(ID_ICON_BUTTON_MENU)+5,iconY,str,0);
			
			appOsdColorSet(OSD_COLOR_WHITE);
			break;
		case DPS_PRINT_CANCEL:
			 //sp1kOsdStrDraw (1+Xoffset, (OsdMaxY>>1)+1, "                        ", 0);    //clear connect print
		 	//sp1kOsdStrDraw ((OsdMaxX >> 1)-1+ Xoffset-4, OsdMaxY-1, "               ", 0);
			break;		 			 	
	}
}
  
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */
#endif

void appUiVideoStatusGuiInit(UINT8 mode, UINT8 status)
{
	mode=mode;
	status=status;
#if FUNC_HOST_TOUCH_PANEL

	appGuiObjIconClr(0,GUI_OBJ_MAX);

	if (mode == 0)
	{
		switch(status)
		{
			case VID_REC_RUN:
				appTPRangeReg(VIDEO_OBJ_0, 80, 60, 160, 120, 
					VIDEO_FUNC_NULL, FUNC_GUI_HDL_IDX_VIDEO, 0, SP1K_MSG_KEY_PRESS_S1,0);
				break;
			case VID_REC_PAUSE:
				break;
			default:
				break;			
		}
	}
	else
	{
		switch(status)
		{
			case VID_PB_RUN:
			case VID_PB_FW:
			case VID_PB_RW:
				//STOP
				appTPIconReg(PB_OBJ_0, ID_ICON_SIGNAL,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_F2,0);
				//PAUSE
				appTPIconReg(PB_OBJ_1, ID_ICON_MOVIE_PAUSE,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
				//REW
				appTPIconReg(PB_OBJ_2, ID_ICON_SYMBOL_REW,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_LEFT,0);
				//FF
				appTPIconReg(PB_OBJ_3, ID_ICON_SYMBOL_FF,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_RIGHT,0);
				break;
			case VID_PB_PAUSE:
				//STOP
				appTPIconReg(PB_OBJ_0, ID_ICON_SIGNAL,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_F2,0);
				//PLAY
				appTPIconReg(PB_OBJ_1, ID_ICON_MEDIA_RIGHT_ARROW,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
				break;
			default:
				break;
		}

	}
#endif
}


void uiVideoPbIconShow(UINT8 run)
{
	run=run;
#if FUNC_HOST_TOUCH_PANEL

	appOsdColorSet(OSD_COLOR_GREEN);
	if(run)
	{
		sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SYMBOL_REW,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SYMBOL_FF,ICON_SHOW);

	}
	else
	{
		sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SYMBOL_REW,ICON_CLEAR);
		sp1kOsdIconShow(ID_ICON_SYMBOL_FF,ICON_CLEAR);

	}
	appOsdColorSet(OSD_COLOR_WHITE);
#endif
}

//-----------------------------------------------------------------------------
//uiUpdateVideoStatus
//-----------------------------------------------------------------------------
/**
 * @brief	video status osd update
 * @param	mode: 0: video clip, 1: video play.
 * @retval	void
 * @see
 * @author	yong.sun
 * @since	2008-02-20
 * @todo
 * @bug
*/
void uiUpdateVideoStatus(UINT8 mode, UINT8 status)
{
	UINT8 str[128];
	UINT8 Xoffset = 0;
	UINT16 x,y;
	uiPara_t* puiPara = appUiParaGet();

	//sp1kOsdLocalFlashSet(3);
	if (mode == 0)
	{  /* video clip. */
		switch(status) 
		{
			case VID_REC_RUN:

				#if FUNC_HOST_TOUCH_PANEL
				sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_CLEAR);
				appOsdColorSet(OSD_COLOR_RED);
				sp1kOsdIconShow(ID_ICON_REC_REDBALL,ICON_SHOW);
				appOsdColorSet(OSD_COLOR_WHITE);
				#else
				//sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_CLEAR);
			
				//sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				//strGet(str, 128, ID_STR_REC);
				//sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);
				
				//sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				//strGet(str, 128, ID_STR_PAUSE);
				//sp1kOsdStrDrawExt(sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+x, y, str, 0);
				#endif
				break;
			case VID_REC_PAUSE:
			#if FUNC_HOST_TOUCH_PANEL
				sp1kOsdIconShow(ID_ICON_REC_REDBALL,ICON_CLEAR);
			#endif
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_CLEAR);
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				strGet(str, 128, ID_STR_PAUSE);
				sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);

				sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				strGet(str, 128, ID_STR_REC);
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				sp1kOsdStrDrawExt(sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+x, y, str, 0);
				break;
			default:
			#if FUNC_HOST_TOUCH_PANEL
				sp1kOsdIconShow(ID_ICON_REC_REDBALL,ICON_CLEAR);
			#endif
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				strGet(str, 128, ID_STR_PAUSE);
				sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);
				break;
		}


		
	}
	else if (mode ==1)
	{  /* video play. */
		switch(status) 
		{
			case VID_PB_RUN:
				appHdmiOsdSet(1);

				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(1);				
				#else
				sp1kOsdIconShow(ID_ICON_PLAY_STS_FF,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_BUTTON_MODE/*ID_ICON_BUTTON_MENU*/,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_LEFT,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SYMBOL_REW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_SNAP,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_RIGHT,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SYMBOL_FF,ICON_SHOW);

				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN_L,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN_H,ICON_CLEAR);
				#endif
				
				break;
			case VID_PB_PAUSE:
				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(0);
				#else
				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				#endif
			
				appHdmiOsdSet(1);
				break;
			case VID_PB_FW:
				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(1);
				#else
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_FF,ICON_SHOW);
				#endif
				appHdmiOsdSet(1);
				break;
			case VID_PB_RW:
				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(1);
				#else
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_REW,ICON_SHOW);	
				#endif
				appHdmiOsdSet(1);	
				break;
			default:
				#if FUNC_HOST_TOUCH_PANEL
				sp1kOsdIconPsGet(ID_ICON_SIGNAL,&x,&y);
				#else
				sp1kOsdIconPsGet(ID_ICON_BUTTON_MODE/*ID_ICON_BUTTON_MENU*/,&x,&y);
				#endif
				sp1kOsdClearCharExt(x,y, 320,MENU_CHAR_HEIGHT);
				break;
		}
	}

	appUiVideoStatusGuiInit(mode, status);
	
}

void appUiAudioStatusGuiInit(UINT8 mode, UINT8 status)
{
	mode=mode;
	status=status;
#if FUNC_HOST_TOUCH_PANEL
	if (mode == 0)
	{  /* audio Rec */
		switch(status) 
		{
			case AUD_REC_RUN:
				break;
			case AUD_REC_PAUSE:
				break;
			default:
				break;
		}

	}
	else if (mode ==1) 
	{  /* audio play. */
		switch(status) 
		{
			case AUD_PB_RUN:
			case AUD_PB_FW:
			case AUD_PB_RW:
				//STOP
				appTPIconReg(PB_OBJ_0, ID_ICON_SIGNAL,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_F2,0);
				//PAUSE
				appTPIconReg(PB_OBJ_1, ID_ICON_MOVIE_PAUSE,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
				//REW
				appTPIconReg(PB_OBJ_2, ID_ICON_SYMBOL_REW,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_LEFT,0);
				//FF
				appTPIconReg(PB_OBJ_3, ID_ICON_SYMBOL_FF,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_RIGHT,0);
				break;
			case AUD_PB_PAUSE:
				//STOP
				appTPIconReg(PB_OBJ_0, ID_ICON_SIGNAL,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_F2,0);
				//PLAY
				appTPIconReg(PB_OBJ_1, ID_ICON_MEDIA_RIGHT_ARROW,
					STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
				break;
			default:
				break;
		}
	}
#endif
}

//-----------------------------------------------------------------------------
//uiUpdateAudioStatus
//-----------------------------------------------------------------------------
/**
 * @brief	audio status osd update
 * @param	mode: 0: audio clip, 1: audio play.
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-04-22
 * @todo
 * @bug
*/
void uiUpdateAudioStatus(UINT8 mode, UINT8 status)
{
	UINT8 str[128];
	UINT8 Xoffset = 0;
	UINT16 x,y;
	uiPara_t* puiPara = appUiParaGet();

	if(appPanelTypeGet() == 0) {
		Xoffset = 1;
	}

	//sp1kOsdLocalFlashSet(3);
	if (mode == 0) {  /* audio clip. */
		switch(status) {
			case AUD_REC_RUN:
				sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_CLEAR);
			
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				strGet(str, 128, ID_STR_REC);
				sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);
				
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				strGet(str, 128, ID_STR_PAUSE);
				sp1kOsdStrDrawExt(sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+x, y, str, 0);
				break;
			case AUD_REC_PAUSE:
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_CLEAR);
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				strGet(str, 128, ID_STR_PAUSE);
				sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);

				sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				strGet(str, 128, ID_STR_REC);
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				sp1kOsdStrDrawExt(sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+x, y, str, 0);
				break;
			default:
				sp1kOsdIconPsGet(ID_ICON_BUTTON_UP,&x,&y);
				strGet(str, 128, ID_STR_PAUSE);
				sp1kOsdClearCharExt(x,y,sp1kOsdIconWidthGet(ID_ICON_BUTTON_UP)+strWidthGet(str),MENU_CHAR_HEIGHT);
				break;
		}

	}
	else if (mode ==1)
	{  /* audio play. */
		switch(status) 
		{
			case AUD_PB_RUN:
				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(1);
				#else
				sp1kOsdIconShow(ID_ICON_PLAY_STS_FF,ICON_CLEAR);
				sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_LEFT,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SYMBOL_REW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_RIGHT,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SYMBOL_FF,ICON_SHOW);

				#endif
				appHdmiOsdSet(1);

				break;
			case AUD_PB_PAUSE:
				/*
				sp1kOsdIconPsGet(ID_ICON_BUTTON_MENU,&x,&y);
				sp1kOsdClearCharExt(x,y, 320,y);

				sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);				
				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				*/
				#if FUNC_HOST_TOUCH_PANEL
				uiVideoPbIconShow(0);
				#else
				sp1kOsdIconPsGet(ID_ICON_BUTTON_MENU,&x,&y);
				sp1kOsdClearCharExt(x,y, 320,y);

				sp1kOsdIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);				
				sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				#endif
				appHdmiOsdSet(1);
				break;
			#if FUNC_HOST_TOUCH_PANEL
			case AUD_PB_FW:
			case AUD_PB_RW:
				uiVideoPbIconShow(1);
				appHdmiOsdSet(1);
				break;
			#else
			case AUD_PB_FW:
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_FF,ICON_SHOW);
				appHdmiOsdSet(1);
				break;
			case AUD_PB_RW:
				sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
				sp1kOsdIconShow(ID_ICON_PLAY_STS_REW,ICON_SHOW);		
				appHdmiOsdSet(1);
				break;
			#endif
			default:
				sp1kOsdIconPsGet(ID_ICON_BUTTON_MENU,&x,&y);
				sp1kOsdClearCharExt(x,y, 320,MENU_CHAR_HEIGHT);
				appHdmiOsdSet(1);
				break;
		}
	}
	appUiAudioStatusGuiInit(mode,status);
}

////////////////////////////////////////////////
//PB video模式的osd
////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//appPbVideoOsdInit
//-----------------------------------------------------------------------------
/**
 * @brief	the initial osd of pb video mode
 * @param
 * @retval	void
 * @see
 * @author
 * @since	2008-02-20
 * @todo
 * @bug
*/
void appPbVideoOsdInit(void)
{
	//清除osd
	sp1kOsdClear(0);

	appUiCoverTVOsd();

	//显示video pb的功能按键
	//sp1kOsdStrDraw((osdW>>1)-10, osdH-1, "M ", 2);
#if FUNC_HOST_TOUCH_PANEL

#else
	sp1kOsdIconShow(ID_ICON_BUTTON_MODE/*ID_ICON_BUTTON_MENU*/,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_SIGNAL,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_BUTTON_LEFT,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_SYMBOL_REW,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_BUTTON_RIGHT,ICON_SHOW);
	sp1kOsdIconShow(ID_ICON_SYMBOL_FF,ICON_SHOW);

	//appOsdColorSet(OSD_COLOR_BLUE);
	//sp1kOsdIconShow(ID_ICON_PLAY_STS_RUN,ICON_SHOW);
	//appOsdColorSet(OSD_COLOR_WHITE);
#endif	
}

//-----------------------------------------------------------------------------
//uiOsdDialogDisp
//-----------------------------------------------------------------------------
/**
 * @brief	disp message dialog
 * @param
 * @retval	void
 * @see
 * @author	jintao.liu
 * @since	2008-02-26
 * @todo
 * @bug
*/
void uiOsdDialogDisp(UINT8 stridx, UINT16 wait)
{
	UINT8	str[128];
	UINT8	lgh;
	UINT16 x,y;
	UINT16 strWidth,iconWidth,iconHeight;
	
	sp1kBtnDisableAll();
	lgh = strGet(str, 128, stridx);
	
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);//clear select window
	
	sp1kOsdClearCharExt(55, 57, 198, 114);
	afbackfaruiclearflag = 1;
	
	//sp1kOsdClear(0);//clear osd & half tone
	sp1kOsdIconPsGet(ID_ICON_WARNING,&x,&y);
	strWidth = strWidthGet(str);
	iconWidth = sp1kOsdIconWidthGet(ID_ICON_WARNING);
	iconHeight = sp1kOsdIconHeightGet(ID_ICON_WARNING);	
	
	sp1kOsdSelWinSet(x+iconWidth/2 -LCD_WIDTH/3,y-iconHeight,LCD_WIDTH/3*2,iconHeight*4,4,4);
	sp1kOsdStrDrawExt(x-strWidth/2 +iconWidth/2 ,y+iconHeight+4,str,0);
	appOsdColorSet(OSD_COLOR_YELLOW);
	sp1kOsdIconShow(ID_ICON_WARNING,ICON_SHOW);
	#if FUNC_HOST_TOUCH_PANEL
	if(stridx == ID_STR_NO_FILE)
	{
		appIconPsTabLoad(ICON_MODE_PB);
		sp1kOsdIconShow(ID_ICON_MODE_PLAYBACK, ICON_SHOW);
		appGuiObjIconClr(0,GUI_OBJ_MAX);
		appTPIconReg(PB_OBJ_0, ID_ICON_MODE_PLAYBACK,
			STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_STATE_TO_STILL_VIEW,0);

	}
	#endif
	appOsdColorSet(7);

	if (wait) {
		sp1kHalWait(wait);
	}
	sp1kBtnEnableAll();

}

void uiOsdVerDisp(UINT8 stridx, UINT16 wait)
{
	UINT8	str[128];
	UINT8	lgh;
	UINT16 x,y;
	UINT16 strWidth,iconWidth,iconHeight;


	//sp1kBtnDisableAll();
	lgh = strGet(str, 128, stridx);

	if (!versionclearflag) {
		//sp1kHalWait(wait);
		sp1kOsdClearCharExt(43, 204, 277, 35);
		return;
	}
	
	sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);//clear select window
	//sp1kOsdClear(0);//clear osd & half tone
	sp1kOsdIconPsGet(ID_ICON_WARNING,&x,&y);
	strWidth = strWidthGet(str);
	iconWidth = sp1kOsdIconWidthGet(ID_ICON_WARNING);
	iconHeight = sp1kOsdIconHeightGet(ID_ICON_WARNING);	
	
	//sp1kOsdSelWinSet(x+iconWidth/2 -LCD_WIDTH/3,y-iconHeight,LCD_WIDTH/3*2,iconHeight*4,4,4);
	sp1kOsdStrDrawExt(43,y+iconHeight+4+80,str,0);

	appOsdColorSet(7);

	//sp1kBtnEnableAll();

}

//-----------------------------------------------------------------------------
//appUiBusyWait
//-----------------------------------------------------------------------------
/**
 * @brief	appUiBusyWait
 * @param
 * @retval	void
 * @see
 * @author	phil.lin
 * @since	2008-02-26
 * @todo
 * @bug
*/
void appUiBusyWait(UINT16 Time, UINT8 bFlash)
{
	//Disable all button include power key
#if (_HW_SOLUTION_ == _HW_EVB_)
	sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_GPIO, 0x0002, 0x00);
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_)
	sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0x00);
#elif (_HW_SOLUTION_ == _HW_RDK_A_)
	sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0x00);
	sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0x00);
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
		sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffff, 0x00);
		sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffff, 0x00);
		sp1kBtnCfg(KEY_TYPE_GPIO, 0xffff, 0x00);
	
#endif

	if (osMsgSeek(SP1K_MSG_KEY_PRESS_POWER) == TRUE) {
		osMsgFlush(MSG_CTRL_FLUSH_ALL);
		osMsgPost(SP1K_MSG_POWER_OFF);
	} else {
		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
	}

	PrevIE = IE;
	Time = Time;
//Mask by Phil for disable flash
//if (bFlash) {
	bFlash = bFlash;
	if (0) {
		//sp1kTimer1Config(Time, pFunc_osdBusyShow, NULL);
		//sp1kTimer1Start();
		//IE = 0x8a;
	} else {
		appUiBusyShow(0);
		IE = 0x82;
	}
}

//-----------------------------------------------------------------------------
//appUiBusyExit
//-----------------------------------------------------------------------------
/**
 * @brief	appUiBusyExit
 * @param
 * @retval	void
 * @see
 * @author	phil.lin
 * @since	2008-02-26
 * @todo
 * @bug
*/
void appUiBusyExit(void)
{
	IE = PrevIE;
	appUiBusyShow(1);
	sp1kTimer1Stop();
	//Enable all button
	sp1kBtnEnableAll();
}

//-----------------------------------------------------------------------------
//appUiBusyShow
//-----------------------------------------------------------------------------
/**
 * @brief	appUiBusyShow
 * @param
 * @retval	void
 * @see
 * @author	phil.lin
 * @since	2008-02-26
 * @todo
 * @bug
*/
void appUiBusyShow(UINT8 rst)
{
	static UINT8 code busyIcon[6] = { 	ID_ICON_BUSY_1,
									ID_ICON_BUSY_2,
									ID_ICON_BUSY_3,
									ID_ICON_BUSY_4,
									ID_ICON_BUSY_5,
									0
								};
	static UINT8 code* pIcon = &busyIcon[5];
	static UINT8 iconPosX, iconPosY;
	sp1kDispCapabDesc_t dispCap;
	UINT8 xpos;
	UINT8 ypos;
	UINT8 str[128];
	UINT8 lgh;
	UINT8 attr;
	UINT8 OsdMaxX, OsdMaxY;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	if (rst) {
		pIcon = &busyIcon[5];
		return;
	}
	attr = OSD_ATTR_COLOR_WHITE | OSD_ATTR_MODE_HALFTONE | OSD_ATTR_BACK_EN;
	if (!(*pIcon)) {
		//printf(">\n");
		//first enter function
		sp1kDispCapabilityGet(sp1kDispDevActGet(), &dispCap);
		lgh = strGet(str, sizeof(str), ID_STR_WAITING___);

		if (lgh > OsdMaxX)
			lgh = OsdMaxX;

		xpos = (OsdMaxX - lgh) >> 1;
		iconPosX = OsdMaxX - xpos;
		ypos = OsdMaxY >> 1;
		iconPosY = ypos;

		sp1kOsdSelWinSet(-1, -1, -1, -1, 0, -1);//clear select window
		sp1kOsdClear(0);//clear osd & half tone
		sp1kOsdStrDraw (xpos, ypos, str, attr);

		pIcon--;
	}

	if (!(*(++pIcon))) {
		pIcon = &busyIcon[0];
	}
	//uiIconDisp(iconPosX, iconPosY, (UINT16)*pIcon, attr);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kOsdLoad                                            *
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
void sp1kOsdLoad(void)
{
	UINT16 posIcon = 0x5a;
	uiPara_t* puiPara = appUiParaGet();
	osdMem_t* pMem;
	osdMemCbGet(&pMem);
	sp1kCodeSentrySet(0);
	if ((FONT_COPY_NUM == 2) && (appPanelTypeGet() == 1)) // Small icon and font  @ panel is LCD
	{
		sp1kNandRsvRead(posIcon+1, (pMem->icon.addr << 1));//icon
		sp1kNandRsvRead(posIcon+1+2, (pMem->char8.addr << 1));//english
		sp1kNandRsvRead(posIcon+4+puiPara->Language, (UINT32)13 * (UINT32)0x8000);
		sp1kNandRsvRead(posIcon+4+48+puiPara->Language, (pMem->char16.addr << 1));
	}
	else // Large icon and font
	{
		sp1kNandRsvRead(posIcon, (pMem->icon.addr << 1));//icon
		sp1kNandRsvRead(posIcon+2, (pMem->char8.addr << 1));//english
		sp1kNandRsvRead(posIcon+4+puiPara->Language, (UINT32)13 * (UINT32)0x8000);
		sp1kNandRsvRead(posIcon+4+24+puiPara->Language, (pMem->char16.addr << 1));
	}
	sp1kCodeSentrySet(1);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kOsdLoad                                            *
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
void appIconPsTabLoad(UINT8 mode )
{
	UINT16 charW,charH;
	UINT16 idx=0x50;
	UINT32 addr=K_SDRAM_IconLayoutBufAddr;//TBD
	
	if(mode >= ICON_MODE_MAX)return;

	sp1kOsdFontSizeGet(&charW, &charH);
	if(charH>=28){
		idx=0x50;
	}else if(charH>=22){
		idx=0xa6;
	}else{
		idx=0xb0;
	}
	if((dispDevActGet()!=1)&&(charH>=22)){
		idx += ICON_MODE_MAX;
	}
	idx += mode;
	osdIconPsTabAddrSet(addr);
	sp1kNandRsvRead(idx, addr<<1);

}

void appUiCoverTVOsd(void)
{
	/*UINT8 k;
	UINT8 OsdMaxX, OsdMaxY;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	if(appPanelTypeGet() == 0) {
		for(k =0;k < OsdMaxY +1;k++) {
			sp1kOsdCharDraw(0 ,k, 0x0c, 0x20);
		}
	}*/
}

extern UINT8  G_SDCardPlug;
void uiCardIcon(void)
{
	if (0 == G_SDCardPlug){
		//appOsdColorSet(OSD_COLOR_PINK);
		//sp1kOsdIconShow(ID_ICON_STORAGE_MEDIA,ICON_SHOW);
		appOsdColorSet(OSD_COLOR_WHITE);
		sp1kOsdIconShow(ID_ICON_TF,ICON_SHOW);
	}else{
		sp1kOsdIconShow(ID_ICON_STORAGE_MEDIA,ICON_CLEAR);
		sp1kOsdIconShow(ID_ICON_TF,ICON_CLEAR);
	}

	
}

#ifndef FUNC_CLEAR
void uiUpdateWBVal(UINT8 quadrant, UINT8 whitepoint_cnt, UINT16 rgain_avg, UINT16 bgain_avg)
{
	UINT8 str[2];
	str[0] = ':'-0x20;
	str[1] = '\0';
	sp1kOsdNumberShow((UINT16)quadrant, 2, 5, 2, 0);
	sp1kOsdStrDraw(2 +2, 5, str, 0);
	sp1kOsdNumberShow((UINT16)whitepoint_cnt, 2 +3, 5, 3,0 );
	sp1kOsdStrDraw(2 +6, 5, str, 0);
	sp1kOsdNumberShow((UINT16)rgain_avg, 2+7, 5, 3, 0);
	sp1kOsdStrDraw(2 +10, 5, str, 0);
	sp1kOsdNumberShow((UINT16)bgain_avg, 2+11, 5, 3, 0);
}
#endif

/**
 * @fn        SelFrameInfo uiUpdateSelFrame(UINT8 value)
 * @brief     uiUpdateSelFrame
 * @param     [in] value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-12-13
 * @todo      N/A
 * @bug       N/A
*/
SelFrameInfo uiUpdateSelFrame(UINT8 value)
{	
	SelFrameInfo selFrame;
	
	value=value;
	return selFrame;
#if 0
	SelFrameInfo selFrame;
	UINT16 dispW,dispH;
	UINT16 xMax,xMin,yMax,yMin,wMax,wMin,hMax,hMin;
	
	UINT8 direct,xStep,yStep;
	UINT8 rotOpt;
	static UINT16 xPos = 150;
	static UINT16 yPos = 110;
	static UINT16 width = 40;
	static UINT16 heigth = 30;

	sp1kDispPnlDimGet(&dispH,&dispW);
	rotOpt=sp1kpbDecRotModeGet();

	wMax = dispW/2;
	hMax = wMax * 3 / 4;
	wMin = 20;
	hMin = 15;
	
	if((rotOpt = 0) || (rotOpt = 3))//img rot 0 or 180 
	{
		xMax = dispW - 1;
		xMin = 0;
		yMax = dispH - 1;
		yMin = 0;
	}
	else//img rot 90 or 270
	{
		xMax =(dispW + dispH * dispH / dispW) / 2-1; 
		xMin = (dispW - dispH * dispH / dispW) / 2-1;
		yMax = dispH - 1;
		yMax = 0;
	}

	direct = value & 0xE0;
	xStep = value & 0x1F; 
	yStep = xStep*3/4;

	switch(direct)
	{
		case SEL_FRAME_MOVE_UP:
			yPos = yPos - yStep;
			if((SINT16)yPos < (SINT16)yMin)
			{
				yPos = yMin;
			}
			break;
		case SEL_FRAME_MOVE_DOWN:
			yPos = yPos + yStep;
			if((yPos + heigth) > yMax)
			{
				yPos = yMax - heigth;
			}
			break;
		case SEL_FRAME_MOVE_LEFT:
			xPos = xPos - xStep;
			if((SINT16)xPos < (SINT16)xMin)
			{
				xPos = xMin;
			}
			break;
		case SEL_FRAME_MOVE_RIGHT:
			xPos = xPos + xStep;
			if((xPos +width)> xMax)
			{
				xPos = xMax-width;
			}
			break;
		case SEL_FRAME_ZOOM_IN:
			width += xStep * 2;
			if(width > wMax)
			{
				width = wMax;
				heigth = hMax;
			}
			
			heigth += yStep * 2;
			if(heigth > hMax)
			{
				width = wMax;
				heigth = hMax;
			}
			
			if(width != wMax)
			{
				yPos = yPos - yStep;
				xPos = xPos - xStep;
			}
			
			if((SINT16)yPos < (SINT16)yMin)
			{
				yPos = yMin;
			}else if(yPos > (yMax - heigth))
			{
				yPos = yMax - heigth;
			}
			
			if((SINT16)xPos < (SINT16)xMin)
			{
				xPos = xMin;
			}else if(xPos > (xMax - width))
			{

				xPos = xMax - width;
			}
			break;
		case SEL_FRAME_ZOOM_OUT:
			width -= xStep * 2;
			if(width < wMin)
			{
				width = wMin;
				heigth = hMin;
				break;
			}
			heigth -= yStep * 2;
			if(heigth < hMin)
			{
				width = wMin;
				heigth = hMin;
				break;
			}

			yPos = yPos + yStep;
			if((yPos + heigth) > yMax)
			{
				yPos = yMax - heigth;
			}

			xPos = xPos + xStep;
			if((xPos +width)> xMax)
			{
				xPos = xMax-width;
			}
			break;
		default:
			break;
	}

	selFrame.xpos = xPos;
	selFrame.ypos = yPos;
	selFrame.width = width;
	selFrame.heigth = heigth;
	
	sp1kOsdDispBuffSw();
	appPbOsdInit();
	sp1kOsdSelWinSet(xPos,yPos,width,heigth,2,OSD_ATTR_COLOR_RED);
	sp1kOsdDispBuffShow();
	return selFrame;
#endif
}

/**
 * @fn        UINT8 appVideoDateUpdate(UINT8 time)
 * @brief     appVideoDateUpdate
 * @param     NONE
 * @retval    TRUE:date change,need update;
 *                FALSE: NO need update
 * @see       NULL
 * @author    juanjuan.yuan
 * @since     2012-7-27
 * @todo      N/A
 * @bug       N/A
*/

UINT8 appVideoDateUpdate(UINT8 time)
{
	static UINT8 firstDate=0;	
	static video_osd_text_t* vidOldDate ;
	video_osd_text_t* vidNewDate ;
	vidNewDate = sp1kVideoRecOsdTextGet(STAMP_DATE);
	if(!time){
		firstDate=0;
	}
	if(!firstDate){
		vidOldDate = vidNewDate;
		firstDate=1;
	}
	if(strncmp(vidOldDate->text, vidNewDate->text, vidNewDate->width) != 0)
	{
		vidOldDate=vidNewDate;
		return TRUE;
	}
	else{
		return FALSE;
	}
}

/**
 * @fn        void appCurrentTimeShow(void)
 * @brief     appCurrentTimeShow
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-12-30
 * @todo      N/A
 * @bug       N/A
*/
#if 1
void  appCurrentTimeShow(UINT8 time)
{
#if 0
	video_osd_text_t* vidDate = sp1kVideoRecOsdTextGet(STAMP_DATE);
	video_osd_text_t* vidTime = vidDate+1;
	//UINT16 xoff,yoff;
	UINT16 dispW=320;
	
	appOsdColorSet(OSD_COLOR_YELLOW);
	if((appVideoDateUpdate(time)==TRUE)||(time==0)){//jj modify for only update date when it changes
		sp1kOsdClearCharExt(vidDate->xoff+vidDate->width-32,vidDate->yoff,64,MENU_CHAR_HEIGHT);
		sp1kOsdStrDrawExt(vidDate->xoff,vidDate->yoff,vidDate->text,0);
	}
	sp1kOsdClearCharExt(vidTime->xoff+vidTime->width-32,vidTime->yoff,64,MENU_CHAR_HEIGHT);
	if(stampTimeFmt==3)
	{
		sp1kOsdStrDrawExt(vidTime->xoff-20,vidTime->yoff,vidTime->text,0);//在datestamp各种模式长度不同，hh:mm:ss AM/PM在屏上显示不全
	}
	else
	{
		sp1kOsdStrDrawExt(vidTime->xoff,vidTime->yoff,vidTime->text,0);
	}	
	
	appOsdColorSet(MENU_STR_LOST_FOCUS);
#else
	dateStc_t dateInf;
	uiPara_t * pPara;
	UINT8 strDateTime[20] = {0};
	UINT16 dispW=320, TotalWidth;
	UINT16 DatePosX, DatePosY;

	time = time;

	sp1kHalReadRTC(&dateInf);
	pPara = appUiParaGet();

	switch(pPara->DateStyle)
	{
		case DATESTYLE_MMDDYYYY:
			sprintf(strDateTime, "%02bu/%02bu/20%02bu %02bu:%02bu:%02bu", dateInf.Month, dateInf.Day, dateInf.Year, dateInf.Hour, dateInf.Minute, dateInf.Second);
			break;
			
		case DATESTYLE_DDMMYYYY:
			sprintf(strDateTime, "%02bu/%02bu/20%02bu %02bu:%02bu:%02bu", dateInf.Day, dateInf.Month, dateInf.Year, dateInf.Hour, dateInf.Minute, dateInf.Second);
			break;
			
		case DATESTYLE_YYYYMMDD:
		default:
			sprintf(strDateTime, "20%02bu/%02bu/%02bu %02bu:%02bu:%02bu", dateInf.Year, dateInf.Month, dateInf.Day, dateInf.Hour, dateInf.Minute, dateInf.Second);
			break;
	}
	
	strDateTime[19] = '\0';
	
	TotalWidth = strWidthGet(strDateTime);
	DatePosX = (dispW - TotalWidth) / 2;
	DatePosY = 213;//213
	appOsdColorSet(OSD_COLOR_YELLOW);
	sp1kOsdClearCharExt(50, DatePosY, TotalWidth + 10, 32);
	if(pPara->StampMode != PRV_STAMP_OFF){
		sp1kOsdStrDrawExt(50, DatePosY, strDateTime, 0);
		}
	appOsdColorSet(MENU_STR_LOST_FOCUS);
#endif
}
#else
void appCurrentTimeShow(UINT8 Repaint)
{
	
	static dateStc_t prevDateTime = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	UINT8 strDate[] = "4444/44/44 ";
	UINT8 strHourMinute[] = "44:44:";
	UINT8 strSec[] = "44";


	dateStc_t dateInf;
	
	UINT16 xpos, ypos = 198;//213;//286

	UINT16 dateW, hourMinuteW, secW, totalW;

	uiPara_t * pPara;

	
	sp1kHalReadRTC(&dateInf);
	pPara = appUiParaGet();

	dateW = strWidthGet(strDate);
	hourMinuteW = strWidthGet(strHourMinute);
	secW = strWidthGet(strSec);
	totalW = dateW + hourMinuteW + secW;
	

	/* =============================================================================== date */
	/* YYYY/MM/DD */
	if ( prevDateTime.Year != dateInf.Year ||\
		 prevDateTime.Month != dateInf.Month ||\
		 prevDateTime.Day != dateInf.Day ||\
		 Repaint) 
	{
		xpos = ((LCD_WIDTH - totalW) >> 1)-32;
		
		sp1kOsdClearCharExt(xpos, ypos, dateW, 32);
		switch(pPara->DateStyle)
		{
			case DATESTYLE_MMDDYYYY:
				sprintf(strDate, "%02bu/%02bu/20%02bu ", dateInf.Month, dateInf.Day, dateInf.Year);
				break;
				
			case DATESTYLE_DDMMYYYY:
				sprintf(strDate, "%02bu/%02bu/20%02bu ", dateInf.Day, dateInf.Month, dateInf.Year);
				break;
				
			case DATESTYLE_YYYYMMDD:
			default:
				sprintf(strDate, "20%02bu/%02bu/%02bu ", dateInf.Year, dateInf.Month, dateInf.Day);
				break;
		}
		sp1kOsdStrDrawExt(xpos, ypos, strDate, 0);

		prevDateTime.Year = dateInf.Year;
		prevDateTime.Month = dateInf.Month;
		prevDateTime.Day = dateInf.Day;
	}

	

	/* =============================================================================== time */
	/* HH:MM: */
	if ( prevDateTime.Hour != dateInf.Hour ||\
		 prevDateTime.Minute != dateInf.Minute ||\
		 Repaint) 
	{
		sprintf(strDate, "20%02bu/%02bu/%02bu ", dateInf.Year, dateInf.Month, dateInf.Day);
		
		xpos = ((LCD_WIDTH - totalW) >> 1) + strWidthGet(strDate)-32;
		
		sp1kOsdClearCharExt(xpos, ypos, hourMinuteW, 32);
		sprintf(strHourMinute, "%02bu:%02bu:", dateInf.Hour, dateInf.Minute);
		sp1kOsdStrDrawExt(xpos, ypos, strHourMinute, 0);

		prevDateTime.Hour = dateInf.Hour;
		prevDateTime.Minute = dateInf.Minute;
	}

	/* SS */
	if ( prevDateTime.Second != dateInf.Second ||\
		 Repaint) 
	{
		sprintf(strDate, "20%02bu/%02bu/%02bu ", dateInf.Year, dateInf.Month, dateInf.Day);
		sprintf(strHourMinute, "%02bu:%02bu:", dateInf.Hour, dateInf.Minute);
		
		xpos = ((LCD_WIDTH - totalW) >> 1) + strWidthGet(strDate) + strWidthGet(strHourMinute)-32;
		
		sp1kOsdClearCharExt(xpos, ypos, secW, 32);
		sprintf(strSec, "%02bu", dateInf.Second);
		sp1kOsdStrDrawExt(xpos, ypos, strSec, 0);

		prevDateTime.Second = dateInf.Second;
	}
}
#endif


void appSlideGuiInit(UINT8 mode)
{
	mode=mode;
#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	
	if (0 == mode)
	{
		appTPIconReg(PB_OBJ_0, ID_ICON_MOVIE_PAUSE,
			STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
		appTPIconReg(PB_OBJ_1, ID_ICON_SLIDE_SIGNAL,
			STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_MENU,0);
	}
	else
	{
		appTPIconReg(PB_OBJ_0, ID_ICON_MEDIA_RIGHT_ARROW,
			STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
		appTPIconReg(PB_OBJ_1, ID_ICON_SLIDE_SIGNAL,
			STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_MENU,0);
	}
#endif


}
/********************************
* uiUpdateSlideShowState()
*
* mode=0 : in play status
* mode=1 : in stop status
* mode=2 : clear icon
*/
void uiUpdateSlideShowState(UINT8 mode)
{
	if (0 == mode)
	{
		#if FUNC_HOST_TOUCH_PANEL
		appOsdColorSet(OSD_COLOR_YELLOW);
		sp1kOsdIconShow(ID_ICON_MOVIE_PAUSE,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_SIGNAL,ICON_SHOW);
		appOsdColorSet(OSD_COLOR_WHITE);
		#else
		sp1kOsdIconShow(ID_ICON_SLIDE_BUTTON_UP,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_PLAY_PAUSE,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_CLEAR);
		sp1kOsdIconShow(ID_ICON_SLIDE_BUTTON_MENU,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_SIGNAL,ICON_SHOW);
		#endif
	}
	else if (1 == mode)
	{

		#if FUNC_HOST_TOUCH_PANEL
		appOsdColorSet(OSD_COLOR_YELLOW);
		sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_SIGNAL,ICON_SHOW);
		appOsdColorSet(OSD_COLOR_WHITE);
		#else
		sp1kOsdIconShow(ID_ICON_SLIDE_BUTTON_UP,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_RIGHT_ARROW,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_PLAY_STS_PAUSE,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_BUTTON_MENU,ICON_SHOW);
		sp1kOsdIconShow(ID_ICON_SLIDE_SIGNAL,ICON_SHOW);
		#endif
	}
	else
	{
		#if FUNC_HOST_TOUCH_PANEL

		#else
		sp1kOsdIconShow(ID_ICON_BUTTON_UP,ICON_CLEAR);
		#endif
	}
	
	appSlideGuiInit(mode);
	
}

void appVoiceMemoUI(void)
{
	UINT16 x,y;
	UINT8 str[16];
	appOsdColorSet(OSD_COLOR_GREEN);
	sp1kOsdIconShow(ID_ICON_REC_REDBALL,ICON_SHOW);
	sp1kOsdIconPsGet(ID_ICON_REC_REDBALL,&x,&y);
	strGet(str, 16, ID_STR_BACK);
	sp1kOsdStrDrawExt( x + sp1kOsdIconWidthGet(ID_ICON_REC_REDBALL),  y, str, 0);
	appOsdColorSet(OSD_COLOR_WHITE);
	#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	appTPRangeReg(PB_OBJ_0, x,y,100,	sp1kOsdIconHeightGet(ID_ICON_REC_REDBALL),
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_S2,0);
	#endif
}

#ifdef _ENABLE_LCD_OFF_
//Set OSD & LCD mode
//0   LCD on with OSD
//1   LCD on w/o OSD
//2   LCD off
void appOsdModeSet(UINT8 mode)
{
	if (UI_LCD_ON_WITH_OSD == mode)
	{
		osdMode = UI_LCD_ON_WITH_OSD;
		//sp1kDispPowerOn(sp1kDispDevActGet());
		sp1kHalCtrlScreen(3);
	}
	else if (UI_LCD_ON_WO_OSD == mode)
	{
		osdMode = UI_LCD_ON_WO_OSD;
		//sp1kDispPowerOn(sp1kDispDevActGet());
		sp1kHalCtrlScreen(1);
	}
	else
	{
		osdMode = UI_LCD_OFF;
		//sp1kDispPowerOff(sp1kDispDevActGet());
		sp1kHalCtrlScreen(0);
	}
}

//a ctrl function for OSD mode
//0   reset to LCD on with OSD
//1   switch to next OSD mode
//2   reenter current mode
void appOsdModeSw(UINT8 ctrl)
{
	if (0 == ctrl)	//Reset Mode
	{
		if (UI_LCD_ON_WO_OSD == osdMode)
		{
			sp1kHalCtrlScreen(3);
		}
		else if (UI_LCD_OFF == osdMode)
		{
			//sp1kDispPowerOn(sp1kDispDevActGet());
			sp1kHalCtrlScreen(3);
		}

		osdMode = UI_LCD_ON_WITH_OSD;
	}
	else if (1 == ctrl)			//next mode
	{
		osdMode++;
		if (osdMode == UI_OSD_MODE_MAX)
		{
			osdMode = UI_LCD_ON_WITH_OSD;
		}

		appOsdModeSet(osdMode);
	}
	else		//reenter current mode
	{
		appOsdModeSet(osdMode);
	}
}

/**
 * @fn UINT8 appOsdSelfSnapEnableOsd(UINT8 attrib)
 * @brief	enable osd anymore.
 * @param[in] attrib	0: show as osdMode config.
 *\n                    1: enable OSD.
 * @retval void
 */
void appOsdSelfSnapEnableOsd(UINT8 mode)
{
	if (1 == mode) {
		if ( osdMode == UI_LCD_ON_WO_OSD) {
			sp1kHalCtrlScreen(3);
			sp1kOsdClear(0);
		}
		else if ( osdMode == UI_LCD_OFF ) {
			sp1kHalCtrlScreen(2);
			sp1kOsdClear(0);
		}
	}
	else {
		appOsdModeSet(osdMode);
		//appStillOsdInit(0);
	}
}

UINT8 appGetOsdMode(void)
{
	return osdMode;
}

#endif /*_ENABLE_LCD_OFF_*/

void appPanShowGuiInit(void)
{
	#if FUNC_HOST_TOUCH_PANEL
	appGuiObjIconClr(0,GUI_OBJ_MAX);
	//UP (msg)
	appTPIconReg(STILL_OBJ_0, ID_ICON_MEDIA_UP_ARROW,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_UP,0);
	//DOWN (msg)
	appTPIconReg(STILL_OBJ_1, ID_ICON_MEDIA_DOWN_ARROW,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_DOWN,0);
	//LEFT (msg)
	appTPIconReg(STILL_OBJ_2, ID_ICON_MEDIA_LEFT_ARROW,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_LEFT,0);
	//RIGHT(msg)
	appTPIconReg(STILL_OBJ_3, ID_ICON_MEDIA_RIGHT_ARROW_1,
		STILLPB_FUNC_NULL,FUNC_GUI_HDL_IDX_STILLPB, 0,SP1K_MSG_KEY_PRESS_RIGHT,0);
	#endif
}

void appDirectionIconShow(UINT8 isEdge)
{
	appOsdColorSet(OSD_COLOR_GREEN);
	if(isEdge & 0x08)
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_DOWN_ARROW,ICON_CLEAR);
	}
	else
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_DOWN_ARROW,ICON_SHOW);
	}

	if(isEdge & 0x04)
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_UP_ARROW,ICON_CLEAR);
	}
	else
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_UP_ARROW,ICON_SHOW);
	}
	
	if(isEdge & 0x02)
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW_1,ICON_CLEAR);
	}
	else
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_RIGHT_ARROW_1,ICON_SHOW);
	}

	if(isEdge & 0x01)
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_LEFT_ARROW,ICON_CLEAR);
	}
	else
	{
		sp1kOsdIconShow(ID_ICON_MEDIA_LEFT_ARROW,ICON_SHOW);
	}
	appOsdColorSet(OSD_COLOR_WHITE);
	appPanShowGuiInit();
}


void appOsdDigitalZoomWinDraw(UINT16 ZoomFact)
{
	UINT16 bufferW,bufferH;
	UINT16 bgX,bgY,bgW,bgH;
	UINT16 foreX,foreY,foreW,foreH;
	UINT32 imgx0,imgx1,imgy0,imgy1;
	UINT8 aspectRatio;
	UINT16 imgW,imgH;
	sp1kOsdDispBuffSizeGet(&bufferW,&bufferH);
	sp1kHALDispImgXyGet(&imgx0,&imgy0,&imgx1,&imgy1);
	sp1kHALDispImgSizeGet(&imgW,&imgH);
	//printf("decodew=%d,%d\n",imgW,imgH);
	aspectRatio=appPbFileAspectRatioGet();
	switch(aspectRatio)
	{
		case PB_IMG_ORIENT_4X3:
			bgW = 100;
			bgH = 75;
			break;
		case PB_IMG_ORIENT_3X4:
			bgW = 75;
			bgH = 100;
			break;
		case PB_IMG_ORIENT_16X9:
			bgW = 96;
			bgH = 54;
			break;
		case PB_IMG_ORIENT_9X16:
			bgW = 54;
			bgH = 96;
			break;
		default:
			bgW = 100;
			bgH = 75;
			break;
	}

	bgX = LCD_WIDTH - bgW; // (bufferW>>2)*3 - (bgW>>1);
	bgY = LCD_HEIGHT - bgH; // (bufferH>>2)*3 - (bgH>>1);
	foreW =(bgW*100L + ZoomFact/2) / ZoomFact;
	foreH = (bgH*100L + ZoomFact/2) / ZoomFact;
	foreX = bgX + (imgx0*bgW + imgW/2) / imgW;
	foreY = bgY + (imgy0*bgH + imgH/2) / imgH;

	//printf("bgx=%d,y=%d,w=%d,h=%d\n",bgX,bgY,bgW,bgH);
	//printf("forex=%d,y=%d,w=%d,h=%d,factor=%d\n",foreX,foreY,foreW,foreH,ZoomFact);
	sp1kOsdIndexSave();
	sp1kOsdPenCfg(OSD_ATTR_MODE_HALFT1 | OSD_ATTR_COLOR_RED,1);
	sp1kOsdRectDraw(bgX,bgY,bgW,bgH);
	sp1kOsdPenCfg(OSD_ATTR_MODE_BLD| OSD_ATTR_COLOR_PINK,1);
	sp1kOsdRectDraw(foreX,foreY,foreW,foreH);
	sp1kOsdIndexRestore();
	
}

void appCrossLineDraw(UINT16 x,UINT16 y,UINT8 color,UINT16 lineW)
{
	color=color&0x3f;
	sp1kOsdIndexSave();
	sp1kOsdPenCfg(color | 0xC0, 2);
	sp1kOsdRectDraw(x-lineW/2,y,lineW+4,4);
	sp1kOsdRectDraw(x,y-lineW/2,4,lineW+4);
	sp1kOsdPenCfg(OSD_ATTR_COLOR_RED | 0xC0, 2);
	sp1kOsdRectDraw(40-2, 30-2, 4, 4);
	sp1kOsdRectDraw(280-2, 30-2, 4, 4);
	sp1kOsdRectDraw(280-2, 190-2, 4, 4);
	sp1kOsdRectDraw(40-2, 190-2, 4, 4);
	sp1kOsdIndexRestore();

}


#if SUPPORT_USB_ICON
void appChargingIconFlicker() {	
	static UINT8 ucDelayTime = 5;
	static UINT8 bFlag = 1;
	
	//if (--ucDelayTime == 0) {
	//	ucDelayTime = 5;
	//	bFlag = !bFlag;		
	//}
	appOsdColorSet(OSD_COLOR_GREEN);

	if (appCurrStateGet() != APP_STATE_POWER_ON && appCurrStateGet() != APP_STATE_MENU && appCurrStateGet() != APP_STATE_STILL_PLAY && appCurrStateGet() != APP_STATE_VIDEO_PLAY) {
		if (sp1kUSB_PlugGet()) {
			if (showChargingIcon) {
				#if SUPPORT_AUTO_TEST
				if (autoTestOpenFlag) {
					if (0 != autoTestStatus) {
						
						sp1kOsdIconShow(ID_ICON_BATTERY_CHARGE, bFlag ? ICON_SHOW : ICON_CLEAR);
					}
				} else {
					sp1kOsdIconShow(ID_ICON_BATTERY_CHARGE, bFlag ? ICON_SHOW : ICON_CLEAR);
				}
				#else
				sp1kOsdIconShow(ID_ICON_BATTERY_CHARGE, bFlag ? ICON_SHOW : ICON_CLEAR);
				#endif
			}
		} 
	}
	appOsdColorSet(OSD_COLOR_WHITE);
}
#endif

#if SUPPORT_AUTO_TEST
void appButtonAutoTest(void) {
	UINT16 bk_x, bk_y, bk_width, bk_height, title_x, title_y, next_title_x, next_title_y;
	UINT16 upKeyX, downKeyX, upDownY;
	UINT16 okKeyX, pwrKeyX, okPwrY;
	UINT16 usbX, usbY, pbKeyX, pbKeyY, setKeyX, setKeyY;
	UINT8 check_str[32] = {0}, yes_str[32] = {0};
	
	//UINT8 up_no_str[] = {"Up : NO"}, up_yes_str[] = {"Up : YES"}, down_no_str[] = {"Down : NO"}, down_yes_str[] = {"Down : YES"};
	//UINT8 ok_no_str[] = {"Ok : NO"}, ok_yes_str[] = {"Ok : YES"}, pwr_no_str[] = {"Pwr : NO"}, pwr_yes_str[] = {"Pwr : YES"};
	//UINT8 usb_no_str[] = {"usb : NO"}, usb_yes_str[] = {"usb : YES"}, pb_no_str[] = {"pb : NO"}, pb_yes_str[] = {"pb : YES"};
	//UINT8 set_no_str[] = {"set : NO"}, set_yes_str[] = {"set : YES"};

	bk_width = LCD_WIDTH - 30;
	bk_height = (LCD_HEIGHT / 7) * 5;
	bk_x = 5;
	bk_y = (LCD_HEIGHT - bk_height) >> 1;
	
	// title_x = bk_x;
	title_y = bk_y;

	next_title_x = bk_x;
	next_title_y = (bk_y + bk_height) - MENU_CHAR_HEIGHT;
	
	if (autoTestOpenFlag) {
		

		switch (autoTestStatus) {
			case 0:
				upKeyX = okKeyX = usbX = bk_x;
				downKeyX = pwrKeyX = ((bk_width + bk_x) >> 1);
				upDownY = bk_y + MENU_CHAR_HEIGHT;
				okPwrY = upDownY + MENU_CHAR_HEIGHT;
				usbY = okPwrY + MENU_CHAR_HEIGHT;
				pbKeyX = pwrKeyX;
				pbKeyY = usbY;
				setKeyX = usbX;
				setKeyY = usbY + MENU_CHAR_HEIGHT;
				
				if (upKeyCheck) {
					strGet(check_str, 32, ID_STR_UP_NO);
					sp1kOsdClearCharExt(upKeyX, upDownY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (downKeyCheck) {
					strGet(check_str, 32, ID_STR_DOWN_NO);
					sp1kOsdClearCharExt(downKeyX, upDownY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (okKeyCheck) {
					strGet(check_str, 32, ID_STR_OK_NO);
					sp1kOsdClearCharExt(okKeyX, okPwrY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (pwrKeyCheck) {
					strGet(check_str, 32, ID_STR_PWR_NO);
					sp1kOsdClearCharExt(pwrKeyX, okPwrY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (UsbCheck) {
					strGet(check_str, 32, ID_STR_USB_NO);
					sp1kOsdClearCharExt(usbX, usbY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (pbKeyCheck) {
					strGet(check_str, 32, ID_STR_PB_NO);
					sp1kOsdClearCharExt(pbKeyX, pbKeyY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				if (setKeyCheck) {
					strGet(check_str, 32, ID_STR_SET_NO);
					sp1kOsdClearCharExt(setKeyX, setKeyY, strWidthGet(check_str), MENU_CHAR_HEIGHT);
				}
				
				// 显示蓝色背景
				sp1kOsdIndexSave();
				sp1kOsdPenCfg(OSD_ATTR_MODE_BLD | OSD_ATTR_COLOR_BLUE, 4);
				sp1kOsdRectDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
				sp1kOsdIndexRestore();
				
				// 显示黄色测试标题
				title_x = (LCD_WIDTH - strWidthGet("Key And USB Test :")) >> 1;
				appOsdColorSet(OSD_COLOR_YELLOW);
				sp1kOsdStrDrawExt(title_x, title_y, "Key And USB Test :", 1);

				// 显示当前测试内容
				appOsdColorSet(upKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_UP_NO);
				strGet(yes_str, 32, ID_STR_UP_YES);
				sp1kOsdStrDrawExt(upKeyX, upDownY, upKeyCheck ? yes_str : check_str, 1);

				appOsdColorSet(downKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_DOWN_NO);
				strGet(yes_str, 32, ID_STR_DOWN_YES);
				sp1kOsdStrDrawExt(downKeyX, upDownY, downKeyCheck ? yes_str : check_str, 1);

				appOsdColorSet(okKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_OK_NO);
				strGet(yes_str, 32, ID_STR_OK_YES);
				sp1kOsdStrDrawExt(okKeyX, okPwrY, okKeyCheck ? yes_str : check_str, 1);

				appOsdColorSet(pwrKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_PWR_NO);
				strGet(yes_str, 32, ID_STR_PWR_YES);
				sp1kOsdStrDrawExt(pwrKeyX, okPwrY, pwrKeyCheck ? yes_str : check_str, 1);

				appOsdColorSet(UsbCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_USB_NO);
				strGet(yes_str, 32, ID_STR_USB_YES);
				sp1kOsdStrDrawExt(usbX, usbY, UsbCheck ? yes_str : check_str, 1);

				appOsdColorSet(pbKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_PB_NO);
				strGet(yes_str, 32, ID_STR_PB_YES);
				sp1kOsdStrDrawExt(pbKeyX, pbKeyY, pbKeyCheck ? yes_str : check_str, 1);

				appOsdColorSet(setKeyCheck ? OSD_COLOR_YELLOW : OSD_COLOR_RED);
				strGet(check_str, 32, ID_STR_SET_NO);
				strGet(yes_str, 32, ID_STR_SET_YES);
				sp1kOsdStrDrawExt(setKeyX, setKeyY, setKeyCheck ? yes_str : check_str, 1);

				if (upKeyCheck && downKeyCheck && okKeyCheck && pwrKeyCheck && UsbCheck && pbKeyCheck && setKeyCheck) {
					// 自动录像测试
					autoTestStatus = 1;
					sp1kHalWait(500);
					osMsgPost(SP1K_MSG_KEY_PRESS_S2);
					sp1kOsdClearCharExt(0, 0, LCD_WIDTH, LCD_HEIGHT);
				}
				break;

			case 1:
				sp1kOsdClearCharExt(bk_x, bk_y, bk_width, bk_height);
				
				// 显示黄色测试标题
				title_x = (LCD_WIDTH - strWidthGet("Video Record Test")) >> 1;
				appOsdColorSet(OSD_COLOR_YELLOW);
				sp1kOsdStrDrawExt(title_x, title_y, "Video Record Test", 1);
				break;

			case 2:
				// 显示黄色测试标题
				title_x = (LCD_WIDTH - strWidthGet("Video Playback Test")) >> 1;
				appOsdColorSet(OSD_COLOR_YELLOW);
				sp1kOsdStrDrawExt(title_x, title_y, "Video Playback Test", 1);
				break;

			case 3:
				autoTestOpenFlag = 0;
				
				// 显示黄色测试标题
				title_x = (LCD_WIDTH - strWidthGet("Test End")) >> 1;
				appOsdColorSet(OSD_COLOR_YELLOW);
				sp1kOsdStrDrawExt(title_x, title_y, "Test End", 1);
				break;
		}

		

		appOsdColorSet(OSD_COLOR_WHITE);
	}
}
#endif

#if SUPPORT_AF

#if SUPPORT_FIXED_AF
void AF_uiAfIconShow(void) {
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->AFMode)
	{
		case 0:
			sp1kOsdIconShow(ID_ICON_YUANJIAO,ICON_SHOW);	
			break;
		case 1:
			sp1kOsdIconShow(ID_ICON_JINJIAO,ICON_SHOW);	
			break;
		case 2:
			sp1kOsdIconShow(ID_ICON_JINJIAO,ICON_SHOW);	
			break;
		default:
			break;
	}

}
#endif


void AF_AfCalStatusShow(UINT8 status, UINT8 flag) {
	UINT16 xs, ys, width = 64, height = 64;

	xs = (LCD_WIDTH - width) >> 1;
	ys = (LCD_HEIGHT - height) >> 1;

	if (flag) {
		sp1kOsdSelWinSet(xs, ys, width, height, 2, (status ? OSD_ATTR_COLOR_LIGHT_GREEN : OSD_ATTR_COLOR_YELLOW));
	} else {
		sp1kOsdClearCharExt(xs, ys, width, height);
	}
	

	//sp1kOsdIndexSave();
	//sp1kOsdPenCfg(OSD_ATTR_MODE_HALFT1 | (status ? OSD_ATTR_COLOR_GREEN : OSD_ATTR_COLOR_RED), 1);
	//sp1kOsdRectDraw(xs, ys, width, height);
	//sp1kOsdIndexRestore();
}
#endif

void uiCycleIcon(void)
{
	uiPara_t* puiPara = appUiParaGet();

	appOsdColorSet(OSD_COLOR_CYCLE);
	switch( puiPara->Seamless)
	{
		case 0:
			sp1kOsdIconShow(ID_ICON_CYCLIC_REC_3MIN,ICON_CLEAR);
			break;
		case  1:	
			sp1kOsdIconShow(ID_ICON_CYCLIC_REC_3MIN,ICON_SHOW);
			break;
		default:
			break;
	}
	appOsdColorSet(OSD_COLOR_WHITE);
}

