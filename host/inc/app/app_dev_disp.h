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

#ifndef __APP_DEV_DISP_H__
#define __APP_DEV_DISP_H__

#define WHITE_LED

typedef enum panelStatus {
	PANEL_STATUS_LCD = 0x00,
	PANEL_STATUS_NTSC = 0x01,
	PANEL_STATUS_PAL = 0x02,
	PANEL_STATUS_RESET = 0xFF,
	PANEL_STATUS_TOTAL = 0x04,
} panelStatus_t;

void appTVOutDetect(void);//jintao.liu 2008-3-12 test
void appHDMIDetect(void);
void appPanelStatusGet(panelStatus_t* panelMode);
void appPanelStatusSet(panelStatus_t panelMode);
void appDispSet(void);
UINT8 appPanelTypeGet(void);
UINT8 appDispHdmiOpen(void);
void appDispOpen();
void appHdmiOsdSet(UINT8 on);
void appPreviewCdspDispResSet(void);
#ifdef WHITE_LED
void appWhiteLedCtrl(void);
#endif

void app_set_led(UINT8 falg);

void app_set_lcd_backlight_status(UINT8 status);
UINT8 app_get_lcd_backlight_status();
#if SUPPORT_PANEL_PROTECT
void appPanelAutoProt();
void app_set_close_lcd_backlight_time();
#endif

#if SUPPORT_PANEL_PROTECT
void appUpdateStatsLightStatus();
#endif

#if SUPPORT_SPI_WRITE_CONTROL
void appSetGpio6SpiWP(UINT8 falg);
#endif

#if SUPPORT_SPEAKER_CONTROL
void app_set_speaker(UINT8 falg);
#endif

#if SUPPORT_MCU_WTD
void appSetWTDHeartbeat(UINT8 falg);
#endif

void appFlashLedSet(UINT8 flag);
void appSnapFlashAdjustsAe(UINT8 flag);
void appSnapFlashLed(UINT8 flag);

//#if SUPPORT_SENSOR_SWITCH
void appSensorPWDNSwitch(UINT8 falg);
//#endif

#if SUPPORT_AF
void AF_Set(UINT16 pos);
UINT16 AF_GetPos(void);

#if SUPPORT_FIXED_AF
void AF_Switch(void);
#endif

#endif

void appAFCtrl(UINT8 flag);


#endif /*__APP_DEV_DISP_H__*/

